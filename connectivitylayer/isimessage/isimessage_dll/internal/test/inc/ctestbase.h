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

    This file contains the header file of CLoopTest of 
    CTestModule

-------------------------------------------------------------------------------
*/

#ifndef CTestBase_H
#define CTestBase_H

// INCLUDES
#include "mtest.h"

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
//typedef TPckgBuf< TUint8 > TPnPepPipeHandlePckg;

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// A typedef for function that does the actual testing,
// function is a type TInt NameOfFunction ( TTestResult& aResult )
typedef TInt (MTest::* MTestFunction)( TExecutionAndDebugInfo& ); 

// DESCRIPTION
// Abstract base class for test classes.
class CTestBase : public CBase, public MTest
    {
    
    public:

        /**
        * Returns the name of the case.
        * @param TInt aCase
        * @return TText*, the name of the test case.
        */
        const TText* GetCaseNameL( TInt aClassCase );

        /**
        * Returns array of function pointers.
        * No ownership of the array is given to calling function.
        * @param None
        * @return CArrayFixFlat<MTestFunction>*, array of function pointers.
        */
        CArrayFixFlat<MTestFunction>* GetFunctionPtrArray(){ return iArray;}

        /**
        * Destructor
        * Handles the deletion of member variables.
        * @param None
        * @return None
        */
        virtual ~CTestBase();

    protected:        

        /**
        * Adds new test case.
        * Get as first parameter the pointer to function. As second parameter
        * gets the test case's name.
        * @param TInt (MTest::* aTestFunction)( TExecutionAndDebugInfo& )
        * @param const TText* aFunctionName
        * @return None
        */
        void AddTestCaseL( TInt (MTest::* aTestFunction)( TExecutionAndDebugInfo& ),
                           const TText* aFunctionName
                         );

        /**
        * Adds new test case.
        * Get as first parameter the pointer to function. As second parameter
        * gets the test case's name.
        * @param const TDesC& aFunctionName, the name of the function.
        * @param TInt (MTest::* aTestFunction)( TExecutionAndDebugInfo& )
        * @return None
        */
        void AddTestCaseL( const TDesC& aFunctionName,
                           TInt (MTest::* aTestFunction)( TExecutionAndDebugInfo& )
                         );

        /**
        * Initializes the cases.
        * Must be implemneted in child clases to make sure that the cases are
        * initialized.
        * @param None
        * @return None
        */        
        virtual void InitializeCasesL() = 0;
    
    private:

        // None

    protected:

        // Array for test function pointers.
        CArrayFixFlat<MTestFunction>*   iArray;
        // Array for test function names.
        CArrayFixFlat<const TText*>*    iNameArray;
        // The number of cases.
        TInt                            iNumberOfCases;

    };

#endif

// End of file.