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
#include <pn_const.h>             // For PN_OBJ_ROUTING_REQ
#include <phonetisi.h>            // For ISI_HEADER_OFFSET_...
#include "isirouter.h"            // For DISIRouter
#include "isiroutertrace.h"       // For C_TRACE, ASSERT_RESET.. and fault codes
#include "isidevice.h"            // For DLogicalDevice
#include "isihelpers.h"           // For SET_SENDER_DEV...
//#include "misirouterobjectif.h"  // For MISIRouterObjectIf
#include "isithreadcontainer.h"   // For DISIThreadContainer
#include "isicltransceiver.h"     // For DISICLTransceiver
#include "misirouterobjectif.h"

const TUint32 KCommunicationManagerUID( 0x2002B3D0 );
const TUint32 KNameServiceUID( 0x2002A5A1 );
const TUint32 KIsiShRouterServiceUID( 0x20026FE7 );
const TUint32 KIsiShPipeHandlerUID( 0x2002E6E2 );

#define ROUTER_OBJECT_IDENTIFIER 0xFA // Only internally agreed inside isi router
#define PIPEHANDLER_OBJECT_IDENTIFIER 0xFB // Only internally agreed inside isi router

DISIRouter* DISIRouter::iSelfPtr = NULL;

// Faults
enum TISIRouterFaults
    {
    EISIRouterMemAllocFailure = 0x01,
    EISIRouterMemAllocFailure1,
    EISIRouterMemAllocFailure2,
    EISIRouterNotUniqueUID,
    EISIRouterNULLObjId,
    EISIRouterNULLPtr,
    EISIRouterNULLPtr1,
    EISIRouterNULLPtr2,
    EISIRouterNULLPtr3,
    EISIRouterNULLThreadPointer,
    EISIRouterRouterServiceNULLPtr,
    EISiShPipeHandlerNULLPtr,
    EISIRouterCommunicationManagerNULLPtr,
    EISIRouterNameServiceNULLPtr,
    EISIWrongObjectApiClient,
    };

const TInt KDfcPriority( 5 );

// #define WITHOUT_WRAPPERS_IN_USE //  to be defined on when wrappers are not used anymore
#ifndef WITHOUT_WRAPPERS_IN_USE
#include <iscnokiadefinitions.h> // Only allowed to include when wrappers are used
const TUint16 KFirstUserChannel( EIscNokiaUpdateApplication );
const TUint16 KLastKernelChannel( EIscNokiaLastKernelChannel );
#endif

void DISIRouter::Connect(
        const TInt32 aUID,
        TUint8& aObjId, 
        MISIRouterObjectIf* aCallback
        )
    {
    C_TRACE( ( _T( "DISIRouter::Connect 0x%x 0x%x 0x%x 0x%x>" ), aUID, aObjId, aCallback, this ) );
    ASSERT_RESET_ALWAYS( aCallback, ( EISIRouterNULLPtr | EDISIRouterTraceId << KClassIdentifierShift ) );
    TISIClient* tmp = new TISIClient();
    ASSERT_RESET_ALWAYS( tmp, ( EISIRouterMemAllocFailure | EDISIRouterTraceId << KClassIdentifierShift ) );
    tmp->iUID = aUID;
    tmp->iChannel = aCallback;
    TUint8 staticObjId = ReserveStaticObjId( aUID );
    NKern::FMWait( iClientTableFastMutex );
    TBool reserved = CheckUIDUniqueness( aUID );
#ifndef WITHOUT_WRAPPERS_IN_USE
    if( reserved )
        {
        NKern::FMSignal( iClientTableFastMutex );
        aObjId = 0xEE;
        }
    else
        {
#endif //WITHOUT_WRAPPERS_IN_USE
        if( KNotInitializedId == staticObjId )
            {
            tmp->iObjId = ReserveNewDynamicObjId();
            ASSERT_RESET_ALWAYS( tmp->iObjId, ( EISIRouterNULLObjId | EDISIRouterTraceId << KClassIdentifierShift ) );
            }
        else
            {
            tmp->iObjId = staticObjId;//  check that dynamic allocation don't return statically reserved object identifiers
            }
        iClientTable[ tmp->iObjId ] = tmp;
        NKern::FMSignal( iClientTableFastMutex );
        aObjId = tmp->iObjId;
#ifndef WITHOUT_WRAPPERS_IN_USE
        }
#endif
    C_TRACE( ( _T( "DISIRouter::Connect 0x%x 0x%x 0x%x<" ), aUID, aObjId, aCallback ) );
    }

TBool DISIRouter::CheckUIDUniqueness( const TInt32 aUID )
    {
    TBool ret(EFalse);
    // No tracing with mutex
    for( TInt i( 0 ); i < KMaxAmountOfObjId; i++ )
        {
        if( iClientTable[ i ]  && ( iClientTable[ i ]->iUID == aUID ) )
            {
#ifndef WITHOUT_WRAPPERS_IN_USE
            if( aUID <= KLastKernelChannel )
                {
                ret = ETrue;
                }
            else
                {
#endif //WITHOUT_WRAPPERS_IN_USE
                ASSERT_RESET_ALWAYS( 0, ( EISIRouterNotUniqueUID | EDISIRouterTraceId << KClassIdentifierShift ) );
#ifndef WITHOUT_WRAPPERS_IN_USE
                }
#endif //WITHOUT_WRAPPERS_IN_USE
            }
        }
    return ret;
    }

TUint8 DISIRouter::ReserveStaticObjId( const TInt32 aUID )
    {
    C_TRACE( ( _T( "DISIRouter::ReserveStaticObjId 0x%x 0x%x>" ), aUID ) );
    for( TUint8 i = 0; i < iStaticObjIdTable.Count(); i++ )
        {
        if( iStaticObjIdTable[ i ]->iUID == aUID )
            {
            C_TRACE( ( _T( "DISIRouter::ReserveStaticObjId 0x%x 0x%x 0x%x<" ), aUID, iStaticObjIdTable[i]->iUID, iStaticObjIdTable[i]->iObjId ) );
            return iStaticObjIdTable[i]->iObjId;
            }
        }
    return KNotInitializedId;
    }

TUint8 DISIRouter::ReserveNewDynamicObjId()
    {
    // No tracing with mutex
#ifdef WITHOUT_WRAPPERS_IN_USE    
    TUint8 firstAllowedObjId = KFirstAllowedObjId;
#else
    TUint8 firstAllowedObjId = KLastKernelChannel;
#endif

    for( TInt i(firstAllowedObjId); i < KMaxAmountOfObjId; i++ )
        {
        if( !iClientTable[ i ] )
            {
            TBool reserved( EFalse );
            for( TUint8 j = 0; j < iStaticObjIdTable.Count(); j++ )
                {
                if( j == iStaticObjIdTable[ j ]->iObjId )
                    {
                    reserved = ETrue;
                    break;
                    }
                }
            if( !reserved )
                {
                return i;
                }
            }
        }
    return KNotInitializedId;
    }

void DISIRouter::Disconnect(
        const TUint8 aObjId
        )
    {
    C_TRACE( ( _T( "DISIRouter::Disconnect 0x%x>" ), aObjId ) );
    TISIClient* tmp = iClientTable[ aObjId ];
    NKern::FMWait( iClientTableFastMutex );
    iClientTable[ aObjId ] = NULL;
    NKern::FMSignal( iClientTableFastMutex );
    //Must exist
    delete tmp;
    tmp = NULL;
    C_TRACE( ( _T( "DISIRouter::Disconnect 0x%x<" ), aObjId ) );
    }

TDfcQue* DISIRouter::GetDfcThread(
        const TISIDfcQThreadType aType
        )
    {
    C_TRACE( ( _T( "DISIRouter::GetDfcThread<>" ) ) );  
    return iShClientThreadContainer->AllocateThread( aType );
    }

void DISIRouter::FreeDfcThread( TDfcQue* aThread )
    {
    C_TRACE( ( _T( "DISIRouter::FreeDfcThread 0x%x>" ), aThread ) );
    iShClientThreadContainer->DeallocateThread( aThread );
    C_TRACE( ( _T( "DISIRouter::FreeDfcThread 0x%x<" ), aThread ) );
    }

EXPORT_C MISIObjectRouterIf* MISIObjectRouterIf::GetIf()
    {
    C_TRACE( ( _T( "MISIObjectRouterIf::GetIf<>" ) ) );
    return DISIRouter::GetRouter();
    }

DISIRouter::DISIRouter(
        // None
        )
    {
    C_TRACE( ( _T( "DISIRouter::DISIRouter>" ) ) );
    iShClientThreadContainer = new DISIThreadContainer();
    ASSERT_RESET_ALWAYS( iShClientThreadContainer, ( EISIRouterMemAllocFailure1 | EDISIRouterTraceId << KClassIdentifierShift ) );
    iInitThread = iShClientThreadContainer->AllocateThread( MISIObjectRouterIf::EISIKernelMainThread );
    ASSERT_RESET_ALWAYS( iInitThread, ( EISIRouterNULLThreadPointer | EDISIRouterTraceId << KClassIdentifierShift ) );
    iInitializeDfc = new TDfc( InitializeDfc, this, iInitThread, KDfcPriority );
    ASSERT_RESET_ALWAYS( iInitializeDfc, ( EISIRouterMemAllocFailure2 | EDISIRouterTraceId << KClassIdentifierShift ) );
    iInitializeDfc->Enque();
    C_TRACE( ( _T( "DISIRouter::DISIRouter<" ) ) );
    }

void DISIRouter::InitializeDfc(
        TAny* aPtr
        )
    {
    C_TRACE( ( _T( "DISIRouter::InitializeDfc>" ) ) );
    DISIRouter* self = reinterpret_cast<DISIRouter*>( aPtr );
    self->Initialize();
    C_TRACE( ( _T( "DISIRouter::InitializeDfc<" ) ) );
    }

void DISIRouter::Initialize(
        // None
        )
    {
    C_TRACE( ( _T( "DISIRouter::Initialize> this: 0x%x" ), this ) );
    iSelfPtr = this;
    iClientTableFastMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iClientTableFastMutex, ( EISIRouterNULLPtr2 | EDISIRouterTraceId << KClassIdentifierShift ) );
    for( TInt i( 0 ); i < KMaxAmountOfObjId; i++)
        {
        iClientTable[ i ] = NULL;
        }
    //  comment improve hex or des
    //  nameservice, communication manager, routerservice...
    //                                       UID  ObjId
    iStaticObjIdTable.Append( new TStaticId( 123, 0x70 ) ); //  for testing ISI API
    iStaticObjIdTable.Append( new TStaticId( 200, 0x71 ) ); //  for testing ISI API
    iStaticObjIdTable.Append( new TStaticId( 201, 0x72 ) ); //  for testing ISI API
    iStaticObjIdTable.Append( new TStaticId( 202, 0x73 ) ); //  for testing ISI API
    //
#ifndef WITHOUT_WRAPPERS_IN_USE
    // Map all the kernel channels as they used to be
    for( TInt id( KFirstUserChannel ); id < KLastKernelChannel; id++ )
        {
        if ( ( id != KNotInitializedId ) &&
             ( id != PN_OBJ_EVENT_MULTICAST ) &&
             ( id != PN_OBJ_EVENT_MULTICAST ) &&
             ( id != ROUTER_OBJECT_IDENTIFIER ) &&
             ( id != PIPEHANDLER_OBJECT_IDENTIFIER ) ) 
            {        
            iStaticObjIdTable.Append( new TStaticId( id, id ) );
            }
        else
            {
            iStaticObjIdTable.Append( new TStaticId( id, 0xEE ) );
            }
        }
    // Map all the kernel channels as they used to be
#endif // WITHOUT_WRAPPERS_IN_USE
    // ? vaikutus iShClientThreadContainer->DeallocateThread( iInitThread );

    iStaticObjIdTable.Append( new TStaticId( 0xD11BADA1, KNotInitializedId ) );// Make sure no one can get KNotInitializedId obj id
    iStaticObjIdTable.Append( new TStaticId( KCommunicationManagerUID, PN_OBJ_EVENT_MULTICAST ) );
    iStaticObjIdTable.Append( new TStaticId( KNameServiceUID, PN_OBJ_ROUTING_REQ ) );
    iStaticObjIdTable.Append( new TStaticId( KIsiShRouterServiceUID, ROUTER_OBJECT_IDENTIFIER ) );
    iStaticObjIdTable.Append( new TStaticId( KIsiShPipeHandlerUID, PIPEHANDLER_OBJECT_IDENTIFIER ) );
    
    iShCLTransceiver = new DISICLTransceiver( *this, iInitThread );
    ASSERT_RESET_ALWAYS( iShCLTransceiver, ( EISIRouterNULLPtr1 | EDISIRouterTraceId << KClassIdentifierShift ) ); 
    
    C_TRACE( ( _T( "DISIRouter::Initialize<" ) ) );
    }

DISIRouter::~DISIRouter(
        // None
        )
    {
    C_TRACE( ( _T( "DISIRouter::~DISIRouter>" ) ) );
    iSelfPtr = NULL;
    for( TUint8 i( 0 ); i < iStaticObjIdTable.Count(); i++)
        {
        delete iStaticObjIdTable[ i ];
        iStaticObjIdTable[ i ] = NULL;
        }
    iStaticObjIdTable.Reset();
    for( TInt i( 0 ); i <= KMaxAmountOfObjId; i++)
        {
        if( iClientTable[ i ] )
            {
            delete iClientTable[ i ];
            iClientTable[ i ] = NULL;
            }
        }
    // All must exist otherways reseted.
    delete iInitializeDfc;
    iInitializeDfc = NULL;
    delete iShClientThreadContainer;
    iShClientThreadContainer = NULL;
    delete iShCLTransceiver;
    iShCLTransceiver = NULL;
    delete iClientTableFastMutex;
    iClientTableFastMutex = NULL;
    C_TRACE( ( _T( "DISIRouter::~DISIRouter<" ) ) );
    }

TInt DISIRouter::Send(
        TDes8& aMessage,
        const TUint8 aObjId
        )
    {
    C_TRACE( ( _T( "DISIRouter::Send 0x%x 0x%x>" ), &aMessage, aObjId ) );

    if( ( ( aObjId == EIscNokiaUsbPhonetLink ) && ( GET_SENDER_DEV( aMessage.Ptr() ) == PN_DEV_PC ) ) ||
        ( aObjId == PN_OBJ_ROUTING_REQ ) || 
        ( aObjId == PN_OBJ_EVENT_MULTICAST ) ||
        ( aObjId == ROUTER_OBJECT_IDENTIFIER ) ||
        ( aObjId == PIPEHANDLER_OBJECT_IDENTIFIER ) ) 
        {
        // No need to update sender dev and obj id
        C_TRACE( ( _T( "DISIRouter::Send message tx address not needed to updata 0x%x 0x%x>" ), aObjId, GET_SENDER_DEV( aMessage.Ptr() ) ) );
        }
    else
        {
        TUint8* messageBlockPtr( const_cast<TUint8*>( aMessage.Ptr() ) );        
        SET_SENDER_DEV( messageBlockPtr, PN_DEV_OWN );
        SET_SENDER_OBJ( messageBlockPtr, aObjId );
        }
    TInt error = iShCLTransceiver->RouteISIMessage( aMessage );
    C_TRACE( ( _T( "DISIRouter::Send 0x%x 0x%x<" ), &aMessage, aObjId ) );
    return error;
    }

TBool DISIRouter::Receive( TDes8& aMessage, const TUint8 aObjId )
    {
    C_TRACE( ( _T( "DISIRouter::Receive 0x%x 0x%x>" ), &aMessage, aObjId ) );
    TBool error( EFalse );
    TUint8* messageBlockPtr( const_cast<TUint8*>( aMessage.Ptr() ) ); 

    NKern::FMWait( iClientTableFastMutex );
    if( iClientTable[ aObjId ] )
        {
        NKern::FMSignal( iClientTableFastMutex );
        ( iClientTable[ aObjId ]->iChannel )->Receive( aMessage ); //may not be safe, consider receive/delete sync
        C_TRACE( ( _T( "DISIRouter::Receive ok 0x%x 0x%x" ), &aMessage, aObjId ) );
        error = ETrue;
        }
    else
        {
        NKern::FMSignal( iClientTableFastMutex );
        C_TRACE( ( _T( "DISIRouter::Receive failed 0x%x 0x%x" ), &aMessage, aObjId ) );
        error = EFalse;
        }

    C_TRACE( ( _T( "DISIRouter::Receive 0x%x 0x%x %d<" ), &aMessage, aObjId, error ) );
    return error;

    }

DECLARE_STANDARD_EXTENSION()
    {
    Kern::Printf( "ISI Router kext>" );
    // Create a container extension
    DISIRouter* extension = new DISIRouter();
    TRACE_ASSERT( extension );
    Kern::Printf( "ISI Router kext 0x%x<", extension );
    return extension ? KErrNone : KErrNoMemory;
    }

DECLARE_EXTENSION_LDD()
    {
    Kern::Printf( "ISI Router kextldd>" );
    DLogicalDevice* device = new DISIDevice;
    TRACE_ASSERT( device );
    Kern::Printf( "ISI Router kextldd 0x%x<", device );
    return device;
    }
