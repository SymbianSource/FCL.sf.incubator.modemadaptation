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



#ifndef __ISIKERNELCHANNEL_H__
#define __ISIKERNELCHANNEL_H__

#include <e32def.h>                 // For TInt, TAny
#include <e32cmn.h>                 // For TDesC8
#include <kernel.h>                 // For TMessageQue
#include "misirouterchannelif.h"    // For MISIRouterChannelIf
#include "isiinternaldefs.h"        // For TISIAsyncRequest


// Forward declarations
class DISIMsgQueue;
class MISIChannelRouterIf;

/*
* Kernel side ISI channel.
*/
NONSHARABLE_CLASS( DISIKernelChannel ) : public DBase, public MISIRouterChannelIf
    {

    public:

        /*
        * Default constructor
        */
        DISIKernelChannel( const TUint16& aObjId );

        /*
        * Destructor
        */
        ~DISIKernelChannel();

        /*
        * Handles the thread message. Synchronizes the execution
        * from multiple client threads to single thread.
        * @param aMsg, thread message
        * @return, return value of the operation.
        */
        TInt HandleRequest( TThreadMessage& aMsg );

        // From MISIRouterChannelIf start
        
        /*
        * See comments from MISIRouterChannelIf.
        */
        void ReceiveMsg( const TDesC8& aMessage );

        /*
        * See comments from MISIRouterChannelIf.
        */
        void EnqueChannelRequestCompleteDfc( TInt aRequest, TInt aStatusToComplete );
        
        // From MISIRouterChannelIf end

        /*
        * iKernelChMsgQue callback function in p2p router
        * thread context.
        * @param aPtr, pointer to self
        */
        static void MsgQDfc( TAny* aPtr );

        static void EmptyRxDfc( TAny* aPtr );

    private:
      
        void HandleThreadMsg( TThreadMessage& aMsg );
      
        void HandleDfcRequest( TThreadMessage& aMsg );

        TInt HandleSyncRequest( TThreadMessage& aMsg );

        void DoCancel( TInt aRequest, TInt aMask );
      
        void ResetQueues();
        
        void Disconnect();

        /*
        * Internal class for handling requests.
        */
        NONSHARABLE_CLASS( DISIKernelAsyncRequests ) : public DBase
            {

            public:

                /*
                * Constructor
                * @param aSize, sife of the request list.
                */
                DISIKernelAsyncRequests( const TInt aSize );

                /*
                * Destructor
                */
                ~DISIKernelAsyncRequests();

                /*
                * Completes the request if it is pending.
                * Do not call with FM held.
                * @param aRequest, request to complete
                * @param aStatusToComplete, completion status to request to complete
                */
                void Complete( const TUint aRequest, const TInt aStatusToComplete );
                
                /*
                * Check does is the request pending.
                * Do not call with FM held.
                * @param  aRequest, request to check
                */
                TBool IsPending( const TUint aRequest );
                
                /*
                * Set the request pending.
                * Do not call with FM held.
                * @param aRequest, request to set pending
                * @param aDfc, pointer to DFC function which is enqued when request is completed
                * @param aStatus, pointer to status which is modified when request is completed
                */
                void SetPending( const TUint aRequest, TDfc* aDfc, TRequestStatus* aStatus );

            private:

                // Guarding lock.
                NFastMutex*           iRequestLock;
                TDfc**                iDfcFunctionList;
                TRequestStatus**      iRequestStatusList;

            };
      
    private:
        
        // Synhronized internally with fastmutexes
        DISIKernelAsyncRequests*  iRequests;
        // Not owned, just using
        // Only acccessed from ISI Router thread.
        TMessageQue               iKernelChMsgQue;
        TDes8**                   iPtrPtrToRxBuf;
        MISIChannelRouterIf*      iRouterIf;
        // Owned
        TDfc*                     iEmptyRxDfc;
        TDfcQue*                  iMainThread;
        TDfcQue*                  iCompletionThread;
        // Owned, synhronized internally with fastmutexes
        DISIMsgQueue*                iRx;
        TUint8                    iObjId;
        TInt32                    iUID;
        
        
    };

#endif /* __ISIKERNELCHANNEL_H__ */
