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
#include "cusbpnisasender.h"
#include "cusbpnpacket.h"
#include "musbpnbufferlistener.h"
#include "usbpndefinitions.h"
#include "usbpntrace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnisasenderTraces.h"
#endif

#include <iscapi.h>                 // For RIscApi

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
// CUsbPnIsaSender::CUsbPnIsaSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnIsaSender::CUsbPnIsaSender( MUsbPnBufferListener& aBufferListener, RIscApi& aIscApi  )
    :CActive( CActive::EPriorityStandard )
    ,iBufferListener( aBufferListener )
    ,iIscApi( aIscApi )
    ,iPacketCount( 0 )
    {
    OstTraceExt2( TRACE_NORMAL, CUSBPNISASENDER_CUSBPNISASENDER_ENTRY, "CUsbPnIsaSender::CUsbPnIsaSender;aBufferListener=%x;aIscApi=%x", ( TUint )&( aBufferListener ), ( TUint )&( aIscApi ) );
    C_TRACE( ( _T( "CUsbPnIsaSender::CUsbPnIsaSender( aBufferListener:0x%x, aIscApi:0x%x )" ), &aBufferListener, &aIscApi ) );

    CActiveScheduler::Add(this);

    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_CUSBPNISASENDER_EXIT, "CUsbPnIsaSender::CUsbPnIsaSender - return" );
    C_TRACE( ( _T( "CUsbPnIsaSender::CUsbPnIsaSender() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaSender::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnIsaSender::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_CONSTRUCTL_ENTRY, "CUsbPnIsaSender::ConstructL" );
    C_TRACE( ( _T( "CUsbPnIsaSender::ConstructL()" ) ) );

    // Create circular buffer objects
    for(TUint i = 1; i < (KPnIsaPacketCount + 1); i++)
        {
        AddPacketBufferL(i);
        }
    iCurrentPacket = iPacket;

    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_CONSTRUCTL_EXIT, "CUsbPnIsaSender::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnIsaSender::ConstructL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnIsaSender* CUsbPnIsaSender::NewL( MUsbPnBufferListener& aBufferListener, RIscApi& aIscApi )
    {
    OstTraceExt2( TRACE_NORMAL, CUSBPNISASENDER_NEWL_ENTRY, "CUsbPnIsaSender::NewL;aBufferListener=%x;aIscApi=%x", ( TUint )&( aBufferListener ), ( TUint )&( aIscApi ) );
    C_TRACE( ( _T( "CUsbPnIsaSender::CUsbPnIsaSender( aBufferListener 0x%x, aIscApi:0x%x )" ), &aBufferListener, &aIscApi ) );

    CUsbPnIsaSender* self = new( ELeave ) CUsbPnIsaSender( aBufferListener, aIscApi );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTrace1( TRACE_NORMAL, CUSBPNISASENDER_NEWL_EXIT, "CUsbPnIsaSender::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnIsaSender::CUsbPnIsaSender() - return 0x%x" ), self ) );
    return self;
    }


// Destructor
CUsbPnIsaSender::~CUsbPnIsaSender()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_CUSBPNISASENDER_DESTRUCTOR_ENTRY, "CUsbPnIsaSender::~CUsbPnIsaSender" );
    C_TRACE( ( _T( "CUsbPnIsaSender::~CUsbPnIsaSender()" ) ) );

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

    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_CUSBPNISASENDER_DESTRUCTOR_EXIT, "CUsbPnIsaSender::~CUsbPnIsaSender - return" );
    C_TRACE( ( _T( "CUsbPnIsaSender::~CUsbPnIsaSender() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaSender::AddPacketBuffer
// -----------------------------------------------------------------------------
void CUsbPnIsaSender::AddPacketBufferL( TInt aIndex )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_ADDPACKETBUFFERL_ENTRY, "CUsbPnIsaSender::AddPacketBufferL" );
    C_TRACE((_T("CUsbPnIsaSender::AddPacketBuffer()")));

    iPacketCount++;
    iPacket = CUsbPnPacket::NewL( iPacket, aIndex );

    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_ADDPACKETBUFFERL_EXIT, "CUsbPnIsaSender::AddPacketBufferL - return void" );
    C_TRACE((_T("CUsbPnIsaSender::AddPacketBuffer - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaSender::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnIsaSender::DoCancel( )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_DOCANCEL_ENTRY, "CUsbPnIsaSender::DoCancel" );
    C_TRACE( ( _T( "CUsbPnIsaSender::DoCancel()" ) ) );

    iIscApi.SendCancel();

    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_DOCANCEL_EXIT, "CUsbPnIsaSender::DoCancel - return void" );
    C_TRACE( ( _T( "CUsbPnIsaSender::DoCancel() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaSender::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnIsaSender::RunL( )
    {
    OstTrace1( TRACE_API, CUSBPNISASENDER_RUNL_ENTRY, "CUsbPnIsaSender::RunL;iStatus=%d", iStatus.Int() );
    A_TRACE( ( _T( "CUsbPnIsaSender::RunL() iStatus:%d" ), iStatus.Int() ) );

    User::LeaveIfError( iStatus.Int() );

    iCurrentPacket->ReleaseL();

    iBufferListener.Receive( ETrue/*dummy*/ );

    iCurrentPacket = &iCurrentPacket->NextPacket();
    OstTraceExt2( TRACE_DETAILED, CUSBPNISASENDER_RUNL, "CUsbPnIsaSender::RunL;tail:%d,head:%d", iCurrentPacket->PacketNumber(), iPacket->PacketNumber() );
    E_TRACE( ( _T( "CUsbPnIsaSender::RunL() - tail:%d,head:%d " ), iCurrentPacket->PacketNumber(), iPacket->PacketNumber()));

    if(iCurrentPacket != iPacket)
        {
        // Continue write
        OstTrace0( TRACE_DETAILED, CUSBPNISASENDER_RUNL_DUP1, "CUsbPnIsaSender::RunL - Write next in queue." );
        E_TRACE( ( _T( "CUsbPnIsaSender::RunL() - Write next in queue." )));
        TryToSendPacket( iCurrentPacket->NextPacket() );
        }

    OstTrace0( TRACE_API, CUSBPNISASENDER_RUNL_EXIT, "CUsbPnIsaSender::RunL - return void" );
    A_TRACE( ( _T( "CUsbPnIsaSender::RunL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaSender::RunError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUsbPnIsaSender::RunError( TInt aError )
    {
    OstTrace1( TRACE_API, CUSBPNISASENDER_RUNERROR_ENTRY, "CUsbPnIsaSender::RunError;aError=%d", aError );
    A_TRACE( ( _T( "CUsbPnIsaSender::RunError( aError:%d )" ), aError ) );

    switch( aError )
        {
        case KErrNotReady:
        case KErrBadDescriptor:
        case KErrUnderflow:
            {
            TRACE_ASSERT_ALWAYS;
            CActiveScheduler::Stop();
            aError = KErrNone;
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Panic( KUsbPnPanicCat, aError );
            break;
            }
        }

    OstTrace1( TRACE_NORMAL, CUSBPNISASENDER_RUNERROR_EXIT, "CUsbPnIsaSender::RunError - return;aError=%d", aError );
    C_TRACE( ( _T( "CUsbPnIsaSender::RunError() return - aError:%d )" ), aError ) );
    return aError;
    }
// -----------------------------------------------------------------------------
// CUsbPnIsaSender::Packet
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUsbPnPacket& CUsbPnIsaSender::PacketL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNISASENDER_PACKETL_ENTRY, "CUsbPnIsaSender::PacketL" );
    C_TRACE( ( _T( "CUsbPnIsaSender::PacketL()" ) ) );

    CUsbPnPacket& packet( iPacket->NextPacket() );
    TBool err = packet.PacketInUse();
    if (EFalse != err)
        {
        User::Leave( err );
        }

    OstTrace1( TRACE_NORMAL, CUSBPNISASENDER_PACKETL_EXIT, "CUsbPnIsaSender::PacketL - return;packet=%d", packet.PacketNumber() );
    C_TRACE((_T("CUsbPnIsaSender::PacketL() - return packet:%d"), packet.PacketNumber()));

    return packet;
    }

// -----------------------------------------------------------------------------
// CUsbPnIsaSender::Send
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnIsaSender::Send( CUsbPnPacket& aPacket )
    {
    OstTrace1( TRACE_API, CUSBPNISASENDER_SEND_ENTRY, "CUsbPnIsaSender::Send;aPacket=%x", ( TUint )&( aPacket ) );
    A_TRACE( ( _T( "CUsbPnIsaSender::Send( aPacket:0x%x)" ), &aPacket ) );

    CUsbPnPacket& packet( iPacket->NextPacket() );
    if( &packet == &aPacket )
        {
        iPacket = &packet;
        OstTrace1( TRACE_DETAILED, CUSBPNISASENDER_SEND, "CUsbPnIsaSender::Send;packet number:%d", iPacket->PacketNumber() );
        E_TRACE( ( _T( "CUsbPnIsaSender::Send() - packet number:%d" ), iPacket->PacketNumber() ) );

        TryToSendPacket( *iPacket );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        User::Panic(_L("USBPNSERVER"), KErrTotalLossOfPrecision);
        }

    OstTrace0( TRACE_API, CUSBPNISASENDER_SEND_EXIT, "CUsbPnIsaSender::Send - return void" );
    A_TRACE( ( _T( "CUsbPnIsaSender::Send() - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CUsbPnIsaSender::TryToSendPacket
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnIsaSender::TryToSendPacket( CUsbPnPacket& aPacket )
    {
    OstTrace1( TRACE_API, CUSBPNISASENDER_TRYTOSENDPACKET_ENTRY, "CUsbPnIsaSender::TryToSendPacket;aPacket=%x", ( TUint )&( aPacket ) );
    A_TRACE( ( _T( "CUsbPnIsaSender::TryToSendPacket( aPacket:0x%x )" ), &aPacket ) );

    if(!IsActive())
        {
        if( &aPacket == &iCurrentPacket->NextPacket() || &aPacket == iCurrentPacket )
            {
            OstTrace0( TRACE_DETAILED, CUSBPNISASENDER_TRYTOSENDPACKET, "CUsbPnIsaSender::TryToSendPacket - Send to ISA" );
            E_TRACE( ( _T( "CUsbPnIsaSender::TryToSendPacket() - Send to ISA")));

#ifdef EXTENDED_TRACE_FLAG
            TInt i(0);
            HBufC8& data(aPacket.Buffer());
            while(i < data.Length() )
                {
                OstTraceExt2( TRACE_DETAILED, CUSBPNISASENDER_TRYTOSENDPACKET_DUP1, "CUsbPnIsaSender::Send([%d] = %x )", i, data[i] );
                E_TRACE( ( _T( "CUsbPnIsaSender::Send([%d] = %x )" ), i, data[i] ) );
                i++;
                }
#endif

            iIscApi.Send( iStatus, aPacket.Buffer() );
            SetActive();
            }
        else
            {
            TRACE_ASSERT_ALWAYS;
            User::Panic(_L("USBPNSERVER"), KErrTotalLossOfPrecision);
            }
        }
    else
        {
        OstTrace0( TRACE_DETAILED, CUSBPNISASENDER_TRYTOSENDPACKET_DUP2, "CUsbPnIsaSender::TryToSendPacket - Already sending" );
        E_TRACE( ( _T( "CUsbPnIsaSender::TryToSendPacket() - Already sending")));
        }

    OstTrace0( TRACE_API, CUSBPNISASENDER_TRYTOSENDPACKET_EXIT, "CUsbPnIsaSender::TryToSendPacket - return void" );
    A_TRACE( ( _T( "CUsbPnIsaSender::TryToSendPacket() - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
