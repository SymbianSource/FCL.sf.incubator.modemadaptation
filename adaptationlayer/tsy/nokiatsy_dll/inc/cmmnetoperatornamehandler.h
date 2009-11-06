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



#ifndef CMMNETOPERATORNAMEHANDLER_H
#define CMMNETOPERATORNAMEHANDLER_H

// INCLUDES
#include <e32base.h> // Symbian base types
#include <etelmm.h> // Symbian Etel related types
#include <ctsy/rmmcustomapi.h>

#include "cmmnetmesshandler.h"

// CONSTANTS
// Max length of Operator Name String (ONS name).
const TUint8 KOnsNameMaxLength = 0x19;
// Display tag max length.
const TUint8 KDisplayTagLength = 30;
// Operator code BCD string length.
const TUint8 KBCDLength = 3;

// MACROS
    //None

// DATA TYPES

// Network Operator name type.
enum TNetOperatorNameType
    {
    ENetNameNotSet = 0,
    ENetApacName,
    ENetLatinName,
    ENetCountryMncName,
    ENetMccMncName,
    ENetNitzName,
    ENetEonsName
    };

// Coding scheme type in NITZ and EONS data.
enum TDataCodingSchemeType
    {
    EGSMDefaultAlphabetCoding = 0,
    EUCS2Coding = 16
    };

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
class TIsiReceiveC;
class TIsiSubBlock;

// CLASS DECLARATION

/**
* Used for creating and sending Operator name handling specific
* ISI messages to PhoNet via PhoNetSender. It also receives
* Operator name handling specific ISI messages from PhoNetReceiver.
*/
class CMmNetOperatorNameHandler : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNetMessHandler: Pointer to the NetMessHandler.
        * @return created message handler object.
        */
        static CMmNetOperatorNameHandler* NewL(
            CMmNetMessHandler* aNetMessHandler );

        /**
        * Destructor.
        */
        ~CMmNetOperatorNameHandler();

    public: // Functions

        /**
        * Breaks a NET_NITZ_NAME_IND ISI-message.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message.
        * @return None.
        */
        void NetNitzNameInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Gets correct Operator name according priority order.
        * @param aCampedInHplmn is camped in HPLMN.
        * @param aNetworkType is Network type.
        * @param aOperatorCode is Operator Code in BCD format.
        * @param aLocationArea is current Location Area Code.
        * @param aNetworkName includes network data.
        */
        void GetOperatorName(
            TBool aCampedInHplmn,
            TUint8 aNetworkType,
            const TDesC8& aOperatorCode,
            const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea,
            RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkName );

        /**
        * Handles NET_MODEM_AVAILABLE_GET_RESP ISI-message success case.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message.
        * @return None.
        */
        void HandleManualSearchSuccessCaseL( const TIsiReceiveC& aIsiMessage );

        /**
        * Gets correct Operator names in manual search.
        */
        void GetManualSearchOperatorNameL();

        /**
        * Creates Product Profile request to read INFO_PP_EONS_FEAT bit.
        */
        void InfoPpReadReq();

        /**
        * Breaks a INFO_PP_READ_RESP ISI-message.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message.
        * @return None.
        */
        void InfoPpReadResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates a SIM_OPERATOR_REQ ISI message with service type
        * SIM_ST_READ_PNN and sends it to phonet.
        * Used to read the PLMN Network Name from EFpnn on (U)SIM.
        * @param aPnnIdentifier is identifier of the record to be read.
        */
        void SimOperatorReqReadPnn( TUint8 aPnnIdentifier );

        /**
        * Breaks a SIM_OPERATOR_RESP ISI message with service type
        * SIM_ST_READ_PNN.
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message.
        */
        void SimOperatorRespReadPnnL( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates a PERM_PM_RECORD_WRITE_REQ ISI message and sends
        * it to phonet.
        * Used to Write record for PMM.
        */
        void PermPmRecordWriteReq();

        /**
        * Breaks a PERM_PM_RECORD_WRITE_RESP ISI message.
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message.
        */
        void PermPmRecordWriteResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates a PERM_PM_RECORD_READ_REQ ISI message and sends
        * it to phonet.
        * Used to read record for PMM.
        */
        void PermPmRecordReadReq();

        /**
        * Breaks a PERM_PM_RECORD_READ_RESP ISI message.
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message.
        */
        void PermPmRecordReadResp( const TIsiReceiveC& aIsiMessage );

        /**
        * PNN record identifier value.
        * @return PNN record identifier value.
        */
        TInt GetPnnRecordIdentifierValue();

    private: // Constructors and destructor

        /**
        * 2nd phase constructor
        */
        void ConstructL();

    private: // Functions

        /**
        * Convert data field to operator name string according to coding scheme.
        * @param aNameType is name type to be converted.
        * @param aMCC is Country code.
        * @param aNameData is operator name data.
        * @param aOperatorName is converted operator name.
        * @return None.
        */
        void ConvertOperatorName(
            TNetOperatorNameType aNameType,
            TUint aMCC,
            TPtrC8& aNameData,
            TDes& aOperatorName );

        /**
        * Handles ONS and Hard coded names in priority.
        * @param aNetworkType is current network type.
        * @param aHardCodedNameType is hard coded name type.
        * @param aDisplayTag is hard coded operator name.
        * @param aNetworkName includes network data.
        * @return None.
        */
        void HandleOnsAndHardCodedName(
            TUint8 aNetworkType,
            TInt aHardCodedNameType,
            const TDes& aDisplayTag,
            RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkName );

        /**
        * Find out possible PNN record number with PP bit INFO_PP_EONS_FEAT
        * value when registered in HPLMN.
        * @param aOperatorCodeData is Operator Code data in BCD format.
        * @param aLacData is Location Area Code data.
        * @return possible PNN record number.
        */
        TInt HandlePnnRecordWithPPbitValueInHplmn(
            const TDesC8& aOperatorCodeData,
            const TUint& aLacData );

        /**
        * Go through OPL rule records and find out matching record if exist.
        * @param aOperatorCode is Operator Code in BCD format.
        * @param aLac is Location Area Code.
        * @return possible PNN record number.
        */
        TInt OplRulesChecker(
            const TDesC8& aOperatorCode,
            TUint aLac );

        /**
        * Checks one OPL rule record against received Operator Code
        * in BCD format and LAC value.
        * @param aIndex is OPL record index.
        * @param aOperCode is Operator Code in BCD format.
        * @param aLAC is Location Area Code.
        * @return None.
        */
        void OplRuleRecordChecker(
            TUint8 aIndex,
            const TDesC8& aOperCode,
            TUint aLac );

        /**
        * Check BCD char against rule to find out is that matching
        * with OPL list.
        * @param aOperCodeChar is character from one BCD byte.
        * @param aPlmnListChar is character from OPL list BCD byte.
        * @return success/failure value.
        */
        TInt BCDCharChecker(
             TUint8 aOperCodeChar,
             TUint8 aPlmnListChar );

        /**
        * Copy EONS Long and Short names if exist.
        * @param aNetworkData is Network data.
        * @return None.
        */
        void CopyEonsName(
            RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkData );

        /**
        * Check NITZ name data to current MCC/MNC values and update
        * name if match.
        * @param aMCC is current Mobile Country Code.
        * @param aMNC is current Mobile Network Code.
        * @param aNetworkData is Network data.
        * @return None.
        */
        void NitzNameChecker(
            TUint aMCC,
            TUint aMNC,
            RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkData );

        /**
        * Compare NITZ name to stored values in PMM.
        */
        void CompareNitzNameToPmmValues();

        /**
        * Converts 7-bit packed string to 8-bit format.
        * @param aNumberOf7BitChars is number of 7-bit characters.
        * @param aString is 7-bit packed source string.
        * @param aOutput is converted to 8-bit string.
        * @return None.
        */
        void Packed7to8Format(
            TUint8 aNumberOf7BitChars,
            const TDesC8& aString,
            TDes8& aOutput );

        /**
        * Browse a text in Unicode format, and for some specific characters
        * replace with the correct byte value.
        * @param aOutput unicode 16-bit format string.
        * @param aInput input 8-bit string.
        * @return None.
        */
        void ConvertGsm7ToUnicode16(
            TDes16& aOutput,
            const TDesC8& aInput );

        /**
        * Completes IPC EMobilePhoneGetDetectedNetworksV2Phase1.
        */
        void CompleteMobilePhoneGetDetectedNetworksV2Phase1L();

    protected: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CMmNetOperatorNameHandler();

    protected: // Functions

    private: // Enumerations, data types.

        // Used for storing EONS or NITZ name data.
        struct TOperatorNameData
            {
            // Mobile Country Code.
            TUint iMCC;
            // Mobile Network Code.
            TUint iMNC;
            // Long Name.
            RMobilePhone::TMobilePhoneNetworkLongName iLongName;
            // Short Name.
            RMobilePhone::TMobilePhoneNetworkShortName iShortName;
            // PNN identifier.
            TUint8 iPNNIdentifier;
            };

        // Operator PLMN List struct.
        struct TOperatorPLMNList
            {
            // Operator Code in BCD string format.
            TBuf8<KBCDLength> iOperCodeBCD;
            // Location Area Code, upper limit.
            TUint iLACUpperLimit;
            // Location Area Code, lower limit.
            TUint iLACLowerLimit;
            // Identifier of operator name to be displayed.
            TUint8 iPNNIdentifier;
            };

        // Manual search data list struct.
        struct TManualSearchDataList
            {
            // Mobile Country Code.
            TUint iMCC;
            // Mobile Network Code.
            TUint iMNC;
            // Network Type.
            TUint8 iNetworkType;
            // Location Area Code.
            TUint iLocationAreaCode;
            // Operator Code.
            TBuf8<KBCDLength> iOperatorCode;
            };

    public: // Data

    private: // Data

        // Pointer to the NetMessHandler.
        CMmNetMessHandler* iNetMessHandler;

        // Storing Location Area data for later use.
        RMobilePhone::TMobilePhoneLocationAreaV1 iLocationAreaData;
        // Storing Network data for later use.
        RMobilePhone::TMobilePhoneNetworkInfoV5 iNetworkData;

        // NET_NITZ_NAME_IND details need to be stored for later use.
        TOperatorNameData iNitzName;

        // EONS name details need to be stored for later use.
        TOperatorNameData iEonsName;

        // NITZ Name data stored to PMM.
        TOperatorNameData iNitzNamePmm;

        // Custom operator name info data.
        RMmCustomAPI::TOperatorNameInfo iOperNameInfo;

        // Value of the Product Profile bit INFO_PP_EONS_FEAT.
        TUint8 iInfoPpEonsFeatValue;

        // Buffer for Operator Name String (ONS name).
        TBuf<KOnsNameMaxLength> iOperatorNameString;

        // Operator PLMN list available.
        TBool iOplListAvailable;

        // OPL Rules read.
        TBool iOplRulesRead;

        // OPL Rule match.
        TBool iOplRuleMatch;

        // This is ETrue when SimOperatorReqReadPnn request
        // is sended from GetOperatorName function.
        TBool iFromGetOperatorName;

        // This is ETrue when SimOperatorReqReadPnn request
        // is sended from GetManualSearchOperatorName function.
        TBool iFromGetManualSearchOperatorName;

        // Index value of network in manual search network list.
        TUint8 iManualSearchIndexValue;

        // Operator PLMN List entry array.
        RArray<TOperatorPLMNList> iOperatorPlmnListTable;

        // Manual search Network List array.
        RArray<RMobilePhone::TMobilePhoneNetworkInfoV5> iManualSearchNetworkList;

        // Manual search data list array.
        RArray<TManualSearchDataList> iManualSearchDataList;

    protected: // Data

    };

#endif // CMMNETOPERATORNAMEHANDLER_H

// End of File
