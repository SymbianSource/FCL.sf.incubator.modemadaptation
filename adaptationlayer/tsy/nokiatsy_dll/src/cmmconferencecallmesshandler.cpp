/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmphonetsender.h"
#include "cmmconferencecallmesshandler.h"
#include "cmmstaticutility.h"
#include "cmmcallmesshandler.h"
#include "cmmmessagerouter.h"
#include "tsylogger.h"
#include "OstTraceDefinitions.h"

#include <etelmm.h>
#include <tisi.h>
#include <call_modemisi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmconferencecallmesshandlerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
const TUint8 KConfCallPadding = 0x00; // Filler byte for ISI messages

// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// ======================= LOCAL FUNCTIONS =====================================
    // None

// ==================== MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::CMmConferenceCallMessHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmConferenceCallMessHandler::CMmConferenceCallMessHandler()
    {
    // None
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::CMmConferenceCallMessHandler");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_CMMCONFERENCECALLMESSHANDLER, "CMmConferenceCallMessHandler::CMmConferenceCallMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::NewL
// Creates a new CMmConferenceCallMessHandler object instance.
// -----------------------------------------------------------------------------
//
CMmConferenceCallMessHandler* CMmConferenceCallMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender, //pointer to the phonet sender
    CMmPhoNetReceiver* aPhoNetReceiver, //pointer to the phonet receiver
    CMmMessageRouter* aMessageRouter// pointer to the message router
    )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_NEWL, "CMmConferenceCallMessHandler::NewL" );

    CMmConferenceCallMessHandler* conferenceCallMessHandler =
        new ( ELeave ) CMmConferenceCallMessHandler();

    CleanupStack::PushL( conferenceCallMessHandler );
    conferenceCallMessHandler->iPhoNetSender = aPhoNetSender;
    conferenceCallMessHandler->iMessageRouter = aMessageRouter;
    conferenceCallMessHandler->ConstructL();

    aPhoNetReceiver->RegisterL(
        conferenceCallMessHandler,
        PN_MODEM_CALL,
        CALL_MODEM_CONTROL_IND );
    aPhoNetReceiver->RegisterL(
        conferenceCallMessHandler,
        PN_MODEM_CALL,
        CALL_MODEM_CONTROL_RESP );
    aPhoNetReceiver->RegisterL(
        conferenceCallMessHandler,
        PN_MODEM_CALL,
        CALL_MODEM_RELEASE_RESP );

    CleanupStack::Pop( conferenceCallMessHandler );

    return conferenceCallMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmConferenceCallMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_CONSTRUCTL, "CMmConferenceCallMessHandler::ConstructL" );

    iIsCreateConference = EFalse;
    iLastOperation = 0;
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::~CMmConferenceCallMessHandler
// C++ destructor
// -----------------------------------------------------------------------------
//
CMmConferenceCallMessHandler::~CMmConferenceCallMessHandler()
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::~CMmConferenceCallMessHandler");
OstTrace0( TRACE_NORMAL, DUP1_CMMCONFERENCECALLMESSHANDLER_CMMCONFERENCECALLMESSHANDLER, "CMmConferenceCallMessHandler::~CMmConferenceCallMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::ExtFuncL
// Forwards requests coming from the Symbian OS layer to the
// specific method.
// -----------------------------------------------------------------------------
//
TInt CMmConferenceCallMessHandler::ExtFuncL(
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING2("TSY: CMmConferenceCallMessHandler::ExtFuncL. Ipc:%d", aIpc);
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_EXTFUNCL, "CMmConferenceCallMessHandler::ExtFuncL" );

    TInt ret( KErrNone );
    TUint8 transId( KConferenceCallTransId );

    switch( aIpc )
        {
        case EMobileConferenceCallAddCall:
            {
            // A CCallDataPackage is expected here
            const CCallDataPackage* callDataPackage =
                static_cast<const CCallDataPackage*>( aDataPackage );

            TUint8 isaCallId( CALL_MODEM_ID_NONE );
            TInt eTelCallId( KSymbianCallIdNone );
            RMobilePhone::TMobileService service(
                RMobilePhone::EServiceUnspecified );
            // Unpack ETel call ID parameter
            callDataPackage->GetCallIdAndMode( eTelCallId, service );
            // Map to ISA call ID
            isaCallId = MapETelCallIdToISACallId( eTelCallId );
            if ( CALL_MODEM_ID_NONE == isaCallId )
                {
                ret = KErrArgument;
                }
            else
                {
                ret = CallControlReq(
                    transId,
                    isaCallId,
                    CALL_MODEM_OP_CONFERENCE_BUILD,
                    EFalse );
                }
            break;
            }
        case EMobileConferenceCallCreateConference:
            {
            // No data to unpack
            ret = CallControlReq(
                transId,
                CALL_MODEM_ID_ACTIVE,
                CALL_MODEM_OP_CONFERENCE_BUILD,
                ETrue );
            break;
            }
        case EMobileConferenceCallSwap:
            {
            // No data to unpack
            ret = CallControlReq(
                transId,
                CALL_MODEM_ID_ACTIVE,
                CALL_MODEM_OP_SWAP,
                EFalse );
            break;
            }
        case EMobileConferenceCallHangUp:
            {
            // No data to unpack
            ret = CallReleaseReq(
                transId,
                CALL_MODEM_ID_CONFERENCE,
                CALL_MODEM_CAUSE_RELEASE_BY_USER );
            break;
            }
        // No DOS call is required for the following Conference Call IPCs:
        // Notifications
        case EMobileConferenceCallNotifyCapsChange:
        case EMobileConferenceCallNotifyConferenceStatusChange:
        case EMobileConferenceCallNotifyConferenceEvent:
        // Cancellations
        case EMobileConferenceCallNotifyCapsChangeCancel:
        case EMobileConferenceCallCreateConferenceCancel:
        case EMobileConferenceCallAddCallCancel:
        case EMobileConferenceCallSwapCancel:
        case EMobileConferenceCallHangUpCancel:
        case EMobileConferenceCallNotifyConferenceStatusChangeCancel:
        case EMobileConferenceCallNotifyConferenceEventCancel:
        // Query methods (do not change any state)
        case EMobileConferenceCallGetCaps:
        case EMobileConferenceCallEnumerateCalls:
        case EMobileConferenceCallGetMobileCallInfo:
        case EMobileConferenceCallGetConferenceStatus:
        default:
            {
TFLOGSTRING2("TSY: CMmConferenceCallMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMCONFERENCECALLMESSHANDLER_EXTFUNCL, "CMmConferenceCallMessHandler::ExtFuncL - Unknown IPC=%d", aIpc );
            ret = KErrNotSupported;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::MapETelCallIdToISACallId
// Utility function that maps an ETel call id to a call id
// in ISA Call Server's format.
// -----------------------------------------------------------------------------
//
TUint8 CMmConferenceCallMessHandler::MapETelCallIdToISACallId(
    const TInt aETelCallId )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::MapETelCallIdToISACallId");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_MAPETELCALLIDTOISACALLID, "CMmConferenceCallMessHandler::MapETelCallIdToISACallId" );

    TUint8 callId( CALL_MODEM_ID_NONE );

    if ( CALL_MODEM_ID_1 > aETelCallId || CALL_MODEM_ID_7 < aETelCallId )
        {
        callId = CALL_MODEM_ID_NONE;
        }
    else
        {
        callId = static_cast<TUint8>( aETelCallId );
        }

    return callId;
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::ReceiveMessageL
// Called when an ISI message has been received.
// -----------------------------------------------------------------------------
//
void CMmConferenceCallMessHandler::ReceiveMessageL(
    const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::ReceiveMessageL.");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_RECEIVEMESSAGEL, "CMmConferenceCallMessHandler::ReceiveMessageL" );

    TInt resource( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    switch( resource )
        {
        case PN_MODEM_CALL:
            {
            switch( messageId )
                {
                case CALL_MODEM_CONTROL_IND:
                    {
                    CallControlInd( aIsiMsg );
                    break;
                    }
                case CALL_MODEM_CONTROL_RESP:
                    {
                    CallControlResp( aIsiMsg );
                    break;
                    }
                case CALL_MODEM_RELEASE_RESP:
                    {
                    CallReleaseResp( aIsiMsg );
                    break;
                    }
                default:
                    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::ReceiveMessageL. PN_CALL Switch messageID case default.");
OstTrace0( TRACE_NORMAL, DUP1_CMMCONFERENCECALLMESSHANDLER_RECEIVEMESSAGEL, "CMmConferenceCallMessHandler::ReceiveMessageL. PN_CALL Switch messageID case default" );
                    break;
                    }
                }
            break; // Case PN_CALL
            }
        default:
            {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::ReceiveMessageL. Switch resource case default.");
OstTrace0( TRACE_NORMAL, DUP2_CMMCONFERENCECALLMESSHANDLER_RECEIVEMESSAGEL, "CMmConferenceCallMessHandler::ReceiveMessageL. Switch resource case default" );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::CallControlReq
// Sends Conference call related control requests to the Phonet
// -----------------------------------------------------------------------------
//
TInt CMmConferenceCallMessHandler::CallControlReq(
    TUint8 aTransactionId,
    TUint8 aCallId,
    TUint8 aOperation,
    TBool aIsCreateConference )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::CallControlReq");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_CALLCONTROLREQ, "CMmConferenceCallMessHandler::CallControlReq" );

    // Data for ISI message
    TBuf8<2 + SIZE_CALL_MODEM_SB_OPERATION> messageBuf;

    messageBuf.Append( aCallId );

    messageBuf.Append( 1 ); // Number of Subblocks

    // Set value if this is conference call creation request or not.
    iIsCreateConference = aIsCreateConference;

    // Save control operation
    iLastOperation = aOperation;

    // Call control request sub blocks
    messageBuf.Append( CALL_MODEM_SB_OPERATION );
    messageBuf.Append( SIZE_CALL_MODEM_SB_OPERATION );
    messageBuf.Append( aOperation );
    // Call operation sub block, no operation info used
    messageBuf.Append( KConfCallPadding ); // Padding

    // Call conrol req
    // Send message via phonet
    return iPhoNetSender->Send(
        PN_MODEM_CALL,
        aTransactionId,
        CALL_MODEM_CONTROL_REQ,
        messageBuf );
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::CallControlResp
// Breaks a CALL_MODEM_CONTROL_RESP ISI-message. Completes failed
// CreateConference, AddCall or Swap API request if there is a cause sub
// block in this response
// -----------------------------------------------------------------------------
//
void CMmConferenceCallMessHandler::CallControlResp(
    const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::CallControlResp");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_CALLCONTROLRESP, "CMmConferenceCallMessHandler::CallControlResp" );

    TUint sbStartOffSet( 0 );

    // Try to find CALL_MODEM_SB_CAUSE subblock. If found something has gone wrong.
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_CONTROL_RESP,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // CALL_CAUSE sub block found, something has gone wrong.
        // Get cause type
        TUint8 causeType( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );
        // Get cause value
        TUint8 causeValue( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        // Get Symbian OS error value
        TInt result( CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_CALL,
            causeType,
            causeValue ) );

        // If last operation was build, CreateConference or AddCall was asked
        if ( CALL_MODEM_OP_CONFERENCE_BUILD == iLastOperation )
            {
            // Has CreateConference been requested last.
            if ( iIsCreateConference )
                {
                // Complete with error
                iMessageRouter->Complete(
                    EMobileConferenceCallCreateConference,
                    result );
                // Reset flag
                iIsCreateConference = EFalse;
                }
            else
                {
                // Must be response to AddCall request
                iMessageRouter->Complete(
                    EMobileConferenceCallAddCall,
                    result );
                }
            iLastOperation = 0;
            }
        // If last operation was hold or retrieve
        else if ( CALL_MODEM_OP_HOLD == iLastOperation ||
            CALL_MODEM_OP_RETRIEVE == iLastOperation )
            {
            // Complete swap with error
            iMessageRouter->Complete(
                EMobileConferenceCallSwap,
                result );
            }
        // No else
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::CallControlInd
// Breaks a CALL_CONTROL_IND ISI-message. Completes failed
// CreateConference, AddCall or Swap API request if there is a cause sub
// block in this response. In case that this message contains Split operation
// sub block, inform ConferenceCallTsy that GoOneToOne handling has started
// -----------------------------------------------------------------------------
//
void CMmConferenceCallMessHandler::CallControlInd(
    const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::CallControlInd");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_CALLCONTROLIND, "CMmConferenceCallMessHandler::CallControlInd" );

    TUint sbStartOffSet( 0 );

    // Try to find CALL_MODEM_SB_CAUSE subblock. If found something has gone wrong
    if( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_CONTROL_IND,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // CALL_MODEM_SB_CAUSE sub block found, something has gone wrong.
        // Get cause type
        TUint8 causeType( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );
        // Get cause value
        TUint8 causeValue( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        // Get Symbian OS error value
        TInt result( CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_CALL,
            causeType,
            causeValue ) );

        // If last operation was build, CreateConference or AddCall was asked
        if ( CALL_MODEM_OP_CONFERENCE_BUILD == iLastOperation )
            {
            // Has CreateConference been requested last.
            if ( iIsCreateConference )
                {
                // Complete with error
                iMessageRouter->Complete(
                    EMobileConferenceCallCreateConference,
                    result);
                // Reset flag
                iIsCreateConference = EFalse;
                }
            else
                {
                // Must be response to AddCall request
                iMessageRouter->Complete(
                    EMobileConferenceCallAddCall,
                    result );
                }
            iLastOperation = 0;
            }
        // if last operation was hold or retrieve
        else if ( CALL_MODEM_OP_HOLD == iLastOperation ||
            CALL_MODEM_OP_RETRIEVE == iLastOperation )
            {
            // Complete swap with error
            iMessageRouter->Complete(
                EMobileConferenceCallSwap,
                result );
            }
        // No else
        }
    // Successful operation handling for operations
    // Initiated bypassing the ETel (ghost operetion).
    else if ( !iIsCreateConference && 0 == iLastOperation )
        {
        TUint8 callOperation( 0 );
        // Get call Id (three bits (1-3) contain unique call ID)
        TInt uniqueCallId( static_cast<TInt>( aIsiMsg.Get8bit(
            ISI_HEADER_SIZE + CALL_MODEM_CONTROL_IND_OFFSET_CALLID )
            & KMaskBits1to3 ) );

        // Get call operation sub block
        if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + CALL_MODEM_CONTROL_IND_OFFSET_OPERATION,
            CALL_MODEM_SB_OPERATION,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            TInt symbianConferenceEvent( -1 );
            // Get call operation
            callOperation = aIsiMsg.Get8bit(
                sbStartOffSet + CALL_MODEM_SB_OPERATION_OFFSET_OPERATION );

TFLOGSTRING2("TSY: CMmConferenceCallMessHandler::CallControlInd - Call operation: %d", callOperation);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMCONFERENCECALLMESSHANDLER_CALLCONTROLIND, "CMmConferenceCallMessHandler::CallControlInd;callOperation=%hhu", callOperation );
            // Purpose of this case is to prepare handling of externally
            // controlled conference call
            switch ( callOperation )
                {
                case CALL_MODEM_OP_CONFERENCE_BUILD:
                    {
                    symbianConferenceEvent =
                        RMobileConferenceCall::EConferenceBuilt;
                    break;
                    }
                case CALL_MODEM_OP_HOLD:
                case CALL_MODEM_OP_SWAP:
                    {
                    symbianConferenceEvent =
                        RMobileConferenceCall::EConferenceSwapped;
                    break;
                    }
                case CALL_MODEM_OP_CONFERENCE_SPLIT:
                    {
                    symbianConferenceEvent =
                        RMobileConferenceCall::EConferenceSplit;
                    break;
                    }
                default:
                    {
                    // Nothing to do
                    break;
                    }
                }

            // Update the information to Conference call
            if ( 0 <= symbianConferenceEvent )
                {
                // Create call package
                CCallDataPackage callData;
                // Pack the data for sending to the manager
                callData.SetCallIdAndMode(
                    uniqueCallId,
                    RMobilePhone::EVoiceService );
                // Pack conference event
                callData.PackData( &symbianConferenceEvent );
                // Pass call information to the Symbian OS layer
                // complete mobile call info change
                iMessageRouter->Complete(
                    EMobileConferenceCallNotifyConferenceEvent,
                    &callData,
                    KErrNone );
                }
            // No else
            }
        // No else
        }
    // No else

    iIsCreateConference = EFalse;
    iLastOperation = 0;
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::CallReleaseReq
// Called by conference call Tsy to construct a
// CALL_MODEM_RELEASE_REQ ISI message. Releases whole Conference call.
// -----------------------------------------------------------------------------
//
TInt CMmConferenceCallMessHandler::CallReleaseReq(
    TUint8 aTransactionId,
    TUint8 aCallId,
    TUint8 aCause )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::CallReleaseReq");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_CALLRELEASEREQ, "CMmConferenceCallMessHandler::CallReleaseReq" );

    // Create isi message
    TIsiSend callreleaseReq( iPhoNetSender->SendBufferDes() );
    callreleaseReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_CALL );

    // Set the Call Release Req data
    callreleaseReq.Set8bit(
        ISI_HEADER_OFFSET_TRANSID,
        aTransactionId );
    callreleaseReq.Set8bit(
        ISI_HEADER_OFFSET_MESSAGEID,
        CALL_MODEM_RELEASE_REQ );
    callreleaseReq.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RELEASE_REQ_OFFSET_CALLID,
        aCallId );

    // Initialize message offset
    TInt currentMsgOffset(
        ISI_HEADER_SIZE + CALL_MODEM_RELEASE_REQ_OFFSET_CAUSE );

    iReleaseCauseValueSent = aCause;

    // Number of subblocks
    TUint8 numOfSbInMessage( 0 );

    // CALL_MODEM_SB_CAUSE Sub block
    TBuf8< SIZE_CALL_MODEM_SB_CAUSE > callCauseBuf( 0 );
    TIsiSubBlock callCauseSb(
        callCauseBuf,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8 );

    callCauseBuf.Append( CALL_MODEM_CAUSE_TYPE_CLIENT );
    callCauseBuf.Append( aCause );
    callreleaseReq.CopyData(
        currentMsgOffset,
        callCauseSb.CompleteSubBlock() );

    // Increase subblock count
    numOfSbInMessage++;

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
    callStateAutoChangeBuf.Append( KConfCallPadding );

    callreleaseReq.CopyData(
        currentMsgOffset,
        callStateAutoChangeSb.CompleteSubBlock() );

    // Increase subblock count
    numOfSbInMessage++;

    // Set number of subblocs in CALL_MODEM_RELEASE_REQ message
    callreleaseReq.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RELEASE_REQ_OFFSET_SUBBLOCKS,
        numOfSbInMessage );

    // Send CALL_MODEM_RELEASE_REQ message
    return iPhoNetSender->Send( callreleaseReq.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::CallReleaseResp
// Breaks a CALL_MODEM_RELEASE_RESP ISI-message. This handles the
// situation where Conference call releasing has failed.
// -----------------------------------------------------------------------------
//
void CMmConferenceCallMessHandler::CallReleaseResp(
    const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::CallReleaseResp");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_CALLRELEASERESP, "CMmConferenceCallMessHandler::CallReleaseResp" );

    TUint sbStartOffSet( 0 );

    // Try to find CALL_MODEM_SB_CAUSE subblock. If found something has gone wrong
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RELEASE_RESP,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get cause type
        TUint8 causeType( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE ) );

        // Get cause value
        TUint8 causeValue( aIsiMsg.Get8bit(
            sbStartOffSet + CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        // Get Symbian OS error value
        TInt result( CMmStaticUtility::CSCauseToEpocError(
            PN_MODEM_CALL,
            causeType,
            causeValue ) );

        // Complete conference call hangup
        if ( iReleaseCauseValueSent != causeValue )
            {
            // Error occurred. Route the error value up.
            iMessageRouter->Complete(
                EMobileConferenceCallHangUp,
                result );
            }
        // No else
        iReleaseCauseValueSent = 0;
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmConferenceCallMessHandler::HandleError
// Handles CMmConferenceCallMessHandler's errors that comes
// via PhoNetReceiver RunError method.
// -----------------------------------------------------------------------------
//
void CMmConferenceCallMessHandler::HandleError(
    const TIsiReceiveC& /*aIsiMsg*/,
    TInt /*aError*/ )
    {
TFLOGSTRING("TSY: CMmConferenceCallMessHandler::HandleError");
OstTrace0( TRACE_NORMAL, CMMCONFERENCECALLMESSHANDLER_HANDLEERROR, "CMmConferenceCallMessHandler::HandleError" );

    // None
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
    // None

// End of File
