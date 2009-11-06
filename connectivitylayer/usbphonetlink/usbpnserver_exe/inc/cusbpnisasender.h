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


#ifndef CUSBPNISASENDER_H
#define CUSBPNISASENDER_H

//  INCLUDES
#include "musbpncontrolobserver.h"

#include <e32base.h>    // For CActive

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MUsbPnBufferListener;
class CUsbPnPacket;
class RIscApi;

// CLASS DECLARATION

/**
*  Data sender to ISA direction.
*/
class CUsbPnIsaSender : public CActive, public MUsbPnControlObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUsbPnIsaSender* NewL( MUsbPnBufferListener&, RIscApi& );

        /**
        * Destructor.
        */
        ~CUsbPnIsaSender();


    public: // Functions from base classes

        /**
        * From MUsbPnControlObserver. Returns usable circular packet if any.
        * @return CUsbPnPacket&. Packet for receiver.
        */
        CUsbPnPacket& PacketL();

        /**
        * From MUsbPnControlObserver. Adds packet to send queue.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        void Send( CUsbPnPacket& aPacket );


    protected:  // Functions from base classes

        /**
        * From CActive
        */
        void DoCancel( );

        /**
        * From CActive
        */
        void RunL( );

        /**
        * From CActive
        */
        TInt RunError( TInt );


    private:

        /**
        * C++ default constructor.
        */
        CUsbPnIsaSender( MUsbPnBufferListener&, RIscApi& );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates packet object.
        */
        void AddPacketBufferL( TInt );

        /**
        * Sends packet if it is next in line and not already sending.
        */
        void TryToSendPacket( CUsbPnPacket& );


    private:    // Data

        // Reference to receiver for continuing receive
        MUsbPnBufferListener&   iBufferListener;

        // Reference to ISCAPI handle
        RIscApi&                iIscApi;

        // Number of packets in queue
        TUint                   iPacketCount;

        // Ownership. Newest packet -marker of circular buffer. Packets added to this 'end' of pool.
        CUsbPnPacket*           iPacket;

        // Pointer to oldest packet -marker of circular buffer. Packets written from this 'end' of pool.
        CUsbPnPacket*           iCurrentPacket;


    };

#endif      // CUSBPNISASENDER_H

// End of File
