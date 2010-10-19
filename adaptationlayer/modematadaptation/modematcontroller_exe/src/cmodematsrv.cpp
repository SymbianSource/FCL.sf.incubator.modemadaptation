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

#include <pn_const.h>
#include <iscnokiadefinitions.h>
#include "cmodematsrv.h"
#include "modematclientsrv.h"
#include "modematsecpolicy.h"
#include "cmodematsession.h"
#include "cmodemathandler.h"
#include "modemattrace.h"
#include "catmessage.h"
#include "cmodematpipecontroller.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmodematsrvTraces.h"
#endif

_LIT8( KCursor, ">" );
_LIT8( KOkResponse, "OK" );
_LIT8( KErrorResponse, "ERROR" );
_LIT8( KOkResponseNoVerbose, "0" );
_LIT8( KErrorResponseNoVerbose, "4" );

_LIT8( KAtv, "ATV" );
_LIT8( KAtv1, "ATV1" );
_LIT8( KAtv0, "ATV0" );
_LIT8( KAtq, "ATQ" );
_LIT8( KAtq1, "ATQ1" );
_LIT8( KAtq0, "ATQ0" );
_LIT8( KAte, "ATE" );
_LIT8( KAte1, "ATE1" );
_LIT8( KAte0, "ATE0" );

_LIT8( KCmgs, "+CMGS" );
_LIT8( KCmgc, "+CMGC" );
_LIT8( KCmgsQuery, "+CMGS=?" );
_LIT8( KCmgcQuery, "+CMGC=?" );

// Carriage return needs to be checked from ATS3, or the following AT&V
// fails, because its not yet informed to upper layers.
_LIT8( KAts3, "ATS3=" );
const TUint8 KDefaultCarriageChar( 0xd );  // 13

const TUint8 KOff( 0x00 );
const TUint8 KOn( 0x01 );

const TInt KEchoOn( 1 );
const TInt KEchoOff( 2 );
const TInt KVerboseOn( 3 );
const TInt KVerboseOff( 4 );
const TInt KQuietOn( 5 );
const TInt KQuietOff( 6 );
const TInt KReplyTypeEditorStart( 1 );
const TInt KReplyTypeEditorEnd( 2 );

CModemAtSrv::CModemAtSrv( TInt aPriority ) :
    CPolicyServer( aPriority, KSrvPolicy ),
    iHandler(NULL),
    iEcho( KOn ),
    iEchoNotified( KInitialValue ),
    iEchoQueryIsOngoing( EFalse ),
    iVerbose( KOff ),
    iVerboseNotified( KInitialValue ),
    iVerboseQueryIsOngoing( EFalse ),
    iQuiet( KOff ),
    iQuietNotified( KInitialValue ),
    iQuietQueryIsOngoing( EFalse ),
    iCarriageReturn( KDefaultCarriageChar ),
    iCarriageReturnFound( EFalse )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_CMODEMATSRV, "CModemAtSrv::CModemAtSrv" );
    }

CModemAtSrv::~CModemAtSrv()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_CMODEMATSRV, "CModemAtSrv::~CModemAtSrv" );
    C_TRACE (( _L(">>CModemAtSrv::~CModemAtSrv()") ));
    iAtMessageArray.ResetAndDestroy();
    iAtMessageArray.Close();
    C_TRACE (( _L("<<CModemAtSrv::~CModemAtSrv()") ));
    }

CModemAtSrv* CModemAtSrv::NewLC()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_NEWLC, "CModemAtSrv::NewLC" );
    CModemAtSrv* self = new (ELeave) CModemAtSrv( EPriorityNormal );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->StartL( KATExtSrvName );
    RThread().SetPriority( EPriorityNormal );
    return self;
    }

void CModemAtSrv::RemovePipe( CModemAtHandler* aHandler )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_REMOVEPIPE, "CModemAtSrv::RemovePipe" );
    C_TRACE((_L("CModemAtSrv::RemovePipe()") ));
    ASSERT_PANIC_ALWAYS( NULL != aHandler );
    aHandler->RemovePipe();
    }

void CModemAtSrv::ClientClosed( CModemAtSession* aSession )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_CLIENTCLOSED, "CModemAtSrv::ClientClosed" );
    C_TRACE (( _L(">>CModemAtSrv::ClientClosed()") ));
    TInt index = iSessions.Find( aSession );
    if( index >= 0 )
        {
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSRV_CLIENTCLOSED, "CModemAtSrv::ClientClosed - Removing session;iSessions[index]->GetPluginType()=%d", iSessions[index]->GetPluginType() );
        C_TRACE((_L("Removing session (plugin type %d)"), iSessions[index]->GetPluginType() ));
        iSessions.Remove( index );

        if( ECommonPlugin == aSession->GetPluginType() )
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_CLIENTCLOSED, "CModemAtSrv::ClientClosed - Disconnecting ISI message handler" );
            C_TRACE((_L("Disconnecting ISI message handler ") ));
            aSession->GetHandler()->Disconnect();
            }

        for( int i = 0; i < iDteInfo.Count(); i++ )
            {
            if( aSession == iDteInfo[i].iSession )
                {
                OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSRV_CLIENTCLOSED, "CModemAtSrv::ClientClosed - removing dte info" );
                C_TRACE((_T("CModemAtSrv::ClientClosed() removing dte info") ));
                iDteInfo.Remove( i );
                }
            }

        TBool pluginsFound( EFalse );       
        for( int i = 0; i < iSessions.Count(); i++ )
            {
            if( 0 == iSessions[i]->GetName().Compare( aSession->GetName() ) )
                {
                pluginsFound = ETrue;
                break;
                }
            }

        if( !pluginsFound &&
            iHandler != aSession->GetHandler() )
            {
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSRV_CLIENTCLOSED, "CModemAtSrv::ClientClosed - Deleting handler" );
            C_TRACE((_L("Deleting handler") ));
            iFreeChannels.Append( aSession->GetHandler()->GetChannelId() );
            delete aSession->GetHandler();
            }

        C_TRACE((_T("<<CModemAtSrv::ClientClosed()") ));
        }
    }
 
CSession2* CModemAtSrv::NewSessionL( const TVersion& aVersion,
                                   const RMessage2& /*aMessage*/ ) const
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_NEWSESSIONL, "CModemAtSrv::NewSessionL" );
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
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_CONSTRUCTL, "CModemAtSrv::ConstructL" );
    C_TRACE (( _T("CModemAtSrv::ConstructL()") ));
    iHandler = CModemAtHandler::NewL( *this, EIscNokiaAtController );

    iFreeChannels.Append( EIscNokiaAtController2 );
    iFreeChannels.Append( EIscNokiaAtController3 );
    }

void CModemAtSrv::HandleSignalInd( const TUint8 aDteId, const TInt aCompleteCode )
    {
    C_TRACE((_T("CModemAtSrv::HandleSignalInd aDteId = %d sessions = %d, code = %d"), aDteId, iSessions.Count(), aCompleteCode ));
    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( iSessions[i]->GetDteId() == aDteId && iSessions[i]->IsSignalIndReqActive() )
            {
            C_TRACE (( _T("CModemAtSrv::HandleSignalInd() session found") ));
            iSessions[i]->SignalIndReceived( aCompleteCode );
            }
        }
    }

void CModemAtSrv::HandleUnsolicitedData( const TUint8 aDteId, const TDesC8& aData )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATSRV_HANDLEUNSOLICITEDDATA, "CModemAtSrv::HandleUnsolicitedData;aDteId=%d;iSessions.Count()=%d", aDteId, iSessions.Count() );
    C_TRACE ((_T("CModemAtSrv::HandleUnsolicitedData aDteId = %d sessions = %d"), aDteId, iSessions.Count() ));
    TBool unsolicitedDataHandled( EFalse );

    _LIT8( KNoCarrier, "NO CARRIER" );
    if( aData.Find( KNoCarrier ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_HANDLEUNSOLICITEDDATA, "CModemAtSrv::HandleUnsolicitedData - NO CARRIER -> COMMAND MODE" );
        C_TRACE(_L("NO CARRIER -> COMMAND MODE"));
        HandleCommandModeChange( aDteId, ECommandMode );
        }

    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( ( iSessions[i]->GetDteId() == aDteId || aDteId == KUnsolicitedDataDteId )&&
            iSessions[i]->IsUnsolicitedDataReqActive() &&
            iSessions[i]->GetPluginType() == EATExtPlugin )
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_HANDLEUNSOLICITEDDATA, "CModemAtSrv::HandleUnsolicitedData - session found" );
            C_TRACE (( _T("CModemAtSrv::HandleUnsolicitedData() session found") ));
            iSessions[i]->UnsolicitedData( aData );
            unsolicitedDataHandled = ETrue;
            return;
            }
        }
    if( ! unsolicitedDataHandled )
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSRV_HANDLEUNSOLICITEDDATA, "CModemAtSrv::HandleUnsolicitedData - request is not active yet so save message to the server" );
        C_TRACE (( _T("CModemAtSrv::HandleSignalInd() request is not active yet so save message to the server") ));
        
        // Note: If call is received with unsolicited "RING", it will not be sent, because request is not active in data mode.
        // After that "NO CARRIER" will copy over the "RING", and it will be sent, when returning to command mode.
        iTemporaryUnsolicitedData = aData;
        }
    }

TBool CModemAtSrv::IsUnsolicitedDataInBuffer()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_ISUNSOLICITEDDATAINBUFFER, "CModemAtSrv::IsUnsolicitedDataInBuffer;%d", (TInt) iTemporaryUnsolicitedData.Length() );
    C_TRACE ((_T("CModemAtSrv::IsUnsolicitedDataInBuffer %d"), (TInt) iTemporaryUnsolicitedData.Length() ));
    if( iTemporaryUnsolicitedData.Length() > 0)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

void CModemAtSrv::ResetUnsolicitedDataInBuffer()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_RESETUNSOLICITEDDATAINBUFFER, "CModemAtSrv::ResetUnsolicitedDataInBuffer" );
    C_TRACE (( _T("CModemAtSrv::ResetUnsolicitedDataInBuffer()") ));
    iTemporaryUnsolicitedData.Zero();
    }

void CModemAtSrv::GetUnsolicitedDataInBuffer( TDes8& aData )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_GETUNSOLICITEDDATAINBUFFER, "CModemAtSrv::GetUnsolicitedDataInBuffer" );
    C_TRACE (( _T("CModemAtSrv::GetUnsolicitedDataInBuffer()") ));
    aData.Copy( iTemporaryUnsolicitedData );
    }


void CModemAtSrv::HandleCommandModeChange( TUint8 aDteId, TCommandMode aMode )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATSRV_HANDLECOMMANDMODECHANGE, "CModemAtSrv::HandleCommandModeChange;iSessions.Count()=%d;aMode=%d", iSessions.Count(), (TInt)aMode );
    C_TRACE ((_T("CModemAtSrv::HandleCommandModeChange sessions = %d mode = %d"), iSessions.Count(), (TInt) aMode ));
    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( iSessions[i]->IsCommandModeReqActive() &&
            ECommonPlugin == iSessions[i]->GetPluginType() &&
            aDteId == iSessions[i]->GetDteId() )
            {
            OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_HANDLECOMMANDMODECHANGE, "CModemAtSrv::HandleCommandModeChange - session found" );
            C_TRACE (( _T("CModemAtSrv::HandleCommandModeChange() session found") ));
            iSessions[i]->CommandModeChanged( aMode );
            }
        }
    }

void CModemAtSrv::HandleIntermediateDataInd( const TUint8 aDteId,
    const TATPluginInterface aPluginType,
    const TDesC8& aResponse,
    const TUint8 aCommand )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATSRV_HANDLEINTERMEDIATEDATAIND, "CModemAtSrv::HandleIntermediateDataInd;aDteId=%d;iSessions.Count()=%d", aDteId, iSessions.Count() );
    C_TRACE ((_T("CModemAtSrv::HandleIntermediateDataInd aDteId = %d sessions = %d "), aDteId, iSessions.Count() ));
    C_TRACE ((_T("plugintype = %d command = %d"), (TInt)aPluginType, aCommand ));
    
    if ( iAtMessageArray.Count() > 0 )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_HANDLEINTERMEDIATEDATAIND, "CModemAtSrv::HandleIntermediateDataInd - iAtMessageArray[0]->ReceiveATResponse Intermediate data" );
        C_TRACE((_L("iAtMessageArray[0]->ReceiveATResponse Intermediate data")));
        iAtMessageArray[0]->ReceiveATResponse( aResponse, EFalse, KErrNone );
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_HANDLEINTERMEDIATEDATAIND, "CModemAtSrv::HandleIntermediateDataInd - INTERMEDIATE DATA received but no request active" );
        C_TRACE ((_T("CModemAtSrv::HandleIntermediateDataInd INTERMEDIATE DATA received but no request active") ));
        HandleUnsolicitedData( aDteId, aResponse);
        }
    }

void CModemAtSrv::HandleATResponse( 
    const TUint8 aDteId, 
    CModemAtHandler* aHandler,
    const TDesC8& aResponse, 
    const TUint8 aCommand )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse;aDteId=%d;iSessions.Count()=%d", aDteId, iSessions.Count() );
    C_TRACE ((_T(">>CModemAtSrv::HandleATResponse aDteId = %d sessions = %d "), aDteId, iSessions.Count() ));
    C_TRACE ((_T("CModemAtSrv::HandleATResponse command = %d"), aCommand ));
    TRACE_ASSERT( NULL != aHandler )
    TInt completeCode( KErrNone );
    if( IsTextMessageMode( aHandler ) )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Text message mode is on" );
        C_TRACE(_L("Text message mode is on"));
        if( aResponse.Find( KCursor ) != KErrNotFound )
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - detected during text message mode" );
            C_TRACE(_L("> detected during text message mode"));
            completeCode = KReplyTypeEditorStart; // replace later with EReplyTypeEditorStart
            }
        else if( aResponse.Find( KOkResponse ) != KErrNotFound ||
        	  aResponse.Find( KErrorResponse ) != KErrNotFound ||
        	  aResponse.Find( KOkResponseNoVerbose ) != KErrNotFound ||
        	  aResponse.Find( KErrorResponseNoVerbose ) != KErrNotFound )
            {
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Response detected after text message mode" );
            C_TRACE(_L("Response detected after text message mode"));
            aHandler->SetTextMessageMode( EFalse );
            completeCode = KReplyTypeEditorEnd; // replace later with EReplyTypeEditorEnd
            }
        if( completeCode == KErrNone )
            {
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Response detected after text message mode (quiet mode)" );
            C_TRACE(_L("Response detected after text message mode (quiet mode)"));
            aHandler->SetTextMessageMode( EFalse );
            completeCode = KReplyTypeEditorEnd;
            }
        }

    if( iEchoQueryIsOngoing )
        {
        OstTraceExt2( TRACE_NORMAL, DUP5_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Echo query ongoing.;iEcho=%d;iEchoNitified=%d", iEcho, iEchoNotified );
        C_TRACE((_L("Echo query ongoing. current: %d, notified %d"), iEcho, iEchoNotified ));
        if( iEcho == KOff && iEchoNotified != KOff )
            {
            OstTrace0( TRACE_NORMAL, DUP7_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Echo off, notify common plug-in." );
            C_TRACE(_L("Echo off, notify common plug-in."));
            HandleSignalInd( aDteId, KEchoOff );
            iEchoNotified = KOff;
            }
        else if( iEcho == KOn && iEchoNotified != KOn )
            {
            OstTrace0( TRACE_NORMAL, DUP8_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Echo on, notify common plug-in." );
            C_TRACE(_L("Echo on, notify common plug-in."));
            HandleSignalInd( aDteId, KEchoOn );
            iEchoNotified = KOn;
            }
        iEchoQueryIsOngoing = EFalse;
        }
    else if( iVerboseQueryIsOngoing )
        {
        OstTraceExt2( TRACE_NORMAL, DUP9_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Verbose query ongoing.;iVerbose=%d;iVerboseNotified=%d", iVerbose, iVerboseNotified );
        C_TRACE((_L("Verbose query ongoing. current: %d, notified %d"), iVerbose, iVerboseNotified ));
        if( iVerbose == KOff && iVerboseNotified != KOff )
            {
            OstTrace0( TRACE_NORMAL, DUP10_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Verbose off received, notify common plug-in." );
            C_TRACE(_L("Verbose off received, notify common plug-in."));
            HandleSignalInd( aDteId, KVerboseOff );
            iVerboseNotified = KOff;
            }
        else if( iVerbose == KOn && iVerboseNotified != KOn )
            {
            OstTrace0( TRACE_NORMAL, DUP11_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Verbose on received, notify common plug-in." );
            C_TRACE(_L("Verbose on received, notify common plug-in."));
            HandleSignalInd( aDteId, KVerboseOn );
            iVerboseNotified = KOn;
            }
        iVerboseQueryIsOngoing = EFalse;
        }
    else if( iQuietQueryIsOngoing )
        {
        OstTraceExt2( TRACE_NORMAL, DUP12_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Quiet query ongoing.;iQuiet=%d;iQuietNotified=%d", iQuiet, iQuietNotified );
        C_TRACE((_L("Quiet query ongoing. current: %d, notified %d"), iQuiet, iQuietNotified ));
        if( iQuiet == KOff && iQuietNotified != KOff )
            {
            OstTrace0( TRACE_NORMAL, DUP13_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Quiet off, notify common plug-in." );
            C_TRACE(_L("Quiet off, notify common plug-in."));
            HandleSignalInd( aDteId, KQuietOff );
            iQuietNotified = KOff;
            }
        else if( iQuiet == KOn && iQuietNotified != KOn )
            {
            OstTrace0( TRACE_NORMAL, DUP14_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Quiet on, notify common plug-in." );
            C_TRACE(_L("Quiet on, notify common plug-in."));
            HandleSignalInd( aDteId, KQuietOn );
            iQuietNotified = KOn;
            }
        iQuietQueryIsOngoing = EFalse;
        }

    if ( iAtMessageArray.Count() > 0 )
        {
        CAtMessage* tmp = iAtMessageArray[0];
        OstTrace1( TRACE_NORMAL, DUP15_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - iAtMessageArray[0]->ReceiveATResponse;tmp=%x", tmp );
        C_TRACE((_L("iAtMessageArray[0]->ReceiveATResponse 0x%x"), tmp ));
        tmp->ReceiveATResponse( aResponse, ETrue, completeCode );
        OstTrace0( TRACE_NORMAL, DUP16_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - Remove the first message in Fifo" );
        C_TRACE((_L("CModemAtSrv::HandleATResponse Remove the first message in Fifo")));
        RemoveFirstFromSendFifo();
        OstTrace0( TRACE_NORMAL, DUP17_CMODEMATSRV_HANDLEATRESPONSE, "CModemAtSrv::HandleATResponse - SendNextFromFifo" );
        C_TRACE((_L("CModemAtSrv::HandleATResponse SendNextFromFifo")));
        SendNextFromFifo();
        }
    else
        {
        HandleUnsolicitedData( aDteId, aResponse);
        }
    C_TRACE ((_T("<<CModemAtSrv::HandleATResponse()") ));
    }

TInt CModemAtSrv::ConnectToModemL( CModemAtSession* aSession, TATPluginInterface aPluginType ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL;aSession=%x", aSession );
    C_TRACE ((_T(">>CModemAtSrv::ConnectToModem 0x%x"), aSession));

    OstTraceExt2( TRACE_NORMAL, DUP1_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL;iSessions.Count()=%d;aPluginType=%d", iSessions.Count(), (TInt)aPluginType );
    C_TRACE(( _L("session count: %d, type: %d"), iSessions.Count(), (TInt) aPluginType ));

    if( aPluginType == ECommonPlugin )
        {
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Common plug-in connecting;aSession=%x", aSession );
        C_TRACE ((_T("Common plug-in connecting")));
        C_TRACE ((_T("<<CModemAtSrv::ConnectToModem session: 0x%x"), aSession));
        
        if( KMaxDteCount >= GetDteCount() )
            {
            TBool handlerAttached( EFalse );
            for( TInt i = 0; i < iSessions.Count(); i++ )
                {
                if( iHandler == iSessions[i]->GetHandler() )
                    {
                    handlerAttached = ETrue;
                    break;
                    }
                }

            TUint8 devId( 0 );
            TUint8 objId( 0 );
            TBool dteFound( EFalse );
            OstTrace1( TRACE_NORMAL, DUP3_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL;iDteInfo.Count()=%d", iDteInfo.Count() );
            C_TRACE ((_T("iDteInfo.Count(): %d"), iDteInfo.Count()));
            for( TInt j = 0; j < iDteInfo.Count(); j++ )
                {
                if( NULL == iDteInfo[j].iSession )
                    {
                    dteFound = ETrue;
                    CModemAtHandler* handler( NULL );
                    devId = iDteInfo[j].iDevId;
                    objId = iDteInfo[j].iObjId;
                    if( handlerAttached )
                        {
                        OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Creating new handler" );
                        C_TRACE ((_T("Creating new handler")));
                        TUint8 channelId( 0 );
                        if( KErrNone == GetFreeChannelId( channelId ) )
                            {
                            handler = CModemAtHandler::NewL(
                                *this,
                                channelId );
                            iDteInfo[j].iSession = aSession;
                            aSession->SetHandler( handler );
                            }
                        else
                            {
                            TRACE_ASSERT_ALWAYS;
                            aSession->ModemConnected( KErrGeneral );
                            OstTrace0( TRACE_NORMAL, DUP14_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL No free channel id" );
                            C_TRACE ((_T("No free channel id")));
                            return KErrGeneral;
                            }
                        }
                    else
                        {
                        OstTrace0( TRACE_NORMAL, DUP5_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Using already created handler" );
                        C_TRACE ((_T("Using already created handler")));
                        iDteInfo[j].iSession = aSession;
                        aSession->SetHandler( iHandler );
                        }
                    break;
                    }
                }
            if( dteFound )
                {
                return aSession->GetHandler()->Connect( devId, objId );
                }
            else
                {
                OstTrace0( TRACE_NORMAL, DUP6_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - No dte info found. Possibly PNS_NAME_ADD_IND not received -> creating default pipe." );
                C_TRACE ((_T("No dte info found. Possibly PNS_NAME_ADD_IND not received -> creating default pipe.")));
                
                CModemAtHandler* handler( NULL );
                
                TUint KInitialDpObjId( 0x2f );
                TUint dataportObjectid( KInitialDpObjId );
                TBool found( EFalse );
                    
                if( !handlerAttached )
                    {
                    OstTrace0( TRACE_NORMAL, DUP7_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Using already created handler" );
                    C_TRACE ((_T("Using already created handler")));
                    aSession->SetHandler( iHandler );

                    OstTrace0( TRACE_NORMAL, DUP8_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Calling GetDataportObjectId()" );
                    C_TRACE ((_T("Calling GetDataportObjectId()")));
                    aSession->GetHandler()->GetDataportObjectId( dataportObjectid );
                    
                    OstTrace0( TRACE_NORMAL, DUP9_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Calling AddDevice()" );
                    C_TRACE ((_T("Calling AddDevice()")));
                    AddDevice( THIS_DEVICE, (TUint8)dataportObjectid );
                    
                    OstTrace0( TRACE_NORMAL, DUP10_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Set session to this device" );
                    C_TRACE ((_T("Set session to this device")));
                    for( TInt j = 0; j < iDteInfo.Count(); j++ )
                        {
                        if( NULL == iDteInfo[j].iSession ) // if iSession is NULL, the device handler is not attached
                            {
                            OstTrace0( TRACE_NORMAL, DUP11_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Device session found" );
                            C_TRACE ((_T("Device session found")));
                            iDteInfo[j].iSession = aSession;
                            found = ETrue;
                            }
                        }
                    if( !found )
                        {
                        OstTrace0( TRACE_NORMAL, DUP12_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Device session not found" );
                        C_TRACE ((_T("Device session not found")));
                        if( iDteInfo.Count() == 1 )
                            {
                            OstTrace0( TRACE_NORMAL, DUP13_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Set session to first device" );
                            C_TRACE ((_T("Set session to first device")));
                            iDteInfo[0].iSession = aSession;
                            }
                        }
                    }
                else
                    {
                    // NOTE: It might be possible to have second device attached and then no pipe has been created
                    // before this, EIscNokiaDataport2 has been used with first device. Now we need use
                    // EIscNokiaDataport4. EIscNokiaDataport5 is not currently supported without PNS_NAME_ADD_IND.
                    TUint8 channelId( 0 );
                    if( KErrNone == GetFreeChannelId( channelId ) )
                        {
                        C_TRACE ((_T("Creating new handler")));
                        handler = CModemAtHandler::NewL(
                            *this,
                            channelId );
                        OstTrace0( TRACE_NORMAL, DUP15_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Using just created handler" );
                        C_TRACE ((_T("Using just created handler")));
                        aSession->SetHandler( handler );

                        OstTrace0( TRACE_NORMAL, DUP16_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Calling GetDataportObjectId()" );
                        C_TRACE ((_T("Calling GetDataportObjectId()")));

                        // Get dataport 4 
                        aSession->GetHandler()->GetSecondaryDataportObjectId( dataportObjectid );

                        OstTrace0( TRACE_NORMAL, DUP17_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Calling AddDevice()" );
                        C_TRACE ((_T("Calling AddDevice()")));
                        AddDevice( THIS_DEVICE, (TUint8)dataportObjectid );


                        OstTrace0( TRACE_NORMAL, DUP18_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Set session to the second device" );
                        C_TRACE ((_T("Set session to the second device")));

                        for( TInt j = 0; j < iDteInfo.Count(); j++ )
                            {
                            if( NULL == iDteInfo[j].iSession )
                                {
                                OstTrace0( TRACE_NORMAL, DUP19_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Device session found" );
                                C_TRACE ((_T("Device session found")));
                                iDteInfo[j].iSession = aSession;
                                found = ETrue;
                                }
                            }
                        if( !found )
                            {
                            OstTrace0( TRACE_NORMAL, DUP20_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Device 2 session not found" );
                            C_TRACE ((_T("Device 2 session not found")));
                            if( iDteInfo.Count() == 2 )
                                {
                                OstTrace0( TRACE_NORMAL, DUP21_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Set session to second device" );
                                C_TRACE ((_T("Set session to second device")));
                                iDteInfo[1].iSession = aSession;
                                }
                            }
                        }
                    else
                        {	
                        TRACE_ASSERT_ALWAYS;
                        aSession->ModemConnected( KErrGeneral );
                        OstTrace0( TRACE_NORMAL, DUP26_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL No free channel id" );
                        C_TRACE ((_T("No free channel id")));
                        }
                    }
                if( found )
                    {
                    OstTrace0( TRACE_NORMAL, DUP22_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Calling Connect( THIS_DEVICE, dataportObjectid )" );
                    C_TRACE ((_T("Calling Connect( THIS_DEVICE, dataportObjectid )")));
                    return aSession->GetHandler()->Connect( THIS_DEVICE, dataportObjectid );
                    }
                else
                    {
                    C_TRACE ((_T("no dte info found, connect failed")));
                    OstTrace0( TRACE_NORMAL, DUP27_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL no dte info found, connect failed" );
                    aSession->ModemConnected( KErrGeneral );
                    }
                }
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP23_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Too many connections, connect failed" );
            C_TRACE ((_T("Too many connections, connect failed")));
            aSession->ModemConnected( KErrGeneral );
            }
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP24_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL - Atext plug-in connecting" );
        C_TRACE ((_T("Atext plug-in connecting")));

        for( TInt i = 0; i < iSessions.Count(); i++ )
            {
            if( ECommonPlugin == iSessions[i]->GetPluginType() &&
                0 == iSessions[i]->GetName().Compare( aSession->GetName() ) )
                {
                aSession->SetDteId( iSessions[i]->GetDteId() );
                aSession->SetHandler( iSessions[i]->GetHandler() );
                break;
                }
            }
        aSession->ModemConnected( KErrNone );

        OstTrace1( TRACE_NORMAL, DUP25_CMODEMATSRV_CONNECTTOMODEML, "CModemAtSrv::ConnectToModemL;aSession=%x", aSession );
        C_TRACE ((_T("<<CModemAtSrv::ConnectToModem 0x%x"), aSession));
        }
    return KErrNone;
    }

void CModemAtSrv::AddToSendFifo( const TATPluginInterface aPluginType, CAtMessage* aMessage )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATSRV_ADDTOSENDFIFO, "CModemAtSrv::AddToSendFifo;aPluginType=%d;aMessage=%p", (TInt)aPluginType, aMessage );
    C_TRACE (( _T("CModemAtSrv::AddToSendFifo( aPluginType: %d, aMessage: 0x%x)"), (TInt)aPluginType, aMessage));

    iAtMessageArray.Append( aMessage ); // only one AT command at the time in modem

    if( iAtMessageArray.Count() == 1 ) //if empty Fifo send immediately
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_ADDTOSENDFIFO, "CModemAtSrv::AddToSendFifo - Sending message immediately" );
        C_TRACE((_L("Sending message immediately")));
        CModemAtSession* session = aMessage->GetSession();
        if( session )
            {
            CModemAtHandler* handler( session->GetHandler() );
            PeekAtCommand( handler, aMessage->GetBuffer() );

            handler->SendATCommand( 
                aPluginType,
                aMessage->GetMessageType(),
                aMessage->GetBuffer() );
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_ADDTOSENDFIFO, "CModemAtSrv::AddToSendFifo - Session NULL" );
            C_TRACE((_L("Session NULL")));
            TRACE_ASSERT_ALWAYS;
            }
        }
    }

void CModemAtSrv::RemoveFirstFromSendFifo()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_REMOVEFIRSTFROMSENDFIFO, "CModemAtSrv::RemoveFirstFromSendFifo;iAtMessageArray.Count()=%d", iAtMessageArray.Count() );
    C_TRACE(( _T("CModemAtSrv::RemoveFirstFromSendFifo iAtMessageArray.Count = %d"), iAtMessageArray.Count() ));
    if( iAtMessageArray.Count( ) > 0 )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_REMOVEFIRSTFROMSENDFIFO, "CModemAtSrv::RemoveFirstFromSendFifo - 1" );
        C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 1") ));
        CAtMessage* msg = iAtMessageArray[0];
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATSRV_REMOVEFIRSTFROMSENDFIFO, "CModemAtSrv::RemoveFirstFromSendFifo - 2;msg=%x", msg );
        C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 2 0x%x"), msg ));
        iAtMessageArray.Remove(0);
        if( msg )
            {
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSRV_REMOVEFIRSTFROMSENDFIFO, "CModemAtSrv::RemoveFirstFromSendFifo - 3" );
            C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 3") ));
            delete msg;
            }
        else
            {
            TRACE_ASSERT_ALWAYS;
            }
        OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSRV_REMOVEFIRSTFROMSENDFIFO, "CModemAtSrv::RemoveFirstFromSendFifo - 4" );
        C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 4") ));
        }
    OstTrace0( TRACE_NORMAL, DUP5_CMODEMATSRV_REMOVEFIRSTFROMSENDFIFO, "CModemAtSrv::RemoveFirstFromSendFifo - 5" );
    C_TRACE (( _T("CModemAtSrv::RemoveFirstFromSendFifo 5") ));
    }

void CModemAtSrv::SendNextFromFifo()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_SENDNEXTFROMFIFO, "CModemAtSrv::SendNextFromFifo - fifo message count;iAtMessageArray.Count()=%d", iAtMessageArray.Count() );
    C_TRACE ((_T("CModemAtSrv::SendNextFromFifo fifo has %d message(s)"), iAtMessageArray.Count() ));
    
    TPtr8 ptr( 0, 0 );
    if( iAtMessageArray.Count() > 0 )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_SENDNEXTFROMFIFO, "CModemAtSrv::SendNextFromFifo - Messages in buffer... Sending message" );
        C_TRACE((_L("Messages in buffer... Sending message")));
        ptr.Set( iAtMessageArray[0]->GetBuffer() );

        CModemAtHandler* handler( iAtMessageArray[0]->GetSession()->GetHandler() );
        if( handler )
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_SENDNEXTFROMFIFO, "CModemAtSrv::SendNextFromFifo - iHandler->SendATCommand()" );
            C_TRACE (( _T("iHandler->SendATCommand()") ));
            PeekAtCommand( handler, ptr );
            handler->SendATCommand( 
                iAtMessageArray[0]->GetSession()->GetPluginType(),
                iAtMessageArray[0]->GetMessageType(),
                ptr );
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSRV_SENDNEXTFROMFIFO, "CModemAtSrv::SendNextFromFifo - Handler NULL" );
            C_TRACE((_L("Handler NULL")));
            TRACE_ASSERT_ALWAYS;
            }   
        }
    }

void CModemAtSrv::CancelFifoRequests( const CModemAtSession* aSession )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_CANCELFIFOREQUESTS, "CModemAtSrv::CancelFifoRequests;iAtMessageArray.Count()=%d", iAtMessageArray.Count() );
    C_TRACE((_T(">>CModemAtSrv::CancelFifoRequests count: %d"), iAtMessageArray.Count() ));
    TInt messageCount = iAtMessageArray.Count();
    for( TInt i = 0; i < messageCount; i++ )
        {
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSRV_CANCELFIFOREQUESTS, "CModemAtSrv::CancelFifoRequests - ;i=%d", i );
        C_TRACE((_T("CancelFifoRequests %d"), i ));
        CAtMessage* msg = iAtMessageArray[ i ];
        if( msg )
            {
            OstTrace1( TRACE_NORMAL, DUP2_CMODEMATSRV_CANCELFIFOREQUESTS, "CModemAtSrv::CancelFifoRequests - Found Request:;msg=%x", msg );
            C_TRACE((_T("Found Request: 0x%x"),msg ));
            if( msg->GetSession() == aSession)
                {
                OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSRV_CANCELFIFOREQUESTS, "CModemAtSrv::CancelFifoRequests - canceling request" );
                C_TRACE((_T("CancelFifoRequests - canceling request") ));
                msg->CancelRequest();
                OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSRV_CANCELFIFOREQUESTS, "CModemAtSrv::CancelFifoRequests - delete msg" );
                C_TRACE (( _T("CModemAtSrv::CancelFifoRequest delete msg") ));
                delete msg;
                }
            }
        }
    OstTrace1( TRACE_NORMAL, DUP5_CMODEMATSRV_CANCELFIFOREQUESTS, "CModemAtSrv::CancelFifoRequests - iAtMessageArray.Remove;messageCount=%d", messageCount );
    C_TRACE (( _T("CModemAtSrv::CancelFifoRequest iAtMessageArray.Remove %d messages"), messageCount ));
    for( TInt j = 0; j < messageCount && iAtMessageArray.Count() > 0; j++ )
        {
        iAtMessageArray.Remove( 0 );
        }
    C_TRACE(( _T("<<CModemAtSrv::CancelFifoRequests()") ));
    }

TInt CModemAtSrv::SessionCount()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_SESSIONCOUNT, "CModemAtSrv::SessionCount;iSessions.Count()=%d", iSessions.Count() );
    C_TRACE(( _T("CModemAtSrv::SessionCount() %d"), iSessions.Count() ));
    return iSessions.Count();
    }

void CModemAtSrv::SetDteIdAndConnect( const TUint8 aDteId, const TInt aConnectionError ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_SETDTEIDANDCONNECT, "CModemAtSrv::SetDteIdAndConnect;aDteId=%d", aDteId );
    C_TRACE(( _T("CModemAtSrv::SetDteIdAndConnect( %d )"), aDteId ));
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSRV_SETDTEIDANDCONNECT, "CModemAtSrv::SetDteIdAndConnect;iSessions.Count()=%d", iSessions.Count() );
    C_TRACE ((_T("sessions = %d"), iSessions.Count() ));

    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        if( iSessions[i]->IsConnectReqActive() )
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_SETDTEIDANDCONNECT, "CModemAtSrv::SetDteIdAndConnect - set sessions to connected" );
            C_TRACE (( _T("set sessions to connected") ));
            iSessions[i]->SetDteId( aDteId );
            iSessions[i]->ModemConnected( aConnectionError );
            }
        }
    }

TBool CModemAtSrv::IsPipeInDataMode( CModemAtHandler* aHandler )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_ISPIPEINDATAMODE, "CModemAtSrv::IsPipeInDataMode;aHandler=%x", aHandler );
    C_TRACE(( _T("CModemAtSrv::IsPipeInDataMode() handler: 0x%x"), aHandler ));
    TRACE_ASSERT( NULL != aHandler )
    return aHandler->IsPipeInDataMode();
    }

TBool CModemAtSrv::IsTextMessageMode( CModemAtHandler* aHandler )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_ISTEXTMESSAGEMODE, "CModemAtSrv::IsTextMessageMode;aHandler=%x", aHandler );
    C_TRACE(( _T("CModemAtSrv::IsTextMessageMode() handler: 0x%x"), aHandler ));
    TRACE_ASSERT( NULL != aHandler )
    return aHandler->IsTextMessageMode();
    }

void CModemAtSrv::PeekAtCommand( CModemAtHandler* aHandler, TDes8& aCmd )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand;aHandler=%x", aHandler );
    C_TRACE(( _T("CModemAtSrv::PeekAtCommand() handler: 0x%x"), aHandler ));
    TRACE_ASSERT( NULL != aHandler )

    if( ( aCmd.Find( KCmgs ) != KErrNotFound || 
    	  aCmd.Find( KCmgc ) != KErrNotFound ) &&
    	  aCmd.Find( KCmgsQuery ) == KErrNotFound &&
    	  aCmd.Find( KCmgcQuery ) == KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - +CMGS or +CMGC detected. The indications after this will be ignored." );
        C_TRACE(_L("+CMGS or +CMGC detected. The indications after this will be ignored."));
        aHandler->SetTextMessageMode( ETrue );
        }
    // no else

    if( aCmd.Find( KAtv1 ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATV1 received" );
        C_TRACE(( _T("ATV1 received") ));
        iVerbose = KOn;
        iVerboseQueryIsOngoing = ETrue;
        }
    else if( aCmd.Find( KAtv0 ) != KErrNotFound || 
    	  aCmd.Find( KAtv ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATV0 received" );
        C_TRACE(( _T("ATV(0) received") ));
        iVerbose = KOff;
        iVerboseQueryIsOngoing = ETrue;
        }
    else if( aCmd.Find( KAtq1 ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATQ1 received" );
        C_TRACE(( _T("ATQ1 received") ));
        iQuiet = KOn;
        iQuietQueryIsOngoing = ETrue;
        }
    else if( aCmd.Find( KAtq0 ) != KErrNotFound ||
    	  aCmd.Find( KAtq ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP5_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATQ0 received" );
        C_TRACE(( _T("ATQ(0) received") ));
        iQuiet = KOff;
        iQuietQueryIsOngoing = ETrue;
        }
    else if( aCmd.Find( KAte1 ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP6_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATE1 received" );
        C_TRACE(( _T("ATE(1) received") ));
        iEcho = KOn;
        iEchoQueryIsOngoing = ETrue;
        }
    else if( aCmd.Find( KAte0 ) != KErrNotFound ||
    	  aCmd.Find( KAte ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP7_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATE0 received" );
        C_TRACE(( _T("ATE(0) received") ));
        iEcho = KOff;
        iEchoQueryIsOngoing = ETrue;
        }
    else if( aCmd.Find( KAts3 ) != KErrNotFound )
        {
        OstTrace0( TRACE_NORMAL, DUP8_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATS3 received" );
        C_TRACE(( _T("ATS3= received, saving it for the next profile query") ));

        TInt valueLength = aCmd.Length() - KAts3().Length() - 1;
		OstTrace1( TRACE_NORMAL, DUP9_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand;valueLength=%d", valueLength );
		OstTrace1( TRACE_NORMAL, DUP10_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand;KAts3Length=%d", KAts3().Length() );
        C_TRACE(( _T("Reading %d char's starting from %d."), valueLength, KAts3().Length() ));
        if( valueLength > 0 )
            {
            TLex8 lex( aCmd.Mid( KAts3().Length(), valueLength ) );
            TInt value;
            if( lex.Val(value) == KErrNone )
                {
                C_TRACE(( _T("iCarriageReturn 0x%x "), value ));
                iCarriageReturn = (TUint8)value;
                iCarriageReturnFound = ETrue;
                }
            }
        // no else
        }

    if( aCmd.Find( KAts3 ) == KErrNotFound &&
    	  iCarriageReturnFound )
        {
        OstTrace0( TRACE_NORMAL, DUP11_CMODEMATSRV_PEEKATCOMMAND, "CModemAtSrv::PeekAtCommand - ATE=? received" );
        C_TRACE(( _T("ATS3 has set the carriage return, using it once for profile query that will notify it to upper layers") ));
        aCmd.Copy( aCmd.Mid( 0, aCmd.Length() - 1 ) );
        aCmd.Append( iCarriageReturn );
        iCarriageReturnFound = EFalse;
        }

    }

TBool CModemAtSrv::IsEchoNotified( const TUint8 aEchoValue )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSRV_ISECHONOTIFIED, "CModemAtSrv::IsEchoNotified;aEchoValue=%d", aEchoValue );
    C_TRACE(( _T("CModemAtSrv::IsEchoNotified( %d )"), aEchoValue ));
    if( aEchoValue != iEchoNotified )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_ISECHONOTIFIED, "CModemAtSrv::IsEchoNotified - Echo value is not notified yet." );
        C_TRACE(( _T("Echo value is not notified yet.") ));
        return EFalse;
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_ISECHONOTIFIED, "CModemAtSrv::IsEchoNotified - Echo value is the same as before" );
        C_TRACE(( _T("Echo value is the same as before") ));
        return ETrue;
        }
    }

TBool CModemAtSrv::IsSettingQueryOngoing()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_ISSETTINGQUERYONGOING, "CModemAtSrv::IsSettingQueryOngoing" );
    C_TRACE(( _T("CModemAtSrv::IsSettingQueryOngoing()") ));
    if ( iEchoQueryIsOngoing || iVerboseQueryIsOngoing || iQuietQueryIsOngoing )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


void CModemAtSrv::AddDevice( TUint8 aDevId, TUint8 aObjId )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_ADDDEVICE, "CModemAtSrv::AddDevice" );
    C_TRACE(( _T("CModemAtSrv::AddDevice()") ));
    TBool alreadyFound( EFalse );
    
    for( int i = 0; i < iDteInfo.Count(); i++ )
        {
        if( aDevId == iDteInfo[i].iDevId && aObjId == iDteInfo[i].iObjId )
            {
            OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_ADDDEVICE, "CModemAtSrv::AddDevice - dte already found" );
            C_TRACE(( _T("CModemAtSrv::AddDevice() dte already found") ));
            alreadyFound = ETrue;
            break;
            }
        }
    if( !alreadyFound )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSRV_ADDDEVICE, "CModemAtSrv::AddDevice - adding dte" );
        C_TRACE(( _T("CModemAtSrv::AddDevice() adding dte") ));
        TDteInfo devInfo;
        devInfo.iDevId = aDevId;
        devInfo.iObjId = aObjId;
        devInfo.iSession = NULL;
        iDteInfo.Append( devInfo );
        }
    }


void CModemAtSrv::RemoveDevice( TUint8 aDevId, TUint8 aObjId )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_REMOVEDEVICE, "CModemAtSrv::RemoveDevice" );
    C_TRACE(( _T("CModemAtSrv::RemoveDevice()") ));

    for( int i = 0; i < iDteInfo.Count(); i++ )
        {
        if( aDevId == iDteInfo[i].iDevId && aObjId == iDteInfo[i].iObjId )
            {
            OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSRV_REMOVEDEVICE, "CModemAtSrv::RemoveDevice - CModemAtSrv::RemoveDevice() device found" );
            C_TRACE(( _T("CModemAtSrv::RemoveDevice() device found") ));
            iDteInfo.Remove( i );
            break;
            }
        }
    }

TUint8 CModemAtSrv::GetDteCount()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_GETDTECOUNT, "CModemAtSrv::GetDteCount" );
    C_TRACE(( _T("CModemAtSrv::GetDteCount()") ));
    TUint8 count( 0 );
    for( int i = 0; i < iSessions.Count(); i++ )
        {
        if( ECommonPlugin == iSessions[i]->GetPluginType() )
            {
            count++;
            }
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CModemAtSrv::GetFreeChannelId
// Gets free ISC channel id for new connection
// -----------------------------------------------------------------------------
//
TInt CModemAtSrv::GetFreeChannelId( TUint8& aChannel )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSRV_GETFREECHANNELID, "CModemAtSrv::GetFreeChannelId" );
    C_TRACE(( _T("CModemAtSrv::GetFreeChannelId()") ));
    TInt ret( KErrNotFound );

    if( 0 < iFreeChannels.Size() )
        {
        ret = KErrNone;
        aChannel = iFreeChannels[0];
        iFreeChannels.Delete( 0, 1 );
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSRV_GETFREECHANNELID, "CModemAtSrv::GetFreeChannelId  free channel found: 0x%x", aChannel );
        C_TRACE(( _T("CModemAtSrv::GetFreeChannelId: free channel found: 0x%x"), aChannel ));
        }
    return ret;
    }
