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



#ifndef IADDEFINITIONS_H
#define IADDEFINITIONS_H

#include <iscnokiadefinitions.h> // For EIscNokia...

//  INCLUDES

// CONSTANTS
enum TIADNokiaUserChannels
    {
    EIADNokiaBtPhonetLink = EIscNokiaBtPhonetLink,
    EIADNokiaUsbPhonetLink = EIscNokiaUsbPhonetLink,
    EIADNokiaDRM = EIscNokiaDRM,
    EIADNokiaLastUserChannel = EIscNokiaLastUserChannel,
    };

enum TIADNokiaKernelChannels
    {
    EIADNokiaSecurityDriver = EIscNokiaSecurityDriver,
    EIADSizeOfChannels = EIscNokiaLastKernelChannel
    };

// MACROS

// DATA TYPES
// For BC reasons see (iscnokiadefinitions.h). //  change to here!
enum TIADCustomFunctions
    {
    EIADCustomAsyncSubscribeIndications = EIscNokiaAsyncEventSubscribe,
    EIADCustomAsyncSubscribeIndications32Bit = EIscNokiaAsyncEventSubscribe32bit,
    EIADCustomAsyncSendIndication = EIscNokiaAsyncEventSend,
    EIADCustomSubscribeIndications = EIscNokiaEventSubscribe,
    EIADCustomSubscribeIndications32Bit = EIscNokiaEventSubscribe32bit,
    EIADCustomSendIndication = EIscNokiaEventSend,
    EIADCancelCustomAsyncSubscribeIndications = EIscNokiaCancelAsyncEventSubscribe,
    EIADCancelCustomAsyncSubscribeIndications32Bit = EIscNokiaCancelAsyncEventSubscribe32bit,
    EIADCancelCustomAsyncSendIndication = EIscNokiaCancelAsyncEventSend,
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


#endif      // IADNOKIADEFINITIONS_H
            
// End of File
