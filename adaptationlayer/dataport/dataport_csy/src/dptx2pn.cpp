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
#include "dpbreak.h"         // break
#include "dptx2pn.h"         // moves data from tx buffer to isc api
#include "dpstd.h"           // fault codes etc.
#include "dppif.h"           // pipe interface (pif)
#include "dpflowctrl.h"      // flow control handling
#include "dpdataconfig.h"    // configuration store
#include "dpparityhandler.h" // parity bit handling in 7-bit data mode
#include "pep_comm_types.h"  // pep_comm_types
#include "dplog.h"           // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dptx2pnTraces.h"
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
// C++ default constructor.
//
// ---------------------------------------------------------
CDpTx2Pn::CDpTx2Pn(
    CDpDataPort& aDataPort ) :
    CActive( KDpTx2PnPriority ),
    iDataPort( aDataPort ),
    iBreak( iDataPort.BreakHandler() ),
    iBufferTx( iDataPort.Tx() ),
    iPifDcs( iDataPort.Pif() ),
    iFlowCtrl( iDataPort.FlowCtrl() ),
    iDataConfig( iDataPort.DataConfig() ),
    iParityHandler( iDataPort.ParityHandler() ),
    iTx( 0, 0 ),
    iTxTail( 0, 0 ),
    iRole( MDpDataClient::EDbDataClientReader )
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_CDPTX2PN, "CDpTx2Pn::CDpTx2Pn" );
    LOGM(" CDpTx2Pn::CDpTx2Pn");
    }

// ---------------------------------------------------------
// CDpTx2Pn::ConstructL
// Symbian 2nd phase constructor.
// ---------------------------------------------------------
void CDpTx2Pn::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_CONSTRUCTL, "CDpTx2Pn::ConstructL" );
    LOGM(" CDpTx2Pn::ConstructL");

    // If registering fails => can't do nothing here => must leave
    User::LeaveIfError( iBufferTx.RegisterDataClient( *this ) );

     //  initialise inner observer object
    iDpTx2PnPifObserver.Init( this );
    // register to PIF as observer
    iPifDcs.Attach( iDpTx2PnPifObserver );
    //  initialise inner observer object
    iDpTx2PnFcObserver.Init( this );
    // register to FlowCtrl as observer
    iFlowCtrl.Attach( iDpTx2PnFcObserver );

    CActiveScheduler::Add( this );

    if ( !IsActive() )
        {
        iRequestActive = ETrue;
        iStatus = KRequestPending;
        SetActive();
        }
    //no else
    }

// ---------------------------------------------------------
// CDpTx2Pn::NewL
// Static constructor.
// ---------------------------------------------------------
CDpTx2Pn* CDpTx2Pn::NewL(
    CDpDataPort& aDataPort )
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_NEWL, "CDpTx2Pn::NewL" );
    LOGM(" CDpTx2Pn::NewL");

    CDpTx2Pn* self = new( ELeave ) CDpTx2Pn( aDataPort );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CDpTx2Pn::~CDpTx2Pn
// Destructor
// ---------------------------------------------------------
CDpTx2Pn::~CDpTx2Pn()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPTX2PN_CDPTX2PN, "CDpTx2Pn::~CDpTx2Pn" );
    LOGM(" CDpTx2Pn::~CDpTx2Pn");
    DEBUG( "CDpTx2Pn::~CDpTx2Pn" )

    // unregister from PIF as observer
    iPifDcs.Detach( iDpTx2PnPifObserver );
    // unregister from FlowCtrl as observer
    iFlowCtrl.Detach( iDpTx2PnFcObserver );
    }

// ---------------------------------------------------------
// CDpTx2Pn::RunL
// This is called when Tx2Pn is signalled. Tx2Pn can be
// signalled by using iDataPort.SignalTx2Pn() or inside
// CDpTx2Pn class just calling ReleaseNotify().
// ---------------------------------------------------------
//
void CDpTx2Pn::RunL()
    {
    OstTrace1( TRACE_NORMAL, CDPTX2PN_RUNL, "CDpTx2Pn::RunL - Port: %u", iDataPort.PortUnit() );
    LOGM1("CDpTx2Pn::RunL - Port %d", iDataPort.PortUnit() );
    DEBUG("CDpTx2Pn::RunL\r\n" )

    TInt ret( KErrNone );
    TBool isNotReady( EFalse );

    // Request is completed
    iRequestActive = EFalse;

    if ( iStatus != KErrCancel )
        {
        if ( !IsFlowCtrlOn() )
            {
            // Call WritePn
            ret = WritePn();

            switch ( ret )
                {
                case KErrNone:
                case KErrLocked:
                    {
                    // Ok, do nothing
                    break;
                    }
                case KErrNotReady:
                    {
                    LOG1("CDpTx2Pn::RunL - Error: %d", ret );
                    OstTrace1( TRACE_NORMAL, DUP1_CDPTX2PN_RUNL, "CDpTx2Pn:: - Error: %d", ret );
                    // Try to recover from KErrNotReady fail
                    if ( !IsActive() )
                        {
                        iRequestActive = ETrue;
                        iStatus = KRequestPending;
                        SetActive();
                        }
                    //no else
                    isNotReady = ETrue;
                    break;
                    }
                case KErrOverflow:
                case KErrNoMemory:
                    {
                    LOG1("CDpTx2Pn::RunL - Error: %d", ret );
                    OstTrace1( TRACE_NORMAL, DUP2_CDPTX2PN_RUNL, "CDpTx2Pn:: - Error: %d", ret );

                    // KErrOverFlow (== ISC flow control on),
                    // KErrNomemory (== other ISC problems, no memory)
                    // => send failed
                    // Keep data in buffer but release data element
                    // (will be reserved again)
                    if ( KErrNone == iBufferTx.ReadElement().Release( 0 ) )
                        {
                        // reset descriptors
                        iTx.SetLength( 0 );
                        iTxTail.SetLength( 0 );
                        }
                    break;
                    }
                default:
                    {
                    LOG1("CDpTx2Pn::RunL - Error: %d", ret );
                    OstTrace1( TRACE_NORMAL, DUP3_CDPTX2PN_RUNL, "CDpTx2Pn:: - Error: %d", ret );

                    // Something went wrong in WritePn method
                    // e.g. element reservation, must leave.
                    User::Leave(ret);
                    break;
                    }

                } // switch ( ret )
            }
        else
            {
            LOG(" CDpTx2Pn::RunL, Flow control blocking");
            OstTrace0( TRACE_NORMAL, DUP4_CDPTX2PN_RUNL, "CDpTx2Pn:: Flow control blocking" );
            }

        if ( !isNotReady )
            {
            // Transmit buffer empty, is client intrested about that
            if ( 0 == ( iBufferTx.UsedBytes() ) && ( iOutBufIsEmptyNotify ) )
                {
                // We should notify client now.
                iOutBufIsEmptyNotify = EFalse;
                iDataPort.NotifyOutputEmptyCompleted( KErrNone );
                }
            //no else

            // Set active again
            if ( !IsActive() )
                {
                iRequestActive = ETrue;
                iStatus = KRequestPending;
                SetActive();
                }
            //no else

            // Signal ourselves only if there is data in buffer and
            // DP->DCS flow control is off.
            if ( iBufferTx.UsedBytes() && !IsFlowCtrlOn() )
                {
                ReleaseNotify();
                }
            //no else
            }
        //no else
        }
    //no else

    }

// ---------------------------------------------------------
// CDpTx2Pn::RunError
// Leave in RunL() is handled here. Error code is returned,
// when internal error has occured.
// ---------------------------------------------------------
//
TInt CDpTx2Pn::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_RUNERROR, "CDpTx2Pn::RunError" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPTX2PN_RUNERROR, "CDpTx2Pn:: Port: %u, error code: %d", iDataPort.PortUnit(), aError );

    LOGM2("CDpTx2Pn::RunError - Port %d, error code: %d",
        iDataPort.PortUnit(), aError );

    return aError;
    }

// ---------------------------------------------------------
// CDpTx2Pn::DoCancel
// This method cancels this active object by
// RequestComplete(). This method must be called directly
// (not through Cancel()), when RunL() should be completed
// with KErrCancel.
// ---------------------------------------------------------
//
void CDpTx2Pn::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_DOCANCEL, "CDpTx2Pn::DoCancel" );
    LOGM(" CDpTx2Pn::DoCancel");
    DEBUG( "CDpTx2Pn::DoCancel\n" )

    if ( iRequestActive )
        {
        // do the required signaling
        iRequestActive = EFalse;
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, KErrCancel );
        }
    //no else
    }

// ---------------------------------------------------------
// CDpTx2Pn::WritePn
// Writes data to Isc Api.
// ---------------------------------------------------------
//
TInt CDpTx2Pn::WritePn()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_WRITEPN, "CDpTx2Pn::WritePn" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPTX2PN_WRITEPN, "CDpTx2Pn:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpTx2Pn::WritePn - Port %d", iDataPort.PortUnit() );

    TInt len( 0 );
    TInt ret( KErrNone );

    // check pipe is enabled
    if ( iPifDcs.PipeState() != CDpPif::EDpPipeEnabled )
        {
        ret = KErrNotReady;
        }
    else if ( iBreakPending )
        {
        // length is as much as there is data before break,
        // but maximum KDpMaximumTxReservationSize
        if ( iBreakBytes > iBufferTx.MaxReservationSize() )
            {
            len = iBufferTx.MaxReservationSize();
            }
        else
            {
            len = iBreakBytes;
            }
        }
    else
        {
        // length is as much as there is but maximum KDpMaximumTxReservationSize
        if ( iBufferTx.UsedBytes() > iBufferTx.MaxReservationSize() )
            {
            len = iBufferTx.MaxReservationSize();
            }
        else
            {
            len = iBufferTx.UsedBytes();
            }
        }

    // Length can be 0, if Tx buffer has been flushed.
    // Try to recover with KErrNotReady error.
    if ( ( 0 == len ) && ( KErrNone == ret ) )
        {
        ret = KErrNotReady;
        }
    else if ( KErrNone == ret )
        {
        LOG("  CDpTx2Pn::WritePn, Tx element reservation");
        OstTrace0( TRACE_NORMAL, DUP2_CDPTX2PN_WRITEPN, "CDpTx2Pn:: Tx element reservation" );

        // try reserve read element, descriptor set to iTx,
        // possible tail into iTxTail
        TInt result( iBufferTx.ReadElement().Reserve( len, iTx, iTxTail ) );

        ret = HandleReturnValue( result );
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpTx2Pn::IsFlowCtrlOn
// Is flow control on between DP->DCS. Returns also ETrue,
// when pipe is not enabled.
// ---------------------------------------------------------
//
TBool CDpTx2Pn::IsFlowCtrlOn()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_ISFLOWCTRLON, "CDpTx2Pn::IsFlowCtrlOn" );
    LOGM(" CDpTx2Pn::IsFlowCtrlOn");

    TBool ret( EFalse );

    if ( EFlowControlOn == iFlowCtrl.FlowCtrlDp2Dcs() ||
        CDpPif::EDpPipeEnabled != iPifDcs.PipeState() )
        {
        ret = ETrue;
        }
    else
        {
        ret = EFalse;
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpTx2Pn::ReleaseNotify
// This method signals this active object.
// ---------------------------------------------------------
//
void CDpTx2Pn::ReleaseNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_RELEASENOTIFY, "CDpTx2Pn::ReleaseNotify" );
    LOG(" CDpTx2Pn::ReleaseNotify");
    DEBUG( "CDpTx2Pn::ReleaseNotify" )

    // signal ourselves
    if ( iRequestActive )
        {
        iRequestActive = EFalse;
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, KErrNone );
        }
    //no else
    }

// ---------------------------------------------------------
// This method notifies that buffer is flushed. Classes which have access
// to buffers are derived from this class. Derived class could override
// this method, otherwise this default method will be used.
// release this cactive object
// ---------------------------------------------------------
//
void CDpTx2Pn::FlushNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_FLUSHNOTIFY, "CDpTx2Pn::FlushNotify" );
    LOGM(" CDpTx2Pn::FlushNotify()");

    ReleaseNotify();
    }

// -----------------------------------------------------------------------------
// CDpTx2Pn::Role
// This method gets the role (reader/writer) of data client.
// -----------------------------------------------------------------------------
//
TUint8 CDpTx2Pn::Role()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_ROLE, "CDpTx2Pn::Role" );

    return iRole;
    }

// ---------------------------------------------------------
// CDpTx2Pn::UpDate
// Called after pipe state change and after flow control
// state change.
// ---------------------------------------------------------
//
void CDpTx2Pn::UpDate(
    CDpSubject* /*aChangedSubject*/ )
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_UPDATE, "CDpTx2Pn::UpDate" );
    LOG(" CDpTx2Pn::UpDate");
    DEBUG( "CDpTx2Pn::UpDate" )

    // if there is data ready signal ourselves
    if ( ( 0 < iBufferTx.UsedBytes() ) && ( !IsFlowCtrlOn() ) )
        {
        ReleaseNotify();
        }
    //no else
    }

// ---------------------------------------------------------
// CDpTx2Pn::SetBreakBytes
// Set up flag if there is some data to be send before break
// (this is client->DCS break)
// ---------------------------------------------------------
//
void CDpTx2Pn::SetBreakBytes(
    const TInt aBreakBytes)
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_SETBREAKBYTES, "CDpTx2Pn::SetBreakBytes" );
    LOG(" CDpTx2Pn::SetBreakBytes");
    DEBUG( "CDpTx2Pn::SetBreakBytes()" )

    iBreakBytes = aBreakBytes;
    // put break pending
    iBreakPending = ETrue;
    }

// ---------------------------------------------------------
// CDpTx2Pn::CancelBreakBytes
// Clears pending break bytes. Client may have cancelled
// it's break request.
// ---------------------------------------------------------
//
void CDpTx2Pn::CancelBreakBytes()
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_CANCELBREAKBYTES, "CDpTx2Pn::CancelBreakBytes" );
    LOG(" CDpTx2Pn::CancelBreakBytes");
    DEBUG( "CDpTx2Pn::CancelBreakBytes()" )

    iBreakBytes = 0;
    iBreakPending = EFalse;
    }

// ---------------------------------------------------------
// CDpTx2Pn::SetOutputBufferEmptyNotification
// Setup flag that we are requested to notify or not the
// client, when output buffer (TX buffer) empties.
// ---------------------------------------------------------
//
void CDpTx2Pn::SetOutputBufferEmptyNotification(
    const TBool aUsedOrNot )
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_SETOUTPUTBUFFEREMPTYNOTIFICATION, "CDpTx2Pn::SetOutputBufferEmptyNotification" );
    LOG(" CDpTx2Pn::SetOutputBufferEmptyNotification");
    DEBUG( "CDpTx2Pn::SetOutputBufferEmptyNotification")

    iOutBufIsEmptyNotify = aUsedOrNot;
    }

// ---------------------------------------------------------
// CDpTx2Pn::PipeSendWithParityAdd
// This method writes data to Isc Api. In 7-bit data case
// parity bits are added here using parity handler class.
// ---------------------------------------------------------
//
TInt CDpTx2Pn::PipeSendWithParityAdd(
    TPtr8& aTx )
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_PIPESENDWITHPARITYADD, "CDpTx2Pn::PipeSendWithParityAdd" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPTX2PN_PIPESENDWITHPARITYADD, "CDpTx2Pn:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpTx2Pn::PipeSendWithParityAdd - Port %d", iDataPort.PortUnit() );

    TUint8 pipeHandle( 0 );
    TInt result( iDataPort.ActivePipeHandle( pipeHandle ) );

    if ( KErrNone == result )
        {
        if ( EData7 == iDataConfig.DataBits() )
            {
            // DteParityBitWhen7Mode parameter tells whether parity bit should
            // be carried or not in data mesages in a pipe when DTE interface
            // is in  7-bit mode.
            if ( iDataConfig.DteParityBitWhen7Mode() == PEP_COMM_DATA_WITH_PARITY )
                {
                // use parity bits
                iParityHandler.AddParityBits( aTx, iDataConfig.Parity() );
                }
            //no else
            }
        //no else

        result = iDataPort.ISAHandle().DataSend( aTx );

#if defined(_DEBUG)
        if ( KErrNone != result )
            {
            LOG1("Error Pipe send %d", result );
            OstTrace1( TRACE_NORMAL, DUP2_CDPTX2PN_PIPESENDWITHPARITYADD, "CDpTx2Pn:: Error Pipe send %d", result );
            }
        //no else
#endif
        }
    //no else

    return result;
    }

// ---------------------------------------------------------
// CDpTx2Pn::HandleReturnValue
// Handles the reserve methods return value.
// ---------------------------------------------------------
//
TInt CDpTx2Pn::HandleReturnValue(
    const TInt aReturnValue )
    {
    OstTrace0( TRACE_NORMAL, CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn::HandleReturnValue" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpTx2Pn::HandleReturnValue - Port %d", iDataPort.PortUnit() );

    TInt result( KErrNone );

    switch ( aReturnValue )
        {
        case KErrNone:
            {
            // write iTx
            if ( 0 < iTx.Length() )
                {
                result = PipeSendWithParityAdd( iTx );
                }
            //no else

#ifdef _DEBUG
            LOG1("-- WRITE -- %x", &iDataPort);
            OstTrace1( TRACE_NORMAL, DUP2_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: -- WRITE -- %x", &iDataPort );

            if ( 0 < iTx.Length() )
                {
                if ( KErrOverflow != result )
                    {
                    iTotalSent = iTotalSent + iTx.Length();
                    }
                OstTraceExt2( TRACE_NORMAL, DUP3_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: Tx len = %d, Total Bytes sent = %d", iTx.Length(), iTotalSent );
                OstTraceExt1( TRACE_NORMAL, DUP12_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: TX = %s", iTx );

                LOG2("    Tx len = %d, Total Bytes sent = %d",
                    iTx.Length(), iTotalSent );
                LOGHEX( iTx, ETrue );
                }
            //no else
#endif

            if ( KErrNone == result )
                {
                TInt len( 0 );

                // Write iTx OK => Write iTxTail
                if ( 0 < iTxTail.Length() )
                    {
                    result = PipeSendWithParityAdd( iTxTail );
                    }
                //no else

#ifdef _DEBUG
                if ( 0 < iTxTail.Length() )
                    {
                    if ( KErrOverflow != result )
                        {
                        iTotalSent = iTotalSent + iTxTail.Length();
                        }
                    OstTraceExt2( TRACE_NORMAL, DUP5_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: Tx Tail len = %d, Total Bytes sent = %d", iTxTail.Length(), iTotalSent );
                    OstTraceExt1( TRACE_NORMAL, DUP13_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: TX = %s", iTxTail );

                    LOG2("    Tx Tail len = %d, Total Bytes sent = %d",
                        iTxTail.Length(), iTotalSent );
                    LOGHEX( iTxTail, ETrue );
                    }
                //no else
#endif

                if ( KErrNone == result )
                    {
                    // Write iTx OK & Write iTxTail OK
                    // => release iTx and iTxTail
                    LOG(" CDpTx2Pn::HandleReturnValue, Tx element release (iTx + iTxTail)");
                    OstTrace0( TRACE_NORMAL, DUP7_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: Tx element release (iTx + iTxTail)" );
                    len = iTx.Length() + iTxTail.Length();
                    }
                else
                    {
                    // Write iTx OK & Write iTxTail FAIL
                    // => release only iTx
                    OstTrace0( TRACE_NORMAL, DUP8_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: Tail sending failed" );
                    OstTrace0( TRACE_NORMAL, DUP9_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: Tx element release (iTx)" );

                    LOG(" CDpTx2Pn::HandleReturnValue, Tail sending failed");
                    LOG(" CDpTx2Pn::HandleReturnValue, Tx element release (iTx)");
                    len = iTx.Length();
                    }

                // release read element
                result = iBufferTx.ReadElement().Release( len );

                if ( KErrNone == result )
                    {
                    // reset descriptors
                    iTx.SetLength( 0 );
                    iTxTail.SetLength( 0 );

                    if ( iBreakPending )
                        {
                        iBreakBytes -= len;

                        if ( iBreakBytes == 0 )
                            {
                            iBreakPending = EFalse;
                            TInt ret = iBreak.SendBreak();
                            iDataPort.BreakCompleted( ret );
                            }
                        //no else
                        }
                    //no else
                    }
                //no else
                }
            //no else

            break;
            }
        case KErrNoMemory:
            {
            DEBUG( "CDpTx2Pn, waiting..." )
            OstTrace0( TRACE_NORMAL, DUP10_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: waiting..." );

            // do nothing, in RunL we are set active again
            break;
            }
        default:
            {
            LOG(" CDpTx2Pn::HandleReturnValue, Element reservation failed");
            OstTrace0( TRACE_NORMAL, DUP11_CDPTX2PN_HANDLERETURNVALUE, "CDpTx2Pn:: Element reservation failed" );

            // Return the error code. Caller of the WritePn can handel this.
            result = aReturnValue;
            }
        }

    return result;
    }

//===========================================================

// ---------------------------------------------------------
// CDpTx2Pn::CDpTx2PnObserver::Init
// This method initializes Tx2Pn observer.
// ---------------------------------------------------------
//
void CDpTx2Pn::CDpTx2PnObserver::Init(
    CDpTx2Pn* aDpTx2Pn )
    {
    iDpTx2Pn = aDpTx2Pn;
    }

// ---------------------------------------------------------
// CDpTx2Pn::CDpTx2PnObserver::UpDate
// This method updates Tx2Pn.
// ---------------------------------------------------------
//
void CDpTx2Pn::CDpTx2PnObserver::UpDate(
    CDpSubject* aChangedSubject )
    {
    LOG(" CDpTx2Pn::CDpTx2PnObserver::UpDate");

    if ( iDpTx2Pn )
        {
        iDpTx2Pn->UpDate( aChangedSubject );
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
// none

//  End of File
