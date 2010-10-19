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



#include "cmodematsrv.h"
#include "modematclientsrv.h"
#include "modemattrace.h"

static void RunServerL();

/**
* Main entry-point for the server process.
**/
GLDEF_C TInt E32Main()
    {
    

    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt retTrap = KErrNoMemory;
    if ( cleanup )
        {
        TRAP( retTrap, RunServerL() );
        delete cleanup;
        }
    __UHEAP_MARKEND;
    
    return retTrap;
    }

/**
Create the active scheduler and server instances, and start the
server.
*/
static void RunServerL()
    {
    
    TFindServer find( KATExtSrvName );
    TFullName name;

    if ( find.Next(name) == KErrNone )
        {
        // Server is already up
        C_TRACE((_L("modematcontroller.exe already running.")));
        return;
        }

    User::LeaveIfError( User::RenameThread(KATExtSrvName) );
    // Create and install the active scheduler.
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    // Create the server.
    CModemAtSrv::NewLC();

    // Initialisation complete, now signal the client
    RProcess::Rendezvous( KErrNone );

    // Ready to run. This only returns when the server is closing down.
    CActiveScheduler::Start();

    // Clean up the server and scheduler.
    CleanupStack::PopAndDestroy( 2 );
    }

