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



#include <kernel.h>      // For Kern
#include <kern_priv.h>   // For DMutex

#include <mmuxtrxif.h>          // For MMuxTrxIf
#include <mtrxmuxif.h>          // For MTrxMuxIf, EMultiplexer... faults

#include "multiplexer.h"        // For DMultiplexer
#include "mux.h"                // For DMux
#include "multiplexertrace.h"   // For C_TRACE..

// Faults
enum TMultiplexerFaults
    {
    EMultiplexerMemAllocFailure = 0x00,
    EMultiplexerMemAllocFailure2,
    EMultiplexerNullParam,
    EMultiplexerInvalidTrxId,
    EMultiplexerMutexCreateFailed,
    EMultiplexerMutexWaitFailed,
    };

// CONSTS
DMux* DMultiplexer::iShMultiplexers[ EAmountOfTrxs ] = { NULL };
DMutex* DMultiplexer::iShMultiplexersMutex = NULL;
_LIT8( KMultiplexerMutex, "KMultiplexerMutex" );

DMultiplexer::DMultiplexer(
        // None
        )
    {

    C_TRACE( ( _T( "DMultiplexer::DMultiplexer>" ) ) );
    // We need DMutex because lock is held while allocating memory and we do not have strict perf requirements for registerings.
    TInt err( Kern::MutexCreate( iShMultiplexersMutex, KMultiplexerMutex, KMutexOrdGeneral0 ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EMultiplexerMutexCreateFailed | EDMultiplexerTraceId << KClassIdentifierShift ) );
    // No need to protect cause done only from one thread.
    for( TInt i( 0 ); i < EAmountOfTrxs; i++ )
        {
        iShMultiplexers[ i ] = NULL;
        }
    C_TRACE( ( _T( "DMultiplexer::DMultiplexer<" ) ) );

    }

DMultiplexer::~DMultiplexer(
        // None
        )
    {

    C_TRACE( ( _T( "DMultiplexer::~DMultiplexer>" ) ) );
    // No protection needed, cause done only when kernel restarts and from one thread.
    for( TInt i( 0 ); i < EAmountOfTrxs; i++ )
        {
        DMux* temp = iShMultiplexers[ i ];
        delete temp;
        temp = NULL;
        iShMultiplexers[ i ] = NULL;
        }
        
    if ( iShMultiplexersMutex )
        {
        iShMultiplexersMutex->Close( NULL );
        }
        
    C_TRACE( ( _T( "DMultiplexer::~DMultiplexer<" ) ) );

    }

/*
* Registers a transceiver with unique transceiver identifier to multiplexer.
* Returns an interface to transceiver to use multiplexer services.
*/
MTrxMuxIf* DMultiplexer::RegisterTrx(
        MMuxTrxIf* aTrx,
        const TUint8 aTrxId
        )
    {

    C_TRACE( ( _T( "DMultiplexer::RegisterTrx 0x%x id %d>" ), aTrx, aTrxId ) );
    DMux* mux = GetMuxForTrx( aTrxId );
    mux->SetTrx( aTrx );
    C_TRACE( ( _T( "DMultiplexer::RegisterTrx 0x%x id %d mux 0x%x<" ), aTrx, aTrxId, mux ) );
    return mux;

    }

/*
* Registers a link with unique link identifier to multiplexer to send and receive data through
* transceiver identified with unique transceiver id.
* Returns an interface to link to use multiplexer services.
*/
MLinkMuxIf* DMultiplexer::RegisterLink(
        MMuxLinkIf* aLink,
        const TUint8 aTrxId,
        const TUint8 aLinkId
        )
    {

    C_TRACE( ( _T( "DMultiplexer::RegisterLink 0x%x id %d %d>" ), aLink, aLinkId, aTrxId ) );
    ASSERT_RESET_ALWAYS( ( aLink ), ( EMultiplexerNullParam | EDMultiplexerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( EAmountOfTrxs > aTrxId ), ( EMultiplexerInvalidTrxId | EDMultiplexerTraceId << KClassIdentifierShift ) );
    DMux* mux = GetMuxForTrx( aTrxId );
    mux->SetLink( aLink, aLinkId );
    C_TRACE( ( _T( "DMultiplexer::RegisterLink 0x%x id %d %d mux 0x%x<" ), aLink, aLinkId, aTrxId, mux ) );
    return mux;

    }

/*
* Creates new multiplexer for transceiver if not existing, if exists selects the existing one.
* Returns a pointer to transceivers multiplexer.
*/
DMux* DMultiplexer::GetMuxForTrx(
        const TUint8 aTrxId
        )
    {

    C_TRACE( ( _T( "DMultiplexer::GetMuxForTrx id %d>" ), aTrxId ) );
    TInt err( Kern::MutexWait( *iShMultiplexersMutex ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EMultiplexerMutexWaitFailed| EDMultiplexerTraceId << KClassIdentifierShift ) );
    DMux* mux = iShMultiplexers[ aTrxId ];
    if( !mux )
        {
        DMux* newMux = new DMux( aTrxId );
        iShMultiplexers[ aTrxId ] = newMux;
        mux = newMux;
        }
    ASSERT_RESET_ALWAYS( ( mux ), EMultiplexerMemAllocFailure );
    Kern::MutexSignal( *iShMultiplexersMutex );
    C_TRACE( ( _T( "DMultiplexer::GetMuxForTrx mux 0x%x id %d<" ), mux, aTrxId ) );
    return mux;

    }

DECLARE_STANDARD_EXTENSION()
    {

    Kern::Printf( "Multiplexer (L2) extension>" );
    // Create a container extension
    DMultiplexer* extension = new DMultiplexer();
    ASSERT_RESET_ALWAYS( ( extension ), ( EMultiplexerMemAllocFailure2 | EDMultiplexerTraceId << KClassIdentifierShift ) );
    Kern::Printf( "Multiplexer (L2) extension<" );
    return extension ? KErrNone : KErrNoMemory;

    }
