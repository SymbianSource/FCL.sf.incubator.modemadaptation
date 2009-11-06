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
#include <pipeisi.h>    // isi pipe

#include "dpdef.h"      // dataport definitions
#include "dpdataport.h" // dataport main and c32 interface
#include "dppif.h"      // pipe interface (pif)
#include "dpmif.h"      // message inteface (mif)
#include "dplog.h"      // dataport logging
#include <pipe_sharedisi.h>
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dppiftraces.h"
#endif


// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES
// none

// CONSTANTS
// none

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================

// none


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CDpPif::CDpPif
// C++ default constructor.
// ---------------------------------------------------------
CDpPif::CDpPif(
    CDpDataPort& aDataPort ) :
    CActive( KDpPifPriority ),
    iDataPort( aDataPort ),
    iPipeState( EDpPipeDisconnected ),
    iPipeHandle( PN_PIPE_INVALID_HANDLE ),
    iPifState( EDpPifInitial )
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_CDPPIF, "CDpPif::CDpPif" );
    LOGM(" CDpPif::CDpPif");

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CDpPif::~CDpPif
// Destructor
// ---------------------------------------------------------
CDpPif::~CDpPif()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPPIF_CDPPIF, "CDpPif::~CDpPif" );
    LOGM(" CDpPif::~CDpPif");
    }

// ---------------------------------------------------------
// CDpPif::RunL
// PIF state is changed here after registering.
// ---------------------------------------------------------
//
void CDpPif::RunL()
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_RUNL, "CDpPif::RunL" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPPIF_RUNL, "CDpPif:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpPif::RunL - Port %d", iDataPort.PortUnit() );

    // which state in initializing are we
    switch ( iPifState )
        {
        case EDpPifRegister:
            {
            // if pns_name_add failed, there's not much to do,
            // we just wait to get thrown away...
            if ( KErrNone == iStatus.Int() )
                {
                iPifState = EDpPifReady;
                }
            //no else
            break;
            }
        case EDpPifReady:
        case EDpPifInitial:
            {
            break;
            }
        default:
            {
            LOG1(" ERROR, CDpPif::RunL, Unsupported PIFState: %d", iPifState );
            OstTrace1( TRACE_NORMAL, DUP2_CDPPIF_RUNL, "ERROR, CDpPif::RunL, Unsupported PIFState: %d", iPifState );

            User::Leave( KErrGeneral );
            }
        }
    }

// ---------------------------------------------------------
// CDpPif::RunError
// Leave in RunL() is handled here. Error code is returned,
// when internal error has occured (unsupported pipe state).
// ---------------------------------------------------------
//
TInt CDpPif::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_RUNERROR, "CDpPif::RunError" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPPIF_RUNERROR, "CDpPif:: Port: %u, error code: %d", iDataPort.PortUnit(), aError );

    LOGM2("CDpPif::RunError - Port %d, error code: %d",
        iDataPort.PortUnit(), aError );

    return aError;
    }

// ---------------------------------------------------------
// CDpPif::DoCancel
// This method signals PIF with KErrCancel, if PIF is
// registered state.
// ---------------------------------------------------------
//
void CDpPif::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_DOCANCEL, "CDpPif::DoCancel" );
    LOGM(" CDpPif::DoCancel");

    if ( IsActive() )
        {
        iDataPort.SignalPif( KErrCancel );
        }
    }

// ---------------------------------------------------------
// CDpPif::InitiatePipe
// This method registers DataPort to Name Service.
// ---------------------------------------------------------
//
TInt CDpPif::InitiatePipe(
    const TUint aUnit )
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_INITIATEPIPE, "CDpPif::InitiatePipe" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPPIF_INITIATEPIPE, "CDpPif:: Port: %u", aUnit );

    LOGM1(" CDpPif::InitiatePipe - Port %d", aUnit );

    TInt ret( KErrNone );

    if ( !iPepRegistered )
        {
        // pep_register

        TRAP( ret, iDataPort.Mif().SendNameAddReqL( aUnit ) );

        if ( KErrNone == ret )
            {
            // set active
            if ( !IsActive() )
                {
                iStatus = KRequestPending;
                SetActive();
                }
            //no else

            iPepRegistered = ETrue;
            // state_change
            iPifState = EDpPifRegister;
            }
        //no else
        }
    else
        {
        // registered pipe already exists
        ret = KErrAlreadyExists;
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpPif::ClosePipeL
// This method unregisters DataPort from Name Service.
// ---------------------------------------------------------
//
void CDpPif::ClosePipeL( const TUint aUnit )
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_CLOSEPIPEL, "CDpPif::ClosePipeL" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPPIF_CLOSEPIPEL, "CDpPif:: Port: %u", aUnit );

    LOGM1(" CDpPif::ClosePipeL - Port %d", aUnit );

    iPepRegistered = EFalse;

    iDataPort.Mif().SendNameRemoveReqL( aUnit );

    // state_change
    iPifState = EDpPifInitial;
    }

// -----------------------------------------------------------------------------
// CDpPif::Attach
// Attach observer to CDpSubject.
// -----------------------------------------------------------------------------
//
void CDpPif::Attach( CDpObserver& aObserver )
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_ATTACH, "CDpPif::Attach" );
    iDpSubject.Attach( aObserver );
    }

// -----------------------------------------------------------------------------
// CDpPif::Detach
// Detach observer from CDpSubject.
// -----------------------------------------------------------------------------
//
void CDpPif::Detach( CDpObserver& aObserver )
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_DETACH, "CDpPif::Detach" );
    iDpSubject.Detach( aObserver );
    }

// -----------------------------------------------------------------------------
// CDpPif::Notify
// Notify all observers of CDpSubject object.
// -----------------------------------------------------------------------------
//
void CDpPif::Notify()
    {
    OstTrace0( TRACE_NORMAL, CDPPIF_NOTIFY, "CDpPif::Notify" );
    iDpSubject.Notify();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
// none

//  End of File
