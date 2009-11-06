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
#include "isirouter.h"            // For DISIRouter
#include "isiroutertrace.h"       // For C_TRACE, ASSERT_RESET.. and fault codes
#include "isidevice.h"            // For DLogicalDevice
#include "isihelpers.h"           // For SET_SENDER_DEV...
#include "misirouterchannelif.h"  // For MISIRouterChannelIf
#include "isithreadcontainer.h"   // For DISIThreadContainer
#include "isicltransceiver.h"     // For DISICLTransceiver

#define PN_DEV_APE 0x14 //TODO take from real location in future

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
    EISIRouterNULLThreadPointer,
    };

const TInt KDfcPriority( 5 );

void DISIRouter::Connect(
        const TInt32 aUID,
        TUint8& aObjId, 
        MISIRouterChannelIf* aCallback
        )
    {
   	C_TRACE( ( _T( "DISIRouter::Connect %d 0x%x 0x%x>" ), aUID, aObjId, aCallback ) );
    ASSERT_RESET_ALWAYS( aCallback, ( EISIRouterNULLPtr | EDISIRouterTraceId << KClassIdentifierShift ) );
    TISIClient* tmp = new TISIClient();
    ASSERT_RESET_ALWAYS( tmp, ( EISIRouterMemAllocFailure | EDISIRouterTraceId << KClassIdentifierShift ) );
    tmp->iUID     = aUID;
    tmp->iChannel = aCallback;
    
    TUint8 staticObjId = ReceiveStaticObjId( aUID );
    NKern::FMWait( iClientTableFastMutex );
    CheckUIDUniqueness( aUID );
    if( !staticObjId )
        {
        tmp->iObjId = ReserveNewDynamicObjId();
        ASSERT_RESET_ALWAYS( tmp->iObjId, ( EISIRouterNULLObjId | EDISIRouterTraceId << KClassIdentifierShift ) );
        }
    else
        {
        tmp->iObjId = staticObjId;
        }
    iClientTable[ tmp->iObjId ] = tmp;
    NKern::FMSignal( iClientTableFastMutex );
    aObjId = tmp->iObjId;
    C_TRACE( ( _T( "DISIRouter::Connect %d 0x%x 0x%x<" ), aUID, aObjId, aCallback ) );
    }

void DISIRouter::CheckUIDUniqueness( const TInt32 aUID )
    {
    // No tracing with mutex
    for( TInt i( KFirstAllowedObjId ); i < KMaxAmountOfObjId; i++ )
        {
  	    if( iClientTable[ i ]  && ( iClientTable[ i ]->iUID == aUID ) )
    	      {
    	      ASSERT_RESET_ALWAYS( 0, ( EISIRouterNotUniqueUID | EDISIRouterTraceId << KClassIdentifierShift ) );
    	      }
        }
    }

TUint8 DISIRouter::ReceiveStaticObjId( const TInt32 aUID )
    {
    C_TRACE( ( _T( "DISIRouter::ReceiveStaticObjId 0x%x 0x%x>" ), aUID ) );
    for( TUint8 i = 0; i < iStaticObjIdTable.Count(); i++ )
        {
        if( iStaticObjIdTable[ i ]->iUID == aUID )
            {
            C_TRACE( ( _T( "DISIRouter::ReceiveStaticObjId 0x%x 0x%x 0x%x<" ), aUID, iStaticObjIdTable[i]->iUID, iStaticObjIdTable[i]->iObjId ) );
            return iStaticObjIdTable[i]->iObjId;
            }
        }
    return KNotInitializedId;
    }

TUint8 DISIRouter::ReserveNewDynamicObjId()
    {
    // No tracing with mutex
	  for( TInt i( KFirstAllowedObjId ); i < KMaxAmountOfObjId; i++ )
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
    C_TRACE( ( _T( "DISIRouter::Disconnect 0x%x<" ), aObjId ) );
    }

TDfcQue* DISIRouter::GetDfcThread(
        const TISIDfcQThreadType aType
        )
    {
    C_TRACE( ( _T( "DISIRouter::GetDfcThread<>" ) ) );  
    return iClientThreadContainer->AllocateThread( aType );
    }

void DISIRouter::FreeDfcThread( TDfcQue* aThread )
    {
    C_TRACE( ( _T( "DISIRouter::FreeDfcThread 0x%x>" ), aThread ) );
    iClientThreadContainer->DeallocateThread( aThread );
    C_TRACE( ( _T( "DISIRouter::FreeDfcThread 0x%x<" ), aThread ) );
    }

MISIChannelRouterIf* MISIChannelRouterIf::GetIf()
    {
    C_TRACE( ( _T( "MISIChannelRouterIf::GetIf<>" ) ) );
    return DISIRouter::GetRouter();
    }

DISIRouter::DISIRouter(
        // None
        )
    {
    C_TRACE( ( _T( "DISIRouter::DISIRouter>" ) ) );
    iClientThreadContainer = new DISIThreadContainer();
    ASSERT_RESET_ALWAYS( iClientThreadContainer, ( EISIRouterMemAllocFailure1 | EDISIRouterTraceId << KClassIdentifierShift ) );
    iInitThread = iClientThreadContainer->AllocateThread( MISIChannelRouterIf::EISIKernelMainThread );
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
    C_TRACE( ( _T( "DISIRouter::Initialize>" ) ) );
    
    iCLTransceiver = new DISICLTransceiver( this );
    ASSERT_RESET_ALWAYS( iCLTransceiver, ( EISIRouterNULLPtr1 | EDISIRouterTraceId << KClassIdentifierShift ) ); //TODO error codes
    iClientTableFastMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iClientTableFastMutex, ( EISIRouterNULLPtr2 | EDISIRouterTraceId << KClassIdentifierShift ) );
    
    for( TInt i( 0 ); i < KMaxAmountOfObjId; i++)
        {
        iClientTable[ i ] = NULL;
        }
    //                                       UID  ObjId
    iStaticObjIdTable.Append( new TStaticId( 123, 15 ) ); //NokiaTSY etc...
    iStaticObjIdTable.Append( new TStaticId( 200, 34 ) );
    
    iSelfPtr = this;
    iClientThreadContainer->DeallocateThread( iInitThread );
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
    // All must exist
    iInitializeDfc->Cancel();
    delete iInitializeDfc;
    iInitializeDfc = NULL;
    delete iClientThreadContainer;
    iClientThreadContainer = NULL;
    delete iCLTransceiver;
    iCLTransceiver = NULL;
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
    TUint8* messageBlockPtr( const_cast<TUint8*>( aMessage.Ptr() ) );
    SET_SENDER_DEV( messageBlockPtr, PN_DEV_APE );
    SET_SENDER_OBJ( messageBlockPtr, aObjId );
    iCLTransceiver->RouteISIMessage( aMessage );
    C_TRACE( ( _T( "DISIRouter::Send 0x%x %d 0x%x<" ), &aMessage, aObjId ) );
    return KErrNone;
    }

TBool DISIRouter::Receive( TDes8& aMessage, const TUint8 aObjId )
    {
    C_TRACE( ( _T( "DISIRouter::Receive 0x%x 0x%x>" ), &aMessage, aObjId ) );
    NKern::FMWait( iClientTableFastMutex );
    if( iClientTable[ aObjId ] )
        {
        NKern::FMSignal( iClientTableFastMutex );
        ( iClientTable[ aObjId ]->iChannel )->ReceiveMsg( aMessage ); //may not be safe, consider receive/delete sync
        C_TRACE( ( _T( "DISIRouter::Receive ok 0x%x 0x%x>" ), &aMessage, aObjId ) );
        return ETrue;
    	  }
    NKern::FMSignal( iClientTableFastMutex );
    C_TRACE( ( _T( "DISIRouter::Receive failed 0x%x 0x%x<" ), &aMessage, aObjId ) );
    return EFalse;
    }

DECLARE_STANDARD_EXTENSION()
    {
    Kern::Printf( "ISI Router extension>" );
    // Create a container extension
    DISIRouter* extension = new DISIRouter();
    TRACE_ASSERT( extension );
    Kern::Printf( "ISI Router extension<" );
    return extension ? KErrNone : KErrNoMemory;
    }

DECLARE_EXTENSION_LDD()
    {
    Kern::Printf( "ISI Router ldd>" );
    DLogicalDevice* device = new DISIDevice;
    TRACE_ASSERT( device );
    Kern::Printf( "ISI Router ldd 0x%x<", device );
    return( device );
    }

