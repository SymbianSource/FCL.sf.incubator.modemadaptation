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
* Description:   Specifick class for NWE tests
*
*/




#ifndef NWE_H
#define NWE_H

//  INCLUDES
#include "core.h"

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <ctsy/rmmcustomapi.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#endif


// CONSTANTS


// MACROS
// Logging path
_LIT( KNWELogPath, "\\logs\\testframework\\NWE\\" );
// Log file
_LIT( KNWELogFile, "NWE.txt" );

_LIT(KIscControlTestCaseFileNWE, "nokiatsy_nwe.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;
class CNWE;


// CLASS DECLARATIONS

// Used for saving the enhanced cell information.
class TECIDInfo
    {
    public: // data
    RMmCustomAPI::TMmCellInfo iCellInfo; // Cell Information
    TUint iMCC; // Mobile Country Code
    TUint iMNC; // Mobile Network Code
    TUint iCID; // Cell identity
    TUint iLAC; // Location area code
    };

class CNWE : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNWE* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CNWE();

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
        IMPORT_C virtual void RouteCompletion( TInt aIpc,
            CMmDataPackage* aDataPackage, TInt aResult );

        /**
        * Completes a request or a notification with no data
        * @since Series60_ver 2.8
        * @param aNotifyList reference to enum indicating specified notify
        * @return None
        */
        virtual void NotifyNWE( TNotifyList& aNotifyList );


    protected:  // New functions

        /**
        * Sets NetworkSelectionSetting
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetNetworkSelectionSettingL( CStifItemParser& aItem );

        /**
        * Complete SetNetworkSelectionSetting
        * @since Series60_ver 2.8
        * @param aResult
        * @return None
        */
        void CompleteSetNetworkSelectionSetting( TInt aResult );

        /**
        * Get HomeNetwork
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt GetHomeNetworkL( CStifItemParser& aItem );

        /**
        * Complete Get HomeNetwork
        * @since Series60_ver 2.8
        * @param aResult
        *        aDataPackage
        * @return none.
        */
        void CompleteGetHomeNetwork( TInt aResult,
          CMmDataPackage* aDataPackage );

        /**
        * Get NetworkRegistrationStatus
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt GetNetworkRegistrationStatusL( CStifItemParser& aItem );

        /**
        * Complete GetNetworkRegistrationStatus
        * @since Series60_ver 2.8
        * @param aResult
        * @return none.
        */
        void CompleteGetNetworkRegistrationStatus( TInt aResult );

        /**
        * NotifyNWRegStatusChangeCompareParameterSetting
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt NotifyNWRegStatusChangeCompareParameterSettingL( CStifItemParser& aItem );

        /**
        * Notify NetworkRegistrationStatusChange
        * @since Series60_ver 2.8
        * @param aResult
        *        aDataPackage
        * @return none.
        */
        void NotifyNetworkRegistrationStatusChange( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * NotifyNetworkSelectionSettingChangeCompareParameterSetting
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt NotifyNetworkSelectionSettingChangeCompareParameterSettingL( CStifItemParser& aItem );

        /**
        * Notify NetworkSelectionSettingChange
        * @since Series60_ver 2.8
        * @param aResult
        *        aDataPackage
        * @return none.
        */
        void NotifyNetworkSelectionSettingChange( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * NotifyCurrentNetworkChangeCompareParameterSetting
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt NotifyCurrentNetworkChangeCompareParameterSettingL( CStifItemParser& aItem );

        /**
        * Notify CurrentNetworkChange
        * @since Series60_ver 2.8
        * @param aResult
        *        aDataPackage
        * @return none.
        */
        void NotifyCurrentNetworkChange( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * Notify ModeChange
        * @since Series60_ver 2.8
        * @param aResult
        *        aDataPackage
        * @return none.
        */
        void NotifyModeChange( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * Get CipheringIndicatorStatus
        * EMobilePhoneGetCipheringIndicatorStatus
        */
        TInt GetCipheringIndicatorStatusL( CStifItemParser& aItem );

        /**
        * Complete Get CipheringIndicatorStatus
        * EMobilePhoneGetCipheringIndicatorStatus
        */
        void CompleteGetCipheringIndicatorStatus( TInt aResult,
          CMmDataPackage* aDataPackage );

        /**
        * Custom Get CipheringInfo
        * ECustomGetCipheringInfoIPC
        */
        TInt CustomGetCipheringInfoL( CStifItemParser& aItem );

        /**
        * Complete Custom Get CipheringInfo
        * ECustomGetCipheringInfoIPC
        */
        void CompleteCustomGetCipheringInfo( TInt aResult,
          CMmDataPackage* aDataPackage );

        /**
        * Custom NetWakeup
        * ECustomNetWakeupIPC
        */
        TInt CustomNetWakeupL( CStifItemParser& aItem );

        /**
        * Complete Custom NetWakeup
        * ECustomNetWakeupIPC
        */
        void CompleteCustomNetWakeup( TInt aResult );

        /**
        * Custom Get OperatorName
        * ECustomGetOperatorNameIPC
        */
        TInt CustomGetOperatorNameL( CStifItemParser& aItem );

        /**
        * Complete Custom Get OperatorName
        * ECustomGetOperatorNameIPC
        */
        void CompleteCustomGetOperatorName( TInt aResult,
          CMmDataPackage* aDataPackage );

        /**
        * Custom ResetNetworkRegistrationStatus
        * ECustomResetNetServerIPC
        */
        TInt CustomResetNetworkRegistrationStatusL( CStifItemParser& aItem );

        /**
        * Complete Custom ResetNetworkRegistrationStatus
        * ECustomResetNetServerIPC
        */
        void CompleteCustomResetNetworkRegistrationStatus( TInt aResult );

        /**
        * Custom Get SystemNetworkModes
        * ECustomGetSystemNetworkModesIPC
        */
        TInt CustomGetSystemNetworkModesL( CStifItemParser& aItem );

        /**
        * Complete Custom Get SystemNetworkModes
        * ECustomGetSystemNetworkModesIPC
        */
        void CompleteCustomGetSystemNetworkModes( TInt aResult,
          CMmDataPackage* aDataPackage );

        /**
        * Custom Set SystemNetworkMode
        * ECustomSetSystemNetworkModeIPC
        */
        TInt CustomSetSystemNetworkModeL( CStifItemParser& aItem );

        /**
        * Complete Custom Set SystemNetworkMode
        * ECustomSetSystemNetworkModeIPC
        */
        void CompleteCustomSetSystemNetworkMode( TInt aResult );

        /**
        * Custom Get Current SystemNetworkModes
        * ECustomGetCurrentSystemNetworkModesIPC
        */
        TInt CustomGetCurrentSystemNetworkModesL( CStifItemParser& aItem );

        /**
        * Complete Custom Get Current SystemNetworkModes
        * ECustomGetCurrentSystemNetworkModesIPC
        */
        void CompleteCustomGetCurrentSystemNetworkModes( TInt aResult,
          CMmDataPackage* aDataPackage );

        /**
        * NotifySignalStrengthChange
        * EMobilePhoneNotifySignalStrengthChange
        */
        void NotifySignalStrengthChange( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * NotifyNITZInfoChange
        * EMobilePhoneNotifyNITZInfoChange
        */
        void NotifyNITZInfoChange( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * NotifyNetworkSecurityLevelChange
        * EMobilePhoneNotifyNetworkSecurityLevelChange
        */
        void NotifyNetworkSecurityLevelChange( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * Manual Network Search
        * EMobilePhoneGetDetectedNetworksV2Phase1
        */
        TInt ManualNetworkSearchL( CStifItemParser& aItem );

        /**
        * Complete Manual Network Search
        * EMobilePhoneGetDetectedNetworksV2Phase1
        */
        void CompleteManualNetworkSearch( TInt aResult,
          CMmDataPackage* aDataPackage );

        /**
        * Manual Network Search Cancel
        * EMobilePhoneGetDetectedNetworksCancel
        */
        TInt ManualNetworkSearchCancelL( CStifItemParser& aItem );

        /**
        * Complete Manual Network Search Cancel
        * EMobilePhoneGetDetectedNetworksCancel
        */
        void CompleteManualNetworkSearchCancel( TInt aResult );

        /**
        * Manual Network Selection
        * EMobilePhoneSelectNetwork
        */
        TInt ManualNetworkSelectionL( CStifItemParser& aItem );

        /**
        * Complete Manual Network Selection
        * EMobilePhoneSelectNetwork
        */
        void CompleteManualNetworkSelection( TInt aResult );

        /**
        * Manual Network Selection Cancel
        * EMobilePhoneSelectNetworkCancel
        */
        TInt ManualNetworkSelectionCancelL( CStifItemParser& aItem );

        /**
        * Complete Manual Network Selection Cancel
        * EMobilePhoneSelectNetworkCancel
        */
        void CompleteManualNetworkSelectionCancel( TInt aResult );

        /**
        * Notify Network Connection Failure
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt NotifyNetworkConnectionFailure( CStifItemParser& aItem );

        /**
        * Complete NotifyNetworkConnectionFailure
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteNotifyNetworkConnectionFailure( TInt aResult );

        /**
        * Get Network Info
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetNetworkInfo( CStifItemParser& aItem );

        /**
        * Set Network Info
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void SetNetworkInfo( TInt aResult, CMmDataPackage* aDataPackage,
                   TInt completedIpc );

        /**
        * Wait For Call Status Change
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt WaitForCallStatusChange( CStifItemParser& aItem );

        /**
        * Complete CallStatusChange
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteCallStatusChange( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Gets the cell info
        * @since Series60_ver 5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetCellInfo( CStifItemParser& aItem );

        /**
         * Complete GetCellInfo
         * @since Series60_ver 5.0
         * @param aResult
         * @param aDataPackage Data package containing cell info
         * @return None
         */
         void CompleteGetCellInfo( TInt aResult, CMmDataPackage* aDataPackage );

// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

        /**
        * Set Band Selection
        * ECustomSetBandSelectionIPC
        * @since Series60_ver 5.0
        */
        TInt CustomSetBandSelectionL( CStifItemParser& aItem );

        /**
        * Complete Set Band Selection
        * ECustomSetBandSelectionIPC
        * @since Series60_ver 5.0
        */
        void CompleteCustomSetBandSelection( TInt aResult );

        /**
        * Get Band Selection
        * ECustomGetBandSelectionIPC
        * @since Series60_ver 5.0
        */
        TInt CustomGetBandSelectionL( CStifItemParser& aItem );

        /**
        * Complete Get Band Selection
        * ECustomGetBandSelectionIPC
        * @since Series60_ver 5.0
        */
        void CompleteCustomGetBandSelection(
            TInt aResult,
            CMmDataPackage* aDataPackage );

#endif //NCP_COMMON_S60_VERSION_SUPPORT

        /**
        * NotifyPrivacyConfirmation
        * EMobileCallNotifyPrivacyConfirmation
        */
        void NotifyPrivacyConfirmation( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * NotifyCipheringInfoChangeIPC
        * ECustomNotifyCipheringInfoChangeIPC
        */
        void NotifyCipheringInfoChangeIPC( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * NotifyNSPSStatusIPC
        * ECustomNotifyNSPSStatusIPC
        */
        void NotifyNSPSStatusIPC( TInt aResult,
          CMmDataPackage* aDataPackage);

        /**
        * SetShortAndLongOperName
        * Short and Long operator comparison names can be set using this
        * method, because if operator name is set e.g. in CustomGetOperatorName
        * that method sets all comparison strings the same (tag, short name and
        * long name). With this method those can be overridden.
        */
        TInt SetShortAndLongOperNameL( CStifItemParser& aItem );

    protected:  // Functions from base classes


    private:

        /**
        * C++ default constructor.
        */
        CNWE( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_ver 2.8
        */
        void Delete();

        /**
        * Test methods are listed below.
        */

        /**
        * Prints current network info
        * @since Series60_ver 2.8
        * @param aNetworkInfo
        * @return Symbian OS error code.
        */
        void PrintCurrentNetworkInfo(
        RMobilePhone::TMobilePhoneNetworkInfoV5 aNetworkInfo );


    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * Pointer to logger
        */
        TBuf<100>           iSavedDisplayTagForComparison[4];
        TBuf<100>           iSavedShortNameForComparison[4];
        TBuf<100>           iSavedLongNameForComparison[4];
        TBuf<100>           iSavedOperNameTypeForComparison;
        TBuf<100>           iSavedOperNameForComparison;

        CStifLogger*        iNWELog;

        RMobilePhone::TMobilePhoneNetworkSelectionV1        iNWSelSetting;
        RMobilePhone::TMobilePhoneNetworkInfoV5             iNWInfo;

        RMobilePhone::TMobilePhoneRegistrationStatus        iRegistrationStatus;
        RMobilePhone::TMobilePhoneNetworkStatus             iNetworkStatus;

        RMobilePhone::TMobilePhoneNetworkSelectionV1        iNetworkSelection;

        RMobilePhone::TMobilePhoneNetworkInfoV5             iNetworkInfo;
        RMobilePhone::TMobilePhoneLocationAreaV1            iLocationArea;

        RMobilePhone::TMobilePhoneNetworkMode               iNetworkMode;

        TBuf8<CCore::ENumberOfNotifys>                      iNotifyList;

        RMobilePhone::TMobileCallCipheringIndicator*        iCipherIndicatorStatus;

        RMmCustomAPI::TOperatorNameInfo                     iOperatorName;

        RMmCustomAPI::TNetworkModeCaps                      iNetworkModeCaps;

        RMobilePhone::TMobilePhoneNITZ                      iNITZInfo;

        RMobilePhone::TMobilePhoneNetworkSecurity*          iNetworkSecurityLevel;
        RMobilePhone::TMobilePhoneNetworkInfoV5             iSavedNetworkInfo;

        // true if the case is ongoing (HandleRequest made and / or expecting Complete)

        TBool iSetNetworkSelectionSettingOngoing;
        TBool iGetHomeNetworkOngoing;
        TBool iGetNetworkRegistrationStatusOngoing;
        TBool iGetCipheringIndicatorStatusOngoing;
        TBool iCustomGetCipheringInfoOngoing;
        TBool iCustomNetWakeupOngoing;
        TBool iCustomGetOperatorNameOngoing;
        TBool iCustomResetNetworkRegistrationStatusOngoing;
        TBool iCustomGetSystemNetworkModesOngoing;
        TBool iCustomSetSystemNetworkModeOngoing;
        TBool iCustomGetCurrentSystemNetworkModesOngoing;
        TBool iManualNetworkSearchOngoing;
        TBool iManualNetworkSearchCancelOngoing;
        TBool iManualNetworkSelectionOngoing;
        TBool iManualNetworkSelectionCancelOngoing;
        TBool iNetworkFailureTestOnGoing;
        TBool iNetworkModeTestOnGoing;
        TBool iCompareNetworkInfoTestOngoing;
        TBool iHSXPATestOngoing;
        
        // Completion is handed normally even if result is not OK
        // See e.g. NotifyNetworkRegistrationStatusChange when NTSY has set
        // NOK result and that result should be passed to be checked in
        // nwe_simulte.cfg.
        TBool iAllowAlsoNokStatusFromTSY;

        //arrays and variables for saving given and received values
        TBool iLocationAreaKnown[2];
        RMobilePhone::TMobilePhoneNetworkMode iNetworkModeStatus[2];
        RMobilePhone::TMobilePhoneRegistrationStatus iNWRegStatusForComparison;
        RMobilePhone::TMobilePhoneNetworkStatus iNetworkStatusForComparison;
        RMobilePhone::TMobileCallCipheringIndicator iCipherIndicatorStatusForComparison;
        TBool iCipherIndicatorOnOff;
        TInt  iNetworkModeForComparison;
        RMobilePhone::TMobilePhoneNetworkAccess iNetworkAccessForComparison;
        TBool nspsIsOffOnForComparison;
        RMobilePhone::TMobilePhoneNetworkSelectionV1 iNetworkSelectionForComparison;
        RMobilePhone::TMobilePhoneLocationAreaV1 iLocationAreaForComparison;
        RMobilePhone::TMobilePhoneNetworkIdentity iNetworkIdForComparison[4];
        RMobilePhone::TMobilePhoneNetworkCountryCode iCountryCodeForComparison[4];

        // for parameter checking
        TECIDInfo iRefECIDInfo;
        TInt iNmrListIndex;
        TInt iDetailedCellInfoIndex;

// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

        RMmCustomAPI::TBandSelection iNetworkBand;

        TBool iCustomSetBandSelectionOngoing;
        TBool iCustomGetBandSelectionOngoing;

#endif //NCP_COMMON_S60_VERSION_SUPPORT

    };

#endif      // NWE_H

// End of File