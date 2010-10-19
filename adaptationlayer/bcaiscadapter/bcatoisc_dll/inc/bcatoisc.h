/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef BCATOISC_H
#define BCATOISC_H

#include <networking/bca.h>
#include <iscapi.h>
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>

#include <tisi.h>
const TUint8 KMaxChannelNbr = 11;
const TUint8 KNameLen = 4;

/**
* BCATOISC Panic code.
* @internalComponent.
*/

enum TBcaToIscPanic
    {

    /** Monitor already existrs, we have a memory leak */
    EMonitorAlreadyExists,

    };
        


/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation
{
    
    class CBcaToIsc;
        
    /**
    * Monitors the ISCAPI flow control
    */        
    NONSHARABLE_CLASS(CNotifyFlowControlMonitor): public CActive
        {
        
        public:
        
        CNotifyFlowControlMonitor(RIscApi& aIscApi);
        void NotifyFlowControlStatus();
        void RequestFlowcontrolChange( TRequestStatus& aClientStatus );
        void CompleteFlowcontrolChangeWithCancel();
        void RunL();
        void DoCancel();
        TInt GetFlowControlState();
        ~CNotifyFlowControlMonitor();
        
        private:
        
        RIscApi& iIscApi;
        TInt iFlowControlValue;
        TRequestStatus* iWriteStatus;

        };
        
  
    NONSHARABLE_CLASS(CNotifyWriteStatusMonitor): public CActive
        {
        
        public:
        
        CNotifyWriteStatusMonitor(CBcaToIsc& aUser, RIscApi& aIscApi);
        void Write( TRequestStatus& aStatus, const TDesC8& aBuf );
        void SendAndComplete(TInt error);
        void RunL();
        void DoCancel();
        ~CNotifyWriteStatusMonitor();

        private:

        CBcaToIsc& iUser;
        RIscApi& iIscApi;
        TRequestStatus* iClientStatus;
        const TDesC8* iBuf;
     //   TPtr8* iBuf;

        };

    /**
    * The interface between NIF and Isc Bearer Transport.     
    */
    NONSHARABLE_CLASS(CBcaToIsc): 
                                  public CActive,
                                  public MBca // BCA is accessed via this interface by clients.
        {

        public:
            
        CBcaToIsc(); 
        virtual ~CBcaToIsc();
            virtual void Release();

        void ConstructL();

        virtual void Open(TRequestStatus& aStatus, const TDesC& aChannelId);
            
        virtual void Shutdown(TRequestStatus& aStatus);
        virtual void Close();

        virtual void Read(TRequestStatus& aStatus, TDes8& aBuf);
        virtual void Write(TRequestStatus& aStatus, const TDesC8& aBuf);

        virtual void CancelRead();
        virtual void CancelWrite();
        
        virtual void Ioctl(TRequestStatus& aStatus, TUint aOptLevel, TUint aOptName, TDes8& aOpt);
        virtual void CancelIoctl();

        TInt SendNameAddReq( TUint16& aObjId, TUint16& aClientObjId );
        TInt SendNameRemoveReq();
        TInt SendPepCtrlResp(
            TUint8 aTrId,
            TUint8 aReceiverDev,
            TUint8 aReceiverObj,
            TUint8 aPipeHandle,
            TUint8 aPepCtrlId,
            TUint8 aPepType,
            TUint8 aResult );
        void HandleReceivedMsg( const TIsiReceiveC& aIsiMsg );
        void HandleNameServiceMsg( const TIsiReceiveC& aIsiMsg );
        void HandlePipeMsg( const TIsiReceiveC& aIsiMsg );
        void HandlePepCtrlReq( const TIsiReceiveC& aIsiMsg );

        // Ownership
        CNotifyWriteStatusMonitor* iWriteStatusMonitor;
        CNotifyFlowControlMonitor* iFlowControlMonitor;

        protected:
        
        // from CActive
        void RunL();
        void DoCancel();

        private:

        RIscApi iIscApi;
        // no ownership
        TRequestStatus* iClientShutdownStatus;
        TRequestStatus* iClientOpenStatus;
        
        HBufC8* iReceivedMessageBuffer;
        TPtr8 iMessageBufferPtr;
        TBuf8<KNameLen> iName;
        TBuf16<KMaxChannelNbr> iChannelList;
        
        TUint16 iReadLength;
        TUint16 iChannelNumber;
        };

} // namespace BasebandChannelAdaptation

#endif //BCATOISC_H
