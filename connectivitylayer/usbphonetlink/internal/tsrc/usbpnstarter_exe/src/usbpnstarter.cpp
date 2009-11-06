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
* Source file for USB Phonet Link starter.
*/

// INCLUDE FILES
#include    "usbpnstarter.h"    // For trace macros
#include    <rusbpnclient.h>    // For RUsbPnClient
#include    <e32std.h>          // For User
#include    <d32usbc.h>         // For RDevUsbcClient
#ifdef BSW_USB_DRC // flag for OTG support
#include    <d32otgdi.h>        // RUsbOtgDriver
#endif // BSW_USB_DRC


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
_LIT(KUsbLDDName, "eusbc");
#ifdef BSW_USB_DRC
_LIT(KOtgdiLddFilename, "otgdi");
#endif // BSW_USB_DRC

//Device Descriptor Offsets
const TInt KUsbSpecOffset                   = 2;
const TInt KUsbDeviceClassOffset            = 4;
const TInt KUsbDeviceSubClassOffset         = 5;
const TInt KUsbDeviceProtocolOffset         = 6;
const TInt KUsbVendorIdOffset               = 8;
const TInt KUsbProductIdOffset              = 10;
const TInt KUsbDevReleaseOffset             = 12;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// LoadUsbDriverL()
// -----------------------------------------------------------------------------
void LoadUsbDriverL( RDevUsbcClient& aDriver )
    {
    C_TRACE( ( _T("usbpnstarter # LoadUsbDriverL") ) );

    TInt error = User::LoadLogicalDevice(KUsbLDDName);
    if( error != KErrNone || error != KErrAlreadyExists )
        {
        C_TRACE( ( _T("usbpnstarter # LoadUsbDriverL - LDD load failed: %d"), error ) );
        User::Leave(error);
        }
    // Open channel to device driver.
    error = aDriver.Open(0);
    if ( error != KErrNone )
        {
        C_TRACE( ( _T("usbpnstarter # LoadUsbDriverL - aDriver.Open failed: %d"), error ) );
        User::Leave(error);
        }
#ifdef BSW_USB_DRC
    error = User::LoadLogicalDevice(KOtgdiLddFilename);
    if( error != KErrNone && error != KErrAlreadyExists )
        {
        C_TRACE( ( _T("usbpnstarter # Error %d on loading OTG LDD"), error ) );
        User::Leave(error);
        }
    RUsbOtgDriver otgDevice;
    error = otgDevice.Open();
    if (error != KErrNone)
        {
        C_TRACE( ( _T("usbpnstarter # Error %d on opening OTG port"), error ) );
        User::Leave(error);
        }
    error = otgDevice.StartStacks();
    if (error != KErrNone)
        {
        C_TRACE( ( _T("usbpnstarter # Error %d on starting USB stack"), error ) );
        User::Leave(error);
        }
#endif // BSW_USB_DRC
    }
// -----------------------------------------------------------------------------
// SetupDeviceDescriptors()
// -----------------------------------------------------------------------------
TInt SetupDeviceDescriptors( RDevUsbcClient& aDriver )
    {
    C_TRACE( ( _T( "usbpnstarter # SetupDeviceDescriptors") ) );
    // Simulates a physical removal of the USB cable
    TInt ret = aDriver.DeviceDisconnectFromHost();

    TBuf8<KUsbDescSize_Device> deviceDescriptor;
    ret = aDriver.GetDeviceDescriptor( deviceDescriptor );
    if( ret != KErrNone )
        {
        return KErrGeneral;
        }

    // Change the USB spec number to 2.00
    deviceDescriptor[ KUsbSpecOffset ]   = 0x00;
    deviceDescriptor[ KUsbSpecOffset+1 ] = 0x02;

    //Change the Device Class Codes
    deviceDescriptor[ KUsbDeviceClassOffset ] = 0x02;     // Class = 0x02;
    deviceDescriptor[ KUsbDeviceSubClassOffset ] = 0x00;  // SubClass = 0x00;
    deviceDescriptor[ KUsbDeviceProtocolOffset ] = 0x00;  // Protocol = 0x00;

    // Change the device vendor ID ( VID ) to 0x0421 ( Nokia Vendor ID )
    deviceDescriptor[ KUsbVendorIdOffset ]   = 0x21;
    deviceDescriptor[ KUsbVendorIdOffset+1 ] = 0x04;

    // Change the device product ID ( PID )( Generic Nokia ID ) to 0x00c8
    deviceDescriptor[ KUsbProductIdOffset ] = 0xC8;
    deviceDescriptor[ KUsbProductIdOffset+1 ] = 0x00;

    // Change the device release number to 0x0110
    deviceDescriptor[ KUsbDevReleaseOffset ] = 0x10;
    deviceDescriptor[ KUsbDevReleaseOffset+1 ] = 0x01;

    ret = aDriver.SetDeviceDescriptor( deviceDescriptor );
    if( ret != KErrNone )
        {
        return KErrGeneral;
        }

    C_TRACE( ( _T( "usbpnstarter # SetupDeviceDescriptors return %d"), ret) );
    return ret;
    }
// -----------------------------------------------------------------------------
// CreateGenericCdcInterface()
// -----------------------------------------------------------------------------
TInt CreateGenericCdcInterface( RDevUsbcClient& aDriver )
    {
    C_TRACE( ( _T( "usbpnstarter # CreateGenericCdcInterface()") ) );
    TInt ret = KErrNone;

    TUsbDeviceCaps deviceCaps;
    ret = aDriver.DeviceCaps(deviceCaps);

    if ( KErrNone == ret )
        {
        TInt totalEndpoints = deviceCaps().iTotalEndpoints;

        C_TRACE( ( _T( "usbpnstarter # CreateGenericCdcInterface Number of endpoints: %d"), totalEndpoints) );

        // Endpoints
        TUsbcEndpointData data[KUsbcMaxEndpoints];
        TPtr8 dataptr(reinterpret_cast<TUint8*>(data), sizeof(data), sizeof(data));
        ret = aDriver.EndpointCaps(dataptr);
        if ( KErrNone == ret )
            {
            TUsbcInterfaceInfoBuf ifc;
            TInt endPointsFound = 0;
            TBool foundBulkIN = EFalse;
            TBool foundBulkOUT = EFalse;
            TInt i = 0;
            ret = KErrNotFound;
            while ( i < totalEndpoints && endPointsFound < 2 )
                {
                const TUsbcEndpointCaps* caps = &data[i].iCaps;
                const TInt maxPacketSize = caps->MaxPacketSize();
                if ( !foundBulkIN &&
                    (caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirIn)) == (KUsbEpTypeBulk | KUsbEpDirIn) )
                    {
                    C_TRACE( ( _T( "usbpnstarter # CreateGenericCdcInterface EEndpoint%d is going to be our TX (IN, write) endpoint"), i) );
                    // EEndpoint1 is going to be our TX (IN, write) endpoint
                    ifc().iEndpointData[0].iType = KUsbEpTypeBulk;
                    ifc().iEndpointData[0].iDir  = KUsbEpDirIn;
                    ifc().iEndpointData[0].iSize = maxPacketSize;
                    foundBulkIN = ETrue;
                    endPointsFound++;
                    }
                if ( !foundBulkOUT &&
                    (caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirOut)) == (KUsbEpTypeBulk | KUsbEpDirOut) )
                    {
                    C_TRACE( ( _T( "usbpnstarter # CreateGenericCdcInterface EEndpoint%d is going to be our RX (OUT, read) endpoint"), i) );
                    // EEndpoint2 is going to be our RX (OUT, read) endpoint
                    ifc().iEndpointData[1].iType = KUsbEpTypeBulk;
                    ifc().iEndpointData[1].iDir  = KUsbEpDirOut;
                    ifc().iEndpointData[1].iSize = maxPacketSize;
                    foundBulkOUT = ETrue;
                    endPointsFound++;
                    }
                i++;
                }

            if ( endPointsFound == 2 )
                {
                _LIT16(KUSBName, "HS USB Test Interface");

                TBufC16<30> string(KUSBName);
                ifc().iString = &string;
                ifc().iTotalEndpointsUsed = 2;
                ifc().iClass.iClassNum    = 0xff;
                ifc().iClass.iSubClassNum = 0xff;
                ifc().iClass.iProtocolNum = 0xff;

                ret = aDriver.SetInterface(0, ifc);
                }
            }
        }

    C_TRACE( ( _T( "usbpnstarter # CreateGenericCdcInterface return %d"), ret) );
    return ret;
    }
// ============================ MEMBER FUNCTIONS ===============================
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32Main()
// -----------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    C_TRACE( ( _T( "usbpnstarter # E32Main()") ) );
    RDevUsbcClient usbDriver;
    TRAPD( ret, LoadUsbDriverL( usbDriver ) );
    if( ret != KErrNone )
        {
        C_TRACE( ( _T( "usbpnstarter # E32Main() - LoadUsbDriverL failed.") ) );
        return ret;
        }
    ret = SetupDeviceDescriptors( usbDriver );
    if( ret != KErrNone )
        {
        C_TRACE( ( _T( "usbpnstarter # E32Main() - SetupDeviceDescriptors failed.") ) );
        return ret;
        }

    RUsbPnClient usbPnClient;
    TRAPD( ret, usbPnClient.ConnectL() );
    if ( ret != KErrNone )
        {
        C_TRACE( ( _T( "usbpnstarter # E32Main() - ConnectL failed.") ) );
        return ret;
        }
    usbDriver.PowerUpUdc();
    usbDriver.DeviceConnectToHost();

    ret = CreateGenericCdcInterface( usbDriver );
    if( ret != KErrNone )
        {
        C_TRACE( ( _T( "usbpnstarter # E32Main() - CreateGenericCdcInterface failed.") ) );
        return ret;
        }
    C_TRACE( ( _T( "usbpnstarter # E32Main() - Going to infinite loop") ) );
    while( 1 ); // Stay on forever. Usbpnserver will destroy itself when there are no clients.

    }

//  End of File
