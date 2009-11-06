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



#ifndef DPMIF_H
#define DPMIF_H

// INCLUDES
#include <iscapi.h>
#include <iscnokiadefinitions.h>

// LOCAL CONSTANTS
// none

// DATA TYPES
// none

// FORWARD DECLARATIONS
class TIsiReceiveC;

// CLASS DECLARATION

/**
*  CDpMif is message interface towards DCS.
*  CDpMif receives PEP messages from DCS and sends PEP messages to DCS. CDpMif
*  handles also  registration of name to Name Service and removal of name from
*  Name Service.
* 
*  CDpMif uses pre-reserved buffers to receive and send ISI messages.
*/
class CDpMif :public CActive
    {

    public:  // Constructors and destructor

        /**
        * Static constructor.
        * @param aDataPort: Dataport main class
        */
        static CDpMif* NewL( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpMif();

    public:  // New methods

        /**
        * This method makes an asynchronous receive request to IsaApi.
        * @return TInt: Error code
        */
        TInt Read();

        /**
        * Send ISI message
        * @param aResource: ISI resource (server)
        * @param aTransactionId: ISI transaction ID
        * @param aMessageId: ISI message ID
        * @param aMessage: ISI message
        * @return TInt: Success/failure result value
        */
        TInt SendMessage(
            TUint8 aResource,
            TUint8 aTransactionId,
            TUint8 aMessageId,
            const TDesC8& aMessage );

        /**
        * Send ISI message
        * @param aRecDevice: ISI receiver device
        * @param aRecObject: ISI receiver object
        * @param aResource: ISI resource (server)
        * @param aTransactionId: ISI transaction ID
        * @param aMessageId: ISI message ID
        * @param aMessage: ISI message
        * @return TInt: success/failure result value
        */
        TInt SendMessage(
            TUint8 aRecDevice,
            TUint8 aRecObject,
            TUint8 aResource,
            TUint8 aTransactionId,
            TUint8 aMessageId,
            const TDesC8& aMessage );

        /**
        * This method sends name add request to Name Service.
        * Message's proxy ID depends on the port unit number.
        * @param aUnit: Dataport unit number
        */
        void SendNameAddReqL( const TUint aUnit );

        /**
        * This method sends name remove request to Name Service.
        * @param aUnit: Dataport unit number
        */
        void SendNameRemoveReqL( const TUint aUnit );

    public:  // Methods from base classes

        /**
        * From CActive This method cancels IsaApi receive.
        */
        void DoCancel();

        /**
        * From CActive This method handles received message from IsaApi
        * and renews read from IsaApi.
        */
        void RunL();

        /**
        * From CActive This method handles leave in RunL().
        * This method returns the given error value.
        * @param aError: Leave reason
        * @return TInt: Leave reason is returned in error code
        */
        TInt RunError( TInt aError );

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aDataPort: Dataport main class
        */
        CDpMif( CDpDataPort& aDataPort );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:  // New methods

        /**
        * This method handles pep control request. Handling is forwarded to
        * COMM PEP, if pep type is correct. Response is sent with possible
        * error code.
        * @param aIsiMessage: Reference to the received message.
        */
        void HandlePepCtrlReq( TIsiReceiveC& aIsiMessage );

        /**
        * This method handles pep status indication. This method forwads
        * handling to COMM PEP, if pep type is correct. 
        * @param aIsiMessage: Reference to the received message.
        * @return TInt: Error code
        */
        TInt HandlePepStatusInd( TIsiReceiveC& aIsiMessage );

    private:  // Data

        // DataPort
        CDpDataPort& iDataPort;

        // Pipe interface (PIF)
        CDpPif& iPifDcs;

        // COMM PEP
        CDpCommPep& iCommPep;

        // Pre-reserved receive message buffer
        HBufC8* iMsgReceiveBuffer;

        // Pointer for receive message buffer
        TPtr8 iReceiveMessage;

        // Pre-reserved send message buffer
        HBufC8* iMessageSendBuffer;

        // Pointer for send message buffer
        TPtr8 iSendMessage;

        // Length of received message
        TUint16 iLen;

    };

#endif  // DPMIF_H

//  End of File
