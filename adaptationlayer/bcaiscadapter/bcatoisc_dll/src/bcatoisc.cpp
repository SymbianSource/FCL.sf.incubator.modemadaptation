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



#include "bcatoisc.h"
#include "bcatoisctrace.h"
#include "iscdefinitions.h"
#include <cdbcols.h>

#include <nsisi.h>
#include <pipeisi.h>
#include <pn_const.h>
#include <iscnokiadefinitions.h>
#include "pep_bca_types.h"


const TUint8 KDataLength = 7;
const TUint8 KStatusIndex = 0;
const TUint8 KDevIdIndex = 5;
const TUint8 KObjIdIndex = 6;
const TUint8 KPadding = 0;
const TUint KDefaultReceiveBufferSize = 2048;

using namespace BasebandChannelAdaptation;

/**
* Utility function to complete a pending request and NULL request pointer.
*/   
 
static void CompleteRequest(TRequestStatus* &aReqStatus, const TInt aError)
    {
    if(( aReqStatus ) &&
       ( *aReqStatus == KRequestPending ))
        {
        // Only realy need to trace if we actually complete the request.
        C_TRACE( (_T("CompleteRequest completing client request with aError %d ->"), aError ) );

        //Complete clients request
        User::RequestComplete( aReqStatus, aError );

        aReqStatus = NULL;
        }
    }

    
/**
* Default constructor to create a CNotifyFlowControlMonitor instance.
*/    
CNotifyFlowControlMonitor::CNotifyFlowControlMonitor(RIscApi& aIscApi)
    :CActive(EPriorityHigh),
    iIscApi(aIscApi),
    iWriteStatus(NULL)
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::CNotifyFlowControlMonitor RIscApi 0x%x ->"), &aIscApi ) );
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
    
void CNotifyFlowControlMonitor::CompleteFlowcontrolChangeWithCancel()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::CompleteFlowcontrolChangeWithCancel() ->") ) );
    // If pending request, complete as cancelled
    CompleteRequest(iWriteStatus, KErrCancel);
    C_TRACE( (_T("CNotifyFlowControlMonitor::CompleteFlowcontrolChangeWithCancel() <-") ) );
    }
    
void CNotifyFlowControlMonitor::RunL()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::RunL() iStatus %d ->"), iStatus.Int() ) );
    
    ASSERT( iStatus.Int() == KErrNone);
    
    iFlowControlValue = iIscApi.FlowControlStatus();
      
    C_TRACE( (_T("CNotifyFlowControlMonitor::RunL flow control Value has changed and is now: (%d)"), iFlowControlValue ) );

    if( iFlowControlValue == EIscFlowControlOff)
        {
        // If pending request, complete it as flow control is now OFF
        CompleteRequest(iWriteStatus,KErrNone);
        }
    
    //Check for further flow control status changes
    iIscApi.NotifyFlowControl(iStatus, iFlowControlValue);
    SetActive();
    C_TRACE( (_T("CNotifyFlowControlMonitor::RunL() <-") ) );
    }
    
TInt CNotifyFlowControlMonitor::GetFlowControlState()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::GetFlowControlState() flowcontrol value %d <->"), iFlowControlValue ) );
      return iFlowControlValue;
    }
    
void CNotifyFlowControlMonitor::DoCancel()
    {
    C_TRACE( (_T("CNotifyFlowControlMonitor::DoCancel() ->") ) );
    // If pending request, complete it as cancelled
    CompleteRequest(iWriteStatus, KErrCancel);
    C_TRACE( (_T("CNotifyFlowControlMonitor::DoCancel() req is completed") ) );

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
    :CActive(EPriorityStandard),
    iUser(aUser),
    iIscApi(aIscApi)
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::CNotifyWriteStatusMonitor ->") ) );
    CActiveScheduler::Add(this);
    C_TRACE( (_T("CNotifyWriteStatusMonitor::CNotifyWriteStatusMonitor <-") ) );
    }


void CNotifyWriteStatusMonitor::Write( TRequestStatus& aStatus, const TDesC8& aBuf )
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::Write ->") ) );
    iBuf = &aBuf;
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;

    if( iUser.iFlowControlMonitor->GetFlowControlState() == EIscFlowControlOff )
        {
        SendAndComplete(KErrNone);             
        }
    else  //flow control on
        {
        iUser.iFlowControlMonitor->RequestFlowcontrolChange( iStatus );
        SetActive();
        }
    C_TRACE( (_T("CNotifyWriteStatusMonitor::Write <-") ) );
    }
    
void CNotifyWriteStatusMonitor::RunL()
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::RunL() iStatus %d ->"), iStatus.Int() ) );
// KARI ei kutsu IASC apin data sendiä jos erksa OK?    
    SendAndComplete(iStatus.Int());
    
    C_TRACE( (_T("CNotifyWriteStatusMonitor::RunL() <-") ) );
    }

void CNotifyWriteStatusMonitor::SendAndComplete(TInt aError)
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::SendAndComplete() ->")));
    
    // We may have had the request cancelled, in which case do not send the write data
    if (aError == KErrNone)
        {    
        C_TRACE( (_T("CNotifyWriteStatusMonitor::SendAndComplete() iIscApi.DataSend ")));
        aError = iIscApi.DataSend( *iBuf );
        }

    // If pending request, complete it as specified by the error status
    // This could be OK or canncelled
    CompleteRequest(iClientStatus, aError);
    
    C_TRACE( (_T("CNotifyWriteStatusMonitor::SendAndComplete() <-") ) );
    }
    

void CNotifyWriteStatusMonitor::DoCancel()
    {
    C_TRACE( (_T("CNotifyWriteStatusMonitor::DoCancel() ->") ) );
    
    iUser.iFlowControlMonitor->CompleteFlowcontrolChangeWithCancel();

    // If pending request, complete it as cancelled
    CompleteRequest(iClientStatus, KErrCancel);
    
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
CBcaToIsc::CBcaToIsc() : 
    CActive( EPriorityStandard ),
    iMessageBufferPtr( 0,0 ),
    iReadLength(0), 
    iChannelNumber( 0xffff )
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
    iReceivedMessageBuffer = HBufC8::NewL(KDefaultReceiveBufferSize);
    iMessageBufferPtr.Set( iReceivedMessageBuffer->Des() );
    iMessageBufferPtr.FillZ();
    iChannelList.Append( EIscNokiaNifPep5 );
    iChannelList.Append( EIscNokiaNifPep6 );
    iChannelList.Append( EIscNokiaNifPep7 );
    iChannelList.Append( EIscNokiaNifPep8 );
    CActiveScheduler::Add( this );
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

    if( iReceivedMessageBuffer )
        {
        delete iReceivedMessageBuffer;
        }

    iClientShutdownStatus = NULL;
    iClientOpenStatus = NULL;
    C_TRACE( (_T("CBcaToIsc::~CBcaToIsc() <-") ) );

    }
// remove loan / returnloan deadcode from lower layers
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
    iClientOpenStatus = &aStatus;
    *iClientOpenStatus = KRequestPending;

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
    C_TRACE( (_T("CBcaToIsc::Open parsed channel id: 0x%x"), channelid ) );
    if( error == KErrNone )
        {
        TInt index( 0 );
        TBool channelOpened( EFalse );
        TRequestStatus status( KRequestPending );
        for( index = 0; index < iChannelList.Length(); index++ )
            {
            status = KRequestPending;
            iIscApi.Open( iChannelList[index], status, EOwnerThread );
            User::WaitForRequest( status );
            if( KErrNone == status.Int() )
                {
                C_TRACE( (_T("CBcaToIsc::Open channel 0x%x opened"), iChannelList[index] ) );
                channelOpened = ETrue;
                break;
                }
            }

        if( !channelOpened )
            {
            C_TRACE( (_T("CBcaToIsc::Open channel opening failed") ) );
            User::RequestComplete( iClientOpenStatus, KErrGeneral );
            iClientOpenStatus = NULL;
            }
        else if( KErrNone == SendNameAddReq( iChannelList[index], channelid ) )
            {
            iFlowControlMonitor->NotifyFlowControlStatus();
            iChannelNumber = status.Int() == KErrNone ? iChannelList[index] : iChannelNumber; 
            
            iMessageBufferPtr.SetLength( 0 );
            iStatus = KRequestPending;
            iIscApi.Receive( iStatus, iMessageBufferPtr, iReadLength );
            SetActive();
            }
        else
            {
            C_TRACE( (_T("CBcaToIsc::Open sending of PNS_NAME_ADD_REQ failed") ) );
            iIscApi.Close();
            User::RequestComplete( iClientOpenStatus, KErrGeneral );
            iClientOpenStatus = NULL;
            }
        }
    else
        {    
        User::RequestComplete( iClientOpenStatus, error );
        iClientOpenStatus = NULL;
        }

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

    if( KErrNone == SendNameRemoveReq() )
        {
        iMessageBufferPtr.SetLength( 0 );
        iStatus = KRequestPending;
        iIscApi.Receive( iStatus, iMessageBufferPtr, iReadLength );
        SetActive();
        }
    else
        {
        C_TRACE( (_T("CBcaToIsc::Shutdown sending of PNS_NAME_REMOVE_REQ failed") ) );
        iIscApi.Close();
        User::RequestComplete(iClientShutdownStatus, KErrGeneral);
        iClientShutdownStatus = NULL;
        }

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
    
    iIscApi.Close();

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

/**
* Sends PNS_NAME_ADD_REQ

* @param aObjId object id for the name to be added
* @return KErrNone if message sent succesfully, otherwise error code
*/    
TInt CBcaToIsc::SendNameAddReq( TUint16& aObjId, TUint16& aClientObjId )
    {
    C_TRACE( (_T("CBcaToIsc::SendNameAddReq() ->") ) );

    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_REQ );
    TInt err( KErrGeneral );

    if( message )
        {
        TPtr8 messageptr = message->Des();
        messageptr.FillZ();

        // Add 32bit name (Big-endian)
        iName.Append( aClientObjId );
        iName.Append( PN_PEP_SUB_TYPE_SYMBIAN_INTERNAL );
        iName.Append( PN_PEP_TYPE_COMMON );
        iName.Append( PN_PIPE );
        TUint8 nameOffset( ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY );

        TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_REQ );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_NAMESERVICE );
        isimessage.Set8bit( ISI_HEADER_OFFSET_TRANSID, 0 );
        isimessage.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, PNS_NAME_ADD_REQ );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED1, KPadding );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED2, KPadding );
        isimessage.Set8bit( nameOffset, iName[0] );
        isimessage.Set8bit( nameOffset + 1, iName[1] );
        isimessage.Set8bit( nameOffset + 2, iName[2] );
        isimessage.Set8bit( nameOffset + 3, iName[3] );
        isimessage.Set8bit( nameOffset + PN_NAME_SRV_ITEM_STR_OFFSET_DEV, THIS_DEVICE );
        isimessage.Set8bit( nameOffset + PN_NAME_SRV_ITEM_STR_OFFSET_OBJ, aObjId );
        isimessage.Set8bit( nameOffset + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS, PN_NAME_UNDEF );
        isimessage.Set8bit( nameOffset + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED, KPadding );
        isimessage.Complete();

        err = iIscApi.Send( messageptr );
        C_TRACE( (_T("CBcaToIsc::SendNameAddReq() success: 0x%x"), err ) );
        delete message;
        }
    else
        {
        C_TRACE( (_T("CBcaToIsc::SendNameAddReq() memory allocation failed") ) );
        ASSERT(0);
        }

    C_TRACE( (_T("CBcaToIsc::SendNameAddReq() <-") ) );
    return err;
    }

/**
* Sends PNS_NAME_REMOVE_REQ

* @return KErrNone if message sent succesfully, otherwise error code
*/
TInt CBcaToIsc::SendNameRemoveReq()
    {
    C_TRACE( (_T("CBcaToIsc::SendNameRemoveReq() ->") ) );

    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_NAME_REMOVE_REQ );
    TInt err( KErrGeneral );
    if( message )
        {
        TPtr8 messageptr = message->Des();
        messageptr.FillZ();
        TUint8 nameOffset( ISI_HEADER_SIZE + PNS_NAME_REMOVE_REQ_OFFSET_NAME );

        TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_NAME_REMOVE_REQ );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_NAMESERVICE );
        isimessage.Set8bit( ISI_HEADER_OFFSET_TRANSID , 0 );
        isimessage.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, PNS_NAME_REMOVE_REQ );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_REMOVE_REQ_OFFSET_RESERVED1, KPadding );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_REMOVE_REQ_OFFSET_RESERVED2, KPadding );
        isimessage.Set8bit( nameOffset, iName[0] );
        isimessage.Set8bit( nameOffset + 1, iName[1] );
        isimessage.Set8bit( nameOffset + 2, iName[2] );
        isimessage.Set8bit( nameOffset + 3, iName[3] );
        isimessage.Complete();
        err = iIscApi.Send( messageptr );
        C_TRACE( (_T("CBcaToIsc::SendNameRemoveReq() success: 0x%x"), err ) );
        delete message;
        }
    else
        {
        C_TRACE( (_T("CBcaToIsc::SendNameRemoveReq() memory allocation failed") ) );
        ASSERT(0);
        }

    C_TRACE( (_T("CBcaToIsc::SendNameRemoveReq() <-") ) );
    return err;
    }

/**
* Sends PNS_PEP_CTRL_RESP

* @return KErrNone if message sent succesfully, otherwise error code
*/    
TInt CBcaToIsc::SendPepCtrlResp( 
    TUint8 aTrId,
    TUint8 aReceiverDev,
    TUint8 aReceiverObj,
    TUint8 aPipeHandle,
    TUint8 aPepCtrlId,
    TUint8 aPepType,
    TUint8 aResult )
    {
    C_TRACE( (_T("CBcaToIsc::SendPepCtrlResp() ->") ) );

    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_PEP_CTRL_RESP );
    TInt err( KErrGeneral ); 

    if( message )
        {
        TPtr8 messageptr = message->Des();
        messageptr.FillZ();

        TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_PEP_CTRL_RESP );        
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, aReceiverDev);
        isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE);
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, aReceiverObj);
        isimessage.Set8bit( ISI_HEADER_OFFSET_TRANSID , aTrId);
        isimessage.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, PNS_PEP_CTRL_RESP );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PIPEHANDLE, aPipeHandle );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PEPCTRLID, aPepCtrlId );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PEPTYPE, aPepType );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_ERRORCODE, aResult );
        isimessage.Complete();
        err = iIscApi.Send( messageptr );
        C_TRACE( (_T("CBcaToIsc::SendPepCtrlResp() success: 0x%x"), err ) );
        delete message;
        }
    else
        {
        C_TRACE( (_T("CBcaToIsc::SendPepCtrlResp() memory allocation failed") ) );
        ASSERT(0);
        }

    C_TRACE( (_T("CBcaToIsc::SendPepCtrlResp() <-") ) );
    return err;
    }

/**
* Handles received messages

* @param aObjId object id for the name to be added
* @return KErrNone if message sent succesfully, otherwise error code
*/
void CBcaToIsc::RunL()
    {
    C_TRACE( (_T("CBcaToIsc::RunL() ->") ) );

    if ( KErrNone == iStatus.Int() )
        {
        HandleReceivedMsg( TIsiReceiveC( iMessageBufferPtr ) );
        }
    else
        {
        C_TRACE( (_T("CBcaToIsc::RunL() error occured: 0x%x"), iStatus.Int() ) );
        ASSERT(0);
        }

    C_TRACE( (_T("CBcaToIsc::RunL() <-") ) );
    }

/**
* Handles cancel operation for this active object

*/
void CBcaToIsc::DoCancel()
    {
    C_TRACE( (_T("CBcaToIsc::DoCancel() ->") ) );
    iIscApi.ReceiveCancel();
    C_TRACE( (_T("CBcaToIsc::DoCancel() <-") ) );
    }

/**
* Handles received message

*/
void CBcaToIsc::HandleReceivedMsg( const TIsiReceiveC& aIsiMsg )
    {
    C_TRACE( (_T("CBcaToIsc::HandleReceivedMsg() ->") ) );

    TInt resource( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );

    switch( resource )
        {
        case PN_NAMESERVICE:
            {
            HandleNameServiceMsg( aIsiMsg );
            break;
            }
        case PN_PIPE:
            {
            HandlePipeMsg( aIsiMsg );
            break;
            }
        default:
            {
            C_TRACE( (_T("CBcaToIsc::HandleReceivedMsg() unknown resource 0x%x"), resource ) );
            ASSERT(0);
            break;
            }
        }

    C_TRACE( (_T("CBcaToIsc::HandleReceivedMsg() <-") ) );
    }

/**
* Handles received messages from resource PN_NAME

*/
void CBcaToIsc::HandleNameServiceMsg( const TIsiReceiveC& aIsiMsg )
    {
    C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() ->") ) );
    
    TInt msgId( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );
    switch( msgId )
        {
        case PNS_NAME_ADD_RESP:
            {
            C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() PNS_NAME_ADD_RESP") ) );
            TUint8 reason( aIsiMsg.Get8bit( 
                ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ) );
            if( PN_NAME_OK != reason )
                {
                C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() Name adding failed: 0x%x"), reason ) );
                if( iClientOpenStatus )
                    {
                    C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() completing open") ) );
                    User::RequestComplete( iClientOpenStatus, KErrGeneral );
                    iClientOpenStatus = NULL;
                    iIscApi.Close();
                    }
                // no else
                }
            else
                {
                C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() Name added succesfully") ) );
                iMessageBufferPtr.SetLength( 0 );
                iStatus = KRequestPending;
                iIscApi.Receive( iStatus, iMessageBufferPtr, iReadLength );
                SetActive();
                }
            break;
            }
        case PNS_NAME_REMOVE_RESP:
            {
            C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() PNS_NAME_REMOVE_RESP") ) );
            TUint8 reason( aIsiMsg.Get8bit( 
                ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ) );
            if( PN_NAME_OK != reason )
                {
                C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() Name removing failed: 0x%x"), reason ) );
                ASSERT(0);
                if( iClientShutdownStatus )
                    {
                    C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() completing shutdown") ) );
                    User::RequestComplete(iClientShutdownStatus, KErrGeneral );
                    iClientShutdownStatus = NULL;
                    }
                }
            else
                {
                C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() Name removed succesfully") ) );
                iName.Zero();
                iMessageBufferPtr.SetLength( 0 );
                iStatus = KRequestPending;
                iIscApi.Receive( iStatus, iMessageBufferPtr, iReadLength );
                SetActive();
                }
            break;
            }
        default:
            {
            C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() unknown msg 0x%x"), msgId ) );
            break;
            }
        }

    C_TRACE( (_T("CBcaToIsc::HandleNameServiceMsg() <-") ) );
    }

/**
* Handles received messages from resource PN_PIPE

*/
void CBcaToIsc::HandlePipeMsg( const TIsiReceiveC& aIsiMsg )
    {
    C_TRACE( (_T("CBcaToIsc::HandlePipeMsg() ->") ) );

    TInt msgId( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );
    switch( msgId )
        {
        case PNS_PEP_CTRL_REQ:
            {
            C_TRACE( (_T("CBcaToIsc::HandlePipeMsg() PNS_PEP_CTRL_REQ") ) );
            HandlePepCtrlReq( aIsiMsg );
            break;
            }
        default:
            {
            C_TRACE( (_T("CBcaToIsc::HandlePipeMsg() unknown msg 0x%x"), msgId ) );
            ASSERT(0);
            break;
            }
        }
    
    C_TRACE( (_T("CBcaToIsc::HandlePipeMsg() <-") ) );
    }

/**
* Handles PNS_PEP_CTRL_REQ

*/
void CBcaToIsc::HandlePepCtrlReq( const TIsiReceiveC& aIsiMsg )
    {
    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() ->") ) );

    TUint8 pipeHandle( aIsiMsg.Get8bit( 
        ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PIPEHANDLE ) );
    TUint8 pepCtrlId( aIsiMsg.Get8bit( 
        ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLID ) );
    TUint8 pepType( aIsiMsg.Get8bit( 
        ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPTYPE ) );
    TUint8 result( PN_PIPE_ERR_NOT_SUPPORTED );

    TUint8 trId( aIsiMsg.Get8bit( 
        ISI_HEADER_OFFSET_TRANSID ) );
    // Get sender device
    TUint8 senderDevice( aIsiMsg.Get8bit(
        ISI_HEADER_OFFSET_SENDERDEVICE ) );
    // Get sender object
    TUint8 senderObject( aIsiMsg.Get8bit(
        ISI_HEADER_OFFSET_SENDEROBJECT ) );

    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() pipe handle: 0x%x"), pipeHandle ) );
    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() pep ctrl id: 0x%x"), pepCtrlId ) );
    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() pep type: 0x%x"), pepType ) );

    TBuf8<KDataLength> data( aIsiMsg.GetData( 
        ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA, 
        KDataLength ) );

    TUint8 devId( data[KDevIdIndex] );
    TUint8 objId( data[KObjIdIndex] );
    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() dev id: 0x%x"), devId ) );
    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() obj id: 0x%x"), objId ) );
    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() iChannelNumber: 0x%x"), iChannelNumber ) );

    if( PN_PEP_TYPE_BCA == pepType &&
        THIS_DEVICE == devId &&
        iChannelNumber == objId )
        {
        C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() PNS_PEP_CTRL_REQ for this PEP") ) );
        result = PN_PIPE_NO_ERROR;
        TInt ret( KErrNone );

        if( PEP_BCA_OK != data[KStatusIndex] )
            {
            C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() PNS_PEP_CTRL_REQ with error 0x%x"), data[KStatusIndex] ) );
            ret = KErrGeneral;
            }

        if( PEP_CTRL_ID_LOAN == pepCtrlId )
            {
            C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() ctrl id PEP_CTRL_ID_LOAN") ) );
            if( iClientOpenStatus )
                {
                C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() completing open") ) );
                User::RequestComplete( iClientOpenStatus, ret );
                iClientOpenStatus = NULL;
                }
             }
         else if( PEP_CTRL_ID_RETURNLOAN == pepCtrlId )
             {
             C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() ctrl id PEP_CTRL_ID_RETURNLOAN") ) );
             if( iClientShutdownStatus )
                 {
                 C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() completing shutdown") ) );
                 User::RequestComplete(iClientShutdownStatus, ret);
                 iClientShutdownStatus = NULL;
                 }
             }
         }

    SendPepCtrlResp(
        trId,
        senderDevice,
        senderObject,
        pipeHandle,
        pepCtrlId,
        pepType,
        result );

    C_TRACE( (_T("CBcaToIsc::HandlePepCtrlReq() <-") ) );
    }


