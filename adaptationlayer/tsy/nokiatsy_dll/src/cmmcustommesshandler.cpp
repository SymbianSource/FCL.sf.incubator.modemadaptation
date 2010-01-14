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

#include "cmmcustommesshandler.h"
#include "cmmsecuritymesshandler.h"
#include "cmmstaticutility.h"
#include "cmmnetmesshandler.h"
#include "cmmnetoperatornamehandler.h"
#include "cmmcallmesshandler.h"
#include "cmmphonetsender.h"
#include "cmmmessagerouter.h"
#include "tsylogger.h"

#include <tisi.h>
#include <call_modemisi.h>
#include <csdisi.h>
#include <gssisi.h>
#include <infoisi.h>
#include <net_modemisi.h>
#include <uiccisi.h>
#include <ss_wmisi.h>
// #include <permisi.h> To be done in CPS
#include <product_profile_definitions.h>

#include <ctsy/serviceapi/mmgsmwcdmautils.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include <ctsy/serviceapi/gsmerror.h>
#include <etelmmerr.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcustommesshandlerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
// hard-coded transaction id for ISI messages
const TUint8 KCustomTransId = 6;
// const TUint8 KCustomWimTransId = 7;

// Max length for HSXPA message request
const TUint KHSxPAMaxMessageSize = 2;

// Uicc Card Status Byte (hardcoded). Values are derived from APDU server
// interface documentation. Bitfield READER_STATUS_BYTE
const TUint8 KUiccCardStatusCardNotPresentOrPoweredOff = 0x10; //0001 0000
const TUint8 KUiccCardStatusCardPresentAndPoweredOn = 0xD0; //1101 0000 (0001 0000|0x40|0x80)

// Consts needed when constructing APDU's going to be sent to the UICC
const TUint8 KEvenInstructionCode = 0x88;
const TUint8 KOddInstructionCode  = 0x89;

const TUint8 KFirstBlockOfAuthenticationData         = 0x80;
const TUint8 KFirstBlockOfAuthenticationResponseData = 0xA0;

const TUint8 KGsmAuthenticationContext  = 0x80; // 1000 0000
const TUint8 K3GAuthenticationContext   = 0x81; // 1000 0001
const TUint8 KGBAAuthenticationContext  = 0x84; // 1000 0100
const TUint8 KMBMSAuthenticationContext = 0x85; // 1000 0101

const TUint8 KMaximumLenOfDataExpected = 0x00;
const TUint8 KRunGsmAlgorithmRespLen   = 0x0C;
const TUint8 KClaNoSm = 0x00;
const TUint8 KClaIcc  = 0xA0;

const TUint8 KSw1Position = 0x02;
const TUint8 KSw2Position = 0x01;

const TUint8 KNormalCommandEnding                     = 0x00;
const TUint8 KAppAuthErrorIncorrectMac                = 0x01;
const TUint8 KCmdNotAllowedConditionsNotSatisfied     = 0x02;
const TUint8 KWarningAuthRespAvailable                = 0x03;
const TUint8 KWarningMoreDataAvailable                = 0x04;
const TUint8 KAppErrorAuthMbmsKeyFresh                = 0x05;
const TUint8 KAppErrorAuthMbmsOutOfMemMsk             = 0x06;
const TUint8 KAppErrorAuthMbmsOutOfMemMuk             = 0x07;
const TUint8 KWrongParametersDataNotFound             = 0x08;
const TUint8 KCmdNotAllowedSecurityStatusNotSatisfied = 0x09;
const TUint8 KUnknownCommandEnding                    = 0xFF;

const TUint8 KApduOk  = 0x00;
const TUint8 KApduNok = 0x01;

const TUint8 KMinLenOfRes     = 0x04;
const TUint8 KMaxLenOfRes     = 0x10;
const TUint8 KLenOfSRes       = 0x04;
const TUint8 KLenOfIk         = 0x10;
const TUint8 KLenOfCk         = 0x10;
const TUint8 KLenOfKc         = 0x08;
const TUint8 KLenOfAuts       = 0x0E;
const TUint8 KMaxMbmsMikeyLen = 0xFB;
const TUint8 KMaxMbmsSaltLen  = 0xFB;
const TUint8 KMaxApduSize     = 0xFF;
const TUint8 KMaxParentalRatingDataSize = 0x02;
const TUint8 KMaxSpeSize                = 0x01;

const TUint8 KSuccessfull3GAuthTag    = 0xDB;
const TUint8 KSyncFailureTag          = 0xDC;
const TUint8 KGBABootstappingModeTag  = 0xDD;
const TUint8 KGBANAFDerivationModeTag = 0xDE;
const TUint8 KMBMSDataObjectTag       = 0x53;

const TUint8 KMskUpdateMode                 = 0x01;
const TUint8 KMtkGenerationMode             = 0x02;
const TUint8 KMskDeletionMode               = 0x03;
const TUint8 KSuccessfullMbmsOperationTag   = 0xDB;
const TUint8 KMbmsOperationResponseTag53    = 0x53;
const TUint8 KMbmsOperationResponseTag73    = 0x73;
const TUint8 KOMABcastOperationResponseTag  = 0xAE;
const TUint8 KBcastManagementDataTag = 0x80;
const TUint8 KParentalRatingDataTag  = 0x8A;
const TUint8 KSPETypeNotSupportedTag = 0x8B;
const TUint8 KMikeyMessageTag        = 0x8C;
const TUint8 KTekDataTag             = 0x86;
const TUint8 KSaltDataTag            = 0x87;
const TUint8 KParentalControlTag     = 0x88;

const TUint8 KServiceGBA          = 0x44; // service no 68
const TUint8 KServiceMBMSsecurity = 0x45; // service no 69


// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// ============================= LOCAL FUNCTIONS ===============================
    //None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CMmCustomMessHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmCustomMessHandler::CMmCustomMessHandler()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::CMmCustomMessHandler.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CMMCUSTOMMESSHANDLER, "CMmCustomMessHandler::CMmCustomMessHandler" );
    iSimCBTopicToBeDeleted = KUnusedCbMsgId;
    // Set to true because it doesn't delete topic at start up
    iTopicInSimMemoryDelete = ETrue;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::~CMmCustomMessHandler
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmCustomMessHandler::~CMmCustomMessHandler()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::~CMmCustomMessHandler.\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_CMMCUSTOMMESSHANDLER, "CMmCustomMessHandler::~CMmCustomMessHandler" );

    if( iListOfCiphValues )
        {
        //close the array of ciphering values
        iListOfCiphValues->Close();
        //delete the list of ciphering values
        delete iListOfCiphValues;
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmCustomMessHandler* CMmCustomMessHandler::NewL
        (
        CMmPhoNetSender* aPhoNetSender, // pointer to the phonet sender
        CMmPhoNetReceiver* aPhoNetReceiver, // pointer to the phonet sender
        CMmNetMessHandler* aNetMessHandler, // pointer to the Net messHandler
        CMmSecurityMessHandler* aSecurityMessHandler, // pointer to the security
        CMmMessageRouter* aMessageRouter, //pointer to the message router
        CMmUiccMessHandler* aUiccMessHandler
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NewL.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NEWL, "CMmCustomMessHandler::NewL" );

    CMmCustomMessHandler* customMessHandler =
        new( ELeave ) CMmCustomMessHandler();

    CleanupStack::PushL( customMessHandler );
    customMessHandler->iPhoNetSender = aPhoNetSender;
    customMessHandler->iNetMessHandler = aNetMessHandler;
    customMessHandler->iSecurityMessHandler = aSecurityMessHandler;
    customMessHandler->iMessageRouter = aMessageRouter;
    customMessHandler->iMmUiccMessHandler = aUiccMessHandler;

    customMessHandler->ConstructL();

    // CALL
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_MODEM_CALL );

    // NETWORK
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_MODEM_NETWORK );

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING

// TO BE DONE WITH INFO_PP_DATA_READ_RESP
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_INFO,
        INFO_PP_READ_RESP );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // SS
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_SS,
        SS_SERVICE_COMPLETED_IND );
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_SS,
        SS_SERVICE_FAILED_RESP );
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_SS,
        SS_GSM_USSD_SEND_RESP );

    // GSS
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_GSS,
        GSS_CS_SERVICE_RESP );
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_GSS,
        GSS_CS_SERVICE_FAIL_RESP );
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_GSS,
        GSS_HSXPA_USER_SETTING_READ_RESP );
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_GSS,
        GSS_HSXPA_USER_SETTING_WRITE_RESP );
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_GSS,
        GSS_HSXPA_USER_SETTING_IND );

    // PMM
    /* To be done in CPS
    aPhoNetReceiver->RegisterL( customMessHandler,
        PN_PERMANENT_DATA,
        PERM_PM_RECORD_READ_RESP );

    aPhoNetReceiver->RegisterL( customMessHandler,
        PN_PERMANENT_DATA,
        PERM_PM_RECORD_WRITE_RESP );*/

    // UICC
    aPhoNetReceiver->RegisterL(
        customMessHandler,
        PN_UICC,
        UICC_CARD_IND);

    CleanupStack::Pop( customMessHandler );

    // Send PERM_PM_RECORD_READ_REQ and update
    // iCallLifeTimer.
    customMessHandler->ReadLifeTimerFromPermanentMemory ( KCustomTransId );

    return customMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ConstructL.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CONSTRUCTL, "CMmCustomMessHandler::ConstructL" );

    // Initialize iListOfCiphValues
    iListOfCiphValues = new ( ELeave ) RArray<TCiphListEntry>( 1 );

    // Initialize refresh indication boolean
    iIsRefresh = EFalse;

    // Initialize ECID boolean
    iECIDInfoRequested = EFalse;

    // Initialize ECID information
    iECIDInfo.iMCC = 0; // Mobile Country Code
    iECIDInfo.iMNC = 0; // Mobile Network Code
    iECIDInfo.iCID = 0; // Cell identity
    iECIDInfo.iLAC = 0; // Location area code

    // Initialize Call Life Timer value.
    iCallLifeTimer = KCallLifeTimerNotSet;

    // initialize ISim application activation status
    iIsimApplActivated = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ReceiveMessageL
// Entry point for messages received from Domestic OS. Switches the message
// to the correct method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::ReceiveMessageL
        (
        const TIsiReceiveC& aIsiMessage  // ISI-message received
        )
    {
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

TFLOGSTRING3("TSY: CMmCustomMessHandler::ReceiveMessageL, Resource: %d, Message: %d", resource, messageId);
OstTraceExt2( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch( resource )
        {
        case PN_MODEM_CALL:
            {
            // switch according to the message id
            // and call the appropriate messagehandler method
            switch( messageId )
                {
                case CALL_MODEM_RELEASE_RESP:
                    {
                    CallReleaseResp( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_NOTIFICATION_IND:
                    {
                    CallGsmNotificationInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_MO_ALERT_IND:
                    {
                    CallMoAlertInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_TERMINATED_IND:
                    {
                    CallTerminateInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_RELEASE_IND:
                    {
                    CallReleaseInd( aIsiMessage );
                    break;
                    }
                case CALL_MODEM_BLACKLIST_CLEAR_RESP:
                    {
                    CallGsmBlackListClearResp();
                    break;
                    }
                case CALL_MODEM_EMERG_NBR_CHECK_RESP:
                    {
                    CallEmergencyNbrCheckResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL - switch resource - case PN_MODEM_CALL, switch messageId - default");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL- switch resource - case PN_CALL, switch messageId - default" );
                    break;
                    }
                }
            break;
            }
        case PN_GSS:
            {
            // switch according to the message id
            // and call the appropriate messagehandler method
            switch( messageId )
                {
                case GSS_CS_SERVICE_RESP:
                    {
                    GssCsServiceResp( aIsiMessage );
                    break;
                    }
                case GSS_CS_SERVICE_FAIL_RESP:
                    {
                    GssCsServiceFailResp( aIsiMessage );
                    break;
                    }
                case GSS_HSXPA_USER_SETTING_READ_RESP:
                    {
                    ReadHSxPAStatusResp( aIsiMessage );
                    break;
                    }
                case GSS_HSXPA_USER_SETTING_WRITE_RESP:
                    {
                    WriteHSxPAStatusResp( aIsiMessage );
                    break;
                    }
                case GSS_HSXPA_USER_SETTING_IND:
                    {
                    HSxPASettingInd( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL - switch resource - case PN_GSS, switch messageId - default");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL- switch resource - case PN_GSS, switch messageId - default" );
                    break;
                    }
                }
            break; // end case PN_GSS
            }
        case PN_MODEM_NETWORK:
            {
            // switch according to the message id
            // and call the appropriate messagehandler method
            switch( messageId )
                {
                case NET_CS_WAKEUP_RESP:
                    {
                    NetCsWakeupResp();
                    break;
                    }
                case NET_MODEM_REG_STATUS_IND:
                    {
                    NetModemRegStatusInd( aIsiMessage );
                    break;
                    }
                case NET_CIPHERING_IND:
                    {
                    NetCipheringInd( aIsiMessage );
                    break;
                    }
                case NET_MODEM_REG_STATUS_GET_RESP:
                    {
                    NetModemRegStatusGetResp( aIsiMessage );
                    break;
                    }
                case NET_SET_RESP:
                    {
                    NetSetResp( aIsiMessage );
                    break;
                    }
                case NET_RAT_RESP:
                    {
                    NetRatResp( aIsiMessage );
                    break;
                    }
                case NET_NEIGHBOUR_CELLS_RESP:
                    {
                    NetNeighbourCellsResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL - switch resource - case PN_MODEM_NETWORK, switch messageId - default");
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL- switch resource - case PN_MODEM_NETWORK, switch messageId - default" );
                    break;
                    }
                }
            break;
            }
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case PN_INFO:
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        case PN_MODEM_INFO:
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            {
            // switch according to the message id
            // and call the appropriate messagehandler method
            switch( messageId )
                {
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
                 // TO BE DONE WITH INFO_PP_DATA_READ_RESP
                 case INFO_PP_READ_RESP:
                    {
                    InfoPpReadResp( aIsiMessage );
                    break;
                    }
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
                 default:
                    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL - switch resource - case PN_INFO, switch messageId - default");
OstTrace0( TRACE_NORMAL, DUP6_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL- switch resource - case PN_INFO, switch messageId - default" );
                    break;
                    }
                }
            break;
            }
        case PN_SS:
            {
            switch( messageId )
                {
                case SS_SERVICE_COMPLETED_IND:
                    {
                    SsServiceCompletedInd( aIsiMessage );
                    break;
                    }
                case SS_SERVICE_FAILED_RESP:
                    {
                    SsServiceFailedResp( aIsiMessage );
                    break;
                    }
                case SS_GSM_USSD_SEND_RESP:
                    {
                    SsGsmUssdSendResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL - switch resource - case PN_SS, switch messageId - default");
OstTrace0( TRACE_NORMAL, DUP12_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL- switch resource - case PN_SS, switch messageId - default" );
                    break;
                    }
                }
            break;
            }
        /* To be done in CPS
        case PN_PERMANENT_DATA:
            {
            if ( KCustomTransId == aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) )
                {
                switch( messageId )
                    {
                    case PERM_PM_RECORD_READ_RESP:
                        {
                        LifeTimerFromPermanentMemoryReadResponse( aIsiMessage );
                        break;
                        }

                    case PERM_PM_RECORD_WRITE_RESP:
                        {
                        LifeTimerToPermanentMemoryWriteResponse( aIsiMessage );
                        break;
                        }

                    default:
                        {
                        TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL\
                            - switch resource - case PN_PERMANENT_DATA, switch messageId - default.\n");
OstTrace0( TRACE_NORMAL, DUP16_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL - switch resource - case PN_PERMANENT_DATA, switch messageId - default." );
                        break;
                        }
                    }
                TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL\
                            - switch resource - case PN_PERMANENT_DATA, transaction id is not KCustomTransId\n");
OstTrace0( TRACE_NORMAL, DUP14_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL - switch resource - case PN_PERMANENT_DATA, transaction id is not KCustomTransId" );
                }
            break;
            }*/

        case PN_UICC:
            {
            if( UICC_CARD_IND == messageId )
                {
                UiccCardInd( aIsiMessage );
                }
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::ReceiveMessageL - switch resource - default" );
OstTrace0( TRACE_NORMAL, DUP13_CMMCUSTOMMESSHANDLER_RECEIVEMESSAGEL, "CMmCustomMessHandler::ReceiveMessageL- switch resource - default" );
            break; // server not known
            }
        } // end of switch
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallReleaseReq
// Constructs CALL_MODEM_RELEASE_REQ ISI message from input parameters and sends
// it through phonet. HangUp request message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::CallReleaseReq
        (
        TUint8 aTransactionId, //transaction id
        TUint8 aCallId, //call id
        TUint8 aCause //cause value for releasing the call
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallReleaseReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLRELEASEREQ, "CMmCustomMessHandler::CallReleaseReq" );

    iReleaseCauseValueSent = aCause;

    //Append data for ISI message
    TBuf8< 2 + SIZE_CALL_MODEM_SB_CAUSE > messageBuf;

    messageBuf.Append( aCallId );
    messageBuf.Append( 1 ); // Num of Subblocks

    //Call_Release_Req sub block
    messageBuf.Append( CALL_MODEM_SB_CAUSE );
    messageBuf.Append( SIZE_CALL_MODEM_SB_CAUSE );
    messageBuf.Append( CALL_MODEM_CAUSE_TYPE_CLIENT );
    messageBuf.Append( aCause );

    //send message via phonet
    return iPhoNetSender->Send( PN_MODEM_CALL, aTransactionId, CALL_MODEM_RELEASE_REQ,
      messageBuf );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallReleaseResp
// Breaks response to the HangUp request. In case of failure, HangUp is
// completed to the Symbian OS layer. Otherwise HangUp handle the success case.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CallReleaseResp
        (
        const TIsiReceiveC& aIsiMsg    //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallReleaseResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLRELEASERESP, "CMmCustomMessHandler::CallReleaseResp" );

    //initialize the causeType and causeValue
    TUint8 causeType  = 0;
    TUint8 causeValue = 0;
    TInt ret = KErrNone;

    TUint sbStartOffSet( 0 );

    //get cause sub block
    //if subblock is present
    if( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RELEASE_RESP, CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
      {
        //get cause type
        causeType = aIsiMsg.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE );

        //get cause value
        causeValue = aIsiMsg.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_CAUSE_OFFSET_CAUSE );

        if( iReleaseCauseValueSent != causeValue )
            {
            //map error value to ETEL error value
            ret = CMmStaticUtility::CSCauseToEpocError ( PN_MODEM_CALL,
                causeType, causeValue );
            }
        }
  // no packed parameters for completion
  iMessageRouter->Complete( ECustomTerminateCallIPC, ret );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallMoAlertInd
// This method breaks the CallMo alerting indication message.
// Trying to find Remote alerting information. In success case
// send LocalRemoteAlertToneRequired value to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CallMoAlertInd
        (
        const TIsiReceiveC& aIsiMsg    //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallMoAlertInd.");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLMOALERTIND, "CMmCustomMessHandler::CallMoAlertInd" );

    TBool playRemoteAlertToneLocally( EFalse );
    TUint sbStartOffSet( 0 );

    // Get call alerting info sub block
    // if subblock was found
    if( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_MO_ALERT_IND,
        CALL_MODEM_SB_ALERTING_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallMoAlertInd,\
            - CALL_MODEM_SB_ALERTING_INFO sub block found.");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_CALLMOALERTIND, "CMmCustomMessHandler::CallMoAlertInd, CALL_MODEM_SB_ALERTING_INFO sub block found" );

        // Get alerting info
        TUint8 callAlertingInfo( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_ALERTING_INFO_OFFSET_ALERTINGINFO ) );

        // If LSB is "1" inform that remote alerting tone
        // should be generated locally.
        if ( callAlertingInfo & 0x01 )
            {
            playRemoteAlertToneLocally = ETrue;
            }
        }

    // Complete remote alerting tone notification
    // parameter for SOS layer: a TBool playRemoteAlertToneLocally
    CMmDataPackage dataPackage;
    dataPackage.PackData( &playRemoteAlertToneLocally );
    iMessageRouter->Complete( ECustomGetRemoteAlertingToneStatusIPC,
        &dataPackage,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadCiReq
// Read ciphering indicator status from UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccReadCiReq( TInt aTrId )
    {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccReadCiReq, transaction ID: %d", aTrId );
OstTrace1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADCIREQ, "CMmCustomMessHandler::UiccReadCiReq;aTrId=%d", aTrId );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = static_cast<TUiccTrId>( aTrId );
    // We need to know the CI status info which is located in byte 3 in EF AD
    // b1=0: ciphering indicator feature disabled
    // b1=1: ciphering indicator feature enabled
    params.dataAmount = 1;
    params.dataOffset = 2;
    params.fileId = KElemFileAdministrativeData;
    params.fileIdSfi = 3;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadCiResp
// Complete ciphering indicator status
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccReadCiResp(
    TInt aStatus,
    TInt aTrId,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadCiResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADCIRESP, "CMmCustomMessHandler::UiccReadCiResp" );

    TInt ret( KErrNone );
    TBool cipherIndicatorStatus( EFalse );

    if( UICC_STATUS_OK == aStatus  )
        {
        // Get ciphering indicator info ( bit 0)
        TUint8 cipheringIndicator(aFileData[0] & 0x01 );
        if ( cipheringIndicator )
            {
            cipherIndicatorStatus = ETrue;
            }
        }
    else
        {
        ret = KErrNotFound;
        }

    if ( ETrIdReadCipheringIndicatorStatusCustom == aTrId )
        {
        // Complete GetCipheringInfo
        // packed parameter: TBool (ciphering indicator required/not required)
        CMmDataPackage dataPackage;
        dataPackage.PackData( &cipherIndicatorStatus );
        iMessageRouter->Complete(
            ECustomGetCipheringInfoIPC,
            &dataPackage,
            ret );
        }
    else if( ETrIdNotifyCipheringIndicatorStatusCustom == aTrId )
        {
        CMmDataPackage dataPackage;
        TCiphListEntry chiplistentry;
        chiplistentry.iTraId = aTrId;

        // Find the right value of ciphering for this resp message
        TInt index ( iListOfCiphValues->Find( chiplistentry, Match ) );

        // If the right value is found
        if ( KErrNotFound != index )
            {
            TBool cipheringOn( ( *iListOfCiphValues )[index ].iCiphStatus );
            dataPackage.PackData( &cipherIndicatorStatus, &cipheringOn );

            // Delete the used ciphering value from the list
            iListOfCiphValues->Remove( index );
            // Compress the list
            iListOfCiphValues->Compress();
            }
        else
            {
            TBool constantFalse( EFalse );
            dataPackage.PackData( &constantFalse, &constantFalse );
            ret = KErrGeneral;
            }

        iMessageRouter->Complete(
            ECustomNotifyCipheringInfoChangeIPC,
            &dataPackage,
            ret );
        }
    //No else
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetCsWakeupReq
// Constructs NET_CS_WAKEUP_REQ ISI message from input parameters and sends
// it through phonet.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::NetCsWakeupReq( TUint8 aTransId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetCsWakeupReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETCSWAKEUPREQ, "CMmCustomMessHandler::NetCsWakeupReq" );

    //Data buffer length is 2
    TBuf8<2> data;
    data.Append( aTransId );
    data.Append( NET_CS_WAKEUP_REQ );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_NETWORK );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    return( iPhoNetSender->Send( isimsg.Complete() ) );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetCsWakeupResp
// Breaks a NET_CS_WAKEUP_RESP ISI message and complete NetWakeup with
// KErrNone to SOS layer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::NetCsWakeupResp()
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::NetCsWakeupResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETCSWAKEUPRESP, "CMmCustomMessHandler::NetCsWakeupResp" );

    //getting this message indicates that the wakeup was successful
    //complete NetWakeup method (no packed parameters)
    iMessageRouter->Complete( ECustomNetWakeupIPC, KErrNone );

    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetCipheringInd
// This method breaks the ciphering indication message.
// This indication saves the ciphering status and calls SimReadCiReq to get the
// status of the ciphering indicator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::NetCipheringInd(
        const TIsiReceiveC &aIsiMsg )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::NetCipheringInd.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETCIPHERINGIND, "CMmCustomMessHandler::NetCipheringInd" );

    //set chipering to False
    TBool cipher( EFalse );

    //get the ciphering status
    TUint8 cipheringStatus( aIsiMsg.Get8bit( ISI_HEADER_SIZE +
      NET_CIPHERING_IND_OFFSET_CIPHERINGSTATUS ) );

    //check the value of the ciphering status
    if ( NET_CIPHERING_INDICATOR_ON == cipheringStatus )
        {
        cipher = ETrue;
        }

    TUint8 traId( ETrIdNotifyCipheringIndicatorStatusCustom );

    //save the ciphering information for later completion in the list.
    //this method has to request the sim to complete, so new indication
    //might come, values must be buffered.
    TCiphListEntry chiplistentry;
    chiplistentry.iTraId = traId;
    chiplistentry.iCiphStatus = cipher;
    iListOfCiphValues->Append( chiplistentry );

    //get the ciphering indicator information from SIM card
    //the response of this method will call the completion for the
    //notification
    UiccReadCiReq( ETrIdNotifyCipheringIndicatorStatusCustom );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetModemRegStatusInd
// This method breaks the NET_MODEM_REG_STATUS_IND message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::NetModemRegStatusInd
        (
        const TIsiReceiveC& aIsiMsg
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetModemRegStatusInd");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETMODEMREGSTATUSIND, "CMmCustomMessHandler::NetModemRegStatusInd" );

    if ( 0 != aIsiMsg.Get8bit(
        ISI_HEADER_SIZE +
        NET_MODEM_REG_STATUS_IND_OFFSET_SUBBLOCKCOUNT ) )
        {
        TUint sbStartOffSet( 0 );

        TInt retValue( aIsiMsg.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_IND,
            NET_MODEM_REG_INFO_COMMON,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) );

        // Check if NET_MODEM_REG_INFO_COMMON sub block is present.
        if ( KErrNone == retValue )
            {
            // Get registration status
            TUint8 registrationStatus( aIsiMsg.Get8bit(
                sbStartOffSet +
                NET_MODEM_REG_INFO_COMMON_OFFSET_REGISTRATIONSTATUS ) );

            TBool nspsIsOn( EFalse );

            switch ( registrationStatus )
                {
                case NET_REG_STATUS_NSPS:
                case NET_REG_STATUS_NSPS_NO_COVERAGE:
                    {
                    nspsIsOn = ETrue;
                    break;
                    }
                case NET_REG_STATUS_NOSERV_NOSIM:
                case NET_REG_STATUS_NOSERV_SIM_REJECTED_BY_NW:
                    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetModemRegStatusInd, No SIM");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_NETMODEMREGSTATUSIND, "CMmCustomMessHandler::NetModemRegStatusInd, No SIM" );
                    // Complete NetworkConnectionFailure notification
                    // (no packed parameters).
                    // If puk code is required notification shouldn't
                    // be completed.
                    TBool pukCodeRequired( iSecurityMessHandler->GetPukCodeReq() );
                    if( !pukCodeRequired )
                        {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetModemRegStatusInd - Complete network connection failure");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_NETMODEMREGSTATUSIND, "CMmCustomMessHandler::NetModemRegStatusInd - Complete network connection failure" );

                        iMessageRouter->Complete(
                            ECustomNotifyNetworkConnectionFailureIPC,
                            KErrNone );
                        }
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetModemRegStatusInd, switch registrationStatus - default");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_NETMODEMREGSTATUSIND, "CMmCustomMessHandler::NetModemRegStatusInd, switch registrationStatus - default" );
                    break;
                    }
                }

            // Inform of the status of NSPS.
            // Packed parameter: TBool nspsIsOn.
            CMmDataPackage dataPackage;
            dataPackage.PackData( &nspsIsOn );

            iMessageRouter->Complete(
                ECustomNotifyNSPSStatusIPC,
                &dataPackage,
                KErrNone );
            }
        // No else

        // Check if ECID info needs to be updated.
        CheckECIDInfo( aIsiMsg );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadDynamic2FlagsReq
// Read dynamic 2 flags from UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccReadDynamic2FlagsReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadDynamic2FlagsReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADDYNAMIC2FLAGSREQ, "CMmCustomMessHandler::UiccReadDynamic2FlagsReq" );
    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadDynamic2Flags;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.fileId = KElemFileDyn2FlagsOrange;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;
    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( KOrangeDedicatedFile >> 8 );
    params.filePath.Append( KOrangeDedicatedFile );
    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadDynamic2FlagsResp
// Complete dynamic 2 flags
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccReadDynamic2FlagsResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadDynamic2FlagsResp");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADDYNAMIC2FLAGSRESP, "CMmCustomMessHandler::UiccReadDynamic2FlagsResp" );

    TInt ret( KErrNone );
    RMmCustomAPI::TGetAlsBlockStatus blockStatus
        ( RMmCustomAPI::EBlockStatusUnknown );

    if ( UICC_STATUS_OK == aStatus )
        {
        // The value of the block status is in the LSB byte
        if ( aFileData[0] & 0x01 )
            {
            blockStatus = RMmCustomAPI::EBlockStatusActive;
            }
        else
            {
            blockStatus = RMmCustomAPI::EBlockStatusInactive;
            }
        }
    else
        {
        blockStatus = RMmCustomAPI::EBlockStatusNotSupported;
        ret = KErrNotFound;
        }

    // Complete, packed parameter: TGetAlsBlockStatus (block status)
    CMmDataPackage dataPackage;
    dataPackage.PackData( &blockStatus );
    iMessageRouter->Complete(
        ECustomGetAlsBlockedIPC,
        &dataPackage,
        ret );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccWriteDynamic2FlagsReq
// Write dynamic 2 flags to UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccWriteDynamic2FlagsReq( TUint8 aInfo )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccWriteDynamic2FlagsReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCWRITEDYNAMIC2FLAGSREQ, "CMmCustomMessHandler::UiccWriteDynamic2FlagsReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteDynamic2Flags;
    params.dataOffset = 0;
    params.fileId = KElemFileDyn2FlagsOrange;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( KOrangeDedicatedFile >> 8 );
    params.filePath.Append( KOrangeDedicatedFile );

    params.fileData.Append( aInfo );
    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccWriteDynamic2FlagsResp
// Complete write dynamic flags request ro UICC
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccWriteDynamic2FlagsResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccWriteDynamic2FlagsReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCWRITEDYNAMIC2FLAGSRESP, "CMmCustomMessHandler::UiccWriteDynamic2FlagsResp" );

    TInt ret( KErrNone );

    if ( UICC_STATUS_OK != aStatus )
        {
        ret = KErrGeneral;
        }

    iMessageRouter->Complete( ECustomSetAlsBlockedIPC, ret );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallTerminateInd
// This method breaks the CALL_MODEM_TERMINATE_IND
// indication message. Gets CALL_MODEM_SB_SS_DIAGNOSTICS sub blocks and sets
// information into a call object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CallTerminateInd
        (
        const TIsiReceiveC& aIsiMsg    //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallTerminateInd.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLTERMINATEIND, "CMmCustomMessHandler::CallTerminateInd" );

    //unique call id (without possible generic id)
    TInt callId = static_cast<TInt>( aIsiMsg.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_TERMINATED_IND_OFFSET_CALLID ) & 0x07 );

    if ( CALL_MODEM_ID_NONE != callId )
        {
        TUint sbStartOffSet( 0 );

        TUint8 ssDiagnostics = 0;

        //find sub block
        if( KErrNone == aIsiMsg.FindSubBlockOffsetById(
          ISI_HEADER_SIZE + SIZE_CALL_MODEM_TERMINATED_IND, CALL_MODEM_SB_SS_DIAGNOSTICS,
          EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
          {
            //get ss diagnostics
            ssDiagnostics = aIsiMsg.Get8bit( sbStartOffSet +
                CALL_MODEM_SB_SS_DIAGNOSTICS_OFFSET_DIAGNOSTICS );

            // Set highest bit to 1 if there is some diagnostic information
            // (This is in accordance with 3GPP TS 24.008 "Mobile radio
            // interface Layer 3 specification; Core network protocols" and with
            // Series 60 Supplementary Services UI Specification)
            if ( 0 != ssDiagnostics )
                {
                ssDiagnostics |= 0x80; // binary 10000000
                TFLOGSTRING3("TSY:CMmCustomMessHandler::CallTerminatedInd: Diagnostic octet=%d received for call id=%d",
                    ssDiagnostics, callId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_CALLTERMINATEIND, "CMmCustomMessHandler::CallTerminateInd;Diagnostic octet==%hhu received for call id=%d", ssDiagnostics, callId );
                }
            }

        // set the diagnostics in call object (might be zero)
        // parameters for SOS layer: Call id and TUint8 with diagnostics
        CMmDataPackage dataPackage;
        dataPackage.PackData( &callId, &ssDiagnostics );
        iMessageRouter->Complete( ECustomGetDiagnosticOctetsIPC, &dataPackage,
           KErrNone );

        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallReleaseInd
// This method breaks the CALL_MODEM_RELEASE_IND.
// indication message. Gets CALL_MODEM_SB_SS_DIAGNOSTICS sub blocks and sets
// information into a call object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CallReleaseInd
        (
        const TIsiReceiveC& aIsiMsg    //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallReleaseInd.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLRELEASEIND, "CMmCustomMessHandler::CallReleaseInd" );

    //unique call id (without possible generic id)
    TInt callId = static_cast<TInt>( aIsiMsg.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_RELEASE_IND_OFFSET_CALLID ) & 0x07 );

    if ( CALL_MODEM_ID_NONE != callId )
        {
        TUint sbStartOffSet( 0 );

        TUint8 ssDiagnostics = 0;

        //get call operation sub block
        if( KErrNone == aIsiMsg.FindSubBlockOffsetById(
          ISI_HEADER_SIZE + SIZE_CALL_MODEM_RELEASE_IND, CALL_MODEM_SB_SS_DIAGNOSTICS,
          EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
          {
            //get ss diagnostics
            ssDiagnostics = aIsiMsg.Get8bit( sbStartOffSet +
                CALL_MODEM_SB_SS_DIAGNOSTICS_OFFSET_DIAGNOSTICS );

            // Set highest bit to 1 if there is some diagnostic information
            // (This is in accordance with 3GPP TS 24.008 "Mobile radio
            // interface Layer 3 specification; Core network protocols" and with
            // Series 60 Supplementary Services UI Specification)
            if ( 0 != ssDiagnostics )
                {
                ssDiagnostics |= 0x80; // binary 10000000
                TFLOGSTRING3("TSY:CMmCustomMessHandler::CallReleaseInd: Diagnostic octet=%d received for call id=%d",
                    ssDiagnostics, callId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_CALLRELEASEIND, "CMmCustomMessHandler::CallReleaseInd;Diagnostic octet==%hhu received for call id=%d", ssDiagnostics, callId );
                }
            }

        // set the diagnostics in call object (might be zero)
        // parameters for SOS layer: Call id and TUint8 with diagnostics
        CMmDataPackage dataPackage;
        dataPackage.PackData( &callId, &ssDiagnostics );
        iMessageRouter->Complete( ECustomGetDiagnosticOctetsIPC, &dataPackage,
           KErrNone );

        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::InfoPpReadReq
// Constructs INFO_PP_READ_REQ ISI message from input parameters and sends
// it through phonet. This message can be used to read ALS or Two Digit Dialling
// support status in the product profiles from INFO server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::InfoPpReadReq
        (
        TProductProfileRequestType aReqType // request type, used as
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::InfoPpReadReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_INFOPPREADREQ, "CMmCustomMessHandler::InfoPpReadReq" );

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    //create the buffer for SB_PP subblock
    TBuf8<SIZE_INFO_SB_PP> sbPPSubBlockBuf( 0 );
    TIsiSubBlock sbPPSubBlock( sbPPSubBlockBuf, INFO_SB_PP, EIsiSubBlockTypeId8Len8 );
    //add 0x00 as the Dynamic Value ID, not needed cause we want to get
    //a static value
    sbPPSubBlockBuf.Append( 0x00 );

    //add 0x01 as the length of the product profile, we want only als
    //profile, thus lenght is 1
    sbPPSubBlockBuf.Append( 0x01 );

    //add the product profile number
    if ( EGetAlsPPSupport == aReqType )
        {
        sbPPSubBlockBuf.Append( INFO_PP_ALS );
        }
    else if ( ECheckTwoDigitDialSupport == aReqType )
        {
        sbPPSubBlockBuf.Append( KinfoPpTwoDigitDial );
        }
  else
    {
    // not supported
    return KErrArgument;
    }

    //value of the product profile is 0x00, not need cause we are reading
    sbPPSubBlockBuf.Append( 0x00 );

  // Construct INFO_PP_READ_REQ message (1 data byte +
  // sub block count + INFO_SB_PP sub block)
  TBuf8< 2 + SIZE_INFO_SB_PP > infoPpReadReq;

    //set pp batch reading to read specific product profile, thus 0.
    infoPpReadReq.Append( INFO_PP_PROD_PROFILE_FEATURE );

    // add sub block
    infoPpReadReq.Append( 1 ); // number of sub blocks
    infoPpReadReq.Append( sbPPSubBlock.CompleteSubBlock() );

    //create the isi message
  //transaction id = request type
  TUint8 transId = static_cast<TUint8>( aReqType );

    return iPhoNetSender->Send( PN_INFO, transId, INFO_PP_READ_REQ, infoPpReadReq );

#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::InfoPpReadResp
// Breaks a INFO_PP_READ_RESP ISI message. This method
// gets the status of the ALS or two digit support.
// Complete with packed parameter RMmCustomAPI::TAlsSupport or
// RMmCustomAPI::TTwoDigitDialSupport to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::InfoPpReadResp
        (
        const TIsiReceiveC& aIsiMessage    //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::InfoPpReadResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_INFOPPREADRESP, "CMmCustomMessHandler::InfoPpReadResp" );
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TInt errorValue( KErrGeneral );

    // Get Status
    TUint8 status = aIsiMessage.Get8bit( ISI_HEADER_SIZE + INFO_PP_READ_RESP_OFFSET_STATUS );

    //initialize the als support with off
    RMmCustomAPI::TAlsSupport alsSupport( RMmCustomAPI::EAlsSupportOff );

    //initialize two digit support with off
    RMmCustomAPI::TTwoDigitDialSupport twoDigitDialSupport(
        RMmCustomAPI::ETwoDigitDialSupportOff );

  TFLOGSTRING2("TSY: CMmCustomMessHandler::InfoPpReadResp - Status: %d",
    status );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_INFOPPREADRESP, "CMmCustomMessHandler::InfoPpReadResp;status=%hhu", status );

    switch ( status )
        {
        case INFO_OK:
            {

          TUint sbInfoPpReadStartOffset( 0 );


            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById( ISI_HEADER_SIZE + SIZE_INFO_PP_READ_RESP,
              INFO_SB_PP, EIsiSubBlockTypeId8Len8, sbInfoPpReadStartOffset ) )

                {

                TInt length = aIsiMessage.Get8bit( sbInfoPpReadStartOffset + INFO_SB_PP_OFFSET_STRLEN  );

        TFLOGSTRING2("TSY: CMmCustomMessHandler::InfoPpReadResp - Info length: %d",
          length );
OstTrace1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_INFOPPREADRESP, "CMmCustomMessHandler::InfoPpReadResp - Info length=%d", length );

                if ( 1 == length )
                    {
                    //introduce the pPFeatures buffer to hold the feature value,
                    //buffer size is 2
                    TBuf8<KTwo> pPFeatures;

                    //copy the 2 bytes from product profile
                    pPFeatures.Copy( aIsiMessage.GetData( sbInfoPpReadStartOffset
                      + INFO_SB_PP_OFFSET_PRODUCTPROFILE, KTwo ) );

          TFLOGSTRING2("TSY: CMmCustomMessHandler::InfoPpReadResp - Info data: %S",
            &pPFeatures );
OstTraceExt1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_INFOPPREADRESP, "CMmCustomMessHandler::InfoPpReadResp;pPfeatures=%s", pPFeatures );

                    //check that product profile is requested one
                    if ( INFO_PP_ALS == pPFeatures [0] )
                        {
                        //Get the value of the Als feature
                        if ( 0 != pPFeatures [1] )
                            {
              TFLOGSTRING("TSY: CMmCustomMessHandler::InfoPpReadResp - EAlsSupportOn");
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_INFOPPREADRESP, "CMmCustomMessHandler::InfoPpReadResp - EAlsSupportOn" );
              alsSupport = RMmCustomAPI::EAlsSupportOn;
                            }
                        //set the error value to KErrNone
                        errorValue = KErrNone;
                        }
                    else if ( KinfoPpTwoDigitDial == pPFeatures [0] )
                        {
                        //Get the value of the two digit dial feature
                        if ( 0 != pPFeatures [1] )
                            {
              TFLOGSTRING("TSY: CMmCustomMessHandler::InfoPpReadResp - EAlsSupportOff");
OstTrace0( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_INFOPPREADRESP, "CMmCustomMessHandler::InfoPpReadResp - EAlsSupportOff" );
              twoDigitDialSupport =
                RMmCustomAPI::ETwoDigitDialSupportOn;
                            }
                        //set the error value to KErrNone
                        errorValue = KErrNone;
                        }

                    //No else

                    }
                }
            break;
            }
        case INFO_FAIL:
            {
            //error in operation
            errorValue = KErrGeneral;
            break;
            }
        case INFO_NO_NUMBER:
            {
            //information is not found
            errorValue = KErrNotFound;
            break;
            }
        default:
            {
            TFLOGSTRING("TSY: CMmCustomMessHandler::InfoPpReadResp, switch status - default.\n" );
OstTrace0( TRACE_NORMAL, DUP6_CMMCUSTOMMESSHANDLER_INFOPPREADRESP, "CMmCustomMessHandler::InfoPpReadResp, switch status - default" );
            errorValue = KErrNotSupported;
            break;
            }
        }

  TUint8 traId = aIsiMessage.Get8bit( ISI_HEADER_SIZE + INFO_PP_READ_RESP_OFFSET_TRANSID );
    if ( EGetAlsPPSupport == traId )
        {
    // complete with packed parameter RMmCustomAPI::TAlsSupport
    CMmDataPackage dataPackage;
    dataPackage.PackData( &alsSupport );
    iMessageRouter->Complete( ECustomCheckAlsPpSupportIPC,
      &dataPackage, errorValue );
        }
    else if ( ECheckTwoDigitDialSupport == traId )
        {
    // complete with packed parameter RMmCustomAPI::TTwoDigitDialSupport
    CMmDataPackage dataPackage;
    dataPackage.PackData( &twoDigitDialSupport );
    iMessageRouter->Complete( ECustomCheckTwoDigitDialSupportIPC,
      &dataPackage, errorValue );
        }

    //No else
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallGsmBlackListClearReq
// Constructs CALL_MODEM_BLACKLIST_CLEAR_REQ ISI message from input parameters and
// sends it through phonet.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::CallGsmBlackListClearReq
        (
         TUint8 aTransId // Transaction Id
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmBlackListClearReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLGSMBLACKLISTCLEARREQ, "CMmCustomMessHandler::CallGsmBlackListClearReq" );

  // Construct CALL_MODEM_BLACKLIST_CLEAR_REQ message (1 data byte +
  // sub block count)
  TBuf8<2> blackListClearReq;

    blackListClearReq.Append( KCustomPadding );
    blackListClearReq.Append( 0x00 ); // sub block count

    //Send message via PhoNet
    return iPhoNetSender->Send( PN_MODEM_CALL, aTransId, CALL_MODEM_BLACKLIST_CLEAR_REQ,
      blackListClearReq );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallGsmBlackListClearResp
// Breaks a CALL_MODEM_BLACKLIST_CLEAR_RESP ISI message. This method
// gets indicates that the clear blacklist was successful.
// Complete CallGsmBlackListClear with KErrNone to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CallGsmBlackListClearResp
        (
//         const TIsiReceiveC& aIsiMsg // Received isi messge
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmBlackListClearResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLGSMBLACKLISTCLEARRESP, "CMmCustomMessHandler::CallGsmBlackListClearResp" );

    // call server response
    //getting this message indicates that the clear blacklist was successful
    //complete SOS layer method
  iMessageRouter->Complete( ECustomClearCallBlackListIPC, KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::SsServiceCompletedInd
// This method breaks the SS_SERVICE_COMPLETED_IND ISI message.
// Complete SsRequestCompleteNotification method from custom api or
// Complete NotifySsAdditionalInfo to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::SsServiceCompletedInd
        (
        const TIsiReceiveC& aIsiMessage //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::SsServiceCompletedInd");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_SSSERVICECOMPLETEDIND, "CMmCustomMessHandler::SsServiceCompletedInd" );

    if ( 0 < aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                SS_SERVICE_COMPLETED_IND_OFFSET_SUBBLOCKCOUNT ) )
        {
        TUint ssStatus( 0 );
        TUint8 sbSsStatus( 0 );
        TUint sbForwardingFeatureOffset( 0 );
        TUint sbBarringFeatureOffset( 0 );

        TUint sbStartOffset(0);
        // SS_STATUS_RESULT sub block
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_IND,
                SS_STATUS_RESULT,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset ) )
            {
            sbSsStatus = aIsiMessage.Get8bit( sbStartOffset +
                                              SS_STATUS_RESULT_OFFSET_SSSTATUS );
            }

        // SS_GSM_FORWARDING_INFO sub block
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_IND,
                SS_GSM_FORWARDING_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset ) )
            {
            // Sub block's sub block SS_GSM_FORWARDING_FEATURE:
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                            sbStartOffset + SIZE_SS_GSM_FORWARDING_INFO,
                            SS_GSM_FORWARDING_FEATURE,
                            EIsiSubBlockTypeId8Len8,
                            sbForwardingFeatureOffset
                            ) )
                {
                sbSsStatus = aIsiMessage.Get8bit( sbForwardingFeatureOffset +
                                SS_GSM_FORWARDING_FEATURE_OFFSET_SSSTATUS );
                }
      //Cases which don't return the other sub block should still be ok.
      else
        {
        // This simulates successfull SS case, where the info subblock
        // is received, but the forwarding feature isn't(the real status
        // cannot be read).
        sbSsStatus = SS_GSM_ACTIVE;
        }
            }

        // SS_GSM_BARRING_INFO sub block
        if ( 0 == sbSsStatus && KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_IND,
                SS_GSM_BARRING_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset
                ) )
            {
            // Sub block's sub block SS_GSM_BARRING_FEATURE
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                            sbStartOffset + SIZE_SS_GSM_BARRING_FEATURE,
                            SS_GSM_BARRING_FEATURE,
                            EIsiSubBlockTypeId8Len8,
                            sbBarringFeatureOffset
                            ) )
                {

                sbSsStatus = aIsiMessage.Get8bit( sbBarringFeatureOffset +
                                    SS_GSM_BARRING_FEATURE_OFFSET_SSSTATUS );
                }
            }


        // SS_GSM_GENERIC_SERVICE_INFO sub block
        if ( 0 == sbSsStatus && KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_IND,
                SS_GSM_GENERIC_SERVICE_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset
                ) )
            {
            sbSsStatus = aIsiMessage.Get8bit( sbStartOffset +
                                              SS_GSM_GENERIC_SERVICE_INFO_OFFSET_SSSTATUS );
            }

        // SS_GSM_DATA sub block
        if ( 0 == sbSsStatus && KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_IND,
                SS_GSM_DATA,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset
                ) )
            {
            sbSsStatus = aIsiMessage.Get8bit( sbStartOffset +
                                              SS_GSM_DATA_OFFSET_SSSTATUS );
            }

        if ( 0 != sbSsStatus )
            {
            //Found SS status info from SS_SERVICE_COMPLETED_IND
            if ( SS_GSM_ACTIVE & sbSsStatus )
                {
                ssStatus = KCustomApiSsGsmActive;
                }
            else if ( SS_GSM_REGISTERED & sbSsStatus )
                {
                ssStatus = KCustomApiSsGsmRegistered;
                }
            else if ( SS_GSM_PROVISIONED & sbSsStatus )
                {
                ssStatus = KCustomApiSsGsmProvisioned;
                }
            else
                {
                ssStatus = KCustomApiSsGsmQuiescent;
                }
      TFLOGSTRING2("TSY: CMmCustomMessHandler::SsServiceCompletedInd: Complete SsRequestCompleteNotification ssStatus:%d",
        ssStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_SSSERVICECOMPLETEDIND, "CMmCustomMessHandler::SsServiceCompletedInd;Complete SsRequestCompleteNotification ssStatus=%u", ssStatus );

            CMmDataPackage dataPackage;
            // pack parameter: a TInt with SS status
            dataPackage.PackData( &ssStatus );
            iMessageRouter->Complete( ECustomNotifySsRequestCompleteIPC,
                &dataPackage, KErrNone );

            }

        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_IND,
                SS_GSM_ADDITIONAL_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset ) )
            {
            //create SsAdditionalInfo structure
            RMmCustomAPI::TSsAdditionalInfo additionalInfo;

            additionalInfo.iOperationCode =
                aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                    SS_SERVICE_COMPLETED_IND_OFFSET_OPERATION );

            TUint8 length = aIsiMessage.Get8bit( sbStartOffset +
                SS_GSM_ADDITIONAL_INFO_OFFSET_RETURNRESULTLEN );

            additionalInfo.iAdditionalInfo.Copy( aIsiMessage.GetData(
                sbStartOffset + SS_GSM_ADDITIONAL_INFO_OFFSET_RETURNRESULT,
                length  ) );

      TFLOGSTRING("TSY: CMmCustomMessHandler::SsServiceCompletedInd: Complete SsAdditionalInfoNotification");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_SSSERVICECOMPLETEDIND, "CMmCustomMessHandler::SsServiceCompletedInd - Complete SsAdditionalInfoNotification" );

      // complete notification
            CMmDataPackage dataPackage;
            // pack parameter: pointer to RMmCustomAPI::TSsAdditionalInfo
            RMmCustomAPI::TSsAdditionalInfo* additionalInfoPtr =
              &additionalInfo;
            dataPackage.PackData( &additionalInfoPtr );
            iMessageRouter->Complete( ECustomSsAdditionalInfoNotificationIPC,
                &dataPackage, KErrNone );

            }
        }
    else  // No subblock found, e.g. in AlterEgo functionality
        {
        TInt ssStatus( KCustomApiSsGsmQuiescent );

         CMmDataPackage dataPackage;
         // pack parameter: a TInt with SS status
         dataPackage.PackData( &ssStatus );
         iMessageRouter->Complete( ECustomNotifySsRequestCompleteIPC,
                &dataPackage, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::SsServiceFailedResp
// Breaks a SS_SERVICE_FAILED_RESP ISI message. This method
// triggers SsRequestCompleteNotification notification.
// Completes SsRequestCompleteNotification method from custom tsy with
// status value 0xFFFF to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::SsServiceFailedResp
        (
        const TIsiReceiveC& aIsiMessage //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::SsServiceFailedResp");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_SSSERVICEFAILEDRESP, "CMmCustomMessHandler::SsServiceFailedResp" );

    TInt ssStatus( RMmCustomAPI::KSsStatusReturnError );

    CMmDataPackage dataPackage;

    TUint sbStartOffset( 0 );

    //create SsAdditionalInfo structure
    RMmCustomAPI::TSsAdditionalInfo additionalInfo;

    // this data is not available, set to 0 just in case
    additionalInfo.iOperationCode = 0x00;

    //if sub block exists
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
        1,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        TUint8 subBlockId( aIsiMessage.Get8bit( sbStartOffset ) );
        switch ( subBlockId )
            {
            case SS_GSM_INDICATE_PASSWORD_ERROR:
                {
                // Set error code to additional info. No mapping needed because
                // client wants error code as it was received from network.
                additionalInfo.iAdditionalInfo.Append(
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_PASSWORD_ERROR_OFFSET_GUIDANCEINFO ) );
                break;
                }
            case SS_GSM_INDICATE_ERROR:
                {
                additionalInfo.iAdditionalInfo.Append( aIsiMessage.Get8bit(
                    sbStartOffset + SS_GSM_INDICATE_ERROR_OFFSET_ERRORCODE ) );
                // If additional indication is not default, add it to buffer
                TUint8 addIndication(
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_ERROR_OFFSET_ADDITIONALINDICATION ) );
                // Just to keep previous functionality, the default value
                // not added to buffer
                if ( SS_DEFAULT_VALUE != addIndication )
                    {
                    additionalInfo.iAdditionalInfo.Append( addIndication );
                    }
                break;
                }
            case SS_GSM_INDICATE_PROBLEM:
                {
                // Problem code is added to additional info only if RELEASE
                // COMPLETE message from network includes Return Result
                // component
                if ( SS_GSM_RETURN_RESULT_PROBLEM ==
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMTYPE ) )
                    {
                    additionalInfo.iAdditionalInfo.Append(
                        aIsiMessage.Get8bit( sbStartOffset +
                            SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMCODE ) );
                    }
                break;
                }
            case SS_GSM_INDICATE_MSG_ERROR:
                {
                additionalInfo.iAdditionalInfo.Append(
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_MSG_ERROR_OFFSET_MESSAGEERRORCODE ) );
                break;
                }
            case SS_OTHER_ERROR:
                {
                additionalInfo.iAdditionalInfo.Append( aIsiMessage.Get8bit(
                    sbStartOffset + SS_OTHER_ERROR_OFFSET_ERRORCODE ) );
                break;
                }
            case SS_GSM_MM_RELEASED:
                {
                // This subblock occurs when network has released SS request,
                // therefore SS status is set accordingly
                ssStatus = RMmCustomAPI::KSsStatusNetworkFailure;

                additionalInfo.iAdditionalInfo.Append( aIsiMessage.Get8bit(
                    sbStartOffset + SS_GSM_MM_RELEASED_OFFSET_CAUSE ) );
                break;
                }
            default:
                {
                // unknown error sub block
                additionalInfo.iAdditionalInfo.Append( 0x00 );
                break;
                }
            }
        TFLOGSTRING2("TSY: CMmCustomMessHandler::SsServiceFailedResp: Ss Status: %d", ssStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_SSSERVICEFAILEDRESP, "CMmCustomMessHandler::SsServiceFailedResp;Ss Status=%d", ssStatus );
        // pack parameter: a TInt with SS status
        dataPackage.PackData( &ssStatus );

        // To complete outstanding SsRequestCompleteNotification
        iMessageRouter->Complete(
            ECustomNotifySsRequestCompleteIPC,
            &dataPackage,
            KErrNone );

        TFLOGSTRING2("TSY: CMmCustomMessHandler::SsServiceFailedResp: SB ID: 0x%x",subBlockId );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_SSSERVICEFAILEDRESP, "CMmCustomMessHandler::SsServiceFailedResp;subBlockId=%hhx", subBlockId );
        // pack parameter: pointer to RMmCustomAPI::TSsAdditionalInfo
        RMmCustomAPI::TSsAdditionalInfo* additionalInfoPtr = &additionalInfo;
        dataPackage.PackData( &additionalInfoPtr );

        // To complete outstanding SsAdditionalInfoNotification
        iMessageRouter->Complete(
            ECustomSsAdditionalInfoNotificationIPC,
            &dataPackage,
            KErrNone );
        }
    else
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::SsServiceFailedResp: No subblock's");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_SSSERVICEFAILEDRESP, "CMmCustomMessHandler::SsServiceFailedResp - No subblock's" );

        dataPackage.PackData( &ssStatus );

        // No subblocks, complete with default values
        iMessageRouter->Complete( ECustomNotifySsRequestCompleteIPC,
                              &dataPackage,
                              KErrNone );
        // No additional info given, no need to complete
        // ECustomSsAdditionalInfoNotificationIPC
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallEmergencyNbrCheckReq
// Constructs CALL_MODEM_EMERGENCY_NBR_CHECK_REQ ISI message from input parameters
// and sends it through phonet. This message can be used to check if given
// number is an emrgency number.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::CallEmergencyNbrCheckReq
        (
        TUint8 aTransId,              // Transaction Id
        RMmCustomAPI::TMobileTelNumber& aNumber,  // Telnumber
        RMmCustomAPI::TCheckMode aCheckMode     // Check mode
        )
    {
  TFLOGSTRING( "TSY: CMmCustomMessHandler::CallEmergencyNbrCheckReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLEMERGENCYNBRCHECKREQ, "CMmCustomMessHandler::CallEmergencyNbrCheckReq" );

    //save the information to match it when the response is got
    iUsedEmergencyNbrCheckMode = aCheckMode;
    iEmergencyNumberLength = aNumber.Length();

  // Construct CALL_MODEM_EMERG_NBR_CHECK_REQ message (1 filler +
  // sub block count + bytes for CALL_MODEM_SB_DESTINATION_ADDRESS sub block)
  TBuf8< 2 + SIZE_CALL_MODEM_SB_DESTINATION_ADDRESS +
      ( RMobilePhone::KMaxMobileTelNumberSize * 2 ) > callEmergencyNbrCheckReq;

    // Filler
    callEmergencyNbrCheckReq.Append( KCustomPadding );
    // Sub block count
    callEmergencyNbrCheckReq.Append( 1 );

    //create the CALL_MODEM_DESTINATION_ADDRESS subblock
    TBuf8<SIZE_CALL_MODEM_SB_DESTINATION_ADDRESS +
        ( RMobilePhone::KMaxMobileTelNumberSize * 2 )>
        destinationAddressBuf( 0 );
    TIsiSubBlock destinationAddressSb( destinationAddressBuf,
        CALL_MODEM_SB_DESTINATION_ADDRESS, EIsiSubBlockTypeId8Len8 );

    //add the Address type, not needed because this is not checked on ISA side
    destinationAddressBuf.Append( CALL_MODEM_NBR_PLAN_UNKNOWN );

    //add padding bytes
    destinationAddressBuf.Append( KCustomPadding );
    destinationAddressBuf.Append( KCustomPadding );

    //add number length
  //lint -e{732} Warning about "loss of sign". Root cause is that
  //TDesC::Length() returns a signed TInt
    destinationAddressBuf.Append( aNumber.Length() );

    //add the number string
    TIsiUtility::CopyToBigEndian( aNumber, destinationAddressBuf );

    // add the CALL_MODEM_DESTINATION_ADDRESS subblock
    callEmergencyNbrCheckReq.Append( destinationAddressSb.CompleteSubBlock() );

    // Sending message to phonet
    return iPhoNetSender->Send( PN_MODEM_CALL, aTransId, CALL_MODEM_EMERG_NBR_CHECK_REQ,
      callEmergencyNbrCheckReq );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallEmergencyNbrCheckResp
// Breaks a CALL_EMERGENCY_CHECK_RESP ISI message. This method
// provides information whether the given number was an emergency number or not.
// Complete CompleteCheckEmergencyNumber with status and errorValue to SOS layer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CMmCustomMessHandler::CallEmergencyNbrCheckResp
        (
        const TIsiReceiveC& aIsiMsg        // Received ISI message
        )
    {
  TFLOGSTRING("TSY: CMmCustomMessHandler::CallEmergencyNbrCheckResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLEMERGENCYNBRCHECKRESP, "CMmCustomMessHandler::CallEmergencyNbrCheckResp" );

    // Get Status
    TUint8 status( aIsiMsg.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_EMERG_NBR_CHECK_RESP_OFFSET_STATUS ));

  RMmCustomAPI::TMobileTelNumber telNumber;

    //Using the advance checking of the call server forces us to make sure
    //that the call server is not is not misleading us...
    if ( CALL_MODEM_EMERG_NBR_CHECK_PASS == status )
        {
        TUint sbStartOffSet( 0 );

        if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
          ISI_HEADER_SIZE + SIZE_CALL_MODEM_EMERG_NBR_CHECK_RESP,
          CALL_MODEM_SB_EMERG_NUMBER, EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
            {
            //get length of returned number (number of Unicode characters)
            TUint8 numberLength( aIsiMsg.Get8bit( sbStartOffSet +
                CALL_MODEM_SB_EMERG_NUMBER_OFFSET_ADDRLEN ));

             //copy 8-bit number to the 16-bit target using correct endianess
             //ISI message contains one Unicode digit in two bytes ==> multiply length by 2
             TIsiUtility::CopyFromBigEndian( aIsiMsg.GetData( sbStartOffSet +
                 CALL_MODEM_SB_EMERG_NUMBER_OFFSET_ADDR,
                 numberLength * 2 ), telNumber );

            //Check that the Number length returned by call server doesn't
            //differ from original phone number.
            //Call server uses advanced mode while checking
            //numbers. For example 2112 is valid emergency number for
            //call server -> server returns number 112. However 2112 is not
            //emergency number.
            if ( RMmCustomAPI::EEmerNumberCheckNormal ==
              iUsedEmergencyNbrCheckMode && numberLength !=
              iEmergencyNumberLength )
                {
                status = CALL_MODEM_EMERG_NBR_CHECK_FAIL;
                }
            }
        else
            {
            //acording to call server feature description, in case of succesful
            //checking the sub block should be present. Therefore we consider
            //this case as a failure.
            status = CALL_MODEM_EMERG_NBR_CHECK_FAIL;
            }
        }

  // map to result: failed is 0, successful is ptr to number
  RMmCustomAPI::TMobileTelNumber* result =
    ( ( CALL_MODEM_EMERG_NBR_CHECK_FAIL == status ) ?
      0 : &telNumber );

  // packed parameter: TBool result
  CMmDataPackage dataPackage;
  dataPackage.PackData( &result );

  TFLOGSTRING2( "TSY: TSY: CMmCustomMessHandler::CallEmergencyNbrCheckResp status: %d", status);
  TFLOGSTRING2( "TSY: TSY: CMmCustomMessHandler::CallEmergencyNbrCheckResp emergency number: %S", &telNumber);
    //we complete with KErrNone as there is always a value to return
  iMessageRouter->Complete( ECustomCheckEmergencyNumberIPC, &dataPackage,
    KErrNone );

    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccOperatorReq
// Read operator name from UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccOperatorReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccOperatorReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCOPERATORREQ, "CMmCustomMessHandler::UiccOperatorReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadOperatorNameCustom;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.fileId = KElemFileOperatorName;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccOperatorResp
// Complete operator name
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccOperatorResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccOperatorResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCOPERATORRESP, "CMmCustomMessHandler::UiccOperatorResp" );

    TInt errorValue( KErrGeneral );

    // Network provider name
    TBuf<KNwProviderNameLength> name;

    if ( KErrNone == aStatus )
        {
        errorValue = KErrNone;
        name.Copy( aFileData );
        }

    // Packed parameter: TDes* (network provider name)
    CMmDataPackage dataPackage;
    TDes* namePtr( &name );
    dataPackage.PackData( &namePtr );

    iMessageRouter->Complete(
        ECustomGetNetworkProviderNameIPC,
        &dataPackage,
        errorValue );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::SsGsmUssdSendReq
// Constructs SS_GSM_USSD_SEND_REQ ISI message from input parameters
// and sends it through phonet.
// This message can be used only to cancel ussd session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::SsGsmUssdSendReq
        (
        TUint8 aTransId             //transaction Id
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::SsGsmUssdSendReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_SSGSMUSSDSENDREQ, "CMmCustomMessHandler::SsGsmUssdSendReq" );

    // create buffer for isi msg data
    TBuf8<2> data;
    data.Append( SS_GSM_USSD_END );
    //number of sub blocks
    data.Append( 0 );

    // Message sent via Phonet and return
    return iPhoNetSender->Send( PN_SS, aTransId, SS_GSM_USSD_SEND_REQ, data  );

    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::SsGsmUssdSendResp
// Breaks a SS_USSD_SEND_RESP ISI message.
// This method gets USSD type information from message and completes cancelling
// of ussd session. Complete CancelUssdSession with KErrNone to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::SsGsmUssdSendResp
        (
        const TIsiReceiveC& aIsiMessage //received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::SsGsmUssdSendResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_SSGSMUSSDSENDRESP, "CMmCustomMessHandler::SsGsmUssdSendResp" );


    TUint ussdType( aIsiMessage.Get8bit(
                        ISI_HEADER_SIZE +
                        SS_GSM_USSD_SEND_RESP_OFFSET_USSDTYPE ) );

    if ( SS_GSM_USSD_END == ussdType )
        {
        // no packed parameters for completion
        iMessageRouter->Complete( ECustomCancelUssdSessionIPC, KErrNone );
        }

    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::Match
// This method can be checks if two TCiphListEntry entries are same.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMmCustomMessHandler::Match
        (
        const TCiphListEntry& aArg1, //first argument
        const TCiphListEntry& aArg2  //second argument
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::Match.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_MATCH, "CMmCustomMessHandler::Match" );

    TBool ret ( EFalse );

    //we are interested only in the traId value
    if ( aArg1.iTraId == aArg2.iTraId )
        {
        ret = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetModemRegStatusGetReq
// Constructs NET_MODEM_REG_STATUS_GET_REQ ISI message from input parameters
// and sends it through phonet.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::NetModemRegStatusGetReq
        (
        const TUint8 aTransactionId
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetModemRegStatusGetReq." );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETMODEMREGSTATUSGETREQ, "CMmCustomMessHandler::NetModemRegStatusGetReq" );

    // Data length buffer is 2.
    TBuf8<2> data;
    data.Append( aTransactionId );
    data.Append( NET_MODEM_REG_STATUS_GET_REQ );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_NETWORK );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    return( iPhoNetSender->Send( isimsg.Complete() ) );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetModemRegStatusGetResp
// NET_MODEM_REG_STATUS_GET_RESP message is handled in queue functionality
// in cmmnetmesshandler/cmmnetoperatornamehandler and will be completed
// there after handling.
// This method check if ECID info needs to be updated.
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::NetModemRegStatusGetResp(
       const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetModemRegStatusGetResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETMODEMREGSTATUSGETRESP, "CMmCustomMessHandler::NetModemRegStatusGetResp" );

    // Get the success code.
    TUint8 successCode( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE +
        NET_MODEM_REG_STATUS_GET_RESP_OFFSET_SUCCESSCODE ) );

    if ( NET_CAUSE_OK == successCode )
        {
        // Check if ECID info needs to be updated.
        CheckECIDInfo( aIsiMsg );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CallGsmNotificationInd
// This method breaks the CALL_MODEM_NOTIFICATION_IND ISI message.
// Complete: ECustomNotifySsNetworkEventIPC to SOS layer.
// If CALL_MODEM_SB_ALERTING_INFO sub block exist ECustomGetRemoteAlertingToneStatusIPC
// is completed to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CallGsmNotificationInd
        (
        const TIsiReceiveC& aIsiMsg // received ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd" );

    TInt ret( KErrNone );

    // For packaging
    CMmDataPackage dataPackage;

    RMmCustomAPI::TSsTypeAndMode ssTypeAndMode;
    RMmCustomAPI::TSsInfo ssInfo;

    // Initialize the default values
    ssInfo.iCallHold = RMmCustomAPI::ESsHoldNotActive;
    ssInfo.iEctCallState = RMmCustomAPI::ESsEctNotActive;
    ssInfo.iCallWait = EFalse;
    ssInfo.iConfInd = EFalse;
    ssInfo.iClirSuppReject = EFalse;
    ssInfo.iForwMode = RMmCustomAPI::EForwNotActive;
    ssInfo.iCugIndex = CALL_MODEM_CUG_DEFAULT;
    ssInfo.iChoice = RMmCustomAPI::ESsChoiceUnknown;
    ssInfo.iRemoteAddress.FillZ();
    ssTypeAndMode.iSsMode = RMmCustomAPI::ESsModeUnknown;
    ssTypeAndMode.iSsType = RMmCustomAPI::ESsAllSs;

    TUint sbStartOffSet( 0 );

    // Get CALL_MODEM_SB_SS_CODE sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_CODE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_SS_CODE sub block founded");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_SS_CODE sub block found" );

        TUint16 ssCode( aIsiMsg.Get16bit(
            sbStartOffSet + CALL_MODEM_SB_SS_CODE_OFFSET_MMISSCODE ) );

        switch ( ssCode )
            {
            case CALL_MODEM_SSC_ALL_FWDS:
            case CALL_MODEM_SSC_CFU:
            case CALL_MODEM_SSC_CFB:
            case CALL_MODEM_SSC_CFNRY:
            case CALL_MODEM_SSC_CFGNC:
            case CALL_MODEM_SSC_ALL_COND_FWD:
                {
                HandleForwarding( ssCode, aIsiMsg, ssTypeAndMode );
                break;
                }
            case CALL_MODEM_SSC_OUTGOING_BARR_SERV:
            case CALL_MODEM_SSC_INCOMING_BARR_SERV:
                {
                HandleBarring( ssCode, aIsiMsg, ssTypeAndMode );
                break;
                }
            case CALL_MODEM_SSC_CALL_WAITING:
                {
                ssTypeAndMode.iSsType = RMmCustomAPI::ESsCallWaiting;
                break;
                }
            case CALL_MODEM_SSC_CLIP:
                {
                ssTypeAndMode.iSsType = RMmCustomAPI::ESsClip;
                break;
                }
            case CALL_MODEM_SSC_CLIR:
                {
                ssTypeAndMode.iSsType = RMmCustomAPI::ESsClir;
                break;
                }
            case CALL_MODEM_SSC_COLP:
                {
                ssTypeAndMode.iSsType = RMmCustomAPI::ESsColp;
                break;
                }
            case CALL_MODEM_SSC_COLR:
                {
                ssTypeAndMode.iSsType = RMmCustomAPI::ESsColr;
                break;
                }
            default:
                {
                TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
                    switch ssCode - default.");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, switch ssCode - default" );
                break;
                }
            }
        }

    // Get CALL_GSM_SS_NOTIFY sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_NOTIFY,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_SS_NOTIFY sub block founded");
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_SS_NOTIFY sub block found" );

        // Get the SsNotifyIndictor
        TUint8 ssNotify( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_SS_NOTIFY_OFFSET_SSNOTIFICATION ) );

        // Mask the value
        // Incoming call is a forwarded call
        if ( CALL_MODEM_SSN_INCOMING_IS_FWD == ssNotify )
            {
            ssInfo.iForwMode = RMmCustomAPI::ESsIncCallIsForw;
            }
        // Incoming call has been forwarded to C
        else if ( CALL_MODEM_SSN_INCOMING_FWD == ssNotify )
            {
            ssInfo.iForwMode = RMmCustomAPI::ESsIncCallForwToC;
            }
        //Outgoing call is fowarded to C
        else if ( CALL_MODEM_SSN_OUTGOING_FWD == ssNotify )
            {
            ssInfo.iForwMode = RMmCustomAPI::ESsOutCallForwToC;
            }
        // No else
        }

    // Get CALL_MODEM_SB_SS_NOTIFY_INDICATOR sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_NOTIFY_INDICATOR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_SS_NOTIFY_INDICATOR sub block founded");
OstTrace0( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_SS_NOTIFY_INDICATOR sub block found" );

        // Get the SsNotifyIndictor
        TUint8 ssNotifyIndicator( aIsiMsg.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_SS_NOTIFY_INDICATOR_OFFSET_SSINDICATOR ) );

        // Mask the value of the Indicator
        if ( CALL_MODEM_SSI_CALL_IS_WAITING == ssNotifyIndicator ) // Call waiting
            {
            ssInfo.iCallWait = ETrue;
            }
        else if ( CALL_MODEM_SSI_MPTY == ssNotifyIndicator ) // Conference call
            {
            ssInfo.iConfInd = ETrue;
            }
        // CLIR suppresion rejected
        else if ( CALL_MODEM_SSI_CLIR_SUPPR_REJ == ssNotifyIndicator )
            {
            ssInfo.iClirSuppReject = ETrue;
            }
        // No else
        }

    // Get CALL_MODEM_SB_SS_HOLD_INDICATOR sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_HOLD_INDICATOR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_SS_HOLD_INDICATOR sub block founded");
OstTrace0( TRACE_NORMAL, DUP6_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_SS_HOLD_INDICATOR sub block found" );

        // Get the ectIndictor
        TUint8 holdIndicator( aIsiMsg.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_SS_HOLD_INDICATOR_OFFSET_SSHOLDINDICATOR ) );

        // If hold indicator is 1
        if ( holdIndicator )
            {
            ssInfo.iCallHold = RMmCustomAPI::ESsHoldActive;
            }
        else // hold indicator is 0
            {
            ssInfo.iCallHold = RMmCustomAPI::ESsHoldResume;
            }
        }

    // Get CALL_MODEM_SB_CUG_INFO sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_CUG_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_CUG_INFO sub block founded");
OstTrace0( TRACE_NORMAL, DUP7_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_CUG_INFO sub block found" );

        // Get the CUG index
        ssInfo.iCugIndex = aIsiMsg.Get16bit(
            sbStartOffSet + CALL_MODEM_SB_CUG_INFO_OFFSET_CUGIND );
        }

    // Get CALL_MODEM_SB_SS_ECT_INDICATOR sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_ECT_INDICATOR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_SS_ECT_INDICATOR sub block founded");
OstTrace0( TRACE_NORMAL, DUP8_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_SS_ECT_INDICATOR sub block found" );

        // Get the ectIndictor
        TUint8 ectIndicator( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_SS_ECT_INDICATOR_OFFSET_SSECTINDICATOR ) );

        // If ect is 1
        if ( ectIndicator )
            {
            ssInfo.iEctCallState = RMmCustomAPI::ESsEctActive;
            }
        else // ect is 0
            {
            ssInfo.iEctCallState = RMmCustomAPI::ESsEctAlerting;
            }
        }

    // Get CALL_MODEM_SB_REMOTE_ADDRESS sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_REMOTE_ADDRESS,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_REMOTE_ADDRESS sub block founded");
OstTrace0( TRACE_NORMAL, DUP9_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_REMOTE_ADDRESS sub block found" );

        // Get the presentation

        TUint8 presentationIndicator( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_PRESENTATION ) );
        // Mask with 0110 0000 to get bits 6-7
        presentationIndicator &= 0x60;

        switch ( presentationIndicator )
            {
            case CALL_MODEM_PRESENTATION_ALLOWED:
                {
                ssInfo.iChoice = RMmCustomAPI::ESsPresAllowed;
                break;
                }
            case CALL_MODEM_PRESENTATION_RESTRICTED:
                {
                ssInfo.iChoice = RMmCustomAPI::ESsPresRestricted;
                break;
                }
            case CALL_MODEM_PRESENTATION_UNAVAILABLE:
                {
                ssInfo.iChoice = RMmCustomAPI::ESsNumberNotAvailable;
                break;
                }
            default:
                {
                TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
                    - CALL_MODEM_SB_REMOTE_ADDRESS sub block\
                    - switch presentationIndicator - default.");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, switch presentationIndicator - default" );
                // The variable already has a default value
                break;
                }
            }

        // Copy the number with rigth endianess
        TUint8 remoteAddressLength( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_ADDRLEN ) );

        // Copy 8-bit name to the 16-bit target using correct endianess
        TIsiUtility::CopyFromBigEndian( aIsiMsg.GetData(
            sbStartOffSet + CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_ADDR,
            remoteAddressLength ),
            ssInfo.iRemoteAddress );
        }

    // Get CALL_MODEM_SB_ALERTING_INFO sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_ALERTING_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::CallGsmNotificationInd,\
            - CALL_MODEM_SB_ALERTING_INFO sub block founded");
OstTrace0( TRACE_NORMAL, DUP10_CMMCUSTOMMESSHANDLER_CALLGSMNOTIFICATIONIND, "CMmCustomMessHandler::CallGsmNotificationInd, CALL_MODEM_SB_ALERTING_INFO sub block found" );

        TBool playRemoteAlertToneLocally( EFalse );

        // Get alerting info
        TUint8 callAlertingInfo( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_ALERTING_INFO_OFFSET_ALERTINGINFO ) );

        // If LSB is "1" inform that remote alerting tone
        // should be generated locally.
        if ( callAlertingInfo & 0x01 )
            {
            playRemoteAlertToneLocally = ETrue;
            }

        // Complete remote alerting tone notification
        // Parameter for SOS layer: a TBool playRemoteAlertToneLocally
        dataPackage.PackData( &playRemoteAlertToneLocally );
        iMessageRouter->Complete( ECustomGetRemoteAlertingToneStatusIPC,
            &dataPackage,
            KErrNone );
        }

    // Complete SsNetworkEvent notification
    // Pack parameter: a RMmCustomAPI::TSsTypeAndMode and
    // a RMmCustomAPI::TSsInfo
    dataPackage.PackData( &ssTypeAndMode, &ssInfo );
    iMessageRouter->Complete( ECustomNotifySsNetworkEventIPC,
        &dataPackage,
        ret );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::HandleForwarding
// Breaks a CALL_MODEM_NOTIFICATION_IND ISI message.
// Gets forwardfing related information from message and sets it in
// aSsTypeAndMode for further completion in the method CallGsmNotificationIndL2
// to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::HandleForwarding
        (
        TUint16 aSsCode, //SS code
        const TIsiReceiveC& aIsiMsg,  //Isi message
        RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode //ss type and mode
        ) const
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::HandleForwarding.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_HANDLEFORWARDING, "CMmCustomMessHandler::HandleForwarding" );

    //identify the kind of forwarding
    switch ( aSsCode )
        {
        case CALL_MODEM_SSC_ALL_FWDS:
            aSsTypeAndMode.iSsType = RMmCustomAPI::ESsAllForwardings;
            break;
        case CALL_MODEM_SSC_CFU:
            aSsTypeAndMode.iSsType = RMmCustomAPI::ESsForwUncond;
            break;
        case CALL_MODEM_SSC_CFB:
            aSsTypeAndMode.iSsType = RMmCustomAPI::ESsForwBusy;
            break;
        case CALL_MODEM_SSC_CFNRY:
            aSsTypeAndMode.iSsType = RMmCustomAPI::ESsForwNoReply;
            break;
        case CALL_MODEM_SSC_CFGNC:
            aSsTypeAndMode.iSsType = RMmCustomAPI::ESsForwNoReach;
            break;
        case CALL_MODEM_SSC_ALL_COND_FWD:
            aSsTypeAndMode.iSsType = RMmCustomAPI::ESsAllCondForwardings;
            break;
        default:
            TFLOGSTRING("TSY: CMmCustomMessHandler::HandleForwarding, switch aSsCode - default.\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_HANDLEFORWARDING, "CMmCustomMessHandler::HandleForwarding, switch aSsCode - default" );
            //this case is imposible, added due code convention
            break;
        }

    TUint sbStartOffSet( 0 );

    //get call gsm ss status sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_STATUS, EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
        {
        //get the ss status
        TUint8 ssStatus ( aIsiMsg.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_SS_STATUS_OFFSET_SSSTATUS ) );

        //set the right status for the bit configuration of ssStatus
        if ( CALL_MODEM_SS_STATUS_ACTIVE & ssStatus ) //if active is present
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeActive;
            }
        // if quiescent is present
        else if ( CALL_MODEM_SS_STATUS_QUIESCENT & ssStatus )
            {
            //if registered and provisioned, and not active or quiescent
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotAvailable;
            }
        //if registered and provisioned, and not active or quiescent
        else if ( ( CALL_MODEM_SS_STATUS_REGISTERED | CALL_MODEM_SS_STATUS_PROVISIONED )
          == ssStatus )
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotActive;
            }
        else if ( CALL_MODEM_SS_STATUS_PROVISIONED == ssStatus ) //only provisioned
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotRegistered;
            }
        else //not provisioned
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotProvisioned;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::HandleBarring
// Breaks a CALL_MODEM_NOTIFICATION_IND ISI message.
// Gets barring related information from message and sets it in
// aSsTypeAndMode for further completion in the method CallGsmNotificationIndL2.
// to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::HandleBarring
        (
        TUint16 aSsCode,       //SS code
        const TIsiReceiveC& aIsiMsg,  //ISI message
        RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode //Ss type and mode
        ) const
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::HandleBarring.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_HANDLEBARRING, "CMmCustomMessHandler::HandleBarring" );

    //identify the kind of barring
    switch ( aSsCode )
        {
         case CALL_MODEM_SSC_OUTGOING_BARR_SERV:
             aSsTypeAndMode.iSsType = RMmCustomAPI::ESsOutgoingBarrServ;
             break;
         case CALL_MODEM_SSC_INCOMING_BARR_SERV:
             aSsTypeAndMode.iSsType = RMmCustomAPI::ESsIncomingBarrServ;
             break;
         default:
             TFLOGSTRING("TSY: CMmCustomMessHandler::HandleBarring, switch aSsCode - default.\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_HANDLEBARRING, "CMmCustomMessHandler::HandleBarring, switch aSsCode - default" );
             //this case is imposible, added due code convention
             break;
        }

    TUint sbStartOffSet( 0 );

    //get call gsm ss status sub block
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_NOTIFICATION_IND,
        CALL_MODEM_SB_SS_STATUS, EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
        {
        //get the ss status
        TUint8 ssStatus ( aIsiMsg.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_SS_STATUS_OFFSET_SSSTATUS ) );

        //set the right status for the bit configuration of ssStatus
        if ( CALL_MODEM_SS_STATUS_ACTIVE & ssStatus ) //if active is present
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeActive;
            }
        //if quiescent is present
        else if ( CALL_MODEM_SS_STATUS_QUIESCENT & ssStatus )
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotAvailable;
            }
        else if ( CALL_MODEM_SS_STATUS_PROVISIONED == ssStatus ) //only provisioned
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotRegistered;
            }
        else
            {
            aSsTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotProvisioned;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetSetReq
// Constructs NET_SET_REQ ISI message from input parameters
// and sends it through phonet.
// This message is created for reseting the network server
// to previous registered network when the user doesn't select
// a network from the network list got in a search
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::NetSetReq( TUint8 aTransId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetSetReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETSETREQ, "CMmCustomMessHandler::NetSetReq" );

    // Data for ISI message, buffer size is one
    TBuf8<2 + SIZE_NET_OPERATOR_INFO_COMMON> netSetReq;
    // Registered in another protocol is set to 0
    netSetReq.Append ( 0 );

    // 1 SubBlock
    netSetReq.Append ( 1 );

    TBuf8<SIZE_NET_OPERATOR_INFO_COMMON> netGsmOperatorInfoCommonBuf;
    TIsiSubBlock netGsmOperatorInfoCommonSb(
        netGsmOperatorInfoCommonBuf,
        NET_OPERATOR_INFO_COMMON,
        EIsiSubBlockTypeId8Len8 );

    netGsmOperatorInfoCommonBuf.Append( NET_SELECT_MODE_NO_SELECTION );
    netGsmOperatorInfoCommonBuf.Append( NET_INDEX_NOT_USED );

    netSetReq.Append( netGsmOperatorInfoCommonSb.CompleteSubBlock() );

    // Send message via Phonet
    return iPhoNetSender->Send(
        PN_MODEM_NETWORK,
        aTransId,
        NET_SET_REQ,
        netSetReq );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetSetResp
// Breaks a NET_SERVER_RESP ISI message.
// This message maps the CS cause value to ETel error value and complete
// CompleteResetNetServer to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::NetSetResp( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetSetResp");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETSETRESP, "CMmCustomMessHandler::NetSetResp" );

    // Get the cause
    TUint8 cause( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE +
        NET_SET_RESP_OFFSET_SUCCESSCODE ) );

    // Map the cs error as symbian value
    TInt error( CMmStaticUtility::CSCauseToEpocError(
        PN_MODEM_NETWORK,
        KTsyNetCauseCommon,
        cause ) );

    // Completion ResetNetServer method (no packed parameters)
    iMessageRouter->Complete( ECustomResetNetServerIPC, error );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetRatResp
// Breaks a NET_SERVER_RESP ISI message.
// This method get RatName sub blocks maps the CS cause value to ETel error
// value and complete ResetNetServer to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::NetRatResp( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING( "TSY: CMmCustomMessHandler::NetRatResp");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETRATRESP, "CMmCustomMessHandler::NetRatResp" );

    // Get the success code
    TUint8 successCode( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + NET_RAT_RESP_OFFSET_SUCCESSCODE ) );

    TInt ret( KErrNone );

    TUint32 supportedNetworkModes( 0 );

TFLOGSTRING2( "TSY: CMmCustomMessHandler::NetRatResp successCode : %d", successCode);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_NETRATRESP, "CMmCustomMessHandler::NetRatResp;successCode=%hhu", successCode );

    // Check if we have sub blocks in the message
    if ( aIsiMsg.Get8bit( ISI_HEADER_SIZE + NET_RAT_RESP_OFFSET_SUBBLOCKCOUNT )
        != 0 && NET_CAUSE_OK == successCode )
        {
        TUint8 subBlockIndex( 1 );
        TUint sbStartOffSet( 0 );

        // NET_RAT_INFO sub block
        while ( KErrNone == aIsiMsg.FindSubBlockOffsetByIndex(
            ISI_HEADER_SIZE + SIZE_NET_RAT_RESP,
            subBlockIndex,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // Get ratName
            TUint8 ratName( aIsiMsg.Get8bit(
                sbStartOffSet + NET_RAT_INFO_OFFSET_RATNAME ) );

            switch ( ratName )
                {
                case NET_GSM_RAT:
                    {
                    supportedNetworkModes += RMmCustomAPI::KCapsNetworkModeGsm;
                    break;
                    }
                case NET_UMTS_RAT:
                    {
                    supportedNetworkModes += RMmCustomAPI::KCapsNetworkModeUmts;
                    break;
                    }
                }
            subBlockIndex++;
            }
        }
    else
        {
        // Map the cs error as symbian value
        ret = CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_NETWORK,
            KTsyNetCauseCommon,
            successCode );
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &supportedNetworkModes );

TFLOGSTRING2( "TSY: CMmCustomMessHandler::NetRatResp - ret: %d", ret);
OstTrace1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_NETRATRESP, "CMmCustomMessHandler::NetRatResp;ret=%d", ret );

    // Completion ResetNetServer method (packed parameters)
    iMessageRouter->Complete(
        ECustomGetSystemNetworkModesIPC,
        &dataPackage,
        ret );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::GssCsServiceResp
// Breaks a GSS_CS_SERVICE_RESP ISI message.
// completes ECustomSetSystemNetworkModeIPC,
// ECustomGetCurrentSystemNetworkModesIPC, ECustomSetBandSelectionIPC or
// ECustomGetBandSelectionIPC with KErrNone to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::GssCsServiceResp (
        const TIsiReceiveC& aIsiMessage ) // Received isi message
    {
TFLOGSTRING( "TSY: CMmCustomMessHandler::GssCsServiceResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_GSSCSSERVICERESP, "CMmCustomMessHandler::GssCsServiceResp" );

    TUint8 transactId ( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GSS_CS_SERVICE_RESP_OFFSET_TRANSID ) );

    if ( KCustomTransId == transactId )
        {
        TUint8 operation ( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GSS_CS_SERVICE_RESP_OFFSET_OPERATION ) );

        // GSS_SELECTED_RAT_WRITE (0x0E)
        if ( GSS_SELECTED_RAT_WRITE == operation )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::GssCsServiceResp - Complete ECustomSetSystemNetworkModeIPC");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_GSSCSSERVICERESP, "CMmCustomMessHandler::GssCsServiceResp - Complete ECustomSetSystemNetworkModeIPC" );
            //completion ResetGssServer method (no packed parameters)
            iMessageRouter->Complete( ECustomSetSystemNetworkModeIPC, KErrNone );
            }
        // GSS_SELECTED_RAT_READ (0x9C)
        else if ( GSS_SELECTED_RAT_READ == operation )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::GssCsServiceResp - Complete ECustomGetCurrentSystemNetworkModesIPC");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_GSSCSSERVICERESP, "CMmCustomMessHandler::GssCsServiceResp - Complete ECustomGetCurrentSystemNetworkModesIPC" );
            TInt err( KErrNone );
            TUint32 ratMode ( RMmCustomAPI::KCapsNetworkModeDual );
            TUint subblockOffset( 0 );
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_GSS_CS_SERVICE_RESP,
                GSS_RAT_INFO,
                EIsiSubBlockTypeId8Len8,
                subblockOffset ) )
                {
                TUint8 mode = aIsiMessage.Get8bit( subblockOffset +
                    GSS_RAT_INFO_OFFSET_FUNCTION );
                switch ( mode )
                    {
                    case GSS_GSM_RAT:
                        {
                        ratMode = RMmCustomAPI::KCapsNetworkModeGsm;
                        break;
                        }
                    case GSS_UMTS_RAT:
                        {
                        ratMode = RMmCustomAPI::KCapsNetworkModeUmts;
                        break;
                        }
                    case GSS_DUAL_RAT:
                        {
                        ratMode = RMmCustomAPI::KCapsNetworkModeDual;
                        break;
                        }
                    default:
                        {
TFLOGSTRING("TSY: CMmCustomMessHandler::GssCsServiceResp - Unrecognized RAT");
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_GSSCSSERVICERESP, "CMmCustomMessHandler::GssCsServiceResp - Unrecognized RAT" );
                        err = KErrGeneral;
                        break;
                        }
                    }
                }
            else
                {
                err = KErrGeneral;
                }
            CMmDataPackage dataPackage;
            dataPackage.PackData( &ratMode );
            //completion ResetNetServer method (packed parameters)
            iMessageRouter->Complete( ECustomGetCurrentSystemNetworkModesIPC,
                &dataPackage, err );
            }
    // not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )
        // GSS_SELECTED_BANDS_WRITE (0x9D)
        else if ( GSS_SELECTED_BANDS_WRITE == operation )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::GssCsServiceResp - Complete ECustomSetBandSelectionIPC");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_GSSCSSERVICERESP, "CMmCustomMessHandler::GssCsServiceResp - Complete ECustomSetBandSelectionIPC" );
            iMessageRouter->Complete( ECustomSetBandSelectionIPC, KErrNone );
            }
        // GSS_SELECTED_BANDS_READ (0x9E)
        else if ( GSS_SELECTED_BANDS_READ == operation )
            {
            CompleteGetBandSelection( aIsiMessage );
            }
#endif // NCP_COMMON_S60_VERSION_SUPPORT
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::GssCsServiceFailResp
// Breaks a GSS_CS_SERVICE_FAIL_RESP ISI message.
// completes ECustomSetSystemNetworkModeIPC, ECustomSetBandSelectionIPC
// or ECustomGetBandSelectionIPC with error value to SOS layer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::GssCsServiceFailResp (
        const TIsiReceiveC& aIsiMessage ) // Received isi message
    {
    TFLOGSTRING( "TSY: CMmCustomMessHandler::GssCsServiceFailResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_GSSCSSERVICEFAILRESP, "CMmCustomMessHandler::GssCsServiceFailResp" );

    TUint8 transactId ( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        GSS_CS_SERVICE_FAIL_RESP_OFFSET_TRANSID ) );
    TUint8 operation ( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        GSS_CS_SERVICE_FAIL_RESP_OFFSET_OPERATION ) );

    // GSS_SELECTED_RAT_WRITE (0x0E)
    if ( KCustomTransId == transactId && GSS_SELECTED_RAT_WRITE == operation )
        {
        //completion ResetGssServer method (no packed parameters)
        TFLOGSTRING("TSY: CMmCustomMessHandler::GssCsServiceFailResp - ECustomSetSystemNetworkModeIPC");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_GSSCSSERVICEFAILRESP, "CMmCustomMessHandler::GssCsServiceFailResp - ECustomSetSystemNetworkModeIPC" );
        iMessageRouter->Complete( ECustomSetSystemNetworkModeIPC, KErrGeneral );
        }
// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

    else if ( GSS_SELECTED_BANDS_WRITE == operation )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::GssCsServiceFailResp - ECustomSetBandSelectionIPC");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_GSSCSSERVICEFAILRESP, "CMmCustomMessHandler::GssCsServiceFailResp - ECustomSetBandSelectionIPC" );
        iMessageRouter->Complete( ECustomSetBandSelectionIPC, KErrGeneral );
        }
    else if ( GSS_SELECTED_BANDS_READ == operation )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::GssCsServiceFailResp - ECustomGetBandSelectionIPC");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_GSSCSSERVICEFAILRESP, "CMmCustomMessHandler::GssCsServiceFailResp - ECustomGetBandSelectionIPC" );
        iMessageRouter->Complete( ECustomGetBandSelectionIPC, KErrGeneral );
        }
#endif // NCP_COMMON_S60_VERSION_SUPPORT
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadFieldReq
// Read given file from UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccReadFieldReq(
    RMmCustomAPI::TSimFileInfo& aSimFileInfo )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadFieldReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADFIELDREQ, "CMmCustomMessHandler::UiccReadFieldReq" );

    // We have received only file path, file ID is two last bytes of path
    TInt length( aSimFileInfo.iPath.Length() );
    TBuf8<2> temp;
    if ( 2 <= length )
        {
        temp.Append( aSimFileInfo.iPath.Mid( length - 2 ) );
        }
    TUint16 fileId( static_cast<TUint16> ( (temp[0] << 8 ) | temp[1] ) );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadField;
    params.dataAmount = aSimFileInfo.iSize;
    params.dataOffset = aSimFileInfo.iOffSet;
    params.fileId = fileId;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;
    params.filePath = aSimFileInfo.iPath.Mid( 0, length - 2 );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadFieldResp
// Complete read file
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccReadFieldResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadFieldResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADFIELDRESP, "CMmCustomMessHandler::UiccReadFieldResp" );

    TInt error( KErrNone );
    if ( aStatus != KErrNone )
        {
        error = KErrNotFound;
        }

    // Packed parameter for completion: pointer to TDesC8 with data
    const TDesC8* dataPtr = &aFileData;
    CMmDataPackage dataPackage;
    dataPackage.PackData( &dataPtr );
    iMessageRouter->Complete( ECustomReadSimFileIPC, &dataPackage, error);
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetRatReq
// Constructs NET_RAT_REQ ISI message from input parameters and
// sends it through phonet.This message is created to read Supported Net Rats
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::NetRatReq
        (
        TUint8 aTransId, // transaction Id
        TBool aUseCurrentRats // use current rats setting
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetRatReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETRATREQ, "CMmCustomMessHandler::NetRatReq" );

TFLOGSTRING3( "TSY: CCMmCustomMessHandler::NetRatReq - aTransId: %d, aUseCurrentRats: %d", aTransId, aUseCurrentRats );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_NETRATREQ, "CMmCustomMessHandler::NetRatReq;aTransId=%hhu;aUseCurrentRats=%hhu", aTransId, aUseCurrentRats );

    // Create message data buffer for service type, buffer size is 1
    TBuf8<1> messageData;
    // Append service type
    if ( aUseCurrentRats )
        {
        messageData.Append( NET_CURRENT_RAT );
        }
    else
        {
        messageData.Append( NET_SUPPORTED_RATS );
        }

    // Sending message to phonet
    return iPhoNetSender->Send(
        PN_MODEM_NETWORK,
        aTransId,
        NET_RAT_REQ,
        messageData );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::GssCsServiceReq
// Constructs GSS_CS_SERVICE_REQ ISI message from input parameters and
// sends it through phonet.This message is created to set System network mode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::GssCsServiceReq
        (
        TUint8 aTransId,             //transaction Id
        RMmCustomAPI::TNetworkModeCaps aNetworkModeCaps //network mode caps
        )
    {

    TFLOGSTRING2( "TSY: CMmCustomMessHandler::GssCsServiceReq - aTransId: %d",
                    aTransId );
OstTraceExt1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_GSSCSSERVICEREQ, "CMmCustomMessHandler::GssCsServiceReq;aTransId=%hhu", aTransId );

    // create message data buffer for service type, buffer size is 6
    TBuf8< KGssCsBufferSize > messageData;

    // append service type, GSS_SELECTED_RAT_WRITE (0x0E)
    messageData.Append( GSS_SELECTED_RAT_WRITE );

    // Number of sub blocks
    messageData.Append( 1 );

    // add the GSS_RAT_INFO (0x0B) subblock into ISI
    messageData.Append( GSS_RAT_INFO );

    // Length of sub block is 4 (SIZE_GSS_RAT_INFO)
    messageData.Append( SIZE_GSS_RAT_INFO );

    //Sub block for GSS_RAT_INFO_FUNCTIONS
    if ( RMmCustomAPI::KCapsNetworkModeGsm == aNetworkModeCaps )
        {
        // GSS_GSM_RAT (0x01) is GSM mode
        messageData.Append( GSS_GSM_RAT );
        }
    else if ( RMmCustomAPI::KCapsNetworkModeUmts == aNetworkModeCaps )
        {
        // GSS_UMTS_RAT (0x02) is Umts mode
        messageData.Append( GSS_UMTS_RAT );
        }
    else
        {
        // GSS_DUAL_RAT (0x00) is Dual mode
        messageData.Append( GSS_DUAL_RAT );
        }

    // Padding byte
    messageData.Append( KCustomPadding );

    // Sending message to phonet
    return iPhoNetSender->Send( PN_GSS, aTransId, GSS_CS_SERVICE_REQ, messageData );

    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::GssCsServiceReq
// Constructs GSS_CS_SERVICE_REQ ISI message from input parameters and
// sends it through phonet.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::GssCsServiceReq
        (
        TUint8 aTransId   //transaction Id
        )
    {
TFLOGSTRING( "TSY: CMmCustomMessHandler::GssCsServiceReq" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_GSSCSSERVICEREQ, "CMmCustomMessHandler::GssCsServiceReq" );

    TBuf8<KTwo> messageData;

    messageData.Append( GSS_SELECTED_RAT_READ );
    messageData.Append( 0 ); // number of subblocks

    // Sending message to phonet
    return iPhoNetSender->Send( PN_GSS, aTransId, GSS_CS_SERVICE_REQ, messageData );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ExtFuncL
// Forwards requests coming from the Symbian OS layer to the
// specific method.
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::ExtFuncL
        (
        TInt aIpc, //Ipc
        const CMmDataPackage* aDataPackage  //data package
        )
    {

    //*************************************************************//
    // NOTE
    //
    // LICENSEE SPECIFIC MESSAGE HANDLER IMPLEMENTATION STARTS HERE
    //
    //*************************************************************//

    TInt ret( KErrNone );

    TUint8 transId = KCustomTransId;

    TFLOGSTRING2("TSY: CMmCustomMessHandler::ExtFuncL -- aIpc -- (%d)", aIpc );
OstTrace1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_EXTFUNCL, "CMmCustomMessHandler::ExtFuncL;aIpc=%d", aIpc );

    switch ( aIpc )
        {
        case ECustomCancelUssdSessionIPC:
            {
            //no packed parameters
            ret = SsGsmUssdSendReq( transId );
            break;
            }
        case ECustomClearCallBlackListIPC:
            {
            //no packed parameters
            ret = CallGsmBlackListClearReq( transId );
            break;
            }
        case ECustomCheckEmergencyNumberIPC:
            {
            // packed parameter: a RMmCustomAPI::TEmerNumberCheckMode
            RMmCustomAPI::TEmerNumberCheckMode checkMode;
            aDataPackage->UnPackData( checkMode );
            ret = CallEmergencyNbrCheckReq( transId, checkMode.iNumber,
                                            checkMode.iCheckMode );
            break;
            }
        case ECustomTerminateCallIPC:
            {
            // packed parameters: pointer to an array of TInt (Call Id)
            // and TBool isError
            TInt callId;
            TBool isError;
            aDataPackage->UnPackData( callId, isError );

            // map Custom API values to ISA values
            // cause Value (error or user request)
            TUint8 causeValue =
                ( isError ? CALL_MODEM_CAUSE_ERROR_REQUEST : CALL_MODEM_CAUSE_RELEASE_BY_USER );

            // call Id
            TUint8 callIdISA = 0;
            // martpiir 27-May-2004: This cannot remain, must be checked
            // jlof 29-Sep-2004: "all calls" id was originally -2, but now
            // negative or zero
            // call id means all calls. Taking absolute value of it gives
            // alerting call id.
            if ( 0 >= callId )
                {
                TFLOGSTRING("TSY: CMmCustomMessHandler::ExtFuncL -- ECustomTerminateCallIPC -- (hold+active+alerting)");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_EXTFUNCL, "CMmCustomMessHandler::ExtFuncL - ECustomTerminateCallIPC - (hold+active+alerting)" );
                // alerting call (if any) is included as negated value..
                callIdISA = TUint8( CALL_MODEM_ID_HOLD | CALL_MODEM_ID_ACTIVE | ( -callId ));
                }
            else if ( 1 <= callId && 7 >= callId )
                {
                TFLOGSTRING2("TSY: CMmCustomMessHandler::ExtFuncL -- ECustomTerminateCallIPC -- (%d)", TInt(callId) );
OstTrace1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_EXTFUNCL, "CMmCustomMessHandler::ExtFuncL;callId=%d", callId );
                callIdISA = static_cast<TUint8>( callId );
                }
            else
                {
                // no call found
                ret = KErrArgument;
                }

            if ( KErrNone == ret )
                {
                ret = CallReleaseReq( transId, callIdISA, causeValue );
                }
            break;
            }
        case ECustomGetAlsBlockedIPC:
            {
            ret = UiccReadDynamic2FlagsReq();
            break;
            }
        case ECustomSetAlsBlockedIPC:
            {
            // Packed parameter: TSetAlsBlock (block status)
            RMmCustomAPI::TSetAlsBlock blockStatus;
            aDataPackage->UnPackData( blockStatus );

            // Map Custom API values to ISA values
            TUint8 blockStatusISA = static_cast<TUint8>( blockStatus );
            ret = UiccWriteDynamic2FlagsReq( blockStatusISA );
            break;
            }
        case ECustomCheckAlsPpSupportIPC:
            {
            //no packed parameters
            // request type is used as transaction id. needed for
            // completion.
            ret = InfoPpReadReq( EGetAlsPPSupport );
            break;
            }
        case ECustomCheckTwoDigitDialSupportIPC:
            {
            //no packed parameters
            // request type is used as transaction id. needed for
            // completion.
            ret = InfoPpReadReq( ECheckTwoDigitDialSupport );
            break;
            }
        case ECustomGetCipheringInfoIPC:
            {
            //no packed parameters
            ret = UiccReadCiReq( ETrIdReadCipheringIndicatorStatusCustom );
            break;
            }
        case ECustomNetWakeupIPC:
            {
            //no packed parameters
            ret = NetCsWakeupReq( transId );
            break;
            }
        case ECustomGetOperatorNameIPC:
            {
            //no packed parameters
            ret = NetModemRegStatusGetReq( transId );
            break;
            }
        case ECustomResetNetServerIPC:
            {
            //no packed parameters
            ret = NetSetReq( transId );
            break;
            }
        case ECustomGetNetworkProviderNameIPC:
            {
            ret = UiccOperatorReq();
            break;
            }
        case ECustomReadSimFileIPC:
            {
            //packed parameter: RMmCustomAPI::TSimFileInfo
            RMmCustomAPI::TSimFileInfo simFileInfo;
            aDataPackage->UnPackData( simFileInfo );
            ret = UiccReadFieldReq( simFileInfo );
            break;
            }
        case EMobilePhoneIMSAuthenticate:
            {
            ret = UiccHandleImsAuthentication( *aDataPackage );
            break;
            }
        case ECustomGetSimAuthenticationDataIPC:
            {
            ret = UiccSendAuthenticateApdu( *aDataPackage );
            break;
            }
        // Supported network modes
        case ECustomGetSystemNetworkModesIPC:
            {
            TBool useCurrentRats;
            aDataPackage->UnPackData( useCurrentRats );
            ret = NetRatReq( transId, useCurrentRats );
            break;
            }
        // Set system network mode
        case ECustomSetSystemNetworkModeIPC:
            {
            RMmCustomAPI::TNetworkModeCaps networkModeCaps;
            aDataPackage->UnPackData( networkModeCaps );
            ret = GssCsServiceReq( transId, networkModeCaps  );
            break;
            }
        // Current network modes
        case ECustomGetCurrentSystemNetworkModesIPC:
            {
            ret = GssCsServiceReq( transId );
            break;
            }
        // Get GSM/WCDMA cell info
        case ECustomGetCellInfoIPC:
            {
            iECIDInfoRequested = ETrue;
            ret = NetNeighbourCellsReq( transId );
            break;
            }

        case ECustomSimWarmResetIPC:
            {
            ret = UiccSimWarmResetReq();
            break;
            }

        case ECustomGetATRIPC:
            {
            ret = UiccSimGetAtrReq();
            break;
            }

        case ECustomGetSimCardReaderStatusIPC:
            {
            ret = UiccCardReaderStatusReq();
            break;
            }

        case ECustomSendAPDUReqV2IPC:
            {
            // packed parameter: RMmCustomAPI::TApduParameters
            RMmCustomAPI::TApduParameters apduParameters;
            TInt traId( ETrIdSendAPDUv2 );

            //Unpack aDataPackage
            aDataPackage->UnPackData( apduParameters );

            ret = UiccSendAPDUReq( apduParameters, traId);
            break;
            }
        case ECustomPowerSimOnIPC:
            {
            ret = UiccSimPowerOnReq();
            break;
            }
        case ECustomPowerSimOffIPC:
            {
            ret = UiccSimPowerOffReq();
            break;
            }

        case ECustomSendAPDUReqIPC:
            {
            // packed parameter: RMmCustomAPI::TApdu
            RMmCustomAPI::TApdu apduData;
            RMmCustomAPI::TApduParameters apduParameters;
            TInt traId( ETrIdSendAPDU );

            //Unpack aDataPackage
            aDataPackage->UnPackData( apduData );

            apduParameters.iCmdData.Append(*apduData.iData);

            ret = UiccSendAPDUReq( apduParameters, traId );
            break;
            }

        //from MmSIMMessHandler
        case ECustomStartSimCbTopicBrowsingIPC:
            {
            ret = UiccReadCbMsgIdsReq( ETrIdReadCbMsgIds );
            break;
            }
        case ECustomDeleteSimCbTopicIPC:
            {
            // packed parameter: TUint with topic id
            aDataPackage->UnPackData( iSimCBTopicToBeDeleted,
                iTopicInSimMemoryDelete );
            ret = UiccReadCbMsgIdsReq( ETrIdUpdateCbMsgIdsPhase1 );
            break;
            }
        case EReadViagHomeZoneParamsIPC:
            {
            ret = UiccReadViagHomeZoneParametersReq();
            break;
            }
        case EReadViagHomeZoneCacheIPC:
            {
            // packed parameter: RMmCustomAPI::TViagCacheRecordId
            RMmCustomAPI::TViagCacheRecordId recordId;
            aDataPackage->UnPackData( recordId );
            ret = UiccReadViagHomeZoneCacheReq( recordId );
            break;
            }
        case EWriteViagHomeZoneCacheIPC:
            {
            // packed parameters: RMmCustomAPI::TViagCacheRecordId
            // and RMmCustomAPI::TViagCacheRecordContent
            RMmCustomAPI::TViagCacheRecordId recordId;
            RMmCustomAPI::TViagCacheRecordContent recordContent;
            aDataPackage->UnPackData( recordId, recordContent );
            ret = UiccWriteViagHomeZoneCacheReq( recordId, recordContent );
            break;
            }
        //WCDMA HZ
        case EWriteViagHomeZoneUHZIUESettingsIPC:
            {
            //Unpack the packed settings data
            RMmCustomAPI::TViagUHZIUESettings settingUhziui;
            aDataPackage->UnPackData( settingUhziui );
            ret = UiccWriteViagHomeZoneUhziueSettingsReq( settingUhziui );
            break;
            }
        case ECustomWriteHSxPAStatusIPC:
            {
            RMmCustomAPI::THSxPAStatus aStatus;
            aDataPackage->UnPackData( aStatus );
            ret = WriteHSxPAStatusReq( transId, aStatus );
            break;
            }

        case ECustomReadHSxPAStatusIPC:
            {
            ret = ReadHSxPAStatusReq( transId );
            break;
            }

// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

        // Set custom band selection
        case ECustomSetBandSelectionIPC:
            {
            RMmCustomAPI::TNetworkModeCaps networkModeCaps;
            RMmCustomAPI::TBandSelection bandSelection;
            aDataPackage->UnPackData( bandSelection, networkModeCaps );

            ret = GssCsServiceSetBandReq( transId,
                networkModeCaps,
                bandSelection );
            break;
            }

        // Get custom band selection
        case ECustomGetBandSelectionIPC:
            {
            ret = GssCsServiceGetBandReq( transId );
            break;
            }

#endif // NCP_COMMON_S60_VERSION_SUPPORT

        case EMmTsyUpdateLifeTimeIPC:
            {
            TTimeIntervalSeconds time;
            aDataPackage->UnPackData( time );

            ret = WriteLifeTimerToPermanentMemory( transId, time );

            break;
            }

        case ECustomGetLifeTimeIPC:
            {
            //no packed parameters
            ret = GetTotalLifeTimerValue();
            break;
            }

        default:
            {
            // this method should only be called for Custom Tsy cases
            TFLOGSTRING2("TSY: CMmCustomMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_EXTFUNCL, "CMmCustomMessHandler::ExtFuncL;aIpc=%d", aIpc );
            ret = KErrArgument;
            break;
            }
        }

    return ret;
  }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ProcessUiccMsg
// Handles data received from UICC server
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY: CMmCustomMessHandler::ProcessUiccMsg, transaction ID: %d, status: %d", aTraId, aStatus );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_PROCESSUICCMSG, "CMmCustomMessHandler::ProcessUiccMsg" );

    TInt ret( KErrNone );

    switch( aTraId )
        {
        case ETrIdReadField:
            {
            UiccReadFieldResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadCbMsgIds:
        case ETrIdUpdateCbMsgIdsPhase1:
            {
            UiccReadCbMsgIdsResp( aStatus, aTraId, aFileData );
            break;
            }
        case ETrIdUpdateCbMsgIdsPhase2:
            {
            UiccDeleteCbMsgIdResp( aStatus );
            break;
            }
        case ETrIdReadCipheringIndicatorStatusCustom:
        case ETrIdNotifyCipheringIndicatorStatusCustom:
            {
            UiccReadCiResp( aStatus, aTraId, aFileData );
            break;
            }
        case ETrIdReadOperatorNameCustom:
            {
            UiccOperatorResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadViagHomeZoneParamsCustom:
            {
            UiccReadViagHomeZoneParametersResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadViagHomeZoneCacheCustom:
            {
            UiccReadViagHomeZoneCacheResp( aStatus, aFileData );
            break;
            }
        case ETrIdWriteViagHomeZoneCacheCustom:
            {
            UiccWriteViagHomeZoneCacheResp( aStatus );
            break;
            }
        case ETrIdWriteViagHomeZoneUhziueSettingsCustom:
            {
            UiccWriteViagHomeZoneUhziueSettingsResp( aStatus );
            break;
            }
        case ETrIdReadDynamic2Flags:
            {
            UiccReadDynamic2FlagsResp( aStatus, aFileData );
            break;
            }
        case ETrIdWriteDynamic2Flags:
            {
            UiccWriteDynamic2FlagsResp( aStatus );
            break;
            }

        case ETrIdSimPowerOff:
            {
            UiccSimPowerOffResp( aStatus );
            break;
            }

        case ETrIdSimWarmReset:
            {
            UiccSimWarmResetResp( aStatus );
            break;
            }

        case ETrIdSimGetATR:
            {
            UiccSimGetAtrResp( aStatus, aFileData );
            break;
            }

        case ETrIdSimPowerOn:
            {
            UiccSimPowerOnResp( aStatus );
            break;
            }

        case ETrIdSendAPDUv2:
        case ETrIdSendAPDU:
            {
            UiccSendAPDUResp( aStatus, aFileData, aTraId );
            break;
            }

        case ETrIdCardReaderStatus:
            {
            UiccCardReaderStatusResp( aStatus, aFileData );
            break;
            }
        case ETrIdEEapSimAuthenticate:
            {
            UiccGsmSecurityContextApduResp( aTraId, aStatus, aFileData );
            break;
            }
        case ETrIdEEapAkaAuthenticate:
        case ETrIdEEapAkaAuthenticateIms:
            {
            Uicc3GSecurityContextApduResp( aTraId, aStatus, aFileData );
            break;
            }
        case ETrIdEGbaBootstrap:
            {
            UiccGBABootstrappingApduResp( aTraId, aStatus, aFileData );
            break;
            }
        case ETrIdEGbaBootstrapRead:
            {
            UiccGBABootstrapReadResp( aStatus, aFileData );
            break;
            }
        case ETrIdEGbaBootstrapUpdate:
            {
            UiccGBABootstrapUpdateResp( aStatus );
            break;
            }
        case ETrIdEGbaNafDerivation:
            {
            UiccGBANafDerivationApduResp( aTraId, aStatus, aFileData );
            break;
            }
        case ETrIdEMbmsMskUpdate:
            {
            UiccMbmsMskUpdateApduResp( aStatus, aFileData );
            break;
            }
        case ETrIdEMbmsMtkGeneration:
            {
            UiccMbmsMtkGenerationApduResp( aStatus, aFileData );
            break;
            }
        case ETrIdEMbmsMskDeletion:
            {
            UiccMbmsMskDeletionApduResp( aStatus, aFileData );
            break;
            }
        case ETrIdRunGsmAlgorithmSim:
        case ETrIdRunGsmAlgorithmAka:
        case ETrIdRunGsmAlgorithmIms:
            {
            UiccRunGsmAlgorithmApduResp( aTraId, aStatus, aFileData );
            break;
            }
        case ETrIdActivateIsimApplication:
            {
            UiccHandleIsimActivationResp( aStatus );
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmNetMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_PROCESSUICCMSG, "CMmCustomMessHandler::ProcessUiccMsg - unknown transaction ID" );

            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadViagHomeZoneParametersReq
// Read home zone parameters from UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccReadViagHomeZoneParametersReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadViagHomeZoneParametersReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADVIAGHOMEZONEPARAMETERSREQ, "CMmCustomMessHandler::UiccReadViagHomeZoneParametersReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadViagHomeZoneParamsCustom;
    params.dataAmount = 0;
    params.dataOffset = 0;
    params.fileId = KElemFileHomeZone;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( KO2DedicatedFile >> 8 );
    params.filePath.Append( KO2DedicatedFile );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadViagHomeZoneParametersResp
// Complete home zone parameters
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccReadViagHomeZoneParametersResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadViagHomeZoneParametersResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADVIAGHOMEZONEPARAMETERSRESP, "CMmCustomMessHandler::UiccReadViagHomeZoneParametersResp" );

    TInt error( KErrNone );
    if ( KErrNone == aStatus )
        {
        TUint8 length( 0 );
        TUint8 i( 0 );
        TUint8 j( 0 );
        TUint8 index( 2 );

        // Viag params, reset
        iViagParams.iScp.Zero();
        iViagParams.iSmsC.Zero();
        iViagParams.iSubscribedZoneAndVersion = 0;

        // Set homezone subscription and version ( byte 1 )
        iViagParams.iSubscribedZoneAndVersion = aFileData[0];

        // Set SMSC number ( 7 bytes, not include length )
        length = aFileData[1];
        for ( i = 0; i < length; i++ )
            {
            iViagParams.iSmsC.Append( aFileData[index++] );
            }

        // Set SCP number
        length = aFileData[9];
        index = 10;
        for ( i = 0; i < length; i++ )
            {
            iViagParams.iScp.Append( aFileData[index++] );
            }

        // Viag elements
        RMmCustomAPI::TViagElements* viagElems(
            new (ELeave) RMmCustomAPI::TViagElements
            ( RMmCustomAPI::KViagElementCount ) );
        CleanupStack::PushL( viagElems );
        TUint8 elemDataIndex( 17 ); // Home zone elements stars from byte 18
        TUint32 tempCoordinate( 0 );

        TUint8 byte1( 0 );
        TUint8 byte2( 0 );
        TUint8 byte3( 0 );
        TUint8 byte4( 0 );

        // Set viag elements
        for ( ; j < RMmCustomAPI::KViagElementCount; j++ )
            {
            RMmCustomAPI::TViagElement elem;
            // Set zone id
            elem.iCoordinates.iZoneId = aFileData[ elemDataIndex ];

            // Next four bytes are for x coordinate
            byte1 = aFileData[++elemDataIndex];
            byte2 = aFileData[++elemDataIndex];
            byte3 = aFileData[++elemDataIndex];
            byte4 = aFileData[++elemDataIndex];

            tempCoordinate = ( byte1 << 24 ) |
                ( byte2 << 16 ) |
                ( byte3 << 8 ) |
                ( byte4 );
            elem.iCoordinates.iX = tempCoordinate;

            // Next four bytes are for y coordinate
            byte1 = aFileData[++elemDataIndex];
            byte2 = aFileData[++elemDataIndex];
            byte3 = aFileData[++elemDataIndex];
            byte4 = aFileData[++elemDataIndex];

            tempCoordinate = ( byte1 << 24 ) |
                ( byte2 << 16 ) |
                ( byte3 << 8 ) |
                ( byte4 );
            elem.iCoordinates.iY = tempCoordinate;

            // Next four bytes are for r2
            byte1 = aFileData[++elemDataIndex];
            byte2 = aFileData[++elemDataIndex];
            byte3 = aFileData[++elemDataIndex];
            byte4 = aFileData[++elemDataIndex];

            tempCoordinate = ( byte1 << 24 ) |
                ( byte2 << 16 ) |
                ( byte3 << 8 ) |
                ( byte4 );
            elem.iCoordinates.iR2 = tempCoordinate;

            // Set active flag
            // Bit 1 indicates homzone active
            TBool isActive(
                ( aFileData[++elemDataIndex] & KHomeZoneActiveBit ) );

            if ( isActive )
                {
                // Bit 2 indicates if an active homezone is marked as a cityzone
                TBool isCityZone(
                    ( aFileData[elemDataIndex] & KCityZoneActiveBit ) );
                if ( isCityZone )
                    {
                    elem.iActiveFlag = RMmCustomAPI::ECityZone;
                    }
                else
                    {
                    elem.iActiveFlag = RMmCustomAPI::EHomeZone;
                    }
                }
            else
                {
                elem.iActiveFlag = RMmCustomAPI::ENotActive;
                }

            // Set name (12 bytes)
            elem.iName.Zero();
            for ( i = 0; i < 12; i++ )
                {
                elem.iName.Append( aFileData[++elemDataIndex] );
                }

            viagElems->AppendL( elem );
            elemDataIndex++;
            }

        // Complete
        CMmDataPackage dataPackage;
        dataPackage.PackData( &iViagParams, &viagElems );
        iMessageRouter->Complete( EReadViagHomeZoneParamsIPC,
            &dataPackage, error );

        //delete iViagElems;
        CleanupStack::PopAndDestroy(viagElems);
        }
    else
        {
        // Complete with error value
        iMessageRouter->Complete(
            EReadViagHomeZoneParamsIPC,
            KErrAccessDenied );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadVIAGHomeZoneCacheReq
// Read home zone cache
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccReadViagHomeZoneCacheReq(
    const RMmCustomAPI::TViagCacheRecordId& aRecordId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadViagHomeZoneCacheReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADVIAGHOMEZONECACHEREQ, "CMmCustomMessHandler::UiccReadViagHomeZoneCacheReq" );

    TUint8 cacheId( aRecordId.iCacheId );
    TUint8 recordId( aRecordId.iRecordId );
    TUint16 fileId( 0 );

    // The record numbers in SIM Server range from 1 to 21. Client's record
    // numbers range from 0 to 20. We have to increase the index by one.
    recordId++;

    // Define file ID according to cache ID
    switch( cacheId )
        {
        case 1:
            {
            fileId = KElemFileHomeZoneCache1;
            break;
            }
        case 2:
            {
            fileId = KElemFileHomeZoneCache2;
            break;
            }
        case 3:
            {
            fileId = KElemFileHomeZoneCache3;
            break;
            }
        case 4:
            {
            fileId = KElemFileHomeZoneCache4;
            break;
            }
        default:
            {
            break;
            }
        }

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadViagHomeZoneCacheCustom;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = recordId;

    params.fileId = fileId;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( KO2DedicatedFile >> 8 );
    params.filePath.Append( KO2DedicatedFile );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadViagHomeZoneCacheResp
// Complete home zone cache
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccReadViagHomeZoneCacheResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadViagHomeZoneCacheResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADVIAGHOMEZONECACHERESP, "CMmCustomMessHandler::UiccReadViagHomeZoneCacheResp" );

    TInt ret( KErrAccessDenied );

    RMmCustomAPI::TViagCacheRecordContent viagRecord;

    if ( KErrNone == aStatus )
        {
        viagRecord.iLac = ( aFileData[0] << 8 ) | aFileData[1];
        viagRecord.iCellId = ( aFileData[2] << 8 ) | aFileData[3];
        ret = KErrNone;
        }
    else
        {
        viagRecord.iLac = NULL;
        viagRecord.iCellId = NULL;
        }

    // Complete, packed parameter: RMmCustomAPI::TViagCacheRecordContent
    CMmDataPackage dataPackage;
    dataPackage.PackData( &viagRecord );
    iMessageRouter->Complete( EReadViagHomeZoneCacheIPC, &dataPackage, ret );

    return;
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccWriteVIAGHomeZoneCacheReq
// Write home zone cache
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccWriteViagHomeZoneCacheReq(
    const RMmCustomAPI::TViagCacheRecordId& aRecordId,
    const RMmCustomAPI::TViagCacheRecordContent& aViagRecordContent )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccWriteViagHomeZoneCacheReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCWRITEVIAGHOMEZONECACHEREQ, "CMmCustomMessHandler::UiccWriteViagHomeZoneCacheReq" );

    TUint8 cacheId( (TUint8)aRecordId.iCacheId );
    TUint8 recordId( (TUint8)aRecordId.iRecordId );
    TUint16 lac( (TUint16)aViagRecordContent.iLac );
    TUint16 cellId( (TUint16)aViagRecordContent.iCellId );
    TUint16 fileId( 0 );

    // The record numbers in SIM Server range from 1 to 21. Client's record
    // numbers range from 0 to 20. We have to shift the index by one.
    recordId++;

    // Define file ID according to cache ID
    switch( cacheId )
        {
        case 1:
            {
            fileId = KElemFileHomeZoneCache1;
            break;
            }
        case 2:
            {
            fileId = KElemFileHomeZoneCache2;
            break;
            }
        case 3:
            {
            fileId = KElemFileHomeZoneCache3;
            break;
            }
        case 4:
            {
            fileId = KElemFileHomeZoneCache4;
            break;
            }
        default:
            {
            break;
            }
        }

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteViagHomeZoneCacheCustom;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = recordId;

    params.fileId = fileId;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( KO2DedicatedFile >> 8 );
    params.filePath.Append( KO2DedicatedFile );

    // File data to be updated.
    TBuf8<4> fileDataBuf;
    fileDataBuf.Append( lac >> 8 );
    fileDataBuf.Append( lac & 0x00FF );
    fileDataBuf.Append( cellId >> 8);
    fileDataBuf.Append( cellId & 0x00FF );
    params.fileData.Append( fileDataBuf );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccWriteViagHomeZoneCacheResp
// Complete home zone cache
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccWriteViagHomeZoneCacheResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccWriteViagHomeZoneCacheResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCWRITEVIAGHOMEZONECACHERESP, "CMmCustomMessHandler::UiccWriteViagHomeZoneCacheResp" );

    if ( KErrNone == aStatus )
        {
        iMessageRouter->Complete( EWriteViagHomeZoneCacheIPC, KErrNone );
        }
    else
        {
        iMessageRouter->Complete( EWriteViagHomeZoneCacheIPC, KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsReq
// Write home zone settings to UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsReq(
    const RMmCustomAPI::TViagUHZIUESettings& aUhziuiSettings )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCWRITEVIAGHOMEZONEUHZIUESETTINGSREQ, "CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsReq" );

    TUint8 settingsField( aUhziuiSettings.iSettings );
    TUint8 versionField( aUhziuiSettings.iVersion );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteViagHomeZoneUhziueSettingsCustom;
    params.dataOffset = 0;

    params.fileId = KElemFileUhziueSettings;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( KO2DedicatedFile >> 8 );
    params.filePath.Append( KO2DedicatedFile );

    // File data to be updated.
    TBuf8<2> fileDataBuf;
    fileDataBuf.Append( settingsField );
    fileDataBuf.Append( versionField);
    params.fileData.Append( fileDataBuf );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsResp
// Complete home zone settings
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsResp(
    TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCWRITEVIAGHOMEZONEUHZIUESETTINGSRESP, "CMmCustomMessHandler::UiccWriteViagHomeZoneUhziueSettingsResp" );

    TInt status( KErrAccessDenied );

    if ( KErrNone == aStatus  )
        {
        status = KErrNone;
        }
    iMessageRouter->Complete( EWriteViagHomeZoneUHZIUESettingsIPC, status );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadCbMsgIdsReq
// Read CB message IDs from UICC
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccReadCbMsgIdsReq( TUiccTrId aTrId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadCbMsgIdsReq" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADCBMSGIDSREQ, "CMmCustomMessHandler::UiccReadCbMsgIdsReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = aTrId;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.fileId = KElemFileCellBroadcastMessId;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccReadCbMsgIdsResp
// Complete CB message IDs
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccReadCbMsgIdsResp(
    TInt aStatus,
    TInt aTraId,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccReadCbMsgIdsResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCREADCBMSGIDSRESP, "CMmCustomMessHandler::UiccReadCbMsgIdsResp" );

    TInt numOfCbIds( 0 );
    // File data in bytes
    TInt length( aFileData.Length() );
    // Divide by two to get number of 16-bit CB ids
    numOfCbIds = length >> 1;
    // Buffer for 16 bits CB identifiers
    TBuf16<KSimNumberOfCbMsgIds> cbIds;
    TInt i( 0 );
    TInt j( 0 );
    // Loop all the bytes and append 16 bit id to buffer
    for ( ; i < length && j < numOfCbIds; i += 2, j++ )
        {
        TUint16 cbId ( ( aFileData[i] << 8 ) | ( aFileData[i+1] ) );
        cbIds.Append( cbId );
        }
    // Read request, complete CB topic list to CTSY
    if ( KErrNone == aStatus && ETrIdReadCbMsgIds == aTraId && numOfCbIds > 0 )
        {
        CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* simCbTopics = new ( ELeave )
            CArrayFixFlat<RMmCustomAPI::TSimCbTopic> ( numOfCbIds );

        for ( i = 0; i < numOfCbIds; i++ )
            {
            if ( KUnusedCbMsgId != cbIds[i] )
                {
                RMmCustomAPI::TSimCbTopic simCbTopic;
                // CB ID
                simCbTopic.iNumber = cbIds[i];
                // Topic names are not stored on the SIM
                simCbTopic.iName.Zero();
                // Add CB topic to array
                simCbTopics->AppendL( simCbTopic );
                }
             }
        simCbTopics->Compress();

        // Complete to SOS layer with packed parameter:
        // pointer to array with CB topics
        CMmDataPackage dataPackage;
        dataPackage.PackData( &simCbTopics );

        iMessageRouter->Complete(
            ECustomStartSimCbTopicBrowsingIPC,
            &dataPackage,
            KErrNone );
        }
    // Delete request, update EF CBMI with new topic list
    else if ( KErrNone == aStatus && ETrIdUpdateCbMsgIdsPhase1 == aTraId &&
        numOfCbIds > 0 && iTopicInSimMemoryDelete )
        {
        // Set parameters for UICC_APPL_CMD_REQ message
        TUiccWriteTransparent params;
        params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        params.trId = ETrIdUpdateCbMsgIdsPhase2;
        params.dataOffset = 0;
        params.fileId = KElemFileCellBroadcastMessId;
        params.fileIdSfi = UICC_SFI_NOT_PRESENT;
        params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;

        // File id path
        params.filePath.Append( KMasterFileId >> 8 );
        params.filePath.Append( KMasterFileId );
        params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

        // File data to be updated.
        TBuf8<KSimNumberOfCbMsgIds*2> fileDataBuf;
        for ( i = 0; i < numOfCbIds; i++ )
            {
            if ( cbIds[ i ] != iSimCBTopicToBeDeleted )
                {
                fileDataBuf.Append( cbIds[ i ] >> 8 ); // MSB
                fileDataBuf.Append( cbIds[ i ] & 0x00FF ); // LSB
                }
            else // This CB id is deleted-> set unused
                {
                fileDataBuf.Append( 0xFF );
                fileDataBuf.Append( 0xFF );
                }
            }
        iSimCBTopicToBeDeleted = KUnusedCbMsgId;
        params.fileData.Append( fileDataBuf );
        iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    else // Complete error values
        {
        if ( ETrIdReadCbMsgIds == aTraId )
            {
            iMessageRouter->Complete(
                ECustomStartSimCbTopicBrowsingIPC,
                KErrNotFound );
            }
        else // Delete request
            {
            iMessageRouter->Complete(
                ECustomDeleteSimCbTopicIPC,
                KErrNotFound );
            }
        iTopicInSimMemoryDelete = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccDeleteCbMsgIdResp
// Delete CB message ID
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccDeleteCbMsgIdResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccDeleteCbMsgIdResp" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCDELETECBMSGIDRESP, "CMmCustomMessHandler::UiccDeleteCbMsgIdResp" );

    TInt err( KErrNone );
    if ( KErrNone != aStatus )
        {
        err = KErrAccessDenied;
        }

    iMessageRouter->Complete( ECustomDeleteSimCbTopicIPC, err );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::WriteHSxPAStatusReq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::WriteHSxPAStatusReq
        (
        TUint8 aTraId,                      // Transaction identification
        RMmCustomAPI::THSxPAStatus status   // HSxPA status
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::WriteHSxPAStatusReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_WRITEHSXPASTATUSREQ, "CMmCustomMessHandler::WriteHSxPAStatusReq" );

    TBuf8<KHSxPAMaxMessageSize> data;
    TUint8 value(0);

    if ( RMmCustomAPI::EHSxPAEnabled == status )
        {
        value = GSS_HSXPA_ALLOWED;
        }
    else // ( RMmCustomAPI::EHSxPADisabled == status )
        {
        value = GSS_HSXPA_DISABLED;
        }

    data.Append( value );
    data.Append( KCustomPadding ); //Filler

    return iPhoNetSender->Send( PN_GSS,
                                aTraId,
                                GSS_HSXPA_USER_SETTING_WRITE_REQ,
                                data );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::WriteHSxPAStatusResp
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::WriteHSxPAStatusResp
        (
        const TIsiReceiveC& aIsiMessage    // an ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::WriteHSxPAStatusResp");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_WRITEHSXPASTATUSRESP, "CMmCustomMessHandler::WriteHSxPAStatusResp" );

    TInt epocError( KErrWrite );

    TUint8 writeStatus = aIsiMessage.Get8bit(
      ISI_HEADER_SIZE + GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_WRITESTATUS );

    TUint8 reason = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_FAILCAUSE );

    if ( GSS_OK == writeStatus )
        {
        epocError = KErrNone;
        }
    // GSS_FAIL == writeStatus and PP bit is disabled
    else if ( GSS_HSXPA_DISABLED_VIA_PP == reason )
        {
        TFLOGSTRING("TSY: CMmCustomMessHandler::WriteHSxPAStatusResp. KErrNotSupported");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_WRITEHSXPASTATUSRESP, "CMmCustomMessHandler::WriteHSxPAStatusResp, KErrNotSupported" );
        epocError = KErrNotSupported;
        }

    iMessageRouter->Complete( ECustomWriteHSxPAStatusIPC, epocError );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ReadHSxPAStatusReq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::ReadHSxPAStatusReq
        (
        TUint8 aTraId              // Transaction identification
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::ReadHSxPAStatusReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_READHSXPASTATUSREQ, "CMmCustomMessHandler::ReadHSxPAStatusReq" );

    TBuf8<KHSxPAMaxMessageSize> data;
    data.Append( KCustomPadding ); //Filler
    data.Append( KCustomPadding ); //Filler

    return iPhoNetSender->Send( PN_GSS,
                                aTraId,
                                GSS_HSXPA_USER_SETTING_READ_REQ,
                                data );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ReadHSxPAStatusResp
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::ReadHSxPAStatusResp
        (
        const TIsiReceiveC& aIsiMessage    // an ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::ReadHSxPAStatusResp");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_READHSXPASTATUSRESP, "CMmCustomMessHandler::ReadHSxPAStatusResp" );

    CMmDataPackage dataPackage;

    TUint8 state = aIsiMessage.Get8bit(
      ISI_HEADER_SIZE + GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_HSXPAUSERSETTING );
    TUint8 causeBit = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_CAUSE );

    RMmCustomAPI::THSxPAStatus  hsxpaStatus;
    TInt epocError( KErrNone );

    if ( GSS_HSXPA_DISABLED_VIA_PP != causeBit  )
        {

        if ( GSS_HSXPA_ALLOWED == state )
            {
            hsxpaStatus = RMmCustomAPI::EHSxPAEnabled;
            }
        else if ( GSS_HSXPA_DISABLED == state )
            {
            hsxpaStatus = RMmCustomAPI::EHSxPADisabled;
            }
        else //default -> general error occurs
            {
            epocError = KErrGeneral;
            }

        // complete with packed parameter
        dataPackage.PackData( &hsxpaStatus );
        }
    else  // PPbit is disabled -> not supported
        {
        epocError =  KErrNotSupported;
        }

    iMessageRouter->Complete( ECustomReadHSxPAStatusIPC, &dataPackage, epocError );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::HSxPASettingInd
// This method breaks the GSS_HSXPA_SETTING_IND ISI.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::HSxPASettingInd
        (
        const TIsiReceiveC& aIsiMessage    // an ISI message
        )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::HSxPASettingInd");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_HSXPASETTINGIND, "CMmCustomMessHandler::HSxPASettingInd" );

    CMmDataPackage dataPackage;
    RMmCustomAPI::THSxPAStatus hsxpaStatus;

    TUint8 currentState = aIsiMessage.Get8bit(
      ISI_HEADER_SIZE + GSS_HSXPA_USER_SETTING_IND_OFFSET_HSXPAUSERSETTING );

    if ( GSS_HSXPA_ALLOWED == currentState )
        {
        hsxpaStatus = RMmCustomAPI::EHSxPAEnabled;
        }
    else // ( GSS_HSXPA_DISABLED == currentState )
        {
        hsxpaStatus = RMmCustomAPI::EHSxPADisabled;
        }

    // complete with packed parameter
    dataPackage.PackData( &hsxpaStatus );
    iMessageRouter->Complete( ECustomNotifyHSxPAStatusIPC, &dataPackage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetNeighbourCellsReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::NetNeighbourCellsReq( const TUint8 aTraId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetNeighbourCellsReq");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETNEIGHBOURCELLSREQ, "CMmCustomMessHandler::NetNeighbourCellsReq" );

    // Add NET_NEIGHBOUR_CELL_INFO_TYPE
    // (data size is allways 1)
    TBuf8<1> data;
    data.Append( NET_ECID_INFORMATION );

    return iPhoNetSender->Send(
        PN_MODEM_NETWORK,
        aTraId,
        NET_NEIGHBOUR_CELLS_REQ,
        data );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::NetNeighbourCellsResp
// This method breaks the NET_NEIGHBOUR_CELLS_RESP message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::NetNeighbourCellsResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetNeighbourCellsResp");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_NETNEIGHBOURCELLSRESP, "CMmCustomMessHandler::NetNeighbourCellsResp" );

    TECIDInfo tempECIDInfo;
    TInt ret( KErrNone );

    // Get the success code.
    TUint8 successCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE +
        NET_NEIGHBOUR_CELLS_RESP_OFFSET_SUCCESSCODE ) );

    if ( NET_CAUSE_OK == successCode )
        {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetNeighbourCellsResp - NET_CAUSE_OK");
OstTrace0( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_NETNEIGHBOURCELLSRESP, "CMmCustomMessHandler::NetNeighbourCellsResp, NET_CAUSE_OK" );

        TUint sbOffset( 0 );

        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_NEIGHBOUR_CELLS_RESP,
            NET_ECID_GERAN_INFO,
            EIsiSubBlockTypeId8Len8,
            sbOffset ) )
            {
            // GSM cell info.
TFLOGSTRING("TSY: CMmCustomMessHandler::NetNeighbourCellsResp - NET_ECID_GERAN_INFO found");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_NETNEIGHBOURCELLSRESP, "CMmCustomMessHandler::NetNeighbourCellsResp - NET_ECID_GERAN_INFO found" );
            // Set mode
            tempECIDInfo.iCellInfo.iMode = RMmCustomAPI::TMmCellInfo::EGSM;

            // Set Current MCC.
            tempECIDInfo.iMCC = aIsiMessage.Get16bit(
                sbOffset + NET_ECID_GERAN_INFO_OFFSET_CURRENTMCC );

            // Set Current MNC.
            tempECIDInfo.iMNC = aIsiMessage.Get16bit(
                sbOffset + NET_ECID_GERAN_INFO_OFFSET_CURRENTMNC );

            // Set Current LAC.
            tempECIDInfo.iLAC = aIsiMessage.Get16bit(
                sbOffset + NET_ECID_GERAN_INFO_OFFSET_CURRENTLAC );

            // Set Current Cell ID.
            tempECIDInfo.iCID = aIsiMessage.Get16bit(
                sbOffset + NET_ECID_GERAN_INFO_OFFSET_CURRENTCELLID );

            // Set Current TA.
            TUint8 temp8bitUVal( aIsiMessage.Get8bit(
                sbOffset + NET_ECID_GERAN_INFO_OFFSET_CURRENTTA ) );

            if ( NET_ECID_TA_UNDEFINED != temp8bitUVal )
                {
                tempECIDInfo.iCellInfo.iGsmCellInfo.iTA = temp8bitUVal;
                }
            // No else.

            // Get Number of Neighbours included.
            TUint8 nmrCount( aIsiMessage.Get8bit(
                sbOffset + NET_ECID_GERAN_INFO_OFFSET_NNMR ) );

            // Set NMR list.
            for ( TUint8 i = 0; i < nmrCount; i++ )
                {
                // Set Carrier number.
                tempECIDInfo.iCellInfo.iGsmCellInfo.iNmr[ i ].iARFCN =
                    aIsiMessage.Get16bit(
                        sbOffset + NET_ECID_GERAN_INFO_OFFSET_GSMNMRLIST +
                        ( i * SIZE_NET_GSM_NMR_LIST_SEQ ) +
                        NET_GSM_NMR_LIST_SEQ_OFFSET_ARFCN );
                // Set BSIC.
                tempECIDInfo.iCellInfo.iGsmCellInfo.iNmr[ i ].iBSIC =
                    aIsiMessage.Get8bit(
                        sbOffset + NET_ECID_GERAN_INFO_OFFSET_GSMNMRLIST +
                        ( i * SIZE_NET_GSM_NMR_LIST_SEQ ) +
                        NET_GSM_NMR_LIST_SEQ_OFFSET_BSIC );
                // Set RX level.
                tempECIDInfo.iCellInfo.iGsmCellInfo.iNmr[ i ].iRxLEV =
                    aIsiMessage.Get8bit(
                        sbOffset + NET_ECID_GERAN_INFO_OFFSET_GSMNMRLIST +
                        ( i * SIZE_NET_GSM_NMR_LIST_SEQ ) +
                        NET_GSM_NMR_LIST_SEQ_OFFSET_RXLEVEL );
                }
            }
        else if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_NEIGHBOUR_CELLS_RESP,
            NET_ECID_UTRAN_FDD_INFO,
            EIsiSubBlockTypeId8Len16,
            sbOffset ) )
            {
            // WCDMA cell info.
TFLOGSTRING("TSY: CMmCustomMessHandler::NetNeighbourCellsResp - NET_ECID_UTRAN_FDD_INFO found");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_NETNEIGHBOURCELLSRESP, "CMmCustomMessHandler::NetNeighbourCellsResp - NET_ECID_UTRAN_FDD_INFO found" );
            // Set mode.
            tempECIDInfo.iCellInfo.iMode = RMmCustomAPI::TMmCellInfo::EWCDMA;

            // Set UCID.
            tempECIDInfo.iCID = aIsiMessage.Get32bit(
                sbOffset + NET_ECID_UTRAN_FDD_INFO_OFFSET_UCID );

            // Set current MCC.
            tempECIDInfo.iMCC = aIsiMessage.Get16bit(
                sbOffset + NET_ECID_UTRAN_FDD_INFO_OFFSET_CURRENTMCC );

            // Set current MNC.
            tempECIDInfo.iMNC = aIsiMessage.Get16bit(
                sbOffset + NET_ECID_UTRAN_FDD_INFO_OFFSET_CURRENTMNC );

            // Set current LAC (new value not received here).
            tempECIDInfo.iLAC = iECIDInfo.iLAC;

            // Set primary scrambling code.
            tempECIDInfo.iCellInfo.iWcdmaCellInfo.iPrimaryScrambilingCode =
                aIsiMessage.Get16bit(
                    sbOffset + NET_ECID_UTRAN_FDD_INFO_OFFSET_PRISCRAMBLING );

            // Set Frequency Info.
            // Set frequency Info (Uplink UARFCN and UARFCN-Nt not available)
            // Downlink UARFCN.
            tempECIDInfo.iCellInfo.iWcdmaCellInfo.iFrequencyInfo.iFddDL =
                aIsiMessage.Get16bit(
                    sbOffset + NET_ECID_UTRAN_FDD_INFO_OFFSET_UARFCNDL );

            // Get number of neighbours included.
            TUint8 nmrCount( aIsiMessage.Get8bit(
                sbOffset + NET_ECID_UTRAN_FDD_INFO_OFFSET_NNMR ) );

            // Set nmrListOffset to starting position of first NMR list.
            TUint nmrListOffset(
                sbOffset + NET_ECID_UTRAN_FDD_INFO_OFFSET_WCDMANMRLIST );

            // Handle NMR lists.
            for ( TUint8 i( 0 ); i < nmrCount; i++ )
                {
                // Set Frequency Info: Downlink UARFCN.
                tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                    iNwkMeasureReport[i].iFrequencyInfo.iFddDL =
                        aIsiMessage.Get16bit( nmrListOffset );

                // Uplink UARFCN and UARFCN-Nt not available, set to -1.
                tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                    iNwkMeasureReport[i].iFrequencyInfo.iFddUL = -1;
                tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                    iNwkMeasureReport[i].iFrequencyInfo.iTddNt = -1;

                // Set UTRA Carrier RSSI.
                tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                    iNwkMeasureReport[i].iCarrierRSSI =
                        aIsiMessage.Get8bit(
                            nmrListOffset +
                            NET_UTRAN_NEIGH_LIST_SEQ_OFFSET_UCRSSI );

                // Get number of cells included.
                TUint8 cellCount( aIsiMessage.Get8bit(
                    nmrListOffset + NET_UTRAN_NEIGH_LIST_SEQ_OFFSET_NCELL ) );

                // Set nmrListOffset to starting position of first 'detailed
                // cell information'.
                nmrListOffset += 4;

                // Handle detailed cell information.
                for ( TUint8 f( 0 ); f < cellCount; f++ )
                    {
                    // Set UCID.
                    TUint32 temp32bitUVal(
                        aIsiMessage.Get32bit( nmrListOffset ) );

                    if ( NET_ECID_UCID_UNDEFINED != temp32bitUVal )
                        {
                        tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                            iNwkMeasureReport[i].iCellMeasuredResult[f].iCID =
                                temp32bitUVal;
                        }
                    // No else.

                    // Set Primary CPICH.
                    tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                        iNwkMeasureReport[i].iCellMeasuredResult[f].
                            iFddInfo.iPrimaryCPICH = aIsiMessage.Get16bit(
                                nmrListOffset +
                                    NET_UTRAN_CELL_LIST_SEQ_OFFSET_PCPICH );

                    // Set CPICH Ec NO.
                    TUint8 temp8bitUVal( aIsiMessage.Get8bit(
                        nmrListOffset +
                            NET_UTRAN_CELL_LIST_SEQ_OFFSET_CPICHECNO ) );

                    if ( NET_ECID_CPICH_ECNO_UNDEFINED != temp8bitUVal )
                        {
                        tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                            iNwkMeasureReport[i].iCellMeasuredResult[f].
                                iFddInfo.iCpichEcN0 = temp8bitUVal;
                        }
                    // No else.

                    // Set CPICH RSCP.
                    tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                        iNwkMeasureReport[i].iCellMeasuredResult[f].
                            iFddInfo.iCpichRscp = aIsiMessage.Get8bit(
                                nmrListOffset +
                                    NET_UTRAN_CELL_LIST_SEQ_OFFSET_CPICHRSCP );

                    // Set Pathloss.
                    temp8bitUVal = aIsiMessage.Get8bit(
                            nmrListOffset +
                                NET_UTRAN_CELL_LIST_SEQ_OFFSET_PATHLOSS );

                    if ( NET_ECID_PATHLOSS_UNDEFINED != temp8bitUVal )
                        {
                        tempECIDInfo.iCellInfo.iWcdmaCellInfo.
                            iNwkMeasureReport[i].iCellMeasuredResult[f].
                                iFddInfo.iPathloss = temp8bitUVal;
                        }
                    // No else.

                    // Increase nmrListOffset to starting point of the next
                    // element of 'detailed cell information'.
                    nmrListOffset += SIZE_NET_UTRAN_CELL_LIST_SEQ;
                    }
                }
            }
        else
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::NetNeighbourCellsResp - No ECID data found");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_NETNEIGHBOURCELLSRESP, "CMmCustomMessHandler::NetNeighbourCellsResp - No ECID data found" );
            ret = KErrNotFound;
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::NetNeighbourCellsResp - NET Server Error, Success Code: %d", successCode);
OstTraceExt1( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_NETNEIGHBOURCELLSRESP, "CMmCustomMessHandler::NetNeighbourCellsResp;NET Server Error, Success Code=%hhu", successCode );

        // Map cause to symbian error value.
        ret = CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_NETWORK,
            KTsyNetCauseCommon,
            successCode );
        }

    // Complete get request.
    if ( iECIDInfoRequested )
        {
        iECIDInfoRequested = EFalse;

        // Complete with packed parameter.
        CMmDataPackage dataPackage;
        dataPackage.PackData( &tempECIDInfo.iCellInfo );

        iMessageRouter->Complete(
            ECustomGetCellInfoIPC,
            &dataPackage,
            ret );
        }
    // No else.

    // If info received, save and complete change notification.
    if ( KErrNone == ret )
        {
        if ( iECIDInfo.iCellInfo.iMode != tempECIDInfo.iCellInfo.iMode ||
            iECIDInfo.iMCC != tempECIDInfo.iMCC ||
            iECIDInfo.iMNC != tempECIDInfo.iMNC ||
            iECIDInfo.iCID != tempECIDInfo.iCID ||
            iECIDInfo.iLAC != tempECIDInfo.iLAC )
            {
            // Complete with packed parameter.
            CMmDataPackage dataPackage;
            dataPackage.PackData( &tempECIDInfo.iCellInfo );

            iMessageRouter->Complete(
                ECustomNotifyCellInfoChangeIPC,
                &dataPackage,
                KErrNone );
            }
        // No else.

        // Save ECID information.
        iECIDInfo = tempECIDInfo;
        }
    // No else.
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CheckECIDInfo
// This method breaks received NET_MODEM_REG_STATUS_IND or
// NET_MODEM_REG_STATUS_GET_RESP message and checks if enhanced cell
// information is chaged ( => needs to be requested/updated ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CheckECIDInfo(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::CheckECIDInfo");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_CHECKECIDINFO, "CMmCustomMessHandler::CheckECIDInfo" );

    TUint8 msgId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    if ( NET_MODEM_REG_STATUS_IND == msgId ||
        NET_MODEM_REG_STATUS_GET_RESP == msgId )
        {
        // Check if cell information needs to be updated.
        TBool isNetRetStatusInd( NET_MODEM_REG_STATUS_IND == msgId );

TFLOGSTRING("TSY:CMmCustomMessHandler::CheckECIDInfo - Find NET_MODEM_CURRENT_CELL_INFO SubBlock");
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_CHECKECIDINFO, "CMmCustomMessHandler::CheckECIDInfo - Find NET_MODEM_CURRENT_CELL_INFO SubBlock" );
        // Get NET_MODEM_CURRENT_CELL_INFO sub block
        // (should be always used when exists (coming when in 3G/RRC
        // connected mode), otherwise NET_MODEM_GSM_REG_INFO is used).

        TUint sbStartOffSet( 0 );
        TInt retValue( KErrNone );

        if ( isNetRetStatusInd )
            {
            retValue = aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_IND,
                NET_MODEM_CURRENT_CELL_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet );
            }
        else
            {
            retValue = aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_GET_RESP,
                NET_MODEM_CURRENT_CELL_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet );
            }

        // Get NET_MODEM_GSM_REG_INFO sub block.
        if ( KErrNone != retValue )
            {
TFLOGSTRING("TSY:CMmCustomMessHandler::CheckECIDInfo - Find NET_MODEM_GSM_REG_INFO SubBlock");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_CHECKECIDINFO, "CMmCustomMessHandler::CheckECIDInfo - Find NET_MODEM_GSM_REG_INFO SubBlock" );

            sbStartOffSet = 0;

            if ( isNetRetStatusInd )
                {
                retValue = aIsiMessage.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_IND,
                    NET_MODEM_GSM_REG_INFO,
                    EIsiSubBlockTypeId8Len8,
                    sbStartOffSet );
                }
            else
                {
                retValue = aIsiMessage.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_GET_RESP,
                    NET_MODEM_GSM_REG_INFO,
                    EIsiSubBlockTypeId8Len8,
                    sbStartOffSet );
                }
            }
        // No else.

        // If subblock found.
        if ( KErrNone == retValue )
            {
TFLOGSTRING("TSY:CMmCustomMessHandler::CheckECIDInfo - SubBlock Found");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_CHECKECIDINFO, "CMmCustomMessHandler::CheckECIDInfo, SubBlock Found" );
            TUint tempMCC( 0 ); // Mobile Country Code.
            TUint tempMNC( 0 ); // Mobile Network Code.
            TUint tempCID( 0 ); // Cell identity.
            TUint tempLAC( 0 ); // Location area code.

            // Get the Location Area Code.
            tempLAC = aIsiMessage.Get16bit(
                sbStartOffSet +
                NET_MODEM_CURRENT_CELL_INFO_OFFSET_CURRENTLAC );
            // Get the Cell Id.
            tempCID = aIsiMessage.Get32bit(
                sbStartOffSet +
                NET_MODEM_CURRENT_CELL_INFO_OFFSET_CURRENTCELLID );

            // Get the operator code. Size is 3 bytes.
            TPtrC8 operatorCode( aIsiMessage.GetData(
                sbStartOffSet +
                NET_MODEM_CURRENT_CELL_INFO_OFFSET_OPERATORCODE,
                3 ) );

            // Map the operator code to symbian values (MCC and MNC).
            RMobilePhone::TMobilePhoneNetworkIdentity networkId;
            RMobilePhone::TMobilePhoneNetworkCountryCode countryCode;
            iNetMessHandler->MapOperatorAndCountryCode(
                                 operatorCode,
                                 networkId,
                                 countryCode );
            // Convert descriptor contained number to integer.
            CMmStaticUtility::GetIntFromDescriptor(
                tempMCC,
                countryCode );
            CMmStaticUtility::GetIntFromDescriptor(
                tempMNC,
                networkId );

            // If changed, request new ECID information.
            if ( iECIDInfo.iMCC != tempMCC ||
                iECIDInfo.iMNC != tempMNC ||
                iECIDInfo.iCID != tempCID ||
                iECIDInfo.iLAC != tempLAC )
                {
                TUint8 transId( KCustomTransId );
                NetNeighbourCellsReq( transId );
                // Save current LAC.
                // (not received in NetNeighbourCellsResp when in 3G).
                iECIDInfo.iLAC = tempLAC;
                }
            // No else.
            }
        // No else.
        }
    // No else.
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::HandleError
// Handles CMmCustomMessHandler's errors that comes
// via PhoNetReceiver RunError method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::HandleError
    (
      const TIsiReceiveC& aIsiMessage, TInt aError
      )
    {
    TFLOGSTRING2( "TSY: CMmCustomMessHandler::HandleError - Error: %d", aError );
OstTrace1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_HANDLEERROR, "CMmCustomMessHandler::HandleError;aError=%d", aError );

    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    TUint8 serviceType( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_SUBMESSAGEID ) );

    switch( resource )
        {
        default:
            {
            TFLOGSTRING("TSY: CMmCustomMessHandler::HandleError, switch resource - default.\n" );
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_HANDLEERROR, "CMmCustomMessHandler::HandleError, switch resource - default" );
            // Nothing to do here.
            break;
            }
        } // end switch ( resource )
    }

// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::GssCsServiceSetBandReq
// Creates and sends via phonet a GSS_CS_SERVICE_REQ including
// selected band mode (ENetworkBandAny, ENetworkBandUmts850, ENetworkBandUmts2100).
// Affects only in Umts or Dual mode.
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::GssCsServiceSetBandReq( TUint8 aTransId, // transaction Id
        RMmCustomAPI::TNetworkModeCaps aNetworkModeCaps, // network mode caps
        RMmCustomAPI::TBandSelection aBandSelection ) // band selection
    {
    TFLOGSTRING2( "TSY: CMmCustomMessHandler::GssCsServiceSetBandReq - aTransId: %d", aTransId );
OstTraceExt1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_GSSCSSERVICESETBANDREQ, "CMmCustomMessHandler::GssCsServiceSetBandReq;aTransId=%hhu", aTransId );

    if ( RMmCustomAPI::KCapsNetworkModeUmts == aNetworkModeCaps ||
         RMmCustomAPI::KCapsNetworkModeDual == aNetworkModeCaps )
        {
        TFLOGSTRING3( "TSY: CMmCustomMessHandler::GssCsServiceSetBandReq - NWModeCaps: %d - Band: %d",
            aNetworkModeCaps, aBandSelection );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_GSSCSSERVICESETBANDREQ, "CMmCustomMessHandler::GssCsServiceSetBandReq; - NWModeCaps=%d - Band=%d", aNetworkModeCaps, aBandSelection );

        // create message data buffer
        TBuf8< 20 > messageData;
        messageData.Append( GSS_SELECTED_BANDS_WRITE ); // append service type
        messageData.Append( 1 ); // number of subblocks

        // subblock
        messageData.Append( GSS_SELECTED_UMTS_BAND_INFO ); // append band info
        messageData.Append( 0x08 ); // subblock length
        messageData.Append( KCustomPadding );
        messageData.Append( KCustomPadding );

        if ( RMmCustomAPI::ENetworkBandUmts850 == aBandSelection )
            {
            messageData.Append( TUint8( GSS_UMTS_BAND_V_MASK >> 24 ) );
            messageData.Append( TUint8( GSS_UMTS_BAND_V_MASK >> 16 ) );
            messageData.Append( TUint8( GSS_UMTS_BAND_V_MASK >> 8 ) );
            messageData.Append( TUint8( GSS_UMTS_BAND_V_MASK ) );
            }
        else if ( RMmCustomAPI::ENetworkBandUmts2100 == aBandSelection )
            {
            messageData.Append( TUint8( GSS_UMTS_BAND_I_MASK >> 24 ) );
            messageData.Append( TUint8( GSS_UMTS_BAND_I_MASK >> 16 ) );
            messageData.Append( TUint8( GSS_UMTS_BAND_I_MASK >> 8 ) );
            messageData.Append( TUint8( GSS_UMTS_BAND_I_MASK ) );
            }
        else // ENetworkBandAny
            {
            messageData.Append( TUint8( GSS_UMTS_ALL_BANDS >> 24 ) );
            messageData.Append( TUint8( GSS_UMTS_ALL_BANDS >> 16 ) );
            messageData.Append( TUint8( GSS_UMTS_ALL_BANDS >> 8 ) );
            messageData.Append( TUint8( GSS_UMTS_ALL_BANDS ) );
            }

        return iPhoNetSender->Send( PN_GSS, aTransId, GSS_CS_SERVICE_REQ, messageData );
        }

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::GssCsServiceGetBandReq
// Creates request to get current band.
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::GssCsServiceGetBandReq( TUint8 aTransId ) //transaction Id
    {
    TFLOGSTRING2( "TSY: CMmCustomMessHandler::GssCsServiceGetBandReq - aTransId: %d", aTransId );
OstTraceExt1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_GSSCSSERVICEGETBANDREQ, "CMmCustomMessHandler::GssCsServiceGetBandReq;aTransId=%hhu", aTransId );

    // create message data buffer
    TBuf8< 4 > messageData;
    messageData.Append( GSS_SELECTED_BANDS_READ ); // append service type
    messageData.Append( 0 ); // No subblocks

    return iPhoNetSender->Send( PN_GSS, aTransId, GSS_CS_SERVICE_REQ, messageData );
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::CompleteGetBandSelection
// Completes and breaks Get band selection request via PhoNetReceiver method.
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::CompleteGetBandSelection( const TIsiReceiveC& aIsiMessage )
    {
    TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection" );

    RMmCustomAPI::TNetworkModeCaps nwModeCaps; // network mode caps
    RMmCustomAPI::TBandSelection bandSelection; // band selection
    bandSelection = RMmCustomAPI::ENetworkBandAny; // set "any" as a default

    TUint8 nrOfSubblocks ( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        GSS_CS_SERVICE_RESP_OFFSET_NBROFSUBBLOCKS ) );

    // first subblocks start from 'GSS_CS_SERVICE_RESP_OFFSET_NBROFSUBBLOCKS' + 1
    TUint8 sbOffset ( ISI_HEADER_SIZE +
        GSS_CS_SERVICE_RESP_OFFSET_NBROFSUBBLOCKS + 1 );

    TUint8 firstSubblock ( aIsiMessage.Get8bit( sbOffset ) );

    TFLOGSTRING2("TSY: CMmCustomMessHandler::CompleteGetBandSelection - ECustomGetBandSelectionIPC. NrOfSubblocks:%d", nrOfSubblocks);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection;nrOfSubblocks=%hhu", nrOfSubblocks );

    // In 2G only products only sub block GSS_SELECTED_GSM_BANDS_INFO is returned.
    if ( 1 == nrOfSubblocks && firstSubblock == GSS_SELECTED_GSM_BAND_INFO )
        {
        nwModeCaps = RMmCustomAPI::KCapsNetworkModeGsm;
        }
    else // 1 < nrOfSubblocks || GSS_SELECTED_UMTS_BAND_INFO
        {
        //Band info offsets for reading
        TUint8 gsmBandInfoOffset = 0;
        TUint8 umtsBandInfoOffset = 0;

        // first SB is GSS_SELECTED_GSM_BAND_INFO
        if ( GSS_SELECTED_GSM_BAND_INFO == firstSubblock )
            {
            TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection - 1st SB = GSM BAND INFO");
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection - 1st SB = GSM BAND INFO" );

            // calculate offsets
            gsmBandInfoOffset = sbOffset +
                GSS_SELECTED_GSM_BAND_INFO_OFFSET_SELECTEDGSMBANDS;

            // if there are more than one subblock read offset for second one
            if ( 1 < nrOfSubblocks )
                {
                umtsBandInfoOffset = sbOffset +
                    SIZE_GSS_SELECTED_GSM_BAND_INFO +
                    GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SELECTEDUMTSBANDS;
                }
            }
        else if ( GSS_SELECTED_UMTS_BAND_INFO == firstSubblock )
            {
            TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection - 1st SB = UMTS BAND INFO");
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection - 1st SB = UMTS BAND INFO" );

            // calculate offsets
            umtsBandInfoOffset = sbOffset +
                GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SELECTEDUMTSBANDS;

            // if there are more than one subblock read offset for second one
            if ( 1 < nrOfSubblocks )
                {
                gsmBandInfoOffset = sbOffset +
                    SIZE_GSS_SELECTED_UMTS_BAND_INFO +
                    GSS_SELECTED_GSM_BAND_INFO_OFFSET_SELECTEDGSMBANDS;
                }
            }

        //If phone is forced to GSM, value in GSS_SELECTED_UMTS_BAND_INFO shall be GSS_UMTS_NO_BANDS
        if ( GSS_UMTS_NO_BANDS == aIsiMessage.Get32bit( umtsBandInfoOffset ) ) // forced GSM
            {
            TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection - umtsBandInfo==GSS_UMTS_NO_BANDS => Forced GSM");
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection - umtsBandInfo==GSS_UMTS_NO_BANDS => Forced GSM" );
            nwModeCaps = RMmCustomAPI::KCapsNetworkModeGsm;
            }
        else
            {
            //If phone is forced to GSM, value in GSS_SELECTED_UMTS_BAND_INFO shall be GSS_UMTS_NO_BANDS.
            if ( 0 < gsmBandInfoOffset && GSS_GSM_NO_BANDS == aIsiMessage.Get8bit( gsmBandInfoOffset ) ) // forced UMTS
                {
                TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection - gsmBandInfo==GSS_GSM_NO_BANDS => Forced UMTS");
OstTrace0( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection - gsmBandInfo==GSS_GSM_NO_BANDS => Forced UMTS" );
                nwModeCaps = RMmCustomAPI::KCapsNetworkModeUmts;
                }
            else // dual mode
                {
                TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection - Dual mode");
OstTrace0( TRACE_NORMAL, DUP6_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection - Dual mode" );
                nwModeCaps = RMmCustomAPI::KCapsNetworkModeDual;
                }

            // Set network band
            if ( GSS_UMTS_BAND_V_MASK == aIsiMessage.Get32bit( umtsBandInfoOffset ) )
                {
                TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection - UMTS850");
OstTrace0( TRACE_NORMAL, DUP7_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection - UMTS850" );
                bandSelection = RMmCustomAPI::ENetworkBandUmts850;
                }
            else if( GSS_UMTS_BAND_I_MASK == aIsiMessage.Get32bit( umtsBandInfoOffset ) )
                {
                TFLOGSTRING("TSY: CMmCustomMessHandler::CompleteGetBandSelection - UMTS2100");
OstTrace0( TRACE_NORMAL, DUP8_CMMCUSTOMMESSHANDLER_COMPLETEGETBANDSELECTION, "CMmCustomMessHandler::CompleteGetBandSelection - UMTS2100" );
                bandSelection = RMmCustomAPI::ENetworkBandUmts2100;
                }
            }
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &bandSelection, &nwModeCaps );
    return iMessageRouter->Complete( ECustomGetBandSelectionIPC, &dataPackage, KErrNone );
    }

#endif // NCP_COMMON_S60_VERSION_SUPPORT

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ReadLifeTimerFromPermanentMemory
// Constructs PERM_PM_RECORD_READ_REQ ISI message from input parameters and send
// it through phonet.
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::ReadLifeTimerFromPermanentMemory
        (
        const TUint8 //aTransId
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ReadLifeTimerFromPermanentMemory.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_READLIFETIMERFROMPERMANENTMEMORY, "CMmCustomMessHandler::ReadLifeTimerFromPermanentMemory" );

    // Group ID 2 + Index 2 + Filler 2 + Offset 4 + Data amount to be read 4 = 14
    TBuf8<14> data;

    // Append Group ID
    data.Append( static_cast<TUint8>( KPmmGroupNokiaTsy >> 8 ) );
    data.Append( static_cast<TUint8>( KPmmGroupNokiaTsy ) );

    // Append Call Life Timer Index
    data.Append( static_cast<TUint8>( KCallLifeTimerPmmIndexValue >> 8 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerPmmIndexValue ) );

    // Append Filler
    data.AppendFill( KCustomPadding, KTwo );

    // Append Offset
    data.AppendFill( KCustomPadding, KFour );

    // Append Data amount to be read
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount >> 24 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount >> 16 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount >> 8 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount ) );

    /* To be done in CPS
    return iPhoNetSender->Send( PN_PERMANENT_DATA,
        aTransId,
        PERM_PM_RECORD_READ_REQ,
        data );*/
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse
// Breaks  PERM_PM_RECORD_READ_RESP message.
// -----------------------------------------------------------------------------
//
/* To be done in CPS
void CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse
        (
        const TIsiReceiveC& aIsiMessage    // an ISI message
        )
    {
TFLOGSTRING("TSY: CCMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_LIFETIMERFROMPERMANENTMEMORYREADRESPONSE, "CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse" );

    TInt ret( KErrNone );

    TUint8 pmmStatus = aIsiMessage.Get8bit(
               ISI_HEADER_SIZE +
               PERM_PM_RECORD_READ_RESP_OFFSET_PMMSTATUS );

    if ( PMM_OK == pmmStatus )
        {
        iPMMReadRetryCounter = 0; // NUL trial counter
        TUint sbStartOffSet( 0 );
        ret = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_PERM_PM_RECORD_READ_RESP,
            PERM_SB_PM_DATA,
            EIsiSubBlockTypeId8Len8, sbStartOffSet );

        if ( KErrNone == ret )
            {
            TUint32 dataSize( aIsiMessage.Get32bit( sbStartOffSet +
                        PERM_SB_PM_DATA_OFFSET_SIZE ) );
            // Timer value size in message is 4 bytes. If some else length
            // has been returned error code KErrGeneral is set and
            // assert is done.
            if ( KCallLifeTimerDataAmount == dataSize )
                {
                iCallLifeTimer = aIsiMessage.Get32bit( sbStartOffSet +
                                     PERM_SB_PM_DATA_OFFSET_DATA );
TFLOGSTRING2( "TSY: CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - iCallLifeTimer: %d", iCallLifeTimer );
OstTrace1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_LIFETIMERFROMPERMANENTMEMORYREADRESPONSE, "CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse;iCallLifeTimer=%d", iCallLifeTimer );
                }
            else
                {
TFLOGSTRING2( "TSY: CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - Incorrect data size: %d", dataSize );
OstTrace1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_LIFETIMERFROMPERMANENTMEMORYREADRESPONSE, "CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - Incorrect data size=%u", dataSize );
                TF_ASSERT( KCallLifeTimerDataAmount != dataSize );
                ret = KErrGeneral;
                }
            }
        }
    else
        {
        if ( PMM_NOT_READY == pmmStatus )
            {
            // Send request again max 3 times
TFLOGSTRING("TSY: CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - PMM not ready. Send request again\n" );
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_LIFETIMERFROMPERMANENTMEMORYREADRESPONSE, "CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - PMM not ready. Send request again" );
            iPMMReadRetryCounter++;
            if ( KMaxPMMReadRequests > iPMMReadRetryCounter )
                {
                ReadLifeTimerFromPermanentMemory ( KCustomTransId );
                }
            }
        else if ( PMM_RECORD_NOT_FOUND == pmmStatus )
            {
            // The first boot of the phone.
TFLOGSTRING("TSY: CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - The first boot. Set iCallLifeTimer 0\n" );
            OstTrace0( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_LIFETIMERFROMPERMANENTMEMORYREADRESPONSE, "CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - The first boot. Set iCallLifeTimer 0" );
            iCallLifeTimer = 0;
            }
        else
            {
TFLOGSTRING2( "TSY: CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - PMM Status NOT OK: %d", pmmStatus );
OstTraceExt1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_LIFETIMERFROMPERMANENTMEMORYREADRESPONSE, "CMmCustomMessHandler::LifeTimerFromPermanentMemoryReadResponse - PMM Status NOT OK=%hhu", pmmStatus );
            }
        }
    }*/

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::WriteLifeTimerToPermanentMemory
// Constructs PERM_PM_RECORD_WRITE_REQ ISI message from input parameters and send
// it through phonet.
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::WriteLifeTimerToPermanentMemory
        (
        const TUint8 /*aTransId*/,
        TTimeIntervalSeconds aTime
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::WriteLifeTimerToPermanentMemory.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_WRITELIFETIMERTOPERMANENTMEMORY, "CMmCustomMessHandler::WriteLifeTimerToPermanentMemory" );

    // Group ID 2 + Index 2 + Filler 2 + Offset 4 + Data amount to be read 4 = 14
    TBuf8<14> data;

    // Append Group ID
    data.Append( static_cast<TUint8>( KPmmGroupNokiaTsy >> 8 ) );
    data.Append( static_cast<TUint8>( KPmmGroupNokiaTsy ) );

    // Append Call Life Timer Index
    data.Append( static_cast<TUint8>( KCallLifeTimerPmmIndexValue >> 8 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerPmmIndexValue ) );

    // Append Filler
    data.AppendFill( KCustomPadding, KTwo );

    // Append Data amount to be written
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount >> 24 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount >> 16 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount >> 8 ) );
    data.Append( static_cast<TUint8>( KCallLifeTimerDataAmount ) );

    if ( KCallLifeTimerNotSet == iCallLifeTimer )
        {
TFLOGSTRING( "TSY: CMmCustomMessHandler::WriteLifeTimerToPermanentMemory - Call Life Timer is not set from PMM ret KErrNotReady\n" );
TFLOGSTRING("TSY: CMmCustomMessHandler::WriteLifeTimerToPermanentMemory - PERM_PM_RECORD_WRITE_REQ is NOT send\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_WRITELIFETIMERTOPERMANENTMEMORY, "CMmCustomMessHandler::WriteLifeTimerToPermanentMemory - Call Life Timer is not set from PMM ret KErrNotReady" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_WRITELIFETIMERTOPERMANENTMEMORY, "CMmCustomMessHandler::WriteLifeTimerToPermanentMemory - PERM_PM_RECORD_WRITE_REQ is NOT send" );

        // Return request with KErrNotReady
        return ( KErrNotReady );
        }
    else
        {
        // Update Life Timer Value
        iCallLifeTimer += aTime.Int();

        // Append Data
        data.Append( static_cast<TUint8>( iCallLifeTimer >> 24 ) );
        data.Append( static_cast<TUint8>( iCallLifeTimer >> 16 ) );
        data.Append( static_cast<TUint8>( iCallLifeTimer >> 8 ) );
        data.Append( static_cast<TUint8>( iCallLifeTimer ) );

        /* To be done in CPS
        return iPhoNetSender->Send( PN_PERMANENT_DATA,
            aTransId,
            PERM_PM_RECORD_WRITE_REQ,
            data );*/
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse
// Breaks  PERM_PM_RECORD_WRITE_RESP message.
// -----------------------------------------------------------------------------
//
/* To be done in CPS
void CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse
        (
        const TIsiReceiveC& aIsiMessage    // an ISI message
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_LIFETIMERTOPERMANENTMEMORYWRITERESPONSE, "CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse" );

    TInt ret( KErrNone );

    TUint8 pmmStatus = aIsiMessage.Get8bit(
               ISI_HEADER_SIZE +
               PERM_PM_RECORD_WRITE_RESP_OFFSET_PMMSTATUS );
TFLOGSTRING2( "TSY: CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse - pmmStatus: %d", pmmStatus );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_LIFETIMERTOPERMANENTMEMORYWRITERESPONSE, "CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse;pmmStatus=%hhu", pmmStatus );

    if ( PMM_OK != pmmStatus )
        {
        ret = KErrNotReady;
        TF_ASSERT( PMM_OK != pmmStatus );
TFLOGSTRING2( "TSY: CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse - Call Life Timer is not updated / error from PMM ret: %d", ret );
OstTrace1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_LIFETIMERTOPERMANENTMEMORYWRITERESPONSE, "CMmCustomMessHandler::LifeTimerToPermanentMemoryWriteResponse - Call Life Timer is not updated / error from PMM ret=%d", ret );
        }

    return iMessageRouter->Complete( EMmTsyUpdateLifeTimeIPC, ret );
    }*/

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::GetTotalLifeTimerValue
// Breaks  PERM_PM_RECORD_WRITE_RESP message.
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::GetTotalLifeTimerValue
        (
        )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::GetTotalLifeTimerValue.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_GETTOTALLIFETIMERVALUE, "CMmCustomMessHandler::GetTotalLifeTimerValue" );

    TInt ret( KErrNone );
    RMmCustomAPI::TLifeTimeData timeInfo;

    timeInfo.iHours = 0;
    timeInfo.iMinutes = 0;

    if ( KCallLifeTimerNotSet == iCallLifeTimer )
        {
        ret = KErrNotReady;
TFLOGSTRING( "TSY: CMmCustomMessHandler::GetTotalLifeTimerValue - PMM has not been able to be read ret: KErrNotReady\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_GETTOTALLIFETIMERVALUE, "CMmCustomMessHandler::GetTotalLifeTimerValue - PMM has not been able to be read ret: KErrNotReady" );
        }
    else
        {
        //set the data into timeInfo class, one hour is 3600 seconds.
        timeInfo.iHours = iCallLifeTimer / KSecsInHour ;
        timeInfo.iMinutes =
            static_cast<TUint8>( ( iCallLifeTimer % KSecsInHour ) /
                                   KMinsInHour );
        //update the caps
        timeInfo.iCaps |=
            RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsLifeTime;
TFLOGSTRING2( "TSY: TSY: CMmCustomMessHandler::GetTotalLifeTimerValue - iCallLifeTimer: %d", iCallLifeTimer );
TFLOGSTRING2( "TSY: TSY: CMmCustomMessHandler::GetTotalLifeTimerValue - timeInfo.iHours: %d", timeInfo.iHours );
TFLOGSTRING2( "TSY: TSY: CMmCustomMessHandler::GetTotalLifeTimerValue - timeInfo.iMinutes: %d", timeInfo.iMinutes );
OstTrace1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_GETTOTALLIFETIMERVALUE, "CMmCustomMessHandler::GetTotalLifeTimerValue;iCallLifeTimer=%d", iCallLifeTimer );
OstTrace1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_GETTOTALLIFETIMERVALUE, "CMmCustomMessHandler::GetTotalLifeTimerValue;timeInfo.iHours=%u", timeInfo.iHours );
OstTraceExt1( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_GETTOTALLIFETIMERVALUE, "CMmCustomMessHandler::GetTotalLifeTimerValue;timeInfo.iMinutes=%hhu", timeInfo.iMinutes );
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &timeInfo );
    iMessageRouter->Complete( ECustomGetLifeTimeIPC, &dataPackage, ret );

    return( ret );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimPowerOffReq
// Disconnects UICC server from smartcard and activates UICC SAP APDU interface
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccSimPowerOffReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimPowerOffReq.\n");
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMPOWEROFFREQ, "CMmCustomMessHandler::UiccSimPowerOffReq" );

    iSapApduIfState = EUiccSapIfStatus1;
    // Set parameters for UICC_CONNECTION_REQ message
    TUiccParamsBase params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdSimPowerOff;
    params.serviceType = UICC_DISCONNECT;

    return iMmUiccMessHandler->CreateUiccConnectorReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimPowerOffResp
//
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccSimPowerOffResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimPowerOffResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMPOWEROFFRESP, "CMmCustomMessHandler::UiccSimPowerOffResp" );

    TInt err ( KErrNone );

    if( aStatus == UICC_STATUS_OK )
        {
        switch( iSapApduIfState )
            {
            case EUiccSapIfStatus1:
                {
                // Set parameters for UICC_APDU_REQ message
                TUiccParamsApduReq params;
                params.messHandlerPtr = static_cast<MUiccOperationBase*>(
                    this );
                params.trId = ETrIdSimPowerOff;
                params.serviceType = UICC_APDU_CONTROL;
                params.action = UICC_CONTROL_CARD_ACTIVATE;

                err = iMmUiccMessHandler->CreateUiccApduReq( params );

                iSapApduIfState = EUiccSapIfStatus2;
                break;
                }

            case EUiccSapIfStatus2:
                {
                // No handling for UICC_APDU_RESP,
                // wait for UICC_APDU_RESET_IND
                iSapApduIfState = EUiccSapIfStatus3;
                break;
                }

            case EUiccSapIfStatus3:
                {
                // UICC_APDU_RESET_IND, UICC_READY, SAP IF active
                iMessageRouter->Complete( ECustomPowerSimOffIPC, err );
                break;
                }
            }
        }
    else
        {
        if( aStatus == UICC_STATUS_SHUTTING_DOWN )
            {
            err = CMmStaticUtility::EpocErrorCode( KErrNotReady, KErrNotFound );
            }
        else
            {
            // UICC_STATUS_FAIL
            err = CMmStaticUtility::EpocErrorCode( KErrGeneral, KErrNotFound );
            }
        iMessageRouter->Complete( ECustomPowerSimOffIPC, err );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimWarmResetReq
//
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccSimWarmResetReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimWarmResetReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMWARMRESETREQ, "CMmCustomMessHandler::UiccSimWarmResetReq" );

    TUiccParamsApduReq params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdSimWarmReset;
    params.serviceType = UICC_APDU_CONTROL;
    params.action = UICC_CONTROL_WARM_RESET;

    return iMmUiccMessHandler->CreateUiccApduReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimWarmResetResp
//
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccSimWarmResetResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimWarmResetResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMWARMRESETRESP, "CMmCustomMessHandler::UiccSimWarmResetResp" );

    TInt err ( KErrNone );

    if( aStatus != UICC_STATUS_OK )
        {
        if( aStatus == UICC_STATUS_SHUTTING_DOWN )
            {
            err = CMmStaticUtility::EpocErrorCode( KErrNotReady, KErrNotFound );
            }
        else
            {
            // UICC_STATUS_FAIL
            err = CMmStaticUtility::EpocErrorCode( KErrGeneral, KErrNotFound );
            }
        }
    iMessageRouter->Complete( ECustomSimWarmResetIPC, err );

    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimGetAtrReq
//
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccSimGetAtrReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimGetAtrReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMGETATRREQ, "CMmCustomMessHandler::UiccSimGetAtrReq" );

    TUiccParamsApduReq params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdSimGetATR;
    params.serviceType = UICC_APDU_ATR_GET;

    return iMmUiccMessHandler->CreateUiccApduReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimGetAtrResp
//
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccSimGetAtrResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimGetAtrResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMGETATRRESP, "CMmCustomMessHandler::UiccSimGetAtrResp" );

    TInt err( KErrNone );
    const TDesC8* dataPtr = &aFileData;
    CMmDataPackage dataPackage;
    dataPackage.PackData( &dataPtr );

    if( aStatus != UICC_STATUS_OK )
        {
        if( aStatus == UICC_STATUS_SHUTTING_DOWN )
            {
            err = CMmStaticUtility::EpocErrorCode( KErrNotReady, KErrNotFound );
            }
        else
            {
            // UICC_STATUS_FAIL
            err = CMmStaticUtility::EpocErrorCode( KErrGeneral, KErrNotFound );
            }
        }

    iMessageRouter->Complete( ECustomGetATRIPC, &dataPackage, err );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimPowerOnReq
// Connects UICC server to smartcard and deactivates UICC SAP APDU interface
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccSimPowerOnReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimPowerOnReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMPOWERONREQ, "CMmCustomMessHandler::UiccSimPowerOnReq" );

    //Set APDU interface state
    iSapApduIfState = EUiccSapIfStatus1;

    // Set parameters for UICC_CONNECTION_REQ message
    TUiccParamsBase params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdSimPowerOn;
    params.serviceType = UICC_CONNECT;

    return iMmUiccMessHandler->CreateUiccConnectorReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimPowerOnResp
//
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccSimPowerOnResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimPowerOnResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMPOWERONRESP, "CMmCustomMessHandler::UiccSimPowerOnResp" );

    TInt err( KErrNone );

    if( aStatus == UICC_STATUS_OK )
        {
        switch( iSapApduIfState )
            {
            case EUiccSapIfStatus1:
                {
                // No handling for UICC_CONNECTOR_RESP
                // wait for UICC_APPLICATION_IND
                iSapApduIfState = EUiccSapIfStatus2;
                break;
                }

            case EUiccSapIfStatus2:
                {
                // UICC_APPLICATION_IND (UICC_APPL_ACTIVATED) received
                // SAP IF deactivated
                iMessageRouter->Complete( ECustomPowerSimOnIPC, err );
                break;
                }
            }
        }
    else
        {
        if( aStatus == UICC_STATUS_SHUTTING_DOWN )
            {
            err = CMmStaticUtility::EpocErrorCode( KErrNotReady, KErrNotFound );
            }
        else
            {
            // UICC_STATUS_FAIL
            err = CMmStaticUtility::EpocErrorCode( KErrGeneral, KErrNotFound );
            }
        iMessageRouter->Complete( ECustomPowerSimOnIPC, err );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSimSendAPDUReq
//
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccSendAPDUReq(
    const RMmCustomAPI::TApduParameters& aApduParameters,
    TInt aTraId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSimSendAPDUReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMSENAPDUREQ, "CMmCustomMessHandler::UiccSendAPDUReq" );

    TUiccParamsApduReq params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ( TUiccTrId )aTraId;
    params.serviceType = UICC_APDU_SEND;
    params.apduData.Append( aApduParameters.iCmdData );

    return iMmUiccMessHandler->CreateUiccApduReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSendAPDUResp
//
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccSendAPDUResp(
    TInt aStatus,
    const TDesC8& aFileData,
    TInt aTraId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSendAPDUResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSIMSENAPDURESP, "CMmCustomMessHandler::UiccSendAPDUResp" );

    TInt err( KErrNone );
    const TDesC8* dataPtr = &aFileData;
    CMmDataPackage dataPackage;
    dataPackage.PackData( &dataPtr );

    if( aStatus != UICC_STATUS_OK )
        {
        if( aStatus == UICC_STATUS_SHUTTING_DOWN )
            {
            err = CMmStaticUtility::EpocErrorCode( KErrNotReady, KErrNotFound );
            }
        else
            {
            // UICC_STATUS_FAIL
            err = CMmStaticUtility::EpocErrorCode( KErrGeneral, KErrNotFound );
            }
        }

    if( aTraId == ETrIdSendAPDUv2 )
        {
        iMessageRouter->Complete(
            ECustomSendAPDUReqV2IPC,
            &dataPackage,
            err );
        }
    else
        {
        iMessageRouter->Complete(
            ECustomSendAPDUReqIPC,
            &dataPackage,
            err );
        }

    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCardReaderStatusReq
//
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccCardReaderStatusReq()
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCardReaderStatusReq.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCARDREADERSTATUSREQ, "CMmCustomMessHandler::UiccCardReaderStatusReq" );

    TUiccParamsApduReq params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdCardReaderStatus;
    params.serviceType = UICC_CARD_STATUS_GET;

    // Current UICC IF implementatation does not support getting
    // READER_STATUS_BYTE. However, apdu server uses hard coded values
    // for READER STATUS BYTE bits 1 to 6
    // and information contained in bits 7 and 8 can be acquired with
    // UICC_CARD_REQ request.

    return iMmUiccMessHandler->CreateUiccCardReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCardReaderStatusResp
//
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCardReaderStatusResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCardReaderStatusResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCARDREADERSTATUSRESP, "CMmCustomMessHandler::UiccCardReaderStatusResp" );

    TInt err ( KErrNone );
    CMmDataPackage dataPackage;
    TUint8 cardStatus( 0 );
    TBuf8<1> statusByte;

    cardStatus = aFileData[0];

    if( aStatus == UICC_STATUS_OK )
        {
        switch( cardStatus )
            {
            case UICC_STATUS_CARD_READY:
                {
                // UICC Card Status: UICC_STATUS_CARD_READY ->
                // Card reader status byte: B1101_0000
                statusByte.Append(KUiccCardStatusCardPresentAndPoweredOn);
                break;
                }

            case UICC_STATUS_CARD_DISCONNECTED:
            case UICC_STATUS_CARD_NOT_PRESENT:
                {
                // UICC Card Status: UICC_STATUS_CARD_DISCONNECTED or
                // UICC_STATUS_CARD_NOT_PRESENT ->
                // Card reader status byte: B0001_0000
                statusByte.Append(KUiccCardStatusCardNotPresentOrPoweredOff);
                break;
                }

            default:
                break;
            }
        dataPackage.PackData( &statusByte );

        }
    else
        {
        if( aStatus == UICC_STATUS_SHUTTING_DOWN )
            {
            err = CMmStaticUtility::EpocErrorCode( KErrNotReady, KErrNotFound );
            }
        else
            {
            // UICC_STATUS_FAIL
            err = CMmStaticUtility::EpocErrorCode( KErrGeneral, KErrNotFound );
            }
        }

        iMessageRouter->Complete(
            ECustomGetSimCardReaderStatusIPC,
            &dataPackage,
            err );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCardInd
// Breaks UICC_CARD_IND ISI-message and completes " Notify SimCard Status"
// to CommonTSY.
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCardInd( const TIsiReceiveC& aIsiMessage )
    {
    // Get service type
    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_IND_OFFSET_SERVICETYPE ) );

TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccCardInd, service type: %d", serviceType );
OstTraceExt1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCIND, "CMmCustomMessHandler::UiccCardInd;serviceType=%hhu", serviceType );

    RMmCustomAPI::TSIMCardStatus returnStatus;
    returnStatus = RMmCustomAPI::SimCardUnknowError;

    switch ( serviceType )
        {
        case UICC_CARD_READY :
            {
            returnStatus = RMmCustomAPI::SimCardInserted;
            break;
            }
        case UICC_CARD_DISCONNECTED:
            {
            returnStatus = RMmCustomAPI::SimCardRemoved;
            break;
            }
        default:
            {
            break;
            }
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &returnStatus );
    iMessageRouter->Complete(
        ECustomNotifySimCardStatusIPC,
        &dataPackage,
        KErrNone );
   }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccSendAuthenticateApdu
// Constructs and sends AUTHENTICATE APDU to the UICC server
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccSendAuthenticateApdu(
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu" );

    TInt ret( KErrNone );
    TBool apduSendNeeded( EFalse );

    // Let's construct AUTHENTICATE APDU based on authenticate type
    RMmCustomAPI::TSimAuthenticationBase authBase;
    aDataPackage.UnPackData( authBase );
    TUint8 simAuthType( authBase.ExtensionId() );

    // Fill parameters to instance created from TUiccSendApdu
    // needed for APDU sending
    TUiccSendApdu params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.serviceType = UICC_APPL_APDU_SEND;
    params.fileId = UICC_EF_ID_NOT_PRESENT;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    TUint8 cardType( iMmUiccMessHandler->GetCardType() );
    TBool serviceStatus( EFalse );

    switch( simAuthType )
        {
        case RMmCustomAPI::TSimAuthenticationBase::EEapSim:
            {
            if( UICC_CARD_TYPE_ICC == cardType )
                {
                RMmCustomAPI::TSimAuthenticationEapSim eapSim;
                aDataPackage.UnPackData( eapSim );

                // In 2G we need to send command RUN GSM ALGORITHM to the ICC
                UiccCreateRunGsmAlgorithmApdu( params, eapSim.iRandomParameters, ETrIdRunGsmAlgorithmSim );
                apduSendNeeded = ETrue;
                }
            else if( UICC_CARD_TYPE_UICC == cardType )
                {
                // In 3G we need to send command AUTHENTICATE to the UICC
                UiccCreateGsmSecurityContextApdu( params, aDataPackage );
                apduSendNeeded = ETrue;
                }
            break;
            }
        case RMmCustomAPI::TSimAuthenticationBase::EEapAka:
            {
            RMmCustomAPI::TSimAuthenticationEapAka eapAka;
            aDataPackage.UnPackData( eapAka );

            if( UICC_CARD_TYPE_ICC == cardType )
                {
                // In 2G we need to send command RUN GSM ALGORITHM to the ICC
                UiccCreateRunGsmAlgorithmApdu( params, eapAka.iRandomParameters, ETrIdRunGsmAlgorithmAka );
                apduSendNeeded = ETrue;
                }
            else if( UICC_CARD_TYPE_UICC == cardType )
                {
                // In 3G we need to send command AUTHENTICATE to the UICC
                UiccCreate3GSecurityContextApdu(
                    params,
                    eapAka.iRandomParameters,
                    eapAka.iAUTN,
                    ETrIdEEapAkaAuthenticate );
                apduSendNeeded = ETrue;
                }
            break;
            }
        case RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap:
            {
            serviceStatus = iMmUiccMessHandler->GetServiceStatus( KServiceGBA );

            if( UICC_CARD_TYPE_UICC == cardType &&
                EFalse != serviceStatus )
                {
                UiccCreateGBABootstrappingApdu( params, aDataPackage );
                apduSendNeeded = ETrue;
                }
            else
                {
                // GBA not supported if card type is ICC or GBA not supported in EFust
TFLOGSTRING3("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: GBA not supported (card type: 0x%x, ss: 0x%x)\n", cardType, serviceStatus );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu: GBA not supported (card type: %x, ss: 0x%x)", cardType, serviceStatus );
                ret = KErrGeneral;
                }
            break;
            }
        case RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapUpdate:
            {
            serviceStatus = iMmUiccMessHandler->GetServiceStatus( KServiceGBA );

            if( UICC_CARD_TYPE_UICC == cardType &&
                EFalse != serviceStatus )
                {
                UiccGBABootstrapUpdate( aDataPackage );
                }
            else
                {
                // GBA not supported if card type is ICC or GBA not supported in EFust
TFLOGSTRING3("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: GBA not supported (card type: 0x%x, ss: 0x%x)\n", cardType, serviceStatus );
OstTraceExt2( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu: GBA not supported (card type: 0x%x, ss: 0x%x)", cardType, serviceStatus );
                ret = KErrGeneral;
                }
            break;
              }
        case RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapNafDerivation:
            {
            serviceStatus = iMmUiccMessHandler->GetServiceStatus( KServiceGBA );

            if( UICC_CARD_TYPE_UICC == cardType &&
                EFalse != serviceStatus )
                {
                UiccCreateGBABootstrapNafDerivationApdu( params, aDataPackage );
                apduSendNeeded = ETrue;
                }
            else
                {
                // GBA not supported if card type is ICC or GBA not supported in EFust
TFLOGSTRING3("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: GBA not supported (card type: 0x%x, ss: 0x%x)\n", cardType, serviceStatus );
OstTraceExt2( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu: GBA not supported (card type: 0x%x, ss: 0x%x)", cardType, serviceStatus );
                ret = KErrGeneral;
                }
            break;
            }
        case RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate:
            {
            serviceStatus = iMmUiccMessHandler->GetServiceStatus( KServiceMBMSsecurity );

            if( UICC_CARD_TYPE_UICC == cardType &&
                EFalse != serviceStatus )
                {
                UiccCreateMbmsMskUpdateApdu( params, aDataPackage );
                apduSendNeeded = ETrue;
                }
            else
                {
                // MBMS not supported if card type is ICC or MBMS not supported in EFust
TFLOGSTRING3("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: MBMS security not supported (card type: 0x%x, ss: 0x%x)\n", cardType, serviceStatus );
OstTraceExt2( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu: MBMS security not supported (card type: 0x%x, ss: 0x%x)", cardType, serviceStatus );
                ret = KErrGeneral;
                }
            break;
            }
        case RMmCustomAPI::TSimAuthenticationBase::EMgvMtkGeneration:
            {
            serviceStatus = iMmUiccMessHandler->GetServiceStatus( KServiceMBMSsecurity );

            if( UICC_CARD_TYPE_UICC == cardType &&
                EFalse != serviceStatus )
                {
                UiccCreateMbmsMtkGenerationApdu( params, aDataPackage );
                apduSendNeeded = ETrue;
                }
            else
                {
                // MBMS not supported if card type is ICC or MBMS not supported in EFust
TFLOGSTRING3("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: MBMS security not supported (card type: 0x%x, ss: 0x%x)\n", cardType, serviceStatus );
OstTraceExt2( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu: MBMS security not supported (card type: 0x%x, ss: 0x%x)", cardType, serviceStatus );
                ret = KErrGeneral;
                }
            break;
            }
        case RMmCustomAPI::TSimAuthenticationBase::EMgvMskDeletion:
            {
            serviceStatus = iMmUiccMessHandler->GetServiceStatus( KServiceMBMSsecurity );

            if( UICC_CARD_TYPE_UICC == cardType &&
                EFalse != serviceStatus )
                {
                UiccCreateMbmsMskDeletionApdu( params, aDataPackage );
                apduSendNeeded = ETrue;
                }
            else
                {
                // MBMS not supported if card type is ICC or MBMS not supported in EFust
TFLOGSTRING3("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: MBMS security not supported (card type: 0x%x, ss: 0x%x)\n", cardType, serviceStatus );
OstTraceExt2( TRACE_NORMAL, DUP6_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu: MBMS security not supported (card type: 0x%x, ss: 0x%x)", cardType, serviceStatus );
                ret = KErrGeneral;
                }
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: Unknown APDU\n" );
OstTrace0( TRACE_NORMAL, DUP7_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApdu: Unknown APDU" );
            break;
            }
        }

    if( EFalse != apduSendNeeded )
        {
        // send the apdu to the UICC server
        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    else
        {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccSendAuthenticateApdu: APDU not sent\n" );
OstTrace0( TRACE_NORMAL, DUP8_CMMCUSTOMMESSHANDLER_UICCSENDAUTHENTICATEAPDU, "CMmCustomMessHandler::UiccSendAuthenticateApduu: APDU not sent" );
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateRunGsmAlgorithmApdu
// Constructs GSM context AUTHENTICATE APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateRunGsmAlgorithmApdu(
    TUiccSendApdu& aParams,
    const TDesC8& aRand,
    TUiccTrId aTraId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateRunGsmAlgorithmApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATERUNGSMALGORITHMAPDU, "CMmCustomMessHandler::UiccCreateRunGsmAlgorithmApdu" );

    aParams.trId = aTraId;
    aParams.apdu.Append( KClaIcc );              // CLA
    aParams.apdu.Append( KEvenInstructionCode ); // INS
    aParams.apdu.Append( 0 ); // P1 is set to 0 in case of RUN GSM ALGORITH
    aParams.apdu.Append( 0 ); // P2 is set to 0 in case of RUN GSM ALGORITH
    aParams.apdu.Append( aRand.Size() );     // Lc
    aParams.apdu.Append( aRand );            // Data
    aParams.apdu.Append( KRunGsmAlgorithmRespLen ); // Le
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp
// Handles response APDU for RUN GSM ALGORITHM
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp(
    TInt aTraId,
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCRUNGSMALGORITHMAPDURESP, "CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp" );

    TInt ret( KErrNone );
    TInt ipc( ETrIdRunGsmAlgorithmIms == aTraId ? EMobilePhoneIMSAuthenticate : ECustomGetSimAuthenticationDataIPC );
    RMmCustomAPI::TSimAuthenticationEapSim eapSim;
    RMmCustomAPI::TSimAuthenticationEapAka eapAka;
    RMobilePhone::TImsAuthenticateDataV5 ims;

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        if( KNormalCommandEnding == result )
            {
            if( KApduOk == ValidateReceivedAuthenticateApdu( aTraId, aFileData ) )
                {
                // At the momen UICC server get the response from ICC, but in the
                // future it may be removed from UICC. So then we should get the
                // response from ICC.
                TUint8 index = 0;

                if( ETrIdRunGsmAlgorithmSim == aTraId ) // EAP SIM
                    {
                    // get the SRES
                    eapSim.iSRES.Copy( aFileData.Mid(index, KLenOfSRes) );
                    index += KLenOfSRes;

                    // get the Kc
                    eapSim.iKC.Copy( aFileData.Mid(index, KLenOfKc ) );
                    }
                else if( ETrIdRunGsmAlgorithmIms == aTraId ) // IMS
                    {
                    // get the SRES
                    ims.iRES.Copy( aFileData.Mid(index, KLenOfSRes) );
                    index += KLenOfSRes;

                    // let's calculate ck and ik from kc
                    DeriveCkFromKc( ims.iCK, aFileData.Mid(index, KLenOfKc ) );
                    DeriveIkFromKc( ims.iIK, aFileData.Mid(index, KLenOfKc ) );
                    }
                else // EAP AKA
                    {
                    // get the SRES
                    eapAka.iRES.Copy( aFileData.Mid(index, KLenOfSRes) );
                    index += KLenOfSRes;

                    // let's calculate ck and ik from kc
                    DeriveCkFromKc( eapAka.iCK, aFileData.Mid(index, KLenOfKc ) );
                    DeriveIkFromKc( eapAka.iIK, aFileData.Mid(index, KLenOfKc ) );
                    }
                }
            else
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp. APDU validation fails\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCRUNGSMALGORITHMAPDURESP, "CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp. APDU validation fails" );
                ret = KErrMMEtelAuthenticateFailed;
                }
            }
        else if( KCmdNotAllowedSecurityStatusNotSatisfied == result )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp: Security conditions not satisfied\n" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_UICCRUNGSMALGORITHMAPDURESP, "CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp: Security conditions not satisfied" );
            ret = CMmStaticUtility::EpocErrorCode(
                KErrAccessDenied,
                KErrGsm0707SimPin1Required );
            }
        else
            {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp. unknown result: 0x%X\n", result );
OstTrace1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_UICCRUNGSMALGORITHMAPDURESP, "CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp. unknown result: 0x%x", result );
            ret = KErrMMEtelAuthenticateFailed;
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_UICCRUNGSMALGORITHMAPDURESP, "CMmCustomMessHandler::UiccRunGsmAlgorithmApduResp: UICC status not ok: 0x%x", aStatus );
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( KErrNone != ret )
        {
        iMessageRouter->Complete(
            ipc,
            ret );
        }
    else
        {
        CMmDataPackage dataPackage;
        if( ETrIdRunGsmAlgorithmSim == aTraId ) // EAP SIM
            {
            dataPackage.PackData( &eapSim );
            }
        else if( ETrIdRunGsmAlgorithmIms == aTraId )
            {
            dataPackage.PackData( &ims );
            }
        else
            {
            dataPackage.PackData( &eapAka );
            }

        iMessageRouter->Complete(
            ipc,
            &dataPackage,
            ret );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateGsmSecurityContextApdu
// Constructs GSM context AUTHENTICATE APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateGsmSecurityContextApdu(
    TUiccSendApdu& params,
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateGsmSecurityContextApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATEGSMSECURITYCONTEXTAPDU, "CMmCustomMessHandler::UiccCreateGsmSecurityContextApdu" );

    RMmCustomAPI::TSimAuthenticationEapSim eapSim;
    aDataPackage.UnPackData( eapSim );

    params.trId = ETrIdEEapSimAuthenticate;
    params.apdu.Append( KClaNoSm );             // CLA
    params.apdu.Append( KEvenInstructionCode ); // INS
    params.apdu.Append( 0 ); // P1 is set to 0 in case of even instruction
    params.apdu.Append( KGsmAuthenticationContext ); // P2
    params.apdu.Append( eapSim.iRandomParameters.Size() + 1 ); // Lc
    params.apdu.Append( eapSim.iRandomParameters.Size() );     // Data
    params.apdu.Append( eapSim.iRandomParameters );            // Data
    params.apdu.Append( KMaximumLenOfDataExpected );           // Le
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccGsmSecurityContextApduResp
// Handles response APDU for GSM context AUTHENTICATE APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccGsmSecurityContextApduResp(
    TInt aTraId,
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGsmSecurityContextApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCGSMSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::UiccGsmSecurityContextApduResp" );

    TInt ret( KErrNone );
    RMmCustomAPI::TSimAuthenticationEapSim eapSim;

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        if( KNormalCommandEnding == result )
            {
            if( KApduOk == ValidateReceivedAuthenticateApdu( aTraId, aFileData ) )
                {
                // get the SRES
                TUint8 index( 0 );
                TUint8 len( aFileData[index++] );
                eapSim.iSRES.Copy( aFileData.Mid( index , len ) );
                index += len;

                // get the Kc
                len = aFileData[index++];
                eapSim.iKC.Copy( aFileData.Mid( index , len ) );
                }
            else
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGsmSecurityContextApduResp: APDU validation failed\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCGSMSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::UiccGsmSecurityContextApduResp: APDU validation failed" );
                ret = KErrMMEtelAuthenticateFailed;
                }
            }
        else
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGsmSecurityContextApduResp: unknown result\n" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_UICCGSMSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::UiccGsmSecurityContextApduResp: unknown result" );
            ret = KErrMMEtelAuthenticateFailed;
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccGsmSecurityContextApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_UICCGSMSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::UiccGsmSecurityContextApduResp: UICC status not ok: 0x%x", aStatus );
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( KErrNone != ret )
        {
        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            ret );
        }
    else
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( &eapSim );

        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            &dataPackage,
            ret );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreate3GSecurityContextApdu
// Constructs 3G security context AUTHENTICATE APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreate3GSecurityContextApdu(
    TUiccSendApdu& aParams,
    const TDesC8& aRand,
    const TDesC8& aAuth,
    TUiccTrId aTraId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreate3GSecurityContextApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATE3GSECURITYCONTEXTAPDU, "CMmCustomMessHandler::UiccCreate3GSecurityContextApdu" );

    // len of data is sizeof RAND + sizeof AUTN + two length fields
    // (one for RAND len and one for AUTN len)
    TUint8 lc( aRand.Size() + aAuth.Size() + 2 );

    aParams.trId = aTraId;
    aParams.apdu.Append( KClaNoSm );             // CLA
    aParams.apdu.Append( KEvenInstructionCode ); // INS
    aParams.apdu.Append( 0 ); // P1 is set to 0 in case of even instruction
    aParams.apdu.Append( K3GAuthenticationContext ); // P2
    aParams.apdu.Append( lc );                       // Lc
    aParams.apdu.Append( aRand.Size() ); // len of RAND
    aParams.apdu.Append( aRand );        // RAND
    aParams.apdu.Append( aAuth.Size() );             // len of AUTN
    aParams.apdu.Append( aAuth );                    // AUTN
    aParams.apdu.Append( KMaximumLenOfDataExpected );       // Le
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::Uicc3GSecurityContextApduResp
// Handles response APDU for 3G security context AUTHENTICATE APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::Uicc3GSecurityContextApduResp(
         TInt aTraId,
         TInt aStatus,
         const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::Uicc3GSecurityContextApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICC3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Uicc3GSecurityContextApduResp" );

    TInt ret( KErrNone );
    TInt ipc( ETrIdEEapAkaAuthenticateIms == aTraId ? EMobilePhoneIMSAuthenticate : ECustomGetSimAuthenticationDataIPC );
    RMmCustomAPI::TSimAuthenticationEapAka eapAka;
    RMobilePhone::TImsAuthenticateDataV5 ims;

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        if( KNormalCommandEnding == result )
            {
            if( KApduOk == ValidateReceivedAuthenticateApdu( aTraId, aFileData ) )
                {
                TUint32 index( 0 );

                if( KSuccessfull3GAuthTag == aFileData[index] )
                    {
                    index++;

                    if( ETrIdEEapAkaAuthenticate == aTraId )
                        {
                        // get the RES
                        TUint8 len( aFileData[index++] );
                        eapAka.iRES.Copy( aFileData.Mid( index, len ) );
                        index += len;

                        // get the CK
                        len = aFileData[index++];
                        eapAka.iCK.Copy( aFileData.Mid( index, len ) );
                        index += len;

                        // get the IK
                        len = aFileData[index++];
                        eapAka.iIK.Copy( aFileData.Mid( index, len ) );
                        index += len;

                        // Kc is not supported at the moment because of
                        // RMmCustomAPI::TSimAuthenticationEapAka doesn't
                        // have parameter for that
                        }
                    else
                        {
                        // get the RES
                        TUint8 len( aFileData[index++] );
                        ims.iRES.Copy( aFileData.Mid( index, len ) );
                        index += len;

                        // get the CK
                        len = aFileData[index++];
                        ims.iCK.Copy( aFileData.Mid( index, len ) );
                        index += len;

                        // get the IK
                        len = aFileData[index++];
                        ims.iIK.Copy( aFileData.Mid( index, len ) );
                        index += len;
                        }
                    }
                else if( KSyncFailureTag == aFileData[index] )
                    {
                    index++;

                    if( ETrIdEEapAkaAuthenticate == aTraId )
                        {
                        // get the AUTS
                        TUint8 len( aFileData[index++] );
                        eapAka.iAUTS.Copy( aFileData.Mid( index, len ) );
                        }
                    else
                        {
                        // get the AUTS
                        TUint8 len( aFileData[index++] );
                        ims.iAUTS.Copy( aFileData.Mid( index, len ) );
                        }
                    ret = KErrMMEtelSqnVerificationFailed;
                    }
                }
            else
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::Uicc3GSecurityContextApduResp: APDU validation failed\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICC3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Uicc3GSecurityContextApduResp: APDU validation failed" );
                ret = KErrMMEtelAuthenticateFailed;
                }
            }
        else if( KAppAuthErrorIncorrectMac == result )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::Uicc3GSecurityContextApduResp: incorrect MAC\n" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_UICC3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Uicc3GSecurityContextApduResp: incorrect MAC" );
            // no parameters in this case
            ret = KErrMMEtelMacVerificationFailed;
            }
        else if( KCmdNotAllowedSecurityStatusNotSatisfied == result )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::Uicc3GSecurityContextApduResp: Security conditions not satisfied\n" );
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_UICC3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Uicc3GSecurityContextApduResp: Security conditions not satisfied" );
            ret = CMmStaticUtility::EpocErrorCode(
                KErrAccessDenied,
                KErrGsm0707SimPin1Required );
            }
        else
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::Uicc3GSecurityContextApduResp: unknown result\n" );
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_UICC3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Uicc3GSecurityContextApduResp: unknown result" );
            ret = KErrMMEtelAuthenticateFailed;
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::Uicc3GSecurityContextApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_UICC3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Uicc3GSecurityContextApduResp: UICC status not ok: 0x%x", aStatus );
        ret = ETrIdEEapAkaAuthenticateIms == aTraId ? KErrGeneral : KErrMMEtelAuthenticateFailed;
        }

    if( KErrNone == ret ||
        KErrMMEtelSqnVerificationFailed == ret ||
        KErrMMEtelMacVerificationFailed == ret )
        {
        CMmDataPackage dataPackage;

        if( ETrIdEEapAkaAuthenticate == aTraId )
            {
            dataPackage.PackData( &eapAka );
            }
        else
            {
            dataPackage.PackData( &ims );
            }

        iMessageRouter->Complete(
            ipc,
            &dataPackage,
            ret );
        }
    else
        {
        iMessageRouter->Complete(
            ipc,
            ret );
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateGBABootstrappingApdu
// Constructs GBA security context AUTHENTICATE APDU (bootstrapping mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateGBABootstrappingApdu(
    TUiccSendApdu& aParams,
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateGBABootstrappingApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATEGBABOOTSTRAPPINGAPDU, "CMmCustomMessHandler::UiccCreateGBABootstrappingApdu" );

    RMmCustomAPI::TSimAuthenticationGbaBootstrap gbaBootstrap;
    aDataPackage.UnPackData( gbaBootstrap );

    // len of data is GBA bootstrapping mode tag (1 byte) + sizeof RAND
    // + sizeof AUTN + two length fields (one for RAND len and one for AUTN len)
    TUint8 lc( 1 + gbaBootstrap.iRandomParameters.Size() + gbaBootstrap.iAUTN.Size() + 2 );

    aParams.trId = ETrIdEGbaBootstrap;
    aParams.apdu.Append( KClaNoSm );             // CLA
    aParams.apdu.Append( KEvenInstructionCode ); // INS
    aParams.apdu.Append( 0 ); // P1 is set to 0 in case of even instruction
    aParams.apdu.Append( KGBAAuthenticationContext ); // P2
    aParams.apdu.Append( lc );                        // Lc
    aParams.apdu.Append( KGBABootstappingModeTag );   // GBA bootstrapping mode tag
    aParams.apdu.Append( gbaBootstrap.iRandomParameters.Size() ); // len of RAND
    aParams.apdu.Append( gbaBootstrap.iRandomParameters );        // RAND
    aParams.apdu.Append( gbaBootstrap.iAUTN.Size() );             // len of AUTN
    aParams.apdu.Append( gbaBootstrap.iAUTN );                    // AUTN
    aParams.apdu.Append( KMaximumLenOfDataExpected );             // Le
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccGBABootstrappingApduResp
// Handles response APDU for GBA security context AUTHENTICATE APDU
// (bootstrapping mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccGBABootstrappingApduResp(
    TInt aTraId,
    TInt aStatus,
     const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrappingApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPPINGAPDURESP, "CMmCustomMessHandler::UiccGBABootstrappingApduResp" );

    TInt ret( KErrNone );
    RMmCustomAPI::TSimAuthenticationGbaBootstrap gbaBootstrap;

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        if( KNormalCommandEnding == result )
            {
            if( KApduOk == ValidateReceivedAuthenticateApdu( aTraId, aFileData ) )
                {
                TUint32 index( 0 );

                if( KSuccessfull3GAuthTag == aFileData[index] )
                    {
                    index++;

                    // get the RES
                    TUint8 len( aFileData[index++] );
                    gbaBootstrap.iRES.Copy( aFileData.Mid( index, len ) );
                    }

                else if( KSyncFailureTag == aFileData[index] )
                    {
                    index++;

                    // get the AUTS
                    TUint8 len( aFileData[index++] );
                    gbaBootstrap.iAUTS.Copy( aFileData.Mid( index, len ) );

                    ret = KErrMMEtelSqnVerificationFailed;
                    }
                }
            else
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrappingApduResp: APDU validation failed\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPPINGAPDURESP, "CMmCustomMessHandler::UiccGBABootstrappingApduResp: APDU validation failed" );
                ret = KErrGeneral;
                }
            }
        else if( KAppAuthErrorIncorrectMac == result )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrappingApduResp: incorrect MAC\n" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPPINGAPDURESP, "CMmCustomMessHandler::UiccGBABootstrappingApduResp: incorrect MAC" );
            // no parameters in this case
            ret = KErrMMEtelMacVerificationFailed;
            }
        else
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrappingApduResp: unknown result\n" );
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPPINGAPDURESP, "CMmCustomMessHandler::UiccGBABootstrappingApduResp: unknown result" );
            ret = KErrGeneral;
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccGBABootstrappingApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPPINGAPDURESP, "CMmCustomMessHandler::UiccGBABootstrappingApduResp: UICC status not ok: 0x%x", aStatus );
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( KErrNone == ret ||
        KErrMMEtelSqnVerificationFailed == ret ||
        KErrMMEtelMacVerificationFailed == ret )
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( &gbaBootstrap );

        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            &dataPackage,
            ret );
        }
    else
        {
        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            ret );
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccGBABootstrapUpdate
// starts GBA bootstrap update operation
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccGBABootstrapUpdate(
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrapUpdate.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPUPDATE, "CMmCustomMessHandler::UiccGBABootstrapUpdate" );

    // GBA bootstrap update is done by first reading of elementary file EFgba.
    // After that RAND is stored temporarily and B-Tid and keylifetime is written
    // to EFgba. After write operation, RAND is completed to the client.

    RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate gbaBootstrapUpdate;
    aDataPackage.UnPackData( gbaBootstrapUpdate );

    iBTidBuf.Zero();
    iKeyLifetimeBuf.Zero();

    // Store B-TID temporarily
    iBTidBuf.Append( gbaBootstrapUpdate.iBTid );

    // Store Key lifetime temporarily
    iKeyLifetimeBuf.Append( gbaBootstrapUpdate.iKeyLifeTime );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdEGbaBootstrapRead;

    params.dataAmount = 0;
    params.dataOffset = 0;
    params.fileId = KElemGba;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccGBABootstrapReadResp
// Handles response to elementary file EFgba read operation
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccGBABootstrapReadResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrapReadResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPREADRESP, "CMmCustomMessHandler::UiccGBABootstrapReadResp" );

    iRandBuf.Zero();

    if( UICC_STATUS_OK == aStatus)
        {
        // Get the rand len
        TUint32 index( 0 );
        TUint8 randLen( aFileData[index++] );

        if( aFileData.Length() >= randLen +
            iBTidBuf.Length() +
            iKeyLifetimeBuf.Length() +
            3 )
            {
            // store rand temporarily so that we can complete it
            // when B-Tid and keylifetime is updated to the EFgba.
            iRandBuf.Copy( aFileData.Mid( index, randLen ) );

            // Set parameters for UICC_APPL_CMD_REQ message
            TUiccWriteTransparent params;
            params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
            params.trId = ETrIdEGbaBootstrapUpdate;
            params.dataOffset = randLen + 1;
            params.fileId = KElemGba;
            params.fileIdSfi = UICC_SFI_NOT_PRESENT;
            params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;

            // File id path
            params.filePath.Append( KMasterFileId >> 8 );
            params.filePath.Append( KMasterFileId );
            params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

            // fill file data
            params.fileData.Append( iBTidBuf.Length() );
            params.fileData.Append( iBTidBuf );
            params.fileData.Append( iKeyLifetimeBuf.Length() );
            params.fileData.Append( iKeyLifetimeBuf );

            iMmUiccMessHandler->CreateUiccApplCmdReq( params );
            }
        else
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrapReadResp. File too small for update\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPREADRESP, "CMmCustomMessHandler::UiccGBABootstrapReadResp. File too small for update" );
            iMessageRouter->Complete(
                ECustomGetSimAuthenticationDataIPC,
                KErrArgument );
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccGBABootstrapReadResp. EFgba reading failed (0x%x)\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPREADRESP, "CMmCustomMessHandler::UiccGBABootstrapReadResp. EFgba reading failed (%x)", aStatus );
        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            KErrMMEtelAuthenticateFailed );
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccGBABootstrapUpdateResp
// Handles response to elementary file EFgba write operation
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccGBABootstrapUpdateResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrapUpdateResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCGBABOOTSTRAPUPDATERESP, "CMmCustomMessHandler::UiccGBABootstrapUpdateResp" );

    TInt ret( KErrGeneral );
    RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate gbaBootstrapUpdate;

    if( UICC_STATUS_OK == aStatus )
        {
        ret = KErrNone;
        gbaBootstrapUpdate.iRandomParameters.Copy( iRandBuf );
        }
    else
        {
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( KErrNone == ret )
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( &gbaBootstrapUpdate );
        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            &dataPackage,
            ret );
        }
    else
        {
        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            ret );
        }
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateGBABootstrapNafDerivationApdu
// Constructs GBA security context AUTHENTICATE APDU (NAF derivation mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateGBABootstrapNafDerivationApdu(
    TUiccSendApdu& aParams,
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateGBABootstrapNafDerivationApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATEGBABOOTSTRAPNAFDERIVATIONAPDU, "CMmCustomMessHandler::UiccCreateGBABootstrapNafDerivationApdu" );

    RMmCustomAPI::TSimAuthenticationGbaNafDerivation gbaBootstrapNafDerivation;
    aDataPackage.UnPackData( gbaBootstrapNafDerivation );

    // len of data is GBA NAF derivation mode tag (1 byte) + sizeof NAFId
    // + sizeof IMPI + two length fields (one for NAFId len and one for IMPI len)
    TUint8 lc( 1 +
               gbaBootstrapNafDerivation.iNafId.Size() +
               gbaBootstrapNafDerivation.iImpi.Size() +
               2 );

    aParams.trId = ETrIdEGbaNafDerivation;
    aParams.apdu.Append( KClaNoSm );             // CLA
    aParams.apdu.Append( KEvenInstructionCode ); // INS
    aParams.apdu.Append( 0 ); // P1 is set to 0 in case of even instruction
    aParams.apdu.Append( KGBAAuthenticationContext ); // P2
    aParams.apdu.Append( lc );                        // Lc
    aParams.apdu.Append( KGBANAFDerivationModeTag );   // GBA bootstrapping mode tag
    aParams.apdu.Append( gbaBootstrapNafDerivation.iNafId.Size() ); // len of NAFId
    aParams.apdu.Append( gbaBootstrapNafDerivation.iNafId );        // NAFId
    aParams.apdu.Append( gbaBootstrapNafDerivation.iImpi.Size() );  // len of IMPI
    aParams.apdu.Append( gbaBootstrapNafDerivation.iImpi );         // IMPI
    aParams.apdu.Append( KMaximumLenOfDataExpected );               // Le
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccGBABootstrapNafDerivationApduResp
// Handles response APDU for GBA security context AUTHENTICATE APDU
// (NAF derivation mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccGBANafDerivationApduResp(
    TInt aTraId,
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrapNafDerivationApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCGBANAFDERIVATIONAPDURESP, "CMmCustomMessHandler::UiccGBANafDerivationApduResp" );

    TInt ret( KErrNone );
    RMmCustomAPI::TSimAuthenticationGbaNafDerivation gbaNafDerivation;

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        if( KNormalCommandEnding == result )
            {
            if( KApduOk == ValidateReceivedAuthenticateApdu( aTraId, aFileData ) )
                {
                // Let's skip "Successful GBA operation" tag
                // and start with Length of Length of Ks ext NAF
                TUint32 index( 1 );

                // get the Ks ext NAF
                TUint8 len( aFileData[index++] );
                gbaNafDerivation.iKsExtNaf.Copy( aFileData.Mid( index, len ) );
                }
            else
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrapNafDerivationApduResp: APDU validation failed\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCGBANAFDERIVATIONAPDURESP, "CMmCustomMessHandler::UiccGBANafDerivationApduResp: APDU validation failed" );
                ret = KErrGeneral;
                }
            }
        else if( KCmdNotAllowedConditionsNotSatisfied == result )
            {
            ret = KErrNotSupported;
            }
        else
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccGBABootstrapNafDerivationApduResp: unknown result\n" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_UICCGBANAFDERIVATIONAPDURESP, "CMmCustomMessHandler::UiccGBANafDerivationApduResp: unknown result" );
            ret = KErrGeneral;
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccGBABootstrapNafDerivationApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_UICCGBANAFDERIVATIONAPDURESP, "CMmCustomMessHandler::UiccGBANafDerivationApduResp: UICC status not ok: 0x%x", aStatus );
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( KErrNone == ret )
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( &gbaNafDerivation );

        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            &dataPackage,
            ret );
        }
    else
        {
        iMessageRouter->Complete(
            ECustomGetSimAuthenticationDataIPC,
            ret );
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateMbmsMskUpdateApdu
// Constructs MBMS security context AUTHENTICATE APDU (MSK Update Mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateMbmsMskUpdateApdu(
    TUiccSendApdu& params,
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateMbmsMskUpdateApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATEMBMSMSKUPDATEAPDU, "CMmCustomMessHandler::UiccCreateMbmsMskUpdateApdu" );

    // Note. MBMS functionality is not tested in real hardware and in real environmen
    //  because of there is no users for this authentication type. So this is implemented
    // with the best knowledge at the moment and it can contain some bugs which
    // can be found when this is tested in real environment.

    RMmCustomAPI::TSimAuthenticationMgvMskUpdate mskUpdate;
    aDataPackage.UnPackData( mskUpdate );

    // len of data is MBMS Data Object tag (1 byte) +
    // MBMS Data Object length (1 byte) +
    // MBMS Security Context Mode (1 byte) + size of Mikey
    TUint8 lc( 1 + 1 + 1 + mskUpdate.iMikey.Size() );

    params.trId = ETrIdEMbmsMskUpdate;
    params.apdu.Append( KClaNoSm );             // CLA
    params.apdu.Append( KOddInstructionCode );  // INS
    params.apdu.Append( KFirstBlockOfAuthenticationData ); // P1
    params.apdu.Append( KMBMSAuthenticationContext ); // P2
    params.apdu.Append( lc );                         // Lc
    params.apdu.Append( KMBMSDataObjectTag );          // MBMS Data object tag
    params.apdu.Append( mskUpdate.iMikey.Size() + 1 ); // MBMS data obj len
    params.apdu.Append( KMskUpdateMode );              // MBMS Security Context Mode
    params.apdu.Append( mskUpdate.iMikey );            // Mikey
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccMbmsMskUpdateApduResp
// Handles response APDU for MBMS security context AUTHENTICATE APDU
// (MSK Update Mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccMbmsMskUpdateApduResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccMbmsMskUpdateApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCMBMSMSKUPDATEAPDURESP, "CMmCustomMessHandler::UiccMbmsMskUpdateApduResp" );

    // Note. MBMS functionality is not tested in real hardware and in real environment
    // because of at the moment there is no users for this authentication type.
    // So this is implemented with the best knowledge at the moment and it can contain
    // some bugs which can be found when this is tested in real environment.

    TInt ret( KErrGeneral );
    RMmCustomAPI::TSimAuthenticationMgvMskUpdate mskUpdate;
    TBool completeNeeded( ETrue );

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        switch( result )
            {
            case KNormalCommandEnding:
            case KWarningMoreDataAvailable:
                {
                TBuf8<KMaxApduSize> mbmsOperationData;

                if( FindTlvObject( KMbmsOperationResponseTag53, aFileData.Mid( 0, aFileData.Length() - 2 ), mbmsOperationData ) ||
                    FindTlvObject( KMbmsOperationResponseTag73, aFileData.Mid( 0, aFileData.Length() - 2 ), mbmsOperationData ) )
                    {
                    ret = KErrNone;
                    mskUpdate.iMikey.Zero();
                    if( KSuccessfullMbmsOperationTag == mbmsOperationData[0] )
                        {
                        UiccMskUpdateHandleMbmsOperationData( mskUpdate, mbmsOperationData );
                        }
                    else
                        {
                        UiccMskUpdateHandleOMABcastOperationData( mskUpdate, mbmsOperationData );
                        }
                    }
                break;
                }
            case KWarningAuthRespAvailable:
                {
                UiccCreateFirstBlockOfAuthRespApdu( ETrIdEMbmsMskUpdate );
                completeNeeded = EFalse;
                break;
                }
            case KCmdNotAllowedConditionsNotSatisfied:
                {
                ret = KErrCustomSCCondOfuseNotSatisfied;
                break;
                }
            case KAppErrorAuthMbmsOutOfMemMuk:
                {
                ret = KErrCustomSCNoMemSpaceAvailableMukAuthError;
                break;
                }
            case KAppErrorAuthMbmsOutOfMemMsk:
                {
                ret = KErrCustomSCNoMemSpaceAvailableAuthError;
                break;
                }
            case KAppAuthErrorIncorrectMac:
                {
                ret = KErrCustomSCIncorrectMACAuthError;
                break;
                }
            case KWrongParametersDataNotFound:
                {
                ret = KErrCustomSCRefDataNotFound;
                break;
                }
            default:
                {
                ret = KErrGeneral;
                break;
                }
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccMbmsMskUpdateApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCMBMSMSKUPDATEAPDURESP, "CMmCustomMessHandler::UiccMbmsMskUpdateApduResp: UICC status not ok: 0x%x", aStatus );
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( completeNeeded )
        {
        if( KErrNone == ret )
            {
            CMmDataPackage dataPackage;
            dataPackage.PackData( &mskUpdate );

            iMessageRouter->Complete(
                ECustomGetSimAuthenticationDataIPC,
                &dataPackage,
                ret );
            }
        else
            {
            iMessageRouter->Complete(
                ECustomGetSimAuthenticationDataIPC,
                ret );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccMskUpdateHandleMbmsOperationData
// Handles MBMS operation data from MSK Update authenticate APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccMskUpdateHandleMbmsOperationData(
    RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aMskUpdate,
    TDesC8& aMbmsData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccMskUpdateHandleMbmsOperationData.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCMSKUPDATEHANDLEMBMSOPERATIONDATA, "CMmCustomMessHandler::UiccMskUpdateHandleMbmsOperationData" );

    TUint8 index( 0 );
    TUint32 objLen( aMbmsData.Length() );

    if( KSuccessfullMbmsOperationTag == aMbmsData[index] &&
        1 <= objLen &&
        ( KMaxMbmsMikeyLen + 1 ) >= objLen  ) // + 1 for 0xDB TLV tag
        {
        index++;

        // len of MIKEY is obj len - 1 because of
        // obj data contains 1 byte for
        // Successfull Mbms Operation Tag
        TUint8 mikeyLen = objLen - 1;
        if( 0 < mikeyLen )
            {
            aMskUpdate.iMikey.Copy( aMbmsData.Mid( index, mikeyLen ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccMskUpdateHandleMbmsOperationData
// Handles OMA BCAST operation data from authenticate APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccMskUpdateHandleOMABcastOperationData(
                RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aMskUpdate,
                TDesC8& aMbmsData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccMskUpdateHandleOMABcastOperationData.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCMSKUPDATEHANDLEOMABCASTOPERATIONDATA, "CMmCustomMessHandler::UiccMskUpdateHandleOMABcastOperationData" );

    TBuf8<KMaxApduSize> omaBcastData;

    if( FindTlvObject( KOMABcastOperationResponseTag, aMbmsData, omaBcastData ) )
        {
        TBuf8<KMaxApduSize> bcastManagementData;
        TBuf8<KMaxParentalRatingDataSize> parentalRatingData;
        TBuf8<KMaxSpeSize> speTypeNotSupportedData;
        TBuf8<KMaxApduSize> mikeyData;

        if( FindTlvObject( KBcastManagementDataTag, omaBcastData, bcastManagementData ) )
            {
            aMskUpdate.iBCASTManagement.Copy( bcastManagementData );
            }
        if( FindTlvObject( KParentalRatingDataTag, omaBcastData, parentalRatingData ) )
            {
            aMskUpdate.iParentalRating.Copy( parentalRatingData );
            }
        if( FindTlvObject( KSPETypeNotSupportedTag, omaBcastData, speTypeNotSupportedData ) )
            {
            aMskUpdate.iSecurityPolicyExt.Copy( speTypeNotSupportedData );
            }

        if( FindTlvObject( KMikeyMessageTag, omaBcastData, mikeyData ) )
            {
            aMskUpdate.iMikey.Copy( mikeyData );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateMbmsMtkGenerationApdu
// Constructs MBMS security context AUTHENTICATE APDU (MTK Generation Mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateMbmsMtkGenerationApdu(
    TUiccSendApdu& params,
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateMbmsMtkGenerationApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATEMBMSMTKGENERATIONAPDU, "CMmCustomMessHandler::UiccCreateMbmsMtkGenerationApdu" );

    // Note. MBMS functionality is not tested in real hardware and in real environment
    // because of at the moment there is no users for this authentication type.
    // So this is implemented with the best knowledge at the moment and it can contain
    // some bugs which can be found when this is tested in real environment.

    RMmCustomAPI::TSimAuthenticationMgvMtkGeneration mtkGen;
    aDataPackage.UnPackData( mtkGen );

    // len of data is MBMS Data Object tag (1 byte) +
    // MBMS Data Object length (1 byte) +
    // MBMS Security Context Mode (1 byte) + size of Mikey
    TUint8 lc( 1 + 1 + 1 + mtkGen.iMikey.Size() );

    params.trId = ETrIdEMbmsMtkGeneration;
    params.apdu.Append( KClaNoSm );             // CLA
    params.apdu.Append( KOddInstructionCode );  // INS
    params.apdu.Append( KFirstBlockOfAuthenticationData ); // P1
    params.apdu.Append( KMBMSAuthenticationContext ); // P2
    params.apdu.Append( lc );                         // Lc
    params.apdu.Append( KMBMSDataObjectTag );          // MBMS Data object tag
    params.apdu.Append( mtkGen.iMikey.Size() + 1 ); // MBMS data obj len
    params.apdu.Append( KMtkGenerationMode );          // MBMS Security Context Mode
    params.apdu.Append( mtkGen.iMikey );            // Mikey
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccMbmsMtkGenerationApduResp
// Handles response APDU for MBMS security context AUTHENTICATE APDU
// (MTK Generation Mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccMbmsMtkGenerationApduResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccMbmsMtkGenerationApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCMBMSMTKGENERATIONAPDURESP, "CMmCustomMessHandler::UiccMbmsMtkGenerationApduResp" );

    // Note. MBMS functionality is not tested in real hardware and in real environment
    // because of at the moment there is no users for this authentication type.
    // So this is implemented with the best knowledge at the moment and it can contain
    // some bugs which can be found when this is tested in real environment.

    TInt ret( KErrGeneral );
    RMmCustomAPI::TSimAuthenticationMgvMtkGeneration mtkGen;
    TBool completeNeeded( ETrue );

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        switch( result )
            {
            case KNormalCommandEnding:
            case KWarningMoreDataAvailable:
                {
                TBuf8<KMaxApduSize> mbmsOperationData;

                if( FindTlvObject( KMbmsOperationResponseTag53, aFileData.Mid( 0, aFileData.Length() - 2 ), mbmsOperationData ) ||
                    FindTlvObject( KMbmsOperationResponseTag73, aFileData.Mid( 0, aFileData.Length() - 2 ), mbmsOperationData ) )
                    {
                    ret = KErrNone;
                    if( KSuccessfullMbmsOperationTag == mbmsOperationData[0] )
                        {
                        UiccMtkGenHandleMbmsOperationData( mtkGen, mbmsOperationData );
                        }
                    else
                        {
                        UiccMtkGenHandleOMABcastOperationData( mtkGen, mbmsOperationData );
                        }
                    }
                break;
                }
            case KWarningAuthRespAvailable:
                {
                UiccCreateFirstBlockOfAuthRespApdu( ETrIdEMbmsMtkGeneration );
                completeNeeded = EFalse;
                break;
                }
            case KCmdNotAllowedConditionsNotSatisfied:
                {
                ret = KErrCustomSCCondOfuseNotSatisfied;
                break;
                }
            case KAppErrorAuthMbmsKeyFresh:
                {
                ret = KErrCustomSCKeyRefreshFail;
                break;
                }
            case KAppAuthErrorIncorrectMac:
                {
                ret = KErrCustomSCIncorrectMACAuthError;
                break;
                }
            case KWrongParametersDataNotFound:
                {
                ret = KErrCustomSCRefDataNotFound;
                break;
                }
            default:
                {
                ret = KErrGeneral;
                break;
                }
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccMbmsMtkGenerationApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCMBMSMTKGENERATIONAPDURESP, "CMmCustomMessHandler::UiccMbmsMtkGenerationApduResp: UICC status not ok: 0x%x", aStatus );
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( completeNeeded )
        {
        if( KErrNone == ret )
            {
            CMmDataPackage dataPackage;
            dataPackage.PackData( &mtkGen );

            iMessageRouter->Complete(
                ECustomGetSimAuthenticationDataIPC,
                &dataPackage,
                ret );
            }
        else
            {
            iMessageRouter->Complete(
                ECustomGetSimAuthenticationDataIPC,
                ret );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccMtkGenHandleMbmsOperationData
// Handles MBMS operation data from MTK Generation authenticate APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccMtkGenHandleMbmsOperationData(
    RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aMtkGen,
    TDesC8& aMbmsData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccMtkGenHandleMbmsOperationData.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCMTKGENHANDLEMBMSOPERATIONDATA, "CMmCustomMessHandler::UiccMtkGenHandleMbmsOperationData" );

    TUint8 index( 0 );
    TUint32 objLen( aMbmsData.Length() );

    if( KSuccessfullMbmsOperationTag == aMbmsData[index] &&
        1 <= objLen &&
        ( KMaxMbmsSaltLen + 1 ) >= objLen  ) // + 1 for 0xDB TLV tag
        {
        index++;

        // len of SALT is obj len - 1 because of
        // obj data contains 1 byte for
        // Successfull Mbms Operation Tag
        TUint8 saltLen( objLen - 1 );
        if( 0 < saltLen )
            {
            aMtkGen.iMtkSalt.Copy( aMbmsData.Mid( index, saltLen ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccMtkGenHandleOMABcastOperationData
// Handles OMA BCAST operation data from authenticate APDU
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccMtkGenHandleOMABcastOperationData(
         RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aMtkGen,
         TDesC8& aMbmsData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccMtkGenHandleOMABcastOperationData.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCMTKGENHANDLEOMABCASTOPERATIONDATA, "CMmCustomMessHandler::UiccMtkGenHandleOMABcastOperationData" );

    TBuf8<KMaxApduSize> omaBcastData;

    if( FindTlvObject( KOMABcastOperationResponseTag, aMbmsData, omaBcastData ) )
        {
        TBuf8<KMaxApduSize> bcastManagementData;
        TBuf8<KMaxApduSize> tekData;
        TBuf8<KMaxApduSize> saltData;
        TBuf8<KMaxApduSize> parentalControlData;

        if( FindTlvObject( KBcastManagementDataTag, omaBcastData, bcastManagementData ) )
            {
            aMtkGen.iBCASTManagement.Copy( bcastManagementData );
            }
        if( FindTlvObject( KTekDataTag, omaBcastData, tekData ) )
            {
            aMtkGen.iTrafficEncryptionKey.Copy( tekData );
            }
        if( FindTlvObject( KParentalControlTag, omaBcastData, parentalControlData ) )
            {
            // first parameter of parental control data is "key reference for second
            // application PIN defined for parental contol" but it's not supported
            // at the moment
            aMtkGen.iParentalControl.Append( parentalControlData[1] ); // rating type
            aMtkGen.iParentalControl.Append( parentalControlData[2] ); // rating value
            aMtkGen.iParentalControl.Append( parentalControlData[3] ); // level granted value
            }
        if( FindTlvObject( KSaltDataTag, omaBcastData, saltData ) )
            {
            aMtkGen.iMtkSalt.Copy( saltData );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateMbmsMskDeletionApdu
// Constructs MBMS security context AUTHENTICATE APDU (MSK Deletion Mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateMbmsMskDeletionApdu(
    TUiccSendApdu& params,
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateMbmsMskDeletionApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATEMBMSMSKDELETIONAPDU, "CMmCustomMessHandler::UiccCreateMbmsMskDeletionApdu" );

    // Note. MBMS functionality is not tested in real hardware and in real environment
    // because of at the moment there is no users for this authentication type.
    // So this is implemented with the best knowledge at the moment and it can contain
    // some bugs which can be found when this is tested in real environment.

    RMmCustomAPI::TSimAuthenticationMgvMskDeletion mskDel;
    aDataPackage.UnPackData( mskDel );

    // len of data is MBMS Data Object tag (1 byte) +
    // MBMS Data Object length (1 byte) +
    // MBMS Security Context Mode (1 byte) +
    // size of Key Domain Id + size of Key Group Id Part
    TUint8 lc( 1 +
               1 +
               1 +
               mskDel.iKeyDomainId.Size() +
               mskDel.iKeyGroupIdPart.Size() );

    // data size in MBMS Data Object Tag is:
    // MBMS Security Context Mode (1 byte) +
    // size of Key Domain Id + size of Key Group Id Part
    TUint8 dataSize( 1 +
                     mskDel.iKeyDomainId.Size() +
                     mskDel.iKeyGroupIdPart.Size() );

    params.trId = ETrIdEMbmsMskDeletion;
    params.apdu.Append( KClaNoSm );             // CLA
    params.apdu.Append( KOddInstructionCode );  // INS
    params.apdu.Append( KFirstBlockOfAuthenticationData ); // P1
    params.apdu.Append( KMBMSAuthenticationContext ); // P2
    params.apdu.Append( lc );                         // Lc
    params.apdu.Append( KMBMSDataObjectTag );       // MBMS Data object tag
    params.apdu.Append( dataSize );                 // MBMS data obj len
    params.apdu.Append( KMskDeletionMode );         // MBMS Security Context Mode
    params.apdu.Append( mskDel.iKeyDomainId );      // Key Domain Id
    params.apdu.Append( mskDel.iKeyGroupIdPart );   // Key Group Id
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccMbmsMskDeletionApduResp
// Handles response APDU for MBMS security context AUTHENTICATE APDU
// (MSK Deletion Mode)
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccMbmsMskDeletionApduResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccMbmsMskDeletionApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCMBMSMSKDELETIONAPDURESP, "CMmCustomMessHandler::UiccMbmsMskDeletionApduResp" );

    // Note. MBMS functionality is not tested in real hardware and in real environment
    // because of at the moment there is no users for this authentication type.
    // So this is implemented with the best knowledge at the moment and it can contain
    // some bugs which can be found when this is tested in real environment.

    TInt ret( KErrGeneral );
    RMmCustomAPI::TSimAuthenticationMgvMskDeletion mskDel;
    TBool completeNeeded( ETrue );

    if( UICC_STATUS_OK == aStatus )
        {
        // get SW1 and SW2
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2(aFileData[aFileData.Length() - KSw2Position] );

        TUint8 result( MapSw1Sw2ToAuthenticateResult( sw1, sw2 ) );

        switch( result )
            {
            case KNormalCommandEnding:
                {
                TBuf8<KMaxApduSize> mbmsOperationData;

                if( FindTlvObject( KMbmsOperationResponseTag53, aFileData.Mid( 0, aFileData.Length() - 2 ), mbmsOperationData ) )
                    {
                    if( KSuccessfullMbmsOperationTag == mbmsOperationData[0] )
                        {
                        ret = KErrNone;
                        }
                    }
                break;
                }
            case KWarningAuthRespAvailable:
                {
                UiccCreateFirstBlockOfAuthRespApdu( ETrIdEMbmsMskDeletion );
                completeNeeded = EFalse;
                break;
                }
            case KWrongParametersDataNotFound:
                {
                ret = KErrCustomSCRefDataNotFound;
                break;
                }
            default:
                {
                ret = KErrGeneral;
                break;
                }
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccMbmsMskDeletionApduResp: UICC status not ok: 0x%x\n", aStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_UICCMBMSMSKDELETIONAPDURESP, "CMmCustomMessHandler::UiccMbmsMskDeletionApduResp: UICC status not ok: 0x%x", aStatus );
        ret = KErrMMEtelAuthenticateFailed;
        }

    if( completeNeeded )
        {
        if( KErrNone == ret )
            {
            CMmDataPackage dataPackage;
            dataPackage.PackData( &mskDel );

            iMessageRouter->Complete(
                ECustomGetSimAuthenticationDataIPC,
                &dataPackage,
                ret );
            }
        else
            {
            iMessageRouter->Complete(
                ECustomGetSimAuthenticationDataIPC,
                ret );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccCreateFirstBlockOfAuthRespApdu
// Constructs APDU to get authenticate response data in case of odd ins code
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccCreateFirstBlockOfAuthRespApdu( TUiccTrId aTrId )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccCreateFirstBlockOfAuthRespApdu.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCCREATEFIRSTBLOCKOFAUTHRESPAPDU, "CMmCustomMessHandler::UiccCreateFirstBlockOfAuthRespApdu" );

    // Fill parameters to instance created from TUiccSendApdu
    // needed for APDU sending
    TUiccSendApdu params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.serviceType = UICC_APPL_APDU_SEND;
    params.fileId = UICC_EF_ID_NOT_PRESENT;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    params.trId = aTrId;
    params.apdu.Append( KClaNoSm );             // CLA
    params.apdu.Append( KOddInstructionCode );  // INS
    params.apdu.Append( KFirstBlockOfAuthenticationResponseData ); // P1
    params.apdu.Append( 0x00 ); // P2
    params.apdu.Append( 0x00 ); // Le

    iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::MapSw1Sw2ToAuthenticateResult
// Maps sw1 and sw2 from response authenticate apdu to result
// -----------------------------------------------------------------------------
//
TUint8 CMmCustomMessHandler::MapSw1Sw2ToAuthenticateResult( TUint8 sw1, TUint8 sw2 )
    {
TFLOGSTRING3("TSY: CMmCustomMessHandler::MapSw1Sw2ToAuthenticateResult. sw1: 0x%x sw2: 0x%x\n", sw1, sw2 );
OstTraceExt2( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_MAPSW1SW2TOAUTHENTICATERESULT, "CMmCustomMessHandler::MapSw1Sw2ToAuthenticateResult. sw1: 0x%x sw2: 0x%x", sw1, sw2 );

    TUint8 ret( KUnknownCommandEnding );

    switch( sw1 )
        {
        case 0x90:
            {
            if( 0x00 == sw2 )
                {
                ret = KNormalCommandEnding;
                }
            break;
            }
        case 0x62:
            {
            if( 0x00 == sw2 )
                {
                ret = KNormalCommandEnding;
                }
            else if( 0xF1 == sw2 ||
                     0xF2 == sw2 )
                {
                ret = KWarningMoreDataAvailable;
                }
            else if( 0xF3 == sw2 )
                {
                ret = KWarningAuthRespAvailable;
                }
            break;
            }
        case 0x91:
            {
            ret = KNormalCommandEnding;
            break;
            }
        case 0x98:
            {
            if( 0x62 == sw2 )
                {
                ret = KAppAuthErrorIncorrectMac;
                }
            else if( 0x65 == sw2 )
                {
                ret = KAppErrorAuthMbmsKeyFresh;
                }
            else if( 0x66 == sw2 )
                {
                ret = KAppErrorAuthMbmsOutOfMemMsk;
                }
            else if( 0x67 == sw2 )
                {
                ret = KAppErrorAuthMbmsOutOfMemMuk;
                }
            break;
            }
        case 0x69:
            {
            if( 0x82 == sw2 )
                {
                ret = KCmdNotAllowedSecurityStatusNotSatisfied;
                }
            else if( 0x85 == sw2 )
                {
                ret = KCmdNotAllowedConditionsNotSatisfied;
                }
            break;
            }
        case 0x6A:
            {
            if( 0x88 == sw2 )
                {
                ret = KWrongParametersDataNotFound;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ValidateReceivedAuthenticateApdu
// Validates received apdu
// -----------------------------------------------------------------------------
//
TUint8 CMmCustomMessHandler::ValidateReceivedAuthenticateApdu(
    TInt aTraId,
    const TDesC8& aApdu )
    {
    TUint8 ret( KApduOk );

    switch( aTraId )
        {
        case ETrIdEEapSimAuthenticate:
            {
            ret = ValidateGsmSecurityContextApduResp( aApdu );
            break;
            }
        case ETrIdEEapAkaAuthenticate:
        case ETrIdEEapAkaAuthenticateIms:
            {
            ret = Validate3GSecurityContextApduResp( aApdu );
            break;
            }
        case ETrIdEGbaBootstrap:
            {
            ret = ValidateGBABootstrappingApduResp( aApdu );
            break;
            }
       case ETrIdEGbaNafDerivation:
            {
            ret = ValidateGBANafDerivationApduResp( aApdu );
            break;
            }
        case ETrIdRunGsmAlgorithmSim:
        case ETrIdRunGsmAlgorithmAka:
        case ETrIdRunGsmAlgorithmIms:
            {
            ret = ValidateRunGsmAlgorithmApduResp( aApdu );
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::ValidateReceivedAuthenticateApdu: unknown APDU\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_VALIDATERECEIVEDAUTHENTICATEAPDU, "CMmCustomMessHandler::ValidateReceivedAuthenticateApdu: unknown APDU" );
            ret = KApduNok;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ValidateGsmSecurityContextApduResp
// Validates received apdu
// -----------------------------------------------------------------------------
//
TUint8 CMmCustomMessHandler::ValidateGsmSecurityContextApduResp( const TDesC8& aApdu )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ValidateGsmSecurityContextApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_VALIDATEGSMSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::ValidateGsmSecurityContextApduResp" );
    TUint8 ret( KApduNok );

    // apdu len is len of aApdu - 2 (because of
    // aApdu contains sw1 and sw2)
    TUint apduLen( aApdu.Length() - 2 );
    TUint32 index( 0 );

    index += aApdu[index] + 1;
    TUint8 lenOfKc = aApdu[index++];

    if( KLenOfKc == lenOfKc && index + lenOfKc == apduLen )
        {
        ret = KApduOk;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::Validate3GSecurityContextApduResp
// Validates received apdu
// -----------------------------------------------------------------------------
//
TUint8 CMmCustomMessHandler::Validate3GSecurityContextApduResp( const TDesC8& aApdu )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp" );

    TUint8 ret( KApduOk );

    // Let's calculate total APDU data len in aApdu. Length is
    // decremented by 2 because of aApdu contains also sw1 and sw2
    // and these are total 2 bytes long.
    TUint8 apduLen( aApdu.Length() - 2 );
    TUint32 index( 0 );
    TUint8 tag( aApdu[index++] );

    if( KSuccessfull3GAuthTag == tag  )
        {
        // check the len of RES
        if( KMinLenOfRes > aApdu[index] || KMaxLenOfRes < aApdu[index] )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid RES\n" );
OstTrace0( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid RES" );
            ret = KApduNok;
            }
        else
            {
            // move pointer to field length of CK
            index += aApdu[index] + 1;
            }

        // check the len of CK
        if( KApduNok == ret ||
            KLenOfCk != aApdu[index] )
            {
            if( KApduNok != ret )
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid CK\n" );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid CK" );
                ret = KApduNok;
                }
            }
        else
            {
            // move pointer to field length of IK
            index += aApdu[index] + 1;
            }

        // check the len of IK
        if( KApduNok == ret ||
            KLenOfIk != aApdu[index] )
            {
            if( KApduNok != ret )
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid IK\n" );
OstTrace0( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid IK" );
                ret = KApduNok;
                }
            }
        else
            {
            // move pointer to end of IK
            index += aApdu[index];
            }

        // check the len of Kc if present.
        // This parameter is only present
        // when service nro 27 is available
        if( KApduOk == ret && index < apduLen - 1 )
            {
            // move pointer to field length of Kc
            index++;

            // check the len of Kc
            if( KLenOfKc != aApdu[index] )
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid Kc\n" );
OstTrace0( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid Kc" );
                ret = KApduNok;
                }
            else
                {
                // move pointer to end of Kc
                index += aApdu[index];
                }
            }

        // let's check that pointer doesn't go over
        // aApdu's limits.
        if( KApduOk == ret && index > apduLen - 1 )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: error, APDU buffer overflow\n" );
OstTrace0( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: error, APDU buffer overflow" );
            ret = KApduNok;
            }
        }

    else if(KSyncFailureTag == tag)
        {
        // check the len of AUTS
        if( KLenOfAuts != aApdu[index])
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid AUTS\n" );
OstTrace0( TRACE_NORMAL, DUP6_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: invalid AUTS" );
            ret = KApduNok;
            }
        else
            {
            // move pointer to end of AUTS
            index += aApdu[index];
            }

        // let's check that pointer doesn't go over
        // aApdu's limits.
        if( KApduOk == ret && index > apduLen - 1 )
            {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: error, APDU buffer overflow\n" );
OstTrace0( TRACE_NORMAL, DUP7_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: error, APDU buffer overflow" );
            ret = KApduNok;
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmCustomMessHandler::Validate3GSecurityContextApduResp: error, unknown tag\n" );
OstTrace0( TRACE_NORMAL, DUP8_CMMCUSTOMMESSHANDLER_VALIDATE3GSECURITYCONTEXTAPDURESP, "CMmCustomMessHandler::Validate3GSecurityContextApduResp: error, unknown tag" );
        ret = KApduNok;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ValidateGBABootstrappingApduResp
// Validates received apdu
// -----------------------------------------------------------------------------
//
TUint8 CMmCustomMessHandler::ValidateGBABootstrappingApduResp( const TDesC8& aApdu )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ValidateGBABootstrappingApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_VALIDATEGBABOOTSTRAPPINGAPDURESP, "CMmCustomMessHandler::ValidateGBABootstrappingApduResp" );

    TUint8 ret( KApduNok );

    // Let's calculate total APDU data len in aApdu. Length is
    // decremented by 2 because of aApdu contains also sw1 and sw2
    // and these are total 2 bytes long.
    TUint8 apduLen( aApdu.Length() - 2 );
    TUint32 index( 0 );
    TUint8 tag( aApdu[index++] );

    if( apduLen > 1 )
        {
        if( KSuccessfull3GAuthTag == tag ||
            KSyncFailureTag == tag  )
            {
            if( apduLen >= aApdu[index] )
                {
                ret = KApduOk;
                }
            }
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ValidateGBANafDerivationApduResp
// Validates received apdu
// -----------------------------------------------------------------------------
//
TUint8 CMmCustomMessHandler::ValidateGBANafDerivationApduResp( const TDesC8& aApdu )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ValidateGBANafDerivationApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_VALIDATEGBANAFDERIVATIONAPDURESP, "CMmCustomMessHandler::ValidateGBANafDerivationApduResp" );

    TUint8 ret = KApduNok;

    // Let's calculate total APDU data len in aApdu. Length is
    // decremented by 2 because of aApdu contains also sw1 and sw2
    // and these are total 2 bytes long.
    TUint8 apduLen( aApdu.Length() - 2 );
    TUint32 index( 0 );
    TUint8 tag( aApdu[index++] );

    if( apduLen > 1 )
        {
        if( KSuccessfull3GAuthTag == tag )
            {
            // response apdu contains only parameter Ks Ext NAF
            // so, let's check that apdu len contains parameter
            // correctly. So apduLen should be len of Ks Ext NAF
            // + 2 ("Successful GBA operation" field +
            // Length of Ks_ext_NAF field)
            if( apduLen == aApdu[index] + 2 )
                {
                ret = KApduOk;
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmCustomMessHandler::ValidateRunGsmAlgorithmApduResp
// Validates received apdu
// -----------------------------------------------------------------------------
//
TUint8 CMmCustomMessHandler::ValidateRunGsmAlgorithmApduResp( const TDesC8& aApdu )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::ValidateRunGsmAlgorithmApduResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_VALIDATERUNGSMALGORITHMAPDURESP, "CMmCustomMessHandler::ValidateRunGsmAlgorithmApduResp" );

    TUint8 ret( KApduNok );

    if( aApdu.Length() == KRunGsmAlgorithmRespLen + 2 )
        {
        ret = KApduOk;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::FindTlvObject
// Finds TLV object
// -----------------------------------------------------------------------------
//
TBool CMmCustomMessHandler::FindTlvObject(
    TUint8 aTlvTag,
    const TDesC8& aBerTlv,
    TDes8& aTlvObject )
    {
TFLOGSTRING2("TSY: CMmCustomMessHandler::FindTlvObject (tag: 0x%x)\n", aTlvTag );
OstTrace1( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_FINDTLVOBJECT, "CMmCustomMessHandler::FindTlvObject (tag: 0x%x)", aTlvTag );

    TBool ret( EFalse );
    TUint32 index( 0 );
    TUint8 tag( 0 );
    TUint32 tlvLength( 0 );

    while( index + 2 < aBerTlv.Length() )
        {
        tag = aBerTlv[index++];
        tlvLength = aBerTlv[index++];

        // let's check if len of tlv object is coded
        // with one or two bytes
        if( 0x81 == tlvLength ||
            0x82 == tlvLength )
            {
            TUint8 byteCount( tlvLength & 0x7F );
            TUint8 ind( 0 );
            tlvLength = 0;
            do
                {
                tlvLength = tlvLength << 8;
                tlvLength = tlvLength + aBerTlv[index++];
                ind++;
                }
            while ( ind < byteCount );
            }

        if( tag == aTlvTag )
            {
            if( index + tlvLength <= aBerTlv.Length() )
                {
                if( tlvLength <= aTlvObject.MaxSize() )
                    {
                    // tlv object found
TFLOGSTRING2("TSY: CMmCustomMessHandler::FindTlvObject: tag: 0x%x found\n", aTlvTag );
OstTrace1( TRACE_NORMAL, DUP1_CMMCUSTOMMESSHANDLER_FINDTLVOBJECT, "CMmCustomMessHandler::FindTlvObject: tag: 0x%x found", aTlvTag );
                    aTlvObject.Copy( aBerTlv.Mid( index , tlvLength ) );
                    ret = ETrue;
                    }
                else
                    {
TFLOGSTRING3("TSY: CMmCustomMessHandler::FindTlvObject: length of aTlvObject is not enough, needed: %d max size: %d \n", tlvLength, aTlvObject.MaxSize() );
OstTrace0( TRACE_NORMAL, DUP2_CMMCUSTOMMESSHANDLER_FINDTLVOBJECT, "CMmCustomMessHandler::FindTlvObject: length of aTlvObject is not enough" );
OstTrace1( TRACE_NORMAL, DUP3_CMMCUSTOMMESSHANDLER_FINDTLVOBJECT, "CMmCustomMessHandler::FindTlvObject: needed: %d", tlvLength );
OstTrace1( TRACE_NORMAL, DUP4_CMMCUSTOMMESSHANDLER_FINDTLVOBJECT, "CMmCustomMessHandler::FindTlvObject: max size: %d", aTlvObject.MaxSize() );
                    }
                }
            else
                {
TFLOGSTRING("TSY: CMmCustomMessHandler::FindTlvObject: buffer overflow \n" );
OstTrace0( TRACE_NORMAL, DUP5_CMMCUSTOMMESSHANDLER_FINDTLVOBJECT, "CMmCustomMessHandler::FindTlvObject: buffer overflow" );
                }
            break;
            }
        else
            {
            index += tlvLength;
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::DeriveCkFromKc
// derives Ck from Kc
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::DeriveCkFromKc(
    TDes8& aCk,
    const TDesC8& aKc )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::DeriveCkFromKc.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_DERIVECKFROMKC, "CMmCustomMessHandler::DeriveCkFromKc" );

    /* This operation calculates the UMTS Ciphering Key (CK) from the GSM
       Ciphering Key (Kc). This is done by using the c4 algorithm defined in
       3GPP TS 33.102 v3.8.0 (Release 1999).

       c4: CK = Kc || Kc;
       where || means concatination.

       The resulting CK is as follows:
       -----------------------------------------------------------------
       |kc1|kc2|kc3|kc4|kc5|kc6|kc7|kc8|kc1|kc2|kc3|kc4|kc5|kc6|kc7|kc8|
       ----------------------------------------------------------------- */
    aCk.Copy( aKc );
    aCk.Append( aKc );
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::DeriveIkFromKc
// derives Ck from Kc
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::DeriveIkFromKc(
    TDes8& aIk,
    const TDesC8& aKc )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::DeriveIkFromKc.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_DERIVEIKFROMKC, "CMmCustomMessHandler::DeriveIkFromKc" );

    /* This operation derives the UMTS Integrity Key (IK) from the GSM
       Ciphering Key (Kc). This is done by using the c5 algorithm defined in
       3GPP TS 33.102 v3.8.0 (release 1999).

       c5: IK = Kc1 xor Kc2 || Kc || Kc1 xor Kc2;
       where Kc1 and Kc2 are both 4 bytes long and || means concatination.

       |--------------> Kc 8 bytes <-----------|
       -----------------------------------------
       | B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8 |
       -----------------------------------------
       |--> Kc1,4 bytes <--|--> Kc2,4 bytes <--|

       Kx = Kc1 xor Kc2

       The resulting IK is as follows:

       |-----------------------> IK, 16 bytes <------------------------|
       -----------------------------------------------------------------
       |Kx1|Kx2|Kx3|Kx4|Kc1|Kc2|Kc3|Kc4|Kc5|Kc6|Kc7|Kc8|Kx1|Kx2|Kx3|Kx4|
       -----------------------------------------------------------------
       |----> Kx <-----|------------> Kc <-------------|-----> Kx <----| */

    TBuf8<KLenOfKc> kc1;
    TBuf8<KLenOfKc> kc2;
    TBuf8<KLenOfKc> kx;

    kc1.Copy( aKc.Mid( 0, 4 ) );
    kc2.Copy( aKc.Mid( 4, 4 ) );

    for ( TUint8 i = 0; i < 4; i++ )
        {
        kx.Append( kc1[i] ^ kc2[i] );
        }

    aIk.Append( kx );
    aIk.Append( aKc );
    aIk.Append( kx );
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::DeriveIkFromKc
// derives Ck from Kc
// -----------------------------------------------------------------------------
//
TInt CMmCustomMessHandler::UiccHandleImsAuthentication(
    const CMmDataPackage& aDataPackage )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccHandleImsAuthentication.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCHANDLEIMSAUTHENTICATION, "CMmCustomMessHandler::UiccHandleImsAuthentication" );
    TInt ret( KErrNone );

    // IMS authentication:
    // If card type is ICC, we just send the RUN GSM ALGORITHM to the ICC
    // If card type is UICC, we check is ISIM application activated:
    //     * If ISIM application is activated, we send 3G security context
    //     to the ISIM application.
    //     * If ISIM application is NOT activated, we try to activate it.
    //         * If actication succeeded, we send 3G security context to the
    //         ISIM application
    //         * If activation fails, we send 3G security context to the USIM
    //         application

    RMobilePhone::TImsAuthenticateDataV5 authenticationData;
    aDataPackage.UnPackData( authenticationData );

    TUiccSendApdu params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.serviceType = UICC_APPL_APDU_SEND;
    params.fileId = UICC_EF_ID_NOT_PRESENT;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    TUint8 cardType( iMmUiccMessHandler->GetCardType() );

    if( UICC_CARD_TYPE_ICC == cardType )
        {
        // no need to activate ISIM application,
        // let's just send the authentication APDU
        UiccCreateRunGsmAlgorithmApdu(
            params,
            authenticationData.iRAND,
            ETrIdRunGsmAlgorithmIms );

        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    else if( UICC_CARD_TYPE_UICC == cardType )
        {
        // let's store RAND and AUTN while we try to acticate
        // ISIM application
        RMobilePhone::TImsAuthenticateDataV5 authenticationData;
        aDataPackage.UnPackData( authenticationData );

        if( iMmUiccMessHandler->IsIsimApplicationFound() )
            {
            // let's check is ISIM application already tried to activate
            if( iIsimApplActivated )
                {
                UiccCreate3GSecurityContextApdu(
                    params,
                    authenticationData.iRAND,
                    authenticationData.iAUTN,
                    ETrIdEEapAkaAuthenticateIms );

                ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params, UICC_APPL_TYPE_UICC_ISIM );
                }
            else
                {
                iRandBuf.Zero();
                iAutnBuf.Zero();

                iRandBuf.Append( authenticationData.iRAND );
                iAutnBuf.Append( authenticationData.iAUTN );

                TUiccParamsBase activationParams;
                activationParams.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
                activationParams.trId = ETrIdActivateIsimApplication;

                ret = iMmUiccMessHandler->CreateUiccApplicationReq(
                          activationParams,
                          UICC_APPL_HOST_ACTIVATE,
                          UICC_APPL_TYPE_UICC_ISIM );
                }
            }
        else
            {
            // no ISim application, let's send authenticate apdu to the Uicc application
            UiccCreate3GSecurityContextApdu(
                    params,
                    authenticationData.iRAND,
                    authenticationData.iAUTN,
                    ETrIdEEapAkaAuthenticateIms );

            ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
            }
        }
    else
        {
        ret = KErrGeneral;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmCustomMessHandler::UiccHandleIsimActivationResp
// handles repsonse for ISim application activation
// -----------------------------------------------------------------------------
//
void CMmCustomMessHandler::UiccHandleIsimActivationResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmCustomMessHandler::UiccHandleIsimActivationResp.\n" );
OstTrace0( TRACE_NORMAL, CMMCUSTOMMESSHANDLER_UICCHANDLEISIMACTIVATIONRESP, "CMmCustomMessHandler::UiccHandleIsimActivationResp" );

    TUiccSendApdu params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.serviceType = UICC_APPL_APDU_SEND;
    params.fileId = UICC_EF_ID_NOT_PRESENT;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    UiccCreate3GSecurityContextApdu(
        params,
        iRandBuf,
        iAutnBuf,
        ETrIdEEapAkaAuthenticateIms );

    if( UICC_STATUS_OK == aStatus )
        {
        iIsimApplActivated = ETrue;

        // ISim application activated successfully, let's send authentication apdu
        // to the ISim application
        iMmUiccMessHandler->CreateUiccApplCmdReq( params, UICC_APPL_TYPE_UICC_ISIM );
        }
    else
        {
        // ISim application activation fails, le'ts send authentication apdu
        // to the USim application
        iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
    //None

// End of File
