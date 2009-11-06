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


#ifndef USBPNDEFINITIONS_H
#define USBPNDEFINITIONS_H

// INCLUDES
// CONSTANTS

// Name of the server thread
_LIT( KUsbPnServerName, "USBPNServer" );

const TInt KPnNumInterfaces                         = 2; // dummy and data interfaces
const TUint KUsbPnRequiredNumberOfEndpoints         = 2;

// Lengths of the Pn descriptor.
const TInt KPnInterfaceDescriptorLength             = 9;
const TInt KPnDataClassEndpointInDescriptorLength   = 7;
const TInt KPnDataClassEndpointOutDescriptorLength  = 7;
const TInt KPnHeaderFunctionalDescriptorLength      = 5;
const TInt KPnFunctionalDescriptorLength            = 3;
const TInt KPnUnionFunctionalDescriptorLength       = 5;

// Total length of descriptor
const TInt KPnDescriptorLength =
    KPnHeaderFunctionalDescriptorLength +
    KPnFunctionalDescriptorLength +
    KPnUnionFunctionalDescriptorLength +
    KPnInterfaceDescriptorLength +      // dummy
    KPnInterfaceDescriptorLength +      // data alt 0
    KPnInterfaceDescriptorLength +      // data alt 1
    KPnDataClassEndpointInDescriptorLength +
    KPnDataClassEndpointOutDescriptorLength;

// Interface descriptions
_LIT(KUsbPnCommIfc, "UsbPnComm");
_LIT(KUsbPnDataAlt0Ifc, "UsbPnDataAlt0");
_LIT(KUsbPnDataAlt1Ifc, "UsbPnDataAlt1");

// Communication class PHONET model Interface descriptor
const TInt KCommInterfaceClass                      = 2;
const TInt KCommInterfaceSubClass                   = 0xFE;
const TInt KCommInterfaceProtocol                   = 0;

// Data Class Interface Descriptor for PHONET
const TInt KDataInterfaceClass                      = 0x0A;
const TInt KDataInterfaceSubClass                   = 0x0;
const TInt KDataInterfaceProtocol                   = 0;

// PHONET communication class header functional descriptor
const TInt KHeaderDescriptorSubType                 = 0x0;
const TInt8 KCDCVersionNumber[2]                    = { 0x01, 0x10 };

// PHONET model functional descriptor
const TInt KFunctionalDescriptorSubType             = 0xAB;

// PHONET union functional descriptor
const TInt KUnionDescriptorSubType                  = 0x6;

// Vendor DMA
const TInt KVendorDMAFDDescriptorLength             = 4;
const TInt KVendorDMADescriptorSubType              = 0xfd;
const TInt KDmaCapabilities                         = 0;

// Panic category
_LIT( KUsbPnPanicCat, "USB Phonet Link" );

// Circular buffet packet size
const TInt KPnPacketSize                            = 512;

// Packet count of IsaSender
const TInt KPnIsaPacketCount                        = 4;

// Packet count of UsbSender
const TInt KPnUsbPacketCount                        = 20;

enum TUsbPnClientMessage
    {
    EPnDetach
    };

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

#endif      // USBPNDEFINITIONS_H
// End of File
