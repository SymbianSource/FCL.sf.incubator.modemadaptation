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
* Description:   Specific class for MishPhone tests
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "miscphone.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCSDI::CMiscPhone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMiscPhone::CMiscPhone( 
    CTestModuleIf& aTestModuleIf ):
        CCore( aTestModuleIf )
    {
    // Initial values
    }

// -----------------------------------------------------------------------------
// CCSDI::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMiscPhone::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iMiscPhoneLog = CStifLogger::NewL( KSSLogPath, 
                          KSSLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );                  
                          
    _LIT8(KSS, "CMiscPhone: ConstructL");
    iMiscPhoneLog->Log((TDesC8)KSS );
    
    InitializeIscControlL(KIscControlTestCaseFileMiscPhone);
   
    }

// -----------------------------------------------------------------------------
// CMiscPhone::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMiscPhone* CMiscPhone::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMiscPhone* self = new (ELeave) CMiscPhone( aTestModuleIf );
    
    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    
    }
    
// Destructor
CMiscPhone::~CMiscPhone()
    { 
    
    // Delete resources allocated from test methods
    Delete();
    
    // Delete logger
    delete iMiscPhoneLog;

    }
    
// -----------------------------------------------------------------------------
// CMiscPhone::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//    
EXPORT_C void CMiscPhone::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,
        
        TInt aResult
        )
    {
    
    
    // Process the ISI msg according to IPC requested.
    _LIT8(KResult, "CMiscPhone::RouteCompletion result: %d");
    iMiscPhoneLog->Log((TDesC8)KResult, aResult );
    
    if ( !iHandleSpecifiedRequests )
        {
        switch(aIpc)
            {
            case EMobilePhoneGetCustomerServiceProfile:
                {
                CompleteGetCustomerServiceProfile( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneGetPhoneId:
                {
                CompleteGetPhoneId( aResult, aDataPackage );
                break;  
                }
            case ECustomSetDriveModeIPC:
                {
                CompleteSetDriveMode( aResult );
                break;
                }
    
            default:
                {
                _LIT8(KDefault, "CMiscPhone::RouteCompletion default");
                iMiscPhoneLog->Log((TDesC8)KDefault );
                break;
                }
            }
        }
    else
        {
        TBool isIPCHandled = EFalse;
        switch(aIpc)
            {
            case EMobilePhoneGetCustomerServiceProfile:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetCustomerServiceProfile" ) ) )
                    {
                    CompleteGetCustomerServiceProfile( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneGetPhoneId:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetPhoneId" ) ) )
                    {
                    CompleteGetPhoneId( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case ECustomSetDriveModeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomSetDriveModeIPC" ) ) )
                    {
                    CompleteSetDriveMode( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
    
            default:
                {
                _LIT8(KDefault, "CMiscPhone::RouteCompletion default");
                iMiscPhoneLog->Log((TDesC8)KDefault );
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
    
    return ( CScriptBase* ) CMiscPhone::NewL( aTestModuleIf );
        
    }
    
//  End of File
