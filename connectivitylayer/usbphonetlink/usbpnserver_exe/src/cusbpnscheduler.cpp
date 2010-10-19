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
#include    "cusbpnserver.h"
#include    "usbpntrace.h"

#include    "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include    "cusbpnschedulerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUsbPnScheduler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnScheduler::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSCHEDULER_CONSTRUCTL_ENTRY, "CUsbPnScheduler::ConstructL" );
    C_TRACE( ( _T( "CUsbPnScheduler::ConstructL()" ) ) );

    /* Create active scheduler */
    CUsbPnScheduler* self = new (ELeave) CUsbPnScheduler;
    CleanupStack::PushL(self);
    CActiveScheduler::Install( self);

    TInt err( KErrNone );

    /* Create USB Phonet Link server */
    self->iServer = CUsbPnServer::NewL();

    /* Signal error code to client */
    RProcess::Rendezvous( err );

    if( err )
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( err );
        }

    /* Start active scheduler (Wait loop) */
    CActiveScheduler::Start();

    /* Active scheduler stopped */
    CleanupStack::PopAndDestroy(self);

    OstTrace0( TRACE_NORMAL, CUSBPNSCHEDULER_CONSTRUCTL_EXIT, "CUsbPnScheduler::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnScheduler::ConstructL() - return void" ) ) );
    }

// Destructor
CUsbPnScheduler::~CUsbPnScheduler()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSCHEDULER_CUSBPNSCHEDULER_DESTRUCTOR_ENTRY, "CUsbPnScheduler::~CUsbPnScheduler" );
    C_TRACE( ( _T( "CUsbPnScheduler::~CUsbPnScheduler()" ) ) );

    if( iServer )
        {
        delete iServer;
        }

    OstTrace0( TRACE_NORMAL, CUSBPNSCHEDULER_CUSBPNSCHEDULER_DESTRUCTOR_EXIT, "CUsbPnScheduler::~CUsbPnScheduler - return" );
    C_TRACE( ( _T( "CUsbPnScheduler::~CUsbPnScheduler() - return" ) ) );
    }


// -----------------------------------------------------------------------------
// CUsbPnScheduler::ThreadStart
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CUsbPnScheduler::ThreadStart( )
    {
    OstTrace0( TRACE_BORDER, CUSBPNSCHEDULER_THREADSTART_ENTRY, "CUsbPnScheduler::ThreadStart" );
    A_TRACE(_T("CUsbPnScheduler::ThreadStart()"));
    TInt err(KErrNone);

    CTrapCleanup* cleanup = CTrapCleanup::New();

#ifdef _DEBUG
    // To make sure that cleanupstack will not grow before __UHEAP_MARKEND
    // to prevent server panic in shut down in UDEB
    TRAP( err, for( TInt i = 0; i < 20; i++) CleanupStack::PushL( ( TAny* )NULL );
    CleanupStack::Pop(20);
    );
#endif

    if(cleanup)
        {
        __UHEAP_MARK;
        TRAP( err, ConstructL() );
        __UHEAP_MARKEND;
        delete cleanup;
        }


    OstTrace1( TRACE_BORDER, CUSBPNSCHEDULER_THREADSTART_EXIT, "CUsbPnScheduler::ThreadStart - return %d", err );
    A_TRACE((_T("CUsbPnScheduler::ThreadStart() - return %d"), err));
    return err;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
