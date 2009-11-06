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



#include <nk_priv.h>              // For __ASSERT_NO_FAST_MUTEX
#include <kern_priv.h>            // For __ASSERT_CRITICAL
#include <dfcs.h>                 // For TDfc
#include "isikernelchannel.h"     // For DISIKernelChannel
#include "isiroutertrace.h"       // For C_TRACE, ASSERT_RESET.. and fault codes
#include "misichannelrouterif.h"  // For MISIChannelRouterIf
#include "memapi.h"               // For MemApi
#include "isimsgqueue.h"          // For DISIMsgQueue

// CONST
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );

const TInt KISIKernelChannelMsgQueDfcPrio( 2 ); // LDD FW has one
const TInt KISIKernelEmptyRxQueuePrio( 1 );
const TInt KDestroyChannelMsg( 0xff );

enum TISIKernelChannelFaults
    {
    EISIKernelChannelMemAllocFailure = 0x01,
    EISIKernelChannelMemAllocFailure1,
    EISIKernelChannelNotThreadContext,
    EISIKernelChannelNotThreadContext1,
    EISIKernelChannelNotThreadContext2,
    EISIKernelChannelNullParam,
    EISIKernelChannelNullParam2,
    EISIKernelChannelNullParam3,
    EISIKernelChannelNullParam4,
    EISIKernelChannelNullParam5,
    EISIKernelChannelNullParam6,
    EISIKernelChannelWrongParam,
    EISIKernelChannelWrongRequest,
    EISIKernelChannelWrongRequest1,
    EISIKernelChannelWrongRequest2,
    EISIKernelChannelWrongRequest3,
    EISIKernelChannelOverTheLimits,
    EISIKernelChannelOverTheLimits2,
    EISIKernelChannelOverTheLimits3,
    EISIKernelChannelOverTheLimits4,
    EISIKernelChannelSameRequestTwice,
    EISIKernelChannelDfcAlreadyQueued,
    EISIKernelChannelRxBufferNotReleased,
    EISIKernelChannelAlreadyCreated,
    };


DISIKernelChannel::DISIKernelChannel( 
        const TUint16& aObjId
        ) : iKernelChMsgQue( MsgQDfc, this, NULL, KISIKernelChannelMsgQueDfcPrio )
    {
    C_TRACE( ( _T( "DISIKernelChannel::DISIKernelChannel 0x%x>" ), this ) );
    iRouterIf = MISIChannelRouterIf::GetIf();
    ASSERT_RESET_ALWAYS( iRouterIf, ( EISIKernelChannelMemAllocFailure | EDISIKernelChannelId << KClassIdentifierShift ) );
    iRequests = new DISIKernelAsyncRequests( EISILastAsyncRequest );
    iRx = new DISIMsgQueue( KISILddRxQueueSize );
    iCompletionThread = iRouterIf->GetDfcThread( MISIChannelRouterIf::EISIUserRequestCompletionThread );
    iEmptyRxDfc = new TDfc( EmptyRxDfc, this, iCompletionThread, KISIKernelEmptyRxQueuePrio );
    ASSERT_RESET_ALWAYS( ( iEmptyRxDfc && iRequests && iRx ), ( EISIKernelChannelMemAllocFailure1 | EDISIKernelChannelId << KClassIdentifierShift ) );
    iMainThread = iRouterIf->GetDfcThread( MISIChannelRouterIf::EISIKernelMainThread );
    iKernelChMsgQue.SetDfcQ( iMainThread );
    iKernelChMsgQue.Receive();
    C_TRACE( ( _T( "DISIKernelChannel::DISIKernelChannel 0x%x<" ), this ) );
    }

DISIKernelChannel::~DISIKernelChannel(
        // None
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::~DISIKernelChannel 0x%x 0x%x>" ), this ) );
    // Send channel destroyed message to complete with EFalse.
    TThreadMessage& m=Kern::Message();
    m.iValue = KDestroyChannelMsg;
    m.SendReceive( &iKernelChMsgQue );
    C_TRACE( ( _T( "DISIKernelChannel::~DISIKernelChannel iRx 0x%x" ), iRx ) );
    // Only modified in constructor, if not created already reseted.
    delete iRx;
    iRx = NULL;
    C_TRACE( ( _T( "DISIKernelChannel::~DISIKernelChannel iEmptyRxDfc 0x%x" ), iEmptyRxDfc ) );
    // Only modified in constructor, if not created already reseted.
    iEmptyRxDfc->Cancel();
    delete iEmptyRxDfc;
    iEmptyRxDfc = NULL;
    C_TRACE( ( _T( "DISIKernelChannel::~DISIKernelChannel iRequests 0x%x" ), iEmptyRxDfc ) );
    iPtrPtrToRxBuf = NULL;
    iRouterIf = NULL;
    // Only modified in constructor, if not created already reseted.
    delete iRequests;
    iRequests = NULL;
    C_TRACE( ( _T( "DISIKernelChannel::~DISIKernelChannel 0x%x 0x%x<" ), this ) );
    }

void DISIKernelChannel::MsgQDfc(
        TAny* aPtr
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::MsgQDfc>" ) ) );
    DISIKernelChannel* tmp = reinterpret_cast<DISIKernelChannel*>( aPtr );
    tmp->HandleThreadMsg( static_cast<TThreadMessage&>(*tmp->iKernelChMsgQue.iMessage ) );
    C_TRACE( ( _T( "DISIKernelChannel::MsgQDfc<" ) ) );
    }

TInt DISIKernelChannel::HandleRequest(
        TThreadMessage& aMsg
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::HandleRequest 0x%x 0x%x 0x%x>" ), this, aMsg.iValue, iObjId ) );
    __ASSERT_CRITICAL;
    __ASSERT_NO_FAST_MUTEX;
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIKernelChannelNotThreadContext | EDISIKernelChannelId << KClassIdentifierShift ) );
    TInt valueToReturn( KErrAlreadyExists );
    // Channel is not open.
    if( iObjId == KNotInitializedId )
        {
        switch( aMsg.iValue )
            {
            // Only connect is legal
            case EISIConnect:
                {
                C_TRACE( ( _T( "DISIKernelChannel::HandleRequest connect 0x%x" ), this ) );
                valueToReturn = aMsg.SendReceive( &iKernelChMsgQue );
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EISIKernelChannelWrongParam | EDISIKernelChannelId << KClassIdentifierShift ) );
                break;
                }
            }
        }
    // Channel is open.
    else
        {
        // Accept all calls except new.
        if( EISIConnect != aMsg.iValue )
            {
            valueToReturn = aMsg.SendReceive( &iKernelChMsgQue );
            }
        else
            {
        	  ASSERT_RESET_ALWAYS( ( 0 ), ( EISIKernelChannelAlreadyCreated | EDISIKernelChannelId << KClassIdentifierShift ) );
            }
        }
    C_TRACE( ( _T( "DISIKernelChannel::HandleRequest 0x%x %d 0x%x<" ), this, valueToReturn, iObjId ) );
    return valueToReturn;
    }

// Called only in router extension thread context.
void DISIKernelChannel::EnqueChannelRequestCompleteDfc(
        TInt aRequest,
        TInt aStatusToComplete
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::EnqueChannelRequestCompleteDfc 0x%x %d %d 0x%x 0x%x>" ), this, aRequest, aStatusToComplete, iObjId ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIKernelChannelNotThreadContext2 | EDISIKernelChannelId << KClassIdentifierShift ) );
    iRequests->Complete( aRequest, aStatusToComplete );
    C_TRACE( ( _T( "DISIKernelChannel::EnqueChannelRequestCompleteDfc 0x%x %d %d 0x%x 0x%x<" ), this, aRequest, aStatusToComplete, iObjId ) );
    }

void DISIKernelChannel::HandleThreadMsg(
         TThreadMessage& aMsg
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::HandleThreadMsg 0x%x 0x%x 0x%x>" ), this, &aMsg, iObjId ) );
    TThreadMessage& m = ( aMsg );
    TInt completeValue( KErrNone );
    TBool complete( ETrue );
    switch( m.iValue )
        {
        // All asynchronous requests. Return result after DFC function is run.
        case EISIAsyncReceive:
            {
            // No need to check return value in async functions, completed to client from DFC.
            HandleDfcRequest( m );
            break;
            }
        // From synchronized request return the result immediately
        case EISIConnect:
        case EISIDisconnect:
        case EISIAllocateBlock:
        case EISIDeallocateBlock:
        case EISISend:
            {
            completeValue = HandleSyncRequest( m );
            break;
            }
        case KDestroyChannelMsg:
            {
            completeValue = KErrNone;
            // Don't receive anymore messages.
            complete = EFalse;
            break;
            }
        case KMaxTInt:
            {
            completeValue = KErrNone;
            DoCancel( KMaxTInt, m.Int0() );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EISIKernelChannelWrongRequest | EDISIKernelChannelId << KClassIdentifierShift ) );
            break;
            }
        }
    m.Complete( completeValue, complete );
    C_TRACE( ( _T( "DISIKernelChannel::HandleThreadMsg 0x%x< 0x%x" ), this, iObjId ) );
    }

void DISIKernelChannel::HandleDfcRequest(
        TThreadMessage& aMsg
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::HandleDfcRequest 0x%x 0x%x 0x%x>" ), this, &aMsg, iObjId ) );
    TThreadMessage& m = ( aMsg );
    TInt request( m.iValue );
    ASSERT_RESET_ALWAYS( m.iArg, ( EISIKernelChannelNullParam | EDISIKernelChannelId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( EISILastAsyncRequest > ( request ), ( EISIKernelChannelWrongRequest1 | EDISIKernelChannelId << KClassIdentifierShift ) );
    if( iRequests->IsPending( request) )
        {
        C_TRACE( ( _T( "DISIKernelChannel::HandleDfcRequest existing 0x%x request 0x%x" ), this, request ) );
        // Should not give same request object twice before completing the first one.
        ASSERT_RESET_ALWAYS( 0, ( EISIKernelChannelSameRequestTwice | iObjId << KObjIdShift | static_cast<TUint8>( request ) << KExtraInfoShift ) );
        }
    else
        {
        C_TRACE( ( _T( "DISIKernelChannel::HandleDfcRequest 0x%x handling %d" ), this, request ) );
        TUint32* tablePtr = reinterpret_cast<TUint32*>( m.Ptr0() );
        TRequestStatus* dfcStatus = reinterpret_cast<TRequestStatus*>( tablePtr[ KFirstParam ] );
        TDfc* dfc = reinterpret_cast<TDfc*>( tablePtr[ KThirdParam ] );
        ASSERT_RESET_ALWAYS( dfcStatus, ( EISIKernelChannelNullParam2 | EDISIKernelChannelId << KClassIdentifierShift ) );
        ASSERT_RESET_ALWAYS( dfc, ( EISIKernelChannelNullParam3 | EDISIKernelChannelId << KClassIdentifierShift ) );
        iRequests->SetPending( request, dfc, dfcStatus );
        switch( request )
            {
            case EISIAsyncReceive:
                {
                ASSERT_RESET_ALWAYS( !iPtrPtrToRxBuf, ( EISIKernelChannelRxBufferNotReleased | EDISIKernelChannelId << KClassIdentifierShift ) );
                iPtrPtrToRxBuf = reinterpret_cast<TDes8**>( tablePtr[ KSecondParam ] );
                C_TRACE( ( _T( "DISIKernelChannel::HandleDfcRequest EIADAsyncReceive 0x%x 0x%x 0x%x" ), this, iPtrPtrToRxBuf, &iPtrPtrToRxBuf ) );
                iEmptyRxDfc->Enque();
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EISIKernelChannelWrongRequest2 | EDISIKernelChannelId << KClassIdentifierShift ) );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DISIKernelChannel::HandleDfcRequest 0x%x 0x%x 0x%x<" ), this, &aMsg, iObjId ) );
    }

// Called in 1...N transceivers thread context
void DISIKernelChannel::ReceiveMsg(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::ReceiveMsg 0x%x 0x%x 0x%x>" ), this, &aMessage, iObjId ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIKernelChannelNotThreadContext1 | EDISIKernelChannelId << KClassIdentifierShift ) );
    iRx->Add( aMessage );
    iEmptyRxDfc->Enque();
    C_TRACE( ( _T( "DISIKernelChannel::ReceiveMsg 0x%x 0x%x 0x%x<" ), this, &aMessage, iObjId ) );
    }

// From MISIRouterChannelIf end

DISIKernelChannel::DISIKernelAsyncRequests::DISIKernelAsyncRequests(
        const TInt aSize
        )
    {
    C_TRACE( ( _T( "DISIKernelAsyncRequests::DISIKernelAsyncRequests size %d>" ), aSize ) );
    iRequestLock = new NFastMutex();
    iDfcFunctionList = new TDfc*[ aSize ];
    iRequestStatusList = new TRequestStatus*[ aSize ];
    C_TRACE( ( _T( "DISIKernelAsyncRequests::DISIKernelAsyncRequests<" ) ) );
    }

DISIKernelChannel::DISIKernelAsyncRequests::~DISIKernelAsyncRequests()
    {
    C_TRACE( ( _T( "DISIKernelAsyncRequests::~DISIKernelAsyncRequests>" ) ) );
    // Delete space reserved for the array not the contents of the array, so mem behind pointers that are not owned are not deleted.
    delete iDfcFunctionList;
    delete iRequestStatusList;
    // Deletes lock
    delete iRequestLock;
    C_TRACE( ( _T( "DISIKernelAsyncRequests::~DISIKernelAsyncRequests<" ) ) );
    }

void DISIKernelChannel::DISIKernelAsyncRequests::SetPending(
        const TUint aRequest,
        TDfc* aDfc,
        TRequestStatus* aStatus
        )
    {
    C_TRACE( ( _T( "DISIKernelAsyncRequests::SetPending %d 0x%x 0x%x>" ), aRequest, aDfc, aStatus ) );
    ASSERT_RESET_ALWAYS( aDfc, ( EISIKernelChannelNullParam5 | EDISIKernelChannelId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( aStatus, ( EISIKernelChannelNullParam6 | EDISIKernelChannelId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( EISILastAsyncRequest > aRequest && EISIAsyncReceive <= aRequest ), ( EISIKernelChannelOverTheLimits2 | EDISIKernelChannelId << KClassIdentifierShift ) );
    // Note asserts must be done before holding the lock.
    NKern::FMWait( iRequestLock );
    iDfcFunctionList[ aRequest ] = aDfc;
    iRequestStatusList[ aRequest ] = aStatus;
    *iRequestStatusList[ aRequest ] = KRequestPending;
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DISIKernelAsyncRequests::SetPending %d 0x%x 0x%x<" ), aRequest, aDfc, aStatus ) );
    }

TBool DISIKernelChannel::DISIKernelAsyncRequests::IsPending(
        const TUint aRequest
        )
    {
    C_TRACE( ( _T( "DISIKernelAsyncRequests::IsPending %d>" ), aRequest ) );
    ASSERT_RESET_ALWAYS( ( EISILastAsyncRequest > aRequest && EISIAsyncReceive <= aRequest ), ( EISIKernelChannelOverTheLimits3 | EDISIKernelChannelId << KClassIdentifierShift ) );
    TBool ret( EFalse );
    NKern::FMWait( iRequestLock );
    ret = ( iDfcFunctionList[ aRequest ] && iRequestStatusList[ aRequest ] ) ? ETrue : EFalse;
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DISIKernelAsyncRequests::IsPending %d %d<" ), aRequest, ret ) );
    return ret;
    }

void DISIKernelChannel::DISIKernelAsyncRequests::Complete(
        const TUint aRequest,
        const TInt aStatusToComplete
        )
    {
    C_TRACE( ( _T( "DISIKernelAsyncRequests::Complete %d>" ), aRequest ) );
    // Check that request is legal.
    ASSERT_RESET_ALWAYS( ( EISILastAsyncRequest > aRequest && EISIAsyncReceive <= aRequest ), ( EISIKernelChannelOverTheLimits4 | EDISIKernelChannelId << KClassIdentifierShift ) );
    NKern::FMWait( iRequestLock );
    TDfc* completeDfc = iDfcFunctionList[ aRequest ];
    if( ( completeDfc && iRequestStatusList[ aRequest ] ) )
        {
        // Writing straight to clients pointer. There is a risk that malfunctioning client can mess up it's own pointer, if used out side of rx dfc, but what can you do..
        *iRequestStatusList[ aRequest ] = aStatusToComplete;
        ASSERT_RESET_ALWAYS( !completeDfc->Queued(), ( EISIKernelChannelDfcAlreadyQueued | EDISIKernelChannelId << KClassIdentifierShift ) );
        completeDfc->Enque();
        iDfcFunctionList[ aRequest ] = NULL;
        }
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DISIKernelAsyncRequests::Complete %d<" ), aRequest ) );
    }

void DISIKernelChannel::EmptyRxDfc(
        TAny* aPtr // self
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::EmptyRxDfc>" ) ) );
    DISIKernelChannel& chTmp = *reinterpret_cast<DISIKernelChannel*>( aPtr );
    C_TRACE( ( _T( "DISIKernelChannel::EmptyRxDfc 0x%x 0x%x>" ), &chTmp, chTmp.iObjId ) );
    if( chTmp.iRequests->IsPending( EISIAsyncReceive ) && chTmp.iRx->Count() > 0 )
        {
        C_TRACE( ( _T( "DISIKernelChannel::EmptyRxDfc 0x%x writing to kernel client" ), &chTmp ) );
        TDes8*& tmpWrite = *chTmp.iPtrPtrToRxBuf;
        tmpWrite = &chTmp.iRx->Get();
        C_TRACE( ( _T( "DISIKernelChannel::EmptyRxDfc 0x%x 0x%x 0x%x clientRx 0x%x " ), &chTmp, chTmp.iPtrPtrToRxBuf, &chTmp.iPtrPtrToRxBuf, *chTmp.iPtrPtrToRxBuf ) );
        chTmp.EnqueChannelRequestCompleteDfc( EISIAsyncReceive, KErrNone );
        }
    else
        {
        C_TRACE( ( _T( "DISIKernelChannel::EmptyRxDfc 0x%x no receive active or no message" ), &chTmp ) );
        }
    C_TRACE( ( _T( "DISIKernelChannel::EmptyRxDfc 0x%x 0x%x<" ), &chTmp, chTmp.iObjId ) );
    }

void DISIKernelChannel::DoCancel(
        TInt aRequest,
        TInt aMask )
    {
    C_TRACE( ( _T( "DISIKernelChannel::DoCancel 0x%x 0x%x>" ), this, iObjId ) );
    ASSERT_RESET_ALWAYS( EISILastAsyncRequest > ( aMask&aRequest ), EISIKernelChannelOverTheLimits | EDISIKernelChannelId << KClassIdentifierShift );
    if( iRequests->IsPending( aMask&aRequest ) )
        {
        switch( aMask&aRequest )
            {
            case EISIAsyncReceive:
                {
                C_TRACE( ( _T( "DISIKernelChannel::DoCancel EIADAsyncReceive 0x%x ptrs 0x%x 0x%x" ), this, iPtrPtrToRxBuf, &iPtrPtrToRxBuf ) );
                iPtrPtrToRxBuf = NULL;
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, EISIKernelChannelWrongRequest | EDISIKernelChannelId << KClassIdentifierShift );
                break;
                }
            }
        EnqueChannelRequestCompleteDfc( aMask&aRequest, KErrCancel );
        }
    else
        {
        C_TRACE( ( _T( "DISIKernelChannel::DoCancel nothing to cancel 0x%x" ), this ) );
        }
    C_TRACE( ( _T( "DISIKernelChannel::DoCancel 0x%x 0x%x<" ), this, iObjId ) );
    }

TInt DISIKernelChannel::HandleSyncRequest(
        TThreadMessage& aMsg
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest 0x%x 0x%x 0x%x>" ), this, &aMsg, iObjId ) );
    TThreadMessage& m = ( aMsg );
    TInt request( m.iValue );
    ASSERT_RESET_ALWAYS( m.iArg, ( EISIKernelChannelNullParam4 | EDISIKernelChannelId << KClassIdentifierShift ) );
    TInt returnValue( KErrNone );
    C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest 0x%x handling %d" ), this, request ) );
    TUint32* tablePtr = reinterpret_cast<TUint32*>( m.Ptr0() );
 
    switch( request )
        {
        case EISIConnect:
            {
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIConnect 0x%x" ), this ) );
            iUID = tablePtr[ KFirstParam ];
            TUint8* objIdPtr = reinterpret_cast<TUint8*>( tablePtr[ KSecondParam ] ); //TODO vasta tempobjid
            C_TRACE( ( _T( "DISIKernelChannel::HandleDfcRequest EISINokiaKernelOpen 0x%x" ), this ) );
            iRouterIf->Connect( iUID, iObjId, this );
            *objIdPtr = iObjId;
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIConnect 0x%x" ), this ) );
            break;
            }
        case EISIDisconnect:
            {
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIDisconnect 0x%x" ), this ) );
            Disconnect();
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIDisconnect 0x%x" ), this ) );
            returnValue = KErrNone;
            break;
            }
        case EISIAllocateBlock:
            {
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIAllocateBlock 0x%x" ), this ) );
            const TInt size = *reinterpret_cast<TInt*>( tablePtr[ KFirstParam ] );
            TDes8*& block = *(reinterpret_cast<TDes8**>( tablePtr[ KSecondParam ] ));
            C_TRACE( ( _T( "DISIKernelChannel:: EISIAllocateBlock 0x%x block 0x%x %d" ), this, block, size ) );
            block = ( &MemApi::AllocBlock( size ) );
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIAllocateBlock 0x%x" ), this ) );
            break;
            }
        case EISIDeallocateBlock:
            {
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIDeallocateBlock 0x%x" ), this ) );
            TDes8& block = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIAllocateBlock 0x%x block 0x%x" ), this, &block ) );
            // Needed to ensure that right channel is deleting the right block. (Could it be done otherways too?)
            if( iPtrPtrToRxBuf )
                {
                if ( &block == *iPtrPtrToRxBuf )
                    {
                    C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest release 0x%x 0x%x clientRx 0x%x"), iPtrPtrToRxBuf, &iPtrPtrToRxBuf, *iPtrPtrToRxBuf ) );
                    iPtrPtrToRxBuf = NULL;
                    }
                }
            MemApi::DeallocBlock( block );
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISIDeallocateBlock 0x%x" ), this ) );
            break;
            }
        case EISISend:
            {
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISISend 0x%x" ), this ) );
            TDes8& block = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISISend 0x%x block 0x%x" ), this, &block ) );
            returnValue = iRouterIf->Send( block, iObjId );
            C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest EISISend 0x%x" ), this ) );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EISIKernelChannelWrongRequest3 | EDISIKernelChannelId << KClassIdentifierShift ) );
            break;
            }
        }
    C_TRACE( ( _T( "DISIKernelChannel::HandleSyncRequest 0x%x 0x%x %d 0x%x<" ), this, &aMsg, returnValue, iObjId ) );
    return returnValue;
    }

void DISIKernelChannel::Disconnect(
        // None
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::Disconnect 0x%x 0x%x>" ), this, iObjId ) );
    ResetQueues();
    for( TInt i( EISILastSyncRequest ); i < EISILastAsyncRequest; ++i )
        {
        C_TRACE( ( _T( "DISIKernelChannel::CancelRequests req to cancel %d" ), i ) );
        DoCancel( KMaxTInt, i );
        }
    iRouterIf->Disconnect( iObjId );
    iRouterIf->FreeDfcThread( iMainThread );
    iRouterIf->FreeDfcThread( iCompletionThread );
    C_TRACE( ( _T( "DISIKernelChannel::Disconnect 0x%x 0x%x<" ), this, iObjId ) );
    }

void DISIKernelChannel::ResetQueues(
        // None
        )
    {
    C_TRACE( ( _T( "DISIKernelChannel::ResetQueues 0x%x 0x%x>" ), this, iObjId ) );
    // TODO: assert router ext thread context
    if( iRx )
        {
        C_TRACE( ( _T( "DISIKernelChannel::ResetQueues 0x%x iRx 0x%x" ), this, iRx ) );
        while( iRx->Count() )
            {
            MemApi::DeallocBlock( iRx->Get() );
            }
        }
    C_TRACE( ( _T( "DISIKernelChannel::ResetQueues 0x%x 0x%x<" ), this, iObjId ) );
    }

// End of file.

