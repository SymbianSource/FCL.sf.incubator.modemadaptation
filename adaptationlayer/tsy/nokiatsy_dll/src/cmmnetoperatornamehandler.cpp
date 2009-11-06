/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmnetoperatornamehandler.h"
#include "cmmmessagerouter.h"
#include "cmmphonemesshandler.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h"
#include "toperatornames.h"
#include "tsylogger.h"

#include <etelmm.h>
#include <mmlist.h> // For network list functionality
#include <net_modemisi.h>
#include <infoisi.h>
#include <product_profile_definitions.h>
#include <tisi.h>
//#include <permisi.h> To be done in CPS

#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmnetoperatornamehandlertraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
const TUint8 KNetTransId = 0x04; // TO BE DONE WITH INFO_PP_DATA_READ_REQ
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
const TUint8 KNetPadding = 0x00;

// PNN record indentifiers in HPLMN.
const TUint8 KNetNoHplmnPnnRecordNumber = 0x00;
const TUint8 KNetHplmnPnnRecordNumber = 0x01;

// Max buffer length for Operator name.
const TInt KMaxLengthOfOperatorName = 124;

// Max data string length in FULL_NAME or SHORT_NAME.
// Length is enough big to hold max data length converted
// to 7-bit characters.
const TInt KDataStringMaxLength = 300;

// Shifting & masking.
const TUint8 KSiftedSixTimes = 0x06;
const TUint8 KSpareBitsMask = 0x07; // Mask bits 1-3.
const TUint8 KCountryInitialsMask = 0x08; // Mask bit 4.
const TUint8 KCodingSchemeMask = 0x70; // Mask bits 5-7.

// Wild char mask for BCD code checking.
const TUint8 KBCDWildChar = 0x0D;
// Character mask.
const TUint8 KBCDOneCharMask = 0x0F;

// Extended table.
const TUint8 KExtendedTable = 0x1B;

// Message data buffer length for PMM write request (74 bytes).
const TUint8 KPmmDataLengthWriteReq = 0x4A;
// Message data buffer length for PMM read request (14 bytes).
const TUint8 KPmmDataLengthReadReq = 0x0E;
// NITZ data length for PMM (64 bytes).
const TUint32 KNitzPmmDataLength = 0x00000040;
// NITZ data read offset values for PMM.
const TUint32 KNitzPmmDataOffset = 0x00000000;
//const TUint8 KNitzPmmLongNameOffset = 0x04; To be done in CPS
//const TUint8 KNitzPmmShortNameOffset = 0x2C; To be done in CPS
// NITZ name lengths in 8bit buffers.
//const TUint8 KNitzPmmLongNamelength8Bit = 40; To be done in CPS
//const TUint8 KNitzPmmShortNameLength8Bit = 20; To be done in CPS

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES

// GSM 7-bit -> unicode translation table.
// See 3GPP TS 23.038.
const TUint16 KUnicode[128] =
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

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ============================= LOCAL FUNCTIONS ===============================
    //None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::CMmNetOperatorNameHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmNetOperatorNameHandler::CMmNetOperatorNameHandler()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CMmNetOperatorNameHandler()");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_CMMNETOPERATORNAMEHANDLER, "CMmNetOperatorNameHandler::CMmNetOperatorNameHandler" );
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmNetOperatorNameHandler* CMmNetOperatorNameHandler::NewL
        (
        CMmNetMessHandler* aNetMessHandler // Pointer to the NetMessHandler.
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_NEWL, "CMmNetOperatorNameHandler::NewL" );

    CMmNetOperatorNameHandler* netOperatorNameHandler =
        new ( ELeave ) CMmNetOperatorNameHandler();

    CleanupStack::PushL( netOperatorNameHandler );

    netOperatorNameHandler->iNetMessHandler = aNetMessHandler;
    netOperatorNameHandler->ConstructL();

    CleanupStack::Pop( netOperatorNameHandler );

    return netOperatorNameHandler;
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConstructL()");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_CONSTRUCTL, "CMmNetOperatorNameHandler::ConstructL" );

    // Initialization of NITZ data.
    iNitzName.iMCC = 0;
    iNitzName.iMNC = 0;
    iNitzName.iLongName.Zero();
    iNitzName.iShortName.Zero();

    // Initialization of EONS data.
    iEonsName.iMCC = 0;
    iEonsName.iMNC = 0;
    iEonsName.iLongName.Zero();
    iEonsName.iShortName.Zero();
    iEonsName.iPNNIdentifier = 0;

    // Initialization of NITZ name stored to PMM.
    iNitzNamePmm.iMCC = 0;
    iNitzNamePmm.iMNC = 0;
    iNitzNamePmm.iLongName.Zero();
    iNitzNamePmm.iShortName.Zero();

    // Initialize CustomAPI values.
    iOperNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
    iOperNameInfo.iName.Zero();

    // Initialization of Operator Name String (ONS).
    iOperatorNameString.Zero();

    // Operator PLMN list available flag initialization.
    iOplListAvailable = EFalse;

    // OPL rules read flag initialization.
    iOplRulesRead = EFalse;

    // OPL rule match flag initialization.
    iOplRuleMatch = EFalse;

    // Initialize.
    iFromGetOperatorName = EFalse;
    iFromGetManualSearchOperatorName = EFalse;

    iManualSearchIndexValue = 0;

    // Initialization of PP bit value, initialized to non used value.
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    iInfoPpEonsFeatValue = 0xFF;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // TO BE DONE WITH INFO_PP_DATA_READ_RESP
    iInfoPpEonsFeatValue = INFO_PP_EONS_FEAT_0;
    
    // Read INFO_PP_EONS_FEAT PP bit value.
    InfoPpReadReq();

    // Read NITZ data from PMM.
    PermPmRecordReadReq();
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::~CMmNetOperatorNameHandler
// Destructor
// -----------------------------------------------------------------------------
//
CMmNetOperatorNameHandler::~CMmNetOperatorNameHandler()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::~CMmNetOperatorNameHandler()");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_CMMNETOPERATORNAMEHANDLER, "CMmNetOperatorNameHandler::~CMmNetOperatorNameHandler" );

    // Closes the array and frees all memory allocated to the array.
    iOperatorPlmnListTable.Close();
    iManualSearchNetworkList.Close();
    iManualSearchDataList.Close();
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::NetNitzNameInd
// Breaks a NET_NITZ_NAME_IND ISI-message.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::NetNitzNameInd
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::NetNitzNameInd");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_NETNITZNAMEIND, "CMmNetOperatorNameHandler::NetNitzNameInd" );

    // Reset NITZ data.
    iNitzName.iMCC = 0;
    iNitzName.iMNC = 0;
    iNitzName.iLongName.Zero();
    iNitzName.iShortName.Zero();

    // Initialize.
    TUint tempMCC( 0 ); // Mobile Country Code
    TUint tempMNC( 0 ); // Mobile Network Code
    TUint sbStartOffSet( 0 );

    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode;
    RMobilePhone::TMobilePhoneNetworkIdentity networkId;

    // Get the operator code. Size is 3 bytes.
    TPtrC8 operatorCode( aIsiMessage.GetData(
        ISI_HEADER_SIZE + NET_NITZ_NAME_IND_OFFSET_OPERATORCODE,
        3 ) );

    // Map the operator code and country code to symbian values.
    iNetMessHandler->MapOperatorAndCountryCode(
                         operatorCode,
                         networkId,
                         countryCode );

    // Convert descriptor contained number to integer.
    CMmStaticUtility::GetIntFromDescriptor( tempMCC, countryCode );
    CMmStaticUtility::GetIntFromDescriptor( tempMNC, networkId );

    // Store Country Code and Network Code.
    iNitzName.iMCC = tempMCC;
    iNitzName.iMNC = tempMNC;

    // Get NET_FULL_NITZ_NAME sub block.
    TInt retValue( aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_NET_NITZ_NAME_IND,
        NET_FULL_NITZ_NAME,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) );

    if ( KErrNone == retValue )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::NetNitzNameInd - NET_FULL_NITZ_NAME sub block");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_NETNITZNAMEIND, "CMmNetOperatorNameHandler::NetNitzNameInd - NET_FULL_NITZ_NAME sub block" );
        // Temporaly buffer for Long NITZ name.
        TBuf<KMaxLengthOfOperatorName> longNitzName;

        // Get data length.
        TUint8 dataLength( aIsiMessage.Get8bit(
            sbStartOffSet + NET_FULL_NITZ_NAME_OFFSET_DATALENGTH ) );

        // Get NITZ data from data field.
        TPtrC8 nitzFullNameData( aIsiMessage.GetData(
            sbStartOffSet + NET_FULL_NITZ_NAME_OFFSET_DATA,
            dataLength ) );

        // Convert NITZ data field to NITZ operator name.
        ConvertOperatorName(
            ENetNitzName,
            iNitzName.iMCC,
            nitzFullNameData,
            longNitzName );

        // Store NITZ Long name.
        iNitzName.iLongName.Copy( longNitzName.Left(
            iNitzName.iLongName.MaxLength() ) );
        }

    // Get NET_SHORT_NITZ_NAME sub block.
    retValue = aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_NET_NITZ_NAME_IND,
        NET_SHORT_NITZ_NAME,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet );

    if ( KErrNone == retValue )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::NetNitzNameInd - NET_SHORT_NITZ_NAME sub block");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_NETNITZNAMEIND, "CMmNetOperatorNameHandler::NetNitzNameInd - NET_SHORT_NITZ_NAME sub block" );
        // Temporaly buffer for Short NITZ name.
        TBuf<KMaxLengthOfOperatorName> shortNitzName;

        // Get data length.
        TUint8 dataLength( aIsiMessage.Get8bit(
            sbStartOffSet + NET_SHORT_NITZ_NAME_OFFSET_DATALENGTH ) );

        // Get NITZ data from data field.
        TPtrC8 nitzShortNameData( aIsiMessage.GetData(
            sbStartOffSet + NET_SHORT_NITZ_NAME_OFFSET_DATA,
            dataLength ) );

        // Convert NITZ data field to NITZ operator name.
        ConvertOperatorName(
            ENetNitzName,
            iNitzName.iMCC,
            nitzShortNameData,
            shortNitzName );

        // Store NITZ Short name.
        iNitzName.iShortName.Copy( shortNitzName.Left(
            iNitzName.iShortName.MaxLength() ) );
        }

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iMCC: %d", iNitzName.iMCC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iMNC: %d", iNitzName.iMNC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iLongName: %S", &iNitzName.iLongName);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iShortName: %S", &iNitzName.iShortName);
OstTrace1( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_NETNITZNAMEIND, "CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iMCC=%u", iNitzName.iMCC );
OstTrace1( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_NETNITZNAMEIND, "CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iMNC=%u", iNitzName.iMNC );
OstTraceExt1( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_NETNITZNAMEIND, "CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iLongName=%S", iNitzName.iLongName );
OstTraceExt1( TRACE_NORMAL, DUP6_CMMNETOPERATORNAMEHANDLER_NETNITZNAMEIND, "CMmNetOperatorNameHandler::NetNitzNameInd - iNitzName.iShortName=%S", iNitzName.iShortName );

    // Compare NITZ name got in NET_NITZ_NAME_IND to stored values in PMM.
    CompareNitzNameToPmmValues();
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::ConvertOperatorName
// Convert received data to operator name string according to coding scheme.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::ConvertOperatorName
        (
        TNetOperatorNameType aNameType, // Name type
        TUint aMCC, // Country code
        TPtrC8& aNameData, // Operator name data
        TDes& aOperatorName // Converted operator name
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConvertOperatorName");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_CONVERTOPERATORNAME, "CMmNetOperatorNameHandler::ConvertOperatorName" );

    // Network Name information element.
    // See 3GPP 24.008 figure 10.5.80 and table 10.5.94.
    //
    //   8   7   6   5   4   3   2   1
    //  -------------------------------
    // |   |     Network Name IEI      |octet 1
    // |-------------------------------|
    // |Length of Network name contents|octet 2
    // |-------------------------------|
    // |ext|  coding   |Add| number of |
    // | 1 |  scheme   |CI | spare bits|octet 3
    // |-------------------------------|
    // |                               |octet 4
    // |          Text String          |  to
    // |                               |octet n
    //  -------------------------------

    // Initialze values for offsets.
    TInt offsetValueForOctet2( 0 );
    TInt offsetValueForOctet3( 0 );
    TInt offsetValueForOctet4( 0 );

    // Initialize length of Text String.
    TInt lengthOfTextString( 0 );

    // Operator name max length.
    TInt operatorNameMaxLength( aOperatorName.MaxLength() );

    // Check what name type is handled.
    if ( ENetEonsName == aNameType )
        {
        // EONS name data doesn't include Network Name IEI octed
        // and Length of Network Name contents octed,
        // so we need adjust offsets for reading correct position.
        offsetValueForOctet3 = 0;
        offsetValueForOctet4 = 1;

        // Get length of Text String.
        // Data includes also octet3, take that out
        // for real length of text string.
        lengthOfTextString = aNameData.Length() - 1;
        }
    else if ( ENetNitzName == aNameType )
        {
        // NITZ name data handling.
        // Offsets are according to figure 10.5.80.
        offsetValueForOctet2 = 1;
        offsetValueForOctet3 = 2;
        offsetValueForOctet4 = 3;

        // Get length of Text String.
        // Length of Network name contents includes also octet3,
        // take that out for real length of text string.
        lengthOfTextString = aNameData[offsetValueForOctet2] - 1;
        }

    // Get number of spare bits in last octet.
    // Mask bits 1-3 from octet 3.
    TUint8 numberOfSpareBits(
        aNameData[offsetValueForOctet3] & KSpareBitsMask );

    // Check out if Country's initials is needed.
    // Mask bit 4 from octet 3.
    TBool countryInitialsNeeded(
        aNameData[offsetValueForOctet3] & KCountryInitialsMask );

    // Get used Coding Scheme.
    // Mask bits 5-7 from octet 3.
    TUint8 codingSchemeInUse(
        aNameData[offsetValueForOctet3] & KCodingSchemeMask );

    // Pointer to operator name Text String.
    TPtrC8 textString( aNameData.Mid(
        offsetValueForOctet4,
        lengthOfTextString ) );

    // Add country's initials if needed to operator name string.
    if ( countryInitialsNeeded )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConvertOperatorName - Country's Initials needed");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_CONVERTOPERATORNAME, "CMmNetOperatorNameHandler::ConvertOperatorName - Country's Initials needed" );

        // Initialize ret value.
        TInt ret( KErrNotFound );

        // Country's initials is needed.
        ret = TOperatorNames::GetCountryID( aMCC, aOperatorName );

        // Country was found and initials has added.
        if ( KErrNone == ret )
            {
            // Add also 'space' between Country's initials
            // and operator name string.
            aOperatorName.Append(' ');
            }
        // No else.

        }

    // Check Coding Scheme and convert string as needed.
    if ( EGSMDefaultAlphabetCoding == codingSchemeInUse )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConvertOperatorName - 7-Bit coding used");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_CONVERTOPERATORNAME, "CMmNetOperatorNameHandler::ConvertOperatorName - 7-Bit coding used" );
        // Used coding is GSM 7-bit Default alphabet.
        //
        // Calculate correct value of GSM 7-bit characters.
        // numberOf7BitChars includes count of 7-bit characters
        // in 8-bit text string buffer.
        TInt numberOf7BitChars(
            ( ( lengthOfTextString * 8 ) - numberOfSpareBits ) / 7 );

        // Temporaly buffer for 8-bit string.
        TBuf8<KDataStringMaxLength> string8Bit;
        // Convert string first to 8-bit format.
        Packed7to8Format( numberOf7BitChars, textString, string8Bit );

        // Convert 8-bit string to unicode 16-bit format.
        // Temporaly buffer for 16-bit string.
        TBuf<KDataStringMaxLength> converted16BitString;
        // Convert to 16-bit.
        ConvertGsm7ToUnicode16( converted16BitString, string8Bit );

        // Converted string length.
        TInt converted16BitStringLength( converted16BitString.Length() );

        // Append operator name Text String.
        for ( TInt i = 0;
            i < converted16BitStringLength && operatorNameMaxLength > aOperatorName.Length();
            i++ )
            {
            aOperatorName.Append( converted16BitString[i] );
            }
        }
    else if ( EUCS2Coding == codingSchemeInUse )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConvertOperatorName - UCS2 coding used");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_CONVERTOPERATORNAME, "CMmNetOperatorNameHandler::ConvertOperatorName - UCS2 coding used" );
        // Used coding is UCS2(16-bit).
        //
        // Append operator name Text String.
        for ( TInt i = 0;
            i < lengthOfTextString && operatorNameMaxLength > aOperatorName.Length();
            i += 2 )
            {
            aOperatorName.Append(
                TUint16( textString[i + 1] | ( textString[i] << 8 ) ) );
            }
        }
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::ConvertOperatorName - Converted Operator Name: %S", &aOperatorName);
OstTraceExt1( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_CONVERTOPERATORNAME, "CMmNetOperatorNameHandler::ConvertOperatorName - Converted Operator Name=%S", aOperatorName );
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::GetOperatorName
// Gets correct Operator name according priority order.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::GetOperatorName
        (
        TBool aCampedInHplmn, // Camped in HPLMN
        TUint8 aNetworkType, // Network type
        const TDesC8& aOperatorCode, // Operator Code in BCD format
        const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea, // LAC info
        RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkName // Network info
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName" );

    // Buffer for Display tag.
    TBuf<KDisplayTagLength> tempDisplayTag;
    // Hard coded name type.
    TInt hardCodedNameType( ENetNameNotSet );
    // PNN record number.
    TUint8 tempPnnIdentifier( 0 );
    // Reset operator name info data.
    iOperNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
    iOperNameInfo.iName.Zero();

    // Is NITZ name checking needed. Value initialization to ETrue.
    TBool checkNitzName( ETrue );

    // Initialize value for completing.
    TBool completeNow( ETrue );

    // Location Area Code.
    TUint lac( aLocationArea.iLocationAreaCode );

    // Initialize.
    TUint tempMCC( 0 ); // Country Code.
    TUint tempMNC( 0 ); // Network Code.
    // Convert descriptor contained number to integer.
    CMmStaticUtility::GetIntFromDescriptor(
        tempMCC,
        aNetworkName.iCountryCode );
    CMmStaticUtility::GetIntFromDescriptor(
        tempMNC,
        aNetworkName.iNetworkId );

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName - Current MCC: %d", tempMCC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName - Current MNC: %d", tempMNC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName - Current LAC: %d", lac);
TFLOGSTRING4("TSY: CMmNetOperatorNameHandler::GetOperatorName - Operator Code BCD string: %02X %02X %02X", aOperatorCode[0], aOperatorCode[1], aOperatorCode[2]);
OstTrace1( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - Current MCC=%u", tempMCC );
OstTrace1( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - Current MNC=%u", tempMNC );
OstTrace1( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - Current LAC=%u", lac );
OstTraceExt3( TRACE_NORMAL, DUP10_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - Operator Code BCD string=%hhx %hhx %hhx", aOperatorCode[0], aOperatorCode[1], aOperatorCode[2] );

    // Starting to get Network name in priority order.
    //
    // Get Hard coded name from operators.h
    hardCodedNameType = TOperatorNames::GetName(
        tempMCC,
        tempMNC,
        tempDisplayTag );

    // Handle ONS and Hard coded name.
    HandleOnsAndHardCodedName(
        aNetworkType,
        hardCodedNameType,
        tempDisplayTag,
        aNetworkName );

    // Check PP bit INFO_PP_EONS_FEAT value impact to PNN record
    // when registered to HPLMN.
    if ( aCampedInHplmn )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - Registered to HPLMN");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - Registered to HPLMN" );
        // Handle PP bit values.
        tempPnnIdentifier = HandlePnnRecordWithPPbitValueInHplmn(
            aOperatorCode,
            lac );

        // Is there PNN record to read.
        if ( KNetNoHplmnPnnRecordNumber != tempPnnIdentifier )
            {
            // If matching PNN record is not stored in internal memory
            // it will be read from SIM.

            // Check if matching PNN record is same than stored
            // PNN record number.
            if ( iEonsName.iPNNIdentifier == tempPnnIdentifier )
                {
                // Matching PNN record has read earlier from SIM,
                // no need to read again.
                // Copy EONS names if exist.
                if ( 0 < iEonsName.iLongName.Length() )
                    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - PNN Record is read earlier and EONS names exist");
OstTrace0( TRACE_NORMAL, DUP13_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - PNN Record is read earlier and EONS names exist" );
                    CopyEonsName( aNetworkName );

                    // EONS name handled. No need to check NITZ.
                    checkNitzName = EFalse;
                    }
                }
            else
                {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - PNN Record not read earlier");
OstTrace0( TRACE_NORMAL, DUP6_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - PNN Record not read earlier" );
                // Setting iFromGetOperatorName flag to ETrue as
                // SimOperatorReqReadPnn request is sended from
                // GetOperatorName method.
                // This flag is needed to handle correct IPC value.
                iFromGetOperatorName = ETrue;

                // No need to check NITZ.
                checkNitzName = EFalse;

                // Set EFalse as completing will be done after
                // SimOperatorRespReadPnn message is handled.
                // completeNow = EFalse;
                }
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - Not registered to HPLMN");
OstTrace0( TRACE_NORMAL, DUP7_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - Not registered to HPLMN" );
        // OPL rules are read and list available.
        if ( iOplRulesRead && iOplListAvailable )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - OPL Rules handling");
OstTrace0( TRACE_NORMAL, DUP8_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - OPL Rules handling" );
            // EONS name consists of OPL rule and PNN record. One OPL rule consist
            // of PLMN code, LAC range and PNN record number. OPL rules are read
            // from SIM when EMmTsyBootNotifySimStatusReadyIPC is completed and
            // when refresh happens.
            //
            // Note: BCD value of 'D' in any of the MCC and/or MNC digits shall be
            // used to indicate a "wild" value for that corresponding MCC/MNC digit
            // (spec 31.102/4.2.59).
            tempPnnIdentifier = OplRulesChecker( aOperatorCode, lac );

            // Is match found from OPL list.
            if ( iOplRuleMatch )
                {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - OPL Rules match");
OstTrace0( TRACE_NORMAL, DUP9_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - OPL Rules match" );
                // OPL rule exist for current network.
                //
                // If matching PNN record is not stored in internal memory
                // it will be read from SIM.

                // Check if matching PNN record is same than stored
                // PNN record number.
                if ( iEonsName.iPNNIdentifier == tempPnnIdentifier )
                    {
                    // Matching PNN record has read earlier from SIM,
                    // no need to read again.
                    // Copy EONS names if exist.
                    if ( 0 < iEonsName.iLongName.Length() )
                        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - PNN Record is read earlier and EONS names exist");
OstTrace0( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - PNN Record is read earlier and EONS names exist" );
                        CopyEonsName( aNetworkName );

                        // EONS name handled. No need to check NITZ.
                        checkNitzName = EFalse;
                        }
                    }
                else
                    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - PNN Record not read earlier");
OstTrace0( TRACE_NORMAL, DUP11_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - PNN Record not read earlier" );
                    // Setting iFromGetOperatorName flag to ETrue as
                    // SimOperatorReqReadPnn request is sended from
                    // GetOperatorName method.
                    // This flag is needed to handle correct IPC value.
                    iFromGetOperatorName = ETrue;

                    // No need to check NITZ.
                    checkNitzName = EFalse;

                    // Set EFalse as completing will be done after
                    // SimOperatorRespReadPnn message is handled.
                    completeNow = EFalse;
                    }
                }
            }
        }

    // Is NITZ name needed to be checked.
    if ( checkNitzName )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - NITZ name is needed to check");
OstTrace0( TRACE_NORMAL, DUP12_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - NITZ name is needed to check" );
        // Check if NITZ long/short name exist and NITZ data will match for
        // current Country and Network codes.
        NitzNameChecker( tempMCC, tempMNC, aNetworkName );
        }

    // Complete here if not needed to make read request for PNN record.
    if ( completeNow )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetOperatorName - Completing");
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName, Operator display tag: %S", &aNetworkName.iDisplayTag);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName, Operator long name: %S", &aNetworkName.iLongName);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName, Operator short name: %S", &aNetworkName.iShortName);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName, RMmCustomAPI::TOperatorNameInfo, Name: %S", &iOperNameInfo.iName);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetOperatorName, RMmCustomAPI::TOperatorNameInfo, Type: %d", iOperNameInfo.iType);

OstTrace0( TRACE_NORMAL, DUP14_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName - Completing" );
OstTraceExt1( TRACE_NORMAL, DUP15_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName, Operator display tag=%S", aNetworkName.iDisplayTag );
OstTraceExt1( TRACE_NORMAL, DUP16_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName, Operator long name=%S", aNetworkName.iLongName );
OstTraceExt1( TRACE_NORMAL, DUP17_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName, Operator short name=%S", aNetworkName.iShortName );
OstTraceExt2( TRACE_NORMAL, DUP21_CMMNETOPERATORNAMEHANDLER_GETOPERATORNAME, "CMmNetOperatorNameHandler::GetOperatorName, RMmCustomAPI::TOperatorNameInfo - Name=%S;Type=%hhu", iOperNameInfo.iName, iOperNameInfo.iType );

        // Complete ECustomGetOperatorNameIPC.
        // Packed parameter: a RMmCustomAPI::TOperatorNameInfo.
        CMmDataPackage dataPackage;
        dataPackage.PackData( &iOperNameInfo );

        iNetMessHandler->GetMessageRouter()->Complete(
            ECustomGetOperatorNameIPC,
            &dataPackage,
            KErrNone );

        // Complete EMobilePhoneNotifyCurrentNetworkChange with
        // operator names.
        iNetMessHandler->CompleteMobilePhoneNotifyCurrentNetworkChange(
            aLocationArea,
            aNetworkName );

        // Reset operator name info data as these are completed.
        iOperNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
        iOperNameInfo.iName.Zero();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL
// Breaks a NET_MODEM_AVAILABLE_GET_RESP ISI-message and handles succes case.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_HANDLEMANUALSEARCHSUCCESSCASEL, "CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL" );

    TUint8 nbOfSubBlocks( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_MODEM_AVAILABLE_GET_RESP_OFFSET_SUBBLOCKCOUNT ) );

    // There are always 2 sub blocks per one detected network,
    // so divide by 2.
    TUint8 amountOfNetworks( nbOfSubBlocks / 2 );

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL - Found networks: %d", amountOfNetworks);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_HANDLEMANUALSEARCHSUCCESSCASEL, "CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL - Found networks=%hhu", amountOfNetworks );

    // Reset manual network search list array that it can be reused.
    iManualSearchNetworkList.Reset();
    // Reset manual search data list array that it can be reused.
    iManualSearchDataList.Reset();

    // Create the reusable temp variables.
    RMobilePhone::TMobilePhoneNetworkInfoV5 entry;
    TManualSearchDataList tempManualSearchDataList;

    // Set the CDMA specific values to 0, those
    // values remains the same for all entries.
    entry.iAnalogSID.FillZ( 0 );
    entry.iCdmaSID.FillZ( 0 );
    entry.iBandInfo = RMobilePhone::EBandUnknown;

    // The NET Server returns all blocks of same type together,
    // so same network's information is not contiguous in the
    // ISI message, we need two loops to get the information
    // due ISIMSG implementation to get contiguous subblocks.

    // Temporaly array for handling network status.
    RArray<RMobilePhone::TMobilePhoneNetworkStatus> networkStatusList;

    TUint sbStartOffSet( 0 );
    TUint8 subBlockIndex( 1 );

    // Initialize value for data append handled successfully.
    TBool handlingSuccessful( ETrue );

    // Get all NET_MODEM_AVAIL_NETWORK_INFO_COMMON subblocks first,
    // keep data in the networkStatusList.
    for ( TUint8 i = 0; i < amountOfNetworks; i++ )
        {
        // Helper variable.
        RMobilePhone::TMobilePhoneNetworkStatus status(
            RMobilePhone::ENetworkStatusUnknown );

        // SubBlock NET_MODEM_AVAIL_NETWORK_INFO_COMMON.
        if ( ( KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_AVAILABLE_GET_RESP,
            subBlockIndex,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            && ( NET_MODEM_AVAIL_NETWORK_INFO_COMMON == aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_AVAIL_NETWORK_INFO_COMMON_OFFSET_SUBBLOCKID ) ) )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL - NET_MODEM_AVAIL_NETWORK_INFO_COMMON subblock");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_HANDLEMANUALSEARCHSUCCESSCASEL, "CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL - NET_MODEM_AVAIL_NETWORK_INFO_COMMON subblock" );

            // Network status.
            TUint8 networkStatus( aIsiMessage.Get8bit(
                sbStartOffSet +
                NET_MODEM_AVAIL_NETWORK_INFO_COMMON_OFFSET_NETWORKSTATUS ) );

            // Map the network status.
            iNetMessHandler->MapNwStatus( networkStatus, status );

            // Add the status into the status list, at the next empty location.
            networkStatusList.Append( status );
            }

        if ( KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_AVAILABLE_GET_RESP,
            subBlockIndex,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            subBlockIndex++;
            }
        // No else

        sbStartOffSet = 0;
        }

    // Another loop to get the NET_MODEM_DETAILED_NETWORK_INFO subblocks,
    // update the iManualSearchNetworkList and iManualSearchDataList
    // with the new values.
    for ( TUint8 j = 0; j < amountOfNetworks; j++ )
        {
        // SubBlock NET_MODEM_DETAILED_NETWORK_INFO.
        if ( ( KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_AVAILABLE_GET_RESP,
            subBlockIndex,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            && ( NET_MODEM_DETAILED_NETWORK_INFO == aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_SUBBLOCKID ) )
            && handlingSuccessful )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL - NET_MODEM_DETAILED_NETWORK_INFO subblock");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_HANDLEMANUALSEARCHSUCCESSCASEL, "CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL - NET_MODEM_DETAILED_NETWORK_INFO subblock" );
            // Set the network status from the networkStatusList created in the
            // previous loop.
            entry.iStatus = networkStatusList[j];

            // Get the operator code.
            // Length of the operator code is 3 bytes.
            tempManualSearchDataList.iOperatorCode = aIsiMessage.GetData(
                sbStartOffSet +
                NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_OPERCODE,
                3 );

            // Map the Operator and country code to symbian values.
            TBool networkCodeMapped( iNetMessHandler->MapOperatorAndCountryCode(
                tempManualSearchDataList.iOperatorCode,
                entry.iNetworkId,
                entry.iCountryCode ) );

            // Convert descriptor contained number to integer.
            CMmStaticUtility::GetIntFromDescriptor(
                tempManualSearchDataList.iMCC,
                entry.iCountryCode );
            CMmStaticUtility::GetIntFromDescriptor(
                tempManualSearchDataList.iMNC,
                entry.iNetworkId );

            // Get Network type.
            tempManualSearchDataList.iNetworkType = aIsiMessage.Get8bit(
                sbStartOffSet +
                NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_NETWORKTYPE );

            // Get Location Area Code.
            tempManualSearchDataList.iLocationAreaCode = aIsiMessage.Get16bit(
                sbStartOffSet +
                NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_CURRENTLAC );

            // Check if "0" is added at the end of the networkId.
            if ( networkCodeMapped )
                {
                // We need to remove Null string from networkId
                // to match network code received from NET Server.
                // Without this operation in some case network
                // can't be selected due the added "extra null".
                entry.iNetworkId.Delete(
                    ( entry.iNetworkId.Length() - 1 ),
                    1 );
                }

            // Band info.
            TUint8 band( aIsiMessage.Get8bit(
                sbStartOffSet +
                NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_BANDINFO ) );
            // RAT info.
            TUint8 rat( aIsiMessage.Get8bit(
                sbStartOffSet +
                NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_UMTSAVAILABLE ) );

            // Band info = 900_1800.
            // Rat info = Umts not available (GSM only).
            if ( NET_GSM_BAND_900_1800 == band &&
                NET_UMTS_NOT_AVAILABLE == rat )
                {
                entry.iMode = RMobilePhone::ENetworkModeGsm;
                entry.iAccess = RMobilePhone::ENetworkAccessGsm;
                }
            // Band info = Not available.
            // Rat info = Umts available (WCDMA only).
            else if ( NET_GSM_BAND_INFO_NOT_AVAIL == band &&
                NET_UMTS_AVAILABLE == rat )
                {
                entry.iMode = RMobilePhone::ENetworkModeWcdma;
                entry.iAccess = RMobilePhone::ENetworkAccessUtran;
                }
            // Band info = 900_1800.
            // Rat info = Umts available (GSM+WCDMA).
            else if ( NET_GSM_BAND_900_1800 == band &&
                NET_UMTS_AVAILABLE == rat )
                {
                // Add only the WCDMA network
                entry.iMode = RMobilePhone::ENetworkModeWcdma;
                entry.iAccess = RMobilePhone::ENetworkAccessUtran;
                }

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL, MCC: %d", tempManualSearchDataList.iMCC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL, MNC: %d", tempManualSearchDataList.iMNC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL, LocationAreaCode: %d", tempManualSearchDataList.iLocationAreaCode);
OstTrace1( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_HANDLEMANUALSEARCHSUCCESSCASEL, "CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL; MCC=%u", tempManualSearchDataList.iMCC );
OstTrace1( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_HANDLEMANUALSEARCHSUCCESSCASEL, "CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL; MNC=%u", tempManualSearchDataList.iMNC );
OstTrace1( TRACE_NORMAL, DUP6_CMMNETOPERATORNAMEHANDLER_HANDLEMANUALSEARCHSUCCESSCASEL, "CMmNetOperatorNameHandler::HandleManualSearchSuccessCaseL; LocationAreaCode=%u", tempManualSearchDataList.iLocationAreaCode );

            // Add entry to iManualSearchNetworkList array.
            // Catch possible error in append.
            TInt errValue1( iManualSearchNetworkList.Append( entry ) );
            // Add temp manual search data to iManualSearchDataList array.
            // Catch possible error in append.
            TInt errValue2( iManualSearchDataList.Append(
                tempManualSearchDataList ) );

            // Check possible error.
            if ( KErrNone != errValue1 || KErrNone != errValue2 )
                {
                // There was error when append data to array.
                // Set handlingSuccessful to false.
                handlingSuccessful = EFalse;

                // Reset arrays.
                iManualSearchNetworkList.Reset();
                iManualSearchDataList.Reset();
                }

            // Reset the buffers before next loop.
            tempManualSearchDataList.iOperatorCode.Zero();
            entry.iNetworkId.Zero();
            entry.iCountryCode.Zero();
            entry.iDisplayTag.Zero();
            entry.iLongName.Zero();
            entry.iShortName.Zero();
            }

        if ( KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_AVAILABLE_GET_RESP,
            subBlockIndex,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            subBlockIndex++;
            }
        // No else.

        sbStartOffSet = 0;
        }

    // networkStatusList is not needed anymore.
    networkStatusList.Reset();
    networkStatusList.Close();

    // Is handling successful.
    if ( handlingSuccessful )
        {
        // Start index value for manual search network list.
        iManualSearchIndexValue = 0;

        // Get Operator names to all networks.
        // EMobilePhoneGetDetectedNetworksV2Phase1 will be completed after
        // Network names are handled.
        GetManualSearchOperatorNameL();
        }
    else
        {
        // There was error when append data to array,
        // continue to complete.
        CompleteMobilePhoneGetDetectedNetworksV2Phase1L();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::GetManualSearchOperatorNameL
// Gets correct Operator name in manual search.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::GetManualSearchOperatorNameL()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL" );

    // Amount of networks to be checked.
    TUint8 amountOfNetworks( iManualSearchNetworkList.Count() );

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - amount of networks: %d", amountOfNetworks);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - amount of networks=%hhu", amountOfNetworks );

    // Temporaly network name data.
    RMobilePhone::TMobilePhoneNetworkInfoV5 networkName;
    // Temporaly location area data.
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
    // Buffer for Display tag.
    TBuf<KDisplayTagLength> tempDisplayTag;
    // Initialize value for completing.
    TBool completeNow( ETrue );
    // Setting EFalse first.
    // This will be ETrue when all found networks are handled.
    TBool allNetworksInManualSearchHandled( EFalse );

    // Check every network and update operator names.
    // If EONS name needed, then handling of iManualSearchNetworkList
    // continues after PNN record has read and handled.
    while ( ( iManualSearchIndexValue < amountOfNetworks )
        && completeNow )
        {
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Index value: %d", iManualSearchIndexValue);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Index value=%hhu", iManualSearchIndexValue );

        // Hard coded name type.
        TInt hardCodedNameType( ENetNameNotSet );
        // PNN record number.
        TUint8 tempPnnIdentifier( 0 );

        // Country Code and Network Code.
        TUint mcc( iManualSearchDataList[iManualSearchIndexValue].iMCC );
        TUint mnc( iManualSearchDataList[iManualSearchIndexValue].iMNC );
        // Network Type.
        TUint8 networkType(
            iManualSearchDataList[iManualSearchIndexValue].iNetworkType );
        // Location Area Code.
        TUint locationAreaCode(
            iManualSearchDataList[iManualSearchIndexValue].iLocationAreaCode );
        // Operator code in BCD format.
        TBuf8<KBCDLength> operatorCode(
            iManualSearchDataList[iManualSearchIndexValue].iOperatorCode );

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL, MCC: %d", mcc);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL, MNC: %d", mnc);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL, LocationAreaCode: %d", locationAreaCode);
OstTrace1( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL; MCC=%u", mcc );
OstTrace1( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL; MNC=%u", mnc );
OstTrace1( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL; LocationAreaCode=%u", locationAreaCode );

        // Starting to get Network name in priority order.
        //
        // Get Hard coded name from operators.h
        hardCodedNameType = TOperatorNames::GetName(
            mcc,
            mnc,
            tempDisplayTag );

        // Handle ONS and Hard coded name.
        HandleOnsAndHardCodedName(
            networkType,
            hardCodedNameType,
            tempDisplayTag,
            networkName );

        // Copy iDisplayTag to correct index.
        iManualSearchNetworkList[iManualSearchIndexValue].iDisplayTag.Copy(
            networkName.iDisplayTag );

        // Copy location area code to temporaly location area data.
        locationArea.iLocationAreaCode = locationAreaCode;

        // Check PP bit INFO_PP_EONS_FEAT value impact to PNN record
        // when registered to HPLMN.
        if ( NET_GSM_HOME_PLMN == networkType )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Registered to HPLMN");
OstTrace0( TRACE_NORMAL, DUP6_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Registered to HPLMN" );
            // Handle PP bit values.
            tempPnnIdentifier = HandlePnnRecordWithPPbitValueInHplmn(
                operatorCode,
                locationAreaCode );

            // Is there PNN record to read.
            if ( KNetNoHplmnPnnRecordNumber != tempPnnIdentifier )
                {
                // If matching PNN record is not stored in internal memory
                // it will be read from SIM.

                // Check if matching PNN record is same than stored
                // PNN record number.
                if ( iEonsName.iPNNIdentifier == tempPnnIdentifier )
                    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record is read earlier");
OstTrace0( TRACE_NORMAL, DUP7_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record is read earlier" );
                    // Matching PNN record has read earlier from SIM,
                    // no need to read again.
                    // Copy EONS names.
                    CopyEonsName( networkName );
                    }
                else
                    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record not read earlier");
OstTrace0( TRACE_NORMAL, DUP8_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record not read earlier" );
                    // Setting iFromGetManualSearchOperatorName flag to
                    // ETrue as SimOperatorReqReadPnn request is sended
                    // from GetManualSearchOperatorName method.
                    // This flag is needed to handle correct IPC value.
                    iFromGetManualSearchOperatorName = ETrue;

                    // Set EFalse as completing will be done after
                    // SimOperatorRespReadPnn message is handled.
                    // completeNow = EFalse;
                    }
                }
            // No else.
            }
        else
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Not registered to HPLMN");
OstTrace0( TRACE_NORMAL, DUP9_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Not registered to HPLMN" );
            // OPL rules are read and list available.
            if ( iOplRulesRead && iOplListAvailable )
                {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - OPL Rules handling");
OstTrace0( TRACE_NORMAL, DUP10_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - OPL Rules handling" );
                // EONS name consists of OPL rule and PNN record. One OPL rule consist
                // of PLMN code, LAC range and PNN record number. OPL rules are read
                // from SIM when EMmTsyBootNotifySimStatusReadyIPC is completed and
                // when refresh happens.
                //
                // Note: BCD value of 'D' in any of the MCC and/or MNC digits shall be
                // used to indicate a "wild" value for that corresponding MCC/MNC digit
                // (spec 31.102/4.2.59).
                tempPnnIdentifier = OplRulesChecker(
                    operatorCode,
                    locationAreaCode );

                // Is match found from OPL list.
                if ( iOplRuleMatch )
                    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - OPL Rules match");
OstTrace0( TRACE_NORMAL, DUP11_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - OPL Rules match" );
                    // OPL rule exist for current network.
                    //
                    // If matching PNN record is not stored in internal memory
                    // it will be read from SIM.

                    // Check if matching PNN record is same than stored
                    // PNN record number.
                    if ( iEonsName.iPNNIdentifier == tempPnnIdentifier )
                        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record is read earlier");
OstTrace0( TRACE_NORMAL, DUP12_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record is read earlier" );
                        // Matching PNN record has read earlier from SIM,
                        // no need to read again.
                        // Copy EONS names.
                        CopyEonsName( networkName );
                        }
                    else
                        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record not read earlier");
OstTrace0( TRACE_NORMAL, DUP13_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - PNN Record not read earlier" );
                        // Setting iFromGetManualSearchOperatorName flag to
                        // ETrue as SimOperatorReqReadPnn request is sended
                        // from GetManualSearchOperatorName method.
                        // This flag is needed to handle correct IPC value.
                        iFromGetManualSearchOperatorName = ETrue;

                        // Set EFalse as completing will be done after
                        // SimOperatorRespReadPnn message is handled.
                        // completeNow = EFalse;
                        }
                    }
                // No else.
                }
            // No else.
            }

        // EONS name will be added here if exist and if didn't needed
        // to read PNN record.
        // If EONS name didn't exist DisplayTag will copied
        // to long and short name buffers.
        if ( completeNow )
            {
            // Check is EONS Long name exist.
            // If no Long EONS name there is no short EONS name either.
            if ( 0 < networkName.iLongName.Length() )
                {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - EONS name available");
OstTrace0( TRACE_NORMAL, DUP14_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - EONS name available" );
                // Copy EONS Long name to correct index.
                iManualSearchNetworkList[iManualSearchIndexValue].iLongName.Copy(
                    networkName.iLongName );

                // Check is EONS Short name exist.
                if ( 0 < networkName.iShortName.Length() )
                    {
                    // Copy EONS Short name to correct index.
                    iManualSearchNetworkList[iManualSearchIndexValue].iShortName.Copy(
                        networkName.iShortName );
                    }
                }
            else
                {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - No EONS name available, DisplayTag name added to Long and Short names");
OstTrace0( TRACE_NORMAL, DUP15_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - No EONS name available, DisplayTag name added to Long and Short names" );
                // If no EONS name, copy DisplayTag to long and short name buffers.
                networkName.iLongName.Copy(
                    networkName.iDisplayTag.Left(
                    networkName.iLongName.MaxLength() ) );
                networkName.iShortName.Copy(
                    networkName.iDisplayTag.Left(
                    networkName.iShortName.MaxLength() ) );

                iManualSearchNetworkList[iManualSearchIndexValue].iLongName.Copy(
                    networkName.iLongName );
                iManualSearchNetworkList[iManualSearchIndexValue].iShortName.Copy(
                    networkName.iShortName );
                }

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Operator display tag: %S", &networkName.iDisplayTag);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Operator long name: %S", &networkName.iLongName);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Operator short name: %S", &networkName.iShortName);
OstTraceExt1( TRACE_NORMAL, DUP16_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Operator display tag=%S", networkName.iDisplayTag );
OstTraceExt1( TRACE_NORMAL, DUP17_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Operator long name=%S", networkName.iLongName );
OstTraceExt1( TRACE_NORMAL, DUP18_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Operator short name=%S", networkName.iShortName );

            // Update index value.
            iManualSearchIndexValue++;
            }

        // Reset the buffers before next loop.
        networkName.iDisplayTag.Zero();
        networkName.iLongName.Zero();
        networkName.iShortName.Zero();
        } // End of while

    // Check is all found network handled.
    if ( iManualSearchIndexValue == amountOfNetworks )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - All found Networks are handled");
OstTrace0( TRACE_NORMAL, DUP19_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - All found Networks are handled" );
        // All found networks are handled.
        allNetworksInManualSearchHandled = ETrue;
        }

    // All networks found in manual search is now handled.
    // Continue to completing.
    if ( allNetworksInManualSearchHandled && completeNow )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Completing");
OstTrace0( TRACE_NORMAL, DUP20_CMMNETOPERATORNAMEHANDLER_GETMANUALSEARCHOPERATORNAMEL, "CMmNetOperatorNameHandler::GetManualSearchOperatorNameL - Completing" );

        // Complete.
        CompleteMobilePhoneGetDetectedNetworksV2Phase1L();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::InfoPpReadReq
// Creates Product Profile request to read INFO_PP_EONS_FEAT bit.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::InfoPpReadReq()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::InfoPpReadReq");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_INFOPPREADREQ, "CMmNetOperatorNameHandler::InfoPpReadReq" );

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
// TO BE DONE WITH INFO_PP_DATA_READ_REQ
    // Construct INFO_PP_READ_REQ message.
    // Data length is INFO_BATCH_TYPE + sub block count + INFO_SB_PP sub block.
    TBuf8<2 + SIZE_INFO_SB_PP> infoPpReadReqData;

    // Set PP Batch Reading to read specific product profile.
    infoPpReadReqData.Append( INFO_PP_PROD_PROFILE_FEATURE );

    // Add number of sub blocks.
    infoPpReadReqData.Append( 1 );

    // Create INFO_SB_PP sub block.
    TIsiSubBlock sbPPSubBlock(
        infoPpReadReqData,
        INFO_SB_PP,
        EIsiSubBlockTypeId8Len8 );

    // Add 0x00 as the Dynamic Value ID, used to read a static value.
    infoPpReadReqData.Append( 0x00 );

    // Add 0x01 as the length of the product profile, reading only one profile.
    infoPpReadReqData.Append( 0x01 );

    // Add the product profile feature number being requested.
    infoPpReadReqData.Append( INFO_PP_EONS_FEAT );

    // Value of the product profile is 0x00, since reading.
    infoPpReadReqData.Append( 0x00 );

    // Complete sub block and append to INFO_PP_READ_REQ.
    sbPPSubBlock.CompleteSubBlock();

    // Sending message to phonet.
    iNetMessHandler->GetPhoNetSender()->Send(
        PN_INFO,
        KNetTransId,
        INFO_PP_READ_REQ,
        infoPpReadReqData );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::InfoPpReadResp
// Breaks a INFO_PP_READ_RESP ISI-message.
// Gets the status of the INFO_PP_EONS_FEAT values. This value affects to
// deciding of used PNN record number for given PLMN and LAC.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::InfoPpReadResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    // Get status.
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
// TO BE DONE WITH INFO_PP_DATA_READ_RESP  
    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + INFO_PP_READ_RESP_OFFSET_STATUS ) );

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::InfoPpReadResp - Status: %d", status);
OstTraceExt1( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_INFOPPREADRESP, "CMmNetOperatorNameHandler::InfoPpReadResp;status=%hhu", status );

    if ( INFO_OK == status )
        {
        TUint sbStartOffSet( 0 );

        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_INFO_PP_READ_RESP,
            INFO_SB_PP,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            TInt lengthOfPp( aIsiMessage.Get8bit(
                sbStartOffSet + INFO_SB_PP_OFFSET_STRLEN ) );

            if ( lengthOfPp )
                {
                // Buffer to hold product profile feature and its
                // corresponding value, thus buffer size is 2.
                TBuf8<2> pPFeatures;

                // Copy bytes from product profile:
                // 1st byte = PP Feature
                // 2nd byte = PP Value
                pPFeatures.Copy( aIsiMessage.GetData(
                    sbStartOffSet + INFO_SB_PP_OFFSET_PRODUCTPROFILE,
                    2 ) );

                // Check that product profile is requested one.
                if ( INFO_PP_EONS_FEAT == pPFeatures[0] )
                    {
                    // Get the value of used PNN record number configuration.
                    if ( INFO_PP_EONS_FEAT_0 == pPFeatures[1] )
                        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_0");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_INFOPPREADRESP, "CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_0" );
                        // Phone will always assume that there is a name
                        // for HPLMN in PNN#1 (in PNN record number one),
                        // without checking rules from EF-OPL first.
                        iInfoPpEonsFeatValue = INFO_PP_EONS_FEAT_0;
                        }
                    else if ( INFO_PP_EONS_FEAT_1 == pPFeatures[1] )
                        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_1");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_INFOPPREADRESP, "CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_1" );
                        // Phone will go through rules from OPL even when
                        // in service in HPLMN, if rule for HPLMN is not
                        // found from OPL, then it assumes that PNN#1
                        // is reserved to HPLMN.
                        iInfoPpEonsFeatValue = INFO_PP_EONS_FEAT_1;
                        }
                    else if ( INFO_PP_EONS_FEAT_2 == pPFeatures[1] )
                        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_2");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_INFOPPREADRESP, "CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_2" );
                        // Phone will never assume that there is a name
                        // for HPLMN in PNN#1. It will show EONS name for
                        // HPLMN only when it is found from EF-OPL, except
                        // when EF-OPL does not exist at all.
                        iInfoPpEonsFeatValue = INFO_PP_EONS_FEAT_2;
                        }
                    else if ( INFO_PP_EONS_FEAT_3 == pPFeatures[1] )
                        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_3");
OstTrace0( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_INFOPPREADRESP, "CMmNetOperatorNameHandler::InfoPpReadResp - PP Value - INFO_PP_EONS_FEAT_3" );
                        // Phone will never assume that there is a name
                        // for HPLMN in PNN#1. It will show EONS name for
                        // HPLMN only when it is found from EF-OPL, except
                        // when EF-OPL does not exist at all or when only
                        // one PNN record exists in the card.
                        iInfoPpEonsFeatValue = INFO_PP_EONS_FEAT_3;
                        }
                    }
                }
            }
        }
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    }


// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::PermPmRecordWriteReq
// Creates PERM_PM_RECORD_WRITE_REQ ISI message and sends it through phonet.
// This request is used to write NITZ data to PMM.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::PermPmRecordWriteReq()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::PermPmRecordWriteReq");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_PERMPMRECORDWRITEREQ, "CMmNetOperatorNameHandler::PermPmRecordWriteReq" );

    // Temporaly buffers for names.
    RMobilePhone::TMobilePhoneNetworkLongName tempLongName;
    RMobilePhone::TMobilePhoneNetworkShortName tempShortName;

    // Create message data buffer (72 bytes) for
    // - Group ID (2 bytes),
    // - Index (2 bytes),
    // - Filler (1 bytes),
    // - Filler (1 bytes),
    // - Size of Data (4 bytes),
    // - Data ( NITZ data: MCC, MNC, Long name and Short name (64 bytes) ).
    TBuf8<KPmmDataLengthWriteReq> messageData;

    // Append Group ID.
    messageData.Append( static_cast<TUint8>( KPmmGroupNokiaTsy >> 8 ) );
    messageData.Append( static_cast<TUint8>( KPmmGroupNokiaTsy ) );
    // Append Index. NITZ name use first index, value is "0".
    messageData.Append( static_cast<TUint8>( KNitzNamePmmIndexValue >> 8 ) );
    messageData.Append( static_cast<TUint8>( KNitzNamePmmIndexValue ) );
    // Filler.
    messageData.Append( KNetPadding );
    // Filler.
    messageData.Append( KNetPadding );
    // Append Size of Data.
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength >> 24 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength >> 16 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength >> 8 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength ) );
    // Append NITZ Data.
    // - MCC (2 bytes).
    messageData.Append( static_cast<TUint8>( iNitzNamePmm.iMCC >> 8 ) );
    messageData.Append( static_cast<TUint8>( iNitzNamePmm.iMCC ) );
    // - MNC (2 bytes).
    messageData.Append( static_cast<TUint8>( iNitzNamePmm.iMNC >> 8 ) );
    messageData.Append( static_cast<TUint8>( iNitzNamePmm.iMNC ) );
    // - Long name (40 bytes).
    //
    // Copy long name to temp buffer.
    tempLongName.Copy( iNitzNamePmm.iLongName );
    // Check long name length and fill temp name until max length.
    TUint16 zeroValue( 0 );
    while ( tempLongName.MaxLength() > tempLongName.Length() )
        {
        // Fill with zero.
        tempLongName.Append( zeroValue );
        }
    // Append long name.
    for ( TInt i = 0; i < tempLongName.MaxLength(); i++ )
        {
        messageData.Append( static_cast<TUint8>( tempLongName[i] >> 8 ) );
        messageData.Append( static_cast<TUint8>( tempLongName[i] ) );
        }
    // - Short name (20 Bytes).
    //
    // Copy short name to temp buffer.
    tempShortName.Copy( iNitzNamePmm.iShortName );
    // Check short name length and fill temp name until max length.
    while ( tempShortName.MaxLength() > tempShortName.Length() )
        {
        // Fill with zero.
        tempShortName.Append( zeroValue );
        }
    // Append short name.
    for ( TInt i = 0; i < tempShortName.MaxLength(); i++ )
        {
        messageData.Append( static_cast<TUint8>( tempShortName[i] >> 8 ) );
        messageData.Append( static_cast<TUint8>( tempShortName[i] ) );
        }

    // Sending message to phonet.
    /* To be done in CPS
    iNetMessHandler->GetPhoNetSender()->Send(
        PN_PERMANENT_DATA,
        KNetTransId,
        PERM_PM_RECORD_WRITE_REQ,
        messageData );*/
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::PermPmRecordWriteResp
// Breaks a PERM_PM_RECORD_WRITE_RESP ISI message.
// -----------------------------------------------------------------------------
//
/* To be done in CPS
void CMmNetOperatorNameHandler::PermPmRecordWriteResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::PermPmRecordWriteResp");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_PERMPMRECORDWRITERESP, "CMmNetOperatorNameHandler::PermPmRecordWriteResp" );


    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PERM_PM_RECORD_WRITE_RESP_OFFSET_PMMSTATUS ) );

    // Check status.
    if ( PMM_OK == status )
        {
        // NITZ data is successfully stored to PMM.
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::PermPmRecordWriteResp - NITZ data successfully stored to PMM");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDWRITERESP, "CMmNetOperatorNameHandler::PermPmRecordWriteResp - NITZ data successfully stored to PMM" );
        }
    else
        {
        // Storing failed.
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::PermPmRecordWriteResp - NITZ data storing failed, error value: %d", status);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDWRITERESP, "CMmNetOperatorNameHandler::PermPmRecordWriteResp - NITZ data storing failed, error value=%hhu", status );
        }
    } */

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::PermPmRecordReadReq
// Creates PERM_PM_RECORD_READ_REQ ISI message and sends it through phonet.
// This request is used to read record for PMM.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::PermPmRecordReadReq()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::PermPmRecordReadReq");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADREQ, "CMmNetOperatorNameHandler::PermPmRecordReadReq" );

    // Create message data buffer (14 bytes) for
    // - Group ID (2 bytes),
    // - Index (2 bytes),
    // - Filler (1 bytes),
    // - Filler (1 bytes),
    // - Offset (4 bytes),
    // - Data amount to be read (4 bytes).
    TBuf8<KPmmDataLengthReadReq> messageData;

    // Append Group ID.
    messageData.Append( static_cast<TUint8>( KPmmGroupNokiaTsy >> 8 ) );
    messageData.Append( static_cast<TUint8>( KPmmGroupNokiaTsy ) );
    // Append Index. NITZ name use first index, value is "0".
    messageData.Append( static_cast<TUint8>( KNitzNamePmmIndexValue >> 8 ) );
    messageData.Append( static_cast<TUint8>( KNitzNamePmmIndexValue ) );
    // Filler.
    messageData.Append( KNetPadding );
    // Filler.
    messageData.Append( KNetPadding );
    // Append Offset.
    messageData.Append( static_cast<TUint8>( KNitzPmmDataOffset >> 24 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataOffset >> 16 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataOffset >> 8 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataOffset ) );
    // Append Data amount to be read.
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength >> 24 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength >> 16 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength >> 8 ) );
    messageData.Append( static_cast<TUint8>( KNitzPmmDataLength ) );

    // Sending message to phonet.
    /* To be done in CPS
    iNetMessHandler->GetPhoNetSender()->Send(
        PN_PERMANENT_DATA,
        KNetTransId,
        PERM_PM_RECORD_READ_REQ,
        messageData );*/
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::PermPmRecordReadResp
// Breaks a PERM_PM_RECORD_READ_RESP ISI message.
// -----------------------------------------------------------------------------
//
/* To be done in CPS
void CMmNetOperatorNameHandler::PermPmRecordReadResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::PermPmRecordReadResp");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADRESP, "CMmNetOperatorNameHandler::PermPmRecordReadResp" );

    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PERM_PM_RECORD_READ_RESP_OFFSET_PMMSTATUS ) );

    // Check status.
    if ( PMM_OK == status )
        {
        // NITZ data is successfully read from PMM.
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::PermPmRecordReadResp - NITZ data successfully read from PMM");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADRESP, "CMmNetOperatorNameHandler::PermPmRecordReadResp - NITZ data successfully read from PMM" );

        // NITZ data buffer has room for:
        // - MCC, MNC, Long name and Short name ( 64 bytes ).
        TBuf8<KNitzPmmDataLength> readNitzData;

        // Temp Buffers.
        TBuf8<KNitzPmmLongNamelength8Bit> tempLongName8Bit;
        TBuf8<KNitzPmmShortNameLength8Bit> tempShortName8Bit;

        TUint16 byte1( 0 );
        TUint16 byte2( 0 );

        // Mobile Country Code.
        TUint tempMCC( 0 );
        // Mobile Network Code.
        TUint tempMNC( 0 );
        // Long Name.
        RMobilePhone::TMobilePhoneNetworkLongName tempLongName;
        // Short Name.
        RMobilePhone::TMobilePhoneNetworkShortName tempShortName;

        TUint sbStartOffSet( 0 );

        // Check PERM_SB_PM_DATA subblock.
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_PERM_PM_RECORD_READ_RESP,
            PERM_SB_PM_DATA,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // Get data size.
            TUint32 nitzPmmDataLength( aIsiMessage.Get32bit(
                sbStartOffSet + PERM_SB_PM_DATA_OFFSET_SIZE ) );

            // Get read NITZ data to temp buffer.
            readNitzData = aIsiMessage.GetData(
                sbStartOffSet + PERM_SB_PM_DATA_OFFSET_DATA,
                nitzPmmDataLength );

            // Get Mobile Country Code.
            // - Buffer bytes 0-1 includes MCC value.
            byte1 = readNitzData[0];
            byte2 = readNitzData[1];
            tempMCC = TUint16( ( byte1 << 8 ) | byte2 );

            // Get Mobile Network Code.
            // - Buffer bytes 2-3 includes MNC value.
            byte1 = readNitzData[2];
            byte2 = readNitzData[3];
            tempMNC = TUint16( ( byte1 << 8 ) | byte2 );

            // Get long name.
            // - Buffer bytes 4-43 includes long name.
            tempLongName8Bit.Copy( readNitzData.Mid(
                KNitzPmmLongNameOffset,
                KNitzPmmLongNamelength8Bit ) );
            // Get short name.
            // - Buffer bytes 44-63 includes short name.
            tempShortName8Bit.Copy( readNitzData.Mid(
                KNitzPmmShortNameOffset,
                KNitzPmmShortNameLength8Bit ) );
            // Convert.
            TIsiUtility::CopyFromBigEndian(
                tempLongName8Bit,
                tempLongName );
            TIsiUtility::CopyFromBigEndian(
                tempShortName8Bit,
                tempShortName );

            // Check names and strip NULL chars from end if exist.
            TInt longNameNullPos( tempLongName.Locate( NULL ) );
            if ( KErrNotFound != longNameNullPos )
                {
                tempLongName.SetLength( longNameNullPos );
                }

            TInt shortNameNullPos( tempShortName.Locate( NULL ) );
            if ( KErrNotFound != shortNameNullPos )
                {
                tempShortName.SetLength( shortNameNullPos );
                }

            // Copy NITZ data got from PMM.
            iNitzNamePmm.iMCC = tempMCC;
            iNitzNamePmm.iMNC = tempMNC;
            iNitzNamePmm.iLongName.Copy( tempLongName );
            iNitzNamePmm.iShortName.Copy( tempShortName );

            // Copy name data to iNitzName struct to keep PMM data in handle.
            iNitzName = iNitzNamePmm;
            }
        }
    else
        {
        // Reading failed. No data stored or other error when reading.
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::PermPmRecordReadResp - NITZ data reading failed from PMM, error value: %d", status);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADRESP, "CMmNetOperatorNameHandler::PermPmRecordReadResp - NITZ data reading failed from PMM, error value=%hhu", status );
        }

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iMCC: %d", iNitzNamePmm.iMCC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iMNC: %d", iNitzNamePmm.iMNC);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iLongName: %S", &iNitzNamePmm.iLongName);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iShortName: %S", &iNitzNamePmm.iShortName);
OstTrace1( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADRESP, "CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iMCC=%u", iNitzNamePmm.iMCC );
OstTrace1( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADRESP, "CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iMNC=%u", iNitzNamePmm.iMNC );
OstTraceExt1( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADRESP, "CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iLongName=%S", iNitzNamePmm.iLongName );
OstTraceExt1( TRACE_NORMAL, DUP6_CMMNETOPERATORNAMEHANDLER_PERMPMRECORDREADRESP, "CMmNetOperatorNameHandler::PermPmRecordReadResp - iNitzNamePmm.iShortName=%S", iNitzNamePmm.iShortName );
    }*/

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::HandleOnsAndHardCodedName
// Handles ONS and Hard coded names in priority.
// Add also Operator name info data ( RMmCustomAPI::TOperatorNameInfo ).
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::HandleOnsAndHardCodedName
        (
        TUint8 aNetworkType, // Network type.
        TInt aHardCodedNameType, // Hard coded name type.
        const TDes& aDisplayTag, // Hard coded operator name.
        RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkName // Network name data.
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleOnsAndHardCodedName");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_HANDLEONSANDHARDCODEDNAME, "CMmNetOperatorNameHandler::HandleOnsAndHardCodedName" );

    // Check ONS name and hard coded names in priority and add then
    // correct name to iDisplayTag.
    // Add also Operator name info data.
    if ( NET_GSM_HOME_PLMN == aNetworkType &&
        0 < iOperatorNameString.Length() &&
        ENetApacName != aHardCodedNameType )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - ONS Name added");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_HANDLEONSANDHARDCODEDNAME, "CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - ONS Name added" );
        // ONS (Operator Name String) name is read from SIM when
        // EMmTsyBootNotifySimStatusReadyIPC is completed and
        // when refresh happens.
        //
        // ONS name is shown only for home operator.
        // ONS name replaces the Latin name if exists.

        // Copy ONS name.
        aNetworkName.iDisplayTag.Copy( iOperatorNameString );
        // Add Operator name info data.
        iOperNameInfo.iType = RMmCustomAPI::EOperatorNameHardcodedLatin;
        iOperNameInfo.iName.Copy( iOperatorNameString );
        }
    else if ( ENetApacName == aHardCodedNameType )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded APAC Name added");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_HANDLEONSANDHARDCODEDNAME, "CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded APAC Name added" );
        // Copy Hard coded APAC name.
        aNetworkName.iDisplayTag.Copy( aDisplayTag );
        // Add Operator name info data.
        iOperNameInfo.iType = RMmCustomAPI::EOperatorNameHardcodedUcs2;
        iOperNameInfo.iName.Copy( aDisplayTag );
        }
    else if ( ENetLatinName == aHardCodedNameType )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded Latin Name added");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_HANDLEONSANDHARDCODEDNAME, "CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded Latin Name added" );
        // Copy Hard coded Latin name.
        aNetworkName.iDisplayTag.Copy( aDisplayTag );
        // Add Operator name info data.
        iOperNameInfo.iType = RMmCustomAPI::EOperatorNameHardcodedLatin;
        iOperNameInfo.iName.Copy( aDisplayTag );
        }
    else if ( ENetCountryMncName == aHardCodedNameType )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded CountryMnc Name added");
OstTrace0( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_HANDLEONSANDHARDCODEDNAME, "CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded CountryMnc Name added" );
        // Copy Hard coded CountryMnc name.
        aNetworkName.iDisplayTag.Copy( aDisplayTag );
        // Add Operator name info data.
        iOperNameInfo.iType = RMmCustomAPI::EOperatorNameCountryMcn;
        iOperNameInfo.iName.Copy( aDisplayTag );
        }
    else if ( ENetMccMncName == aHardCodedNameType )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded MccMnc Name added");
OstTrace0( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_HANDLEONSANDHARDCODEDNAME, "CMmNetOperatorNameHandler::HandleOnsAndHardCodedName - Hard coded MccMnc Name added" );
        // Copy Hard coded MccMnc name.
        aNetworkName.iDisplayTag.Copy( aDisplayTag );
        // Add Operator name info data.
        iOperNameInfo.iType = RMmCustomAPI::EOperatorNameMccMnc;
        iOperNameInfo.iName.Copy( aDisplayTag );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn
// Handles PNN record number with PP bit INFO_PP_EONS_FEAT value when
// registered in HPLMN.
// -----------------------------------------------------------------------------
//
TInt CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn
        (
        const TDesC8& aOperatorCodeData, // Operator Code data in BCD format
        const TUint& aLacData // Location Area Code data
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_HANDLEPNNRECORDWITHPPBITVALUEINHPLMN, "CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn" );

    // Temp PNN record identifier initialization.
    TUint8 tempPnnIdentifier( 0 );
    // PNN record identifier initialization.
    TUint8 pnnHplmnIdentifier( KNetNoHplmnPnnRecordNumber );

    // Check PP bit value.
    if ( INFO_PP_EONS_FEAT_0 == iInfoPpEonsFeatValue )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - PP bit INFO_PP_EONS_FEAT_0");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_HANDLEPNNRECORDWITHPPBITVALUEINHPLMN, "CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - PP bit INFO_PP_EONS_FEAT_0" );
        // Phone will always assume that there is a name
        // for HPLMN in PNN#1 (in PNN record number one),
        // without checking rules from EF-OPL first.

        // PNN#1 is reserved for HPLMN.
        pnnHplmnIdentifier = KNetHplmnPnnRecordNumber;
        }
    else
        {
        // OPL rules are read and list available.
        if ( iOplRulesRead && iOplListAvailable )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - OPL Rules handling");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_HANDLEPNNRECORDWITHPPBITVALUEINHPLMN, "CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - OPL Rules handling" );
            // EONS name consists of OPL rule and PNN record.
            // One OPL rule consist of PLMN code, LAC range
            // and PNN record number. OPL rules are read from
            // SIM when EMmTsyBootNotifySimStatusReadyIPC is
            // completed and when refresh happens.
            //
            // Note: BCD value of 'D' in any of the MCC and/or
            // MNC digits shall be used to indicate a "wild"
            // value for that corresponding MCC/MNC digit
            // (spec 31.102/4.2.59).

            // Checking is there matching record in OPL list.
            tempPnnIdentifier = OplRulesChecker(
                aOperatorCodeData,
                aLacData );
            }

        // Check PP bit value.
        if ( INFO_PP_EONS_FEAT_1 == iInfoPpEonsFeatValue )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - PP bit INFO_PP_EONS_FEAT_1");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_HANDLEPNNRECORDWITHPPBITVALUEINHPLMN, "CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - PP bit INFO_PP_EONS_FEAT_1" );
            // Phone will go through rules from OPL even when
            // in service in HPLMN, if rule for HPLMN is not
            // found from OPL, then it assumes that PNN#1
            // is reserved to HPLMN.

            // OPL rules are read and list available.
            if ( iOplRulesRead && iOplListAvailable )
                {
                // Matching OPL rules not found.
                if ( !iOplRuleMatch )
                    {
                    // PNN#1 is reserved for HPLMN.
                    pnnHplmnIdentifier = KNetHplmnPnnRecordNumber;
                    }
                else
                    {
                    // Matching OPL rule record is PNN record number.
                    pnnHplmnIdentifier = tempPnnIdentifier;
                    }
                }
            else
                {
                // OPL list does not exist.
                // No EONS name in this case.
                pnnHplmnIdentifier = KNetNoHplmnPnnRecordNumber;
                }
            }
        else if ( INFO_PP_EONS_FEAT_2 == iInfoPpEonsFeatValue )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - PP bit INFO_PP_EONS_FEAT_2");
OstTrace0( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_HANDLEPNNRECORDWITHPPBITVALUEINHPLMN, "CMmNetOperatorNameHandler::HandlePnnRecordWithPPbitValueInHplmn - PP bit INFO_PP_EONS_FEAT_2" );
            // Phone will never assume that there is a name
            // for HPLMN in PNN#1. It will show EONS name for
            // HPLMN only when it is found from EF-OPL, except
            // when EF-OPL does not exist at all.

            // OPL rules are read and list available.
            if ( iOplRulesRead && iOplListAvailable )
                {
                // Matching OPL rule found.
                if ( iOplRuleMatch )
                    {
                    // Matching OPL rule record is PNN record number.
                    pnnHplmnIdentifier = tempPnnIdentifier;
                    }
                else
                    {
                    // No EONS name in this case.
                    pnnHplmnIdentifier = KNetNoHplmnPnnRecordNumber;
                    }
                }
            else
                {
                // OPL list does not exist. PNN#1 is reserved for HPLMN.
                pnnHplmnIdentifier = KNetHplmnPnnRecordNumber;
                }
            }
        // No else.
        }

    return pnnHplmnIdentifier;
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::OplRulesChecker
// Go through OPL rule records and find out matching record if exist.
// -----------------------------------------------------------------------------
//
TInt CMmNetOperatorNameHandler::OplRulesChecker
        (
        const TDesC8& aOperatorCode, // Operator Code in BCD format
        TUint aLac // Location Area Code
        )
    {
TFLOGSTRING4("TSY: CMmNetOperatorNameHandler::OplRulesChecker - Operator Code BCD string: %02X %02X %02X", aOperatorCode[0], aOperatorCode[1], aOperatorCode[2]);
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::OplRulesChecker - LAC: %d", aLac);
OstTraceExt3( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_OPLRULESCHECKER, "CMmNetOperatorNameHandler::OplRulesChecker - Operator Code BCD string: %hhx %hhx %hhx", aOperatorCode[0], aOperatorCode[1], aOperatorCode[2] );
OstTrace1( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_OPLRULESCHECKER, "CMmNetOperatorNameHandler::OplRulesChecker - LAC: %u", aLac );

    // Initialize.
    TUint8 index( 0 );
    TUint8 tempPnnIdentifier( 0 );
    iOplRuleMatch = EFalse;

    // Number of records to be checked.
    TUint8 numberOfRecords( iOperatorPlmnListTable.Count() );

    // Go through records and find match if exist.
    while ( index < numberOfRecords && !iOplRuleMatch )
        {
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::OplRulesChecker - saved OPL list index: %d", index);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_OPLRULESCHECKER, "CMmNetOperatorNameHandler::OplRulesChecker - saved OPL list index=%hhu", index );
        // Check OPL rule record.
        OplRuleRecordChecker(
            index,
            aOperatorCode,
            aLac );

        // If match, take PNN Identifier from matching record.
        if ( iOplRuleMatch )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::OplRulesChecker - OPL rule match");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_OPLRULESCHECKER, "CMmNetOperatorNameHandler::OplRulesChecker - OPL rule match" );
            tempPnnIdentifier = iOperatorPlmnListTable[index].iPNNIdentifier;
            }
        else
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::OplRulesChecker - OPL rule doesn't match");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_OPLRULESCHECKER, "CMmNetOperatorNameHandler::OplRulesChecker - OPL rule doesn't match" );
            // No match.
            // Increase index value.
            index++;
            }
        }

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::OplRulesChecker - PNN Identifier: %d", tempPnnIdentifier);
OstTraceExt1( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_OPLRULESCHECKER, "CMmNetOperatorNameHandler::OplRulesChecker - PNN Identifier=%hhu", tempPnnIdentifier );

    return tempPnnIdentifier;
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::OplRuleRecordChecker
// Checks one OPL rule record against received Operator Code in BCD format
// and LAC value.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::OplRuleRecordChecker
        (
        TUint8 aIndex, // Record index
        const TDesC8& aOperCode, // Operator Code in BCD format
        TUint aLac // Location Area Code
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::OplRuleRecordChecker");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_OPLRULERECORDCHECKER, "CMmNetOperatorNameHandler::OplRuleRecordChecker" );

    // Temp data.
    TUint8 operCodeChar( 0 );
    TUint8 plmnListChar( 0 );

    // Setting ret value to KErrNone first to get compare working.
    TInt ret( KErrNone );

    // Compare one BCD code byte at time.
    for ( TUint8 i = 0; i < KBCDLength; i++ )
        {
        if ( KErrNone == ret )
            {
            // Check upper part of byte.
            operCodeChar = ( aOperCode[i] >> 4 ) & KBCDOneCharMask;
            plmnListChar =
                ( iOperatorPlmnListTable[aIndex].iOperCodeBCD[i] >> 4 ) &
                KBCDOneCharMask;

            ret = BCDCharChecker( operCodeChar, plmnListChar );

            // Continue checking lower part if previous matches.
            if ( KErrNone == ret )
                {
                // Check lower part of byte.
                operCodeChar = aOperCode[i] & KBCDOneCharMask;
                plmnListChar =
                    iOperatorPlmnListTable[aIndex].iOperCodeBCD[i] &
                    KBCDOneCharMask;

                ret = BCDCharChecker( operCodeChar, plmnListChar );
                }
            }
        }

    // Compare Location Area Value information.
    // LAC should be in between upper and lower limit.
    //
    // If BCD code match then continue to check LAC values.
    if ( KErrNone == ret )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::OplRuleRecordChecker - BCD code match");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_OPLRULERECORDCHECKER, "CMmNetOperatorNameHandler::OplRuleRecordChecker - BCD code match" );
        if ( ( aLac <= iOperatorPlmnListTable[aIndex].iLACUpperLimit )
            && ( aLac >= iOperatorPlmnListTable[aIndex].iLACLowerLimit ) )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::OplRuleRecordChecker - LAC match => OPL Rule match");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_OPLRULERECORDCHECKER, "CMmNetOperatorNameHandler::OplRuleRecordChecker - LAC match => OPL Rule match" );
            // Both BCD code and LAC value match.
            // Set iOplRuleMatch value to ETrue.
            // This informs that OPL record match.
            iOplRuleMatch = ETrue;
            }
        else
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::OplRuleRecordChecker - LAC doesn't match => OPL Rule doesn't match");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_OPLRULERECORDCHECKER, "CMmNetOperatorNameHandler::OplRuleRecordChecker - LAC doesn't match => OPL Rule doesn't match" );
            // LAC value range doesn't match.
            // Set iOplRuleMatch value to EFalse.
            // This informs that OPL record didn't match.
            iOplRuleMatch = EFalse;
            }
        }
    else
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::OplRuleRecordChecker - BCD code didn't match => OPL Rule doesn't match");
OstTrace0( TRACE_NORMAL, DUP4_CMMNETOPERATORNAMEHANDLER_OPLRULERECORDCHECKER, "CMmNetOperatorNameHandler::OplRuleRecordChecker - BCD code didn't match => OPL Rule doesn't match" );
        // Set iOplRuleMatch value to EFalse.
        // This informs that OPL record didn't match.
        iOplRuleMatch = EFalse;
        }
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::OplRuleRecordChecker - OPL Rule Match value T/F: %d", iOplRuleMatch);
OstTraceExt1( TRACE_NORMAL, DUP5_CMMNETOPERATORNAMEHANDLER_OPLRULERECORDCHECKER, "CMmNetOperatorNameHandler::OplRuleRecordChecker - OPL Rule Match value T/F=%hhu", iOplRuleMatch );
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::BCDCharChecker
// Check BCD char against rule to find out is that matching with OPL list.
// -----------------------------------------------------------------------------
//
TInt CMmNetOperatorNameHandler::BCDCharChecker
        (
        TUint8 aOperCodeChar,
        TUint8 aPlmnListChar
        )
    {
TFLOGSTRING3("TSY: CMmNetOperatorNameHandler::BCDCharChecker - Operator code char : Plmn list char = %X : %X", aOperCodeChar, aPlmnListChar);
OstTraceExt2( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_BCDCHARCHECKER, "CMmNetOperatorNameHandler::BCDCharChecker - Operator code char : Plmn list char = %hhx : %hhx", aOperCodeChar, aPlmnListChar );

    TInt ret( KErrGeneral );

    // Check character.
    // Operator code character needs to match OPL list Operator character
    // or if OPL list character value is 'D' it indicates "wild" value what
    // corresponding to all values.
    if ( aOperCodeChar == aPlmnListChar || KBCDWildChar == aPlmnListChar )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::BCDCharChecker - Character match");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_BCDCHARCHECKER, "CMmNetOperatorNameHandler::BCDCharChecker - Character match" );
        // Character match.
        ret = KErrNone;
        }
    // No else, ret is already set to KErrGeneral.

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::CopyEonsName
// Copy EONS Long and Short names if exist.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::CopyEonsName
        (
        RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkData // Network data
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CopyEonsName");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_COPYEONSNAME, "CMmNetOperatorNameHandler::CopyEonsName" );

    // Copy Long EONS name if exist.
    // If no Long EONS name there is no short EONS name either.
    if ( 0 < iEonsName.iLongName.Length() )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CopyEonsName - EONS Long name");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_COPYEONSNAME, "CMmNetOperatorNameHandler::CopyEonsName - EONS Long name" );

        // Copy EONS Long name.
        aNetworkData.iLongName.Copy( iEonsName.iLongName );

        // Add Operator name info data.
        iOperNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
        iOperNameInfo.iName.Copy( iEonsName.iLongName );

        // Copy Short EONS name if exist.
        if ( 0 < iEonsName.iShortName.Length() )
            {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CopyEonsName - EONS Short name");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_COPYEONSNAME, "CMmNetOperatorNameHandler::CopyEonsName - EONS Short name" );

            // Copy EONS Short name.
            aNetworkData.iShortName.Copy( iEonsName.iShortName );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::NitzNameChecker
// Check NITZ name data to current MCC/MNC values and update name if match.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::NitzNameChecker
        (
        TUint aMCC, // Current Mobile Country Code
        TUint aMNC, // Current Mobile Network Code
        RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkData // Network data
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::NitzNameChecker");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_NITZNAMECHECKER, "CMmNetOperatorNameHandler::NitzNameChecker" );

    // Check if NITZ long/short name exist and will match for
    // current Country and Network codes.
    if ( ( iNitzName.iMCC == aMCC )
        && ( iNitzName.iMNC == aMNC ) )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::NitzNameChecker - NITZ MCC/MNC codes match");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_NITZNAMECHECKER, "CMmNetOperatorNameHandler::NitzNameChecker - NITZ MCC/MNC codes match" );

        // Copy Long NITZ name if exist.
        if ( 0 < iNitzName.iLongName.Length() )
            {
            // Copy NITZ Long name.
            aNetworkData.iLongName.Copy( iNitzName.iLongName );

            // Add Operator name info data.
            iOperNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
            iOperNameInfo.iName.Copy( iNitzName.iLongName );
            }

        // Copy Short NITZ name if exist.
        if ( 0 < iNitzName.iShortName.Length() )
            {
            // Copy NITZ Short name.
            aNetworkData.iShortName.Copy( iNitzName.iShortName );

            // If NITZ Long name not exist.
            if ( 0 == iNitzName.iLongName.Length() )
                {
                // Add Operator name info data.
                iOperNameInfo.iType = RMmCustomAPI::EOperatorNameNitzShort;
                iOperNameInfo.iName.Copy( iNitzName.iShortName );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::CompareNitzNameToPmmValues
// Compare NITZ name got in NET_NITZ_NAME_IND to stored values in PMM.
// If data doesn't match then new NITZ name data will stored to PMM.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::CompareNitzNameToPmmValues()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CompareNitzNameToPmmValues");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_COMPARENITZNAMETOPMMVALUES, "CMmNetOperatorNameHandler::CompareNitzNameToPmmValues" );

    // Initialize to ETrue.
    TBool valuesMatch( ETrue );

    // Compare values.
    if ( ( iNitzNamePmm.iMCC != iNitzName.iMCC )
        || ( iNitzNamePmm.iMNC != iNitzName.iMNC )
        || ( 0 != iNitzNamePmm.iLongName.Compare( iNitzName.iLongName ) )
        || ( 0 != iNitzNamePmm.iShortName.Compare( iNitzName.iShortName ) ) )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CompareNitzNameToPmmValues - No match");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_COMPARENITZNAMETOPMMVALUES, "CMmNetOperatorNameHandler::CompareNitzNameToPmmValues - No match" );
        // No match.
        valuesMatch = EFalse;
        }

    // If values doesn't match, store new NITZ name to PMM.
    if ( !valuesMatch )
        {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CompareNitzNameToPmmValues - Update PMM with new NITZ name data");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_COMPARENITZNAMETOPMMVALUES, "CMmNetOperatorNameHandler::CompareNitzNameToPmmValues - Update PMM with new NITZ name data" );
        // Copy new values.
        iNitzNamePmm = iNitzName;

        // We need to save current NITZ name data to PMM,
        // then NITZ name data will be in save over power cycle.
        PermPmRecordWriteReq();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::Packed7to8Format
// Converts packed GSM default alphabet format to 8-bit format.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::Packed7to8Format
        (
        TUint8 aNumberOf7BitChars, // Number of 7-bit characters
        const TDesC8& aString, // 7-bit packed string
        TDes8& aOutput // 8-bit string
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::Packed7to8Format");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_PACKED7TO8FORMAT, "CMmNetOperatorNameHandler::Packed7to8Format" );

    // The string is in packed GSM 7-bit Default Alphabet format.
    // To be converted to 8-bit format.
    TUint8 maskRightPartOfCurrentByte( 0x7F );
    TUint8 shiftLeft( 0 );
    TUint8 leftPartFromPreviousByte( 0 );
    TInt stringLength( aString.Length() );

    aOutput.Zero();

    for ( TInt i = 0;
        i < stringLength && aNumberOf7BitChars > aOutput.Length();
        i++ )
        {
        TUint8 leftPartOfCurrentChar(
            ( aString[i] & maskRightPartOfCurrentByte )
            << shiftLeft );
TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::Packed7to8Format - Buffer index: %d", i);
OstTrace1( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_PACKED7TO8FORMAT, "CMmNetOperatorNameHandler::Packed7to8Format - Buffer index=%u", i );

        TUint8 character( leftPartOfCurrentChar | leftPartFromPreviousByte );

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::Packed7to8Format - Character: %X", character);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_PACKED7TO8FORMAT, "CMmNetOperatorNameHandler::Packed7to8Format - Character=%hhx", character );
        // Append the character in the output text string.
        aOutput.Append( character );

        // Check that possible spare bits doesn't append to output string.
        if ( aNumberOf7BitChars > aOutput.Length() )
            {
            // Updates.
            if ( KSiftedSixTimes == shiftLeft )
                {
                // After 6 shifts, the character is in bit7..bit1, therefore it has to
                // be shifted one bit to the right.
                character = aString[i] >> 1;

TFLOGSTRING2("TSY: CMmNetOperatorNameHandler::Packed7to8Format - Character: %X", character);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMNETOPERATORNAMEHANDLER_PACKED7TO8FORMAT, "CMmNetOperatorNameHandler::Packed7to8Format - Character=%hhx", character );
                // Continue.
                aOutput.Append( character );
                // Restart.
                leftPartFromPreviousByte = 0;
                shiftLeft = 0;
                maskRightPartOfCurrentByte = 0x7F;
                }
            else
                {
                leftPartFromPreviousByte = ( aString[i] ) >> ( 7 - shiftLeft );
                maskRightPartOfCurrentByte = maskRightPartOfCurrentByte >> 1;
                shiftLeft++;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16
// Convert a text from GSM alphabet to Unicode format.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16
        (
        TDes16& aOutput, // Unicode 16-bit format string
        const TDesC8& aInput // Input string
        )
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_CONVERTGSM7TOUNICODE16, "CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16" );

    TInt i( 0 );
    TInt outputMaxLength( aOutput.MaxLength() );
    TInt inputLength( aInput.Length() );

    for ( i = 0; i < inputLength && outputMaxLength > aOutput.Length(); i++ )
        {
        TUint8 character( aInput[i] );

        // This code is an escape to an extension of the 7 bit default alphabet
        // table.
        if ( KExtendedTable == character && ( i + 1 != inputLength ) )
            {
            // extension table
            switch ( aInput[i+1] )
                {
                case 0x28: // {
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7B ) );
                    break;
                    }
                case 0x29: // }
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7D ) );
                    break;
                    }
                case 0x3C: // [
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5B ) );
                    break;
                    }
                case 0x3E: // ]
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5D ) );
                    break;
                    }
                case 0x3D: // ~
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7E ) );
                    break;
                    }
                case 0x2F: /* \ */
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5C ) );
                    break;
                    }
                case 0x14: // ^
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5E ) );
                    break;
                    }
                case 0x65: // euro 0x20AC
                    {
                    aOutput.Append( static_cast<TUint16>( 0x20AC ) );
                    break;
                    }
                case 0x40: // |
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7C ) );
                    break;
                    }
                case 0x0A: // Form feed
                    {
                    aOutput.Append( static_cast<TUint16>( 0x0C ) );
                    break;
                    }
                case 0x1B: // Escape to yet another table.
                    {
                    // 3GPP TS 23.038 V6.1.0:
                    // "space until another extension table is defined"
                    aOutput.Append( static_cast<TUint16>( 0x20 ) );
                    break;
                    }
                default:
                    {
                    // 3GPP TS 23.038: "In the event that an MS receives
                    // a code where a symbol is not represented in the above
                    // table then the MS shall display the character shown in
                    // the main GSM 7 bit default alphabet table in clause
                    // 6.2.1"

                    // Check that bit 8 is set to '0'
                    if ( 0x7F >= aInput[i+1] )
                        {
                        // Character is in normal 7-bit table.
                        aOutput.Append( KUnicode[ aInput[i+1] ] );
                        }
                    else
                        {
                        // Unknown characters will be converted into a space.
                        aOutput.Append( static_cast<TUint16>( 0x20 ) );
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16, Unknown char is converted into a space.");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETOPERATORNAMEHANDLER_CONVERTGSM7TOUNICODE16, "CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16, Unknown char is converted into a space." );
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
            // Unknown characters will be converted into a space.
            aOutput.Append( static_cast<TUint16>( 0x20 ) );
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16, Unknown char is converted into a space.");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETOPERATORNAMEHANDLER_CONVERTGSM7TOUNICODE16, "CMmNetOperatorNameHandler::ConvertGsm7ToUnicode16, Unknown char is converted into a space." );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::CompleteMobilePhoneGetDetectedNetworksV2Phase1L
// Creates a CMobilePhoneNetworkListV2 with the information of the networks and
// completes EMobilePhoneGetDetectedNetworksV2Phase1 to SOS layer.
// -----------------------------------------------------------------------------
//
void CMmNetOperatorNameHandler::CompleteMobilePhoneGetDetectedNetworksV2Phase1L()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::CompleteMobilePhoneGetDetectedNetworksV2Phase1L");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_COMPLETEMOBILEPHONEGETDETECTEDNETWORKSV2PHASE1L, "CMmNetOperatorNameHandler::CompleteMobilePhoneGetDetectedNetworksV2Phase1L" );

    // Completing EMobilePhoneGetDetectedNetworksV2Phase1 IPC with found
    // operators in manual search need to use the CMobilePhoneNetworkListV2
    // class which is read only.
    //
    // Pointer to the List of Operators in manual search.
    CMobilePhoneNetworkListV2* list = CMobilePhoneNetworkListV2::NewL();
    // Use the cleanup stack for the list.
    CleanupStack::PushL( list );

    // Temporaly network data entry, because we need to use V2 version
    // when completing network list.
    RMobilePhone::TMobilePhoneNetworkInfoV2 networkEntry;

    // Amount of networks in Manual search network list.
    TUint8 amountOfNetworks( iManualSearchNetworkList.Count() );
    // Index value of network in manual search network list.
    TUint8 indexValue( 0 );

    // Add all found operators to list.
    while ( indexValue < amountOfNetworks )
        {
        // Copy Manual search data list entry first to V2 version.
        networkEntry = ( RMobilePhone::TMobilePhoneNetworkInfoV2 )
            iManualSearchNetworkList[indexValue];

        // Catch possible leave in AddEntryL.
        TRAP_IGNORE(
            // Add V2 version network entry to read only list.
            list->AddEntryL( networkEntry ) );

        // Update indexValue.
        indexValue++;
        }

    // Assistant for packaging.
    CMmDataPackage dataPackage;

    // Pack the data.
    dataPackage.PackData( &list );

    // Complete client request
    iNetMessHandler->GetMessageRouter()->Complete(
        EMobilePhoneGetDetectedNetworksV2Phase1,
        &dataPackage,
        KErrNone );

    // List has been already used, destroy it.
    CleanupStack::PopAndDestroy( list );

    // Reset manual network search list array.
    iManualSearchNetworkList.Reset();
    // Reset manual search data list array.
    iManualSearchDataList.Reset();
    }

// -----------------------------------------------------------------------------
// CMmNetOperatorNameHandler::GetPnnRecordIdentifierValue
// Return PNN record identifier value.
// -----------------------------------------------------------------------------
//
TInt CMmNetOperatorNameHandler::GetPnnRecordIdentifierValue()
    {
TFLOGSTRING("TSY: CMmNetOperatorNameHandler::GetPnnRecordIdentifierValue");
OstTrace0( TRACE_NORMAL, CMMNETOPERATORNAMEHANDLER_GETPNNRECORDIDENTIFIERVALUE, "CMmNetOperatorNameHandler::GetPnnRecordIdentifierValue" );
    return iEonsName.iPNNIdentifier;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
    //None

//  End of File
