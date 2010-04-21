/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cusbpnisareceiver.h"
#include "cusbpnusbsender.h"
#include "cusbpnpacket.h"
#include "usbpndefinitions.h"   // For constants
#include "usbpntrace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnisareceiverTraces.h"
#endif

#include <iscapi.h>             // For RIscApi
#include <d32usbc.h>            // For RDevUsbcClient
#include <phonetisi.h>          // For Phonet definitions
#ifndef NCP_COMMON_BRIDGE_FAMILY
#include <pn_const.h>           // For Resource ID
#include <mediaisi.h>           // For Media specific message ID
#endif /* NCP_COMMON_BRIDGE_FAMILY */


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::CUsbPnIsaReceiver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnIsaReceiver::CUsbPnIsaReceiver( RIscApi& aIscApi )
#ifdef NCP_COMMON_ENOSW_IN_USE
// CUsbPnUsbSender uses ECommTransmitPriority=100,
// Set priority lower so that CUsbPnUsbSender can send packages.
    :CActive( 99 )
#else
    :CActive( CActive::EPriorityStandard - 1 )
#endif
    ,iIscApi(aIscApi)
    ,iRecvPtr( NULL, NULL )
    ,iForwardFlag(EFalse)
    {
    OstTrace1( TRACE_NORMAL, CUSBPNISARECEIVER_CUSBPNISARECEIVER_ENTRY, "CUsbPnIsaReceiver::CUsbPnIsaReceiver;aIscApi=%x", ( TUint )&( aIscApi ) );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::CUsbPnIsaReceiver( aIscApi:0x%x )" ), &aIscApi ) );

    CActiveScheduler::Add(this);

    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_CUSBPNISARECEIVER_EXIT, "CUsbPnIsaReceiver::CUsbPnIsaReceiver - return" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::CUsbPnIsaReceiver() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnIsaReceiver::ConstructL( RDevUsbcClient& aLdd )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_CONSTRUCTL_ENTRY, "CUsbPnIsaReceiver::ConstructL" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::ConstructL()" ) ) );

    iUsbSender = CUsbPnUsbSender::NewL( *this, aLdd );

    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_CONSTRUCTL_EXIT, "CUsbPnIsaReceiver::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::ConstructL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnIsaReceiver* CUsbPnIsaReceiver::NewL( RIscApi& aIscApi, RDevUsbcClient& aLdd )
    {
    OstTraceExt2( TRACE_NORMAL, CUSBPNISARECEIVER_NEWL_ENTRY, "CUsbPnIsaReceiver::NewL;aIscApi=%x;aLdd=%x", ( TUint )&( aIscApi ), ( TUint )&( aLdd ) );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::NewL( aIscApi:0x%x, aLdd:0x%x )" ), &aIscApi, &aLdd ) );

    CUsbPnIsaReceiver* self = new( ELeave ) CUsbPnIsaReceiver( aIscApi );

    CleanupStack::PushL( self );
    self->ConstructL( aLdd );
    CleanupStack::Pop( self );

    OstTrace1( TRACE_NORMAL, CUSBPNISARECEIVER_NEWL_EXIT, "CUsbPnIsaReceiver::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::NewL() - return 0x%x" ), self ) );
    return self;
    }

// Destructor
CUsbPnIsaReceiver::~CUsbPnIsaReceiver()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_CUSBPNISARECEIVER_DESTRUCTOR_ENTRY, "CUsbPnIsaReceiver::~CUsbPnIsaReceiver" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::~CUsbPnIsaReceiver()" ) ) );

    Cancel();
    if( iUsbSender )
        {
        delete iUsbSender;
        iUsbSender = NULL;
        }

    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_CUSBPNISARECEIVER_DESTRUCTOR_EXIT, "CUsbPnIsaReceiver::~CUsbPnIsaReceiver - return" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::~CUsbPnIsaReceiver() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::DoCancel
// -----------------------------------------------------------------------------
//
void CUsbPnIsaReceiver::DoCancel( )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_DOCANCEL_ENTRY, "CUsbPnIsaReceiver::DoCancel" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::DoCancel()" ) ) );

    iIscApi.ReceiveCancel();

    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_DOCANCEL_EXIT, "CUsbPnIsaReceiver::DoCancel - return void" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::DoCancel() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::RunL
// -----------------------------------------------------------------------------
//
void CUsbPnIsaReceiver::RunL( )
    {
    OstTrace1( TRACE_API, CUSBPNISARECEIVER_RUNL_ENTRY, "CUsbPnIsaReceiver::RunL;iStatus=%d", iStatus.Int() );
    A_TRACE( ( _T( "CUsbPnIsaReceiver::RunL() iStatus:%d" ), iStatus.Int() ) );

    User::LeaveIfError(iStatus.Int());

    iRecvPtr.Set( iPacket->Buffer().Des() );

#ifndef NCP_COMMON_BRIDGE_FAMILY
    //Media specific resp from SOS USB MM
    if( (iRecvPtr[ISI_HEADER_OFFSET_RESOURCEID] == PN_MEDIA_CONTROL) &&
            (iRecvPtr[ISI_HEADER_OFFSET_MESSAGEID] == PNS_MEDIA_SPECIFIC_RESP) )
        {
        OstTrace0( TRACE_API, CUSBPNISARECEIVER_RUNL, "CUsbPnIsaReceiver::RunL - Media specific resp" );
        A_TRACE( ( _T( "CUsbPnIsaReceiver::RunL - Media specific resp" )));
        iPacket->ReleaseL();
        ReceivingNextPacket();
        }
    else
#endif /* NCP_COMMON_BRIDGE_FAMILY */
        {
        //Normal case
        if( iForwardFlag )
            {
            ConstructMessage();
            iUsbSender->AddPacketToSendingQueue( *iPacket );
            Receive( iForwardFlag );
            }

        // PC phonet connection not open
        else
            {
            OstTrace0( TRACE_API, CUSBPNISARECEIVER_RUNL_DUP2, "CUsbPnIsaReceiver::RunL - Discarding" );
            A_TRACE( ( _T( "CUsbPnIsaReceiver::RunL - Discarding" )));
            iPacket->ReleaseL();
            ReceivingNextPacket();
            }
        }

    OstTrace0( TRACE_API, CUSBPNISARECEIVER_RUNL_EXIT, "CUsbPnIsaReceiver::RunL - return void" );
    A_TRACE( ( _T( "CUsbPnIsaReceiver::RunL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::ReceivingNextPacket
// -----------------------------------------------------------------------------
//
void CUsbPnIsaReceiver::ReceivingNextPacket()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_RECEIVINGNEXTPACKET_ENTRY, "CUsbPnIsaReceiver::ReceivingNextPacket" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::ReceivingNextPacket()" ) ) );
    iRecvPtr.Set( iPacket->Buffer().Des() );
    iIscApi.Receive( iStatus, iRecvPtr, iNeededLength );
    OstTrace0( TRACE_DETAILED, CUSBPNISARECEIVER_RECEIVINGNEXTPACKET_DUP1, "CUsbPnIsaReceiver::ReceivingNextPacket - receiving new" );
    E_TRACE((_T("CUsbPnIsaReceiver::ReceivingNextPacket - Receiving new")));
    SetActive();
    OstTrace0( TRACE_API, CUSBPNISARECEIVER_RECEIVINGNEXTPACKET_EXIT, "CUsbPnIsaReceiver::ReceivingNextPacket - return void" );
    A_TRACE( ( _T( "CUsbPnIsaReceiver::ReceivingNextPacket() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::RunError
// -----------------------------------------------------------------------------
//
TInt CUsbPnIsaReceiver::RunError( TInt aError )
    {
    OstTrace1( TRACE_API, CUSBPNISARECEIVER_RUNERROR_ENTRY, "CUsbPnIsaReceiver::RunError;aError=%d", aError );
    A_TRACE( ( _T( "CUsbPnIsaReceiver::RunError( aError:%d )" ), aError ) );

    switch( aError )
        {
        case KErrNoMemory:
            {
            OstTrace0( TRACE_DETAILED, CUSBPNISARECEIVER_RUNERROR, "CUsbPnIsaReceiver::RunError - Release packet buffer and alloc bigger for temporary use" );
            E_TRACE( ( _T( "CUsbPnIsaReceiver::RunError - Release packet buffer and alloc bigger for temporary use" )));
            aError = KErrNone;
            TRAP( aError, iPacket->ReallocBufferL( iNeededLength ) );
            if( aError == KErrNone )
                {
                ReceivingNextPacket();
                }
            break;
            }
        case KErrOverflow:
            {
            OstTrace0( TRACE_DETAILED, CUSBPNISARECEIVER_RUNERROR_DUP2, "CUsbPnIsaReceiver::RunError - Release ISC buffer." );
            E_TRACE( ( _T( "CUsbPnIsaReceiver::RunError - Release ISC buffer." )));
            iIscApi.ResetBuffers(); // Connection was probably down, so the old messages are not needed
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Panic( KUsbPnPanicCat, aError );
            break;
            }
        }

    OstTrace1( TRACE_NORMAL, CUSBPNISARECEIVER_RUNERROR_EXIT, "CUsbPnIsaReceiver::RunError - return;aError=%d", aError );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::RunError() return - aError:%d )" ), aError ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::Receive
// aForward should be ETrue only when Phonet connection is open to PC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnIsaReceiver::Receive( TBool aForward )
    {
    OstTrace1( TRACE_NORMAL, CUSBPNISARECEIVER_RECEIVE_ENTRY, "CUsbPnIsaReceiver::Receive;aForward=%d", aForward );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::Receive( aForward:%d)" ), aForward ));

    SetForwardFlag( aForward );

    if(!IsActive())
        {
        TRAPD( err, iPacket = &iUsbSender->GetNextPacketL() );
        if(!err)
            {
            ReceivingNextPacket();
            }
        else
            {
            OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_RECEIVE_DUP1, "CUsbPnIsaReceiver::Receive - Sender tells when to continue" );
            C_TRACE( ( _T( "CUsbPnIsaReceiver::Receive - Sender tells when to continue" )) );
            }
        }

    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_RECEIVE_EXIT, "CUsbPnIsaReceiver::Receive - return void" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::Receive() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::SetForwardFlag
// -----------------------------------------------------------------------------
//
void CUsbPnIsaReceiver::SetForwardFlag( TBool aForward )
    {
    OstTrace1( TRACE_NORMAL, CUSBPNISARECEIVER_SETFORWARDFLAG_ENTRY, "CUsbPnIsaReceiver::SetForwardFlag=%d", aForward );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::SetForwardFlag( aForward:%d)" ), aForward ));

    iForwardFlag = aForward;

    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_SETFORWARDFLAG_EXIT, "CUsbPnIsaReceiver::SetForwardFlag - return void" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::SetForwardFlag() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaReceiver::ConstructMessage
// -----------------------------------------------------------------------------
//
void CUsbPnIsaReceiver::ConstructMessage()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_CONSTRUCTMESSAGE_ENTRY, "CUsbPnIsaReceiver::ConstructMessage" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::ConstructMessage()" ) ) );

    OstTrace1( TRACE_DETAILED, CUSBPNISARECEIVER_CONSTRUCTMESSAGE, "CUsbPnIsaReceiver::ConstructMessage - Convert endianness;iRecvPtr.Length()=%d", iRecvPtr.Length() );
    E_TRACE( ( _T( "CUsbPnIsaReceiver::ConstructMessage() - Convert endianness - iRecvPtr.Length():%d" ), iRecvPtr.Length() ) );
#ifdef ISI_LENGTH_LITTLE_ENDIAN
    TUint8 lsb(iRecvPtr[ISI_HEADER_OFFSET_LENGTH]);
    TUint8 msb(iRecvPtr[ISI_HEADER_OFFSET_LENGTH +1]);
    iRecvPtr[ISI_HEADER_OFFSET_LENGTH] = msb;
    iRecvPtr[ISI_HEADER_OFFSET_LENGTH +1] = lsb;
#endif // ISI_LENGTH_LITTLE_ENDIAN
    OstTrace0( TRACE_NORMAL, CUSBPNISARECEIVER_CONSTRUCTMESSAGE_EXIT, "CUsbPnIsaReceiver::ConstructMessage - return void" );
    C_TRACE( ( _T( "CUsbPnIsaReceiver::ConstructMessage() - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
