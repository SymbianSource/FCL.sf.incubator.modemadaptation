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



#ifndef __P2PDEFS_H__
#define __P2PDEFS_H__

// Each P2P protocol can have only one channel either it is in user or in kernel side.
// ProtocolIds are static so the enums and tables can be too.
// Match these with multiplexing protocol ids in iscedefs.h
enum TP2PProtocol
    {
    EP2PRpc = 0x00,
    EP2PTest,
    EP2PTest2,
    EP2PAmountOfProtocols,
    };

#endif /* __P2PDEFS_H__ */
