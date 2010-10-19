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


#include "isiif.h"            // For DISIIf
#include "isiroutertrace.h"   // For C_TRACE, ASSERT_RESET.. and fault codes
#include "isikernelchannel.h" // For DISIKernelChannel
#include <dfcs.h>             // For TDfc
#include <kern_priv.h>        // For __ASSERT_CRITICAL 


// Constants
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );
const TInt KOneParam( 1 );
const TInt KTwoParams( 2 );
const TInt KThreeParams( 3 );


// Faults
enum TISIIfFaults
    {
    EISIIfMemAllocFailure = 0x01,
    EISIIfMemAllocFailure2,
    EISIIfNotThreadContext,
    EISIIfNotThreadContext2,
    EISIIfNotNullPtr,
    EISIIfWrongResponseToRequest,
    EISIIfWrongResponseToRequest2,
    EISIIfWrongResponseToRequest3,
    EISIIfWrongResponseToRequest4,
    EISIIfWrongResponseToRequest5,
    EISIIfWrongResponseToRequest6,
    };


EXPORT_C MISIIf* MISIIf::NewISIIf( const TInt32 aUID, TUint8& aObjId )
    {
    return DISIIf::NewF( aUID, aObjId );
    }

DISIIf* DISIIf::NewF( const TInt32 aUID, TUint8& aObjId )
    {
    C_TRACE((_T("DISIIf::NewF>") ));
    DISIIf* tmp = new DISIIf( aUID, aObjId );
    ASSERT_RESET_ALWAYS( tmp, ( EISIIfMemAllocFailure | EDISIIfTraceId << KClassIdentifierShift ) );
    C_TRACE((_T("DISIIf::NewF 0x%x<"), tmp ));
    return tmp;
    }

DISIIf::DISIIf( const TInt32 aUID, TUint8& aObjId )
    {
    C_TRACE( ( _T( "DISIIf::DISIIf 0x%x>" ), this ) );
    __ASSERT_CRITICAL;// From kern_priv.h published partner
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIIfNotThreadContext | EDISIIfTraceId << KClassIdentifierShift ) );
    iKernelChannel = new DISIKernelChannel( aObjId );
    ASSERT_RESET_ALWAYS( iKernelChannel, ( EISIIfMemAllocFailure2 | EDISIIfTraceId << KClassIdentifierShift ) );
    TAny* params[ KTwoParams ];  // should probably take process identifier into this to identify binary
    params[ KFirstParam ] = reinterpret_cast<TAny*>( aUID );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aObjId );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EISIConnect;
    m.iArg[ KFirstParam ] = params;
    // Other asserts in kernel channel.
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EISIIfWrongResponseToRequest2 | EDISIIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIIf::DISIIf 0x%x<" ), this ) );
    }

DISIIf::~DISIIf()
    {
    C_TRACE( ( _T( "DISIIf::~DISIIf 0x%x>" ), this ) );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EISIDisconnect;
    m.iArg[ KFirstParam ] = NULL;
        
    if( iKernelChannel )
        {
        // Other asserts in kernel channel.
        ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EISIIfWrongResponseToRequest | EDISIIfTraceId << KClassIdentifierShift ) );
        delete iKernelChannel;
        iKernelChannel = NULL;
        }
        
    C_TRACE( ( _T( "DISIIf::~DISIIf 0x%x<" ), this ) );
    }

TDes8& DISIIf::AllocateMsgBlock( const TUint16 aSize )
    {   
    C_TRACE( ( _T( "DISIIf::AllocateMsgBlock 0x%x %d>" ), this, aSize ) );
    TDes8* temp = NULL;
    TInt size( aSize );
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &size );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &temp );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EISIAllocateBlock;
    m.iArg[ KFirstParam ] = params;
    // Other asserts in kernel channel.
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EISIIfWrongResponseToRequest3 | EDISIIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIIf::AllocateMsgBlock 0x%x %d<" ), this, aSize ) );
    return *temp;  
    }
    
void DISIIf::DeallocateMsgBlock( TDes8& aMsgBlock )
    {
    C_TRACE( ( _T( "DISIIf::DeallocateMsgBlock 0x%x s 0x%x>" ), this, &aMsgBlock ) );
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aMsgBlock );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EISIDeallocateBlock;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EISIIfWrongResponseToRequest4 | EDISIIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIIf::DeallocateMsgBlock 0x%x s 0x%x<" ), this, &aMsgBlock ) );
    }
  
void DISIIf::Receive( TRequestStatus& aRxStatus, TDes8*& aRxMsg, const TDfc& aRxCompletedDfc )
    {
    C_TRACE( ( _T( "DISIIf::Receive 0x%x %d 0x%x 0x%x>" ), this, &aRxStatus, &aRxMsg, &aRxCompletedDfc ) );
    ASSERT_RESET_ALWAYS( !aRxMsg, ( EISIIfNotNullPtr | EDISIIfTraceId << KClassIdentifierShift ) );
    aRxStatus = KRequestPending;
    TAny* params[ KThreeParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aRxStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aRxMsg );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( &const_cast<TDfc&>( aRxCompletedDfc ) );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EISIAsyncReceive;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EISIIfWrongResponseToRequest5 | EDISIIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIIf::Receive 0x%x %d 0x%x 0x%x<" ), this, &aRxStatus, &aRxMsg, &aRxCompletedDfc ) );
    }
void DISIIf::ReceiveCancel()
    {
    C_TRACE( ( _T( "DISIIf::ReceiveCancel 0x%x>" ), this ) );
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EISIAsyncReceive );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), ( EISIIfWrongResponseToRequest6 | EDISIIfTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIIf::ReceiveCancel 0x%x<" ), this ) );
    }
    
void DISIIf::Release()
    {
    C_TRACE( ( _T( "DISIIf::Release 0x%x>" ), this ) );
    __ASSERT_CRITICAL;
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISIIfNotThreadContext2 | EDISIIfTraceId << KClassIdentifierShift ) );
    delete this;
    C_TRACE( ( _T( "DISIIf::Release 0x%x<" ), this ) );
    }
TInt DISIIf::Send( const TDesC8& aTxMsg )
    {
    C_TRACE( ( _T( "DISIIf::Send 0x%x m 0x%x>" ), this, &aTxMsg ) );
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( const_cast<TDesC8*>( &aTxMsg )); // need const?
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EISISend;
    m.iArg[ KFirstParam ] = params;
    TInt retValue( iKernelChannel->HandleRequest( m ) );
    C_TRACE( ( _T( "DISIIf::Send 0x%x m 0x%x %d<" ), this, &aTxMsg, retValue ) );
	  return retValue;
    }

// End of file
