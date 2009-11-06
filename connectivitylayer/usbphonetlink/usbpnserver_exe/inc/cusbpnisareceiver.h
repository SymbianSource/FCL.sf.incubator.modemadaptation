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


#ifndef CUSBPNISARECEIVER_H
#define CUSBPNISARECEIVER_H

//  INCLUDES
#include <e32base.h>

#include "musbpnbufferlistener.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MUsbPnBufferListener;
class CUsbPnUsbSender;
class CUsbPnPacket;
class RIscApi;
class RDevUsbcClient;

// CLASS DECLARATION

/**
*  Message sender for ISA direction.
*/
class CUsbPnIsaReceiver : public CActive, public MUsbPnBufferListener
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUsbPnIsaReceiver* NewL( RIscApi& aIscApi, RDevUsbcClient& aLdd );

        /**
        * Destructor.
        */
        ~CUsbPnIsaReceiver();


    public: // Functions from base classes

        /**
        * From MUsbPnBufferListener. Called by CUsbPnInterface when initialised
        * or by sender when reactivating receive.
        * @param TBool aDiscard. Flag for determining if data can be transmitted to USB.
        */
        void Receive( TBool aDiscard );

        /**
        * Handles flag that identify if Phonet connection is open to PC
        * @param TBool aForwardFlag. Flag for determining if data can be transmitted to USB.
        */
        void SetForwardFlag( TBool aForward );


    protected: // Functions from base classes

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
        CUsbPnIsaReceiver( RIscApi& );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RDevUsbcClient& );

        /**
        * Construct message as needed by receiver.
        */
        void ConstructMessage();


    private:    // Data

        // Ownership. Pointer for sending received data to USB
        CUsbPnUsbSender*        iUsbSender;

        // Reference to ISCAPI handle for receiving data
        RIscApi&                iIscApi;

        //Not owned. Packet to receive to
        CUsbPnPacket*           iPacket;

        // Modifiable pointer to buffer.
        TPtr8                   iRecvPtr;

        // ISCAPI tells if buffer is too short
        TUint16                 iNeededLength;

        // Flag for discarding when USB is not active
        TBool                   iForwardFlag;

    };

#endif      // CUSBPNISARECEIVER_H

// End of File
