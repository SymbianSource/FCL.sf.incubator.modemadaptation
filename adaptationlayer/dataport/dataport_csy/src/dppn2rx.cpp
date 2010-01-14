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
#include "dpdef.h"           // dataport definitions
#include "dpdataport.h"      // dataport main and c32 interface
#include "dppn2rx.h"         // moves data from isc api to rx buffer
#include "dppif.h"           // pipe interface (pif)
#include "dpflowctrl.h"      // flow control handling
#include "dpdataconfig.h"    // configuration store
#include "dpparityhandler.h" // parity bit handling in 7-bit data mode
#include "dplog.h"           // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dppn2rxTraces.h"
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

// -----------------------------------------------------------------------------
// CDpPn2Rx::CDpPn2Rx
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDpPn2Rx::CDpPn2Rx(
    CDpDataPort& aDataPort ) :
    CActive( KDpPn2RxPriority ),
    iDataPort( aDataPort ),
    iBufferRx( iDataPort.Rx() ),
    iPifDcs( iDataPort.Pif() ),
    iDataConfig( iDataPort.DataConfig() ),
    iFlowCtrl( iDataPort.FlowCtrl() ),
    iParityHandler( iDataPort.ParityHandler() ),
    iRx( 0, 0 ),
    iState( EElementWait ),
    iInBufHasDataNotify( EFalse ),
    iRequestActive( EFalse ),
    iRole( MDpDataClient::EDbDataClientWriter )
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_CDPPN2RX, "CDpPn2Rx::CDpPn2Rx" );
    LOGM(" CDpPn2Rx::CDpPn2Rx");
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_CONSTRUCTL, "CDpPn2Rx::ConstructL" );
    LOGM("CDpPn2Rx::ConstructL");

    //  initialise inner observer object
    iDpPn2RxObserver.Init( this );
    // register to PIF as observer
    iPifDcs.Attach( iDpPn2RxObserver );
    // If registering fails => can't do nothing here => must leave
    User::LeaveIfError( iBufferRx.RegisterDataClient( *this ) );

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDpPn2Rx* CDpPn2Rx::NewL(
    CDpDataPort& aDataPort )
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_NEWL, "CDpPn2Rx::NewL" );
    LOGM("CDpPn2Rx::NewL");

    CDpPn2Rx* self = new (ELeave) CDpPn2Rx( aDataPort );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::~CDpPn2Rx
// Destructor
// -----------------------------------------------------------------------------
//
CDpPn2Rx::~CDpPn2Rx()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPPN2RX_CDPPN2RX, "CDpPn2Rx::~CDpPn2Rx" );
    LOGM("CDpPn2Rx::~CDpPn2Rx");
    // unregister from PIF as observer
    iPifDcs.Detach( iDpPn2RxObserver );
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::RunL
// This active object is signalled by Isc Api.
// We will renew read to Isc Api.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::RunL()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_RUNL, "CDpPn2Rx::RunL" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPPN2RX_RUNL, "CDpPn2Rx:: Port: %u, Status: %d", iDataPort.PortUnit(), iStatus.Int() );

    LOGM2("CDpPn2Rx::RunL - Port %d, Status: %d ",
        iDataPort.PortUnit(), iStatus.Int() );

    iRequestActive = EFalse;
    User::LeaveIfError( Read() );

    // don't set iStatus nor set_active here as it is better done in functions
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::RunError
// Leave in RunL() is handled here. Error code is returned,
// when internal error has occured.
// -----------------------------------------------------------------------------
//
TInt CDpPn2Rx::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_RUNERROR, "CDpPn2Rx::RunError" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPPN2RX_RUNERROR, "CDpPn2Rx:: Port: %u, error code: %d", iDataPort.PortUnit(), aError );

    LOGM2("CDpPn2Rx::RunError - Port %d, error code: %d",
        iDataPort.PortUnit(), aError );

    return aError;
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::DoCancel
// Cancels this active object and cancels pipe receive.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_DOCANCEL, "CDpPn2Rx::DoCancel" );
    LOGM("CDpPn2Rx::DoCancel");

    switch ( iState )
        {
        case EDpReadWait:
            {
            if ( iRequestActive )
                {
                // cancel pipe
                iDataPort.ISAHandle().DataReceiveCancel();

                LOG("  CDpPn2Rx::DoCancel, Rx element release");
                OstTrace0( TRACE_NORMAL, DUP1_CDPPN2RX_DOCANCEL, "CDpPn2Rx:: Rx element release" );

                TInt ret = iBufferRx.WriteElement().Release( iRx.Length() );

                if ( KErrNone == ret )
                    {
#ifdef USE_FILE_SERIAL_TRUST_DEBUG
                    iDataPort.DebugBytesFromPn( iRx.Length() );
#endif
                    if ( 0 < iRx.Length() )
                        {
                        // 7-bit data extract parity bits
                        if ( EData7 == iDataConfig.DataBits() )
                            {
                            iParityHandler.ExtractParityBits( iRx );
                            }
                        //no else

                        if ( iDataPort.IsRx2DteReadPending() )
                            {
                            // Signal RX2DTE
                            iDataPort.SignalRx2Dte();
                            }
                        //no else

                        // New data is available. is client interested about that
                        if ( iInBufHasDataNotify )
                            {
                            // We should notify client
                            iInBufHasDataNotify = EFalse;
                            iDataPort.NotifyDataAvailableCompleted( KErrNone );
                            }
                        //no else
                        }
                    //no else
                    }
                else
                    {
                    // Element release couldn't be done. Can't leave here.
                    // Element remains reserved and something has gone totally
                    // wrong before release.
                    LOG("  ERROR, CDpPn2Rx::DoCancel, Element release failed");
                    OstTrace0( TRACE_NORMAL, DUP2_CDPPN2RX_DOCANCEL, "CDpPn2Rx:: Element release failed" );
                    }
                //no else

                // reset the state
                iState = EDefault;
                iRequestActive = EFalse;
                }
            //no else
            break;
            }
        case EElementWait:
            {
            // Do not complete request if pipe is closed
            if ( CDpPif::EDpPipeDisabled != iPifDcs.PipeState() &&
                CDpPif::EDpPipeDisconnected != iPifDcs.PipeState() )
                {
                // do the required signaling
                if ( iStatus == KRequestPending )
                    {
                    if ( iRequestActive )
                        {
                        // do the required signaling
                        iRequestActive = EFalse;
                        iState = EDefault;
                        TRequestStatus* s = &iStatus;
                        User::RequestComplete( s, KErrCancel );
                        }
                    //no else
                    }
                //no else
                }
            //no else
            break;
            }
        case EDefault:
            {
            break;
            }
        default:
            {
            LOG("  ERROR, CDpPn2Rx::DoCancel, Unknown iState");
            OstTrace0( TRACE_NORMAL, DUP3_CDPPN2RX_DOCANCEL, "ERROR, CDpPn2Rx::DoCancel, Unknown iState" );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::Read
// Reserves space from rx buffer and makes new PipeReceive
// to Isc Api. Checks if flow control high water mark is reached.
// Extracts paritybits if 7-bit data.Signals Rx2Dte.
// -----------------------------------------------------------------------------
//
TInt CDpPn2Rx::Read()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_READ, "CDpPn2Rx::Read" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPPN2RX_READ, "CDpPn2Rx::Read %x", &iDataPort );
    OstTraceExt2( TRACE_NORMAL, DUP2_CDPPN2RX_READ, "CDpPn2Rx:: (Read from ISC) - Port: %u, iRx.Length: %d", iDataPort.PortUnit(), iRx.Length() );

    LOGM1("CDpPn2Rx::Read %x", &iDataPort);
    LOGM2("CDpPn2Rx::Read (Read from ISC) - Port %d, iRx.Length: %d",
     iDataPort.PortUnit(), iRx.Length() );

    TInt ret( KErrNone );

    // We've been listening pn_pipe
    if ( EDpReadWait == iState )
        {
        LOGHEX(iRx, EFalse);
        OstTraceExt1( TRACE_NORMAL, DUP10_CDPPN2RX_READ, "CDpPn2Rx:: RX = %s", iRx );

        ret = iBufferRx.WriteElement().Release( iRx.Length() );

        if ( KErrNone == ret )
            {
#ifdef USE_FILE_SERIAL_TRUST_DEBUG
            iDataPort.DebugBytesFromPn( iRx.Length() );
#endif
            if ( 0 < iRx.Length() )
                {
                // 7-bit data extract parity bits
                if ( EData7 == iDataConfig.DataBits() )
                    {
                    iParityHandler.ExtractParityBits( iRx );
                    }
                //no else

                if ( iDataPort.IsRx2DteReadPending() )
                    {
                    // Signal RX2DTE
                    iDataPort.SignalRx2Dte();
                    }
                //no else

                // New data is available. is client interested about that
                if ( iInBufHasDataNotify )
                    {
                    // We should notify client
                    iInBufHasDataNotify = EFalse;
                    iDataPort.NotifyDataAvailableCompleted( KErrNone );
                    }
                //no else
                }
            //no else
            }
        //no else
        }
    //no else

    // Check pipe state
    if ( KErrNone == ret && CDpPif::EDpPipeEnabled != iPifDcs.PipeState() )
        {
        ret = KErrNotReady;
        }
    else if ( KErrNone == ret )
        {
        // Try reserve element for writing
        TPtr8 nullPtr( 0, 0 );

        // Check if reserved buffer was too small
        // => override MaxReservationSize for next read request.
        if ( KErrNoMemory == iStatus.Int() &&
            iLen > iBufferRx.MaxReservationSize() )
            {
            OstTrace0( TRACE_NORMAL, DUP4_CDPPN2RX_READ, "CDpPn2Rx::KErrNoMemory from ISC =>" );
            OstTraceExt1( TRACE_NORMAL, DUP5_CDPPN2RX_READ, "CDpPn2Rx:: -- Try to read more than MaxReservationSize, %hu --", iLen );

            LOG("  CDpPn2Rx::Read, KErrNoMemory from ISC =>");
            LOG1("  -- Try to read more than MaxReservationSize, %d --", iLen );

            ret = iBufferRx.WriteElement().Reserve(
                iLen, iRx, nullPtr, ETrue );
            // Set iLen to zero (required buffer lenght for read,
            // received from ISC if buffer was too small)
            iLen = 0;
            }
        else
            {
            ret = iBufferRx.WriteElement().Reserve(
                iBufferRx.MaxReservationSize(), iRx, nullPtr );
            }

        // Is flowctrl off and rx "high watermark" reached
        if ( EFlowControlOff == iFlowCtrl.FlowCtrlDcs2Dp() )
            {
            if ( IsHighWaterMarkReached() )
                {
                // Buffer is almost full set flowcontrol on
                LOG("  CDpPn2Rx::Read, Rx High WaterMarkReached");
                OstTrace0( TRACE_NORMAL, DUP6_CDPPN2RX_READ, "CDpPn2Rx:: Rx High WaterMark reached" );

                iFlowCtrl.WaterMarkHighReached();
                }
            //no else
            }
        //no else

        switch ( ret )
            {
            // Reservation successed
            case KErrNone:
                {
                LOG1("  CDpPn2Rx::Read, ISAHandle Pipe Receive. iRx.MaxLength: %d",
                    iRx.MaxLength() );
                OstTrace1( TRACE_NORMAL, DUP7_CDPPN2RX_READ, "CDpPn2Rx:: ISAHandle Pipe Receive. iRx.MaxLength: %d", iRx.MaxLength() );

                // Set pipe receive
                if ( !IsActive() )
                    {
                    iDataPort.ISAHandle().DataReceive( iStatus, iRx, iLen );

                    iRequestActive = ETrue;
                    // Change state
                    iState = EDpReadWait;

                    SetActive();
                    }
                //no else
                break;
                }
            // Reservation failed
            case KErrNotReady:
                {
                LOG("  ERROR, CDpPn2Rx::Read, Element reservation failed (KErrNotReady).");
                OstTrace0( TRACE_NORMAL, DUP8_CDPPN2RX_READ, "ERROR, CDpPn2Rx::Read, Element reservation failed (KErrNotReady)" );

                // Change state
                iState = EElementWait;
                // Better done here
                if ( !IsActive() )
                    {
                    iStatus = KRequestPending;
                    SetActive();
                    }
                //no else
                iRequestActive = ETrue;

                ret = KErrNone;

                break;
                }
            default:
                {
                // An error value returned
                LOG1("  ERROR, CDpPn2Rx::Read, Fail to reserve Element, error code: %d",
                    ret );
                OstTrace1( TRACE_NORMAL, DUP9_CDPPN2RX_READ, "CDpPn2Rx:: ERROR, CDpPn2Rx::Read, Fail to reserve Element, error code: %d", ret );
                }
            }
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::UpDate
// Pipe state has been changed.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::UpDate(
    CDpSubject* /*aChangedSubject*/ )
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_UPDATE, "CDpPn2Rx::UpDate" );
    LOGM("CDpPn2Rx::UpDate");

    TInt r( KErrNone );
    // DCS PIF state changed
    switch ( iPifDcs.PipeState() )
        {
        case CDpPif::EDpPipeEnabled:
            {
            // start read but beware double read
            if ( iState == EElementWait )
                {
                r = Read();
                if ( r != KErrNone )
                    {
                    LOG("  ERROR, CDpPn2Rx::UpDate, Read() failed");
                    OstTrace0( TRACE_NORMAL, DUP1_CDPPN2RX_UPDATE, "ERROR, CDpPn2Rx::UpDate, Read() failed" );
                    }
                //no else
                }
            //no else
            break;
            }
        case CDpPif::EDpPipeDisabled: // Fall through
        case CDpPif::EDpPipeDisconnected:
            {
            if ( IsActive() )
                {
                Cancel();
                }
            //no else
            break;
            }
        default:
            {
            // Pipe is in state that is not regonized
            LOG1("  ERROR, CDpPn2Rx::UpDate, Unrecognized pipe state: %d",
                iPifDcs.PipeState() );
            OstTraceExt1( TRACE_NORMAL, DUP2_CDPPN2RX_UPDATE, "ERROR, CDpPn2Rx::UpDate, Unrecognized pipe state: %s", iPifDcs.PipeState() );

            Cancel();
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::ReleaseNotify
// This method signals ourselves.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::ReleaseNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_RELEASENOTIFY, "CDpPn2Rx::ReleaseNotify" );
    LOGM("CDpPn2Rx::ReleaseNotify");

    // This is done because receive data might me also active
    DoCancel();
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::FlushNotify
// This method notifies that buffer is flushed. Classes which have access
// to buffers are derived from this class. Derived class could override
// this method, otherwise this default method will be used.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::FlushNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_FLUSHNOTIFY, "CDpPn2Rx::FlushNotify" );
    LOGM("CDpPn2Rx::FlushNotify()");

    ReleaseNotify();
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::Role
// This method gets the role (reader/writer) of data client.
// -----------------------------------------------------------------------------
//
TUint8 CDpPn2Rx::Role()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_ROLE, "CDpPn2Rx::Role" );

    return iRole;
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::SetDataAvailableNotification
// Set flag that indicates are requested to notify the C32
// client, if there is data available in rx buffer.
// -----------------------------------------------------------------------------
//
TInt CDpPn2Rx::SetDataAvailableNotification(
    const TBool aUsedOrNot )
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_SETDATAAVAILABLENOTIFICATION, "CDpPn2Rx::SetDataAvailableNotification" );
    LOGM(" CDpPn2Rx::SetDataAvailableNotification");

    iInBufHasDataNotify = aUsedOrNot;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::IsHighWaterMarkReached
// Is high water mark reached
// -----------------------------------------------------------------------------
//
TBool CDpPn2Rx::IsHighWaterMarkReached()
    {
    OstTrace0( TRACE_NORMAL, CDPPN2RX_ISHIGHWATERMARKREACHED, "CDpPn2Rx::IsHighWaterMarkReached" );
    TBool waterMarkReached( EFalse );

    if ( iBufferRx.UsedBytes() >= iDataConfig.WaterMarkHighSize( iBufferRx ) )
        {
        waterMarkReached = ETrue;
        }
    else
        {
        waterMarkReached = EFalse;
        }

    return waterMarkReached;
    }

//==============================================================================

// -----------------------------------------------------------------------------
// CDpPn2Rx::CDpPn2RxObserver::Init
// This method initializes Pn2Rx observer.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::CDpPn2RxObserver::Init(
    CDpPn2Rx* aDpPn2Rx )
    {
    iDpPn2Rx = aDpPn2Rx;
    }

// -----------------------------------------------------------------------------
// CDpPn2Rx::CDpPn2RxObserver::UpDate
// This method updates Pn2Rx.
// -----------------------------------------------------------------------------
//
void CDpPn2Rx::CDpPn2RxObserver::UpDate(
    CDpSubject* aChangedSubject )
    {
    LOGM("CDpPn2Rx::CDpPn2RxObserver::UpDate");

    if ( iDpPn2Rx )
        {
        iDpPn2Rx->UpDate( aChangedSubject );
        }
    //no else
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// none

//  End of File
