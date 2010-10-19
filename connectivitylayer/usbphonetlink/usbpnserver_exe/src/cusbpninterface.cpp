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
#include "cusbpninterface.h"
#include "cusbpnalt.h"
#include "cusbpnpacket.h"
#include "cusbpnisareceiver.h"
#include "cusbpnusbreceiver.h"
#include "musbpncontrolobserver.h"
#include "usbpndefinitions.h"       // For constants
#include "usbpntrace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbpninterfaceTraces.h"
#endif

#include <d32usbc.h>                // For RDevUsbcClient
#include <iscapi.h>                 // For RIscApi
#include <iscnokiadefinitions.h>    // For Channel ID

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
// CUsbPnInterface::CUsbPnInterface
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsbPnInterface::CUsbPnInterface()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_CUSBPNINTERFACE_ENTRY, "CUsbPnInterface::CUsbPnInterface" );
    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_CUSBPNINTERFACE_EXIT, "CUsbPnInterface::CUsbPnInterface - return" );
    C_TRACE( ( _T( "CUsbPnInterface::CUsbPnInterface()" ) ) );
    C_TRACE( ( _T( "CUsbPnInterface::CUsbPnInterface() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnInterface::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsbPnInterface::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_CONSTRUCTL_ENTRY, "CUsbPnInterface::ConstructL" );
    C_TRACE( ( _T( "CUsbPnInterface::ConstructL()" ) ) );

    iAlt = CUsbPnAlt::NewL( *this, iPnDataLdd, iIscApi );
    iIsaReceiver = CUsbPnIsaReceiver::NewL( iIscApi, iPnDataLdd );
    iUsbReceiver = CUsbPnUsbReceiver::NewL( iPnDataLdd, iIscApi );

    // Open ISC Channel
    TRequestStatus openStatus;
    iIscApi.Open( EIscNokiaUsbPhonetLink, openStatus );
    User::WaitForRequest( openStatus );
    User::LeaveIfError( openStatus.Int() );

    // Start ISA receiver without forward
    iIsaReceiver->Receive( EFalse );

    InitialiseInterfaceL();

    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_CONSTRUCTL_EXIT, "CUsbPnInterface::ConstructL - return void" );
    C_TRACE( ( _T( "CUsbPnInterface::ConstructL() - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnInterface::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnInterface* CUsbPnInterface::NewL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_NEWL_ENTRY, "CUsbPnInterface::NewL" );
    C_TRACE( ( _T( "CUsbPnInterface::NewL()" ) ) );

    CUsbPnInterface* self = new( ELeave ) CUsbPnInterface();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTrace1( TRACE_NORMAL, CUSBPNINTERFACE_NEWL_EXIT, "CUsbPnInterface::NewL - return;self=%x", self );
    C_TRACE( ( _T( "CUsbPnInterface::NewL() - return 0x%x" ), self ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CUsbPnInterface::~CUsbPnInterface
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbPnInterface::~CUsbPnInterface()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_CUSBPNINTERFACE_DESTRUCTOR_ENTRY, "CUsbPnInterface::~CUsbPnInterface" );
    C_TRACE( ( _T( "CUsbPnInterface::~CUsbPnInterface()" ) ) );

    if( iUsbReceiver )
        {
        delete iUsbReceiver;
        }

    if( iIsaReceiver )
        {
        delete iIsaReceiver;
        }

    if( iInitialised )
        {
        ReleaseInterface();
        }

    if( iAlt )
        {
        delete iAlt;
        }

    iIscApi.Close();

    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_CUSBPNINTERFACE_DESTRUCTOR_EXIT, "CUsbPnInterface::~CUsbPnInterface - return" );
    C_TRACE( ( _T( "CUsbPnInterface::~CUsbPnInterface() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CUsbPnInterface::HandleDeviceStateChange
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnInterface::HandleDeviceStateChange( TUint aDeviceState )
    {
    OstTrace1( TRACE_BORDER, CUSBPNINTERFACE_HANDLEDEVICESTATECHANGE_ENTRY, "CUsbPnInterface::HandleDeviceStateChange;aDeviceState=%u", aDeviceState );
    A_TRACE( ( _T( "CUsbPnInterface::HandleDeviceStateChange( aDeviceState:%d )"), aDeviceState ));

    if(aDeviceState & KUsbAlternateSetting)
        {
        OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_HANDLEDEVICESTATECHANGE, "CUsbPnInterface::HandleDeviceStateChange - Change alternate setting" );
        A_TRACE( ( _T( "CUsbPnInterface::HandleDeviceStateChange - Change alternate setting")));
        TUint altSetNbr( aDeviceState & ~KUsbAlternateSetting );

        SetAltSetting( altSetNbr );
        }
    else
        {
        OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_HANDLEDEVICESTATECHANGE_DUP1, "CUsbPnInterface::HandleDeviceStateChange - Normal state change" );
        E_TRACE( ( _T( "CUsbPnInterface::HandleDeviceStateChange - Normal state change")));
        switch( aDeviceState )
            {
            case EUsbcDeviceStateUndefined:
                {
                OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_HANDLEDEVICESTATECHANGE_DUP4, "CUsbPnInterface::HandleDeviceStateChange - EUsbcDeviceStateUndefined" );
                A_TRACE( ( _T( "CUsbPnInterface::HandleDeviceStateChange - EUsbcDeviceStateUndefined")));
                // Cancel async USB operations
                iUsbReceiver->SetEnumerated( EFalse );
                iUsbReceiver->Cancel();
                break;
                }
            case EUsbcDeviceStateDefault:
                {
                OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_HANDLEDEVICESTATECHANGE_DUP5, "CUsbPnInterface::HandleDeviceStateChange - EUsbcDeviceStateDefault" );
                A_TRACE( ( _T( "CUsbPnInterface::HandleDeviceStateChange - EUsbcDeviceStateDefault")));
                // Cancel async USB operations
                iUsbReceiver->SetEnumerated( EFalse );
                iUsbReceiver->Cancel();
                break;
                }
            default:
                {
                OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_HANDLEDEVICESTATECHANGE_DUP6, "CUsbPnInterface::HandleDeviceStateChange - State of no interest" );
                A_TRACE( ( _T( "CUsbPnInterface::HandleDeviceStateChange - State of no interest")));
                break;
                }
            }
        }

    OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_HANDLEDEVICESTATECHANGE_EXIT, "CUsbPnInterface::HandleDeviceStateChange - return void" );
    A_TRACE( ( _T( "CUsbPnInterface::HandleDeviceStateChange() - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnInterface::SetAltSetting
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnInterface::SetAltSetting( TUint aAltSetNumber )
    {
    OstTrace1( TRACE_BORDER, CUSBPNINTERFACE_SETALTSETTING_ENTRY, "CUsbPnInterface::SetAltSetting;aAltSetNumber=%u", aAltSetNumber );
    A_TRACE( ( _T( "CUsbPnInterface::SetAltSetting( aAltSetNumber:%d )" ), aAltSetNumber ) );

    if( aAltSetNumber )
        {
        // Receive from ISA with forwarding
        iIsaReceiver->SetForwardFlag( ETrue );

        iUsbReceiver->SetEnumerated( ETrue );
        iUsbReceiver->Receive( ETrue /*dummy*/ );
        }
    else
        {
        // Receive from ISA without forwarding
        iIsaReceiver->SetForwardFlag( EFalse );

        iUsbReceiver->SetEnumerated( EFalse );
        iUsbReceiver->Cancel();
        }

    OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_SETALTSETTING_EXIT, "CUsbPnInterface::SetAltSetting - return void" );
    A_TRACE( ( _T( "CUsbPnInterface::SetAltSetting() - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbPnInterface::InitialiseInterfaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUsbPnInterface::InitialiseInterfaceL()
    {
    OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_INITIALISEINTERFACEL_ENTRY, "CUsbPnInterface::InitialiseInterfaceL" );
    A_TRACE( ( _T( "CUsbPnInterface::InitialiseInterfaceL()" ) ) );

    TInt ret = iCommLdd.Open(0);
    if (ret != KErrNone)
        {
        // Maybe the device is not loaded yet.
        _LIT(KUsbLDDName, "eusbc");
        ret = User::LoadLogicalDevice( KUsbLDDName );

        OstTrace1( TRACE_INTERNALS, DUP100_CUSBPNINTERFACE_INITIALISEINTERFACEL, "CUsbPnInterface::InitialiseInterfaceL - Tried loading the device with status %d", ret );
        E_TRACE( ( _T("CUsbPnInterface::InitialiseInterfaceL - Tried loading the device with status %d"), ret ) );

        if (ret == KErrNone || ret == KErrAlreadyExists)
            {
            ret = iCommLdd.Open(0);
            }

        if (ret != KErrNone)
            {
            OstTrace1( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Cannot open Comm LDD %d", ret );
            E_TRACE( ( _T( "USBPN - Cannot open Comm LDD %d" ), ret ) );
            User::Leave( KErrNotReady );
            }
        }

    ret = iPnDataLdd.Open(0);
    if (ret != KErrNone)
        {
        OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP1, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Cannot open PnData LDD" );
        E_TRACE( ( _T( "USBPN - Cannot open PnData LDD" ) ) );
        User::Leave( KErrNotReady );
        }


//////////////////////////////////////////////////////////////////////
// Comm interface
//////////////////////////////////////////////////////////////////////

    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP2, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Setup Comm interface" );
    E_TRACE( ( _T( "USBPN - Setup Comm interface" ) ) );

    TUsbcInterfaceInfoBuf ifc;

    ifc().iString = const_cast<TDesC16*> (&KUsbPnCommIfc);
    ifc().iTotalEndpointsUsed = 0;
    ifc().iClass.iClassNum    = KCommInterfaceClass;
    ifc().iClass.iSubClassNum = KCommInterfaceSubClass;
    ifc().iClass.iProtocolNum = KCommInterfaceProtocol;

    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP3, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Call SetInterface 0" );
    E_TRACE( ( _T( "USBPN - Call SetInterface 0" ) ) );

    ret = iCommLdd.SetInterface(0, ifc);
    if (ret != KErrNone)
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( ret );
        }

    //Get the interface number for later
    TBuf8<100> interfaceDescriptor;
    iCommLdd.GetInterfaceDescriptor(0, interfaceDescriptor);
    TUint8 interfaceNum;
    interfaceNum = interfaceDescriptor.Ptr()[2];

    HBufC8* descBuf = HBufC8::NewLC( 64 );
    TPtr8 desc( descBuf->Des() );

    //Comms Class Header Functional Descriptor
    desc.Append(KPnHeaderFunctionalDescriptorLength);
    desc.Append(KUsbDescType_CS_Interface);
    desc.Append(KHeaderDescriptorSubType);
    desc.Append(KCDCVersionNumber[0]);
    desc.Append(KCDCVersionNumber[1]);

    // PHONET Functional Descriptor
    desc.Append(KPnFunctionalDescriptorLength);
    desc.Append(KUsbDescType_CS_Interface);
    desc.Append(KFunctionalDescriptorSubType);

    // Union Functional Descriptor
    desc.Append(KPnUnionFunctionalDescriptorLength);
    desc.Append(KUsbDescType_CS_Interface);
    desc.Append(KUnionDescriptorSubType);
    desc.Append(interfaceNum);
    TInt dataInt;
    dataInt = interfaceNum + 1;
    desc.Append(static_cast<TUint8>(dataInt));

    // Vendor DMA FD
    desc.Append(KVendorDMAFDDescriptorLength);
    desc.Append(KUsbDescType_CS_Interface);
    desc.Append(KVendorDMADescriptorSubType);
    desc.Append(KDmaCapabilities);      // Is zero. Meaning interface is DMA incapable

    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP4, "CUsbPnInterface::InitialiseInterfaceL - USBPN - SetCSInterfaceDescriptorBlock" );
    E_TRACE( ( _T( "USBPN - SetCSInterfaceDescriptorBlock" ) ) );
    ret = iCommLdd.SetCSInterfaceDescriptorBlock(0, desc);

    CleanupStack::PopAndDestroy( descBuf );

    if (ret != KErrNone)
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( ret );
        }


//////////////////////////////////////////////////////////////////////
// PHONET data interface alt 0
//////////////////////////////////////////////////////////////////////

    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP5, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Register PHONET Data IF (alt 0)" );
    E_TRACE( ( _T( "USBPN - Register PHONET Data IF (alt 0)" ) ) );
    TUsbcInterfaceInfoBuf dataifc;

    dataifc().iString = const_cast<TDesC16*> (&KUsbPnDataAlt0Ifc);
    dataifc().iClass.iClassNum = KDataInterfaceClass;
    dataifc().iClass.iSubClassNum = KDataInterfaceSubClass;
    dataifc().iClass.iProtocolNum = KDataInterfaceProtocol;
    dataifc().iTotalEndpointsUsed = 0;

    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP6, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Setup PHONET Data IF (alt 0)" );
    E_TRACE( ( _T( "USBPN - Setup PHONET Data IF (alt 0)" ) ) );

    ret = iPnDataLdd.SetInterface(0, dataifc);
    if (ret != KErrNone)
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( ret );
        }



//////////////////////////////////////////////////////////////////////
// data interface alt 1
//////////////////////////////////////////////////////////////////////

    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP7, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Register PHONET Data IF (alt 1)" );
    E_TRACE( ( _T( "USBPN - Register PHONET Data IF (alt 1)" ) ) );
    //Register Data Interface 1
    TUsbcInterfaceInfoBuf dataifc1;
    // Fill in dataifc1
    dataifc1().iString = const_cast<TDesC16*> (&KUsbPnDataAlt1Ifc);;
    dataifc1().iClass.iClassNum = KDataInterfaceClass;
    dataifc1().iClass.iSubClassNum = KDataInterfaceSubClass;
    dataifc1().iClass.iProtocolNum = KDataInterfaceProtocol;
    dataifc1().iTotalEndpointsUsed = KUsbPnRequiredNumberOfEndpoints;

    TUsbDeviceCaps dCaps1;
    ret = iPnDataLdd.DeviceCaps(dCaps1);
    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP8, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Checking result of DeviceCaps" );
    E_TRACE( ( _T( "USBPN - Checking result of DeviceCaps" ) ) );
    if ( ret )
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( ret );
        }

    const TUint totalEndpoints1 = static_cast<TUint>(dCaps1().iTotalEndpoints);
    OstTrace1( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP9, "CUsbPnInterface::InitialiseInterfaceL - USBPN - TotalEndpoints1=%d", totalEndpoints1 );
    E_TRACE( ( _T( "USBPN - TotalEndpoints1 %d" ), totalEndpoints1 ) );
    if (totalEndpoints1 < KUsbPnRequiredNumberOfEndpoints)
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( KErrGeneral );
        }


    TUsbcEndpointData data1[KUsbcMaxEndpoints];
    TPtr8 dataptr1 = TPtr8(reinterpret_cast<TUint8*>(data1), sizeof(data1), sizeof(data1));
    ret = iPnDataLdd.EndpointCaps(dataptr1);
    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP10, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Checking result of EndpointCaps" );
    E_TRACE( ( _T( "USBPN - Checking result of EndpointCaps" ) ) );
    if (ret)
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( ret );
        }

    // Set data interface 1
    TBool outFound = EFalse;
    TBool inFound = EFalse;

    for (TUint ii = 0 ; ii < totalEndpoints1; ii++)
        {
        OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP11, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Set data interface 1" );
        E_TRACE( ( _T( "USBPN - Set data interface 1" ) ) );
        const TUsbcEndpointCaps* caps;
        caps = &data1[ii].iCaps;

        OstTraceExt2( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP12, "CUsbPnInterface::InitialiseInterfaceL - USBPN - iTypesAndDir=%d, epSize=%d", caps->iTypesAndDir, caps->MaxPacketSize() );
        E_TRACE( ( _T( "USBPN - iTypesAndDir %d, epSize:%d" ), caps->iTypesAndDir, caps->MaxPacketSize() ) );

#ifdef EXTENDED_TRACE_FLAG
        if(((caps->iTypesAndDir & (KUsbEpDirIn)) == (KUsbEpDirIn) ) )
            {
            OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP13, "CUsbPnInterface::InitialiseInterfaceL - USBPN - IN" );
            E_TRACE( ( _T( "USBPN - IN" )));
            }
        if(((caps->iTypesAndDir & (KUsbEpDirOut)) == (KUsbEpDirOut) ) )
            {
            OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP14, "CUsbPnInterface::InitialiseInterfaceL - USBPN - OUT" );
            E_TRACE( ( _T( "USBPN - OUT" )));
            }
        if(((caps->iTypesAndDir & (KUsbEpTypeBulk)) == (KUsbEpTypeBulk)))
            {
            OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP15, "CUsbPnInterface::InitialiseInterfaceL - USBPN - BULK" );
            E_TRACE( ( _T( "USBPN - BULK" )));
            }
#endif // EXTENDED_TRACE_FLAG

        if(data1[ii].iInUse)
            {
            OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP16, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Ep in use. Next..." );
            E_TRACE( ( _T( "USBPN - Ep in use. Next...")));
            continue;
            }

        //changes to bulk in ep
        if (((caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirIn)) ==
            (KUsbEpTypeBulk | KUsbEpDirIn)) && !inFound)
            {
            OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP17, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Bulk IN found for EEndpoint1" );
            E_TRACE( ( _T( "USBPN - Bulk IN found for EEndpoint1" ) ) );
            // EEndpoint1 is bulk in endpoint
            dataifc1().iEndpointData[0].iType       = KUsbEpTypeBulk;
            dataifc1().iEndpointData[0].iDir        = KUsbEpDirIn;
            //dataifc1().iEndpointData[0].iInterval   = 0;

            dataifc1().iEndpointData[0].iSize       = caps->MaxPacketSize();
            inFound = ETrue;
            }
        else if (((caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirOut)) ==
            (KUsbEpTypeBulk | KUsbEpDirOut)) && !outFound)
            {
            OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP18, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Bulk OUT found for EEndpoint2" );
            E_TRACE( ( _T( "USBPN - Bulk OUT found for EEndpoint2" ) ) );
            // EEndpoint2 is bulk out endpoint
            dataifc1().iEndpointData[1].iType       = KUsbEpTypeBulk;
            dataifc1().iEndpointData[1].iDir        = KUsbEpDirOut;
            //dataifc1().iEndpointData[1].iInterval   = 0;

            dataifc1().iEndpointData[1].iSize       = caps->MaxPacketSize();
            outFound = ETrue;
            }

        if (outFound && inFound)
            {
            break;
            }
        } // end for ( ... )
    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP19, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Checking epFound" );
    E_TRACE( ( _T( "USBPN - Checking epFound" ) ) );
    if (!outFound || !inFound )
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( KErrGeneral );
        }

    OstTrace0( TRACE_INTERNALS, CUSBPNINTERFACE_INITIALISEINTERFACEL_DUP20, "CUsbPnInterface::InitialiseInterfaceL - USBPN - Setting LCIF Data IF (alt 1)" );
    E_TRACE( ( _T( "USBPN - Setting LCIF Data IF (alt 1)" ) ) );

    ret = iPnDataLdd.SetInterface(1, dataifc1);
    if (ret != KErrNone)
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( ret );
        }

    iAlt->ListenStateChange();
    iInitialised = ETrue;

    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_INITIALISEINTERFACEL_EXIT, "CUsbPnInterface::InitialiseInterfaceL - return void" );
    C_TRACE( ( _T( "CUsbPnInterface::InitialiseInterfaceL() - return void")));
    }

// -----------------------------------------------------------------------------
// CUsbInterface::ReleaseInterface
// Release and close all LDD in reverse order as in setup
// -----------------------------------------------------------------------------
//
EXPORT_C void CUsbPnInterface::ReleaseInterface()
    {
    OstTrace0( TRACE_BORDER, CUSBPNINTERFACE_RELEASEINTERFACE_ENTRY, "CUsbPnInterface::ReleaseInterface" );
    A_TRACE( ( _T( "CUsbPnInterface::ReleaseInterface()" ) ) );

    iAlt->Cancel();

    TInt ret;

    ret = iPnDataLdd.ReleaseInterface(1);
    if (ret != KErrNone)
        {
        TRACE_ASSERT_ALWAYS;
        }

    ret = iPnDataLdd.ReleaseInterface(0);
    if (ret != KErrNone)
        {
        TRACE_ASSERT_ALWAYS;
        }

    iPnDataLdd.Close();

    ret = iCommLdd.ReleaseInterface(0);
    if (ret != KErrNone)
        {
        TRACE_ASSERT_ALWAYS;
        }

    iCommLdd.Close();

    iInitialised = EFalse;

    OstTrace0( TRACE_NORMAL, CUSBPNINTERFACE_RELEASEINTERFACE_EXIT, "CUsbPnInterface::ReleaseInterface - return void" );
    C_TRACE( ( _T( "CUsbPnInterface::ReleaseInterface() - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
