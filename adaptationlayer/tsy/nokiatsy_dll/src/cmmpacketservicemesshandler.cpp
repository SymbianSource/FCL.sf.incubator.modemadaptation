/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "cmmpacketservicemesshandler.h"
#include "cmmnetmesshandler.h"
#include "cmmmessagerouter.h"
#include "cmmstaticutility.h"
#include "tsylogger.h"

#include <in_sock.h>
#include <tisi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <gpdsisi.h>
#include <pcktcs.h>
#include <gpds_sharedisi.h>
#include <ctsy/serviceapi/gsmerror.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpacketservicemesshandlerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
const TUint8 KSetAlwaysOnMode = 0x12;

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ==================== LOCAL FUNCTIONS =======================================
    //None

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::CMmPacketServiceMessHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMmPacketServiceMessHandler::CMmPacketServiceMessHandler()
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::C++ constructor");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_CMMPACKETSERVICEMESSHANDLER_TD, "CMmPacketServiceMessHandler::CMmPacketServiceMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::~CMmPacketServiceMessHandler
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmPacketServiceMessHandler::~CMmPacketServiceMessHandler()
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::~CMmPacketServiceMessHandler.\n" );
OstTrace0( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_CMMPACKETSERVICEMESSHANDLER_TD, "CMmPacketServiceMessHandler::~CMmPacketServiceMessHandler" );

    // Delete iPacketMesshandlerContextList object
    if ( iPacketMesshandlerContextList )
        {
        delete iPacketMesshandlerContextList;
        }
    }


// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMmPacketServiceMessHandler* CMmPacketServiceMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender,
    CMmPhoNetReceiver* aPhoNetReceiver,
    CMmMessageRouter* aMessageRouter )
    {
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::NewL" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_NEWL_TD, "CMmPacketServiceMessHandler::NewL" );

    CMmPacketServiceMessHandler* packetServiceMessHandler =
        new( ELeave ) CMmPacketServiceMessHandler();

    CleanupStack::PushL( packetServiceMessHandler );
    packetServiceMessHandler->iPhoNetSender = aPhoNetSender;
    packetServiceMessHandler->iMessageRouter = aMessageRouter;

    packetServiceMessHandler->ConstructL();

    aPhoNetReceiver->RegisterL(
        packetServiceMessHandler,
        PN_GPDS );
    aPhoNetReceiver->RegisterL(
        packetServiceMessHandler,
        PN_MODEM_NETWORK,
        NET_MODEM_REG_STATUS_GET_RESP );
    aPhoNetReceiver->RegisterL(
        packetServiceMessHandler,
        PN_MODEM_NETWORK,
        NET_MODEM_REG_STATUS_IND );
    aPhoNetReceiver->RegisterL(
        packetServiceMessHandler,
        PN_MODEM_NETWORK,
        NET_RAT_IND );
    aPhoNetReceiver->RegisterL(
        packetServiceMessHandler,
        PN_MODEM_NETWORK,
        NET_RAT_RESP );
    aPhoNetReceiver->RegisterL(
        packetServiceMessHandler,
        PN_MODEM_NETWORK,
        NET_RADIO_INFO_IND );

    CleanupStack::Pop( packetServiceMessHandler );

    return packetServiceMessHandler;
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::ConstructL()
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_CONSTRUCTL_TD, "CMmPacketServiceMessHandler::ConstructL" );

    iAlwaysOnMode = RMmCustomAPI::EAlwaysModeNeither;

    iPacketMesshandlerContextList = CMmPacketContextMesshandlerList::NewL();
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::ExtFuncL
// Entry point for messages received from Symbian OS layer (Message manager)
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::ExtFuncL(
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::ExtFuncL. IPC: %d", aIpc );
OstTrace1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_EXTFUNCL_TD, "CMmPacketServiceMessHandler::ExtFuncL;aIpc=%d", aIpc );

    TInt ret( KErrNotSupported );
    CMmDataPackage dataPackage;

    switch ( aIpc )
        {
        case EPacketNotifyContextAdded:
            {
            // DO NOTHING
            break;
            }
        case EPacketAttach:
            {
            ret = GpdsAttachReq();
            break;
            }
        case EPacketDetach:
            {
            ret = GpdsDetachReq();
            break;
            }
        case EPacketRejectActivationRequest:
            {
            ret = GpdsContextNwiActRejectReq();
            break;
            }
        case EPacketGetNtwkRegStatus:
            {
            ret = NetModemRegStatusGetReq();
            break;
            }
        case EPacketSetPrefBearer:
            {
            ret = SetPreferredBearer( *aDataPackage );
            break;
            }
        case EPacketSetAttachMode:
            {
            RPacketService::TAttachMode* dataPtr = NULL;
            aDataPackage->UnPackData ( &dataPtr );
            ret = SetAttachMode( dataPtr );
            break;
            }
        case EPacketGetAttachMode:
            {
            TBool calledOnInitPhase;
            aDataPackage->UnPackData( calledOnInitPhase );
            ret = GpdsConfigurationInfoReq( calledOnInitPhase );
            break;
            }
        case EPacketSetDefaultContextParams:
            {
            ret = SetDefaultContextParams( *aDataPackage );
            break;
            }
        case EPacketNotifyStatusChange:
            {
            ret = GpdsStatusReq();
            break;
            }
        case EPacketInitProxiesIPC:
            {
            ret = KMmPacketContextProxy1_DOS;
            break;
            }
        case ECustomSetAlwaysOnMode:
            {
            RMmCustomAPI::TSetAlwaysOnMode alwaysOnMode;
            aDataPackage->UnPackData( alwaysOnMode );

            ret = GpdsConfigureReq(
                KSetAlwaysOnMode,
                GPDS_ATTACH_MODE_DEFAULT,
                GPDS_MT_ACT_MODE_ACCEPT,
                GPDS_CLASSC_MODE_DEFAULT,
                alwaysOnMode );
            break;
            }
        default:
            {

            TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::ExtFuncL - Unknown IPC: %d",  aIpc);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_EXTFUNCL_TD, "CMmPacketServiceMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
            break;
            }
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::ReceiveMessageL
// Entry point for messages received from Domestic OS.
// Switches the message to the correct method.
// -----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::ReceiveMessageL(
    const TIsiReceiveC &aIsiMessage )
    {
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

TFLOGSTRING3("TSY: CMmPacketServiceMessHandler::ReceiveMessageL. resource:%d, msgId:%d", resource, messageId);
OstTraceExt2( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmPacketServiceMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch ( resource )
        {
        case PN_MODEM_NETWORK:
            {
            switch ( messageId )
                {
                case NET_MODEM_REG_STATUS_GET_RESP:
                case NET_MODEM_REG_STATUS_IND:
                    {
                    NetModemRegStatusGetRespOrInd( aIsiMessage );
                    break;
                    }
                case NET_RAT_IND:
                case NET_RAT_RESP:
                    {
                    NetRatIndOrResp( aIsiMessage );
                    break;
                    }
                case NET_RADIO_INFO_IND:
                    {
                    NetRadioInfoInd( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::ReceiveMessageL. default in case PN_MODEM_NETWORK");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmPacketServiceMessHandler::ReceiveMessageL, default in case PN_MODEM_NETWORK" );
                    break;
                    }
                }
            break;
            }
        case PN_GPDS:
            {
            switch ( messageId )
                {
                case GPDS_CONTEXT_ID_CREATE_IND:
                    {
                    GpdsContextIdCreateInd( aIsiMessage );
                    break;
                    }
                case GPDS_CONFIGURATION_INFO_RESP:
                    {
                    GpdsConfigurationInfoResp( aIsiMessage );
                    break;
                    }
                case GPDS_CONFIGURATION_INFO_IND:
                    {
                    GpdsConfigurationInfoInd( aIsiMessage );
                    break;
                    }
                case GPDS_TRANSFER_STATUS_IND:
                    {
                    GpdsTransferStatusInd( aIsiMessage );
                    break;
                    }
                case GPDS_CONFIGURE_RESP:
                    {
                    GpdsConfigureResp( aIsiMessage );
                    break;
                    }
                case GPDS_STATUS_RESP:
                    {
                    GpdsStatusResp( aIsiMessage );
                    break;
                    }
                case GPDS_ATTACH_RESP:
                    {
                    GpdsAttachResp( aIsiMessage );
                    break;
                    }
                case GPDS_ATTACH_IND:
                    {
                    GpdsAttachInd();
                    break;
                    }
                case GPDS_DETACH_RESP:
                    {
                    GpdsDetachResp( aIsiMessage );
                    break;
                    }
                case GPDS_DETACH_IND:
                    {
                    GpdsDetachInd();
                    break;
                    }
                case GPDS_CONTEXT_NWI_ACT_REQUEST_IND:
                    {
                    GpdsContextNwiActRequestInd( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_NWI_ACT_REJECT_RESP:
                    {
                    GpdsContextNwiActRejectResp( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_CONFIGURE_RESP:
                    {
                    GpdsContextConfigureResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::SpecificPacketServiceMessageReceived: KErrNotFound");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmPacketServiceMessHandler::ReceiveMessageL, KErrNotFound" );
                    break;
                    }
                }
            break;
            }
        default:
            {
            // No appropriate handler methods for ISI-message found.
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::SpecificPacketServiceMessageReceived no handler for the message id");
OstTrace0( TRACE_NORMAL,  DUP3_CMMPACKETSERVICEMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmPacketServiceMessHandler::ReceiveMessageL, SpecificPacketServiceMessageReceived no handler for the message id" );
            // server not known
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsStatusReq
// Constructs GPDS_STATUS_REQ ISI message and sends it to Phonet
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::GpdsStatusReq() const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsStatusReq.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSSTATUSREQ_TD, "CMmPacketServiceMessHandler::GpdsStatusReq" );

    // Append data for ISI message
    TBuf8<KBuffSizeOneByte> data;

    // Message sent via Phonet
    // Transaction id ( 0xDD - KDummyTrIdDD ) is not needed to store, dummy is enough
    // create gpds status request
    return iPhoNetSender->Send( PN_GPDS, KDummyTrIdDD, GPDS_STATUS_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsStatusResp
// Breaks a GPDS_STATUS_RESP ISI message
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsStatusResp(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsStatusResp.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSSTATUSRESP_TD, "CMmPacketServiceMessHandler::GpdsStatusResp" );

    const TUint8 attachStatus( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_STATUS_RESP_OFFSET_ATTACHSTATUS ) );
    const TUint8 transfStatus( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_STATUS_RESP_OFFSET_TRANSFERSTATUS ) );

    TFLOGSTRING3("TSY: CMmPacketServiceMessHandler::GpdsStatusResp.\n  \t Attach Status:%d\n\t Transfer Status:%d\n", attachStatus, transfStatus );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_GPDSSTATUSRESP_TD, "CMmPacketServiceMessHandler::GpdsStatusResp;attachStatus=%hhu;transfStatus=%hhu", attachStatus, transfStatus );

    CompleteGpdsStatusRequest( attachStatus, transfStatus );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsAttachReq
// Constructs GPDS_ATTACH_REQ ISI message and sends it to Phonet.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::GpdsAttachReq() const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsAttachReq.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSATTACHREQ_TD, "CMmPacketServiceMessHandler::GpdsAttachReq" );

    // Append data for ISI message
    TBuf8<1> data;
    data.Append( GPDS_FOLLOW_OFF );

    // Message sent via Phonet
    return iPhoNetSender->Send(
        PN_GPDS, GPDS_ATTACH_REQ, GPDS_ATTACH_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsAttachResp
// Breaks a GPDS_ATTACH_RESP ISI message
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsAttachResp(
    const TIsiReceiveC& aIsiMessage ) const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsAttachResp.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSATTACHRESP_TD, "CMmPacketServiceMessHandler::GpdsAttachResp" );

    if ( GPDS_ATTACH_REQ == aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_ATTACH_RESP_OFFSET_UTID ) )
        {
        const TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_ATTACH_RESP_OFFSET_STATUS ) );
        const TUint8 cause( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_ATTACH_RESP_OFFSET_CAUSE ) );
        TInt errorValue( KErrNone );

        if ( GPDS_OK != status )
            {
            // Map CS error code to EPOC error code
            errorValue = CMmStaticUtility::PacketDataCSCauseToEpocError(
                cause );
            }
        //no else

        TFLOGSTRING3("TSY: CMmPacketServiceMessHandler::GpdsAttachResp.\n  \t Status:%d\n\t Cause:%d\n", status, cause );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_GPDSATTACHRESP_TD, "CMmPacketServiceMessHandler::GpdsAttachResp;status=%hhu;cause=%hhu", status, cause );

        //Complete Attach
        iMessageRouter->Complete(
            EPacketAttach,
            errorValue );
        }
     //no else
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsAttachInd
// Breaks a GPDS_ATTACH_IND ISI message
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsAttachInd() const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsAttachInd.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSATTACHIND_TD, "CMmPacketServiceMessHandler::GpdsAttachInd" );

    RPacketService::TStatus status( RPacketService::EStatusAttached );
    TBool isResumed( EFalse );

    CMmDataPackage data;
    data.PackData( &status, &isResumed );

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsAttachInd. Complete\n\t Status:%d\n", status );

    iMessageRouter->Complete(
        EPacketNotifyStatusChange,
        &data,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsDetachReq
// Constructs GPDS_DETACH_REQ ISI message and sends it to Phonet.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::GpdsDetachReq() const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsDetachReq.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSDETACHREQ_TD, "CMmPacketServiceMessHandler::GpdsDetachReq" );

    // Create isi message
    TBuf8<SIZE_GPDS_DETACH_REQ + SIZE_GPDS_CONDITIONAL_DETACH_INFO>
        messageData;

    // Add filler
    messageData.Append( KGpdsPadding );
    // Add number of subblocks (GPDS_CONDITIONAL_DETACH_INFO)
    TUint8 subBlockCount( 1 );
    messageData.Append( subBlockCount );

    // Add conditional detach info sub block
    TIsiSubBlock gpdsConditionalDetachinfoSb(
        messageData,
        GPDS_CONDITIONAL_DETACH_INFO,
        EIsiSubBlockTypeId8Len8 );

    // Add fillers
    messageData.Append( KGpdsPadding );
    messageData.Append( KGpdsPadding );
    // Complete subblock
    gpdsConditionalDetachinfoSb.CompleteSubBlock();

    // Message sent via Phonet
    return iPhoNetSender->Send(
        PN_GPDS, GPDS_DETACH_REQ, GPDS_DETACH_REQ, messageData );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsDetachResp
// Breaks a GPDS_DETACH_RESP ISI message
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsDetachResp(
    const TIsiReceiveC& aIsiMessage ) const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsDetachResp.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSDETACHRESP_TD, "CMmPacketServiceMessHandler::GpdsDetachResp" );

    if ( GPDS_DETACH_REQ == aIsiMessage.Get8bit(
        ISI_HEADER_SIZE +  GPDS_DETACH_RESP_OFFSET_UTID ) )
        {
        TInt errorValue( KErrNone );
        const TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_DETACH_RESP_OFFSET_STATUS ) );

        if ( GPDS_OK != status )
            {
            // Error is network failure
            errorValue = CMmStaticUtility::EpocErrorCode(
                KErrGeneral, KErrGsmMMNetworkFailure );
            }

        TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsDetachResp.\n\t Status:%d\n", status );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_GPDSDETACHRESP_TD, "CMmPacketServiceMessHandler::GpdsDetachResp;status=%hhu", status );

        //Complete
        iMessageRouter->Complete(
            EPacketDetach,
            errorValue );
        }
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsDetachInd
// Breaks a GPDS_DETACH_IND ISI message
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsDetachInd() const
    {

    // Params for completion
    RPacketService::TStatus status = RPacketService::EStatusUnattached;

    // is resumed from suspend
    TBool isResumed( EFalse );

    CMmDataPackage data;
    data.PackData( &status, &isResumed );

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsDetachInd. Complete\n\t status:%d\n", status );

OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSDETACHIND_TD, "CMmPacketServiceMessHandler::GpdsDetachInd;status=%hhu", status );
    iMessageRouter->Complete(
        EPacketNotifyStatusChange,
        &data,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::NetModemRegStatusGetReq
// Constructs and sends NET_MODEM_REG_STATUS_GET_REQ ISI message.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::NetModemRegStatusGetReq() const
    {
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::NetModemRegStatusGetReq" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_NETMODEMREGSTATUSGETREQ_TD, "CMmPacketServiceMessHandler::NetModemRegStatusGetReq" );

    // Data buffer for transactionId and Message Id.
    TBuf8<2> data;
    data.Append( NET_MODEM_REG_STATUS_GET_REQ );
    data.Append( NET_MODEM_REG_STATUS_GET_REQ );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_NETWORK );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    return ( iPhoNetSender->Send( isimsg.Complete() ) );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd
// Breaks an ISI message. NET_MODEM_REG_STATUS_GET_RESP or
// NET_MODEM_REG_STATUS_IND -ISI message
// ----------------------------------------------------------------------------
void CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_NETMODEMREGSTATUSGETRESPORIND_TD, "CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd" );

    TUint sbStartOffSet( 0 );
    TInt ret( 0 );
    TBool isNetModemRegStatusInd( NET_MODEM_REG_STATUS_IND ==
        aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    TUint8 regStatus( 0 );
    ret = iMessageRouter->GetNetMessHandler()->ExtractNetworkRegistrationStatus(
        aIsiMessage,
        regStatus
        );

    if ( KErrNone == ret )
        {
        if ( !isNetModemRegStatusInd )
            {
            CMmDataPackage data;
            const TUint8 successCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE +
                NET_MODEM_REG_STATUS_GET_RESP_OFFSET_SUCCESSCODE ) );

TFLOGSTRING3("TSY: CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - NetModemRegStatusGetResp, Registration Status:%d Success Code:%d", regStatus, successCode);
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_NETMODEMREGSTATUSGETRESPORIND_TD, "CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - NetModemRegStatusGetResp, Registration Status=%hhu Success Code=%hhu", regStatus, successCode );

            // Map registration status mapping
            RPacketService::TRegistrationStatus regStatusClient(
                MapNtwkRegStatus( regStatus, successCode ) );

            data.PackData( &regStatusClient );

            // Complete
            iMessageRouter->Complete(
                EPacketGetNtwkRegStatus,
                &data,
                KErrNone );
            }
        else
            {
TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - NetModemRegStatusInd, Registration Status:%d", regStatus);
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_NETMODEMREGSTATUSGETRESPORIND_TD, "CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - NetModemRegStatusInd, Registration Status=%hhu", regStatus );

            CMmDataPackage data;

            // Map Status
            RPacketService::TRegistrationStatus regstatus(
                MapNtwkRegStatus( regStatus ) );

            // Pack data for completion
            data.PackData( &regstatus );

            // Complete
            iMessageRouter->Complete(
                EPacketNotifyChangeOfNtwkRegStatus,
                &data,
                KErrNone );
            }
        }
    // No else

    sbStartOffSet = 0;

    // NET_MODEM_GSM_REG_INFO sub block
    if( isNetModemRegStatusInd )
        {
        ret = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_IND,
            NET_MODEM_GSM_REG_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );
        }
    else
        {
        ret = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_GET_RESP,
            NET_MODEM_GSM_REG_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );
        }

    // Check that NET_MODEM_GSM_REG_INFO sub block is present
    if ( KErrNone == ret )
        {
        // Get cs services
        const TBool csServices( aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_GSM_REG_INFO_OFFSET_CSSERVICES ) );
        // Get gprs support in cell
        const TBool gprsSupported( aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_GSM_REG_INFO_OFFSET_GPRSSUPPORTINCELL ) );
        // Get edge gprs support
        const TBool edgeGprsSupport( aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_GSM_REG_INFO_OFFSET_EGPRSSUPPORTINCELL ) );

        const TBool dtmSupportInCell( aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_GSM_REG_INFO_OFFSET_DTMSUPPORTINCELL ) );

        // save last DTM support
        iLastDtmSupportInCell = dtmSupportInCell;

TFLOGSTRING3("TSY: CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - CS Services:%d GPRS support in cell:%d", csServices, gprsSupported);
TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - EdgeGprsSupport:%d", edgeGprsSupport);
TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - DtmSupport:%d", dtmSupportInCell);
OstTraceExt2( TRACE_NORMAL,  DUP3_CMMPACKETSERVICEMESSHANDLER_NETMODEMREGSTATUSGETRESPORIND_TD, "CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - CS Services=%hhu GPRS support in cell=%hhu", csServices, gprsSupported );
OstTraceExt1( TRACE_NORMAL,  DUP4_CMMPACKETSERVICEMESSHANDLER_NETMODEMREGSTATUSGETRESPORIND_TD, "CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - EdgeGprsSupport=%hhu", edgeGprsSupport );
OstTraceExt1( TRACE_NORMAL,  DUP5_CMMPACKETSERVICEMESSHANDLER_NETMODEMREGSTATUSGETRESPORIND_TD, "CMmPacketServiceMessHandler::NetModemRegStatusGetRespOrInd - DtmSupport=%hhu", dtmSupportInCell );

        // Complete notification to the active extension
        CompleteGsmRegistrationInfo(
            csServices,
            gprsSupported,
            edgeGprsSupport );

        // Complete EPacketNotifyTransferCapsIPC
        CompletePacketNotifyTransferCaps(
            edgeGprsSupport );

        // Get gprs network mode
        const TUint8 gprsNetworkMode( aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_GSM_REG_INFO_OFFSET_GPRSNETWORKMODE ) );

        CompleteNotifyMSClassChange( gprsNetworkMode );
        }
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsConfigureReq
// Constructs and sends GPDS_CONFIGURE_REQ ISI message.
// This request is used to set attach mode, class C mode or/and MT context
// activation mode. Mode to be change should be set to other value than default
// ( GPDS_ATTACH_MODE_DEFAULT, GPDS_MT_ACT_MODE_DEFAULT or GPDS_CLASSC_MODE_DEFAULT )
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::GpdsConfigureReq(
    const TUint8 aTransactionId,
    const TUint8 aAttachMode,
    const TUint8 aMtContextActivationMode,
    const TUint8 aClassCMode,
    RMmCustomAPI::TSetAlwaysOnMode alwaysOnMode )
    {

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsConfigureReq.\n\t Attach Mode:%d\n", aAttachMode );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGUREREQ_TD, "CMmPacketServiceMessHandler::GpdsConfigureReq;aAttachMode=%hhu", aAttachMode );

    TUint8 aAolContext = GPDS_AOL_CTX_NOT_ACTIVE;
    iAlwaysOnMode = alwaysOnMode;

    TFLOGSTRING3("TSY: CMmPacketServiceMessHandler::GpdsConfigureReq.\n  \t Mt Context Activation Mode:%d\n\t Class C Mode:%d\n", aMtContextActivationMode, aClassCMode );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGUREREQ_TD, "CMmPacketServiceMessHandler::GpdsConfigureReq;aMtContextActivationMode=%hhu;aClassCMode=%hhu", aMtContextActivationMode, aClassCMode );

    switch ( iAlwaysOnMode )
        {
        case RMmCustomAPI::EAlwaysModeHPLMN :
            {
            aAolContext = GPDS_AOL_CTX_HPLMN_ACTIVE;
            break;
            }
        case RMmCustomAPI::EAlwaysModeVPLMN:
            {
            aAolContext = GPDS_AOL_CTX_VPLMN_ACTIVE;
            break;
            }
        case RMmCustomAPI::EAlwaysModeBoth:
            {
            aAolContext = GPDS_AOL_CTX_ACTIVE;
            break;
            }
        case RMmCustomAPI::EAlwaysModeNeither:
            {
            aAolContext = GPDS_AOL_CTX_NOT_ACTIVE;
            break;
            }
        default:
            {

            TFLOGSTRING2("CMmPacketServiceMessHandler::GpdsConfigureReq.\n\t AOL Context Mode:%d\n", aAolContext);
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGUREREQ_TD, "CMmPacketServiceMessHandler::GpdsConfigureReq;aAolContext=%hhu", aAolContext );

            break;
            }
        }

    // Introduce data buffer for needed Gpds Configure Request data
    TBuf8<KBuffSizeFourBytes> data;
    data.Append( aAttachMode );
    data.Append( aMtContextActivationMode );
    data.Append( aClassCMode );
    data.Append( aAolContext );

    TUint8 transactionId( 0 );

    // Create gpds configure request, 0xDC - KDummyTrIdDC is dummy for identifying calling method
    if ( KDummyTrIdDC == aTransactionId )
        {
        transactionId = KDummyTrIdDC;
        }
    else if ( KSetAlwaysOnMode == aTransactionId )
        {
        transactionId = KSetAlwaysOnMode;
        }
    else
        {
        transactionId = GPDS_CONFIGURE_REQ;
        }

    // Message sent via Phonet
    return iPhoNetSender->Send(
        PN_GPDS, transactionId, GPDS_CONFIGURE_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsConfigureResp
// Breaks a GPDS_CONFIGURE_RESP ISI message.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsConfigureResp(
    const TIsiReceiveC& aIsiMessage ) const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsConfigureResp.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGURERESP_TD, "CMmPacketServiceMessHandler::GpdsConfigureResp" );

    TInt errorValue( KErrNone );
    TBool called( EFalse );

    const TUint8 transactionId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONFIGURE_RESP_OFFSET_UTID ) );

    // Is this request called
    if ( GPDS_CONFIGURE_REQ == transactionId
        || KDummyTrIdDC == transactionId
        || KSetAlwaysOnMode == transactionId)
        // 0xDC is dummy for identifying calling method
        {
        called = ETrue;
        }
    //no else

    if ( called )
        {
        // Get status
        const TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONFIGURE_RESP_OFFSET_STATUS ) );

        TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsConfigureResp.\n\t Status:%d\n", status );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGURERESP_TD, "CMmPacketServiceMessHandler::GpdsConfigureResp;status=%hhu", status );

        if ( GPDS_CONFIGURE_REQ == transactionId )
            {
            if ( GPDS_OK != status )
                {
                errorValue = KErrGeneral;
                }
            //no else

            // Complete
            iMessageRouter->Complete(
                EPacketSetAttachMode,
                errorValue );
            }
        // if called SetPrefBearer 0xDC is dummy for identifying calling method
        else if ( KDummyTrIdDC == transactionId )
            {
            // error mapping
            if ( GPDS_OK != status )
                {
                errorValue = KErrGeneral;
                }
            //no else

            // Complete
            iMessageRouter->Complete(
                EPacketSetPrefBearer,
                errorValue );
            }

         else if ( KSetAlwaysOnMode == transactionId )
            {

            if ( GPDS_OK != status )
                {
                errorValue = KErrWrite;
                }

            // Complete
            iMessageRouter->Complete(
                ECustomSetAlwaysOnMode,
                errorValue );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsTransferStatusInd
// Breaks a GPDS_TRANSFER_STATUS_IND ISI message
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsTransferStatusInd(
    const TIsiReceiveC& aIsiMessage )
    {

    const TUint8 transferCause( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_TRANSFER_STATUS_IND_OFFSET_TRANSFERCAUSE) );

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsTransferStatusInd.\n  \t Transfer cause:%d\n", transferCause );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSTRANSFERSTATUSIND_TD, "CMmPacketServiceMessHandler::GpdsTransferStatusInd;transferCause=%hhu", transferCause );

    CompleteTransferStatusIndication( transferCause );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsContextIdCreateInd
// Breaks a GPDS_CONTEXT_ID_CREATE_IND ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsContextIdCreateInd(
    const TIsiReceiveC& aIsiMessage ) const
    {
    // Get context id from message
    const TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_ID_CREATE_IND_OFFSET_CID ) );

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsContextIdCreateInd.\n\t Context Id:%d\n", contextId );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONTEXTIDCREATEIND_TD, "CMmPacketServiceMessHandler::GpdsContextIdCreateInd;contextId=%hhu", contextId );

    // initial variables
    CMmDataPackage data;
    TInfoName contextName;
    contextName.Zero();

    // Get Contextname from CMmPacketMesshandlerContextList
    TInt ret( iPacketMesshandlerContextList->
        GetContextNameByContextId( contextId, contextName ) );

    // Dialup context
    if( KErrArgument == ret )
        {
        iPacketMesshandlerContextList->SetDialUpContextId( contextId );
        }
    //no else

    data.PackData( &contextName, &ret );

    iMessageRouter->Complete(
        EPacketNotifyContextAdded,
        &data,
        KErrNone);
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsConfigurationInfoInd
// Breaks a GPDS_CONFIGURATION_INFO_IND ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsConfigurationInfoInd(
    const TIsiReceiveC& aIsiMessage ) const
    {

    CMmDataPackage data;

    // Get class C mode
    const TUint8 classC( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONFIGURATION_INFO_IND_OFFSET_CLASSCMODE) );

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsConfigurationInfoInd.\n\t Class C Mode:%d\n", classC );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGURATIONINFOIND_TD, "CMmPacketServiceMessHandler::GpdsConfigurationInfoInd;classC=%hhu", classC );

    RPacketService::TPreferredBearer bearer(
        RPacketService::EBearerPacketSwitched );

    // set bearer
    if ( GPDS_CLASSC_MODE_GSM == classC )
        {
        bearer = RPacketService::EBearerCircuitSwitched;
        }

    // Pack data
    data.PackData( &bearer );

    // Complete
    iMessageRouter->Complete(
        EPacketNotifyAttachModeChange,
        &data,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsContextNwiActRejectReq
// Creates GPDS_CONTEXT_NWI_ACT_REJECT_REQ ISI message.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::GpdsContextNwiActRejectReq() const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsContextNwiActRejectReq.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONTEXTNWIACTREJECTREQ_TD, "CMmPacketServiceMessHandler::GpdsContextNwiActRejectReq" );

    // Introduce data buffer for needed GpdsContextNwiActRejectReq data
    // Needed length is 1
    TBuf8<1> data;
    data.Append( GPDS_CAUSE_ACT_REJECT );

    // Message sent via Phonet
    return iPhoNetSender->Send(
        PN_GPDS,
        GPDS_CONTEXT_NWI_ACT_REJECT_REQ,
        GPDS_CONTEXT_NWI_ACT_REJECT_REQ,
        data );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsContextNwiActRejectResp
// Breaks a GPDS_CONTEXT_NWI_ACT_REJECT_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsContextNwiActRejectResp(
    const TIsiReceiveC& /*aIsiMessage*/) const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsContextNwiActRejectResp.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONTEXTNWIACTREJECTRESP_TD, "CMmPacketServiceMessHandler::GpdsContextNwiActRejectResp" );

    iMessageRouter->Complete(
        EPacketRejectActivationRequest,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsContextNwiActRequestInd
// Breaks a GPDS_CONTEXT_NWI_ACT_REQUEST_IND ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsContextNwiActRequestInd(
    const TIsiReceiveC& aIsiMessage )
    {

    RPacketContext::TContextConfigGPRS completeNotifyContextActivationReq;
    // DataPackage
    CMmDataPackage data;

    // Read pdp type from isi message
    const TUint8 pdpType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_NWI_ACT_REQUEST_IND_OFFSET_PDPTYPE ) );

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsContextNwiActRequestInd. PdpType:%d", pdpType );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONTEXTNWIACTREQUESTIND_TD, "CMmPacketServiceMessHandler::GpdsContextNwiActRequestInd;pdpType=%hhu", pdpType );

    TPtrC8 pdpAddress;
    TPtrC8 accessPointName;
    TUint sbStartOffSet( 0 );

    // GPDS_PDP_ADDRESS_INFO sub block
    TInt retValue ( aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_NWI_ACT_REQUEST_IND,
        GPDS_PDP_ADDRESS_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) );

    if ( KErrNone == retValue )
        {
        TUint8 addressLen( aIsiMessage.Get8bit(
            sbStartOffSet + GPDS_PDP_ADDRESS_INFO_OFFSET_ADDRLENGTH ) );
        // Read pdp address from GPDS_PDP_ADDRESS_INFO sub block
        pdpAddress.Set( aIsiMessage.GetData(
            sbStartOffSet + GPDS_PDP_ADDRESS_INFO_OFFSET_ADDRESS,
            addressLen ) );
        }
    //no else

    if ( 0 < pdpAddress.Length() )
        {
        CMmStaticUtility::ConvertIPAddressToClient(
            pdpAddress,
            completeNotifyContextActivationReq.iPdpAddress );
        }
    //no else

    // GPDS_APN_INFO sub block
    retValue = aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_NWI_ACT_REQUEST_IND,
        GPDS_APN_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet );

    if ( KErrNone == retValue )
        {
        TUint8 apnSize( aIsiMessage.Get8bit(
            sbStartOffSet + GPDS_APN_INFO_OFFSET_ADDRLENGTH ) );
        // Read access point name from GPDS_APN_INFO sub block
        accessPointName.Set( aIsiMessage.GetData(
            sbStartOffSet + GPDS_APN_INFO_OFFSET_APNNAME, apnSize ) );
        }
    //no else

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsContextNwiActRequestInd.\n  \t Pdp Type:%d\n", pdpType );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_GPDSCONTEXTNWIACTREQUESTIND_TD, "CMmPacketServiceMessHandler::GpdsContextNwiActRequestInd;pdpType=%hhu", pdpType );

    // Map server pdp type to client pdp type
    CMmStaticUtility::MapPdpTypeToClient(
        completeNotifyContextActivationReq.iPdpType, pdpType );

    // Copy aAccessPointName from TDesC8& to RPacketContext::TGSNAddress
    completeNotifyContextActivationReq.iAccessPointName.Copy(
        accessPointName );

    // Packdata for completion
    data.PackData( &completeNotifyContextActivationReq );

    // Complete indication
    iMessageRouter->Complete(
        EPacketNotifyContextActivationRequested,
        &data,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsConfigurationInfoReq
// Constructs and sends GPDS_CONFIGURATION_INFO_REQ ISI message.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::GpdsConfigurationInfoReq(
    TBool aCalledOnInitPhase ) const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsConfigurationInfoReq.\n\t" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGURATIONINFOREQ_TD, "CMmPacketServiceMessHandler::GpdsConfigurationInfoReq" );

    TBuf8<1> data;
    TUint8 transactionId( 0 );

    // Create gpds configuration info request
    if ( aCalledOnInitPhase )
        {
        //0xDF -KDummyTrIdDF is dummy transaction id
        transactionId = KDummyTrIdDF;
        }
    else
        {
        transactionId = GPDS_CONFIGURATION_INFO_REQ;
        }

    // Message sent via Phonet
    return iPhoNetSender->Send(
        PN_GPDS, transactionId, GPDS_CONFIGURATION_INFO_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsConfigurationInfoResp
// Breaks a GPDS_CONFIGURATION_INFO_RERESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsConfigurationInfoResp(
    const TIsiReceiveC& aIsiMessage ) const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsConfigurationInfoResp.\n\t" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGURATIONINFORESP_TD, "CMmPacketServiceMessHandler::GpdsConfigurationInfoResp" );

    CMmDataPackage data;
    TBool ret( EFalse );

    // Is this request called
    if ( GPDS_CONFIGURATION_INFO_REQ == aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONFIGURATION_INFO_RESP_OFFSET_UTID ) )
        {
        ret = ETrue;
        }
    //no else

    if ( ret )
        {
        // Get attach mode
        const TUint8 attachMode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONFIGURATION_INFO_RESP_OFFSET_ATTACHMODE ) );

        TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::GpdsConfigurationInfoResp.\n\t Attach mode:%d\n", attachMode );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_GPDSCONFIGURATIONINFORESP_TD, "CMmPacketServiceMessHandler::GpdsConfigurationInfoResp;attachMode=%hhu", attachMode );

        RPacketService::TAttachMode attachModeForCompletion(
            RPacketService::EAttachWhenNeeded );

        if ( GPDS_ATTACH_MODE_AUTOMATIC == attachMode )
            {
            attachModeForCompletion = RPacketService::EAttachWhenPossible;
            }
        //no else

        data.PackData( &attachModeForCompletion );
        iMessageRouter->Complete(
            EPacketGetAttachMode,
            &data,
            KErrNone );
        }
    // If transaction id is 0xDF - KDummyTrIdDF, this is response to the request made during
    // initialisation
    else if ( KDummyTrIdDF == aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        GPDS_CONFIGURATION_INFO_RESP_OFFSET_UTID) )
        {
        // Get class c mode
        const TUint8 classCMode ( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        GPDS_CONFIGURATION_INFO_RESP_OFFSET_CLASSCMODE ) );

        RPacketService::TPreferredBearer bearer(
            RPacketService::EBearerPacketSwitched );

        if ( GPDS_CLASSC_MODE_GSM == classCMode )
            {
            bearer = RPacketService::EBearerCircuitSwitched;
            }
        //no else

        // Pack data
        data.PackData( &bearer );
        // Complete
        iMessageRouter->Complete(
            EPacketNotifyAttachModeChange,
            &data,
            KErrNone );
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsContextConfigureResp
// Response for default dial-up context configuration.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::GpdsContextConfigureResp(
    const TIsiReceiveC& aIsiMessage ) const
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsContextConfigureResp.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONTEXTCONFIGURERESP_TD, "CMmPacketServiceMessHandler::GpdsContextConfigureResp" );

    TBool called( EFalse );

    if ( GPDS_CONTEXT_CONFIGURE_REQ == aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_UTID ) )
        {
        called = ETrue;
        }
    //no else

    if ( called )
        {
        const TUint8 status ( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_STATUS ) );

        TInt errorCause ( KErrGeneral );

        if ( GPDS_OK == status )
            {
            errorCause = KErrNone;
            }
        //no else

        iMessageRouter->Complete(
            EPacketSetDefaultContextParams,
            errorCause );
        }
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GpdsContextConfigureReq
// Request for default dial-up context configuration.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::GpdsContextConfigureReq(
    TUint8 aDataCompression,
    TUint8 aHeaderCompression,
    TBuf8<KAccessPointBuffer256> aAccessPointName,
    TUint8 aPdpType )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GpdsContextConfigureReq.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GPDSCONTEXTCONFIGUREREQ_TD, "CMmPacketServiceMessHandler::GpdsContextConfigureReq" );

    // Append data for ISI message
    TBuf8<SIZE_GPDS_CONTEXT_CONFIGURE_REQ + SIZE_GPDS_COMP_INFO +
        SIZE_GPDS_APN_INFO + GPDS_MAX_APN_STRING_SIZE +
        SIZE_GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO> messageData;

    messageData.Append( GPDS_CID_ALL );
    messageData.Append( aPdpType );
    messageData.Append( GPDS_CONT_TYPE_NORMAL );
    messageData.Append( KGpdsPadding ); //primary CID
    messageData.Append( KGpdsPadding ); //filler
    // number of subblocks
    // GPDS_APN_INFO, GPDS_COM_INFO and GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO
    TUint8 subBlockCount( 3 );
    messageData.Append( subBlockCount );

    // Set the APN info
    TInt apnLength( aAccessPointName.Length() );

    // Add GPDS_APN_INFO subblock
    TIsiSubBlock gpdsApnInfoSb(
        messageData,
        GPDS_APN_INFO,
        EIsiSubBlockTypeId8Len8 );

    messageData.Append( static_cast<TUint8>( apnLength ) );
    messageData.Append( aAccessPointName );
    gpdsApnInfoSb.CompleteSubBlock();

    // Add GPDS_COMP_INFO subblock
    TIsiSubBlock gpdsCompInfoSb(
        messageData,
        GPDS_COMP_INFO,
        EIsiSubBlockTypeId8Len8 );

    messageData.Append( aDataCompression );
    messageData.Append( aHeaderCompression );
    gpdsCompInfoSb.CompleteSubBlock();

    // Add GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO sub block
    TIsiSubBlock radioActivitySubBlock(
        messageData,
        GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO,
        EIsiSubBlockTypeId8Len8 );

    // contains no parameters
    messageData.Append( KGpdsPadding );
    messageData.Append( KGpdsPadding );
    radioActivitySubBlock.CompleteSubBlock();

    // Send Isi message via Phonet
    return iPhoNetSender->Send(
        PN_GPDS, GPDS_CONTEXT_CONFIGURE_REQ,
        GPDS_CONTEXT_CONFIGURE_REQ,
        messageData );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::MapNtwkRegStatus
// Maps network reg status to Packet Data reg status.
// ----------------------------------------------------------------------------
//
RPacketService::TRegistrationStatus CMmPacketServiceMessHandler::MapNtwkRegStatus(
    const TUint8 aRegistrationStatus,
    const TUint8 aSuccessCode ) const
    {

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::MapNtwkRegStatus. aRegistrationStatus: %d", aRegistrationStatus);
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_MAPNTWKREGSTATUS_TD, "CMmPacketServiceMessHandler::MapNtwkRegStatus;aRegistrationStatus=%hhu", aRegistrationStatus );

    RPacketService::TRegistrationStatus registrationStatus(
        RPacketService::EUnknown );

    switch ( aRegistrationStatus )
        {
        case NET_REG_STATUS_HOME:
            {
            // Registration status is ERegisteredOnHomeNetwork
            registrationStatus = RPacketService::ERegisteredOnHomeNetwork;
            break;
            }
        case NET_REG_STATUS_ROAM:
            {
            // Registration status is ERegisteredRoaming
            registrationStatus = RPacketService::ERegisteredRoaming;
            break;
            }
        case NET_REG_STATUS_NOSERV:
            {
            // if aSuccessCode is KNetCauseNetNotFound or
            // KNetCauseNoSelectedNetwork
            if ( NET_CAUSE_NET_NOT_FOUND == aSuccessCode ||
                 NET_CAUSE_NO_SELECTED_NETWORK == aSuccessCode )
                {
                // Registration status is ENotRegisteredAndNotAvailable
                registrationStatus =
                    RPacketService::ENotRegisteredAndNotAvailable;
                }
            else
                {
                // Registration status is ENotRegisteredButAvailable
                registrationStatus =
                    RPacketService::ENotRegisteredButAvailable;
                }
            break;
            }
        case NET_REG_STATUS_NOSERV_SEARCHING:
            {
            // Registration status is ENotRegisteredSearching
            registrationStatus = RPacketService::ENotRegisteredSearching;
            break;
            }
        case NET_REG_STATUS_NOSERV_NOTSEARCHING:
            {
            // Registration status is ENotRegisteredNotSearching
            registrationStatus = RPacketService::ENotRegisteredNotSearching;
            break;
            }
        case NET_REG_STATUS_NOSERV_NOSIM:
        case NET_REG_STATUS_NOSERV_SIM_REJECTED_BY_NW:
            {
            // Registration status is ERegistrationDenied
            registrationStatus = RPacketService::ERegistrationDenied;
            break;
            }
        case NET_REG_STATUS_POWER_OFF:
        case NET_REG_STATUS_NSPS:
            {
            // Registration status is ENotRegisteredAndNotAvailable
            registrationStatus = RPacketService::ENotRegisteredAndNotAvailable;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketServiceMessHandler::MapNtwkRegStatus: Default, DO NOTHING\n" );
OstTrace0( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_MAPNTWKREGSTATUS_TD, "CMmPacketServiceMessHandler::MapNtwkRegStatus, Default, DO NOTHING" );
            // Do nothing
            break;
            }
        }

    TFLOGSTRING3("TSY: CMmPacketServiceMessHandler::MapNtwkRegStatus.\n  \t Registration Status Server:%d\n\t Registration Status Client:%d\n", aRegistrationStatus, registrationStatus );
OstTraceExt2( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_MAPNTWKREGSTATUS_TD, "CMmPacketServiceMessHandler::MapNtwkRegStatus;aRegistrationStatus=%hhu;registrationStatus=%d", aRegistrationStatus, registrationStatus );

    return registrationStatus;
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::CompleteGsmRegistrationInfo
// Completes GSM Registration Info from GPDS Server.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::CompleteGsmRegistrationInfo(
    const TBool aCsServices,
    const TBool aGprsSupportInCell,
    const TBool aEdgeGprsSupport )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::CompleteGsmRegistrationInfo.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_COMPLETEGSMREGISTRATIONINFO_TD, "CMmPacketServiceMessHandler::CompleteGsmRegistrationInfo" );

    CMmDataPackage data;
    // save aCsServices internally
    iLastCsServices = aCsServices;
    // save aGprsSupportInCell internally
    iLastGprsSupportInCell = aGprsSupportInCell;

    // check that value is changed
    if ( iLastEdgeGprsSupport != aEdgeGprsSupport )
        {
        iLastEdgeGprsSupport = aEdgeGprsSupport;

        // Pack data for completion
        data.PackData( &iLastEdgeGprsSupport );

        // EMmTsyNotifyEGprsInfoChangeIPC is internal IPC
        iMessageRouter->Complete(
            EMmTsyNotifyEGprsInfoChangeIPC,
            &data,
            KErrNone );
        }
    //no else

    // call CompleteDynamicCapsChange()
    CompleteDynamicCapsChange();
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps
// Completes EPacketNotifyTransferCapsIPC.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps(
    const TBool aEdgeGprsSupport )
    {
    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps.\n" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_COMPLETEPACKETNOTIFYTRANSFERCAPS_TD, "CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps" );

    TDynamicTransferCapsFlags transferCaps ( 0 );

    if ( aEdgeGprsSupport )
        {
        transferCaps |= KCapsEGPRS;

        TFLOGSTRING2( "TSY: CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps, Transfer Caps = %d.", transferCaps );
OstTrace1( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_COMPLETEPACKETNOTIFYTRANSFERCAPS_TD, "CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps;transferCaps=%d", transferCaps );
        }

    if ( iLastHsdpaAllocationStatus )
        {
        transferCaps |= KCapsHSDPA;

        TFLOGSTRING2( "TSY: CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps, Transfer Caps = %d.", transferCaps );
OstTrace1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_COMPLETEPACKETNOTIFYTRANSFERCAPS_TD, "CMmPacketServiceMessHandler::CompletePacketNotifyTransferCaps;transferCaps=%d", transferCaps );
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &transferCaps );

    // Complete transfer caps
    iMessageRouter->Complete (
        EPacketNotifyTransferCapsIPC,
        &dataPackage,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::CompleteDynamicCapsChange
// Completes dynamic capabilities change through Message manager
// Packs changed parameters
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::CompleteDynamicCapsChange()
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::CompleteDynamicCapsChange.");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_COMPLETEDYNAMICCAPSCHANGE_TD, "CMmPacketServiceMessHandler::CompleteDynamicCapsChange" );

    CMmDataPackage data;
    data.PackData( &iLastGprsSupportInCell, &iLastCsServices );

    iMessageRouter->Complete (
        EPacketNotifyDynamicCapsChange,
        &data,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::SetAttachMode
// Sets attach mode to the GPDS server.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::SetAttachMode(
    RPacketService::TAttachMode* const aMode )
    {

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::SetAttachMode.\n\t Attach Mode:%d\n", *aMode );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_SETATTACHMODE_TD, "CMmPacketServiceMessHandler::SetAttachMode;aMode=%hhu", *aMode );

    TInt ret( KErrNone );
    TUint8 attachMode( GPDS_ATTACH_MODE_DEFAULT );

    if ( RPacketService::EAttachWhenPossible == *aMode )
        {
        attachMode = GPDS_ATTACH_MODE_AUTOMATIC;
        }
    else if ( RPacketService::EAttachWhenNeeded == *aMode )
        {
        attachMode = GPDS_ATTACH_MODE_MANUAL;
        }
    else
        {
        ret = KErrArgument;
        }

    if ( KErrNone == ret )
        {
        // GPDS_CONFIGURE_REQ is for identifying calling method
        ret = GpdsConfigureReq(
            GPDS_CONFIGURE_REQ,
            attachMode,
            GPDS_MT_ACT_MODE_DEFAULT,
            GPDS_CLASSC_MODE_DEFAULT,
            iAlwaysOnMode );
        }
    //no else

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::PacketContextList
// Returns poineter to CMmPacketMesshandlerContextList
// ----------------------------------------------------------------------------
//
CMmPacketContextMesshandlerList* CMmPacketServiceMessHandler::PacketContextList()
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::PacketContextList.");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_PACKETCONTEXTLIST_TD, "CMmPacketServiceMessHandler::PacketContextList" );

    return iPacketMesshandlerContextList;
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::GetMessageRouter
// Returns pointer to MessageRouter
// ----------------------------------------------------------------------------
//
CMmMessageRouter* CMmPacketServiceMessHandler::GetMessageRouter()
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::GetMessageRouter");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_GETMESSAGEROUTER_TD, "CMmPacketServiceMessHandler::GetMessageRouter" );

    return iMessageRouter;
    }


// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::SetPreferredBearer
// Sets preferrer bearer to the GPDS server.
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::SetPreferredBearer(
    const CMmDataPackage& aDataPackage )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::SetPreferredBearer");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_SETPREFERREDBEARER_TD, "CMmPacketServiceMessHandler::SetPreferredBearer" );

    RPacketService::TPreferredBearer* abearer = NULL;
    TInt ret( KErrNone );

    aDataPackage.UnPackData( &abearer );

    TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::SetPreferredBearer.\n\t Preferred Bearer:%d\n", &abearer );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_SETPREFERREDBEARER_TD, "CMmPacketServiceMessHandler::SetPreferredBearer;abearer=%hhu", *abearer );

    TUint8 bearer( GPDS_CLASSC_MODE_DEFAULT );

    if ( RPacketService::EBearerCircuitSwitched == *abearer )
        {
        bearer = GPDS_CLASSC_MODE_GSM;
        }
    else if ( RPacketService::EBearerPacketSwitched == *abearer )
        {
        bearer = GPDS_CLASSC_MODE_GPRS;
        }
    else
        {
        ret = KErrArgument;
        }

    if ( KErrNone == ret )
        {
        // 0xDC - KDummyTrIdDC is dummy for indentifying calling method
        ret = GpdsConfigureReq(
            KDummyTrIdDC,
            GPDS_ATTACH_MODE_DEFAULT,
            GPDS_MT_ACT_MODE_ACCEPT,
            bearer,
            iAlwaysOnMode );
        }
    //no else

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::SetDefaultContextParams
// Set default context parameters
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::SetDefaultContextParams(
    const CMmDataPackage& aPackage )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::SetDefaultContextParams");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_SETDEFAULTCONTEXTPARAMS_TD, "CMmPacketServiceMessHandler::SetDefaultContextParams" );

    TInt ret( KErrNotSupported );
    TUint8 dataCompression( GPDS_PDP_HCOMP_DEFAULT );
    TUint8 headerCompression( GPDS_PDP_HCOMP_DEFAULT );
    // temporary access point name
    RPacketContext::TGSNAddress tempAccessPointName;
    // temporary Pdp Type
    RPacketContext::TProtocolType tempPdpType( RPacketContext::EPdpTypeCDPD );

    TPacketDataConfigBase* aconfig = NULL;
    aPackage.UnPackData ( &aconfig );

    // Check that the mode given in aconfig is GPRS
    if ( TPacketDataConfigBase::KConfigGPRS == aconfig->ExtensionId() )
        {

        TFLOGSTRING("TSY: CMmPacketServiceMessHandler::SetDefaultContextParams. ExtensionId: KConfigGPRS");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_SETDEFAULTCONTEXTPARAMS_TD, "CMmPacketServiceMessHandler::SetDefaultContextParams, ExtensionId: KConfigGPRS" );

        RPacketContext::TContextConfigGPRS& configGPRS =
            *reinterpret_cast<RPacketContext::TContextConfigGPRS*>( aconfig );

        // Set data compression
        dataCompression = GPDS_PDP_HCOMP_OFF;

        if ( configGPRS.iPdpCompression &
            RPacketContext::KPdpDataCompression )
            {
            dataCompression = GPDS_PDP_HCOMP_ON;
            }
        //no else

        // Set header compression
        headerCompression = GPDS_PDP_HCOMP_OFF;

        if ( configGPRS.iPdpCompression &
            RPacketContext::KPdpHeaderCompression )
            {
            headerCompression = GPDS_PDP_HCOMP_ON;
            }
        //no else

        tempAccessPointName = configGPRS.iAccessPointName;
        tempPdpType = configGPRS.iPdpType;
        }

    // Check that the mode given in aconfig is R99_R4
    else if ( TPacketDataConfigBase::KConfigRel99Rel4 ==
        aconfig->ExtensionId() )
        {

        TFLOGSTRING("TSY: CMmPacketServiceMessHandler::SetDefaultContextParams. ExtensionId: KConfigRel99Rel4");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_SETDEFAULTCONTEXTPARAMS_TD, "CMmPacketServiceMessHandler::SetDefaultContextParams, ExtensionId: KConfigRel99Rel4" );

        RPacketContext::TContextConfigR99_R4& configR99_R4 =
            *reinterpret_cast<RPacketContext::TContextConfigR99_R4*>(
                aconfig );

        tempAccessPointName = configR99_R4.iAccessPointName;
        tempPdpType = configR99_R4.iPdpType;
        }
    //no else

    TUint8 pdpType( GPDS_PDP_TYPE_DEFAULT );

    // This part is for pdp type mapping
    if ( RPacketContext::EPdpTypeIPv4 == tempPdpType )
        {
        pdpType = GPDS_PDP_TYPE_IPV4;
        }
    else if ( RPacketContext::EPdpTypeIPv6 == tempPdpType )
        {
        pdpType = GPDS_PDP_TYPE_IPV6;
        }
    else if ( RPacketContext::EPdpTypePPP == tempPdpType )
        {
        pdpType = GPDS_PDP_TYPE_PPP;
        }
    else
        {
        pdpType = GPDS_PDP_TYPE_DEFAULT;
        }

    if ( GPDS_PDP_TYPE_DEFAULT != pdpType )
        {
        // Call GpdsContextConfigureReq
        ret = GpdsContextConfigureReq(
            dataCompression,
            headerCompression,
            tempAccessPointName,
            pdpType );
        }
    //no else

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::CompleteTransferStatusIndication
// Completes Transfer Status Indication.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::CompleteTransferStatusIndication(
    const TUint8 aTransferCause )
    {
TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::CompleteTransferStatusIndication.\n  \t Transfer Cause:%d\n", aTransferCause );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_COMPLETETRANSFERSTATUSINDICATION_TD, "CMmPacketServiceMessHandler::CompleteTransferStatusIndication;aTransferCause=%hhu", aTransferCause );

    RPacketService::TStatus packetStatus( RPacketService::EStatusAttached );
    RMmCustomAPI::TRauEventStatus rauindication( RMmCustomAPI::ERauEventActive );

    TInt error( KErrNone );
    TBool resumed( EFalse );
    TBool completeToTsy( ETrue );
    // Pack and complete
    CMmDataPackage data;

    switch ( aTransferCause )
        {
        case GPDS_TRANSFER_CAUSE_RESUMED:
            {
            resumed = ETrue;
            rauindication = RMmCustomAPI::ERauEventResumed;
            data.PackData( &rauindication );
            iMessageRouter->Complete( ECustomNotifyRauEventIPC, &data, error );
TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::CompleteTransferStatusIndicationResumed.\n  \t Rau indication:%d\n", rauindication );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_COMPLETETRANSFERSTATUSINDICATION_TD, "CMmPacketServiceMessHandler::CompleteTransferStatusIndication;rauindication=%hhu", rauindication );
            break;
            }
        case GPDS_TRANSFER_CAUSE_ATTACHED:
            {
            // packetStatus is 'RPacketService::EStatusAttached' by default, do nothing
            break;
            }
        case GPDS_TRANSFER_CAUSE_DETACHED:
            {
            packetStatus = RPacketService::EStatusUnattached;
            break;
            }
        case GPDS_TRANSFER_CAUSE_SUSPENDED_NO_COVERAGE:
        case GPDS_TRANSFER_CAUSE_SUSPENDED_CALL_SMS:
        case GPDS_TRANSFER_CAUSE_SUSPENDED_CALL:
            {
            packetStatus = RPacketService::EStatusSuspended;
            break;
            }
        case GPDS_TRANSFER_CAUSE_SUSPENDED_RAU :
            {
TFLOGSTRING2("TSY: CMmPacketServiceMessHandler::CompleteTransferStatusIndicationActivated.\n  \t Rau indication:%d\n", rauindication );
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_COMPLETETRANSFERSTATUSINDICATION_TD, "CMmPacketServiceMessHandler::CompleteTransferStatusIndication;rauindication=%hhu", rauindication );
            // rauindication is 'RMmCustomAPI::ERauEventActive' by default, no need to set here
            data.PackData( &rauindication );
            iMessageRouter->Complete( ECustomNotifyRauEventIPC, &data, error );
            completeToTsy = EFalse;
            break;
            }
        case GPDS_TRANSFER_CAUSE_SUSPENDED_LU :
            {
            completeToTsy = EFalse;
            break;
            }
#if ( NCP_COMMON_CELLMO_BRANCH_SUPPORT == NCP_COMMON_CELLMO_BRANCH_0711 )
        case GPDS_TRANSFER_CAUSE_DSAC_RESTRICTION:
            {
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::CompleteTransferStatusIndication. DSAC: connection failed because of barred PS \n");
OstTrace0( TRACE_NORMAL,  DUP4_CMMPACKETSERVICEMESSHANDLER_COMPLETETRANSFERSTATUSINDICATION_TD, "CMmPacketServiceMessHandler::CompleteTransferStatusIndication, DSAC: connection failed because of barred PS \n" );
            // Core error and extended error are completed to CTSY
            error = ( KErrGeneral & 0x0000FFFF ) |
                ( KErrGsmPSConnectionBarred << 16 );
            break;
            }
#endif // NCP_COMMON_CELLMO_BRANCH_SUPPORT == NCP_COMMON_CELLMO_BRANCH_0711
       default:
            {

TFLOGSTRING("TSY: CMmPacketServiceMessHandler::CompleteTransferStatusIndication.Default case: DO NOTHING\n");
OstTrace0( TRACE_NORMAL,  DUP3_CMMPACKETSERVICEMESSHANDLER_COMPLETETRANSFERSTATUSINDICATION_TD, "CMmPacketServiceMessHandler::CompleteTransferStatusIndication, Default case: DO NOTHING" );
            break;
            }
        }

    if ( completeToTsy )
        {
        //Pack and complete
        CMmDataPackage data;
        data.PackData( &packetStatus, &resumed );
        iMessageRouter->Complete(
            EPacketNotifyStatusChange, &data, error );

        CompleteDynamicCapsChange();
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::CompleteNotifyMSClassChange
// Completes MS Class change notification to the Service Tsy.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::CompleteNotifyMSClassChange(
    const TUint8 aGprsNetworkMode )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::CompleteNotifyMSClassChange");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_COMPLETENOTIFYMSCLASSCHANGE_TD, "CMmPacketServiceMessHandler::CompleteNotifyMSClassChange" );

    CMmDataPackage data;
    RPacketService::TMSClass msClass( RPacketService::EMSClassUnknown );

    // If Gprs supported in current cell
    if ( iLastGprsSupportInCell )
        {
         if ( RMobilePhone::ENetworkModeWcdma == iCurrentMode ||
            iLastDtmSupportInCell )
            {
            // MS Class is Class A
            msClass = RPacketService::EMSClassDualMode;
            }
        else if ( NET_GPRS_MODE_II == aGprsNetworkMode ||
            NET_GPRS_MODE_I == aGprsNetworkMode )
            {
            // MS Class is Class B
            msClass = RPacketService::EMSClassSuspensionRequired;
            }
        else if ( NET_GPRS_MODE_III == aGprsNetworkMode )
            {
            if ( iLastCsServices )
                {
                // MS Class is Class C where cs and gprs both are supported
                msClass = RPacketService::EMSClassAlternateMode;
                }
            else
                {
                // MS Class is Class C where only gprs is supported
                msClass = RPacketService::EMSClassPacketSwitchedOnly;
                }
            }
        //no else
        }
    else if ( iLastCsServices )
        {
        // MS Class is Class C where only cs is supported
        msClass = RPacketService::EMSClassCircuitSwitchedOnly;
        }
    //no else

    // Pack and complete
    data.PackData( &msClass );
    iMessageRouter->Complete(
        EPacketNotifyMSClassChange,
        &data,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::CompleteGpdsStatusRequest
// Completes Gpds Status Request.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::CompleteGpdsStatusRequest(
    const TUint8 aAttachStatus,
    const TUint8 aTransferStatus )
    {

    TFLOGSTRING3("TSY: CMmPacketService::CompleteGpdsStatusRequest.\n\t Attach Status:%d\n\t Transfer Status:%d\n", aAttachStatus, aTransferStatus );
OstTraceExt1( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_COMPLETEGPDSSTATUSREQUEST_TD, "CMmPacketServiceMessHandler::CompleteGpdsStatusRequest;aAttachStatus=%hhu", aAttachStatus );

    // Initiate EStatusAttached to packet status
    RPacketService::TStatus packetStatus( RPacketService::EStatusAttached );

    if ( GPDS_DETACHED == aAttachStatus )
        {
        // If attach status is detached, packet status is unattached
        packetStatus = RPacketService::EStatusUnattached;
        }
    else if ( GPDS_TRANSFER_NOT_AVAIL == aTransferStatus )
        {
        // If attach status is attached and transfer status is not available,
        // packet status is suspended
        packetStatus = RPacketService::EStatusSuspended;
        }

    TBool resumed( EFalse );
    CMmDataPackage data;
    data.PackData( &packetStatus, &resumed  );

    // Complete
    iMessageRouter->Complete(
        EPacketNotifyStatusChange,
        &data,
        KErrNone );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler:NetRatReq
// Current Network Radio Access Technology Request
// ----------------------------------------------------------------------------
//
TInt CMmPacketServiceMessHandler::NetRatReq()
    {
TFLOGSTRING("TSY: CMmPacketServiceMessHandler::NetRatReq");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_NETRATREQ_TD, "CMmPacketServiceMessHandler::NetRatReq" );

    // Set the message data that consists of one parameters
    TBuf8<KBuffSizeOneByte> messageData;
    // Ask information of current network radio access technology
    messageData.Append( NET_CURRENT_RAT );

    // Send Isi message via Phonet
    return( iPhoNetSender->Send(
        PN_MODEM_NETWORK,
        KDummyTrIdDC,
        NET_RAT_REQ,
        messageData ) );
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::NetRatIndOrResp
// Breaks NET_RAT_IND and NET_RAT_RESP ISI-messages.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::NetRatIndOrResp(
    const TIsiReceiveC &aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::NetRatIndOrResp");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_NETRATINDORRESP_TD, "CMmPacketServiceMessHandler::NetRatIndOrResp" );

    TBool readSubBlock( ETrue );
    TBool isNetRatResp( NET_RAT_RESP == aIsiMessage.
        Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    // If received message is response then transaction id must be dummy 0xDC
    // and success code must ok.
    if ( isNetRatResp )
        {
        // 0xDC - KDummyTrIdDC is dummy transaction id
        if ( NET_CAUSE_INVALID_PARAMETER == aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        NET_RAT_RESP_OFFSET_SUCCESSCODE ) || KDummyTrIdDC != aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) )
            {
            readSubBlock = EFalse;
            }
        //no else
        }
    //no else

    if ( readSubBlock )
        {
        TUint sbStartOffSet( 0 );
        TInt ret( 0 );

        // Get network registration info common sub block
        if ( isNetRatResp )
            {
            ret = aIsiMessage.FindSubBlockOffsetById( ISI_HEADER_SIZE +
                SIZE_NET_RAT_RESP ,NET_RAT_INFO, EIsiSubBlockTypeId8Len8, sbStartOffSet );
            }
        else
            {
            ret = aIsiMessage.FindSubBlockOffsetById( ISI_HEADER_SIZE +
                SIZE_NET_RAT_IND ,NET_RAT_INFO, EIsiSubBlockTypeId8Len8, sbStartOffSet );
            }

        if ( KErrNone == ret )
            {
            // Get Rat Name
            TUint8 ratName( aIsiMessage.Get8bit( sbStartOffSet +
                NET_RAT_INFO_OFFSET_RATNAME ) );

            RMobilePhone::TMobilePhoneNetworkMode ntwkMode(
                RMobilePhone::ENetworkModeUnknown );

            // If Gsm Rat is supported
            if ( NET_GSM_RAT == ratName )
                {
                // GSM currently not supported.
                TFLOGSTRING("TSY: CMmPacketServiceMessHandler::NetRatIndOrResp. RMobilePhone::ENetworkModeGsm");

                ntwkMode = RMobilePhone::ENetworkModeGsm;
OstTrace0( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_NETRATINDORRESP_TD, "CMmPacketServiceMessHandler::NetRatIndOrResp, RMobilePhone::ENetworkModeGsm" );
                }
            // Else if Wcdma Rat is supported
            else if ( NET_UMTS_RAT == ratName )
                {
                // WCDMA currently not supported.
                TFLOGSTRING("TSY: CMmPacketServiceMessHandler::NetRatIndOrResp. RMobilePhone::ENetworkModeWcdma");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_NETRATINDORRESP_TD, "CMmPacketServiceMessHandler::NetRatIndOrResp, RMobilePhone::ENetworkModeWcdma" );
                ntwkMode = RMobilePhone::ENetworkModeWcdma;
                }

            // Store networkmode internally
            iCurrentMode = ntwkMode;

            CMmDataPackage data;
            data.PackData( &ntwkMode );

            // Complete EPacketNotifyReleaseModeChange IPC is used for this request
            iMessageRouter->Complete(
                EPacketNotifyReleaseModeChange,
                &data,
                KErrNone );
            }
        //no else
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::HandleError
// Handles CMmPacketServiceMessHandler's errors
// that comes via PhoNetReceiver RunError method.
// ----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::HandleError(
    const TIsiReceiveC& /*aIsiMessage*/,
    TInt /*aError*/ )
    {

    TFLOGSTRING("TSY: CMmPacketServiceMessHandler::HandleError DO NOTHING");
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_HANDLEERROR_TD, "CMmPacketServiceMessHandler::HandleError DO NOTHING" );
    //none
    }

// -----------------------------------------------------------------------------
// CMmPacketServiceMessHandler::NetRadioInfoInd
// Breaks a NET_RADIO_INFO_IND ISI message. Check the HSDPA allocated status
// and completes with EPacketNotifyTransferCapsIPC.
// -----------------------------------------------------------------------------
//
void CMmPacketServiceMessHandler::NetRadioInfoInd(
    const TIsiReceiveC& aIsiMessage )  // received ISI message
    {

    TFLOGSTRING( "TSY: CMmPacketServiceMessHandler::NetRadioInfoInd" );
OstTrace0( TRACE_NORMAL,  CMMPACKETSERVICEMESSHANDLER_NETRADIOINFOIND_TD, "CMmPacketServiceMessHandler::NetRadioInfoInd" );

    TDynamicTransferCapsFlags transferCaps ( 0 );
    TBool hsdpaAllocated ( EFalse );

    // Find NET_UTRAN_RADIO_INFO sub block
    TUint sbStartOffSet = 0;
    TInt retValue = aIsiMessage.FindSubBlockOffsetById( ISI_HEADER_SIZE +
        SIZE_NET_RADIO_INFO_IND, NET_UTRAN_RADIO_INFO,
        EIsiSubBlockTypeId8Len8, sbStartOffSet );

    if ( KErrNone == retValue )
        {
        // Get HSDPA allocation status
        hsdpaAllocated = aIsiMessage.Get8bit( sbStartOffSet +
            NET_UTRAN_RADIO_INFO_OFFSET_HSDPAALLOCATED );

        // Save HSDPA allocation status
        iLastHsdpaAllocationStatus = hsdpaAllocated;

        TFLOGSTRING2( "TSY: CMmPacketServiceMessHandler::NetRadioInfoInd, HSDPA Allocated = %d.", hsdpaAllocated );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPACKETSERVICEMESSHANDLER_NETRADIOINFOIND_TD, "CMmPacketServiceMessHandler::NetRadioInfoInd;hsdpaAllocated=%hhu", hsdpaAllocated );
        }

    // Dynamic transfer caps
    if ( hsdpaAllocated )
        {
        transferCaps |= KCapsHSDPA;

        TFLOGSTRING2( "TSY: CMmPacketServiceMessHandler::NetRadioInfoInd, Transfer Caps = %d.", transferCaps );
OstTrace1( TRACE_NORMAL,  DUP3_CMMPACKETSERVICEMESSHANDLER_NETRADIOINFOIND_TD, "CMmPacketServiceMessHandler::NetRadioInfoInd;transferCaps=%d", transferCaps );
        }

// This is done for SUB 403-7200: NokiaTSY: HSUPA Cell Indicator for S60 (S60 3.2.2 / S_CPR8.1).
// This can be removed when actual implementation for HSUPA Cell Indicator is done.
// CASW_HSXPA_INDICATOR_SUPPORTED flag.
#if defined ( CASW_HSXPA_INDICATOR_SUPPORTED ) || defined ( INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT )

    TBool hsupaAllocated ( EFalse );

    if ( KErrNone == retValue )
        {
        // Get HSUPA allocation status
        hsupaAllocated = aIsiMessage.Get8bit( sbStartOffSet +
            NET_UTRAN_RADIO_INFO_OFFSET_HSUPAALLOCATED );

        TFLOGSTRING2( "TSY: CMmPacketServiceMessHandler::NetRadioInfoInd, HSUPA Allocated = %d.", hsupaAllocated );
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMPACKETSERVICEMESSHANDLER_NETRADIOINFOIND_TD, "CMmPacketServiceMessHandler::NetRadioInfoInd;hsupaAllocated=%hhu", hsupaAllocated );
        }

    if ( hsdpaAllocated || hsupaAllocated )
        {
        transferCaps |= KCapsHSDPA;

        // Save HSDPA allocation status
        iLastHsdpaAllocationStatus = ETrue;

        TFLOGSTRING2( "TSY: CMmPacketServiceMessHandler::NetRadioInfoInd, Transfer Caps (HSDPA or HSUPA) = %d.", transferCaps );
OstTrace1( TRACE_NORMAL,  DUP4_CMMPACKETSERVICEMESSHANDLER_NETRADIOINFOIND_TD, "CMmPacketServiceMessHandler::NetRadioInfoInd;transferCaps=%d", transferCaps );

        }
#endif // defined ( CASW_HSXPA_INDICATOR_SUPPORTED ) || defined ( INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT )

    if ( iLastEdgeGprsSupport )
        {
        transferCaps |= KCapsEGPRS;

        TFLOGSTRING2( "TSY: CMmPacketServiceMessHandler::NetRadioInfoInd, Transfer Caps = %d.", transferCaps );
OstTrace1( TRACE_NORMAL,  DUP5_CMMPACKETSERVICEMESSHANDLER_NETRADIOINFOIND_TD, "CMmPacketServiceMessHandler::NetRadioInfoInd;transferCaps=%d", transferCaps );
        }

    // Pack the data
    CMmDataPackage dataPackage;
    dataPackage.PackData( &transferCaps );
    // Complete transfer caps
    iMessageRouter->Complete (
        EPacketNotifyTransferCapsIPC,
        &dataPackage,
        KErrNone );
    }

// End of File

