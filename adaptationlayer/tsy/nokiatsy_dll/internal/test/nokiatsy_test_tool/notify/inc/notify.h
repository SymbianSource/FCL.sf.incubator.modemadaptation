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
* Description:   
*
*/




#ifndef NOTIFY_H
#define NOTIFY_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include "core.h"


// CONSTANTS

// MACROS

// Logging path
_LIT( KNotifyLogPath, "\\logs\\testframework\\Notify\\" ); 
// Log file
_LIT( KNotifyLogFile, "Notify.txt" ); 


// FORWARD DECLARATIONS
class CNotify;
class CCore;


// CLASS DECLARATION
class CNotify : public CScriptBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CNotify* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CNotify();


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since Series60_version 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt RunMethodL( CStifItemParser& aItem );
        
        /**
        * Handles async calls
        * @since Series60_version 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt UseObjectAddress( CStifItemParser& aItem );
                
    protected:  // New functions
        
        /**
        * Directs script call to correct functions 
        * @since Series60_version 2.8
        * @param aCount number of functions
        * @return Pointer to stif function info
        */
        const TStifFunctionInfo* RunMethodCases( TInt& aCount );

    private:

        /**
        * C++ default constructor.
        */
        CNotify( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        
        /**
        * Core functions, enumerates
        * @since Series60_version 2.8
        * @param aItem reference to descriptor holding data
        * @return enumral from core class
        */
        CCore::TNotifyList MapScriptToEnum( const TDesC& aItem );

    public:     // Data
        
       //Pointer to logger	
       	CStifLogger* iNotifyLog;

    private:    // Data
        
        
        /**
        * TBoolean hold information async call / cancellation
        * when true request is called
        * when false cancel request is called
        */
	 	TBool                   iRequest;
	 	
        //Pointer to core 
	    CCore*                  iCore; 
	    
        //TNotifyList variable from core class
		CCore::TNotifyList		iNotify;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // NOTIFY_H
            
// End of File