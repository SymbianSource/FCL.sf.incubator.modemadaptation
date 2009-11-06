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



#include "msgqueue.h"           // For DMsgQueue
#include "p2proutertrace.h"     // For C_TRACE..

// Fault identifiers
enum TDMsgQueueFaults
    {
    EDMsgQueueMemAllocFail = 0x00,
    EDMsgQueueMemAllocFail2,
    EDMsgQueueInvalidQueueSize,
    EDMsgQueueNotEmpty,
    EDMsgQueueNotInSync,
    EDMsgQueueNotInSync2,
    EDMsgQueueNotInSync3,
    EDMsgQueueNotInSync4,
    EDMsgQueueNotInSync5,
    };

DMsgQueue::DMsgQueue(
        const TUint16 aSize
        )
    {

    C_TRACE( ( _T( "DMsgQueue::DMsgQueue 0x%x %d>" ), this, aSize ) );
    iQueueMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iQueueMutex, ( EDMsgQueueMemAllocFail | EDMsgQueueTraceId << KClassIdentifierShift ) );
    iSize = aSize;
    ASSERT_RESET_ALWAYS( ( iSize > 0 ), ( EDMsgQueueInvalidQueueSize | EDMsgQueueTraceId << KClassIdentifierShift ) );
    iShInputIndex = 0;
    iShOutputIndex = 0;
    iShCount = 0;
    // Reserve memory for the buffer.
    iShRingBuffer = new TDes8*[ aSize ];
    C_TRACE( ( _T( "DMsgQueue::DMsgQueue 0x%x" ), iShRingBuffer ) );
    ASSERT_RESET_ALWAYS( iShRingBuffer, ( EDMsgQueueMemAllocFail2 | EDMsgQueueTraceId << KClassIdentifierShift ) );
    for( TInt i( 0 ); i < iSize; ++i )
        {
        iShRingBuffer[ i ] = NULL;
        }
    C_TRACE( ( _T( "DMsgQueue::DMsgQueue 0x%x<" ), this ) );

    }

DMsgQueue::~DMsgQueue(
        // None
        )
    {
    
    C_TRACE( ( _T( "DMsgQueue::~DMsgQueue 0x%x %d>" ), this, iShCount ) );
    ASSERT_RESET_ALWAYS( iShCount == 0, ( EDMsgQueueNotEmpty | EDMsgQueueTraceId << KClassIdentifierShift ) );
    // NOTE! This does not deallocate the blocks from the allocated memory just the pointers!
    for( TInt i( 0 ); i < iSize; ++i )
        {
        TDes8* temp = iShRingBuffer[ i ];
        if( temp )
            {
            C_TRACE( ( _T( "DMsgQueue::~DMsgQueue ptr 0x%x" ), temp ) );
            delete temp;
            temp = NULL;
            }
        }
    if( iShRingBuffer )
        {
        C_TRACE( ( _T( "DMsgQueue::~DMsgQueue iShRingBuffer 0x%x iShRingBuffer[0] 0x%x" ), iShRingBuffer, iShRingBuffer[0] ) );
        delete [] iShRingBuffer;
        iShRingBuffer = NULL;
        }

    iSize = 0;
    iShInputIndex = 0;
    iShOutputIndex = 0;
    iShCount = 0;

    C_TRACE( ( _T( "DMsgQueue::~DMsgQueue iQueueMutex" ) ) );
    // Only modified in constructor if allocated failed, already reseted.
    delete iQueueMutex;
    iQueueMutex = NULL;
    C_TRACE( ( _T( "DMsgQueue::~DMsgQueue 0x%x<" ), this ) );

    }

void DMsgQueue::Add(
        const TDesC8& aMessage
        )
    {

    C_TRACE( ( _T( "DMsgQueue::Add 0x%x %d %d %d %d 0x%x>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMessage ) );
    NKern::FMWait( iQueueMutex );
    // If queue get's overfilled throw kernel fault.
    ASSERT_RESET_ALWAYS( ( iShCount < iSize ), ( EDMsgQueueNotInSync | EDMsgQueueTraceId << KClassIdentifierShift ) );
    // Place the buffer into the queue.
    iShRingBuffer[ iShInputIndex ] = static_cast<TDes8*>( &const_cast<TDesC8&>( aMessage ) );
    // Adjust input pointer.
    iShInputIndex = ( ( iShInputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iShCount++;
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DMsgQueue::Add 0x%x %d %d %d %d 0x%x<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMessage ) );

    }

TUint16 DMsgQueue::Count(
        // None
        )
    {

    C_TRACE( ( _T( "DMsgQueue::Count 0x%x %d>" ), this, iShCount ) );
    TUint16 count( KErrNone );
    NKern::FMWait( iQueueMutex );
    // If count is too big.
    ASSERT_RESET_ALWAYS( ( iShCount <= iSize ), ( EDMsgQueueNotInSync2 | EDMsgQueueTraceId << KClassIdentifierShift ) );
    count = iShCount;
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DMsgQueue::Count 0x%x %d %d<" ), this, iShCount, count ) );
    return count;

    }

TDes8& DMsgQueue::Get(
        // None
        )
    {

    C_TRACE( ( _T( "DMsgQueue::Get 0x%x %d %d %d %d>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex ) );
    NKern::FMWait( iQueueMutex );
    // If queue is empty.
    ASSERT_RESET_ALWAYS( ( iShCount > 0 ), ( EDMsgQueueNotInSync3 | EDMsgQueueTraceId << KClassIdentifierShift ) );
    // Get the buffer from the queue.
    TDes8* temp = iShRingBuffer[ iShOutputIndex ];
    // Set buffer location to NULL.
    iShRingBuffer[ iShOutputIndex ] = NULL;
    // Adjust output pointer.
    iShOutputIndex = ( ( iShOutputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iShCount--;
    NKern::FMSignal( iQueueMutex );
    ASSERT_RESET_ALWAYS( temp, ( EDMsgQueueNotInSync4 | EDMsgQueueTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DMsgQueue::Get 0x%x %d %d %d %d 0x%x<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, temp ) );
    return *temp;

    }

void DMsgQueue::RollBack(
        const TDesC8& aMsgToRoll
        )
    {

    C_TRACE( ( _T( "DMsgQueue::RollBack 0x%x %d %d %d %d 0x%x>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMsgToRoll ) );
    NKern::FMWait( iQueueMutex );
    ASSERT_RESET_ALWAYS( ( iShCount < iSize ), ( EDMsgQueueNotInSync5 | EDMsgQueueTraceId << KClassIdentifierShift ) );
    // If needs to rollback from 0 to iSize -1 index.
    iShOutputIndex = ( iShOutputIndex == 0 ) ? iSize : iShOutputIndex; 
    // Adjust output pointer.
    iShOutputIndex = ( ( iShOutputIndex - 1 ) % iSize );
    // Get the descriptor from queue where the rollback is made.
    iShRingBuffer[ iShOutputIndex ] = static_cast<TDes8*>( &const_cast<TDesC8&>( aMsgToRoll ) );
    // Remember the amount of the requests in the queue.
    iShCount++;    
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DMsgQueue::RollBack 0x%x %d %d %d %d 0x%x<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMsgToRoll ) );

    }

// End of file.
