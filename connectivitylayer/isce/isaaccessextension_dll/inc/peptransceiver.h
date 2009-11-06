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



#ifndef _DPEP_TRANSCEIVER_H_
#define _DPEP_TRANSCEIVER_H_

#include <klib.h>           // For DBase
#include "internalapi.h"    // For MIAD2ChannelApi
#include "iadtrace.h"		// For ASSERT_RESET_ALWAYS

class MIAD2ChannelApi;
class DRouter;

// TODO: remove virtual from classes that are not ment to be derived.

NONSHARABLE_CLASS(DPepTransceiver) : public DBase
{
public:

    DPepTransceiver( DRouter* aRouter, MIAD2ChannelApi* aUl );
    DPepTransceiver( TUint8 aPipeHandle ); // for derived classes
    virtual ~DPepTransceiver();
    virtual void PepStatusInd( const TDesC8& aInd ){ aInd; ASSERT_RESET_ALWAYS( 0, 0 ); };
    virtual TInt Receive( const TDesC8& aData );
    virtual TInt Send( TDes8& aData );
    virtual void PipeEnabled(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
    virtual void PipeDisabled(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
    virtual void SetFlowCtrl( TIADFlowControlStatus aFlow );
    virtual TInt SendPnsPepConnectResp( const TDesC8& );
    virtual TInt SendResetResp( const TDesC8& );
    virtual TInt SendPnsPepDisconnectResp( const TDesC8& );
    virtual TInt SendEnableResp( const TDesC8& );
    virtual TInt SendDisableResp( const TDesC8& );
    void FillIsiHeader( TDes8& aData );
    virtual void RegisterDataMsgStatusCb( MDataMessageStatus* aCb );
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
    virtual void SetNewChannel( MIAD2ChannelApi* aCh ){ ASSERT_RESET_ALWAYS( 0, 0 ); };
    void SetMyChannel( MIAD2ChannelApi* aCh );
#endif
    TDes8& AllocateDataBlock( TUint16 aSize );
    TInt PepControlReq( const TDesC8& aReq );
    TUint16 ChannelId()
        {
        return iChannelId;
        }

    TUint16 PipeHandle()//NEW
        {
        return iPipeHandle;
        }

protected:

    //R&D helper to check that right thread is used to avoid sync problems
    void CheckDfc();

    TIADFlowControlStatus   iFlowCtrl;
    TUint8                  iPipeHandle;

private:
    TDes8& CreateCommonPepResp(const TDesC8& aReq, TUint16 aLenght, TUint8 aMsgId);

private:

    TUint16             iChannelId;
	DRouter*			iRouter;
    MIAD2ChannelApi*    iRcvChannel;

};

NONSHARABLE_CLASS(DPepTx) : public DPepTransceiver
{
public:

    DPepTx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DPepTx();
    virtual TInt Send(TDes8& aData) = 0;
    virtual void PepStatusInd(const TDesC8& aInd) = 0;
    virtual void PipeEnabled(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
    virtual void PipeDisabled();
    virtual void SetFlowCtrl( TIADFlowControlStatus aFlow );

protected:

    DPepTransceiver& iSuper;

};

NONSHARABLE_CLASS(DPepRx) : public DPepTransceiver, public MDataMessageStatus
{
public:

    DPepRx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DPepRx();
    virtual TInt Receive(const TDesC8& aData) = 0;
    virtual void PipeEnabled(){ ASSERT_RESET_ALWAYS( 0, 0 ); };
    virtual void PipeDisabled(){ ASSERT_RESET_ALWAYS( 0, 0 ); };

    // From MDataMessageStatus
    virtual void DataMessageDelivered() = 0;
    virtual void RegisterDataMsgStatusCb( MDataMessageStatus* aCb ){ aCb; ASSERT_RESET_ALWAYS( 0, 0 ); };

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
    void SetNewChannel( MIAD2ChannelApi* aCh );
#endif

protected:

    DPepTransceiver& iSuper;

private:
//TBR    TUint16 iReceiveQueueCount;
};

NONSHARABLE_CLASS(DMultiRx) : public DPepRx
{
public:
    DMultiRx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DMultiRx();

    virtual TInt Receive(const TDesC8& aData);
    virtual void PipeEnabled();
    virtual void PipeDisabled();
    // From MDataMessageStatus
    void DataMessageDelivered();

private:

    void SendCredits(TUint8 aNum);
    TUint16 iThresholdCount;
#ifdef _DEBUG // TODO: remove debug flag from actual code.
    TUint16 iGrantedCreditCount;
#endif

};

NONSHARABLE_CLASS(DLegacyRx) : public DPepRx
{
public:

    DLegacyRx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DLegacyRx();
    
    virtual TInt Receive(const TDesC8& aData);
    virtual void PipeEnabled();
    virtual void PipeDisabled();

    // From MDataMessageStatus
    void DataMessageDelivered();

private:

    void SendPnsPepStatusInd( TIADFlowControlStatus aFlowCtrlStatus );
    TUint16 iRxQueueCount;

};

NONSHARABLE_CLASS(DOneRx) : public DPepRx
{
public:
    DOneRx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DOneRx();
    
    virtual TInt Receive(const TDesC8& aData);
    virtual void PipeEnabled();
    virtual void PipeDisabled();

    // From MDataMessageStatus
    void DataMessageDelivered();

private:
    void SendOneCredit();
};

NONSHARABLE_CLASS( DMultiTx ) : public DPepTx
{
public:

    DMultiTx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DMultiTx();
    TInt Send(TDes8& aData);
    void PepStatusInd(const TDesC8& aInd);
    void PipeEnabled();
    void PipeDisabled();

private:
    TUint16 iGrantedCredits;
};

NONSHARABLE_CLASS( DLegacyTx ) : public DPepTx
{
public:

    DLegacyTx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DLegacyTx();
    virtual TInt Send(TDes8& aData);
    virtual void PepStatusInd(const TDesC8& aInd);
    virtual void PipeEnabled();

};

NONSHARABLE_CLASS( DOneTx ) : public DPepTx
{
public:

    DOneTx(DPepTransceiver& aSuper, TUint8 aPipeHandle);
    virtual ~DOneTx();
    virtual TInt Send(TDes8& aData);
    virtual void PepStatusInd(const TDesC8& aInd);
    virtual void PipeEnabled();

};

#endif //_DPEP_TRANSCEIVER_H_
