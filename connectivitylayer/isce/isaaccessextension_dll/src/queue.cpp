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



#include "queue.h"
#include "iadtrace.h"    // For C_TRACE..
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "queueTraces.h"
#endif

// NOTE!!! Ensure thattrace prints are out of critical section's at least with fast mutex / semaphore.

// CONSTS
const TInt KOneLeft( 1 );

EXPORT_C DQueue::DQueue(
        const TInt aSize
        // None
        )
    {
    OstTrace1( TRACE_NORMAL, DQUEUE_DQUEUE_ENTRY, ">DQueue::DQueue;aSize=%d", aSize );

    C_TRACE( ( _T( "DQueue::DQueue 0x%x %d ->" ), this, aSize ) );
    iQueueMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iQueueMutex, EIADMemoryAllocationFailure | EIADFaultIdentifier14 << KFaultIdentifierShift );
    iSize = aSize;
    ASSERT_RESET_ALWAYS( ( iSize >= KOneLeft ), EIADQueueOutOfSync | EIADFaultIdentifier15 << KFaultIdentifierShift );
    iInputIndex = 0;
    iOutputIndex = 0;
    iCount = 0;
    // Reserve memory for the buffer.
    iRingBuffer = new TDes8*[ aSize ];
    C_TRACE( ( _T( "DQueue::DQueue 0x%x" ), iRingBuffer ) );
    ASSERT_RESET_ALWAYS( iRingBuffer, EIADMemoryAllocationFailure | EIADFaultIdentifier15 << KFaultIdentifierShift );
    for( TInt i( 0 ); i < iSize; ++i )
        {
        iRingBuffer[ i ] = NULL;
        }    
    C_TRACE( ( _T( "DQueue::DQueue 0x%x <-" ), this ) );
        
    OstTrace0( TRACE_NORMAL, DQUEUE_DQUEUE, "<DQueue::DQueue" );
    }

EXPORT_C DQueue::~DQueue(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DUP1_DQUEUE_DQUEUE_ENTRY, ">DQueue::~DQueue" );

    C_TRACE( ( _T( "DQueue::~DQueue 0x%x %d ->" ), this, iCount ) );
    // NOTE! This don't deallocate the blocks from the allocated memory just the pointers!
    ASSERT_RESET_ALWAYS( iCount == 0, EIADQueueOutOfSync | EIADFaultIdentifier1 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DQueue::~DQueue TBRx" ) ) );
    for( TInt i( 0 ); i < iSize; ++i )
        {
        TDes8* temp = iRingBuffer[ i ];
        if( temp )
            {
            C_TRACE( ( _T( "DQueue::~DQueue ptr 0x%x" ), temp ) );
            OstTrace1( TRACE_NORMAL, DUP1_DQUEUE_DQUEUE, "DQueue::~DQueue;temp=%x", temp );
            
            delete temp;
            temp = NULL;
            }
        }
    C_TRACE( ( _T( "DQueue::~DQueue TBRz" ) ) );
    ///* TODO!!! CHECK THIS OUT
    if( iRingBuffer )
        {
        C_TRACE( ( _T( "DQueue::~DQueue iRingBuffer 0x%x, iRingBuffer[0] 0x%x" ), iRingBuffer, iRingBuffer[0] ) );
        OstTraceExt2( TRACE_NORMAL, DUP2_DQUEUE_DQUEUE, "DQueue::~DQueue;iRingBuffer=%x;iRingBuffer[0]=%x", (TUint)(iRingBuffer), (TUint)(iRingBuffer[0]) );
        delete [] iRingBuffer;
        iRingBuffer = NULL;
        }

    iSize = 0;
    iInputIndex = 0;
    iOutputIndex = 0;
    iCount = 0;
    if( iQueueMutex )
        {
        C_TRACE( ( _T( "DQueue::~DQueue iQueueMutex" ) ) );
        OstTrace0( TRACE_NORMAL, DUP3_DQUEUE_DQUEUE, "DQueue::~DQueue iQueueMutex" );        
        delete iQueueMutex;
        iQueueMutex = NULL;
        }
    C_TRACE( ( _T( "DQueue::~DQueue 0x%x <-" ), this ) );

    OstTrace0( TRACE_NORMAL, DUP1_DQUEUE_DQUEUE_EXIT, "<DQueue::~DQueue" );
    }

EXPORT_C void DQueue::Add(
        const TDesC8& aMessage
        )
    {
    OstTrace1( TRACE_NORMAL, DQUEUE_ADD_ENTRY, ">DQueue::Add;aMessage=%x", ( TUint )&( aMessage ) );

    C_TRACE( ( _T( "DQueue::Add 0x%x %d %d %d %d 0x%x ->" ), this, iSize, iCount, iInputIndex, iOutputIndex, &aMessage ) );
    NKern::FMWait( iQueueMutex );
    // If queue get's overfilled throw kernel fault.
    // TODO: to change with cmt so that returns error code before when queue gets full
    ASSERT_RESET_ALWAYS( ( iCount < iSize ), EIADQueueOutOfSync | EIADFaultIdentifier2 << KFaultIdentifierShift );
    // Place the buffer into the queue.
    iRingBuffer[ iInputIndex ] = static_cast<TDes8*>( &const_cast<TDesC8&>( aMessage ) );
    // Adjust input pointer.
    iInputIndex = ( ( iInputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iCount++;
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DQueue::Add 0x%x %d %d %d %d 0x%x <-" ), this, iSize, iCount, iInputIndex, iOutputIndex, &aMessage ) );

    OstTrace0( TRACE_NORMAL, DQUEUE_ADD, "<DQueue::Add" );
    }

EXPORT_C TUint16 DQueue::Count(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DQUEUE_COUNT_ENTRY, ">DQueue::Count" );

    C_TRACE( ( _T( "DQueue::Count 0x%x %d <->" ), this, iCount ) );
    TUint16 count( KErrNone );
    NKern::FMWait( iQueueMutex );
    // If count is too big.
    ASSERT_RESET_ALWAYS( ( iCount <= iSize ), EIADQueueOutOfSync | EIADFaultIdentifier3 << KFaultIdentifierShift );
    count = iCount;
    NKern::FMSignal( iQueueMutex );

    OstTraceExt1( TRACE_NORMAL, DQUEUE_COUNT, "<DQueue::Count;count=%hu", count );    
    return count;
    }

EXPORT_C TDes8& DQueue::Get(
		// None
        )
    {
    OstTraceExt5( TRACE_NORMAL, DQUEUE_GET_ENTRY, ">DQueue::Get;iSize=%hu;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu;this=%x", iSize, iCount, iInputIndex, iOutputIndex, (TUint)(this));

    C_TRACE( ( _T( "DQueue::Get 0x%x %d %d %d %d ->" ), this, iSize, iCount, iInputIndex, iOutputIndex ) );
    NKern::FMWait( iQueueMutex );
    // If queue is empty.
    ASSERT_RESET_ALWAYS( ( iCount > 0 ), EIADQueueOutOfSync | EIADFaultIdentifier4 << KFaultIdentifierShift );
    // Get the buffer from the queue.
    TDes8* temp = iRingBuffer[ iOutputIndex ];
    // Set buffer location to NULL.
    iRingBuffer[ iOutputIndex ] = NULL;
    // Adjust output pointer.
    iOutputIndex = ( ( iOutputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iCount--;
    NKern::FMSignal( iQueueMutex );
    ASSERT_RESET_ALWAYS( temp, EIADQueueOutOfSync | EIADFaultIdentifier5 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DQueue::Get 0x%x %d %d %d %d 0x%x <-" ), this, iSize, iCount, iInputIndex, iOutputIndex, temp ) );

    OstTraceExt5( TRACE_NORMAL, DQUEUE_GET_EXIT, "<DQueue::Get;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu;this=%x;temp=%x", iCount, iInputIndex, iOutputIndex, (TUint)(this), (TUint)(temp) );
    return *temp;
    }

EXPORT_C void DQueue::RollBack(
        const TDesC8& aMsgToRoll
        )
    {
    OstTraceExt5( TRACE_NORMAL, DQUEUE_ROLLBACK_ENTRY, "DQueue::RollBack;aMsgToRoll=%x;iSize=%hu;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu", ( TUint )&( aMsgToRoll ), iSize, iCount, iInputIndex, iOutputIndex );
    OstTrace1( TRACE_NORMAL, DQUEUE_ROLLBACK, "DQueue::RollBack;this=%x", (TUint)(this) );


    C_TRACE( ( _T( "DQueue::RollBack 0x%x %d %d %d %d 0x%x ->" ), this, iSize, iCount, iInputIndex, iOutputIndex, &aMsgToRoll ) );
    NKern::FMWait( iQueueMutex );
    ASSERT_RESET_ALWAYS( ( iCount < iSize ), EIADQueueOutOfSync | EIADFaultIdentifier6 << KFaultIdentifierShift );
    // If needs to rollback from 0 to iSize -1 index.
    iOutputIndex = ( iOutputIndex == 0 ) ? iSize : iOutputIndex; 
    // Adjust output pointer.
    iOutputIndex = ( ( iOutputIndex - 1 ) % iSize );
    // Get the descriptor from queue where the rollback is made.
    iRingBuffer[ iOutputIndex ] = static_cast<TDes8*>( &const_cast<TDesC8&>( aMsgToRoll ) );
    // Remember the amount of the requests in the queue.
    iCount++;    
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DQueue::RollBack 0x%x %d %d %d %d 0x%x <-" ), this, iSize, iCount, iInputIndex, iOutputIndex, &aMsgToRoll ) );

    OstTraceExt5( TRACE_NORMAL, DQUEUE_ROLLBACK_EXIT, "<DQueue::RollBack;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu;aMsgToRoll=%x;this=%x", iCount, iInputIndex, iOutputIndex, (TUint)&(aMsgToRoll), (TUint)(this) );
    }

TIADReq::TIADReq()
:iRequest( EIADAsyncLast ), iCompleteStatus( KRequestPending )
	{
	OstTrace0( TRACE_NORMAL, TIADREQ_TIADREQ_ENTRY, "<>TIADReq::TIADReq" );
	// None
	}

EXPORT_C TIADReq::TIADReq( TIADAsyncRequest aReq, TInt aStat  )
:iRequest( aReq), iCompleteStatus( aStat )
{
    OstTraceExt2( TRACE_NORMAL, DUP1_TIADREQ_TIADREQ_ENTRY, "<>TIADReq::TIADReq;aReq=%x;aStat=%d", ( TUint )&( aReq ), aStat );
    ASSERT_RESET_ALWAYS( iRequest != EIADAsyncLast, EIADQueueOutOfSync | EIADFaultIdentifier7 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( iCompleteStatus != KRequestPending, EIADQueueOutOfSync | EIADFaultIdentifier8 << KFaultIdentifierShift );
}


EXPORT_C DReqQueue::DReqQueue()
    {
    OstTrace1( TRACE_NORMAL, DREQQUEUE_DREQQUEUE_ENTRY, ">DReqQueue::DReqQueue;this=%x", (TUint)(this) );

    C_TRACE( ( _T( "DReqQueue::DReqQueue 0x%x %d ->" ), this ) );
    iQueueMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iQueueMutex, EIADMemoryAllocationFailure | EIADFaultIdentifier16 << KFaultIdentifierShift );
    iSize = EIADAsyncLast;
    ASSERT_RESET_ALWAYS( ( iSize >= KOneLeft ), EIADQueueOutOfSync | EIADFaultIdentifier9 << KFaultIdentifierShift );
    iInputIndex = 0;
    iOutputIndex = 0;
    iCount = 0;
    for( TInt i( 0 ); i < iSize; ++i )
        {
        iReqRingBuffer[ i ].iRequest = EIADAsyncLast;
        iReqRingBuffer[ i ].iCompleteStatus = KRequestPending;
        iReqList[ i ] = NULL;
        }    
    C_TRACE( ( _T( "DReqQueue::DReqQueue 0x%x <-" ), this ) );

    OstTrace1( TRACE_NORMAL, DREQQUEUE_DREQQUEUE_EXIT, "<DReqQueue::DReqQueue;this=%x", (TUint)(this) );
    }

EXPORT_C DReqQueue::~DReqQueue()
    {
    OstTraceExt2( TRACE_NORMAL, DUP1_DREQQUEUE_DREQQUEUE_ENTRY, "DReqQueue::~DReqQueue;this=%x;iCount=%hu", (TUint)(this), iCount );

    C_TRACE( ( _T( "DReqQueue::~DReqQueue 0x%x %d ->" ), this, iCount ) );
    // NOTE! This don't deallocate the blocks from the allocated memory just the pointers!
    ASSERT_RESET_ALWAYS( iCount == 0, EIADQueueOutOfSync | EIADFaultIdentifier10 << KFaultIdentifierShift );
    for( TInt i( 0 ); i < iSize; ++i )
        {
        iReqRingBuffer[ i ].iRequest = EIADAsyncLast;
        iReqRingBuffer[ i ].iCompleteStatus = KRequestPending;
        iReqList[ i ] = NULL;
        }
    iSize = 0;
    iInputIndex = 0;
    iOutputIndex = 0;
    iCount = 0;
    if( iQueueMutex )
        {
        C_TRACE( ( _T( "DReqQueue::~DReqQueue iQueueMutex" ) ) );
        delete iQueueMutex;
        iQueueMutex = NULL;
        }
    C_TRACE( ( _T( "DReqQueue::~DReqQueue 0x%x <-" ), this ) );

    OstTrace1( TRACE_NORMAL, DUP1_DREQQUEUE_DREQQUEUE_EXIT, "<DReqQueue::~DReqQueue;this=%x", this );
    }

/* 
* In case of queue overflow throw kern::fault, REQ: isaaccessdriver should not lose ISI nor data messages.
* Adds the message in the end of the ringbuffer queue.
* @param const TDesC8& aMessage, message to be added.
*/
EXPORT_C TBool DReqQueue::Add( TIADReq aReq )
    {
    OstTraceExt5( TRACE_NORMAL, DREQQUEUE_ADD_ENTRY, "DReqQueue::Add;aReq=%x;iSize=%hu;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu", ( TUint )&( aReq ), iSize, iCount, iInputIndex, iOutputIndex );
    OstTrace1( TRACE_NORMAL, DREQQUEUE_ADD, ">DReqQueue::Add;this=%x", this );

    C_TRACE( ( _T( "DReqQueue::Add 0x%x %d %d %d %d ->" ), this, iSize, iCount, iInputIndex, iOutputIndex ) );
    TBool ok( EFalse );
    NKern::FMWait( iQueueMutex );
    // If queue get's overfilled throw kernel fault.
    ASSERT_RESET_ALWAYS( ( iCount < iSize ), EIADQueueOutOfSync | EIADFaultIdentifier11 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( EIADAsyncLast > aReq.iRequest, EIADOverTheLimits | EIADFaultIdentifier11 << KFaultIdentifierShift );
    if( iReqList[ aReq.iRequest ] )
        {
        // Place the buffer into the queue.
        iReqRingBuffer[ iInputIndex ] = aReq;
        // Adjust input pointer.
        iInputIndex = ( ( iInputIndex + 1 ) % iSize );
        // Remember the amount of the requests in the queue.
        iCount++;
        ok = ETrue;
        }
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DReqQueue::Add 0x%x %d %d %d %d %d <-" ), this, iSize, iCount, iInputIndex, iOutputIndex, ok ) );
    
    OstTraceExt5( TRACE_NORMAL, DUP1_DREQQUEUE_ADD_EXIT, "DReqQueue::Add;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu;ok=%x;this=%x", iCount, iInputIndex, iOutputIndex, (TUint)(ok), (TUint)(this) );    
    return ok;
    }

/*
*Returns the first pointer ( iOutputIndex ) from the ring buffer.
*/
EXPORT_C TBool DReqQueue::Empty()
    {
    OstTraceExt2( TRACE_NORMAL, DREQQUEUE_EMPTY_ENTRY, "DReqQueue::Empty;this=%x;iCount=%hu", (TUint)this, iCount );

    C_TRACE( ( _T( "DReqQueue::Empty 0x%x %d <->" ), this, iCount ) );
    return iCount == 0 ?  ETrue : EFalse;

    }

/*
*Returns the first pointer ( iOutputIndex ) from the ring buffer.
*/
EXPORT_C TIADReq DReqQueue::Get()
    {
    OstTraceExt5( TRACE_NORMAL, DREQQUEUE_GET_ENTRY, "DReqQueue::Get;iSize=%hu;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu;this=%x", iSize, iCount, iInputIndex, iOutputIndex, (TUint)this );

    C_TRACE( ( _T( "DReqQueue::Get 0x%x %d %d %d %d ->" ), this, iSize, iCount, iInputIndex, iOutputIndex ) );
    NKern::FMWait( iQueueMutex );
    // If queue is empty.
    ASSERT_RESET_ALWAYS( ( iCount > 0 ), EIADQueueOutOfSync | EIADFaultIdentifier12 << KFaultIdentifierShift );
    // Get the buffer from the queue.
    ASSERT_RESET_ALWAYS( EIADAsyncLast > iOutputIndex, EIADOverTheLimits | EIADFaultIdentifier12 << KFaultIdentifierShift );
    TIADReq temp = iReqRingBuffer[ iOutputIndex ];
    // Set buffer location to NULL.
    iReqRingBuffer[ iOutputIndex ].iRequest = EIADAsyncLast;
    iReqRingBuffer[ iOutputIndex ].iCompleteStatus = KRequestPending;
    // Adjust output pointer.
    iOutputIndex = ( ( iOutputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iCount--;
    NKern::FMSignal( iQueueMutex );
    ASSERT_RESET_ALWAYS( temp.iRequest != EIADAsyncLast, EIADQueueOutOfSync | EIADFaultIdentifier13 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( temp.iCompleteStatus != KRequestPending, EIADQueueOutOfSync | EIADFaultIdentifier14 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DQueue::Get 0x%x %d %d %d %d <-" ), this, iSize, iCount, iInputIndex, iOutputIndex ) );
    
    OstTraceExt5( TRACE_NORMAL, DREQQUEUE_GET, "<DReqQueue::Get;iCount=%hu;iInputIndex=%hu;iOutputIndex=%hu;this=%x;temp=%x", iCount, iInputIndex, iOutputIndex, (TUint)(this), (TUint)&temp );       
    return temp;
    }

/*
* Set req active / deactive. Default deactive.
*/
EXPORT_C void DReqQueue::SetReq( TIADAsyncRequest aReqToSet, TRequestStatus* aStatus )
    {
    OstTraceExt3( TRACE_NORMAL, DREQQUEUE_SETREQ_ENTRY, ">DReqQueue::SetReq;aReqToSet=%x;aStatus=%x;this=%x", ( TUint )&( aReqToSet ), ( TUint )( aStatus ), (TUint)(this) );

    C_TRACE( ( _T( "DReqQueue::SetReq 0x%x %d 0x%x ->" ), this, aReqToSet, aStatus ) );
    ASSERT_RESET_ALWAYS( aReqToSet < EIADAsyncLast, EIADWrongRequest | EIADFaultIdentifier13 << KFaultIdentifierShift );
    // If setting same request twice.
    C_TRACE( ( _T( "DReqQueue::SetReq 0x%x %d 0x%x 0x%x TBR" ), this, aReqToSet, aStatus, iReqList[ aReqToSet ] ) );
    //jos !NULL ja !NULL fault
    ASSERT_RESET_ALWAYS( !( !iReqList[ aReqToSet ] && aStatus == NULL ), EIADCommon );
    iReqList[ aReqToSet ] = aStatus;
    C_TRACE( ( _T( "DReqQueue::SetReq 0x%x %d 0x%x <-" ), this, aReqToSet, aStatus ) );

    OstTraceExt3( TRACE_NORMAL, DREQQUEUE_SETREQ_EXIT, "<DReqQueue::SetReq;aReqToSet=%x;aStatus=%x;this=%x", ( TUint )&( aReqToSet ), ( TUint )( aStatus ), (TUint)(this) );
    }

/*
* Set req active / deactive. Default deactive.
*/
EXPORT_C TRequestStatus* DReqQueue::GetReq( TIADAsyncRequest aReqToGet )
    {
    OstTraceExt2( TRACE_NORMAL, DREQQUEUE_GETREQ_ENTRY, ">DReqQueue::GetReq;aReqToGet=%x;this=%x", (TUint)this, ( TUint )&( aReqToGet ) );

    ASSERT_RESET_ALWAYS( aReqToGet < EIADAsyncLast, EIADWrongRequest | EIADFaultIdentifier14 << KFaultIdentifierShift );
    
    C_TRACE( ( _T( "DReqQueue::GetReq 0x%x 0x%x %d <->" ), this, iReqList[ aReqToGet ], aReqToGet ) );
    
    TRequestStatus* tmpStatus = iReqList[ aReqToGet ];

    OstTraceExt3( TRACE_NORMAL, DREQQUEUE_GETREQ_EXIT, "<DReqQueue::GetReq;this=%x;aReqToGet=%x;retStatusPtr=%x", (TUint)this, ( TUint )&( aReqToGet ), (TUint)tmpStatus );    
    return tmpStatus;
    }

// End of file.
