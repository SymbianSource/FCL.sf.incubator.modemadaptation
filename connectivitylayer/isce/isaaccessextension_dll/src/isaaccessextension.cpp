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


#include <kernel.h>                 // For Kern
#include "isaaccessextension.h"
#include "iadtrace.h"               // For C_TRACE..
#include "router.h"                 // For DRouter

// CONSTS
// Priorities to DFC queues
const TInt KIADLddPrioriAdd( 0 );
const TInt KIADExtensionPrioriAdd( 0 );

DRouter* DIsaAccessExtension::iRouter = NULL;
TDfcQue* DIsaAccessExtension::iDfcQueueList[ EIADSizeOfDfcThreadList ] = { NULL, NULL };

// Internal fault codes.
enum TIADExtensionFaults
    {
    EIADDFCThreadCreationFailed = 1,
    EIADDFCThreadGetFailed = 2,
    };

DIsaAccessExtension::DIsaAccessExtension
        (
        // None
        )
    {

    C_TRACE( ( _T( "DIsaAccessExtension::DIsaAccessExtension ->" ) ) );
    // One above DFCQue0 (27)
    TInt defaultDfcPriority( 27 ); //TODO: NOTE!!! change to use DFCQue0 priority if possible //Kern::DfcQue0()->iThread->iPriority; NOTE!! IST prios
    // LDD DFC queue.
    TDfcQue* lddTDfcQueue = NULL;
    ASSERT_RESET_ALWAYS( KErrNone == Kern::DfcQCreate( lddTDfcQueue, defaultDfcPriority + KIADLddPrioriAdd, &KIADLddDfc ), EIADDFCThreadCreationFailed );
    iDfcQueueList[ EIADLddDfcQueue ] = lddTDfcQueue; 
    // Extension DFC queue.
    TDfcQue* extensionTDfcQueue = NULL;
    ASSERT_RESET_ALWAYS( KErrNone == Kern::DfcQCreate( extensionTDfcQueue, defaultDfcPriority + KIADExtensionPrioriAdd, &KIADExtensionDfc ), EIADDFCThreadCreationFailed );
    iDfcQueueList[ EIADExtensionDfcQueue ] = extensionTDfcQueue; 
    C_TRACE( ( _T( "DIsaAccessExtension::DIsaAccessExtension <-" ) ) );
    iRouter = new DRouter();
    ASSERT_RESET_ALWAYS( iRouter, EIADMemoryAllocationFailure | EIADFaultIdentifier4 << KFaultIdentifierShift );
//    _LIT8( KIADCOMPONAME, "ISA ACCESS DRIVER" );
    //BUILD_TRACE( KIADCOMPONAME() );

    }

DIsaAccessExtension::~DIsaAccessExtension
        (
        // None
        )
    {

    C_TRACE( ( _T( "DIsaAccessExtension::~DIsaAccessExtension ->" ) ) );
    // Never unloaded so no actual need for these, but we could test memory allocations
    // and deallocations if we destroy this extension in test code.
    delete iRouter;
    iRouter = NULL;
    for( TInt i( 0 ); i < EIADSizeOfDfcThreadList; ++i )
        {
        TDfcQue* dfcQueueToBeDeleted = GetDFCThread( static_cast< TIADDfcThread >( i ) );
        ASSERT_RESET_ALWAYS( dfcQueueToBeDeleted, EIADDFCThreadGetFailed | EIADFaultIdentifier1 << KFaultIdentifierShift );
        delete dfcQueueToBeDeleted;
        dfcQueueToBeDeleted = NULL;
        }
    iDfcQueueList[ EIADLddDfcQueue ] = NULL;
    iDfcQueueList[ EIADExtensionDfcQueue ] = NULL;
    C_TRACE( ( _T( "DIsaAccessExtension::~DIsaAccessExtension <-" ) ) );

    }


EXPORT_C MChannel2IADApi* DIsaAccessExtension::GetChannel2IADApi
        (
        // None
        )
    {

    C_TRACE( ( _T( "DIsaAccessExtension::::GetChannel2IADApi 0x%x <->" ), iRouter ) );
    return iRouter;

    }

EXPORT_C TDfcQue* DIsaAccessExtension::GetDFCThread
        (
        TIADDfcThread aDfcThread
        )
    {

    C_TRACE( ( _T( "DIsaAccessExtension::GetDFCThread <->" ) ) );
    ASSERT_RESET_ALWAYS( ( EIADLddDfcQueue == aDfcThread || EIADExtensionDfcQueue == aDfcThread ), EIADDFCThreadGetFailed | EIADFaultIdentifier2 << KFaultIdentifierShift );
    // Ownership not given to caller.
    return iDfcQueueList[ EIADExtensionDfcQueue ];//SMPSAFE

    }

DECLARE_STANDARD_EXTENSION()
    {

    Kern::Printf( "ISA Access Extension" );
    C_TRACE( ( _T( "DIsaAccessExtension EntryPoint ->" ) ) );
    // Create a container extension
    DIsaAccessExtension* extension = new DIsaAccessExtension();
    TRACE_ASSERT( extension );
    C_TRACE( ( _T( "DIsaAccessExtension EntryPoint <-" ) ) );
    return extension ? KErrNone : KErrNoMemory;

    }
