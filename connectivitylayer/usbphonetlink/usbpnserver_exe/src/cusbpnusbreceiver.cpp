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
#include "cusbpnusbreceiver.h"
#include "cusbpnisasender.h"        // For sending messages to ISA
#include "cusbpnpacket.h"           // For packet buffers
#include "usbpndefinitions.h"       // For constants
#include "usbpntrace.h"             // For tracing

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnusbreceiverTraces.h"
#endif

#include <d32usbc.h>                // For RDevUsbcClient
#include <pn_const.h>               // For header size
#include <phonetisi.h>              // For Phonet definitions

#ifndef NCP_COMMON_BRIDGE_FAMILY
#include <test_serverisi.h>         // For APE Test Server message ID
#include <iscnokiadefinitions.h>    // For APE Test Server channel ID
#endif /* NCP_COMMON_BRIDGE_FAMILY */

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
// CUsbPnUsbReceiver::CUsbPnUsbReceiver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnUsbReceiver::CUsbPnUsbReceiver( RDevUsbcClient& aLdd )
    :CActive( CActive::EPriorityStandard )
    , iLdd( aLdd )
    , iRecvPtr(0,0)
    , iStorage(0)
    {
    OstTrace1( TRACE_NORMAL, CUSBPNUSBRECEIVER_CUSBPNUSBRECEIVER_ENTRY, "CUsbPnUsbReceiver::CUsbPnUsbReceiver;aLdd=%x", ( TUint )&( aLdd ) );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::CUsbPnUsbReceiver( aLdd:0x%x )" ), &aLdd ) );

    CActiveScheduler::Add(this);

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_CUSBPNUSBRECEIVER_EXIT, "CUsbPnUsbReceiver::CUsbPnUsbReceiver - return" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::CUsbPnUsbReceiver() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnUsbReceiver::ConstructL( RIscApi& aIscApi )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_CONSTRUCTL_ENTRY, "CUsbPnUsbReceiver::ConstructL" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructL()" ) ) );

    iIsaSender = CUsbPnIsaSender::NewL( *this, aIscApi );

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_CONSTRUCTL_EXIT, "CUsbPnUsbReceiver::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnUsbReceiver* CUsbPnUsbReceiver::NewL( RDevUsbcClient& aLdd, RIscApi& aIscApi )
    {
    OstTraceExt2( TRACE_NORMAL, CUSBPNUSBRECEIVER_NEWL_ENTRY, "CUsbPnUsbReceiver::NewL;aLdd=%x;aIscApi=%x", ( TUint )&( aLdd ), ( TUint )&( aIscApi ) );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::NewL( aLdd:0x%x, aIscApi:0x%x )" ), &aLdd, &aIscApi ) );

    CUsbPnUsbReceiver* self = new( ELeave ) CUsbPnUsbReceiver( aLdd );

    CleanupStack::PushL( self );
    self->ConstructL( aIscApi );
    CleanupStack::Pop( self );

    OstTrace1( TRACE_NORMAL, CUSBPNUSBRECEIVER_NEWL_EXIT, "CUsbPnUsbReceiver::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::NewL() - return 0x%x" ), self ) );
    return self;
    }


// Destructor
CUsbPnUsbReceiver::~CUsbPnUsbReceiver()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_CUSBPNUSBRECEIVER_DESTRUCTOR_ENTRY, "CUsbPnUsbReceiver::~CUsbPnUsbReceiver" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::~CUsbPnUsbReceiver()" ) ) );

    Cancel();
    if( iIsaSender )
        {
        delete iIsaSender;
        }

    iPacket = 0;

    if( iStorage )
        {
        delete iStorage;
        }
    iStorage = NULL;

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_CUSBPNUSBRECEIVER_DESTRUCTOR_EXIT, "CUsbPnUsbReceiver::~CUsbPnUsbReceiver - return" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::~CUsbPnUsbReceiver() - return" ) ) );
    }


// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbReceiver::DoCancel( )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_DOCANCEL_ENTRY, "CUsbPnUsbReceiver::DoCancel" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::DoCancel()" ) ) );

    iLdd.ReadCancel( EEndpoint2 );
    TRAPD(err, iPacket->ReleaseL());

    OstTrace1( TRACE_NORMAL, CUSBPNUSBRECEIVER_DOCANCEL, "CUsbPnUsbReceiver::DoCancel - err=%d", err );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::DoCancel() - err=%d" ), err ) );

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_DOCANCEL_EXIT, "CUsbPnUsbReceiver::DoCancel - return void" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::DoCancel() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbReceiver::RunL( )
    {
    OstTrace1( TRACE_API, CUSBPNUSBRECEIVER_RUNL_ENTRY, "CUsbPnUsbReceiver::RunL;iStatus:%d", iStatus.Int() );
    A_TRACE( ( _T( "CUsbPnUsbReceiver::RunL() iStatus:%d" ), iStatus.Int() ) );

    User::LeaveIfError( iStatus.Int() );
    ConstructMessageL();
    if( iMessageComplete )
        {
        iIsaSender->Send( *iPacket );
        iMessageComplete = EFalse;
        iByteCount = 0;
        iMessageLength = 0;

        // Start fresh receive
        Receive( ETrue );
        }

    OstTrace0( TRACE_API, CUSBPNUSBRECEIVER_RUNL_EXIT, "CUsbPnUsbReceiver::RunL - return void" );
    A_TRACE( ( _T( "CUsbPnUsbReceiver::RunL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::RunError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUsbPnUsbReceiver::RunError( TInt aError )
    {
    OstTrace1( TRACE_API, CUSBPNUSBRECEIVER_RUNERROR_ENTRY, "CUsbPnUsbReceiver::RunError;aError=%d", aError );
    A_TRACE( ( _T( "CUsbPnUsbReceiver::RunError(aError:%d)" ), aError ) );

    switch( aError )
        {
        case KErrUsbDriverBase:
        case KErrUsbInterfaceNotReady:
        case KErrUsbEpNotInInterface:
        case KErrUsbEpBadDirection:
        case KErrUsbTransferSize:
        case KErrUsbDeviceNotConfigured:
        case KErrUsbBadEndpoint:
        case KErrUsbDeviceClosing:
        case KErrUsbInterfaceChange:
        case KErrUsbCableDetached:
        case KErrUsbDeviceBusReset:
        case KErrUsbEpNotReady:
            {
            OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_RUNERROR, "CUsbPnUsbReceiver::RunError - Cable detached!" );
            C_TRACE( ( _T( "CUsbPnUsbReceiver::RunError - Cable detached!" )));
            aError = KErrNone;
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;  // Log the errorcase.
            aError = KErrNone; // Ignore error..
            break;
            }
        }

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_RUNERROR_EXIT, "CUsbPnUsbReceiver::RunError - return" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::RunL() - return" ) ) );
    return aError;
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::Receive
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbReceiver::Receive( TBool /*aForward*/ )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_RECEIVE_ENTRY, "CUsbPnUsbReceiver::Receive" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::Receive()" )) );

    // iEnumerated flag is controlled by CUsbPnInterface. ETrue when altset1. EFalse if altset0 or cable disconnected.
    if( iEnumerated )
        {
        if(!IsActive())
            {
            TRAPD( err, iPacket = &iIsaSender->PacketL() );
            if(!err)
                {
                iRecvPtr.Set( iPacket->Buffer().Des() );
                iLdd.ReadUntilShort( iStatus, EEndpoint2, iRecvPtr );
                OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_RECEIVE, "CUsbPnUsbReceiver::Receive - Receiving" );
                C_TRACE( ( _T( "CUsbPnUsbReceiver::Receive - Receiving" )) );
                SetActive();
                }
            else
                {
                // No free packets so receiving not activated until sender
                OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_RECEIVE_DUP1, "CUsbPnUsbReceiver::Receive - Sender tells when to continue" );
                C_TRACE( ( _T( "CUsbPnUsbReceiver::Receive - Sender tells when to continue" )) );
                }
            }
        }

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_RECEIVE_EXIT, "CUsbPnUsbReceiver::Receive - return void" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::Receive() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::IsaSender
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MUsbPnControlObserver* CUsbPnUsbReceiver::IsaSender()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_ISASENDER_ENTRY, "CUsbPnUsbReceiver::IsaSender" );
    OstTrace1( TRACE_NORMAL, CUSBPNUSBRECEIVER_ISASENDER_EXIT, "CUsbPnUsbReceiver::IsaSender - return;MUsbPnControlObserver*=%x", (MUsbPnControlObserver*)iIsaSender );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::IsaSender()")));
    C_TRACE( ( _T( "CUsbPnUsbReceiver::IsaSender() - return MUsbPnControlObserver*: 0x%x"), (MUsbPnControlObserver*)iIsaSender ));

    return (MUsbPnControlObserver*)iIsaSender;
    }
// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::SetEnumerated
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbReceiver::SetEnumerated( TBool aState )
    {
    OstTrace1( TRACE_NORMAL, CUSBPNUSBRECEIVER_SETENUMERATED_ENTRY, "CUsbPnUsbReceiver::SetEnumerated;aState=%d", aState );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::SetEnumerated( aState:%d)"), aState));

    iEnumerated = aState;

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_SETENUMERATED_EXIT, "CUsbPnUsbReceiver::SetEnumerated - return void" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::SetEnumerated() - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnUsbReceiver::ConstructMessage
// Messages larger than max packet size must be constructed from multiple packets
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnUsbReceiver::ConstructMessageL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_ENTRY, "CUsbPnUsbReceiver::ConstructMessageL" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage()" ) ) );

    TUint packetLength( iPacket->Buffer().Length() );
    OstTrace1( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL, "CUsbPnUsbReceiver::ConstructMessageL;packetLength=%d", packetLength );
    E_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage - packetLength:%d" ), packetLength ) );

    // New message
    if( iByteCount == 0 )
        {
        TUint16 lsb(iRecvPtr[ISI_HEADER_OFFSET_LENGTH +1]);
        TUint16 msb(iRecvPtr[ISI_HEADER_OFFSET_LENGTH]);

        iMessageLength = (lsb | (msb<<8));

        OstTrace1( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP1, "CUsbPnUsbReceiver::ConstructMessageL;iMessageLength=%d", iMessageLength );
        E_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage - iMessageLength:%d" ), iMessageLength ) );
        if( iMessageLength >= 4 )
            {
            iByteCount = packetLength;

            OstTrace1( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP2, "CUsbPnUsbReceiver::ConstructMessageLNew message;Received:%d", iByteCount );
            E_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage - New message. Received:%d"), iByteCount));

            if( ( PN_HEADER_SIZE + iMessageLength ) > packetLength )
                {
                // realloc
                OstTrace0( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP3, "CUsbPnUsbReceiver::ConstructMessageL - partial message. Receive more" );
                E_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage - partial message. Receive more")));

                // Alloc temporary buffer to store data
                iStorage = HBufC8::NewL( iMessageLength + PN_HEADER_SIZE );
                TPtr8 tmp( iStorage->Des() );
                tmp.Append( iPacket->Buffer() );
                }
            }
        }

    // Appending to previous message
    else
        {
        iByteCount = iByteCount + packetLength;
        OstTrace1( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP4, "CUsbPnUsbReceiver::ConstructMessageL - Append to previous part;iByteCount:%d", iByteCount );
        E_TRACE((_T("CUsbPnUsbReceiver::ConstructMessage - Append to previous part. iByteCount:%d"), iByteCount));
        TPtr8 tmp( iStorage->Des() );
        tmp.Append( iPacket->Buffer() );
        }

    // Ready to be sent?
    if( iByteCount == iMessageLength + PN_HEADER_SIZE )
        {
        OstTrace0( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP5, "CUsbPnUsbReceiver::ConstructMessageL - message complete" );
        E_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage - message complete")));

        if( iByteCount > KPnPacketSize )
            {
            // Realloc iPacket's buffer with needed length
            OstTrace0( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP6, "CUsbPnUsbReceiver::ConstructMessageL - realloc for whole message" );
            E_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage - realloc for whole message")));
            iPacket->ReallocBufferL( iMessageLength + PN_HEADER_SIZE );
            iRecvPtr.Set( iPacket->Buffer().Des() );
            iRecvPtr.Append( *iStorage );
            delete iStorage;
            iStorage = NULL;
            }

        // Converts the endianess of message length
        TUint8 tmp4(iRecvPtr[ISI_HEADER_OFFSET_LENGTH]);
        TUint8 tmp5(iRecvPtr[ISI_HEADER_OFFSET_LENGTH +1]);
        iRecvPtr[ISI_HEADER_OFFSET_LENGTH] = tmp5;
        iRecvPtr[ISI_HEADER_OFFSET_LENGTH +1] = tmp4;

#ifndef NCP_COMMON_BRIDGE_FAMILY
        // Message directly to APE Test Server. Rare labeling case for optimized performance
        if( (iRecvPtr[ISI_HEADER_OFFSET_RESOURCEID] == PN_EPOC_TEST) &&
            (iRecvPtr[ISI_HEADER_OFFSET_MESSAGEID] == TS_SOS_FILE_WRITE_REQ ) )
            {
            OstTrace0( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP7, "CUsbPnUsbReceiver::ConstructMessageL - Message to APE Test Server" );
            E_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage - Message to APE Test Server")));

            // Set Receiver device
            //TUint8 recvDev = iRecvPtr[ISI_HEADER_OFFSET_RECEIVERDEVICE] & 0x03;
            //iRecvPtr[ISI_HEADER_OFFSET_RECEIVERDEVICE] = (recvDev | (PN_DEV_SOS & 0xFC));

            iRecvPtr[ISI_HEADER_OFFSET_RECEIVERDEVICE] = PN_DEV_SOS;

            // Set Receiver Object ID
            TUint8 objIdHighBits = (EIscNokiaTest >> 8) & 0x03;
            TUint8 objIdLowBits = (EIscNokiaTest & 0x00ff);

            TUint8 receiverDevice( (iRecvPtr[ISI_HEADER_OFFSET_RECEIVERDEVICE] ) & 0xFC );

            receiverDevice = (receiverDevice | objIdHighBits);
            iRecvPtr[ISI_HEADER_OFFSET_RECEIVERDEVICE] = receiverDevice;

            iRecvPtr[ISI_HEADER_OFFSET_RECEIVEROBJECT] = objIdLowBits;
            }
#endif /* NCP_COMMON_BRIDGE_FAMILY */

        iMessageComplete = ETrue;
        }
    else
        {
        // Empty packet buffer and receive next packet to it.
        iPacket->ReleaseL();
        iRecvPtr.Set( iPacket->Buffer().Des() );
        iLdd.ReadUntilShort( iStatus, EEndpoint2, iRecvPtr );
        OstTrace0( TRACE_DETAILED, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_DUP8, "CUsbPnUsbReceiver::ConstructMessageL - Receiving" );
        E_TRACE((_T("CUsbPnUsbReceiver::ConstructMessage - Receiving")));
        SetActive();
        }

    OstTrace0( TRACE_NORMAL, CUSBPNUSBRECEIVER_CONSTRUCTMESSAGEL_EXIT, "CUsbPnUsbReceiver::ConstructMessageL - return void" );
    C_TRACE( ( _T( "CUsbPnUsbReceiver::ConstructMessage() - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File

