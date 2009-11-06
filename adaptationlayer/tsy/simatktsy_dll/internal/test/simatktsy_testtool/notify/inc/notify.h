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
* Description:   Set and cancel async calls and write logs
*
*/



#ifndef SIMATKNOTIFY_H
#define SIMATKNOTIFY_H

//  INCLUDES
#include "core.h"

// FORWARD DECLARATIONS
class CSimAtkNotifyHandler;

// CLASS DECLARATION

/**
 * Notify test class for STIF Test Framework TestScripter.
 * Writes async calls log info to file 
 * @lib SimAtkNotify.dll
 */
NONSHARABLE_CLASS( CSimAtkNotify ) : public CScriptBase
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aTestMofuleIf reference to Testmodule
        * @return Pointer ro this class
        */       
		static CSimAtkNotify* NewL( CTestModuleIf& aTestModuleIf );
		
        /**
        * Destructor
        */
        virtual ~CSimAtkNotify();
        

    public: // New functions

        /**
        * Core return pointer to it own class 
        * @param none
        * @return Pointer to own class
        */        
        virtual CSimAtkCore* SimAtkCore();
        
        /**
        * Handles async calls and cancellations
        * @param aItem reference to CStifItemParser stores parameters
        * @return Symbian OS error code
        */        
       	TInt UseObjectAddress( CStifItemParser& aItem );


    public: // Functions from base classes

        /**
        * Runs a script line, must be implemented in derived class.
        * @param aTestMofuleIf reference to Testmodule
        * @return Symbian OS error code
        */
    	virtual TInt RunMethodL( CStifItemParser& aItem );
    	

    protected:  // Functions from base classes
    	
	    /**
        * Internal fuction to run specified method.
        * @param aFunctions info about stif functions
        * aCount number of functions
        * aItem reference to CStifItemParser stores parameters
        * read from scripter 
        * @return Pointer ro this class
        */        
    	virtual TInt RunInternalL( 
            TStifFunctionInfo const aFunctions[], 
            TInt aCount, 
            CStifItemParser& aItem );        	


    private:
    
        /**
        * C++ default constructor.
        */
        CSimAtkNotify();
        
        /**
        * Constructor.
        */       
        CSimAtkNotify( CTestModuleIf& aTestModuleIf );
            
        /**
        * Standart ConstructL 
        * @param none
        * @return none
        */                    
        void ConstructL();
        
        /**
        * Directs script call to correct functions 
        * @param aCount number of functions
        * @return Pointer to stif function info
        */
        const TStifFunctionInfo* RunMethodCases( TInt& aCount );
        

    private:    // Data

        /**
        * Pointer to notify handler
        */
		CSimAtkNotifyHandler*      		iNotifyHandler;
		
	    /**
        * Pointer to core
        */
	    CSimAtkCore*              		iCore;     
        
	};

#endif      // SIMATKNOTIFY_H
            
// End of File
