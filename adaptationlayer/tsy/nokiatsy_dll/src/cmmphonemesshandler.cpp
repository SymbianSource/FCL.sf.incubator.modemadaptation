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
#include "cmmmessagerouter.h"
#include "cmmphonemesshandler.h"
#include "cmmphonetsender.h"
#include "cmmsmsmesshandler.h"
#include "cmmstaticutility.h"
#include "cmmsupplservmesshandler.h"
#include "cmmnetmesshandler.h"
#include "cmmnetoperatornamehandler.h"
#include "tsylogger.h"
#include "cmmphonebookoperationinit.h"

#include <call_modemisi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <csdisi.h>
#include <gpdsisi.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <gssisi.h>
#include <infoisi.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
#include <mtcisi.h>
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <mceisi.h>
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <net_modemisi.h>
#include <smsisi.h>
#include <ss_wmisi.h>
#include <tisi.h>
#include <uiccisi.h>
#include <nsisi.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonemesshandlerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS

const TUint8 KPhoneTransId = 6;
const TUint8 KServiceProviderSize = 36;
const TUint8 KSpnFileSize = 16;

const TUint8 KServiceAcl( 35 );
const TUint8 KAclStateMask( 4 );
const TUint8 KNumOfApnsIndex( 0 );
const TUint8 KApnDataIndex( 1 );
const TUint8 KCpsLength( 18 );
const TUint8 KSpnLength( 17 );
const TUint8 KLineInfoLength( 1 );
const TUint8 KAclStatusLength( 1 );

const TUint8 KImsiSize = 9;
const TUint8 KUnusedNibble = 0x0F;
const TUint8 KImsiStringByteCount = 0;
const TUint8 KFirstImsiDigit = 1;
const TUint8 KSecondImsiDigit = 2;
const TUint8 KNibbleMask = 0x0f;
const TUint8 KNibbleShift = 4;

// ------------------------------------------------------
// --- Alternate Line Service (ALS)-related constants ---
// ------------------------------------------------------
// Consts for mapping the Als Line values, as used in SIMSON's
// SIM_SERV_DYNAMIC_FLAGS_STR structure.
const TUint8 KAlsAuxiliaryLine = 0x00;
const TUint8 KAlsPrimaryLine = 0x01;

// ----------------------------------------------
// --- (U)ICC Service Table-related constants ---
// ----------------------------------------------

const TUint8  KUiccCspCallOfferingCode          ( 0x01 );
const TUint8  KUiccCspCallRestrictionCode       ( 0x02 );
const TUint8  KUiccCspOtherSupplServCode        ( 0x03 );
const TUint8  KUiccCspCallCompletionCode        ( 0x04 );
const TUint8  KUiccCspTeleServicesCode          ( 0x05 );
const TUint8  KUiccCspCphsTeleServicesCode      ( 0x06 );
const TUint8  KUiccCspCphsFeaturesCode          ( 0x07 );
const TUint8  KUiccCspNumberIdentifCode         ( 0x08 );
const TUint8  KUiccCspValueAddedServicesCode    ( 0xC0 );

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// ==================== LOCAL FUNCTIONS =====================================
    //None

// ================= MEMBER FUNCTIONS =======================================

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::CMmPhoneMessHandler
// C++ constructor.
// --------------------------------------------------------------------------
//
CMmPhoneMessHandler::CMmPhoneMessHandler()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler constructed.");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_CMMPHONEMESSHANDLER_TD, "CMmPhoneMessHandler::CMmPhoneMessHandler" );

    iACLIsProgress = EFalse;
    iAPNList = NULL;
    iAPNReadOrDeleteIndex = 0;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::NewL
// Creates a new PhoneMessageHandler object instance.
// --------------------------------------------------------------------------
//
CMmPhoneMessHandler* CMmPhoneMessHandler::NewL
        (
        CMmPhoNetSender* aPhoNetSender,
        CMmPhoNetReceiver* aPhoNetReceiver,
        CMmMessageRouter* aMessageRouter,
        CMmSupplServMessHandler* aSupplServMessHandler,
        CMmUiccMessHandler* aUiccMessHandler
        )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::NewL");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_NEWL_TD, "CMmPhoneMessHandler::NewL" );

    CMmPhoneMessHandler* phoneMessHandler =
        new ( ELeave ) CMmPhoneMessHandler();

    CleanupStack::PushL( phoneMessHandler );
    phoneMessHandler->iPhoNetSender = aPhoNetSender;
    phoneMessHandler->ConstructL(
        aMessageRouter,
        aPhoNetSender,
        aSupplServMessHandler,
        aUiccMessHandler );

    aPhoNetReceiver->RegisterL(
        phoneMessHandler,
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        PN_INFO,
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        PN_MODEM_INFO,
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        INFO_SERIAL_NUMBER_READ_RESP );

    aPhoNetReceiver->RegisterL(
        phoneMessHandler,
        PN_UICC,
        UICC_REFRESH_RESP );

    aPhoNetReceiver->RegisterL(
        phoneMessHandler,
        PN_UICC,
        UICC_REFRESH_IND );

    CleanupStack::Pop( phoneMessHandler );

    return phoneMessHandler;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::ConstructL
// Initialises object attributes.
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::ConstructL
        (
        CMmMessageRouter* aMessageRouter,
        CMmPhoNetSender* aPhoNetSender,
        CMmSupplServMessHandler* aSupplServMessHandler,
        CMmUiccMessHandler* aUiccMessHandler
        )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_CONSTRUCTL_TD, "CMmPhoneMessHandler::ConstructL" );

    iPhoNetSender = aPhoNetSender;
    iSupplServMessHandler = aSupplServMessHandler;
    iMmUiccMessHandler = aUiccMessHandler;
    iMessageRouter = aMessageRouter;

    iSSTFileData = NULL;
    iStoreInfo = NULL;
    iLocIndex = 0;

    iCommonTSYRefreshPending = EFalse;
    iRefreshError = EFalse;
    iCompleteRefresfDone = EFalse;
    iInternalRefreshFiles = 0;

    SubscribeEventsFromPhoNet();

    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::~CMmPhoneMessHandler
// C++ destructor.
// --------------------------------------------------------------------------
//
CMmPhoneMessHandler::~CMmPhoneMessHandler()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler destructed.");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_CMMPHONEMESSHANDLER_TD, "CMmPhoneMessHandler::~CMmPhoneMessHandler" );

    if( iStoreInfo )
        {
        delete iStoreInfo;
        }

    if( iSSTFileData )
        {
        delete iSSTFileData;
        }

    if( iAPNList )
        {
        iAPNList->Reset();
        delete iAPNList;
        }
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::SubcribeEventsFromPhonet
// Subscribes indications from PhoNet. The wanted indications
// are first added to an array and the array is then passed to the PhoNet.
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::SubscribeEventsFromPhoNet()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::SubscribeEventsFromPhoNet");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_SUBSCRIBEEVENTSFROMPHONET_TD, "CMmPhoneMessHandler::SubscribeEventsFromPhoNet" );

    // Cross check requests with PhonetReceiver
    TUint8 requestArray[] = {

        // CALL SERVER INDICATIONS
            // PhonetReceiver - PhoneDispatchList
            PN_MODEM_CALL, CALL_MODEM_DTMF_STATUS_IND,
            PN_MODEM_CALL, CALL_MODEM_DTMF_TONE_IND,

            // PhonetReceiver - LineDispatchList
            PN_MODEM_CALL, CALL_MODEM_COMING_IND,

            // PhonetReceiver - CallDispatchList
            PN_MODEM_CALL, CALL_MODEM_CONTROL_IND,
            PN_MODEM_CALL, CALL_MODEM_MO_ALERT_IND,
            PN_MODEM_CALL, CALL_MODEM_RELEASE_IND,
            PN_MODEM_CALL, CALL_MODEM_STATUS_IND, // used also in SimAtkTSY
            PN_MODEM_CALL, CALL_MODEM_TERMINATED_IND,
            PN_MODEM_CALL, CALL_MODEM_NOTIFICATION_IND,
            PN_MODEM_CALL, CALL_MODEM_SERVICE_DENIED_IND,
            PN_MODEM_CALL, CALL_MODEM_MESSAGE_IND, // Used also in simatktsy
            PN_MODEM_CALL, CALL_MODEM_PRESENT_IND, // Used also in simatktsy
            PN_MODEM_CALL, CALL_MODEM_RESOURCE_IND, // Used also in simatktsy
            PN_MODEM_CALL, CALL_MODEM_RESOURCE_CONF_IND, // Used only in simatktsy

            //CSD SERVER INDICATIONS
            PN_CSD, CSD_VIDEO_CALL_STATUS_IND,
            PN_CSD, CSD_MULTIMEDIA_DATA_RATE_IND,

        // NET SERVER INDICATIONS
            PN_MODEM_NETWORK, NET_MODEM_REG_STATUS_IND,
            PN_MODEM_NETWORK, NET_NITZ_NAME_IND,
            PN_MODEM_NETWORK, NET_CELL_INFO_IND, // used only in SimAtkTSY
            PN_MODEM_NETWORK, NET_RSSI_IND,
            PN_MODEM_NETWORK, NET_RAT_IND, // used also in SimAtkTSY
            PN_MODEM_NETWORK, NET_CIPHERING_IND,
            PN_MODEM_NETWORK, NET_TIME_IND, // used also in SimAtkTSY
            PN_MODEM_NETWORK, NET_RADIO_INFO_IND,

        // SS SERVER INDICATIONS
            PN_SS, SS_SERVICE_COMPLETED_IND,
            PN_SS, SS_STATUS_IND, // used also in SimAtkTSY
            PN_SS, SS_RESOURCE_CONTROL_IND, // used in SimAtkTSY
            PN_SS, SS_RESOURCE_CONF_IND, // used in SimAtkTSY

        // USSD
            PN_SS, SS_GSM_USSD_RECEIVE_IND,

        // SMS SERVER INDICATIONS
            PN_SMS, SMS_RECEIVED_SIM_MSG_IND, // used only in SimAtkTSY
            PN_SMS, SMS_RESOURCE_IND, // used only in SimAtkTSY
            PN_SMS, SMS_RECEIVED_MSG_IND,
            PN_SMS, SMS_CB_ROUTING_IND,
            PN_SMS, SMS_CB_SIM_ROUTING_IND, // used only in SimAtkTSY
            PN_SMS, SMS_RESOURCE_CONF_IND,

        // UICC SERVER INDICATIONS
            PN_UICC, UICC_CARD_IND,
            PN_UICC, UICC_IND,
            PN_UICC, UICC_REFRESH_IND,
            PN_UICC, UICC_CAT_IND,
            PN_UICC, UICC_APDU_RESET_IND,

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        // MTC SERVER INDICATIONS
            PN_MTC, MTC_STATE_INFO_IND,
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        // MCE SERVER INDICATIONS
            PN_MODEM_MCE, MCE_MODEM_STATE_IND,
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        // GPDS SERVER INDICATIONS
            PN_GPDS, GPDS_CONTEXT_ID_CREATE_IND,
            PN_GPDS, GPDS_CONTEXT_ID_DELETE_IND,
            PN_GPDS, GPDS_CONTEXT_ACTIVATE_IND,
            PN_GPDS, GPDS_CONTEXT_DEACTIVATE_IND,
            PN_GPDS, GPDS_CONTEXT_NWI_ACT_REQUEST_IND,
            PN_GPDS, GPDS_ATTACH_IND,
            PN_GPDS, GPDS_DETACH_IND,
            PN_GPDS, GPDS_TRANSFER_STATUS_IND,
            PN_GPDS, GPDS_CONTEXT_ACTIVATE_FAIL_IND,
            PN_GPDS, GPDS_CONTEXT_STATUS_IND,
            PN_GPDS, GPDS_CONTEXT_ACTIVATING_IND,
            PN_GPDS, GPDS_CONTEXT_DEACTIVATING_IND,
            PN_GPDS, GPDS_CONFIGURATION_INFO_IND,
            PN_GPDS, GPDS_CONTEXT_MODIFY_IND,
            PN_GPDS, GPDS_RADIO_ACTIVITY_IND,
            PN_GPDS, GPDS_RESOURCE_CONTROL_IND,
            PN_GPDS, GPDS_RESOURCE_CONF_IND,

        // NVD SERVER INDICATIONS

        // PIPE SERVER INDICATIONS

        // GSS SERVER INDICATIONS
            PN_GSS, GSS_HSXPA_USER_SETTING_IND,

        // NAME SERVICE INDICATIONS
            PN_NAMESERVICE, PNS_NAME_ADD_IND,
            PN_NAMESERVICE, PNS_NAME_REMOVE_IND,
        };

    TInt ret = iPhoNetSender->SubscribeEvents(
        TPtr8( &requestArray[0],
            sizeof( requestArray ),
            sizeof( requestArray ) ) );

    return ret;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::ExtFuncL
// Dispatches Etel requests to DOS level handlers
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::ExtFuncL
        (
        TInt aIpc,
        const CMmDataPackage* aDataPackage
        )
    {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::ExtFuncL. Licensee specific implemtantion. IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL,  CMMPHONEMESSHANDLER_EXTFUNCL_TD, "CMmPhoneMessHandler::ExtFuncL;Licensee specific implemtantion aIpc=%d", aIpc );

    TInt ret( KErrNone );
    TUint8 transId( KPhoneTransId );

    switch ( aIpc )
        {
        case EMobilePhoneGetALSLine:
            {
            ret = UiccReadDynamicFlagsReq();
            break;
            }
        case EMobilePhoneSetALSLine:
            {
            TUint8 alsLine;
            if ( aDataPackage )
                {
                aDataPackage->UnPackData( alsLine );
                ret = UiccWriteDynamicFlagsReq( alsLine );
                }
            else
                {
                ret = KErrArgument;
                }
            break;
            }
        case EMobilePhoneGetCustomerServiceProfile:
            {
            ret = UiccCspFileReq();
            break;
            }
        case EMobilePhoneGetPhoneId:
            {
            // no packed parameters
            // read IMEI
            ret = InfoSerialNumberReadReq( transId, INFO_SN_IMEI_PLAIN );
            break;
            }
        case EMobilePhoneGetSubscriberId:
            {
            // read IMSI
            ret = UiccImsiReq();
            break;
            }
        case EMobilePhoneGetServiceProviderName:
            {
            ret = UiccReadServiceProviderName();
            break;
            }
        case EMmTsySimRefreshDoneIPC:
            {
            iCommonTSYRefreshPending = EFalse;
            TBool readError;
            aDataPackage->UnPackData( readError );

            if ( ! readError )
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ExtFuncL: commontsy refresh successfully completed ==> REFRESH_DONE");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_EXTFUNCL_TD, "CMmPhoneMessHandler::ExtFuncL: commontsy refresh successfully completed ==> REFRESH_DONE" );
                ret = UiccRefreshReq( UICC_REFRESH_DONE );
                }
            else
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ExtFuncL: commontsy refresh completed with an error ==> REFRESH_DONE");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPHONEMESSHANDLER_EXTFUNCL_TD, "CMmPhoneMessHandler::ExtFuncL: commontsy refresh completed with an error ==> REFRESH_DONE" );
                ret = UiccRefreshReq( UICC_REFRESH_NOT_OK );
                }
            break;
            }

        case EMobilePhoneGetAPNControlListServiceStatus:
            {
            if( !iACLIsProgress )
                {
                iOngoingAclIpc = EMobilePhoneGetAPNControlListServiceStatus;
                iACLIsProgress = ETrue;
                ret = UiccReadEfEstReq();
                }
            else
                {
                ret = KErrInUse;
                }
            break;
            }
        case EMobilePhoneSetAPNControlListServiceStatus:
            {
            if( !iACLIsProgress )
                {
                iOngoingAclIpc = EMobilePhoneSetAPNControlListServiceStatus;
                iACLIsProgress = ETrue;
                aDataPackage->UnPackData( iAclStatus );
                ret = UiccReadEfEstReq();
                }
            else
                {
                ret = KErrInUse;
                }
            break;
            }
        case EMobilePhoneDeleteAPNName:
            {
            if( !iACLIsProgress )
                {
                iACLIsProgress = ETrue;
                iOngoingAclIpc = EMobilePhoneDeleteAPNName;
                aDataPackage->UnPackData( iAPNReadOrDeleteIndex );
                if( !iAPNList )
                    {
                    ret = UiccReadAclReq();
                    }
                else
                    {
                    ret = UiccDeleteApnEntry( iAPNReadOrDeleteIndex );
                    }
                }
            else
                {
                ret = KErrInUse;
                }
            break;
            }
        case EMobilePhoneAppendAPNName:
            {
            if( !iACLIsProgress )
                {
                iACLIsProgress = ETrue;
                iOngoingAclIpc = EMobilePhoneAppendAPNName;
                RMobilePhone::TAPNEntryV3* apnEntryName( NULL );
                aDataPackage->UnPackData( &apnEntryName );
                iAPNDataBuffer =  *apnEntryName;

                if( !iAPNList )
                    {
                    ret = UiccReadAclReq();
                    }
                else
                    {
                    iAPNList->AppendL( iAPNDataBuffer.iApn );
                    ret = UiccWriteEfAclReq();
                    }
                }
            else
                {
                ret = KErrInUse;
                }
            break;
            }
        case EMobilePhoneEnumerateAPNEntries:
            {
            if( !iAPNList )
                {
                if( !iACLIsProgress )
                    {
                    iACLIsProgress = ETrue;
                    iOngoingAclIpc = EMobilePhoneEnumerateAPNEntries;
                    ret = UiccReadAclReq();
                    }
                else
                    {
                    ret = KErrInUse;
                    }
                }
            else
                {
                CompleteEnumerateAPNEntries();
                }
            break;
            }
        case EMobilePhoneGetAPNname:
            {
            aDataPackage->UnPackData( iAPNReadOrDeleteIndex );
            if( !iAPNList )
                {
                if( !iACLIsProgress )
                    {
                    iACLIsProgress = ETrue;
                    iOngoingAclIpc = EMobilePhoneGetAPNname;
                    ret = UiccReadAclReq();
                    }
                else
                    {
                    ret = KErrInUse;
                    }
                }
            else
                {
                CompleteGetAPNName( iAPNReadOrDeleteIndex );
                }
            break;
            }

        default:
            {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL,  DUP7_CMMPHONEMESSHANDLER_EXTFUNCL_TD, "CMmPhoneMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
            ret = KErrNotSupported;
            break;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::ReceiveMessageL
// Called by extension message handler when a common ISI
// message has been received.
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::ReceiveMessageL
        (
        const TIsiReceiveC &aIsiMessage
        )
    {
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

TFLOGSTRING3("TSY: CMmPhoneMessHandler::ReceiveMessageL, Resource: %d, Message: %d", resource, messageId );
OstTraceExt2( TRACE_NORMAL,  CMMPHONEMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmPhoneMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch( resource )
        {
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case PN_INFO:
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        case PN_MODEM_INFO:
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            {
            if ( INFO_SERIAL_NUMBER_READ_RESP == messageId )
                {
                InfoSerialNumberReadResp( aIsiMessage );
                }
            else
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ReceiveMessageL, switch resource - case PN_MODEM_INFO, switch messageId - default.\n" );
OstTrace0( TRACE_NORMAL,  DUP4_CMMPHONEMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmPhoneMessHandler::ReceiveMessageL, switch resource - case PN_MODEM_INFO, switch messageId - default" );
                }
            break; // end case PN_MODEM_INFO
            }
        case PN_UICC:
            {
            if ( UICC_REFRESH_IND == messageId )
                {
                UiccRefreshInd( aIsiMessage );
                }
            else if ( UICC_REFRESH_RESP == messageId )
                {
                UiccRefreshResp( aIsiMessage );
                }
            // No else
            break;
            }
       default:
            {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ReceiveMessageL, switch resource - default.\n" );
OstTrace0( TRACE_NORMAL,  DUP5_CMMPHONEMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmPhoneMessHandler::ReceiveMessageL, switch resource - default" );
            break;
            }
        } // End of switch
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::InfoSerialNumberReadReq
// Constructs INFO_SERIAL_NUMBER_READ_REQ ISI message.
// Returns KErrNone on success, other error value on failure.
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::InfoSerialNumberReadReq
        (
        TUint8 aTransactionId,
        TUint8 aTarget // type of requested serial number
        )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::InfoSerialNumberReadReq.");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_INFOSERIALNUMBERREADREQ_TD, "CMmPhoneMessHandler::InfoSerialNumberReadReq" );

    TBuf8<1> data;
    data.Append( aTarget ); // Target identifier ( INFO_SN_IMEI_PLAIN )

    return iPhoNetSender->Send(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        PN_INFO,
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        PN_MODEM_INFO,
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        aTransactionId,
        INFO_SERIAL_NUMBER_READ_REQ,
        data );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::InfoSerialNumberReadResp
// Breaks a INFO_SERIAL_NUMBER_READ_RESP ISI-message.
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::InfoSerialNumberReadResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    TInt err( KErrNotFound );

    TBuf8<KSerialNumberLength> serialData;

    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + INFO_SERIAL_NUMBER_READ_RESP_OFFSET_STATUS ) );

TFLOGSTRING2("TSY: CMmPhoneMessHandler::InfoSerialNumberReadResp, status=0x%02x",status);
OstTraceExt1( TRACE_NORMAL,  CMMPHONEMESSHANDLER_INFOSERIALNUMBERREADRESP_TD, "CMmPhoneMessHandler::InfoSerialNumberReadResp;status=%hhx", status );

    // Get information from possible sub block(s).
    if ( INFO_OK == status )
        {
        TBool subblockFound( EFalse );
        TUint sbSerialNumberStartOffset( 0 );

        // If sub blocks
        if ( 0 != aIsiMessage.Get8bit(
            ISI_HEADER_SIZE +
            INFO_SERIAL_NUMBER_READ_RESP_OFFSET_SUBBLOCKCOUNT ) )
            {
            subblockFound = ETrue;
            }

        if ( subblockFound && ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_INFO_SERIAL_NUMBER_READ_RESP,
            INFO_SB_SN_IMEI_PLAIN,
            EIsiSubBlockTypeId8Len8,
            sbSerialNumberStartOffset ) ) )
            {
            // Read the string length - the zero terminator
            TUint8 strLength( aIsiMessage.Get8bit(
                sbSerialNumberStartOffset +
                INFO_SB_SN_IMEI_PLAIN_OFFSET_STRLEN ) - 1 );

            TUint8 dataOffset(
                sbSerialNumberStartOffset +
                INFO_SB_SN_IMEI_PLAIN_OFFSET_IMEIPLAINU8 );

            serialData.Append( aIsiMessage.GetData(
                dataOffset,
                strLength ).Left( KSerialNumberLength ) );

            err = KErrNone;
            }
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &serialData );
    iMessageRouter->Complete( EMobilePhoneGetPhoneId, &dataPackage, err );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccReadDynamicFlagsReq
// Read dynamic flags
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccReadDynamicFlagsReq()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadDynamicFlagsReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREADDYNAMICFLAGSREQ_TD, "CMmPhoneMessHandler::UiccReadDynamicFlagsReq" );

    return UiccApplCmdReq(
        ETrIdReadDynamicFlags,
        UICC_APPL_READ_TRANSPARENT,
        0,
        0,
        KElemFileDynFlagsOrange,
        UICC_SFI_NOT_PRESENT );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccWriteDynamicFlagsReq
// Write dynamic flags
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccWriteDynamicFlagsReq( TUint8 aInfo )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccWriteDynamicFlagsReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCWRITEDYNAMICFLAGSREQ_TD, "CMmPhoneMessHandler::UiccWriteDynamicFlagsReq" );

    TInt ret( KErrNone );
    TUint8 line( 0 );

    if ( RMobilePhone::EAlternateLinePrimary == aInfo )
        {
        line = KAlsPrimaryLine;
        iAlsLine = RMobilePhone::EAlternateLinePrimary;
        }
    else if ( RMobilePhone::EAlternateLineAuxiliary == aInfo )
        {
        line = KAlsAuxiliaryLine;
        iAlsLine = RMobilePhone::EAlternateLineAuxiliary;
        }
    else
        {
        iAlsLine = RMobilePhone::EAlternateLineUnknown;
        ret = KErrArgument;
        }

    if ( KErrNone == ret )
        {
        TBuf8<KLineInfoLength> fileData;
        fileData.Append( line );
        ret = UiccApplCmdReq(
            ETrIdWriteDynamicFlags,
            UICC_APPL_UPDATE_TRANSPARENT,
            0,
            0,
            KElemFileDynFlagsOrange,
            UICC_SFI_NOT_PRESENT,
            fileData);
        }
    // No else

    return ret;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccReadDynamicFlagsResp
// Complete dynamic flags
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccReadDynamicFlagsResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadDynamicFlagsResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREADDYNAMICFLAGSRESP_TD, "CMmPhoneMessHandler::UiccReadDynamicFlagsResp" );

    RMobilePhone::TMobilePhoneALSLine alsLine
        ( RMobilePhone::EAlternateLineUnknown );

    TInt ret( KErrNone );
    if ( UICC_STATUS_OK == aStatus && ( 0 < aFileData.Length() ) )
        {
        // Dynamic flags byte
        if ( aFileData[0] & 0x1 )
            {
            alsLine = RMobilePhone::EAlternateLinePrimary;
            }
        else
            {
            alsLine = RMobilePhone::EAlternateLineAuxiliary;
            }
        }

    iSupplServMessHandler->SetVoiceCallForwardLine( alsLine );

    CMmDataPackage dataPackage;
    dataPackage.PackData ( &alsLine );
    iMessageRouter->Complete(
        EMobilePhoneGetALSLine,
        &dataPackage,
        ret );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccWriteDynamicFlagsResp
// Complete write dynamic flags
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccWriteDynamicFlagsResp( TInt aStatus )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccWriteDynamicFlagsResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCWRITEDYNAMICFLAGSRESP_TD, "CMmPhoneMessHandler::UiccWriteDynamicFlagsResp" );

    if ( UICC_STATUS_OK == aStatus )
        {
        iSupplServMessHandler->SetVoiceCallForwardLine( iAlsLine );
        iMessageRouter->Complete( EMobilePhoneSetALSLine, KErrNone );
        }
    else
        {
        iMessageRouter->Complete( EMobilePhoneSetALSLine, KErrGeneral );
        }
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccCspFileReq
// Read CSP
// --------------------------------------------------------------------------
TInt CMmPhoneMessHandler::UiccCspFileReq()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccCspFileReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCCSPFILEREQ_TD, "CMmPhoneMessHandler::UiccCspFileReq" );

    return UiccApplCmdReq(
        ETrIdReadCsp,
        UICC_APPL_READ_TRANSPARENT,
        18,
        0,
        KElemFileCustomerServiceProfile,
        UICC_SFI_NOT_PRESENT );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccCspFileResp
// Complete CSP
// --------------------------------------------------------------------------
void CMmPhoneMessHandler::UiccCspFileResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccHandleCallFwdFlagsResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCCSPFILERESP_TD, "CMmPhoneMessHandler::UiccCspFileResp" );

    RMobilePhone::TMobilePhoneCspFileV1 cspFileEtel;

    if ( ( aStatus == UICC_STATUS_OK ) &&
         ( KCpsLength <= aFileData.Length() ) )
        {
        TUint8 i( 0 );
        // Call offering capabilities
        if ( KUiccCspCallOfferingCode == aFileData[i++] )
            {
            cspFileEtel.iCallOfferingServices = aFileData[i++];
            }

        // Call restriction capabilities
        if ( KUiccCspCallRestrictionCode == aFileData[i++] )
            {
            cspFileEtel.iCallRestrictionServices = aFileData[i++];
            }

        // Other supplementary services capabilities
        if ( KUiccCspOtherSupplServCode == aFileData[i++] )
            {
            cspFileEtel.iOtherSuppServices = aFileData[i++];
            }

        // Call completion capabilities
        if ( KUiccCspCallCompletionCode == aFileData[i++] )
            {
            cspFileEtel.iCallCompletionServices = aFileData[i++];
            }

        // Teleservices capabilities
        if ( KUiccCspTeleServicesCode == aFileData[i++] )
            {
            cspFileEtel.iTeleservices = aFileData[i++];
            }

        // CPHS teleservices capabilities
        if ( KUiccCspCphsTeleServicesCode == aFileData[i++] )
            {
            cspFileEtel.iCphsTeleservices = aFileData[i++];
            }

        // CPHS features capabilities
        if ( KUiccCspCphsFeaturesCode == aFileData[i++] )
            {
            cspFileEtel.iCphsFeatures = aFileData[i++];
            }

        // Number identification capabilities
        if ( KUiccCspNumberIdentifCode == aFileData[i++] )
            {
            cspFileEtel.iNumberIdentServices = aFileData[i++];
            }

        // Value added services capabilities
        if ( KUiccCspValueAddedServicesCode == aFileData[i++] )
            {
            cspFileEtel.iValueAddedServices = aFileData[i++];
            }
        } // End of if
    else
        {
        aStatus = KErrNotFound;
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &cspFileEtel );

    iMessageRouter->Complete(
        EMobilePhoneGetCustomerServiceProfile,
        &dataPackage,
        aStatus );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccImsiReq
// Read IMSI
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccImsiReq( )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccImsiReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCIMSIREQ_TD, "CMmPhoneMessHandler::UiccImsiReq" );

    return UiccApplCmdReq(
        ETrIdReadImsi,
        UICC_APPL_READ_TRANSPARENT,
        9,
        0,
        KElemFileImsi,
        7 );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccImsiResp
// Complete IMSI
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccImsiResp( TInt aStatus, const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccImsiResp" );
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCIMSIRESP_TD, "CMmPhoneMessHandler::UiccImsiResp" );

    TInt err( KErrNone );
    TBuf8<RMobilePhone::KIMSISize> imsiData;

    if ( ( UICC_STATUS_OK == aStatus ) &&
         ( KImsiSize == aFileData.Length() ) )
        {
        // 1st digit of the buffer is byte count (see 3GPP TS 51.011
        // 10.3.2 EFIMSI (IMSI)). Get first IMSI number from second byte higher
        // nibbles. Lower part has parity information and it is ignored.
        imsiData.AppendNum(
            ( ( aFileData[KFirstImsiDigit]>> KNibbleShift )&KNibbleMask ),
            EDecimal );

        for( TUint8 i = KSecondImsiDigit;
             i <= aFileData[KImsiStringByteCount];
             i++ )
            {
            // Unused nibbles are set to 'F' so pack only valid digits.
            if ( KUnusedNibble != aFileData[i]&KNibbleMask )
                {
                imsiData.AppendNum( ( ( aFileData[i]&KNibbleMask ) ), EDecimal );
                }
            else
                {
                // Last digit found.
                break;
                }
            if ( KUnusedNibble != ( ( aFileData[i]>>KNibbleShift )&KNibbleMask ) )
                {
                imsiData.AppendNum(
                    ( ( aFileData[i]>>KNibbleShift )&KNibbleMask ),
                    EDecimal );
                }
            else
                {
                // Last digit found.
                break;
                }
            }
        err = KErrNone;
        }
    else
        {
        err = KErrNotFound;
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &imsiData );

    iMessageRouter->Complete(
        EMobilePhoneGetSubscriberId,
        &dataPackage,
        err );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccReadServiceProviderName
// Read service provider name
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccReadServiceProviderName()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadServiceProviderName");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREADSERVICEPROVIDERNAME_TD, "CMmPhoneMessHandler::UiccReadServiceProviderName" );

    return UiccApplCmdReq(
        ETrIdReadServiceProviderName,
        UICC_APPL_READ_TRANSPARENT,
        17,
        0,
        KElemFileServiceProviderName,
        UICC_SFI_NOT_PRESENT );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccReadServiceProviderNameResp
// Complete service provider name
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccReadServiceProviderNameResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadServiceProviderNameResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREADSERVICEPROVIDERNAMERESP_TD, "CMmPhoneMessHandler::UiccReadServiceProviderNameResp" );

    TInt ret( KErrNotFound );
    if ( ( UICC_STATUS_OK == aStatus ) &&
         ( KSpnLength == aFileData.Length() ) )
        {
        // Store data and read SPN display info
        ret = UiccProcessServiceTypeCheck( aFileData );
        }

    if ( ( UICC_STATUS_OK != aStatus ) || ( KErrNone != ret ) )
        {
        // Complete error without data
        iMessageRouter->Complete(
            EMobilePhoneGetServiceProviderName,
            KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CMmPhoneMessHandler::ProcessUiccMsg
// Handles data received from UICC server
// -----------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY: CMmPhoneMessHandler::ProcessUiccMsg, transaction ID: %d, status %d", aTraId, aStatus );
OstTraceExt2( TRACE_NORMAL,  CMMPHONEMESSHANDLER_PROCESSUICCMSG_TD, "CMmPhoneMessHandler::ProcessUiccMsg;aTraId=%d;aStatus=%d", aTraId, aStatus );

switch( aTraId )
        {
        case ETrIdReadImsi:
            {
            UiccImsiResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadServiceProviderName:
            {
            UiccReadServiceProviderNameResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadServiceProviderDisplayInfo:
            {
            UiccProcessSpnNameInfo( aStatus, aFileData );
            break;
            }
        case ETrIdReadCsp:
            {
            UiccCspFileResp( aStatus, aFileData );
            }
        case ETrIdReadDynamicFlags:
            {
            UiccReadDynamicFlagsResp( aStatus, aFileData );
            break;
            }
        case ETrIdWriteDynamicFlags:
            {
            UiccWriteDynamicFlagsResp( aStatus );
            break;
            }
        case ETrIdAclStatusReadEfEst:
            {
            if( EMobilePhoneGetAPNControlListServiceStatus == iOngoingAclIpc )
                {
                UiccGetAclStatusReadEfEstResp( aStatus, aFileData );
                }
            else if( EMobilePhoneSetAPNControlListServiceStatus == iOngoingAclIpc )
                {
                UiccSetAclStatusReadEfEstResp( aStatus, aFileData );
                }
            break;
            }
        case ETrIdAclStatusWriteEfEst:
            {
            UiccSetAclStatusWriteEfEstResp( aStatus );
            break;
            }
        case ETrIdAclReadEfAcl:
            {
            UiccReadAclResp( aStatus, aFileData );
            break;
            }
        case ETrIdAclWriteEfAcl:
            {
            UiccWriteEfAclResp( aStatus );
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_PROCESSUICCMSG_TD, "CMmPhoneMessHandler::ProcessUiccMsg - unknown transaction ID" );
            break;
            }
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccProcessServiceTypeCheck
// Process service type checking response
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccProcessServiceTypeCheck(
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccProcessServiceTypeCheck");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCPROCESSSERVICETYPECHECK_TD, "CMmPhoneMessHandler::UiccProcessServiceTypeCheck" );

    TBuf8<KSpnFileSize> spnBuffer( aFileData.Mid( 1 ) );
    TBuf8<KServiceProviderSize> spnOutputBuffer;
    CMmStaticUtility::ConvertGsmDataToUcs2(
        spnBuffer,
        KSpnFileSize,
        spnOutputBuffer );

    // Get the display condition ( 1st byte )
    const TUint8 displayCondition( aFileData[0] );

    // From TS 31.102:
    // b1=1: display of registered PLMN name required when registered PLMN is
    // either HPLMN or a PLMN in the service provider PLMN list.
    if ( displayCondition & 0x01 )
        {
        iServiceProviderName.iDisplayReq = RMobilePhone::KDisplayPLMNRequired;
        }
    else
        {
        iServiceProviderName.iDisplayReq =
            RMobilePhone::KDisplayPLMNNotRequired;
        }

    // Set iSecondDisplayCondition as not required by default
    iSecondDisplayCondition = RMobilePhone::KDisplaySPNNotRequired;

    // From TS 31.102:
    // b2=0:  Display of the service provider name is required when
    // registered PLMN is neither HPLMN nor a PLMN in the service provider
    // PLMN list
    if ( 0 == ( displayCondition & 0x02 ) )
        {
        iSecondDisplayCondition = RMobilePhone::KDisplaySPNRequired;
        }

    iServiceProviderName.iDisplayReq =
        iServiceProviderName.iDisplayReq + iSecondDisplayCondition;
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccProcessServiceTypeCheck - display condition: %d", (TUint8)iServiceProviderName.iDisplayReq);

    TBuf16<KSpnFileSize> tempBuf;
    TIsiUtility::CopyFromBigEndian( spnOutputBuffer, tempBuf );
    // Copy service provider name
    iServiceProviderName.iSPName.Copy( tempBuf );
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccProcessServiceTypeCheck - SPN Name: %S", &iServiceProviderName.iSPName);


    // We still need to get the PLMN list to complete the information.
    // Read the SIM file EF SPDI ('6FCD')
    return UiccApplCmdReq(
        ETrIdReadServiceProviderDisplayInfo,
        UICC_APPL_READ_TRANSPARENT,
        0,
        0,
        KElemFileServiceProviderDisplayInfo,
        0x1B );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccProcessSpnNameInfo
// Processes SPN name info response
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccProcessSpnNameInfo(
    TInt aStatus,
    const TDesC8& aFileData)
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccProcessSpnNameInfo");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCPROCESSSPNNAMEINFO_TD, "CMmPhoneMessHandler::UiccProcessSpnNameInfo" );

    TInt numOfPlmns( 0 );
    TInt lengthOfDataInBytes( 0 );
    TInt i( 0 ); // Index used of data extracting
    if ( UICC_STATUS_OK == aStatus )
        {
        TInt lengthOfLengthInBytes( 0 );

        // Display info is coded in TLV format in USIM.
        if ( 0xA3 == aFileData[i] )
            {
            i++;
            // If bit8 is set, rest of bits tell the number of bytes used for
            // length
            if ( aFileData[i] & 0x80 )
                {
                lengthOfLengthInBytes = aFileData[i] & 0x7F;
                i += lengthOfLengthInBytes;
                }
            i++;

            if ( 0x80 == aFileData[i] )
                {
                // Check how many bytes are used for length field
                i++;
                if ( 0 ==  ( aFileData[i] & 0x80 ) )
                    {
                    lengthOfDataInBytes = aFileData[i] & 0x7F;
                    numOfPlmns = lengthOfDataInBytes / 3;
                    i++; // Data starting point
                    }
                else
                    {
                    lengthOfLengthInBytes = aFileData[i] & 0x7F;
                    i++; // Skip length tag
                    if ( 1 == lengthOfLengthInBytes )
                        {
                        lengthOfDataInBytes = aFileData[i];
                        numOfPlmns = lengthOfDataInBytes / 3;
                        i++; // Data starting point
                        }
                    else // Length is in two bytes
                        {
                        lengthOfDataInBytes =
                            ( aFileData[i] << 8 ) | aFileData[i+1];
                        numOfPlmns = lengthOfDataInBytes / 3;
                        i += 2; // Data starting point
                        }
                    }
                }
            else // Incorrect tag
                {
                aStatus = KErrGeneral;
                }
            }
        else // Incorrect tag
            {
            aStatus = KErrGeneral;
            }
        }

    // Copy PLMNs and complete
    if ( UICC_STATUS_OK == aStatus )
        {
        if ( 170 < numOfPlmns )
            {
            numOfPlmns = 170;
            }

        TUint16 word( static_cast<TUint16>(
                numOfPlmns << 8 | iSecondDisplayCondition ) );
        iServiceProviderName.iPLMNField.Append( word );

        for ( TUint8 j( 0 ); j < lengthOfDataInBytes / 2; j++, i += 2 )
            {
            // PLMN entries are copied to 16-bit buffer as follows:
            // 1st byte to 8 LSB bits of 16-bit buffer
            // 2nd byte to 8 MSB bits of 16-bit buffer
            word = static_cast<TUint16>(
                ( aFileData[i+1] << 8 ) | aFileData[i] );
            iServiceProviderName.iPLMNField.Append( word );
            }

        iServiceProviderName.iPLMNField.Append( ( 0xFF << 8 ) | aFileData[i] );

        CMmDataPackage dataPackage;
        dataPackage.PackData( &iServiceProviderName );
        iMessageRouter->Complete(
            EMobilePhoneGetServiceProviderName,
            &dataPackage,
            KErrNone );
        }
    else
        {
        iMessageRouter->Complete(
             EMobilePhoneGetServiceProviderName,
             KErrNotFound );
        }

    iServiceProviderName.iSPName.Zero();
    iServiceProviderName.iPLMNField.Zero();
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccRefreshInd
//
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccRefreshInd( const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccRefreshInd");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREFRESHIND_TD, "CMmPhoneMessHandler::UiccRefreshInd" );
    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_REFRESH_IND_OFFSET_SERVICETYPE ) );
    if ( UICC_REFRESH_PERMISSION == serviceType )
        {
        UiccRefreshReq( UICC_REFRESH_OK );
        }
    else if ( UICC_REFRESH_NOW == serviceType )
        {
        HandleUiccRefresh( aIsiMessage );
        }
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccRefreshResp
//
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccRefreshResp( const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccRefreshResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREFRESHRESP_TD, "CMmPhoneMessHandler::UiccRefreshResp" );
    if ( iCompleteRefresfDone )
        {
        TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + UICC_REFRESH_RESP_OFFSET_STATUS ) );
        TInt err( KErrNone );
        if ( UICC_STATUS_OK != status )
            {
            err = KErrGeneral;
            }
        iMessageRouter->Complete( EMmTsySimRefreshDoneIPC, err );
        iCompleteRefresfDone = EFalse;
        }
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccRefreshReq
//
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccRefreshReq( TUint8 aStatus )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccRefreshReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREFRESHREQ_TD, "CMmPhoneMessHandler::UiccRefreshReq" );
    TInt ret( KErrNone );

    // If there was error in cacheing some file, set internal flag
    if ( UICC_REFRESH_NOT_OK == aStatus )
        {
        iRefreshError = ETrue;
        }

    // If NTSY or CTSY cacheing is ongoing, message is not sent
    if ( ! iCommonTSYRefreshPending && ! iInternalRefreshFiles )
        {
        if ( iRefreshError ) // Some cacheing was failed
            {
            aStatus = UICC_REFRESH_NOT_OK;
            }

        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, KPhoneTransId );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, UICC_REFRESH_REQ );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_REFRESH_REQ_OFFSET_SERVICETYPE,
            UICC_REFRESH_STATUS );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_REFRESH_REQ_OFFSET_STATUS,
            aStatus );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_REFRESH_REQ_OFFSET_CLIENTID,
            iMmUiccMessHandler->GetUiccClientId() );
        isiMsg.Set16bit( ISI_HEADER_SIZE + UICC_REFRESH_REQ_OFFSET_FILLERBYTE1,
            KPadding );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_REFRESH_REQ_OFFSET_NSB,
            0 ); // No subblocks

        ret = iPhoNetSender->Send( isiMsg.Complete() );
        iRefreshError = EFalse;

        // When NTSY/CTSY refresh was performed
        // IPC EMmTsySimRefreshDoneIPC will be completed to CTSY
        if ( UICC_REFRESH_DONE == aStatus || UICC_REFRESH_NOT_OK == aStatus )
            {
            iCompleteRefresfDone = ETrue;
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::HandleUiccRefresh
//
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::HandleUiccRefresh( const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING("TSY:CMmPhoneMessHandler::HandleUiccRefresh" );
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh" );

    TUint16 refreshFiles( 0 );
    TBool enabledServicesChanged( EFalse );
    TUint startIndex( ISI_HEADER_SIZE + SIZE_UICC_REFRESH_IND );

    // There is one subblock UICC_SB_APPL_PATH per refreshed file
    TUint uiccSbApplPathOffset( 0 );
    while ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        startIndex,
        UICC_SB_APPL_PATH,
        EIsiSubBlockTypeId16Len16,
        uiccSbApplPathOffset ) )
        {
        TUint16 sbLen( aIsiMessage.Get16bit(
            uiccSbApplPathOffset + UICC_SB_APPL_PATH_OFFSET_SBLEN ) );

        // Get file id
        TUint16 fileId( aIsiMessage.Get16bit(
            uiccSbApplPathOffset + UICC_SB_APPL_PATH_OFFSET_EF ) );

        switch( fileId )
            {
            case KElemFileSmsParams: // 6F42, SMS parameters
                {
TFLOGSTRING("TSY:CMmPhoneMessHandler::HandleUiccRefresh: SMSP refresh.");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: SMSP refresh" );
                refreshFiles |= KCacheSmsParams;
                break;
                }
            case KElemFileFixedDiallingNumbers: // 6F3B, Storage FDN
                {
TFLOGSTRING("TSY:CMmPhoneMessHandler::HandleUiccRefresh: FDN refresh");
OstTrace0( TRACE_NORMAL,  DUP3_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: FDN refresh" );
                refreshFiles |= KCacheFdn;
                break;
                }
            case KElemSimServiceTable: // 6F38, Service table
                {
TFLOGSTRING("TSY:CMmPhoneMessHandler::HandleUiccRefresh: ServiceTable refresh");
OstTrace0( TRACE_NORMAL,  DUP4_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: ServiceTable refresh" );
                refreshFiles |= KCacheServiceTable;
                // Also internal service table cacheing is needed. EFalse means
                // no complete to CTSY.
                iInternalRefreshFiles |= KRefreshServiceTable;
                iMmUiccMessHandler->InitializeSimServiceTableCache( EFalse );
                break;
                }
            case KElemFileDynFlagsOrange: // 6F9F, ALS
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: ALS line refresh");
OstTrace0( TRACE_NORMAL,  DUP5_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: ALS line." );
                refreshFiles |= KCacheALSline;
                break;
                }
            case KElemFileShortMessages: // 6F3C, SMS
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: SMS refresh");
OstTrace0( TRACE_NORMAL,  DUP6_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: SMS refresh" );
                iInternalRefreshFiles |= KRefreshSms;
                iMessageRouter->GetSmsMessHandler()->InitializeSmsCache();
                break;
                }
            case KElemFileAcl: // 6F57, ACL
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: APN refresh");
OstTrace0( TRACE_NORMAL,  DUP7_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: APN refresh" );
                iMessageRouter->Complete(
                    EMobilePhoneNotifyAPNListChanged,
                    KErrNone );

                if ( iAPNList )
                    {
                    iAPNList->Reset();
                    delete iAPNList;
                    iAPNList = NULL;
                    }
                break;
                }
            case KElemEst: // 6F56, Enabled service table
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: ServiceTable enabled");
OstTrace0( TRACE_NORMAL,  DUP8_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: ServiceTable enabled" );
                enabledServicesChanged = ETrue;
                break;
                }
            case KElemFileCallFwdFlagsCphs:
            case KElemFileCallFwdIndicationStatus:
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: Call forwarding flags refresh ");
OstTrace0( TRACE_NORMAL,  DUP9_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: Call forwarding flags refresh" );
                // Re-read call forwarding flags
                iInternalRefreshFiles |= KRefreshCallForwardingFlags;

                iMessageRouter->GetSupplServHandler()->ExtFuncL(
                    ECustomNotifyIccCallForwardingStatusChangeIPC,
                    NULL );
                break;
                }
            case KElemFileOpl: // 6FC6, Operator PLMN List
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: OPL list refresh");
OstTrace0( TRACE_NORMAL,  DUP10_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: OPL list refresh" );

                iInternalRefreshFiles |= KRefreshOplRules;

                iMessageRouter->
                    GetNetMessHandler()->
                    GetNetOperatorNameHandler()->
                    UiccReadOplRecordCount();
                break;
                }
            case KElemFilePlmnNetworkName: // 6FC5, PLMN Network Name
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: PNN refresh");
OstTrace0( TRACE_NORMAL,  DUP11_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: PNN refresh" );

                iInternalRefreshFiles |= KRefreshPnnRecord;

                // Get PNN record to be cached.
                TUint8 pnnIdentifier(
                    iMessageRouter->
                    GetNetMessHandler()->
                    GetNetOperatorNameHandler()->
                    GetPnnRecordIdentifierValue() );

                iMessageRouter->
                    GetNetMessHandler()->
                    GetNetOperatorNameHandler()->
                    UiccOperatorReqReadPnn( pnnIdentifier );
                break;
                }
            case KElemFileOperatorName: // 6F14, Operator name
                {
TFLOGSTRING("TSY: CMmPhoneMessHandler::HandleUiccRefresh: ONS name refresh");
OstTrace0( TRACE_NORMAL,  DUP12_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: ONS name refresh" );

                iInternalRefreshFiles |= KRefreshOnsName;

                iMessageRouter->
                    GetNetMessHandler()->
                    GetNetOperatorNameHandler()->
                    UiccOperatorReq();
                break;
                }
            default:
                {
TFLOGSTRING2("TSY:CMmPhoneMessHandler::HandleUiccRefresh:Unknown type: 0x%x", fileId);
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh;fileId=%hu", fileId );
                break;
                }
            }

        // Abbreviated dialling numbers, EFadn 4FXX
        if ( 0x4F == ( fileId >> 8 ) )
            {
TFLOGSTRING("TSY:CMmPhoneMessHandler::HandleUiccRefresh: ADN refresh.");
OstTrace0( TRACE_NORMAL,  DUP13_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh: ADN refresh" );

            refreshFiles |= KCacheAdn;
            }

        startIndex = uiccSbApplPathOffset + sbLen;
        }

    // If Enabled Services table has been changed during the refresh,
    // we need to make phonebook init phase 1 to enable UICC to send
    // indication about FDN.
    if ( enabledServicesChanged )
        {
        // In case of ADN or FDN phonebook refresh, CommonTSY is
        // responsible of making phonebook initialization. => No need for
        // internal initialization.
        if ( !( refreshFiles & KCacheAdn )
            && !( refreshFiles & KCacheFdn ) )
            {
            TName phonebookName;
            phonebookName.Copy( KInternalPhoneBookType );

            CPhoneBookDataPackage package;
            package.SetPhoneBookName( phonebookName );

            iMessageRouter->PhoneBookStoreMessHandler()->ExtFuncL(
                EMmTsyPhoneBookStoreInitIPC,
                &package );
            }
        }

    // Check if we have any files to refresh in CommonTSY.
TFLOGSTRING2("TSY: CMmPhoneMessHandler::HandleUiccRefresh: refreshFiles = %d", refreshFiles );
OstTraceExt1( TRACE_NORMAL,  DUP14_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh;refreshFiles=%hu", refreshFiles );
    if ( 0 < refreshFiles )
        {
        iCommonTSYRefreshPending = ETrue;

TFLOGSTRING2("TSY: CMmPhoneMessHandler::HandleUiccRefresh: CommonTSY Refresh Pending = %d", iCommonTSYRefreshPending );
OstTrace1( TRACE_NORMAL,  DUP15_CMMPHONEMESSHANDLER_HANDLEUICCREFRESH_TD, "CMmPhoneMessHandler::HandleUiccRefresh;iCommonTSYRefreshPending=%d", iCommonTSYRefreshPending );

        CMmDataPackage dataPackage;
        dataPackage.PackData( &refreshFiles );

        // Complete. This will start the cache update process.
        iMessageRouter->Complete(
            EMmTsySimRefreshNowIPC,
            &dataPackage,
            KErrNone );
        }

    if ( ! iCommonTSYRefreshPending && ! iInternalRefreshFiles )
        {
        UiccRefreshReq( UICC_REFRESH_DONE );
        }
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::SmsCachingCompleted
// Checks if "Refresh Done" will be sent to UICC now or later.
// Counterpart is CMmPhoneMessHandler::ExtFuncL, case
// EMmTsySimRefreshDoneIPC.
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::SmsCachingCompleted( TInt aError )
    {
    iInternalRefreshFiles ^= KRefreshSms;

    if ( KErrNone == aError )
        {
TFLOGSTRING("TSY: CMmPhoneMessHandler::SmsCachingCompleted: nokiatsy sms caching completed");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_SMSCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::SmsCachingCompleted: nokiatsy sms caching completed" );
        UiccRefreshReq( UICC_REFRESH_DONE );
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::SmsCachingCompleted: nokiatsy sms caching completed with error %d", aError);
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_SMSCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::SmsCachingCompleted: nokiatsy sms caching completed with error" );
        UiccRefreshReq( UICC_REFRESH_NOT_OK );
        }
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::ServiceTableCachingCompleted
// Checks if "Refresh Done" will be sent to UICC now or later.
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::ServiceTableCachingCompleted( TInt aError )
    {
    iInternalRefreshFiles ^= KRefreshServiceTable;

    if ( UICC_STATUS_OK == aError )
        {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ServiceTableCachingCompleted: nokiatsy service table caching completed");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_SERVICETABLECACHINGCOMPLETED_TD, "CMmPhoneMessHandler::ServiceTableCachingCompleted: nokiatsy service table caching completed" );
        UiccRefreshReq( UICC_REFRESH_DONE );
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::ServiceTableCachingCompleted: nokiatsy service table caching completed with error %d", aError);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_SERVICETABLECACHINGCOMPLETED_TD, "CMmPhoneMessHandler::ServiceTableCachingCompleted: nokiatsy service table caching completed with error;aError=%d", aError );
        UiccRefreshReq( UICC_REFRESH_NOT_OK );
        }
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::CallForwFlagsCachingCompleted
// Checks if "Refresh Done" will be sent to UICC.
// Counterpart is CMmPhoneMessHandler::ExtFuncL, case
// EMmTsySimRefreshDoneIPC.
// ----------------------------------------------------------------------------
//
void CMmPhoneMessHandler::CallForwFlagsCachingCompleted( TUint aError )
    {
    iInternalRefreshFiles ^= KRefreshCallForwardingFlags;

    if ( UICC_STATUS_OK == aError )
        {
TFLOGSTRING("TSY: CMmPhoneMessHandler::CallForwFlagsCachingCompleted: nokiatsy refresh successfully completed ==> REFRESH_DONE");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_CALLFORWFLAGSCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::CallForwFlagsCachingCompleted: nokiatsy refresh successfully completed" );
        UiccRefreshReq( UICC_REFRESH_DONE );
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::CallForwFlagsCachingCompleted: nokiatsy refresh completed with error %d ==> REFRESH_DONE", aError);
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_CALLFORWFLAGSCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::CallForwFlagsCachingCompleted: nokiatsy refresh completed with error" );
        UiccRefreshReq( UICC_REFRESH_NOT_OK );
        }
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::OplRulesCachingCompleted
// Checks if "Refresh Done" will be sent to UICC.
// ----------------------------------------------------------------------------
//
TBool CMmPhoneMessHandler::OplRulesCachingCompleted( TInt aError )
    {
    // Initialize OPL refresh ongoing value.
    TBool oplRefreshOngoing( EFalse );

    // Check if refresh is ongoing.
    if ( iInternalRefreshFiles & KRefreshOplRules )
        {
        iInternalRefreshFiles ^= KRefreshOplRules;

        oplRefreshOngoing = ETrue;

        if ( KErrNone == aError )
            {
TFLOGSTRING("TSY: CMmPhoneMessHandler::OplRulesCachingCompleted: nokiatsy refresh successfully completed ==> REFRESH_DONE");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_OPLRULESCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::OplRulesCachingCompleted: nokiatsy refresh successfully completed ==> REFRESH_DONE" );
            UiccRefreshReq( UICC_REFRESH_DONE );
            }
        else
            {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::OplRulesCachingCompleted: nokiatsy refresh completed with error %d ==> REFRESH_DONE", aError);
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_OPLRULESCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::OplRulesCachingCompleted: nokiatsy refresh completed with error" );
            UiccRefreshReq( UICC_REFRESH_NOT_OK );
            }
        }

    return oplRefreshOngoing;
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::PnnRecordCachingCompleted
// Checks if "Refresh Done" will be sent to UICC.
// ----------------------------------------------------------------------------
//
void CMmPhoneMessHandler::PnnRecordCachingCompleted( TUint8 aError )
    {
    // Check if refresh is ongoing.
    if ( iInternalRefreshFiles & KRefreshPnnRecord )
        {
        iInternalRefreshFiles ^= KRefreshPnnRecord;

        if ( UICC_STATUS_OK == aError )
            {
TFLOGSTRING("TSY: CMmPhoneMessHandler::PnnRecordCachingCompleted: nokiatsy refresh successfully completed ==> REFRESH_DONE");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_PNNRECORDCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::PnnRecordCachingCompleted: nokiatsy refresh successfully completed ==> REFRESH_DONE" );
            UiccRefreshReq( UICC_REFRESH_DONE );
            }
        else
            {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::PnnRecordCachingCompleted: nokiatsy refresh completed with error %d ==> REFRESH_DONE", aError);
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_PNNRECORDCACHINGCOMPLETED_TD, "CMmPhoneMessHandler::PnnRecordCachingCompleted: nokiatsy refresh completed with error" );
            UiccRefreshReq( UICC_REFRESH_NOT_OK );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::OnsNameCachingCompleted
// Checks if "Refresh Done" will be sent to SIMSON.
// ----------------------------------------------------------------------------
//
void CMmPhoneMessHandler::OnsNameCachingCompleted( TUint8 aError )
    {
    // Check if refresh is ongoing.
    if ( iInternalRefreshFiles & KRefreshOnsName )
        {
        iInternalRefreshFiles ^= KRefreshOnsName;

        if ( UICC_STATUS_OK == aError )
            {
TFLOGSTRING("TSY: CMmPhoneMessHandler::OnsNameCachingCompleted: nokiatsy refresh successfully completed ==> REFRESH_DONE");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_ONSNAMECACHINGCOMPLETED_TD, "CMmPhoneMessHandler::OnsNameCachingCompleted: nokiatsy refresh successfully completed ==> REFRESH_DONE" );
            UiccRefreshReq( UICC_REFRESH_DONE );
            }
        else
            {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::OnsNameCachingCompleted: nokiatsy refresh completed with error %d ==> REFRESH_DONE", aError);
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_ONSNAMECACHINGCOMPLETED_TD, "CMmPhoneMessHandler::OnsNameCachingCompleted: nokiatsy refresh completed with error" );
            UiccRefreshReq( UICC_REFRESH_NOT_OK );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccReadEfEstReq
// reads EFest
// ----------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccReadEfEstReq()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadEfEstReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREADEFESTREQ_TD, "CMmPhoneMessHandler::UiccReadEfEstReq" );

    TInt ret( KErrNone );

    if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() &&
        iMmUiccMessHandler->GetServiceStatus( KServiceAcl ) )
        {
        ret = UiccApplCmdReq(
            ETrIdAclStatusReadEfEst,
            UICC_APPL_READ_TRANSPARENT,
            0,
            0,
            KElemEst,
            5 );
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadEfEstReq: card type is NOT UICC or ACL not supported in EFust");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCREADEFESTREQ_TD, "CMmPhoneMessHandler::UiccReadEfEstReq: card type is NOT UICC or ACL not supported in EFust" );
        iACLIsProgress = EFalse;
        ret = KErrNotFound;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccGetAclStatusReadEfEstResp
// Handles EFest read response in case of getting ACL status
// ----------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccGetAclStatusReadEfEstResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccGetAclStatusReadEfEstResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCGETACLSTATUSREADEFESTRESP_TD, "CMmPhoneMessHandler::UiccGetAclStatusReadEfEstResp" );

    TInt ret( KErrNone );
    RMobilePhone::TAPNControlListServiceStatus aclStatus;

    if( ( UICC_STATUS_OK == aStatus ) && ( 0 < aFileData.Length() ) )
        {
        TUint8 aclState( aFileData[0] & KAclStateMask );
        if( aclState )
            {
            aclStatus = RMobilePhone::EAPNControlListServiceEnabled;
            }
        else
            {
            aclStatus = RMobilePhone::EAPNControlListServiceDisabled;
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccGetAclStatusReadEfEstResp: EFest reading failed: 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCGETACLSTATUSREADEFESTRESP_TD, "CMmPhoneMessHandler::UiccGetAclStatusReadEfEstResp: EFest reading failed: 0x%x", aStatus );
        ret = KErrAccessDenied;
        }

    CMmDataPackage dataPackage;

    if( KErrNone == ret )
        {
        dataPackage.PackData( &aclStatus );
        }

    iACLIsProgress = EFalse;
    iMessageRouter->Complete(
        EMobilePhoneGetAPNControlListServiceStatus,
        &dataPackage,
        ret );
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp
// Handles EFest read response in case of setting ACL status
// ----------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCSETACLSTATUSREADEFESTRESP_TD, "CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp" );

    TInt ret( KErrNone );
    TBool completeNeeded( ETrue );
    TUint8 aclState( 0 );

    if ( 0 < aFileData.Length() )
        {
        aclState = aFileData[0];
        }

    if ( ( UICC_STATUS_OK == aStatus ) && ( aclState & KAclStateMask ) )
        {
        if ( RMobilePhone::EAPNControlListServiceDisabled == iAclStatus )
            {
            // ACL state is enabled and we need to set it to disabled.
            // So EFest needs to be updated
            completeNeeded = EFalse;
            UiccSetAclStatusWriteEfEstReq( aclState );
            }
        else
            {
            // ACL status already correct, let's complete
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp: ACL status already correct, let's complete");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCSETACLSTATUSREADEFESTRESP_TD, "CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp: ACL status already correct, let's complete" );
            }
        }
    else if ( ( UICC_STATUS_OK == aStatus ) && ( ! aclState & KAclStateMask ) )
        {
        if ( RMobilePhone::EAPNControlListServiceEnabled == iAclStatus )
            {
            // EFest needs to be updated
            completeNeeded = EFalse;
            UiccSetAclStatusWriteEfEstReq( aclState );
            }
        else
            {
            // ACL status already correct, let's complete
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp: ACL status already correct, let's complete");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPHONEMESSHANDLER_UICCSETACLSTATUSREADEFESTRESP_TD, "CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp: ACL status already correct, let's complete" );
            }
        }
    else
        {
        // EFest reading failed
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp: EFest reading failed: 0x%x", aStatus );
OstTrace1( TRACE_NORMAL,  DUP3_CMMPHONEMESSHANDLER_UICCSETACLSTATUSREADEFESTRESP_TD, "CMmPhoneMessHandler::UiccSetAclStatusReadEfEstResp: EFest reading failed: 0x%x", aStatus );
        ret = KErrAccessDenied;
        }

    if( completeNeeded )
        {
        iACLIsProgress = EFalse;
        iMessageRouter->Complete(
            EMobilePhoneSetAPNControlListServiceStatus,
            ret );
        }
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstReq
// Writes ACL status to EFest
// ----------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstReq( TUint8 aOldAclState )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCSETACLSTATUSWRITEEFESTREQ_TD, "CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstReq" );

    TInt ret( KErrNone );
    TUint8 newState;
    if( RMobilePhone::EAPNControlListServiceDisabled == iAclStatus )
        {
        // ACL status is going to be disabled and status is in 3rd
        // bit of 1st byte, so let's set 3rd bit in 1st byte to 0
        newState = aOldAclState & 0xFB;
        }
    else
        {
        // ACL status is going to be enabled and status is in 3rd
        // bit of 1st byte, so let's set 3rd bit in 1st byte to 1
        newState = aOldAclState + 0x04;
        }

    if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() &&
        iMmUiccMessHandler->GetServiceStatus( KServiceAcl ) )
        {
        TBuf8<KAclStatusLength> fileData;
        fileData.Append( newState );
        ret = UiccApplCmdReq(
            ETrIdAclStatusWriteEfEst,
            UICC_APPL_UPDATE_TRANSPARENT,
            0,
            0,
            KElemEst,
            5,
            fileData );
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstReq: card type is NOT UICC or ACL not supported in EFust");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCSETACLSTATUSWRITEEFESTREQ_TD, "CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstReq: card type is NOT UICC or ACL not supported in EFust" );
        iACLIsProgress = EFalse;
        ret = KErrGeneral;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstResp
// Handles EFest write response in case of setting ACL status
// ----------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstResp(
    TInt aStatus )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCSETACLSTATUSWRITEEFESTRESP_TD, "CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstResp" );
    TInt ret( KErrAccessDenied );

    if( UICC_STATUS_OK == aStatus )
        {
        ret = KErrNone;
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstResp: writing failed: 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCSETACLSTATUSWRITEEFESTRESP_TD, "CMmPhoneMessHandler::UiccSetAclStatusWriteEfEstResp: writing failed: 0x%x", aStatus );
        }

    iACLIsProgress = EFalse; //set ACL flag
    iMessageRouter->Complete(
        EMobilePhoneSetAPNControlListServiceStatus,
        ret );

    if ( KErrNone == ret )
        {
        iMessageRouter->Complete(
            EMobilePhoneNotifyAPNControlListServiceStatusChange,
            KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccReadAclReq
// reads ACL list from EFacl
// ----------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccReadAclReq()
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadAclReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREADACLREQ_TD, "CMmPhoneMessHandler::UiccReadAclReq" );

    TInt ret( KErrNone );

    if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() &&
        iMmUiccMessHandler->GetServiceStatus( KServiceAcl ) )
        {
        ret = UiccApplCmdReq(
            ETrIdAclReadEfAcl,
            UICC_APPL_READ_TRANSPARENT,
            0,
            0,
            KElemFileAcl,
            UICC_SFI_NOT_PRESENT );
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadAclReq: card type is NOT UICC or ACL not supported in EFust");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCREADACLREQ_TD, "CMmPhoneMessHandler::UiccReadAclReq: card type is NOT UICC or ACL not supported in EFust" );
        iACLIsProgress = EFalse;
        ret = KErrNotFound;
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccReadAclResp
// Handles response for ACL list reading from EFacl
// ----------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccReadAclResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccReadAclResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCREADACLRESP_TD, "CMmPhoneMessHandler::UiccReadAclResp" );
    TInt ret( KErrNone );
    if ( ( UICC_STATUS_OK == aStatus ) &&
        ( KApnDataIndex < aFileData.Length() ) )
        {
        if( iAPNList )
            {
            delete iAPNList;
            iAPNList = 0;
            }

        TPtrC8 dataPoint;
        dataPoint.Set( &aFileData[KApnDataIndex], aFileData.Length() - 1 );
        iAPNList = DecodeACL( dataPoint, aFileData[KNumOfApnsIndex] );

        switch( iOngoingAclIpc )
            {
            case EMobilePhoneEnumerateAPNEntries:
                {
                CompleteEnumerateAPNEntries();
                break;
                }
            case EMobilePhoneGetAPNname:
                {
                CompleteGetAPNName( iAPNReadOrDeleteIndex );
                break;
                }
            case EMobilePhoneDeleteAPNName:
                {
                ret = UiccDeleteApnEntry( iAPNReadOrDeleteIndex );
                break;
                }
            case EMobilePhoneAppendAPNName:
                {
                iAPNList->AppendL( iAPNDataBuffer.iApn );
                ret = UiccWriteEfAclReq();
                break;
                }
            default:
                {
TFLOGSTRING2( "TSY: CMmPhoneMessHandler::UiccReadAclResp: unknown IPC (%d)", iOngoingAclIpc );
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCREADACLRESP_TD, "CMmPhoneMessHandler::UiccReadAclResp: unknown IPC (%d)", iOngoingAclIpc );
                iACLIsProgress = EFalse;
                break;
                }
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccReadAclResp: reading failed: 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP2_CMMPHONEMESSHANDLER_UICCREADACLRESP_TD, "CMmPhoneMessHandler::UiccReadAclResp: reading failed: 0x%x", aStatus );
        ret = KErrAccessDenied;
        }

    if( KErrNone != ret )
        {
        iACLIsProgress = EFalse;
        iMessageRouter->Complete( iOngoingAclIpc, ret );
        }
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::DecodeACL
// Encrypt from ACL to CDesC8ArrayFlat
// --------------------------------------------------------------------------
//
CDesC8ArrayFlat* CMmPhoneMessHandler::DecodeACL
        (
        const TDesC8& aTlv,
        TInt aTotalEntries
        ) const
    {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::DecodeACL. Number of total entries: %d", aTotalEntries);
OstTrace1( TRACE_NORMAL,  CMMPHONEMESSHANDLER_DECODEACL_TD, "CMmPhoneMessHandler::DecodeACL. Number of total entries: %d", aTotalEntries );

    CDesC8ArrayFlat* apnList = new( ELeave ) CDesC8ArrayFlat( 1 );
    CleanupStack::PushL( apnList );

    TInt offset( 0 );

    if ( 0 < aTlv.Length() )
        {
        // decode TLV entries to CDesC8ArrayFlat
        for ( TInt i = 0; i < aTotalEntries; i++ )
            {
            TInt tag( aTlv[offset++] );

            // spec: The tag value of the APN-TLV shall be 'DD'
            TF_ASSERT( tag == 0xdd );

            TInt len( aTlv[offset++] );
            TF_ASSERT( len <= 100 );   // spec: APN max length is 100

            TPtrC8 data = aTlv.Mid( offset, len );
            apnList->InsertL( i, data );

            offset += len; // advance to next TLV
            }
        }

    CleanupStack::Pop( apnList );
    return apnList;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::CompleteEnumerateAPNEntries
// send number of ACL index to the client.
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::CompleteEnumerateAPNEntries()
    {
    TInt indexCount( iAPNList->MdcaCount() );
TFLOGSTRING2("TSY: CMmPhoneMessHandler::CompleteEnumerateAPNEntries. Number of APN's: %d",indexCount);
OstTrace1( TRACE_NORMAL,  CMMPHONEMESSHANDLER_COMPLETEENUMERATEAPNENTRIES_TD, "CMmPhoneMessHandler::CompleteEnumerateAPNEntries. Number of APN's: %d", indexCount );

    CMmDataPackage dataPackage;
    dataPackage.PackData( &indexCount );

    iACLIsProgress = EFalse;
    iMessageRouter->Complete(
        EMobilePhoneEnumerateAPNEntries,
        &dataPackage,
        KErrNone );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::CompleteGetAPNName
// send APN name to the client.
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::CompleteGetAPNName( TUint8 aIndex )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::CompleteGetAPNName ");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_COMPLETEGETAPNNAME_TD, "CMmPhoneMessHandler::CompleteGetAPNName" );

    CMmDataPackage dataPackage;
    RMobilePhone::TAPNEntryV3 aclEntry;
    TInt err( KErrNone );

    if ( iAPNList->MdcaCount() <= aIndex )
        {
        err = KErrOverflow;
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::CompleteGetAPNName. Copy entry from index %d",aIndex);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_COMPLETEGETAPNNAME_TD, "CMmPhoneMessHandler::CompleteGetAPNName. Copy entry from index %d", aIndex );
        aclEntry.iApn.Copy( iAPNList->MdcaPoint( aIndex ) );
        }

    dataPackage.PackData( &aclEntry );

    iACLIsProgress = EFalse;
    iMessageRouter->Complete( EMobilePhoneGetAPNname, &dataPackage, err );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccDeleteApnEntry
// Deletes APN entry from internal cache and writes the change to EFacl
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccDeleteApnEntry( TInt aIndex )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccDeleteApnEntry ");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCDELETEAPNENTRY_TD, "CMmPhoneMessHandler::UiccDeleteApnEntry" );

    TInt ret;
    if ( iAPNList->MdcaCount() <= aIndex )
        {
        ret = KErrOverflow;
        }
    else
        {
        iAPNList->Delete( aIndex );
        iAPNList->Compress();

        //write new list to the SIM
        ret = UiccWriteEfAclReq();
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccWriteEfAclReq
// Writes APN entries from internal cache to the EFAcl
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccWriteEfAclReq()
    {
TFLOGSTRING( "TSY: CMmPhoneMessHandler::UiccWriteEfAclReq" );
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCWRITEEFACLREQ_TD, "CMmPhoneMessHandler::UiccWriteEfAclReq" );

    TInt ret( KErrNone );

    if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() &&
        iMmUiccMessHandler->GetServiceStatus( KServiceAcl ) )
        {
        TInt dataLen( ACLLength( iAPNList ) );
        TUint8 apnCount( iAPNList->MdcaCount() );

        TBuf8<KFileDataLength> fileData;
        fileData.Append( apnCount );
        for ( TInt i = 0; i < apnCount; i++ )
            {
            TPtrC8 apn = iAPNList->MdcaPoint( i );
            // spec: The tag value of the APN-TLV shall be 'DD'
            fileData.Append( 0xdd );
            fileData.Append( apn.Length() );
            fileData.Append( apn );
            }

        ret = UiccApplCmdReq(
            ETrIdAclWriteEfAcl,
            UICC_APPL_UPDATE_TRANSPARENT,
            0,
            0,
            KElemFileAcl,
            UICC_SFI_NOT_PRESENT,
            fileData );
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccWriteEfAclReq: card type is NOT UICC or ACL not supported in EFust");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCWRITEEFACLREQ_TD, "CMmPhoneMessHandler::UiccWriteEfAclReq: card type is NOT UICC or ACL not supported in EFust" );
        iACLIsProgress = EFalse;
        ret = KErrNotFound;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccWriteEfAclResp
// Handles response for EFAcl writing
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccWriteEfAclResp( TInt aStatus )
    {
TFLOGSTRING( "TSY: CMmPhoneMessHandler::UiccWriteEfAclResp" );
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCWRITEEFACLRESP_TD, "CMmPhoneMessHandler::UiccWriteEfAclResp" );

    TInt ret( KErrNone );
    if( UICC_STATUS_OK != aStatus )
        {
TFLOGSTRING2( "TSY: CMmPhoneMessHandler::UiccWriteEfAclResp: EFacl writing failed: 0x%x", aStatus );
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_UICCWRITEEFACLRESP_TD, "CMmPhoneMessHandler::UiccWriteEfAclResp: EFacl writing failed: 0x%x", aStatus );

        ret = KErrAccessDenied;
        }

    iACLIsProgress = EFalse;
    iMessageRouter->Complete(
        iOngoingAclIpc,
        ret );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::ACLLength
// calculate total length of ACL
// --------------------------------------------------------------------------
//
TUint16 CMmPhoneMessHandler::ACLLength( CDesC8ArrayFlat* aApnList ) const
    {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::ACLLength. Count: %d", aApnList->MdcaCount());
OstTrace1( TRACE_NORMAL,  CMMPHONEMESSHANDLER_ACLLENGTH_TD, "CMmPhoneMessHandler::ACLLength. Count: %d", aApnList->MdcaCount() );

    TUint16 length( 0 );

    if ( aApnList )
        {
        for ( TInt i = 0; i < aApnList->MdcaCount(); i++ )
            {
            TPtrC8 apn = aApnList->MdcaPoint( i );
            length += apn.Length() + 2;
            }
        }

    // result is incremented by one because of EFacl contains number of tlv objects
    // as first byte of the file data
    length++;

TFLOGSTRING2("TSY: CMmPhoneMessHandler::ACLLength. ACL len: %d", length);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEMESSHANDLER_ACLLENGTH_TD, "CMmPhoneMessHandler::ACLLength. ACL len: %d", length );

    return length;
    }


// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccApplCmdReq
//
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccApplCmdReq(
    const TUiccTrId aTrId,
    const TUint8 aServiceType,
    const TUint16 aDataAmount,
    const TUint16 aDataOffset,
    const TUint16 aFileId,
    const TUint8 aFileIdSfi,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccApplCmdReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEMESSHANDLER_UICCAPPLCMDREQ_TD, "CMmPhoneMessHandler::UiccApplCmdReq" );

    TInt ret( KErrNone );
    TBuf8<KFilePathLength> filePath;
    filePath.Append( KMasterFileId >> 8 );
    filePath.Append( KMasterFileId );
    if ( KElemFileDynFlagsOrange == aFileId )
        {
        filePath.Append( KOrangeDedicatedFile >> 8 );
        filePath.Append( KOrangeDedicatedFile );
        }
    else
        {
        filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
        }

    if ( UICC_APPL_READ_TRANSPARENT == aServiceType )
        {
        TUiccReadTransparent params;
        params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        params.trId = aTrId;
        params.dataAmount = aDataAmount;
        params.dataOffset = aDataOffset;
        params.fileId = aFileId;
        params.fileIdSfi = aFileIdSfi;
        params.serviceType = aServiceType;
        params.filePath.Append( filePath );
        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    else if ( UICC_APPL_UPDATE_TRANSPARENT == aServiceType )
        {
        TUiccWriteTransparent params;
        params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        params.trId = aTrId;
        params.dataAmount = aDataAmount;
        params.dataOffset = aDataOffset;
        params.fileId = aFileId;
        params.fileIdSfi = aFileIdSfi;
        params.serviceType = aServiceType;
        params.fileData.Append( aFileData );
        params.filePath.Append( filePath );
        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    // No else
    return ret;
    }

// End of file
