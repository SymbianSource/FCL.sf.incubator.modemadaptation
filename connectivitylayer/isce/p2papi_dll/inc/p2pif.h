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



#ifndef __P2PIF_H__
#define __P2PIF_H__

#include <e32cmn.h>     // For RBusLogicalChannel
#include <p2pdefs.h>    // For TP2PProtocol

/**
* An interface for transceiving point-to-point (P2P) data between
* two end points according to used P2P protocol.
* Interface is used from user side.
*/
class RP2PIf : public RBusLogicalChannel
    {

    public:

        /*
        * C++ default constructor.
        */
        IMPORT_C RP2PIf();

        /*
        * Destructor
        */
        IMPORT_C ~RP2PIf();

        /*
        * Closes the interface and cancels any outstanding requests with KErrCancel.
        * If interface is not opened with RP2PIf::Open does nothing.
        * Automatically called in destructor.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @post Calling thread not blocked
        */
        IMPORT_C void Close();

        /*
        * Notifies when connection to other point is lost.
        * Automatically cancelled in RP2PIf::Close.
        * All data received after connection is lost shall be discarded.
        * NOTE! If interface is not open panicked with User::Panic.
        * NOTE! After connection lost RP2PIf::Close shall be called.
        * Execution:    Asynchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @param aStatus, updated when connection is lost
        *                 KErrNotReady when connection is lost
        *                 KErrCancel when request is cancelled
        * @post Calling thread not blocked
        */
        IMPORT_C void ConnectionLost( TRequestStatus& aStatus );

        /*
        * Cancels the pending connection lost request with KErrCancel.
        * Automatically cancelled in RP2PIf::Close.
        * NOTE! If interface is not open panicked with User::Panic.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @post Calling thread not blocked
        */
        IMPORT_C void ConnectionLostCancel();

        /**
        * Asynchronously opens a P2P protocol communication path from this point to another.
        * Open is not complited before the communication path is open.
        * When request is set to pending blocked calling thread is released.
        * Automatically cancelled in RP2PIf::Close.
        * Execution:    Asynchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @param aP2PProtocolId, unique identifier of the used point-to-point protocol
        * @param aStatus, KErrNone if ok
        *                 KErrNotSupported if protocol is not supported
        *                 KErrInUse if someone uses the same protocol id
        *                 KErrAlreadyExists same interface object is opened with other protocol id
        *                 KErrCancel if outstanding request is cancelled
        *                 KErrPermissionDenied if not enough capabilities
        * @param aHandleType, specifies whether the handle is owned by process or by thread.
        * @post Calling thread not blocked
        */
        IMPORT_C void Open( const TP2PProtocol aP2PProtocolId,
                            TRequestStatus& aStatus,
                            TOwnerType aHandleType = EOwnerThread );

        /**
        * Cancel asynchronous channel opening with KErrCancel.
        * Does nothing if request is not pending.
        * Automatically called in RP2PIf::Close.
        * NOTE! If interface is not open panicked with User::Panic.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @post Calling thread not blocked
        */
        IMPORT_C void OpenCancel();

        /**
        * Receive data from other point.
        * Either receives data immediately or set the receive request pending.
        * When request is set to pending blocked calling thread is released.
        * Automatically cancelled in RP2PIf::Close.
        * NOTE! If interface is not open panicked with User::Panic.
        * NOTE! If interface is not opened when other point is sending data to it data shall
        * be deallocated.
        * Execution:    Asynchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @param aStatus, KErrNone if ok.
        *                 KErrBadDescriptor if receive buffer is invalid or too small.
        * @param aReceiveBuffer, buffer where to receive data from kernel
        */
        IMPORT_C void Receive( TRequestStatus& aStatus, TDes8& aReceiveBuffer );

        /**
        * Cancel asynchronous receive request with KErrCancel.
        * Does nothing if request is not pending.
        * Automatically called in RP2PIf::Close.
        * NOTE! If interface is not open panicked with User::Panic.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @post Calling thread not blocked
        */
        IMPORT_C void ReceiveCancel();

        /**
        * Sends data to other point.
        * NOTE! If interface is not open panicked with User::Panic.
        * NOTE! Absolutely maximum send buffer size is 16-bit value.
        * Execution:    Synchronous
        * Re-entrant:   No
        * Blocking:     Yes
        * Panic mode:   User::Panic
        * SMP safe:     Yes
        * @param aSendBuffer, data to be send
        * @return KErrNone, is succesfull
        *                   KErrNoMemory buffer size is over the maximum size
        *                   KErrNotReady connection to other point lost, data lost
        *                   KErrBadDescriptor if send buffer is invalid
        * @post Calling thread not blocked
        */
        IMPORT_C TInt Send( const TDesC8& aSendBuffer );

    };

#endif      // __P2PIF_H__

// End of File
