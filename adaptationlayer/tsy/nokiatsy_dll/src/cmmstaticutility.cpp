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



// INCLUDE FILES
#include "cmmstaticutility.h"
#include "nokiatsy_internal_variation.h"
#include "tsylogger.h"

#include <etelmm.h>
#include <etelmmerr.h>
#include <etelsat.h>
#include <exterror.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/rmmcustomapi.h>
#include <call_modemisi.h>
#include <csdisi.h>
#include <ss_wmisi.h>
#include <net_modemisi.h>
#include <smsisi.h>
#include <pipeisi.h>
#include <gpdsisi.h>
#include <pipe_sharedisi.h>
#include <in_sock.h>
#include <f32file.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmstaticutilityTraces.h"
#endif

#include "cmmuiccmesshandler.h"
// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
const TUint8 KNotAllowed = 0x01;
const TUint8 KAllowedWithModifications = 0x02;

// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
#ifdef __WINSCW__
    TBool apacInfoToBeReturned;
#endif // __WINSCW__

// LOCAL FUNCTION PROTOTYPES
    // None

// ======================= LOCAL FUNCTIONS =====================================
    // None

// ====================== MEMBER FUNCTIONS =====================================

// -----------------------------------------------------------------------------
// CMmStaticUtility::CSCauseToEpocError
// Converts CS cause to EPOC error value
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmStaticUtility::CSCauseToEpocError
        (
        TInt aResource, // CALL, SS, NET etc
        TUint8 aIsiCauseType, // Cause type
        TUint8 aIsiCauseValue // Cause value
        )
    {
TFLOGSTRING4("TSY: CMmStaticUtility::CSCauseToEpocError - resource: %d, cause type: %d, cause value: %d", aResource, aIsiCauseType, aIsiCauseValue);
OstTraceExt3( TRACE_NORMAL,  CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;aResource=%d;aIsiCauseType=%hhu;aIsiCauseValue=%hhu", aResource, aIsiCauseType, aIsiCauseValue );
    TInt epocError = ( KErrGeneral & 0x0000FFFF ) | ( KErrNotFound << 16 );

    if ( 0 == aIsiCauseValue && 0 == aIsiCauseType )
        {
        return KErrNone;
        }

    switch ( aResource )
        {
        case PN_MODEM_CALL:
            {
            // Common error
            if ( CALL_MODEM_CAUSE_TYPE_CLIENT == ( aIsiCauseType & 0x0F ) ||
                CALL_MODEM_CAUSE_TYPE_SERVER == ( aIsiCauseType & 0x0F ) )
                {
                // NOTE
                //
                // Following Error Mapping is HIGHLY GSM ISA 3.0 CALL SERVER
                // dependent. Following information was received 9.9.2002 from
                // the person that is in responsible of GSM ISA 3.0 Call server.
                //
                switch ( aIsiCauseValue )
                    {
                    // This is used only with CALL_DTMF_STATUS_IND when audio
                    // connection is suddenly disconnected by lower layers (and
                    // with CALL_STATUS_RESP when there are no other calls,
                    // CALL_STATUS_REQ/RESP is not used by MMTSY).
                    case CALL_MODEM_CAUSE_NO_CALL:
                    case CALL_MODEM_CAUSE_NO_CALL_ACTIVE:
                        {
                        epocError = EpocErrorCode( KErrEtelCallNotActive,
                            KErrMMEtelCallTerminated );
                        break;
                        }
                    // This is used when the local user disconnects the call.
                    // MMTSY maps this always to KErrNone. If remote user
                    // disconnects the call, network error code is used instead.
                    case CALL_MODEM_CAUSE_RELEASE_BY_USER:
                        {
                        epocError = EpocErrorCode( KErrGsmReleaseByUser,
                            KErrNotFound );
                        break;
                        }
                    // This occurs for example if Hold is requested when Call is
                    // not active, DTMF sending is requested if call is not
                    // active etc.
                    case CALL_MODEM_CAUSE_ERROR_REQUEST:
                        {
                        epocError = EpocErrorCode( KErrNotReady,
                            KErrNotFound );
                        break;
                        }
                    // This is used with Autohold if network rejects it
                    case CALL_MODEM_CAUSE_CALL_ACTIVE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    // This is used if Emergency call fails.
                    case CALL_MODEM_CAUSE_EMERGENCY_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCResourceNotAvailable );
                        break;
                        }
                    // This is used with Autohold if network rejects it
                    // This is used if call mode changing fails
                    case CALL_MODEM_CAUSE_INVALID_CALL_MODE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCBearerCapabilityNotAuthorised );
                        break;
                        }
                    // If there are other calls active when emergency call is
                    // activated, normal calls will be terminated with this
                    // error value
                    case CALL_MODEM_CAUSE_EMERGENCY:
                        {
                        epocError = EpocErrorCode( KErrAbort,
                            KErrGsmRRPreEmptiveRelease );
                        break;
                        }
                    // PIN number has not been given by user -> only emergency
                    // calls allowed
                    case CALL_MODEM_CAUSE_CODE_REQUIRED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsm0707SimPin1Required );
                        break;
                        }
                    // This is a basic error value returned when something is
                    // not allowed. Used in various situations inside the Call
                    // Server
                    case CALL_MODEM_CAUSE_NOT_ALLOWED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrMMEtelCallForbidden );
                        break;
                        }
                    // Used in rarely occuring DTMF buffer handling errors. Not
                    // really used.
                    case CALL_MODEM_CAUSE_DTMF_ERROR:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrMMEtelFormatNotSupported );
                        break;
                        }
                    // This is used only if re-establihment procedure fails
                    // which means that channel to the NW is lost and it can not
                    // be set-up again. This feature is supported very rarely by
                    // networks.
                    case CALL_MODEM_CAUSE_CHANNEL_LOSS:
                        {
                        epocError = EpocErrorCode( KErrEtelNoCarrier,
                            KErrMMEtelNetworkNotResponding );
                        break;
                        }
                    // This is used with CALL_CREATE_RESP when dialled number
                    // has not been in the FDN list -> dialling to the requested
                    // number has not been allowed.
                    case CALL_MODEM_CAUSE_FDN_NOT_OK:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmInvalidFdn );
                        break;
                        }
                    // Dialled number is in blacklist -> calling to this number
                    // is not allowed
                    case CALL_MODEM_CAUSE_BLACKLIST_BLOCKED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrMMEtelCallForbidden );
                        break;
                        }
                    // Dialled number is in blacklist -> calling to this number
                    // has been delayed and dialling is not allowed until
                    // specified time (see 3GPP specs) has elapsed.
                    case CALL_MODEM_CAUSE_BLACKLIST_DELAYED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrMMEtelCallForbidden );
                        break;
                        }
                    // Indicates that SIM card has not been inserted
                    case CALL_MODEM_CAUSE_NO_SIM:
                        {
                        epocError = EpocErrorCode( KErrNotReady,
                            KErrGsm0707SimNotInserted );
                        break;
                        }
                    // Error comes when dial is made when there is no network
                    case CALL_MODEM_CAUSE_NO_SERVICE:
                        {
                        epocError = EpocErrorCode( KErrCouldNotConnect,
                            KErrGsmNoService );
                        break;
                        }

                    // Not used by GSM ISA Call server: checking of
                    // parameters has been left to the client's responsibility
                    case CALL_MODEM_CAUSE_TOO_LONG_ADDRESS:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrNotFound );
                        break;
                        }
                    case CALL_MODEM_CAUSE_BUSY_USER_REQUEST:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCallRejected );
                        break;
                        }
                    case CALL_MODEM_CAUSE_INCOMPATIBLE_DEST:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCBearerCapabilityNotAuthorised );
                        break;
                        }
                    // Not used by GSM ISA Call server: checking of
                    // parameters has been left to the client's responsibility
                    case CALL_MODEM_CAUSE_INVALID_ADDRESS:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_MODEM_CALL - common error, default, ISI cause value: %d", aIsiCauseValue);
                        OstTraceExt1( TRACE_NORMAL,  DUP1_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_MODEM_CALL - common error, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }

            else if ( CALL_MODEM_CAUSE_TYPE_NETWORK == ( aIsiCauseType & 0x0F ) )
                {
                switch ( aIsiCauseValue )
                    {
                    case CALL_MODEM_NW_CAUSE_UNASSIGNED_NUMBER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCUnassignedNumber );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NO_ROUTE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCNoRouteToDestination );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_CH_UNACCEPTABLE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCChannelUnacceptable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_OPER_BARRING:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmCCOperatorDeterminedBarring );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NORMAL:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCNormalUnspecified );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_USER_BUSY:
                        {
                        epocError = EpocErrorCode( KErrEtelBusyDetected,
                            KErrGsmCCUserBusy );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NO_USER_RESPONSE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCUserNotResponding );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_ALERT_NO_ANSWER:
                        {
                        epocError = EpocErrorCode( KErrEtelNoAnswer,
                            KErrGsmCCUserAlertingNoAnswer );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_CALL_REJECTED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCCallRejected );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NUMBER_CHANGED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCNumberChanged );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NON_SELECT_CLEAR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCNonSelectedUserClearing );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_DEST_OUT_OF_ORDER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCDestinationOutOfOrder );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_INVALID_NUMBER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCInvalidNumberFormat );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_FACILITY_REJECTED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCFacilityRejected );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_RESP_TO_STATUS:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCResponseToStatusEnquiry );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NORMAL_UNSPECIFIED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCNormalUnspecified );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NO_CHANNEL:
                        {
                        epocError = EpocErrorCode( KErrEtelNoCarrier,
                            KErrGsmCCNoChannelAvailable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NETW_OUT_OF_ORDER:
                        {
                        epocError = EpocErrorCode( KErrEtelNoCarrier,
                            KErrGsmCCNetworkOutOfOrder );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_TEMPORARY_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrEtelNoCarrier,
                            KErrGsmCCTemporaryFailure );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_CONGESTION:
                        {
                        epocError = EpocErrorCode( KErrEtelNoCarrier,
                            KErrGsmCCSwitchingEquipmentCongestion );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_ACCESS_INFO_DISC:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCAccessInformationDiscarded );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_CHANNEL_NA:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCRequestedChannelNotAvailable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_RESOURCES_NA:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCResourceNotAvailable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_QOS_NA:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCQualityOfServiceNotAvailable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_FACILITY_UNSUBS:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCRequestedFacilityNotSubscribed );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_COMING_BARRED_CUG:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmCCIncomingCallsBarredInCug );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_BC_UNAUTHORIZED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCBearerCapabilityNotAuthorised );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_BC_NA:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCBearerCapabilityNotCurrentlyAvailable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_SERVICE_NA:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCServiceNotAvailable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_BEARER_NOT_IMPL:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCServiceNotImplemented );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_ACM_MAX:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmCCAcmGreaterThanAcmMax );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_FACILITY_NOT_IMPL:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCRequestedFacilityNotImplemented );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_ONLY_RDI_BC:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCOnlyRestrictedDigitalInformationBCAvailable );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_SERVICE_NOT_IMPL:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCServiceNotImplemented );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_INVALID_TI:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCInvalidCallReferenceValue );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_NOT_IN_CUG:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmCCUserNotInCug );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_INCOMPATIBLE_DEST:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCIncompatibleDestination );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_INV_TRANS_NET_SEL:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCInvalidTransitNetworkSelection );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_SEMANTICAL_ERR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCSemanticallyIncorrectMessage );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_INVALID_MANDATORY:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCInvalidMandatoryInformation );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_MSG_TYPE_INEXIST:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCNonExistentMessageType );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_MSG_TYPE_INCOMPAT:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCIncompatibleMessageInProtocolState );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_IE_NON_EXISTENT:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCNonExistentInformationElement );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_COND_IE_ERROR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCConditionalIEError );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_MSG_INCOMPATIBLE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCIncompatibleMessageInCallState );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_TIMER_EXPIRY:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCRecoveryOnTimerExpiry );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_PROTOCOL_ERROR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCUnspecifiedProtocolError );
                        break;
                        }
                    case CALL_MODEM_NW_CAUSE_INTERWORKING:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCCUnspecifiedInterworkingError );
                        break;
                        }
                    // 160,163: call collision, not in gsm spec but networks
                    // gives these values -> KErrGsmCallCollision.
                    case 160:
                    case 163:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmCallCollision );
                        break;
                        }
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_MODEM_CALL - Network specific error, unknown ISI cause value: %d", aIsiCauseValue);
                        OstTraceExt1( TRACE_NORMAL,  DUP2_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_MODEM_CALL - Network specific error, unknown ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            break;
            }
        case PN_MODEM_NETWORK:
            {
            if ( KTsyNetCauseCommon == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    case NET_CAUSE_OK:
                        {
                        epocError = KErrNone;
                        break;
                        }
                    case NET_CAUSE_COMMUNICATION_ERROR:
                        {
                        epocError = EpocErrorCode( KErrCouldNotConnect,
                            KErrGsmMMNetworkFailure );
                        break;
                        }
                    case NET_CAUSE_INVALID_PARAMETER:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_NO_SIM:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsm0707SimNotInserted );
                        break;
                        }
                    case NET_CAUSE_SIM_NOT_YET_READY:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_NET_NOT_FOUND:
                        {
                        epocError = EpocErrorCode( KErrNotFound,
                            KErrGsm0707NoNetworkService );
                        break;
                        }
                    case NET_CAUSE_REQUEST_NOT_ALLOWED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmCCFacilityRejected );
                        break;
                        }
                    case NET_CAUSE_CALL_ACTIVE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmNetCauseCallActive );
                        break;
                        }
                    case NET_CAUSE_SERVER_BUSY:
                        {
                        epocError = EpocErrorCode( KErrServerBusy,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_SECURITY_CODE_REQUIRED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_NOTHING_TO_CANCEL:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_UNABLE_TO_CANCEL:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_NETWORK_FORBIDDEN:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMPlmnNotAllowed );
                        break;
                        }
                    case NET_CAUSE_REQUEST_REJECTED:
                    case NET_CAUSE_CS_NOT_SUPPORTED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_PAR_INFO_NOT_AVAILABLE:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_NOT_DONE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_NO_SELECTED_NETWORK:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmMMNetworkFailure );
                        break;
                        }
                    case NET_CAUSE_REQUEST_INTERRUPTED:
                        {
                        epocError = EpocErrorCode( KErrCancel,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_TOO_BIG_INDEX:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_MEMORY_FULL:
                        {
                        epocError = EpocErrorCode( KErrNoMemory,
                            KErrNotFound );
                        break;
                        }
                    case NET_CAUSE_SERVICE_NOT_ALLOWED:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_MODEM_NETWORK, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP3_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_MODEM_NETWORK, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            // GSM specific error
            else if ( KTsyNetCauseGsm == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    case NET_GSM_IMSI_UNKNOWN_IN_HLR:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMImsiUnknownInHlr );
                        break;
                        }
                    case NET_GSM_ILLEGAL_MS:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMIllegalMs );
                        break;
                        }
                    case NET_GSM_IMSI_UNKNOWN_IN_VLR:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMImsiUnknownInVlr );
                        break;
                        }
                    case NET_GSM_IMEI_NOT_ACCEPTED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMImeiNotAccepted );
                        break;
                        }
                    case NET_GSM_ILLEGAL_ME:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMIllegalMe );
                        break;
                        }
                    case NET_GSM_PLMN_NOT_ALLOWED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMPlmnNotAllowed );
                        break;
                        }
                    case NET_GSM_LA_NOT_ALLOWED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMLocationAreaNotAllowed );
                        break;
                        }
                    case NET_GSM_ROAMING_NOT_IN_THIS_LA:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmMMRoamingNotAllowedInThisLocationArea );
                        break;
                        }
                    case NET_GSM_NETWORK_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmMMNetworkFailure );
                        break;
                        }
                    case NET_GSM_CONGESTION:
                        {
                        epocError = EpocErrorCode( KErrInUse,
                            KErrGsmMMCongestion );
                        break;
                        }
                    case NET_GSM_SERV_OPT_NOT_SUPPORTED:
                        {
                        epocError = EpocErrorCode( KErrNotSupported,
                            KErrGsmMMServiceOptionNotSupported );
                        break;
                        }
                    case NET_GSM_SERV_OPT_NOT_SUBSCRIBED:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmMMServiceOptionNotSubscribed );
                        break;
                        }
                    case NET_GSM_SERV_TEMP_OUT_OF_ORDER:
                        {
                        epocError = EpocErrorCode( KErrNotReady,
                            KErrGsmMMServiceOptionTemporaryOutOfOrder );
                        break;
                        }
                    case NET_GSM_SEMANTICALLY_INCORRECT:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmMMSemanticErrorInMessage );
                        break;
                        }
                    case NET_GSM_INVALID_MANDATORY_INFO:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmMMMandatoryInformationElementError );
                        break;
                        }
                    case NET_GSM_MSG_TYPE_NONEXISTENT:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmMMNonExistentMessageType );
                        break;
                        }
                    case NET_GSM_CONDITIONAL_IE_ERROR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmMMConditionalIEError );
                        break;
                        }
                    case NET_GSM_MSG_TYPE_WRONG_STATE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmMMIncompatibleMessageWithProtocolState );
                        break;
                        }
                    case NET_GSM_PROTOCOL_ERROR_UNSPECIFIED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmMMUnspecifiedProtocolError );
                        break;
                        }
                    case NET_GSM_AUTH_UNACCEPTABLE:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrMMEtelAuthenticateFailed );
                        break;
                        }
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_MODEM_NETWORK - GSM specific error, unknown ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP4_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_MODEM_NETWORK - GSM specific error, unknown ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGeneral );
                        break;
                        }
                    }
                }
            break;
            }
        case PN_SS:
            {
            if ( SS_GSM_INDICATE_ERROR == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    // Error codes ( GSM )
                    case SS_GSM_UNKNOWN_SUBSCRIBER:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSUnknownSubscriber );
                        break;
                        }
                    case SS_GSM_ILLEGAL_SUBSCRIBER:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSIllegalSubscriber );
                        break;
                        }
                    case SS_GSM_BEARER_SERV_NOT_PROVISION:
                        {
                        epocError = EpocErrorCode( KErrNotFound,
                            KErrGsmSSBearerServiceNotProvisioned );
                        break;
                        }
                    case SS_GSM_TELE_SERV_NOT_PROVISION:
                        {
                        epocError = EpocErrorCode( KErrNotFound,
                            KErrGsmSSTeleserviceNotProvisioned );
                        break;
                        }
                    case SS_GSM_ILLEGAL_EQUIPMENT:
                        {
                        epocError = EpocErrorCode( KErrNotSupported,
                            KErrGsmSSIllegalEquipment );
                        break;
                        }
                    case SS_GSM_CALL_BARRED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSCallBarred );
                        break;
                        }
                    case SS_GSM_ILLEGEL_SS_OPERATION:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSIllegalOperation );
                        break;
                        }
                    case SS_GSM_SS_ERROR_STATUS:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSErrorStatus );
                        break;
                        }
                    case SS_GSM_SS_NOT_AVAILABLE:
                        {
                        epocError = EpocErrorCode( KErrNotSupported,
                            KErrGsmSSNotAvailable );
                        break;
                        }
                    case SS_GSM_SS_SUBSCRIPTION_VIOLATION:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSSubscriptionViolation );
                        break;
                        }
                    case SS_GSM_SS_INCOMPATIBILITY:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSIncompatibility );
                        break;
                        }
                    case SS_GSM_FACILITY_NOT_SUPPORTED:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSFacilityNotSupported );
                        break;
                        }
                    case SS_GSM_ABSENT_SUBSCRIBER:
                        {
                        epocError = EpocErrorCode( KErrNotFound,
                            KErrGsmSSAbsentSubscriber );
                        break;
                        }
                    case SS_GSM_SHORT_TERM_DENIAL:
                    case SS_GSM_LONG_TERM_DENIAL:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrNotFound );
                        break;
                        }
                    case SS_GSM_SYSTEM_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSSSystemFailure );
                        break;
                        }
                    case SS_GSM_DATA_MISSING:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSDataMissing );
                        break;
                        }
                    case SS_GSM_UNEXPECTED_DATA_VALUE:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSUnexpectedDataValue );
                        break;
                        }
                    case SS_GSM_PASSWORD_REGIS_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSPasswordRegistrationFailure );
                        break;
                        }
                    case SS_GSM_NEGATIVE_PASSWORD_CHECK:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSNegativePasswordCheck );
                        break;
                        }
                    case SS_GSM_MAXNUM_OF_PW_ATT_VIOLATION:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSSPasswordAttemptsViolation );
                        break;
                        }
                    case SS_GSM_UNKNOWN_ALPHABET:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSUnknownAlphabet );
                        break;
                        }
                    case SS_GSM_USSD_BUSY:
                        {
                        epocError = EpocErrorCode( KErrInUse,
                            KErrGsmSSUssdBusy );
                        break;
                        }
                    case SS_GSM_MAXNUM_OF_MPTY_PART_EXCEED:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSMaxNumMptyParticipants );
                        break;
                        }
                    case SS_GSM_RESOURCES_NOT_AVAILABLE:
                        {
                        epocError = EpocErrorCode( KErrNotFound,
                            KErrGsmSSResourcesUnavailable );
                        break;
                        }
                    case SS_GSM_UNKNOWN_ERROR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsm0707UnknownError );
                        break;
                        }
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS, unknown ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP5_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS, unknown ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_GSM_INDICATE_PASSWORD_ERROR == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    // GSM Guidance info
                    case SS_GSM_BAD_PASSWORD:
                    case SS_GSM_BAD_PASSWORD_FORMAT:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSNegativePasswordCheck );
                        break;
                        }
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - GSM_INDICATE_PASSWORD_ERROR, unknown ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP6_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - GSM_INDICATE_PASSWORD_ERROR, unknown ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_GSM_INDICATE_MSG_ERROR == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    // GSM Message errors
                    case SS_GSM_MSG_INCORRECT:
                    case SS_GSM_MSG_UNKNOWN_COMPONENT:
                    case SS_GSM_MSG_INVALID_INVOKE_ID:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - GSM_INDICATE_MSG_ERROR, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP7_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - GSM_INDICATE_MSG_ERROR, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_OTHER_ERROR == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    // Miscellaneous SS errors
                    case SS_GSM_SERVICE_NOT_ON_FDN_LIST:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSSCallBarred );
                        break;
                        }
                    case SS_GSM_USSD_BUSY:
                        {
                        epocError = EpocErrorCode( KErrServerBusy,
                            KErrGsmSSUssdBusy );
                        break;
                        }
                    case SS_RESOURCE_CONTROL_DENIED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrSatControl );
                        break;
                        }
                    case SS_SERVICE_BUSY:
                    case SS_GSM_MM_RELEASE:
                    case SS_GSM_SS_NOT_AVAILABLE:
                    case SS_GSM_DATA_ERROR:
                    case SS_GSM_REQUEST_CANCELLED:
                    case SS_GSM_INDICATE_DEVICE_PASSWORD:
                    case SS_GSM_ACTIVATION_DATA_LOST:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - OTHER_ERROR, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP8_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - OTHER_ERROR, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( aIsiCauseType == 0xFF )
                {
                switch ( aIsiCauseValue )
                    {
                    // Service not supported resp
                    // Reason values
                    case SS_UNSPECIFIED_REASON:
                    case SS_UNKNOWN_MMI_SS_CODE:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - service not supprted, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP9_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - service not supprted, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_GSM_GENERAL_PROBLEM == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    case SS_GSM_UNRECOGNIZED_COMP:
                    case SS_GSM_MISTYPED_COMP:
                    case SS_GSM_BADLY_STRUCTURED_COMP:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - GSM_GENERAL_PROBLEM, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP10_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - GSM_GENERAL_PROBLEM, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_GSM_INVOKE_PROBLEM == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                     case SS_GSM_MISTYPED_INVOKE_PARAMETER:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSSIllegalOperation );
                        break;
                        }
                    case SS_GSM_DUPLICATE_INVOKE_ID:
                    case SS_GSM_UNRECOGNIZED_OPERATION:
                    case SS_GSM_RECOURCE_LIMITATION:
                    case SS_GSM_INITIATING_RELEASE:
                    case SS_GSM_UNRECOGNIZED_LINKED_ID:
                    case SS_GSM_LINKED_RESPONSE_UNEXPECTED:
                    case SS_GSM_UNEXPECTED_LINKED_OPERATION:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - GSM_INVOKE_PROBLEM, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP11_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - GSM_INVOKE_PROBLEM, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_GSM_RETURN_RESULT_PROBLEM == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    case SS_GSM_UNRECOGNIZED_INVOKE_ID:
                    case SS_GSM_RETURN_RESULT_UNEXPECTED:
                    case SS_GSM_MISTYPED_RESP_PARAMETER:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - GSM_RETURN_RESULT_PROBLEM, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP12_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - GSM_RETURN_RESULT_PROBLEM, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_GSM_RETURN_ERROR_PROBLEM == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    case SS_GSM_RETURN_ERROR_UNEXPECTED:
                    case SS_GSM_UNRECOGNIZED_ERROR:
                    case SS_GSM_UNEXPECTED_ERROR:
                    case SS_GSM_MISTYPED_ERR_PARAMETER:
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - GSM_RETURN_ERROR_PROBLEM, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP13_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - GSM_RETURN_ERROR_PROBLEM, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SS_GSM_MM_RELEASED == aIsiCauseType )
                {
                switch ( aIsiCauseValue )
                    {
                    case SS_GSM_MM_CAUSE_OUT_OF_ORDER_S_OPTION:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmMMServiceOptionTemporaryOutOfOrder );
                        break;
                        }
                    // Error codes ( GSM )
                    case 0x00:
                        {
                        epocError = EpocErrorCode( KErrCouldNotConnect,
                            KErrGsmSMSNoNetworkService );
                        break;
                        }
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SS - GSM_MM_RELEASED, default, ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP14_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SS - GSM_MM_RELEASED, default, ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            // 0x01 is KSSREQUESTCANCELLED
            else if ( aIsiCauseType == 0x01 )
                {
                epocError = EpocErrorCode( KErrCancel,
                    KErrNotFound );
                }
            break;
            }
        case PN_SMS:
            {
            if ( SMS_CAUSE_TYPE_COMMON == aIsiCauseType )
                {
                // In some cases we can't make optimal error mapping because
                // UI doesn't have error message for all Symbian OS errors.
                switch ( aIsiCauseValue )
                    {
                    // Fail reasons
                    case SMS_OK:
                        {
                        epocError = KErrNone;
                        break;
                        }
                    case SMS_ERR_ROUTING_RELEASED:
                    case SMS_ERR_ROUTE_NOT_AVAILABLE:
                    case SMS_ERR_CS_INACTIVE:
                    case SMS_ERR_RECEPTION_FAILED:
                    case SMS_ERR_RESEND_ALREADY_DONE:
                        {
                        epocError = KErrGeneral;
                        break;
                        }
                    case SMS_ERR_ROUTE_NOT_ALLOWED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSNoNetworkService );
                        break;
                        }
                    case SMS_ERR_INVALID_PARAMETER:
                        {
                        epocError = KErrArgument;
                        break;
                        }
                    case SMS_ERR_DEVICE_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSFailureInME );
                        break;
                        }
                    case SMS_ERR_PP_RESERVED:
                    case SMS_ERR_SERVICE_RESERVED:
                        {
                        epocError = KErrInUse;
                        break;
                        }
                    case SMS_ERR_SENDING_ONGOING:
                        {
                        epocError = KErrServerBusy;
                        break;
                        }
                    case SMS_ERR_INVALID_LOCATION:
                        {
                        epocError = KErrArgument;
                        break;
                        }
                    case SMS_ERR_NO_NETW_RESPONSE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSNetworkFailure );
                        break;
                        }
                    case SMS_ERR_DEST_ADDR_FDN_RESTRICTED:
                    case SMS_ERR_SMSC_ADDR_FDN_RESTRICTED:
                        {
                        epocError = EpocErrorCode( KErrAccessDenied,
                            KErrGsmSMSOperationNotAllowed );
                        break;
                        }
                    case SMS_ERR_SMSC_ADDR_NOT_AVAILABLE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnassignedNumber );
                        break;
                        }
                    case SMS_ERR_SERVER_NOT_READY:
                        {
                        epocError = KErrNotReady;
                        break;
                        }
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SMS - common error, unknown ISI cause value: %d",aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP15_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SMS - common error, unknown ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            else if ( SMS_CAUSE_TYPE_EXT == aIsiCauseType )
                {
                // In some cases we can't make optimal error mapping because
                // UI doesn't have error message for all Symbian OS errors.
                switch ( aIsiCauseValue )
                    {
                    // Fail reasons
                    case SMS_EXT_ERR_UNASSIGNED_NUMBER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnassignedNumber );
                        break;
                        }
                    case SMS_EXT_ERR_OPER_DETERMINED_BARR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSOperatorDeterminedBarring );
                        break;
                        }
                    case SMS_EXT_ERR_CALL_BARRED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSCallBarred );
                        break;
                        }
                    case SMS_EXT_ERR_RESERVED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSReserved );
                        break;
                        }
                    case SMS_EXT_ERR_MSG_TRANSFER_REJ:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSShortMessageTransferRejected );
                        break;
                        }
                    case SMS_EXT_ERR_MEMORY_CAPACITY_EXC:
                        {
                        epocError = KErrDiskFull;
                        break;
                        }
                    case SMS_EXT_ERR_DEST_OUT_OF_ORDER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSDestinationOutOfOrder );
                        break;
                        }
                    case SMS_EXT_ERR_UNDEFINED_SUBSCRIBER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnidentifiedSubscriber );
                        break;
                        }
                    case SMS_EXT_ERR_FACILITY_REJECTED:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSFacilityRejected );
                        break;
                        }
                    case SMS_EXT_ERR_UNKNOWN_SUBSCRIBER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnknownSubscriber );
                        break;
                        }
                    case SMS_EXT_ERR_NETW_OUT_OF_ORDER:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSNetworkOutOfOrder );
                        break;
                        }
                    case SMS_EXT_ERR_TEMPORARY_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSTemporaryFailure );
                        break;
                        }
                    case SMS_EXT_ERR_CONGESTION:
                    case SMS_EXT_ERR_LOW_LAYER_CONGESTION:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSCongestion );
                        break;
                        }
                    case SMS_EXT_ERR_RESOURCE_UNAVAILABLE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSResourcesUnavailable );
                        break;
                        }
                    case SMS_EXT_ERR_REQ_FACILITY_NOT_SUB:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSRequestedFacilityNotSubscribed );
                        break;
                        }
                    case SMS_EXT_ERR_REQ_FACILITY_NOT_IMP:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSRequestedFacilityNotImplemented );
                        break;
                        }
                    case SMS_EXT_ERR_INVALID_REFERENCE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSInvalidShortMessageTransferReferenceValue );
                        break;
                        }
                    case SMS_EXT_ERR_INCORRECT_MESSAGE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnspecifiedInvalidMessage );
                        break;
                        }
                    case SMS_EXT_ERR_INVALID_MAND_INFO:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSInvalidMandatoryInformation );
                        break;
                        }
                    case SMS_EXT_ERR_INVALID_MSG_TYPE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSNonExistentMessageType );
                        break;
                        }
                    case SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSIncompatibleMessageWithSmsProtocolState );
                        break;
                        }
                    case SMS_EXT_ERR_INVALID_INFO_ELEMENT:
                        {
                        epocError = EpocErrorCode( KErrArgument,
                            KErrGsmSMSInformationElementNotImplemented );
                        break;
                        }
                    case SMS_EXT_ERR_PROTOCOL_ERROR:
                    case SMS_EXT_ERR_NO_CAUSE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnspecifiedProtocolError );
                        break;
                        }
                    case SMS_EXT_ERR_INTERWORKING:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnspecifiedInterworkingError );
                        break;
                        }
                    case SMS_EXT_ERR_NETWORK_FAILURE:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSNetworkFailure );
                        break;
                        }
                    case SMS_EXT_ERR_IMSI_UNKNOWN_HLR:
                    case SMS_EXT_ERR_ILLEGAL_MS:
                    case SMS_EXT_ERR_IMSI_UNKNOWN_VLR:
                    case SMS_EXT_ERR_IMEI_NOT_ACCEPTED:
                    case SMS_EXT_ERR_ILLEGAL_ME:
                    case SMS_EXT_ERR_PLMN_NOT_ALLOWED:
                    case SMS_EXT_ERR_LA_NOT_ALLOWED:
                    case SMS_EXT_ERR_ROAM_NOT_ALLOWED_LA:
                    case SMS_EXT_ERR_NO_SUITABLE_CELLS_LA:
                    case SMS_EXT_ERR_MAC_FAILURE:
                    case SMS_EXT_ERR_SYNC_FAILURE:
                    case SMS_EXT_ERR_AUTH_UNACCEPTABLE:
                    case SMS_EXT_ERR_SERV_OPT_NOT_SUPPORTED:
                    case SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED:
                    case SMS_EXT_ERR_SERV_OPT_TEMP_OUT_OF_ORDER:
                    case SMS_EXT_ERR_CALL_CANNOT_BE_IDENTIFIED:
                    case SMS_EXT_ERR_SEMANTICALLY_INCORR_MSG:
                    case SMS_EXT_ERR_LOW_LAYER_INVALID_MAND_INFO:
                    case SMS_EXT_ERR_LOW_LAYER_INVALID_MSG_TYPE:
                    case SMS_EXT_ERR_LOW_LAYER_MSG_TYPE_NOT_COMP_WITH_ST:
                    case SMS_EXT_ERR_LOW_LAYER_INVALID_INFO_ELEMENT:
                    case SMS_EXT_ERR_CONDITIONAL_IE_ERROR:
                    case SMS_EXT_ERR_LOW_LAYER_MSG_NOT_COMP_WITH_ST:
                    case SMS_EXT_ERR_LOW_LAYER_PROTOCOL_ERROR:
                        {
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmSMSUnspecifiedProtocolError );
                        break;
                        }
#if ( NCP_COMMON_CELLMO_BRANCH_SUPPORT == NCP_COMMON_CELLMO_BRANCH_0711 )
                    case SMS_EXT_ERR_CS_BARRED:
                        {
TFLOGSTRING("TSY: CMmStaticUtility::CSCauseToEpocError - DSAC: SMS was rejected because of barred CS" );
OstTrace0( TRACE_NORMAL,  DUP22_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError - DSAC: SMS was rejected because of barred CS" );
                        epocError = EpocErrorCode(
                            KErrGeneral, KErrGsmCSConnectionBarred );
                        break;
                        }
#endif // NCP_COMMON_CELLMO_BRANCH_SUPPORT == NCP_COMMON_CELLMO_BRANCH_0711
                    default:
                        {
TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError - PN_SMS - GSM specific error, unknown ISI cause value: %d",aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP16_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;PN_SMS - GSM specific error, unknown ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                        epocError = EpocErrorCode( KErrGeneral,
                            KErrNotFound );
                        break;
                        }
                    }
                }
            break;
            }
        case PN_CSD:
            {
            switch ( aIsiCauseValue )
                {
                case CSD_CALL_CAUSE_BEARER_RESERVATION_FAILED:
                    {
                    // Resource Reservation is denied.
                    epocError = EpocErrorCode( KErrNotReady,
                            KErrGsmCCBearerCapabilityNotAuthorised );
                    break;
                    }
                case CSD_CALL_CAUSE_NOT_ALLOWED:
                    {
                    // General cause value.
                    epocError = EpocErrorCode( KErrGeneral,
                            KErrGsmNotAllowed );
                    break;
                    }
                case CSD_CALL_CAUSE_NO_VIDEO_CALL:
                    {
                    // There is no (video) call to be answered,
                    // released or rejected.
                    epocError = EpocErrorCode( KErrNotFound,
                        KErrGsmNoCallActive );
                    break;
                    }
                case CSD_CALL_CAUSE_OTHER_CALL_ACTIVE:
                    {
                    // There is another call active, and call can’t
                    // be created or answered via CSD ISI interface.
                    epocError = EpocErrorCode( KErrInUse,
                        KErrGsmCallActive );
                    break;
                    }
                case CSD_CALL_CAUSE_USER_ABORT:
                    {
                    // User has canceled the call creation.
                    epocError = EpocErrorCode( KErrAbort,
                        KErrGsmCallRejected );
                    break;
                    }
                case CSD_CALL_CAUSE_INVALID_SYSTEM:
                    {
                    // Call creation has been asked in invalid network
                    // system
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmNoService );
                    break;
                    }
                case CSD_CALL_CAUSE_INVALID_CBST_PARAMS:
                    {
                    // Given CBST parameters are invalid.
                    epocError = EpocErrorCode( KErrArgument,
                        KErrGsmInvalidParameter );
                    break;
                    }
                default:
                    {
                    TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError\
                        - PN_CSD, deafault, ISI cause value: %d",
                        aIsiCauseValue);
OstTrace1( TRACE_NORMAL,  DUP23_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;aIsiCauseValue=%d", aIsiCauseValue );

                    epocError = EpocErrorCode( KErrGeneral, KErrNotFound );
                    break;
                    }
                }
            break;
            }
        }

TFLOGSTRING2("TSY: CMmStaticUtility::CSCauseToEpocError, Error was originated by ISA Server, Resource ID: 0x%x", aResource);
OstTrace1( TRACE_NORMAL,  DUP20_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;Error was originated by ISA Server, Resource ID=%x", aResource );

TFLOGSTRING3("TSY: CMmStaticUtility::CSCauseToEpocError,ISA Error type: %d, ISA Error value: %d", aIsiCauseType, aIsiCauseValue);
OstTraceExt2( TRACE_NORMAL,  DUP21_CMMSTATICUTILITY_CSCAUSETOEPOCERROR_TD, "CMmStaticUtility::CSCauseToEpocError;ISA Error type=%hhu; ISA Error value=%hhu", aIsiCauseType, aIsiCauseValue );

    return epocError;
    }



// -----------------------------------------------------------------------------
// CMmStaticUtility::UICCCSCauseToEpocError
// Converts CS cause to EPOC error value
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmStaticUtility::UICCCSCauseToEpocError
        (
        TUint8 aIsiCauseValue // Cause value
        )
    {
TFLOGSTRING2("TSY: CMmStaticUtility::UICCCSCauseToEpocError - cause value: %d", aIsiCauseValue);
//OstTraceExt2( TRACE_NORMAL,  CMMSTATICUTILITY_SIMCSCAUSETOEPOCERROR_TD, "CMmStaticUtility::SimCSCauseToEpocError;aIsiCauseValue=%hhu", aIsiCauseValue );

    TInt epocError = ( KErrGeneral & 0x0000FFFF ) | ( KErrNotFound << 16 );

    // SIM_STATUS_TABLE
        switch ( aIsiCauseValue )
            {
            case UICC_STATUS_OK:
                {
                epocError = KErrNone;
                }
                break;
            case UICC_STATUS_FAIL:
            case UICC_STATUS_UNKNOWN:
                {
                epocError = KErrGeneral;
                }
                break;
            case UICC_STATUS_NOT_READY:
                {
                epocError = EpocErrorCode( KErrNotReady,
                    KErrGsm0707SimFailure );
                }
                break;
            case UICC_STATUS_START_UP_COMPLETED:
                {
                epocError = KErrNone;
                }
                break;
            case UICC_STATUS_SHUTTING_DOWN:
                {
                epocError = EpocErrorCode( KErrServerTerminated,
                    KErrNotFound );
                }
                break;
            case UICC_STATUS_CARD_NOT_READY:
                {
                epocError = KErrGeneral;
                }
                break;
            case UICC_STATUS_CARD_READY:
                {
                epocError = KErrNone;
                }
                break;
            case UICC_STATUS_CARD_DISCONNECTED:
                {
                epocError = EpocErrorCode( KErrDisconnected,
                    KErrGsm0707SimWrong );
                }
                break;
            case UICC_STATUS_CARD_NOT_PRESENT:
                {
                epocError = EpocErrorCode( KErrHardwareNotAvailable,
                    KErrGsm0707SimNotInserted );
                }
                break;
            case UICC_STATUS_APPL_ACTIVE:
                {
                epocError = KErrNone;
                }
                break;
            case UICC_STATUS_APPL_NOT_ACTIVE:
                {
                epocError = KErrGeneral;
                }
                break;
            case UICC_STATUS_PIN_ENABLED:
                {
                epocError = EpocErrorCode( KErrAccessDenied,
                    KErrGsm0707SimPin1Required );
                }
                break;
            case UICC_STATUS_PIN_DISABLED:
                {
                epocError = EpocErrorCode( KErrLocked,
                    KErrGsm0707OperationNotAllowed );
                break;
                }
            default:
                {
TFLOGSTRING2("TSY: CMmStaticUtility::UICCCSCauseToEpocError -  UICC_STATUS_TABLE, unknown ISI cause value: %d", aIsiCauseValue);
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMSTATICUTILITY_UICCCSCAUSETOEPOCERROR_TD, "CMmStaticUtility::UICCCSCauseToEpocError; - UICC_STATUS_TABLE, unknown ISI cause value aIsiCauseValue=%hhu", aIsiCauseValue );
                epocError = EpocErrorCode( KErrGeneral,
                    KErrNotFound );
                break;
                }
            }

TFLOGSTRING("TSY: CMmStaticUtility::UiccCSCauseToEpocError, Error was originated by UICC Server - Resource ID: PN_SIM");
OstTrace0( TRACE_NORMAL,  DUP11_CMMSTATICUTILITY_SIMCSCAUSETOEPOCERROR_TD, "CMmStaticUtility::SimCSCauseToEpocError, Error was originated by ISA Server - Resource ID: PN_SIM" );

TFLOGSTRING2("TSY: CMmStaticUtility::UiccCSCauseToEpocError, UICC Error value: %d", aIsiCauseValue );
OstTraceExt1( TRACE_NORMAL,  DUP12_CMMSTATICUTILITY_UICCCSCAUSETOEPOCERROR_TD, "CMmStaticUtility::UiccCSCauseToEpocError;UICC Error value:aIsiCauseValue=%hhu", aIsiCauseValue );

    return epocError;
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::PacketDataCSCauseToEpocError
// Converts Packet Data CS cause to EPOC error value
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmStaticUtility::PacketDataCSCauseToEpocError
        (
        TUint8 aIsiCauseValue, // cause value from server
        TInt aResource, // resource, default value PN_GPDS
        TUint8 aIsiCauseType // cause type, default value 0x00
        )
    {
TFLOGSTRING4("TSY: CMmStaticUtility::PacketDataCSCauseToEpocError - resource: %d, cause type: %d, cause value: %d", aResource, aIsiCauseType, aIsiCauseValue);
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_PACKETDATACSCAUSETOEPOCERROR_TD, "CMmStaticUtility::PacketDataCSCauseToEpocError" );

    TInt epocError( KErrGeneral );

    if ( PN_GPDS == aResource )
        {
        // Default value for aIsiCauseType is 0x00
        if ( 0x00 == aIsiCauseType )
            {
            switch ( aIsiCauseValue )
                {
                case GPDS_CAUSE_GPRS_NOT_ALLOWED:
                case GPDS_CAUSE_GPRS_NOT_ALLOWED_IN_PLMN:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsServicesNotAllowed );
                    break;
                    }
                case GPDS_NOT_ALLOWED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsAndNonGprsServicesNotAllowed );
                    break;
                    }
                case GPDS_CAUSE_MS_IDENTITY:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsMSIdentityCannotBeDerivedByTheNetwork );
                    break;
                    }
                case GPDS_CAUSE_DETACH:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsMSImplicitlyDetached );
                    break;
                    }
                case GPDS_CAUSE_MSC_NOT_REACH:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsMSCTemporarilyNotReachable );
                    break;
                    }
                case GPDS_CAUSE_RESOURCE_INSUFF:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMNetworkFailure );
                    break;
                    }
                case GPDS_CAUSE_APN:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsMissingorUnknownAPN );
                    break;
                    }
                case GPDS_CAUSE_PDP_UNKNOWN:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsUnknownPDPAddress );
                    break;
                    }
                case GPDS_CAUSE_AUT_FAILURE:
                case GPDS_CAUSE_AUTHENTICATION:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsUserAuthenticationFailure );
                    break;
                    }
                case GPDS_CAUSE_ACT_REJECT_GGSN:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsActivationRejectedByGGSN );
                    break;
                    }
                case GPDS_CAUSE_ACT_REJECT:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsActivationRejected );
                    break;
                    }
                case GPDS_CAUSE_DEACT_REGULAR:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsRegularContextDeactivation );
                    break;
                    }
                case GPDS_CAUSE_QOS:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsQoSNotAccepted );
                    break;
                    }
                case GPDS_CAUSE_REACTIVATION_REQ:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsReactivationRequested );
                    break;
                    }
                case GPDS_CAUSE_FEAT_NOT_SUPPORTED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsFeatureNotSupported );
                    break;
                    }
                case GPDS_CAUSE_NETWORK_CONGESTION:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMCongestion );
                    break;
                    }
                case GPDS_CAUSE_NETWORK_FAIL:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMNetworkFailure );
                    break;
                    }
                case GPDS_CAUSE_UNKNOWN:
                    {
                    epocError = KErrUnknown;
                    break;
                    }
                case GPDS_CAUSE_LLC_SNDCP_FAILURE:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsLlcOrSndcpFailure );
                    break;
                    }
                case GPDS_CAUSE_UNSPECIFIED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMUnspecifiedProtocolError );
                    break;
                    }
                case GPDS_CAUSE_CONTEXT_UNKNOWN:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsUnknownPDPContext );
                    break;
                    }
                case GPDS_CAUSE_ME_ILLEGAL:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMIllegalMe );
                    break;
                    }
                case GPDS_CAUSE_MS_ILLEGAL:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMIllegalMs );
                    break;
                    }
                case GPDS_ROAMING_NOT_ALLOWED:
                case GPDS_CAUSE_LAI_FORBIDDEN_NATIONAL_ROAM_LIST:
                case GPDS_CAUSE_LAI_FORBIDDEN_REG_PROVISION_LIST:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMRoamingNotAllowedInThisLocationArea );
                    break;
                    }
                case GPDS_PLMN_NOT_ALLOWED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMPlmnNotAllowed );
                    break;
                    }
                case GPDS_LA_NOT_ALLOWED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMLocationAreaNotAllowed );
                    break;
                    }
                case GPDS_CAUSE_IMSI:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGsmMMImsiUnknownInHlr );
                    break;
                    }
                case GPDS_CAUSE_ACCESS_BARRED:
                    {
                    epocError = KErrAccessDenied;
                    break;
                    }
                case GPDS_CAUSE_NO_CONNECTION:
                    {
                    epocError = KErrDisconnected;
                    break;
                    }
                case GPDS_CAUSE_POWER_OFF:
                case GPDS_CAUSE_SIM_REMOVED:
                    {
                    epocError = KErrHardwareNotAvailable;
                    break;
                    }
                case GPDS_CAUSE_SERV_OPT_NOT_SUPPORTED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsServiceOptionNotSupported );
                    break;
                    }
                case GPDS_CAUSE_SERV_OPT_NOT_SUBSCRIBED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsRequestedServiceOptionNotSubscribed );
                    break;
                    }
                case GPDS_CAUSE_SERV_OPT_OUT_OF_ORDER:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsServiceOptionTemporarilyOutOfOrder );
                    break;
                    }
                case GPDS_CAUSE_TFT_SEMANTIC_ERROR:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsSemanticErrorInTFTOperation );
                    break;
                    }
                case GPDS_CAUSE_FILTER_SEMANTIC_ERROR:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsSemanticErrorsInPacketFilters );
                    break;
                    }
                case GPDS_CAUSE_TFT_SYNTAX_ERROR:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsSyntacticalErrorInTFTOperation );
                    break;
                    }
                case GPDS_CAUSE_CONT_WITHOUT_TFT:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsPDPContextWithoutTFTAlreadyActivated );
                    break;
                    }
                case GPDS_CAUSE_FILTER_SYNTAX_ERROR:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsSyntacticalErrorInPacketFilters );
                    break;
                    }
                case GPDS_CAUSE_RETRY_COUNTER_EXPIRED:
                    {
                    epocError = KErrTimedOut;
                    break;
                    }
                case GPDS_CAUSE_FDN:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsServicesNotAllowed );
                    break;
                    }
                case GPDS_CAUSE_NSAPI_ALREADY_USED:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrGprsNSAPIAlreadyUsed );
                    break;
                    }
                case GPDS_CAUSE_APN_CONTROL:
                    {
                    epocError = EpocErrorCode( KErrGeneral,
                        KErrPacketDataTsyInvalidAPN );
                    break;
                    }
                case GPDS_CAUSE_APN_INCOMPATIBLE_WITH_CURR_CTXT:
                case GPDS_CAUSE_PLMN_FAIL:
                case GPDS_CAUSE_FATAL_FAILURE:
                case GPDS_CAUSE_TEMPERATURE_LIMIT:
                default:
                    {
                    epocError = KErrGeneral;
                    break;
                    }
                }
            }
        }
    else if ( PN_PIPE == aResource )
        {
        // Default value for aIsiCauseType is 0x00
        if ( 0x00 == aIsiCauseType )
            {
            switch ( aIsiCauseValue )
                {
                case PN_PIPE_ERR_NOT_ALLOWED:
                    {
                    epocError = KErrAccessDenied;
                    break;
                    }
                case PN_PIPE_ERR_ALL_PIPES_IN_USE:
                case PN_PIPE_ERR_PEP_IN_USE:
                    {
                    epocError = KErrInUse;
                    break;
                    }
                case PN_PIPE_ERR_DEV_DISCONNECTED:
                    {
                    epocError = KErrDisconnected;
                    break;
                    }
                case PN_PIPE_ERR_TIMEOUT:
                    {
                    epocError = KErrTimedOut;
                    break;
                    }
                case PN_PIPE_ERR_INVALID_PARAM:
                case PN_PIPE_ERR_INVALID_HANDLE:
                case PN_PIPE_ERR_INVALID_CTRL_ID:
                    {
                    epocError = KErrArgument;
                    break;
                    }
                case PN_PIPE_ERR_OVERLOAD:
                case PN_PIPE_ERR_GENERAL:
                default:
                    {
                    epocError = KErrGeneral;
                    break;
                    }
                }
            }
        }
TFLOGSTRING2("TSY: CMmStaticUtility::PacketDataCSCauseToEpocError - epoc error to be returned: %d", epocError);
OstTrace1( TRACE_NORMAL,  DUP1_CMMSTATICUTILITY_PACKETDATACSCAUSETOEPOCERROR_TD, "CMmStaticUtility::PacketDataCSCauseToEpocError;epocError=%d", epocError );

    return epocError;
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::MapPdpTypeToClient
// Description: This function maps the Pdp type from Gpds server type
// to PacketAPI type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::MapPdpTypeToClient
        (
        RPacketContext::TProtocolType& aPdpTypeClient, // Pdp address type
        TUint8 aPdpTypeServer // Pdp address type
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::MapPdpTypeToClient");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_MAPPDPTYPETOCLIENT_TD, "CMmStaticUtility::MapPdpTypeToClient" );

    if ( GPDS_PDP_TYPE_PPP == aPdpTypeServer )
        {
        aPdpTypeClient = RPacketContext::EPdpTypePPP;
        }
    else if ( GPDS_PDP_TYPE_IPV4 == aPdpTypeServer )
        {
        aPdpTypeClient = RPacketContext::EPdpTypeIPv4;
        }
    else if ( GPDS_PDP_TYPE_IPV6 == aPdpTypeServer )
        {
        aPdpTypeClient = RPacketContext::EPdpTypeIPv6;
        }
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::ConvertIPAddressToClient
// Converts GPDS IP address to Client format suitable to be used
// with TInetAddr::Input. IPv4 and IPv6 are supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::ConvertIPAddressToClient
        (
        const TDesC8& aGPDSAddres,
        TDes8& aInetAddr
        )
    {
    TFLOGSTRING2("TSY: CMmStaticUtility::ConvertIPAddressToClient - %S", &aGPDSAddres);
    OstTraceExt1( TRACE_NORMAL,  CMMSTATICUTILITY_CONVERTIPADDRESSTOCLIENT_TD, "CMmStaticUtility::ConvertIPAddressToClient;aGPDSAddress=%s", aGPDSAddres );

    aInetAddr.Zero();

    // The length of IPv4 address is 4
    // Output format is 32-bit decimal number (eg. 3232245764 )
    if ( KIpv4AddressLen == aGPDSAddres.Length() )
        {
        // Numbers 0...4 are the element numbers of the table pdpAddress
        TUint32 inetAddr( INET_ADDR( aGPDSAddres[0],
            aGPDSAddres[1],
            aGPDSAddres[2],
            aGPDSAddres[3] ) );

        aInetAddr.AppendNum( inetAddr );
        TFLOGSTRING2("TSY: CMmStaticUtility::ConvertIPAddressToClient: ClientIP=%S", &aInetAddr);
        }
    // The length of IPv6 address is 16
    // Output format is 8 16-bit hex numbers
    // separated by ':' (eg.2001:618:400:6a:0:0:0:abc )
    else if ( KIpv6AddressLen == aGPDSAddres.Length() )
        {
        TIp6Addr ipv6Address;

        // change endianness
        for ( TInt i = 0; i < KIpv6AddressLen; i += 2 )
            {
            ipv6Address.u.iAddr8[i] = static_cast<TUint8>(
                aGPDSAddres[i + 1] );
            ipv6Address.u.iAddr8[i + 1] = static_cast<TUint8>(
                aGPDSAddres[i] );
            }

        for ( TInt i = 0; i < KIpv6AddressLen/2 - 1; i++ )
            {
            aInetAddr.AppendNum(
                ipv6Address.u.iAddr16[i],
                EHex );
            aInetAddr.Append( KClientIPv6AddressDigitsSeparator );
            }
        // appending last digit that is not followed by separator
        aInetAddr.AppendNum(
            ipv6Address.u.iAddr16[KIpv6AddressLen/2 - 1],
            EHex );
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::ConvertIPAddressFromClient
// Converts IP address string from client into GPDS suitable format
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmStaticUtility::ConvertIPAddressFromClient
        (
        const TDesC8& aClientAddr,
        TDes8& aGpdsAddr
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::ConvertIPAddressFromClient");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_CONVERTIPADDRESSFROMCLIENT_TD, "CMmStaticUtility::ConvertIPAddressFromClient" );
    TInt ret(KErrNone);

    TBuf<RPacketContext::KMaxPDPAddressLength> tmpClientAddress;
    tmpClientAddress.Copy(aClientAddr);
    TInetAddr inetAddr;
    ret = inetAddr.Input(tmpClientAddress);

    if ( KErrNone == ret )
        {
        if ( KAfInet == inetAddr.Family() )
            {
            TUint32 ipV4Address = inetAddr.Address();
            TPtrC8 ptrIpV4Address( (TUint8*)&ipV4Address, KIpv4AddressLen );
            aGpdsAddr.SetLength( KIpv4AddressLen );
            // reverse copy IP address bytes to get correct endianness
            for ( TInt i=0; i<KIpv4AddressLen; i++)
                {
                aGpdsAddr[i] = ptrIpV4Address[KIpv4AddressLen - i - 1];
                }
            }
        else if ( KAfInet6 == inetAddr.Family() )
            {
            TPtrC8 ptrIpV6Address(
                inetAddr.Ip6Address().u.iAddr8,
                KIpv6AddressLen );
            aGpdsAddr.Copy( ptrIpV6Address );
            }
        else
            {
TFLOGSTRING2("TSY: CMmStaticUtility::ConvertIPAddressFromClient; not supported family(%d)", inetAddr.Family());
OstTrace1( TRACE_NORMAL,  DUP2_CMMSTATICUTILITY_CONVERTIPADDRESSFROMCLIENT_TD, "CMmStaticUtility::ConvertIPAddressFromClient; not supported family(%d)", inetAddr.Family() );
            ret = KErrNotSupported;
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmStaticUtility::ConvertIPAddressFromClient; TInetAddr::Input failed");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSTATICUTILITY_CONVERTIPADDRESSFROMCLIENT_TD, "CMmStaticUtility::ConvertIPAddressFromClient; TInetAddr::Input failed" );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::GetIntFromDescriptor
// Converts number string to integer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::GetIntFromDescriptor
        (
        TUint& integer, // Integer value
        TDesC& buffer // Buffer containing the number string
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::GetIntFromDescriptor");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_GETINTFROMDESCRIPTOR_TD, "CMmStaticUtility::GetIntFromDescriptor" );

    // Initialize.
    integer = 0;

    // Loop all the numbers through
    for ( TInt i = 0; i < buffer.Length(); i++ )
        {
        integer = ( integer * 10 ) + ( buffer[ i ] - '0' );
        }
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::FormOperatorCode
// Extracts MNC codes from operator code.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::FormOperatorCode
        (
        TBuf8<3>& aOperatorCode, // Buffer for MNC and MCC code
        const TBuf<4>* aMnc, // Pointer to MNC code
        const TBuf<3>* aMcc // pointer to MCC code
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::FormOperatorCode");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_FORMOPERATORCODE_TD, "CMmStaticUtility::FormOperatorCode" );

    TUint16 mnc1 = 0x0F;
    TUint16 mnc2 = 0x0F;
    TUint16 mnc3 = 0x0F;
    // Extract MNC codes (it is in unicode form)
    if ( 0 < aMnc->Length() )
        {
        TPtrC16 mnc1unicode = aMnc->Left( 1 );
        // Transfrom unicode to 8 bit by converting it first to 16 bit
        mnc1 = (TUint16)( *mnc1unicode.Ptr() );
        // Mask the 4 low-bits and 0000000000001111 to get the mnc numbers.
        mnc1 = (TUint16)( mnc1 & 0x000F );
        // In case we have only one digit, we need to add '0' as a second digit.
        // This is possible if operator has not configured network properly
        // and we receive for example 7 when it should be 07. This information
        // is then passed to client which also has then faulty information
        // and we need to fix that before requesting network selection.
        if ( 1 == aMnc->Length() )
            {
            // Store the single MNC as second digit
            mnc2 = mnc1;
            // Set the first digit to zero, this will create MNC like 07 or 05
            mnc1 = 0x00;
            }
        if ( 1 < aMnc->Length() )
            {
            TPtrC16 mnc2unicode = aMnc->Mid( 1,1 );
            mnc2 = (TUint16)( *mnc2unicode.Ptr() );
            mnc2 = (TUint16)( mnc2 & 0x000F );
            }
        if ( 2 < aMnc->Length() )
            {
            TPtrC16 mnc3unicode = aMnc->Mid( 2,1 );
            mnc3 = (TUint16)( *mnc3unicode.Ptr() );
            mnc3 = (TUint16)( mnc3 & 0x000F );
            }
        }
    TUint16 mcc1 = 0x0F;
    TUint16 mcc2 = 0x0F;
    TUint16 mcc3 = 0x0F;

    // Extract MCC codes (it is in unicode form)
    if ( 0 < aMcc->Length() )
        {
        TPtrC16 mcc1unicode = aMcc->Left( 1 );
        // Transfrom unicode to 8 bit by converting it first to 16 bit
        mcc1 = (TUint16)( *mcc1unicode.Ptr() );
        // Mask the 4 low-bits and 0000000000001111 to get the mcc numbers.
        mcc1 = (TUint16)( mcc1 & 0x000F );
        // As with MNC, MCC also needs to make sure the code is correct.
        if ( 1 == aMcc->Length() )
            {
            // Store the sinlge MCC as third digit
            mcc3 = mcc1;
            // Set the first and second digits to zero,
            // this will create MCC like "00 X7 XX" or "00 X5 XX".
            mcc2 = 0x00;
            mcc1 = 0x00;
            }
        if ( 2 == aMcc->Length() )
            {
            // Store the first client digit as second MCC digit
            mcc2 = mcc1;

            // Set the first MCC digit to zero
            mcc1 = 0x00;

            // Store the second client digit as third MCC digit
            TPtrC16 mcc2unicode = aMcc->Mid( 1,1 );
            mcc3 = (TUint16)( *mcc2unicode.Ptr() );
            mcc3 = (TUint16)( mcc3 & 0x000F );
            }
        if ( 2 < aMcc->Length() )
            {
            // Store the second client digit as second MCC digit.
            // First digit is already set.
            TPtrC16 mcc2unicode = aMcc->Mid( 1,1 );
            mcc2 = (TUint16)( *mcc2unicode.Ptr() );

            // Store the third client digit as third MCC digit
            TPtrC16 mcc3unicode = aMcc->Mid( 2,1 );
            mcc3 = (TUint16)( *mcc3unicode.Ptr() );
            mcc3 = (TUint16)( mcc3 & 0x000F );
            }
        }
    // Mask the mcc2 and mcc1 to one byte (4 hi-bits mcc2, 4 low-bits mcc1)
    TUint8 mcc2_mcc1 = (TUint8)( ( mcc2 << 4 ) | ( mcc1 ) );
    // Mask the mnc3 and mcc3 to one byte (4 hi-bits mnc3, 4 low-bits mcc3)
    TUint8 mnc3_mcc3 = (TUint8)( ( mnc3 << 4 ) | ( mcc3 ) );
    // Mask the mnc2 and mnc1 to one byte (4 hi-bits mnc2, 4 low-bits mnc1)
    TUint8 mnc2_mnc1 = (TUint8)( ( mnc2 << 4 ) | ( mnc1 ) );

    aOperatorCode.Append( mcc2_mcc1 );
    aOperatorCode.Append( mnc3_mcc3 );
    aOperatorCode.Append( mnc2_mnc1 );
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::GetMccCodes
// Extracts MCC codes from operator code.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::GetMccCodes
        (
        TPtrC8 aOperatorCode, // Combined MCC and MNC
        TUint16* aMcc1, // 1st number of MCC code
        TUint16* aMcc2, // 2nd number of MCC code
        TUint16* aMcc3 // 3rd number of MCC code
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::GetMccCodes");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_GETMCCCODES_TD, "CMmStaticUtility::GetMccCodes" );

    // Extract MCC and MNC from aOperatorCode.
    TPtrC8 mcc2_mcc1 = aOperatorCode.Left( 0 );
    TPtrC8 mnc3_mcc3 = aOperatorCode.Mid( 1,1 );

    // Get the MCC3 and MCC2 code.
    *aMcc3 = (TUint16)( *mnc3_mcc3.Ptr() );
    *aMcc2 = (TUint16)( *mcc2_mcc1.Ptr() );

    // Mask the 4 low-bits and 0000000000001111 to get the mcc3 number.
    *aMcc3 = (TUint16)( *aMcc3 & 0x000F );

    // Mask the 4 hi-bits and 0000000011110000 to get the mcc2 number.
    *aMcc2 = (TUint16)( *aMcc2 & 0x00F0 );
    // Move the 4 hi-bits to low-bits,
    // mcc2 numbercan be read.
    *aMcc2 = (TUint16)( *aMcc2 >> 4 );

    // Get the MCC1 code
    *aMcc1 = (TUint16)( *mcc2_mcc1.Ptr() );
    // Mask the 4 hi-bits and 0000000000001111 to get the mcc1 number.
    *aMcc1 = (TUint16)( *aMcc1 & 0x000F );
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::GetMncCodes
// Extracts MNC codes from operator code.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::GetMncCodes
        (
        TPtrC8 aOperatorCode, // Combined MCC and MNC
        TUint16* aMnc1, // 1st number of MNC code
        TUint16* aMnc2, // 2nd number of MNC code
        TUint16* aMnc3 // 3rd number of MNC code
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::GetMncCodes");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_GETMNCCODES_TD, "CMmStaticUtility::GetMncCodes" );

    // Extract MCC and MNC from aOperatorCode.
    TPtrC8 mnc3_mcc3 = aOperatorCode.Mid( 1,1 );
    TPtrC8 mnc2_mnc1 = aOperatorCode.Mid( 2,1 );

    // Get the MNC3 and MNC2 code.
    *aMnc3 = (TUint16)( *mnc3_mcc3.Ptr() );
    *aMnc2 = (TUint16)( *mnc2_mnc1.Ptr() );

    // Mask the 4 hi-bits and 0000000011110000 to get the mnc3 number.
    *aMnc3 = (TUint16)( *aMnc3 & 0x00F0 );
    // Move the 4 hi-bits to low-bits
    // mnc3 number can be read.
    *aMnc3 = (TUint16)( *aMnc3 >> 4 );

    // Mask the 4 hi-bits and 0000000011110000 to get the mnc2 number.
    *aMnc2 = (TUint16)( *aMnc2 & 0x00F0 );
    // Move the 4 hi-bits to low-bits
    // mnc2 number can be read.
    *aMnc2 = (TUint16)( *aMnc2 >> 4 );

    // Get the MNC1 code.
    *aMnc1 = (TUint16)( *mnc2_mnc1.Ptr() );
    // Mask the 4 hi-bits and 0000000000001111 to get the mnc1 number.
    *aMnc1 = (TUint16)( *aMnc1 & 0x000F );
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::CalculateMccMncNumbers
// Calculates mcc and mcn numbers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::CalculateMccMncNumbers
        (
        TUint &aMccNumber, // mcc number
        TUint &aMncNumber, // mnc number
        TUint16 aMcc1, // first mcc digit
        TUint16 aMcc2, // second mcc digit
        TUint16 aMcc3, // third mcc digit
        TUint16 aMnc1, // first mnc digit
        TUint16 aMnc2, // second mnc digit
        TUint16 aMnc3 // third mnc digit
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::CalculateMccMncNumbers");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_CALCULATEMCCMNCNUMBERS_TD, "CMmStaticUtility::CalculateMccMncNumbers" );

    // Mcc is always three digits
    // If mcc is bigger than 10, code is invalid
    if ( 10 > aMcc1 )
        {
        aMccNumber = 100 * aMcc1 + 10 * aMcc2 + aMcc3;
        }

    // only mnc digits below 10 are valid
    if ( 10 > aMnc1 )
        {
        aMncNumber = 100 * aMnc1;
        }

    if ( 10 > aMnc2 )
        {
        aMncNumber = aMncNumber + ( 10 * aMnc2 );
        }
    else
        {
        // if second mnc digit not found whole mnc number will be one digit
        aMncNumber = aMncNumber / 10;
        }

    if ( 10 > aMnc3 )
        {
        aMncNumber = aMncNumber + aMnc3;
        }
    else if ( 0x0F == aMnc3 &&
        ( 302 == aMccNumber ||
        310 == aMccNumber ||
        311 == aMccNumber ||
        312 == aMccNumber ||
        313 == aMccNumber ||
        314 == aMccNumber ||
        315 == aMccNumber ||
        316 == aMccNumber ) )
        {
        aMncNumber = aMncNumber + 0;
        }
    else
        {
        // if third mnc digit not found whole mnc number will be two digits
        aMncNumber = aMncNumber / 10;
        }
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::IsOperinfoApac
// Returns ETrue if operinfo apac is in use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMmStaticUtility::IsOperinfoApac()
    {
TFLOGSTRING("TSY: CMmStaticUtility::IsOperinfoApac");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_ISOPERINFOAPAC_TD, "CMmStaticUtility::IsOperinfoApac" );

#ifdef __WINSCW__
    return apacInfoToBeReturned;
#else
    TBool ret( EFalse );

    switch ( User::Language() )
        {
        case ELangTaiwanChinese:
        case ELangHongKongChinese:
        case ELangPrcChinese:
            {
TFLOGSTRING("TSY: CMmStaticUtility::IsOperinfoApac, Eastern");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSTATICUTILITY_ISOPERINFOAPAC_TD, "CMmStaticUtility::IsOperinfoApac, Eastern" );
            ret = ETrue;
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmStaticUtility::IsOperinfoApac, Western");
OstTrace0( TRACE_NORMAL,  DUP2_CMMSTATICUTILITY_ISOPERINFOAPAC_TD, "CMmStaticUtility::IsOperinfoApac, Western" );
            // already set to ret = EFalse;
            break;
            }
        }
    return ret;

#endif // __WINSCW__
    }

// ----------------------------------------------------------------------------
// CMmStaticUtility::ProcessIfIdentityServices
// Checks if this is related to identity services
// -----------------------------------------------------------------------------
//
TInt CMmStaticUtility::MapSw1Sw2ToEpocError( 
    const TUint8 aSw1, 
    const TUint8 aSw2, 
    const TUint8 aResult )
    {
TFLOGSTRING4("TSY:CMmSupplServMessHandler::MapSw1Sw2ToEpocError: sw1: 0x%x sw2: 0x%x result: 0x%x", aSw1, aSw2, aResult );

    TInt ret( KErrGeneral );

    //These are the ok responses ( for envelope )
    //-90 00 normal ending of the command
    //-91 XX normal ending of the command with extra information from the
    // proactive SIM containing a command for ME length XX of response data
    //-9E XX length XX of the response data given in case of a SIM data

    // download error
    //-9F XX length XX of the response data
    //This is the busy response
    //-93 00 SAT is busy. Command cannot be executed at present, further
    // normal commands are allowed
    //This is the memory status
    //-92 0X command succesful but after using and internal update retry
    // routine x times
    //-92 40 memory problem
    //Application independent errors
    //-67 XX incorrect parameter P3
    //-6B XX incorrect parameter P1 or P2
    //-6E XX wrong instruction class given in the command
    //-6F XX technical problem with no diagnostic given.
    //Also possible
    //-94 02 P1 or P2 is caused by the addressed record being out of range

    switch( aSw1 )
        {
        case 0x90:
        case 0x91:
        case 0x9F:
            {
            if( KNotAllowed == aResult )
                {
                ret = CMmStaticUtility::EpocErrorCode( 
                    KErrGeneral,
                    KErrGsmCCCallRejected );
                }
            else if( KAllowedWithModifications == aResult )
                {
                ret = CMmStaticUtility::EpocErrorCode( 
                    KErrAccessDenied,
                    KErrSatControl );
                }
            break;
            }
        case 0x93:
            {
            if( 0x00 == aSw2 )
                {
                ret = CMmStaticUtility::EpocErrorCode( 
                    KErrAccessDenied,
                    KErrSatBusy );
                }
            break;
            }
        case 0x6F:
            {
            ret = CMmStaticUtility::EpocErrorCode( 
                KErrGeneral,
                KErrGsmCallControlBase );
            break;
            }
        default:
            {
TFLOGSTRING("TSY:CMmSupplServMessHandler::MapSw1Sw2ToEpocError: default");
            ret = CMmStaticUtility::EpocErrorCode( 
                KErrGeneral,
                KErrGsmCallControlBase );
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmStaticUtility::SetOperinfoApac
// Sets internal oper info value. THIS IS ONLY FOR WINSCW USE
// -----------------------------------------------------------------------------
//
#ifdef __WINSCW__

EXPORT_C void CMmStaticUtility::SetOperinfoApac
        (
        TBool aApac
        )
    {
TFLOGSTRING("TSY: CMmStaticUtility::SetOperinfoApac");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_SETOPERINFOAPAC_TD, "CMmStaticUtility::SetOperinfoApac" );
    apacInfoToBeReturned = aApac;
    }

#endif // __WINSCW__


// -----------------------------------------------------------------------------
// CMmStaticUtility::ConvertGsmDataToUcs2
// Convert data to UCS format. See ETSI TS 102.221 Annex A
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::ConvertGsmDataToUcs2(
    TDes8& aInputString,
    TUint16 aLength,
    TDes8& aUcs2String )
    {
TFLOGSTRING("TSY: CMmStaticUtility::ConvertGsmDataToUcs2");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_CONVERTGSMDATATOUCS2_TD, "CMmStaticUtility::ConvertGsmDataToUcs2" );

    TInt i( 0 );
    TInt j( 0 );
    TInt pos( 0 );
    TUint16 basePointer16( 0 );
    TUint16 ucs2Char( 0 );
    TUint8 numOfChars( 0 );

    if ( 0 < aLength )
        {
        // Check the encoding type
        switch ( aInputString[0] )
            {
            case 0x80: // 16 bit UCS2, unused bytes set to 0xFF
                {
                // Search for the termination mark
                pos = aInputString.Locate( 0xFF );
                // End mark was not found, copy all the data
                if ( KErrNotFound == pos )
                    {
                    pos = aLength;
                    }
                // Decrease by one for not copying end mark
                if ( 0 < pos )
                    {
                    pos--;
                    }
                // Copy data, skip 1st byte
                aUcs2String.Copy( aInputString.Mid( 1, pos ) );
                break;
                }
            case 0x81: // 16 bits UCS
                {
                numOfChars = aInputString[1];
                // 3rd byte means bits 15 to 8 for base pointer
                TUint8 basePointerMsb( aInputString[2]>> 1 );
                TUint8 basePointerLsb( ( aInputString[2] & 1 ) << 7 );
                basePointer16 = ( basePointerMsb << 8 )| basePointerLsb;

                // Loop all characters and decide if GSM default or UCS2
                for ( i = 0, j = 3; i < numOfChars; i++, j++ )
                    {
                    if ( 0 == ( aInputString[j] & 0x80 ) ) // 7 bit default GSM
                        {
                        ucs2Char = GetUnicodeGsm( aInputString[j] );
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char >> 8 ) ); // MSB bytes
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char & 0x00FF ) ); // LSB bytes
                        }
                    else // Offset to be added to base pointer
                        {
                        ucs2Char = basePointer16 | ( aInputString[j] & 0x7F );
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char >> 8 ) ); // MSB bytes
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char & 0x00FF )  ); // LSB bytes
                        }
                    }
                break;
                }
            case 0x82:
                {
                numOfChars = aInputString[1];
                // Base pointer is octets 3 and 4
                basePointer16 = ( aInputString[2] << 8 ) | aInputString[3];

                // Loop all characters and decide if GSM default or UCS2
                for ( i = 0, j = 4; i < numOfChars; i++, j++ )
                    {
                    if ( 0 == ( aInputString[j] & 0x80 ) ) // 7 bit default GSM
                        {
                        ucs2Char = GetUnicodeGsm( aInputString[j] );
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char >> 8 ) ); // MSB bytes
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char & 0x00FF )  ); // LSB bytes
                        }
                    else // Offset to be added to base pointer
                        {
                        ucs2Char = basePointer16 | ( aInputString[j] & 0x7F );
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char >> 8 ) ); // MSB bytes
                        aUcs2String.Append( static_cast<TUint8>
                            ( ucs2Char & 0x00FF )  ); // LSB bytes
                        }
                    }
                break;
                }
            default: // 7-bit default GSM
                {
                // Find the termination mark
                pos = aInputString.Locate( 0xFF );
                // End mark was not found, copy all the data
                if ( KErrNotFound == pos )
                    {
                    pos = aLength;
                    }
                for ( i = 0; i < pos; i++ )
                    {
                    ucs2Char = GetUnicodeGsm( aInputString[i] );
                    aUcs2String.Append( static_cast<TUint8>
                        ( ucs2Char >> 8 ) ); // MSB bytes
                    aUcs2String.Append( static_cast<TUint8>
                        ( ucs2Char & 0x00FF ) ); // LSB bytes
                    }
                break;
                }
            } // End of switch ( aInputString[0] )
        } // End of if ( 0 < aLength )
   } // End of function



// -----------------------------------------------------------------------------
// CMmStaticUtility::GetUnicodeGsm
// Convert character to UCS format
// -----------------------------------------------------------------------------
//
TUint16 CMmStaticUtility::GetUnicodeGsm( TUint8 aCharacter)
    {
TFLOGSTRING("TSY: CMmStaticUtility::GetUnicodeGsm");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_GETUNICODEGSM_TD, "CMmStaticUtility::GetUnicodeGsm" );

    TUint16 ret( 0 );

    if ( 0x80 < aCharacter )
        {
        aCharacter = aCharacter & 0x7f;
        }

    switch ( aCharacter )
        {
        case 0x00:
            {
            ret = 0x0040;
            break;
            }
        case 0x01:
            {
            ret = 0x00A3;
            break;
            }
        case 0x02:
            {
            ret = 0x0024;
            break;
            }
        case 0x03:
            {
            ret = 0x00A5;
            break;
            }
        case 0x04:
            {
            ret = 0x00E8;
            break;
            }
        case 0x05:
            {
            ret = 0x00E9;
            break;
            }
        case 0x06:
            {
            ret = 0x00F;
            break;
            };
        case 0x07:
            {
            ret = 0x00EC;
            break;
            }
        case 0x08:
            {
            ret = 0x00F2;
            break;
            }
        case 0x09:
            {
            ret = 0x00C7;
            break;
            }
        case 0x0A:
            {
            ret = 0x000A;
            break;
            }
        case 0x0B:
            {
            ret = 0x00D8;
            break;
            }
        case 0x0C:
            {
            ret = 0x00F8;
            break;
            }
        case 0x0D:
            {
            ret = 0x000D;
            break;
            }
        case 0x0E:
            {
            ret = 0x00C5;
            break;
            }
        case 0x0F:
            {
            ret = 0x00E5;
            break;
            }
        case 0x10:
            {
            ret = 0x0394;
            break;
            }
        case 0x11:
            {
            ret = 0x005F;
            break;
            }
        case 0x12:
            {
            ret = 0x03A6;
            break;
            }
        case 0x13:
            {
            ret = 0x0393;
            break;
            }
        case 0x14:
            {
            ret = 0x039B;
            break;
            }
        case 0x15:
            {
            ret = 0x03A9;
            break;
            }
        case 0x16:
            {
            ret = 0x03A0;
            break;
            }
        case 0x17:
            {
            ret = 0x03A8;
            break;
            }
        case 0x18:
            {
            ret = 0x03A3;
            break;
            }
        case 0x19:
            {
            ret = 0x0398;
            break;
            }
        case 0x1A:
            {
            ret = 0x039E;
            break;
            }
        case 0x1C:
            {
            ret = 0x00C6;
            break;
            }
        case 0x1D:
            {
            ret = 0x00E6;
            break;
            }
        case 0x1E:
            {
            ret = 0x00DF;
            break;
            }
        case 0x1F:
            {
            ret = 0x00C9;
            break;
            }
        case 0x20:
            {
            ret = 0x0020;
            break;
            }
        case 0x21:
            {
            ret = 0x0021;
            break;
            }
        case 0x22:
            {
            ret = 0x0022;
            break;
            }
        case 0x23:
            {
            ret = 0x0023;
            break;
            }
        case 0x24:
            {
            ret = 0x00A4;
            break;
            }
        case 0x25:
            {
            ret = 0x0025;
            break;
            }
        case 0x26:
            {
            ret = 0x0026;
            break;
            }
        case 0x27:
            {
            ret = 0x0027;
            break;
            }
        case 0x28:
            {
            ret = 0x0028;
            break;
            }
        case 0x29:
            {
            ret = 0x0029;
            break;
            }
        case 0x2A:
            {
            ret = 0x002A;
            break;
            }
        case 0x2B:
            {
            ret = 0x002B;
            break;
            }
        case 0x2C:
            {
            ret = 0x002C;
            break;
            }
        case 0x2D:
            {
            ret = 0x002D;
            break;
            }
        case 0x2E:
            {
            ret = 0x002E;
            break;
            }
        case 0x2F:
            {
            ret = 0x002F;
            break;
            }
        case 0x30:
            {
            ret = 0x0030;
            break;
            }
        case 0x31:
            {
            ret = 0x0031;
            break;
            }
        case 0x32:
            {
            ret = 0x0032;
            break;
            }
        case 0x33:
            {
            ret = 0x0033;
            break;
            }
        case 0x34:
            {
            ret = 0x0034;
            break;
            }
        case 0x35:
            {
            ret = 0x0035;
            break;
            }
        case 0x36:
            {
            ret = 0x0036;
            break;
            }
        case 0x37:
            {
            ret = 0x0037;
            break;
            }
        case 0x38:
            {
            ret = 0x0038;
            break;
            }
        case 0x39:
            {
            ret = 0x0039;
            break;
            }
        case 0x3A:
            {
            ret = 0x003A;
            break;
            }
        case 0x3B:
            {
            ret = 0x003B;
            break;
            }
        case 0x3C:
            {
            ret = 0x003C;
            break;
            }
        case 0x3D:
            {
            ret = 0x003D;
            break;
            }
        case 0x3E:
            {
            ret = 0x003E;
            break;
            }
        case 0x3F:
            {
            ret = 0x003F;
            break;
            }
        case 0x40:
            {
            ret = 0x00A1;
            break;
            }
        case 0x41:
            {
            ret = 0x0041;
            break;
            }
        case 0x42:
            {
            ret = 0x0042;
            break;
            }
        case 0x43:
            {
            ret = 0x0043;
            break;
            }
        case 0x44:
            {
            ret = 0x0044;
            break;
            }
        case 0x45:
            {
            ret = 0x0045;
            break;
            }
        case 0x46:
            {
            ret = 0x0046;
            break;
            }
        case 0x47:
            {
            ret = 0x0047;
            break;
            }
        case 0x48:
            {
            ret = 0x0048;
            break;
            }
        case 0x49:
            {
            ret = 0x0049;
            break;
            }
        case 0x4A:
            {
            ret = 0x004A;
            break;
            }
        case 0x4B:
            {
            ret = 0x004B;
            break;
            }
        case 0x4C:
            {
            ret = 0x004C;
            break;
            }
        case 0x4D:
            {
            ret = 0x004D;
            break;
            }
        case 0x4E:
            {
            ret = 0x004E;
            break;
            }
        case 0x4F:
            {
            ret = 0x004F;
            break;
            }
        case 0x50:
            {
            ret = 0x0050;
            break;
            }
        case 0x51:
            {
            ret = 0x0051;
            break;
            }
        case 0x52:
            {
            ret = 0x0052;
            break;
            }
        case 0x53:
            {
            ret = 0x0053;
            break;
            }
        case 0x54:
            {
            ret = 0x0054;
            break;
            }
        case 0x55:
            {
            ret = 0x0055;
            break;
            }
        case 0x56:
            {
            ret = 0x0056;
            break;
            }
        case 0x57:
            {
            ret = 0x0057;
            break;
            }
        case 0x58:
            {
            ret = 0x0058;
            break;
            }
        case 0x59:
            {
            ret = 0x0059;
            break;
            }
        case 0x5A:
            {
            ret = 0x005A;
            break;
            }
        case 0x5B:
            {
            ret = 0x00C4;
            break;
            }
        case 0x5C:
            {
            ret = 0x00D6;
            break;
            }
        case 0x5D:
            {
            ret = 0x00D1;
            break;
            }
        case 0x5E:
            {
            ret = 0x00DC;
            break;
            }
        case 0x5F:
            {
            ret = 0x00A7;
            break;
            }
        case 0x60:
            {
            ret = 0x00BF;
            break;
            }
        case 0x61:
            {
            ret = 0x0061;
            break;
            }
        case 0x62:
            {
            ret = 0x0062;
            break;
            }
        case 0x63:
            {
            ret = 0x0063;
            break;
            }
        case 0x64:
            {
            ret = 0x0064;
            break;
            }
        case 0x65:
            {
            ret = 0x0065;
            break;
            }
        case 0x66:
            {
            ret = 0x0066;
            break;
            }
        case 0x67:
            {
            ret = 0x0067;
            break;
            }
        case 0x68:
            {
            ret = 0x0068;
            break;
            }
        case 0x69:
            {
            ret = 0x0069;
            break;
            }
        case 0x6A:
            {
            ret = 0x006A;
            break;
            }
        case 0x6B:
            {
            ret = 0x006B;
            break;
            }
        case 0x6C:
            {
            ret = 0x006C;
            break;
            }
        case 0x6D:
            {
            ret = 0x006D;
            break;
            }
        case 0x6E:
            {
            ret = 0x006E;
            break;
            }
        case 0x6F:
            {
            ret = 0x006F;
            break;
            }
        case 0x70:
            {
            ret = 0x0070;
            break;
            }
        case 0x71:
            {
            ret = 0x0071;
            break;
            }
        case 0x72:
            {
            ret = 0x0072;
            break;
            }
        case 0x73:
            {
            ret = 0x0073;
            break;
            }
        case 0x74:
            {
            ret = 0x0074;
            break;
            }
        case 0x75:
            {
            ret = 0x0075;
            break;
            }
        case 0x76:
            {
            ret = 0x0076;
            break;
            }
        case 0x77:
            {
            ret = 0x0077;
            break;
            }
        case 0x78:
            {
            ret = 0x0078;
            break;
            }
        case 0x79:
            {
            ret = 0x0079;
            break;
            }
        case 0x7A:
            {
            ret = 0x007A;
            break;
            }
        case 0x7B:
            {
            ret = 0x00E4;
            break;
            }
        case 0x7C:
            {
            ret = 0x00F6;
            break;
            }
        case 0x7D:
            {
            ret = 0x00F1;
            break;
            }
        case 0x7E:
            {
            ret = 0x00FC;
            break;
            }
        case 0x7F:
            {
            ret = 0x00E0;
            break;
            }
        case 0x94:
            {
            ret = 0x005E;  /* ^ */
            break;
            }
        case 0x9B:
            {
            ret = 0x0020; /* extension table mark as space*/
            break;
            }
        case 0xA8:
            {
            ret = 0x007B; /* { */
            break;
            }
        case 0xA9:
            {
            ret = 0x007D; /* } */
            break;
            }
        case 0xAF:
            {
            ret = 0x005C; /* \ */
            break;
            }
        case 0xBD:
            {
            ret = 0x007E; /* ~ */
            break;
            }
        case 0xBC:
            {
            ret = 0x005B; /* [ */
            break;
            }
        case 0xBE:
            {
            ret = 0x005D; /* ] */
            break;
            }
        case 0xC0:
            {
            ret = 0x007C; /* | */
            break;
            }
        case 0xE5:
            {
            ret = 0x20AC; /* The euro character */
            break;
            }
        default:
            {
            ret = 0x0020; /* If illegal gsm char return space */
            break;
            }
       }

    return ret;
    }


// -----------------------------------------------------------------------------
// CMmStaticUtility::ConvertUcs2To7BitCodedData
// Convert data to 7 bit GSM format.
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::ConvertUcs2To7BitCodedData(
    TDesC16& aInputString,
    TDes8& aGsmDataString )
    {
TFLOGSTRING("TSY: CMmStaticUtility::ConvertUcs2To7BitCodedData");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_CONVERTUCS2TO7BITCODEDDATA_TD, "CMmStaticUtility::ConvertUcs2To7BitCodedData" );

    if ( 0 < aInputString.Length() )
        {
        // Store string in GSM default coding scheme
        for( TInt count = 0; count < aInputString.Length(); count++)
            {
            // get the GSM defalut Character for UCS character
            TUint8 gsmChar = GetGsmForUnicode( aInputString[count] );
            if( gsmChar > 0x7F )
                {
                aGsmDataString.Append( 0x1B );
                }
            aGsmDataString.Append(gsmChar & 0x7F);
            }
            // Append 0xFF for End of the String
            aGsmDataString.Append( 0xFF );
        }   // End of if there is no Input String
    } // End of function



// -----------------------------------------------------------------------------
// CMmStaticUtility::ConvertUcs2ToGsmUcs2Data
// Convert data to UCS2 format. See ETSI TS 102.221 Annex A
// -----------------------------------------------------------------------------
//
void CMmStaticUtility::ConvertUcs2ToGsmUcs2Data(
    TDesC16& aInputString,
    TDes8& aGsmDataString )
    {
TFLOGSTRING("TSY: CMmStaticUtility::ConvertUcs2ToGsmData");
OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_CONVERTUCS2TOGSMDATA_TD, "CMmStaticUtility::ConvertUcs2ToGsmData" );

    if ( 0 < aInputString.Length() )
        {
        // Check if there is any GSM default alphabet
        if( GetCodingScheme( aInputString ))
            {
            // Store as one of the UCS2 Coding scheme
            // Append the GSM data with 0x80
            aGsmDataString.Append( 0x80 );
            for( TInt count = 0; count < aInputString.Length(); count++)
                {
                // get MSB Byte in the first place
                aGsmDataString.Append( aInputString[count]>>8);
                aGsmDataString.Append( aInputString[count] );
                }
            }  // End of check the encoding type
        else
            {
            // Store string in GSM default coding scheme
            for( TInt count = 0; count < aInputString.Length(); count++)
                {
                // get the GSM defalut Character for UCS character
                TUint8 gsmChar = GetGsmForUnicode( aInputString[count] );
                if( gsmChar > 0x7F )
                    {
                    aGsmDataString.Append( 0x1B );
                    }
                aGsmDataString.Append(gsmChar & 0x7F);
                }
            // Append 0xFF for End of the String
            aGsmDataString.Append( 0xFF );
            }
        }   // End of if there is no Input String
    } // End of function


// -----------------------------------------------------------------------------
// CMmStaticUtility::GetCodingScheme
// GEt the Coding scheme GSM default or UCS2
// -----------------------------------------------------------------------------
//
TBool CMmStaticUtility::GetCodingScheme( TDesC16& aInputString )
    {
    TFLOGSTRING("TSY: CMmStaticUtility::GetCodingScheme");
    OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_GETCODINGSCHEME_TD, "CMmStaticUtility::GetCodingScheme" );
    TInt ret ( ETrue );
    TInt count(0);
    
    while( count < aInputString.Length() )
        {
        TUint16 unicodeChar = aInputString[count];
        TUint8 gsmCode = GetGsmForUnicode( unicodeChar );
        if( gsmCode > 0x80 )
            {
            ret = EFalse;
            }
        count++;
        }
    return ret;
    
    }



// -----------------------------------------------------------------------------
// CMmStaticUtility::GetGsmForUnicode
// convert from Unicode character to GSM unicode character
// -----------------------------------------------------------------------------
//
TUint8 CMmStaticUtility::GetGsmForUnicode( TUint16 aUniCode )
    {
    TUint8 ret ( 0 );
    switch ( aUniCode )
        {
        case 0x0040:
            {
            ret = 0x00;
            break;
            }
        case 0x00A3:
            {
            ret = 0x01;
            break;
            }
        case 0x0024:
            {
            ret = 0x02;
            break;
            }
        case 0x00A5:
            {
            ret = 0x03;
            break;
            }
        case 0x00E8:
            {
            ret = 0x04;
            break;
            }
        case 0x00E9:
            {
            ret = 0x05;
            break;
            }
        case 0x00F9:
            {
            ret = 0x06;
            break;
            };
        case 0x00EC:
            {
            ret = 0x07;
            break;
            }
        case 0x00F2:
            {
            ret = 0x08;
            break;
            }
        case 0x00C7:
            {
            ret = 0x09;
            break;
            }
        case 0x000A:
            {
            ret = 0x0A;
            break;
            }
        case 0x00D8:
            {
            ret = 0x0B;
            break;
            }
        case 0x00F8:
            {
            ret = 0x0C;
            break;
            }
        case 0x000D:
            {
            ret = 0x0D;
            break;
            }
        case 0x00C5:
            {
            ret = 0x0E;
            break;
            }
        case 0x00E5:
            {
            ret = 0x0F;
            break;
            }
        case 0x0394:
            {
            ret = 0x10;
            break;
            }
        case 0x005F:
            {
            ret = 0x11;
            break;
            }
        case 0x03A6:
            {
            ret = 0x12;
            break;
            }
        case 0x0393:
            {
            ret = 0x13;
            break;
            }
        case 0x039B:
            {
            ret = 0x14;
            break;
            }
        case 0x03A9:
            {
            ret = 0x15;
            break;
            }
        case 0x03A0:
            {
            ret = 0x16;
            break;
            }
        case 0x03A8:
            {
            ret = 0x17;
            break;
            }
        case 0x03A3:
            {
            ret = 0x18;
            break;
            }
        case 0x0398:
            {
            ret = 0x19;
            break;
            }
        case 0x039E:
            {
            ret = 0x1A;
            break;
            }
        case 0x00C6:
            {
            ret = 0x1C;
            break;
            }
        case 0x00E6:
            {
            ret = 0x1D;
            break;
            }
        case 0x00DF:
            {
            ret = 0x1E;
            break;
            }
        case 0x00C9:
            {
            ret = 0x1F;
            break;
            }
        case 0x0020:
            {
            ret = 0x20;
            break;
            }
        case 0x0021:
            {
            ret = 0x21;
            break;
            }
        case 0x0022:
            {
            ret = 0x22;
            break;
            }
        case 0x0023:
            {
            ret = 0x23;
            break;
            }
        case 0x00A4:
            {
            ret = 0x24;
            break;
            }
        case 0x0025:
            {
            ret = 0x25;
            break;
            }
        case 0x0026:
            {
            ret = 0x26;
            break;
            }
        case 0x0027:
            {
            ret = 0x27;
            break;
            }
        case 0x0028:
            {
            ret = 0x28;
            break;
            }
        case 0x0029:
            {
            ret = 0x29;
            break;
            }
        case 0x002A:
            {
            ret = 0x2A;
            break;
            }
        case 0x002B:
            {
            ret = 0x2B;
            break;
            }
        case 0x002C:
            {
            ret = 0x2C;
            break;
            }
        case 0x002D:
            {
            ret = 0x2D;
            break;
            }
        case 0x002E:
            {
            ret = 0x2E;
            break;
            }
        case 0x002F:
            {
            ret = 0x2F;
            break;
            }
        case 0x0030:
            {
            ret = 0x30;
            break;
            }
        case 0x0031:
            {
            ret = 0x31;
            break;
            }
        case 0x0032:
            {
            ret = 0x32;
            break;
            }
        case 0x0033:
            {
            ret = 0x33;
            break;
            }
        case 0x0034:
            {
            ret = 0x34;
            break;
            }
        case 0x0035:
            {
            ret = 0x35;
            break;
            }
        case 0x0036:
            {
            ret = 0x36;
            break;
            }
        case 0x0037:
            {
            ret = 0x37;
            break;
            }
        case 0x0038:
            {
            ret = 0x38;
            break;
            }
        case 0x0039:
            {
            ret = 0x39;
            break;
            }
        case 0x003A:
            {
            ret = 0x3A;
            break;
            }
        case 0x003B:
            {
            ret = 0x3B;
            break;
            }
        case 0x003C:
            {
            ret = 0x3C;
            break;
            }
        case 0x003D:
            {
            ret = 0X3D;
            break;
            }
        case 0x003E:
            {
            ret = 0X3E;
            break;
            }
        case 0x003F:
            {
            ret = 0X3F;
            break;
            }
        case 0x00A1:
            {
            ret = 0X40;
            break;
            }
        case 0x0041:
            {
            ret = 0X41;
            break;
            }
        case 0x0042:
            {
            ret = 0X42;
            break;
            }
        case 0x0043:
            {
            ret = 0X43;
            break;
            }
        case 0x0044:
            {
            ret = 0X44;
            break;
            }
        case 0x0045:
            {
            ret = 0X45;
            break;
            }
        case 0x0046:
            {
            ret = 0X46;
            break;
            }
        case 0x0047:
            {
            ret = 0X47;
            break;
            }
        case 0x0048:
            {
            ret = 0X48;
            break;
            }
        case 0x0049:
            {
            ret = 0X49;
            break;
            }
        case 0x004A:
            {
            ret = 0X4A;
            break;
            }
        case 0x004B:
            {
            ret = 0X4B;
            break;
            }
        case 0x004C:
            {
            ret = 0X4C;
            break;
            }
        case 0x004D:
            {
            ret = 0X4D;
            break;
            }
        case 0x004E:
            {
            ret = 0X4E;
            break;
            }
        case 0x004F:
            {
            ret = 0X4F;
            break;
            }
        case 0x0050:
            {
            ret = 0X50;
            break;
            }
        case 0x0051:
            {
            ret = 0X51;
            break;
            }
        case 0x0052:
            {
            ret = 0X52;
            break;
            }
        case 0x0053:
            {
            ret = 0X53;
            break;
            }
        case 0x0054:
            {
            ret = 0X54;
            break;
            }
        case 0x0055:
            {
            ret = 0X55;
            break;
            }
        case 0x0056:
            {
            ret = 0X56;
            break;
            }
        case 0x0057:
            {
            ret = 0X57;
            break;
            }
        case 0x0058:
            {
            ret = 0X58;
            break;
            }
        case 0x0059:
            {
            ret = 0X59;
            break;
            }
        case 0x005A:
            {
            ret = 0X5A;
            break;
            }
        case 0x00C4:
            {
            ret = 0X5B;
            break;
            }
        case 0x00D6:
            {
            ret = 0X5C;
            break;
            }
        case 0x00D1:
            {
            ret = 0X5D;
            break;
            }
        case 0x00DC:
            {
            ret = 0X5E;
            break;
            }
        case 0x00A7:
            {
            ret = 0X5F;
            break;
            }
        case 0x00BF:
            {
            ret = 0X60;
            break;
            }
        case 0x0061:
            {
            ret = 0X61;
            break;
            }
        case 0x0062:
            {
            ret = 0X62;
            break;
            }
        case 0x0063:
            {
            ret = 0X63;
            break;
            }
        case 0x0064:
            {
            ret = 0X64;
            break;
            }
        case 0x0065:
            {
            ret = 0X65;
            break;
            }
        case 0x0066:
            {
            ret = 0X66;
            break;
            }
        case 0x0067:
            {
            ret = 0X67;
            break;
            }
        case 0x0068:
            {
            ret = 0X68;
            break;
            }
        case 0x0069:
            {
            ret = 0X69;
            break;
            }
        case 0x006A:
            {
            ret = 0X6A;
            break;
            }
        case 0x006B:
            {
            ret = 0X6B;
            break;
            }
        case 0x006C:
            {
            ret = 0X6C;
            break;
            }
        case 0x006D:
            {
            ret = 0X6D;
            break;
            }
        case 0x006E:
            {
            ret = 0X6E;
            break;
            }
        case 0x006F:
            {
            ret = 0X6F;
            break;
            }
        case 0x0070:
            {
            ret = 0X70;
            break;
            }
        case 0x0071:
            {
            ret = 0X71;
            break;
            }
        case 0x0072:
            {
            ret = 0X72;
            break;
            }
        case 0x0073:
            {
            ret = 0X73;
            break;
            }
        case 0x0074:
            {
            ret = 0X74;
            break;
            }
        case 0x0075:
            {
            ret = 0X75;
            break;
            }
        case 0x0076:
            {
            ret = 0X76;
            break;
            }
        case 0x0077:
            {
            ret = 0X77;
            break;
            }
        case 0x0078:
            {
            ret = 0X78;
            break;
            }
        case 0x0079:
            {
            ret = 0X79;
            break;
            }
        case 0x007A:
            {
            ret = 0X7A;
            break;
            }
        case 0x00E4:
            {
            ret = 0X7B;
            break;
            }
        case 0x00F6:
            {
            ret = 0X7C;
            break;
            }
        case 0x00F1:
            {
            ret = 0X7D;
            break;
            }
        case 0x00FC:
            {
            ret = 0X7E;
            break;
            }
        case 0x00E0:
            {
            ret = 0X7F;
            break;
            }
        case 0x005E:
            {
            ret = 0X94;  /* ^ */
            break;
            }
        case 0x007B:
            {
            ret = 0XA8; /* { */
            break;
            }
        case 0x007D:
            {
            ret = 0XA9; /* } */
            break;
            }
        case 0x005C:
            {
            ret = 0XAF; /* \ */
            break;
            }
        case 0x007E:
            {
            ret = 0XBD; /* ~ */
            break;
            }
        case 0x005B:
            {
            ret = 0XBC; /* [ */
            break;
            }
        case 0x005D:
            {
            ret = 0XBE; /* ] */
            break;
            }
        case 0x007C:
            {
            ret = 0XC0; /* | */
            break;
            }
        case 0x20AC:
            {
            ret = 0XE5; /* The euro character */
            break;
            }
        default:
            {
            ret = 0X9B; /* If illegal gsm char return space */
            break;
            }
       }

    return ret;
    }
// -----------------------------------------------------------------------------
// CMmStaticUtility::Get16Bit
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmStaticUtility::Get16Bit
    (
    const TDesC8& aSource, //Source
    TInt aIndex            //Index
    )
 {
 TFLOGSTRING("TSY: CMmStaticUtility::Get16bit");
 OstTrace0( TRACE_NORMAL,  CMMSTATICUTILITY_GET16BIT_TD, "CMmStaticUtility::Get16Bit" );

 TInt target( 0 );

 if ( aIndex + 1 < aSource.Length() )
     {
     target = TUint16( aSource[aIndex] ) << 8;
     target = TUint16( target | aSource[aIndex + 1] );
     }
     
 return target;
 }



// ==================== OTHER EXPORTED FUNCTIONS ===============================
    //None

// End of File
