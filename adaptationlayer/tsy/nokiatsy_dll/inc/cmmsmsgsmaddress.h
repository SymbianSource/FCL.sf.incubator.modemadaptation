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



#ifndef _CMMSMSGSMADDRESS_H_
#define _CMMSMSGSMADDRESS_H_


//  INCLUDES
#include <etelmm.h>

//  CONSTANTS
//Maximum length of address data in bytes
const TUint8 KMaxLengthOfAddressData = 10;

//Constants for char conversion handling
const TUint8 KGsmLibConvTableSize     = 92;
const TUint8 KGsmExtensionConvTableSize = 10;

// Struct holding a pair of GSM 7-Bit character and Unicode character
typedef struct
    {
    TUint8 iGsmChar;
    TUint16 iUnicodeChar;
    } TGsmLibConv;

// This table is used when a 03.40 or 04.11 address is converted to Unicode.
const TUint gsmSmsSemiOctetToCharTable[16] =
   {
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00
   };

// This table is used when a sms message is being unpacked.
static const TUint KGsmLibSmsUnpackMaskTable[ 8 ] =
    { 0x7F, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };

// Conversion table GSM 7-Bit Extension to Unicode
// See 3GPP TS 23.038
static const TGsmLibConv KGsmExtensionConvTable[
    KGsmExtensionConvTableSize ] =
    {
        { 0x0A, 0x000A }, /* Convert Page Break to Line Feed*/
        { 0x3C, 0x005B }, /* [ */
        { 0x2F, 0x005C }, /* \ */
        { 0x3E, 0x005D }, /* ] */
        { 0x14, 0x005E }, /* ^ */
        { 0x28, 0x007B }, /* { */
        { 0x40, 0x007C }, /* | */
        { 0x29, 0x007D }, /* } */
        { 0x3D, 0x007E }, /* ~ */
        { 0x65, 0x20AC }  /* Euro currency symbol */
    };

// Conversion table GSM 7-Bit to Unicode
// See 3GPP TS 23.038
static const TGsmLibConv KGsmLibConvTable[ KGsmLibConvTableSize ] =
    {
        { 0x20, 0x0020 }, // SPACE, this pair must be here at index #1 to avoid
                          // converting space to GSM(0x11) or to GSM(0x1B)
                          // - see indeces 16 and 17 in the table below.
        { 0x00, 0x0040 }, /* @ */
        { 0x01, 0x00A3 }, /* £ */
        { 0x02, 0x0024 }, /* $ */
        { 0x03, 0x00A5 }, /* ¥ */
        { 0x04, 0x00E8 }, /* è */
        { 0x05, 0x00E9 }, /* é */
        { 0x06, 0x00F9 }, /* ù */
        { 0x07, 0x00EC }, /* ì */
        { 0x08, 0x00F2 }, /* ò */
        { 0x09, 0x00C7 }, /* Ç */
        { 0x0A, 0x000A }, /* LF */
        { 0x0B, 0x00D8 }, /* Ø */
        { 0x0C, 0x00F8 }, /* ø */
        { 0x0D, 0x000D }, /* CR */
        { 0x0E, 0x00C5 }, /* Å */
        { 0x0F, 0x00E5 }, /* å */
        { 0x10, 0x0394 }, /* Greek: DELTA */
        { 0x11, 0x005F }, /* _ */
        { 0x12, 0x03A6 }, /* Greek: PHI */
        { 0x13, 0x0393 }, /* Greek: GAMMA */
        { 0x14, 0x039B }, /* Greek: LAMDA */
        { 0x15, 0x03A9 }, /* Greek: OMEGA */
        { 0x16, 0x03A0 }, /* Greek: PI */
        { 0x17, 0x03A8 }, /* Greek: PSI */
        { 0x18, 0x03A3 }, /* Greek: SIGMA */
        { 0x19, 0x0398 }, /* Greek: THETA */
        { 0x1A, 0x039E }, /* Greek: XI */
        /*0x1B             Extension code */
        { 0x1C, 0x00C6 }, /* Æ */
        { 0x1D, 0x00E6 }, /* æ */
        { 0x1E, 0x00DF }, /* beta */
        { 0x1F, 0x00C9 }, /* É */
        /*0x20             SPACE */
        { 0x21, 0x0021 }, /* ! */
        { 0x22, 0x0022 }, /* " */
        { 0x23, 0x0023 }, /* # */
        { 0x24, 0x00A4 }, /* ¤ */
        /*0x25 - 0x3F:     Unicode and GSM sets are similar */
        { 0x40, 0x00A1 }, /* ¡ */
        /*0x41 - 0x5A:     A..Z */
        { 0x5B, 0x00C4 }, /* Ä */
        { 0x5C, 0x00D6 }, /* Ö */
        { 0x5D, 0x00D1 }, /* Ñ */
        { 0x5E, 0x00DC }, /* Ü */
        { 0x5F, 0x00A7 }, /* § */
        { 0x60, 0x00BF }, /* ¿ */
        /*0x61 - 0x7A:     a..z */
        { 0x7B, 0x00E4 }, /* ä */
        { 0x7C, 0x00F6 }, /* ö */
        { 0x7D, 0x00F1 }, /* ñ */
        { 0x7E, 0x00FC }, /* ü */
        { 0x7F, 0x00E0 }, /* à */

/*The characters in the list below can be found from 8bit Unicode set
  but not from GSM character set. Part of them are changed to spaces,
  part of them to the 'nearest' equivalent (by removing the acute/grave/
  tilde/etc. marks.*/

        { 0x63, 0x00A2 },   /* Unicode: ¢, GSM: c */
        /*      0x5B-0x5E These chars can be found GSM extension table: [\]^ */
        { 0x20, 0x0060 },   /* Unicode: ', GSM: using SPACE */
        /*      0x7B-0x7E These chars can be found GSM extension table: {|}~ */
        { 0x20, 0x007F },   /* Unicode: DEL, GSM: using SPACE */
        /*      0x80...0xBF: see conversion methods */
        { 0x41, 0x00C0 },   /* Unicode: À, GSM: A */
        { 0x41, 0x00C1 },   /* Unicode: Á, GSM: A */
        { 0x41, 0x00C2 },   /* Unicode: Â, GSM: A */
        { 0x41, 0x00C3 },   /* Unicode: Ã, GSM: A */
        /*      0xC4:        Ä */
        /*      0xC5:        Å */
        /*      0xC6:        Æ */
        /*      0xC7:        Ç */
        { 0x45, 0x00C8 },   /* Unicode: È, GSM: E */
        /*      0xC9:        É */
        { 0x45, 0x00CA },   /* Unicode: Ê, GSM: E */
        { 0x45, 0x00CB },   /* Unicode: Ë, GSM: E */
        { 0x49, 0x00CC },   /* Unicode: Ì, GSM: I */
        { 0x49, 0x00CD },   /* Unicode: Í, GSM: I */
        { 0x49, 0x00CE },   /* Unicode: Î, GSM: I */
        { 0x49, 0x00CF },   /* Unicode: Ï, GSM: I */
        { 0x44, 0x00D0 },   /* Unicode: D with line, GSM: D */
        /*      0xD1:        Ñ */
        { 0x4F, 0x00D2 },   /* Unicode: Ò, GSM: O */
        { 0x4F, 0x00D3 },   /* Unicode: Ó, GSM: O */
        { 0x4F, 0x00D4 },   /* Unicode: Ô, GSM: O */
        { 0x4F, 0x00D5 },   /* Unicode: Õ, GSM: O */
        /*      0xD6:        Ö */
        { 0x78, 0x00D7 },   /* Unicode: ×, GSM: x */
        /*      0xD8:        Ø */
        { 0x55, 0x00D9 },   /* Unicode: Ù, GSM: U */
        { 0x55, 0x00DA },   /* Unicode: Ú, GSM: U */
        { 0x55, 0x00DB },   /* Unicode: Û, GSM: U */
        /*      0xDC:        Ü */
        { 0x59, 0x00DD },   /* Unicode: Y with ', GSM: Y */
        { 0x20, 0x00DE },   /* Unicode: Latin capital thorn, GSM: using SPACE */
        /*      0xDF:        beta */
        /*      0xE0:        à */
        { 0x61, 0x00E1 },   /* Unicode: á, GSM: a */
        { 0x61, 0x00E2 },   /* Unicode: â, GSM: a */
        { 0x61, 0x00E3 },   /* Unicode: ã, GSM: a */
        /*      0xE4:        ä */
        /*      0xE5:        å */
        /*      0xE6:        æ */
        { 0x63, 0x00E7 },   /* Unicode: ç, GSM: c */
        /*      0xE8:        è */
        /*      0xE9:        é */
        { 0x65, 0x00EA },   /* Unicode: ê, GSM: e */
        { 0x65, 0x00EB },   /* Unicode: e with two dots, GSM: e */
        /*      0xEC:        ì */
        { 0x69, 0x00ED },   /* Unicode: í, GSM: i */
        { 0x69, 0x00EE },   /* Unicode: î, GSM: i */
        { 0x69, 0x00EF },   /* Unicode: ï, GSM: i */
        { 0x20, 0x00F0 },   /* Unicode: Latin small eth, GSM: using SPACE */
        /*      0xF1:        ñ */
        /*      0xF2:        ò */
        { 0x6F, 0x00F3 },   /* ó, GSM: o */
        { 0x6F, 0x00F4 },   /* ô, GSM: o */
        { 0x6F, 0x00F5 },   /* õ, GSM: o */
        /*      0xF6:        ö */
        { 0x20, 0x00F7 },   /* Unicode: ÷, GSM: using SPACE */
        /*      0xF8:        ø */
        /*      0xF9:        ù */
        { 0x75, 0x00FA },   /* Unicode: ú, GSM: u */
        { 0x75, 0x00FB },   /* Unicode: û, GSM: u */
        /*      0xFC:        ü */
        { 0x79, 0x00FD },   /* Unicode: y with ', GSM: y */
        { 0x20, 0x00FE },   /* Unicode: Latin small thorn, GSM: using SPACE */
        { 0x79, 0x00FF }    /* Unicode: y with two dots, GSM: y */
    };

//  MACROS

//  DATA TYPES

//  EXTERNAL DATA STRUCTURES

//  FUNCTION PROTOTYPES

//  FORWARD DECLARATIONS

//  CLASS DEFINITIONS

// DESCRIPTION
//
// CMmSmsGsmAddress class makes GSM 03.40 and 04.11 address conversions
class CMmSmsGsmAddress : CBase
    {
    //METHODS
    public:

        /**
        * Build gsm 0340 address
        * @param TUint8 aTon: Type of number
        * @param TUint8 aNpi: Numbering plan identification
        * @param TDes8& aDest: Service center address
        * @param TDesC16 const & aSource: Telephone number
        * @return: void: None
        */
        static void GsmConvUnicodeTo0340Addr( TUint8 aTon,
            TUint8 aNpi, TDes8& aDest, TDesC16 const& aSource );

        /**
        * Convert 0340 address to unicode
        * @param TDes& aDest: Service center address
        * @param TDesC8 const& aSource: Telephone number
        * @param RMobilePhone::TMobileTON& aTon: Type of number
        * @param RMobilePhone::TMobileNPI& aNpi: Telephone number
        * @return: void: None
        */
        static void GsmConv0340AddrToUnicode( TDes& aDest,
            TDesC8 const& aSource, RMobilePhone::TMobileTON& aTon,
            RMobilePhone::TMobileNPI& aNpi );

        /**
        * Build gsm 0411 address
        * @param TUint8 aTon: Type of number
        * @param TUint8 aNpi: Numbering plan identification
        * @param TDes8& aDest: Service center address
        * @param TDesC16 const& aSource: Telephone number
        * @return: void: None
        */
        static void GsmConvUnicodeTo0411Addr(TUint8 aTon,
            TUint8 aNpi, TDes8& aDest, TDesC16 const& aSource );

        /**
        * Convert 0411 address to unicode
        * @param TDes& aDest: Destination
        * @param TDesC8 const& aSource: Source
        * @param RMobilePhone::TMobileTON& ton: Type of number
        * @param RMobilePhone::TMobileNPI& npi: Numbering plan identification
        * @return: void: None
        */
        static void GsmConv0411AddrToUnicode( TDes& aDest,
            TDesC8 const& aSource, RMobilePhone::TMobileTON& ton,
            RMobilePhone::TMobileNPI& npi );

        /**
        * Find out type of number and numbering plan identification
        * @param TUint8 typeOfAddress: Type of address
        * @param RMobilePhone::TMobileTON* ton: Type of number
        * @param RMobilePhone::TMobileNPI* npi: Numbering plan identification
        * @return: void: None
        */
        static void GsmFindOutTonAndNpi( TUint8 typeOfAddress,
            RMobilePhone::TMobileTON* ton, RMobilePhone::TMobileNPI* npi );

        /**
        * Map TMobileTon to the TUint8
        * @param RMobilePhone::TMobileTON aTon: Type of number
        * @return: TUint8: Type Of Number
        */
        static TUint8 GsmMapTonToTUint8( RMobilePhone::TMobileTON aTon );

        /**
        * Map TMobileNPI to the TUint8
        * @param RMobilePhone::TMobileNPI aNpi: Numbering plan identification
        * @return: TUint8: Numbering Plan Identification
        */
        static TUint8 GsmMapNpiToTUint8( RMobilePhone::TMobileNPI aNpi );

        /**
        * Convert Unicode address to GSM 0340 or 0411 address
        * @param TUint8 aTon: Type of number
        * @param TUint8 aNpi: Numbering plan identification
        * @param TDes8& aDest: 03.40 or 04.11 will be constructed to here
        * @param TDesC16 const& aSource: Telephone number
        * @param TBool a0340Address: Address type is 0340 if TRUE
        * @return: void: None
        */
        static void GsmConvUnicodeToAddr( TUint8 aTon, TUint8 aNpi,
            TDes8& aDest, TDesC16 const& aSource, TBool a0340Address );

        /**
        * Convert GSM 0340 or 0411 address to Unicode address
        * @param TDes& aDest: Unicode address will be constructed to here
        * @param TDesC8& aSource: 03.40 or 04.11 address
        * @param TUint8 aTon: Type of number will be set to here
        * @param TUint8 aNpi: Numbering plan identification will be set to here
        * @param TBool a0340Address: Address type is 0340 if TRUE
        * @return: void: None
        */
        static void GsmConvAddrToUnicode( TDes& aDest, TDesC8 const& aSource,
            RMobilePhone::TMobileTON& aTon, RMobilePhone::TMobileNPI& aNpi,
            TBool a0340Address );

        /**
        * Construct alphanumeric address data from Unicode address
        * @param TDesC16 const& aSource: Source data
        * @param TDes8& aDestination: Destination data
        * @param TUint8& aAmountOfSemiOctets: Amount of used semi-octets in
        * address data
        * @return: void: None
        */
        static void GsmUnicodeAddrToAlphanumericAddr( TDesC16 const& aSource,
            TDes8& aDestination, TUint8& aAmountOfSemiOctets );

        /**
        * Construct Unicode address data from alphanumeric address
        * @param TDesC8 const& aSource: Source data
        * @param TUint& aAmountOfChars: Amount of characters in address data
        * @param TDes16& aDestination: Destination data
        * @return: void: None
        */
        static void GsmAlphanumericAddrToUnicodeAddr( TDesC8 const& aSource,
            TUint& aAmountOfChars, TDes16& aDestination );

        /**
        * Packs a given text string consisting of 7-bit characters
        * into 8 bit bytes.
        * @param TDes8& dest: Packed message
        * @param TDes8& src: Unpacked message
        * @return: TUint8: Amount of packed characters
        */
        static TUint8 GsmLibPackAlphanumericAddress( TDes8& aDest,
            TDes8& aSource );

        /**
        * Decodes a address that has been coded using the algorithm described
        * in GSM 03.40 annex 2.
        * @param TDes8 const& src: Unpacked message
        * @param TUint aAmountOfChars: Amount of characters in address data
        * @param TDes8& dest: Packed message
        * @return: void: None
        */
        static void GsmLibUnpackAlphanumericAddress( TDesC8 const& aSource,
            TUint aAmountOfChars, TDes8& aDest );

    private:
        /**
        * Constructor
        * @return: None
        */
        CMmSmsGsmAddress();

    };

#endif  // _CMMSMSGSMADDRESS_H_

// End of File


