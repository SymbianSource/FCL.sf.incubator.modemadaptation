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



#ifndef MDATAMESSAGESTATUS_H
#define MDATAMESSAGESTATUS_H

class MDataMessageStatus
    {

   public:

    /*
    * Called when datamessage is delivered to client.
    * @param none
    * @return void
    */
    virtual void DataMessageDelivered() = 0;

    };

#endif // MDATAMESSAGESTATUS_H


#ifndef MIAD2CHANNEL_H
#define MIAD2CHANNEL_H

#include "iadinternaldefinitions.h"    // For TIADConnectionStatus
//#include "mist2iadapi.h"  // For TIADConnectionStatus // 
#include "misiobjectrouterif.h"
#include "misirouterobjectif.h"

// abstract interface for both channels user & kernel 
// called by extension, provided by extension
class MIAD2ChannelApi : public MISIRouterObjectIf
    {
    
    public:

    /*
    * Complete async request with given params.
    * @param TInt aRequest, request to complete.
    * @param TInt aStatusToComplete, status of the request to complete.
    * @return void
    */  
    virtual void CompleteChannelRequest( TInt aRequest, TInt aStatusToComplete ) = 0;
    
    /*
    * Info channel about the connection status.
    * @param TIADConnectionStatus aStatus, new connection status
    * @return void
    */
    virtual void NotifyConnectionStatus( TIADConnectionStatus aStatus ) = 0;

    /*
    * Info channel about the flowcontorl status.
    * @param TIADFlowControlStatus aStatus, new flow control status
    * @return void
    */    
    virtual void NotifyFlowCtrl( TIADFlowControlStatus aStatus) = 0;

    // When data message is written to user thread call
    // MDataMesssageStatus::DataMessageDelivered. It's responsibility is to
    // tell PEP that one data message e.g.a credit is used. PEP's should also
    // keep count of data messages added to channel to be abel to control the
    // legacy flowcontrol

    /*
    * Give channel a reference to received data message.
    * @param const TDesC8& aDataMessage
    * @return void
    */        
    virtual void ReceiveDataMsg( const TDesC8& aDataMessage ) = 0;

    /*
    * Give channel a reference to received message.
    * @param const TDesC8& aMessage
    * @return void
    */    
    virtual void ReceiveMsg( const TDesC8& aMessage ) = 0;

    /*
           * Register pep associated to this channel to listen data message deliveries.
    * @param MDataMessageStatus& aPep
    * @return void
    */    
    virtual void RegisterPep( MDataMessageStatus& aPep ) = 0;

    };

#endif // MIAD2CHANNEL_H

#ifndef MCHANNEL2IADAPI_H
#define MCHANNEL2IADAPI_H

// abstract inteface for LDD to access Extension services
// *called by LDD
// *provided from extension
// *must take care of syncronization, LDD and kernel channels using this
class MChannel2IADApi : public MISIObjectRouterIf
    {

    public:

    /*
    * Allocate a block. Descriptors length is set to aSize, but MaxLength can
    * can be bigger, depends on memory block configurations.
    * @param const TUint16 aSize
    * @return TDes8&
    */      
    virtual TDes8& AllocateBlock( const TUint16 aSize ) = 0;

    virtual TDes8& AllocateDataBlock( const TUint16 aSize ) = 0;

    virtual void Close( const TUint16 aChannel ) = 0;

    virtual void DeAllocateBlock(TDes8& aBlock) = 0;

    virtual TInt GetConnectionStatus() = 0;
  
    virtual TInt GetFlowControlStatus() = 0;

    virtual TInt GetMaxDataSize() = 0;

    virtual void Open( const TUint16 aChannel, const TUint16 aRequest, MIAD2ChannelApi* aCallback ) = 0;

    virtual void Open( const TUint16 aChannel, const TUint16 aRequest, const TDesC8& aOpenInfo, MIAD2ChannelApi* aCallback ) = 0;

    virtual TInt OrderIndication( TDes8& anOrder, const TUint16 aCh, const TBool a32Bit = EFalse ) = 0;

    virtual TInt SendMessage( TDes8& aMessage, const TUint16 aCh ) = 0;

    /*
    * Sends an ISI indication message.
    * @param TDes8& aMessage, indication message
    * @param const TUint16 aCh, sender channels id.
    * @return TInt, KErrNone if send succesfull.
    */
    virtual TInt SendIndication( TDes8& aMessage, const TUint16 aCh ) = 0;

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
    virtual TInt Loan( const TUint16 aChannel, const TUint16 aRequest,
                       MIAD2ChannelApi* aCallback ) = 0;

    virtual TInt ReturnLoan( const TUint16 aChannel, const TUint16 aRequest,
                             MIAD2ChannelApi* aCallback ) = 0;
#endif

    };

#endif // MCHANNEL2IADAPI_H

