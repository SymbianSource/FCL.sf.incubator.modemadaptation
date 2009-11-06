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
#include <etelext.h>
#include <etelmm.h>
#include <ctsy/rmmcustomapi.h>
#include "rmmcustomapiext.h"
#include "cmmcustomextptrh.h"

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
    //None
enum
    { 
    ESlotNotifyProgrammableOperatorId,
    ESlotNotifyMultimediaDataRate,
    //let the next value be the last in the enum
    ESlotMaxNumOfNonConstSlots
    };

//Const slots enumeration
enum
    {
    ESlotCSetSimConnection,

    //let the next value be the last in the enum
    ESlotCMaxNumOfConstSlots,
    };

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ============================= LOCAL FUNCTIONS ===============================

    //None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RMmCustomApiExt::RMmCustomApiExt
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RMmCustomApiExt::RMmCustomApiExt() : iCustomExtPtrHolder( NULL )
    {
    }

// -----------------------------------------------------------------------------
// RMmCustomApiExt::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void RMmCustomApiExt::ConstructL()
    {
    iCustomExtPtrHolder = CMmCustomExtPtrHolder::NewL( ESlotMaxNumOfNonConstSlots,
        ESlotCMaxNumOfConstSlots );
    }

// Destructor
EXPORT_C void RMmCustomApiExt::Destruct()
    {
    if ( NULL != iCustomExtPtrHolder )
        {
        delete iCustomExtPtrHolder;
        }
    iCustomExtPtrHolder = NULL;
    }

// -----------------------------------------------------------------------------
// RMmCustomApiExt::Open
// This method opens a RMmCustomApiExt subsession from RMobilePhone.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomApiExt::Open(
    RMobilePhone& aPhone )
    {
    TInt ret = RMmCustomAPI::Open( aPhone );
    
    if ( KErrNone == ret )
        {
        TRAP(ret, ConstructL());
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// RMmCustomApiExt::Close
// This method closes a RMmCustomApiExt subsession
// -----------------------------------------------------------------------------
//
EXPORT_C void RMmCustomApiExt::Close()
    {

    CloseSubSession( EEtelClose );
    Destruct();

    //Call also Destruct() from base class (deletes base class's pointer
    //container)
    RMmCustomAPI::Destruct();
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
    //None

//  End of File
