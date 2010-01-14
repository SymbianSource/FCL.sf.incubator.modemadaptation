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



#ifndef DISAKERNELCHANNEL_H
#define DISAKERNELCHANNEL_H

#include <e32def.h>             // For TInt, TAny
#include <e32cmn.h>             // For TDesC8, 
#include <dfcs.h>               // For TDfc
#include "internalapi.h"        // For MIAD2ChannelApi, MChannel2IADApi

class DExtensionAPI;
class DQueue;

NONSHARABLE_CLASS( DISAKernelChannel ) : public MIAD2ChannelApi
    {
    
    public:
    DISAKernelChannel();
    ~DISAKernelChannel();
  
    // Handles request according to enumeration fromm DIsaKernelApi
    // TInt HandleRequest( const TUint16 aRequest, TAny* aParams );
    TInt HandleRequest( TThreadMessage& aMsg );

    // From MIAD2ChannelApi
    
    /*
    * @param aStatus, connection status
    * @return void
    */
    void NotifyConnectionStatus( TIADConnectionStatus aStatus );

    void ReceiveMsg( const TDesC8& aMessage );

    // Register pep associated to this channel to listen data message
    // deliveries.
    void RegisterPep( MDataMessageStatus& aPep );

    // When data message is written to user thread call
    // MDataMesssageStatus::DataMessageDelivered. It's responsibility is to
    // tell PEP that one data message e.g.a credit is used. PEP's should also
    // keep count of data messages added to channel to be abel to control the
    // legacy flowcontrol
    void ReceiveDataMsg( const TDesC8& aDataMessage );

    void NotifyFlowCtrl( TIADFlowControlStatus aStatus );

    void CompleteChannelRequest( TInt aRequest, TInt aStatusToComplete );
    
    inline TUint8 GetChannelNumber(){ return iChannelNumber; };
    // MIAD2ChannelApi end

    private:

        void Close( const TUint16 aChannelId );

        static void MsgQDfc( TAny* aPtr );

        void HandleThreadMsg( TThreadMessage& aMsg );

        void HandleDfcRequest( TThreadMessage& aMsg );

        TInt HandleSyncRequest( TThreadMessage& aMsg );

        void DoCancel( TInt aRequest, TInt aMask );

        // TODO: unique??
        void ResetQueues();

        static void EmptyRxDfc( TAny* aPtr );

        static void EmptyDataRxDfc( TAny* aPtr );

        // Checking context.
        void CheckDfc();
        
        //From objectapi
        void Receive( const TDesC8& aMessage );

  private:

    class TIADKernelAsyncRequest
        {
        public:
           TDfc* iDfcFunction;
           TInt* iRequestStatus;
        };

    TUint16                 iChannelNumber;
    // owned    
    DQueue*                 iRx;
    DQueue*                 iDataRx;
    TDes8**                 iPtrPtrToRxBuf;
    TDes8**                 iPtrPtrToDataRxBuf;
    TDfc*                   iEmptyRxDfc;
    TDfc*                   iEmptyDataRxDfc;
    // not owned, just using
    MDataMessageStatus*     iPep;
    MChannel2IADApi*        iExtensionApi;
    TIADKernelAsyncRequest  iRequests[ EIADAsyncLast ]; // TODO: the amount of kernel request.
    TMessageQue             iKernelChMsgQue;

    };



#endif /* DISAKERNELCHANNEL_H */
