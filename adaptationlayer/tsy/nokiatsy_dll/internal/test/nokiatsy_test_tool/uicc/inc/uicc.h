/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Specific class for UICC tests
*
*/



#ifndef UICC_H
#define UICC_H

//  INCLUDES
#include "core.h"
#include "etelmm.h"

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#else
#include <ctsy/rmmcustomapi.h>
#endif


// CONSTANTS

struct SRefPbStorageData
    {
    TBool iIsPhonebookInitialized;
    TBool iIsPhonebookInitializeFailed;
    TInt iXXXNumOfEntries;
    TInt iXXXTextLengthMax;
    TInt iXXXNumberLengthMax;
    };

enum TPhoneBookType
    {
    TPhoneBookTypeAdn = 0,
    TPhoneBookTypeFdn,
    TPhoneBookTypeSdn,
    TPhoneBookTypeMbdn,
    TPhoneBookTypeUnknown
    };

//TICCType enumerates the SIM card types
enum TICCType
    {
    EICCTypeSim2GGsm = 0,
    EICCTypeSim3G,
    EICCTypeSimUnknown
    };

// MACROS
// Logging path
_LIT( KSSLogPath, "\\logs\\testframework\\UICC\\" );
// Log file
_LIT( KSSLogFile, "UICC.txt" );

_LIT(KIscControlTestCaseFileUICC, "nokiatsy_uicc.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;
class CUICC;


// CLASS DECLARATION
class CUICC : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUICC* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CUICC();


    public: // New functions

    public: // Functions from base classes


        /**
        * From CScriptBase Runs a script line.
        * @since Series60_ver 2.8
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
            CMmDataPackage* /*aDataPackage*/,
            TInt aResult );


    protected:  // New functions

        /**
        * Power SIM on
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PoweringSimOn( CStifItemParser& aItem );

        /**
        * Complete PoweringSimOn
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePoweringSimOn( TInt aResult );

        /**
        * Test GetLifeTimer
        * @since Series60_ver 3.2
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetLifeTimer( CStifItemParser& aItem );

        /**
        * Complete Life Timer value request
        * @since Series60_ver 2.8
        * @param aResult
                 aDataPackage
        * @return none.
        */
        void CompleteGetLifeTimer(
             TInt aResult,
             CMmDataPackage* aDataPackage);

        /**
        * Get subscriber ID
        */
        TInt GetSubscriberId( CStifItemParser& aItem );

        /**
        * Complete subscriber ID
        */
        void CompleteGetSubscriberId(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Get ciphering indicator status
        */
        TInt GetCiStatus( CStifItemParser& aItem );

        /**
        * Get ciphering indicator status custom
        */
        TInt GetCiStatusCustom( CStifItemParser& aItem );

        /**
        * Complete ciphering indicator status
        */
        void CompleteGetCiStatus( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Set call forwarding status
        */
        TInt SetCallFwdStatus( CStifItemParser& aItem );

        /**
        * Complete call forwarding status
        */
        void CompleteSetCallFwdStatus(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Complete notify call forwarding status change
        */
        void CompleteNotifyIccCallForwardingStatusChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Get EHPLMN
        */
        TInt GetEhplmn( CStifItemParser& aItem );

        /**
        * Complete get EHPLMN
        */
        void CompleteGetEhplmn( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get service provider name
        */
        TInt GetServiceProviderName( CStifItemParser& aItem );

        /**
        * Complete get service provider name
        */
        void CompleteGetServiceProviderName(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Phonebook init
        */
        TInt PbInitL( CStifItemParser& aItem );

        /**
        * Get CSP
        */
        TInt GetCustomerServiceProfile( CStifItemParser& aItem );

        /**
        * Complete get CSP
        */
        void CompleteGetCustomerServiceProfile(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Read SIM file
        */
        TInt ReadSimFile( CStifItemParser& aItem );

        /**
        * Complete read SIM file
        */
        void CompleteReadSimFile( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Complete PB Init
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePbInit( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Checks phonebook data
        * @since Series60_ver 5.0
        * @param pbData
        * @param pbType
        * @return aResult
        */
        TInt CheckPbData( CStorageInfoData* aPbData, TPhoneBookType aPbType );

        /**
        * Prints Pb Init Info
        * @since Series60_ver 3.0
        * @param aPbInitInfo
        * @return None
        */
        void PrintPbInitInfo(CStorageInfoData* aPbInitInfo);

        /**
        * Get mailbox identifier
        */
        TInt GetMailboxIdentifier( CStifItemParser& aItem );

        /**
        * Complete get mailbox identifier
        */
        void CompleteGetMailboxIdentifier(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Read CBS topics (CBMI)
        */
        TInt ReadCBSSimTopics( CStifItemParser& aItem );

        /**
        * Complete read CBS topics (CBMI)
        */
        void CompleteReadCBSSimTopics( TInt aErr, CMmDataPackage* aData);

        /**
        * Delete CBS topic
        */
        TInt DeleteCBSSimTopic( CStifItemParser& aItem );

        /**
        * Complete delete CBS topic
        */
        void CompleteDeleteCBSSimTopic( TInt aErr );

        /**
        * Get network provider name
        */
        TInt GetNetworkProviderName( CStifItemParser& aItem );

        /**
        * Complete get network provider name
        */
        void CompleteGetNetworkProviderName(
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Read viag home zone parameters
        */
        TInt ViagHomeZoneReadParamL( CStifItemParser& aItem );

        /**
        * Complete read viag home zone parameters
        */
        void CompleteViagHomeZoneReadParam(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Read viag home zone cache
        */
        TInt ViagHomeZoneReadCacheL( CStifItemParser& aItem );

        /**
        * Write viag home zone settings
        */
        TInt ViagHomeZoneWriteSettingsL( CStifItemParser& aItem );

        /**
        * Write viag home zone cache
        */
        TInt ViagHomeZoneWriteCacheL( CStifItemParser& aItem );

        /**
        * Complete write viag home zone cache
        */
        void CompleteViagHomeZoneReadCache(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Read voice message flags REL 4
        */
        TInt ReadVoiceMsgFlagsRel4( CStifItemParser& aItem );

        /**
        * Complete read voice message flags REL 4
        */
        void CompleteReadVoiceMsgFlags(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Write voice message flags REL 4
        */
        TInt WriteVoiceMsgFlagsRel4( CStifItemParser& aItem );

        /**
        * Read voice message flags CPHS
        */
        TInt ReadVoiceMsgFlagsCphs( CStifItemParser& aItem );

        /**
        * Write voice message flags CPHS
        */
        TInt WriteVoiceMsgFlagsCphs( CStifItemParser& aItem );

        /**
        * Complete notify message waiting indicator status change
        */
        void CompleteNotifyMwiChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Read dynamic flags
        */
        TInt ReadDynamicFlags( CStifItemParser& aItem );

        /**
        * Complete read dynamic flags
        */
        void CompleteReadDynamicFlags(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Read dynamic flags
        */
        TInt WriteDynamicFlags( CStifItemParser& aItem );

        /**
        * Read dynamic 2 flags
        */
        TInt ReadDynamic2Flags( CStifItemParser& aItem );

        /**
        * Complete read dynamic 2 flags
        */
        void CompleteReadDynamic2Flags(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Read dynamic 2 flags
        */
        TInt WriteDynamic2Flags( CStifItemParser& aItem );

        /**
        * Get UICC status
        */
        TInt GetUiccStatus( CStifItemParser& aItem );

        /**
        * Disconnect UICC server from smarcard and activate SAP APDU interface
        */
        TInt PowerOffSim();

        /**
        * Complete PowerOffSim
        */
        void CompletePowerOffSim( TInt aErr);

        /**
        * SIM warm reset
        */
        TInt SimWarmReset();

        /**
        * Complete SimWarmReset
        */
        void CompleteSimWarmReset( TInt aErr);

        /**
        * SIM get ATR
        */
        TInt SimGetATR(CStifItemParser& aItem);

        /**
        * Complete get ATR
        */
        void CompleteSimGetATR(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Connect UICC server to smarcard and deactivates SAP APDU interface
        */
        TInt PowerOnSim();

        /**
        * Complete PowerOnSim
        */
        void CompletePowerOnSim( TInt aErr);

        /**
        * Get RF status
        */
        TInt GetRfStatus( CStifItemParser& aItem );

        /**
        * Complete RF status
        */
        void CompleteGetRfStatus(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Send APDUv2
        */
        TInt SendAPDUv2();

        /**
        * Complete send APDU
        */
        void CompleteSendAPDUv2(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Get Card Reader Status
        */
        TInt GetCardReaderStatus();

        /**
        * Complete send GetCardReaderStatus
        */
        void CompleteGetCardReaderStatus(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Card status indication
        */
        TInt UiccCardInd();

        /**
        * Complete card status indication
        */
        void CompleteUiccCardInd(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Send APDU
        */
        TInt SendAPDU();

        /**
        * Complete send APDU
        */
        void CompleteSendAPDU(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Notify security event
        */
        TInt NotifySecurityEvent( CStifItemParser& aItem );

        /**
        * Complete notify security event
        */
        void CompleteNotifySecurityEvent(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Verify security code
        */
        TInt VerifySecurityCode( CStifItemParser& aItem );

        /**
        * Get PIN info
        */
        TInt GetPinInfo( CStifItemParser& aItem );

        /**
        * Complete get PIN info
        */
        void CompleteGetPinInfo(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Change PIN code
        */
        TInt ChangePinCode( CStifItemParser& aItem );


        /**
        * Change PIN code state
        */
        TInt SetLockSetting( CStifItemParser& aItem );

        /**
        * Get PIN attempts left
        */
        TInt GetPinAttemptsLeft( CStifItemParser& aItem );

        /**
        * Complete get PIN attempts left
        */
        void CompleteGetPinAttemptsLeft(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Get ICC type
        */
        TInt GetIccType( CStifItemParser& aItem );

        /**
        * Complete get ICC type
        */
        void CompleteGetIccType(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * GetCurrentActiveUSIMApplication
        */
        TInt GetCurrentActiveUSIMApplication( CStifItemParser& aItem );

        /**
        * CompleteGetCurrentActiveUSIMApplication
        */
        void CompleteGetCurrentActiveUSIMApplication( TInt aResult, CMmDataPackage* aDataPackage );


        /**
        * EMmTsySecurityGetSimActivePinStateIPC
        */
        TInt GetActivePin( CStifItemParser& aItem );

        /**
        * Complete EMmTsySecurityGetSimActivePinStateIPC
        */
        void CompleteGetActivePin(
            TInt aErr, CMmDataPackage& aData );



    protected:  // Functions from base classes


    private:

        /**
        * C++ default constructor.
        */
        CUICC( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_ver 2.8
        */
        void Delete();


    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * Pointer to logger
        */
        CStifLogger* iUICCLog;

        RMmCustomAPI::TLifeTimeData iLifeTimerForComparison;

        TBuf8<RMobilePhone::KIMSISize> iImsi;

        RMobilePhone::TMobileCallCipheringIndicator iCipherIndicatorStatus;

        TUint iIndicator;

        RMobilePhone::TMobilePhoneCFCondition iCFCondition;

        RMobilePhone::TMobilePhoneServiceProviderNameV2 iServiceProviderName;

        TName iPhoneBookName;
        TPhoneBookType iPbType[2];
        SRefPbStorageData iPbStorageDataAdn;
        SRefPbStorageData iPbStorageDataFdn;
        SRefPbStorageData iPbStorageDataSdn;
        SRefPbStorageData iPbStorageDataMbdn;

        RMobilePhone::TMobilePhoneCspFileV1 iCspFile;

        TBool iTestOngoing;

        CStorageInfoData *iRefPBStoreInfoData;

        RMobilePhone::TMobilePhoneVoicemailIdsV3 iVoicemailIds;

        RMmCustomAPI::TSimCbTopic iSimCbTopic1;
        RMmCustomAPI::TSimCbTopic iSimCbTopic2;
        RMmCustomAPI::TSimCbTopic iSimCbTopic3;

        TBuf<20> iName;

        RMmCustomAPI::TViagParams iSavedViagParamsForComparison;
        RMmCustomAPI::TViagElements* iSavedViagElementsForComparison;
        RMmCustomAPI::TViagCacheRecordContent iSavedRecordForComparison;

        RMobilePhone::TMobilePhoneMessageWaitingV1 iMsgWaiting;

        RMobilePhone::TMobilePhoneALSLine iAlsLine;

        RMmCustomAPI::TGetAlsBlockStatus iBlockStatus;
        TBuf8<40> iAPDU;
        TBuf8<100> iCardReaderStatusByte;

        TRfStateInfo iStatusInfo;

        RMmCustomAPI::TSIMCardStatus iCardStatus;

        TBool iUiccCardIndTest;

        RMobilePhone::TMobilePhoneSecurityEvent iEvent;

        RMobilePhone::TMobilePhoneLockInfoV1 iLockInfo;

        TInt iPinAttemptsLeft;

        TICCType iIccType;

        RMobilePhone::TAID iActiveID;

        RMobilePhone::TMobilePhoneSecurityCode iActivePin;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // UICC_H

// End of File
