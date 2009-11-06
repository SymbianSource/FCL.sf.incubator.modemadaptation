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



//  INCLUDE FILES
#include "satdatadownload.h"    // sat datadownload class
#include "satmesshandler.h"     // sat message handler class
#include "satmessaging.h"       // sat messaging class
#include "ber_tlv.h"            // sat ber-tlv classes
#include "satutil.h"            // sat utility class

#include <pn_const.h>           // server id constants
#include <tisi.h>               // isi message
#include <smsisi.h>             // sms server
#include <etelmm.h>             // etel multimode api
#include <uiccisi.h>            // UICC server
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satdatadownloadtraces.h"
#endif


// CONSTANTS
// Temporary TPDU data store buffer
const TUint8 KTtpduMaxSize               = 255;
// Cell Broadcast isi msg length
const TUint8 KCbsMsgMaxLength           = 92;
// Max address length
const TUint8 KAddrMaxLength             = 255;
// ParamIndicator value
const TUint8 KParamIndicators           = 7;
// ParamIndicator length
const TUint8 KParamIndicatorsLength     = 1;
// Max Info Length
const TUint8 KMaxInfoLength             = 82;
// SMS Delivery report buffer size
const TUint8 KDeliveryReportSize        = 248;


// ==================== MEMBER FUNCTIONS ====================================

// -----------------------------------------------------------------------------
// CSatDataDownload::CSatDataDownload
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatDataDownload::CSatDataDownload
        (
        CSatMessHandler* aSatMessHandler,
        CTsySatMessaging* aSatMessaging
        )
        :
        iSatMessHandler( aSatMessHandler ),
        iSatMessaging( aSatMessaging ),
        iSmsPpProtocolId( KZero ),
        iSmsPpDcs( KZero ),
        iSmsPpTransactionId( KZero ),
        iSmsPpDdOngoing( EFalse ), // No SMS PP DD on going during construction
        iSmsPpDdSupported( ETrue ) // Most likely SIM supports SMS PP DD
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_CSATDATADOWNLOAD, "CSatDataDownload::CSatDataDownload" );
    }


// -----------------------------------------------------------------------------
// CSatDataDownload::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatDataDownload::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_CONSTRUCTL, "CSatDataDownload::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CSatDataDownload::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatDataDownload* CSatDataDownload::NewL
        (
        CSatMessHandler*    aSatMessHandler,
        CTsySatMessaging*   aSatMessaging
        )
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_NEWL, "CSatDataDownload::NewL" );
    TFLOGSTRING("TSY: CSatDataDownload::NewL");

    CSatDataDownload* self =
        new ( ELeave ) CSatDataDownload( aSatMessHandler, aSatMessaging );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CSatDataDownload::~CSatDataDownload
// C++ destructor
// -----------------------------------------------------------------------------
//
CSatDataDownload::~CSatDataDownload()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATDATADOWNLOAD_CSATDATADOWNLOAD, "CSatDataDownload::~CSatDataDownload" );
    TFLOGSTRING("TSY: CSatDataDownload::~CSatDataDownload");
    }


// -----------------------------------------------------------------------------
// CSatDataDownload::UiccCatRespEnvelopeReceived
// Breaks a ISI message data notification from Sim server
// -----------------------------------------------------------------------------
//
void CSatDataDownload::UiccCatRespEnvelopeReceived(
    const TIsiReceiveC& aIsiMessage
    )
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_UICCCATRESPENVELOPERECEIVED, "CSatDataDownload::UiccCatRespEnvelopeReceived" );
    TFLOGSTRING("TSY:CSatDataDownload::UiccCatRespEnvelopeReceived");

    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_STATUS ) );

    TUint8 sw1( 0 );
    TUint8 sw2( 0 );
    TPtrC8 apduData;

    if ( UICC_STATUS_OK == status )
        {
        TUint uiccSbApduOffset( 0 );
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_UICC_CAT_RESP,
            UICC_SB_APDU,
            EIsiSubBlockTypeId16Len16,
            uiccSbApduOffset ) )
            {
            TUint16 apduLength( aIsiMessage.Get16bit(
                uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDULENGTH ) );
            apduData.Set( aIsiMessage.GetData(
                uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDU,
                apduLength ) );
            // Status bytes are two last bytes in APDU
            TUint8 sw1( apduData[apduLength - 2] );
            TUint8 sw2( apduData[apduLength - 1] );
            }
        else // Subblock is mandatory
            {
            TFLOGSTRING("TSY: CSatMessHandler::UiccCatRespEnvelopeReceived - Mandatory subblock UICC_SB_APDU not found");
            OstTrace0( TRACE_NORMAL, DUP1_CSATDATADOWNLOAD_UICCCATRESPENVELOPERECEIVED, "CSatDataDownload::UiccCatRespEnvelopeReceived- Mandatory subblock UICC_SB_APDU not found" );
            }
        }

    // Create delivery report according to SW result
    if ( KError != TSatUtility::Sw1Sw2Check( sw1, sw2 ) )
        {
        TFLOGSTRING("TSY: CSatDataDownload::UiccCatRespEnvelopeReceived, OK");
        OstTrace0( TRACE_NORMAL, DUP2_CSATDATADOWNLOAD_UICCCATRESPENVELOPERECEIVED, "CSatDataDownload::UiccCatRespEnvelopeReceived" );
        BuildSimMsgReport( ENone, apduData );
        }
    else if ( KAtkSwDataNtfSw1busy == sw1 )
        {
        TFLOGSTRING("TSY:CSatDataDownload::UiccCatRespEnvelopeReceived, SIM Busy");
        OstTrace0( TRACE_NORMAL, DUP3_CSATDATADOWNLOAD_UICCCATRESPENVELOPERECEIVED, "CSatDataDownload::UiccCatRespEnvelopeReceived" );
        BuildSimMsgReport( ESatBusy, apduData );
        }
    else
        {
        TFLOGSTRING("TSY:CSatDataDownload::UiccCatRespEnvelopeReceived, Data Download Error");
        OstTrace0( TRACE_NORMAL, DUP4_CSATDATADOWNLOAD_UICCCATRESPENVELOPERECEIVED, "CSatDataDownload::UiccCatRespEnvelopeReceived" );
        BuildSimMsgReport( ESatDlError, apduData );
        }
    }


// -----------------------------------------------------------------------------
// CSatDataDownload::BuildSmsSimReport
// Creates a SMS PP DD delivery report
// -----------------------------------------------------------------------------
//


void CSatDataDownload::BuildSimMsgReport
        (
        const TTpFailure aTpFailure,
        const TDesC8& aUserData
        )
    {
    OstTraceExt2( TRACE_NORMAL, CSATDATADOWNLOAD_BUILDSIMMSGREPORT, "CSatDataDownload::BuildSimMsgReport TpFailure: %{TTpFailure}, UserDataLen: %d", aTpFailure, aUserData.Length() );
    TFLOGSTRING3("TSY:CSatDataDownload::BuildSimMsgReport TpFailure: %x, UserDataLen: %d", aTpFailure, aUserData.Length() );

    // Select Cause and CauseType according to routing result
    TUint8 causeType( ENone == aTpFailure ?
        SMS_CAUSE_TYPE_COMMON : SMS_CAUSE_TYPE_EXT );
    TUint8 smsCause( ENone == aTpFailure ?
        SMS_OK : SMS_ERR_PP_RESERVED );

    TUint16 dataLen( aUserData.Length() );

    // SMS_RECEIVED_MSG_REPORT_REQ- first subblock is needed when:
    //  1) Delivery of SMS PP failed
    // second and third subblock is added when
    //  2)  SIM provided data in AtkSwDataNtf

    // Create SMS_RECEIVED_MSG_REPORT_REQ message
    TBuf8<KDeliveryReportSize> msgBuffer;
    TIsiSend reportReq( msgBuffer );

    // Report has following structure:
    // SMS_RECEIVED_MSG_REPORT_REQ   (Minimum, OK case AND no SIM Data)
    // + SMS_SB_DELIVER_REPORT       (optional  Added when reception is not OK)
    //   + SMS_SB_PARAM_INDICATOR    (optional, Added when user data is not empty )
    //     + SMS_SB_USER_DATA        (optional, Response user data is put here)

    // SMS_RECEIVED_MSG_REPORT_REQ header
    msgBuffer.Append( causeType );
    msgBuffer.Append( smsCause );
    msgBuffer.AppendFill( KPadding, 3 );
    msgBuffer.Append( 0 );           // no of sublocks

    // Add SMS_SB_DELIVER_REPORT subblock if any failure is there
    if( ENone != aTpFailure )
        {
        TFLOGSTRING("TSY:CSatDataDownload::BuildSimMsgReport \
                 Adding SMS_SB_DELIVER_REPORT" );
        OstTrace0( TRACE_NORMAL, DUP1_CSATDATADOWNLOAD_BUILDSIMMSGREPORT, "CSatDataDownload::BuildSimMsgReport Adding SMS_SB_DELIVER_REPORT" );

        TIsiSubBlock deliverReport( msgBuffer, SMS_SB_DELIVER_REPORT,EIsiSubBlockTypeId16Len16 );

        // set message parameters zero as in S40 ATK server also
        // First octet of SMS-Deliver-Report TPDU. Contains TP-UDHI and TP-MTI
        // elements. See 3GPP TS 23.040 chapter 9.2.2.1a (i) and (ii)
        // SMS-DELIVER-REPORT for RP-ACK and RP-ERROR.

        msgBuffer.Append( 0x00 ); // SMS_MTI_DELIVER_REPORT

        msgBuffer.Append( aTpFailure ); // GSM-TP Failure cause
        msgBuffer.AppendFill( KPadding, 2);
        // Increment number of subblock
        msgBuffer[5]++;
        deliverReport.CompleteSubBlock();
        }

    // Add SMS_SB_PRSM_INDICATOR and SMS_SB_USER_DATA subblock
    if ( dataLen )
        {
        TFLOGSTRING("TSY:CSatDataDownload::BuildSimMsgReport \
              Adding SMS_SB_PARAM_INDICATOR & SMS_SB_USER_DATA" );
        OstTrace0( TRACE_NORMAL, DUP2_CSATDATADOWNLOAD_BUILDSIMMSGREPORT, "CSatDataDownload::BuildSimMsgReport Adding SMS_SB_PARAM_INDICATOR AND SMS_SB_USER_DATA" );

        // Add two more Sublock:
        // SMS_SB_PARAM_INDICATOR
        /////////////////////////
        TIsiSubBlock paramInd( msgBuffer, SMS_SB_PARAM_INDICATOR,
                EIsiSubBlockTypeId16Len16 );

        msgBuffer.Append( iSmsPpProtocolId );
        msgBuffer.Append( iSmsPpDcs );

        msgBuffer.Append( KParamIndicatorsLength ); // = 1
        msgBuffer.Append( KParamIndicators ); // All selected (=7)
        // Increment number of subblock
        msgBuffer[5]++;

        paramInd.CompleteSubBlock();

        // SMS_SB_USER_DATA (subblock for TPDU data)
        //////////////////
        TIsiSubBlock userData( msgBuffer, SMS_SB_USER_DATA,
                EIsiSubBlockTypeId16Len16 );

        // data length
        // to append MSB byte
        msgBuffer.Append( dataLen >> 8 );
        msgBuffer.Append( dataLen );

        // Append for character count on basis of DCS used
        TUint16 dataLengthInOctets = 0;
        TUint8 aDefaultAlphabet = ( iSmsPpDcs && 0x0C) ;
        if( !aDefaultAlphabet )
            {
            dataLengthInOctets =  ( ( dataLen + 1 ) * 7 ) / 8 ;
            }
        else
            {
            dataLengthInOctets = dataLen;
            }
        // To append MSB byte
        msgBuffer.Append( dataLengthInOctets >> 8 );
        msgBuffer.Append( dataLengthInOctets );

        // Append whole msg or SMS_GSM_DELIVER_ACK_UD_MAX_LEN bytes
        msgBuffer.Append( aUserData.Left(Min( dataLen,
                SMS_COMMAND_DATA_MAX_LEN ) ) );
        // Increment number of subblock
        msgBuffer[5]++;

        userData.CompleteSubBlock();
        }

    iSatMessHandler->SendSmsReportReq( iSatMessaging->GetTransactionId(),
            msgBuffer );
    }


// -----------------------------------------------------------------------------
// CSatDataDownload::CellBroadcastReceived
// Breaks a cell broadcast isi message coming from network.
// Calls SendEnvelopeForCellBroadcast in order to send the envelope to
// SIM server
// -----------------------------------------------------------------------------
//
void CSatDataDownload::CellBroadcastReceived
        (
        const TIsiReceiveC& aIsiMessage // SMS_GSM(_TEMP)_CB_ROUTING_NTF
        )
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_CELLBROADCASTRECEIVED, "CSatDataDownload::CellBroadcastReceived" );
    TFLOGSTRING( "TSY:CSatDataDownload::CellBroadcastReceived" );

    TBuf8<KCbsMsgMaxLength> cbsMsg;

    // For each subblock i
    //     Check whether the CB message id of this subblock is allowed:
    //     if allowed,
    //          - Retrieve the infoLenght byte B
    //          - if B==0xFF, then this is a GSM CB DDL message
    //          - otherwise this is a WCDMA CD DDL message (0<B<83)
    //              and the content of message must be truncated to B bytes,
    //              the unused bytes (B+1 to 82) must be filled with zeros.
    //          - send envelope containing Page data
    // End

    TInt sbNumber( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SMS_CB_SIM_ROUTING_IND_OFFSET_SUBBLOCKCOUNT ) );

    TUint startsbOffset( 0 );

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SMS_CB_SIM_ROUTING_IND ,
            SMS_SB_CB_MESSAGE,
            EIsiSubBlockTypeId16Len16,
            startsbOffset ) )
        {
        TUint sbOffset( 0 );
        for ( TInt sb( 1 ); sb < sbNumber; sb++ )
            {
            // fill cbsMsg block with 0
            cbsMsg.Zero();
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
                    startsbOffset ,
                    sb,
                    EIsiSubBlockTypeId16Len16,
                    sbOffset ) )
                {
                //Append Serial number
                cbsMsg.Append( ( aIsiMessage.Get16bit(
                        sbOffset + SMS_SB_CB_MESSAGE_OFFSET_SERIALNUMBER )>> 8) );
                cbsMsg.Append( aIsiMessage.Get16bit(
                        sbOffset + SMS_SB_CB_MESSAGE_OFFSET_SERIALNUMBER ) );

                // Append CB Message id
                cbsMsg.Append( aIsiMessage.Get16bit(
                        sbOffset + SMS_SB_CB_MESSAGE_OFFSET_CBMESSAGEID )>> 8 );
                cbsMsg.Append( aIsiMessage.Get16bit(
                        sbOffset + SMS_SB_CB_MESSAGE_OFFSET_CBMESSAGEID ) );

                // Append Data Coding scheme
                cbsMsg.Append( aIsiMessage.Get8bit(
                        sbOffset + SMS_SB_CB_MESSAGE_OFFSET_DATACODINGSCHEME ) );

                // Append Page
                cbsMsg.Append( aIsiMessage.Get8bit(
                        sbOffset + SMS_SB_CB_MESSAGE_OFFSET_PAGE ) );

                // Append data length
                TUint8 infoLength( aIsiMessage.Get8bit(
                        sbOffset + SMS_SB_CB_MESSAGE_OFFSET_INFOLENGTH ) );

                if ( KMaxInfoLength < infoLength )
                    {
                    // CB message max length is 82. Info length may contain
                    // value 0xFF, which means that length should be ignored.
                    // Inthis case maximum is used.
                    infoLength = SMS_CB_MESSAGE_CONTENT_SIZE; // 82
                    }

                // Append the message content and fill with zeroes if necessary
                cbsMsg.AppendJustify( aIsiMessage.GetData( sbOffset +
                        SMS_SB_CB_MESSAGE_OFFSET_CONTENTOFMESSAGE, infoLength ),
                        SMS_CB_MESSAGE_CONTENT_SIZE, ELeft, KPadding );

                SendCellBroadcastDdlEnvelope(
                        iSatMessaging->GetTransactionId(),
                        cbsMsg );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatDataDownload::SmsSimMsgIndReceived
// Breaks a sms point to point isi message
// Sends envelope
// -----------------------------------------------------------------------------
//

void CSatDataDownload::SmsSimMsgIndReceived
        (
         const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_SMSSIMMSGINDRECEIVED, "CSatDataDownload::SmsSimMsgIndReceived" );
    TFLOGSTRING( "TSY:CSatDataDownload::SmsSimMsgIndReceived" );

    TBuf8<KAddrMaxLength> bcdSmscAddress;          // to store Service centre number
    TBuf8<KTtpduMaxSize> smsTpdu;                  // Temporary buffer to store TPDU data
    TUint sbDeliver( 0 );

    // Check for correct subblock SMS_SB_ADDRESS
    if( KErrNone == aIsiMessage.FindSubBlockOffsetById(
         ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_SIM_MSG_IND,
         SMS_SB_ADDRESS,
         EIsiSubBlockTypeId16Len16,
         sbDeliver ) )
        {
        TUint8 addressLen( aIsiMessage.Get8bit(
            sbDeliver + SMS_SB_ADDRESS_OFFSET_ADDRESSDATALENGTH ) );
        // Service centre Address  append in bcdSmscAddress buffer
        // If address type is SMS_SMSC_ADDRESS, addtional length is
        // included in addres data. Let's omit that.
        TUint8 addressType( aIsiMessage.Get8bit(
            sbDeliver + SMS_SB_ADDRESS_OFFSET_ADDRESSTYPE ) );

        if ( SMS_SMSC_ADDRESS == addressType )
            {
             bcdSmscAddress.Append( aIsiMessage.GetData(
                 sbDeliver + SMS_SB_ADDRESS_OFFSET_ADDRESSDATA + 1,
                 addressLen - 1 ) );
            }
        }
    // Check for correct Subblock SMS_SB_TPDU
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SMS_RECEIVED_SIM_MSG_IND,
            SMS_SB_TPDU,
            EIsiSubBlockTypeId16Len16,
            sbDeliver ) )
        {
        // Read user data length
        TUint8 userDataLen( aIsiMessage.Get8bit(
            sbDeliver + SMS_SB_TPDU_OFFSET_DATALENGTH ) );
        // SMS_SB_TPDU subblock,
        // which contain the message payload data.
        smsTpdu.Append( aIsiMessage.GetData(
            sbDeliver + SMS_SB_TPDU_OFFSET_DATABYTES, userDataLen ));
        // Destination Address Length
        // 1st Byte of destination address contains no os semioctets 
        //in address bytes

        // +1 to calculate correct nos of bytes in address
        // divide by 2 to convert semioctets to no of octets
        // +2 to add type of address byte and no of
        TUint8 tpduIndexCalc( ( ( smsTpdu[1] + 1 )/2 ) + 2 ); 

        // Storing protocol id and datacoding scheme from TPDU data buffer
        iSmsPpProtocolId = smsTpdu[ tpduIndexCalc + 1 ];
        iSmsPpDcs = smsTpdu[ tpduIndexCalc + 2 ];
        }

    // Either Envelope sending or SMS storing is going on
    iSmsPpDdOngoing = ETrue;

    if( iSmsPpDdSupported ) // Check for SMS PP-DATA Download supported
        {
        TFLOGSTRING( "TSY:CSatDataDownload::SmsSimMsgIndReceived SMS SIM Supported, sending Envelope..." );
        OstTrace0( TRACE_NORMAL, DUP1_CSATDATADOWNLOAD_SMSSIMMSGINDRECEIVED, "CSatDataDownload::SmsSimMsgIndReceived SMS Sim Supported, sending Envelope..." );

        iSmsPpTransactionId = iSatMessaging->GetTransactionId();
        // Data Download supported, send envelope
        SendSmsPpDdlEnvelope(
            iSmsPpTransactionId,
            bcdSmscAddress,
            smsTpdu );
        }
    else
        {
        TFLOGSTRING( "TSY:CSatDataDownload::SmsSimMsgIndReceived SMS SIM not supported, storing SMS..." );
        OstTrace0( TRACE_NORMAL, DUP2_CSATDATADOWNLOAD_SMSSIMMSGINDRECEIVED, "CSatDataDownload::SmsSimMsgIndReceived SMS SIM not supported, storing SMS..." );

        // Save the SMS instead.
        RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntry;

        // if SMSC present
        if ( bcdSmscAddress.Length() )
            {
            // TON & NPI, stored in first index
            TSatUtility::GetTonAndNpi(
                bcdSmscAddress[0],
                smsEntry.iServiceCentre.iTypeOfNumber,
                smsEntry.iServiceCentre.iNumberPlan );

            // SMSC Address, exclude TON/NPI
            TBuf8<KAddrMaxLength> scAscii;
            TSatUtility::BCDToAscii( bcdSmscAddress.Mid( 1 ), scAscii );
             smsEntry.iServiceCentre.iTelNumber.Copy( scAscii );
            }
        //  TPDU data
        smsEntry.iMsgData.Copy( smsTpdu );

        // Store SMS
        if ( KErrNone != iSatMessaging->StoreSmsL( smsEntry ) )
            {
            // Clear the flag
            iSmsPpDdOngoing = EFalse;
            BuildSimMsgReport( ESatDlError, KNullDesC8 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatDataDownload::SendCellBroadcastDdlEnvelope
// Prepare the envelope to be sent to SIM server after a Cell Broadcast message
// has been received, without request, by the mobile equipment.
// -----------------------------------------------------------------------------
//
void CSatDataDownload::SendCellBroadcastDdlEnvelope
        (
        TUint8 aTransId,
        TDesC8& aPdu
        )
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_SENDCELLBROADCASTDDLENVELOPE, "CSatDataDownload::SendCellBroadcastDdlEnvelope" );
    TFLOGSTRING( "TSY:CSatDataDownload::SendEnvelopeForCellBroadcast" );

    TTlv envelope;
    //Tag
    envelope.Begin( KBerTlvCellBroadcastTag );
    //device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KNetwork );
    envelope.AddByte( KSim );
    // cell broadcast page
    envelope.AddTag( KTlvCellBroadcastPageTag );
    envelope.AddData( aPdu );

    iSatMessHandler->UiccCatReqEnvelope( aTransId, envelope.End() );
    }

// -----------------------------------------------------------------------------
// CSatDataDownload::SendSmsPpDdlEnvelope
// Prepare the envelope to be sent to SIM server after a SMS-PP message has
// has been received, without request, by the mobile equipment.
// -----------------------------------------------------------------------------
//
void CSatDataDownload::SendSmsPpDdlEnvelope
        (
        TUint8 aTransId,
        TDesC8& aSmsScAddress,
        TDesC8& aPdu
        )
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_SENDSMSPPDDLENVELOPE, "CSatDataDownload::SendSmsPpDdlEnvelope" );
    TFLOGSTRING( "TSY:CSatDataDownload::SendEnvelopeForSmsPpDownload" );

    TTlv envelope;

    envelope.Begin( KBerTlvSmsPpDownloadTag );
    //device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KNetwork );
    envelope.AddByte( KSim );
    envelope.AddTag( KTlvAddressTag );
    envelope.AddData( aSmsScAddress );
    envelope.AddTag( KTlvSmsTpduTag );
    envelope.AddData( aPdu );

    iSatMessHandler->UiccCatReqEnvelope( aTransId, envelope.End() );
    }


// -----------------------------------------------------------------------------
// CSatDataDownload::SmsPpDlSupported
// Setter for SMP PP DD support status
// -----------------------------------------------------------------------------
//
void CSatDataDownload::SmsPpDlSupported
        (
        TBool aStatus
        )
    {
    OstTrace1( TRACE_NORMAL, CSATDATADOWNLOAD_SMSPPDLSUPPORTED, "CSatDataDownload::SmsPpDlSupported: %d", aStatus );
    TFLOGSTRING2( "TSY: CSatDataDownload::SmsPpDlSupported: %d", aStatus );

    iSmsPpDdSupported = aStatus;
    }


// -----------------------------------------------------------------------------
// CSatDataDownload::MessageReceivedL
// Handle received messages related to event download
// Called by CSatMessHandler::MessageReceivedL, when a new ISI message arrives.
// -----------------------------------------------------------------------------
//
void CSatDataDownload::MessageReceivedL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATDATADOWNLOAD_MESSAGERECEIVEDL, "CSatDataDownload::MessageReceivedL" );
    TFLOGSTRING( "TSY:CSatDataDownload::MessageReceivedL" );

    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    // handle SIM Data Download related isi messages
    if ( PN_SMS == resource )
        {
        switch ( messageId )
            {
            case SMS_CB_SIM_ROUTING_IND:
                {
                // A Cell Broadcast message was sent by the
                // network, without having been requested by the phone.
                // Process received message
                CellBroadcastReceived( aIsiMessage );
                break;
                }
            case SMS_RECEIVED_SIM_MSG_IND:
                {
                // Receive SMS PP Data Download Indication , Send by SMS Server
                // on reception of message on network
                SmsSimMsgIndReceived(aIsiMessage);
                break;
                }
            default:
                {
                // none
                break;
                }
            }
        }
    else if ( PN_UICC == resource )
        {
        switch( messageId )
            {
            case UICC_CAT_RESP:
                {
                // In case of envelope response handle the data
                TUint8 serviceType(
                    aIsiMessage.Get8bit(
                        ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_SERVICETYPE ) );
                if ( UICC_CAT_ENVELOPE == serviceType )
                    {
                    if ( iSmsPpDdOngoing &&
                         iSmsPpTransactionId ==
                         aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) )
                        {
                        // Set flag iSmsPpDdOngoing false
                        // since a response is received for the envelope
                        // that was sent for a received sms-pp message
                        iSmsPpDdOngoing = EFalse;
                        // Process received message
                        UiccCatRespEnvelopeReceived( aIsiMessage );
                        }
                    }
                break;
                }
            default:
                {
                // none
                break;
                }
            }
        }

    else
        {
        // None
        }
    }

// End of File
