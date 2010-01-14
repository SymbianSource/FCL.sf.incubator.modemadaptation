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
#include "cusbpnserversession.h"
#include "cusbpnserver.h"
#include "usbpndefinitions.h"
#include "usbpntrace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnserversessionTraces.h"
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
// CUsbPnServerSession::CUsbPnServerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnServerSession::CUsbPnServerSession( CUsbPnServer& aServer )
    :iUsbPnServer( aServer )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_CUSBPNSERVERSESSION_ENTRY, "CUsbPnServerSession::CUsbPnServerSession" );
    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_CUSBPNSERVERSESSION_EXIT, "CUsbPnServerSession::CUsbPnServerSession - return" );
    C_TRACE( ( _T( "CUsbPnServerSession::CUsbPnServerSession()" ) ) );
    C_TRACE( ( _T( "CUsbPnServerSession::CUsbPnServerSession() - return" ) ) );
    }

// Destructor
CUsbPnServerSession::~CUsbPnServerSession()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_CUSBPNSERVERSESSION_DESTRUCTOR_ENTRY, "CUsbPnServerSession::~CUsbPnServerSession" );
    C_TRACE( ( _T( "CUsbPnServerSession::~CUsbPnServerSession()" ) ) );
    iUsbPnServer.SessionClosed();
    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_CUSBPNSERVERSESSION_DESTRUCTOR_EXIT, "CUsbPnServerSession::~CUsbPnServerSession - return" );
    C_TRACE( ( _T( "CUsbPnServerSession::~CUsbPnServerSession() - return" ) ) );
    }


// -----------------------------------------------------------------------------
// CUsbPnServerSession::ServiceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnServerSession::ServiceL(
    const RMessage2& aMessage )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_SERVICEL_ENTRY, "CUsbPnServerSession::ServiceL" );
    C_TRACE( ( _T( "CUsbPnServerSession::ServiceL()" ) ) );

    TInt ret = KErrNone;
    switch( aMessage.Function() )
        {
        case EPnDetach:
            {
            OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_SERVICEL, "CUsbPnServerSession::ServiceL - EPnDisconnect" );
            C_TRACE( ( _T( "CUsbPnServerSession::ServiceL - EPnDisconnect" ) ) );
            iUsbPnServer.Detach();
            break;
            }
        default:
            {
            OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_SERVICEL_DUP1, "CUsbPnServerSession::ServiceL - Wrong param from client" );
            C_TRACE( ( _T( "CUsbPnServerSession::ServiceL - Wrong param from client" ) ) );
            aMessage.Panic(KUsbPnPanicCat, KErrArgument);
            break;
            }
        }

    aMessage.Complete(ret);

    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_SERVICEL_EXIT, "CUsbPnServerSession::ServiceL - return void" );
    C_TRACE( ( _T( "CUsbPnServerSession::ServiceL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnServerSession::CreateL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnServerSession::CreateL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_CREATEL_ENTRY, "CUsbPnServerSession::CreateL" );
    C_TRACE( ( _T( "CUsbPnServerSession::CreateL()" ) ) );
    iUsbPnServer.SessionOpened();
    OstTrace0( TRACE_NORMAL, CUSBPNSERVERSESSION_CREATEL_EXIT, "CUsbPnServerSession::CreateL - return void" );
    C_TRACE( ( _T( "CUsbPnServerSession::CreateL() - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
