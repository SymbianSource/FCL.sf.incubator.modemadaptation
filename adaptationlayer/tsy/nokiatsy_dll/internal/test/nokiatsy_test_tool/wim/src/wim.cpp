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
* Description:   Specific class for WIM tests
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "wim.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWIM::CWIM
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWIM::CWIM(
    CTestModuleIf& aTestModuleIf ):
        CCore( aTestModuleIf )
    {
    // Initial values
    }

// -----------------------------------------------------------------------------
// CWIM::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWIM::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iWIMLog = CStifLogger::NewL( KSSLogPath,
                          KSSLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    InitializeIscControlL(KIscControlTestCaseFileWIM);

    _LIT8(KSS, "CWIM: ConstructL");
    iWIMLog->Log((TDesC8)KSS );

    }

// -----------------------------------------------------------------------------
// CWIM::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWIM* CWIM::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CWIM* self = new (ELeave) CWIM( aTestModuleIf );

    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CWIM::~CWIM()
    {

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iWIMLog;

    }

// -----------------------------------------------------------------------------
// CWIM::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CWIM::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,

        TInt aResult
        )
    {


    // Process the ISI msg according to IPC requested.
    _LIT8(KResult, "CWIM::RouteCompletion result: %d");
    iWIMLog->Log((TDesC8)KResult, aResult );

    if ( !iHandleSpecifiedRequests )
        {
        switch(aIpc)
            {
            case ECustomPowerSimOnIPC:
                {
                CompletePoweringSimOn( aResult );
                break;
                }
            case ECustomPowerSimOffIPC:
                {
                CompletePoweringSimOff( aResult );
                break;
                }
            case ECustomSimWarmResetIPC:
                {
                CompleteSIMWarmReset( aResult );
                break;
                }
            case ECustomGetATRIPC:
                {
                CompleteGetAnswerToResetStatus( aResult, aDataPackage );
                break;
                }
            case ECustomGetSimCardReaderStatusIPC:
                {
                CompleteGetSIMCardReaderStatus( aResult, aDataPackage );
                break;
                }
            case ECustomSendAPDUReqV2IPC:
                {
                CompleteSendAPDU( aResult, aDataPackage );
                break;
                }
            case ECustomSendAPDUReqIPC:
                {
                CompleteSendAPDURequest( aResult, aDataPackage );
                break;
                }

            default:
                {
                _LIT8(KDefault, "CWIM::RouteCompletion default");
                iWIMLog->Log((TDesC8)KDefault );
                break;
                }
            }
        }
    else
        {
        TBool isIPCHandled = EFalse;
        switch(aIpc)
            {
            case ECustomPowerSimOnIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomPowerSimOnIPC" ) ) )
                    {
                    CompletePoweringSimOn( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomPowerSimOffIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomPowerSimOffIPC" ) ) )
                    {
                    CompletePoweringSimOff( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomSimWarmResetIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomSimWarmResetIPC" ) ) )
                    {
                    CompleteSIMWarmReset( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetATRIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetATRIPC" ) ) )
                    {
                    CompleteGetAnswerToResetStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetSimCardReaderStatusIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetSimCardReaderStatusIPC" ) ) )
                    {
                    CompleteGetSIMCardReaderStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomSendAPDUReqV2IPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomSendAPDUReqV2IPC" ) ) )
                    {
                    CompleteSendAPDU( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomSendAPDUReqIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomSendAPDUReqIPC" ) ) )
                    {
                    CompleteSendAPDURequest( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            default:
                {
                _LIT8(KDefault, "CWIM::RouteCompletion default");
                iWIMLog->Log((TDesC8)KDefault );
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

    return ( CScriptBase* ) CWIM::NewL( aTestModuleIf );

    }

//  End of File
