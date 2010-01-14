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



#ifndef __P2PIDS_H__
#define __P2PIDS_H__

enum TP2PSyncRequest
    {
    EP2PClose = 1,
    EP2PSend,
    EP2PAllocateBlock,
    EP2PDeallocateBlock,
    EP2PLastSyncRequest
    };

enum TP2PAsyncRequest
    {
    EP2PAsyncReceive = EP2PLastSyncRequest,
    EP2PAsyncConnectionLost,
    EP2PAsyncOpen,
    EP2PLastAsyncRequest
    };

_LIT( KP2PDriverName, "P2PDriver" );
// We need binary type suffix because LDD FW loads .ldd suffix as assumption.
_LIT( KP2PDriverLddBinaryName, "p2prouter.dll" );
const TInt KP2PLddRxQueuSize( 50 );

#endif /* __P2PIDS_H__ */
