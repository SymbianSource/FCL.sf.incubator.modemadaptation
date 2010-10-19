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



#include <kernel.h>               // For Kern
#include "isiroutertrace.h"       // For C_TRACE, ASSERT_RESET.. and fault codes
#include "isiinternaldefs.h"      // For KAmountOfKernelThreads...
#include "isithreadcontainer.h"   // For DISIThreadContainer

// Faults
enum TISIThreadContainerFaults
    {
    EISIThreadContainerDfcQAllocFailure = 0x01,
    EISIThreadContainerDfcQAllocFailure1,
    EISIThreadContainerUnknownThreadType,
    EISIThreadContainerNULLThreadPtr,
    EISIThreadContainerThreadNotFound,
    EISIThreadContainerOverflow,
    EISIThreadContainerOverflow1,
    EISIThreadContainerUnderflow,
    EISIThreadContainerUnderflow1,
    };


_LIT8( KISIKernelClientThread, "ISIKernelClientThread" );
_LIT8( KISIUserClientThread, "ISIUserClientThread" );

const TInt KDefaultDfcThreadPriority( 27 );
const TUint8 KMaxThreadUsers( 255 );
const TUint8 KMaxThreadNameLength( 25 );

DISIThreadContainer::DISIThreadContainer()
    {
    C_TRACE( ( _T( "DISIThreadContainer::DISIThreadContainer>" ) ) );
    TUint8 nameIndex = 0;
    for( TUint8 i( 0 ); i < KAmountOfKernelThreads; i++ )
        {
        iNameTable[ nameIndex ] = HBuf8::New( KMaxThreadNameLength );
        iNameTable[ nameIndex ]->Append( (&KISIKernelClientThread)->Ptr(), (&KISIKernelClientThread)->Length() );
        iNameTable[ nameIndex ]->AppendNum( i, EDecimal );
        Kern::DfcQCreate( iKClientDfcQueList[ i ], KDefaultDfcThreadPriority, iNameTable[ nameIndex ] );
        ASSERT_RESET_ALWAYS( iKClientDfcQueList[ i ], ( EISIThreadContainerDfcQAllocFailure | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
        nameIndex++;
        }
    for( TUint8 i( 0 ); i < KAmountOfUserThreads; i++ )
        {
        iNameTable[ nameIndex ] = HBuf8::New( KMaxThreadNameLength );
        iNameTable[ nameIndex ]->Append( (&KISIUserClientThread)->Ptr(), (&KISIUserClientThread)->Length() );
        iNameTable[ nameIndex ]->AppendNum( i, EDecimal );
        Kern::DfcQCreate( iUClientDfcQueList[ i ], KDefaultDfcThreadPriority, iNameTable[ nameIndex ] );
        ASSERT_RESET_ALWAYS( iUClientDfcQueList[ i ], ( EISIThreadContainerDfcQAllocFailure1 | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
        nameIndex++;
        }
    C_TRACE( ( _T( "DISIThreadContainer::DISIThreadContainer<" ) ) );
    }

DISIThreadContainer::~DISIThreadContainer()
    {
    C_TRACE( ( _T( "DISIThreadContainer::~DISIThreadContainer>" ) ) );
    for( TUint8 i( 0 ); i < ( KAmountOfKernelThreads + KAmountOfUserThreads ); i++ )
        {
        if( iNameTable[ i ] )
            {
            delete iNameTable[ i ];
            iNameTable[ i ] = NULL;
            }
        }
    for( TUint8 i( 0 ); i < KAmountOfKernelThreads; i++ )
        {
        if( iKClientDfcQueList[ i ] )
            {
            delete iKClientDfcQueList[ i ];
            iKClientDfcQueList[ i ] = NULL;
            }
        }
    for( TUint8 i( 0 ); i < KAmountOfUserThreads; i++ )
        {
        if( iUClientDfcQueList[ i ] )
            {
            delete iUClientDfcQueList[ i ];
            iUClientDfcQueList[ i ] = NULL;
            }
        }
    C_TRACE( ( _T( "DISIThreadContainer::~DISIThreadContainer<" ) ) );
    }

TDfcQue* DISIThreadContainer::AllocateThread( const MISIObjectRouterIf::TISIDfcQThreadType aType )
    {
    C_TRACE( ( _T( "DISIThreadContainer::AllocateThread %d>" ), aType ) );
    TDfcQue* tmpPtr( NULL );
    switch( aType )
        {
        case MISIObjectRouterIf::EISIKernelMainThread:  
        case MISIObjectRouterIf::EISIKernelRequestCompletionThread:
            {
            tmpPtr = ReserveKernelThread();
            break;
            }
        case MISIObjectRouterIf::EISIUserMainThread:
        case MISIObjectRouterIf::EISIUserRequestCompletionThread:
            {
            tmpPtr = ReserveUserThread();
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EISIThreadContainerUnknownThreadType | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
            break;
            }
        }
    ASSERT_RESET_ALWAYS( tmpPtr, ( EISIThreadContainerNULLThreadPtr | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIThreadContainer::AllocateThread %d 0x%x<" ), aType, tmpPtr ) );
    return tmpPtr;
    }

TDfcQue* DISIThreadContainer::ReserveKernelThread()
    {
    C_TRACE( ( _T( "DISIThreadContainer::ReservekernelThread>" ) ) );
    TUint8 clientCount = iKThreadOccupation[ 0 ];
    TUint8 index = 0;
    for( TUint8 i( 0 ) ; i < KAmountOfKernelThreads; i++ )
        {
        if( clientCount > iKThreadOccupation[ i ] )
            {
            clientCount = iKThreadOccupation[ i ];
            index = i;
            }
        }
    ++iKThreadOccupation[ index ];
    ASSERT_RESET_ALWAYS( iKThreadOccupation[ index ] <= KMaxThreadUsers, ( EISIThreadContainerOverflow | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIThreadContainer::ReserveKernelThread 0x%x %d %d<" ), iKClientDfcQueList[ index ], iKThreadOccupation[ index ], index ) );
    return iKClientDfcQueList[ index ];
    }

TDfcQue* DISIThreadContainer::ReserveUserThread()
    {
    C_TRACE( ( _T( "DISIThreadContainer::ReserveUserThread>" ) ) );
    TUint8 clientCount = iUThreadOccupation[ 0 ];
    TUint8 index = 0;
    for( TUint8 i( 0 ); i < KAmountOfUserThreads; i++ )
        {
        if( clientCount > iUThreadOccupation[ i ] )
            {
            clientCount = iUThreadOccupation[ i ];
            index = i;
            }
        }
    ++iUThreadOccupation[ index ];
    ASSERT_RESET_ALWAYS( iUThreadOccupation[ index ] <= KMaxThreadUsers, ( EISIThreadContainerOverflow1 | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIThreadContainer::ReserveUserThread 0x%x %d %d<" ), iUClientDfcQueList[ index ], iUThreadOccupation[ index ], index ) );
    return iUClientDfcQueList[ index ];
    }

void DISIThreadContainer::DeallocateThread( TDfcQue* aThread )
    {
    C_TRACE( ( _T( "DISIThreadContainer::DeallocateThread 0x%x>" ), aThread ) );
    TBool found( EFalse );
    for( TUint8 i( 0 ) ; i < KAmountOfKernelThreads; i++ )
        {
        if( aThread == iKClientDfcQueList[ i ] )
            {
            --iKThreadOccupation[ i ];
            ASSERT_RESET_ALWAYS( iKThreadOccupation[ i ] != KMaxThreadUsers, ( EISIThreadContainerUnderflow | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
            C_TRACE( ( _T( "DISIThreadContainer DeallocKThread 0x%x %d %d>" ), aThread, iKThreadOccupation[ i ], i ) );
            found = ETrue;
            break;
            }
        }
    if( !found )
        {
        for( TUint8 i( 0 ) ; i < KAmountOfUserThreads; i++ )
            {
            if( aThread == iUClientDfcQueList[ i ] )
                {
                --iUThreadOccupation[ i ];
                ASSERT_RESET_ALWAYS( iUThreadOccupation[ i ] != KMaxThreadUsers, ( EISIThreadContainerUnderflow1 | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
                C_TRACE( ( _T( "DISIThreadContainer DeallocUThread 0x%x %d %d>" ), aThread, iUThreadOccupation[ i ], i ) );
                found = ETrue;
                break;
                }
            }
        }
    ASSERT_RESET_ALWAYS( found, ( EISIThreadContainerThreadNotFound | EDISIThreadContainerTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISIThreadContainer::DeallocateThread 0x%x<" ), aThread ) );
    }

