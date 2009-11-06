/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Abstract base class for all SIM ATK TSY tests classes.
*                Provides genenal helper methods to validation of data
*                structures for subclasses.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "satbase.h"
#include "saticon.h"

// TYPE DEFINITIONS
typedef TBuf<300>       TSatRefString;


// CONSTANTS
_LIT( KNullString,      "\"\"");
_LIT( KAlphaId,         "AlphaId");
_LIT( KAddress,         "Address" );
_LIT( KDuration,        "Duration" );
_LIT( KIconId,          "IconId" );
_LIT( KIconQualifier,   "IconQualifier" );
_LIT( KUssdString,      "UssdString" );
_LIT( KDcs,             "Dcs" );
_LIT( KNumOfUnits,      "NumOfUnits" );
_LIT( KTypeOfNumber,    "TypeOfNumber" );
_LIT( KNumberPlan,      "NumberPlan" );
_LIT( KStatus,          "Status" );

#ifndef LEGACY
_LIT( KTextAttribute,   "TextAttribute");
#endif // LEGACY

// ERROR MESSAGES
_LIT( KMissing, "ERROR: Mandatory parameter \"%S\" missing from the script!" );
_LIT( KMismatch, "ERROR: Parameter \"%S\" does not match!" );
_LIT( KExpectedInt, "ERROR: Expected: 0x%x, received: 0x%x" );
_LIT( KExpectedStr, "ERROR: Expected: %S, received: %S" );

// Constants used when parsing hex value from ascii string
_LIT( KHexTag,              "\\x"); // Tag for hex value within ascii string

const TUint KKNumHexTag     ='x';   // Tag for hex values when handling numbers
const TUint KMaxHexDigits   = 0x04; // Max number of digits allowed 


// -----------------------------------------------------------------------------
// CSatBase::CSatBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatBase::CSatBase
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : iSat( aSat ), iLog( aLog ), iIpc ( aIpc ), iParser( aParser )
    {
    }
    

// -----------------------------------------------------------------------------
// CSatBase::~CSatBase
// Destructor
// -----------------------------------------------------------------------------
//
CSatBase::~CSatBase()
    {
    }
    

// -----------------------------------------------------------------------------
// CSatBase::CancelRequest
// Cancels pending request
// -----------------------------------------------------------------------------
//
void CSatBase::CancelRequest()
    {
    iSat.CancelAsyncRequest( iIpc );
    }    


// -----------------------------------------------------------------------------
// CSatBase::MatchIpc
// Checks if the provided IPC matches with current command's Ipc
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TBool CSatBase::MatchIpc
        (
        TSatIpc aRequestedIpc
        )
    {
    return iIpc == aRequestedIpc ? ETrue : EFalse;
    }    

    

// -----------------------------------------------------------------------------
// CSatBase::CheckAddress
// Verifies the validity of address structure
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckAddress
        (
        const RSat::TAddress& aAddress
        )
    {
    iLog.Log( _L( "CSatBase::CheckAddress" ) );
    
    TInt ret ( KErrNone );

    RSat::TAddress scriptedAddress;
    
    if ( KErrNone == GetAddress( scriptedAddress ) )
        {
        // Check number first
        if (  aAddress.iTelNumber != scriptedAddress.iTelNumber )
            {
            iLog.Log( KMismatch, &KAddress );
            iLog.Log( KExpectedStr, &scriptedAddress.iTelNumber,
                &aAddress.iTelNumber );

            ret = KErrCorrupt;
            }
            
        // Check TON
        if ( aAddress.iTypeOfNumber != scriptedAddress.iTypeOfNumber )
            {
            iLog.Log( KMismatch, &KTypeOfNumber );
            iLog.Log( KExpectedInt, scriptedAddress.iTypeOfNumber,
                aAddress.iTypeOfNumber );

            ret = KErrCorrupt;
            }            

        // Check NPI
        if ( aAddress.iNumberPlan != scriptedAddress.iNumberPlan )
            {
            iLog.Log( KMismatch, &KNumberPlan );
            iLog.Log( KExpectedInt, scriptedAddress.iNumberPlan,
                aAddress.iNumberPlan );

            ret = KErrCorrupt;
            }            
        }
    else
        {
        iLog.Log( KMissing, &KAddress );
        ret = KErrNotFound;
        }

    return ret;
    }    


// -----------------------------------------------------------------------------
// CSatBase::CheckAlphaId
// Verifies the validity of alpha id
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckAlphaId
        (
        const RSat::TAlphaId&   aAlphaId,
        const TDesC&            aAlphaIdTag
        )
    {
    iLog.Log( _L( "CSatBase::CheckAlphaId" ) );
    
    TInt ret ( KErrNone );
    
    // Wether to use default alpha id tag "AlphaId" or not.
    TBool useDefaultTag( aAlphaIdTag.Length() ? EFalse : ETrue );    
    
    // Set scripted defaults
    RSat::TAlphaId scriptedAlphaId;

    // Try to parse AlphaId from script
    if ( KErrNone != GetText( useDefaultTag ? KAlphaId : aAlphaIdTag, 
            scriptedAlphaId.iAlphaId ) )
        {
        // AlphaId tag was missing from the script
        scriptedAlphaId.iStatus = RSat::EAlphaIdNotPresent;
        }
    else if ( 0 == scriptedAlphaId.iAlphaId.Compare( KNullDesC ) )
        {
        // AlphaId tag found, but content is empty (AlphaId "")
        scriptedAlphaId.iStatus = RSat::EAlphaIdNull;
        }
    else
        {
        // Tag is present and sontain some data
        scriptedAlphaId.iStatus = RSat::EAlphaIdProvided;
        }
    
    // Verify AlphaId's status
    if ( scriptedAlphaId.iStatus != aAlphaId.iStatus )
        {
        iLog.Log( KMismatch, &KStatus );
        iLog.Log( KExpectedInt, scriptedAlphaId.iStatus,
            aAlphaId.iStatus );

        ret = KErrCorrupt;
        }

    // Verify AlphaId
    if ( scriptedAlphaId.iAlphaId != aAlphaId.iAlphaId )
        {
        iLog.Log( KMismatch, &KAlphaId );
        iLog.Log( KExpectedStr, &scriptedAlphaId.iAlphaId,
            &aAlphaId.iAlphaId );

        ret = KErrCorrupt;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::CheckDuration
// Verifies the validity of duration structure
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckDuration
        (
        const RSat::TDuration& aDuration
        )
    {
    iLog.Log( _L( "CSatBase::CheckDuration" ) );
    
    TInt ret ( KErrNone );
    
    TInt number( RSat::ENoDurationAvailable ); // Default
    
    // Get scripted value if neede and available
    if ( RSat::ENoDurationAvailable != aDuration.iTimeUnit 
        && KErrNone != GetValue( KDuration, number ) )
        {
        iLog.Log( KMissing, &KDuration );
        ret = KErrNotFound;
        }
    else
        {
        // Time unit
        if ( number != aDuration.iTimeUnit )
            {
            iLog.Log( KMismatch, &KDuration );
            iLog.Log( KExpectedInt, number, aDuration.iTimeUnit );

            ret = KErrCorrupt;
            }

        // Number of units 
        if ( RSat::ENoDurationAvailable != aDuration.iTimeUnit )
            {
            if ( KErrNone == GetNextValue( number ) )
                {
                if ( number != aDuration.iNumOfUnits )
                    {
                    iLog.Log( KMismatch, &KNumOfUnits );
                    iLog.Log( KExpectedInt, number, aDuration.iNumOfUnits );

                    ret = KErrCorrupt;
                    }
                }
            else
                {
                // Time unit parameter was found but NumOfUnits is missing
                iLog.Log( KMissing, &KNumOfUnits );
                ret = KErrNotFound;
                }
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::CheckIcon
// Handles the Icons provided by proactive command
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckIcon
        (
        const RSat::TIconId& aIconId,
        const TDesC& aIconTag
        )
    {
    iLog.Log( _L( "CSatBase::CheckIcon" ) );
    
    TInt ret( KErrNone );
    
    // Wether to use default icon tag or special one.
    TPtrC iconTag( aIconTag.Length() ? aIconTag : KIconId );
    
    TInt number( RSat::ENoIconId ); // Default

    // Get scripted value if needed and available
    if ( RSat::ENoIconId != aIconId.iQualifier
        && KErrNone != GetValue( iconTag, number ) )
        {
        iLog.Log( KMissing, &KIconId );
        ret = KErrNotFound;
        }
    else
        {
        // Qualifier
        if ( number != aIconId.iQualifier )
            {
            iLog.Log( KMismatch, &KIconQualifier );
            iLog.Log( KExpectedInt, number, aIconId.iQualifier );

            ret = KErrCorrupt;
            }
            
        // Identifier
        if ( RSat::ENoIconId != aIconId.iQualifier )
            {
            if ( KErrNone == GetNextValue( number ) )
                {
                if ( number != aIconId.iIdentifier )
                    {
                    iLog.Log( KMismatch, &iconTag );
                    iLog.Log( KExpectedInt, number, aIconId.iIdentifier );

                    ret = KErrCorrupt;
                    }
                }
            else
                {
                // Qualifier parameter was found but Id is missing
                iLog.Log( KMissing, &iconTag );
                ret = KErrNotFound;
                }
            }
        }
        
    // If successful so far and image should be displayed, proceed with
    // image loading.
    if ( KErrNone == ret && aIconId.iQualifier != RSat::ENoIconId )
        {
        // Try to load mandatory SimType parameter from script
        TPtrC simType;
        
        if ( KErrNone == GetText( KLoadIcon, simType ) )
            {
            CSatIcon satIcon( iSat, simType, aIconId.iIdentifier  );
            
            // Get optional instance number identifier
            TInt instanceNumber( 1 );
            GetNextValue( instanceNumber );
            
            ret = satIcon.CheckIcon( &iLog, instanceNumber );
            }
        }
    
    return ret;
    }    


// -----------------------------------------------------------------------------
// CSatBase::TextAttribute
// Handles the verification of TextAttribute provided by proactive command
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
#ifndef LEGACY
TInt CSatBase::CheckTextAttribute
        (
        const RSat::TTextAttribute& aTa
        )
    {
    iLog.Log( _L( "CSatBase::CheckTextAttribute" ) );
    
    TInt ret( KErrNone );
    
    TPtrC test;
    if ( KErrNone == GetText( KTextAttribute, test ) )
        {
        // Scripts expect TA. Status should be set appropriately.
        if ( aTa.iStatus != RSat::ETextAttributeProvided )
            {
            iLog.Log( KMismatch, &KStatus );
            iLog.Log( KExpectedInt, RSat::ETextAttributeProvided,
                aTa.iStatus );

            ret = KErrCorrupt;
            }
        else
            {
            // Status Ok, check content
            ret = CheckBuffer( KTextAttribute, aTa.iTextAttributeData );
                
            if ( KErrNone != ret )
                {
                iLog.Log( _L( "CSatBase::CheckTextAttribute - ERROR\
                    TextAttribute contect validation failed!" ) );
                }
            }
        }
    else
        {
        // Not provided by the script. Check TA status
        switch ( aTa.iStatus )
            {
            case RSat::ETextAttributeNotSet:
                {
                iLog.Log( _L( "CSatBase::CheckTextAttribute - Warning\
                    TextAttribute in construction default." ) );
                break;
                }
            case RSat::ETextAttributeProvided:
                {
                iLog.Log( KMissing, &KTextAttribute );
                ret = KErrArgument;
                break;
                }
            case RSat::ETextAttributeNotPresent:
                {
                if ( aTa.iTextAttributeData.Length() )
                    {
                    iLog.Log( _L( "CSatBase::CheckTextAttribute - ERROR\
                        TextAttribute contains data (should be empty)!" ) );
                    ret = KErrCorrupt;
                    }
                break;
                }
            default:
                {
                iLog.Log( _L( "CSatBase::CheckTextAttribute - ERROR\
                    Unexpected TextAttribute value: %d" ), aTa.iStatus );
                ret = KErrCorrupt;
                break;
                }
            }
        }
    
    return ret;
    }
#endif // LEGACY


// -----------------------------------------------------------------------------
// CSatBase::CheckBuffer
// Verifies the validity of received binary data.
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckBuffer
        (
        const TDesC&    aTag,
        const TDesC&    aData
        )
    {
    iLog.Log( _L( "CSatBase::CheckBuffer:  \"%S\"" ), &aTag );
    
    TInt ret ( KErrNone );
    TUint length( aData.Length() );
    
    TSatRefString refBuffer;
    if ( KErrNone == GetBuffer( aTag, refBuffer ) )
        {
        // Check lenght
        if ( length != refBuffer.Length() )
            {
            iLog.Log( KMismatch, &aTag );
            iLog.Log( _L( "Data length does not match!" ) );
            iLog.Log( KExpectedInt, refBuffer.Length(), length );

            ret = KErrCorrupt;
            }
        else
            {
            // Start validating the values
            for ( TInt i( 0 ) ; i < length ; i++ )
                {
                if ( refBuffer[i] != aData[i] )
                    {
                    iLog.Log( KMismatch, &aTag );
                    iLog.Log( _L( "Value mismatch at index %d:" ), i );
                    iLog.Log( KExpectedInt, refBuffer[i], aData[i] );
                        
                    ret = KErrCorrupt;
                    }                
                }
            }
        }
    else if ( length )
        {
        iLog.Log( KMissing, &aTag );
        ret = KErrNotFound;
        }

    return ret;
    }
    

// -----------------------------------------------------------------------------
// CSatBase::CheckBuffer
// Verifies the content of provided 8-bit data against scripted one
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckBuffer
        (
        const TDesC&    aTag,
        const TDesC8&   aData
        )
    {
    TInt ret( KErrNone );
    
    TSatRefString buffer16;
    
    if ( aData.Length() > buffer16.MaxLength() )
        {
        ret = KErrOverflow;
        }
    else
        {
        buffer16.Copy( aData );
        ret = CheckBuffer( aTag, buffer16 );
        }
        
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::CheckText
// Verifies the content of provided string against scripted one
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckText
        (
        const TDesC&    aTag,
        const TDesC&    aData
        )
    {
    iLog.Log( _L( "CSatBase::CheckText: \"%S\"" ), &aTag );

    TInt ret( KErrNone );
    
    TSatRefString refString;
    
    if ( KErrNone != GetText( aTag, refString ) && aData.Length() )
        {
        iLog.Log( KMissing, &aTag );
        ret = KErrNotFound;
        }
    else if ( aData != refString )
        {
        iLog.Log( KMismatch, &aTag );
        iLog.Log( KExpectedStr, &refString, &aData );
        
        ret = KErrCorrupt;
        }

    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSatBase::CheckText
// Verifies the content of provided 8-bit string against scripted one
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckText
        (
        const TDesC&    aTag,
        const TDesC8&   aData
        )
    {
    TInt ret( KErrNone );
    
    TSatRefString buffer16;
    
    if ( aData.Length() > buffer16.MaxLength() )
        {
        ret = KErrOverflow;
        }
    else
        {
        buffer16.Copy( aData );
        ret = CheckText( aTag, buffer16 );
        }
        
    return ret; 
    }
    
    
// -----------------------------------------------------------------------------
// CSatBase::CheckValue
// Verifies the value of provided number against scripted one
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckValue
        (
        const TDesC&    aTag,
        const TInt&     aValue,
        const TInt      aDefault
        )
    {
    iLog.Log( _L( "CSatBase::CheckValue: \"%S\"" ), &aTag );
    
    TInt ret( KErrNone );
    
    TInt refValue( aDefault );
    
    // If no default value is provided, we shoud find one from the script
    if ( KErrNone != GetValue( aTag, refValue ) && KMaxTInt == aDefault )
        {
        iLog.Log( KMissing, &aTag );
        ret = KErrNotFound;
        }
    else if ( aValue != refValue )
        {
        iLog.Log( KMismatch, &aTag );
        iLog.Log( KExpectedInt, refValue, aValue );

        ret = KErrCorrupt;
        }
    
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::CheckNextValue
// Verifies the value of provided number against next scripted number. 
// CheckValue has to be used first to find proper location in the script.
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::CheckNextValue
        (
        const TInt&     aValue,
        const TInt      aDefault
        )
    {
    iLog.Log( _L( "CSatBase::CheckNextValue: 0x%x" ), aValue );
        
    TInt ret( KErrNone );
    
    TInt refValue( aDefault );
    
    // If no default value is provided, we shoud find one from the script
    if ( KErrNone != GetNextValue( refValue ) && aDefault == KMaxTInt )
        {
        ret = KErrNotFound;
        }
    else if ( aValue != refValue )
        {
        iLog.Log( _L( "CSatBase::CheckNextValue: ERROR:" ) );
        iLog.Log( KExpectedInt, refValue, aValue );

        ret = KErrCorrupt;
        }
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatBase::GetAddress
// Parses address struct from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::GetAddress
        (
        RSat::TAddress& aAddress
        )
    {
    TInt ret( KErrNone );
    
    if ( KErrNone != GetText( KAddress, aAddress.iTelNumber ) )
        {
        ret = KErrNotFound;
        }
    else
        {
        // Get TON. Use default if not provided by the script
        TInt number( RSat::EUnknownNumber ); // Default
        GetNextValue( number );
        aAddress.iTypeOfNumber = static_cast<RSat::TTypeOfNumber>( number );

        // Get NPI. Use default if not provided by the script
        number = RSat::EUnknownNumberingPlan;
        GetNextValue( number );
        aAddress.iNumberPlan = static_cast<RSat::TNumberingPlan>( number );
        }

    return ret;    
    }


// -----------------------------------------------------------------------------
// CSatBase::GetValue
// Parses hex value from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//    
TInt CSatBase::GetValue
        (
        const TDesC&    aTag,
        TInt&           aValue,
        TRadix          aRadix
        )
    {
    TInt ret( KErrNone );
    
    TPtrC numberAsString;
    
    iParser.SetParsingType( CStifItemParser::ENormalParsing );
    
    if ( KErrNone == iParser.GetString( aTag, numberAsString ) )
        {
        ret = ConvertToNumber( numberAsString, aRadix , aValue);
        }
    else
        {
        ret = KErrNotFound;
        }
    
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::GetNextValue
// Parses next hex value from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//  
TInt CSatBase::GetNextValue
        (
        TInt&   aValue,
        TRadix  aRadix
        )
    {
    TInt ret( KErrNone );
    
    iParser.SetParsingType( CStifItemParser::ENormalParsing );
        
    TPtrC nextNumberAsString;
    if ( KErrNone == iParser.GetNextString( nextNumberAsString ) )
        {
        ret = ConvertToNumber( nextNumberAsString, aRadix, aValue );
        }
    else
        {
        // Next number not found
        ret = KErrNotFound;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::GetText
// Parses text string from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::GetText
        (
        const TDesC&    aTag,
        TDes&           aText
        )
    {
    TInt ret( KErrNone );
    TPtrC text;

    aText.Zero();
    
    // First check if text is given as unicode
    if ( KErrNone == GetBuffer( aTag, aText ) )
        {
        // Was unicode. No additional actions needed.
        }
    // No unicode. Parse text between the quotes
    else if ( KErrNone == GetText( aTag, text ) )
        {
        if ( text.Length() )
            {
            // Normal string received                    
            TSatRefString parsed;
            
            if ( text.Length() > parsed.MaxLength() )
                {
                // Too long data parsed.
                iLog.Log( _L( "CSatBase::GetText Script text too long!" ) );
                ret = KErrCorrupt;                       
                }
            else
                {
                // It fits
                parsed.Append( text );
                
                // Check and convert hex markers if found
                ConvertHexTags( parsed );
                
                if ( parsed.Length() > aText.MaxLength() )
                    {
                    // Too long data parsed.
                    iLog.Log( _L( "CSatBase::GetText Read text won't fit!" ) );
                    ret = KErrCorrupt;                       
                    }
                else
                    {
                    // It fits
                    aText.Append( parsed );                            
                    }
                }
            }
        }
    else    // No requested text tag found
        {
        ret = KErrNotFound;
        }
    
    return ret;
    }
    

// -----------------------------------------------------------------------------
// CSatBase::GetText
// Parses text string from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::GetText
        (
        const TDesC&    aTag,
        TPtrC&          aText,
        TBool           aGetNext
        )
    {
    TInt ret( KErrNone );
    
    // Do not include quotes.            
    iParser.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    if ( aGetNext )
        {
        ret = iParser.GetNextString( aTag, aText );
        }
    else
        {
        ret = iParser.GetString( aTag, aText );
        }

    if ( KErrNone == ret )
        {
        // If scripted string contains a zero length string (""), 
        // GetString-method  does not exclude quotes and 
        // asciiText contains a string '""' and length is 2.
        if ( KErrNone == aText.Compare( KNullString ) )
            {
            // The string is '""'. Append zero lenght string instead.
            aText.Set( KNullDesC );
            }
        }
    else    // No recognizeable text tags found
        {
        ret = KErrNotFound;
        }
    
    return ret;
    }
    

// -----------------------------------------------------------------------------
// CSatBase::GetNextText
// Parses next text string with given tag from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatBase::GetNextText
        (
        const TDesC&    aTag,
        TPtrC&          aText
        )
    {
    return GetText( aTag, aText, ETrue );
    }    
    
    
// -----------------------------------------------------------------------------
// CSatBase::GetUssdString
// Parses UssdString structure from the script
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatBase::GetUssdString
        (
        RSat::TUssdString&      aUssdString
        )
    {
    TInt ret( KErrNone );

    if ( KErrNone == GetText( KUssdString, aUssdString.iUssdString ) )
        {
         // Parse DCS. If parsing failed, number already contain default
         // value
        TUint dcs( 0 );
        GetValue( KDcs, dcs );
        aUssdString.iDcs = dcs;
        }
    else
        {
        iLog.Log( KMissing, &KUssdString );
        ret = KErrNotFound;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::ConvertHexTags
// Converts '\x'-tagged hex values to hex numbers within ascii buffer
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
void  CSatBase::ConvertHexTags
        (
        TDes&       aString
        )
    {
    TInt    tagStart( 0 );      // Start index of hex-tag in string
    TInt    numberStart( 0 );   // Start index of hex number in string
    TPtrC   string;             // String presentation of value
    TInt    hex( 0 );           // Parsed hex value
    
    // Allow only 16-bit number that are presented by two digits in the string
    TUint8  numDigits( KMaxHexDigits / 2);
    
    while ( KErrNotFound != ( tagStart = aString.Find( KHexTag ) ) )
        {
        numberStart = KHexTag.iTypeLength + tagStart;

        // Check boundary
        if ( aString.Length() >= numberStart + numDigits )
            {
            string.Set( aString.Mid( numberStart, numDigits ) );
            
            if ( KErrNone == ConvertToNumber( string, EHex, hex ) )
                {
                // Delete tag + number. Leave one byte for replacing
                aString.Delete( tagStart, 
                    KHexTag.iTypeLength + numDigits - 1 );
                aString[tagStart] = hex;
                }
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CSatBase::ConvertToNumber
// Converts a string presentation of value to a integer presentation
// (detailed description in header file)
// -----------------------------------------------------------------------------
// 
TInt CSatBase::ConvertToNumber
        (
        TPtrC&      aNumberAsString,
        TRadix      aRadix,
        TInt&       aValue
        )
    {
    TInt ret( KErrNone );
    
    // Override radix if number contain hex tag
    TInt hexStartPos( aNumberAsString.Locate( KKNumHexTag ) );
    if ( KErrNotFound != hexStartPos )
        {
        aRadix = EHex;
        aNumberAsString.Set( aNumberAsString.Mid( hexStartPos + 1 ) );
        }
        
    TLex lex( aNumberAsString );    
    
    if ( EDecimal == aRadix )
        {
        ret = lex.Val( aValue );
        }
    // Number is in hex format
    else if ( KMaxHexDigits >= aNumberAsString.Length() )
        {
        // Lenght ok (max 4 digits)
        TUint hex( 0 ); // Can't be negative
        ret = lex.Val( hex, aRadix );
        aValue = hex;
        }
    else
        {
        // Too long hex number
        ret = KErrArgument;
        }
        
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatBase::GetBuffer
// Reads byte data from script to a buffer
// (detailed description in header file)
// -----------------------------------------------------------------------------
// 
TInt CSatBase::GetBuffer
        (
        const TDesC&    aTag,
        TDes&           aBuffer
        )
    {
    TInt ret( KErrNone );
    
    // Read bytes until non readable value is reached or buffer limit is reached
    TInt byte( 0 );
    
    aBuffer.Zero();
    
    if ( KErrNone == GetValue( aTag, byte, EHex ) )
        {
        do
            {
            // Append parsed unicodeCharacter
            if ( aBuffer.Length() < aBuffer.MaxLength() )
                {
                aBuffer.Append( byte );
                }
            else
                {
                iLog.Log( _L( "CSatBase::GetBuffer: ERROR: \
                    Output buffer ran out of space" ) );
                ret = KErrCorrupt;
                }
            
            } while( KErrNone == ret && KErrNone == GetNextValue(
                byte, EHex ) );
        }
    else
        {
        ret = KErrNotFound;
        }
        
    return ret;
    }

    
// End of File    
