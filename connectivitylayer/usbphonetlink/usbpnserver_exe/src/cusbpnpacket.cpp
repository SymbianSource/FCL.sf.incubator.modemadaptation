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
#include "cusbpnpacket.h"
#include "usbpndefinitions.h"
#include "usbpntrace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnpacketTraces.h"
#endif

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
// CUsbPnPacket::CUsbPnPacket
// -----------------------------------------------------------------------------
CUsbPnPacket::CUsbPnPacket( CUsbPnPacket* aPacket, TInt aNum )

    /*lint -e1927 disabled because all variables are initialized after construction */
    : iBuffer(NULL)
    , iNumber( aNum )
    /*lint -restore */

    {
    OstTraceExt2( TRACE_NORMAL, CUSBPNPACKET_CUSBPNPACKET_ENTRY, "CUsbPnPacket::CUsbPnPacket;aPacket=%x;aNum=%d", ( TUint )( aPacket ), aNum );
    C_TRACE((_T("CUsbPnPacket::CUsbPnPacket(0x%x, aNum %d)"), aPacket, aNum));

    if(aPacket != NULL)
        {
        // Link the packet to the list.
        iNextPacket = aPacket->iNextPacket;
        aPacket->iNextPacket = this;
        }
    else
        {
        // Create a circular buffer.
        iNextPacket = this;
        }

    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_CUSBPNPACKET_EXIT, "CUsbPnPacket::CUsbPnPacket - return void" );
    C_TRACE((_T("CUsbPnPacket::CUsbPnPacket - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnPacket::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_CONSTRUCTL_ENTRY, "CUsbPnPacket::ConstructL" );
    C_TRACE( ( _T( "CUsbPnPacket::ConstructL()" ) ) );

    iBuffer = HBufC8::NewL( KPnPacketSize );

    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_CONSTRUCTL_EXIT, "CUsbPnPacket::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnPacket::ConstructL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnPacket* CUsbPnPacket::NewL( CUsbPnPacket* aPacket, TInt aNum )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_NEWL_ENTRY, "CUsbPnPacket::NewL" );
    C_TRACE( ( _T( "CUsbPnPacket::NewL( )" ) ) );

    CUsbPnPacket* self = new( ELeave ) CUsbPnPacket( aPacket, aNum );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    OstTrace1( TRACE_NORMAL, CUSBPNPACKET_NEWL_EXIT, "CUsbPnPacket::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnPacket::NewL() - return 0x%x" ), self ) );
    return self;
    }


// -----------------------------------------------------------------------------
// CUsbPnPacket::~CUsbPnPacket
// -----------------------------------------------------------------------------
CUsbPnPacket::~CUsbPnPacket()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_CUSBPNPACKET_DESTRUCTOR_ENTRY, "CUsbPnPacket::~CUsbPnPacket" );
    C_TRACE((_T("CUsbPnPacket::~CUsbPnPacket()")));

    OstTrace1( TRACE_NORMAL, CUSBPNPACKET_CUSBPNPACKET_DESTRUCTOR, "CUsbPnPacket::~CUsbPnPacket deleting packet: %d", iNumber );
    C_TRACE((_T("CUsbPnPacket::~CUsbPnPacket() deleting packet: %d"), iNumber));

    if( iBuffer )
        {
        delete iBuffer;
        }

    iBuffer = NULL;
    iNextPacket = NULL;

    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_CUSBPNPACKET_DESTRUCTOR_EXIT, "CUsbPnPacket::~CUsbPnPacket - return void" );
    C_TRACE((_T("CUsbPnPacket::~CUsbPnPacket - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::NextPacket
// -----------------------------------------------------------------------------
CUsbPnPacket& CUsbPnPacket::NextPacket() const
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_NEXTPACKET_ENTRY, "CUsbPnPacket::NextPacket" );
    OstTrace1( TRACE_NORMAL, CUSBPNPACKET_NEXTPACKET_EXIT, "CUsbPnPacket::NextPacket - return;CUsbPnPacket*=%x", iNextPacket );
    C_TRACE((_T("CUsbPnPacket::NextPacket()")));
    C_TRACE((_T("CUsbPnPacket::NextPacket - return CUsbPnPacket* 0x%x"), iNextPacket));
    return *iNextPacket;
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::Buffer
// -----------------------------------------------------------------------------
HBufC8& CUsbPnPacket::Buffer() const
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_BUFFER_ENTRY, "CUsbPnPacket::Buffer" );
    OstTrace1( TRACE_NORMAL, CUSBPNPACKET_BUFFER_EXIT, "CUsbPnPacket::Buffer - return;HBufC8* iBuffer=%x", iBuffer );
    C_TRACE((_T("CUsbPnPacket::Buffer()")));
    C_TRACE((_T("CUsbPnPacket::Buffer - return HBufC8* iBuffer 0x%x"), iBuffer));
    return *iBuffer;
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::PacketInUse
// -----------------------------------------------------------------------------
TBool CUsbPnPacket::PacketInUse() const
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_PACKETINUSE_ENTRY, "CUsbPnPacket::PacketInUse" );
    C_TRACE((_T("CUsbPnPacket::PacketInUse()")));

    TBool isInUse( EFalse );
    TPtr8 ptr(iBuffer->Des());
    if( ptr.Length() > 0 )
        {
        isInUse = ETrue;
        }
    else
        {
        isInUse = EFalse;
        }

    OstTrace1( TRACE_NORMAL, CUSBPNPACKET_PACKETINUSE_EXIT, "CUsbPnPacket::PacketInUse - return TBool %d", isInUse );
    C_TRACE((_T("CUsbPnPacket::PacketInUse - return TBool %d"), isInUse));
    return isInUse;
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::PacketNumber
// -----------------------------------------------------------------------------
TInt CUsbPnPacket::PacketNumber() const
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_PACKETNUMBER_ENTRY, "CUsbPnPacket::PacketNumber" );
    OstTrace1( TRACE_NORMAL, CUSBPNPACKET_PACKETNUMBER_EXIT, "CUsbPnPacket::PacketNumber - return TInt %d", iNumber );
    C_TRACE((_T("CUsbPnPacket::PacketNumber()")));
    C_TRACE((_T("CUsbPnPacket::PacketNumber - return TInt %d"),iNumber));
    return iNumber;
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::ReleaseL
// -----------------------------------------------------------------------------
void CUsbPnPacket::ReleaseL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_RELEASEL_ENTRY, "CUsbPnPacket::ReleaseL" );
    C_TRACE((_T("CUsbPnPacket::ReleaseL()")));

    TPtr8 ptr(iBuffer->Des());
    if(ptr.Length() > KPnPacketSize )
        {
        OstTrace0( TRACE_INTERNALS, CUSBPNPACKET_RELEASEL, "CUsbPnPacket::ReleaseL - Returning to normal size" );
        E_TRACE((_T("CUsbPnPacket::ReleaseL() - Returning to normal size")));
        delete iBuffer;
        iBuffer = NULL;
        iBuffer = HBufC8::NewL( KPnPacketSize );
        }
    else
        {
        ptr.FillZ();
        ptr.SetLength( 0 );
        }

    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_RELEASEL_EXIT, "CUsbPnPacket::ReleaseL - return void" );
    C_TRACE((_T("CUsbPnPacket::ReleaseL() - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnPacket::ReallocBufferL
// -----------------------------------------------------------------------------
void CUsbPnPacket::ReallocBufferL( const TInt aNeededLength )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_REALLOCBUFFERL_ENTRY, "CUsbPnPacket::ReallocBufferL" );
    C_TRACE((_T("CUsbPnPacket::ReallocBufferL()")));

    if(iBuffer)
        {
        delete iBuffer;
        iBuffer = NULL;
        }

    iBuffer = HBufC8::NewL( aNeededLength );

    OstTrace0( TRACE_NORMAL, CUSBPNPACKET_REALLOCBUFFERL_EXIT, "CUsbPnPacket::ReallocBufferL - return void" );
    C_TRACE((_T("CUsbPnPacket::ReallocBufferL() - return void")));
    }
