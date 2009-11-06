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


#ifndef _MEMMANAGER_H_
#define _MEMMANAGER_H_

#include <e32def.h>         // For TUint16
#include <e32cmn.h>         // For TDesC8
#include "memapi.h"         // For MemApi

// Forward declarations
class DPlatChunkHw;
class DMemPool;


const TUint8 KPoolHighWaterLimit( 80 );

const TInt KPoolAllocateDfcPriority( 7 );
const TInt KPoolDeleteDfcPriority( 1 );

_LIT( KMemManagerDfcQThreadName, "MemManagerDfcQueueThread" );

/*
* Class implementing the ISCEs internal MEM API.
*/
NONSHARABLE_CLASS( DMemManager ) : public DBase, public MemApi
{
    public:

        //Functions
        /*
        * Constructor
        */
        DMemManager();

        /*
        * Destructor
        */
        ~DMemManager();
        
        // From MemApi
        /*
        * See comments from MemApi.
        * Can be called in 1...N kernel thread contexts.
        */
        static TDes8& AllocBlock( const TUint16 aSize );

        /*
        * See comments from MemApi.
        * Can be called in 1...N kernel thread contexts.
        */
        static void DeallocBlock( TDes8& aBlock );
        

        // Internal data
        static DMemManager*         iThisptr;

        static TDfcQue*     iDfcQueue;

        static void PoolAllocateDfc( TAny* aPtr );
        static void PoolDeleteDfc( TAny* aPtr );
        
        TDfc* iPoolAllocateDfc;
        TDfc* iPoolDeleteDfc;
        
    private:
    
        /*
        * Internal class for handling memory pools consisting of linked lists.
        */      
        NONSHARABLE_CLASS( DMemPool ) : public DBase
        {
            private:
                            
                struct sUnit              //The type of the unit of linkedlist
                    {
                    struct sUnit *iPrev, *iNext;
                    TPtr8*  iMemPtr;
                    };

            public:

                DMemPool( const TUint16 aUnitSize, const TUint16 aUnitNum );
                ~DMemPool();

                TPtr8* Alloc( const TUint16 aSize ); //Allocate memory unit
                TBool Free( const TUint8* aBlockAddress );

            private:        

                //Manage all units with two linkedlist
                struct sUnit*    iAllocatedMemBlock; //Head pointer to Allocated linkedlist
                struct sUnit*    iFreeMemBlock;      //Head pointer to Free linkedlist

            public:

                //Memory pool features
                TUint8*    iMemoryArea;
                TInt       iPoolSize;
                TUint16    iBlockSize;
                TUint16    iBlockNum;
                TUint16    iBlockUsage;
                TUint16    iHighWaterMark;
                TBool      iCopyPoolInUse;
        
            private:
            
                //Environment related variables
                DPlatChunkHw*    iHwChunk;
                TPhysAddr        iPhysicalAddress;
    
    };


    public:
    
        NFastMutex*            iFastMutex;
        RArray<DMemPool*>      iMemPond;
        RArray<DMemPool*>      iPoolCreateQueue;
        RArray<DMemPool*>      iPoolDeleteQueue;

};

#endif // _MEMMANAGER_H_
