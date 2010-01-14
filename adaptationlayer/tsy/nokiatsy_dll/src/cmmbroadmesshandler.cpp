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
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <tisi.h>
#include <smsisi.h>
#include "cmmbroadmesshandler.h"
#include "cmmmessagerouter.h"
#include "tsylogger.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmbroadmesshandlerTraces.h"
#endif


// EXTERNAL DATA STRUCTURES
    //none

// EXTERNAL FUNCTION PROTOTYPES
    //none

// CONSTANTS
const TUint8 KBroadPadding( 0x00 );

// MACROS
    //none
//  LOCAL CONSTANTS AND MACROS
    //none

//  MODULE DATA STRUCTURES
    //none

// LOCAL FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
    //none

// ============================= LOCAL FUNCTIONS ===============================
    //none

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmBroadMessHandler::CMmBroadMessHandler
// C++ constructor
// -----------------------------------------------------------------------------
//
CMmBroadMessHandler::CMmBroadMessHandler()
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::CMmBroadMessHandler");
OstTrace0( TRACE_NORMAL, CMMBROADMESSHANDLER_CMMBROADMESSHANDLER, "CMmBroadMessHandler::CMmBroadMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmSmsMessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmBroadMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMBROADMESSHANDLER_CONSTRUCTL, "CMmBroadMessHandler::ConstructL" );
    // Initialise the array. Maximun of pages in a WCDMA CBS message is 15
    iCbsMsg = new( ELeave ) CArrayPtrFlat< TWcdmaCbsMsg >( 10 );

    // Cb subscription number.
    iCbSubscriptionNumber = SMS_NEW_SUBSCRIPTION;
    }

// -----------------------------------------------------------------------------
// CMmBroadMessHandler:: NewL
// Two-phased constructor.
// Creates a new MessageHandler object instance
// -----------------------------------------------------------------------------
//
CMmBroadMessHandler* CMmBroadMessHandler::NewL
    (
    CMmPhoNetSender* aPhoNetSender,     // a pointer to the PhonetSender
    CMmPhoNetReceiver* aPhoNetReceiver, // a pointer to the phonetReceiver
    CMmMessageRouter* aMessageRouter    // a pointer to the Message router
    )
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMBROADMESSHANDLER_NEWL, "CMmBroadMessHandler::NewL" );
    CMmBroadMessHandler* mmBroadMessHandler =
        new( ELeave ) CMmBroadMessHandler();

    CleanupStack::PushL( mmBroadMessHandler );
    mmBroadMessHandler->iPhoNetSender = aPhoNetSender;
    mmBroadMessHandler->iMessageRouter = aMessageRouter;

    aPhoNetReceiver->RegisterL(
        mmBroadMessHandler,
        PN_SMS,
        SMS_CB_ROUTING_RESP );
    aPhoNetReceiver->RegisterL(
        mmBroadMessHandler,
        PN_SMS,
        SMS_CB_ROUTING_IND );

    mmBroadMessHandler->ConstructL();
    CleanupStack::Pop( mmBroadMessHandler );

    return mmBroadMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmBroadMessHandler:: ~CMmBroadMessHandler
// C++ destructor destroys all objects which are used
// -----------------------------------------------------------------------------
//
CMmBroadMessHandler::~CMmBroadMessHandler()
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::~CMmBroadMessHandler");
OstTrace0( TRACE_NORMAL, DUP1_CMMBROADMESSHANDLER_CMMBROADMESSHANDLER, "CMmBroadMessHandler::~CMmBroadMessHandler" );
    if ( iCbsMsg )
        {
        iCbsMsg->ResetAndDestroy();
        delete iCbsMsg;
        }
    }

// -----------------------------------------------------------------------------
// CMmBroadMessHandler:: ReceiveMessageL
// Called by PhonetReceiver when an ISI message has been received
// -----------------------------------------------------------------------------
//
void CMmBroadMessHandler::ReceiveMessageL( const TIsiReceiveC& aIsiMessage )
    {
    TUint8 resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TUint8 messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

TFLOGSTRING3("TSY: CMmBroadMessHandler::ReceiveMessageL. Resource:%d, MsgId:%d", resource,messageId);
OstTraceExt2( TRACE_NORMAL, CMMBROADMESSHANDLER_RECEIVEMESSAGEL, "CMmBroadMessHandler::ReceiveMessageL;resource=%hhd;messageId=%hhx", resource, messageId );

    switch ( resource )
        {
        case PN_SMS:
            {
TFLOGSTRING("TSY: CMmBroadMessHandler::ReceiveMessageL - PN_SMS");
OstTrace0( TRACE_NORMAL, DUP1_CMMBROADMESSHANDLER_RECEIVEMESSAGEL, "CMmBroadMessHandler::ReceiveMessageL - PN_SMS" );

            switch( messageId )
                {
                case SMS_CB_ROUTING_RESP:
                    {
                    SmsCbRoutingResp( aIsiMessage );
                    break;
                    }
                case SMS_CB_ROUTING_IND:
                    {
                    SmsCbRoutingIndL( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmBroadMessHandler::ReceiveMessageL - PN_SMS - default");
OstTrace0( TRACE_NORMAL, DUP3_CMMBROADMESSHANDLER_RECEIVEMESSAGEL, "CMmBroadMessHandler::ReceiveMessageL - PN_SMS - default" );
                    // No appropriate handler methods for ISI-message found.
                    break;
                    }
                }
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmBroadMessHandler::ReceiveMessageL. Switch resource case default");
OstTrace0( TRACE_NORMAL, DUP2_CMMBROADMESSHANDLER_RECEIVEMESSAGEL, "CMmBroadMessHandler::ReceiveMessageL. Switch resource case default" );
            // No appropriate handler methods for ISI-message found.
            break;
            }
        }
    return;
    }


// -----------------------------------------------------------------------------
// CMmBroadMessHandler:: SmsCbRoutingRequest
// Send CB message routing request to the SMS server
// -----------------------------------------------------------------------------
//
TInt CMmBroadMessHandler::SmsCbRoutingRequest
    (
    TUint8 aTransId, // Transaction ID
    const CMmDataPackage* aDataPackage // Data Package
    )
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::SmsCbRoutingRequest");
OstTrace0( TRACE_NORMAL, CMMBROADMESSHANDLER_SMSCBROUTINGREQUEST, "CMmBroadMessHandler::SmsCbRoutingRequest" );

    // Variable for the routing command initialized
    TUint8 routingCommand( 0 );
    // Contents of the data package
    TCbsCbmiAndLangAndFilter data;
    // Unpack data
    aDataPackage->UnPackData( data );

    if ( RMobileBroadcastMessaging::EBroadcastAcceptNone == data.iSetting )
        {
        // Release routing
TFLOGSTRING("TSY:CMmBroadMessHandler::SmsCbRoutingRequest:CB routing will be released.");
OstTrace0( TRACE_NORMAL, DUP1_CMMBROADMESSHANDLER_SMSCBROUTINGREQUEST, "CMmBroadMessHandler::SmsCbRoutingRequest:CB routing will be released." );
        routingCommand = SMS_ROUTING_RELEASE;
        }
    else if ( RMobileBroadcastMessaging::EBroadcastAcceptAll == data.iSetting )
        {
        // Activate routing
TFLOGSTRING("TSY:CMmBroadMessHandler::SmsCbRoutingRequest:CB routing will be activated.");
OstTrace0( TRACE_NORMAL, DUP2_CMMBROADMESSHANDLER_SMSCBROUTINGREQUEST, "CMmBroadMessHandler::SmsCbRoutingRequest:CB routing will be activated." );
        routingCommand = SMS_ROUTING_SET;
        }
    else
        {
TFLOGSTRING2("TSY:CMmBroadMessHandler::SmsCbRoutingRequest:Unsupported filter 0x%x.",data.iSetting);
// Following lines flagged out just get rid of compiler warning when trace
// compiler is not in use.
#ifdef OST_TRACE_COMPILER_IN_USE
        TUint tempValue = data.iSetting; // Parameter just for tracing
OstTraceExt1( TRACE_NORMAL, DUP3_CMMBROADMESSHANDLER_SMSCBROUTINGREQUEST, "CMmBroadMessHandler::SmsCbRoutingRequest;data.iSetting=%hhx", tempValue );
#endif // OST_TRACE_COMPILER_IN_USE
        return KErrArgument;
        }

    // Create a buffer to hold the request
    TBuf8<SIZE_SMS_CB_ROUTING_REQ> dataBuffer;
    dataBuffer.Append( routingCommand ); // Routing command

    if ( SMS_ROUTING_RELEASE == routingCommand )
        {
        // Subscription number
        dataBuffer.Append( iCbSubscriptionNumber );
        }
    else if ( SMS_ROUTING_SET == routingCommand )
        {
        // Subscription number
        dataBuffer.Append( SMS_NEW_SUBSCRIPTION );
        }

    dataBuffer.Append( SMS_TYPE_DEFAULT ); // Subscription type
    dataBuffer.Append( KBroadPadding ); // Filler
    dataBuffer.Append( KBroadPadding ); // Filler
    dataBuffer.Append( 0 ); // Number of subblocks

    return iPhoNetSender->Send(
        PN_SMS,
        aTransId,
        SMS_CB_ROUTING_REQ,
        dataBuffer );
   }


// -----------------------------------------------------------------------------
// CMmBroadMessHandler:: SmsCbRoutingResp
// Response for SmsCbRoutingReq.
// Response doesn't include CB message
// -----------------------------------------------------------------------------
//
void CMmBroadMessHandler::SmsCbRoutingResp
    (
    const TIsiReceiveC& aSmsCbRoutingResp // Received isimsg
    )
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::SmsCbRoutingResp");
OstTrace0( TRACE_NORMAL, CMMBROADMESSHANDLER_SMSGSMCBROUTINGRESP, "CMmBroadMessHandler::SmsCbRoutingResp" );
    TInt ipc( 0 ); // Initialize to zero

    iCbSubscriptionNumber = aSmsCbRoutingResp.Get8bit(
        ISI_HEADER_SIZE + SMS_CB_ROUTING_RESP_OFFSET_SUBSCRIPTIONNUMBER );

    TUint8 isiCause( aSmsCbRoutingResp.Get8bit( ISI_HEADER_SIZE
        + SMS_CB_ROUTING_RESP_OFFSET_SMSCAUSE ) );

    TInt cause( CMmStaticUtility::CSCauseToEpocError(
        PN_SMS,
        SMS_CAUSE_TYPE_COMMON,
        isiCause ) );

    TUint8 traid( aSmsCbRoutingResp.Get8bit( ISI_HEADER_SIZE
        + SMS_CB_ROUTING_RESP_OFFSET_TRANSID ) );

    // Response for SmsGsmCbRoutingReq (receive message)
    if ( EBroadcastMessagingReceiveMessage == traid )
        {
        ipc = EMobileBroadcastMessagingReceiveMessage;
        }
    // Response for SmsGsmCbRoutingReq (receive message cancel)
    else if ( EBroadcastMessagingReceiveMessageCancel == traid )
        {
        ipc = EMobileBroadcastMessagingReceiveMessageCancel;
        }
    // Response for SmsGsmCbRoutingReq (set filter setting)
    else if ( EBroadcastMessagingSetFilterSetting == traid )
        {
        ipc = EMobileBroadcastMessagingSetFilterSetting;
        }

    // Complete status change indication, if ipc is set
    if ( 0 != ipc )
        {
        iMessageRouter->Complete( ipc, cause );
        }

    return;
    }

// -----------------------------------------------------------------------------
// CMmBroadMessHandler:: SmsCbRoutingInd
// Incoming CB message. When the SMS Server receives a CB message
// from the network and routing of this CB message has been
// accepted, the server shall send message to the client
// -----------------------------------------------------------------------------
//
void CMmBroadMessHandler::SmsCbRoutingIndL
    (
    const TIsiReceiveC& aSmsCbRoutingInd // Received ISI message
    )
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::SmsCbRoutingInd");
OstTrace0( TRACE_NORMAL, CMMBROADMESSHANDLER_SMSGSMCBROUTINGNTFL, "CMmBroadMessHandler::SmsCbRoutingIndL" );

    CMmDataPackage data;
    TInt error ( KErrNone );

    // Get the number of subblocks
    TUint sbNumber( aSmsCbRoutingInd.Get8bit( ISI_HEADER_SIZE
        + SMS_CB_ROUTING_IND_OFFSET_SUBBLOCKCOUNT ) );

    // Check the info length of the 1st SMS_SB_CB_MESSAGE subblock to
    // know if it is a WCDMA CBS message or a GSM one
    TUint firstSbOffset( 0 );
    TInt sbFound( aSmsCbRoutingInd.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SMS_CB_ROUTING_IND,
        SMS_SB_CB_MESSAGE,
        EIsiSubBlockTypeId16Len16,
        firstSbOffset ) );

    TUint8 infoLength( SMS_CB_MESSAGE_CONTENT_SIZE + 1 ); // Illegal value
    if ( KErrNone == sbFound )
        {
        infoLength = aSmsCbRoutingInd.Get8bit( firstSbOffset
            + SMS_SB_CB_MESSAGE_OFFSET_INFOLENGTH );
        }

TFLOGSTRING2("TSY: CMmBroadMessHandler::SmsCbRoutingIndL. infoLength:%d",infoLength);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMBROADMESSHANDLER_SMSGSMCBROUTINGNTFL, "CMmBroadMessHandler::SmsCbRoutingIndL;infoLength=%hhu", infoLength );

    // Reset and destroy the array
    iCbsMsg->ResetAndDestroy();

    // GSM mode, don't care of info length
    if ( KInfoLengthIgnored == infoLength )
        {
        // GSM CB Message. First SMS_SB_CB_MESSAGE sub block is the only one.
        TGsmCbsMsg cbsMsg;

        // Get serial number
        TUint16 serialNumber( aSmsCbRoutingInd.Get16bit( firstSbOffset
            + SMS_SB_CB_MESSAGE_OFFSET_SERIALNUMBER ) );
        // Append MSB bits from serial number
        cbsMsg.iCbsMsg.Append( serialNumber >> 8 );
        // Append LSB bits from serial number
        cbsMsg.iCbsMsg.Append( serialNumber & 0x00FF );

        // Get message ID
        TUint16 messageId( aSmsCbRoutingInd.Get16bit( firstSbOffset
            + SMS_SB_CB_MESSAGE_OFFSET_CBMESSAGEID ) );
        // Append MSB bits from message ID
        cbsMsg.iCbsMsg.Append( messageId >> 8 );
        // Append LSB bits from message ID
        cbsMsg.iCbsMsg.Append( messageId & 0x00FF );

        // Data coding scheme
        cbsMsg.iCbsMsg.Append( aSmsCbRoutingInd.Get8bit( firstSbOffset
            + SMS_SB_CB_MESSAGE_OFFSET_DATACODINGSCHEME ) );

        // Number of pages
        cbsMsg.iCbsMsg.Append( aSmsCbRoutingInd.Get8bit( firstSbOffset
            + SMS_SB_CB_MESSAGE_OFFSET_PAGE ) );

        // Content of the message
        cbsMsg.iCbsMsg.Append( aSmsCbRoutingInd.GetData( firstSbOffset
            + SMS_SB_CB_MESSAGE_OFFSET_CONTENTOFMESSAGE,
            SMS_CB_MESSAGE_CONTENT_SIZE ) );

        // Pack data
        data.PackData( &cbsMsg );

        // Complete
        iMessageRouter->Complete(
            EMmTsyGsmBroadcastNotifyMessageReceived,
            &data,
            error );
        }

    // WCDMA mode,take care of info length and number of subblocks should be
    // less or equal to 15
    else if ( ( SMS_CB_MESSAGE_CONTENT_SIZE >= infoLength )
        && ( KWcdmaCbsNumberOfSbMax >= sbNumber ) )
        {
        TUint currSbOffset( firstSbOffset );
        // Loop through the SMS_SB_CB_MESSAGE sub blocks.
        for ( TInt i( 0 ); i < sbNumber; i++)
            {
            TWcdmaCbsMsg* wcdmaCbsMsg = new ( ELeave ) TWcdmaCbsMsg;
            CleanupStack::PushL( wcdmaCbsMsg );

            // Total number of subblocks in SMS_CB_ROUTING_IND
            wcdmaCbsMsg->iSbNumber = sbNumber;

            // Serial number
            wcdmaCbsMsg->iSerialNum = aSmsCbRoutingInd.Get16bit(
                currSbOffset + SMS_SB_CB_MESSAGE_OFFSET_SERIALNUMBER );

            // Message ID
            wcdmaCbsMsg->iMessageId = aSmsCbRoutingInd.Get16bit(
                currSbOffset + SMS_SB_CB_MESSAGE_OFFSET_CBMESSAGEID );

            // Data coding scheme
            wcdmaCbsMsg->iDCS = aSmsCbRoutingInd.Get8bit(
                currSbOffset + SMS_SB_CB_MESSAGE_OFFSET_DATACODINGSCHEME );

            // Number of pages
            wcdmaCbsMsg->iNumberOfPages = static_cast<TInt>(
                aSmsCbRoutingInd.Get8bit(
                    currSbOffset + SMS_SB_CB_MESSAGE_OFFSET_PAGE ) );

            // Information length
            wcdmaCbsMsg->iInfoLength = aSmsCbRoutingInd.Get8bit(
                currSbOffset + SMS_SB_CB_MESSAGE_OFFSET_INFOLENGTH );

            // Message type is here CBS Message
            // (see spec 3GPP 25.324 v6.1 chapter 11.1.2)
            wcdmaCbsMsg->iMessageType = KCbsMessageType;

            // Content of the message
            TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize>
                wcdmaCbsDataTemp = aSmsCbRoutingInd.GetData(
                currSbOffset + SMS_SB_CB_MESSAGE_OFFSET_CONTENTOFMESSAGE,
                SMS_CB_MESSAGE_CONTENT_SIZE );

            // Append data portion from beginning to info length
            wcdmaCbsMsg->iWcdmaCbsData.Append(
                wcdmaCbsDataTemp.Mid ( 0, wcdmaCbsMsg->iInfoLength ) );

            iCbsMsg->AppendL( wcdmaCbsMsg );

            currSbOffset += SIZE_SMS_SB_CB_MESSAGE;

            // Pop wcdmaCbsMsg but don't destroy it
            CleanupStack::Pop( wcdmaCbsMsg );
            //note: Lint doesn't understand the use of PopAndDestroy and 'thinks'
            //that there is a memory leak for wcdmaCbsMsg, we disable that warning with
            //the following command
            //lint -e429
            }

        // Pack data
        data.PackData( &iCbsMsg, &sbNumber );

        // Complete
        iMessageRouter->Complete(
            EMmTsyWcdmaBroadcastNotifyMessageReceived,
            &data,
            error );
        }
    else    // Illegal value ( e.g network is not working correctly )
        {
TFLOGSTRING("TSY: CMmBroadMessHandler::SmsGsmCbRoutingNtfL illegal value (e.g network problem )");
OstTrace0( TRACE_NORMAL, DUP2_CMMBROADMESSHANDLER_SMSGSMCBROUTINGNTFL, "CMmBroadMessHandler::SmsGsmCbRoutingNtfL-illegal value (e.g network problem" );

        error = KErrGeneral;
        // Complete. We could complete with either ipc
        // EMmTsyWcdmaBroadcastNotifyMessageReceived or
        // EMmTsyGsmBroadcastNotifyMessageReceived but as it doesn't matter,
        // we arbitrarily decide to complete with a GSM CBS ipc
        iMessageRouter->Complete(
            EMmTsyGsmBroadcastNotifyMessageReceived,
            &data,
            error );
        }
    return;
    }

// -----------------------------------------------------------------------------
// CMmBroadMessHandler::ExtFuncL
// Dispatches Etel requests to DOS level handlers
// -----------------------------------------------------------------------------
//
TInt CMmBroadMessHandler::ExtFuncL
    (
    TInt aIpc, // IPC number
    const CMmDataPackage* aDataPackage // packed data
    )
    {
TFLOGSTRING("TSY: CMmBroadMessHandler::ExtFuncL");
OstTrace0( TRACE_NORMAL, CMMBROADMESSHANDLER_EXTFUNCL, "CMmBroadMessHandler::ExtFuncL" );
    TInt ret( KErrNone );

    TUint8 transId( 0 ); // Initialize to zero

    switch ( aIpc )
        {
        case EMobileBroadcastMessagingReceiveMessage:
            {
            transId = EBroadcastMessagingReceiveMessage;
            break;
            }
        case EMobileBroadcastMessagingReceiveMessageCancel:
            {
            transId = EBroadcastMessagingReceiveMessageCancel;
            break;
            }
        case EMobileBroadcastMessagingSetFilterSetting:
            {
            transId = EBroadcastMessagingSetFilterSetting;
            break;
            }
        default:
            {
            // This method should only be called for Broadcast cases
TFLOGSTRING2("TSY: CMmBroadMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMBROADMESSHANDLER_EXTFUNCL, "CMmBroadMessHandler::ExtFuncL;aIpc=%d", aIpc );
            ret = KErrArgument;
            break;
            }
        }
    if ( EBroadcastMessagingUnknown != transId )
        {
        ret = SmsCbRoutingRequest( transId, aDataPackage );
        }
    return ret;
    }


// End of the file

