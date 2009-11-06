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



#include <platform.h> // For DPlatChunkHw           
#include <nk_priv.h>  // For __ASSERT_NO_FAST_MUTEX (published to partners)

#include "memmanager.h"             // For MemManager
#include "memmanagertrace.h"        // For C_TRACE...

DMemManager* DMemManager::iThisptr = NULL;
TDfcQue* DMemManager::iDfcQueue = NULL;


// Memory management fault enumerations
enum TMemFault
   {
   EMemBlockAllocationFailed    = 0,
   EMemBlockSizeZero,
   EExtensionMemoryAllocationFailed,
   EDfcCreateFailed,
   EDfcQueueCreateFailed,
   EInvalidParameter,
   EPhysicalMemReleaseFailed,
   EMemoryAllocationFailed,
   EHWMemAllocFailed,
   EPhysicalMemAllocFailed,
   EInvalidQueueCount,
   EAllocNotThreadContext,
   EDeallocNotThreadContext,
   EMemBlockInvalidReleaseDetected
   };

/*
* Constructor.
*/
DMemManager::DMemManager()
    {

    C_TRACE( ( _T( "DMemManager::DMemManager>" ) ) );
    
    iFastMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iFastMutex, ( EMemoryAllocationFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
    
    //Priority must be larger than the threads calling alloc and dealloc
    Kern::DfcQCreate( iDfcQueue, 28, &KMemManagerDfcQThreadName );
    ASSERT_RESET_ALWAYS( ( iDfcQueue ), ( EDfcQueueCreateFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
      
    iPoolAllocateDfc = new TDfc( PoolAllocateDfc, this, iDfcQueue, KPoolAllocateDfcPriority );
    ASSERT_RESET_ALWAYS( ( iPoolAllocateDfc ), ( EDfcCreateFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
      
    iPoolDeleteDfc = new TDfc( PoolDeleteDfc, this, iDfcQueue , KPoolDeleteDfcPriority );
    ASSERT_RESET_ALWAYS( ( iPoolDeleteDfc ), ( EDfcCreateFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
   
    //Static array configuration
    iMemPond.Append( new DMemPool( 16, 384 ) );
    iMemPond.Append( new DMemPool( 32, 128 ) );
    iMemPond.Append( new DMemPool( 64, 64 ) );
    iMemPond.Append( new DMemPool( 128, 80 ) );
    iMemPond.Append( new DMemPool( 256, 60 ) );
    iMemPond.Append( new DMemPool( 2048, 100 ) );
    iMemPond.Append( new DMemPool( 4096, 100 ) );
    iMemPond.Append( new DMemPool( 65524, 4 ) );
    
    C_TRACE( ( _T( "DMemManager::DMemManager<" ) ) );
    }

/*
* Destructor.
*/
DMemManager::~DMemManager()
    {
    C_TRACE( ( _T( "DMemManager::~DMemManager>" ) ) );
      
    for( int i = 0; i < iMemPond.Count(); i++ )
        {
        delete iMemPond[i];
    	  }
    
    iMemPond.Reset();
        
    for( int i = 0; i < iPoolCreateQueue.Count(); i++ )
        {
        delete iPoolCreateQueue[i];
    	  }
    
    iPoolCreateQueue.Reset();
        	  
    for( TInt i = 0; i < iPoolDeleteQueue.Count(); i++ )
        {
        delete iPoolDeleteQueue[i];
    	  }
    
    iPoolDeleteQueue.Reset();
        
    delete iFastMutex;
    iFastMutex = NULL;
   
    C_TRACE( ( _T( "DMemManager::~DMemManager<" ) ) );
    }

/*
* DFC for dynamic pool allocation.
*/
void DMemManager::PoolAllocateDfc(
        TAny* aPtr  // Pointer to this object.
        )
    {
    C_TRACE( ( _T( "DMemManager::PoolAllocateDfc aPtr 0x%x>" ), aPtr ) );
            
    DMemManager& tmp = *reinterpret_cast<DMemManager*>( aPtr );
    
    NKern::FMWait( tmp.iFastMutex );
    
    ASSERT_RESET_ALWAYS( ( tmp.iPoolCreateQueue.Count() > 0 ), ( EInvalidQueueCount | EDMemmanagerTraceId << KClassIdentifierShift ) );
    TInt index = tmp.iMemPond.Find( tmp.iPoolCreateQueue[0] );
    ( tmp.iMemPond[ index ] )->iCopyPoolInUse = ETrue;
    tmp.iMemPond.Insert( (new DMemPool( tmp.iPoolCreateQueue[0]->iBlockSize, tmp.iPoolCreateQueue[0]->iBlockNum ) ), index );
    tmp.iPoolCreateQueue.Remove(0);
    
    NKern::FMSignal( tmp.iFastMutex );
    
    C_TRACE( ( _T( "DMemManager::PoolAllocateDfc<" ) ) );
    }

/*
* DFC for dynamic pool deletion.
*/
void DMemManager::PoolDeleteDfc(
        TAny* aPtr  // Pointer to this object.
        )
    {
    C_TRACE( ( _T( "DMemManager::PoolDeleteDfc aPtr 0x%x>" ), aPtr ) );
            
    DMemManager& tmp = *reinterpret_cast<DMemManager*>( aPtr );
    
    NKern::FMWait( tmp.iFastMutex );
    
    ASSERT_RESET_ALWAYS( ( tmp.iPoolDeleteQueue.Count() > 0 ), ( EInvalidQueueCount | EDMemmanagerTraceId << KClassIdentifierShift ) );
    delete tmp.iPoolDeleteQueue[0];
    tmp.iPoolDeleteQueue.Remove(0);
 
    NKern::FMSignal( tmp.iFastMutex );
    
    C_TRACE( ( _T( "DMemManager::PoolDeleteDfc<" ) ) );
    }


/*
* Constructor.
*/
DMemManager::DMemPool::DMemPool( const TUint16 aUnitSize, const TUint16 aUnitNum ) :
    iAllocatedMemBlock(NULL), iFreeMemBlock(NULL), iMemoryArea(NULL), 
    iPoolSize( aUnitNum * ( aUnitSize + sizeof(struct sUnit) ) ), 
    iBlockSize( aUnitSize ), iBlockNum( aUnitNum ),
    iBlockUsage(0), iHwChunk(NULL), iPhysicalAddress(0)
    {
    C_TRACE( ( _T( "DMemManager::DMemPool::DMemPool aUnitSize 0x%x, aUnitNum 0x%x>" ), aUnitSize, aUnitNum ) );
	  
	  ASSERT_RESET_ALWAYS( ((aUnitSize || aUnitNum) != 0 ), ( EInvalidParameter | EDMemmanagerTraceId << KClassIdentifierShift ) );

#ifndef __WINS__	  
    TInt r = Epoc::AllocPhysicalRam( iPoolSize, iPhysicalAddress );
    ASSERT_RESET_ALWAYS( (  r == KErrNone ), ( EPhysicalMemAllocFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
  
    // Create chunk as: non accessible from user side and non-cached.

    r = DPlatChunkHw::New( iHwChunk, iPhysicalAddress, iPoolSize, EMapAttrSupRw | EMapAttrFullyBlocking );
    ASSERT_RESET_ALWAYS( ( r == KErrNone ), ( EHWMemAllocFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
   
    iMemoryArea = (TUint8*)(iHwChunk->LinearAddress());
    ASSERT_RESET_ALWAYS( ( iMemoryArea ), ( EPhysicalMemAllocFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
    
#else
    iMemoryArea = (TUint8*)Kern::Alloc( iPoolSize );
    //ASSERT_RESET_ALWAYS( (  r == KErrNone ), ( EPhysicalMemAllocFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
  
    // Create chunk as: non accessible from user side and non-cached.
   
    //iMemoryArea = (TUint8*)(iPoolSize/*->LinearAddress()*/);
    ASSERT_RESET_ALWAYS( ( iMemoryArea ), ( EPhysicalMemAllocFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
#endif // __WINS__
    
    for( TUint16 i = 0; i < aUnitNum; i++ )  //Link all mem unit . Create linked list.
        {
        struct sUnit *pCurUnit = (struct sUnit *)( iMemoryArea + i*(aUnitSize+sizeof(struct sUnit)) );
                                    
        pCurUnit->iPrev = NULL;
        pCurUnit->iNext = iFreeMemBlock;    //Insert the new unit at head.
            
        if(NULL != iFreeMemBlock)
            {
            iFreeMemBlock->iPrev = pCurUnit;
            }
                      
        pCurUnit->iMemPtr = new TPtr8( ((TUint8*)pCurUnit + sizeof(struct sUnit)) , 0, iBlockSize );
                     
        iFreeMemBlock = pCurUnit;        
        }
      
    iHighWaterMark = ( KPoolHighWaterLimit * aUnitNum ) / 100;

    C_TRACE( ( _T( "DMemManager::DMemPool::DMemPool<" ) ) );
    }

/*
* Destructor.
*/
DMemManager::DMemPool::~DMemPool()
    {
    C_TRACE( ( _T( "DMemManager::DMemPool::~DMemPool>" ) ) );
    
    for( TUint16 i = 0; i < iBlockNum; i++ )
        {
        struct sUnit *pCurUnit = (struct sUnit *)( iMemoryArea + i*(iBlockSize+sizeof(struct sUnit)) );
                   
        if( pCurUnit->iMemPtr )   
            {
            delete pCurUnit->iMemPtr;
            pCurUnit->iMemPtr = NULL;
            }
        }
#ifndef __WINS__        
    TInt r = Epoc::FreePhysicalRam( iPhysicalAddress, iPoolSize );
    ASSERT_RESET_ALWAYS( ( r == KErrNone ), ( EPhysicalMemReleaseFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
    
    iHwChunk->Close(NULL);
    
    iHwChunk = NULL;
#else

    Kern::Free( iMemoryArea );

#endif // __WINS__    
       
    C_TRACE( ( _T( "DMemManager::DMemPool::~DMemPool<" ) ) );
    }


/*
* Allocate memory unit.
*/
TPtr8* DMemManager::DMemPool::Alloc( const TUint16 aSize )
    {
	  C_TRACE( ( _T( "DMemManager::DMemPool::Alloc>" ) ) );
	  
	  ASSERT_RESET_ALWAYS( (iMemoryArea || iFreeMemBlock), ( EMemBlockAllocationFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
    
    struct sUnit *pCurUnit = iFreeMemBlock;

    iFreeMemBlock = pCurUnit->iNext;

    if( iFreeMemBlock )
        {
        iFreeMemBlock->iPrev = NULL;
        }

    pCurUnit->iNext = iAllocatedMemBlock;

    if( iAllocatedMemBlock )
        {
        iAllocatedMemBlock->iPrev = pCurUnit; 
        }

    iAllocatedMemBlock = pCurUnit;
    
    iBlockUsage++;
   
	  C_TRACE( ( _T( "DMemManager::DMemPool::Alloc<" ) ) );
    return iAllocatedMemBlock->iMemPtr;
    }


/*
* Free memory unit.
*/
TBool DMemManager::DMemPool::Free( const TUint8* aBlockAddress )
    {
    C_TRACE( ( _T( "DMemManager::DMemPool::Free>" ) ) );
	    
    struct sUnit *pCurUnit = (struct sUnit *)(aBlockAddress - sizeof(struct sUnit) );

    iAllocatedMemBlock = pCurUnit->iNext;
        
    if(NULL != iAllocatedMemBlock)
        {
        iAllocatedMemBlock->iPrev = NULL;
        }
        
    pCurUnit->iNext = iFreeMemBlock;

    if(NULL != iFreeMemBlock)
        {
        iFreeMemBlock->iPrev = pCurUnit;
        }

    iFreeMemBlock = pCurUnit;
    iFreeMemBlock->iMemPtr->Zero();
    iBlockUsage--;
    
    C_TRACE( ( _T( "DMemManager::DMemPool::Free<" ) ) );

    //If empty & ready to be deleted
    return ( iCopyPoolInUse && iBlockUsage == 0 ) ? ETrue : EFalse; 
 
    }

EXPORT_C TDes8& MemApi::AllocBlock( const TUint16 aSize )
    {
    C_TRACE( ( _T( "MemApi::AllocBlock 0x%x>" ), aSize ) );
  
    __ASSERT_NO_FAST_MUTEX;
    
    ASSERT_RESET_ALWAYS( ( aSize > 0 ), ( EMemBlockSizeZero | EDMemmanagerTraceId << KClassIdentifierShift ) );
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EAllocNotThreadContext | EDMemmanagerTraceId << KClassIdentifierShift ) );
  
    TPtr8* ptr( NULL );
  
    NKern::FMWait( DMemManager::iThisptr->iFastMutex );
 
    for( TUint8 i( 0 ); i < DMemManager::iThisptr->iMemPond.Count(); ++i )
        {
        if( aSize <= DMemManager::iThisptr->iMemPond[ i ]->iBlockSize )
            {
            ptr = DMemManager::iThisptr->iMemPond[i]->Alloc( aSize );
           
            if( DMemManager::iThisptr->iMemPond[i]->iBlockUsage > DMemManager::iThisptr->iMemPond[i]->iHighWaterMark )
                {          	                        
                DMemManager::iThisptr->iPoolCreateQueue.Append( DMemManager::iThisptr->iMemPond[i] );
                DMemManager::iThisptr->iPoolAllocateDfc->Enque();   
                }

            NKern::FMSignal( DMemManager::iThisptr->iFastMutex );
            break;
            }
        }
  
    ASSERT_RESET_ALWAYS( ptr, ( EMemBlockAllocationFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ptr->Size() == 0, ( EMemBlockInvalidReleaseDetected | EDMemmanagerTraceId << KClassIdentifierShift ) );
    
    C_TRACE( ( _T( "MemApi::AllocBlock 0x%x<" ), ptr ) );
    return *ptr;
    }


EXPORT_C void MemApi::DeallocBlock( TDes8& aBlock )
    {
    C_TRACE( ( _T( "MemApi::DeallocBlock aBlock 0x%x>" ), &aBlock ) );
    
    __ASSERT_NO_FAST_MUTEX;
    
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EDeallocNotThreadContext | EDMemmanagerTraceId << KClassIdentifierShift ) );
    
    TBool removePool = EFalse;
    NKern::FMWait( DMemManager::iThisptr->iFastMutex );
    
    for( TUint8 i( 0 ); i < DMemManager::iThisptr->iMemPond.Count(); ++i )
        {
        //Check if inside pools memory area
        if( ( (DMemManager::iThisptr->iMemPond[i]->iMemoryArea) < aBlock.Ptr() ) &&
            ( aBlock.Ptr() < (DMemManager::iThisptr->iMemPond[i]->iMemoryArea + DMemManager::iThisptr->iMemPond[i]->iPoolSize) ) )
            {
            removePool = DMemManager::iThisptr->iMemPond[i]->Free( aBlock.Ptr() );   
              
            if( removePool )
                {
                DMemManager::iThisptr->iPoolDeleteQueue.Append( DMemManager::iThisptr->iMemPond[i] );
                DMemManager::iThisptr->iMemPond.Remove( i );
                DMemManager::iThisptr->iPoolDeleteDfc->Enque();
                }
              
            break;
            }
        }
   
    NKern::FMSignal( DMemManager::iThisptr->iFastMutex );
   
    }

DECLARE_STANDARD_EXTENSION()
    {
    Kern::Printf( "Memory Manager Extension>" );
    DMemManager* extension = new DMemManager();
    ASSERT_RESET_ALWAYS( ( extension ), ( EExtensionMemoryAllocationFailed | EDMemmanagerTraceId << KClassIdentifierShift ) );
    DMemManager::iThisptr = static_cast< DMemManager* >( extension );
    Kern::Printf( "Memory Manager Extension<" );
    return KErrNone;
    }

//  End of File

