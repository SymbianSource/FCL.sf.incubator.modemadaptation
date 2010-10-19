/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES
#include "mmmmesshandlerbase.h"
#include "cmmmessagerouter.h"
#include "cmmphonetsender.h"
#include "cmmphonetreceiver.h"
#include "cmmcallmesshandler.h"
#include "cmmconferencecallmesshandler.h"
#include "cmmdtmfmesshandler.h"
#include "cmmnetmesshandler.h"
#include "cmmnetoperatornamehandler.h"
#include "cmmphonemesshandler.h"
#include "cmmsupplservmesshandler.h"
#include "cmmphonebookstoremesshandler.h"
#include "cmmenstoremesshandler.h"
#include "cmmpacketservicemesshandler.h"
#include "cmmpacketcontextmesshandler.h"
#include "cmmpacketqosmesshandler.h"
#include "cmmussdmesshandler.h"
#include "cmmbroadmesshandler.h"
#include "cmmcustommesshandler.h"
#include "cmmsmsmesshandler.h"
#include "cmmsecuritymesshandler.h"
#include "cmmuiccmesshandler.h"
#include "nokiatsy_internal_variation.h"
#include "nokiatsy_release.h" // for print release information to TSY logs
#include "tsylogger.h"

#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <pcktcs.h> // for packet data IPCs
#include <satcs.h> // for SIM ATK call control
#include <pn_const.h>
#include <pipe_sharedisi.h>
#include <ctsy/rmmcustomapi.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmmessagerouterTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// ==================== LOCAL FUNCTIONS =====================================
    //None

// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CMmMessageRouter::NewL
// Creates new CMmMessageRouter object and returns it.
// -----------------------------------------------------------------------------
//
EXPORT_C CMmMessageRouter* CMmMessageRouter::NewL
        (
        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        CMmMessageManager* aMessageManager
        #else
        MmMessageManagerCallback* aMessageManager
        #endif // NCP_COMMON_S60_VERSION_SUPPORT
        )
    {
TFLOGSTRING("TSY: CMmMessageRouter::NewL");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_NEWL_TD, "CMmMessageRouter::NewL" );

    CMmMessageRouter* messageRouter = NULL;

    messageRouter = new ( ELeave ) CMmMessageRouter();
    CleanupStack::PushL( messageRouter );
    messageRouter->ConstructL( aMessageManager );
    CleanupStack::Pop( messageRouter );

    return messageRouter;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::~CMmMessageRouter
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmMessageRouter::~CMmMessageRouter()
    {
TFLOGSTRING("TSY: CMmMessageRouter::~CMmMessageRouter");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_CMMMESSAGEROUTER_TD, "CMmMessageRouter::~CMmMessageRouter" );

    delete iMmPhoneMessHandler;
    delete iMmCallMessHandler;
    delete iMmConferenceCallMessHandler;
    delete iMmDtmfMessHandler;
    delete iMmNetMessHandler;
    delete iMmSecurityMessHandler;
    delete iMmPhoneBookStoreMessHandler;
    delete iMmPacketContextMessHandler;
    delete iMmPacketQoSMessHandler;
    delete iMmPacketServiceMessHandler;
    delete iMmSmsMessHandler;
    delete iMmSupplServMessHandler;
    delete iMmUssdMessHandler;
    delete iMmCustomMessHandler;
    delete iMmBroadMessHandler;
    delete iMmUiccMessHandler;

    //delete Phonet components
    delete iPhoNetSender;
    delete iPhoNetReceiver;

    iPhoNet.Close();
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::CMmMessageRouter
// Constructor
// -----------------------------------------------------------------------------
//
CMmMessageRouter::CMmMessageRouter()
    {
OstTrace0( TRACE_NORMAL,  DUP1_CMMMESSAGEROUTER_CMMMESSAGEROUTER_TD, "CMmMessageRouter::CMmMessageRouter" );
    // none
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::ConstructL
// Creates and initialises class attributes
// -----------------------------------------------------------------------------
//
#if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
void CMmMessageRouter::ConstructL
        (
        CMmMessageManager* aMessageManager
        )
#else
void CMmMessageRouter::ConstructL
        (
        MmMessageManagerCallback* aMessageManager
        )
#endif // NCP_COMMON_S60_VERSION_SUPPORT
    {
    //PRINT VERSION STRING TO LOG
TFLOGSTRING(NOKIATSY_RELEASE_STR);
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_CONSTRUCTL_TD, NOKIATSY_RELEASE_STR );

    //INIT PHONET
    /**************************************************************/

    // Creation of PhoNet objects.

TFLOGSTRING("TSY: CMmMessageRouter::ConstructL -- initializing ISC API");
OstTrace0( TRACE_NORMAL,  DUP1_CMMMESSAGEROUTER_CONSTRUCTL_TD, "CMmMessageRouter::ConstructL, initializing ISC API" );

    // Only for server information, which is defined with one byte.
    TBuf8<1> resource;
    // resource.Append( PN_ATK );
    TRequestStatus status( KRequestPending );
    // open returns void
    iPhoNet.Open( EIscNokiaMmTsy, status, EOwnerThread, &resource );
    User::WaitForRequest( status );

TFLOGSTRING2("TSY: CMmMessageRouter::ConstructL - status: %d",  status.Int() );
OstTrace1( TRACE_NORMAL, DUP2_CMMMESSAGEROUTER_CONSTRUCTL, "CMmMessageRouter::ConstructL;status=%d", status.Int());

    User::LeaveIfError( status.Int() );

    iMessageManager = aMessageManager;

    iPhoNetSender = CMmPhoNetSender::NewL( &iPhoNet );
    iPhoNetReceiver = CMmPhoNetReceiver::NewL( &iPhoNet );
    //start receiving
    iPhoNetReceiver->ReceiveL();


TFLOGSTRING("TSY: CMmMessageRouter::ConstructL -- initializing done");
OstTrace0( TRACE_NORMAL,  DUP3_CMMMESSAGEROUTER_CONSTRUCTL_TD, "CMmMessageRouter::ConstructL, initializing done" );

    //END INIT PHONET
    /**************************************************************/

    iMmUiccMessHandler = CMmUiccMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this );

    iMmDtmfMessHandler = CMmDtmfMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this );

    iMmCallMessHandler = CMmCallMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        iMmDtmfMessHandler,
        this );

    iMmConferenceCallMessHandler = CMmConferenceCallMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this );

    iMmNetMessHandler = CMmNetMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this,
        iMmUiccMessHandler );

    iMmSupplServMessHandler = CMmSupplServMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this,
        iMmUiccMessHandler );

    iMmSecurityMessHandler = CMmSecurityMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        iMmNetMessHandler,
        iMmCallMessHandler,
        this,
        iMmUiccMessHandler );

    iMmPhoneMessHandler = CMmPhoneMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this,
        iMmSupplServMessHandler,
        iMmUiccMessHandler );

    iMmSmsMessHandler = CMmSmsMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this,
        iMmUiccMessHandler );

    iMmBroadMessHandler = CMmBroadMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this );

    iMmUssdMessHandler = CMmUssdMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this );

    iMmCustomMessHandler = CMmCustomMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        iMmNetMessHandler,
        iMmSecurityMessHandler,
        this,
        iMmUiccMessHandler );

    iMmPhoneBookStoreMessHandler = CMmPhoneBookStoreMessHandler::NewL(
        this,
        iMmUiccMessHandler);

    iMmENStoreMessHandler = CMmENStoreMessHandler::NewL(
        this,
        iMmUiccMessHandler);

    iMmPacketServiceMessHandler = CMmPacketServiceMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        this );

    iMmPacketQoSMessHandler = CMmPacketQoSMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        iMmPacketServiceMessHandler->PacketContextList(),
        this );

    iMmPacketContextMessHandler = CMmPacketContextMessHandler::NewL(
        iPhoNetSender,
        iPhoNetReceiver,
        iMmPacketServiceMessHandler->PacketContextList(),
        iMmPacketQoSMessHandler,
        this );
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::Complete
// Complete without a data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouter::Complete(
    TInt aIpc,
    TInt aResult )
    {
TFLOGSTRING3("NTSY: CMmMessageRouter::Complete. without data package. IPC = %d, result: %d", aIpc, aResult);
OstTraceExt2( TRACE_NORMAL,  CMMMESSAGEROUTER_COMPLETE_TD, "CMmMessageRouter::Complete;aIpc=%d;aResult=%d", aIpc, aResult );
    CMmDataPackage emptyDataPackage;
    Complete( aIpc, &emptyDataPackage, aResult );
    return;
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::Complete
// Complete with a data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouter::Complete(
    TInt aIpc,
    CMmDataPackage* data,
    TInt aResult )
    {
TFLOGSTRING3("NTSY: CMmMessageRouter::Complete. IPC = %d, result: %d", aIpc, aResult);
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMMESSAGEROUTER_COMPLETE_TD, "CMmMessageRouter::Complete;aIpc=%d;aResult=%d", aIpc, aResult );

    // Read Operator Name String (ONS Name) when SIM is ready.
    // Read Operator PLMN List (OPL) rules when SIM is ready.
    // SIM SMS cache: Start cacheing when SIM is ready.
    if ( EMmTsyBootNotifySimStatusReadyIPC == aIpc && KErrNone == aResult )
        {
        // Start ONS and OPL reading.
        iMmNetMessHandler->InitializeOnsAndOplReading();
        // Start SMS cacheing.
        iMmSmsMessHandler->InitializeSmsCache();
        }

    // complete request to upper layers
    #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
    RouteCompletion( aIpc, data, aResult );
    #else
    iMessageManager->Complete( aIpc, data, aResult );
    #endif // NCP_COMMON_S60_VERSION_SUPPORT

    return;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::RouteRequest
// Returns pointer to the message handler for this request, based on IPC number
// -----------------------------------------------------------------------------
//
MMmMessHandlerBase* CMmMessageRouter::RouteRequest
        (
        const TInt aIpc // IPC number
        )
    {
TFLOGSTRING2("TSY: CMmMessageRouter::RouteRequest. IPC:%d",aIpc);
OstTrace1( TRACE_NORMAL,  CMMMESSAGEROUTER_ROUTEREQUEST_TD, "CMmMessageRouter::RouteRequest;aIpc=%d", aIpc );

    MMmMessHandlerBase* messHandler = NULL;

        switch( aIpc )
            {
            case EMobileCallDialEmergencyCall:
            case EEtelCallAnswer:
            case EEtelCallDial:
            case EEtelCallHangUp:
            case EMobileCallHold:
            case EMobileCallResume:
            case EMobileCallSwap:
            case EMobileCallGoOneToOne:
            case EMobileCallTransfer:
            case EMobileCallDeflect:
            case EMobileCallDialISV:
            case EMobileCallAnswerISV:
            case EMobileCallActivateUUS:
            case EMobileCallDialNoFdnCheck:
            //CSD features
            case EEtelCallLoanDataPort:
            case EEtelCallRecoverDataPort:
            // for call control purposes
            case ESatNotifyCallControlRequest:
            case ESatNotifySetUpCallPCmd:
                {
                messHandler = iMmCallMessHandler;
                break;
                }
            case EMobilePhoneSetNetworkSelectionSetting:
            case EMobilePhoneSelectNetwork:
            case EMobilePhoneGetDetectedNetworksV2Phase1:
            case EMobilePhoneSelectNetworkCancel:
            case EMobilePhoneGetDetectedNetworksCancel:
            case EMobilePhoneGetNetworkRegistrationStatus:
            case EMobilePhoneGetHomeNetwork:
            case EMobilePhoneGetNetworkSecurityLevel:
            case EMobilePhoneGetCipheringIndicatorStatus:
            case EMobilePhoneAuthorizationInfoPhase1:
                {
                messHandler = iMmNetMessHandler;
                break;
                }
            // DTMF functionality
            case EMobilePhoneStartDTMFTone:
            case EMobilePhoneStopDTMFTone:
            case EMobilePhoneSendDTMFTones:
            case EMobilePhoneSendDTMFTonesCancel:
            case EMobilePhoneContinueDTMFStringSending:
                {
                messHandler = iMmDtmfMessHandler;
                break;
                }
            // Conference call functionality
            case EMobileConferenceCallCreateConference:
            case EMobileConferenceCallAddCall:
            case EMobileConferenceCallSwap:
            case EMobileConferenceCallHangUp:
                {
                messHandler = iMmConferenceCallMessHandler;
                break;
                }
            case EMmTsyPhoneBookStoreReadIPC:
            case EMmTsyPhoneBookStoreWriteIPC:
            case EMmTsyPhoneBookStoreGetInfoIPC:
            case EMmTsyPhoneBookStoreDeleteIPC:
            case EMmTsyPhoneBookStoreDeleteAllIPC:
            case EMmTsyPhoneBookStoreCacheIPC:
            case EMmTsyPhoneBookStoreInitIPC:
            case EMmTsyPhoneBookStoreCacheCancelIPC:
            case EMobilePhoneGetMailboxNumbers:
            case EMmTsyONStoreReadIPC:
            case EMmTsyONStoreReadEntryIPC:
            case EMmTsyONStoreGetInfoIPC:
            case EMmTsyONStoreDeleteIPC:
            case EMmTsyONStoreDeleteAllIPC:
            case EMmTsyONStoreReadSizeIPC:
            case EMmTsyONStoreWriteSizeIPC:
            case EMmTsyONStoreWriteEntryIPC:
            case EMmTsyONStoreWriteIPC:
                 {
                messHandler = iMmPhoneBookStoreMessHandler;
                break;
                }
            case EMmTsyENStoreReadAllPhase1IPC:
            case EMmTsyENStoreReadIPC:
            case EMmTsyENStoreGetInfoIPC:
                {
                messHandler = iMmENStoreMessHandler;
                break;
                }
            case EPacketAttach:
            case EPacketDetach:
            case EPacketGetNtwkRegStatus:
            case EPacketSetAttachMode:
            case EPacketRejectActivationRequest:
            case EPacketGetAttachMode:
            case EPacketSetPrefBearer:
            case EPacketSetDefaultContextParams:
            case EPacketNotifyContextAdded:
            case EPacketNotifyDynamicCapsChange:
            case EPacketNotifyStatusChange:
            case ECustomSetAlwaysOnMode:
            case EPacketInitProxiesIPC:
            case EPacketNotifyTransferCapsIPC:
                {
                messHandler = iMmPacketServiceMessHandler;
                break;
                }
            case EPacketContextInitialiseContext:
            case EPacketContextSetConfig:
            case EPacketContextActivate:
            case EPacketContextDelete:
            case EPacketContextGetDataVolumeTransferred:
            case EPacketContextDeactivate:
            case EPacketContextAddPacketFilter:
            case EPacketContextModifyActiveContext:
            case EPacketAddMediaAuthorization:
            case ECustomSetDriveModeIPC:
                {
                messHandler = iMmPacketContextMessHandler;
                break;
                }
            case EPacketQoSSetProfileParams:
            case EPacketQoSNotifyProfileChanged:
            case EPacketQoSNotifyProfileChangedCancel:
                {
                messHandler = iMmPacketQoSMessHandler;
                break;
                }
            case EMobileUssdMessagingSendMessage:
            case EMobileUssdMessagingSendRelease:
            case EMobileUssdMessagingSendMessageNoFdnCheck:
                {
                messHandler = iMmUssdMessHandler;
                break;
                }
            // Supplementary Services functionality
            case EMobilePhoneGetCallForwardingStatusPhase1:
            case EMobilePhoneSetCallForwardingStatus:
            case EMobilePhoneGetBarringStatusPhase1:
            case EMobilePhoneSetCallBarringStatus:
            case EMobilePhoneSetSSPassword:
            case EMobilePhoneGetWaitingStatusPhase1:
            case EMobilePhoneSetCallWaitingStatus:
            case EMobilePhoneNotifyCallWaitingStatusChange:
            case EMobilePhoneGetIdentityServiceStatus:
            case EMobilePhoneSendNetworkServiceRequest:
            case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
            case EMmTsyGetCallForwardingNumberIPC:
            case EMobilePhoneGetIccMessageWaitingIndicators:
            case EMobilePhoneSetIccMessageWaitingIndicators:
            case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
            case ECustomGetIccCallForwardingStatusIPC:
            case ECustomNotifyIccCallForwardingStatusChangeIPC:
            // Temporary until SAT: ETEL Multimode API
            // usage for SAT has been implemented
            case ESatNotifySendSsPCmd:
                {
                messHandler = iMmSupplServMessHandler;
                break;
                }
            // Custom API functionality
            case ECustomCancelUssdSessionIPC:
            case ECustomClearCallBlackListIPC:
            case ECustomCheckEmergencyNumberIPC:
            case ECustomGetLifeTimeIPC:
            case ECustomTerminateCallIPC:
            case ECustomGetAlsBlockedIPC:
            case ECustomSetAlsBlockedIPC:
            case ECustomCheckAlsPpSupportIPC:
            case ECustomCheckTwoDigitDialSupportIPC:
            case ECustomGetCipheringInfoIPC:
            case ECustomNetWakeupIPC:
            case ECustomGetOperatorNameIPC:
            case ECustomResetNetServerIPC:
            case ECustomGetNetworkProviderNameIPC:
            case ECustomReadSimFileIPC:

            // GetProgrammableOperatorLogo is not supported, see Change
            // Request TSW ActionDB MS04052536038
            // case ECustomGetProgrammableOperatorLogoIPC:

            // martpiir 26-May-2004: The cases handled in
            // Custom Sub-messagehandlers are currently routed to Custom
            // GSM message handler, which in turn routes them to the
            // appropriate Sub-messagehandler. This extra-routing has to be
            // removed later!!!
            case ECustomSimLockActivateIPC:
            case ECustomSimLockDeActivateIPC:
            case ECustomSendAPDUReqIPC:
            case ECustomSimWarmResetIPC:
            case ECustomGetATRIPC:
            case ECustomGetSimCardReaderStatusIPC:
            case ECustomSendAPDUReqV2IPC:
            case ECustomNotifySimCardStatusIPC:
            case ECustomPowerSimOnIPC:
            case ECustomPowerSimOffIPC:
            case ECustomStartSimCbTopicBrowsingIPC:
            case ECustomDeleteSimCbTopicIPC:
            case EReadViagHomeZoneParamsIPC:
            case EReadViagHomeZoneCacheIPC:
            case EWriteViagHomeZoneCacheIPC:
            //WCDMA HZ:
            case EWriteViagHomeZoneUHZIUESettingsIPC:
            case ECustomGetSystemNetworkModesIPC:
            case ECustomSetSystemNetworkModeIPC:
            case ECustomGetCurrentSystemNetworkModesIPC:
            case EMobilePhoneIMSAuthenticate:
            case ECustomGetSimAuthenticationDataIPC:
            case ECustomGetCellInfoIPC:
            case ECustomWriteHSxPAStatusIPC:
            case ECustomReadHSxPAStatusIPC:

// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )
            case ECustomSetBandSelectionIPC:
            case ECustomGetBandSelectionIPC:
#endif // NCP_COMMON_S60_VERSION_SUPPORT

            case ECustomGetServiceTableSupportbyApplicationIPC:
            case EMmTsyUpdateLifeTimeIPC:
                {
                messHandler = iMmCustomMessHandler;
                break;
                }

            case EMobileBroadcastMessagingReceiveMessage:
            case EMobileBroadcastMessagingReceiveMessageCancel:
            case EMobileBroadcastMessagingSetFilterSetting:
            case EMobileBroadcastMessagingSetLanguageFilter:
            case EMobileBroadcastMessagingStoreIdList:
                {
                messHandler = iMmBroadMessHandler;
                break;
                }
            //SMS functionality
            case EMmTsyActivateSmsRouting:
            case EMmTsyDeactivateSmsRouting:
            case EMobileSmsMessagingGetMessageStoreInfo:
            case EMobileSmsMessagingReceiveMessage:
            case EMobileSmsMessagingSendMessage:
            case EMobileSmsMessagingResumeSmsReception:
            case EMobileSmsMessagingGetSmspListPhase1:
            case EMobileSmsMessagingStoreSmspList:
            case EMobileSmsMessagingAckSmsStored:
            case EMobileSmsMessagingNackSmsStored:
            case EMobilePhoneStoreGetInfo:
            case EMobilePhoneStoreDelete:
            case EMobilePhoneStoreDeleteAll:
            case EMobilePhoneStoreRead:
            case EMobilePhoneStoreReadAllPhase1:
            case EMobilePhoneStoreWrite:
            case EMmTsySmsSendSatMessage:
            case ECustomSetSimMessageStatusReadIPC:
            case EMobileSmsMessagingSetMoSmsBearer:
            case EMobileSmsMessagingSendMessageNoFdnCheck:
                {
                messHandler = iMmSmsMessHandler;
                break;
                }
            //Security and SIM
            case EMobilePhoneGetLockInfo:
            case EMobilePhoneChangeSecurityCode:
            case EMobilePhoneAbortSecurityCode:
            case EMobilePhoneVerifySecurityCode:
            case EMobilePhoneSetLockSetting:
            case EMmTsySecurityGetSimActivePinStateIPC:
            case EMmTsyBootNotifySimStatusReadyIPC:
            case EMobilePhoneNotifySecurityEvent:
            case EMmTsySimGetICCType:
            case EMobilePhoneGetCurrentActiveUSimApplication:
            case EMobilePhoneSetFdnSetting:
            case EMobilePhoneGetFdnStatus:
            case EMmTsyBootGetRFStatusIPC:
            case EMobilePhoneGetSecurityCodeInfo:
                {
                messHandler = iMmSecurityMessHandler;
                break;
                }
            // Miscellaneous Phone features
            // case EMobilePhoneGetBatteryInfo: Get battery info is handled completely in Commontsy
            case EMobilePhoneGetPhoneStoreInfo:
            case EMobilePhoneGetPhoneId:
            case EMobilePhoneGetSubscriberId:
            case EMobilePhoneGetALSLine:
            case EMobilePhoneSetALSLine:
            case EMobilePhoneGetCustomerServiceProfile:
            case EMobilePhoneGetServiceTable:
            case EMmTsyPhoneGetPin1DisableSupportedIPC:
            case EMobilePhoneGetServiceProviderName:
            case EMmTsySimRefreshRegisterIPC:
            case EMmTsySimRefreshDoneIPC:
            case EMobilePhoneGetAPNControlListServiceStatus:
            case EMobilePhoneSetAPNControlListServiceStatus:
            case EMobilePhoneDeleteAPNName:
            case EMobilePhoneAppendAPNName:
            case EMobilePhoneEnumerateAPNEntries:
            case EMobilePhoneGetAPNname:
                {
                messHandler = iMmPhoneMessHandler;
                break;
                }
            default:
                {
TFLOGSTRING("TSY: CMmMessageRouter::RouteRequest. Switch aIpc case Default");
OstTrace0( TRACE_NORMAL,  DUP1_CMMMESSAGEROUTER_ROUTEREQUEST_TD, "CMmMessageRouter::RouteRequest, Switch aIpc case Default" );
                messHandler = NULL;
                }
            }

    return messHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::ExtFuncL
// Find out the correct message handler and call its ExtFuncL
// -----------------------------------------------------------------------------
//
TInt CMmMessageRouter::ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING2("TSY: CMmMessageRouter::ExtFuncL. IPC = %d", aIpc);
OstTrace1( TRACE_NORMAL,  CMMMESSAGEROUTER_EXTFUNCL_TD, "CMmMessageRouter::ExtFuncL;aIpc=%d", aIpc );

    TInt ret ( KErrNone );

    // Check if the IPC is supported
    MMmMessHandlerBase* messageHandler = RouteRequest( aIpc );
    if ( NULL == messageHandler )
        {
        ret = KErrNotSupported;
        }
    else
        {
        // Call to DOS layer
        ret = messageHandler->ExtFuncL( aIpc, aDataPackage );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetPhoNetSender
// Returns pointer to Phonet sender
// -----------------------------------------------------------------------------
//
EXPORT_C CMmPhoNetSender* CMmMessageRouter::GetPhoNetSender()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetPhoNetSender");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETPHONETSENDER_TD, "CMmMessageRouter::GetPhoNetSender" );
    return iPhoNetSender;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetPhoNetReceiver
// Returns pointer to Phonet receiver
// -----------------------------------------------------------------------------
//
EXPORT_C CMmPhoNetReceiver* CMmMessageRouter::GetPhoNetReceiver()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetPhoNetReceiver");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETPHONETRECEIVER_TD, "CMmMessageRouter::GetPhoNetReceiver" );
    return iPhoNetReceiver;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetCallMessHandler
// Returns pointer to Call message handler
// -----------------------------------------------------------------------------
//
CMmCallMessHandler* CMmMessageRouter::GetCallMessHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetCallMessHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETCALLMESSHANDLER_TD, "CMmMessageRouter::GetCallMessHandler" );
    return iMmCallMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetPhoneMessHandler
// Returns pointer to Phone message handler
// -----------------------------------------------------------------------------
//
CMmPhoneMessHandler* CMmMessageRouter::GetPhoneMessHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetPhoneMessHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETPHONEMESSHANDLER_TD, "CMmMessageRouter::GetPhoneMessHandler" );
    return iMmPhoneMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetPacketServiceMessHandler
// Returns pointer to Packet service message handler
// -----------------------------------------------------------------------------
//
CMmPacketServiceMessHandler* CMmMessageRouter::GetPacketServiceMessHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetPacketServiceMessHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETPACKETSERVICEMESSHANDLER_TD, "CMmMessageRouter::GetPacketServiceMessHandler" );
    return iMmPacketServiceMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetCustomMessHandler
// Returns pointer to Custom message handler
// -----------------------------------------------------------------------------
//
CMmCustomMessHandler* CMmMessageRouter::GetCustomMessHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetCustomMessHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETCUSTOMMESSHANDLER_TD, "CMmMessageRouter::GetCustomMessHandler" );
    return iMmCustomMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetSmsMessHandler
// Returns pointer to SMS message handler
// -----------------------------------------------------------------------------
//
CMmSmsMessHandler* CMmMessageRouter::GetSmsMessHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetSmsMessHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETSMSMESSHANDLER_TD, "CMmMessageRouter::GetSmsMessHandler" );
    return iMmSmsMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetSupplServHandler
// Returns pointer to SS message handler
// -----------------------------------------------------------------------------
//
CMmSupplServMessHandler* CMmMessageRouter::GetSupplServHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetSupplServHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETSUPPLSERVHANDLER_TD, "CMmMessageRouter::GetSupplServHandler" );
    return iMmSupplServMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::PhoneBookStoreMessHandler
// Returns pointer to Phonebookstore message handler
// -----------------------------------------------------------------------------
//
CMmPhoneBookStoreMessHandler* CMmMessageRouter::PhoneBookStoreMessHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::PhoneBookStoreMessHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_PHONEBOOKSTOREMESSHANDLER_TD, "CMmMessageRouter::PhoneBookStoreMessHandler" );
    return iMmPhoneBookStoreMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouter::GetNetMessHandler
// Returns pointer to NET message handler
// -----------------------------------------------------------------------------
//
CMmNetMessHandler* CMmMessageRouter::GetNetMessHandler()
    {
TFLOGSTRING("TSY: CMmMessageRouter::GetNetMessHandler");
OstTrace0( TRACE_NORMAL,  CMMMESSAGEROUTER_GETNETMESSHANDLER_TD, "CMmMessageRouter::GetNetMessHandler" );
    return iMmNetMessHandler;
    }

// ==================== OTHER EXPORTED FUNCTIONS ===============================
    //None

//  End of File
