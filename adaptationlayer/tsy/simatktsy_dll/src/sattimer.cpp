/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES
#include "sattimer.h"           // sat timer class
#include "satmessaging.h"       // sat messaging class
#include "satmesshandler.h"     // sat message handler class
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "sattimertraces.h"
#endif


// CONSTANTS
const TInt KTimersEqual                 = 0;
const TInt KFirstBigger                 = 1;
const TInt KFirstSmaller                = -1;
const TInt KTick                        = 500000; // 500 milliseconds
const TInt KMaxNumberOfStoredEnvelopes  = 10;
const TInt KHiByteMask                  = 0xF0;
const TInt KTimerIdPosition             = 8;
const TInt KDefaultDelay                = 1; // Envelope resending delay


// ==================== MEMBER FUNCTIONS =======================================
// -----------------------------------------------------------------------------
// CSatTimer::CSatTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatTimer::CSatTimer
        (
        CSatMessHandler* aSatMessHandler,
        CTsySatMessaging* aSatMessaging
        )
        :
        iSatMessHandler( aSatMessHandler ),
        iSatMessaging( aSatMessaging ),
        iSecondsPassed( 0 ),
        iHalfSecondTick( EFalse ),
        iProactiveCommandOnGoing( EFalse ),
        iTimer( NULL ),
        iTimerTable( NULL ),
        iEnvelopeTable( NULL )

    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_CSATTIMER, "CSatTimer::CSatTimer" );
    }


// -----------------------------------------------------------------------------
// CSatTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatTimer::ConstructL()
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_CONSTRUCTL, "CSatTimer::ConstructL" );
    TFLOGSTRING("TSY: CSatTimer::ConstructL");

    iTimerTable = new ( ELeave ) RArray<TTimer>( 8 );
    iEnvelopeTable = new ( ELeave ) RArray<TSatEnvelope>( 1 );

     // Neutral priority, 0
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }


// -----------------------------------------------------------------------------
// CSatTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatTimer* CSatTimer::NewL
        (
        CSatMessHandler*    aSatMessHandler,
        CTsySatMessaging*   aSatMessaging
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_NEWL, "CSatTimer::NewL" );
    TFLOGSTRING("TSY: CSatTimer::NewL");

    CSatTimer* self = new( ELeave ) CSatTimer( aSatMessHandler, aSatMessaging );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CSatTimer::~CSatTimer
// Destructor
// -----------------------------------------------------------------------------
//
CSatTimer::~CSatTimer()
    {
    OstTrace0( TRACE_DETAILED, DUP1_CSATTIMER_CSATTIMER, "CSatTimer::~CSatTimer" );
    TFLOGSTRING("TSY: CSatTimer::~CSatTimer");

    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }

    if ( iTimerTable )
        {
        iTimerTable->Close();
        delete iTimerTable;
        }

    if ( iEnvelopeTable )
        {
        iEnvelopeTable->Close();
        delete iEnvelopeTable;
        }
    }


// -----------------------------------------------------------------------------
// CSatTimer::StartTimer
// Starts a timer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatTimer::Start()
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_START, "CSatTimer::Start" );
    //Check if timer is active just in case
    if ( !iTimer->IsActive() )
        {
        TFLOGSTRING("TSY: CSatTimer::Start: Timer started");
        OstTrace0( TRACE_DETAILED, DUP1_CSATTIMER_START, "CSatTimer::Start: Timer started" );
        //Timer will tick every half second
        TCallBack callback( Tick, this );
        iTimer->Start( KTick, KTick, callback );
        }
    }

// -----------------------------------------------------------------------------
// CSatTimer::StartTimer
// Starts a timer with specified id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::StartTimer
        (
        TInt aTimerId,
        TUint32 aTimerValue
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_STARTTIMER, "CSatTimer::StartTimer" );
    TFLOGSTRING("TSY: CSatTimer::StartTimer");
    TInt ret( KErrNone );
    // Check if the entry is already in the table and delete if found.
    if ( CheckTimerTable( aTimerId ) )
        {
        DeleteTimerById( aTimerId );
        }

    // Calculate the trigger time (actual beat amount + timeout)
    TUint32 timeStamp = iSecondsPassed + aTimerValue;

    // Create timer
    TTimer timer( aTimerId, iSecondsPassed, timeStamp );

    // Insert the entry in the table and start timer
    ret = iTimerTable->InsertInOrder( timer, TTimer::OrderEntries );

    // Start timer
    if ( KErrNone == ret )
        {
        Start();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::RestartTimer
// Restarts the inactive timer with specified id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::RestartTimer
        (
        TInt aTimerId
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_RESTARTTIMER, "CSatTimer::RestartTimer" );
    TFLOGSTRING("TSY: CSatTimer::RestartTimer");
    TInt ret( KErrNone );

    // Find timer from TimerTable
    TTimer helper( aTimerId, 0, 0 );
    TInt index = iTimerTable->Find( helper, TTimer::CompareEntries );

    if ( KErrNotFound != index )
        {
        // Reactivate timer if proactive command is not ongoing, otherwise
        // set state to witing for terminal response
        if ( iProactiveCommandOnGoing )
            {
            ( *iTimerTable )[index].SetState( TTimer::EWaitingForTr );
            }
        else
            {
            ( *iTimerTable )[index].SetState( TTimer::ETicking );
            }

        // Start clock
        Start();
         }
    else
        {
        TFLOGSTRING2("TSY: CSatTimer::ReStart - Requested ID (%d) not found!",
            aTimerId);
        OstTrace1( TRACE_DETAILED, DUP1_CSATTIMER_RESTARTTIMER, "CSatTimer::RestartTimer - Requested ID (%d) not found!", aTimerId );

        ret = KErrNotFound;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatTimer::Stop
// Stops the timer counting
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatTimer::Stop()
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_STOP, "CSatTimer::Stop" );
    // If both timer and envelope arrays are empty, stop the timer.
    // Also if timer table is empty, clear the second counter.
    if ( iTimer->IsActive () )
        {
        if ( !iEnvelopeTable->Count()
          && !iTimerTable->Count() )
            {
            TFLOGSTRING("TSY: CSatTimer::Stop: Timer stopped");
            OstTrace0( TRACE_DETAILED, DUP1_CSATTIMER_STOP, "CSatTimer::Stop: Timer stopped" );
            iTimer->Cancel();
            }
        if ( !iTimerTable->Count() )
            {
            TFLOGSTRING("TSY: CSatTimer::Stop: Second counters cleared");
            OstTrace0( TRACE_DETAILED, DUP2_CSATTIMER_STOP, "CSatTimer::Stop: Second counters cleared" );
            iSecondsPassed = 0;
            iHalfSecondTick = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatTimer::Tick
// Static callback method for CPeriodic timer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::Tick
        (
        TAny* aSatTimer
        )
    {
    // Logging omitted to avoid excessive log output

    // This is a static method, we need a pointer to parent class in
    // order to utilize it's method and members
    CSatTimer* satTimer = static_cast<CSatTimer*>( aSatTimer );

    // Invert boolean value of half second boolean
    satTimer->iHalfSecondTick = !satTimer->iHalfSecondTick;

    // Update second counter and check timers only when there are active
    // timers and whole second has passed since last update.
    TInt tableSize( satTimer->iTimerTable->Count() );
    if ( KZero != tableSize && !satTimer->iHalfSecondTick )
        {
        satTimer->iSecondsPassed++;

        TFLOGSTRING2("TSY: CSatTimer::Tick, seconds %d",
            satTimer->iSecondsPassed );
        OstTrace1( TRACE_DETAILED, CSATTIMER_TICK, "CSatTimer::Tick, seconds %u", satTimer->iSecondsPassed );

        for( TInt i = tableSize - 1; KZero <= i; i-- )
            {
            // Get reference to a timer in TimerTable
            TTimer& timer = ( *satTimer->iTimerTable )[i];

            // Send timer expiration envelope if:
            // 1) Enough time has passed
            // 2) Timer is active
            // 3) Proactive command is not ongoing
            if ( timer.TimeStamp() <= satTimer->iSecondsPassed
                 && TTimer::ETicking == timer.State() )
                {
                // Deactive timer to avoid multiple envelopes
                timer.SetState( TTimer::EBeingSent);

                // Elapsed time
                TInt time( satTimer->iSecondsPassed - timer.StartTime() );

                satTimer->iSatMessaging->TimerExpiration( timer.Id(), time );
                }
            }
        }


    // Handle the stored envelopes. Look for envelopes activated for resending.
    // Resend the oldest envelope if delay criteria has been met.
    tableSize = satTimer->iEnvelopeTable->Count();
    TBool done( EFalse );

    if ( KZero != tableSize )
        {
        for ( TInt i = 0 ; i < tableSize && done == EFalse ; i++ )
            {
            TSatEnvelope& current = ( *satTimer->iEnvelopeTable )[i];

            if ( current.iActive )
                {
                // Active envelope found. Decrease delay and send envelope
                // if zero.
                current.iDelay--;

                if ( KZero >= current.iDelay )
                    {
                    TFLOGSTRING2("TSY: CSatTimer::Tick: \
                        Resending type %x envelope", current.iType );
                    OstTraceExt1( TRACE_DETAILED, DUP1_CSATTIMER_TICK, "CSatTimer::Tick Resending type %hhu envelope", current.iType );

                    // Deactivate envelope just in case. Get new transaction
                    // if for envelope and resend it. Finally remove the
                    // envelope from table.
                    current.iActive = EFalse;

                    satTimer->iSatMessHandler->UiccCatReqEnvelope(
                        current.iTId, current.iEnvelope, EFalse );

                    satTimer->RemoveEnvelope( current.iTId );
                    tableSize = satTimer->iEnvelopeTable->Count();

                    // Exit resending loop, only one resent envelope per tick
                    done = ETrue;
                    }
                } // If active
            } // For-loop
        } // Table size not zero

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatTimer::CheckTimerTable
// Returns true if the timer exists in the timer table.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSatTimer::CheckTimerTable
        (
        TInt aTimerId
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_CHECKTIMERTABLE, "CSatTimer::CheckTimerTable" );
    TFLOGSTRING("TSY: CSatTimer::CheckTimerTable");

    TBool ret( EFalse );

    // Create the helper entry with meaningful values only
    TTimer timer( aTimerId, 0, 0 );

    // Find the entry for this Handle in the list
    TInt position = iTimerTable->Find( timer, TTimer::CompareEntries );
    if ( KErrNotFound != position )
        {
        ret = ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::DeleteTimerById
// Deletes timer from the table
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::DeleteTimerById
        (
        TInt aTimerId // Timer identifier
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_DELETETIMERBYID, "CSatTimer::DeleteTimerById" );
    TFLOGSTRING("TSY: CSatTimer::DeleteTimerById");

    TInt ret( KErrNotFound );

    // Create the entry with meaningful values only
    TTimer timer( aTimerId, 0, 0 );

    // Find the entry for this Handle in the list
    TInt position = iTimerTable->Find (timer, TTimer::CompareEntries );

    // Check if element is found
    if ( KErrNotFound != position )
        {
        iTimerTable->Remove( position );
        iTimerTable->Compress();
        // Check if empty
        if ( KZero == iTimerTable->Count())
            {
            // Request stopping of timer
            Stop();
            }
        ret = KErrNone;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::GetCurrentValueOfTimerById
// Gets current value of timer by timer id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CSatTimer::GetCurrentValueOfTimerById
        (
        TInt aTimerId // Timer identifier
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_GETCURRENTVALUEOFTIMERBYID, "CSatTimer::GetCurrentValueOfTimerById" );
    TFLOGSTRING("TSY: CSatTimer::GetCurrentValueOfTimerById");

    TInt ret( KErrNotFound );

    // Create the entry with meaningful values only
    TTimer timer( aTimerId, 0, 0 );

    // Find the entry for this Handle in the list
    TInt position = iTimerTable->Find( timer, TTimer::CompareEntries );

    // Check if element is found
    if ( KErrNotFound != position )
        {
        // Get the reference to the a timer in timer table
        TTimer& timer = ( *iTimerTable )[ position ];
        // Get timeStamp
        TUint32 timeStamp = timer.TimeStamp();
        // Current timer value = timer timeout value - current time
        ret = timeStamp - iSecondsPassed;
        // If current timer value is smaller than zero set value to 0
        // this can happen if there is some kind of delay before
        // timer is expired and iSecondsPassed is still counting...
        if( KZero > ret )
            {
            ret = 0;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::StoreEnvelope
// Stores envelope for possible resending
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::StoreEnvelope
        (
        const TUint8 aTId,
        const TDesC8& aEnvelope
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_STOREENVELOPE, "CSatTimer::StoreEnvelope" );
    TFLOGSTRING("TSY: CSatTimer::StoreEnvelope");

    TInt ret( KErrNone );

    // Add envelope to array only if it is not already there,
    // envelope has proper length a envelope data begins with
    // envelope tag (D1..D8)
    if ( KErrNotFound == LocateEnvelope( aTId )
      && KZero < aEnvelope.Length() )
        {
        if ( ( aEnvelope[KZero] & KHiByteMask ) == ETypeUnknown )
            {
            // Backup system. Oldest envelope is deleted if too
            // many envelopes exist in the queue
            if ( KMaxNumberOfStoredEnvelopes < iEnvelopeTable->Count() )
                {
                iEnvelopeTable->Remove( 0 );
                iEnvelopeTable->Compress();
                }

            TSatEnvelope envelope;

            envelope.iTId = aTId;
            envelope.iDelay = KDefaultDelay;
            envelope.iType = aEnvelope[KZero]; // Type is at index 0
            envelope.iActive = EFalse;
            envelope.iEnvelope.Copy( aEnvelope );

            iEnvelopeTable->Append( envelope );

            TFLOGSTRING2("CSatTimer::StoreEnvelope \
                Stored envelopes: % d", iEnvelopeTable->Count() );
            OstTrace1( TRACE_DETAILED, DUP1_CSATTIMER_STOREENVELOPE, "CSatTimer::StoreEnvelope Stored envelopes: %d", iEnvelopeTable->Count() );

            }
        else
            {
            TFLOGSTRING2("CSatTimer::StoreEnvelope \
                Storing failed, not an envelope! %x", aEnvelope[KZero] );
            OstTraceExt1( TRACE_DETAILED, DUP2_CSATTIMER_STOREENVELOPE, "CSatTimer::StoreEnvelope Storing failed, not an envelope! %s", aEnvelope[KZero] );
            ret = KErrArgument;
            }

        }
    else
        {
        TFLOGSTRING2(" CSatTimer::StoreEnvelope \
            Envelope with requested ID %d already exist!", aTId );
        OstTraceExt1( TRACE_DETAILED, DUP3_CSATTIMER_STOREENVELOPE, "CSatTimer::StoreEnvelope Envelope with requested ID %hhd already exists!", aTId );
        ret = KErrAlreadyExists;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatTimer::ActivateEnvelopeResend
// Activates the resending procedure of stored envelope
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::ActivateEnvelopeResend
        (
        const TUint8 aTId,
        const TUint8 aCause
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_ACTIVATEENVELOPERESEND, "CSatTimer::ActivateEnvelopeResend" );
    TFLOGSTRING("TSY: CSatTimer::ActivateEnvelopeResend");

    TInt ret( LocateEnvelope( aTId ) );

    if ( KErrNotFound != ret )
        {
        TSatEnvelope& current = ( *iEnvelopeTable )[ret];

        switch ( aCause )
            {
            case ESimBusy:
                {
                if ( ETimerExpiration == current.iType
                  && KTimerIdPosition < current.iEnvelope.Length() )
                    {
                    // Resending of Timer envelope is different from others,
                    // because timer value has to be updated within the resent
                    // envelope. As a result, we can't just resend old message.

                    // Restart timer using ID.
                    RestartTimer( current.iEnvelope[KTimerIdPosition] );

                    // Old stored envelope has to be removed from envelope,
                    // but not from timer table in case of timer reactivation.
                    RemoveEnvelope( aTId, EFalse );
                    }
                else
                    {
                    // For other envelope, just activate it
                    current.iActive = ETrue;
                    }

                ret = KErrNone;
                break;
                }
            case ERefManagement:
                {
                // Resending of envelope when SIM return Reference Management
                // error, and active envelope is Call Control, is based on
                // error item( No call after autolock). Because need for send 
                // is unclear (in other Nokia devices does it),
                // only CC envelope are resent.
                if ( ECallControl == current.iType )
                    {
                    current.iActive = ETrue;
                    ret = KErrNone;
                    }
                else
                    {
                    ret = KErrNotSupported;
                    }
                break;
                }
            default:
                {
                // Unexpected cause
                ret = KErrNotSupported;
                }
            }

        if ( KErrNone == ret )
            {
            TFLOGSTRING("TSY: CSatTimer::ActivateEnvelopeResend \
                Resending activated");
            OstTrace0( TRACE_DETAILED, DUP1_CSATTIMER_ACTIVATEENVELOPERESEND, "CSatTimer::ActivateEnvelopeResend Resending activated" );
            // (Re)start the timer if needed
            Start();
            }
        else
            {
            TFLOGSTRING2("TSY: CSatTimer::ActivateEnvelopeResend \
                Resending NOT activated. Cause: %x", aCause );
            OstTraceExt1( TRACE_DETAILED, DUP2_CSATTIMER_ACTIVATEENVELOPERESEND, "CSatTimer::ActivateEnvelopeResend Resending NOT activated. Cause: %hhu", aCause );
            RemoveEnvelope( aTId );
            }
        }

    return ret;
    }



// -----------------------------------------------------------------------------
// CSatTimer::RemoveEnvelope
// Removes the envelope from the table by given id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::RemoveEnvelope
        (
        const TUint8 aTId,
        TBool aRemoveTimer
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_REMOVEENVELOPE, "CSatTimer::RemoveEnvelope" );
    TFLOGSTRING("TSY: CSatTimer::RemoveEnvelope");

    TInt ret( LocateEnvelope( aTId ) );

    if ( KErrNotFound != ret )
        {
        TSatEnvelope& current = (*iEnvelopeTable)[ret];

        // The timer corresponding the envelope must be removed from timer
        // table if timer expiration envelope was sent successfully.
        if ( ETimerExpiration == current.iType && aRemoveTimer )
            {
            DeleteTimerById( current.iEnvelope[KTimerIdPosition] );
            }

        iEnvelopeTable->Remove( ret );
        iEnvelopeTable->Compress();

        // Stop-method halts the timer only if there are no envelopes or
        // timer expiration left
        Stop();

        ret = KErrNone;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatTimer::LocateEnvelope
// Locates the position of envelope in table by given id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::LocateEnvelope
        (
        const TUint8 aTId
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_LOCATEENVELOPE, "CSatTimer::LocateEnvelope" );
    TFLOGSTRING("TSY: CSatTimer::LocateEnvelope");

    TInt ret( KErrNotFound );
    TInt numEnvelopes( iEnvelopeTable->Count() );

    for ( TInt i = 0 ; i < numEnvelopes && ret == KErrNotFound ; i++ )
        {
        if ( ( *iEnvelopeTable )[i].iTId == aTId )
            {
            TFLOGSTRING2("TSY: CSatTimer::LocateEnvelope \
                Requested envelope was found at index %d", i );
            OstTrace1( TRACE_DETAILED, DUP1_CSATTIMER_LOCATEENVELOPE, "CSatTimer::LocateEnvelope Requested envelope was found at index %d", i );
            ret = i;
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatTimer::SetProactiveCommandOnGoingStatus
// Sets status for ProactiveCommandOnGoing flag
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatTimer::SetProactiveCommandOnGoingStatus
        (
        TBool aStatus
        )
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_SETPROACTIVECOMMANDONGOINGSTATUS, "CSatTimer::SetProactiveCommandOnGoingStatus" );
    iProactiveCommandOnGoing = aStatus;

    TInt tableSize( iTimerTable->Count() );

    if ( !iProactiveCommandOnGoing && tableSize )
        {
        // Some of the timers has been deactivated because of ongoing pcmd
        for ( TInt i( 0 ) ; i < tableSize ; i++ )
            {
            // Get reference to a timer in TimerTable
            TTimer& timer = ( *iTimerTable )[i];

            if ( TTimer::EWaitingForTr == timer.State() )
                {
                timer.SetState( TTimer::ETicking );
                Start(); // Just in case
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CSatTimer::ClearTimerTable
// Clears the timer table
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatTimer::ClearTimerTable()
    {
    OstTrace0( TRACE_DETAILED, CSATTIMER_CLEARTIMERTABLE, "CSatTimer::ClearTimerTable" );

    if ( KZero != iTimerTable->Count() )
        {
        // Clear the array
        iTimerTable->Reset();
        iTimerTable->Compress();

        // Request timer stop till new entry is done
        Stop();
        }
    }


// -----------------------------------------------------------------------------
// CSatTimer::TTimer::TTimer
// Non default Constructor, for nested class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatTimer::TTimer::TTimer
        (
        TInt aTimerId,       // Timer id
        TUint32 aStartTime,  // Timer start time
        TUint32 aTimeStamp   // Time stamp
        )
        :
        iTimerId( aTimerId ),
        iStartTime( aStartTime ),
        iTimeStamp( aTimeStamp ),
        iState( ETicking )
    {
    }


// -----------------------------------------------------------------------------
// CSatTimer::TTimer::TimeStamp
// returns iTimeStamp from a TTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CSatTimer::TTimer::TimeStamp()
    {
    return iTimeStamp;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TTimer::Id
// returns iTimerId from a TTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::TTimer::Id()
    {
    return iTimerId;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TTimer::StartTime
// returns iStartTime from a TTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CSatTimer::TTimer::StartTime()
    {
    return iStartTime;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TTimer::SetState
// Setter for timer's internal state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSatTimer::TTimer::SetState
        (
        TState aState
        )
    {
    iState = aState;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TTimer::IsActive
// Getter for timer's internal state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatTimer::TTimer::TState CSatTimer::TTimer::State()
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TTimer::CompareEntries
// Returns if two entries are equal
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSatTimer::TTimer::CompareEntries
        (
        const CSatTimer::TTimer& arg1, // First entry
        const CSatTimer::TTimer& arg2  // Second entry
        )
    {
    TBool ret( EFalse );

    // We are interested only in the timer id
    if ( arg1.iTimerId == arg2.iTimerId )
        {
        ret =  ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TTimer::OrderEntries
// Returns order between two entries
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatTimer::TTimer::OrderEntries
        (
        const CSatTimer::TTimer& arg1, // First entry
        const CSatTimer::TTimer& arg2  // Second entry
        )
    {
    TInt ret( KTimersEqual );

    // We are interested only in the timer id
    if ( arg1.iTimerId < arg2.iTimerId )
        {
        ret = KFirstSmaller;
        }
    else if( arg1.iTimerId > arg2.iTimerId )
        {
        ret = KFirstBigger;
        }
    else
        {
        // Timer Id's are equal. Return value already properly initialized.
        }
    return ret;

  }

// End of file
