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
#include <e32math.h>
#include "demomodule.h"

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

    This module contains the implementation of CTestModuleCommon class 
    member functions that does the actual tests.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestModuleCommon

    Method: Case

    Description: Returns a test case by number.

    This function contains an array of all available test cases 
    i.e pair of case name and test function. If case specified by parameter
    aCaseNumber is found from array, then that item is returned.

    The reason for this rather complicated function is to specify all the
    test cases only in one place. It is not necessary to understand how
    function pointers to class member functions works when adding new test
    cases. See function body for instructions how to add new test case.
    
    Parameters:    const TInt aCaseNumber :in:      Test case number

    Return Values: const TCaseInfo Struct containing case name & function

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/ 
const TCaseInfo CTestModuleCommon::Case ( const TInt aCaseNumber ) const 
     {

    // NOTE!!!
    // To add new test cases, implement new test case class add construct it
    // in this classes ConstructL and append it to array. Add The number of test
    // cases to static TCaseInfoInternal KCases[ 50 ].

    TInt numOfCases( KErrNone );
    // Get amount of functions from all member testcase objects.
    for( TInt j( 0 ); j < iArray->Count(); j++ )
        {
        numOfCases += iArray->At( j )->GetFunctionPtrArray()->Count();
        }


    TInt currentCase( 0 );
    // Go through the array of all member testcase objects.
    for( TInt i( 0 ); i < iArray->Count(); i++ )
        {
        TInt amountOfFunctions( KErrNone );
        CArrayFixFlat<MTestFunction>* functionArray = NULL;
        // Get the function pointer array of object nn from arrays place i.
        // No ownership ain't transferred.
        functionArray = iArray->At( i )->GetFunctionPtrArray();
        amountOfFunctions = functionArray->Count();
        // Go through all the functions of the object.
        for( TInt k( 0 ); k < amountOfFunctions; k++ )
            {
            // Get casename for newCase from object nn at array place i for case number k.
            iTestCaseArray->At( currentCase ).iCaseName = iArray->At( i )->GetCaseNameL( k );
            // Get method for newCase from functionArray at place k at object nn in place
            // i at array.
            iTestCaseArray->At( currentCase ).iMethod = ( functionArray->At( k ) );
            // If the current case ain't the last case.
            if( currentCase < numOfCases )
                {
                currentCase++;
                }
            }
        }

    // Verify that case number is valid
    if( aCaseNumber < 0 || aCaseNumber >= numOfCases )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"", NULL );
        null.iMethod = NULL;
        return null;
        }
   
    if( NULL == iTestCaseArray->At( aCaseNumber ).iCaseName )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"", NULL );
        null.iMethod = NULL;
        return null;
        }

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( iTestCaseArray->At( aCaseNumber ).iCaseName,
                    iTestCaseArray->At( aCaseNumber ).iMethod );

    return tmp;
     
    }
    
// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
