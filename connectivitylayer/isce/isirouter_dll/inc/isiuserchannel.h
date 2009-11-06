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



#ifndef __ISIUSERCHANNEL_H__
#define __ISIUSERCHANNEL_H__

// Includes
#include <kernel.h>                 // For DLogicalChannel
#include "misirouterchannelif.h"    // For MISIRouterChannelIf
#include "isiinternaldefs.h"        // For TISIAsyncRequest

// Forward declaration
class DISIMsgQueue;
class MISIChannelRouterIf;

/**
* Provides channel for user side ISI API
*/
NONSHARABLE_CLASS( DISIUserChannel ) : public DLogicalChannel, public MISIRouterChannelIf
    {

    public: 

        /**
        * C++ default constructor.
        */
        DISIUserChannel();

        /**
        * Destructor.
        */
        ~DISIUserChannel();

        ///// Functions from base class DLogicalChannel start

        /**
        * Second phase constructor. Run in user thread context.
        * Thread in CS and can not be pre-empted.
        * See other comments from DLogicalChannel.
        * @param aUnit, Not used at the moment.
        * @param anInfo, Not used at the moment.
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

        ///// Functions from MISIRouterChannelIf start
        
        /**
        * See comments from MISIRouterChannelIf.
        */
        void EnqueChannelRequestCompleteDfc( TInt aRequest, TInt aStatusToComplete );
        
        /*
        * See comments from MISIRouterChannelIf.
        */
        void ReceiveMsg( const TDesC8& aMessage );

        ///// Functions from MISIRouterChannelIf end

    private:

        /*
        * Handling of all requests.
        * @param aFunction, request id
        * @param a1, pointer to parameters.
        * @param a2, not used
        * @return error code
        */
        TInt HandleRequest( TInt aFunction, TAny* a1, TAny* a2 );

        /*
        * Handling of synchronous requests.
        * @param aFunction, request id
        * @param a1, pointer to parameters.
        * @return error code
        */
        TInt HandleSyncRequest( TInt aFunction, TAny* a1 );

        /*
        * Handling of asynchronous requests.
        * @param aFunction, request id
        * @param a1, pointer to parameters.
        * @return None
        */
        void HandleAsyncRequest( TInt aFunction, TAny* a1 );

        /*
        * Do the cancellation of request.
        * @param aRequest,
        * @param aMask
        * @return None
        */
        void DoCancel( TInt aRequest, TInt aMask );

        /*
        * Disconnect channel
        * @return None
        */
        void Disconnect();
        
        /**
        * DFC function that empties message receive queue to user thread.
        * @param aPtr, pointer to self
        * @return None
        */
        static void EmptyRxQueueDfc( TAny* aPtr );

        /**
        * DFC function that completes channel requests to user thread.
        * Run always in ISI user channel kernel thread context.
        * @param aPtr, pointer to self
        * @return None
        */
        static void CompleteChannelRequestDfc( TAny* aPtr );

        /**
        * Resets channel message queues.
        * @return None
        */
        void ResetQueues();

        /*
        * Internal class for handling async requests.
        */
        NONSHARABLE_CLASS( DISIUserAsyncRequests ) : public DBase
            {

            public:

                /*
                * Constructor
                * @param aSize, size of the request list.
                */
                DISIUserAsyncRequests( const TInt aSize );

                /*
                * Destructor
                */
                ~DISIUserAsyncRequests();
                
                /*
                * Check is the request pending.
                * Do not call with FM held.
                * @param aRequest, request to check
                * @return pending status
                */
                TBool IsPending( const TUint aRequest );
                
                /*
                * Set the request pending.
                * Do not call with FM held.
                * @param aRequest, request to set pending
                * @param aDfc, pointer to DFC function which is enqued when request is completed
                * @param aStatus, pointer to status which is modified when request is completed
                */
                void SetPending( const TUint aRequest, TRequestStatus* aStatus = NULL );
                
            private:
                
                // For list access
                friend class    DISIUserChannel;
                // Guarding lock.
                NFastMutex*     iRequestLock;
                // For clients request status pointers
                TRequestStatus* iRequestStatusList[ EISILastAsyncRequest ];
                // Request completion values
                TInt            iRequestCompletionValueList[ EISILastAsyncRequest ];
                // Request completion queue
                RArray<TInt>    iRequestCompletedList;
                           
            };
  
    private:

        // Not owned, no need to guard open/close in one thread in construction and desctruction.
        DThread*               iThread;
        // Not owned, no need to guard only written in one thread in construction and desctruction.
        MISIChannelRouterIf*   iRouterIf;
        // Pointers to client message buffer, not owned
        TAny*                  iReceiveBufPtr;
        // not owned
        TUint16*               iNeededBufLen;
        // Owned, synchronized internally with fastmutexes.
        DISIUserAsyncRequests* iRequests;
        // Owned, no need to sync don internally
        TDfc*                  iEmptyRxQueueDfc;
        // Owned, no need to sync don internally
        TDfc*                  iCompleteChannelRequestDfc;
        TDfcQue*               iMainThread;
        TDfcQue*               iCompletionThread;
        // Owned, synchronized internally with fastmutexes no need to guard
        DISIMsgQueue*             iRx;
        // Channel indentifiers
        TUint8                 iObjId;
        TInt32                 iUID;

    };

#endif      // __ISIUSERCHANNEL_H__
            
// End of File