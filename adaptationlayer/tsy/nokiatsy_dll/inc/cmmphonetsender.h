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



#ifndef CMMPHONETSENDER_H
#define CMMPHONETSENDER_H

// INCLUDES
#include <e32base.h>
#include <iscnokiadefinitions.h>
#include <iscapi.h>

// CONSTANTS
// Default size for send buffer
const TUint KMaxSendBufferSize = 16384; // 16k

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
    // None

// CLASS DECLARATION

/**
* CMmPhoNetSender is used for sending ISI messages to PhoNet.
*/
class CMmPhoNetSender : public CBase
    {
    public:

        /**
        * Default destcructor
        */
        ~CMmPhoNetSender();

        /**
        * Creates a new CMmPhoNetSender object
        * @param RIsaApi, reference to the PDA side Phonet
        * @return CMmPhoNetSender*, reference to the created object
        */
        static CMmPhoNetSender* NewL( RIscApi* aPn );

        /**
        * Send ISI message
        * @param TInt aResource ISI resource (server)
        * @param TInt aTransactionId ISI transaction ID
        * @param TInt aMessageId ISI message ID
        * @param const TDesC8& aData data part of message
        * @return TInt, success/failure result value
        */
        virtual TInt Send( TInt aResource,
            TInt aTransactionId,
            TInt aMessageId,
            const TDesC8& aData );

        /**
        * Send ISI message
        * @param ISI const TDesC8& aMsg ISI message to send
        * @return TInt, success/failure result value
        */
        virtual TInt Send( const TDesC8& aMsg );

        /**
        * Returns descriptor to the phonet send buffer
        * @return TDes8&: phonet send buffer
        */
        virtual TDes8& SendBufferDes();

        /**
        * Send events
        * @param TDesC8& aEvents, events to be sent
        * @return TInt, success/failure result value
        */
        virtual TInt EventSend( const TDesC8& aEvent );

        /**
        * Subscribe events from Phonet
        * @param TDesC8& aEvents, events to be subscribed
        * @return TInt, success/failure result value
        */
        TInt SubscribeEvents( const TDesC8& aEvents );

        /**
        * Get channel info. The data that is returned depends on the
        * ISC Multiplexer
		* @param aChannel The channel which info is requested
        * @param aInfo
        * @return TInt KErrNone if successful. KErrNotSupported, if not
        * supported by the ISC Multiplexer
        */
        TInt GetChannelInfo( const TUint16 aChannel, TDes8& aInfo );

    private:

        /**
        * Default constructor
        */
        CMmPhoNetSender();

        /**
        * Initialises object attributes.
        */
        void ConstructL();

    // Data
    public:
        // None

    private:

        // Pointer to the IscApi
        RIscApi* iPhoNet;

		// A pointer to isimessage buffer
		HBufC8* iSendBuffer;

		// Modifiable descriptor to isimessage buffer
		TPtr8 iSendBufferPtr;
    };

#endif // CMMPHONETSENDER_H

// End of File
