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
* Description: 
*
*/



/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This file contains the header file of DemoModule of 
    STIF TestFramework.

-------------------------------------------------------------------------------
*/

#ifndef DEMOMODULE_H
#define DEMOMODULE_H

// INCLUDES
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include <stiftestmodule.h>
#include <stiflogger.h>                     // For CLogger
#include "cisimsgapitest.h"                  // For IsiMsg 2G Api Test
//#include "ctestbase.h"
//#include "cdspvoiceisimsgtest.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestModuleCommon;
class TCaseInfo;
class CTestLogger;

// CLASS DECLARATION

// A typedef for function that does the actual testing,
// function is a type TInt NameOfFunction ( TTestResult& aResult )
// rapotink this had changed since original param CTestModuleIf& added.
typedef TInt (CTestBase/*MTest*/::* TestFunction)( TExecutionAndDebugInfo& ); 

// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfoInternal
    {
    public:
        const TText* iCaseName;
        TestFunction iMethod;
    };

// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfo
    {
    public:
        TPtrC iCaseName;    
        TestFunction iMethod;

    TCaseInfo( const TText* a, TestFunction aMethod ) :
        iCaseName( /*(TText*)*/ a), iMethod( aMethod )
        {        
        };

    };

// DESCRIPTION
// Demo testmodule class definition.
class CTestModuleCommon 
        :public CTestModuleBase
    {

    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTestModuleCommon* NewL();
        
        /**
        * Destructor.
        */
        ~CTestModuleCommon();

    public: // New functions
        // None
        
    public: // Functions from base classes

        /**
        * Test cases are inquired from the Test Module by calling GetTestCases. 
        * Test cases are appended to RPointerArray<TTestCaseInfo>& aTestCases
        * that is a list consisting of several TTestCaseInfo objects.
        */
        TInt GetTestCasesL( const TFileName& aConfigFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );
        /**
        * RunTestCase is used to run an individual test case.
        */
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aConfig,
                           TTestResult& aResult );
        
    protected: // New functions

        /**
        * Initializes the test case array.
        * Automatically initializes the array as big as we need for 
        * necessary amount of test cases.
        * @param None
        * @return None
        */
        void InitializeTestCaseArrayL();
        
    protected: // Functions from base classes
        // None
        
    private:

        /**
        * C++ default constructor.
        */
        CTestModuleCommon();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

        /**
        * Function returning test case name and pointer to test case function
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;
            
    public: //Data
        // None
        
    protected: // Data
        // None
        
    private: // Data

        // Array for test class object pointers.
        CArrayFixFlat<CTestBase*>*          iArray;
        // Logger for writing log. From STIF.
        CStifLogger*                        iLogger;
        // Pointer to test (function) to be executed
        TestFunction                        iMethod;
        // Array for test class object pointers.
        CArrayFixFlat<TCaseInfoInternal>*   iTestCaseArray;

    public: // Friend classes
        // None
    
    protected: // Friend classes
        // None
    
    private: // Friend classes
        // None
    
    };


// Function pointer related internal definitions

// Hack around known GCC bug.
#ifndef __GCC32__
    #define GETPTR
#else
    #define GETPTR &
#endif  

#endif // DEMOMODULE_H

// End of File
