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
#include    "cusbpnserver.h"
#include    "cusbpnserversession.h"
#include    "cusbpninterface.h"
#include    "usbpntrace.h"

#include    "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include    "cusbpnserverTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUsbPnServer::CUsbPnServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnServer::CUsbPnServer()
    :CServer2(CActive::EPriorityStandard)
    ,iSessionCount(0)
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_CUSBPNSERVER_ENTRY, "CUsbPnServer::CUsbPnServer" );
    C_TRACE( ( _T( "CUsbPnServer::CUsbPnServer()" ) ) );

    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_CUSBPNSERVER_EXIT, "CUsbPnServer::CUsbPnServer - return" );
    C_TRACE( ( _T( "CUsbPnServer::CUsbPnServer() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnServer::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_CONSTRUCTL_ENTRY, "CUsbPnServer::ConstructL" );
    C_TRACE( ( _T( "CUsbPnServer::ConstructL()" ) ) );

    StartL( KUsbPnServerName );
    iInterface = CUsbPnInterface::NewL();

    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_CONSTRUCTL_EXIT, "CUsbPnServer::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnServer::ConstructL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnServer* CUsbPnServer::NewL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_NEWL_ENTRY, "CUsbPnServer::NewL" );
    C_TRACE( ( _T( "CUsbPnServer::NewL()" ) ) );

    CUsbPnServer* self = new( ELeave ) CUsbPnServer;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTrace1( TRACE_NORMAL, CUSBPNSERVER_NEWL_EXIT, "CUsbPnServer::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnServer::NewL() - return 0x%x" ), self ) );
    return self;
    }


// Destructor
CUsbPnServer::~CUsbPnServer()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_CUSBPNSERVER_DESTRUCTOR_ENTRY, "CUsbPnServer::~CUsbPnServer" );
    C_TRACE( ( _T( "CUsbPnServer::~CUsbPnServer()" ) ) );

    if( iInterface )
        {
        delete iInterface;
        }
    iInterface = NULL;
    iSessionCount = 0;

    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_CUSBPNSERVER_DESTRUCTOR_EXIT, "CUsbPnServer::~CUsbPnServer - return" );
    C_TRACE( ( _T( "CUsbPnServer::~CUsbPnServer() - return" ) ) );
    }


// -----------------------------------------------------------------------------
// CUsbPnServer::NewSessionL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSession2* CUsbPnServer::NewSessionL(
    const TVersion& aVersion,
    const RMessage2& aMessage ) const
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_NEWSESSIONL_ENTRY, "CUsbPnServer::NewSessionL" );
    C_TRACE( ( _T( "CUsbPnServer::NewSessionL()" ) ) );

    aMessage.HasCapabilityL( ECapabilityCommDD );

    CSession2* session = new(ELeave) CUsbPnServerSession( const_cast<CUsbPnServer&>(*this) );

    OstTrace1( TRACE_NORMAL, CUSBPNSERVER_NEWSESSIONL_EXIT, "CUsbPnServer::NewSessionL - return;session=%x", session );
    C_TRACE( ( _T( "CUsbPnServer::NewSessionL() - return 0x%x" ), session ) );

    return session;
    }
// -----------------------------------------------------------------------------
// CUsbPnServer::SessionOpened
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnServer::SessionOpened()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_SESSIONOPENED_ENTRY, "CUsbPnServer::SessionOpened" );
    C_TRACE( ( _T( "CUsbPnServer::SessionOpened()" ) ) );

    iSessionCount++;
    OstTrace1( TRACE_NORMAL, CUSBPNSERVER_SESSIONOPENED, "CUsbPnServer::SessionOpened - session count = 0x%x", iSessionCount );
    C_TRACE( ( _T( "CUsbPnServer::SessionOpened() - session count = 0x%x" ), iSessionCount ) );

    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_SESSIONOPENED_EXIT, "CUsbPnServer::SessionOpened - return void" );
    C_TRACE( ( _T( "CUsbPnServer::SessionOpened() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnServer::SessionClosed
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnServer::SessionClosed()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_SESSIONCLOSED_ENTRY, "CUsbPnServer::SessionClosed" );
    C_TRACE( ( _T( "CUsbPnServer::SessionClosed()" ) ) );

    iSessionCount--;
    OstTrace1( TRACE_NORMAL, CUSBPNSERVER_SESSIONCLOSED, "CUsbPnServer::SessionClosed - session count = 0x%x", iSessionCount );
    C_TRACE( ( _T( "CUsbPnServer::SessionClosed() - session count = 0x%x" ), iSessionCount ) );

    if( iSessionCount == 0 )
        {
        OstTrace0( TRACE_NORMAL, CUSBPNSERVER_SESSIONCLOSED_DUP1, "CUsbPnServer::SessionClosed - shutdown USBPNServer" );
        C_TRACE( ( _T( "CUsbPnServer::SessionClosed() - shutdown USBPNServer" ) ) );
        CActiveScheduler::Stop();
        }

    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_SESSIONCLOSED_EXIT, "CUsbPnServer::SessionClosed - return void" );
    C_TRACE( ( _T( "CUsbPnServer::SessionClosed() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnServer::Detach
// Deletes interface so it's safe for class controller's Stop() to return
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnServer::Detach()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_DETACH_ENTRY, "CUsbPnServer::Detach" );
    C_TRACE( ( _T( "CUsbPnServer::Detach()" ) ) );

    // Delete delete interface if last session and it exists
    if( (iSessionCount < 2) && (iInterface) )
        {
        delete iInterface;
        }
    iInterface = NULL;

    OstTrace0( TRACE_NORMAL, CUSBPNSERVER_DETACH_EXIT, "CUsbPnServer::Detach - return void" );
    C_TRACE( ( _T( "CUsbPnServer::Detach() - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
