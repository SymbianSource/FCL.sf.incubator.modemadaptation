/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for NWE tests
*
*/



// INCLUDE FILES
#include <stiftestinterface.h>
#include "nwe.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNWE::CNWE
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNWE::CNWE(
    CTestModuleIf& aTestModuleIf ):
        CCore( aTestModuleIf )
    {
    // Initial values
    iNWSelSetting.iMethod = RMobilePhone::ENetworkSelectionUnknown;
    iRegistrationStatus = RMobilePhone::ERegistrationUnknown;
    iNetworkStatus = RMobilePhone::ENetworkStatusUnknown;
    iCipherIndicatorStatus = NULL;
    iNetworkSecurityLevel = NULL;
    }

// -----------------------------------------------------------------------------
// CNWE::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNWE::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    // This sets object type to baseclass to make possible to callback while
    // notification sequence
    CCore::SetObjectType( CCore::ENWE, this);

    iNWELog = CStifLogger::NewL( KNWELogPath,
                          KNWELogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    _LIT8(KNWE, "CNWE: ConstructL");
    iNWELog->Log((TDesC8)KNWE );

    iNotifyList.FillZ( CCore::ENumberOfNotifys );

    InitializeIscControlL(KIscControlTestCaseFileNWE);
    }

// -----------------------------------------------------------------------------
// CNWE::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNWE* CNWE::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CNWE* self = new (ELeave) CNWE( aTestModuleIf );

    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CNWE::~CNWE
// Destructor
// -----------------------------------------------------------------------------
//
CNWE::~CNWE()
    {
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iNWELog;
    }

// -----------------------------------------------------------------------------
// CCore::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CNWE::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,
        TInt aResult
        )
    {
    // Process the ISI msg according to IPC requested.
    _LIT8(KResult, "CNWE::RouteCompletion result: %d");
    iNWELog->Log((TDesC8)KResult, aResult );
    if ( !iHandleSpecifiedRequests )
        {
        switch(aIpc)
            {
            case EMobilePhoneSetNetworkSelectionSetting:
                {
                CompleteSetNetworkSelectionSetting( aResult );
                break;
                }
            case EMobilePhoneGetHomeNetwork:
                {
                CompleteGetHomeNetwork(aResult, aDataPackage );
                break;
                }
            case EMobilePhoneGetNetworkRegistrationStatus:
                {
                CompleteGetNetworkRegistrationStatus( aResult );
                break;
                }
            case EMobilePhoneNotifyNetworkRegistrationStatusChange:
                {
                NotifyNetworkRegistrationStatusChange( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneNotifyNetworkSelectionSettingChange:
                {
                NotifyNetworkSelectionSettingChange( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneNotifyCurrentNetworkChange:
                {
                if( !iNetworkModeTestOnGoing )
                    {
                    NotifyCurrentNetworkChange( aResult, aDataPackage );
                    }
                else
                    {
                    SetNetworkInfo( aResult, aDataPackage, EMobilePhoneNotifyCurrentNetworkChange );
                    }
                break;
                }
            case EMobilePhoneNotifyModeChange:
                {
                if( !iNetworkModeTestOnGoing )
                    {
                    NotifyModeChange( aResult, aDataPackage  );
                    }
                else
                    {
                    SetNetworkInfo( aResult, aDataPackage, EMobilePhoneNotifyModeChange );
                    }
                break;
                }
            case EMobilePhoneGetCipheringIndicatorStatus:
                {
                CompleteGetCipheringIndicatorStatus( aResult, aDataPackage );
                break;
                }
            case ECustomGetCipheringInfoIPC:
                {
                CompleteCustomGetCipheringInfo( aResult, aDataPackage );
                break;
                }
            case ECustomNetWakeupIPC:
                {
                CompleteCustomNetWakeup( aResult );
                break;
                }
            case ECustomGetOperatorNameIPC:
                {
                CompleteCustomGetOperatorName( aResult, aDataPackage );
                break;
                }
            case ECustomResetNetServerIPC:
                {
                CompleteCustomResetNetworkRegistrationStatus( aResult );
                break;
                }
            case ECustomGetSystemNetworkModesIPC:
                {
                CompleteCustomGetSystemNetworkModes( aResult, aDataPackage );
                break;
                }
            case ECustomSetSystemNetworkModeIPC:
                {
                CompleteCustomSetSystemNetworkMode( aResult );
                break;
                }
            case ECustomGetCurrentSystemNetworkModesIPC:
                {
                CompleteCustomGetCurrentSystemNetworkModes( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneNotifySignalStrengthChange:
                {
                NotifySignalStrengthChange( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneNotifyNITZInfoChange:
                {
                NotifyNITZInfoChange( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneNotifyNetworkSecurityLevelChange:
                {
                NotifyNetworkSecurityLevelChange( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneGetDetectedNetworksV2Phase1:
                {
                CompleteManualNetworkSearch( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneGetDetectedNetworksCancel:
                {
                CompleteManualNetworkSearchCancel( aResult );
                break;
                }
            case EMobilePhoneSelectNetwork:
                {
                CompleteManualNetworkSelection( aResult );
                break;
                }
            case EMobilePhoneSelectNetworkCancel:
                {
                CompleteManualNetworkSelectionCancel( aResult );
                break;
                }
            case ECustomNotifyNetworkConnectionFailureIPC:
                {
                CompleteNotifyNetworkConnectionFailure( aResult);
                break;
                }
            case EMobileCallNotifyMobileCallStatusChange:
                {
                CompleteCallStatusChange( aResult, aDataPackage );
                break;
                }

    // not suppoted for S60 ver 3.2
    #if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

            case ECustomSetBandSelectionIPC:
                {
                CompleteCustomSetBandSelection( aResult );
                break;
                }
            case ECustomGetBandSelectionIPC:
                {
                CompleteCustomGetBandSelection(
                    aResult, aDataPackage );
                break;
                }
    #endif //NCP_COMMON_S60_VERSION_SUPPORT

            default:
                {
                _LIT8(KDefault, "CNWE::RouteCompletion default");
                iNWELog->Log((TDesC8)KDefault );
                break;
                }
            }
        } // End of if ( !iHandleSpecifiedRequests )
    else
        {
        TBool isIPCHandled = EFalse;
        switch(aIpc)
            {
            case ECustomGetCellInfoIPC:
            case ECustomNotifyCellInfoChangeIPC:
                {
                if ( ( 0 == iSavedIPCNameForComparison.Compare
                          ( _L( "ECustomGetCellInfoIPC" ) ) ) ||
                     ( 0 == iSavedIPCNameForComparison.Compare
                          ( _L( "ECustomNotifyCellInfoChangeIPC" ) ) ) )
                    {
                    CompleteGetCellInfo( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneSetNetworkSelectionSetting:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneSetNetworkSelectionSetting" ) ) )
                    {
                    CompleteSetNetworkSelectionSetting( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneNotifyNetworkRegistrationStatusChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneNotifyNetworkRegistrationStatusChange" ) ) )
                    {
                    NotifyNetworkRegistrationStatusChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
                
            case EMobilePhoneNotifyNetworkSelectionSettingChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyNetworkSelectionSettingChange" ) ) )
                    {
                    NotifyNetworkSelectionSettingChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneNotifyCurrentNetworkChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneNotifyCurrentNetworkChange" ) ) )
                    {
                    if( !iNetworkModeTestOnGoing )
                        {
                        NotifyCurrentNetworkChange( aResult, aDataPackage );
                        }
                    else
                        {
                        SetNetworkInfo( aResult, aDataPackage, EMobilePhoneNotifyCurrentNetworkChange );
                        }
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneNotifyModeChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyModeChange" ) ) )
                    {
                    if( !iNetworkModeTestOnGoing )
                        {
                        NotifyModeChange( aResult, aDataPackage  );
                        }
                    else
                        {
                        SetNetworkInfo( aResult, aDataPackage, EMobilePhoneNotifyModeChange );
                        }
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneGetNetworkRegistrationStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetNetworkRegistrationStatus" ) ) )
                    {
                    CompleteGetNetworkRegistrationStatus( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneGetCipheringIndicatorStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetCipheringIndicatorStatus" ) ) )
                    {
                    CompleteGetCipheringIndicatorStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
                
            case ECustomGetCipheringInfoIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetCipheringInfoIPC" ) ) )
                    {
                    CompleteCustomGetCipheringInfo( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomNetWakeupIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomNetWakeupIPC" ) ) )
                    {
                    CompleteCustomNetWakeup( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomResetNetServerIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomResetNetServerIPC" ) ) )
                    {
                    CompleteCustomResetNetworkRegistrationStatus( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetSystemNetworkModesIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetSystemNetworkModesIPC" ) ) )
                    {
                    CompleteCustomGetSystemNetworkModes( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomSetSystemNetworkModeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomSetSystemNetworkModeIPC" ) ) )
                    {
                    CompleteCustomSetSystemNetworkMode( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetCurrentSystemNetworkModesIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetCurrentSystemNetworkModesIPC" ) ) )
                    {
                    CompleteCustomGetCurrentSystemNetworkModes( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneNotifyNITZInfoChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyNITZInfoChange" ) ) )
                    {
                    NotifyNITZInfoChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneNotifyNetworkSecurityLevelChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyNetworkSecurityLevelChange" ) ) )
                    {
                    NotifyNetworkSecurityLevelChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileCallNotifyPrivacyConfirmation:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobileCallNotifyPrivacyConfirmation" ) ) )
                    {
                    NotifyPrivacyConfirmation( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomNotifyCipheringInfoChangeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomNotifyCipheringInfoChangeIPC" ) ) )
                    {
                    NotifyCipheringInfoChangeIPC( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneGetDetectedNetworksCancel:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetDetectedNetworksCancel" ) ) )
                    {
                    CompleteManualNetworkSearchCancel( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneSelectNetworkCancel:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneSelectNetworkCancel" ) ) )
                    {
                    CompleteManualNetworkSelectionCancel( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomNotifyNSPSStatusIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomNotifyNSPSStatusIPC" ) ) )
                    {
                    NotifyNSPSStatusIPC( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomNotifyNetworkConnectionFailureIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomNotifyNetworkConnectionFailureIPC" ) ) )
                    {
                    CompleteNotifyNetworkConnectionFailure( aResult);
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetOperatorNameIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetOperatorNameIPC" ) ) )
                    {
                    CompleteCustomGetOperatorName( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneGetHomeNetwork:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetHomeNetwork" ) ) )
                    {
                    CompleteGetHomeNetwork(aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneGetDetectedNetworksV2Phase1:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetDetectedNetworksV2Phase1" ) ) )
                    {
                    CompleteManualNetworkSearch( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneSelectNetwork:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneSelectNetwork" ) ) )
                    {
                    CompleteManualNetworkSelection( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )
            case ECustomSetBandSelectionIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomSetBandSelectionIPC" ) ) )
                    {
                    CompleteCustomSetBandSelection( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetBandSelectionIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetBandSelectionIPC" ) ) )
                    {
                    CompleteCustomGetBandSelection( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
#endif // ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

            default:
                {
                _LIT8(KDefault, "CNWE::RouteCompletion default");
                iNWELog->Log((TDesC8)KDefault );
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
        } // End of else
    }

// -----------------------------------------------------------------------------
// CCore::NotifyNWE
//
// -----------------------------------------------------------------------------
//
void CNWE::NotifyNWE( TNotifyList& aNotifyList )
    {
    _LIT8(KNotifyNWE, "CNWE::NotifyNWE");
    iNWELog->Log((TDesC8)KNotifyNWE );

    iNotifyList[aNotifyList] = 0x01;
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
    return ( CScriptBase* ) CNWE::NewL( aTestModuleIf );
    }

//  End of File
