/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: 
*
*/



#include "cmodematsrv.h"
#include "modematclientsrv.h"
#include "modematsecpolicy.h"
#include "cmodematsession.h"
#include "cmodemathandler.h"
#include "modemattrace.h"
#include "catmessage.h"


CModemAtSrv::CModemAtSrv( TInt aPriority ) :
    CPolicyServer( aPriority, KSrvPolicy ), iHandler(NULL)
    {
    }

CModemAtSrv::~CModemAtSrv()
    {
    C_TRACE (( _L(">>CModemAtSrv::~CModemAtSrv()") ));
    iAtMessageArray.ResetAndDestroy();
    iAtMessageArray.Close();
    C_TRACE (( _L("<<CModemAtSrv::~CModemAtSrv()") ));
    }

CModemAtSrv* CModemAtSrv::NewLC()
    {
    CModemAtSrv* self = new (ELeave) CModemAtSrv( EPriorityNormal );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->StartL( KATExtSrvName );
    RThread().SetPriority( EPriorityNormal );
    return self;
    }

void CModemAtSrv::RemovePipe( const TUint8 aDteId )
    {
    C_TRACE((_L("CModemAtSrv::RemovePipe()") ));
    iHandler->RemovePipe( aDteId );
    }

void CModemAtSrv::ClientClosed( CModemAtSession* aSession )
    {
    C_TRACE (( _L("CModemAtSrv::ClientClosed()") ));
    TInt index = iSessions.Find( aSession );
    if( index >= 0 )
        {
        //remove session from routing table, if dteId exists
        if(iSessions[index]->GetDteId() != KInitialDteId )
            {
            iRouteTable[ iSessions[index]->GetPluginType() ][ iSessions[index]->GetDteId() ] = NULL;
            }
        
        if( iSessions.Count() == 1 )
            {
            C_TRACE((_L("Disconnecting ISI message handler ") ));
            iHandler->Disconnect( iSessions[ index ]->GetDteId() );
            }
    
        C_TRACE((_L("Removing session (plugin type %d dteid: %d)"), iSessions[index]->GetPluginType(), iSessions[index]->GetDteId() ));
        iSessions.Remove( index );
        
        C_TRACE((_T("<<CModemAtSrv::ClientClosed()") ));
        }
    }
 
CSession2* CModemAtSrv::NewSessionL( const TVersion& aVersion,
                                   const RMessage2& /*aMessage*/ ) const
    {
    C_TRACE (( _T("CModemAtSrv::NewSessionL()") ));
    CModemAtSrv& thisServer = const_cast<CModemAtSrv&>( *this );
    CModemAtSession* session = CModemAtSession::NewL( thisServer, aVersion );
    CleanupStack::PushL( session );
    thisServer.iSessions.AppendL( session );
    CleanupStack::Pop( session );
    return session;
    }

void CModemAtSrv::ConstructL()
    {
    C_TRACE (( _T("CModemAtSrv::ConstructL()") ));
    iHandler = CModemAtHandler::NewL( *this );
    
     //setup routing table
    for( TInt i = 0 ; i < KPluginCount ; i++)
        {
        for(TInt o = 0; o < KMaxDteIdCount; o++)
            {
            iRouteTable[ i ][ o ] = NULL;
            }
        }
    } 

void CModemAtSrv::HandleSignalInd( const TInt aDteId )
    {
    C_TRACE ((_T("CModemAtSrv::HandleSignalInd aDteId = %d sessions = %d"), aDteId, iSessions.Count() ));
    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( iSessions[i]->GetDteId() == aDteId  && iSessions[i]->IsSignalIndReqActive() )
            {
            C_TRACE (( _T("CModemAtSrv::HandleSignalInd() session found") ));
            iSessions[i]->SignalIndReceived();
            }
        }
    }

void CModemAtSrv::HandleUnsolicitedData( const TInt aDteId, const TDesC8& aData )
    {
    C_TRACE ((_T("CModemAtSrv::HandleUnsolicitedData aDteId = %d sessions = %d"), aDteId, iSessions.Count() ));
    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( (iSessions[i]->GetDteId() == aDteId || aDteId == KUnsolicitedDataDteId )&&
            iSessions[i]->IsUnsolicitedDataReqActive() &&
            iSessions[i]->GetPluginType() == EATExtPlugin )
            {
            C_TRACE (( _T("CModemAtSrv::HandleUnsolicitedData() session found") ));
            iSessions[i]->UnsolicitedData( aData );
            }
        }
    }

void CModemAtSrv::HandleCommandModeChange( TInt aDteId, TCommandMode aMode )
    {
    C_TRACE ((_T("CModemAtSrv::HandleCommandModeChange aDteId = %d sessions = %d mode = %d"), aDteId, iSessions.Count(), (TInt) aMode ));
    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( iSessions[i]->GetDteId() == aDteId &&
            iSessions[i]->IsCommandModeReqActive() &&
            iSessions[i]->GetPluginType() == ECommonPlugin )
            {
            C_TRACE (( _T("CModemAtSrv::HandleCommandModeChange() session found") ));
            iSessions[i]->CommandModeChanged( aMode );
            }
        }
    }

void CModemAtSrv::BroadcastModemConnected(const TUint aDteId, TInt aErr )
    {
    C_TRACE ((_T("CModemAtSrv::BroadcastModemConnected aDteId = %d err = %d sessions = %d"), aDteId, aErr, iSessions.Count() ));
    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( iSessions[i]->GetDteId() == aDteId  && iSessions[i]->IsConnectReqActive() )
            {
            C_TRACE (( _T("CModemAtSrv::BroadcastModemConnected() session found") ));
            iSessions[i]->ModemConnected( aErr );
            }
        }
    }

void CModemAtSrv::HandleIntermediateDataInd( const TInt aDteId,
    const TATPluginInterface aPluginType,
    const TDesC8& aResponse,
    const TUint8 aCommand )
    {
    C_TRACE ((_T("CModemAtSrv::HandleIntermediateDataInd aDteId = %d sessions = %d "), aDteId, iSessions.Count() ));
    C_TRACE ((_T("plugintype = %d command = %d"), (TInt)aPluginType, aCommand ));
    
    if ( iAtMessageArray.Count() > 0 )
        {
        C_TRACE((_L("iAtMessageArray[0]->ReceiveATResponse Intermediate data")));
        iAtMessageArray[0]->ReceiveATResponse( aResponse, EFalse );
        }
    else
        {
        C_TRACE ((_T("CModemAtSrv::HandleIntermediateDataInd INTERMEDIATE DATA received but no request active") ));
        }
    }

void CModemAtSrv::HandleATResponse( const TInt aDteId, const TDesC8& aResponse, const TUint8 aCommand )
    {
    C_TRACE ((_T(">>CModemAtSrv::HandleATResponse aDteId = %d sessions = %d "), aDteId, iSessions.Count() ));
    C_TRACE ((_T("CModemAtSrv::HandleATResponse command = %d"), aCommand ));
    
    if ( iAtMessageArray.Count() > 0 )
        {
        CAtMessage* tmp = iAtMessageArray[0];
        C_TRACE((_L("iAtMessageArray[0]->ReceiveATResponse 0x%x"), tmp ));
        tmp->ReceiveATResponse( aResponse, ETrue );
        C_TRACE((_L("CModemAtSrv::HandleATResponse Remove the first message in Fifo")));
        RemoveFirstFromSendFifo();
        C_TRACE((_L("CModemAtSrv::HandleATResponse SendNextFromFifo")));
        SendNextFromFifo();
        }
    else
        {
        HandleUnsolicitedData( aDteId, aResponse);
        }
    C_TRACE ((_T("<<CModemAtSrv::HandleATResponse()") ));
    }

void CModemAtSrv::ConnectToModem(CModemAtSession* aSession) 
    {
    C_TRACE ((_T(">>CModemAtSrv::ConnectToModem 0x%x"), aSession));
    TInt type = aSession->GetPluginType();
   
    C_TRACE(( _L("session count: %d, type: %d"), iSessions.Count(), type ));
    //find already defined dteid
    for(TInt i = 0; i < iSessions.Count(); i++) 
        {
        TInt dteid = iSessions[i]->GetDteId();
        TDesC8& name = iSessions[i]->GetName();
        C_TRACE((_L("i: %d, dteid: %d"), i, dteid));
        
        //Is there plugin with samename
        if( name.Compare(aSession->GetName())== 0 && dteid != KInitialDteId )
            {
            if( type == iSessions[i]->GetPluginType())
                {
                TRACE_ASSERT_ALWAYS;
                aSession->SetDteId( dteid );
                aSession->ModemConnected((TInt)KErrAlreadyExists);
                C_TRACE ((_T("<<CModemAtSrv::ConnectToModem 0x%x"), aSession));
                return;
                }
            //add current session to route table
            C_TRACE((_L("AddSessionToRouteTable type: %d, dteid: %d"), type, aSession->GetDteId()));
            iRouteTable[aSession->GetPluginType()][dteid] = aSession;
            C_TRACE((_L("Interface exists=> %d"),dteid));
          
            aSession->SetDteId( dteid );
            aSession->ModemConnected(KErrNone); //connected
            C_TRACE ((_T("<<CModemAtSrv::ConnectToModem 0x%x"), aSession));
            return;
            }
        }
             
    //no AT-plugin& Common plugin, find first free dteid
    TInt dteId = 0;
    while(iRouteTable[0][dteId] || iRouteTable[1][dteId])
        {
        dteId++;
      }     
    ASSERT_PANIC_ALWAYS( dteId < KMaxDteIdCount );
    iRouteTable[type][dteId] = aSession;
    C_TRACE((_L("Added new dteid: %d"),dteId));

    aSession->SetDteId(dteId);
    iHandler->Connect(dteId);
    C_TRACE ((_T("<<CModemAtSrv::ConnectToModem 0x%x"), aSession));
    }

void CModemAtSrv::AddToSendFifo( const TUint8 aDteId,
    const TATPluginInterface aPluginType,
    CAtMessage* aMessage )
    {
    C_TRACE (( _T("CModemAtSrv::AddToSendFifo( dteId = %d,0x%x)"), aDteId, aMessage  ));

    iAtMessageArray.Append( aMessage ); // only one AT command at the time in modem

    if( iAtMessageArray.Count() == 1 ) //if empty Fifo send immediately
        {
        C_TRACE((_L("Sending message immediately")));
        iHandler->SendATCommand( aDteId, 
           aPluginType,
           aMessage->GetMessageType(),
           aMessage->GetBuffer() );
        }
    }

void CModemAtSrv::RemoveFirstFromSendFifo()
    {
    C_TRACE(( _T("CModemAtSrv::RemoveFirstFromSendFifo iAtMessageArray.Count = %d"), iAtMessageArray.Count() ));
    if( iAtMessageArray.Count( ) > 0 )
        {
        C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 1") ));
        CAtMessage* msg = iAtMessageArray[0];
        C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 2 0x%x"), msg ));
        iAtMessageArray.Remove(0);
        if( msg )
            {
            C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 3") ));
            delete msg;
            }
        else
            {
            TRACE_ASSERT_ALWAYS;
            }
        C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 4") ));
        }
    C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 5") ));
    }

void CModemAtSrv::SendNextFromFifo()
    {
    C_TRACE ((_T("CModemAtSrv::SendNextFromFifo fifo has %d message(s)"), iAtMessageArray.Count() ));
    
    TPtr8 ptr(0,0);
    if( iAtMessageArray.Count() > 0 )
        {
        C_TRACE((_L("Messages in buffer... Sending message")));
        ptr.Set( iAtMessageArray[0]->GetBuffer() );
        
        C_TRACE (( _T("iHandler->SendATCommand()") ));
        iHandler->SendATCommand( iAtMessageArray[0]->GetSession()->GetDteId(), 
           iAtMessageArray[0]->GetSession()->GetPluginType(),
           iAtMessageArray[0]->GetMessageType(),
           ptr );
        }
    }

void CModemAtSrv::CancelFifoRequests( const CModemAtSession* aSession )
    {
    C_TRACE((_T(">>CModemAtSrv::CancelFifoRequests count: %d"), iAtMessageArray.Count() ));
    TInt messageCount = iAtMessageArray.Count();
    for( TInt i = 0; i < messageCount; i++ )
        {
        C_TRACE((_T("CancelFifoRequests %d"), i ));
        CAtMessage* msg = iAtMessageArray[ i ];
        if( msg )
            {
            C_TRACE((_T("Found Request: 0x%x"),msg ));
            if( msg->GetSession() == aSession)
                {
                C_TRACE((_T("CancelFifoRequests - canceling request") ));
                msg->CancelRequest();
                C_TRACE (( _T("CModemAtSrv::CancelFifoRequest delete msg") ));
                delete msg;
                }
            }
        }
    C_TRACE (( _T("CModemAtSrv::CancelFifoRequest iAtMessageArray.Remove %d messages"), messageCount ));
    for( TInt j = 0; j < messageCount && iAtMessageArray.Count() > 0; j++ )
        {
        iAtMessageArray.Remove( 0 );
        }
    C_TRACE(( _T("<<CModemAtSrv::CancelFifoRequests()") ));
    }

TInt CModemAtSrv::SessionCount()
    {
    C_TRACE(( _T("CModemAtSrv::SessionCount() %d"), iSessions.Count() ));
    return iSessions.Count();
    }

