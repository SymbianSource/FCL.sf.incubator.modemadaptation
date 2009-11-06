/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specifick class for WIM tests
*
*/




#ifndef WIM_H
#define WIM_H

//  INCLUDES
#include "core.h"
#include "ctsy/serviceapi/cmmgsmphonestorageutility.h"
#include "etelmm.h"

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
_LIT( KSSLogPath, "\\logs\\testframework\\WIM\\" ); 
// Log file
_LIT( KSSLogFile, "WIM.txt" ); 

_LIT(KIscControlTestCaseFileWIM, "nokiatsy_wim.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;
class CWIM;


// CLASS DECLARATION
class CWIM : public CCore
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWIM* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CWIM();
        

    public: // New functions

    public: // Functions from base classes
		
		
		 /**
        * RunIscTestCase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RunIscTestCase( CStifItemParser& aItem );

        /**
        * SyncIscTestCase
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SyncIscTestCase( CStifItemParser& aItem );
		
		
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
            CMmDataPackage* /*aDataPackage*/, TInt aResult );

      
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
        * Power SIM off
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PoweringSimOff( CStifItemParser& aItem );
        
        /**
        * Complete PoweringSimOff
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePoweringSimOff( TInt aResult );
        
        /**
        * SIM warm reset
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SIMWarmReset( CStifItemParser& aItem );
        
        /**
        * Complete SIMWarmReset
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSIMWarmReset( TInt aResult );
        
               
        /**
        * Get answer to reset status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetAnswerToResetStatus( CStifItemParser& aItem );
        
        /**
        * Complete GetAnswerToResetStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetAnswerToResetStatus( TInt aResult, CMmDataPackage* aDataPackage );
        
        /**
        * Get SIM card reader status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetSIMCardReaderStatus( CStifItemParser& aItem );
        
        /**
        * Complete GetSIMCardReaderStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetSIMCardReaderStatus( TInt aResult, CMmDataPackage* aDataPackage );
        
        /**
        * Send Application Data Unit (APDU)
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendAPDU( CStifItemParser& aItem );
        
        /**
        * Complete SendAPDU
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSendAPDU( TInt aResult, CMmDataPackage* aDataPackage );
        
        /**
        * Send Application Data Unit (APDU) request
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendAPDURequest( CStifItemParser& aItem );
        
        /**
        * Complete SendAPDURequest request
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSendAPDURequest( TInt aResult, CMmDataPackage* aDataPackage );
        
        
        
    protected:  // Functions from base classes
        

    private:
		
        /**
        * C++ default constructor.
        */
        CWIM( CTestModuleIf& aTestModuleIf );

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
        
    


    public:     // Data
    
    protected:  // Data

    private:    // Data
    
    TBuf8<255>                    iAnswerToResetStatus;
    TBuf8<255>                    iSIMCardReaderStatus;
    TBuf8<255>                    iInfo;
    TBuf8<255>                    iData;
    
    RMmCustomAPI::TApduParameters iParams;
        
    /**
    * Pointer to logger
    */
    CStifLogger* iWIMLog;
       
		
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
    

   
   
#endif      // WIM_H
            
// End of File
