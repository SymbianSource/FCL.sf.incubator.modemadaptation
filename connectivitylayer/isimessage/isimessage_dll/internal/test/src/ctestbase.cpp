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

    This file contains the header file of CTestBase of 
    CTestModule

-------------------------------------------------------------------------------
*/
// INCLUDES
#include <f32file.h>            // For RDebug 

#include "ctestbase.h"
     
/*
-------------------------------------------------------------------------------

    Class: CTestBase

    Method: GetCaseNameL

    Description: Returns the name of the case.

    Parameters: TInt aCase

    Return Values: TText*

    Errors/Exceptions: If parameter is wrong value.

    Parameter is under zero or below the amount of cases.
    
    Status: Draft
    
-------------------------------------------------------------------------------
*/
const TText* CTestBase::GetCaseNameL
        (
        TInt aClassCase // The number of the case.
        )
    {

    const TText* caseName = NULL;
    TInt caseCount( iNameArray->Count() );
    // If case out of limits panic.
    if( aClassCase < 0 || aClassCase > caseCount )
        {
        RDebug::Print( _L("CTestBase GetCaseNameL") );
        User::Panic( _L("CTestBase"), 0 );
        }
    caseName = iNameArray->At( aClassCase );

    return caseName;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestBase

    Method: ~CTestBase

    Description: Destructor

    Handles the deletion of member variables.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None
    
    Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestBase::~CTestBase
        (
        // None
        )
    {

    if( iArray )
        {
        for( TInt i( 0 ); i < iArray->Count(); i++ )
            {
            iArray->Delete( i );
            }
        iArray->Reset();
        delete iArray;
        }
    if( iNameArray )
        {
        for( TInt i( 0 ); i < iNameArray->Count(); i++ )
            {
            iNameArray->Delete( i );
            }
        iNameArray->Reset();
        delete iNameArray;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestBase

    Method: AddTestCaseL

    Description: Add a new test case.

    Adds new test case.

    Return Values: None
    
    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestBase::AddTestCaseL
        (
        TInt (MTest::* aTestFunction)( TExecutionAndDebugInfo& ),
        const TText* aFunctionName
        )
    {

    // If not initialized the function array.
    if( !iArray )
        {
        iArray = new( ELeave )CArrayFixFlat<MTestFunction>( 1 );
        }
    // Add function to array.
    iArray->AppendL( ( MTestFunction )( ( aTestFunction ) ) );
    iNumberOfCases++;
    // If not initialized the case name array.
    if( !iNameArray )
        {
        iNameArray = new( ELeave )CArrayFixFlat<const TText*>( 1 );
        }
    // Add name to array.
    const TText* text = aFunctionName;
    iNameArray->AppendL( text );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestBase

    Method: AddTestCaseL

    Description: Add a new test case.

    Adds new test case.

    Return Values: None
    
    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestBase::AddTestCaseL
        (
        const TDesC& aFunctionName, // The name of the function as descriptor.
        TInt (MTest::* aTestFunction)( TExecutionAndDebugInfo& ) // Function Ptr
        )
    {

    // If not initialized the function array.
    if( !iArray )
        {
        iArray = new( ELeave )CArrayFixFlat<MTestFunction>( 1 );
        }
    // Add function to array.
    iArray->AppendL( ( MTestFunction )( ( aTestFunction ) ) );
    iNumberOfCases++;
    // If not initialized the case name array.
    if( !iNameArray )
        {
        iNameArray = new( ELeave )CArrayFixFlat<const TText*>( 1 );
        }
    // Add name to array.
    const TText* text = aFunctionName.Ptr();
    iNameArray->AppendL( text );

    }

// End of file.
