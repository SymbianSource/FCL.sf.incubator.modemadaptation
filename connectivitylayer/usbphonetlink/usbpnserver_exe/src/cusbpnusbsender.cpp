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
#include "cusbpnusbsender.h"
#include "cusbpnpacket.h"
#include "musbpnbufferlistener.h"
#include "usbpndefinitions.h"       // For constants
#include "usbpntrace.h"

#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnusbsenderTraces.h"
#endif

#include <d32usbc.h>                // RDevUsbcClient

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::CUsbPnUsbSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnUsbSender::CUsbPnUsbSender( MUsbPnBufferListener& aListener, RDevUsbcClient& aLdd )
    :CActive( 100 )  //ECommTransmitPriority=100 in es_prot.h
    ,iBufferListener( aListener )
    ,iLdd( aLdd )
    ,iPacketCount( 0 )
    ,iPacket( NULL )
    {
    OstTraceExt2( TRACE_NORMAL, CUSBPNUSBSENDER_CUSBPNUSBSENDER_ENTRY, "CUsbPnUsbSender::CUsbPnUsbSender;aListener=%x;aLdd=%x", ( TUint )&( aListener ), ( TUint )&( aLdd ) );
    C_TRACE( ( _T( "CUsbPnUsbSender::CUsbPnUsbSender( aListener:0x%x, aLdd:0x%x )" ), &aListener, &aLdd ) );

    CActiveScheduler::Add(this);

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_CUSBPNUSBSENDER_EXIT, "CUsbPnUsbSender::CUsbPnUsbSender - return" );
    C_TRACE( ( _T( "CUsbPnUsbSender::CUsbPnUsbSender() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnUsbSender::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_CONSTRUCTL_ENTRY, "CUsbPnUsbSender::ConstructL" );
    C_TRACE( ( _T( "CUsbPnUsbSender::ConstructL()" ) ) );

    // Create circular buffer objects
    for(TUint i = 1; i < (KPnUsbPacketCount + 1); i++)
        {
        AddPacketBufferL(i);
        }
    iCurrentPacket = iPacket;

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_CONSTRUCTL_EXIT, "CUsbPnUsbSender::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnUsbSender::ConstructL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnUsbSender* CUsbPnUsbSender::NewL( MUsbPnBufferListener& aBufferListener, RDevUsbcClient& aLdd )
    {
    OstTraceExt2( TRACE_NORMAL, CUSBPNUSBSENDER_NEWL_ENTRY, "CUsbPnUsbSender::NewL;aBufferListener=%x;aLdd=%x", ( TUint )&( aBufferListener ), ( TUint )&( aLdd ) );
    C_TRACE( ( _T( "CUsbPnUsbSender::NewL( aBufferListener:0x%x, aLdd:0x%x )" ), &aBufferListener, &aLdd ) );

    CUsbPnUsbSender* self = new( ELeave ) CUsbPnUsbSender( aBufferListener, aLdd );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    OstTrace1( TRACE_NORMAL, CUSBPNUSBSENDER_NEWL_EXIT, "CUsbPnUsbSender::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnUsbSender::NewL() - return 0x%x" ), self ) );
    return self;
    }


// Destructor
CUsbPnUsbSender::~CUsbPnUsbSender()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_CUSBPNUSBSENDER_DESTRUCTOR_ENTRY, "CUsbPnUsbSender::~CUsbPnUsbSender" );
    C_TRACE( ( _T( "CUsbPnUsbSender::~CUsbPnUsbSender()" ) ) );

    Cancel();

    // Delete circular buffer objects
    CUsbPnPacket* packet;
    while(iPacketCount > 0)
        {
        iPacketCount--;
        packet = &iPacket->NextPacket();
        delete iPacket;
        iPacket = packet;
        }
    iCurrentPacket = NULL;
    iPacket = NULL;

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_CUSBPNUSBSENDER_DESTRUCTOR_EXIT, "CUsbPnUsbSender::~CUsbPnUsbSender - return" );
    C_TRACE( ( _T( "CUsbPnUsbSender::~CUsbPnUsbSender() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::AddPacketBufferL
// -----------------------------------------------------------------------------
void CUsbPnUsbSender::AddPacketBufferL( TInt aIndex )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_ADDPACKETBUFFERL_ENTRY, "CUsbPnUsbSender::AddPacketBufferL" );
    C_TRACE((_T("CUsbPnUsbSender::AddPacketBuffer()")));

    iPacketCount++;
    iPacket = CUsbPnPacket::NewL( iPacket, aIndex );

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_ADDPACKETBUFFERL_EXIT, "CUsbPnUsbSender::AddPacketBufferL - return void" );
    C_TRACE((_T("CUsbPnUsbSender::AddPacketBuffer - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::PacketL
// -----------------------------------------------------------------------------
CUsbPnPacket& CUsbPnUsbSender::PacketL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_PACKETL_ENTRY, "CUsbPnUsbSender::PacketL" );
    C_TRACE((_T("CUsbPnUsbSender::PacketL()")));

    CUsbPnPacket& packet( iPacket->NextPacket() );
    TBool err = packet.PacketInUse();
    if (EFalse != err)
        {
        User::Leave( err );
        }

    OstTrace1( TRACE_NORMAL, CUSBPNUSBSENDER_PACKETL_EXIT, "CUsbPnUsbSender::PacketL - return;packet=%d", packet.PacketNumber() );
    C_TRACE((_T("CUsbPnUsbSender::PacketL() - return packet:%d"), packet.PacketNumber()));

    return packet;
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::Send
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbSender::Send( CUsbPnPacket& aPacket )
    {
    OstTrace0( TRACE_API, CUSBPNUSBSENDER_SEND_ENTRY, "CUsbPnUsbSender::Send" );
    A_TRACE( ( _T( "CUsbPnUsbSender::Send()" ) ) );

    iPacket = &iPacket->NextPacket();
    OstTrace1( TRACE_DETAILED, CUSBPNUSBSENDER_SEND, "CUsbPnUsbSender::Send;packet number:%d", iPacket->PacketNumber() );
    E_TRACE( ( _T( "CUsbPnUsbSender::Send() - packet number:%d" ), iPacket->PacketNumber() ) );

    TryToSendPacket( *iPacket );

    OstTrace0( TRACE_API, CUSBPNUSBSENDER_SEND_EXIT, "CUsbPnUsbSender::Send - return void" );
    A_TRACE( ( _T( "CUsbPnUsbSender::Send() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::TryToSendPacket
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbSender::TryToSendPacket( const CUsbPnPacket& aPacket )
    {
    OstTraceExt2( TRACE_API, CUSBPNUSBSENDER_TRYTOSENDPACKET_ENTRY, "CUsbPnUsbSender::TryToSendPacket;aPacket=%d;iCurrentPacket=%d", aPacket.PacketNumber(), iCurrentPacket->PacketNumber() );
    A_TRACE( ( _T( "CUsbPnUsbSender::TryToSendPacket( aPacketNumber:%d, iCurrentPacket:%d )" ), aPacket.PacketNumber(), iCurrentPacket->PacketNumber( ) ));

    // Write message to USB
    if(!IsActive())
        {
        if( &aPacket == &iCurrentPacket->NextPacket() || &aPacket == iCurrentPacket )
            {
            OstTrace0( TRACE_DETAILED, CUSBPNUSBSENDER_TRYTOSENDPACKET, "CUsbPnUsbSender::TryToSendPacket - Write to socket" );
            E_TRACE( ( _T( "CUsbPnUsbSender::TryToSendPacket() - Write to socket")));

            HBufC8& data(aPacket.Buffer());
            TInt length( data.Length() );
#ifdef EXTENDED_TRACE_FLAG
            TInt i(0);

            while(i < length )
                {
                OstTraceExt2( TRACE_DETAILED, CUSBPNUSBSENDER_TRYTOSENDPACKET_DUP1, "CUsbPnUsbSender::Send( [%d] = %x )", i, data[i] );
                E_TRACE( ( _T( "CUsbPnUsbSender::Send([%d] = %x )" ), i, data[i] ) );
                i++;
                }
#endif

            // ZLP flag setting is optional. Could be always true as driver checks against max packet size
            TBool zlp( ( length != 0 ) && ( length % KPnPacketSize ) == 0 );

            iLdd.Write( iStatus, EEndpoint1, data, length, zlp );
            SetActive();
            }
        else
            {
            TRACE_ASSERT_ALWAYS;
            }
        }
    else
        {
        OstTrace0( TRACE_DETAILED, CUSBPNUSBSENDER_TRYTOSENDPACKET_DUP2, "CUsbPnUsbSender::TryToSendPacket - Already sending" );
        E_TRACE( ( _T( "CUsbPnUsbSender::TryToSendPacket() - Already sending")));
        }

    OstTrace0( TRACE_API, CUSBPNUSBSENDER_TRYTOSENDPACKET_EXIT, "CUsbPnUsbSender::TryToSendPacket - return void" );
    A_TRACE( ( _T( "CUsbPnUsbSender::TryToSendPacket() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbSender::DoCancel( )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_DOCANCEL_ENTRY, "CUsbPnUsbSender::DoCancel" );
    C_TRACE( ( _T( "CUsbPnUsbSender::DoCancel()" ) ) );
    iLdd.WriteCancel( EEndpoint1 );
    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_DOCANCEL_EXIT, "CUsbPnUsbSender::DoCancel - return void" );
    C_TRACE( ( _T( "CUsbPnUsbSender::DoCancel() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbSender::RunL( )
    {
    OstTrace1( TRACE_API, CUSBPNUSBSENDER_RUNL_ENTRY, "CUsbPnUsbSender::RunL;iStatus:%d", iStatus.Int() );
    A_TRACE( ( _T( "CUsbPnUsbSender::RunL() iStatus:%d" ), iStatus.Int() ) );

    User::LeaveIfError( iStatus.Int() );

    iCurrentPacket->ReleaseL();
    iBufferListener.Receive( ETrue );
    iCurrentPacket = &iCurrentPacket->NextPacket();

    if(iCurrentPacket != iPacket)
        {
        // Continue write
        OstTraceExt2( TRACE_DETAILED, CUSBPNUSBSENDER_RUNL, "CUsbPnUsbSender::RunL - Write next in queue;tail=%d;head=%d", iCurrentPacket->PacketNumber(), iPacket->PacketNumber() );
        E_TRACE( ( _T( "CUsbPnUsbSender::RunL() - Write next in queue tail:%d,head:%d " ), iCurrentPacket->PacketNumber(), iPacket->PacketNumber()));
        TryToSendPacket( iCurrentPacket->NextPacket() );
        }

    OstTrace0( TRACE_API, CUSBPNUSBSENDER_RUNL_EXIT, "CUsbPnUsbSender::RunL - return void" );
    A_TRACE( ( _T( "CUsbPnUsbSender::RunL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::RunError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUsbPnUsbSender::RunError( TInt aError )
    {
    OstTrace1( TRACE_API, CUSBPNUSBSENDER_RUNERROR_ENTRY, "CUsbPnUsbSender::RunError;aError=%d", aError );
    A_TRACE( ( _T( "CUsbPnUsbSender::RunError(aError:%d)" ), aError ) );

    switch( aError )
        {
        case KErrUsbCableDetached:
        case KErrUsbDeviceBusReset:
        case KErrUsbInterfaceNotReady:
        case KErrUsbEpNotInInterface:
        case KErrUsbDeviceNotConfigured:
        case KErrUsbBadEndpoint:
        case KErrUsbDeviceClosing:
        case KErrUsbInterfaceChange:
        case KErrUsbEpNotReady:
            {
            OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_RUNERROR, "CUsbPnUsbSender::RunError - Cable detached!" );
            C_TRACE( ( _T( "CUsbPnUsbSender::RunError - Cable detached!" )));
            aError = KErrNone;
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;
            }
        }

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_RUNERROR_EXIT, "CUsbPnUsbSender::RunError - return void" );
    C_TRACE( ( _T( "CUsbPnUsbSender::RunL() - return void" ) ) );
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
