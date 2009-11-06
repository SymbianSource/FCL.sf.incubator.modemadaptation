/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handles notification of the active object
*
*/



// INCLUDE FILES
#include "notifyhandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::CSimAtkNotifyHandler
// Standart constractor
// -----------------------------------------------------------------------------
//
CSimAtkNotifyHandler::CSimAtkNotifyHandler
        (
        CSimAtkNotify* aNotify,
        CStifLogger* aNotifyLog
        ) : CActive(EPriorityStandard)												
	{
	iNotify = aNotify;
	iLog = aNotifyLog;
	iLog->Log( _L( "CSimAtkNotifyHandler::CSimAtkNotifyHandler \
	    - Construction ongoing " ) );
	CActiveScheduler::Add( this );
	iLog->Log( _L( "CSimAtkNotifyHandler::CActiveScheduler " ) );	
	}

// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::ConstructL
// Standart ConstructL
// -----------------------------------------------------------------------------
//
void CSimAtkNotifyHandler::ConstructL()
	{
	// No implementation needed
	}

// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::NewL
// Standart two way construct
// -----------------------------------------------------------------------------
//
CSimAtkNotifyHandler* CSimAtkNotifyHandler::NewL
        ( 
        CSimAtkNotify* aNotify,
        CStifLogger* aNotifyLog
        )
	{
	CSimAtkNotifyHandler* self =
	    new (ELeave) CSimAtkNotifyHandler( aNotify, aNotifyLog );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;	
	}

// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::~CSimAtkNotifyHandler
// Standart destructor
// -----------------------------------------------------------------------------
//
CSimAtkNotifyHandler::~CSimAtkNotifyHandler()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::Activate
// activates active object if not active
// -----------------------------------------------------------------------------
//
void CSimAtkNotifyHandler::Activate()
	{
	iLog->Log( _L( "CSimAtkNotifyHandler::Activate - Activating! " ) );
	if ( !IsActive() )
		{
		SetActive();
		iLog->Log( _L( "CSimAtkNotifyHandler::Activate \
		    - Active, waiting RunL! " ) );		
		}		
	}
	
// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::SetNotify
// Copies notification IPC to class member
// -----------------------------------------------------------------------------   	
void CSimAtkNotifyHandler::SetNotify
        (
        TSatIpc aNotifyIpc
        )
    {
    iNotifyIpc = aNotifyIpc;
    } 

// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::Status
// returns active object status
// -----------------------------------------------------------------------------
//
TRequestStatus& CSimAtkNotifyHandler::Status()
	{
	return iStatus;	
	}
	
// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::DoCancel
// Cancel active request
// -----------------------------------------------------------------------------
//
void CSimAtkNotifyHandler::DoCancel()
    {
	iLog->Log( _L( "CSimAtkNotifyHandler::DoCancel \
	    - Canceling %d " ), iNotifyIpc );    

    if( IsActive()  && ( iNotify->SimAtkCore() != NULL ) )
        {
        iNotify->SimAtkCore()->NotifyCancel( iNotifyIpc );
        }
    }
      
// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::RunError
// Handles the Cancel
// -----------------------------------------------------------------------------
//
TInt CSimAtkNotifyHandler::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSimAtkNotifyHandler::RunL
// Handles completed event wait
// -----------------------------------------------------------------------------
//
void CSimAtkNotifyHandler::RunL()
    {
    iLog->Log( _L( "CSimAtkNotifyHandler::RunL - Signaling!" ) );
    
    iNotify->Signal( iStatus.Int() );
	}	
	    
//  End of File
