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



#ifndef __P2PROUTERCHANNELIFS_H__
#define __P2PROUTERCHANNELIFS_H__

#include <e32def.h>     // For TInt, TAny

/*
* Abstract interface for router to use channel services.
*/
class MP2PRouterChIf
    {

    public:

        /*
        * Closes the connection and interface.
        * Called in P2P router ext thread context.
        */
        virtual void ConnectionLost() = 0;

        /*
        * Enques channel request completing DFC.
        * Called in P2P router ext thread context.
        * @param aRequest, request to be completed
        * @param aStatusToComplete, status to complete to client.
        */
        virtual void EnqueChannelRequestCompleteDfc( TInt aRequest, TInt aStatusToComplete ) = 0;

        /*
        * Receive a message from router.
        * Responsibility to deallocate the message is transferred to channel.
        * Can be called in 1..N thread contextes.
        * Cat no be called with FM held.
        * Same restríctions than in IST API (see from MTrxMuxIf).
        * @param aMessage, message to receive
        */
        virtual void ReceiveMsg( const TDesC8& aMessage ) = 0;

    };

/*
* Abstract interface for channel to use router services.
*/
class MP2PChRouterIf
    {

    public:

        // Enumerations for different threads.
        enum TP2PDfcThread
            {
            EP2PDfcThread = 0,
            EP2PLddDfcThread,
            EAmountOfP2PDfcThreads
            };

        /*
        * Informs router that channel is closed.
        * @param aProtocolId, closing protocol id
        */
        virtual void Close( const TUint8 aProtocolId ) = 0;

        /*
        * Return ETrue if the connection is lost.
        * Called in P2P router ext thread context.
        */
        virtual TBool ConnectionExist( const TUint8 aProtocolId ) = 0;

        /*
        * Returns thread according to TP2PDfcThread enums.
        * @param aThreadIndex, threads index in table
        * @return pointer to thread
        */
        virtual TDfcQue* GetDfcThread( const TP2PDfcThread aThreadIndex ) = 0;

        /*
        * Returns the interface.
        * @return pointer to the interface
        */
        static MP2PChRouterIf* GetIf();

        /*
        * Informs router that channel is ready to be open.
        * If transceiver is not found sets the open pending.
        * Router shall complete the pending opens when transceiver is ready.
        * @param aProtocolId, channels unique P2P identifier
        * @param aCallback, pointer to channel interface
        */
        virtual void Open( const TUint8 aProtocolId, MP2PRouterChIf* aCallback ) = 0;

        /*
        * Sends a message to router.
        * Responsibility to deallocate the message is transferred router.
        * @param aMessage, messages reference
        * @aProtocolId, channels unique P2P identifier
        * @return, KErrNone if succesfull, KErrNotReady if transceiver is not present.
        */
        virtual TInt Send( TDes8& aMessage, const TUint8 aProtocolId ) = 0;

    };

#endif /* __P2PROUTERCHANNELIFS_H__ */
