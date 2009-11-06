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



#ifndef __MEMAPI_H__
#define __MEMAPI_H__

// Headers
#include <e32def.h>         // For TInt
#include <e32cmn.h>         // For TDes8

/*
* Part of ISCEs internal MEM API.
* Static interface for for usage of Memory management services.
* Class spesific faults can be found from source file.
*/
class MemApi
    {

    public:

        /*
        * Allocates a block of minimum of size aSize.
        * Descriptor length shall be set to zero, but maximum length may be bigger than size given as parameter.
        * Memory content of the descriptor shall be filled with zeros until maximum length.
        * Memory allocated for the block shall be physically contigous, non-cached and word aligned.
        * If aSize is zero class spesific fault thrown.
        * If not possible to allocate a block class spesific fault thrown.
        * If not called in kernel thread context class spesific fault thrown.
        * If called with FM held or kernel unlocked asserts thrown according to OS macros.
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Kernel unlocked
        * @post Calling thread not blocked
        */
        IMPORT_C static TDes8& AllocBlock( const TUint16 aSize );

        /*
        * Deallocates a block given as parameter.
        * If block was already deallocated class spesific fault thrown.
        * If block was not from the memory management pool class spesific fault thrown.
        * If not called in kernel thread context class spesific fault thrown.
        * If called with FM held or kernel unlocked asserts thrown according to OS macros.
        * @pre Called always in kernel thread context
        * @pre No fastmutex held
        * @pre Kernel unlocked
        * @post Calling thread not blocked
        */
        IMPORT_C static void DeallocBlock( TDes8& aBlock );

    };

#endif // __MEMAPI_H__
