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



// INCLUDE FILES
#include <p2pif.h>                  // For RP2PIf
#include "p2puserapitrace.h"        // For C_TRACE...
#include "p2pinternaldefs.h"        // For KP2PDriverName..

// CONSTANTS
const TInt KConnectionLostArraySize( 1 );
const TInt KOpenArraySize( 1 );
const TInt KReceiveArraySize( 2 );
const TInt KSendArraySize( 1 );
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );

enum TP2PApiFaults
    {
    EP2PApiDriverLoadFailed = 0x00,
    EP2PApiCloseFailed,
    };

// LOCAL CONSTANTS AND MACROS
const TInt KInfoLength( sizeof(TUint8) );

EXPORT_C RP2PIf::RP2PIf()
    {

    C_TRACE( ( _T( "RP2PIf::RP2PIf<>" ) ) );

    }


EXPORT_C RP2PIf::~RP2PIf()
    {

    C_TRACE( ( _T( "RP2PIf::~RP2PIf 0x%x>" ), this ) );
    Close();
    C_TRACE( ( _T( "RP2PIf::~RP2PIf 0x%x<" ), this ) );

    }

EXPORT_C void RP2PIf::Close(
        // None
        )
    {

    C_TRACE( ( _T( "RP2PIf::Close 0x%x>" ), this ) );
    RHandleBase::Close();
    C_TRACE( ( _T( "RP2PIf::Close 0x%x<" ), this ) );

    }

EXPORT_C void RP2PIf::ConnectionLost(
        TRequestStatus& aStatus
        )
    {

    C_TRACE( ( _T( "RP2PIf::ConnectionLost 0x%x>" ), this ) );
    TAny* params[ KConnectionLostArraySize ];
    // Set status pending, just in case client haven't done it.
    aStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    DoControl( EP2PAsyncConnectionLost, params );
    C_TRACE( ( _T( "RP2PIf::ConnectionLost 0x%x<" ), this ) );

    }

EXPORT_C void RP2PIf::ConnectionLostCancel(
        // None
        )
    {

    C_TRACE( ( _T( "RP2PIf::ConnectionLostCancel 0x%x>" ), this ) );
    DoCancel( EP2PAsyncConnectionLost );
    C_TRACE( ( _T( "RP2PIf::ConnectionLostCancel 0x%x<" ), this ) );

    }

EXPORT_C void RP2PIf::Open(
        const TP2PProtocol aP2PProtocolId,
        TRequestStatus& aStatus,
        TOwnerType aType
        )
    {

    C_TRACE( ( _T( "RP2PIf::Open 0x%x 0x%x 0x%x 0x%x>" ), this, aP2PProtocolId, &aStatus, aType ) );
    // Same driver is used for all P2P protocols, it might be already loaded (KErrAlreadyExists), that is ok.
    TInt loadStatus( User::LoadLogicalDevice( KP2PDriverLddBinaryName ) );
    C_TRACE( ( _T( "RP2PIf::Open devicedriver loaded %d" ), loadStatus ) );
    ASSERT_PANIC_ALWAYS( ( KErrNone == loadStatus || KErrAlreadyExists == loadStatus), EP2PApiDriverLoadFailed );
    TInt error( KErrInUse );
    C_TRACE( ( _T( "RP2PIf::Open ldd" ) ) );
    HBufC8* buffer = NULL;
    TRAPD( err, buffer = HBufC8::NewL( KInfoLength ) );        
    ASSERT_PANIC_ALWAYS( err == KErrNone, KErrNoMemory );
        
    TPtr8 info = buffer->Des();
    info.Append( aP2PProtocolId );
    //  check wrong usage of driver load differently than from here
    // KErrPermissionDenied( no capabilities) ,  KErrNotSupported (Wrong protocol id), KErrGeneral (DThread::Open), 
    // KErrAlreadyExists same object had opened the interface with some other protocolid
    error = DoCreate( KP2PDriverName, 
                      TVersion(), 
                      KNullUnit, 
                      NULL, 
                      &info, 
                      aType );
    delete buffer;
    if( KErrNone != error )
        {
        C_TRACE( ( _T( "RP2PIf::Open 0x%x failed 0x%x error %d" ), this, aP2PProtocolId, error ) );
        TRequestStatus* ptrStatus = &aStatus;
        User::RequestComplete( ptrStatus, error );
        }
    else
        {
        C_TRACE( ( _T( "RP2PIf::Open 0x%x aP2PProtocolId 0x%x" ), this, aP2PProtocolId ) );
        aStatus = KRequestPending;
        TAny* params[ KOpenArraySize ];
        params[ KFirstParam ] = reinterpret_cast< TAny* >( &aStatus );
        // KErrInUse (protocol id already used)
        error = DoControl( EP2PAsyncOpen, params );
        TRACE_ASSERT_ALWAYS_COND( KErrNone == error );
        }
    C_TRACE( ( _T( "RP2PIf::Open 0x%x 0x%x 0x%x 0x%x<" ), this, aP2PProtocolId, &aStatus, aType ) );

    }

EXPORT_C void RP2PIf::OpenCancel(
        // None
        )
    {

    C_TRACE( ( _T( "RP2PIf::OpenCancel 0x%x>" ), this ) );
    DoCancel( EP2PAsyncOpen );
    Close();
    C_TRACE( ( _T( "RP2PIf::OpenCancel 0x%x<" ), this ) );

    }

EXPORT_C void RP2PIf::Receive(
        TRequestStatus& aStatus,
        TDes8& aReceiveBuffer
        )
    {

    C_TRACE( ( _T( "RP2PIf::Receive 0x%x 0x%x 0x%x>" ), this, &aStatus, &aReceiveBuffer ) );
    TAny* params[ KReceiveArraySize ];
    // Set status pending, just in case client haven't done it.
    aStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aReceiveBuffer );
    DoControl( EP2PAsyncReceive, params );
    C_TRACE( ( _T( "RP2PIf::Receive 0x%x 0x%x 0x%x<" ), this, &aStatus, &aReceiveBuffer ) );

    }

EXPORT_C void RP2PIf::ReceiveCancel(
        // None
        )
    {

    C_TRACE( ( _T( "RP2PIf::ReceiveCancel 0x%x>"), this ) );
    DoCancel( EP2PAsyncReceive );
    C_TRACE( ( _T( "RP2PIf::ReceiveCancel 0x%x<"), this ) );

    }

EXPORT_C TInt RP2PIf::Send(
        const TDesC8& aSendBuffer
        )
    {

    C_TRACE( ( _T( "RP2PIf::Send 0x%x 0x%x>" ), this, &aSendBuffer ) );
    TAny* params[ KSendArraySize ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( const_cast<TDesC8*>( &aSendBuffer ));
    TInt sendResult = DoControl( EP2PSend, params );
    C_TRACE( ( _T( "RP2PIf::Send 0x%x %d<" ), this, sendResult ) );
    return sendResult;

    }

//  End of File

