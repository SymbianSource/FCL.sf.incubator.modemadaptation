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
#include "dpdef.h"        // dataport definitions
#include "dpdataport.h"   // dataport main and c32 interface
#include "dpdte2tx.h"     // moves data from client to tx buffer
#include "dpflowctrl.h"   // flow control handling
#include "dpdataconfig.h" // configuration store
#include "dpescdetect.h"  // escape sequence detection
#include "dprx2dte.h"     // moves data from rx buffer to client
#include "dplog.h"        // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpdte2txTraces.h"
#endif

// LOCAL FUNCTION PROTOTYPES
// none

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDpDte2Tx::CDpDte2Tx
// C++ default constructor.
// ---------------------------------------------------------
CDpDte2Tx::CDpDte2Tx(
    CDpDataPort& aDataPort ) :
    CActive( KDpDte2TxPriority ),
    iDataPort( aDataPort ),
    iBufferTx( iDataPort.Tx() ),
    iFlowCtrl( iDataPort.FlowCtrl() ),
    iEscDetect( iDataPort.EscDetect() ),
    iDataConfig( iDataPort.DataConfig() ),
    iRx2Dte( iDataPort.Rx2Dte() ),
    iTx( 0, 0 ),
    iRequestActive( EFalse ),
    iRole( MDpDataClient::EDbDataClientWriter )
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_CDPDTE2TX, "CDpDte2Tx::CDpDte2Tx" );
    LOGM(" CDpDte2Tx::CDpDte2Tx");
    }

// ---------------------------------------------------------
// CDpDte2Tx::ConstructL
// Symbian 2nd phase constructor.
// ---------------------------------------------------------
void CDpDte2Tx::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_CONSTRUCTL, "CDpDte2Tx::ConstructL" );
    LOGM(" CDpDte2Tx::ConstructL");

    //  initialise inner observer object
    iDpDte2TxFcObserver.Init(this);

    // register to FlowCtrl as observer
    iFlowCtrl.Attach( iDpDte2TxFcObserver );

    // If registering fails => can't do nothing here => must leave
    User::LeaveIfError( iBufferTx.RegisterDataClient( *this ) );

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CDpDte2Tx::NewL
// Static constructor.
// ---------------------------------------------------------
CDpDte2Tx* CDpDte2Tx::NewL(
    CDpDataPort& aDataPort )
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_NEWL, "CDpDte2Tx::NewL" );
    LOGM(" CDpDte2Tx::NewL");

    CDpDte2Tx* self = new ( ELeave ) CDpDte2Tx( aDataPort );

    CleanupStack::PushL( reinterpret_cast< CActive* > ( self ) );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CDpDte2Tx::~CDpDte2Tx
// Destructor
// ---------------------------------------------------------
CDpDte2Tx::~CDpDte2Tx()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPDTE2TX_CDPDTE2TX, "CDpDte2Tx::~CDpDte2Tx" );
    LOGM(" CDpDte2Tx::~CDpDte2Tx");

    iPendingClientBuffer = 0;

    // unregister from FlowCtrl as observer
    iFlowCtrl.Detach( iDpDte2TxFcObserver );
    }

// ---------------------------------------------------------
// CDpDte2Tx::WriteTx
// This method puts write pending and signals itself
// (Dte2Tx).
// ---------------------------------------------------------
//
TInt CDpDte2Tx::WriteTx(
    const TAny* aClientBuffer,
    const TInt aLength)
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_WRITETX, "CDpDte2Tx::WriteTx" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPDTE2TX_WRITETX, "CDpDte2Tx:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpDte2Tx::WriteTx - Port %d", iDataPort.PortUnit() );

    TInt ret( KErrNone );

    if ( iWritePending )
        {
        //double write
        ret = KErrWrite;
        }
    else if ( 0 > aLength  )
        {
        //underflow
        ret = KErrArgument;
        }
    else if ( 0 != aLength )
        {
        // store pointer of client buffer
        iPendingClientBuffer = ( TAny* ) aClientBuffer;
        iPendingLength = aLength;

        iWritePending = ETrue;

        if ( !IsActive() )
            {
            iRequestActive = ETrue;
            iStatus = KRequestPending;
            SetActive();
            }
        //no else

        iDataPort.SignalDte2Tx();
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDte2Tx::RunL
// This method handles write operation after signalling.
// ---------------------------------------------------------
//
void CDpDte2Tx::RunL()
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_RUNL, "CDpDte2Tx::RunL" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPDTE2TX_RUNL, "CDpDte2Tx::RunL %x", &iDataPort );
    OstTrace1( TRACE_NORMAL, DUP2_CDPDTE2TX_RUNL, "CDpDte2Tx:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpDte2Tx::RunL %x", &iDataPort);
    LOGM1("CDpDte2Tx::RunL - Port %d", iDataPort.PortUnit() );

    if ( iWritePending && iStatus != KErrCancel )
        {
        // check linefail
        TCommRole role;
        iDataPort.GetRole( role );
        if ( iDataConfig.IsLineFail( role ) )
            {
            LOG(" ERROR, Dte2Tx::RunL, Dte write completed with LINE FAIL.");
            OstTrace0( TRACE_NORMAL, DUP3_CDPDTE2TX_RUNL, "ERROR, Dte2Tx::RunL, Dte write completed with LINE FAIL." );

            iDataPort.WriteCompleted( KErrCommsLineFail );
            iWritePending = EFalse;
            iPendingClientBuffer = 0;
            iPendingLength = 0;
            }
        else if ( iDataConfig.IsEchoDte() &&
            ( iPendingLength - iIPCReadOffset ) > iRx2Dte.EchoBytesFreeSpace() )
            {
            if ( !IsActive() )
                {
                iRequestActive = ETrue;
                iStatus = KRequestPending;
                SetActive();
                }
            //no else
            }
        else
            {
            // first take minimum of three elements
            TInt clientLength( iPendingLength - iIPCReadOffset );
            TInt freeBytes( iBufferTx.FreeBytes() );
            TInt len( 0 );

            if ( clientLength >= iBufferTx.MaxReservationSize() )
                {
                len = iBufferTx.MaxReservationSize();
                }
            else
                {
                len = clientLength;
                }

            if ( freeBytes > 0 )
                {
                if (len >= freeBytes)
                    {
                    len = freeBytes;
                    }
                //no else
                }
            //no else

            TPtr8 nullPtr( 0, 0 );
            CDpDataElement& elem = iBufferTx.WriteElement();

            TInt ret( elem.Reserve( len, iTx, nullPtr ) );

            if ( ret != KErrNone)
                {
                if ( ret == KErrNotReady )
                    {
                    if ( !IsActive() )
                        {
                        iRequestActive = ETrue;
                        iStatus = KRequestPending;
                        SetActive();
                        }
                    //no else
                    }
                else
                    {
                    LOG1(" ErrorDte2Tx Tx element reservation failure! Id: %d",
                        ret );
                    OstTrace1( TRACE_NORMAL, DUP4_CDPDTE2TX_RUNL, "CDpDte2Tx:: ErrorDte2Tx Tx element reservation failure! Id: %d", ret );

                    User::Leave( ret );
                    }
                }
            else
                {
                // we suppose it's always either KErrNone or KErrNotReady ...
                User::LeaveIfError( iDataPort.IPCRead(
                    iPendingClientBuffer, iTx, iIPCReadOffset ) );

                if ( iDataConfig.IsEchoDte() && 0 < iTx.Length() )
                    {
                    // configured to echo tx bytes
                    iRx2Dte.EchoTx( iTx );
                    }
                //no else

                iEscDetect.Scan( iTx );

                // WasReaderWaiting is used to check if the reader was waiting
                // for more data until it is being able to read. Otherwise it
                // should signal to reader (Tx2Pn) that it can then read
                TBool WasReaderWaiting = iBufferTx.IsReaderWaiting();

                User::LeaveIfError( elem.Release( iTx.Length() ) );

                iIPCReadOffset += iTx.Length();

                // If reader was waiting for more data and got the enough,
                // DPElement from Release will notify Reader using ReleaseNotify().
                // WasReaderWaiting checks if Reader was waiting, if it is not,
                // it should signal the reader to read the buffer.
                if ( !WasReaderWaiting )
                    {
                    iDataPort.SignalTx2Pn();
                    }
                //no else

                if ( iIPCReadOffset >= iPendingLength )
                    {
                    LOG(" Dte2Tx::RunL, Dte write completed.");
                    OstTrace0( TRACE_NORMAL, DUP5_CDPDTE2TX_RUNL, "CDpDte2Tx:: Dte write completed" );

                    iIPCReadOffset = 0;
                    iDataPort.WriteCompleted( KErrNone );
                    iWritePending = EFalse;
                    iPendingClientBuffer = 0;
                    iPendingLength = 0;
                    }
                else
                    {
                    if ( !IsActive() )
                        {
                        iRequestActive = ETrue;
                        iStatus = KRequestPending;
                        SetActive();
                        }
                    //no else
                    iDataPort.SignalDte2Tx();
                    }
                }
            }
        }
        //no else
    }

// ---------------------------------------------------------
// CDpDte2Tx::RunError
// Leave in RunL() is handled here. Error code is returned,
// when internal error has occured.
// ---------------------------------------------------------
//
TInt CDpDte2Tx::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_RUNERROR, "CDpDte2Tx::RunError" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPDTE2TX_RUNERROR, "CDpDte2Tx:: Port: %u, error code: %d", iDataPort.PortUnit(), aError );

    LOGM2("CDpDte2Tx::RunError - Port %d, error code: %d",
        iDataPort.PortUnit(), aError );

    return aError;
    }

// ---------------------------------------------------------
// CDpDte2Tx::UpDate
// We are observer of FlowCtrl. E.g. when flow control
// status changes, we are called.
// ---------------------------------------------------------
//
void CDpDte2Tx::UpDate(
    CDpSubject* /*aChangedSubject*/)
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_UPDATE, "CDpDte2Tx::UpDate" );
    LOGM(" CDpDte2Tx::UpDate");

    ReleaseNotify();
    }

// ---------------------------------------------------------
// CDpDte2Tx::DoCancel
// This method cancels this active object by
// RequestComplete(). This method must be called directly
// (not through Cancel()), when RunL() should be completed
// with KErrCancel.
// ---------------------------------------------------------
//
void CDpDte2Tx::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_DOCANCEL, "CDpDte2Tx::DoCancel" );
    LOGM(" CDpDte2Tx::DoCancel");

    if ( iRequestActive )
        {
        iRequestActive = EFalse;
        // do the required signaling
        TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrCancel );
        }
    //no else
    }

// ---------------------------------------------------------
// CDpDte2Tx::ReleaseNotify
// This method signals ourselves (Dte2Tx).
// ---------------------------------------------------------
//
void CDpDte2Tx::ReleaseNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_RELEASENOTIFY, "CDpDte2Tx::ReleaseNotify" );
    LOGM(" CDpDte2Tx::ReleaseNotify");

    // signal ourselves
    if ( iRequestActive )
        {
        iRequestActive = EFalse;
        TRequestStatus* status = &iStatus;

        User::RequestComplete( status, KErrNone );
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
void CDpDte2Tx::FlushNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_FLUSHNOTIFY, "CDpDte2Tx::FlushNotify" );
    LOGM(" CDpDte2Tx::FlushNotify()");

    ReleaseNotify();
    }

// ---------------------------------------------------------
// CDpDte2Tx::Role
// This method gets the role (reader/writer) of data client.
// ---------------------------------------------------------
//
TUint8 CDpDte2Tx::Role()
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_ROLE, "CDpDte2Tx::Role" );
    return iRole;
    }

// ---------------------------------------------------------
// CDpDte2Tx::CancelWrite
// This method is called by client via
// CDpDataPort::WriteCancel(). Client want to cancel pending
// write operation. C32 makes WriteCompleted to DTE -->
// don't make it here at all.
// Status : Proposal
// ---------------------------------------------------------
//
void CDpDte2Tx::CancelWrite()
    {
    OstTrace0( TRACE_NORMAL, CDPDTE2TX_CANCELWRITE, "CDpDte2Tx::CancelWrite" );
    LOGM(" CDpDte2Tx::CancelWrite");

    if ( iWritePending )
        {
        iPendingClientBuffer = 0;
        iPendingLength = 0;
        iWritePending = EFalse;
        iIPCReadOffset = 0;
        iDataPort.SignalDte2Tx();
        }
    //no else
    }

// ---------------------------------------------------------
// CDpDte2Tx::CDpDte2TxObserver::Init
// This method initializes Dte2Tx observer.
// ---------------------------------------------------------
//
void CDpDte2Tx::CDpDte2TxObserver::Init(
    CDpDte2Tx* aDpDte2Tx )
    {
    iDpDte2Tx = aDpDte2Tx;
    }

// ---------------------------------------------------------
// CDpDte2Tx::CDpDte2TxObserver::UpDate
// This method updates Dte2Tx.
// ---------------------------------------------------------
//
void CDpDte2Tx::CDpDte2TxObserver::UpDate(
    CDpSubject* aChangedSubject )
    {
    LOG(" CDpDte2Tx::CDpDte2TxObserver::UpDate");

    if ( iDpDte2Tx )
        {
        iDpDte2Tx->UpDate( aChangedSubject );
        }
    //no else
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
// none

//  End of File
