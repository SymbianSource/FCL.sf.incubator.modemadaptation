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



#ifndef ISAKERNELAPI_H
#define ISAKERNELAPI_H

//  INCLUDES
#include <klib.h>       // For DBase

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class DISAKernelWrapperHelper;

// CLASS DECLARATION

/**
*  Kernel interface for ISA Access Driver.
*
*  @lib IsaKernelAPI
*  @since S_CPR81
*/
NONSHARABLE_CLASS( DIsaKernelAPI ) : public DBase
{


    public:

    /**
    * Static constructor. Returns an new instance of the object.
    * NOTE! Kern::Fault raised if no free memory left.
    */
    static IMPORT_C DIsaKernelAPI* NewF();

    /**
    * Destructor.
    * Cancels any outstanding DFC requests and closes the channel.
    */
    IMPORT_C ~DIsaKernelAPI();

    /**
    * Get a free block for ISI message sending. Block is at least the size
    * of the param aSize.
    * NOTE! Kern::Fault raised if no free blocks left.
    * Preconditions: Channel is succesfully opened.
    * Postconditions: Block allocated and reference returned.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aSize, minimum size of the buffer.
    * @return TDes8&, reference to allocated block.
    */
    IMPORT_C TDes8& AllocateMsgBlock( const TInt aSize );

    /**
    * Get a free block for data message sending. Block is at least the size
    * of the param aSize. 
    * NOTE! Message allocated with this function must be
    * send with DataSend.
    * NOTE! Kern::Fault raised if no free blocks left.
    * Preconditions: Channel is succesfully opened.
    * Postconditions: Block allocated and reference returned.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aSize, minimum size of the buffer.
    * @return TDes8&, reference to allocated datablock.
    */
    IMPORT_C TDes8& AllocateDataMsgBlock( const TInt aSize );

    /**
    * Closes the channel. If channel is not open does nothing. Cancels any
    * outstanding DFC requests.
    * Preconditions: Channel is succesfully opened.
    * Postconditions: Channel closed and outstanding requests cancelled.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @return None
    */
    IMPORT_C void Close();

    /**
    * Receive message. DFC function given as param
    * shall be queued to run when message is received.
    * NOTE! After Receive DFC completion, client must deallocate block with
    * DeAllocateDataMsgBlock and set the pointer to NULL.
    * Preconditions: Channel is succesfully opened.
    *                           Parameter aDataMsgBlock must be NULL.
    * Postconditions: Receive is pending.
    * Kern::Fault will be raised if API can't allocate receive buffer.
    * Kern::Fault will be raised if same request is set when it is already pending.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aDataMsgBlock, block where received data message is written.
    * @param aDfcStatus, updated when DFC is queued. 
    *                    KErrNone, succefull
    *                    KErrCancel, receive was cancelled client no need to deallocate block get from receive.
    * @param aDataReceiveDfc, clients DFC function for receiving message. 
    * @return None
    */
    IMPORT_C void DataReceive( TDes8*& aDataMsgBlock,
                               TInt& aDfcStatus,
                               const TDfc& aDataReceiveDfc );

    /**
    * Cancels the pending data receive request.
    * Data receive cancellation will cancel pending DataReceive DFC with
    * status KErrCancel. If nothing pending does nothing.
    * Preconditions: Channel is succesfully opened.
    * Postconditions: DataReceive is cancelled.
    * Called automatically in destructor.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @return None
    */
    IMPORT_C void DataReceiveCancel();


    /**
    * Sends a data message and deallocates the block given as parameter.
    * Preconditions: Channel is succesfully opened.
    *                Descriptor was allocated with AllocateDataMsgBlock.
    * Postconditions: Data message block is deallocated.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param TDes8& aMsg, reference to message to be send.
    * @return TInt, an error code
    *               KErrNone, send succesfully
    *               KErrBadDescriptor, descriptor length too small.
    *               KErrUnderflow, ISI message length either bigger than descriptor
    *                              length or bigger than maximum ISI message length
    *                              or smaller than minimum ISI message length.
    *               KErrOverFlow, send queue overflow, client must resend.
    *               KErrNotReady, modem sw down, send failed, start listen state change, when ok, possible to resend.
    */
    IMPORT_C TInt DataSend( TDes8& aMsg );


    /**
    * Deallocates the message block retrieved with Receive.
    * NOTE! After deallocation set pointer to NULL.
    * Preconditions: Channel is succesfully opened.
    *                          Message was received with Receive.
    * Postconditions: Message block is deallocated.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aMsgBlock, reference to block to deallocate.
    * @return None
    */
    IMPORT_C void DeAllocateMsgBlock( TDes8& aMsgBlock );

    /**
    * Deallocates the data message block retrieved with DataReceive.
    * NOTE! After deallocation set pointer to NULL.
    * Preconditions: Channel is succesfully opened.
    *                          Data message was received with DataReceive.
    * Postconditions: Data message block is deallocated.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aMsgBlock, reference to data block to deallocate.
    * @return None
    */
    IMPORT_C void DeAllocateDataMsgBlock( TDes8& aMsgBlock );

    /**
    * Notifies flow control status change. When flowcontrol status changes
    * DFC will be queued and  status value updated to parameter.
    * Preconditions: Channel is succesfully opened.
    * Postconditions: Flowcontrol status notification is pending.
    * Kern::Fault will be raised if same request is set when it is already pending.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aNotifyFlowDfc, DFC function for flow status changes.
    * @param aDfcStatus, EIscFlowControlOn when datasend not allowed,
    *                    EIscFlowControlOff when datasend allowed,
    *                    EIscTransmissionEnd when datasend connection is removed
    *                    KErrCancel, when cancelled
    * @return None
    */
    IMPORT_C void NotifyFlowChange( TDfc& aNotifyFlowDfc,
                                    TInt& aDfcStatus );

    /** 
    * Cancels connection status change request.
    * Does nothing if request is not pending.
     * Preconditions: Channel is succesfully opened.
    * Postconditions: Flowcontrol status notification is cancelled.
    * Called automatically in destructor.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @return None
    */
    IMPORT_C void NotifyFlowChangeCancel();

    /**
    * Notifies connection status change. When connection status changes
    * DFC will be queued and connection status value updated to parameter.
    * Preconditions: Channel is succesfully opened.
    * Postconditions: Connection status notification is pending.
    * Kern::Fault will be raised if same request is set when it is already pending.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aNotifyStateDfc, DFC function for connection status changes.
    * @param aDfcStatus, EIscConnectionOk, EIscConnectionNotOk or KErrCancel
    * @return None
    */
    IMPORT_C void NotifyStateChange( TDfc& aNotifyStateDfc,
                                     TInt& aDfcStatus );

    /**
    * Cancels pending connection status notification with KErrCancel.
    * Does nothing if request not pending.
    * Preconditions: Channel is succesfully opened.
    * Postconditions: Connection status notification is cancelled.
    * Called automatically in destructor.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @return None 
    */
    IMPORT_C void NotifyStateChangeCancel() const;

    /**
    * Opens a clients channel. 
    * NOTE! If opened with resource client must give appropriate resource
    * as aResource.
    * Preconditions: Channel is closed.
    * Postconditions: Open is pending.
    * Kern::Fault will be raised if same request is set when it is already pending.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aChannelNumber, clients dedicated channel number.
    * @param aDfcStatus, updated when DFC queued. 
    *                KErrNone, is succesfully
    *                KErrAlreadyExist, channel already open
    *                KErrNotSupported, open with resource failed.
    * @param aOpenDfc, ref. to DFC  to be queued when channel is opened.
    * @param aResource, 32-bit resourceid default value (PN_NO_ROUTING (0xff)).
    * @return None
    */
    IMPORT_C void Open( const TUint16 aChannelNumber, TInt& aDfcStatus,
                        TDfc& aOpenDfc, const TUint32 aResource = 0x000000ff );

    /**
    * Cancels the pending open DFC request with KErrCancel.
    * If the open is not pending anymore does nothing.
    * Called automatically in destructor.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @return None
    */
    IMPORT_C void OpenCancel();

    /**
    * Receive message. DFC function given as param
    * shall be queued to run when message is received.
    * NOTE! After Receive DFC completion, client must deallocate block with
    * DeAllocateBlock and set the pointer to NULL.
    * Preconditions: Channel is succesfully opened.
    *                Parameter aMsgBlock must be NULL.
    * Postconditions: Receive is pending.
    * Kern::Fault will be raised if API can't allocate receive buffer.
    * Kern::Fault will be raised if same request is set when it is already pending.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aMsgBlock, block where receive message is stored.
    * @param aStatus, updated when DFC is queued. 
    *                 KErrNone, succefull
    *                 KErrCancel, receive was cancelled client no need to deallocate block get from receive.
    * @param aReceiveDfc, clients DFC function for receiving message. 
    * @return None
    */
    IMPORT_C void Receive( TDes8*& aMsgBlock, TInt& aStatus,
                           const TDfc& aReceiveDfc );

    /**
    * Cancels the pending receive DFC request with KErrCancel.
    * If the receive is not pending anymore does nothing.
    * Preconditions: Channel is succesfully opened.
    * Called automatically in destructor.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @return None
    */
    IMPORT_C void ReceiveCancel();

    /**
    * Sends a message and deallocates the block given as parameter.
    * Preconditions: Channel is succesfully opened.
    *                Descriptor was allocated with AllocateMsgBlock.
    * Postconditions: Message block is deallocated.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param TDes8& aMsg, reference to message to be send.
    * @return TInt, an error code
    *               KErrNone, send succesfully
    *               KErrBadDescriptor, descriptor length too small.
    *               KErrUnderflow, ISI message length either bigger than descriptor
    *                              length or bigger than maximum ISI message length
    *                              or smaller than minimum ISI message length.
    *               KErrOverFlow, send queue overflow, client must resend.
    *               KErrNotReady, modem sw down, send failed, start listen state change, when ok, possible to resend.
    */
    IMPORT_C TInt Send( TDes8& aMsg );

    /**
    * Sends an event to CMT. NOTE! Only servers in ISA-sense can do this.
    * Preconditions: Channel is succesfully opened with resource.
    *                Block was allocated with AllocateMsgBlock.
    * Postconditions: Indication was send.
    *                 Block was deallocated.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param TDes8& aMsg, reference to indication to be send.
    * @return TInt, an error code.
    *               KErrNone, send succesfully
    *               KErrBadDescriptor, descriptor length too small.
    *               KErrUnderflow, ISI message length either bigger than descriptor
    *                              length or bigger than maximum ISI message length
    *                              or smaller than minimum ISI message length.
    *               KErrOverFlow, send queue overflow, client must resend.
    *               KErrNotReady, modem sw down, send failed, start listen state change, when ok, possible to resend.
    */
    IMPORT_C TInt SendIndication( TDes8& aMsg );

    /**
    * Subscribe indications. Indications are received with
    * Receive.
    * NOTE! Don't dellocate the descriptor for indications.
    * 8-bit subscribing list item. Must be div. by two.
    *                               byte1:[8bit resourceid]
    *                               byte2:[8bit indicationid]
    * 32-bit subscribing list item. Must be div. by five.
    *                               byte1:[32-24 bits of resourceid]
    *                               byte2:[24-16 bits of resourceid]
    *                               byte3:[16-8 bits of resourceid]
    *                               byte4:[8-0 bits of resourceid]
    *                               byte5:[8bit indicationid]
    * Preconditions: Channel is succesfully opened.
    * Postconditions: Channel is add as indication(s) subscriber.
    * Called in kernel thread context.
    * Called with no fastmutex held.
    * Called with interrupts enabled.
    * Called without kernel lock held.
    * @param aIndications, indication subscription list.
    * @param aThirtyTwoBit, EFalse by default, ETrue when 32-bit subscribing used. 
    * @return TInt, an error code.
    *               KErrNone, subscribe succesfully
    *               KErrBadDescriptor, descriptor length too small or not according to subscribing list items.
    *               KErrUnderflow, subscribing list item too long.
    *               KErrOverFlow, send queue overflow, client must resend.
    *               KErrNotReady, modem sw down, send failed, start listen state change, when ok, possible to resend.
    */
    IMPORT_C TInt SubscribeIndications( const TDesC8& aIndications,
                                        const TBool aThirtyTwoBit = EFalse );

    private:

    /**
     * Private constructor.
     * One reason class size BC.
     */
    DIsaKernelAPI();

    private:

    // Owned
    // Kernel channel dedicated for this.
    DISAKernelWrapperHelper*      iHelper;

    };

#endif // ISAKERNELAPI_H

// End of File.

