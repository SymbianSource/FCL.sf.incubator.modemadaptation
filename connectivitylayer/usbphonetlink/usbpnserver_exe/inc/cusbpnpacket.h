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


#ifndef CUSBPNPACKET_H
#define CUSBPNPACKET_H

//  INCLUDES
#include <e32base.h>    // For CBase

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


/**
*  CUsbPnPacket
*  Implements circular data packet buffer functionality.
*/
class CUsbPnPacket : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CUsbPnPacket* NewL( CUsbPnPacket* aPacket, TInt aNum );

        /**
        * Destructor
        */
        ~CUsbPnPacket();

        /**
        * Returns the next packet.
        * @return CUsbPnPacket* The next packet.
        */
        CUsbPnPacket& NextPacket() const;

        /**
        * Returns the buffer.
        * @return TDes8* Buffer
        */
        HBufC8& Buffer() const;

        /**
        * Returns true if packet contains data already.
        * @return TBool Sync flag.
        */
        TBool PacketInUse() const;

        /**
        *
        */
        TInt PacketNumber() const;

        /**
        *Release packet's buffer after usage.
        *Realloc to standard size if bigger than usual.
        */
        void ReleaseL();

        /**
        *Realloc packet's buffer if bigger needed.
        */
        void ReallocBufferL( const TInt aNeededLength );

    private:

        /**
        * Constructor. Allocates memory.
        * @param CUsbPnPacket* aUsbPacket   A packet which is linked as next packet to this packet.
        * @param TInt aNum                  Serial number of the created packet for easing debugging.
        */
        CUsbPnPacket( CUsbPnPacket* aPacket, TInt aNum );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /* defined for preventing misuse */
        explicit CUsbPnPacket(const CUsbPnPacket&);
        CUsbPnPacket& operator=(const CUsbPnPacket&);


        // Pointer to next packet in circular buffer.
        CUsbPnPacket*    iNextPacket;

        // Ownership. Pointer to buffer
        HBufC8*          iBuffer;

        // Order number
        TInt            iNumber;
    };




#endif  // #ifndef CUSBPNPACKET_H
