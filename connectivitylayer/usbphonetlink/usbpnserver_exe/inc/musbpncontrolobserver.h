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


#ifndef MUSBPNCONTROLOBSERVER_H
#define MUSBPNCONTROLOBSERVER_H

// INCLUDES
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CUsbPnPacket;
// CLASS DECLARATION

class MUsbPnControlObserver
    {
    public: // New functions

        /**
        * Get packet for sending.
        */
        virtual CUsbPnPacket& GetNextPacketL() = 0;

        /**
        * Send packet.
        */
        virtual void AddPacketToSendingQueue( CUsbPnPacket& aPacket ) = 0;

    };

#endif      // MUSBPNCONTROLOBSERVER_H

// End of File
