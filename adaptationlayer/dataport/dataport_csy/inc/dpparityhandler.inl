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
// none

// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES  
// none

// CONSTANTS
// none

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================

// none


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDpParityHandler::Even
// This method returns even parity for byte.
// -----------------------------------------------------------------------------
//
inline const TUint8 CDpParityHandler::Even(
    const TUint8 aByte )
    {
    return iTable[aByte][0];
    }

// -----------------------------------------------------------------------------
// CDpParityHandler::Odd
// This method returns odd parity for byte.
// -----------------------------------------------------------------------------
//
inline const TUint8 CDpParityHandler::Odd(
    const TUint8 aByte )
    {
    return iTable[aByte][1];
    }

// -----------------------------------------------------------------------------
// CDpParityHandler::Space
// This method return space parity for byte.
// -----------------------------------------------------------------------------
//
inline const TUint8 CDpParityHandler::Space(
    const TUint8 aByte )
    {
    return iTable[aByte][2];
    }

// -----------------------------------------------------------------------------
// CDpParityHandler::Mark
// This method return mark parity for byte.
// -----------------------------------------------------------------------------
//
inline const TUint8 CDpParityHandler::Mark(
    const TUint8 aByte )
    {
    return iTable[aByte][3];
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// none

//  End of File
