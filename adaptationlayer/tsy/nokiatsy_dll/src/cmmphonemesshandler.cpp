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
#include "cmmmessagerouter.h"
#include "cmmphonemesshandler.h"
#include "cmmphonetsender.h"
#include "cmmsmsmesshandler.h"
#include "cmmstaticutility.h"
#include "cmmsupplservmesshandler.h"
#include "cmmnetmesshandler.h"
#include "cmmnetoperatornamehandler.h"
#include "tsylogger.h"
#include "cmmuiccmesshandler.h"

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

#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonemesshandlertraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS

const TUint8 KPhoneTransId = 6; //hard coded transaction ID
const TUint8 KImsiSize = 8;
const TUint8 KServiceProviderSize = 36;
const TUint8 KSpnFileSize = 16;

// ------------------------------------------------------
// --- Alternate Line Service (ALS)-related constants ---
// ------------------------------------------------------
// Consts for mapping the Als Line values, as used in SIMSON's
// SIM_SERV_DYNAMIC_FLAGS_STR structure.
const TUint8 KAlsAuxiliaryLine = 0x00; // ALS alternate line
const TUint8 KAlsPrimaryLine = 0x01;   // ALS primary line

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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_CMMPHONEMESSHANDLER, "CMmPhoneMessHandler::CMmPhoneMessHandler" );

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
        CMmPhoNetSender* aPhoNetSender,  //a ptr to the phonet sender
        CMmPhoNetReceiver* aPhoNetReceiver,  //a ptr to the phonet receiver
        CMmMessageRouter* aMessageRouter, // pointer to the msg router
        CMmSupplServMessHandler* aSupplServMessHandler,
        CMmUiccMessHandler* aUiccMessHandler
        )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_NEWL, "CMmPhoneMessHandler::NewL" );

    CMmPhoneMessHandler* phoneMessHandler =
        new ( ELeave ) CMmPhoneMessHandler();

    CleanupStack::PushL( phoneMessHandler );
    phoneMessHandler->iPhoNetSender = aPhoNetSender;
    phoneMessHandler->ConstructL( aMessageRouter );
    phoneMessHandler->iSupplServMessHandler = aSupplServMessHandler;
    phoneMessHandler->iMmUiccMessHandler = aUiccMessHandler;

    aPhoNetReceiver->RegisterL(
        phoneMessHandler,
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        PN_INFO,
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        PN_MODEM_INFO,
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        INFO_SERIAL_NUMBER_READ_RESP );

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
        CMmMessageRouter* aMessageRouter
        )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_CONSTRUCTL, "CMmPhoneMessHandler::ConstructL" );

    // Internal pointers, must be nulled before used.
    iSSTFileData = NULL;
    iStoreInfo = NULL;
    iLocIndex = 0;

    iMessageRouter = aMessageRouter;

    iCommonTSYRefreshPending = EFalse;
    iRefreshError = EFalse;

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
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEMESSHANDLER_CMMPHONEMESSHANDLER, "CMmPhoneMessHandler::~CMmPhoneMessHandler" );

    // Delete iStoreInfo and set it to NULL, if it exist.
    if( iStoreInfo )
        {
        delete iStoreInfo;
        }

    // Delete iSSTFileData and set it to NULL, if it exist.
    if( iSSTFileData )
        {
        // Delete object
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_SUBSCRIBEEVENTSFROMPHONET, "CMmPhoneMessHandler::SubscribeEventsFromPhoNet" );

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

        // UICC SERVER INDICATIONS
            PN_UICC, UICC_CARD_IND,
            PN_UICC, UICC_IND,

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
        const CMmDataPackage* aDataPackage // Data package
        )
    {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::ExtFuncL. Licensee specific implemtantion. IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, CMMPHONEMESSHANDLER_EXTFUNCL, "CMmPhoneMessHandler::ExtFuncL;Licensee specific implemtantion aIpc=%d", aIpc );

    //*************************************************************//
    // NOTE.
    //
    // LICENSEE SPECIFIC MESSAGE HANDLER IMPLEMENTATION STARTS HERE
    //
    //*************************************************************//

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
            aDataPackage->UnPackData( alsLine );
            ret = UiccWriteDynamicFlagsReq( alsLine );
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
        default:
            {
TFLOGSTRING2("TSY: CMmPhoneMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP7_CMMPHONEMESSHANDLER_EXTFUNCL, "CMmPhoneMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
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
        const TIsiReceiveC &aIsiMessage  // ISI message received
        )
    {
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

TFLOGSTRING3("TSY: CMmPhoneMessHandler::ReceiveMessageL, Resource: %d, Message: %d", resource, messageId );
OstTraceExt2( TRACE_NORMAL, CMMPHONEMESSHANDLER_RECEIVEMESSAGEL, "CMmPhoneMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch( resource )
        {
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case PN_INFO:
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        case PN_MODEM_INFO:
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            {
            switch( messageId )
                {
                case INFO_SERIAL_NUMBER_READ_RESP:
                    {
                    InfoSerialNumberReadResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ReceiveMessageL, switch resource - case PN_MODEM_INFO, switch messageId - default.\n" );
OstTrace0( TRACE_NORMAL, DUP4_CMMPHONEMESSHANDLER_RECEIVEMESSAGEL, "CMmPhoneMessHandler::ReceiveMessageL, switch resource - case PN_MODEM_INFO, switch messageId - default" );
                    break;
                    }
                }
            break; // end case PN_MODEM_INFO
            }
        default:
            {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ReceiveMessageL, switch resource - default.\n" );
OstTrace0( TRACE_NORMAL, DUP5_CMMPHONEMESSHANDLER_RECEIVEMESSAGEL, "CMmPhoneMessHandler::ReceiveMessageL, switch resource - default" );
            break;
            }
        } // End of switch
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::InfoSerialNumberReadReq
// Constructs INFO_SERIAL_NUMBER_READ_REQ ISI message..
// Returns KErrNone on success, other error value on failure.
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::InfoSerialNumberReadReq
        (
        TUint8 aTransactionId, // transaction id
        TUint8 aTarget // type of requested serial number
        )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::InfoSerialNumberReadReq.");
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_INFOSERIALNUMBERREADREQ, "CMmPhoneMessHandler::InfoSerialNumberReadReq" );

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

    // Get status
    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + INFO_SERIAL_NUMBER_READ_RESP_OFFSET_STATUS ) );

TFLOGSTRING2("TSY: CMmPhoneMessHandler::InfoSerialNumberReadResp, status=0x%02x",status);
OstTraceExt1( TRACE_NORMAL, CMMPHONEMESSHANDLER_INFOSERIALNUMBERREADRESP, "CMmPhoneMessHandler::InfoSerialNumberReadResp;status=%hhx", status );

    // If status is Ok get information from possible sub block(s).
    if ( INFO_OK == status )
        {
        // If sub blocks
        if ( 0 != aIsiMessage.Get8bit(
            ISI_HEADER_SIZE +
            INFO_SERIAL_NUMBER_READ_RESP_OFFSET_SUBBLOCKCOUNT ) )
            {
            TUint sbSerialNumberStartOffset( 0 );

            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_INFO_SERIAL_NUMBER_READ_RESP,
                INFO_SB_SN_IMEI_PLAIN,
                EIsiSubBlockTypeId8Len8,
                sbSerialNumberStartOffset ) )
                {
                // Read the string length - the zero terminator...
                TUint8 strLength( aIsiMessage.Get8bit(
                    sbSerialNumberStartOffset +
                    INFO_SB_SN_IMEI_PLAIN_OFFSET_STRLEN ) - 1 );
                // ...and compare it to the available buffer size
                // (50 bytes in etel),
                // and choose the shorter one strLength =
                // ( RMobilePhone::KPhoneSerialNumberSize > strLength )?
                // strLength : RMobilePhone::KPhoneSerialNumberSize;

                TUint8 dataOffset(
                    sbSerialNumberStartOffset +
                    INFO_SB_SN_IMEI_PLAIN_OFFSET_IMEIPLAINU8 );

                serialData.Append( aIsiMessage.GetData(
                    dataOffset,
                    strLength ).Left( KSerialNumberLength ) );

                err = KErrNone;
                }
            }
        }

    // Complete to CommonTSY, ONLY if all data has already been received
    // packed parameter: TBuf8<KSerialNumberLength> serialData
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCREADDYNAMICFLAGSREQ, "CMmPhoneMessHandler::UiccReadDynamicFlagsReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadDynamicFlags;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.fileId = KElemFileDynFlagsOrange;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;
    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( KOrangeDedicatedFile >> 8 );
    params.filePath.Append( KOrangeDedicatedFile );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccWriteDynamicFlagsReq
// Write dynamic flags
// --------------------------------------------------------------------------
//
TInt CMmPhoneMessHandler::UiccWriteDynamicFlagsReq( TUint8 aInfo )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccWriteDynamicFlagsReq");
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCWRITEDYNAMICFLAGSREQ, "CMmPhoneMessHandler::UiccWriteDynamicFlagsReq" );

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
        // Set parameters for UICC_APPL_CMD_REQ message
        TUiccWriteTransparent params;
        params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        params.trId = ETrIdWriteDynamicFlags;
        params.dataOffset = 0;
        params.fileId = KElemFileDynFlagsOrange;
        params.fileIdSfi = UICC_SFI_NOT_PRESENT;
        params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;

        // File id path
        params.filePath.Append( KMasterFileId >> 8 );
        params.filePath.Append( KMasterFileId );
        params.filePath.Append( KOrangeDedicatedFile >> 8 );
        params.filePath.Append( KOrangeDedicatedFile );

        params.fileData.Append( line );
        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCREADDYNAMICFLAGSRESP, "CMmPhoneMessHandler::UiccReadDynamicFlagsResp" );

    RMobilePhone::TMobilePhoneALSLine alsLine
        ( RMobilePhone::EAlternateLineUnknown );

    TInt ret( KErrNone );
    if ( UICC_STATUS_OK == aStatus )
        {
        // Dynamic flags byte: if bit 0 is 1, it is line 1, else line 2
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCWRITEDYNAMICFLAGSRESP, "CMmPhoneMessHandler::UiccWriteDynamicFlagsResp" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCCSPFILEREQ, "CMmPhoneMessHandler::UiccCspFileReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadCsp;
    params.dataAmount = 18;
    params.dataOffset = 0; // Read from first byte
    params.fileId = KElemFileCustomerServiceProfile;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCCSPFILERESP, "CMmPhoneMessHandler::UiccCspFileResp" );

    RMobilePhone::TMobilePhoneCspFileV1 cspFileEtel;

    if ( aStatus == KErrNone )
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
        } // End of if ( aStatus == KErrNone )
    else
        {
        aStatus = KErrNotFound;
        }

    // Complete with CSP data and error code
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCIMSIREQ, "CMmPhoneMessHandler::UiccImsiReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadImsi;
    params.dataAmount = 9; // Length + IMSI = 9 bytes
    params.dataOffset = 0; // Read from first byte
    params.fileId = KElemFileImsi;
    params.fileIdSfi = 7;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// --------------------------------------------------------------------------
// CMmPhoneMessHandler::UiccImsiResp
// Complete IMSI
// --------------------------------------------------------------------------
//
void CMmPhoneMessHandler::UiccImsiResp( TInt aStatus, const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneMessHandler::UiccImsiResp" );
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCIMSIRESP, "CMmPhoneMessHandler::UiccImsiResp" );

    TInt err( KErrNone );
    TBuf8<RMobilePhone::KIMSISize> imsiData;
    if ( UICC_STATUS_OK == aStatus )
        {
        TUint8 i( 0 );
        TUint8 valueMSB( 0 );
        TUint8 valueLSB( 0 );
        // Get length of IMSI (the first byte)
        TUint8 lengthOfImsi( aFileData[0] );
        // Get the first IMSI number: MSB semioctet of byte 2
        valueMSB = static_cast<TUint8>( aFileData[1] >> 4 );
        // Check the validity
        if ( KImsiSize >= lengthOfImsi && 10 > valueMSB  )
            {
            imsiData.AppendNum( valueMSB, EDecimal);
            }
        else
            {
            err = KErrCorrupt;
            }

        if ( KErrNone == err )
            {
            // Check and append the rest of IMSI numbers
            for ( i = 2; i <= lengthOfImsi; i++ )
                {
                valueLSB = static_cast<TUint8>( aFileData[i] & 0x0F );
                valueMSB = static_cast<TUint8>( aFileData[i] >> 4 );

                // If both values are valid
                if ( 10 > valueLSB && 10 > valueMSB )
                    {
                    imsiData.AppendNum( valueLSB, EDecimal);
                    imsiData.AppendNum( valueMSB, EDecimal);
                    }
                // Last nibble is unused
                else if( 10 > valueLSB && 0xF == valueMSB )
                    {
                    imsiData.AppendNum( valueLSB, EDecimal);
                    break;
                    }
                // Either is invalid
                else
                    {
                    err = KErrCorrupt;
                    break;
                    }
                }
            }
        }
    else
        {
        err = KErrNotFound;
        }

    // Complete with packed parameter
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCREADSERVICEPROVIDERNAME, "CMmPhoneMessHandler::UiccReadServiceProviderName" );

    // Read SIM file '6F46', Service Provider Name
    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadServiceProviderName;
    // Read all 17 bytes
    params.dataAmount = 17;
    params.dataOffset = 0;
    params.fileId = KElemFileServiceProviderName;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCREADSERVICEPROVIDERNAMERESP, "CMmPhoneMessHandler::UiccReadServiceProviderNameResp" );

    TInt ret( KErrNone );
    if ( KErrNone == aStatus )
        {
        // Store data and read SPN display info
        ret = UiccProcessServiceTypeCheck( aFileData );
        }

    if ( KErrNone != aStatus || KErrNone != ret )
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
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY: CMmPhoneMessHandler::ProcessUiccMsg, transaction ID: %d, status %d", aTraId, aStatus );
OstTraceExt2( TRACE_NORMAL, CMMPHONEMESSHANDLER_PROCESSUICCMSG, "CMmPhoneMessHandler::ProcessUiccMsg;aTraId=%d;aStatus=%d", aTraId, aStatus );

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
        default:
            {
TFLOGSTRING("TSY: CMmPhoneMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEMESSHANDLER_PROCESSUICCMSG, "CMmPhoneMessHandler::ProcessUiccMsg - unknown transaction ID" );
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCPROCESSSERVICETYPECHECK, "CMmPhoneMessHandler::UiccProcessServiceTypeCheck" );

    // Copy service provider name, starting from byte 2
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

    // PLMN and SPN control information summed up to iDisplayReq field
    iServiceProviderName.iDisplayReq =
        iServiceProviderName.iDisplayReq + iSecondDisplayCondition;
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccProcessServiceTypeCheck - display condition: %d", (TUint8)iServiceProviderName.iDisplayReq);

    // Buffer for service provider name
    TBuf16<KSpnFileSize> tempBuf;
    TIsiUtility::CopyFromBigEndian( spnOutputBuffer, tempBuf );
    // Copy service provider name
    iServiceProviderName.iSPName.Copy( tempBuf );
TFLOGSTRING2("TSY: CMmPhoneMessHandler::UiccProcessServiceTypeCheck - SPN Name: %S", &iServiceProviderName.iSPName);


    // We still need to get the PLMN list to complete the information.
    // Read the SIM file EF SPDI ('6FCD')
    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadServiceProviderDisplayInfo;
    // Read all the data
    params.dataAmount = 0;
    params.dataOffset = 0;
    params.fileId = KElemFileServiceProviderDisplayInfo;
    params.fileIdSfi = 0x1B;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
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
OstTrace0( TRACE_NORMAL, CMMPHONEMESSHANDLER_UICCPROCESSSPNNAMEINFO, "CMmPhoneMessHandler::UiccProcessSpnNameInfo" );

    TInt numOfPlmns( 0 );
    TInt lengthOfDataInBytes( 0 );
    TInt i( 0 ); // Index used of data extracting
    if ( KErrNone == aStatus )
        {
        TInt lengthOfLengthInBytes( 0 );

        // Display info is coded in TLV format in USIM.
        // Check the tag
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

            // Check the tag
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
    if ( KErrNone == aStatus )
        {
        // Number of PLMNs cannot exceed 170
        if ( 170 < numOfPlmns )
            {
            numOfPlmns = 170;
            }

        // At first append number of PLMNs and second display condition
        TUint16 word( static_cast<TUint16>(
                numOfPlmns << 8 | iSecondDisplayCondition ) );
        iServiceProviderName.iPLMNField.Append( word );

        // Copy PLMNs to 16 bit buffer
        for ( TUint8 j( 0 ); j < lengthOfDataInBytes / 2; j++, i += 2 )
            {
            // PLMN entries are copied to 16-bit buffer as follows:
            // 1st byte to 8 LSB bits of 16-bit buffer
            // 2nd byte to 8 MSB bits of 16-bit buffer
            word = static_cast<TUint16>(
                ( aFileData[i+1] << 8 ) | aFileData[i] );
            iServiceProviderName.iPLMNField.Append( word );
            }
        // Last word is added
        iServiceProviderName.iPLMNField.Append( ( 0xFF << 8 ) | aFileData[i] );

        // Complete SPN info
        CMmDataPackage dataPackage;
        dataPackage.PackData( &iServiceProviderName );
        iMessageRouter->Complete(
            EMobilePhoneGetServiceProviderName,
            &dataPackage,
            KErrNone );
        }
    else // Complete error without data
        {
        iMessageRouter->Complete(
             EMobilePhoneGetServiceProviderName,
             KErrNotFound );
        }

    // Reset iServiceProviderName for next time.
    iServiceProviderName.iSPName.Zero();
    iServiceProviderName.iPLMNField.Zero();
    }

// End of file
