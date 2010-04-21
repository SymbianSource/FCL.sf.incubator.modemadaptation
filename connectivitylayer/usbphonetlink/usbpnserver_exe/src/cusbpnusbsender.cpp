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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnusbsenderTraces.h"
#endif

#include <d32usbc.h>                // RDevUsbcClient

// LOCAL CONSTANTS AND MACROS
const TInt KTotalUsbPacketCount( KPnUsbPacketCount + 1 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::CUsbPnUsbSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnUsbSender::CUsbPnUsbSender( MUsbPnBufferListener& aListener, 
    RDevUsbcClient& aLdd )
    :CActive( 100 )  //ECommTransmitPriority=100 in es_prot.h
    ,iBufferListener( aListener )
    ,iLdd( aLdd )
    ,iPacketCount( NULL )
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

    // Create circular buffer objects, start count in 1, otherwise one packet is not deleted in destructor
    for(TUint i = 1; i < KTotalUsbPacketCount; i++)
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
    CleanupStack::Pop( self );

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
    DeletePackets();
    
    iCurrentPacket = NULL;
    iPacket = NULL;

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_CUSBPNUSBSENDER_DESTRUCTOR_EXIT, "CUsbPnUsbSender::~CUsbPnUsbSender - return" );
    C_TRACE( ( _T( "CUsbPnUsbSender::~CUsbPnUsbSender() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::DeletePackets
// -----------------------------------------------------------------------------
void CUsbPnUsbSender::DeletePackets( )
    {
    OstTrace0( TRACE_API, CUSBPNUSBSENDER_DELETEPACKETS_ENTRY, "CUsbPnUsbSender::DeletePackets" );
    A_TRACE( ( _T( "CUsbPnUsbSender::DeletePackets()" ) ) );    
    CUsbPnPacket* packet = NULL;
    while(iPacketCount > 0)
        {
        iPacketCount--;
        packet = &iPacket->NextPacket();
        delete iPacket;
        iPacket = packet;
        }
    OstTrace0( TRACE_API, CUSBPNUSBSENDER_DELETEPACKETS_EXIT, "CUsbPnUsbSender::DeletePackets - return void" );
    A_TRACE( ( _T( "CUsbPnUsbSender::DeletePackets() - return void" ) ) );        
    }            

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::AddPacketBufferL
// -----------------------------------------------------------------------------
void CUsbPnUsbSender::AddPacketBufferL( const TInt aIndex )
    {
    OstTrace1( TRACE_NORMAL, CUSBPNUSBSENDER_ADDPACKETBUFFERL_ENTRY, "CUsbPnUsbSender::AddPacketBufferL aIndex %d", aIndex );
    C_TRACE((_T("CUsbPnUsbSender::AddPacketBuffer() aIndex %d"), aIndex));
        
    if( aIndex <= KTotalUsbPacketCount )
        {
        iPacketCount++;
        iPacket = CUsbPnPacket::NewL( iPacket, aIndex );
        }       
    else
        {
        TRACE_ASSERT_ALWAYS;
        }

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_ADDPACKETBUFFERL_EXIT, "CUsbPnUsbSender::AddPacketBufferL - return void" );
    C_TRACE((_T("CUsbPnUsbSender::AddPacketBuffer - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::GetNextPacketL
// -----------------------------------------------------------------------------
CUsbPnPacket& CUsbPnUsbSender::GetNextPacketL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_GETNEXTPACKETL_ENTRY, "CUsbPnUsbSender::GetNextPacketL" );
    C_TRACE((_T("CUsbPnUsbSender::PacketL()")));

    CUsbPnPacket& nextPacket( iPacket->NextPacket() );
    TBool err = nextPacket.PacketInUse();
    if (EFalse != err)
        {
        User::Leave( err );
        }
    OstTrace1( TRACE_NORMAL, CUSBPNUSBSENDER_GETNEXTPACKETL_EXIT, "CUsbPnUsbSender::GetNextPacketL - return;packet=%d", nextPacket.PacketNumber() );
    C_TRACE((_T("CUsbPnUsbSender::GetNextPacketL() - return packet:%d"), nextPacket.PacketNumber()));

    return nextPacket;
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::AddPacketToSendingQueue
// -----------------------------------------------------------------------------
//
void CUsbPnUsbSender::AddPacketToSendingQueue( CUsbPnPacket& aPacket )
    {
    ASSERT( &aPacket );
    OstTrace0( TRACE_API, CUSBPNUSBSENDER_ADDPACKETTOSENDINGQUEUE_ENTRY, "CUsbPnUsbSender::AddPacketToSendingQueue" );
    A_TRACE( ( _T( "CUsbPnUsbSender::Send()" ) ) );

    iPacket = &iPacket->NextPacket();
    OstTrace1( TRACE_DETAILED, CUSBPNUSBSENDER_ADDPACKETTOSENDINGQUEUE, "CUsbPnUsbSender::AddPacketToSendingQueue;packet number:%d", iPacket->PacketNumber() );
    E_TRACE( ( _T( "CUsbPnUsbSender::AddPacketToSendingQueue() - packet number:%d" ), iPacket->PacketNumber() ) );

    TryToSendPacket( *iPacket );

    OstTrace0( TRACE_API, CUSBPNUSBSENDER_ADDPACKETTOSENDINGQUEUE_EXIT, "CUsbPnUsbSender::AddPacketToSendingQueue - return void" );
    A_TRACE( ( _T( "CUsbPnUsbSender::AddPacketToSendingQueue() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::TryToSendPacket
// -----------------------------------------------------------------------------
//
void CUsbPnUsbSender::TryToSendPacket( const CUsbPnPacket& aPacket )
    {
    ASSERT( &aPacket );
    OstTraceExt2( TRACE_API, CUSBPNUSBSENDER_TRYTOSENDPACKET_ENTRY, "CUsbPnUsbSender::TryToSendPacket;aPacket=0x%x;iCurrentPacket=%d", ( TUint )&( aPacket ), iCurrentPacket->PacketNumber() );
    A_TRACE( ( _T( "CUsbPnUsbSender::TryToSendPacket( aPacketNumber:0x%x, iCurrentPacket:%d )" ), &aPacket, iCurrentPacket->PacketNumber( ) ));

    // Write message to USB
    if(!IsActive())
        {
        if( &aPacket == ( &iCurrentPacket->NextPacket() )
            || &aPacket == iCurrentPacket )
            {
            OstTrace0( TRACE_DETAILED, CUSBPNUSBSENDER_TRYTOSENDPACKET, "CUsbPnUsbSender::TryToSendPacket - Write to socket" );
            E_TRACE( ( _T( "CUsbPnUsbSender::TryToSendPacket() - Write to socket")));

            HBufC8& data(aPacket.Buffer());
            TInt length( data.Length() );
#ifdef EXTENDED_TRACE_FLAG
            for(TInt i = 0; i < length; i++)
                {
                OstTraceExt2( TRACE_DETAILED, CUSBPNUSBSENDER_TRYTOSENDPACKET_DUP1, "CUsbPnUsbSender::Send( [%d] = %x )", i, data[i] );
                E_TRACE( ( _T( "CUsbPnUsbSender::Send([%d] = %x )" ), i, data[i] ) );
                i++;
                }
#endif

            // ZLP flag setting is optional. Could be always true as driver checks against max packet size
            TBool zlp( ETrue );
            if( length != 0 )
                {
                zlp = ( ( length != 0 ) && 
                ( length % KPnPacketSize ) == 0 );
                }
				
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
        OstTraceExt2( TRACE_DETAILED, CUSBPNUSBSENDER_RUNL, "CUsbPnUsbSender::RunL - Write next in queue;tail=%d;head=%d", iCurrentPacket->PacketNumber(), iPacket->PacketNumber() );
        E_TRACE( ( _T( "CUsbPnUsbSender::RunL() - Write next in queue tail:%d,head:%d " ), iCurrentPacket->PacketNumber(), iPacket->PacketNumber()));
        TryToSendPacket( iCurrentPacket->NextPacket() );
        }
    OstTrace0( TRACE_API, CUSBPNUSBSENDER_RUNL_EXIT, "CUsbPnUsbSender::RunL - return void" );
    A_TRACE( ( _T( "CUsbPnUsbSender::RunL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbSender::RunError
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
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;
            break;     
            }
        }

    OstTrace0( TRACE_NORMAL, CUSBPNUSBSENDER_RUNERROR_EXIT, "CUsbPnUsbSender::RunError - return" );
    C_TRACE( ( _T( "CUsbPnUsbSender::RunError() - return" ) ) );
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
