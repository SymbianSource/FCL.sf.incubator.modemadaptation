/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMPHONETRECEIVER_H
#define CMMPHONETRECEIVER_H

// INCLUDES
#include <e32base.h>
#include <iscapi.h>

// CONSTANTS
// Default size for receive buffer
const TUint KDefaultReceiveBufferSize = 16384; // 16k

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class RIscApi;
class TIsiReceiveC;

// CLASS DECLARATION

/**
* Message receiver interface
*/
class MMmMessageReceiver
    {
    public:

        /**
        * Virtual ReceiveMessageL method. Implemented in message handlers.
        * @param const TIsiReceiveC &aIsiMessage: The received ISI message
        */
        virtual void ReceiveMessageL( const TIsiReceiveC& /*aIsiMessage*/ )
        	{
    		// no code
        	}

        /**
        * Virtual HandleError method. Implemented in message handlers.
        * @param const TIsiReceiveC &aIsiMessage: The received ISI message
        * @param TInt aError: Error code
        */
        virtual void HandleError( const TIsiReceiveC& /*aIsiMessage*/,
            TInt /*aError*/ )
        	{
        	// no code
        	}
    };

/**
* CMmPhoNetReceiver is used for receiving ISI messages from PhoNet.
*/
class CMmPhoNetReceiver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param RIsaApi* aPn: pointer to the Phonet
        * @return CMmPhoNetReceiver*: created object
        */
        static CMmPhoNetReceiver* NewL( RIscApi* aPn );

        /**
        * Destructor.
        */
        ~CMmPhoNetReceiver();

    public: // New functions

        struct TMessageReceiverInfo
            {
            TInt iResource;
            TInt iMessageId;
            MMmMessageReceiver* iReceiver;
            };

        /**
        * Registers message handler to CMmPhoNetReceiver
        */
        void RegisterL( MMmMessageReceiver* aReceiver,
            TInt aResource,
            TInt aMessageId = -1 );
 
        /**
        * When RunL leaves, RunError is called.
        * @param TInt aError: Error from RunL
        * @return TInt: KErrNone
        */
        TInt RunError( TInt aError );

        /**
        * Handles an active object’s request completion event.
        */
        void RunL();

        /**
        * Receive message from PDA phonet
        */
        void ReceiveL( TInt aBufferSize = KDefaultReceiveBufferSize );

        /**
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Sets SAT MessageHandler.
        * @param MMmMessageReceiver*: pointer to SAT message handler
        * @return TInt: success/failure indication
        */
        virtual TInt SetSatMessHandler( MMmMessageReceiver* aSatMessHandler );

		/**
        * Sat ready indication
		* @param TUint8: Message to be retrieved.
		* @return TInt: success/failure if msg was found
        */
        virtual TInt SatReady( TUint8 aMsg );

    private:

        /**
        * By default EPOC constructor is private.
        */
        CMmPhoNetReceiver();

        /**
        * Class attributes are created in ConstructL
        */
        void ConstructL();

        /**
        * Routes received ISI message to the correct message handler
        */
        void DispatchL( const TIsiReceiveC& aIsiMessage );

    // Data
    public:
        // None

    private:

        // A pointer to the iscapi object
        RIscApi* iPhoNet;

        RArray<TMessageReceiverInfo> iMsgReceivers;

        // A pointer to the called MessageReceiver object
        MMmMessageReceiver* iMessageReceiver;

		// a pointer to isimessage buffer
		HBufC8* iMessageBuffer;

		// Modifiable descriptor to isimessage buffer
		TPtr8 iMessageBufferPtr;

        // Needed buffer size for overly large phonet message
        TUint16 iNeededBufferLength;

        // A pointer to the sat message handler
        MMmMessageReceiver* iSatMessHandler;

        // A pointer to the stored isimessage for sat
        HBufC8* iSatMessageBuffer;
    };

#endif // CMMPHONETRECEIVER_H

// End of file
