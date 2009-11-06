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



#include "isauserdevice.h"            // For DISAUserDevice
#include "isauserchannel.h"            // For DISAUserChannel
#include "iadtrace.h"                // For ASSERT_RESET.. and fault codes.
#include "iadinternaldefinitions.h"    // For KIADDriver...
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "isauserdeviceTraces.h"
#endif

// CONSTS
const TInt KMajorVersionNumber = 1;
const TInt KMinorVersionNumber = 0;
const TInt KBuildVersionNumber = 1;

EXPORT_C DISAUserDevice::DISAUserDevice(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DISAUSERDEVICE_DISAUSERDEVICE_ENTRY, ">DISAUserDevice::DISAUserDevice" );

    C_TRACE( ( _T( "DISAUserDevice::DISAUserDevice ->" ) ) );
    iVersion = TVersion( KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber );
    //iParseMask |= KDeviceAllowUnit;   // NOTE! this was on in ISC, forcing ISC to use extensionldd for > 32 channels?
    iParseMask |= KDeviceAllowInfo;    
    C_TRACE( ( _T( "DISAUserDevice::DISAUserDevice <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAUSERDEVICE_DISAUSERDEVICE_EXIT, "<DISAUserDevice::DISAUserDevice" );
    }

EXPORT_C DISAUserDevice::~DISAUserDevice(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DUP1_DISAUSERDEVICE_DISAUSERDEVICE_ENTRY, ">DISAUserDevice::~DISAUserDevice" );

    C_TRACE( ( _T( "DISAUserDevice::~DISAUserDevice ->" ) ) );
    C_TRACE( ( _T( "DISAUserDevice::~DISAUserDevice <-" ) ) );

    OstTrace0( TRACE_NORMAL, DUP1_DISAUSERDEVICE_DISAUSERDEVICE_EXIT, "<DISAUserDevice::~DISAUserDevice" );
    }

TInt DISAUserDevice::Install(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DISAUSERDEVICE_INSTALL_ENTRY, "<>DISAUserDevice::Install" );

    C_TRACE( ( _T( "DISAUserDevice::Install -><-" ) ) );
    return ( SetName( &KIADDriverName ) );

    }

void DISAUserDevice::GetCaps(
        TDes8& //aDes
        ) const
    {
    OstTrace0( TRACE_NORMAL, DISAUSERDEVICE_GETCAPS_ENTRY, ">DISAUserDevice::GetCaps" );
    C_TRACE( ( _T( "DISAUserDevice::GetCaps -><-" ) ) );
    OstTrace0( TRACE_NORMAL, DISAUSERDEVICE_GETCAPS_EXIT, "<DISAUserDevice::GetCaps" );

    }
// 
// context of the user thread
// thread in CS
//
TInt DISAUserDevice::Create(
        DLogicalChannelBase*& aChannel
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERDEVICE_CREATE_ENTRY, ">DISAUserDevice::Create;aChannel=%x", ( TUint )( aChannel ) );

    C_TRACE( ( _T( "DISAUserDevice::Create 0x%x ->" ), &aChannel ) );
    aChannel = new DISAUserChannel();
    C_TRACE( ( _T( "DISAUserDevice::Create 0x%x <-" ), &aChannel ) );    

    TInt retVal = aChannel ? KErrNone : KErrNoMemory;
    OstTrace1( TRACE_NORMAL, DISAUSERDEVICE_CREATE_EXIT, "DISAUserDevice::Create;retVal=%d", retVal );
    return retVal;    
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// Standard LDD entry point macro
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
DECLARE_STANDARD_LDD()
    {
    OstTrace0( TRACE_NORMAL, _DECLARE_STANDARD_LDD_ENTRY, ">DISAUserDevice    ::DECLARE_STANDARD_LDD" );

    C_TRACE( ( _T( "DISAUserDevice::DECLARE_STANDARD_LDD ->" ) ) );    
    DISAUserDevice* device = new DISAUserDevice;
    ASSERT_RESET_ALWAYS( device, EIADMemoryAllocationFailure | EIADFaultIdentifier23 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DISAUserDevice::DECLARE_STANDARD_LDD <-" ) ) );    
    
    OstTrace1( TRACE_NORMAL, _DECLARE_STANDARD_LDD_EXIT, "::DECLARE_STANDARD_LDD;device=%x", device );
    return device;    
    }
