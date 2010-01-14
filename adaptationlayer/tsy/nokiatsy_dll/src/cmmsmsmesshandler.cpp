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



//  EXTERNAL RESOURCES

//  Include Files
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
// Temporarily removed for Bridge camp!
//#include <ctsy/serviceapi/ctsysatmessagingbase.h>
#include <etelsat.h>  // for sat mo sm control error values
#include <ctsy/serviceapi/gsmerror.h> // for sat mo sm control error values
#include <etelmmerr.h>
#include <tisi.h>
#include <smsisi.h>
#include <gsmuelem.h> // for tsmsfirstoctet
#include <ctsy/rmmcustomapi.h>
#include "cmmsmsmesshandler.h"
#include "cmmnetmesshandler.h"
#include "cmmsmsgsmaddress.h"
#include "cmmstaticutility.h"
#include "cmmmessagerouter.h"
#include "cmmphonemesshandler.h"
#include "tsylogger.h"
#include "cmmphonetsender.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsmsmesshandlerTraces.h"
#endif

//  External Data Structures
//  none

//  External Function Prototypes
//  none

//  LOCAL CONSTANTS AND MACROS
const TUint8 KSmsPadding( 0x00 ); // Filler byte for ISI messages
// const TUint8 KUnicodeCharLength( 2 ); Compiler warning removal

// The following constanst are for extracting information from TPDU
// See 3GPP 23.040.

// For submit and command types
const TUint8 KTpduIndexMessageParameters( 0 );
const TUint8 KTpduIndexMessageReference( 1 );

// SMS submit type specific costanst
const TUint8 KTpduSubmitIndexDestinationAddressLength( 2 );
const TUint8 KTpduSubmitIndexProtocolIdentifier( 4 );
const TUint8 KTpduSubmitIndexDataCodingScheme( 5 );
const TUint8 KTpduSubmitIndexValidityperiod( 6 );
// User data length index when no validity period
const TUint8 KTpduSubmitIndexUserDataLengthIfNoTpVp( 6 );
// User data length index when validity period length is 1
const TUint8 KTpduSubmitIndexUserDataLengthVpfRelative( 7 );
// User data length index when validity period length is 7
const TUint8 KTpduSubmitIndexUserDataLengthVpfAbsoluteEnchanced( 13 );

// SMS command type specific constants
const TUint8 KTpduCommandIndexProtocolIdentifier( 2 );
const TUint8 KTpduCommandIndexType( 3 );
const TUint8 KTpduCommandIndexMessageNumber( 4 );
const TUint8 KTpduCommandIndexDestinationAddressLength( 5 );
const TUint8 KTpduCommandIndexUserDataLength( 7 );

// const TUint8 KSizeOfAlphaTag( 34 ); Compiler warning removal
const TUint8 KSmsScTimeStampMaxLength( 7 );

// Constanst for SMS parameters
const TUint8 KSmsMandatoryParamsLength( 28 );
const TUint8 KSmsMaxAddressLength( 12 );
const TUint8 KSmsParamsParamIndicatorOffset( 0 );
const TUint8 KSmsParamsDestAddressOffset( 1 );
const TUint8 KSmsParamsScAddressOffset( 13 );
const TUint8 KSmsParamsProtocolIdOffset( 25 );
const TUint8 KSmsParamsDcsOffset( 26 );
const TUint8 KSmsParamsValidityPeriodOffset( 27 );
const TUint8 KSizeOfAlphaTag( 62 );

//  MODULE DATA STRUCTURES

//  Local Data Structures
//  none

//  Local Function Prototypes
//  none

//  LOCAL FUNCTIONS
//  none

//  MEMBER FUNCTIONS

//=============================================================================


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::CMmSmsMessHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmSmsMessHandler::CMmSmsMessHandler()
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::CMmSmsMessHandler: constructor.");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_CMMSMSMESSHANDLER, "CMmSmsMessHandler::CMmSmsMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::CMmSmsMessHandler
// C++ default destructor
// -----------------------------------------------------------------------------
//
CMmSmsMessHandler::~CMmSmsMessHandler()
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::CMmSmsMessHandler: destructor.");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_CMMSMSMESSHANDLER, "CMmSmsMessHandler::~CMmSmsMessHandler" );
    // Delete arrays
    if( iSmsListArray )
        {
        iSmsListArray->ResetAndDestroy();
        delete iSmsListArray;
        }

    if( iSmspListArray )
        {
        iSmspListArray->ResetAndDestroy();
        delete iSmspListArray;
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::NewL
// Creates a new Gsm specific SmsMessageHandler object instance
// -----------------------------------------------------------------------------
//
CMmSmsMessHandler* CMmSmsMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender,      // pointer to PhoNet sender
    CMmPhoNetReceiver* aPhoNetReceiver,  // pointer to PhoNet receiver
    CMmMessageRouter* aMessageRouter,    // pointer to Message router
    CMmUiccMessHandler* aUiccMessHandler // pointer to UICC message handler
    )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_NEWL, "CMmSmsMessHandler::NewL" );
    CMmSmsMessHandler* smsMessHandler = new ( ELeave ) CMmSmsMessHandler();

    CleanupStack::PushL( smsMessHandler );
    smsMessHandler->iPhoNetSender = aPhoNetSender;
    smsMessHandler->iMessageRouter = aMessageRouter;
    smsMessHandler->iMmUiccMessHandler = aUiccMessHandler;
    smsMessHandler->ConstructL();
    aPhoNetReceiver->RegisterL( smsMessHandler, PN_SMS );
    CleanupStack::Pop( smsMessHandler );

    return smsMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::ConstructL
// Initialises object attributes
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_CONSTRUCTL, "CMmSmsMessHandler::ConstructL" );
    iSmsListArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
    iSmspListArray = new ( ELeave ) CArrayPtrFlat<TSmsParameters>( 1 );

    iReceivedClass2ToBeReSent = EFalse;
    // default bearer setting is "CS preferred"
    iMobileSmsBearer = RMobileSmsMessaging::ESmsBearerCircuitPreferred;

    iMemoryCapacityExceeded = EFalse;

    // Reading of SMSP entries starts from record 1
    iSmspRecordNumber = 1;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::ReceiveMessageL
// Called by PhonetReceiver when an ISI message has been received
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::ReceiveMessageL( const TIsiReceiveC& aIsiMessage )
    {
    TUint8 resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TUint8 messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

TFLOGSTRING3("TSY: CMmSmsMessHandler::ReceiveMessageL - resource: %d, msgId: %d", resource, messageId);
OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_RECEIVEMESSAGEL,"CMmSmsMessHandler::ReceiveMessageL;resource=%hhu;messageId=%hhu", resource, messageId );

    switch ( resource )
        {
        case PN_SMS:
            {
            switch( messageId )
                {
                case SMS_MESSAGE_SEND_RESP:
                    {
                    // check transaction id type
                    TUint8 traId( aIsiMessage.Get8bit(
                        ISI_HEADER_OFFSET_TRANSID ) );
                    if ( ESmsMessagingSendMessage == traId )
                        {
                        // SMS Messaging-originated SendSMS
                        SmsMessageSendResp( aIsiMessage,
                            EMobileSmsMessagingSendMessage );
                        }
                    else if ( ESmsMessagingSendNoFdnMessage == traId )
                        {
                        SmsMessageSendResp( aIsiMessage,
                            EMobileSmsMessagingSendMessageNoFdnCheck );
                        }
                    else if ( ESmsMessagingSendSatMessage == traId )
                        {
                        // SAT-originated SendSMS
                        SmsMessageSendResp( aIsiMessage,
                            EMmTsySmsSendSatMessage );
                        }
                    else
                        {
TFLOGSTRING2("TSY:CMmSmsMessHandler::ReceiveMessageL:Unexpected transId=%d in SMS_MESSAGE_SEND_RESP.",traId);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_RECEIVEMESSAGEL, "CMmSmsMessHandler::ReceiveMessageL;traId=%hhu", traId );
                        }
                    break;
                    }
                case SMS_RECEIVE_MESSAGE_RESP:
                    {
                    SmsReceiveMessageResp( aIsiMessage );
                    break;
                    }
                case SMS_RECEIVED_MSG_IND:
                    {
                    SmsReceivedMsgInd( aIsiMessage );
                    break;
                    }
                case SMS_RECEIVED_MSG_REPORT_RESP:
                    {
                    SmsReceivedMsgReportResp( aIsiMessage );
                    break;
                    }
                case SMS_SETTINGS_UPDATE_RESP :
                    {
                    SmsSettingsUpdateResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::ReceiveMessageL - PN_SMS - unknown msgId: %d", messageId);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_RECEIVEMESSAGEL, "CMmSmsMessHandler::ReceiveMessageL;messageId=%hhu", messageId );
                    // No handler methods for ISI-message found.
                    break;
                    }
                }
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmSmsMessHandler::ReceiveMessageL - unknown resource: %d", resource);
OstTraceExt1( TRACE_NORMAL, DUP6_CMMSMSMESSHANDLER_RECEIVEMESSAGEL, "CMmSmsMessHandler::ReceiveMessageL;resource=%hhu", resource );
            // No handler methods for ISI-message found.
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsMessageSendReq
// Creates request for SMS message sending
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::SmsMessageSendReq(
    TUint8 aTransactionId,              // Transaction ID
    const CMmDataPackage* aDataPackage, // Packed data
    TBool aSmsCheckDisableFdn )         // Add SMS_SB_CHECK_INFO with
                                        // SMS_CHECK_DISABLE_FDN

    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsMessageSendReq. Transaction ID: %d", aTransactionId);
OstTraceExt1( TRACE_NORMAL,DUP18_CMMSMSMESSHANDLER_SMSMESSAGESENDREQ,"CMmSmsMessHandler::SmsMessageSendReq;aTransactionId=%hhu", aTransactionId );

    TInt ret( KErrNone );
    TUint8 subBlockId( 0 );

    // Structure for all SMS parameters and user data
    TSendSmsDataAndAttributes sendData;
    // Structure for message attributes
    RMobileSmsMessaging::TMobileSmsSendAttributesV1* msgAttr;
    // Buffer for TPDU
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> msgData;

    aDataPackage->UnPackData( sendData );
    msgAttr = sendData.iAttributes;

    // Check TPDU length
    if ( RMobileSmsMessaging::KGsmTpduSize >= sendData.iMsgData->Length() )
        {
        msgData.Append( *sendData.iMsgData );

        // Get TP-MTI (TP-Message-Type-Indicator) from first octet of TPDU
        TUint8 tpMti( msgData[KTpduIndexMessageParameters] &
            TSmsFirstOctet::ESmsMTIMask );
        if ( TSmsFirstOctet::ESmsMTISubmitOrSubmitReport == tpMti )
            {
            subBlockId = SMS_SB_SUBMIT;
            }
        else if ( TSmsFirstOctet::ESmsMTIStatusReportOrCommand == tpMti )
            {
            subBlockId = SMS_SB_COMMAND;
            }
        else // Message type not supported
            {
            ret = CMmStaticUtility::EpocErrorCode(
                KErrArgument,
                KErrGsmSMSTpduNotSupported );
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsMessageSendReq. Error! SMS type not supported: %d", tpMti);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_SMSMESSAGESENDREQ, "CMmSmsMessHandler::SmsMessageSendReq.Error! SMS type not supported;tpMti=%hhu", tpMti );
            }
        }
    else
        {
        // TPDU too long
        ret = CMmStaticUtility::EpocErrorCode(
            KErrArgument,
            KErrGsmSMSTpduNotSupported );
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsMessageSendReq. Error! TPDU too long: %d", sendData.iMsgData->Length());
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSMESSAGESENDREQ, "CMmSmsMessHandler::SmsMessageSendReq.Error! TPDU too long;sendData.iMsgData->Length()=%d", sendData.iMsgData->Length() );
        }

    // Check the format of TPDU and data buffer
    if ( ! ( RMobileSmsMessaging::KSmsDataFormat & msgAttr->iFlags &&
        RMobileSmsMessaging::EFormatGsmTpdu & msgAttr->iDataFormat ) )
        {
        ret = CMmStaticUtility::EpocErrorCode(
            KErrArgument,
            KErrGsmSMSTpduNotSupported );
TFLOGSTRING3("TSY: CMmSmsMessHandler::SmsMessageSendReq. Error! Invalid TPDU format (%d) or data buffer format (%d)", msgAttr->iFlags, msgAttr->iDataFormat );
OstTraceExt2( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_SMSMESSAGESENDREQ, "CMmSmsMessHandler::SmsMessageSendReq.Error!;msgAttr->iFlags=%d;msgAttr->iDataFormat=%hhu", msgAttr->iFlags, msgAttr->iDataFormat );
        }

#if (NCP_COMMON_S60_VERSION_SUPPORT>S60_VERSION_32)
    // Check whether there is SMS sending ongoing.
    if ( iSMSSendingOngoing )
        {
        ret = KErrServerBusy;
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsMessageSendReq. SMS sending failed, server busy!" );
OstTrace0( TRACE_NORMAL, DUP17_CMMSMSMESSHANDLER_SMSMESSAGESENDREQ, "CMmSmsMessHandler::SmsMessageSendReq. SMS sending failed, server busy!" );
        }
#endif // NCP_COMMON_S60_VERSION_SUPPORT

    if ( KErrNone == ret )
        {
        // Create and send SMS_MESSAGE_SEND_REQ message with needed subblocks
        ret = CreateSmsMessageSendReq(
            aTransactionId,
            msgAttr,
            msgData,
            subBlockId,
            aSmsCheckDisableFdn );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::CreateSmsMessageSendReq
// Construct a SMS_MESSAGE_SEND_REQ ISI message with needed subblocks
// for SMS message send request to SMS server
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::CreateSmsMessageSendReq(
    TUint8 aTransactionId,
    RMobileSmsMessaging::TMobileSmsSendAttributesV1* aMsgAttr,
    const TDesC8& aMsgData,
    TUint8 aSubblockId,
    TBool aSmsCheckDisableFdn )         // Add SMS_SB_CHECK_INFO with
                                        // SMS_CHECK_DISABLE_FDN

    {
    // Structure of the message, depending on the TP-MTI:
    // SMS_MESSAGE_SEND_REQ (8 bytes + sub blocks, total max. 212)
    //  |
    //  |- SMS_SB_SUBMIT (mandatory, 8 bytes)
    //  |
    //  |- SMS_SB_ADDRESS (destination address, mandatory, max. 20 bytes)
    //  |
    //  |- SMS_SB_ADDRESS (service center address, mandatory, max. 20 bytes)
    //  |
    //  |- SMS_SB_USER_DATA (optional, max. 148 bytes)
    //  |
    //  |- SMS_SB_VALIDITY_PERIOD (optional, max. 12 bytes)
    //
    // or
    //
    // SMS_MESSAGE_SEND_REQ (8 bytes + sub blocks, total max. 220)
    //  |
    //  |- SMS_SB_COMMAND (mandatory, 12 bytes)
    //  |
    //  |- SMS_SB_ADDRESS (destination address, mandatory, max. 20 bytes)
    //  |
    //  |- SMS_SB_ADDRESS (service center address, mandatory, max. 20 bytes)
    //  |
    //  |- SMS_SB_USER_DATA (optional, max. 174 bytes)

TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. Subblock ID: %d", aSubblockId);
OstTraceExt1( TRACE_NORMAL, DUP9_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq;aSubblockId=%hhu", aSubblockId );

    TInt ret( KErrNone );
    TUint8 addressLength( 0 );
    ret = GetDestAddressLength( aMsgData, aSubblockId, addressLength );

    // Destination address is OK. Create ISI message and needed subblocks
    if ( KErrNone == ret )
        {
        TUint8 moreMsgToSend( 0 );
        TUint8 smsRoute( SMS_ROUTE_DEFAULT );
        TUint8 tpVpLength( 0 );
        TUint8 tpVpf( 0 );
        TUint8 tpVpSubblockLength( SIZE_SMS_SB_VALIDITY_PERIOD );
        TUint8 tpUdl( 0 );
        TUint8 tpUserDataLengthIndex( KTpduSubmitIndexUserDataLengthIfNoTpVp );
        TUint8 tpUserDataIndex( 0 );
        TUint8 tpDaIndex( KTpduSubmitIndexDestinationAddressLength );
        TUint8 tpDcs( 0 );
        TUint8 msgOffset( ISI_HEADER_SIZE + SIZE_SMS_MESSAGE_SEND_REQ );
        TUint8 numOfSubblocks( 3 ); // 3 mandatory subblocks
        TUint8 lengthOfAddressSb( 0 );
        TUint8 lengthOfSMSUserDataSb( 0 );
        TBool defaultAlphabet( EFalse ); // Data coding type, true if 7-bit

        // Check if more to send parameter is included
        if ( RMobileSmsMessaging::KMoreToSend & aMsgAttr->iFlags )
            {
            moreMsgToSend = static_cast<TUint8>( aMsgAttr->iMore );
            }

        // Set the bearer
        if ( RMobileSmsMessaging::ESmsBearerPacketOnly == iMobileSmsBearer )
            {
            smsRoute = SMS_ROUTE_PS;
            }
        else if ( RMobileSmsMessaging::ESmsBearerCircuitOnly ==
            iMobileSmsBearer )
            {
            smsRoute = SMS_ROUTE_CS;
            }
        // No else. SMS_ROUTE_DEFAULT is used. The route is used that is
        // available and preferred by PP-bits or prior client settings

        // Create SMS_MESSAGE_SEND_REQ message
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SMS );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_MESSAGE_SEND_REQ_OFFSET_TRANSID,
            aTransactionId );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_MESSAGE_SEND_REQ_OFFSET_MESSAGEID,
            SMS_MESSAGE_SEND_REQ );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_MESSAGE_SEND_REQ_OFFSET_MOREMESSAGESTOSEND,
            moreMsgToSend );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_MESSAGE_SEND_REQ_OFFSET_SMSROUTE,
            smsRoute );
        // Set repeatedMessage to FALSE. Tsy doesn't know if message is tried
        // to send first or second time.
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_MESSAGE_SEND_REQ_OFFSET_REPEATEDMESSAGE,
            EFalse );
        // Filler, two bytes
        isiMsg.Set16bit(
            ISI_HEADER_SIZE + SMS_MESSAGE_SEND_REQ_OFFSET_FILLERBYTE1,
            0 );

TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_MESSAGE_SEND_REQ created. Message offset: %d", msgOffset );
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. More messages to be sent: %d", moreMsgToSend);
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS route: %d", smsRoute);
OstTraceExt3( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq;msgOffset=%hhu;moreMsgToSend=%hhu;smsRoute=%hhu", msgOffset, moreMsgToSend, smsRoute );

        // Create SMS_SB_SUBMIT subblock
        if ( SMS_SB_SUBMIT == aSubblockId )
            {
            BuildSmsSbSubmit( aMsgData, isiMsg, msgOffset, addressLength );
            msgOffset += SIZE_SMS_SB_SUBMIT;
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_SUBMIT created. Message offset: %d", msgOffset );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_SUBMIT created.;msgOffset=%hhu", msgOffset );
            // Set needed TPDU index values that depends on SMS type, VP existence
            // and destination address length. Those parameters are needed when
            // creating subblocks SMS_DESTINATION_ADDRESS, SMS_SB_VALIDITY_PERIOD
            // and SMS_SB_USER_DATA.
            // TP-VPF is LSB bits 3 and 4 from the first octect of TPDU
            tpVpf = ( aMsgData[KTpduIndexMessageParameters] >> 3 ) & 3;
            if ( tpVpf )
                {
                // Relative format (tpVpf == 2).
                // Subblock length SIZE_SMS_SB_VALIDITY_PERIOD by default
                if ( 2 == tpVpf)
                    {
                    tpVpLength = SMS_VPF_RELATIVE;
                    tpUserDataLengthIndex =
                        KTpduSubmitIndexUserDataLengthVpfRelative;
                    }
                // Absolute or enhanced format, subblock length is 12
                else
                    {
                    tpVpLength = SMS_VPF_ABSOLUTE_OR_ENHANCED;
                    tpVpSubblockLength = 12;
                    tpUserDataLengthIndex =
                        KTpduSubmitIndexUserDataLengthVpfAbsoluteEnchanced;
                    }
                }
                // No else. If no TP-VP, User data length index was set to
                // 'KTpduSubmitIndexUserDataLengthIfNoTpVp' by default
            }
        else // Create SMS_SB_COMMAND subblock
            {
            BuildSmsSbCommand( aMsgData, isiMsg, msgOffset );
            msgOffset += SIZE_SMS_SB_COMMAND;
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_COMMAND created. Message offset: %d", msgOffset );
OstTraceExt1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_COMMAND created.;msgOffset=%hhu", msgOffset );

            tpDaIndex = KTpduCommandIndexDestinationAddressLength;
            tpUserDataLengthIndex = KTpduCommandIndexUserDataLength;
            }

TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. Validity period format: %d", tpVpf);
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. User data length index: %d", tpUserDataLengthIndex);
OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq;tpVpf=%hhu;tpUserDataLengthIndex=%hhu", tpVpf, tpUserDataLengthIndex );

        // Create SMS_SB_ADDRESS subblock for destination address
        // TP-DA length = Length (1 byte) + TON/NPI (1 byte) + address data
        TPtrC8 addressData( aMsgData.Mid( tpDaIndex, ( addressLength + 2 ) ) );
        lengthOfAddressSb = BuildSmsSbAddress(
            addressData,
            isiMsg,
            SMS_DESTINATION_ADDRESS,
            msgOffset );
        msgOffset += lengthOfAddressSb;
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_ADDRESS created for destination address. Message offset: %d", msgOffset );
OstTraceExt1( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_ADDRESS created for destination address.;msgOffset=%hhu", msgOffset );

        // Create SMS_SB_ADDRESS subblock for service center address
        TBuf8<SMS_ADDRESS_MAX_LEN> scAddr;
        BuildScAddress( aMsgAttr->iGsmServiceCentre, scAddr );
        lengthOfAddressSb =
            BuildSmsSbAddress( scAddr, isiMsg, SMS_SMSC_ADDRESS, msgOffset );
        msgOffset += lengthOfAddressSb;
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_ADDRESS created for SC address. Message offset: %d", msgOffset );
OstTraceExt1( TRACE_NORMAL, DUP5_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq.SMS_SB_ADDRESS created for SC address.;msgOffset=%hhu", msgOffset );

        // Create SMS_SB_VALIDITY_PERIOD subblock (optional, used only in
        // case of SMS_SB_SUBMIT)
        if ( SMS_SB_SUBMIT == aSubblockId && tpVpf )
            {
            TUint8 tpVpIndex(
                KTpduSubmitIndexValidityperiod + addressLength );
            BuildSmsSbValidityPeriod(
                aMsgData,
                isiMsg,
                tpVpIndex,
                tpVpLength,
                msgOffset );
            // Udate message offset according to validity period length
            msgOffset += tpVpSubblockLength;
            numOfSubblocks++;
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_VALIDITY_PERIOD created. Message offset: %d", msgOffset );
OstTraceExt1( TRACE_NORMAL, DUP6_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq.SMS_SB_VALIDITY_PERIOD created.;msgOffset=%hhu", msgOffset );
            }

        // Create SMS_SB_USER_DATA subblock if user data exists
        // Destination address length must be added to get user data
        // length index
        tpUserDataLengthIndex += addressLength;
        tpUdl = aMsgData[tpUserDataLengthIndex];
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. User data length from TPDU: %d", tpUdl);
OstTrace1( TRACE_NORMAL, DUP10_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq;tpUdl=%d", tpUdl );

        if ( tpUdl )
            {
            // Actual user data is next index from TP-UDL position
            tpUserDataIndex = tpUserDataLengthIndex + 1;
            if ( SMS_SB_SUBMIT == aSubblockId )
                {
                tpDcs = aMsgData[ KTpduSubmitIndexDataCodingScheme + addressLength ];
                defaultAlphabet = IsDataCodingScheme7Bit( tpDcs );
                }
            lengthOfSMSUserDataSb = BuildSmsSbUserData(
                aMsgData,
                isiMsg,
                tpUdl,
                tpUserDataIndex,
                defaultAlphabet,
                msgOffset );
            numOfSubblocks++;
            msgOffset += lengthOfSMSUserDataSb;
TFLOGSTRING2("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_USER_DATA created. Message offset: %d", msgOffset );
OstTraceExt1( TRACE_NORMAL, DUP7_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_USER_DATA created.;msgOffset=%hhu", msgOffset );
            }

        // Create SMS_SB_CHECK_INFO subblock if user data exists
        // Destination address length must be added to get user data
        // length index
        if ( aSmsCheckDisableFdn )
            {
            BuildSmsCheckInfo(
                isiMsg,
                msgOffset );
            numOfSubblocks++;
TFLOGSTRING("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_CHECK_INFO created." );
OstTrace0( TRACE_NORMAL, DUP11_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_SB_CHECK_INFO created." );
            }


#if (NCP_COMMON_S60_VERSION_SUPPORT>S60_VERSION_32)
        iSMSSendingOngoing = ETrue;
#endif //NCP_COMMON_S60_VERSION_SUPPORT

        // Set number of subblocks and then send message via phonet
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_MESSAGE_SEND_REQ_OFFSET_SUBBLOCKCOUNT,
            numOfSubblocks );
        ret = iPhoNetSender->Send( isiMsg.Complete() );

TFLOGSTRING3("TSY: CMmSmsMessHandler::CreateSmsMessageSendReq. SMS_MESSAGE_SEND_REQ was sent. Number of subblocks in message: %d, ret = %d", numOfSubblocks, ret  );
OstTraceExt2( TRACE_NORMAL, DUP8_CMMSMSMESSHANDLER_CREATESMSMESSAGESENDREQ, "CMmSmsMessHandler::CreateSmsMessageSendReq.SMS_MESSAGE_SEND_REQ was sent.;numOfSubblocks=%hhu;ret=%d", numOfSubblocks, ret );
        } // End of if destination address is OK

    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsReceiveMessageReq
// Construct a SMS_RECEIVE_MESSAGE_REQ ISI message for activating SMS receiving
// and send it to SMS server
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::SmsReceiveMessageReq( TUint8 aAction )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsReceiveMessageReq. Action = %d",aAction );
OstTraceExt1( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSRECEIVEMESSAGEREQ, "CMmSmsMessHandler::SmsReceiveMessageReq;aAction=%hhu", aAction );

    TUint8 trId( 0 );
    if ( SMS_RECEPTION_STORAGE_STATUS_UPDATE == aAction )
        {
        trId = ESmsMessagingResumeSmsReception;
        }

    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SMS );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_RECEIVE_MESSAGE_REQ_OFFSET_TRANSID, trId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_RECEIVE_MESSAGE_REQ_OFFSET_MESSAGEID,
        SMS_RECEIVE_MESSAGE_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_RECEIVE_MESSAGE_REQ_OFFSET_ACTION, aAction );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_RECEIVE_MESSAGE_REQ_OFFSET_FILLERBYTE1,
        KSmsPadding );

    return iPhoNetSender->Send( isiMsg.Complete() );
    }

// --------------------------------------------------------------------------
// CMmSmsMessHandler::SmsReceivedMsgReportReq
// Construct a SMS_RECEIVED_MSG_REPORT_REQ ISI message sub block for received
// SMS report request to the SMS server
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::SmsReceivedMsgReportReq(
    TUint8 aTransactionId,      // Transaction identifier
    const TDesC8* aMsgData,     // A pointer to the message data
    TInt aRpCause )             // RP cause value
    {
    // Structure of the message:
    // SMS_RECEIVED_MSG_REPORT_REQ (8 bytes + sub blocks. Max. total 204 bytes)
    //  |
    //  |- SMS_SB_DELIVER_REPORT (optional, included in NACK case
    //  |                         8 bytes )
    //  |- SMS_SB_PARAM_INDICATOR (optional, included if PDU (aMsgData) is not
    //  |                          empty. Max. 20 bytes)
    //  |- SMS_SB_USER_DATA (optional, included if PDU contains user data.
    //                       Max. 8  + SMS_GSM_DELIVER_ACK_UD_MAX_LEN
    //                       + padding = 168 bytes)

TFLOGSTRING3("TSY: CMmSmsMessHandler::SmsReceivedMsgReportReq - traId: %d, RP cause value: %d", aTransactionId, aRpCause);
OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSRECEIVEDPPREPORTREQ, "CMmSmsMessHandler::SmsReceivedMsgReportReq;aTransactionId=%hhu;aRpCause=%d", aTransactionId, aRpCause );

    TInt ret( KErrNone );
    TUint8 numOfSubblocks( 0 ); // All subblocks are optional
    TUint8 tpFailureCause( 0 );
    TUint8 causeType( 0 ); // SMS_CAUSE_TYPE_COMMON
    TUint8 smsCause( 0 ); // SMS_CAUSE_OK
    TUint8 messageParameters( 0 ); // Default message parameters
    TUint8 sbOffset( ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_REPORT_REQ );
    TUint8 paramIndSbLength( SIZE_SMS_SB_PARAM_INDICATOR );
    TBool userDataIncluded( EFalse );

    // Create SMS_SB_DELIVER_REPORT. Buffer size must be 168
    // (subblock size + user data). This subblock is added to message only
    // in NACK case.
    TBuf8<SIZE_SMS_SB_DELIVER_REPORT> deliverReportBuf;
    TIsiSubBlock deliverReportSb(
        deliverReportBuf,
        SMS_SB_DELIVER_REPORT,
        EIsiSubBlockTypeId16Len16 );

    // Create SMS_SB_USER_DATA. This subblock is added to message only if
    // user data exists
    TBuf8<SIZE_SMS_SB_USER_DATA + SMS_DELIVER_ACK_UD_MAX_LEN>
        userDataBuf;
    TIsiSubBlock userDataSb(
        userDataBuf,
        SMS_SB_USER_DATA,
        EIsiSubBlockTypeId16Len16 );

    // Create SMS_SB_PARAM_INDICATOR. This subblock is added to message only
    // if user data exists
    TBuf8<20> paramIndicatorBuf;
    TIsiSubBlock paramIndicatorSb(
         paramIndicatorBuf,
         SMS_SB_PARAM_INDICATOR,
         EIsiSubBlockTypeId16Len16 );

    // If RP Cause value is other than KErrNone, values of the cause type and the
    // SMS cause must be defined. In this case we also include subblock
    // SMS_SB_DELIVER_REPORT.
    if ( KErrNone != aRpCause )
        {
        causeType = SMS_CAUSE_TYPE_EXT;
        smsCause = SmsMapCause( aRpCause );
        numOfSubblocks++;
        }

    // If message data is available
    if ( NULL != aMsgData && 0 < aMsgData->Length() )
        {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsReceivedMsgReportReq. Message data availabe, length %d", aMsgData->Length());
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSRECEIVEDMSGREPORTREQ, "CMmSmsMessHandler::SmsReceivedMsgReportReq;aMsgData->Length()=%d", aMsgData->Length() );

        TUint8 ind( 0 );

        // Message parameters = 1st byte of deliver report
        messageParameters = ( *aMsgData )[ind++];

        // Message type indicator is 2 LSB in message parameters.
        TUint8 tpMti( messageParameters & 0x03 );

        // If both bits in the TP_MTI are 0, message type is
        // SMS DELIVER REPORT (3GPP 23.040 chapter 9.2.3.1)
        if ( 0 == tpMti )
            {
            TUint8 protocolInd( 0 );
            TUint8 dcs( 0 );
            TUint8 tpUdl( 0 );
            TPtrC8 userData( 0, 0 );

            if ( 0 != aRpCause )
                {
                tpFailureCause = ( *aMsgData )[ind++];
                }

            // Lets find out how many parameter indicators are there
            TBuf8<SMS_PARAM_INDICATOR_MAX_LEN> paramIndicator;
            TUint8 paramIndCount( 1 ); // There is at least one TP-PI
            TUint8 paramInd( 0 );
            TBool moreParamInd( ETrue );

            while ( moreParamInd )
                {
                paramInd = ( *aMsgData )[ind++];
                paramIndicator.Append( paramInd );
                paramInd = paramInd >> 7;

                // If true, another TP-PI octet follows immediately afterwards
                if ( KSmsDelRepParamIndExtensionBit == paramInd )
                    {
                    paramIndCount++;
                    }
                else
                    {
                    moreParamInd = EFalse;
                    }

                // Too many TP-PI octets -> this PDU is corrupted.
                if ( SMS_PARAM_INDICATOR_MAX_LEN < paramIndCount )
                    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsReceivedMsgReportReq. Error! Too mamy TP-PI parameters: %d", paramIndCount );
OstTraceExt1( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_SMSRECEIVEDMSGREPORTREQ, "CMmSmsMessHandler::SmsReceivedMsgReportReq. Error!;paramIndCount=%hhu", paramIndCount );
                    ret = KErrArgument;
                    moreParamInd = EFalse;
                    }
                }

            // Calculate final length of SMS_SB_PARAM_INDICATOR, depends on
            // number of param indicators. Must be divisible by 4.
            if ( paramIndCount > 1)
                {
                // One param indicator has already counted
                paramIndSbLength += ( --paramIndCount );
                while( paramIndSbLength % 4 )
                    {
                    paramIndSbLength++;
                    }
                }

            // Lets take first TP-PI and check what fields this PDU includes
            paramInd = paramIndicator[0];
            TInt paramask( KSmsParametersIndMask & paramInd ); // Mask bits

            // Gather data from PDU using parameter indicator information
            if ( KSmsParametersIndProtocolId & paramask )
                {
                protocolInd = ( *aMsgData )[ind++];
                }
            if ( KSmsParametersIndDataCodingScheme & paramask )
                {
                dcs = ( *aMsgData )[ind++];
                }
            if ( KSmsParametersIndUserData & paramask )
                {
                tpUdl = ( *aMsgData )[ind++];
                }
TFLOGSTRING4("TSY: CMmSmsMessHandler::SmsReceivedMsgReportReq. protocolInd: %d, dcs: %d, tpUdl: %d", protocolInd, dcs, tpUdl );
OstTraceExt3( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_SMSRECEIVEDMSGREPORTREQ, "CMmSmsMessHandler::SmsReceivedMsgReportReq;protocolInd=%hhu;dcs=%hhu;tpUdl=%hhu", protocolInd, dcs, tpUdl );

            // If user data exists, fill SMS_SB_USER_DATA and
            // SMS_SB_PARAM_INDICATOR subblocks
            if ( 0 < tpUdl )
                {
                TUint8 userDataLengthInBytes( 0 );
                userDataIncluded = ETrue;
                numOfSubblocks += 2;

                TBool defaultAlphabet( IsDataCodingScheme7Bit( dcs ) );
                // If data is 7bit, then TP-UDL is integer reprentation of
                // the number of septets within the TP-UD field.
                if ( defaultAlphabet )
                    {
                    // Data length in bytes
                    userDataLengthInBytes = ( ( tpUdl + 1 ) * 7 ) / 8 ;
                    }
                else
                    {
                    userDataLengthInBytes = tpUdl;
                    }

                // Append filler + user data length in bytes
                userDataBuf.Append( KSmsPadding );
                userDataBuf.Append( userDataLengthInBytes );
                // Append filler + amount of characters in user data
                userDataBuf.Append( KSmsPadding );
                userDataBuf.Append( tpUdl );
                // Append user data
                userData.Set( aMsgData->Mid( ind, userDataLengthInBytes ) );
                userDataBuf.Append( userData );

                paramIndicatorBuf.Append( protocolInd );
                paramIndicatorBuf.Append( dcs );
                paramIndicatorBuf.Append( paramIndicator.Length() );
                paramIndicatorBuf.Append( paramIndicator );
                } // End of if ( 0 < tpUdl )
            } // end of if ( 0 == tpMti )
        else
        // TP-MTI is other than SMS_GSM_DELIVER_REPORT, message is not sent
            {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsReceivedMsgReportReq. Error! Invalid message type indicator: %d", tpMti);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSRECEIVEDMSGREPORTREQ, "CMmSmsMessHandler::SmsReceivedMsgReportReq;tpMti=%hhu", tpMti );
            ret = KErrArgument;
            }
        } // end of if ( NULL != aMsgData && 0 < aMsgData->Length() )
    else
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsReceivedMsgReportReq. No message data availabe!");
OstTrace0( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_SMSRECEIVEDMSGREPORTREQ, "CMmSmsMessHandler::SmsReceivedMsgReportReq. No message data available" );
        // If memory capacity exceeded -> gsmTpfailureCause is 0
        if ( 0 != aRpCause && SMS_EXT_ERR_MEMORY_CAPACITY_EXC != smsCause  )
            {
            // 0xAF is unspecified TP-command error
            tpFailureCause = 0xAF;
            }
        }

    // Send SMS_RECEIVED_MSG_REPORT_REQ
    if ( KErrNone == ret )
        {
        // Create SMS_RECEIVED_MSG_REPORT_REQ message
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SMS );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_TRANSID,
            aTransactionId );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_MESSAGEID,
            SMS_RECEIVED_MSG_REPORT_REQ );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_CAUSETYPE,
            causeType );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_SMSCAUSE,
            smsCause );
        isiMsg.Set8bit( ISI_HEADER_SIZE +
            SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_FILLERBYTE1,
            KSmsPadding );
        isiMsg.Set8bit( ISI_HEADER_SIZE +
            SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_SUBBLOCKCOUNT,
            numOfSubblocks );

        // Add SMS_SB_DELIVER_REPORT if needed
        if ( 0 != aRpCause )
            {
            deliverReportBuf.Append( messageParameters );
            deliverReportBuf.Append( tpFailureCause);
            // Add SMS_SB_DELIVER_REPORT subblock
            isiMsg.CopyData( sbOffset, deliverReportSb.CompleteSubBlock() );
            sbOffset += SIZE_SMS_SB_DELIVER_REPORT;
            }

        // Add SMS_SB_PARAM_INDICATOR & SMS_SB_USER_DATA subblocks if needed
        if ( userDataIncluded )
            {
            // Add SMS_SB_PARAM_INDICATOR subblock
            isiMsg.CopyData( sbOffset, paramIndicatorSb.CompleteSubBlock() );
            // Add SMS_SB_USER_DATA subblock
            sbOffset += paramIndSbLength;
            isiMsg.CopyData( sbOffset, userDataSb.CompleteSubBlock() );
            }

        ret = iPhoNetSender->Send( isiMsg.Complete() );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// Maps RP cause value to SMS cause value.
// --------------------------------------------------------------------------
TUint8 CMmSmsMessHandler::SmsMapCause ( TInt aRpCause )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsReceivedPpReportReq - aRpCause: %d", aRpCause);
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSMAPCAUSE, "CMmSmsMessHandler::SmsMapCause;aRpCause=%d", aRpCause );

    //initialization of the Sms Cause value is 0x00 (SMS_CAUSE_OK)
    TUint8 retSmsCause( 0 );

    switch ( aRpCause )
        {
        case KErrGsmSMSMemoryCapacityExceeded:
            {
            retSmsCause = SMS_EXT_ERR_MEMORY_CAPACITY_EXC;
            break;
            }

        //Try to find matching RP cause value from GSM 04.11

        case KErrGsmSMSInvalidShortMessageTransferReferenceValue:
            {
            retSmsCause = SMS_EXT_ERR_INVALID_REFERENCE;
            break;
            }

        case KErrGsmSMSNonExistentMessageType:
            {
            retSmsCause = SMS_EXT_ERR_INCORRECT_MESSAGE;
            break;
            }

        case KErrGsmSMSInvalidMandatoryInformation:
            {
            retSmsCause = SMS_EXT_ERR_INVALID_MAND_INFO;
            break;
            }

        case KErrGsmSMSIncompatibleMessageWithSmsProtocolState:
            {
            retSmsCause = SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST;
            break;
            }

        case KErrGsmSMSInformationElementNotImplemented:
            {
            retSmsCause = SMS_EXT_ERR_INVALID_INFO_ELEMENT;
            break;
            }

        case KErrGsmSMSUnspecifiedInvalidMessage:
            {
            retSmsCause = SMS_EXT_ERR_INVALID_MSG_TYPE;
            break;
            }

        case KErrGsmSMSUnspecifiedProtocolError:
            {
            retSmsCause = SMS_EXT_ERR_PROTOCOL_ERROR;
            break;
            }

        default:
            {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsReceivedPpReportReq - unknown RpCause: %d", aRpCause);
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSMAPCAUSE, "CMmSmsMessHandler::SmsMapCause;unknown aRpCause=%d", aRpCause );
            retSmsCause = SMS_EXT_ERR_PROTOCOL_ERROR;
            break;
            }
        }

    return retSmsCause;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsMessageSendResp
// Breaks a ISI-message and get possible TP-Failure-Cause from
// ISI-message.
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::SmsMessageSendResp(
    const TIsiReceiveC& aIsiMsg,
    TInt aIpc )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsMessageSendResp - IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSMESSAGESENDRESP, "CMmSmsMessHandler::SmsMessageSendResp;aIpc=%d", aIpc );

    TInt epocCause( KErrNone );
    TUint16 msgRef( 0 );
    TUint8 tpFailureCause( 0 );

    // Cause type and cause value
    TUint8 smsServerCauseType( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + SMS_MESSAGE_SEND_RESP_OFFSET_CAUSETYPE ) );
    TUint8 smsServerCauseValue = aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + SMS_MESSAGE_SEND_RESP_OFFSET_SMSCAUSE );

    if ( SMS_OK != smsServerCauseValue )
        {
        if ( SMS_CAUSE_TYPE_COMMON == smsServerCauseType )
            {
            if ( SMS_ERR_RC_REJECTED  == smsServerCauseValue )
                {
                if ( EMobileSmsMessagingSendMessage == aIpc )
                    {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsMessageSendResp - server cause: SMS_ERR_RC_REJECTED");
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_SMSMESSAGESENDRESP, "CMmSmsMessHandler::SmsMessageSendResp, server cause: SMS_ERR_RC_REJECTED" );
                    // This is an ETel MM-initiated SMS, barred by SAT.
                    epocCause = CMmStaticUtility::EpocErrorCode(
                        KErrAccessDenied,
                        KErrSatControl );
                    }
                else if (  EMmTsySmsSendSatMessage == aIpc )
                    {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsMessageSendResp - server cause: SMS_ERR_RC_REJECTED");
OstTrace0( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_SMSMESSAGESENDRESP, "CMmSmsMessHandler::SmsMessageSendResp, server cause: SMS_ERR_RC_REJECTED" );
                    // This is a SAT-initiated SMS, barred by SAT.
                    epocCause = KErrSatMoSmControlBarred;
                    }
                }
             else
                {
                epocCause = CMmStaticUtility::CSCauseToEpocError(
                    PN_SMS,
                    SMS_CAUSE_TYPE_COMMON,
                    smsServerCauseValue );
                }
            }
        else if ( SMS_CAUSE_TYPE_EXT == smsServerCauseType )
            {
            epocCause = CMmStaticUtility::CSCauseToEpocError(
                PN_SMS,
                SMS_CAUSE_TYPE_EXT,
                smsServerCauseValue );
            }
        else // Invalid cause type
            {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsMessageSendResp: Unknown cause type %d.", smsServerCauseType );
OstTraceExt1( TRACE_NORMAL, DUP5_CMMSMSMESSHANDLER_SMSMESSAGESENDRESP, "CMmSmsMessHandler::SmsMessageSendResp;smsServerCauseType=%hhu", smsServerCauseType );
TF_ASSERT_NOT_REACHED();
            }
        }
    else // Sending was OK
        {
        msgRef = aIsiMsg.Get8bit( ISI_HEADER_SIZE +
            SMS_MESSAGE_SEND_RESP_OFFSET_REFERENCEFORMESSAGE );
        }

    TUint smsSbSubmitReportOffset( 0 );
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SMS_MESSAGE_SEND_RESP,
        SMS_SB_SUBMIT_REPORT,
        EIsiSubBlockTypeId16Len16,
        smsSbSubmitReportOffset ) )
        {
        // SMS_SB_SUBMIT_REPORT subblock exists.
        // SMS server sets TP-Failure-Cause to zero in OK case.
        tpFailureCause = aIsiMsg.Get8bit(
            smsSbSubmitReportOffset
            + SMS_SB_SUBMIT_REPORT_OFFSET_TPFAILURECAUSE );
        }

    // Response for send SMS request
    if ( EMobileSmsMessagingSendMessage == aIpc
        || EMobileSmsMessagingSendMessageNoFdnCheck == aIpc )
        {
        TBuf8<RMobileSmsMessaging::KGsmTpduSize> pdu;

        // Create default SMS SUBMIT REPORT. Some operators may send
        // some information inside of this pdu. SMS stack refuses
        // to handle submit report that doesn't follow GSM 24.040 & 23.038
        // specifications.
        pdu.Append( KSmsMTISubmitOrSubmitReport ); // TP-MSG-Parameters

        if ( KErrNone == epocCause )
            {
            tpFailureCause = 0;
            }
        else if ( 0 == tpFailureCause )
            {
            tpFailureCause = 0xFF; // Unspecified error
            }

        pdu.Append( tpFailureCause ); // TP Failure Cause
        pdu.Append( 0x00 ); // TP Parameter Indicator

        TBuf8<KSmsScTimeStampMaxLength> timeStamp;
        TUint8 tsBuffer[KSmsScTimeStampMaxLength] =
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
        timeStamp.Append( &tsBuffer[0], sizeof( tsBuffer ) );
        pdu.Append( timeStamp ); // TP Service Centre Time Stamp

        // Create package and pack data
        CMmDataPackage data;
        data.PackData( &msgRef, &pdu );

        //complete the request
        if ( EMobileSmsMessagingSendMessage == aIpc )
            {
            iMessageRouter->Complete(
                EMobileSmsMessagingSendMessage,
                &data,
                epocCause );
            }
        else
            {
            iMessageRouter->Complete(
                EMobileSmsMessagingSendMessageNoFdnCheck,
                &data,
                epocCause );
            }

#if (NCP_COMMON_S60_VERSION_SUPPORT>S60_VERSION_32)
        iSMSSendingOngoing = EFalse;
#endif // NCP_COMMON_S60_VERSION_SUPPORT
        }
    // Response for send SAT SMS request
    else if ( EMmTsySmsSendSatMessage == aIpc )
        {
        //complete the request
        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        iMessageRouter->Complete( EMmTsySmsSendSatMessage, epocCause );
        #else
#ifdef __WINSCW__
            // for simatktsy testtool
            // Temporarily removed for Bridge camp!
      /*if ( iMessageRouter->iSatMessaging )
                {
                iMessageRouter->iSatMessaging->CompleteSendSmsMessage ( epocCause );
                }
            // for nokiatsy testtool
      */
            iMessageRouter->Complete ( EMmTsySmsSendSatMessage, epocCause );
#else // __WINSCW__
        // Temporarily removed for Bridge camp!
    //iMessageRouter->iSatMessaging->CompleteSendSmsMessage ( epocCause );
#endif // __WINSCW__
        iSMSSendingOngoing = EFalse;
#endif //NCP_COMMON_S60_VERSION_SUPPORT
        }
    else
        {
TFLOGSTRING2("TSY:CMmSmsMessHandler::SmsMessageSendResp:Unexpected IPC %d.",aIpc);
OstTrace1( TRACE_NORMAL, DUP6_CMMSMSMESSHANDLER_SMSMESSAGESENDRESP, "CMmSmsMessHandler::SmsMessageSendResp;aIpc=%d", aIpc );
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsReceiveMessageResp
// Breaks a SMS_RECEIVE_MESSAGE_RESP ISI-message and
// completes for SMS receiving request.
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::SmsReceiveMessageResp( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsReceiveMessageResp");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSPPROUTINGRESP, "CMmSmsMessHandler::SmsReceiveMessageResp" );

    // Get transaction ID for completing with correct IPC
    TUint8 trId( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + SMS_RECEIVE_MESSAGE_RESP_OFFSET_TRANSID ) );
    // Get reception status
    TUint8 receptionStatus( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + SMS_RECEIVE_MESSAGE_RESP_OFFSET_STATUS ) );

    // Get cause type and cause value. SMS_SB_CAUSE is mandatory subblock
    TUint8 smsServerCauseType( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + SIZE_SMS_RECEIVE_MESSAGE_RESP
        + SMS_SB_CAUSE_OFFSET_CAUSETYPE ) );
    TUint8 smsServerCauseValue( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + SIZE_SMS_RECEIVE_MESSAGE_RESP
        + SMS_SB_CAUSE_OFFSET_SMSCAUSE ) );

    TInt epocError( CMmStaticUtility::CSCauseToEpocError(
        PN_SMS,
        smsServerCauseType,
        smsServerCauseValue ) );

    if ( KErrNone == epocError )
        {
        // even if client deactivates reception, next time it is activated
        // SMS server will ask network to resend all NACKed MT SMs
        // only query status does not affect this flag, but this action
        // is not used
        iMemoryCapacityExceeded = EFalse;
        }

    // Compete active reception status
    if ( SMS_RECEPTION_ACTIVE == receptionStatus )
        {
        if ( 0 == trId ) // SMS receiving activated
            {
            CMmDataPackage package;
            package.PackData( &receptionStatus );
            iMessageRouter->Complete(
                EMmTsyActivateSmsRouting,
                &package,
                epocError );
            }
        else if ( ESmsMessagingResumeSmsReception == trId ) // Receive resumed
            {
            iMessageRouter->Complete(
                EMobileSmsMessagingResumeSmsReception,
                epocError );
            }
        // No else
        }
    else // SMS_RECEPTION_INACTIVE
        {
        CMmDataPackage package;
        package.PackData( &receptionStatus );
        iMessageRouter->Complete(
            EMmTsyDeactivateSmsRouting,
            &package,
            epocError );
        }
    return;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::IsSmsClass2
// Check if the class of the received sms is Class2 or not
// -----------------------------------------------------------------------------
//
TBool CMmSmsMessHandler::IsSmsClass2(
    const TIsiReceiveC& aIsiMsg, //Isi Msg
    TUint8& aSmsClass2ReplaceTpPid ) // TP-PID to replace (set by this function)
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::IsSmsClass2");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_ISSMSCLASS2, "CMmSmsMessHandler::IsSmsClass2" );
    TBool smsClass2( EFalse );
    TBool cphsCase( EFalse );

    // If received Class 2 SMS is a "replace type" message, contains
    // the TP-PID value, else value is zero.
    // See 3GPP TS 23.040 "Technical realization of Short Message
    // Service (SMS)", version V6.2.0 (2003-09), chapter 9.2.3.9
    // "TP Protocol Identifier (TP PID)"
    aSmsClass2ReplaceTpPid = 0;

    TUint tpduOffset( 0 );
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
        SMS_SB_TPDU,
        EIsiSubBlockTypeId16Len16,
        tpduOffset ) )
        {
        TUint8 userDataLength( aIsiMsg.Get8bit(
            tpduOffset + SMS_SB_TPDU_OFFSET_DATALENGTH ) );
        TPtrC8 sms( aIsiMsg.GetData(
            tpduOffset + SMS_SB_TPDU_OFFSET_DATABYTES,
            userDataLength ) );

        if ( 0 < sms.Length() )
            {
            // Begin CPHS special case handling
            // "CPHS Version 4.2, B.4.2.1 Voice Message Waiting Indicator":
            // An MT Class 2 ((U)SIM-specific) SMS with
            // - TP-Originating-Address coded as a Voice Message Waiting
            //   Indicator message
            // - User data a single space is not saved into EF_SMS.
            const TUint8 KCphsAddressLength( 4 );     // 00000100
            const TUint8 KCphsTypeOfAddress( 0xD0 );  // 11010000
            const TUint8 KCphsSpaceCharacter( 0x20 ); // 01000000
            // Skip message parameters (1 byte)
            TUint8 oaLen( sms[1] );
            if ( KCphsAddressLength == oaLen )
                {
                TUint8 typeOfAddress( sms[2] );
                TUint8 addressValueChar1( sms[3] );
                TUint8 addressValueChar2( sms[4] );
                if ( ( KCphsTypeOfAddress == typeOfAddress )
                     && ( 0x10 == ( 0x7E & addressValueChar1 ) )  //x001000x
                     && ( 0x00 == ( 0x7E & addressValueChar2 ) ) )//x000000x
                    {
                    // Skip PID(1 byte), skip DCS(1 byte), skip SCTS(7 bytes)
                    TUint8 udl( sms[14] );
                    if ( 1 == udl )
                        {
                        TUint8 ud( sms[15] );
                        if ( KCphsSpaceCharacter == ud )
                            {
                            // This is a CPHS message indicator. No further
                            // check of DCS and PID necessary.
                            cphsCase = ETrue;
                            smsClass2 = EFalse;
TFLOGSTRING("TSY: CMmSmsMessHandler::IsSmsClass2: CPHS message");
OstTrace0( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_ISSMSCLASS2, "CMmSmsMessHandler::IsSmsClass2. CPHS message" );
                            }
                        }
                    }
                }
            // End CPHS special case handling

            // No CPHS case, continue checking
            if ( ! cphsCase )
                {
                // Check type of received SMS message
                TUint8 messageParams( sms[0] );
                messageParams &= 0x03;
                if ( KSmsMTIDeliverOrDeliverReport == messageParams )
                    {
                    TUint8 protocolId( 0 );
                    // To retrieve the TP-DCS, we have to find the right index
                    // See 3GPP TS 23.040 "Technical realization of Short
                    // Message Service (SMS)", version V6.2.0 (2003-09),
                    // chapter 9.2.2.1 "SMS Deliver Type"

                    TUint8 index( 1 ); // Skip the first byte (message params)
                    TUint8 addressLength( sms[index] );
                    // Address is coded in semi-octets in TPDU->divide by two.
                    // Add two mandatory bytes of TP-OA
                    addressLength = ( addressLength + 1 ) / 2 + 2;
                    index += addressLength;
                    protocolId = sms[index++];

                    // Check if this class 2 SMS is a "replace type" message
                    // See 3GPP TS 23.040 "Technical realization of Short
                    // Message Service (SMS)", version V6.2.0 (2003-09),
                    // chapter 9.2.3.9 "TP Protocol Identifier (TP PID)"
                    if ( ( 0x41 <= protocolId ) && ( 0x47 >= protocolId ) )
                        {
                        // Yes, this is a "replace type" message
TFLOGSTRING2("TSY: CMmSmsMessHandler::IsSmsClass2: Replace type message, protocol ID: %d", protocolId );
OstTraceExt1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_ISSMSCLASS2, "CMmSmsMessHandler::IsSmsClass2. Replace type message;protocolId=%hhu", protocolId );
                        aSmsClass2ReplaceTpPid = protocolId;
                        }

                    // TP-DCS tells us the class of the SMS
                    TUint8 dataCodingScheme( sms[index++] );
                    // Copy TP-DCS to get the stronger bits later
                    TUint8 strongerBitsDCS( dataCodingScheme );

                    // Received message is of Class2 if dataCodingScheme
                    // contains either bits 00x1xx10 or 1111xx10.
                    // Gets the lower bits.
                    // See 3GPP TS 23.038 "Technical Specification Group
                    // Terminals; Alphabets and language-specific information"
                    // version V6.0.0 (2003-09), chapter 4 "SMS Data Coding
                    // Scheme"
                    dataCodingScheme &= 0x03;

                    // Gets the stronger bits
                    strongerBitsDCS >>= 4;
                    if (  ( 2 == dataCodingScheme ) &&
                        ( ( 1 == strongerBitsDCS ) ||
                        ( 3 == strongerBitsDCS ) ||
                        ( 15 == strongerBitsDCS ) ) )
                        {
                        // SMS is from Class2
                        smsClass2 = ETrue;
                        }
                    } // End of if KSmsMTIDeliverOrDeliverReport==messageParams
                } // End of if ( ! cphsCase )
            } // End of if ( 0 < sms.Length() )
        } // End of if SMS_SB_USER_DATA found

    // We are only interested in TP-PID "replace", if message is Class 2
    // (TP-PID "replace" for class 1 messages is handled by upper layers)
    if ( 0 != aSmsClass2ReplaceTpPid )
        {
        if ( smsClass2 )
            {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsClass: Received class 2 message has TP-PID 0x%x", aSmsClass2ReplaceTpPid);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_ISSMSCLASS2, "CMmSmsMessHandler::IsSmsClass2;aSmsClass2ReplaceTpPid=%hhx", aSmsClass2ReplaceTpPid );
            }
        else
            {
            aSmsClass2ReplaceTpPid = 0;
            }
        }
    return smsClass2;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsReceivedMsgInd
// Breaks SMS_RECEIVED_MSG_IND ISI-message that contains incoming
// SMS message. SMS message is not acknowledged to the network
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::SmsReceivedMsgInd( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsReceivedMsgInd");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSPPROUTINGNTF, "CMmSmsMessHandler::SmsReceivedMsgInd" );

    if ( iMemoryCapacityExceeded )
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsReceivedMsgInd no storage - internal NACK");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSRECEIVEDMSGIND, "CMmSmsMessHandler::SmsReceivedMsgInd no storage - internal NACK" );
        // error is ignored
        SmsReceivedMsgReportReq(
            EInternalNack,
            NULL,
            KErrGsmSMSMemoryCapacityExceeded );
        }
    else
        {
        TInt ret( KErrNone );
        // Create a package
        CMmDataPackage package;

        TUint8 replaceTpPid( 0 ); // IsSmsClass2 also fills this
        TBool receivedSmsClass2( IsSmsClass2( aIsiMsg, replaceTpPid ) );

        // SIM SMS cache: incoming class 2 SMS
        if ( receivedSmsClass2 )
            {
            ret = SmsClass2ReceivedMsgInd( aIsiMsg, replaceTpPid );
            }
        // Received SMS is not a class 2 SMS (it is a normal SMS)
        else
            {
            ret = SmsClass1ReceivedMsgInd( aIsiMsg );
            }
        // There was an error, complete to upper level
        if ( KErrNone != ret )
            {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsReceivedMsgInd;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSPPROUTINGNTF, "CMmSmsMessHandler::SmsReceivedMsgInd;ret=%d", ret );
            TBool smsInd( ETrue );
            TSmsMsg* nullSms = NULL;

            package.PackData( &smsInd, &nullSms );

            // Complete request to client
            iMessageRouter->Complete(
                EMobileSmsMessagingReceiveMessage,
                &package,
                ret );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsReceivedMsgReportResp
// Breaks a ISI-message, gets GSM specific sub blocks and
// response for Sms gsm received pp report request
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::SmsReceivedMsgReportResp( const TIsiReceiveC& aIsiMsg )
    {
    TUint8 traId( aIsiMsg.Get8bit( ISI_HEADER_SIZE
        + SMS_RECEIVED_MSG_REPORT_RESP_OFFSET_TRANSID ) );
    TUint8 cause( aIsiMsg.Get8bit( ISI_HEADER_SIZE
        + SMS_RECEIVED_MSG_REPORT_RESP_OFFSET_SMSCAUSE ) );

TFLOGSTRING3("TSY: CMmSmsMessHandler::SmsReceivedPpReportResp - traId: %d, cause: %d", traId, cause);
OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSRECEIVEDPPREPORTRESP, "CMmSmsMessHandler::SmsReceivedPpReportResp;traId=%hhu;cause=%hhu", traId, cause );

    if ( iMemoryCapacityExceeded )
        {
        // ignore the response, this is response to self-NACK
        // caused by client not having anymore storage space
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsReceivedPpReportResp - self-NACK");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSRECEIVEDMSGREPORTRESP, "CMmSmsMessHandler::SmsReceivedPpReportResp - self-NACK" );
        }
    else
        {
        // Response for SmsReceivedPpReportReq (Ack)
        if ( ESmsMessagingAckSmsStored == traId )
            {
            iMessageRouter->Complete(
                EMobileSmsMessagingAckSmsStored,
                CMmStaticUtility::CSCauseToEpocError(
                    PN_SMS,
                    SMS_CAUSE_TYPE_COMMON,
                    cause ) );
            }
        // Response for SmsReceivedPpReportReq (Nack)
        else if ( ESmsMessagingNackSmsStored == traId ||
            ESmsMessagingNackSmsStoredCapacityExceeded == traId )
            {
            iMessageRouter->Complete(
                EMobileSmsMessagingNackSmsStored,
                CMmStaticUtility::CSCauseToEpocError(
                    PN_SMS,
                    SMS_CAUSE_TYPE_COMMON,
                    cause ) );
            }
        else
            {
TFLOGSTRING2("TSY:CMmSmsMessHandler::SmsReceivedPpReportResp:Unexpected transaction ID %d.",traId);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSRECEIVEDPPREPORTRESP, "CMmSmsMessHandler::SmsReceivedPpReportResp;Unexpected  transaction ID=%hhu", traId );
            }

        if ( SMS_OK != cause )
            {
            //Acknowledging failed.
            //Complete possible receive message request with KErrGeneral and
            //set routing activity to false. SMS Stack makes new ReceiveMessage
            //request.
            //Client continues receiving MT SM indications if NACKing failed
            iMemoryCapacityExceeded = EFalse;

            TBool smsInd( EFalse );
            TSmsMsg* nullSms = NULL;

            //Complete request to client
            CMmDataPackage package;
            package.PackData( &smsInd, &nullSms );
            iSmsSlotLocation = 0;

            // ISI message construction failed or phonet sender
            // returned error
            iMessageRouter->Complete(
                EMobileSmsMessagingReceiveMessage,
                &package,
                KErrGeneral );
            }
        else if ( ESmsMessagingNackSmsStoredCapacityExceeded == traId )
            {
            // client succeeded NACKing MT SM
            // it wont receive any further MT SM indications
            iMemoryCapacityExceeded = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::BuildScAddress
// Build Service Centre Address
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::BuildScAddress(
    RMobilePhone::TMobileAddress const & scPtr, //Pointer to Service center
    TDes8 &aScAddress ) const                  //Service center address
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::BuildScAddress");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDSCADDRESS, "CMmSmsMessHandler::BuildScAddress" );
    //get type of number from attributes
    RMobilePhone::TMobileTON ton( scPtr.iTypeOfNumber );
    //map type of number from TMobileTON to TUint8
    TUint8 typeOfNumber( CMmSmsGsmAddress::GsmMapTonToTUint8( ton ) );
    //get numbering plan identification from attributes
    RMobilePhone::TMobileNPI npi( scPtr.iNumberPlan );
    //map numbering plan identification from TMobileTON to TUint8
    TUint8 numberPlan( CMmSmsGsmAddress::GsmMapNpiToTUint8( npi ) );
    //return service center address
    TPtrC16 telNumber( scPtr.iTelNumber );

    //build Gsm 04.11 type of address structure
    CMmSmsGsmAddress::GsmConvUnicodeTo0411Addr(
        typeOfNumber,
        numberPlan,
        aScAddress,
        telNumber );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::BuildDeAddress
// Build Destination Address
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::BuildDeAddress(
    RMobilePhone::TMobileAddress const & scPtr,  // Service centre pointer
    TDes8 &aDeAddress ) const                    // Destination address
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::BuildDeAddress");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDDEADDRESS, "CMmSmsMessHandler::BuildDeAddress" );
    //get type of number from attributes
    RMobilePhone::TMobileTON ton( scPtr.iTypeOfNumber );

    //map type of number from TMobileTON to TUint8
    TUint8 typeOfNumber( CMmSmsGsmAddress::GsmMapTonToTUint8( ton ) );

    //get numbering plan identification from attributes
    RMobilePhone::TMobileNPI npi( scPtr.iNumberPlan );

    //map numbering plan identification from TMobileTON to TUint8
    TUint8 numberPlan( CMmSmsGsmAddress::GsmMapNpiToTUint8( npi ) );

    //return service center address
    TPtrC16 telNumber( scPtr.iTelNumber );

    //build Gsm 03.40 type of address structure
    CMmSmsGsmAddress::GsmConvUnicodeTo0340Addr(
        typeOfNumber,
        numberPlan,
        aDeAddress,
        telNumber );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::DataCodingScheme
// Check if message data is coded using 7bit alphabet
// (other items were commented in header)
// -----------------------------------------------------------------------------
//
TBool CMmSmsMessHandler::IsDataCodingScheme7Bit( TUint8 aDcs ) const
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::IsDataCodingScheme7Bit");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_ISDATACODINGSCHEME7BIT, "CMmSmsMessHandler::IsDataCodingScheme7Bit" );
    TUint8 codingGroup( static_cast<TUint8>( 0xf0 & aDcs ) );  // 11110000
    TUint8 lowerBits( static_cast<TUint8>( 0x0f & aDcs ) );    // 00001111

    // See 3GPP TS 23.038 "Technical Specification Group Terminals;
    //Alphabets and language-specific information"
    // version V6.0.0 (2003-09), chapter 4 "SMS Data Coding Scheme"
    // 00XX General data coding indication
    // 01XX Message Marked for Automation Deletion Group
    if ( codingGroup >> 6 == 0x00 || codingGroup >> 6 == 0x01 )
        {
        // 00XX Default alphabet, 11XX Reserved
        if ( ( lowerBits >> 2 == 0x00 ) || ( lowerBits >> 2 == 0x03 ) )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    // 1100 Message Waiting Indication Group: Discard Message
    // 1101 Message Waiting Indication Group: Store Message
    else if ( codingGroup >> 4 == 0x0C || codingGroup >> 4 == 0x0D )
        {
        return ETrue;
        }
    // 1110 Message Waiting Indication Group: Store Message (UCS2)
    else if ( codingGroup >> 4 == 0x0E )
        {
        return EFalse;
        }
    // 1111 Data coding/message class
    else if ( codingGroup >> 4 == 0x0F )
        {
        lowerBits&= 0x04;
        // X0XX Default alphabet
        if ( lowerBits == 0 )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    // 1000..1011 Reserved coding groups
    else
        {
        // Any reserved codings shall be assumed to be the GSM default alphabet
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::CalculateNumberOfCharsInUserData
// Calculate number of characters in 7-bit user data
// -----------------------------------------------------------------------------
//
TUint8 CMmSmsMessHandler::CalculateNumberOfCharsInUserData(
    TUint8 aCharacterAmount,           //Character amount (from ISI message)
    TUint8 aDataLength,                //Data length in bytes (from ISI message)
    TPtrC8 const & aUserData ) const   //User data
    {
TFLOGSTRING3("TSY: CMmSmsMessHandler::CalculateNumberOfCharsInUserData - char amount (from ISI): %d, data length (from ISI): %d", aCharacterAmount, aDataLength);
OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_CALCULATENUMBEROFCHARSINUSERDATA, "CMmSmsMessHandler::CalculateNumberOfCharsInUserData;aCharacterAmount=%hhu;aDataLength=%hhu", aCharacterAmount, aDataLength );

    TUint16 characterAmount;

    // This calculation is only needed if the number of characters is
    // not provided in the aCharacterAmount parameter. Seems to be some
    // old backwards-compatibility issue or so ...
    if ( ( 0 != aCharacterAmount ) && ( 0xff != aCharacterAmount ) )
        {
        characterAmount = aCharacterAmount;
        }
    else
        {
        //we have to calculate number of characters in user data, when
        //7-bit chars are used.
        characterAmount = static_cast<TUint16>(  ( aDataLength * 8 ) / 7 );
        if ( ( 0 == ( ( aDataLength * 8 ) % 7 ) ) && characterAmount )
            {
            if ( 0 == ( aUserData[aDataLength-1] & 0xFE ) )
                {
                //LSB in user data's last byte is zero -> number of characters
                //is characterAmount-1
                characterAmount--;
                }
            }
        }
    return static_cast<TUint8>( characterAmount );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::ExtFuncL
// Dispatches Etel requests to DOS level handlers
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::ExtFuncL(
    TInt aIpc,                           // IPC number
    const CMmDataPackage* aDataPackage ) // Data package
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::ExtFuncL: IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_EXTFUNCL, "CMmSmsMessHandler::ExtFuncL;aIpc=%d", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobileSmsMessagingGetMessageStoreInfo:
        case EMobilePhoneStoreGetInfo:
            {
            ret = GetSmsStoreInfo( aIpc );
            break;
            }
        case EMmTsyActivateSmsRouting:
            {
#if defined( INTERNAL_TESTING_SMS_WITH_EMULATOR )  && ( ! defined( __WINS__ ) )
TFLOGSTRING("TSY: CMmSmsMessHandler::ExtFuncL: SMS routing deactivated with flag INTERNAL_TESTING_SMS_WITH_EMULATOR");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_EXTFUNCL, "CMmSmsMessHandler::ExtFuncL, SMS routing deactivated with flag INTERNAL_TESTING_SMS_WITH_EMULATOR" );
#else
            ret = SmsReceiveMessageReq( SMS_RECEPTION_ACTIVATE );
#endif // defined( INTERNAL_TESTING_SMS_WITH_EMULATOR ) && ( ! defined( __WINS__ ) )
            break;
            }
        case EMmTsyDeactivateSmsRouting:
            {
            //Release the SMS_PP_ROUTING
            ret = SmsReceiveMessageReq( SMS_RECEPTION_DEACTIVATE );
            break;
            }
        case EMobileSmsMessagingSendMessage:
            {
            ret = SmsMessageSendReq(
                ESmsMessagingSendMessage,
                aDataPackage,
                EFalse );
            break;
            }
        case EMobileSmsMessagingSendMessageNoFdnCheck:
            {
            ret = SmsMessageSendReq(
                ESmsMessagingSendNoFdnMessage,
                aDataPackage,
                ETrue );
            break;
            }
        case EMmTsySmsSendSatMessage:
            {
            ret = SmsMessageSendReq(
                ESmsMessagingSendSatMessage,
                aDataPackage,
                ETrue );
            break;
            }
        case EMobileSmsMessagingResumeSmsReception:
            {
            ret = SmsReceiveMessageReq( SMS_RECEPTION_STORAGE_STATUS_UPDATE );
            break;
            }
        case EMobileSmsMessagingGetSmspListPhase1:
            {
            // Reset internal temporary SMSP storage
            iSmspListArray->ResetAndDestroy();
            // Start reading SMSP entries
            ret = UiccGetSmspEntryReq();
            break;
            }
        case EMobileSmsMessagingStoreSmspList:
            {
            ret = UiccSmsUpdateParameterReq( aDataPackage );
            break;
            }
        case EMobileSmsMessagingAckSmsStored:
            {
            TDesC8* data;
            //unpack data
            aDataPackage->UnPackData( data );

            TBuf8<RMobileSmsMessaging::KGsmTpduSize>* msgData =
                reinterpret_cast<TBuf8<RMobileSmsMessaging::KGsmTpduSize> *> ( data );

            if ( NULL == msgData )
                {
TFLOGSTRING("TSY: CMmSmsMessHandler::ExtFuncL.Internal Ack handling started" );
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_EXTFUNCL, "CMmSmsMessHandler::ExtFuncL, Internal Ack handling started" );
                }
            ret = SmsReceivedMsgReportReq(
                ESmsMessagingAckSmsStored,
                msgData,
                KErrNone );
            break;
            }
        case EMobileSmsMessagingNackSmsStored:
            {
            TInt rpCause( 0 );
            TBuf8<RMobileSmsMessaging::KGsmTpduSize>* msgData;

            //unpack data
            aDataPackage->UnPackData( msgData, rpCause );

            if ( NULL == msgData )
                {
TFLOGSTRING("TSY: CMmSmsMessHandler::ExtFuncL.Internal Nack handling started" );
OstTrace0( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_EXTFUNCL, "CMmSmsMessHandler::ExtFuncL, Internal Nack handling started" );
                }

            // NACK due to lack of storage space causes NTSY to reject further
            // MT SMs until client resumes SMS reception
            TUint8 traId = KErrGsmSMSMemoryCapacityExceeded == rpCause ?
                ESmsMessagingNackSmsStoredCapacityExceeded : ESmsMessagingNackSmsStored;
            ret = SmsReceivedMsgReportReq(
                traId,
                msgData,
                rpCause );
            break;
            }
        case EMobilePhoneStoreDelete:
            {
            ret = DeleteSms( aDataPackage );
            break;
            }
        case EMobilePhoneStoreDeleteAll:
            {
            iRecordId = 1; // Start fron 1st location
            ret = DeleteAllSms();
            break;
            }
        case EMobilePhoneStoreRead:
            {
            ret = ReadSms( aDataPackage );
            break;
            }
        case EMobilePhoneStoreReadAllPhase1:
            {
            ret = ReadAllSmsL();
            break;
            }
        case EMobilePhoneStoreWrite:
            {
            TInt index( 0 );
            TDesC8* data;

            // unpacked index is not used since the index to write to
            // can also be taken from smsData
            aDataPackage->UnPackData( data, index );

            //typecast for aEntry
            RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* entryPckg =
                reinterpret_cast<RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*>(
                const_cast<TDesC8*>( data ) );
            RMobileSmsStore::TMobileGsmSmsEntryV1& entry = ( *entryPckg )();
            iSMSClass2Write = EFalse;
            ret = WriteSms( entry, index );
           break;
            }
        case ECustomSetSimMessageStatusReadIPC:
            {
TFLOGSTRING("TSY: CMmSmsMessHandler::ExtFuncL - ECustomSetSimMessageStatusReadIPC starts.");
OstTrace0( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_EXTFUNCL, "CMmSmsMessHandler::ExtFuncL, ECustomSetSimMessageStatusReadIPC starts" );
            TTime scTime;
            TInt timezoneDiff;
            aDataPackage->UnPackData( scTime, timezoneDiff );

            // start searching for a SIM message with this timestamp
            TUint8 totalSlots( static_cast<TUint8>(
                iSmsCache.TotalEntries() ) );
            TUint8 currentSlot( 1 );
            while ( currentSlot <= totalSlots )
                {
                RMobileSmsStore::TMobileGsmSmsEntryV1* smsFromCache =
                    iSmsCache.GetEntry( currentSlot );

                if ( smsFromCache )
                    {
                    if ( CheckSCTimestamp( *smsFromCache, scTime ) )
                        {
                        // Message with the same timestamp found,
                        // update SIM flag.
                        UiccUpdateSMSStatus( currentSlot );
                        currentSlot = totalSlots; // we're done, exit loop
                        }
                    }
                currentSlot++;
                }
TFLOGSTRING("TSY: CMmSmsMessHandler::ExtFuncL - ECustomSetSimMessageStatusReadIPC done.");
OstTrace0( TRACE_NORMAL, DUP5_CMMSMSMESSHANDLER_EXTFUNCL, "CMmSmsMessHandler::ExtFuncL, ECustomSetSimMessageStatusReadIPC done" );
            ret = KErrNone;
            break;
            }
        case EMobileSmsMessagingSetMoSmsBearer:
            {
            RMobileSmsMessaging::TMobileSmsBearer bearer;
            aDataPackage->UnPackData( bearer );
            ret = SmsSettingsUpdateReq( bearer );
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmSmsMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP6_CMMSMSMESSHANDLER_EXTFUNCL, "CMmSmsMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
            ret = KErrArgument;
            break;
            }
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter
// Compares TP-PID, sender number and SC number of the received class 2
// SMS in aReceivedSmsIsiMsg, and the SMS read from SIM in aReadSmsIsiMsg.
// -----------------------------------------------------------------------------
//
TBool CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter(
    TUint8 aReceivedTpPid,                             //received TP-PID
    const TIsiReceiveC&  aReceivedSmsIsiMsg,           //received isi msg
    RMobileSmsStore::TMobileGsmSmsEntryV1* aSMSOnSIM ) //SMS stored on SIM
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter, aReceivedTpPid = %d", aReceivedTpPid);
OstTraceExt1( TRACE_NORMAL, CMMSMSMESSHANDLER_CHECKTPPIDANDSENDERANDSERVICECENTER, "CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter;aReceivedTpPid=%hhu", aReceivedTpPid );

    TBool matchFound( EFalse );

    // Get protocol id from stored SMS

    TInt offset( 1 ); // Set offset to message reference in TPDU
    TUint8 messageReference( aSMSOnSIM->iMsgData[offset] );

    // Destination address length is integer representation
    // of the number of useful semi-octets of address field
    // Add two mandatory bytes of TP-OA, too.

    messageReference = ( ( messageReference + 1 ) / 2) + 2;
    offset += messageReference;

    TUint8 protocolId( aSMSOnSIM->iMsgData[offset] );
    if ( protocolId == aReceivedTpPid )
        {
        // this message has the same TP-PID. Now check if it also has
        // same sender number and service center number
        TBuf<KMaxAddressBufferSize> receivedMsgServiceCentre;
        RMobilePhone::TMobileTON receivedMsgMobileScTON(
            RMobilePhone::EUnknownNumber );
        RMobilePhone::TMobileNPI receivedMsgMobileScNPI(
            RMobilePhone::EUnknownNumberingPlan );

        TBuf<KMaxAddressBufferSize> receivedMsgOriginatorAdress;
        RMobilePhone::TMobileTON receivedMsgMobileOaTON(
            RMobilePhone::EUnknownNumber );
        RMobilePhone::TMobileNPI receivedMsgMobileOaNPI(
            RMobilePhone::EUnknownNumberingPlan );

        TBuf<KMaxAddressBufferSize> readMsgOriginatorAdress;
        RMobilePhone::TMobileTON readMsgMobileOaTON(
            RMobilePhone::EUnknownNumber );
        RMobilePhone::TMobileNPI readMsgMobileOaNPI(
            RMobilePhone::EUnknownNumberingPlan );

        // 1. Get service center address from received message
        TUint smsAddressOffset( 0 );
        if ( KErrNone == aReceivedSmsIsiMsg.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
            SMS_SB_ADDRESS,
            EIsiSubBlockTypeId16Len16,
            smsAddressOffset ) )
            {
            if ( SMS_SMSC_ADDRESS == aReceivedSmsIsiMsg.Get8bit(
                smsAddressOffset + SMS_SB_ADDRESS_OFFSET_ADDRESSTYPE ) )
                {
                // Convert service centre address (SC Address 04.11)
                TUint8 addressDataLength( aReceivedSmsIsiMsg.Get8bit(
                    smsAddressOffset +
                    SMS_SB_ADDRESS_OFFSET_ADDRESSDATALENGTH ) );
                TPtrC8 scA( aReceivedSmsIsiMsg.GetData(
                    smsAddressOffset + SMS_SB_ADDRESS_OFFSET_ADDRESSDATA,
                    addressDataLength ) );
                CMmSmsGsmAddress::GsmConv0411AddrToUnicode(
                    receivedMsgServiceCentre,
                    scA,
                    receivedMsgMobileScTON,
                    receivedMsgMobileScNPI );
                }
            }
        else // could not extract data from received message
            {
TFLOGSTRING("TSY: CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter: Could not extract data from received message");
            OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_CHECKTPPIDANDSENDERANDSERVICECENTER, "CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter: Could not extract data from received message" );
            return EFalse;
            }

        // 2. Get sender address from received message
        TUint smsUserDataOffset( 0 );
        if ( KErrNone == aReceivedSmsIsiMsg.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
            SMS_SB_TPDU,
            EIsiSubBlockTypeId16Len16,
            smsUserDataOffset ) )
            {
            TUint8 userDataLength( aReceivedSmsIsiMsg.Get8bit(
                smsUserDataOffset + SMS_SB_TPDU_OFFSET_DATALENGTH ) );
            TPtrC8 sms( aReceivedSmsIsiMsg.GetData(
                smsUserDataOffset + SMS_SB_TPDU_OFFSET_DATABYTES,
                userDataLength ) );

            if ( 0 < sms.Length() )
                {
                // Check type of received SMS message
                TUint8 messageParams( sms[0] );
                // Extract the least 2 bits to get SMS message type
                messageParams &= 0x03;
                if ( KSmsMTIDeliverOrDeliverReport == messageParams )
                    {
                    TPtrC8 tpOa( sms.Mid( 1 ) );
                    CMmSmsGsmAddress::GsmConv0340AddrToUnicode(
                        receivedMsgOriginatorAdress,
                        tpOa,
                        receivedMsgMobileOaTON,
                        receivedMsgMobileOaNPI );
                    }
                }
            }
        else // Could not extract data from received message
            {
TFLOGSTRING("TSY: CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter: Could not extract data from received message");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_CHECKTPPIDANDSENDERANDSERVICECENTER, "CMmSmsMessHandler::CheckTpPidAndSenderAndServiceCenter - Could not extract data from received message" );
            return EFalse;
            }

        // 3. Get sender number from stored message

        TPtrC8 tpOa( aSMSOnSIM->iMsgData.Mid ( 1 ) );
        CMmSmsGsmAddress::GsmConv0340AddrToUnicode(
            readMsgOriginatorAdress,
            tpOa,
            readMsgMobileOaTON,
            readMsgMobileOaNPI );

        // 4. Compare
        if ( ( receivedMsgMobileOaTON == readMsgMobileOaTON )
            && ( receivedMsgMobileOaNPI == readMsgMobileOaNPI )
            && ( 0 == receivedMsgOriginatorAdress.Compare(
            readMsgOriginatorAdress ) )

            && ( receivedMsgMobileScTON == aSMSOnSIM->iServiceCentre.iTypeOfNumber )
            && ( receivedMsgMobileScNPI == aSMSOnSIM->iServiceCentre.iNumberPlan )
            && ( 0 == receivedMsgServiceCentre.Compare(
                 aSMSOnSIM->iServiceCentre.iTelNumber ) ) )
            {
            matchFound = ETrue;
            }
        }
    return matchFound;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::GetSmsStoreInfo
// ETel requests EMobileSmsMessagingGetMessageStoreInfo and
// EMobilePhoneStoreGetInfo
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::GetSmsStoreInfo( TInt aIpc )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::GetSmsStoreInfo - IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_GETSMSSTOREINFO, "CMmSmsMessHandler::GetSmsStoreInfo;aIpc=%d", aIpc );
    TInt ret( KErrNone );
    // SIM SMS cache:
    // -- EMobileSmsMessagingGetMessageStoreInfo/EMobilePhoneStoreGetInfo
    if ( KErrNone == iSmsCache.Status() )
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::GetSmsStoreInfo -- completed immediatedly");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_GETSMSSTOREINFO, "CMmSmsMessHandler::GetSmsStoreInfo, completed immediatedly" );
        TUint8 numOfLoc( TUint8( iSmsCache.TotalEntries() ) );
        TInt usedEntries( iSmsCache.UsedEntries() );
        CMmDataPackage data;
        data.PackData( &numOfLoc, &usedEntries );
        iMessageRouter->Complete( aIpc, &data, KErrNone );
        }
    // try to recover from out of memory condition
    else if ( KErrNoMemory == iSmsCache.Status() )
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::GetSmsStoreInfo -- recovering from OOM");
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_GETSMSSTOREINFO, "CMmSmsMessHandler::GetSmsStoreInfo, recovering from OOM" );
        InitializeSmsCache();
        }
    // cache is not ready, start waiting
    else if ( KErrNotReady == iSmsCache.Status() )
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::GetSmsStoreInfo -- waiting");
OstTrace0( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_GETSMSSTOREINFO, "CMmSmsMessHandler::GetSmsStoreInfo, waiting" );
        }
    // some other, error return error value
    else
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::GetSmsStoreInfo -- failed");
OstTrace0( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_GETSMSSTOREINFO, "CMmSmsMessHandler::GetSmsStoreInfo, failed" );
        ret = iSmsCache.Status();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::DeleteSms
// Delete SMS from SIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::DeleteSms( const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::DeleteSms");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_DELETESMS, "CMmSmsMessHandler::DeleteSms" );
    // SIM SMS cache: -- EMobilePhoneStoreDelete
    TInt ret( KErrNone );

    // Symbian sms stack will try to delete received SMS2 from SIM if it can't
    // save it to filesystem, so deletion should be possible in out of memory
    // state..
    if ( KErrNone == iSmsCache.Status() || KErrNoMemory == iSmsCache.Status() )
        {
        TInt index( 0 );
        //unpack data
        aDataPackage->UnPackData( index );
        RMobileSmsStore::TMobileGsmSmsEntryV1* smsData(
            iSmsCache.GetEntry( index ) );

        if ( smsData )
            {
            iSmsCache.SetDeleteLocation( index );

            // Set parameters for UICC_APPL_CMD_REQ message
            TUiccWriteLinearFixed params;
            params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
            params.trId = ETrIdDeleteSMS;
            params.dataOffset = 0;
            params.dataAmount = 0;
            params.record = index;

            params.fileId = KElemFileShortMessages;
            params.fileIdSfi = UICC_SFI_NOT_PRESENT;
            params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

            // File id path
            params.filePath.Append( KMasterFileId >> 8 );
            params.filePath.Append( KMasterFileId );
            params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

            // Update file data with 0xFF
            TBuf8<KSmsElemetaryFileRecordLength> fileDataBuf;
            fileDataBuf.AppendFill( 0xFF, KSmsElemetaryFileRecordLength );

            params.fileData.Append( fileDataBuf );

            return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
            }
        else
            {
            // Location empty or not in use.
            iMessageRouter->Complete( EMobilePhoneStoreDelete, KErrNone );
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::DeleteSms, cache not ready/failed");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_DELETESMS, "CMmSmsMessHandler::DeleteSms, cache not ready/failed" );
        ret = iSmsCache.Status();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::DeleteAllSms
// Delete all SMS's from sim
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::DeleteAllSms()
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::DeleteAllSms");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_DELETEALLSMS, "CMmSmsMessHandler::DeleteAllSms" );
    TInt ret( KErrNone );
    // SIM SMS cache: -- EMobilePhoneStoreDeleteAll
    if ( KErrNone == iSmsCache.Status() )
        {
        if ( 0 < iSmsCache.TotalEntries() )
            {
            RMobileSmsStore::TMobileGsmSmsEntryV1* smsData(
                iSmsCache.GetEntry( iRecordId ) );

            if ( smsData )
                {
                iSmsCache.SetDeleteLocation( iRecordId );

                // Set parameters for UICC_APPL_CMD_REQ message
                TUiccWriteLinearFixed params;
                params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
                params.trId = ETrIdDeleteAllSMSs;
                params.dataOffset = 0;
                params.dataAmount = 0;
                params.record = iRecordId;

                params.fileId = KElemFileShortMessages;
                params.fileIdSfi = UICC_SFI_NOT_PRESENT;
                params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

                // File id path
                params.filePath.Append( KMasterFileId >> 8 );
                params.filePath.Append( KMasterFileId );
                params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

                // Update file data with 0xFF
                TBuf8<KSmsElemetaryFileRecordLength> fileDataBuf;
                fileDataBuf.AppendFill( 0xFF, KSmsElemetaryFileRecordLength );

                params.fileData.Append( fileDataBuf );

                return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
                }
            else
                {
                // Location is empty. Check next location
                UiccDeleteAllSMSResp( KErrNone );
                }
            }
        else
            {
            iMessageRouter->Complete( EMobilePhoneStoreDeleteAll, KErrNone );
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::DeleteAllSms, cache not ready/failed");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_DELETEALLSMS, "CMmSmsMessHandler::DeleteAllSms, cache not ready/failed" );
        ret = iSmsCache.Status();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::ReadSms
// Read SMS's from SIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::ReadSms( const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::ReadSms");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_READSMS, "CMmSmsMessHandler::ReadSms" );

    TInt ret( KErrNone );

    TInt location( 0 );
    //unpack data
    aDataPackage->UnPackData( location );

    // SIM SMS cache: -- EMobilePhoneStoreRead
    if ( KErrNone == iSmsCache.Status() )
        {
         // Find sms from cache
        RMobileSmsStore::TMobileGsmSmsEntryV1* smsData(
            iSmsCache.GetEntry( location ) );

        if ( smsData )
            {
            TSmsMsg smsMsg;
            smsMsg.iLocation = location;
            smsMsg.iMessageStatus = smsData->iMsgStatus;
            smsMsg.iMobileScNPI = smsData->iServiceCentre.iNumberPlan;
            smsMsg.iMobileScTON = smsData->iServiceCentre.iTypeOfNumber;
            smsMsg.iServiceCentre = smsData->iServiceCentre.iTelNumber;
            smsMsg.iSmsClass2 = ETrue;
            smsMsg.iSmsMsg = smsData->iMsgData;

            CMmDataPackage data;
            data.PackData( &smsMsg );
            iMessageRouter->Complete(
                EMobilePhoneStoreRead,
                &data,
                KErrNone );

            ret = KErrNone;
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::ReadSms, cache not ready/failed");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_READSMS, "CMmSmsMessHandler::ReadSms, cache not ready/failed" );
        ret = iSmsCache.Status();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::ReadAllSmsL
// Read SMS's from SIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::ReadAllSmsL()
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::ReadAllSms");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_READALLSMSL, "CMmSmsMessHandler::ReadAllSmsL" );
    TInt ret( KErrNone );

    // SIM SMS cache: -- EMobilePhoneStoreReadAllPhase1
    if ( KErrNone == iSmsCache.Status() )
        {
        // reads all sms's from SIM to a list and returns
        // the list to commontsy.

        iSmsListArray->ResetAndDestroy();

        // process & copy SMS's to iSmsListArray..
        for ( TInt i( 1 ); i <= iSmsCache.TotalEntries(); i++ )
            {
            // Add read SMS to the TSY's internal storage
            // Find sms from cache
            RMobileSmsStore::TMobileGsmSmsEntryV1* smsData(
                iSmsCache.GetEntry( i ) );

            TSmsMsg* smsMsg = new ( ELeave )TSmsMsg();
            CleanupDeletePushL( smsMsg );

            if ( smsData )
                {
                smsMsg->iLocation = i;
                smsMsg->iMessageStatus = smsData->iMsgStatus;
                smsMsg->iMobileScNPI = smsData->iServiceCentre.iNumberPlan;
                smsMsg->iMobileScTON = smsData->iServiceCentre.iTypeOfNumber;
                smsMsg->iServiceCentre = smsData->iServiceCentre.iTelNumber;
                smsMsg->iSmsClass2 = ETrue;
                smsMsg->iSmsMsg = smsData->iMsgData;

                iSmsListArray->AppendL( smsMsg );
                CleanupStack::Pop( smsMsg );
                }
            else
                {
                CleanupStack::PopAndDestroy( smsMsg );
                }
            }

        // complete immediatedly...
        CMmDataPackage package;
        package.PackData( &iSmsListArray, &iReceivedClass2ToBeReSent );
        iMessageRouter->Complete(
            EMobilePhoneStoreReadAllPhase1,
            &package,
            KErrNone );

        //Delete array
        iSmsListArray->ResetAndDestroy();
        iReceivedClass2ToBeReSent = EFalse;
        }
    else
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::ReadAllSms --- cache not ready/failed");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_READALLSMSL, "CMmSmsMessHandler::ReadAllSmsL, cache not ready/failed" );
        iSmsListArray->ResetAndDestroy();
        CMmDataPackage package;
        package.PackData( &iSmsListArray, &iReceivedClass2ToBeReSent );
        iMessageRouter->Complete(
            EMobilePhoneStoreReadAllPhase1,
            &package,
            iSmsCache.Status() );
        iReceivedClass2ToBeReSent = EFalse;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::WriteSms
// Write SMS to SIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::WriteSms(
    const RMobileSmsStore::TMobileGsmSmsEntryV1& aEntry,
    TInt aIndex)
    {
    TInt ret( KErrNone );

TFLOGSTRING("TSY: CMmSmsMessHandler::WriteSms");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_WRITESMS, "CMmSmsMessHandler::WriteSms" );

    // SIM SMS cache: -- EMobilePhoneStoreWrite
    //check if sms writing is ongoing or not
    if ( 0 != iSmsSlotLocation )
        {
        ret = KErrNotReady;
        }
    else if ( KErrNone == iSmsCache.Status() )
        {

        //iWriteSmsEntryPtr = aEntry;

        //find a free slot to write the Sms
        // -1 means "find the frst free slot"
        if ( -1 != aIndex )
            {
            iSmsSlotLocation = TUint8( aIndex );
            }
        else
            {
            iSmsSlotLocation = TUint8( iSmsCache.FirstFreeLocation() );
            aIndex = iSmsSlotLocation;
            }

        if ( 0 == iSmsSlotLocation )
            {
            // asked to write to a free slot, but no free slots found
            ret = CMmStaticUtility::EpocErrorCode(
                KErrOverflow,
                KErrMMEtelMaxReached );
            }
        else
            {
            ret = UiccWriteSMSReq( aEntry, iSmsSlotLocation );
            }
        }
    else
        {
        ret = iSmsCache.Status();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::InitializeSmsCache
// Start reading cache
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::InitializeSmsCache()
    {
    // SIM SMS cache: initialize cache
TFLOGSTRING("TSY: CMmSmsMessHandler::InitializeSmsCache -- SMS CACHEING START ---");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_INITIALIZESMSCACHE, "CMmSmsMessHandler::InitializeSmsCache" );
    iSmsCache.Reset();
    GetNumOfEFSMSRecords (); // Read EF SMS count.
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::HandleError
// Handles CMmSmsMessHandler's errors comes via PhoNetReceiver RunError method.
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::HandleError(
        const TIsiReceiveC& aIsiMsg, // Isi message
        TInt aError )                // Error code

    {
TFLOGSTRING2( "TSY: CMmSmsMessHandler::HandleError - Error: %d", aError );
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_HANDLEERROR, "CMmSmsMessHandler::HandleError;aError=%d", aError );

    TUint8 resource( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TUint8 messageId( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );
    TUint8 transId( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

TFLOGSTRING4( "TSY: CMmSmsMessHandler::HandleError - resource: %d, msgId: %d, traId: %d", resource, messageId, transId );
OstTraceExt3(TRACE_NORMAL,DUP1_CMMSMSMESSHANDLER_HANDLEERROR,"CMmSmsMessHandler::HandleError;resource=%hhu;messageId=%hhu;transId=%hhu", resource, messageId, transId );

    switch ( resource )
        {
        default:
            {
TFLOGSTRING2( "TSY: CMmSmsMessHandler::HandleError - PN_SMS, unknown resource: %d", resource );
OstTraceExt1( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_HANDLEERROR, "CMmSmsMessHandler::HandleError;resource=%hhu", resource );
            // Nothing to do here.
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsSettingsUpdateReq
// Sets SMS bearer.
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::SmsSettingsUpdateReq(
    RMobileSmsMessaging::TMobileSmsBearer& aBearer )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsSettingsUpdateReq. aBearer: %d", aBearer);
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSSETTINGSUPDATEREQ, "CMmSmsMessHandler::SmsSettingsUpdateReq;aBearer=%d", aBearer );

    TUint8 csRoutePriority( SMS_ROUTE_NOT_AVAILABLE );
    TUint8 psRoutePriority( SMS_ROUTE_NOT_AVAILABLE );

    // The bearer information must be used in two ways:
    // 1. It must be saved here, and is used later for SMS sending.
    iMobileSmsBearer = aBearer;

    // 2. It must be immediadely set to ISI SMS Server (affects
    // RP-SHORT-MESSAGE-MEMORY-AVAILABLE).
    switch ( iMobileSmsBearer )
        {
        case RMobileSmsMessaging::ESmsBearerPacketOnly:
            { // PS with highest priority, CS not available
            psRoutePriority = SMS_ROUTE_PRIORITY_1;
            break;
            }
        case RMobileSmsMessaging::ESmsBearerPacketPreferred:
            { // PS with highest priority, CS with second highest priority
            psRoutePriority = SMS_ROUTE_PRIORITY_1;
            csRoutePriority = SMS_ROUTE_PRIORITY_2;
            break;
            }
        case RMobileSmsMessaging::ESmsBearerCircuitOnly:
            { // CS with highest priority, PS not available
            csRoutePriority = SMS_ROUTE_PRIORITY_1;
            break;
            }
        case RMobileSmsMessaging::ESmsBearerCircuitPreferred:
            { // CS with highest priority, PS with second highest priority
            csRoutePriority = SMS_ROUTE_PRIORITY_1;
            psRoutePriority = SMS_ROUTE_PRIORITY_2;
            break;
            }
        default:
            {
TFLOGSTRING2("TSY:CMmSmsMessHandler::SmsSettingsUpdateReq:Invalid bearer = %d",iMobileSmsBearer);
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSSETTINGSUPDATEREQ, "CMmSmsMessHandler::SmsSettingsUpdateReq;iMobileSmsBearer=%d", iMobileSmsBearer );
            break;
            }
        }

TFLOGSTRING3("TSY:CMmSmsMessHandler::SmsSettingsUpdateReq:CS priority = %d, PS priority = %d",csRoutePriority,psRoutePriority);
OstTraceExt2( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_SMSSETTINGSUPDATEREQ, "CMmSmsMessHandler::SmsSettingsUpdateReq;csRoutePriority=%hhd;psRoutePriority=%hhd", csRoutePriority, psRoutePriority );

    // SMS_SB_ROUTE_INFO sub block (8 bytes)
    TBuf8<SIZE_SMS_SB_ROUTE_INFO> smsRouteInfoBuf( 0 );
    TIsiSubBlock smsRouteInfoSb(
        smsRouteInfoBuf,
        SMS_SB_ROUTE_INFO,
        EIsiSubBlockTypeId16Len16 );
    smsRouteInfoBuf.Append( csRoutePriority );
    smsRouteInfoBuf.Append( psRoutePriority );

    // Create SMS_SETTINGS_UPDATE_REQ message
    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SMS );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_SETTINGS_UPDATE_REQ_OFFSET_TRANSID,
        0 );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_SETTINGS_UPDATE_REQ_OFFSET_MESSAGEID,
        SMS_SETTINGS_UPDATE_REQ  );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_SETTINGS_UPDATE_REQ_OFFSET_SETTINGTYPE,
        SMS_SETTING_TYPE_ROUTE );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + SMS_SETTINGS_UPDATE_REQ_OFFSET_SUBBLOCKCOUNT,
        1 );

    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_SMS_SETTINGS_UPDATE_RESP,
        smsRouteInfoSb.CompleteSubBlock() );

    return ( iPhoNetSender->Send( isiMsg.Complete() ) );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsSettingsUpdateResp
// Completes SMS bearer setting.
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::SmsSettingsUpdateResp( const TIsiReceiveC& aIsiMsg )
    {
    TUint8 cause( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + SMS_SETTINGS_UPDATE_RESP_OFFSET_SMSCAUSE ) );

TFLOGSTRING2("TSY:CMmSmsMessHandler::SmsSettingsUpdateResp: response status=%d.",cause);
OstTraceExt1( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSGSMSETTINGSUPDATERESP, "CMmSmsMessHandler::SmsSettingsUpdateResp;cause=%hhu", cause );

    TInt err( CMmStaticUtility::CSCauseToEpocError(
        PN_SMS,
        SMS_CAUSE_TYPE_COMMON,
        cause ) );

    iMessageRouter->Complete( EMobileSmsMessagingSetMoSmsBearer, err );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsClass2ReceivedMsgInd
// Processes class2 SMS. Called by CMmSmsMessHandler::SmsReceivedMsgInd.
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::SmsClass2ReceivedMsgInd(
    const TIsiReceiveC& aIsiMsg,
    TUint8 aIsReplace )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsClass2ReceivedMsgInd");
OstTrace0( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_SMSCLASS2PPROUTINGNTF, "CMmSmsMessHandler::SmsClass2ReceivedMsgInd" );
    TInt ret( KErrNone );

    TUint smsTpduOffset( 0 );
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
        SMS_SB_TPDU,
        EIsiSubBlockTypeId16Len16,
        smsTpduOffset ) )
        {
        // SMS_SB_TPDU sub block found
        // Checked if sms writing is ongoing or not and cache is ok
        if ( 0 == iSmsSlotLocation && KErrNone == iSmsCache.Status() )
            {
            TUint location( 0 );

            // If this is a replace message, try find a SMS to replace
            if ( aIsReplace )
                {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsClass2ReceivedMsgInd --- replace");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSCLASS2PPROUTINGNTF, "CMmSmsMessHandler::SmsClass2ReceivedMsgInd, replace" );
                for ( TInt i( 0 ); i <= iSmsCache.TotalEntries(); i++ )
                    {
                    RMobileSmsStore::TMobileGsmSmsEntryV1* simIsiMsg =
                        iSmsCache.GetEntry( i );
                    if ( simIsiMsg )
                        {
                        if ( CheckTpPidAndSenderAndServiceCenter(
                            aIsReplace,
                            aIsiMsg,
                            simIsiMsg ) )
                            {
                            location = i;
                            break;
                            }
                        }
                    }
                }

            // Not a replace message, or no message to replace found.
            // Try to find a free slot
            if ( 0 == location )
                {
                location = iSmsCache.FirstFreeLocation();
                }

            // Location found, start SMS writing
            if ( 0 != location )
                {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsClass2ReceivedMsgInd -- incoming SMS2, writing to location %d", location);
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSCLASS2PPROUTINGNTF, "CMmSmsMessHandler::SmsClass2ReceivedMsgInd;incoming SMS2, writing to location=%d", location );
                // iSmsSlotLocation = TUint8(location);
                // Write received class2 SMS to SIM Card
                // Get data from incoming message.
                RMobileSmsStore::TMobileGsmSmsEntryV1 incomingSMS;

                // 1. Get service center address from received message
                TUint smsAddressOffset( 0 );
                if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
                    SMS_SB_ADDRESS,
                    EIsiSubBlockTypeId16Len16,
                    smsAddressOffset ) )
                    {
                    if ( SMS_SMSC_ADDRESS == aIsiMsg.Get8bit(
                        smsAddressOffset + SMS_SB_ADDRESS_OFFSET_ADDRESSTYPE ) )
                        {
                        // Convert service centre address (SC Address 04.11)
                        TUint8 addressDataLength( aIsiMsg.Get8bit(
                            smsAddressOffset +
                            SMS_SB_ADDRESS_OFFSET_ADDRESSDATALENGTH ) );
                        TPtrC8 scA( aIsiMsg.GetData(
                            smsAddressOffset + SMS_SB_ADDRESS_OFFSET_ADDRESSDATA,
                            addressDataLength ) );
                        CMmSmsGsmAddress::GsmConv0411AddrToUnicode(
                            incomingSMS.iServiceCentre.iTelNumber,
                            scA,
                            incomingSMS.iServiceCentre.iTypeOfNumber,
                            incomingSMS.iServiceCentre.iNumberPlan );
                        }
                    }

                // 2. Get sender address from received message
                TUint smsUserDataOffset( 0 );
                if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
                    SMS_SB_TPDU,
                    EIsiSubBlockTypeId16Len16,
                    smsUserDataOffset ) )
                    {
                    TUint8 userDataLength( aIsiMsg.Get8bit(
                        smsUserDataOffset + SMS_SB_TPDU_OFFSET_DATALENGTH ) );
                    TPtrC8 sms( aIsiMsg.GetData(
                        smsUserDataOffset + SMS_SB_TPDU_OFFSET_DATABYTES,
                        userDataLength ) );

                    if ( 0 < sms.Length() )
                        {
                        // Check type of received SMS message
                        TUint8 position( 0 );
                        incomingSMS.iMsgStatus =
                            static_cast <RMobileSmsStore::TMobileSmsStoreStatus>( sms[position] );

                        // Start of the user data
                        incomingSMS.iMsgData =  sms.Mid( position );
                        }
                    }
                    iSMSClass2Write = ETrue;
                    ret = WriteSms( incomingSMS, location );
               }

            // No free slots! SIM full
            else
                {
                ret = KErrGsmSMSUnspecifiedProtocolError;
                }
            }

        // Cache not ready
        else
            {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsClass2ReceivedMsgInd -- incoming SMS2, cache not ready/failed");
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_SMSCLASS2PPROUTINGNTF, "CMmSmsMessHandler::SmsClass2ReceivedMsgInd, incoming SMS2, cache not ready/failed" );
            ret = KErrGsmSMSMemoryCapacityExceeded;
            // Setting this true will cause it to be resent?
            iReceivedClass2ToBeReSent = ETrue;

            // If this is out of memory error, try to refill cache..
            if ( KErrNoMemory == iSmsCache.Status() )
                {
                InitializeSmsCache();
                }
            }
        }

TFLOGSTRING2("TSY: CMmSmsMessHandler::SmsClass2ReceivedMsgInd ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_SMSCLASS2PPROUTINGNTF, "CMmSmsMessHandler::SmsClass2ReceivedMsgInd;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::SmsClass1ReceivedMsgInd
// Processes class1 SMS. Called by CMmSmsMessHandler::SmsReceivedMsgInd.
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::SmsClass1ReceivedMsgInd(
        const TIsiReceiveC& aIsiMsg)
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::SmsClass1ReceivedMsgInd");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_SMSCLASS1PPROUTINGNTF, "CMmSmsMessHandler::SmsClass1ReceivedMsgInd" );
    TInt ret( KErrNone );
    TBool validPduFormat( EFalse );
    TSmsMsg  smsMsgValue;
    TSmsMsg* smsMsg = &smsMsgValue;

    // Handle SMS_SB_ADDRESS subblock (service center address)
    TUint smsAddressOffset( 0 );
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
        SMS_SB_ADDRESS,
        EIsiSubBlockTypeId16Len16,
        smsAddressOffset ) )
        {
         TUint8 addressDataLength( aIsiMsg.Get8bit(
             smsAddressOffset + SMS_SB_ADDRESS_OFFSET_ADDRESSDATALENGTH ) );

         TPtrC8 serviceCenterAddress( aIsiMsg.GetData(
             smsAddressOffset + SMS_SB_ADDRESS_OFFSET_ADDRESSDATA,
             addressDataLength ) );

         CMmSmsGsmAddress::GsmConv0411AddrToUnicode(
             smsMsg->iServiceCentre,
             serviceCenterAddress,
             smsMsg->iMobileScTON,
             smsMsg->iMobileScNPI );
         }
     else
         {
TFLOGSTRING("TSY:CMmSmsMessHandler::SmsClass1ReceivedMsgInd,SMS_SB_ADDRESS expected");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_SMSCLASS1PPROUTINGNTF, "CMmSmsMessHandler::SmsClass1ReceivedMsgInd,SMS_SB_ADDRESS expected" );
         TF_ASSERT_NOT_REACHED();
         }

    // Handle SMS_SB_TPDU subblock
    TUint smsTpduOffset( 0 );
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_MSG_IND,
        SMS_SB_TPDU,
        EIsiSubBlockTypeId16Len16,
        smsTpduOffset ) )
        {
        TUint8 userDataLength( aIsiMsg.Get8bit(
            smsTpduOffset + SMS_SB_TPDU_OFFSET_DATALENGTH ) );

        TPtrC8 smsUserData( aIsiMsg.GetData(
            smsTpduOffset + SMS_SB_TPDU_OFFSET_DATABYTES,
            userDataLength ) );

        if ( 0 < smsUserData.Length() )
            {
            // Check type of received SMS message
            TUint8 messageParams( smsUserData[0] );
            messageParams &= 0x03;

            // Copy SMS to iSmsMsg if message type was SMS-DELIVER or
            // SMS-STATUS-REPORT. Set also KSimSmsNotPresent in iMessageStatus
            // because message is routed directly to the SMS stack (not
            // stored in SIM).
            if ( KSmsMTIDeliverOrDeliverReport == messageParams ||
                KSmsMTIStatusReportOrCommand == messageParams )
                {
                validPduFormat = ETrue;
                smsMsg->iMessageStatus = KSimSmsNotPresent;
                smsMsg->iSmsMsg.Copy( smsUserData );
                }
            }
        else
            {
TFLOGSTRING("TSY:CMmSmsMessHandler::SmsClass1ReceivedMsgInd, SMS_SB_USER_DATA expected");
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_SMSCLASS1PPROUTINGNTF, "CMmSmsMessHandler::SmsClass1ReceivedMsgInd, SMS_SB_USER_DATA expected" );
            TF_ASSERT_NOT_REACHED();
            }

        if ( validPduFormat )
            {
            TBool smsInd( ETrue );
            smsMsg->iSmsClass2 = EFalse;

            // Pack data
            CMmDataPackage package;
            package.PackData( &smsInd, &smsMsg );

            // Complete request to client
            iMessageRouter->Complete(
                EMobileSmsMessagingReceiveMessage,
                &package,
                KErrNone );

            ret = KErrNone; // avoid doubled completion
            }
        else
            {
            // Message type is undefined. Send NACK to the SMS Server.
            SmsReceivedMsgReportReq(
                EInternalNack,
                NULL,
                KErrGsmSMSUnspecifiedInvalidMessage );
            // Caller is forced to complete
            ret = KErrGeneral;
            }
        }
TFLOGSTRING2("TSY:CMmSmsMessHandler::SmsClass1ReceivedMsgInd;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_SMSCLASS1PPROUTINGNTF, "CMmSmsMessHandler::SmsClass1ReceivedMsgInd;ret=%d", ret );
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::GetDestAddressLength
// Extracts the length of destination address from TPDU in octets
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::GetDestAddressLength(
    const TDesC8& aMsgData,
    TUint8 subBlockId,
    TUint8& aDestAddressLength ) const
    {
TFLOGSTRING("TSY:CMmSmsMessHandler::GetDestAddressLength");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_GETDESTADDRESSLENGTH, "CMmSmsMessHandler::GetDestAddressLength" );

    TInt ret( KErrNone );

    if ( SMS_SB_SUBMIT == subBlockId )
        {
        aDestAddressLength =
            aMsgData[ KTpduSubmitIndexDestinationAddressLength ] ;
        }
    else
        {
        aDestAddressLength =
            aMsgData[ KTpduCommandIndexDestinationAddressLength ] ;
        }

    // According 3GPP 23.040 max length of destination address is 20
    if ( KMaxAmountOfDigits >= aDestAddressLength )
        {
        // Destination address length is integer representation
        // of the number of useful semi-octets of address field
        aDestAddressLength =  ( aDestAddressLength + 1 ) / 2 ;
        }
    else
        {
        ret = KErrArgument;
TFLOGSTRING2("TSY:CMmSmsMessHandler::GetDestinationAddressLength: Error! Destination address too long: %d.",aDestAddressLength);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_GETDESTADDRESSLENGTH, "CMmSmsMessHandler::GetDestAddressLength;aDestAddressLength=%hhu", aDestAddressLength );
        }
TFLOGSTRING2("TSY:CMmSmsMessHandler::GetDestinationAddressLength:Destination address length %d.",aDestAddressLength);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_GETDESTADDRESSLENGTH, "CMmSmsMessHandler::GetDestAddressLength;aDestAddressLength=%hhu", aDestAddressLength );
    return ret;
    }


// -----------------------------------------------------------------------------
// Creates SMS_SB_SUBMIT subblock and appends it to ISI message
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::BuildSmsSbSubmit(
    const TDesC8& aMsgData,
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset,
    TUint8 aDestAddressLength ) const
    {
TFLOGSTRING("TSY:CMmSmsMessHandler::BuildSmsSbSubmit");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDSMSSBSUBMIT, "CMmSmsMessHandler::BuildSmsSbSubmit" );

    // Extract needed parameters from TPDU for for SMS_SB_SUBMIT
    TUint8 messageParameters( aMsgData[ KTpduIndexMessageParameters ] );
    TUint8 messageReference( aMsgData[ KTpduIndexMessageReference ] );
    TUint8 protocolId( aMsgData[ KTpduSubmitIndexProtocolIdentifier +
        aDestAddressLength ] );
    TUint8 dataCodingScheme( aMsgData[ KTpduSubmitIndexDataCodingScheme +
        aDestAddressLength ] );

    // Create and fill SMS_SB_SUBMIT subblock
    TBuf8<SIZE_SMS_SB_SUBMIT> submitBuf;
    TIsiSubBlock submit( submitBuf, SMS_SB_SUBMIT, EIsiSubBlockTypeId16Len16 );
    submitBuf.Append( messageParameters );
    submitBuf.Append( messageReference );
    submitBuf.Append( protocolId );
    submitBuf.Append( dataCodingScheme );

    aIsiMsg.CopyData( aMsgOffset, submit.CompleteSubBlock() );

TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbSubmit. Message parameters: %d", messageParameters );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbSubmit. Message reference: %d", messageReference );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbSubmit. Protocol ID: %d", protocolId );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbSubmit. Data coding scheme: %d", dataCodingScheme );
OstTraceExt4( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_BUILDSMSSBSUBMIT, "CMmSmsMessHandler::BuildSmsSbSubmit;messageParameters=%hhu;messageReference=%hhu;protocolId=%hhu;dataCodingScheme=%hhu", messageParameters, messageReference, protocolId, dataCodingScheme );
    }


// -----------------------------------------------------------------------------
// Creates SMS_SB_COMMAND subblock and appends it to ISI message
// -----------------------------------------------------------------------------
void CMmSmsMessHandler::BuildSmsSbCommand(
    const TDesC8& aMsgData,
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset ) const
    {
TFLOGSTRING("TSY:CMmSmsMessHandler::BuildSmsSbCommand");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDSMSSBCOMMAND, "CMmSmsMessHandler::BuildSmsSbCommand" );

    // Extract needed parameters from TPDU for SMS_SB_COMMAND
    TUint8 messageParameters( aMsgData[ KTpduIndexMessageParameters ] );
    TUint8 messageReference( aMsgData[ KTpduIndexMessageReference ] );
    TUint8 protocolId( aMsgData[ KTpduCommandIndexProtocolIdentifier ] );
    TUint8 commandType( aMsgData[ KTpduCommandIndexType ] );
    TUint8 messageNumber( aMsgData[ KTpduCommandIndexMessageNumber ] );

    // Create and fill SMS_SB_COMMAND subblock
    TBuf8<SIZE_SMS_SB_COMMAND> commandBuf;
    TIsiSubBlock command(
        commandBuf,
        SMS_SB_COMMAND,
        EIsiSubBlockTypeId16Len16 );

    commandBuf.Append( messageParameters );
    commandBuf.Append( messageReference );
    commandBuf.Append( protocolId );
    commandBuf.Append( commandType );
    commandBuf.Append( messageNumber );

    aIsiMsg.CopyData( aMsgOffset, command.CompleteSubBlock() );

TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbCommand. Message parameters: %d", messageParameters );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbCommand. Message reference: %d", messageReference );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbCommand. Protocol ID: %d", protocolId );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbCommand. Command type: %d", commandType );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbCommand. Message Number: %d", messageNumber );
OstTraceExt5( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_BUILDSMSSBCOMMAND, "CMmSmsMessHandler::BuildSmsSbCommand;messageParameters=%hhu;messageReference=%hhu;protocolId=%hhu;commandType=%hhu;messageNumber=%hhu", messageParameters, messageReference, protocolId, commandType, messageNumber );
    }


// -----------------------------------------------------------------------------
// Creates SMS_SB_ADDRESS subblock and appends it to ISI message
// -----------------------------------------------------------------------------
TUint8 CMmSmsMessHandler::BuildSmsSbAddress(
    const TDesC8& aAddress,
    TIsiSend& aIsiMsg,
    TUint8 aAddressType,
    TUint8 aMsgOffset ) const
    {
TFLOGSTRING("TSY:CMmSmsMessHandler::BuildSmsSbAddress");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDSMSSBADDRESS, "CMmSmsMessHandler::BuildSmsSbAddress" );

    TUint8 addressLength( aAddress.Length() );
    TUint8 subblockLength( 0 );
    // Create and fill SMS_SB_ADDRESS subblock
    TBuf8<SIZE_SMS_SB_ADDRESS + SMS_ADDRESS_MAX_LEN> addressBuf( 0 );
    TIsiSubBlock address(
        addressBuf,
        SMS_SB_ADDRESS,
        EIsiSubBlockTypeId16Len16 );
    addressBuf.Append( aAddressType  );
    addressBuf.Append( addressLength );
    addressBuf.Append( aAddress );

    aIsiMsg.CopyData( aMsgOffset, address.CompleteSubBlock() );

    // Subblock length is needed for return value
    subblockLength = addressBuf.Length();

TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbAddress. Address type: %d", aAddressType );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbAddress. Address length: %d", addressLength );
OstTraceExt2( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_BUILDSMSSBADDRESS, "CMmSmsMessHandler::BuildSmsSbAddress;aAddressType=%hhu;addressLength=%hhu", aAddressType, addressLength );
#ifdef _DEBUG
for ( TInt i( 0 ); i < aAddress.Length(); i++ )
    {
TFLOGSTRING3( "TSY:CMmSmsMessHandler::BuildSmsSbAddress. Address data[%d]: 0x%x", i, aAddress[i] );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_BUILDSMSSBADDRESS, "CMmSmsMessHandler::BuildSmsSbAddress;i=%hhu;aAddress[i]=%hhu", i, aAddress[i] );
    }
#endif // _DEBUG
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbAddress. Length of subblock: %d", subblockLength );
OstTraceExt1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_BUILDSMSSBADDRESS, "CMmSmsMessHandler::BuildSmsSbAddress;subblockLength=%hhu", subblockLength );

    return subblockLength;
    }


// -----------------------------------------------------------------------------
// Creates SMS_SB_USER_DATA subblock and appends it to ISI message
// -----------------------------------------------------------------------------
TUint8 CMmSmsMessHandler::BuildSmsSbUserData(
    const TDesC8& aMsgData,
    TIsiSend& aIsiMsg,
    TUint8 aTpUdl,
    TUint8 aTpUserDataIndex,
    TBool aDefaultAlphabet,
    TUint8 aMsgOffset ) const
    {
TFLOGSTRING("TSY:CMmSmsMessHandler::BuildSmsSbUserData");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDSMSSBUSERDATA, "CMmSmsMessHandler::BuildSmsSbUserData" );

    TUint8 dataLengthInOctets( 0 );
    TUint8 subblockLength( 0 );

    // If data is 7-bit, then TP-UDL is integer representation of
    // the number of septets within the TP-UD field
    if ( aDefaultAlphabet )
        {
        dataLengthInOctets = ( ( aTpUdl + 1 ) * 7 ) / 8;
        }
    else
        {
        dataLengthInOctets = aTpUdl;
        }

    // Extract actual data bytes from TPDU
    TPtrC8 dataBytes( aMsgData.Mid( aTpUserDataIndex, dataLengthInOctets ) );

    // Create and fill SMS_SB_USER_DATA subblock
    TBuf8<SIZE_SMS_SB_USER_DATA + SMS_COMMAND_DATA_MAX_LEN> userDataBuf;
    TIsiSubBlock userData(
        userDataBuf,
        SMS_SB_USER_DATA,
        EIsiSubBlockTypeId16Len16 );
    userDataBuf.Append( KSmsPadding );
    userDataBuf.Append( dataLengthInOctets );
    userDataBuf.Append( KSmsPadding );
    userDataBuf.Append( aTpUdl );
    userDataBuf.Append( dataBytes );

    aIsiMsg.CopyData( aMsgOffset, userData.CompleteSubBlock() );

    subblockLength = userDataBuf.Length();

TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbUserData. User data length in octets: %d", dataLengthInOctets );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbUserData. User data character count: %d", aTpUdl );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_BUILDSMSSBUSERDATA, "CMmSmsMessHandler::BuildSmsSbUserData;dataLengthInOctets=%hhu;aTpUdl=%hhu", dataLengthInOctets, aTpUdl );
TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbUserData. Length of subblock: %d", subblockLength );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_BUILDSMSSBUSERDATA, "CMmSmsMessHandler::BuildSmsSbUserData;subblockLength=%hhu", subblockLength );

    return subblockLength;
    }


// -----------------------------------------------------------------------------
// Creates SMS_SB_VALIDITY_PERIOD subblock and appends it to ISI message
// -----------------------------------------------------------------------------
void CMmSmsMessHandler::BuildSmsSbValidityPeriod(
    const TDesC8& aMsgData,
    TIsiSend& aIsiMsg,
    TUint8 aTpVpIndex,
    TUint8 aTpVpLength,
    TUint8 aMsgOffset ) const
    {
TFLOGSTRING("TSY:CMmSmsMessHandler::BuildSmsSbValidityPeriod");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDSMSSBVALIDITYPERIOD, "CMmSmsMessHandler::BuildSmsSbValidityPeriod" );

    // Extract validity period data bytes
    TPtrC8 tpVpData( aMsgData.Mid( aTpVpIndex, aTpVpLength ) );

    // Create and fill SMS_SB_VALIDITY_PERIOD subblock, max length is 12
    TBuf8<12> validityPeriodBuf;
    TIsiSubBlock validityPeriod(
        validityPeriodBuf,
        SMS_SB_VALIDITY_PERIOD,
        EIsiSubBlockTypeId16Len16 );
    validityPeriodBuf.Append( aTpVpLength );
    validityPeriodBuf.Append( tpVpData );

    aIsiMsg.CopyData( aMsgOffset, validityPeriod.CompleteSubBlock() );

TFLOGSTRING2( "TSY:CMmSmsMessHandler::BuildSmsSbValidityPeriod. Validity period length: %d", aTpVpLength );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_BUILDSMSSBVALIDITYPERIOD, "CMmSmsMessHandler::BuildSmsSbValidityPeriod;aTpVpLength=%hhu", aTpVpLength );

#ifdef _DEBUG
for ( TInt i( 0 ); i < aTpVpLength; i++ )
    {
TFLOGSTRING3( "Validity period, byte[%d], 0x%x", i, tpVpData[i] );
OstTraceExt2( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_BUILDSMSSBVALIDITYPERIOD, "CMmSmsMessHandler::BuildSmsSbValidityPeriod;i=%hhu;tpVpData[i]=%hhu", i, tpVpData[i] );
    }
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// Creates SMS_SB_CHECK_INFO sub block with SMS_CHECK_DISABLE_FDN and appends it
// to ISI message.
// -----------------------------------------------------------------------------
void CMmSmsMessHandler::BuildSmsCheckInfo(
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset ) const
    {
TFLOGSTRING("TSY:CMmSmsMessHandler::BuildSmsCheckInfo");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_BUILDSMSCHECKINFO, "CMmSmsMessHandler::BuildSmsCheckInfo" );

    // Create and fill SMS_SB_CHECK_INFO subblock.
    TBuf8<SIZE_SMS_SB_CHECK_INFO> checkInfoBuf;
    TIsiSubBlock checkInfo(
        checkInfoBuf,
        SMS_SB_CHECK_INFO,
        EIsiSubBlockTypeId16Len16 );
    checkInfoBuf.Append( SMS_CHECK_DISABLE_FDN );
    checkInfoBuf.Append( KSmsPadding );
    checkInfoBuf.Append( KSmsPadding );
    checkInfoBuf.Append( KSmsPadding );

    aIsiMsg.CopyData( aMsgOffset, checkInfo.CompleteSubBlock() );

    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::ProcessUiccMsg
// Handles data received from UICC server
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY:CMmSmsMessHandler::ProcessUiccMsg, aTraId: %d, status: %d", aTraId, aStatus );
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_PROCESSUICCMSG, "CMmSmsMessHandler::ProcessUiccMsg;aTraId=%d", aTraId );
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_PROCESSUICCMSG, "CMmSmsMessHandler::ProcessUiccMsg;aStatus=%d", aStatus );

    TInt ret( KErrNone );

    switch( aTraId )
        {
        case ETrIdReadSMS:
            {
            UiccReadSMSResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadSMSForComplete:
            {
            UiccReadSMSRespForComplete( aStatus, aFileData );
            break;
            }
        case ETrIdWriteSMS:
            {
            UiccWriteSMSResp( aStatus );
            break;
            }
        case ETrIdReadSMSRecordCount:
            {
            GetNumOfEFSMSRecordsResp( aStatus, aFileData );
            break;
            }
        case ETrIdDeleteSMS:
            {
            UiccDeleteSMSResp( aStatus );
            break;
            }
        case ETrIdDeleteAllSMSs:
            {
            UiccDeleteAllSMSResp( aStatus );
            break;
            }
        case ETrIdUpdateSMSStatusReadSMS:
            {
            UiccUpdateSMSStatusReadSMSResp( aStatus, aFileData );
            break;
            }
        case ETrIdUpdateSMSStatusWriteSMS:
            {
            UiccUpdateSMSStatusWriteSMSResp( aStatus );
            break;
            }
        case ETrIdGetSmspEntries:
            {
            // If status is OK, save the entry and then try to read next record
            if ( UICC_STATUS_OK == aStatus )
                {
                // Store the entry to the list
                ret = UiccStoreSmspEntry( aFileData );
                }

            // If the first record was tried to read, but UICC server returned
            // error or data was invalid, error is completed to commontsy
            if ( 1 == iSmspRecordNumber &&
                ( UICC_STATUS_OK != aStatus || KErrNone != ret ) )
                {
                iMessageRouter->Complete(
                    EMobileSmsMessagingGetSmspListPhase1,
                    KErrNotFound );
                // Reset internal array
                iSmspListArray->ResetAndDestroy();
                }
            // At least one SMS entry was read and stored successfully
            else if( UICC_STATUS_OK != aStatus || KErrNone != ret )
                {
                CMmDataPackage package;
                package.PackData( iSmspListArray );
                iMessageRouter->Complete(
                    EMobileSmsMessagingGetSmspListPhase1,
                    &package,
                    KErrNone );
                // Reset internal array
                iSmspListArray->ResetAndDestroy();
                }
            break;
            }
        case ETrIdWriteSmspEntry:
            {
            TInt error( KErrNone );
            if ( UICC_STATUS_OK != aStatus )
                {
                error = KErrNotFound;
                }
            iMessageRouter->Complete( EMobileSmsMessagingStoreSmspList, error );
            break;
            }
        default:
            {
TFLOGSTRING("TSY:CMmSmsMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_PROCESSUICCMSG, "CMmSmsMessHandler::ProcessUiccMsg - unknown transaction ID" );

            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccReadSMSOrSMSRecordCountReq
// Read SMS or SMS EF record count from SIM/USIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::UiccReadSMSOrSMSRecordCountReq(
    const TUint8 aRecordId,
    const TUiccTrId aTrId )
    {
TFLOGSTRING3("TSY: CMmSmsMessHandler::UiccReadSMSReq, aTraId: %d, aRecordId: %d", aTrId, aRecordId );
OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCREADSMSORSMSRECORDCOUNTREQ, "CMmSmsMessHandler::UiccReadSMSOrSMSRecordCountReq;aTrId=%hhu;aRecordId=%hhu", aTrId, aRecordId );

    TUint8 serviceType( 0 );
    TInt ret( KErrNone );

    switch( aTrId )
        {
        case ETrIdReadSMS:
        case ETrIdReadSMSForComplete:
            {
            // Save record id for next request
            iRecordId = aRecordId;
            serviceType = UICC_APPL_READ_LINEAR_FIXED;
            break;
            }
        case ETrIdReadSMSRecordCount:
            {
            serviceType = UICC_APPL_FILE_INFO;
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccReadSMSReq - unknown transaction ID" );
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_UICCREADSMSORSMSRECORDCOUNTREQ, "CMmSmsMessHandler::UiccReadSMSOrSMSRecordCountReq - unknown transaction ID" );
            ret = KErrUnknown;
            break;
            }
        }

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = aTrId;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = aRecordId;

    params.fileId = KElemFileShortMessages;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = serviceType;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    if ( KErrNone == ret )
        {
        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccReadSMSResp
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::UiccReadSMSResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccReadSMSResp" );
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCREADSMSRESP, "CMmSmsMessHandler::UiccReadSMSResp" );

    if ( KErrNone == aStatus )
        {
TFLOGSTRING2("TSY: CMmSmsMessHandler::SimStSmsHandleReadCacheEntriesL -- cacheing iRecordId=%d", iRecordId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_UICCREADSMSRESP, "CMmSmsMessHandler::UiccReadSMSResp;iRecordId=%hhu", iRecordId );

        if ( 0 != aFileData.Length() )
            {
            // add to cache
            RMobileSmsStore::TMobileGsmSmsEntryV1 smsData;

            TUint8 position( 0 );
            smsData.iMsgStatus =
                static_cast <RMobileSmsStore::TMobileSmsStoreStatus>(
                    aFileData[position] );

            TBuf<KMaxAddressBufferSize> receivedMsgServiceCentre;
            RMobilePhone::TMobileTON receivedMsgMobileScTON(
                RMobilePhone::EUnknownNumber );
            RMobilePhone::TMobileNPI receivedMsgMobileScNPI(
                RMobilePhone::EUnknownNumberingPlan );

            position++;
            TUint8 addressDataLength( aFileData[position] );
            position++;
            TPtrC8 scA( aFileData.Mid( position, addressDataLength ) );
            CMmSmsGsmAddress::GsmConv0411AddrToUnicode(
                receivedMsgServiceCentre,
                scA,
                receivedMsgMobileScTON,
                receivedMsgMobileScNPI );

            smsData.iServiceCentre.iTypeOfNumber = receivedMsgMobileScTON;
            smsData.iServiceCentre.iNumberPlan = receivedMsgMobileScNPI;
            smsData.iServiceCentre.iTelNumber = receivedMsgServiceCentre;

            position += addressDataLength; // Start of the TPDU
            TUint8 totalLength( aFileData.Length() ); // Total aFileData length
            TUint8 tpduLength( totalLength - position );
            smsData.iMsgData = aFileData.Mid( position, tpduLength  );

            iSmsCache.AddEntryL( &smsData, iRecordId );
            }

        // if the cache is not ready, try reading the next entry
        if ( iRecordId < iSmsCache.TotalEntries() )
            {
            iRecordId++; // Next record
            UiccReadSMSOrSMSRecordCountReq( iRecordId, ETrIdReadSMS );
            }
        // or else this is the last entry, mark cache as ready
        else
            {
            iSmsCache.SetStatus( KErrNone );
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccReadSMSResp - abort caching");
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_UICCREADSMSRESP, "CMmSmsMessHandler::UiccReadSMSResp - abort caching" );
        // else SIM suddenly removed ==> abort caching
        // or example SIM rejected (EFSA-6ZQ9K3).

        // SMS is on SIM, but we couldn't read it. Cache must be
        // invalidated
        iSmsCache.Reset();
        // use KErrNoMemory as error condition, so that getinfo will try
        // to fill cache again
        iSmsCache.SetStatus( KErrNoMemory );
        }

    // either done or failed
    if ( KErrNotReady != iSmsCache.Status() )
        {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccReadSMSResp - Success or Error value of SMS cache read = %d",iSmsCache.Status() );
OstTrace1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_UICCREADSMSRESP, "CMmSmsMessHandler::UiccReadSMSResp;iSmsCache.Status()=%d", iSmsCache.Status() );

TFLOGSTRING3("TSY: CMmSmsMessHandler::UiccReadSMSResp --- SMS CACHE READ DONE --- total=%d used=%d",iSmsCache.TotalEntries(), iSmsCache.UsedEntries() );
OstTrace1( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_UICCREADSMSRESP, "CMmSmsMessHandler::UiccReadSMSResp;iSmsCache.TotalEntries()=%d", iSmsCache.TotalEntries() );

        // there is a chance that EMobileSmsMessagingGetMessageStoreInfo
        // request is already on if so, then complete it here... if there is
        // no request on, then this is harmless..
        TUint8 numOfLoc( TUint8( iSmsCache.TotalEntries() ) );
        TInt usedEntries( iSmsCache.UsedEntries() );

        //create package
        CMmDataPackage data;
        data.PackData( &numOfLoc, &usedEntries );
        iSmsSlotLocation = 0;

        iMessageRouter->Complete(
            EMobileSmsMessagingGetMessageStoreInfo,
            &data,
            iSmsCache.Status() );

        iMessageRouter->Complete(
            EMobilePhoneStoreGetInfo,
            &data,
            iSmsCache.Status() );

        // it is possible that re-caching was done due to sim refresh.
        iMessageRouter->GetPhoneMessHandler()->
            SmsCachingCompleted( iSmsCache.Status() );

        // if cache is up and a message was received while cacheing then resume
        if ( KErrNone == iSmsCache.Status() && iReceivedClass2ToBeReSent )
            {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccReadSMSResp -- resume SMS reception");
OstTrace0( TRACE_NORMAL, DUP5_CMMSMSMESSHANDLER_UICCREADSMSRESP, "CMmSmsMessHandler::UiccReadSMSResp -- resume SMS reception" );
            SmsReceiveMessageReq( SMS_RECEPTION_STORAGE_STATUS_UPDATE );
            iReceivedClass2ToBeReSent = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccReadSMSRespForComplete
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::UiccReadSMSRespForComplete(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccReadSMSRespForComplete" );
//OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCREADSMSRESP, "CMmSmsMessHandler::UiccReadSMSResp;aStatus=%d;aFileData=%s", aStatus,aFileData );


    if ( KErrNone == aStatus )
        {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccReadSMSRespForComplete -- cacheing iRecordId=%d", iRecordId );
OstTraceExt1( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCREADSMSRESPFORCOMPLETE, "CMmSmsMessHandler::UiccReadSMSRespForComplete;iRecordId=%hhu", iRecordId );

        if ( 0 != aFileData.Length() )
            {
            // add to cache
            RMobileSmsStore::TMobileGsmSmsEntryV1 smsData;

            TUint8 position( 0 );
            // Force status as message to be read
            smsData.iMsgStatus =
                static_cast <RMobileSmsStore::TMobileSmsStoreStatus>(
                    aFileData[position] );

            TBuf<KMaxAddressBufferSize> receivedMsgServiceCentre;
            RMobilePhone::TMobileTON receivedMsgMobileScTON(
                RMobilePhone::EUnknownNumber );
            RMobilePhone::TMobileNPI receivedMsgMobileScNPI(
                RMobilePhone::EUnknownNumberingPlan );

            position++;
            TUint8 addressDataLength( aFileData[position] );
            position++;
            TPtrC8 scA( aFileData.Mid( position, addressDataLength ) );
            CMmSmsGsmAddress::GsmConv0411AddrToUnicode(
                receivedMsgServiceCentre,
                scA,
                receivedMsgMobileScTON,
                receivedMsgMobileScNPI );

            smsData.iServiceCentre.iTypeOfNumber = receivedMsgMobileScTON;
            smsData.iServiceCentre.iNumberPlan = receivedMsgMobileScNPI;
            smsData.iServiceCentre.iTelNumber = receivedMsgServiceCentre;

            position += addressDataLength; // Start of the TPDU
            TUint8 totalLength( aFileData.Length() ); // Total aFileData length
            TUint8 tpduLength( totalLength - position );
            smsData.iMsgData = aFileData.Mid( position, tpduLength  );

            // This is done only when SMS written through ETel.
            if ( !iSMSClass2Write )
                {
                //create package
                CMmDataPackage data;
                data.PackData( &iSmsSlotLocation, &iReceivedClass2ToBeReSent );
                iSmsSlotLocation = 0;
                iMessageRouter->Complete(
                    EMobilePhoneStoreWrite,
                    &data,
                    KErrNone );

                iReceivedClass2ToBeReSent = EFalse;
                }
            else
                {
                // This is done only when received class 2 message has been handled.
                TSmsMsg smsMsg;
                smsMsg.iLocation = iSmsSlotLocation;
                smsMsg.iMessageStatus = smsData.iMsgStatus;
                smsMsg.iMobileScNPI = smsData.iServiceCentre.iNumberPlan;
                smsMsg.iMobileScTON = smsData.iServiceCentre.iTypeOfNumber;
                smsMsg.iServiceCentre = smsData.iServiceCentre.iTelNumber;
                smsMsg.iSmsClass2 = ETrue;
                smsMsg.iSmsMsg = smsData.iMsgData;

                iSmsSlotLocation = 0;

                //Complete request to client
                TBool smsInd( ETrue );
                TSmsMsg* smsMsgPtr = &smsMsg;

                //create package
                CMmDataPackage data;
                data.PackData( &smsInd, &smsMsgPtr );
                iMessageRouter->Complete(
                    EMobileSmsMessagingReceiveMessage,
                    &data,
                    KErrNone );
                }
            // At this point sms is safely in SIM and request is completed
            // Add to cache may fail, in which case we just invalidate
            // cache in out of memory handler

            iSmsCache.AddEntryL( &smsData, iRecordId );
            }

        }
    else
        {
        // This is done only when received class 2 message has been handled.
        if ( iSMSClass2Write )
            {
            TInt ret ( SmsReceivedMsgReportReq(
                ESmsMessagingAckSmsStored,
                NULL,
                KErrNone ) );

            if ( KErrNone != ret )
                {
                TBool smsInd( EFalse );
                TSmsMsg* nullSms = NULL;

                //create package
                CMmDataPackage data;
                //Complete request to client
                data.PackData( &smsInd, &nullSms );
                iSmsSlotLocation = 0;

                // ISI message construction failed or phonet sender
                // returned error
                iMessageRouter->Complete(
                    EMobileSmsMessagingReceiveMessage,
                    &data,
                    KErrGeneral );
                }
            }
        else
            {
            //create package
            CMmDataPackage data;
            data.PackData( &iSmsSlotLocation, &iReceivedClass2ToBeReSent );
            iSmsSlotLocation = 0;
            iMessageRouter->Complete(
                EMobilePhoneStoreWrite,
                &data,
                CMmStaticUtility::UICCCSCauseToEpocError( aStatus ) );
            }
        // SMS is on SIM, but we couldn't read it. Cache must be
        // invalidated
        iSmsCache.Reset();
        // use KErrNoMemory as error condition, so that getinfo will try
        // to fill cache again
        iSmsCache.SetStatus( KErrNoMemory );
        }

    iSMSClass2Write = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccWriteSMSReq
// Write SMS to SIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::UiccWriteSMSReq(
    const RMobileSmsStore::TMobileGsmSmsEntryV1& aEntry,
    const TUint8 aRecordId
    )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccWriteSMSReq aRecordId: %d", aRecordId );
OstTraceExt1( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCWRITESMSREQ, "CMmSmsMessHandler::UiccWriteSMSReq;aRecordId=%hhu", aRecordId );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteSMS;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = aRecordId;

    params.fileId = KElemFileShortMessages;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    // File data to be updated.
    TBuf8<KSmsElemetaryFileRecordLength> fileDataBuf;

    // Fill record. See ETSI TS 31 102 "4.2.25   EFSMS (Short messages)"

    TUint8 totalDataLength( 0 );
    fileDataBuf.Append( KSimSmsMtNotRead );
    totalDataLength++;

    //build Gsm 04.11 type of address structure
    TBuf8<KMaxAddressBufferSize>  scAddress;
    TPtrC16 telNumber( aEntry.iServiceCentre.iTelNumber );
    CMmSmsGsmAddress::GsmConvUnicodeTo0411Addr(
        aEntry.iServiceCentre.iTypeOfNumber,
        aEntry.iServiceCentre.iNumberPlan,
        scAddress,
        telNumber );

    // addressDataLength
    fileDataBuf.Append( scAddress.Length() );
    totalDataLength++;

    fileDataBuf.Append( scAddress );

    // It is possible for a TS Service Centre Address of maximum permitted
    // length, e.g. containing more than 18 address digits, to be associated
    // with a maximum length TPDU such that their combined length is 176 bytes.
    // In this case the ME shall store in the USIM the TS Service Centre
    // Address and the TPDU in bytes 2 176 without modification, except for the
    // last byte of the TPDU, which shall not be stored.

    RMobileSmsMessaging::TMobileSmsGsmTpdu msgData( aEntry.iMsgData );

    totalDataLength += ( msgData.Length() + scAddress.Length() );

    TUint msgDataLen( msgData.Length() );
    TInt unfilledLength( fileDataBuf.MaxLength() - totalDataLength );

    if ( 0 >  unfilledLength )
        {
        msgDataLen--;
        }
    fileDataBuf.Append( msgData.Left( msgDataLen ) );

    // Fill unused fields with FF
    if ( 0 <  unfilledLength )
        {
        fileDataBuf.AppendFill( 0xFF, unfilledLength );
        }

    params.fileData.Append( fileDataBuf );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccWriteSMSResp
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::UiccWriteSMSResp( TInt aStatus )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccWriteSMSResp aStatus: %d", aStatus );
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCWRITESMSRESP, "CMmSmsMessHandler::UiccWriteSMSResp;aStatus=%d", aStatus );

    // Create Package
    CMmDataPackage package;

    if ( iSMSClass2Write )
        {
        if ( KErrNone == aStatus )
            {
            // After writing class2 SMS to SIM card, it has
            // to be read in case SIM SW has changed the contents
            TInt ret( UiccReadSMSOrSMSRecordCountReq( iSmsSlotLocation, ETrIdReadSMSForComplete ) );
            if ( KErrNone != ret )
                {
                ret = SmsReceivedMsgReportReq( EInternalNack, NULL, KErrGsmSMSUnspecifiedProtocolError );
                if ( KErrNone != ret )
                    {
                    TBool smsInd( EFalse );
                    TSmsMsg* nullSms = NULL;
                    //Complete request to client
                    package.PackData( &smsInd, &nullSms );
                    iSmsSlotLocation = 0;

                    // ISI message construction failed or phonet sender
                    // returned error
                    iMessageRouter->Complete(
                        EMobileSmsMessagingReceiveMessage,
                        &package,
                        KErrGeneral );
                    }
                }
            }
        else
            {
            TInt ret( SmsReceivedMsgReportReq( EInternalNack, NULL, KErrGsmSMSUnspecifiedProtocolError ) );
            if ( KErrNone != ret )
                {
                // ISI message construction failed or phonet sender
                // returned error
                TBool smsInd( EFalse );
                TSmsMsg* nullSms = NULL;

                //Pack data
                package.PackData( &smsInd, &nullSms );
                iSmsSlotLocation = 0;

                // ISI message construction failed or phonet sender
                // returned error
                iMessageRouter->Complete(
                    EMobileSmsMessagingReceiveMessage,
                    &package,
                    KErrGeneral );
                }
            }
        }
    else
        {
        if ( KErrNone == aStatus )
            {
            // After writing class2 SMS to SIM card, it has
            // to be read in case SIM SW has changed the contents
            TInt ret( UiccReadSMSOrSMSRecordCountReq( iSmsSlotLocation, ETrIdReadSMSForComplete ) );
            if ( KErrNone != ret )
                {
                package.PackData(
                    &iSmsSlotLocation,
                    &iReceivedClass2ToBeReSent );
                iSmsSlotLocation = 0;
                iMessageRouter->Complete(
                    EMobilePhoneStoreWrite,
                    &package,
                    ret );
                iReceivedClass2ToBeReSent = EFalse;
                }
            }
        else
            {
            package.PackData( &iSmsSlotLocation, &iReceivedClass2ToBeReSent );
            iSmsSlotLocation = 0;
            iMessageRouter->Complete(
                EMobilePhoneStoreWrite,
                &package,
            KErrGeneral );
            iReceivedClass2ToBeReSent = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::GetNumOfEFSMSRecords
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::GetNumOfEFSMSRecords( void )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::GetNumOfEFSMSRecords" );
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_GETNUMOFEFSMSRECORDS, "CMmSmsMessHandler::GetNumOfEFSMSRecords" );

    TUint8 recordID( 0 );
    TUiccTrId trId( ETrIdReadSMSRecordCount );

    CMmSmsMessHandler::UiccReadSMSOrSMSRecordCountReq ( recordID,
                                                        trId  );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::GetNumOfEFSMSRecordsResp
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::GetNumOfEFSMSRecordsResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::GetNumOfEFSMSRecordsResp" );
OstTraceExt2( TRACE_NORMAL, CMMSMSMESSHANDLER_GETNUMOFEFSMSRECORDSRESP, "CMmSmsMessHandler::GetNumOfEFSMSRecordsResp;aStatus=%d;aFileData=%s", aStatus, aFileData );

    //Save number of SMS locations on SIM card
    TInt smsNumOfLoc( 0 );

    if ( KErrNone == aStatus )
        {
        TFci fci( aFileData );
        smsNumOfLoc = fci.GetNumberOfRecords();
        }

    // Continue with reading all sms entries from sim
    if ( smsNumOfLoc > 0 )
        {
        // got the total number or SIM SMS entries
        iSmsCache.SetTotalEntriesL( smsNumOfLoc );
TFLOGSTRING2("TSY: CMmSmsMessHandler::SimStSmsGetNumOfLocRespL -- total number of locations on sim %d", iSmsCache.TotalEntries() );
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_GETNUMOFEFSMSRECORDSRESP, "CMmSmsMessHandler::GetNumOfEFSMSRecordsResp;iSmsCache.TotalEntries()=%d", iSmsCache.TotalEntries() );

        // now start reading entries one by one starting from record 1
        UiccReadSMSOrSMSRecordCountReq( 1, ETrIdReadSMS );
        }
    else
        {
        // There is a chance that getinfo requests are already ongoing
        // if so, then complete them here.
        TUint8 numOfLoc( 0 );
        TInt usedEntries( -1 ); // unable to determine.
        CMmDataPackage data;
        data.PackData( &numOfLoc, &usedEntries );
        iMessageRouter->Complete(
            EMobileSmsMessagingGetMessageStoreInfo,
            &data,
            CMmStaticUtility::UICCCSCauseToEpocError( aStatus )  );

        iMessageRouter->Complete(
            EMobilePhoneStoreGetInfo,
            &data,
            KErrNoMemory );
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccDeleteSMSResp
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::UiccDeleteSMSResp( TInt aStatus )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccDeleteSMSResp aStatus: %d", aStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_UICCDELETESMSRESP, "CMmSmsMessHandler::UiccDeleteSMSResp;aStatus=%d", aStatus );

    // Create Package
    CMmDataPackage package;

    TInt error( CMmStaticUtility::UICCCSCauseToEpocError( aStatus ) );

    if ( KErrNone == error )
        {
        iSmsCache.Delete();
        }
    iMessageRouter->Complete( EMobilePhoneStoreDelete, error );

    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccDeleteAllSMSResp
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::UiccDeleteAllSMSResp( TInt aStatus )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccDeleteSMSResp aStatus: %d", aStatus );
OstTrace1( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCDELETEALLSMSRESP, "CMmSmsMessHandler::UiccDeleteAllSMSResp;aStatus=%d", aStatus );

    // Create Package
    CMmDataPackage package;

    TInt error( CMmStaticUtility::UICCCSCauseToEpocError( aStatus ) );

    // Delete SMS from SIM one by one and then all from cache.
    if ( ( iRecordId < iSmsCache.TotalEntries() ) &&
         ( KErrNone == error ) )
        {
        iRecordId++; // Next record
        error = DeleteAllSms();

        if ( KErrNone != error )
            {
            iMessageRouter->Complete( EMobilePhoneStoreDeleteAll, error );
            }
        }
    else
        {
        iSmsCache.DeleteAll();
        iMessageRouter->Complete( EMobilePhoneStoreDeleteAll, error );
        }

    }
// -----------------------------------------------------------------------------
// CMmSmsMessHandler::CheckSCTimestamp
// Returns ETrue if the SC timestamp in this message is equal to
// the client's timestamp in aScTime.
// -----------------------------------------------------------------------------
//
TBool CMmSmsMessHandler::CheckSCTimestamp(
    const RMobileSmsStore::TMobileGsmSmsEntryV1& aEntry,
    const TTime& aScTime )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::CheckSCTimestamp");
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp" );
    TBool matchFound( EFalse );

    TInt offset( 1 ); // Message Reference position
    TInt messageReference( aEntry.iMsgData[offset] );
    // Destination address length is integer representation
    // of the number of useful semi-octets of address field
    messageReference = ( messageReference + 1 ) / 2;
    offset += messageReference;
    offset += 4; // Service Centre Time Stamp

    TBuf8<KSmsScTimeStampMaxLength> scTimeStamp;
    scTimeStamp = aEntry.iMsgData.Mid( offset, KSmsScTimeStampMaxLength );

    // Convert from the semi-octet representation to decimals.
    for ( TUint8 i ( 0 ); i < KSmsScTimeStampMaxLength; i++ )
        {
        TUint8 lowDigit( static_cast<TUint8>( (
            scTimeStamp[i] & 0xf0 ) >> 4 ) );
        TUint8 highDigit( static_cast<TUint8>( scTimeStamp[i] & 0xf ) );
        scTimeStamp[i] = static_cast<TUint8>( (10 * highDigit) + lowDigit );
        }

    // Ignore this messsage if month or day are zero (should not happen)
    if ( ( 0 == scTimeStamp[1] ) || ( 0 == scTimeStamp[2] ) )
        {
TFLOGSTRING("TSY:CMmSmsMessHandler::CheckSCTimestamp:Invalid timestamp found, ignored.");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp, Invalid timestamp found, ignored" );
        matchFound = ETrue;
        }

    if ( !matchFound )
        {
        // Construct a string of the form YYYYMMDD:HHMMSS
        TBuf<15> timeString;
        // This is the same way Symbian GSMU does it,
        // see GSMU's method TSmsServiceCenterTimeStamp::DecodeL.
        if ( scTimeStamp[0]>95 )
            {
            timeString.Append( _L( "19" ) );
            }
        else
            {
            timeString.Append( _L( "20" ) );
            }
        //two digits for year, month, day, hour, minute, second
        _LIT( KDateFormat, "%02d" );
        //year
        timeString.AppendFormat( KDateFormat,scTimeStamp[0] );
        //month, starting at 0
        timeString.AppendFormat( KDateFormat,scTimeStamp[1]-1 );
        //day, starting at 0
        timeString.AppendFormat( KDateFormat,scTimeStamp[2]-1 );
        timeString.Append( _L(":" ) );
        //hour
        timeString.AppendFormat( KDateFormat,scTimeStamp[3] );
        //minute
        timeString.AppendFormat( KDateFormat,scTimeStamp[4] );
        //second
        timeString.AppendFormat( KDateFormat,scTimeStamp[5] );

        TTime simStoredTime( 0 );
        TInt err( simStoredTime.Set( timeString ) );

        if ( KErrNone == err )
            {
            // Handle the timezone difference. The timezone is found in the
            // last byte of the SC timestamp buffer.
            TUint8 simTimezoneDiffRaw( scTimeStamp[6] );
            // Highest bit is the algebraic sign (0=positive, 1=negative).
            // Timezone is defined in 'quarters of an hour', but we allow
            // only whole numbers.
            TInt8 simTimezoneSign( ( simTimezoneDiffRaw & 0x80 ) ? -1 : 1 );
            TInt8 simTimezoneDiff( static_cast<TInt8>(
                simTimezoneSign * ( simTimezoneDiffRaw & 0x7F) / 4 ) );
            // Substract timezone difference
            TTimeIntervalHours simTimezoneInterval( simTimezoneDiff );
            simStoredTime -= simTimezoneInterval;

#ifdef _DEBUG
            // debug print
            TDateTime dt = simStoredTime.DateTime();
TFLOGSTRING("TSY:CMmSmsMessHandler::CheckSCTimestamp:Message stored on SIM:");
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp, Message stored on SIM:" );
TFLOGSTRING2("    year=%d",dt.Year());
OstTrace1( TRACE_NORMAL, DUP3_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Year=%d", dt.Year() );
TFLOGSTRING2("    month=%d",dt.Month()+1);
OstTrace1( TRACE_NORMAL, DUP4_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Month=%d", ( dt.Month() + 1 ) );
TFLOGSTRING2("    day=%d",dt.Day()+1);
OstTrace1( TRACE_NORMAL, DUP5_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Day=%d", ( dt.Day() + 1 ) );
TFLOGSTRING2("    hour=%d",dt.Hour());
OstTrace1( TRACE_NORMAL, DUP6_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Hour=%d", dt.Hour() );
TFLOGSTRING2("    minute=%d",dt.Minute());
OstTrace1( TRACE_NORMAL, DUP7_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Minute=%d", dt.Minute() );
TFLOGSTRING2("    second=%d",dt.Second());
OstTrace1( TRACE_NORMAL, DUP8_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Second=%d", dt.Second() );
TFLOGSTRING2("    timezone difference=%d",simTimezoneDiff);
OstTraceExt1( TRACE_NORMAL, DUP9_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;simTimezoneDiff=%hhd", simTimezoneDiff );
            dt = aScTime.DateTime();
TFLOGSTRING("TSY:CMmSmsMessHandler::CheckSCTimestamp:Message opened on client side:");
OstTrace0( TRACE_NORMAL, DUP10_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp, Message opened on client side:" );
TFLOGSTRING2("    year=%d",dt.Year());
OstTrace1( TRACE_NORMAL, DUP11_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Year=%d", ( dt.Year() + 1 ) );
TFLOGSTRING2("    month=%d",dt.Month()+1);
OstTrace1( TRACE_NORMAL, DUP12_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Month=%d", ( dt.Month() + 1 ) );
TFLOGSTRING2("    day=%d",dt.Day()+1);
OstTrace1( TRACE_NORMAL, DUP13_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Day=%d", ( dt.Day() + 1 ) );
TFLOGSTRING2("    hour=%d",dt.Hour());
OstTrace1( TRACE_NORMAL, DUP14_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Hour=%d", dt.Hour() );
TFLOGSTRING2("    minute=%d",dt.Minute());
OstTrace1( TRACE_NORMAL, DUP15_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Minute=%d", dt.Minute() );
TFLOGSTRING2("    second=%d",dt.Second());
OstTrace1( TRACE_NORMAL, DUP16_CMMSMSMESSHANDLER_CHECKSCTIMESTAMP, "CMmSmsMessHandler::CheckSCTimestamp;Second=%d", dt.Second() );
            // debug print
#endif // _DEBUG

            if ( simStoredTime == aScTime )
                {
                matchFound = ETrue;
                }
            }
        }

    return matchFound;

    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccUpdateSMSStatus
// Write SMS to SIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::UiccUpdateSMSStatus(
    const TUint8 aRecordId
    )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccUpdateSMSStatusReq aRecordId: %d", aRecordId );
OstTraceExt1( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCUPDATESMSSTATUS, "CMmSmsMessHandler::UiccUpdateSMSStatus;aRecordId=%hhu", aRecordId );

    // Read parameters from SIM
    // Set parameters for UICC_APPL_CMD_REQ message
    iRecordId = aRecordId;
    TUiccReadLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdUpdateSMSStatusReadSMS;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = aRecordId;

    params.fileId = KElemFileShortMessages;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );

    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccUpdateSMSStatusReadSMSResp
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::UiccUpdateSMSStatusReadSMSResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccReadSMSResp" );
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCUPDATESMSSTATUSREADSMSRESP, "CMmSmsMessHandler::UiccUpdateSMSStatusReadSMSResp" );

    if ( KErrNone == aStatus )
        {
        if ( 0 != aFileData.Length() )
            {
           // Set parameters for UICC_APPL_CMD_REQ message
            TUiccWriteLinearFixed params;
            params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
            params.trId = ETrIdUpdateSMSStatusWriteSMS;
            params.dataOffset = 0;
            params.dataAmount = 0;
            params.record = iRecordId;

            params.fileId = KElemFileShortMessages;
            params.fileIdSfi = UICC_SFI_NOT_PRESENT;
            params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

            // File id path
            params.filePath.Append( KMasterFileId >> 8 );
            params.filePath.Append( KMasterFileId );
            params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

            // Update status as read
            // File data to be updated.
            TBuf8<KSmsElemetaryFileRecordLength> fileDataBuf;
            fileDataBuf = aFileData.Left( aFileData.Length() );
            fileDataBuf[0] = KSimSmsMtRead;

            TInt unfilledLength( fileDataBuf.MaxLength() - aFileData.Length() );
            // Fill unused fields with FF
            if ( 0 <  unfilledLength )
                {
                fileDataBuf.AppendFill( 0xFF, unfilledLength );
                }

            params.fileData.Append( fileDataBuf );

            TInt ret( iMmUiccMessHandler->CreateUiccApplCmdReq( params ) );

            if ( KErrNone != ret )
                {
                iMessageRouter->Complete(
                    ECustomSetSimMessageStatusReadIPC,
                    ret );
                }
            }
        }
    else
        {
        iMessageRouter->Complete(
            ECustomSetSimMessageStatusReadIPC,
            CMmStaticUtility::UICCCSCauseToEpocError( aStatus )  );
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccUpdateSMSStatusWriteSMSResp
//
// -----------------------------------------------------------------------------
//
void CMmSmsMessHandler::UiccUpdateSMSStatusWriteSMSResp( TInt aStatus )
    {
TFLOGSTRING2("TSY: CMmSmsMessHandler::UiccUpdateSMSStatusReadSMSResp aStatus: %d", aStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_UICCUPDATESMSSTATUSREADSMSRESP, "CMmSmsMessHandler::UiccUpdateSMSStatusReadSMSResp;aStatus=%d", aStatus );

    iMessageRouter->Complete(
        ECustomSetSimMessageStatusReadIPC,
        CMmStaticUtility::UICCCSCauseToEpocError( aStatus )  );

    // Update the same flag also in TSY's cache.
    if ( KErrNone == aStatus )
        {
        iSmsCache.SetStorageStatus( iRecordId, RMobileSmsStore::EStoredMessageRead );
        }

    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccGetSmspEntryReq
//
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::UiccGetSmspEntryReq()
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccGetSmspEntryReq" );
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCREADNUMOFSMSPENTRIES, "CMmSmsMessHandler::UiccGetSmspEntryReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdGetSmspEntries;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = iSmspRecordNumber;
    params.fileId = KElemFileSmsParams;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccStoreSmspEntry
//
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::UiccStoreSmspEntry( const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccStoreSmspEntry" );
OstTrace0( TRACE_NORMAL, CMMSMSMESSHANDLER_UICCSTORESMSPENTRY, "CMmSmsMessHandler::UiccStoreSmspEntry" );

    TInt ret( KErrNone );
    TInt dataLength( aFileData.Length() );
    TPtrC8 data( KNullDesC8 );
    TSmsParameters* smsParam = new ( ELeave ) TSmsParameters();
    CleanupStack::PushL( smsParam );

    // SMS parameters are located in EF smsp, see 3GPP TS 31.102
    // V8.3.0 chapter 4.2.27 EFSMSP (Short message service parameters)

    // Alpha identifier is optional. Check if it exists.
    if ( KSmsMandatoryParamsLength < dataLength )
        {
        // Copy mandatory fields to different buffer for later use
        data.Set( aFileData.Right( KSmsMandatoryParamsLength ) );
        smsParam->iAlphaTagPresent = ETrue;

        // Alpha identifier length
        TInt alphaLength( dataLength - KSmsMandatoryParamsLength );
        if ( RMobileSmsMessaging::KMaxSmspTextSize < alphaLength )
            {
            alphaLength = RMobileSmsMessaging::KMaxSmspTextSize;
            }

        // Copy alpha identifier data as it is in SIM elementary file
        TBuf8<RMobileSmsMessaging::KMaxSmspTextSize> alphaIdentifier;
        alphaIdentifier.Copy( aFileData.Left( alphaLength ) );

        // Coding of alpha identifier may be 7-bit default of one of UCS2 code
        // options. At first convert data to 16-bit format
        TBuf8<RMobileSmsMessaging::KMaxSmspTextSize> alphaIdentifierOutput;
        CMmStaticUtility::ConvertGsmDataToUcs2(
            alphaIdentifier,
            alphaIdentifier.Length(),
            alphaIdentifierOutput );

        // Convert to 16-bit UNICODE string and copy to SMS parameters buffer
        TBuf16<RMobileSmsMessaging::KMaxSmspTextSize> alphaBuf;
        TIsiUtility::CopyFromBigEndian( alphaIdentifierOutput, alphaBuf );
        smsParam->iAlphaTagData.Copy( alphaBuf );
        }
    // Only mandatory 28 bytes are included, no alpha identifier
    else if ( KSmsMandatoryParamsLength == dataLength )
        {
        smsParam->iAlphaTagPresent = EFalse;
        // Copy all the data
        data.Set( aFileData );
        }
    else
        {
        ret = KErrNotFound;
        }

    if ( KErrNone == ret )
        {
        // Parameters indicator.
        // In SIM card bit value "0" means that parameter is present,
        // in S60 vice versa
        TUint8 paramIndicators( data[0] );
        paramIndicators = ~paramIndicators;
        paramIndicators &= KSmsGsmParametersIndMask; // Mask 5 LSB
        smsParam->iParameterIndicator = paramIndicators;

        // Location where SMS parameters in EFsms were fetched
        smsParam->iLocationNumber = iSmspRecordNumber;

        // Service center address
        if ( paramIndicators & KServiceCentreAddress )
            {
            TBuf8<KSmsMaxAddressLength>scAddress( data. Mid(
                KSmsParamsScAddressOffset, KSmsMaxAddressLength ) );
            CMmSmsGsmAddress::GsmConv0411AddrToUnicode(
                smsParam->iServiceCenterAddress,
                scAddress,
                smsParam->iMobileScTON,
                smsParam->iMobileScNPI );
            }

        // Destination address
        if ( paramIndicators & KDestinationAddress )
            {
            TBuf8<KSmsMaxAddressLength>destAddress( data.Mid(
                KSmsParamsDestAddressOffset, KSmsMaxAddressLength ) );
            CMmSmsGsmAddress::GsmConv0340AddrToUnicode(
                 smsParam->iDestinationAddress,
                 destAddress,
                 smsParam->iMobileDeTON,
                 smsParam->iMobileDeNPI );
            }

        // Protocol ID
        if ( paramIndicators & KProtocolID )
            {
            smsParam->iProtocolId = data[KSmsParamsProtocolIdOffset];
            }

        // Data coding scheme
        if ( paramIndicators & KDataCodingScheme )
            {
            smsParam->iDataCodingScheme = data[KSmsParamsDcsOffset];
            }

        // Validity period
        if ( paramIndicators & KValidityPeriod )
            {
            smsParam->iValidityPeriod = data[KSmsParamsValidityPeriodOffset];
            }

        // Add this parameter set to array
        iSmspListArray->AppendL( smsParam );
        // Read the next entry
        iSmspRecordNumber++;
        ret = UiccGetSmspEntryReq();
        }
    // Don't reset struct because it may be stored to CArrayPtrFlat.
    // All structs will be reseted when request is completed.
    CleanupStack::Pop( smsParam );
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSmsMessHandler::UiccSmsUpdateParameterReq
// -----------------------------------------------------------------------------
//
TInt CMmSmsMessHandler::UiccSmsUpdateParameterReq(
    const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING("TSY: CMmSmsMessHandler::UiccSmsUpdateParameterReq");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSMESSHANDLER_UICCSTORESMSPENTRY, "CMmSmsMessHandler::UiccStoreSmspEntry" );

    RMobileSmsMessaging::TMobileSmspEntryV1* smsParameters;
    // Unpack data
    aDataPackage->UnPackData( &smsParameters );

    // Buffer for all the file data to be written to UICC
    // (possible alpha tag + params )
    TBuf8<KSmsMandatoryParamsLength + KSizeOfAlphaTag> smspBuffer;

    // Check if there is alpha tag
    TInt alphaTagLength( smsParameters->iText.Length() );
    // Buffer for alpha tag
    if ( 0 < alphaTagLength )
        {
        // Temporary buffer for alpha tag
        TBuf8<KSizeOfAlphaTag> alphaTagData;
        TIsiUtility::CopyToBigEndian( smsParameters->iText, alphaTagData );
        smspBuffer.Append( 0x80 ); // First byte 0x80 means 16-bit UCS coding
        smspBuffer.Append( alphaTagData );
        }

    // Temporary buffer for mandatory data, filled by FF (unused bytes)
    TBuf8<KSmsMandatoryParamsLength> paramsData;
    paramsData.Fill( 0xFF, KSmsMandatoryParamsLength );

    // Fill SMS parameters to data buffer ( see 3GPP TS 31.102 V8.3.0
    // 4.2.27  EFSMSP (Short message service parameters). Unused parameters
    // are filled by 0xFF

    // Parameter indicator
    TUint8 paramInd( static_cast<TUint8>( smsParameters->iValidParams ) );
    paramsData[KSmsParamsParamIndicatorOffset] = paramInd;

    // Destination address
    if ( KDestinationAddress & paramInd )
        {
        TBuf8<KSmsMaxAddressLength> deAddr;
        // Fill address by 0x00 at first
        BuildDeAddress( smsParameters->iDestination, deAddr );
        // Add address data to buffer
        paramsData.Replace(
            KSmsParamsDestAddressOffset,
            deAddr.Length(),
            deAddr );
        }

    // Service center address
    if ( KServiceCentreAddress & paramInd )
        {
        TBuf8<KSmsMaxAddressLength> scAddr;
        // Fill address by 0x00 at first
        BuildScAddress( smsParameters->iServiceCentre, scAddr );
        // Add address data to buffer
        paramsData.Replace(
            KSmsParamsScAddressOffset,
            scAddr.Length(),
            scAddr );
        }

    // Protocol ID
    if ( KProtocolID & paramInd )
        {
        paramsData[KSmsParamsProtocolIdOffset] = smsParameters->iProtocolId;
        }

    // Data coding scheme
    if ( KDataCodingScheme & paramInd )
        {
        paramsData[KSmsParamsDcsOffset] = smsParameters->iDcs;
        }

    // If validity period is valid, set value
    if ( KValidityPeriod & paramInd )
        {
        paramsData[KSmsParamsValidityPeriodOffset] =
            smsParameters->iValidityPeriod;
        }

    smspBuffer.Append( paramsData );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteSmspEntry;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = smsParameters->iIndex;

    params.fileId = KElemFileSmsParams;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    // File data
    params.fileData.Append( smspBuffer );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

//  End of File
