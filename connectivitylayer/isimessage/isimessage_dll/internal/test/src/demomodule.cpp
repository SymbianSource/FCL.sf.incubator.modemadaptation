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



// INCLUDE FILES
#include "demomodule.h"
#include "tisimsgloggerconsts.h"        // For CLogger file path etc..
#include "ttestenums.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================

           
/*
-------------------------------------------------------------------------------

    DESCRIPTION
    
    This file (DemoModule.cpp) contains all test framework related parts of
    this test module. Actual test cases are implemented in file 
    DemoModuleCases.cpp.

    CTestModuleCommon is an example of test module implementation. This example
    uses hard coded test cases (i.e it does not have any test case
    configuration file).

    Example uses function pointers to call test cases. This provides an easy
    method to add new test cases.

    See function Cases in file DemoModuleCases.cpp for instructions how to
    add new test cases. It is not necessary to modify this file when adding
    new test cases.

    To take this module into use, add following lines to test framework
    initialisation file:

# Demo module
[New_Module]
ModuleName= DemoModule
[End_Module]


-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: CTestModuleCommon

    Description: Default constructor.
    
    Parameters:    None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestModuleCommon::CTestModuleCommon
        (
        // None
        )
    {
  
    // None

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: ConstructL

    Description: Symbian OS second phase constructor. Nothing to do.

    Parameters:    None

    Return Values: None

    Errors/Exceptions: None
    
    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestModuleCommon::ConstructL()
    { 

    // NOTE!!
    // Here you can construct a new test class and append it to array.
    // See CBoobTest for help how to create new test class.
    iArray = new( ELeave )CArrayFixFlat<CTestBase*>( 1 );
    // Add here test case classes.
    iArray->AppendL( CIsiMsgApiTest::NewL() );

    // Initialize the test case array.
    // NOTE!! The testclasses must be appended first
    InitializeTestCaseArrayL();

    // For writing a log file.
    TRAPD( err, iLogger = CStifLogger::NewL( KLogPath, KHtmlLogFile,
                                             CStifLogger::EHtml,
                                             CStifLogger::EFile, ETrue,
                                             ETrue, ETrue, 
                                             EFalse, EFalse, EFalse ) );
    if( KErrNone != err )
        {
        // Add the writing of notice to STIF/ATS that log malfunction.
        delete iLogger;
        iLogger = NULL;
        }    

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: NewL

    Description: Two-phased constructor. Constructs new CTestModuleCommon
    instance and returns pointer to it.
        
    Parameters:    None

    Return Values: CTestModuleCommon*: new object.

    Errors/Exceptions: Leaves if memory allocation fails or ConstructL leaves.

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestModuleCommon* CTestModuleCommon::NewL
        (
        // None
        )
    {

    // Construct new CTestModuleCommon instance.
    // Mark Heap memory.
    __UHEAP_MARK;
    CTestModuleCommon* selfTest = new ( ELeave ) CTestModuleCommon();    
    CleanupStack::PushL( selfTest );
    selfTest->ConstructL();
    CleanupStack::PopAndDestroy( selfTest );
    // See is the memory still the same as when marked.
    __UHEAP_MARKEND;

    CTestModuleCommon* self = new ( ELeave ) CTestModuleCommon();    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
  
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: ~CTestModuleCommon

    Description: Destructor.
    
    Parameters:    None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/    
CTestModuleCommon::~CTestModuleCommon
        (
        // None
        )
    {

    if( iArray )
        {
        // Delete all the member variable one by one.
        for( TInt i( 0 ); i < iArray->Count(); i++ )
            {
            delete iArray->At( i );
            }
        iArray->Reset();
        }
    delete iArray;
    iArray = NULL;
    delete iTestCaseArray;
    iTestCaseArray = NULL;

    if( iLogger )
        {
        delete iLogger;
        iLogger = NULL;
        }

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: GetTestCases

    Description: GetTestCases is used to inquire test cases 
    from the test module. Because this test module have hard coded test cases
    (i.e cases are not read from file), paramter aConfigFile is not used.

    This function loops through all cases defined in Cases() function and 
    adds corresponding items to aTestCases array.

    Parameters: const TFileName&  : in: Configuration file name. Not used                                                       
                RPointerArray<TTestCaseInfo>& aTestCases: out: 
                      Array of TestCases.
    
    Return Values: KErrNone: No error

    Errors/Exceptions: Function leaves if any memory allocation operation fails

    Status: Proposal
    
-------------------------------------------------------------------------------
*/      
TInt CTestModuleCommon::GetTestCasesL( const TFileName& /*aConfigFile*/, 
                                     RPointerArray<TTestCaseInfo>& aTestCases )
    {

    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {
 
        // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();
    
        // PushL TTestCaseInfo to CleanupStack. 
        CleanupStack::PushL( newCase );

        // Set number for the testcase.
        // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

        // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).iCaseName );

        // Append TTestCaseInfo to the testcase array. After appended 
        // successfully the TTestCaseInfo object is owned (and freed) 
        // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );
        
        // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );

        }
    
    return KErrNone;
  
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: RunTestCase

    Description: Run a specified testcase.

    Function runs a test case specified by test case number. Test case file
    parameter is not used.

    If case number is valid, this function runs a test case returned by
    function Cases(). 
  
    Parameters: const TInt aCaseNumber: in: Testcase number 
                const TFileName& : in: Configuration file name. Not used
                TTestResult& aResult: out: Testcase result
    
    Return Values: KErrNone: Testcase ran.
                   KErrNotFound: Unknown testcase

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestModuleCommon::RunTestCaseL
        (
        const TInt aCaseNumber, 
        const TFileName& /*aConfig*/,
        TTestResult& aResult
        )
    {

    
    // For Heap Memory check.
    __UHEAP_MARK;
    // Return value
    TInt execStatus( KErrNone );

    // Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );

    // Check that case number was valid
    if ( NULL != tmp.iMethod )
        {
        // Valid case was found, call it via function pointer
        iMethod = tmp.iMethod;
        execStatus = ETestNotImplemented; 
        
        for( TInt i( 0 );
             i < iArray->Count() && execStatus != KErrNone;
             i++
           )
            {
            TExecutionAndDebugInfo info( aResult, TestModuleIf(), *iLogger,
                                         tmp.iCaseName );
            execStatus  = ( iArray->At( i )->*iMethod )( info );
            }           
        if( ETestNotImplemented == execStatus )
            {
            // Valid case was not found, return error.
            execStatus = KErrNotFound;
            }
        }
    else
        {        
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }
    // For Heap Memory check.
    __UHEAP_MARKEND;
    // Return case execution status (not the result of the case execution)
    return execStatus;
      
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: InitializeTestCaseArrayL

    Description: Initialize the test case array.
  
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/    
void CTestModuleCommon::InitializeTestCaseArrayL
        (
        // None
        )
    {

    iTestCaseArray = new( ELeave )CArrayFixFlat<TCaseInfoInternal>( 1 );
    TInt numOfCases( KErrNone );
    // Get amount of functions from all member testcase objects.
    for( TInt j( 0 ); j < iArray->Count(); j++ )
        {
        numOfCases += iArray->At( j )->GetFunctionPtrArray()->Count();
        }
    for( TInt i( 0 ); i < numOfCases; i++ )
        {
        TCaseInfoInternal newCase;
        newCase.iCaseName = NULL;
        newCase.iMethod = NULL;
        iTestCaseArray->AppendL( newCase );
        }

    }

// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------
   
    Function: LibEntryL

    Description: Polymorphic Dll Entry Point
    
    Test framework calls this function to obtain new instance of test module
    class. 

    Parameters:    None
    
    Return Values: CTestModuleCommon*    Pointer to CTestModuleCommon instance
    
    Errors/Exceptions: Leaves if CTestModuleCommon::NewL leaves
    
    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleCommon* LibEntryL()
    {
    return CTestModuleCommon::NewL();

    }


// End of File
