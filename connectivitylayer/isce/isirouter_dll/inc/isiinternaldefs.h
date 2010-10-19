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



#ifndef __ISIDEFS_H__
#define __ISIDEFS_H__

enum TISISyncRequest
    {
      EISIConnect = 0,
      EISIDisconnect,
      EISISend,
      EISIAllocateBlock,
      EISIDeallocateBlock,
      EISILastSyncRequest
    };

enum TISIAsyncRequest
    {
    	EISIAsyncReceive = EISILastSyncRequest,
      EISILastAsyncRequest
    };


_LIT( KISIDriverName, "ISIDriver" );

const TUint8 KNotInitializedId( 0xff );
const TInt32 KNotInitializedUID( 0x00 );
const TInt KNotInitializedStatus( 0xff );
const TInt KISILddRxQueueSize( 50 );
const TInt KISIMainRxQueueSize( 200 );
const TInt KMaxISIMsgSize( 65536 );
const TInt KMaxAmountOfObjId( 256 );
const TInt KFirstAllowedObjId( 1 );

#endif /* __ISIDEFS_H__ */
