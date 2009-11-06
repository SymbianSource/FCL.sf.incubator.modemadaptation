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
#include "cmodematsession.h"
#include "modemattrace.h"
#include "catmessage.h"


CModemAtSession* CModemAtSession::NewL( CModemAtSrv& aServer,
    const TVersion& aVersion )
    {
    return new (ELeave) CModemAtSession( aServer, aVersion );
    }

CModemAtSession::~CModemAtSession()
    {
    C_TRACE (( _T(">>CModemAtSession::~CModemAtSession()") ));

    iServer.CancelFifoRequests( this );

    if( !iConnectReq.IsNull() )
        {
        C_TRACE(_L("iConnectReq canceled"));
        iConnectReq.Complete( KErrCancel );
        }
    if( !iUnsolicitedDataReq.IsNull() )
        {
        C_TRACE(_L("iUnsolicitedDataReq canceled"));
        iUnsolicitedDataReq.Complete( KErrCancel );
        }
    if( !iSignalIndReq.IsNull() )
        {
        C_TRACE(_L("iSignalIndReq canceled"));
        iSignalIndReq.Complete( KErrCancel );
        }
    if( !iCommandModeReq.IsNull() )
        {
        C_TRACE(_L("iCommandModeReq canceled"));
        iCommandModeReq.Complete( KErrCancel );
        }
    if(iClientName)
        {
        delete iClientName;
        iClientName = NULL;
        }
    iServer.ClientClosed(this);
    C_TRACE (( _T("<<CModemAtSession::~CModemAtSession()") ));
    }

void CModemAtSession::ServiceL( const RMessage2& aMessage )
    {
   C_TRACE (( _T("CModemAtSession::ServiceL()") ));
    if ( iVersion.iMajor==KServerMajorVersionNumber &&
         iVersion.iMinor==KServerMinorVersionNumber &&
         iVersion.iBuild==KServerBuildVersionNumber )
        {
        DoServiceL( aMessage );
        }
    else
        {
        C_TRACE (( _T("Complete KErrNotSupported") ));
        aMessage.Complete( KErrNotSupported );
        }
    }

TBool CModemAtSession::CheckAndCompleteExistingRequest( const RMessage2& aMessage )
    {
    C_TRACE (( _T("CModemAtSession::CheckAndCompleteExistingRequest()") ));
    TBool RequestAlreadyPending = EFalse;
    switch ( aMessage.Function() )
        {     
        case EATExtSetExtensionInterface:
            C_TRACE (( _T("EATExtSetExtensionInterface") ));
        break;
        case EModemAtRemovePipe:
            C_TRACE (( _T("EModemAtRemovePipe") ));
        break;
        case EModemATHandleCommand:
            C_TRACE (( _T("EModemATHandleCommand") ));
        break;
        case  EReceiveUnsolicitedResult:
            C_TRACE (( _T("EReceiveUnsolicitedResult") ));
          RequestAlreadyPending = !iUnsolicitedDataReq.IsNull(); 
        break;
        case EReceiveUnsolicitedResultCancel:
            C_TRACE (( _T("EReceiveUnsolicitedResultCancel") ));
            if ( iUnsolicitedDataReq.IsNull() )
                {
                C_TRACE (( _T("STRAY CANCEL") ));
                aMessage.Complete( KErrNone );
                return ETrue;
                }  
        break;
        case EModemATHandleCommandCancel:
            C_TRACE (( _T("EModemATHandleCommandCancel") ));
        break;
        case EModemATReceiveSignalInd:
            C_TRACE (( _T("EModemATReceiveSignalInd") ));
            RequestAlreadyPending = !iSignalIndReq.IsNull(); 
        break;
        case EModemATReceiveSignalIndCancel:
            C_TRACE (( _T("EModemATReceiveSignalIndCancel") ));
            if( iSignalIndReq.IsNull() )
                {
                C_TRACE (( _T("STRAY CANCEL") ));
                aMessage.Complete( KErrNone );
                return ETrue;
                }   
        break;
        
        case EModemATGetNvramStatus:            
            C_TRACE (( _T("EModemATGetNvramStatus") ));
        break;
        
        case EModemATCommandMode:
            C_TRACE (( _T("EModemATCommandMode") ));
            RequestAlreadyPending = !iCommandModeReq.IsNull(); 
            break;
            
        case EModemATCommandModeCancel:
            C_TRACE (( _T("EModemATCommandModeCancel") ));
            if( iCommandModeReq.IsNull())
                {
                C_TRACE (( _T("STRAY CANCEL") ));
                aMessage.Complete(KErrNone);
                return ETrue;
                }
            break;

        default:
            C_TRACE(_L("default"));
            _LIT( KPanicString, "AtServer");
            aMessage.Panic( KPanicString, KErrNotSupported );
            break;
        }
   
    if( RequestAlreadyPending ) 
        {
        C_TRACE (( _T("Complete KErrAlreadyExists") ));
        aMessage.Complete( KErrAlreadyExists );
        }
            
    C_TRACE (( _T("return %d"),RequestAlreadyPending ));
    return RequestAlreadyPending;
    }

void CModemAtSession::DoServiceL( const RMessage2& aMessage )
    {
    C_TRACE (( _T("CModemAtSession::DoServiceL() function: %d message: 0x%x handle: 0x%x"), aMessage.Function(), &aMessage, aMessage.Handle() ));

    if( CheckAndCompleteExistingRequest( aMessage ) )
        {
        C_TRACE (( _T("CModemAtSession::DoServiceL() - return") ));
        return;
        }

    switch ( aMessage.Function() )
        {
        case EATExtSetExtensionInterface:
            C_TRACE(_L("EATExtSetExtensionInterface"));
            iPluginType=(TATPluginInterface) aMessage.Int0();
            if(iClientName) 
               {
                delete iClientName;
                iClientName = NULL;
               }
            iClientName = HBufC8::New( aMessage.GetDesLength( KATModemResponseArgumentIndex ));
            iClientNamePtr.Set( iClientName->Des() );
            aMessage.Read( KATModemResponseArgumentIndex, iClientNamePtr );
            iConnectReq = aMessage;
            iServer.ConnectToModem(this);
            break;

        case  EReceiveUnsolicitedResult:
            C_TRACE(_L("EReceiveUnsolicitedResult"));
            iUnsolicitedDataReq = aMessage;         
            break;

        case EReceiveUnsolicitedResultCancel:
            C_TRACE(_L("EReceiveUnsolicitedResultCancel"));
            C_TRACE(_L("Complete"));
            iUnsolicitedDataReq.Complete(KErrCancel);    
            aMessage.Complete(KErrNone);
            break;

        case EModemATHandleCommandCancel:
            C_TRACE(_L("EModemATHandleCommandCancel"));
            // actual message is canceled from  fifo
            iServer.CancelFifoRequests( this );
            aMessage.Complete(KErrNone);
            break;

        case EModemATCommandModeCancel:
            C_TRACE (( _T("EModemATCommandModeCancel") ));
            iCommandModeReq.Complete(KErrCancel);
            aMessage.Complete(KErrNone);
            break;

        case EModemAtRemovePipe:
            C_TRACE (( _T("EModemAtRemovePipe") ));
            iServer.RemovePipe( iDteId );
            aMessage.Complete(KErrNone);
            break;

        case EModemATReceiveSignalIndCancel:
            C_TRACE(_L("EModemATReceiveSignalIndCancel"));
            C_TRACE(_L("Complete"));
            iSignalIndReq.Complete(KErrCancel);
            aMessage.Complete(KErrNone);
            break;
           
        case EModemATReceiveSignalInd:
            C_TRACE(_L("EModemATReceiveSignalInd"));
            iSignalIndReq = aMessage;           
            break;
            
        case EModemATHandleCommand:
            {
            C_TRACE(_L("EModemATHandleCommand"));
            CAtMessage* message = new CAtMessage( EAtCmd, this, aMessage );
            TRACE_ASSERT( message );
            C_TRACE(_L("iServer.AddToSendFifo EAtCmd"));
            iServer.AddToSendFifo( iDteId, iPluginType, message  );
            }
            break;
        
        case EModemATCommandMode:
            {
            C_TRACE(_L("EModemATCommandMode"));
            iCommandModeReq = aMessage;    
            }
            break;
        
        case EModemATGetNvramStatus:
            {
            C_TRACE(_L("EModemATGetNvramStatus"));
            CAtMessage* message = new CAtMessage( EGetNvramStatus, this, aMessage );
            TRACE_ASSERT( message );
            C_TRACE(_L("iServer.AddToSendFifo EGetNvramStatus"));
            iServer.AddToSendFifo( iDteId, iPluginType, message  );
            break;
            }
        default:
            {
            C_TRACE(_L("CModemAtSession::DoServiceL message not supported"));
            aMessage.Complete( KErrNotSupported );
            User::Panic(_L("ATServer"),KErrNotSupported);
            break;
            }
        }  
    }

CModemAtSession::CModemAtSession( CModemAtSrv& aServer,
    const TVersion& aVersion ) :
    iServer( aServer ),
    iVersion( aVersion ),
    iDteId( KInitialDteId ),
    iClientName( NULL ),
    iClientNamePtr( NULL, 0 )
    {
    C_TRACE (( _T("CModemAtSession::CModemAtSession()") ));
    }

void CModemAtSession::UnsolicitedData( const TDesC8& aData ) 
    {
    C_TRACE (( _T("CModemAtSession::UnsolicitedData()") ));
    if(!iUnsolicitedDataReq.IsNull())
        {
        iUnsolicitedDataReq.Write(KATModemUnsolicitedDataIndex, aData);
        C_TRACE (( _T("iUnsolicitedDataReq.Complete( KErrNone ) iUnsolicitedDataReq: 0x%x"), &iUnsolicitedDataReq ));
        iUnsolicitedDataReq.Complete( KErrNone );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    }

TUint8 CModemAtSession::GetDteId() 
    {
    C_TRACE (( _T("CModemAtSession::GetDteId()") ));
    return iDteId;    
    }


TATPluginInterface CModemAtSession::GetPluginType()
    {
    C_TRACE (( _T("CCModemAtSession::GetPluginType(%d)"),iPluginType ));
    return iPluginType;
    }


TDesC8& CModemAtSession::GetName() 
    {
    C_TRACE( _T("CModemAtSession::GetName()") ); 
    return *iClientName;
    }

void CModemAtSession::SetDteId( const TUint8 aDteId ) 
    {
    C_TRACE(( _T("CModemAtSession::SetDteId( %d )"), aDteId ));
    iDteId = aDteId;   
    }

void CModemAtSession::ModemConnected( const TInt aErr ) 
    {
    C_TRACE(( _T("CModemAtSession::ModemConnected( %d )"), aErr ));
    if( !iConnectReq.IsNull() ) 
        {
        C_TRACE (( _T("Complete iConnectReq( %d )"), aErr ));
        iConnectReq.Complete(aErr);
        }   
    }

void CModemAtSession::SignalIndReceived() 
    {
    C_TRACE(( _T("CModemAtSession::SignalIndReceived()") ));
    if(!iSignalIndReq.IsNull())
        {
        C_TRACE (( _T("Complete iSignalIndReq( KErrNone ) iSignalIndReq: 0x%x"), &iSignalIndReq ));
        iSignalIndReq.Complete( KErrNone );
        }
    }

void CModemAtSession::CommandModeChanged( TCommandMode aMode ) 
    {
    C_TRACE((_L("CModemAtSession::CommandModeChanged( %d )"), (TInt) aMode ));
    C_TRACE((_L("Writing mode to iCommandModeReq %d"), (TInt) aMode ));

    if( !iCommandModeReq.IsNull() )
        {
        TInt length = iCommandModeReq.GetDesMaxLength( KATModemCommandArgumentIndex );
        C_TRACE((_L("iCommandModeReq length: %d"), length ));

        HBufC8* modeBuffer = HBufC8::New( length );
        TPtr8 ptr = modeBuffer->Des();
        iCommandModeReq.Read( KATModemCommandArgumentIndex, ptr );
        modeBuffer->Des().AppendNum( (TInt) aMode );
        C_TRACE((_L("modeBuffer->Des() contents: 0x%x"), &ptr ));
        DUMP_MESSAGE( ptr );
        
        TInt err = iCommandModeReq.Write( KATModemCommandArgumentIndex, modeBuffer->Des() );
        delete modeBuffer;

        C_TRACE (( _T("Complete %d"),err ));
        iCommandModeReq.Complete( err );
        }
    else
        {
        C_TRACE((_L("CommandModeChanged Ignored!!! Mode: %d"), aMode ));
        }
    }

TBool CModemAtSession::IsUnsolicitedDataReqActive()
    {
    C_TRACE(( _T("CModemAtSession::IsUnsolicitedDataReqActive ret: %d"), (TInt) !iUnsolicitedDataReq.IsNull() ));
    return !iUnsolicitedDataReq.IsNull();
    }

TBool CModemAtSession::IsSignalIndReqActive()
    {
    C_TRACE(( _T("CModemAtSession::IsSignalIndReqActive ret: %d"), (TInt) !iSignalIndReq.IsNull() ));
    return !iSignalIndReq.IsNull();
    }

TBool CModemAtSession::IsCommandModeReqActive()
    {
    C_TRACE(( _T("CModemAtSession::IsCommandModeReqActive ret: %d"), (TInt) !iCommandModeReq.IsNull() ));
    return !iCommandModeReq.IsNull();
    }

TBool CModemAtSession::IsConnectReqActive()
    {
    C_TRACE(( _T("CModemAtSession::IsConnectReqActive ret: %d"), (TInt) !iConnectReq.IsNull() ));
    return !iConnectReq.IsNull();
    }


