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
#include <nk_priv.h>      // For __ASSERT_NO_FAST_MUTEX (published to partners)

#include <mmuxtrxif.h>          // For MMuxTrxIf
#include <mtrxmuxif.h>          // For MTrxMuxIf, E... faults

#include "mmuxlinkif.h"         // For internal MMuxLinkIf
#include "multiplexer.h"        // For DMultiplexer
#include "mux.h"                // For DMux
#include "multiplexertrace.h"   // For C_TRACE..
#include "memapi.h"             // For MemApi

// Faults
enum TMuxFaults
    {
    EMuxMemAllocFailure = 0x00,
    EMuxMemAllocFailure2,
    EMuxNotThreadContext,
    EMuxNullParam,
    EMuxNullPtr,
    EMuxNullPtr2,
    EMuxInvalidLinkId,
    };

EXPORT_C MLinkMuxIf* MLinkMuxIf::Register( MMuxLinkIf* aTrx, const TUint8 aTrxId, const TUint8 aLinkId )
    {

    C_TRACE( ( _T( "MLinkMuxIf::Register 0x%x %d %d<>" ), aTrx, aTrxId, aLinkId ) );
    __ASSERT_NO_FAST_MUTEX;
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EMuxNotThreadContext | EDMuxTraceId << KClassIdentifierShift ) );
    return DMultiplexer::RegisterLink( aTrx, aTrxId, aLinkId );

    }

EXPORT_C MTrxMuxIf* MTrxMuxIf::Register( MMuxTrxIf* aTrx, const TUint8 aTrxId )
    {

    C_TRACE( ( _T( "MLinkMuxIf::Register 0x%x %d<>" ), aTrx, aTrxId ) );
    __ASSERT_NO_FAST_MUTEX;
    MUX_ASSERT_FAULT_MACRO( ( aTrx ), MTrxMuxIf::ENullTrxPointer );
    MUX_ASSERT_FAULT_MACRO( ( EAmountOfTrxs > aTrxId ), MTrxMuxIf::EInvalidTrxId );
    MUX_ASSERT_FAULT_MACRO( ( ISTAPI_ASSERT_KERNEL_THREAD_CONTEXT_ALWAYS ), MTrxMuxIf::ENotKernelThreadContext );
    return DMultiplexer::RegisterTrx( aTrx, aTrxId );

    }
/*
* Can be called in different kernel thread contextes.
*/
DMux::DMux(
        const TUint8 aTrxId
        )
    {

    C_TRACE( ( _T( "DMux::DMux this 0x%x id %d>" ), this, iShTrxId ) );
    // Synch with fastMutex, not nested no blocking no allocation, and we need perf.
    iFastMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iFastMutex, ( EMuxMemAllocFailure | EDMuxTraceId << KClassIdentifierShift ) );
    NKern::FMWait( iFastMutex );
    iShTrx = NULL;
    iShTrxId = aTrxId;
    for( TInt i( 0 ); i < EMuxAmountOfProtocols; i++ )
        {
        iShLinks[ i ] = NULL;
        }
    NKern::FMSignal( iFastMutex );
    C_TRACE( ( _T( "DMux::DMux this 0x%x id %d<" ), this, iShTrxId ) );

    }

/*
* Deleted by one thread when kernel is shutting down.
*/
DMux::~DMux(
        // None
        )
    {

    C_TRACE( ( _T( "DMux::~DMux this 0x%x id %d>" ), this, iShTrxId ) );
    NKern::FMWait( iFastMutex );
    iShTrxId = 0x00;
    iShTrx = NULL;
    // NULL due no ownership of items
    for( TInt i( 0 ); i < EMuxAmountOfProtocols; i++ )
        {
        iShLinks[ i ] = NULL;
        }
    NKern::FMSignal( iFastMutex );
    // No need to check, if not created already reseted.
    delete iFastMutex;
    iFastMutex = NULL;
    C_TRACE( ( _T( "DMux::~DMux this 0x%x id %d<" ), this, iShTrxId ) );

    }

TUint8 DMux::GetTrxId(
        // None
        ) const
    {

    C_TRACE( ( _T( "DMux::GetTrxId this 0x%x id %d<>" ), this, iShTrxId ) );
    return iShTrxId;

    }

/*
* Can be called in different kernel thread contextes.
*/
void DMux::SetLink(
        MMuxLinkIf* aLink,
        const TUint8 aLinkId
        )
    {

    C_TRACE( ( _T( "DMux::SetLink 0x%x this 0x%x id %d %d>" ), aLink, this, aLinkId, iShTrxId ) );
    __ASSERT_NO_FAST_MUTEX;
    ASSERT_RESET_ALWAYS( ( aLink ), ( EMuxNullParam | EDMuxTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aLinkId < EMuxAmountOfProtocols ), ( EMuxInvalidLinkId | EDMuxTraceId << KClassIdentifierShift ) );
    NKern::FMWait( iFastMutex );
    iShLinks[ aLinkId ] = aLink;
    NKern::FMSignal( iFastMutex );
    C_TRACE( ( _T( "DMux::SetLink 0x%x 0x%x this 0x%x id %d %d<" ), aLink, this, aLinkId, iShTrxId ) );

    }

/*
* Most likely called in one kernel thread context, but not limited to one.
*/
void DMux::SetTrx(
        MMuxTrxIf* aTrx
        )
    {

    C_TRACE( ( _T( "DMux::SetTrx 0x%x this 0x%x id %d>" ), aTrx, this, iShTrxId ) );
    __ASSERT_NO_FAST_MUTEX;
    MUX_ASSERT_FAULT_MACRO( ( aTrx ), MTrxMuxIf::ENullTrxPointer );
    MUX_ASSERT_FAULT_MACRO( ( !iShTrx ), MTrxMuxIf::ENullTrxPointer );
    NKern::FMWait( iFastMutex );
    // Set transceiver first and then notify it's status.
    iShTrx = aTrx;
    NotifyTrxStatusChangeToAllLinks( ETrue );
    NKern::FMSignal( iFastMutex );
    C_TRACE( ( _T( "DMux::SetTrx 0x%x 0x%x this 0x%x id %d<" ), aTrx, iShTrx, this, iShTrxId ) );

    }

// From MTrxMuxIf start

/*
* Can be called in different kernel thread contextes.
*/
TDes8& DMux::AllocateBlock(
        const TUint16 aSize
        )
    {

    C_TRACE( ( _T( "DMux::AllocateBlock this 0x%x id %d size %d<>" ), this, iShTrxId, aSize ) );
    __ASSERT_NO_FAST_MUTEX;
    MUX_ASSERT_FAULT_MACRO( aSize, MTrxMuxIf::EInvalidSize );
    MUX_ASSERT_FAULT_MACRO( ( ISTAPI_ASSERT_KERNEL_THREAD_CONTEXT_ALWAYS ), MTrxMuxIf::ENotKernelThreadContext );
    return MemApi::AllocBlock( aSize );

    }

/*
* Can be called in different kernel thread contextes.
*/
void DMux::DeallocateBlock(
        TDes8& aBlock
        )
    {

    C_TRACE( ( _T( "DMux::DeallocateBlock this 0x%x id %d block 0x%x>" ), this, iShTrxId, &aBlock ) );
    __ASSERT_NO_FAST_MUTEX;
    MUX_ASSERT_FAULT_MACRO( ( ISTAPI_ASSERT_KERNEL_THREAD_CONTEXT_ALWAYS ), MTrxMuxIf::ENotKernelThreadContext );
    MemApi::DeallocBlock( aBlock );
    C_TRACE( ( _T( "DMux::DeallocateBlock this 0x%x id %d block 0x%x<" ), this, iShTrxId, &aBlock ) );

    }

/*
* Most likely called in one kernel thread context, but not limited to one.
* Message received from transceiver, should be demultiplexed to appropriate link.
*/
void DMux::Receive(
        TDes8& aMsg,
        const TUint32 aMuxPacket
        )
    {

    C_TRACE( ( _T( "DMux::Receive this 0x%x id %d msg 0x%x aMuxId 0x%x>" ), this, iShTrxId, &aMsg, aMuxPacket ) );
    const TUint8 protocolId = static_cast<TUint8>( aMuxPacket >> 24 );    
    C_TRACE( ( _T( "DMux::Receive this 0x%x id %d msg 0x%x aMuxId 0x%x protocolId 0x%x" ), this, iShTrxId, &aMsg, aMuxPacket, protocolId ) );
    __ASSERT_NO_FAST_MUTEX;
    MUX_ASSERT_FAULT_MACRO( ( protocolId < EMuxAmountOfProtocols ), MTrxMuxIf::EInvalidMuxingHeader );
    MUX_ASSERT_FAULT_MACRO( ( ISTAPI_ASSERT_KERNEL_THREAD_CONTEXT_ALWAYS ), MTrxMuxIf::ENotKernelThreadContext );
    //  ( aMsg.Length() != protocollenght) 
    NKern::FMWait( iFastMutex );
    MMuxLinkIf* link = iShLinks[ protocolId ];
    NKern::FMSignal( iFastMutex );
    ASSERT_RESET_ALWAYS( ( link ), ( EMuxNullPtr | EDMuxTraceId << KClassIdentifierShift ) );
    link->Receive( aMsg );
    C_TRACE( ( _T( "DMux::Receive this 0x%x id %d msg 0x%x aMuxId 0x%x<" ), this, iShTrxId, &aMsg, aMuxPacket ) );

    }
    
/*
* Most likely called in one kernel thread context, but not limited to one.
*/
void DMux::Unregister(
        // None
        )
    {

    C_TRACE( ( _T( "DMux::Unregister this 0x%x id %d trx 0x%x>" ), this, iShTrxId, iShTrx ) );
    __ASSERT_NO_FAST_MUTEX;
    MUX_ASSERT_FAULT_MACRO( ( iShTrx ), MTrxMuxIf::ETrxNotRegistered );
    MUX_ASSERT_FAULT_MACRO( ( ISTAPI_ASSERT_KERNEL_THREAD_CONTEXT_ALWAYS ), MTrxMuxIf::ENotKernelThreadContext );
    NKern::FMWait( iFastMutex );
    NotifyTrxStatusChangeToAllLinks( EFalse );
    iShTrx = NULL;
    NKern::FMSignal( iFastMutex );
    C_TRACE( ( _T( "DMux::Unregister this 0x%x id %d trx 0x%x<" ), this, iShTrxId, iShTrx ) );

    }

// From MTrxMuxIf end

// From MLinkMuxIf start
void DMux::Send(
        TDes8& aMsg,
        const TUint8 aLinkId,
        const TMessageSendPriority aPriority
        )
    {

    C_TRACE( ( _T( "DMux::Send this 0x%x id %d msg 0x%x aLinkId 0x%x priority %d <>" ), this, iShTrxId, &aMsg, aLinkId, aPriority ) );
    TUint32 muxId = static_cast<TUint32>( aMsg.Length() | aLinkId << 24 );     
    C_TRACE( ( _T( "DMux::Send this 0x%x id %d msg 0x%x aLinkId 0x%x priority %d muxId 0x%x<>" ), this, iShTrxId, &aMsg, aLinkId, aPriority, muxId ) );
    ASSERT_RESET_ALWAYS( ( iShTrx ), ( EMuxNullPtr2 | EDMuxTraceId << KClassIdentifierShift ) );
    iShTrx->Transmit( aMsg, (MMuxTrxIf::TDataTransmitPriority) aPriority, muxId );

    }

// From MLinkMuxIf end

// Internal start
/*
* Called with fast mutex held no blocking operations like traces allowed.
*/
void DMux::NotifyTrxStatusChangeToAllLinks(
       TBool aPresent
       )
    {

    //  Change status so that it can not be happen send from up and status change from down simultaneously
    for( TInt i( 0 ); i < EMuxAmountOfProtocols; i++ )
        {
        MMuxLinkIf* link = iShLinks[ i ];
        if( link )
            {
            // Link must enque a DFC after this call.
            link->EnqueTrxPresenceChangedDfc( aPresent );
            }
        }

    }
// Internal start end
