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

#ifndef __ISIIF_H__
#define __ISIIF_H__

// Forward declarations
#ifdef __KERNEL_MODE__
class TDfc;
#endif


/**
* Class MISIIf defines an abstract interface for ISA entities.
* Interface is used to transceive ISI messages between ISA entities.
*/
class MISIIf
    {

    public:

#ifdef __KERNEL_MODE__
        /**
        * Returns a free block for sending ISI messages. Blocks descriptor length is set
        * to zero, but maximum length can be more than size given as parameter.
        * Interface shall be created before with MISIIf::NewISIIf or Kern::Fault raised.
        * Fault is raised if no free blocks left.
        * Responsibility to deallocate the block is transferred to caller.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @param aSize, minimum size of the buffer in bytes.
        * @return TDes8&, reference to allocated block.
        * @pre Called always in kernel thread context.
        * @pre No fastmutex held.
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked.
        */
        virtual TDes8& AllocateMsgBlock( const TUint16 aSize ) = 0;
      
        
        /**
        * Deallocates the ISI message block retrieved with MISIIf::Receive.
        * After deallocation set pointer to NULL. Interface shall be created
        * before with MISIIf::NewISIIf or Kern::Fault raised. ISI message shall
        * be received before with MISIIf::Receive.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @param aMsgBlock, reference to block to deallocate.
        * @pre Called always in kernel thread context.
        * @pre No fastmutex held.
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked.
        */
        virtual void DeallocateMsgBlock( TDes8& aMsgBlock ) = 0;
#endif


        /**
        * Creates a new ISI interface.
        * Interface must be released with MISIIf::Release.
        * If no memory available panics. When same UID already used Kern::Fault raised.
        * Execution:    Synchronous
        * Re-entrant:   Yes
        * Blocking:     No
        * Panic mode:   Kernel: Kern::Fault, User: User::Panic
        * SMP safe:     Yes
        * @param aUID, clients unique UID identifier. TODO: Symbian UID
        * @param aObjId, reference to write clients object id.
        * @return pointer to new interface.
        * @pre Called always in thread context (Kernel: in kernel thread context).
        * @pre No fastmutex held. (Kernel only)
        * @post Calling thread not blocked.
        */
        IMPORT_C static MISIIf* NewISIIf( const TInt32 aUID, TUint8& aObjId );


        /**
        * Receive an ISI message from other ISA entity. Interface shall be created
        * before with MISIIf::NewISIIf or panic raised. Receive shall always be
        * pending after interface is created.
        * Execution:    Asynchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kernel: Kern::Fault, User: User::Panic
        * SMP safe:     Yes
        * @param aRxStatus, updated when receive either succesful or failed.
                            KErrNone, receive succesful.
                            KErrCancel, outstanding request is cancelled.
                            KErrOverflow, not enough memory to store the message.
        * @param aRxMsg, reference to store received message (Kernel only pointer reference).
        * @param aNeededBufLen, if receive buffer too short correct lenght is written to this (User only).
        * @param aRxCompletedDfc, DFC executed when receive is completed (Kernel only).
        * @pre Called always in thread context (Kernel: in kernel thread context).
        * @pre No fastmutex held (Kernel only).
        * @post Calling thread not blocked.
        */
        virtual void Receive( TRequestStatus& aRxStatus,
#ifndef __KERNEL_MODE__
                      TDes8& aRxMsg,
                      TUint16& aNeededBufLen
#else
                      TDes8*& aRxMsg,
                      const TDfc& aRxCompletedDfc
#endif
                     ) = 0;


        /**
        * Cancels the pending receive request with KErrCancel.
        * If the request is not pending anymore does nothing.
        * Automatically called in MISIIf::Release. Interface shall be created
        * before with MISIIf::NewISIIf or panic raised.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kernel: Kern::Fault, User: User::Panic
        * SMP safe:     Yes
        * @pre Called always in kernel thread context.
        * @pre No fastmutex held (Kernel only).
        * @pre Called with interrupts enabled and kernel unlocked (Kernel only).
        * @post Calling thread not blocked.
        */
        virtual void ReceiveCancel() = 0;


        /**
        * Deletes the concret object of the interface and releases reserved resources.
        * Cancels pending receive request. Caller is responsible to set the interface pointer to NULL.
        * In case of User::Panic called automatically.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kernel: Kern::Fault, User: User::Panic
        * SMP safe:     Yes
        * @pre Called always in kernel thread context.
        * @pre No fastmutex held (Kernel only).
        * @pre Called with interrupts enabled and kernel unlocked (Kernel only).
        * @post Calling thread not blocked.
        */
        virtual void Release() = 0;


        /**
        * Send ISI message to other ISA entity. Interface shall be created before 
        * with MISIIf::NewISIIf or panic raised.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kernel: Kern::Fault, User: User::Panic
        * SMP safe:     Yes
        * @param aTxMsg, ISI message to be send.
        * @return success code of the operation.
                  KErrNone, send succesful.
                  KErrOverflow, too long message tried to send.
        * @pre Called always in thread context (Kernel: in kernel thread context).
        * @pre No fastmutex held (Kernel only).
        * @post Calling thread not blocked.
        */       
        virtual TInt Send( const TDesC8& aTxMsg ) = 0;

    };

#endif      // _ISIIF_H_

// End of File
