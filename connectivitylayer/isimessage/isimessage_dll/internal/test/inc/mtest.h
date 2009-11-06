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

    This file contains the header file of MTest of 
    CTestModule

-------------------------------------------------------------------------------
*/

#ifndef MTEST_H
#define MTEST_H

// INCLUDES
#include "e32std.h" 
#include <stiftestmodule.h>
#include "texecutionanddebuginfo.h"

// Function pointer related internal definitions

// Hack around known GCC bug.
#ifndef __GCC32__
    #define GETPTR
#else
    #define GETPTR &
#endif  

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
class MTest;

// A typedef for function that does the actual testing,
// function is a type TInt NameOfFunction ( TTestResult& aResult )
typedef TInt (MTest::* MTestFunction)( TExecutionAndDebugInfo& ); 

// DESCRIPTION
// Interface for test cases.
class MTest
    {

    public:
    
        virtual const TText* GetCaseNameL( TInt aClassCase ) = 0;

        /**
        * Returns array of function pointers.
        * No ownership of the array is given to calling function.
        * @param None
        * @return CArrayFixFlat<MTestFunction>*, array of function pointers.
        */
        virtual CArrayFixFlat<MTestFunction>* GetFunctionPtrArray() = 0;
  
    };

#endif

// End of File
