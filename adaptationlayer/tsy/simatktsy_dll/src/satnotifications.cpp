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
#include "satnotifications.h"   // sat notifications class
#include "satmessaging.h"       // sat messaging class
#include "satmesshandler.h"     // sat message handler class
#include "sattimer.h"           // sat timer class
#include "ber_tlv.h"            // sat ber-tlv classes
#include "satutil.h"            // sat utility class

#include <tisi.h>               // isi message
#include <net_modemisi.h>       // net server
#include <gssisi.h>             // gss server
#include <atkisi.h>             // atk server
#include <uiccisi.h>            // uicc server
#include <satcs.h>
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satnotificationstraces.h"
#endif



// CONSTANTS

// Character code
const TUint8 KPlusMarkCharacterCode                 = 0x2B;

// Send SM
const TUint8 KTPDUMandatoryFieldsSmsSubmitUdl       = 0x06;
const TUint8 KTPDUMandatoryFieldsSmsCommandUdl      = 0x07;
const TUint8 KTPDUMandatoryFieldsSmsSubmitDcs       = 0x05;
const TUint8 KSmsVPFEnhanced                        = 0x01;
const TUint8 KSmsVPFRelative                        = 0x02;
const TUint8 KSmsVPFAbsolute                        = 0x03;
const TUint8 KSmsVpFormatMask                       = 0x03;
const TUint8 KSATSmsMTISubmitOrSubmitReport         = 0x01;
const TUint8 KSmsDcsDefaultAlphabet                 = 0x00;
const TUint8 KMinSmsTpduLength                      = 0x07;

// Masks
const TUint8 KMask0                                 = 0x00;
const TUint8 KMask3                                 = 0x03;
const TUint8 KMaskC0                                = 0xC0;
const TUint8 KMaskF0                                = 0xF0;
const TUint8 KMaskF3                                = 0xF3;
const TUint8 KMaskFB                                = 0xFB;

// Additional info
const TUint8 KNoCause                               = 0x00;

// SetUpCall
const TUint8 KOnlyIfNotBusy                         = 0x00;
const TUint8 KOnlyIfNotBusyWithRedial               = 0x01;
const TUint8 KHoldOtherCalls                        = 0x02;
const TUint8 KHoldOtherCallsWithRedial              = 0x03;
const TUint8 KDisconnectOtherCalls                  = 0x04;
const TUint8 KDisconnectOtherCallsWithRedial        = 0x05;

// SetUpEventList
const TUint8 KMTCall                                = 0x00;
const TUint8 KCallConnected                         = 0x01;
const TUint8 KCallDisconnected                      = 0x02;
const TUint8 KLocationStatus                        = 0x03;
const TUint8 KUserActivity                          = 0x04;
const TUint8 KIdleScreenAvailable                   = 0x05;
const TUint8 KCardReaderStatus                      = 0x06;
const TUint8 KLanguageSelection                     = 0x07;
const TUint8 KBrowserTermination                    = 0x08;
const TUint8 KDataAvailable                         = 0x09;
const TUint8 KChannelStatus                         = 0x0A;
const TUint8 KAccessTechnologyChange                = 0x0B;
const TUint8 KDisplayParamsChanges                  = 0x0C;
const TUint8 KLocalConnection                       = 0x0D;


// Timer management
const TUint8 KMinTimerValue                         = 0;
const TUint  KMaxTimerValueInSeconds                = 86400;

// Launch browser
const TUint8 KLaunchBrowserCmdQualifierNotUsed      = 0x01;
const TUint8 KLaunchBrowserCmdQualifierReserved     = 0x04;
const TUint8 KCsdBearer                             = 0x01;
const TUint8 KGprsBearer                            = 0x03;
const TUint8 KDefaultBrowser                        = 0x00;
#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
const TUint8 KWMLBrowser                            = 0x01;
const TUint8 KHTMLBrowser                           = 0x02;
const TUint8 KXHTMLBrowser                          = 0x03;
const TUint8 KCHTMLBrowser                          = 0x04;
#endif

// Bearer independent protocol
const TUint8 KBipCsdBearer                          = 0x01;
const TUint8 KBipGprsBearer                         = 0x02;
const TUint8 KBipDefaultBearer                      = 0x03;
const TUint8 KBipLocalLinkTechnologyIndependentBearer  = 0x04;
const TUint8 KBipBluetoothBearer                    = 0x05;
const TUint8 KBipIrDABearer                         = 0x06;
const TUint8 KBipRS232Bearer                        = 0x07;
const TUint8 KBipUSBBearer                          = 0x10;
const TUint8 KBipSendDataImmediately                = 0x01;

// Language notification
const TUint8 KSpecificLanguage                      = 0x01;

// Elementary files
const TUint8 KRefresh1StLevelDedicatedFileHeader    = 0x7F;
const TUint8 KRefresh2StLevelDedicatedFileHeader    = 0x5F;

// SIM/ME Interface transport protocol type coding
const TUint8 KProtocolUdp                           = 0x01;
const TUint8 KProtocolTcp                           = 0x02;

// Link Establishment
const TUint8 KLinkEstablishmentMask                 = 0x01;

// Reconnection mode
const TUint8 KReconnectionModeMask                  = 0x02;


// LOCAL FUNCTION PROTOTYPES

// Declaration needed here for ARMV5 because method is used in Notifications
// base, but method implementation is bit later.
CSatNotificationsBase::TAllowedResults operator+(
    const RSat::TPCmdResult aResultA, const RSat::TPCmdResult aResultB );

// ==================== MEMBER FUNCTIONS ====================================

// -----------------------------------------------------------------------------
// CSatNotificationsBase::CSatNotificationsBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::CSatNotificationsBase
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        iSatMessHandler( aSatMessHandler ),
        iSatMessaging( aSatMessaging ),
        iTransId( KZero ),
        iReqHandle( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFICATIONSBASE_CSATNOTIFICATIONSBASE, "CSatNotificationsBase::CSatNotificationsBase" );
    iCommandDetails.Zero();

    // Following results are accepted by all commands
    iAllowedResults = RSat::KSuccess + RSat::KPartialComprehension
        + RSat::KMissingInformation + RSat::KMeUnableToProcessCmd
        + RSat::KCmdBeyondMeCapabilities + RSat::KCmdTypeNotUnderstood
        + RSat::KCmdDataNotUnderstood + RSat::KCmdNumberNotKnown;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::~CSatNotificationsBase
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::~CSatNotificationsBase()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFICATIONSBASE_CSATNOTIFICATIONSBASE, "CSatNotificationsBase::~CSatNotificationsBase" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::CancelNotification
// Cancels the current notification request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsBase::CancelNotification
        (
        const TTsyReqHandle aReqHandle  // Request handle
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFICATIONSBASE_CANCELNOTIFICATION, "CSatNotificationsBase::CancelNotification" );
    TFLOGSTRING("TSY: CSatNotificationsBase::CancelNotification");

    // Check that the handle is valid
    if ( aReqHandle == iReqHandle )
        {
        iSatMessaging->ReqCompleted( aReqHandle, KErrCancel );
        }
    else
        {
        // Handle is not valid
        iSatMessaging->ReqCompleted( aReqHandle, KErrCorrupt );
        }

    iReqHandle = NULL;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::TerminalResponse
// Handles the terminal response of a proactive command
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsBase::TerminalResponse
        (
        TDes8* /* aRsp */   // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFICATIONSBASE_TERMINALRESPONSE, "CSatNotificationsBase::TerminalResponse" );
    // Base class does not do anything
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::GetCmdDetails
// Returns a reference to member iCommandDetails
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDes8& CSatNotificationsBase::GetCmdDetails()
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFICATIONSBASE_GETCMDDETAILS, "CSatNotificationsBase::GetCmdDetails" );
    return iCommandDetails;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::GetTransactionId
// Returns the transaction identifier of the current PCmd.
// This is a Nokia specific value found in ISI messages.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CSatNotificationsBase::GetTransactionId()
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFICATIONSBASE_GETTRANSACTIONID, "CSatNotificationsBase::GetTransactionId" );
    return iTransId;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::TsyReqHandle
// Returns the TsyReqHandle of a request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTsyReqHandle CSatNotificationsBase::TsyReqHandle()
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFICATIONSBASE_TSYREQHANDLE, "CSatNotificationsBase::TsyReqHandle" );
    return iReqHandle;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::CompleteRequest
// Completes the Request notification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotificationsBase::CompleteRequest
        (
        const TInt aError   // Possible error
        )
    {
    OstTraceExt2( TRACE_NORMAL, CSATNOTIFICATIONSBASE_COMPLETEREQUEST, "CSatNotificationsBase::CompleteRequest, Handle: %d, Error: %d", (TInt)iReqHandle, aError );
    TFLOGSTRING3("CSatNotificationsBase::CompleteRequest. \n\t\t\t Handle:%d\n\t\t\t Error:%d",
               iReqHandle,
               aError);
    TTsyReqHandle tempReqHandle = iReqHandle;
    iReqHandle = NULL;
    iSatMessaging->ReqCompleted( tempReqHandle, aError );
    TFLOGSTRING("CSatNotificationsBase::CompleteRequest. Request is now completed");
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFICATIONSBASE_COMPLETEREQUEST, "CSatNotificationsBase::CompleteRequest. Request is now completed" );
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::TAllowedResults::TAllowedResults
// C++ default constructor of the nested class.
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::TAllowedResults::TAllowedResults() : iResultField( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TBool CSatNotificationsBase::TAllowedResults::operator==
// The equality operator (==) for TAllowedResults
// -----------------------------------------------------------------------------
//
TBool CSatNotificationsBase::TAllowedResults::operator==
        (
        const RSat::TPCmdResult aResult
        ) const
    {
    // Create mask from result for comparation
    TAllowedResults mask;
    mask = aResult;

    // Compare with bit-wise AND operation
    TBool match( iResultField & mask.iResultField );

    // Unfortunately AND operator does not seem to work if the mask contains a
    // value in the high order 32 bits of a 64 bit number. This case has to be
    // checked with following macro.
    if ( I64HIGH( mask.iResultField ) )
        {
        // Mask value is in the high order of the number so compare these.
        match = I64HIGH( iResultField ) & I64HIGH( mask.iResultField );
        }

    // Open System Trace doesn't support tracing in nested classes
    TFLOGSTRING3( "TAllowedResults::operator==: Bit Field: %08x %08x",
        I64HIGH( iResultField ), I64LOW( iResultField ) );
    TFLOGSTRING2( "TAllowedResults::operator==: Match: %d",
        ( match ? 1 : 0 ) );

    return match;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::TAllowedResults::operator!=
// The not-equal operator (!=) for TAllowedResults.
// -----------------------------------------------------------------------------
//
TBool CSatNotificationsBase::TAllowedResults::operator!=
        (
        const RSat::TPCmdResult aResult
        ) const
    {
    // Return the value of equals-operation (==) as inverted.
    return !( *this == aResult );
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::TAllowedResults::operator=
// The assignment operator (=) for TAllowedResults
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::TAllowedResults&
    CSatNotificationsBase::TAllowedResults::operator=
        (
        const RSat::TPCmdResult aResult
        )
    {
    // Convert result to bit field and assing internal field to it.
    iResultField = CreateBitField( aResult );

    // Return reference to this
    return *this;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::operator+=
// The increment operator (+=) for TAllowedResults.
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::TAllowedResults&
    CSatNotificationsBase::TAllowedResults::operator+=
        (
        const RSat::TPCmdResult aResult
        )
    {
    // Append result to bit field by calling approriate plus operator
    // and return reference to this
    return *this = *this + aResult;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::operator+=
// The increment operator (+=) for TAllowedResults.
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::TAllowedResults&
    CSatNotificationsBase::TAllowedResults::operator+=
        (
        const CSatNotificationsBase::TAllowedResults aResults
        )
    {
    // Append result fields together
    iResultField = iResultField | aResults.iResultField;

    // Return reference to this
    return *this;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::TAllowedResults::operator+
// The increment operator (+) for TAllowedResults.
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::TAllowedResults&
    CSatNotificationsBase::TAllowedResults::operator+
        (
        const RSat::TPCmdResult aResult
        )
    {
    // Convert to bit field and append to result field.
    iResultField = iResultField | CreateBitField( aResult );

    // Return reference to this
    return *this;
    }

// -----------------------------------------------------------------------------
// operator+
// Friend increment operator (+) for adding two TPCmdResults together as
// bitfield.
// -----------------------------------------------------------------------------
//
CSatNotificationsBase::TAllowedResults operator+
        (
        const RSat::TPCmdResult aResultA,
        const RSat::TPCmdResult aResultB
        )
    {
    // Create temporary bit field and append values into it.
    // Because internal TAllowedResults is used, this method is declared as a
    // friend
    CSatNotificationsBase::TAllowedResults temp;

    temp = aResultA;
    temp += aResultB;

    return temp;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsBase::CreateBitField
// Converts result code into bitfield representation.
// -----------------------------------------------------------------------------
//
inline TUint64
    CSatNotificationsBase::TAllowedResults::CreateBitField
        (
        const RSat::TPCmdResult aResult
        ) const
    {
    // Create a bitfield from a result by shifting '1' to the left.
    // Number of shifts depend on the result number:
    // ...0000 0001 = KSuccess (0x00)
    // ...0000 0010 = KPartialComprehension(0x01)
    // ...0001 0000 = KSuccessRequestedIconNotDisplayed(0x04)
    // and so forth.

    return static_cast<TInt64>( 1 ) << ( aResult );
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::CSatNotifyDisplayText
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyDisplayText::CSatNotifyDisplayText
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iDisplayTextV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYDISPLAYTEXT_CSATNOTIFYDISPLAYTEXT, "CSatNotifyDisplayText::CSatNotifyDisplayText" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KBackwardModeRequestedByUser
        + RSat::KNoResponseFromUser + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::~CSatNotifyDisplayText
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyDisplayText::~CSatNotifyDisplayText()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYDISPLAYTEXT_CSATNOTIFYDISPLAYTEXT, "CSatNotifyDisplayText::~CSatNotifyDisplayText" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::MessageReceived
// Handles a DisplayText proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyDisplayText::MessageReceived( const TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYDISPLAYTEXT_MESSAGERECEIVED, "CSatNotifyDisplayText::MessageReceived" );
    TFLOGSTRING("TSY: CSatNotifyDisplayText::MessageReceived");
    // Completion return value
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iCommandDetails.Copy( commandDetails.Data() );
    // Get command qualifier
    TUint8 cmdQualifier(
        commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
    // Save ISA/CellMo transaction id
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    // Test if pending request
    if ( !iReqHandle )
        {
        TFLOGSTRING("TSY: CSatNotifyDisplayText::MessageReceived - Request Off");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYDISPLAYTEXT_MESSAGERECEIVED, "CSatNotifyDisplayText::MessageReceived - Request Off" );

        // Request not on, returning response immediately
        iSatMessHandler->DisplayTextTerminalResp(
            iTransId,                       // Transaction id
            iCommandDetails,                // Command details tlv
            RSat::KMeUnableToProcessCmd,    // Result
            RSat::KNoAdditionalInfo );      // Additional info
        }
    else
        {
        // Pending request, adapts the PCmd ISI message.
        // Let's fill the display text structure
        RSat::TDisplayTextV2& displayTextV2 = ( *iDisplayTextV2Pckg )();
        // Store command number
        displayTextV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Priority scheme
        if ( KPriorityMask & cmdQualifier )
            {
            // Display priority high
            displayTextV2.iPriority = RSat::EHighPriority;
            }
        else
            {
            // Display priority normal
            displayTextV2.iPriority = RSat::ENormalPriority;
            }
        // Clearing scheme
        if ( KMessageClearMask & cmdQualifier)
            {
            // Wait for user to clear the message
            displayTextV2.iClearScreenTrigger = RSat::EUserClear;
            }
        else
            {
            // Clear message after a delay
            displayTextV2.iClearScreenTrigger = RSat::EClearAfterDelay;
            }
        // Immediate response TLV
        CTlv immediateResponse;
        TInt returnValue( berTlv.TlvByTagValue(
            &immediateResponse,
            KTlvImmediateResponseTag ) );
        if ( KErrNotFound != returnValue )
            {
            // Respond immediately
            displayTextV2.iImmediateRsp = RSat::EImmediateRsp;
            }
        else
            {
            // Do not respond immediately
            displayTextV2.iImmediateRsp = RSat::ENoImmediateRsp;
            }
        // Text to display TLV
        displayTextV2.iText.Zero();
        CTlv textString;
        returnValue = berTlv.TlvByTagValue( &textString,
            KTlvTextStringTag );
        if ( KErrNone == returnValue )
            {
            // Check if Icon Id found
            // Iconid (optional)
            TSatUtility::FillIconStructure(
                berTlv, displayTextV2.iIconId );

            // Text string tag is found, now
            // check if the text string is null.
            if ( textString.GetLength() )
                {
                // Convert and set text
                TSatUtility::SetText( textString, displayTextV2.iText );
                }
            else if ( RSat::ENoIconId != displayTextV2.iIconId.iQualifier )
                {
                // Text string is a null data object and there is icon qualifier
                // presented
                // Return terminal response immediately.
                iSatMessHandler->DisplayTextTerminalResp(
                    iTransId,           // Transaction id
                    iCommandDetails,                    // Command details TLV
                    RSat::KCmdDataNotUnderstood,        // Result
                    KNoCause );                         // Additional info
                ret = KErrCorrupt;
                }
            }
        else
            {
            iSatMessHandler->DisplayTextTerminalResp(
                iTransId,                           // Transaction id
                iCommandDetails,                    // Command details TLV
                RSat::KErrorRequiredValuesMissing,  // Result
                KNoCause );                         // Additional info
            ret = KErrCorrupt;
            }
        // Duration,( optional ) see 3GPP TS 31.111
        TSatUtility::FillDurationStructure(
            berTlv, displayTextV2.iDuration );

        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayTextNotify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a DisplayText PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyDisplayText::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYDISPLAYTEXT_NOTIFY, "CSatNotifyDisplayText::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifyDisplayText::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iDisplayTextV2Pckg = static_cast< RSat::TDisplayTextV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KDisplayText );
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::TerminalResponse
// Handles a DisplayText terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyDisplayText::TerminalResponse
        (
        TDes8* aRsp     // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYDISPLAYTEXT_TERMINALRESPONSE, "CSatNotifyDisplayText::TerminalResponse" );
    TFLOGSTRING("CSatNotifyDisplayText::TerminalResponse");
    // Completion return value
    TInt   ret( KErrNone );
    // Additional info
    TUint8 additionalInfo( 0 );
    // Cast the input parameter to EtelSat data structure
    RSat::TDisplayTextRspV1Pckg* aRspPckg =
            reinterpret_cast<RSat::TDisplayTextRspV1Pckg*>( aRsp );
    RSat::TDisplayTextRspV1& rspV1 = ( *aRspPckg ) ();
    // Check that general result value is valid

    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Equipment) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo = static_cast<TUint8>( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    // Prepare and send the terminal response to ISA/CellMo sw.
    iSatMessHandler->DisplayTextTerminalResp(
        iTransId,                                     // Transaction id
        iCommandDetails,                             // Command details tlv
        static_cast< TUint8 >( rspV1.iGeneralResult ),  // Result
        additionalInfo );                            // Additional info
    // return completion value.
    return ret;

    }


// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::CSatNotifyGetInkey
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyGetInkey::CSatNotifyGetInkey
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iGetInkeyV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETINKEY_CSATNOTIFYGETINKEY, "CSatNotifyGetInkey::CSatNotifyGetInkey" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KBackwardModeRequestedByUser
        + RSat::KNoResponseFromUser + RSat::KHelpRequestedByUser
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::~CSatNotifyGetInkey
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyGetInkey::~CSatNotifyGetInkey()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYGETINKEY_CSATNOTIFYGETINKEY, "CSatNotifyGetInkey::~CSatNotifyGetInkey" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::MessageReceived
// Handles a GetInkey proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyGetInkey::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETINKEY_MESSAGERECEIVED, "CSatNotifyGetInkey::MessageReceived" );
    TFLOGSTRING( "TSY: CSatNotifyGetInkey::MessageReceived" );

    // Completion return value
    TInt ret( KErrNone );

    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );

    // Get command details
    CTlv cmdDetails;
    berTlv.TlvByTagValue( &cmdDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( cmdDetails.Data() );

    // Save ISA/CellMo transaction id
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );

    // Get command qualifier
    TUint8 cmdQualifier( cmdDetails.GetShortInfo( ETLV_CommandQualifier ) );

    // Test if pending request
    if ( !iReqHandle )
        {
        RSat::TGetInkeyRspV2 error;
        error.iGeneralResult = RSat::KMeUnableToProcessCmd;
        error.iInfoType = RSat::KNoAdditionalInfo;

        iSatMessHandler->GetInkeyTerminalResp(
            iTransId,                    // Transaction Id
            iCommandDetails,             // Command details
            error                        // Error result
            );
        }
    else
        {
        // Fill the get inkey structure
        RSat::TGetInkeyV2& getInkeyV2 = ( *iGetInkeyV2Pckg )();

        // Store command number
        getInkeyV2.SetPCmdNumber(
            cmdDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Transfer command qualifier parameter to ETelSat data structure
        if ( KYesNoMask & cmdQualifier )
            {
            // The character set bits are disabled and yes/no response
            // is requested
            getInkeyV2.iRspFormat = RSat::EYesNo;
            }
        else
            {
            // The character set bits are enabled
            if ( KAlphabetMask & cmdQualifier )
                {
                // Character set selected
                if ( KUCS2AlphabetMask & cmdQualifier )
                    {
                    // UCS2 alphabet selected
                    getInkeyV2.iRspFormat = RSat::ECharUcs2Alphabet;
                    }
                else
                    {
                    // SMS default alphabet selected
                    getInkeyV2.iRspFormat = RSat::ECharSmsDefaultAlphabet;
                    }
                }
            else
            {
            // Digits only selected
            getInkeyV2.iRspFormat = RSat::EDigitOnly;
            }

        }
        // Help information.
        if ( KHelpAvailabilityMask & cmdQualifier )
            {
            // Help information available
            getInkeyV2.iHelp = RSat::EHelpAvailable;
            }
        else
            {
            // No help
            getInkeyV2.iHelp = RSat::ENoHelpAvailable;
            }
        // Response Mode
        getInkeyV2.iMode = RSat::EGetInkeyRspModeNotSet;
        if ( KImmediateDigitResponse & cmdQualifier )
            {
            // an immediate digit response (0-9, * and #) is requested.
            getInkeyV2.iMode = RSat::EImmediateDigitRsp;
            }
        else
            {
            getInkeyV2.iMode = RSat::EUserRspDisplayed;
            }
        // Get Inkey text
        CTlv textString;
        if ( KErrNone == berTlv.TlvByTagValue(
                &textString, KTlvTextStringTag ) )
            {
            // Convert and set text
            TSatUtility::SetText( textString, getInkeyV2.iText );
            }
        else
            {
            RSat::TGetInkeyRspV2 error;
            error.iGeneralResult = RSat::KErrorRequiredValuesMissing;
            error.iInfoType = RSat::KNoAdditionalInfo;

            iSatMessHandler->GetInkeyTerminalResp(
                iTransId,                           // Transaction id
                iCommandDetails,                    // Command details TLV
                error );                            // Error Result
            ret = KErrCorrupt;
            }
        // IconId Optional
        TSatUtility::FillIconStructure( berTlv,
            getInkeyV2.iIconId );
        // Duration
        TSatUtility::FillDurationStructure( berTlv,
            getInkeyV2.iDuration );
        // Complete request to Etel
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a GetInkey PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyGetInkey::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYGETINKEY_NOTIFY, "CSatNotifyGetInkey::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2( "TSY: CSatNotifyGetInkey::Notify. Handle: %d", aReqHandle );

    iReqHandle = aReqHandle;
    iGetInkeyV2Pckg = static_cast< RSat::TGetInkeyV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KGetInkey );
    }


// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::TerminalResponse
// Handles a GetInkey terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInkey::TerminalResponse
        (
        TDes8* aRsp // Response packet from ETel
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETINKEY_TERMINALRESPONSE, "CSatNotifyGetInkey::TerminalResponse" );
    TFLOGSTRING("TSY: CSatNotifyGetInkey::TerminalResponse" );

    TInt ret( KErrNone );

    RSat::TGetInkeyRspV2Pckg& aRspPckg =
            reinterpret_cast<RSat::TGetInkeyRspV2Pckg&>( *aRsp );
    RSat::TGetInkeyRspV2& rspV2 = aRspPckg();

    // Checking that infotype is valid
    switch ( rspV2.iInfoType )
        {
        case RSat::KMeProblem:
        case RSat::KTextString:
            {
            if ( !rspV2.iAdditionalInfo.Length() )
                {
                // These info types require additional info
                ret = KErrCorrupt;
                }
            break;
            }
        case RSat::KNoAdditionalInfo:
            {
            if ( rspV2.iAdditionalInfo.Length() )
                {
                // For this type, AI shouldn't be present
                ret = KErrCorrupt;
                }
            break;
            }
        default:
            {
            // Unexpected value
            ret = KErrCorrupt;
            break;
            }
        }

    // Checking that the response type is valid
    switch ( rspV2.iRspFormat )
        {
        case RSat::EDigitOnly:
        case RSat::ECharSmsDefaultAlphabet:
        case RSat::ECharUcs2Alphabet:
        case RSat::EYesNo:
            {
            // Ok
            break;
            }
        default:
            {
            // Unexpected value
            ret = KErrCorrupt;
            break;
            }
        }

    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Unexpected value
        ret = KErrCorrupt;
        }

    if ( ( RSat::ETimeUnitNotSet == rspV2.iDuration.iTimeUnit ) )
        {
        // Time should have been initialized
        ret = KErrCorrupt;
        }

    iSatMessHandler->GetInkeyTerminalResp(
        iTransId,                                  // Transaction id
        iCommandDetails,                           // Command details
        rspV2 );                                   // Response data

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyGetInput::CSatNotifyGetInput
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyGetInput::CSatNotifyGetInput
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iGetInputV1Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETINPUT_CSATNOTIFYGETINPUT, "CSatNotifyGetInput::CSatNotifyGetInput" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KBackwardModeRequestedByUser
        + RSat::KNoResponseFromUser + RSat::KHelpRequestedByUser
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInput::~CSatNotifyGetInput
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyGetInput::~CSatNotifyGetInput()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYGETINPUT_CSATNOTIFYGETINPUT, "CSatNotifyGetInput::~CSatNotifyGetInput" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInput::MessageReceived
// Handles a GetInput proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyGetInput::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETINPUT_MESSAGERECEIVED, "CSatNotifyGetInput::MessageReceived" );
    TFLOGSTRING("CSatNotifyGetInput::MessageReceived");
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        TBuf<1> emptyString;
        // Request not on, returning response immediately
        iSatMessHandler->GetInputTerminalResp(
            iTransId,                          // Transaction id
            iCommandDetails,                    // Command details tlv
            RSat::KMeUnableToProcessCmd,        // Result
            KNoCause,                           // Additional info, KNoCause: 0
            emptyString,                        // No string
            RSat::ESmsDefaultAlphabetUnpacked );// data coding scheme

        }
    else
        {
        // Fill the get input structure
        RSat::TGetInputV1& getInputV1 = ( *iGetInputV1Pckg )();
        // Store command number
        getInputV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Get command qualifier
        TUint8 cmdQualifier(
            commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
        // First comes the user input display option
        if ( KUserInputMask & cmdQualifier )
            {
            // User input hidden
            getInputV1.iInputDisplayOption = RSat::EHideUserInput;
            }
        else
            {
            // User input shown
            getInputV1.iInputDisplayOption = RSat::EDisplayUserInput;
            }
        // Next the response format expected
        if ( KUCS2AlphabetMask & cmdQualifier )
            {
            getInputV1.iRspFormat = RSat::EUcs2Alphabet;
            }
        else if ( KAlphabetMask & cmdQualifier )
            {
            if ( KGetInputSmsPackedMask & cmdQualifier )
                {
                // SMS default alphabet
                getInputV1.iRspFormat = RSat::ESmsDefaultAlphabetPacked;
                }
            else
                {
                getInputV1.iRspFormat = RSat::ESmsDefaultAlphabetUnpacked;
                }
            }
        else
            {
            if  ( KGetInputSmsPackedMask & cmdQualifier )
                {
                // Digit only
                getInputV1.iRspFormat = RSat::EDigitOnlyPacked;
                }
            else
                {
                getInputV1.iRspFormat = RSat::EDigitOnlyUnpacked;
                }
            }
        // Help information.
        if ( KHelpAvailabilityMask & cmdQualifier )
            {
            // Help information available
            getInputV1.iHelp = RSat::EHelpAvailable;
            }
        else
            {
            // No help
            getInputV1.iHelp = RSat::ENoHelpAvailable;
            }

        // Get Input text
        getInputV1.iText.Zero();
        CTlv textString;
        TInt returnValue( berTlv.TlvByTagValue( &textString,
                                                KTlvTextStringTag ) );
         if ( KErrNone == returnValue )
            {
            // Set and conver text
            TSatUtility::SetText( textString, getInputV1.iText );
            }
        else
            {
            TBuf<1> emptyString;
            iSatMessHandler->GetInputTerminalResp(
                iTransId,                           // Transaction id
                iCommandDetails,                    // Command details tlv
                RSat::KErrorRequiredValuesMissing,  // Result
                KNoCause,                           // KNoCause: 0
                emptyString,                        // No string
                RSat::ESmsDefaultAlphabetUnpacked );// Data coding scheme

            ret = KErrCorrupt;
            }
        TFLOGSTRING2("TSY: GetInput, text: %S", &getInputV1.iText );
        OstTraceExt1( TRACE_NORMAL, DUP1_CSATNOTIFYGETINPUT_MESSAGERECEIVED, "CSatNotifyGetInput::MessageReceived GetInput, text: %S", getInputV1.iText );

        // Response length expected
        CTlv responseLength;
        returnValue = berTlv.TlvByTagValue( &responseLength,
                                            KTlvResponseLengthTag );
        if ( KErrNone == returnValue )
            {
            // Set minimum response length
            getInputV1.iRspLength.iMinRspLength =
                responseLength.GetShortInfo( ETLV_MinimumLengthOfResponse );

            if ( RSat::KGetInputTextMaxSize <
                getInputV1.iRspLength.iMinRspLength )
                {
                TBuf<1> emptyString;
                iSatMessHandler->GetInputTerminalResp(
                    iTransId,                           // Transaction id
                    iCommandDetails,                    // Command details tlv
                    RSat::KCmdDataNotUnderstood,        // Result
                    KNoCause,                           // KNoCause: 0
                    emptyString,                        // No string
                    RSat::ESmsDefaultAlphabetUnpacked );// Data coding scheme

                ret = KErrCorrupt;
                }
            // Set maximum response length
            getInputV1.iRspLength.iMaxRspLength =
                responseLength.GetShortInfo( ETLV_MaximumLengthOfResponse );
            }
        else
            {
            // Mandatory reponse length tlv is missing
            TBuf<1> emptyString;
            iSatMessHandler->GetInputTerminalResp(
                iTransId,                           // Transaction id
                iCommandDetails,                    // Command details tlv
                RSat::KErrorRequiredValuesMissing,  // Result
                KNoCause,                           // KNoCause: 0
                emptyString,                        // No string
                RSat::ESmsDefaultAlphabetUnpacked );// Data coding scheme

            ret = KErrCorrupt;
            }
        // Default text to display (optional)
        getInputV1.iDefaultText.Zero();
        returnValue = berTlv.TlvByTagValue( &textString, KTlvDefaultTextTag );
        // If default text exist
        if ( KErrNone == returnValue )
            {
            TSatUtility::SetText( textString, getInputV1.iDefaultText );
            }
        // Iconid ( optional )
        TSatUtility::FillIconStructure( berTlv, getInputV1.iIconId );
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInput::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a GetInput PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyGetInput::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYGETINPUT_NOTIFY, "CSatNotifyGetInput::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifyGetInput::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iGetInputV1Pckg = static_cast< RSat::TGetInputV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KGetInput );
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInput::TerminalResponse
// Handles a GetInput terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInput::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETINPUT_TERMINALRESPONSE, "CSatNotifyGetInput::TerminalResponse" );
    TFLOGSTRING("CSatNotifyGetInput::TerminalResponse");
    TInt ret( KErrNone );
    TUint8 additionalInfo( 0 );

    RSat::TGetInputRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TGetInputRspV1Pckg* >( aRsp );
    RSat::TGetInputRspV1& rspV1 = ( *aRspPckg ) ();

    // Checking that the response type is valid. If additional info contain
    // a text string, response format has to be set.
    if ( RSat::EGetInputRspFormatNotSet == rspV1.iRspFormat
        && RSat::KTextString == rspV1.iInfoType )
        {
        ret = KErrCorrupt;
        }

    // Check the additional info
    if ( ( RSat::KTextString == rspV1.iInfoType )
         || ( RSat::KSuccess  == rspV1.iGeneralResult ) )
        {
        // Making sure first that the string is not empty
        if ( RSat::KNoAdditionalInfo != rspV1.iInfoType
                && 0 == rspV1.iAdditionalInfo.Length() )
            {
            ret = KErrCorrupt;
            }
        }
    // Check that general result values are valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // If there is Me (Mobile Entity) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo = static_cast< TUint8 >( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    iSatMessHandler->GetInputTerminalResp(
        iTransId,                                 // Transaction id
        iCommandDetails,                          // Command details Tlv
        static_cast< TUint8 >(
        rspV1.iGeneralResult ),                   // Result
        additionalInfo,                           // Additional info
        rspV1.iAdditionalInfo,                    // Input text
        static_cast< TUint8 >( rspV1.iRspFormat ) );    // Resp. format
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::CSatNotifyPlayTone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyPlayTone::CSatNotifyPlayTone
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iPlayToneV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPLAYTONE_CSATNOTIFYPLAYTONE, "CSatNotifyPlayTone::CSatNotifyPlayTone" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::~CSatNotifyPlayTone
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyPlayTone::~CSatNotifyPlayTone()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYPLAYTONE_CSATNOTIFYPLAYTONE, "CSatNotifyPlayTone::~CSatNotifyPlayTone" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::MessageReceived
// Handles a PlayTone proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyPlayTone::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPLAYTONE_MESSAGERECEIVED, "CSatNotifyPlayTone::MessageReceived" );
    TFLOGSTRING("CSatNotifyPlayTone::MessageReceived");
    //get ber tlv
    CBerTlv berTlv;
    TInt returnValue( KErrNone );
    returnValue = berTlv.BerTlv( aIsiMessage );
    //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        iSatMessHandler->PlayToneTerminalResp(
            iTransId,
            iCommandDetails,
            RSat::KMeUnableToProcessCmd,
            KNoCause ); // KNoCause: 0
        }
    else
        {
        // Fill the play tone structure
        RSat::TPlayToneV2& playToneV2 = ( *iPlayToneV2Pckg )();
        // command number
        playToneV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );

        // Alpha ID string (optional)
        CTlv alphaIdentifier;
        returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
            KTlvAlphaIdentifierTag );
        if ( KErrNotFound != returnValue )
            {
            TUint16 alphaIdLength = alphaIdentifier.GetLength();
            if ( alphaIdLength )
                {
                // get the alpha id
                TPtrC8 sourceString;
                sourceString.Set(
                    alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
                // convert and set the alpha id
                TSatUtility::SetAlphaId( sourceString ,
                    playToneV2.iAlphaId.iAlphaId );
                }
            // Set Alpha ID status
            if ( playToneV2.iAlphaId.iAlphaId.Length() )
                {
                playToneV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                }
            else
                {
                playToneV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
                }
            }
        else
            {
            playToneV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            }

        // Tone selection (optional)
        playToneV2.iTone = RSat::KToneNotSet;
        CTlv tone;
        returnValue = berTlv.TlvByTagValue( &tone, KTlvToneTag );
        if (returnValue != KErrNotFound)
            {
            playToneV2.iTone = ( RSat::TTone ) tone.GetShortInfo( ETLV_Tone );
            }
        // Duration of the tone (optional)
        TSatUtility::FillDurationStructure( berTlv,
            playToneV2.iDuration );
        // Icond Id (optional)
        TSatUtility::FillIconStructure( berTlv,
            playToneV2.iIconId );

        CompleteRequest( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a PlayTone PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyPlayTone::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYPLAYTONE_NOTIFY, "CSatNotifyPlayTone::Notify Handle: %u", aReqHandle );

    TFLOGSTRING2("CSatNotifyPlayTone::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iPlayToneV2Pckg = static_cast< RSat::TPlayToneV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KPlayTone );
    }

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::TerminalResponse
// Handles a PlayTone terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyPlayTone::TerminalResponse
        (
        TDes8* aRsp     // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPLAYTONE_TERMINALRESPONSE, "CSatNotifyPlayTone::TerminalResponse" );
    TFLOGSTRING("CSatNotifyPlayTone::TerminalResponse");
    TInt ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TPlayToneRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TPlayToneRspV1Pckg* >( aRsp );
    RSat::TPlayToneRspV1& rspV1 = ( *aRspPckg ) ();
    // Check that general result value is valid.
    // Note: When the phone is in silent mode, KMeUnableToProcessCmd
    // is returned as a result, with no additional info.
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Entity) error, additional info is needed
    if ( RSat::KMeProblem == rspV1.iInfoType )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() )
            {
            additionalInfo = TUint8( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    iSatMessHandler->PlayToneTerminalResp(
        iTransId,                                       // Transaction Id
        iCommandDetails,                                // Command number
        TUint8( rspV1.iGeneralResult ),                 // Result
        additionalInfo );                               // Additional info
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::CSatNotifyPollInterval
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyPollInterval::CSatNotifyPollInterval
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPOLLINTERVAL_CSATNOTIFYPOLLINTERVAL, "CSatNotifyPollInterval::CSatNotifyPollInterval" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::~CSatNotifyPollInterval
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyPollInterval::~CSatNotifyPollInterval()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYPOLLINTERVAL_CSATNOTIFYPOLLINTERVAL, "CSatNotifyPollInterval::~CSatNotifyPollInterval" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::MessageReceived
// Handles a PollInterval proactive command ISI message.
// No ETel pending request is required.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyPollInterval::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPOLLINTERVAL_MESSAGERECEIVED, "CSatNotifyPollInterval::MessageReceived" );
    TFLOGSTRING("CSatNotifyPollInterval::MessageReceived");
    //get ber tlv
    CBerTlv berTlv;
    TInt returnValue( KErrNone );
    berTlv.BerTlv( aIsiMessage );
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    TUint16 anIntervalInSeconds( KDefaultPollInterval );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    // Duration of the interval
    CTlv duration;
    returnValue = berTlv.TlvByTagValue( &duration, KTlvDurationTag );
    if ( KErrNone != returnValue )
        {
        // Required values are missing
        iSatMessHandler->PollIntervalTerminalResp( iTransId, iCommandDetails,
            RSat::KErrorRequiredValuesMissing, RSat::KNoSpecificMeProblem, 0);
        }
    else
        {
        TUint8 timeIntv( duration.GetShortInfo( ETLV_TimeInteval ) );

        // Time Interval 0 is SIM server's default value (25 seconds)
        if ( 0 != timeIntv )
            {
            TUint8 durationTimeUnit( duration.GetShortInfo( ETLV_TimeUnit ) );

            switch (durationTimeUnit)
                {
                case KMinutes:
                    {
                    anIntervalInSeconds = timeIntv * 60;
                    break;
                    }
                case KSeconds:
                    {
                    anIntervalInSeconds = timeIntv;
                    break;
                    }
                case KTenthsOfSeconds:
                    {
                        // Rounding
                    if ( 5 >= ( timeIntv % 10 ) )
                        {
                        // if the intervals the ME can offer are equidistant
                        // (higher and lower) from the SIM's request, the ME shall
                        // respond with the lower interval of the two.
                        anIntervalInSeconds = static_cast<TUint8> (timeIntv / 10 );
                        }
                    else
                        {
                        anIntervalInSeconds = static_cast<TUint8> ( (timeIntv / 10) + 1);
                        }
                    break;
                    }
                default:
                    {
                    returnValue = KErrNotFound;
                    TFLOGSTRING("TSY: CSatNotifyPollInterval::MessageReceived, Time unit did not match.");
                    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYPOLLINTERVAL_MESSAGERECEIVED, "CSatNotifyPollInterval::MessageReceived, Time unit did not match." );
                    // Command data not understood
                    iSatMessHandler->PollIntervalTerminalResp( iTransId, iCommandDetails,
                    RSat::KCmdDataNotUnderstood, RSat::KNoSpecificMeProblem, 0);
                    break;
                    }
                }//switch

            // Check interval limits
            if ( KMinPollInterval > anIntervalInSeconds )
                {
                anIntervalInSeconds = KMinPollInterval;
                }
            else if ( KMaxPollInterval < anIntervalInSeconds )
                {
                if ( iSatMessHandler->OldPollInterval() )
                    {
                    TFLOGSTRING("TSY: CSatNotifyPollInterval::MessageReceived MaxPolInterval");
                    OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYPOLLINTERVAL_MESSAGERECEIVED, "CSatNotifyPollInterval::MessageReceived MaxPolInterval" );
                    anIntervalInSeconds = KMaxPollInterval;
                    }
                else
                    {
                    TFLOGSTRING("TSY: CSatNotifyPollInterval::MessageReceived, DefaultPollInterval");
                    OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYPOLLINTERVAL_MESSAGERECEIVED, "CSatNotifyPollInterval::MessageReceived, DefaultPollInterval" );
                    // Use SIM server's default value (0)
                    anIntervalInSeconds = KDefaultPollInterval;
                    }
                }
            }
        else
            {
            // Use SIM server's default value (0)
            anIntervalInSeconds = KDefaultPollInterval;
            }

        if ( KErrNone == returnValue )
            {

            // A PCmd PollingOff may be executed.
            iSatMessHandler->SetPollingOff( EFalse );
            // Send SetPollInterval request to ISA CellMo side. The terminal response
            // will be sent to ISA CellMo side after SetPollInterval response.
            iSatMessHandler->SetPollingInterval(
                iTransId,
                static_cast<TUint8>( anIntervalInSeconds ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a PollInterval PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyPollInterval::Notify
        (
        const TTsyReqHandle /*aReqHandle*/, // Request handle
        TDes8*              /*aDataPtr*/    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPOLLINTERVAL_NOTIFY, "CSatNotifyPollInterval::Notify" );
    // PollInterval is completely implemented by SimAtkTsy. Client
    // insn't notified about the command.
    }


// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::CSatNotifySetUpMenu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySetUpMenu::CSatNotifySetUpMenu
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSetUpMenuV2Pckg( NULL ),
        iSatIsiMsg( NULL ),
        iItemsNextIndicatorRemoved( EFalse )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPMENU_CSATNOTIFYSETUPMENU, "CSatNotifySetUpMenu::CSatNotifySetUpMenu" );
    TFLOGSTRING("CSatNotifySetUpMenu::CSatNotifySetUpMenu");
    iToolKitName.Zero();

    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::~CSatNotifySetUpMenu
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySetUpMenu::~CSatNotifySetUpMenu()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPMENU_CSATNOTIFYSETUPMENU, "CSatNotifySetUpMenu::~CSatNotifySetUpMenu" );
    TFLOGSTRING("CSatNotifySetUpMenu::~CSatNotifySetUpMenu");
    if ( iSatIsiMsg )
        {
        delete iSatIsiMsg;
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::MessageReceived
// Handles a SetUpMenu proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpMenu::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived" );
    TFLOGSTRING("TSY:CSatNotifySetUpMenu::MessageReceived");
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
    // get ber tlv
    CBerTlv berTlv;
    returnValue = berTlv.BerTlv( aIsiMessage );
     //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
     // Get command qualifier
    TUint8 cmdQualifier( commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request is not pending. Store pcmd for later use
        // This may happen e.g. when changing ME's language (warm reset)
        if ( iSatIsiMsg )
            {
            // first clear buffer if there's already data
            delete iSatIsiMsg;
            iSatIsiMsg = NULL;
            }
        // Alloc memory
        iSatIsiMsg = aIsiMessage.GetBuffer().Alloc();

        if ( iSatIsiMsg )
            {
            TFLOGSTRING("TSY:CSatNotifySetUpMenu::MessageReceived, pcmd saved");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived, pcmd saved" );
            }
        else
            {
            TFLOGSTRING("TSY:CSatNotifySetUpMenu::MessageReceived, \
                out of memory unable to buffer received cmd");
            OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived, out of memory unable to buffer received cmd" );
            iSatMessHandler->SetUpMenuTerminalResp( iTransId,
                                                iCommandDetails,
                                                RSat::KMeUnableToProcessCmd,
                                                NULL );
            ret = KErrCorrupt;
            }
        }
    RSat::TSetUpMenuV2 menu;
    // Store command number
    menu.SetPCmdNumber(
        commandDetails.GetShortInfo( ETLV_CommandNumber ) );
    // Selection preference
    menu.iPreference = RSat::ESelectionPreferenceNotSet;
    if ( cmdQualifier & KSelectionUsingSoftKey )
        {
        menu.iPreference = RSat::ESoftKeyPreferred;
        }
    else
        {
        menu.iPreference = RSat::ENoSelectionPreference;
        }
    // Alpha Id string (optional)
    menu.iAlphaId.iAlphaId.Zero();
    //get alpha identifier tlv
    CTlv alphaIdentifier;
    returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
                                        KTlvAlphaIdentifierTag );
    if ( KErrNone != returnValue )
        {

        iSatMessHandler->SetUpMenuTerminalResp(
                                    iTransId,
                                    iCommandDetails,
                                    RSat::KErrorRequiredValuesMissing,
                                    RSat::KNoAdditionalInfo );
        ret = KErrCorrupt;
        }
    else
        {
        TUint16 alphaIdLength = alphaIdentifier.GetLength();
        TFLOGSTRING2("TSY: Alpha ID length:%d", alphaIdLength );
        OstTraceExt1( TRACE_NORMAL, DUP3_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived Alpha ID: %hu", alphaIdLength );
        if ( alphaIdLength > RSat::KAlphaIdMaxSize )
            {
            // String too long
            ret = KErrCorrupt;
            }
        else if ( alphaIdLength )
            {
            TPtrC8 temp;
            // get the alpha id
            temp.Set( alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
            // convert and set the alpha id
            TSatUtility::SetAlphaId( temp , menu.iAlphaId.iAlphaId );
            // Set SAT toolikit name, send SAT SMS logging purpose
            iToolKitName.Copy( menu.iAlphaId.iAlphaId );
            TFLOGSTRING2("TSY: SetUpMenu iToolKitName:%S",
                  &iToolKitName );
            OstTraceExt1( TRACE_NORMAL, DUP4_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived SetUpMenu iToolKitName: %S", iToolKitName );
            }
        else
            {
            TFLOGSTRING("TSY: CSatNotifySetUpMenu::MessageReceived,\
                Wrong length of alpha id.");
            OstTrace0( TRACE_NORMAL, DUP5_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived, Wrong length of alpha id." );
            }
        // Alpha Id status
        if ( menu.iAlphaId.iAlphaId.Length() )
            {
            menu.iAlphaId.iStatus = RSat::EAlphaIdProvided;
            }
        else
            {
            menu.iAlphaId.iStatus = RSat::EAlphaIdNull;
            }
        // Help information.
        if ( commandDetails.GetShortInfo( ETLV_CommandQualifier )
              & KHelpAvailabilityMask )
            {
            // Help information available
            menu.iHelp = RSat::EHelpAvailable;
            }
        else
            {
            // No help
            menu.iHelp = RSat::ENoHelpAvailable;
            }
        // Icon identifier
        TSatUtility::FillIconStructure( berTlv, menu.iIconId );
        // Item Icon identifier list
        CTlv itemsIconIdentifierList;
        TInt retValue = berTlv.TlvByTagValue( &itemsIconIdentifierList,
                                         KTlvItemIconIdentifierListTag );
        TPtrC8 iconIdList;
        menu.iIconListQualifier = RSat::EIconQualifierNotSet;

        if( KErrNone == retValue )
            {
            TUint8 iIconListQualifier( itemsIconIdentifierList.GetShortInfo(
                                             ETLV_IconListQualifier ) );
            iconIdList.Set( itemsIconIdentifierList.GetData( ETLV_IconIdentifierList ));

            if( iIconListQualifier )
                {
                menu.iIconListQualifier = RSat::ENotSelfExplanatory;
                }
            else
                {
                menu.iIconListQualifier = RSat::ESelfExplanatory;
                }
            }
        else if ( KErrNotFound == retValue )
            {
            menu.iIconListQualifier = RSat::ENoIconId;
            }
        else
            {
            TFLOGSTRING("TSY: CSatNotifySetUpMenu::MessageReceived, \
                Wrong return value of icon identifier list.");
            OstTrace0( TRACE_NORMAL, DUP6_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived, Wrong return value of icon identifier list." );
            }

        //Items Data
        TInt itemNbr( 0 );
        CTlv itemsData;
        returnValue = berTlv.TlvByTagValue( &itemsData, KTlvItemTag,
                                            itemNbr );
        TUint8 numberOfItemData( 0 );
        TUint16 stringLength( 0 );
        if( !returnValue )
            {
            CTlv itemsDataTemp;
            stringLength = ( itemsData.GetLength());
            TInt ret(KErrNone);
            // Set numberOfItemData
            while ( KErrNone == ret )
                {
                numberOfItemData++;
                ret = berTlv.TlvByTagValue( &itemsDataTemp, KTlvItemTag,
                                                numberOfItemData );
                }
            }

        //Item Next Action Indicator
        CTlv itemNextActionIndicator;
        retValue = berTlv.TlvByTagValue( &itemNextActionIndicator,
                                         KTlvItemsNextActionIndicatorTag );
        TPtrC8 itemNextIndicator;
        if ( KErrNone == retValue )
            {
            TPtrC8 itemNextIndicatorTemp;
            itemNextIndicatorTemp.Set(
                 itemNextActionIndicator.GetData( ETLV_ItemsNextActionIndicator ));

            //In case the number of items in this list does not match the number
            //of items in the menu the Items Next Action Indicator list is ignored by ME
            if(itemNextIndicatorTemp.Length() != numberOfItemData )
                {
                iItemsNextIndicatorRemoved = ETrue;
                }
            else
                   {
                itemNextIndicator.Set(itemNextIndicatorTemp);
                   }
            }

        //if first item is NULL -> remove existing menu
        if( stringLength != 0 )
            {
            for ( TInt8 i( 0 ) ; ( KErrNone == ret )
                && ( KErrNone == returnValue ) ; i++ )
                {
                // Filling up the menu items
                RSat::TItem newItem;
                // Suffle through all the menu items
                stringLength = 0;
                TFLOGSTRING2("TSY: item number:%d", i );
                OstTraceExt1( TRACE_NORMAL, DUP7_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived Item number: %hhd", i );
                // Fill the newitem
                newItem.iItemId = itemsData.GetShortInfo( ETLV_IdentifierOfItem );
                stringLength = itemsData.GetLength();
                stringLength--;
                TPtrC8 itemData = itemsData.GetData( ETLV_TextStringOfItem );
                // Menu item string, coded as EFadn (3gpp 11.11)
                newItem.iItemString.Zero();
                if ( ( KUCS2ArabicCoding == itemData[0] )
                    || ( KUCS2GreekCoding == itemData[0] )
                    || ( KUCS2TurkishCoding == itemData[0] ) )
                    {
                    TSatUtility::ConvertAlphaFieldsToUnicode( itemData,
                        newItem.iItemString );
                    }
                else
                    {
                    TSatUtility::ConvertSms7ToUnicode16( newItem.iItemString,
                          itemData );
                    }
                TFLOGSTRING2("TSY: SetUpMenu newItem.iItemString:%S",
                    &newItem.iItemString );
                OstTraceExt1( TRACE_NORMAL, DUP8_CSATNOTIFYSETUPMENU_MESSAGERECEIVED, "CSatNotifySetUpMenu::MessageReceived, newItem.iItemString: %S", newItem.iItemString );
                // Adding the new menuitem
                if( NULL != iconIdList.Size()  && ( i < iconIdList.Length() ) )
                    {
                    if( ( NULL != itemNextIndicator.Size() )
                        && ( i < itemNextIndicator.Length() ) )
                        {
                        //Menu item with item next idicator and icon identifier
                        if ( KErrNoMemory == menu.AddItem( newItem,
                             itemNextIndicator[i], iconIdList[i] ) )
                            {
                            // Too many or long menu items
                            ret = KErrCorrupt;
                            }
                        }
                    //menu item with icon identifier
                    else if ( KErrNoMemory == menu.AddItemIcon( newItem,
                                                                iconIdList[i] ) )
                        {
                        // Too many or long menu items
                        ret = KErrCorrupt;
                        }
                    else
                        {
                        // All ok
                        }
                    }
                else
                    {
                    if( ( NULL != itemNextIndicator.Size() )
                        && ( i < itemNextIndicator.Length() ) )
                        {
                        //menu item with item next indicator
                        if ( KErrNoMemory == menu.AddItem( newItem,
                                                           itemNextIndicator[i] ) )
                            {
                            // Too many or long menu items
                            ret = KErrCorrupt;
                            }
                        }
                    //menu item
                    else
                        {
                        TInt retAdd = menu.AddItem( newItem );
                        if ( KErrNoMemory == retAdd )
                            {
                            // Too many or long menu items
                            // // If there is not enough space left in the
                            // buffer used
                            // by the menu KErrNoMemory is returned.
                            ret = KErrCorrupt;
                            // send terminal response
                            iSatMessHandler->SetUpMenuTerminalResp(
                                    iTransId,
                                    iCommandDetails,
                                    RSat::KCmdDataNotUnderstood,
                                    RSat::KNoAdditionalInfo );
                            }
                        }
                    }
                itemNbr++;
                returnValue = berTlv.TlvByTagValue( &itemsData,
                                                        KTlvItemTag,
                                                        itemNbr );
                }
            }
        }
    if ( iReqHandle )
        {
        RSat::TSetUpMenuV2& setUpMenuV2 = ( *iSetUpMenuV2Pckg )();
        setUpMenuV2 = menu;
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SetUpMenu PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpMenu::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSETUPMENU_NOTIFY, "CSatNotifySetUpMenu::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifySetUpMenu::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    TInt ret( KErrNone );
    iReqHandle = aReqHandle;
    iSetUpMenuV2Pckg = static_cast< RSat::TSetUpMenuV2Pckg* >( aDataPtr );
    ret = iSatMessaging->SatReady( KSetUpMenu );
    if ( KErrNotFound == ret )
        {
        if ( iSatIsiMsg )
            {
            MessageReceived( TIsiReceiveC( iSatIsiMsg->Des() ) );
            delete iSatIsiMsg;
            iSatIsiMsg = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::TerminalResponse
// Handles a SetUpMenu terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpMenu::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPMENU_TERMINALRESPONSE, "CSatNotifySetUpMenu::TerminalResponse" );
    TFLOGSTRING("CSatNotifySetUpMenu::TerminalResponse");
    TInt ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TSetUpMenuRspV1Pckg* aRspPckg =
            reinterpret_cast<RSat::TSetUpMenuRspV1Pckg*>( aRsp );
    RSat::TSetUpMenuRspV1& rspV1 = ( *aRspPckg ) ();

    // Check that general result value is valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // If there is Me (Mobile Entity) error, additional info is needed
    if ( RSat::KMeProblem == rspV1.iInfoType )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo = static_cast<TUint8>( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }

    if( RSat::KSuccess == rspV1.iGeneralResult && iItemsNextIndicatorRemoved )
        {
        TFLOGSTRING("CSatNotifySetUpMenu::TerminalResponseL, iItemsNextIndicatorRemoved");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPMENU_TERMINALRESPONSE, "CSatNotifySetUpMenu::TerminalResponseL, iItemsNextIndicatorRemoved" );
        rspV1.iGeneralResult = RSat::KPartialComprehension;
        }
    iItemsNextIndicatorRemoved = EFalse;

    // Creating the info message
    iSatMessHandler->SetUpMenuTerminalResp(
        iTransId,                                   // Transaction id
        iCommandDetails,                            // Command number
        static_cast<TUint8>( rspV1.iGeneralResult ),  // Result
        additionalInfo );                           // additional info
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::SatApplicationName
// Return a reference to sat application name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDes16& CSatNotifySetUpMenu::SatApplicationName()
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPMENU_SATAPPLICATIONNAME, "CSatNotifySetUpMenu::SatApplicationName" );
    if ( 0x0 == iToolKitName.Length() )
        {
        // Default toolkit name
        iToolKitName.Copy( KSatToolkit );
        }
    return iToolKitName;
    }


// -----------------------------------------------------------------------------
// CSatNotifySelectItem::CSatNotifySelectItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySelectItem::CSatNotifySelectItem
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSelectItemV2Pckg( NULL ),
        iItemNextIndicatorRemoved( EFalse )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSELECTITEM_CSATNOTIFYSELECTITEM, "CSatNotifySelectItem::CSatNotifySelectItem" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KBackwardModeRequestedByUser
        + RSat::KNoResponseFromUser + RSat::KHelpRequestedByUser
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::~CSatNotifySelectItem
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySelectItem::~CSatNotifySelectItem()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSELECTITEM_CSATNOTIFYSELECTITEM, "CSatNotifySelectItem::~CSatNotifySelectItem" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::MessageReceived
// Handles a SelectItem proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySelectItem::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSELECTITEM_MESSAGERECEIVED, "CSatNotifySelectItem::MessageReceived" );
    TFLOGSTRING("CSatNotifySelectItem::MessageReceived");
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iCommandDetails.Copy( commandDetails.Data() );
    // Get command qualifier
    TUint8 cmdQualifier(
        commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        iSatMessHandler->SelectItemTerminalResp(
            iTransId,                       // Transaction id
            iCommandDetails,                // Command details TLV
            RSat::KMeUnableToProcessCmd,    // Result
            KNoCause  );                    // Additional info
        }
    else
        {
         // Fill the select item structure
         RSat::TSelectItemV2& selectItemV2 = ( *iSelectItemV2Pckg )();
        // Store command number
        selectItemV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        TPtrC8 sourceString; // Used in unicode conversions
        // Alpha id string (optional)
        selectItemV2.iAlphaId.iAlphaId.Zero();
        CTlv alphaIdentifier;
        TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier,
                                                KTlvAlphaIdentifierTag ) );
        // If alpha id string exist
        if ( KErrNone == returnValue )
            {
            TUint16 alphaIdLength(
                alphaIdentifier.GetLength() );
           if ( alphaIdLength )
                {
                // Get the alpha id
                sourceString.Set( alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );

                // Convert and set the alpha id
                TSatUtility::SetAlphaId( sourceString ,
                                         selectItemV2.iAlphaId.iAlphaId );
                }
            // Check alpha tag (id) status
           if ( alphaIdLength )
                {
                // Alpha tag present
                if ( selectItemV2.iAlphaId.iAlphaId.Length() )
                    {
                    selectItemV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    }
                else
                    {
                    selectItemV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
                    }
                }
           }
        // Alpha id not present
        else
            {
            selectItemV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            }
        // Help information
        if ( KHelpAvailabilityMask & cmdQualifier )
            {
            // Help information available
            selectItemV2.iHelp = RSat::EHelpAvailable;
            }
        else
            {
            // No help
            selectItemV2.iHelp = RSat::ENoHelpAvailable;
            }

        // Presentation type
        if ( KSelectItemPresentationTypeMask & cmdQualifier )
            {
            if ( KSelectItemChoiceOfNavigationMask & cmdQualifier )
                {
                // Presentation as a choice of navigation options
                selectItemV2.iPresentationType = RSat::ENavigationOptions;
                }
            else
                {
                // Presentation as a choice of data values
                selectItemV2.iPresentationType = RSat::EDataValues;
                }
            }
        else
            {
            TFLOGSTRING("TSY: CSatNotifySelectItem::MessageReceived, \
            Presentation type was not set.");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSELECTITEM_MESSAGERECEIVED, "CSatNotifySelectItem::MessageReceived, Presentation type was not set" );

            selectItemV2.iPresentationType = RSat::ENotSpecified;
            }
        // Selection preference
        if ( KSelectItemSelectUsingSoftKeyMask & cmdQualifier )
            {
            // Preference = soft key
            selectItemV2.iPreference = RSat::ESoftKeyPreferred;
            }
        else
            {
            // Preference not set
            selectItemV2.iPreference = RSat::ENoSelectionPreference;
            }
        // Default item id ( optional )
        CTlv defaultItem;
        returnValue = berTlv.TlvByTagValue( &defaultItem,
                                            KTlvItemIdentifierTag );
        if ( KErrNone == returnValue )
            {
            selectItemV2.iDefaultItemId =
                defaultItem.GetShortInfo( ETLV_IdentifierOfItemChosen );
            }
        // Icon qualifier list ( optional )
        // Each item of a list of items has an icon identifier coded on one
        // byte. The length of the Items icon identifier list shall be the
        // number of items of the list of items (X-1 shall be the number of
        // items in the list). The order of each item icon identifier,
        // shall reflect the order of the items in the list of items.
        // Each icon identifier addresses a record
        // in EFIMG as defined in TS 11.11
        CTlv itemsIconIdentifierList;
        TInt retValue( berTlv.TlvByTagValue( &itemsIconIdentifierList,
                                             KTlvItemIconIdentifierListTag ) );
        selectItemV2.iIconListQualifier = RSat::EIconQualifierNotSet;
        TPtrC8 iconIdList;
        if ( KErrNone == retValue )
            {
            TUint8 iconListQualifier( itemsIconIdentifierList.GetShortInfo(
                ETLV_IconListQualifier ) );
            // set iconIdList pointer
            iconIdList.Set(
                itemsIconIdentifierList.GetData( ETLV_IconIdentifierList ));
            // The icon list qualifier indicates to the ME how
            // the icons are to be used
            if( iconListQualifier )
                {
                // icon is not self-explanatory, i.e. if displayed,
                // it shall be displayed together with the item text
                selectItemV2.iIconListQualifier = RSat::ENotSelfExplanatory;
                }
            else
                {
                // icon is self-explanatory, i.e. if displayed,
                // it replaces the item text
                selectItemV2.iIconListQualifier = RSat::ESelfExplanatory;
                }
            }
        else if ( KErrNotFound == retValue )
            {
            selectItemV2.iIconListQualifier = RSat::ENoIconId;
            }
        else
            {
            TFLOGSTRING("TSY: CSatNotifySelectItem::MessageReceived, \
            Return value not valid.");
            OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYSELECTITEM_MESSAGERECEIVED, "CSatNotifySelectItem::MessageReceived, Return value not valid." );

            }

        CTlv item;
        // Get all items
        returnValue = berTlv.TlvByTagValueMulti( &item, KTlvItemTag );
        TPtrC8 itemData = item.Data();
        TUint8 numberOfItemData( 0 );
        TInt ind( 0 );
        TUint16 tlvLength( 0 );
        TInt currentTlv( 0 );
        TInt i( 0 );
        // Set numberOfItemData
        for ( i = 0; i < itemData.Length(); i += TInt( tlvLength ) )
            {
            // First determine if the length of the TLV is
            // coded with 1 or 2 bytes.
            if ( KTwoByteLengthCoding == itemData[ind+1] )
                {
                // Length is coded with 2 bytes -> real length is in second
                // byte first byte is 81 and it is "tag" for 2 byte length
                // coding.
                tlvLength =
                    TUint16( itemData[ind + 2] + KTlvHeaderLength + 1 );
                }
            else
                {
                //TLV header bytes (2) must be added to total length
                tlvLength = TUint16( itemData[ind + 1]  + KTlvHeaderLength );
                }

            currentTlv = itemData[ind]&KTagValueMask;

            if ( KTlvItemTag == currentTlv )
                {
                numberOfItemData++;
                }

            ind += tlvLength;
            }

        // See if itemnextindicator is included ( optional )
        CTlv nextIndicator;

        returnValue = berTlv.TlvByTagValue( &nextIndicator,
            KTlvItemsNextActionIndicatorTag );
        TPtrC8 itemNextIndicator;
        if ( KErrNone == returnValue )
            {
            TPtrC8 itemNextIndicatorTemp;
            itemNextIndicatorTemp.Set(
                nextIndicator.GetData( ETLV_ItemsNextActionIndicator ) );

            //In case the number of items in this list does not match the number
            //of items in the menu the Items Next Action Indicator list is ignored by ME
            if( itemNextIndicatorTemp.Length() != numberOfItemData )
                {
                iItemNextIndicatorRemoved = ETrue;
                }
            else
                {
                itemNextIndicator.Set(itemNextIndicatorTemp);
                }
            }

        // Filling up the menu items
        RSat::TItem newItem;
        // Set string length and pos
        TUint8 stringLength( 0 );
        TUint8  pos( 2 );
        // Transfers menu items to EtelSat data structure
        for ( i = 0; ( i < numberOfItemData ) && ( KErrNone == ret ); i++ )
            {
            // Check if two byte length coding is used
            if ( itemData[pos - 1] == KTwoByteLengthCoding )
                {
                // Set new offset
                pos++;
                }
            // Fill item if TLV contains real data, length < 0
            if ( KZero < itemData[pos - 1] )
                {
                // Fill the newitem
                newItem.iItemId = itemData[pos];
                stringLength = itemData[pos - 1];
                }
            else
                {
                // No real ítem data in TLV
                newItem.iItemId = 0;
                stringLength = 0;
                }

            // Copying the string from 8-bit to 16-bit
            if ( stringLength > RSat::KMenuItemMaxSize )
                {
                // String too long
                ret = KErrCorrupt;
                }
            newItem.iItemString.Zero();
            // ArabicCoding, GreekCoding and TurkishCoding have different
            // coding method. There is a tag for each type of alphabet
            // (resp. 80, 81 or 82) before the text, and there are base
            // pointers used for expanding 1 byte to 2 bytes as required
            // in UCS2
            // Ref: 3gpp 11.11, Annex B

            //Check if string present
            if ( 0 < ( stringLength - 1 ) )
                {
                TBuf8<RSat::KTextStringMaxSize> itemText;
                itemText = itemData.Mid( pos + 1, stringLength - 1 );

                if ( ( itemText[0] == KUCS2ArabicCoding )
                    || ( itemText[0] == KUCS2GreekCoding )
                    || ( itemText[0] == KUCS2TurkishCoding ) )
                    {
                    TSatUtility::ConvertAlphaFieldsToUnicode( itemText,
                    newItem.iItemString );
                    }
                else
                    {
                    // 8-bit format
                    TSatUtility::ConvertSms7ToUnicode16( newItem.iItemString,
                                                         itemText );
                    }
                }

            if( NULL != iconIdList.Size()
                && ( i < iconIdList.Length() ) )
                {
                if( ( NULL != itemNextIndicator.Size() )
                    && ( i < itemNextIndicator.Length() ) )
                    {
                    // ADD ITEM WITH ITEM NEXT INDICATOR AND ICON IDENTIFIER
                    if ( KErrNoMemory == selectItemV2.AddItem(
                        newItem, itemNextIndicator[i], iconIdList[i] ) )
                        {
                        // Too many or long menu items
                        ret = KErrCorrupt;
                        }
                    }
                // ADD ITEM WITH ICON
                else if ( KErrNoMemory == selectItemV2.AddItemIcon(
                    newItem, iconIdList[i] ) )
                    {
                    // Too many or long menu items
                    ret = KErrCorrupt;
                    }
                else
                    {
                    TFLOGSTRING("TSY: CSatNotifySelectItem::MessageReceived,\
                    Item with icon not valid.");
                    OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYSELECTITEM_MESSAGERECEIVED, "CSatNotifySelectItem::MessageReceived, Item with icon not valid." );
                    }
                }
            else
                {
                if( ( NULL != itemNextIndicator.Size() )
                    && ( i < itemNextIndicator.Length() ) )
                    {
                    // ADD ITEM WITH ITEM NEXT INDICATOR
                    if ( KErrNoMemory == selectItemV2.AddItem(
                        newItem, itemNextIndicator[i] ) )
                        {
                        // Too many or long menu items
                        ret = KErrCorrupt;
                        }
                    }
                //ADD ITEM
                else
                    {
                    TInt retAdd = selectItemV2.AddItem( newItem );
                    if ( KErrNoMemory == retAdd )
                        {
                        // Too many or long menu items
                        // If there is not enough space left in the buffer used
                        // by the menu KErrNoMemory is returned.
                        // Send terminal response
                        iSatMessHandler->SelectItemTerminalResp(
                            iTransId,                        // transaction id
                            iCommandDetails,                // Command details TLV
                            RSat::KCmdDataNotUnderstood,    // Result
                            KNoCause );                     // Additional info
                        ret = KErrCorrupt;
                        }
                    }
                }
            // Calculating the new position
            pos = TUint8( pos + itemData[pos - 1] + 2 );
            }
        // Iconid
        TSatUtility::FillIconStructure( berTlv, selectItemV2.iIconId );
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SelectItem PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySelectItem::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSELECTITEM_NOTIFY, "CSatNotifySelectItem::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifySelectItem::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iSelectItemV2Pckg = static_cast< RSat::TSelectItemV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSelectItem );
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::TerminalResponse
// Handles a SelectItem terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySelectItem::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSELECTITEM_TERMINALRESPONSE, "CSatNotifySelectItem::TerminalResponse" );
    TFLOGSTRING("CSatNotifySelectItem::TerminalResponse");
    TInt   ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TSelectItemRspV1Pckg* aRspPckg =
            reinterpret_cast<RSat::TSelectItemRspV1Pckg*>( aRsp );
    RSat::TSelectItemRspV1& rspV1 = ( *aRspPckg ) ();

    // Check that general result values are valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Entity) error or there there should be
    // item identifier or
    // general result is success, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType )
        || ( RSat::KItemIdentifier == rspV1.iInfoType )
        || ( RSat::KSuccess == rspV1.iGeneralResult ) )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo = static_cast<TUint8>( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }

    if( RSat::KSuccess == rspV1.iGeneralResult && iItemNextIndicatorRemoved )
        {
        TFLOGSTRING("CSatNotifySelectItem::TerminalResponseL, iItemNextIndicatorRemoved");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSELECTITEM_TERMINALRESPONSE, "CSatNotifySelectItem::TerminalResponse, iItemNextIndicatorRemoved" );

        rspV1.iGeneralResult = RSat::KPartialComprehension;
        }
    iItemNextIndicatorRemoved = EFalse;

    iSatMessHandler->SelectItemTerminalResp(
                                iTransId,                    // Transaction id
                                iCommandDetails,            // Command details
                                static_cast< TUint8 >(
                                    rspV1.iGeneralResult ), // General result
                                additionalInfo );           // Additional info
    return ret;

    }


// -----------------------------------------------------------------------------
// CSatNotifySendSm::CSatNotifySendSm
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySendSm::CSatNotifySendSm
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSendSmV1Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSM_CSATNOTIFYSENDSM, "CSatNotifySendSm::CSatNotifySendSm" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KNetworkUnableToProcessCmd + RSat::KSmsRpError
        + RSat::KErrorRequiredValuesMissing
        + RSat::KInteractionWithCCPermanentError;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::~CSatNotifySendSm
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySendSm::~CSatNotifySendSm()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDSM_CSATNOTIFYSENDSM, "CSatNotifySendSm::~CSatNotifySendSm" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::MessageReceived
// Handles a SendSm proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendSm::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSM_MESSAGERECEIVED, "CSatNotifySendSm::MessageReceived" );
    TFLOGSTRING("CSatNotifySendSm::MessageReceived");
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        iSatMessHandler->SendSmTerminalResp(
            iTransId,                       // Transaction id
            iCommandDetails,                // Command details tlv
            RSat::KMeUnableToProcessCmd,    // Result
            KNoCause );                     // Additional info
        }
    else
        {
        // Fill the send sm structure
        RSat::TSendSmV1& sendSmV1 = ( *iSendSmV1Pckg )();
        // Store command number
        sendSmV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        sendSmV1.iSmsTpdu.Zero();
        CTlv smsTpduTlv;
        // Get TPdu tlv
        TInt returnValue( berTlv.TlvByTagValue( &smsTpduTlv,
                                                KTlvSmsTpduTag ) );
        if ( KErrNone == returnValue )
            {
            // Tpdu TLV length
            TUint16 smsTpduLength( smsTpduTlv.GetLength() );
            // Set pointer to Tpdu
            TPtrC8 smsTpdu = smsTpduTlv.GetData( ETLV_SmsTPdu );
            // Check that TP-UDL is valid
            ret = CheckTpdu( smsTpdu );
            if ( KErrCorrupt == ret )
                {
                // Length of TPDU is invalid
                iSatMessHandler->SendSmTerminalResp(
                         iTransId,                      // Transaction id
                         iCommandDetails,               // Command details tlv
                         RSat::KCmdDataNotUnderstood,   // Result
                         KNoCause );                    // Additional info
                }
            if ( smsTpduLength && KErrNone == ret )
                {
                // Copying the smsTpdu
                sendSmV1.iSmsTpdu.Append( smsTpdu );
                }
            // Checking if packing is required for the SMS message or not
            // Packing: see ETSI 3.38 and 3.40
            if ( KSmsPackingRequiredMask & iCommandDetails[KCommandQualifier]
                && ( KErrNone == ret ) )
                {
                // Packing required
                if ( smsTpduLength )
                    {
                    // Call method to pack sms
                    ret = PackSms( smsTpdu, sendSmV1.iSmsTpdu );
                    if ( KErrNone != ret )
                        {
                        // tpdu is invalid or packing cannot be requested
                        // if tpdu is something else than SMS-SUBMIT
                        iSatMessHandler->SendSmTerminalResp(
                            iTransId,                   // Transaction id
                            iCommandDetails,            // Command details tlv
                            RSat::KCmdDataNotUnderstood,// Result
                            KNoCause );                 // Additional info
                        ret = KErrCorrupt;
                        }
                    }
                }
            }
        else
            {
            // TPdu is missing, returning response immediately
            iSatMessHandler->SendSmTerminalResp(
                         iTransId,                      // Transaction id
                         iCommandDetails,               // Command details tlv
                         RSat::KErrorRequiredValuesMissing,// Result
                         KNoCause );                    // Additional info
            ret = KErrCorrupt;
            }
        if ( KErrNone == ret )
            {
            TPtrC8 sourceString; // Used in unicode conversions
            // Alpha id string (optional)
            sendSmV1.iAlphaId.iAlphaId.Zero();
            CTlv alphaIdTlv;
            returnValue = berTlv.TlvByTagValue( &alphaIdTlv,
                                                KTlvAlphaIdentifierTag ) ;
            if ( KErrNone == returnValue )
                {
                // alpha id tlv found
                if ( alphaIdTlv.GetLength() )
                    {
                    // get alpha id text
                    sourceString.Set(
                        alphaIdTlv.GetData( ETLV_AlphaIdentifier ) );

                    // convert and set alpha id
                    TSatUtility::SetAlphaId( sourceString,
                                             sendSmV1.iAlphaId.iAlphaId );
                    }
                // Alpha Tag present
                if ( sendSmV1.iAlphaId.iAlphaId.Length() )
                    {
                    sendSmV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    }
                else
                    {
                    sendSmV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
                    }
                }
            else
                {
                sendSmV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
                }
            // The address data object holds the RP_Destination_Address of the
            // Service Centre. If no RP_Destination_Address is transferred,
            // then the ME shall insert the default Service Centre address.
            // Service centre address
            sendSmV1.iAddress.iTelNumber.Zero();
            CTlv addressTlv;
            // Get address tlv. Includes SCA number. (optional)
            returnValue = berTlv.TlvByTagValue( &addressTlv, KTlvAddressTag );
            if ( KErrNone == returnValue )
                {
                if ( 0 < addressTlv.GetLength() )
                    {
                    // Initialize ton and npi
                    RSat::TTypeOfNumber ton;
                    RSat::TNumberingPlan npi;
                    // Call utility function that maps received TON and NPI to
                    // RSat values
                    TSatUtility::GetTonAndNpi(
                        addressTlv.GetShortInfo( ETLV_TonAndNpi ), &ton, &npi );
                    // Set TON and NPI
                    sendSmV1.iAddress.iTypeOfNumber = ton;
                    sendSmV1.iAddress.iNumberPlan = npi;

                    TBuf8<RSat::KMaxMobileTelNumberSize> tempScaNumber;
                    // set pointer to address tlv
                    sourceString.Set(
                        addressTlv.GetData( ETLV_DiallingNumberString ) );
                    if ( sourceString.Length() )
                        {
                        // Semi-octet presentation used
                        // Converting back to ASCII format
                        TSatUtility::BCDToAscii( sourceString, tempScaNumber );

                        sendSmV1.iAddress.iTelNumber.Copy( tempScaNumber );

                        TFLOGSTRING2("TSY:SendSm, SCA number: %S",
                        &sendSmV1.iAddress.iTelNumber );
                        OstTraceExt1( TRACE_NORMAL, DUP1_CSATNOTIFYSENDSM_MESSAGERECEIVED, "CSatNotifySendSm::MessageReceived SCA number: %S", sendSmV1.iAddress.iTelNumber );
                        }
                    else
                        {
                        // Address TLV found, TON/NPI present, but number epmpty.
                        sendSmV1.iAddress.iTypeOfNumber = RSat::EUnknownNumber;
                        sendSmV1.iAddress.iNumberPlan = RSat::EUnknownNumberingPlan;
                        }
                    }
                else
                    {
                    // Address TLV found, but the Value part doesn't exist
                    sendSmV1.iAddress.iTypeOfNumber = RSat::EUnknownNumber;
                    sendSmV1.iAddress.iNumberPlan = RSat::EUnknownNumberingPlan;
                    }
                }
            else
                {
                // Addrress TLV not found
                sendSmV1.iAddress.iTypeOfNumber = RSat::ETypeOfNumberNotSet;
                sendSmV1.iAddress.iNumberPlan = RSat::ENumberingPlanNotSet;
                }
            // Iconid
            TSatUtility::FillIconStructure( berTlv, sendSmV1.iIconId );
            }
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SendSm PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendSm::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSENDSM_NOTIFY, "CSatNotifySendSm::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifySendSm::Notify. \n\t\t\t Handle:%d\n\t\t\t",
                aReqHandle );
    iReqHandle = aReqHandle;
    iSendSmV1Pckg = static_cast< RSat::TSendSmV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSendShortMessage );
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::TerminalResponse
// Handles a SendSm terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSM_TERMINALRESPONSE, "CSatNotifySendSm::TerminalResponse" );
    TFLOGSTRING("CSatNotifySendSm::TerminalResponse");
    TInt ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TSendSmRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TSendSmRspV1Pckg* >( aRsp );
    RSat::TSendSmRspV1& rspV1 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // If there is Me (Mobile Entity) error or network error, additional info
    // is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType )
        || ( RSat::KSatNetworkErrorInfo == rspV1.iInfoType )
        || ( RSat::KControlInteraction == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( 0 == rspV1.iAdditionalInfo.Length() )
            {
            // No info
            ret = KErrCorrupt;
            }
        else
            {
            additionalInfo = static_cast< TUint8 >( rspV1.iAdditionalInfo[0] );
            }
        }
    // Creating the info message
    iSatMessHandler->SendSmTerminalResp(
                        iTransId,                   // Transaction id
                        iCommandDetails,            // Command details tlv
                        static_cast< TUint8 >(
                            rspV1.iGeneralResult ), // Result
                        additionalInfo );           // Additional info
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::GetTpUdlIndex
// Returns respectively index of TP-UDL or TP-CDL field if sms type is
// respectively SMS-SUBMIT or SMS-COMMAND
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CSatNotifySendSm::GetTpUdlIndex
    (
    TPtrC8 aTpdu // Tpdu where to read TP-UDL or TP-CDL
    )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSM_GETTPUDLINDEX, "CSatNotifySendSm::GetTpUdlIndex" );
    TFLOGSTRING("CSatNotifySendSm::GetTpUdlIndex");

    // Position on TP-UDL/TP-CDL field in SMS TPDU
    TUint8 tpDlIndex( 0 );

    // SMS-SUBMIT
    if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) )
        {
        // Add mandatory fields at first: TP-MTI, TP-MR, TP-DA(address length,
        // TON&NPI), TP-PID, DCS and TP-UDL.
        // Index starts from [0]
        tpDlIndex = KTPDUMandatoryFieldsSmsSubmitUdl;

        // Checking the TP-VP field existence and format
        // bit4    bit3
        // 0       0   TP-VP field not present
        // 1       0   TP-VP field present - relative format
        // 0       1   TP-VP field present - enhanced format
        // 1       1   TP-VP field present - absolute format

        // Shift by 3 and mask for getting bits 3 and 4
        TUint8 vpFormat( ( aTpdu[0] >> 3 ) & KSmsVpFormatMask );

        if ( KSmsVPFRelative == vpFormat )
            {
            // TP-Validity period comprises 1 octet
            tpDlIndex++;
            }
        else if ( KSmsVPFEnhanced == vpFormat || KSmsVPFAbsolute == vpFormat )
            {
            // TP-Validity period comprises 7 octets
            tpDlIndex += 7;
            }

        // Calculate the address length in bytes, located in index 2
        TUint8 addressLengthInBytes( aTpdu[2] );
        // Address length is number of BCD characters.
        // Two BCD string chars are coded in one byte =>
        // divide by two (shitf to right). Before dividing, 1 is added
        // for ensuring that result is rounded up in case of odd value
        addressLengthInBytes++;
        addressLengthInBytes >>= 1;

        // Add address length
        tpDlIndex += addressLengthInBytes;
        }
    // SMS-COMMAND
    else
        {
        // Add mandatory fields at first: TP-MTI, TP-MR, TP-PID, TP-CT, TP-MN,
        // TP-DA(address length, TON&NPI) and TP-CDL.
        // Index starts from [0]
        tpDlIndex = KTPDUMandatoryFieldsSmsCommandUdl;
        // Address length, located in index 5
        TUint8 addressLengthInBytes( aTpdu[5] );
        // Address length is number of BCD characters
        // Two BCD string chars are coded in one byte =>
        // divide by two (shitf to right). Before dividing, 1 is added
        // for ensuring that result is rounded up in case of odd value
        addressLengthInBytes++;
        addressLengthInBytes >>= 1;

        // Add address length
        tpDlIndex += addressLengthInBytes;
        }

    return tpDlIndex; // Return TP-UDL/TP-CDL index
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::GetTpDcsIndex
// Returns index of TP-DCS (data coding scheme) of the sms.
// SMS type is SMS-SUBMIT.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CSatNotifySendSm::GetTpDcsIndex
        (
        TPtrC8 aTpdu // Tpdu
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSM_GETTPDCSINDEX, "CSatNotifySendSm::GetTpDcsIndex" );
    TFLOGSTRING("CSatNotifySendSm::GetTpDcsIndex");

    // Position of TP-DCS field in SMS TPDU
    // Add mandatory fields at first: TP-MTI, TP-MR, TP-DA(address length,
    // TON/NPI), TP-PID and TP-DCS.
    // Index starts from [0]
    TUint8 tpDcsIndex( KTPDUMandatoryFieldsSmsSubmitDcs );
    // Calculate the length of address, located in index 2
    TUint8 addressLengthInBytes( aTpdu[2] );
    // Address lenght is number of BCD characters.
    // Add one for ensuring the rounding up after division in case of odd
    // number
    addressLengthInBytes++;
    // Divide by two for getting length in bytes
    addressLengthInBytes >>= 1;
    // Add address length to dcs position
    tpDcsIndex += addressLengthInBytes;

    return tpDcsIndex; // Return index of TP-DCS
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::CheckTpdu
// Check TPDU validity
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::CheckTpdu
        (
        TPtrC8 aTpdu // TPDU
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSM_CHECKTPDU, "CSatNotifySendSm::CheckTpdu" );
    TFLOGSTRING("CSatNotifySendSm::CheckTpdu");
    TInt ret( KErrNone );
    TUint8 tpUdlPos( 0 ); // UDL position
    TUint8 tpUdl( 0 ); // User data length
    TUint8 bufferLength( aTpdu.Length() ); // TPDU buffer length

    // At first check that TPDU length isn't under minimum
    if ( KMinSmsTpduLength > bufferLength )
        {
        TFLOGSTRING("CSatNotifySendSm::CheckTpdu, error: invalid TPDU length");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDSM_CHECKTPDU, "CSatNotifySendSm::CheckTpdu, error: invalid TPDU length" );
        ret = KErrCorrupt;
        }
    else
        {
        // Get index of UDL field
        tpUdlPos = GetTpUdlIndex( aTpdu );
        // Check that UDL index is valid
        if ( tpUdlPos < bufferLength )
            {
            // Get user data length in 8-bit characters (without packing)
            tpUdl = aTpdu[tpUdlPos];
            }
        else
            {
            TFLOGSTRING("CSatNotifySendSm::CheckTpdu, error: invalid UDL index");
            OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYSENDSM_CHECKTPDU, "CSatNotifySendSm::CheckTpdu, error: invalid UDL index" );
            ret = KErrCorrupt;
            }
        }

    if ( KErrNone == ret )
        {
        // SMS-SUBMIT
        if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) )
            {
            // Only user data bytes are remained
            bufferLength -= ( tpUdlPos + 1 );
            // Get index of TP-DCS field
            TUint8 tpDcsPos( GetTpDcsIndex( aTpdu ) );
            // Get data coding scheme
            TUint8 dcs( aTpdu[tpDcsPos] );

            // Bits 2 and 3 are for coding scheme ( spec. 3GPP TS23.038 )
            dcs = ( dcs >> 2 ) & 0x03;
            // GSM 7 bit default alphabet
            if ( KSmsDcsDefaultAlphabet == dcs )
                {
                // When 7 bit packing is used, number of characters is calculated
                // as follows (spec. 3GPP TS 23.038 V7.0.0 ):
                // number of characters = length of user data in bytes * 8 / 7
                bufferLength = ( bufferLength * 8 ) / 7;
                // Check the validity of user data length
                if ( KSmsMaxSize < tpUdl || bufferLength < tpUdl )
                    {
                    // Message is over 160 bytes or invalid user data length
                    TFLOGSTRING("CSatNotifySendSm::CheckTpdu, error: invalid user data length");
                    OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYSENDSM_CHECKTPDU, "CSatNotifySendSm::CheckTpdu, error: invalid user data length" );
                    ret = KErrCorrupt;
                    }
                }
            else // 8-bit or 16-bit data
                {
                // Check if packing is required
                TBool isPackingRequired( KSmsPackingRequiredMask &
                    iCommandDetails[KCommandQualifier] );
                TUint8 maxSmsSize( isPackingRequired ?
                    KSmsMaxSize : KSmsMaxSizeWithoutPacking );
                // Check the validity of user data length
                if ( maxSmsSize < tpUdl || bufferLength < tpUdl )
                    {
                    // Message size is over the limit or invalid user data length
                    TFLOGSTRING("CSatNotifySendSm::CheckTpdu, error: invalid user data length");
                    OstTrace0( TRACE_NORMAL, DUP4_CSATNOTIFYSENDSM_CHECKTPDU, "CSatNotifySendSm::CheckTpdu, error: invalid user data length" );
                    ret = KErrCorrupt;
                    }
                }
            } // end of if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) )
        else // SMS-COMMAND
            {
            if ( KSMSCommandMaxSize < tpUdl )
                {
                TFLOGSTRING("CSatNotifySendSm::CheckTpdu, error: user data length over maximum");
                OstTrace0( TRACE_NORMAL, DUP5_CSATNOTIFYSENDSM_CHECKTPDU, "CSatNotifySendSm::CheckTpdu, error: user data length over maximum" );
                ret = KErrCorrupt;
                }
            }
        } // End of if ( KErrNone == ret )
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::PackSms
// Pack SMS content if it is required by the sim
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::PackSms
        (
        TPtrC8 aTpdu, // Received Tpdu
        TTpdu& aSendSm // Where to store packed sms
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSM_PACKSMS, "CSatNotifySendSm::PackSms" );
    TFLOGSTRING("CSatNotifySendSm::PackSms");
    TInt ret( KErrNone );

    // Checking the message type, SMS-SUBMIT can be packed:
    //
    // Bit1 | Bit0 | Message type
    // ------------------------------------------------------------
    //   0  |   0  | SMS-DELIVER (in the direction SC to MS)
    //   0  |   0  | SMS-DELIVER REPORT (in the direction MS to SC)
    //   1  |   0  | SMS-STATUS-REPORT (in the direction SC to MS)
    //   1  |   0  | SMS-COMMAND (in the direction MS to SC)
    //   0  |   1  | SMS-SUBMIT (in the direction MS to SC)
    //   0  |   1  | SMS-SUBMIT-REPORT (in the direction SC to MS)
    //   1  |   1  | Reserved
    //
    if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) )
        {
        // Message is a SMS-SUBMIT message.
        //
        // Get index of TP-UDL field and it is also starting position.
        TUint8 startPosition( GetTpUdlIndex( aTpdu ) );
        // Get number of characters what will be packed.
        TUint8 charsToPack( aTpdu[startPosition] );
        // Calculate end position.
        TUint8 endPosition( startPosition + charsToPack );
        // Get SMS TPDU length
        TUint8 tpduLength( aTpdu.Length() );

        // Initialize values.
        TInt i( 0 );
        TInt x( startPosition + 1 );
        TBool endOfTPDU( EFalse );
        TUint8 move( 0 );

        // Lets pack all the characters in SMS Message (TP-UD), including header
        // (TP-UD-Header) if exist.
        //
        // Go through the TP-UD.
        for ( i = startPosition; i < endPosition; i++ )
            {
            // Check that index is not over SMS TPDU length.
            if ( ( i + 1 ) < tpduLength )
                {
                TUint8 char1( TUint8( aTpdu[i + 1] >> move ) );

                TUint8 char2( 0 );

                if ( ( i + 2 ) <= endPosition )
                    {
                    // Check that index is not over SMS TPDU length.
                    if ( ( i + 2 ) < tpduLength )
                        {
                        char2 = TUint8( aTpdu[i + 2] << ( 7 - move ) );
                        }
                    else
                        {
                        ret = KErrCorrupt;
                        break;
                        }
                    }
                else
                    {
                    // No more characters to pack.
                    endOfTPDU = ETrue;
                    char2 = 0;
                    }

                // Append packed characters.
                aSendSm[x++] = TUint8( char1 | char2 );

                // New values for next round.
                if ( ( 6 == move ) && !endOfTPDU )
                    {
                    i++;
                    move = 0;
                    }
                else
                    {
                    move++;
                    }
                }
            else
                {
                ret = KErrCorrupt;
                break;
                }
            }

        // TP-UD is now packed
        if ( KErrCorrupt != ret )
            {
            // New string length (number of characters)
            //
            // Calculate if string length is changed from orginal length.
            // These lengths are aTpdu[] buffer length so it start from "0"
            // > "i" includes old value
            // > "x" is new value with packed TP-UD
            // > "x - 1" because we needed to add one for start point earlier
            //           to append characters to correct position.
            TUint8 stringLengthChanged( i - ( x - 1 ) );

            if ( stringLengthChanged )
                {
                aSendSm.SetLength( aSendSm.Length() - stringLengthChanged );
                }

            // Change the DCS to default alphabet coding. See ETSI 3.38
            //
            // Get the position of TP-DCS
            TUint8 posDCS( GetTpDcsIndex( aTpdu ) );

            // Coding Group Bits are 7..4 and Use of bits are 3..0
            // We need to find out correct Coding Group.
            //
            // KMaskC0: 0xC0 = 1100 0000
            // > This check include Coding Group:
            // >> Bits:  7..4
            // >> Group: 00xx
            //
            if ( KMask0 == ( aTpdu[posDCS] & KMaskC0 ) )
                {
                // Set bits 2 and 3 to 0.
                // Bits 3 and 2 indicate the alphabet being used, as follows:
                //
                // Bit3 | Bit2 | Alphabet:
                // ----------------------------------------
                //   0  |   0  | GSM 7 bit default alphabet
                //   0  |   1  | 8 bit data
                //   1  |   0  | UCS2 (16bit)
                //   1  |   1  | Reserved
                //
                // KMaskF3: 0xF3 = 1111 0011
                aSendSm[posDCS] = TUint8( aSendSm[posDCS] & KMaskF3 );
                }

            // KMaskF0: 0xF0 = 1111 0000
            // > This check include Coding Group:
            // >> Bits:  7..4
            // >> Group: 1111
            //
            if ( KMaskF0 == ( aTpdu[posDCS] & KMaskF0 ) )
                {
                // Set bit 2 to 0.
                // Bit 3 is reserved, set to 0.
                //
                // Bit 2 indicate the alphabet being used, as follows:
                //
                // Bit2 | Message coding:
                // ---------------------------------
                //   0  | GSM 7 bit default alphabet
                //   1  | 8-bit data
                //
                // KMaskFB: 0xFB = 1111 1011
                aSendSm[posDCS] = TUint8( aSendSm[posDCS] & KMaskFB );
                }
            }
        }
    else
        {
        // Packing cannot be requested if TPDU is something else
        // than SMS submit.
        ret = KErrCorrupt;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::CSatNotifySendDtmf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySendDtmf::CSatNotifySendDtmf
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSendDtmfV1Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDDTMF_CSATNOTIFYSENDDTMF, "CSatNotifySendDtmf::CSatNotifySendDtmf" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::~CSatNotifySendDtmf
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySendDtmf::~CSatNotifySendDtmf()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDDTMF_CSATNOTIFYSENDDTMF, "CSatNotifySendDtmf::~CSatNotifySendDtmf" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::MessageReceived
// Handles a SendDtmf proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendDtmf::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDDTMF_MESSAGERECEIVED, "CSatNotifySendDtmf::MessageReceived" );
    TFLOGSTRING("CSatNotifySendDtmf::MessageReceived");
    // get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        iSatMessHandler->SendDtmfTerminalResp( iTransId,
                                                iCommandDetails,
                                                RSat::KMeUnableToProcessCmd,
                                                KNoCause );
        }
    else
        {
        TInt returnValue( KErrNone );
        TInt ret( KErrNone );
        // Fill the send ss structure
        RSat::TSendDtmfV1& sendDtmfV1 = ( *iSendDtmfV1Pckg )();
        // command number
        sendDtmfV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
         // Alpha id string (optional)
        sendDtmfV1.iAlphaId.iAlphaId.Zero();
        CTlv alphaIdentifier;
        returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
            KTlvAlphaIdentifierTag );
        // if alpha id string exist
        if( KErrNone == returnValue )
            {
            TUint16 alphaIdLength = alphaIdentifier.GetLength();

            if ( alphaIdLength > RSat::KAlphaIdMaxSize )
                {
                // String too long
                iSatMessHandler->SendDtmfTerminalResp( iTransId,
                                    iCommandDetails,
                                    RSat::KCmdBeyondMeCapabilities,
                                    KNoCause );
                ret = KErrCorrupt;
                }
            else if ( alphaIdLength )
                {
                TPtrC8 temp; // Used with Copy8to16
                 // get the alpha id
                temp.Set( alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
                // convert and set the alpha id
                TSatUtility::SetAlphaId( temp ,
                    sendDtmfV1.iAlphaId.iAlphaId );

                sendDtmfV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                }
            else
                {
                 sendDtmfV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
                }
            }
        // Alpha id not present
        else
            {
            sendDtmfV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            }

        // Dtmf string length, 8-bit string, mandatory
        CTlv sendDtmf;
        returnValue = berTlv.TlvByTagValue( &sendDtmf,
            KTlvDtmfStringTag );
        if ( KErrNone == returnValue )
            {
            TUint8 generalResult( RSat::KSuccess );
            // length - 1 for string
            TUint16 dtmfStringLength = TUint16( sendDtmf.GetLength() );

            //If first byte of the dtmf string is 0xFF it means that dtmf string
            //is empty and we have to return general result
            if ( ( dtmfStringLength && ( sendDtmf.Data()[2] == 0xFF ) ) ||
                !dtmfStringLength )
                {
                generalResult = RSat::KCmdDataNotUnderstood;
                ret = KErrCorrupt;
                }
            else if( RSat::KDtmfStringMaxSize < dtmfStringLength )
                {
                generalResult = RSat::KCmdBeyondMeCapabilities;
                ret = KErrCorrupt;
                }
            else
                {
                sendDtmfV1.iDtmfString.Append( sendDtmf.GetData( ETLV_DtmfString ) );
                }

            if( KErrCorrupt == ret )
                {
                // String too long
                iSatMessHandler->SendDtmfTerminalResp( iTransId,
                                    iCommandDetails,
                                    generalResult,
                                    KNoCause );
                }
            }
        else
            {
            iSatMessHandler->SendDtmfTerminalResp( iTransId,
                                    iCommandDetails,
                                    RSat::KErrorRequiredValuesMissing,
                                    KNoCause );
            ret = KErrCorrupt;
            }

        // Iconid
        if ( KErrNone == ret )
            {
            TSatUtility::FillIconStructure( berTlv,
                sendDtmfV1.iIconId );
            }
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SendDtmf PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendDtmf::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSENDDTMF_NOTIFY, "CSatNotifySendDtmf::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifySendDtmf::Notify. \n\t\t\t Handle:%d\n\t\t\t",
                aReqHandle );
    iReqHandle = aReqHandle;
    iSendDtmfV1Pckg = static_cast< RSat::TSendDtmfV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSendDtmf );
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::TerminalResponse
// Handles a SendDtmf terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendDtmf::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDDTMF_TERMINALRESPONSE, "CSatNotifySendDtmf::TerminalResponse" );
    TFLOGSTRING("CSatNotifySendDtmf::TerminalResponse");
    TInt ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TSendDtmfRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TSendDtmfRspV1Pckg* >( aRsp );
    RSat::TSendDtmfRspV1& rspV1 = ( *aRspPckg ) ();

    // Check that general result values are valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    if( RSat::KMeProblem == rspV1.iInfoType )
        {
        if ( rspV1.iAdditionalInfo.Length() )
            {
            additionalInfo = TUint8( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    else
        {
        // Invalid additional info field
        ret = KErrCorrupt;
        }

    // Creating the info message
    ret = iSatMessHandler->SendDtmfTerminalResp( iTransId, iCommandDetails,
        TUint8( rspV1.iGeneralResult ), additionalInfo );
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifySendSs::CSatNotifySendSs
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySendSs::CSatNotifySendSs
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSendSsV1Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSS_CSATNOTIFYSENDSS, "CSatNotifySendSs::CSatNotifySendSs" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KModifiedByCallControl + RSat::KUssdTransactionTerminatedByUser
        + RSat::KNetworkUnableToProcessCmd
        + RSat::KInteractionWithCCTemporaryError + RSat::KSsReturnError
        + RSat::KErrorRequiredValuesMissing
        + RSat::KInteractionWithCCPermanentError;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::~CSatNotifySendSs
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySendSs::~CSatNotifySendSs()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDSS_CSATNOTIFYSENDSS, "CSatNotifySendSs::~CSatNotifySendSs" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::MessageReceived
// Handles a DisplayText proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendSs::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSS_MESSAGERECEIVED, "CSatNotifySendSs::MessageReceived" );
    TFLOGSTRING("CSatNotifySendSs::MessageReceived");
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        TBuf8<1> noCause;
        iSatMessHandler->SendSsTerminalResp( iTransId,
                                              iCommandDetails,
                                              RSat::KMeUnableToProcessCmd,
                                              noCause );
        }
    else
        {
        // Fill the send ss structure
        RSat::TSendSsV1& sendSsV1 = ( *iSendSsV1Pckg )();
        // Store command number
        sendSsV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        TPtrC8 sourceString; // Used with conversions
         // Alpha id string (optional)
        sendSsV1.iAlphaId.iAlphaId.Zero();
        CTlv alphaIdentifier;
        // Get alpha id tlv from berTlv
        TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier,
                                                KTlvAlphaIdentifierTag ) );
        // if alpha id string exist
        if ( KErrNone == returnValue )
            {
            if ( alphaIdentifier.GetLength() )
                {
                // 8-bit string to 16-bit string
                sourceString.Set(
                    alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );

                // Convert and set the alpha id
                TSatUtility::SetAlphaId(
                    sourceString ,sendSsV1.iAlphaId.iAlphaId );
                // Set alpha id status
                sendSsV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                }
            else
                {
                sendSsV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
                }
            }
        // Alpha id not present
        else
            {
            sendSsV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            }
        // SS string Tlv
        sendSsV1.iSsString.iSsString.Zero();
        CTlv ssString;
        // Get SS string tlv
        returnValue = berTlv.TlvByTagValue( &ssString,
                                            KTlvSsStringTag );
        if ( KErrNone == returnValue )
            {
            // Initialize ton and npi
            RSat::TTypeOfNumber ton;
            RSat::TNumberingPlan npi;

            iSatMessaging->SetTonNpi ( ssString.GetShortInfo( ETLV_TonAndNpi ) );

            // Call utility function that maps received TON and NPI
            // to RSat values
            TSatUtility::GetTonAndNpi(
                ssString.GetShortInfo( ETLV_TonAndNpi ), &ton, &npi );
            // Set TON and NPI
            sendSsV1.iSsString.iTypeOfNumber = ton;
            sendSsV1.iSsString.iNumberPlan = npi;
            // Check length of SS string.
            // BCD format = two digits "packed" in one characher
            if ( ssString.GetLength() * 2 <= RSat::KStringMaxSize )
                {
                TBuf8<RSat::KStringMaxSize> tempString;
                // Set pointer to the BCD SS string
                sourceString.Set( ssString.GetData( ETLV_SsOrUssdString ) );
                // Semi-octet presentation used
                // Converting back to ASCII format
                TSatUtility::BCDToAscii( sourceString, tempString );
                // Set pointer to the ASCII string.
                sourceString.Set( tempString );
                sendSsV1.iSsString.iSsString.Append( tempString );
                }
            else
                {
                // SS string is too long
                ret = KErrCorrupt;
                }
            if ( RSat::EInternationalNumber ==
                sendSsV1.iSsString.iTypeOfNumber )
                {
                // Append '+' character if the string is for call forwarding
                CheckCallForwarding( sourceString, sendSsV1.iSsString );
                }
            }
        else
            {
            // Mandatory tlv is missing, returning response immediately
            TBuf8<1> noCause;
            iSatMessHandler->SendSsTerminalResp(
                iTransId,                           // Transaction id
                iCommandDetails,                    // Command details TLV
                RSat::KErrorRequiredValuesMissing,  // Result
                noCause );                          // Additional info
            ret = KErrCorrupt;
            CompleteRequest( ret );
            return;
            }
        if ( KErrCorrupt == CheckSsStringValidity( sourceString ) ||
            KErrCorrupt == ret )
            {
            TBuf8<1> noCause;
            iSatMessHandler->SendSsTerminalResp(
                iTransId,                           // Transaction id
                iCommandDetails,                    // Command details TLV
                RSat::KCmdDataNotUnderstood,        // Result
                noCause );                          // Additional info
            ret = KErrCorrupt;
            }

        // Icon (optional)
        TSatUtility::FillIconStructure( berTlv, sendSsV1.iIconId );
        CompleteRequest( ret );
#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
        // Notify NokiaTSY that next SS request is SAT originated
        if ( KErrNone == ret )
            {
            TFLOGSTRING("TSY: Inform NokiaTSY SS request being SAT originated");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDSS_MESSAGERECEIVED, "Inform NokiaTSY SS request being SAT originated" );
            iSatMessaging->GetMessageRouter()->ExtFuncL(
                ESatNotifyCallControlRequest,
                NULL );
            }
#endif
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SendSs PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendSs::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSENDSS_NOTIFY, "CSatNotifySendSs::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifySendSs::Notify. \n\t\t\t Handle:%d\n\t\t\t",
                aReqHandle );
    iReqHandle = aReqHandle;
    iSendSsV1Pckg = static_cast< RSat::TSendSsV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSendSs );
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::TerminalResponse
// Handles a DisplayText terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSs::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSS_TERMINALRESPONSE, "CSatNotifySendSs::TerminalResponse" );
    TFLOGSTRING("CSatNotifySendSs::TerminalResponse");

    TInt ret( KErrNone );

    // Buffer for additional information
    TBuf8<RSat::KAdditionalInfoMaxSize> additionalInfo;

    RSat::TSendSsRspV2Pckg* aRspPckg =
            reinterpret_cast<RSat::TSendSsRspV2Pckg*>( aRsp );
    RSat::TSendSsRspV2& rspV2 = ( *aRspPckg ) ();

    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If no SS error, set additional info
    if ( RSat::KSsReturnError != rspV2.iGeneralResult )
        {
        // iAdditionalInfo is 8-bit
        additionalInfo.Copy( rspV2.iAdditionalInfo );

        if ( RSat::KSendSsInfo == rspV2.iInfoType && additionalInfo.Length() )
            {
            // Remove operation code.
            // SATUI appends iOperationCode to additional info.
            // RMmCustomAPI::TSsAdditionalInfo iAddtionalInfo includes
            // correct SS Return Result Operation code, so fisrt byte of
            // additionalInfo is not needed.
            additionalInfo.Delete( 0, 1 );
            }
        }
    else
        {
        // SS error code, one byte
        // For the general result "SS Return Error", it is mandatory for the ME
        // to provide additional information. The first byte shall be the error
        // value given in the Facility (Return result) information element
        // returned by the network (as defined in TS 24.080).
        if ( 2 <= rspV2.iAdditionalInfo.Length() )
            {
            additionalInfo.Append( rspV2.iAdditionalInfo[1] );
            }
        else
            {
            additionalInfo.Append( 0x00 );
            }
        }

    // Creating the info message
    iSatMessHandler->SendSsTerminalResp( iTransId,
                                         iCommandDetails,
                                         TUint8( rspV2.iGeneralResult ),
                                          additionalInfo );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::CheckCallForwarding
// Check SS string if it is for call forwarding. If it is
// append '+' character to pretending international number.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendSs::CheckCallForwarding
        (
        TPtrC8 aSource,                 // original string
        RSat::TSsString& aSsString      // where to store modified string
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSS_CHECKCALLFORWARDING, "CSatNotifySendSs::CheckCallForwarding" );
    TFLOGSTRING("CSatNotifySendSs::CheckCallForwarding");
    _LIT8(KCFU,"*21*");    // Call forwarding unconditional (CFU)
    _LIT8(KCFB,"*67*");    // Call forwarding on Mobile Subscriber Busy(CFB)
    _LIT8(KCFNRy, "*61*"); // Call forwarding on No Reply (CFNRy)
    _LIT8(KCFNRc, "*62*"); // Call forwarding on Mobile Subscriber Not
                           // Reachable (CFNRc)
    // Try to find call forwarding string
    TInt pos( aSource.Find( KCFU ) );
    if ( KErrNotFound == pos )
        {
        pos = aSource.Find( KCFB );
        }
    if ( KErrNotFound == pos )
        {
        pos = aSource.Find( KCFNRy );
        }
    if ( KErrNotFound == pos )
        {
        pos = aSource.Find( KCFNRc );
        }
    if ( KErrNotFound != pos )
        {
        // String is for call forwarding
        aSsString.iSsString.Zero();
        aSsString.iSsString.Append( aSource.Left( pos + 4 ) );
        aSsString.iSsString.Append( KPlusMarkCharacterCode );
        aSsString.iSsString.Append(
            aSource.Right( aSource.Length() - ( pos + 4) ) );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::CheckSsStringValidity
// Checks validity of SS string.If string includes undefined SS
// characters or length is zero then return KErrCorrupt.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSs::CheckSsStringValidity
        (
        TPtrC8 aSsString
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDSS_CHECKSSSTRINGVALIDITY, "CSatNotifySendSs::CheckSsStringValidity" );
    TFLOGSTRING("CSatNotifySendSs::CheckSsStringValidity");
    TInt ret( KErrNone );
    if ( !aSsString.Length() )
        {
        // If length is zero
        ret = KErrCorrupt;
        }
    _LIT8( KDtmf,            "p");
    _LIT8( KWild,            "w");
    _LIT8( KExpansionDigit,  ".");
    // SS string can contain only digits, star '*' and dash '#' characters.
    if ( KErrNotFound != aSsString.Find( KDtmf )
        || KErrNotFound != aSsString.Find( KWild )
        || KErrNotFound != aSsString.Find( KExpansionDigit ) )
        {
        // Incompatible character found
        ret = KErrCorrupt;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifySendUssd::CSatNotifySendUssd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySendUssd::CSatNotifySendUssd
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSendUssdV1Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDUSSD_CSATNOTIFYSENDUSSD, "CSatNotifySendUssd::CSatNotifySendUssd" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KModifiedByCallControl + RSat::KUssdTransactionTerminatedByUser
        + RSat::KNetworkUnableToProcessCmd
        + RSat::KInteractionWithCCTemporaryError
        + RSat::KErrorRequiredValuesMissing + RSat::KUssdReturnError
        + RSat::KInteractionWithCCPermanentError;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::~CSatNotifySendUssd
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySendUssd::~CSatNotifySendUssd()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDUSSD_CSATNOTIFYSENDUSSD, "CSatNotifySendUssd::~CSatNotifySendUssd" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::MessageReceived
// Handles a SendUssd proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendUssd::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDUSSD_MESSAGERECEIVED, "CSatNotifySendUssd::MessageReceived" );
    TFLOGSTRING("TSY:CSatNotifySendUssd::MessageReceived");
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
    //get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        TUint8 noCause( 0 );
        TBuf<1>  noTextString;
        TUint8  paramNotSet( 0 );
        iSatMessHandler->SendUssdTerminalResp( iTransId,
                                    iCommandDetails,
                                    RSat::KMeUnableToProcessCmd,
                                    noCause,
                                    noTextString,
                                    paramNotSet );
        }
    else
        {
        // Fill the Send Ussd structure
        RSat::TSendUssdV1& sendUssdV1 = ( *iSendUssdV1Pckg )();
        sendUssdV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        TPtrC8 sourceStr; // Used with Copy8to16 function
        // Alpha id string (optional)
        sendUssdV1.iAlphaId.iAlphaId.Zero();
        CTlv alphaIdentifier;
        returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
            KTlvAlphaIdentifierTag );
        if ( KErrNotFound != returnValue )
            {
            TUint16 alphaIdLength = alphaIdentifier.GetLength() ;
            if ( RSat::KAlphaIdMaxSize < alphaIdLength )
                {
                // String too long
                TUint8 noCause( 0 );
                TBuf<1>  noTextString;
                TUint8  paramNotSet( 0 );
                iSatMessHandler->SendUssdTerminalResp( iTransId,
                                iCommandDetails,
                                RSat::KMeUnableToProcessCmd,
                                noCause,
                                noTextString,
                                paramNotSet );
                ret = KErrCorrupt;
                }
            else if ( alphaIdLength )
                {
                // get the alpha id
                sourceStr.Set( alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );

                // convert and set the alpha id
                TSatUtility::SetAlphaId( sourceStr ,
                    sendUssdV1.iAlphaId.iAlphaId );
                }
            // Check alpha id status
            // Alpha Tag present
            if ( sendUssdV1.iAlphaId.iAlphaId.Length() )
                {
                sendUssdV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                }
            else
                {
                sendUssdV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
                }
            }
        else
            {
            sendUssdV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            }
        // Ussd string (mandatory)
        CTlv ussdTlv;
        sendUssdV1.iUssdString.iUssdString.Zero();
        returnValue = berTlv.TlvByTagValue( &ussdTlv,
            KTlvUssdStringTag );
        if ( KErrNone == returnValue )
            {
            // Get the data coding scheme from the ISI msg
            // and set the corresponding ETel Sat data field.
            // The DCS is coded as for Cell Broadcast.
            sendUssdV1.iUssdString.iDcs =
                ussdTlv.GetShortInfo( ETLV_DataCodingScheme );
            // Decode DCS
            TSmsDcs decodedDcs( ESmsUnknownOrReservedDcs );
            decodedDcs = TSatUtility::DecodeCbsDcs( sendUssdV1.iUssdString.iDcs );
            TPtrC8 ussdString = ussdTlv.GetData( ETLV_UssdString );
            TUint16 ussdStringLengthInBytes = TUint16( ussdString.Length() );
            if ( ( ( ESms16BitDcs==decodedDcs )
                   && 2*RSat::KStringMaxSize<ussdStringLengthInBytes)
               || ( ( ESms8BitDcs==decodedDcs )
                  && RSat::KStringMaxSize<ussdStringLengthInBytes)
               || ( ( ESms7BitDcs==decodedDcs )
                  && RSat::KStringMaxSize<8*ussdStringLengthInBytes/7))
                {
                // The Ussd text string is too long.
                TFLOGSTRING("TSY:CSatNotifySendUssd::MessageReceived, \
                USSD String too long");
                OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDUSSD_MESSAGERECEIVED, "CSatNotifySendUssd::MessageReceived, USSD String too long" );
                ret = KErrCorrupt;
                TUint8 noCause( 0 );
                TBuf<1>  emptyTextString;
                TUint8  paramNotSet( 0 );
                iSatMessHandler->SendUssdTerminalResp( iTransId,
                                    iCommandDetails,
                                    RSat::KCmdDataNotUnderstood,
                                    noCause,
                                    emptyTextString,
                                    paramNotSet );
                }
            else
                {
                // Conversion to 16-bit following the DCS
                switch ( decodedDcs )
                    {
                    case ESms7BitDcs:
                        {
                        TBuf8<RSat::KStringMaxSize> ussdString8;
                        TSatUtility::Packed7to8Unpacked( ussdString,
                            ussdString8 );
                        TSatUtility::ConvertSms7ToUnicode16(
                            sendUssdV1.iUssdString.iUssdString, ussdString8 );
                        break;
                        }
                    case ESms8BitDcs:
                        {
                        TSatUtility::ConvertSms7ToUnicode16(
                            sendUssdV1.iUssdString.iUssdString, ussdString );
                        break;
                        }
                    case ESms16BitDcs:
                        {
                        TIsiUtility::CopyFromBigEndian( ussdString , sendUssdV1.iUssdString.iUssdString );
                        break;
                        }
                    default:
                        {
                        TFLOGSTRING("TSY:CSatNotifySendUssd::MessageReceived, \
                        USSD DCS has a reserved value");
                        OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYSENDUSSD_MESSAGERECEIVED, "CSatNotifySendUssd::MessageReceived, USSD DCS has a reserved value" );
                        // the DCS has a reserved value
                        ret = KErrCorrupt;
                        TUint8 noCause( 0 );
                        TBuf<1>  emptyTextString;
                        TUint8  paramNotSet( 0 );
                        iSatMessHandler->SendUssdTerminalResp( iTransId,
                                            iCommandDetails,
                                            RSat::KCmdDataNotUnderstood,
                                            noCause,
                                            emptyTextString,
                                            paramNotSet );
                        break;
                        }
                    }
                }
            }
        else
            {
            TFLOGSTRING("TSY:CSatNotifySendUssd::MessageReceived, \
            Mandatory field missing");
            OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYSENDUSSD_MESSAGERECEIVED, "CSatNotifySendUssd::MessageReceived, Mandatory field missing" );
            // Mandatory field missing
            ret = KErrCorrupt;
            TUint8 noCause( 0 );
            TBuf<1>  noTextString;
            TUint8  paramNotSet( 0 );
            iSatMessHandler->SendUssdTerminalResp( iTransId,
                                iCommandDetails,
                                RSat::KErrorRequiredValuesMissing,
                                noCause,
                                noTextString,
                                paramNotSet );
            }
        if ( KErrNone == ret )
            {
            // Iconid
            TSatUtility::FillIconStructure( berTlv,
                sendUssdV1.iIconId );
            }
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SendUssd PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendUssd::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSENDUSSD_NOTIFY, "CSatNotifySendUssd::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("TSY:CSatNotifySendUssd::Notify. \n\t\t\t Handle:%d\n\t\t\t",
                aReqHandle );
    iReqHandle = aReqHandle;
    iSendUssdV1Pckg = static_cast< RSat::TSendUssdV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSendUssd );
    }

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::TerminalResponse
// Handles a SendUssd terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendUssd::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDUSSD_TERMINALRESPONSE, "CSatNotifySendUssd::TerminalResponse" );
    TFLOGSTRING("TSY:CSatNotifySendUssd::TerminalResponse");
    TInt ret( KErrNone );
    RSat::TSendUssdRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TSendUssdRspV1Pckg* >( aRsp );
    RSat::TSendUssdRspV1& rspV1 = ( *aRspPckg ) ();
    TUint8 additionalInfo( 0 );
    // Check that general result values are valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // If there is Me (Mobile Entity) error, network error or text string,
    // additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType )
        || ( RSat::KCallControlRequestedAction == rspV1.iInfoType )
        || ( RSat::KSatNetworkErrorInfo == rspV1.iInfoType )
        || ( RSat::KTextString == rspV1.iInfoType ) )
        {
        // Check the length of additional info:
        if ( 0 == rspV1.iAdditionalInfo.Length() )
            {
            // No info
            TFLOGSTRING("TSY:CSatNotifySendUssd::TerminalResponse, \
            AdditionalInfoType set, but no additional info available");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDUSSD_TERMINALRESPONSE, "CSatNotifySendUssd::TerminalResponse, AdditionalInfoType set, but no additional info available" );
            ret = KErrCorrupt;
            }
        else if ( RSat::KTextString == rspV1.iInfoType )
            {
            // Text string - additional info for a
            // successful GET INKEY, GET INPUT or SEND USSD.
            // --> Not used by SAT Server when the command has been
            // performed successfully, SAT Server uses the
            // rspV1.iUssdString.iUssdString to return the USSD string
            // sent by the network.
            TFLOGSTRING("TSY:CSatNotifySendUssd::TerminalResponse, \
            AdditionalInfoType set to TextString.");
            OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYSENDUSSD_TERMINALRESPONSE, "CSatNotifySendUssd::TerminalResponse, AdditionalInfoType set to TextString." );
            }
        else
            {
            // For the general result "USSD Return Error",
            // the ME shall provide additional information.
            // The first byte shall be the error value given in
            // the Facility (Return result) information element
            // returned by the network (as defined in TS 24.080 [10]).
            // One further value is defined:
            // -    '00' = No specific cause can be given.
            additionalInfo = TUint8( rspV1.iAdditionalInfo[0] );
            }
        }
    // Creating the info message
    iSatMessHandler->SendUssdTerminalResp( iTransId, iCommandDetails,
                               TUint8( rspV1.iGeneralResult ),
                               additionalInfo,
                               rspV1.iUssdString.iUssdString,
                               rspV1.iUssdString.iDcs );
    return ret;

    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpCall::CSatNotifySetUpCall
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySetUpCall::CSatNotifySetUpCall
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSetUpCallV1Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPCALL_CSATNOTIFYSETUPCALL, "CSatNotifySetUpCall::CSatNotifySetUpCall" );
    TFLOGSTRING( "TSY:CSatNotifySetUpCall::CSatNotifySetUpCall" );
    iCallConnectedEvent.Zero();

    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KModifiedByCallControl + RSat::KPSessionTerminatedByUser
        + RSat::KUssdTransactionTerminatedByUser
        + RSat::KNetworkUnableToProcessCmd + RSat::KPCmdNotAcceptedByUser
        + RSat::KCallClearedBeforeConnectionOrReleased
        + RSat::KInteractionWithCCTemporaryError + RSat::KSsReturnError
        + RSat::KErrorRequiredValuesMissing
        + RSat::KInteractionWithCCPermanentError;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpCall::~CSatNotifySetUpCall
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySetUpCall::~CSatNotifySetUpCall()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPCALL_CSATNOTIFYSETUPCALL, "CSatNotifySetUpCall::~CSatNotifySetUpCall" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpCall::MessageReceived
// Handles a SetUpCall proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpCall::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPCALL_MESSAGERECEIVED, "CSatNotifySetUpCall::MessageReceived" );
    TFLOGSTRING("CSatNotifySetUpCall::MessageReceived");
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        iSatMessHandler->SetUpCallTerminalResp( iTransId,
                                                 iCommandDetails,
                                                 RSat::KMeUnableToProcessCmd,
                                                 KNoCause );
        }
    else
        {
        // Fill the Set Up Call structure
        RSat::TSetUpCallV1& setUpCallV1 = ( *iSetUpCallV1Pckg )();
        // Store command number
        setUpCallV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
       switch ( iCommandDetails[KCommandQualifier] )
            {
            case KOnlyIfNotBusy:
                {
                // Set up call, if not busy on another call
                setUpCallV1.iType = RSat::EOnlyIfNotBusy;
                break;
                }
            case KOnlyIfNotBusyWithRedial:
                {
                // Set up call, if not busy on another call, with redial
                setUpCallV1.iType = RSat::EOnlyIfNotBusyWithRedial;
                break;
                }
            case KHoldOtherCalls:
                {
                // Set up call, put other calls (if any) on hold
                setUpCallV1.iType = RSat::EHoldOtherCalls;
                break;
                }
            case KHoldOtherCallsWithRedial:
                {
                // Set up call, put other calls (if any) on hold, with redial
                setUpCallV1.iType = RSat::EHoldOtherCallsWithRedial;
                break;
                }
            case KDisconnectOtherCalls:
                {
                // Set up call, disconnect other calls (if any)
                setUpCallV1.iType = RSat::EDisconnectOtherCalls;
                break;
                }
            case KDisconnectOtherCallsWithRedial:
                {
                // Set up call, disconnect other calls (if any), with redial
                setUpCallV1.iType = RSat::EDisconnectOtherCallsWithRedial;
                break;
                }
            default:
                {
                // Call type not set
                setUpCallV1.iType = RSat::ESetUpCallTypeNotSet;
                break;
                }
            }
        TPtrC8 sourceString; // Used with conversions
        // Alpha id string (optional)
        setUpCallV1.iAlphaIdConfirmationPhase.iAlphaId.Zero();
         // Call setup phase alphaid (optional)
        setUpCallV1.iAlphaIdCallSetUpPhase.iAlphaId.Zero();
        CTlv alphaIdentifier;
        // check alpha identifiers (for user confirmation phase and call set
        // up phase )
        returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
                                            KTlvAlphaIdentifierTag );
        // if alpha id string exist
        if ( KErrNone == returnValue )
            {
            // fist alpha identifier is for user confirmation phase
            if ( alphaIdentifier.GetLength() )
                {
                // set status
                setUpCallV1.iAlphaIdConfirmationPhase.iStatus =
                    RSat::EAlphaIdProvided;

                 // get alpha id string
                sourceString.Set( alphaIdentifier.GetData(
                    ETLV_AlphaIdentifier ) );

                // convert and set the alpha id
                TSatUtility::SetAlphaId( sourceString ,
                    setUpCallV1.iAlphaIdConfirmationPhase.iAlphaId );

                }
            // alpha id found for user confirmation phase, but length
            // is zero
            else
                {
                setUpCallV1.iAlphaIdConfirmationPhase.iStatus =
                    RSat::EAlphaIdNull;
                }
            }
        // no alpha id for user confirmation phase
        else
            {
            setUpCallV1.iAlphaIdConfirmationPhase.iStatus =
                RSat::EAlphaIdNotPresent;
            }
        // Get alpha identifier for call set up phase
        returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
                                            KTlvAlphaIdentifierTag, 1 );
        // if alpha id string exist
        if ( KErrNone == returnValue )
            {
            //second alpha identifier is for call set up phase
            if ( alphaIdentifier.GetLength() )
                {
                // set status
                setUpCallV1.iAlphaIdCallSetUpPhase.iStatus =
                    RSat::EAlphaIdProvided;
                 // get alpha id string
                sourceString.Set( alphaIdentifier.GetData(
                    ETLV_AlphaIdentifier ) );
                // convert and set the alpha id
                TSatUtility::SetAlphaId( sourceString,
                    setUpCallV1.iAlphaIdCallSetUpPhase.iAlphaId );

                }
            // alpha id found for set up call phase,  but length is zero
            else
                {
                setUpCallV1.iAlphaIdCallSetUpPhase.iStatus =
                    RSat::EAlphaIdNull;
                }
            }
        // no alpha id set up call phase
        else
            {
            setUpCallV1.iAlphaIdCallSetUpPhase.iStatus =
                RSat::EAlphaIdNotPresent;
            }
        // First icon for user confirmation phase
        TSatUtility::FillIconStructure(
            berTlv, setUpCallV1.iIconIdConfirmationPhase, 0 );
        // Second icon for set up call phase
        TSatUtility::FillIconStructure(
            berTlv, setUpCallV1.iIconIdCallSetUpPhase, 1 );
        // Expecting address
        CTlv addressTlv;
        returnValue = berTlv.TlvByTagValue( &addressTlv,
                                            KTlvAddressTag );
        if ( KErrNone == returnValue && addressTlv.GetLength() )
            {
            // Initialize ton and npi
            RSat::TTypeOfNumber ton( RSat::EUnknownNumber );
            RSat::TNumberingPlan npi( RSat::EUnknownNumberingPlan );
            // Call utility function that maps received TON and NPI to
            // RSat values
            TSatUtility::GetTonAndNpi(
                addressTlv.GetShortInfo( ETLV_TonAndNpi ) , &ton, &npi );
            // Set TON and NPI
            setUpCallV1.iAddress.iTypeOfNumber = ton;
            setUpCallV1.iAddress.iNumberPlan = npi;
            // Address
            setUpCallV1.iAddress.iTelNumber.Zero();
            // length is number of BCD characters,
            // multiply by two to get real length and - 2 for ton&npi
            TInt dialNumberLength( ( ( addressTlv.GetLength() * 2 ) - 2 ) );

           if ( dialNumberLength )
                {
                // Semi-octet presentation used
                sourceString.Set(
                    addressTlv.GetData( ETLV_DiallingNumberString ) );
                TBuf8<RSat::KMaxMobileTelNumberSize> tempNumber;
                // Convert BCD string to ascii
                TSatUtility::BCDToAscii( sourceString, tempNumber );
                sourceString.Set( tempNumber );
                //add '+' character to the preceding of international number
                if ( ( setUpCallV1.iAddress.iTypeOfNumber
                    == RSat::EInternationalNumber )
                        && (sourceString[0] != KPlusMarkCharacterCode )
                        && RSat::KMaxMobileTelNumberSize > dialNumberLength )
                    {
                    _LIT8( KPlusMark, "+");
                    tempNumber.Insert( 0, KPlusMark );
                    sourceString.Set( tempNumber );
                    }
                // The command may also include DTMF digits, which the ME shall
                // send to the network after the call has connected.
                // Remove expansion digits from it if present.
                TSatUtility::RemoveExpansionDigit( tempNumber );
                // Set actual number
                setUpCallV1.iAddress.iTelNumber.Copy( tempNumber );
                TFLOGSTRING2("TSY: SetUpCall, TelNumber: %S",
                &setUpCallV1.iAddress.iTelNumber );
                OstTraceExt1( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPCALL_MESSAGERECEIVED, "CSatNotifySetUpCall::MessageReceived TelNumber: %S", setUpCallV1.iAddress.iTelNumber );
                }
            }
        else
            {
            // address is missing, returning response immediately
            iSatMessHandler->SetUpCallTerminalResp( iTransId,
                                    iCommandDetails,
                                    RSat::KCmdDataNotUnderstood,
                                    KNoCause );
            ret = KErrCorrupt;
            }
        // Bearer capability
        setUpCallV1.iCapabilityConfigParams.Zero();
        CTlv capabilityConfig;
        returnValue = berTlv.TlvByTagValue( &capabilityConfig,
            KTlvCapabilityConfigurationParametersTag );
        if ( ( KErrNone == returnValue )
            && ( capabilityConfig.GetLength() ) )
            {
            // iCapabilityConfigParams is 8-bit string
            setUpCallV1.iCapabilityConfigParams.Append( capabilityConfig.
                GetData( ETLV_CapabilityConfigurationParameters ) );
            }
        // Called Party SubAddress
        setUpCallV1.iSubAddress.Zero();
        CTlv subAddress;
        returnValue = berTlv.TlvByTagValue( &subAddress, KTlvSubaddressTag );
        if ( KErrNone == returnValue )
            {
            if ( subAddress.GetLength()
                || subAddress.GetComprehensionRequired() )
                {
                TFLOGSTRING("TSY: SetUpCall, SubAddress is not supported!" );
                OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYSETUPCALL_MESSAGERECEIVED, "SetUpCall, SubAddress is not supported!" );
                iSatMessHandler->SetUpCallTerminalResp( iTransId,
                                        iCommandDetails,
                                        RSat::KCmdBeyondMeCapabilities,
                                        KNoCause );
                ret = KErrCorrupt;
                }
            }

        // Duration (optional, maximum duration for the redial mechanism)
        TSatUtility::FillDurationStructure(
            berTlv, setUpCallV1.iDuration );

        // Set SetUpCall flag on for Call connected event. To avoid situation
        // that event is send before terminal response
        if ( KErrNone == ret )
            {
            iSatMessaging->SetSetUpCallStatus( ETrue );
            }
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpCall::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SetUpCall PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpCall::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPCALL_NOTIFY, "CSatNotifySetUpCall::Notify" );
    TFLOGSTRING("CSatNotifySetUpCall::Notify");
    iReqHandle = aReqHandle;
    iSetUpCallV1Pckg = static_cast< RSat::TSetUpCallV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSetUpCall );
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpCall::TerminalResponse
// Handles a SetUpCall terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpCall::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPCALL_TERMINALRESPONSE, "CSatNotifySetUpCall::TerminalResponse" );
    TFLOGSTRING("CSatNotifySetUpCall::TerminalResponse");
    TInt    ret( KErrNone );
    TUint8  additionalInfo( 0 );
    RSat::TSetUpCallRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TSetUpCallRspV2Pckg* >( aRsp );
    RSat::TSetUpCallRspV2& rspV2 = ( *aRspPckg ) ();

    // Check that general result values are valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Entity) error or network error,
    // additional info is needed
    if ( ( RSat::KSatNetworkErrorInfo == rspV2.iInfoType )
        || ( RSat::KMeProblem == rspV2.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV2.iAdditionalInfo.Length() )
            {
            additionalInfo = static_cast<TUint8>( rspV2.iAdditionalInfo[0] );
            }
        else
            {
             // No info
            ret = KErrCorrupt;
            }
        }

    // Creating the terminal response message
    iSatMessHandler->SetUpCallTerminalResp( iTransId,
        iCommandDetails, TUint8( rspV2.iGeneralResult ), additionalInfo );

    // Clear flag
    iSatMessaging->SetSetUpCallStatus( EFalse );

    // In the case of a call initiated through a SET UP CALL proactive command
    // while the call connected event is part of the current event list, the
    // ME shall send both the TERMINAL RESPONSE related to the proactive
    // command, and the EVENT DOWNLOAD command, in the order TERMINAL RESPONSE
    // first, ENVELOPE(EVENT DOWNLOAD - call connected) second.
    // Check if there is stored envelope in the buffer
    if ( iCallConnectedEvent.Length() )
        {
        // Send envelope..
        iSatMessHandler->UiccCatReqEnvelope( iSatMessaging->GetTransactionId(),
                                             iCallConnectedEvent );
        // ..and clear the buffer
        iCallConnectedEvent.Zero();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpCall::StoreCallConnectedEvent
// Store a Call connected envelope to buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpCall::StoreCallConnectedEvent
        (
        const TDesC8& aEnvelope // envelope received from CSatEventDownload
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPCALL_STORECALLCONNECTEDEVENT, "CSatNotifySetUpCall::StoreCallConnectedEvent" );
    // store it to the buffer
    iCallConnectedEvent = aEnvelope;
    }


// -----------------------------------------------------------------------------
// CSatNotifyRefresh::CSatNotifyRefresh
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyRefresh::CSatNotifyRefresh
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iRefreshV2Pckg( NULL ),
        iRefreshRequiredV2Pckg( NULL ),
        iReqHandleRefreshRequired( NULL ),
        iInternalCache( KZero )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_CSATNOTIFYREFRESH, "CSatNotifyRefresh::CSatNotifyRefresh" );
    iFileList.Zero();
    iAid.Zero();

    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KRefreshAdditionEFRead
        + RSat::KRefreshUSIMNotActive + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::~CSatNotifyRefresh
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyRefresh::~CSatNotifyRefresh()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYREFRESH_CSATNOTIFYREFRESH, "CSatNotifyRefresh::~CSatNotifyRefresh" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::MessageReceived
// Handles a Refresh proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyRefresh::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived" );
    TFLOGSTRING("TSY: CSatNotifyRefresh::MessageReceived");
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
     // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv. Returned to SIM card in terminal resp.
    iCommandDetails.Copy( commandDetails.Data() );
    // Store Transaction id
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle || !iReqHandleRefreshRequired )
        {
        TFLOGSTRING3("TSY: CSatNotifyRefresh::MessageReceived, iReqHandle:%d,\
        iReqHandleRefreshRequired:%d ", iReqHandle, iReqHandleRefreshRequired );
        OstTraceExt2( TRACE_NORMAL, DUP1_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived iReqHandle: %u, iReqHandleRefreshRequired: %u", iReqHandle, iReqHandleRefreshRequired );

        // Some of the request were not on, returning response immediately
        iSatMessHandler->RefreshTerminalResp(
            iTransId,                       // Transaction id
            iCommandDetails,                // Command details tlv
            RSat::KMeUnableToProcessCmd,    // Result
            RSat::KNoSpecificMeProblem );   // Additional info
        }
    else
        {
        // Fill the refresh structure
        RSat::TRefreshV2& refreshV2 = ( *iRefreshRequiredV2Pckg )();
        TInt returnValue( KErrNone );
        // Is the command valid
        TInt ret( KErrNone );
         // File list
        refreshV2.iFileList.Zero();
        iFileList.Zero();
        iInternalCache = 0;

        CTlv fileListTlv;
        returnValue = berTlv.TlvByTagValue( &fileListTlv,
                                            KTlvFileListTag );
        if ( KErrNone == returnValue )
            {
            // Filelist found
            // Check if file list really contains any files
            if ( KZero < fileListTlv.GetLength() )
                {
                // File list length, -1 for number of files
                TUint8 fileLength( TUint8( fileListTlv.GetLength() - 1 ) );
                // Save file list with full path.
                // Needed when creating a SIM_ATK_REQ isi-message
                iFileList = fileListTlv.GetData( ETLV_Files );
                // Going through all files
                // Received as 8-bit, append to 16-bit
                TInt i( 0 );
                for ( i = 0; i < fileLength / 2; i++ )
                    {
                    TUint16 oneItem( 0 );
                    // read 16-bit
                    TSatUtility::GetWord( oneItem, iFileList, 2 * i );
                    // File header 8-bit
                    TUint8 header( oneItem >> 8 );
                    // The TSY needs to strip the Refresh File List
                    // provided by the ICC to remove the paths to the files.
                    //'3FXX': Master File;
                    //'7FXX': 1st level Dedicated File;
                    //'5FXX': 2nd level Dedicated File;
                    if ( ( KRefreshMasterFileHeader != header )
                        && ( KRefresh1StLevelDedicatedFileHeader != header )
                        && ( KRefresh2StLevelDedicatedFileHeader != header ) )
                        {
                        // Append file to the file list, without path
                        refreshV2.iFileList.Append( oneItem );
                        }
                    }
                // Check if the file list contains the EF-SST file which is
                // cached in SIM ATK TSY
                if ( KErrNotFound != refreshV2.iFileList.Locate( RSat::KSstEf ) )
                    {
                    iInternalCache |= KCacheEFSST;
                    }
                // Check EF-CBMID
                if ( KErrNotFound != refreshV2.iFileList.Locate( RSat::KCbmidEf ) )
                    {
                    iInternalCache |= KCacheEFCBMID;
                    }
                }
            }
        else if ( iCommandDetails[KCommandQualifier] ==
                KFileChangeNotification )
            {
            // File list object is missing, return terminal resp immediately.
            ret = KErrCorrupt;
            iSatMessHandler->RefreshTerminalResp( iTransId,
                                                   iCommandDetails,
                                                   RSat::KErrorRequiredValuesMissing,
                                                   RSat::KNoSpecificMeProblem );
            }
        else
            {
            TFLOGSTRING("TSY: CSatNotifyRefresh::MessageReceived, \
            Return value of file list TLV not valid.");
            OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Return value of file list TLV not valid." );
            }
        if ( KErrNone == ret )
            {
            // Set refresh mode
            switch ( iCommandDetails[KCommandQualifier] )
                {
                case KSimInitFullFileChangeNotification:
                    {
                    TFLOGSTRING("TSY: SAT, Refresh mode: Sim init and \
                    full file change notification");
                    OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Refresh mode: Sim init and full file change notification" );
                    refreshV2.iType = RSat::ESimInitFullFileChangeNotification;
                    iInternalCache = KCacheEFSST + KCacheEFCBMID;
                    break;
                    }
                case KFileChangeNotification:
                    {
                    TFLOGSTRING("TSY: SAT, refresh mode: File Change \
                    Notification");
                    TFLOGSTRING2("TSY: Number of files: %d",
                    refreshV2.iFileList.Length() );

                    OstTrace0( TRACE_NORMAL, DUP4_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Refresh mode: File Change Notification" );
                    OstTrace1( TRACE_NORMAL, DUP5_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Number of files: %d", refreshV2.iFileList.Length() );

                    refreshV2.iType = RSat::EFileChangeNotification;
                    if ( KErrNotFound !=
                        refreshV2.iFileList.Locate( RSat::KImsiEf )
                            && !fileListTlv.GetComprehensionRequired() )
                        {
                        // IMSI was part of file list and the CR bit was cleared
                        // This is done here because client does not have the CR
                        // bit information available,
                        // Other error values comes from Client
                        ret = KErrCorrupt;
                        iSatMessHandler->RefreshTerminalResp( iTransId,
                            iCommandDetails,
                            RSat::KErrorRequiredValuesMissing,
                            RSat::KNoSpecificMeProblem );
                        }
                    break;
                    }
                case KSimInitFileChangeNotification:
                    {
                    TFLOGSTRING("TSY: SAT, Refresh mode: Sim init and \
                    file change notification");
                    TFLOGSTRING2("TSY: Number of files: %d",
                    refreshV2.iFileList.Length() );

                    OstTrace0( TRACE_NORMAL, DUP6_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Refresh mode: Sim init and file change notification" );
                    OstTrace1( TRACE_NORMAL, DUP7_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Number of files: %d", refreshV2.iFileList.Length() );

                    refreshV2.iType = RSat::ESimInitFileChangeNotification;
                    break;
                    }
                case KSimInit:
                    {
                    TFLOGSTRING("TSY: SAT, Refresh mode: Sim init ");
                    OstTrace0( TRACE_NORMAL, DUP8_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Refresh mode: Sim init" );
                    refreshV2.iType = RSat::ESimInit;
                    iInternalCache = KCacheEFSST + KCacheEFCBMID;
                    break;
                    }
                case KSimReset:
                    {
                    refreshV2.iType = RSat::ESimReset;
                    TFLOGSTRING("TSY: SAT, Refresh mode: Reset");
                    OstTrace0( TRACE_NORMAL, DUP9_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Refresh mode: Reset" );
                    iInternalCache = KCacheEFSST + KCacheEFCBMID;
                    break;
                    }
                case KUSIMApplicationReset:
                    {
                    TFLOGSTRING("TSY: SAT, Refresh mode: USIM Application Reset");
                    OstTrace0( TRACE_NORMAL, DUP10_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Refresh mode: USIM Application Reset" );
                    refreshV2.iType = RSat::EUsimApplicationReset;
                    iInternalCache = KCacheEFSST + KCacheEFCBMID;
                    break;
                    }
                case K3GSessionReset:
                    {
                    TFLOGSTRING("TSY: SAT, Refresh mode: 3G Session Reset");
                    OstTrace0( TRACE_NORMAL, DUP11_CSATNOTIFYREFRESH_MESSAGERECEIVED, "CSatNotifyRefresh::MessageReceived, Refresh mode: 3G Session Reset" );
                    refreshV2.iType = RSat::E3GSessionReset;
                    break;
                    }
                default:
                    // This migth be an error case
                    refreshV2.iType = RSat::ERefreshTypeNotSet;
                    break;
                }
            // Application identifies(optional)
            // If an AID TLV is present, it indicates the USIM application which
            // needs to be refreshed. If it is not present, the ME shall assume
            // the current USIM application needs to be refreshed.
            CTlv applicationId;
            returnValue = berTlv.TlvByTagValue( &applicationId, KTlvAIDTag );
            iAid.Zero();
            if ( KErrNone == returnValue )
                {
                // Set application identifies
                refreshV2.iAid = applicationId.GetData( ETLV_AID );
                // Store aplication Id to member variable.
                iAid = refreshV2.iAid;
                }
            }
        // Copy parameters
        (*iRefreshV2Pckg)() = ( *iRefreshRequiredV2Pckg )();
        TTsyReqHandle tempReqHandle = iReqHandleRefreshRequired;
        iReqHandleRefreshRequired = NULL;
        // Ask permission from client
        iSatMessaging->ReqCompleted( tempReqHandle, ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a Refresh PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyRefresh::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_NOTIFY, "CSatNotifyRefresh::Notify" );
    TFLOGSTRING("TSY: CSatNotifyRefresh::Notify");
    iReqHandle = aReqHandle;
    iRefreshV2Pckg = static_cast< RSat::TRefreshV2Pckg* >( aDataPtr );
    if ( iReqHandleRefreshRequired )
        {
        // Check the command buffer, if both notifications are received
        iSatMessaging->SatReady( KRefresh );
        }

    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::NotifyRefreshRequired
// Notification requested by ETel server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyRefresh::NotifyRefreshRequired
        (
        const TTsyReqHandle aReqHandle,  // Request handle
        TDes8*              aDataPtr     // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_NOTIFYREFRESHREQUIRED, "CSatNotifyRefresh::NotifyRefreshRequired" );
    TFLOGSTRING("TSY: CSatNotifyRefresh::NotifyRefreshRequired");
    iReqHandleRefreshRequired = aReqHandle;
    iRefreshRequiredV2Pckg = static_cast< RSat::TRefreshV2Pckg* >( aDataPtr );
    if ( iReqHandle )
        {
        // Check the command buffer, if both notifications are received
        iSatMessaging->SatReady( KRefresh );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::CancelRefreshRequiredNotification
// Cancels the current notification request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyRefresh::CancelRefreshRequiredNotification
        (
        const TTsyReqHandle aReqHandle  // Request handle
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_CANCELREFRESHREQUIREDNOTIFICATION, "CSatNotifyRefresh::CancelRefreshRequiredNotification" );
    TFLOGSTRING("TSY: CSatNotifyRefresh::CancelRefreshRequiredNotification");
    // Check that the handle is valid
    if ( aReqHandle == iReqHandleRefreshRequired )
        {
        iSatMessaging->ReqCompleted( aReqHandle, KErrCancel );
        }
    else
        {
        // Handle is not valid
        iSatMessaging->ReqCompleted( aReqHandle, KErrCorrupt );
        }
    iReqHandleRefreshRequired = NULL;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::TerminalResponse
// Handles a Refresh terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyRefresh::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_TERMINALRESPONSE, "CSatNotifyRefresh::TerminalResponse" );
    TFLOGSTRING("TSY: CSatNotifyRefresh::TerminalResponse");
    RSat::TRefreshRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TRefreshRspV1Pckg* >( aRsp );
    RSat::TRefreshRspV1& rspV1 = ( *aRspPckg ) ();
    TUint8 additionalInfo( 0x00 );
    if ( RSat::KNoAdditionalInfo != rspV1.iInfoType )
        {
        additionalInfo = TUint8( rspV1.iAdditionalInfo[0] );
        }

    // Empty TR is sent to Simson, if refresh type was Reset or IMSI was
    // changed during the SIM Init refresh. Empty TR is not sent to SIM,
    if ( KSimReset == iCommandDetails[KCommandQualifier] )
        {
        TFLOGSTRING("TSY: CSatNotifyRefresh::TerminalResponse, Reset performed, send an empty TR");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYREFRESH_TERMINALRESPONSE, "CSatNotifyRefresh::TerminalResponse, Reset performed, send an empty TR" );
        TBuf8<1> emptyDescriptor( KNullDesC8 );
        iSatMessHandler->UiccCatReqTerminalResponse(
            emptyDescriptor, emptyDescriptor, iTransId );
        }
    else
        {
        TFLOGSTRING("TSY: CSatNotifyRefresh::TerminalResponse, S60 has done the Refresh" );
        OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYREFRESH_TERMINALRESPONSE, "CSatNotifyRefresh::TerminalResponse, S60 has done the Refresh" );
        // Send terminal response
        iSatMessHandler->RefreshTerminalResp(
                        iTransId,                        // Transaction id
                        iCommandDetails,                 // Command number
                        TUint8 ( rspV1.iGeneralResult ), // Result
                        additionalInfo );                // Additinal info
        }

    return iAllowedResults == rspV1.iGeneralResult ? KErrNone : KErrCorrupt;
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::RefreshAllowed
// Response to RefreshRequired notify. Tells if the refresh is allowed by
// the Client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyRefresh::RefreshAllowed
        (
        TDesC8* aDataPtr
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_REFRESHALLOWED, "CSatNotifyRefresh::RefreshAllowed" );
    TFLOGSTRING("TSY: CSatNotifyRefresh::RefreshAllowed ");
    RSat::TRefreshRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TRefreshRspV1Pckg* >( aDataPtr );
    RSat::TRefreshRspV1& rspV1 = ( *aRspPckg ) ();
    if ( ( RSat::KSuccess == rspV1.iGeneralResult )
        || ( RSat::KRefreshAdditionEFRead == rspV1.iGeneralResult ) )
        {
        // Refresh allowed, send refresh request to UICC
        iSatMessHandler->UiccCatReqRefresh(
            iSatMessaging->GetTransactionId(),
            Map3GppRefreshToUiccValues( iCommandDetails[KCommandQualifier] ),
            iFileList,
            iAid );
        }
    else
        {
        // Refresh not allowed by the client
        TFLOGSTRING("TSY: CSatNotifyRefresh::RefreshAllowed, refresh was not \
            allowed by the client");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYREFRESH_REFRESHALLOWED, "CSatNotifyRefresh::RefreshAllowed, refresh was not allowed by the client" );
        TUint8 additionalInfo( 0x00 );
        if ( RSat::KNoAdditionalInfo != rspV1.iInfoType )
            {
            additionalInfo = TUint8( rspV1.iAdditionalInfo[0] );
            }
        // Send terminal response
        iSatMessHandler->RefreshTerminalResp( iTransId,
                                              iCommandDetails,
                                              TUint8( rspV1.iGeneralResult ),
                                              additionalInfo );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::Map3GppRefreshToUiccValues
// Maps 3GPP refresh values to match with UICC values.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CSatNotifyRefresh::Map3GppRefreshToUiccValues
        (
        const TUint8 a3GppRefresh // 3GPP refresh level
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_MAP3GPPREFRESHTOUICCVALUES, "CSatNotifyRefresh::Map3GppRefreshToUiccValues" );
    TFLOGSTRING("TSY: CSatNotifyRefresh::Map3GppRefreshToUiccValues ");

    TUint8 serviceType( a3GppRefresh );
    switch ( serviceType )
        {
        case KSimInitFullFileChangeNotification:
            {
            serviceType = UICC_REFRESH_NAA_INIT_FULL_FILE_CHANGE;
            break;
            }
        case KFileChangeNotification:
            {
            serviceType = UICC_REFRESH_NAA_FILE_CHANGE;
            break;
            }
        case KSimInitFileChangeNotification:
            {
            serviceType = UICC_REFRESH_NAA_INIT_FILE_CHANGE;
            break;
            }
        case KSimInit:
            {
            serviceType = UICC_REFRESH_NAA_INIT;
            break;
            }
        case KSimReset:
            {
            serviceType = UICC_REFRESH_UICC_RESET;
            break;
            }
        case KUSIMApplicationReset:
            {
            serviceType = UICC_REFRESH_NAA_APPLICATION_RESET;
            break;
            }
        case K3GSessionReset:
            {
            serviceType = UICC_REFRESH_NAA_SESSION_RESET;
            break;
            }
        default:
            {
            break;
            }
        }
    return serviceType;
    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::CompleteRequest
// Overloads original protected CompleteRequest method from Base class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyRefresh::CompleteRequest
        (
        const TInt aError
        )
    {
    OstTraceExt2( TRACE_NORMAL, CSATNOTIFYREFRESH_COMPLETEREQUEST, "CSatNotifyRefresh::CompleteRequest Handle: %d, Error: %d", (TInt)iReqHandle, aError );
    TFLOGSTRING3("CSatNotifyRefresh::CompleteRequest. \n\t\t\t Handle:%d\n\t\t\t Error:%d",
               iReqHandle,
               aError);
    TTsyReqHandle tempReqHandle = iReqHandle;
    iReqHandle = NULL;
    iSatMessaging->ReqCompleted( tempReqHandle, aError );
    TFLOGSTRING("CSatNotifyRefresh::CompleteRequest. Request is now completed");
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYREFRESH_COMPLETEREQUEST, "CSatNotifyRefresh::CompleteRequest Request is now completed" );

    }

// -----------------------------------------------------------------------------
// CSatNotifyRefresh::CachedFiles
// Returns list of files that are cached in SIM ATK TSY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint16 CSatNotifyRefresh::CachedFiles() const
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYREFRESH_CACHEDFILES, "CSatNotifyRefresh::CachedFiles" );
    return iInternalCache;
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::CSatNotifySimSessionEnd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySimSessionEnd::CSatNotifySimSessionEnd
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSIMSESSIONEND_CSATNOTIFYSIMSESSIONEND, "CSatNotifySimSessionEnd::CSatNotifySimSessionEnd" );
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::~CSatNotifySimSessionEnd
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySimSessionEnd::~CSatNotifySimSessionEnd()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSIMSESSIONEND_CSATNOTIFYSIMSESSIONEND, "CSatNotifySimSessionEnd::~CSatNotifySimSessionEnd" );
    // None
    }


// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::MessageReceived
// Handles a SimSessionEnd proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySimSessionEnd::MessageReceived
        (
        const TIsiReceiveC& /*aIsiMessage*/ // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSIMSESSIONEND_MESSAGERECEIVED, "CSatNotifySimSessionEnd::MessageReceived" );
    TFLOGSTRING("CSatNotifySimSessionEnd::MessageReceived");
    if ( iReqHandle ) // If request on
        {
        CompleteRequest( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SimSessionEnd PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySimSessionEnd::Notify
        (
        const TTsyReqHandle aReqHandle,     // Request handle
        TDes8*           /* aDataPtr */     // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSIMSESSIONEND_NOTIFY, "CSatNotifySimSessionEnd::Notify" );
    TFLOGSTRING("CSatNotifySimSessionEnd::Notify");
    iReqHandle = aReqHandle;
    }


// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::CSatNotifySetUpIdleModeText
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySetUpIdleModeText::CSatNotifySetUpIdleModeText
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSetUpIdleModeTextV1Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPIDLEMODETEXT_CSATNOTIFYSETUPIDLEMODETEXT, "CSatNotifySetUpIdleModeText::CSatNotifySetUpIdleModeText" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::~CSatNotifySetUpIdleModeText
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySetUpIdleModeText::~CSatNotifySetUpIdleModeText()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPIDLEMODETEXT_CSATNOTIFYSETUPIDLEMODETEXT, "CSatNotifySetUpIdleModeText::~CSatNotifySetUpIdleModeText" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::MessageReceived
// Handles a SetUpIdleModeText proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpIdleModeText::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPIDLEMODETEXT_MESSAGERECEIVED, "CSatNotifySetUpIdleModeText::MessageReceived" );
    TFLOGSTRING("CSatNotifySetUpIdleModeText::MessageReceived");
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
     //get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        iSatMessHandler->SetUpIdleModeTextTerminalResp(
             iTransId,
             iCommandDetails,
             RSat::KMeUnableToProcessCmd,
             KNoCause );
        }
    else
        {
        // Fill the set up idle mode text structure
        RSat::TSetUpIdleModeTextV1& setUpIdleModeTextV1
            = ( *iSetUpIdleModeTextV1Pckg )();
        setUpIdleModeTextV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        //Initialize Type to notSet, coding scheme to unicode and text to zero
        setUpIdleModeTextV1.iType = RSat::ESetUpIdleModeTextTypeNotSet;
        setUpIdleModeTextV1.iCodingScheme = RSat::E16bitUCS2;
        setUpIdleModeTextV1.iText.Zero();
        CTlv textString;
        returnValue = berTlv.TlvByTagValue( &textString,
            KTlvTextStringTag );
        // If the returnValue is KErrNone
        if ( KErrNone == returnValue )
            {
            if ( textString.GetLength() )
                {
                // Convert and set text
                TSatUtility::SetText( textString, setUpIdleModeTextV1.iText );
                //set type
                setUpIdleModeTextV1.iType = RSat::EUpdateIdleModeText;
                }
            else
                {
                //set type
                setUpIdleModeTextV1.iType = RSat::ERemoveExistingIdleModeText;
                }
            }
        else
            {
            TFLOGSTRING("CSatNotifySetUpIdleModeText::MessageReceived \
            No TLV text string found");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPIDLEMODETEXT_MESSAGERECEIVED, "CSatNotifySetUpIdleModeText::MessageReceived No TLV text string found" );
            iSatMessHandler->SetUpIdleModeTextTerminalResp(
                iTransId,
                iCommandDetails,
                RSat::KCmdDataNotUnderstood,
                KNoCause );
            ret = KErrCorrupt;
            }
        // Iconid (optional)
        TSatUtility::FillIconStructure( berTlv,
            setUpIdleModeTextV1.iIconId );
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SetUpIdleModeText PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpIdleModeText::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPIDLEMODETEXT_NOTIFY, "CSatNotifySetUpIdleModeText::Notify" );
    TFLOGSTRING("CSatNotifySetUpIdleModeText::Notify");
    iReqHandle = aReqHandle;
    iSetUpIdleModeTextV1Pckg =
     static_cast< RSat::TSetUpIdleModeTextV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSetUpIdleModeText );
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::TerminalResponse
// Handles a SetUpIdleModeText terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpIdleModeText::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPIDLEMODETEXT_TERMINALRESPONSE, "CSatNotifySetUpIdleModeText::TerminalResponse" );
    TFLOGSTRING("CSatNotifySetUpIdleModeText::TerminalResponse");
    TInt   ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TSetUpIdleModeTextRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TSetUpIdleModeTextRspV1Pckg* >( aRsp );
    RSat::TSetUpIdleModeTextRspV1& rspV1 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // If there is Me (Mobile Equipment) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo = TUint8( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    // Creating the info message
    iSatMessHandler->SetUpIdleModeTextTerminalResp(
        iTransId,
        iCommandDetails,
        TUint8( rspV1.iGeneralResult ), additionalInfo );
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyCallControlRequest::CSatNotifyCallControlRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyCallControlRequest::CSatNotifyCallControlRequest
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iCallControlPckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCALLCONTROLREQUEST_CSATNOTIFYCALLCONTROLREQUEST, "CSatNotifyCallControlRequest::CSatNotifyCallControlRequest" );
    }

// -----------------------------------------------------------------------------
// CSatNotifyCallControlRequest::~CSatNotifyCallControlRequest
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyCallControlRequest::~CSatNotifyCallControlRequest()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYCALLCONTROLREQUEST_CSATNOTIFYCALLCONTROLREQUEST, "CSatNotifyCallControlRequest::~CSatNotifyCallControlRequest" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyCallControlRequest::MessageReceived
// Handles a CallControlRequest proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyCallControlRequest::MessageReceived
        (
        const TIsiReceiveC& /*aIsiMessage*/ // ISI  message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCALLCONTROLREQUEST_MESSAGERECEIVED, "CSatNotifyCallControlRequest::MessageReceived" );
    // In S60 phones, the call control request is received via an ISI
    // message coming from the ATK Guardian / Call server. That's why the body
    // of this method is empty, see the class CSatCC. The Etel Sat API is used
    // to pass to S60 Sat Server the alpha identifier provided by the SIM,
    // if any. The SIM can provide such an alpha id to tell the user that the
    // number to be called has been modified by the SIM.
    }

// -----------------------------------------------------------------------------
// CSatNotifyCallControlRequest::CompleteNotification
// Handles the Call Control notification received from ATK Guardian Server.
// Completes an Etel Sat API CallControlRequest pending request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyCallControlRequest::CompleteNotification
        (
        TDesC& aAlphaId,
        RSat::TControlResult aResult
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATION, "CSatNotifyCallControlRequest::CompleteNotification" );
    TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotification");
    //check that someone has requested this notifications
    if ( iReqHandle )
        {
        // Temporary storage for Alpha Id
        RSat::TAlphaId alphaId;
        // Alpha ID validity
        RSat::TAlphaIdValidity validity( RSat::EValidAlpaId );
        alphaId.iStatus = RSat::EAlphaIdProvided;
        alphaId.iAlphaId.Append( aAlphaId );
        // Fill the call control structure
        RSat::TCallControlV5& callControl = ( *iCallControlPckg )();

#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
        if ( RSat::KSatV6 == callControl.ExtensionId() )
            {
            callControl = ( RSat::TCallControlV6& )callControl;
            callControl = ( *iCallControlPckg )();
            }
#endif

        // Set default control result
        callControl.SetCcGeneralResult( aResult );
        callControl.SetAlphaId( validity, alphaId );
        // Complete notification
        CompleteRequest( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyCallControlRequest::CompleteNotification
// Handles the Call Control GPRS notification received from ATK Guardian Server.
// Completes an Etel Sat API CallControlRequest pending request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyCallControlRequest::CompleteNotification
        (
        const TDesC& aAlphaId,
        const RSat::TControlResult aResult
        )
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATION, "CSatNotifyCallControlRequest::CompleteNotification" );
    TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotification");
    // Check that someone has requested this notifications
    if ( iReqHandle )
        {
        // Create the call control structure
        RSat::TCallControlV5& callControl = ( *iCallControlPckg )();

#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
        // Check which version in use
        if ( RSat::KSatV6 == callControl.ExtensionId() )
            {
            callControl = ( RSat::TCallControlV6& )callControl;
            callControl = ( *iCallControlPckg )();
            }
#endif

        // Check if the Alpha ID is present
        if ( aAlphaId.Length() )
            {
            // Temporary storage for Alpha Id
            RSat::TAlphaId alphaId;
            // Alpha ID validity
            RSat::TAlphaIdValidity validity( RSat::EValidAlpaId );
            alphaId.iStatus = RSat::EAlphaIdProvided;
            alphaId.iAlphaId.Append( aAlphaId );
            // Set Alpha ID data
            callControl.SetAlphaId( validity, alphaId );
            }

        // The PDP Parameters need to be set even though they are not present.
        // Otherwise the Call Control Type will not be set to ECcPDPParameters
        // Temporary storage for PDP Context Parameters
        TPdpParameters pdpParameters;
        // Set empty PDP Parameters data
        callControl.SetPdpParameters( pdpParameters );

        // Set call control result
        callControl.SetCcGeneralResult( aResult );
        // In Call control on GPRS, action originator is always modem
        callControl.SetActionOriginator( RSat::EMeSideOriginator );
        // Complete notification
        CompleteRequest( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyCallControlRequest::CompleteNotificationL
// Completes an Etel Sat API CallControlRequest pending request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyCallControlRequest::CompleteNotificationL
        (
        const TUint8 aCcResult,
        TPtrC8 aEnvelopeResponse
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL" );
    TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL");
#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
    // Check that someone has requested this notifications
    if ( iReqHandle )
        {
        // When this method is used V6 structure is always used. Therefore
        // casting is needed. Otherwise compiler gives errors.
        // Create the call control structure.
        RSat::TCallControlV6& callControl =
            ( *(RSat::TCallControlV6Pckg*)iCallControlPckg )();

        switch ( aCcResult )
            {
            case KAllowed:
                {
                callControl.SetCcGeneralResult( RSat::EAllowedNoModification );
                // Cellmo continues with original operation
                callControl.SetActionOriginator( RSat::EMeSideOriginator );
                break;
                }
            case KRejected:
                {
                callControl.SetCcGeneralResult( RSat::ENotAllowed );
                // original operation is rejected
                callControl.SetActionOriginator( RSat::EMeSideOriginator );
                break;
                }
            case KModified:
                {
                callControl.SetCcGeneralResult(
                    RSat::EAllowedWithModifications );
                // With cellmo value ATK_MODIFIED, same action is modified to
                // similar action and cellmo takes care of this
                callControl.SetActionOriginator( RSat::EMeSideOriginator );
                break;
                }
            case KChanged:
                {
                callControl.SetCcGeneralResult(
                    RSat::EAllowedWithModifications );
                // With cellmo value ATK_CHANGED, same action is modified to
                // another action and client is supposed to care of next action
                callControl.SetActionOriginator( RSat::EClientOriginator );
                break;
                }
            default:
                {
                TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL\
                    Not a valid call control result");
                OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL Not a valid call control result" );
                break;
                }
            }
        TInt ret( KErrNotFound );
        // Initialize ton and npi
        RSat::TTypeOfNumber ton( RSat::EUnknownNumber );
        RSat::TNumberingPlan npi( RSat::EUnknownNumberingPlan );
        // incoming ber-tlv
        CBerTlv response;
        response.SetData( aEnvelopeResponse );

        CTlv ssString;
        CTlv ussdTlv;

        if ( KErrNone == response.TlvByTagValue( &ssString, KTlvSsStringTag ) )
            {
            TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                Modified to SS");
            OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL Modified to SS" );
            // This SS related
            RSat::TSsString tempSs;
            // Call utility function that maps received TON and NPI
            // to RSat values
            TSatUtility::GetTonAndNpi(
                ssString.GetShortInfo( ETLV_TonAndNpi ), &ton, &npi );
            // Set TON and NPI
            tempSs.iTypeOfNumber = ton;
            tempSs.iNumberPlan = npi;

            TBuf8<RSat::KStringMaxSize> tempString;
            // Check length of SS string.
            // BCD format = two digits "packed" in one characher
            if ( ssString.GetLength() * 2 <= RSat::KStringMaxSize )
                {
                // Semi-octet presentation used
                // Converting back to ASCII format
                TSatUtility::BCDToAscii(
                    ssString.GetData( ETLV_SsOrUssdString ),
                    tempString );

                tempSs.iSsString.Append( tempString );
                TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                    SetSendSsDetails");
                OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL SetSendSsDetails" );
                // Set SS string to struct
                callControl.SetSendSsDetails( tempSs );
                // Notify NokiaTSY that next SS request is SAT originated
                TFLOGSTRING("TSY: Inform NokiaTSY SS request being Call Control originated");
                OstTrace0( TRACE_NORMAL, DUP4_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "Inform NokiaTSY SS request being Call Control originated" );
                iSatMessaging->GetMessageRouter()->ExtFuncL(
                    ESatNotifyCallControlRequest,
                    NULL );
                }
            else
                {
                // SS string is too long
                ret = KErrCorrupt;
                }
            }
        else if ( KErrNone ==
                    response.TlvByTagValue( &ussdTlv, KTlvUssdStringTag ) )
            {
            TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                Modified to USSD");
            OstTrace0( TRACE_NORMAL, DUP5_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL Modified to USSD" );
            // This is USSD related
            RSat::TUssdString tempUssd;
            // Get the data coding scheme from the ISI msg
            // and set the corresponding ETel Sat data field.
            tempUssd.iDcs = ussdTlv.GetShortInfo( ETLV_DataCodingScheme );

            // Check length of USSD string
            if ( ussdTlv.GetLength() <= RSat::KStringMaxSize )
                {
                // Client wants USSD data as received from SIM
                tempUssd.iUssdString.Copy(
                    ussdTlv.GetData( ETLV_UssdString ) );
                TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                    SetSendUssdDetails");
                OstTrace0( TRACE_NORMAL, DUP6_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL SetSendUssdDetails" );
                callControl.SetSendUssdDetails( tempUssd );
                }
            else
                {
                // USSD string is too long
                ret = KErrCorrupt;
                }
            }
        else
            {
            TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                Modified to Call");
            OstTrace0( TRACE_NORMAL, DUP7_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL Modified to Call" );
            // This must be call related response
            CTlv addressTlv;

            RSat::TCallSetUpParams tempCallParams;

            ret = response.TlvByTagValue( &addressTlv, KTlvAddressTag );
            if ( KErrNone == ret && addressTlv.GetLength() )
                {
                TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                    Address found");
                OstTrace0( TRACE_NORMAL, DUP8_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL Address found" );

                // Call utility function that maps received TON and NPI to
                // RSat values
                TSatUtility::GetTonAndNpi(
                    addressTlv.GetShortInfo( ETLV_TonAndNpi ), &ton, &npi );
                // Set TON and NPI
                tempCallParams.iAddress.iTypeOfNumber = ton;
                tempCallParams.iAddress.iNumberPlan = npi;
                // Address
                tempCallParams.iAddress.iTelNumber.Zero();
                // length is number of BCD characters,
                // multiply by two to get real length and - 2 for ton&npi
                TInt dialNumberLength(
                    ( ( addressTlv.GetLength() * 2 ) - 2 ) );

                if ( dialNumberLength )
                    {
                    // Semi-octet presentation used
                    TBuf8<RSat::KMaxMobileTelNumberSize> tempNumber;
                    // Convert BCD string to ascii
                    TSatUtility::BCDToAscii(
                        addressTlv.GetData( ETLV_DiallingNumberString ),
                        tempNumber );

                    // add '+' character to the preceding of international
                    // number
                    if ( ( tempCallParams.iAddress.iTypeOfNumber
                        == RSat::EInternationalNumber )
                        && ( tempNumber[ 0 ] != KPlusMarkCharacterCode )
                        && RSat::KMaxMobileTelNumberSize > dialNumberLength )
                        {
                        _LIT8( KPlusMark, "+");
                        tempNumber.Insert( 0, KPlusMark );
                        }

                    // Set actual number
                    tempCallParams.iAddress.iTelNumber.Copy( tempNumber );
                    }
                }
            // Check if Capability configuration parameters 1 are found
            CTlv bcc1;
            ret = response.TlvByTagValue(
                &bcc1,
                KTlvCapabilityConfigurationParametersTag );

            if ( KErrNone == ret )
                {
                TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                    bcc1 found");
                OstTrace0( TRACE_NORMAL, DUP9_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL bcc1 found" );
                // BC length is the first byte in BC data
                tempCallParams.iCcp1.Copy(
                    bcc1.GetData( ETLV_CapabilityConfigurationParameters ) );
                }

            // Subaddress (optional)
            CTlv subAddress;
            ret = response.TlvByTagValue( &subAddress, KTlvSubaddressTag );
            if ( KErrNone == ret )
                {
                TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                    subaddress found");
                OstTrace0( TRACE_NORMAL, DUP10_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL subaddress found" );
                tempCallParams.iSubAddress.Copy(
                    subAddress.GetData( ETLV_SubAddress ) );
                }
            // Capability configuration parameters 2
            // --> BC repeat indicator is mandatory, only if BCC2 present
            CTlv bcc2;
            ret = response.TlvByTagValue(
                &bcc2,
                KTlvCapabilityConfigurationParametersTag,
                1 );

            if ( KErrNone == ret )
                {
                TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                    bcc2 found");
                OstTrace0( TRACE_NORMAL, DUP11_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL bcc2 found" );
                CTlv bcRepeat;
                ret = response.TlvByTagValue(
                    &bcRepeat,
                    KTlvBcRepeatIndicatorTag );

                if ( KErrNone == ret )
                    {
                    // If bcc2 present, BC repeat indicator is mandatory
                    if ( ATK_CIRCULAR_REPEAT_INDICATOR ==
                        bcRepeat.GetShortInfo( ETLV_BcRepeatIndicatorValues ) )
                        {
                        callControl.SetBCRepeatIndicator(
                            RSat::EBCAlternateMode );
                        }
                    else if ( ATK_SEQUENTIAL_REPEAT_INDICATOR ==
                        bcRepeat.GetShortInfo( ETLV_BcRepeatIndicatorValues ) )
                        {
                        callControl.SetBCRepeatIndicator(
                            RSat::EBCFallbackMode );
                        }
                    else
                        {
                        // repeat indicator not valid
                        ret = KErrCorrupt;
                        }
                    // If mandatory BC repeat indicator present
                    if ( ret != KErrCorrupt )
                        {
                        tempCallParams.iCcp2.Copy(
                            bcc2.GetData(
                            ETLV_CapabilityConfigurationParameters ) );
                        }
                    }
                }
            // set call parameters
            TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                SetCallSetUpDetails");
            OstTrace0( TRACE_NORMAL, DUP12_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL SetCallSetUpDetails" );
            callControl.SetCallSetUpDetails( tempCallParams );
            }
        // Check if alpha id present
        TPtrC8 sourceString; // Used with conversions
        RSat::TAlphaId tempAlphaId;
        RSat::TAlphaIdValidity tempValidity( RSat::EAlphaIdValidityNotSet );
        CTlv alphaIdentifier;
        // Get alpha id tlv from berTlv
        ret = response.TlvByTagValue(
            &alphaIdentifier,
            KTlvAlphaIdentifierTag );
        // if alpha id string exist
        if ( KErrNone == ret )
            {
            TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                Alpha id found");
            OstTrace0( TRACE_NORMAL, DUP13_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL Alpha id found" );
            if ( alphaIdentifier.GetLength() )
                {
                // 8-bit string to 16-bit string
                sourceString.Set(
                    alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );

                // Convert and set the alpha id
                TSatUtility::SetAlphaId(
                    sourceString, tempAlphaId.iAlphaId );
                // Set alpha id status
                tempAlphaId.iStatus = RSat::EAlphaIdProvided;
                tempValidity = RSat::EValidAlpaId;
                }
            else
                {
                tempAlphaId.iStatus = RSat::EAlphaIdNull;
                tempValidity = RSat::ENullAlphaId;
                }
            }
        // Alpha id not present
        else
            {
            TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
                Alpha ID not present");
            OstTrace0( TRACE_NORMAL, DUP14_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL Alpha ID not present" );
            tempAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            tempValidity = RSat::ENoAlphaId;
            }
        // Set alpha id values
        TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
            SetAlphaId");
        OstTrace0( TRACE_NORMAL, DUP15_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATIONL, "CSatNotifyCallControlRequest::CompleteNotificationL SetAlphaId" );
        callControl.SetAlphaId( tempValidity, tempAlphaId );
        TFLOGSTRING("CSatNotifyCallControlRequest::CompleteNotificationL \
            SetCallParamOrigin");
        OstTrace0( TRACE_NORMAL, DUP18_CSATNOTIFYCALLCONTROLREQUEST_COMPLETENOTIFICATION, "CSatNotifyCallControlRequest::CompleteNotification SetCallParamOrigin" );

        CompleteRequest( KErrNone );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CSatNotifyCallControlRequest::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a CallControlRequest PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyCallControlRequest::Notify
        (
        const TTsyReqHandle aReqHandle,
        TDes8* aDataPtr
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCALLCONTROLREQUEST_NOTIFY, "CSatNotifyCallControlRequest::Notify" );
    TFLOGSTRING("CSatNotifyCallControlRequest::Notify");
    iReqHandle = aReqHandle;

    iCallControlPckg = static_cast<RSat::TCallControlV5Pckg*>( aDataPtr );
    }


// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::CSatNotifyLaunchBrowser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyLaunchBrowser::CSatNotifyLaunchBrowser
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iLaunchBrowserV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLAUNCHBROWSER_CSATNOTIFYLAUNCHBROWSER, "CSatNotifyLaunchBrowser::CSatNotifyLaunchBrowser" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KNetworkUnableToProcessCmd + RSat::KPCmdNotAcceptedByUser
        + RSat::KLaunchBrowserError;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::~CSatNotifyLaunchBrowser
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyLaunchBrowser::~CSatNotifyLaunchBrowser()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYLAUNCHBROWSER_CSATNOTIFYLAUNCHBROWSER, "CSatNotifyLaunchBrowser::~CSatNotifyLaunchBrowser" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::MessageReceived
// Handles a LaunchBrowser proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyLaunchBrowser::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLAUNCHBROWSER_MESSAGERECEIVED, "CSatNotifyLaunchBrowser::MessageReceived" );
    TFLOGSTRING("CSatNotifyLaunchBrowser::MessageReceived");
    //get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    // Get command qualifier
    TUint8 cmdQualifier( commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        iSatMessHandler->LaunchBrowserTerminalResp(
            iTransId, iCommandDetails,
            RSat::KMeUnableToProcessCmd,
            KNoCause );
        }
    else if ( CSatMessHandler::ENotBusy != iSatMessHandler->SsStatus() )
        {
        TUint8 additionalInfo( RSat::KMeBusyOnSs );
        if( CSatMessHandler::EUssdBusy == iSatMessHandler->SsStatus() )
            {
            additionalInfo = RSat::KMeBusyOnUssd;
            }
        // Ss or Ussd transaction ongoing
        iSatMessHandler->LaunchBrowserTerminalResp(
            iTransId, iCommandDetails,
            RSat::KMeUnableToProcessCmd,
            additionalInfo );
        }
    else
        {
        TInt ret( KErrNone );
        TInt returnValue( KErrNone );
        // Launch browser structure
        RSat::TLaunchBrowserV2& launchBrowserV2 = ( *iLaunchBrowserV2Pckg )();
        launchBrowserV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ));
        switch ( cmdQualifier )
            {
            case KLaunchBrowserIfNotYetLaunched:
               {
                launchBrowserV2.iBrowserSel =
                    RSat::ELaunchBrowserIfNotAlreadyLaunched;
                }
                break;
            case KUseExistingBrowser:
                {
                launchBrowserV2.iBrowserSel = RSat::EUseExistingBrowser;
                break;
                }
            case KCloseAndLaunchNewBrowser:
                {
                launchBrowserV2.iBrowserSel =
                    RSat::ECloseExistingLaunchNewBrowserSession;
                break;
                }
            case KLaunchBrowserCmdQualifierNotUsed:
            case KLaunchBrowserCmdQualifierReserved:
                {
                iSatMessHandler->LaunchBrowserTerminalResp(
                    iTransId, iCommandDetails,
                    RSat::KCmdTypeNotUnderstood, KNoCause );
                ret = KErrCorrupt;
                break;
                }
            default:
                {
                launchBrowserV2.iBrowserSel = RSat::EBrowserSelectionNotSet;
                break;
                }
            }
        if ( KErrNone == ret )
            {
            CTlv browserId; // optional
            returnValue = berTlv.TlvByTagValue( &browserId,
                KTlvBrowserIdentityTag );
            if ( KErrNotFound != returnValue )
                {
                // Browser id 0x00-0x04 allowed, other values are RFU
                switch ( browserId.GetShortInfo( ETLV_BrowserIdentity ) )
                    {
                    case KDefaultBrowser:
                        {
                        launchBrowserV2.iBrowserId = RSat::EDefaultBrowser;
                        break;
                        }
#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
                    case KWMLBrowser:
                        {
                        launchBrowserV2.iBrowserId = RSat::EWMLBrowser;
                        break;
                        }
                    case KHTMLBrowser:
                        {
                        launchBrowserV2.iBrowserId = RSat::EHTMLBrowser;
                        break;
                        }
                    case KXHTMLBrowser:
                        {
                        launchBrowserV2.iBrowserId = RSat::EXHTMLBrowser;
                        break;
                        }
                    case KCHTMLBrowser:
                        {
                        launchBrowserV2.iBrowserId = RSat::ECHTMLBrowser;
                        break;
                        }
#endif
                    default:
                        {
                        // Object present but does not identify a known
                        // browser ID
                        launchBrowserV2.iBrowserId =
                            RSat::EBrowserIdNotSpecified;
                        break;
                        }
                    }
                }
            else
                {
                launchBrowserV2.iBrowserId = RSat::EBrowserIdNotPresent;
                }
            }
        if ( KErrNone == ret )
            {
            CTlv url; // mandatory
            returnValue = berTlv.TlvByTagValue( &url, KTlvUrlTag );
            if ( KErrNotFound != returnValue )
                {
                if ( RSat::KUrlMaxSize < url.GetLength() )
                    {
                    iSatMessHandler->LaunchBrowserTerminalResp(
                        iTransId, iCommandDetails,
                        RSat::KMeUnableToProcessCmd, KNoCause );
                    ret = KErrCorrupt;
                    }
                else
                    {
                    launchBrowserV2.iUrl.Copy( url.GetData( ETLV_Url ) );
                    }
                }
            else
                {
                iSatMessHandler->LaunchBrowserTerminalResp(
                    iTransId, iCommandDetails,
                    RSat::KErrorRequiredValuesMissing, KNoCause );
                ret = KErrCorrupt;
                }
            }
        if ( KErrNone == ret )
            {
            CTlv provisioningFileList; // optional
            returnValue = berTlv.TlvByTagValueMulti( &provisioningFileList,
                KTlvProvisioningReferenceFileTag );
            TPtrC8 fileRefData = provisioningFileList.Data();
            TUint8 numberOfFileRefData( 0 );
            TInt ind( 0 );
            TInt tlvLength( 0 );
            TInt currentTlv( 0 );
            if ( KErrNotFound != returnValue )
                {
                // search for number of file references
                TInt i( 0 );
                for ( i = 0; i < fileRefData.Length(); i += tlvLength )
                    {
                    //first determine if the length of the TLV is coded
                    // with 1 or 2 bytes.
                    if ( KTwoByteLengthCoding == fileRefData[ind+1] )
                        {
                        //length is coded with 2 bytes -> 1 extra byte required
                        // to be
                        //added to total length. Also TLV header bytes (2)
                        // must be added
                        //to total length
                        tlvLength = fileRefData[ind+2] + KTlvHeaderLength + 1;
                        }
                    else
                        {
                        //TLV header bytes (2) must be added to total length
                        tlvLength = fileRefData[ind+1]  + KTlvHeaderLength;
                        }

                    currentTlv = fileRefData[ind];
                    if ( KTlvProvisioningReferenceFileTag == currentTlv )
                        {
                            numberOfFileRefData++;
                        }

                    ind += tlvLength;
                    }
                RSat::TProvisioningFileRef newFileRef;
                // Set string length and pos
                TUint8 stringLength( 0 );
                TUint  pos( 2 );
                // add FileReferences to the structure launchBrowserV2
                for ( i = 0; ( i < numberOfFileRefData ) && ( KErrNone == ret );
                    i++ )
                    {
                    // Fill the newfileref
                    stringLength = TUint8( fileRefData[pos - 1] );
                    // Test whether the text contains more than "MF"
                    if ( ( RSat::KFileRefMaxSize < stringLength )
                        || ( 4 > stringLength ) )
                        {
                        iSatMessHandler->LaunchBrowserTerminalResp(
                            iTransId, iCommandDetails,
                            RSat::KLaunchBrowserError,
                            RSat::KMeUnableToReadProvisioningData );
                        ret = KErrCorrupt;
                        break;
                        }
                    newFileRef.Zero();
                    TUint8 x( 0 );
                    for ( x = 0; x < stringLength; x++ )
                        {
                        newFileRef.Append( fileRefData[pos+x] );
                        }
                     // Adding the new fileref
                    if ( KErrNoMemory ==
                         launchBrowserV2.AddFileRef( newFileRef ) )
                        {
                        // Too many or long menuitems
                        iSatMessHandler->LaunchBrowserTerminalResp(
                            iTransId, iCommandDetails,
                            RSat::KLaunchBrowserError,
                            RSat::KMeUnableToReadProvisioningData );
                        ret = KErrCorrupt;
                        }
                    // Calculating the new position
                    pos = TUint( pos + fileRefData[pos - 1] + 2 );
                    }
                }
            }
        if ( KErrNone == ret )
            {
            // Bearers (optional)
            CTlv bearerList;
            returnValue = berTlv.TlvByTagValue( &bearerList,
                KTlvBearerTag );
            if ( KErrNotFound != returnValue )
                {
                if ( RSat::KBearerListMaxSize >= bearerList.GetLength() )
                    {
                    TBuf8<RSat::KBearerListMaxSize> brList =
                        bearerList.GetData( ETLV_ListOfBearers );
                    // SMS or USSD are not available bearers.
                    TInt i( 0 );
                    for ( i = 0; i < brList.Length() ; i++)
                        {
                        if ( KCsdBearer == brList[i]
                            || KGprsBearer == brList[i] )
                            {
                            launchBrowserV2.iBearerList.Append( brList[i] );
                            }
                        }
                    if ( 0x00 == launchBrowserV2.iBearerList.Length() )
                        {
                        iSatMessHandler->LaunchBrowserTerminalResp(
                            iTransId, iCommandDetails,
                            RSat::KLaunchBrowserError,
                            RSat::KBearerUnvailable );
                        ret = KErrCorrupt;
                        }
                    }
                else
                    {
                    iSatMessHandler->LaunchBrowserTerminalResp(
                        iTransId, iCommandDetails,
                        RSat::KCmdDataNotUnderstood, KNoCause );
                    ret = KErrCorrupt;
                    }
                }
            }
        if ( KErrNone == ret )
            {
            // Text string (Gateway/Proxy)
            CTlv textString;
            returnValue = berTlv.TlvByTagValue( &textString,
                                                KTlvTextStringTag );
            if ( KErrNone == returnValue )
                {
                TSatUtility::SetText( textString, launchBrowserV2.iText );
                }
            }
        if ( KErrNone == ret )
            {
            // Alpha ID string (optional) for user confirmation phase
            CTlv alphaIdentifier;
            returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
                KTlvAlphaIdentifierTag );
            launchBrowserV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            if ( KErrNotFound != returnValue )
                {
                TUint8 alphaIdLength( alphaIdentifier.GetLength() );
                if ( alphaIdLength )
                    {
                    TPtrC8 sourceString;

                    // get the alpha id
                    sourceString.Set(
                        alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );

                    // convert and set the alpha id
                    TSatUtility::SetAlphaId( sourceString ,
                        launchBrowserV2.iAlphaId.iAlphaId );

                    launchBrowserV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    }
                else
                    {
                    launchBrowserV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
                    }
                }
            // Iconid
            TSatUtility::FillIconStructure( berTlv,
                launchBrowserV2.iIconId );
            CompleteRequest( ret );
            }

        } // end else

    }

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a LaunchBrowser PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyLaunchBrowser::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYLAUNCHBROWSER_NOTIFY, "CSatNotifyLaunchBrowser::Notify Handle :%u", aReqHandle );
    TFLOGSTRING2("CSatNotifyLaunchBrowser::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iLaunchBrowserV2Pckg = static_cast< RSat::TLaunchBrowserV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KLaunchBrowser );
    }

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::TerminalResponse
// Handles a LaunchBrowser terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLAUNCHBROWSER_TERMINALRESPONSE, "CSatNotifyLaunchBrowser::TerminalResponse" );
    TFLOGSTRING("CSatNotifyLaunchBrowser::TerminalResponse");
    TInt   ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TLaunchBrowserRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TLaunchBrowserRspV2Pckg* >( aRsp );
    RSat::TLaunchBrowserRspV2& rspV2 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // Check that infotype is valid: from enum TBrowserError in etelsat.h
    if ( ( RSat::KMeProblem != rspV2.iInfoType )
        && ( RSat::KNoAdditionalInfo != rspV2.iInfoType )
        && ( RSat::KBearerUnvailable != rspV2.iInfoType )
        && ( RSat::KBrowserUnavailable != rspV2.iInfoType )
        && ( RSat::KMeUnableToReadProvisioningData != rspV2.iInfoType )
        && ( RSat::KNoSpecificBrowserError != rspV2.iInfoType )
        )
        {
        ret = KErrCorrupt;
        }
    // If there is Me (Mobile Equipment) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV2.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV2.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo = TUint8( rspV2.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    iSatMessHandler->LaunchBrowserTerminalResp(
        iTransId,                                   // Transaction id
        iCommandDetails,                            // Command number
        TUint8( rspV2.iGeneralResult ),             // Result
        additionalInfo );                           // Additional info
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::CSatNotifySetUpEventList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySetUpEventList::CSatNotifySetUpEventList
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSetUpEventListV1Pckg( NULL ),
        iCmdQualifier( KZero ),
        iEvents( KZero )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPEVENTLIST_CSATNOTIFYSETUPEVENTLIST, "CSatNotifySetUpEventList::CSatNotifySetUpEventList" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::~CSatNotifySetUpEventList
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySetUpEventList::~CSatNotifySetUpEventList()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPEVENTLIST_CSATNOTIFYSETUPEVENTLIST, "CSatNotifySetUpEventList::~CSatNotifySetUpEventList" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::MessageReceived
// Handles a SetUpEventList proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpEventList::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPEVENTLIST_MESSAGERECEIVED, "CSatNotifySetUpEventList::MessageReceived" );
    TFLOGSTRING("CSatNotifySetUpEventList::MessageReceived");
    TInt ret( KErrNone );
    TUint8 generalResult( RSat::KSuccess );
    //get ber tlv
    CBerTlv berTlv;
    ret = berTlv.BerTlv( aIsiMessage );
    //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        generalResult = RSat::KMeUnableToProcessCmd;
        }
    else
        {
        // Let's fill the setup event list structure
        RSat::TSetUpEventListV1& setUpEventListV1 = ( *iSetUpEventListV1Pckg )();
        setUpEventListV1.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        CTlv eventListTlv;
        ret = berTlv.TlvByTagValue( &eventListTlv, KTlvEventListTag );
        //Initialize Type to Not set
        setUpEventListV1.iType = RSat::ESetUpEventListTypeNotSet;
        // If the returnValue is KErrNone
        if ( KErrNone == ret )
            {
            iEvents = 0;
            TPtrC8 setUpEventList = eventListTlv.GetData( ETLV_EventList );
            TUint16 eventListLength = eventListTlv.GetLength();
            if ( !eventListLength )
                {
                 setUpEventListV1.iType = RSat::ERemoveExistingEventList;
                 // remove events from TSY:s list
                 iSatMessaging->SetUpEventList( 0 ); // no events = 0
                }
            else
                {
                setUpEventListV1.iType = RSat::EUpdateEventList;
                TInt8 i( 0 );
                TBuf8<14> eventList;
                eventList.FillZ( 14 );
                for ( i = 0; i < eventListLength; i++ )
                    {
                    if(  KLocalConnection < setUpEventList[i] )
                        {
                        //Invalid event code
                        generalResult = RSat::KCmdBeyondMeCapabilities;
                        }
                    else if( !eventList[setUpEventList[i]] )
                        {
                        //Each event type shall not appear more than ones
                        eventList[ setUpEventList[i] ] = 0x01;
                        switch( setUpEventList[i] )
                            {
                            case KMTCall:
                                iEvents += RSat::KMTCall;
                                break;
                            case KCallConnected:
                                iEvents += RSat::KCallConnected;
                                break;
                            case KCallDisconnected:
                                iEvents += RSat::KCallDisconnected;
                                break;
                            case KLocationStatus:
                                iEvents += RSat::KLocationStatus;
                                break;
                            case KUserActivity:
                                setUpEventListV1.iEvents
                                    += RSat::KUserActivity;
                                iEvents += RSat::KUserActivity;
                                break;
                            case KIdleScreenAvailable:
                                setUpEventListV1.iEvents
                                    += RSat::KIdleScreenAvailable;
                                iEvents += RSat::KIdleScreenAvailable;
                                break;
                            case KCardReaderStatus:
                                setUpEventListV1.iEvents
                                    += RSat::KCardReaderStatus;
                                iEvents += RSat::KCardReaderStatus;
                                break;
                            case KLanguageSelection:
                                setUpEventListV1.iEvents
                                    += RSat::KLanguageSelection;
                                iEvents += RSat::KLanguageSelection;
                                break;
                            case KBrowserTermination:
                                setUpEventListV1.iEvents
                                    += RSat::KBrowserTermination;
                                iEvents += RSat::KBrowserTermination;
                                break;
                            case KDataAvailable:
                                setUpEventListV1.iEvents
                                    += RSat::KDataAvailable;
                                iEvents += RSat::KDataAvailable;
                                break;
                            case KChannelStatus:
                                setUpEventListV1.iEvents
                                    += RSat::KChannelStatus;
                                iEvents += RSat::KChannelStatus;
                                break;
                            case KAccessTechnologyChange:
                                iEvents += RSat::KAccessTechnologyChange;
                                break;
                            case KDisplayParamsChanges:
                                setUpEventListV1.iEvents
                                    += RSat::KDisplayParamsChanges;
                                iEvents += RSat::KDisplayParamsChanges;
                                break;
                            case KLocalConnection:
                                setUpEventListV1.iEvents
                                    += RSat::KLocalConnection;
                                iEvents += RSat::KLocalConnection;
                                break;
                            default:
                                break;
                            }
                        }
                    else
                        {
                        TFLOGSTRING("TSY: \
                        CSatNotifySetUpEventList::MessageReceived, \
                        Event not recognized or not supported.");
                        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSETUPEVENTLIST_MESSAGERECEIVED, "CSatNotifySetUpEventList::MessageReceived, Event not recognized or not supported." );
                        }
                    }
                }
            }
        else
            {
            //Event list is missing
            generalResult = RSat::KErrorRequiredValuesMissing;
            }
        }
    if( RSat::KSuccess != generalResult )
        {
        iSatMessHandler->SetUpEventListTerminalResp(
                                iTransId,
                                iCommandDetails,
                                generalResult,
                                KNoCause );  // KNoCause: 0
        }
    else
        {
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SetUpEventList PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpEventList::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSETUPEVENTLIST_NOTIFY, "CSatNotifySetUpEventList::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifySetUpEventList::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iSetUpEventListV1Pckg = static_cast< RSat::TSetUpEventListV1Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSetUpEventList );
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::TerminalResponse
// Handles a SetUpEventList terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpEventList::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSETUPEVENTLIST_TERMINALRESPONSE, "CSatNotifySetUpEventList::TerminalResponse" );
    TFLOGSTRING("CSatNotifySetUpEventList::TerminalResponse");
    TInt   ret( KErrNone );
    TUint8 additionalInfo( 0 );
    RSat::TSetUpEventListRspV1Pckg* aRspPckg =
            reinterpret_cast< RSat::TSetUpEventListRspV1Pckg* >( aRsp );
    RSat::TSetUpEventListRspV1& rspV1 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV1.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // If there is Me (Mobile Entity) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( NULL != rspV1.iAdditionalInfo.Length() )
            {
            additionalInfo = static_cast< TUint8 >( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    if( RSat::KSuccess == rspV1.iGeneralResult )
        {
        // set events to TSY:s list
        iSatMessaging->SetUpEventList( iEvents );
        }
    iSatMessHandler->SetUpEventListTerminalResp(
        iTransId,
        iCommandDetails,
        TUint8( rspV1.iGeneralResult ),
        additionalInfo );
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyPollingOff::CSatNotifyPollingOff
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyPollingOff::CSatNotifyPollingOff
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPOLLINGOFF_CSATNOTIFYPOLLINGOFF, "CSatNotifyPollingOff::CSatNotifyPollingOff" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollingOff::~CSatNotifyPollingOff
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyPollingOff::~CSatNotifyPollingOff()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYPOLLINGOFF_CSATNOTIFYPOLLINGOFF, "CSatNotifyPollingOff::~CSatNotifyPollingOff" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollingOff::MessageReceived
// Handles a PollingOff proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyPollingOff::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPOLLINGOFF_MESSAGERECEIVED, "CSatNotifyPollingOff::MessageReceived" );
    TFLOGSTRING("CSatNotifyPollingOff::MessageReceived");
    //get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !( iSatMessHandler->PollingOff() ) )
        {
        iSatMessHandler->SetPollingOff( ETrue );
        iSatMessHandler->SetPollingInterval(
            iTransId,
            KDefaultPollInterval );
        // Terminal response is sent when the response
        //from SIM server is received
        }
    else
        {
         // PollingOff is already off
        iSatMessHandler->PollingOffTerminalResp(
            iTransId,
            iCommandDetails,
            RSat::KSuccess,
            RSat::KNoAdditionalInfo );
            }
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollingOff::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a PollingOff PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyPollingOff::Notify
        (
        const TTsyReqHandle /*aReqHandle*/, // Request handle
        TDes8*              /*aDataPtr*/    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYPOLLINGOFF_NOTIFY, "CSatNotifyPollingOff::Notify" );
    // PollingOff is completely implemented by SimAtkTsy. Client
    // insn't notified about the command.
    }


// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CSatNotifyLocalInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyLocalInfo::CSatNotifyLocalInfo
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iLocalInfoV3Pckg( NULL ),
        iLocalInfoIsOngoing( EFalse )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLOCALINFO_CSATNOTIFYLOCALINFO, "CSatNotifyLocalInfo::CSatNotifyLocalInfo" );
    TFLOGSTRING("CSatNotifyLocalInfo::CSatNotifyLocalInfo");
    // Initialize response structure
    iLocalInfoRspV3 = RSat::TLocalInfoRspV3();

    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessLimitedService
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::~CSatNotifyLocalInfo
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyLocalInfo::~CSatNotifyLocalInfo()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYLOCALINFO_CSATNOTIFYLOCALINFO, "CSatNotifyLocalInfo::~CSatNotifyLocalInfo" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::MessageReceived
// Handles a LocalInfo proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyLocalInfo::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived" );
    TFLOGSTRING("CSatNotifyLocalInfo::MessageReceived");
    // Clear additional info
    iLocalInfoRspV3.iAdditionalInfo.Zero();
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iCommandDetails.Copy( commandDetails.Data() );
    // Trans id
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    switch ( iCommandDetails[KCommandQualifier] )
        {
        case RSat::KProvideLocalInfo:
            {
            TFLOGSTRING("CSatNotifyLocalInfo:: request: LOCAL INFO");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::Messagereceived Request: LOCAL INFO" );
            iLocalInfoIsOngoing = ETrue;

            // Send a net cell info request to the net server.
            // Use same transaction id what comes with proactive
            // command and compare that when response is received
            iSatMessHandler->NetCellInfoGetReq( iTransId );

            break;
            }
        case RSat::KProvideLocalInfoImei:
            {
            TFLOGSTRING("CSatNotifyLocalInfo:: request: IMEI");
            OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::Messagereceived Request: IMEI" );
            // Check if IMEI is received
            if ( iSatMessHandler->ImeiAvailable() )
                {
                // IMEI is received from info server
                iLocalInfoRspV3.iGeneralResult = RSat::KSuccess;
                }
            else
                {
                // No IMEI available at the moment
                iLocalInfoRspV3.iGeneralResult =
                    RSat::KMeUnableToProcessCmd;
                iLocalInfoRspV3.iAdditionalInfo.Append(
                    KNoSpecificCauseCanBeGiven );
                }
            break;
            }
        case RSat::KProvideLocalInformationNmr:
            {
            // Check if UTRAN Measurement Qualifier TLV is present
            CTlv utranMeasurement;
            if ( KErrNone == berTlv.TlvByTagValue( &utranMeasurement,
                                        KTlvUtranMeasurementQualifierTag ) )
                {
                // ME has to be connected to GERAN if UTRAN NMR's are requested
                if ( KNetworkModeUtran ==
                    iSatMessHandler->CurrentAccessTechnology() )
                    {
                    TFLOGSTRING("CSatNotifyLocalInfo:: request: UTRAN NMR");
                    OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::Messagereceived UTRAN NMR" );

                    switch( utranMeasurement.GetShortInfo(
                                ETLV_UtranMeasurementQualifier ) )
                        {
                        case KIntraFrequencyMeasurements:
                            {
                            iLocalInfoIsOngoing = ETrue;
                            TFLOGSTRING("CSatNotifyLocalInfo::INTRA_FREQ_NMR");
                            OstTrace0( TRACE_NORMAL, DUP4_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::Messagereceived INTRA_FREQ_NMR" );
                            iSatMessHandler->NetNeighbourCellsReq(
                                            iSatMessaging->GetTransactionId(),
                                            NET_SIM_INTRA_FREQ_NMR );
                            break;
                            }
                        case KInterFrequencyMeasurements:
                            {
                            iLocalInfoIsOngoing = ETrue;
                            TFLOGSTRING("CSatNotifyLocalInfo::INTER_FREQ_NMR");
                            OstTrace0( TRACE_NORMAL, DUP5_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived INTER_FREQ_NMR" );
                            iSatMessHandler->NetNeighbourCellsReq(
                                            iSatMessaging->GetTransactionId(),
                                            NET_SIM_INTER_FREQ_NMR );
                            break;
                            }
                        case KInterRatMeasurements:
                            {
                            iLocalInfoIsOngoing = ETrue;
                            TFLOGSTRING("CSatNotifyLocalInfo::INTER_RAT_NMR");
                            OstTrace0( TRACE_NORMAL, DUP6_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived INTER_RAT_NMR" );
                            iSatMessHandler->NetNeighbourCellsReq(
                                            iSatMessaging->GetTransactionId(),
                                            NET_SIM_INTER_RAT_NMR );
                            break;
                            }
                        default:
                            {
                            iLocalInfoRspV3.iGeneralResult =
                            RSat::KCmdDataNotUnderstood;
                            TerminalResponse();
                            break;
                            }
                        }
                    }
                else
                    {
                    // ME connected to GSM, cannot serve the requested command
                    iLocalInfoRspV3.iGeneralResult =
                    RSat::KMeUnableToProcessCmd;
                    iLocalInfoRspV3.iAdditionalInfo.Append( KNoService );
                    TerminalResponse();
                    }
                }
            else
                {
                TFLOGSTRING("CSatNotifyLocalInfo:: request: NMR");
                OstTrace0( TRACE_NORMAL, DUP7_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived Request: NMR" );
                iLocalInfoIsOngoing = ETrue;
                // Request NET_SIM_NMR
                iSatMessHandler->NetNeighbourCellsReq(
                    iSatMessaging->GetTransactionId(),
                    NET_SIM_NMR );
                }

            break;
            }
        case RSat::KProvideLocalInfoDateTimeTimeZone:
            {
            TFLOGSTRING("CSatNotifyLocalInfo:: request: DATE/TIME/TIMEZONE");
            OstTrace0( TRACE_NORMAL, DUP8_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived Request: DATE/TIME/TIMEZONE" );
            TTime time;
            TLocale locale;
            TDateTime dateTime;
            // Get Universal time
            time.UniversalTime();
            // Get Universal time offset ( Time zone in seconds )
            TTimeIntervalSeconds universalTimeOffset(
                locale.UniversalTimeOffset() );
            // Add locale's universal time offset to universal time
            // to get the local time
            time += universalTimeOffset;
            // Converts a TTime object into a TDateTime object
            dateTime = time.DateTime();
            // Get and conver year
            TUint8 temp2( TSatUtility::ConvertToSemiOctet(
                dateTime.Year() ) );
            // Append year to additional info
            iLocalInfoRspV3.iAdditionalInfo.Append( temp2 );
            // Convert month
            // Offset from zero. So adding one.
            temp2 = TSatUtility::ConvertToSemiOctet(
                dateTime.Month() + 1 );
            // Append month value to the additional info
            iLocalInfoRspV3.iAdditionalInfo.Append( temp2 );
            // Convert day to the semi-octec presentation
            // The day.Offset from zero, so add one before displaying
            // the day number.
            temp2 = TSatUtility::ConvertToSemiOctet(
                dateTime.Day() + 1 );
            // Append the day value to the additional info
            iLocalInfoRspV3.iAdditionalInfo.Append( temp2 );
            // Conver current time to the semi-octec presentation.
            // First hour
            temp2 = TSatUtility::ConvertToSemiOctet(
                dateTime.Hour() );
            // Append the hour value to the additional info
            iLocalInfoRspV3.iAdditionalInfo.Append( temp2 );
            // Minute value
            temp2 = TSatUtility::ConvertToSemiOctet(
                dateTime.Minute() );
            // Append the minute value to the additional info
            iLocalInfoRspV3.iAdditionalInfo.Append( temp2 );
            // Seconds
            temp2 = TSatUtility::ConvertToSemiOctet(
                dateTime.Second() );
            // Append the seconds value to the additional info
            iLocalInfoRspV3.iAdditionalInfo.Append( temp2 );
            // Append time zone
            // received from NET_TIME_IND isi-message
            iLocalInfoRspV3.iAdditionalInfo.Append(
                iSatMessHandler->TimeZone() );
            // Set generall result as Success
            iLocalInfoRspV3.iGeneralResult = RSat::KSuccess;
            break;
            }
        case RSat::KProvideLocalInfoLanguage:
            {
            TFLOGSTRING("CSatNotifyLocalInfo:: request: Language");
            OstTrace0( TRACE_NORMAL, DUP9_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived Request: Language" );
            if( !iReqHandle )
                {
                TFLOGSTRING("CSatNotifyLocalInfo:: No reqHandle");
                OstTrace0( TRACE_NORMAL, DUP10_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived No reqHandle" );

                iLocalInfoRspV3.iGeneralResult = RSat::KMeUnableToProcessCmd;
                iLocalInfoRspV3.iAdditionalInfo.Append( RSat::KNoSpecificMeProblem );
                TerminalResponse();
                }
            else
                {
                TFLOGSTRING("CSatNotifyLocalInfo:: reqHandle");
                OstTrace0( TRACE_NORMAL, DUP11_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived reqHandle" );
                // Fill the send sm structure
                RSat::TLocalInfoV3& localInfoV3 = ( *iLocalInfoV3Pckg )();
                // Get command details tlv
                CTlv deviceIdentities;
                berTlv.TlvByTagValue( &deviceIdentities, KTlvDeviceIdentityTag );
                localInfoV3.iDevideId = ( RSat::TDeviceId )
                    deviceIdentities.GetShortInfo( ETLV_SourceDeviceIdentity );
                localInfoV3.iInfoType =
                   ( RSat::TLocaInfomationType) iCommandDetails[KCommandQualifier];
                localInfoV3.SetPCmdNumber(
                    commandDetails.GetShortInfo( ETLV_CommandNumber ) );
                CompleteRequest( KErrNone );
                }
            break;
            }
        case RSat::KProvideLocalInfoTimingAdv:
            {
            TFLOGSTRING("CSatNotifyLocalInfo:: request: TimingAdvance");
            OstTrace0( TRACE_NORMAL, DUP12_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived Request: TimingAdvance" );
            iLocalInfoIsOngoing = ETrue;
            //request GSS_CS_SERVICE_REQ
            iSatMessHandler->GssCsServiceReq(
                             iSatMessaging->GetTransactionId(),
                             GSS_ATK_TIMING_ADVANCE_GET );
            break;
            }
        case RSat::KProvideLocalInfoAccTech:
            {
            TFLOGSTRING("CSatNotifyLocalInfo:: request: Access Technology");
            OstTrace0( TRACE_NORMAL, DUP13_CSATNOTIFYLOCALINFO_MESSAGERECEIVED, "CSatNotifyLocalInfo::MessageReceived Request: Access Technology" );
            iLocalInfoIsOngoing = ETrue;
            //request NET_RAT_REQ
            iSatMessHandler->NetRatReq( iSatMessaging->GetTransactionId() );
            break;
            }
        // Currently not supported:
        case RSat::KProvideLocalInfoESN:
            {
            iLocalInfoRspV3.iGeneralResult =
                RSat::KCmdBeyondMeCapabilities;
            break;
            }
        default:
            {
            iLocalInfoRspV3.iGeneralResult = RSat::KCmdTypeNotUnderstood;
            }
        }
    if ( RSat::KProvideLocalInformationNmr !=
            iCommandDetails[KCommandQualifier]
        && RSat::KProvideLocalInfo != iCommandDetails[KCommandQualifier]
        && RSat::KProvideLocalInfoTimingAdv != iCommandDetails[KCommandQualifier]
        && RSat::KProvideLocalInfoLanguage != iCommandDetails[KCommandQualifier]
        && RSat::KProvideLocalInfoAccTech != iCommandDetails[KCommandQualifier] )
        {
        // Send terminal response to the sim
        TerminalResponse();
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a LocalInfo PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyLocalInfo::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYLOCALINFO_NOTIFY, "CSatNotifyLocalInfo::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifyLocalInfo::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iLocalInfoV3Pckg = static_cast< RSat::TLocalInfoV3Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KProvideLocalInformation );
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::TerminalResponse
// Handles a LocalInfo terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLocalInfo::TerminalResponse
        (
        TDes8* aRsp     // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLOCALINFO_TERMINALRESPONSE, "CSatNotifyLocalInfo::TerminalResponse" );
    TFLOGSTRING("CSatNotifyLocalInfo::TerminalResponse");
    TInt ret( KErrNone );
    if ( aRsp )
        {
        RSat::TLocalInfoRspV3Pckg* aRspPckg =
                reinterpret_cast< RSat::TLocalInfoRspV3Pckg* >( aRsp );
        RSat::TLocalInfoRspV3& rspV1 = ( *aRspPckg ) ();
        // Check that general result value is valid.
        if ( iAllowedResults != rspV1.iGeneralResult )
            {
            // Invalid general result
            ret = KErrCorrupt;
            }
        iLocalInfoRspV3.iGeneralResult = rspV1.iGeneralResult;
        // If there is Me (Mobile Entity) error, additional info is needed
        if ( RSat::KMeProblem == rspV1.iInfoType &&
             NULL == rspV1.iAdditionalInfo.Length() )
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        iLocalInfoRspV3.iAdditionalInfo = rspV1.iAdditionalInfo;
        }
    iSatMessHandler->LocalInfoTerminalResp(
        iTransId,                            // Transaction id
        iCommandDetails,                     // Command details tlv
        TUint8( iLocalInfoRspV3.iGeneralResult ),  // Result
        iLocalInfoRspV3.iAdditionalInfo );   // Additional info
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::Status
// Method to check and set local info status. This is used when
// SIM request NMR or Local Info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSatNotifyLocalInfo::Status
        (
        TBool aClearStatus
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLOCALINFO_STATUS, "CSatNotifyLocalInfo::Status" );
    TFLOGSTRING("CSatNotifyLocalInfo::Status");
    if ( aClearStatus )
        {
        iLocalInfoIsOngoing = EFalse;
        }
    return iLocalInfoIsOngoing;
    }


// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::CSatNotifyTimerMgmt
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyTimerMgmt::CSatNotifyTimerMgmt
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iCmdQualifier( KZero )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYTIMERMGMT_CSATNOTIFYTIMERMGMT, "CSatNotifyTimerMgmt::CSatNotifyTimerMgmt" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KContradictionWithTimerState
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::~CSatNotifyTimerMgmt
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyTimerMgmt::~CSatNotifyTimerMgmt()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYTIMERMGMT_CSATNOTIFYTIMERMGMT, "CSatNotifyTimerMgmt::~CSatNotifyTimerMgmt" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::MessageReceived
// Handles a TimerMgmt proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyTimerMgmt::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYTIMERMGMT_MESSAGERECEIVED, "CSatNotifyTimerMgmt::MessageReceived" );
    TFLOGSTRING("CSatNotifyTimerMgmt::MessageReceived");
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
    TUint8 generalResult ( RSat::KSuccess );
    //get ber tlv
    CBerTlv berTlv;
    returnValue = berTlv.BerTlv( aIsiMessage );
    //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    // Get command qualifier
    TUint8 cmdQualifier(
        commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
    // get timer id tlv
    CTlv timerId;
    returnValue = berTlv.TlvByTagValue( &timerId,
                           KTlvTimerIdentifierTag );
    // initalize
    TUint8 timerIdentifier( 0 );
    TUint8 time[3] = {0, 0, 0};
    if ( KErrNone == returnValue )
        {
        timerIdentifier = timerId.GetShortInfo( ETLV_TimerIdentifier );
        // The Timer Identifier can take 8 different values
        if ( ( RSat::KTimer8 < timerIdentifier  )
             || ( RSat::KTimer1 > timerIdentifier ) )
            {
            generalResult = RSat::KCmdDataNotUnderstood;
            }
        else
            {
            // if operation type is startTimer = 0x00
            if ( !cmdQualifier )
                {
                 //get timer value tlv
                CTlv timerValue;
                returnValue = berTlv.TlvByTagValue( &timerValue,
                                                KTlvTimerValueTag );
                if ( KErrNotFound != returnValue)
                    {
                    TPtrC8 temp;
                    TUint32 timeValue( 0 );
                    // value of a timer, expressed using
                    // the format hour, minute, second
                    // Semi-octec presentation used (23.040)
                    temp.Set( timerValue.GetData( ETLV_TimerValue ) );
                    //convert to seconds
                    timeValue = ConvertToSeconds( temp );
                    // The possible duration of a timer is
                    // between 1 second and 24 hours
                    if ( KMinTimerValue < timeValue &&
                         KMaxTimerValueInSeconds >= timeValue )
                        {
                        ret = iSatMessaging->GetSatTimer()->
                                   StartTimer( timerIdentifier, timeValue );
                        if ( KErrNone != ret )
                            {
                            generalResult = RSat::KMeUnableToProcessCmd;
                            }
                        }
                    else
                        {
                        generalResult = RSat::KCmdDataNotUnderstood;
                        }
                    }
                else
                    {
                    generalResult = RSat::KErrorRequiredValuesMissing;
                    }
                }
            // if operation type is KDeactivateTimer or KGetTimerValue
            else if ( KDeactivateTimer == cmdQualifier ||
                      KGetTimerValue == cmdQualifier )
                {
                TInt retValue( KErrNone );
                //get current value of the timer
                ret = iSatMessaging->GetSatTimer()->
                          GetCurrentValueOfTimerById( timerIdentifier );
                if ( KDeactivateTimer == cmdQualifier && KErrNotFound != ret )
                    {
                    //deactivate timer
                    retValue = iSatMessaging->GetSatTimer()->
                                      DeleteTimerById( timerIdentifier );
                    }
                //if timer is found convert timer value to semi-octets
                if ( KErrNotFound != ret && KErrNotFound != retValue )
                    {
                    generalResult = RSat::KSuccess;
                    TInt num[3];
                    num[0] = ret/3600;         //hours
                    num[1] = ( ret%3600 )/60;  //minutes
                    num[2] = ( ret%3600 )%60;  //seconds
                    TInt i( 0 );
                    for ( i = 0; i < 3; i++ )
                        {
                        //convert to semi-octet
                        time[i] = TSatUtility::ConvertToSemiOctet( num[i] );
                        }
                    }
                else
                    {
                    generalResult = RSat::KContradictionWithTimerState;
                    }
                }
            else
                {
                generalResult = RSat::KCmdTypeNotUnderstood;
                }
            }
        }
    else
        {
        generalResult = RSat::KErrorRequiredValuesMissing;
        }
    iSatMessHandler->TimerMgmtTerminalResp( aIsiMessage.Get8bit(
        ISI_HEADER_OFFSET_TRANSID ), iCommandDetails, generalResult,
        time, timerIdentifier, KNoCause );
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a TimerManagement PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyTimerMgmt::Notify
        (
        const TTsyReqHandle /*aReqHandle*/, // Request handle
        TDes8*              /*aDataPtr*/    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYTIMERMGMT_NOTIFY, "CSatNotifyTimerMgmt::Notify" );
    // TimerManagement is completely implemented by SimAtkTsy. Client
    // insn't notified about the command.
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::ConvertToSeconds
// Converts to seconds semi-octet presented value of timer,
// expressed using the format hour, minute, second
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CSatNotifyTimerMgmt::ConvertToSeconds
         (
         TPtrC8 time
         )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYTIMERMGMT_CONVERTTOSECONDS, "CSatNotifyTimerMgmt::ConvertToSeconds" );
    TFLOGSTRING("CSatNotifyTimerMgmt::ConvertToSeconds");
     TUint32 timeValue( 0 );
    // value of a timer, expressed using
    // the format hour, minute, second
    // Semi-octec presentation used (23.040)
    // time[0] is hours
    // time[1] is minutes
    // time[2] is seconds
     TInt i( 0 );
    for ( i = 0; i < 3; i++ )
        {
        TUint value = ( time[i] & 0x0F )*10 + ( ( time[i] & 0xF0 ) >> 4 );
        timeValue = timeValue * 60 + value;
        }
    return timeValue;

    }


// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::CSatNotifyMoreTime
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyMoreTime::CSatNotifyMoreTime
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYMORETIME_CSATNOTIFYMORETIME, "CSatNotifyMoreTime::CSatNotifyMoreTime" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::~CSatNotifyMoreTime
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyMoreTime::~CSatNotifyMoreTime()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYMORETIME_CSATNOTIFYMORETIME, "CSatNotifyMoreTime::~CSatNotifyMoreTime" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::MessageReceived
// Handles a MoreTime proactive command ISI message,
// Etel SAT request not required.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyMoreTime::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYMORETIME_MESSAGERECEIVED, "CSatNotifyMoreTime::MessageReceived" );
    TFLOGSTRING("CSatNotifyMoreTime::MessageReceived");
    //get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    //get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iSatMessHandler->MoreTimeTerminalResp(
                        aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ),
                        iCommandDetails,
                        RSat::KSuccess );
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a MoreTime PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyMoreTime::Notify
        (
        const TTsyReqHandle /*aReqHandle*/, // Request handle
        TDes8*              /*aDataPtr*/    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYMORETIME_NOTIFY, "CSatNotifyMoreTime::Notify" );
    // MoreTime is completely implemented by SimAtkTsy. Client
    // insn't notified about the command.
    }


// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::CSatNotifyLanguageNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyLanguageNotification::CSatNotifyLanguageNotification
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iLanguageNotificationV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLANGUAGENOTIFICATION_CSATNOTIFYLANGUAGENOTIFICATION, "CSatNotifyLanguageNotification::CSatNotifyLanguageNotification" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::~CSatNotifyLanguageNotification
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyLanguageNotification::~CSatNotifyLanguageNotification()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYLANGUAGENOTIFICATION_CSATNOTIFYLANGUAGENOTIFICATION, "CSatNotifyLanguageNotification::~CSatNotifyLanguageNotification" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::MessageReceived
// Handles a LanguageNotification proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyLanguageNotification::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLANGUAGENOTIFICATION_MESSAGERECEIVED, "CSatNotifyLanguageNotification::MessageReceived" );
    TFLOGSTRING("TSY: CSatNotifyLanguageNotification::MessageReceived");
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    returnValue = berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if( !iReqHandle )
        {
        iSatMessHandler->LanguageNotificationTerminalResp(
                            iTransId,
                            iCommandDetails,
                            RSat::KMeUnableToProcessCmd,
                            KNoCause ); // KNoCause: 0
        }
    else
        {

        RSat::TLanguageNotificationV2& languageNotificationV2 =
                                    ( *iLanguageNotificationV2Pckg )();
        // Command number
        languageNotificationV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Initialisate
        languageNotificationV2.iNotificationType = RSat::ENotificationTypeNotSet;
        // Get command qualifier
        TUint8 cmdQualifier(
            commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
        if ( !cmdQualifier )
           {
            // No specific language used
            languageNotificationV2.iNotificationType =
                RSat::ENonSpecificLangNotification;
            }
        else if ( KSpecificLanguage == cmdQualifier )
            {
            // Currently used language
            languageNotificationV2.iNotificationType =
                RSat::ESpecificLangNotification;
            }
        else
            {
            TFLOGSTRING("TSY: \
            CSatNotifyLanguageNotification::MessageReceived, \
            Command qualifier did not match.");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYLANGUAGENOTIFICATION_MESSAGERECEIVED, "CSatNotifyLanguageNotification::MessageReceived, Command qualifier did not match." );
            }
        CTlv language;
        returnValue = berTlv.TlvByTagValue( &language,
            KTlvLanguageTag );
        // If the Language tag is found
        if ( KErrNone == returnValue )
            {
            languageNotificationV2.iLanguage = language.GetLongInfo( ETLV_Language );
            }
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a LanguageNotification PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyLanguageNotification::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLANGUAGENOTIFICATION_NOTIFY, "CSatNotifyLanguageNotification::Notify" );
    TFLOGSTRING("CSatNotifyLanguageNotification::Notify");
    iReqHandle = aReqHandle;
    iLanguageNotificationV2Pckg =
        static_cast< RSat::TLanguageNotificationV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KLanguageNotification );
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::TerminalResponse
// Handles a LanguageNotification terminal response provided by an ETel SAT
// client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLanguageNotification::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYLANGUAGENOTIFICATION_TERMINALRESPONSE, "CSatNotifyLanguageNotification::TerminalResponse" );
    TFLOGSTRING("CSatNotifyLanguageNotification::TerminalResponse");
    TUint8 additionalInfo( 0 );
    RSat::TLanguageNotificationRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TLanguageNotificationRspV2Pckg* >( aRsp );
    RSat::TLanguageNotificationRspV2& rspV1 = ( *aRspPckg ) ();
    iSatMessHandler->LanguageNotificationTerminalResp(
                            iTransId,
                            iCommandDetails,
                            rspV1.iGeneralResult,
                            additionalInfo );

    return iAllowedResults == rspV1.iGeneralResult ? KErrNone : KErrCorrupt;
    }


// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::CSatNotifyOpenChannel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyOpenChannel::CSatNotifyOpenChannel
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iOpenBaseChannelV2Pckg( NULL ),
        iOpenCsChannelV2Pckg( NULL ),
        iOpenGprsChannelV4Pckg( NULL ),
        iOpenLocalLinksChannelV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_CSATNOTIFYOPENCHANNEL, "CSatNotifyOpenChannel::CSatNotifyOpenChannel" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPerformedWithModifications+ RSat::KPSessionTerminatedByUser
        + RSat::KNetworkUnableToProcessCmd + RSat::KPCmdNotAcceptedByUser
        + RSat::KInteractionWithCCTemporaryError
        + RSat::KErrorRequiredValuesMissing + RSat::KBearerIndepProtocolError
        + RSat::KAccessTechUnableProcessCmd;
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::~CSatNotifyOpenChannel
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyOpenChannel::~CSatNotifyOpenChannel()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYOPENCHANNEL_CSATNOTIFYOPENCHANNEL, "CSatNotifyOpenChannel::~CSatNotifyOpenChannel" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::MessageReceived
// Handles a OpenChannel proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyOpenChannel::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage // ISI message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived" );
    TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived");
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived - Request Off");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived - Request Off" );
        // Request not on, returning response immediately
        TBuf8<1> noBearer( 0 );
        TBuf8<1> noAdditionalInfo( 0 );
        TUint16 noBufferSize( 0 );
        iSatMessHandler->OpenChannelTerminalResp(
             iTransId,
             iCommandDetails,
             RSat::KMeUnableToProcessCmd,
             noAdditionalInfo,
             TUint8( RSat::EBearerTypeNotSet ),
             noBearer,
             noBufferSize );
        ret = KErrCorrupt;
        }
    else
        {
        RSat::TOpenChannelBaseV2& openChannelV2
            = ( *iOpenBaseChannelV2Pckg ) ();

        // Bearer Description & Buffer Size ( Mandatory )
        CTlv bearerDescriptionTlv;
        CTlv bufferSizeTlv;

        if ( KErrNone == berTlv.TlvByTagValue( &bearerDescriptionTlv,
             KTlvBearerDescriptionTag )
            && KErrNone == berTlv.TlvByTagValue( &bufferSizeTlv,
             KTlvBufferSizeTag ) )
            {
            // Command details
            openChannelV2.SetPCmdNumber(
                commandDetails.GetShortInfo( ETLV_CommandNumber ) );

            // Link Establishment, obtained
            // from Command details Bit 1
            // Get command qualifier
            TUint8 cmdQualifier( commandDetails.GetShortInfo(
                ETLV_CommandQualifier ) );
            if ( KLinkEstablishmentMask & cmdQualifier )
                {
                openChannelV2.iLinkEst = RSat::EImmediate;
                }
            else
                {
                openChannelV2.iLinkEst = RSat::EOnDemand;
                }

            // Reconnection mode, obtained from Command
            // details Bit 2
            if ( KReconnectionModeMask & cmdQualifier )
                {
                openChannelV2.iReconnectionMode =
                    RSat::EAutomaticReconnection;
                }
            else
                {
                openChannelV2.iReconnectionMode =
                    RSat::ENoAutomaticReconnection;
                }

            // Buffer size
            openChannelV2.iBufferSize = bufferSizeTlv.GetLongInfo(
                ETLV_BufferSize );

            // Handling of
            // Bearer type
            // Bearer parameters
            GetBearerTypeAndParams( bearerDescriptionTlv,
                openChannelV2.iBearer );

            // Alpha Identifier ( Optional )
            TPtrC8 sourceString; // Used with conversions
            CTlv alphaIdentifierTlv;
            // Get Alpha Identifier tlv from berTlv
            TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifierTlv,
                KTlvAlphaIdentifierTag ) );
            // If Alpha Identifier string exist
            if ( KErrNone == returnValue )
                {
                if ( alphaIdentifierTlv.GetLength() )
                    {
                    // 8-bit string to 16-bit string
                    sourceString.Set(
                        alphaIdentifierTlv.GetData( ETLV_AlphaIdentifier ) );
                    // Convert and set the Alpha Identifier
                    TSatUtility::SetAlphaId(
                        sourceString, openChannelV2.iAlphaId.iAlphaId );
                    // Set Alpha Identifier status
                    openChannelV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    }
                else
                    {
                    openChannelV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
                    }
                }
            // Alpha Identifier not present
            else
                {
                openChannelV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
                }

            // Icon Identifier ( optional )
            TSatUtility::FillIconStructure( berTlv,
                openChannelV2.iIconId );

            // Handling of
            // SIM/ME interface transport level ( Optional )
            // Other Address - Set Destination Address
            TInt retValueLocalAdd = SimMeInterfaceAndDataDestinationAddress(
                berTlv,
                openChannelV2.iSimMeInterface,
                openChannelV2.iDestinationAddress );

            // Switch according to bearer type
            //
            switch ( openChannelV2.iBearer.iType )
                {
                case RSat::EGPRSBearer:
                    {
                    TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived - GPRS Bearer");
                    OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived - GPRS Bearer" );
                    // Fill in TOpenGprsChannelV4
                    RSat::TOpenGprsChannelV4& openGprsChannelV4
                        = ( *iOpenGprsChannelV4Pckg )();
                    openGprsChannelV4
                        = static_cast<RSat::TOpenGprsChannelV4&>( openChannelV2 );
                    openGprsChannelV4.iPCmdType = RSat::EGprsBearer;

                    // Handling of
                    // Other Address - Local Address ( Optional )
                    if ( KErrNone == retValueLocalAdd )
                        {
                        LocalAddress( berTlv, openGprsChannelV4.iLocalAddress );
                        }
                    else
                        {
                        TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived -\
                            Local Adress - not present");
                        OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived - Local Adress - not present" );
                        openGprsChannelV4.iLocalAddress.iType =
                            RSat::EAddressNotPresent;
                        }

                    // Handling GPRS bearer specific data
                    //
                    // Network Access Point Name ( Optional )
                    // Text string - User Login ( Optional )
                    // Text string - User Password ( Optional )
                    GprsBearerSpecific( berTlv, openGprsChannelV4 );
                    break;
                    }
                case RSat::EDefaultBearer:
                    {
                    TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived - Default Bearer");
                    OstTrace0( TRACE_NORMAL, DUP4_CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived - Default Bearer" );
                    // Fill in TOpenChannelBaseV2
                    openChannelV2.iPCmdType = RSat::EAnyBearer;
                    break;
                    }
                case RSat::ECSDBearer:
                case RSat::EBTBearer:
                case RSat::EIrDaBearer:
                case RSat::ERS232Bearer:
                case RSat::EUSBBearer:
                    {
                    TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived - Bearer not supported");
                    OstTrace0( TRACE_NORMAL, DUP5_CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived - Bearer not supported" );
                    // Bearer not supported
                    TBuf8<1> noAdditionalInfo( 0 );
                    iSatMessHandler->OpenChannelTerminalResp(
                         iTransId,
                         iCommandDetails,
                         RSat::KCmdBeyondMeCapabilities,
                         noAdditionalInfo,
                         openChannelV2.iBearer.iType,
                         openChannelV2.iBearer.iParams,
                         openChannelV2.iBufferSize );
                    ret = KErrCorrupt;
                    break;
                    }
                default:
                    {
                    // Bearer not supported (RFU)
                    TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived - Bearer not supported");
                    OstTrace0( TRACE_NORMAL, DUP6_CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived - Bearer not supported" );
                    // Required values missing
                    TBuf8<1> noBearer( 0 );
                    TBuf8<1> noAdditionalInfo( 0 );
                    TUint16 noBufferSize( 0 );
                    iSatMessHandler->OpenChannelTerminalResp(
                         iTransId,
                         iCommandDetails,
                         RSat::KCmdDataNotUnderstood,
                         noAdditionalInfo,
                         TUint8( RSat::EBearerTypeNotSet ),
                         noBearer,
                         noBufferSize );
                    ret = KErrCorrupt;
                    break;
                    }
                } // End of switch according to bearer type
            }
        else
            {
            TFLOGSTRING("TSY: CSatNotifyOpenChannel::MessageReceived - Required values missing");
            OstTrace0( TRACE_NORMAL, DUP7_CSATNOTIFYOPENCHANNEL_MESSAGERECEIVED, "CSatNotifyOpenChannel::MessageReceived - Required values missing" );
            // Required values missing
            TBuf8<1> noBearer( 0 );
            TBuf8<1> noAdditionalInfo( 0 );
            TUint16 noBufferSize( 0 );
            iSatMessHandler->OpenChannelTerminalResp(
                 iTransId,
                 iCommandDetails,
                 RSat::KErrorRequiredValuesMissing,
                 noAdditionalInfo,
                 TUint8( RSat::EBearerTypeNotSet ),
                 noBearer,
                 noBufferSize );
            ret = KErrCorrupt;
            } // End of Bearer Description & Buffer Size ( Mandatory )

        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a OpenChannel PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyOpenChannel::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_NOTIFY, "CSatNotifyOpenChannel::Notify" );
    TFLOGSTRING("CSatNotifyOpenChannel::Notify");
    iReqHandle = aReqHandle;
    iOpenBaseChannelV2Pckg =
        static_cast< RSat::TOpenChannelBaseV2Pckg* >( aDataPtr );
    iOpenCsChannelV2Pckg =
        static_cast< RSat::TOpenCsChannelV2Pckg* >( aDataPtr );
    iOpenGprsChannelV4Pckg =
        static_cast< RSat::TOpenGprsChannelV4Pckg* >( aDataPtr );
    iOpenLocalLinksChannelV2Pckg =
        static_cast< RSat::TOpenLocalLinksChannelV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KOpenChannel );
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::TerminalResponse
// Handles a OpenChannel terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyOpenChannel::TerminalResponse
        (
        TDes8* aRsp // Response
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_TERMINALRESPONSE, "CSatNotifyOpenChannel::TerminalResponse" );
    TFLOGSTRING("CSatNotifyOpenChannel::TerminalResponse");
    TInt   ret( KErrNone );
    TBuf8<RSat::KAdditionalInfoMaxSize> additionalInfo;
    additionalInfo.Zero();
    RSat::TOpenChannelRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TOpenChannelRspV2Pckg* >( aRsp );
    RSat::TOpenChannelRspV2& rspV2 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    // Check fo additional info type
    // - If there is Me (Mobile Equipment) error, additional info is needed
    // - Channel Status Info is sent through additional info
    if ( ( RSat::KMeProblem == rspV2.iInfoType )
        || ( RSat::KChannelStatusInfo == rspV2.iInfoType )
        )
        {
        // Check the length of additional info
        if ( !rspV2.iAdditionalInfo.Length() )
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        else
            {
            TIsiUtility::CopyToBigEndian( rspV2.iAdditionalInfo, additionalInfo );
            }
        }
    // Send Terminal Response
    iSatMessHandler->OpenChannelTerminalResp(
        iTransId,
        iCommandDetails,
        TUint8( rspV2.iGeneralResult ),
        additionalInfo,
        TUint8( rspV2.iBearer.iType ),
        rspV2.iBearer.iParams,
        rspV2.iBufferSize);
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::GetBearerTypeAndParams
// Handles a OpenChannel Bearer type and parameters specific data.
// -----------------------------------------------------------------------------
//
void CSatNotifyOpenChannel::GetBearerTypeAndParams
        (
        CTlv& aBearerDescriptionTlv,
        RSat::TBearer& aBearer
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_GETBEARERTYPEANDPARAMS, "CSatNotifyOpenChannel::GetBearerTypeAndParams" );
    TFLOGSTRING("TSY: CSatNotifyOpenChannel::GetBearerTypeAndParams");
    TUint8 bearerType( aBearerDescriptionTlv.GetShortInfo( ETLV_BearerType ) );

    // Switch for setting bearer type values
    switch( bearerType )
        {
        case KBipCsdBearer:
            {
            aBearer.iType = RSat::ECSDBearer;
            break;
            }
        case KBipGprsBearer:
            {
            aBearer.iType = RSat::EGPRSBearer;
            break;
            }
        case KBipDefaultBearer:
            {
            aBearer.iType = RSat::EDefaultBearer;
            break;
            }
        case KBipBluetoothBearer:
            {
            aBearer.iType = RSat::EBTBearer;
            break;
            }
        case KBipIrDABearer:
            {
            aBearer.iType = RSat::EIrDaBearer;
            break;
            }
        case KBipRS232Bearer:
            {
            aBearer.iType = RSat::ERS232Bearer;
            break;
            }
        case KBipUSBBearer:
            {
            aBearer.iType = RSat::EUSBBearer;
            break;
            }
        // Currently Local link technology independent bearer type is missing from
        // Etel SAT API ( EtelSat.h)
        case KBipLocalLinkTechnologyIndependentBearer:
        default:
            {
            aBearer.iType = RSat::EBearerTypeNotSet;
            break;
            }
        }

    // Bearer parameters
    aBearer.iParams = aBearerDescriptionTlv.GetData( ETLV_BearerParameters );
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress
// Handles a OpenChannel SIM/ME Interface transport level and Data Destination
// Address specific data.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress
        (
        CBerTlv& aBerTlv,
        RSat::TSimMeInterface& aSimMeInterface,
        RSat::TOtherAddress& aDestinationAddress
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_SIMMEINTERFACEANDDATADESTINATIONADDRESS, "CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress" );
    TFLOGSTRING("TSY: CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress");

    // SIM/ME Interface transport level ( Optional )
    CTlv simMeInterfaceTlv;

    if ( KErrNone == aBerTlv.TlvByTagValue( &simMeInterfaceTlv,
        KTlvSimMeTransportLevelTag ) )
        {
        TFLOGSTRING("CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress -\
            SIM/ME Interface");
        OstTrace0( TRACE_NORMAL, DUP5_CSATNOTIFYOPENCHANNEL_SIMMEINTERFACEANDDATADESTINATIONADDRESS, "CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress - SIM/ME Interface" );

        aSimMeInterface.iPrtNumber = simMeInterfaceTlv.GetLongInfo(
            ETLV_PortNumber );
        TUint8 protocol( simMeInterfaceTlv.GetShortInfo(
            ETLV_TransportProtocolType ) );

        if ( KProtocolUdp == protocol )
            {
            aSimMeInterface.iTransportProto = RSat::EUdp;
            }
        else if ( KProtocolTcp == protocol )
            {
            aSimMeInterface.iTransportProto = RSat::ETcp;
            }
        else
            {
            aSimMeInterface.iTransportProto = RSat::EProtocolNotSet;
            }
        }
    else
        {
        TFLOGSTRING("TSY: CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress -\
            SIM/ME Interface - Not present");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYOPENCHANNEL_SIMMEINTERFACEANDDATADESTINATIONADDRESS, "CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress -SIM/ME Interface - Not present" );
        aSimMeInterface.iTransportProto = RSat::EProtocolNotPresent;
        }

    // Data destination address
    // The Data Destination Address is the end point destination
    // address of sent data. This data destination address is
    // requested when a SIM/ME Interface transport is present,
    // otherwise it is ignored.

    // Determine the number of Other Address tags.
    // - If there is one and no SIM/ME Interface then this is a Local Address.
    // - If there is one and SIM/ME Interface, this is a Destination Address.
    // - If there are two and SIM/ME Interface, then the last one is
    //   the Destination Address.
    CTlv* dataDestAddressTlv = NULL;
    CTlv localAddress1;
    CTlv localAddress2;

    TInt retValueLocalAdd( KErrNotFound );
    TInt retValAddr1( aBerTlv.TlvByTagValue( &localAddress1,
        KTlvOtherAddressTag ) );
    TInt retValAddr2( aBerTlv.TlvByTagValue( &localAddress2,
        KTlvOtherAddressTag, 1 ) );

    if ( ( RSat::EProtocolNotPresent !=
           aSimMeInterface.iTransportProto )
         && ( KErrNone == retValAddr1 )
         && ( KErrNotFound == retValAddr2 ) )
        {
        // Destination Address is localAddress1
        dataDestAddressTlv = &localAddress1;
        // There can not be a Local Address
        }
    else if ( ( RSat::EProtocolNotPresent !=
           aSimMeInterface.iTransportProto )
         && ( KErrNone == retValAddr1 )
         && ( KErrNone == retValAddr2 ) )
        {
        // Local Address is localAddress1
        retValueLocalAdd = retValAddr1;
        // Destination Address is localAddress2
        dataDestAddressTlv = &localAddress2;
        }
    else if ( ( RSat::EProtocolNotPresent ==
           aSimMeInterface.iTransportProto )
         && ( KErrNone == retValAddr1 ) )
        {
        // There is only a Local Address
        // Local Address is localAddress1
        retValueLocalAdd = retValAddr1;
        }
    else
        {
        TFLOGSTRING("TSY: CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress,\
            Transport protocol did not match.");
        OstTrace0( TRACE_NORMAL, DUP2_CSATNOTIFYOPENCHANNEL_SIMMEINTERFACEANDDATADESTINATIONADDRESS, "CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress, Transport protocol did not match." );
        }

    // Other Address - Set Destination Address
    if( dataDestAddressTlv )
        {
        TFLOGSTRING("TSY: CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress -\
            Destination Address");
        OstTrace0( TRACE_NORMAL, DUP3_CSATNOTIFYOPENCHANNEL_SIMMEINTERFACEANDDATADESTINATIONADDRESS, "CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress - Destination Address" );
        TUint8 typeOfAddress( dataDestAddressTlv->GetShortInfo(
            ETLV_TypeOfAddress ) );
        switch( typeOfAddress )
            {
            case KIPV4: // IPv4
                {
                aDestinationAddress.iType = RSat::EIPv4Address;
                break;
                }
            case KIPV6: // IPv6
                {
                aDestinationAddress.iType = RSat::EIPv6Address;
                break;
                }
            default:
                {
                aDestinationAddress.iType = RSat::EAddressNotSet;
                break;
                }
            }
        // Is there an address
        if( dataDestAddressTlv->GetLength() - 1 )
            {
            aDestinationAddress.iAddress =
                dataDestAddressTlv->GetData( ETLV_Address );
            }
        }
    else
        {
        TFLOGSTRING("TSY: CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress -\
            Destination Address - Not present");
        OstTrace0( TRACE_NORMAL, DUP4_CSATNOTIFYOPENCHANNEL_SIMMEINTERFACEANDDATADESTINATIONADDRESS, "CSatNotifyOpenChannel::SimMeInterfaceAndDataDestinationAddress - Destination Address - Not present" );
        aDestinationAddress.iType = RSat::EAddressNotPresent;
        } // End of Other Address - Set Destination Address

    return retValueLocalAdd;
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::LocalAddress
// Handles a OpenChannel Local Address specific data.
// -----------------------------------------------------------------------------
//
void CSatNotifyOpenChannel::LocalAddress
        (
        CBerTlv& aBerTlv,
        RSat::TOtherAddress& aLocalAddress
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_LOCALADDRESS, "CSatNotifyOpenChannel::LocalAddress" );
    TFLOGSTRING("TSY: CSatNotifyOpenChannel::LocalAddress");

    // Other Address - Set Local Address ( Optional )
    CTlv localAddressTlv;

    if ( KErrNone == aBerTlv.TlvByTagValue( &localAddressTlv,
        KTlvOtherAddressTag ) )
        {
        // If Other local address present
        if ( localAddressTlv.GetLength() )
            {
            aLocalAddress.iAddress = localAddressTlv.GetData( ETLV_Address );
            // Type of address
            TUint8 typeOfAddress( localAddressTlv.GetShortInfo(
                ETLV_TypeOfAddress ) );

            if ( KIPV4 == typeOfAddress )
                {
                aLocalAddress.iType = RSat::EIPv4Address;
                }
            else if ( KIPV6 == typeOfAddress )
                {
                aLocalAddress.iType = RSat::EIPv6Address;
                }
            else
                {
                aLocalAddress.iType = RSat::EAddressNotSet;
                }
            }
        else
            {
            TFLOGSTRING("TSY: CSatNotifyOpenChannel::LocalAddress - not present");
            OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYOPENCHANNEL_LOCALADDRESS, "CSatNotifyOpenChannel::LocalAddress - not present" );
            aLocalAddress.iType = RSat::EAddressNotPresent;
            } // End of If Other local address present
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::GprsBearerSpecific
// Handles a OpenChannel GPRS bearer specific data.
// -----------------------------------------------------------------------------
//
void CSatNotifyOpenChannel::GprsBearerSpecific
        (
        CBerTlv& aBerTlv,
        RSat::TOpenGprsChannelV4& aOpenGprsChannelV4
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYOPENCHANNEL_GPRSBEARERSPECIFIC, "CSatNotifyOpenChannel::GprsBearerSpecific" );
    TFLOGSTRING("TSY: CSatNotifyOpenChannel::GprsBearerSpecific");

    // Network Access Point Name ( Optional )
    CTlv nanTlv;
    // If Network Access Point Name present
    if ( KErrNone == aBerTlv.TlvByTagValue( &nanTlv,
        KTlvNetworkAccessNameTag ) )
        {
        TFLOGSTRING("TSY: CSatNotifyOpenChannel::GprsBearerSpecific -\
            Access Point name");
        OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYOPENCHANNEL_GPRSBEARERSPECIFIC, "CSatNotifyOpenChannel::GprsBearerSpecific - Access Point name" );
        aOpenGprsChannelV4.iAccessName =
            nanTlv.GetData( ETLV_NetworkAccessName );
        }

    // Text string - User Login ( Optional )
    // Text string - User Password ( Optional )
    TSatUtility::SetUserLoginAndUserPassword( aBerTlv,
        aOpenGprsChannelV4.iUserLogin,
        aOpenGprsChannelV4.iUserPassword );
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::CSatNotifyGetChannelStatus
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyGetChannelStatus::CSatNotifyGetChannelStatus
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iGetChannelStatusRspV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETCHANNELSTATUS_CSATNOTIFYGETCHANNELSTATUS, "CSatNotifyGetChannelStatus::CSatNotifyGetChannelStatus" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KPSessionTerminatedByUser
        + RSat::KErrorRequiredValuesMissing;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::~CSatNotifyGetChannelStatus
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyGetChannelStatus::~CSatNotifyGetChannelStatus()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYGETCHANNELSTATUS_CSATNOTIFYGETCHANNELSTATUS, "CSatNotifyGetChannelStatus::~CSatNotifyGetChannelStatus" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::MessageReceived
// Handles a GetChannelStatus proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyGetChannelStatus::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage    // Received isi-message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETCHANNELSTATUS_MESSAGERECEIVED, "CSatNotifyGetChannelStatus::MessageReceived" );
    TFLOGSTRING("CSatNotifyGetChannelStatus::MessageReceived");
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details TLV
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        TBuf8<1> empty(0);
        iSatMessHandler->GetChannelStatusTerminalResp( iTransId,
                                                       iCommandDetails,
                                                       RSat::KMeUnableToProcessCmd,
                                                       empty );
        }
    else
        {
        // Fill the Get Channel status structure
        RSat::TGetChannelStatusV2& channelStatusV2 =
            ( *iGetChannelStatusRspV2Pckg )();
        // Store transaction ID
        channelStatusV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Complete request
        CompleteRequest( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a GetChannelStatus PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyGetChannelStatus::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYGETCHANNELSTATUS_NOTIFY, "CSatNotifyGetChannelStatus::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifyGetChannelStatus::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iGetChannelStatusRspV2Pckg =
        static_cast< RSat::TGetChannelStatusV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KGetChannelStatus );
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::TerminalResponse
// Handles a GetChannelStatus terminal response provided by an ETel SAT client
// via ETel server,
// transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetChannelStatus::TerminalResponse
        (
        TDes8* aRsp     // Response package
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYGETCHANNELSTATUS_TERMINALRESPONSE, "CSatNotifyGetChannelStatus::TerminalResponse" );
    TFLOGSTRING("CSatNotifyGetChannelStatus::TerminalResponse");
    TInt   ret( KErrNone );
    TBuf8<RSat::KAdditionalInfoMaxSize> additionalInfo;
    additionalInfo.Zero();
    RSat::TGetChannelStatusRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TGetChannelStatusRspV2Pckg* >( aRsp );
    RSat::TGetChannelStatusRspV2& rspV2 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    if ( ( RSat::KChannelStatusInfo == rspV2.iInfoType )
        || ( RSat::KMeProblem == rspV2.iInfoType ) )
        {
        if( !rspV2.iAdditionalInfo.Length()
            && rspV2.iAdditionalInfo.Length())
            {
            ret = KErrCorrupt;
            }
        else
            {
            TIsiUtility::CopyToBigEndian( rspV2.iAdditionalInfo, additionalInfo );
            }
        }
    iSatMessHandler->GetChannelStatusTerminalResp(
        iTransId,
        iCommandDetails,
        TUint8( rspV2.iGeneralResult ),
        additionalInfo );
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::CSatNotifyCloseChannel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyCloseChannel::CSatNotifyCloseChannel
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iCloseChannelRspV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCLOSECHANNEL_CSATNOTIFYCLOSECHANNEL, "CSatNotifyCloseChannel::CSatNotifyCloseChannel" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KErrorRequiredValuesMissing
        + RSat::KBearerIndepProtocolError;
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::~CSatNotifyCloseChannel
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyCloseChannel::~CSatNotifyCloseChannel()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYCLOSECHANNEL_CSATNOTIFYCLOSECHANNEL, "CSatNotifyCloseChannel::~CSatNotifyCloseChannel" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::MessageReceived
// Handles a CloseChannel proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyCloseChannel::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage    // Received isi-message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCLOSECHANNEL_MESSAGERECEIVED, "CSatNotifyCloseChannel::MessageReceived" );
    TFLOGSTRING("CSatNotifyCloseChannel::MessageReceived");
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details TLV
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        TBuf8<1> empty(0);
        iSatMessHandler->CloseChannelTerminalResp( iTransId,
                                                    iCommandDetails,
                                                    RSat::KMeUnableToProcessCmd,
                                                    empty );
        }
    else
        {
        // Fill the Close Channel structure
        RSat::TCloseChannelV2& closeChannelV2 =
            ( *iCloseChannelRspV2Pckg )();
        // Store transaction ID
        closeChannelV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Store Channel Id
        CTlv deviceIds;
        TInt returnValue( berTlv.TlvByTagValue( &deviceIds,
            KTlvDeviceIdentityTag ) );
        if ( KErrNotFound != returnValue )
            {
            closeChannelV2.iDestination = (RSat::TDeviceId) deviceIds.GetShortInfo(
                ETLV_DestinationDeviceIdentity );
            }
        // Alpha Id (Optional)
        CTlv alphaIdentifier;
        returnValue = berTlv.TlvByTagValue( &alphaIdentifier,
            KTlvAlphaIdentifierTag ) ;
        closeChannelV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
        if ( KErrNotFound != returnValue )
            {
            TUint16 alphaIdLength = alphaIdentifier.GetLength();
            if ( alphaIdLength )
                {
                // get the alpha id
                TPtrC8 sourceString;
                sourceString.Set(
                    alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
                // convert and set the alpha id
                TSatUtility::SetAlphaId( sourceString ,
                    closeChannelV2.iAlphaId.iAlphaId );
                }

            // Set Alpha ID status
            if ( closeChannelV2.iAlphaId.iAlphaId.Length() )
                {
                closeChannelV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                }
            else
                {
                closeChannelV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
                }
            }
        // Icon Id (Optional)
        TSatUtility::FillIconStructure( berTlv,
            closeChannelV2.iIconId );
        // Complete request
        CompleteRequest( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a CloseChannel PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyCloseChannel::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYCLOSECHANNEL_NOTIFY, "CSatNotifyCloseChannel::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifyCloseChannel::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iCloseChannelRspV2Pckg =
        static_cast< RSat::TCloseChannelV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KCloseChannel );
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::TerminalResponse
// Handles a CloseChannel terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyCloseChannel::TerminalResponse
        (
        TDes8* aRsp     // Response package
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYCLOSECHANNEL_TERMINALRESPONSE, "CSatNotifyCloseChannel::TerminalResponse" );
    TFLOGSTRING("CSatNotifyCloseChannel::TerminalResponse");
    TInt   ret( KErrNone );
    TBuf8<RSat::KAdditionalInfoMaxSize> additionalInfo;
    additionalInfo.Zero();
    RSat::TCloseChannelRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TCloseChannelRspV2Pckg* >( aRsp );
    RSat::TCloseChannelRspV2& rspV2 = ( *aRspPckg ) ();

    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    if ( RSat::KMeProblem == rspV2.iInfoType )
        {
        if( rspV2.iAdditionalInfo.Length() )
            {
            additionalInfo.Append( TUint8( rspV2.iAdditionalInfo[0] ) );
            }
        else
            {
            ret = KErrCorrupt;
            }
        }
    iSatMessHandler->CloseChannelTerminalResp(
        iTransId,
        iCommandDetails,
        TUint8( rspV2.iGeneralResult ),
        additionalInfo );
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifySendData::CSatNotifySendData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifySendData::CSatNotifySendData
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iSendDataRspV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDDATA_CSATNOTIFYSENDDATA, "CSatNotifySendData::CSatNotifySendData" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KNetworkUnableToProcessCmd
        + RSat::KErrorRequiredValuesMissing + RSat::KBearerIndepProtocolError;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::~CSatNotifySendData
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifySendData::~CSatNotifySendData()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYSENDDATA_CSATNOTIFYSENDDATA, "CSatNotifySendData::~CSatNotifySendData" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::MessageReceived
// Handles a SendData proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendData::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage    // Received isi-message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDDATA_MESSAGERECEIVED, "CSatNotifySendData::MessageReceived" );
    TFLOGSTRING("CSatNotifySendData::MessageReceived");
    // return value for completion of the request
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details TLV
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        TBuf8<1> empty(0);
        TUint8 channelDataLength( 0 );
        iSatMessHandler->SendDataTerminalResp(
           iTransId,
           iCommandDetails,
           RSat::KMeUnableToProcessCmd,
           empty,
           channelDataLength );
        }
    else
        {
        // Fill the Send Data structure
        RSat::TSendDataV2& sendDataV2 =
            ( *iSendDataRspV2Pckg )();
        // Store transaction ID
        sendDataV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Store Channel Id
        CTlv deviceIds;
        if ( KErrNotFound != berTlv.TlvByTagValue( &deviceIds,
            KTlvDeviceIdentityTag ) )
            {
            sendDataV2.iDestination = (RSat::TDeviceId) deviceIds.GetShortInfo(
                ETLV_DestinationDeviceIdentity );
            }
        // Channel data (Mandatory)
        // When the length is greater than 127, the byte 0x81 is appended before
        // the actual length.
        CTlv channelData;
        if ( KErrNone == berTlv.TlvByTagValue( &channelData,
             KTlvChannelDataTag ) )
            {
            // Set channel data
            sendDataV2.iChannelData.Copy(
                channelData.GetData( ETLV_ChannelDataString ) );
            //Set Data mode using command qualifier
            TUint8 cmdQualifier(
                commandDetails.GetShortInfo( ETLV_CommandQualifier ) );
            if ( KBipSendDataImmediately & cmdQualifier )
                {
                // bit 1 = 1
                sendDataV2.iMode = RSat::ESendDataImmediately;
                }
            else
                {
                // bit 1 = 0
                sendDataV2.iMode = RSat::EStoreDataInTxBuffer;
                }
            // Alpha Id (Optional)
            sendDataV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            CTlv alphaIdentifier;
            TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier,
                KTlvAlphaIdentifierTag ) );
            if ( KErrNotFound != returnValue )
                {
                TUint16 alphaIdLength = alphaIdentifier.GetLength();
                if ( alphaIdLength )
                    {
                    // get the alpha id
                    TPtrC8 sourceString;
                    sourceString.Set(
                        alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
                    // convert and set the alpha id
                    TSatUtility::SetAlphaId( sourceString ,
                        sendDataV2.iAlphaId.iAlphaId );
                    sendDataV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    }
                else
                    {
                    sendDataV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
                    }
                }
            // Icon Id (Optional)
            TSatUtility::FillIconStructure( berTlv,
                sendDataV2.iIconId );
            }
        else
            {
            // Required values missing
            TBuf8<1> noAdditionalInfo( 0 );
            TUint8 channelDataLength( 0 );
            iSatMessHandler->SendDataTerminalResp(
                 iTransId,
                 iCommandDetails,
                 RSat::KErrorRequiredValuesMissing,
                 noAdditionalInfo,
                 channelDataLength );
            ret = KErrCorrupt;
            }
        // Complete request
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a SendData PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifySendData::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYSENDDATA_NOTIFY, "CSatNotifySendData::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifySendData::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iSendDataRspV2Pckg =
        static_cast< RSat::TSendDataV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KSendData );
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::TerminalResponse
// Handles a SendData terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendData::TerminalResponse
        (
        TDes8* aRsp     // Response package
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYSENDDATA_TERMINALRESPONSE, "CSatNotifySendData::TerminalResponse" );
    TFLOGSTRING("CSatNotifySendData::TerminalResponse");
    TInt   ret( KErrNone );
    TBuf8<RSat::KAdditionalInfoMaxSize> additionalInfo;
    additionalInfo.Zero();
    RSat::TSendDataRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TSendDataRspV2Pckg* >( aRsp );
    RSat::TSendDataRspV2& rspV2 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }

    if ( RSat::KMeProblem == rspV2.iInfoType )
        {
        if( rspV2.iAdditionalInfo.Length() )
            {
            additionalInfo.Append( TUint8( rspV2.iAdditionalInfo[0] ) );
            }
        else
            {
            ret = KErrCorrupt;
            }
        }
    iSatMessHandler->SendDataTerminalResp(
        iTransId,
        iCommandDetails,
        TUint8( rspV2.iGeneralResult ),
        additionalInfo,
        rspV2.iChannelDataLength );
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::CSatNotifyReceiveData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyReceiveData::CSatNotifyReceiveData
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        CSatNotificationsBase( aSatMessHandler, aSatMessaging ),
        iReceiveDataRspV2Pckg( NULL )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYRECEIVEDATA_CSATNOTIFYRECEIVEDATA, "CSatNotifyReceiveData::CSatNotifyReceiveData" );
    // Following results are also allowed for this command:
    // (in addition to result declared in base class constructor)
    iAllowedResults += RSat::KSuccessRequestedIconNotDisplayed
        + RSat::KPSessionTerminatedByUser + RSat::KErrorRequiredValuesMissing
        + RSat::KBearerIndepProtocolError;
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::~CSatNotifyReceiveData
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyReceiveData::~CSatNotifyReceiveData()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYRECEIVEDATA_CSATNOTIFYRECEIVEDATA, "CSatNotifyReceiveData::~CSatNotifyReceiveData" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::MessageReceived
// Handles a ReceiveData proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyReceiveData::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage    // Received isi-message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYRECEIVEDATA_MESSAGERECEIVED, "CSatNotifyReceiveData::MessageReceived" );
    TFLOGSTRING("CSatNotifyReceiveData::MessageReceived");
    // return value for completion of the request
    TInt ret( KErrNone );
    // Get ber tlv
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details TLV
    iCommandDetails.Copy( commandDetails.Data() );
    iTransId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );
    if ( !iReqHandle )
        {
        // Request not on, returning response immediately
        TBuf8<1> empty(0);
        TUint8 channelDataLength( 0 );
        iSatMessHandler->ReceiveDataTerminalResp( iTransId,
                                                    iCommandDetails,
                                                    RSat::KMeUnableToProcessCmd,
                                                    empty,
                                                    channelDataLength );
        }
    else
        {
        // Fill the Receive Data structure
        RSat::TReceiveDataV2& receiveDataV2 =
            ( *iReceiveDataRspV2Pckg )();
        // Store transaction ID
        receiveDataV2.SetPCmdNumber(
            commandDetails.GetShortInfo( ETLV_CommandNumber ) );
        // Store Channel Id
        CTlv deviceIds;
        if ( KErrNotFound != berTlv.TlvByTagValue( &deviceIds,
            KTlvDeviceIdentityTag ) )
            {
            receiveDataV2.iDestination = (RSat::TDeviceId) deviceIds.GetShortInfo(
                ETLV_DestinationDeviceIdentity );
            }
        // Channel data length (Mandatory)
        CTlv channelDataLengthTlv;
        if ( KErrNone == berTlv.TlvByTagValue( &channelDataLengthTlv,
             KTlvChannelDataLengthTag ) )
            {
            receiveDataV2.iChannelDataLength =
                channelDataLengthTlv.GetShortInfo( ETLV_ChannelDataLength );
            // Alpha Id (Optional)
            receiveDataV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
            CTlv alphaIdentifier;
            TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier,
                KTlvAlphaIdentifierTag ) );
            if ( KErrNotFound != returnValue )
                {
                TUint16 alphaIdLength = alphaIdentifier.GetLength();
                if ( alphaIdLength )
                    {
                    // get the alpha id
                    TPtrC8 sourceString;
                    sourceString.Set(
                        alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
                    // convert and set the alpha id
                    TSatUtility::SetAlphaId( sourceString ,
                        receiveDataV2.iAlphaId.iAlphaId );
                    receiveDataV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    }
                else
                    {
                    receiveDataV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
                    }
                }
            // Icon Id (Optional)
            TSatUtility::FillIconStructure( berTlv,
                receiveDataV2.iIconId );
            }
        else
            {
            // Required values missing
            TBuf8<1> noAdditionalInfo( 0 );
            TUint8 channelDataLength( 0 );
            iSatMessHandler->ReceiveDataTerminalResp(
                 iTransId,
                 iCommandDetails,
                 RSat::KErrorRequiredValuesMissing,
                 noAdditionalInfo,
                 channelDataLength );
            ret = KErrCorrupt;
            }
        // Complete request
        CompleteRequest( ret );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a ReceiveData PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyReceiveData::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace1( TRACE_NORMAL, CSATNOTIFYRECEIVEDATA_NOTIFY, "CSatNotifyReceiveData::Notify Handle: %u", aReqHandle );
    TFLOGSTRING2("CSatNotifyReceiveData::Notify. \n\t\t\t Handle:%d\n\t\t\t",
               aReqHandle );
    iReqHandle = aReqHandle;
    iReceiveDataRspV2Pckg =
        static_cast< RSat::TReceiveDataV2Pckg* >( aDataPtr );
    iSatMessaging->SatReady( KReceiveData );
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::TerminalResponse
// Handles a ReceiveData terminal response provided by an ETel SAT client via
// ETel server, transforms it into an ISI message, and passes it to domestic OS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatNotifyReceiveData::TerminalResponse
        (
        TDes8* aRsp     // Response package
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYRECEIVEDATA_TERMINALRESPONSE, "CSatNotifyReceiveData::TerminalResponse" );
    TFLOGSTRING("CSatNotifyReceiveData::TerminalResponse");
    TInt   ret( KErrNone );
    TBuf8<RSat::KAdditionalInfoMaxSize> additionalInfo;
    additionalInfo.Zero();
    RSat::TReceiveDataRspV2Pckg* aRspPckg =
            reinterpret_cast< RSat::TReceiveDataRspV2Pckg* >( aRsp );
    RSat::TReceiveDataRspV2& rspV2 = ( *aRspPckg ) ();
    // Check that general result value is valid
    if ( iAllowedResults != rspV2.iGeneralResult )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }
    if ( ( RSat::KMeProblem == rspV2.iInfoType )
        || ( RSat::KChannelData == rspV2.iInfoType ) )
        {
        if( rspV2.iAdditionalInfo.Length() )
            {
            // Channel Data available and sent using Additional info
            // Data are received in unicode format: 0x00XX where XX is
            // meaningful.
            additionalInfo.Copy( rspV2.iAdditionalInfo );
            }
        else
            {
            ret = KErrCorrupt;
            }
        }
    iSatMessHandler->ReceiveDataTerminalResp(
        iTransId,
        iCommandDetails,
        TUint8( rspV2.iGeneralResult ),
        additionalInfo,
        rspV2.iChannelDataLength );
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::CSatNotifyMoSmControlRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatNotifyMoSmControlRequest::CSatNotifyMoSmControlRequest
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        ) : CSatNotificationsBase( aSatMessHandler, aSatMessaging )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYMOSMCONTROLREQUEST_CSATNOTIFYMOSMCONTROLREQUEST, "CSatNotifyMoSmControlRequest::CSatNotifyMoSmControlRequest" );
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::~CSatNotifyMoSmControlRequest
// Destructor
// -----------------------------------------------------------------------------
//
CSatNotifyMoSmControlRequest::~CSatNotifyMoSmControlRequest()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATNOTIFYMOSMCONTROLREQUEST_CSATNOTIFYMOSMCONTROLREQUEST, "CSatNotifyMoSmControlRequest::~CSatNotifyMoSmControlRequest" );
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::MessageReceived
// Handles a MoSmControlRequest proactive command ISI message,
// and completes a pending ETel request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyMoSmControlRequest::MessageReceived
        (
        const TIsiReceiveC& /*aIsiMessage*/    // ISI  message
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYMOSMCONTROLREQUEST_MESSAGERECEIVED, "CSatNotifyMoSmControlRequest::MessageReceived" );
    // In S60 phones, the MoSm control request is received via an ISI
    // message coming from the SMS server. That's why the body
    // of this method is empty, see the class CSatMoSmCtrl. The Etel Sat API is used
    // to pass to S60 Sat Server the alpha identifier provided by the SIM,
    // if any. The SIM can provide such an alpha id to tell the user that the
    // number to which the SMS has to be sent, has been modified by the SIM.
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::CompleteNotification
// Completes an ETel server Mo-Sms Control notification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyMoSmControlRequest::CompleteNotification
        (
        TDesC& aAlphaId,                // Alpha Id
        RSat::TControlResult aResult    // Call Control result
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFICATION, "CSatNotifyMoSmControlRequest::CompleteNotification" );
    TFLOGSTRING("CSatNotifyMoSmControlRequest::CompleteNotification");
    //check that a client has requested this notifications
    if ( iReqHandle )
        {
        // Fill the call control structure
        RSat::TMoSmControlV1& moSmControlV1 = ( *iMoSmControlV1Pckg )();
        // Set Alpha id status
        if ( aAlphaId. Length() )
            {
            moSmControlV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
            }
        else
            {
            moSmControlV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
            }
        //set control result
        moSmControlV1.iResult = aResult;
        // set alpha id
        moSmControlV1.iAlphaId.iAlphaId.Append( aAlphaId );
        //Complete notification
        CompleteRequest( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::Notify
// An ETel SAT client can call this method via ETel server to set a pending
// request in SimAtkTsy for a MoSmControlRequest PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatNotifyMoSmControlRequest::Notify
        (
        const TTsyReqHandle aReqHandle, // Request handle
        TDes8*              aDataPtr    // Pointer to data
        )
    {
    OstTrace0( TRACE_NORMAL, CSATNOTIFYMOSMCONTROLREQUEST_NOTIFY, "CSatNotifyMoSmControlRequest::Notify" );
    TFLOGSTRING("CSatNotifyMoSmControlRequest::Notify");
    iReqHandle = aReqHandle;
    iMoSmControlV1Pckg = static_cast< RSat::TMoSmControlV1Pckg* >( aDataPtr );
    }

//  End of File
