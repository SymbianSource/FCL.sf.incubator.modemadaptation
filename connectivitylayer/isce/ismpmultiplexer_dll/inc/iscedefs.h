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



#ifndef __ISCEDEFS_H__
#define __ISCEDEFS_H__

//  static configuration needs works
//  Separate protocol spesific parts from generic ones.
//  check dependenciest of. h files (definitions etc..)

// L2 Multiplexing protocol identifiers.
// Follow MeXe protocol
enum TPrototocolId
    {
    EMuxISI = 0x00,
    EMuxRpc,
    EMuxP2PTest,
    EMuxP2PTest2,
    EMuxAmountOfProtocols,
    };
    
#endif /* __ISCEDEFS_H__ */
