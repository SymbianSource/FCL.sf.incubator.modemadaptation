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


#include <phonetisi.h>              // For phonet-header offsets like ISI_HEADER_OFFSET_MESSAGEID
#include <pipeisi.h>                // For PEP_IND_BUSY etc..
#include <pipe_sharedisi.h>         // For PN_MULTI_CREDIT_FLOW_CONTROL etc..
#include "pep.h"
#include "peptransceiver.h"
#include "iadtrace.h"
#include "pipehandler.h"
#include "router.h"                    // For DRouter
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pepTraces.h"
#endif
// ************* DPipe methods ************* 

DPep::DPep(
        DRouter* aRouter,
        MIAD2ChannelApi* aChannel,
        TUint16 aChannelId,
        DPipeHandler& aHandler
        ):
        iRouter( aRouter ),
        iPipeHandle( 0 ),
        iChannelId( aChannelId ),
        iPipeHandler( aHandler ),
        iChannel( aChannel ),
        iDecoratedRx( NULL ),
        iDecoratedTx( NULL ),
        iMainTxRx( aRouter, aChannel ),
        iCreating( *this ),
        iCreatingEnabled( *this ),
        iDisabled( *this ),
        iDisabling( *this ),
        iEnabling( *this ),
        iEnabled( *this ),
//TBR        
        iEnableEnabled( *this ),
        iDisabledDisabled( *this ),
        iDisabledEnabled( *this ),
        iEnabledDisabled( *this ),
        iEnabledEnabled( *this ),
        iDisconnected( *this ),
        iCurrentState( NULL )
    {
    OstTraceExt4( TRACE_PIPE, DPEP_DPEP_ENTRY, ">DPep::DPep;aRouter=%x;aChannel=%x;aChannelId=%hx;aHandler=%x", ( TUint )( aRouter ), ( TUint )( aChannel ), aChannelId, ( TUint )&( aHandler ) );

    C_TRACE( ( _T( "DPep::DPep 0x%x ->" ), aChannelId ) );
    ASSERT_RESET_ALWAYS( iRouter && iChannel, EIADNullParameter | EIADFaultIdentifier5 << KFaultIdentifierShift  );
    iCurrentState = &iCreating;
    C_TRACE( ( _T( "DPep::DPep <-" ) ) );

    OstTrace0( TRACE_PIPE, DPEP_DPEP_EXIT, "<DPep::DPep" );
    }


DPep::~DPep()
    {
    OstTrace0( TRACE_PIPE, DPEP_DPEP_DES_ENTRY, ">DPep::~DPep" );

    C_TRACE( ( _T( "DPep::~DPep ->" ) ) );
    if( iDecoratedRx )
        {
        delete iDecoratedRx;
        iDecoratedRx = NULL;
        }
    if( iDecoratedTx )
        {
        delete iDecoratedTx;
        iDecoratedTx = NULL;
        }
    C_TRACE( ( _T( "DPep::~DPep <-" ) ) );

    OstTrace0( TRACE_PIPE, DPEP_DPEP_DES_EXIT, "<DPep::~DPep" );
    }

TInt DPep::SendMessage(
        TDes8& aMsg,
        TUint16 // TBR aFromChannel
        )
    {
    OstTrace1( TRACE_PIPE, DPEP_SENDMESSAGE_ENTRY, ">DPep::SendMessage;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DPep::SendMessage From: 0x%x 0x%x ->" ), iChannelId, this ) );
    TInt retVal( KErrNotSupported );
    ASSERT_RESET_ALWAYS( aMsg.Length() > ( ISI_HEADER_OFFSET_MESSAGEID ), EIADOverTheLimits | EIADFaultIdentifier26 << KFaultIdentifierShift );                                
    switch( aMsg.Ptr()[ ISI_HEADER_OFFSET_MESSAGEID ] )
        {
        case PNS_PIPE_DATA:
            {
            retVal = iCurrentState->TxPnsPipeData( aMsg );
            break;
            }
        case PNS_PEP_STATUS_IND:// if data port _COMM
            {
            if( aMsg.Ptr()[ ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ] == PN_PEP_TYPE_COMM )
                {
                retVal = iRouter->SendMsg( aMsg );
                }
            break;
            }
        case PNS_PEP_CTRL_RESP: // if data port _COMM
            {
            if( aMsg.Ptr()[ ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PEPTYPE ] == PN_PEP_TYPE_COMM )
                {
                retVal = iRouter->SendMsg( aMsg );
                }
            break;
            }
        // Controller can send these.
        case PNS_PEP_CTRL_REQ:
        case PNS_PIPE_ENABLE_REQ:
        case PNS_PIPE_DISABLE_REQ:
        case PNS_PIPE_RESET_REQ:
        case PNS_PIPE_REDIRECT_REQ:
        case PNS_PIPE_REMOVE_REQ:
            {
            retVal = iRouter->SendMsg( aMsg );
            break;
            }
        default:
            {
            DATA_DUMP_TRACE( aMsg, ETrue );
            OstTraceData( TRACE_ISIMSG, DPEP_SENDMESSAGE, "DPep::SendMessage %{hex8[]}", aMsg.Ptr(), aMsg.Length() );
            ASSERT_RESET_ALWAYS( 0, EIADNotSupported | (TUint8)iChannelId<<KChannelNumberShift | EIADFaultIdentifier4<<KFaultIdentifierShift | aMsg.Ptr()[ ISI_HEADER_OFFSET_MESSAGEID ]<<KExtraInfoShift);
            break;
            }
        }
    C_TRACE( ( _T( "DPep::SendMessage %d 0x%x <-" ), retVal, this ) );


    OstTrace1( TRACE_PIPE, DPEP_SENDMESSAGE_EXIT, "<DPep::SendMessage;retVal=%d", retVal );
    return retVal;
    }

TInt DPep::ReceiveMessage(
        const TDesC8& aMessage,
        MIAD2ChannelApi* aChannel
        )
    {
    OstTraceExt2( TRACE_PIPE, DPEP_RECEIVEMESSAGE_ENTRY, ">DPep::ReceiveMessage;aMessage=%x;aChannel=%x", ( TUint )&( aMessage ), ( TUint )( aChannel ) );

    C_TRACE( ( _T( "DPep::ReceiveMessage 0x%x ->" ), aChannel ) );
    TInt retVal( KErrNone );
    ASSERT_RESET_ALWAYS( aMessage.Length() > ( ISI_HEADER_OFFSET_MESSAGEID ), EIADOverTheLimits | EIADFaultIdentifier27 << KFaultIdentifierShift );                                    
    switch( aMessage.Ptr()[ ISI_HEADER_OFFSET_MESSAGEID ] )
        {
        case PNS_PIPE_DATA:
            {
            retVal = iCurrentState->RxPnsPipeData( aMessage );
            break;
            }
        case PNS_PIPE_CREATED_IND:
            {
            HandlePnPipeSbNegotiatedFC( aMessage );
            iCurrentState->PnsPipeCreatedInd();
               
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PIPE_DISABLED_IND:
            {
            iCurrentState->PnsPipeDisabledInd();// TODO: checking of pipehandle
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PIPE_RESET_IND:
            {
            iCurrentState->PnsPipeResetInd();
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PIPE_ENABLED_IND:
            {
            iCurrentState->PnsPipeEnabledInd();
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PIPE_REDIRECTED_IND:
            {
            HandlePnPipeSbNegotiatedFC( aMessage );
            iCurrentState->PnsPipeRedirectedInd();
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PEP_ENABLE_REQ:
            {
            retVal = iCurrentState->PnsPepEnableReq( aMessage );
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PEP_DISCONNECT_REQ:
            {
            retVal = iCurrentState->PnsPepDisconnectReq( aMessage );
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            iPipeHandler.RemovePep(this);
            break;
            }
        case PNS_PEP_CONNECT_REQ:
            {
            // Needed to set for SOS originated pipes
            iChannel = ( NULL == iChannel ) ? aChannel : iChannel;
            retVal = iCurrentState->PnsPepConnectReq( aMessage ); 
               
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PEP_RESET_REQ:
            {
            retVal = iCurrentState->PnsPepResetReq( aMessage );
               
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PEP_DISABLE_REQ:
            {
            retVal = iCurrentState->PnsPepDisableReq( aMessage );
               
            iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
            break;
            }
        case PNS_PEP_STATUS_IND:
            {
            // TODO: Remove this if pep_type_common to be the only pep_type ( && ( ...== PN_PEP_TYPE_COMMON ) )
            ASSERT_RESET_ALWAYS( aMessage.Length() > ( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ), EIADOverTheLimits | EIADFaultIdentifier28 << KFaultIdentifierShift );                                    
            if( ( aMessage.Ptr()[ ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ] == PN_PEP_TYPE_COMMON ) )
                {
                iDecoratedTx->PepStatusInd( aMessage );
                iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
                }
            else
                {
                aChannel->ReceiveMsg(aMessage); // To dataport if _COMM
                }
            break;
            }
        case PNS_PEP_CTRL_REQ:
            {
            // TODO Remove this if pep_type_common to be the only pep_type 
            ASSERT_RESET_ALWAYS( aMessage.Length() > ( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ), EIADOverTheLimits | EIADFaultIdentifier29 << KFaultIdentifierShift );                                    
            if( ( aMessage.Ptr()[ ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ] != PN_PEP_TYPE_COMMON ) )
                {
                aChannel->ReceiveMsg( aMessage ); // to dataport _COMM 
                }
            else{
                retVal = iDecoratedRx->PepControlReq( aMessage );
                iRouter->DeAllocateBlock( ((TDes8&)aMessage ) );
                }
            break;
            }
        // directly to controller
        case PNS_PIPE_REMOVE_RESP:
        case PNS_PIPE_REDIRECT_RESP:
        case PNS_PIPE_ENABLE_RESP:
        case PNS_PIPE_DISABLE_RESP:
        case PNS_PIPE_RESET_RESP:
        case PNS_PIPE_CREATE_RESP:
        case PNS_PEP_CTRL_RESP:
            {
            aChannel->ReceiveMsg( aMessage );
            break;
            }
            
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADUnknownPipeOrPepMsg | EIADFaultIdentifier1 << KFaultIdentifierShift );
            break;
            }
        }
    C_TRACE( ( _T( "DPep::ReceiveMessage 0x%x <-" ), retVal ) );


    OstTrace1( TRACE_PIPE, DPEP_RECEIVEMESSAGE_EXIT, "<DPep::ReceiveMessage;retVal=%d", retVal );
    return retVal;
    }

TInt DPep::SendPepConnectResp( const TDesC8& aReq )
    {
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPCONNECTRESP_ENTRY, ">DPep::SendPepConnectResp;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DPep::SendPepConnectResp ->" )) );
    TInt retVal = iMainTxRx.SendPnsPepConnectResp( aReq ); // Handle is stored in Tx
    C_TRACE( ( _T( "DPep::SendPepConnectResp 0x%x <-" ), retVal) );
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPCONNECTRESP_EXIT, "<DPep::SendPepConnectResp;retVal=%d", retVal );
    return retVal;
    }
    
TInt DPep::SendPepResetResp( const TDesC8& aReq )
    {
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPRESETRESP_ENTRY, ">DPep::SendPepResetResp;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DPep::SendPepResetResp ->" )) );
    TInt retVal = iMainTxRx.SendResetResp( aReq );
    C_TRACE( ( _T( "DPep::SendPepResetResp 0x%x <-" ), retVal ) );
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPRESETRESP_EXIT, "<DPep::SendPepResetResp;retVal=%d", retVal );
    return retVal;
    }
    
TInt DPep::SendPepDisconnectResp( const TDesC8& aReq )
    {
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPDISCONNECTRESP_ENTRY, ">DPep::SendPepDisconnectResp;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DPep::SendPepDisconnectResp ->" )) );
    TInt retVal =  iMainTxRx.SendPnsPepDisconnectResp( aReq );
    C_TRACE( ( _T( "DPep::SendPepDisconnectResp 0x%x <-" ), retVal) );
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPDISCONNECTRESP_EXIT, "<DPep::SendPepDisconnectResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPep::SendPepEnableResp( const TDesC8& aReq )
    {
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPENABLERESP_ENTRY, ">DPep::SendPepEnableResp;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DPep::SendPepEnableResp ->" )) );
    TInt retVal =  iMainTxRx.SendEnableResp( aReq );
    C_TRACE( ( _T( "DPep::SendPepEnableResp 0x%x <-" ), retVal) );
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPENABLERESP_EXIT, "<DPep::SendPepEnableResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPep::SendPepDisableResp(const TDesC8& aReq)
    {
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPDISABLERESP_ENTRY, ">DPep::SendPepDisableResp;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DPep::SendPepDisableResp ->" )) );
    TInt retVal = iMainTxRx.SendDisableResp( aReq );
    C_TRACE( ( _T( "DPep::SendPepDisableResp 0x%x <-" ), retVal) );
    OstTrace1( TRACE_PIPE, DPEP_SENDPEPDISABLERESP_EXIT, "<DPep::SendPepDisableResp;retVal=%d", retVal );
    return retVal;
    }

TInt DPep::RxPipeData( const TDesC8& aData )
    {
    OstTrace1( TRACE_PIPE, DPEP_RXPIPEDATA_ENTRY, ">DPep::RxPipeData;aData=%x", ( TUint )&( aData ) );
    C_TRACE( ( _T( "DPep::RxPipeData ->" )) );
    ASSERT_RESET_ALWAYS( iDecoratedRx, EIADNullParameter | EIADFaultIdentifier6 << KFaultIdentifierShift  );
    TInt retVal = iDecoratedRx->Receive( aData );
    C_TRACE( ( _T( "DPep::RxPipeData 0x%x <-" ), retVal) );
    OstTrace1( TRACE_PIPE, DPEP_RXPIPEDATA_EXIT, "<DPep::RxPipeData;retVal=%d", retVal );
    return retVal;    
    }

TInt DPep::TxPipeData( TDes8& aData )
    {
    OstTrace1( TRACE_PIPE, DPEP_TXPIPEDATA_ENTRY, ">DPep::TxPipeData;aData=%x", ( TUint )&( aData ) );
    C_TRACE( ( _T( "DPep::TxPipeData ->" )) );
    ASSERT_RESET_ALWAYS( iDecoratedTx, EIADNullParameter | EIADFaultIdentifier7 << KFaultIdentifierShift  );
    TInt retVal = iDecoratedTx->Send( aData );
    C_TRACE( ( _T( "DPep::TxPipeData %d <-" ), retVal) );
    OstTrace1( TRACE_PIPE, DPEP_TXPIPEDATA_EXIT, "<DPep::TxPipeData;retVal=%d", retVal );
    return retVal;
    }

void DPep::PipeEnabled()
    {
    OstTrace0( TRACE_PIPE, DPEP_PIPEENABLED_ENTRY, ">DPep::PipeEnabled" );
    C_TRACE( ( _T( "DPep::PipeEnabled ->" ) ) );
    ASSERT_RESET_ALWAYS( iDecoratedRx && iDecoratedTx, EIADNullParameter | EIADFaultIdentifier8 << KFaultIdentifierShift );
    iDecoratedRx->PipeEnabled(); 
    iDecoratedTx->PipeEnabled();
    C_TRACE( ( _T( "DPep::PipeEnabled <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEP_PIPEENABLED_EXIT, "<DPep::PipeEnabled" );
    }

void DPep::PipeDisabling()
    {
    OstTrace0( TRACE_PIPE, DPEP_PIPEDISABLING_ENTRY, ">DPep::PipeDisabling" );
    C_TRACE( ( _T( "DPep::PipeDisabling ->" ) ) );
    ASSERT_RESET_ALWAYS( iDecoratedTx, EIADNullParameter | EIADFaultIdentifier9 << KFaultIdentifierShift );
    iDecoratedTx->PipeDisabled();
    C_TRACE( ( _T( "DPep::PipeDisabling <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEP_PIPEDISABLING_EXIT, "<DPep::PipeDisabling" );
    }

void DPep::PipeDisabled()
    {
    OstTrace0( TRACE_PIPE, DPEP_PIPEDISABLED_ENTRY, ">DPep::PipeDisabled" );
    C_TRACE( ( _T( "DPep::PipeDisabled ->" ) ) );
    ASSERT_RESET_ALWAYS( iDecoratedRx, EIADNullParameter | EIADFaultIdentifier10 << KFaultIdentifierShift );
    // TX Already disabled (or then create new Disabling for tx's
    iDecoratedRx->PipeDisabled();
    C_TRACE( ( _T( "DPep::PipeDisabled <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEP_PIPEDISABLED_EXIT, "<DPep::PipeDisabled" );
    }

void DPep::PipeEnableEnabled()
    {
    OstTrace0( TRACE_PIPE, DPEP_PIPEENABLEENABLED_ENTRY, ">DPep::PipeEnableEnabled" );
    C_TRACE( ( _T( "DPep::PipeEnableEnabled ->" ) ) );
    ASSERT_RESET_ALWAYS( iDecoratedTx, EIADNullParameter | EIADFaultIdentifier11 << KFaultIdentifierShift );
    iDecoratedTx->PipeEnabled();
    C_TRACE( ( _T( "DPep::PipeEnableEnabled <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEP_PIPEENABLEENABLED_EXIT, "<DPep::PipeEnableEnabled" );
    }

void DPep::PipeDisableDisabled()
    {
    OstTrace0( TRACE_PIPE, DPEP_PIPEDISABLEDISABLED_ENTRY, ">DPep::PipeDisableDisabled" );
    C_TRACE( ( _T( "DPep::PipeDisableDisabled ->" ) ) );
    ASSERT_RESET_ALWAYS( iDecoratedRx, EIADNullParameter | EIADFaultIdentifier12 << KFaultIdentifierShift );
    TRACE_ASSERT_ALWAYS;// specifications (pipe, one- and multicredit) don't say anything about this.
    iDecoratedRx->PipeDisabled();
    C_TRACE( ( _T( "DPep::PipeDisableDisabled <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEP_PIPEDISABLEDISABLED_EXIT, "<DPep::PipeDisableDisabled" );
    }

void DPep::HandlePnPipeSbNegotiatedFC(
        const TDesC8& aMsg
        )
    {
    OstTrace1( TRACE_PIPE, DPEP_HANDLEPNPIPESBNEGOTIATEDFC_ENTRY, ">DPep::HandlePnPipeSbNegotiatedFC;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DPep::HandlePnPipeSbNegotiatedFC ->" ) ) );
    const TUint8* ptr( aMsg.Ptr() );
    TUint8 txFcType( PN_LEGACY_FLOW_CONTROL );
    TUint8 rxFcType( PN_LEGACY_FLOW_CONTROL );
    ASSERT_RESET_ALWAYS( PNS_PIPE_CREATED_IND_OFFSET_NSB == PNS_PIPE_REDIRECTED_IND_OFFSET_NSB, 0 );
    const TUint8 sbCountOffset( ( PNS_PIPE_CREATED_IND_OFFSET_NSB + PNS_PIPE_REDIRECTED_IND_OFFSET_NSB ) / 2 );
    ASSERT_RESET_ALWAYS( aMsg.Length() > ( ISI_HEADER_SIZE + sbCountOffset ), EIADOverTheLimits | EIADFaultIdentifier30 << KFaultIdentifierShift );
    if( ptr[ ISI_HEADER_SIZE + sbCountOffset ] != 0x00 )
        {
        txFcType = ( ptr[ ISI_HEADER_SIZE + SIZE_PNS_PIPE_CREATED_IND + PN_PIPE_SB_NEGOTIATED_FC_OFFSET_SBID + PN_PIPE_SB_NEGOTIATED_FC_OFFSET_NEGOTIATEDTXFC ] );
        rxFcType = ( ptr[ ISI_HEADER_SIZE + SIZE_PNS_PIPE_CREATED_IND + PN_PIPE_SB_NEGOTIATED_FC_OFFSET_SBID + PN_PIPE_SB_NEGOTIATED_FC_OFFSET_NEGOTIATEDRXFC ] );
        ASSERT_RESET_ALWAYS( aMsg.Length() > ( ISI_HEADER_SIZE + SIZE_PNS_PIPE_CREATED_IND + PN_PIPE_SB_NEGOTIATED_FC_OFFSET_SBID + PN_PIPE_SB_NEGOTIATED_FC_OFFSET_NEGOTIATEDRXFC ), EIADOverTheLimits | EIADFaultIdentifier31 << KFaultIdentifierShift );                                    
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        C_TRACE( ( _T( "DPep::HandlePnPipeSbNegotiatedFC legacy-legacy in use or corrupted" ) ) );
        OstTrace0( TRACE_PIPE, DPEP_HANDLEPNPIPESBNEGOTIATEDFC_LEGACY, "DPep::HandlePnPipeSbNegotiatedFC legacy used or corrupted" );
        }

    iPipeHandle = iMainTxRx.PipeHandle();
    ASSERT_RESET_ALWAYS( ptr[ ISI_HEADER_SIZE + PNS_PIPE_CREATED_IND_OFFSET_PIPEHANDLE ] == iPipeHandle, EIADWrongPipeHandle | EIADFaultIdentifier1 << KFaultIdentifierShift );
    // Can also be PN_NO_FLOW_CONTROL but this PEP is not ready for it.
    if( iDecoratedRx )
        {
        delete iDecoratedRx;
        iDecoratedRx = NULL;
        }
    switch( rxFcType )
        {
        case PN_LEGACY_FLOW_CONTROL:
            {
            iDecoratedRx = new DLegacyRx( iMainTxRx, iPipeHandle );
            break;
            }
        case PN_ONE_CREDIT_FLOW_CONTROL:
            {
            iDecoratedRx = new DOneRx( iMainTxRx, iPipeHandle );
            break;
            }
        case PN_MULTI_CREDIT_FLOW_CONTROL:
            {
            iDecoratedRx = new DMultiRx( iMainTxRx, iPipeHandle );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS(0, EIADUnknownPipeFCType | EIADFaultIdentifier1 << KFaultIdentifierShift );
            break;
            }
        }
    if( iDecoratedTx )
        {
        delete iDecoratedTx;
        iDecoratedTx = NULL;
        }
    switch( txFcType )
        {
        case PN_LEGACY_FLOW_CONTROL:
            {
            iDecoratedTx = new DLegacyTx( iMainTxRx, iPipeHandle );
            break;
            }
        case PN_ONE_CREDIT_FLOW_CONTROL:
            {
            iDecoratedTx = new DOneTx( iMainTxRx, iPipeHandle );
            break;
            }
        case PN_MULTI_CREDIT_FLOW_CONTROL:
            {
            iDecoratedTx = new DMultiTx( iMainTxRx, iPipeHandle );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS(0, EIADUnknownPipeFCType | EIADFaultIdentifier2 << KFaultIdentifierShift );
            break;
            }
        }
    C_TRACE( ( _T( "DPep::HandlePnPipeSbNegotiatedFC <-" ) ) );
    OstTrace0( TRACE_PIPE, DPEP_HANDLEPNPIPESBNEGOTIATEDFC_EXIT, "<DPep::HandlePnPipeSbNegotiatedFC" );

    }


void DPep::NextState(
        TPipeState aNextState
        )
    {

    OstTrace1( TRACE_PIPE, DPEP_NEXTSTATE_ENTRY, ">DPep::NextState;aNextState=%x", ( TUint )&( aNextState ) );
    C_TRACE( ( _T( "DPep::NextState 0x%x 0x%x ->" ), aNextState, this ) );
    C_TRACE( ( _T( "DPep::NextState TBR currentstate pipehandle 0x%x " ), iCurrentState->iPep.GetPipeHandle() ) );//TBR
    switch( aNextState )
        {
        case ECreating:
            {
            iCurrentState = &iCreating;
            C_TRACE( ( _T( "ECreating" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_CREATING, "DPep::NextState ECreating" );
            break;
            }
        case ECreatingEnabled:
            {
            iCurrentState = &iCreatingEnabled;
            C_TRACE( ( _T( "ECreatingEnabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_CREATINGENABLED, "DPep::NextState ECreatingEnabled" );
            break;
            }
        case EDisabled:
            {
            iCurrentState = &iDisabled;
            C_TRACE( ( _T( "EDisabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_DISABLED, "DPep::NextState EDisabled" );
            break;
            }
        case EDisabling:
            {
            iCurrentState = &iDisabling;
            C_TRACE( ( _T( "EDisabling" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_DISABLING, "DPep::NextState EDisabling" );
            break;
            }
        case EEnabled:
            {
            iCurrentState = &iEnabled;
            C_TRACE( ( _T( "EEnabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_ENABLED, "DPep::NextState EEnabled" );
            break;
            }
        case EEnabling:
            {
            iCurrentState = &iEnabling;
            C_TRACE( ( _T( "EEnabling" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_ENABLING, "DPep::NextState EEnabling" );
            break;
            }
//TBR
        case EEnableEnabled:
            {
            iCurrentState = &iEnableEnabled;
            C_TRACE( ( _T( "EEnableEnabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_ENABLEENABLED, "DPep::NextState EEnableEnabled" );
            break;
            }
//TBR
        case EDisabledDisabled:
            {
            iCurrentState = &iDisabledDisabled;
            C_TRACE( ( _T( "EDisabledDisabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_DISABLEDDISABLED, "DPep::NextState EDisabledDisabled" );
            break;
            }
        case EDisabledEnabled:
            {
            iCurrentState = &iDisabledEnabled;
            C_TRACE( ( _T( "EDisabledEnabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_DISABLEDENABLED, "DPep::NextState EDisabledEnabled" );
            break;
            }
        case EEnabledDisabled:
            {
            iCurrentState = &iEnabledDisabled;
            C_TRACE( ( _T( "EEnabledDisabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_ENABLEDDISABLED, "DPep::NextState EEnabledDisabled" );
            break;
            }
        case EEnabledEnabled:
            {
            iCurrentState = &iEnabledEnabled;
            C_TRACE( ( _T( "EEnabledEnabled" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_ENABLEDENABLED, "DPep::NextState EEnabledEnabled" );
            break;
            }
        case EDisconnected:
            {
            iCurrentState = &iDisconnected;
            C_TRACE( ( _T( "EDisconnected" ) ) );
            OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_DISCONNECTED, "DPep::NextState EDisconnected" );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADWrongParameter | EIADFaultIdentifier20 << KFaultIdentifierShift );
            break;
            }
        }
    iCurrentState->EnterState();
    C_TRACE( ( _T( "DPep::NextState 0x%x <-" ), this ) );
    OstTrace0( TRACE_PIPE, DPEP_NEXTSTATE_EXIT, "<DPep::NextState" );

    }

// ************* DPipe methods ************* 

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
void DPep::SetLoaned(
        MIAD2ChannelApi* aCh
        )
    {

    C_TRACE( ( _T( "DPep::SetLoaned 0x%x 0x%x ->" ), this, aCh ) );
    OstTraceExt2( TRACE_PIPE, DPEP__SETLOANED_ENTRY, ">DPep::SetLoaned ;this=%x;ch=0x%x", (TUint)this, (TUint) aCh );
    this->iDecoratedRx->SetNewChannel( aCh );
    this->PipeEnabled();
    C_TRACE( ( _T( "DPep::SetLoaned 0x%x 0x%x <-" ), this, aCh ) );
    OstTraceExt2( TRACE_PIPE, DPEP__SETLOANED_EXIT, "<DPep::SetLoaned ;this=%x;ch=0x%x", (TUint)this, (TUint)aCh );

    }

void DPep::SetLoanReturned(
        MIAD2ChannelApi* aOldCh
        )
    {

    C_TRACE( ( _T( "DPep::SetLoanReturned 0x%x aOldCh 0x%x ->" ), this, aOldCh ) );
    OstTraceExt2( TRACE_PIPE, DPEP__SETLOANRETURNED_ENTRY, ">DPep::SetLoaned ;this=%x;oldch=0x%x", (TUint)this, (TUint)aOldCh );
    this->iDecoratedRx->SetNewChannel( aOldCh );
    OstTraceExt2( TRACE_PIPE, DPEP__SETLOANRETURNED_EXIT, "<DPep::SetLoaned ;this=%x;oldch=0x%x", (TUint)this, (TUint)aOldCh );
    C_TRACE( ( _T( "DPep::SetLoanReturned 0x%x aOldCh 0x%x <-" ), this, aOldCh ) );

    }
#endif

// ************* Main pipe state methods **************
DPipeState::DPipeState(DPep& aPep) : iPep( aPep )
    {
    OstTrace1( TRACE_PIPE, DPIPESTATE_DPIPESTATE_ENTRY, "<>DPipeState::DPipeState;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DPipeState::DPipeState <->" ) ) );
    }

void DPipeState::NextState(TPipeState aNextState)
    {
    OstTrace1( TRACE_PIPE, DPIPESTATE_NEXTSTATE_ENTRY, "<>DPipeState::NextState;aNextState=%x", ( TUint )&( aNextState ) );
    C_TRACE( ( _T( "DPipeState::NextState ->" ) ) );
    iPep.NextState(aNextState);
    C_TRACE( ( _T( "DPipeState::NextState <-" ) ) );
    }

void DPipeState::EnterState()
    {
    OstTrace0( TRACE_PIPE, DPIPESTATE_ENTERSTATE_ENTRY, "<>DPipeState::EnterState" );
    C_TRACE( ( _T( "DPipeState::EnterState <-> nothing to do" ) ) );
    //TRACE_ASSERT_ALWAYS;
    }


TInt DPipeState::PnsPepDisconnectReq(const TDesC8& aReq)
    {

    OstTrace1( TRACE_PIPE, DPIPESTATE_PNSPEPDISCONNECTREQ_ENTRY, ">DPipeState::PnsPepDisconnectReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DPipeState::PnsPepDisconnectReq ->" ) ) );
    iPep.NextState(EDisconnected);
    TInt retVal = iPep.SendPepDisconnectResp(aReq);
    C_TRACE( ( _T( "DPipeState::PnsPepDisconnectReq 0x%x <-" ),retVal ) );
    OstTrace1( TRACE_PIPE, DPIPESTATE_PNSPEPDISCONNECTREQ_EXIT, "<DPipeState::PnsPepDisconnectReq;retVal=%d", retVal );
    return retVal;

    }

TInt DPipeState::TxPnsPipeData(
        TDes8& // aData
        )
    {

    OstTrace0( TRACE_PIPE, DPIPESTATE_TXPNSPIPEDATA_ENTRY, ">DPipeState::TxPnsPipeData" );
    C_TRACE( ( _T( "DPipeState::TxPnsPipeData <->" ) ) );
    TRACE_ASSERT_ALWAYS;
    // illegal to send in other than enabled state inform with KErrOverFlow that flowcontrol is on.
    return KErrOverflow;

    }

//********************** DCreating methods *******************/
DCreating::DCreating(DPep& aPep) : DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DCREATING_DCREATING_ENTRY, ">DCreating::DCreating;aPep=%x", ( TUint )&( aPep ) );

    C_TRACE( ( _T( "DCreating::DCreating <->" ) ) );

    }

TInt DCreating::PnsPepConnectReq(
        const TDesC8& aReq
        )
    {

    OstTrace1( TRACE_PIPE, DCREATING_PNSPEPCONNECTREQ_ENTRY, ">DCreating::PnsPepConnectReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DCreating::PnsPepConnectReq ->" ) ) );
    TInt retVal = iPep.SendPepConnectResp( aReq );
    ASSERT_RESET_ALWAYS( aReq.Length() > ( ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_STATEAFTERCONNECT ), EIADOverTheLimits | EIADFaultIdentifier32 << KFaultIdentifierShift );                                        
    const TUint8 stateAfter( aReq.Ptr()[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_STATEAFTERCONNECT ] );
    if( stateAfter == PN_PIPE_ENABLE )
        {
        iPep.NextState( ECreatingEnabled );
        }
    C_TRACE( ( _T( "DCreating::PnsPepConnectReq 0x%x <-" ),retVal ) );
    OstTrace1( TRACE_PIPE, DCREATING_PNSPEPCONNECTREQ_EXIT, "<DCreating::PnsPepConnectReq;retVal=%d", retVal );
    return retVal;

    }

void DCreating::PnsPipeCreatedInd(
        // None
        )
    {

    OstTrace0( TRACE_PIPE, DCREATING_PNSPIPECREATEDIND_ENTRY, ">DCreating::PnsPipeCreatedInd" );
    C_TRACE( ( _T( "DCreating::PnsPipeCreatedInd ->" ) ) );
    iPep.NextState( EDisabling );
    iPep.NextState( EDisabled );
    C_TRACE( ( _T( "DCreating::PnsPipeCreatedInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DCREATING_PNSPIPECREATEDIND_EXIT, "<DCreating::PnsPipeCreatedInd" );

    }

void DCreating::PnsPipeRedirectedInd(
        // None
        )
    {

    OstTrace0( TRACE_PIPE, DCREATING_PNSPIPEREDIRECTEDIND_ENTRY, ">DCreating::PnsPipeRedirectedInd" );
    C_TRACE( ( _T( "DCreating::PnsPipeRedirectedInd ->" ) ) );
    iPep.NextState( EDisabling );
    iPep.NextState( EDisabled );
    C_TRACE( ( _T( "DCreating::PnsPipeRedirectedInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DCREATING_PNSPIPEREDIRECTEDIND_EXIT, "<DCreating::PnsPipeRedirectedInd" );

    }

DCreatingEnabled::DCreatingEnabled(DPep& aPep) : DPipeState( aPep )
    {

    OstTrace1( TRACE_PIPE, DCREATINGENABLED_DCREATINGENABLED_ENTRY, "<>DCreatingEnabled::DCreatingEnabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DCreatingEnabled::DCreatingEnabled <->" ) ) );

    }

void DCreatingEnabled::PnsPipeCreatedInd(
        // None
        )
    {

    OstTrace0( TRACE_PIPE, DCREATINGENABLED_PNSPIPECREATEDIND_ENTRY, ">DCreatingEnabled::PnsPipeCreatedInd" );
    C_TRACE( ( _T( "DCreatingEnabled::PnsPipeCreatedInd ->" ) ) );
    iPep.NextState( EEnabled );
    C_TRACE( ( _T( "DCreatingEnabled::PnsPipeCreatedInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DCREATINGENABLED_PNSPIPECREATEDIND_EXIT, "<DCreatingEnabled::PnsPipeCreatedInd" );

    }

void DCreatingEnabled::PnsPipeRedirectedInd(
        // None
        )
    {

    OstTrace0( TRACE_PIPE, DCREATINGENABLED_PNSPIPEREDIRECTEDIND_ENTRY, ">DCreatingEnabled::PnsPipeRedirectedInd" );
    C_TRACE( ( _T( "DCreatingEnabled::PnsPipeRedirectedInd ->" ) ) );
    iPep.NextState( EEnabled );
    C_TRACE( ( _T( "DCreatingEnabled::PnsPipeRedirectedInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DCREATINGENABLED_PNSPIPEREDIRECTEDIND_EXIT, "<DCreatingEnabled::PnsPipeRedirectedInd" );

    }

//********************** DCreating methods *******************/

DDisconnected::DDisconnected(DPep& aPep) : DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DDISCONNECTED_DDISCONNECTED_ENTRY, "<>DDisconnected::DDisconnected;aPep=%x", ( TUint )&( aPep ) );

    C_TRACE( ( _T( "DDisconnected::DDisconnected <->" ) ) );

    }
// When Disconnected the pipe is no longer valid -> any transmission cancelled


//********************** DDisabled methods *******************/
DDisabled::DDisabled(DPep& aPep) : DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DDISABLED_DDISABLED_ENTRY, "<>DDisabled::DDisabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DDisabled::DDisabled <->" ) ) );
    }

void DDisabled::EnterState()
    {
    OstTrace0( TRACE_PIPE, DDISABLED_ENTERSTATE_ENTRY, ">DDisabled::EnterState" );
    C_TRACE( ( _T( "DDisabled::EnterState ->" ) ) );
    iPep.PipeDisabled();
    C_TRACE( ( _T( "DDisabled::EnterState <-" ) ) );
    OstTrace0( TRACE_PIPE, DDISABLED_ENTERSTATE_EXIT, "<DDisabled::EnterState" );
    }

TInt DDisabled::PnsPepEnableReq(const TDesC8& aReq)
    {
    OstTrace1( TRACE_PIPE, DDISABLED_PNSPEPENABLEREQ_ENTRY, ">DDisabled::PnsPepEnableReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DDisabled::PnsPepEnableReq ->" ) ) );
    iPep.NextState(EEnabling);
    C_TRACE( ( _T( "DDisabled::PnsPepEnableReq <-" ) ) );
    TInt retVal = iPep.SendPepEnableResp(aReq);
    OstTrace1( TRACE_PIPE, DDISABLED_PNSPEPENABLEREQ_EXIT, "<DDisabled::PnsPepEnableReq;retVal=%d", retVal );
    return retVal;
    }

TInt DDisabled::PnsPepDisableReq(const TDesC8& aReq)
    {
    OstTrace1( TRACE_PIPE, DDISABLED_PNSPEPDISABLEREQ_ENTRY, ">DDisabled::PnsPepDisableReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DDisabled::PnsPepDisableReq <->" ) ) );
    TInt retVal = iPep.SendPepDisableResp(aReq);
    OstTrace1( TRACE_PIPE, DDISABLED_PNSPEPDISABLEREQ_EXIT, "<DDisabled::PnsPepDisableReq;retVal=%d", retVal );
    return retVal;
    }

void DDisabled::PnsPipeDisabledInd(
        // None
        )
    {
OstTrace0( TRACE_PIPE, DDISABLED_PNSPIPEDISABLEDIND_ENTRY, ">DDisabled::PnsPipeDisabledInd" );

    C_TRACE( ( _T( "DDisabled::PnsPipeDisabledInd <->" ) ) );
    C_TRACE( ( _T( "DDisabled::PnsPipeDisabledInd disabling already disabled pipe" ) ) );

    }

//********************** DDisabling methods *******************/
DDisabling::DDisabling(
        DPep& aPep
        ) :
    DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DDISABLING_DDISABLING_ENTRY, "<>DDisabling::DDisabling;aPep=%x", ( TUint )&( aPep ) );

    C_TRACE( ( _T( "DDisabling::DDisabling <->" ) ) );

    }

void DDisabling::PnsPipeDisabledInd(
        // None
        )
    {
    OstTrace0( TRACE_PIPE, DDISABLING_PNSPIPEDISABLEDIND_ENTRY, ">DDisabling::PnsPipeDisabledInd" );

    C_TRACE( ( _T( "DDisabling::PnsPipeDisabledInd ->" ) ) );
    iPep.NextState( EDisabled );
    C_TRACE( ( _T( "DDisabling::PnsPipeDisabledInd <-" ) ) );

    OstTrace0( TRACE_PIPE, DDISABLING_PNSPIPEDISABLEDIND_EXIT, "<DDisabling::PnsPipeDisabledInd" );
    }

void DDisabling::EnterState(
        // None
        )
    {
    OstTrace0( TRACE_PIPE, DDISABLING_ENTERSTATE_ENTRY, ">DDisabling::EnterState" );

    C_TRACE( ( _T( "DDisabling::EnterState ->" ) ) );
    iPep.PipeDisabling();
    C_TRACE( ( _T( "DDisabling::EnterState <-" ) ) );

    OstTrace0( TRACE_PIPE, DDISABLING_ENTERSTATE_EXIT, "<DDisabling::EnterState" );
    }

void DDisabled::PnsPipeRedirectedInd(
        // None
        )
    {

    OstTrace0( TRACE_PIPE, DDISABLED_PNSPIPEREDIRECTEDIND_ENTRY, "<>DDisabled::PnsPipeRedirectedInd" );
    C_TRACE( ( _T( "DDisabled::PnsPipeRedirectedInd <->" ) ) );
    C_TRACE( ( _T( "DDisabled::PnsPipeRedirectedInd redirecting to disabled pipe" ) ) );

    }

TInt DDisabled::PnsPepResetReq(
        const TDesC8& aReq
        )
    {

    OstTrace1( TRACE_PIPE, DDISABLED_PNSPEPRESETREQ_ENTRY, ">DDisabled::PnsPepResetReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DDisabled::PnsPepResetReq ->" ) ) );
    ASSERT_RESET_ALWAYS( aReq.Length() > ( ISI_HEADER_SIZE + PNS_PIPE_RESET_REQ_OFFSET_PIPESTATEAFTERRESET ), EIADOverTheLimits | EIADFaultIdentifier33 << KFaultIdentifierShift );                                            
    const TUint8 stateAfter( aReq.Ptr()[ ISI_HEADER_SIZE + PNS_PIPE_RESET_REQ_OFFSET_PIPESTATEAFTERRESET ] );
    TPipeState nextState( stateAfter == PN_PIPE_DISABLE ? EDisabledDisabled : EDisabledEnabled );
    iPep.NextState( nextState );
    TInt retVal = iPep.SendPepResetResp( aReq ); // TODO: Why-iiiiiiiiiiiii retval?
    C_TRACE( ( _T( "DDisabled::PnsPepResetReq <-" ) ) );
    OstTrace1( TRACE_PIPE, DDISABLED_PNSPEPRESETREQ_EXIT, "<DDisabled::PnsPepResetReq;retVal=%d", retVal );
    return retVal;

    }

TInt DEnabled::PnsPepResetReq(
        const TDesC8& aReq
        )
    {

    OstTrace1( TRACE_PIPE, DENABLED_PNSPEPRESETREQ_ENTRY, ">DEnabled::PnsPepResetReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DEnabled::PnsPepResetReq ->" ) ) );
    ASSERT_RESET_ALWAYS( aReq.Length() > ( ISI_HEADER_SIZE + PNS_PIPE_RESET_REQ_OFFSET_PIPESTATEAFTERRESET ), EIADOverTheLimits | EIADFaultIdentifier34 << KFaultIdentifierShift );                                            
    const TUint8 stateAfter( aReq.Ptr()[ ISI_HEADER_SIZE + PNS_PIPE_RESET_REQ_OFFSET_PIPESTATEAFTERRESET ] );
    TPipeState nextState( stateAfter == PN_PIPE_DISABLE ? EEnabledDisabled : EEnabledEnabled );
    iPep.NextState( nextState );
    TInt retVal = iPep.SendPepResetResp( aReq ); // TODO: Why-iiiiiiiiiiiii retval?
    C_TRACE( ( _T( "DEnabled::PnsPepResetReq <-" ) ) );
    OstTrace1( TRACE_PIPE, DENABLED_PNSPEPRESETREQ_EXIT, "<DEnabled::PnsPepResetReq;retVal=%d", retVal );
    return retVal;

    }

//TBR
DEnableEnabled::DEnableEnabled(DPep& aPep) : DEnabled( aPep )
    {
    OstTrace1( TRACE_PIPE, DENABLEENABLED_DENABLEENABLED_ENTRY, "<>DEnableEnabled::DEnableEnabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DEnableEnabled::DEnabledEnabled <->" ) ) );
    }

void DEnableEnabled::EnterState()
    {
    OstTrace0( TRACE_PIPE, DENABLEENABLED_ENTERSTATE_ENTRY, ">DEnableEnabled::EnterState" );
    C_TRACE( ( _T( "DEnableEnabled::EnterState ->" ) ) );
    iPep.PipeEnableEnabled();
    C_TRACE( ( _T( "DEnableEnabled::EnterState <-" ) ) );
    OstTrace0( TRACE_PIPE, DENABLEENABLED_ENTERSTATE_EXIT, "<DEnableEnabled::EnterState" );
    }
//TBR

//********************** DEnabled methods *******************/
DEnabled::DEnabled(DPep& aPep) : DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DENABLED_DENABLED_ENTRY, "<>DEnabled::DEnabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DEnabled::DEnabled <->" ) ) );
    }

void DEnabled::EnterState()
    {
    OstTrace0( TRACE_PIPE, DENABLED_ENTERSTATE_ENTRY, ">DEnabled::EnterState" );
    C_TRACE( ( _T( "DEnabled::EnterState <->" ) ) );
    iPep.PipeEnabled();
    OstTrace0( TRACE_PIPE, DENABLED_ENTERSTATE_EXIT, "<DEnabled::EnterState" );
    }

TInt DEnabled::PnsPepDisableReq(
        const TDesC8& aReq
        )
    {

    OstTrace1( TRACE_PIPE, DENABLED_PNSPEPDISABLEREQ_ENTRY, ">DEnabled::PnsPepDisableReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DEnabled::PnsPepDisableReq ->" ) ) );
// TODO: Disabling for tx credits discarding    
    iPep.NextState( EDisabling );
    C_TRACE( ( _T( "DEnabled::PnsPepDisableReq <-" ) ) );
    TInt retVal = iPep.SendPepDisableResp( aReq );
    OstTrace1( TRACE_PIPE, DENABLED_PNSPEPDISABLEREQ_EXIT, "<DEnabled::PnsPepDisableReq;retVal=%d", retVal );
    return retVal;

    }

TInt DEnabled::PnsPepEnableReq(const TDesC8& aReq)
    {

    OstTrace1( TRACE_PIPE, DENABLED_PNSPEPENABLEREQ_ENTRY, ">DEnabled::PnsPepEnableReq;aReq=%x", ( TUint )&( aReq ) );
    C_TRACE( ( _T( "DEnabled::PnsPepEnableReq <->" ) ) );
    TInt retVal = iPep.SendPepEnableResp(aReq);
    OstTrace1( TRACE_PIPE, DENABLED_PNSPEPENABLEREQ_EXIT, "<DEnabled::PnsPepEnableReq;retVal=%d", retVal );
    return retVal;

    }

TInt DEnabled::RxPnsPipeData(const TDesC8& aData)
    {

    OstTrace1( TRACE_PIPE, DENABLED_RXPNSPIPEDATA_ENTRY, ">DEnabled::RxPnsPipeData;aData=%x", ( TUint )&( aData ) );
    C_TRACE( ( _T( "DEnabled::RxPnsPipeData <->" ) ) );
    TInt retVal = iPep.RxPipeData(aData);
    OstTrace1( TRACE_PIPE, DENABLED_RXPNSPIPEDATA_EXIT, "<DEnabled::RxPnsPipeData;retVal=%d", retVal );
    return retVal;

    }

TInt DEnabled::TxPnsPipeData(
        TDes8& aData
        )
    {

    OstTrace1( TRACE_PIPE, DENABLED_TXPNSPIPEDATA_ENTRY, ">DEnabled::TxPnsPipeData;aData=%x", ( TUint )&( aData ) );
    C_TRACE( ( _T( "DEnabled::TxPnsPipeData <->" ) ) );
    TInt retVal = iPep.TxPipeData( aData );
    OstTrace1( TRACE_PIPE, DENABLED_TXPNSPIPEDATA_EXIT, "<DEnabled::TxPnsPipeData;retVal=%d", retVal );
    return retVal;

    }

void DEnabled::PnsPipeEnabledInd()
    {

    OstTrace0( TRACE_PIPE, DENABLED_PNSPIPEENABLEDIND_ENTRY, ">DEnabled::PnsPipeEnabledInd" );
    C_TRACE( ( _T( "DEnabled::PnsPipeEnabledInd ->" ) ) );
    TRACE_ASSERT_ALWAYS;
    C_TRACE( ( _T( "DEnabled::PnsPipeEnabledInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DENABLED_PNSPIPEENABLEDIND_EXIT, "<DEnabled::PnsPipeEnabledInd" );

    }

//********************** DEnabling methods *******************/
DEnabling::DEnabling(DPep& aPep) : DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DENABLING_DENABLING_ENTRY, "<>DEnabling::DEnabling;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DEnabling::DEnabling <->" ) ) );
    }

void DEnabling::PnsPipeEnabledInd()
    {

    OstTrace0( TRACE_PIPE, DENABLING_PNSPIPEENABLEDIND_ENTRY, ">DEnabling::PnsPipeEnabledInd" );
    C_TRACE( ( _T( "DEnabling::PnsPipeEnabledInd ->" ) ) );
    iPep.NextState(EEnabled);
    C_TRACE( ( _T( "DEnabling::PnsPipeEnabledInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DENABLING_PNSPIPEENABLEDIND_EXIT, "<DEnabling::PnsPipeEnabledInd" );
    }
///////////////     /////////////////////////////////////////////////////
// TODO: error to pipe spec: we need this in enabling state cause in redirect no enabled_ind is coming! Create and error!!
////////////////////////////////////////////
void DEnabling::PnsPipeRedirectedInd(
        // None
        )
    {
    OstTrace0( TRACE_PIPE, DENABLING_PNSPIPEREDIRECTEDIND_ENTRY, ">DEnabling::PnsPipeRedirectedInd" );

    C_TRACE( ( _T( "DEnabling::PnsPipeRedirectedInd ->" ) ) );
    iPep.NextState( EEnabled );
    C_TRACE( ( _T( "DEnabling::PnsPipeRedirectedInd <-" ) ) );

    OstTrace0( TRACE_PIPE, DENABLING_PNSPIPEREDIRECTEDIND_EXIT, "<DEnabling::PnsPipeRedirectedInd" );
    }

///////////////     /////////////////////////////////////////////////////
// TODO: error to pipe spec: we need this in enabling state cause in redirect no enabled_ind is coming! Create and error!!
////////////////////////////////////////////


DEnabledEnabled::DEnabledEnabled(DPep& aPep) : DPipeState(aPep)
    {
    OstTrace1( TRACE_PIPE, DENABLEDENABLED_DENABLEDENABLED_ENTRY, "<>DEnabledEnabled::DEnabledEnabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DEnabledEnabled::DEnabledEnabled <->" ) ) );
    }

void DEnabledEnabled::PnsPipeResetInd() 
    {
    OstTrace0( TRACE_PIPE, DENABLEDENABLED_PNSPIPERESETIND_ENTRY, ">DEnabledEnabled::PnsPipeResetInd" );
    C_TRACE( ( _T( "DEnabledEnabled::PnsPipeResetInd ->" ) ) );
    //iPep.NextState( EEnabled );// TODO: to enabled?
    iPep.NextState( EEnableEnabled );
    C_TRACE( ( _T( "DEnabledEnabled::PnsPipeResetInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DENABLEDENABLED_PNSPIPERESETIND_EXIT, "<DEnabledEnabled::PnsPipeResetInd" );
    }

DEnabledDisabled::DEnabledDisabled(DPep& aPep) : DPipeState(aPep)
    {
    OstTrace1( TRACE_PIPE, DENABLEDDISABLED_DENABLEDDISABLED_ENTRY, "<>DEnabledDisabled::DEnabledDisabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DEnabledDisabled::DEnabledDisabled <->" ) ) );
    }

void DEnabledDisabled::EnterState()
    {
    OstTrace0( TRACE_PIPE, DENABLEDDISABLED_ENTERSTATE_ENTRY, "DEnabledDisabled::EnterState" );

    C_TRACE( ( _T( "DEnabledDisabled::EnterState ->" ) ) );
    iPep.PipeDisabling();
    C_TRACE( ( _T( "DEnabledDisabled::EnterState <-" ) ) );

    OstTrace0( TRACE_PIPE, DENABLEDDISABLED_ENTERSTATE_EXIT, "<DEnabledDisabled::EnterState" );
    }

void DEnabledDisabled::PnsPipeResetInd() 
    {
    OstTrace0( TRACE_PIPE, DENABLEDDISABLED_PNSPIPERESETIND_ENTRY, ">DEnabledDisabled::PnsPipeResetInd>" );
    C_TRACE( ( _T( "DEnabledDisabled::PnsPipeResetInd ->" ) ) );
    iPep.NextState( EDisabled );
    C_TRACE( ( _T( "DEnabledDisabled::PnsPipeResetInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DENABLEDDISABLED_PNSPIPERESETIND_EXIT, "<DEnabledDisabled::PnsPipeResetInd" );
    }

DDisabledEnabled::DDisabledEnabled(DPep& aPep) : DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DDISABLEDENABLED_DDISABLEDENABLED_ENTRY, "<>DDisabledEnabled::DDisabledEnabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DDisabledEnabled::DDisabledEnabled <->" ) ) );
    }
// TODO EnterState?
void DDisabledEnabled::PnsPipeResetInd() 
    {
    OstTrace0( TRACE_PIPE, DDISABLEDENABLED_PNSPIPERESETIND_ENTRY, ">DDisabledEnabled::PnsPipeResetInd>" );
    C_TRACE( ( _T( "DDisabledEnabled::PnsPipeResetInd ->" ) ) );
    iPep.NextState( EEnabled );
    C_TRACE( ( _T( "DDisabledEnabled::PnsPipeResetInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DDISABLEDENABLED_PNSPIPERESETIND_EXIT, "<DDisabledEnabled::PnsPipeResetInd" );
    }

DDisabledDisabled::DDisabledDisabled(DPep& aPep) : DPipeState( aPep )
    {
    OstTrace1( TRACE_PIPE, DDISABLEDDISABLED_DDISABLEDDISABLED_ENTRY, "<>DDisabledDisabled::DDisabledDisabled;aPep=%x", ( TUint )&( aPep ) );
    C_TRACE( ( _T( "DDisabledDisabled::DDisabledDisabled <->" ) ) );
    }

void DDisabledDisabled::PnsPipeResetInd() 
    {
    OstTrace0( TRACE_PIPE, DDISABLEDDISABLED_PNSPIPERESETIND_ENTRY, ">DDisabledDisabled::PnsPipeResetInd" );
    C_TRACE( ( _T( "DDisabledDisabled::PnsPipeResetInd ->" ) ) );
    iPep.NextState( EDisabled );
    C_TRACE( ( _T( "DDisabledDisabled::PnsPipeResetInd <-" ) ) );
    OstTrace0( TRACE_PIPE, DDISABLEDDISABLED_PNSPIPERESETIND_EXIT, "<DDisabledDisabled::PnsPipeResetInd" );
    }

//end of file
