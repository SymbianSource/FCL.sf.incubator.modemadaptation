/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef SIMATKCORE_H
#define SIMATKCORE_H

//  INCLUDES
#include <testscripterinternal.h>
#include <etelmm.h>
#include <satcs.h>

// CONSTANTS

// Logging path and file
_LIT( KCoreLogPath, "c:\\logs\\testframework\\" ); 
_LIT( KCoreLogFile, "SimAtkTestTool.txt" ); 

// Notifys list 

      
// DATA TYPES

// SatIpc is used to identify the Sat notifications
typedef TInt TSatIpc;

// Structure that contain the string and ipc presentation of notify
struct TSatNotify
    {
    const TText* iNotifyString;
    const TSatIpc iIpc;
    };
 
// CONSTANTS

// Array of notifications
TSatNotify const KNotifyList[] =
    {
        { _S("ESatNotifyCloseChannelPCmd"), ESatNotifyCloseChannelPCmd },
        { _S("ESatNotifyGetChannelStatusPCmd"), ESatNotifyGetChannelStatusPCmd },
        { _S("ESatNotifyGetInkeyPCmd"), ESatNotifyGetInkeyPCmd },
        { _S("ESatNotifyGetInputPCmd"), ESatNotifyGetInputPCmd },
        { _S("ESatNotifyGetReaderStatusPCmd"), ESatNotifyGetReaderStatusPCmd },
        { _S("ESatNotifyLanguageNotificationPCmd"), ESatNotifyLanguageNotificationPCmd },
        { _S("ESatNotifyPlayTonePCmd"), ESatNotifyPlayTonePCmd },
        { _S("ESatNotifyPollingIntervalPCmd"), ESatNotifyPollingIntervalPCmd },
        { _S("ESatNotifyPollingOffPCmd"), ESatNotifyPollingOffPCmd },
        { _S("ESatNotifyPowerOffCardPCmd"), ESatNotifyPowerOffCardPCmd },
        { _S("ESatNotifyPowerOnCardPCmd"), ESatNotifyPowerOnCardPCmd },
        { _S("ESatNotifyProactiveSimSessionEnd"), ESatNotifyProactiveSimSessionEnd },
        { _S("ESatNotifyProactiveSimSessionStart"), ESatNotifyProactiveSimSessionStart },
        { _S("ESatNotifyReceiveDataPCmd"), ESatNotifyReceiveDataPCmd },
        { _S("ESatNotifySelectItemPCmd"), ESatNotifySelectItemPCmd },
        { _S("ESatNotifyDisplayTextPCmd"), ESatNotifyDisplayTextPCmd },        	
        { _S("ESatNotifySetUpEventListPCmd"), ESatNotifySetUpEventListPCmd },
        { _S("ESatNotifySetUpMenuPCmd"), ESatNotifySetUpMenuPCmd },
        { _S("ESatNotifyTimerExpiration"), ESatNotifyTimerExpiration },
        { _S("ESatNotifyTimerMgmtPCmd"), ESatNotifyTimerMgmtPCmd },
        { _S("ESatNotifyTsyStateUpdated"), ESatNotifyTsyStateUpdated },
        { _S("ESatNotifyRefreshRequiredParam"), ESatNotifyRefreshRequiredParam },
        { _S("ESatNotifyRefreshRequired"), ESatNotifyRefreshRequired },
        { _S("ESatNotifyRefreshPCmd"), ESatNotifyRefreshPCmd },
        { _S("ESatNotifyLaunchBrowserPCmd"), ESatNotifyLaunchBrowserPCmd },
        { _S("ESatNotifyLocalInfoPCmd"), ESatNotifyLocalInfoPCmd },
        { _S("ESatNotifyOpenChannelPCmd"), ESatNotifyOpenChannelPCmd },
        { _S("ESatNotifyPerformCardApduPCmd"), ESatNotifyPerformCardApduPCmd },
        { _S("ESatNotifyRunAtCommandPCmd"), ESatNotifyRunAtCommandPCmd },
        { _S("ESatNotifySendDataPCmd"), ESatNotifySendDataPCmd },
        { _S("ESatNotifySendDtmfPCmd"), ESatNotifySendDtmfPCmd },
        { _S("ESatNotifySendSmPCmd"), ESatNotifySendSmPCmd },
        { _S("ESatNotifySendSsPCmd"), ESatNotifySendSsPCmd },
        { _S("ESatNotifySendUssdPCmd"), ESatNotifySendUssdPCmd },
        { _S("ESatNotifyServiceSearchPCmd"), ESatNotifyServiceSearchPCmd },
        { _S("ESatNotifySetUpCallPCmd"), ESatNotifySetUpCallPCmd },
        { _S("ESatNotifySetUpIdleModeTextPCmd"), ESatNotifySetUpIdleModeTextPCmd },
        { _S("ESatNotifyCallControlRequest"), ESatNotifyCallControlRequest },
        { _S("ESatNotifyCbDownload"), ESatNotifyCbDownload },
        { _S("ESatNotifyMoSmControlRequest"), ESatNotifyMoSmControlRequest },
        { _S("ESatNotifySmsPpDownload"), ESatNotifySmsPpDownload }
    };
    
const TUint KNotifyListSize = static_cast<TUint>
    ( sizeof( KNotifyList ) / sizeof( TSatNotify ) ) ;
    

// FORWARD DECLARATIONS
class CSimAtk;

// CLASS DECLARATION

/**
 *  CSimAtkCore test class for STIF Test Framework TestScripter.
 *  Base class for all sub classe
 *  @lib SimAtkCore.dll
 */
class CSimAtkCore : public CScriptBase
    {
    public:  // Enumerations
    
        /**
        * An enum object type. Enumarates required classed 
        * for Notify class.
        */
        enum TObjectType
        	{
        	ECore,      /**< TObjectType 0 ECore.   */  
            ESimAtk,    /**< TObjectType 1 ESimAtk. */
        	};
        	
    
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSimAtkCore* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSimAtkCore();
        
      
    public: // New methods

        /**
        * Core functions, enumerates notify type from scripted
        * parameter.
        * @param aNotifyString String that shall be mapped
        * @param aIsNotify Output which is set true if aNotifyString
        * contained notification and false when it was a cancellation
        * @return IPC or KErrNotSupported when string was a notify.
        * KErrNone if string was cancellation
        */  
        IMPORT_C virtual TSatIpc MapStringToIpc(
                const TDesC& aNotifyString,
                TBool& aIsNotify );
    
        /**
        * Core functions, enumerates notify string from Ipc.
        * @param aNotifyIpc Ipc that shall be mapped
        * @return Notify string or NullDesc when ipc remain unidentified
        */  
        IMPORT_C virtual TPtrC MapIpcToString(
                const TSatIpc aNotifyIpc );    
                
        /**
        * This sets asyncronous method call 
        * @param aStatus reference to recuest status
        * @param aNotifyIpc IPC of notify to be notified
        * @return Symbian OS Error code
        */
        IMPORT_C virtual TInt Notify( TRequestStatus& aStatus,
            const TSatIpc& aNotifyIpc );
       
        /**
        * This cancels async method call
        * @param aNotifyIpc IPC of notify to be cancelled
        * @return none
        */
        IMPORT_C virtual void NotifyCancel( const TSatIpc& aNotifyIpc );

        /**
        * Set parameters from subclass to base class if base class
        * implementation is used.
        * @param aString reference to parameter reed from the script
        * @return none
        */
        IMPORT_C void SetString( const TDesC& aString );


    public: // Functions from base classes

        /**
        * Function direct script calls to correct functions.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        IMPORT_C virtual TInt RunMethodL( CStifItemParser& aItem );
    	        

    protected: // New methods
    
        /**
        * Close functio to close required services after test is done 
        * @param none
        * @return Symbian OS Error code
        */
        IMPORT_C virtual TInt Close( CStifItemParser& aItem );
       
        /**
        * Open function for required services 
        * @param none
        * @return  Symbian Error code
        */
        IMPORT_C virtual TInt Open( CStifItemParser& aItem );
        
        /**
        * Sets object type by casting class
        * @param aType enum to current class
        * @param aModule pointer to any class this parameter is cast to 
        * refered class 
        * @return none
        */
		IMPORT_C virtual void SetObjectType( TObjectType aType,
		    TAny* aModule );
		    

    protected: // Constructors
    
        /**
        * Constructor  
        * @param aTestModuleIf reference to test module
        * @return base class object
        */
		IMPORT_C CSimAtkCore( CTestModuleIf& aTestModuleIf );
    
        /**
        * By default Symbian 2nd phase constructor is non public.
        */
        IMPORT_C void ConstructL();


    protected:  // Functions from base classes
    
    /**
        * Internal fuction to run specified method 
        * @param aFunction function available
        * @param aCount number of functions
        * @param aItem reference to parser class holding parameter data
        * @return Symbian Os Error Code
        */
        IMPORT_C virtual TInt RunInternalL( 
            TStifFunctionInfo const aFunctions[], TInt aCount, 
            CStifItemParser& aItem );
            
            
    private:
    
        /**
        * Connect function connects to needed servers
        * @param aItem reference to Parser class holding parameter data
        * @return Symbian OS Error code
        */
        TInt Connect( CStifItemParser& aItem );
       
        /**
        * Load function load required services and files
        * @param aItem reference to Parser class holding parameter data
        * @return Symbian OS Error code
        */ 
        TInt Load( CStifItemParser& aItem );
       
        /**
        * Return enum of the current class
        * @param none
        * @return enum to current class
        */
        TInt ObjectType();
        
        /**
        * Function direct script calls to correct functions.
        * @param ACount number of functions
        * @return Pointer to Stif function Info
        */     
        const TStifFunctionInfo* RunMethodCases( TInt& aCount );

        /**
        * Basic set function to set various data
        * @param aItem reference to Parser class holding parameter data
        * @return Symbian OS Error code
        */
        TInt Set( CStifItemParser& aItem );
         
        /**
        * UnLoad function unload required services and files
        * @param aItem reference to Parser class holding parameter data
        * @return Symbian OS Error code
        */ 
        TInt Unload( CStifItemParser& aItem );
       

    protected: // Data
        
        /**
        * RTelService object
        */ 
        RTelServer                      		iRtel;
        
        /**
        * RCommServer object
        */
        RCommServ                      			iRcomm;

        /**
        * RMobile phone object
        */		
    	RMobilePhone                    		iRphone;

        /**
        * Pointer to logger
        */	
       	CStifLogger*							iCoreLog;
                

    private: // Data

        /**
        * Pointer to string holding parameter from sub class
        * if base class implementation is needed
        */		
        TPtrC									iString;
        
        /**
        * Enumerale to object type
        */	
        TObjectType                             iObjectType;
    	
        /**
        * Pointer to CSimAtk class
        */	
        CSimAtk*                                iSimAtk;
        
        /**
        * Search optimization
        */
        TInt                                    iLastIndex;
    };

#endif      // SIMATKCORE_H
            
// End of File
