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


#ifndef CUSBPNUSBRECEIVER_H
#define CUSBPNUSBRECEIVER_H

//  INCLUDES
#include <e32base.h>                // For CActive

#include "musbpnbufferlistener.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CUsbPnIsaSender;
class MUsbPnControlObserver;
class CUsbPnPacket;
class RDevUsbcClient;
class RIscApi;

// CLASS DECLARATION

/**
*  Receiver for data coming from USB.
*/
class CUsbPnUsbReceiver : public CActive, public MUsbPnBufferListener
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUsbPnUsbReceiver* NewL( RDevUsbcClient& aLdd, RIscApi& aIsc );

        /**
        * Destructor.
        */
        ~CUsbPnUsbReceiver();

    public: // New functions

        /**
        * Get pointer for sending control messages.
        * @return pointer to control observer API
        */
        MUsbPnControlObserver* IsaSender();

        /**
        * Sets state to enumerated.
        * @param aState ETrue says usb is enumerated
        */
        void SetEnumerated( TBool aState );

    public: // Functions from base classes

        /**
        * From MUsbPnBufferListener. Called by CUsbPnInterface when initialised
        * or by sender when reactivating receive.
        * @param TBool aDiscard. Flag for determining if data can be transmitted to USB.
        */
        void Receive( TBool aDiscard );


    protected:  // Functions from base classes

        /**
        * From CActive.
        */
        void DoCancel();

        /**
        * From CActive.
        */
        void RunL();

        /**
        * From CActive.
        */
        TInt RunError( TInt );


    private:

        /**
        * C++ default constructor.
        */
        CUsbPnUsbReceiver( RDevUsbcClient& aLdd );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RIscApi& aIscApi );


        /**
        * Construct message as needed by receiver.
        */
        void ConstructMessageL();

        /**
        * Set message complete flag
        * @param aMsgComplete ETrue says message constructed
        */
        void SetMessageComplete( const TBool aMsgComplete );

        /**
        * Set message byte count
        * @param aByteCount counts packet bytes. Append to previous if next msg
        */
        void SetByteCount( const TUint aByteCount );

        /**
        * Set message length
        * @param aMessageLength ISI msg length  
        */
        void SetMessageLength( const TUint aMessageLength );

    private:    // Data

        // Ownership. Object for forwarding received data.
        CUsbPnIsaSender*        iIsaSender;

        // Reference to USB driver ldd.
        RDevUsbcClient&         iLdd;

        // Not owned. Pointer to receivable packet
        CUsbPnPacket*           iPacket;

        // Modifiable ptr for receiving
        TPtr8                   iRecvPtr;

        // Whole message received
        TBool                   iMessageComplete;

        // ISI message length
        TUint                   iMessageLength;

        // Whole message length
        TUint                   iByteCount;

        // Ownership. Temporary buffer for first part of exceptionally big message
        HBufC8*                 iStorage;

        TBool                   iEnumerated;

    };

#endif      // CUSBPNUSBRECEIVER_H

// End of File
