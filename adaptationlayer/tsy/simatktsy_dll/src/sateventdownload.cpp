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
#include "sateventdownload.h"   // sat event download class
#include "satmessaging.h"       // sat messaging class
#include "ber_tlv.h"            // sat ber-tlv classes
#include "satutil.h"            // sat utility class

#include <pn_const.h>           // server id constants
#include <tisi.h>               // isi message
#include <net_modemisi.h>       // net server
#include <call_modemisi.h>      // Call Modem server
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "sateventdownloadTraces.h"
#endif


// CONSTANTS
const TUint8 KMaxCallModemDetailedCause = 248;
const TUint8 KCallModemOriginCsAddressMaxLength = 246;
const TUint8 KMSBMask = 0x80;


// ==================== MEMBER FUNCTIONS ====================================

// -----------------------------------------------------------------------------
// CSatEventDownload::CSatEventDownload
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatEventDownload::CSatEventDownload
        (
        CSatMessHandler*    aSatMessHandler,
        CTsySatMessaging*   aSatMessaging
        )
        :
        iSatMessHandler( aSatMessHandler ),
        iSatMessaging( aSatMessaging ),
        iSetUpCallOngoing( EFalse ),
        iForceLocationStatusEnvelope( EFalse ),
        iCallsOngoing( EFalse ),
        iLocationStatusEnvelopePending( EFalse ),
        iNetCellInfoIndReceived( EFalse )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_CSATEVENTDOWNLOAD, "CSatEventDownload::CSatEventDownload" );
    iLocInfo.iOperatorCode.FillZ( iLocInfo.iOperatorCode.MaxLength() );
    iLocInfo.iLac.FillZ( iLocInfo.iLac.MaxLength() );
    iLocInfo.iCellId.FillZ( iLocInfo.iCellId.MaxLength() );
    iLocInfo.iRegStatus = NET_NO_COVERAGE;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatEventDownload::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_CONSTRUCTL, "CSatEventDownload::ConstructL" );
    // None. Reserved for future use.
    }


// -----------------------------------------------------------------------------
// CSatEventDownload::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatEventDownload* CSatEventDownload::NewL
        (
        CSatMessHandler*    aSatMessHandler,
        CTsySatMessaging*   aSatMessaging
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_NEWL, "CSatEventDownload::NewL" );
    TFLOGSTRING("TSY: CSatEventDownload::NewL");

    CSatEventDownload* self =
        new( ELeave ) CSatEventDownload( aSatMessHandler, aSatMessaging );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CSatEventDownload::~CSatEventDownload
// C++ destructor
// -----------------------------------------------------------------------------
//
CSatEventDownload::~CSatEventDownload()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATEVENTDOWNLOAD_CSATEVENTDOWNLOAD, "CSatEventDownload::~CSatEventDownload" );
    TFLOGSTRING("TSY: CSatEventDownload::~CSatEventDownload");
    }


// -----------------------------------------------------------------------------
// CSatEventDownload::GetTransactionId
// Generate transaction id for ISI message
// -----------------------------------------------------------------------------
//
TUint8 CSatEventDownload::GetTransactionId()
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_GETTRANSACTIONID, "CSatEventDownload::GetTransactionId" );
    TFLOGSTRING("TSY: CSatEventDownload::GetTransactionId");
    return iSatMessaging->GetTransactionId();
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SetUpEventList
// Sets up event list
// -----------------------------------------------------------------------------
//
void CSatEventDownload::SetUpEventList
        (
        TUint32 aEvents        // bit mask of enabled events
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_SETUPEVENTLIST, "CSatEventDownload::SetUpEventList" );
    TFLOGSTRING("TSY: CSatEventDownload::SetUpEventList");
    iEventList.Set( aEvents );

    // Force sending of envelope at least once, if location status event set
    if ( iEventList.IsEnabled( RSat::KLocationStatus ) )
        {
        iForceLocationStatusEnvelope = ETrue;
        }
    // Reset flag if removal of event list or not location status event
    else
        {
        iForceLocationStatusEnvelope = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendMTCallEnvelope
// Send MT call envelope to USIM.
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendMTCallEnvelope
        (
        TUint8 aTransactionId,                 // Transaction Identifier
        const TDesC8& aDiallingNumber,         // Calling Party BCD number
        const TDesC8& aCalledPartySubAddress   // Calling Party Subaddress
        )
    {
    OstTraceExt1( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDMTCALLENVELOPE, "CSatEventDownload::SendMTCallEnvelope (transactionId=%hhu)", aTransactionId );
    TFLOGSTRING2("TSY: CSatEventDownload::SendMTCallEnvelope\
        (transactionId=%d)", TInt( aTransactionId ) );

    TInt ret( KErrNone );

    // create envelope
    TTlv envelope;
    envelope.Begin  ( KBerTlvEventDownloadTag );

    // event list
    envelope.AddTag ( KTlvEventListTag );
    envelope.AddByte( KEventMTCall );

    // device identities
    envelope.AddTag ( KTlvDeviceIdentityTag );
    envelope.AddByte( KNetwork );
    envelope.AddByte( KSim );

    // transaction identifier
    envelope.AddTag ( KTlvTransactionIdentifierTag );
    envelope.AddByte( aTransactionId );

    if ( aDiallingNumber.Length() )
        {
        envelope.AddTag( KTlvAddressTag );
        // First byte contains TON/NPI, rest of the bytes address
        envelope.AddData( aDiallingNumber );
        }

    if ( aCalledPartySubAddress.Length() )
        {
        // optional called party subaddress
        envelope.AddTag( KTlvSubaddressTag );
        envelope.AddData( aCalledPartySubAddress );
        }

    ret = iSatMessHandler->UiccCatReqEnvelope(
        GetTransactionId(), envelope.End() );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendCallConnectedEnvelope
// Send Call connected envelope to USIM.
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendCallConnectedEnvelope
        (
        TUint8 aTransactionId, // Transaction Identifier in the CONNECT message.
        TBool aNearEnd
        )
    {
    OstTraceExt1( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDCALLCONNECTEDENVELOPE, "CSatEventDownload::SendCallConnectedEnvelope (transactionId=%hhu)", aTransactionId );
    TFLOGSTRING2("TSY: CSatEventDownload::SendCallConnectedEnvelope\
        (transactionId=%d)", TInt( aTransactionId ) );

    TInt ret( KErrNone );

    // create envelope
    TTlv envelope;
    envelope.Begin  ( KBerTlvEventDownloadTag );

    // event list
    envelope.AddTag ( KTlvEventListTag );
    envelope.AddByte( KEventCallConnected );

    // device identities
    envelope.AddTag ( KTlvDeviceIdentityTag );
    envelope.AddByte( aNearEnd ? KMe : KNetwork );
    envelope.AddByte( KSim );

    // transaction identifier
    envelope.AddTag ( KTlvTransactionIdentifierTag );
    envelope.AddByte( aTransactionId );

    // Check if the call is SetUpCall originated and it's an MO call
    if ( iSetUpCallOngoing && !aNearEnd )
        {
        // Store envelope to cache and it will be send after a terminal resp
        // of set up call.
        iSatMessaging->StoreCallConnectedEvent( envelope.End() );
        }
    else
        {
        // send envelope now
        ret = iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendCallDisconnectedEnvelope
// Send call disconnected event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendCallDisconnectedEnvelope
        (
        TUint8 aTransactionId,
        TBool aNearEnd,
        TBool aCauseGiven,
        const TDesC8& aCause
        )
    {
    OstTraceExt1( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDCALLDISCONNECTEDENVELOPE, "CSatEventDownload::SendCallDisconnectedEnvelope (transactionId=%hhu)", aTransactionId );
    TFLOGSTRING2("TSY: CSatEventDownload::SendCallDisconnectedEnvelope\
        (transactionId=%d)", TInt( aTransactionId ) );

    TInt ret( KErrNone );

    // create envelope
    TTlv envelope;
    envelope.Begin  ( KBerTlvEventDownloadTag );

    // event list
    envelope.AddTag ( KTlvEventListTag );
    envelope.AddByte( KEventCallDisconnected );

    // device identities
    envelope.AddTag ( KTlvDeviceIdentityTag );
    envelope.AddByte( aNearEnd ? KMe : KNetwork );
    envelope.AddByte( KSim );

    // transaction identifier
    envelope.AddTag ( KTlvTransactionIdentifierTag );
    envelope.AddByte( aTransactionId );

    // cause
    if ( aCauseGiven )
        {
        envelope.AddTag ( KTlvCauseTag );
        envelope.AddData( aCause );
        }

    // send
    ret = iSatMessHandler->UiccCatReqEnvelope(
        GetTransactionId(), envelope.End() );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendLocationStatusEnvelope
// Send location status event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendLocationStatusEnvelope
        (
        const CSatMessHandler::TLocationInfo& aNewLocInfo
        )
    {
    OstTrace1( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDLOCATIONSTATUSENVELOPE, "CSatEventDownload::SendLocationStatusEnvelope locationStatus = %d", aNewLocInfo.iRegStatus );
    TFLOGSTRING2("TSY: CSatEventDownload::SendLocationStatusEnvelope \
        locationStatus = %d", TInt( aNewLocInfo.iRegStatus ) );

    TInt ret( KErrNone );

    // Send location status envelope only if there
    // have been changes
    if ( ( iLocInfo.iRegStatus    != aNewLocInfo.iRegStatus )
      || ( iLocInfo.iOperatorCode != aNewLocInfo.iOperatorCode )
      || ( iLocInfo.iLac          != aNewLocInfo.iLac )
      || ( iLocInfo.iCellId       != aNewLocInfo.iCellId )
      || iForceLocationStatusEnvelope )
        {
        TFLOGSTRING("TSY: CSatEventDownload::SendLocationStatusEnvelope \
            Saving and sending");
        OstTrace0( TRACE_NORMAL, DUP1_CSATEVENTDOWNLOAD_SENDLOCATIONSTATUSENVELOPE, "CSatEventDownload::SendLocationStatusEnvelope Saving and sending" );

        // Cache values
        iLocInfo = aNewLocInfo;

        // Create envelope
        TTlv envelope;
        envelope.Begin( KBerTlvEventDownloadTag );

        // Event list
        envelope.AddTag( KTlvEventListTag );
        envelope.AddByte( KEventLocationStatus );

        // Device identities
        envelope.AddTag( KTlvDeviceIdentityTag );
        envelope.AddByte( KMe );
        envelope.AddByte( KSim );

        // Locationstatus
        envelope.AddTag( KTlvLocationStatusTag );

        switch ( iLocInfo.iRegStatus )
            {
            case NET_SERVICE:
                {
                envelope.AddByte( KLocationStatusNormalService );

                // Location information, this is included only if status
                // indicates normal service
                envelope.AddTag( KTlvLocationInformationTag );
                envelope.AddData( iLocInfo.iOperatorCode );
                envelope.AddData( iLocInfo.iLac );
                envelope.AddData( iLocInfo.iCellId );
                break;
                }
            case NET_LIMITED_SERVICE:
                {
                envelope.AddByte( KLocationStatusLimitedService );
                break;
                }
            case NET_NO_COVERAGE:
            default:
                {
                envelope.AddByte( KLocationStatusNoService );
                break;
                }
            }

        // Reset the flag
        iForceLocationStatusEnvelope = EFalse;

        // send
        ret = iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(),
            envelope.End() );
        }
    else
        {
        TFLOGSTRING("TSY: CSatEventDownload::SendLocationStatusEnvelope \
            Location Info already received, envelope sending omitted.");
        OstTrace0( TRACE_NORMAL, DUP2_CSATEVENTDOWNLOAD_SENDLOCATIONSTATUSENVELOPE, "CSatEventDownload::SendLocationStatusEnvelope Location Info already received, envelope sending omitted." );

        }

    iLocationStatusEnvelopePending = EFalse;

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendUserActivityEnvelope
// Send user activity envelope to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendUserActivityEnvelope()
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDUSERACTIVITYENVELOPE, "CSatEventDownload::SendUserActivityEnvelope" );
    TFLOGSTRING("TSY: CSatEventDownload::SendUserActivityEnvelope");

    TInt ret = KErrNone;

    if ( iEventList.IsEnabled(RSat::KUserActivity) )
        {
        // create envelope
        TTlv envelope;
        envelope.Begin  ( KBerTlvEventDownloadTag );

        // event list
        envelope.AddTag ( KTlvEventListTag );
        envelope.AddByte( KEventUserActivity );

        // device identities
        envelope.AddTag ( KTlvDeviceIdentityTag );
        envelope.AddByte( KMe );
        envelope.AddByte( KSim );

        // send
        ret = iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );

        // remove from event list
        iEventList.Remove( RSat::KUserActivity );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendIdleScreenAvailableEnvelope
// Send idle screeen available event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendIdleScreenAvailableEnvelope()
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDIDLESCREENAVAILABLEENVELOPE, "CSatEventDownload::SendIdleScreenAvailableEnvelope" );
    TFLOGSTRING("TSY: CSatEventDownload::SendIdleScreenAvailableEnvelope");

    TInt ret = KErrNone;

    if ( iEventList.IsEnabled( RSat::KIdleScreenAvailable ) )
        {
        // create envelope
        TTlv envelope;
        envelope.Begin  ( KBerTlvEventDownloadTag );

        // event list
        envelope.AddTag ( KTlvEventListTag );
        envelope.AddByte( KEventIdleScreenAvailable );

        // device identities
        envelope.AddTag ( KTlvDeviceIdentityTag );
        envelope.AddByte( KDisplay );
        envelope.AddByte( KSim );

        // send
        ret = iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );

        if( KErrNone == ret )
            {
            // remove from event list
            iEventList.Remove( RSat::KIdleScreenAvailable );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendBrowserTerminationEnvelope
// Send browser termination event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendBrowserTerminationEnvelope
        (
        RSat::TBrowserTerminationCause aCause
        )
    {
    OstTrace1( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDBROWSERTERMINATIONENVELOPE, "CSatEventDownload::SendBrowserTerminationEnvelope (cause=%d)", TInt(aCause) );
    TFLOGSTRING2("TSY: CSatEventDownload::SendBrowserTerminationEnvelope\
        (cause=%d)", TInt( aCause ) );

    TInt ret = KErrNone;

    if ( iEventList.IsEnabled( RSat::KBrowserTermination ) )
        {
        // create envelope
        TTlv envelope;
        envelope.Begin  ( KBerTlvEventDownloadTag );

        // event list
        envelope.AddTag ( KTlvEventListTag );
        envelope.AddByte( KEventBrowserTermination );

        // device identities
        envelope.AddTag ( KTlvDeviceIdentityTag );
        envelope.AddByte( KMe );
        envelope.AddByte( KSim );

        // cause
        envelope.AddTag( KTlvBrowserTerminationCauseTag );
        envelope.AddByte(
            TUint8( ( RSat::EUserTermination == aCause) ?
            KBrowserTerminationUser : KBrowserTerminationError) );

        // send
        ret = iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendLanguageSelectionEnvelope
// Send Languate selection event to USIMSend Languate selection event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendLanguageSelectionEnvelope
        (
        TUint16 aLanguage
        )
    {
    OstTraceExt1( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDLANGUAGESELECTIONENVELOPE, "CSatEventDownload::SendLanguageSelectionEnvelope (language=%hu)", aLanguage );
    TFLOGSTRING2("TSY: CSatEventDownload::SendLanguageSelectionEnvelope\
        (language=%d)", TInt ( aLanguage ) );

    TInt ret = KErrNone;

    if ( iEventList.IsEnabled( RSat::KLanguageSelection ) )
        {
        // create envelope
        TTlv envelope;
        envelope.Begin  ( KBerTlvEventDownloadTag );

        // event list
        envelope.AddTag ( KTlvEventListTag );
        envelope.AddByte( KEventLanguageSelection );

        // device identities
        envelope.AddTag ( KTlvDeviceIdentityTag );
        envelope.AddByte( KMe );
        envelope.AddByte( KSim );

        // language
        envelope.AddTag( KTlvLanguageTag );
        envelope.AddByte( TUint8((aLanguage >> 8)&0x7f) ); // MSB
        envelope.AddByte( TUint8((aLanguage >> 0)&0x7f) ); // LSB

        // send
        ret =  iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::ExtFunc
// Handles event download IPC requests originating from ETel.
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::ExtFunc
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_EXTFUNC, "CSatEventDownload::ExtFunc" );
    TFLOGSTRING("TSY: CSatEventDownload::ExtFunc");

    TInt ret ( KErrNotSupported );

    // get parameters (eventList, eventInfo) from package
    RSat::TEventDownloadBaseV2Pckg* eventInfoPckg =
        (RSat::TEventDownloadBaseV2Pckg*)aPackage.Des2n();
    RSat::TEventDownloadBaseV2& eventInfo = (*eventInfoPckg)();
    RSat::TEventList& singleEvent = *(RSat::TEventList*)aPackage.Ptr1();

    if ( RSat::KSatV2 == eventInfo.ExtensionId() )
        {

        // branch and do whatever is necessary according to the event type..
        switch ( singleEvent )
            {
            case RSat::KUserActivity:
                {
                ret = SendUserActivityEnvelope();
                break;
                }
            case RSat::KIdleScreenAvailable:
                {
                ret = SendIdleScreenAvailableEnvelope();
                break;
                }
            case RSat::KBrowserTermination:
                {
                ret = SendBrowserTerminationEnvelope(
                    ((RSat::TBrowserTerminationEventV2*)&eventInfo)->iCause );
                break;
                }
            case RSat::KLanguageSelection:
                {
                ret = SendLanguageSelectionEnvelope(
                    ((RSat::TLanguageSelectionEventV2*)&eventInfo)->iLanguage );
                break;
                }
            case RSat::KDataAvailable:
                {
                // DataAvailable will trigger a ReceiveData PCmd
                ret = SendDataAvailableEnvelope(
                    ((RSat::TDataAvailableEventV2*)&eventInfo)->iStatus,
                    ((RSat::TDataAvailableEventV2*)&eventInfo)->iLength );
                break;
                }
            case RSat::KChannelStatus:
                {
                // when the ME detects one of the following changes:
                // -    a link is error, or
                // -    any other error,
                // which is not resulting from the execution of a proactive
                // command, the ME shall inform the SIM that this has occurred.
                ret = SendChannelStatusEnvelope(
                    ((RSat::TChannelStatusEventV2*)&eventInfo)->iStatus );
                break;
                }

            // TSY is responsible for these events
            // but they are not reported to ETel
            //case RSat::KMTCall:
            //case RSat::KCallConnected:
            //case RSat::KCallDisconnected:
            //case RSat::KLocationStatus:
            //case RSat::KAccessTechnologyChange:

            // events that can be reported by client, but are not supported
            // by TSY
            //case RSat::KCardReaderStatus:
            //case RSat::KDisplayParamsChanges:
            //case RSat::KLocalConnection:
            default:
                {
                ret = KErrNotSupported;
                break;
                }
            } // end switch

        } // end if

    // complete the request...
    iSatMessaging->ReqCompleted( aTsyReqHandle, ret );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::Cancel
//
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::Cancel
        (
        const TTsyReqHandle /*aTsyReqHandle*/
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_CANCEL, "CSatEventDownload::Cancel" );
    TFLOGSTRING("TSY: CSatEventDownload::Cancel");
    // since event download is served at once.
    // we don't have anything to do here
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::CallModemMessageIndReceived
// Handle CALL_MODEM_MESSAGE_IND indication
// -----------------------------------------------------------------------------
//
void CSatEventDownload::CallModemMessageIndReceived
        (
        const TIsiReceiveC& aIsiMessage  // incoming ISI message
        )
    {
TFLOGSTRING("TSY: CSatEventDownload::CallModemMessageIndReceived");
OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived" );

    // CALL_MODEM_SB_DETAILED_CAUSE length max value is 248
    TBuf8<KMaxCallModemDetailedCause> cause;
    TBool causeGiven( EFalse );

    // extract call message, message direction and transaction id
    TUint8 callMessage( 0 );
    TUint8 messageDirection( 0 );
    TUint8 transactionId( 0 );
    TUint sbStartOffset( 0 );

    TInt retValue( aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_MESSAGE_IND,
        CALL_MODEM_SB_MSG_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) );

    if ( KErrNone == retValue )
        {
        callMessage = aIsiMessage.Get8bit(
            sbStartOffset + CALL_MODEM_SB_MSG_INFO_OFFSET_MSG );
        messageDirection = aIsiMessage.Get8bit(
            sbStartOffset + CALL_MODEM_SB_MSG_INFO_OFFSET_MSGDIR );
        transactionId = aIsiMessage.Get8bit(
            sbStartOffset + CALL_MODEM_SB_MSG_INFO_OFFSET_TRANSACTIONID );

        switch ( callMessage )
            {
            case CALL_MODEM_MSG_SETUP:
                {
                if ( CALL_MODEM_MSG_DIRECT_RECEIVED == messageDirection )
                    {
                    // Received event is mobile terminated call
                    if ( iEventList.IsEnabled( RSat::KMTCall ) )
                        {
                        // 'Calling Party BCD Number IE'.
                        TBuf8<KCallModemOriginCsAddressMaxLength> BCDAddress;
                        // CALL_ORIGIN_SUBADDRESS, value range is 0 to 248
                        TPtrC8 subAddress;
                           
                        // Extract address and address type (=TON and NPI)
                        retValue = aIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_CALL_MODEM_MESSAGE_IND,
                            CALL_MODEM_SB_ORIGIN_CS_ADDRESS,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset );

                        if ( KErrNone == retValue )
                            {
                            TInt addressLength( aIsiMessage.Get8bit(
                                sbStartOffset +
                                CALL_MODEM_SB_ORIGIN_CS_ADDRESS_OFFSET_ADDRLEN ) );

                            // Set address type (TON/NPI) in correct form
                            BCDAddress.Append( aIsiMessage.Get8bit( 
                                sbStartOffset +
                                CALL_MODEM_SB_ORIGIN_CS_ADDRESS_OFFSET_ADDR )
                                | KMSBMask );
                            // Append BCD address. Skip TON/NPI
                            BCDAddress.Append( aIsiMessage.GetData(
                                sbStartOffset +
                                CALL_MODEM_SB_ORIGIN_CS_ADDRESS_OFFSET_ADDR + 1,
                                addressLength - 1 ) );
                            }
                        // Extract subaddress
                        retValue = aIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_CALL_MODEM_MESSAGE_IND,
                            CALL_MODEM_SB_ORIGIN_SUBADDRESS,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset );

                        if ( KErrNone == retValue )
                            {
                            TInt subAddressLength( aIsiMessage.Get8bit(
                                sbStartOffset +
                                CALL_MODEM_SB_ORIGIN_SUBADDRESS_OFFSET_ADDRLEN ) );

                            subAddress.Set( aIsiMessage.GetData(
                                sbStartOffset + CALL_MODEM_SB_ORIGIN_SUBADDRESS_OFFSET_ADDR,
                                subAddressLength ) );
                            }
TFLOGSTRING( "CSatEventDownload::CallModemMessageIndReceived Send MT call event" );
OstTrace0( TRACE_NORMAL, DUP1_CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived Send MT call event" );

                        SendMTCallEnvelope( 
                            transactionId,
                            BCDAddress,
                            subAddress );
                        }
                    }
                break;
                }
            case CALL_MODEM_MSG_CONNECT:
                {
                if ( iEventList.IsEnabled( RSat::KCallConnected ) )
                    {
                    if ( CALL_MODEM_MSG_DIRECT_RECEIVED == messageDirection )
                        {
TFLOGSTRING("CSatEventDownload::CallModemMessageIndReceived Creating a call");
OstTrace0( TRACE_NORMAL, DUP2_CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived Creating a call" );

                        SendCallConnectedEnvelope(
                            transactionId,
                            EFalse/*mobile originated*/ );
                        }
                    else if ( CALL_MODEM_MSG_DIRECT_SENT == messageDirection )
                        {
TFLOGSTRING("CSatEventDownload::CallModemMessageIndReceived Incoming call arrives");
OstTrace0( TRACE_NORMAL, DUP3_CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived Incoming call arrives" );

                        SendCallConnectedEnvelope(
                            transactionId,
                            ETrue/*mobile terminated*/);
                        }
                    }
                break;
                }
            case CALL_MODEM_MSG_RELEASE:
                {
                if ( iEventList.IsEnabled( RSat::KCallDisconnected ) )
                    {
                    // extract cause
                    retValue = aIsiMessage.FindSubBlockOffsetById(
                        ISI_HEADER_SIZE + SIZE_CALL_MODEM_MESSAGE_IND,
                        CALL_MODEM_SB_DETAILED_CAUSE,
                        EIsiSubBlockTypeId8Len8,
                        sbStartOffset );

                    if ( KErrNone == retValue )
                        {
                        TInt causeLength( aIsiMessage.Get8bit(
                            sbStartOffset + CALL_MODEM_SB_DETAILED_CAUSE_OFFSET_CAUSELEN ) );
                        cause.Copy( aIsiMessage.GetData(
                            sbStartOffset + CALL_MODEM_SB_DETAILED_CAUSE_OFFSET_CAUSE,
                            causeLength ) );
                        causeGiven = ETrue;
                        }
                        
                    if ( CALL_MODEM_MSG_DIRECT_SENT == messageDirection )
                        {
                        // 2.33.11 Event Indications on Mobile Originated Call
                        // Clearing (GSM)
TFLOGSTRING( "CSatEventDownload::CallModemMessageIndReceived MO call clearing ");
OstTrace0( TRACE_NORMAL, DUP4_CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived MO call clearing" );

                        SendCallDisconnectedEnvelope(
                            transactionId,
                            ETrue/*near end*/,
                            causeGiven,
                            cause );
                        }
                    else if ( CALL_MODEM_MSG_DIRECT_RECEIVED == messageDirection )
                        {
                        // 2.33.12 Event indications on Mobile Terminated Call
                        // Clearing (GSM)
TFLOGSTRING( "CSatEventDownload::CallModemMessageIndReceived MT call clearing");
OstTrace0( TRACE_NORMAL, DUP7_CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived MT call clearing" );

                        SendCallDisconnectedEnvelope(
                            transactionId,
                            EFalse/*far end*/,
                            causeGiven,
                            cause );
                        }
                    else if ( CALL_MODEM_MSG_DIRECT_INTERNAL == messageDirection )
                        {
                        // 2.33.13 Event indications on radio link failure
TFLOGSTRING( "CSatEventDownload::CallModemMessageIndReceived radiolink failure");
OstTrace0( TRACE_NORMAL, DUP5_CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived Radiolink failure" );
                        // In the case of a radio link timeout, the Cause data
                        // object shall be included, with a value part of zero
                        // length.
                        causeGiven = ETrue;
                        cause.Zero();
                        SendCallDisconnectedEnvelope(
                            transactionId,
                            ETrue/*near end*/,
                            causeGiven,
                            cause );
                        }
                    }
                break;
                }
            default:
                {
TFLOGSTRING( "CSatEventDownload::CallModemMessageIndReceived UNKNOWN" );
OstTrace0( TRACE_NORMAL, DUP6_CSATEVENTDOWNLOAD_CALLMODEMMESSAGEINDRECEIVED, "CSatEventDownload::CallModemMessageIndReceived UNKNOWN" );
                // do nothing
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendDataAvailableEnvelope
// Send data available event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendDataAvailableEnvelope
        (
        RSat::TChannelStatus aStatus,    // Status of the channel
        TInt8                 aLength     // Channel data length
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDDATAAVAILABLEENVELOPE, "CSatEventDownload::SendDataAvailableEnvelope" );
    TFLOGSTRING( "TSY: CSatEventDownload::SendDataAvailableEnvelope" );

    TInt ret = KErrNone;

    if ( iEventList.IsEnabled( RSat::KDataAvailable ) )
        {
        // create envelope
        TTlv envelope;
        envelope.Begin  ( KBerTlvEventDownloadTag );

        // event list
        envelope.AddTag ( KTlvEventListTag );
        envelope.AddByte( KEventDataAvailable );

        // device identities
        envelope.AddTag ( KTlvDeviceIdentityTag );
        envelope.AddByte( KMe );
        envelope.AddByte( KSim );

        // channel status
        envelope.AddTag( KTlvChannelStatusTag );
        envelope.AddData( aStatus );

        // channel data length
        envelope.AddTag( KTlvChannelDataLengthTag );
        envelope.AddByte( aLength );

        // send
        ret =  iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendChannelStatusEnvelope
// Send channel status event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendChannelStatusEnvelope
        (
        RSat::TChannelStatus aStatus    // Status of the channel
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDCHANNELSTATUSENVELOPE, "CSatEventDownload::SendChannelStatusEnvelope" );
    TFLOGSTRING("TSY: CSatEventDownload::SendChannelStatusEnvelope");

    TInt ret = KErrNone;

    if ( iEventList.IsEnabled( RSat::KChannelStatus ) )
        {
        // create envelope
        TTlv envelope;
        envelope.Begin  ( KBerTlvEventDownloadTag );

        // event list
        envelope.AddTag ( KTlvEventListTag );
        envelope.AddByte( KEventChannelStatus );

        // device identities
        envelope.AddTag ( KTlvDeviceIdentityTag );
        envelope.AddByte( KMe );
        envelope.AddByte( KSim );

        // channel status
        envelope.AddTag( KTlvChannelStatusTag );
        envelope.AddData( aStatus );

        // send
        ret =  iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::MessageReceived
// Handle received messages related to event download.
// Called by CSatMessHandler::MessageReceivedL, when a new ISI message arrives.
// Identifies call server CALL_STATUS_IND message and passes it to
// CallStatusIndReceived.
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_MESSAGERECEIVED, "CSatEventDownload::MessageReceived" );
    TFLOGSTRING("TSY: CSatEventDownload::MessageReceived");
    // Get resource and message id's
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    if ( PN_MODEM_CALL == resource )
        {
        switch ( messageId )
            {
            case CALL_MODEM_MESSAGE_IND:
                {
                if ( iEventList.IsEnabled( RSat::KMTCall )
                    || iEventList.IsEnabled( RSat::KCallConnected )
                    || iEventList.IsEnabled( RSat::KCallDisconnected ) )
                    {
                    // for call connected and disconnected events
                    CallModemMessageIndReceived( aIsiMessage );
                    }
                break;
                }
            case CALL_MODEM_PRESENT_IND:
                {
                // Used only for location status event
                if ( iEventList.IsEnabled( RSat::KLocationStatus ) )
                    {
                    TUint8 serverStatus( aIsiMessage.Get8bit(
                        ISI_HEADER_SIZE +
                        CALL_MODEM_PRESENT_IND_OFFSET_CALLPRESENT ) );
                    // Upper three bits (5-7) indicate the underlying
                    // cellular system. We don't use them, let's AND them away
                    serverStatus &= 0x0F;

                    if ( CALL_MODEM_NOT_PRESENT == serverStatus )
                        {
                        if ( iLocationStatusEnvelopePending )
                            {
                            // Location status event sent if ME is not busy
                            // on calls and envelope is pending
                            SendLocationStatusEnvelope(
                                iSatMessHandler->LocationInfo() );
                            }
                        iCallsOngoing = EFalse;
                        }
                    else
                        {
                        // There's active calls
                        iCallsOngoing = ETrue;
                        }
                    }
                break;
                }
            default:
                {
                // do nothing
                break;
                }
            }
        }
    else if ( PN_MODEM_NETWORK == resource )
        {
        switch ( messageId )
            {
            case NET_CELL_INFO_IND:
                {
                iNetCellInfoIndReceived = ETrue;
                if ( iEventList.IsEnabled( RSat::KLocationStatus ) )
                    {
                    if ( !iCallsOngoing )
                        {
                        // Location status event sent if ME is not busy on calls
                        SendLocationStatusEnvelope(
                            iSatMessHandler->LocationInfo() );
                        }
                    else
                        {
                        // Let's send envelope after ongoing call(s)
                        iLocationStatusEnvelopePending = ETrue;
                        }
                    }
                break;
                }
            default:
                {
                // do nothing
                break;
                }
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SetSetUpCallStatus
// Set SetUpCall status
// -----------------------------------------------------------------------------
//
void CSatEventDownload::SetSetUpCallStatus
        (
        const TBool aStatus // SetUpCall status
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_SETSETUPCALLSTATUS, "CSatEventDownload::SetSetUpCallStatus" );
    TFLOGSTRING("TSY: CSatEventDownload::SetSetUpCallStatus");
    iSetUpCallOngoing = aStatus;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::SendAccessTechnologyChangeEnvelope
// Send access technology change event to USIM
// -----------------------------------------------------------------------------
//
TInt CSatEventDownload::SendAccessTechnologyChangeEnvelope
        (
        TUint8  accTechChange //Acc Tech
        )
    {
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_SENDACCESSTECHNOLOGYCHANGEENVELOPE, "CSatEventDownload::SendAccessTechnologyChangeEnvelope" );
    TFLOGSTRING("TSY: CSatEventDownload::SendAccessTechnologyChangeEnvelope");

    TInt ret = KErrNone;

    if ( iEventList.IsEnabled( RSat::KAccessTechnologyChange ) )
        {
        TFLOGSTRING("TSY: CSatEventDownload::SendAccessTechnologyChangeEnvelope\
        - Event enabled: sending envelope");
        OstTrace0( TRACE_NORMAL, DUP1_CSATEVENTDOWNLOAD_SENDACCESSTECHNOLOGYCHANGEENVELOPE, "CSatEventDownload::SendAccessTechnologyChangeEnvelope - Event enabled: sending envelope" );
        // create envelope
        TTlv envelope;
        envelope.Begin  ( KBerTlvEventDownloadTag );

        // event list
        envelope.AddTag ( KTlvEventListTag );
        envelope.AddByte( KEventAccessTechnologyChange );

        // device identities
        envelope.AddTag ( KTlvDeviceIdentityTag );
        envelope.AddByte( KMe );
        envelope.AddByte( KSim );

        // channel status
        envelope.AddTag( KTlvAccessTechnologyTag );
        envelope.AddByte( accTechChange );

        // send
        ret =  iSatMessHandler->UiccCatReqEnvelope(
            GetTransactionId(), envelope.End() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatEventDownload::ForceLocationStatusEnvelope
// Force sending of Location status envelope if ENS supported in product profile
// and SET UP EVENT LIST terminal response is sent
// -----------------------------------------------------------------------------
//
#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
TInt CSatEventDownload::ForceLocationStatusEnvelope()
    {
    TFLOGSTRING("TSY: CSatEventDownload::ForceLocationStatusEnvelope");
    OstTrace0( TRACE_NORMAL, CSATEVENTDOWNLOAD_FORCELOCATIONSTATUSENVELOPE, "CSatEventDownload::ForceLocationStatusEnvelope" );

    TInt ret( KErrNone );
    if ( iEventList.IsEnabled( RSat::KLocationStatus )
        && iSatMessHandler->EnsSupported()
        && iNetCellInfoIndReceived )
        {
        TFLOGSTRING("TSY: CSatEventDownload::ForceLocationStatusEnvelope, Event list includes Location status and ENS is supported in PP");
        OstTrace0( TRACE_NORMAL, DUP1_CSATEVENTDOWNLOAD_FORCELOCATIONSTATUSENVELOPE, "CSatEventDownload::ForceLocationStatusEnvelope, Event list includes Location status and ENS is supported in PP" );
        ret = SendLocationStatusEnvelope( 
            iSatMessHandler->LocationInfo() );
        }
    return ret;
    }
#endif // INFO_PP_ENHANCED_NETWORK_SELECTION
// End of File
