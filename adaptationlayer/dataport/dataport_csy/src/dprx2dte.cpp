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
#include "dprx2dte.h"     // moves data from rx buffer to client
#include "dpbreak.h"      // break handling
#include "dpflowctrl.h"   // flow control handling
#include "dpdataconfig.h" // configuration store
#include "dptermdetect.h" // terminator detection
#include "dplog.h"        // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dprx2dteTraces.h"
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
// CDpRx2Dte::CDpRx2Dte
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDpRx2Dte::CDpRx2Dte (
    CDpDataPort& aDataPort ) :
    CActive( KDpRx2DtePriority ),
    iDataPort( aDataPort ),
    iBufferRx( iDataPort.Rx() ),
    iFlowCtrl( iDataPort.FlowCtrl() ),
    iDataConfig( iDataPort.DataConfig() ),
    iBreak( iDataPort.BreakHandler() ),
    iTermDetect( iDataPort.TermDetect() ),
    iRx( 0, 0 ),
    iRxTail( 0, 0 ),
    iRole( MDpDataClient::EDbDataClientReader )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_CDPRX2DTE, "CDpRx2Dte::CDpRx2Dte" );
    LOGM("CDpRx2Dte::CDpRx2Dte");
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_CONSTRUCTL, "CDpRx2Dte::ConstructL" );
    LOGM("CDpRx2Dte::ConstructL");

    // If registering fails => can't do nothing here => must leave
    User::LeaveIfError( iBufferRx.RegisterDataClient( *this ) );

    CActiveScheduler::Add( this );

    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        iRequestActive = ETrue;
        SetActive();
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDpRx2Dte* CDpRx2Dte::NewL(
    CDpDataPort& aDataPort )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_NEWL, "CDpRx2Dte::NewL" );
    LOGM("CDpRx2Dte::NewL");

    CDpRx2Dte* self = new (ELeave) CDpRx2Dte( aDataPort );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::~CDpRx2Dte
// Destructor
// -----------------------------------------------------------------------------
//
CDpRx2Dte::~CDpRx2Dte()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPRX2DTE_CDPRX2DTE, "CDpRx2Dte::~CDpRx2Dte" );
    LOGM("CDpRx2Dte::~CDpRx2Dte");

    iClientBuffer = 0;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::RunL
// We are signalled from Pn2Rx or from ourselves (Rx2Dte).
// Always when we are signalled there is data available to be written
// into client space. This method also checks if RX flow control
// 'water mark low' is reached and informs flow control handler about that.
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::RunL()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_RUNL, "CDpRx2Dte::RunL" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPRX2DTE_RUNL, "CDpRx2Dte:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpRx2Dte::RunL - Port %d", iDataPort.PortUnit() );

    if ( iStatus != KErrCancel )
        {
#ifdef _DEBUG
        TInt ret( KErrNone );
#endif

        if ( 0 < iEchoData.Length() && iReadPending )
            {
            // Writes echo bytes to the client
#ifdef _DEBUG
            ret =
#endif
            WriteEchoToClient();
            }
        else if ( iReadPending )
            {
#ifdef _DEBUG
            ret =
#endif
            WriteIntoClientSpace();
            }
        //no else

#ifdef _DEBUG
        if ( ret != KErrNone )
            {
            LOG(" ERROR, CDpRx2Dte::RunL, Writing echo bytes to the client");
            OstTrace0( TRACE_NORMAL, DUP2_CDPRX2DTE_RUNL, "ERROR, CDpRx2Dte::RunL, Writing echo bytes to the client" );
            }
        //no else
#endif

        if ( !IsActive() )
            {
            iStatus = KRequestPending;
            iRequestActive = ETrue;
            SetActive();
            }
        //no else

        if ( iReadPending &&
            ( 0 < iBufferRx.UsedBytes() || 0 < iEchoData.Length() ) )
            {
            iDataPort.SignalRx2Dte();
            }
        //no else
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::RunError
// Leave in RunL() is handled here. RunL() should not leave.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_RUNERROR, "CDpRx2Dte::RunError" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPRX2DTE_RUNERROR, "CDpRx2Dte:: Port: %u, error code: %d", iDataPort.PortUnit(), aError );

    LOGM2("CDpRx2Dte::RunError - Port %d, error code: %d",
        iDataPort.PortUnit(), aError );

    return aError;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::DoCancel
// This method cancels this active object
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_DOCANCEL, "CDpRx2Dte::DoCancel" );
    LOGM("CDpRx2Dte::DoCancel");

    if ( iRequestActive )
        {
        // do the required signaling
        iRequestActive = EFalse;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::SetDteRead
// This is forwarded client StartRead().
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::SetDteRead(
    const TAny* aDes,
    TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_SETDTEREAD, "CDpRx2Dte::SetDteRead" );
    LOGM("CDpRx2Dte::SetDteRead");

    TInt ret( KErrNone );

    // no pending read
    if ( !iReadPending )
        {
        iReadPending = ETrue;

        // pointer into client buffer
        iClientBuffer = const_cast<TAny*>( aDes );
        iClientRequestedLength = aLength;

        if ( 0 > aLength )
            {
            // ReadOneOrMore
            // C32 gives client buffer size as negative value
            // see ECUART implementation
            iClientBufferLength = -aLength;
            }
        else
            {
            iClientBufferLength = aLength;
            }

        // if there is data ready signal ourselves
        if ( 0 < iBufferRx.UsedBytes() || 0 < iEchoData.Length() )
            {
            iDataPort.SignalRx2Dte();
            }
        //no else
        }
    else
        {
        // read twice attempted
        ret = KErrInUse;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::ResetDteRead
// This is forwarded client made ReadCancel().
// C32 makes ReadCompleted() to DTE --> don't make it here at all.
// This is because DTE may have made timed read and when timer runs
// out C32 wants to complete read by KErrTimedOut.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::ResetDteRead()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_RESETDTEREAD, "CDpRx2Dte::ResetDteRead" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPRX2DTE_RESETDTEREAD, "CDpRx2Dte:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpRx2Dte::ResetDteRead - Port %d", iDataPort.PortUnit() );

    // reset pointer pointing into client space & its length
    iReadPending = EFalse;
    iClientBuffer = 0;
    iClientBufferLength = 0;
    iClientRequestedLength = 0;
    iIPCWriteOffset = 0;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::Write
// Reserve read element and IPCWrite data to client space.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::Write()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_WRITE, "CDpRx2Dte::Write" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPRX2DTE_WRITE, "CDpRx2Dte:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpRx2Dte::Write - Port %d", iDataPort.PortUnit() );

    TInt len( 0 );
    TInt maxLen( 0 );
    TInt ret( KErrNone );

    TCommRole role;
    iDataPort.GetRole( role );

    // verify read pending, else do nothing
    if ( !iReadPending )
        {
        ret = KErrNotReady;
        }
    // check line fail
    else if ( iDataConfig.IsLineFail( role ) )
        {
        iDataPort.ReadCompleted( KErrCommsLineFail );
        ResetDteRead();
        }
    else
        {
        // Find maximum length. i.e. which one is smaller our
        // maximum reservation size or client buffer to write
        // We can't write data over client buffer size -> smaller must be chocen
        maxLen = FindMaximumLength();
        // Lenght is count of bytes before break, but maximum maxLen or
        // lenght is as much as possible, but maximum maxLen
        len = FindLength( maxLen );

        LOG1("  CDpRx2Dte::Write, Rx element reservation - len: %d", len );
        OstTrace1( TRACE_NORMAL, DUP8_CDPRX2DTE_WRITE, "CDpRx2Dte:: Rx element reservation - len: %d", len );

        // try reserve read element, if return success descriptors are
        // set into buffer data
        ret = iBufferRx.ReadElement().Reserve( len, iRx, iRxTail, ETrue );

        switch ( ret )
            {
            case KErrNone:
                {
#ifdef _DEBUG
                iTotalReceived = iTotalReceived + iRx.Length();

                OstTrace1( TRACE_NORMAL, DUP2_CDPRX2DTE_WRITE, "CDpRx2Dte:: iBufferRx.UsedBytes = %d", iBufferRx.UsedBytes() );
                OstTraceExt2( TRACE_NORMAL, DUP3_CDPRX2DTE_WRITE, "CDpRx2Dte:: len = %d, Total Received Bytes = %d", iRx.Length(), iTotalReceived );
                OstTraceExt2( TRACE_NORMAL, DUP4_CDPRX2DTE_WRITE, "CDpRx2Dte:: Client buffer size = %d, DP Max. resv. size = %d", (iClientBufferLength - iIPCWriteOffset), iBufferRx.MaxReservationSize() );
                OstTraceExt2( TRACE_NORMAL, DUP5_CDPRX2DTE_WRITE, "CDpRx2Dte:: Chosen maxLength = %d, Elem. reserved length = %d", maxLen, len );
                OstTraceExt2( TRACE_NORMAL, DUP6_CDPRX2DTE_WRITE, "CDpRx2Dte:: iClientBufferLength = %d, iIPCWriteOffset = %d", iClientBufferLength, iIPCWriteOffset );
                if( iRx.Length() > 0 )
                    {
                    OstTraceExt1( TRACE_NORMAL, DUP7_CDPRX2DTE_WRITE, "CDpRx2Dte:: RX = %s", iRx );
                    }
                else
                    {
                    OstTrace0( TRACE_NORMAL, DUP12_CDPRX2DTE_WRITE, "CDpRx2Dte:: RX buffer empty" );
                    }

                LOG1("  \tiBufferRx.UsedBytes() = %d", iBufferRx.UsedBytes() );
                LOG2("  \tlen = %d, Total Received Bytes = %d",
                    iRx.Length(), iTotalReceived );
                LOG2(" \tClient buffer size = %d, DP Max. resv. size = %d",
                    ( iClientBufferLength - iIPCWriteOffset ),
                    iBufferRx.MaxReservationSize() );
                LOG2(" \tChocen maxLength = %d, Elem. reserved length = %d",
                    maxLen, len );
                LOG2(" \tiClientBufferLength = %d, iIPCWriteOffset = %d",
                    iClientBufferLength, iIPCWriteOffset );
                LOGHEX( iRx, EFalse );

#endif
                if ( iRxTail.Length() )
                    {
                    OstTrace1( TRACE_NORMAL, DUP9_CDPRX2DTE_WRITE, "CDpRx2Dte:: len tail = %d", iRxTail.Length() );
                    OstTraceExt1( TRACE_NORMAL, DUP10_CDPRX2DTE_WRITE, "CDpRx2Dte:: RXTail = %s", iRxTail );

                    LOG1("  \tlen tail = %d", iRxTail.Length() );
                    LOGHEX( iRxTail, EFalse );
                    }
                //no else

                // Write operation is handled here.
                ret = MakeRx2DteWrite( len );
                break;
                }
            case KErrNotReady:
                {
                // we are set active in RunL
                // OBS: returning KErrNone
                ret = KErrNone;
                break;
                }
            default:
                {
                LOG1("  ERROR, CDpRx2Dte::Write Element reservation FAILED, error code: %d",
                    ret );
                OstTrace1( TRACE_NORMAL, DUP11_CDPRX2DTE_WRITE, "ERROR, CDpRx2Dte::Write Element reservation FAILED, error code: %d", ret );

                // returning error code. There's no need to leave because
                // the error situation can be handeled.
                }
            }
        }

    // if there is still data in buffer we are signalled from
    // 1. CDpRx2Dte::SetDteRead when client makes new read
    // 2. from end of RunL when buffer has data and read is pending

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::ReleaseNotify
// This method signals ourselves (Rx2Dte).
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::ReleaseNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_RELEASENOTIFY, "CDpRx2Dte::ReleaseNotify" );
    LOG("CDpRx2Dte::ReleaseNotify");

    // signal ourselves
    if ( iRequestActive )
        {
        iRequestActive = EFalse;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::FlushNotify
// This method notifies that buffer is flushed. Classes which have access
// to buffers are derived from this class. Derived class could override
// this method, otherwise this default method will be used.
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::FlushNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_FLUSHNOTIFY, "CDpRx2Dte::FlushNotify" );
    LOGM("CDpRx2Dte::FlushNotify()");

    ReleaseNotify();
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::Role
// This method gets the role (reader/writer) of data client.
// -----------------------------------------------------------------------------
//
TUint8 CDpRx2Dte::Role()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_ROLE, "CDpRx2Dte::Role" );
    return iRole;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::SetBreakBytes
// Set break bytes to be send before break. If no breakbytes
// we can complete break notify right here.
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::SetBreakBytes(
    const TInt aBreakBytes )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_SETBREAKBYTES, "CDpRx2Dte::SetBreakBytes" );
    LOGM("CDpRx2Dte::SetBreakBytes");

    iBreakBytes = aBreakBytes;

    if ( iBreakBytes > 0 )
        {
        // there is some data before break
        iBreakPending = ETrue;
        }
    else
        {
        // data before break has sent
        iDataPort.BreakNotifyCompleted( KErrNone );
        iBreak.SetBreakNotify( EFalse );
        iBreakPending = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::EchoTx
// We are requested to write TX data back to client buffer.
//  - Append echo data to small buffer.
//  - Signal ourselves to data be written into client space
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::EchoTx(
    const TPtr8& aEchoData )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_ECHOTX, "CDpRx2Dte::EchoTx" );
    LOGM("CDpRx2Dte::EchoTx");

    if ( EchoBytesFreeSpace() >= aEchoData.Length() )
        {
        iEchoData.Append( aEchoData );
        ReleaseNotify();
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::EchoBytes
// This method gets count of echo bytes.
// -----------------------------------------------------------------------------
TInt CDpRx2Dte::EchoBytes()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_ECHOBYTES, "CDpRx2Dte::EchoBytes" );
    LOGM("CDpRx2Dte::EchoBytes");

    return iEchoData.Length();
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::EchoBytesFreeSpace
// This method gets echo buffer's free space.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::EchoBytesFreeSpace()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_ECHOBYTESFREESPACE, "CDpRx2Dte::EchoBytesFreeSpace" );
    LOGM("CDpRx2Dte::EchoBytesFreeSpace");

    TInt space( iEchoData.MaxLength() - iEchoData.Length() );

    if ( 0 > space )
        {
        space = 0;
        }
    //no else

    return space;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::ResetEchoBytes
// This method resets bytes from echo data.
// -----------------------------------------------------------------------------
//
void CDpRx2Dte::ResetEchoBytes()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_RESETECHOBYTES, "CDpRx2Dte::ResetEchoBytes" );
    LOGM("CDpRx2Dte::ResetEchoBytes");

    iEchoData.Zero();
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::WriteEchoToClient
// Writes echo bytes back to client
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::WriteEchoToClient()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_WRITEECHOTOCLIENT, "CDpRx2Dte::WriteEchoToClient" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPRX2DTE_WRITEECHOTOCLIENT, "CDpRx2Dte:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpRx2Dte::WriteEchoToClient - Port %d", iDataPort.PortUnit() );

    TInt ret( KErrNone );
    TInt spaceLeft( iClientBufferLength - iIPCWriteOffset );

    LOG1("  Rx2Dte::WriteEchoToClient, Client buffer length: %d",
        iClientBufferLength );
    OstTrace1( TRACE_NORMAL, DUP2_CDPRX2DTE_WRITEECHOTOCLIENT, "CDpRx2Dte:: Client buffer length: %d", iClientBufferLength );

    if ( iEchoData.Length() > spaceLeft )
        {
        // Avoid client buffer overflow when writing echo data
        // (e.g. at-commands)
        ret = iDataPort.IPCWrite(
            iClientBuffer, iEchoData.Left(spaceLeft), iIPCWriteOffset );
        // Move rest data to left
        iEchoData.Copy( iEchoData.Mid( spaceLeft ) );
        iIPCWriteOffset = iIPCWriteOffset + spaceLeft;
        }
    else
        {
        ret = iDataPort.IPCWrite( iClientBuffer, iEchoData, iIPCWriteOffset );
        iIPCWriteOffset = iIPCWriteOffset + iEchoData.Length();
        iEchoData.Zero();
        }

    if ( KErrNone != ret )
        {
        // Try to recover from write fail
        iEchoData.Zero();

        // Reset read
        iReadPending = EFalse;
        iStatus = KRequestPending;

        if ( !IsActive() )
            {
            iStatus = KRequestPending;
            iRequestActive = ETrue;
            SetActive();
            }
        //no else

        // Complete the read
        iDataPort.ReadCompleted( ret );
        ResetDteRead();
        }
    else
        {
        // We have space
        iDataPort.SignalDte2Tx();

        if ( ( 0 > iClientRequestedLength ) ||
            ( iIPCWriteOffset >= iClientRequestedLength ) ||
            ( iIPCWriteOffset >= iClientBufferLength ) )
            {
            // Complete the read in these situations:
            // 1. ReadOneOrMore (iClientRequestedLength is then negative)
            // 2. client requested length is full
            // 3. client buffer is full
            iDataPort.ReadCompleted( ret );
            ResetDteRead();
            }
        //no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::WriteIntoClientSpace
// Writes bytes into the client space
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::WriteIntoClientSpace()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_WRITEINTOCLIENTSPACE, "CDpRx2Dte::WriteIntoClientSpace" );
    LOGM1("CDpRx2Dte::WriteIntoClientSpace - Port %d", iDataPort.PortUnit() );

    TInt ret( KErrNone );

    // Write into client space
    ret = Write();
    if ( ret != KErrNone )
        {
        // Try to recover from write fail
        iDataPort.ReadCompleted( ret );
        ResetDteRead();

        if ( !IsActive() )
            {
            iStatus = KRequestPending;
            iRequestActive = ETrue;
            SetActive();
            }
        //no else
        }
    else
        {
        // Is flowctrl on and rxBuffer "low watermark" reached
        if ( iFlowCtrl.FlowCtrlDcs2Dp() == EFlowControlOn )
            {
            if ( iBufferRx.UsedBytes() <= iDataConfig.WaterMarkLowSize( iBufferRx ) )
                {
                // OK, now we have again space in buffer
                LOG("  CDpRx2Dte::WriteIntoClientSpace, Rx Low WaterMarkReached.");
                OstTrace0( TRACE_NORMAL, DUP1_CDPRX2DTE_WRITEINTOCLIENTSPACE, "CDpRx2Dte:: Rx Low WaterMark reached" );
                iFlowCtrl.WaterMarkLowReached();
                }
            //no else
            }
        //no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::FindMaximumLength
// Find maximum length i.e. which one is smaller: our maximum
// reservation size or client buffer where we write. We can't
// write data over client buffer size -> smaller must be chocen.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::FindMaximumLength()
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_FINDMAXIMUMLENGTH, "CDpRx2Dte::FindMaximumLength" );
    LOGM("CDpRx2Dte::FindMaximumLength");

    TInt len( 0 );

    if ( ( iClientBufferLength - iIPCWriteOffset ) >
            iBufferRx.UsedBytes() )
        {
        len = iBufferRx.UsedBytes();
        }
    else
        {
        len = iClientBufferLength - iIPCWriteOffset;
        }

    return len;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::FindLength
// This method returns amount of break bytes, if break is pending.
// Amount of used bytes is returned, if break is not pending. If the
// amount exceeds given length, the length is returned.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::FindLength(
    const TInt aMaxLen )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_FINDLENGTH, "CDpRx2Dte::FindLength" );
    LOGM("CDpRx2Dte::FindLength");

    TInt len( 0 );

    if ( iBreakPending )
        {
        // lenght is count of bytes before break, but maximum maxLen
        if ( iBreakBytes <= aMaxLen )
            {
            len = iBreakBytes;
            }
        else
            {
            len = aMaxLen;
            }
        }
    else
        {
        // lenght is as much as possible, but maximum maxLen
        if ( iBufferRx.UsedBytes() <= aMaxLen )
            {
            len = iBufferRx.UsedBytes();
            }
        else
            {
            len = aMaxLen;
            }
        }

    return len;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::MakeRx2DteWrite
// Makes the actual write operation.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::MakeRx2DteWrite(
    const TInt aLen )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_MAKERX2DTEWRITE, "CDpRx2Dte::MakeRx2DteWrite" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPRX2DTE_MAKERX2DTEWRITE, "CDpRx2Dte::MakeRx2DteWrite %x, Port: %u", (TUint)&iDataPort, iDataPort.PortUnit() );

    LOGM2("CDpRx2Dte::MakeRx2DteWrite %x, Port %d",
        &iDataPort, iDataPort.PortUnit() );

    TInt ret( KErrNone );
    TInt ind( 0 );
    TInt termCount( iDataConfig.TerminatorCount() );

    // There is data in rx buffer
    if ( iRx.Length() )
        {
        // Scan terminator from iRx
        if ( 0 < termCount )
            {
            ind = iTermDetect.Scan( iRx );
            // terminator found
            if ( 0 <= ind )
                {
                iRx.SetLength( ind + 1 );
                iRxTail.SetLength( 0 );
                }
            //no else
            }
        //no else

        // IPC write into client space
        ret = iDataPort.IPCWrite( iClientBuffer, iRx, iIPCWriteOffset );
        iIPCWriteOffset = iIPCWriteOffset + iRx.Length();

        if ( KErrNone != ret )
            {
            LOG("  CDpRx2Dte::MakeRx2DteWrite, Rx element release (IPCWrite failed)");
            OstTrace0( TRACE_NORMAL, DUP2_CDPRX2DTE_MAKERX2DTEWRITE, "CDpRx2Dte:: Rx element release (IPCWrite failed)" );

            // Release element, because IPCWrite failed
            TInt err ( iBufferRx.ReadElement().Release( 0 ) );

            if ( KErrNone != err )
                {
                ret = err;
                }
            //no else

            }
        //no else
        }
    //no else

    // Tail has a length, make the write in two pieces
    if ( KErrNone == ret && iRxTail.Length() )
        {
        ret = WriteTail( ind, termCount );

        iIPCWriteOffset = iIPCWriteOffset + iRxTail.Length();

        if ( KErrNone != ret )
            {
            LOG("  CDpRx2Dte::MakeRx2DteWrite, Rx element release (IPCWrite failed)");
            OstTrace0( TRACE_NORMAL, DUP3_CDPRX2DTE_MAKERX2DTEWRITE, "CDpRx2Dte:: Rx element release (IPCWrite failed)" );

            // Release element, because IPCWrite failed
            TInt err ( iBufferRx.ReadElement().Release( iRx.Length() ) );

            if ( KErrNone == err )
                {
                iReadPending = EFalse;
                }
            else
                {
                ret = err;
                }
            }
        //no else
        }
    //no else

    if ( KErrNone == ret )
        {
        if ( ( 0 > iClientRequestedLength ) ||
            ( 0 < termCount && 0 <= ind ) ||
            ( iIPCWriteOffset >= iClientRequestedLength ) ||
            ( iIPCWriteOffset >= iClientBufferLength ) )
            {
            // Complete the read in these situations:
            // 1. ReadOneOrMore (iClientRequestedLength is then negative)
            // 2. client uses terminated_read and terminator byte found
            // 3. client requested length is full
            // 4. client buffer is full
            //
            // (if terminator not found we don't complete read yet)
            iDataPort.ReadCompleted( ret );
            ResetDteRead();
            }
        //no else

        // Release read element
        OstTrace0( TRACE_NORMAL, DUP4_CDPRX2DTE_MAKERX2DTEWRITE, "CDpRx2Dte:: Rx element release" );
        OstTrace1( TRACE_NORMAL, DUP5_CDPRX2DTE_MAKERX2DTEWRITE, "CDpRx2Dte:: Rx element release - iRx: %d", iRx.Length() );
        OstTrace1( TRACE_NORMAL, DUP6_CDPRX2DTE_MAKERX2DTEWRITE, "CDpRx2Dte:: Rx element release - iRxTail: %d", iRxTail.Length() );

        LOG("  CDpRx2Dte::MakeRx2DteWrite, Rx element release");
        LOG1("  CDpRx2Dte::MakeRx2DteWrite, Rx element release - iRx: %d",
            iRx.Length() );
        LOG1("  CDpRx2Dte::MakeRx2DteWrite, Rx element release - iRxTail: %d",
            iRxTail.Length() );

        ret = iBufferRx.ReadElement().Release( iRx.Length() + iRxTail.Length() );
        if ( KErrNone == ret )
           {
            // Reset descriptors
            iRx.SetLength( 0 );
            iRxTail.SetLength( 0 );

            if ( iBreakPending )
                {
                iBreakBytes -= aLen;
                if ( 0 == iBreakBytes )
                    {
                    iBreakPending = EFalse;
                    iDataPort.BreakNotifyCompleted( KErrNone );
                    iBreak.SetBreakNotify( EFalse );
                    }
                //no else
                }
            //no else
            }
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpRx2Dte::WriteTail
// Writes the tail.
// -----------------------------------------------------------------------------
//
TInt CDpRx2Dte::WriteTail(
    TInt& aInd,
    const TInt aTermCount )
    {
    OstTrace0( TRACE_NORMAL, CDPRX2DTE_WRITETAIL, "CDpRx2Dte::WriteTail" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPRX2DTE_WRITETAIL, "CDpRx2Dte:: Port: %u", iDataPort.PortUnit() );
    OstTrace0( TRACE_NORMAL, DUP2_CDPRX2DTE_WRITETAIL, "CDpRx2Dte:: -------------- Write Rx Tail --------------" );

    LOGM1("CDpRx2Dte::WriteTail - Port %d", iDataPort.PortUnit() );
    LOG("-------------- Write Rx Tail --------------");

    // scan terminator from iRxTail
    if ( 0 < aTermCount )
        {
        aInd = iTermDetect.Scan( iRxTail );

        // terminator found
        if ( 0 <= aInd )
            {
            iRxTail.SetLength( aInd + 1 );
            }
        //no else
        }
    //no else

    // we have to make two writes to client buffer. Second write starts
    // from where first one ends --> offset = iRx.Length().
    return ( iDataPort.IPCWrite( iClientBuffer, iRxTail, iIPCWriteOffset ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// none

//  End of File
