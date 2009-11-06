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
* Description:   Specifick class for MiscPhone tests
*
*/




#ifndef MiscPhone_H
#define MiscPhone_H

//  INCLUDES
#include "core.h"
#include "ctsy/serviceapi/cmmgsmphonestorageutility.h"
#include "etelmm.h"
#include "pcktcs.h" //ETel Packet
#include "etelpckt.h" //ETel Packet

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
_LIT( KSSLogPath, "\\logs\\testframework\\MiscPhone\\" ); 
// Log file
_LIT( KSSLogFile, "MiscPhone.txt" ); 

_LIT(KIscControlTestCaseFileMiscPhone, "nokiatsy_miscphone.xml");


// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;
class CMiscPhone;



// CLASS DECLARATION
class CMiscPhone : public CCore
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMiscPhone* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CMiscPhone();
        
 
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
            CMmDataPackage* aDataPackage , TInt aResult );

      
    protected:  // New functions
        
    	/**
        * Get Customer Service Profile
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetCustomerServiceProfile( CStifItemParser& aItem );
        
        /**
        * Complete GetCustomerServiceProfile
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetCustomerServiceProfile( TInt aResult, CMmDataPackage* aDataPackage );
       
        /**
        * Get Phone Id
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetPhoneId( CStifItemParser& aItem );
        
        /**
        * Complete GetPhoneId
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetPhoneId( TInt aResult, CMmDataPackage* aDataPackage );
         
        /**
        * Set drive mode
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetDriveMode( CStifItemParser& aItem );
        
        /**
        * Complete SetDriveMode
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetDriveMode( TInt aResult );       
        
      
      
        
    protected:  // Functions from base classes
        

    private:
		
        /**
        * C++ default constructor.
        */
        CMiscPhone( CTestModuleIf& aTestModuleIf );

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
    	
    	
    	TBool iGetNotifyNetworkConnectionFailure;
    	
    /**
    * Pointer to logger
    */
    CStifLogger* iMiscPhoneLog;
       
		
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
    

   
   
#endif      // MiscPhone_H
            
// End of File