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

//  Include Files

#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32std.h>        // error values
#include <etelmm.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include <ss_wmisi.h>
#include <tisi.h>

#include "cmmmessagerouter.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h"
#include "cmmussdmesshandler.h"
#include "tsylogger.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmussdmesshandlerTraces.h"
#endif

//  External Data Structures
    //  none

// External Function Prototypes
    //  none

// LOCAL CONSTANTS AND MACROS

// hard-coded transaction id for ISA messages.
const TUint8 KUssdTransId = 5;
const TUint8 KFiller = 0x00;

// MODULE DATA STRUCTURES
    //  none

// Local Data Structures
    //  none

// Local Function Prototypes
    //  none

//  LOCAL FUNCTIONS
    //  none

// MEMBER FUNCTIONS

//=============================================================================

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
CMmUssdMessHandler::CMmUssdMessHandler
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmUssdMessHandler::CMmUssdMessHandler");
OstTrace0( TRACE_NORMAL, CMMUSSDMESSHANDLER_CMMUSSDMESSHANDLER, "CMmUssdMessHandler::CMmUssdMessHandler" );
    //None
    }

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::~CMmUssdMessHandler
// Destructor
// -----------------------------------------------------------------------------
//
CMmUssdMessHandler::~CMmUssdMessHandler
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmUssdMessHandler::~CMmUssdMessHandler");
OstTrace0( TRACE_NORMAL, DUP1_CMMUSSDMESSHANDLER_CMMUSSDMESSHANDLER, "CMmUssdMessHandler::~CMmUssdMessHandler" );
    }
// -----------------------------------------------------------------------------
// CMmUssdMessHandler::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CMmUssdMessHandler::ConstructL
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmUssdMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMUSSDMESSHANDLER_CONSTRUCTL, "CMmUssdMessHandler::ConstructL" );
    // The flag indicates the correct complete method in failed case for
    // SS_SERVICE_FAILED_RESP
    iIsSendReleaseCalled = EFalse;
    //No ongoing request
    iIsUssdSendReqOnGoing = EFalse;
    //No pending request
    iIsSendReleaseReqPending = EFalse;

    iNoFdnUSSDReq = EFalse;

    iLastMtUssdIsRequest = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmUssdMessHandler* CMmUssdMessHandler::NewL
    (
    CMmPhoNetSender* aPhoNetSender, //pointer to phonet sender
    CMmPhoNetReceiver* aPhoNetReceiver, // pointer to phonet receiver
    CMmMessageRouter* aMessageRouter    //pointer to message router
    )
    {
TFLOGSTRING("TSY: CMmUssdMessHandler::NewL.\n");
OstTrace0( TRACE_NORMAL, CMMUSSDMESSHANDLER_NEWL, "CMmUssdMessHandler::NewL" );

    CMmUssdMessHandler* ussdMessHandler =
        new( ELeave ) CMmUssdMessHandler();

    CleanupStack::PushL( ussdMessHandler );

    ussdMessHandler->ConstructL();
    ussdMessHandler->iPhoNetSender = aPhoNetSender;
    ussdMessHandler->iMessageRouter = aMessageRouter;
    aPhoNetReceiver->RegisterL(ussdMessHandler, PN_SS, SS_GSM_USSD_SEND_RESP);
    aPhoNetReceiver->RegisterL(ussdMessHandler, PN_SS, SS_SERVICE_FAILED_RESP);
    aPhoNetReceiver->RegisterL(ussdMessHandler, PN_SS, SS_GSM_USSD_RECEIVE_IND);

    CleanupStack::Pop( ussdMessHandler );

    return ussdMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::ReceiveMessageL
// Called when an ISI message has been received.
// -----------------------------------------------------------------------------
//
void CMmUssdMessHandler::ReceiveMessageL
    (
    const TIsiReceiveC& aIsiMessage    // ISI-message received
    )
    {
    TInt resource (aIsiMessage.Get8bit(ISI_HEADER_OFFSET_RESOURCEID) );
    TInt messageId (aIsiMessage.Get8bit(ISI_HEADER_OFFSET_MESSAGEID) );

TFLOGSTRING3("TSY: CMmUssdMessHandler::ReceiveMessageL - resouce: %d, msgId: %d", resource, messageId);
OstTraceExt2( TRACE_NORMAL, CMMUSSDMESSHANDLER_RECEIVEMESSAGEL, "CMmUssdMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch( resource )
        {
        case PN_SS:
            {
            switch( messageId )
                {
                case SS_GSM_USSD_SEND_RESP:
                    {
                    SsGsmUssdSendResp( aIsiMessage );
                    break;
                    }
                case SS_SERVICE_FAILED_RESP:
                    {
                    SsServiceFailedResp( aIsiMessage );
                    // SS_SERVICE_FAILED_RESP message is needed by Custom
                    // message handler
                    break;
                    }
                case SS_GSM_USSD_RECEIVE_IND:
                    {
                    SsGsmUssdReceiveInd( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING2("TSY: CMmUssdMessHandler::ReceiveMessageL - PN_SS - unknown msgId: %d", messageId);
OstTrace1( TRACE_NORMAL, DUP1_CMMUSSDMESSHANDLER_RECEIVEMESSAGEL, "CMmUssdMessHandler::ReceiveMessageL;unknown messageId=%d", messageId );
                    // No propiert handler methods for ISI-message found
                    break;
                    }
                }
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmUssdMessHandler::ReceiveMessageL - unknown resource: %d", resource);
OstTrace1( TRACE_NORMAL, DUP2_CMMUSSDMESSHANDLER_RECEIVEMESSAGEL, "CMmUssdMessHandler::ReceiveMessageL;resource=%d", resource );
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CMmUssdMessHandler::SsGsmUssdSendReq
// Construct a SS_GSM_USSD_SEND_REQ ISI message
// -----------------------------------------------------------------------------
//
TInt CMmUssdMessHandler::SsGsmUssdSendReq
    (
    const CMmDataPackage* aDataPackage
    )
    {
TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdSendReq");
OstTrace0( TRACE_NORMAL, CMMUSSDMESSHANDLER_SSGSMUSSDSENDREQ, "CMmUssdMessHandler::SsGsmUssdSendReq" );
    TInt ret ( KErrNone );
    TBuf8<RMobileUssdMessaging::KGsmUssdDataSize> data( 0 );
    TDes8* attributes( 0 );
    TUint8 numOfSubblocks( 0 );

    aDataPackage->UnPackData( data, attributes );

    RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* msgAttrPck (
        (RMobileUssdMessaging::TMobileUssdAttributesV1Pckg*) ( attributes ) );

    RMobileUssdMessaging::TMobileUssdAttributesV1 msgAttr ( ( *msgAttrPck )() );

TFLOGSTRING2("TSY: CMmUssdMessHandler::SsGsmUssdSendReq. AttributeType: %d", msgAttr.iType );
OstTrace1( TRACE_NORMAL, DUP1_CMMUSSDMESSHANDLER_SSGSMUSSDSENDREQ, "CMmUssdMessHandler::SsGsmUssdSendReq;msgAttr.iType=%d", msgAttr.iType );

    TUint8 ussdType( KFiller ); //initialize ussdtype to 0

    if ( RMobileUssdMessaging::KUssdMessageType & msgAttr.iFlags )
        {
        switch ( msgAttr.iType )
            {
            // User has requested to send MO USSD
            case RMobileUssdMessaging::EUssdMORequest:
                {
                ussdType = SS_GSM_USSD_COMMAND;
                break;
                }
            case RMobileUssdMessaging::EUssdMOReply:
                {
TFLOGSTRING2("TSY: CMmUssdMessHandler::SsGsmUssdSendReq; iLastMtUssdIsRequest = %d", iLastMtUssdIsRequest);
OstTrace1( TRACE_NORMAL, DUP5_CMMUSSDMESSHANDLER_SSGSMUSSDSENDREQ, "CMmUssdMessHandler::SsGsmUssdSendReq;iLastMtUssdIsRequest=%d", iLastMtUssdIsRequest );
                if ( iLastMtUssdIsRequest )
                    {
                    ussdType = SS_GSM_USSD_MT_REPLY;
                    }
                else
                    {
                    ussdType = SS_GSM_USSD_COMMAND;
                    }
                // no need to reset the field, because MT USSD indication
                // will always precede this kind of response
                break;
                }
            // User is signing for the MT request
            case RMobileUssdMessaging::EUssdMOAcknowledgement:
                {
                ussdType = SS_GSM_USSD_NOTIFY;
                break;
                }
            // Unknown or illegal cases return error to client
            default:
                {
                // ussd type is unknown, report error
                ret = KErrArgument;
                break;
                }
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdSendReq. Type flag missing" );
OstTrace0( TRACE_NORMAL, DUP2_CMMUSSDMESSHANDLER_SSGSMUSSDSENDREQ, "CMmUssdMessHandler::SsGsmUssdSendReq, Type flag missing" );
        // ussd type is unknown, report error
        ret = KErrArgument;
        }

    TUint8 codingInfo( KFiller );  //initialize codinginfo to 0
    // If messagetype was valid, check data coding scheme.
    if ( KErrNone == ret )
        {
        if ( RMobileUssdMessaging::KUssdMessageDcs & msgAttr.iFlags )
            {
            //get data coding scheme
            codingInfo = msgAttr.iDcs;
            }
         else
            {
TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdSendReq. DCS flag missing" );
OstTrace0( TRACE_NORMAL, DUP3_CMMUSSDMESSHANDLER_SSGSMUSSDSENDREQ, "CMmUssdMessHandler::SsGsmUssdSendReq, DCS flag missing" );
            // data coding scheme is unknown, report error.
            ret = KErrArgument;
            }
        }

    // If messagetype and data coding scheme were valid, send message.
    if ( KErrNone == ret )
        {
        if ( RMobileUssdMessaging::KUssdDataFormat & msgAttr.iFlags )
            {
            // Create UssdSendReq ISI-message
            TIsiSend ssGsmUssdSendReq( iPhoNetSender->SendBufferDes() );
            ssGsmUssdSendReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SS );
            ssGsmUssdSendReq.Set8bit( ISI_HEADER_OFFSET_TRANSID, KUssdTransId );
            ssGsmUssdSendReq.Set8bit(
                ISI_HEADER_OFFSET_MESSAGEID,
                SS_GSM_USSD_SEND_REQ );

            ssGsmUssdSendReq.Set8bit( ISI_HEADER_SIZE +
                SS_GSM_USSD_SEND_REQ_OFFSET_USSDTYPE, ussdType );

            if ( SS_GSM_USSD_NOTIFY != ussdType )
                {
                // Create SsGsmUssdString subblock.
                // subblock header + ussd string length subblock max length = 164
                TBuf8<KMaxLengthOfUssdMessage + 3> ssGsmUssdStringSb( 0 );
                TIsiSubBlock subblockSSGsmUssdString(
                    ssGsmUssdStringSb, SS_GSM_USSD_STRING, EIsiSubBlockTypeId8Len8 );

                ssGsmUssdStringSb.Append( codingInfo );
                ssGsmUssdStringSb.Append( data.Length() );
                ssGsmUssdStringSb.Append( data );

                ssGsmUssdSendReq.CopyData( ISI_HEADER_SIZE +
                    SIZE_SS_GSM_USSD_SEND_REQ,
                    subblockSSGsmUssdString.CompleteSubBlock() );
                numOfSubblocks++;

                if ( SS_GSM_USSD_COMMAND == ussdType )
                    {
                    // create subblock SS_SB_CHECK_INFO
                    TBuf8<SIZE_SS_SB_CHECK_INFO>sbData( 0 );
                    TIsiSubBlock ssCheckInfoSb(
                        sbData,
                        SS_SB_CHECK_INFO,
                        EIsiSubBlockTypeId8Len8 );

                    if( iNoFdnUSSDReq )
                        {
                        sbData.Append( SS_FDN_CHECK_SUPPRESS );
                        }
                    else
                        {
                        sbData.Append( SS_NO_FDN_CHECK_SUPPRESS );
                        }

                    sbData.Append( SS_NO_RESOURCE_CONTROL_SUPPRESS );

                    // add the SIZE_SS_SB_CHECK_INFO subblock to service req
                    ssGsmUssdSendReq.CopyData(
                        ISI_HEADER_SIZE + SIZE_SS_GSM_USSD_SEND_REQ +
                        ssGsmUssdStringSb.Length(),
                        ssCheckInfoSb.CompleteSubBlock( ) );
                    numOfSubblocks++;
                    }
                }

            ssGsmUssdSendReq.Set8bit( ISI_HEADER_SIZE +
                SS_GSM_USSD_SEND_REQ_OFFSET_SUBBLOCKCOUNT, numOfSubblocks );

            //send message via phonet
            ret = iPhoNetSender->Send( ssGsmUssdSendReq.Complete() );
            }
        else
            {
            // dataformat is unspecified, report error.
            ret = KErrArgument;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::SsGsmUssdSendReq
// Construct a SS_GSM_USSD_SEND_REQ ISI message that ends ussd session
// -----------------------------------------------------------------------------
//
TInt CMmUssdMessHandler::SsGsmUssdSendReq
    (
    void
    )
    {
TFLOGSTRING("LTSY: CMmUssdMessHandler::SsGsmUssdSendReq. End USSD session" );
OstTrace0( TRACE_NORMAL, DUP4_CMMUSSDMESSHANDLER_SSGSMUSSDSENDREQ, "CMmUssdMessHandler::SsGsmUssdSendReq" );

    // Change the flag to indicate the correct complete method for failed resp
    iIsSendReleaseCalled = ETrue;
    // No pending request anymore
    iIsSendReleaseReqPending = EFalse;

    // create buffer for isi msg data
    TBuf8<2> messageData( 0 );

    messageData.Append( SS_GSM_USSD_END );
    // number of sub blocks
    messageData.Append( 0 );

    return iPhoNetSender->Send(
        PN_SS,
        KUssdTransId,
        SS_GSM_USSD_SEND_REQ,
        messageData );
    }

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::SsGsmUssdSendResp
// Completes successful Ss Gsm Ussd Send Request.
// -----------------------------------------------------------------------------
//
void CMmUssdMessHandler::SsGsmUssdSendResp
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {
TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdSendResp" );
OstTrace0( TRACE_NORMAL, CMMUSSDMESSHANDLER_SSGSMUSSDSENDRESP, "CMmUssdMessHandler::SsGsmUssdSendResp" );

    TUint8 length ( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_GSM_USSD_SEND_RESP_OFFSET_LENGTHOFUSSDSTRING ) );

    TBuf8<KMaxLengthOfUssdMessage> ussdString ( aIsiMessage.GetData(
        ISI_HEADER_SIZE + SS_GSM_USSD_SEND_RESP_OFFSET_USSDSTRING, length ) );

    TUint8 ussdType ( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_GSM_USSD_SEND_RESP_OFFSET_USSDTYPE ) );

    //create package.
    CMmDataPackage package;

    // If the message sent was of type SS_GSM_USSD_END, and the request
    // was a SendRelease request, then a different request must be completed
    if ( SS_GSM_USSD_END == ussdType && iIsSendReleaseCalled )
        {
TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdSendResp. Response to SendRelease." );
OstTrace0( TRACE_NORMAL, DUP1_CMMUSSDMESSHANDLER_SSGSMUSSDSENDRESP, "CMmUssdMessHandler::SsGsmUssdSendResp, Response to SendRelease" );

        // Reset the flag
        iIsSendReleaseCalled = EFalse;

        RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;

        returnResult.iOpCode = KErrNone;
        returnResult.iAdditionalInfo.Append( KErrNone );

        //pack the data
        package.PackData( &returnResult );

        //complete the request
        iMessageRouter->Complete(
            EMobileUssdMessagingSendRelease,
            &package,
            KErrNone);
        }
    // otherwise complete the send message request
    else
        {
        if ( iNoFdnUSSDReq )
            {
            // Call CompleteSendMessageNoFdnCheck request.
            iMessageRouter->Complete(
                EMobileUssdMessagingSendMessageNoFdnCheck,
                KErrNone );
            }
        else
            {
            // Call CompleteSendMessage request.
            iMessageRouter->Complete(
                EMobileUssdMessagingSendMessage,
                KErrNone );
            }

        //Req is not ongoing anymore
        iIsUssdSendReqOnGoing = EFalse;

        //if a cancel request has been made
        if ( iIsSendReleaseReqPending )
            {
TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdSendResp. SendRelease pending." );
OstTrace0( TRACE_NORMAL, DUP2_CMMUSSDMESSHANDLER_SSGSMUSSDSENDRESP, "CMmUssdMessHandler::SsGsmUssdSendResp, SendRelease pending" );

            TInt ret ( KErrNone );
            ret = SsGsmUssdSendReq();

            if ( KErrNone != ret )
                {
                RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;

                returnResult.iOpCode = static_cast <TUint8 > ( KErrGeneral );
                returnResult.iAdditionalInfo.Append(
                    static_cast < TUint8 > ( KErrGeneral ) );

                //complete the request
                iMessageRouter->Complete(
                    EMobileUssdMessagingSendRelease,
                    &package,
                    KErrGeneral );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::SsServiceFailedResp
// Completes unsuccessful Ss Gsm Ussd Send Request ( MO USSD ).
// -----------------------------------------------------------------------------
//
void CMmUssdMessHandler::SsServiceFailedResp
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {

TFLOGSTRING("TSY: CMmUssdMessHandler::SsServiceFailedResp" );
OstTrace0( TRACE_NORMAL, CMMUSSDMESSHANDLER_SSSERVICEFAILEDRESP, "CMmUssdMessHandler::SsServiceFailedResp" );

    // Initialize cause type and value, if no specified.
    // subblock found, use SS_UNSPECIFIED_REASON.
    TUint8 causeType( KErrNone );
    TUint8 causeValue( SS_UNSPECIFIED_REASON );
    // If no sub block, set epocerror to KErrGeneral.
    TInt epocError( KErrGeneral );

    // offset where the subblock starts
    TUint sbStartOffset( KErrNone );
    TBool errorMappingNeeded( ETrue );

    //create package.
    CMmDataPackage package;

    if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
        SS_GSM_INDICATE_MSG_ERROR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        causeType = SS_GSM_INDICATE_MSG_ERROR;
        causeValue = aIsiMessage.Get8bit(
            sbStartOffset + SS_GSM_INDICATE_MSG_ERROR_OFFSET_MESSAGEERRORCODE );
        }
    else if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
        SS_GSM_INDICATE_ERROR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        causeType = SS_GSM_INDICATE_ERROR;
        causeValue = aIsiMessage.Get8bit(
            sbStartOffset + SS_GSM_INDICATE_ERROR_OFFSET_ERRORCODE );
        }
    else if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
        SS_GSM_INDICATE_PROBLEM,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        causeType = aIsiMessage.Get8bit(
            sbStartOffset + SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMTYPE );
        causeValue = aIsiMessage.Get8bit(
            sbStartOffset + SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMCODE );
        }
    else if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
        SS_OTHER_ERROR,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        TUint8 errorCode( aIsiMessage.Get8bit(
            sbStartOffset + SS_OTHER_ERROR_OFFSET_ERRORCODE ) );

        if( SS_RESOURCE_CONTROL_DENIED == errorCode )
            {
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
                SS_SB_RESOURCE_CONTROL_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset ) )
                {
                TUint8 dataLen( aIsiMessage.Get8bit(
                sbStartOffset + SS_SB_RESOURCE_CONTROL_INFO_OFFSET_DATALENGTH ) );

                TPtrC8 data( aIsiMessage.GetData(
                sbStartOffset + SS_SB_RESOURCE_CONTROL_INFO_OFFSET_DATA,
                dataLen ) );
                // sw1, sw2 and result is inserted to SS_SB_RESOURCE_CONTROL_INFO
                // by simatktsy and ther order from first byte is: sw1, sw2 and result
                TUint8 sw1 = data[KSw1Index];
                TUint8 sw2 = data[KSw2Index];
                TUint8 result = data[KResultIndex];
                epocError = CMmStaticUtility::MapSw1Sw2ToEpocError(
                    sw1,
                    sw2,
                    result );
                errorMappingNeeded = EFalse;
                }
            }
        else
            {
            causeType = SS_OTHER_ERROR;
            causeValue = errorCode;
            }
        }
    else if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
        SS_GSM_MM_RELEASED,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        causeType= SS_GSM_MM_RELEASED;
        causeValue = aIsiMessage.Get8bit(
            sbStartOffset + SS_GSM_MM_RELEASED_OFFSET_CAUSE );
        }
    else
        {
        causeType = KNoCsCauseType; // causetype not defined
        }

    //If cause value is SS_GSM_SS_NOT_AVAILABLE (comes in SS_OTHER_ERROR sub block)
    //this indicates that there is not network coverage.
    if( SS_GSM_SS_NOT_AVAILABLE == causeValue )
        {
        epocError = CMmStaticUtility::EpocErrorCode(
            KErrCouldNotConnect,
            KErrGsmSMSNoNetworkService );
TFLOGSTRING("TSY: CMmUssdMessHandler::SsServiceFailedResp. Cause: SS_GSM_SS_NOT_AVAILABLE => No network coverage." );
OstTrace0( TRACE_NORMAL, DUP1_CMMUSSDMESSHANDLER_SSSERVICEFAILEDRESP, "CMmUssdMessHandler::SsServiceFailedResp, Cause: SS_GSM_SS_NOT_AVAILABLE => No network coverage" );
        }
    else
        {
        if( errorMappingNeeded )
            {
            // Translate error to epoc world, Fix RFState problem
            epocError = CMmStaticUtility::CSCauseToEpocError(
                PN_SS,
                causeType,
                causeValue );
            }
        }

    if ( iIsSendReleaseCalled )
        {
TFLOGSTRING("TSY: CMmUssdMessHandler::SsServiceFailedResp. SendRelease failed." );
OstTrace0( TRACE_NORMAL, DUP2_CMMUSSDMESSHANDLER_SSSERVICEFAILEDRESP, "CMmUssdMessHandler::SsServiceFailedResp, SendRelease failed" );

        // Reset the flag
        iIsSendReleaseCalled = EFalse;

        RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;

        returnResult.iOpCode = static_cast < TUint8 > ( KErrGeneral );
        returnResult.iAdditionalInfo.Append( epocError );

        //pack the data
        package.PackData( &returnResult );

        // Call CompleteSendRelease request with epocError from TSY.
        iMessageRouter->Complete(
            EMobileUssdMessagingSendRelease,
            &package,
            epocError );
        }
    else
        {
        if ( iNoFdnUSSDReq )
            {
            // Call CompleteSendMessageNoFdnCheck request with epocError from TSY.
            iMessageRouter->Complete(
                EMobileUssdMessagingSendMessageNoFdnCheck,
                epocError );
            }
        else
            {
            // Call CompleteSendMessage request with epocError from TSY.
            iMessageRouter->Complete(
                EMobileUssdMessagingSendMessage,
                epocError );
            }

        //Request is not ongoing anymore
        iIsUssdSendReqOnGoing = EFalse;

        //if a Cancel request has been made
        if ( iIsSendReleaseReqPending )
            {
            iIsSendReleaseReqPending = EFalse;

            RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;

            returnResult.iOpCode = static_cast < TUint8 > ( KErrNone );
            returnResult.iAdditionalInfo.Append( KErrNone );

            //pack the data
            package.PackData( &returnResult );

            //complete the request immediately without having sent anything
            //to SS server because the 1rst request has failed so there is no
            //need to cancel it.
            iMessageRouter->Complete(
                EMobileUssdMessagingSendRelease,
                &package,
                KErrNone );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmUssdMessHandler::SsGsmUssdReceiveInd
// Breaks a SS_GSM_USSD_RECEIVE_IND ISI message
// -----------------------------------------------------------------------------
//
void CMmUssdMessHandler::SsGsmUssdReceiveInd
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {
    //create package.
    CMmDataPackage notifyPackage;

    TUint8 length ( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_GSM_USSD_SEND_RESP_OFFSET_LENGTHOFUSSDSTRING ) );

    TBuf8<KMaxLengthOfUssdMessage> ussdString ( aIsiMessage.GetData(
        ISI_HEADER_SIZE + SS_GSM_USSD_SEND_RESP_OFFSET_USSDSTRING, length ) );

    TUint8 codingInfo ( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_GSM_USSD_RECEIVE_IND_OFFSET_CODINGINFO ) );

    TUint8 ussdType ( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_GSM_USSD_RECEIVE_IND_OFFSET_USSDTYPE ) );

    RMobileUssdMessaging::TMobileUssdAttributesV1 receiveUssdMessageAttributes;
    TBuf8<RMobileUssdMessaging::KGsmUssdDataSize> receiveUssdMessage( 0 );

    receiveUssdMessage.Copy( ussdString );

    receiveUssdMessageAttributes.iDcs = codingInfo;

    receiveUssdMessageAttributes.iFlags =
        ( RMobileUssdMessaging::KUssdDataFormat+
        RMobileUssdMessaging::KUssdMessageType+
        RMobileUssdMessaging::KUssdMessageDcs );

    receiveUssdMessageAttributes.iFormat =
        ( RMobileUssdMessaging::EFormatPackedString );

TFLOGSTRING2("TSY: CMmUssdMessHandler::SsGsmUssdReceiveInd: UssdType %d", ussdType);
OstTraceExt1( TRACE_NORMAL, CMMUSSDMESSHANDLER_SSGSMUSSDRECEIVEIND, "CMmUssdMessHandler::SsGsmUssdReceiveInd;ussdType=%hhu", ussdType );

    switch ( ussdType )
        {
        case SS_GSM_USSD_REQUEST:
        case SS_GSM_USSD_COMMAND:
            {
            iLastMtUssdIsRequest = SS_GSM_USSD_REQUEST == ussdType;
TFLOGSTRING2("TSY: CMmUssdMessHandler::SsGsmUssdSendReq; iLastMtUssdIsRequest = %d", iLastMtUssdIsRequest);
OstTrace1( TRACE_NORMAL, DUP3_CMMUSSDMESSHANDLER_SSGSMUSSDRECEIVEIND, "CMmUssdMessHandler::SsGsmUssdReceiveInd;iLastMtUssdIsRequest=%d", iLastMtUssdIsRequest );
            receiveUssdMessageAttributes.iType =
                RMobileUssdMessaging::EUssdMTRequest;
            break;
            }
        // Set USSD message type to EUssdMTReply if ussdType is .
        // SS_GSM_USSD_MT_REPLY or SS_GSM_USSD_END.
        case SS_GSM_USSD_MT_REPLY:
            {
            receiveUssdMessageAttributes.iType =
                RMobileUssdMessaging::EUssdMTReply;
            break;
            }
        case SS_GSM_USSD_END:
            {
            receiveUssdMessageAttributes.iType =
                RMobileUssdMessaging::EUssdMTReply;
            RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;
            returnResult.iOpCode = KErrNone;
            returnResult.iAdditionalInfo.Append( KErrNone );

            //pack the data
            notifyPackage.PackData( &returnResult );
            break;
            }
        case SS_GSM_USSD_NOTIFY:
            {
            receiveUssdMessageAttributes.iType =
                RMobileUssdMessaging::EUssdMTNotify;
            break;
            }
        // Can't map SS_GSM_USSD_COMMAND to MM ETel values.
        default:
            {
            receiveUssdMessageAttributes.iType =
                RMobileUssdMessaging::EUssdUnknown;
            break;
            }
        }   // end switch

    //create package.
    CMmDataPackage package;

    //pack the data
    package.PackData( &receiveUssdMessage, &receiveUssdMessageAttributes );

    if ( SS_GSM_USSD_END != ussdType || ussdString.Length() )
        {
        iMessageRouter->Complete(
            EMobileUssdMessagingReceiveMessage,
            &package,
            KErrNone );

TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdReceiveInd: Complete : EMobileUssdMessagingReceiveMessage" );
OstTrace0( TRACE_NORMAL, DUP1_CMMUSSDMESSHANDLER_SSGSMUSSDRECEIVEIND, "CMmUssdMessHandler::SsGsmUssdReceiveInd, EMobileUssdMessagingReceiveMessage" );
        }

    if ( SS_GSM_USSD_END == ussdType )
        {
        // Complete the request
        // This notifies a client of a MT USSD release message to the mobile device.
        iMessageRouter->Complete(
            EMobileUssdMessagingNotifyNetworkRelease,
            &notifyPackage,
            KErrNone );

TFLOGSTRING("TSY: CMmUssdMessHandler::SsGsmUssdReceiveInd: Complete : EMobileUssdMessagingNotifyNetworkRelease" );
OstTrace0( TRACE_NORMAL, DUP2_CMMUSSDMESSHANDLER_SSGSMUSSDRECEIVEIND, "CMmUssdMessHandler::SsGsmUssdReceiveInd, EMobileUssdMessagingNotifyNetworkRelease" );
        }
    }


// -----------------------------------------------------------------------------
// CMmUssdMessHandler::ExtFuncL
// Dispatches Etel requests to DOS level handlers
// -----------------------------------------------------------------------------
//
TInt CMmUssdMessHandler::ExtFuncL
    (
    TInt aIpc,
    const CMmDataPackage* aDataPackage
    )
    {

    //*************************************************************//.
    // NOTE.
    //.
    // LICENSEE SPECIFIC MESSAGE HANDLER IMPLEMENTATION STARTS HERE.
    //.
    //*************************************************************//.

TFLOGSTRING("TSY: CMmUssdMessHandler::ExtFuncL");
OstTrace0( TRACE_NORMAL, DUP1_CMMUSSDMESSHANDLER_EXTFUNCL, "CMmUssdMessHandler::ExtFuncL" );

    TInt ret( KErrNone );

         if( EMobileUssdMessagingSendMessageNoFdnCheck == aIpc )
            {
            TFLOGSTRING("TSY: CMmUssdMessHandler::ExtFuncL - iNoFdnUSSDReq --> TRUE");
            // set flag
            iNoFdnUSSDReq = ETrue;
            }
        else
            {
            TFLOGSTRING("TSY: CMmUssdMessHandler::ExtFuncL - iNoFdnUSSDReq --> FALSE");
            iNoFdnUSSDReq = EFalse;
            }

    switch ( aIpc )
        {
        case EMobileUssdMessagingSendMessage:
        case EMobileUssdMessagingSendMessageNoFdnCheck:
            {
            ret = SsGsmUssdSendReq( aDataPackage );
            iIsUssdSendReqOnGoing = ETrue;
            break;
            }
        case EMobileUssdMessagingSendRelease:
            {
            if ( iIsUssdSendReqOnGoing )
                {
                iIsSendReleaseReqPending = ETrue;
                }
            else
                {
                ret = SsGsmUssdSendReq();
                }
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmUssdMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP2_CMMUSSDMESSHANDLER_EXTFUNCL, "CMmUssdMessHandler::ExtFuncL;aIpc=%d", aIpc );
            ret = KErrNotSupported;
            break;
            }
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmUssdMessHandler::HandlerError
// Handles CMmUssdMessHandler's errors that comes via PhoNetReceiver RunError
// method.
// -----------------------------------------------------------------------------
//
void CMmUssdMessHandler::HandleError
    (
    const TIsiReceiveC& /*aIsiMessage*/,// Isi message
    TInt /*aError*/                     // Error code
    )
    {
TFLOGSTRING("TSY: CMmUssdMessHandler::HandleError");
OstTrace0( TRACE_NORMAL, CMMUSSDMESSHANDLER_HANDLEERROR, "CMmUssdMessHandler::HandleError" );
    }


//end of file

