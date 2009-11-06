/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Spesific class for SMS tests
*
*/




#ifndef SMS_H
#define SMS_H

//  INCLUDES
#include "core.h"

#include <ctsy/serviceapi/cmmsmsutility.h>
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include "cmmsmstsy.h"
#include <mmmmesshandlerbase.h>
#else
#include "cmmsmstsy_export_to_roots.h"
#endif

// Logging path
_LIT( KSMSLogPath, "\\logs\\testframework\\SMS\\" );
// Log file
_LIT( KSMSLogFile, "SMS.txt" );

_LIT(KIscControlTestCaseFileSMS, "nokiatsy_sms.xml");

// CONSTANTS

const TUint  TKGsmLibConvertToGsmFormat = 1;
const TUint  TKGsmLibConvertToNmpFormat = 2;

//Maximum amount of digits in address data
const TUint8 KMaxLengthOfAddressData    = 10;

// The number of codes that exist in
// both character sets (GSM <--> NMP),
// i.e. bidirectional conversions.
const TUint8 TKGsmLibBidirTableSize     = 34;

// The number of all conversion pairs,
// both GSM <--> NMP and NMP --> substitute.
const TUint8 TKGsmLibConvTableSize      = 86;


const TUint  TKGsmLibAToNTableSize      = 41;

typedef struct
    {
    TUint8 iSmsChar;
    TUint8 iHandsetChar;
    } GsmLibConv;

typedef struct
    {
    TUint8 iGsmChar;
    TUint16 iUnicodeChar;
    } TGsmLibConv;

// This table is used when a sms message is being unpacked.
static const TUint TKGsmLibSmsUnpackMaskTable[ 8 ] =
    { 0x7F, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };

// This table is used when a sms message is being unpacked.
static const TUint KGsmLibSmsUnpackMaskTable[ 8 ] =
    { 0x7F, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };

const TUint8 TKGsmExtensionConvTableSize    = 8;


static const GsmLibConv TKGsmExtensionConvTable[
    TKGsmExtensionConvTableSize ] =
    {
    /*1*/   { 0x3C, 0x5B }, // [ //
    /*2*/   { 0x2F, 0x5C }, // \ //
    /*3*/   { 0x3E, 0x5D }, // ] //
    /*4*/   { 0x14, 0x5E }, // ^ //
    /*5*/   { 0x28, 0x7B }, // { //
    /*6*/   { 0x40, 0x7C }, // | //
    /*7*/   { 0x29, 0x7D }, // } //
    /*8*/   { 0x3D, 0x7E }  // ~ //
    };


const TUint8 KGsmExtensionConvTableSize = 10;
const TUint8 KGsmLibConvTableSize       = 92;

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


    static const TGsmLibConv KGsmLibConvTable[ KGsmLibConvTableSize ] =
    {
        { 0x20, 0x0020 },
        // SPACE, this pair must be here at index #1 to avoid
        // converting NMP(space) to GSM(0x11) or to GSM(0x1B)
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

//The characters in the list below can be found from 8bit Unicode set
//but not from GSM character set. Part of them are changed to spaces,
//part of them to the 'nearest' equivalent (by removing the acute/grave/
//tilde/etc. marks.

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
        { 0x6F, 0x00F3 },   /* NMP: ó, GSM: o */
        { 0x6F, 0x00F4 },   /* NMP: ô, GSM: o */
        { 0x6F, 0x00F5 },   /* NMP: õ, GSM: o */
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


static const GsmLibConv TKGsmLibConvTable[ TKGsmLibConvTableSize ] =
        {
     /*1*/  { 0x20, 0x20 },
     // SPACE, this pair must be here at index #1 to avoid
     // converting NMP(space) to GSM(0x11) or to GSM(0x1B)
     // - see indeces 16 and 17 in the table below.

     /*2*/  { 0x00, 0x40 },   /* @ */
     /*3*/  { 0x01, 0x1C },   /* £ */
     /*4*/  { 0x02, 0x24 },   /* $ */
     /*5*/  { 0x03, 0x5C },   /* ¥ */
     /*6*/  { 0x04, 0xE8 },   /* è */
     /*7*/  { 0x05, 0xE9 },   /* é */
     /*8*/  { 0x06, 0xF9 },   /* ù */
     /*9*/  { 0x07, 0xEC },   /* ì */
     /*10*/ { 0x08, 0xF2 },   /* ò */
     /*11*/ { 0x09, 0xC7 },   /* Ç */
     /*       0x0A:             LF */
     /*12*/ { 0x0B, 0xD8 },   /* Ø */
     /*13*/ { 0x0C, 0xF8 },   /* ø */
     /*       0x0D:             CR */
     /*14*/ { 0x0E, 0xC5 },   /* Å */
     /*15*/ { 0x0F, 0xE5 },   /* å */
     /*       0x10:              NMP and GSM: delta */
     /*16*/ { 0x11, 0x5F },   /* Underscore   */
     /*17*/ { 0x1B, 0x7B },   /*   */
     /*       0x12 - 0x1A:       NMP and GSM: greek letters */
     /*18*/ { 0x1C, 0xC6 },   /* Æ */
     /*19*/ { 0x1D, 0xE6 },   /* æ */
     /*20*/ { 0x1E, 0xDF },   /* beta */
     /*21*/ { 0x1F, 0xC9 },   /* É */
     /*       0x20:              space */
     /*       0x21:              ! */
     /*       0x22:              " */
     /*       0x23:              # */
     /*22*/ { 0x24, 0x1F },   /* ¨ */
     /*       0x25 - 0x3F:       NMP and GSM sets are similar */
     /*23*/ { 0x40, 0x11 },   /* ¡ */
     /*       0x41 - 0x5A:    A..Z */
     /*24*/ { 0x5B, 0xC4 },   /* Ä */
     /*25*/ { 0x5C, 0xD6 },   /* Ö */
     /*26*/ { 0x5D, 0xD1 },   /* Ñ */
     /*27*/ { 0x5E, 0xDC },   /* Ü */
     /*28*/ { 0x5F, 0x1B },   /* § */
     /*29*/ { 0x60, 0x1D },   /* ¿ */
     /*       0x61 - 0x7A:    a..z */
     /*30*/ { 0x7B, 0xE4 },   /* ä */
     /*31*/ { 0x7C, 0xF6 },   /* ö */
     /*32*/ { 0x7D, 0xF1 },   /* ñ */
     /*33*/ { 0x7E, 0xFC },   /* ü */
     /*34*/ { 0x7F, 0xE0 },   /* à */

     //   The characters in the list below exist in NMP character set
     //   but not in GSM character set. Part of them are changed to spaces,
     //   part of them to the 'nearest' equivalent (by removing the acute/grave/
     //   tilde/etc. marks.

     /*35*/ { 0x63, 0x1E },   /* NMP: ¢, GSM: c */
     /*36*/ { 0x20, 0x5B },   /* no equivalent in GSM char set, using SPACE */
     /*37*/ { 0x20, 0x5D },   /* no equivalent in GSM char set, using SPACE */
     /*38*/ { 0x10, 0x5E },
     /* NMP: filled triangle, GSM: delta (unfilled triangle)*/
     /*39*/ { 0x20, 0x5F },   /* NMP: _, GSM: SPACE */
     /*40*/ { 0x20, 0x60 },   /* no equivalent in GSM char set, using SPACE*/
     /*41*/ { 0x20, 0x7B },   /* no equivalent in GSM char set, using SPACE */
     /*42*/ { 0x20, 0x7C },   /* no equivalent in GSM char set, using SPACE */
     /*43*/ { 0x20, 0x7D },   /* no equivalent in GSM char set, using SPACE */
     /*44*/ { 0x20, 0x7E },   /* no equivalent in GSM char set, using SPACE */
     /*45*/ { 0x20, 0x7F },   /* no equivalent in GSM char set, using SPACE */
     /*       0x20, 0x80...0xBF: see line *3000* in the function. */
     /*46*/ { 0x41, 0xC0 },   /* NMP: À, GSM: A */
     /*47*/ { 0x41, 0xC1 },   /* NMP: Á, GSM: A */
     /*48*/ { 0x41, 0xC2 },   /* NMP: Â, GSM: A */
     /*49*/ { 0x41, 0xC3 },   /* NMP: Ã, GSM: A */
     /*       0xC4:        Ä */
     /*       0xC5:        Å */
     /*       0xC6:        Æ */
     /*       0xC7:        Ç */
     /*50*/ { 0x45, 0xC8 },   /* NMP: È, GSM: E */
     /*             0xC9:        É */
     /*51*/ { 0x45, 0xCA },   /* NMP: Ê, GSM: E */
     /*52*/ { 0x45, 0xCB },   /* NMP: Ë, GSM: E */
     /*53*/ { 0x49, 0xCC },   /* NMP: Ì, GSM: I */
     /*54*/ { 0x49, 0xCD },   /* NMP: Í, GSM: I */
     /*55*/ { 0x49, 0xCE },   /* NMP: Î, GSM: I */
     /*56*/ { 0x49, 0xCF },   /* NMP: Ï, GSM: I */
     /*57*/ { 0x45, 0xD0 },   /* NMP: E with tilde, GSM: E */
     /*             0xD1:        Ñ */
     /*58*/ { 0x4F, 0xD2 },   /* NMP: Ò, GSM: O */
     /*59*/ { 0x4F, 0xD3 },   /* NMP: Ó, GSM: O */
     /*60*/ { 0x4F, 0xD4 },   /* NMP: Ô, GSM: O */
     /*61*/ { 0x4F, 0xD5 },   /* NMP: Õ, GSM: O */
     /*             0xD6:        Ö */
     /*62*/ { 0x4F, 0xD7 },   /* NMP: ×, GSM: O */
     /*             0xD8:        Ø */
     /*63*/ { 0x55, 0xD9 },   /* NMP: Ù, GSM: U */
     /*64*/ { 0x55, 0xDA },   /* NMP: Ú, GSM: U */
     /*65*/ { 0x55, 0xDB },   /* NMP: Û, GSM: U */
     /*             0xDC:        Ü */
     /*66*/ { 0x20, 0xDD },
     /* (dotted cursor) no equivalent in GSM, SPACE used */
     /*67*/ { 0x77, 0xDE },   /* NMP: w with caret (^), GSM: w */
     /*             0xDF:        lower case beta */
     /*             0xE0:        à */
     /*68*/ { 0x61, 0xE1 },   /* NMP: á, GSM: a */
     /*69*/ { 0x61, 0xE2 },   /* NMP: â, GSM: a */
     /*70*/ { 0x61, 0xE3 },   /* NMP: ã, GSM: a */
     /*             0xE4:        ä */
     /*             0xE5:        å */
     /*             0xE6:        æ */
     /*71*/ { 0x63, 0xE7 },   /* NMP: ç, GSM: c */
     /*             0xE8:        è */
     /*             0xE9:        é */
     /*72*/ { 0x65, 0xEA },   /* NMP: ê, GSM: e */
     /*73*/ { 0x65, 0xEB },   /* NMP: e with two dots, GSM: e */
     /*             0xEC:        ì */
     /*74*/ { 0x69, 0xED },   /* NMP: í, GSM: i */
     /*75*/ { 0x69, 0xEE },   /* NMP: î, GSM: i */
     /*76*/ { 0x69, 0xEF },   /* NMP: ï, GSM: i */
     /*77*/ { 0x65, 0xF0 },   /* NMP: e with tidle (~), GSM: e */
     /*             0xF1:        ñ */
     /*             0xF2:        ò */
     /*78*/ { 0x6F, 0xF3 },   /* NMP: ó, GSM: o */
     /*79*/ { 0x6F, 0xF4 },   /* NMP: ô, GSM: o */
     /*80*/ { 0x6F, 0xF5 },   /* NMP: õ, GSM: o */
     /*             0xF6:        ö */
     /*81*/ { 0x6F, 0xF7 },   /* NMP: ÷, GSM: o */
     /*             0xF8:        ø */
     /*             0xF9:        ù */
     /*82*/ { 0x75, 0xFA },   /* NMP: ú, GSM: u */
     /*83*/ { 0x75, 0xFB },   /* NMP: û, GSM: u */
     /*             0xFC:        ü */
     /*84*/ { 0x79, 0xFD },   /* NMP: y with ', GSM: y */
     /*85*/ { 0x79, 0xFE },   /* NMP: y with ^, GSM: y */
     /*86*/ { 0x79, 0xFF }    /* NMP: ý, GSM: y */
    };

// ENUMERATIONS
// Test case enumerations can be used for defining parameter checks for
// certain test cases. Can be set by function SetSmsParameterCheck.
enum TTestCase
    {
    ETestCaseNotDefined = 0,
    ETestCaseSms001 = 1,
    ETestCaseSms008 = 8,
    ETestCaseSms019 = 19
    };

// FORWARD DECLARATIONS
class CSMS;
class CCore;
class RMobileSmsMessaging;
class TDataPackage;
class CMmDataPackage;
class RMobilePhone;
class CMessageRouterBase;


// CLASS DECLARATION

class CSMS : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSMS* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSMS();


    protected:  // New functions

        /**
        * Set SMS parameter check ON/OFF and test case number
        * @since Series60_ver 5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetSmsParameterCheck( CStifItemParser& aItem );

        /**
        * Set SMS reference parameters for parameter checking
        * @since Series60_ver 5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetSmsReferenceParameters( CStifItemParser& aItem );

        /**
         * Updates SMS settings
         * @param aItem Script line containing method name and parameters
         * @return Symbian OS error code.
         */
        TInt SmsSettingsUpdate( CStifItemParser& aItem );

        /**
        * Send SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendSMSMessageL(CStifItemParser& aItem);

        /**
        * Complete Send SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteSendSMSMessage( TInt aResult );

        /**
        * Complete Send SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteSendSMSMessage( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Send Two SMS Messages
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendTwoSMSMessagesL(CStifItemParser& aItem);

        /**
        * Read SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadSMSMessageL(CStifItemParser& aItem);

        /**
        * Complete Read SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteReadSMSMessage( TInt aResult,CMmDataPackage* aDataPackage );

        /**
        * Delete SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeleteSMSMessageL( CStifItemParser& aItem );

        /**
        * Complete Delete SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteDeleteSMSMessage( TInt aResult );

        /**
        * Delete All SMS Messages
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeleteAllSMSMessagesL( CStifItemParser& );

        /**
        * Complete Delete All SMS Messages
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteDeleteAllSMSMessages( TInt aResult );

        /**
        * Set MO SMS Bearer
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetMoSmsBearerL( CStifItemParser& aItem );

        /**
        * Complete Set MO SMS Bearer
        * @param TInt aResult Error code
        * @return None
        */
        void CompleteSetMoSmsBearer( TInt aResult );

        /**
        * Write SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt WriteSMSMessageL(CStifItemParser& aItem);

        /**
        * Complete Write SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteWriteSMSMessage( TInt aResult,CMmDataPackage* aDataPackage );

        /**
        * Receive SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReceiveSMSMessageL(CStifItemParser& aItem);

        /**
        * Complete Receive SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteReceiveSMSMessage( TInt aResult, CMmDataPackage* aDataPackage);


        /**
        * Activate SMS Routing
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ActivateSMSRoutingL( CStifItemParser& );

        /**
        * Complete Activate SMS Routing
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteActivateSMSRouting( TInt aResult );

        /**
        * Complete Activate SMS Routing
        * @param aResult Error code to be completed
        * @param aDataPackage Data package containing reception status
        * @return None
        */
        void CompleteActivateSMSRouting(
            TInt aResult,
            CMmDataPackage* aDataPackage );


        /**
        * Acknowledge SMS message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt AckSMSL( CStifItemParser& );

        /**
        * Complete Acknowledge SMS message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteAckSMS( TInt aResult );

        /**
        * Deactivate SMS Routing
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt DeactivateSMSRoutingL( CStifItemParser& );

        /**
        * Complete Deactivate SMS Routing
        * @param aResult Error code to be completed
        * @return None
        */
        void CompleteDeactivateSMSRouting( TInt aResult );

        /**
        * Complete Deactivate SMS Routing
        * @param aResult Error code to be completed
        * @param aDataPackage Data package containing reception status
        * @return None
        */
        void CompleteDeactivateSMSRouting(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Resume SMS
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ResumeSMSL( CStifItemParser& );

        /**
        * Complete Resume SMS
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteResumeSMS( TInt aResult );

        /**
        * Read All SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadAllSMSMessageL( CStifItemParser& aItem );

        /**
        * Complete Read All SMS Message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteReadAllSMSMessage( TInt aResult,CMmDataPackage* aDataPackage );

        /**
        * Non Acknowledge SMS message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt NackSMSL( CStifItemParser& aItem);

        /**
        * Complete Non Acknowledge SMS message
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteNackSMS( TInt aResult );

        /**
        * Get SMS Store Info
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt SIMMsgStorageInfoL(CStifItemParser& aItem);

        /**
        * Complete SIMMsgStorageInfo
        * @param aItem Script line containing method name and parameters
        * @return None
        */
        void CompleteSIMMsgStorageInfo( TInt aResult,CMmDataPackage* aDataPackage );


        /**
        * Read SMS Parameters
        * @return Symbian OS error code.
        */
        TInt ReadSmsParametersL( CStifItemParser& aItem );

        /**
        * Complete Read SMS Parameters
        * @param TInt aResult Error code
        * @param CMmDataPackage* aDataPackage SMS Parameters
        * @return None
        */
        void CompleteReadSmsParameters( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Store SMS Parameters
        * @param aItem SMS parameters from script
        * @return Symbian OS error code.
        */
        TInt StoreSmsParametersL( CStifItemParser& aItem );

        /**
        * Complete Store SMS Parameters
        * @param TInt aResult Error code
        * @return None
        */
        void CompleteStoreSmsParameters( TInt aResult );

        /**
        * SMS cache initialization
        * @param aItem SMS parameters from script
        * @return Symbian OS error code.
        */
        TInt SMSCacheInitializationL( CStifItemParser& aItem );

        /**
        * Complete SMS cache initialization
        * @param TInt aResult Error code
        * @return None
        */
        void CompleteSMSCacheInitialization( TInt aResult );

        /**
        * Complete Mobile Phone Store Get Info
        * @param TInt aResult Error code
        * @return None
        */
        void CompleteMobilePhoneStoreGetInfo( TInt aResult );

     private:

        // These functions are mainly taken from
        // Nokia DOS TSY / mmsmsiface
        // CMmSmsGsmAddress.h

        /**
        * Build gsm 0340 address
        * @since Series60_ver 2.8
        * @param TUint8 aTon: Type of number
        * @param TUint8 aNpi: Numbering plan identification
        * @param TDes8 &aDest: Service center address
        * @param TDesC16 &aSource: Telephone number
        * @return: None
        */
        void GsmConvUnicodeTo0340Addr(
            TUint8 aTon,        //Type of number
            TUint8 aNpi,        //Numbering plan identification
            TDes8 &aDest,       //Service center address
            TDesC16 &aSource    //Telephone number
            );

        /**
        * Character conversion
        * @since Series60_ver 2.8
        * @param &dest: Packed message
        * @param &src: Unpacked message
        * @param byte_count number of bytes
        * @return: unsigned index
        */
        TUint16 GsmLibSmsPackMessage(
            TDes8 &aDest,
            TDes8 &aSrc,
            TUint16 aByte_count
            );


        /**
        * Convert GSM 0340 or 0411 address to Unicode address
        * @since Series60_ver 2.8
        * @param TDes &aDest: Unicode address will be constructed to here
        * @param TDesC8 &aSource: 03.40 or 04.11 address
        * @param TUint8 aTon: Type of number will be set to here
        * @param TUint8 aNpi: Numbering plan identification will be set to here
        * @param TBool a0340Address: Address type is 0340 if TRUE
        * @return: void: None
        */
        void GsmConvAddrToUnicode(
            TDes &aDest,                            //Service center address
            TDesC8 &aSource,                        //Telephone number
            RMobilePhone::TMobileTON& aTon,         //Type of number
            RMobilePhone::TMobileNPI& aNpi,         //Telephone number
            TBool a0340Address                      //GSM 03.40 address
            );

        /**
        * Convert Unicode address to GSM 0340 or 0411 address
        * @since Series60_ver 2.8
        * @param TUint8 aTon: Type of number
        * @param TUint8 aNpi: Numbering plan identification
        * @param TDes8 &aDest: 03.40 or 04.11 will be constructed to here
        * @param TDesC16 &aSource: Telephone number
        * @param TBool a0340Address: Address type is 0340 if TRUE
        * @return:None
        */
        void GsmConvUnicodeToAddr(
            TUint8 aTon,        //Type of number
            TUint8 aNpi,        //Numbering plan identification
            TDes8 &aDest,       //Service center address
            TDesC16 &aSource,   //Telephone number
            TBool a0340Address  //GSM 03.40 address
            );

        /**
        * Construct alphanumeric address data from Unicode address
        * @since Series60_ver 2.8
        * @param TDesC16& aSource: Source data
        * @param TDes8& aDestination: Destination data
        * @param TUint8& aAmountOfSemiOctets: Amount of used semi-octets in
        * address data
        * @return None
        */
       void GsmUnicodeAddrToAlphanumericAddr(
            TDesC16& aSource,           // Source address data
            TDes8& aDestination,        // Destination data
            TUint8& aAmountOfSemiOctets // Amount of semi-octets
            );

        /**
        * Fint out type of number and numbering plan identification
        * @param TUint8 typeOfAddress: Type of address
        * @param RMobilePhone::TMobileTON* ton: Type of number
        * @param RMobilePhone::TMobileNPI* npi: Numbering plan identification
        * @return None
        */
        void GsmFindOutTonAndNpi(
            TUint8 typeOfAddress,               //Type of address
            RMobilePhone::TMobileTON* ton,      //Type of number
            RMobilePhone::TMobileNPI* npi       //Numbering plan identification
            );

       /**
        * Construct Unicode address data from alphanumeric address
        * @since Series60_ver 2.8
        * @param TDesC8& aSource: Source data
        * @param TUint& aAmountOfChars: Amount of characters in address data
        * @param TDes16& aDestination: Destination data
        * @return None
        */
        void GsmAlphanumericAddrToUnicodeAddr(
            TDesC8& aSource,        // Source address data
            TUint& aAmountOfChars,  // Amount of characters in address data
            TDes16& aDestination    // Destination data
            );

        /**
        * Packs a given text string consisting of 7-bit characters
        * into 8 bit bytes.
        * @since Series60_ver 2.8
        * @param TDes8 &dest: Packed message
        * @param TDes8 &src: Unpacked message
        * @return TUint8: Amount of packed characters
        */
        TUint8 GsmLibPackAlphanumericAddress(
            TDes8 &aDest,            // Packed address
            TDes8 &aSource           // Unpacked address
            );

        /**
        * Decodes a address that has been coded using the algorithm described
        * in GSM 03.40 annex 2.
        * @since Series60_ver 2.8
        * @param TDes8 &aSource: Unpacked message
        * @param TUint aAmountOfChars: Amount of characters in address data
        * @param TDes8 &dest: Packed message
        * @return None
        */
        void GsmLibUnpackAlphanumericAddress(
            TDesC8 &aSource,        // Packed address data
            TUint aAmountOfChars,   // Amount of characters in address data
            TDes8 &aDest            // Unpacked address data
            );


    public:

        /**
        * From CScriptBase Runs a script line.
        * @since Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );


        /**
        * Completes a request or notification via the specific Tsy object,
        * based on IPC number and data package
        * @since Series60_ver 2.8
        * @param aIpc: IPC number
        *        aDataPackage: Data package
        *        aResult: result
        * @return None
        */
        IMPORT_C virtual void RouteCompletion(
            TInt aIpc,
            CMmDataPackage* aDataPackage,
            TInt aResult );


       /**
        * Character conversion
        * @since Series60_ver 2.8
        * @param aDirection
        * @param aString
        * @param aLength Packed message
        * @return None
        */
        void GsmLibCharConversion(
            TUint aDirection,
            TDes8& aString,
            TUint16 aLength
            );


    protected:

        /**
        * C++ default constructor.
        */
        CSMS( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_version
        */
        void Delete();

    private:  // Data
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        CStifLogger* iSMSLog;

       /**
        * By default Symbian 2nd phase constructor is private.
        */
        TRequestStatus iStatus;

       /**
        * RMobileSmsMessaging object
        */
        RMobileSmsMessaging iSmsMessaging;

       /**
        *H olds Sms attributes variable
        */
        RMobileSmsMessaging::TMobileSmsSendAttributesV1 iMsgAttrib;

       /**
        * Holds Sms Gsm Tdpu variable
        */
        RMobileSmsMessaging::TMobileSmsGsmTpdu iGsmMes;

        /**
        * Holds Sms attributes package holding iMsgAttrib
        */
        RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg iMsgAttribPckg;

        /**
        * Holds Sms caps package holding iSmsCaps
        */
        RMobileSmsMessaging::TMobileSmsCapsV1Pckg iSmsCapsPckg;

       /**
        * Holds Sms caps variable
        */
        RMobileSmsMessaging::TMobileSmsCapsV1 iSmsCaps;

        /*@see RMobilePhone::TMobileAddress */
        RMobilePhone::TMobileAddress iServiceCentre;

        /** The GSM 03.40 TPDU.
        Modes: GSM/WCDMA
        @see RMobileSmsMessaging::TMobileSmsGsmTpdu */
        RMobileSmsMessaging::TMobileSmsGsmTpdu iMsgData;

        TBool iSecondMessage;

        // Write SMS message pointer
        TAny* iWriteSmsEntryPtr;

        // A pointer to send message attributes
        TDes8* iSendMessageMsgAttrPckgPtr;

        // Wait for unstored message pointer
        TDes8* iReceiveMessagePduPtr;

        // Wait for unstored message pointer
        TAny* iReceiveMessageParamsPtr;

        //For answering only to requests from scripts
        TBool iSMSMsgStorageRequestOngoing;

        //For comparison SMS class type (class 1 or 2)
        TBool iReceivedSmsClass2;

        // For parameter checking
        TBool iSmsParameterCheck;

        // Test case number for parameter checking
        TTestCase iTestCaseNumber;

        // Reference parameters for parameter checking
        TUint16 iMsgRef;

        TBuf8<RMobileSmsMessaging::KGsmTpduSize> iPdu;

        CArrayPtrFlat<TSmsParameters>* iSmspListArray;

        TInt iNumOfSmspSets;

        TSmsMsg iSmsMsg;
    };

#endif      // SMS_H

// End of File