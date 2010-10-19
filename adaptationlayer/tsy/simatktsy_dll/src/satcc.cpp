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




//  INCLUDE FILES
#include "satcc.h"              // sat call control class
#include "satmessaging.h"       // sat messaging class
#include "satmesshandler.h"     // sat message handler class
#include "ber_tlv.h"            // sat ber-tlv classes
#include "satutil.h"            // sat utility class

#include <satcs.h>
#include <pn_const.h>           // server id constants
#include <tisi.h>               // isi message
#include <ss_wmisi.h>           // Modem SS server
#include <call_modemisi.h>      // Modem Call server
#include <gpdsisi.h>            // GPDS server
#include <uiccisi.h>            // UICC server
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satccTraces.h"
#endif



// CONSTANTS
// Zero character code
const TUint8 KZeroCharacterCode                 = 0x30;
// Maximum length for PDP Context Activation Parameters, see 3GPP TS 31.111
const TUint8 KPdpContextActivationParamsMaxSize = 124;
// From 3GPP TS 31.111, 7.3.1.6 Structure of ENVELOPE (CALL CONTROL)
const TUint8 KCcResultAllowedNoModification     = 0x00;
const TUint8 KCcResultNotAllowed                = 0x01;
const TUint8 KCcResultAllowedWithModifications  = 0x02;

// Maximum data length without additional TLVs in envelope response
const TUint8 KCcEmptyResponseLenght     = 0x02;
// Buffer seze used for all CC responses
const TInt KMaximumCcBufferSize =
    4 +     //CALL_MODEM_SB_RESOURCE
    4 +     //CALL_MODEM_SB_RESOURCE_SEQ_ID
    4 +     //CALL_MODEM_SB_RESOURCE_STATUS
    4 +     //CALL_MODEM_SB_MODE
    252 +   //CALL_MODEM_SB_BC
    252 +   //CALL_MODEM_SB_DESTINATION_ADDRESS
    252 +   //CALL_MODEM_SB_DESTINATION_SUBADDRESS
    252 +   //CALL_MODEM_SB_BC
    4 +     //CALL_MODEM_SB_CAUSE
    4;      //CALL_MODEM_SB_CAUSE

const TUint8 KMSBMask = 0x80;
const TUint8 KSw1Sw2Unknown = 0;
const TUint8 KValidTonNpi   = 0x3F;
const TUint8 KSw1Sw2Length  = 2;


// ==================== MEMBER FUNCTIONS ====================================

// -----------------------------------------------------------------------------
// CSatCC::CSatCC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatCC::CSatCC
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
        :
        iSatMessHandler( aSatMessHandler ),
        iSatMessaging( aSatMessaging ),
        iTonNpiPresent( EFalse ),
        iEnvelopeResponseData( NULL )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_CSATCC_TD, "CSatCC::CSatCC" );
    }

// -----------------------------------------------------------------------------
// CSatCC::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatCC::ConstructL()
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_CONSTRUCTL_TD, "CSatCC::ConstructL" );
    TFLOGSTRING("TSY: CSatCC::ConstructL");

    // Create array for CC request information storing
    iCallControlArray = new( ELeave ) RArray<TCallControl>( 1 );

    iUssdTlvSupported = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatCC::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatCC* CSatCC::NewL
        (
        CSatMessHandler*    aSatMessHandler, //Pointer to the message handler
        CTsySatMessaging*   aSatMessaging    //Pointer to satmessaging class
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_NEWL_TD, "CSatCC::NewL" );
    TFLOGSTRING("TSY: CSatCC::NewL");

    CSatCC* self = new( ELeave ) CSatCC( aSatMessHandler, aSatMessaging );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CSatCC::~CSatCC
// C++ destructor
// -----------------------------------------------------------------------------
//
CSatCC::~CSatCC()
    {
    OstTrace0( TRACE_NORMAL,  DUP1_CSATCC_CSATCC_TD, "CSatCC::~CSatCC" );
    TFLOGSTRING("TSY: CSatCC::~CSatCC");

    if( iCallControlArray )
        {
        iCallControlArray->Close();
        delete iCallControlArray;
        }

    if ( iEnvelopeResponseData )
        {
        delete iEnvelopeResponseData;
        }
    }


// -----------------------------------------------------------------------------
// CSatCC::GetArrayIndexById
// Helper method for searching desired information from array
// used for locating stored call control event information
// -----------------------------------------------------------------------------
//
TInt CSatCC::GetArrayIndexById
        (
        const TInt &aTid
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_GETARRAYINDEXBYID_TD, "CSatCC::GetArrayIndexById" );
    TFLOGSTRING("TSY: CSatCC::GetArrayIndexById");

    TInt ret ( KErrNotFound );
    TInt arraySize ( iCallControlArray->Count() );

    // Only there's Cc transaction ongoing
    if ( arraySize )
        {
        for ( TInt i = 0 ; ret == KErrNotFound && i < arraySize ; i++ )
            {
            if ( ( *iCallControlArray )[i].iTransId == aTid )
                {
                ret = i;
                }
            }
        if ( KErrNotFound == ret)
            {
            TFLOGSTRING3("TSY: CSatCC::GetArrayIndexById, Error:ID 0x%x not found. Array size: %d", aTid, arraySize );
            OstTraceExt2( TRACE_NORMAL,  DUP1_CSATCC_GETARRAYINDEXBYID_TD, "CSatCC::GetArrayIndexById, Error: ID 0x%x not found. Array size: %d", aTid, arraySize );
            }
        }
    else
        {
        //None. This is not our message.
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatCC::AddLocationInformationToTlv
// Adds location informating to given TTlv.
// -----------------------------------------------------------------------------
//
void CSatCC::AddLocationInformationToTlv
        (
        TTlv& aTlv //Tlv to be sent
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_ADDLOCATIONINFORMATIONTOTLV_TD, "CSatCC::AddLocationInformationToTlv" );
    TFLOGSTRING("TSY: CSatCC::AddLocationInformationToTlv");

    const CSatMessHandler::TLocationInfo& locInfo
        = iSatMessHandler->LocationInfo();

    aTlv.AddTag( KTlvLocationInformationTag );
    // append Mobile Country  & Network Codes, 3 bytes
    aTlv.AddData( locInfo.iOperatorCode );
    // append LAC code
    aTlv.AddData( locInfo.iLac );
    // append CellId
    aTlv.AddData( locInfo.iCellId );
    }


// -----------------------------------------------------------------------------
// CSatCC::SendSSEnvelope
// Sends call control SS envelope
// -----------------------------------------------------------------------------
//
TInt CSatCC::SendSSEnvelope
        (
        const TCallControl& aCcstruct //Structure containing call event info
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_SENDSSENVELOPE_TD, "CSatCC::SendSSEnvelope" );
    TFLOGSTRING("TSY: CSatCC::SendSSEnvelope");

    // create envelope
    TTlv envelope;
    envelope.Begin  ( KBerTlvCallControlTag );
    // device identities
    envelope.AddTag ( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe );
    envelope.AddByte( KSim );
    // SS string
    envelope.AddTag( KTlvSsStringTag );

    //There is no TON and NPI in SS cc request. Information is part of the
    //number string: Spec ETSI TS 122 030 v5.0.0 says that if number starts
    //with +, TON is international and if it starts without it, it's unknown.
    //Default NPI is used if all digits are in the range 0-9.
    //NPI shall be unknown if other number information is included.
    //In any case if the user has selected some particular
    //NPI, that will be used.
    TInt i( 0 );
    TUint8 tonNpi( KTonNpiUnknown );

    // If SS is entered by user, there's no SEND SS from where to resolve TonNpi
    // Then TonNpi is resolved from SS string
    // TonNpi   = Unknown if '+'-sign is not present
    //          = International if '+'-sign is found
    //          = No TonNpi if phone number is not present or SS-string contains
    //            only SS code
    if ( !iTonNpiPresent )
        {
        // 3GPP TS 22 030 V7.0.0 (2004-03) says:
        // "The procedure always starts with *, #, **, ## or *#
        // and is finished by #. Each part within the procedure
        // is separated by *."

        // skip start characters
        while ( ( i < aCcstruct.iString.Length() ) &&
                 ( ( '*' == aCcstruct.iString[i] ) ||
                   ( '#' == aCcstruct.iString[i] ) ) )
            {
            i++;
            }
        TInt ssCode( 0 );
        // seek for separation character and store service code
        while ( ( i < aCcstruct.iString.Length() )
                && ( '*' != aCcstruct.iString[i] )
                && ( '#' != aCcstruct.iString[i] ) )
            {
            ssCode *= 10;
            ssCode += ( aCcstruct.iString[i] - KZeroCharacterCode );
            i++;
            }
        // With these SS codes dialling number may be included. It's located
        // after first separation mark '*', but it can be empty.
        // See 3GPP TS 22.030 7.0.0, 6.5.2 and Annex B.
        // With checking the service code ton/npi resolving is made
        // unambiguous.
        // Structure of SS string: *service code*SIA*SIB*SIC#
        // Content of SIB and SIC have no impact to ton/npi
        if( SS_GSM_ALL_FORWARDINGS == ssCode
            || SS_GSM_ALL_COND_FORWARDINGS == ssCode
            || SS_GSM_FORW_UNCONDITIONAL == ssCode
            || SS_GSM_FORW_BUSY == ssCode
            || SS_GSM_FORW_NO_REPLY == ssCode
            || SS_GSM_FORW_NO_REACH == ssCode)
            {
            // Set tonNpi international, if separation character (*) and (+)
            // sign is found
            if ( ( ( i + 1 ) < aCcstruct.iString.Length() )
                && ( ( '*' == aCcstruct.iString[i] )
                && ( '+' == aCcstruct.iString[i + 1] ) ) )
                {
                tonNpi = KTonNpiInternational;
                }
            // Procedure is finished by #
            // If * is not found -> no number found
            else if ( ( i < aCcstruct.iString.Length() )
                    && ('#' == aCcstruct.iString[i] ) )
                {
                tonNpi = KNoTonNpi;
                }
            // Additional check for (**) "double star" case
            else if ( ( ( i + 1 )  < aCcstruct.iString.Length() )
                    && ( '*' == aCcstruct.iString[i] )
                    && ( '*' == aCcstruct.iString[i + 1] ) )
                {
                // ** means there is an additional SIB field included,
                // but SIA (number) is empty
                // PhoneNumber is not present
                tonNpi = KNoTonNpi;
                }
            // All other compinations shall result the "unknown" ton/npi
            else
                {
                // Number is found but its tonnpi is unknown
                tonNpi = KTonNpiUnknown;
                }
            }
        else
            {
            //With all other SS codes TON/NPI is set to 0xff
            tonNpi = KNoTonNpi;
            }
        }
    else
        {
        // TonNpi is resolved from SEND SS proactive command
        tonNpi = iTonNpiForSS;
        }
    iTonNpiPresent = EFalse;

    envelope.AddByte( tonNpi );

    TBuf8<255> ss;
    TSatUtility::AsciiToBCD( aCcstruct.iString, ss );
    envelope.AddData( ss );
    // location information
    AddLocationInformationToTlv( envelope );
    // send it away...
    return iSatMessHandler->UiccCatReqEnvelope(
        aCcstruct.iTransId,
        envelope.End());
    }


// -----------------------------------------------------------------------------
// CSatCC::SendUSSDEnvelopeL
// Sends call control USSD envelope
// -----------------------------------------------------------------------------
//
TInt CSatCC::SendUSSDEnvelopeL
        (
        const TCallControl& aCcstruct //Structure containing call event info
        )
    {
    TFLOGSTRING( "TSY: CSatCC::SendUSSDEnvelopeL" );
    OstTrace0( TRACE_NORMAL,  CSATCC_SENDUSSDENVELOPEL_TD, "CSatCC::SendUSSDEnvelopeL" );

    TInt ret( KErrNone );
    // create envelope
    TTlv envelope;
    envelope.Begin( KBerTlvCallControlTag );

    // device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe );
    envelope.AddByte( KSim );

    // USSD string tlv supported
    if ( iUssdTlvSupported )
        {
        envelope.AddTag( KTlvUssdStringTag );
        envelope.AddByte( aCcstruct.iUssdCodingInfo );
        envelope.AddData( aCcstruct.iUssdString );
        AddLocationInformationToTlv( envelope );

        ret = iSatMessHandler->UiccCatReqEnvelope(
            aCcstruct.iTransId,
            envelope.End() );
        }
    else
        {
        // UssdTlv is not supported by the SIM, with Call control
        // Checks whether the USSD string can be coded in the SS obj. SS obj.
        // can only contain "*", "#", and the numbers 0-9.
        if ( IsOnlyDigitsInUssd( aCcstruct.iUssdString ) )
            {
            envelope.AddTag( KTlvSsStringTag );
            // If the Dialling Number/SSC String does not contain a dialling
            // number e.g. a control string deactivating a service, the
            // TON/NPI byte shall be set to 'FF' by the ME (see note
            // 3GPP TS 11.11). in this case TON & NPI must be set to 0xFF
            envelope.AddByte( 0xFF );
            TBuf8<255> ss;
            TSatUtility::AsciiToBCD( aCcstruct.iUssdString, ss );
            envelope.AddData( ss );
            AddLocationInformationToTlv( envelope );

            ret = iSatMessHandler->UiccCatReqEnvelope(
                aCcstruct.iTransId,
                envelope.End());
            }
        else
            {
            // SIM does not support USSD and converting it to SS was
            // impossible. Remove the created CcStruct from the CC array and
            // send CC event response.
            TPtrC8 atkData;
            SendSsResourceControlReqL(
                aCcstruct,
                KSw1Sw2Unknown,
                KSw1Sw2Unknown,
                KError,
                atkData );

            TInt index( GetArrayIndexById( aCcstruct.iTransId ) );
            if ( index != KErrNotFound )
                {
                iCallControlArray->Remove( index );
                iCallControlArray->Compress();
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatCC::SendCallEnvelope
// Sends call control call envelope
// -----------------------------------------------------------------------------
//
TInt CSatCC::SendCallEnvelope
        (
        const TIsiReceiveC& aIsiMessage,
        const TCallControl& aCallControl //Structure containing call event info
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_SENDCALLENVELOPE_TD, "CSatCC::SendCallEnvelope" );

    // create envelope
    TTlv envelope;
    envelope.Begin( KBerTlvCallControlTag );
    // device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe );
    envelope.AddByte( KSim );
    // address is mandatory
    TInt addressLength( 0 );
    TUint sbStartOffset( 0 );

    TInt retValue( aIsiMessage.FindSubBlockOffsetById(
       ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
       CALL_MODEM_SB_DESTINATION_ADDRESS,
       EIsiSubBlockTypeId8Len8,
       sbStartOffset ) );

    if ( KErrNone == retValue )
        {
        envelope.AddTag( KTlvAddressTag );
        envelope.AddByte( KMSBMask | aCallControl.iAddressType );
        // Unicode address must be converted to BCD number
        TBuf8<123> address;
        TSatUtility::AsciiToBCD( aCallControl.iString, address );
        envelope.AddData( address );
        }
    // In Nokia context BC is mandatory
    envelope.AddTag( KTlvCapabilityConfigurationParametersTag );
    // Length of actual BC information element
    envelope.AddByte( aCallControl.iBearerCapabilities.Length() );
    envelope.AddData( aCallControl.iBearerCapabilities );
    // If subaddress present
    retValue = aIsiMessage.FindSubBlockOffsetById(
          ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
          CALL_MODEM_SB_DESTINATION_SUBADDRESS,
          EIsiSubBlockTypeId8Len8,
          sbStartOffset );
    if ( KErrNone == retValue )
        {
        envelope.AddTag( KTlvSubaddressTag );
        // Subaddress is given in same form as expected in envelope
        addressLength = aIsiMessage.Get8bit( sbStartOffset +
            CALL_MODEM_SB_DESTINATION_SUBADDRESS_OFFSET_ADDRLEN );
        envelope.AddData( aIsiMessage.GetData( sbStartOffset +
            CALL_MODEM_SB_DESTINATION_SUBADDRESS_OFFSET_ADDR,
            addressLength ) );
        }
    // Add mandatory location information
    AddLocationInformationToTlv( envelope );
    // send it away..
    return iSatMessHandler->UiccCatReqEnvelope(
        aCallControl.iTransId,
        envelope.End() );
    }

// -----------------------------------------------------------------------------
// CSatCC::SendPdpContextActivationEnvelope
// Sends Call Control PDP context activation envelope
// -----------------------------------------------------------------------------
//
TInt CSatCC::SendPdpContextActivationEnvelope
        (
        const TUint8 aPdpCcEnvelopeTid,
        const TDesC8& aPdpContextActivationParams
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_SENDPDPCONTEXTACTIVATIONENVELOPE_TD, "CSatCC::SendPdpContextActivationEnvelope" );
    TFLOGSTRING( "TSY: CSatCC::SendPdpContextActivationEnvelope" );
    // Create envelope
    TTlv envelope;
    envelope.Begin( KBerTlvCallControlTag );
    // Add Device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe);
    envelope.AddByte( KSim);
    // Add PDP context activation parameters
    envelope.AddTag ( KTlvPdpContextActivationParametersTag );
    envelope.AddData( aPdpContextActivationParams );
    // Add location information
    AddLocationInformationToTlv( envelope );
    // send it away..
    return iSatMessHandler->UiccCatReqEnvelope( aPdpCcEnvelopeTid,
        envelope.End() );
    }

// -----------------------------------------------------------------------------
// CSatCC::UiccCatRespEnvelopeReceivedL
// Handler function of incoming call control related data notification messages
// -----------------------------------------------------------------------------
//
void CSatCC::UiccCatRespEnvelopeReceivedL( const TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_UICCCATRESPENVELOPERECEIVEDL_TD, "CSatCC::UiccCatRespEnvelopeReceivedL" );
    TFLOGSTRING( "TSY:CSatCC::UiccCatRespEnvelopeReceivedL" );

    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_STATUS ) );
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    if ( UICC_STATUS_OK  == status )
        {
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

            // Get Call Control General Result
            TInt result( TSatUtility::Sw1Sw2Check( sw1, sw2 ) );

            // This msg comes for envelope, terminal response and
            // terminal profile. So let's check first if this response is for
            // CC event by comparing transaction id with the one used when
            // sending the CC event envelope.
            TInt ccIndex( GetArrayIndexById( transId ) );

            // This is a response to either Voice Call, SS or USSD CC envelope
            if ( ccIndex != KErrNotFound )
                {
                switch( ( *iCallControlArray )[ccIndex].iRecourceId )
                    {
                    case PN_MODEM_CALL:
                        {
                        SendCallModemResourceReqL(
                            ( *iCallControlArray )[ccIndex],
                            result,
                            apduData.Mid( 0, apduData.Length() - KSw1Sw2Length ) );
                        break;
                        }
                    case PN_SS:
                        {
                        SendSsResourceControlReqL(
                            ( *iCallControlArray )[ccIndex],
                            sw1,
                            sw2,
                            result,
                            apduData.Mid( 0, apduData.Length() - KSw1Sw2Length ) );
                        break;
                        }
                    case PN_GPDS:
                        {
                        SendGpdsResourceControlReq(
                            ( *iCallControlArray )[ccIndex],
                            result,
                            apduData.Mid( 0, apduData.Length() - KSw1Sw2Length ) );
                        break;
                        }
                    default:
                        {
                        break;
                        }
                    } // End of switch
                iCallControlArray->Remove( ccIndex );
                iCallControlArray->Compress();
                } // End of if ( ccIndex != KErrNotFound )
            }
        else // Subblock is mandatory
            {
            TFLOGSTRING("TSY: CSatMessHandler::UiccCatRespEnvelopeReceivedL - Mandatory subblock UICC_SB_APDU not found");
            OstTrace0( TRACE_NORMAL,  DUP1_CSATCC_UICCCATRESPENVELOPERECEIVEDL_TD, "CSatCC::UiccCatRespEnvelopeReceivedL - Mandatory subblock UICC_SB_APDU not found" );
            }
        } // End of if ( UICC_STATUS_OK  == status )
    }

// -----------------------------------------------------------------------------
// CSatCC::ClearArraysForRefresh
// Clears the array containing call control event data due refresh.
// Empiric studies show that transaction id count starts from beginning
// in SIMSON after refresh and therefore possible incoming responses
// for call control are no longer recognizable.
// -----------------------------------------------------------------------------
//
void CSatCC::ClearArraysForRefresh()
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_CLEARARRAYSFORREFRESH_TD, "CSatCC::ClearArraysForRefresh" );
    TFLOGSTRING("TSY: CSatCC::ClearArraysForRefresh");

    iCallControlArray->Reset();
    }

// -----------------------------------------------------------------------------
// CSatCC::SetStatusOfUssdSupport
// Set internal flag according to EF-SST
// -----------------------------------------------------------------------------
//
void CSatCC::SetStatusOfUssdSupport
        (
        TBool aStatus
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_SETSTATUSOFUSSDSUPPORT_TD, "CSatCC::SetStatusOfUssdSupport" );
    TFLOGSTRING("TSY: CSatCC::SetStatusOfUssdSupport");
    iUssdTlvSupported = aStatus;
    }

// -----------------------------------------------------------------------------
// CSatCC::IsOnlyDigitsInUssd
// This methods returns ETrue when a Ussd string contains only "*", "#",
// and the numbers 0-9.
// Note: Ussd string is always received in packed format, when  it 7-bit GSM
// default alphabet.
// -----------------------------------------------------------------------------
//
TBool CSatCC::IsOnlyDigitsInUssd
        (
        TPtrC8 aUSSDString
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_ISONLYDIGITSINUSSD_TD, "CSatCC::IsOnlyDigitsInUssd" );
    TFLOGSTRING("TSY: CSatCC::IsOnlyDigitsInUssd");

    // Unpack it
    TBuf8<256> ussdString;
    TSatUtility::Packed7to8Unpacked( aUSSDString, ussdString );

    TBool ret( ETrue );
    for( TInt k=0; k < ussdString.Length(); k++)
        {
        if ( ussdString[k]!='#' &&
             ussdString[k]!='*' &&
             ussdString[k]!='+' &&
             ( ussdString[k]<'0'  ||
             ussdString[k]>'9' ) )
            {
            ret = EFalse;
            break;
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatCC::MessageReceivedL
// Handle received messages related to event download
// Called by CSatMessHandler::MessageReceivedL, when a new ISI message arrives.
// -----------------------------------------------------------------------------
//
void CSatCC::MessageReceivedL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_MESSAGERECEIVEDL_TD, "CSatCC::MessageReceivedL" );
    TFLOGSTRING("TSY: CSatCC::MessageReceivedL");

    TUint8 resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TUint8 messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );


    if ( PN_MODEM_CALL == resource )
        {
        switch( messageId )
            {
            case CALL_MODEM_RESOURCE_IND:
                {
                CallModemResourceIndL( aIsiMessage );
                break;
                }
            case CALL_MODEM_RESOURCE_CONF_IND:
                {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_CONF_IND");
    OstTrace0( TRACE_NORMAL,  DUP1_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_CONF_IND" );

                if ( CALL_MODEM_RES_CONF_STARTUP ==
                    aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                        CALL_MODEM_RESOURCE_CONF_IND_OFFSET_CONFSTATUS ) )
                    {
                    // configure resource control if CC enabled in (U)SIM
                    // MT call is always controlled by NTSY
                    if ( iCallControlEnabled )
                        {
                        iSatMessHandler->CallModemResourceConfReq(
                            CALL_MODEM_RES_ID_MO_INIT |
                            CALL_MODEM_RES_ID_MT_INIT,
                            CALL_MODEM_RES_ID_MASK_MO_INIT |
                            CALL_MODEM_RES_ID_MASK_MT_INIT );
                        }
                    else
                        {
                        iSatMessHandler->CallModemResourceConfReq(
                            CALL_MODEM_RES_ID_MT_INIT,
                            CALL_MODEM_RES_ID_MASK_MT_INIT );
                        }
                    }
                break;
                }
            case CALL_MODEM_RESOURCE_CONF_RESP:
                {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_CONF_RESP");
    OstTrace0( TRACE_NORMAL,  DUP2_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_CONF_RESP" );

                if ( CALL_MODEM_RES_CONF_SET ==
                    aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                        CALL_MODEM_RESOURCE_CONF_RESP_OFFSET_CONFOPERATION ) )
                    {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_CONF_RESP Resource configured");
    OstTrace0( TRACE_NORMAL,  DUP3_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_CONF_RESP Resource configured" );

                    }
                break;
                }
            case CALL_MODEM_RESOURCE_RESP:
                {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_RESP Resource control sequence done");
    OstTrace0( TRACE_NORMAL,  DUP4_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, CALL_MODEM_RESOURCE_RESP Resource control sequence done" );

                break;
                }
            default:
                {
                // none
                break;
                }

            }
        }
    else if ( PN_SS == resource )
        {
        switch( messageId )
            {
            case SS_RESOURCE_CONTROL_IND:
                {
                SsResourceControlIndL( aIsiMessage );
                break;
                }
            case SS_RESOURCE_CONF_IND:
                {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, SS_RESOURCE_CONF_IND");
    OstTrace0( TRACE_NORMAL,  DUP5_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, SS_RESOURCE_CONF_IND" );

                if ( SS_RESOURCE_CONF_READY  ==
                    aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                        SS_RESOURCE_CONF_IND_OFFSET_CONFSTATUS  ) )
                    {
                    // configure resource control if CC enabled in (U)SIM
                    if ( iCallControlEnabled )
                        {
                        iSatMessHandler->SsResourceConfReq();
                        }
                    }
                break;
                }
            case SS_RESOURCE_CONF_RESP:
                {
                if ( SS_RESOURCE_CONF_SET ==
                        aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                            SS_RESOURCE_CONF_RESP_OFFSET_CONFOPERATION  ) )
                    {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, SS_RESOURCE_CONF_RESP Resource configured");
    OstTrace0( TRACE_NORMAL,  DUP6_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, SS_RESOURCE_CONF_RESP Resource configured" );

                    }
                break;
                }
            case SS_STATUS_IND:
                {
                TUint8 status( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                    SS_STATUS_IND_OFFSET_SSSTATUSINDICATION  ) );

                if ( SS_STATUS_REQUEST_SERVICE_FAILED == status
                    || SS_GSM_STATUS_REQUEST_USSD_FAILED )
                    {
                    if( iEnvelopeResponseData )
                        {
                        iSatMessaging->NotifyClientAboutCallControlEventL(
                            iCcResult,
                            iEnvelopeResponseData->Des() );
                        // Delete temporarily stored envelope response data
                        delete iEnvelopeResponseData;
                        // No matter what the response was, set related data to
                        // init values
                        iCcResult = KAllowed;
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
    else if ( PN_GPDS == resource )
        {
        switch( messageId )
            {
            case GPDS_RESOURCE_CONTROL_IND:
                {
                GpdsResourceControlInd( aIsiMessage );
                break;
                }
            case GPDS_RESOURCE_CONTROL_RESP:
                {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, GPDS_RESOURCE_CONTROL_RESP Resource control sequence done");
    OstTrace0( TRACE_NORMAL,  DUP7_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, GPDS_RESOURCE_CONTROL_RESP Resource control sequence done" );

                break;
                }
            case GPDS_RESOURCE_CONF_IND:
                {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, GPDS_RESOURCE_CONF_IND");
    OstTrace0( TRACE_NORMAL,  DUP8_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, GPDS_RESOURCE_CONF_IND" );

                if ( iCallControlOnGPRSEnabled )
                    {
                    iSatMessHandler->GpdsResourceConfReq();
                    }
                break;
                }
            case GPDS_RESOURCE_CONF_RESP:
                {
                if ( GPDS_RESOURCE_CONF_SET ==
                        aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                            GPDS_RESOURCE_CONF_RESP_OFFSET_CONFOPERATION  ) )
                    {
    TFLOGSTRING("TSY: CSatCC::MessageReceived, GPDS_RESOURCE_CONF_RESP Resource configured");
    OstTrace0( TRACE_NORMAL,  DUP9_CSATCC_MESSAGERECEIVED_TD, "TSY: CSatCC::MessageReceived, GPDS_RESOURCE_CONF_RESP Resource configured" );

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
    else if ( PN_UICC == resource )
        {
        switch( messageId )
            {
            case UICC_APPL_CMD_RESP:
                {
                TUint8 trId( aIsiMessage.Get8bit(
                    ISI_HEADER_SIZE + UICC_APPL_CMD_RESP_OFFSET_TRANSID ) );
                TUint8 status( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                    UICC_APPL_CMD_RESP_OFFSET_STATUS ) );
                TUint8 serviceType( aIsiMessage.Get8bit( ISI_HEADER_SIZE +
                    UICC_APPL_CMD_RESP_OFFSET_SERVICETYPE ) );
                // Card type
                TUint8 cardType( aIsiMessage.Get8bit(
                    ISI_HEADER_SIZE + UICC_APPL_CMD_RESP_OFFSET_CARDTYPE ) );

                if ( UICC_STATUS_OK == status &&
                    UICC_APPL_READ_TRANSPARENT == serviceType )
                    {
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
                        // Check call control status
                        if ( KUiccTrIdServiceTableByte4 == trId )
                            {
                            TInt ret( KErrNone );
                            // Bit 6 is the status bit of call control
                            if ( fileData[0] & KCallControlBitMaskUsim )
                                {
                                // Call control enabled in (U)SIM
                                iCallControlEnabled = ETrue;
                                ret = iSatMessHandler->CallModemResourceConfReq(
                                    CALL_MODEM_RES_ID_MO_INIT |
                                    CALL_MODEM_RES_ID_MT_INIT,
                                    CALL_MODEM_RES_ID_MASK_MO_INIT |
                                    CALL_MODEM_RES_ID_MASK_MT_INIT );
                                User::LeaveIfError( ret );
                                ret = iSatMessHandler->SsResourceConfReq();
                                User::LeaveIfError( ret );
                                }
                             else
                                {
                                // Call control disabled in (U)SIM
                                iCallControlEnabled = EFalse;
                                ret = iSatMessHandler->CallModemResourceConfReq(
                                    CALL_MODEM_RES_ID_MT_INIT,
                                    CALL_MODEM_RES_ID_MASK_MT_INIT );
                                User::LeaveIfError( ret );
                                }
                            }
                        // Check call control GPRS status (UICC) or
                        // Call control status (ICC)
                        else if ( KUiccTrIdServiceTableByte7 == trId )
                            {
                            TInt ret( KErrNone );
                            if ( UICC_CARD_TYPE_UICC == cardType )
                                {
                                if ( fileData[0] & KCallControlBitMaskUsim )
                                    {
                                    // Call control on GPRS enabled in USIM
                                    iCallControlOnGPRSEnabled = ETrue;
                                    ret = iSatMessHandler->GpdsResourceConfReq();
                                    User::LeaveIfError( ret );
                                    }
                                else
                                    {
                                    // Call control on GPRS disabled in USIM
                                    iCallControlOnGPRSEnabled = EFalse;
                                    }
                                }
                            else if ( UICC_CARD_TYPE_ICC == cardType )
                                {
                                if ( KCallControlBitMaskSim == ( fileData[0] & KCallControlBitMaskSim ) )
                                    {
                                    // Call control enabled in (U)SIM
                                    iCallControlEnabled = ETrue;
                                    ret = iSatMessHandler->CallModemResourceConfReq(
                                        CALL_MODEM_RES_ID_MO_INIT |
                                        CALL_MODEM_RES_ID_MT_INIT,
                                        CALL_MODEM_RES_ID_MASK_MO_INIT |
                                        CALL_MODEM_RES_ID_MASK_MT_INIT );
                                    User::LeaveIfError( ret );
                                    ret = iSatMessHandler->SsResourceConfReq();
                                    User::LeaveIfError( ret );
                                    }
                                else
                                    {
                                    // Call control disabled in (U)SIM
                                    iCallControlEnabled = EFalse;
                                    ret = iSatMessHandler->CallModemResourceConfReq(
                                        CALL_MODEM_RES_ID_MT_INIT,
                                        CALL_MODEM_RES_ID_MASK_MT_INIT );
                                    User::LeaveIfError( ret );
                                    }
                                }
                            }
                        }
                    else // Subblock is mandatory
                        {
                        TFLOGSTRING("TSY: CSatCC::MessageReceivedL - Mandatory subblock UICC_SB_FILE_DATA not found");
                        OstTrace0( TRACE_NORMAL,  DUP1_CSATCC_MESSAGERECEIVEDL_TD, "CSatCC::MessageReceivedL - - Mandatory subblock UICC_SB_FILE_DATA not found" );
                        }
                    } // End of if ( UICC_STATUS_OK == status
                break;
                } // End of case UICC_APPL_CMD_RESP
            case UICC_CAT_RESP:
                {
                // In case of envelope response handle the data
                TUint8 serviceType(
                    aIsiMessage.Get8bit(
                        ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_SERVICETYPE ) );
                if ( UICC_CAT_ENVELOPE == serviceType )
                    {
                    UiccCatRespEnvelopeReceivedL( aIsiMessage );
                    }
                break;
                }
            default:
                {
                break;
                }
            } // End of switch( messageId )
        } // End of else if ( PN_UICC == resource )
    // No else
    }

// -----------------------------------------------------------------------------
// CSatCC::SetTonNpi
// Set CSatCC internal Ton and Npi
// Called by CTsySatMessaging::SetTonNpi when SEND SS proactive command arrives
// -----------------------------------------------------------------------------
//
void CSatCC::SetTonNpi
        (
        const TUint8 aTonNpi
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATCC_SETTONNPI_TD, "CSatCC::SetTonNpi" );
    TFLOGSTRING("TSY: CSatCC::SetTonNpi");

    iTonNpiForSS = aTonNpi;
    iTonNpiPresent = ETrue;
    }

// -----------------------------------------------------------------------------
// CSatCC::CallModemResourceIndL
// Handles resource control request from modem Call server
// -----------------------------------------------------------------------------
//
void CSatCC::CallModemResourceIndL( const TIsiReceiveC& aIsiMessage )
    {
    TFLOGSTRING("TSY: CSatCC::CallModemResourceIndL");
    OstTrace0( TRACE_NORMAL,  CSATCC_CALLMODEMRESOURCEINDL_TD, "CSatCC::CallModemResourceIndL" );

    TUint sbStartOffset( 0 );
    // Check if resource control is requested for MO call.

    TInt retValue( aIsiMessage.FindSubBlockOffsetById(
           ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
           CALL_MODEM_SB_RESOURCE,
           EIsiSubBlockTypeId8Len8,
           sbStartOffset ) );

    if ( KErrNone == retValue
        && CALL_MODEM_RES_ID_MO_INIT & aIsiMessage.Get16bit( sbStartOffset +
            CALL_MODEM_SB_RESOURCE_OFFSET_RES  ) )
        {
        TCallControl callcontrol;
        // store traid's
        callcontrol.iTransId = aIsiMessage.Get8bit( ISI_HEADER_SIZE +
            CALL_MODEM_RESOURCE_IND_OFFSET_TRID );
        callcontrol.iRecourceId = aIsiMessage.Get8bit(
            ISI_HEADER_OFFSET_RESOURCEID);
        callcontrol.iCallId = aIsiMessage.Get8bit( ISI_HEADER_SIZE +
            CALL_MODEM_RESOURCE_IND_OFFSET_CALLID );

        retValue = aIsiMessage.FindSubBlockOffsetById(
           ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
           CALL_MODEM_SB_RESOURCE_SEQ_ID,
           EIsiSubBlockTypeId8Len8,
           sbStartOffset );
        if( KErrNone == retValue )
            {
            callcontrol.iResourceSeqId = aIsiMessage.Get8bit( sbStartOffset +
                CALL_MODEM_SB_RESOURCE_SEQ_ID_OFFSET_SEQUENCEID );
            }

        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
            CALL_MODEM_SB_MODE,
            EIsiSubBlockTypeId8Len8,
            sbStartOffset );
        if( KErrNone == retValue )
            {
            callcontrol.iCallMode.Append( aIsiMessage.Get8bit( sbStartOffset +
                CALL_MODEM_SB_MODE_OFFSET_MODE ) );
            callcontrol.iCallMode.Append( aIsiMessage.Get8bit( sbStartOffset +
                CALL_MODEM_SB_MODE_OFFSET_MODEINFO ) );
            }

        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
            CALL_MODEM_SB_BC,
            EIsiSubBlockTypeId8Len8,
            sbStartOffset );
        if( KErrNone == retValue )
            {
            TInt bearerLength( aIsiMessage.Get8bit( sbStartOffset +
                CALL_MODEM_SB_BC_OFFSET_BCLENGTH ) );

            callcontrol.iBearerCapabilities.Copy( aIsiMessage.GetData(
                sbStartOffset + CALL_MODEM_SB_BC_OFFSET_BCDATA,
                bearerLength ) );
            }

        retValue = aIsiMessage.FindSubBlockOffsetById(
           ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
           CALL_MODEM_SB_DESTINATION_ADDRESS,
           EIsiSubBlockTypeId8Len8,
           sbStartOffset );

        if( KErrNone == retValue )
            {
            callcontrol.iAddressType = aIsiMessage.Get8bit( sbStartOffset +
                CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDRTYPE );
            TUint8 addressLength( aIsiMessage.Get8bit( sbStartOffset +
                CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDRLEN ) );
            callcontrol.iString.Copy( aIsiMessage.GetData( sbStartOffset +
                CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDR,
                addressLength * 2 ) );
            }

        retValue = aIsiMessage.FindSubBlockOffsetById(
           ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND ,
           CALL_MODEM_SB_CHECK_INFO,
           EIsiSubBlockTypeId8Len8,
           sbStartOffset );
        if( KErrNone == retValue )
            {
            callcontrol.iCheckInfo = aIsiMessage.GetData( sbStartOffset,
                SIZE_CALL_MODEM_SB_CHECK_INFO );
            }
        if( CALL_MODEM_MODE_EMERGENCY == callcontrol.iCallMode[0] )
            {
            // Do not make SIM call control, allow emergency calls always
            TPtrC8 atkData;
            SendCallModemResourceReqL(
                callcontrol,
                KAllowed,
                atkData );
            }
        else
            {
            // store struct
            iCallControlArray->Append( callcontrol );
            // Send envelope
            SendCallEnvelope( aIsiMessage, callcontrol );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatCC::SendCallModemResourceReqL
// Creates resource control response for modem Call server
// -----------------------------------------------------------------------------
//
void CSatCC::SendCallModemResourceReqL(
    const TCallControl& aTcc,
    const TUint8 aResult,
    TPtrC8 aApduData )
    {
    TFLOGSTRING("TSY: CSatCC::SendCallModemResourceReqL");
    OstTrace0( TRACE_NORMAL,  CSATCC_SENDCALLMODEMRESOURCEREQL_TD, "CSatCC::SendCallModemResourceReqL" );

    TBuf8<KMaximumCcBufferSize> isiMessage;
    TInt ret( KErrNotFound );
    TBool checkInfoNeeded( EFalse );
    TUint8 checkInfoParams( 0 );
    CBerTlv response;
    response.SetData( aApduData );
    // Set initial cc result, e.g. the SIM has responded with sw1/sw2 90 00
    // without any additional data. In error case reject the initiated action.
    TUint8 ccresult( KAllowed == aResult ?
        KCcResultAllowedNoModification : KCcResultNotAllowed );
    // if result provided in SIM response
    if ( KCcEmptyResponseLenght < aApduData.Length() )
        {
        ccresult = aApduData[ 0 ];
        }
    // Internal call control result
    TUint8 internalCcResult( KAllowed );
    // CALL_MODEM_RESOURCE_REQ has 5 mandatory sb's
    TUint8 sbcount( 5 );

    // Add mandatory data
    // Modem Call ID [M]: the unique call ID or CALL_ID_NONE.
    isiMessage.Append( aTcc.iCallId );
    // CALL_MODEM_SB_RESOURCE [M]: resource. Shall be same as in the corresponding indication.
    TIsiSubBlock resource(
        isiMessage,
        CALL_MODEM_SB_RESOURCE,
        EIsiSubBlockTypeId8Len8 );
    TSatUtility::AppendWord( CALL_MODEM_RES_ID_MO_INIT, isiMessage );
    resource.CompleteSubBlock();

    // CALL_MODEM_SB_RESOURCE_SEQ_ID [M]: Sequence ID. Shall be same as in the corresponding indication.
    TIsiSubBlock resourceSeqId(
        isiMessage,
        CALL_MODEM_SB_RESOURCE_SEQ_ID,
        EIsiSubBlockTypeId8Len8 );
    isiMessage.Append( aTcc.iResourceSeqId  );
    isiMessage.Append( KPadding );
    resourceSeqId.CompleteSubBlock();

    // CALL_MODEM_SB_RESOURCE_STATUS [M]: resource status is allowed or denied.
    TIsiSubBlock status(
        isiMessage,
        CALL_MODEM_SB_RESOURCE_STATUS,
        EIsiSubBlockTypeId8Len8 );

    switch( ccresult )
        {
        case KCcResultAllowedNoModification:
            {
            isiMessage.Append( CALL_MODEM_RESOURCE_ALLOWED );
            isiMessage.Append( KPadding );
            break;
            }
        case KCcResultNotAllowed:
            {
            isiMessage.Append( CALL_MODEM_RESOURCE_DENIED );
            isiMessage.Append( KPadding );
            internalCcResult = KRejected;
            break;
            }
        case KCcResultAllowedWithModifications:
            {
            // First check if this has been modified to a new SS/USSD action
            CTlv ssServerString;

            if ( KErrNone == response.TlvByTagValue( &ssServerString,
                KTlvSsStringTag ) ||
                KErrNone == response.TlvByTagValue( &ssServerString,
                KTlvUssdStringTag ) )
                {
                isiMessage.Append( CALL_MODEM_RESOURCE_DENIED );
                isiMessage.Append( KPadding );
                internalCcResult = KChanged;
                }
            else
                {
                // Other parameters have been changed
                isiMessage.Append( CALL_MODEM_RESOURCE_ALLOWED );
                isiMessage.Append( KPadding );
                internalCcResult = KModified;
                checkInfoNeeded = ETrue;
                checkInfoParams += CALL_MODEM_CHK_DISABLE_FDN;
                }
            break;
            }
        default:
            {
            break;
            }
        }
    status.CompleteSubBlock();

    if ( KRejected == internalCcResult || KChanged == internalCcResult )
        {
        // CALL_MODEM_SB_CAUSE sb is needed in rejected cases
        TIsiSubBlock cause(
            isiMessage,
            CALL_MODEM_SB_CAUSE,
            EIsiSubBlockTypeId8Len8 );
        isiMessage.Append( CALL_MODEM_CAUSE_TYPE_CLIENT  );
        isiMessage.Append( CALL_MODEM_CAUSE_NOT_ALLOWED );
        cause.CompleteSubBlock();
        sbcount++;
        }

    // CALL_MODEM_SB_MODE [M]: call mode.
    TIsiSubBlock mode(
        isiMessage,
        CALL_MODEM_SB_MODE,
        EIsiSubBlockTypeId8Len8 );
    // 2 byte buffer
    isiMessage.Append( aTcc.iCallMode );
    mode.CompleteSubBlock();

    // CALL_MODEM_SB_BC [M]: Bearer Capabilities for the call.
    TIsiSubBlock bearer(
        isiMessage,
        CALL_MODEM_SB_BC,
        EIsiSubBlockTypeId8Len8 );

    // Check if (U)SIM has provided new bearer caps
    CTlv bcTlv;
    if ( KCcEmptyResponseLenght < aApduData.Length() &&
        KErrNone == response.TlvByTagValue( &bcTlv,
            KTlvCapabilityConfigurationParametersTag ) )
        {
        // BC length is the first byte in BC data
        isiMessage.Append( bcTlv.GetData(
            ETLV_CapabilityConfigurationParameters ) );
        }
    else
        {
        isiMessage.Append( aTcc.iBearerCapabilities.Length() );
        isiMessage.Append( aTcc.iBearerCapabilities );
        }
    bearer.CompleteSubBlock();

    TIsiSubBlock address(
        isiMessage,
        CALL_MODEM_SB_DESTINATION_ADDRESS ,
        EIsiSubBlockTypeId8Len8 );
    // Check if request has been modified and therefore response includes
    // additional data
    if ( KModified == internalCcResult &&
        ( KCcEmptyResponseLenght < aApduData.Length() ) )
        {
        CTlv addressTlv;
        ret = response.TlvByTagValue( &addressTlv, KTlvAddressTag );
        if ( KErrNone == ret  )
            {
            // CALL_MODEM_ADDRESS_TYPE, mask MSB off
            TUint8 tonNpi( addressTlv.GetValue()[ 0 ] ^ KMSBMask );

            // check if valid TON/NPI
            if( KValidTonNpi >= tonNpi )
                {
                isiMessage.Append( tonNpi );
                }
            else
                {
                // use default TON/NPI in reserved cases
                isiMessage.Append(
                    CALL_MODEM_NBR_TYPE_UNKNOWN |
                    CALL_MODEM_NBR_PLAN_ISDN_TELEPHONY );
                }

            isiMessage.Append( KPadding );
            isiMessage.Append( KPadding );

            // Temp storage for address
            TBuf8<2 * KCallServerMaxAddressLenght> asciiAddress;
            TSatUtility::BCDToAscii( addressTlv.GetValue().Mid( 1 ),
                asciiAddress );

            // let's check is new number "112". This because of if number
            // is "112" emergency call is needed to establish. otherwise
            // normal call needs to be established, even new number is found
            // from EFecc. So if number is not "112" we need to add subblock
            // CALL_MODEM_CHECK_INFO with CALL_MODEM_CHK_DISABLE_EMERG
            _LIT8(KEccNbr, "112");
            if( 0 != asciiAddress.Compare( KEccNbr ) )
                {
                checkInfoNeeded = ETrue;
                checkInfoParams += CALL_MODEM_CHK_DISABLE_EMERG;
                }

            //add the number string as unicode.
            TBuf16<KCallServerMaxAddressLenght> unicodeNumber;
            TSatUtility::ConvertSms7ToUnicode16( unicodeNumber, asciiAddress );
            TBuf8<2 * KCallServerMaxAddressLenght> temp;
            TIsiUtility::CopyToBigEndian( unicodeNumber, temp );

            // Address length = Number of Unicode characters in address
            isiMessage.Append( temp.Length()/2 );
            isiMessage.Append( temp );

            address.CompleteSubBlock();
            sbcount++;
            }

        CTlv subAddressTlv;
        ret = response.TlvByTagValue( &subAddressTlv, KTlvSubaddressTag );
        if ( KErrNone == ret )
            {
            TIsiSubBlock subAddress(
                isiMessage,
                CALL_MODEM_SB_DESTINATION_SUBADDRESS,
                EIsiSubBlockTypeId8Len8 );
            isiMessage.Append( subAddressTlv.GetLength() );
            isiMessage.Append( subAddressTlv.GetData( ETLV_SubAddress ) );
            subAddress.CompleteSubBlock();
            sbcount++;
            }
        }
    else
        {
        // use original destination address
        isiMessage.Append( aTcc.iAddressType );
        isiMessage.Append( KPadding );
        isiMessage.Append( KPadding );
        // Number of Unicode characters
        isiMessage.Append( aTcc.iString.Length()/2 );
        isiMessage.Append( aTcc.iString );
        address.CompleteSubBlock();
        sbcount++;
        }

    // CALL_MODEM_SB_CHECK_INFO[O]: bitfield saying if some checks
    // should be ignored in Call Modem Server
    if ( aTcc.iCheckInfo.Length() )
        {
        sbcount++;
        if( checkInfoNeeded )
            {
            TUint8 oldInfo( aTcc.iCheckInfo[CALL_MODEM_SB_CHECK_INFO_OFFSET_CHECKINFO] );
            TUint8 newInfo( checkInfoParams | oldInfo );
            TIsiSubBlock checkInfo(
                isiMessage,
                CALL_MODEM_SB_CHECK_INFO,
                EIsiSubBlockTypeId8Len8 );
            isiMessage.Append( newInfo );
            checkInfo.CompleteSubBlock();
            }
        else
            {
            isiMessage.Append( aTcc.iCheckInfo );
            }
        }
    else if( checkInfoNeeded )
        {
        sbcount++;
        TIsiSubBlock checkInfo(
            isiMessage,
            CALL_MODEM_SB_CHECK_INFO,
            EIsiSubBlockTypeId8Len8 );
        isiMessage.Append( checkInfoParams );
        checkInfo.CompleteSubBlock();
        }
    // no else

    TBuf8<1> numOfSubblocks;
    numOfSubblocks.Append( sbcount);
    isiMessage.Insert( 1, numOfSubblocks );

    // before sending the call control result to call server,
    // inform NTSY about call control CALL_ID and call control result
    CMmDataPackage dataPackage;
    TUint8 callId( aTcc.iCallId );
    dataPackage.PackData( &callId, &ccresult );
    iSatMessaging->GetMessageRouter()->ExtFuncL(
        ESatNotifyCallControlRequest, &dataPackage );
    // send request
    iSatMessHandler->CallModemResourceReq( aTcc.iTransId, isiMessage );

    if ( KCcEmptyResponseLenght < aApduData.Length() )
        {
        // Notify client about call control if alpha id present or original
        // action has changed to new action
        iSatMessaging->NotifyClientAboutCallControlEventL(
            internalCcResult,
            aApduData );
        }
   }

// -----------------------------------------------------------------------------
// CSatCC::SsResourceControlIndL
// Handles resource control request from modem SS server
// -----------------------------------------------------------------------------
//
void CSatCC::SsResourceControlIndL( const TIsiReceiveC& aIsiMessage )
    {
    TFLOGSTRING("TSY: CSatCC::SsResourceControlIndL");
    OstTrace0( TRACE_NORMAL,  CSATCC_SSRESOURCECONTROLINDL_TD, "CSatCC::SsResourceControlIndL" );

    TCallControl callcontrol;
    TInt stringLength;
    // store traid's
    callcontrol.iTransId = aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        SS_RESOURCE_CONTROL_IND_OFFSET_TRANSID );
    callcontrol.iRecourceId = aIsiMessage.Get8bit(
        ISI_HEADER_OFFSET_RESOURCEID);

    TUint sbStartOffset( 0 );
    TInt retValue( aIsiMessage.FindSubBlockOffsetById(
       ISI_HEADER_SIZE + SIZE_SS_RESOURCE_CONTROL_IND,
       SS_SB_RESOURCE_SEQ_ID,
       EIsiSubBlockTypeId8Len8,
       sbStartOffset ) );

    if( KErrNone == retValue )
        {
        callcontrol.iResourceSeqId = aIsiMessage.Get8bit( sbStartOffset +
            SS_SB_RESOURCE_SEQ_ID_OFFSET_SEQUENCEID );
        }

    retValue = aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_RESOURCE_CONTROL_IND,
        SS_SB_SS_CONTROL,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset );

    if( KErrNone == retValue )
        {
        stringLength = aIsiMessage.Get8bit( sbStartOffset +
            SS_SB_SS_CONTROL_OFFSET_SSSTRINGLENGTH );
        callcontrol.iString.Copy( aIsiMessage.GetData(
            sbStartOffset + SS_SB_SS_CONTROL_OFFSET_SSSTRING,
            stringLength) );
        }

    retValue = aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_RESOURCE_CONTROL_IND,
        SS_SB_USSD_CONTROL,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset );

    if( KErrNone == retValue )
        {
        callcontrol.iUssdCodingInfo = aIsiMessage.Get8bit( sbStartOffset +
            SS_SB_USSD_CONTROL_OFFSET_CODINGINFO );
        stringLength = aIsiMessage.Get8bit( sbStartOffset +
            SS_SB_USSD_CONTROL_OFFSET_USSDSTRINGLENGTH  );

        callcontrol.iUssdString.Copy( aIsiMessage.GetData(
            sbStartOffset + SS_SB_USSD_CONTROL_OFFSET_USSDSTRING,
            stringLength ) );
        }
    // store struct
    iCallControlArray->Append( callcontrol );
    // Send envelope
    if ( callcontrol.iString.Length() )
        {
        SendSSEnvelope( callcontrol );
        }
    else
        {
        SendUSSDEnvelopeL( callcontrol );
        }
    }

// -----------------------------------------------------------------------------
// CSatCC::SendSsResourceControlReqL
// Creates resource control response for modem SS server
// -----------------------------------------------------------------------------
//
void CSatCC::SendSsResourceControlReqL(
        const TCallControl& aTcc,
        const TUint8 aSw1,
        const TUint8 aSw2,
        const TUint8 aResult,
        TPtrC8 aApduData )
    {
    TFLOGSTRING("TSY: CSatCC::SendSsResourceControlReqL");
    OstTrace0( TRACE_NORMAL,  CSATCC_SENDSSRESOURCECONTROLREQL_TD, "CSatCC::SendSsResourceControlReqL" );

    TBuf8<KMaximumCcBufferSize> isiMessage;
    CBerTlv response;
    response.SetData( aApduData );
    // Set initial cc result, e.g. the SIM has responded with sw1/sw2 90 00
    // without any additional data. In error case reject the initiated action.
    TUint8 ccresult( KAllowed == aResult ?
        KCcResultAllowedNoModification : KCcResultNotAllowed );
    // if provided in SIM response
    if ( KCcEmptyResponseLenght < aApduData.Length() )
        {
        ccresult = aApduData[0];
        }

    // Internal call control result
    TUint8 internalCcResult( KAllowed );
    // if original request is changed to new action, the sequence differs
    // when the original action is changed to other SS server action
    TBool changedSsServerAction( EFalse );
    // SS_RESOURCE_CONTROL_REQ has 3 mandatory sb's
    TUint8 sbcount( 3 );
    // Add mandatory data
    isiMessage.Append( KPadding );
    // SS_SB_RESOURCE [M]: resource. Shall be same as in the corresponding indication.
    TIsiSubBlock resource(
        isiMessage,
        SS_SB_RESOURCE ,
        EIsiSubBlockTypeId8Len8 );
    TSatUtility::AppendWord( SS_RES_ID_MO_SS_OPERATION, isiMessage );
    resource.CompleteSubBlock();

    // SS_SB_RESOURCE_SEQ_ID [M]: [M]: Sequence ID. Shall be same as in the corresponding indication.
    TIsiSubBlock resourceSeqId(
        isiMessage,
        SS_SB_RESOURCE_SEQ_ID,
        EIsiSubBlockTypeId8Len8 );
    isiMessage.Append( aTcc.iResourceSeqId  );
    isiMessage.Append( KPadding );
    resourceSeqId.CompleteSubBlock();

    // CALL_MODEM_SB_RESOURCE_STATUS [M]: resource status is allowed or denied.
    TIsiSubBlock status(
        isiMessage,
        SS_SB_RESOURCE_STATUS,
        EIsiSubBlockTypeId8Len8 );

    switch( ccresult )
        {
        case KCcResultAllowedNoModification:
            {
            isiMessage.Append( SS_RESOURCE_ALLOWED );
            isiMessage.Append( KPadding );
            break;
            }
        case KCcResultNotAllowed:
            {
            isiMessage.Append( SS_RESOURCE_DENIED );
            isiMessage.Append( KPadding );
            internalCcResult = KRejected;
            break;
            }
        case KCcResultAllowedWithModifications:
            {
            // First check if this has been modified to a new CALL
            CTlv address;

            if ( KErrNone == response.TlvByTagValue( &address,
                    KTlvAddressTag ) )
                {
                // Original action has been modified to call
                isiMessage.Append( SS_RESOURCE_DENIED );
                isiMessage.Append( KPadding );
                internalCcResult = KChanged;
                }
            // Now check if original SS action has been changed to USSD action
            CTlv ssServerString;
            if ( aTcc.iString.Length() )
                {
                if ( KErrNone == response.TlvByTagValue( &ssServerString,
                        KTlvUssdStringTag ) )
                    {
                    isiMessage.Append( SS_RESOURCE_DENIED );
                    isiMessage.Append( KPadding );
                    internalCcResult = KChanged;
                    changedSsServerAction = ETrue;
                    }
                else if ( KErrNone == response.TlvByTagValue( &ssServerString,
                    KTlvSsStringTag ) )
                    {
                    // original SS string has been modified to new SS string
                    isiMessage.Append( SS_RESOURCE_ALLOWED );
                    isiMessage.Append( KPadding );
                    internalCcResult = KModified;
                    }
                }
            else if ( aTcc.iUssdString.Length() )
                {
                // Or original USSD action has been modified to SS action
                if ( KErrNone == response.TlvByTagValue( &ssServerString,
                        KTlvSsStringTag ) )
                    {
                    isiMessage.Append( SS_RESOURCE_DENIED );
                    isiMessage.Append( KPadding );
                    internalCcResult = KChanged;
                    changedSsServerAction = ETrue;
                    }
                else if ( KErrNone == response.TlvByTagValue( &ssServerString,
                    KTlvUssdStringTag ) )
                    {
                    // Check if modified USSD string has valid DCS
                    if ( ESmsUnknownOrReservedDcs
                        != TSatUtility::DecodeCbsDcs( ssServerString.GetValue()[0] ) )
                        {
                        // USSD string has been modified to new USSD string
                        isiMessage.Append( SS_RESOURCE_ALLOWED );
                        internalCcResult = KModified;
                        }
                    else
                        {
                        // Not valid DCS, do not send USSD to network
                        isiMessage.Append( SS_RESOURCE_DENIED );
                        internalCcResult = KRejected;
                        }
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    status.CompleteSubBlock();

    // SS_SB_SS_CONTROL/SS_SB_USSD_CONTROL is added only if original action has
    // been modified
    if ( KModified == internalCcResult )
        {
        if ( aTcc.iString.Length() )
            {
            CTlv ssStringTlv;
            if ( KErrNone == response.TlvByTagValue( &ssStringTlv,
                KTlvSsStringTag ) )
                {
                TIsiSubBlock ssControl(
                    isiMessage,
                    SS_SB_SS_CONTROL,
                    EIsiSubBlockTypeId8Len8 );

                TUint8 tonNpi( ssStringTlv.GetValue()[0] );
                TPtrC8 ssString( ssStringTlv.GetValue().Mid(1) );

                TBuf8<255> ss;
                TSatUtility::BCDToAscii( ssString, ss );

                if ( KTonNpiInternational == tonNpi )
                    {
                    // insert '+' to the number
                    // ETSI TS 122 030 V5.0.0 (2002-06) says:
                    // "The procedure always starts with *, #, **, ## or *#
                    // and is finished by #. Each part within the procedure
                    // is separated by *."
                    TInt i( 0 );
                    // skip start
                    while ( (i < ss.Length()) && ('*' == ss[i]  || '#' == ss[i] ) )
                        {
                        i++;
                        }
                    // seek to separation
                    while ( (i < ss.Length()) && (ss[i] != '*') )
                        {
                        i++;
                        }
                    // now insert '+' to start of SI
                    if ( i < ss.Length() )
                        {
                        _LIT8(KPlus, "+");
                        ss.Insert(i+1, KPlus);
                        }
                    }
                isiMessage.Append( ss.Length() );
                isiMessage.Append( ss );
                ssControl.CompleteSubBlock();
                sbcount++;
                }
            }
        else
            {
            CTlv ussdStringTlv;
            if ( KErrNone == response.TlvByTagValue( &ussdStringTlv,
                KTlvUssdStringTag ) )
                {
                TIsiSubBlock ussdControl(
                    isiMessage,
                    SS_SB_USSD_CONTROL,
                    EIsiSubBlockTypeId8Len8 );
                // Coding of USSD string. 3GPP TS 23.038 CBS Data Coding Sceme
                isiMessage.Append( ussdStringTlv.GetValue()[0] );
                TPtrC8 ussdString( ussdStringTlv.GetValue().Mid(1) );
                isiMessage.Append( ussdString.Length() );
                isiMessage.Append( ussdString );
                ussdControl.CompleteSubBlock();
                sbcount++;
                }
            }
        }

    // SS_SB_RESOURCE_CONTROL_INFO [O] with sw1, sw2 and result
    TIsiSubBlock resourceCtrlInfo(
        isiMessage,
        SS_SB_RESOURCE_CONTROL_INFO,
        EIsiSubBlockTypeId8Len8 );
    isiMessage.Append( 3 ); // data size is 3 (sw1 + sw2 + result)
    isiMessage.Append( aSw1 );
    isiMessage.Append( aSw2 );
    isiMessage.Append( ccresult );
    sbcount++;
    resourceCtrlInfo.CompleteSubBlock();

    TBuf8<1> numOfSubblocks;
    numOfSubblocks.Append( sbcount);
    isiMessage.Insert( 1, numOfSubblocks );
    // send request
    iSatMessHandler->SsResourceControlReq( aTcc.iTransId, isiMessage );

    if ( KCcEmptyResponseLenght < aApduData.Length()
        && !changedSsServerAction )
        {
        // Notify client about call control if alpha id present
        iSatMessaging->NotifyClientAboutCallControlEventL(
            internalCcResult,
            aApduData );
        }
    else if ( changedSsServerAction )
        {
        // SIM ATK TSY has to wait SS_STATUS_IND before continuing with
        // changed action. This because SS server is still busy with original
        // action. Therefore save data for later usage.
        iEnvelopeResponseData = aApduData.Alloc();
        iCcResult = internalCcResult;
        }
    }

// -----------------------------------------------------------------------------
// CSatCC::GpdsResourceControlInd
// Handles resource control request from modem GPDS server
// -----------------------------------------------------------------------------
//
void CSatCC::GpdsResourceControlInd( const TIsiReceiveC& aIsiMessage )
    {
    TFLOGSTRING("TSY: CSatCC::GpdsResourceControlInd");
    OstTrace0( TRACE_NORMAL,  CSATCC_GPDSRESOURCECONTROLIND_TD, "CSatCC::GpdsResourceControlInd" );

    TCallControl callcontrol;
    TInt paramsLength;
    TBuf8<KPdpContextActivationParamsMaxSize> paramsBuffer;
    // store traid's
    callcontrol.iTransId = aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        GPDS_RESOURCE_CONTROL_REQ_OFFSET_UTID );
    callcontrol.iRecourceId = aIsiMessage.Get8bit(
        ISI_HEADER_OFFSET_RESOURCEID);
    callcontrol.iResourceSeqId = aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        GPDS_RESOURCE_CONTROL_IND_OFFSET_SEQUENCEID );

    TUint sbStartOffset( 0 );
    TInt retValue( KErrNotFound );

    // GPDS_RESOURCE sub block is skipped since resource is always
    // GPDS_RES_ID_CC_FOR_GPRS
    retValue = aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_GPDS_RESOURCE_CONTROL_IND + SIZE_GPDS_RESOURCE,
        GPDS_ACTIVATE_PDP_CONTEXT_REQUEST,
        EIsiSubBlockTypeId8Len16,
        sbStartOffset );

    if( KErrNone == retValue )
        {
        paramsLength = aIsiMessage.Get16bit( sbStartOffset +
            GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_DATALENGTH );
        paramsBuffer.Copy( aIsiMessage.GetData(
            sbStartOffset + GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_DATA,
            paramsLength) );
        }

    // store struct
    iCallControlArray->Append( callcontrol );
    // Send envelope
    SendPdpContextActivationEnvelope( callcontrol.iTransId, paramsBuffer );
    }

// -----------------------------------------------------------------------------
// CSatCC::SendGpdsResourceControlReq
// Creates resource control response for modem GPDS server
// -----------------------------------------------------------------------------
//
void CSatCC::SendGpdsResourceControlReq(
        const TCallControl& aTcc,
        const TUint8 aResult,
        TPtrC8 aAtkData )
    {
    TFLOGSTRING("TSY: CSatCC::SendGpdsResourceControlReq");
    OstTrace0( TRACE_NORMAL,  CSATCC_SENDGPDSRESOURCECONTROLREQ_TD, "CSatCC::SendGpdsResourceControlReq" );

    TBuf8<KMaximumCcBufferSize> isiMessage;
    CBerTlv response;
    response.SetData( aAtkData );
    RSat::TControlResult internalCcResult = RSat::EAllowedNoModification;
    // Set initial cc result, e.g. the SIM has responded with sw1/sw2 90 00
    // without any additional data. In error case reject the initiated action.
    TUint8 ccresult( KAllowed == aResult ?
        KCcResultAllowedNoModification : KCcResultNotAllowed );
    // if provided in SIM response
    if ( aAtkData.Length() )
        {
        ccresult = response.Data()[ 0 ];
        }
    TInt retValue( KErrNotFound );
    CTlv paramsTlv;

    isiMessage.Append( aTcc.iResourceSeqId );
    switch( ccresult )
            {
            case KCcResultAllowedNoModification:
                {
                isiMessage.Append( GPDS_ALLOWED );
                isiMessage.AppendFill( KPadding, 3 );
                // no of sb's
                isiMessage.Append( 1 );
                break;
                }
            case KCcResultNotAllowed:
                {
                isiMessage.Append( GPDS_REJECTED );
                isiMessage.AppendFill( KPadding, 3 );
                // no of sb's
                isiMessage.Append( 1 );
                internalCcResult = RSat::ENotAllowed;
                break;
                }
            case KCcResultAllowedWithModifications:
                {
                // response is modified in GPDS server terminology only if
                // new PDP params are provided
                if ( KCcEmptyResponseLenght < aAtkData.Length()
                    && KErrNone == response.TlvByTagValue( &paramsTlv,
                        KTlvPdpContextActivationParametersTag ) )
                    {
                    isiMessage.Append( GPDS_MODIFIED );
                    isiMessage.AppendFill( KPadding, 3 );
                    // no of sb's
                    isiMessage.Append( 2 );
                    internalCcResult = RSat::EAllowedWithModifications;
                    }
                else
                    {
                    // No new PDP params provided, hence GPDS_ALLOWED
                    isiMessage.Append( GPDS_ALLOWED );
                    isiMessage.AppendFill( KPadding, 3 );
                    // no of sb's
                    isiMessage.Append( 1 );
                    }
                break;
                }
            default:
                {
                break;
                }
            }
    isiMessage.Append( GPDS_RESOURCE  );
    isiMessage.Append( 4 );
    TSatUtility::AppendWord( GPDS_RES_ID_CC_FOR_GPRS, isiMessage );

    if ( KCcEmptyResponseLenght < aAtkData.Length() )
        {
        // GPDS_ACTIVATE_PDP_CONTEXT_REQUEST is shall be present if and only if
        // Resource is GPDS_CC_FOR_GPRS and Result is GPDS_MODIFIED
        if ( RSat::EAllowedWithModifications == internalCcResult )
            {
            TIsiSubBlock contextParams(
                isiMessage,
                GPDS_ACTIVATE_PDP_CONTEXT_REQUEST,
                EIsiSubBlockTypeId8Len16 );
            TInt paramsLength( paramsTlv.GetLength() );
            // Set data length
            TSatUtility::AppendWord( paramsLength, isiMessage);
            // Copy data after tag & length value
            isiMessage.Append( paramsTlv.Data().Mid( 2, paramsLength ) );
            //isiMessage.Append( paramsTlv.GetValue() );
            contextParams.CompleteSubBlock();
            }
        // Check if alpha id present and valid
        CTlv aidTlv;
        retValue = response.TlvByTagValue( &aidTlv, KTlvAlphaIdentifierTag );
        if ( KErrNone == retValue && aidTlv.GetLength() )
            {
            TPtrC8 sourceString;
            sourceString.Set( aidTlv.GetData( ETLV_AlphaIdentifier ) );
            RSat::TAlphaIdBuf alphaId;
            // convert and set the alpha id
            TSatUtility::SetAlphaId( sourceString,
                alphaId );
            iSatMessaging->NotifyClientAboutGprsCallControlEvent(
                alphaId,
                internalCcResult );
            }
        }
    iSatMessHandler->GpdsResourceControlReq( aTcc.iTransId, isiMessage );
    }
// End of File

