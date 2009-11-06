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



#include "p2pdevice.h"          // For DP2PDevice
#include "p2puserchannel.h"     // For DP2PUserChannel
#include "p2proutertrace.h"     // For C_TRACE..
#include "p2pinternaldefs.h"    // For KP2PDriverName

// CONSTS
const TInt KMajorVersionNumber = 0;
const TInt KMinorVersionNumber = 1;
const TInt KBuildVersionNumber = 1;

DP2PDevice::DP2PDevice(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PDevice::DP2PDevice>" ) ) );
    iVersion = TVersion( KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber );
    //iParseMask |= KDeviceAllowUnit;   // NOTE! allows over 32-channel objects
    iParseMask |= KDeviceAllowInfo;
    C_TRACE( ( _T( "DP2PDevice::DP2PDevice<" ) ) );

    }

DP2PDevice::~DP2PDevice(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PDevice::~DP2PDevice<>" ) ) );

    }

TInt DP2PDevice::Install(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PDevice::Install<>" ) ) );
    return ( SetName( &KP2PDriverName ) );

    }

void DP2PDevice::GetCaps(
        TDes8& //aDes
        ) const
    {

    C_TRACE( ( _T( "DP2PDevice::GetCaps><-" ) ) );

    }

// In context of the user thread, thread in CS.
TInt DP2PDevice::Create(
        DLogicalChannelBase*& aChannel
        )
    {

    C_TRACE( ( _T( "DP2PDevice::Create 0x%x>" ), &aChannel ) );
    aChannel = new DP2PUserChannel();
    TInt retVal = aChannel ? KErrNone : KErrNoMemory;
    C_TRACE( ( _T( "DP2PDevice::Create 0x%x %d<" ), &aChannel, retVal ) );
    return retVal;

    }
