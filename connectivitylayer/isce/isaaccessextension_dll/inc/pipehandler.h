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



#include <klib.h>       // For DBase


class DPep;
class MIAD2ChannelApi;
class DRouter;

NONSHARABLE_CLASS(DPipeHandler) : public DBase
{
    public:

        DPipeHandler(DRouter& aRouter);
        ~DPipeHandler();

        void ReceivePipeMessage( const TDesC8& aMsg, MIAD2ChannelApi* aToCb );
        TInt SendPipeMessage( TDes8& aMsg, TUint16 aFromChannel );
        void RemovePep(DPep* aPep);
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
        void PipeLoaned( const TUint16 aLoanerChannel, MIAD2ChannelApi* aCh );
        void PipeLoanReturned( const TUint16 aLoanerChannel, MIAD2ChannelApi* aOldCh );
#endif

    private:

        void HandlePepConnectReq( const TDesC8& aMsg, MIAD2ChannelApi* aReceiverCb);
        TInt HandlePipeCreateReq( TDes8& aMessage, TUint16 aFromChannel );
        TInt CreatePep( TUint16 aPepChannel, TBool aIsController );
        DPep* FindPep( const TUint16 aChannelId );//const TDesC8& aMsg );

    private:

        RPointerArray<DPep>     iPepContainer;
        MIAD2ChannelApi*        iChannelApi;
        DRouter&                iRouter;
};



