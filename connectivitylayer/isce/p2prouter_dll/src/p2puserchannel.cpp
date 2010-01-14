
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
#include "p2puserchannel.h"         // For DP2PUserChannel
#include "msgqueue.h"               // For DMsgQueue 
#include "p2proutertrace.h"         // For C_TRACE, ASSERT_RESET.. and fault codes.
#include "memapi.h"                 // MemApi
#include "p2pdefs.h"                // For EP2PAmountOfProtocols

enum TP2PUserChannelFaults
    {
    EP2PUserChannelMemAllocFail = 0x00,
    EP2PUserChannelMemAllocFail1,
    EP2PUserChannelMemAllocFail2,
    EP2PUserChannelWrongRequest,
    EP2PUserChannelWrongRequest1,
    EP2PUserChannelWrongRequest2,
    EP2PUserChannelWrongRequest3,
    EP2PUserChannelWrongRequest4,
    EP2PUserChannelWrongRequest5,
    EP2PUserChannelWrongRequest6,
    EP2PUserChannelWrongRequest7,
    EP2PUserChannelWrongRequest8,
    EP2PUserChannelWrongParam,
    EP2PUserChannelWrongParam2,
    EP2PUserChannelProtocolIdNotSpecified,
    EP2PUserChannelOverTheArrayLimits,
    EP2PUserChannelDesWriteFailed,
    EP2PUserChannelSameRequest,
    EP2PUserChannelSameRequest2,
    EP2PUserChannelNullParam,
    EP2PUserChannelNullParam2,
    EP2PUserChannelDesReadFailed,
    EP2PIUserChannelfNotThreadContext,
    EP2PIUserChannelfNotThreadContext2,
    EP2PIUserChannelfNotThreadContext3,
    EP2PIUserChannelfNotThreadContext4,
    EP2PIUserChannelfNotThreadContext5,
    EP2PUserChannelMutexCreateFailed,
    EP2PUserChannelMutexWaitFailed,
    EP2PUserChannelMutexWaitFailed2,
    EP2PUserChannelMutexWaitFailed3,
    EP2PUserChannelMutexWaitFailed4,
    EP2PUserChannelReqQueueOutOfSync,
    EP2PUserChannelReqQueueOutOfSync1,
    EP2PUserChannelReqQueueOutOfSync2,
    EP2PUserChannelReqQueueOutOfSync3,
    EP2PUserChannelReqQueueOutOfSync4,
    EP2PUserChannelReqQueueOutOfSync5,
    EP2PUserChannelReqQueueOutOfSync6,
    EP2PUserChannelReqQueueOutOfSync7,
    EP2PUserChannelReqQueueOverTheLimits,
    EP2PUserChannelReqQueueOverTheLimits2,
    EP2PUserChannelReqQueueWrongRequest,
    EP2PUserChannelReqQueueWrongRequest2,
    EP2PUserChannelReqQueueMemoryAllocFailure,
    EP2PUserChannelReqQueueCommon,
    };


// Extracting and adding the pipeheader.
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KNoParams( KErrNone );
const TInt KOneParam( 1 );
const TInt KTwoParams( 2 );
const TInt KThreeParams( 3 );

const TInt KDestStartOffset( 0 );
const TInt KP2PLddEmptyRxPriori( 1 );
const TInt KP2PLddCompletePriori( 1 );

const TInt KP2PMaximumSendSize( 0xffff );

//DMutex* DP2PUserChannel::iShP2PProtocolIdMutex = NULL;
_LIT8( KP2PUserChannelP2PProtocolIdMutex, "P2PUserChannelP2PProtocolIdMutex" );

//
// user<> kernel interaction() done in LDD DFC thread
// 
// kernel<>kernel interaction() done in Extension DFC thread
//
// DEMAND_PAGING
// Receive (write k>u) is done only in LDD thread context to allow Extension thread to continue when dp swaps.
// Send ((write u>k) is not done at the moment in LDD thread context only. Check is it possible to happend (not to be in usable memory after send (unlikely?)).
DP2PUserChannel::DP2PUserChannel(
        // None
        )
    :
    iShP2PProtocolId( EP2PAmountOfProtocols ),
    iReceiveBufPtr( NULL ),
    iRx( NULL ),
    iP2PReqQueue( NULL ),
    iThread( &Kern::CurrentThread() )
    {

    C_TRACE( ( _T( "DP2PUserChannel::DP2PUserChannel>" ) ) );
    iRouterIf = MP2PChRouterIf::GetIf();
    ASSERT_RESET_ALWAYS( iRouterIf, ( EP2PUserChannelMemAllocFail | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    iEmptyRxQueueDfc = new TDfc( EmptyRxQueueDfc, this, iRouterIf->GetDfcThread( MP2PChRouterIf::EP2PLddDfcThread ), KP2PLddEmptyRxPriori );
    iCompleteChannelRequestDfc = new TDfc( CompleteChReqDfc, this, iRouterIf->GetDfcThread( MP2PChRouterIf::EP2PLddDfcThread ), KP2PLddCompletePriori );
    iP2PReqQueue = new DP2PReqQueue();
    ASSERT_RESET_ALWAYS( iEmptyRxQueueDfc && iCompleteChannelRequestDfc && iP2PReqQueue, ( EP2PUserChannelMemAllocFail1 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    TInt err( Kern::MutexCreate( iShP2PProtocolIdMutex, KP2PUserChannelP2PProtocolIdMutex, KMutexOrdGeneral0 ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EP2PUserChannelMutexCreateFailed | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PUserChannel::DP2PUserChannel<" ) ) );

    }

DP2PUserChannel::~DP2PUserChannel(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x>" ), this, iShP2PProtocolId ) );
    // owned starts
    if( iEmptyRxQueueDfc )
        {
        C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x iEmptyRxQueueDfc 0x%x" ), this, iShP2PProtocolId, iEmptyRxQueueDfc ) );
        iEmptyRxQueueDfc->Cancel();
        delete iEmptyRxQueueDfc;
        iEmptyRxQueueDfc = NULL;
        }
    if( iCompleteChannelRequestDfc )
        {
        C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x iCompleteChannelRequestDfc 0x%x" ), this, iShP2PProtocolId, iCompleteChannelRequestDfc ) );
        iCompleteChannelRequestDfc->Cancel();
        delete iCompleteChannelRequestDfc;
        iCompleteChannelRequestDfc = NULL;
        }
    if( iRx )
        {
        C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x iRx 0x%x" ), this, iShP2PProtocolId, iRx ) );
        delete iRx;
        iRx = NULL;
        }
    if( iReceiveBufPtr )
        {
        C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x iReceiveBufPtr 0x%x" ), this, iShP2PProtocolId, iReceiveBufPtr ) );
        iReceiveBufPtr = NULL;
        }
    if( iP2PReqQueue )
       {
       C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x iP2PReqQueue 0x%x" ), this, iShP2PProtocolId, iP2PReqQueue ) );
       delete iP2PReqQueue;
       iP2PReqQueue = NULL;
       }
    // No need to check, if failed reseted already
    C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x iShP2PProtocolIdMutex 0x%x" ), this, iShP2PProtocolId, iShP2PProtocolIdMutex ) );
    iShP2PProtocolIdMutex->Close( NULL );
    // owned ends
    iRouterIf = NULL;
    Kern::SafeClose( reinterpret_cast<DObject*&>(iThread), NULL );
    C_TRACE( ( _T( "DP2PUserChannel::~DP2PUserChannel 0x%x 0x%x<" ), this, iShP2PProtocolId ) );

    }

/*
* Executed in user thread context thread inside CS, cannot be pre-empted.
* Channel can not be used before creation, so no need to synch if congesting only btw the creating user thread and one thread.
*/    
TInt DP2PUserChannel::DoCreate(
        TInt, //aUnit,              // No need to use this, we can avoid the limit of 32 channels
        const TDesC8* anInfo,       // Contains the protocolId
        const TVersion& // aVer     // Not used at the moment.
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::DoCreate 0x%x 0x%x 0x%x>" ), this, iShP2PProtocolId, anInfo ) );
    if( !Kern::CurrentThreadHasCapability( ECapabilityCommDD, __PLATSEC_DIAGNOSTIC_STRING( "Check by: P2PRouter" ) ) ) return KErrPermissionDenied;  
    TRACE_ASSERT_INFO( anInfo, EP2PUserChannelProtocolIdNotSpecified );
    // Check for channel number inside anInfo.
    TRACE_ASSERT_INFO( anInfo->Length() > 0, ( EP2PUserChannelOverTheArrayLimits | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    if( !anInfo || anInfo->Length() == 0 ) return KErrNotSupported;
    TUint8 protocolId = static_cast<TUint8>( ( *anInfo )[ 0 ] );
    TRACE_ASSERT_INFO( ( protocolId < EP2PAmountOfProtocols ), ( EP2PUserChannelWrongParam | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    if( protocolId >= EP2PAmountOfProtocols ) return KErrNotSupported;
    TInt err( Kern::MutexWait( *iShP2PProtocolIdMutex ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EP2PUserChannelMutexWaitFailed | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    // If channel is already used for some protocol.
    if( iShP2PProtocolId != EP2PAmountOfProtocols )
        {
        Kern::MutexSignal( *iShP2PProtocolIdMutex );
        return KErrAlreadyExists;
        }
    iShP2PProtocolId = ~protocolId;
    Kern::MutexSignal( *iShP2PProtocolIdMutex );
    C_TRACE( ( _T( "DP2PUserChannel::DoCreate protocolId 0x%x 0x%x" ), this, iShP2PProtocolId ) );
    iRx = new DMsgQueue( KP2PLddRxQueuSize );
    ASSERT_RESET_ALWAYS( iRx, ( EP2PUserChannelMemAllocFail2 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    // Other DFC functions handling user<>kernel copying done in ldd DFC. Ldd DFC function priority is 1.
    SetDfcQ( iRouterIf->GetDfcThread( MP2PChRouterIf::EP2PDfcThread ) );
    iMsgQ.Receive();
    DObject* thread = reinterpret_cast<DObject*>( iThread );
    // Open is needed to increase ref count to calling thread that is decreased in Kern::SafeClose
    // Possible returns KErrNone ? KErrGeneral
    TInt threadOpen( thread->Open() );
    TRACE_ASSERT_INFO( threadOpen == KErrNone, (TUint8)iShP2PProtocolId );
    C_TRACE( ( _T( "DP2PUserChannel::DoCreate 0x%x 0x%x %d<" ), this, iShP2PProtocolId, threadOpen ) );
    return threadOpen;
    
    }

void DP2PUserChannel::HandleMsg(
        TMessageBase* aMsg
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::HandleMsg 0x%x 0x%x 0x%x>" ), this, iShP2PProtocolId, aMsg ) );
    TThreadMessage& m= *( static_cast< TThreadMessage* >( aMsg ) );
    TInt id( m.iValue );
    if( static_cast<TInt>( ECloseMsg ) == id )
        {
        C_TRACE( ( _T( "DP2PUserChannel::HandleMsg ECloseMsg 0x%x 0x%x 0x%x" ), this, iShP2PProtocolId, aMsg ) );
        m.Complete( HandleSyncRequest( EP2PClose, NULL ), EFalse );
        }
    else if( KMaxTInt == id )
        {
        C_TRACE( ( _T( "DP2PUserChannel::HandleMsg cancel 0x%x 0x%x 0x%x" ), this, iShP2PProtocolId, aMsg ) );
        DoCancel( id, m.Int0() );
        m.Complete( KErrNone, ETrue );
        }
    else
        {
        ASSERT_RESET_ALWAYS( ( KErrNotFound < id ), ( EP2PUserChannelWrongRequest | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
        C_TRACE( ( _T( "DP2PUserChannel::HandleMsg request 0x%x 0x%x %d 0x%x" ), this, iShP2PProtocolId, id, aMsg ) );
        TInt completeValue( KErrNone );
        TInt ulen( KErrNotFound );
        switch ( id )
            {
            case EP2PClose:
                {
                ulen = KNoParams;
                break;
                }
            case EP2PSend:
            case EP2PAsyncConnectionLost:
                {
                ulen = KOneParam;
                break;
                }
            case EP2PAsyncOpen:
            case EP2PAsyncReceive:
                {
                ulen = KTwoParams;
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EP2PUserChannelWrongRequest1 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
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
                case EP2PAsyncOpen:
                case EP2PAsyncReceive:
                case EP2PAsyncConnectionLost:
                    {
                    // No need to check return value in async functions, completed to user
                    HandleAsyncRequest( id, table );
                    break;
                    }
                case EP2PClose:
                case EP2PSend:
                    {
                    completeValue = HandleSyncRequest( id, table );
                    break;
                    }
                default:
                    {
                    ASSERT_RESET_ALWAYS( 0, ( EP2PUserChannelWrongRequest2 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
                    break;
                    }
                }
            }
        m.Complete( completeValue, ETrue );
        }
    C_TRACE( ( _T( "DP2PUserChannel::HandleMsg 0x%x 0x%x 0x%x<" ), this, iShP2PProtocolId, aMsg ) );

    }

TInt DP2PUserChannel::Request(
        TInt aReqNo,
        TAny* a1,
        TAny* //a2
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::Request 0x%x 0x%x 0x%x 0x%x>" ), this, iShP2PProtocolId, aReqNo, a1 ) );
    // Programmer errors.
    ASSERT_RESET_ALWAYS( aReqNo >= ( TInt ) EMinRequestId, ( EP2PUserChannelWrongRequest3 | EDP2PUserChannelTraceId << KClassIdentifierShift | KExtraInfoShift << (TUint8)aReqNo ) );
    ASSERT_RESET_ALWAYS( ( aReqNo <= EP2PLastAsyncRequest || aReqNo == KMaxTInt ), ( EP2PUserChannelWrongRequest4 | EDP2PUserChannelTraceId << KClassIdentifierShift | KExtraInfoShift << (TUint8)aReqNo ) );
    
    if ( iShP2PProtocolId < EP2PAmountOfProtocols )
        {
        // normal activity
        }
    else if ( iShP2PProtocolId > EP2PAmountOfProtocols ) // Open ongoing, not completed 
        {
        ASSERT_PANIC_USER_THREAD_ALWAYS( ( aReqNo == EP2PAsyncOpen                              ||
                                           ( aReqNo == KMaxTInt && (TInt)a1 == EP2PAsyncOpen )   || // cancel open
                                           aReqNo == EP2PClose ),
                                         iThread, ( EP2PUserChannelWrongParam2 | EDP2PUserChannelTraceId << KClassIdentifierShift | KExtraInfoShift << (TUint8)aReqNo ) );
        }
	else
        {
		// Not possible to come here
        ASSERT_RESET_ALWAYS( 0, ( EP2PUserChannelWrongRequest8 | EDP2PUserChannelTraceId << KClassIdentifierShift | KExtraInfoShift << (TUint8)aReqNo ) );
        }

    TInt result( KErrNotFound );
    // All request go in kernel context and with ::DoControl call.
    TThreadMessage& m=Kern::Message();
    m.iValue = aReqNo;
    m.iArg[ KFirstParam ] = a1;
    m.iArg[ KSecondParam ] = NULL;
    result = m.SendReceive( &iMsgQ );
    C_TRACE( ( _T( "DP2PUserChannel::Request 0x%x 0x%x %d 0x%x %d<" ), this, iShP2PProtocolId, aReqNo, a1, result ) );
    return result;

    }

///// Functions from MChannelCallback start (from extension binary)
// called in router ext context
void DP2PUserChannel::ConnectionLost()
    {

    C_TRACE( ( _T( "DP2PKernelChannel::ConnectionLost 0x%x 0x%x %d %d 0x%x>" ), this, iShP2PProtocolId ) );
    EnqueChannelRequestCompleteDfc( EP2PAsyncConnectionLost, KErrNotReady );
    ResetQueues();
    //Closing();
    C_TRACE( ( _T( "DP2PKernelChannel::ConnectionLost 0x%x 0x%x %d %d 0x%x<" ), this, iShP2PProtocolId ) );

    }

// called in router ext context
void DP2PUserChannel::EnqueChannelRequestCompleteDfc(
        TInt aRequest,
        TInt aStatusToComplete
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::EnqueChannelRequestCompleteDfc 0x%x 0x%x %d %d>" ), this, iShP2PProtocolId, aRequest, aStatusToComplete ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PIUserChannelfNotThreadContext | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    TP2PReq req( static_cast<TP2PAsyncRequest>( aRequest ), aStatusToComplete );
    if( iP2PReqQueue->Add( req ) )
        {
        TRACE_ASSERT_INFO( !iCompleteChannelRequestDfc->Queued(), (TUint8)iShP2PProtocolId << KProtocolIdShift | (TUint16)aRequest );
        iCompleteChannelRequestDfc->Enque();
        }
    C_TRACE( ( _T( "DP2PUserChannel::EnqueChannelRequestCompleteDfc 0x%x 0x%x %d %d<" ), this, iShP2PProtocolId, aRequest, aStatusToComplete ) );

    }

// This is called in 1...N thread contextes
void DP2PUserChannel::ReceiveMsg(
        const TDesC8& aMessage
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::ReceiveMsg 0x%x 0x%x 0x%x>" ), this, iShP2PProtocolId, &aMessage ) );
    // Can only be called from thread context.
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PIUserChannelfNotThreadContext3 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    iRx->Add( aMessage );
    iEmptyRxQueueDfc->Enque();
    C_TRACE( ( _T( "DP2PUserChannel::ReceiveMsg 0x%x 0x%x 0x%x<" ), this, iShP2PProtocolId, &aMessage ) );

    }


///// Functions from MChannelCallback end (from extension binary)

// Run in P2P extension kernel thread context.
void DP2PUserChannel::Close(
        const TUint8 aP2PProtocolId
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::Close 0x%x 0x%x 0x%x>" ), this, iShP2PProtocolId, aP2PProtocolId ) );
    if( EP2PAmountOfProtocols != iShP2PProtocolId )
        {
        C_TRACE( ( _T( "DP2PKernelChannel::Close closing 0x%x 0x%x>" ), iShP2PProtocolId, aP2PProtocolId ) );
        iRouterIf->Close( aP2PProtocolId );
        }
    iShP2PProtocolId = EP2PAmountOfProtocols;
    C_TRACE( ( _T( "DP2PUserChannel::Close 0x%x 0x%x 0x%x<" ), this, iShP2PProtocolId, aP2PProtocolId ) );

    }

void DP2PUserChannel::Closing(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::Closing 0x%x 0x%x 0x%x>" ), this, iShP2PProtocolId ) );
    ResetQueues();
    TInt err( Kern::MutexWait( *iShP2PProtocolIdMutex ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EP2PUserChannelMutexWaitFailed2 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    if( EP2PAmountOfProtocols != iShP2PProtocolId )
        {
        // Cancel any outstanding request. // remember in asynch close not to close it asynhronously
        for( TInt i( 0 ); i < EP2PLastAsyncRequest; ++i )
            {
            if( iP2PReqQueue->GetReq( static_cast< TP2PAsyncRequest >( i ) ) )
                {
                C_TRACE( ( _T( "DP2PUserChannel::Closing 0x%x 0x%x EP2PClose req to cancel" ), this, iShP2PProtocolId, i ) );
                DoCancel( KMaxTInt, i );
                }
            }
        Close( iShP2PProtocolId );
        }
    Kern::MutexSignal( *iShP2PProtocolIdMutex );
    C_TRACE( ( _T( "DP2PUserChannel::Closing 0x%x 0x%x 0x%x<" ), this, iShP2PProtocolId ) );

    }

// Run in P2P user channel kernel thread context.
void DP2PUserChannel::CompleteChReqDfc(
        TAny* aPtr
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::CompleteChReqDfc>" ) ) );
    // Make sure that user side is accessed only by ldd DFCThread.
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PIUserChannelfNotThreadContext2 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    //TODO    ASSERT_DFCTHREAD_INLDD();
    DP2PUserChannel* chPtr = reinterpret_cast<DP2PUserChannel*>( aPtr );
    C_TRACE( ( _T( "DP2PUserChannel::CompleteChReqDfc 0x%x 0x%x" ), chPtr, chPtr->iShP2PProtocolId ) );
    TP2PReq requ = chPtr->iP2PReqQueue->Get();
    if( EP2PAsyncOpen == requ.iRequest )
        {
        C_TRACE( ( _T( "DP2PUserChannel::CompleteChReqDfc 0x%x 0x%x status %d" ), chPtr, chPtr->iShP2PProtocolId, requ.iCompleteStatus ) );
        // Check of KErrNone and KErrInUse (same object same id open) and in only those cases ~iCh = ~~iCh kernel already has.
        TInt err( Kern::MutexWait( *chPtr->iShP2PProtocolIdMutex ) );
        ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EP2PUserChannelMutexWaitFailed3 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
        TUint8 chNumber( ( KErrNone == requ.iCompleteStatus || KErrInUse == requ.iCompleteStatus ? ~chPtr->iShP2PProtocolId : EP2PAmountOfProtocols ) );
        chPtr->iShP2PProtocolId = chNumber;
        Kern::MutexSignal( *chPtr->iShP2PProtocolIdMutex );
        }
    TRequestStatus* status( chPtr->iP2PReqQueue->GetReq( requ.iRequest ) );
    Kern::RequestComplete( chPtr->iThread, status, requ.iCompleteStatus );
    C_TRACE( ( _T( "DP2PUserChannel::CompleteChReqDfc 0x%x 0x%x req %d status %d" ), chPtr, chPtr->iShP2PProtocolId, requ.iRequest, requ.iCompleteStatus ) );
    chPtr->iP2PReqQueue->SetReq( requ.iRequest, NULL );
    if( !chPtr->iP2PReqQueue->Empty() )
        {
        CompleteChReqDfc( chPtr );
        }
    C_TRACE( ( _T( "DP2PUserChannel::CompleteChReqDfc 0x%x 0x%x<" ), chPtr, chPtr->iShP2PProtocolId ) );

    }

void DP2PUserChannel::EmptyRxQueueDfc(
        TAny* aPtr // Pointer to self
        )
    {

// TODO    ASSERT_DFCTHREAD_INLDD();
    DP2PUserChannel& chTmp = *reinterpret_cast<DP2PUserChannel*>( aPtr );
    C_TRACE( ( _T( "DP2PUserChannel::EmptyRxQueueDfc 0x%x 0x%x>" ), &chTmp, chTmp.iShP2PProtocolId ) );
    if( ( chTmp.iP2PReqQueue->GetReq( EP2PAsyncReceive ) ) && ( chTmp.iRx->Count() > 0 ) )
        {
        TDes8& tmpDes = chTmp.iRx->Get();
        // Write to user address space (iReceiveBufPtr) the content of tmpDes starting from zero offset as 8bit descriptor data.
        TInt writeError( Kern::ThreadDesWrite( chTmp.iThread, chTmp.iReceiveBufPtr, tmpDes, KDestStartOffset, KChunkShiftBy0, chTmp.iThread ) );
        TRACE_ASSERT_INFO( writeError == KErrNone, ( chTmp.iShP2PProtocolId | writeError << KClassIdentifierShift ) );
        C_TRACE( ( _T( "DP2PUserChannel::EmptyRxQueueDfc writing 0x%x k>u 0x%x 0x%x writeError %d length %d" ), &tmpDes, chTmp.iReceiveBufPtr, chTmp.iShP2PProtocolId, writeError, tmpDes.Length() ) );
        TP2PReq req( static_cast<TP2PAsyncRequest>( EP2PAsyncReceive ), writeError );
        if( chTmp.iP2PReqQueue->Add( req ) )
            {
            TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
            CompleteChReqDfc( &chTmp );
            }
        MemApi::DeallocBlock( tmpDes );
        }
    else
        {
        C_TRACE( ( _T( "DP2PUserChannel::EmptyRxQueueDfc 0x%x 0x%x rx inactive or no message" ), &chTmp, chTmp.iShP2PProtocolId ) );
        }
    C_TRACE( ( _T( "DP2PUserChannel::EmptyRxQueueDfc 0x%x 0x%x<" ), &chTmp, chTmp.iShP2PProtocolId ) );

    }


void DP2PUserChannel::ResetQueues(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::ResetQueues 0x%x 0x%x>" ), this, iShP2PProtocolId ) );
    if( iRx )
        {
        C_TRACE( ( _T( "DP2PUserChannel::ResetQueues 0x%x 0x%x iRx 0x%x" ), this, iShP2PProtocolId, iRx ) );
        while( iRx->Count() )
            {
            MemApi::DeallocBlock( iRx->Get() );
            }
        }
    C_TRACE( ( _T( "DP2PUserChannel::ResetQueues 0x%x 0x%x<" ), this, iShP2PProtocolId ) );

    }

void DP2PUserChannel::HandleAsyncRequest(
        TInt aRequest,
        TAny* a1
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::HandleAsyncRequest 0x%x 0x%x %d>" ), this, iShP2PProtocolId, aRequest ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PIUserChannelfNotThreadContext4 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
    TRACE_ASSERT_INFO( tablePtr[ KFirstParam ], (TUint16)aRequest );
    TRequestStatus* requestStatus = reinterpret_cast<TRequestStatus*>( tablePtr[ KFirstParam ] );
    // If request already active.
    if( ( iP2PReqQueue->GetReq( static_cast<TP2PAsyncRequest>( aRequest ) ) ) )
        {
        // Fault if request is already pending and the request status pointer is different.
        // Fault prints 0-7bits: request, 8-15bits: ch number, 16-31bits: fault identifier
        // Assert cause request already active.
        TRACE_ASSERT_INFO( 0, ( EP2PUserChannelSameRequest | static_cast<TUint8>( iShP2PProtocolId ) << KProtocolIdShift | static_cast<TUint8>( aRequest ) << KExtraInfoShift ) );
        // Active object should not give same request object twice before completing the first one.
        ASSERT_PANIC_USER_THREAD_ALWAYS( iP2PReqQueue->GetReq( static_cast< TP2PAsyncRequest >( aRequest ) ) == requestStatus, iThread,
                                       ( EP2PUserChannelSameRequest2 | static_cast<TUint8>( iShP2PProtocolId ) << KProtocolIdShift | static_cast<TUint8>( aRequest ) << KExtraInfoShift  ) );
        }
    else
        {
        iP2PReqQueue->SetReq( static_cast<TP2PAsyncRequest>( aRequest ), requestStatus );
        switch ( aRequest )
            {
            case EP2PAsyncOpen:
                {
                C_TRACE( ( _T( "DP2PUserChannel::HandleAsyncRequest 0x%x 0x%x EP2PAsyncOpen" ), this, iShP2PProtocolId ) );
                // Set open to pending to router, router completes it when the interconnection to other point is ready.
                iRouterIf->Open( ~iShP2PProtocolId, this );
                break;
                }
            case EP2PAsyncReceive:
                {
                iReceiveBufPtr = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                C_TRACE( ( _T( "DP2PUserChannel::HandleAsyncRequest 0x%x 0x%x EP2PAsyncReceive 0x%x" ), this, iShP2PProtocolId, iReceiveBufPtr ) );
                iEmptyRxQueueDfc->Enque();
                break;
                }
            case EP2PAsyncConnectionLost:
                {
                C_TRACE( ( _T( "DP2PUserChannel::HandleAsyncRequest 0x%x 0x%x EP2PAsyncConnectionLost" ), this, iShP2PProtocolId ) );
                // If the connection is already lost when function is called return immediately.
                // This might happend in between calls to ::Open and ::NotifyClose
                if( !iRouterIf->ConnectionExist( iShP2PProtocolId ) )
                    {
                    EnqueChannelRequestCompleteDfc( EP2PAsyncConnectionLost, KErrNotReady );
                    ResetQueues();
                    //Closing( iP2PProtocolId );
                    }
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EP2PUserChannelWrongRequest5 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DP2PUserChannel::HandleAsyncRequest 0x%x 0x%x %d<" ), this, iShP2PProtocolId, aRequest ) );

    }

TInt DP2PUserChannel::HandleSyncRequest(
        TInt aRequest,
        TAny* a1
        )
    {

    C_TRACE( ( _T( "DP2PUserChannel::HandleSyncRequest 0x%x 0x%x %d>" ), this, iShP2PProtocolId, aRequest ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PIUserChannelfNotThreadContext5 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
    TInt error( KErrNotSupported );
    switch( aRequest )
        {
        case EP2PClose:
            {
            C_TRACE( ( _T( "DP2PUserChannel::HandleSyncRequest 0x%x 0x%x EP2PClose" ), this, iShP2PProtocolId ) );
            Closing();
            error = KErrNone;
            break;
            }
        case EP2PSend:
                {
                C_TRACE( ( _T( "DP2PUserChannel::HandleSyncRequest 0x%x 0x%x EP2PSend" ), this, iShP2PProtocolId ) );
                TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
                TAny* firstParam = reinterpret_cast<TAny*>( tablePtr[ KFirstParam ] );
                TRACE_ASSERT_INFO( firstParam, ( (TUint8)iShP2PProtocolId << KProtocolIdShift | ( EP2PUserChannelNullParam2 | EDP2PUserChannelTraceId << KClassIdentifierShift ) ) );
                TInt msgLength( Kern::ThreadGetDesLength( iThread, firstParam ) );
                C_TRACE( ( _T( "DP2PUserChannel::HandleSyncRequest 0x%x 0x%x EP2PSend 0x%x %d" ), this, iShP2PProtocolId, firstParam, msgLength ) );
                if( msgLength > 0 && msgLength < KP2PMaximumSendSize )
                    {
                    TDes8& sendBlock = MemApi::AllocBlock( msgLength );
                    ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, firstParam, sendBlock, 0, KChunkShiftBy0 ), ( EP2PUserChannelDesReadFailed | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
                    TRACE_ASSERT_INFO( sendBlock.Length() == msgLength, (TUint8)iShP2PProtocolId << KProtocolIdShift );
                    C_TRACE( ( _T( "DP2PUserChannel::HandleAsyncRequest EP2PAsyncSend 0x%x 0x%x 0x%x" ), this, iShP2PProtocolId, &sendBlock ) );
                    error = iRouterIf->Send( sendBlock, iShP2PProtocolId );
                    }
                else
                    {
                    error = ( msgLength > KP2PMaximumSendSize ) ? KErrNoMemory : KErrBadDescriptor;
                    TRACE_ASSERT_INFO( 0, (TUint8)iShP2PProtocolId << KProtocolIdShift | (TUint16)msgLength );
                    }
                break;
                }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EP2PUserChannelWrongRequest6 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
            break;
            }
        }
    C_TRACE( ( _T( "DP2PUserChannel::HandleSyncRequest 0x%x 0x%x %d ret %d<" ), this, iShP2PProtocolId, aRequest, error ) );
    return error;

    }

void DP2PUserChannel::DoCancel(
        TInt aRequest,
        TInt aMask )
    {

    C_TRACE( ( _T( "DP2PUserChannel::DoCancel 0x%x 0x%x>" ), this, iShP2PProtocolId ) );
    switch( aMask&aRequest )
        {
        case EP2PAsyncReceive:
            {
            C_TRACE( ( _T( "DP2PUserChannel::DoCancel 0x%x 0x%x EP2PAsyncReceive 0x%x " ), this, iShP2PProtocolId, iReceiveBufPtr ) );
            iReceiveBufPtr = NULL;
            break;
            }
        case EP2PAsyncOpen:
            {
            // Just complete with cancel
            C_TRACE( ( _T( "DP2PUserChannel::DoCancel 0x%x 0x%x EP2PAsyncOpen" ), this, iShP2PProtocolId ) );
            TInt err( Kern::MutexWait( *iShP2PProtocolIdMutex ) );
            ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EP2PUserChannelMutexWaitFailed4 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
            TUint8 protocolId = ( iShP2PProtocolId > EP2PAmountOfProtocols ) ? ~iShP2PProtocolId : iShP2PProtocolId;
            Close( protocolId );
                
            Kern::MutexSignal( *iShP2PProtocolIdMutex );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EP2PUserChannelWrongRequest7 | EDP2PUserChannelTraceId << KClassIdentifierShift ) );
            break;
            }
        }
    EnqueChannelRequestCompleteDfc( aMask&aRequest, KErrCancel );
    C_TRACE( ( _T( "DP2PUserChannel::DoCancel 0x%x 0x%x<" ), this, iShP2PProtocolId ) );

    }

// Internal class start
DP2PUserChannel::TP2PReq::TP2PReq()
:iRequest( EP2PLastAsyncRequest ), iCompleteStatus( KRequestPending )
    {
    }

DP2PUserChannel::TP2PReq::TP2PReq( TP2PAsyncRequest aReq, TInt aStat  )
:iRequest( aReq), iCompleteStatus( aStat )
{
    ASSERT_RESET_ALWAYS( iRequest != EP2PLastAsyncRequest, ( EP2PUserChannelReqQueueOutOfSync | EDP2PReqQueueId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( iCompleteStatus != KRequestPending, ( EP2PUserChannelReqQueueOutOfSync1 | EDP2PReqQueueId << KClassIdentifierShift ) );
}

DP2PUserChannel::DP2PReqQueue::DP2PReqQueue()
    {
    C_TRACE( ( _T( "DP2PReqQueue::DP2PReqQueue 0x%x %d>" ), this ) );
    iQueueMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iQueueMutex, ( EP2PUserChannelReqQueueMemoryAllocFailure | EDP2PReqQueueId << KClassIdentifierShift ) );
    iSize = EP2PLastAsyncRequest;
    ASSERT_RESET_ALWAYS( ( iSize > 0 ), ( EP2PUserChannelReqQueueOutOfSync2 | EDP2PReqQueueId << KClassIdentifierShift ) );
    iShInputIndex = 0;
    iShOutputIndex = 0;
    iShCount = 0;
    for( TInt i( 0 ); i < iSize; ++i )
        {
        iShReqRingBuffer[ i ].iRequest = EP2PLastAsyncRequest;
        iShReqRingBuffer[ i ].iCompleteStatus = KRequestPending;
        iShReqList[ i ] = NULL;
        }    
    C_TRACE( ( _T( "DP2PReqQueue::DP2PReqQueue 0x%x<" ), this ) );
    }

DP2PUserChannel::DP2PReqQueue::~DP2PReqQueue()
    {
    C_TRACE( ( _T( "DP2PReqQueue::~DP2PReqQueue 0x%x %d>" ), this, iShCount ) );
    // NOTE! This don't deallocate the blocks from the allocated memory just the pointers!
    ASSERT_RESET_ALWAYS( iShCount == 0, ( EP2PUserChannelReqQueueOutOfSync3 | EDP2PReqQueueId << KClassIdentifierShift ) );
    for( TInt i( 0 ); i < iSize; ++i )
        {
        iShReqRingBuffer[ i ].iRequest = EP2PLastAsyncRequest;
        iShReqRingBuffer[ i ].iCompleteStatus = KRequestPending;
        iShReqList[ i ] = NULL;
        }
    iSize = 0;
    iShInputIndex = 0;
    iShOutputIndex = 0;
    iShCount = 0;
    if( iQueueMutex )
        {
        C_TRACE( ( _T( "DP2PReqQueue::~DP2PReqQueue iQueueMutex" ) ) );
        delete iQueueMutex;
        iQueueMutex = NULL;
        }
    C_TRACE( ( _T( "DP2PReqQueue::~DP2PReqQueue 0x%x<" ), this ) );
    }

/* 
* In case of queue overflow throw kern::fault
*/
TBool DP2PUserChannel::DP2PReqQueue::Add( TP2PReq aReq )
    {
    C_TRACE( ( _T( "DP2PReqQueue::Add 0x%x %d %d %d %d>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex ) );
    TBool ok( EFalse );
    NKern::FMWait( iQueueMutex );
    // If queue get's overfilled throw kernel fault.
    ASSERT_RESET_ALWAYS( ( iShCount < iSize ), ( EP2PUserChannelReqQueueOutOfSync4 | EDP2PReqQueueId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( EP2PLastAsyncRequest > aReq.iRequest, ( EP2PUserChannelReqQueueOverTheLimits | EDP2PReqQueueId << KClassIdentifierShift ) );
    if( iShReqList[ aReq.iRequest ] )
        {
        // Place the buffer into the queue.
        iShReqRingBuffer[ iShInputIndex ] = aReq;
        // Adjust input pointer.
        iShInputIndex = ( ( iShInputIndex + 1 ) % iSize );
        // Remember the amount of the requests in the queue.
        iShCount++;
        ok = ETrue;
        }
    NKern::FMSignal( iQueueMutex );
    C_TRACE( ( _T( "DP2PReqQueue::Add 0x%x %d %d %d %d %d<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex, ok ) );
    return ok;
    }

/*
*Returns the first pointer ( iShOutputIndex ) from the ring buffer.
*/
TBool DP2PUserChannel::DP2PReqQueue::Empty()
    {
    C_TRACE( ( _T( "DP2PReqQueue::Empty 0x%x %d<>" ), this, iShCount ) );
    return iShCount == 0 ?  ETrue : EFalse;

    }

/*
*Returns the first pointer ( iShOutputIndex ) from the ring buffer.
*/
DP2PUserChannel::TP2PReq DP2PUserChannel::DP2PReqQueue::Get()
    {
    C_TRACE( ( _T( "DP2PReqQueue::Get 0x%x %d %d %d %d>" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex ) );
    NKern::FMWait( iQueueMutex );
    // If queue is empty.
    ASSERT_RESET_ALWAYS( ( iShCount > 0 ), ( EP2PUserChannelReqQueueOutOfSync5 | EDP2PReqQueueId << KClassIdentifierShift ) );
    // Get the buffer from the queue.
    ASSERT_RESET_ALWAYS( EP2PLastAsyncRequest > iShOutputIndex, ( EP2PUserChannelReqQueueOverTheLimits2 | EDP2PReqQueueId << KClassIdentifierShift ) );
    TP2PReq temp = iShReqRingBuffer[ iShOutputIndex ];
    // Set buffer location to NULL.
    iShReqRingBuffer[ iShOutputIndex ].iRequest = EP2PLastAsyncRequest;
    iShReqRingBuffer[ iShOutputIndex ].iCompleteStatus = KRequestPending;
    // Adjust output pointer.
    iShOutputIndex = ( ( iShOutputIndex + 1 ) % iSize );
    // Remember the amount of the requests in the queue.
    iShCount--;
    NKern::FMSignal( iQueueMutex );
    ASSERT_RESET_ALWAYS( temp.iRequest != EP2PLastAsyncRequest, ( EP2PUserChannelReqQueueOutOfSync6 | EDP2PReqQueueId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( temp.iCompleteStatus != KRequestPending, ( EP2PUserChannelReqQueueOutOfSync7 | EDP2PReqQueueId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DQueue::Get 0x%x %d %d %d %d<" ), this, iSize, iShCount, iShInputIndex, iShOutputIndex ) );
    return temp;
    }

/*
* Set req active / deactive. Default deactive.
*/
void DP2PUserChannel::DP2PReqQueue::SetReq( TP2PAsyncRequest aReqToSet, TRequestStatus* aStatus )
    {
    C_TRACE( ( _T( "DP2PReqQueue::SetReq 0x%x %d 0x%x>" ), this, aReqToSet, aStatus ) );
    ASSERT_RESET_ALWAYS( aReqToSet < EP2PLastAsyncRequest, ( EP2PUserChannelReqQueueWrongRequest | EDP2PReqQueueId << KClassIdentifierShift ) );
    // If setting same request twice.
    C_TRACE( ( _T( "DP2PReqQueue::SetReq 0x%x %d 0x%x 0x%x TBR" ), this, aReqToSet, aStatus, iShReqList[ aReqToSet ] ) );
    ASSERT_RESET_ALWAYS( !( !iShReqList[ aReqToSet ] && aStatus == NULL ), EP2PUserChannelReqQueueCommon );
    iShReqList[ aReqToSet ] = aStatus;
    C_TRACE( ( _T( "DP2PReqQueue::SetReq 0x%x %d 0x%x<" ), this, aReqToSet, aStatus ) );
    }

/*
* Set req active / deactive. Default deactive.
*/
TRequestStatus* DP2PUserChannel::DP2PReqQueue::GetReq( TP2PAsyncRequest aReqToGet )
    {
    ASSERT_RESET_ALWAYS( aReqToGet < EP2PLastAsyncRequest, ( EP2PUserChannelReqQueueWrongRequest2 | EDP2PReqQueueId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PReqQueue::GetReq 0x%x 0x%x %d<>" ), this, iShReqList[ aReqToGet ], aReqToGet ) );
    TRequestStatus* tmpStatus = iShReqList[ aReqToGet ];
    return tmpStatus;
    }
// Internal class end
