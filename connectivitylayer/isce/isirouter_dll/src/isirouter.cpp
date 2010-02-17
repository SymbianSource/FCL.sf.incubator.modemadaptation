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
#include "misirouterobjectif.h"

#define PN_DEV_OWN 0x6C //TODO take from real location in future

const TInt KDfcPriority( 5 );
const TUint32 KCommunicationManagerUID( 0x2002B3D0 );
const TUint32 KNameServiceUID( 0x2002A5A1 );

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
    Kern::Printf( "ISIRouter::GetDfcThread" );
    return iClientThreadContainer->AllocateThread( aType );
    }

void DISIRouter::FreeDfcThread( TDfcQue* aThread )
    {
    C_TRACE( ( _T( "DISIRouter::FreeDfcThread 0x%x>" ), aThread ) );
    Kern::Printf( "ISIRouter::FreeDfcThread" );
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
    Kern::Printf( "ISIRouter::Send" );

    if( ( aObjId != PN_OBJ_ROUTING_REQ ) 
     && ( aObjId != PN_OBJ_EVENT_MULTICAST ) )
        {
        TUint8* messageBlockPtr( const_cast<TUint8*>( aMessage.Ptr() ) );
        SET_SENDER_DEV( messageBlockPtr, PN_DEV_OWN );
        SET_SENDER_OBJ( messageBlockPtr, aObjId );
        }
    TInt error = iCLTransceiver->RouteISIMessage( aMessage );
    C_TRACE( ( _T( "DISIRouter::Send 0x%x %d 0x%x<" ), &aMessage, aObjId ) );
    return error;
    }

TBool DISIRouter::Receive( TDes8& aMessage, const TUint8 aObjId )
    {
    C_TRACE( ( _T( "DISIRouter::Receive 0x%x 0x%x>" ), &aMessage, aObjId ) );
    TBool error( EFalse );
    TUint8* messageBlockPtr( const_cast<TUint8*>( aMessage.Ptr() ) ); 
    switch( GET_RECEIVER_OBJ( aMessage ) )
        {
        case PN_OBJ_ROUTING_REQ:
            {
            C_TRACE( ( _T( "DISIRouter msg to PN_OBJ_ROUTING_REQ: nameservice" ) ) );
            //route with resource and nameservice
            iNameService->Receive( aMessage );
            C_TRACE( ( _T( "DRouter::HandleIsiMessage to NAMESERVICE<" ) ) );
            error = ETrue;
            break;
            }
        case PN_OBJ_EVENT_MULTICAST:
            {
            C_TRACE( ( _T( "DISIRouter msg to PN_OBJ_EVENT_MULTICAST: communication manager" ) ) );
            iCommunicationManager->Receive( aMessage );
            C_TRACE( ( _T( "DRouter::HandleIsiMessage to Communication Manager<" ) ) );
            error = ETrue;
            break;
            }           
        default:
            {   //default regular channel
             NKern::FMWait( iClientTableFastMutex );
             if( iClientTable[ aObjId ] )
                {
                NKern::FMSignal( iClientTableFastMutex );
                ( iClientTable[ aObjId ]->iChannel )->ReceiveMsg( aMessage ); //may not be safe, consider receive/delete sync
                C_TRACE( ( _T( "DISIRouter::Receive ok 0x%x 0x%x>" ), &aMessage, aObjId ) );
                error = ETrue;
	              }
	          else
	              {
                NKern::FMSignal( iClientTableFastMutex );
                C_TRACE( ( _T( "DISIRouter::Receive failed 0x%x 0x%x<" ), &aMessage, aObjId ) );
                error = EFalse;
                }
            break;    
            }
         }   
     return error;

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

//From objectapi
EXPORT_C MISIObjectRouterIf* MISIObjectRouterIf::Connect( const TInt32 aUID, TUint8& aObjId, MISIRouterObjectIf* aCallback )
    {
    C_TRACE( ( _T( "MISIObjectRouterIf::Connect %d 0x%x 0x%x>" ), aUID, aObjId, aCallback ) );
    
    Kern::Printf( "ISIRouter::Connect" );
    if( aUID == KNameServiceUID )
        {
        C_TRACE( ( _T( "MISIObjectRouterIf was nameservice" ) ) );
        DISIRouter::iSelfPtr->iNameService = aCallback;
        aObjId = PN_OBJ_ROUTING_REQ; // 0x00
        }
    else if( aUID == KCommunicationManagerUID )
        {
        C_TRACE( ( _T( "MISIObjectRouterIf was communicationmanager" ) ) );
        DISIRouter::iSelfPtr->iCommunicationManager = aCallback;
        aObjId = PN_OBJ_EVENT_MULTICAST; // 0x20
        }
    else
        {
        C_TRACE( ( _T( "MISIObjectRouterIf unknown object api client" ) ) );
        }
    MISIObjectRouterIf* tmp =  DISIRouter::iSelfPtr;
    C_TRACE( ( _T( "MISIObjectRouterIf::Connect %d 0x%x 0x%x<" ), aUID, aObjId, aCallback ) );
    return tmp;
    }

