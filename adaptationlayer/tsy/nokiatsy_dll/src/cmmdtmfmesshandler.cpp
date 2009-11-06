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



//  INCLUDE FILES
#include "cmmdtmfmesshandler.h"
#include "cmmcallmesshandler.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h"
#include "cmmmessagerouter.h"
#include "tsylogger.h" // logging
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdtmfmesshandlertraces.h"
#endif

#include <etelmm.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_defaults.h> // KDtmfStringMaxLength constant needed
#include <ctsy/rmmcustomapi.h> // Custom API's NotifyDtmfEvent implemented here
#include <tisi.h>
#include <call_modemisi.h>



// EXTERNAL DATA STRUCTURES
    //none

// EXTERNAL FUNCTION PROTOTYPES
    //none

// CONSTANTS
// hard-coded transaction id for ISA messages
const TUint8 KDtmfTransId = 3;
const TUint8 KDtmfPadding = 0x00;

// For shifting by one byte
const TUint8 KShiftByOneByte = 8;

// MACROS
    //none

// LOCAL CONSTANTS AND MACROS
    //none

// MODULE DATA STRUCTURES
    //none

// LOCAL FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
    //none

// ============================= LOCAL FUNCTIONS ===============================
    //none

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::CMmDtmfMessHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmDtmfMessHandler::CMmDtmfMessHandler()
    {
    //none

TFLOGSTRING("TSY: CMmDtmfMessHandler::CMmDtmfMessHandler");
OstTrace0( TRACE_NORMAL, CMMDTMFMESSHANDLER_CMMDTMFMESSHANDLER, "CMmDtmfMessHandler::CMmDtmfMessHandler" );

    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmDtmfMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMDTMFMESSHANDLER_CONSTRUCTL, "CMmDtmfMessHandler::ConstructL" );
    iDTMFStringAfterW.Zero();
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmDtmfMessHandler* CMmDtmfMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender, //pointer to the phonet sender
    CMmPhoNetReceiver* aPhoNetReceiver, // pointer to the phoner receiver
    CMmMessageRouter* aMessageRouter ) // pointer to the message router
    {
TFLOGSTRING("TSY: CMmDtmfMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMDTMFMESSHANDLER_NEWL, "CMmDtmfMessHandler::NewL" );

    CMmDtmfMessHandler* dtmfMessHandler =
        new ( ELeave ) CMmDtmfMessHandler();

    CleanupStack::PushL( dtmfMessHandler );
    dtmfMessHandler->iPhoNetSender = aPhoNetSender;
    dtmfMessHandler->iMessageRouter = aMessageRouter;
    dtmfMessHandler->ConstructL();

    aPhoNetReceiver->RegisterL( dtmfMessHandler, PN_MODEM_CALL, CALL_MODEM_DTMF_STATUS_IND );
    aPhoNetReceiver->RegisterL( dtmfMessHandler, PN_MODEM_CALL, CALL_MODEM_DTMF_SEND_RESP );
    aPhoNetReceiver->RegisterL( dtmfMessHandler, PN_MODEM_CALL, CALL_MODEM_DTMF_STOP_RESP );
    aPhoNetReceiver->RegisterL( dtmfMessHandler, PN_MODEM_CALL, CALL_MODEM_DTMF_TONE_IND );

    CleanupStack::Pop( dtmfMessHandler );

    return dtmfMessHandler;
    }



// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::~CMmDtmfMessHandler
// Destructor
// -----------------------------------------------------------------------------
//

CMmDtmfMessHandler::~CMmDtmfMessHandler()
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::~CMmDtmfMessHandler");
OstTrace0( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_CMMDTMFMESSHANDLER, "CMmDtmfMessHandler::~CMmDtmfMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::ExtFuncL
// Forwards requests coming from the Symbian OS layer to the
// specific method.
// -----------------------------------------------------------------------------
//
TInt CMmDtmfMessHandler::ExtFuncL(
    TInt aIpc, // request ipc
    const CMmDataPackage* aDataPackage ) //request data
    {

TFLOGSTRING2("TSY: CMmDtmfMessHandler::ExtFuncL. IPC:%d", aIpc);
OstTrace1( TRACE_NORMAL, CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL;aIpc=%d", aIpc );

    //*************************************************************//
    // NOTE:
    //
    // LICENSEE SPECIFIC MESSAGE HANDLER IMPLEMENTATION STARTS HERE
    //
    //*************************************************************//

    TInt ret( KErrNone );

    TUint8 transId( KDtmfTransId );

    switch ( aIpc )
        {
        case EMobilePhoneStartDTMFTone:
            {
TFLOGSTRING("TSY: CMmDtmfMessHandler::ExtFuncL -- EMobilePhoneStartDTMFTone");
OstTrace0( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL, EMobilePhoneStartDTMFTone" );
            //unpack parameter: DTMF tone to be sent
            TChar* tone( NULL );
            aDataPackage->UnPackData ( &tone );
            // CallDtmfSendReq needs a descriptor of length one
            // instead of a TChar
            TBuf<1> toneDes;
            toneDes.SetLength(0);
            toneDes.Append(*tone);
            //forward the request to the specific method
            ret = CallDtmfSendReqL ( transId, &toneDes, EDtmfTypeDigit );
            break;
            }
        case EMobilePhoneStopDTMFTone:
            {
TFLOGSTRING("TSY: CMmDtmfMessHandler::ExtFuncL -- EMobilePhoneStopDTMFTone");
OstTrace0( TRACE_NORMAL, DUP2_CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL, EMobilePhoneStopDTMFTone" );
            //no packed parameters
            //forward the request to the specific method
            CallDtmfStopReq ( transId );
            break;
            }

        case EMobilePhoneContinueDTMFStringSending:
            {
TFLOGSTRING("TSY: CMmDtmfMessHandler::ExtFuncL -- EMobilePhoneContinueDTMFStringSending");
OstTrace0( TRACE_NORMAL, DUP3_CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL, EMobilePhoneContinueDTMFStringSending" );
            }
        case EMobilePhoneSendDTMFTones:
            {
TFLOGSTRING("TSY: CMmDtmfMessHandler::ExtFuncL -- EMobilePhoneSendDTMFTones");
OstTrace0( TRACE_NORMAL, DUP4_CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL, EMobilePhoneSendDTMFTones" );
            if ( !iDtmfSendOngoing || EMobilePhoneContinueDTMFStringSending == aIpc )
                {
                //unpack parameter: DTMF tones to be sent
                TDesC* tonesDes ( NULL );
                aDataPackage->UnPackData ( &tonesDes );
                //forward the request to the specific method
                ret = CallDtmfSendReqL ( transId, tonesDes, EDtmfTypeString );
                if ( KErrNone == ret && EMobilePhoneSendDTMFTones == aIpc )
                    {
                    iDtmfSendOngoing = ETrue;
                    }
                //no else
                }
            else
                {
TFLOGSTRING("TSY: CMmDtmfMessHandler::ExtFuncL -- dtmf send already ongoing");
OstTrace0( TRACE_NORMAL, DUP5_CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL, DTMF sending already ongoing" );
                ret = KErrServerBusy;
                }
            break;
            }
        case EMobilePhoneSendDTMFTonesCancel:
            {
TFLOGSTRING("TSY: CMmDtmfMessHandler::ExtFuncL -- EMobilePhoneSendDTMFTonesCancel");
OstTrace0( TRACE_NORMAL, DUP6_CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL, EMobilePhoneSendDTMFTonesCancel" );
            iDtmfSendOngoing = EFalse;
            //no packed parameters
            //forward the request to the specific method
            ret = CallDtmfStopReq( transId );
            break;
            }
        default:
            {
            // this method should only be called for DTMF cases
TFLOGSTRING2("TSY: CMmDtmfMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP7_CMMDTMFMESSHANDLER_EXTFUNCL, "CMmDtmfMessHandler::ExtFuncL;Unknown IPC=%d", aIpc );
            ret = KErrArgument;
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::ReceiveMessageL
// Called when an ISI message has been received.
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::ReceiveMessageL(
    const TIsiReceiveC &aIsiMessage  ) //received ISI message
    {
    TInt resource (aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ));
    TInt messageId(aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ));

TFLOGSTRING3("TSY: CMmDtmfMessHandler::ReceiveMessageL. Resource:%d. MsgId:%d",resource, messageId);
OstTraceExt2( TRACE_NORMAL, CMMDTMFMESSHANDLER_RECEIVEMESSAGEL, "CMmDtmfMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    // DTMF functionality is implemented in CALL server
    if ( PN_MODEM_CALL == resource )
        {
        switch ( messageId )
            {
            case CALL_MODEM_DTMF_STATUS_IND:
                {
                CallDtmfStatusInd( aIsiMessage );
                break;
                }
            case CALL_MODEM_DTMF_SEND_RESP:
                {
                CallDtmfSendResp( aIsiMessage );
                break;
                }
            case CALL_MODEM_DTMF_STOP_RESP:
                {
                CallDtmfStopResp( aIsiMessage );
                break;
                }
            case CALL_MODEM_DTMF_TONE_IND:
                {
                CallDtmfToneInd( aIsiMessage );
                break;
                }
            default:
                {

TFLOGSTRING("TSY: CMmDtmfMessHandler::ReceiveMessageL. PN_MODEM_CALL: Switch messageID case default");
OstTrace0( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_RECEIVEMESSAGEL, "CMmDtmfMessHandler::ReceiveMessageL,PN_MODEM_CALL: Switch messageID case default" );

                break;
                }
            } // switch( messageId )
        } // if ( PN_MODEM_CALL == resource )
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::SendPostAddressL
// Called by call message handler when a call goes active and there
// is a post address string to send.
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::SendPostAddressL(
    const TDesC* aDtmfString ) //dtmf character(s) to be sent
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::SendPostAddressL");
OstTrace0( TRACE_NORMAL, CMMDTMFMESSHANDLER_SENDPOSTADDRESSL, "CMmDtmfMessHandler::SendPostAddressL" );

    if ( !iDtmfSendOngoing )
        {
        iDtmfSendOngoing = ETrue;
        CallDtmfSendReqL( KDtmfTransId, aDtmfString, EDtmfTypeString );
        }
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::CallDtmfSendReq
// Called by MmDtmfTsy to construct a CALL_MODEM_DTMF_SEND_REQ ISI
// message and to send it to Phonet. Can be used with strings and single
// digits.
// -----------------------------------------------------------------------------
//
TInt CMmDtmfMessHandler::CallDtmfSendReqL(
    TUint8 aTransactionId,  //transaction id
    const TDesC* aDtmfString, //dtmf character(s) to be sent
    TNOSDtmfType aDtmfType ) // Type of dtmf to send
    {
TFLOGSTRING3("TSY: CMmDtmfMessHandler::CallDtmfSendReqL. TransId:%d, DtmfString:%S", aTransactionId, aDtmfString);
OstTraceExt2( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_CALLDTMFSENDREQL, "CMmDtmfMessHandler::CallDtmfSendReqL;aTransactionId=%hhu;aDtmfString=%s", aTransactionId, ( TUint )( aDtmfString ) );

    TInt ret( KErrNone );

    // Get dtmf character(s) length
    TInt length( aDtmfString->Length() );

    if ( ( KDtmfStringMaxLength >= length ) && ( 0 < length ) )
        {
        // Create isi message
        TIsiSend callDtmfSendReq( iPhoNetSender->SendBufferDes() );
        callDtmfSendReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_CALL );

        // Set the call Dtmf Send Req data
        callDtmfSendReq.Set8bit(
            ISI_HEADER_OFFSET_TRANSID, aTransactionId );
        callDtmfSendReq.Set8bit(
            ISI_HEADER_OFFSET_MESSAGEID, CALL_MODEM_DTMF_SEND_REQ );
        callDtmfSendReq.Set8bit(
            ISI_HEADER_SIZE + CALL_MODEM_DTMF_SEND_REQ_OFFSET_CALLID,
            CALL_MODEM_ID_ACTIVE );

        // Initialize message offset and subblock count
        TUint currentMsgOffset(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_DTMF_SEND_REQ );
        TUint8 numOfSbInMessage( 0 );

        iDtmfType = aDtmfType;

        //get call dtmf send req sub blocks
        TBool createReq( GetCallDtmfSendReqSubBlockL(
            aDtmfString,
            callDtmfSendReq,
            currentMsgOffset,
            numOfSbInMessage ) );

        callDtmfSendReq.Set8bit(
            ISI_HEADER_SIZE + CALL_MODEM_DTMF_SEND_REQ_OFFSET_SUBBLOCKS,
            numOfSbInMessage );

        //createReq flag is set to TRUE so request will be sent
        if ( createReq )
            {
            //send message via phonet
            ret = iPhoNetSender->Send( callDtmfSendReq.Complete() );
            }
        }
    else
        {
        ret = KErrArgument;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::CallDtmfSendResp
// Breaks a CALL_MODEM_DTMF_SEND_RESP ISI-message. Handles error
// situations of DTMF sending.
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::CallDtmfSendResp(
    const TIsiReceiveC &aIsiMessage ) //received ISI message
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::CallDtmfSendResp");
OstTrace0( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_CALLDTMFSENDRESP, "CMmDtmfMessHandler::CallDtmfSendResp" );

    TUint sbStartOffSet( 0 );

    if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_DTMF_SEND_RESP, CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
        {
        //get cause type
        TUint8 causeType( aIsiMessage.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE) );

        //get cause value
        TUint8 causeValue( aIsiMessage.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_CAUSE_OFFSET_CAUSE ) );

        if ( ( EDtmfTypeString == iDtmfType ) && ( !iCharWFound ) )
            {
            iDtmfSendOngoing = EFalse;
            TInt result( CMmStaticUtility::CSCauseToEpocError(
                PN_MODEM_CALL, causeType, causeValue ) );
            iMessageRouter->Complete(
                EMobilePhoneSendDTMFTones,
                result );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::CallDtmfStopReq
// Called by CMmDtmfTsy to construct a CALL_MODEM_DTMF_STOP_REQ ISI
// message
// -----------------------------------------------------------------------------
//
TInt CMmDtmfMessHandler::CallDtmfStopReq(
    TUint8 aTransactionId ) //transaction id
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::CallDtmfStopReq");
OstTrace0( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_CALLDTMFSTOPREQ, "CMmDtmfMessHandler::CallDtmfStopReq" );

    //create dtmf stop request message
    //Append data for ISI message
    TBuf8<2> data;
    data.Append( CALL_MODEM_ID_ACTIVE ); // CallId
    data.Append( 0x00 ); // Number of Subblocks

    // Send Isi message via Phonet
    return iPhoNetSender->Send( PN_MODEM_CALL, aTransactionId, CALL_MODEM_DTMF_STOP_REQ, data );
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::CallDtmfStopResp
// Breaks a CALL_MODEM_DTMF_STOP_RESP ISI message
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::CallDtmfStopResp(
    const TIsiReceiveC &aIsiMessage ) //received ISI message
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::CallDtmfStopResp");
OstTrace0( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_CALLDTMFSTOPRESP, "CMmDtmfMessHandler::CallDtmfStopResp" );

    TUint8 nbrOfsubBlocks( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_DTMF_STOP_RESP_OFFSET_SUBBLOCKS ) );

    if ( 0 < nbrOfsubBlocks )
        {
        TUint sbStartOffSet( 0 );

        //check that is this response to Cancel request
        if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_DTMF_STOP_RESP, CALL_MODEM_SB_DTMF_STRING,
            EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
            {
            iMessageRouter->Complete(
                EMobilePhoneStopDTMFTone,
                KErrNone );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::CallDtmfStatusInd
// Called by PhonetReceiver to break a CALL_MODEM_DTMF_STATUS_IND
// ISI message.
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::CallDtmfStatusInd(
    const TIsiReceiveC &aIsiMessage ) // Received ISI message
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::CallDtmfStatusInd");
OstTrace0( TRACE_NORMAL, DUP2_CMMDTMFMESSHANDLER_CALLDTMFSTATUSIND, "CMmDtmfMessHandler::CallDtmfStatusInd" );

    TUint sbStartOffSet( 0 );

    //get call dtmf status sub block
    if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + CALL_MODEM_DTMF_STATUS_IND_OFFSET_STATUS, CALL_MODEM_SB_DTMF_STATUS,
        EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
        {
        // DTMF event information
        RMobilePhone::TMobilePhoneDTMFEvent event;
        CMmDataPackage eventDataPackage;
        //get dtmf status
        TUint8 dtmfStatus( aIsiMessage.Get8bit( sbStartOffSet +
            CALL_MODEM_SB_DTMF_STATUS_OFFSET_STATUS) );

TFLOGSTRING2("TSY: CMmDtmfMessHandler::CallDtmfStatusInd. dtmfStatus:%d",dtmfStatus);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMDTMFMESSHANDLER_CALLDTMFSTATUSIND, "CMmDtmfMessHandler::CallDtmfStatusInd;dtmfStatus=%hhu", dtmfStatus );

        if( CALL_MODEM_DTMF_IDLE == dtmfStatus )
            {
            if( EDtmfTypeUnknown != iDtmfType )
                {
                TUint8 causeType( 0 );
                TUint8 causeValue( 0 );

                if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_CALL_MODEM_DTMF_STATUS_IND, CALL_MODEM_SB_CAUSE,
                    EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
                    {
                    //Get Cause Type
                    causeType = aIsiMessage.Get8bit( sbStartOffSet +
                        CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE);
                    //Get Cause Value
                    causeValue = aIsiMessage.Get8bit( sbStartOffSet +
                        CALL_MODEM_SB_CAUSE_OFFSET_CAUSE );
                    }

                //If sub block found, DTMF string sending has been pause with 'w'
                //mark in the DTMF string
                if ( iCharWFound )
                    {
                    CMmDataPackage dataPackage;
                    dataPackage.PackData( &iDTMFStringAfterW );
                    iMessageRouter->Complete(
                        EMobilePhoneNotifyStopInDTMFString,
                        &dataPackage,
                        KErrNone );

                    // Set iCharWFound flag to False
                    iCharWFound = EFalse;
                    iDTMFStringAfterW.Zero();
                    }
                //Complete sending DTMF string
                else if ( EDtmfTypeString == iDtmfType )
                    {
                    iDtmfSendOngoing = EFalse;
                    TInt result( CMmStaticUtility::CSCauseToEpocError(
                        PN_MODEM_CALL, causeType, causeValue ) );

                    CMmDataPackage dataPackage;
                    iMessageRouter->Complete(
                        EMobilePhoneSendDTMFTones,
                        &dataPackage,
                        result );
                    }
                }
            iDtmfType       = EDtmfTypeUnknown;
            iDtmfDigitSent  = 0;
            event = RMobilePhone::EStopDtmfTone;
            }
        else
            {
            event = RMobilePhone::EStartDtmfTone;
            }
        // Complete DTMF Event
        eventDataPackage.PackData( &event );
        iMessageRouter->Complete(
            EMobilePhoneNotifyDTMFEvent,
            &eventDataPackage,
            KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::GetCallDtmfSendReqSubBlock
// Appends the sub blocks for CALL_MODEM_DTMF_SEND_REQ ISI message.
//    if iDtmfType is DTMF_TYPE_DIGIT
//        append digit to temporary buffer
//        append callDtmfSubBlock subblock to the message using the temporary
//        buffer
//        set iDtmfDigitSent to point to the aDtmfString (input parameter)
//    else
//        append dtmf timer sub block (minimum duration and gap)
//        append DTMF string to temporary buffer
//        append callDtmfSubBlock subblock to the message using the temporary
//        buffer
// -----------------------------------------------------------------------------
//
TBool CMmDtmfMessHandler::GetCallDtmfSendReqSubBlockL(
    const TDesC* aDtmfString,   //dtmf character(s)
    TIsiSend &aIsiMessage, //IsiMsg to be build
    TUint &aCurrentMsgOffset,
    TUint8 &aNumOfSbInMessage )
    {
TFLOGSTRING2("TSY: CMmDtmfMessHandler::GetCallDtmfSendReqSubBlockL. DtmfType:%d", iDtmfType );
OstTrace1( TRACE_NORMAL, CMMDTMFMESSHANDLER_GETCALLDTMFSENDREQSUBBLOCKL, "CMmDtmfMessHandler::GetCallDtmfSendReqSubBlockL;iDtmfType=%d", iDtmfType );
    // createReq flag is set to 'false' only in case the first character is 'w'
    // in DTMF string. If 'true' CALL_DTMF_SEND_REQ will be sent.
    TBool createReq( ETrue );

    if( EDtmfTypeDigit == iDtmfType )
        {
        TBuf8< SIZE_CALL_MODEM_SB_DTMF_DIGIT > callDtmfDigitBuf( 0 );
        TIsiSubBlock dtmfDigitSubBlock( callDtmfDigitBuf,
            CALL_MODEM_SB_DTMF_DIGIT, EIsiSubBlockTypeId8Len8 );

        TIsiUtility::CopyToBigEndian( *aDtmfString, callDtmfDigitBuf );

        aIsiMessage.CopyData(
            aCurrentMsgOffset, dtmfDigitSubBlock.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + SIZE_CALL_MODEM_SB_DTMF_DIGIT;
        aNumOfSbInMessage++;

        iDtmfDigitSent = *aDtmfString->Ptr();
        }
    else
        {
        // This flag indicates is 'w' found in DTMF string.
        iCharWFound = EFalse;

        // Buffer length for pause length, string length(unicode)
        // and 16-bit data.
        TBuf8<KAddressSubBlockMaxLength> dataString( 0 );
        TIsiSubBlock callDtmfStringSubBlock( dataString,
            CALL_MODEM_SB_DTMF_STRING, EIsiSubBlockTypeId8Len8 );

        // Pause length
        dataString.Append( CALL_MODEM_DTMF_PAUSE_3S );

        // Get dtmf character(s) length
        TInt length( aDtmfString->Length() );

        // Check if character 'w' is  found in the DTMF string
        TInt positionCharW( aDtmfString->Locate('w') );

        // Character 'w' was found, the string after 'w' must must be stored,
        // it will be sent back to client when CALL_DTMF_STATUS_IND with
        // CALL_DTMF_IDLE is received.
        if ( KErrNotFound != positionCharW )
            {
            // Copy string after 'w' for later use.
            iDTMFStringAfterW.Copy(
                aDtmfString->Right( length - ( positionCharW + 1 ) ) );

            // If 'w' was received as the first character, complete
            // immediately with remaining string.
            if ( 0 == positionCharW )
                {
                // Notify about the stop mark in the DTMF string.
                CMmDataPackage dataPackage;
                dataPackage.PackData( &iDTMFStringAfterW );
                iMessageRouter->Complete(
                    EMobilePhoneNotifyStopInDTMFString,
                    &dataPackage,
                    KErrNone );
                iDTMFStringAfterW.Zero();
                // Don't send the request in this case.
                createReq = EFalse;
                }
            else
                {
                // Length of the datastring including character 'w'.
                length = positionCharW + 1;
                // 'w' was found, set the flag.
                iCharWFound = ETrue;
                }
            }
        // No else. If 'w' was not found, iCreateReq is set to 'true' by
        // default and iCharWFound to 'false'. Whole string will be sent.

        // First append number length
        dataString.Append( length ); // DTMF length

        // Create buffer for dtmf data. Max data buffer size is 256.
        TBuf8<KMaxDataBufferSize> tempdata;
        // Copy 16 to 8
        TIsiUtility::CopyToBigEndian( *aDtmfString, tempdata );
        // Append data to the buffer which contains unicode string
        dataString.Append( tempdata.Left( length * 2 ) );

        aIsiMessage.CopyData( aCurrentMsgOffset,
            callDtmfStringSubBlock.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + dataString.Length();
        aNumOfSbInMessage++;

        //Sub Block CALL_MODEM_SB_DTMF_TIMERS
        TBuf8< SIZE_CALL_MODEM_SB_DTMF_TIMERS > dataTimers( 0 ); //data timers is a unicode character
        TIsiSubBlock callDtmfTimersSubBlock( dataTimers,
            CALL_MODEM_SB_DTMF_TIMERS, EIsiSubBlockTypeId8Len8 );

        //DTFM duration in milliseconds
        dataTimers.Append( static_cast<TUint8>( CALL_MODEM_DTMF_NO_DELAY >> KShiftByOneByte ) );
        dataTimers.Append( static_cast<TUint8>( CALL_MODEM_DTMF_NO_DELAY ) );
        //DTMF gap in milliseconds
        dataTimers.Append( static_cast<TUint8>( CALL_MODEM_DTMF_NO_DELAY >> KShiftByOneByte ) );
        dataTimers.Append( static_cast<TUint8>( CALL_MODEM_DTMF_NO_DELAY ) );
        //append padding
        dataTimers.AppendFill( KDtmfPadding, 2 );

        aIsiMessage.CopyData( aCurrentMsgOffset,
            callDtmfTimersSubBlock.CompleteSubBlock() );

        // Set new offset and increase subblock count
        aCurrentMsgOffset =
            aCurrentMsgOffset + SIZE_CALL_MODEM_SB_DTMF_TIMERS;
        aNumOfSbInMessage++;
        }

    return createReq;
    }

// NOTE:
// The following methods are part of Custom Tsy
//

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::CallDtmfToneInd
// Breaks a CALL_MODEM_DTMF_TONE_IND ISI message and completes NotifyDtmfEvent
// with gathered data.
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::CallDtmfToneInd(
    const TIsiReceiveC &aIsiMessage ) //received ISI message
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::CallDtmfToneInd");
OstTrace0( TRACE_NORMAL, CMMDTMFMESSHANDLER_CALLDTMFTONEIND, "CMmDtmfMessHandler::CallDtmfToneInd" );

    //create and initialize TDtmfInfo
    RMmCustomAPI::TDtmfInfo dtmfInfo;
    dtmfInfo.iType = RMmCustomAPI::EDtmfSequence;
    dtmfInfo.iEvent = RMmCustomAPI::EDtmfAbort;
    dtmfInfo.iTone = CALL_MODEM_DTMF_NO_TONE;

    TUint sbStartOffSet( 0 );

    // get dtmf tone sub block
    TInt returnValue( aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + CALL_MODEM_DTMF_TONE_IND_OFFSET_TONE, CALL_MODEM_SB_DTMF_TONE,
        EIsiSubBlockTypeId8Len8, sbStartOffSet ) );

    // if subblock was found
    if ( KErrNone == returnValue )
        {
        // get DTMF tone
        TUint16 dtmfTone ( aIsiMessage.Get16bit( sbStartOffSet +
            CALL_MODEM_SB_DTMF_TONE_OFFSET_TONE ) );

        //map the values into a TDtmfInfo
        //set the event type
        switch ( iDtmfType )
            {
            case EDtmfTypeString:
                {
                //dtmfInfo.iType already initialized as EDtmfSequence
                break;
                }
            case EDtmfTypeDigit:
                {
                dtmfInfo.iType = RMmCustomAPI::EDtmfManual;
                break;
                }
            default:
                {
                returnValue = KErrGeneral;
                break;
                }
            }
        //set the event
         if ( CALL_MODEM_DTMF_NO_TONE == dtmfTone )
            {
            dtmfInfo.iEvent = RMmCustomAPI::EDtmfStop;
            //valus of dtmfInfo.iTone has been already set as KCallDtmfNoTone;
TFLOGSTRING("TSY: CMmDtmfMessHandler::CallDtmfToneInd-> No Tone");
OstTrace0( TRACE_NORMAL, DUP1_CMMDTMFMESSHANDLER_CALLDTMFTONEIND, "CMmDtmfMessHandler::CallDtmfToneInd, No Tone" );
            }
         else
            {
            dtmfInfo.iEvent = RMmCustomAPI::EDtmfStart;
            dtmfInfo.iTone = dtmfTone;
//In this logging the MSB are not wanted to be shown. This clarifies testing.
TFLOGSTRING2("TSY: CMmDtmfMessHandler::CallDtmfToneInd. Tone: %x", 0x0F&dtmfTone);
// following lines flagged out just get rid of
// compiler warning when trace compiler is not
// in use.
#ifdef OST_TRACE_COMPILER_IN_USE

TUint16 tempDtmfTone( 0x0F&dtmfTone ); // Parameter just for tracing
OstTraceExt1( TRACE_NORMAL, DUP2_CMMDTMFMESSHANDLER_CALLDTMFTONEIND, "CMmDtmfMessHandler::CallDtmfToneInd;dtmfTone=%hx",tempDtmfTone );
#endif // OST_TRACE_COMPILER_IN_USE
            }
        }
    // complete with return value
    // parameter for SOS layer: a RMmCustomAPI::TDtmfInfo
    CMmDataPackage dataPackage;
    dataPackage.PackData( &dtmfInfo );
    iMessageRouter->Complete( ECustomNotifyDtmfEventIPC, &dataPackage, returnValue );
    }

// -----------------------------------------------------------------------------
// CMmDtmfMessHandler::HandleError
// Handles CMmDtmfMessHandler's errors that comes via PhoNetReceiver RunError
// method.
// -----------------------------------------------------------------------------
//
void CMmDtmfMessHandler::HandleError(
    const TIsiReceiveC& /*aIsiMessage*/, // Isi message
    TInt /*aError*/ ) // Error code
    {

TFLOGSTRING("TSY: CMmDtmfMessHandler::HandleError");
OstTrace0( TRACE_NORMAL, CMMDTMFMESSHANDLER_HANDLEERROR, "CMmDtmfMessHandler::HandleError" );

    }

//  End of File

