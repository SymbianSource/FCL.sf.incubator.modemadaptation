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
#include "satmesshandler.h"     // sat message handler class
#include "satmessaging.h"       // sat messaging class
#include "sattimer.h"           // sat timer class
#include "sateventdownload.h"   // sat event download class
#include "satdatadownload.h"    // sat data download class
#include "saticon.h"            // sat icon class
#include "satmosmsctrl.h"       // sat mo sms control class
#include "satnotifications.h"   // sat notifications class
#include "ber_tlv.h"            // sat ber-tlv classes
#include "satutil.h"            // sat utility class
#include "cmmphonetsender.h"    // phonet sender
#include "terminalprofile.h"    // terminal profile
#include "satflightmodestatus.h"// flight mode status

#include <pn_const.h>           // server id constants
#include <tisi.h>               // isi message
#include <ss_wmisi.h>           // ss server
#include <infoisi.h>            // info server
#include <net_modemisi.h>       // net server
#include <gssisi.h>             // gss server
#include <call_modemisi.h>      // Call Modem server
#include <smsisi.h>             // sms server
#include <uiccisi.h>            // UICC server
#include <gpdsisi.h>            // GPDS server

#include <atk_sharedisi.h>
#include <call_sharedisi.h>
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satmesshandlertraces.h"
#endif



// CONSTANTS
const TUint8 KOneByteLengthLimit                = 0x7F;
// Maximum value for get input string
const TUint8 KGetInputStringMaxSize             = 0xEF;
// No transaction ongoing
const TInt KNoTransactionOngoing                = -1;
// Time zone not available
const TUint8 KTimeZoneNotAvailable              = 0xFF;
// Access Technology unknown
const TUint8 KAccTechUnknown                    = 0xFF;
// This is a common length constant (2 bytes)
const TUint8 KLengthTwoBytes                    = 0x02;
// Maximum polling interval of legacy phones
const TUint8 KMaxLegacyPollInterval             = 0x19; // 25 decimal
// Size of one clut entry in bytes
const TUint16 KClutEntrySize                    = 3;

// Padding bytes for UICC messages
const TUint8 KUiccPadding                       = 0x00;
// Size of terminal profile subblock
const TUint8 KSizeOfTerminalProfileSb           = 40;
// Offset of UICC_APPL_CMD_REQ message for adding subblocks
const TUint8 KUiccApplCmdReqOffset(
    ISI_HEADER_SIZE + SIZE_UICC_APPL_CMD_REQ );
// Size of UICC_SB_APPL_PATH
const TUint8 KUiccSbApplPathSize(
    SIZE_UICC_SB_APPL_PATH + KUiccFilePathLength );
// UICC file IDs
const TUint16 KUiccMasterFileId( 0x3F00 );
const TUint16 KUiccElemFileServiceTable( 0x6F38 );
const TUint16 KUiccDedicatedFileTelecom( 0x7F10 );
const TUint16 KUiccDedicatedFileGraphics( 0x5F50 );
const TUint16 KUiccElemFileImg( 0x4F20 );
const TUint8 KUiccSfiServiceTable( 4 );
// Number of subblocks in UICC_APPL_CMD_REQ
const TUint8 KNumOfSbsInUiccApplCmdReq( 3 );
// Max size of UICC_SB_TERMINAL_PROFILE
const TUint16 KMaxSizeOfUiccSbTerminalResponse( 264 );
// Length of device identity TLV data object
const TUint8 KTlvDeviceIdentityLength( 2 );
// Max length of UICC_SB_AID
const TUint16 KMaxLengthUiccSbAid( 260 );
// Elementary file ID length
const TUint8 KUiccFileIdLength( 2 );
// Maximum size of envelope
const TUint16  KMaxSizeOfEnvelope( 256 );


// ==================== MEMBER FUNCTIONS ====================================

// -----------------------------------------------------------------------------
// CSatMessHandler::CSatMessHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatMessHandler::CSatMessHandler
        (
        CTsySatMessaging*   aTsySatMessaging,
        CMmPhoNetSender*    aPnSend
        )
        :
        iTsySatMessaging( aTsySatMessaging ),
        iPnSend( aPnSend )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_CSATMESSHANDLER, "CSatMessHandler::CSatMessHandler" );
    // >= 0 if there is ongoing transaction
    iTerminalRespTraId              = KNoTransactionOngoing;
    iGetIconSimReadFieldTraId       = KNoTransactionOngoing;
    iDataDownloadSimReadFieldTraId  = KNoTransactionOngoing;

    iCardId                         = KZero;
    iSerialNumberReadReceived       = EFalse;
    // By default, there's no need to request routing from SMS server
    iSmsCbRoutingComplete           = ETrue;
    iRefreshRequestIsOn             = EFalse;
    iTimingAdvance                  = GSS_TIMING_ADVANCE_NOT_AVAILABLE;
    iTaStatus                       = GSS_TIMING_ADVANCE_NOT_VALID;
    iTimeZone                       = KTimeZoneNotAvailable;
    iSimServerObjectId              = KZero;
    iCurrentAccTech                 = KAccTechUnknown;
    // Set polling status to the use of the default value (25 sec.)
    iPollingOff                     = ETrue;
    iSsStatus                       = ENotBusy;
    // No long poll interval requests yet
    iLongPollIntervalReq            = EFalse;

#ifdef INFO_PP_ATK_MO_SM_CONTROL
    // MO SMS support not yet read from product profile
    iMoSmsSupportedInPp             = EFalse;
#endif // INFO_PP_ATK_MO_SM_CONTROL

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
    // ENS support not yet read from product profile
    iEnsSupportedInPp               = EFalse;
    // SET UP EVENT LIST TR not yet sent
    iSetUpEventListTrSent           = EFalse;
#endif // INFO_PP_ENHANCED_NETWORK_SELECTION

    // Init location info structure
    iLocInfo.iOperatorCode.FillZ( iLocInfo.iOperatorCode.MaxLength() );
    iLocInfo.iLac.FillZ( iLocInfo.iLac.MaxLength() );
    iLocInfo.iCellId.FillZ( iLocInfo.iCellId.MaxLength() );
    iLocInfo.iRegStatus = NET_NO_COVERAGE;

    // Init IMEI of the ME
    iIMEI.FillZ( 16 ); // KSerialNumberLength + 1

    // Clear NMR (Network Measurement result)
    iNMR.Zero();

    // Clear BCCH channel list
    iBCCHChannelList.Zero();

    // Clear UTRAN measurement result
    iUtranNMR.Set( KNullDesC8 );

    // ME is not yet in service during startup
    iPreviousRegistrationStatus = NET_REG_STATUS_NOSERV_SEARCHING;

    // INFO_PP_SIM_OLD_POLL_INTERVAL flag not read yet
    iOldPollIntervalSupportedInPp = ETrue;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::~CSatMessHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CSatMessHandler::~CSatMessHandler()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_CSATMESSHANDLER, "CSatMessHandler::~CSatMessHandler" );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatMessHandler* CSatMessHandler::NewL
        (
        CTsySatMessaging*   aTsySatMessaging,
        CMmPhoNetSender*    aPnSend
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NEWL, "CSatMessHandler::NewL" );
    TFLOGSTRING("TSY:CSatMessHandler::NewL");

    CSatMessHandler* self = new( ELeave ) CSatMessHandler( aTsySatMessaging,
        aPnSend );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::ConstructL
// Symbian 2nd phase constructor can leave
// -----------------------------------------------------------------------------
//
void CSatMessHandler::ConstructL()
    {
    TFLOGSTRING("TSY:CSatMessHandler::ConstructL");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_CONSTRUCTL, "CSatMessHandler::ConstructL" );

    // This flag is set to 'true' when indication ESatUsatClientReadyIndication
    // is received from SAT server. CAT can be enabled when the state of this
    // flag is 'true' and also UICC server is ready.
    iSatReady = EFalse;
    // Application ID will be set when UICC_APPLICATION_IND is received
    iApplicationId = 0xFF;
    // UICC client ID will be set when UICC_APPLICATION_RESP is received
    iUiccClientId = 0;
    // Terminal profile status flag is set 'true' when terminal profile has been
    // sent to prevent CAT enabling request to be sent before terminal profile
    iTerminalProfileSent = EFalse;
    // Card type is not known yet
    iCardType = UICC_CARD_TYPE_UNKNOWN;
    // Application file id is initialize to 'FFFF'
    TBuf8<2> unknownFileId;
    unknownFileId.Append( 0xFF );
    unknownFileId.Append( 0xFF );
    iApplFileId.Copy( unknownFileId );

    iPollingInterval = KDefaultPollInterval;
    iImageCodingScheme = KBlackAndWhite;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::ProactiveProcedureMessageReceivedL
// Called SatMessHandler::ReceivedL, handles Event download,
// Call control, Data Download and MO-SMS Control related messages
// -----------------------------------------------------------------------------
//
void CSatMessHandler::ProactiveProcedureMessageReceivedL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_PROACTIVEPROCEDUREMESSAGERECEIVEDL, "CSatMessHandler::ProactiveProcedureMessageReceivedL" );
    TFLOGSTRING("TSY:CSatMessHandler::ProactiveProcedureMessageReceivedL");

    // handle event download and related messages
    iTsySatMessaging->EventDownloadReceived( aIsiMessage );

    // handle call control and releated messages
    iTsySatMessaging->CallControlReceivedL( aIsiMessage );

    // handle Data Download and related messages
    iTsySatMessaging->DataDownloadReceivedL( aIsiMessage );

    // handle MO-SMS Control related messages
    iTsySatMessaging->MoSmsControlReceived( aIsiMessage );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NetServerMessageReceived
// Called from SatMessHandler::ReceivedL, handles Net Server related messages
// -----------------------------------------------------------------------------
//
void CSatMessHandler::NetServerMessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETSERVERMESSAGERECEIVED, "CSatMessHandler::NetServerMessageReceived" );
    TFLOGSTRING("TSY:CSatMessHandler::NetServerMessageReceived");

    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    switch ( messageId )
        {
        case NET_CELL_INFO_IND:
            {
            StoreNetServiceStatus( aIsiMessage );
            // If SMS CB routing is still pending, regenerate routing request.
            // This might be the case if CS was inactive or SIM is not ready
            // during the first request.
            if ( !iSmsCbRoutingComplete )
                {
                TFLOGSTRING("TSY:CSatMessHandler::NetServerMessageReceived \
                    Sending delayed SMS CB routing request.");
                OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_NETSERVERMESSAGERECEIVED, "CSatMessHandler::NetServerMessageReceived Sending delayed SMS CB routing request." );
                SmsCbRoutingReq(
                    iTsySatMessaging->GetTransactionId(), SMS_ROUTING_SET );
                }
            break;
            }
        case NET_NEIGHBOUR_CELLS_RESP:
            {
            NetNeighbourCellResp( aIsiMessage );
            break;
            }
        case NET_CELL_INFO_GET_RESP:
            {
            NetCellInfoGetResp( aIsiMessage );
            break;
            }
        case NET_TIME_IND:
            {
            NetTimeInd( aIsiMessage );
            break;
            }
        case NET_RAT_RESP:
            {
            NetRatResp( aIsiMessage );
            break;
            }
        case NET_RAT_IND:
            {
            NetRatInd( aIsiMessage );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::PhoneInfoMessageReceived
// Called from SatMessHandler::ReceivedL, handles Phone Info related messages
// -----------------------------------------------------------------------------
//
void CSatMessHandler::PhoneInfoMessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_PHONEINFOMESSAGERECEIVED, "CSatMessHandler::PhoneInfoMessageReceived" );
    TFLOGSTRING("TSY:CSatMessHandler::PhoneInfoMessageReceived");

    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    switch ( messageId )
        {
        case INFO_SERIAL_NUMBER_READ_RESP:
            {
            InfoSerialNumberReadResp( aIsiMessage );
            break;
            }
        case INFO_PP_READ_RESP:
            {
            InfoPpReadResp( aIsiMessage );
            break;
            }
        default:
            {
            // do nothing
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccServerMessageReceived
// Called from SatMessHandler::ReceivedL, handles UICC Server messages
// -----------------------------------------------------------------------------
//
TBool CSatMessHandler::UiccServerMessageReceived(
    const TIsiReceiveC& aIsiMessage )
    {
    TUint8 messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );
    OstTraceExt1( TRACE_NORMAL, DUP3_CSATMESSHANDLER_UICCSERVERMESSAGERECEIVED, "CSatMessHandler::UiccServerMessageReceived;messageId=%hhu", messageId );
    TFLOGSTRING2("TSY:CSatMessHandler::UiccServerMessageReceived, message ID: %d", messageId );

    TBool handled( EFalse );

    switch ( messageId )
        {
        case UICC_CAT_IND:
            {
            UiccCatInd( aIsiMessage );
            break;
            }
        case UICC_CAT_RESP:
            {
            TUint8 serviceType(
                aIsiMessage.Get8bit(
                    ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_SERVICETYPE ) );
            TUint8 status(
                aIsiMessage.Get8bit(
                    ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_STATUS ) );

            TFLOGSTRING3("TSY:CSatMessHandler:: KUiccCatResp: service type %d, status %d", serviceType, status);
            OstTraceExt2( TRACE_NORMAL, DUP2_CSATMESSHANDLER_UICCSERVERMESSAGERECEIVED, "CSatMessHandler::KUiccCatResp;serviceType=%hhu;status=%hhu", serviceType, status );

            // If terminal profile was sent succesfully and SAT is ready,
            // start fetching proactive commands
            if ( UICC_CAT_TERMINAL_PROFILE == serviceType &&
                UICC_STATUS_OK == status )
                {
                // If SAT is not ready, CAT enable request cannot be sent yet.
                // When indication of SAT ready is received by satmessaging,
                // CAT enable request is sent if this flag is set to 'true'.
                iTerminalProfileSent = ETrue;

                // Enable or disable proactive command fetching depending on
                // flight mode status
                if ( iSatReady )
                    {
                    // Get instance of CSatFlightModeStatus for flight mode
                    // status
                    CSatFlightModeStatus* satFlightModeStatus(
                        iTsySatMessaging->GetSatFlightModeStatus() );
                    if ( ! satFlightModeStatus->FlightModeStatusValue() )
                        {
                        UiccCatReq( UICC_CAT_ENABLE );
                        }
                    else
                        {
                        UiccCatReq( UICC_CAT_DISABLE );
                        }
                    }
                }
            // Response for refresh command
            else if ( UICC_CAT_REFRESH == serviceType )
                {
                RefreshResult( status );
                }
            // Response for polling set command
            else if ( UICC_CAT_POLLING_SET == serviceType  )
                {
                SetPollingResult(
                    status,
                    aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ),
                    iPollingInterval );
                }
            // Response for envelope
            else if ( UICC_CAT_ENVELOPE == serviceType )
                {
                handled = UiccCatRespEnvelope( aIsiMessage );
                }
            // Response for terminal response
            else if ( UICC_CAT_TERMINAL_RESPONSE  == serviceType )
                {
                handled = UiccCatRespTerminalResponse( aIsiMessage );
                }
            break;
            }
        case UICC_APPLICATION_RESP:
            {
            TUint uiccSbClientOffset( 0 );
            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_UICC_APPLICATION_RESP,
                UICC_SB_CLIENT,
                EIsiSubBlockTypeId16Len16,
                uiccSbClientOffset ) )
                {
                iUiccClientId = aIsiMessage.Get8bit(
                    uiccSbClientOffset + UICC_SB_CLIENT_OFFSET_CLIENTID );
                }
            break;
            }
        case UICC_APPLICATION_IND:
            {
            // Application ID is needed when reading data from UICC
            iApplicationId = aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + UICC_APPLICATION_IND_OFFSET_APPLID );
            // Access to UICC files is possible when application is active
            TUint8 serviceType( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + UICC_APPLICATION_IND_OFFSET_SERVICETYPE ) );
            if ( UICC_APPL_ACTIVATED == serviceType )
                {
                // USIM and SIM: Read SMS-PP DD and Call Control from byte 4.
                // Only USIM: MO-SMS control from byte 4
                UiccReadServiceTableReq( KUiccTrIdServiceTableByte4, 3 );

                if ( UICC_CARD_TYPE_UICC == iCardType )
                    {
                    // USIM: Read call control GPRS from byte 7
                    UiccReadServiceTableReq( KUiccTrIdServiceTableByte7, 6 );
                    }
                else if ( UICC_CARD_TYPE_ICC == iCardType )
                    {
                    // SIM: Read MO-SMS control from byte 5
                    UiccReadServiceTableReq( KUiccTrIdServiceTableByte5, 4 );
                    }
                }
            break;
            }
        case UICC_APPL_CMD_RESP:
            {
            UiccApplCmdResp( aIsiMessage );
            }
        } // End of switch ( messageId )
    return handled;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SmsServerMessageReceived
// Called from SatMessHandler::ReceivedL, handles SMS Server messages
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SmsServerMessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message from SMS server
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SMSSERVERMESSAGERECEIVED, "CSatMessHandler::SmsServerMessageReceived" );
    TFLOGSTRING("TSY:CSatMessHandler::SmsServerMessageReceived");

    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    switch ( messageId )
        {
        case SMS_CB_ROUTING_RESP:
            {
            SmsCbRoutingResp( aIsiMessage );
            break;
            }
        case SMS_RESOURCE_CONF_IND:
            {
            SmsResourceConfInd( aIsiMessage );
            break;
            }
        case SMS_RESOURCE_CONF_RESP:
            {
            // Take a log because result is always ok
            TFLOGSTRING("TSY:CSatMoSmsCtrl::SmsResourceConfRespReceived: Response for Resource confoguration Req, Receive response from SMS server");
            OstTrace0( TRACE_NORMAL, DUP1_CSATMOSMSCTRL_SMSRESOURCECONFRESPRECEIVED, "CSatMoSmsCtrl::SmsResourceRespReceived: Response for Resource Configuration Req, Receive response from SMS server" );
            break;
            }
        default:
            // do nothing
            break;
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::SsServerMessageReceived
// Called from SatMessHandler::ReceivedL, handles SS Server messages
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SsServerMessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SSSERVERMESSAGERECEIVED, "CSatMessHandler::SsServerMessageReceived" );
    TFLOGSTRING("TSY:CSatMessHandler::SsServerMessageReceived");

    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    switch ( messageId )
        {
        case SS_STATUS_IND:
            {
            SsStatusInd( aIsiMessage );
            break;
            }
         default:
            // do nothing
            break;
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::GsmStackServerMessageReceived
// Called from SatMessHandler::ReceivedL, handles GSM Stack Server messages
// -----------------------------------------------------------------------------
//
void CSatMessHandler::GsmStackServerMessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GSMSTACKSERVERMESSAGERECEIVED, "CSatMessHandler::GsmStackServerMessageReceived" );
    TFLOGSTRING("TSY:CSatMessHandler::GsmStackServerMessageReceived");

    switch ( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
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
        default:
            {
            // do nothing
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::DisplayTextTerminalResp
// Constructs DisplayText specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::DisplayTextTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // PCmd number
        TUint8 aGeneralResult,      // General Result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_DISPLAYTEXTTERMINALRESP, "CSatMessHandler::DisplayTextTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::DisplayTextTerminalResp");

    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    // General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( KMeCurrentlyUnableToProcessCommand == aGeneralResult )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::GetInkeyTerminalResp
// Constructs GetInkey specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::GetInkeyTerminalResp
        (
        const TUint8 aTransId,            // Transaction id
        const TDes8& aCommandDetails,     // PCmd number
        const RSat::TGetInkeyRspV2& aRsp  // Response packet from Etel
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GETINKEYTERMINALRESP, "CSatMessHandler::GetInkeyTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::GetInkeyTerminalRespL");

    TTlv tlvSpecificData;

    //General result
    TUint8 result( aRsp.iGeneralResult );

    tlvSpecificData.AddTag( KTlvResultTag );
    tlvSpecificData.AddByte( result );

    if ( CommandPerformedSuccessfully( result ) )
        {
        TUint8 dcs( K8BitDCS ); // Default
        TBuf8<2> character; // Unicode has 2 bytes

        // Text String Tlv
        if ( NULL != aRsp.iAdditionalInfo.Length() )
            {
            // Response contain on one character
            TPtrC16 unicodeChar( aRsp.iAdditionalInfo.Mid( 0, 1 ) );
            switch ( aRsp.iRspFormat )
                {
                case RSat::ECharUcs2Alphabet:
                    {
                    dcs = KUCS2DCS;
                    TIsiUtility::CopyToBigEndian( unicodeChar, character );
                    break;
                    }
                case RSat::EDigitOnly:
                case RSat::ECharSmsDefaultAlphabet:
                case RSat::EYesNo:
                default:
                    {
                    // Dcs already set to K8BitDCS
                    TSatUtility::ConvertUnicode16ToSms7(
                        character, unicodeChar );
                    break;
                    }
                }
            }
        else
            {
            // Just in case
            dcs = K8BitDCS;
            character.Copy( NULL );
            }

        tlvSpecificData.AddTag( KTlvTextStringTag );
        tlvSpecificData.AddByte( dcs );
        tlvSpecificData.AddData( character );
        }
    else if ( RSat::KNoAdditionalInfo != aRsp.iInfoType )
        {
        // Command not successfull
        if ( NULL != aRsp.iAdditionalInfo.Length() )
            {
            tlvSpecificData.AddByte( TUint8 ( aRsp.iAdditionalInfo[0] ) );
            }
        }
    else
        {
        // Command was not successful but additional info was not provided
        }

    // Append Duration TLV, if provided
    TInt tlvTimeUnit( KErrNotFound );
    switch ( aRsp.iDuration.iTimeUnit )
        {
        case RSat::EMinutes:
            {
            tlvTimeUnit = KMinutes;
            break;
            }
        case RSat::ESeconds:
            {
            tlvTimeUnit = KSeconds;
            break;
            }
        case RSat::ETenthsOfSeconds:
            {
            tlvTimeUnit = KTenthsOfSeconds;
            break;
            }
        case RSat::ETimeUnitNotSet:
        case RSat::ENoDurationAvailable:
        default:
            {
            // Duration is not added to terminal response
            break;
            }
        }
    if ( KErrNotFound != tlvTimeUnit )
        {
        // Duration provided. Append it to terminal response.
        tlvSpecificData.AddTag( KTlvDurationTag );
        tlvSpecificData.AddByte( tlvTimeUnit );
        tlvSpecificData.AddByte( aRsp.iDuration.iNumOfUnits );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );

    }

// -----------------------------------------------------------------------------
// CSatMessHandler::GetInputTerminalResp
// Constructs Get Input specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::GetInputTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Command details tlv
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo,     // Additional info
        TDesC16& aTextString,       // Response string
        TUint8 aDataCodingScheme    // Coding scheme
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GETINPUTTERMINALRESP, "CSatMessHandler::GetInputTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::GetInputTerminalResp");

    TTlv tlvSpecificData;
    TBuf8<KGetInputStringMaxSize> string;

    tlvSpecificData.AddTag( KTlvResultTag );
    tlvSpecificData.AddByte( aGeneralResult );
    if ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    if ( CommandPerformedSuccessfully( aGeneralResult ) )
        {
        // create TextString TLV
        tlvSpecificData.AddTag( KTlvTextStringTag );

        if ( aTextString.Length() )
            {
            switch ( aDataCodingScheme )
                {
                case RSat::EUcs2Alphabet:
                    {
                     // data coding scheme UCS2 (16-bit)
                    tlvSpecificData.AddByte( KUCS2DCS );

                    if ( aTextString.Length() )
                        {
                        TIsiUtility::CopyToBigEndian( aTextString, string );
                        tlvSpecificData.AddData( string );
                        }
                    break;
                    }

                case RSat::EDigitOnlyPacked:
                case RSat::ESmsDefaultAlphabetPacked:
                    {
                    // data coding scheme 7-bit default sms
                    tlvSpecificData.AddByte( K7BitDefaultSmsDCS );
                    TSatUtility::UCSToPacket7( aTextString, string );
                    tlvSpecificData.AddData( string );
                    break;
                    }

                case RSat::EDigitOnlyUnpacked:
                case RSat::ESmsDefaultAlphabetUnpacked:
                    {
                    // data coding scheme 8-bit
                    tlvSpecificData.AddByte( K8BitDCS );
                    TSatUtility::ConvertUnicode16ToSms7( string, aTextString );
                    tlvSpecificData.AddData( string );
                    break;
                    }
                default:
                    // do nothing
                    break;
                }
            }
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::PlayToneTerminateResponse
// Constructs PlayTone specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::PlayToneTerminalResp
        (
        TUint8 aTransId,            // Transaction Id
        TDes8& aCommandDetails,     // Command details
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_PLAYTONETERMINALRESP, "CSatMessHandler::PlayToneTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::PlayToneTerminalResp");

    TTlv tlvSpecificData;

    tlvSpecificData.AddTag( KTlvResultTag );
    //General result
    tlvSpecificData.AddByte( aGeneralResult );
    if ( AdditionalInfoNeeded( aGeneralResult ) )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::SetUpMenuTerminalResp
// Constructs SetUpMenu specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SetUpMenuTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // aPCmdNumber
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SETUPMENUTERMINALRESP, "CSatMessHandler::SetUpMenuTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SetUpMenuTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;

    //create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    //General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SelectItemTerminalResp
// Constructs SelectItem specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SelectItemTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Comamnd number
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SELECTITEMTERMINALRESP, "CSatMessHandler::SelectItemTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SelectItemTerminalResp");

    TTlv tlvSpecificData;
    // Create general result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( !( CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            case RSat::KHelpRequestedByUser:
                {
                // add selected Item Id tag
                tlvSpecificData.AddTag( KTlvItemIdentifierTag );
                tlvSpecificData.AddByte( aAdditionalInfo ); // selected item
                break;
                }
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
                {
                break;
                }
            default:
                {
                tlvSpecificData.AddByte( aAdditionalInfo );
                break;
                }
            }
        }
    else
        {
        // add selected Item Id tag
        tlvSpecificData.AddTag( KTlvItemIdentifierTag );
        tlvSpecificData.AddByte( aAdditionalInfo ); // selected item
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::PollIntervalTerminateResp
// Constructs PollInterval specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::PollIntervalTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // command number
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo,     // Additional info
        TUint8 aNumOfUnits          // Time interval
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_POLLINTERVALTERMINALRESP, "CSatMessHandler::PollIntervalTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::PollIntervalTerminalResp");

    TTlv tlvSpecificData;

    tlvSpecificData.AddTag( KTlvResultTag );
    tlvSpecificData.AddByte( aGeneralResult );

    if ( !( CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            // Cases in which additional info is not required
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
            case RSat::KErrorRequiredValuesMissing:
            case RSat::KHelpRequestedByUser:
                {
                break;
                }
            default:
                {
                tlvSpecificData.AddByte( aAdditionalInfo );
                break;
                }
            }
        }
    else
        {
        tlvSpecificData.AddTag( KTlvDurationTag );
        tlvSpecificData.AddByte( KSeconds );
        tlvSpecificData.AddByte( aNumOfUnits );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
}

// -----------------------------------------------------------------------------
// CSatMessHandler::SendSmTerminalResp
// Constructs SendSm specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SendSmTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Command details
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDSMTERMINALRESP, "CSatMessHandler::SendSmTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SendSmTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;

    //create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    //General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KMeUnableToProcessCmd == aGeneralResult ||
        RSat::KNetworkUnableToProcessCmd == aGeneralResult ||
        RSat::KSmsRpError == aGeneralResult ||
        RSat::KInteractionWithCCPermanentError == aGeneralResult )
        {
        // if there is ME, Network or SMS error, append additional info
        tlvSpecificData.AddByte( aAdditionalInfo );
        }
    // Send terminal response
    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SendSsTerminalResp
// Constructs SendSS specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SendSsTerminalResp
        (
        TUint8  aTransId,           // Transaction id
        TDes8&  aCommandDetails,    // Command details tlv
        TUint8  aGeneralResult,     // General result
        TDesC8& aAdditionalInfo     // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDSSTERMINALRESP, "CSatMessHandler::SendSsTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SendSsTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KErrorRequiredValuesMissing != aGeneralResult &&
        RSat::KCmdDataNotUnderstood != aGeneralResult )
        {
        tlvSpecificData.AddData( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SendDtmfTerminalResp
// Constructs Send Dtmf specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SendDtmfTerminalResp
        (
        TUint8 aTransId,
        TDes8& aCommandDetails,
        TUint8  aGeneralResult,     // General result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDDTMFTERMINALRESP, "CSatMessHandler::SendDtmfTerminalResp" );
    TFLOGSTRING("CSatMessHandler::SendDtmfTerminalResp");

    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        {
        // UnSuccessful result neccessitating additional info byte
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SendUssdTerminalResponse
// Constructs and sends the Send USSD response to Phonet, in order
// to reach the CellMo side.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SendUssdTerminalResp
        (
        TUint8 aTransId,
        TDes8& aCommandDetails,
        TUint8  aGeneralResult,
        TUint8 aAdditionalInfo,
        TDes& aUssdData,
        TUint8  aUssdCbsDataCodingScheme
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDUSSDTERMINALRESP, "CSatMessHandler::SendUssdTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SendUssdTerminalResp");

    TTlv tlvSpecificData;
    TBuf8<RSat::KStringMaxSize> string;

    tlvSpecificData.AddTag( KTlvResultTag );
    tlvSpecificData.AddByte( aGeneralResult );

    // For the general results '20', '21', '37', '38'
    // it is mandatory for the ME to provide a specific
    // cause value as additional
    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult )
        || ( RSat::KUssdReturnError == aGeneralResult)
        || ( RSat::KMultipleCardCmdsError == aGeneralResult) )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }
    // For general result '32' additional info or text string is not used
    else if ( RSat::KCmdDataNotUnderstood != aGeneralResult )
        {
        // Append received Ussd text string, with a Text string tag
        tlvSpecificData.AddTag( KTlvTextStringTag );

        // Decode DCS
        // Remains to know if SatServer sets the general result to
        // KUssdReturnError when the DCS has a reserved value.
        TSmsDcs decodedDcs( ESmsUnknownOrReservedDcs );
        decodedDcs = TSatUtility::DecodeCbsDcs( aUssdCbsDataCodingScheme );
        //Data Coding Scheme for Text String
        tlvSpecificData.AddByte( (TUint8)decodedDcs );
        switch ( decodedDcs )
            {
            case ESms7BitDcs:
                {
                // Convert the Response string (which comes in the form of
                // a TBuf (unicode)) to 7-bit gsm format.
                TSatUtility::UCSToPacket7( aUssdData, string );
                break;
                }
            case ESms8BitDcs:
                {
                // Convert the Response string (which comes in the form of
                // a TBuf (unicode)) to 7-bit gsm format.
                TSatUtility::ConvertUnicode16ToSms7( string, aUssdData );
                break;
                }
            case ESms16BitDcs:
                {
                TIsiUtility::CopyToBigEndian( aUssdData, string );
                break;
                }
            default:
                {
                // Reserved
                // The general result should in fact prevent reaching this
                // branch of the switch.
                TFLOGSTRING("TSY:CSatMessHandler::SendUssdTerminalResp, \
                The DCS sent by the network has a reserved value.");
                OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_SENDUSSDTERMINALRESP, "CSatMessHandler::SendUssdTerminalResp, The DCS sent by the network has a reserved value." );
                break;
                }
            }
        // Text
        tlvSpecificData.AddData( string );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SetUpCallTerminalResp
// Constructs SetUpCall specific part of terminal
// response and calls SendTerminalResponse to send the terminal response
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SetUpCallTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Command Details tlv
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SETUPCALLTERMINALRESP, "CSatMessHandler::SetUpCallTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SetUpCallTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );
    // Result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KMeUnableToProcessCmd == aGeneralResult ||
        RSat::KNetworkUnableToProcessCmd == aGeneralResult ||
        RSat::KInteractionWithCCPermanentError == aGeneralResult )
        {
        // If there was ME or network error, add additional info
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::RefreshTerminalResp
// Constructs Refresh specific part of terminal
// response and calls SendTerminalResponse to send the terminal response
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::RefreshTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Command number
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_REFRESHTERMINALRESP, "CSatMessHandler::RefreshTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::RefreshTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;
    // append result tag
    tlvSpecificData.AddTag( KTlvResultTag );

    // General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::MenuSelectionInd
// Constructs and sends the Menu Selection response to phonet
// -----------------------------------------------------------------------------
//
void CSatMessHandler::MenuSelectionInd
        (
        TUint8 aTransId,        // Transaction id
        TUint8 aItemIdentifier, // Item identifier
        TUint8 aHelp            // Help requested status
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_MENUSELECTIONIND, "CSatMessHandler::MenuSelectionInd" );
    TFLOGSTRING("TSY:CSatMessHandler::MenuSelectionInd");

    TTlv tlvSpecificData;

    tlvSpecificData.Begin( KBerTlvMenuSelectionTag );
    tlvSpecificData.AddTag( KTlvDeviceIdentityTag );
    tlvSpecificData.AddByte( KKeypad );
    tlvSpecificData.AddByte( KSim );
    tlvSpecificData.AddTag( KTlvItemIdentifierTag );
    tlvSpecificData.AddByte( aItemIdentifier );
    if ( aHelp )
        {
        tlvSpecificData.AddTag( KTlvHelpRequestTag );
        }

    // Save menu selection envelope's transaction id, because SIM session
    // has to be completed if menu selection does not have successive commands.
    iTerminalRespTraId = TInt( aTransId );
    UiccCatReqEnvelope( aTransId, tlvSpecificData.End(), EFalse );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::LaunchBrowserTerminalResp
// Constructs and sends the Launch Browser response to phonet
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::LaunchBrowserTerminalResp
        (
        TUint8 aTransId,        // aTransId
        TDes8& aCommandDetails, // command number
        TUint8 aGeneralResult,  // Result
        TUint8 aAdditionalInfo  // additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_LAUNCHBROWSERTERMINALRESP, "CSatMessHandler::LaunchBrowserTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::LaunchBrowserTerminalResp");

    TTlv tlvSpecificData;

    tlvSpecificData.AddTag( KTlvResultTag );
   //create General Result TLV here
    tlvSpecificData.AddByte( aGeneralResult );
    if ( !( CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        if ( RSat::KCmdTypeNotUnderstood != aGeneralResult
            && RSat::KCmdDataNotUnderstood != aGeneralResult
            && RSat::KErrorRequiredValuesMissing != aGeneralResult )
            {
            // additional info
            tlvSpecificData.AddByte( aAdditionalInfo );
            }
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::CommandPerformedSuccessfully
// Test if a proactive command was performed successfully
// -----------------------------------------------------------------------------
//
TBool CSatMessHandler::CommandPerformedSuccessfully
        (
        TUint8 aGeneralResult
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_COMMANDPERFORMEDSUCCESSFULLY, "CSatMessHandler::CommandPerformedSuccessfully" );
    TFLOGSTRING("TSY:CSatMessHandler::CommandPerformedSuccessfully");

    TBool ret( EFalse );
    switch ( aGeneralResult )
        {
        case RSat::KSuccess:
        case RSat::KPartialComprehension:
        case RSat::KMissingInformation:
        case RSat::KRefreshAdditionEFRead:
        case RSat::KSuccessRequestedIconNotDisplayed:
        case RSat::KModifiedByCallControl:
        case RSat::KSuccessLimitedService:
        case RSat::KPerformedWithModifications:
        case RSat::KRefreshUSIMNotActive:
            {
            ret = ETrue;
            break;
            }
        default:
            {
            ret = EFalse;
            break;
            }
        }
    return ret;
   }


// -----------------------------------------------------------------------------
// CSatMessHandler::SetUpIdleModeTextTerminalResp
// Constructs SetUpIdleModeText specific part of terminal
// response and calls SendTerminalResponse to send the terminal response
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SetUpIdleModeTextTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // command number
        TUint8 aGeneralResult,      // Result
        TUint8 aAdditionalInfo      // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SETUPIDLEMODETEXTTERMINALRESP, "CSatMessHandler::SetUpIdleModeTextTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SetUpIdleModeTextTerminalResp");

    TTlv tlvSpecificData;

    //create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    //General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( !( CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            // Cases in which additional info is not required
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
            case RSat::KErrorRequiredValuesMissing:
            case RSat::KHelpRequestedByUser:
            case RSat::KCmdDataNotUnderstood:
                {
                break;
                }
            default:
                {
                tlvSpecificData.AddByte( aAdditionalInfo );
                break;
                }
            }
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
}


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccCatReqEnvelope
// Create and send UICC_CAT_REQ with UICC_SB_ENVELOPE
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::UiccCatReqEnvelope
        (
        TUint8 aTransId,
        const TDesC8& aBerTlv,  // BER-TLV specific data
        TBool aStore            // Is envelope stored for resending
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCCATREQENVELOPE, "CSatMessHandler::UiccCatReqEnvelope" );
    TFLOGSTRING("TSY:CSatMessHandler::UiccCatReqEnvelope");

    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_TRANSID,
        aTransId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_MESSAGEID,
        UICC_CAT_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_SERVICETYPE,
        UICC_CAT_ENVELOPE );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_NSB,
        1 );

    // Add UICC_SB_ENVELOPE
    TBuf8<SIZE_UICC_SB_ENVELOPE + KMaxSizeOfEnvelope> uiccSbEnvelopeBuf( 0 );
    TIsiSubBlock uiccSbEnvelope(
        uiccSbEnvelopeBuf,
        UICC_SB_ENVELOPE,
        EIsiSubBlockTypeId16Len16 );

    // 2x filler
    uiccSbEnvelopeBuf.Append( KUiccPadding );
    uiccSbEnvelopeBuf.Append( KUiccPadding );

    uiccSbEnvelopeBuf.Append( aBerTlv.Length() >> 8 ); // Length of envelope
    uiccSbEnvelopeBuf.Append( aBerTlv.Length() );
    uiccSbEnvelopeBuf.Append( aBerTlv ); // Envelope

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_REQ,
        uiccSbEnvelope.CompleteSubBlock() );

    // Store information if needed for possible resending
    if ( aStore )
        {
        iTsySatMessaging->GetSatTimer()->StoreEnvelope( aTransId, aBerTlv );
        }

    return( iPnSend->Send( isiMsg.Complete() ) );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::StoreNetServiceStatus
// Stores location information and network service status
// -----------------------------------------------------------------------------
//
void CSatMessHandler::StoreNetServiceStatus
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_STORENETSERVICESTATUS, "CSatMessHandler::StoreNetServiceStatus" );
    TFLOGSTRING("TSY: CSatMessHandler::StoreNetServiceStatus");

    // SubBlock offset
    TUint sbOffset( 0 );

    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    TInt headerSize( KErrNotFound );

    if ( NET_CELL_INFO_IND == messageId )
        {
        headerSize = SIZE_NET_CELL_INFO_IND;
        }
    else if ( NET_CELL_INFO_GET_RESP == messageId )
        {
        headerSize = SIZE_NET_CELL_INFO_GET_RESP;
        }
    else
        {
        // Header size contains already an error value as default.
        }

    if ( KErrNotFound != headerSize )
        {
        // Find NET_GSM_CELL_INFO or NET_WCDMA_CELL_INFO subblock
        TInt retGsmSb( aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + headerSize,
            NET_GSM_CELL_INFO,
            EIsiSubBlockTypeId8Len8,
            sbOffset ) );

        TInt retWcdmaSb( aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + headerSize,
            NET_WCDMA_CELL_INFO,
            EIsiSubBlockTypeId8Len8,
            sbOffset ) );

        // Check if subblock exists.
        if ( KErrNone == retGsmSb || KErrNone == retWcdmaSb )
            {
            // NET_GSM_CELL_INFO and NET_WCDMA_CELL_INFO subblocks have identical
            // bytes and sizes ,use only NET_GSM_CELL_INFO subblock offset
            // values when getting data.

            // Get registration status value
            iLocInfo.iRegStatus = aIsiMessage.Get8bit(
                sbOffset + NET_GSM_CELL_INFO_OFFSET_SERVICESTATUS );

            TFLOGSTRING2("TSY: CSatMessHandler::StoreNetServiceStatus: \
                NET_REGISTRATION_STATUS = %d", iLocInfo.iRegStatus );
            OstTrace1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_STORENETSERVICESTATUS, "CSatMessHandler::StoreNetServiceStatus NET_REGISTRATION_STATUS = %d", iLocInfo.iRegStatus );

            if ( NET_SERVICE == iLocInfo.iRegStatus ||
                NET_LIMITED_SERVICE == iLocInfo.iRegStatus )
                {
                // Get country code
                iLocInfo.iOperatorCode.Copy(
                    aIsiMessage.GetData(
                        sbOffset + NET_GSM_CELL_INFO_OFFSET_OPERATORCODE,
                        3 ) );

                // Set location area code, 2 bytes
                iLocInfo.iLac.Copy(
                    aIsiMessage.GetData(
                        sbOffset + NET_GSM_CELL_INFO_OFFSET_CURRENTLAC,
                        2 ) );

                // Cell id + Extented Cell id, 4 bytes
                // Id's are included to Location Information TLV
                // reverse order, see 31.111, 8.19
                TPtrC8 tempCellId( aIsiMessage.GetData(
                    sbOffset + NET_GSM_CELL_INFO_OFFSET_CURRENTCELLID,
                    4 ) );
                // Copy cell id to be first in buffer
                iLocInfo.iCellId.Copy( tempCellId.Mid( 2 ) );
                // Append extented cell id part
                iLocInfo.iCellId.Append( tempCellId.Left( 2 ) );

                // Network type
                if ( NET_GSM_CELL_INFO == aIsiMessage.Get8bit(
                    sbOffset + NET_GSM_CELL_INFO_OFFSET_SUBBLOCKID ) )
                    {
                    iCurrentAccTech = KNetworkModeGsm;
                    }
                else
                    {
                    iCurrentAccTech = KNetworkModeUtran;
                    }
                }
            }
        else
            {
            TFLOGSTRING("TSY: CSatMessHandler::StoreNetServiceStatus \
                NET_GSM_CELL_INFO or NET_WCDMA_CELL_INFO not found!");
            OstTrace0( TRACE_NORMAL, DUP2_CSATMESSHANDLER_STORENETSERVICESTATUS, "CSatMessHandler::StoreNetServiceStatus NET_GSM_CELL_INFO or NET_WCDMA_CELL_INFO not found!" );
            }
        }
    else
        {
        TFLOGSTRING2("TSY: CSatMessHandler::StoreNetServiceStatus \
            Unexpected message id: %d", messageId );
        OstTrace1( TRACE_NORMAL, DUP3_CSATMESSHANDLER_STORENETSERVICESTATUS, "CSatMessHandler::StoreNetServiceStatus Unexpected message id: %d", messageId );
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SetUpEventListTerminalResp
// Constructs Setup EventList and calls
// SendTerminalResponse to send the terminal response
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SetUpEventListTerminalResp
        (
        TUint8 aTransId,             // Transaction id
        TDes8& aCommandDetails,      // Command details
        TUint8 aGeneralResult,       // General result
        TUint8 aAdditionalInfo       // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SETUPEVENTLISTTERMINALRESP, "CSatMessHandler::SetUpEventListTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SetUpEventListTerminalResp");
    // create responce data
    TTlv tlvSpecificData;

    //create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    //General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KSuccess != aGeneralResult &&
        RSat::KCmdBeyondMeCapabilities != aGeneralResult &&
        RSat::KErrorRequiredValuesMissing != aGeneralResult)
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
    iSetUpEventListTrSent = ETrue;
#endif // INFO_PP_ENHANCED_NETWORK_SELECTION

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::PollingOffTerminalResp
// Constructs PollingOff specific part of terminal
// response and calls SendTerminalResponse
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::PollingOffTerminalResp
        (
        TUint8 aTransId,
        TDes8& aCommandDetails,         // Transaction id
        TUint8 aGeneralResult,          // General result
        TUint8 aAdditionalInfo          // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_POLLINGOFFTERMINALRESP, "CSatMessHandler::PollingOffTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::PollingOffTerminalResp");

    TTlv tlvSpecificData;

    tlvSpecificData.AddTag( KTlvResultTag );
    tlvSpecificData.AddByte( aGeneralResult );

    if ( !( CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            // Cases in which additional info is not required
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
            case RSat::KErrorRequiredValuesMissing:
            case RSat::KHelpRequestedByUser:
                {
                break;
                }
            default:
                {
                tlvSpecificData.AddByte( aAdditionalInfo );
                break;
                }
            }
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::LocalInfoTerminalResp
// Constructs and sends the Local information response to
// the SIM card.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::LocalInfoTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Command details tlv
        TUint8 aGeneralResult,      // General result
        TDes& aAdditionalInfo       // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_LOCALINFOTERMINALRESP, "CSatMessHandler::LocalInfoTerminalResp" );
    TFLOGSTRING("TSY: CSatMessHandler::LocalInfoTerminalResp");
    // Create and append response data
    TTlv tlvData;
    // Create General Result TLV
    tlvData.AddTag( KTlvResultTag );

    // General result
    tlvData.AddByte( aGeneralResult );

    if ( RSat::KSuccess != aGeneralResult
        && RSat::KSuccessLimitedService != aGeneralResult )
        {
        if ( aAdditionalInfo.Length() )
            {
            tlvData.AddByte( TUint8( aAdditionalInfo[0] & 0xFF ) );
            }
        }
    else
        {
        switch ( aCommandDetails[KCommandQualifier] )
            {
            case RSat::KProvideLocalInfo:
                {
                const TLocationInfo& locInfo = LocationInfo();
                // Create Location Information TLV
                tlvData.AddTag( KTlvLocationInformationTag );
                // Append Mobile Country  & Network Codes, 3 bytes
                tlvData.AddData( locInfo.iOperatorCode );

                // Append LAC code
                tlvData.AddData( locInfo.iLac );

                // Append CellId
                tlvData.AddData( locInfo.iCellId );
                 break;
                }
            case RSat::KProvideLocalInfoImei:
                {
                // Add tlv tag
                tlvData.AddTag( KTlvImeiTag );
                TBuf8<8> bcdImei;
                // Convert IMEI to BCD format
                _LIT8( KTen, "*" );
                _LIT8( KZero, "0" );

                // Check if required changes is allready done for the IMEI
                if ( '*' != iIMEI[0] )
                    {
                    // Processing BCD string is much harder than ASCII so
                    // required changes is done before BCD conversion.
                    // (lot's of shifting etc..)
                    // Insert '*'(0x2A) as a first character which is converted
                    // to 0xA in AsciiToBCD method.
                    // Type of identity for IMEI code is 0xA "1010b"
                    // See 3GPP TS 04.08
                    iIMEI.Insert( 0, KTen );

                    // Spare digit: this digit shall be zero, when transmitted
                    // by the Mobile Station.
                    // See GSM 03.03
                    iIMEI.Replace( iIMEI.Length() - 1, 1 , KZero );
                    }

                TSatUtility::AsciiToBCD( iIMEI, bcdImei );
                // Add IMEI code to the terminal response
                tlvData.AddData( bcdImei );
                break;
                }
            case RSat::KProvideLocalInformationNmr:
                {
                // Check if NMR's were for UTRAN
                if ( iUtranNMR.Length() )
                    {
                    tlvData.AddTag( KTlvNetworkMeasurementResultsTag );
                    tlvData.AddData( iUtranNMR );
                    }
                else
                    {
                    // NMR's were for GSM
                    if ( iNMR.Length() )
                        {
                        // Network measurement result
                        tlvData.AddTag( KTlvNetworkMeasurementResultsTag );
                        tlvData.AddData( iNMR );
                        }

                    if ( iBCCHChannelList.Length() )
                        {
                        // append BCCH channel list tag
                        tlvData.AddTag( KTlvBccdChannelListTag );

                        TInt i( 0 );
                        TInt mask( 3 );
                        TInt shiftMove( 2 );
                        TUint8 nextChar( 0 );

                        // Convert BCCH channel list from 16-bit to 10-bit
                        // format
                        for ( i = 0; i < iBCCHChannelList.Length(); i++ )
                            {
                            TUint char2 =  ( iBCCHChannelList[i] & 0x03FF );
                            TUint8 char1 = TUint8( char2 >> shiftMove);

                            // append byte
                            tlvData.AddByte( TUint8( char1 | nextChar ) );

                            nextChar = TUint8 ( ( char2 & mask )
                                << ( 8 - shiftMove ) );

                            if ( 8 == shiftMove )
                                {
                                shiftMove = 2;
                                nextChar = 0;
                                tlvData.AddByte( TUint8( char2 & 0x00FF ) );
                                mask = 3;
                                }
                            else
                                {
                                shiftMove += 2;
                                mask = ( 1L << shiftMove) - 1;
                                }
                            }
                        // add last byte
                        tlvData.AddByte( nextChar );
                        }
                    }
                // clear nmr
                iNMR.Zero();
                // clear BCCH channel list
                iBCCHChannelList.Zero();
                // clear UTRAN NMR's
                iUtranNMR.Set( KNullDesC8 );
                break;
                }
            case RSat::KProvideLocalInfoDateTimeTimeZone:
                {
                tlvData.AddTag( KTlvDateTimeAndTimeZoneTag );
                // append current date/time/timezone to the terminal response
                TInt i( 0 );
                for ( i = 0; i < aAdditionalInfo.Length(); i++ )
                    {
                    tlvData.AddByte( TUint8( aAdditionalInfo[i] & 0x00FF ) );
                    }
                break;
                }
            case RSat::KProvideLocalInfoTimingAdv:
                {
                 TFLOGSTRING("TSY: CSatMessHandler::LocalInfoTerminalResp,\
                    TimingAdvance");
                 OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_LOCALINFOTERMINALRESP, "CSatMessHandler::LocalInfoTerminalResp, TimingAdvance" );

                 // Timing advance result
                 tlvData.AddTag( KTlvTimingAdvanceTag );
                 tlvData.AddByte( iTaStatus );
                 tlvData.AddByte( iTimingAdvance );
                break;
                }
            case RSat::KProvideLocalInfoLanguage:
                {
                TFLOGSTRING("TSY: CSatMessHandler::LocalInfoTerminalResp,\
                    LocalInfoLanguage");
                OstTrace0( TRACE_NORMAL, DUP2_CSATMESSHANDLER_LOCALINFOTERMINALRESP, "CSatMessHandler::LocalInfoTerminalResp, LocalInfoLanguage" );

                if ( aAdditionalInfo.Length() )
                    {
                    // create Language TLV
                    tlvData.AddTag( KTlvLanguageTag );
                    TBuf8<2> character;
                    TIsiUtility::CopyToBigEndian( aAdditionalInfo, character);
                    tlvData.AddData( character );
                    }
                break;
                }
            case RSat::KProvideLocalInfoAccTech:
                {
                TFLOGSTRING("TSY: CSatMessHandler::LocalInfoTerminalResp,\
                    Access Technology");
                OstTrace0( TRACE_NORMAL, DUP3_CSATMESSHANDLER_LOCALINFOTERMINALRESP, "CSatMessHandler::LocalInfoTerminalResp, Access Technology" );

                // Access technology result
                tlvData.AddTag( KTlvAccessTechnologyTag );
                tlvData.AddByte( iCurrentAccTech );

                break;
                }
            default:
                {
                break;
                }
            }
        }
    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvData.GetDataWithoutTopLevelTag(),
        aTransId );
    }
// -----------------------------------------------------------------------------
// CSatMessHandler::InfoSerialNumberReadReq
// Constructs INFO_SERIAL_NUMBER_READ_REQ ISI message.
// Called from CTsySatMessaging::ConstructL method.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::InfoSerialNumberReadReq
        (
        TUint8 aTransId,     // Transactio id
        TUint8 aTarget
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_INFOSERIALNUMBERREADREQ, "CSatMessHandler::InfoSerialNumberReadReq" );
    TFLOGSTRING("TSY:CSatMessHandler::InfoSerialNumberReadReq");

    //set infoSerialNumberReadReq to point to the send buffer
    TIsiSend infoSerialNumberReadReq( iPnSend->SendBufferDes() );
    infoSerialNumberReadReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_INFO );
    infoSerialNumberReadReq.Set8bit( ISI_HEADER_OFFSET_TRANSID, aTransId );
    infoSerialNumberReadReq.Set8bit( ISI_HEADER_OFFSET_MESSAGEID,
                                        INFO_SERIAL_NUMBER_READ_REQ );

    infoSerialNumberReadReq.Set8bit( ISI_HEADER_SIZE
                    + INFO_SERIAL_NUMBER_READ_REQ_OFFSET_TARGET, aTarget );

    //send message via Phonet
    return iPnSend->Send( infoSerialNumberReadReq.Complete() );

    }

// -----------------------------------------------------------------------------
// CSatMessHandler::InfoSerialNumberReadResp
// Breaks a INFO_SERIAL_NUMBER_READ_RESP ISI-message.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::InfoSerialNumberReadResp
        (
        const TIsiReceiveC& aIsiMessage    // received ISI-message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_INFOSERIALNUMBERREADRESP, "CSatMessHandler::InfoSerialNumberReadResp" );
    TFLOGSTRING("TSY:CSatMessHandler::InfoSerialNumberReadResp");

    TUint sbStartOffset( 0 );

    TInt retValue( aIsiMessage.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_INFO_SERIAL_NUMBER_READ_RESP,
                    INFO_SB_SN_IMEI_PLAIN, EIsiSubBlockTypeId8Len8,
                    sbStartOffset ) );
    //check if INFO_SB_SN_IMEI_PLAIN sub block is present
    if ( KErrNone == retValue )
        {
        //store imei code
        iIMEI.Copy( aIsiMessage.GetData(
                    sbStartOffset + INFO_SB_SN_IMEI_PLAIN_OFFSET_IMEIPLAINU8,
                    aIsiMessage.Get8bit( sbStartOffset +
                    INFO_SB_SN_IMEI_PLAIN_OFFSET_STRLEN ) - 1 ) );
        //Set the flag to inform that needed data has been received
        iSerialNumberReadReceived = ETrue;
        TFLOGSTRING("TSY: SAT, Imei received.");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_INFOSERIALNUMBERREADRESP, "SAT, Imei received." );
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NetNeighbourCellsReq
// Constructs NET_NEIGHBOUR_CELLS_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::NetNeighbourCellsReq
        (
        TUint8 aTransId,        // Transaction Id
        TUint8 aCellInfoType    // Cell info type
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETNEIGHBOURCELLSREQ, "CSatMessHandler::NetNeighbourCellsReq" );
    TFLOGSTRING("TSY:CSatMessHandler::NetNeighbourCellsReq");

    TBuf8<1> data;
    data.Append( aCellInfoType );

    return iPnSend->Send(
        PN_MODEM_NETWORK,
        aTransId,
        NET_NEIGHBOUR_CELLS_REQ,
        data );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::NetNeighbourCellResp
// Parse NET_NEIGHBOUR_CELLS_RESP message and read from it BCCH
// channel list and Network measurement results.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::NetNeighbourCellResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETNEIGHBOURCELLRESP, "CSatMessHandler::NetNeighbourCellResp" );
    TFLOGSTRING("TSY:CSatMessHandler::NetNeighbourCellResp");

    TUint8 result( RSat::KSuccess );

    if ( iTsySatMessaging->GetNotifyLocalInfo()->Status() )
        {
        // Provide local info proactive command is ongoing

        // Subblock offset
        TUint sbOffset( 0 );

        TInt successCode ( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + NET_NEIGHBOUR_CELLS_RESP_OFFSET_SUCCESSCODE ) );

        // Additional info
        TBuf<1> additionalInfo;
        additionalInfo.Zero();

        // Get transaction id
        TUint8 transId(
            iTsySatMessaging->GetNotifyLocalInfo()->GetTransactionId() );

        // Get command details tlv
        TBuf8<5> commandDetails;
        commandDetails =
            iTsySatMessaging->GetNotifyLocalInfo()->GetCmdDetails();

        TInt sbIndex( 1 );

        TInt retValue( aIsiMessage.FindSubBlockOffsetByIndex(
            ISI_HEADER_SIZE + SIZE_NET_NEIGHBOUR_CELLS_RESP,
            sbIndex,
            EIsiSubBlockTypeId8Len8,
            sbOffset ) );

        if ( NET_CAUSE_OK == successCode && KErrNone == retValue )
            {
            if ( NET_LIMITED_SERVICE == iLocInfo.iRegStatus )
                {
                result = RSat::KSuccessLimitedService;
                }

            switch( aIsiMessage.Get8bit( sbOffset ) )
                {
                case NET_GSM_SIM_NMR_INFO:
                    {
                    // Store network measurements results, 16 bytes
                    iNMR = aIsiMessage.GetData( sbOffset
                        + NET_GSM_SIM_NMR_INFO_OFFSET_MEASUREMENTRESULTS,
                        16 );

                    // Get BCCH channel list
                    TUint numChannels( aIsiMessage.Get8bit( sbOffset
                        + NET_GSM_SIM_NMR_INFO_OFFSET_NUMBEROFCHANNELS ) );

                    // Each channel takes 2 bytes
                    TPtrC8 bcchList( aIsiMessage.GetData(
                        sbOffset + NET_GSM_SIM_NMR_INFO_OFFSET_ARFCN,
                        2 * numChannels ) );

                    for ( TInt i = 0; i < bcchList.Length(); i+=2 )
                        {
                        iBCCHChannelList.Append(
                            bcchList[i] << 8 | bcchList[i+1] );
                        }
                    // Call local info terminal response method to send
                    // terminal response to the sim card.
                    LocalInfoTerminalResp( transId,
                        commandDetails,
                        result,
                        additionalInfo );
                    break;
                    }
                case NET_UTRAN_SIM_NMR_INFO:
                    {
                    TUint8 reportLength( aIsiMessage.Get8bit( sbOffset
                        + NET_UTRAN_SIM_NMR_INFO_OFFSET_MEASREPORTLENGTH ) );

                    iUtranNMR.Set( aIsiMessage.GetData(
                        sbOffset + NET_UTRAN_SIM_NMR_INFO_OFFSET_MEASREPORT,
                        reportLength ) );

                    LocalInfoTerminalResp( transId,
                        commandDetails,
                        result,
                        additionalInfo );
                    break;
                    }
                default:
                    {
                    //Response included unexpected subblock
                    iNMR.Zero();
                    iBCCHChannelList.Zero();
                    iUtranNMR.Set( KNullDesC8 );
                    additionalInfo.Append( RSat::KNoService );
                    // Send terminal response to the sim card
                    LocalInfoTerminalResp( transId,
                        commandDetails,
                        RSat::KMeUnableToProcessCmd,
                        additionalInfo );
                    break;
                    }
                }
            }
        else
            {
            // No service
            iNMR.Zero();
            iBCCHChannelList.Zero();
            iUtranNMR.Set( KNullDesC8 );
            additionalInfo.Append( RSat::KNoService );
            // Send terminal response to the sim card
            LocalInfoTerminalResp( transId,
                commandDetails,
                RSat::KMeUnableToProcessCmd,
                additionalInfo );
            }
        // Clear local info flag
        iTsySatMessaging->GetNotifyLocalInfo()->Status( ETrue );
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::CallModemResourceReq
// Sends a CALL_MODEM_RESOURCE_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::CallModemResourceReq( 
    TUint8 aTransId,
    const TDesC8& aMsg 
    )
    {
    TFLOGSTRING("TSY:CSatMessHandler::CallModemResourceReq");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_CALLMODEMRESOURCEREQ, "TSY:CSatMessHandler::CallModemResourceReq" );

    return iPnSend->Send( PN_MODEM_CALL, aTransId, CALL_MODEM_RESOURCE_REQ, aMsg );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::CallModemResourceConfReq
// Sends a CALL_MODEM_RESOURCE_CONF_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::CallModemResourceConfReq(
    TUint16 aResourceId,
    TUint16 aResourceIdMask )
    {
    TFLOGSTRING("TSY:CSatMessHandler::CallModemResourceConfReq");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_CALLMODEMRESOURCECONFREQ, "TSY:CSatMessHandler::CallModemResourceConfReq" );

    TBuf8< SIZE_CALL_MODEM_RESOURCE_CONF_REQ + 
        SIZE_CALL_MODEM_SB_RESOURCE_CONF> msg;

    // CALL_MODEM_RES_CONF_OPERATION
    msg.Append( CALL_MODEM_RES_CONF_SET );
    // Nbr of sb's
    msg.Append( 1 );
    
    TIsiSubBlock sbResourceConf(
        msg,
        CALL_MODEM_SB_RESOURCE_CONF,
        EIsiSubBlockTypeId8Len8 );
    
    TSatUtility::AppendWord( aResourceId, msg );
    TSatUtility::AppendWord( aResourceIdMask, msg);
    
    sbResourceConf.CompleteSubBlock();
    
    return iPnSend->Send( 
        PN_MODEM_CALL, 
        iTsySatMessaging->GetTransactionId(),
        CALL_MODEM_RESOURCE_CONF_REQ,
        msg );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SsResourceConfReq
// Sends a SS_RESOURCE_CONF_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SsResourceConfReq()
    {
    TFLOGSTRING("TSY:CSatMessHandler::SsResourceConfReq");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SSRESOURCECONFREQ, "TSY:CSatMessHandler::SsResourceConfReq" );
     
    TBuf8< SIZE_SS_RESOURCE_CONF_REQ  + 
        SIZE_SS_SB_RESOURCE_CONF > msg;

    // SS_RESOURCE_CONF_OPERATION 
    msg.Append( SS_RESOURCE_CONF_SET );
    // Nbr of sb's
    msg.Append( 1 );
    
    TIsiSubBlock sbResourceConf(
        msg,
        SS_SB_RESOURCE_CONF,
        EIsiSubBlockTypeId8Len8 );
    
    TSatUtility::AppendWord( SS_RES_ID_MO_SS_OPERATION, msg );
    TSatUtility::AppendWord( SS_RES_ID_MO_SS_OPERATION_MASK, msg);
    
    sbResourceConf.CompleteSubBlock();
    
    return iPnSend->Send( 
        PN_SS, 
        iTsySatMessaging->GetTransactionId(),
        SS_RESOURCE_CONF_REQ,
        msg );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SsResourceControlReq
// Sends a SS_RESOURCE_CONNTROL_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SsResourceControlReq( 
    TUint8 aTransId,
    const TDesC8& aMsg 
    )
    {
    TFLOGSTRING("TSY:CSatMessHandler::SsResourceControlReq");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SSRESOURCECONTROLREQ, "TSY:CSatMessHandler::SsResourceControlReq" );

    return iPnSend->Send(
        PN_SS,
        aTransId,
        SS_RESOURCE_CONTROL_REQ,
        aMsg );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::GpdsResourceConfReq
// Sends a GPDS_RESOURCE_CONF_REQISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::GpdsResourceConfReq()
    {
    TFLOGSTRING("TSY:CSatMessHandler::GpdsResourceConfReq");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GPDSRESOURCECONFREQ, "TSY:CSatMessHandler::GpdsResourceConfReq" );
    
    TBuf8< SIZE_GPDS_RESOURCE_CONF_REQ  + 
        SIZE_GPDS_RESOURCE_CONF > msg;

    // SS_RESOURCE_CONF_OPERATION 
    msg.Append( GPDS_RESOURCE_CONF_SET );
    // Nbr of sb's
    msg.Append( 1 );

    TIsiSubBlock sbResourceConf(
        msg,
        GPDS_RESOURCE_CONF,
        EIsiSubBlockTypeId8Len8 );

    TSatUtility::AppendWord( GPDS_RES_ID_CC_FOR_GPRS, msg );
    TSatUtility::AppendWord( GPDS_RES_ID_MASK_CC_FOR_GPRS, msg);

    sbResourceConf.CompleteSubBlock();

    return iPnSend->Send( 
        PN_GPDS, 
        iTsySatMessaging->GetTransactionId(),
        GPDS_RESOURCE_CONF_REQ,
        msg );
        }

// -----------------------------------------------------------------------------
// CSatMessHandler::GpdsResourceControlReq
// Sends a GPDS_RESOURCE_CONNTROL_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::GpdsResourceControlReq( 
    TUint8 aTransId,
    const TDesC8& aMsg 
    )
    {
    TFLOGSTRING("TSY:CSatMessHandler::GpdsResourceControlReq");
OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GPDSRESOURCECONTROLREQ, "CSatMessHandler::GpdsResourceControlReq" );

    return iPnSend->Send(
        PN_GPDS,
        aTransId,
        GPDS_RESOURCE_CONTROL_REQ,
        aMsg );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SetPollingInterval
// Set Polling Interval, sending a request to sim server
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SetPollingInterval( TUint8 aTraId, TUint8 aValue )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SETPOLLINGINTERVAL, "CSatMessHandler::SetPollingInterval" );
    TFLOGSTRING("TSY:CSatMessHandler::SetPollingInterval");

    iPollingInterval = aValue;

    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_TRANSID,
        aTraId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_MESSAGEID,
        UICC_CAT_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_SERVICETYPE,
        UICC_CAT_POLLING_SET );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_NSB,
        1 );

    // Add UICC_SB_POLLING_SET
    TBuf8<SIZE_UICC_SB_POLLING_SET> uiccSbPollingBuf( 0 );
    TIsiSubBlock uiccSbPolling(
        uiccSbPollingBuf,
        UICC_SB_POLLING_SET,
        EIsiSubBlockTypeId16Len16 );

    // Polling interval
    uiccSbPollingBuf.Append( aValue >> 8 );
    uiccSbPollingBuf.Append( aValue );

    // 2x filler
    uiccSbPollingBuf.Append( KPadding );
    uiccSbPollingBuf.Append( KPadding );

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_REQ,
        uiccSbPolling.CompleteSubBlock() );

    iPnSend->Send( isiMsg.Complete() );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::TimerMgmtTerminalResp
// Constructs and sends the Local information response to phonet
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::TimerMgmtTerminalResp
        (
        TUint8 aTransId,              // Transaction id
        TDes8& aCommandDetails,       // command details
        TUint8 aGeneralResult,        // General result
        TUint8 aTimerValue[3],        // Timer Value
        TUint8 aTimerId,              // Timer Identifier
        TUint8 aAdditionalInfo       // Additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_TIMERMGMTTERMINALRESP, "CSatMessHandler::TimerMgmtTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::TimerMgmtTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;

    //create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    //General result
    tlvSpecificData.AddByte( aGeneralResult );

    //include timer value and id only when command is performed successfully
    if( RSat::KSuccess == aGeneralResult )
        {

        //Timer Id
        //Value KTlvTimerIdentifierTagCRBitOn ( 0xA4 ) is added,
        //because used KTlvTimerIdentifierTag ( 0x24 ) doesn't
        //include comprehension bit in tag the value and it is expected
        //to be present in indications and terminal responses.
        tlvSpecificData.AddTag( KTlvTimerIdentifierTagCRBitOn );

        tlvSpecificData.AddByte( aTimerId );

        //Timer value is included when the Command Qualifier is indicating
        //"deactivate" or "get the current value of the timer"
        if ( aCommandDetails[4] )
            {
            tlvSpecificData.AddTag( KTlvTimerValueTag );
            //Timer value in semi-octets
            tlvSpecificData.AddByte( aTimerValue[0] ); //hours
            tlvSpecificData.AddByte( aTimerValue[1] ); //minutes
            tlvSpecificData.AddByte( aTimerValue[2] ); //seconds
            }
        }
    else if( RSat::KContradictionWithTimerState != aGeneralResult &&
             RSat::KErrorRequiredValuesMissing != aGeneralResult &&
             RSat::KCmdDataNotUnderstood != aGeneralResult &&
             RSat::KCmdTypeNotUnderstood != aGeneralResult )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }
    else
        {
        TFLOGSTRING("TSY: CSatMessHandler::TimerMgmtTerminalResp, Unknown result.");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_TIMERMGMTTERMINALRESP, "CSatMessHandler::TimerMgmtTerminalResp, Unknown result." );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::TimerExpirationInd
// Constructs TimerExpiration and calls SendAtkEnvelopeNtf to send the indication
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::TimerExpirationInd
        (
        TUint8 aTransId,        // Transaction id
        TUint8 aTimerId,        // Timer Identifier
        TUint8 aTimerValue[3]   // Timer Value
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_TIMEREXPIRATIONIND, "CSatMessHandler::TimerExpirationInd" );
    TFLOGSTRING("TSY:CSatMessHandler::TimerExpirationInd");

    TTlv data;

    data.Begin( KBerTlvTimerExpirationTag );

    //Create Device Identity TLV
    //Device identity tag
    data.AddTag( KTlvDeviceIdentityTag );
    //Source device identity = ME
    data.AddByte( KMe );
    //Destination device identity = SIM
    data.AddByte( KSim );

    //Value KTlvTimerIdentifierTagCRBitOn ( 0xA4 ) is added,
    //because used KTlvTimerIdentifierTag ( 0x24 ) doesn't
    //include comprehension bit in tag the value and it is expected
    //to be present in indications and terminal responses.
    data.AddTag( KTlvTimerIdentifierTagCRBitOn );
    data.AddByte( aTimerId);
    //Timer value in semi-octets
    data.AddTag( KTlvTimerValueTag );
    data.AddByte( aTimerValue[0] ); //hours
    data.AddByte( aTimerValue[1] ); //minutes
    data.AddByte( aTimerValue[2] ); //seconds

    return UiccCatReqEnvelope( aTransId, data.End() );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::RefreshResult
// Method to analyze refresh result from SIM Server.
// Called by CSatMessHandler::SimAtkResp.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::RefreshResult( TUint8 aStatus ) // UICC server status
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_REFRESHRESULT, "CSatMessHandler::RefreshResult" );
    TFLOGSTRING("TSY:CSatMessHandler::RefreshResult");

    TUint8 result( RSat::KSuccess );
    TUint8 additionalInfo( 0 );
    TInt ret( KErrNone );

    if ( iRefreshRequestIsOn )
        {
        switch ( aStatus )
            {
            case UICC_STATUS_OK:
                {
                // Refresh is done!
                TFLOGSTRING("TSY: CSatMessHandler::RefreshResult, ISA has performed refresh successfully ");
                TFLOGSTRING("TSY: CSatMessHandler::RefreshResult, Inform S60 to re-read SIM data");
                OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_REFRESHRESULT, "CSatMessHandler::RefreshResult, ISA has performed refresh successfully " );
                OstTrace0( TRACE_NORMAL, DUP2_CSATMESSHANDLER_REFRESHRESULT, "CSatMessHandler::RefreshResult, Inform S60 to re-read SIM data" );

                // Read service table
                if ( CSatNotifyRefresh::KCacheEFSST &
                    iTsySatMessaging->GetNotifyRefresh()->CachedFiles() )
                    {
                    // Re-read EF-UST
                    // Call control in USSD is supported only in SIM, byte 6
                    if ( UICC_CARD_TYPE_ICC == iCardType )
                        {
                        UiccReadServiceTableReq( KUiccTrIdServiceTableByte6, 5 );
                        }

                    // Call control, byte 4 from service table
                    UiccReadServiceTableReq( KUiccTrIdServiceTableByte4, 3 );
#ifdef INFO_PP_ATK_MO_SM_CONTROL
                    // Check MO SMS control is supported by product profile
                    // USIM: byte 4, SIM: byte 5
                    TUint8 serviceTableTrIdMoSms( KUiccTrIdServiceTableByte4 );
                    TUint8 serviceTableOffsetMoSms( 3 );
                    if ( UICC_CARD_TYPE_ICC == iCardType )
                        {
                        // Transaction ID and byte offset for SIM
                        serviceTableTrIdMoSms = KUiccTrIdServiceTableByte5;
                        serviceTableOffsetMoSms = 4;
                        }
                    if ( iMoSmsSupportedInPp )
                        {
                        // ...and by SIM service table
                        UiccReadServiceTableReq(
                            serviceTableTrIdMoSms,
                            serviceTableOffsetMoSms );
                        }
#else
                    // If flag not defined, check feature availability only
                    // from USIM
                    UiccReadServiceTableReq(
                        serviceTableTrIdMoSms,
                        serviceTableOffsetMoSms );
#endif // INFO_PP_ATK_MO_SM_CONTROL
                    }
                break;
                }
            default:
                {
                TFLOGSTRING2("TSY: CSatMessHandler::RefreshResult, refresh failed, UICC STATUS: 0x%x", aStatus );
                OstTraceExt1( TRACE_NORMAL, DUP4_CSATMESSHANDLER_REFRESHRESULT, "CSatMessHandler::RefreshResult, refresh failed, UICC STATUS: 0x%hhx", aStatus );
                result = RSat::KMeUnableToProcessCmd;
                additionalInfo = RSat::KNoSpecificMeProblem;
                ret = KErrGeneral;
                break;
                }
            }

        if ( RSat::KSuccess != result )
            {
            // If result was not successfull then send terminal response
            // Otherwise terminal response is sent by the Client.
            RefreshTerminalResp(
                iTsySatMessaging->GetNotifyRefresh()->GetTransactionId(),
                iTsySatMessaging->GetNotifyRefresh()->GetCmdDetails(),
                result,
                additionalInfo );
            }
        // Complete refresh to the client even if it was not successfully,
        // for buffer cleaning purposes. Also this way the Client will know
        // that refresh was terminated.
        iTsySatMessaging->GetNotifyRefresh()->CompleteRequest( ret );
        // Refresh is now handled, clear refresh flag
        iRefreshRequestIsOn = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::SendSmsReportReq
// Send a Sms-Pp report to SMS server using a request
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SendSmsReportReq
        (
        TUint8 aTransId,
        TDesC8& aData
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDSMSREPORTREQ, "CSatMessHandler::SendSmsReportReq" );
    TFLOGSTRING("CSatMessHandler::SendSmsReportReq");

    iPnSend->Send( PN_SMS, aTransId, SMS_RECEIVED_MSG_REPORT_REQ, aData );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SmsResourceConfReq
// Send a request to SMS server
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SmsResoureConfReq
        (
        TUint8 aTransId,
        TUint8 aMsgId,
        TDesC8& aData
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SMSRESOURCECONFREQ, "CSatMessHandler::SmsResourceConfReq" );
    TFLOGSTRING("CSatMessHandler::SmsResourceConfReq");

    iPnSend->Send( PN_SMS, aTransId, aMsgId, aData );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SetPollingResult
// Sends terminal response according to the response coming from Sim server
// to a request having a service type KSimStSetPolling.
// This method handles both PollingInterval and PollingOff proactive commands.
// Called by SimAtkResp.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SetPollingResult(
        TUint8 aStatus,
        TUint8 aTransId,
        TUint8 aInterval
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SETPOLLINGRESULT, "CSatMessHandler::SetPollingResult" );
    TFLOGSTRING("TSY:CSatMessHandler::SetPollingResult ");

    if ( UICC_STATUS_OK == aStatus )
        {
        // The response can be from a PollInterval or a PollingOff command
        if ( iPollingOff )
            {
            TFLOGSTRING("TSY:CSatMessHandler::SetPollingResult, iPollingOff = ETrue");
            OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_SETPOLLINGRESULT, "CSatMessHandler::SetPollingResult, iPollingOff = ETrue" );
            PollingOffTerminalResp(
                aTransId,
                iTsySatMessaging->GetNotifyPollingOff()->GetCmdDetails(),
                RSat::KSuccess,
                RSat::KNoAdditionalInfo );
            }
        else
            {
            TFLOGSTRING("TSY:CSatMessHandler::SetPollingResult, iPollingOff = EFalse");
            OstTrace0( TRACE_NORMAL, DUP2_CSATMESSHANDLER_SETPOLLINGRESULT, "CSatMessHandler::SetPollingResult, iPollingOff = EFalse" );
            PollIntervalTerminalResp(
                aTransId,
                iTsySatMessaging->GetNotifyPollInterval()->GetCmdDetails(),
                RSat::KSuccess,
                RSat::KNoAdditionalInfo,
                aInterval );
            }
        }
    else
        {
        if ( iPollingOff )
            {
            // PollingOff was actually not executed, then set iPollingOff to false
            iPollingOff = EFalse;
            PollingOffTerminalResp(
                aTransId,
                iTsySatMessaging->GetNotifyPollingOff()->GetCmdDetails(),
                RSat::KMeUnableToProcessCmd,
                RSat::KNoSpecificMeProblem );
            }
        else
            {
            // If last request rejected and request was over 25 seconds,
            // renew the request with legacy maximum. Otherwise there was
            // an actual error
            if ( iLongPollIntervalReq )
                {
                SetPollingInterval(aTransId, KMaxLegacyPollInterval );
                iLongPollIntervalReq = EFalse;
                }
            else
                {
                PollIntervalTerminalResp(
                aTransId,
                iTsySatMessaging->GetNotifyPollInterval()->GetCmdDetails(),
                RSat::KMeUnableToProcessCmd,
                RSat::KNoSpecificMeProblem,
                aInterval );
                }
            }
        }

        // Terminal Response is sent, reset flag
        iLongPollIntervalReq = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::CheckProactiveCommand
// Method to check validity of received proactive command.
// Checked fields are: Proactive command tag, length, device id and
// command details tag.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::CheckProactiveCommand
        (
        const TIsiReceiveC& aIsiMessage // received isi message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_CHECKPROACTIVECOMMAND, "CSatMessHandler::CheckProactiveCommand" );
    TFLOGSTRING("TSY:CSatMessHandler::CheckProactiveCommand");

    TTlv data;
    // Get ber tlv
    CBerTlv berTlv;
    TBuf8<5> commandDetailsTlv; // 5 = command details data length

    // BerTlv method checks Proactive command tag 'D0'.
    if ( KErrNone == berTlv.BerTlv( aIsiMessage ) )
        {
        TInt ret( KErrNone );
        TUint8 generalResult( 0 );

        CTlv commandDetails;
        CTlv deviceId;
        // Get length part of TLV
        TUint16 length( berTlv.GetLength() );

        // compare TLVs length value and real length
        if ( length > KOneByteLengthLimit )
            {
            // 5 = Proactive command tag + two byte length tag + length + SW1
            // + SW2
            // GetSize returns total size.
            if ( ( length + 5 ) !=  berTlv.GetSize() )
                {
                // result
                generalResult = RSat::KCmdDataNotUnderstood;
                ret = KErrCorrupt;
                }
            }
        // 4 = Proactive command tag + length + SW1 + SW2
        else if ( ( length + 4 ) !=  berTlv.GetSize() )
            {
            // result
            generalResult = RSat::KCmdDataNotUnderstood;
            ret = KErrCorrupt;
            }
        else if ( KErrNone != CheckTlvObjects( berTlv ) )
            {
            // result
            generalResult = RSat::KCmdDataNotUnderstood;
            ret = KErrCorrupt;
            }

        // Check command details tlv and device id tlv
        else if ( ( KErrNotFound == berTlv.TlvByTagValue( &commandDetails,
            KTlvCommandDetailsTag ) )
            || ( KErrNotFound == berTlv.TlvByTagValue( &deviceId,
                KTlvDeviceIdentityTag ) ) )
            {
            // result
            generalResult = RSat::KErrorRequiredValuesMissing;
            ret = KErrCorrupt;
            }

        // Check that len of command details is correct
        else if( 5 != commandDetails.GetSize() )
            {
            generalResult = RSat::KCmdDataNotUnderstood;
            ret = KErrCorrupt;
            }
        else
            {
            TFLOGSTRING("TSY: CSatMessHandler::CheckProactiveCommand. Proactive command Ok.");
            OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_CHECKPROACTIVECOMMAND, "CSatMessHandler::CheckProactiveCommand Proactive command Ok." );
            }

        if ( KErrCorrupt == ret )
            {
            if ( !berTlv.TlvByTagValue( &commandDetails,
                KTlvCommandDetailsTag ) && 
                5 == commandDetails.GetSize()  )
                {
                commandDetailsTlv.Copy( commandDetails.Data() );
                }
            else
                {
                // The SIM shall interpret a Terminal Response with a command
                // number '00' as belonging to the last proactive command
                // having been sent to the ME.
                // See 3GPP TS 11.14
                commandDetailsTlv.FillZ( 5 ); // 5 = command details data length
                }

            data.AddTag( KTlvResultTag );
            data.AddByte( generalResult );

            UiccCatReqTerminalResponse(
                commandDetailsTlv,
                data.GetDataWithoutTopLevelTag(),
                aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
            return KErrCorrupt;
            }

        return KErrNone;
        }
    // 'D0' is missing in the begin of the proactive command.
    data.AddTag( KTlvResultTag );
    // result
    data.AddByte( RSat::KErrorRequiredValuesMissing );
    // Set command details tlv to Zero, because method to read command details
    // does not work if 'D0' is missing...
    commandDetailsTlv.FillZ( 5 ); // 5 = command details data length
    UiccCatReqTerminalResponse(
        commandDetailsTlv,
        data.GetDataWithoutTopLevelTag(),
        aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    return KErrCorrupt;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::MoreTimeTerminalResp
// Constructs and sends the More Time response to phonet
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::MoreTimeTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Command details
        TUint8 aGeneralResult       // General result
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_MORETIMETERMINALRESP, "CSatMessHandler::MoreTimeTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::MoreTimeTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;

    //create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    //General result
    tlvSpecificData.AddByte( aGeneralResult );

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SmsCbRoutingReq
// Sends a request to Sms server to allow routing of Cell Broadcast messages.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SmsCbRoutingReq
        (
        TUint8 aTransId,        // Transaction Id
        TUint8 aRoutingCommand  // Routing command
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SMSCBROUTINGREQ, "CSatMessHandler::SmsCbRoutingReq" );
    TFLOGSTRING("TSY:CSatMessHandler::SmsRoutingReq");

    TInt ret( KErrNone );

        // Routing is ongoing
        iSmsCbRoutingComplete = EFalse;

        // TBuf8<SIZE_SMS_CB_ROUTING_REQ> because we are sending [U]SIM EF CBMID Message
        // Subscription;
        TBuf8<SIZE_SMS_CB_ROUTING_REQ> data;
        
        // Append Routing command
        data.Append( aRoutingCommand );
        // Append Subscription number
        data.Append( SMS_NEW_SUBSCRIPTION );
        // append Subscription type
        data.Append( SMS_TYPE_SIM );
        // Append Filler bytes
        data.AppendFill( KPadding, 2 );
        // Append no of Subblocks
        data.Append( 0 );

        // Send Subscription request 
        ret = iPnSend->Send( PN_SMS, aTransId, SMS_CB_ROUTING_REQ,
                data );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SmsCbRoutingResp
// Handles response from Sms server for a request of type SmsGsmTempCbRoutingReq.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SmsCbRoutingResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SMSCBROUTINGRESP, "CSatMessHandler::SmsCbRoutingResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SmsCbRoutingResp");

    TUint8 cause( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SMS_CB_ROUTING_RESP_OFFSET_SMSCAUSE ) );

    switch ( cause )
        {
        case SMS_ERR_CS_INACTIVE:
        case SMS_ERR_SUBJECT_COUNT_OVERFLOW:
        case SMS_ERR_DCS_COUNT_OVERFLOW:
            {
            TFLOGSTRING2("TSY:CSatMessHandler::SmsCbRoutingResp \
                Routing req failed on temporary problem: %d", cause );
            OstTraceExt1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_SMSCBROUTINGRESP, "CSatMessHandler::SmsCbRoutingResp Routing req failed on temporary problem: %hhu", cause );
            iSmsCbRoutingComplete = EFalse;
            break;
            }
        case SMS_OK:
        case SMS_ERR_INVALID_SUBSCRIPTION_NR:
        case SMS_ERR_INVALID_PARAMETER:
        case SMS_ERR_ALL_SUBSCRIPTIONS_ALLOCATED:
        default:
            {
            // Routing req was either completed successfully, or permanent
            // problem occured. In both cases, no need to retry.
            iSmsCbRoutingComplete = ETrue;
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::SmsResourceConfInd
// Handles Indication from SMS Server for resource configuration Startup 
// and reconfigure condition .
// -----------------------------------------------------------------------------
//

void CSatMessHandler::SmsResourceConfInd
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    TFLOGSTRING("TSY:CSatMoSmsCtrl::SmsResourceConfIndReceived: Response for Resource confoguration Indication, Receive Indication from SMS server");
    OstTrace0( TRACE_NORMAL, DUP1_CSATMOSMSCTRL_SMSRESOURCECONFINDRECEIVED, "CSatMoSmsCtrl::SmsResourceRespReceived: Response for Resource Configuration Ind, Receive indication from SMS server" );

    // To check The receive indication is for startup or reconfiguration
    if ( SMS_RES_CONF_STARTUP == aIsiMessage.Get8bit( ISI_HEADER_SIZE
        + SMS_RESOURCE_CONF_IND_OFFSET_CONFSTATUS ) )
        {
        TUint sbStartOffset( 0 );
        TInt retValue( KErrNotFound );
        // Check if resource control is requested for MO SM.
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SMS_RESOURCE_CONF_IND,
            SMS_SB_RESOURCE_CONF_REQUIRED,
            EIsiSubBlockTypeId16Len16,
            sbStartOffset ) )
            {
            TUint16 resource( aIsiMessage.Get16bit( sbStartOffset +
                SMS_SB_RESOURCE_CONF_REQUIRED_OFFSET_RESPP ) );
            // Check MO SMS control is supported by SIM service table
            if ( SMS_RES_ID_CONF_MO_SM_INIT == resource )
                {
                if ( UICC_CARD_TYPE_UICC == iCardType )
                    {
                    // USIM: Read MO-SMS control from byte 4
                    UiccReadServiceTableReq( KUiccTrIdServiceTableByte4, 3 );
                    }
                else if ( UICC_CARD_TYPE_ICC == iCardType )
                    {
                    // SIM: Read MO-SMS control from byte 5
                    UiccReadServiceTableReq( KUiccTrIdServiceTableByte5, 4 );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NetCellInfoGetReq
// Creates NET_CELL_INFO_GET_REQ ISI message and sends it to Phonet
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::NetCellInfoGetReq
        (
        TUint8 aTransId
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETCELLINFOGETREQ, "CSatMessHandler::NetCellInfoGetReq" );
    TFLOGSTRING("TSY:CSatMessHandler::NetCellInfoGetReq");

    TBuf8<1> noData;
    return iPnSend->Send(
        PN_MODEM_NETWORK,
        aTransId,
        NET_CELL_INFO_GET_REQ,
        noData );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NetCellInfoGetResp
// Breaks a NET_CELL_INFO_GET_RESP ISI-message and if this response is for
// local info proactive command then send a terminal response to the sim card.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::NetCellInfoGetResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETCELLINFOGETRESP, "CSatMessHandler::NetCellInfoGetResp" );
    TFLOGSTRING("TSY: CSatMessHandler::NetCellInfoGetResp");

    TBuf<1> additionalInfo;

    // Default result
    TUint8 result( RSat::KMeUnableToProcessCmd );

    // Store recieved network parameters
    StoreNetServiceStatus( aIsiMessage );

    switch ( iLocInfo.iRegStatus )
        {
        case NET_SERVICE:
            {
            result = RSat::KSuccess;
            break;
            }
        case NET_LIMITED_SERVICE:
            {
            result = RSat::KSuccessLimitedService;
            break;
            }
        case NET_NO_COVERAGE:
            {
            // Default result value already set
            additionalInfo.Append( RSat::KNoService );
            break;
            }
        default:
            {
            // Default result value already set
            TFLOGSTRING2("TSY: CSatMessHandler::NetCellInfoGetResp, Unexpected iRegStatus: %d", iLocInfo.iRegStatus );
            OstTrace1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_NETCELLINFOGETRESP, "CSatMessHandler::NetCellInfoGetResp Unexpected iRegStatus: %d", iLocInfo.iRegStatus );
            additionalInfo.Append( RSat::KNoSpecificMeProblem );
            break;
            }
        }

    TInt trId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // Check if this response is for provide local info proactive command.
    if ( iTsySatMessaging->GetNotifyLocalInfo()->Status()
        && iTsySatMessaging->GetNotifyLocalInfo()->GetTransactionId()
            == trId )
        {
        // Send terminal response
        LocalInfoTerminalResp( iTsySatMessaging->GetNotifyLocalInfo()
                                    ->GetTransactionId(),
                                iTsySatMessaging->GetNotifyLocalInfo()
                                    ->GetCmdDetails(),
                                result,
                                additionalInfo );
        // Clear local info flag
        iTsySatMessaging->GetNotifyLocalInfo()->Status( ETrue );
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::GssCsServiceReq
// Constructs GSS_CS_SERVICE_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::GssCsServiceReq
        (
        TUint8 aTransId,          // Transaction Id
        TUint8 aOperation         // Gss operation
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GSSCSSERVICEREQ, "CSatMessHandler::GssCsServiceReq" );
    TFLOGSTRING("TSY: CSatMessHandler::GssCsServiceReq ");

    TBuf8<2> data;
    data.Append( aOperation );
    data.Append( 0x00 ); // number of subblocks
    return iPnSend->Send( PN_GSS, aTransId, GSS_CS_SERVICE_REQ, data );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::GssCsServiceResp
// Breaks GSS_CS_SERVICE_RESP ISI message.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::GssCsServiceResp
        (
        const TIsiReceiveC& aIsiMessage // Received isi messge
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GSSCSSERVICERESP, "CSatMessHandler::GssCsServiceResp" );
    TFLOGSTRING("CSatMessHandler::GssCsServiceResp ");

    if ( iTsySatMessaging->GetNotifyLocalInfo()->Status() )
        {
        TUint8 generalResult ( RSat::KSuccess );

        // Provide local info proactive command is ongoing

        TBuf<1> additionalInfo;
        additionalInfo.Zero();

        // Get transaction id
        TUint8 transId(
            iTsySatMessaging->GetNotifyLocalInfo()->GetTransactionId() );
        // Get command details tlv
        TBuf8<5> commandDetails;
        commandDetails =
            iTsySatMessaging->GetNotifyLocalInfo()->GetCmdDetails();

        // If this is a Timing advance response and message includes subblocks
        if ( GSS_ATK_TIMING_ADVANCE_GET == aIsiMessage.Get8bit(
             ISI_HEADER_SIZE + GSS_CS_SERVICE_RESP_OFFSET_OPERATION )
             &&
             0 < aIsiMessage.Get8bit(
             ISI_HEADER_SIZE + GSS_CS_SERVICE_RESP_OFFSET_NBROFSUBBLOCKS ) )
            {
            // GSS_ATK_TIMING_ADVANCE sub block

            // SubBlock offset
            TUint sbOffset( 0 );

            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_GSS_CS_SERVICE_RESP,
                GSS_ATK_TIMING_ADVANCE,
                EIsiSubBlockTypeId8Len8,
                sbOffset) )
                {
                // Store Timing Advance Status
                iTaStatus = aIsiMessage.Get8bit(
                    sbOffset + GSS_ATK_TIMING_ADVANCE_OFFSET_TASTATUS );

                TFLOGSTRING2("TSY: CSatMessHandler::GssCsServiceResp iTaStatus=%d", iTaStatus);
                OstTraceExt1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_GSSCSSERVICERESP, "CSatMessHandler::GssCsServiceResp iTaStatus: %hhu", iTaStatus );

                // Store timing advance
                iTimingAdvance = aIsiMessage.Get8bit(
                    sbOffset + GSS_ATK_TIMING_ADVANCE_OFFSET_TIMINGADVANCE );

                TFLOGSTRING2("TSY: CSatMessHandler::GssCsServiceResp \
                    iTimingAdvance=%d", iTimingAdvance);
                OstTraceExt1( TRACE_NORMAL, DUP2_CSATMESSHANDLER_GSSCSSERVICERESP, "CSatMessHandler::GssCsServiceResp iTimingAdvance: %hhu", iTimingAdvance );

                if ( GSS_TIMING_ADVANCE_NOT_AVAILABLE == iTimingAdvance )
                    {
                    generalResult = RSat::KMeUnableToProcessCmd;
                    }
                }
            else
                {
                 generalResult = RSat::KErrorRequiredValuesMissing;
                }

            // Call local info terminal response method to send
            // terminal response to the sim card.
            LocalInfoTerminalResp( transId,
                                    commandDetails,
                                    generalResult,
                                    additionalInfo );

            // Clear local info flag
            iTsySatMessaging->GetNotifyLocalInfo()->Status( ETrue );
            }
        else
            {
            // No service
            additionalInfo.Append( RSat::KNoService );

            // Clear local info flag
            iTsySatMessaging->GetNotifyLocalInfo()->Status( ETrue );

            // Call local info terminal response method to send
            // terminal response to the sim card.
            LocalInfoTerminalResp( transId,
                                    commandDetails,
                                    RSat::KMeUnableToProcessCmd,
                                    additionalInfo );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::GssCsServiceFailResp
// Breaks a GSS_CS_SERVICE_FAIL_RESP ISI message.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::GssCsServiceFailResp
        (
        const TIsiReceiveC& aIsiMessage // Received isi messge
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GSSCSSERVICEFAILRESP, "CSatMessHandler::GssCsServiceFailResp" );
    TFLOGSTRING("CSatMessHandler::GssCsServiceFailResp ");

    if ( iTsySatMessaging->GetNotifyLocalInfo()->Status() )
        {
        if ( GSS_ATK_TIMING_ADVANCE_GET == aIsiMessage.Get8bit(
             ISI_HEADER_SIZE + GSS_CS_SERVICE_RESP_OFFSET_OPERATION ) )
            {
            TBuf<1> additionalInfo;
            additionalInfo.Zero();
            additionalInfo.Append( RSat::KNoService );
            // Get transaction id
            TUint8 transId(
                iTsySatMessaging->GetNotifyLocalInfo()->GetTransactionId() );
            // Get command details tlv
            TBuf8<5> commandDetails;
            commandDetails =
                iTsySatMessaging->GetNotifyLocalInfo()->GetCmdDetails();

            // Call local info terminal response method to send
            // terminal response to the sim card.
            LocalInfoTerminalResp( transId,
                                    commandDetails,
                                    RSat::KMeUnableToProcessCmd,
                                    additionalInfo );

            // Clear local info flag
            iTsySatMessaging->GetNotifyLocalInfo()->Status( ETrue );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::LanguageNotificationTerminalResp
// Constructs LanguageNotification specific part of terminal
// response and calls SendTerminalResponse to send the terminal response
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::LanguageNotificationTerminalResp
        (
        TUint8 aTransId,            // Transaction id
        TDes8& aCommandDetails,     // Comamnd number
        TUint8 aGeneralResult,      // General result
        TUint8 aAdditionalInfo      // additional info
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_LANGUAGENOTIFICATIONTERMINALRESP, "CSatMessHandler::LanguageNotificationTerminalResp" );
    TFLOGSTRING("TSY:CSatMessHandler::LanguageNotificationTerminalResp");

    TTlv tlvSpecificData;
    // Create general result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    // General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( RSat::KSuccess != aGeneralResult )
        {
        tlvSpecificData.AddByte( aAdditionalInfo );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NetTimeInd
// Break's a NET_TIME_IND isi-message and stores time zone information.
// Time zone is needed in provide local information proactive command.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::NetTimeInd
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETTIMEIND, "CSatMessHandler::NetTimeInd" );
    TFLOGSTRING("TSY: CSatMessHandler::NetTimeInd");

    // NET_TIME_INFO is mandatory sub block

    // SubBlock offset
    TUint sbOffset( 0 );

    // Find NET_TIME_INFO subblock
    TInt ret ( aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_NET_TIME_IND,
                NET_TIME_INFO,
                EIsiSubBlockTypeId8Len8,
                sbOffset ) );

    if ( KErrNone == ret )
        {
        // Get timezone
        // If the time zone is invalid or time zone is unknown, all bits are
        // set to 1 (including B6).
        iTimeZone = aIsiMessage.Get8bit( sbOffset + NET_TIME_INFO_OFFSET_TIMEZONE );
        TFLOGSTRING2("TSY: CSatMessHandler::NetTimeInd, Time zone: 0x%x", iTimeZone );
        OstTraceExt1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_NETTIMEIND, "CSatMessHandler::NetTimeInd Time zone: %hhu", iTimeZone );
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NetRatReq
// Constructs NET_RAT_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::NetRatReq
        (
        TUint8 aTransId         // Transaction Id
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETRATREQ, "CSatMessHandler::NetRatReq" );
    TFLOGSTRING("TSY: CSatMessHandler::NetRatReq ");

    TBuf8<1> data;
    //we need to know the current RAT
    data.Append( NET_CURRENT_RAT );

    return iPnSend->Send( PN_MODEM_NETWORK, aTransId, NET_RAT_REQ, data );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::NetRatResp
// Constructs NET_RAT_RESP ISI message.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::NetRatResp
        (
        const TIsiReceiveC& aIsiMessage //received ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETRATRESP, "CSatMessHandler::NetRatResp" );
    TFLOGSTRING("TSY:CSatMessHandler::NetRatResp");

    if ( iTsySatMessaging->GetNotifyLocalInfo()->Status() )
        {
        TBuf<1> additionalInfo;
        additionalInfo.Zero();

        // Get transaction id
        TUint8 transId(
            iTsySatMessaging->GetNotifyLocalInfo()->GetTransactionId() );

        // Get command details tlv
        TBuf8<5> commandDetails;
        commandDetails =
            iTsySatMessaging->GetNotifyLocalInfo()->GetCmdDetails();

        if ( NET_CAUSE_OK == aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + NET_RAT_RESP_OFFSET_SUCCESSCODE ) )
            {
            TUint8 generalResult ( RSat::KSuccess );

            // Subblock offset
            TUint sbOffset( 0 );

            // Check if NET_RAT_INFO sub block is present
            TInt ret( aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_NET_RAT_RESP,
                NET_RAT_INFO,
                EIsiSubBlockTypeId8Len8,
                sbOffset ) );

            if ( KErrNone == ret )
                {
                // Get ratName
                TUint8 ratName( aIsiMessage.Get8bit(
                    sbOffset + NET_RAT_INFO_OFFSET_RATNAME ) );

                switch ( ratName )
                    {
                    case NET_GSM_RAT:
                        {
                        iCurrentAccTech = KNetworkModeGsm;
                        break;
                        }
                    case NET_UMTS_RAT:
                        {
                        iCurrentAccTech = KNetworkModeUtran;
                        break;
                        }
                    default:
                        {
                        generalResult= RSat::KMeUnableToProcessCmd;
                        break;
                        }
                    }
                }
            else
                {
                generalResult = RSat::KMeUnableToProcessCmd;
                }

            // Call local info terminal response method to send
            // terminal response to the sim card.
            LocalInfoTerminalResp( transId,
                                commandDetails,
                                generalResult,
                                additionalInfo );

            // Clear local info flag
            iTsySatMessaging->GetNotifyLocalInfo()->Status( ETrue );

            }
         else
            {
            // No service
            additionalInfo.Append( RSat::KNoService );

            // Clear local info flag
            iTsySatMessaging->GetNotifyLocalInfo()->Status( ETrue );

            // Call local info terminal response method to send
            // terminal response to the sim card.
            LocalInfoTerminalResp( transId,
                                    commandDetails,
                                    RSat::KMeUnableToProcessCmd,
                                    additionalInfo );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::OpenChannelTerminalResp
// Constructs Open Channel specific part of terminal
// response and calls SendTerminalResponse to send the terminal response.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::OpenChannelTerminalResp
        (
        TUint8 aTransId,            // Transaction Id
        TDes8& aCommandDetails,     // Command details
        TUint8 aGeneralResult,      // General Result
        TDes8& aAdditionalInfo,     // Additional Info
        TUint8 aBearerType,         // Bearer type
        TDesC8& aBearer,            // Bearer
        TUint16 aBufferSize         // Buffer size
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_OPENCHANNELTERMINALRESP, "CSatMessHandler::OpenChannelTerminalResp" );
    TFLOGSTRING("CSatMessHandler::OpenChannelTerminalResp");

    // Create and append response data
    TTlv tlvSpecificData;

    //create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    //General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult )
        || ( RSat::KInteractionWithCCPermanentError == aGeneralResult )
        || ( RSat::KErrorRequiredValuesMissing == aGeneralResult )
        || ( RSat::KBearerIndepProtocolError == aGeneralResult ) )
        {
        if( aAdditionalInfo.Length())
        // UNSuccessful result neccessitating additional info byte
        tlvSpecificData.AddByte( aAdditionalInfo[1] );
        }
    else if ( RSat::KSuccess == aGeneralResult
        || ( RSat::KPartialComprehension == aGeneralResult )
        || ( RSat::KMissingInformation == aGeneralResult )
        || ( RSat::KPerformedWithModifications == aGeneralResult )
        || ( RSat::KSuccessRequestedIconNotDisplayed == aGeneralResult ) )
        {
        // Successful result: append Channel Status using
        // additional info.
        for ( TInt i=0; i<aAdditionalInfo.Length();i+=4 )
            {
            tlvSpecificData.AddTag( KTlvChannelStatusTag );
            tlvSpecificData.AddByte( aAdditionalInfo[i+1] );
            tlvSpecificData.AddByte( aAdditionalInfo[i+3] );
            }
        }
    else
        {
        TFLOGSTRING("TSY: CSatMessHandler::OpenChannelTerminalResp, General result did not match.");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_OPENCHANNELTERMINALRESP, "CSatMessHandler::OpenChannelTerminalResp, General result did not match." );
        }

    // For any result:
    // Append Bearer Description data

    if ( aBearer.Length() )
        {
        tlvSpecificData.AddTag( KTlvBearerDescriptionTag );
        tlvSpecificData.AddByte( aBearerType );
        tlvSpecificData.AddData( aBearer );
        }
    else
        {
        // Well, if Sat Server doesn't provide it,
        // then what values should we use in here??

        }
    // Append Buffer Size
    tlvSpecificData.AddTag( KTlvBufferSizeTag );
    tlvSpecificData.AddByte( TUint8( (aBufferSize&0xFF00)>>8 ));
    tlvSpecificData.AddByte( TUint8( aBufferSize&0x00FF ) );

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::GetChannelStatusTerminalResp
// Constructs Get Channel Status specific part of terminal
// response and calls SendTerminalResponse method to send it.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::GetChannelStatusTerminalResp
        (
        TUint8 aTransId,
        TDes8 &aCommandDetails,
        TUint8 aGeneralResult,
        const TDes8& aAdditionalInfo
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GETCHANNELSTATUSTERMINALRESP, "CSatMessHandler::GetChannelStatusTerminalResp" );
    TFLOGSTRING("CSatMessHandler::GetChannelStatusTerminalResp");
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult )
        || ( RSat::KInteractionWithCCPermanentError == aGeneralResult )
        || ( RSat::KErrorRequiredValuesMissing == aGeneralResult )
        || ( RSat::KBearerIndepProtocolError == aGeneralResult ) )
        {
        // UNSuccessful result neccessitating additional info byte
        tlvSpecificData.AddData( aAdditionalInfo );
        }
    else if ( RSat::KSuccess == aGeneralResult )
        {
        // Length check is carried out in
        // CSatNotifyGetChannelStatus::TerminalResponseL
        for ( TInt i=0; i<aAdditionalInfo.Length();i+=4 )
            {
            tlvSpecificData.AddTag( KTlvChannelStatusTag );
            tlvSpecificData.AddByte( aAdditionalInfo[i+1] );
            tlvSpecificData.AddByte( aAdditionalInfo[i+3] );
            }

        }
    else
        {
        TFLOGSTRING("CSatMessHandler::GetChannelStatusTerminalResp - mandatory channel status missing");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_GETCHANNELSTATUSTERMINALRESP, "CSatMessHandler::GetChannelStatusTerminalResp - mandatory channel status missing" );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::CloseChannelTerminalResp
// Constructs Close Channel specific part of terminal
// response and calls SendTerminalResponse method to send it.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::CloseChannelTerminalResp
        (
        TUint8 aTransId,
        TDes8 &aCommandDetails,
        TUint8 aGeneralResult,
        const TDes8& aAdditionalInfo
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_CLOSECHANNELTERMINALRESP, "CSatMessHandler::CloseChannelTerminalResp" );
    TFLOGSTRING("CSatMessHandler::CloseChannelTerminalResp");
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult )
        || ( RSat::KInteractionWithCCPermanentError == aGeneralResult )
        || ( RSat::KErrorRequiredValuesMissing == aGeneralResult )
        || ( RSat::KBearerIndepProtocolError == aGeneralResult ) )
        {
        // UNSuccessful result neccessitating additional info byte
        tlvSpecificData.AddData( aAdditionalInfo );
        }

    // Successful result: no additional info,
    // no parameters

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SendDataTerminalResp
// Constructs Send Data specific part of terminal
// response and calls SendTerminalResponse method to send it.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SendDataTerminalResp
        (
        TUint8 aTransId,
        TDes8 &aCommandDetails,
        TUint8 aGeneralResult,
        const TDes8& aAdditionalInfo,
        TUint8 aChannelDataLength
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDDATATERMINALRESP, "CSatMessHandler::SendDataTerminalResp" );
    TFLOGSTRING("CSatMessHandler::SendDataTerminalResp");
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );
    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult )
        || ( RSat::KInteractionWithCCPermanentError == aGeneralResult )
        || ( RSat::KErrorRequiredValuesMissing == aGeneralResult )
        || ( RSat::KBearerIndepProtocolError == aGeneralResult ) )
        {
        // UNSuccessful result neccessitating additional info byte
        tlvSpecificData.AddData( aAdditionalInfo );
        }
    else if (( RSat::KSuccess == aGeneralResult )
       || ( RSat::KSuccessRequestedIconNotDisplayed == aGeneralResult ))
        {
        // Successful result:
        // append Channel Status
        tlvSpecificData.AddTag( KTlvChannelDataLengthTag );
        tlvSpecificData.AddByte( aChannelDataLength );
        }
    else
        {
        TFLOGSTRING("TSY: CSatMessHandler::SendDataTerminalResp, General result did not match.");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_SENDDATATERMINALRESP, "CSatMessHandler::SendDataTerminalResp, General result did not match." );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::ReceiveDataTerminalResp
// Constructs Receive Data specific part of terminal
// response and calls SendTerminalResponse method to send it.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::ReceiveDataTerminalResp
        (
        TUint8 aTransId,
        TDes8& aCommandDetails,
        TUint8 aGeneralResult,
        const TDes8& aAdditionalInfo,
        TUint8 aChannelDataLength
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_RECEIVEDATATERMINALRESP, "CSatMessHandler::ReceiveDataTerminalResp" );
    TFLOGSTRING("CSatMessHandler::ReceiveDataTerminalResp");
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );
    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult )
        || ( RSat::KInteractionWithCCPermanentError == aGeneralResult )
        || ( RSat::KErrorRequiredValuesMissing == aGeneralResult )
        || ( RSat::KBearerIndepProtocolError == aGeneralResult ) )
        {
        // UNSuccessful result neccessitating additional info byte
        tlvSpecificData.AddData( aAdditionalInfo );
        }
    else if (( RSat::KSuccess == aGeneralResult )
    || ( RSat::KSuccessRequestedIconNotDisplayed == aGeneralResult )
    || ( RSat::KMissingInformation == aGeneralResult ))
        {
        // Successful result:
        //append Channel Data using additional info
        tlvSpecificData.AddTag( KTlvChannelDataTag );
        tlvSpecificData.AddData( aAdditionalInfo );
        // Channel data length
        tlvSpecificData.AddTag( KTlvChannelDataLengthTag );
        tlvSpecificData.AddByte( aChannelDataLength );
        }
    else
        {
        TFLOGSTRING("TSY: CSatMessHandler::ReceiveDataTerminalResp, General result did not match.");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_RECEIVEDATATERMINALRESP, "CSatMessHandler::ReceiveDataTerminalResp, General result did not match." );
        }

    return UiccCatReqTerminalResponse(
        aCommandDetails,
        tlvSpecificData.GetDataWithoutTopLevelTag(),
        aTransId );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::SendSmsResourceReq
// Send a response to SMS server after reception of SmsResourceInd.
// This response tells whether the SMS can be sent or not.
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SendSmsResourceReq
        (
        TUint8 aTransId,
        TDesC8& aData,
        TUint8 aReceiverObject
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDSMSRESOURCERESP, "CSatMessHandler::SendSmsResourceResp" );
    TFLOGSTRING("TSY:CSatMessHandler::SendSmsResourceResp");

    TIsiSend isimsg( iPnSend->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, aReceiverObject );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SMS );
    isimsg.Set8bit( ISI_HEADER_OFFSET_TRANSID , aTransId );
    isimsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, SMS_RESOURCE_REQ );
    isimsg.CopyData( ISI_HEADER_SIZE + SMS_RESOURCE_REQ_OFFSET_FILLERBYTE1,
                    aData );

    // add padding
    while ( iPnSend->SendBufferDes().Length() % 4 )
        {
        iPnSend->SendBufferDes().Append( 0x00 );
        }

    //send message
    return iPnSend->Send( isimsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SsStatusInd
// Break's a SS_STATUS_IND isi-message and stores status information
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SsStatusInd
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SSSTATUSIND, "CSatMessHandler::SsStatusInd" );
    TFLOGSTRING("TSY: CSatMessHandler::SsStatusInd");

    TUint8 ssStatus  = aIsiMessage.Get8bit( ISI_HEADER_SIZE
            + SS_STATUS_IND_OFFSET_SSSTATUSINDICATION );

    if( ( SS_STATUS_REQUEST_SERVICE_BUSY == ssStatus )
       || ( SS_STATUS_REQUEST_SERVICE_START == ssStatus  ) )
        {
        iSsStatus = ESsBusy;
        }
    else if( ( SS_GSM_STATUS_REQUEST_USSD_BUSY == ssStatus )
            || ( SS_GSM_STATUS_REQUEST_USSD_START == ssStatus  ) )
        {
        iSsStatus = EUssdBusy;
        }
    else
        {
        iSsStatus = ENotBusy;
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::AdditionalInfoNeeded
// Checks if given result requires an additional info byte(s)
// -----------------------------------------------------------------------------
//
TBool CSatMessHandler::AdditionalInfoNeeded
        (
        const TUint8 aGeneralResult
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_ADDITIONALINFONEEDED, "CSatMessHandler::AdditionalInfoNeeded" );
    // NOTE: This method shall not be used with SendSS proactive command as its
    // additional info handling differs from other commands.

    TBool ret( EFalse );

    switch ( aGeneralResult )
        {
        // This list is based on 3GPP TS 11.14 chapter 12.12
        case RSat::KMeUnableToProcessCmd:
        case RSat::KNetworkUnableToProcessCmd:
        case RSat::KLaunchBrowserError:
        case RSat::KSsReturnError:
        case RSat::KSmsRpError:
        case RSat::KUssdReturnError:
        case RSat::KMultipleCardCmdsError:
        case RSat::KInteractionWithCCPermanentError:
        case RSat::KBearerIndepProtocolError:
            {
            ret = ETrue;
            break;
            }
        default:
            {
            // do nothing. ret is already set to EFalse
            break;
            }
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler:: ReceiveMessageL
// Called by phonet receiver when an ISI message
// has been received
// -----------------------------------------------------------------------------
//
void CSatMessHandler::ReceiveMessageL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_RECEIVEMESSAGEL, "CSatMessHandler::ReceiveMessageL" );
    TFLOGSTRING("TSY: CSatMessHandler::ReceiveMessageL");

    // Get ISI message resource and id
    TInt resource ( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );

    // Received AtkSwDataNtf-message can be response for an envelope
    // or terminal response. Responses for TR and failed envelopes (SIM busy)
    // are handled in AtkServerMessageReceivedL. All other cases are passed
    // ProactiveProcedureMessageReceivedL-method
    TBool handled( EFalse );

    switch ( resource )
        {
        case PN_SS:
            {
            SsServerMessageReceived( aIsiMessage );
            break;
            }
        case PN_PHONE_INFO:
            {
            PhoneInfoMessageReceived( aIsiMessage );
            break;
            }
        case PN_MODEM_NETWORK:
            {
            NetServerMessageReceived( aIsiMessage );
            break;
            }
        case PN_GSS:
            {
            GsmStackServerMessageReceived( aIsiMessage );
            break;
            }
        case PN_SMS:
            {
            SmsServerMessageReceived( aIsiMessage );
            break;
            }
        case PN_UICC:
            {
            handled = UiccServerMessageReceived( aIsiMessage );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if ( !handled )
        {
        ProactiveProcedureMessageReceivedL( aIsiMessage );
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::HandleError
// Handles CSatMessHandler's errors that comes via PhoNetReceiver
// RunError method.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::HandleError
        (
        const TIsiReceiveC& /*aIsiMessage*/,
        TInt /*aErrorCode*/
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_HANDLEERROR, "CSatMessHandler::HandleError" );
    // No implementation
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::NetRatInd
// Breaks a NET_RAT_IND isi-message
// -----------------------------------------------------------------------------
//
void CSatMessHandler::NetRatInd
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_NETRATIND, "CSatMessHandler::NetRatInd" );
    TFLOGSTRING("TSY:CSatMessHandler::NetRatInd");

    // Check the presence of subblocks
    if ( 0 != aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_RAT_IND_OFFSET_SUBBLOCKCOUNT ) )
        {
        // Subblock offset
        TUint sbOffset( 0 );

        // NET_RAT_INFO sub block
        TInt ret = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_RAT_IND,
            NET_RAT_INFO,
            EIsiSubBlockTypeId8Len8,
            sbOffset );

        // Check if NET_RAT_INFO sub block is present
        if( KErrNone == ret )
            {
            // Get ratName
            TUint8 ratName( aIsiMessage.Get8bit(
                sbOffset + NET_RAT_INFO_OFFSET_RATNAME ) );

            switch ( ratName )
                {
                case NET_GSM_RAT:
                    {
                    iCurrentAccTech = KNetworkModeGsm;
                    iTsySatMessaging->GetEventDownload()
                    ->SendAccessTechnologyChangeEnvelope( KNetworkModeGsm );
                    break;
                    }
                case NET_UMTS_RAT:
                    {
                    iCurrentAccTech = KNetworkModeUtran;
                    iTsySatMessaging->GetEventDownload()
                    ->SendAccessTechnologyChangeEnvelope( KNetworkModeUtran );
                    break;
                    }
                default:
                    {
                    // Do nothing
                    break;
                    }
                }
            }
        TFLOGSTRING2("TSY:CSatMessHandler::NetRatInd - Current Acc Tech: %d",iCurrentAccTech);
        OstTraceExt1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_NETRATIND, "CSatMessHandler::NetRatInd - Current Acc Tech: %hhu", iCurrentAccTech );
        }

    }

// -----------------------------------------------------------------------------
// CSatMessHandler::LocationInfo
// Getter for location information structure
// -----------------------------------------------------------------------------
//
const CSatMessHandler::TLocationInfo& CSatMessHandler::LocationInfo()
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_LOCATIONINFO, "CSatMessHandler::LocationInfo" );
    // Extended Cell Id is provided only if both USIM and
    // 3G network are present.
    if ( KNetworkModeUtran != iCurrentAccTech
        || UICC_CARD_TYPE_UICC != iCardType )
        {
        // Exclude the extended Cell Id

        iLocInfo.iCellId.SetLength( 2 );
        }
    else
        {
        iLocInfo.iCellId.SetLength( 4 );
        }

    return iLocInfo;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SendPnAtkMessage
// Sends a PN_ATK message with the given data, sets also possible filler bytes
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::SendPnAtkMessage
        (
        const TUint8 aReceiverObject,
        const TUint8 aTransactionId,
        const TInt aMessageId,
        const TDesC8& aData
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SENDPNATKMESSAGE, "CSatMessHandler::SendPnAtkMessage" );
    TFLOGSTRING("TSY:CSatMessHandler::SendPnAtkMessage");

    TBuf8<KLengthTwoBytes> data;
    data.Append( aTransactionId );
    data.Append( aMessageId );

    // Create send buffer (sets the msg length to NULL)
    TIsiSend isimsg( iPnSend->SendBufferDes() );

    // Set the receiver object and resource ID
    isimsg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, aReceiverObject );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_ATK );
    // Set Transaction ID and Msg ID at the end of the message buffer
    isimsg.CopyData( ISI_HEADER_SIZE, data );
    // Copy the message data at the end of the message buffer
    isimsg.CopyData( ( ISI_HEADER_SIZE + KLengthTwoBytes ), aData );

    // Add padding(s)
    while ( iPnSend->SendBufferDes().Length() % 4 )
        {
        iPnSend->SendBufferDes().Append( 0x00 );
        }

    // Send message
    return iPnSend->Send( isimsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::UiccApplCmdResp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatMessHandler::UiccApplCmdResp( const TIsiReceiveC& aIsiMessage )
    {
    TFLOGSTRING("TSY: CSatMessHandler::UiccApplCmdResp");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCAPPLCMDRESP, "CSatMessHandler::UiccApplCmdResp" );

    TInt error( KErrNone );
    // Get transaction ID, status and service type
    TUint8 trId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_RESP_OFFSET_TRANSID ) );
    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_RESP_OFFSET_STATUS ) );
    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE +
        UICC_APPL_CMD_RESP_OFFSET_SERVICETYPE ) );

    // Card type
    iCardType = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_RESP_OFFSET_CARDTYPE );

    // Read file data from UICC_SB_FILE_DATA
    TInt fileDataLength( 0 );
    TPtrC8 fileData;
    TUint uiccSbFileDataOffset( 0 );
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_APPL_CMD_RESP,
        UICC_SB_FILE_DATA,
        EIsiSubBlockTypeId16Len16,
        uiccSbFileDataOffset ) )
        {
        fileDataLength = aIsiMessage.Get32bit(
            uiccSbFileDataOffset + UICC_SB_FILE_DATA_OFFSET_DATALENGTH );
        fileData.Set( aIsiMessage.GetData(
            uiccSbFileDataOffset + UICC_SB_FILE_DATA_OFFSET_DATA,
            fileDataLength ) );
        }
    else // Subblock is mandatory
        {
        TFLOGSTRING("TSY: CSatMessHandler::UiccApplCmdResp - Mandatory subblock UICC_SB_FILE_DATA not found");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_UICCAPPLCMDRESP, "CSatMessHandler::UiccApplCmdResp - Mandatory subblock UICC_SB_FILE_DATA not found" );
        error = KErrNotFound;
        }

    switch( trId )
        {
        // Call control, SMS PP DD, USIM MO-SMS control
        case KUiccTrIdServiceTableByte4:
            {
            if ( KErrNone == error )
                {
                TUint8 usedBitMaskSmsPpDd( KSmsPpDdBitMaskUsim );
                if ( UICC_CARD_TYPE_ICC == iCardType )
                    {
                    usedBitMaskSmsPpDd = KSmsPpDdBitMaskSim;
                    }
                else
                    {
                    // MO SMS control status, only in case of USIM
                    SimMoSmsControlAvail(
                        fileData[0] & KMoSmsControlBitMaskUsim );
                    }
                // Get status of SMS PP DD and report to DataDownload
                iTsySatMessaging->GetDataDownload()->SmsPpDlSupported(
                    ( fileData[0] & usedBitMaskSmsPpDd ) ? ETrue : EFalse );
                }
            break;
            }
        // MO SMS control ( this transaction ID is used for SIM only )
        case KUiccTrIdServiceTableByte5:
            {
            if ( KErrNone == error )
                {
                SimMoSmsControlAvail( fileData[0] & KMoSmsControlBitMaskSim );
                }
            break;
            }
        // USSD in Call Control ( this transaction ID is used for SIM only )
        case KUiccTrIdServiceTableByte6:
            {
            iTsySatMessaging->SetStatusOfUssdSupport(
                fileData[0] & KMoUssdCallControlBitMaskSim );
            break;
            }
        // SAT icon
        case KUiccTrIdReadSatIcon:
            {
            TBuf8<1> infoNotSet( 0 );
            iTsySatMessaging->GetSatIcon()->
                CompleteIconRequest( error, fileData, infoNotSet );
            break;
            }
        // SAT icon instance
        case KUiccTrIdReadIconInstancePhase1:
        case KUiccTrIdReadIconInstancePhase2:
        case KUiccTrIdReadIconClut:
            {
            if ( KUiccTrIdReadIconInstancePhase1 == trId )
                {
                // See image instance data context coding from
                // 3GPP 31.102 v8.3.0. ANNEX B
                // Save image coding scaheme
                iImageCodingScheme = fileData[2];
                // File ID of instance is in bytes 4 and 5
                // Read instance file in phase 2
                TUint16 fileId( ( fileData[3] << 8 ) | fileData[4] );
                UiccReadSatIconInstanceReq( fileId );
                }
            else // Instance data and clut are set here
                {
                TPtrC8 data;
                TBuf8<6> info;
                TUint8 width( fileData[0] );
                TUint8 heigth( fileData[1] );
                TUint8 colorDepth( 1 );
                TUint16 clutOffset( 0 );
                TUint offset( 2 ); // instance data offset in basic coding

                // Set in case of coulor image coding
                iNumberOfClut = 0;

                info.Append( width );
                info.Append( heigth );

                // Colour image coding scheme
                if ( KBlackAndWhite != iImageCodingScheme )
                    {
                    colorDepth = fileData[2];
                    iNumberOfClut = fileData[3];
                    clutOffset = ( fileData[4] << 8 | fileData[5] );
                    offset = 6;

                    // Save clut data for clut request.
                    // So it is not needed to read instance file from UICC
                    // again.
                    TInt clutLength( KClutEntrySize * iNumberOfClut );
                    if ( 0 == clutLength )
                        {
                        clutLength = KClutEntrySize * 256;
                        }
                    iClutData.Set( fileData.Mid( clutOffset, clutLength ));
                    }

                info.Append( colorDepth );
                info.Append( iNumberOfClut );
                info.Append( 0x00 );  // offset (default value added)
                info.Append( 0x00 );  // offset (default value added)

                TUint lengthInBits( width * heigth * colorDepth );
                TUint lengthInBytes( lengthInBits / 8 );

                // There might be rounding error in statement above, if
                // length in bits is not evenly divisible by 8 ( one byte ).
                if ( lengthInBits % 8 )
                    {
                    lengthInBytes++;
                    }

                data.Set( fileData.Mid( offset, lengthInBytes ) );
                iTsySatMessaging->GetSatIcon()->
                    CompleteIconRequest( error, data, info);
                }
            break;
            }
        default:
            {
            break;
            }
        } // End of switch( trId )
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccCatInd
// Breaks UICC_CAT_IND ISI-message.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::UiccCatInd
    (
    const TIsiReceiveC& aIsiMessage // Received ISI message
    )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCCATIND, "CSatMessHandler::UiccCatInd" );
    TFLOGSTRING("TSY: CSatMessHandler::UiccCatInd");

    // UICC status
    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_CAT_IND_OFFSET_SERVICETYPE ) );

    if ( UICC_READY == serviceType )
        {
        // Card type
        iCardType = aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + UICC_CAT_IND_OFFSET_CARDTYPE );

        // Application file ID is concluded from card type
        if ( UICC_CARD_TYPE_UICC == iCardType )
            {
            // ADF USIM is used, ID = '7FFF'
            TBuf8<2> umtsFileId;
            umtsFileId.Append( 0x7F );
            umtsFileId.Append( 0xFF );
            iApplFileId.Copy( umtsFileId );
            }
        else if ( UICC_CARD_TYPE_ICC == iCardType )
            {
            // DF GSM is used, ID = '7F20'
            TBuf8<2> gsmFileId;
            gsmFileId.Append( 0x7F );
            gsmFileId.Append( 0x20 );
            iApplFileId.Copy( gsmFileId );
            }
        // No else, initialized to 'FFFF' in constructor.

        // Send terminal profile
        UiccTerminalProfileReq();
        }
    // Proactive command was fetched from SIM
    else if ( UICC_CAT_FETCHED_CMD == serviceType )
        {
        if ( KErrNone == CheckProactiveCommand( aIsiMessage ) )
            {
            iTsySatMessaging->PCmdReceivedL( aIsiMessage );
            }
        else
            {
            TFLOGSTRING("TSY: Received SAT message was not valid!");
            OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_UICCCATIND, "CSatMessHandler::UiccCatInd. Received SAT message was not valid!" );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::InfoPpReadReq
// Construct a INFO_PP_READ_REQ ISI message
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::InfoPpReadReq
        (
        const TUint8 aReqType // request type
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_INFOPPREADREQ, "CSatMessHandler::InfoPpReadReq" );
    TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadReq" );

    // Construct INFO_PP_READ_REQ message (INFO_BATCH_TYPE +
    // sub block count + INFO_SB_PP sub block)
    TBuf8<2 + SIZE_INFO_SB_PP> infoPpReadReq;

    //set pp batch reading to read specific product profile, thus 0.
    infoPpReadReq.Append( INFO_PP_PROD_PROFILE_FEATURE );

    // add sub block
    infoPpReadReq.Append( 1 ); // number of sub blocks

    // create INFO_SB_PP sub block
    TIsiSubBlock sbPPSubBlock( infoPpReadReq, INFO_SB_PP,
                                EIsiSubBlockTypeId8Len8 );
    // Add 0x00 as the Dynamic Value ID, used to read a static value
    infoPpReadReq.Append( 0x00 );

    // Add 0x01 as the length of the product profile, reading only one profile
    infoPpReadReq.Append( 0x01 );

    // Add the product profile feature number being requested
    infoPpReadReq.Append( aReqType );

    // Value of the product profile is 0x00, since reading
    infoPpReadReq.Append( 0x00 );

    // complete sub block and append to INFO_PP_READ_REQ
    sbPPSubBlock.CompleteSubBlock();

    TUint8 tempTraId( iTsySatMessaging->GetTransactionId() );

#ifdef INFO_PP_ATK_MO_SM_CONTROL
    if ( INFO_PP_ATK_MO_SM_CONTROL == aReqType )
        {
        // Save transaction id to separate actions in InfoPpReadResp.
        // INFO_PP_ATK_MO_SM_CONTROL is the only PP flag which causes actions
        // if response is unsuccessful and there can be several simultaneous
        // PP queries ongoing
        iInfoPpMoSmsTraId = tempTraId;
        }
#endif

    //create the isi message
    return iPnSend->Send(
        PN_INFO,
        tempTraId,
        INFO_PP_READ_REQ,
        infoPpReadReq );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::InfoPpReadResp
// Breaks a INFO_PP_READ_RESP ISI-message.
// -----------------------------------------------------------------------------
//
void CSatMessHandler::InfoPpReadResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp" );
    TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp" );

    TUint8 traId( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                            INFO_PP_READ_RESP_OFFSET_TRANSID ) );

    TUint8 status( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                            INFO_PP_READ_RESP_OFFSET_STATUS ) );

    TFLOGSTRING2("TSY: CSatMessHandler::InfoPpReadResp - Status: %d",
        status );
    OstTraceExt1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp - Status: %hhu", status );


    switch ( status )
        {
        case INFO_OK:
            {
            TUint sbInfoPpReadStartOffset( 0 );

            if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_INFO_PP_READ_RESP, INFO_SB_PP,
                EIsiSubBlockTypeId8Len8, sbInfoPpReadStartOffset ) )
                {
                TInt length( aIsiMessage.Get8bit( sbInfoPpReadStartOffset +
                            INFO_SB_PP_OFFSET_STRLEN ) );

                if ( length )
                    {
                    // Buffer to hold product profile feature and its
                    // corresponding value, thus buffer size is 2
                    TBuf8<2> pPFeatures;

                    // copy 2 bytes from product profile
                    // 1st byte = PP Feature
                    // 2nd byte = PP Value
                    pPFeatures.Copy( aIsiMessage.GetData(
                        sbInfoPpReadStartOffset +
                        INFO_SB_PP_OFFSET_PRODUCTPROFILE, 2 ) );

#ifdef INFO_PP_ATK_MO_SM_CONTROL
                    // Check that product profile is requested one
                    if ( INFO_PP_ATK_MO_SM_CONTROL == pPFeatures [0] )
                        {
                        //Get the value of the MO SMS control feature
                        if ( INFO_PP_FALSE != pPFeatures [1] )
                            {
                            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, MO SMS control supported by ME product profile" );
                            OstTrace0( TRACE_NORMAL, DUP2_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, MO SMS control supported by ME product profile" );
                            iMoSmsSupportedInPp = ETrue;
                            // Continue MO SMS control check by checking
                            // feature availability from USIM
                            if ( UICC_CARD_TYPE_UICC == iCardType )
                                {
                                // USIM: Read MO-SMS control from byte 4
                                UiccReadServiceTableReq( KUiccTrIdServiceTableByte4, 5 );
                                }
                            else if ( UICC_CARD_TYPE_ICC == iCardType )
                                {
                                // SIM: Read MO-SMS control from byte 5
                                UiccReadServiceTableReq( KUiccTrIdServiceTableByte5, 6 );
                                }
                            }
                        else
                            {
                            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, MO SMS control not supported by ME product profile" );
                            OstTrace0( TRACE_NORMAL, DUP3_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, MO SMS control not supported by ME product profile" );
                            iMoSmsSupportedInPp = EFalse;
                            // Feature was not activated in product profile
                            // Set MoSmsCtrl object´s member
                            // iIsMoSmsCtrlActivated to EFalse
                            iTsySatMessaging->GetMoSmsCtrl()->Deactivate();
                            }
                        }
#endif // INFO_PP_ATK_MO_SM_CONTROL

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
                    if ( INFO_PP_ENHANCED_NETWORK_SELECTION == pPFeatures [0] )
                        {
                        //Get the value of the ENS feature
                        if ( INFO_PP_FALSE != pPFeatures [1] )
                            {
                            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, INFO_PP_ENHANCED_NETWORK_SELECTION supported by ME product profile" );
                            OstTrace0( TRACE_NORMAL, DUP8_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, INFO_PP_ENHANCED_NETWORK_SELECTION supported by ME product profile" );
                            iEnsSupportedInPp = ETrue;
                            }
                        else
                            {
                            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, INFO_PP_ENHANCED_NETWORK_SELECTION not supported by ME product profile" );
                            OstTrace0( TRACE_NORMAL, DUP9_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, INFO_PP_ENHANCED_NETWORK_SELECTION not supported by ME product profile" );
                            iEnsSupportedInPp = EFalse;
                            }
                        }
#endif // INFO_PP_ENHANCED_NETWORK_SELECTION

#ifdef INFO_PP_SIM_OLD_POLL_INTERVAL
                    if ( INFO_PP_SIM_OLD_POLL_INTERVAL == pPFeatures [0] )
                        {
                        //Get the value of the SIM_POLL_INTERVAL control feature
                        if ( INFO_PP_FALSE != pPFeatures [1] )
                            {
                            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, INFO_PP_SIM_OLD_POLL_INTERVAL supported by ME product profile" );
                            OstTrace0( TRACE_NORMAL, DUP4_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, INFO_PP_SIM_OLD_POLL_INTERVAL supported by ME product profile" );
                            iOldPollIntervalSupportedInPp = ETrue;
                            }
                        else
                            {
                            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, INFO_PP_SIM_OLD_POLL_INTERVAL not supported by ME product profile" );
                            OstTrace0( TRACE_NORMAL, DUP5_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, INFO_PP_SIM_OLD_POLL_INTERVAL not supported by ME product profile" );
                            iOldPollIntervalSupportedInPp = EFalse;
                            }
                        }
#endif // INFO_PP_SIM_OLD_POLL_INTERVAL
                    //no else
                    }
                }
            break;
            }
        case INFO_NO_NUMBER:
            {
            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, Requested feature not defined in product profile" );
            OstTrace0( TRACE_NORMAL, DUP6_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, Requested feature not defined in product profile" );
#ifdef INFO_PP_ATK_MO_SM_CONTROL
            // Request was for INFO_PP_ATK_MO_SM_CONTROL
            if ( traId == iInfoPpMoSmsTraId )
                {
                // If the MO SMS feature is not defined in product profile,
                // internal Boolean used as feature supported ( for refresh )
                iMoSmsSupportedInPp = ETrue;
                // Continue as usual by checking feature avalability from USIM
                if ( UICC_CARD_TYPE_UICC == iCardType )
                    {
                    // USIM: Read MO-SMS control from byte 4
                    UiccReadServiceTableReq( KUiccTrIdServiceTableByte4, 5 );
                    }
                else if ( UICC_CARD_TYPE_ICC == iCardType )
                    {
                    // SIM: Read MO-SMS control from byte 5
                    UiccReadServiceTableReq( KUiccTrIdServiceTableByte5, 6 );
                    }
                }
#endif // INFO_PP_ATK_MO_SM_CONTROL

            break;
            }
        case INFO_FAIL:
        default:
            {
            TFLOGSTRING("TSY: CSatMessHandler::InfoPpReadResp, - default Requested feature not supported by ME product profile" );
            OstTrace0( TRACE_NORMAL, DUP7_CSATMESSHANDLER_INFOPPREADRESP, "CSatMessHandler::InfoPpReadResp, - default Requested feature not supported by ME product profile" );
#ifdef INFO_PP_ATK_MO_SM_CONTROL
            // Request was for INFO_PP_ATK_MO_SM_CONTROL
            if ( traId == iInfoPpMoSmsTraId )
                {
                iMoSmsSupportedInPp = EFalse;
                iTsySatMessaging->GetMoSmsCtrl()->Deactivate();
                }
#endif // INFO_PP_ATK_MO_SM_CONTROL

            break;
            }
        }
    // Zero iInfoPpMoSmsTraId if used. Not used by default.
    if ( traId == iInfoPpMoSmsTraId )
        {
        iInfoPpMoSmsTraId = 0;
        }
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::CheckTlvObjects
// Validates every simple tlv object from proactive command
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::CheckTlvObjects( CBerTlv& berTlv )
    {
    TFLOGSTRING("TSY: CSatMessHandler::CheckTlvObjects" );
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_CHECKTLVOBJECTS, "CSatMessHandler::CheckTlvObjects" );

    TInt ret( KErrNone );
    TPtrC8 dataPtr = berTlv.Data();
    TUint16 berTlvLength( berTlv.GetLength() );
    TInt totalLength = 0;

    // index is set to beginning of data inside BER-TLV object. If length
    // of BER-TLV object is coded as one byte, data starts from index 2
    // and if length of BER-TLV object is coded as two bytes, data starts
    // from index 3.
    TInt index = 2;

    if( KTwoByteLengthCoding == dataPtr[KTlvLengthStartPosition] )
        {
        // data len is coded as two bytes, let's increment these variables
        // so that two byte len is handled correctly
        index++;
        }

    while( berTlvLength > ( index - 2 ) )
        {
        // now index points to tag of simple TLV object, let's increment
        // index by one so that it points to length of the simple TLV object
        index++;

        // totalLength is incremented by 2 so that tag & len fields of the current
        // simple tlv object are calcucated to total length of BER TLV object
        totalLength += 2;

        if( KTwoByteLengthCoding == dataPtr[index] )
            {
            // data len is coded as two bytes, let's increment these variables
            // so that two byte len is handled correctly
            index++;
            totalLength++;

            // if length of the simple TLV value is coded with two bytes,
            // length of the simple TLV value must be 128-255, otherwise
            // it is treaded as error
            if( 0x80 > dataPtr[index] || 0xFF < dataPtr[index] )
                {
                TFLOGSTRING2("TSY: CSatMessHandler::CheckTlvObjects: corrupted simple tlv obj, len: %d even it should be 128-255", dataPtr[index] );
OstTrace1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_CHECKTLVOBJECTS, "CSatMessHandler::CheckTlvObjects; corrupted simple tlv obj, len: %d even it should be 128-255", dataPtr[index] );
                ret = KErrCorrupt;
                break;
                }
            }

        else
            {
            // if length of the simple TLV value is coded with one byte,
            // length of the simple TLV value must be 0-127, otherwise
            // it is treaded as error
            if( 0x80 <= dataPtr[index] )
                {
                TFLOGSTRING2("TSY: CSatMessHandler::CheckTlvObjects: corrupted simple tlv obj, len: %d even it should be 0-127", dataPtr[index] );
OstTrace1( TRACE_NORMAL, DUP2_CSATMESSHANDLER_CHECKTLVOBJECTS, "CSatMessHandler::CheckTlvObjects; corrupted simple tlv obj, len: %d even it should be 0-127", dataPtr[index] );
                ret = KErrCorrupt;
                break;
                }
            }

        // total length is incremented by data len of the current simple
        // tlv object
        totalLength += dataPtr[index];

        // let's move index to beginning of next simple TLV object. So
        // after incrementing the index, it points to next simple TLV
        // object's tag value
        index += dataPtr[index] + 1;
        } // end of while

    // let's check does length from proactive command match to
    // calculated length
    if( berTlvLength != totalLength )
        {
        TFLOGSTRING3("TSY: CSatMessHandler::CheckTlvObjects: proactive command len (%d) and calculated len (%d) doesn't match", berTlvLength, totalLength );
OstTraceExt2( TRACE_NORMAL, DUP3_CSATMESSHANDLER_CHECKTLVOBJECTS, "CSatMessHandler::CheckTlvObjects; proactive command len (%d) and calculated len (%d) doesn't match", berTlvLength, totalLength );
        ret = KErrCorrupt;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::SimMoSmsControlAvail
// Activate or deactivate MO SMS Control on SMS Server
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SimMoSmsControlAvail( TUint8 aStatus )
    {
    TFLOGSTRING("TSY: CSatMessHandler::SimMoSmsControlAvail" );
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SIMMOSMSCONTROLAVAIL, "CSatMessHandler::SimMoSmsControlAvail" );

    // Activate the MO-SMS Control in SMS server. This is done by
    // sending a request SMS_RESOURCE_CONF_REQ to SMS server
    TBuf8<SIZE_SMS_RESOURCE_CONF_REQ + SIZE_SMS_SB_RESOURCE_CONF> data;
    // Append Confoguration operation
    data.Append( SMS_RES_CONF_SET );
    // Number of Subblocks
    data.Append( 1 );
    // Add Subblock
    TIsiSubBlock ResourceConfReqSb(
        data,
        SMS_SB_RESOURCE_CONF,
        EIsiSubBlockTypeId16Len16 );

    TSatUtility::AppendWord( SMS_RES_ID_MO_SM_INIT, data );

    if( aStatus )
        {
        // Set MoSmsCtrl object´s member iIsMoSmsCtrlActivated to ETrue
        iTsySatMessaging->GetMoSmsCtrl()->Activate();
        TSatUtility::AppendWord( SMS_RES_ID_MASK_MO_SM_INIT, data);
        }
    else
        {
        // Set MoSmsCtrl object´s member IsMoSmsCtrlDeActivated to EFalse
        iTsySatMessaging->GetMoSmsCtrl()->Deactivate();
        TSatUtility::AppendWord( 0x0000, data);
        }
    ResourceConfReqSb.CompleteSubBlock();
    SmsResoureConfReq(
        iTsySatMessaging->GetTransactionId(),
        SMS_RESOURCE_CONF_REQ,
        data );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccTerminalProfileReq
// Send terminal profile
// -----------------------------------------------------------------------------
//
void CSatMessHandler::UiccTerminalProfileReq()
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCTERMINALPROFILEREQ, "CSatMessHandler::UiccTerminalProfileReq" );
    TFLOGSTRING("TSY: CSatMessHandler::UiccTerminalProfileReq");

    // Pointer to terminal profile buffer
    const TUint8* terminalProfilePtr( NULL );
    // Size of terminal profile
    TInt sizeofTerminalProfile( 0 );

    if ( UICC_CARD_TYPE_UICC == iCardType )
        {
        terminalProfilePtr = &KTerminalProfileUicc[0];
        sizeofTerminalProfile = sizeof( KTerminalProfileUicc );
        }
    else // ICC type
        {
        terminalProfilePtr = &KTerminalProfileIcc[0];
        sizeofTerminalProfile = sizeof( KTerminalProfileIcc );
        }

    // Create UICC_CAT_REQ message
    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_TRANSID,
        KUiccTrIdCommon );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_MESSAGEID,
        UICC_CAT_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_SERVICETYPE,
        UICC_CAT_TERMINAL_PROFILE );
    isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_NSB, 1 );

    // Create UICC_SB_TERMINAL_PROFILE subblock
    TBuf8<KSizeOfTerminalProfileSb> terminalProfileBuf( 0 );
    TIsiSubBlock uiccSbTerminalProfile(
        terminalProfileBuf,
        UICC_SB_TERMINAL_PROFILE,
        EIsiSubBlockTypeId16Len16 );

    // 2x filler
    terminalProfileBuf.Append( KUiccPadding );
    terminalProfileBuf.Append( KUiccPadding );

    // Terminal profile length (16-bit)
    terminalProfileBuf.Append( 0 );
    terminalProfileBuf.Append( sizeofTerminalProfile );

    // Terminal profile
    terminalProfileBuf.Append( terminalProfilePtr, sizeofTerminalProfile );

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_REQ,
        uiccSbTerminalProfile.CompleteSubBlock() );

    iPnSend->Send( isiMsg.Complete() );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccReadServiceTable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::UiccReadServiceTableReq(
    TUint8 aTrId,
    TUint16 aFileOffset )
    {
    TFLOGSTRING2("TSY: CSatMessHandler::UiccReadServiceTableReq, transaction ID: %d", aTrId );
    OstTraceExt1( TRACE_NORMAL, CSATMESSHANDLER_UICCREADSERVICETABLE, "CSatMessHandler::UiccReadServiceTableReq;aTrId=%hhu", aTrId );

    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_TRANSID,
        aTrId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_MESSAGEID,
        UICC_APPL_CMD_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SERVICETYPE,
        UICC_APPL_READ_TRANSPARENT );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_APPLID,
        iApplicationId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SESSIONID,
        UICC_SESSION_ID_NOT_USED );
    isiMsg.Set16bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_FILLERBYTE1,
        KUiccPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_NSB,
        KNumOfSbsInUiccApplCmdReq );

    // UICC_SB_CLIENT
    TBuf8<SIZE_UICC_SB_CLIENT> uiccSbClientBuf( 0 );
    TIsiSubBlock uiccSbClient(
        uiccSbClientBuf,
        UICC_SB_CLIENT,
        EIsiSubBlockTypeId16Len16 );
    // 3x padding
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( iUiccClientId );
    // Append subblock to ISI message
    isiMsg.CopyData( KUiccApplCmdReqOffset, uiccSbClient.CompleteSubBlock() );

    // UICC_SB_TRANSPARENT
    TBuf8<SIZE_UICC_SB_TRANSPARENT> uiccSbTransparentBuf( 0 );
    TIsiSubBlock uiccSbTransparent(
        uiccSbTransparentBuf,
        UICC_SB_TRANSPARENT,
        EIsiSubBlockTypeId16Len16 );
    // File offset defines which service is read, 16-bit.
    // Service table is max. 10 bytes so MSB bytes are set to 0
    uiccSbTransparentBuf.Append( 0 );
    uiccSbTransparentBuf.Append( aFileOffset );
    // Data amount, 16-bit
    // Read only one byte -> MSB bits are set to 0
    uiccSbTransparentBuf.Append( 0 );
    uiccSbTransparentBuf.Append( 1 );
    // Append subblock to ISI message
    isiMsg.CopyData(
        KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT,
        uiccSbTransparent.CompleteSubBlock() );

    // UICC_SB_APPL_PATH
    TBuf8<KUiccSbApplPathSize> uiccSbApplPathBuf( 0 );
    TIsiSubBlock uiccSbApplPath(
        uiccSbApplPathBuf,
        UICC_SB_APPL_PATH,
        EIsiSubBlockTypeId16Len16 );
    // File ID is '6F38', EF UST
    uiccSbApplPathBuf.Append( KUiccElemFileServiceTable >> 8 );
    uiccSbApplPathBuf.Append( KUiccElemFileServiceTable );
    // File ID SFI, 8-bit
    uiccSbApplPathBuf.Append( KUiccSfiServiceTable );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Length of the DF path, 8-bit
    uiccSbApplPathBuf.Append( KUiccFilePathLengthServiceTable );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Application file path:
    // master file ID + application file ID
    uiccSbApplPathBuf.Append( KUiccMasterFileId >> 8 );
    uiccSbApplPathBuf.Append( KUiccMasterFileId );
    uiccSbApplPathBuf.Append( iApplFileId );

    // Append subblock to ISI message
    isiMsg.CopyData(
        KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT + SIZE_UICC_SB_TRANSPARENT,
        uiccSbApplPath.CompleteSubBlock() );

    return( iPnSend->Send( isiMsg.Complete() ) );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccReadSatIconReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::UiccReadSatIconReq(
    TUint8 aRecordNumber,
    TUint8 aFileOffset,
    TUint8 aDataAmount,
    TUint8 aTrId )
    {
    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_TRANSID,
        aTrId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_MESSAGEID,
        UICC_APPL_CMD_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SERVICETYPE,
        UICC_APPL_READ_TRANSPARENT );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_APPLID,
        iApplicationId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SESSIONID,
        UICC_SESSION_ID_NOT_USED );
    isiMsg.Set16bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_FILLERBYTE1,
        KUiccPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_NSB,
        KNumOfSbsInUiccApplCmdReq );

    // UICC_SB_CLIENT
    TBuf8<SIZE_UICC_SB_CLIENT> uiccSbClientBuf( 0 );
    TIsiSubBlock uiccSbClient(
        uiccSbClientBuf,
        UICC_SB_CLIENT,
        EIsiSubBlockTypeId16Len16 );
    // 3x padding
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( iUiccClientId );
    // Append subblock to ISI message
    isiMsg.CopyData( KUiccApplCmdReqOffset, uiccSbClient.CompleteSubBlock() );

    // UICC_SB_LINEAR_FIXED
    TBuf8<SIZE_UICC_SB_LINEAR_FIXED> uiccSbLinearFixedBuf( 0 );
    TIsiSubBlock uiccSbLinearFixed(
        uiccSbLinearFixedBuf,
        UICC_SB_LINEAR_FIXED,
        EIsiSubBlockTypeId16Len16 );
    // Record number, 8-bit
    uiccSbLinearFixedBuf.Append( aRecordNumber );
    // File offset, 8-bit
    uiccSbLinearFixedBuf.Append( aFileOffset );
    // Data amount, 8-bit
    uiccSbLinearFixedBuf.Append( aDataAmount );
    // Append subblock to ISI message
    isiMsg.CopyData(
        KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT,
        uiccSbLinearFixed.CompleteSubBlock() );

    // UICC_SB_APPL_PATH
    TBuf8<KUiccSbApplPathSize> uiccSbApplPathBuf( 0 );
    TIsiSubBlock uiccSbApplPath(
        uiccSbApplPathBuf,
        UICC_SB_APPL_PATH,
        EIsiSubBlockTypeId16Len16 );
    // File ID is '4F20', EF IMG
    uiccSbApplPathBuf.Append( KUiccElemFileImg >> 8 );
    uiccSbApplPathBuf.Append( KUiccElemFileImg );
    // File ID SFI, 8-bit
    uiccSbApplPathBuf.Append( UICC_SFI_NOT_PRESENT );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Length of the DF path, 8-bit
    uiccSbApplPathBuf.Append( KUiccFilePathLength );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Application file path:
    // master file + application file + dedicated file +
    // elementary file
    uiccSbApplPathBuf.Append( KUiccMasterFileId >> 8 );
    uiccSbApplPathBuf.Append( KUiccMasterFileId );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileTelecom >> 8 );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileTelecom );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileGraphics >> 8 );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileGraphics );

    // Append subblock to ISI message
    isiMsg.CopyData(
        KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
        SIZE_UICC_SB_LINEAR_FIXED,
        uiccSbApplPath.CompleteSubBlock() );

    return( iPnSend->Send( isiMsg.Complete() ) );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccReadSatIconInstanceReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::UiccReadSatIconInstanceReq( TUint16 aFileId )
    {
    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_TRANSID,
        KUiccTrIdReadIconInstancePhase2 );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_MESSAGEID,
        UICC_APPL_CMD_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SERVICETYPE,
        UICC_APPL_READ_TRANSPARENT );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_APPLID,
        iApplicationId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SESSIONID,
        UICC_SESSION_ID_NOT_USED );
    isiMsg.Set16bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_FILLERBYTE1,
        KUiccPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_NSB,
        KNumOfSbsInUiccApplCmdReq );

    // UICC_SB_CLIENT
    TBuf8<SIZE_UICC_SB_CLIENT> uiccSbClientBuf( 0 );
    TIsiSubBlock uiccSbClient(
        uiccSbClientBuf,
        UICC_SB_CLIENT,
        EIsiSubBlockTypeId16Len16 );
    // 3x padding
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( iUiccClientId );
    // Append subblock to ISI message
    isiMsg.CopyData( KUiccApplCmdReqOffset, uiccSbClient.CompleteSubBlock() );

    // UICC_SB_LINEAR_FIXED
    TBuf8<SIZE_UICC_SB_TRANSPARENT> uiccSbTransparentBuf( 0 );
    TIsiSubBlock uiccSbTransparent(
        uiccSbTransparentBuf,
        UICC_SB_TRANSPARENT,
        EIsiSubBlockTypeId16Len16 );
    // File offset, 8-bit
    uiccSbTransparentBuf.Append( 0 );
    // Data amount, 8-bit
    uiccSbTransparentBuf.Append( 0 );
    // Append subblock to ISI message
    isiMsg.CopyData(
        KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT,
        uiccSbTransparent.CompleteSubBlock() );

    // UICC_SB_APPL_PATH
    TBuf8<KUiccSbApplPathSize> uiccSbApplPathBuf( 0 );
    TIsiSubBlock uiccSbApplPath(
        uiccSbApplPathBuf,
        UICC_SB_APPL_PATH,
        EIsiSubBlockTypeId16Len16 );
    // File ID is '4F20', EF IMG
    uiccSbApplPathBuf.Append( aFileId >> 8 );
    uiccSbApplPathBuf.Append( aFileId );
    // File ID SFI, 8-bit
    uiccSbApplPathBuf.Append( UICC_EF_ID_NOT_PRESENT );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Length of the DF path, 8-bit
    uiccSbApplPathBuf.Append( KUiccFilePathLength );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Application file path:
    // master file + application file + dedicated file +
    // elementary file
    uiccSbApplPathBuf.Append( KUiccMasterFileId >> 8 );
    uiccSbApplPathBuf.Append( KUiccMasterFileId );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileTelecom >> 8 );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileTelecom );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileGraphics >> 8 );
    uiccSbApplPathBuf.Append( KUiccDedicatedFileGraphics );

    // Append subblock to ISI message
    isiMsg.CopyData(
        KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
        SIZE_UICC_SB_TRANSPARENT,
        uiccSbApplPath.CompleteSubBlock() );

    return( iPnSend->Send( isiMsg.Complete() ) );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccCatReq
// Create and send UICC_CAT_REQ ISI message
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::UiccCatReq( TUint8 aCommand )
    {
    TFLOGSTRING("TSY: CSatMessHandler::UiccCatReq");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCCATREQ, "CSatMessHandler::UiccCatReq" );

    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_TRANSID,
        KUiccTrIdCommon );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_MESSAGEID,
        UICC_CAT_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_SERVICETYPE,
        aCommand );

    // No subblocks
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_NSB,
        0 );

    return( iPnSend->Send( isiMsg.Complete() ) );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccCatReqRefresh
// Create and send UICC_CAT_REQ ISI message for refresh command
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::UiccCatReqRefresh(
    const TUint8 aTransId,        // Transaction ID
    const TUint8 aServiceType,    // Type of refresh
    TDes8& aFileList,             // List of files
    const TDesC8& aAid )          // Application ID (USIM)
    {
    TFLOGSTRING("TSY: CSatMessHandler::UiccCatReqRefresh");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCCATREQREFRESH, "CSatMessHandler::UiccCatReqRefresh" );

    TUint8 numOfSubblocks( 1 ); // One mandatory subblock UICC_SB_REFRESH
    TUint8 uiccSbOffset(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_REQ + SIZE_UICC_SB_REFRESH );

    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_TRANSID,
        aTransId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_MESSAGEID,
        UICC_CAT_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_SERVICETYPE,
        UICC_CAT_REFRESH );

    // Add UICC_SB_REFRESH
    TBuf8<SIZE_UICC_SB_REFRESH> uiccSbRefreshBuf( 0 );
    TIsiSubBlock uiccSbRefresh(
        uiccSbRefreshBuf,
        UICC_SB_REFRESH,
        EIsiSubBlockTypeId16Len16 );
    // Type of refresh
    uiccSbRefreshBuf.Append( aServiceType );
    // 3x filler
    uiccSbRefreshBuf.Append( KUiccPadding );
    uiccSbRefreshBuf.Append( KUiccPadding );
    uiccSbRefreshBuf.Append( KUiccPadding );
    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_REQ,
        uiccSbRefresh.CompleteSubBlock() );

    // Add UICC_SB_AID if needed
    if ( aAid.Length() )
        {
        numOfSubblocks++;
        TBuf8<KMaxLengthUiccSbAid> uiccSbAidBuf( 0 );
        TIsiSubBlock uiccSbAid(
            uiccSbAidBuf,
            UICC_SB_AID,
            EIsiSubBlockTypeId16Len16 );
        // AID length
        uiccSbAidBuf.Append( aAid.Length() );
        // Add AID
        uiccSbAidBuf.Append( aAid );
        // Append subblock to ISI message
        isiMsg.CopyData( uiccSbOffset, uiccSbAid.CompleteSubBlock() );

        // Calculate offset for next subblock
        TUint16 lengthOfUiccSbAid( uiccSbAidBuf.Length() ); // Length of UICC_SB_AID
        uiccSbOffset += lengthOfUiccSbAid;
        }

    // Add UICC_SB_APPL_PATH subblock(s) if needed
    while ( aFileList.Length() )
        {
        numOfSubblocks++;
        // Search last file's start position
        TInt position( aFileList.LocateReverse( KRefreshMasterFileHeader ) );
        if ( KErrNotFound == position )
            {
            // File path is corrupted
            TFLOGSTRING("TSY: CSatMessHandler::SimAtkReq, File path is corrupted");
            OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_UICCCATREQREFRESH, "CSatMessHandler::UiccCatReqRefresh,File path is corrupted" );
            break;
            }

        // Length of file path
        TUint8 length( (TUint8) ( aFileList.Length() - position ) );
        // File path
        TBuf8<KUiccFilePathLength> filePath(
            aFileList.Mid( position, length ) );
        // File ID is two last bytes
        TBuf8<KUiccFileIdLength> fileId(
            filePath.Mid( filePath.Length() - 2 ) );

        TBuf8<SIZE_UICC_SB_APPL_PATH + KUiccFilePathLength>
            uiccSbApplPathBuf( 0 );
        TIsiSubBlock uiccSbApplPath(
            uiccSbApplPathBuf,
            UICC_SB_APPL_PATH,
            EIsiSubBlockTypeId16Len16 );
        // Append file ID
        uiccSbApplPathBuf.Append( fileId );
        // File ID SFI is not known
        uiccSbApplPathBuf.Append( UICC_SFI_NOT_PRESENT );
        // Filler
        uiccSbApplPathBuf.Append( KUiccPadding );
        // File path length in bytes
        uiccSbApplPathBuf.Append( length );
        // Filler
        uiccSbApplPathBuf.Append( KUiccPadding );
        // File path
        uiccSbApplPathBuf.Append( filePath );
        // Append subblock to ISI message
        isiMsg.CopyData( uiccSbOffset, uiccSbApplPath.CompleteSubBlock() );
        // Update subblock offset
        uiccSbOffset += uiccSbApplPathBuf.Length();

        // Remove already added file from buffer
        aFileList.SetLength( aFileList.Length() - length );
        // Clear buffer for a new sub block
        uiccSbApplPathBuf.Zero();
        }

    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_NSB,
        numOfSubblocks );

    // set flag on that we are waiting a response for a refresh request
    iRefreshRequestIsOn = ETrue;

    return( iPnSend->Send( isiMsg.Complete() ) );
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccCatReqTerminalResponse
// Send UICC_CAT_REQ ISI message for sending terminal response to UICC
// -----------------------------------------------------------------------------
//
TInt CSatMessHandler::UiccCatReqTerminalResponse(
    const TDes8& aCommandDetails,         // Command details tlv
    const TDesC8& aBerTlvSpecificData,    // BER-TLV specific data
    const TUint8 aTransId )
    {
    TFLOGSTRING("TSY: CSatMessHandler::UiccCatReqTerminalResponse");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCCATREQTERMINALRESPONSE, "CSatMessHandler::UiccCatReqTerminalResponse" );

    TIsiSend isiMsg( iPnSend->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_TRANSID,
        aTransId );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_MESSAGEID,
        UICC_CAT_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_SERVICETYPE,
        UICC_CAT_TERMINAL_RESPONSE );
    // One subblock, UICC_SB_TERMINAL_RESPONSE
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_NSB,
        1 );

    // UICC_SB_TERMINAL_RESPONSE
    TBuf8<KMaxSizeOfUiccSbTerminalResponse> uiccSbTerminalResponseBuf( 0 );
    TIsiSubBlock uiccSbTerminalResponse(
        uiccSbTerminalResponseBuf,
        UICC_SB_TERMINAL_RESPONSE,
        EIsiSubBlockTypeId16Len16 );

    // 2x filler
    uiccSbTerminalResponseBuf.Append( KUiccPadding );
    uiccSbTerminalResponseBuf.Append( KUiccPadding );

    if ( aCommandDetails.Length() )
        {
        // Calculate terminal response data length
        // 9 = command details lenght + device identity length
        TUint16 lengthOfData( ( TUint16 ) ( 9 + aBerTlvSpecificData.Length() ) );
        // Terminal response data length (2 bytes)
        uiccSbTerminalResponseBuf.Append( lengthOfData >> 8 );
        uiccSbTerminalResponseBuf.Append( lengthOfData );

        // Add command details
        uiccSbTerminalResponseBuf.Append( aCommandDetails );

        // Add device identity TLV
        // Device identity tag
        uiccSbTerminalResponseBuf.Append( KTlvDeviceIdentityTag );
        // Device identity TLV length
        uiccSbTerminalResponseBuf.Append( KTlvDeviceIdentityLength );
        // Source device identity = ME
        uiccSbTerminalResponseBuf.Append( KMe );
        // Destination device identity = SIM
        uiccSbTerminalResponseBuf.Append( KSim );

        // Add TLV specific data
        uiccSbTerminalResponseBuf.Append( aBerTlvSpecificData );
        }
    else // Empty terminal response =>  length is zero
        {
        uiccSbTerminalResponseBuf.Append( 0 );
        uiccSbTerminalResponseBuf.Append( 0 );
        }

    // Save terminal resp transaction id
    iTerminalRespTraId = aTransId;
    TFLOGSTRING2("TSY: CSatMessHandler::UiccCatReqTerminalResponse iTerminalRespTraId=%d", iTerminalRespTraId);
    OstTrace1( TRACE_NORMAL, DUP1_CSATMESSHANDLER_UICCCATREQTERMINALRESPONSE, "CSatMessHandler::UiccCatReqTerminalResponse;iTerminalRespTraId=%d", iTerminalRespTraId );

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_REQ,
        uiccSbTerminalResponse.CompleteSubBlock() );

    return( iPnSend->Send( isiMsg.Complete() ) );
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::UiccCatRespEnvelope
// Handles UICC response for envelope
// -----------------------------------------------------------------------------
//
TBool CSatMessHandler::UiccCatRespEnvelope( const TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCCATENVELOPERESP, "CSatMessHandler::UiccCatRespEnvelope" );
    TFLOGSTRING("TSY:CSatMessHandler::UiccCatRespEnvelope");

    TBool handled( EFalse );
    TUint8 trId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    TUint uiccSbApduOffset( 0 );
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_RESP,
        UICC_SB_APDU,
        EIsiSubBlockTypeId16Len16,
        uiccSbApduOffset ) )
        {
        TPtrC8 apduData;
        TUint16 apduLength( aIsiMessage.Get16bit(
            uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDULENGTH ) );
        apduData.Set( aIsiMessage.GetData(
            uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDU,
            apduLength ) );
        // Status bytes are two last bytes in APDU
        TUint8 sw1( apduData[apduLength - 2] );
        TUint8 sw2( apduData[apduLength - 1] );

        if ( KAtkSwDataNtfSw1NormalEnding == sw1
            && KAtkSwDataNtfSw2NormalEnding == sw2 )
            {
            // SIM responded OK. Remove stored envelope.
            iTsySatMessaging->GetSatTimer()->RemoveEnvelope( trId );
            }
        else
            {
            // Problems in envelope sending, resend envelope if appropriate.
            if ( KErrNone == iTsySatMessaging->GetSatTimer()->
                ActivateEnvelopeResend( trId, sw1 ) )
                {
                TFLOGSTRING( "TSY:CSatMessHandler::UiccCatRespEnvelope, resending of envelope" );
                OstTrace0( TRACE_NORMAL, DUP2_CSATMESSHANDLER_UICCCATENVELOPERESP, "CSatMessHandler::UiccCatRespEnvelope,resending of envelope" );
                // Envelope will be resent, mark as handled
                handled = ETrue;
                }
            }
        }
    else // Subblock is mandatory
        {
        TFLOGSTRING("TSY: CSatMessHandler::UiccCatRespEnvelope - Mandatory subblock UICC_SB_APDU not found");
        OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_UICCCATENVELOPERESP, "CSatMessHandler::UiccCatRespEnvelope- Mandatory subblock UICC_SB_APDU not found" );
        }

    return handled;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::UiccCatRespTerminalResponse
// Handles UICC response for envelope
// -----------------------------------------------------------------------------
//
TBool CSatMessHandler::UiccCatRespTerminalResponse(
    const TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_NORMAL, DUP2_CSATMESSHANDLER_UICCCATRESPTERMINALRESPONSE, "CSatMessHandler::UiccCatRespTerminalResponse" );
    TFLOGSTRING("TSY:CSatMessHandler::UiccCatRespTerminalResponse");

    TBool handled( EFalse );
    TUint8 trId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    TUint uiccSbApduOffset( 0 );
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_RESP,
        UICC_SB_APDU,
        EIsiSubBlockTypeId16Len16,
        uiccSbApduOffset ) )
        {
        TPtrC8 apduData;
        TUint16 apduLength( aIsiMessage.Get16bit(
            uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDULENGTH ) );
        apduData.Set( aIsiMessage.GetData(
            uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDU,
            apduLength ) );
        // Status bytes are two last bytes in APDU
        TUint8 sw1( apduData[apduLength - 2] );
        TUint8 sw2( apduData[apduLength - 1] );

        if ( KAtkSwDataNtfSw1NormalEnding == sw1
            && KAtkSwDataNtfSw2NormalEnding == sw2 )
            {
            TFLOGSTRING("CSatMessHandler::UiccCatRespTerminalResponse, - SIM SESSION END, 90 00 -");
            OstTrace0( TRACE_NORMAL, DUP1_CSATMESSHANDLER_UICCCATRESPTERMINALRESPONSE, "CSatMessHandler::UiccCatRespTerminalResponse" );

            iTsySatMessaging->SessionEnd( aIsiMessage );

            // Set flag to indicate that terminal response has been
            // processed by the SIM with status '90 00'.
            iTsySatMessaging->GetSatTimer()->
                SetProactiveCommandOnGoingStatus( EFalse );
            }
        handled = ETrue;

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
        // Only if SET UP EVENT LIST TR is sent
        if ( iSetUpEventListTrSent )
            {
            iSetUpEventListTrSent = EFalse;
            iTsySatMessaging->GetEventDownload()->ForceLocationStatusEnvelope();
            }
#endif // INFO_PP_ENHANCED_NETWORK_SELECTION
        }
    else // Subblock is not found
        {
        TFLOGSTRING("TSY: CSatMessHandler::UiccCatRespTerminalResponse - Subblock UICC_SB_APDU not found");
        OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_UICCCATRESPTERMINALRESPONSE, "CSatMessHandler::UiccCatRespTerminalResponse" );
        }

    return handled;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::GetApplicationId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CSatMessHandler::GetApplicationFileId()
    {
    TFLOGSTRING("TSY: CSatMessHandler::GetApplicationId");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GETAPPLICATIONID, "CSatMessHandler::GetApplicationId" );
    return iApplFileId;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::SetSatReadyStatus
// Set status of iSatReady flag
// -----------------------------------------------------------------------------
//
void CSatMessHandler::SetSatReadyStatus( TBool aSatReadyStatus )
    {
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_SETSATREADYSTATUS, "CSatMessHandler::SetSatReadyStatus" );
    TFLOGSTRING("TSY: CSatMessHandler::SetSatReadyStatus");
    iSatReady = aSatReadyStatus;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::GetSatReadyStatus
// Set status of iSatReady flag
// -----------------------------------------------------------------------------
//
TBool CSatMessHandler::GetSatReadyStatus()
    {
    TFLOGSTRING("TSY: CSatMessHandler::GetSatReadyStatus");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GETSATREADYSTATUS, "CSatMessHandler::GetSatReadyStatus" );
    return iSatReady;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::GetTerminalProfileStatus
// Get status of iTerminalProfileSent flag
// -----------------------------------------------------------------------------
//
TBool CSatMessHandler::GetTerminalProfileStatus()
    {
    TFLOGSTRING("TSY: CSatMessHandler::GetTerminalProfileStatus");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GETTERMINALPROFILESTATUS, "CSatMessHandler::GetTerminalProfileStatus" );
    return iTerminalProfileSent;
    }


// -----------------------------------------------------------------------------
// CSatMessHandler::GetClut
// Get clut data
// -----------------------------------------------------------------------------
//
const TDesC8& CSatMessHandler::GetClut()
    {
    TFLOGSTRING("TSY: CSatMessHandler::SetImageInstanceRequestStatus");
    OstTrace0( TRACE_NORMAL, CSATMESSHANDLER_GETCLUT, "CSatMessHandler::GetClut" );
    return iClutData;
    }


// End of file
