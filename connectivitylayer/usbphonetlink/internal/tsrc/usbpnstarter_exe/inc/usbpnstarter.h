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
* Header file for USB Phonet Link starter.
*/

#ifndef _USBPNSTARTER_H_
#define _USBPNSTARTER_H_

#include <e32svr.h>       // For RDebug

// Define trace macros.
#ifdef _DEBUG
    #define _T( a ) _L( a )
    #define C_TRACE( a ) RDebug::Print a
#else // _DEBUG
    #define _T( a )
    #define C_TRACE( a )
#endif // _DEBUG

#endif // _USBPNSTARTER_H_

// End of File
