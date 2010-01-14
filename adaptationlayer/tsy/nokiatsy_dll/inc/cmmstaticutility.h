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



#ifndef CMMSTATICUTILITY_H
#define CMMSTATICUTILITY_H

// INCLUDES
#include <e32base.h>
#include <e32math.h>
#include <etelpckt.h>
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
#include "pn_const_for_uicc.h"
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <pn_const.h>
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <ctsy/serviceapi/cmmutility.h>

#include "tsylogger.h"

// CONSTANTS
const TUint8 KTsyNetCauseCommon = 0x00;
const TUint8 KTsyNetCauseGsm = 0x01;
const TUint8 KTsyNetCauseCdma = 0x02;
const TUint8 KTsyNetCauseWcdma = 0x03;

const TUint8 KNoCsCauseType = 0xFF;

// SIM Status tables
const TUint8 KSimStatusTable = 0x01;
const TUint8 KSimPbStatusTable = 0x02;
const TUint8 KSimStaTable = 0x03;
const TUint8 KApduStatus = 0x04;
const TUint8 KSimSbStaTable = 0x05;

// IPV4 address length
const TInt KIpv4AddressLen = 4;
// IPV6 address  length
const TInt KIpv6AddressLen = 16;
// Digits separator for IPv6 address format
static const TUint8 KClientIPv6AddressDigitsSeparator = ':';

// PMM Group ID and indexes for storing NITZ and Call Life Timer.
// Group ID for PMM (PMM_GROUP_NOKIA_TSY = 382).
const TUint16 KPmmGroupNokiaTsy = 0x017E;
// NITZ Name index value for PMM. Reserved index '0'.
const TUint16 KNitzNamePmmIndexValue = 0x0000;
// Call Life Timer index value for PMM. Reserver index '1'.
const TUint16 KCallLifeTimerPmmIndexValue = 0x0001;

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class TInetAddr;

// CLASS DECLARATION

/**
* CMmStaticUtility contains commmonly used static utility functions.
*/
class CMmStaticUtility : public CBase
    {
    public: // New functions

        /**
        * Stores the core error code to lower 16 bits and adds the extended
        * error code to higher 16 bits and returns the error code that can be
        * sent to ETel Server.
        * @param TInt aCoreErrorCode
        * @param TInt aExtendedErrorCode
        * @return TInt: KErrNone or error value
        */
        static TInt EpocErrorCode( TInt aCoreErrorCode,
            TInt aExtendedErrorCode );

        /**
        * Converts ISI cause to EPOC cause value
        * @param TInt aResource
        * @param TUint8 aIsiCauseType
        * @param TUint8 aIsiCauseValue
        * @return TInt: KErrNone or error value
        */
        static TInt CSCauseToEpocError( TInt aResource,
            TUint8 aIsiCauseType,
            TUint8 aIsiCauseValue );

        /**
        * Converts UICC cause to EPOC cause value
        * @param TUint8 aIsiCauseValue
        * @return TInt: KErrNone or error value
        */
        static TInt UICCCSCauseToEpocError( TUint8 aIsiCauseValue );

        /**
        * Converts Packet Data related ISI cause to EPOC cause value
        * @param TUint8 aIsiCauseValue: cause value
        * @param TInt aResource: resource
        * @param TUint8 aIsiCauseType: Cause type
        * @return TInt: EPOC error code
        */
        static TInt PacketDataCSCauseToEpocError( TUint8 aIsiCauseValue,
            TInt aResource = PN_GPDS,
            TUint8 aIsiCauseType = 0x00 );

        /**
        * Converts Gpds Pdp type to Packet API Pdp type
        * @param RPacketContext::TProtocolType& aPdpTypeClient: Pdp type to
        * client
        * @param TUint8 aPdpTypeServer: Pdp type from Gpds
        * @return TInt: EPOC error code
        */
        static void MapPdpTypeToClient(
            RPacketContext::TProtocolType& aPdpTypeClient,
            TUint8 aPdpTypeServer );
        /**
        * Converts GPDS IP address to Client format suitable to be used
        * with TInetAddr::Input. IPv4 and IPv6 are supported
        * @param TDesC8& aGPDSAddres: GPDS IP address
        * @param TDes8& aInetAddr: Clients IP address
        */
        static void ConvertIPAddressToClient(
            const TDesC8& aGPDSAddres,
            TDes8& aInetAddr );
        /**
        * Converts number string to integer
        * @param TUint& integer: integer value
        * @param TDesC& buffer: buffer containing the number string
        */
        static void GetIntFromDescriptor( TUint& integer, TDesC& buffer );

        // Network utility function
        /**
        * Form operator code from MNC and MCC codes.
        * @param TBuf8<3>& aOperatorCode: Buffer for MNC and MCC code
        * @param TBuf<4>* aMnc: Pointer to MNC code
        * @param TBuf<3>* aMcc: Pointer to MCC code
        */
        static void FormOperatorCode( TBuf8<3>& aOperatorCode,
            const TBuf<4>* aMnc,
            const TBuf<3>* aMcc );

        /**
        * Extract and return MCC codes from operator code.
        * @param TPtrC8 aOperatorCode: Combined MCC and MNC
        * @param TUint16* aMcc1: 1st number of MCC code
        * @param TUint16* aMcc2: 2nd number of MCC code
        * @param TUint16* aMcc3: 3rd number of MCC code
        */
        static void GetMccCodes( TPtrC8 aOperatorCode,
            TUint16* aMcc1,
            TUint16* aMcc2,
            TUint16* aMcc3 );

        /**
        * Extract and return MNC codes from operator code.
        * @param TPtrC8 aOperatorCode: Combined MCC and MNC
        * @param TUint16* aMnc1: 1st number of MNC code
        * @param TUint16* aMnc2: 2nd number of MNC code
        * @param TUint16* aMnc3: 3rd number of MNC code
        */
        static void GetMncCodes( TPtrC8 aOperatorCode,
            TUint16* aMnc1,
            TUint16* aMnc2,
            TUint16* aMnc3 );

        /**
        * Calculates mcc and mnc numbers from given digits
        * @param TUint& aMccNumber: Mcc number
        * @param TUint& aMncNumber: Mnc number
        * @param TUint16 aMcc1: first mcc digit
        * @param TUint16 aMcc2: second mcc digit
        * @param TUint16 aMcc3: third mcc digit
        * @param TUint16 aMnc1: first mnc digit
        * @param TUint16 aMnc2: second mnc digit
        * @param TUint16 aMnc3: third mnc digit
        */
        static void CalculateMccMncNumbers( TUint& aMccNumber,
            TUint& aMncNumber,
            TUint16 aMcc1,
            TUint16 aMcc2,
            TUint16 aMcc3,
            TUint16 aMnc1,
            TUint16 aMnc2,
            TUint16 aMnc3 );

        /**
        * Determine if operinfo is APAC
        * @return ETrue if APAC is in use
        */
        static TBool IsOperinfoApac();


        /**
        * Converts data to UCS format
        * @param aInputString Input string
        * @param aLength Length
        * @param aUcs2String Output string
        * @return None
        */
        static void ConvertGsmDataToUcs2(
            TDes8& aInputString,
            TUint16 aLength,
            TDes8& aUcs2String );

        /**
        * Get unicode character
        * @param aCharacter Input character
        * @return Unicode character
        */
        static TUint16 GetUnicodeGsm( TUint8 aCharacter);

        /**
        * Converts data to GSM format
        * @param aInputString Input string
        * @param aGsmDataString Output string
        * @return None
        */
        static void ConvertUcs2ToGsmUcs2Data(
                TDesC16& aInputString,
                TDes8& aGsmDataString );
        
        /**
        * Gets the coding scheme GSM default or UCS2
        * @param aInputString Input string
        * @return ETrue or EFalse
        */
        static TBool GetCodingScheme(
                TDesC16& aInputString );
        
        /**
        * Get gsm unicode character
        * @param aCharacter Input character
        * @return Unicode character
        */
        static TUint8 GetGsmForUnicode( TUint16 aCharacter);

        /**
        * Gets 16 bits from the aSource
        * @param const TDes8& aSource
        * @param TInt aIndex: Index of the data in aSource
        * @return TInt
        */
        static TInt Get16Bit(
            const TDesC8& aSource, //Source
            TInt aIndex );         //Index

        /**
        * Set value for oper info
        * THIS IS ONLY FOR WINSCW TESTING USE
        */
#if __WINSCW__
        IMPORT_C static void SetOperinfoApac( TBool aApac );
#endif // __WINSCW__

    };

// INLINE FUNCTION IMPLEMENTATIONS

// -----------------------------------------------------------------------------
// CMmStaticUtility::EpocErrorCode
// Stores the core error code to lower 16 bits and adds the
// extended error code to higher 16 bits and returns the error code that can
// be sent to ETel Server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CMmStaticUtility::EpocErrorCode
        (
        TInt aCoreErrorCode, // core error
        TInt aExtendedErrorCode // extended error
        )
    {
    TFLOGSTRING3("TSY: CMmStaticUtility::EpocErrorCode, Error mapping done,\
        Core error: %d, Extended error: %d",
        aCoreErrorCode,
        aExtendedErrorCode);

    TInt errorCode = ( aCoreErrorCode & 0x0000FFFF ) |
        ( aExtendedErrorCode << 16 );

    TFLOGSTRING2("TSY: CMmStaticUtility::EpocErrorCode. It was mapped\
        to the following Symbian OS error: %d",
        errorCode);

    return errorCode;
    }

#endif // CMMSTATICUTILITY_H

// End of file
