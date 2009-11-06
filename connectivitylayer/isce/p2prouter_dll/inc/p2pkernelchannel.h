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



#ifndef __P2PKERNELCHANNEL_H_
#define __P2PKERNELCHANNEL_H_

#include <e32def.h>                 // For TInt, TAny
#include <e32cmn.h>                 // For TDesC8, 
#include <kernel.h>                 // For TMessageQue;
#include "p2prouterchannelifs.h"    // For MP2PRouterChIf

// Forward declarations
class TDfc;
class MP2PChRouterIf;
class TThreadMessage;
class DMsgQueue;

/*
* Kernel side P2P channel.
*/
NONSHARABLE_CLASS( DP2PKernelChannel ) : public MP2PRouterChIf
    {

    public:

        /*
        * Default constructor
        */
        DP2PKernelChannel();

        /*
        * Destructor
        */
        ~DP2PKernelChannel();

        /*
        * Handles the thread message. Synchronizes the execution
        * from multiple client threads to single thread.
        * @param aMsg, thread message
        * @return, return value of the operation.
        */
        TInt HandleRequest( TThreadMessage& aMsg );

        // From MP2PRouterChIf start
        /*
        * See comments from MP2PRouterChIf.
        */
        void ConnectionLost();

        /*
        * See comments from MP2PRouterChIf.
        */
        void EnqueChannelRequestCompleteDfc( TInt aRequest, TInt aStatusToComplete );

        /*
        * See comments from MP2PRouterChIf.
        */
        void ReceiveMsg( const TDesC8& aMessage );
        // From MP2PRouterChIf end

    private:

        void Close( const TUint8 aP2PProtocolId );

        void Closing( const TUint8 aP2PProtocolId );

        static void EmptyRxDfc( TAny* aPtr );

        /*
        * iKernelChMsgQue callback function in p2p router
        * thread context.
        * @param aPtr, pointer to self
        */
        static void MsgQDfc( TAny* aPtr );

        void HandleThreadMsg( TThreadMessage& aMsg );

        void HandleDfcRequest( TThreadMessage& aMsg );

        TInt HandleSyncRequest( TThreadMessage& aMsg );

        void DoCancel( TInt aRequest, TInt aMask );

        void ResetQueues();

        /*
        * Internal class for handling requests.
        */
        NONSHARABLE_CLASS( DP2PKernelAsyncRequests ) : public DBase
            {

            public:

                /*
                * Constructor
                * @param aSize, sife of the request list.
                */
                DP2PKernelAsyncRequests( const TInt aSize );

                /*
                * Destructor
                */
                ~DP2PKernelAsyncRequests();

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
                void SetPending( const TUint aRequest, TDfc* aDfc, TInt* aStatus );

            private:

                // Added convention:
                // Prefix "iSh" means shared member variable
                // Prefix "gSh" means shared global variable

                // Guarding lock.
                NFastMutex* iRequestLock;
                TDfc**      iShDfcFunctionList;
                TInt**      iShRequestStatusList;

            };

  private:

        // Protocol id of this channel.
        // Only acccessed from p2p router thread.
        TUint8                      iP2PProtocolId;
        // Owned, synhronized internally with fastmutexes
        DMsgQueue*                  iRx;
        // Owned
        TDfc*                       iEmptyRxDfc;
        // Not owned, just using
        // Only acccessed from p2p router thread.
        TDes8**                     iPtrPtrToRxBuf;
        MP2PChRouterIf*             iRouterIf;
        TMessageQue                 iKernelChMsgQue;
        // Synhronized internally with fastmutexes
        DP2PKernelAsyncRequests*    iRequests;

    };



#endif /* __P2PKERNELCHANNEL_H_ */
