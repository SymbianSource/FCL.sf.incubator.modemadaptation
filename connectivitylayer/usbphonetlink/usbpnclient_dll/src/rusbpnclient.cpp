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
#include "rusbpnclient.h"
#include "cusbpnserver.h"
#include "usbpndefinitions.h"
#include "usbpntrace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "rusbpnclientTraces.h"
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
// RUsbPnClient::RUsbPnClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RUsbPnClient::RUsbPnClient()
    :RSessionBase()
    {
    }

// -----------------------------------------------------------------------------
// RUsbPnClient::ConnectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RUsbPnClient::ConnectL()
    {
    OstTrace0( TRACE_API, RUSBPNCLIENT_CONNECTL_ENTRY, "RUsbPnClient::ConnectL" );
    A_TRACE( ( _T( "RUsbPnClient::ConnectL()" ) ) );

    TInt err( KErrNone );

    // Create USB Phonet Link Server session
    err = CreateSession( KUsbPnServerName, TVersion(1,0,0));

    if ( err == KErrNotFound )
        {
        // Session not created

        // Find lock semaphore for server process creation
        TFindSemaphore lock( KUsbPnServerName );
        TFullName semaphoreName;
        err = lock.Next( semaphoreName );

        if ( err == KErrNotFound )
            {
            // Lock is not enabled
            OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL, "RUsbPnClient::ConnectL() - semaphore not found, start server" );
            E_TRACE( ( _L( "RUsbPnClient::ConnectL() - semaphore not found, start server" ) ) );

            RSemaphore startLock;

            // Create lock
            User::LeaveIfError( startLock.CreateGlobal( KUsbPnServerName, 0, EOwnerProcess ) );


            /********************************************/
            /* Start the USB Phonet Link process process */
            TRequestStatus status;
            RProcess server;

            User::LeaveIfError( server.Create( KUsbPnServerName, TPtrC( NULL, 0),
                         EOwnerThread ) );

            server.Rendezvous( status );

            if( status != KRequestPending )
              {
              server.Kill(0);  // Abort startup
              }
            else
              {
              server.Resume();  // Logon OK -> start the server
              }

            OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP1, "RUsbPnClient::ConnectL() - waiting server response" );
            E_TRACE( ( _T( "RUsbPnClient::ConnectL() - waiting server response" ) ) );
            User::WaitForRequest( status );  // Wait for start or death

            // we can't use the 'exit reason' if the server panicked as this
            // is the panic 'reason' and may be '0' which cannot be distinguished
            // from KErrNone
            TInt err = status.Int();
            if (err == KErrNone && (server.ExitType() == EExitPanic || server.ExitType() == EExitKill))
              {
              err = KErrServerTerminated;
              }

            server.Close();

            if( err )
                {
                OstTrace1( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP2, "RUsbPnClient::ConnectL() - waiting server response status; err=%d", err );
                E_TRACE( ( _T( "RUsbPnClient::ConnectL() - waiting server response status: %d" ), err ) );
                TRACE_ASSERT_ALWAYS;
                User::LeaveIfError( err );
                }
            /* End of starting process */
            /********************************************/

            OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP3, "RUsbPnClient::ConnectL() - server is started, signal other clients" );
            E_TRACE( ( _L( "RUsbPnClient::ConnectL() - server is started, signal other clients" ) ) );
            // Signal other clients
            startLock.Signal( KMaxTInt );

            // Close semaphore
            startLock.Close();
            }
        else
            {
            // Lock is enabled

            RSemaphore startLock;

            // Open lock semaphore
            User::LeaveIfError( startLock.Open( lock ) );

            OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP4, "RUsbPnClient::ConnectL() - server is starting, wait for signal" );
            E_TRACE( ( _L( "RUsbPnClient::ConnectL() - server is starting, wait for signal" ) ) );
            // Wait for signal
            startLock.Wait();
            OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP5, "RUsbPnClient::ConnectL() - signal received" );
            E_TRACE( ( _L( "RUsbPnClient::ConnectL() - signal received" ) ) );

            // Close semaphore
            startLock.Close();

            }

        // Create USB Phonet Link server session
        OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP6, "RUsbPnClient::ConnectL() - Create session" );
        E_TRACE( ( _L( "RUsbPnClient::ConnectL() - Create session" ) ) );

        User::LeaveIfError( CreateSession( KUsbPnServerName, TVersion(1,0,0) ) );

        OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP7, "RUsbPnClient::ConnectL() - session created1" );
        E_TRACE( ( _L( "RUsbPnClient::ConnectL() - session created1" ) ) );
        }
    else if ( err )
        {
        OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP8, "RUsbPnClient::ConnectL() - session not created, reason uknown" );
        E_TRACE( ( _L( "RUsbPnClient::ConnectL() - session not created, reason uknown" ) ) );            
        TRACE_ASSERT_ALWAYS;
        User::Leave( err );
        }
    else
        {
        OstTrace0( TRACE_DETAILED, RUSBPNCLIENT_CONNECTL_DUP9, "RUsbPnClient::ConnectL() - session created2" );
        E_TRACE( ( _T( "RUsbPnClient::ConnectL() - session created2" ) ) );
        }
    OstTrace0( TRACE_API, RUSBPNCLIENT_CONNECTL_EXIT, "RUsbPnClient::ConnectL() - return void" );
    A_TRACE( ( _T( "RUsbPnClient::ConnectL() - return void" ) ) );
    }

//-----------------------------------------------------------------------------
// RPtp::DestroyPtpStatck()
// Destroy PTP stack
//-----------------------------------------------------------------------------
//
EXPORT_C void RUsbPnClient::Detach()
    {
    OstTrace0( TRACE_API, RUSBPNCLIENT_DETACH_ENTRY, "RUsbPnClient::Detach" );
    A_TRACE( ( _T( "RUsbPnClient::Disconnect()" ) ) );
    SendReceive( EPnDetach );
    // close the session
    RSessionBase::Close();
    OstTrace0( TRACE_API, RUSBPNCLIENT_DETACH_EXIT, "RUsbPnClient::Detach - return void" );
    A_TRACE( ( _T( "RUsbPnClient::Disconnect() - return void" ) ) );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
