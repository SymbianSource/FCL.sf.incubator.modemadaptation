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



#include <PhonetIsi.h>              // For phonet-header offsets like ISI_HEADER_OFFSET_MESSAGEID
#include <pipeisi.h>                // For PEP_IND_BUSY etc..
#include <pipe_sharedisi.h>         // For PN_MULTI_CREDIT_FLOW_CONTROL etc..
#include <pn_const.h>

#include "pep.h"                    // For DPep
#include "peptransceiver.h"
#include "iadtrace.h"
#include "internalapi.h" 
#include "iadnokiadefinitions.h"
#include "iadinternaldefinitions.h" // For CREDIT_.. and KIADChDataRx...
#include "iadhelpers.h"             // For GET_RECEIVER_.., GET_SENDER_... etc.. macros
#include "router.h"					// For DRouter
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "peptransceiverTraces.h"
#endif

const TUint8 KPnPipeSbPreferredFcRx( 0x08 );
const TUint8 KPnPipeSbPreferredFcTx( 0x08 );

const TInt KPepRespCommonErrorCodeOffset( 3 );
const TInt KPnsPepRespPepErrorCode( ISI_HEADER_SIZE + KPepRespCommonErrorCodeOffset );
const TInt KPnsPepStatusIndIndicationId( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_INDICATIONID );// 12 
const TInt KPnsPepStatusIndIndicationData( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_INDICATIONDATA );// 15

const TInt KPnsPepRespLength( 12 );
const TInt KPepRespReqIndCommonPipeHandleOffset( 2 ); // TODO: use offsets!

// TODO: change this to use UniqueID of DThread instead and to extension 
void DPepTransceiver::CheckDfc()
	{
	OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_CHECKDFC_ENTRY, ">DPepTransceiver::CheckDfc" );

    DObject* tempObj = reinterpret_cast<DObject*>( &Kern::CurrentThread() );
    TUint8* buffer = ( TUint8* )Kern::Alloc( 100 );
    TPtr8* bufferPtr = new ( TPtr8 )( buffer, 100 );
    tempObj->Name( *bufferPtr );
    C_TRACE( ( _T( "DRouter::CheckDfc" ) ) );
    if ( bufferPtr->Compare( KIADExtensionDfc ) != KErrNone ) 
        {
        for( TInt i( 0 );i < bufferPtr->Length(); ++i )
           {
           C_TRACE( ( _T( "%c" ), i, bufferPtr->Ptr()[ i ] ) );
           }
        ASSERT_RESET_ALWAYS( 0, EIADWrongDFCQueueUsed | EIADFaultIdentifier4 << KFaultIdentifierShift  );
        }
    Kern::Free( buffer );
    delete bufferPtr;
    bufferPtr = NULL;

	OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_CHECKDFC_EXIT, "<DPepTransceiver::CheckDfc" );
	}

#ifdef _DEBUG
#define    ASSERT_DFCTHREAD_INEXT() CheckDfc()
#else
#define    ASSERT_DFCTHREAD_INEXT()
#endif

DPepTransceiver::DPepTransceiver(
        DRouter* aRouter,
        MIAD2ChannelApi* aUl
        ):
        iFlowCtrl( EIADFlowControlOn ),
        iPipeHandle( 0x00 ),
        iRouter( aRouter ),
        iRcvChannel( aUl )
    {
    OstTraceExt2( TRACE_PIPE, DPEPTRANSCEIVER_DPEPTRANSCEIVER_ENTRY, ">DPepTransceiver::DPepTransceiver;aRouter=%x;aUl=%x", ( TUint )( aRouter ), ( TUint )( aUl ) );

    C_TRACE( ( _T( "DPepTransceiver::DPepTransceiver pipehandle 0x%x ->" ), iPipeHandle ) );
    ASSERT_RESET_ALWAYS( iRcvChannel && iRouter, EIADNullParameter | EIADFaultIdentifier13 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DPepTransceiver::DPepTransceiver <-" ) ) );

    OstTraceExt2( TRACE_PIPE, DPEPTRANSCEIVER_DPEPTRANSCEIVER_EXIT, "<DPepTransceiver::DPepTransceiver;aRouter=%x;aUl=%x", ( TUint )( aRouter ), ( TUint )( aUl ) );
    }

DPepTransceiver::DPepTransceiver(
        TUint8 aPipeHandle
        ):
    iFlowCtrl( EIADFlowControlOn ),
    iPipeHandle( aPipeHandle ),
    iRouter( NULL ),
    iRcvChannel( NULL )
    {
    OstTraceExt1( TRACE_PIPE, DUP1_DPEPTRANSCEIVER_DPEPTRANSCEIVER_ENTRY, ">DPepTransceiver::DPepTransceiver;aPipeHandle=%hhu", aPipeHandle );
    C_TRACE( ( _T( "DPepTransceiver::DPepTransceiver pipehandle 0x%x <->" ), iPipeHandle ) );
    }

DPepTransceiver::~DPepTransceiver()
    {
    OstTrace0( TRACE_PIPE, DUP2_DPEPTRANSCEIVER_DPEPTRANSCEIVER_ENTRY, ">DPepTransceiver::~DPepTransceiver" );
    C_TRACE( ( _T( "DPepTransceiver::~DPepTransceiver ->" ) ) );
    iFlowCtrl = EIADFlowControlOn;
    iPipeHandle = 0x00;
    iChannelId = 0x0000;
    iRouter = NULL;
    iRcvChannel = NULL;
    C_TRACE( ( _T( "DPepTransceiver::~DPepTransceiver <-" ) ) );
    OstTrace0( TRACE_PIPE, DUP2_DPEPTRANSCEIVER_DPEPTRANSCEIVER_EXIT, "<DPepTransceiver::~DPepTransceiver" );
    }

void DPepTransceiver::SetFlowCtrl(
        TIADFlowControlStatus aFlow
        )
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SETFLOWCTRL_ENTRY, ">DPepTransceiver::SetFlowCtrl;aFlow=%x", ( TUint )&( aFlow ) );

    C_TRACE( ( _T( "DPepTransceiver::SetFlowCtrl > %d ->" ), aFlow ) );
    iRcvChannel->NotifyFlowCtrl( aFlow );
    C_TRACE( ( _T( "DPepTransceiver::SetFlowCtrl > %d <-" ), aFlow ) );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SETFLOWCTRL_EXIT, "<DPepTransceiver::SetFlowCtrl;aFlow=%x", ( TUint )&( aFlow ) );
    }
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
void DPepTransceiver::SetMyChannel(
        MIAD2ChannelApi* aCh
        )
    {

    C_TRACE( ( _T( "DPepTransceiver::SetMyChannel old 0x%x new 0x%x this 0x%x ->" ), iRcvChannel, aCh, this ) );
    OstTraceExt3( TRACE_PIPE, DPEPTRANSCEIVER_SETMYCHANNEL_ENTRY, ">DPepTransceiver::SetMyChannel;old=%x;new=%x;this=%x", (TUint)iRcvChannel, (TUint)aCh, (TUint)this );
    ASSERT_RESET_ALWAYS( aCh, 0 );//TODO
    iRcvChannel = aCh;
    C_TRACE( ( _T( "DPepTransceiver::SetMyChannel old 0x%x new 0x%x this 0x%x <-" ), iRcvChannel, aCh, this ) );
    OstTraceExt3( TRACE_PIPE, DPEPTRANSCEIVER_SETMYCHANNEL_EXIT, "<DPepTransceiver::SetMyChannel;old=%x;new=%x;this=%x", (TUint)iRcvChannel, (TUint)aCh, (TUint)this );

    }
#endif

DPepRx::~DPepRx()
    {
    OstTrace0( TRACE_PIPE, DPEPRX_DPEPRX_ENTRY, ">DPepRx::~DPepRx" );
    C_TRACE( ( _T( "DPepRx::~DPepRx ->" ) ) );
//TBR    iReceiveQueueCount = 0x00;
    C_TRACE( ( _T( "DPepRx::~DPepRx <-" ) ) );
    }

DPepTx::~DPepTx()
    {
    OstTrace0( TRACE_PIPE, DPEPTX_DPEPTX_ENTRY, ">DPepTx::~DPepTx" );
    C_TRACE( ( _T( "DPepTx::~DPepTx ->" ) ) );
    C_TRACE( ( _T( "DPepTx::~DPepTx <-" ) ) );
    }

DMultiRx::~DMultiRx()
    {
    OstTrace0( TRACE_PIPE, DMULTIRX_DMULTIRX_ENTRY, ">DMultiRx::~DMultiRx" );
    C_TRACE( ( _T( "DMultiRx::~DMultiRx ->" ) ) );
    iThresholdCount = 0x00;
#ifdef _DEBUG // TODO: remove debug flag from actual code.
    iGrantedCreditCount = 0x00;
#endif
    C_TRACE( ( _T( "DMultiRx::~DMultiRx <-" ) ) );
    OstTrace0( TRACE_PIPE, DMULTIRX_DMULTIRX_EXIT, "<DMultiRx::~DMultiRx" );
    }

DLegacyRx::~DLegacyRx()
    {
    OstTrace0( TRACE_PIPE, DLEGACYRX_DLEGACYRX_ENTRY, ">DLegacyRx::~DLegacyRx" );
    C_TRACE( ( _T( "DLegacyRx::~DLegacyRx ->" ) ) );
    iRxQueueCount = 0x00;
    C_TRACE( ( _T( "DLegacyRx::~DLegacyRx <-" ) ) );
    OstTrace0( TRACE_PIPE, DLEGACYRX_DLEGACYRX_EXIT, "<DLegacyRx::~DLegacyRx" );
    }

DOneRx::~DOneRx()
    {
    OstTrace0( TRACE_PIPE, DONERX_DONERX_ENTRY, ">DOneRx::~DOneRx" );
    C_TRACE( ( _T( "DOneRx::~DOneRx ->" ) ) );
    C_TRACE( ( _T( "DOneRx::DOneRx TBR pipehandle 0x%x" ), iPipeHandle ) ); // TBR

    C_TRACE( ( _T( "DOneRx::~DOneRx <-" ) ) );
    OstTrace0( TRACE_PIPE, DONERX_DONERX_EXIT, "<DOneRx::~DOneRx" );
    }

DMultiTx::~DMultiTx()
    {
    OstTrace0( TRACE_PIPE, DMULTITX_DMULTITX_ENTRY, ">DMultiTx::~DMultiTx" );
    C_TRACE( ( _T( "DMultiRx::~DMultiRx ->" ) ) );
    iGrantedCredits = 0x00;
    C_TRACE( ( _T( "DMultiRx::~DMultiRx <-" ) ) );
    OstTrace0( TRACE_PIPE, DMULTITX_DMULTITX_EXIT, "<DMultiTx::~DMultiTx" );
    }

DLegacyTx::~DLegacyTx()
    {
    OstTrace0( TRACE_PIPE, DLEGACYTX_DLEGACYTX_ENTRY, ">DLegacyTx::~DLegacyTx" );
    C_TRACE( ( _T( "DLegacyRx::~DLegacyRx ->" ) ) );
    C_TRACE( ( _T( "DLegacyRx::~DLegacyRx <-" ) ) );
    }

DOneTx::~DOneTx()
    {
    OstTrace0( TRACE_PIPE, DONETX_DONETX_ENTRY, ">DOneTx::~DOneTx" );
    C_TRACE( ( _T( "DOneTx::~DOneRx ->" ) ) );
    C_TRACE( ( _T( "DOneTx::~DOneRx <-" ) ) );
    }


TInt DPepTransceiver::SendPnsPepConnectResp(
        const TDesC8& aReq
        )
    {
    OstTraceExt2( TRACE_PIPE, DPEPTRANSCEIVER_SENDPNSPEPCONNECTRESP_ENTRY, ">DPepTransceiver::SendPnsPepConnectResp;aReq=%x;iPipeHandle=%x", ( TUint )&( aReq ), iPipeHandle );

    C_TRACE( ( _T( "DPepTransceiver::SendConnectResp -" ) ) );
    const TUint8* temp( aReq.Ptr() );    
    ASSERT_RESET_ALWAYS( aReq.Length() > ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE, EIADOverTheLimits | EIADFaultIdentifier14 << KFaultIdentifierShift );    
    iChannelId = GET_RECEIVER_OBJ( temp );
    iPipeHandle = temp[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE ]; // TODO: pipehandle checking
    C_TRACE( ( _T( "DPepTransceiver::SendConnectResp pipehandle 0x%x" ), iPipeHandle ) );
    const TInt KPnsPepConnectRespLength( ISI_HEADER_SIZE + SIZE_PNS_PEP_CONNECT_RESP + KPnPipeSbPreferredFcRx + KPnPipeSbPreferredFcTx );
    TDes8& resp = CreateCommonPepResp( aReq, KPnsPepConnectRespLength, PNS_PEP_CONNECT_RESP );
    TUint8* msgPtr( const_cast<TUint8*>( resp.Ptr() ) );
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_RESP_OFFSET_ERRORCODE ]   = PN_PIPE_NO_ERROR;// TODO: all pipes in use
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_RESP_OFFSET_FILLERBYTE1 ]                   = 0x00;
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_RESP_OFFSET_FILLERBYTE2 ]                   = 0x00;
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_RESP_OFFSET_FILLERBYTE3 ]                   = 0x00;
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_RESP_OFFSET_NSB ]                           = 0x02;
// SB0
    TUint16 headerAndMsgSize( ISI_HEADER_SIZE + SIZE_PNS_PEP_CONNECT_RESP );
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_SBID ]                = PN_PIPE_SB_PREFERRED_FC_RX;
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_SBLEN ]               = KPnPipeSbPreferredFcRx;
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRXLENGTH ] = 0x03;
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_FILLERBYTE1 ]         = 0x00;
    C_TRACE( ( _T( "DPepTransceiver::SendConnectResp preferred RX 1st multi 2nd legacy 3rd one" )) );
    OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_SENDPNSPEPCONNECTRESP_PREF_RX, "DPepTransceiver::SendPnsPepConnectResp preferred RX 1st muslt 2nd legacy 3rd one" );
    
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRX ]       = PN_MULTI_CREDIT_FLOW_CONTROL; // 1st
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRX + 1 ]   = PN_LEGACY_FLOW_CONTROL;       // 2nd
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRX + 2 ]   = PN_ONE_CREDIT_FLOW_CONTROL;   // 3rd
    msgPtr[ headerAndMsgSize + PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRX + 3 ]   = 0x00;                         // filler
//SB1
    TUint16 headerMsgAndSbSize( headerAndMsgSize + KPnPipeSbPreferredFcRx );
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_SBID ]                 = PN_PIPE_SB_REQUIRED_FC_TX;
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_SBLEN ]                = KPnPipeSbPreferredFcTx;
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTXLENGTH ]   = 0x03;
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_FILLERBYTE1 ]          = 0x00;
    C_TRACE( ( _T( "DPepTransceiver::SendConnectResp preferred TX 1st multi 2nd legacy 3rd one" )) );
    OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_SENDPNSPEPCONNECTRESP_PREF_TX, "DPepTransceiver::SendPnsPepConnectResp preferred TX 1st multi 2nd legacy 3rd one" );
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTX ]         = PN_MULTI_CREDIT_FLOW_CONTROL; // 1st
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTX + 1 ]     = PN_LEGACY_FLOW_CONTROL;       // 2nd
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTX + 2 ]     = PN_ONE_CREDIT_FLOW_CONTROL;   // 3rd
    msgPtr[ headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTX + 3 ]     = 0x00;                         // filler
    ASSERT_RESET_ALWAYS( resp.Length() > ( headerMsgAndSbSize + PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTX + 3 ), EIADOverTheLimits | EIADFaultIdentifier15 << KFaultIdentifierShift );    

    C_TRACE( ( _T( "DPepTransceiver::SendConnectResp <-" ) ) );
    TInt retVal = iRouter->SendMsg( resp );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDPNSPEPCONNECTRESP_EXIT, "<DPepTransceiver::SendPnsPepConnectResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPepTransceiver::SendResetResp(
        const TDesC8& aReq
        )
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDRESETRESP_ENTRY, ">DPepTransceiver::SendResetResp;aReq=%x", ( TUint )&( aReq ) );

    C_TRACE( ( _T( "DPepTransceiver::SendResetResp ->" ) ) );
    ASSERT_RESET_ALWAYS( aReq.Ptr()[ ISI_HEADER_SIZE + PNS_PIPE_RESET_RESP_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier2 << KFaultIdentifierShift );
    TDes8& resp = CreateCommonPepResp( aReq, KPnsPepRespLength, PNS_PEP_RESET_RESP);
    TUint8* msgPtr( const_cast<TUint8*>( resp.Ptr() ) );
    msgPtr[ KPnsPepRespPepErrorCode ] = PN_PIPE_NO_ERROR;
    ASSERT_RESET_ALWAYS( resp.Length() > ( KPnsPepRespPepErrorCode ), EIADOverTheLimits | EIADFaultIdentifier16 << KFaultIdentifierShift );        
    C_TRACE( ( _T( "DPepTransceiver::SendResetResp <-" ) ) );
    TInt retVal = iRouter->SendMsg( resp );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDRESETRESP_EXIT, "<DPepTransceiver::SendResetResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPepTransceiver::SendPnsPepDisconnectResp(
        const TDesC8& aReq
        )
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDPNSPEPDISCONNECTRESP_ENTRY, ">DPepTransceiver::SendPnsPepDisconnectResp;aReq=%x", ( TUint )&( aReq ) );

    C_TRACE( ( _T( "DPepTransceiver::SendDisconnectResp 0x%x ->" ), &aReq ) );
    ASSERT_RESET_ALWAYS( aReq.Ptr()[ ISI_HEADER_SIZE + PNS_PEP_DISCONNECT_RESP_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier3 << KFaultIdentifierShift );
    iRcvChannel->NotifyFlowCtrl( EIADTransmissionEnd );
    TUint16 size( ISI_HEADER_SIZE + SIZE_PNS_PEP_DISCONNECT_RESP );
    TDes8& resp = CreateCommonPepResp( aReq, size, PNS_PEP_DISCONNECT_RESP );
    TUint8* msgPtr( const_cast<TUint8*>( resp.Ptr() ) );
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_DISCONNECT_RESP_OFFSET_ERRORCODE ] = PN_PIPE_NO_ERROR;
    ASSERT_RESET_ALWAYS( resp.Length() > ( ISI_HEADER_SIZE + PNS_PEP_DISCONNECT_RESP_OFFSET_ERRORCODE ), EIADOverTheLimits | EIADFaultIdentifier17 << KFaultIdentifierShift );            
    C_TRACE( ( _T( "DPepTransceiver::SendDisconnectResp 0x%x <-" ), &aReq ) );
    TInt retVal = iRouter->SendMsg( resp );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDPNSPEPDISCONNECTRESP_EXIT, "<DPepTransceiver::SendPnsPepDisconnectResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPepTransceiver::SendEnableResp(
        const TDesC8& aReq
        )
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDENABLERESP_ENTRY, ">DPepTransceiver::SendEnableResp;aReq=%x", ( TUint )&( aReq ) );

    C_TRACE( ( _T( "DPepTransceiver::SendEnableResp ->" ) ) );
    ASSERT_RESET_ALWAYS( aReq.Ptr()[ ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier4 << KFaultIdentifierShift );
    TDes8& resp = CreateCommonPepResp(aReq, KPnsPepRespLength, PNS_PEP_ENABLE_RESP);
    TUint8* msgPtr( const_cast<TUint8*>( resp.Ptr() ) );
    msgPtr[ KPnsPepRespPepErrorCode ] = PN_PIPE_NO_ERROR;
    ASSERT_RESET_ALWAYS( resp.Length() > ( KPnsPepRespPepErrorCode ), EIADOverTheLimits | EIADFaultIdentifier18 << KFaultIdentifierShift );                
    C_TRACE( ( _T( "DPepTransceiver::SendEnableResp <-" ) ) );
    TInt retVal = iRouter->SendMsg( resp );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDENABLERESP_EXIT, "<DPepTransceiver::SendEnableResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPepTransceiver::SendDisableResp(
        const TDesC8& aReq
        )
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDDISABLERESP_ENTRY, ">DPepTransceiver::SendDisableResp;aReq=%x", ( TUint )&( aReq ) );

    C_TRACE( ( _T( "DPepTransceiver::SendDisableResp ->" ) ) );
    ASSERT_RESET_ALWAYS( aReq.Ptr()[ ISI_HEADER_SIZE + PNS_PEP_DISABLE_REQ_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier5 << KFaultIdentifierShift );
    TDes8& resp = CreateCommonPepResp(aReq, KPnsPepRespLength, PNS_PEP_DISABLE_RESP);
    TUint8* msgPtr( const_cast<TUint8*>( resp.Ptr() ) );
    msgPtr[ KPnsPepRespPepErrorCode ] = PN_PIPE_NO_ERROR;
    ASSERT_RESET_ALWAYS( resp.Length() > ( KPnsPepRespPepErrorCode ), EIADOverTheLimits | EIADFaultIdentifier19 << KFaultIdentifierShift );                
    C_TRACE( ( _T( "DPepTransceiver::SendDisableResp <-" ) ) );
    TInt retVal = iRouter->SendMsg( resp );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SENDDISABLERESP_EXIT, "<DPepTransceiver::SendDisableResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPepTransceiver::PepControlReq(
        const TDesC8& aReq
        )
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_PEPCONTROLREQ_ENTRY, ">DPepTransceiver::PepControlReq;aReq=%x", ( TUint )&( aReq ) );

    C_TRACE( ( _T( "DPepTransceiver::PepControlReq ->" ) ) );
    const TUint8* reqPtr( aReq.Ptr() );
    ASSERT_RESET_ALWAYS( reqPtr[ ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier6 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( reqPtr[ ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPTYPE ] == PN_PEP_TYPE_COMMON, EIADUnknownPipeOrPepMsg | EIADFaultIdentifier2 << KFaultIdentifierShift );
    TDes8& resp = CreateCommonPepResp( aReq, (ISI_HEADER_SIZE + SIZE_PNS_PEP_CTRL_RESP), PNS_PEP_CTRL_RESP );
    TUint8* msgPtr( const_cast<TUint8*>( resp.Ptr() ) );
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PEPCTRLID ] = reqPtr[ ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLID ];
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PEPTYPE ] = PN_PEP_TYPE_COMMON;// IAD Supports only common peps.
    msgPtr[ ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_ERRORCODE ] = PN_PIPE_NO_ERROR;
    ASSERT_RESET_ALWAYS( resp.Length() > ( ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_ERRORCODE ), EIADOverTheLimits | EIADFaultIdentifier43 << KFaultIdentifierShift );                
    C_TRACE( ( _T( "DPepTransceiver::PepControlReq <-" ) ) );
    TInt retVal =  iRouter->SendMsg( resp );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_PEPCONTROLREQ_EXIT, "<DPepTransceiver::PepControlReq;retVal=%d", retVal );
    return retVal;
    }

TInt DPepTransceiver::Receive(
        const TDesC8& aData
        )
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_RECEIVE_ENTRY, ">DPepTransceiver::Receive;aData=%x", ( TUint )&( aData ) );

    C_TRACE( ( _T( "DPepTransceiver::Receive ->" ) ) );
    ASSERT_RESET_ALWAYS( aData.Length() > ( ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_PIPEHANDLE ), EIADOverTheLimits | EIADFaultIdentifier20 << KFaultIdentifierShift );                
    ASSERT_RESET_ALWAYS( aData.Ptr()[ ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier7 << KFaultIdentifierShift );
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
    C_TRACE( ( _T( "DPepTransceiver::Receive new 0x%x this 0x%x" ), iRcvChannel, this ) );
    OstTraceExt2( TRACE_PIPE, DPEPTRANSCEIVER_RECEIVE, "DPepTransceiver::Receive;new=%x;this=%x", ( TUint )iRcvChannel, (TUint) this );
#endif
    iRcvChannel->ReceiveDataMsg( aData );//PipeHeader is extracted in Channel
    // TODO: if changed so that header is extracted here
    C_TRACE( ( _T( "DPepTransceiver::Receive <-" ) ) );
    TInt retVal = KErrNone;//TODO

    OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_RECEIVE_EXIT, "<DPepTransceiver::Receive" );
    return retVal;
    }

TInt DPepTransceiver::Send(TDes8& aMsg)
    {
	OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_SEND_ENTRY, ">DPepTransceiver::Send;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DPepTransceiver::Send <->" ) ) );
    TInt retVal = iRouter->SendMsg(aMsg);
    
    OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_SEND_EXIT, "<DPepTransceiver::Send" );
    return retVal;    
    }

TDes8& DPepTransceiver::CreateCommonPepResp(
        const TDesC8& aReq,
        const TUint16 aLength,
        const TUint8  aMsgId
        )
    {
	OstTraceExt3( TRACE_PIPE, DPEPTRANSCEIVER_CREATECOMMONPEPRESP_ENTRY, ">DPepTransceiver::CreateCommonPepResp;aReq=%x;aLength=%hu;aMsgId=%hhu", ( TUint )&( aReq ), aLength, aMsgId );

    C_TRACE( ( _T( "DPepTransceiver::CreateCommonPepResp 0x%x 0x%x %d ->" ), &aReq, aMsgId, aLength ) );
    TDes8& des = iRouter->AllocateBlock( aLength );
    des.SetLength( aLength );
    TUint8* req( const_cast<TUint8*>( aReq.Ptr() ) );
    TUint8* msgPtr( const_cast<TUint8*>( des.Ptr() ) );
    msgPtr[ ISI_HEADER_OFFSET_MEDIA ]           = req[ ISI_HEADER_OFFSET_MEDIA ];
    msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ]    = GET_RECEIVER_DEV( req );
    msgPtr[ ISI_HEADER_OFFSET_RECEIVERDEVICE ]  = GET_SENDER_DEV( req );
    msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ]      = req[ ISI_HEADER_OFFSET_RESOURCEID ];
    msgPtr[ ISI_HEADER_OFFSET_SENDEROBJECT ]    = GET_RECEIVER_OBJ( req );
    msgPtr[ ISI_HEADER_OFFSET_RECEIVEROBJECT ]  = GET_SENDER_OBJ( req );
    msgPtr[ ISI_HEADER_OFFSET_TRANSID ]         = req[ ISI_HEADER_OFFSET_TRANSID ];
    msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ]       = aMsgId;
    SET_LENGTH( msgPtr, aLength - PN_HEADER_SIZE ); // ISI length
    msgPtr[ ISI_HEADER_SIZE + KPepRespReqIndCommonPipeHandleOffset ] = iPipeHandle; // TODO: to offsets
    ASSERT_RESET_ALWAYS( des.Length() > ( ISI_HEADER_SIZE + KPepRespReqIndCommonPipeHandleOffset ), EIADOverTheLimits | EIADFaultIdentifier21 << KFaultIdentifierShift );                    
    C_TRACE( ( _T( "DPepTransceiver::CreateCommonPepResp 0x%x 0x%x %d <-" ), &aReq, aMsgId, aLength ) );

    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_CREATECOMMONPEPRESP_EXIT, "<DPepTransceiver::CreateCommonPepResp;des=%x", (TUint)&(des) );
    return des;
    }

void DPepTransceiver::RegisterDataMsgStatusCb(MDataMessageStatus* aCb)
    {
    OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_REGISTERDATAMSGSTATUSCB_ENTRY, ">DPepTransceiver::RegisterDataMsgStatusCb;aCb=%x", ( TUint )( aCb ) );
    C_TRACE( ( _T( "DPepTransceiver::RegisterDataMsgStatusCb ->" ) ) );
    iRcvChannel->RegisterPep(*aCb);
    C_TRACE( ( _T( "DPepTransceiver::RegisterDataMsgStatusCb <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_REGISTERDATAMSGSTATUSCB_EXIT, "<DPepTransceiver::RegisterDataMsgStatusCb" );
    }


TDes8& DPepTransceiver::AllocateDataBlock(TUint16 aSize)
    {
    OstTraceExt1( TRACE_PIPE, DPEPTRANSCEIVER_ALLOCATEDATABLOCK_ENTRY, ">DPepTransceiver::AllocateDataBlock;aSize=%hu", aSize );
    C_TRACE( ( _T( "DPepTransceiver::AllocateDataBlock ->" ) ) );
    C_TRACE( ( _T( "DPepTransceiver::AllocateDataBlock <-" ) ) );
    return iRouter->AllocateBlock( aSize);
    }


DOneTx::DOneTx(DPepTransceiver& aSuper, TUint8 aPipeHandle) : DPepTx(aSuper, aPipeHandle )
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DONETX_DONETX_ENTRY, ">DOneTx::DOneTx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DOneTx::DOneTx <->" ) ) );
    }

DMultiTx::DMultiTx(DPepTransceiver& aSuper, TUint8 aPipeHandle) : DPepTx(aSuper, aPipeHandle )
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DMULTITX_DMULTITX_ENTRY, ">DMultiTx::DMultiTx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DMultiTx::DMultiTx <->" ) ) );
    }

DLegacyTx::DLegacyTx(DPepTransceiver& aSuper, TUint8 aPipeHandle) : DPepTx(aSuper, aPipeHandle )
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DLEGACYTX_DLEGACYTX_ENTRY, ">DLegacyTx::DLegacyTx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DLegacyTx::DLegacyTx <->" ) ) );
    }

DOneRx::DOneRx( DPepTransceiver& aSuper, TUint8 aPipeHandle ) : DPepRx(aSuper, aPipeHandle )
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DONERX_DONERX_ENTRY, ">DOneRx::DOneRx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DOneRx::DOneRx ->" ) ) );
    C_TRACE( ( _T( "DOneRx::DOneRx TBR pipehandle 0x%x" ), iPipeHandle ) ); // TBR
    iSuper.RegisterDataMsgStatusCb(this);
    C_TRACE( ( _T( "DOneRx::DOneRx <-" ) ) );
    OstTrace0( TRACE_PIPE, DUP1_DONERX_DONERX_EXIT, "<DOneRx::DOneRx" );
    }

DMultiRx::DMultiRx(DPepTransceiver& aSuper, TUint8 aPipeHandle) : DPepRx(aSuper, aPipeHandle )
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DMULTIRX_DMULTIRX_ENTRY, ">DMultiRx::DMultiRx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DMultiRx::DMultiRx ->" ) ) );
    iSuper.RegisterDataMsgStatusCb(this);
    C_TRACE( ( _T( "DMultiRx::DMultiRx <-" ) ) );
    OstTrace0( TRACE_PIPE, DUP1_DMULTIRX_DMULTIRX_EXIT, "<DMultiRx::DMultiRx" );
    }

DLegacyRx::DLegacyRx(DPepTransceiver& aSuper, TUint8 aPipeHandle) : DPepRx(aSuper, aPipeHandle )
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DLEGACYRX_DLEGACYRX_ENTRY, ">DLegacyRx::DLegacyRx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DLegacyRx::DLegacyRx ->" ) ) );
    iSuper.RegisterDataMsgStatusCb(this);
    C_TRACE( ( _T( "DLegacyRx::DLegacyRx <-" ) ) );
    OstTrace0( TRACE_PIPE, DUP1_DLEGACYRX_DLEGACYRX_EXIT, "<DLegacyRx::DLegacyRx" );
    }

DPepRx::DPepRx(DPepTransceiver& aSuper, TUint8 aPipeHandle) : DPepTransceiver(aPipeHandle), iSuper(aSuper)
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DPEPRX_DPEPRX_ENTRY, ">DPepRx::DPepRx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DPepRx::DPepRx ->" ) ) );
    C_TRACE( ( _T( "DPepRx::DPepRx <-" ) ) );
    }

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
void DPepRx::SetNewChannel(
        MIAD2ChannelApi* aCh
        )
    {

    C_TRACE( ( _T( "DPepRx::SetNewChannel new 0x%x this 0x%x ->" ), aCh, this ) );
    OstTraceExt2( TRACE_PIPE, DUP1_DPEPRX_SETNEWCHANNEL_ENTRY, ">DPepRx::SetNewChannel;new=%x;this=%x", (TUint)aCh, (TUint)this );
    iSuper.SetMyChannel( aCh );
    iSuper.RegisterDataMsgStatusCb(this);
    C_TRACE( ( _T( "DPepRx::SetNewChannel new 0x%x this 0x%x <-" ), aCh, this ) );
    OstTraceExt2( TRACE_PIPE, DUP1_DPEPRX_SETNEWCHANNEL_EXIT, "<DPepRx::SetNewChannel;new=%x;this=%x", (TUint)aCh, (TUint)this );

    }
#endif
// same behavior in all pipe types -> to base class   
// Actually not same behaviour -> child classes
DPepTx::DPepTx(DPepTransceiver& aSuper, TUint8 aPipeHandle) : DPepTransceiver(aPipeHandle), iSuper(aSuper)
    {
    OstTraceExt2( TRACE_PIPE, DUP1_DPEPTX_DPEPTX_ENTRY, ">DPepTx::DPepTx;aSuper=%x;aPipeHandle=%hhu", ( TUint )&( aSuper ), aPipeHandle );
    C_TRACE( ( _T( "DPepTx::DPepTx ->" ) ) );
    C_TRACE( ( _T( "DPepTx::DPepTx <-" ) ) );
    }

void DPepTx::SetFlowCtrl(
        TIADFlowControlStatus aFlow
        )
    {
	OstTrace1( TRACE_PIPE, DPEPTX_SETFLOWCTRL_ENTRY, ">DPepTx::SetFlowCtrl;aFlow=%x", ( TUint )&( aFlow ) );

    C_TRACE( ( _T( "DPepTx::SetFlowCtrl from %d to %d ->" ), iFlowCtrl, aFlow ) );
    iSuper.SetFlowCtrl( aFlow );
    iFlowCtrl = aFlow;
    C_TRACE( ( _T( "DPepTx::SetFlowCtrl %d %d <-" ), iFlowCtrl, aFlow ) );

    OstTrace0( TRACE_PIPE, DPEPTX_SETFLOWCTRL_EXIT, "<DPepTx::SetFlowCtrl" );
    }

void DOneTx::PipeEnabled(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DONETX_PIPEENABLED_ENTRY, "<>DOneTx::PipeEnabled" );

    C_TRACE( ( _T( "DOneTx::PipeEnabled ->" ) ) );
    // Do not set flow control off before credits are got.
    C_TRACE( ( _T( "DOneTx::PipeEnabled <-" ) ) );

    }

void DMultiTx::PipeEnabled(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DMULTITX_PIPEENABLED_ENTRY, ">DMultiTx::PipeEnabled" );

    C_TRACE( ( _T( "DMultiTx::PipeEnabled ->" ) ) );
    // Do not set flow control off before credits are got.
    if( iGrantedCredits > 0 && EIADFlowControlOn == iFlowCtrl )
        {
        SetFlowCtrl( EIADFlowControlOff );
        //iSuper.SetFlowCtrl( EIADFlowControlOff );
        }
    C_TRACE( ( _T( "DMultiTx::PipeEnabled <-" ) ) );

    OstTrace0( TRACE_PIPE, DMULTITX_PIPEENABLED_EXIT, "<DMultiTx::PipeEnabled" );
    }

void DLegacyTx::PipeEnabled(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DLEGACYTX_PIPEENABLED_ENTRY, ">DLegacyTx::PipeEnabled" );

    C_TRACE( ( _T( "DLegacyTx::PipeEnabled ->" ) ) );
    //iSuper.SetFlowCtrl( EIADFlowControlOff );
    SetFlowCtrl( EIADFlowControlOff );
    C_TRACE( ( _T( "DLegacyTx::PipeEnabled <-" ) ) );

    OstTrace0( TRACE_PIPE, DLEGACYTX_PIPEENABLED_EXIT, "<DLegacyTx::PipeEnabled" );
    }

//same for legacy and one
void DPepTx::PipeDisabled()
    {
    OstTrace0( TRACE_PIPE, DPEPTX_PIPEDISABLED_ENTRY, ">DPepTx::PipeDisabled" );
    C_TRACE( ( _T( "DPepTx::PipeDisabled ->" ) ) );
    //iSuper.SetFlowCtrl( EIADFlowControlOn );
    SetFlowCtrl( EIADFlowControlOn );
    C_TRACE( ( _T( "DPepTx::PipeDisabled <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEPTX_PIPEDISABLED_EXIT, "<DPepTx::PipeDisabled" );
    }

void DMultiTx::PipeDisabled(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DMULTITX_PIPEDISABLED_ENTRY, ">DMultiTx::PipeDisabled" );

    C_TRACE( ( _T( "DMultiTx::PipeDisabled ->" ) ) );
    iGrantedCredits = 0x00;
    C_TRACE( ( _T( "DMultiTx::PipeDisabled discard TX credits 0x%x" ), iGrantedCredits ) );
    //iSuper.SetFlowCtrl( EIADFlowControlOn );
    SetFlowCtrl( EIADFlowControlOn );
    C_TRACE( ( _T( "DMultiTx::PipeDisabled <-" ) ) );

    OstTrace0( TRACE_PIPE, DMULTITX_PIPEDISABLED_EXIT, "<DMultiTx::PipeDisabled" );
    }


void DOneTx::PepStatusInd(
        const TDesC8& aMessage
        )
    {
	OstTrace1( TRACE_PIPE, DONETX_PEPSTATUSIND_ENTRY, ">DOneTx::PepStatusInd;aMessage=%x", ( TUint )&( aMessage ) );

    C_TRACE( ( _T("DOneTx::PepStatusInd 0x%x ->" ), &aMessage ) );
    const TUint8* ptr( aMessage.Ptr() );
    // TODO: pipehandle info traceen
    C_TRACE( ( _T("DOneTx::PepStatusInd msghandle 0x%x pipehandle 0x%x" ), ptr[ ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PIPEHANDLE ], iPipeHandle ) );
    ASSERT_RESET_ALWAYS( ptr[ ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier8 << KFaultIdentifierShift | (TUint8)iSuper.ChannelId()<<KChannelNumberShift);
    TRACE_ASSERT_INFO( ptr[ KPnsPepStatusIndIndicationId ] == PN_PEP_IND_FLOW_CONTROL, (TUint8)iSuper.ChannelId()<<KChannelNumberShift );
    TRACE_ASSERT_INFO( iFlowCtrl == EIADFlowControlOn, (TUint8)iSuper.ChannelId()<<KChannelNumberShift );
    ASSERT_RESET_ALWAYS( aMessage.Length() > ( KPnsPepStatusIndIndicationData ), EIADOverTheLimits | EIADFaultIdentifier22 << KFaultIdentifierShift );                    
    switch( ptr[ KPnsPepStatusIndIndicationData ] )
        { 
        case PEP_IND_READY:
            {
            C_TRACE( ( _T("DOneTx::PepStatusInd got one credit" ) ) );
            OstTrace0( TRACE_PIPE, DONETX_PEPSTATUSIND_ONE_CREDIT, "DOneTx::PepStatusInd got one credit" );            
            //iSuper.SetFlowCtrl( EIADFlowControlOff );
            SetFlowCtrl( EIADFlowControlOff );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADNotSupported | EIADFaultIdentifier2 << KFaultIdentifierShift );
            break;
            }
        }
    C_TRACE( ( _T("DOneTx::PepStatusInd <-" ) ) );

    OstTrace0( TRACE_PIPE, DONETX_PEPSTATUSIND_EXIT, "<DOneTx::PepStatusInd" );
    }

void DMultiTx::PepStatusInd(
        const TDesC8& aMessage
        )
    {
	OstTrace1( TRACE_PIPE, DMULTITX_PEPSTATUSIND_ENTRY, ">DMultiTx::PepStatusInd;aMessage=%x", ( TUint )&( aMessage ) );

    C_TRACE( ( _T( "DMultiTx::HandlePnsPepStatusInd ->"), &aMessage) );
    const TUint8* ptr( aMessage.Ptr() );
    ASSERT_RESET_ALWAYS( aMessage.Length() > ( KPnsPepStatusIndIndicationId ), EIADOverTheLimits | EIADFaultIdentifier23 << KFaultIdentifierShift );                        
    ASSERT_RESET_ALWAYS( ptr[ ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier9 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( ( ptr[ KPnsPepStatusIndIndicationId ] == PN_PEP_IND_MCFC_GRANT_CREDITS ), EIADWrongParameter | EIADFaultIdentifier8 << KFaultIdentifierShift );
    const TUint8 granted( ptr[ KPnsPepStatusIndIndicationData ] );
    C_TRACE( ( _T("DMultiTx::PepStatusInd TX credits old 0x%x granted 0x%x"), iGrantedCredits, granted ) );
    // Assert if granted is zero, because then FlowControl setting don't work.
    ASSERT_RESET_ALWAYS( 0x00 < granted, EIADWrongPipeHandle | EIADFaultIdentifier10 << KFaultIdentifierShift );
    iGrantedCredits += granted;
    C_TRACE( ( _T("DMultiTx::PepStatusInd TX credits new 0x%x"), iGrantedCredits ) );
    OstTraceExt1( TRACE_PIPE, DMULTITX_PEPSTATUSIND_GRANTED, "DMultiTx::PepStatusInd TX credits new;iGrantedCredits=%hu", iGrantedCredits );
    
    if( EIADFlowControlOn == iFlowCtrl )
        {
        SetFlowCtrl( EIADFlowControlOff );
        //iSuper.SetFlowCtrl( EIADFlowControlOff );
        }
    C_TRACE( ( _T("DMultiTx::PepStatusInd <-") ) );
    
    OstTrace0( TRACE_PIPE, DMULTITX_PEPSTATUSIND_EXIT, "<DMultiTx::PepStatusInd" );
    }

void DLegacyTx::PepStatusInd(
        const TDesC8& aMessage
        )
    {
	OstTrace1( TRACE_PIPE, DLEGACYTX_PEPSTATUSIND_ENTRY, ">DLegacyTx::PepStatusInd;aMessage=%x", ( TUint )&( aMessage ) );

    C_TRACE( ( _T( "DLegacyTx::PepStatusInd ->" ) ) );
    const TUint8* ptr( aMessage.Ptr() );
    ASSERT_RESET_ALWAYS( aMessage.Length() > ( KPnsPepStatusIndIndicationData ), EIADOverTheLimits | EIADFaultIdentifier24 << KFaultIdentifierShift );                            
    TRACE_ASSERT_INFO( ptr[ KPnsPepStatusIndIndicationId ] == PN_PEP_IND_FLOW_CONTROL, (TUint8)iSuper.ChannelId()<<KChannelNumberShift );
    C_TRACE( ( _T("DIscIsiPep::HandlePnsPepStatusInd IndicationData 0x%x" ), ptr[ KPnsPepStatusIndIndicationData ] ) );
    switch( ptr[ KPnsPepStatusIndIndicationData ] )
        { 
        case PEP_IND_EMPTY:
        case PEP_IND_READY:
            {
            SetFlowCtrl( EIADFlowControlOff );
            //iSuper.SetFlowCtrl( EIADFlowControlOff );
            break;
            }
        case PEP_IND_BUSY:
            {
            SetFlowCtrl( EIADFlowControlOn );
            //iSuper.SetFlowCtrl( EIADFlowControlOn );
            break;
            }
        default:
            {
            // TODO How about the PEP_IND_CREDIT?
            ASSERT_RESET_ALWAYS( 0, EIADNotSupported | EIADFaultIdentifier1 << KFaultIdentifierShift | (TUint8)iSuper.ChannelId()<<KChannelNumberShift );
            break;
            }
        }
    C_TRACE( ( _T( "DLegacyTx::PepStatusInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DLEGACYTX_PEPSTATUSIND_EXIT, "<DLegacyTx::PepStatusInd" );
    }

    // From MDataMessageStatus
void DOneRx::DataMessageDelivered()
    {
    OstTrace0( TRACE_PIPE, DONERX_DATAMESSAGEDELIVERED_ENTRY, ">DOneRx::DataMessageDelivered" );
    C_TRACE( ( _T( "DOneRx::DataMessageDelivered ->" ) ) );
    ASSERT_DFCTHREAD_INEXT();
    SendOneCredit();
    C_TRACE( ( _T( "DOneRx::DataMessageDelivered <->" ) ) );
    OstTrace0( TRACE_PIPE, DONERX_DATAMESSAGEDELIVERED_EXIT, "<DOneRx::DataMessageDelivered" );
    }

void DLegacyRx::DataMessageDelivered(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DLEGACYRX_DATAMESSAGEDELIVERED_ENTRY, ">DLegacyRx::DataMessageDelivered" );

    C_TRACE( ( _T( "DLegacyRx::DataMessageDelivered %d %d %d ->" ), iRxQueueCount, KIADChDataRxLowWaterMark, iFlowCtrl ) );
    ASSERT_DFCTHREAD_INEXT();
    iRxQueueCount--;
    // TODO: remove iFlowCtrl (UL) not datames.. counts DL direction messages
    if( iFlowCtrl == EIADFlowControlOn && iRxQueueCount == KIADChDataRxLowWaterMark )
        {
        SendPnsPepStatusInd( EIADFlowControlOff );
        //iFlowCtrl = EIADFlowControlOff;// EFalse;
        }
    C_TRACE( ( _T( "DLegacyRx::DataMessageDelivered %d %d %d <-" ), iRxQueueCount, KIADChDataRxLowWaterMark, iFlowCtrl ) );

    OstTrace0( TRACE_PIPE, DLEGACYRX_DATAMESSAGEDELIVERED_EXIT, "<DLegacyRx::DataMessageDelivered" );
    }

void DMultiRx::DataMessageDelivered()
    {
    OstTrace0( TRACE_PIPE, DMULTIRX_DATAMESSAGEDELIVERED_ENTRY, ">DMultiRx::DataMessageDelivered" );
    C_TRACE( ( _T( "DMultiRx::DataMessageDelivered %d ->" ), iThresholdCount ) );
    ASSERT_DFCTHREAD_INEXT();
    iThresholdCount++;
#ifdef _DEBUG
    TRACE_ASSERT_INFO( iGrantedCreditCount > 0, (TUint8)iSuper.ChannelId()<<KChannelNumberShift ); // used only to verify that anyone does not send us too much data
    iGrantedCreditCount--;
#endif    
    if(iThresholdCount == CREDIT_THRESHOLD )
        {
        SendCredits( CREDIT_THRESHOLD );
        iThresholdCount = 0;
#ifdef _DEBUG
        iGrantedCreditCount += CREDIT_THRESHOLD; // used only to verify that anyone does not send us too much data
#endif    
        }
    C_TRACE( ( _T( "DMultiRx::DataMessageDelivered %d <-" ), iThresholdCount ) );
    OstTrace0( TRACE_PIPE, DMULTIRX_DATAMESSAGEDELIVERED_EXIT, "<DMultiRx::DataMessageDelivered" );
    }

TInt DMultiRx::Receive(const TDesC8& aData)
    {
    OstTrace1( TRACE_PIPE, DMULTIRX_RECEIVE_ENTRY, ">DMultiRx::Receive;aData=%x", ( TUint )&( aData ) );
    C_TRACE( ( _T( "DMultiRx::Receive ->" ) ) );
    iSuper.Receive(aData); 
    return KErrNone;
    }

TInt DOneRx::Receive(
        const TDesC8& aData
        )
    {
	OstTrace1( TRACE_PIPE, DONERX_RECEIVE_ENTRY, ">DOneRx::Receive;aData=%x", ( TUint )&( aData ) );

    C_TRACE( ( _T( "DOneRx::Receive ->" ) ) );
    C_TRACE( ( _T( "DOneRx::DOneRx TBR pipehandle 0x%x" ), iPipeHandle ) ); // TBR
    iSuper.Receive(aData); 
    C_TRACE( ( _T( "DOneRx::Receive <-" ) ) );
    return KErrNone;//TODO

    }

TInt DLegacyRx::Receive(
        const TDesC8& aData
        )
    {
	OstTrace1( TRACE_PIPE, DLEGACYRX_RECEIVE_ENTRY, ">DLegacyRx::Receive;aData=%x", ( TUint )&( aData ) );

    C_TRACE( ( _T( "DLegacyRx::Receive %d %d %d ->" ), iRxQueueCount, KIADChDataRxHighWaterMark, iFlowCtrl ) );
    iSuper.Receive(aData); // clients receive queue may overflow
    iRxQueueCount++;
    if( iRxQueueCount == KIADChDataRxHighWaterMark )
        {
        SendPnsPepStatusInd( EIADFlowControlOn );
//        iFlowCtrl = EIADFlowControlOn;//ETrue;//TODO remove iFlowCtrl from DL direction it's for UL
        }
    C_TRACE( ( _T( "DLegacyRx::Receive %d %d %d <-" ), iRxQueueCount, KIADChDataRxHighWaterMark, iFlowCtrl ) );
    OstTraceExt3( TRACE_PIPE, DLEGACYRX_RECEIVE_EXIT, "<DLegacyRx::Receive;iRxQueueCount=%hu;KIADChDataRxHighWaterMark=%hu;iFlowCtrl=%x", iRxQueueCount, KIADChDataRxHighWaterMark, (TUint)(iFlowCtrl) );
    
    return KErrNone;// TODO: something reasonable here?

    }

void DOneRx::PipeEnabled(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DONERX_PIPEENABLED_ENTRY, ">DOneRx::PipeEnabled" );

    C_TRACE( ( _T( "DOneRx::PipeEnabled ->" ) ) );
    C_TRACE( ( _T( "DOneRx::DOneRx TBR pipehandle 0x%x" ), iPipeHandle ) ); // TBR
    // TODO: Not send when enabled->enabled
    SendOneCredit();
    C_TRACE( ( _T( "DOneRx::PipeEnabled <-" ) ) );

    OstTrace0( TRACE_PIPE, DONERX_PIPEENABLED_EXIT, "<DOneRx::PipeEnabled" );
    }

void DMultiRx::PipeEnabled(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DMULTIRX_PIPEENABLED_ENTRY, ">DMultiRx::PipeEnabled" );

    C_TRACE( ( _T( "DMultiRx::PipeEnabled %d ->" ), iThresholdCount ) );
    // TODO: Not send when enabled->enabled
    SendCredits( CREDIT_AMOUNT );
#ifdef _DEBUG // TODO: WTF why only in debug?
    iGrantedCreditCount = CREDIT_AMOUNT;
#endif
    iThresholdCount = 0;
    C_TRACE( ( _T( "DMultiRx::PipeEnabled %d <-" ), iThresholdCount ) );

    OstTrace0( TRACE_PIPE, DMULTIRX_PIPEENABLED_EXIT, "<DMultiRx::PipeEnabled" );
    }

void DLegacyRx::PipeEnabled()
    {
    OstTrace0( TRACE_PIPE, DLEGACYRX_PIPEENABLED_ENTRY, ">DLegacyRx::PipeEnabled" );
    C_TRACE( ( _T( "DLegacyRx::PipeEnabled ->" ) ) );
    SendPnsPepStatusInd( EIADFlowControlOff ); // Added due to CSD server not sending data without flowcontrol off indication.
    C_TRACE( ( _T( "DLegacyRx::PipeEnabled <-" ) ) );
    OstTrace0( TRACE_PIPE, DLEGACYRX_PIPEENABLED_EXIT, "<DLegacyRx::PipeEnabled" );
    }


void DOneRx::PipeDisabled()
    {
    OstTrace0( TRACE_PIPE, DONERX_PIPEDISABLED_ENTRY, ">DOneRx::PipeDisabled" );
    C_TRACE( ( _T( "DOneRx::PipeDisabled ->" ) ) );
    C_TRACE( ( _T( "DOneRx::DOneRx TBR pipehandle 0x%x" ), iPipeHandle ) ); // TBR
    C_TRACE( ( _T( "DOneRx::PipeDisabled <-" ) ) );
    }

void DMultiRx::PipeDisabled()
    {
    OstTrace0( TRACE_PIPE, DMULTIRX_PIPEDISABLED_ENTRY, ">DMultiRx::PipeDisabled" );
    C_TRACE( ( _T( "DMultiRx::PipeDisabled ->" ) ) );
#ifdef _DEBUG
    iGrantedCreditCount = 0;
#endif
    C_TRACE( ( _T( "DMultiRx::PipeDisabled <-" ) ) );
    }

void DLegacyRx::PipeDisabled()
    {
    OstTrace0( TRACE_PIPE, DLEGACYRX_PIPEDISABLED_ENTRY, ">DLegacyRx::PipeDisabled" );
    C_TRACE( ( _T( "DLegacyRx::PipeDisabled ->" ) ) );
    C_TRACE( ( _T( "DLegacyRx::PipeDisabled <-" ) ) );
    }

void DLegacyRx::SendPnsPepStatusInd(
        TIADFlowControlStatus aFlowCtrlStatus
        )
    {
	OstTrace1( TRACE_PIPE, DLEGACYRX_SENDPNSPEPSTATUSIND_ENTRY, ">DLegacyRx::SendPnsPepStatusInd;aFlowCtrlStatus=%x", ( TUint )&( aFlowCtrlStatus ) );

    C_TRACE( ( _T( "DLegacyRx::SendPnsPepStatusInd ->" ) ) );
    TUint8 length( SIZE_PNS_PEP_STATUS_IND + ISI_HEADER_SIZE - PN_HEADER_SIZE );
    TDes8& tempPtr = iSuper.AllocateDataBlock( SIZE_PNS_PEP_STATUS_IND + ISI_HEADER_SIZE );

    TUint8* msgPtr( const_cast<TUint8*>( tempPtr.Ptr() ) );

    //We start to append from transaction id
    tempPtr.SetLength( ISI_HEADER_OFFSET_TRANSID );

    msgPtr[ ISI_HEADER_OFFSET_MEDIA  ] = PN_MEDIA_SOS;
    SET_RECEIVER_DEV(msgPtr, OTHER_DEVICE_1);
    SET_SENDER_DEV(msgPtr, THIS_DEVICE);
    msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_PIPE;
    SET_LENGTH( msgPtr, length );
    SET_RECEIVER_OBJ( msgPtr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( msgPtr, iSuper.ChannelId() );

    tempPtr.Append( 0x00 );                    // transaction id
    tempPtr.Append( PNS_PEP_STATUS_IND );      // message id
    tempPtr.Append( iPipeHandle );             // pipe handle
    tempPtr.Append( PN_PEP_TYPE_COMMON );      // pep type
    tempPtr.Append( PN_PEP_IND_FLOW_CONTROL );
    tempPtr.Append( 0x00 );                    // reserved
    tempPtr.Append( 0x00 );
    if( aFlowCtrlStatus != EIADFlowControlOff )
        {
        tempPtr.Append( PEP_IND_BUSY );
        OstTrace0( TRACE_PIPE, DLEGACYRX_SENDPNSPEPSTATUSIND_BUSY, "DLegacyRx::SendPnsPepStatusInd PEP_IND_BUSY" );
        
        }
    else
        {
        tempPtr.Append( PEP_IND_READY ); 
        OstTrace0( TRACE_PIPE, DLEGACYRX_SENDPNSPEPSTATUSIND_READY, "DLegacyRx::SendPnsPepStatusInd PEP_IND_READY" );
        }
    iFlowCtrl = aFlowCtrlStatus;
    // send to ISA
    iSuper.Send(tempPtr);
    C_TRACE( ( _T( "DLegacyRx::SendPnsPepStatusInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DLEGACYRX_SENDPNSPEPSTATUSIND_EXIT, "<DLegacyRx::SendPnsPepStatusInd" );
    }

void DOneRx::SendOneCredit(
        // None
        )
    {
	OstTrace0( TRACE_PIPE, DONERX_SENDONECREDIT_ENTRY, ">DOneRx::SendOneCredit" );

    C_TRACE( ( _T( "DOneRx::SendOneCredit 0x%x ->" ), this ) );
    C_TRACE( ( _T( "DOneRx::DOneRx TBR pipehandle 0x%x" ), iPipeHandle ) ); // TBR

    TUint8 length( SIZE_PNS_PEP_STATUS_IND + ISI_HEADER_SIZE - PN_HEADER_SIZE );
    TDes8& tempPtr = iSuper.AllocateDataBlock( SIZE_PNS_PEP_STATUS_IND + ISI_HEADER_SIZE );
    //We start to append from transaction id
    tempPtr.SetLength( ISI_HEADER_OFFSET_TRANSID );
    TUint8* msgPtr( const_cast<TUint8*>( tempPtr.Ptr() ) );
    msgPtr[ ISI_HEADER_OFFSET_MEDIA  ] = PN_MEDIA_SOS;
    SET_RECEIVER_DEV( msgPtr, OTHER_DEVICE_1);
    SET_SENDER_DEV( msgPtr, THIS_DEVICE);
    msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_PIPE;
    SET_LENGTH( msgPtr, length );
    SET_RECEIVER_OBJ( msgPtr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( msgPtr, iSuper.ChannelId() );

    tempPtr.Append( 0x00 );                    // transaction id
    tempPtr.Append( PNS_PEP_STATUS_IND );      // message id
    tempPtr.Append( iPipeHandle );             // pipe handle
    tempPtr.Append( PN_PEP_TYPE_COMMON );      // pep type
    tempPtr.Append( PN_PEP_IND_FLOW_CONTROL ); // Indication ID
    tempPtr.Append( 0x00 );                    // filler
    tempPtr.Append( 0x00 );                    // filler
    tempPtr.Append( PEP_IND_READY );           // Indication data
    iSuper.Send(tempPtr);
    C_TRACE( ( _T( "DOneRx::SendOneCredit <-" ) ) );

    OstTrace0( TRACE_PIPE, DONERX_SENDONECREDIT_EXIT, "<DOneRx::SendOneCredit" );
    }

void DMultiRx::SendCredits(
        TUint8 aNum // TODO : const
        )
    {
	OstTraceExt1( TRACE_PIPE, DMULTIRX_SENDCREDITS_ENTRY, ">DMultiRx::SendCredits;aNum=%hhu", aNum );

    C_TRACE( ( _T( "DMultiRx::SendCredits %d ->" ), aNum ) );
    TUint8 length( SIZE_PNS_PEP_STATUS_IND + ISI_HEADER_SIZE - PN_HEADER_SIZE );
    TDes8& tempPtr = iSuper.AllocateDataBlock( SIZE_PNS_PEP_STATUS_IND + ISI_HEADER_SIZE );
    //We start to append from transaction id
    tempPtr.SetLength( ISI_HEADER_OFFSET_TRANSID );
    TUint8* msgPtr( const_cast<TUint8*>( tempPtr.Ptr() ) );
    msgPtr[ ISI_HEADER_OFFSET_MEDIA  ]      = PN_MEDIA_SOS;
    SET_RECEIVER_DEV( msgPtr, OTHER_DEVICE_1);
    SET_SENDER_DEV( msgPtr, THIS_DEVICE);
    msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ]  = PN_PIPE;
    SET_LENGTH( msgPtr, length );
    SET_RECEIVER_OBJ( msgPtr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( msgPtr, iSuper.ChannelId() );

    tempPtr.Append( 0x00 );                          // transaction id
    tempPtr.Append( PNS_PEP_STATUS_IND );            // message id
    tempPtr.Append( iPipeHandle );                   // pipe handle
    tempPtr.Append( PN_PEP_TYPE_COMMON );            // pep type
    tempPtr.Append( PN_PEP_IND_MCFC_GRANT_CREDITS ); // indication id in MULTI must
    tempPtr.Append( 0x00 );                         // filler
    tempPtr.Append( 0x00 );                         // filler
    tempPtr.Append( aNum );                      // MCBFC granted credits.
    iSuper.Send(tempPtr);
    C_TRACE( ( _T( "DMultiRx::SendCredits %d <-" ), aNum ) );

    OstTrace0( TRACE_PIPE, DMULTIRX_SENDCREDITS_EXIT, "<DMultiRx::SendCredits" );
    }

// ********************* HOX! ***********************
// Here we assume that block is reserved using AllocateDataBlock!!! == there is 12 bytes in front

// HOX HOX: AllocateDataBlock done and it set's most of the information. senderobj and handle left for pep.

void DPepTransceiver::FillIsiHeader(
        TDes8& aData
        )
    {
	OstTrace1( TRACE_PIPE, DPEPTRANSCEIVER_FILLISIHEADER_ENTRY, ">DPepTransceiver::FillIsiHeader;aData=%x", ( TUint )&( aData ) );

    C_TRACE( ( _T( "DPepTransceiver::FillIsiHeader ->" ) ) );
    TUint8* msgPtr( const_cast<TUint8*>( aData.Ptr() ) );
    SET_SENDER_OBJ( msgPtr, iChannelId );
    //Assumption is only for data!
    ASSERT_RESET_ALWAYS( msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] == PNS_PIPE_DATA, EIADNotSupported | EIADFaultIdentifier3 << KFaultIdentifierShift | (TUint8)ChannelId()<<KChannelNumberShift );
    msgPtr[ ISI_HEADER_OFFSET_TRANSID ] = 0x00;
    msgPtr[ ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_PIPEHANDLE ] = iPipeHandle;
    ASSERT_RESET_ALWAYS( aData.Length() > ( ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_PIPEHANDLE ), EIADOverTheLimits | EIADFaultIdentifier25 << KFaultIdentifierShift );                            
    
    C_TRACE( ( _T( "DPepTransceiver::FillIsiHeader <-" ) ) );

    OstTrace0( TRACE_PIPE, DPEPTRANSCEIVER_FILLISIHEADER_EXIT, "<DPepTransceiver::FillIsiHeader" );
    }

TInt DOneTx::Send(
        TDes8& aData
        )
    {
	OstTrace1( TRACE_PIPE, DONETX_SEND_ENTRY, ">DOneTx::Send;aData=%x", ( TUint )&( aData ) );

    C_TRACE( ( _T( "DOneTx::Send ->" ) ) );
    TInt retVal(KErrNone);
    if( iFlowCtrl == EIADFlowControlOff )
        {
        iSuper.FillIsiHeader( aData );
        retVal = iSuper.Send( aData );
        SetFlowCtrl( EIADFlowControlOn );
        //iSuper.SetFlowCtrl( EIADFlowControlOn );
        }
    else
        {
        TRACE_ASSERT_INFO( 0, (TUint8)iSuper.ChannelId()<<KChannelNumberShift );
        Kern::Printf("IAD: iChannelId = %d", iSuper.ChannelId() );
        retVal =  KErrOverflow;
        }
    C_TRACE( ( _T( "DOneTx::Send 0x%x <-" ), retVal ) );

    OstTrace1( TRACE_PIPE, DONETX_SEND_EXIT, "<DOneTx::Send;retVal=%d", retVal );
    return retVal;
    }

TInt DMultiTx::Send(
        TDes8& aData
        )
    {
	OstTrace1( TRACE_PIPE, DMULTITX_SEND_ENTRY, ">DMultiTx::Send;aData=%x", ( TUint )&( aData ) );

    C_TRACE( ( _T( "DMultiTx::Send %d ->" ), iGrantedCredits ) );
    TInt retVal( KErrNone );
    if( iFlowCtrl == EIADFlowControlOff )
        {
        iSuper.FillIsiHeader( aData );
        retVal = iSuper.Send( aData );
        iGrantedCredits--;
        if( iGrantedCredits == 0 )
            {
            SetFlowCtrl( EIADFlowControlOn );
            //iSuper.SetFlowCtrl( EIADFlowControlOn );
            }
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        Kern::Printf("IAD: iChannelId = %d", iSuper.ChannelId() );
        retVal =  KErrOverflow;
        }
    C_TRACE( ( _T( "DMultiTx::Send %d %d <-" ), iGrantedCredits, retVal ) );

    OstTrace1( TRACE_PIPE, DMULTITX_SEND_EXIT, "<DMultiTx::Send;retVal=%d", retVal );
    return retVal;
    }

TInt DLegacyTx::Send(TDes8& aData)
    {
    OstTrace1( TRACE_PIPE, DLEGACYTX_SEND_ENTRY, ">DLegacyTx::Send;aData=%x", ( TUint )&( aData ) );
    C_TRACE( ( _T( "DLegacyTx::Send ->" ) ) );
    TInt retVal( KErrNone );
    if( iFlowCtrl == EIADFlowControlOff )
        {
        iSuper.FillIsiHeader( aData );
        retVal = iSuper.Send( aData );
        }
    else
        {
        TRACE_ASSERT_INFO( 0, (TUint8)iSuper.ChannelId()<<KChannelNumberShift );
        Kern::Printf("IAD: iChannelId = %d", iSuper.ChannelId() );
        retVal =  KErrOverflow; 
        }
    C_TRACE( ( _T( "DLegacyTx::Send return 0x%x <-" ), retVal ) );
    
    OstTrace1( TRACE_PIPE, DLEGACYTX_SEND_EXIT, "<DLegacyTx::Send;retVal=%d", retVal );    
    return retVal;
    }
