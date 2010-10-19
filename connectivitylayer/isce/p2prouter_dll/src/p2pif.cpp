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



#include <kern_priv.h>              // For __ASSERT_CRITICAL published partner
#include "p2pif.h"                  // For DP2PIf
#include "p2proutertrace.h"         // For ASSERT..
#include "p2pinternaldefs.h"        // For EP2PAsyncOpen etc..
#include "p2pkernelchannel.h"       // For DP2PKernelChannel

// Constants
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );
const TInt KOneParam( 1 );
const TInt KTwoParams( 2 );
const TInt KThreeParams( 3 );

// Faults
enum TP2PIfFaults
    {
    EP2PIfMemAllocFailure = 0x00,
    EP2PIfMemAllocFailure2,
    EP2PIfNotThreadContext,
    EP2PIfNotThreadContext2,
    EP2PIfNullParam,
    EP2PIfNullPtr,
    EP2PNotNullPtr,
    EP2PIfWrongResponseToRequest,
    EP2PIfWrongResponseToRequest2,
    EP2PIfWrongResponseToRequest3,
    EP2PIfWrongResponseToRequest4,
    EP2PIfWrongResponseToRequest5,
    EP2PIfWrongResponseToRequest6,
    EP2PIfWrongResponseToRequest7,
    EP2PIfWrongResponseToRequest8,
    EP2PIfWrongResponseToRequest9,
    };

EXPORT_C MP2PIf* TP2PFactory::NewP2PIfF()
    {

    C_TRACE((_T("TP2PFactory::NewP2PIfF<>")));
    return DP2PIf::NewF();

    }

DP2PIf* DP2PIf::NewF()
    {

    C_TRACE((_T("DP2PIf::NewF>") ));
    DP2PIf* tmp = new DP2PIf();
    ASSERT_RESET_ALWAYS( tmp, ( EP2PIfMemAllocFailure | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE((_T("DP2PIf::NewF 0x%x<"), tmp ));
    return tmp;

    }

DP2PIf::DP2PIf(
        // None
        )
    {

    C_TRACE((_T("DP2PIf::DP2PIf 0x%x>"), this ));
    __ASSERT_CRITICAL;// From kern_priv.h published partner
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PIfNotThreadContext | EDP2PIfTraceId << KClassIdentifierShift ) );
    iKernelChannel = new DP2PKernelChannel();
    ASSERT_RESET_ALWAYS( iKernelChannel, ( EP2PIfMemAllocFailure2 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE((_T("DP2PIf::DP2PIf 0x%x<"), this ));

    }

DP2PIf::~DP2PIf(
        // None
        )
    {

    C_TRACE((_T("DP2PIf::~DP2PIf 0x%x>"), this ));
    Close();
    if( iKernelChannel )
        {
        delete iKernelChannel;
        iKernelChannel = NULL;
        }
    C_TRACE((_T("DP2PIf::~DP2PIf 0x%x<"), this ));

    }

TDes8& DP2PIf::AllocateBlock(
        const TUint16 aSize
        )
    {

    C_TRACE( ( _T( "DP2PIf::AllocateBlock 0x%x %d>" ), this, aSize ) );
    TDes8* temp = NULL;
    TInt size( aSize );
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &size );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &temp );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EP2PAllocateBlock;
    m.iArg[ KFirstParam ] = params;
    // Other asserts in kernel channel.
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest4 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::AllocateBlock 0x%x %d<" ), this, aSize ) );
    return *temp;

    }

void DP2PIf::Close(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PIf::Close 0x%x>" ), this ) );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EP2PClose;
    m.iArg[ KFirstParam ] = NULL;
    // Other asserts in kernel channel.
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::Close 0x%x<" ), this ) );

    }

void DP2PIf::ConnectionLost(
        TInt& aStatus,
        const TDfc& aConnectionResetedCompletedDfc
        )
    {

    C_TRACE( ( _T( "DP2PIf::ConnectionLost 0x%x>" ), this, &aStatus, &aConnectionResetedCompletedDfc ) );
    aStatus = KRequestPending;
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &const_cast<TDfc&>( aConnectionResetedCompletedDfc ) );
    TThreadMessage& m = Kern::Message();
    m.iValue = EP2PAsyncConnectionLost;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest8 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::ConnectionLost 0x%x<" ), this, &aStatus, &aConnectionResetedCompletedDfc ) );

    }

void DP2PIf::ConnectionLostCancel(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PIf::ConnectionLostCancel 0x%x>" ), this ) );
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EP2PAsyncConnectionLost );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest9 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::ConnectionLostCancel 0x%x<" ), this ) );

    }

void DP2PIf::DeallocateBlock(
        TDes8& aDataBlock
        )
    {

    C_TRACE( ( _T( "DP2PIf::DeallocateBlock 0x%x b 0x%x>" ), this, &aDataBlock ) );
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aDataBlock );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EP2PDeallocateBlock;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest5 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::DeallocateBlock 0x%x b 0x%x<" ), this, &aDataBlock ) );

    }

void DP2PIf::Open(
        const TP2PProtocol aP2PProtocolId,
        TInt& aDfcStatus,
        TDfc& aOpenCompleteDfc
        )
    {

    C_TRACE( ( _T( "DP2PIf::Open 0x%x 0x%x 0x%x %d 0x%x>" ), this, aP2PProtocolId, &aDfcStatus, aDfcStatus, &aOpenCompleteDfc ) );
    TAny* params[ KThreeParams ];
    aDfcStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aDfcStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aOpenCompleteDfc );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( aP2PProtocolId );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EP2PAsyncOpen;
    m.iArg[ KFirstParam ] = params;
    // Other asserts in kernel channel.
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest2 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::Open 0x%x 0x%x 0x%x %d 0x%x<" ), this, aP2PProtocolId, &aDfcStatus, aDfcStatus, &aOpenCompleteDfc ) );

    }

void DP2PIf::OpenCancel(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PIf::OpenCancel 0x%x>"), this ) );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EP2PAsyncOpen );
    // Other asserts in kernel channel.
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest3 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::OpenCancel 0x%x<"), this ) );

    }

void DP2PIf::Receive(
        TDes8*& aRxDataBlock,
        TInt& aStatus,
        const TDfc& aReceiveCompletedDfc
        )
    {

    C_TRACE( ( _T( "DP2PIf::Receive 0x%x 0x%x 0x%x 0x%x>"), this, aRxDataBlock, &aStatus, &aReceiveCompletedDfc ) );
    ASSERT_RESET_ALWAYS( !aRxDataBlock, ( EP2PNotNullPtr | EDP2PIfTraceId << KClassIdentifierShift ) );
    aStatus = KRequestPending;
    TAny* params[ KThreeParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &const_cast<TDfc&>( aReceiveCompletedDfc ) );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( &aRxDataBlock );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EP2PAsyncReceive;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest6 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::Receive 0x%x 0x%x 0x%x 0x%x<"), this, aRxDataBlock, &aStatus, &aReceiveCompletedDfc ) );

    }

void DP2PIf::ReceiveCancel()
    {

    C_TRACE( ( _T( "DP2PIf::ReceiveCancel 0x%x>" ), this ) );
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EP2PAsyncReceive );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EP2PIfWrongResponseToRequest7 | EDP2PIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PIf::ReceiveCancel 0x%x<" ), this ) );

    }

void DP2PIf::Release()
    {

    C_TRACE( ( _T( "DP2PIf::Release 0x%x>" ), this ) );
    __ASSERT_CRITICAL;// From kern_priv.h published partner
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EP2PIfNotThreadContext2 | EDP2PIfTraceId << KClassIdentifierShift ) );
    delete this;
    C_TRACE( ( _T( "DP2PIf::Release 0x%x<" ), this ) );

    }

TInt DP2PIf::Send(
        TDes8& aData
        )
    {

    C_TRACE( ( _T( "DP2PIf::Send 0x%x b 0x%x>"), this, &aData ) );
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aData );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EP2PSend;
    m.iArg[ KFirstParam ] = params;
    TInt retValue( iKernelChannel->HandleRequest( m ) );
    C_TRACE( ( _T( "DP2PIf::Send 0x%x b 0x%x %d<"), this, &aData, retValue ) );
    return retValue;

    }

// End of file
