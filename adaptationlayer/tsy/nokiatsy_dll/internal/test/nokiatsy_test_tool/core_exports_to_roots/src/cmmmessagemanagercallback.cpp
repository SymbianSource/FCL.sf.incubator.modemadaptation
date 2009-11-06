/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Forwards Tsy requests from Symbian OS to Domestic OS
*
*/



//  INCLUDE FILES
#include "tsylogger.h"
#include "cmmessagemanagercallback.h"
#include "core.h"

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None



// ================= MEMBER FUNCTIONS =======================================


// -----------------------------------------------------------------------------
// CMmMessageManagerCallback::NewL
// Creates new CMmMessageManagerCallback object and returns it. Because the 
// constructor of CMmMessageManagerCallback class is private, calling this 
// method is the only possible way to make a new CMmMessageManagerCallback
// object.
// -----------------------------------------------------------------------------
//
CMmMessageManagerCallback* CMmMessageManagerCallback::NewL
        (
        CCore* aCore
        )
    {
    TFLOGSTRING("TSY: CMmMessageManagerCallback::NewL");

    CMmMessageManagerCallback* iMessageManagerCallBack =
        new ( ELeave ) CMmMessageManagerCallback();
    
    iMessageManagerCallBack->ConstructL ( aCore );

    return iMessageManagerCallBack;
    }


// -----------------------------------------------------------------------------
// CMmMessageManagerCallback::~CMmMessageManagerCallback
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmMessageManagerCallback::~CMmMessageManagerCallback
        (
        // none
        )
    {
    TFLOGSTRING("TSY: CMmMessageManagerCallback::~CMmMessageManagerCallback");
    }
    
 
// -----------------------------------------------------------------------------
// CMmMessageManagerCallback::NewL
// Creates new CMmMessageManagerCallback object and returns it. Because the 
// constructor of CMmMessageManagerCallback class is private, calling this 
// method is the only possible way to make a new CMmMessageManagerCallback
// object.
// -----------------------------------------------------------------------------
//
void CMmMessageManagerCallback::ConstructL
        (
        CCore* aCore
        )
    {
    TFLOGSTRING("TSY: CMmMessageManagerCallback::ConstructL");
    
    iCore = aCore;

    }


// -----------------------------------------------------------------------------
// CMmMessageManagerCallback::CMmMessageManagerCallback
// Constructor
// -----------------------------------------------------------------------------
//
CMmMessageManagerCallback::CMmMessageManagerCallback
        (
        // none
        )
    {
    TFLOGSTRING("TSY: CMmMessageManagerCallback::CMmMessageManagerCallback");
    // none
    }

// -----------------------------------------------------------------------------
// CMmMessageManagerCallback::Complete
// Complete without a data package
// -----------------------------------------------------------------------------
//
void CMmMessageManagerCallback::Complete
        (
        TInt aIpc,
        TInt aResult
        )
    {
    TFLOGSTRING("TSY: CMmMessageManagerCallback::Complete");

    CMmDataPackage emptyDataPackage;
    Complete( aIpc, &emptyDataPackage, aResult );
    return;

    }

// -----------------------------------------------------------------------------
// CMmMessageManagerCallback::Complete
// Complete with a data package
// -----------------------------------------------------------------------------
//
void CMmMessageManagerCallback::Complete
        (
        TInt aIpc,
        CMmDataPackage* data,
        TInt aResult
        )
    {
    TFLOGSTRING4("TSY: CMmMessageManagerCallback::Complete. IPC = %d (%S), result: %d", aIpc, &MapIpcToName(aIpc), aResult);

    if( EMmTsyBootNotifyModemStatusReadyIPC == aIpc )
        {
        //Sim ready
        iCore->HandleRequestL( EMmTsyBootNotifySimStatusReadyIPC );
        }

    iCore->RouteCompletion( aIpc, data, aResult );

    return;

    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

    //None


//  End of File

