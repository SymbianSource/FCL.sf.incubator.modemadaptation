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


#ifndef CUSBPNUSBSENDER_H
#define CUSBPNUSBSENDER_H

//  INCLUDES
#include <e32base.h>        // For CActive

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class RDevUsbcClient;
class MUsbPnBufferListener;
class CUsbPnPacket;

// CLASS DECLARATION

/**
*  Sends received messages to USB
*/
class CUsbPnUsbSender : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUsbPnUsbSender* NewL( MUsbPnBufferListener&, RDevUsbcClient& );

        /**
        * Destructor.
        */
        ~CUsbPnUsbSender();

    public: // New functions

        /**
        * Adds message to sending queue.
        * @param CUsbPnPacket& aPacket. Cell of circular buffer holding received data.
        */
        void Send( CUsbPnPacket& aPacket );

        /**
        * Get next free packet from pool.
        * @return reference to usable packet for receiver.
        */
        CUsbPnPacket& PacketL();


    protected:  // Functions from base classes

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        TInt RunError( TInt );


    private:

        /**
        * C++ default constructor.
        */
        CUsbPnUsbSender( MUsbPnBufferListener&, RDevUsbcClient& );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates packet object.
        */
        void AddPacketBufferL( TInt );

        /**
        * Writes oldest message from queue.
        */
        void TryToSendPacket( const CUsbPnPacket& );


    private:    // Data

        // Transfer mechanism observer
        MUsbPnBufferListener&   iBufferListener;

        // Reference to USB driver LDD.
        RDevUsbcClient&         iLdd;

        // Number of packets in queue
        TUint                   iPacketCount;

        // Ownership. Newest packet -marker of circular buffer. Packets added to this 'end' of pool.
        CUsbPnPacket*           iPacket;

        // Pointer to oldest packet -marker of circular buffer. Packets written from this 'end' of pool.
        CUsbPnPacket*           iCurrentPacket;

    };

#endif      // CUSBPNUSBSENDER_H

// End of File
