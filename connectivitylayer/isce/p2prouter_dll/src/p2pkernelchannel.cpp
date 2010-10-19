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



#include <nk_priv.h>       // For __ASSERT_NO_FAST_MUTEX published partner
#include <kern_priv.h>     // For __ASSERT_CRITICAL published partner
#include <dfcs.h>               // For TDfc
#include "memapi.h"             // For MemApi

#include "p2pkernelchannel.h"   // For DP2PKernelChannel
#include "p2proutertrace.h"     // For TRACEs
#include "p2pdefs.h"            // For EP2PAmountOfProtocols
#include "msgqueue.h"           // For DMsgQueue 
#include "p2pinternaldefs.h"    // For EP2P...

// CONST
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );
const TInt KDestroyChannelMsg( 0xff );
const TInt KP2PKernelChannelMsgQueDfcPrio( 2 ); // LDD FW has one
const TInt KP2PKernelEmptyRxQueuePrio( 1 );

enum TP2PKernelChannelFaults
    {
    EP2PKernelChannelMemAllocFailure = 0x00,
    EP2PKernelChannelMemAllocFailure2,
    EP2PKernelChannelNotThreadContext,
    EP2PKernelChannelNotThreadContext2,
    EP2PKernelChannelNotThreadContext3,
    EP2PKernelChannelNullParam,
    EP2PKernelChannelNullParam2,
    EP2PKernelChannelNullParam3,
    EP2PKernelChannelNullParam4,
    EP2PKernelChannelNullParam5,
    EP2PKernelChannelNullParam6,
    EP2PKernelChannelWrongParam,
    EP2PKernelChannelWrongParam2,
    EP2PKernelChannelWrongParam3,
    EP2PKernelChannelWrongResponse,
    EP2PKernelChannelNullPtr,
    EP2PKernelChannelWrongRequest,
    EP2PKernelChannelWrongRequest2,
    EP2PKernelChannelWrongRequest3,
    EP2PKernelChannelWrongRequest4,
    EP2PKernelChannelOverTheLimits,
    EP2PKernelChannelOverTheLimits2,
    EP2PKernelChannelOverTheLimits3,
    EP2PKernelChannelOverTheLimits4,
    EP2PKernelChannelSameRequestTwice,
    EP2PKernelChannelDfcAlreadyQueued,
    EP2PKernelChannelRxBufferNotReleased,
    };

//  Create a common Kernel channel FW (P2P, ISI, etc..) after APIs are locked

DP2PKernelChannel::DP2PKernelChannel(
        // None
        ) :
        iP2PProtocolId( EP2PAmountOfProtocols ),
        iKernelChMsgQue( MsgQDfc, this, NULL, KP2PKernelChannelMsgQueDfcPrio )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::DP2PKernelChannel 0x%x 0x%x>" ), this, iP2PProtocolId ) );
    iRouterIf = MP2PChRouterIf::GetIf();
    ASSERT_RESET_ALWAYS( iRouterIf, ( EP2PKernelChannelMemAllocFailure | EDP2PKernelChannelId << KClassIdentifierShift ) );
    iRequests = new DP2PKernelAsyncRequests( EP2PLastAsyncRequest );
    iRx = new DMsgQueue( KP2PLddRxQueuSize );
    iEmptyRxDfc = new TDfc( EmptyRxDfc, this, iRouterIf->GetDfcThread( MP2PChRouterIf::EP2PDfcThread ), KP2PKernelEmptyRxQueuePrio );
    ASSERT_RESET_ALWAYS( ( iEmptyRxDfc && iRequests && iRx ), ( EP2PKernelChannelMemAllocFailure2 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    iKernelChMsgQue.SetDfcQ( iRouterIf->GetDfcThread( MP2PChRouterIf::EP2PDfcThread ) );
    iKernelChMsgQue.Receive();
    C_TRACE( ( _T( "DP2PKernelChannel::DP2PKernelChannel 0x%x 0x%x<" ), this, iP2PProtocolId ) );

    }

DP2PKernelChannel::~DP2PKernelChannel(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::~DP2PKernelChannel 0x%x 0x%x>" ), this, iP2PProtocolId ) );
    // Send channel destroyed message to complete with EFalse.
    TThreadMessage& m=Kern::Message();
    m.iValue = KDestroyChannelMsg;
    m.SendReceive( &iKernelChMsgQue );
    iP2PProtocolId = EP2PAmountOfProtocols;
    C_TRACE( ( _T( "DP2PKernelChannel::~DP2PKernelChannel iRx 0x%x" ), iRx ) );
    // Only modified in constructor, if not created already reseted.
    delete iRx;
    iRx = NULL;
    C_TRACE( ( _T( "DP2PKernelChannel::~DP2PKernelChannel iEmptyRxDfc 0x%x" ), iEmptyRxDfc ) );
    // Only modified in constructor, if not created already reseted.
    if (iEmptyRxDfc)
    {	
       iEmptyRxDfc->Cancel();
    } 
    delete iEmptyRxDfc;
    iEmptyRxDfc = NULL;
    iPtrPtrToRxBuf = NULL;
    iRouterIf = NULL;
    C_TRACE( ( _T( "DP2PKernelChannel::~DP2PKernelChannel iRequests 0x%x" ), iEmptyRxDfc ) );
    // Only modified in constructor, if not created already reseted.
    delete iRequests;
    iRequests = NULL;
    C_TRACE( ( _T( "DP2PKernelChannel::~DP2PKernelChannel 0x%x 0x%x<" ), this, iP2PProtocolId ) );

    }

// Handling of the request from kernel api
// Puts calling thread waiting until TThreadMessage::SendReceive is completed
TInt DP2PKernelChannel::HandleRequest(
        TThreadMessage& aMsg
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::HandleRequest 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, aMsg.iValue ) );

    __ASSERT_CRITICAL;      // From kern_priv.h published partner
    __ASSERT_NO_FAST_MUTEX; // From nk_priv.h published partner.
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PKernelChannelNotThreadContext | EDP2PKernelChannelId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PKernelChannel::HandleRequest 0x%x == 0x%x " ), iP2PProtocolId, EP2PAmountOfProtocols ) );
    TInt valueToReturn( KErrAlreadyExists );
    // Channel is not open, either still closed or open is pending.
    if( EP2PAmountOfProtocols <= iP2PProtocolId )
        {
        // Accept only open and close calls
        switch( aMsg.iValue )
            {
            // Open and close calls are legal
            case EP2PAsyncOpen:
            case EP2PClose:
                {
                C_TRACE( ( _T( "DP2PKernelChannel::HandleRequest open or close 0x%x" ), this ) );
                valueToReturn = aMsg.SendReceive( &iKernelChMsgQue );
                break;
                }
            case KMaxTInt:
                {
                // Accept only open cancellation when channel is not open.
                ASSERT_RESET_ALWAYS( ( KMaxTInt & aMsg.Int0() == EP2PAsyncOpen ), ( EP2PKernelChannelWrongParam3 | EDP2PKernelChannelId << KClassIdentifierShift ) );
                C_TRACE( ( _T( "DP2PKernelChannel::HandleRequest open cancel 0x%x" ), this ) );
                valueToReturn = aMsg.SendReceive( &iKernelChMsgQue );
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( ( 0 ), ( EP2PKernelChannelWrongParam | EDP2PKernelChannelId << KClassIdentifierShift ) );
                break;
                }
            }
        }
    // Channel is open.
    else
        {
        // Accept all calls except open.
        if( EP2PAsyncOpen != aMsg.iValue )
            {
            valueToReturn = aMsg.SendReceive( &iKernelChMsgQue );
            }
        }
    C_TRACE( ( _T( "DP2PKernelChannel::HandleRequest 0x%x 0x%x %d<" ), this, iP2PProtocolId, valueToReturn ) );
    return valueToReturn;

    }

// From MP2PRouterChIf start
// Called only in router extension thread context.
void DP2PKernelChannel::ConnectionLost()
    {

    C_TRACE( ( _T( "DP2PKernelChannel::ConnectionLost 0x%x 0x%x %d %d 0x%x>" ), this, iP2PProtocolId ) );
    EnqueChannelRequestCompleteDfc( EP2PAsyncConnectionLost, KErrNotReady );
    ResetQueues();
    //Closing( iP2PProtocolId );
    C_TRACE( ( _T( "DP2PKernelChannel::ConnectionLost 0x%x 0x%x %d %d 0x%x<" ), this, iP2PProtocolId ) );

    }

// Called only in router extension thread context.
void DP2PKernelChannel::EnqueChannelRequestCompleteDfc(
        TInt aRequest,
        TInt aStatusToComplete
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::EnqueChannelRequestCompleteDfc 0x%x 0x%x %d %d 0x%x>" ), this, iP2PProtocolId, aRequest, aStatusToComplete ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PKernelChannelNotThreadContext2 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    //  assert router ext thread context
    if( aRequest == EP2PAsyncOpen )
        {
        iP2PProtocolId = ( KErrNone == aStatusToComplete || KErrInUse == aStatusToComplete ) ? ~iP2PProtocolId : EP2PAmountOfProtocols;
        }
    iRequests->Complete( aRequest, aStatusToComplete );
    C_TRACE( ( _T( "DP2PKernelChannel::EnqueChannelRequestCompleteDfc 0x%x 0x%x %d %d 0x%x<" ), this, iP2PProtocolId, aRequest, aStatusToComplete ) );

    }

// Called in 1...N transceivers thread context
void DP2PKernelChannel::ReceiveMsg(
        const TDesC8& aMessage
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::ReceiveMsg 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, &aMessage ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PKernelChannelNotThreadContext3 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    iRx->Add( aMessage );
    iEmptyRxDfc->Enque();
    C_TRACE( ( _T( "DP2PKernelChannel::ReceiveMsg 0x%x 0x%x 0x%x<" ), this, iP2PProtocolId, &aMessage ) );

    }

// From MP2PRouterChIf end

// Internal functions
void DP2PKernelChannel::EmptyRxDfc(
        TAny* aPtr // self
        )
    {
    
    DP2PKernelChannel& chTmp = *reinterpret_cast<DP2PKernelChannel*>( aPtr );
    C_TRACE( ( _T( "DP2PKernelChannel::EmptyRxDfc 0x%x 0x%x>" ), &chTmp, chTmp.iP2PProtocolId ) );
    if( chTmp.iRequests->IsPending( EP2PAsyncReceive ) && chTmp.iRx->Count() > 0 )
        {
        C_TRACE( ( _T( "DP2PKernelChannel::EmptyRxDfc 0x%x 0x%x writing to kernel client" ), &chTmp, chTmp.iP2PProtocolId ) );
        TDes8*& tmpWrite = *chTmp.iPtrPtrToRxBuf;
        tmpWrite = &chTmp.iRx->Get();
        C_TRACE( ( _T( "DP2PKernelChannel::EmptyRxDfc 0x%x 0x%x 0x%x 0x%x clientRx 0x%x " ), &chTmp, chTmp.iP2PProtocolId, chTmp.iPtrPtrToRxBuf, &chTmp.iPtrPtrToRxBuf, *chTmp.iPtrPtrToRxBuf ) );
        chTmp.EnqueChannelRequestCompleteDfc( EP2PAsyncReceive, KErrNone );
        }
    else
        {
        C_TRACE( ( _T( "DP2PKernelChannel::EmptyRxDfc 0x%x 0x%x no receive active or no message" ), &chTmp, chTmp.iP2PProtocolId ) );
        }
    C_TRACE( ( _T( "DP2PKernelChannel::EmptyRxDfc 0x%x 0x%x <" ), &chTmp, chTmp.iP2PProtocolId ) );

    }

void DP2PKernelChannel::MsgQDfc(
        TAny* aPtr
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::MsgQDfc>" ) ) );
    DP2PKernelChannel* tmp = reinterpret_cast<DP2PKernelChannel*>( aPtr );
    tmp->HandleThreadMsg( static_cast<TThreadMessage&>(*tmp->iKernelChMsgQue.iMessage ) );
    C_TRACE( ( _T( "DP2PKernelChannel::MsgQDfc<" ) ) );

    }

void DP2PKernelChannel::HandleThreadMsg(
         TThreadMessage& aMsg
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::HandleThreadMsg 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, &aMsg ) );
    TThreadMessage& m = ( aMsg );
    TInt completeValue( KErrNone );
    TBool complete( ETrue );
    switch( m.iValue )
        {
        // All asynchronous requests. Return result after DFC function is run.
        case EP2PAsyncReceive:
        case EP2PAsyncOpen:
        case EP2PAsyncConnectionLost:
            {
            // No need to check return value in async functions, completed to client from DFC.
            HandleDfcRequest( m );
            break;
            }
        // From synchronized request return the result immediately
        case EP2PClose:
        case EP2PAllocateBlock:
        case EP2PDeallocateBlock:
        case EP2PSend:
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
            ASSERT_RESET_ALWAYS( 0, ( EP2PKernelChannelWrongRequest | EDP2PKernelChannelId << KClassIdentifierShift ) );
            break;
            }
        }
    m.Complete( completeValue, complete );
    C_TRACE( ( _T( "DP2PKernelChannel::HandleThreadMsg 0x%x 0x%x<" ), this, iP2PProtocolId ) );

    }

void DP2PKernelChannel::HandleDfcRequest(
        TThreadMessage& aMsg
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::HandleDfcRequest 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, &aMsg ) );
    TThreadMessage& m = ( aMsg );
    TInt request( m.iValue );
    ASSERT_RESET_ALWAYS( m.iArg, ( EP2PKernelChannelNullParam | EDP2PKernelChannelId << KClassIdentifierShift ) );
    // If request already active.
    ASSERT_RESET_ALWAYS( EP2PLastAsyncRequest > ( request ), ( EP2PKernelChannelWrongRequest4 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    if( iRequests->IsPending( request) )
        {
        C_TRACE( ( _T( "DP2PKernelChannel::HandleDfcRequest existing 0x%x 0x%x request 0x%x" ), this, iP2PProtocolId, request ) );
        TRACE_ASSERT_INFO( 0, ( (TUint8)iP2PProtocolId << KProtocolIdShift | (TUint8)request << KExtraInfoShift ) );
        // Should not give same request object twice before completing the first one.
        ASSERT_RESET_ALWAYS( 0, ( EP2PKernelChannelSameRequestTwice | static_cast<TUint8>( iP2PProtocolId ) << KProtocolIdShift | static_cast<TUint8>( request ) << KExtraInfoShift ) );
        }
    else
        {
        C_TRACE( ( _T( "DP2PKernelChannel::HandleDfcRequest 0x%x 0x%x handling %d" ), this, iP2PProtocolId, request ) );
        //NOTE! These are tight to parameter passing!!
        TUint32* tablePtr = reinterpret_cast<TUint32*>( m.Ptr0() );
        TInt* dfcStatus = reinterpret_cast<TInt*>( tablePtr[ 0 ] );
        TDfc* dfc = reinterpret_cast<TDfc*>( tablePtr[ 1 ] );
        ASSERT_RESET_ALWAYS( dfcStatus, ( EP2PKernelChannelNullParam2 | EDP2PKernelChannelId << KClassIdentifierShift ) );
        ASSERT_RESET_ALWAYS( dfc, ( EP2PKernelChannelNullParam3 | EDP2PKernelChannelId << KClassIdentifierShift ) );
        iRequests->SetPending( request, dfc, dfcStatus );
        switch( request )
            {
            case EP2PAsyncOpen:
                {
                iP2PProtocolId = tablePtr[ KThirdParam ];
                iP2PProtocolId = ~iP2PProtocolId;
                C_TRACE( ( _T( "DP2PKernelChannel::HandleDfcRequest EP2PNokiaKernelOpen 0x%x 0x%x 0x%x" ), this, iP2PProtocolId, ~iP2PProtocolId ) );
                iRouterIf->Connect( ~iP2PProtocolId, this );
                break;
                }
            case EP2PAsyncReceive:
                {
                ASSERT_RESET_ALWAYS( !iPtrPtrToRxBuf, ( EP2PKernelChannelRxBufferNotReleased | EDP2PKernelChannelId << KClassIdentifierShift ) );
                iPtrPtrToRxBuf = reinterpret_cast<TDes8**>( tablePtr[ KThirdParam ] );
                C_TRACE( ( _T( "DP2PKernelChannel::HandleDfcRequest EIADAsyncReceive 0x%x 0x%x 0x%x 0x%x" ), this, iP2PProtocolId, iPtrPtrToRxBuf, &iPtrPtrToRxBuf ) );
                iEmptyRxDfc->Enque();
                break;
                }
            case EP2PAsyncConnectionLost:
                {
                C_TRACE( ( _T( "DP2PKernelChannel::HandleDfcRequest EP2PAsyncConnectionLost 0x%x 0x%x" ), this, iP2PProtocolId ) );
                // If the connection is already lost when function is called return immediately.
                // This might happend in between calls to ::Open and ::NotifyClose
                if( !iRouterIf->ConnectionExist( iP2PProtocolId ) )
                    {
                    EnqueChannelRequestCompleteDfc( EP2PAsyncConnectionLost, KErrNotReady );
                    ResetQueues();
                    //Closing( iP2PProtocolId );
                    }
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EP2PKernelChannelWrongRequest2 | EDP2PKernelChannelId << KClassIdentifierShift ) );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DP2PKernelChannel::HandleDfcRequest 0x%x 0x%x 0x%x<" ), this, iP2PProtocolId, &aMsg ) );

    }

TInt DP2PKernelChannel::HandleSyncRequest(
        TThreadMessage& aMsg
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::HandleSyncRequest 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, &aMsg ) );
    TThreadMessage& m = ( aMsg );
    TInt request( m.iValue );
    ASSERT_RESET_ALWAYS( m.iArg, ( EP2PKernelChannelNullParam4 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    TInt returnValue( KErrNone );
    C_TRACE( ( _T( "DP2PKernelChannel::HandleSyncRequest 0x%x 0x%x handling %d" ), this, iP2PProtocolId, request ) );
    TUint32* tablePtr = reinterpret_cast<TUint32*>( m.Ptr0() );
    // NOTE! values depend on the P2P Kernel API parameters
    switch( request )
        {
        case EP2PClose:
            {
            C_TRACE( ( _T( "DP2PKernelChannel::HandleSyncRequest EP2PClose 0x%x 0x%x" ), this, iP2PProtocolId ) );
            Closing( iP2PProtocolId );
            C_TRACE( ( _T( "DP2PKernelChannel::HandleSyncRequest EP2PClose 0x%x 0x%x" ), this, iP2PProtocolId ) );
            returnValue = KErrNone;
            break;
            }
        case EP2PAllocateBlock:
            {
            const TInt size = *reinterpret_cast<TInt*>( tablePtr[ KFirstParam ] );
            TDes8*& block = *(reinterpret_cast<TDes8**>( tablePtr[ KSecondParam ] ));
            C_TRACE( ( _T( "DP2PKernelChannel:: EP2PAllocateBlock 0x%x 0x%x block 0x%x %d" ), this, iP2PProtocolId, block, size ) );
            block = ( &MemApi::AllocBlock( size ) );
            break;
            }
        case EP2PDeallocateBlock:
            {
            TDes8& block = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DP2PKernelChannel::HandleSyncRequest EP2PAllocateBlock 0x%x 0x%x block 0x%x" ), this, iP2PProtocolId, &block ) );
            // Needed to ensure that right channel is deleting the right block. (Could it be done otherways too?)
            if( iPtrPtrToRxBuf )
                {
                if ( &block == *iPtrPtrToRxBuf )
                    {
                    C_TRACE( ( _T( "DP2PKernelChannel::HandleSyncRequest release 0x%x 0x%x clientRx 0x%x"), iPtrPtrToRxBuf, &iPtrPtrToRxBuf, *iPtrPtrToRxBuf ) );
                    iPtrPtrToRxBuf = NULL;
                    }
                }
            MemApi::DeallocBlock( block );
            break;
            }
        case EP2PSend:
            {
            TDes8& block = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EP2PSend 0x%x 0x%x block 0x%x" ), this, iP2PProtocolId, &block ) );
            returnValue = iRouterIf->Send( block, iP2PProtocolId );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EP2PKernelChannelWrongRequest3 | EDP2PKernelChannelId << KClassIdentifierShift ) );
            break;
            }
        }
    C_TRACE( ( _T( "DP2PKernelChannel::HandleSyncRequest 0x%x 0x%x 0x%x %d<" ), this, iP2PProtocolId, &aMsg, returnValue ) );
    return returnValue;

    }

void DP2PKernelChannel::DoCancel(
        TInt aRequest,
        TInt aMask )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::DoCancel 0x%x 0x%x>" ), this, iP2PProtocolId ) );
    ASSERT_RESET_ALWAYS( EP2PLastAsyncRequest > ( aMask&aRequest ), EP2PKernelChannelOverTheLimits | EDP2PKernelChannelId << KClassIdentifierShift );
    if( iRequests->IsPending( aMask&aRequest ) )
        {
        switch( aMask&aRequest )
            {
            case EP2PAsyncOpen:
                {
                C_TRACE( ( _T( "DP2PKernelChannel::DoCancel EP2PAsyncOpen 0x%x 0x%x" ), this, iP2PProtocolId ) );
                ResetQueues();
                Close( ~iP2PProtocolId );
                break;
                }
            case EP2PAsyncReceive:
                {
                C_TRACE( ( _T( "DP2PKernelChannel::DoCancel EIADAsyncReceive 0x%x 0x%x ptrs 0x%x 0x%x" ), this, iP2PProtocolId, iPtrPtrToRxBuf, &iPtrPtrToRxBuf ) );
                iPtrPtrToRxBuf = NULL;
                break;
                }
            case EP2PAsyncConnectionLost:
                {
                C_TRACE( ( _T( "DP2PKernelChannel::DoCancel EP2PAsyncConnectionLost 0x%x 0x%x" ), this, iP2PProtocolId ) );
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, EP2PKernelChannelWrongRequest | EDP2PKernelChannelId << KClassIdentifierShift );
                break;
                }
            }
        EnqueChannelRequestCompleteDfc( aMask&aRequest, KErrCancel );
        }
    else
        {
        C_TRACE( ( _T( "DP2PKernelChannel::DoCancel nothing to cancel 0x%x 0x%x" ), this, iP2PProtocolId ) );
        }
    C_TRACE( ( _T( "DP2PKernelChannel::DoCancel 0x%x 0x%x<" ), this, iP2PProtocolId ) );

    }

void DP2PKernelChannel::Close(
        const TUint8 aP2PProtocolId
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::Close 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, aP2PProtocolId ) );
    if( EP2PAmountOfProtocols != iP2PProtocolId )
        {
        C_TRACE( ( _T( "DP2PKernelChannel::Close closing 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, aP2PProtocolId ) );
        iRouterIf->Close( aP2PProtocolId );
        }
    iP2PProtocolId = EP2PAmountOfProtocols;
    C_TRACE( ( _T( "DP2PKernelChannel::Close 0x%x 0x%x 0x%x<" ), this, iP2PProtocolId, aP2PProtocolId ) );

    }

void DP2PKernelChannel::Closing(
        const TUint8 aP2PProtocolId
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::Closing 0x%x 0x%x 0x%x>" ), this, iP2PProtocolId, aP2PProtocolId ) );
    ResetQueues();
    for( TInt i( EP2PLastSyncRequest ); i < EP2PLastAsyncRequest; ++i )
        {
        C_TRACE( ( _T( "DP2PKernelChannel::CancelRequests req to cancel %d" ), i ) );
        DoCancel( KMaxTInt, i );
        }
    Close( iP2PProtocolId );
    C_TRACE( ( _T( "DP2PKernelChannel::Closing 0x%x 0x%x 0x%x<" ), this, iP2PProtocolId, aP2PProtocolId ) );

    }

void DP2PKernelChannel::ResetQueues(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PKernelChannel::ResetQueues 0x%x 0x%x>" ), this, iP2PProtocolId ) );
    //  assert router ext thread context
    if( iRx )
        {
        C_TRACE( ( _T( "DP2PKernelChannel::ResetQueues 0x%x 0x%x iRx 0x%x" ), this, iP2PProtocolId, iRx ) );
        while( iRx->Count() )
            {
            MemApi::DeallocBlock( iRx->Get() );
            }
        }
    C_TRACE( ( _T( "DP2PKernelChannel::ResetQueues 0x%x 0x%x<" ), this, iP2PProtocolId ) );

    }
// Internal functions

// Internal class

DP2PKernelChannel::DP2PKernelAsyncRequests::DP2PKernelAsyncRequests(
        const TInt aSize
        )
    {

    C_TRACE( ( _T( "DP2PKernelAsyncRequests::DP2PKernelAsyncRequests size %d>" ), aSize ) );
    iRequestLock = new NFastMutex();
    iShDfcFunctionList = new TDfc*[ aSize ];
    iShRequestStatusList = new TInt*[ aSize ];
    C_TRACE( ( _T( "DP2PKernelAsyncRequests::DP2PKernelAsyncRequests<" ) ) );

    }


DP2PKernelChannel::DP2PKernelAsyncRequests::~DP2PKernelAsyncRequests()
    {

    C_TRACE( ( _T( "DP2PKernelAsyncRequests::~DP2PKernelAsyncRequests>" ) ) );
    // Delete space reserved for the array not the contents of the array, so mem behind pointers that are not owned are not deleted.
    delete iShDfcFunctionList;
    delete iShRequestStatusList;
    // Deletes lock
    delete iRequestLock;
    C_TRACE( ( _T( "DP2PKernelAsyncRequests::~DP2PKernelAsyncRequests<" ) ) );

    }

void DP2PKernelChannel::DP2PKernelAsyncRequests::SetPending(
        const TUint aRequest,
        TDfc* aDfc,
        TInt* aStatus
        )
    {

    C_TRACE( ( _T( "DP2PKernelAsyncRequests::SetPending %d 0x%x 0x%x>" ), aRequest, aDfc, aStatus ) );
    ASSERT_RESET_ALWAYS( aDfc, ( EP2PKernelChannelNullParam5 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( aStatus, ( EP2PKernelChannelNullParam5 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( EP2PLastAsyncRequest > aRequest && EP2PAsyncReceive <= aRequest ), ( EP2PKernelChannelOverTheLimits2 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    // Note asserts must be done before holding the lock.
    NKern::FMWait( iRequestLock );
    iShDfcFunctionList[ aRequest ] = aDfc;
    iShRequestStatusList[ aRequest ] = aStatus;
    *iShRequestStatusList[ aRequest ] = KRequestPending;
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DP2PKernelAsyncRequests::SetPending %d 0x%x 0x%x<" ), aRequest, aDfc, aStatus ) );

    }

TBool DP2PKernelChannel::DP2PKernelAsyncRequests::IsPending(
        const TUint aRequest
        )
    {

    C_TRACE( ( _T( "DP2PKernelAsyncRequests::IsPending %d>" ), aRequest ) );
    ASSERT_RESET_ALWAYS( ( EP2PLastAsyncRequest > aRequest && EP2PAsyncReceive <= aRequest ), ( EP2PKernelChannelOverTheLimits3 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    TBool ret( EFalse );
    NKern::FMWait( iRequestLock );
    ret = ( iShDfcFunctionList[ aRequest ] && iShRequestStatusList[ aRequest ] ) ? ETrue : EFalse;
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DP2PKernelAsyncRequests::IsPending %d %d<" ), aRequest, ret ) );
    return ret;

    }

void DP2PKernelChannel::DP2PKernelAsyncRequests::Complete(
        const TUint aRequest,
        const TInt aStatusToComplete
        )
    {

    C_TRACE( ( _T( "DP2PKernelAsyncRequests::Complete %d>" ), aRequest ) );
    // Check that request is legal.
    ASSERT_RESET_ALWAYS( ( EP2PLastAsyncRequest > aRequest && EP2PAsyncReceive <= aRequest ), ( EP2PKernelChannelOverTheLimits4 | EDP2PKernelChannelId << KClassIdentifierShift ) );
    NKern::FMWait( iRequestLock );
    TDfc* completeDfc = iShDfcFunctionList[ aRequest ];
    if( ( completeDfc && iShRequestStatusList[ aRequest ] ) )
        {
        // Writing straight to clients pointer. There is a risk that malfunctioning client can mess up it's own pointer, if used out side of rx dfc, but what can you do..
        *iShRequestStatusList[ aRequest ] = aStatusToComplete;
        ASSERT_RESET_ALWAYS( !completeDfc->Queued(), ( EP2PKernelChannelDfcAlreadyQueued | EDP2PKernelChannelId << KClassIdentifierShift ) );
        completeDfc->Enque();
        iShDfcFunctionList[ aRequest ] = NULL;
        }
    NKern::FMSignal( iRequestLock );
    C_TRACE( ( _T( "DP2PKernelAsyncRequests::Complete %d<" ), aRequest ) );

    }

// Internal class

// End of file.

