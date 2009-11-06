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



#ifndef __DP2PUSERCHANNEL_H__
#define __DP2PUSERCHANNEL_H__

// Includes
#include <kernel.h>                 // For DLogicalChannel
#include "p2prouterchannelifs.h"    // For MP2PRouterChIf
#include "p2pinternaldefs.h"        // For TP2PAsyncRequest

// Class declaration
class DMsgQueue;
class DP2PReqQueue;

/**
* Provides channel for user side P2P API
*/
NONSHARABLE_CLASS( DP2PUserChannel ) : public DLogicalChannel, public MP2PRouterChIf
    {

    public: 

        /**
        * C++ default constructor.
        */
        DP2PUserChannel();

        /**
        * Destructor.
        */
        ~DP2PUserChannel();

        ///// Functions from base class DLogicalChannel start

        /**
        * Second phase constructor. Run in user thread context.
        * Thread in CS and can not be pre-empted.
        * See other comments from DLogicalChannel.
        * @param aUnit, Not used at the moment possible to have over 32 channel.
        * @param anInfo, contains the protocol id
        * @param aVer, Not used at the moment.
        */
        virtual TInt DoCreate( TInt aUnit, const TDesC8* anInfo, const TVersion& aVer );

        /**
        * Kernel message que function. Run in kernel thread context.
        * See other comments from DLogicalChannel.
        * @param aMsg, message from Request function.
        */
        virtual void HandleMsg( TMessageBase* aMsg );

        /**
        * Overwritten so that accepts only calls with DoControl.
        * See other comments from DLogicalChannel.
        */
        TInt Request( TInt aReqNo, TAny* a1, TAny* a2 );

        ///// Functions from base class DLogicalChannel end

        ///// Functions from MP2PRouterChIf start
        /*
        * See comments from MP2PRouterChIf.
        */
        void ConnectionLost();

        /**
        * See other comments from MP2PRouterChIf.
        */
        void EnqueChannelRequestCompleteDfc( TInt aRequest, TInt aStatusToComplete );

        /*
        * Called when message to this channel is to be received.
        * @param aDataMessage, data message.
        * @return void
        */
        void ReceiveMsg( const TDesC8& aMessage );

        ///// Functions from MP2PRouterChIf end

    private:

        /*
        * Closes the channel toward router.
        * Modifies member variable iShP2PProtocolId.
        * Called in P2P extension kernel thead context.
        * @param aP2PProtocolId, protocol id of the channel to be closed.
        */
        void Close( const TUint8 aP2PProtocolId );

        /*
        * Handles actions related to closing the channel.
        */
        void Closing();

        /**
        * DFC function that completes channel requests to user thread.
        * Run always in P2P user channel kernel thread context.
        * @param aPtr, pointer to self
        * @return None
        */
        static void CompleteChReqDfc( TAny* aPtr );

        /*
        * Handling of asynchronous requests.
        * @param aFunction, request id
        * @param a1, pointer to parameters.
        * @return void
        */
        void HandleAsyncRequest( TInt aFunction, TAny* a1 );

        /*
        * Handling of cancellation of asynchronous requests.
        * @param aFunction, request id
        * @param a1, pointer to parameters.
        * @return void
        */
        TInt HandleCancelRequest( TInt aFunction, TAny* a1 );

        /*
        * Handling of all requests.
        * @param aFunction, request id
        * @param a1, pointer to parameters.
        * @param a2, not used
        * @return void
        */
        TInt HandleRequest( TInt aFunction, TAny* a1, TAny* a2 );

        /*
        * Handling of synchronous requests.
        * @param aFunction, request id
        * @param a1, pointer to parameters.
        * @return void
        */
        TInt HandleSyncRequest( TInt aFunction, TAny* a1 );

        /**
        * DFC function that empties message receive queue to user thread.
        * @param aPtr, pointer to self
        * @return None
        */
        static void EmptyRxQueueDfc( TAny* aPtr );

        /**
        * Resets channel data message and message queues.
        * @return None
        */
        void ResetQueues();

        /**
        * Do the cancellation of request.
        * @param aRequest,
        * @param aMask
        * @return None
        */
        void DoCancel( TInt aRequest, TInt aMask );

    // Internal Classes

    /*
    * 
    */
    NONSHARABLE_CLASS( TP2PReq )
        {

        public:

           TP2PReq();

           TP2PReq( TP2PAsyncRequest aReq, TInt aStat );

           // Data
           TP2PAsyncRequest    iRequest;
           TInt                iCompleteStatus;

        };

    /*
    * Handles requests in a ring buffer manner.
    * Syncronization protection by NFastMutex.
    */
    NONSHARABLE_CLASS( DP2PReqQueue ) : public DBase
        {

        public:

            /*
            * Default constructor
            */
            DP2PReqQueue();

            /*
            * Destructor
            */
            ~DP2PReqQueue();

            /* 
            * In case of queue overflow throw kern::fault
            * Adds the message in the end of the ringbuffer queue.
            * @param aReq
            */
            TBool Add( TP2PReq aReq );

            /*
            *Returns ETrue if empty.
            */
            TBool Empty();

            /*
            *Returns first request.
            */
            TP2PReq Get();

            /*
            * Set req active / deactive. Default deactive.
            */
            void SetReq( TP2PAsyncRequest aReqToSet, TRequestStatus* aStatus = NULL );

            /*
            * Get req. If NULL, request not pending.
            */
            TRequestStatus* GetReq( TP2PAsyncRequest aReqToGet );

      private:

            // Index where to add next message pointer.
            TUint8          iShInputIndex;
            // Index where to get next message pointer.
            TUint8          iShOutputIndex;
            // Count of items in the array.
            TUint8          iShCount;
            // Size of the ring buffer. Write only in constructor.
            TUint8          iSize;
            // Guards internal states of this class.
            NFastMutex*     iQueueMutex;
            // Ring buffer must be as big as we have request. TODO: is there a danger to get more request..?
            TP2PReq         iShReqRingBuffer[ EP2PLastAsyncRequest ];
            // Active / Deactive requests are kept in list to make finding easier.
            TRequestStatus* iShReqList[ EP2PLastAsyncRequest ];

        };

    private:

        // Added convention:
        // Prefix "iSh" means shared member variable
        // Prefix "gSh" means shared global variable

        // Data members
        // Channel number of this channel.
        TUint8              iShP2PProtocolId;
        // Mutex guarding iShP2PProtocolId, not owned
        DMutex*             iShP2PProtocolIdMutex;
        // Pointers to client message buffer, not owned
        TAny*               iReceiveBufPtr;
        // Owned, synchronized internally with fastmutexes no need to guard
        DMsgQueue*          iRx;
        // Owned, no need to sync don internally
        TDfc*               iEmptyRxQueueDfc;
        // Owned, no need to sync don internally
        TDfc*               iCompleteChannelRequestDfc;
        // Owned, synchronized internally with fastmutexes.
        DP2PReqQueue*       iP2PReqQueue;
        // Not owned, no need to guard open/close in one thread in construction and desctruction.
        DThread*            iThread;
        // Not owned, no need to guard only written in one thread in construction and desctruction.
        MP2PChRouterIf*     iRouterIf;

    };

#endif      // __DP2PUSERCHANNEL_H__
            
// End of File