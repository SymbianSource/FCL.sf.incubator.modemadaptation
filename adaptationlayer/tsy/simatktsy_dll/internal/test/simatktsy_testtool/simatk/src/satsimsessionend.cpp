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
* Description:   Call Control specific test class. Inherits CSatBase.
*
*/



// INCLUDE FILES
#include <etelsat.h>
#include <satcs.h>
#include "satsimsessionend.h"


const TUint KSessionEndTimeOut = 500000; // 1/2 second

// -----------------------------------------------------------------------------
// CSatSimSessionEnd::CSatSimSessionEnd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatSimSessionEnd::CSatSimSessionEnd( const RSat& aSat ) : iSat( aSat )
    {
    }
// -----------------------------------------------------------------------------
// CSatSimSessionEnd::~CSatSimSessionEnd
// Destructor
// -----------------------------------------------------------------------------
//
CSatSimSessionEnd::~CSatSimSessionEnd()
    {
    }

// -----------------------------------------------------------------------------
// CSatSimSessionEnd::NotifySat
// Send a notify request to Etel
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
void CSatSimSessionEnd::NotifySat()
    {
    if ( iSessionEndStatus != KRequestPending )
        {
        iSessionEndCompleted = EFalse;
        iSat.NotifyProactiveSimSessionEnd( iSessionEndStatus );
        }
    else
        {
        // None
        }
    }    


// -----------------------------------------------------------------------------
// CSatSimSessionEnd::Cancel
// Cancels pending Etel request
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
void CSatSimSessionEnd::Cancel()
    {
    iSessionEndCompleted = ETrue;
    
    if ( iSessionEndStatus == KRequestPending )
        {
        iSat.CancelAsyncRequest( ESatNotifyProactiveSimSessionEnd );
        }
    }
    
// -----------------------------------------------------------------------------
// CSatSimSessionEnd::WaitSessionEnd
// Waits for a SessionEnd notification from a SIM for a while
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatSimSessionEnd::WaitSessionEnd()
    {
    TInt ret( KErrNone );
    
    // In case the request is still pending. Wait for SimSessionEnd for one
    // second.    
    RTimer timeOutTimer;
    TRequestStatus timeOutStatus; 
    timeOutTimer.CreateLocal(); 
    timeOutTimer.After( timeOutStatus, KSessionEndTimeOut );

    User::WaitForRequest( iSessionEndStatus, timeOutStatus );

    if ( iSessionEndStatus == KRequestPending )
        {
        // Timeout, no SessionEnd received in time.
        iSessionEndCompleted = EFalse;
        ret = KErrTimedOut;            
        }
    else
        {
        // SessionEnded successfully.
        iSessionEndCompleted = ETrue;
        
        // Cancel the timer
        timeOutTimer.Cancel();
        User::WaitForRequest( timeOutStatus );
        }

    timeOutTimer.Close();
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatSimSessionEnd::TimedOut
// Return true if SessionEnd was timed out
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TBool CSatSimSessionEnd::TimedOut()
    {
    TBool ret( EFalse );
    
    if ( !iSessionEndCompleted )
        {
        // Request is still pending
        if ( KErrTimedOut == WaitSessionEnd() )
            {
            ret = ETrue;
            }
        else
            {
            ret = EFalse;
            }            
        }
        
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatSimSessionEnd::Status
// Returns the status of SessionEnd notification
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatSimSessionEnd::Status()
    {
    return iSessionEndStatus.Int();
    }

// End of file
