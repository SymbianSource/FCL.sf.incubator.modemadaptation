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


#include "isimsgqueue.h"        // For DISIMsgQueue
#include "isiroutertrace.h"     // For C_TRACE..

// Fault identifiers
enum TISIMsgQueueFaults
    {
    EISIMsgQueueMemAllocFail = 0x01,
    EISIMsgQueueMemAllocFail2,
    EISIMsgQueueInvalidQueueSize,
    EISIMsgQueueNotEmpty,
    EISIMsgQueueNotInSync,
    EISIMsgQueueNotInSync2,
    EISIMsgQueueNotInSync3,
    EISIMsgQueueNotInSync4,
    EISIMsgQueueNotInSync5,
    };

DISIMsgQueue::DISIMsgQueue(
        const TUint16 aSize
        )
    {

    C_TRACE( ( _T( "DISIMsgQueue::DISIMsgQueue 0x%x %d>" ), this, aSize ) );
    iQueueMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iQueueMutex, ( EISIMsgQueueMemAllocFail | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    iSize = aSize;
    ASSERT_RESET_ALWAYS( ( iSize > 0 ), ( EISIMsgQueueInvalidQueueSize | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    iShInputIndex = 0;
    iShOutputIndex = 0;
    iShCount = 0;
    // Reserve memory for the buffer.
    iShRingBuffer = new TDes8*[ aSize ];
    C_TRACE( ( _T( "DISIMsgQueue::DISIMsgQueue 0x%x" ), iShRingBuffer ) );
    ASSERT_RESET_ALWAYS( iShRingBuffer, ( EISIMsgQueueMemAllocFail2 | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    for( TInt i( 0 ); i < iSize; ++i )
        {
        iShRingBuffer[ i ] = NULL;
        }
    C_TRACE( ( _T( "DISIMsgQueue::DISIMsgQueue 0x%x<" ), this ) );

    }

DISIMsgQueue::~DISIMsgQueue(
        // None
        )
    {
    
    C_TRACE( ( _T( "DISIMsgQueue::~DISIMsgQueue 0x%x %d>" ), this, iShCount ) );
    ASSERT_RESET_ALWAYS( iShCount == 0, ( EISIMsgQueueNotEmpty | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    // NOTE! This does not deallocate the blocks from the allocated memory just the pointers!
    // Take each individual pointer from the array and free delete them.
    for( TInt i( 0 ); i < iSize; ++i )
        {
        TDes8* temp = iShRingBuffer[ i ];
        if( temp )
            {
            C_TRACE( ( _T( "DISIMsgQueue::~DISIMsgQueue ptr 0x%x" ), temp ) );
            delete temp;
            temp = NULL;
            }
        }
    if( iShRingBuffer )
        {
        C_TRACE( ( _T( "DISIMsgQueue::~DISIMsgQueue iShRingBuffer 0x%x iShRingBuffer[0] 0x%x" ), iShRingBuffer, iShRingBuffer[0] ) );
        // Delete the space reserved for the array. Marks it as non-used do not set to NULL.
        delete [] iShRingBuffer;
//        iShRingBuffer = NULL;
        }

    iSize = 0;
    iShInputIndex = 0;
    iShOutputIndex = 0;
    iShCount = 0;

    C_TRACE( ( _T( "DISIMsgQueue::~DISIMsgQueue iQueueMutex" ) ) );
    // Only modified in constructor if allocated failed, already reseted.
    delete iQueueMutex;
    iQueueMutex = NULL;
    C_TRACE( ( _T( "DISIMsgQueue::~DISIMsgQueue 0x%x<" ), this ) );

    }

void DISIMsgQueue::Add(
        const TDesC8& aMessage
        )
    {

    C_TRACE( ( _T( "DISIMsgQueue::Add 0x%x %d %d %d %d 0x%x>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMessage ) );
    NKern::FMWait( iQueueMutex );
    // If queue get's overfilled throw kernel fault.
    ASSERT_RESET_ALWAYS( ( iShCount < iSize ), ( EISIMsgQueueNotInSync | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    // Place the buffer into the queue.
    iShRingBuffer[ iShInputIndex ] = static_cast<TDes8*>( &const_cast<TDesC8&>( aMessage ) );
    // Adjust input pointer.
    iShInputIndex = ( ( iShInputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iShCount++;
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DISIMsgQueue::Add 0x%x %d %d %d %d 0x%x<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMessage ) );

    }

TUint16 DISIMsgQueue::Count(
        // None
        )
    {

    C_TRACE( ( _T( "DISIMsgQueue::Count 0x%x %d>" ), this, iShCount ) );
    TUint16 count( KErrNone );
    NKern::FMWait( iQueueMutex );
    // If count is too big.
    ASSERT_RESET_ALWAYS( ( iShCount <= iSize ), ( EISIMsgQueueNotInSync2 | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    count = iShCount;
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DISIMsgQueue::Count 0x%x %d %d<" ), this, iShCount, count ) );
    return count;

    }

TDes8& DISIMsgQueue::Get(
        // None
        )
    {

    C_TRACE( ( _T( "DISIMsgQueue::Get 0x%x %d %d %d %d>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex ) );
    NKern::FMWait( iQueueMutex );
    // If queue is empty.
    ASSERT_RESET_ALWAYS( ( iShCount > 0 ), ( EISIMsgQueueNotInSync3 | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    // Get the buffer from the queue.
    TDes8* temp = iShRingBuffer[ iShOutputIndex ];
    // Set buffer location to NULL.
    iShRingBuffer[ iShOutputIndex ] = NULL;
    // Adjust output pointer.
    iShOutputIndex = ( ( iShOutputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iShCount--;
    NKern::FMSignal( iQueueMutex );
    ASSERT_RESET_ALWAYS( temp, ( EISIMsgQueueNotInSync4 | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIMsgQueue::Get 0x%x %d %d %d %d 0x%x<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, temp ) );
    return *temp;

    }

void DISIMsgQueue::RollBack(
        const TDesC8& aMsgToRoll
        )
    {

    C_TRACE( ( _T( "DISIMsgQueue::RollBack 0x%x %d %d %d %d 0x%x>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMsgToRoll ) );
    NKern::FMWait( iQueueMutex );
    ASSERT_RESET_ALWAYS( ( iShCount < iSize ), ( EISIMsgQueueNotInSync5 | EDISIMsgQueueTraceId << KClassIdentifierShift ) );
    // If needs to rollback from 0 to iSize -1 index.
    iShOutputIndex = ( iShOutputIndex == 0 ) ? iSize : iShOutputIndex; 
    // Adjust output pointer.
    iShOutputIndex = ( ( iShOutputIndex - 1 ) % iSize );
    // Get the descriptor from queue where the rollback is made.
    iShRingBuffer[ iShOutputIndex ] = static_cast<TDes8*>( &const_cast<TDesC8&>( aMsgToRoll ) );
    // Remember the amount of the requests in the queue.
    iShCount++;    
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DISIMsgQueue::RollBack 0x%x %d %d %d %d 0x%x<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, &aMsgToRoll ) );

    }

// End of file.
