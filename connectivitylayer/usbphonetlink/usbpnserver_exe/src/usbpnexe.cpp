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
#include    "cusbpnscheduler.h"
#include    "usbpntrace.h"

#include    "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include    "usbpnexeTraces.h"
#endif

#include    <e32std.h>          // For User

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
_LIT( KUsbPnServerThreadName, "USB Phonet Link" );
#ifdef _DEBUG
_LIT(KTraceName, "USBPNLINK");
#endif

// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TInt err( KErrNone );

    // Rename server thread name
    err = User::RenameThread( KUsbPnServerThreadName );

    if( err )
        {
        TRACE_ASSERT_ALWAYS;
        }

    OstTrace0( TRACE_BORDER, _E32MAIN_ENTRY, "USBPhonetLink - UsbPnExe::E32Main" );
    A_TRACE( _L( "USBPhonetLink - UsbPnExe::E32Main()" ) );
    BUILD_TRACE( &KTraceName );

    err = CUsbPnScheduler::ThreadStart();

    if( err )
        {
        TRACE_ASSERT_ALWAYS;
        }

    OstTrace1( TRACE_BORDER, _E32MAIN_EXIT, "USBPhonetLink - UsbPnExe::E32Main - return %d", err );
    A_TRACE( ( _T( "USBPhonetLink - UsbPnExe::E32Main() return %d" ), err ) );
    return err;
    }

//  End of File
