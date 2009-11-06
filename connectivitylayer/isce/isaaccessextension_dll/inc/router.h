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



#ifndef INC_DROUTER_H
#define INC_DROUTER_H

#include <e32def.h>                 // For TUint16
#include <e32cmn.h>                 // For TDesC8
#include "internalapi.h"            // For MIAD2ChannelApi
//ISCE #include "mist2iadapi.h"             // For TIADConnectionStatus, MIST2IADApi
#include "iadinternaldefinitions.h" // For TIADConnectionStatus
#include "iadnokiadefinitions.h"    // For EIADSizeOfChannels
//ISCE
#include "isirouterlinkifs.h"       // For MLinkRouterIf
//ISCE
#ifndef NCP_COMMON_BRIDGE_FAMILY
class DPipeHandler;
#endif
//ISCE class MIAD2ISTApi;
class DRouter;
class DIndicationHandler;
class MIAD2ChannelApi;
class DQueue;

static DObject* iThreadPtr = NULL;
const TUint8 KIADEventSubscriptionObjId( 0xfc );

// TODO: own API for indicationhandler (and pipehandler allocate, deallocate, sendmessage ?)

// - stores UL (APE destinated) message to receive queue
// 
// add DL (CMT destinated) message to IST's appropriate send queue IS THIS
// NEEDED OR FROM HANDLERS?
// 
// route UL message from receive queue to appropriate handler
// 
// receive DL message from LDD
// 
// route DL message to appropriate handler
NONSHARABLE_CLASS( DRouter ) : public MChannel2IADApi,
                            // ISCE
                            public MISILinkRouterIf
                            // ISCE

    {

    public:

        DRouter();

        ~DRouter();

        // From MChannel2IADApi start
        IMPORT_C TDes8& AllocateBlock( const TUint16 aSize );

        // Increases length of the returned descriptor to aSize + pipe header (11).
        IMPORT_C TDes8& AllocateDataBlock( const TUint16 aSize );

        IMPORT_C void Close( const TUint16 aChannel );

        IMPORT_C void DeAllocateBlock( TDes8& aBlock );

        IMPORT_C TInt GetConnectionStatus();
  
        IMPORT_C TInt GetFlowControlStatus();

        IMPORT_C TInt GetMaxDataSize();

        IMPORT_C void Open( const TUint16 aChannel, const TUint16 aRequest, MIAD2ChannelApi* aCallback );

        IMPORT_C void Open( const TUint16 aChannel, const TUint16 aRequest, const TDesC8& aOpenInfo, MIAD2ChannelApi* aCallback );

        IMPORT_C TInt OrderIndication( TDes8& anOrder, const TUint16 aCh, const TBool a32Bit = EFalse );

        IMPORT_C TInt SendMessage( TDes8& aMessage, const TUint16 aCh );

        IMPORT_C TInt SendIndication( TDes8& aMessage, const TUint16 aCh );

        // From MChannel2IADApi end

        // For Router and it's handler DPipeHandler and DIndicationHandler
        TInt SendMsg( TDes8& aMsg );

        // For PipeHandler
        MIAD2ChannelApi* GetChannel( const TUint16 aChannel );
        // For PipeHandler
// ISCE
// TODO: DEF files!!!
        // From MISILinkRouterIf start
        /*
        * See comments from MISILinkRouterIf.
        */
        void NotifyTrxPresenceEnqueDfc( TBool aPresent );

        /*
        * See comments from MISILinkRouterIf.
        */
        void Receive( TDes8& aMsg );
        // From MISILinkRouterIf end
        IMPORT_C void DummyDoNothing();

        IMPORT_C void DummyDoNothing2();

// ISCE

        // For internal receiving.
        void DRouter::MessageReceived( TDes8& aMsg );

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
        EXPORT_C TInt Loan( const TUint16 aChannel, const TUint16 aRequest,
                            MIAD2ChannelApi* aCallback );
        EXPORT_C TInt ReturnLoan( const TUint16 aChannel, const TUint16 aRequest,
                            MIAD2ChannelApi* aCallback );
#endif

    private:

        enum TRoutingRule
            {
            EIsiMsg = 1,
            EPipeMsg,
            EMediaMsg,
            EIndicationMsg,
            EControlMsg,
            EPnNameAddRespMsg,
            ENotKnownMsg,
            };

        enum TWaitingType
            {
            ENormalOpen = 1,
            EDrmOpen,
            ENameAddOpen
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
            ,ELoan
#endif
            };

        void HandleIsiMessage( TDes8& aMsg );

        void HandlePipeMessage( TDes8& aMsg );

        void HandleMediaMessage( TDes8& aMsg );

        void HandleControlMessage( TDes8& aMsg );

        void HandlePnsNameAddResp( TDes8& aMsg );

        void SendCommIsaEntityNotReachableResp( const TDesC8& aMsg );

        static void CheckDfc();

        TInt ValiDateIsiMessage( TDes8& aMsg );

    public:
        static void CheckRouting( DRouter& aTmp, TDes8& aMsg );

    private:
        static void CommonRxDfc( TAny* aPtr );

        static void InitCmtDfc( TAny* aPtr );
#ifdef NCP_COMMON_BRIDGE_FAMILY
        void InitConnectionOk();
#endif
        void InitCmtConnection();

        void SendDrmReq( const TUint16 aChannelId );

        void SendPnsNameAddReq( const TUint16 aChannel, const TDesC8& aOpenInfo );

        static void NotifyObjLayerConnStatDfc( TAny* aPtr );

        void NotifyObjLayerConnStat( const TIADConnectionStatus aStatus );

        void SetSenderInfo( TDes8& aMessage, const TUint16 aCh );

      // Member data
    private:
        
        void CheckSameThreadContext();
        
        // TODO
        // Needed due to opening that are waiting a response from cmt side.
        // Use iWaitingChannel to store a channel pointer then and when resp
        // received from cmt set iChannel = iWaitingChannel and complete request
        // Used also when initializing physical and datalink layers.
        class TIADChannel
            {
            public:
                
                MIAD2ChannelApi*   iChannel;
                MIAD2ChannelApi*   iWaitingChannel;
                TWaitingType       iType;
            };

// ISCE
        TUint8 MapMediaToLinkId( const TUint8 aMedia );

        enum TISIMedias
            {
#ifndef NCP_COMMON_BRIDGE_FAMILY
            EISIMediaSOS            = 0x00,
#else
            EISIMediaHostSSI        = 0x00,
#endif
            EISIAmountOfMedias
            };
// ISCE

        // owned
        // APE <-> CMT connection status ok/nok.
        TIADConnectionStatus                            iConnectionStatus;
#ifndef NCP_COMMON_BRIDGE_FAMILY
        DPipeHandler*                                   iPipeHandler;
#endif
        DIndicationHandler*                             iIndicationHandler;
        DQueue*                                         iCommonRxQueue;
        TIADChannel                                     iChannelTable[ EIADSizeOfChannels ];
        //static TDfc*                                    iConnStatusBcDfc;
        TDfc*                                           iCommonRxDfc;
        TDfc*                                           iInitCmtDfc;
        TDfc*                                           iConnStatDfc;
        // Maximum size of isi/data messages to be send.
        TUint16                                         iMaxFrameSize;
        // not owned, just using     
        // When registered !NULL when unregistered NULL. API towards IST.
        // ISCE        RArray<TUint>                                   iArray;
        TBool                                           iBootDone;

        // ISCE
        MISIRouterLinkIf**                              iLinksArray;
        // ISCE


    };



#endif /* INC_DROUTER_H */
