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



// INCLUDE FILES
#include "satflightmodestatus.h"    // sat flight mode status class
#include "satmesshandler.h"         // sat message handler class
#include "satmessaging.h"           // csatmessaging class
#include "tsylogger.h"              // tsy logger

#include <ssm/startupdomainpskeys.h>    // kpsuidstartup & kpsglobalsystemstate
#include <uiccisi.h>                // UICC server
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satflightmodestatusTraces.h"
#endif

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

// FORWARD DECLARATIONS
    //None

// ============================= LOCAL FUNCTIONS ===============================
    //None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatFlightModeStatus* CSatFlightModeStatus::NewL
        (
        CSatMessHandler* aSatMessHandler,
        CTsySatMessaging* aSatMessaging
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_NEWL_TD, "CSatFlightModeStatus::NewL" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::NewL");

    CSatFlightModeStatus* self =
        new( ELeave ) CSatFlightModeStatus(
            aSatMessHandler,
            aSatMessaging );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::CSatFlightModeStatus
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSatFlightModeStatus::CSatFlightModeStatus(
    CSatMessHandler* aSatMessHandler,
    CTsySatMessaging* aSatMessaging )
    :CActive( CActive::EPriorityStandard )
    ,iSatMessHandler( aSatMessHandler )
    ,iSatMessaging( aSatMessaging )
    {
    OstTrace0( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_CSATFLIGHTMODESTATUS_TD, "CSatFlightModeStatus::CSatFlightModeStatus" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::CSatFlightModeStatus");

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::~CSatFlightModeStatus
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CSatFlightModeStatus::~CSatFlightModeStatus()
    {
    OstTrace0( TRACE_NORMAL,  DUP1_CSATFLIGHTMODESTATUS_CSATFLIGHTMODESTATUS_TD, "CSatFlightModeStatus::~CSatFlightModeStatus" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::~CSatFlightModeStatus");

    Cancel();
    // Closes the property.
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::ConstructL
// 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatFlightModeStatus::ConstructL()
    {
    OstTrace0( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_CONSTRUCTL_TD, "CSatFlightModeStatus::ConstructL" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::ConstructL");

    // Initialization
    iFlightModeActive = ETrue;

    // Attaches to the specified property.
    iProperty.Attach( KPSUidStartup, KPSGlobalSystemState );

    // Get system state value of this property.
    // Needed to check is RF status already on, this might happen in slow
    // startup.
    TInt systemStateValue( 0 );
    TInt errValue( iProperty.Get( systemStateValue ) );

    if ( KErrNone == errValue && ESwStateNormalRfOn == systemStateValue )
        {
        TFLOGSTRING("TSY: CSatFlightModeStatus::ConstructL - RF status already RF ON");
        OstTrace0( TRACE_NORMAL,  DUP1_CSATFLIGHTMODESTATUS_CONSTRUCTL_TD, "CSatFlightModeStatus::ConstructL - RF status already RF ON" );

        // Flight mode Off, RF On.
        iFlightModeActive = EFalse;
        }

    // Subscribes to a property.
    iProperty.Subscribe( iStatus );
    // Setting active.
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::DoCancel
// Cancels active object request.
// -----------------------------------------------------------------------------
//
void CSatFlightModeStatus::DoCancel()
    {
    OstTrace0( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_DOCANCEL_TD, "CSatFlightModeStatus::DoCancel" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::DoCancel");

    // Cancels an outstanding subscription request for this property handle.
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::RunL
// Called when operation completes.
// -----------------------------------------------------------------------------
//
void CSatFlightModeStatus::RunL()
    {
    OstTrace0( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_RUNL_TD, "CSatFlightModeStatus::RunL" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::RunL");

    // Check possible error.
    User::LeaveIfError( iStatus.Int() );

    // Continue monitoring with resubscribe before processing new value to
    // prevent missing updates.
    iProperty.Subscribe( iStatus );
    SetActive();

    // Checks Flight mode support values and sends UICC_CAT_REQ message with
    // correct value.
    CheckStatusChange();
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::RunError
// Called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CSatFlightModeStatus::RunError( TInt aError )
    {
    OstTrace1( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_RUNERROR_TD, "CSatFlightModeStatus::RunError %d", aError );
    TFLOGSTRING2("TSY: CSatFlightModeStatus::RunError %d", aError);

    if ( KErrCancel != aError && !IsActive() )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::CheckStatusChange
// Checks Flight mode statuses and sends SimAtkReq with correct value when
// RF mode status state changes.
// -----------------------------------------------------------------------------
//
void CSatFlightModeStatus::CheckStatusChange()
    {
    OstTrace0( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_CHECKSTATUSCHANGE_TD, "CSatFlightModeStatus::CheckStatusChange" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::CheckStatusChange");

    // Gets system state value of this property.
    TInt systemStateValue( 0 );
    TInt errValue( iProperty.Get( systemStateValue ) );

    // Checking error value.
    if ( KErrNone == errValue )
        {
        switch ( systemStateValue )
            {
            case ESwStateNormalRfOn:
                {
                // Flight mode Off, RF On.
                iFlightModeActive = EFalse;
                // Check that (U)SAT client is ready.
                if ( iSatMessHandler->GetSatReadyStatus() )
                    {
                    TFLOGSTRING("TSY: CSatFlightModeStatus::CheckStatusChange - RF ON - ENHANCED FETCH HANDLING ENABLED");
                    OstTrace0( TRACE_NORMAL,  DUP1_CSATFLIGHTMODESTATUS_CHECKSTATUSCHANGE_TD, "CSatFlightModeStatus::CheckStatusChange - RF ON - ENHANCED FETCH HANDLING ENABLED" );

                    // Enable CAT support.
                    iSatMessHandler->UiccCatReq( UICC_CAT_ENABLE );
                    }
                break;
                }
            case ESwStateNormalRfOff:
                {
                // Flight mode On, RF Off.
                iFlightModeActive = ETrue;
                // Check that (U)SAT client is ready.
                if ( iSatMessHandler->GetSatReadyStatus() )
                    {
                    TFLOGSTRING("TSY: CSatFlightModeStatus::CheckStatusChange - RF OFF - ENHANCED FETCH HANDLING DISABLED");
                    OstTrace0( TRACE_NORMAL,  DUP2_CSATFLIGHTMODESTATUS_CHECKSTATUSCHANGE_TD, "CSatFlightModeStatus::CheckStatusChange - RF OFF - ENHANCED FETCH HANDLING DISABLED" );

                    // Disable CAT support.
                    iSatMessHandler->UiccCatReq( UICC_CAT_DISABLE );
                    }
                break;
                }
            default:
                {
                TFLOGSTRING("TSY: CSatFlightModeStatus::CheckStatusChange - Default - state value not interested");
                OstTrace0( TRACE_NORMAL,  DUP3_CSATFLIGHTMODESTATUS_CHECKSTATUSCHANGE_TD, "CSatFlightModeStatus::CheckStatusChange - Default - state value not interested" );

                // we are not interested of other state changes.
                break;
                }
            } // End of switch.
        }
    else
        {
        // There was error when getting system state value. Just continue.
        TFLOGSTRING2("TSY: CSatFlightModeStatus::CheckStatusChange - Error= %d", errValue);
        OstTrace1( TRACE_NORMAL,  DUP4_CSATFLIGHTMODESTATUS_CHECKSTATUSCHANGE_TD, "CSatFlightModeStatus::CheckStatusChange - Error= %d", errValue );
        } // End of if.
    }

// -----------------------------------------------------------------------------
// CSatFlightModeStatus::FlightModeStatusValue
// Flight mode status value, True or False.
// -----------------------------------------------------------------------------
//
TBool CSatFlightModeStatus::FlightModeStatusValue()
    {
    OstTrace0( TRACE_NORMAL,  CSATFLIGHTMODESTATUS_FLIGHTMODESTATUSVALUE_TD, "CSatFlightModeStatus::FlightModeStatusValue" );
    TFLOGSTRING("TSY: CSatFlightModeStatus::FlightModeStatusValue");
    return iFlightModeActive;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
    //None

//  End of File
