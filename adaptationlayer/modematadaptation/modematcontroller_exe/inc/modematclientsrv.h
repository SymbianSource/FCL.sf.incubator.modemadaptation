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



#ifndef MODEMATCLIENTSRV_H
#define MODEMATCLIENTSRV_H

// Opcodes used in message passing between client and server
enum TModemAtIpc
    {
    EATExtSetExtensionInterface = 0,
    EModemATHandleCommand,
    EReceiveUnsolicitedResult,
    EReceiveUnsolicitedResultCancel,
    EModemATHandleCommandCancel,
    EModemATReceiveSignalInd,
    EModemATReceiveSignalIndCancel,
    EModemATGetNvramStatus,
    EModemATCommandMode,
    EModemATCommandModeCancel,
    EModemAtRemovePipe,
    EInvalidIpc
    };

// server name
_LIT( KATExtSrvName, "modematcontroller" );

// A version must be specifyed when creating a session with the server
const TUint KCommonServerMajorVersionNumber = 1;
const TUint KCommonServerMinorVersionNumber = 0;
const TUint KCommonServerBuildVersionNumber = 0;

// A version must be specifyed when creating a session with the server
const TUint KServerMajorVersionNumber = 1;
const TUint KServerMinorVersionNumber = 0;
const TUint KServerBuildVersionNumber = 0;

#endif  // MODEMATCLIENTSRV_H

