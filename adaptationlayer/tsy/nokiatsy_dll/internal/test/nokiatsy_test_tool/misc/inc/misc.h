/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specifick class for miscellaneous tests
*
*/




#ifndef MISC_H
#define MISC_H

//  INCLUDES
#include "core.h"

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#else
#include <ctsy/rmmcustomapi.h>
#endif



// CONSTANTS

// MACROS
// Logging path
_LIT( KMiscLogPath, "\\logs\\testframework\\Misc\\" );
// Log file
_LIT( KMiscLogFile, "Misc.txt" );

_LIT(KIscControlTestCaseFile, "nokiatsy_misc.xml");


// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;

_LIT8(KEka1, "FirstText" );
_LIT8(KEka2, "SecondText" );
_LIT8(KEka3, "ThirdText" );
_LIT8(KEka4, "FourthText" );
_LIT8(KEka5, "FifthText" );



// CLASS DECLARATION
class CMisc : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMisc* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMisc();

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


    protected:  // Functions from base classes


    private:

        /**
        * C++ default constructor.
        */
        CMisc( CTestModuleIf& aTestModuleIf );

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
        * Start MobilePhoneNotifyBatteryInfoChange testcase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt MobilePhoneNotifyBatteryInfoChange( CStifItemParser& aItem );

        /**
        * Complete IPC for MobilePhoneNotifyBatteryInfoChange testcase
        * @param aDataPackage
        * @param aErrorCode
        * @return None
        */
        void CompleteMobilePhoneNotifyBatteryInfoChange (
            CMmDataPackage* aDataPackage, //
            TInt aErrorCode );

       /**
        * Start  testcase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SetACLStatus( CStifItemParser& );

        /**
        * Complete IPC for  testcase
        * @param aDataPackage
        * @param aErrorCode
        * @return None
        */
        void CompleteSetACLStatus( CMmDataPackage* aDataPackage, TInt aResult );

       /**
        * Start  testcase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt GetACLStatus( CStifItemParser& aItem );

        /**
        * Complete IPC for  testcase
        * @param aDataPackage
        * @param aErrorCode
        * @return None
        */
        void CompleteGetACLStatus(
            CMmDataPackage* aDataPackage, TInt aResult );

       /**
        * Start  testcase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt DeleteACLEntry( CStifItemParser& );

        /**
        * Complete IPC for  testcase
        * @param aDataPackage
        * @param aErrorCode
        * @return None
        */
        void CompleteDeleteACLEntry( TInt aResult );

       /**
        * Start  testcase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt WriteACLEntryL( CStifItemParser& aItem );

        /**
        * Complete IPC for  testcase
        * @param aDataPackage
        * @param aErrorCode
        * @return None
        */
        void CompleteWriteACLEntry( TInt aResult );

       /**
        * Start  testcase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt EnumerateAPNEntries( CStifItemParser& aItem );

        /**
        * Complete IPC for  testcase
        * @param aDataPackage
        * @param aErrorCode
        * @return None
        */
        void CompleteEnumerateAPNEntries( CMmDataPackage* aDataPackage, TInt aResult );

       /**
        * Start  testcase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt GetAPNname( CStifItemParser& );

        /**
        * Complete IPC for  testcase
        * @param aDataPackage
        * @param aErrorCode
        * @return None
        */
        void CompleteGetAPNname( CMmDataPackage* aDataPackage, TInt aResult );

        /**
        * EMobilePhoneVerifySecurityCode
        */
        TInt VerifySecurityCodeL( CStifItemParser& aItem );

        /**
        * Complete EMobilePhoneVerifySecurityCode
        */
        void CompleteVerifySecurityCode(TInt aErr, CMmDataPackage& aData );

        /**
        * Sets AlwaysOnMode
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetAlwaysOnModeSelectionL( CStifItemParser& aItem );

        /**
        * Complete SetNetworkSelectionSetting
        * @since Series60_ver 2.8
        * @param aResult
        * @return None
        */
        void CompleteSetAlwaysOnModeSelection( TInt aResult );

    private:    // Data

        RMmCustomAPI::TSetAlwaysOnMode iAlwaysOnMode;

        CStifLogger* iMiscLog;

        RMobilePhone::TMobilePhoneBatteryInfoV1 iBatteryInfo;
        
        RMobilePhone::TAPNControlListServiceStatus iACLStatus;

        TUint32 iAPNEntries;
        
        RMobilePhone::TAPNEntryV3 iAPNEntryName;
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // NWE_H

// End of File