/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmcustomextptrh.h"
#include <etelext.h>

// EXTERNAL DATA STRUCTURES
//  None

// EXTERNAL FUNCTION PROTOTYPES  
//  None

// CONSTANTS
//  None

// MACROS
//  None

// LOCAL CONSTANTS AND MACROS
//  None

// MODULE DATA STRUCTURES
//  None

// LOCAL FUNCTION PROTOTYPES
//  None

// FORWARD DECLARATIONS
//  None

// ============================= LOCAL FUNCTIONS ===============================
//  None

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CMmCustomExtPtrHolder::CMmCustomExtPtrHolder
// This method is constructor of class RMmCustomAPI.
// -----------------------------------------------------------------------------
//
CMmCustomExtPtrHolder::CMmCustomExtPtrHolder()
    {
    }

// -----------------------------------------------------------------------------
// CMmCustomExtPtrHolder::ConstructL
// This method is the second phase constructor of class RMmCustomAPI.
// -----------------------------------------------------------------------------
//
void CMmCustomExtPtrHolder::ConstructL(
    const TInt aSizeOfPtrArray,
    const TInt aSizeOfPtrCArray )
    {

    TPtr8 ptr( NULL,0 );
    TInt i;
    TInt ret( KErrNone );
    for ( i=0;i<aSizeOfPtrArray;i++ )
        {
        ret = iPtrArray.Append( ptr );
        // In case of an error, remember to close the array.
        if ( ret != KErrNone )
            {
            iPtrArray.Close();
            User::Leave( ret );
            }
        }
    TPtrC8 ptrC( NULL,0 );
    for ( i=0;i<aSizeOfPtrCArray;i++ )
        {
        ret = iPtrCArray.Append( ptrC );
        // In case of an error, remember to close both arrays.
        if( ret != KErrNone )
            {
            iPtrArray.Close();
            iPtrCArray.Close();
            User::Leave( ret );
            }
        }

    }

// -----------------------------------------------------------------------------
// CMmCustomExtPtrHolder::NewL
// This method is two-phased constructor of class RMmCustomAPI.
// -----------------------------------------------------------------------------
//
CMmCustomExtPtrHolder* CMmCustomExtPtrHolder::NewL(
    const TInt aSizeOfPtrArray,
    const TInt aSizeOfPtrCArray )
    {

    CMmCustomExtPtrHolder* p = new ( ELeave ) CMmCustomExtPtrHolder();
    CleanupStack::PushL( p );
    p->ConstructL( aSizeOfPtrArray,aSizeOfPtrCArray );
    CleanupStack::Pop();
    return p;

    }

// Destructor
CMmCustomExtPtrHolder::~CMmCustomExtPtrHolder()
    {

    iPtrArray.Close();
    iPtrCArray.Close();

    }

// -----------------------------------------------------------------------------
// CMmCustomExtPtrHolder::Ptr
// This method returns pointer from pointer array.
// -----------------------------------------------------------------------------
//
TPtr8& CMmCustomExtPtrHolder::Ptr(
    const TInt aIndex )
    {

    __ASSERT_ALWAYS( aIndex<iPtrArray.Count(),PanicClient(
                                                  EEtelPanicIndexOutOfRange ) );
    return iPtrArray[aIndex];

    }


// -----------------------------------------------------------------------------
// CMmCustomExtPtrHolder::PtrC
// This method returns pointer from pointer array.
// -----------------------------------------------------------------------------
//
TPtrC8& CMmCustomExtPtrHolder::PtrC(
    const TInt aIndex )
    {

    __ASSERT_ALWAYS( aIndex<iPtrCArray.Count(),PanicClient(
                                                EEtelPanicIndexOutOfRange ) );
    return iPtrCArray[aIndex];

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  None

//  End of File  
