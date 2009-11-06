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



#ifndef SATUTIL_H
#define SATUTIL_H


//  INCLUDES
#include <etelsat.h>    // etel sat api
#include <etelmm.h>     // etel multimode api


// CONSTANTS

//UCS2 coding
const TUint8 KUCS2ArabicCoding                  = 0x80;
const TUint8 KUCS2GreekCoding                   = 0x81;
const TUint8 KUCS2TurkishCoding                 = 0x82;

// USIM Status words
const TUint8 KAtkSwDataNtfSw1NormalEnding       = 0x90;
const TUint8 KAtkSwDataNtfSw2NormalEnding       = 0x00;
const TUint8 KAtkSwDataNtfSw1busy               = 0x93;

const TUint8 KAllowed                           = 0x01;
const TUint8 KModified                          = 0x02;
const TUint8 KChanged                           = 0x03;
const TUint8 KRejected                          = 0x04;
const TUint8 KError                             = 0x08;
const TUint8 KNoTonNpi                          = 0xFF;
const TUint8 KTonNpiInternational               = 0x91;
const TUint8 KTonNpiUnknown                     = 0x81;


// DATA TYPES

// Sms data coding schemes
enum TSmsDcs
    {
    ESmsUnknownOrReservedDcs    = 0xFF,
    ESms7BitDcs                 = 0x00,
    ESms8BitDcs                 = 0x04,
    ESms16BitDcs                = 0x08
    };


// FORWARD DECLARATIONS
class CBerTlv;
class CTlv;


// CLASS DECLARATION
/**
*  SAT Transaction Id Message class.
*  Handles and stores the current ID of SAT messages, and
*  transaction ID setting and verifying.
*
*  @lib simatktsy.lib
*  @since Series60_ver 2.6
*/
class TTransIdMessage
    {
    public:

        /**
        * C++ default constructor.
        */
        TTransIdMessage();

    public: // New functions

        /**
        * Gets a new transaction id and saves it to transaction id
        * table.
        * @since Series60_ver 2.6
        * @return none
        */
        TUint8 GetTransactionId();

    protected: // Data

        TUint8              iTransNum;
    };


/**
*  SAT Utility class.
*  This class includes small aid fuctions
*  for MMSAT specific use.
*
*  @lib ?library
*  @since Series60_ver 2.6
*/
class TSatUtility
    {
    private:    // Constructors

        /**
        * C++ default constructor. Static utility library isn't meant to be
        * instanced
        * @since NCP 5.0
        */
        TSatUtility();


    public: // New functions

        /**
        * Map TON and NPI to RSat TON and NPI values
        * @since Series60_ver 2.6
        * @param aTonAndNpi: Source TON and NPI
        * @param aTon: Mapped Type of number
        * @param aNpi: Mapped Numbering plan
        * @return void: none
        */
        static void GetTonAndNpi( TInt aTonAndNpi, RSat::TTypeOfNumber* aTon,
            RSat::TNumberingPlan* aNpi );

        /**
        * Map TON and NPI to Etel MM TON and NPI values
        * @since NCP 5.0
        * @param aTonAndNpi: Source TON and NPI
        * @param aTon: Mapped Type of number
        * @param aNpi: Mapped Numbering plan
        * @return void: none
        */
        static void GetTonAndNpi( TInt aTonAndNpi,
            RMobilePhone::TMobileTON& aTon, RMobilePhone::TMobileNPI& aNpi );

        /**
        * Converts 7-bit packed string to 8-bit unpacked format
        * @since Series60_ver 2.6
        * @param aString is source
        * @param aOutput converted 8-bit unpacked string
        * @return void: none
        */
        static void Packed7to8Unpacked( TPtrC8 aString, TDes8& aOutput );

        /**
        * Converts UCS2 string to 7-bit packed format
        * @since Series60_ver 2.6
        * @param aString: is source
        * @param aOutput: converted 7-bit packed string
        * @return void: none
        */
        static void UCSToPacket7( TPtrC aString, TDes8& aOutput );

        /**
        * Converts Binary Coded Decimal to ASCII
        * @since Series60_ver 2.6
        * @param aTring is source
        * @param aOutput converted 8-bit string
        * @return void: none
        */
        static void BCDToAscii( TPtrC8 aString, TDes8& aOutput );

        /**
        * Converts Ascii string to Binary Coded Decimal
        * @since Series60_ver 2.6
        * @param aString is source
        * @param aOutput converted 8-bit string
        */
        static void AsciiToBCD(const TDesC8& aString, TDes8& aOutput );

        /**
        * Remove expansion digit '.' from phone number string if present.
        * @since NCP 3.1
        * @param aPhoneNumber Number to be converted
        * @return void: none
        */
        static void RemoveExpansionDigit( TDes8& aPhoneNumber );

        /**
        * Set Alpha identifier as a Unicode text string
        * and according to the alphabet used
        * @since Series60_ver 2.6
        * @param aRawData input data that was extracted with
        * ETLV_AlphaIdentifier
        * @param aAlphaId output
        * @return void: none
        */
        static void SetAlphaId( TPtrC8 aRawData, TDes& aAlphaId );

        /**
        * Convert integer to BCD format. If number is more than 100 only last
        * two digits is converted.
        * @since Series60_ver 2.6
        * @param aTime time
        * @return TUint8 converted BCD value
        */
        static TUint8 ConvertToSemiOctet( TInt aTime );

        /**
        * Browse a text in Unicode format, and for some specific characters
        * replace with the correct byte value
        * @since Series60_ver 2.6
        * @param aOutput unicode format string
        * @param aInput input
        * @return None
        */
        static void ConvertSms7ToUnicode16( TDes16& aOutput, TDesC8& aInput);

        /**
        * Converts unicode16 string to sms 7
        * @since Series60_ver 2.6
        * @param sms 7 string
        * @param unicode 16
        * @return None
        */
        static void ConvertUnicode16ToSms7( TDes8& aOutput, TDesC16& aInput);

        /**
        * Fill in a TDuration structure
        * @since Series60_ver 2.6
        * @param aBerTlv tlv containing duration data
        * @param aTDuration duration structure
        * @return None
        */
        static void FillDurationStructure( CBerTlv& aBerTlv,
            RSat::TDuration& aTDuration );

        /**
        * Fill in a TIconId structure
        * @since Series60_ver 2.6
        * @param tlv data containing icon address data
        * @param icon id structure
        * @param item number
        * @return None
        */
        static void FillIconStructure( CBerTlv& aBerTlv,
            RSat::TIconId& aTIconId, TInt aItemNmb = 0 );

        /**
        * Checks Sw1 and Sw2 bytes coming from Sim server
        * @since Series60_ver 2.6
        * @param aSw1 status word 1
        * @param aSw2 status word 2
        * @return TInt8 Type of sim atk error
        */
        static TInt8 Sw1Sw2Check( TUint8 aSw1, TUint8 aSw2 );

        /**
        * Set Text string as a Unicode text string
        * @since Series60_ver 2.6
        * @param aTextString text string
        * @param aText unicode output text string
        * @return None
        */
        static void SetText( CTlv& aTextString, TDes& aText );

        /**
        * Convert Alpha field text string to a Unicode text string.
        * @since Series60_ver 2.6
        * @param aSource text string
        * @param aText unicode output text string
        * @return None
        */
        static void ConvertAlphaFieldsToUnicode( TDesC8& aSource,
            TDes& aText );

        /**
        * Finds whether the data coding scheme, coded in CBS format,
        * is 7-bit, 8-bit or 16-bit
        * @since Series60_ver 2.6
        * @param aDcs Input data coding scheme
        * @param aIs7bit Output boolean indicating 7-bit DCS
        * @param aIs8bit Output boolean indicating 8-bit DCS
        * @param aIs16bit Output boolean indicating 16-bit DCS
        * @return None
        */
        static TSmsDcs DecodeCbsDcs( const TUint8 aDcs );

        /**
        * Get 16-bit word from descriptor
        * @since NCP 5.0
        * @param aTarget 16-bit unsigned descriptor
        * @param aSource
        * @param aIndex to point location in descriptor from where to copy word
        * @return None
        */
        static void GetWord( TUint16& aTarget, TDesC8& aSource, TInt aIndex );

        /**
        * Set 16-bit word to descriptor
        * @since NCP 5.0
        * @param aSource 16-bit unsigned descriptor
        * @param aTarget
        * @return None
        */
        static void AppendWord( TUint16 aWord, TDes8& aTarget );

        /**
        * Set Text string User Login and User Password data
        * @since PP 5.2
        * @param aBerTlv tlv containing User Login and User Password data
        * @param aUserLogin User Login data
        * @param aUserPassword User Password data
        * @return None
        */
        static void SetUserLoginAndUserPassword( CBerTlv& aBerTlv,
            RSat::TTextString& aUserLogin,
            RSat::TTextString& aUserPassword );
    };


#endif // SATUTIL_H

// End of File
