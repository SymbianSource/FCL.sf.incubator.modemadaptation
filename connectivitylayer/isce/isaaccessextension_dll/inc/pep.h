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



#include <e32def.h>                     // For TUint16
#include <e32cmn.h>                     // For TDesC8
#include <klib.h>                       // For DBase
#include "iadinternaldefinitions.h"
#include "peptransceiver.h"
#include "iadtrace.h"                   // For ASSERT_RESET_ALWAYS

class DRouter;
class MIAD2ChannelApi;
class DPep;
class DPipeHandler;

enum TPipeState
        {
        ECreating,
        ECreatingEnabled,
        EDisabled,
        EDisabling,
        EEnabling,
        EEnabled,
//TBR        
        EEnableEnabled,
        EDisabledDisabled,
        EDisabledEnabled,
        EEnabledDisabled,
        EEnabledEnabled,
        EDisconnected,
        };


NONSHARABLE_CLASS(DPipeState) : public DBase
    {
    public:

        DPipeState(DPep& aPep);
        //virtual ~DPipeState();
        void NextState(TPipeState aNextState);
        virtual void EnterState();
        // Pipe events 
        virtual void PnsPipeCreatedInd(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
        virtual void PnsPipeEnabledInd(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
        virtual void PnsPipeRedirectedInd(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
        virtual void PnsPipeResetInd(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
        virtual void PnsPipeDisabledInd(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
        virtual TInt PnsPepConnectReq( const TDesC8& aData ){ aData;ASSERT_RESET_ALWAYS( 0, 0 ); return KErrNotFound; };
        virtual TInt PnsPepResetReq( const TDesC8& aData ){ aData;ASSERT_RESET_ALWAYS( 0, 0 ); return KErrNotFound; };
        virtual TInt PnsPepDisconnectReq(const TDesC8& aData);
        virtual TInt PnsPepEnableReq( const TDesC8& aData ){ aData;ASSERT_RESET_ALWAYS( 0, 0 ); return KErrNotFound; };
        virtual TInt PnsPepDisableReq( const TDesC8& aData ){ aData;ASSERT_RESET_ALWAYS( 0, 0 ); return KErrNotFound; };
        virtual TInt RxPnsPipeData( const TDesC8& aData ){ aData;ASSERT_RESET_ALWAYS( 0, 0 ); return KErrNotFound; };
        virtual TInt TxPnsPipeData( TDes8& aData );

        DPep& iPep;// TODO: public?

    };


NONSHARABLE_CLASS(DDisconnected) : public DPipeState
    {
    public:

        DDisconnected(DPep& aPep);
        // Override base class functions cause should not be allowed to do nothing at all, just to be removed.
        TInt PnsPepDisconnectReq( const TDesC8& aData ){ aData;ASSERT_RESET_ALWAYS( 0, 0 ); return KErrNotFound; };
        // TxPnsPipeData in base class already appropriate returns KErrOverFlow.

    };

// Creating state comes when pep is created, use cases pipe creation and pipe redirection (new pep).
// Default is disabled.
NONSHARABLE_CLASS( DCreating ) : public DPipeState
    {

    public:

        DCreating( DPep& aPep );
        TInt PnsPepConnectReq( const TDesC8& aReq );
        void PnsPipeCreatedInd();
        void PnsPipeRedirectedInd();

    };
// Creating / redirecting an enabled pipe.
NONSHARABLE_CLASS( DCreatingEnabled ) : public DPipeState
    {

    public:

        DCreatingEnabled( DPep& aPep );
        void PnsPipeCreatedInd();
        void PnsPipeRedirectedInd();

    };

NONSHARABLE_CLASS(DDisabled) : public DPipeState
    {
    public:

        DDisabled(DPep& aPep);
        void EnterState();
        TInt PnsPepEnableReq(const TDesC8& aData);
        TInt PnsPepResetReq(const TDesC8& aData);
        TInt PnsPepDisableReq(const TDesC8& aData);
        void PnsPipeRedirectedInd(); 
        void PnsPipeDisabledInd();

    };

NONSHARABLE_CLASS( DDisabling ) : public DPipeState
    {

    public:

        DDisabling( DPep& aPep );
        void EnterState();
        void PnsPipeDisabledInd();

    };

NONSHARABLE_CLASS( DEnabled ) : public DPipeState
    {
    public:

        DEnabled(DPep& aPep);
        void EnterState();
        TInt PnsPepEnableReq(const TDesC8& aData);
        TInt RxPnsPipeData( const TDesC8& aData );
        TInt TxPnsPipeData( TDes8& aData );
        TInt PnsPepResetReq(const TDesC8& aData);
        TInt PnsPepDisableReq(const TDesC8& aData);
        void PnsPipeEnabledInd();

    };

NONSHARABLE_CLASS( DEnabling ) : public DPipeState
    {

    public:

        DEnabling( DPep& aPep );
        void PnsPipeEnabledInd();
        void PnsPipeRedirectedInd();

    };

//TBR
NONSHARABLE_CLASS( DEnableEnabled ) : public DEnabled
    {

    public:

        DEnableEnabled( DPep& aPep );
        void EnterState();

    };
//TBR
NONSHARABLE_CLASS( DEnabledEnabled ) : public DPipeState
    {
    public:

        DEnabledEnabled( DPep& aPep);
        void PnsPipeResetInd();

    };

NONSHARABLE_CLASS( DEnabledDisabled ) : public DPipeState
    {
    public:

        DEnabledDisabled( DPep& aPep);
        void EnterState();
        void PnsPipeResetInd();

    };

NONSHARABLE_CLASS( DDisabledEnabled ) : public DPipeState
    {
    public:

        DDisabledEnabled( DPep& aPep);
        void PnsPipeResetInd();

    };

NONSHARABLE_CLASS( DDisabledDisabled ) : public DPipeState
    {
    public:

        DDisabledDisabled( DPep& aPep);
        void PnsPipeResetInd();

    };

NONSHARABLE_CLASS( DPep ) : public DBase
{

public:

    DPep( DRouter* aDl, MIAD2ChannelApi* aUl, TUint16 aChannelId, DPipeHandler& aHandler  );
    ~DPep();
    TUint8 GetPipeHandle()
        {
        return iPipeHandle;
        };
    
    TUint16 ChannelId()
        {
        return iChannelId;
        }
    
    void NextState(TPipeState aState);
    virtual TInt ReceiveMessage(const TDesC8& aMsg, MIAD2ChannelApi* aToCb);
    virtual TInt SendMessage(TDes8& aMsg, TUint16 aFromChannel);
    TInt SendPepConnectResp(const TDesC8& aData);
    TInt SendPepResetResp(const TDesC8& aData);
    TInt SendPepDisconnectResp(const TDesC8& aData);
    TInt SendPepEnableResp(const TDesC8& aData);
    TInt SendPepDisableResp(const TDesC8& aData);
    TInt RxPipeData(const TDesC8& aData);
    TInt TxPipeData(TDes8& aData);
    void PipeDisabling();
    void PipeDisabled();
    void PipeEnabled();
    void PipeDisableDisabled();
    void PipeEnableEnabled();

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
    void SetLoaned( MIAD2ChannelApi* aCh );
    void SetLoanReturned( MIAD2ChannelApi* aOldCh );
#endif

protected:

    void HandlePnPipeSbNegotiatedFC( const TDesC8& aMessage );

protected:

    DRouter*                iRouter;
    TUint8                  iPipeHandle;
    TUint8                  iChannelId;
    DPipeHandler&           iPipeHandler;

    MIAD2ChannelApi*        iChannel; // must be pointer due to it is not always available
    DPepTransceiver*        iDecoratedRx;
    DPepTransceiver*        iDecoratedTx;
    DPepTransceiver         iMainTxRx; // the one having pointers other classes

// StateMachine states:
    DCreating                   iCreating;
    DCreatingEnabled            iCreatingEnabled;
    DDisabled                   iDisabled;
    DDisabling                  iDisabling;
    DEnabling                   iEnabling;
    DEnabled                    iEnabled;
//TBR
    DEnableEnabled              iEnableEnabled;
    DDisabledDisabled           iDisabledDisabled;
    DDisabledEnabled            iDisabledEnabled;
    DEnabledDisabled            iEnabledDisabled;
    DEnabledEnabled             iEnabledEnabled;
    DDisconnected               iDisconnected;

    DPipeState*                 iCurrentState;
};

