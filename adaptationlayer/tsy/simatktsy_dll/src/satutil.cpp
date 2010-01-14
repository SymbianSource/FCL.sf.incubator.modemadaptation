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
#include "satutil.h"            // sat utility class
#include "ber_tlv.h"            // ber-tlv classes

#include <ctsy/serviceapi/cmmsmsutility.h>      // ton/npi constants
#include "tsylogger.h"          // for logging
#include <tisi.h>               // isi message
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satutilTraces.h"
#endif


// CONSTANTS

// Transaction id numbers
const TUint KSatTransIdMaxCountPlusOne          = 255;
const TUint8 KTransactionIdMinNumber            = 0xC0;

// Masking
const TUint8 KPacked7BitTextMask                = 0x0C;

// Shifting
const TUint8 KCrShiftedOneBitToLeft             = 0x1A;
const TUint8 KSiftedSixTimes                    = 0x06;
const TUint8 KCarriageReturn                    = 0x0D;

// Extended table
const TUint8 KExtendedTable                     = 0x1B;


// MODULE DATA STRUCTURES

// Bcd->ascii translation table
static const TUint8 KAscii[16] =
    {
     '0', '1', '2', '3', '4', '5', '6', '7',
     '8', '9', '*', '#', 'p', 'w', '.',  0
    };

// GSM 7-bit ->unicode translation table
// See 3GPP TS 23.038
static const TUint16 KUnicode[128] =
    {
     0x0040, 0x00A3, 0x0024, 0x00A5, 0x00E8, 0x00E9, 0x00F9, 0x00EC,
     0x00F2, 0x00E7, 0x000A, 0x00D8, 0x00F8, 0x000D, 0x00C5, 0x00E5,
     0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8,
     0x03A3, 0x0398, 0x039E, 0x00A0, 0x00C6, 0x00E6, 0x00DF, 0x00C9,
     0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026, 0x0027,
     0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
     0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
     0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
     0x00A1, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
     0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
     0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
     0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6, 0x00D1, 0x00DC, 0x00A7,
     0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
     0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
     0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
     0x0078, 0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1, 0x00FC, 0x00E0
    };


// ==================== MEMBER FUNCTIONS ====================================

// -----------------------------------------------------------------------------
// TTransIdMessage::TTransIdMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TTransIdMessage::TTransIdMessage() : iTransNum(KTransactionIdMinNumber)
    {
    OstTrace0( TRACE_NORMAL, TTRANSIDMESSAGE_TTRANSIDMESSAGE, "TTransIdMessage::TTransIdMessage" );
    }

// -----------------------------------------------------------------------------
// TTransIdMessage::GetTransactionId
// Gets a new transactionID
// -----------------------------------------------------------------------------
//
TUint8 TTransIdMessage::GetTransactionId()
    {
    OstTrace0( TRACE_NORMAL, TTRANSIDMESSAGE_GETTRANSACTIONID, "TTransIdMessage::GetTransactionId" );
    TFLOGSTRING("TSY: TTransIdMessage::GetTransactionId");
    // 0xff and 0x00 are
    // not valid transaction id's and will not be generated. This is done
    // to avoid generating transaction ids that are used in terminal resp
    iTransNum++;
    if ( KSatTransIdMaxCountPlusOne == iTransNum )
        {
        iTransNum = KTransactionIdMinNumber;
        }
    return iTransNum;

    }

// -----------------------------------------------------------------------------
// TSatUtility::GetTonAndNpi
// Map TON and NPI to RSat TON and NPI values
// -----------------------------------------------------------------------------
//
void TSatUtility::GetTonAndNpi
        (
        TInt aTonAndNpi,                // TON and NPI
        RSat::TTypeOfNumber* aTon,      // Type of number
        RSat::TNumberingPlan* aNpi      // Number plan identification
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_GETTONANDNPI, "TSatUtility::GetTonAndNpi: RSat" );
    TFLOGSTRING("TSY: TSatUtility::GetTonAndNpi: RSat");

    TInt ton( ( aTonAndNpi >> 4 ) & KTONMask );

    switch ( ton )
        {
        case KTonUnknown:
            *aTon = RSat::EUnknownNumber;
            break;
        case KInternationalNumber:
            *aTon = RSat::EInternationalNumber;
            break;
        case KNationalNumber:
            *aTon = RSat::ENationalNumber;
            break;
        case KNetworkSpecificNumber:
            *aTon = RSat::ENetworkSpecificNumber;
            break;
        case KSubscriberNumber:
            *aTon = RSat::EDedicatedNumber;
            break;
        case KAlphanumeric:
            *aTon = RSat::EAlphanumericNumber;
            break;
        case KAbbreviatedNumber:
            *aTon = RSat::EAbbreviatedNumber;
            break;
        default:
            *aTon = RSat::ETypeOfNumberNotSet;
            break;
        }

    // Numbering plan
    TInt npi( aTonAndNpi & KNPIMask );

    switch ( npi )
        {
        case KNpiUnknown:
            *aNpi = RSat::EUnknownNumberingPlan;
            break;
        case KIsdnTelephoneNumPlan:
            *aNpi = RSat::EIsdnNumberPlan;
            break;
        case KDataNumPlan:
            *aNpi = RSat::EDataNumberPlan;
            break;
        case KTelexNumPlan:
            *aNpi = RSat::ETelexNumberPlan;
            break;
        case KNationalNumPlan:
            *aNpi = RSat::ENationalNumberPlan;
            break;
        case KPrivateNumPlan:
            *aNpi = RSat::EPrivateNumberPlan;
            break;
        case KServiceCentreSpecificPlan1:
            *aNpi = RSat::EServiceCentreSpecificPlan1;
            break;
        case KServiceCentreSpecificPlan2:
            *aNpi = RSat::EServiceCentreSpecificPlan2;
            break;
        case KErmesNumPlan:
            *aNpi = RSat::EERMESNumberPlan;
            break;
        default:
            *aNpi = RSat::ENumberingPlanNotSet;
            break;
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::GetTonAndNpi
// Map TON and NPI to Etel MM TON and NPI values
// -----------------------------------------------------------------------------
//
void TSatUtility::GetTonAndNpi
        (
        TInt aTonAndNpi,
        RMobilePhone::TMobileTON& aTon,
        RMobilePhone::TMobileNPI& aNpi
        )
    {
    OstTrace0( TRACE_NORMAL, DUP1_TSATUTILITY_GETTONANDNPI, "TSatUtility::GetTonAndNpi: Etel MM" );
    TFLOGSTRING("TSY: TSatUtility::GetTonAndNpi: Etel MM");

    TInt ton( ( aTonAndNpi >> 4 ) & KTONMask );

    switch ( ton )
        {
        case KInternationalNumber:
            aTon = RMobilePhone::EInternationalNumber;
            break;
        case KNationalNumber:
            aTon = RMobilePhone::ENationalNumber;
            break;
        case KNetworkSpecificNumber:
            aTon = RMobilePhone::ENetworkSpecificNumber;
            break;
        case KSubscriberNumber:
            aTon = RMobilePhone::ESubscriberNumber;
            break;
        case KAlphanumeric:
            aTon = RMobilePhone::EAlphanumericNumber;
            break;
        case KAbbreviatedNumber:
            aTon = RMobilePhone::EAbbreviatedNumber;
            break;
        case KTonUnknown:
        default:
            aTon = RMobilePhone::EUnknownNumber;
            break;
        }

    // Numbering plan
    TInt npi( aTonAndNpi & KNPIMask );

    switch ( npi )
        {
        case KIsdnTelephoneNumPlan:
            aNpi = RMobilePhone::EIsdnNumberPlan;
            break;
        case KDataNumPlan:
            aNpi = RMobilePhone::EDataNumberPlan;
            break;
        case KTelexNumPlan:
            aNpi = RMobilePhone::ETelexNumberPlan;
            break;
        case KNationalNumPlan:
            aNpi = RMobilePhone::ENationalNumberPlan;
            break;
        case KPrivateNumPlan:
            aNpi = RMobilePhone::EPrivateNumberPlan;
            break;
        case KServiceCentreSpecificPlan1:
            aNpi = RMobilePhone::EServiceCentreSpecificPlan1;
            break;
        case KServiceCentreSpecificPlan2:
            aNpi = RMobilePhone::EServiceCentreSpecificPlan2;
            break;
        case KErmesNumPlan:
            aNpi = RMobilePhone::EERMESNumberPlan;
            break;
        case KNpiUnknown:
        default:
            aNpi = RMobilePhone::EUnknownNumberingPlan;
            break;
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::Packed7to8Unpacked
// Converts 7-bit packed string to 8-bit unpacked format.
// -----------------------------------------------------------------------------
//
void TSatUtility::Packed7to8Unpacked
        (
        TPtrC8 aString,   //7-bit packed string
        TDes8& aOutput    //8-bit unpacked string
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_PACKED7TO8UNPACKED, "TSatUtility::Packed7to8Unpacked" );
    TFLOGSTRING("TSY: TSatUtility::Packed7to8Unpacked");

    // The string is in packed GSM default alphabet format.
    // Converted to 8-bit format
    TUint8 maskRightPartOfCurrentByte( 0x7F );
    TUint8 shiftLeft( 0 );
    TUint8 leftPartFromPreviousByte( 0 );
    aOutput.Zero();
    for ( TInt i = 0; i < aString.Length(); i++ )
        {
        TUint8 leftPartOfCurrentChar =  ( aString[i] & maskRightPartOfCurrentByte )
            << shiftLeft;

        // append the character in the output text string
        aOutput.Append( leftPartOfCurrentChar | leftPartFromPreviousByte );
        // updates
        if ( KSiftedSixTimes == shiftLeft)
            {
            // After 6 shifts, the character is in bit7..bit1, therefore it has to
            // be shifted one bit to the right.
            TUint8 character = aString[i] >> 1;

            // Check if this is the end character of USSD string 0x0D
            TBool lastCharacter( ( i + 1 ) == aString.Length() );
            if ( !lastCharacter || KCarriageReturn != character )
                {
                // Continue unpacking
                aOutput.Append( character );
                // Restart
                leftPartFromPreviousByte = 0;
                shiftLeft=0;
                maskRightPartOfCurrentByte = 0x7F;
                }
            }
        else
            {
            leftPartFromPreviousByte = (aString[i]) >> (7-shiftLeft);
            maskRightPartOfCurrentByte = maskRightPartOfCurrentByte>>1;
            shiftLeft++;
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::UCSToPacket7
// Converts UCS2 string to 7-bit packed format
// -----------------------------------------------------------------------------
//
void TSatUtility::UCSToPacket7
        (
        TPtrC aString,   //UCS2 string
        TDes8& aOutput   //7-bit packed format output string
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_UCSTOPACKET7, "TSatUtility::UCSToPacket7" );
    TFLOGSTRING("TSY: TSatUtility::UCSToPacket7");

    TBuf8<255> string;
    TSatUtility::ConvertUnicode16ToSms7( string, aString );

    TUint8 move( 0 );
    TInt i( 0 );
    for ( i = 0; i < string.Length(); i += 1 )
        {
        // get first character
        TUint8 char1 = TUint8( string[i] >> move );
        TUint8 char2;

        if ( ( i + 1 ) < string.Length() )
            {
            //  get next character
            char2 = TUint8( string[i + 1]  << ( 7 - move ) );
            }
        else
            {
            // no more characters
            char2 = 0;
            }
        // append packed character
        aOutput.Append( TUint8( char1 | char2 ) );

        if ( ( 6 == move ) && char2 )
            {
            i++;
            move = 0;
            }
        else
            {
            move++;
            }

        }
    if ( !( ( string.Length() + 1 ) % 8 ) )
        {
        // If the total number of characters in the text string equals (8n-1)
        // where n=1,2,3 etc. then there are 7 spare bits at the end of the
        // message. To avoid the situation where the receiving entity confuses
        // 7 binary zero pad bits as the @ character, the carriage return
        // (i.e. <CR>) character shall be used for padding in this situation,
        // as defined in TS 23.038.
        aOutput[  aOutput.Length() - 1 ] =
            TUint8( aOutput[  aOutput.Length() - 1 ] | KCrShiftedOneBitToLeft );
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::BCDToAscii
// Converts BCD string to ASCII format
// -----------------------------------------------------------------------------
//
void TSatUtility::BCDToAscii
        (
        TPtrC8 aString,  //BCD string
        TDes8& aOutput   //Ascii format output string
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_BCDTOASCII, "TSatUtility::BCDToAscii" );
    TFLOGSTRING("TSY: TSatUtility::BCDToAscii");

    // Convert Abbreviated dialling numbers format back to ASCII format.
    // See 3GPP TS 11.11, EFadn
    aOutput.Zero();
    TInt len( aString.Length() );
    TInt outputMaxLength( aOutput.MaxLength() );

    for ( TInt i=0; ( i < len ) && ( aOutput.Length()
            < outputMaxLength ); i++ )
        {
        // two bcd string chars are coded in one byte,
        // 1st char is in low nibble and 2nd is in high nibble
        // if the high nibble doesn't contain a char it's value is 0xf
        TUint8 byte = aString[i];
        aOutput.Append( KAscii[byte & 0xf] );
        if ( 0xf0 != ( byte & 0xf0 )
            && ( aOutput.Length() < outputMaxLength ) )
            {
            aOutput.Append( KAscii[byte >> 4] );
            }
        }

    if ( aOutput.Length() == outputMaxLength )
        {
        OstTrace0( TRACE_NORMAL, DUP1_TSATUTILITY_BCDTOASCII, "TSatUtility::BCDToAscii, probably dropped characters" );
        OstTraceExt2( TRACE_NORMAL, DUP2_TSATUTILITY_BCDTOASCII, "Input buffer length: %d, possible characters: %d", len, len * 2 );
        OstTraceExt2( TRACE_NORMAL, DUP3_TSATUTILITY_BCDTOASCII, "Converted characters: %d, dropped: %d", aOutput.Length(), ( len * 2 ) - aOutput.Length() );

        TFLOGSTRING("TSY: TSatUtility::BCDToAscii, \
            probably dropped characters");
        TFLOGSTRING3("TSY: Input buffer length: %d, possible characters: %d",
            len, len * 2 );
        TFLOGSTRING3("TSY: Converted characters: %d, dropped: %d",
            aOutput.Length(), ( len * 2 ) - aOutput.Length() );
        }
    }


// -----------------------------------------------------------------------------
// TSatUtility::RemoveExpansionDigit
// Expansion digit '.' from phone number string.
// Used in SetUpCall proactive command.
// -----------------------------------------------------------------------------
//
void TSatUtility::RemoveExpansionDigit
        (
        TDes8& aPhoneNumber // Number to be converted
        )

    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_REMOVEEXPANSIONDIGIT, "TSatUtility::RemoveExpansionDigit" );
    TFLOGSTRING("TSY: TSatUtility::RemoveExpansionDigit");

    TInt pos( 0 );

    // Search and delete until no more '.'-characters found
    while ( KErrNotFound != ( pos = aPhoneNumber.Locate( '.' ) ) )
        {
        aPhoneNumber.Delete( pos, 1 );
        }
   }


// -----------------------------------------------------------------------------
// TSatUtility::AsciiToBCD
// Convert ASCII string to binary coded decimal, invalid
// characters are dropped and will not be part of bcd string.
// -----------------------------------------------------------------------------
//
void TSatUtility::AsciiToBCD
        (
        const TDesC8& aString, //unicode string
        TDes8& aOutput         //binary code decimal output string
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_ASCIITOBCD, "TSatUtility::AsciiToBCD" );
    TFLOGSTRING("TSY: TSatUtility::AsciiToBCD");

    TInt i;
    TInt outLen( 0 );
    aOutput.Zero();
    for ( i = 0; i < aString.Length(); i++ )
        {
        // encode as BCD
        TUint8 bcd;
        switch ( aString[i] )
            // do ascii to bcd mapping
            // See 3GPP TS 11.11, EFadn
            {
            case '0': bcd = 0x0; break;
            case '1': bcd = 0x1; break;
            case '2': bcd = 0x2; break;
            case '3': bcd = 0x3; break;
            case '4': bcd = 0x4; break;
            case '5': bcd = 0x5; break;
            case '6': bcd = 0x6; break;
            case '7': bcd = 0x7; break;
            case '8': bcd = 0x8; break;
            case '9': bcd = 0x9; break;
            case '*': bcd = 0xa; break;
            case '#': bcd = 0xb; break;
            case 'p': bcd = 0xc; break; // DTMF
            case 'w': bcd = 0xd; break; // wild
            case '.': bcd = 0xe; break; // RFU
            default:  bcd = 0xf;        // should never be part of number
               TFLOGSTRING3("TSY: TSatUtility::AsciiToBCD -- dropped character %d at i=%d", TInt(aString[i]), i );
               OstTraceExt2( TRACE_NORMAL, DUP1_TSATUTILITY_ASCIITOBCD, "TSatUtility::AsciiToBCD -- dropped character %d at i=%d", TInt(aString[i]), i );

               break;
            }

        // add only valid bcd characters...
        if ( 0xf != bcd )
            {
            if( aOutput.MaxLength() >= aOutput.Length()+1 )
                {
                // store to string
                if ( 0 == ( outLen%2 ) )
                    {
                    aOutput.Append( bcd );
                    }
                else
                    {
                    aOutput[outLen/2] |= ( bcd<<4 );
                    }
                }
             outLen++;
            }
        } // for

    // if odd number of digits add endmark
    if (0 != ( outLen%2 ))
        {
        aOutput[outLen/2] |= 0xf0;
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::SetAlphaId
// Set Alpha identifier as a Unicode text string
// and according to the alphabet used
// -----------------------------------------------------------------------------
//
void TSatUtility::SetAlphaId
        (
        TPtrC8 aRawData, //data
        TDes& aAlphaId   //alpha id
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_SETALPHAID, "TSatUtility::SetAlphaId" );
    TFLOGSTRING("TSY: TSatUtility::SetAlphaId");

    if( aRawData.Length())
        {
        if ( ( KUCS2ArabicCoding == aRawData[0] )
            || ( KUCS2GreekCoding == aRawData[0] )
            || ( KUCS2TurkishCoding == aRawData[0] ) )
            {
            ConvertAlphaFieldsToUnicode( aRawData, aAlphaId );
            }
        else
            {
            // 8-bit
            TBuf8<RSat::KAlphaIdMaxSize> rawData8;
            rawData8.Copy( aRawData );
            ConvertSms7ToUnicode16( aAlphaId, rawData8 );
            }
        }
    else
        {
        TFLOGSTRING("TSY: TSatUtility::SetAlphaId, RawData NULL");
        OstTrace0( TRACE_NORMAL, DUP1_TSATUTILITY_SETALPHAID, "TSatUtility::SetAlphaId, RawData NULL" );

        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::ConvertToSemiOctet
// Convert integer to BCD format. Only two last digits is used.
// Example TInt 2004 -> 0x40.
// -----------------------------------------------------------------------------
//
TUint8 TSatUtility::ConvertToSemiOctet
        (
        TInt aTime  //time
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_CONVERTTOSEMIOCTET, "TSatUtility::ConvertToSemiOctet" );
    TFLOGSTRING("TSY: TSatUtility::ConvertToSemiOctet");

    // Converting given time to meet the TP-Service-Centre-Time-Stamp format in
    // 3GPP TS 23.040.

     TInt msd( ( aTime / 10 ) % 10 ); //  most significant decimal
     TInt lsd( ( aTime % 10 ) );      // least significant decimal

     return TUint8( ( lsd << 4 ) | ( msd ) );

     }

// -----------------------------------------------------------------------------
// TSatUtility::ConvertSms7ToUnicode16
// Convert a text from SMS alphabet to Unicode format.
// -----------------------------------------------------------------------------
//
void TSatUtility::ConvertSms7ToUnicode16
        (
        TDes16& aOutput, //unicode format string
        TDesC8& aInput   //input
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_CONVERTSMS7TOUNICODE16, "TSatUtility::ConvertSms7ToUnicode16" );
    TFLOGSTRING( "TSY: TSatUtility::ConvertSms7ToUnicode16" );

    TInt i( 0 );
    TInt outputMaxLength = aOutput.MaxLength();
    TInt inputLength = aInput.Length();

    for( i = 0; i < inputLength && outputMaxLength > aOutput.Length(); i++ )
        {
        TUint8 character( aInput[i] );

        // This code is an escape to an extension of the 7 bit default alphabet
        // table.
        if ( KExtendedTable == character && ( i + 1 != inputLength ) )
            {
            // extension table
            switch ( aInput[i+1] )
                {
                case 0x28:     // {
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7B ) );
                    break;
                    }
                case 0x29:     // }
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7D ) );
                    break;
                    }
                case 0x3C:     // [
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5B ) );
                    break;
                    }
                case 0x3E:     // ]
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5D ) );
                    break;
                    }
                case 0x3D:     // ~
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7E ) );
                    break;
                    }
                case 0x2F:     /* \ */
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5C ) );
                    break;
                    }
                case 0x14:     // ^
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5E ) );
                    break;
                    }
                case 0x65:     // euro 0x20AC
                    {
                    aOutput.Append( static_cast<TUint16>( 0x20AC ) );
                    break;
                    }
                case 0x40:     // |
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7C ) );
                    break;
                    }
                case 0x0A:  // Form feed
                    {
                    aOutput.Append( static_cast<TUint16>( 0x0C ) );
                    break;
                    }
                case 0x1B:  // Escape to yet another table.
                    {
                    // 3GPP TS 23.038 V6.1.0:
                    // "space until another extension table is defined"
                    aOutput.Append( static_cast<TUint16>( 0x20 ) );
                    break;
                    }
                default:
                    {
                    // 3GPP TS 23.038 V6.1.0: "In the event that an MS receives
                    // a code where a symbol is not represented in the above
                    // table then the MS shall display the character shown in
                    // the main GSM 7 bit default alphabet table"

                    // Check that bit 8 is set to '0'
                    if ( 0x7F >= aInput[i+1] )
                        {
                        // Character is in normal 7-bit table.
                        aOutput.Append( KUnicode[ aInput[i+1] ] );
                        }
                    else
                        {
                        TFLOGSTRING("TSY: TSatUtility::ConvertSms7ToUnicode16, \
                            Character not valid.");
                        OstTrace0( TRACE_NORMAL, DUP1_TSATUTILITY_CONVERTSMS7TOUNICODE16, "TSatUtility::ConvertSms7ToUnicode16, Character not valid." );

                        }
                    break;
                    }
                }
            // Characters in extension table takes two bytes
            i++;
            }
        // Check that bit 8 is set to '0'
        else if ( 0x7F >= character )
            {
            // Character is in normal 7-bit table.
            aOutput.Append( KUnicode[ character ] );
            }
        else
            {
            TFLOGSTRING("TSY: TSatUtility::ConvertSms7ToUnicode16, \
                Character not valid.");
            OstTrace0( TRACE_NORMAL, DUP2_TSATUTILITY_CONVERTSMS7TOUNICODE16, "TSatUtility::ConvertSms7ToUnicode16, Character not valid." );

            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::ConvertUnicode16ToSms7
// Converts unicode16 string to sms 7
// -----------------------------------------------------------------------------
//
void TSatUtility::ConvertUnicode16ToSms7
        (
        TDes8& aOutput,     // sms 7 string
        TDesC16& aInput     // unicode 16
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_CONVERTUNICODE16TOSMS7, "TSatUtility::ConvertUnicode16ToSms7" );
    TFLOGSTRING("TSY: TSatUtility::ConvertUnicode16ToSms7");

    // A hash table would be useful here instead of switch/case
    TInt i( 0 );
    for( i = 0; i < aInput.Length(); i++ )
        {
        switch ( aInput[i] )
            {
             case 0x0040:
                aOutput.Append( 0x00 );     /* @ */
                break;
            case 0x00A3:
                aOutput.Append( 0x01 );     /* £ */
                break;
            case 0x0024:
                aOutput.Append( 0x02 );     /* $ */
                break;
            case 0x00A5:
                aOutput.Append( 0x03 );     /* ¥ */
                break;
            case 0x00E8:
                aOutput.Append( 0x04 );     /* è */
                break;
            case 0x00E9:
                aOutput.Append( 0x05 );     /* é */
                break;
            case 0x00F9:
                aOutput.Append( 0x06 );     /* ù */
                break;
            case 0x00EC:
                aOutput.Append( 0x07 );     /* ì */
                break;
            case 0x00F2:
                aOutput.Append( 0x08 );     /* ò */
                break;
            case 0x00E7:
            case 0x00C7:
                aOutput.Append( 0x09 );     /* Ç */
                break;
            case 0x000A:
                aOutput.Append( 0x0A );     /* LF */
                break;
            case 0x00D8:
                aOutput.Append( 0x0B );     /* Ø */
                break;
            case 0x00F8:
                aOutput.Append( 0x0C );     /* ø */
                break;
            case 0x21B5:
            case 0x000D:
            case 0x2029: //PARAGRAPH SEPARATOR
                aOutput.Append( 0x0D );     /* CR */
                break;
            case 0x00C5:
                aOutput.Append( 0x0E );     /* Å */
                break;
            case 0x00E5:
                aOutput.Append( 0x0F );     /* å */
                break;
            case 0x0394:
                aOutput.Append( 0x10 );     /* Greek: DELTA */
                break;
            case 0x005F:
                aOutput.Append( 0x11 );     /* _ */
                break;
            case 0x03A6:
                aOutput.Append( 0x12 );     /* Greek: PHI */
                break;
            case 0x0393:
                aOutput.Append( 0x13 );     /* Greek: GAMMA */
                break;
            case 0x039B:
                aOutput.Append( 0x14 );     /* Greek: LAMDA */
                break;
            case 0x03A9:
                aOutput.Append( 0x15 );     /* Greek: OMEGA */
                break;
            case 0x03A0:
                aOutput.Append( 0x16 );     /* Greek: PI */
                break;
            case 0x03A8:
                aOutput.Append( 0x17 );     /* Greek: PSI */
                break;
            case 0x03A3:
                aOutput.Append( 0x18 );     /* Greek: SIGMA */
                break;
            case 0x0398:
                aOutput.Append( 0x19 );     /* Greek: THETA */
                break;
            case 0x039E:
                aOutput.Append( 0x1A );     /* Greek: XI */
                break;
            /*0x1B             Extension code */
            case 0x00C6:
                aOutput.Append( 0x1C );     /* Æ */
                break;
            case 0x00E6:
                aOutput.Append( 0x1D );     /* æ */
                break;
            case 0x00DF:
                aOutput.Append( 0x1E );     /* beta */
                break;
            case 0x00C9:
                aOutput.Append( 0x1F );     /* É */
                break;
            /*0x20             SPACE */
            /*0x21 - 0x23:     Unicode and GSM sets are similar */
            case 0x00A4:
                aOutput.Append( 0x24 );
                break;
            /*0x25 - 0x3F:     Unicode and GSM sets are similar */
            case 0x00A1:
                aOutput.Append( 0x40 );     /* ¡ */
                break;
            /*0x41 - 0x5A:     A..Z */
            case 0x00C4:
                aOutput.Append( 0x5B);      /* Ä */
                break;
            case 0x00D6:
                aOutput.Append( 0x5C );     /* Ö */
                break;
            case 0x00D1:
                aOutput.Append( 0x5D );     /* Ñ */
                break;
            case 0x00DC:
                aOutput.Append( 0x5E );     /* Ü */
                break;
            case 0x00A7:
                aOutput.Append( 0x5F );     /* § */
                break;
            case 0x00BF:
                aOutput.Append( 0x60 );     /* ¿ */
                break;
            /*0x61 - 0x7A:     a..z */
            case 0x00E4:
                aOutput.Append( 0x7B );     /* ä */
                break;
            case 0x00F6:
                aOutput.Append( 0x7C );     /* ö */
                break;
            case 0x00F1:
                aOutput.Append( 0x7D );     /* ñ */
                break;
            case 0x00FC:
                aOutput.Append( 0x7E );     /* ü */
                break;
            case 0x00E0:
                aOutput.Append( 0x7F );     /* à */
                break;
/*The characters in the list below can be found from 8bit Unicode set
  but not from GSM character set. Part of them are changed to spaces,
  part of them to the 'nearest' equivalent (by removing the acute/grave/
  tilde/etc. marks.*/
            case 0x00A2:                    /* Unicode: ¢ */
                aOutput.Append( 0x63 );     /* GSM: c */
                break;
            case 0x00DE:                    /* Unicode: Latin capital thorn */
            case 0x00FE:                    /* Unicode: Latin small thorn */
            case 0x00F0:                    /* Unicode: Latin small eth */
            case 0x00F7:                    /* Unicode: Unicode: ÷ */
            case 0x0060:                    /* Unicode: ' */
            case 0x007F:                    /* Unicode: DEL */
                aOutput.Append( 0x20 );     /* GSM: using SPACE */
                break;
            case 0x00C0:                    /* Unicode: À */
            case 0x00C1:                    /* Unicode: Á */
            case 0x00C2:                    /* Unicode: Â */
            case 0x00C3:                    /* Unicode: Ã */
                aOutput.Append( 0x41 );     /* GSM: A */
                break;
            case 0x00C8:                    /* Unicode: È */
            case 0x00CA:                    /* Unicode: Ê */
            case 0x00CB:                    /* Unicode: Ë */
                aOutput.Append( 0x45 );     /* GSM: E */
                break;
            case 0x00CC:                    /* Unicode: Ì */
            case 0x00CD:                    /* Unicode: Í */
            case 0x00CE:                    /* Unicode: Î */
            case 0x00CF:                    /* Unicode: Ï */
                aOutput.Append( 0x49 );     /* GSM: I */
                break;
            case 0x00D0:                    /* Unicode: D with line */
                aOutput.Append( 0x44 );     /* GSM: D */
                break;
            case 0x00D2:                    /* Unicode: Ò */
            case 0x00D3:                    /* Unicode: Ó */
            case 0x00D4:                    /* Unicode: Ô */
            case 0x00D5:                    /* Unicode: Õ */
                aOutput.Append( 0x4F );     /* GSM: O */
                break;
            case 0x00D7:                    /* Unicode: x (multiplication)*/
                aOutput.Append( 0x78 );     /* GSM: x */
                break;
            case 0x00D9:                    /* Unicode: Ù */
            case 0x00DA:                    /* Unicode: Ú */
            case 0x00DB:                    /* Unicode: Û */
                aOutput.Append( 0x55 );     /* GSM: U */
                break;
            case 0x00DD:                    /* Unicode: Ý */
                aOutput.Append( 0x59 );     /* GSM: Y */
                break;
            case 0x00E1:                    /* Unicode: á */
            case 0x00E2:                    /* Unicode: â */
            case 0x00E3:                    /* Unicode: ã */
                aOutput.Append( 0x61 );     /* GSM: a */
                break;
            case 0x00EA:                    /* Unicode: ê */
            case 0x00EB:                    /* Unicode: ë */
                aOutput.Append( 0x65 );     /* GSM: e */
                break;
            case 0x00ED:                    /* Unicode: í */
            case 0x00EE:                    /* Unicode: î */
            case 0x00EF:                    /* Unicode: ï */
                aOutput.Append( 0x69 );     /* GSM: i */
                break;
            case 0x00F3:                    /* Unicode: ó */
            case 0x00F4:                    /* Unicode: ô */
            case 0x00F5:                    /* Unicode: õ */
                aOutput.Append( 0x6F );     /* GSM: o */
                break;
            case 0x00FA:                    /* Unicode: ú */
            case 0x00FB:                    /* Unicode: û */
                aOutput.Append( 0x75 );     /* GSM: u */
                break;
            case 0x00FD:                    /* Unicode: ý */
            case 0x00FF:                    /* Unicode: ÿ */
                aOutput.Append( 0x79 );     /* GSM: y */
                break;
// Conversion table GSM 7-Bit Extension to Unicode
// See 3GPP TS 23.038
            case 0x007B:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x28 );     /* { */
                break;
            case 0x007D:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x29 );     /* } */
                break;
            case 0x005B:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x3C );     /* [ */
                break;
            case 0x007E:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x3D );     /* ~ */
                break;
            case 0x005D:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x3E );     /* ] */
                break;
            case 0x005C:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x2F );     /* \ */
                break;
            case 0x005E:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x14 );     /* ^ */
                break;
            case 0x007C:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x40 );     /* | */
                break;
            case 0x20AC:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x65 );     /* Euro currency symbol */
                break;
            case 0x000C:
                aOutput.Append( 0x1B );     // Extension table
                aOutput.Append( 0x0A );     /* Form feed */
                break;
            default:
                // Similar values in Unicode and GSM sets
                aOutput.Append( TUint8 ( aInput[i] & 0x00FF ) );
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::FillDurationStructure
// Fill in a TDuration structure
// -----------------------------------------------------------------------------
//
void TSatUtility::FillDurationStructure
        (
        CBerTlv& aBerTlv,             // Tlv containing duration data
        RSat::TDuration& aTDuration   // duration structure
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_FILLDURATIONSTRUCTURE, "TSatUtility::FillDurationStructure" );
    TFLOGSTRING("TSY: TSatUtility::FillDurationStructure");

    CTlv duration;
    aTDuration.iTimeUnit = RSat::ENoDurationAvailable;
    TInt returnValue(
        aBerTlv.TlvByTagValue( &duration, KTlvDurationTag ) );
    if ( KErrNotFound != returnValue )
        {
        TUint8 durationTimeUnit = duration.GetShortInfo( ETLV_TimeUnit );
        switch ( durationTimeUnit )
            {
            case KMinutes:
                {
                // Minutes
                aTDuration.iTimeUnit = RSat::EMinutes;
                break;
                }
            case KSeconds:
                {
                // Seconds
                aTDuration.iTimeUnit = RSat::ESeconds;
                break;
                }
            case KTenthsOfSeconds:
                {
                // Tenths of seconds
                aTDuration.iTimeUnit = RSat::ETenthsOfSeconds;
                break;
                }
            default:
                {
                aTDuration.iTimeUnit = RSat::ETimeUnitNotSet;
                }
            }
            // Time interval
            aTDuration.iNumOfUnits = duration.GetShortInfo( ETLV_TimeInteval );
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::FillIconStructure
// Fill in a TIconId structure
// -----------------------------------------------------------------------------
//
void TSatUtility::FillIconStructure
        (
        CBerTlv& aBerTlv,        //tlv data containing icon address data
        RSat::TIconId& aTIconId, //icon id structure
        TInt aItemNmb            //item number
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_FILLICONSTRUCTURE, "TSatUtility::FillIconStructure" );
    TFLOGSTRING("TSY: TSatUtility::FillIconStructure");

    CTlv iconId;
    aTIconId.iQualifier = RSat::ENoIconId;
    TInt returnValue( aBerTlv.TlvByTagValue( &iconId,
                                             KTlvIconIdentifierTag,
                                             aItemNmb ) );
    if ( KErrNotFound != returnValue )
        {
        TUint8 iconQualifier = iconId.GetShortInfo( ETLV_IconQualifier );
        aTIconId.iIdentifier = iconId.GetShortInfo( ETLV_IconIdentifier );
        // The icon qualifier indicates to the ME how the icon is to be used.
        if ( iconQualifier )
            {
            aTIconId.iQualifier = RSat::ENotSelfExplanatory;
            }
        else
            {
            aTIconId.iQualifier = RSat::ESelfExplanatory;
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::Sw1Sw2Check
// Checks status word 1 and status word 2 bytes coming from Sim server
// -----------------------------------------------------------------------------
//
TInt8 TSatUtility::Sw1Sw2Check
        (
        TUint8 aSw1,  //status word 1
        TUint8 aSw2   //status word 2
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_SW1SW2CHECK, "TSatUtility::Sw1Sw2Check" );
    TFLOGSTRING("TSY: TSatUtility::Sw1Sw2Check");

    //These are the ok responses ( for envelope )
    //-90 00 normal ending of the command
    //-91 XX normal ending of the command with extra information from the
    // proactive SIM containing a command for ME length XX of response data
    //-9E XX length XX of the response data given in case of a SIM data
    // download error
    //-9F XX length XX of the response data
    //This is the busy response
    //-93 00 SAT is busy. Command cannot be executed at present, further
    // normal commands are allowed
    //This is the memory status
    //-92 0X command succesful but after using and internal update retry
    // routine x times
    //-92 40 memory problem
    //Application independent errors
    //-67 XX incorrect parameter P3
    //-6B XX incorrect parameter P1 or P2
    //-6E XX wrong instruction class given in the command
    //-6F XX technical problem with no diagnostic given.
    //Also possible
    //-94 02 P1 or P2 is caused by the addressed record being out of range

    TInt8 ret( KError );
    switch ( aSw1 )
        {
        case 0x90:
            {
            if ( 0x00 != aSw2 )
                {
                ret = KError;
                }
            else
                {
                // This means that feature is allowed.
                ret = KAllowed;
                }
            break;
            }
        //This might not be possible ( no sub blocks and
        //sw1 = 91, 9E or 9F )
        case 0x91:
            ret = KAllowed;
            break;
        case 0x9E: // SIM Data Download error
        case 0x62: // USIM Data Download error
        case 0x63: // USIM Data Download error
            ret = KError;
            break;
        case 0x9F:
            ret = KAllowed;
            break;
        case 0x92:
            {
             if ( 0x40 == aSw2  )
                {
                ret = KError;
                }
             else if ( 0 == (aSw2&0xF0) )
                {
                // retry command
                ret = KAllowed;
                }
             else
                {
                ret = KError;
                }
             break;
            }
        //There is no "busy" alternative among possible
        //general results. Therefore this is set to KError.
        case 0x93:
        case 0x67:
        case 0x6B:
        case 0x6E:
        case 0x6F:
        case 0x94:
        default:
            {
            ret = KError;
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// TSatUtility::SetText
// Set Text string as a Unicode text string
// and according to the alphabet used
// -----------------------------------------------------------------------------
//
void TSatUtility::SetText
        (
        CTlv& aTextString, // text string
        TDes& aText        // unicode output text string
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_SETTEXT, "TSatUtility::SetText" );
    TFLOGSTRING("TSY: TSatUtility::SetText");

    if ( aTextString.GetLength() )
        {
        TPtrC8 sourceString;

        sourceString.Set( aTextString.GetData( ETLV_TextString ) );

        TBuf8<KTextBufferMaxSize> string( sourceString );
        // SMS default alphabet DCS
        if ( !( aTextString.GetShortInfo( ETLV_DataCodingScheme )
            & KPacked7BitTextMask ) )
            {
            // Unpack
            Packed7to8Unpacked( sourceString, string );

            if ( aText.MaxLength() < string.Length() )
                {
                string.SetLength( aText.MaxLength() );
                }

            // Convert to unicode format
            ConvertSms7ToUnicode16( aText, string );
            }
        // UCS2 DCS
        else if ( aTextString.GetShortInfo( ETLV_DataCodingScheme )
            & KUCS2DCS )
            {
            TIsiUtility::CopyFromBigEndian( sourceString, aText );
            }
        // 8-bit DCS
        else if ( aTextString.GetShortInfo( ETLV_DataCodingScheme )
            & K8BitDCS )
            {
            // 8-bit string to 16-bit string
            ConvertSms7ToUnicode16( aText, string );
            }
        else // Reserved cases: SMS default alphabet
            {
             // Unpack
            Packed7to8Unpacked( sourceString, string );

            if ( aText.MaxLength() < string.Length() )
                {
                string.SetLength( aText.MaxLength() );
                }
            // Convert to unicode format
            ConvertSms7ToUnicode16( aText, string );
            }
        }
    else
        {
        TFLOGSTRING("TSY: TSatUtility::SetText, text string length not valid");
        OstTrace0( TRACE_NORMAL, DUP1_TSATUTILITY_SETTEXT, "TSatUtility::SetText, text string length not valid" );

        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::ConvertAlphaFieldsToUnicode
// Convert Alpha fields format to Unicode format
// -----------------------------------------------------------------------------
//
void TSatUtility::ConvertAlphaFieldsToUnicode
        (
        TDesC8& aSource,
        TDes& aText
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_CONVERTALPHAFIELDSTOUNICODE, "TSatUtility::ConvertAlphaFieldsToUnicode" );
    TFLOGSTRING("TSY: TSatUtility::ConvertAlphaFieldsToUnicode");

    // ArabicCoding, GreekCoding and TurkishCoding have different coding
    // methods. There is a tag for each type of alphabet (resp. 80, 81 or 82)
    // before the text, and there are base pointers used for expanding 1 byte
    // to 2 bytes as required by UCS2
    // Ref: 3GPP TS 11.11, Annex B

    // Base pointer is a "half-page" in the UCS2 code space
    TUint16 basePointer( 0 );
    // Offset
    TInt x( 0 );

    // needed in case of GreekCoding and TurkishCoding to check
    // that character amount is not bigger than and actual length of
    // the item string.
    TInt maxCharAmount( 0 );

    TInt ret( KErrNone );

    if( aSource.Length() )
        {
        switch ( aSource[0] )
            {
            case KUCS2ArabicCoding:
                {
                // Copy everything after tag byte
                TIsiUtility::CopyFromBigEndian
                    ( aSource.Mid( 1, aSource.Length() - 1 ), aText );
                //Check if any text present
                if ( aText.Length() )
                    {
                    // Remove padding bytes if any
                    if ( 0xFFFF == aText[ aText.Length() - 1 ] )
                        {
                        aText.SetLength( aText.Length() - 1 );
                        }
                    }
                // No expanding needed, already in unicode format.
                ret = KErrNotFound;
                break;
                }
            case KUCS2GreekCoding:
                {
                // Characters starts at position 3
                x = 3;

                // max character amount is total item string len decremented by 3
                // because of 3 first bytes are coding indicator (1 byte), 
                // character amount (1 byte) and base pointer (1 byte).
                maxCharAmount = aSource.Length() - 3; 

                // Base pointer is given in one byte
                // and needs to be sifted 7-bit to left to get correct base pointer
                basePointer = TUint16( aSource[2] << 7 ) ;
                break;
                }
            case KUCS2TurkishCoding:
                {
                // Characters starts at position 4
                x = 4;

                // max character amount is total item string len decremented by 4
                // because of 4 first bytes are coding indicator (1 byte), 
                // character amount (1 byte) and base pointer (2 bytes).
                maxCharAmount = aSource.Length() - 4;

                // Base pointer is two bytes, 16-bit
                GetWord( basePointer, aSource, 2 );
                break;
                }
            default:
                // do nothing
                break;
            }
        }
        else
            {
            TFLOGSTRING("TSY: TSatUtility::ConvertAlphaFieldsToUnicode, Source length NULL ");
            OstTrace0( TRACE_NORMAL, DUP1_TSATUTILITY_CONVERTALPHAFIELDSTOUNICODE, "TSatUtility::ConvertAlphaFieldsToUnicode, Source length NULL" );

            }

    if ( KErrNone == ret && 2 < aSource.Length())
        {
         // The second octet contains a value indicating the number of
         // characters in the string. (Not in the case of Arabic string)
         TInt length( aSource[1] );

         // let's check that character amount inside item string is not
         // bigger than total len of item string
         if( maxCharAmount < length )
             {
             TFLOGSTRING3("TSY: TSatUtility::ConvertAlphaFieldsToUnicode, incorrect char amount: max char amount: %d actual len: %d ", maxCharAmount, length);
             OstTraceExt2( TRACE_NORMAL, DUP3_TSATUTILITY_CONVERTALPHAFIELDSTOUNICODE, "TSatUtility::ConvertAlphaFieldsToUnicode, incorrect char amount: max char amount: %d actual len: %d", maxCharAmount, length );
             length = maxCharAmount;
             }

        // Expanding 1 byte format to 2 bytes
        while ( length )
            {
            // If bit 8 of the octet is set to zero, the remaining 7 bits of
            // the octet contain a GSM Default Alphabet character, whereas if
            // bit 8 of the octet is set to one, the remaining seven bits are
            // an offset value added to the base pointer defined in octets
            // three and four, and the resultant 16 bit value is a UCS2 code
            // point, and defines a UCS2 character.
            if ( 0x7F <= aSource[x] )
                {
                // Append base pointer and offset
                aText.Append( basePointer + ( aSource[x] & 0x7F ) );
                }
            else
                {
                // GSM default alphabet.
                TBuf<1> dest16bit;
                TBuf8<1> src7bit;
                // Get character
                src7bit.Append( aSource[x] );
                // Convert GSM default alphabet character to unicode
                ConvertSms7ToUnicode16( dest16bit, src7bit );
                // Append converted character to output string
                aText.Append( dest16bit );
                }
            x++;
            length--;
            }
        }
    else
        {
        TFLOGSTRING("TSY: TSatUtility::ConvertAlphaFieldsToUnicode, No expanding needed ");
        OstTrace0( TRACE_NORMAL, DUP2_TSATUTILITY_CONVERTALPHAFIELDSTOUNICODE, "TSatUtility::ConvertAlphaFieldsToUnicode, No expanding needed" );

        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::DecodeDcsCbs
// Finds whether the data coding scheme, coded in CBS format,
// is 7-bit, 8-bit or 16-bit. Converts the input DCS from CBS format to SMS
// format. Reference: 3gpp ts 23.038
// -----------------------------------------------------------------------------
//
TSmsDcs TSatUtility::DecodeCbsDcs
        (
        const TUint8 aDcs
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_DECODECBSDCS, "TSatUtility::DecodeCbsDcs" );
    TFLOGSTRING("TSY: TSatUtility::DecodeCbsDcs");

    // Coding group: 4 left most significant bits
    TUint8 codingGroup = (aDcs&0xF0)>>4;
    // lower quartet: 4 right most significant bits
    TUint8 lowQuartet = (aDcs&0x0F);
    // character set: bit 2 and 3, in [bit7...bit0]
    TUint8 characterSet = (aDcs&0x0C)>>2;
    // Initialize output to Unknown or Reserved
    TSmsDcs decodedDcs( ESmsUnknownOrReservedDcs );
    // Switch according to the coding group
    switch ( codingGroup )
        {
        // language specified, usually in 7-bit
        // b0000 or b0010 or b0011
        case 0x00:
        case 0x02:
        case 0x03:
            {
            // 7-bit alphabet
            decodedDcs = ESms7BitDcs;
            break;
            }
        // b0001
        // message preceded by language indication
        case 0x01:
            {

            if ( 0x00 == lowQuartet )
                {
                decodedDcs = ESms7BitDcs;
                }
            else if ( 0x01 == lowQuartet )
                {
                decodedDcs = ESms16BitDcs;
                }
            break;
            }
        // General data coding indication
        // b01xx  or b1001
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x09:
            {
            // The character set determines the alphabet/compression
            if ( 0x00 == characterSet )
                {
                decodedDcs = ESms7BitDcs;
                }
            else if ( 0x01 == characterSet )
                {
                decodedDcs = ESms8BitDcs;
                }
            else if ( 0x02 == characterSet )
                {
                decodedDcs = ESms16BitDcs;
                }
            break;
            }
        // Data coding / Message handling
        // either 8-bit or 7-bit
        case 0x0F:
            {
            // only bit 2 informs about the
            // character set.
            if ( 0x04 & aDcs  )
                {
                decodedDcs = ESms8BitDcs;
                }
            else
                {
                decodedDcs = ESms7BitDcs;
                }
            break;
            }
        default:
            {
            // the DCS value is reserved.
            break;
            }
        }
    return decodedDcs;
    }

// -----------------------------------------------------------------------------
// TSatUtility::GetWord
// Copies unsigned 16-bit word from source descriptor from location pointed
// by index
// -----------------------------------------------------------------------------
//
void TSatUtility::GetWord
        (
        TUint16& aTarget,   //Target
        TDesC8& aSource,    //Source
        TInt aIndex         //Index
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_GETWORD, "TSatUtility::GetWord" );
    if ( aIndex + 1 < aSource.Length() )
        {
        aTarget = TUint16( aSource[aIndex] << 8 );
        aTarget = TUint16( aTarget | aSource[aIndex + 1] );
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::AppendWord
// Copies unsigned 16-bit word to source descriptor
// -----------------------------------------------------------------------------
//
void TSatUtility::AppendWord
        (
        TUint16 aWord,  //Source word
        TDes8& aTarget  //Target
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_APPENDWORD, "TSatUtility::AppendWord" );
    if ( aTarget.MaxLength() >= aTarget.Length() + 2 )
        {
        aTarget.Append( TUint8( aWord >> 8 ) );
        aTarget.Append( TUint8( aWord & 0xff ) );
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::SetUserLoginAndUserPassword
// Set Text string User Login and User Password data
// -----------------------------------------------------------------------------
//
void TSatUtility::SetUserLoginAndUserPassword
        (
        CBerTlv& aBerTlv,
        RSat::TTextString& aUserLogin,
        RSat::TTextString& aUserPassword
        )
    {
    OstTrace0( TRACE_NORMAL, TSATUTILITY_SETUSERLOGINANDUSERPASSWORD, "TSatUtility::SetUserLoginAndUserPassword" );
    TFLOGSTRING("TSY: TSatUtility::SetUserLoginAndUserPassword");

    // Text string - User Login ( Optional )
    CTlv textStringUL;
    TInt returnValue( aBerTlv.TlvByTagValue( &textStringUL,
        KTlvTextStringTag ) );
    // If User Login present
    if ( KErrNone == returnValue )
        {
        TFLOGSTRING("TSY: TSatUtility::SetUserLoginAndUserPassword - User Login");
        OstTrace0( TRACE_NORMAL, DUP1_TSATUTILITY_SETUSERLOGINANDUSERPASSWORD, "TSatUtility::SetUserLoginAndUserPassword - User Login" );

        // Convert and set text
        TSatUtility::SetText( textStringUL, aUserLogin );
        }

    // Text string - User Password ( Optional )
    CTlv textStringUP;
    returnValue = aBerTlv.TlvByTagValue( &textStringUP,
        KTlvTextStringTag, 1 );
    // If User Password present
    if ( KErrNone == returnValue )
        {
        TFLOGSTRING("TSY: TSatUtility::SetUserLoginAndUserPassword - User Password");
        OstTrace0( TRACE_NORMAL, DUP2_TSATUTILITY_SETUSERLOGINANDUSERPASSWORD, "TSatUtility::SetUserLoginAndUserPassword - User Password" );

        // Convert and set text
        TSatUtility::SetText( textStringUP, aUserPassword );
        }
    }


// End of file
