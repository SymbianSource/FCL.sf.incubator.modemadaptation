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
* Description:   Specific class for Security tests
*
*/




#ifndef SECURITY_H
#define SECURITY_H

//  INCLUDES
#include "core.h"
#include "ctsy/serviceapi/cmmutility.h"

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#else
#include <ctsy/rmmcustomapi.h>
#endif


// CONSTANTS
// none

// MACROS
// Logging path
_LIT( KSecurityLogPath, "\\logs\\testframework\\Security\\" );
// Log file
_LIT( KSecurityLogFile, "Security.txt" );

_LIT(KIscControlTestCaseFileSecurity, "nokiatsy_security.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;

// CLASS DECLARATION
class CSecurity : public CCore
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSecurity* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSecurity();

    public: // New functions
        // none

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

    private: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CSecurity( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_ver 2.8
        */
        void Delete();

        /**
        * Test methods are listed below.
        */

        /**
        * ECustomSecurityDeliverCodeIPC
        */
        TInt DeliverCodeL( CStifItemParser& aItem );

        /**
        * ECustomIsBlockedIPC
        */
        TInt IsBlockedL( CStifItemParser& aItem );

        /**
        * Complete ECustomIsBlockedIPC
        */
        void CompleteIsBlocked( TInt aErr, CMmDataPackage& aData );

        /**
        * ECustomCheckSecurityCodeIPC
        */
        TInt CheckSecurityCodeL( CStifItemParser& aItem );

        /**
        * Complete ECustomCheckSecurityCodeIPC
        */
        void CompleteCheckSecurityCode( TInt aErr, CMmDataPackage& aData );

        /**
        * ECustomDisablePhoneLockIPC
        */
        TInt DisablePhoneLockL( CStifItemParser& aItem );

        /**
        * Complete ECustomDisablePhoneLockIPC
        */
        void CompleteDisablePhoneLock( TInt aErr, CMmDataPackage& aData );

        /**
        * ECustomCheckSecurityCodeCancelIPC
        */
        TInt CheckSecurityCodeCancelL( CStifItemParser& aItem );

        /**
        * Complete ECustomCheckSecurityCodeCancelIPC
        */
        void CompleteCheckSecurityCodeCancel(
            TInt aErr, CMmDataPackage& aData );

        /**
        * EMmTsySimGetICCType
        */
        TInt SimGetICCTypeL(CStifItemParser& aItem);

        /**
        * Complete EMmTsySimGetICCType
        */
        void CompleteSimGetICCType( TInt aErr, CMmDataPackage& aData );

        /**
        * EMmTsyBootNotifyModemStatusReadyIPC
        */
        TInt BootNotifyModemStatusReady( CStifItemParser& aItem  );

        /**
        * Complete EMmTsyBootNotifyModemStatusReadyIPC
        */
        void CompleteBootNotifyModemStatusReady(
            TInt aErr, CMmDataPackage& aData );

        /**
        * EMmTsyBootNotifySimStatusReadyIPC
        */
        TInt BootNotifySimStatusReady( CStifItemParser& aItem );

        /**
        * Complete EMmTsyBootNotifySimStatusReadyIPC
        */
        void CompleteBootNotifySimStatusReady(
            TInt aErr, CMmDataPackage& aData );

        /**
        * EMmTsyBootNotifySecurityReadyIPC
        */
        TInt BootNotifySecurityReady( CStifItemParser& aItem );

        /**
        * Complete EMmTsyBootNotifySecurityReadyIPC
        */
        void CompleteBootNotifySecurityReady(
            TInt aErr, CMmDataPackage& aData );

        /**
        * EMmTsyBootGetRFStatusIPC
        */
        TInt BootGetRFStatusL( CStifItemParser& aItem );

        /**
        * Complete EMmTsyBootGetRFStatusIPC
        */
        void CompleteBootGetRFStatus( TInt aErr, CMmDataPackage& aData );

        /**
        * EMmTsyPhoneGetPin1DisableSupportedIPC
        */
        TInt PhoneGetPin1DisableSupportedL( CStifItemParser& aItem  );

        /**
        * Complete EMmTsyPhoneGetPin1DisableSupportedIPC
        */
        void CompletePhoneGetPin1DisableSupported(
            TInt aErr, CMmDataPackage& aData );

        /**
        * EMmTsySecurityGetSimActivePinStateIPC
        */
        TInt SecurityGetSimActivePinStateL( CStifItemParser& aItem );

        /**
        * Complete EMmTsySecurityGetSimActivePinStateIPC
        */
        void CompleteSecurityGetSimActivePinState(
            TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneGetLockInfo
        */
        TInt GetLockInfoL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneGetLockInfo
        */
        void CompleteGetLockInfo( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneNotifyLockInfoChange
        */
        TInt NotifyLockInfoChange( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneNotifyLockInfoChange
        */
        void CompleteNotifyLockInfoChange( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneSetLockSetting
        */
        TInt SetLockSettingL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneSetLockSetting
        */
        void CompleteSetLockSetting( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneChangeSecurityCode
        */
        TInt ChangeSecurityCodeL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneChangeSecurityCode
        */
        void CompleteChangeSecurityCode( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneNotifySecurityEvent
        */
        TInt NotifySecurityEvent( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneNotifySecurityEvent
        */
        void CompleteNotifySecurityEvent( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneVerifySecurityCode
        */
        TInt VerifySecurityCodeL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneVerifySecurityCode
        */
        void CompleteVerifySecurityCode( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneAbortSecurityCode
        */
        TInt AbortSecurityCodeL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneAbortSecurityCode
        */
        void CompleteAbortSecurityCode( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneGetFdnStatus
        */
        TInt SimGetFdnStatusL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneGetFdnStatus
        */
        void CompleteGetFdnStatus( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneSetFdnStatus
        */
        TInt SimSetFdnStatusL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneSetFdnStatus
        */
        void CompleteSetFdnStatus( TInt aErr, CMmDataPackage& aData );

        /**
        * EMobilePhoneGetSecurityCodeInfo
        */
        TInt SecurityInfoL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneGetSecurityCodeInfo
        */
        void SecurityInfoComplete( TInt aErr, CMmDataPackage& aData );

        /**
        * Complete EMobilePhoneNotifySecurityCodeInfoChange
        */
        void CompleteSecurityCodeInfoChange(
            TInt aErr, CMmDataPackage& aData );
            
        /**
        * GetCurrentActiveUSIMApplication
        */
        TInt GetCurrentActiveUSIMApplication( CStifItemParser& aItem );

        /**
        * CompleteGetCurrentActiveUSIMApplication
        */
        void CompleteGetCurrentActiveUSIMApplication( TInt aResult, CMmDataPackage* aDataPackage );

    private: // Data

        // logging object
        CStifLogger* iSecurityLog;

        // Variable for storing temporarily Security code type
        TUint8 iCodeID;

        // Variable for storing temporarily Security code
        RMobilePhone::TMobilePassword iSecCode;
        // Variable for storing AID
        RMobilePhone::TAID iActiveID;

        // Expected value returned from IPC
        TBool iIsBlockedExpected;

        // True if the case is ongoing
        // (HandleRequest made and / or expecting Complete)
        TBool iBootNotifySimStatusReadyOngoing;
        TBool iBootNotifyModemStatusReadyOngoing;
        TBool iBootNotifySecurityReadyOngoing;
        TBool iBootGetRFStatusOngoing;
        TBool iGetLockInfoOngoing;
        TBool iNotifyLockInfoChangeOngoing;
        TBool iSetLockSettingOngoing;
        TBool iChangeSecurityCodeOngoing;
        TBool iNotifySecurityEventOngoing;
        TBool iVerifySecurityCodeOngoing;
        TBool iAbortSecurityCodeOngoing;
        TBool iGetFdnStatusOngoing;
        TBool iSetFdnStatusOngoing;
        TBool iRemainingAttempsLeftOngoing;
        TBool iAIDCheckOngoing;
        TBool iLockReplaceTestOngoing;

        RMobilePhone::TMobilePhoneLockInfoV1 iLockInfo;
    };

#endif      // SECURITY_H

// End of File