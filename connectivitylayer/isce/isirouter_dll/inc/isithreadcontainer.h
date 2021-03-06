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

#ifndef __ISITHREADCONTAINER_H__
#define __ISITHREADCONTAINER_H__

#include "misiobjectrouterif.h"  // For MISIObjectRouterIf

const TUint8 KAmountOfKernelThreads( 1 );
const TUint8 KAmountOfUserThreads( 1 );

NONSHARABLE_CLASS( DISIThreadContainer ) : public DBase
    {

    public:
    
        DISIThreadContainer();
        ~DISIThreadContainer();
            
        TDfcQue* AllocateThread( const MISIObjectRouterIf::TISIDfcQThreadType aType );
        void DeallocateThread( TDfcQue* );
        
        TDfcQue* ReserveKernelThread();
        TDfcQue* ReserveUserThread();
        
    private:
        //  synch
        // Owned
        TDfcQue* iKClientDfcQueList[ KAmountOfKernelThreads ];
        TDfcQue* iUClientDfcQueList[ KAmountOfUserThreads ];
        TUint8   iKThreadOccupation[ KAmountOfKernelThreads ];
        TUint8   iUThreadOccupation[ KAmountOfUserThreads ];
        HBuf8*   iNameTable[ KAmountOfKernelThreads + KAmountOfUserThreads ];
    };

#endif /* __DISITHREADCONTAINER_H__ */
