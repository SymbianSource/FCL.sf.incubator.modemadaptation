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



#include <kern_priv.h>              // For DMutex
#include "isiuserchannel.h"         // For DISIUserChanneel
#include "isiroutertrace.h"         // For C_TRACE, ASSERT_RESET.. and fault codes
#include "misichannelrouterif.h"    // For MISIChannelRouterIf
#include "memapi.h"                 // For MemApi
#include "isimsgqueue.h"            // For DISIMsgQueue

const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );
const TInt KNoParams( 0 );
const TInt KOneParam( 1 );
const TInt KTwoParams( 2 );
const TInt KThreeParams( 3 );
const TInt KFirstRequest( 0 );
const TInt KDestStartOffset( 0 );

enum TISIUserChannelFaults
    {
    EISIUserChannelMemAllocFail = 0x01,
    EISIUserChannelMemAllocFail1,
    EISIUserChannelMemAllocFail2,
    EISIUserChannelMemAllocFail3,
    EISIUserChannelWrongRequest,
    EISIUserChannelWrongRequest1,
    EISIUserChannelWrongRequest2,
    EISIUserChannelWrongRequest3,
    EISIUserChannelWrongRequest4,
    EISIUserChannelWrongRequest5,
    EISIUserChannelWrongRequest6,
    EISIUserChannelReqOverTheLimits,
    EISIUserChannelDesWriteFailed,
    EISIUserChannelSameRequest,
    EISIUserChannelNullParam,
    EISIUserChannelNullParam2,
    EISIUserChannelDesReadFailed,
    EISIUserChannelfNotThreadContext,
    EISIUserChannelfNotThreadContext2,
    EISIUserChannelfNotThreadContext3,
    EISIUserChannelfNotThreadContext4,
    EISIUserChannelfNotThreadContext5,
    };

const TInt KISILddEmptyRxQueuePriori( 1 );
const TInt KISILddCompleteRequestPriori( 1 );


// user<> kernel interaction() done in LDD DFC thread
// 
// kernel<>kernel interaction() done in Extension DFC thread
//
// DEMAND_PAGING
// Receive (write k>u) is done only in LDD thread context to allow Extension thread to continue when dp swaps.
// Send ((write u>k) is not done at the moment in LDD thread context only. Check is it possible to happend (not to be in usable memory after send (unlikely?)).

DISIUserChannel::DISIUserChannel(
        // None
        ) : iThread( &Kern::CurrentThread()),
            iObjId( KNotInitializedId ),
            iUID( KNotInitializedUID )
    {
    C_TRACE( ( _T( "DISIUserChannel::DISIUserChannel>" ) ) );
    iRouterIf = MISIChannelRouterIf::GetIf();
    ASSERT_RESET_ALWAYS( iRouterIf, ( EISIUserChannelMemAllocFail | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    iCompletionThread = iRouterIf->GetDfcThread( MISIChannelRouterIf::EISIUserRequestCompletionThread );
    iEmptyRxQueueDfc = new TDfc( EmptyRxQueueDfc, this, iCompletionThread, KISILddEmptyRxQueuePriori );
    ASSERT_RESET_ALWAYS( iEmptyRxQueueDfc, ( EISIUserChannelMemAllocFail2 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    iCompleteChannelRequestDfc = new TDfc( CompleteChannelRequestDfc, this, iCompletionThread, KISILddCompleteRequestPriori );
    ASSERT_RESET_ALWAYS( iCompleteChannelRequestDfc, ( EISIUserChannelMemAllocFail3 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    iRequests = new DISIUserAsyncRequests( EISILastAsyncRequest );   
    C_TRACE( ( _T( "DISIUserChannel::DISIUserChannel<" ) ) );
    }

DISIUserChannel::~DISIUserChannel(
        // None
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::~DISIUserChannel 0x%x>" ), this ) );
     // owned starts
    if( iEmptyRxQueueDfc )
        {
        C_TRACE( ( _T( "DISIUserChannel::~DISIUserChannel 0x%x 0x%x iEmptyRxQueueDfc 0x%x" ), this, iObjId, iEmptyRxQueueDfc ) );
        iEmptyRxQueueDfc->Cancel();
        delete iEmptyRxQueueDfc;
        iEmptyRxQueueDfc = NULL;
        }
    if( iCompleteChannelRequestDfc )
        {
        C_TRACE( ( _T( "DISIUserChannel::~DISIUserChannel 0x%x 0x%x iCompleteChannelRequestDfc 0x%x" ), this, iObjId, iCompleteChannelRequestDfc ) );
        iCompleteChannelRequestDfc->Cancel();
        delete iCompleteChannelRequestDfc;
        iCompleteChannelRequestDfc = NULL;
        }
    if( iRx )
        {
        C_TRACE( ( _T( "DISIUserChannel::~DISIUserChannel 0x%x 0x%x iRx 0x%x" ), this, iObjId, iRx ) );
        delete iRx;
        iRx = NULL;
        }
    if( iReceiveBufPtr )
        {
        C_TRACE( ( _T( "DISIUserChannel::~DISIUserChannel 0x%x 0x%x iReceiveBufPtr 0x%x" ), this, iObjId, iReceiveBufPtr ) );
        iReceiveBufPtr = NULL;
        }
    if( iRequests )
        {
        delete iRequests;
        iRequests = NULL;
        }
    // owned ends
    iRouterIf = NULL;
    Kern::SafeClose( reinterpret_cast<DObject*&>(iThread), NULL );    
    C_TRACE( ( _T( "DISIUserChannel::~DISIUserChannel 0x%x<" ), this ) );
    }

/*
* Executed in user thread context thread inside CS, cannot be pre-empted.
* Channel can not be used before creation, so no need to synch if congesting only btw the creating user thread and one thread.
*/    
TInt DISIUserChannel::DoCreate(
        TInt, //aUnit,              // Not used at the moment
        const TDesC8*, //anInfo,    // Not used at the moment.
        const TVersion& // aVer     // Not used at the moment.
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::DoCreate 0x%x 0x%x>" ), this, iObjId ) );
    if( !Kern::CurrentThreadHasCapability( ECapabilityCommDD, __PLATSEC_DIAGNOSTIC_STRING( "Check by: ISI Router" ) ) ) return KErrPermissionDenied;
    iRx = new DISIMsgQueue( KISILddRxQueueSize );
    ASSERT_RESET_ALWAYS( iRx, ( EISIUserChannelMemAllocFail1 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    // Other DFC functions handling user<>kernel copying done in ldd DFC. Ldd DFC function priority is 1.
    iMainThread = iRouterIf->GetDfcThread( MISIChannelRouterIf::EISIUserMainThread );
    SetDfcQ( iMainThread );
    iMsgQ.Receive();
    DObject* thread = reinterpret_cast<DObject*>( iThread );
    // Open is needed to increase ref count to calling thread that is decreased in Kern::SafeClose
    // Possible returns KErrNone ? KErrGeneral
    TInt threadOpen( thread->Open() );
    TRACE_ASSERT_INFO( threadOpen == KErrNone, iObjId );
    C_TRACE( ( _T( "DISIUserChannel::DoCreate 0x%x 0x%x<" ), this, iObjId ) );
    return threadOpen;
    }

void DISIUserChannel::HandleMsg(
        TMessageBase* aMsg
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::HandleMsg 0x%x 0x%x 0x%x>" ), this, aMsg, iObjId ) );
    TThreadMessage& m= *( static_cast< TThreadMessage* >( aMsg ) );
    TInt id( m.iValue );
    if( static_cast<TInt>( ECloseMsg ) == id )
        {
        C_TRACE( ( _T( "DISIUserChannel::HandleMsg ECloseMsg 0x%x 0x%x" ), this, aMsg ) );
        m.Complete( HandleSyncRequest( EISIDisconnect, NULL ), EFalse );
        }
    else if( KMaxTInt == id )
        {
        C_TRACE( ( _T( "DISIUserChannel::HandleMsg cancel 0x%x 0x%x" ), this, aMsg ) );
        DoCancel( id, m.Int0() );
        m.Complete( KErrNone, ETrue );
        }
    else
        {
        ASSERT_RESET_ALWAYS( ( KErrNotFound < id ), ( EISIUserChannelWrongRequest | EDISIUserChannelTraceId << KClassIdentifierShift ) );
        C_TRACE( ( _T( "DISIUserChannel::HandleMsg request 0x%x %d 0x%x" ), this, id, aMsg ) );
        TInt completeValue( KErrNone );
        TInt ulen( KErrNotFound );
        switch ( id )
            {
            case EISIDisconnect:
                {
                ulen = KNoParams;
                break;
                }
            case EISISend:
                {
                ulen = KOneParam;
                break;
                }
            case EISIConnect:
                {
                ulen = KTwoParams;
                break;
                }
            case EISIAsyncReceive:
                {
                ulen = KThreeParams;
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EISIUserChannelWrongRequest1 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
                break;
                }
            }
        TUint32* table[ KThreeParams ];
        completeValue = Kern::ThreadRawRead( iThread, m.Ptr0(), table, ulen * sizeof( TAny* ) );
        
        if( completeValue == KErrNone )
            {
            switch( id )
                {
                // All asynchronous requests.
                case EISIAsyncReceive:
                    {
                    // No need to check return value in async functions, completed to user
                    HandleAsyncRequest( id, table );
                    break;
                    }
                case EISIDisconnect:
                case EISIConnect:
                case EISISend:
                    {
                    completeValue = HandleSyncRequest( id, table );
                    break;
                    }
                default:
                    {
                    ASSERT_RESET_ALWAYS( 0, ( EISIUserChannelWrongRequest2 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
                    break;
                    }
                }
            }
        m.Complete( completeValue, ETrue );
        }
    C_TRACE( ( _T( "DISIUserChannel::HandleMsg 0x%x 0x%x 0x%x<" ), this, aMsg, iObjId ) );
    }

TInt DISIUserChannel::Request(
        TInt aReqNo,
        TAny* a1,
        TAny* //a2
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::Request 0x%x %d 0x%x 0x%x>" ), this, aReqNo, a1, iObjId ) );
    // Programmer errors.
    ASSERT_RESET_ALWAYS( aReqNo >= ( TInt ) EMinRequestId, ( EISIUserChannelWrongRequest3 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aReqNo <= EISILastAsyncRequest || aReqNo == KMaxTInt ), ( EISIUserChannelWrongRequest4 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    // Wrong API usage e.g. called function when interface is not open so panic the client thread.
    TInt result( KErrNotFound );
    // All request go in kernel context and with ::DoControl call.
    TThreadMessage& m=Kern::Message();
    m.iValue = aReqNo;
    m.iArg[ KFirstParam ] = a1;
    m.iArg[ KSecondParam ] = NULL;
    result = m.SendReceive( &iMsgQ );
    C_TRACE( ( _T( "DISIUserChannel::Request 0x%x %d 0x%x<" ), this, aReqNo, a1, iObjId ) );
    return result;
    }

TInt DISIUserChannel::HandleSyncRequest(
        TInt aRequest,
        TAny* a1
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::HandleSyncRequest 0x%x %d 0x%x 0x%x>" ), this, aRequest, a1, iObjId ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIUserChannelfNotThreadContext5 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    TInt error( KErrNotSupported );
    switch( aRequest )
        {
        case EISIDisconnect:
            {
            C_TRACE( ( _T( "DISIUserChannel::HandleSyncRequest 0x%x EISIDisconnect" ), this ) );
            Disconnect();
            error = KErrNone;
            break;
            }
        case EISISend:
            {
            C_TRACE( ( _T( "DISIUserChannel::HandleSyncRequest 0x%x EISISend" ), this ) );
            TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
            TAny* firstParam = reinterpret_cast<TAny*>( tablePtr[ KFirstParam ] );
            TRACE_ASSERT_INFO( firstParam, ( iObjId << KObjIdShift | ( EISIUserChannelNullParam2 | EDISIUserChannelTraceId << KClassIdentifierShift ) ) );
            TInt msgLength( Kern::ThreadGetDesLength( iThread, firstParam ) );
            C_TRACE( ( _T( "DISIUserChannel::HandleSyncRequest 0x%x EISISend 0x%x %d" ), this, firstParam, msgLength ) );
            if( msgLength > 0 && msgLength < KMaxISIMsgSize )
                {
                TDes8& sendBlock = MemApi::AllocBlock( msgLength );
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, firstParam, sendBlock, 0, KChunkShiftBy0 ), ( EISIUserChannelDesReadFailed | EDISIUserChannelTraceId << KClassIdentifierShift ) );
                TRACE_ASSERT_INFO( sendBlock.Length() == msgLength, iObjId << KObjIdShift );
                C_TRACE( ( _T( "DISIUserChannel::HandleAsyncRequest EISISend 0x%x 0x%x" ), this, &sendBlock ) );
                error = iRouterIf->Send( sendBlock, iObjId );
                }
            else
                {
                error = ( msgLength > KMaxISIMsgSize ) ? KErrNoMemory : KErrBadDescriptor;
                TRACE_ASSERT_INFO( 0, iObjId << KObjIdShift | (TUint16)msgLength );
                }
            break;
            }
        case EISIConnect:
            {
            TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
            iUID = tablePtr[ KFirstParam ];
            TUint8* objIdPtr = reinterpret_cast<TUint8*>( tablePtr[ KSecondParam ] );
            C_TRACE( ( _T( "DISIUserChannel::HandleSyncRequest EISIConnect 0x%x 0x%x 0x%x" ), this, iUID, objIdPtr ) );
            iRouterIf->Connect( iUID, iObjId, this );
            C_TRACE( ( _T( "DISIUserChannel::HandleSyncRequest received iObjId is 0x%x" ), iObjId ) );
            ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( iThread, objIdPtr, &iObjId, sizeof(TUint8), iThread ), 
                                   EISIUserChannelDesWriteFailed | iObjId << KObjIdShift | EDISIUserChannelTraceId << KClassIdentifierShift );
            error = KErrNone;
            break;            
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EISIUserChannelWrongRequest5 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
            break;
            }
        }
    C_TRACE( ( _T( "DISIUserChannel::HandleSyncRequest 0x%x %d 0x%x 0x%x<" ), this, aRequest, a1, iObjId ) );
    return error;
    }

void DISIUserChannel::HandleAsyncRequest(
        TInt aRequest,
        TAny* a1
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::HandleAsyncRequest 0x%x %d 0x%x 0x%x>" ), this, aRequest, a1, iObjId ) );
    
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIUserChannelfNotThreadContext4 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
    TRACE_ASSERT_INFO( tablePtr[ KFirstParam ], (TUint16)aRequest );
    TRequestStatus* requestStatus = reinterpret_cast<TRequestStatus*>( tablePtr[ KFirstParam ] );
    if( iRequests->IsPending( aRequest ) )
        {
        C_TRACE( ( _T( "DISIUserChannel::HandleAsyncRequest existing 0x%x request 0x%x" ), this, aRequest ) );
        // Should not give same request object twice before completing the first one.
        ASSERT_RESET_ALWAYS( 0, ( EISIUserChannelSameRequest | iObjId << KObjIdShift | static_cast<TUint8>( aRequest ) << KExtraInfoShift ) );
        }
    else
        {
        C_TRACE( ( _T( "DISIUserChannel::HandleAsyncRequest 0x%x handling %d" ), this, aRequest ) );
        iRequests->SetPending( aRequest, requestStatus );
        switch( aRequest )
            {
            case EISIAsyncReceive:
                {
                iReceiveBufPtr = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                iNeededBufLen = reinterpret_cast<TUint16*>( tablePtr[ KThirdParam ] );
                C_TRACE( ( _T( "DISIUserChannel::HandleAsyncRequest 0x%x EISIAsyncReceive 0x%x" ), this, iReceiveBufPtr ) );
                iEmptyRxQueueDfc->Enque();
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EISIUserChannelWrongRequest2 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DISIUserChannel::HandleAsyncRequest 0x%x %d 0x%x 0x%x<" ), this, aRequest, a1, iObjId ) );
    }
    	

void DISIUserChannel::DoCancel(
        TInt aRequest,
        TInt aMask )
    {
    C_TRACE( ( _T( "DISIUserChannel::DoCancel 0x%x %d 0x%x 0x%x>" ), this, aRequest, aMask, iObjId ) );
    switch( aMask&aRequest )
        {
        case EISIAsyncReceive:
            {
            C_TRACE( ( _T( "DISIUserChannel::DoCancel 0x%x EISIAsyncReceive 0x%x " ), this, iReceiveBufPtr ) );
            iReceiveBufPtr = NULL;
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EISIUserChannelWrongRequest6 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
            break;
            }
        }
    EnqueChannelRequestCompleteDfc( aMask&aRequest, KErrCancel );
    C_TRACE( ( _T( "DISIUserChannel::DoCancel 0x%x %d 0x%x 0x%x<" ), this, aRequest, aMask, iObjId ) );
    }
    	

void DISIUserChannel::Disconnect(
        // None
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::Disconnect 0x%x 0x%x>" ), this, iObjId ) );
    ResetQueues();
    for( TInt i( 0 ); i < iRequests->iRequestCompletedList.Count(); ++i )
        {
        C_TRACE( ( _T( "DISIUserChannel::CancelRequests req to cancel %d" ), i ) );
        DoCancel( KMaxTInt, i );
        }
    iRouterIf->Disconnect( iObjId );
    iRouterIf->FreeDfcThread( iMainThread );
    iRouterIf->FreeDfcThread( iCompletionThread );    
    C_TRACE( ( _T( "DISIUserChannel::Disconnect 0x%x 0x%x<" ), this, iObjId ) );
    }

void DISIUserChannel::ResetQueues(
        // None
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::ResetQueues 0x%x 0x%x>" ), this, iObjId ) );
    if( iRx )
        {
        C_TRACE( ( _T( "DISIUserChannel::ResetQueues 0x%x iRx 0x%x" ), this, iRx ) );
        while( iRx->Count() )
            {
            MemApi::DeallocBlock( iRx->Get() );
            }
        }
    C_TRACE( ( _T( "DISIUserChannel::ResetQueues 0x%x 0x%x<" ), this, iObjId ) );
    }

// This is called in 1...N thread contextes
void DISIUserChannel::ReceiveMsg(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::ReceiveMsg 0x%x 0x%x 0x%x>" ), this, &aMessage, iObjId ) );
    // Can only be called from thread context.
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIUserChannelfNotThreadContext3 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    iRx->Add( aMessage );
    iEmptyRxQueueDfc->Enque();
    C_TRACE( ( _T( "DISIUserChannel::ReceiveMsg 0x%x 0x%x 0x%x<" ), this, &aMessage, iObjId ) );
    }

DISIUserChannel::DISIUserAsyncRequests::DISIUserAsyncRequests(
        const TInt aSize
        )
    {
    C_TRACE( ( _T( "DISIUserAsyncRequests::DISIUserAsyncRequests size %d>" ), aSize ) );
    iRequestLock = new NFastMutex();
    C_TRACE( ( _T( "DISIUserAsyncRequests::DISIUserAsyncRequests<" ) ) );
    }

DISIUserChannel::DISIUserAsyncRequests::~DISIUserAsyncRequests()
    {

    C_TRACE( ( _T( "DISIUserAsyncRequests::~DISIUserAsyncRequests>" ) ) );
    iRequestCompletedList.Close();
    delete iRequestLock;
    C_TRACE( ( _T( "DISIUserAsyncRequests::~DISIUserAsyncRequests<" ) ) );

    }

TBool DISIUserChannel::DISIUserAsyncRequests::IsPending(
        const TUint aRequest
        )
    {
    C_TRACE( ( _T( "DISIUserAsyncRequests::IsPending %d>" ), aRequest ) );
    ASSERT_RESET_ALWAYS( ( EISILastAsyncRequest > aRequest && EISIAsyncReceive <= aRequest ), ( EISIUserChannelReqOverTheLimits | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    TBool ret( EFalse );
    NKern::FMWait( iRequestLock );
    ret = ( iRequestStatusList[ aRequest ] ) ? ETrue : EFalse;
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DISIUserAsyncRequests::IsPending %d %d<" ), aRequest, ret ) );
    return ret;
    }

void DISIUserChannel::DISIUserAsyncRequests::SetPending(
        const TUint aRequest,
        TRequestStatus* aStatus
        )
    {
    C_TRACE( ( _T( "DISIUserAsyncRequests::SetPending %d 0x%x>" ), aRequest, aStatus ) );
    ASSERT_RESET_ALWAYS( aRequest, ( EISIUserChannelNullParam | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( aStatus, ( EISIUserChannelNullParam | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( EISILastAsyncRequest > aRequest && EISIAsyncReceive <= aRequest ), ( EISIUserChannelReqOverTheLimits | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    // Note asserts must be done before holding the lock.
    NKern::FMWait( iRequestLock );
    iRequestStatusList[ aRequest ] = aStatus;
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DISIUserAsyncRequests::SetPending %d 0x%x<" ), aRequest, aStatus ) );
    }

void DISIUserChannel::EmptyRxQueueDfc(
        TAny* aPtr // Pointer to self
        )
    {
   	C_TRACE( ( _T( "DISIUserChannel::EmptyRxQueueDfc>" ) ) );
    DISIUserChannel& chTmp = *reinterpret_cast<DISIUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISIUserChannel::EmptyRxQueueDfc 0x%x 0x%x>" ), &chTmp, chTmp.iObjId ) );
    if( chTmp.iRequests->IsPending( EISIAsyncReceive ) && ( chTmp.iRx->Count() > 0 ) )
        {
        TDes8& tmpDes = chTmp.iRx->Get();
        TInt desMaxLen( Kern::ThreadGetDesMaxLength( chTmp.iThread, chTmp.iReceiveBufPtr ) );
        // If user descriptor legth is enough and length get not failed.
        if( desMaxLen >= tmpDes.Length() )
            {
            // Write to user address space (iReceiveBufPtr) the content of tmpDes starting from zero offset as 8bit descriptor data.
            TInt writeError( Kern::ThreadDesWrite( chTmp.iThread, chTmp.iReceiveBufPtr, tmpDes, KDestStartOffset, KChunkShiftBy0, chTmp.iThread ) );
            if( writeError == KErrNone )
                {
                C_TRACE( ( _T( "DISIUserChannel::EmptyRxQueueDfc write 0x%x k->u 0x%x ok" ), &tmpDes, chTmp.iReceiveBufPtr ) );
                chTmp.EnqueChannelRequestCompleteDfc( EISIAsyncReceive, writeError );
                MemApi::DeallocBlock( tmpDes );
                }
            // Write unsuccesfull don't deallocate the block.
            else
                {
                C_TRACE( ( _T( "DISIUserChannel::EmptyRxQueueDfc write 0x%x k->u 0x%x NOK, reason %d" ), &tmpDes, chTmp.iReceiveBufPtr, writeError ) );
                TRACE_ASSERT_INFO( 0, chTmp.iObjId << KObjIdShift | (TUint16)writeError );
                // Roll the message back to que.
                chTmp.iRx->RollBack( tmpDes );
                chTmp.EnqueChannelRequestCompleteDfc( EISIAsyncReceive, writeError );
                }
            }
        // If descriptor invalid.
        else
            {
            TRACE_WARNING( 0, chTmp.iObjId << KObjIdShift | (TUint16)tmpDes.Length() );
            // Roll the message back to que.
            chTmp.iRx->RollBack( tmpDes );
            // If invalid content.
            if( KErrBadDescriptor == desMaxLen )
                {
                TRACE_ASSERT_INFO( 0, chTmp.iObjId << KObjIdShift | (TUint16)desMaxLen );
                C_TRACE( ( _T( "DISIUserChannel::EmptyRxQueueDfc invalid descriptor 0x%x" ), chTmp.iObjId ) );
                chTmp.EnqueChannelRequestCompleteDfc( EISIAsyncReceive, KErrBadDescriptor );
                }
            // If length too small (if other assert).
            else
                {
                TUint16 neededLength( tmpDes.Length() );
                TRACE_WARNING( 0, chTmp.iObjId << KObjIdShift | (TUint16)desMaxLen );
                C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc descriptor length too small length %d needed %d" ), desMaxLen, neededLength ) );
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( chTmp.iThread, chTmp.iNeededBufLen, &neededLength, sizeof(TUint16), chTmp.iThread ), 
                                        EISIUserChannelDesWriteFailed | chTmp.iObjId << KObjIdShift | EDISIUserChannelTraceId << KClassIdentifierShift );
                chTmp.EnqueChannelRequestCompleteDfc( EISIAsyncReceive, KErrOverflow );
                }
            }
        }
    else
        {
        C_TRACE( ( _T( "DISIUserChannel::EmptyRxQueueDfc no receive active or no message" ) ) );
        }
    C_TRACE( ( _T( "DISIUserChannel::EmptyRxQueueDfc 0x%x<" ), chTmp.iObjId ) );
    }

// called in router ext context
void DISIUserChannel::EnqueChannelRequestCompleteDfc(
        TInt aRequest,
        TInt aStatusToComplete
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::EnqueChannelRequestCompleteDfc 0x%x %d %d 0x%x>" ), this, aRequest, aStatusToComplete, iObjId ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIUserChannelfNotThreadContext | EDISIUserChannelTraceId << KClassIdentifierShift ) );    
    //TODO check the error code
    iRequests->iRequestCompletedList.Append( aRequest ); 
    iRequests->iRequestCompletionValueList[ aRequest ] = aStatusToComplete;
    iCompleteChannelRequestDfc->Enque();
    C_TRACE( ( _T( "DISIUserChannel::EnqueChannelRequestCompleteDfc 0x%x %d %d 0x%x<" ), this, aRequest, aStatusToComplete, iObjId ) );
    }

// Run in ISI user channel kernel thread context.
void DISIUserChannel::CompleteChannelRequestDfc(
        TAny* aPtr
        )
    {
    C_TRACE( ( _T( "DISIUserChannel::CompleteChReqDfc>" ) ) );
    // Make sure that user side is accessed only by ldd DFCThread.
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIUserChannelfNotThreadContext2 | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    DISIUserChannel* chPtr = reinterpret_cast<DISIUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISIUserChannel::CompleteChReqDfc 0x%x 0x%x" ), chPtr, chPtr->iObjId ) );
    TInt request = chPtr->iRequests->iRequestCompletedList[ KFirstRequest ];
    TRequestStatus* status( chPtr->iRequests->iRequestStatusList[ request ] );
    Kern::RequestComplete( chPtr->iThread, status, chPtr->iRequests->iRequestCompletionValueList[ request ] );
    chPtr->iRequests->iRequestCompletedList.Remove( KFirstRequest );
    chPtr->iRequests->iRequestStatusList[ request ] = NULL;
    chPtr->iRequests->iRequestCompletionValueList[ request ] = KNotInitializedStatus;
    
    if( chPtr->iRequests->iRequestCompletedList.Count() != 0 )
        {
        CompleteChannelRequestDfc( chPtr );
        }
    C_TRACE( ( _T( "DISIUserChannel::CompleteChReqDfc 0x%x 0x%x<" ), chPtr, chPtr->iObjId ) );
    }
