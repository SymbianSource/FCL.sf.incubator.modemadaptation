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
#include "cmmcallmesshandler.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h"
#include "cmmmessagerouter.h"
#include "tsylogger.h"
#include "cmmdataporthandler.h" // dataport includes
#include "cmmdtmfmesshandler.h" //DTMF includes
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcallmesshandlertraces.h"
#endif

// ISI message coding/decoding functions
#include <call_modemisi.h>
#include <infoisi.h>
#include <gpdsisi.h>
#include <product_profile_definitions.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <csdisi.h>
#include <pipeisi.h>
#include <pipe_sharedisi.h>

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
const TUint8 KCallPadding = 0x00; // Filler byte for ISI messages

// This needs to be distinct from TProductProfileRequestType values
// in cmmcustommesshandler.h
const TUint8 KHsdpaPpTransId = 5;

// Delay for starting the emergency timer, 5 seconds
const TUint KEmergencyTimerInterval = 5000000;

// For shifting by one byte
const TUint8 KShiftByOneByte = 8;

// Preaddress length
const TUint8 KPreAddressLength = 4;

// CBST Data Rates coding
const TUint KRateCodeAuto  = 0;
const TUint KRateCode28800 = 130;
const TUint KRateCode32000 = 131;
const TUint KRateCode33600 = 132;
const TUint KRateCode56000 = 133;
const TUint KRateCode64000 = 134;

//value for information which value is zero
const TUint KZero = 0;

// For call cause no cause
const TUint8 KCallCauseNoCause = 0;

// First PEP
const TUint8 KFirstPepDevice = PN_DEV_HOST;      // Device Id of first PEP
const TUint8 KFirstPepType   = PN_PEP_TYPE_COMM; // First PEP type

// Second PEP
const TUint8 KSecondPepDevice = PN_DEV_MODEM;    // Device Id of second PEP
const TUint8 KSecondPepObject = 54; // PN_OBJ_CSD_SRV // Object Id of second PEP
const TUint8 KSecondPepType   = PN_PEP_TYPE_GPRS; // Second PEP type

// Redirect PEP
const TUint8 KRedirectPepObject = 59; //PN_OBJ_CSD_WTB; // Object Id of redirected PEP

const TUint8 KPipeTransID  = EIscNokiaDataport1;

const TUint8 KInvalidPipeHandle = 0xFF;

// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
    // None

// ============================= LOCAL FUNCTIONS ===============================

// =============================================================================
// CEmergencyTimer: Used for delaying Emergency call when PDP context deactivate
// is ongoing

// -----------------------------------------------------------------------------
// CEmergencyTimer::CEmergencyTimer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CEmergencyTimer::CEmergencyTimer(
     CMmCallMessHandler& aCallMsgHandler )
        :CTimer( CActive::EPriorityHigh ),
        iCallMsgHandler( aCallMsgHandler )
    {
TFLOGSTRING("TSY: CEmergencyTimer::CEmergencyTimer");
OstTrace0( TRACE_NORMAL, CEMERGENCYTIMER_CEMERGENCYTIMER, "CEmergencyTimer::CEmergencyTimer" );
    }

// -----------------------------------------------------------------------------
// CEmergencyTimer::~CEmergencyTimer()
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CEmergencyTimer::~CEmergencyTimer()
    {
TFLOGSTRING("TSY: CEmergencyTimer::~CEmergencyTimer");
OstTrace0( TRACE_NORMAL, DUP1_CEMERGENCYTIMER_CEMERGENCYTIMER, "CEmergencyTimer::~CEmergencyTimer" );
    Cancel();
    }

// -----------------------------------------------------------------------------
// CEmergencyTimer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEmergencyTimer::ConstructL()
    {
TFLOGSTRING("TSY: CEmergencyTimer::ConstructL");
OstTrace0( TRACE_NORMAL, CEMERGENCYTIMER_CONSTRUCTL, "CEmergencyTimer::ConstructL" );
    CTimer::ConstructL(); // Must be called
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CEmergencyTimer::RunL()
// Called when timer elapses.
// -----------------------------------------------------------------------------
//
void CEmergencyTimer::RunL()
    {
TFLOGSTRING("TSY: CEmergencyTimer::RunL - TIMER ELAPSED");
OstTrace0( TRACE_NORMAL, CEMERGENCYTIMER_RUNL, "CEmergencyTimer::RunL" );
    iCallMsgHandler.GpdsContextsClearResp();
    }

// -----------------------------------------------------------------------------
// CEmergencyTimer::StartEmergencyTimer()
// Start Emergency delay timer.
// -----------------------------------------------------------------------------
//
void CEmergencyTimer::StartEmergencyTimer()
    {
TFLOGSTRING("TSY: CEmergencyTimer::StartEmergencyTimer");
OstTrace0( TRACE_NORMAL, CEMERGENCYTIMER_STARTEMERGENCYTIMER, "CEmergencyTimer::StartEmergencyTimer" );
    Cancel();
    After( KEmergencyTimerInterval ); // 5 seconds
    }

// -----------------------------------------------------------------------------
// CEmergencyTimer::StopEmergencyTimer()
// Stop Emergency delay timer.
// -----------------------------------------------------------------------------
//
void CEmergencyTimer::StopEmergencyTimer()
    {
TFLOGSTRING("TSY: CEmergencyTimer::StopEmergencyTimer");
OstTrace0( TRACE_NORMAL, CEMERGENCYTIMER_STOPEMERGENCYTIMER, "CEmergencyTimer::StopEmergencyTimer" );
    Cancel();
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CMmCallMessHandler
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMmCallMessHandler::CMmCallMessHandler() : iEmergencyTimer( *this )
    {
TFLOGSTRING("TSY: CEmergencyTimer::CMmCallMessHandler");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CMMCALLMESSHANDLER, "CMmCallMessHandler::CMmCallMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmCallMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CONSTRUCTL, "CMmCallMessHandler::ConstructL" );

    iSubAddress.Zero();
    iBearer.Zero();
    iReceivedUusData.Zero();
    iWaitingCall = KSymbianCallIdNone;
    iFDNErrorAlredyNotified = EFalse;
    iIs3rdPartyDial = EFalse;
    iIs3rdPartyAnswerIncomingCall = EFalse;
    iDataPortHandler = NULL;
    iGsmMmCauseError = KErrNone;
    iDestPostAddress.SetLength( 0 );
    iDtmfPostAddressCallId = 0;
    iDestPostAddressIncluded = EFalse;
    iUusData.iUUI.Zero();
    iUusData.iServiceReq = 0;
    iDataRateCode = KRateCode64000;
    iCallDirection = RMobileCall::EDirectionUnknown;
    iPipeHandle = KInvalidPipeHandle;

    iEmergCallDelayed = EFalse;
    iHSDPADisabledInPp = EFalse;
    iEmergencyTimer.ConstructL();
    // Read "HSDPA Disabled" status from product profile
    InfoPpDataReadReq();

    iSecondPepDeviceId = KSecondPepDevice;
    iSecondPepObjectId = KSecondPepObject;

    iCallOperationID = CSD_CALL_CREATE;
    iVideoCallMtReleased = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmCallMessHandler* CMmCallMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender,
    CMmPhoNetReceiver* aPhoNetReceiver,
    CMmDtmfMessHandler* aDtmfMessHandler,
    CMmMessageRouter* aMessageRouter )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_NEWL, "CMmCallMessHandler::NewL" );

    CMmCallMessHandler* callMessHandler = new( ELeave ) CMmCallMessHandler();

    CleanupStack::PushL( callMessHandler );
    callMessHandler->iPhoNetSender = aPhoNetSender;
    callMessHandler->iMessageRouter = aMessageRouter;
    callMessHandler->ConstructL();
    aPhoNetReceiver->RegisterL( callMessHandler, PN_MODEM_CALL );
    aPhoNetReceiver->RegisterL(
        callMessHandler,
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        PN_INFO,
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        PN_MODEM_INFO,
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        INFO_PP_DATA_READ_RESP );
    aPhoNetReceiver->RegisterL(
        callMessHandler,
        PN_GPDS,
        GPDS_CONTEXTS_CLEAR_RESP );

    aPhoNetReceiver->RegisterL( callMessHandler, PN_CSD );

    aPhoNetReceiver->RegisterL( callMessHandler, PN_PIPE, PNS_PIPE_CREATE_RESP );
    aPhoNetReceiver->RegisterL( callMessHandler, PN_PIPE, PNS_PIPE_ENABLE_RESP );
    aPhoNetReceiver->RegisterL( callMessHandler, PN_PIPE, PNS_PIPE_RESET_RESP );
    aPhoNetReceiver->RegisterL( callMessHandler, PN_PIPE, PNS_PIPE_REMOVE_RESP );

    callMessHandler->iDtmfMessHandler = aDtmfMessHandler;
    CleanupStack::Pop( callMessHandler );

    return callMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::~CMmCallMessHandler
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmCallMessHandler::~CMmCallMessHandler()
    {
TFLOGSTRING("TSY: CMmCallMessHandler::~CMmCallMessHandler");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CMMCALLMESSHANDLER, "CMmCallMessHandler::~CMmCallMessHandler" );

    if ( iDataPortHandler )
        {
        delete iDataPortHandler;
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::ReceiveMessageL
// Entry point for messages received from Domestic OS. Switches the message
// to the correct method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::ReceiveMessageL(
    const TIsiReceiveC &aIsiMessage // Received ISI message
    )
    {
    TInt resource (aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ));
    TInt messageId(aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ));

TFLOGSTRING3("TSY: CMmCallMessHandler::ReceiveMessageL. Resource:%d. MsgId:%d", resource, messageId);
OstTraceExt2( TRACE_NORMAL, CMMCALLMESSHANDLER_RECEIVEMESSAGEL, "CMmCallMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    // Switch according to the resource
    switch ( resource )
        {
        // Call server messages
        case PN_MODEM_CALL:
           {
            // Switch according to the message id
            // and call the appropriate messagehandler method
            switch( messageId )
                {
                // NOTE: ALL KErrNotFound lines should be removed after the
                // message handlers have been rearranged
                case CALL_MODEM_RELEASE_IND:
                    {
                    CallReleaseInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_TERMINATED_IND:
                    {
                    CallTerminatedInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_STATUS_IND:
                    {
                    CallStatusIndL( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_CONTROL_IND:
                    {
                    CallControlInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_PROPERTY_SET_RESP:
                    {
                    TUint8 transactId( aIsiMessage.Get8bit(
                        ISI_HEADER_OFFSET_TRANSID ) );

TFLOGSTRING2("TSY: CMmCallMessHandler::ReceiveMessageL. CALL_MODEM_PROPERTY_SET_RESP. transactId:%d",transactId);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_RECEIVEMESSAGEL, "CMmCallMessHandler::ReceiveMessageL. CALL_MODEM_PROPERTY_SET_RESP. transactId=%hhu", transactId );

                    if ( KCallTransId == transactId )
                        {
                        CallPropertySetResp( aIsiMessage );
                        }
                    break;
                    }
                case CALL_MODEM_NOTIFICATION_IND:
                    {
                    CallGsmNotificationInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_ANSWER_RESP:
                    {
                    CallAnswerResp( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_CONTROL_RESP:
                    {
                    CallControlResp( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_CREATE_RESP:
                    {
                    CallCreateResp( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_RELEASE_RESP:
                    {
                    CallReleaseResp( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_COMING_IND:
                    {
                    CallComingInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_SERVICE_DENIED_IND:
                    {
                    CallServiceDeniedInd( aIsiMessage );
                    break;
                    }
                default:
                    {
                    // Needed because call server messages are also handled
                    // by DTMF and conference call message handlers
TFLOGSTRING("TSY: CMmCallMessHandler::ReceiveMessageL, switch resource - case PN_MODEM_CALL, switch messageId - default.\n" );
OstTrace0( TRACE_NORMAL, DUP5_CMMCALLMESSHANDLER_RECEIVEMESSAGEL, "CMmCallMessHandler::ReceiveMessageL, switch resource - case PN_MODEM_CALL, switch messageId - default.\n" );
                    break;
                    }
                }
            break;
            }
        // Info server messages
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case PN_INFO:
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        case PN_MODEM_INFO:
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            {
            if ( INFO_PP_DATA_READ_RESP == messageId )
                {
                TUint8 transactId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
                if ( KHsdpaPpTransId == transactId )
                    {
                    InfoPpDataReadResp( aIsiMessage );
                    }
                }
            break;
            }
        // GPDS server messages
        case PN_GPDS:
            {
            if ( GPDS_CONTEXTS_CLEAR_RESP == messageId )
                {
                GpdsContextsClearResp();
                }
            break;
            }
        // CSD server messages
        case PN_CSD:
            {
            // Switch according to the message id
            // and call the appropriate messagehandler method
            switch( messageId )
                {
                case CSD_CALL_CONTROL_RESP:
                    {
                    CsdCallControlResp( aIsiMessage );
                    break;
                    }
                case CSD_VIDEO_CALL_STATUS_IND:
                    {
                    CsdVideoCallStatusInd( aIsiMessage );
                    break;
                    }
                case CSD_MULTIMEDIA_DATA_RATE_IND:
                    {
                    CsdMultimediaDataRateInd( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmCallMessHandler::ReceiveMessageL, switch resource - case PN_CSD, switch messageId - default.\n" );
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_RECEIVEMESSAGEL, "CMmCallMessHandler::ReceiveMessageL, switch resource - case PN_CSD, switch messageId - default." );
                    break;
                    }
                }
            break;
            }
        // ISI messages for Pipe Service
        case PN_PIPE:
            {
            switch( messageId )
                {
                case PNS_PIPE_CREATE_RESP:
                    {
                    PnsPipeCreateResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_ENABLE_RESP:
                    {
                    PnsPipeEnableResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_RESET_RESP:
                    {
                    PnsPipeResetResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_REMOVE_RESP:
                    {
                    PnsPipeRemoveResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_REDIRECT_RESP:
                    {
                    PnsPipeRedirectResp( aIsiMessage );
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmCallMessHandler::ReceiveMessageL, switch resource - case PN_PIPE, switch messageId - default.\n" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_RECEIVEMESSAGEL, "CMmCallMessHandler::ReceiveMessageL, switch resource - case PN_PIPE, switch messageId - default.\n" );
                    break;
                    }
                }
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmCallMessHandler::ReceiveMessageL, switch resource - default.\n" );
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_RECEIVEMESSAGEL, "CMmCallMessHandler::ReceiveMessageL, switch resource - default.\n" );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::ExtFuncL
// Entry point for messages received from Symbian OS layer (Message manager)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::ExtFuncL(
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING2("TSY: CMmCallMessHandler::ExtFuncL - aIpc: %d", aIpc );
OstTrace1( TRACE_NORMAL, CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL;aIpc=%d", aIpc );

    TInt ret( KErrNone );
    TUint8 messageId( 0 );
    TUint8 isiCallId( 0 );
    TInt callId( KSymbianCallIdNone );
    TUint8 dosCallId( CALL_MODEM_ID_NONE );
    RMobilePhone::TMobileService callMode( RMobilePhone::EServiceUnspecified );
    TBool requestDirectedToDataPort( EFalse );
    TBool callCreatedAlready( EFalse );

    const CCallDataPackage* callData(
        reinterpret_cast<const CCallDataPackage*>( aDataPackage ) );

    // In case of emergency call CMmDataPackage is used and it doesn't contain
    // call ID or call mode. CCallDataPackage is used in case of normal call.
    if ( EMobileCallDialEmergencyCall != aIpc )
        {
        callData->GetCallIdAndMode( callId, callMode );
TFLOGSTRING2("TSY: CMmCallMessHandler::ExtFuncL. CallMode:%d", callMode );
OstTrace1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL;callMode=%d", callMode );
        }

    if ( KSymbianCallIdNone == callId )
        {
        dosCallId = CALL_MODEM_ID_NONE;
        }
    else
        {
        dosCallId = static_cast<TUint8>( callId );
        }

    TIsiSend callReq( iPhoNetSender->SendBufferDes() );

    // Resource ID
    callReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_CALL );

    // Transaction ID
    callReq.Set8bit( ISI_HEADER_OFFSET_TRANSID, KCallTransId );

    switch( aIpc )
        {
        case EMobileCallDialEmergencyCall:
            {
            aDataPackage->UnPackData( iTelEmergNumber );
TFLOGSTRING2("TSY: CMmCallMessHandler::ExtFuncL - emergency dial number is %S", &iTelEmergNumber);
OstTraceExt1( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL - emergency dial number is=%S", iTelEmergNumber );
#ifdef NCP_COMMON_HSDPA_EMERGCALL_WORKAROUND
            if ( iHSDPADisabledInPp )
                {
                // HSPDA is disabled in PP; no need to delay the emergency
                // call setup.
#endif //NCP_COMMON_HSDPA_EMERGCALL_WORKAROUND
                dosCallId = CALL_MODEM_ID_NONE;
                // In the case when emergency number checking is needed to be ignored
                // the destination address is delivered
                EmergencyCallCreateReq( &callReq );
                // Emergency calls are created immediately, without
                // setting properties
                messageId = CALL_MODEM_CREATE_REQ;
                isiCallId = CALL_MODEM_CREATE_REQ_OFFSET_CALLID;
#ifdef NCP_COMMON_HSDPA_EMERGCALL_WORKAROUND
                }
            else
                {
                // Destroy all data connections before attempting emergency
                // call. The call will be set up in GpdsContextsClearResp.
TFLOGSTRING( "TSY: CMmCallMessHandler::ExtFuncL - HSDPA workaround. Emergency call delayed, data connections have to be closed first." );
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL - HSDPA workaround. Emergency call delayed, data connections have to be closed first." );
                iEmergCallDelayed = ETrue;
                GpdsContextsClearReq();
                }
#endif //NCP_COMMON_HSDPA_EMERGCALL_WORKAROUND
            break;
            }
        case EEtelCallDial:
        case EMobileCallDialISV:
        case EMobileCallDialNoFdnCheck:
            {
            // Set iNoFDNDial flag in correct state
            if ( EMobileCallDialNoFdnCheck == aIpc )
                {
                iNoFdnDial = ETrue;
                }
            else
                {
                iNoFdnDial = EFalse;
                }
            if ( RMobilePhone::ECircuitDataService == callMode )
                {
                iFDNErrorAlredyNotified = EFalse;
                ret = DialDataCall( callData );
                requestDirectedToDataPort = ETrue;
                }
            else if ( RMobilePhone::EVoiceService == callMode )
                {
                iCallMode = CALL_MODEM_MODE_SPEECH;
                }
            else
                {
                iCallMode = CALL_MODEM_MODE_ALS_LINE_2;
                }

            if ( ! requestDirectedToDataPort )
                {
                iIs3rdPartyDial = EFalse;

                // If 3rd party client
                if ( EMobileCallDialISV == aIpc )
                    {
                    iIs3rdPartyDial = ETrue;
                    }
                // No else

                // Check if CUG info is needed
                RMobileCall::TMobileCallParamsV1Pckg* callParams( NULL );
                RMobileCall::TMobileCallInfoV1* callInfo( NULL );

                aDataPackage->UnPackData( &callParams, &callInfo );

                RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1(
                    reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>
                        ( callParams ) );

                RMobileCall::TMobileCallParamsV1 recentCallParams(
                    ( *paramsPckgV1 )() );

                if ( RMmCustomAPI::KETelCustomExtCustomCallParamsV3 ==
                        recentCallParams.ExtensionId() )
                    {
TFLOGSTRING("TSY: CMmCallMessHandler::ExtFuncL - Custom API call params in use ");
OstTrace0( TRACE_NORMAL, DUP7_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL - Custom API call params in use" );

                    // Unpack custom call parameters
                    RMmCustomAPI::TCustomCallParamsPckg* customparamsPckgV1(
                        reinterpret_cast<RMmCustomAPI::TCustomCallParamsPckg*>
                            ( paramsPckgV1 ) );

                    RMmCustomAPI::TCustomCallParams& aCustomParams(
                        ( *customparamsPckgV1 )() );
                    iSubAddress.Copy( aCustomParams.iSubAddress );
                    iBearer.Copy( aCustomParams.iBearer );
                    }
            #if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
                else if ( RMobileCall::KETelMobileCallParamsV7 == recentCallParams.ExtensionId() )
                    {
TFLOGSTRING("TSY: CMmCallMessHandler::ExtFuncL - V7 call params in use ");
OstTrace0( TRACE_NORMAL, DUP8_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL - V7 call params in use" );

                    RMobileCall::TMobileCallParamsV7Pckg* paramsPckgV7(
                        reinterpret_cast<RMobileCall::TMobileCallParamsV7Pckg*>
                            ( callParams ) );

                    RMobileCall::TMobileCallParamsV7 recentCallParamsV7(
                        ( *paramsPckgV7 )() );

                    iSubAddress.Copy( recentCallParamsV7.iSubAddress );
                    // At the moment only 1 bearer capability is supported in SET UP CALL PCmd
                    iBearer.Copy( recentCallParamsV7.iBearerCap1 );

                    // Prevent FDN check if call is SIM originated (SET UP CALL)
                    if ( RMobileCall::EOriginatorSIM == recentCallParamsV7.iCallParamOrigin )
                        {
                        iNoFdnCheck = ETrue;
                        }
                    else
                        {
                        iNoFdnCheck = EFalse;
                        }
                    }
            #endif
                else
                    {
                    iSubAddress.Zero();
                    iBearer.Zero();
                    }

TFLOGSTRING2("TSY: CMmCallMessHandler::ExtFuncL - Cug explicit invoked: %d", recentCallParams.iCug.iExplicitInvoke );
OstTrace1( TRACE_NORMAL, DUP5_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL - Cug explicit invoked: %d", recentCallParams.iCug.iExplicitInvoke );
                // if CUG is explicitly invoked by
                // the other entries in this structure will be filled in
                // function CallPropertySetReq.
                if ( recentCallParams.iCug.iExplicitInvoke )
                    {
                    // Non-emergency calls set properties first. CallCreateReq will
                    // be called after CallPropertySetResp response has come.
                    messageId = CALL_MODEM_PROPERTY_SET_REQ;
                    dosCallId = CALL_MODEM_PROP_CUG_INFO;

                    CallPropertySetReq( &callReq, callData );
                    }
                else
                    {
//                    RMobileCall::TMobileCallParamsV1Pckg* callParams( NULL );
//                    RMobileCall::TMobileCallInfoV1* callInfo( NULL );
//                    aDataPackage->UnPackData( &callParams, &callInfo );
                    iTelNumber = callInfo->iDialledParty.iTelNumber;
TFLOGSTRING2("TSY: CMmCallMessHandlerExtFuncL - iTelNumber: %S", &iTelNumber);
OstTraceExt1( TRACE_NORMAL, DUP6_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL - iTelNumber=%s", iTelNumber );
                    iIdRestrict = recentCallParams.iIdRestrict;

                    CallCreateReq();
                    callCreatedAlready = ETrue;
                    // Reset information
                    iSubAddress.Zero();
                    iBearer.Zero();
                    }
                }
            // No else
            break;
            }
        case EMobileCallSwap:
            {
            // Set special call Id
            dosCallId = CALL_MODEM_ID_ACTIVE;
            messageId = CALL_MODEM_CONTROL_REQ;
            isiCallId = CALL_MODEM_CONTROL_REQ_OFFSET_CALLID;
            CallControlReq( &callReq, aIpc );
            break;
            }
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallGoOneToOne:
        case EMobileCallTransfer:
        case EMobileCallActivateCCBS:
        case EMobilePhoneAcceptCCBSRecall:
            {
            messageId = CALL_MODEM_CONTROL_REQ;
            isiCallId = CALL_MODEM_CONTROL_REQ_OFFSET_CALLID;
            CallControlReq( &callReq, aIpc );
            break;
            }
        case EMobileCallAnswerISV:
        case EEtelCallAnswer:
            {
            if ( RMobilePhone::ECircuitDataService == callMode )
                {
                ret = AnswerIncomingDataCall( callData );
                requestDirectedToDataPort = ETrue;
                }
            else
                {
                iIs3rdPartyAnswerIncomingCall = EFalse;

                // Is it from a 3rd party client or not
                if ( EMobileCallAnswerISV == aIpc )
                    {
                    iIs3rdPartyAnswerIncomingCall = ETrue;
                    }
                // No else
                messageId = CALL_MODEM_ANSWER_REQ;
                isiCallId = CALL_MODEM_ANSWER_REQ_OFFSET_CALLID;
                callReq.Set8bit(
                    ISI_HEADER_SIZE + CALL_MODEM_ANSWER_REQ_OFFSET_SUBBLOCKS,
                    KCallPadding );
                }
            break;
            }
        case EEtelCallHangUp:
            {
            // Call mode for emergency call is voice
            if ( RMobilePhone::EVoiceService == callMode )
                {
                iEmergCallDelayed = EFalse;
                }

            if ( RMobilePhone::ECircuitDataService == callMode )
                {
                ret = HangUp( callId );
                requestDirectedToDataPort = ETrue;
                }
            else
                {
                messageId = CALL_MODEM_RELEASE_REQ;
                isiCallId = CALL_MODEM_RELEASE_REQ_OFFSET_CALLID;
                CallReleaseReq( &callReq, callData );
                }
            // No else
            break;
            }
        case EMobilePhoneClearBlacklist:
            {
            messageId = CALL_MODEM_BLACKLIST_CLEAR_REQ;
            break;
            }
        // CSD fearures
        case EEtelCallLoanDataPort:
            {
            if ( iDataPortHandler )
                {
                ret = iDataPortHandler->LoanDataPort( callData );
                }
            else
                {
                ret = KErrNotReady;
                }
            requestDirectedToDataPort = ETrue;
            break;
            }
        case EEtelCallRecoverDataPort:
            {
            if ( iDataPortHandler )
                {
                ret = iDataPortHandler->RecoverDataPort( callData );
                }
            else
                {
                ret = KErrNotReady;
                }
            requestDirectedToDataPort = ETrue;
            break;
            }
        case EMobileCallActivateUUS:
            {
            ret = ActivateUUS( aDataPackage );
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmCallMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_EXTFUNCL, "CMmCallMessHandler::ExtFuncL; - Unknown IPC=%d", aIpc );
            ret = KErrNotSupported;
            break;
            }
        }

    if ( ! requestDirectedToDataPort && !callCreatedAlready
#ifdef NCP_COMMON_HSDPA_EMERGCALL_WORKAROUND
         && !iEmergCallDelayed
#endif //NCP_COMMON_HSDPA_EMERGCALL_WORKAROUND
         )
        {
        // Create ISI message
        callReq.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, messageId );

        // Every message definition doesn't contains call ID
        if ( 0 != isiCallId )
            {
            callReq.Set8bit( ISI_HEADER_SIZE + isiCallId, dosCallId );
            }
        // No else

        if ( KErrNone == ret )
            {
            // Send message to PhoNet
            ret = iPhoNetSender->Send( callReq.Complete() );
            }
        // No else
        }
    // No else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallPropertySetReq
// Constructs CALL_MODEM_PROPERTY_SET_REQ message (first phase of voice dial request)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallPropertySetReq(
    TIsiSend* aCallReq,
    const CCallDataPackage* aDataPackage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallPropertySetReq");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLPROPERTYSETREQ, "CMmCallMessHandler::CallPropertySetReq" );

    RMobileCall::TMobileCallParamsV1Pckg* callParams( NULL );
    RMobileCall::TMobileCallInfoV1* callInfo( NULL );
    aDataPackage->UnPackData( &callParams, &callInfo );

    RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1(
        reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>( callParams ) );

    RMobileCall::TMobileCallParamsV1 recentCallParams( ( *paramsPckgV1 )() );

    TBool suppressPref( recentCallParams.iCug.iSuppressPrefCug );
    TBool suppressOA( recentCallParams.iCug.iSuppressOA );
    TInt cugIndex( recentCallParams.iCug.iCugIndex );

    aCallReq->Set8bit( ISI_HEADER_SIZE + CALL_MODEM_PROPERTY_SET_REQ_OFFSET_SUBBLOCKS, 2 );

TFLOGSTRING("TSY: CMmCallMessHandler::CallPropertySetReq - Call Property Info sub block ");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLPROPERTYSETREQ, "CMmCallMessHandler::CallPropertySetReq - Call Property Info sub block" );

    // Initialize message offset
    TInt currentMsgOffset(
        ISI_HEADER_SIZE + CALL_MODEM_PROPERTY_SET_REQ_OFFSET_PROPERTYINFO );

    // Create call property info sub block CALL_MODEM_SB_PROPERTY_INFO
    TBuf8<SIZE_CALL_MODEM_SB_PROPERTY_INFO> propertyInfoBuf;
    TIsiSubBlock propertyInfoSb(
        propertyInfoBuf,
        CALL_MODEM_SB_PROPERTY_INFO,
        EIsiSubBlockTypeId8Len8 );

    propertyInfoBuf.Append( CALL_MODEM_PROP_CUG_INFO ); // Property
    propertyInfoBuf.Append( KCallPadding ); // Padding

    // Add subblock to the message
    aCallReq->CopyData( currentMsgOffset, propertyInfoSb.CompleteSubBlock() );

    // Set new offset
    currentMsgOffset = currentMsgOffset + SIZE_CALL_MODEM_SB_PROPERTY_INFO;

TFLOGSTRING("TSY: CMmCallMessHandler::CallPropertySetReq - Call Modem Cug Info sub block ");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_CALLPROPERTYSETREQ, "CMmCallMessHandler::CallPropertySetReq - Call Modem Cug Info sub block" );

    // Create call GSM CUG info subblock
    TBuf8< SIZE_CALL_MODEM_SB_CUG_INFO > callGsmCugInfoBuf;
    TIsiSubBlock callGsmCugInfoSb(
        callGsmCugInfoBuf,
        CALL_MODEM_SB_CUG_INFO,
        EIsiSubBlockTypeId8Len8 );

    if ( !suppressPref )
        {
        callGsmCugInfoBuf.Append( CALL_MODEM_CUG_PREF );
        cugIndex = CALL_MODEM_CUG_DEFAULT;
        }
    else
        {
        callGsmCugInfoBuf.Append( CALL_MODEM_CUG_PREF_SUPPRESS );
        if ( 0 == cugIndex )
            {
            cugIndex = CALL_MODEM_CUG_DEFAULT;
            }
        // No else
        }

    if ( !suppressOA )
        {
        callGsmCugInfoBuf.Append( CALL_MODEM_CUG_OA );
        }
    else
        {
        callGsmCugInfoBuf.Append( CALL_MODEM_CUG_OA_SUPPRESS );
        }

TFLOGSTRING("TSY: CMmCallMessHandler::CallPropertySetReq - Cug Index copy 16 to 8 ");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_CALLPROPERTYSETREQ, "CMmCallMessHandler::CallPropertySetReq - Cug Index copy 16 to 8" );

    callGsmCugInfoBuf.Append( static_cast<TUint8>( cugIndex >> KShiftByOneByte ) );
    callGsmCugInfoBuf.Append( static_cast<TUint8>( cugIndex ) );

    callGsmCugInfoBuf.Append( KCallPadding );
    callGsmCugInfoBuf.Append( KCallPadding );

    // Complete subblock and add subblock to the message
    aCallReq->CopyData(
        currentMsgOffset,
        callGsmCugInfoSb.CompleteSubBlock() );

    iTelNumber = callInfo->iDialledParty.iTelNumber;
TFLOGSTRING2("TSY: CMmCallMessHandler::CallPropertySetReq - iTelNumber: %S", &iTelNumber);
OstTraceExt1( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_CALLPROPERTYSETREQ, "CMmCallMessHandler::CallPropertySetReq - iTelNumber=%S", iTelNumber );
    iIdRestrict = recentCallParams.iIdRestrict;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallPropertySetResp
// Breaks CALL_MODEM_PROPERTY_SET_RESP message (first phase of voice dial request).
// If a success result is received from Domestic OS, this method send the
// actual Dial request to the Domestic OS. In case of failure, Dial request
// will be completed to the Symbian OS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallPropertySetResp(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallPropertySetResp");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLPROPERTYSETRESP, "CMmCallMessHandler::CallPropertySetResp" );

    // Create call property get response
    // if property set request fails, complete dial

    TUint sbStartOffSet( 0 );

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_PROPERTY_SET_RESP,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get cause type
        TUint8 causeType( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE) );

        // Get cause value
        TUint8 causeValue( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        // Complete dial with error value
        TInt result( CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_CALL,
            causeType,
            causeValue ) );

        // Unique call id (without possible generic id)
        //(three bits (1-3) contain unique call ID)
        TInt callId( static_cast<TInt>( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CALL_MODEM_PROPERTY_GET_RESP_OFFSET_MESSAGEID )
                & KMaskBits1to3 ) );

        // Create call package
        CCallDataPackage callData;
        // Pack the data for sending to the manager
        callData.SetCallIdAndMode( callId, RMobilePhone::EVoiceService );

        if ( iIs3rdPartyDial )
            {
            // 3rd party client, process is over
            iIs3rdPartyDial = EFalse;

            // Complete failed dial request
            iMessageRouter->Complete(
                EMobileCallDialISV,
                &callData,
                result );
            }
        else if ( iNoFdnDial )
            {
            // Complete failed dial request
            iMessageRouter->Complete(
                EMobileCallDialNoFdnCheck,
                &callData,
                result );
            // Reset internal flag
            iNoFdnDial = EFalse;
            }
        else
            {
            // Complete failed dial request
            iMessageRouter->Complete( EEtelCallDial, &callData, result );
            }
        }
    // Else send the actual dial request
    else
        {
        CallCreateReq();
        }

    // Reset information
    iSubAddress.Zero();
    iBearer.Zero();
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallCreateReq
// Constructs the dial request message.and sends it to the domestic OS.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::CallCreateReq()
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallCreateReq");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLCREATEREQ, "CMmCallMessHandler::CallCreateReq" );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_CALL );
    isimsg.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_CREATE_REQ_OFFSET_TRID,
        KCallTransId );

    isimsg.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_CREATE_REQ_OFFSET_MESSAGEID,
        CALL_MODEM_CREATE_REQ );

    // Number of subblocks( also SAT originated subblocks)
    TUint8 numOfSbInMessage( 0 );

    // Initialize message offset
    TInt currentMsgOffset( ISI_HEADER_SIZE + CALL_MODEM_CREATE_REQ_OFFSET_MODE );

    // Create subblocks for CALL_MODEM_CREATE_REQ
    GetCallCreateReqSubBlock(
        iCallMode,
        dynamic_cast<TDesC16&>( iTelNumber ),
        iIdRestrict,
        isimsg,
        numOfSbInMessage,
        currentMsgOffset );

#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
    GetSatOriginatedSubBlocks(
        isimsg,
        numOfSbInMessage,
        currentMsgOffset );
#else
    GetSatOriginatedSubBlocks(
        iSubAddress,
        iBearer,
        isimsg,
        numOfSbInMessage,
        currentMsgOffset );
#endif

    // Set number of subblocs in CALL_MODEM_CREATE_REQ message
    isimsg.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_CREATE_REQ_OFFSET_SUBBLOCKS,
        numOfSbInMessage );

    // Message sent via Phonet
    return ( iPhoNetSender->Send( isimsg.Complete() ) );
    }


// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallCreateResp
// Breaks the response to the Dial request. Completes the Dial request in case
// of failure. Otherwise does not do anything (status indications handle the
// successful case).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallCreateResp(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallCreateResp");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLCREATERESP, "CMmCallMessHandler::CallCreateResp" );

    TUint sbStartOffSet( 0 );

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_CREATE_RESP,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        RMobileCall::TMobileCallInfoV1 mobileCallInfo;
        RMobileCall::TMobileCallStatus statusETel( RMobileCall::EStatusIdle );

        mobileCallInfo.iService = RMobilePhone::EVoiceService;

        if ( CALL_MODEM_MODE_ALS_LINE_2 == iCallMode )
            {
            mobileCallInfo.iService = RMobilePhone::EAuxVoiceService;
            }

        mobileCallInfo.iStatus = statusETel;

        // Get Cause Type
        TUint8 causeType( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );

        // Get Cause Value
        TUint8 causeValue( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        // Map error value to Symbian OS error value
        mobileCallInfo.iExitCode = CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_CALL,
            causeType,
            causeValue );

        // To prevent FDN error to be completed again from CallServiceDeniedInd
        if ( CALL_MODEM_CAUSE_FDN_NOT_OK == causeValue )
            {
            iFDNErrorAlredyNotified = ETrue;
            }

        // Unique call id (without possible generic id)
        // (three bits (1-3) contain unique call ID)
        mobileCallInfo.iCallId = static_cast<TInt>( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CALL_MODEM_CREATE_RESP_OFFSET_CALLID )
                & KMaskBits1to3 );

        // Create call package
        CCallDataPackage callData;
        // Pack the data for sending to the manager
        callData.SetCallIdAndMode(
            mobileCallInfo.iCallId,
            mobileCallInfo.iService );

        if ( CALL_MODEM_ID_NONE == mobileCallInfo.iCallId ) // Call has no ID
            {
            if ( iNoFdnDial )
                {
                // Complete failed dial request
                iMessageRouter->Complete(
                    EMobileCallDialNoFdnCheck,
                    &callData,
                    mobileCallInfo.iExitCode );
                }
            else if ( iIs3rdPartyDial )
                {
                // 3rd party client
                // Process is over
                iIs3rdPartyDial = EFalse;
                iMessageRouter->Complete(
                    EMobileCallDialISV,
                    &callData,
                    mobileCallInfo.iExitCode );
                }
            else
                {
                // Complete failed dial request with error value
                iMessageRouter->Complete(
                    EEtelCallDial,
                    &callData,
                    mobileCallInfo.iExitCode );
                }
            }
        else
            {
            // COMPLETE MOBILE CALL INFO CHANGE
            // (Mobile call information is always completed before call status)
            // Pack mobile call info
            callData.PackData( &mobileCallInfo );
            // Complete mobile call info change
            iMessageRouter->Complete(
                EMobileCallGetMobileCallInfo,
                &callData,
                KErrNone );

            // COMPLETE MOBILE CALL STATUS CHANGE
            // Pack call status
            callData.PackData( &statusETel );
            // Complete status change indication
            iMessageRouter->Complete(
                EMobileCallNotifyMobileCallStatusChange,
                &callData,
                mobileCallInfo.iExitCode );
            }
        }
    // Reset internal flag
    iNoFdnDial = EFalse;
    iNoFdnCheck = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::EmergencyCallCreateReq
// Created emergency call request message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::EmergencyCallCreateReq(
    TIsiSend* aCallReq )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::EmergencyCallCreateReq");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_EMERGENCYCALLCREATEREQ, "CMmCallMessHandler::EmergencyCallCreateReq" );

    // (TAN) Save the call mode of the latest CALL_MODEM_CREATE_REQ. This is
    // used when CALL_MODEM_CREATE_RESP is received and message handler should
    // be able to identify if the resp is emergency or speech call.
    iCallMode = CALL_MODEM_MODE_EMERGENCY;

    // Number of Subblocks
    TUint8 numOfSbInMessage( 0 );

    // Initialize message offset
    TInt currentMsgOffset( ISI_HEADER_SIZE + CALL_MODEM_CREATE_REQ_OFFSET_MODE );

    // Create subblocks for CallCreateReq
    GetCallCreateReqSubBlock(
        iCallMode,
        iTelEmergNumber,
        RMobileCall::ESendMyId,
        *aCallReq,
        numOfSbInMessage,
        currentMsgOffset );

    // Set number of subblocs in CALL_MODEM_CREATE_REQ message
    aCallReq->Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_CREATE_REQ_OFFSET_SUBBLOCKS,
        numOfSbInMessage );

    iTelEmergNumber.Zero();
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::MapCallStatus
// Maps Domestic OS status value to Symbian OS status.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RMobileCall::TMobileCallStatus CMmCallMessHandler::MapCallStatus(
    TUint8 aCallStatus )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::MapCallStatus");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_MAPCALLSTATUS, "CMmCallMessHandler::MapCallStatus" );

    RMobileCall::TMobileCallStatus symbianStatus( RMobileCall::EStatusUnknown );

    switch ( aCallStatus )
        {
        case CALL_MODEM_STATUS_IDLE:
            {
            symbianStatus = RMobileCall::EStatusIdle;
            break;
            }
        case CALL_MODEM_STATUS_CREATE:
            {
            symbianStatus = RMobileCall::EStatusDialling;
            break;
            }
        case CALL_MODEM_STATUS_MO_ALERTING:
            {
            symbianStatus = RMobileCall::EStatusConnecting;
            break;
            }
        case CALL_MODEM_STATUS_WAITING:
        case CALL_MODEM_STATUS_MT_ALERTING:
            {
            symbianStatus = RMobileCall::EStatusRinging;
            break;
            }
        case CALL_MODEM_STATUS_ANSWERED:
            {
            symbianStatus = RMobileCall::EStatusAnswering;
            break;
            }
        case CALL_MODEM_STATUS_ACTIVE:
            {
            symbianStatus = RMobileCall::EStatusConnected;
            break;
            }
        case CALL_MODEM_STATUS_MO_RELEASE:
            {
            symbianStatus = RMobileCall::EStatusDisconnecting;
            break;
            }
        case CALL_MODEM_STATUS_MT_RELEASE:
            {
            if ( iDisconnectingWithInband )
                {
                symbianStatus = RMobileCall::EStatusDisconnectingWithInband;
                iDisconnectingWithInband = EFalse;
                }
            else
                {
                symbianStatus = RMobileCall::EStatusDisconnecting;
                }
            break;
            }
        case CALL_MODEM_STATUS_HOLD:
            {
            symbianStatus = RMobileCall::EStatusHold;
            break;
            }
        case CALL_MODEM_STATUS_COMING:
            {
            symbianStatus = RMobileCall::EStatusUnknown;
            break;
            }
        case CALL_MODEM_STATUS_RETRIEVE_INITIATED:
        case CALL_MODEM_STATUS_RECONNECT_PENDING:
        case CALL_MODEM_STATUS_PROCEEDING:
        case CALL_MODEM_STATUS_HOLD_INITIATED:
        case CALL_MODEM_STATUS_SWAP_INITIATED:
        default:
            {
            symbianStatus = RMobileCall::EStatusUnknown;
            break;
            }
        }
    return symbianStatus;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallAnswerResp
// Breaks the response to the Answer request. In case of failure, answer
// request is completed. Otherwise status indications handle the success case.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallAnswerResp(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallAnswerResp");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLANSWERRESP, "CMmCallMessHandler::CallAnswerResp" );

    TUint sbStartOffSet( 0 );

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_ANSWER_RESP,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get cause type
        TUint8 causeType( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );

        // Get cause value
        TUint8 causeValue( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        // Map error value
        TInt result( CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_CALL,
            causeType,
            causeValue ) );

        // Unique call id (without possible generic id)
        // (three bits (1-3) contain unique call ID)
        TInt callId( static_cast<TInt>( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CALL_MODEM_ANSWER_RESP_OFFSET_CALLID )
                & KMaskBits1to3 ) );

        // Create call package
        CCallDataPackage callData;
        // Pack the data for sending to the manager
        callData.SetCallIdAndMode( callId, RMobilePhone::EVoiceService );

        if ( iIs3rdPartyAnswerIncomingCall )
            {
            // Process is finished, set it to EFalse
            iIs3rdPartyAnswerIncomingCall = EFalse;
            // If req was made by a 3rd party client, complete failed request
            iMessageRouter->Complete(
                EMobileCallAnswerISV,
                &callData,
                result );
            }
        else
            {
            // If not 3rd, complete failed request
            iMessageRouter->Complete( EEtelCallAnswer, &callData, result );
            }
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallReleaseReq
// Constructs HangUp request message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallReleaseReq(
    TIsiSend* aCallReq,
    const CCallDataPackage* aDataPackage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallReleaseReq");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLRELEASEREQ, "CMmCallMessHandler::CallReleaseReq" );

    TUint8 cause( KCallCauseNoCause );
    TInt symbianCauseValue( KErrNone );
    TBool autoStChangeDisable( EFalse );

    // Unpack dial parameters
    aDataPackage->UnPackData( symbianCauseValue, autoStChangeDisable );

    if ( KErrGsmBusyUserRequest == symbianCauseValue )
        {
        cause = CALL_MODEM_CAUSE_BUSY_USER_REQUEST;
        }
    else
        {
        cause = CALL_MODEM_CAUSE_RELEASE_BY_USER;
        }

    iReleaseCauseValueSent = cause;

    // CALL_MODEM_SB_CAUSE subblock
    TBuf8< SIZE_CALL_MODEM_SB_CAUSE > callCauseBuf( 0 );
    TIsiSubBlock callCauseSb(
        callCauseBuf,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8 );

    // Number of subblocks
    TUint8 numOfSbInMessage( 0 );

    // Initialize message offset
    TInt currentMsgOffset( ISI_HEADER_SIZE + CALL_MODEM_RELEASE_REQ_OFFSET_CAUSE );

    callCauseBuf.Append( CALL_MODEM_CAUSE_TYPE_CLIENT );
    callCauseBuf.Append( cause );
    aCallReq->CopyData( currentMsgOffset, callCauseSb.CompleteSubBlock() );

    // Increase subblock count
    numOfSbInMessage++;

    // Disable automatic retrieve for held call
    if ( autoStChangeDisable )
        {
        // Set new offset
        currentMsgOffset = currentMsgOffset + SIZE_CALL_MODEM_SB_CAUSE;

        // CALL_MODEM_SB_STATE_AUTO_CHANGE subblock
        TBuf8< SIZE_CALL_MODEM_SB_STATE_AUTO_CHANGE > callStateAutoChangeBuf( 0 );
        TIsiSubBlock callStateAutoChangeSb(
            callStateAutoChangeBuf,
            CALL_MODEM_SB_STATE_AUTO_CHANGE,
            EIsiSubBlockTypeId8Len8 );

        // Disables auto retrieve when a waiting call exists.
        callStateAutoChangeBuf.Append( CALL_MODEM_AUTO_ST_CHANGE_DISABLE );
        callStateAutoChangeBuf.Append( KCallPadding );

        aCallReq->CopyData(
            currentMsgOffset,
            callStateAutoChangeSb.CompleteSubBlock() );

        // Increase subblock count
        numOfSbInMessage++;
        }
    // No else

    // Set number of subblocs in CALL_MODEM_RELEASE_REQ message
    aCallReq->Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RELEASE_REQ_OFFSET_SUBBLOCKS,
        numOfSbInMessage );

    // Hangup causes iIncomingCallInfo to be reset
    // (no effect if this was an MO call)
    TInt callId( KSymbianCallIdNone );
    RMobilePhone::TMobileService mode( RMobilePhone::EServiceUnspecified );
    aDataPackage->GetCallIdAndMode( callId, mode );

    if ( callId == iIncomingCallInfo.iCallId )
        {
        ResetIncomingCallInfo( iIncomingCallInfo );
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallReleaseResp
// Breaks response to the HangUp request. In case of failure, HangUp is
// completed to the Symbian OS layer. Otherwise status indications handle
// the success case.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallReleaseResp(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallReleaseResp");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLRELEASERESP, "CMmCallMessHandler::CallReleaseResp" );

    TUint8 causeType( 0 );
    TUint8 causeValue( KCallCauseNoCause );

    TUint sbStartOffSet( 0 );

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RELEASE_RESP,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get cause type
        causeType = aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE );

        // Get cause value
        causeValue = aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE );

        // Complete hangup if release has failed
        if ( iReleaseCauseValueSent != causeValue )
            {
            // Map error value
            TInt result( CMmStaticUtility::CSCauseToEpocError(
                PN_MODEM_CALL,
                causeType,
                causeValue ) );

            // Unique call id (without possible generic id)
            // (three bits (1-3) contain unique call ID)
            TInt callId( static_cast<TInt>( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + CALL_MODEM_RELEASE_RESP_OFFSET_CALLID )
                    & KMaskBits1to3 ) );

            // Create call package
            CCallDataPackage callData;
            // pack the data for sending to the manager
            callData.SetCallIdAndMode( callId, RMobilePhone::EVoiceService );

            // Complete failed HangUp request
            iMessageRouter->Complete( EEtelCallHangUp, &callData, result );
            }
        // No else
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallReleaseInd
// Breaks call release indication message containing information whether the
// inband tone is generated by network or should it be generated locally.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallReleaseInd(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallReleaseInd");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLRELEASEIND, "CMmCallMessHandler::CallReleaseInd" );

    TUint sbStartOffSet( 0 );

    // Get the releaseinfo from the ISI message
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RELEASE_IND,
        CALL_MODEM_SB_RELEASE_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get inband tone information
        TUint8 inbandToneInfo( aIsiMessage.Get8bit (
            sbStartOffSet + CALL_MODEM_SB_RELEASE_INFO_OFFSET_RELEASEINFO ) );

        // If inband tone is not played by network, inform it to
        // Phone Engine.
        if ( CALL_MODEM_RI_IN_BAND_INFO & inbandToneInfo )
            {
            iDisconnectingWithInband = ETrue;
            }
        // No else
        }
    // No else

    // Get the diagnostic octet
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RELEASE_IND,
        CALL_MODEM_SB_SS_DIAGNOSTICS,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
          iDiagnosticOctet = aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_SS_DIAGNOSTICS_OFFSET_DIAGNOSTICS );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallTerminatedInd
// Breaks call terminated indication message containing
// GSM MM cause information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallTerminatedInd(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallTerminatedInd");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLTERMINATEDIND, "CMmCallMessHandler::CallTerminatedInd" );

    TUint sbStartOffSet( 0 );

    // Get the GSM MM cause from the ISI message
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_TERMINATED_IND,
        CALL_MODEM_SB_MM_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get Cause Value
        TUint8 mmCauseValue( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_MM_CAUSE_OFFSET_CAUSE ) );

        // Map error value to Symbian OS error value
        if ( CALL_MODEM_MM_CAUSE_CONGESTION == mmCauseValue )
            {
            // Save error code (used when call status idle is received)
            iGsmMmCauseError = CMmStaticUtility::EpocErrorCode(
                KErrEtelNoCarrier,
                KErrGsmMMCongestion );
            }
        }
#if ( NCP_COMMON_CELLMO_BRANCH_SUPPORT == NCP_COMMON_CELLMO_BRANCH_0711 )
    // If call was rejected because of barred CS (DSAC),
    // CALL_DSAC_INFO subblock exists in CALL_TERMINATED_IND.
    if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_TERMINATED_IND,
        CALL_DSAC_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::CallTerminatedInd, DSAC: call was rejected because of barred CS");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLTERMINATEDIND, "CMmCallMessHandler::CallTerminatedInd, DSAC: call was rejected because of barred CS" );
        // Save error code. It is completed when CALL_STATUS_IND is received.
        iGsmMmCauseError = CMmStaticUtility::EpocErrorCode(
            KErrGeneral, KErrGsmCSConnectionBarred );
        }
#endif // NCP_COMMON_CELLMO_BRANCH_SUPPORT == NCP_COMMON_CELLMO_BRANCH_0711
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallControlReq
// This method constructs call control (e.g. Hold, Swap, Split etc.) message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallControlReq(
     TIsiSend* aCallReq,
     TInt aIpc ) const
    {

TFLOGSTRING2("TSY: CMmCallMessHandler::CallControlReq. Ipc:%d",aIpc);
OstTrace1( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLCONTROLREQ, "CMmCallMessHandler::CallControlReq;aIpc=%d", aIpc );

    TUint8 operation( 0 );

    switch ( aIpc )
        {
        case EMobileCallHold:
            {
            operation = CALL_MODEM_OP_HOLD;
            break;
            }
        case EMobileCallResume:
            {
            operation = CALL_MODEM_OP_RETRIEVE;
            break;
            }
        case EMobileCallSwap:
            {
            operation = CALL_MODEM_OP_SWAP;
            break;
            }
        case EMobileCallTransfer:
            {
            operation = CALL_MODEM_OP_TRANSFER;
            break;
            }
        case EMobileCallGoOneToOne:
            {
            operation = CALL_MODEM_OP_CONFERENCE_SPLIT;
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmCallMessHandler::CallControlReq - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLCONTROLREQ, "CMmCallMessHandler::CallControlReq - Unknown aIpc=%d", aIpc );
            break;
            }
        }

    // Call operation sub block, no operation info used
    TBuf8< SIZE_CALL_MODEM_SB_OPERATION > callOperationBuf( 0 );
    TIsiSubBlock callOperationSb(
        callOperationBuf,
        CALL_MODEM_SB_OPERATION,
        EIsiSubBlockTypeId8Len8 );

    // Initialize message offset
    TInt currentMsgOffset(
        ISI_HEADER_SIZE + CALL_MODEM_CONTROL_REQ_OFFSET_OPERATION );

    callOperationBuf.Append( operation );
    callOperationBuf.Append( KCallPadding ); // Operation info

    aCallReq->CopyData( currentMsgOffset,
        callOperationSb.CompleteSubBlock() );

    // Set number of subblocs in CALL_MODEM_RELEASE_REQ message
    aCallReq->Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_CONTROL_REQ_OFFSET_SUBBLOCKS,
        1 );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::GetCallCreateReqSubBlock
// This method constructs a subblock for the Dial request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::GetCallCreateReqSubBlock(
    TUint8 aMode,
    const TDesC16& aTelNumber,
    RMobileCall::TMobileCallIdRestriction aIdRestrict,
    TIsiSend& aCallIsiMsg,
    TUint8& aNumOfSbInMsg,
    TInt& aCurrentMsgOffset )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock" );

    iDestPostAddressIncluded = EFalse;

    // CALL_MODEM_SB_MODE subblock
    TBuf8<SIZE_CALL_MODEM_SB_MODE> callModeBuf;

    TIsiSubBlock callModeSb( callModeBuf, CALL_MODEM_SB_MODE, EIsiSubBlockTypeId8Len8 );

    // Call mode
    callModeBuf.Append( aMode );

    // CALL_MODEM_MODE_INFO must be zero
    callModeBuf.Append( KCallPadding ); // Padding

    // Add call mode subblock
    aCallIsiMsg.CopyData( aCurrentMsgOffset, callModeSb.CompleteSubBlock() );

    // Set new offset and increase subblock count
    aCurrentMsgOffset = aCurrentMsgOffset + SIZE_CALL_MODEM_SB_MODE;
    aNumOfSbInMsg++;

TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_MODE CREATED");
    OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_MODE CREATED" );

    if ( CALL_MODEM_MODE_EMERGENCY == aMode && 0 < aTelNumber.Length() )
        {
        // Create destination address subblock. Size of data buffer
        // "max length of destination address" + "message data"
        TBuf8<SIZE_CALL_MODEM_SB_DESTINATION_ADDRESS + KAddressSubBlockMaxLength>
            destinationAddressBuf;
        TIsiSubBlock destinationAddressSb(
            destinationAddressBuf,
            CALL_MODEM_SB_DESTINATION_ADDRESS,
            EIsiSubBlockTypeId8Len8 );

        // Address type
        destinationAddressBuf.Append( CALL_MODEM_NBR_PLAN_ISDN_TELEPHONY );
        // Byte not used
        destinationAddressBuf.Append( KCallPadding ); // Padding
        // Padding byte
        destinationAddressBuf.Append( KCallPadding ); // Padding
        // Amounth of characters
        destinationAddressBuf.Append( aTelNumber.Length() );

        // Add the number string
        TIsiUtility::CopyToBigEndian( aTelNumber, destinationAddressBuf );

        aCallIsiMsg.CopyData(
            aCurrentMsgOffset, destinationAddressSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + destinationAddressBuf.Length();
        aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_DESTINATION_ADDRESS - Subblock Count:  %d", aNumOfSbInMsg );
OstTraceExt1( TRACE_NORMAL, DUP17_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock- CALL_MODEM_SB_DESTINATION_ADDRESS - Subblock Count=%hhu", aNumOfSbInMsg );

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - emergDestAddress: %S", &aTelNumber);
OstTraceExt1( TRACE_NORMAL, DUP18_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock;emergDestAddress=%S", aTelNumber );

        }

    else if ( CALL_MODEM_MODE_EMERGENCY != aMode && 0 < aTelNumber.Length() )
        {
        // Get length of the given number
        TInt telNumLength( aTelNumber.Length() );
        TUint8 presentationInd( CALL_MODEM_PRESENT_DEFAULT );

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - aIdRestrict: %d", aIdRestrict);
OstTrace1( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock;aIdRestrict=%d", aIdRestrict );

        if ( KTelephonyNumberMaxLength < telNumLength )
            {
            telNumLength = KTelephonyNumberMaxLength;
            }

        TBool internationalAddr( EFalse );
        TBool postAddressSupported( EFalse );
        TBool preAddressSupported( EFalse );
        TInt preAddressLength( 0 );
        TInt destAddressStarts( 0 );
        TInt postAddressStarts( 0 );

        // Destination address max length can be KTelephonyNumberMaxLength
        TBuf16<KTelephonyNumberMaxLength> destAddress;

        // Check prefix address
        if ( 0 < telNumLength &&
            ( '*' == ( aTelNumber )[0] || '#' == ( aTelNumber )[0] ) )
            {
            for ( TInt i ( 1 ); i < telNumLength; i++ )
                {
                if ( 'p' == ( aTelNumber )[i] )
                    {
                    // Post address starts
                    break; // Exit for loop
                    }
                else if ( '#' == ( aTelNumber )[i] )
                    {
                    preAddressLength = i + 1;
                    }
                // No else
                }
            }
        // No else

        // Check if there is calling line identification restriction prefix in
        // the telephony number. Destination pre address subblock will be
        // created only for *31# and #31# commands.
        if ( KPreAddressLength == preAddressLength &&
             '3' == ( aTelNumber )[1] &&
             '1' == ( aTelNumber )[2] &&
             '#' == ( aTelNumber )[3] )
            {
            preAddressSupported = ETrue;
            destAddressStarts = KPreAddressLength;

            // Set presentationInd based on prefix
            if ( '*' == ( aTelNumber )[0] )
                {
                presentationInd = CALL_MODEM_PRESENT_ALLOWED;
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by PREFIX = CALL_MODEM_PRESENT_ALLOWED");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by PREFIX = CALL_MODEM_PRESENT_ALLOWED" );
                }
            else if ( '#' == ( aTelNumber )[0] )
                {
                presentationInd = CALL_MODEM_PRESENT_RESTRICTED;
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by PREFIX = CALL_MODEM_PRESENT_RESTRICTED");
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by PREFIX = CALL_MODEM_PRESENT_RESTRICTED" );
                }
            }
        else
            {
            // Set presentationInd based on aIdRestrict
            switch ( aIdRestrict )
                {
                case RMobileCall::ESendMyId:
                    {
                    presentationInd = CALL_MODEM_PRESENT_ALLOWED; //0x01
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by aIdRestrict = CALL_MODEM_PRESENT_ALLOWED");
OstTrace0( TRACE_NORMAL, DUP5_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by aIdRestrict = CALL_MODEM_PRESENT_ALLOWED" );
                    break;
                    }
                case RMobileCall::EDontSendMyId:
                    {
                    presentationInd = CALL_MODEM_PRESENT_RESTRICTED; //0x02
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by aIdRestrict = CALL_MODEM_PRESENT_RESTRICTED");
OstTrace0( TRACE_NORMAL, DUP6_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by aIdRestrict = CALL_MODEM_PRESENT_RESTRICTED" );
                    break;
                    }
                case RMobileCall::EIdRestrictDefault:
                default:
                    {
                    presentationInd = CALL_MODEM_PRESENT_DEFAULT; //0x00
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by aIdRestrict = CALL_MODEM_PRESENT_DEFAULT");
OstTrace0( TRACE_NORMAL, DUP7_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock -- presentationInd by aIdRestrict = CALL_MODEM_PRESENT_DEFAULT" );
                    break;
                    }
                }
            }

        if ( CALL_MODEM_PRESENT_DEFAULT != presentationInd )
            {

TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock -- Create line id sub block");
OstTrace0( TRACE_NORMAL, DUP8_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock -- Create line id sub block" );
// Create CALL_MODEM_SB_LINE_ID subblock.

            TBuf8<SIZE_CALL_MODEM_SB_LINE_ID> lineIdBuf( 0 );
            TIsiSubBlock lineIdSb(
                lineIdBuf,
                CALL_MODEM_SB_LINE_ID,
                EIsiSubBlockTypeId8Len8 );

            lineIdBuf.Append( presentationInd );
            lineIdBuf.Append( KCallPadding ); //Filler

            aCallIsiMsg.CopyData(
                aCurrentMsgOffset, lineIdSb.CompleteSubBlock() );

            // Set new offset and increase subblock count
            aCurrentMsgOffset =
                aCurrentMsgOffset + lineIdBuf.Length();
            aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_LINE_ID - Subblock Count:  %d", aNumOfSbInMsg );
OstTrace1( TRACE_NORMAL, DUP19_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_LINE_ID - Subblock Count:%d", aNumOfSbInMsg );

            }
        // No else

        // Check if there is post address
        for ( TInt i( destAddressStarts ); i < telNumLength; i++ )
            {
            if ( 'p' == ( aTelNumber )[i] && preAddressLength <= i )
                {
                // Destination address ends and post address starts.
                postAddressSupported = ETrue;
                postAddressStarts = i;
                break; // Exit for loop
                }
            else if ( '+' == ( aTelNumber )[i] && preAddressLength == i )
                {
                // Destination address part contains "+" character. For example
                // +35850123456 or *140#+35850123456. Don't add "+" char to the
                // destination address.
                internationalAddr = ETrue;
                }
            else
                {
                destAddress.Append( ( TUint16 )( aTelNumber )[i] );
                }
            }

        // Create destination address subblock. Size of data buffer
        // "max length of destination address" + "message data"
        TBuf8<SIZE_CALL_MODEM_SB_DESTINATION_ADDRESS + KAddressSubBlockMaxLength>
            destinationAddressBuf;
        TIsiSubBlock destinationAddressSb(
            destinationAddressBuf,
            CALL_MODEM_SB_DESTINATION_ADDRESS,
            EIsiSubBlockTypeId8Len8 );

        if ( internationalAddr )
            {
            destinationAddressBuf.Append(
                CALL_MODEM_NBR_TYPE_INTERNATIONAL | CALL_MODEM_NBR_PLAN_ISDN_TELEPHONY );
            }
        else
            {
            destinationAddressBuf.Append( CALL_MODEM_NBR_PLAN_ISDN_TELEPHONY );
            }

        // Byte not used
        destinationAddressBuf.Append( KCallPadding ); // Padding
        // Padding byte
        destinationAddressBuf.Append( KCallPadding ); // Padding
        // Amounth of characters
        destinationAddressBuf.Append( destAddress.Length() );

        // Add the number string
        TIsiUtility::CopyToBigEndian( destAddress, destinationAddressBuf );

        aCallIsiMsg.CopyData(
            aCurrentMsgOffset, destinationAddressSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + destinationAddressBuf.Length();
        aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_DESTINATION_ADDRESS - Subblock Count:  %d", aNumOfSbInMsg );
OstTraceExt1( TRACE_NORMAL, DUP10_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock- CALL_MODEM_SB_DESTINATION_ADDRESS - Subblock Count=%hhu", aNumOfSbInMsg );

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - destAddress: %S", &destAddress);
OstTraceExt1( TRACE_NORMAL, DUP16_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock;destAddress=%S", destAddress );

        if ( preAddressSupported )
            {
            // Pre address length can be KTelephonyNumberMaxLength
            TBuf16<KTelephonyNumberMaxLength> preAddress;

            // Create pre address subblock
            TBuf8<SIZE_CALL_MODEM_SB_DESTINATION_PRE_ADDRESS +
                KTelephonyNumberMaxLength * 2> preAddressBuf;

            TIsiSubBlock preAddressSb(
                preAddressBuf,
                CALL_MODEM_SB_DESTINATION_PRE_ADDRESS,
                EIsiSubBlockTypeId8Len8 );

            preAddressBuf.Append( KCallPadding );
            preAddressBuf.Append( preAddressLength );
            for ( TInt i ( 0 ); i < preAddressLength; i++ )
                {
                preAddress.Append( ( TUint16 )( aTelNumber )[i] );
                }

            TIsiUtility::CopyToBigEndian( preAddress, preAddressBuf );

            aCallIsiMsg.CopyData(
                aCurrentMsgOffset,
                preAddressSb.CompleteSubBlock() );

            // Set new offset and increase subblock count
            aCurrentMsgOffset =
                aCurrentMsgOffset + preAddressBuf.Length();
            aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_DESTINATION_PRE_ADDRESS - Subblock Count:  %d", aNumOfSbInMsg );
OstTraceExt1( TRACE_NORMAL, DUP11_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock- CALL_MODEM_DESTINATION_PRE_ADDRESS - Subblock Count=%hhu", aNumOfSbInMsg );
            }
        // No else

        if ( postAddressSupported )
            {
            // Creates Post Address subblock
            TInt postAddLength( telNumLength - postAddressStarts );

            // Post address length can be KTelephonyNumberMaxLength
            TBuf16<KTelephonyNumberMaxLength> postAddress;

            // Size of data buffer is "max post address length" + "message data"
            TBuf8<SIZE_CALL_MODEM_SB_DESTINATION_POST_ADDRESS +
                KTelephonyNumberMaxLength * 2> postAddressBuf;
            TIsiSubBlock postAddressSb(
                postAddressBuf,
                CALL_MODEM_SB_DESTINATION_POST_ADDRESS,
                EIsiSubBlockTypeId8Len8 );

            postAddressBuf.Append( KCallPadding );
            postAddressBuf.Append( ( TUint8 )postAddLength );

            for ( TInt i( postAddressStarts ); i < telNumLength; i++ )
                {
                postAddress.Append( ( TUint16 )( aTelNumber )[i] );
                }

            TIsiUtility::CopyToBigEndian( postAddress, postAddressBuf );

            aCallIsiMsg.CopyData(
                aCurrentMsgOffset,
                postAddressSb.CompleteSubBlock() );

            // Set new offset and increase subblock count
            aCurrentMsgOffset =
                aCurrentMsgOffset + postAddressBuf.Length();
            aNumOfSbInMsg++;

            // Destination post address included
            iDestPostAddressIncluded = ETrue;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock  - CALL_MODEM_DESTINATION_POST_ADDRESS - Subblock Count:  %d", aNumOfSbInMsg );
OstTraceExt1( TRACE_NORMAL, DUP14_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock- CALL_MODEM_DESTINATION_POST_ADDRESS - Subblock Count=%hhu", aNumOfSbInMsg );
            }
        // No else

        // Subblock for user to user signaling (for skype service).
        if ( 0 < iUusData.iUUI.Length() && 0 != iUusData.iServiceReq )
            {
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock. Creating sb for UUS signaling");
OstTrace0( TRACE_NORMAL, DUP12_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock. Creating sb for UUS signaling" );

            TBuf8<KUserToUserSbMaxLength> userToUserBuf;
            TBool validServiceType( ETrue );

            TIsiSubBlock userToUserSb(
                userToUserBuf,
                CALL_MODEM_SB_USER_TO_USER,
                EIsiSubBlockTypeId8Len8 );

            // Only UUS1 is supported
            userToUserBuf.Append( CALL_MODEM_UUS_TYPE_UUS1 );

            switch( iUusData.iServiceReq )
                {
                case RMobileCall::KUUS1Implicit:
                    {
                    userToUserBuf.Append( CALL_MODEM_UUS_ACTIVATION_IMPLICIT );
                    break;
                    }
                default:
                    {
                    validServiceType = EFalse;
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock. switch UUS type case default. Illegal service req!");
OstTrace0( TRACE_NORMAL, DUP13_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock. Switch UUS type case default. Illegal service req!" );
                    break;
                    }
                }

            // Creating subblock if service type was ok
            if ( validServiceType )
                {
                userToUserBuf.Append( iUusData.iUUI.Length() );

                // Copying lower bytes of 16-bit data as 8-bit data field of isi msg.
                // So, higher bytes will be discarded.
                // (due to iUUI is TMobileCallUUI, which is 16-bit TBuf)
                for ( TInt i( 0 ); i < iUusData.iUUI.Length(); i++ )
                    {
                    userToUserBuf.Append( iUusData.iUUI[i] );
                    }

                aCallIsiMsg.CopyData(
                        aCurrentMsgOffset,
                        userToUserSb.CompleteSubBlock() );

                // Set new offset and increase subblock count
                aCurrentMsgOffset = aCurrentMsgOffset + userToUserBuf.Length();

                aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_USER_TO_USER - Subblock Count: %d", aNumOfSbInMsg );
                OstTraceExt1( TRACE_NORMAL, DUP15_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock- CALL_MODEM_SB_USER_TO_USER - Subblock Count=%hhu", aNumOfSbInMsg );
                }

            // Reset data
            iUusData.iUUI.Zero();
            iUusData.iServiceReq = 0;
            }
        if ( iNoFdnDial )
            {
TFLOGSTRING("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock -- Create check info sub block");
OstTrace0( TRACE_NORMAL, DUP9_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock -- Create check info sub block" );

            // Create CALL_MODEM_SB_LINE_ID subblock.

            TBuf8<SIZE_CALL_MODEM_SB_CHECK_INFO> checkInfoBuf( 0 );

            TIsiSubBlock checkInfoSb(
                checkInfoBuf,
                CALL_MODEM_SB_CHECK_INFO,
                EIsiSubBlockTypeId8Len8 );

            checkInfoBuf.Append( CALL_MODEM_CHK_DISABLE_FDN );
            checkInfoBuf.Append( KCallPadding ); //Filler

            aCallIsiMsg.CopyData(
                            aCurrentMsgOffset, checkInfoSb.CompleteSubBlock() );

            // Set new offset and increase subblock count
            aCurrentMsgOffset =
                            aCurrentMsgOffset + checkInfoBuf.Length();
            aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_CHECK_INFO - Subblock Count:  %d", aNumOfSbInMsg );
OstTraceExt1( TRACE_NORMAL, DUP21_CMMCALLMESSHANDLER_GETCALLCREATEREQSUBBLOCK, "CMmCallMessHandler::GetCallCreateReqSubBlock - CALL_MODEM_SB_CHECK_INFO - Subblock Count=%hhu", aNumOfSbInMsg );

            }
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallControlResp
// This method breaks the result of the Call Control request. In case of
// failure the appropriate control operation (e.g. hold, swap, split) will be
// completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallControlResp(
    const TIsiReceiveC &aIsiMessage ) const
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallControlResp");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLCONTROLRESP, "CMmCallMessHandler::CallControlResp" );

    TUint sbStartOffSet( 0 );

    // Get call operation sub block
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + CALL_MODEM_CONTROL_RESP_OFFSET_OPERATION,
        CALL_MODEM_SB_OPERATION,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TUint8 operation( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_OPERATION_OFFSET_OPERATION ) );

        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_CONTROL_RESP,
            CALL_MODEM_SB_CAUSE,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // Get call cause type
            TUint8 causeType( aIsiMessage.Get8bit(
                sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );

            // Get call cause value
            TUint8 causeValue( aIsiMessage.Get8bit(
                sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

            TInt errorCode( CMmStaticUtility::CSCauseToEpocError(
                PN_MODEM_CALL,
                causeType,
                causeValue ) );

            TInt ipc( 0 );

            switch( operation )
                {
                case CALL_MODEM_OP_HOLD:
                    {
                    // Complete the failed hold operation
                    ipc = EMobileCallHold;
                    break;
                    }
                case CALL_MODEM_OP_RETRIEVE:
                    {
                    // Complete the failed retrieve operation
                    ipc = EMobileCallResume;
                    break;
                    }
                case CALL_MODEM_OP_SWAP:
                    {
                    // Complete the failed swap operation
                    ipc = EMobileCallSwap;
                    break;
                    }
                case CALL_MODEM_OP_CONFERENCE_SPLIT:
                    {
                    // Complete the failed split operation
                    ipc = EMobileCallGoOneToOne;
                    break;
                    }
                case CALL_MODEM_OP_TRANSFER:
                    {
                    ipc = EMobileCallTransfer;
                    // In case not allowed complete with KErrNone
                    if ( CALL_MODEM_CAUSE_NOT_ALLOWED == causeValue )
                        {
                        errorCode = CMmStaticUtility::EpocErrorCode(
                            KErrNotReady,
                            KErrNotFound );
                        }
                    // No else
                    break;
                    }
                case CALL_MODEM_OP_CONFERENCE_BUILD:
                default:
                    {
TFLOGSTRING2("TSY: CMmCallMessHandler::CallControlResp - Not supported operation: %d", operation);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLCONTROLRESP, "CMmCallMessHandler::CallControlResp - Not supported operation=%hhu", operation );
                    break;
                    }
                }

            // Complete the failed operation
            if ( 0 != ipc )
                {
                // Unique call id (without possible generic id)
                // (three bits (1-3) contain unique call ID)
                TInt callId( static_cast<TInt>( aIsiMessage.Get8bit(
                    ISI_HEADER_SIZE + CALL_MODEM_CONTROL_RESP_OFFSET_CALLID )
                        & KMaskBits1to3 ) );

                // Create call package
                CCallDataPackage callData;
                // Pack the data for sending to the manager
                callData.SetCallIdAndMode(
                    callId,
                    RMobilePhone::EVoiceService );

                // Complete failed request
                iMessageRouter->Complete( ipc, &callData, errorCode );
                }
            // No else
            }
         // No else
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::GetSatOriginatedSubBlocks
// This method constructs Sim Application Toolkit related subblock for the Dial
// request containing bearer and subaddress information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
void CMmCallMessHandler::GetSatOriginatedSubBlocks(
    TIsiSend& aCallIsiMsg,
    TUint8& aNumOfSbInMsg,
    TInt& aCurrentMsgOffset ) const
    {
TFLOGSTRING("TSY: CMmCallMessHandler::GetSatOriginatedSubBlocks");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_GETSATORIGINATEDSUBBLOCKS, "TSY: CMmCallMessHandler::GetSatOriginatedSubBlocks" );

    //Creates sub address sub block
    if ( 0 != iSubAddress.Length() )
        {
        //Length of message data is max address length + message
        TBuf8<KAddressSubBlockMaxLength> subAddressBuf( 0 );

        TIsiSubBlock callDestCsAddressSb( subAddressBuf,
            CALL_MODEM_SB_DESTINATION_CS_ADDRESS, EIsiSubBlockTypeId8Len8 );

        //Fillers
        subAddressBuf.Append( KCallPadding );
        subAddressBuf.Append( KCallPadding );
        subAddressBuf.Append( KCallPadding );

        // Called Party Number length
        subAddressBuf.Append( iSubAddress.Length() );

        // Copy 16 to 8
        TIsiUtility::CopyToBigEndian( iSubAddress, subAddressBuf );

        aCallIsiMsg.CopyData( aCurrentMsgOffset,
            callDestCsAddressSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + subAddressBuf.Length();
        aNumOfSbInMsg++;
        }
     //no else

    //Creates bearer capabilities sub block
    if ( 0 != iBearer.Length() )
        {

        TBuf8<KAddressSubBlockMaxLength> callGsmCcpBuf( 0 );
        TIsiSubBlock callGsmCcpSb( callGsmCcpBuf,
            CALL_MODEM_SB_BC, EIsiSubBlockTypeId8Len8 );

        //aBearer includes length and data bytes, so no need to add length separately
        callGsmCcpBuf.Append( iBearer );

        aCallIsiMsg.CopyData( aCurrentMsgOffset,
            callGsmCcpSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + callGsmCcpBuf.Length();
        aNumOfSbInMsg++;
        }
    //no else
    }

#else

void CMmCallMessHandler::GetSatOriginatedSubBlocks(
    const TBuf<KSubAddressSize>& aSubAddress,
    TBuf8<KBearerCapabilitySize>& aBearer,
    TIsiSend& aCallIsiMsg,
    TUint8& aNumOfSbInMsg,
    TInt& aCurrentMsgOffset ) const
    {
TFLOGSTRING("TSY: CMmCallMessHandler::GetSatOriginatedSubBlocks");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_GETSATORIGINATEDSUBBLOCKS, "CMmCallMessHandler::GetSatOriginatedSubBlocks" );

    // Creates sub address sub block
    if ( 0 != aSubAddress.Length() )
        {
        // Length of message data is max address length + message
        TBuf8<KAddressSubBlockMaxLength> subAddressBuf( 0 );

        TIsiSubBlock callDestCsAddressSb(
            subAddressBuf,
            CALL_MODEM_DESTINATION_CS_ADDRESS,
            EIsiSubBlockTypeId8Len8 );

        // Replace Destination Address
        subAddressBuf.Append( KCallPadding );
        subAddressBuf.Append( KCallPadding );
        subAddressBuf.Append( KCallPadding );

        // Called Party Number length
        subAddressBuf.Append( aSubAddress.Length() );

        // Copy 16 to 8
        TIsiUtility::CopyToBigEndian( aSubAddress, subAddressBuf );

        aCallIsiMsg.CopyData(
            aCurrentMsgOffset,
            callDestCsAddressSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset = aCurrentMsgOffset + subAddressBuf.Length();
        aNumOfSbInMsg++;
        }
     // no else

    // Creates bearer capabilities sub block
    if ( 0 != aBearer.Length() )
        {
        TBuf8<KAddressSubBlockMaxLength> callGsmCcpBuf( 0 );
        TIsiSubBlock callGsmCcpSb(
            callGsmCcpBuf,
            CALL_MODEM_SB_BC,
            EIsiSubBlockTypeId8Len8 );

        // aBearer includes length and data bytes,
        // so no need to add length separately
        callGsmCcpBuf.Append( aBearer );

        aCallIsiMsg.CopyData(
            aCurrentMsgOffset,
            callGsmCcpSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset = aCurrentMsgOffset + callGsmCcpBuf.Length();
        aNumOfSbInMsg++;
        }
    // No else
    }
#endif

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallControlInd
// This method breaks the Call control indication message. In case of failure,
// the approproate control operation will be completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallControlInd(
    const TIsiReceiveC &aIsiMessage ) const
    {
    TInt errorCode( KErrNone );

    // Unique call id (without possible generic id)
    // (three bits (1-3) contain unique call ID)
    TInt callId( static_cast<TInt>( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CALL_MODEM_CONTROL_IND_OFFSET_CALLID )
            & KMaskBits1to3 ) );

TFLOGSTRING2("TSY: CMmCallMessHandler::CallControlInd, CallID: %d", callId);
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLCONTROLIND, "CMmCallMessHandler::CallControlInd" );

    TUint8 callOperation( 0 );
    TUint sbStartOffSet( 0 );

    // Get call operation sub block
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + CALL_MODEM_CONTROL_IND_OFFSET_OPERATION,
        CALL_MODEM_SB_OPERATION,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get call operation
        callOperation = aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_OPERATION_OFFSET_OPERATION );
        }
    // No else

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_CONTROL_IND,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TInt operation( 0 );

        // Get call cause type
        TUint8 causeType( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );

        // Get call cause value
        TUint8 causeValue( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        // Map error value
        errorCode = CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_CALL,
            causeType,
            causeValue );

        switch( callOperation )
            {
            // Complete the hold operation
            case CALL_MODEM_OP_HOLD:
                {
                operation = EMobileCallHold;
                break;
                }
            // Complete the resume operation
            case CALL_MODEM_OP_RETRIEVE:
                {
                operation = EMobileCallResume;
                break;
                }
            case CALL_MODEM_OP_SWAP:
                {
                operation = EMobileCallSwap;
                break;
                }
            case CALL_MODEM_OP_CONFERENCE_SPLIT:
                {
                operation = EMobileCallGoOneToOne;
                break;
                }
            case CALL_MODEM_OP_TRANSFER:
                {
                operation = EMobileCallTransfer;
                break;
                }
            default:
                {
TFLOGSTRING2("TSY: CMmCallMessHandler::CallControlInd - Not supported operation: %d", operation);
OstTrace1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLCONTROLIND, "CMmCallMessHandler::CallControlInd - Not supported operation=%d", operation );
                break;
                }
            }
        CCallDataPackage callPackage;
        callPackage.SetCallIdAndMode(
            callId,
            RMobilePhone::EServiceUnspecified );

        if ( 0 != operation )
            {
            // Unique call id (without possible generic id)
            //( three bits (1-3) contain unique call ID)
            callId = static_cast<TInt>( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + CALL_MODEM_CONTROL_IND_OFFSET_CALLID )
                    & KMaskBits1to3 );

            // Create call package
            CCallDataPackage callData;
            // Pack the data for sending to the manager
            callData.SetCallIdAndMode( callId, RMobilePhone::EVoiceService );

            // Complete failed request
            iMessageRouter->Complete( operation, &callData, errorCode );
            }
        // No else
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallGsmNotificationInd
// This method breaks the GSM notification indication that contains information
// about remote events occurred (remote hold, resume, transfer or conference)
// or about call forwarding (MT only).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallGsmNotificationInd(
    const TIsiReceiveC &aIsiMessage )
    {
    RMobileCall::TMobileCallEvent callEvent( RMobileCall::ERemoteHold );

    // Create call property get response
    // Get call id, (three bits (1-3) contain unique call ID)
    TInt callId( static_cast<TInt>( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CALL_MODEM_NOTIFICATION_IND_OFFSET_CALLID )
            & KMaskBits1to3 ) );

TFLOGSTRING2("TSY: CMmCallMessHandler::CallGsmNotificationInd. CallId:%d",callId);
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCallMessHandler::CallGsmNotificationInd" );

    CCallDataPackage callDataPackage;

    TUint sbStartOffSet( 0 );

    // Check for CallGsmSsHoldIndicator sub block
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_HOLD_INDICATOR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get the hold indicator information from sub block
        TUint8 holdIndicator( aIsiMessage.Get8bit(
            sbStartOffSet +
                CALL_MODEM_SB_SS_HOLD_INDICATOR_OFFSET_SSHOLDINDICATOR ) );

        // If remote hold, complete event to client
        if ( CALL_MODEM_HOLD_IND_ON_HOLD == holdIndicator )
            {
            callEvent = RMobileCall::ERemoteHold;
            }
        // Else if remote resume, complete event to client
        else if ( CALL_MODEM_HOLD_IND_RETRIEVED == holdIndicator )
            {
            callEvent = RMobileCall::ERemoteResume;
            }
        // No else

        // Pack the data for sending to the manager
        callDataPackage.SetCallIdAndMode(
            callId,
            RMobilePhone::EServiceUnspecified );
        callDataPackage.PackData( &callEvent );
        iMessageRouter->Complete(
            EMobileCallNotifyCallEvent,
            &callDataPackage,
            KErrNone );
        }
    // No else

    // Check for CallGsmSsEctIndicator sub block
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_ECT_INDICATOR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get the ectIndictor
        TUint8 ectIndicator( aIsiMessage.Get8bit(
            sbStartOffSet +
                CALL_MODEM_SB_SS_ECT_INDICATOR_OFFSET_SSECTINDICATOR ) );

        if ( CALL_MODEM_ECT_CALL_STATE_ACTIVE == ectIndicator )
            {
            callEvent = RMobileCall::ERemoteTransferring;
            }
        else // CALL_MODEM_ECT_CALL_STATE_ALERT or other, unknown value
            {
            callEvent = RMobileCall::ERemoteTransferAlerting;
            }
        // Pack the data for sending to the manager
        callDataPackage.SetCallIdAndMode(
            callId,
            RMobilePhone::EServiceUnspecified);
        callDataPackage.PackData( &callEvent );
        iMessageRouter->Complete(
            EMobileCallNotifyCallEvent,
            &callDataPackage,
            KErrNone );
        }
    // No else

    // Check for CallGsmSsNotifyIndicator sub block
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_NOTIFY_INDICATOR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get the multiparty indicator information from sub block
        TUint8 SsIndicator( aIsiMessage.Get8bit(
            sbStartOffSet +
                CALL_MODEM_SB_SS_NOTIFY_INDICATOR_OFFSET_SSINDICATOR ) );

        // We are only interested in the CALL_MODEM_SSI_MPTY bit of
        // the CALL_MODEM_SS_INDICATOR bit field
        if ( CALL_MODEM_SSI_MPTY & SsIndicator )
            {
            callEvent = RMobileCall::ERemoteConferenceCreate;
            // Pack the data for sending to the manager
            callDataPackage.SetCallIdAndMode(
                callId,
                RMobilePhone::EServiceUnspecified );
            callDataPackage.PackData( &callEvent );
            iMessageRouter->Complete(
                EMobileCallNotifyCallEvent,
                &callDataPackage,
                KErrNone );
            }
        // No else
        }
    // No else

    // Check call forwarding indicator (applies to MT calls only)
    if ( callId == iIncomingCallInfo.iCallId )
        {
        // We are only interested in the CALL_MODEM_SSN_INCOMING_IS_FWD bit of
        // the CALL_MODEM_SS_NOTIFICATION bit field

        // Check for CallModemSbSsNotify sub block
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
            CALL_MODEM_SB_SS_NOTIFY,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            if ( CALL_MODEM_SSN_INCOMING_IS_FWD & aIsiMessage.Get8bit(
                    sbStartOffSet +
                        CALL_MODEM_SB_SS_NOTIFY_OFFSET_SSNOTIFICATION ) )
                {
                // This is a MT call that has been diverted to this phone
                iIncomingCallInfo.iForwarded = ETrue;
                iIncomingCallInfo.iValid |= RMobileCall::KCallForwarded;
                }
            }
        // No else
        }
    // No else

    // Get CALL_MODEM_SB_SS_CODE sub block
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_CODE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get mmi ss code
        TUint16 mmiSsCode( aIsiMessage.Get16bit(
            sbStartOffSet + CALL_MODEM_SB_SS_CODE_OFFSET_MMISSCODE ) );

        // Get call gsm ss status sub block
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
            CALL_MODEM_SB_SS_STATUS,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            TUint8 ssStatus( aIsiMessage.Get8bit(
                sbStartOffSet + CALL_MODEM_SB_SS_STATUS_OFFSET_SSSTATUS ) );

            if ( ( ssStatus & CALL_MODEM_SS_STATUS_ACTIVE ) &&
                ( CALL_MODEM_SSC_ALL_FWDS == mmiSsCode ||
                CALL_MODEM_SSC_ALL_COND_FWD == mmiSsCode ||
                CALL_MODEM_SSC_CFU == mmiSsCode ||
                CALL_MODEM_SSC_CFB == mmiSsCode ||
                CALL_MODEM_SSC_CFNRY == mmiSsCode ||
                CALL_MODEM_SSC_CFGNC == mmiSsCode ) )
                {
                // If mmi ss code exists, it is a call forwarding code,
                // and the service is active, we have to trigger received
                // a call forwarding active notification

                // Complete CallForwardingActive notification to SOS
                RMobilePhone::TMobilePhoneCFActive activeType;
                if ( CALL_MODEM_SSC_ALL_FWDS == mmiSsCode ||
                    CALL_MODEM_SSC_CFU == mmiSsCode )
                    {
                    // Forwarding is unconditional
                    activeType = RMobilePhone::ECFUnconditionalActive;
                    }
                else
                    {
                    // Forwarding is conditional
                    activeType = RMobilePhone::ECFConditionalActive;
                    }

                // Service group info not available => hard-coded
                RMobilePhone::TMobileService serviceGroup(
                    RMobilePhone::EVoiceService );

                CMmDataPackage dataPackage;
                dataPackage.PackData( &serviceGroup, &activeType );
                iMessageRouter->Complete(
                    EMobilePhoneNotifyCallForwardingActive,
                    &dataPackage,
                    KErrNone );
                }
            // No else
            }
        // No else
        }
    // No else

    // Read call origin info
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_ORIGIN_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::CallGsmNotificationInd: CALL_MODEM_SB_ORIGIN_INFO subblock");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCallMessHandler::CallGsmNotificationInd - CALL_MODEM_SB_ORIGIN_INFO subblock" );

        TBuf16<RMobileCall::KCallingNameSize> targetOrigName;
        CCallDataPackage callData;

        // Update previously received incoming call info
        if ( callId == iIncomingCallInfo.iCallId )
            {
            ReadCallInfo(
                iIncomingCallInfo,
                targetOrigName,
                aIsiMessage,
                sbStartOffSet );
            // Call data package
            callData.SetCallIdAndMode( callId, iIncomingCallInfo.iService );
            callData.PackData( &iIncomingCallInfo );
            }
        // If incoming call info reseted already
        else
            {
            RMobileCall::TMobileCallInfoV1 callInfo;
            ReadCallInfo(
                callInfo,
                targetOrigName,
                aIsiMessage,
                sbStartOffSet );
            // Call data package
            callData.SetCallIdAndMode( callId, callInfo.iService );
            callData.PackData( &callInfo );
            }

        // COMPLETE MOBILE CALL INFO CHANGE
        // (Get mobile call information is always completed
        // before call status change)
        iMessageRouter->Complete(
            EMobileCallGetMobileCallInfo,
            &callData,
            KErrNone );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::FillModeData
// This method maps the Domestic OS call mode to the Symbian OS call mode and
// fills the information to the mobile call information package received as
// input parameter.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::FillModeData(
    RMobileCall::TMobileCallInfoV1& mobileCallInfo,
    TUint8 aIsiCallMode ) const
    {
TFLOGSTRING2("TSY: CMmCallMessHandler::FillModeData. callmode:%d",aIsiCallMode);
OstTraceExt1( TRACE_NORMAL, CMMCALLMESSHANDLER_FILLMODEDATA, "CMmCallMessHandler::FillModeData;aIsiCallMode=%hhu", aIsiCallMode );

    // Set service and alternating call mode in iMobileCallInfo
    mobileCallInfo.iAlternatingCall =
        RMobilePhone::EAlternatingModeUnspecified;
    mobileCallInfo.iService = RMobilePhone::EServiceUnspecified;
    mobileCallInfo.iEmergency = EFalse;

    switch ( aIsiCallMode )
        {
        case CALL_MODEM_MODE_SPEECH:
            {
            // Voice service
            mobileCallInfo.iService = RMobilePhone::EVoiceService;
            break;
            }
        case CALL_MODEM_MODE_EMERGENCY:
            {
            // Voice service
            mobileCallInfo.iService = RMobilePhone::EVoiceService;
            mobileCallInfo.iEmergency = ETrue;
            break;
            }
        case CALL_MODEM_MODE_MULTIMEDIA:
            {
            // The line supports video connections.
            mobileCallInfo.iService = RMobilePhone::ECircuitDataService;
            break;
            }
        case CALL_MODEM_MODE_ALS_LINE_2:
            {
            // The line supports auxiliary voice call connections.
            mobileCallInfo.iService = RMobilePhone::EAuxVoiceService;
            break;
            }
        default:
            {
            TFLOGSTRING2("TSY: CMmCallMessHandler::FillModeData - Not supported ISA call mode: %d", aIsiCallMode);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_FILLMODEDATA, "CMmCallMessHandler::FillModeData - Not supported ISA call mode=%hhu", aIsiCallMode );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::FillNumberPlanAndType
// This method maps the Domestic OS address type to the Symbian OS number plan
// and type and fills the information to the mobile call information package
// received as input parameter.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::FillNumberPlanAndType(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    TUint aOrigAddressType ) const
    {
    // Mask the nbrtype with 00001111 to get the number plan
    //(four least significant bits (1-4) contain numbering plan)
    TUint nbrplan( aOrigAddressType & KMaskBits1to4 );

TFLOGSTRING2("TSY: CMmCallMessHandler::FillNumberPlanAndType. nbrplan (mask KMaskBits1to4) : %d",nbrplan);
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_FILLNUMBERPLANANDTYPE, "CMmCallMessHandler::FillNumberPlanAndType" );

    if ( RMobilePhone::EUnknownNumberingPlan ==
        aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::FillNumberPlanAndType. EUnknownNumberingPlan");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_FILLNUMBERPLANANDTYPE, "CMmCallMessHandler::FillNumberPlanAndType - EUnknownNumberingPlan" );

        if ( CALL_MODEM_NBR_PLAN_UNKNOWN == nbrplan )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
                RMobilePhone::EUnknownNumberingPlan;
            aMobileCallInfo.iDialledParty.iNumberPlan =
                RMobilePhone::EUnknownNumberingPlan;
            }
        else if ( CALL_MODEM_NBR_PLAN_ISDN_TELEPHONY == nbrplan )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
                RMobilePhone::EIsdnNumberPlan;
            aMobileCallInfo.iDialledParty.iNumberPlan =
                RMobilePhone::EIsdnNumberPlan;
            }
        else if ( CALL_MODEM_NBR_PLAN_DATA == nbrplan )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
                RMobilePhone::EDataNumberPlan;
            aMobileCallInfo.iDialledParty.iNumberPlan =
                RMobilePhone::EDataNumberPlan;
            }
        else if ( CALL_MODEM_NBR_PLAN_TELEX == nbrplan )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
                RMobilePhone::ETelexNumberPlan;
            aMobileCallInfo.iDialledParty.iNumberPlan =
                RMobilePhone::ETelexNumberPlan;
            }
        else if ( CALL_MODEM_NBR_PLAN_PRIVATE == nbrplan )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
                RMobilePhone::EPrivateNumberPlan;
            aMobileCallInfo.iDialledParty.iNumberPlan =
                RMobilePhone::EPrivateNumberPlan;
            }
        else if ( CALL_MODEM_NBR_PLAN_NATIONAL == nbrplan )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
                RMobilePhone::ENationalNumberPlan;
            aMobileCallInfo.iDialledParty.iNumberPlan =
                RMobilePhone::ENationalNumberPlan;
            }
        else
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
                RMobilePhone::EUnknownNumberingPlan;
            aMobileCallInfo.iDialledParty.iNumberPlan =
                RMobilePhone::EUnknownNumberingPlan;
            }
        }
    // No else

    // Mask the nbrtype with 01110000 to get the number type
    // (three bits (5-7) contain type of number, the most significant bit (8)
    // is a spare bit and shall be ignored)
    TUint nbrtype( aOrigAddressType & KMaskBits5to7 );

TFLOGSTRING2("TSY: CMmCallMessHandler::FillNumberPlanAndType. nbrplan (mask KMaskBits5to7) : %d",nbrtype);
OstTrace1( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_FILLNUMBERPLANANDTYPE, "CMmCallMessHandler::FillNumberPlanAndType - nbrplan (mask KMaskBits5to7) nbrtype=%d", nbrtype );

    if ( RMobilePhone::EUnknownNumber ==
        aMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::FillNumberPlanAndType. EUnknownNumber");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_FILLNUMBERPLANANDTYPE, "CMmCallMessHandler::FillNumberPlanAndType - EUnknownNumber" );

        if ( CALL_MODEM_NBR_TYPE_UNKNOWN == nbrtype )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
                RMobilePhone::EUnknownNumber;
            aMobileCallInfo.iDialledParty.iTypeOfNumber =
                RMobilePhone::EUnknownNumber;
            }
        else if ( CALL_MODEM_NBR_TYPE_INTERNATIONAL == nbrtype )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
                RMobilePhone::EInternationalNumber;
            aMobileCallInfo.iDialledParty.iTypeOfNumber =
                RMobilePhone::EInternationalNumber;
            }
        else if ( CALL_MODEM_NBR_TYPE_NATIONAL == nbrtype )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
                RMobilePhone::ENationalNumber;
            aMobileCallInfo.iDialledParty.iTypeOfNumber =
                RMobilePhone::ENationalNumber;
            }
        else if ( CALL_MODEM_NBR_TYPE_NETWORK_SPECIFIC == nbrtype )
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
                RMobilePhone::ENetworkSpecificNumber;
            aMobileCallInfo.iDialledParty.iTypeOfNumber =
                RMobilePhone::ENetworkSpecificNumber;
            }
        else
            {
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
                RMobilePhone::EUnknownNumber;
            aMobileCallInfo.iDialledParty.iTypeOfNumber =
                RMobilePhone::EUnknownNumber;
            }
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::FillRemoteStatusAndNumber
// This method maps the Domestic OS address type and orig address to the
// Symbian OS remote information and fills the information to the mobile call
// information package received as input parameter.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::FillRemoteStatusAndNumber(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    TUint8 aOrigPresentInd,
    const TDes16& aOrigAddress,
    RMobileCall::TMobileCallDirection aDirection ) const
    {
TFLOGSTRING3("TSY: CMmCallMessHandler::FillRemoteStatusAndNumber. OrigAdr: %s. CallDirection:%d", &aOrigAddress, aDirection);
OstTraceExt2( TRACE_NORMAL, DUP5_CMMCALLMESSHANDLER_FILLREMOTESTATUSANDNUMBER, "CMmCallMessHandler::FillRemoteStatusAndNumber;OrigAddr=%S;CallDirection=%d", aOrigAddress, aDirection );

    // Find out if telephone number is allowed to be presented.
    if ( RMobileCall::ERemoteIdentityUnknown ==
        aMobileCallInfo.iRemoteParty.iRemoteIdStatus )
        {
        // CPN_ALLOWED
        if ( CALL_MODEM_PRESENT_ALLOWED == aOrigPresentInd && RMobileCall::EMobileOriginated == aDirection
                || CALL_MODEM_PRESENTATION_ALLOWED == aOrigPresentInd && RMobileCall::EMobileTerminated == aDirection)
            {
TFLOGSTRING("TSY: CMmCallMessHandler::FillRemoteStatusAndNumber. CPN_ALLOWED");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_FILLREMOTESTATUSANDNUMBER, "CMmCallMessHandler::FillRemoteStatusAndNumber - CPN_ALLOWED" );

            // Available. iRemoteIdStatus is updated using the tel number
            // presentation status. Remote name does not affect to it, Symbian's
            // response:
            // > In the meantime, I would say that iRemoteIdStatus relates to
            // > the number only and the iCallingName field may be available
            // > even if iRemoteIdStatus says "suppressed" or "not available
            aMobileCallInfo.iRemoteParty.iRemoteIdStatus =
                RMobileCall::ERemoteIdentityAvailable;

            // Copy the actual number
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Copy(
                aOrigAddress.Left( aOrigAddress.Length() ) );
            }
        // CPN_RESTRICTED & MT call
        else if ( CALL_MODEM_PRESENTATION_RESTRICTED == aOrigPresentInd &&
                  aDirection == RMobileCall::EMobileTerminated )
            {
TFLOGSTRING("TSY: CMmCallMessHandler::FillRemoteStatusAndNumber. CPN_RESTRICTED & MT call");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_FILLREMOTESTATUSANDNUMBER, "CMmCallMessHandler::FillRemoteStatusAndNumber - CPN_RESTRICTED and MT call" );

            // There are cases when remote party info should still be presented.
            // See TSW ActionDB, Notes ID: MB03081534698
            aMobileCallInfo.iRemoteParty.iRemoteIdStatus =
                RMobileCall::ERemoteIdentitySuppressed;

            // Copy the actual number
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Copy(
                aOrigAddress.Left( aOrigAddress.Length() ) );
            }
        else // CPN_NOT_AVAILABLE or CPN_RESTRICTED & MO
            {
TFLOGSTRING("TSY: CMmCallMessHandler::FillRemoteStatusAndNumber. CPN_NOT_AVAILABLE or CPN_RESTRICTED & MO ");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_FILLREMOTESTATUSANDNUMBER, "CMmCallMessHandler::FillRemoteStatusAndNumber - CPN_NOT_AVAILABLE or CPN_RESTRICTED and MO" );

            if ( CALL_MODEM_PRESENT_RESTRICTED == aOrigPresentInd &&
                 RMobileCall::EMobileOriginated == aDirection )
                {
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus =
                    RMobileCall::ERemoteIdentitySuppressed;
                }
            else
                {
TFLOGSTRING("TSY: CMmCallMessHandler::FillRemoteStatusAndNumber. OrigPresentInd is unknown");
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_FILLREMOTESTATUSANDNUMBER, "CMmCallMessHandler::FillRemoteStatusAndNumber - OrigPresentInd is unknown" );
                // Unknown
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus =
                    RMobileCall::ERemoteIdentityUnknown;
                }

            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Zero();
            }
        }
    // No else

    // Add dialled party information
    if ( 0 < aOrigAddress.Length() )
        {
        // Copy dialled party number
        aMobileCallInfo.iDialledParty.iTelNumber.Copy(
            aOrigAddress.Left( aOrigAddress.Length() ) );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::FillCallingName
// This method maps the Domestic OS information to the Symbian OS calling name
// and fills the information to the mobile call information package received as
// input parameter.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::FillCallingName(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    TUint8 aOrigInfoPresentInd,
    const TDes16& aOrigInfoName ) const
    {
TFLOGSTRING3("TSY: CMmCallMessHandler::FillCallingName. aOrigInfoPresentInd:%d, origInfoName:%s", aOrigInfoPresentInd,&aOrigInfoName);
OstTraceExt2( TRACE_NORMAL, CMMCALLMESSHANDLER_FILLCALLINGNAME, "CMmCallMessHandler::FillCallingName;aOrigInfoPresentInd=%hhu;aOrigInfoName=%S", aOrigInfoPresentInd, aOrigInfoName );

    // Find out if name is availble and can be presented.
    if ( CALL_MODEM_PRESENTATION_ALLOWED == aOrigInfoPresentInd &&
        0 < aOrigInfoName.Length() )
        {
        // iRemoteIdStatus has to be set, as ETEL MM API design document
        // states: "Indicates whether the remote party information in the
        // rest of this structure is valid or not.
        aMobileCallInfo.iRemoteParty.iRemoteIdStatus =
            RMobileCall::ERemoteIdentityAvailable;
        aMobileCallInfo.iRemoteParty.iCallingName.Copy(
            aOrigInfoName.Left( aOrigInfoName.Length() ) );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallStatusIndL
// This method breaks the Call status information message. It notifies the
// Symbian OS layer about status changes and remote party information changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallStatusIndL(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL" );

    TUint8 callStatusISA( 0xFF );
    TUint8 callModeISA( 0xFF );
    RMobileCall::TMobileCallStatus statusETel( RMobileCall::EStatusUnknown );

    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    // Update data accorging the data received
    mobileCallInfo.iValid = 0;

    // Read call id
    // Remember to mask high bits away from the call ID. They may contain
    // useless information for single call that causes problem with the call
    // retrieval. (three bits (1-3) contain unique call ID).
    mobileCallInfo.iCallId = static_cast<TInt>( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CALL_MODEM_STATUS_IND_OFFSET_CALLID )
            & KMaskBits1to3 );

    TUint sbStartOffSet( 0 );
    TUint8 origPresentInd( CALL_MODEM_PRESENT_DEFAULT );

    // Read call status
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + CALL_MODEM_STATUS_IND_OFFSET_STATUS,
        CALL_MODEM_SB_STATUS,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get ISA call status
        callStatusISA = aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_STATUS_OFFSET_STATUS );

TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_STATUS subblock");
        OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_STATUS subblock" );
        }
        // No else

    statusETel = MapCallStatus( callStatusISA );
    mobileCallInfo.iStatus = statusETel;
    if ( CALL_MODEM_STATUS_MO_ALERTING == callStatusISA )
        {
        iCallDirection = RMobileCall::EMobileOriginated;
        }
    else if ( CALL_MODEM_STATUS_MT_ALERTING == callStatusISA )
        {
        iCallDirection = RMobileCall::EMobileTerminated;
        }

    // Check if Video Call is MT Released.
    // Pipe have to remove to make next call possible.
    // Have to wait that Csd videoCall is disconnected.
    if ( CALL_MODEM_STATUS_MT_RELEASE == callStatusISA )
        {
        iVideoCallMtReleased = ETrue;
        }

    // Read call mode
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + CALL_MODEM_STATUS_IND_OFFSET_MODE,
        CALL_MODEM_SB_MODE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get call mode and add it to mobile call info struct
        callModeISA = aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_MODE_OFFSET_MODE );

TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_MODE subblock");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_MODE subblock" );

        FillModeData( mobileCallInfo, callModeISA );
        mobileCallInfo.iValid |= RMobileCall::KCallEmergency;
        }
        // No else

    if( CALL_MODEM_STATUS_IDLE == callStatusISA &&
        mobileCallInfo.iCallId == iIncomingCallInfo.iCallId )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL:NOTE! Can't complete to CTSY!, because call is terminated before it is indicated to CTSY");
OstTrace0( TRACE_NORMAL, DUP19_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL:NOTE! Can't complete to CTSY!, because call is terminated before it is indicated to CTSY" );
        // reset temporary call info
        ResetIncomingCallInfo( iIncomingCallInfo );
        }
    else
        {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL:Normal completion of call status ind (no termination of calls which haven't been indicated to CTSY yet)");
OstTrace0( TRACE_NORMAL, DUP20_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL:Normal completion of call status ind (no termination of calls which haven't been indicated to CTSY yet)" );

        // Create call package (used for all completions)
        CCallDataPackage callData;
        // Pack the data for sending to the manager
        callData.SetCallIdAndMode(
            mobileCallInfo.iCallId,
            mobileCallInfo.iService );

        CheckCallIsaStatus( mobileCallInfo, callStatusISA );

        // Store UUI data when the call is coming
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
            CALL_MODEM_SB_USER_TO_USER,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) &&
            ( CALL_MODEM_STATUS_COMING == callStatusISA ) )
            {

TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: Receive UUI.");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: Receive UUI" );

            // Get data length
            TUint8 userToUserDataLen( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_USER_TO_USER_OFFSET_UTOULEN ) );

            // Get UUIE data
            TPtrC8 data( aIsiMessage.GetData(
                sbStartOffSet + CALL_MODEM_SB_USER_TO_USER_OFFSET_UTOU,
                userToUserDataLen ) );

            // Copy UUS data to member variable. Receive UUI will be completed
            // after EEtelLineNotifyIncomingCall is completed.
            iReceivedUusData.Zero();
            iReceivedUusData.Copy( data.Left( RMobileCall::KMaxUUISize ) );
            }
        // No else

        // If status is valid in Symbian OS
        if ( RMobileCall::EStatusUnknown != statusETel )
            {
            TUint8 causeType( 0 );
            TUint8 causeValue( 0 );
            // Introduce target
            TBuf16<RMobilePhone::KMaxMobileTelNumberSize> targetOrig;
            TBuf16<RMobileCall::KCallingNameSize> targetOrigName;

            // Copy some data previously received to current MobileCallInfo
            if ( mobileCallInfo.iCallId == iIncomingCallInfo.iCallId )
                {
                mobileCallInfo.iForwarded = iIncomingCallInfo.iForwarded;
                mobileCallInfo.iValid |= RMobileCall::KCallForwarded;
                mobileCallInfo.iRemoteParty = iIncomingCallInfo.iRemoteParty;
                mobileCallInfo.iValid |= RMobileCall::KCallRemoteParty;
                }
            // No else

            // Read gsm redirecting address
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
                CALL_MODEM_SB_REDIRECTING_ADDRESS,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet ) )
                {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_REDIRECTING_ADDRESS subblock");
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_REDIRECTING_ADDRESS subblock" );

                // Get address length
                TUint8 redirectingAddressLength( aIsiMessage.Get8bit(
                    sbStartOffSet +
                    CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_ADDRLEN ) );

                // If there is a redirection address, the call has been forwarded
                // (but not vice versa; this information can come with another
                // indication)
                if ( 0 != redirectingAddressLength )
                    {
                    mobileCallInfo.iForwarded = ETrue;
                    mobileCallInfo.iValid |= RMobileCall::KCallForwarded;
                    }
                    // No else
                }
                // No else

            // Read CALL_MODEM_SB_LINE_ID
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
                CALL_MODEM_SB_LINE_ID,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet ))
                {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_LINE_ID subblock");
OstTrace0( TRACE_NORMAL, DUP21_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_LINE_ID subblock" );

                origPresentInd = aIsiMessage.Get8bit(
                    sbStartOffSet + CALL_MODEM_SB_LINE_ID_OFFSET_LINEID );
                }
            // Read all destination address
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
                CALL_MODEM_SB_DESTINATION_ADDRESS,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet ) )
                {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_DESTINATION_ADDRESS subblock");
OstTrace0( TRACE_NORMAL, DUP5_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_DESTINATION_ADDRESS subblock" );

                ReadAllAddressDestination( mobileCallInfo, targetOrig, aIsiMessage, sbStartOffSet, origPresentInd );
                }
                // No else

            if ( CALL_MODEM_STATUS_CREATE == callStatusISA )
                {
                // Reset orginator address information. Destination address
                // contents in CALL_MODEM_STATUS_IND are same as in CALL_MODEM_CREATE_REQ
                // when call is in create state.
                mobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Zero();
                mobileCallInfo.iValid &= ~RMobileCall::KCallRemoteParty;
                }
                // No else

                // Read call origin info
                if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
                    CALL_MODEM_SB_ORIGIN_INFO,
                    EIsiSubBlockTypeId8Len8,
                    sbStartOffSet ) )
                    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_ORIGIN_INFO subblock");
OstTrace0( TRACE_NORMAL, DUP6_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_ORIGIN_INFO subblock" );

                    ReadCallInfo(
                        mobileCallInfo,
                        targetOrigName,
                        aIsiMessage,
                        sbStartOffSet );
                    }
                    // No else

                // Get error value
                if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
                    CALL_MODEM_SB_CAUSE,
                    EIsiSubBlockTypeId8Len8,
                    sbStartOffSet ) )
                    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_CAUSE subblock");
OstTrace0( TRACE_NORMAL, DUP7_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_CAUSE subblock" );

                    // Get cause type and value
                    causeType = aIsiMessage.Get8bit(
                        sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE );

                    // Get call cause value
                    causeValue = aIsiMessage.Get8bit(
                        sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE );
                    }
                    // No else

                // If iDestPostAddressIncluded flag is (ETrue) we don't handle
                // CALL_MODEM_SB_DESTINATION_POST_ADDRESS subblock here
                if ( !iDestPostAddressIncluded )
                    {
                    // CALL_MODEM_SB_DESTINATION_POST_ADDRESS
                    // If Destination post address is found, save it.
                    // It will be sent later when this call is active
                    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                        ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
                        CALL_MODEM_SB_DESTINATION_POST_ADDRESS,
                        EIsiSubBlockTypeId8Len8,
                        sbStartOffSet ) )
                        {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL:CALL_MODEM_SB_DESTINATION_POST_ADDRESS");
OstTrace0( TRACE_NORMAL, DUP8_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: CALL_MODEM_SB_DESTINATION_POST_ADDRESS" );

                        // Get address length
                        TUint8 destinationPostAddressLength( aIsiMessage.Get8bit(
                            sbStartOffSet + CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_ADDRLEN ) );

                        // Get address
                        TPtrC8 postAddr( aIsiMessage.GetData(
                            sbStartOffSet + CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_ADDR,
                            destinationPostAddressLength * 2 ) );

                        // Do not handle post address which lenght is over 15 chars
                        if ( ( KDestPostAddressMaxLength * 2 ) >= postAddr.Length() )
                            {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL:lenght less than 15 char");
OstTrace0( TRACE_NORMAL, DUP9_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: Lenght less than 15 char" );
                            // Copy 8-bit name to the 16-bit target using correct endianess
                            TIsiUtility::CopyFromBigEndian(
                                postAddr,
                                iDestPostAddress );

                            TInt postAddrLength( iDestPostAddress.Length() );

                            // Store call ID to identify SIM ATK call
                            iDtmfPostAddressCallId = mobileCallInfo.iCallId;

                            // Check that there is not "w" char. "w" will mess up internal
                            // DTMF sending process
                            for ( TInt i( 0 ); i < postAddrLength; i++ )
                                {
                                if ( 'w' == iDestPostAddress[ i ] )
                                    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL:w found");
OstTrace0( TRACE_NORMAL, DUP10_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: w found" );
                                    iDestPostAddress.SetLength( 0 );
                                    iDtmfPostAddressCallId = 0;
                                    break;
                                    }
                                }
                            }
                        }

TFLOGSTRING2("TSY: CMmCallMessHandler::CallStatusIndL:callStatusISA: %d", callStatusISA);
TFLOGSTRING2("TSY: CMmCallMessHandler::CallStatusIndL:mobileCallInfo.iCallId: %d", mobileCallInfo.iCallId);
TFLOGSTRING2("TSY: CMmCallMessHandler::CallStatusIndL:iDtmfPostAddressCallId: %d", iDtmfPostAddressCallId);
OstTraceExt1( TRACE_NORMAL, DUP11_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL;callStatusISA=%hhu", callStatusISA );
OstTraceExt1( TRACE_NORMAL, DUP13_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL;mobileCallInfo.iCallId=%hhu", mobileCallInfo.iCallId );
OstTraceExt1( TRACE_NORMAL, DUP12_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL;iDtmfPostAddressCallId=%hhu", iDtmfPostAddressCallId );

                    // When call is active, send post address
                    // DTMF sending in case this is SIMATK call
                    if ( CALL_MODEM_STATUS_ACTIVE == callStatusISA &&
                        iDtmfPostAddressCallId == mobileCallInfo.iCallId &&
                        0 < iDtmfPostAddressCallId )
                        {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL:Dtmf post address sending (ACTIVE)");
OstTrace0( TRACE_NORMAL, DUP17_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL, Dtmf post address sending (ACTIVE)" );
                        iDtmfMessHandler->SendPostAddressL ( &iDestPostAddress );

                        // Reset iDestPostAddress and iDtmfPostAddressCallId to prevent multiple
                        // requests
                        iDestPostAddress.SetLength( 0 );
                        iDtmfPostAddressCallId = 0;
                        }

                    // If call fails for some reason, and goes to idle, reset
                    // iDestPostAddress and iDtmfPostAddressCallId
                    if ( CALL_MODEM_STATUS_IDLE == callStatusISA &&
                        iDtmfPostAddressCallId == mobileCallInfo.iCallId &&
                        0 < iDtmfPostAddressCallId )
                        {
TFLOGSTRING("TSY: CMmCallMessHandler::CallStatusIndL:If call fails for some reason (IDLE)");
OstTrace0( TRACE_NORMAL, DUP14_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL: If call fails for some reason (IDLE)" );
                        iDestPostAddress.SetLength( 0 );
                        iDtmfPostAddressCallId = 0;
                        }

TFLOGSTRING2("TSY: CMmCallMessHandler::CallStatusIndL:iDtmfPostAddressCallId: %d After value",iDtmfPostAddressCallId);
OstTraceExt1( TRACE_NORMAL, DUP18_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL;iDtmfPostAddressCallId=%hhu After value", iDtmfPostAddressCallId );
                    }
                    // No else

            if ( CALL_MODEM_SSD_NO_CUG == ( iDiagnosticOctet & KMaskBits1to7 ) &&
                CALL_MODEM_NW_CAUSE_FACILITY_REJECTED == causeValue )
                {
                // This is special situation. Dial is made to number that is not
                // in CUG list -> error must be mapped to KErrGsmCCUserNotInCug
                causeType = CALL_MODEM_CAUSE_TYPE_NETWORK;
                causeValue = CALL_MODEM_NW_CAUSE_NOT_IN_CUG;
                }
                // No else

                iDiagnosticOctet = 0;

                TInt resultETel( KErrNone );
                // Check if GSM MM Cause is received in CallTerminatedInd
            if ( KErrNone != iGsmMmCauseError )
                {
                resultETel = iGsmMmCauseError;
                iGsmMmCauseError = KErrNone;
                }
            else
                {
                resultETel = CMmStaticUtility::CSCauseToEpocError(
                    PN_MODEM_CALL,
                    causeType,
                    causeValue );
                }

            // COMPLETE INCOMING CALL NOTIFICATION
            CompleteIncomingCallNotif( mobileCallInfo, statusETel );
            // MT calls which went waiting can alert twice
            // reset stored incoming call information only after they actually rang
            if ( CALL_MODEM_STATUS_MT_ALERTING == callStatusISA )
                {
                ResetIncomingCallInfo( iIncomingCallInfo );
                }

        // Update mobile call info to data port handler also
            if ( ( CALL_MODEM_MODE_MULTIMEDIA == callModeISA ) &&
                CALL_MODEM_STATUS_WAITING != callStatusISA )
                {
                TBool isMultimedia( CALL_MODEM_MODE_MULTIMEDIA == callModeISA );
                TBool isWaitingCall( iWaitingCall == mobileCallInfo.iCallId );

                SetMobileCallInfo(
                    mobileCallInfo,
                    isMultimedia,
                    isWaitingCall );
                }
                // No else

                // PKCN-6NTAHB  FT: No audio alert in "No Answer"
                // condition of MO VT
            if ( CALL_MODEM_STATUS_ACTIVE == callStatusISA ||
                CALL_MODEM_STATUS_MT_RELEASE == callStatusISA ||
                CALL_MODEM_STATUS_MO_RELEASE == callStatusISA )
                {
TFLOGSTRING2("TSY: CMmCallMessHandler::CallStatusIndL:callStatusISA: %d", callStatusISA);
OstTraceExt1( TRACE_NORMAL, DUP15_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL;callStatusISA=%hhu", callStatusISA );

                TBool playRemoteAlertToneLocally( EFalse );

TFLOGSTRING2("TSY: CMmCallMessHandler::CallStatusIndL:playRemoteAlert: %d", playRemoteAlertToneLocally);
OstTraceExt1( TRACE_NORMAL, DUP16_CMMCALLMESSHANDLER_CALLSTATUSINDL, "CMmCallMessHandler::CallStatusIndL;playRemoteAlertToneLocally=%hhu", playRemoteAlertToneLocally );

                CMmDataPackage dataPackage;
                dataPackage.PackData( &playRemoteAlertToneLocally );
                iMessageRouter->Complete(
                    ECustomGetRemoteAlertingToneStatusIPC,
                    &dataPackage,
                    KErrNone );
                }
                // No else

            CompleteMobileCallInfoAndStatusChange(
                mobileCallInfo,
                statusETel,
                resultETel );

            if ( CALL_MODEM_MODE_MULTIMEDIA == callModeISA &&
               ( RMobileCall::EStatusRinging == statusETel ||
                RMobileCall::EStatusDialling == statusETel ) )
                {
                // Client needs to know if call is video
                RMobileCall::TMobileDataCallParamsV1 CSDParams;
                CSDParams.iService = RMobileCall::EServiceDataCircuitSync;
                CSDParams.iQoS = RMobileCall::EQoSTransparent;
                CSDParams.iSpeed = RMobileCall::ESpeed64000;
                CSDParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;
                // Pack CSD params
                RMobileCall::TMobileDataCallParamsV1Pckg csdParamsPckg(
                    CSDParams );
                callData.PackData( &csdParamsPckg );
                // Complete CSD params update
                iMessageRouter->Complete(
                    EEtelCallGetCallParams,
                    &callData,
                    KErrNone );
                }
            // No else

            // Store call ID for waiting call for checking that it is not
            // notified again when it gets ringing (see error item MEKK-5ZMKHG).
            if ( CALL_MODEM_STATUS_WAITING == callStatusISA )
                {
                iWaitingCall = mobileCallInfo.iCallId;
                }
            else if ( iWaitingCall == mobileCallInfo.iCallId )
                {
                iWaitingCall = KSymbianCallIdNone;
                }
            // No else
            }
        // No else

        }//end of if(incoming call && status == idle)
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallComingInd
// This method breaks incoming call indication message and saves information
// in iIncomingCallInfo.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallComingInd(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallComingInd");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLCOMINGIND, "CMmCallMessHandler::CallComingInd" );

    // Call coming ind message
    TUint8 callMode( 0xFF );
    TBuf16<RMobileCall::KCallingNameSize> targetOrigName;
    TUint sbStartOffSet( 0 );

    // Call mode is mandatory sub block
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + CALL_MODEM_COMING_IND_OFFSET_MODE,
        CALL_MODEM_SB_MODE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get call mode value
        callMode = aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_MODE_OFFSET_MODE );

TFLOGSTRING("TSY: CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_MODE");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLCOMINGIND, "CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_MODE" );

        FillModeData( iIncomingCallInfo, callMode );
        iIncomingCallInfo.iValid |= RMobileCall::KCallEmergency;
        }
    // No else

    // Check if GSM sub blocks present
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_COMING_IND,
        CALL_MODEM_SB_REDIRECTING_ADDRESS,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get Address Length
        TUint8 redirectingAddressLength( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_ADDRLEN ) );

        // Get address
        TPtrC8 address( aIsiMessage.GetData(
            sbStartOffSet + CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_ADDR,
            redirectingAddressLength * 2 ) );

TFLOGSTRING("TSY: CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_REDIRECTING_ADDRESS");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_CALLCOMINGIND, "CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_REDIRECTING_ADDRESS" );

        // If there is a redirection address, the call has been forwarded
        // (but not vice versa; this information can come with another
        // indication)
        if ( NULL != address.Length() )
            {
            iIncomingCallInfo.iForwarded = ETrue;
            iIncomingCallInfo.iValid |= RMobileCall::KCallForwarded;
            }
        // No else
        }
    // No else

    // Get unique call id (without possible generic id)
    // (three bits (1-3) contain unique call ID)
    iIncomingCallInfo.iCallId = static_cast<TInt>( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CALL_MODEM_COMING_IND_OFFSET_CALLID )
            & KMaskBits1to3 );

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_COMING_IND,
        CALL_MODEM_SB_ORIGIN_ADDRESS,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TUint8 origAddressType( aIsiMessage.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_ADDRTYPE ) );

        TUint8 origPresentInd( aIsiMessage.Get8bit(
                    sbStartOffSet + CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_PRESENTATION )
                        & KMaskBits6to7 );


        // Get Address Length
        TUint8 callOriginAddressLength( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_ADDRLEN ) );

        // Get address (telephone number)
        TPtrC8 address( aIsiMessage.GetData(
            sbStartOffSet + CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_ADDR,
            callOriginAddressLength * 2 ) );

TFLOGSTRING("TSY: CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_ORIGIN_ADDRESS");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_CALLCOMINGIND, "CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_ORIGIN_ADDRESS" );

        // Check if number is international and addrDataLength is bigger
        // than zero. The length of the string does not have to be checked
        // since the original max length was 100. When the string was sent to
        // ISA side the '+' character was cut off and now we just put it back.
        // (three bits (5-7) contain type of number)
        TBuf8<RMobilePhone::KMaxMobileTelNumberSize * 2> address2;
        if ( ( CALL_MODEM_NBR_TYPE_INTERNATIONAL == ( origAddressType & KMaskBits5to7 ) )
               && ( 0 != address.Length() ) )
            {
            // Add '+' character back to the string
            address2.Append( KCallPadding ); // Padding
            address2.Append( '+' );
            }
        // No else

        // Append tel number to address2
        address2.Append( address );

        if ( 0 != address.Length() )
            {
            // Get address type
            FillNumberPlanAndType( iIncomingCallInfo, origAddressType );
            }
        // No else

        TBuf16<RMobilePhone::KMaxMobileTelNumberSize> target;
        // Copy 8-bit address to the 16-bit target using correct endianess
        TIsiUtility::CopyFromBigEndian( address2, target );

        // Set target to the iAddress
        FillRemoteStatusAndNumber(
            iIncomingCallInfo,
            origPresentInd,
            target,
            RMobileCall::EMobileTerminated );

        iIncomingCallInfo.iValid |= RMobileCall::KCallRemoteParty;
        }
    // No else

    // Call origin info
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_COMING_IND,
        CALL_MODEM_SB_ORIGIN_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get presentation indicator
        // Mask with 0110 0000 to get bits 6-7
        TUint8 origInfoPresentInd( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_ORIGIN_INFO_OFFSET_PRESENTATION )
                & KMaskBits6to7 );

        // Get name length
        TUint8 callOriginInfoNameLength( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_ORIGIN_INFO_OFFSET_NAMELEN ) );

        // Get name
        TPtrC8 name( aIsiMessage.GetData(
            sbStartOffSet + CALL_MODEM_SB_ORIGIN_INFO_OFFSET_NAME,
            callOriginInfoNameLength * 2 ) );

TFLOGSTRING("TSY: CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_ORIGIN_INFO");
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_CALLCOMINGIND, "CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_ORIGIN_INFO" );

        // Copy 8-bit name to the 16-bit target using correct endianess
        TIsiUtility::CopyFromBigEndian( name, targetOrigName );

        // Set target to the iAddress
        FillCallingName(
            iIncomingCallInfo,
            origInfoPresentInd,
            targetOrigName );
        }

    // Cause of no cli
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_COMING_IND,
        CALL_MODEM_SB_CAUSE_OF_NO_CLI,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get cause of no CLI
        TUint8 mmCauseOfNoCli( aIsiMessage.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OF_NO_CLI_OFFSET_CAUSE ) );

TFLOGSTRING("TSY: CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_CAUSE_OF_NO_CLI");
OstTrace0( TRACE_NORMAL, DUP5_CMMCALLMESSHANDLER_CALLCOMINGIND, "CMmCallMessHandler::CallComingInd -- CALL_MODEM_SB_CAUSE_OF_NO_CLI" );

        // Fill cause of no CLI
        FillCauseOfNoCli(iIncomingCallInfo, mmCauseOfNoCli);
        }

    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::ResetIncomingCallInfo
// Resets all relevant fields of aIncomingCallInfo.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::ResetIncomingCallInfo(
    RMobileCall::TMobileCallInfoV1& aIncomingCallInfo ) const
    {
TFLOGSTRING("TSY: CMmCallMessHandler::ResetIncomingCallInfo");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_RESETINCOMINGCALLINFO, "CMmCallMessHandler::ResetIncomingCallInfo" );

    // Reset all fields that are used in aIncomingCallInfo
    aIncomingCallInfo.iValid = 0;
    aIncomingCallInfo.iService = RMobilePhone::EServiceUnspecified;
    aIncomingCallInfo.iStatus = RMobileCall::EStatusUnknown;
    aIncomingCallInfo.iCallId = KSymbianCallIdNone;
    aIncomingCallInfo.iEmergency = EFalse;
    aIncomingCallInfo.iForwarded = EFalse;
    aIncomingCallInfo.iRemoteParty.iRemoteIdStatus =
        RMobileCall::ERemoteIdentityUnknown;
    aIncomingCallInfo.iRemoteParty.iDirection = RMobileCall::EDirectionUnknown;
    aIncomingCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
        RMobilePhone::EUnknownNumber;
    aIncomingCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
        RMobilePhone::EUnknownNumberingPlan;
    aIncomingCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Zero();
    aIncomingCallInfo.iRemoteParty.iCallingName.Zero();
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CallServiceDeniedInd
// This method breaks call service denied indication message and
// informs the Symbian OS layer about the received info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CallServiceDeniedInd(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CallServiceDeniedInd");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CALLSERVICEDENIEDIND, "CMmCallMessHandler::CallServiceDeniedInd" );

    // Call service denied ind message

    TUint sbStartOffSet( 0 );

    // Introduce requested service
    TInt serviceId( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_SERVICE_DENIED_IND_OFFSET_REQMESSAGEID ) );

    switch ( serviceId )
        {
        case CALL_MODEM_CREATE_REQ:
            {
            // If call create request fails, complete dial
            // (used for data/video calls when dialled number has not
            // been in the FDN list -> KCallCauseFdnNotOk).
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_CALL_MODEM_SERVICE_DENIED_IND,
                CALL_MODEM_SB_CAUSE,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet ) )
                {
                // Get cause type
                TUint8 causeType( aIsiMessage.Get8bit(
                    sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );

                // Get cause value
                TUint8 causeValue( aIsiMessage.Get8bit(
                    sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

                if ( CALL_MODEM_CAUSE_FDN_NOT_OK == causeValue &&
                     EFalse == iFDNErrorAlredyNotified )
                    {
                    // If not notified from CallCreateResp
                    // map cause to epoc error
                    TInt result( CMmStaticUtility::CSCauseToEpocError(
                        PN_MODEM_CALL,
                        causeType,
                        causeValue ) );

                    // Create call package
                    CCallDataPackage callData;
                    // Pack the data for sending to the manager
                    // (no call id or service type available)
                    callData.SetCallIdAndMode(
                        CALL_MODEM_ID_NONE,
                        RMobilePhone::ECircuitDataService );

                    if ( iIs3rdPartyDial )
                        {
                        // If 3rd party application, process is over
                        iIs3rdPartyDial = EFalse;

                        // Complete failed dial request with error value
                        iMessageRouter->Complete(
                            EMobileCallDialISV,
                            &callData,
                            result );
                        }
                    else
                        {
                        // Complete failed dial request with error value
                        iMessageRouter->Complete(
                            EEtelCallDial,
                            &callData,
                            result );
                        }
                    }
                // No else

                if ( CALL_MODEM_CAUSE_FDN_NOT_OK == causeValue )
                    {
                    iFDNErrorAlredyNotified = EFalse;
                    }
                // No else
                }
            // No else
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmCallMessHandler::CallServiceDeniedInd. Switch serviceId case default.");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CALLSERVICEDENIEDIND, "CMmCallMessHandler::CallServiceDeniedInd. Switch serviceId case default." );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CheckCallIsaStatus
// Checks call Isa status
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CheckCallIsaStatus(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    TUint8 aCallStatusISA
    )
    {
TFLOGSTRING2("TSY: CMmCallMessHandler::CheckCallIsaStatus. callstatusisa:%d", aCallStatusISA);
OstTraceExt1( TRACE_NORMAL, CMMCALLMESSHANDLER_CHECKCALLISASTATUS, "CMmCallMessHandler::CheckCallIsaStatus;aCallStatusISA=%hhu", aCallStatusISA );

    // Check which status we got
    if ( CALL_MODEM_STATUS_MT_RELEASE == aCallStatusISA )
        {
        // Complete remote release event notification
        RMobileCall::TMobileCallEvent callEvent(
            RMobileCall::ERemoteTerminated );

        // Call Data Package
        CCallDataPackage callData;
        callData.SetCallIdAndMode(
            aMobileCallInfo.iCallId,
            aMobileCallInfo.iService );

        // Pack the data for sending to the manager
        callData.PackData( &callEvent );
        // Compete notification
        iMessageRouter->Complete(
            EMobileCallNotifyCallEvent,
            &callData,
            KErrNone );

        // Reset temporary call info
        if ( aMobileCallInfo.iCallId == iIncomingCallInfo.iCallId )
            {
            ResetIncomingCallInfo( iIncomingCallInfo );
            }
        // No else
        }
    else if ( CALL_MODEM_STATUS_IDLE == aCallStatusISA &&
            aMobileCallInfo.iCallId == iIncomingCallInfo.iCallId )
        {
        // Reset temporary call info
        ResetIncomingCallInfo( iIncomingCallInfo );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::ReadAllDestinationAddress
// Read all address destination
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::ReadAllAddressDestination(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    TBuf16<RMobilePhone::KMaxMobileTelNumberSize>& aTargetOrig,
    const TIsiReceiveC &aIsiMessage,
    TUint& aSbStartOffSet,
    TUint8 origPresentInd
    )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::ReadAllAddressDestination");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_READALLADDRESSDESTINATION, "CMmCallMessHandler::ReadAllAddressDestination" );

    TUint8 origAddressType( aIsiMessage.Get8bit(
        aSbStartOffSet + CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDRTYPE ) );


    // Get destinationAddress Length
    TUint8 destinationAddressLength( aIsiMessage.Get8bit(
        aSbStartOffSet + CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDRLEN ) );

    // Get address (telephone number)
    TPtrC8 address( aIsiMessage.GetData(
        aSbStartOffSet + CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDR,
        destinationAddressLength * 2 ) );

    TBuf8<RMobilePhone::KMaxMobileTelNumberSize * 2> address2;

    // Check if the prefix should be added to the dialled number.
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
        CALL_MODEM_SB_DESTINATION_PRE_ADDRESS,
        EIsiSubBlockTypeId8Len8,
        aSbStartOffSet ) )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::ReadAllAddressDestination: CALL_MODEM_SB_DESTINATION_PRE_ADDRESS");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_READALLADDRESSDESTINATION, "CMmCallMessHandler::ReadAllAddressDestination: CALL_MODEM_SB_DESTINATION_PRE_ADDRESS" );

        // Get destination address length
        TUint8 destinationPreAddressLength( aIsiMessage.Get8bit(
            aSbStartOffSet + CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_ADDRLEN ) );

        // Get address (telephone number)
        TPtrC8 preAddress( aIsiMessage.GetData(
            aSbStartOffSet + CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_ADDR,
            destinationPreAddressLength * 2 ) );

        if ( 0 < destinationPreAddressLength )
            {
            address2.Append( preAddress );
            }
        }

    // Check if number is international and addrDataLength is bigger
    // than zero. The length of the string does not have to be checked
    // since the original max length was 100. When the string was sent
    // to ISA side the '+' character was cut off and now we just put
    // it back. (three bits (5-7) contain type of number)
    if ( ( CALL_MODEM_NBR_TYPE_INTERNATIONAL ==
         ( origAddressType & KMaskBits5to7 ) )
         && ( 0 != address.Length() ) )
        {
        // Add '+' character back to the string
        address2.Append( KCallPadding ); // Padding
        address2.Append( '+' );
        }
    // No else

    address2.Append( address ); //append tel number to address2

    // Check if the postfix should be added to the dialled number.
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_STATUS_IND,
        CALL_MODEM_SB_DESTINATION_POST_ADDRESS,
        EIsiSubBlockTypeId8Len8,
        aSbStartOffSet ) )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::ReadAllAddressDestination: CALL_MODEM_SB_DESTINATION_POST_ADDRESS");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_READALLADDRESSDESTINATION, "CMmCallMessHandler::ReadAllAddressDestination: CALL_MODEM_SB_DESTINATION_POST_ADDRESS" );

        // Get destinationAddress Length
        TUint8 destinationPostAddressLength( aIsiMessage.Get8bit(
            aSbStartOffSet + CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_ADDRLEN ) );

        // Get address (telephone number)
        TPtrC8 postAddress( aIsiMessage.GetData(
            aSbStartOffSet + CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_ADDR,
            destinationPostAddressLength * 2 ) );

        if ( 0 < destinationPostAddressLength )
            {
            address2.Append( postAddress );
            }
        }

    if ( NULL != address.Length() )
        {
        // Get address type
        FillNumberPlanAndType( aMobileCallInfo, origAddressType );
        }
    // No else

    // Copy 8-bit address to the 16-bit target using correct endianess
    TIsiUtility::CopyFromBigEndian( address2, aTargetOrig );

TFLOGSTRING2("TSY: CMmCallMessHandler::ReadAllAddressDestination: aTargetOrig = %S", &aTargetOrig);
OstTraceExt1( TRACE_NORMAL, DUP19_CMMCALLMESSHANDLER_CallStatusIndL, "CMmCallMessHandler::ReadAllDestinationAddress;aTargetOrig=%S", aTargetOrig );
    FillRemoteStatusAndNumber(
        aMobileCallInfo,
        origPresentInd,
        aTargetOrig,
        RMobileCall::EMobileOriginated );

    aMobileCallInfo.iValid |=
        RMobileCall::KCallRemoteParty | RMobileCall::KCallDialledParty;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::ReadCallInfo
// Read All Address Destination
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::ReadCallInfo(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    TBuf16<RMobileCall::KCallingNameSize>& aTargetOrigName,
    const TIsiReceiveC &aIsiMessage,
    TUint& aSbStartOffSet
    )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::ReadCallInfo");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_READCALLINFO, "CMmCallMessHandler::ReadCallInfo" );

     // Get presentation indicator (four bits (1-4))
    TUint8 origInfoPresentInd( aIsiMessage.Get8bit(
        aSbStartOffSet + CALL_MODEM_SB_ORIGIN_INFO_OFFSET_PRESENTATION ) &
        KMaskBits5to7 );

    // Get call origin info name length
    TUint8 callOriginInfoNameLength( aIsiMessage.Get8bit(
        aSbStartOffSet + CALL_MODEM_SB_ORIGIN_INFO_OFFSET_NAMELEN ) );

    // Get name
    TPtrC8 name( aIsiMessage.GetData(
        aSbStartOffSet + CALL_MODEM_SB_ORIGIN_INFO_OFFSET_NAME,
        callOriginInfoNameLength * 2 ) );

    // Copy 8-bit name to the 16-bit target using correct endianess
    TIsiUtility::CopyFromBigEndian( name, aTargetOrigName );

    // Fill calling name
    FillCallingName(
        aMobileCallInfo,
        origInfoPresentInd,
        aTargetOrigName );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CompleteIncomingCallNotif
// Complete incoming Call Notification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CompleteIncomingCallNotif(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    RMobileCall::TMobileCallStatus& aStatusETel
    )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CompleteIncomingCallNotif");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_COMPLETEINCOMINGCALLNOTIF, "CMmCallMessHandler::CompleteIncomingCallNotif" );

    if ( RMobileCall::EStatusRinging == aStatusETel )
        {
        // If call has been waiting, it is already notified
        // (see error item MEKK-5ZMKHG).
        if ( iWaitingCall != aMobileCallInfo.iCallId )
            {
            // Call data package
            CCallDataPackage callData;
            callData.SetCallIdAndMode(
                aMobileCallInfo.iCallId,
                aMobileCallInfo.iService );

            // Pack mobile call info
            callData.PackData( &aMobileCallInfo );

            // Complete IncomingCall notification
TFLOGSTRING("TSY: CMmCallMessHandler::CompleteIncomingCallNotif - iWaitingCall != aMobileCallInfo.iCallId");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_COMPLETEINCOMINGCALLNOTIF, "CMmCallMessHandler::CompleteIncomingCallNotif - iWaitingCall != aMobileCallInfo.iCallId" );

            iMessageRouter->Complete(
                EEtelLineNotifyIncomingCall,
                &callData,
                KErrNone );

            // If UUS data found complete Receive UUI.
            // Receive UUI is always completed after notify incoming call.
            if ( 0 < iReceivedUusData.Length() )
                {
TFLOGSTRING("TSY: CMmCallMessHandler::CompleteIncomingCallNotif - Completing receive uui");
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_COMPLETEINCOMINGCALLNOTIF, "CMmCallMessHandler::CompleteIncomingCallNotif - Completing receive uui" );

                callData.PackData( &iReceivedUusData );
                iMessageRouter->Complete(
                    EMobileCallReceiveUUI,
                    &callData,
                    KErrNone );

                iReceivedUusData.Zero();
                }
            }
        // No else

        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange
// Complete Mobile Call Info and Status Change
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    RMobileCall::TMobileCallStatus& aStatusETel,
    TInt aResultETel )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::CompleteMobileCallInfoChange");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_COMPLETEMOBILECALLINFOANDSTATUSCHANGE, "CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange" );

TFLOGSTRING3("TSY: CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange;aStatusETel=%d;aResultETel=%d", aStatusETel, aResultETel);
TFLOGSTRING2("TSY: CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange;aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber=%S", &aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber);
TFLOGSTRING2("TSY: CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange;aMobileCallInfo.iValid=%x", aMobileCallInfo.iValid);
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_COMPLETEMOBILECALLINFOANDSTATUSCHANGE, "CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange;aStatusETel=%d;aResultETel=%d", aStatusETel, aResultETel );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_COMPLETEMOBILECALLINFOANDSTATUSCHANGE, "CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange;aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber=%S", aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber );
OstTrace1( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_COMPLETEMOBILECALLINFOANDSTATUSCHANGE, "CMmCallMessHandler::CompleteMobileCallInfoAndStatusChange;aMobileCallInfo.iValid=%x", aMobileCallInfo.iValid );

    // Call data package
    CCallDataPackage callData;
    callData.SetCallIdAndMode(
        aMobileCallInfo.iCallId,
        aMobileCallInfo.iService );

    // COMPLETE MOBILE CALL INFO CHANGE
    // (Get mobile call information is always completed
    // before call status change)
    callData.PackData( &aMobileCallInfo );
    iMessageRouter->Complete(
        EMobileCallGetMobileCallInfo,
        &callData,
        KErrNone );

    // COMPLETE MOBILE CALL STATUS CHANGE
    callData.PackData( &aStatusETel );
    iMessageRouter->Complete(
        EMobileCallNotifyMobileCallStatusChange,
        &callData,
        aResultETel );
    }
// -----------------------------------------------------------------------------
// CMmCallMessHandler::HandleError
// Handles CMmCallMessHandler's errors that comes
// via PhoNetReceiver RunError method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::HandleError(
    const TIsiReceiveC& /*aIsiMessage*/,
    TInt /*aError*/ )
    {
    // None
TFLOGSTRING("TSY: CMmCallMessHandler::HandleError.");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_HANDLEERROR, "CMmCallMessHandler::HandleError" );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::FillCauseOfNoCli
// This method maps the Domestic OS cause of no CLI (Calling Line ID) to the
// Symbian OS remote information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::FillCauseOfNoCli(
    RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    TUint8 aCauseOfNoCli )
    {
TFLOGSTRING2("TSY: CMmCallMessHandler::FillCauseOfNoCli. cause:%d", aCauseOfNoCli);
OstTraceExt1( TRACE_NORMAL, CMMCALLMESSHANDLER_FILLCAUSEOFNOCLI, "CMmCallMessHandler::FillCauseOfNoCli;aCauseOfNoCli=%hhu", aCauseOfNoCli );

    switch( aCauseOfNoCli )
        {
        // Cause of No CLI is "reject by user"
        case CALL_MODEM_CLI_REJECT_BY_USER:
            {
            aMobileCallInfo.iRemoteParty.iRemoteIdStatus
                = RMobileCall::ERemoteIdentityAvailableNoCliRejectedByUser;
            break;
            }
        // Cause of No CLI is "interaction with other service"
        case CALL_MODEM_CLI_INTERACTION:
            {
            if (RMobileCall::ERemoteIdentityAvailable ==
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus )
                {
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus
                  = RMobileCall::
                    ERemoteIdentityAvailableNoCliInteractionWithOtherService;
                }
            else
                {
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus
                 = RMobileCall::
                   ERemoteIdentityUnavailableNoCliInteractionWithOtherService;
                }
            break;
            }
        // Cause of No CLI is "Coin Line/ Payphone"
        case CALL_MODEM_CLI_COIN_LINE_PAYPHONE:
            {
            if (RMobileCall::ERemoteIdentityAvailable ==
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus )
                {
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus
                  = RMobileCall::ERemoteIdentityAvailableNoCliCoinOrPayphone;
                }
            else
                {
                aMobileCallInfo.iRemoteParty.iRemoteIdStatus
                 = RMobileCall::ERemoteIdentityUnavailableNoCliCoinOrPayphone;
                }
            break;
            }
        // Cause of No CLI is "unavailable"
        case CALL_MODEM_CLI_UNAVAILABLE:
            {
            aMobileCallInfo.iRemoteParty.iRemoteIdStatus
                = RMobileCall::ERemoteIdentityAvailableNoCliUnavailable;
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmCallMessHandler::FillCauseOfNoCli. Switch cause case default");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_FILLCAUSEOFNOCLI, "CMmCallMessHandler::FillCauseOfNoCli. Switch cause case default" );
            // Keep aMobileCallInfo.iRemoteParty.iRemoteIdStatus unchanged
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::InitializeDataportL
// Initializes dataport if it doesn't exist already
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::InitializeDataportL()
    {
TFLOGSTRING("NTSY: CMmCallMessHandler::InitializeDataportL()");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_INITIALIZEDATAPORTL, "CMmCallMessHandler::InitializeDataportL" );

    // SIM is ready, start dataport handling
    if ( !iDataPortHandler )
        {
TFLOGSTRING("NTSY: CMmCallMessHandler::InitializeDataportL - Start dataport handling");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_INITIALIZEDATAPORTL, "CMmCallMessHandler::InitializeDataportL - Start dataport handling" );
        // Deleted in CMmCallMessHandler::~CMmCallMessHandler()
        iDataPortHandler = CMmDataPortHandler::NewL( iMessageRouter );
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::ActivateUUS()
// Copying received data to the member variable which means UUS is activated
// -----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::ActivateUUS(
    const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::ActivateUUS. Unpacking UUS data." );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_ACTIVATEUUS, "CMmCallMessHandler::ActivateUUS" );

    TInt ret( KErrNone );

    // Reset obsolated data... just in case
    iUusData.iUUI.Zero();
    iUusData.iServiceReq = 0;

    // Unpack data
    aDataPackage->UnPackData( iUusData );

    if ( RMobileCall::KUUS1Implicit != iUusData.iServiceReq )
        {
TFLOGSTRING2( "TSY: CMmCallMessHandler::ActivateUUS. Service type is not supported (%d)!", iUusData.iServiceReq );
OstTrace1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_ACTIVATEUUS, "CMmCallMessHandler::ActivateUUS;Service type is not supported (%d)", iUusData.iServiceReq );
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::InfoPpDataReadReq
// Creates Product Profile request, in order to read if HSDPA is disabled in PP.
// status.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::InfoPpDataReadReq()
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::InfoPpDataReadReq" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_INFOPPDATAREADREQ, "CMmCallMessHandler::InfoPpDataReadReq" );

    // Create INFO_SB_PP_DATA subblock with only one PP feature (12 bytes)
    TBuf8<12> infoSbPpBuf;
    TIsiSubBlock infoSbPp(
        infoSbPpBuf,
        INFO_SB_PP_DATA,
        EIsiSubBlockTypeId8Len16 );
    infoSbPpBuf.Append( 0 ); // Customer ID
    infoSbPpBuf.Append( KCallPadding ); // Filler

    // Append PP feature amount (1) as 16-bit value (high byte / low byte)
    infoSbPpBuf.Append( 0 );
    infoSbPpBuf.Append( 1 );

    // Append PP feature number (INFO_PP_WCDMA_HS_CHANNEL_SUPPORT) as 16-bit
    // value (high byte / low byte)
    infoSbPpBuf.Append( static_cast<TUint8>(
        INFO_PP_WCDMA_HS_CHANNEL_SUPPORT >> KShiftByOneByte ) );
    infoSbPpBuf.Append( static_cast<TUint8>(
        INFO_PP_WCDMA_HS_CHANNEL_SUPPORT ) );

    // Append PP feature value (empty) as 16-bit value (high byte / low byte)
    infoSbPpBuf.Append( 0 );
    infoSbPpBuf.Append( 0 );

    // Create INFO_PP_READ_REQ message:
    // batch mode (1) + number of SB (1) + INFO_SB_PP_DATA sub block with
    // only one PP feature (12)
    TBuf8<14> infoPpReadReq;
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    infoPpReadReq.Append( INFO_PP_PROD_PROFILE_FEATURE ); // No batch reading
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    infoPpReadReq.Append( INFO_PP_FEATURE ); // No batch reading
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    infoPpReadReq.Append( 1 ); // One sub block
    infoPpReadReq.Append( infoSbPp.CompleteSubBlock() );

    // Ignore return value
    iPhoNetSender->Send(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        PN_INFO,
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        PN_MODEM_INFO,
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        KHsdpaPpTransId,
        INFO_PP_DATA_READ_REQ,
        infoPpReadReq );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::InfoPpDataReadResp
// Breaks received InfoPpDataReadResp ISI message.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::InfoPpDataReadResp(
    const TIsiReceiveC &aIsiMessage )
    {
    TUint8 status = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + INFO_PP_DATA_READ_RESP_OFFSET_STATUS );
TFLOGSTRING2( "TSY: CMmCallMessHandler::InfoPpDataReadResp, status=%d.", status );
OstTraceExt1( TRACE_NORMAL, CMMCALLMESSHANDLER_INFOPPDATAREADRESP, "CMmCallMessHandler::InfoPpDataReadResp;status=%hhu", status );

    // iHSDPADisabledInPp is EFalse by default, change only if PP bit
    // is set to "HSDPA_HSUPA_DISABLED".
    if ( INFO_OK == status )
        {
        TUint sbStartOffset( 0 );
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_INFO_PP_DATA_READ_RESP,
            INFO_SB_PP_DATA,
            EIsiSubBlockTypeId8Len16,
            sbStartOffset ) )
            {
            TUint32 ppData( aIsiMessage.Get32bit(
                sbStartOffset + INFO_SB_PP_DATA_OFFSET_PPFEAVAL ) );
            TUint16 ppFeatureValue( ppData & 0x0000FFFF );
            if ( INFO_PP_WCDMA_HS_CHANNEL_SUPPORT_HSDPA_HSUPA_DISABLED
                    == ppFeatureValue )
                {
                iHSDPADisabledInPp = ETrue;
TFLOGSTRING( "TSY: CMmCallMessHandler::InfoPpDataReadResp: HSDPA is disabled in PP." );
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_INFOPPDATAREADRESP, "CMmCallMessHandler::InfoPpDataReadResp: HSDPA is disabled in PP" );
                } // No else, ignore other values
            } // No else, ignore missing sub block
        } // No else, ignore failed reading
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::GpdsContextsClearReq
// Creates GPDS_CONTEXTS_CLEAR request, in order to end all data connections.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::GpdsContextsClearReq()
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::GpdsContextsClearReq" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_GPDSCONTEXTSCLEARREQ, "CMmCallMessHandler::GpdsContextsClearReq" );

    // Start emergency timer
    iEmergencyTimer.StartEmergencyTimer();

    // Ignore return value
    iPhoNetSender->Send(
        PN_GPDS,
        KCallTransId,
        GPDS_CONTEXTS_CLEAR_REQ,
        KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::GpdsContextsClearResp
// Breaks received GPDS_CONTEXTS_CLEAR_RESP ISI message, and proceeds with
// emergency call dialling.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::GpdsContextsClearResp()
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::GpdsContextsClearResp" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_GPDSCONTEXTSCLEARRESP, "CMmCallMessHandler::GpdsContextsClearResp" );

    // Stop emergency timer
    iEmergencyTimer.StopEmergencyTimer();

    // When GPDS_CONTEXTS_CLEAR_RESP response comes, all data contexts have been
    // shut down. Emergency call can now be set up.
    if ( iEmergCallDelayed )
        {
TFLOGSTRING( "TSY: CMmCallMessHandler::GpdsContextsClearResp - HSDPA workaround. Emergency call is created now." );
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_GPDSCONTEXTSCLEARRESP, "CMmCallMessHandler::GpdsContextsClearResp - HSDPA workaround. Emergency call is created now" );

        TIsiSend callReq( iPhoNetSender->SendBufferDes() );
        TUint8 dosCallId( CALL_MODEM_ID_NONE );
        // In the case when emergency number checking is needed to be ignored ¨
        // the destination address is delivered
        EmergencyCallCreateReq( &callReq );
        // Emergency calls are created immediately, without
        // setting properties
        TUint8 messageId( CALL_MODEM_CREATE_REQ );
        TUint8 isiCallId( CALL_MODEM_CREATE_REQ_OFFSET_CALLID );

        // Create ISI message
        // Recource ID
        callReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_CALL );
        // Transaction ID
        callReq.Set8bit( ISI_HEADER_OFFSET_TRANSID, KCallTransId );
        // Message ID
        callReq.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, messageId );
        // Call ID
        callReq.Set8bit( ISI_HEADER_SIZE + isiCallId, dosCallId );

        // Ignore return value
        iPhoNetSender->Send( callReq.Complete() );

        iEmergCallDelayed = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::GetCSDCallControlReqSubBlock
// This method constructs a subblock for the CSD call request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::GetCSDCallControlReqSubBlock(
    const TDesC16& aTelNumber,
    RMobileCall::TMobileCallIdRestriction aIdRestrict,
    TIsiSend& aCsdIsiMsg,
    TUint8& aNumOfSbInMsg,
    TInt& aCurrentMsgOffset )
    {

TFLOGSTRING("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock" );

    iDestPostAddressIncluded = EFalse;

    // Get length of the given number
    TInt telNumLength( aTelNumber.Length() );
    TUint8 presentationInd( CSD_GSM_PRESENTATION_DEFAULT );

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock - aIdRestrict: %d", aIdRestrict);
OstTrace1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock;aIdRestrict=%d", aIdRestrict );

    if ( KTelephonyNumberMaxLength < telNumLength )
        {
        telNumLength = KTelephonyNumberMaxLength;
        }

    TBool internationalAddr( EFalse );
    TBool postAddressSupported( EFalse );
    TBool preAddressSupported( EFalse );
    TInt preAddressLength( 0 );
    TInt destAddressStarts( 0 );
    TInt postAddressStarts( 0 );

    // Destination address max length can be KTelephonyNumberMaxLength
    TBuf16<KTelephonyNumberMaxLength> destAddress;

    // Check prefix address
    if ( 0 < telNumLength &&
        ( '*' == ( aTelNumber )[0] || '#' == ( aTelNumber )[0] ) )
        {
        for ( TInt i ( 1 ); i < telNumLength; i++ )
            {
            if ( 'p' == ( aTelNumber )[i] )
                {
                // Post address starts
                break; // Exit for loop
                }
            else if ( '#' == ( aTelNumber )[i] )
                {
                preAddressLength = i + 1;
                }
            // No else
            }
        }
    // No else

    // Check if there is calling line identification restriction prefix in
    // the telephony number. Destination pre address subblock will be
    // created only for *31# and #31# commands.
    if ( KPreAddressLength == preAddressLength &&
         '3' == ( aTelNumber )[1] &&
         '1' == ( aTelNumber )[2] &&
         '#' == ( aTelNumber )[3] )
        {
        preAddressSupported = ETrue;
        destAddressStarts = KPreAddressLength;

        // Set presentationInd based on prefix
        if ( '*' == ( aTelNumber )[0] )
            {
            presentationInd = CSD_PRESENTATION_ALLOWED;
TFLOGSTRING("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by PREFIX = CSD_PRESENTATION_ALLOWED");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by PREFIX = CSD_PRESENTATION_ALLOWED" );
            }
        else if ( '#' == ( aTelNumber )[0] )
            {
            presentationInd = CSD_PRESENTATION_RESTRICTED;
TFLOGSTRING("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by PREFIX = CSD_PRESENTATION_RESTRICTED");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by PREFIX = CSD_PRESENTATION_RESTRICTED" );
            }
        }
    else
        {
        // Set presentationInd based on aIdRestrict
        switch ( aIdRestrict )
            {
            case RMobileCall::ESendMyId:
                {
                presentationInd = CSD_PRESENTATION_ALLOWED; //0x00
TFLOGSTRING("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by aIdRestrict = CSD_PRESENTATION_ALLOWED");
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by aIdRestrict = CSD_PRESENTATION_ALLOWED" );
                break;
                }
            case RMobileCall::EDontSendMyId:
                {
                presentationInd = CSD_PRESENTATION_RESTRICTED; //0x01
TFLOGSTRING("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by aIdRestrict = CSD_PRESENTATION_RESTRICTED");
OstTrace0( TRACE_NORMAL, DUP5_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by aIdRestrict = CSD_PRESENTATION_RESTRICTED" );
                break;
                }
            case RMobileCall::EIdRestrictDefault:
            default:
                {
                presentationInd = CSD_GSM_PRESENTATION_DEFAULT; //0x07
TFLOGSTRING("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by aIdRestrict = CSD_GSM_PRESENTATION_DEFAULT");
OstTrace0( TRACE_NORMAL, DUP6_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock -- presentationInd by aIdRestrict = CSD_GSM_PRESENTATION_DEFAULT" );
                break;
                }
            }
        }

    // Check if there is post address
    for ( TInt i( destAddressStarts ); i < telNumLength; i++ )
        {
        if ( 'p' == ( aTelNumber )[i] && preAddressLength <= i )
            {
            // Destination address ends and post address starts.
            postAddressSupported = ETrue;
            postAddressStarts = i;
            break; // Exit for loop
            }
        else if ( '+' == ( aTelNumber )[i] && preAddressLength == i )
            {
            // Destination address part contains "+" character. For example
            // +35850123456 or *140#+35850123456. Don't add "+" char to the
            // destination address.
            internationalAddr = ETrue;
            }
        else
            {
            destAddress.Append( ( TUint16 )( aTelNumber )[i] );
            }
        }

    // Create destination address subblock. Size of data buffer
    // "max length of destination address" + "message data"
    TBuf8<SIZE_CSD_DESTINATION_ADDRESS + KAddressSubBlockMaxLength>
        destinationAddressBuf;
    TIsiSubBlock destinationAddressSb(
        destinationAddressBuf,
        CSD_DESTINATION_ADDRESS,
        EIsiSubBlockTypeId8Len8 );

    // Address type
    if ( internationalAddr )
        {
        destinationAddressBuf.Append(
            CSD_NBR_TYPE_INTERNATIONAL | CSD_NBR_PLAN_ISDN_TELEPHONY );
        }
    else
        {
        destinationAddressBuf.Append( CSD_NBR_PLAN_ISDN_TELEPHONY );
        }

    // Presentation indicator
    destinationAddressBuf.Append( presentationInd | CSD_USER_SCREENED_PASSED );
    // Padding byte
    destinationAddressBuf.Append( KCallPadding ); // Padding
    // Address length
    destinationAddressBuf.Append( destAddress.Length() );

    // Add the number string
    TIsiUtility::CopyToBigEndian( destAddress, destinationAddressBuf );

    aCsdIsiMsg.CopyData(
        aCurrentMsgOffset, destinationAddressSb.CompleteSubBlock() );

    // Set new offset and increase subblock count
    aCurrentMsgOffset =
        aCurrentMsgOffset + destinationAddressBuf.Length();
    aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_DESTINATION_ADDRESS - Subblock Count:  %d", aNumOfSbInMsg );
OstTrace1( TRACE_NORMAL, DUP10_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_DESTINATION_ADDRESS - Subblock Count: %d", aNumOfSbInMsg );

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock - destAddress: %S", &destAddress);
OstTraceExt2( TRACE_NORMAL, DUP11_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock;destAddress=%d;iDataRateCode=%S", destAddress, iDataRateCode );

    if ( preAddressSupported )
        {
        // Pre address length can be KTelephonyNumberMaxLength
        TBuf16<KTelephonyNumberMaxLength> preAddress;

        // Create pre address subblock
        TBuf8<SIZE_CSD_DESTINATION_PRE_ADDRESS +
            KTelephonyNumberMaxLength * 2> preAddressBuf;

        TIsiSubBlock preAddressSb(
            preAddressBuf,
            CSD_DESTINATION_PRE_ADDRESS,
            EIsiSubBlockTypeId8Len8 );

        preAddressBuf.Append( CSD_P_ADDR_PRESENTATION_ALLOWED );
        preAddressBuf.Append( preAddressLength );
        for ( TInt i ( 0 ); i < preAddressLength; i++ )
            {
            preAddress.Append( ( TUint16 )( aTelNumber )[i] );
            }

        TIsiUtility::CopyToBigEndian( preAddress, preAddressBuf );

        aCsdIsiMsg.CopyData(
            aCurrentMsgOffset,
            preAddressSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + preAddressBuf.Length();
        aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_DESTINATION_PRE_ADDRESS - Subblock Count:  %d", aNumOfSbInMsg );
OstTrace1( TRACE_NORMAL, DUP12_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_DESTINATION_PRE_ADDRESS - Subblock Count: %d", aNumOfSbInMsg );
        }
    // No else

    if ( postAddressSupported )
        {
        // Creates Post Address subblock
        TInt postAddLength( telNumLength - postAddressStarts );

        // Post address length can be KTelephonyNumberMaxLength
        TBuf16<KTelephonyNumberMaxLength> postAddress;

        // Size of data buffer is "max post address length" + "message data"
        TBuf8<SIZE_CSD_DESTINATION_POST_ADDRESS +
            KTelephonyNumberMaxLength * 2> postAddressBuf;
        TIsiSubBlock postAddressSb(
            postAddressBuf,
            CSD_DESTINATION_POST_ADDRESS,
            EIsiSubBlockTypeId8Len8 );

        postAddressBuf.Append( CSD_P_ADDR_PRESENTATION_ALLOWED );
        postAddressBuf.Append( ( TUint8 )postAddLength );

        for ( TInt i( postAddressStarts ); i < telNumLength; i++ )
            {
            postAddress.Append( ( TUint16 )( aTelNumber )[i] );
            }

        TIsiUtility::CopyToBigEndian( postAddress, postAddressBuf );

        aCsdIsiMsg.CopyData(
            aCurrentMsgOffset,
            postAddressSb.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + postAddressBuf.Length();
        aNumOfSbInMsg++;

        // Destination post address included
        iDestPostAddressIncluded = ETrue;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_DESTINATION_POST_ADDRESS - Subblock Count:  %d", aNumOfSbInMsg );
OstTrace1( TRACE_NORMAL, DUP13_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_DESTINATION_POST_ADDRESS - Subblock Count: %d", aNumOfSbInMsg );
    }
    // No else

    // Create Cbst_Params subblock.
    TBuf8<SIZE_CSD_CBST_PARAMS>  CbstParamsBuf;
    TIsiSubBlock CbstParamsSb(
        CbstParamsBuf,
        CSD_CBST_PARAMS,
        EIsiSubBlockTypeId8Len8 );

    // Data Rate parameter of CBST
    CbstParamsBuf.Append( iDataRateCode );
    // Bearer Service Name
    CbstParamsBuf.Append( 0x01 );  // 1 data circuit synchronous
    // Connection Element
    CbstParamsBuf.Append( 0x00 );  // transparent
    // Paddings
    CbstParamsBuf.Append( KCallPadding );
    CbstParamsBuf.Append( KCallPadding );
    CbstParamsBuf.Append( KCallPadding );

    aCsdIsiMsg.CopyData(
        aCurrentMsgOffset, CbstParamsSb.CompleteSubBlock() );

    // Set new offset and increase subblock count
    aCurrentMsgOffset =
        aCurrentMsgOffset + CbstParamsBuf.Length();
    aNumOfSbInMsg++;

TFLOGSTRING2("TSY: CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_CBST_PARAMS - Subblock Count:  %d", aNumOfSbInMsg );
OstTrace1( TRACE_NORMAL, DUP14_CMMCALLMESSHANDLER_GETCSDCALLCONTROLREQSUBBLOCK, "CMmCallMessHandler::GetCSDCallControlReqSubBlock - CSD_CBST_PARAMS - Subblock Count: %d", aNumOfSbInMsg );

    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::csdCallControlReq
// This method constructs CSD call control message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::CsdCallControlReq( TUint8 aCallOperId )
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::CsdCallControlReq" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CSDCALLCONTROLREQ, "CMmCallMessHandler::CsdCallControlReq" );

    TBuf8<SIZE_CSD_CALL_CONTROL_REQ> data( 0 );

    // Transaction ID
    data.Append( KCallTransId );

    // Message ID
    data.Append( CSD_CALL_CONTROL_REQ );

    // Call Operation ID
    data.Append( aCallOperId );

    // Create CSD ISI message
    TIsiSend csdIsiMsg( iPhoNetSender->SendBufferDes() );
    csdIsiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_CSD );
    csdIsiMsg.CopyData( ISI_HEADER_SIZE, data );

    // Number of Subblocks
    TUint8 numOfSbInMessage( KZero );

    // The CSD Server to create a call
    // Mandatory subblocks CSD_DESTINATION_ADDRESS and CSD_CBST_PARAMS
    if ( CSD_CALL_CREATE == aCallOperId )
        {
        // Initialize message offset
        TInt currentMsgOffset( ISI_HEADER_SIZE + SIZE_CSD_CALL_CONTROL_REQ );

        // Create subblocks for CsdCallControlReq
        GetCSDCallControlReqSubBlock(
            dynamic_cast<TDesC16&>( iTelNumber ),
            RMobileCall::ESendMyId,
            csdIsiMsg,
            numOfSbInMessage,
            currentMsgOffset );
        }

    // Set number of subblocks in CSD_CALL_CONTROL_REQ message
    csdIsiMsg.Set8bit(
        ISI_HEADER_SIZE + CSD_CALL_CONTROL_REQ_OFFSET_SUBBLOCKS,
        numOfSbInMessage );

    // Message sent via Phonet
    return iPhoNetSender->Send( csdIsiMsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CsdCallControlResp
// Breaks received CSD_CALL_CONTROL_RESP ISI message.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CsdCallControlResp( const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::CsdCallControlResp" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CSDCALLCONTROLRESP, "CMmCallMessHandler::CsdCallControlResp" );

    // Get cause
    TUint8 causeValue( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CSD_CALL_CONTROL_RESP_OFFSET_CAUSE ) );

TFLOGSTRING2( "TSY: CMmCallMessHandler::CsdCallControlResp. Cause value: %d", causeValue );
OstTrace1( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_CSDCALLCONTROLRESP, "CMmCallMessHandler::CsdCallControlResp;causeValue=%d", causeValue );

    TUint8 oper_Id( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CSD_CALL_CONTROL_RESP_OFFSET_OPERID ) );

TFLOGSTRING2( "TSY: CMmCallMessHandler::CsdCallControlResp. Call operation id: %d", oper_Id );
OstTrace1( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_CSDCALLCONTROLRESP, "CMmCallMessHandler::CsdCallControlResp;oper_Id=%d", oper_Id );

    TInt result( KErrNone );

    // create call data package
    CCallDataPackage callDataPackage;

    if( CSD_CALL_CAUSE_NONE != causeValue )
        {
        result = CMmStaticUtility::CSCauseToEpocError( PN_CSD, 0x00, causeValue );

        //Remove Pipe for wideo telephony
        PnsPipeRemoveReq();

        // set call id and mode
        if ( iMobileCallInfo.iCallId < 0 )
            {
            // call id not defined, complete with id 0
            // =>special cases in CMmMessageRouterBase::RouteCompletionL
            callDataPackage.SetCallIdAndMode(
                0, iMobileCallInfo.iService );
            }
        else
            {
            // normal completion
            callDataPackage.SetCallIdAndMode(
                    iMobileCallInfo.iCallId, iMobileCallInfo.iService );
            }

        switch( oper_Id )
            {
            case CSD_CALL_CREATE:
            // Csd call set up, user init and dial are sent in first phase
            // of data call creation attempt.
                {
                // complete dial request, with error
                iMessageRouter->Complete(
                        EEtelCallDial,
                        &callDataPackage,
                        result );
                break;
                }
            case CSD_CALL_ANSWER:
            // Answer and autoanswer are sent in answering to incoming call
                {
                // complete answer request, with error
                iMessageRouter->Complete(
                        EEtelCallAnswer,
                        &callDataPackage,
                        result );
                break;
                }
            case CSD_CALL_RELEASE:
                {
                // Complete failed HangUp request
                iMessageRouter->Complete(
                        EEtelCallHangUp,
                        &callDataPackage,
                        result );
                break;
                }
            case CSD_CALL_REJECT:
            default:
                {
TFLOGSTRING("TSY: CMmCallMessHandler::CsdCallControlResp. Switch oper_Id case default.");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CSDCALLCONTROLRESP, "CMmCallMessHandler::CsdCallControlResp, Switch oper_Id case default" );
                break;
                }
            }
        }
    else
        {
        if ( CSD_CALL_RELEASE == oper_Id )
            {
            //Remove Pipe for wideo telephony
            PnsPipeRemoveReq();
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CsdVideoCallStatusInd
// Breaks received CSD_VIDEO_CALL_STATUS_IND ISI message.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CsdVideoCallStatusInd( const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::CsdVideoCallStatusInd" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CSDVIDEOCALLSTATUSIND, "CMmCallMessHandler::CsdVideoCallStatusInd" );

    iVideoCallStatus = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CSD_VIDEO_CALL_STATUS_IND_OFFSET_STATUS );

    // CSD_VIDEO_CALL_STATUS_CONNECT
    // Video call connected
    if( CSD_VIDEO_CALL_STATUS_CONNECT == iVideoCallStatus )
        {
        CCallDataPackage callData;
        // set call id and mode
        callData.SetCallIdAndMode(
            iMobileCallInfo.iCallId, iMobileCallInfo.iService );
        if ( RMobileCall::EMobileOriginated == iCallDirection )
            {
TFLOGSTRING("TSY: CMmCallMessHandler::CsdVideoCallStatusInd: MO call connected");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_CSDVIDEOCALLSTATUSIND, "CMmCallMessHandler::CsdVideoCallStatusInd: MO call connected" );
            iMessageRouter->Complete(
                EEtelCallDial, &callData, KErrNone );
            }
        else if ( RMobileCall::EMobileTerminated == iCallDirection )
            {
TFLOGSTRING("TSY: CMmCallMessHandler::CsdVideoCallStatusInd: MT call connected");
OstTrace0( TRACE_NORMAL, DUP3_CMMCALLMESSHANDLER_CSDVIDEOCALLSTATUSIND, "CMmCallMessHandler::CsdVideoCallStatusInd: MT call connected" );
            iMessageRouter->Complete(
                EEtelCallAnswer, &callData, KErrNone );
            }
        }
    else
        {
        // MT video call case we have to remove pipe
        if ( iVideoCallMtReleased )
            {
TFLOGSTRING("TSY: CMmCallMessHandler::CsdVideoCallStatusInd: call MT releated");
OstTrace0( TRACE_NORMAL, DUP4_CMMCALLMESSHANDLER_CSDVIDEOCALLSTATUSIND, "CMmCallMessHandler::CsdVideoCallStatusInd: call MT released" );
            //Remove Pipe for wideo telephony
            PnsPipeRemoveReq();
            iVideoCallMtReleased = EFalse;
            }
        }
    // CSD_VIDEO_CALL_STATUS_DISCONNECT arrives also when call establishment
    // fails
    // reset call direction to avoid further unnecessary IPC completions
    iCallDirection = RMobileCall::EDirectionUnknown;

TFLOGSTRING2( "TSY: CMmCallMessHandler::CsdVideoCallStatusInd. VideoCallStatus: %d", iVideoCallStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CSDVIDEOCALLSTATUSIND, "CMmCallMessHandler::CsdVideoCallStatusInd. VideoCallStatus: %d", iVideoCallStatus );
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::CsdMultimediaDataRateInd
// Breaks received CSD_MULTIMEDIA_DATA_RATE_IND ISI message.
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::CsdMultimediaDataRateInd( const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING( "TSY: CMmCallMessHandler::CsdMultimediaDataRateInd" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_CSDMULTIMEDIADATARATEIND, "CMmCallMessHandler::CsdMultimediaDataRateInd" );

    // Current maximum bearer rate for multimedia call in bits/s
    TUint dataRate( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CSD_MULTIMEDIA_DATA_RATE_IND_OFFSET_RATE ) );

TFLOGSTRING2( "TSY: CMmCallMessHandler::CsdMultimediaDataRateInd. dataRate: %d", dataRate);
OstTrace1( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_CSDMULTIMEDIADATARATEIND, "CMmCallMessHandler::CsdMultimediaDataRateInd. dataRate: %d", dataRate);

    // Match the bearer speed
    switch ( dataRate )
        {
        case KBps28800:
            {
            iDataRateCode = KRateCode28800;
            break;
            }
        case KBps32000:
            {
            iDataRateCode = KRateCode32000;
            break;
            }
        case KBps33600:
            {
            iDataRateCode = KRateCode33600;
            break;
            }
        case KBps56000:
            {
            iDataRateCode = KRateCode56000;
            break;
            }
        case KBps64000:
            {
            iDataRateCode = KRateCode64000;
            break;
            }
        default:
            {
            iDataRateCode = KRateCodeAuto;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SetMobileCallinfo
// This method sets GSM specific mobile call information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCallMessHandler::SetMobileCallInfo(
    const RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
    const TBool aIsMultimedia,
    const TBool aIsWaitingCall )
    {

TFLOGSTRING3("TSY: CMmCallMessHandler::SetMobileCallInfo. Multimedia:%d, CallWaiting:%d", aIsMultimedia, aIsWaitingCall );
OstTraceExt2( TRACE_NORMAL, CMMCALLMESSHANDLER_SETMOBILECALLINFO, "CMmCallMessHandler::SetMobileCallInfo;aIsMultimedia=%d;aIsWaitingCall=%d", aIsMultimedia, aIsWaitingCall );

TFLOGSTRING("TSY: CMmCallMessHandler::SetMobileCallInfo. iStatus != RMobileCall::EStatusIdle");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_SETMOBILECALLINFO, "CMmCallMessHandler::SetMobileCallInfo, iStatus != RMobileCall::EStatusIdle" );

    // Is call multimedia/video
    iIsMultimedia = aIsMultimedia;
    // Is this a waiting call
    iIsWaitingCall = aIsWaitingCall;

    if ( iIsWaitingCall )
        {
        // We are receicing MT call. Set call direction here,
        // because +CRING can be missed when answering waiting call
        // (e.g. when replace is selected from UI)
        iCallDirection = RMobileCall::EMobileTerminated;
        iMobileCallInfo.iRemoteParty.iDirection = iCallDirection;
        }
    //no else

    // call id should be always available
    iMobileCallInfo.iCallId = aMobileCallInfo.iCallId;

    // call mode should be always available
    iMobileCallInfo.iService = aMobileCallInfo.iService;

    // call status should be always available
    iMobileCallInfo.iStatus = aMobileCallInfo.iStatus;

    // is this an emergency call
    if ( RMobileCall::KCallEmergency & aMobileCallInfo.iValid )
        {
        iMobileCallInfo.iEmergency = aMobileCallInfo.iEmergency;
        }
    //no else

    if ( 0 < aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Length() &&
        ( RMobilePhone::KMaxMobileTelNumberSize >=
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Length() ) )
        {
        iMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Zero();
        iMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Copy(
            aMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber );

        iMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan
            = aMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan;
        iMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber
            = aMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber;

        iMobileCallInfo.iRemoteParty.iRemoteIdStatus
            = aMobileCallInfo.iRemoteParty.iRemoteIdStatus;
        }
    //no else

    if ( RMobileCall::KCallRemoteParty & aMobileCallInfo.iValid )
        {
        iMobileCallInfo.iRemoteParty.iRemoteIdStatus
            = aMobileCallInfo.iRemoteParty.iRemoteIdStatus;
        }
    //no else

    if ( 0 < aMobileCallInfo.iRemoteParty.iCallingName.Length() &&
        ( RMobileCall::KCallingNameSize >=
            aMobileCallInfo.iRemoteParty.iCallingName.Length() ) )
        {
        iMobileCallInfo.iRemoteParty.iCallingName.Zero();
        iMobileCallInfo.iRemoteParty.iCallingName.Copy(
            aMobileCallInfo.iRemoteParty.iCallingName );

        iMobileCallInfo.iRemoteParty.iRemoteIdStatus
            = aMobileCallInfo.iRemoteParty.iRemoteIdStatus;
        }
    //no else

    if ( 0 < aMobileCallInfo.iDialledParty.iTelNumber.Length() &&
        ( RMobilePhone::KMaxMobileTelNumberSize >=
            aMobileCallInfo.iDialledParty.iTelNumber.Length() ) )
        {
        iMobileCallInfo.iDialledParty.iTelNumber.Zero();
        iMobileCallInfo.iDialledParty.iTelNumber.Copy(
            aMobileCallInfo.iDialledParty.iTelNumber );

        iMobileCallInfo.iDialledParty.iNumberPlan
            = aMobileCallInfo.iDialledParty.iNumberPlan;
        iMobileCallInfo.iDialledParty.iTypeOfNumber
            = aMobileCallInfo.iDialledParty.iTypeOfNumber;
        }
    //no else

    if ( RMobileCall::KCallForwarded & aMobileCallInfo.iValid )
        {
        iMobileCallInfo.iForwarded = aMobileCallInfo.iForwarded;
        }
    //no else

    if ( RMobileCall::KCallAlternating & aMobileCallInfo.iValid )
        {
        iMobileCallInfo.iAlternatingCall = aMobileCallInfo.iAlternatingCall;
        }
    //no else

    // update validity flags
    iMobileCallInfo.iValid |= aMobileCallInfo.iValid;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::DialDataCall
// Dial method for data calls
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::DialDataCall(
    const CCallDataPackage* aDataPackage )
    {

TFLOGSTRING("TSY: CMmCallMessHandler::DialDataCall");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_DIALDATACALL, "CMmCallMessHandler::DialDataCall" );

    TInt ret( KErrNone );
    RMobileCall::TMobileCallParamsV1Pckg* callParams( NULL );
    RMobileCall::TMobileCallInfoV1* callInfo( NULL );
    aDataPackage->UnPackData( &callParams, &callInfo );

    RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1(
       reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>( callParams ) );

    RMobileCall::TMobileCallParamsV1 recentCallParams( ( *paramsPckgV1 )() );

    if ( callParams && callInfo )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::DialDataCall. callParams and callInfo ok." );
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_DIALDATACALL, "CMmCallMessHandler::DialDataCall;callParams and callInfo ok." );

        SetMobileCallInfo( *callInfo );

        iIdRestrict = recentCallParams.iIdRestrict;
        iTelNumber = callInfo->iDialledParty.iTelNumber;

        // Dial the call
        iCallOperationID = CSD_CALL_CREATE;

        // Create Pipe for wideo telephony
        // If creation succeed, then CsdCallControlReq( CSD_CALL_CREATE )
        // is called on PnsPipeCreateResp()
        PnsPipeCreateReq( PN_PIPE_ENABLE );
        }
    else
        {
        ret = KErrArgument;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCallMessHandler::AnswerIncomingDataCall
// Answer incoming data call or set automatic answering ready
// for next incoming data call.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::AnswerIncomingDataCall(
    const CCallDataPackage* aCallDataPackage )
    {

    TFLOGSTRING("TSY: CMmCallMessHandler::AnswerIncomingDataCall");
//OstTrace0( TRACE_NORMAL, CMMDATAPORTHANDLER_ANSWERINCOMINGDATACALL, "CMmCallMessHandler::AnswerIncomingDataCall" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_ANSWERINCOMINGDATACALL, "CMmCallMessHandler::AnswerIncomingDataCall" );

    TInt ret( KErrNone );
    TInt callId( KCallIdNone );
    RMobilePhone::TMobileService callMode( RMobilePhone::EServiceUnspecified );

    aCallDataPackage->GetCallIdAndMode( callId, callMode );

TFLOGSTRING3("TSY: CMmCallMessHandler::AnswerIncomingDataCall. CallId:%d, CallMode:%d", callId, callMode);
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_ANSWERINCOMINGDATACALL, "CMmCallMessHandler::AnswerIncomingDataCall;callId=%d;callMode=%d", callId, callMode );

    // If we are answering an incoming call, the call status is not idle
    if ( RMobileCall::EStatusIdle < iMobileCallInfo.iStatus )
        {
TFLOGSTRING("TSY: CMmCallMessHandler::AnswerIncomingDataCall. Send ATA");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_ANSWERINCOMINGDATACALL, "CMmCallMessHandler::AnswerIncomingDataCall, Send ATA" );

        // Answer the call
        iCallOperationID = CSD_CALL_ANSWER;

        //Create Pipe for wideo telephony
        // If creation succeed, then CsdCallControlReq( CSD_CALL_ANSWER )
        // is called on PnsPipeCreateResp()
        PnsPipeCreateReq( PN_PIPE_ENABLE );
        }
        // If we are setting auto answer, the status is unknown
    else if ( RMobileCall::EStatusIdle >= iMobileCallInfo.iStatus )
        {
        // set status to Idle
        iMobileCallInfo.iStatus = RMobileCall::EStatusIdle;
        }
    //no else

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::HangUp
// This CORE API method disconnects the call. Used with normal voice calls,
// emergency calls as well as data calls. DialCancel also uses this method.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::HangUp(
    const TInt aCallId )
    {
TFLOGSTRING2("TSY: CMmCallMessHandler::HangUp - Call Id: %d", aCallId);
OstTrace1( TRACE_NORMAL, CMMCALLMESSHANDLER_HANGUP, "CMmCallMessHandler::HangUp;aCallId=%d", aCallId );

    TInt ret( KErrNone );

    // If valid call id
    if ( 0 < aCallId )
        {
        // Hangup the call
        ret = CsdCallControlReq( CSD_CALL_RELEASE );
        }
    else if ( RMobileCall::EStatusIdle == iMobileCallInfo.iStatus )
        {
        iMobileCallInfo.iStatus = RMobileCall::EStatusUnknown;
        }
    //no else
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeCreateReq
// Construct a PNS_PIPE_CREATE_REQ ISI-message
// ----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::PnsPipeCreateReq( const TUint8 aPipeStateAfter ) const
    {

    TFLOGSTRING("TSY: CMmCallMessHandler::PnsPipeCreateReq" );
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPECREATEREQ, "CMmCallMessHandler::PnsPipeCreateReq" );

   // create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PIPE_CREATE_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PIPE_CREATE_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // set message data
    data.Append( aPipeStateAfter );
    data.Append( PN_MSG_PRIORITY_HIGH );
    data.Append( KFirstPepDevice );
    data.Append( EIscNokiaDataport1 );
    data.Append( KFirstPepType );
    data.Append( KCallPadding );
    data.Append( iSecondPepDeviceId );
    data.Append( iSecondPepObjectId );
    data.Append( KSecondPepType );
    data.Append( KCallPadding );

    // Sending message to phonet
    return iPhoNetSender->Send(
        PN_PIPE, KPipeTransID, PNS_PIPE_CREATE_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeCreateResp
// Breaks a PNS_PIPE_CREATE_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmCallMessHandler::PnsPipeCreateResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::PnsPipeCreateResp");
OstTrace0( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPECREATERESP, "CMmCallMessHandler::PnsPipeCreateResp" );

    // Get transaction Id
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    if ( KPipeTransID == transId )
        {
        iPipeHandle = aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

        TUint8 errorCode( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING4("TSY: CMmCallMessHandler::PnsPipeCreateResp. TransactionId: %d, PipeHandle: %d, error code: %d", transId, iPipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_PNSPIPECREATERESP, "CMmCallMessHandler::PnsPipeCreateResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, iPipeHandle, errorCode );

        if ( PN_PIPE_NO_ERROR == errorCode )
            {
            // Dial or Answer the call
            CsdCallControlReq( iCallOperationID );
            }
        else if ( PN_PIPE_ERR_GENERAL == errorCode ) // Check PIPE Error Code
            {
            TUint8 pep1ErrorCode( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
                ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PEP1ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
                ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_PEP1ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

            TUint8 pep2ErrorCode( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
                ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PEP2ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
                ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_PEP2ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

            // Check PEP Error Codes
            if ( pep1ErrorCode == PN_PIPE_ERR_PEP_IN_USE ||
                 pep1ErrorCode == PN_PIPE_ERR_ALL_PIPES_IN_USE ||
                 pep2ErrorCode == PN_PIPE_ERR_PEP_IN_USE ||
                 pep2ErrorCode == PN_PIPE_ERR_ALL_PIPES_IN_USE )
                {
                // PEP can't create more pipes (all pipes in use),
                // error value is updated here
TFLOGSTRING("TSY: CMmCallMessHandler::PnsPipeCreateResp. ErrorCode changed to PN_PIPE_ERR_ALL_PIPES_IN_USE" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_PNSPIPECREATERESP, "CMmCallMessHandler::PnsPipeCreateResp, ErrorCode changed to PN_PIPE_ERR_ALL_PIPES_IN_USE" );
                }
            }
        //no else
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeEnableReq
// Construct a PNS_PIPE_ENABLE_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::PnsPipeEnableReq() const
    {
    TFLOGSTRING2("TSY: CMmCallMessHandler::PnsPipeEnableReq. PipeHandle: %d", iPipeHandle );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_PNSPIPEENABLEREQ, "CMmCallMessHandler::PnsPipeEnableReq;pipehandle=%hhu", iPipeHandle );

    // TODO: this method is not called. See TelAd-110
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PIPE_ENABLE_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PIPE_ENABLE_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    data.Append( iPipeHandle );
    data.Append( KCallPadding );

    // Send Isi message via phonet
    return iPhoNetSender->Send(
        PN_PIPE, KPipeTransID, PNS_PIPE_ENABLE_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeEnableResp
// Breaks a PNS_PIPE_ENABLE_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmCallMessHandler::PnsPipeEnableResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::PnsPipeEnableResp");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_PNSPIPEENABLERESP, "CMmCallMessHandler::PnsPipeEnableResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // TODO: this code makes no sense. See TelAd-110
    if ( KPipeTransID == transId )
        {
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_ENABLE_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_ENABLE_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING4("TSY: CMmCallMessHandler::PnsPipeEnableResp - traId: %d, PipeHandle: %d, ErrorCode: %d", transId, iPipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPEENABLERESP, "CMmCallMessHandler::PnsPipeEnableResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, iPipeHandle, errorCode );
        }
    // no else
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeResetReq
// Construct a PNS_PIPE_RESET_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::PnsPipeResetReq(
    const TUint8 aStateAfterReset )
    {
    TFLOGSTRING2("TSY: CMmCallMessHandler::PnsPipeResetReq. PipeHandle: %d", iPipeHandle );
OstTraceExt1( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPERESETREQ, "CMmCallMessHandler::PnsPipeResetReq;aPipeHandle=%hhu", iPipeHandle );

    // TODO: this method is not called. See TelAd-110
    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PIPE_RESET_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PIPE_RESET_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    data.Append( iPipeHandle );
    data.Append( aStateAfterReset );

    // Send Isi message via phonet
    return iPhoNetSender->Send(
        PN_PIPE, KPipeTransID, PNS_PIPE_RESET_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeResetResp
// Breaks a PNS_PIPE_RESET_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmCallMessHandler::PnsPipeResetResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::PnsPipeResetResp");
OstTrace0( TRACE_NORMAL, DUP2_CMMCALLMESSHANDLER_PNSPIPERESETRESP, "CMmCallMessHandler::PnsPipeResetResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // TODO: this code makes no sense. See TelAd-110
    if ( KPipeTransID == transId )
        {
        // Get Pipehandle from the ISI message
        TUint8 pipeHandle( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

        iPipeHandle = pipeHandle;

TFLOGSTRING3("TSY: CMmCallMessHandler::PnsPipeResetResp - traId: %d, pipe handle: %d", transId, pipeHandle);
OstTraceExt2( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPERESETRESP, "CMmCallMessHandler::PnsPipeResetResp;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

        TUint8 errorCode( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_RESET_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_RESET_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING3("TSY: CMmCallMessHandler::PnsPipeResetResp. PipeHandle: %d ErrorCode: %d", pipeHandle, errorCode );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_PNSPIPERESETRESP, "CMmCallMessHandler::PnsPipeResetResp;pipeHandle=%hhu;errorCode=%hhu", pipeHandle, errorCode );
        }
    // no else
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeRemoveReq
// Construct a PNS_PIPE_REMOVE_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::PnsPipeRemoveReq()
    {
    TFLOGSTRING2("TSY: CMmCallMessHandler::PnsPipeRemoveReq. PipeHandle: %d", iPipeHandle );
OstTraceExt1( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPEREMOVEREQ, "CMmCallMessHandler::PnsPipeRemoveReq;aPipeHandle=%hhu", iPipeHandle );

    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PIPE_REMOVE_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PIPE_REMOVE_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    data.Append( iPipeHandle );
    data.Append( KCallPadding );

    // Send Isi message via phonet
    return iPhoNetSender->Send(
        PN_PIPE,
        KPipeTransID,
        PNS_PIPE_REMOVE_REQ,
        data );
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeRemoveResp
// Breaks a PNS_PIPE_REMOVE_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmCallMessHandler::PnsPipeRemoveResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::PnsPipeRemoveResp");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_PNSPIPEREMOVERESP, "CMmCallMessHandler::PnsPipeRemoveResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // TODO: this code makes no sense. See TelAd-110
    if ( KPipeTransID == transId )
        {
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_REMOVE_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_REMOVE_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING4("TSY: CMmCallMessHandler::PnsPipeRemoveResp - traId: %d, PipeHandle: %d, ErrorCode: %d", transId, iPipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPEREMOVERESP, "CMmCallMessHandler::PnsPipeRemoveResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, iPipeHandle, errorCode );
        }
    // no else
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeRedirectReq
// Construct a PNS_PIPE_REDIRECT_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmCallMessHandler::PnsPipeRedirectReq()
    {
    TFLOGSTRING2("TSY: CMmCallMessHandler::PnsPipeRedirectReq. PipeHandle: %d", iPipeHandle );
OstTraceExt1( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPEREDIRECTREQ, "CMmCallMessHandler::PnsPipeRedirectReq;aPipeHandle=%hhu", iPipeHandle );

    // TODO: this method is not called. See TelAd-110
    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PIPE_REDIRECT_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PIPE_REDIRECT_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    data.Append( iPipeHandle );
    data.Append( PN_PIPE_ENABLE );
    data.Append( iSecondPepDeviceId );
    data.Append( iSecondPepObjectId );
    data.Append( KSecondPepType );
    data.Append( KCallPadding );
    data.Append( KSecondPepDevice );
    data.Append( KRedirectPepObject );
    data.Append( KSecondPepType );

    // Send Isi message via phonet
    return iPhoNetSender->Send(
        PN_PIPE,
        KPipeTransID,
        PNS_PIPE_REDIRECT_REQ,
        data );
    }

// ----------------------------------------------------------------------------
// CMmCallMessHandler::PnsPipeRedirectResp
// Breaks a PNS_PIPE_REDIRECT_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmCallMessHandler::PnsPipeRedirectResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::PnsPipeRedirectResp");
OstTrace0( TRACE_NORMAL, DUP1_CMMCALLMESSHANDLER_PNSPIPEREDIRECTRESP, "CMmCallMessHandler::PnsPipeRedirectResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    if ( KPipeTransID == transId )
        {
        // Get Pipehandle from the ISI message
        TUint8 pipeHandle( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

        iPipeHandle = pipeHandle;

        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_REDIRECT_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING4("TSY: CMmCallMessHandler::PnsPipeRedirectResp - traId: %d, PipeHandle: %d, ErrorCode: %d", transId, pipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL, CMMCALLMESSHANDLER_PNSPIPEREDIRECTRESP, "CMmCallMessHandler::PnsPipeRedirectResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, pipeHandle, errorCode );
        }
    // no else
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

    // None

//  End of File
