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



#include "isiif.h"            // For RISIIf
#include "isiiftrace.h"       // For C_TRACE, ASSERT_RESET.. and fault codes
#include "isiinternaldefs.h"  //

const TInt KConnectArraySize( 2 );
const TInt KSendArraySize( 1 );
const TInt KReceiveArraySize( 3 );
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 1 );

// We need binary type suffix because LDD FW loads .ldd suffix as assumption.
_LIT( KISIDriverLddBinaryName, "isirouter.dll" );

enum TISIApiFaults
    {
    EISIApiDriverLoadFailed = 0x00,
    };

EXPORT_C MISIIf* MISIIf::NewISIIf( const TInt32 aUID, TUint8& aObjId )
    {
    C_TRACE( ( _T( "RISIIf::RISIIf <->" ) ) );
    return RISIIf::NewF( aUID, aObjId );
    }

RISIIf* RISIIf::NewF( const TInt32 aUID, TUint8& aObjId )
    {
    C_TRACE((_T("RISIIf::NewF>") ));
    RISIIf* tmp = new RISIIf( aUID, aObjId );
    ASSERT_PANIC_ALWAYS( tmp, ( EISIMemAllocFailure ) );
    C_TRACE((_T("RISIIf::NewF 0x%x<"), tmp ));
    return tmp;
    }

RISIIf::RISIIf( const TInt32 aUID, TUint8& aObjId )
    {
    C_TRACE( ( _T( "RISIIf::RISIIf 0x%x>" ), this ) );
	  // Same driver is used for all ISI user clients, it might be already loaded (KErrAlreadyExists), that is ok.
    TInt loadStatus( User::LoadLogicalDevice( KISIDriverLddBinaryName ) );
    C_TRACE( ( _T( "RISIIf::Connect devicedriver loaded %d" ), loadStatus ) );
    ASSERT_PANIC_ALWAYS( ( KErrNone == loadStatus || KErrAlreadyExists == loadStatus), EISIApiDriverLoadFailed );
    TInt error( KErrInUse );
    // KErrPermissionDenied( no capabilities), KErrGeneral (DThread::Open)
    error = DoCreate( KISIDriverName, 
                      TVersion(), 
                      KNullUnit, 
                      NULL, 
                      NULL, 
                      EOwnerThread );
    if( KErrNone != error )
        {
        C_TRACE( ( _T( "RISIIf::Connect 0x%x error %d" ), this, error ) );
        }
    else
        {
        C_TRACE( ( _T( "RISIIf::Connect 0x%x " ), this ) );        
        TAny* params[ KConnectArraySize ];
        params[ KFirstParam ] = reinterpret_cast< TAny* >( aUID );
        params[ KSecondParam ] = reinterpret_cast< TAny* >( &aObjId );
        error = DoControl( EISIConnect, params );
        C_TRACE( ( _T( "RISIIf::id is 0x%x " ), aObjId ) );
        TRACE_ASSERT_ALWAYS_COND( KErrNone == error );
        }
    C_TRACE( ( _T( "RISIIf::RISIIf 0x%x<" ), this ) );
    }

RISIIf::~RISIIf()
    {
    C_TRACE( ( _T( "RISIIf::~RISIIf 0x%x>" ), this ) );
    RHandleBase::Close();
    C_TRACE( ( _T( "RISIIf::~RISIIf 0x%x<" ), this ) );
    }

void RISIIf::Receive( TRequestStatus& aRxStatus, TDes8& aRxMsg, TUint16& aNeededBufLen )
    {
    C_TRACE( ( _T( "RISIIf::Receive 0x%x s %d b 0x%x>" ), this , &aRxStatus, &aRxMsg ) );
	  TAny* params[ KReceiveArraySize ];
    // Set status pending, just in case client haven't done it.
    aRxStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aRxStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aRxMsg );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( &aNeededBufLen );
    DoControl( EISIAsyncReceive, params );
    C_TRACE( ( _T( "RISIIf::Receive 0x%x s %d b 0x%x<" ), this , &aRxStatus, &aRxMsg ) );
    }
    
void RISIIf::ReceiveCancel()
    {
    C_TRACE( ( _T( "RISIIf::ReceiveCancel 0x%x>" ), this ) );
	  DoCancel( EISIAsyncReceive );
    C_TRACE( ( _T( "RISIIf::ReceiveCancel 0x%x<" ), this ) );
    }
    
void RISIIf::Release()
    {
    C_TRACE( ( _T( "RISIIf::Release 0x%x>" ), this ) );
    delete this;
    C_TRACE( ( _T( "RISIIf::Release 0x%x<" ), this ) );
    }
    
TInt RISIIf::Send( const TDesC8& aTxMsg )
    {
    C_TRACE( ( _T( "RISIIf::Send 0x%x m 0x%x>" ), this, &aTxMsg ) );
	  TAny* params[ KSendArraySize ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( const_cast<TDesC8*>( &aTxMsg ));
    TInt sendResult = DoControl( EISISend, params );
    C_TRACE( ( _T( "RISIIf::Send 0x%x 0x%x %d <" ), this, &aTxMsg, sendResult ) );
	  return sendResult;
    }

//  End of File  
