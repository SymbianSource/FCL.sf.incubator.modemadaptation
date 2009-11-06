/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for Security tests
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "security.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSecurity::CSecurity
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSecurity::CSecurity(
    CTestModuleIf& aTestModuleIf )
    : CCore( aTestModuleIf )
    {
    // Initial values
    }

// -----------------------------------------------------------------------------
// CSecurity::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSecurity::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iSecurityLog = CStifLogger::NewL(
        KSecurityLogPath,
        KSecurityLogFile,
        CStifLogger::ETxt,
        CStifLogger::EFile,
        EFalse );

    _LIT8( KSecurity, "CSecurity: ConstructL" );
    iSecurityLog->Log( ( TDesC8 ) KSecurity );

    iSecCode.Zero();
    iLockInfo.iStatus = RMobilePhone::EStatusLockUnknown;
    iLockInfo.iSetting = RMobilePhone::ELockSetUnknown;

    InitializeIscControlL( KIscControlTestCaseFileSecurity );
    }

// -----------------------------------------------------------------------------
// CSecurity::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSecurity* CSecurity::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CSecurity* self = new (ELeave) CSecurity( aTestModuleIf );

    CleanupStack::PushL( ( CScriptBase* ) self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CSecurity::~CSecurity()
    {
    // Delete resources allocated from test methods
    Delete();

    delete iSecurityLog;
    iLog = 0;
    }

// -----------------------------------------------------------------------------
// CSecurity::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CSecurity::RouteCompletion(
    TInt aIpc,
    CMmDataPackage* aDataPackage,
    TInt aResult )
    {
    // Process the ISI msg according to IPC requested.
    _LIT8( KResult, "CSecurity::RouteCompletion, IPC: %d, result: %d" );
    iSecurityLog->Log( ( TDesC8 ) KResult, aIpc, aResult );

    if ( !iHandleSpecifiedRequests )
        {
        switch ( aIpc )
            {
            case ECustomIsBlockedIPC:
                {
                CompleteIsBlocked( aResult, *aDataPackage );
                break;
                }
            case ECustomCheckSecurityCodeIPC:
                {
                CompleteCheckSecurityCode( aResult, *aDataPackage );
                break;
                }
            case ECustomCheckSecurityCodeCancelIPC:
                {
                CompleteCheckSecurityCodeCancel( aResult, *aDataPackage );
                break;
                }
            case ECustomDisablePhoneLockIPC:
                {
                CompleteDisablePhoneLock( aResult, *aDataPackage );
                break;
                }
            case EMmTsySimGetICCType:
                {
                CompleteSimGetICCType( aResult, *aDataPackage );
                break;
                }
            case EMmTsyBootNotifyModemStatusReadyIPC:
                {
                CompleteBootNotifyModemStatusReady( aResult, *aDataPackage );
                break;
                }
            case EMmTsyBootNotifySimStatusReadyIPC:
                {
                CompleteBootNotifySimStatusReady( aResult, *aDataPackage );
                break;
                }
            case EMmTsyBootNotifySecurityReadyIPC:
                {
                CompleteBootNotifySecurityReady( aResult, *aDataPackage );
                break;
                }
            case EMmTsyBootGetRFStatusIPC:
                {
                CompleteBootGetRFStatus( aResult, *aDataPackage );
                break;
                }
            case EMmTsyPhoneGetPin1DisableSupportedIPC:
                {
                CompletePhoneGetPin1DisableSupported( aResult, *aDataPackage );
                break;
                }
            case EMmTsySecurityGetSimActivePinStateIPC:
                {
                CompleteSecurityGetSimActivePinState( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneGetLockInfo:
                {
                CompleteGetLockInfo( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneNotifyLockInfoChange:
                {
                CompleteNotifyLockInfoChange( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneSetLockSetting:
                {
                CompleteSetLockSetting( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneChangeSecurityCode:
                {
                CompleteChangeSecurityCode( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneNotifySecurityEvent:
                {
                CompleteNotifySecurityEvent( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneVerifySecurityCode:
                {
                CompleteVerifySecurityCode( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneAbortSecurityCode:
                {
                CompleteAbortSecurityCode( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneGetFdnStatus:
                {
                CompleteGetFdnStatus( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneSetFdnSetting:
                {
                CompleteSetFdnStatus( aResult, *aDataPackage );
                break;
                }
            case EMobilePhoneGetSecurityCodeInfo:
                {
                SecurityInfoComplete(aResult, *aDataPackage);
                break;
                }
            case EMobilePhoneNotifySecurityCodeInfoChange:
                {
                CompleteSecurityCodeInfoChange(aResult, *aDataPackage);
                break;
                }
            //case ECustomSecurityDeliverCodeIPC:
                // never completed
            default:
                {
                _LIT8( KDefault, "CSecurity::RouteCompletion default" );
                iSecurityLog->Log( ( TDesC8 ) KDefault );
                break;
                }
            } // end switch
        } // if ( !iHandleSpecifiedRequests )
    else
        {
        TBool isIPCHandled = EFalse;

        switch ( aIpc )
            {
            case ECustomIsBlockedIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "ECustomIsBlockedIPC" ) ) )
                    {
                    CompleteIsBlocked( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case ECustomCheckSecurityCodeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "ECustomCheckSecurityCodeIPC" ) ) )
                    {
                    CompleteCheckSecurityCode( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case ECustomCheckSecurityCodeCancelIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "ECustomCheckSecurityCodeCancelIPC" ) ) )
                    {
                    CompleteCheckSecurityCodeCancel( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case ECustomDisablePhoneLockIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "ECustomDisablePhoneLockIPC" ) ) )
                    {
                    CompleteDisablePhoneLock( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsySimGetICCType:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMmTsySimGetICCType" ) ) )
                    {
                    CompleteSimGetICCType( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyBootNotifyModemStatusReadyIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMmTsyBootNotifyModemStatusReadyIPC" ) ) )
                    {
                    CompleteBootNotifyModemStatusReady( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyBootNotifySimStatusReadyIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMmTsyBootNotifySimStatusReadyIPC" ) ) )
                    {
                    CompleteBootNotifySimStatusReady( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyBootNotifySecurityReadyIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMmTsyBootNotifySecurityReadyIPC" ) ) )
                    {
                    CompleteBootNotifySecurityReady( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyBootGetRFStatusIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMmTsyBootGetRFStatusIPC" ) ) )
                    {
                    CompleteBootGetRFStatus( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyPhoneGetPin1DisableSupportedIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMmTsyPhoneGetPin1DisableSupportedIPC" ) ) )
                    {
                    CompletePhoneGetPin1DisableSupported( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsySecurityGetSimActivePinStateIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMmTsySecurityGetSimActivePinStateIPC" ) ) )
                    {
                    CompleteSecurityGetSimActivePinState( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneGetLockInfo:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneGetLockInfo" ) ) )
                    {
                    CompleteGetLockInfo( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneNotifyLockInfoChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneNotifyLockInfoChange" ) ) )
                    {
                    CompleteNotifyLockInfoChange( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneSetLockSetting:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneSetLockSetting" ) ) )
                    {
                    CompleteSetLockSetting( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneChangeSecurityCode:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneChangeSecurityCode" ) ) )
                    {
                    CompleteChangeSecurityCode( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneNotifySecurityEvent:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneNotifySecurityEvent" ) ) )
                    {
                    CompleteNotifySecurityEvent( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneVerifySecurityCode:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneVerifySecurityCode" ) ) )
                    {
                    CompleteVerifySecurityCode( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneAbortSecurityCode:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneAbortSecurityCode" ) ) )
                    {
                    CompleteAbortSecurityCode( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneGetFdnStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneGetFdnStatus" ) ) )
                    {
                    CompleteGetFdnStatus( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneSetFdnSetting:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneSetFdnSetting" ) ) )
                    {
                    CompleteSetFdnStatus( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneGetSecurityCodeInfo:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneGetSecurityCodeInfo" ) ) )
                    {
                    SecurityInfoComplete(aResult, *aDataPackage);
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneNotifySecurityCodeInfoChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare(
                         _L( "EMobilePhoneNotifySecurityCodeInfoChange" ) ) )
                    {
                    CompleteSecurityCodeInfoChange( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneGetCurrentActiveUSimApplication :
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneGetCurrentActiveUSimApplication" ) ) )
                    {
                    CompleteGetCurrentActiveUSIMApplication( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            //case ECustomSecurityDeliverCodeIPC:
                // never completed
            default:
                {
                _LIT8( KDefault, "CSecurity::RouteCompletion default" );
                iSecurityLog->Log( ( TDesC8 ) KDefault );
                break;
                }
            } // end switch
        // Check whether the IPC received should had been exactly the expected one.
        // If not the KErrGeneral is set.
        if( ( iNextIPCMustBeSameAsNextExpected ) &&
            ( !isIPCHandled ) )
            {
            aResult = KErrUnknown;
            Signal( aResult );
            }
        } // else
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

    return ( CScriptBase* ) CSecurity::NewL( aTestModuleIf );

    }

//  End of File
