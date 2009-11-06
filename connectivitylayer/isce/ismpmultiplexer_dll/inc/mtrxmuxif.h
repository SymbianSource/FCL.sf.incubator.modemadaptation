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



/** @mtrxmuxif.h
@brief Abstract interface class. Part of IST API.
@publishedDeviceAbstraction
*/

#ifndef __MTRXMUXIF_H__
#define __MTRXMUXIF_H__

// Include files
#include <e32def.h>         // For TInt
#include <e32cmn.h>         // For TDes8

// Macros
/*
A equals to asserted value and b equals to TMultiplexerFault codes. 
*/
#define MUX_ASSERT_FAULT_MACRO( a, b ) if( !( a ) ){ Kern::Fault("mtrxmuxif:", b ); }

// Forward declarations
class MMuxTrxIf;

/**
@brief Abstract interface class to use multiplexer services.

Interface implemented by multiplexer and used from transceiver.

Class is used for registering and unregistering transceiver to multiplexer.
Class is used for receiving data from transceiver to multiplexer.
Class is used for deallocating over the interconnection transmitted datas memory block.
*/
class MTrxMuxIf
    {

    public:

        /**
        Interface functions fault values.
        */
        enum TMultiplexerFault
           {
           ENotKernelThreadContext     = 1,
           ENullTrxPointer,
           EInvalidTrxId,
           ETrxAlreadyRegistered,
           ETrxNotRegistered,
           EInvalidMuxingHeader,
           EInvalidSize,
           };

        /**
        Allocates a memory block for data receiving over the interconnection.
        Blocks descriptor length is set to zero, but maximum length can be bigger
        than size given as parameter (it depends on memory block configurations).
        Memory content of the descriptor is filled with zeros until maximum length.
        Memory allocated for the block is physically contigous and non cached.
        Note! Transceiver shall register before with MTrxMuxIf::Register.
        Note! Transceiver shall transfer the data block to multiplexer before with MTrxMuxIf::Receive.
        If not called in kernel thread context fault ENotKernelThreadContext thrown with MUX_ASSERT_FAULT_MACRO.
        If size is zero, fault EInvalidSize thrown with MUX_ASSERT_FAULT_MACRO.
        Execution:          Synchronous
        Re-entrant:         No
        Can block:          Yes
        Panic mode:         Kern::Fault
        Memory allocation:  No
        Execution time:     Quick
        SMP safe:           Yes
        @param aSize 16-bit minimum size of the needed data block
        @return reference to allocated block
        @pre Called always in kernel thread context
        @pre No fastmutex held
        @post Calling thread not blocked
        */
        virtual TDes8& AllocateBlock( const TUint16 aSize ) = 0;

        /**
        Deallocates a memory block transmitted over the interconnection.
        Note! Transceiver shall register before with MTrxMuxIf::Register.
        Note! Transceiver shall receive the block before with MMuxTrxIf::Transmit.
        If not called in kernel thread context fault ENotKernelThreadContext thrown with MUX_ASSERT_FAULT_MACRO.
        Execution:          Synchronous
        Re-entrant:         No
        Can block:          Yes
        Panic mode:         Kern::Fault
        Memory allocation:  No
        Execution time:     Quick
        SMP safe:           Yes
        @param aBlock reference to block to be deallocated
        @return void
        @pre Called always in kernel thread context
        @pre No fastmutex held
        @post Calling thread not blocked
        */
        virtual void DeallocateBlock( TDes8& aBlock ) = 0;

        /**
        Transfers data received over the interconnection from transceiver to multiplexer.
        Transceiver is responsible of allocating the data block and giving correct 32-bit multiplexing protocol header.
        Multiplexer is responsible of multiplexing and deallocating the transferred data block.
        Note! Transceiver shall register before with MTrxMuxIf::Register.
        Note! Transceiver shall allocate the block before with MTrxMuxIf::AllocateBlock.
        If not called in kernel thread context fault ENotKernelThreadContext thrown with MUX_ASSERT_FAULT_MACRO.
        If invalid multiplexing protocol header fault EInvalidMuxingHeader thrown with MUX_ASSERT_FAULT_MACRO.
        If invalid lenght either in data or in multiplexing protocol header fault EInvalidSize thrown with MUX_ASSERT_FAULT_MACRO.
        Execution:          Synchronous
        Re-entrant:         No
        Can block:          Yes
        Panic mode:         Kern::Fault
        Memory allocation:  No
        Execution time:     Quick
        SMP safe:           Yes
        @param aData reference to received data
        @param aMuxingHeader multiplexing protocol header formulated by other systems multiplexer
        @return void
        @pre Called always in kernel thread context
        @pre No fastmutex held
        @post Calling thread not blocked
        */
        virtual void Receive( TDes8& aData, const TUint32 aMuxingHeader ) = 0;

        /**
        Registers a transceiver to multiplexer and returns an interface to it.
        Transceiver shall call this function when interconnection to other system becomes functional.
        Note! Multiplexer shall be initialized before.
        If not called in kernel thread context fault ENotKernelThreadContext thrown with MUX_ASSERT_FAULT_MACRO.
        If aTrx is NULL fault ENullTrxPointer thrown with MUX_ASSERT_FAULT_MACRO.
        If aTrxId is invalid fault EInvalidTrxId thrown with MUX_ASSERT_FAULT_MACRO.
        If transceiver is already registered fault ETrxAlreadyRegistered thrown with MUX_ASSERT_FAULT_MACRO.
        Execution:          Synchronous
        Re-entrant:         No
        Can block:          Yes
        Panic mode:         Kern::Fault
        Memory allocation:  Yes
        Execution time:     Quick
        SMP safe:           Yes
        @param aTrx, pointer to transceiver
        @param aTrxId, transceiver unique identifier (from trxdefs.h)
        @return pointer to multiplexer interface
        @pre Called always in kernel thread context
        @pre No fastmutex held
        @post Calling thread not blocked
        */
        IMPORT_C static MTrxMuxIf* Register( MMuxTrxIf* aTrx, const TUint8 aTrxId );

        /**
        Unregisters transceiver
        Transceiver shall call this function when interconnection to other system becomes non-functional.
        Note! Transceiver shall register before with MTrxMuxIf::Register.
        If not called in kernel thread context fault ENotKernelThreadContext thrown with MUX_ASSERT_FAULT_MACRO.
        If transceiver is not registered fault ETrxNotRegistered thrown with MUX_ASSERT_FAULT_MACRO.
        Execution:          Synchronous
        Re-entrant:         No
        Can block:          Yes
        Panic mode:         Kern::Fault
        Memory allocation:  No
        Execution time:     Quick
        SMP safe:           Yes
        @return void
        @pre Called always in kernel thread context
        @pre No fastmutex held
        @post Calling thread not blocked
        */
        virtual void Unregister() = 0;

    };

#endif // __MTRXMUXIF_H__
