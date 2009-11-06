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



#include "isidevice.h"          // For DISIDevice
#include "isiroutertrace.h"     // For C_TRACE, ASSERT_RESET.. and fault codes
#include "isiuserchannel.h"     // For DISIUserChannel
#include "isiinternaldefs.h"    // For KISIDriverName

// CONSTS
const TInt KMajorVersionNumber = 0;
const TInt KMinorVersionNumber = 1;
const TInt KBuildVersionNumber = 1;


DISIDevice::DISIDevice(
        // None
        )
    {
    C_TRACE( ( _T( "DISIDevice::DISIDevice>" ) ) );
    iVersion = TVersion( KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber );
    iParseMask |= KDeviceAllowInfo;
    C_TRACE( ( _T( "DISIDevice::DISIDevice<" ) ) );

    }

DISIDevice::~DISIDevice(
        // None
        )
    {
    C_TRACE( ( _T( "DISIDevice::~DISIDevice<>" ) ) );
    }

TInt DISIDevice::Install(
        // None
        )
    {
    C_TRACE( ( _T( "DISIDevice::Install<>" ) ) );
    return ( SetName( &KISIDriverName ) );
    }

void DISIDevice::GetCaps(
        TDes8& //aDes
        ) const
    {
    C_TRACE( ( _T( "DISIDevice::GetCaps><-" ) ) );
    }

// In context of the user thread, thread in CS.
TInt DISIDevice::Create(
        DLogicalChannelBase*& aChannel
        )
    {
    C_TRACE( ( _T( "DISIDevice::Create 0x%x>" ), &aChannel ) );
    aChannel = new DISIUserChannel();
    TInt retVal = aChannel ? KErrNone : KErrNoMemory;
    C_TRACE( ( _T( "DISIDevice::Create 0x%x %d<" ), &aChannel, retVal ) );
    return retVal;
    }
