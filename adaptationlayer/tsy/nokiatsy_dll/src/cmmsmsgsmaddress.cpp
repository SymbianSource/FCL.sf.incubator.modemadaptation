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
#include "cmmsmsgsmaddress.h"
#include <ctsy/serviceapi/cmmsmsutility.h> // for kmaxamountofdigits
#include "tsylogger.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsmsgsmaddresstraces.h"
#endif
      // for logging purposes

//  External Data Structures  

//  External Function Prototypes  

//  LOCAL CONSTANTS AND MACROS 
const TUint8 KAddressMinLength = 2;
const TUint8 KAddressLengthAndTon = 2;
const TUint8 KDataByteInUnicode = 2;
const TUint8 KHalfByte = 4;
const TUint8 KLengthOfByte = 8;
const TUint8 KLengthOfPackedByte = 7;
const TUint8 KTwoCharInByte = 2;

//  MODULE DATA STRUCTURES  

//  Local Data Structures  

//  Local Function Prototypes  

// Converts semioct-code to character, semioctet can be 0x00 - 0x0F
#define GSMSMSSEMIOCTETTOCHAR( bcd )  gsmSmsSemiOctetToCharTable[ bcd ]

//  MEMBER FUNCTIONS

//=============================================================================

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::CMmSmsGsmAddress
// C++ constructor. Private (this class should not be instantiated).
// ----------------------------------------------------------------------------- 
// 
CMmSmsGsmAddress::CMmSmsGsmAddress
        (
        //None
        )
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::CMmSmsGsmAddress");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_CMMSMSGSMADDRESS, "CMmSmsGsmAddress::CMmSmsGsmAddress" );
    //none
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmConvUnicodeTo0340Addr
// Converts type of number, numbering plan identification and
// phone number given in unicode string to the Gsm 03.40 
// standard
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmConvUnicodeTo0340Addr
        (
        TUint8 aTon,            //Type of number
        TUint8 aNpi,            //Numbering plan identification
        TDes8& aDest,           //Service center address
        TDesC16 const& aSource  //Telephone number
        )    
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmConvUnicodeTo0340Addr");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMCONVUNICODETO0340ADDR, "CMmSmsGsmAddress::GsmConvUnicodeTo0340Addr" );
    
    if ( 0 != aSource.Length() )
        {
        GsmConvUnicodeToAddr( aTon, aNpi, aDest, aSource, ETrue );  
        }
    else
        {
        // Address length of 0 is a special case
        aDest.Append( 0 );      

        // Type-of-Address: TON = Unknown, NPI = ISDN/telephone num plan
        aDest.Append( 0x81 );  
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmConv0340AddrToUnicode
// This function converts an address format string specified in 
// GSM 03.40 into unicode string
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmConv0340AddrToUnicode
        ( 
        TDes& aDest,                            //Service center address
        TDesC8 const& aSource,                  //Telephone number
        RMobilePhone::TMobileTON& aTon,         //Type of number
        RMobilePhone::TMobileNPI& aNpi          //Telephone number
        )
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmConv0340AddrToUnicode");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMCONV0340ADDRTOUNICODE, "CMmSmsGsmAddress::GsmConv0340AddrToUnicode" );
    
    if ( KAddressMinLength <= aSource.Length() ) 
        {
        //Address length and address type fields included
        GsmConvAddrToUnicode( aDest, aSource, aTon, aNpi, ETrue );
        }
    else
        {
        //Corrupted address
        aDest.SetLength( 0 );
        aTon = RMobilePhone::EUnknownNumber;
        aNpi = RMobilePhone::EUnknownNumberingPlan;
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmConvUnicodeTo0411Addr
// Converts type of number, numbering plan identification and
// phone number given in unicode string to the Gsm 04.11 
// standard
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmConvUnicodeTo0411Addr
        (
        TUint8 aTon,            //Type of number
        TUint8 aNpi,            //Numbering plan identification
        TDes8& aDest,           //Service center address
        TDesC16 const& aSource  //Telephone number
        )
    { 
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmConvUnicodeTo0411Addr");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMCONVUNICODETO0411ADDR, "CMmSmsGsmAddress::GsmConvUnicodeTo0411Addr" );
    
    if ( 0 != aSource.Length() )
        {
        GsmConvUnicodeToAddr( aTon, aNpi, aDest, aSource, EFalse );
        }
    else
        {
        // Address length of 0 is a special case: no address type is given.
        aDest.Append( 0 );
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmConv0411AddrToUnicode
// This function converts an address format string specified in 
// GSM 04.11 into unicode string
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmConv0411AddrToUnicode
        (
        TDes& aDest,                        //Destination
        TDesC8 const& aSource,              //Source
        RMobilePhone::TMobileTON& aTon,     //Type of number
        RMobilePhone::TMobileNPI& aNpi      //Numbering plan identification
        )
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmConv0411AddrToUnicode");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMCONV0411ADDRTOUNICODE, "CMmSmsGsmAddress::GsmConv0411AddrToUnicode" );
    
    if ( KAddressMinLength <= aSource.Length() )
        {
        //Address length and address type fields included
        GsmConvAddrToUnicode( aDest, aSource, aTon, aNpi, EFalse );
        }
    else
        {
        //No address type field
        aDest.SetLength( 0 );
        aTon = RMobilePhone::EUnknownNumber;
        aNpi = RMobilePhone::EUnknownNumberingPlan;
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmConvUnicodeToAddr
// Converts type of number, numbering plan identification and
// phone number given in unicode string to the GSM 03.40 or
// 04.11 standard
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmConvUnicodeToAddr
        (
        TUint8 aTon,             //Type of number
        TUint8 aNpi,             //Numbering plan identification
        TDes8& aDest,            //Service center address
        TDesC16 const& aSource,  //Telephone number
        TBool a0340Address       //GSM 03.40 address
        )
    { 
TFLOGSTRING4("TSY: CMmSmsGsmAddress::GsmConvUnicodeToAddr - number type: %d, numbering plan: %d, number: %S", aTon, aNpi, &aSource);
OstTraceExt3( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMCONVUNICODETOADDR, "CMmSmsGsmAddress::GsmConvUnicodeToAddr;aTon=%hhu;aNpi=%hhu;aSource=%S", aTon, aNpi, aSource );
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmConvUnicodeToAddr - GSM 03.40 address: %d", a0340Address);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSMSGSMADDRESS_GSMCONVUNICODETOADDR, "CMmSmsGsmAddress::GsmConvUnicodeToAddr;a0340Address=%hhu", a0340Address );
    
    //get address length (always < 255)
    TUint8 numberLength( static_cast< TUint8 >( aSource.Length() ) );
    TUint8 destLength( numberLength );
    TInt i( 0 );

    /*  Solve type of address
        Type of Address field format is as follows:

           7      6     5     4      3      2       1       0    bit
        |     |      |     |     |      |       |       |       |
        |  1  | Type of number   | Numbering-plan-identification|
        |     |      |     |     |      |       |       |       |
    */

    //type of number is three bits from left and fourth bit must be '1'
    TUint8 typeOfNumber( TUint8( aTon | 0x08 ) );
    //move four bit to the left
    typeOfNumber = TUint8( typeOfNumber << 4 );
    //numbering plan is four bits from left
    TUint8 numberingPlan( TUint8( aNpi & 0x0F ) );
    //set type of number and numbering plan to the typeOfNumber variable
    typeOfNumber = TUint8( typeOfNumber | numberingPlan );

    if ( RMobilePhone::EAlphanumericNumber == aTon )
        {
        TBuf8< KMaxLengthOfAddressData > addressData;
        TUint8 amountOfsemiOctets( 0 );
  
        GsmUnicodeAddrToAlphanumericAddr( 
            aSource, 
            addressData,
            amountOfsemiOctets );           

        //fill address length field
        if ( a0340Address )
            {
            //amount of used semi-octets in address data
            aDest.Append( amountOfsemiOctets );
            }
        else    //0411 address
            {
            //Actually alphanumeric address data for 0411 address is not 
            //supported (see GSM 24.011 and 24.008)...
            //Length of address data + length of address type field
            //lint -e{732} Warning about "loss of sign". 
            //Root cause is that TDesC::Length() returns a signed TInt
            aDest.Append( addressData.Length() + 1 );
            }

        //add typeOfNumber to the buffer
        aDest.Append( typeOfNumber );

        //add address data to the buffer. 
        aDest.Insert( KAddressLengthAndTon, addressData );  
        }
    else
        {
        //number can contain character '+'
        if ( '+' == aSource[ i ] )
            {
            i++;
            destLength--;

            // Maximum amount of digits is 20 (3GPP TS 23.040)
            if ( KMaxAmountOfDigits < destLength ) 
                {                                                   
                destLength = KMaxAmountOfDigits;
                // + 1 comes from '+'
                numberLength = KMaxAmountOfDigits + 1;
                }
            }
        else
            {
            // Maximum amount of digits is 20 (3GPP TS 23.040)
            if ( KMaxAmountOfDigits < destLength ) 
                {                                                   
                destLength = KMaxAmountOfDigits;
                numberLength = KMaxAmountOfDigits;
                }
            }

        // calculate address length in bytes (always < 255). 
        // destLength + 1 ensures that there is enough space in case of odd
        // destLength. 
        TUint8 bufferLength( static_cast< TUint8 >( 
            ( destLength + 1 ) / 2 + KAddressLengthAndTon ) );

        //set buffer length
        aDest.SetLength( bufferLength );

        //add length to the buffer
        if ( a0340Address )
            {
            //amount of used semi-octets in address data
            aDest[ 0 ] = destLength;   
            }
        else
            {
            //length of whole address data - length of address length field
            aDest[ 0 ] = static_cast< TUint8 >( bufferLength - 1 );
            }

        //add typeOfNumber to the buffer
        aDest[1] = typeOfNumber; 

        //needed variables
        TUint8 numLsb( 0 );
        TUint8 numMsb( 0 );
        TInt index( KAddressLengthAndTon );

        //add numbers to the buffer
        for ( ; ( i + 1 ) < numberLength ; i += 2 )
            {
            numLsb = TUint8( aSource[ i ] - '0' );
            numMsb = TUint8( static_cast< TUint8 >( 
                aSource[ i + 1 ] - '0' ) << KHalfByte );

            aDest[index++] = TUint8( numLsb | numMsb );
            }

        //if numberLength is odd, last number is fixed with 0xF0
        if ( i < numberLength )
            {
            aDest[ index ] = TUint8( 0xF0 | ( aSource[ i ] - '0' ) );
            }
        }  
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmConvAddrToUnicode
// Converts GSM 03.40 or 04.11 address to type of number, 
// numbering plan identification and phone number given in 
// unicode string
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmConvAddrToUnicode
        ( 
        TDes& aDest,                            //Service center address
        TDesC8 const& aSource,                  //Telephone number
        RMobilePhone::TMobileTON& aTon,         //Type of number
        RMobilePhone::TMobileNPI& aNpi,         //Telephone number
        TBool a0340Address                      //GSM 03.40 address
        )
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmConvAddrToUnicode");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMCONVADDRTOUNICODE, "CMmSmsGsmAddress::GsmConvAddrToUnicode" );
    
    TUint8  addrDataLength( 0 );
    TBool   halfs( ETrue );
    TInt    index( 0 );               
    TUint   num2( 0 );

    //Get amount of digits in address data
    if ( a0340Address )
        {
        addrDataLength = static_cast< TUint8> ( aSource[ index ] );
        }
    else    //04.11 address
        {
        addrDataLength = static_cast< TUint8 >( KDataByteInUnicode 
            * ( aSource[ index ] - 1 ) );
        }

    // Check if there are too many digits
    // Maximum amount of digits is 20 (3GPP TS 23.040)
    if ( KMaxAmountOfDigits < addrDataLength )
        {
        addrDataLength = KMaxAmountOfDigits;
        }

    index++;

    // Lets take type of number field
    TUint8 typeOfNum( aSource[ index ] );
    GsmFindOutTonAndNpi( typeOfNum, &aTon, &aNpi );

    if ( ( RMobilePhone::EAlphanumericNumber == aTon )
        && ( 0x00 != addrDataLength ) )
        {
        //Calculate address length in bytes. If statement above check's that
        //addrDataLength is not zero.
        TInt addrDataLengthInBytes( ( addrDataLength + 1 ) 
            / KDataByteInUnicode );

        //Get address data
        TPtrC8 addressData( aSource.Mid( 
            KAddressLengthAndTon, 
            addrDataLengthInBytes ) ); 

        //amountOfCharsInAddrData = amount of used bits / length of char
        TUint amountOfCharsInAddrData( ( addrDataLength * KHalfByte ) 
            / KLengthOfPackedByte );
           
        GsmAlphanumericAddrToUnicodeAddr( 
            addressData, 
            amountOfCharsInAddrData,
            aDest );
        }
    else
        {
        // Check if number is international number and addrDataLength is 
        // bigger than zero
        if ( ( RMobilePhone::EInternationalNumber == aTon ) 
            && ( 0x00 != addrDataLength ) )
            {
            aDest.Append( '+' );
            }

        index++;

        while ( addrDataLength-- )
            {
            if ( aSource.Length() > index )
                {
                if ( halfs )
                    {               
                    // Lets take only four bits from right
                    num2 = GSMSMSSEMIOCTETTOCHAR( aSource[ index ] & 0x0F );
                    aDest.Append( num2 );
                    halfs = EFalse;
                    }
                else
                    {
                    if ( ( 0x0F == ( aSource[ index ] >> KHalfByte ) ) && !a0340Address )
                        {
                        //If 04.11 address contains odd number of digits, 
                        //bits 5 to 8 of the last octet shall be filled with an end
                        //mark coded as "1111".
                        break;
                        }

                    halfs = ETrue;  // Lets take only four bits from left
                    num2 = GSMSMSSEMIOCTETTOCHAR( aSource[ index ] >> KHalfByte );
                    aDest.Append( num2 );
                    index++;
                    }
                } // if ( aSource.Length() > index )
            } // while
        }
TFLOGSTRING4("TSY: CMmSmsGsmAddress::GsmConvAddrToUnicode - result: number type: %d, numbering plan: %d, number: %S", aTon, aNpi, &aDest);
OstTraceExt3( TRACE_NORMAL, DUP1_CMMSMSGSMADDRESS_GSMCONVADDRTOUNICODE, "CMmSmsGsmAddress::GsmConvAddrToUnicode;aTon=%d;aNpi=%d;aDest=%S", aTon, aNpi, aDest );
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmConvAddrToUnicode - GSM 03.40 address", a0340Address);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMSMSGSMADDRESS_GSMCONVADDRTOUNICODE, "CMmSmsGsmAddress::GsmConvAddrToUnicode;a0340Address=%hhu", a0340Address );
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmFindOutTonAndNpi
// Converts type of number and numbering plan identification
// information from the type of address parameter to the 
// RMobilePhone::TMobileTON and RMobilePhone::TMobileNPI format
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmFindOutTonAndNpi
        ( 
        TUint8 typeOfAddress,               //Type of address
        RMobilePhone::TMobileTON* ton,      //Type of number
        RMobilePhone::TMobileNPI* npi       //Numbering plan identification
        )     
    {
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmFindOutTonAndNpi - address type: %d", typeOfAddress);
OstTraceExt1( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMFINDOUTTONANDNPI, "CMmSmsGsmAddress::GsmFindOutTonAndNpi;typeOfAddress=%hhu", typeOfAddress );
    
    TUint8 numberingPlanIdentification( static_cast< TUint8 >( 
        typeOfAddress & 0x0F ) );
    TUint8 temp( static_cast< TUint8 >( typeOfAddress >> KHalfByte ) );
    TUint8 typeOfNumber( static_cast< TUint8 >( temp & 0x07 ) );

    switch ( typeOfNumber )
        {
        case KInternationalNumber:
            {
            *ton = RMobilePhone::EInternationalNumber;
            break;
            }
        case KNationalNumber:
            {
            *ton = RMobilePhone::ENationalNumber;
            break;
            }
        case KNetworkSpecificNumber:
            {
            *ton = RMobilePhone::ENetworkSpecificNumber;
            break;
            }
        case KSubscriberNumber:
            {
            *ton = RMobilePhone::ESubscriberNumber;
            break;
            }
        case KAlphanumeric:
            {
            *ton = RMobilePhone::EAlphanumericNumber;
            break;
            }
        case KAbbreviatedNumber:
            {
            *ton = RMobilePhone::EAbbreviatedNumber;
            break;
            }

        default:
            {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmFindOutTonAndNpi - unknown type of number");
OstTrace0( TRACE_NORMAL, DUP1_CMMSMSGSMADDRESS_GSMFINDOUTTONANDNPI, "CMmSmsGsmAddress::GsmFindOutTonAndNpi, unknown type of number" );
            *ton = RMobilePhone::EUnknownNumber;
            break;
            }
        }

    switch ( numberingPlanIdentification )
        {
        case KIsdnTelephoneNumPlan:
            {
            *npi = RMobilePhone::EIsdnNumberPlan;
            break;
            }
        case KDataNumPlan:
            {
            *npi = RMobilePhone::EDataNumberPlan;
            break;
            }
        case KTelexNumPlan:
            {
            *npi = RMobilePhone::ETelexNumberPlan;
            break;
            }
        case KServiceCentreSpecificPlan1:
            {
            *npi = RMobilePhone::EServiceCentreSpecificPlan1;
            break;
            }
        case KServiceCentreSpecificPlan2:
            {
            *npi = RMobilePhone::EServiceCentreSpecificPlan2;
            break;
            }
        case KNationalNumPlan:
            {
            *npi = RMobilePhone::ENationalNumberPlan;
            break;
            }
        case KPrivateNumPlan:
            {
            *npi = RMobilePhone::EPrivateNumberPlan;
            break;
            }
        case KErmesNumPlan:
            {
            *npi = RMobilePhone::EERMESNumberPlan;
            break;
            }

        default:
            {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmFindOutTonAndNpi - unknown numbering plan");
OstTrace0( TRACE_NORMAL, DUP2_CMMSMSGSMADDRESS_GSMFINDOUTTONANDNPI, "CMmSmsGsmAddress::GsmFindOutTonAndNpi, unknown numbering plan" );
            *npi = RMobilePhone::EUnknownNumberingPlan;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmMapTonToTUint8
// Converts type of number from the RMobilePhone::TMobileTON
// parameter to the TUint8 format
// ----------------------------------------------------------------------------- 
//
TUint8 CMmSmsGsmAddress::GsmMapTonToTUint8
        (
        RMobilePhone::TMobileTON aTon       //Type of number
        )
    {
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmMapTonToTUint8 - type of number: %d", aTon);
OstTrace1( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMMAPTONTOTUINT8, "CMmSmsGsmAddress::GsmMapTonToTUint8;aTon=%d", aTon );
    TUint8 typeOfNumber( 0 );

    switch ( aTon )
        {
        case RMobilePhone::EInternationalNumber:
            {
            typeOfNumber = KInternationalNumber;
            break;
            }
        case RMobilePhone::ENationalNumber:
            {
            typeOfNumber = KNationalNumber;
            break;
            }
        case RMobilePhone::ENetworkSpecificNumber:
            {
            typeOfNumber = KNetworkSpecificNumber;
            break;
            }
        case RMobilePhone::ESubscriberNumber:
            {
            typeOfNumber = KSubscriberNumber;
            break;
            } 
        case RMobilePhone::EAlphanumericNumber:
            {
            typeOfNumber = KAlphanumeric;
            break;
            } 
        case RMobilePhone::EAbbreviatedNumber:
            {
            typeOfNumber = KAbbreviatedNumber;
            break;
            } 
        case RMobilePhone::EUnknownNumber:
        default:
            {
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmMapTonToTUint8 - unknown type of number: %d", aTon);
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSGSMADDRESS_GSMMAPTONTOTUINT8, "CMmSmsGsmAddress::GsmMapTonToTUint8;unknown type of number aTon=%d", aTon );
            typeOfNumber = KTonUnknown;
            } 
        }

    return typeOfNumber;
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmMapNpiToTUint8
// Converts type of number from the RMobilePhone::TMobileNPI
// parameter to the TUint8 format
// ----------------------------------------------------------------------------- 
//
TUint8 CMmSmsGsmAddress::GsmMapNpiToTUint8
        ( 
        RMobilePhone::TMobileNPI aNpi   //Numbering plan identification
        )
    {
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmMapNpiToTUint8 - numbering plan identification: %d", aNpi);
OstTrace1( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMMAPNPITOTUINT8, "CMmSmsGsmAddress::GsmMapNpiToTUint8;aNpi=%d", aNpi );
    
    TUint8 numPlanIdentification( 0 );

    switch ( aNpi )
        {
        case RMobilePhone::EIsdnNumberPlan:
            {
            numPlanIdentification = KIsdnTelephoneNumPlan;
            break;
            } 
        case RMobilePhone::EDataNumberPlan:
            {
            numPlanIdentification = KDataNumPlan;
            break;
            } 
        case RMobilePhone::ETelexNumberPlan:
            {
            numPlanIdentification = KTelexNumPlan;
            break;
            } 
        case RMobilePhone::EServiceCentreSpecificPlan1:
            {
            numPlanIdentification = KServiceCentreSpecificPlan1;
            break;
            } 
        case RMobilePhone::EServiceCentreSpecificPlan2:
            {
            numPlanIdentification = KServiceCentreSpecificPlan2;
            break;
            } 
        case RMobilePhone::ENationalNumberPlan:
            {
            numPlanIdentification = KNationalNumPlan;
            break;
            } 
        case RMobilePhone::EPrivateNumberPlan:
            {
            numPlanIdentification = KPrivateNumPlan;
            break;
            } 
        case RMobilePhone::EERMESNumberPlan:
            {
            numPlanIdentification = KErmesNumPlan;
            break;
            } 
        case RMobilePhone::EUnknownNumberingPlan:
        default:
            {
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmMapNpiToTUint8 - unknown numbering plan identification: %d", aNpi);
OstTrace1( TRACE_NORMAL, DUP1_CMMSMSGSMADDRESS_GSMMAPNPITOTUINT8, "CMmSmsGsmAddress::GsmMapNpiToTUint8;unknown numbering plan identification aNpi=%d", aNpi );
            numPlanIdentification = KNpiUnknown;
            } 
        }
        
    return numPlanIdentification;
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmUnicodeAddrToAlphanumericAddr
// Converts Unicode characters to GSM 7-bit bit Default Alphabet
// characters (Defined in GSM 23.038). Note that all Unicode characters 
// are not supported by the GSM 7-bit bit Default Alphabet set. Such 
// characters are converted to spaces or to the nearest substitute
// (generally the acute/grave marks are removed). Control characters and 
// special characters are converted to spaces. This method supports GSM 
// 7-bit bit Default Alphabet extension table
// mechanism.
//
// After character conversion this method packs address data by calling
// GsmLibSmsPackMessage method*/
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmUnicodeAddrToAlphanumericAddr
        ( 
        TDesC16 const& aSource,     // Source address data
        TDes8& aDestination,        // Destination data
        TUint8& aAmountOfSemiOctets // Amount of semi-octets
        )
    {
    
TFLOGSTRING2("TSY: CMmSmsGsmAddress::GsmUnicodeAddrToAlphanumericAddr - source address: %S", &aSource);
OstTraceExt1( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMUNICODEADDRTOALPHANUMERICADDR, "CMmSmsGsmAddress::GsmUnicodeAddrToAlphanumericAddr;aSource=%S", aSource );
    
    TUint16 j( 0 );
    TUint16 i( 0 );

    TBuf8< RMobilePhone::KMaxMobileTelNumberSize > alphaChars;
    for ( i = 0; i < aSource.Length(); i++ )
        {
        if ( ( 0x24 < aSource[ i ] ) && ( 0x5B > aSource[ i ] ) && 
             ( 0x40 != aSource[ i ] ) || 
             ( 0x60 < aSource[ i ] ) && ( 0x7B > aSource[ i ] ) )
            {
            // Conversion could be done only taking out the most significant 
            // bit from Unicode characters between 0x25 - 0x5A or 0x61 - 0x7A
            // excluding '@' (0x40).
            alphaChars.Append(aSource[ i ]);
            }
        else if ( ( 0x5A < aSource[ i ] ) && ( 0x5F > aSource[ i ] ) ||
             ( 0x7A < aSource[ i ] ) && ( 0x7F > aSource[ i ] ) ||
             ( 0x20AC == aSource[ i ] ) )
            {
            //Conversion made using GSM 7 bit default alphabet extension 
            //table. Characters: [,\,],^,{,|,},~,&euro
            for ( j = 0 ; j < KGsmExtensionConvTableSize; j++ )
                {
                if ( aSource[ i ] == KGsmExtensionConvTable[ j ].iUnicodeChar )
                    {
                    alphaChars.Append( 0x1B );// extension table flag
                    alphaChars.Append( 
                        KGsmExtensionConvTable[ j ].iGsmChar );
                    break;
                    }
                }   // end of for loop
            }   
        else
            {
            //Try to find character from conversion table.
            TBool notFound( ETrue );
            for ( j = 0; j < KGsmLibConvTableSize; j++ )
                {
                if ( aSource[ i ] == KGsmLibConvTable[ j ].iUnicodeChar )
                    {
                    alphaChars.Append( 
                        KGsmLibConvTable[ j ].iGsmChar );
                    notFound = EFalse;
                    break;
                    }
                }   // end of for loop 
            
            if( notFound )
                {
                //Couldn't found similar character. Convert to SPACE.
                alphaChars.Append( 0x20 ); 
                }
            }
        }

    TUint8 packedChars( GsmLibPackAlphanumericAddress( aDestination, 
                                                        alphaChars ) );

    //How many unused bits packed address contains
    TUint8 unusedBits( static_cast< TUint8 >( ( aDestination.Length() 
        * KLengthOfByte ) - ( packedChars * KLengthOfPackedByte ) ) );

    //How many semi-octets is used in packed address.
    if ( KHalfByte <= unusedBits )
        {
        //Last semi-octet doesn't contain address data
        aAmountOfSemiOctets = static_cast< TUint8 >( 
            ( aDestination.Length() * KTwoCharInByte ) - 1 );
        }
    else
        {
        //All semi-octets are used
        aAmountOfSemiOctets = static_cast< TUint8 >( 
            aDestination.Length() * KTwoCharInByte );
        }
    }  

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmAlphanumericAddrToUnicodeAddr
// This method first decodes a address data that has been 
// coded using the algorithm described in GSM 03.40 annex 2 by calling
// GsmLibUnpackAlphanumericAddress method. After that this method 
// converts GSM 7-bit default alphabet characters to Unicode format.
// This method supports GSM 7-bit bit Default Alphabet extension table
// mechanism
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmAlphanumericAddrToUnicodeAddr
        ( 
        TDesC8 const& aSource,  // Source address data
        TUint& aAmountOfChars,  // Amount of characters in address data
        TDes16& aDestination    // Destination data
        )
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmAlphanumericAddrToUnicodeAddr");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMALPHANUMERICADDRTOUNICODEADDR, "CMmSmsGsmAddress::GsmAlphanumericAddrToUnicodeAddr" );
    
    //Maximum length of address data is 10 bytes. These bytes can contain
    //11 packed 7-bit characters.
    TBuf8< KMaxLengthOfAddressData + 1 > unpackedSrc;
    GsmLibUnpackAlphanumericAddress( aSource, aAmountOfChars, unpackedSrc );

    TUint8 unpackedLength( static_cast< TUint8 >( unpackedSrc.Length() ) );
    TUint8 j( 0 );
    TUint8 i( 0 );

    for ( i = 0; i < unpackedLength; i++ )
        {
        unpackedSrc[ i ] &= 0x7F;     // only 7 bits used in GSM character set

        if ( ( 0x24 < unpackedSrc[ i ] ) && ( 0x5B > unpackedSrc[ i ] ) && 
             ( 0x40 != unpackedSrc[ i ] ) || 
             ( 0x60 < unpackedSrc[ i ] ) && ( 0x7B > unpackedSrc[ i ] ) )
            {
            // Character codes in Unicode and GSM 7-bit default alphabet 
            // are same between 0x25 - 0x5A and 0x61 - 0x7A
            // excluding 0x40.
            aDestination.Append( unpackedSrc[ i ] );
            }
        else if ( 0x1B == unpackedSrc[ i ] )
            { 
            //0x1B is an escape code to the extension table. Code after escape
            //code should been found from extension table.

            i++;

            //It is possible that extension table contains escape code to 
            //another extension table. Increase index i until we find something 
            //else than another escape code.
            for ( ; i < unpackedLength; i++ )
                {
                if ( 0x1B != unpackedSrc[ i ] )
                    {
                    //Found something else than escape code. Let's convert 
                    //character from extension table to Unicode character.
                    TBool notFound( ETrue );

                    for ( j = 0 ; j < KGsmExtensionConvTableSize; j++ )
                        {
                        if ( unpackedSrc[ i ] == 
                            KGsmExtensionConvTable[ j ].iGsmChar )
                            {
                            aDestination.Append( 
                                KGsmExtensionConvTable[ j ].iUnicodeChar );
                            notFound = EFalse;
                            break;
                            }
                        }  

                    if ( notFound )
                        {
                        //Couldn't find character from extension table.
                        //Convert to SPACE.
                        aDestination.Append( 0x20 );
                        }

                    break;
                    }
                else
                    {
                    //Found escape code to another extension table. 
                    //On receipt of this code, a receiving entity shall display 
                    //a space until another extension table is defined. 
                    aDestination.Append( 0x20 );
                    }
                }   // end of for loop
            }   // end of else if
        else
            {
            //Find character from conversion table.
            for ( j = 0; j < KGsmLibConvTableSize; j++ )
                {
                if ( unpackedSrc[ i ] == KGsmLibConvTable[ j ].iGsmChar )
                    {
                    aDestination.Append( 
                        KGsmLibConvTable[ j ].iUnicodeChar );
                    break;
                    }
                }   // end of for loop 
            }   // end of else
        }   // end of for loop
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmLibPackAlphanumericAddress
// This function packs a given text string consisting of 7-bit
// characters into 8 bit bytes in such a way that a 11-character
// address takes only 10 bytes of memory after the compression
// ----------------------------------------------------------------------------- 
//
TUint8 CMmSmsGsmAddress::GsmLibPackAlphanumericAddress
        ( 
        TDes8& aDest,            // Packed address
        TDes8& aSource           // Unpacked address
        )
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmLibPackAlphanumericAddress");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMLIBPACKALPHANUMERICADDRESS, "CMmSmsGsmAddress::GsmLibPackAlphanumericAddress" );
    TUint8 si( 0 );
    TUint8 di( 0 );
    TUint tempModulo( 0 );
    
    // Set tempModulo and byteCount
    tempModulo = di % KLengthOfPackedByte;
    TUint8 byteCount( static_cast< TUint8 >( aSource.Length() ) );

    // Maximum length of address data field is 10 bytes. 10 bytes can contain
    // 11 compressed 7-bit characters. Because of this maximum
    // amount of characters in alphanumeric address is 11.
    if ( ( KMaxLengthOfAddressData + 1 ) < byteCount )
        {
        byteCount = KMaxLengthOfAddressData + 1;
        }   

    while ( si < byteCount )
        {
        // Check if this is the last character of the message.
        // If it is, just shift it to the right. Otherwise,
        // fill the rest of the byte with the bits of the next 
        // source character.
        
        if ( si == ( byteCount - 1 ) )
            {
            TUint num2( ( aSource[ si ] >> tempModulo ) );
            aDest.Append( num2 );
            }
        else
            {
            TUint num2( ( aSource[ si ] >> tempModulo ) | 
                ( aSource[ si + 1 ] << ( KLengthOfPackedByte - tempModulo ) ) );
            aDest.Append( num2 );
            }
   
        di++;
        tempModulo = di % KLengthOfPackedByte;

        // Check if the destination byte could take the entire source character.
        // In that case, the source character does not have to be divided and
        // the next source character can be taken.
        
        if ( tempModulo == 0)
            {
            si += KTwoCharInByte;
            }
        else
            {
            si++;
            }
        }
    
    // Return the length of the coded message.
    return byteCount;
    }

// -----------------------------------------------------------------------------
// CMmSmsGsmAddress::GsmLibUnpackAlphanumericAddress
//  This function decodes a address that has been coded using 
// the algorithm described in GSM 03.40 annex 2. The result string will 
// consist of bytes that contain a 7-bit character each
// ----------------------------------------------------------------------------- 
//
void CMmSmsGsmAddress::GsmLibUnpackAlphanumericAddress
        ( 
        TDesC8 const& aSource,  // Packed address data
        TUint aAmountOfChars,   // Amount of characters in address data
        TDes8& aDest            // Unpacked address data 
        )
    {
TFLOGSTRING("TSY: CMmSmsGsmAddress::GsmLibUnpackAlphanumericAddress");
OstTrace0( TRACE_NORMAL, CMMSMSGSMADDRESS_GSMLIBUNPACKALPHANUMERICADDRESS, "CMmSmsGsmAddress::GsmLibUnpackAlphanumericAddress" );
    TUint8 di( 0 );
    TUint8 si( 0 );  
    TUint tempModulo( 0 );

    // Maximum length of address data field is 10 bytes. 10 bytes can contain
    // 11 compressed 7-bit characters. Because of this maximum
    // amount of characters in alphanumeric address is 11.
    if ( ( KMaxLengthOfAddressData + 1 ) < aAmountOfChars )
        {
        aAmountOfChars = KMaxLengthOfAddressData + 1;
        }

    // The location of the current 7-bit character determines the
    // action to be taken. Only every 7th character is not divided into
    // two bytes. All other characters will have to be contructed by
    // combining bits of two consequent bytes.

    for ( di = 0; di < aAmountOfChars; di++ )
        {
        TUint num2( 0 );
        tempModulo = di % KLengthOfByte;
        switch ( tempModulo )
            {
            case 0:
                {
                num2 = aSource[ si ] & 0x7F;
                break;
                }
            case KLengthOfPackedByte:
                {
                num2 = ( aSource[ si ] >> 1 ) & 0x7F;
                si++;
                break;
                }
            default:
                {
                num2  = aSource[ si ] >> ( KLengthOfByte - tempModulo );
                num2 &= KGsmLibSmsUnpackMaskTable[ tempModulo ];
                num2 |= aSource[ si+1 ] << ( tempModulo );
                num2 &= 0x7F;
                si++;
                break;
                }
            }
            
        aDest.Append( num2 );
        }
    }

//  End of File  
