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



// INCLUDES
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
// CDpRx2Dte::ReadPending
// This method informs, whether read is pending.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TBool CDpRx2Dte::ReadPending()
    {
    return iReadPending;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::LengthWanted
// This method informs the wanted length.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CDpRx2Dte::LengthWanted()
    {
    return iClientRequestedLength;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::IPCWriteOffset
// This method gests write offset.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::IPCWriteOffset()
    {
    return iIPCWriteOffset;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// none

//  End of File
