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



#ifndef DISAUSERCHANNEL_H
#define DISAUSERCHANNEL_H

//  INCLUDES
#include <kernel.h>             // For DLogicalChannel
#include "internalapi.h"        // For MIAD2ChannelApi, MChannel2IADApi

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class DQueue;
class DReqQueue;
class MDataMessageStatus;


/**
*  Channel to user side.
*
*  @lib ISAAccessldd.ldd
*/
class DISAUserChannel : public DLogicalChannel, public MIAD2ChannelApi
    {

    public: 

        /**
        * C++ default constructor.
        */
        IMPORT_C DISAUserChannel();

        /**
        * Destructor.
        */
        ~DISAUserChannel();

        ///// Functions from base class DLogicalChannel start
        /**
        * Second phase constructor. Run in user thread context. More description from DLogicalChannel.
        * @param aUnit
        * @param anInfo
        * @param aVer
        */
        virtual TInt DoCreate( TInt aUnit, const TDesC8* anInfo, const TVersion& aVer );

        /**
        * Kernel message que function.
        * @param aMsg
        * @return None
        */
        virtual void HandleMsg( TMessageBase* aMsg );

        /**
        * C++ default constructor.
        */
        TInt Request( TInt aReqNo, TAny* a1, TAny* a2 );
        ///// Functions from base class DLogicalChannel end

        ///// Functions from MIAD2ChannelApi start

        /**
        * Completes a channel request.
        * @param aRequest, request to complete
        * @param aStatusToComplete, status of the request to complete.
        * @return void
        */
        void CompleteChannelRequest( TInt aRequest, TInt aStatusToComplete );

        /*
        * Notifies the connection status.
        * @param aStatus, connection status
        * @return void
        */
        void NotifyConnectionStatus( TIADConnectionStatus aStatus);

        /*
        * Notifies the state of data flowcontrol to this channel.
        * @param aStatus, status of flowcontrol
        * @return void
        */
        void NotifyFlowCtrl( TIADFlowControlStatus aStatus );

        /*
        * Called when datamessage to this channel is to be received.
        * When data message is written to user thread call MDataMesssageStatus::DataMessageDelivered.
        * It's responsibility is to tell PEP that one data message e.g.a credit is used.
        * PEP's should also keep count of data messages added to channel to be abel to control the legacy flowcontrol.
        * @param aDataMessage, data message.
        * @return void
        */
        void ReceiveDataMsg( const TDesC8& aDataMessage );

        /*
        * Called when message to this channel is to be received.
        * @param aDataMessage, data message.
        * @return void
        */
        void ReceiveMsg( const TDesC8& aMessage );

        /*
        * Register pep associated to this channel to listen data message deliveries.
        * @param aPep, reference to pep.
        * @return void
        */
        void RegisterPep( MDataMessageStatus& aPep );
        ///// Functions from MIAD2ChannelApi end.

        /*
        * Returns the channelid of this channel.
        * @return TUint16, a channel id.
        */
        inline TUint16 GetChannelNumber(){ return iChannelNumber; };

        //From objectapi
        void Receive( const TDesC8& aMessage );

        ///// Functions for internal use.

    private:

        /*
        * Kernel message que function to be used in LDD thread context.
        * @param aPtr, pointer to self
        * @return void
        */
        static void MsgQueFunc( TAny* aPtr );

        /*
        * DFC function that releases blocks used for messages.
        * @param aPtr, pointer to self
        * @return void
        */
        static void CleanBlocksDfc( TAny* aPtr );

        /*
        * DFC function that releases blocks used for data messages.
        * @param aPtr, pointer to self
        * @return void
        */
        static void CleanDataBlocksDfc( TAny* aPtr );

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
        * @param a2, pointer to parameters.
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
        * DFC function that empties data message receive queue to user thread.
        * @param aPtr, pointer to self
        * @return None
        */
        static void EmptyDataRxQueueDfc( TAny* aPtr );

        /*
        * DFC function to write and complete connection status change to user client.
        * @param aPtr, pointer to self.
        * @return void
        */
        static void ConnectionStatusDfc( TAny* aPtr );

        /**
        * DFC function that completes channel requests to user thread.
        * @param aPtr, pointer to self
        * @return None
        */
        static void CompleteChReqDfc( TAny* aPtr );

        /**
        * DFC function that completes flowcontrol status to user thread.
        * @param aPtr, pointer to self
        * @return None
        */
        static void FlowCtrlStatusDfc( TAny* aPtr );

        /**
        * Resets channel data message and message queues.
        * @return None
        */
        void ResetQueues();

        /**
        * Checks that function is run in appropriate thread context.
        * NOTE! to be used only in UDEB for R&D purposes.
        * @return None
        */
        static void CheckDfc();

        /**
        * Do the cancellation of request.
        * @param aRequest,
        * @param aMask
        * @return None
        */
        void DoCancel( TInt aRequest, TInt aMask );

// CHECK FROM ALL HEADER FILES!!
//
// Compiler puts variables to 4bytes segments if 1,2,3 add filler bytes.
// So let's arrange variables so that they best fit in to 4byte segments.
// Also let's change TBool to TUint8 cause TBool is word (unless ANSI recommended bool is used.

    private: // Data members

        // Channel number of this channel.
        TUint16             iChannelNumber;
        TAny*               iIADUserChannelNumberPtr;
        // not owned
        TAny*               iIADConnectionStatusPtr;
        TInt                iConnectionStatus;
        // Pointer to client flow control status, not owned
        TAny*               iIADFlowControlStatusPtr;
        TInt                iFlowCtrlStatus;
        TInt                iLastNotifiedFlowCtrlStatus;
        // Pointers to client message buffers, not owned
        TAny*               iReceiveBufPtr;
        // not owned
        TAny*               iDataReceiveBufPtr;
        // not owned
        TUint16*            iNeededBufLen;
        // not owned
        TUint16*            iNeededDataBufLen;
        // owned
        DQueue*             iRx;
        // owned
        DQueue*             iDataRx;
        // owned
        TDfc*               iEmptyRxQueueDfc;
        // owned
        TDfc*               iEmptyDataRxQueueDfc;
        // owned
        TDfc*               iConnectionStatusDfc;
        // owned
        TDfc*               iFlowCtrlStatusDfc;
        // owned
        TDfc*               iCompleteChannelRequestDfc;
        // Not owned.
        DThread*                iThread;
        MDataMessageStatus*     iPep;
        MChannel2IADApi*        iExtensionApi;
        NFastMutex*             iChFastMutex;

        // owned
        DReqQueue* iReqQueue;
        TMessageQue             iMsgQue;
        DQueue*                 iRxDeAllocate;
        TDfc*                   iCleanDfc;
        DQueue*                 iDataRxDeAllocate;
        TDfc*                   iCleanDataDfc;

    };

#endif      // DISAUSERCHANNEL_H
            
// End of File