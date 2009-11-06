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



#ifndef __MP2PIF_H__
#define __MP2PIF_H__

#include <p2pdefs.h>    // For TP2PProtocol

// Forward declarations
class MP2PIf;

/*
* Factory class for creating the Point-to-point interface.
*/
class TP2PFactory
    {

    public:

        /*
        * Creates a new P2P inteface.
        * Interface must be released with MP2PIf::Release
        * Execution:    Synchronous
        * Re-entrant:   Yes
        * Blocking:     No
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @return pointer to new interface
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @post Calling thread not blocked
        */
        IMPORT_C static MP2PIf* NewP2PIfF();

    };

/**
* An interface for transceiving point-to-point (P2P) data between
* two end points according to used P2P protocol.
* Interface is used from kernel side.
* NOTE! Do remember binary compatibility issues when chaning the interface.
*/
class MP2PIf
{

    public:

        /**
        * Returns a free block for data sending.
        * Blocks descriptor length is set to zero, but maximum length can be bigger
        * than size given as parameter (it depends on memory block configurations).
        * Responsibility to deallocate the block is transferred to caller.
        * NOTE! Interface shall be opened before with MP2PIf::Open or Kern::Fault raised.
        * NOTE! Kern::Fault raised if no free blocks left.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @param aSize, minimum size of the buffer.
        * @return TDes8&, reference to allocated block.
        * @pre Called always in kernel thread context.
        * @pre No fastmutex held.
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual TDes8& AllocateBlock( const TUint16 aSize ) = 0;

        /**
        * Closes the interface and cancels any outstanding DFC requests.
        * If interface is not opened with MP2PIf::Open function does nothing.
        * Automatically called in Release.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void Close() = 0;

        /*
        * Notifies when connection to other point is lost.
        * DFC function given as param shall be queued to run when connection is lost.
        * Automatically cancelled in MP2PIf::Close.
        * All data received after connection is lost shall be discarded.
        * NOTE! Interface shall be opened before with MP2PIf::Open or Kern::Fault raised.
        * NOTE! After connection lost MP2PIf::Close shall be called.
        * Execution:    Asynchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @param aStatus, updated when connection is lost
        *                 KErrNotReady when connection is lost
        *                 KErrCancel when request is cancelled
        * @param aConnectionResetedCompletedDfc, clients DFC function to notify connection lost
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void ConnectionLost( TInt& aStatus, const TDfc& aConnectionResetedCompletedDfc ) = 0;

        /*
        * Cancels the pending connection lost request with KErrCancel.
        * NOTE! Interface shall be opened before with MP2PIf::Open or Kern::Fault raised.
        * Automatically called in MP2PIf::Close
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void ConnectionLostCancel() = 0;


        /**
        * Deallocates the data block retrieved with MP2PIf::Receive.
        * NOTE! After deallocation set pointer to NULL.
        * NOTE! Interface shall be opened before with MP2PIf::Open or Kern::Fault raised.
        * NOTE! Data shall be received before with MP2PIf::Receive.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @param aDataBlock, reference to block to deallocate.
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void DeallocateBlock( TDes8& aDataBlock ) = 0;

        /**
        * Asynchronously opens the interface.
        * When open request is set to pending blocked caller thread is released.
        * Automatically cancelled in MP2PIf::Close.
        * NOTE! Inteface either not opened or closed with MP2PIf::Close.
        * NOTE! Kern::Fault will be raised if same request is set when it is already pending.
        * Execution:    Asynchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @param aP2PProtocolId, dedicated P2P protocol id.
        * @param aDfcStatus, updated when DFC queued.
        *                    KErrNone, succesfully opened
        *                    KErrCancel, outstanding request is cancelled
        *                    KErrInUse if someone uses the same protocol id
        *                    KErrAlreadyExists same interface object is opened with other protocol id
        * @param aOpenCompleteDfc, reference to DFC to be queued when open completes.
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void Open( const TP2PProtocol aP2PProtocolId, TInt& aDfcStatus, TDfc& aOpenCompleteDfc ) = 0;

        /**
        * Cancels the pending open DFC request with KErrCancel.
        * If the request is not pending anymore does nothing.
        * Automatically called in MP2PIf::Close
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void OpenCancel() = 0;

        /**
        * Receives data from other point.
        * Either receives data immediately or set the receive request pending.
        * Responsibility to deallocate the data is transferred to caller.
        * DFC function given as param shall be queued to run when data is received.
        * Automatically cancelled in MP2PIf::Close.
        * NOTE! Interface shall be opened before with MP2PIf::Open or Kern::Fault raised.
        * NOTE! After Receive DFC completion, client shall deallocate the block with
        * MP2PIf::DeallocateBlock and set aRxDataBlock pointer to NULL.
        * NOTE! pointer to the aRxDataBlock shall not be modified from other DFC functions
        * only from aReceiveCompletedDfc function.
        * NOTE! If interface is not opened when other point is sending data to it data shall
        * be deallocated.
        * NOTE! Kern::Fault will be raised if aRxDataBlock ptr is not NULL.
        * NOTE! Kern::Fault will be raised if API can't allocate receive buffer.
        * NOTE! Kern::Fault will be raised if same request is set when it is already pending.
        * Execution:    Asynchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @param aRxDataBlock, block where received data shall be stored.
        * @param aStatus, updated when DFC is queued.
        *                 KErrNone, succefull
        *                 KErrCancel, receive was cancelled client no need to deallocate block get from receive.
        * @param aReceiveCompletedDfc, clients DFC function for receiving data. 
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void Receive( TDes8*& aRxDataBlock, TInt& aStatus, const TDfc& aReceiveCompletedDfc ) = 0;

        /**
        * Cancels the pending receive request with KErrCancel.
        * If the request is not pending anymore does nothing.
        * Automatically called in MP2PIf::Close
        * NOTE! Interface shall be opened before with MP2PIf::Open or Kern::Fault raised.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void ReceiveCancel() = 0;

        /*
        * Deletes the concret object of the interface and releases reserved resources.
        * Caller is responsible to set the interface pointer to NULL.
        * NOTE! Called when interface shall not be used anymore (usually after MP2PIf::Close).
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual void Release() = 0;

        /**
        * Sends the data and deallocates the block given as parameter.
        * Responsibility to deallocate the block is transferred.
        * NOTE! Interface shall be opened before with MP2PIf::Open or Kern::Fault raised.
        * NOTE! Descriptor shall be allocated before with MP2PIf::AllocateMsgBlock.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   Kern::Fault
        * SMP safe:     Yes
        * Called in kernel thread context.
        * Called with no fastmutex held.
        * Called with interrupts enabled.
        * Called without kernel lock held.
        * @param TDes8& aData, reference to the data to be send.
        * @return TInt, an error code
        *               KErrNone, data send
        *               KErrNotReady, connection to other point lost and data discarded, MP2PIf::Close shall be called after this
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Called with interrupts enabled and kernel unlocked.
        * @post Calling thread not blocked
        */
        virtual TInt Send( TDes8& aData ) = 0;

    };

#endif // __MP2PIF_H__

// End of File.

