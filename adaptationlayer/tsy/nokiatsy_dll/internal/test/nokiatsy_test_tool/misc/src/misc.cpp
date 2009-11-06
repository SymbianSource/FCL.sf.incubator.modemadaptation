/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for miscellaneous tests
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "misc.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMisc::CMisc
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMisc::CMisc(
    CTestModuleIf& aTestModuleIf )
    :CCore( aTestModuleIf )
    {
    // Initial values
    }

// -----------------------------------------------------------------------------
// CNWE::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMisc::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iMiscLog = CStifLogger::NewL( KMiscLogPath,
                          KMiscLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

     // This sets object type to baseclass to make possible to callback while
    // notification sequence
  //CCore::SetObjectType( CCore::EMisc, this);

    _LIT8(KMisc, "CMisc: ConstructL");
    iMiscLog->Log((TDesC8)KMisc );

    InitializeIscControlL(KIscControlTestCaseFile);

    }

// -----------------------------------------------------------------------------
// CNWE::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMisc* CMisc::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CMisc* self = new (ELeave) CMisc( aTestModuleIf );

    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CMisc::~CMisc()
    {
    // Delete resources allocated from test methods
    Delete();

    delete iMiscLog;
    iLog = 0;
    }

// -----------------------------------------------------------------------------
// CCore::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CMisc::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,
        TInt aResult
        )
    {

    // Process the ISI msg according to IPC requested.
    _LIT8(KResult, "CMisc::RouteCompletion result: %d IPC: %d");
    iMiscLog->Log((TDesC8)KResult, aResult, aIpc );

    TBool isIPCHandled = EFalse;
    switch(aIpc)
        {
        case EMobilePhoneNotifyBatteryInfoChange:
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneNotifyBatteryInfoChange" ) ) )
                {
                CompleteMobilePhoneNotifyBatteryInfoChange(aDataPackage, aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;

        case EMobilePhoneSetAPNControlListServiceStatus:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneSetAPNControlListServiceStatus" ) ) )
                {
                CompleteSetACLStatus(aDataPackage, aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }


        case EMobilePhoneGetAPNControlListServiceStatus:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetAPNControlListServiceStatus" ) ) )
                {
                CompleteGetACLStatus(aDataPackage, aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }

        case EMobilePhoneDeleteAPNName:
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneDeleteAPNName" ) ) )
                {
                CompleteDeleteACLEntry( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            
            break;

        case EMobilePhoneAppendAPNName:
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneAppendAPNName" ) ) )
                {
                CompleteWriteACLEntry( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;

        case EMobilePhoneEnumerateAPNEntries:
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneEnumerateAPNEntries" ) ) )
                {
                CompleteEnumerateAPNEntries( aDataPackage, aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;

        case EMobilePhoneGetAPNname:
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetAPNname" ) ) )
                {
                CompleteGetAPNname(aDataPackage, aResult);
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;

        case ECustomSetAlwaysOnMode:
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomSetAlwaysOnMode" ) ) )
                {
                CompleteSetAlwaysOnModeSelection ( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            
            break;

        default:
            {
            _LIT8(KDefault, "CMisc::RouteCompletion default");
            iMiscLog->Log((TDesC8)KDefault );
            break;
            }
        }
        // Check whether the IPC received should had been exactly the expected one.
        // If not the KErrGeneral is set.
        if( ( iNextIPCMustBeSameAsNextExpected ) &&
            ( !isIPCHandled ) )
            {
            aResult = KErrUnknown;
            Signal( aResult );
            }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CMisc::NewL( aTestModuleIf );

    }

//  End of File
