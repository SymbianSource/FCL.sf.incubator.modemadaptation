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



#include <kernel.h>             // For Kern

#include "p2prouter.h"          // For DP2PRouter
#include "memapi.h"             // For MemApi
#include "p2proutertrace.h"     // For C_TRACE..
#include "p2pdevice.h"          // For DP2PDevice
#include "mlinkmuxif.h"         // For MLinkMuxIf
#include "trxdefs.h"            // For ETrx...
#include "p2pinternaldefs.h"    // For EP2PAsyncOpen

// Faults
enum TP2PRouterFaults
    {
    EP2PRouterMemAllocFailure = 0x00,
    EP2PRouterMemAllocFailure1,
    EP2PRouterMemAllocFailure2,
    EP2PRouterMemAllocFailure3,
    EP2PRouterMemAllocFailure4,
    EP2PRouterMemAllocFailure5,
    EP2PRouterMemAllocFailure6,
    EP2PRouterMemAllocFailure7,
    EP2PRouterMemAllocFailure8,
    EP2PRouterOverTheLimits,
    EP2PRouterWrongRequest,
    EP2PRouterWrongRequest2,
    EP2PRouterNullParam,
    EP2PRouterWrongParam,
    EP2PRouterWrongParam2,
    EP2PRouterWrongParam3,
    EP2PRouterWrongParam4,
    EP2PRouterWrongParam5,
    EP2PRouterWrongParam6,
    EP2PRouterWrongParam7,
    EP2PRouterWrongParam8,
    };

const TInt KDfcPriority( 5 );
const TInt KDefaultDfcThreadPriority( 27 );

_LIT( KP2PDfcQThreadName, "P2PRouter" );
_LIT( KP2PLddDfcQThreadName, "P2PUserChannel" );
TDfcQue* DP2PRouter::iP2PDfcQueList[ EAmountOfP2PDfcThreads ] = { NULL, NULL };
DP2PRouter* DP2PRouter::iSelfPtr = NULL;

DP2PRouter::DP2PRouter(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PRouter::DP2PRouter>" ) ) );
    TInt err = Kern::DfcQCreate( iP2PDfcQueList[ MP2PChRouterIf::EP2PDfcThread ], KDefaultDfcThreadPriority, &KP2PDfcQThreadName );
    ASSERT_RESET_ALWAYS( iP2PDfcQueList[ MP2PChRouterIf::EP2PDfcThread ], EP2PRouterMemAllocFailure );
    err = Kern::DfcQCreate( iP2PDfcQueList[ MP2PChRouterIf::EP2PLddDfcThread ], KDefaultDfcThreadPriority, &KP2PLddDfcQThreadName );
    ASSERT_RESET_ALWAYS( iP2PDfcQueList[ MP2PChRouterIf::EP2PLddDfcThread ], EP2PRouterMemAllocFailure1 );
    iInitDfc = new TDfc( InitDfc, this, iP2PDfcQueList[ MP2PChRouterIf::EP2PDfcThread ], KDfcPriority );
    ASSERT_RESET_ALWAYS( iInitDfc, EP2PRouterMemAllocFailure2 );
    iInitDfc->Enque();
    C_TRACE( ( _T( "DP2PRouter::DP2PRouter<" ) ) );

    }

DP2PRouter::~DP2PRouter(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PRouter::~DP2PRouter>" ) ) );
    iSelfPtr = NULL;
    delete iShChannelTableFastMutex;
    iShChannelTableFastMutex = NULL;
    // owning so deleting
    for( TInt i( 0 ); i < EP2PAmountOfProtocols; i++ )
        {
        MP2PRouterLinkIf* temp = iLinksArray[ i ];
        temp->Release();
        temp = NULL;
        iLinksArray[ i ] = NULL;
        }
    delete []iLinksArray;
    delete []iP2PDfcQueList;
    iInitDfc->Cancel();
    delete iInitDfc;
    iInitDfc = NULL;
    iTrxPrecentDfc->Cancel();
    delete iTrxPrecentDfc;
    iTrxPrecentDfc = NULL;
    iTrxNotPrecentDfc->Cancel();
    delete iTrxNotPrecentDfc;
    iTrxNotPrecentDfc = NULL;
    C_TRACE( ( _T( "DP2PRouter::~DP2PRouter<" ) ) );

    }

// From MP2PLinkRouterIf start

// Called with FM held, no blocking no nesting, allocation etc.. just enque dfc.
void DP2PRouter::NotifyTrxPresenceEnqueDfc(
        TBool aPresent
        )
    {

    // No traces allowed due to no blocking.
    if( aPresent )
        {
        iTrxPrecentDfc->Enque();
        }
    else
        {
        iTrxNotPrecentDfc->Enque();
        }

    }

// Called in some thread contextes
void DP2PRouter::Receive(
        TDes8& aMsg,
        const TUint8 aProtocolId
        )
    {

    C_TRACE( ( _T( "DP2PRouter::Receive 0x%x %d>" ), &aMsg, aProtocolId ) );
    ASSERT_RESET_ALWAYS( aProtocolId < EP2PAmountOfProtocols, ( EP2PRouterWrongParam | EDP2PRouterTraceId << KClassIdentifierShift ) );
    NKern::FMWait( iShChannelTableFastMutex );
    MP2PRouterChIf* channel = iShChannelTable[ aProtocolId ].iChannel;
    if( channel )
        {
        NKern::FMSignal( iShChannelTableFastMutex );
        // This functions calling thread shall content with channels thread.
        channel->ReceiveMsg( aMsg );
        }
    else
        {
        NKern::FMSignal( iShChannelTableFastMutex );
        // Discard incoming messages without a receiving point channel open.
        TRACE_ASSERT_ALWAYS;
        MemApi::DeallocBlock( aMsg );
        }
    C_TRACE( ( _T( "DP2PRouter::Receive 0x%x %d<" ), &aMsg, aProtocolId ) );

    }

// From MP2PLinkRouterIf end

// From MP2PChRouterIf start
void DP2PRouter::Open(
        const TUint8 aProtocolId,
        MP2PRouterChIf* aCallback
        )
    {

    C_TRACE( ( _T( "DP2PRouter::Open %d>" ), aProtocolId ) );
    // TODO :  assert to check always called in p2p extension thread context
    ASSERT_RESET_ALWAYS( aCallback, EP2PRouterNullParam );
    ASSERT_RESET_ALWAYS( aProtocolId < EP2PAmountOfProtocols, ( EP2PRouterWrongParam2 | EDP2PRouterTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( iLinksArray[ aProtocolId ], ( EP2PRouterWrongParam3 | EDP2PRouterTraceId << KClassIdentifierShift ) );
    if( iLinksArray[ aProtocolId ]->TrxPresent() )
        {
        C_TRACE( ( _T( "DP2PRouter::Open ok %d" ), aProtocolId ) );
        NKern::FMWait( iShChannelTableFastMutex );
        // Channel with the same protocol id is already opened or waiting to complete the opening.
        if( iShChannelTable[ aProtocolId ].iChannel || iShChannelTable[ aProtocolId ].iWaitingChannel )
            {
            NKern::FMSignal( iShChannelTableFastMutex );
            // If another channel tries to open already open channel.
            TRACE_WARNING( iShChannelTable[ aProtocolId ].iChannel == aCallback, (TUint8)aProtocolId );
            aCallback->EnqueChannelRequestCompleteDfc( EP2PAsyncOpen, KErrInUse );//TODO: synch user and kernel APIs return values
            }
        else
            {
            iShChannelTable[ aProtocolId ].iChannel = aCallback;
            NKern::FMSignal( iShChannelTableFastMutex );
            aCallback->EnqueChannelRequestCompleteDfc( EP2PAsyncOpen, KErrNone );
            }
        }
    else
        {
        C_TRACE( ( _T( "DP2PRouter::Open pending %d" ), aProtocolId ) );
        ASSERT_RESET_ALWAYS( !iShChannelTable[ aProtocolId ].iWaitingChannel, EP2PRouterWrongRequest );
        NKern::FMWait( iShChannelTableFastMutex );
        iShChannelTable[ aProtocolId ].iWaitingChannel = aCallback;
        NKern::FMSignal( iShChannelTableFastMutex );
        }
    C_TRACE( ( _T( "DP2PRouter::Open %d<" ), aProtocolId ) );

    }

void DP2PRouter::Close(
        const TUint8 aProtocolId
        )
    {

    C_TRACE( ( _T( "DP2PRouter::Close %d>" ), aProtocolId ) );
    // TODO :  assert to check always called in p2p extension thread context
    ASSERT_RESET_ALWAYS( aProtocolId < EP2PAmountOfProtocols, ( EP2PRouterWrongParam4 | EDP2PRouterTraceId << KClassIdentifierShift ) );
    NKern::FMWait( iShChannelTableFastMutex );
    if( iShChannelTable[ aProtocolId ].iChannel || iShChannelTable[ aProtocolId ].iWaitingChannel )
        {
        iShChannelTable[ aProtocolId ].iChannel = NULL;
        iShChannelTable[ aProtocolId ].iWaitingChannel = NULL;
        }
    NKern::FMSignal( iShChannelTableFastMutex );
    C_TRACE( ( _T( "DP2PRouter::Close<" ) ) );

    }

// Shall be called from P2P ext router thread context only.
TBool DP2PRouter::ConnectionExist(
        const TUint8 aProtocolId
        )
    {

    C_TRACE( ( _T( "DP2PRouter::ConnectionExists %d<>" ), aProtocolId ) );
    ASSERT_RESET_ALWAYS( aProtocolId < EP2PAmountOfProtocols, ( EP2PRouterWrongParam5 | EDP2PRouterTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( iLinksArray[ aProtocolId ], ( EP2PRouterWrongParam6 | EDP2PRouterTraceId << KClassIdentifierShift ) );
    return iLinksArray[ aProtocolId ]->TrxPresent();

    }

TDfcQue* DP2PRouter::GetDfcThread(
        const TP2PDfcThread anIndex
        )
    {

    C_TRACE( ( _T( "DP2PRouter::GetDfcThread<>" ) ) );
    ASSERT_RESET_ALWAYS( anIndex < EAmountOfP2PDfcThreads, EP2PRouterWrongRequest2 );
    ASSERT_RESET_ALWAYS( iP2PDfcQueList[ anIndex ], EP2PRouterMemAllocFailure6 );
    return iP2PDfcQueList[ anIndex ];

    }


MP2PChRouterIf* MP2PChRouterIf::GetIf()
    {

    C_TRACE( ( _T( "MP2PChRouterIf::GetIf<>" ) ) );
    return DP2PRouter::GetRouter();

    }

TInt DP2PRouter::Send(
        TDes8& aMessage,
        const TUint8 aProtocolId
        )
    {

    C_TRACE( ( _T( "DP2PRouter::Send 0x%x>" ), &aMessage ) );
    // TODO :  assert to check always called in p2p extension thread context
    // Inside link array limits, if not programmer error.
    ASSERT_RESET_ALWAYS( aProtocolId < EP2PAmountOfProtocols, ( EP2PRouterWrongParam7 | EDP2PRouterTraceId << KClassIdentifierShift ) );
    MP2PRouterLinkIf* link = iLinksArray[ aProtocolId ];
    ASSERT_RESET_ALWAYS( link, ( EP2PRouterWrongParam8 | EDP2PRouterTraceId << KClassIdentifierShift ) );
    TInt sendResult( KErrNone );
    if( link->TrxPresent() )
        {
        link->Send( aMessage );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        // Discard send block if connection lost
        MemApi::DeallocBlock( aMessage );
        sendResult = KErrNotReady;
        }
    C_TRACE( ( _T( "DP2PRouter::Send 0x%x %d<" ), &aMessage, sendResult ) );
    return sendResult;

    }
// From MP2PChRouterIf end

// privates start
void DP2PRouter::Init(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PRouter::Init>" ) ) );
    iLinksArray = new MP2PRouterLinkIf*[ EP2PAmountOfProtocols ];
    ASSERT_RESET_ALWAYS( iLinksArray, ( EP2PRouterOverTheLimits | EDP2PRouterTraceId << KClassIdentifierShift ) );
    // Initialize links
    for( TInt i( 0 ); i < EP2PAmountOfProtocols; i++ )
        {
        iLinksArray[ i ] = NULL;
        C_TRACE( ( _T( "DP2PRouter::DP2PRouter %d" ), i ) );
        }
    // Initialize channels
    for( TInt i( 0 ); i < EP2PAmountOfProtocols; ++i )
        {
        iShChannelTable[ i ].iChannel = NULL;
        iShChannelTable[ i ].iWaitingChannel = NULL;
        }
    // TODO:do more clever way to create links.
    // Configuration of links.
    iLinksArray[ EP2PRpc ] = MP2PRouterLinkIf::CreateLinkF( this, EP2PRpc, ETrxTest );
    iLinksArray[ EP2PTest ] = MP2PRouterLinkIf::CreateLinkF( this, EP2PTest, ETrxTest );
    iLinksArray[ EP2PTest2 ] = MP2PRouterLinkIf::CreateLinkF( this, EP2PTest2, ETrxTest );
    iTrxPrecentDfc = new TDfc( TrxPrecentDfc, this, iP2PDfcQueList[ MP2PChRouterIf::EP2PDfcThread ], KDfcPriority );
    iTrxNotPrecentDfc = new TDfc( TrxNotPrecentDfc, this, iP2PDfcQueList[ MP2PChRouterIf::EP2PDfcThread ], KDfcPriority );
    iShChannelTableFastMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( ( iTrxNotPrecentDfc && iTrxPrecentDfc && iShChannelTableFastMutex ), EP2PRouterMemAllocFailure4 );
    iSelfPtr = this;
    C_TRACE( ( _T( "DP2PRouter::Init<" ) ) );

    }

void DP2PRouter::InitDfc(
        TAny* aPtr
        )
    {

    C_TRACE( ( _T( "DP2PRouter::InitDfc>" ) ) );
    DP2PRouter* self = reinterpret_cast<DP2PRouter*>( aPtr );
    self->Init();
    C_TRACE( ( _T( "DP2PRouter::InitDfc<" ) ) );

    }

void DP2PRouter::TrxPrecentDfc(
        TAny* aPtr
        )
    {

    C_TRACE( ( _T( "DP2PRouter::TrxPrecentDfc>" ) ) );
    DP2PRouter& self = *reinterpret_cast<DP2PRouter*>( aPtr );
    // Notify all the protocols channel objects of the change of the presence.
    for( TUint8 i( 0 ); i < EP2PAmountOfProtocols; i++ )
        {
        TUint8 protocolId( i );
        C_TRACE( ( _T( "DP2PRouter::TrxPrecentDfc Trx present id=0x%x" ), protocolId ) );
        NKern::FMWait( self.iShChannelTableFastMutex );
        MP2PRouterChIf* waitingChannel = self.iShChannelTable[ protocolId ].iWaitingChannel;
        if( waitingChannel )
            {
            self.iShChannelTable[ protocolId ].iChannel = waitingChannel;
            NKern::FMSignal( self.iShChannelTableFastMutex );
            C_TRACE( ( _T( "DP2PRouter::TrxPrecentDfc channel open waiting %d>" ), protocolId ) );
            waitingChannel->EnqueChannelRequestCompleteDfc( EP2PAsyncOpen, KErrNone );
            }
        else
            {
            // No need to inform presence, when connection is lost it is closed too and must be opened again.
            NKern::FMSignal( self.iShChannelTableFastMutex );
            C_TRACE( ( _T( "DP2PRouter::TrxPrecentDfc nothing waiting %d>" ), protocolId ) );
            }
        }
    C_TRACE( ( _T( "DP2PRouter::TrxPrecentDfc<" )) );

    }

void DP2PRouter::TrxNotPrecentDfc(
        TAny* aPtr
        )
    {

    C_TRACE( ( _T( "DP2PRouter::TrxNotPrecentDfc>" ) ) );
    DP2PRouter& self = *reinterpret_cast<DP2PRouter*>( aPtr );
    // Notify all the protocols channel objects of the change of the presence.
    for( TUint8 i( 0 ); i < EP2PAmountOfProtocols; i++ )
        {
        TUint8 protocolId( i );
        C_TRACE( ( _T( "DP2PRouter::TrxNotPrecentDfc Trx lost id=0x%x" ), protocolId ) );
        // Notifying channel that connection is lost and discard any received messages.
        NKern::FMWait( self.iShChannelTableFastMutex );
        MP2PRouterChIf* channel = self.iShChannelTable[ protocolId ].iChannel;
        if( channel )
            {
            NKern::FMSignal( self.iShChannelTableFastMutex );
            C_TRACE( ( _T( "DP2PRouter::TrxNotPrecentDfc Trx lost channel found for id=0x%x" ), protocolId ) );
            channel->ConnectionLost();
            }
        else
            {
            NKern::FMSignal( self.iShChannelTableFastMutex );
            C_TRACE( ( _T( "DP2PRouter::TrxNotPrecentDfc Trx lost no channel found for id=0x%x" ), protocolId ) );
            }
        }
    C_TRACE( ( _T( "DP2PRouter::TrxNotPrecentDfc<" )) );

    }
// privates end

DECLARE_STANDARD_EXTENSION()
    {

    Kern::Printf( "P2Prouter extension>" );
    // Create a container extension
    DP2PRouter* extension = new DP2PRouter();
    TRACE_ASSERT( extension );
    Kern::Printf( "P2Prouter extension<" );
    return extension ? KErrNone : KErrNoMemory;

    }

DECLARE_EXTENSION_LDD()
    {

    Kern::Printf( "P2Prouter ldd>" );
    DLogicalDevice* device = new DP2PDevice;
    TRACE_ASSERT( device );
    Kern::Printf( "P2Prouter ldd 0x%x<", device );
    return( device );

    }

