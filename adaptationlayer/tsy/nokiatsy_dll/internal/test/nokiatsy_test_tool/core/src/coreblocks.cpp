/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class of all other test classes
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "cmmstaticutility.h"
#include "core.h"
#include "nwe.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCore::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CCore::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CCore::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCore::RunMethodL(
    CStifItemParser& /*aItem*/ )
    {

    return( KErrNone );

    }


// -----------------------------------------------------------------------------
// CCore::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::RouteCompletion
        (
        TInt /*aIpc*/,
        CMmDataPackage* /*aDataPackage*/,
        TInt /*aResult*/
        )
    {
    _LIT8(KCore, "CORE: In RouteCompletion");
    iCoreLog->Log((TDesC8)KCore);
    }

// -----------------------------------------------------------------------------
// CCore::Notify
// Calls correct Notify
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::Notify( TNotifyList& aNotifyList )
	{
	_LIT8(KCoreNotify, "CORE: Notify");
    iCoreLog->Log((TDesC8)KCoreNotify);

	switch( ObjectType() )
    	{
    	case CCore::ECore:
    	    {

    	    break;
    	    }
    	case CCore::ENWE:
	        {
	        iNWE->NotifyNWE( aNotifyList );
	        break;
	        }
	    default:
	        {
            // Should never enter this block
           	_LIT8(KDefault, "Default block in CCore::Notify");
            iCoreLog->Log((TDesC8)KDefault);
            break;
	        }
    	}
	//return ret;
	}


// -----------------------------------------------------------------------------
// CCore::HandleRequest
// Forwards a request to DOS layer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::HandleRequestL
		(
		const TInt aIpc,
		const CMmDataPackage* aDataPackage
		)
	{
	TInt ret( KErrNone );

	_LIT8(KHandleRequest, "CCore::HandleRequest: %d");
    iCoreLog->Log((TDesC8)KHandleRequest, aIpc);

	ret = iMessageManager->HandleRequestL( aIpc, aDataPackage );

	return ret;
	}

// -----------------------------------------------------------------------------
// CCore::HandleRequest
// Forwards a request to DOS layer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::HandleRequestL
		(
		const TInt aIpc
		)
	{
	TInt ret( KErrNone );

	_LIT8(KHandleRequest, "CCore::HandleRequest: %d");
    iCoreLog->Log((TDesC8)KHandleRequest, aIpc);

	ret = iMessageManager->HandleRequestL( aIpc );

	return ret;
	}


// -----------------------------------------------------------------------------
// CCore::SetObjectType
// SetObjectType method function.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::SetObjectType ( TObjectType aType, TAny* aModule )
    {

   	_LIT8(KSetObjectType, "CORE: In SetObjectType");
    iCoreLog->Log((TDesC8)KSetObjectType);
    // Stores object type
    iObjectType = aType;

    // Stores pointer to object ( this must cast to right type )
    iObject = aModule;

    switch( iObjectType )
        {
        case EVoice:
            {
            iVoice = reinterpret_cast <CVoice*>( aModule );
            break;
            }
        case ENWE:
            {
            iNWE = reinterpret_cast <CNWE*>( aModule );
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCore::ObjectType
// ObjectType method function.
// returns derived object type
// -----------------------------------------------------------------------------
//
TInt CCore::ObjectType()
	{
	_LIT8(KObjectType, "CCore::ObjectType %d");
    iCoreLog->Log((TDesC8)KObjectType, iObjectType);
	return iObjectType;

	}


// -----------------------------------------------------------------------------
// CCore::RunIscTestCase
// Start ISC Test Control test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::RunIscTestCase( CStifItemParser& aItem )
    {
    TInt testCaseId(0);
    TInt ret = aItem.GetNextInt(testCaseId);
    if (KErrNone == ret)
        {
        RunIscControlTestCase(1, testCaseId);
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CCore::SyncIscTestCase
// Syncronize with ISC Test Control
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::SyncIscTestCase( void )
    {
    SyncIscControl();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined
// Start ISC Test Control test case when REAL_CELLMO_IN_USE is defined.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined( CStifItemParser& aItem )
    {
#ifdef REAL_CELLMO_IN_USE
    CStifItemParser& aItemForCompilation = aItem; // Get rid of compiler warning

    return KErrNone;
#else
    // Real CellMo is not in use - Run Isc Test Case.
    TInt testCaseId(0);
    TInt ret = aItem.GetNextInt(testCaseId);
    if (KErrNone == ret)
        {
        RunIscControlTestCase(1, testCaseId);
        }
    return ret;
#endif // REAL_CELLMO_IN_USE
    }


// -----------------------------------------------------------------------------
// CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined
// Syncronize with ISC Test Control when REAL_CELLMO_IN_USE is not defined.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined( void )
    {
#ifdef REAL_CELLMO_IN_USE
    return KErrNone;
#else
    // Real CellMo is not in use - Sync Isc Control.
    SyncIscControl();
    return KErrNone;
#endif // REAL_CELLMO_IN_USE
    }

// -----------------------------------------------------------------------------
// CCore::HandleSpecifiedRequests
// Sets iHandleSpecifiedRequests EFalse/ETrue
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::HandleSpecifiedRequests( CStifItemParser& aItem )
    {
    _LIT8 (KHandleSpecifiedRequests, "CCore::HandleSpecifiedRequests" );
    iCoreLog->Log((TDesC8)KHandleSpecifiedRequests);

    iHandleSpecifiedRequests = EFalse;

    TPtrC string;
    aItem.GetNextString ( string );
    if ( _L("YES") == string )
        {
        iHandleSpecifiedRequests = ETrue;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::InitializeIPCRequestTable
// Initializes iSavedIPCNameForComparisonTable and iSavedIPCNameForComparison
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::InitializeIPCRequestTable( void )
    {
    _LIT8 (KInitializeIPCRequestTable, "CCore::InitializeIPCRequestTable" );
    iCoreLog->Log((TDesC8)KInitializeIPCRequestTable);

    TInt counter;

    iSavedIPCNameForComparison.Zero();

    for ( counter = 0; counter < 150; counter++ )
        {
        iSavedIPCNameForComparisonTable[ counter ].Zero();
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::SetExpectedCompleteIPC
// Sets expected complete IPCs into table iSavedIPCNameForComparisonTable
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt CCore::SetExpectedCompleteIPC( CStifItemParser& aItem )
    {
    _LIT8 (KSetExpectedCompleteIPC, "CCore::SetExpectedCompleteIPC" );
    iCoreLog->Log((TDesC8)KSetExpectedCompleteIPC);

    TInt counter = 0;
    TPtrC string;
    aItem.GetNextString ( string );

    for ( counter = 0; counter < 150; counter++ )
        {
        // Save the first string if it not saved yet
        if ( ( 0 == iSavedIPCNameForComparison.Compare ( _L( "" ) ) ) &&
             ( 0 == counter ) )
            {
            iSavedIPCNameForComparison.Append( string );
            }

        // Check the next free position and store the string into it.
        if ( 0 == iSavedIPCNameForComparisonTable[ counter ].Compare ( _L( "" ) ) )
            {
            iSavedIPCNameForComparisonTable[ counter ].Append( string );
            counter = 150;
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::GetNextExpectedCompleteIPC
// Gets expected complete IPCs from table iSavedIPCNameForComparisonTable
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt CCore::GetNextExpectedCompleteIPC( void )
    {
    _LIT8 (KGetNextExpectedCompleteIPC, "CCore::GetNextExpectedCompleteIPC" );
    iCoreLog->Log((TDesC8)KGetNextExpectedCompleteIPC);

    TInt counter = 0;
    TPtrC string;

    for ( counter = 0; counter < 150; counter++ )
        {
        //
        if ( iSavedIPCNameForComparison == iSavedIPCNameForComparisonTable[ counter ] )
            {
            iSavedIPCNameForComparisonTable[ counter ].Zero();
            // Get next IPC
            counter++;
            if ( 0 != iSavedIPCNameForComparisonTable[ counter ].Compare ( _L( "" ) ) )
                {
                iSavedIPCNameForComparison = iSavedIPCNameForComparisonTable[ counter ];
                }
            else
                // If there's no elements left in the table the comparison string is
                // initialized.
                {
                iSavedIPCNameForComparison.Zero();
                }
            counter = 150;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::SetOperInfoAPAC
// Setting Nokia_TSY's operator info IsOperinfoApac EFalse/ETrue
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt CCore::SetOperInfoAPAC( CStifItemParser& aItem )
    {
    _LIT8 (KSetOperInfoAPAC, "CCore::SetOperInfoAPAC" );
    iCoreLog->Log((TDesC8)KSetOperInfoAPAC);

    TPtrC string;
    aItem.GetNextString ( string );

    if ( _L("OperInfoAPAC") == string )
        {
        CMmStaticUtility::SetOperinfoApac ( ETrue );
        }
    else
        {
        CMmStaticUtility::SetOperinfoApac ( EFalse );
        }
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File

