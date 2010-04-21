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
#include "cusbpnalt.h"
#include "usbpndefinitions.h"       // For constants
#include "usbpntrace.h"
#include "musbpnaltobserver.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpnaltTraces.h"
#endif

#include <d32usbc.h>                // For RDevUsbcClient
#include <iscapi.h>                 // For RIscApi
#ifndef NCP_COMMON_BRIDGE_FAMILY
#include <sos_usbisi.h>             // For SOS USB Media Module Message constants
#include <tisi.h>                   // For TIsiSend
#include <phonetisi.h>              // For PhoNet definitions
#include <pn_const.h>               // For resources
#include <mediaisi.h>               // For media control
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
// CUsbPnAlt::CUsbPnAlt
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnAlt::CUsbPnAlt( MUsbPnAltObserver& aAltObserver, RDevUsbcClient& aLdd, RIscApi& aIscApi )
    :CActive( CActive::EPriorityStandard )
    ,iAltObserver( aAltObserver )
    ,iLdd( aLdd )
    ,iIscApi( aIscApi )
    {
    OstTrace1( TRACE_NORMAL, CUSBPNALT_CUSBPNALT_ENTRY, "CUsbPnAlt::CUsbPnAlt;aAltObserver=%x", ( TUint )&( aAltObserver ) );
    C_TRACE( ( _T( "CUsbPnAlt::CUsbPnAlt( aAltObserver 0x%x )" ), &aAltObserver ) );

    CActiveScheduler::Add(this);

    OstTrace0( TRACE_NORMAL, CUSBPNALT_CUSBPNALT_EXIT, "CUsbPnAlt::CUsbPnAlt - return" );
    C_TRACE( ( _T( "CUsbPnAlt::CUsbPnAlt() - return" ) ) );
    }


// -----------------------------------------------------------------------------
// CUsbPnAlt::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnAlt* CUsbPnAlt::NewL( MUsbPnAltObserver& aAltObserver, RDevUsbcClient& aLdd, RIscApi& aIscApi )
    {
    OstTrace1( TRACE_NORMAL, CUSBPNALT_NEWL_ENTRY, "CUsbPnAlt::NewL;aAltObserver=%x", ( TUint )&( aAltObserver ) );
    C_TRACE( ( _T( "CUsbPnAlt::NewL( aAltObserver 0x%x )" ), &aAltObserver ) );

    CUsbPnAlt* self = new( ELeave ) CUsbPnAlt( aAltObserver, aLdd, aIscApi );

    OstTrace1( TRACE_NORMAL, CUSBPNALT_NEWL_EXIT, "CUsbPnAlt::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnAlt::NewL() - return 0x%x" ), self ) );
    return self;
    }


// Destructor
CUsbPnAlt::~CUsbPnAlt()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNALT_CUSBPNALT_DESTRUCTOR_ENTRY, "CUsbPnAlt::~CUsbPnAlt" );
    C_TRACE( ( _T( "CUsbPnAlt::~CUsbPnAlt()" ) ) );

#ifndef NCP_COMMON_BRIDGE_FAMILY
    SendControlMessage( PNS_USB_CABLE_UNPLUGGED );
#endif /* NCP_COMMON_BRIDGE_FAMILY */

    Cancel();

    OstTrace0( TRACE_NORMAL, CUSBPNALT_CUSBPNALT_DESTRUCTOR_EXIT, "CUsbPnAlt::~CUsbPnAlt - return" );
    C_TRACE( ( _T( "CUsbPnAlt::~CUsbPnAlt() - return" ) ) );
    }


// -----------------------------------------------------------------------------
// CUsbPnAlt::ListenStateChange
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnAlt::ListenStateChange( )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNALT_LISTENSTATECHANGE_ENTRY, "CUsbPnAlt::ListenStateChange" );
    C_TRACE( ( _T( "CUsbPnAlt::ListenStateChange()" ) ) );

    iLdd.AlternateDeviceStatusNotify( iStatus, iState );
    SetActive();

    OstTrace0( TRACE_NORMAL, CUSBPNALT_LISTENSTATECHANGE_EXIT, "CUsbPnAlt::ListenStateChange - return void" );
    C_TRACE( ( _T( "CUsbPnAlt::ListenStateChange() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnAlt::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnAlt::DoCancel( )
    {
    OstTrace0( TRACE_NORMAL, CUSBPNALT_DOCANCEL_ENTRY, "CUsbPnAlt::DoCancel" );
    C_TRACE( ( _T( "CUsbPnAlt::DoCancel()" ) ) );

    iLdd.AlternateDeviceStatusNotifyCancel();

    OstTrace0( TRACE_NORMAL, CUSBPNALT_DOCANCEL_EXIT, "CUsbPnAlt::DoCancel - return void" );
    C_TRACE( ( _T( "CUsbPnAlt::DoCancel() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnAlt::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnAlt::RunL( )
    {
    OstTrace1( TRACE_API, CUSBPNALT_RUNL_ENTRY, "CUsbPnAlt::RunL;iStatus=%d", iStatus.Int() );
    A_TRACE( ( _T( "CUsbPnAlt::RunL() iStatus:%d" ), iStatus.Int() ) );

    User::LeaveIfError( iStatus.Int() );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    // Check and send control messages.
    if( !( iState & KUsbAlternateSetting ) ) // KUsbAlternateSetting bit is not set.
        {
        switch( iState )
            {
            case EUsbcDeviceStateAddress:
                {
                OstTrace0( TRACE_API, CUSBPNALT_RUNL_DUP1, "CUsbPnAlt::RunL - EUsbcDeviceStateAddress" );
                A_TRACE( ( _T( "CUsbPnAlt::RunL - EUsbcDeviceStateAddress")));
                SendControlMessage( PNS_USB_CABLE_PLUGGED );
                break;
                }
            case EUsbcDeviceStateConfigured:
                {
                OstTrace0( TRACE_API, CUSBPNALT_RUNL_DUP2, "CUsbPnAlt::RunL - EUsbcDeviceStateConfigured" );
                A_TRACE( ( _T( "CUsbPnAlt::RunL - EUsbcDeviceStateConfigured")));
                SendControlMessage( PNS_USB_CONNECTED );
                break;
                }
            case EUsbcDeviceStateUndefined:
                {
                OstTrace0( TRACE_API, CUSBPNALT_RUNL_DUP3, "CUsbPnAlt::RunL - EUsbcDeviceStateUndefined" );
                A_TRACE( ( _T( "CUsbPnAlt::RunL - EUsbcDeviceStateUndefined")));
                SendControlMessage( PNS_USB_CABLE_UNPLUGGED );
                break;
                }
            }
        }
#endif /* NCP_COMMON_BRIDGE_FAMILY */
    iAltObserver.HandleDeviceStateChange( iState );

    iLdd.AlternateDeviceStatusNotify( iStatus, iState );
    SetActive();

    OstTrace0( TRACE_API, CUSBPNALT_RUNL_EXIT, "CUsbPnAlt::RunL - return void");
    A_TRACE( ( _T( "CUsbPnAlt::RunL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnAlt::RunError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUsbPnAlt::RunError( TInt aError )
    {
    OstTrace1( TRACE_NORMAL, CUSBPNALT_RUNERROR_ENTRY, "CUsbPnAlt::RunError;aError=%d", aError );
    C_TRACE( ( _T( "CUsbPnAlt::RunError( aError:%d )" ), aError ) );
    TRACE_ASSERT_ALWAYS;
    User::Panic( KUsbPnPanicCat, aError );
    OstTrace1( TRACE_NORMAL, CUSBPNALT_RUNERROR_EXIT, "CUsbPnAlt::RunError - return;aError=%d", aError );
    C_TRACE( ( _T( "CUsbPnAlt::RunError() return - aError:%d )" ), aError ) );
    return aError;
    }

#ifndef NCP_COMMON_BRIDGE_FAMILY
// -----------------------------------------------------------------------------
// CUsbPnAlt::SendControlMessage
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnAlt::SendControlMessage( const TUint8 aId ) const
    {
    C_TRACE( ( _T( "CUsbPnAlt::SendControlMessage( aId:%d )" ), aId ) );
    OstTrace0( TRACE_API, CUSBPNALT_SENDCONTROLMESSAGE_DUP1, "CUsbPnAlt::SendControlMessage( aId )" );

    TBuf8< SIZE_PNS_MEDIA_SPECIFIC_REQ > buf;
    TIsiSend ctrlMsg(buf, 12 );
    ctrlMsg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, PN_DEV_PC );
    ctrlMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MEDIA_CONTROL );
    ctrlMsg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, PN_OBJ_ROUTER );
    ctrlMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, 0x00 );
    ctrlMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, PNS_MEDIA_SPECIFIC_REQ );
    ctrlMsg.Set8bit( ISI_HEADER_OFFSET_SUBMESSAGEID, PN_MEDIA_USB );
    ctrlMsg.Set8bit( ISI_HEADER_OFFSET_SUBMESSAGEID + 1, aId );
    ctrlMsg.Complete();

    C_TRACE( ( _T( "CUsbPnAlt::SendControlMessage() - Sending...") ) );
    OstTrace0( TRACE_API, CUSBPNALT_SENDCONTROLMESSAGE_DUP2, "CUsbPnAlt::SendControlMessage() - Sending..." );
    iIscApi.Send( buf );

    C_TRACE( ( _T( "CUsbPnAlt::SendControlMessage() - return void") ) );
    OstTrace0( TRACE_API, CUSBPNALT_SENDCONTROLMESSAGE_DUP3, "CUsbPnAlt::SendControlMessage() - return void" );
    }

#endif /* NCP_COMMON_BRIDGE_FAMILY */

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
