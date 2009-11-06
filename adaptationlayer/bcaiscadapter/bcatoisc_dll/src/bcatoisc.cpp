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



#include "bcatoisc.h"
#include "bcatoisctrace.h"
#include "iscdefinitions.h"
#include <cdbcols.h>

using namespace BasebandChannelAdaptation;

    
/**
* Default constructor to create a CNotifyFlowControlMonitor instance.
*/    
CNotifyFlowControlMonitor::CNotifyFlowControlMonitor(RIscApi& aIscApi)
    :CActive(EPriorityStandard),
    iIscApi(aIscApi)
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::CNotifyFlowControlMonitor RIscAapi 0x%x ->"), &aIscApi ) );
    
    CActiveScheduler::Add(this);

    C_TRACE( (_T("CNotifyFlowControlMonitor::CNotifyFlowControlMonitor <-") ) );
    }

void CNotifyFlowControlMonitor::NotifyFlowControlStatus()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::NotifyFlowControlStatus() ->") ) );

    ASSERT(!IsActive());
    
    iFlowControlValue = iIscApi.FlowControlStatus();
      
    C_TRACE( (_T("CNotifyFlowControlMonitor::NotifyFlowControlStatus initial iFlowControlValue %d ->"), iFlowControlValue ) );

    iIscApi.NotifyFlowControl(iStatus, iFlowControlValue);
    SetActive();

    C_TRACE( (_T("CNotifyFlowControlMonitor::NotifyFlowControlStatus() <-") ) );
    }

void CNotifyFlowControlMonitor::RequestFlowcontrolChange( TRequestStatus& aStatus )
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::RequestFlowcontrolChange ->") ) );

    iWriteStatus = &aStatus;
    *iWriteStatus = KRequestPending;


    C_TRACE( (_T("CNotifyFlowControlMonitor::RequestFlowcontrolChange <-") ) );
    }
    
void CNotifyFlowControlMonitor::RunL()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::RunL() iStatus %d ->"), iStatus.Int() ) );
    
    iFlowControlValue = iIscApi.FlowControlStatus();
    
  
    C_TRACE( (_T("CNotifyFlowControlMonitor::RunL flow control Value has changed and is now: (%d)"), iFlowControlValue ) );

        if( *iWriteStatus == KRequestPending )
            {
              User::RequestComplete(iWriteStatus, KErrNone);
            }

    if( iStatus == KErrNone )
        {
        //Check for further flow control status changes
        iIscApi.NotifyFlowControl(iStatus, iFlowControlValue);
        SetActive();
        }
                
    C_TRACE( (_T("CNotifyFlowControlMonitor::RunL() <-") ) );
    }
    
TInt CNotifyFlowControlMonitor::GetFlowControlState()
    {
      return iFlowControlValue;
    }
    
void CNotifyFlowControlMonitor::DoCancel()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::DoCancel() ->") ) );
    
    iIscApi.NotifyFlowControlCancel();

    C_TRACE( (_T("CNotifyFlowControlMonitor::DoCancel() <-") ) );
    }
    
CNotifyFlowControlMonitor::~CNotifyFlowControlMonitor()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::~CNotifyFlowControlMonitor() ->") ) );
    
    Cancel();
        
    C_TRACE( (_T("CNotifyFlowControlMonitor::~CNotifyFlowControlMonitor() <-") ) );
    }
    

CNotifyWriteStatusMonitor::CNotifyWriteStatusMonitor( CBcaToIsc& aUser, RIscApi& aIscApi )
    :CActive(EPriorityHigh),
    iUser(aUser),
    iIscApi(aIscApi)
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::CNotifyWriteStatusMonitor ->") ) );
 //   iBuf = new TPtr8( NULL, 0, 0 );
    CActiveScheduler::Add(this);
    C_TRACE( (_T("CNotifyWriteStatusMonitor::CNotifyWriteStatusMonitor <-") ) );
    }


void CNotifyWriteStatusMonitor::Write( TRequestStatus& aStatus, const TDesC8& aBuf )
    {

    iBuf = &aBuf;
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;

    if( iUser.iFlowControlMonitor->GetFlowControlState() == EIscFlowControlOff )
        {
        //iIscApi.DataSend( aStatus, aBuf );
        SendAndComplete();             
        }
    else  //flow control on
        {

        //iBuf = &aBuf;
        //  iBuf->Set( const_cast<TUint8*>(aBuf.Ptr()), aBuf.Length(), aBuf.Length() );
        // iClientStatus = &aStatus;
        // *iClientStatus = KRequestPending;
        iUser.iFlowControlMonitor->RequestFlowcontrolChange( iStatus );
        SetActive();
        }

    }
void CNotifyWriteStatusMonitor::RunL()
        {
        C_TRACE( (_T("CNotifyWriteStatusMonitor::RunL() iStatus %d ->"), iStatus.Int() ) );
    
    SendAndComplete();
    //iIscApi.DataSend( *iClientStatus, *iBuf );
    
    C_TRACE( (_T("CNotifyWriteStatusMonitor::RunL() <-") ) );
    }

void CNotifyWriteStatusMonitor::SendAndComplete()
    {
      TInt error = KErrGeneral;
    
    error = iIscApi.DataSend( *iBuf );
    User::RequestComplete(iClientStatus, error);
    }
    

void CNotifyWriteStatusMonitor::DoCancel()
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::DoCancel() ->") ) );
    
    if( *iClientStatus == KRequestPending )
        {
          C_TRACE( (_T("CNotifyWriteStatusMonitor::DoCancel completing client request with KErrCancel ->") ) );
        //Complete clients request
        User::RequestComplete( iClientStatus, KErrCancel );
        }
    
    C_TRACE( (_T("CNotifyWriteStatusMonitor::DoCancel() <-") ) );
    }


CNotifyWriteStatusMonitor::~CNotifyWriteStatusMonitor()
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::~CNotifyWriteStatusMonitor() ->") ) );
    
    Cancel();

    C_TRACE( (_T("CNotifyWriteStatusMonitor::~CNotifyWriteStatusMonitor() <-") ) );
    }



/**
* Default constructor to create a CBcaToIsc instance.
*/
CBcaToIsc::CBcaToIsc() : iReadLength(0), iChannelNumber( 0xffff )
    {
    C_TRACE( (_T("CBcaToIsc::CBcaToIsc() ->") ) );
    
    C_TRACE( (_T("CBcaToIsc::CBcaToIsc() <-") ) );
    }

/**
2nd phase of construction: allocates member objects 

@leave when members cannot be constructed. */
void CBcaToIsc::ConstructL()
    {
    C_TRACE( (_T("CBcaToIsc::ConstructL() ->") ) );
    __ASSERT_DEBUG(!iFlowControlMonitor, User::Panic(_L("BcaToIsc.dll"), EMonitorAlreadyExists));
    __ASSERT_DEBUG(!iWriteStatusMonitor, User::Panic(_L("BcaToIsc.dll"), EMonitorAlreadyExists));
    iFlowControlMonitor = new (ELeave)CNotifyFlowControlMonitor(iIscApi);
    iWriteStatusMonitor = new (ELeave)CNotifyWriteStatusMonitor(*this, iIscApi);
    C_TRACE( (_T("CBcaToIsc::ConstructL() this 0x%x <-"), this ) );

    }

/**
* Destructor 
*/    
CBcaToIsc::~CBcaToIsc()
    {

    C_TRACE( (_T("CBcaToIsc::~CBcaToIsc() ->") ) );
    if( iFlowControlMonitor )
        {
        delete iFlowControlMonitor;
        }

    if( iWriteStatusMonitor )
        {
        delete iWriteStatusMonitor;
        }
    iClientShutdownStatus = NULL;
    C_TRACE( (_T("CBcaToIsc::~CBcaToIsc() <-") ) );

    }

/** This method deletes the BCA itself.*/
void CBcaToIsc::Release()
    {

    C_TRACE( (_T("CBcaToIsc::Release() this 0x%x ->"), this ) );
    delete this;
    C_TRACE( (_T("CBcaToIsc::Release() this 0x%x <-"), this ) );

    }


/**
* Informs that the BCA is required by the client(for instance, Raw IP NIF). 
    
* @param aStatus complete status, KErrNone if successful, error code otherwise.
* @param aChannelId comm port name.
*/    
void CBcaToIsc::Open(TRequestStatus& aStatus, const TDesC& aChannelId)
    {

    C_TRACE( (_T("CBcaToIsc::Open aStatus %d aChannelId descriptor %S ->"), aStatus.Int(), &aChannelId ) );
    // Expected form either <XXX>Int_<XX> (i.e. 100Int_3) or Int!!!
    TUint16 channelid( 0xffff );
    TInt error( KErrGeneral );
    _LIT(KIntUnderScore, "Int_");
    TInt len1 = aChannelId.Find(KIntUnderScore);
    if( len1 < KErrNone )
        {
        // Int format
        TLex16 channelParser(aChannelId);
        error = channelParser.Val(channelid, EDecimal);
        }
    else
        {
        // <XXX>Int_<XX> format
        TUint numPos = len1 + KIntUnderScore.iTypeLength;
        TPtrC digit = aChannelId.Mid(numPos);
        TLex16 channelParser(digit);
        error = channelParser.Val(channelid, EDecimal);
        }
    ASSERT( KErrNone == error );
    if( error == KErrNone )
        {
        error = iIscApi.Loan( channelid );
        iChannelNumber = error == KErrNone ? channelid : iChannelNumber; 
        }
    ASSERT( KErrNone == error );
    TRequestStatus* request = &aStatus;
    User::RequestComplete( request, error );
    C_TRACE( (_T("CBcaToIsc::Open <-") ) );

    }

/**
 Shuts the BCA channel down in a graceful manner. BCA releases its resources.
 Cancels all outstanding operations: Writes, Reads and Ioctls.
 
 @param aStatus completion status: Always KErrNone.
*/
void CBcaToIsc::Shutdown(TRequestStatus& aStatus)
    {

    C_TRACE( (_T("CBcaToIsc::Shutdown aStatus %d ->"), aStatus.Int() ) );
    iClientShutdownStatus = &aStatus;
    *iClientShutdownStatus = KRequestPending;
    iWriteStatusMonitor->Cancel();
    iFlowControlMonitor->Cancel();
    iIscApi.DataReceiveCancel();
    iIscApi.ReturnLoan( iChannelNumber );
    User::RequestComplete(iClientShutdownStatus, KErrNone);
    C_TRACE( (_T("CBcaToIsc::Shutdown <-") ) );

    }

/**
* Closes the BCA immediately. BCA releases its resources.
* cancels all Writes, Reads and Controls.
*/    
void CBcaToIsc::Close()
    {

    C_TRACE( (_T("CBcaToIsc::Close() iChannelNumber=0x%x->"), iChannelNumber ) );
    if( iClientShutdownStatus && ( *iClientShutdownStatus == KRequestPending ) )
        {
        C_TRACE( (_T("CBcaToIsc::Close completing client Shutdown request with KErrCancel ->") ) );
        User::RequestComplete(iClientShutdownStatus, KErrCancel);
        }
    C_TRACE( (_T("CBcaToIsc::Close() iChannelNumber=0x%x 1"), iChannelNumber ) );
    iWriteStatusMonitor->Cancel();
    C_TRACE( (_T("CBcaToIsc::Close() iChannelNumber=0x%x 2"), iChannelNumber ) );
    iFlowControlMonitor->Cancel();
    C_TRACE( (_T("CBcaToIsc::Close() iChannelNumber=0x%x 3"), iChannelNumber ) );
    iIscApi.DataReceiveCancel();
    C_TRACE( (_T("CBcaToIsc::Close() iChannelNumber=0x%x 4"), iChannelNumber ) );
    iIscApi.DataSendCancel();
    C_TRACE( (_T("CBcaToIsc::Close() iChannelNumber=0x%x 5"), iChannelNumber ) );
    iIscApi.ReturnLoan( iChannelNumber );

    C_TRACE( (_T("CBcaToIsc::Close() <-") ) );

    }

/**
* Queues a Read.

* @param aStatus complete status, KErrNone if successful, error code otherwise.
* @param aBuf buffer for data to be read.
* @note The buffer is owned by the client. Client must not access / modify the buffer until the Read completes.
*/    
void CBcaToIsc::Read(TRequestStatus& aStatus,TDes8& aBuf)
    {

    C_TRACE( (_T("CBcaToIsc::Read aStatus=%d aBuf=0x%x ->"), aStatus.Int(), &aBuf ) );
    iIscApi.DataReceive( aStatus, aBuf, iReadLength );
    C_TRACE( (_T("CBcaToIsc::Read aStatus=%d aBuf=0x%x <-"), aStatus.Int(), &aBuf ) );

    }

/**
* Queues a Write

* @param aStatus the complete status, KErrNone if successful, error code otherwise.
* @param aBuf the buffer to sent.
* @note The buffer is owned by the client. Client must not access / modify the buffer until the Write completes.
*/    
void CBcaToIsc::Write( TRequestStatus& aStatus, const TDesC8& aBuf )
    {
    C_TRACE( (_T("CBcaToIsc::Write aStatus %d aBuf %S ->"), aStatus.Int(), &aBuf ) );
        //write requesti pitää jäädä odottamaan jos flow controlli päälä.
                 
        iWriteStatusMonitor->Write(aStatus, aBuf);
            
    C_TRACE( (_T("CBcaToIsc::Write <-") ) );
    }

/**
 Cancels the outstanding Read operation.(best effort operation: the read may have been completed already.)
*/    
void CBcaToIsc::CancelRead()
    {
    C_TRACE( (_T("CBcaToIsc::CancelRead() ->") ) );
        
    iIscApi.DataReceiveCancel();

    C_TRACE( (_T("CBcaToIsc::CancelRead() <-") ) );
    }

/**
 Cancels all Write operations.(best effort attempt: the write may have been completed already.)
*/    
void CBcaToIsc::CancelWrite()
    {
    C_TRACE( (_T("CBcaToIsc::CancelWrite() ->") ) );
    
    iWriteStatusMonitor->Cancel();

    C_TRACE( (_T("CBcaToIsc::CancelWrite() <-") ) );
    }
    
    
/**
* Ioctl: asynchronously controls the BcaToIsc.

* @param aStatus complete status, KErrNone if successful, system-wide error code otherwise.
* @param aOptLevel option level to be used.
* @param aOptName option name to be used.
* @param aOpt an optional parameter,holds the option value on return or the option value to be set.
*/  
void CBcaToIsc::Ioctl( 
        TRequestStatus& aStatus,
        TUint, // aOptLevel,
        TUint, // aOptName,
        TDes8& // aOpt
        )
    {

    C_TRACE( (_T("CBcaToIsc::Ioctl ->") ) );
    TRequestStatus* ptrStatus = &aStatus;
    User::RequestComplete( ptrStatus, KErrNotSupported);
    C_TRACE( (_T("CBcaToIsc::Ioctl <-") ) );

    }

/** Cancels an outstanding Ioctl, if any. */
void CBcaToIsc::CancelIoctl()
    {

    C_TRACE( (_T("CBcaToIsc::CancelIoctl() ->") ) );
    C_TRACE( (_T("CBcaToIsc::CancelIoctl() <-") ) );

    }

