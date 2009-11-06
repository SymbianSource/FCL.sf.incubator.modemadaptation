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
* Description:   Core is base class for all polymorfic dlls use
*                to implement test cases
*
*/



#ifndef CORE_H
#define CORE_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <pcktcs.h>
#include <satcs.h>
#include <ctsy/mltsyfactory.h>
#include <etelmmcs.h>

#include "cmmmessagerouter.h"
#include "cisctestcase.h"

// Comment out the following definition when testing release test
// cases without hardware.
#define REAL_CELLMO_IN_USE

// MACROS
// Logging path
_LIT( KCoreLogPath, "\\logs\\testframework\\Core\\" );
// Log file
_LIT( KCoreLogFile, "Core.txt" );


const TDesC& MapIpcToName(TInt aIpc);

// FORWARD DECLARATIONS
class CCore;
class CNWE;
class CVoice;
class CMmMessageManagerCallback;

// CLASS DECLARATION
class TTestCaseObserverHelper : public MIscTestCaseObserver
    {
public:
    TTestCaseObserverHelper(CCore& aCore);
    void IscTestCaseCompleteL();
private:
    CCore& iCore;
    };

class CCore : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CCore* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCore();

        /**
        * An enum Notify list
        * List of available notifies
        */
        enum TNotifyList
            {
            // TNotifyList 0 ENotifyUndefined
            ENotifyUndefined=0,
            ENotifyNetworkRegistrationStatusChange,
            ENotifyNetworkSelectionSettingChange,
            ENotifyCurrentNetworkChange,
            ENotifyModeChange,
            ENotifySignalStrengthChange,
            ENotifyNITZInfoChange,
            ENotifyNetworkSecurityLevelChange,
            ENumberOfNotifys, // total number of notifies, keep this last
            };

        /**
        * An enum object type
        * Enumerates required classes for Notify class
        */
        enum TObjectType
            {
            ECore,      //TObjectType 0 ECore.
            EVoice,     //TObjectType 1 EVoice.
            ENWE,       //TObjectType 2 ENWE.
            EMisc,
            };

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
        virtual void RouteCompletion( TInt /*aIpc*/,
            CMmDataPackage* /*aDataPackage*/, TInt /*aResult*/ ) {}


        /**
        * Forwards request to the DOS layer, request contains
        * packaged data
        * @since Series60_ver 2.8
        * @param aIPC
                 aDataPackage
        * @return TInt
        */
        IMPORT_C virtual TInt HandleRequestL( const TInt aIpc,
                                     const CMmDataPackage* aDataPackage );

        IMPORT_C virtual TInt HandleRequestL( const TInt aIpc,
                                     const TDataPackage* aDataPackage );

        IMPORT_C virtual TInt HandleRequestL( const TInt aIpc );
        /**

        * Sets Notify
        * @since Series60_ver 2.8
        * @param aNotifyList reference to enum indicating specified notify
        * @return None
        */
        void Notify( TNotifyList& aNotifyList );

        /**
        * Ínitialize ISC Test Control
        * @param aIscTestCaseFile isc test control testcase file
        * @return None
        */
        IMPORT_C void InitializeIscControlL(const TDesC& aIscTestCaseFile);


        /**
        * Sync testcase with ISC Test Control
        * @param None
        * @return None
        */
        IMPORT_C void SyncIscControl();


        /**
        * Run ISC Test control testcase
        * @param
        * @return None
        */
        IMPORT_C void RunIscControlTestCase(TInt aTestControlId, TInt aTestCaseId);

        /**
        * RunIscTestCase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        IMPORT_C TInt RunIscTestCase( CStifItemParser& aItem );

        /**
        * SyncIscTestCase
        * @return Symbian OS error code.
        */
        IMPORT_C TInt SyncIscTestCase( void );

        /**
        * RunIscTestCaseStartNTSYAfterTestToolStart
        * This function MUST be used with SyncIscTestCaseStartNTSYAfterTestToolStart,
        * but not with other functions !!!
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        IMPORT_C TInt RunIscTestCaseStartNTSYAfterTestToolStart( CStifItemParser& aItem );

        /**
        * SyncIscTestCaseStartNTSYAfterTestToolStart
        * This function MUST be used with RunIscTestCaseStartNTSYAfterTestToolStart,
        * but not with other functions !!!
        * @return Symbian OS error code.
        */
        IMPORT_C TInt SyncIscTestCaseStartNTSYAfterTestToolStart( void );

        /**
        * RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        IMPORT_C TInt RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined( CStifItemParser& aItem );

        /**
        * SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined
        * @return Symbian OS error code.
        */
        IMPORT_C TInt SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined( void );

        /**
        * Test HandleSpecifiedRequests
        * @param aItem Script line containing method name and parameters
        * @return -
        */
        IMPORT_C TInt HandleSpecifiedRequests( CStifItemParser& aItem );

        /**
        * Test InitializeIPCRequestTable
        * @return -
        */
        IMPORT_C TInt InitializeIPCRequestTable( void );

        /**
        * Test SetExpectedCompleteIPC
        * @param aItem Script line containing method name and parameters
        * @return -
        */
        IMPORT_C TInt SetExpectedCompleteIPC( CStifItemParser& aItem );

        /**
        * Test GetNextExpectedCompleteIPC
        * @return -
        */
        IMPORT_C TInt GetNextExpectedCompleteIPC( void );

        /**
        * Test SetOperInfoAPAC
        * @return -
        */
        IMPORT_C TInt SetOperInfoAPAC( CStifItemParser& aItem );

        /**
        * Close/Delete NokiaTSY
        * @return Symbian OS error code
        */
        IMPORT_C TInt CloseNTSY( void );

        /**
        * CreateMMAndLTSYfactoryAndGetMessageRouter
        * @return -
        */
        IMPORT_C void CreateMMAndLTSYfactoryAndGetMessageRouter( void );

        /**
        * Called by CIscTestCase when ISC testcase is done
        * @param aError error value
        */
        void IscTestCaseCompleteL();

    protected:  // Functions from base classes

        /**
        * C++ default constructor.
        */
        CCore( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        IMPORT_C void BaseConstructL( CCore* aTestModule );


        /**
        * Sets object type by casting class
        * @since Series60_ver 2.8
        * @param aType enum to current class
        *        aModule pointer to any class this parameter is cast to
        *        refered class
        * @return none
        */
        IMPORT_C virtual void SetObjectType( TObjectType aType, TAny* aModule );

    /**
        * Return enum of the curren class
        * @since Series60_ver 2.8
        * @param none
        * @return enum to current class
        */
        TInt ObjectType();

    private:

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_ver 2.8
        */
        void Delete();

        /**
        * Test methods are listed below.
        */


    public:     // Data

        // Pointer to "Common TSY"
        CMmMessageManagerCallback* iMessageManagerCallBack;

        //Pointer to logger
        CStifLogger* iCoreLog;

        CIscTestCase* iIscTestCase;

        CCore* iTestModule;

        // IPC's name is saved here and when iHandleSpecifiedRequests is
        // ETrue the Complete is handled if it match with this IPC name.
        TBuf<200> iSavedIPCNameForComparisonTable[150];
        TBuf<200> iSavedIPCNameForComparison;

        // If expected IPC must be excactly the expected one...
        TBool iNextIPCMustBeSameAsNextExpectedTable[150];
        TBool iNextIPCMustBeSameAsNextExpected;

        // See explanation of iSavedIPCNameForComparison
        TBool iHandleSpecifiedRequests;

    protected:  // Data

        // Enumerale to object type
        TObjectType iType;

        //Pointer to Voice class
        CVoice* iVoice;

    private:    // Data

        //Enumerale to object type
        TObjectType                             iObjectType;
        // pointer to any class use to cast on
        TAny*                                   iObject;
        //Pointer to NWE class
        CNWE*                                   iNWE;

        MLtsyFactoryV1* iLtsyFactory;

        MMessageRouter* iMessageRouter;

        CMmDataPackage iDataPackage;

        TTestCaseObserverHelper iHelper;
    };

#endif      // CORE_H

// End of File
