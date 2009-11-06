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
#include <e32std.h> // For basic data types
#include <ecom/implementationproxy.h> // For IMPLEMENTATION_PROXY_ENTRY, TImplementationProxy

#include "usbpnclasscontroller.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TUint32 KUsbPhonetImplementationId = 0x101FB43A;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
const TImplementationProxy UsbCCImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KUsbPhonetImplementationId, CUsbPNClassController::NewL),
    };

// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================


// ---------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(UsbCCImplementationTable) / sizeof(TImplementationProxy);
    return UsbCCImplementationTable;
    }


// ============================ MEMBER FUNCTIONS ===============================

