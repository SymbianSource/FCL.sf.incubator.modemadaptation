/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "nwe.h"

#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <mmlist.h>

// CONSTANTS
const TPtrC BooleanFlag[] =
    {
    _L("EFalse"),
    _L("ETrue")
    };

const TPtrC NetworkMode[] =
    {
    _L("ENetworkModeUnknown"),
    _L("ENetworkModeUnregistered"),
    _L("ENetworkModeGsm"),
    _L("ENetworkModeAmps"),
    _L("ENetworkModeCdma95"),
    _L("ENetworkModeCdma2000"),
    _L("ENetworkModeWcdma")
    };

const TPtrC NetworkStatus[] =
    {
    _L("ENetworkStatusUnknown"),
    _L("ENetworkStatusAvailable"),
    _L("ENetworkStatusCurrent"),
    _L("ENetworkStatusForbidden")
    };

const TPtrC RegistrationStatus[] =
    {
    _L("ERegistrationUnknown"),
    _L("ENotRegisteredNoService"),
    _L("ENotRegisteredEmergencyOnly"),
    _L("ENotRegisteredSearching"),
    _L("ERegisteredBusy"),
    _L("ERegisteredOnHomeNetwork"),
    _L("ERegistrationDenied"),
    _L("ERegisteredRoaming")
    };

const TPtrC MobilePhoneSelectionMethod[] =
    {
    _L("ENetworkSelectionUnknown"),
    _L("ENetworkSelectionAutomatic"),
    _L("ENetworkSelectionManual"),
    _L("ENetworkSelectionHomeOnly")
    };

const TPtrC TMobilePhoneBandClass[] =
    {
    _L("ENetworkBandClassUnknown"),
    _L("ENetworkBandClassAOnly"),
    _L("ENetworkBandClassBOnly"),
    _L("ENetworkBandClassAPreferred"),
    _L("ENetworkBandClassBPreferred")
    };

const TPtrC TMobilePhoneOperation[] =
    {
    _L("ENetworkOperationUnknown"),
    _L("ENetworkOperationAnalogOnly"),
    _L("ENetworkOperationDigitalOnly"),
    _L("ENetworkOperationAnalogPreferred"),
    _L("ENetworkOperationDigitalPreferred")
    };

const TPtrC TMobileCallCipheringIndicator[] =
    {
    _L("ECipheringDisplayOff"),
    _L("ECipheringDisplayOn")
    };

const TPtrC TOperatorNameType[] =
    {
    _L("EOperatorNameNitzFull"),
    _L("EOperatorNameNitzShort"),
    _L("EOperatorNameProgrammableUcs2"),
    _L("EOperatorNameProgrammableLatin"),
    _L("EOperatorNameHardcodedUcs2"),
    _L("EOperatorNameHardcodedLatin"),
    _L("EOperatorNameCountryMcn"),
    _L("EOperatorNameMccMnc"),
    _L("EOperatorNameFlexiblePlmn"),
    _L("EOperatorNameHighestPriority")
    };

const TPtrC TMobilePhoneNetworkSecurity[] =
    {
    _L("ECipheringOff"),
    _L("ECipheringGSM"),
    _L("ECipheringWCDMA"),
    _L("ECipheringCDMA")
    };

_LIT8( KLine, "---------------------------------" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNWE::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CNWE::Delete()
    {
    _LIT8 (KDelete, "CNWE::Delete" );
    iNWELog->Log((TDesC8)KDelete );
    }

// -----------------------------------------------------------------------------
// CNWE::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CNWE::RunMethodL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CNWE: RunMethodL" );
    iNWELog->Log( ( TDesC8 ) KLog );

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "RunIscTestCase",                       CCore::RunIscTestCase ),
        ENTRY( "SyncIscTestCase",                      CCore::SyncIscTestCase ),
        ENTRY( "RunIscTestCaseStartNTSYAfterTestToolStart",
                CCore::RunIscTestCaseStartNTSYAfterTestToolStart),
        ENTRY( "SyncIscTestCaseStartNTSYAfterTestToolStart", 
                CCore::SyncIscTestCaseStartNTSYAfterTestToolStart),
        ENTRY( "SetNetworkSelectionSetting",           CNWE::SetNetworkSelectionSettingL ),
        ENTRY( "GetHomeNetwork",                       CNWE::GetHomeNetworkL ),
        ENTRY( "GetNetworkRegistrationStatus",         CNWE::GetNetworkRegistrationStatusL ),
        ENTRY( "GetCipheringIndicatorStatus",          CNWE::GetCipheringIndicatorStatusL ),
        ENTRY( "CustomGetCipheringInfo",               CNWE::CustomGetCipheringInfoL ),
        ENTRY( "CustomNetWakeup",                      CNWE::CustomNetWakeupL ),
        ENTRY( "CustomGetOperatorName",                CNWE::CustomGetOperatorNameL ),
        ENTRY( "CustomResetNetworkRegistrationStatus", CNWE::CustomResetNetworkRegistrationStatusL ),
        ENTRY( "CustomGetSystemNetworkModes",          CNWE::CustomGetSystemNetworkModesL ),
        ENTRY( "CustomSetSystemNetworkMode",           CNWE::CustomSetSystemNetworkModeL ),
        ENTRY( "CustomGetCurrentSystemNetworkModes",   CNWE::CustomGetCurrentSystemNetworkModesL ),

// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )
        ENTRY( "CustomSetBandSelection",               CNWE::CustomSetBandSelectionL ),
        ENTRY( "CustomGetBandSelection",               CNWE::CustomGetBandSelectionL ),
#endif //NCP_COMMON_S60_VERSION_SUPPORT

        ENTRY( "ManualNetworkSearch",                  CNWE::ManualNetworkSearchL ),
        ENTRY( "ManualNetworkSearchCancel",            CNWE::ManualNetworkSearchCancelL ),
        ENTRY( "ManualNetworkSelection",               CNWE::ManualNetworkSelectionL ),
        ENTRY( "ManualNetworkSelectionCancel",         CNWE::ManualNetworkSelectionCancelL ),
        ENTRY( "NotifyNetworkConnectionFailure",       CNWE::NotifyNetworkConnectionFailure ),
        ENTRY( "GetNetworkInfo",                       CNWE::GetNetworkInfo ),
        ENTRY( "SetNetworkInfo",                       CNWE::SetNetworkInfo ),
        ENTRY( "WaitForCallStatusChange",              CNWE::WaitForCallStatusChange ),
        ENTRY( "RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined",
            CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined ),
        ENTRY( "SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined",
            CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined ),
        ENTRY( "SetOperInfoAPAC",                      CCore::SetOperInfoAPAC ),
        ENTRY( "GetCellInfo",                          CNWE::GetCellInfo ),
        ENTRY( "NotifyNWRegStatusChangeCompareParameterSetting",
            CNWE::NotifyNWRegStatusChangeCompareParameterSettingL ),
        ENTRY( "NotifyNetworkSelectionSettingChangeCompareParameterSetting",
            CNWE::NotifyNetworkSelectionSettingChangeCompareParameterSettingL ),
        ENTRY( "NotifyCurrentNetworkChangeCompareParameterSetting",
            CNWE::NotifyCurrentNetworkChangeCompareParameterSettingL ),
        ENTRY( "SetShortAndLongOperName",              CNWE::SetShortAndLongOperNameL ),
        ENTRY( "HandleSpecifiedRequests",              CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable",            CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC",               CCore::SetExpectedCompleteIPC),
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CNWE::SetNetworkSelectionSettingL
// SetNetworkSelection method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::SetNetworkSelectionSettingL( CStifItemParser& aItem )
    {
    _LIT8(KSetNetworkSelectionSetting, "CNWE::SetNetworkSelectionSettingL");
    iNWELog->Log((TDesC8)KSetNetworkSelectionSetting);

    TInt ret( KErrNotFound );
    TBool selection = ETrue;
    TPtrC string;
    aItem.GetNextString( string );

    // Set NetworkSelection
    if ( _L("Manual") == string )
        {
        iNWELog->Log( _L("NetworkSelectionManual.") );
        iNWSelSetting.iMethod = RMobilePhone::ENetworkSelectionManual;
        }
    else if ( _L("Automatic") == string )
        {
        iNWELog->Log( _L("NetworkSelectionAutomatic.") );
        iNWSelSetting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
        }
    else if ( _L("Unknown") == string )
        {
        iNWELog->Log( _L("NetworkSelectionUnknown.") );
        iNWSelSetting.iMethod = RMobilePhone::ENetworkSelectionUnknown;
        }
    else
        {
        iNWELog->Log( _L("Set NetworkSelection not possible: %d"), ret );
        selection = EFalse;
        }

    if ( selection )
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData(&iNWSelSetting);

        ret = HandleRequestL( EMobilePhoneSetNetworkSelectionSetting,
            &dataPackage );

        if ( !iHandleSpecifiedRequests )
            {
            iSetNetworkSelectionSettingOngoing = ETrue;
            }

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "Set NetworkSelectionSetting request send ok" );
            iNWELog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "Set NetworkSelectionSetting request send failed: %d" );
            iNWELog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteSetNetworkSelectionSetting
// Complete SetNetworkSelection method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteSetNetworkSelectionSetting( TInt aResult )
    {
    _LIT8( KResult, "CNWE::CompleteSetNetworkSelectionSetting result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( ( iSetNetworkSelectionSettingOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iSetNetworkSelectionSettingOngoing = EFalse;
        _LIT8( KResultComplete, "SetNetworkSelectionSetting Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::GetHomeNetworkL
// GetHomeNetwork method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::GetHomeNetworkL( CStifItemParser& aItem )
    {
    _LIT8(KGetHomeNetwork, "CNWE::GetHomeNetworkL");
    iNWELog->Log((TDesC8)KGetHomeNetwork);

    TInt ret( KErrNotFound );
    TPtrC string;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iSavedDisplayTagForComparison[0].Zero();
    aItem.GetNextString( string );

    // Hard coded APAC name string:
    // - Operator name: CSL
    // - Unicode name: 0x8A0A, 0x806F, 0x96FB, 0x4FE1
    // - MCC 454, MNC 18
    if ( _L("APAC_NAME") == string )
        {
        iSavedDisplayTagForComparison[0].Append( 0x8A0A );
        iSavedDisplayTagForComparison[0].Append( 0x806F );
        iSavedDisplayTagForComparison[0].Append( 0x96FB );
        iSavedDisplayTagForComparison[0].Append( 0x4FE1 );
        }
    else
        {
        iSavedDisplayTagForComparison[0].Append( string );
        }

    aItem.GetNextString( string );
    iSavedShortNameForComparison[0].Zero();

    if ( _L("NO_SHORT_NAME") != string )
        {
        iSavedShortNameForComparison[0].Append( string );
        }

    aItem.GetNextString( string );
    iSavedLongNameForComparison[0].Zero();

    if ( _L("NO_LONG_NAME") != string )
        {
        iSavedLongNameForComparison[0].Append( string );
        }

    ret = HandleRequestL( EMobilePhoneGetHomeNetwork );
    iGetHomeNetworkOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KOk, "GetHomeNetwork request send ok: %d" );
        iNWELog->Log((TDesC8)KOk, ret );
        }
    else
        {
        _LIT8( KFail, "GetHomeNetwork request send fails: %d" );
        iNWELog->Log((TDesC8)KFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteGetHomeNetwork
// Complete GetHomeNetwork method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteGetHomeNetwork(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CNWE::CompleteGetHomeNetwork result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData( iNWInfo );
        PrintCurrentNetworkInfo( iNWInfo );
        }
    else
        {
        _LIT8( KFail, "CompleteGetHomeNetwork Failed: %d" );
        iNWELog->Log((TDesC8)KFail, aResult );
        }

    if ( ( 0 != iSavedDisplayTagForComparison[0].Compare( _L( "DONT_CARE_NETWORK_NAME" ) ) ) &&
        ( KErrNone == aResult ) )
        {
        if ( ( iSavedDisplayTagForComparison[0] != iNWInfo.iDisplayTag ) ||
            ( iSavedShortNameForComparison[0] != iNWInfo.iShortName ) ||
            ( iSavedLongNameForComparison[0] != iNWInfo.iLongName ) )
            {
            _LIT8( KResultFail, "CompleteGetHomeNetwork - operator names do not match" );
            aResult = KErrGeneral;
            }
        }

    if ( ( iGetHomeNetworkOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iGetHomeNetworkOngoing = EFalse;
        _LIT8( KResultComplete, "GetHomeNetwork Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::PrintCurrentNetworkInfo
// Prints current network info
// -----------------------------------------------------------------------------
//
void CNWE::PrintCurrentNetworkInfo(
    RMobilePhone::TMobilePhoneNetworkInfoV5 aNetworkInfo )
    {
    iNWELog->Log((TDesC8)KLine );
    _LIT8( KText, "CURRENT NETWORK INFO" );
    iNWELog->Log((TDesC8)KText );
    _LIT8( KMode, "Mode: " );
    iNWELog->Log((TDesC8)KMode );
    iNWELog->Log( NetworkMode[aNetworkInfo.iMode] );
    _LIT8( KStatus, "Status: " );
    iNWELog->Log((TDesC8)KStatus );
    iNWELog->Log( NetworkStatus[aNetworkInfo.iStatus] );
    _LIT8( KCountry, "Country Code: " );
    iNWELog->Log((TDesC8)KCountry );
    iNWELog->Log( aNetworkInfo.iCountryCode );
    _LIT8( KNetworkId, "NetworkId: " );
    iNWELog->Log((TDesC8)KNetworkId );
    iNWELog->Log( aNetworkInfo.iNetworkId );
    _LIT8( KHsdpaCellIndicator, "HSDPA available in cell: " );
    iNWELog->Log((TDesC8)KHsdpaCellIndicator );
    iNWELog->Log( BooleanFlag[aNetworkInfo.iHsdpaAvailableIndicator] );
    _LIT8( KDisplayTag, "DisplayTag: " );
    iNWELog->Log((TDesC8)KDisplayTag );
    iNWELog->Log( aNetworkInfo.iDisplayTag );
    _LIT8( KShortName, "ShortName: " );
    iNWELog->Log((TDesC8)KShortName );
    iNWELog->Log( aNetworkInfo.iShortName );
    _LIT8( KLongName, "LongName: " );
    iNWELog->Log((TDesC8)KLongName );
    iNWELog->Log( aNetworkInfo.iLongName );
    iNWELog->Log((TDesC8)KLine );
    }

// -----------------------------------------------------------------------------
// CNWE::GetNetworkRegistrationStatusL
// GetNetworkRegistrationStatus method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::GetNetworkRegistrationStatusL( CStifItemParser& aItem )
    {
    _LIT8(KGetNetworkRegistrationStatus, "CNWE::GetNetworkRegistrationStatusL" );
    iNWELog->Log((TDesC8)KGetNetworkRegistrationStatus );

    TInt ret( KErrNotFound );
    iHSXPATestOngoing = EFalse;

    TPtrC string;

    aItem.GetNextString( string );
    iSavedDisplayTagForComparison[0].Zero();

    // Hard coded APAC name string:
    // - Operator name: CSL
    // - Unicode name: 0x8A0A, 0x806F, 0x96FB, 0x4FE1
    // - MCC 454, MNC 18
    if ( _L("APAC_NAME") == string )
        {
        iSavedDisplayTagForComparison[0].Append( 0x8A0A );
        iSavedDisplayTagForComparison[0].Append( 0x806F );
        iSavedDisplayTagForComparison[0].Append( 0x96FB );
        iSavedDisplayTagForComparison[0].Append( 0x4FE1 );
        }
    else
        {
        iSavedDisplayTagForComparison[0].Append( string );
        }

    aItem.GetNextString( string );
    iSavedShortNameForComparison[0].Zero();

    if ( _L("NO_SHORT_NAME") != string )
        {
        iSavedShortNameForComparison[0].Append( string );
        }

    aItem.GetNextString( string );
    iSavedLongNameForComparison[0].Zero();

    if ( _L("NO_LONG_NAME") != string )
        {
        iSavedLongNameForComparison[0].Append( string );
        }

    aItem.GetNextString( string );

    if ( _L("HSXPATest") == string )
        {
        iHSXPATestOngoing = ETrue;
        aItem.GetNextString( string );
        if ( _L("HSUPA") == string || _L("HSDPA") == string || _L("HSDPAandHSUPA") == string )
            {
            iSavedNetworkInfo.iHsdpaAvailableIndicator = ETrue;
            }
        else
            {
            iSavedNetworkInfo.iHsdpaAvailableIndicator = EFalse;
            }
        }

    ret = HandleRequestL( EMobilePhoneGetNetworkRegistrationStatus );
    iGetNetworkRegistrationStatusOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "GetNetworkRegistrationStatus request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "GetNetworkRegistrationStatus request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteGetNetworkRegistrationStatus
// Complete GetNetworkRegistrationStatus method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteGetNetworkRegistrationStatus( TInt aResult )
    {
    _LIT8(KResult, "CNWE::CompleteGetNetworkRegistrationStatus result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( ( iGetNetworkRegistrationStatusOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        if ( iHandleSpecifiedRequests )
            {
            if ( iHSXPATestOngoing )
                {
                if ( ( iSavedNetworkInfo.iHsdpaAvailableIndicator != iNetworkInfo.iHsdpaAvailableIndicator ) &&
                    ( KErrNone == aResult ) )
                    {
                    aResult = KErrGeneral;
                    }
                }
            else
                {
                if ( ( iNWRegStatusForComparison != iRegistrationStatus ) &&
                    ( KErrNone == aResult ) )
                    {
                    aResult = KErrGeneral;
                    }
                }
            }

        iGetNetworkRegistrationStatusOngoing = EFalse;
        _LIT8( KResultComplete, "GetNetworkRegistrationStatus Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyNWRegStatusChangeCompareParameterSettingL
// NotifyNWRegStatusChangeCompareParameterSetting method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::NotifyNWRegStatusChangeCompareParameterSettingL(
    CStifItemParser& aItem )
    {
    _LIT8( KNotifyNWRegStatusChangeCompareParameterSettingL,
        "CNWE::NotifyNWRegStatusChangeCompareParameterSettingL");
    iNWELog->Log((TDesC8)KNotifyNWRegStatusChangeCompareParameterSettingL);

    TPtrC string;

    iNWRegStatusForComparison = RMobilePhone::ERegistrationUnknown;
    iNetworkStatusForComparison = RMobilePhone::ENetworkStatusUnknown;

    aItem.GetNextString( string );

    if ( _L("ERegisteredOnHomeNetwork") == string )
        {
        iNWRegStatusForComparison = RMobilePhone::ERegisteredOnHomeNetwork;
        }
    else if ( _L("ERegistrationDenied") == string )
        {
        iNWRegStatusForComparison = RMobilePhone::ERegistrationDenied;
        }
    else if ( _L("ERegisteredRoaming") == string )
        {
        iNWRegStatusForComparison = RMobilePhone::ERegisteredRoaming;
        }

    aItem.GetNextString( string );

    if ( _L("ENetworkStatusCurrent") == string )
        {
        iNetworkStatusForComparison = RMobilePhone::ENetworkStatusCurrent;
        }

    aItem.GetNextString( string );

    if ( _L("ALLOW_ALSO_NOK_RESULT_FROM_TSY") == string )
        {
        iAllowAlsoNokStatusFromTSY = ETrue;
        }
    else
        {
        iAllowAlsoNokStatusFromTSY = EFalse;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyNetworkRegistrationStatusChange
// Notify Change of Network Registration Status
// -----------------------------------------------------------------------------
//
void CNWE::NotifyNetworkRegistrationStatusChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    if ( ( iNotifyList[ENotifyNetworkRegistrationStatusChange] ) ||
        ( iHandleSpecifiedRequests ) )
        {
        _LIT8(KRegStatusChange, "CNWE::NotifyNetworkRegistrationStatusChange" );
        iNWELog->Log((TDesC8)KRegStatusChange);

        if ( ( KErrNone == aResult ) ||
             ( iAllowAlsoNokStatusFromTSY ) )
            {
            aDataPackage->UnPackData( iRegistrationStatus, iNetworkStatus );
            iNWELog->Log((TDesC8)KLine );
            iNWELog->Log((TDesC8)KRegStatusChange);
            _LIT8(KRegStatus, "Registration Status: " );
            iNWELog->Log((TDesC8)KRegStatus );
            iNWELog->Log( RegistrationStatus[iRegistrationStatus] );
            _LIT8(KNetWorkStatus, "Network Status: " );
            iNWELog->Log((TDesC8)KNetWorkStatus );
            iNWELog->Log( NetworkStatus[iNetworkStatus] );
            iNWELog->Log((TDesC8)KLine );
            }
        else
            {
            _LIT8(KFailed, "NotifyNetworkRegistrationStatusChange Failed" );
            iNWELog->Log((TDesC8)KFailed);
            }
        }

    if ( iHandleSpecifiedRequests )
        {
        if ( ( iNWRegStatusForComparison != iRegistrationStatus ) ||
            ( iNetworkStatusForComparison != iNetworkStatus ) )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyNetworkSelectionSettingChangeCompareParameterSettingL
// NotifyNetworkSelectionSettingChangeCompareParameterSetting method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::NotifyNetworkSelectionSettingChangeCompareParameterSettingL(
    CStifItemParser& aItem )
    {
    _LIT8( KNetworkRegistrationStatusChangeCompareParameterSetting,
        "CNWE::NotifyNetworkSelectionSettingChangeCompareParameterSettingL");
    iNWELog->Log((TDesC8)KNetworkRegistrationStatusChangeCompareParameterSetting);

    TPtrC string;

    iNetworkSelectionForComparison.iMethod = RMobilePhone::ENetworkSelectionUnknown;
    iNetworkSelectionForComparison.iBandClass = RMobilePhone::ENetworkBandClassUnknown;
    iNetworkSelectionForComparison.iOperationMode = RMobilePhone::ENetworkOperationUnknown;

    aItem.GetNextString( string );

    if ( _L("ENetworkSelectionAutomatic") == string )
        {
        iNetworkSelectionForComparison.iMethod =
            RMobilePhone::ENetworkSelectionAutomatic;
        }
    else if ( _L("ENetworkSelectionUnknown") == string )
        {
        iNetworkSelectionForComparison.iMethod =
            RMobilePhone::ENetworkSelectionUnknown;
        }
    else if ( _L("ENetworkSelectionManual") == string )
        {
        iNetworkSelectionForComparison.iMethod =
            RMobilePhone::ENetworkSelectionManual;
        }

    aItem.GetNextString( string );

    if ( _L("ENetworkBandClassUnknown") == string )
        {
        iNetworkSelectionForComparison.iBandClass =
            RMobilePhone::ENetworkBandClassUnknown;
        }

    aItem.GetNextString( string );

    if ( _L("ENetworkOperationUnknown") == string )
        {
        iNetworkSelectionForComparison.iOperationMode =
            RMobilePhone::ENetworkOperationUnknown;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyNetworkSelectionSettingChange
// Notify Change of Network Selection Setting
// -----------------------------------------------------------------------------
//
void CNWE::NotifyNetworkSelectionSettingChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    if ( ( iNotifyList[ENotifyNetworkSelectionSettingChange] ) ||
        ( iHandleSpecifiedRequests ) )
        {
        _LIT8(KNotifyNWSelSetChange, "CNWE::NotifyNetworkSelectionSettingChange" );
        iNWELog->Log((TDesC8)KNotifyNWSelSetChange );

        if ( ( KErrNone == aResult ) ||
             ( iAllowAlsoNokStatusFromTSY ) )
            {
            aDataPackage->UnPackData( iNetworkSelection );

            iNWELog->Log((TDesC8)KLine );
            _LIT8(KSelectionSettingChange, "NotifyNetworkSelectionSettingChange" );
            iNWELog->Log((TDesC8)KSelectionSettingChange );
            _LIT8(KMethod, "Selection Method: " );
            iNWELog->Log((TDesC8)KMethod );
            iNWELog->Log( MobilePhoneSelectionMethod[iNetworkSelection.iMethod] );
            _LIT8(KBand, "Band Class: " );
            iNWELog->Log((TDesC8)KBand );
            iNWELog->Log( TMobilePhoneBandClass[iNetworkSelection.iBandClass] );
            _LIT8(KOperation, "Operation: " );
            iNWELog->Log((TDesC8)KOperation );
            iNWELog->Log( TMobilePhoneOperation[iNetworkSelection.iOperationMode] );
            iNWELog->Log((TDesC8)KLine );
            }
        else
            {
            _LIT8(KFailed, "NotifyNetworkSelectionSettingChange Failed" );
            iNWELog->Log((TDesC8)KFailed);
            }
        }

    if ( iHandleSpecifiedRequests )
        {
        if ( ( iNetworkSelection.iMethod != iNetworkSelectionForComparison.iMethod ) ||
            ( iNetworkSelection.iBandClass != iNetworkSelectionForComparison.iBandClass ) ||
            ( iNetworkSelection.iOperationMode != iNetworkSelectionForComparison.iOperationMode ) )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyCurrentNetworkChangeCompareParameterSettingL
// NotifyCurrentNetworkChangeCompareParameterSetting method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::NotifyCurrentNetworkChangeCompareParameterSettingL(
    CStifItemParser& aItem )
    {
    _LIT8(KNotifyCurrentNetworkChangeCompareParameterSetting,
        "CNWE::NotifyCurrentNetworkChangeCompareParameterSettingL");
    iNWELog->Log((TDesC8)KNotifyCurrentNetworkChangeCompareParameterSetting);

    aItem.GetNextInt( iLocationAreaForComparison.iAreaKnown );
    aItem.GetNextInt( iLocationAreaForComparison.iLocationAreaCode );
    aItem.GetNextInt( iLocationAreaForComparison.iCellId );

    TPtrC string;
    aItem.GetNextString( string );

    if ( _L("DONT_CARE_NETWORK_NAME") == string )
        {
        iSavedDisplayTagForComparison[0].Zero();
        iSavedDisplayTagForComparison[0].Append( string );
        iSavedShortNameForComparison[0].Zero();
        iSavedShortNameForComparison[0].Append( string );
        iSavedLongNameForComparison[0].Zero();
        iSavedLongNameForComparison[0].Append( string );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyCurrentNetworkChange
// NotifyChange of Current Network
// -----------------------------------------------------------------------------
//
void CNWE::NotifyCurrentNetworkChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {

    if ( ( iNotifyList[ENotifyCurrentNetworkChange] ) ||
        ( iHandleSpecifiedRequests ) )
        {
        _LIT8(KNotifyCurrentNetworkChange, "CNWE::NotifyCurrentNetworkChange" );
        iNWELog->Log((TDesC8)KNotifyCurrentNetworkChange );

        if ( KErrNone == aResult )
            {
            aDataPackage->UnPackData( iNetworkInfo, iLocationArea );

            PrintCurrentNetworkInfo( iNetworkInfo );

            iNWELog->Log((TDesC8)KLine );
            _LIT8(KLocationArea, "LOCATION AREA INFO" );
            iNWELog->Log((TDesC8)KLocationArea);
            _LIT8(KAreaKnown, "Area Known:" );
            iNWELog->Log((TDesC8)KAreaKnown );
            iNWELog->Log( BooleanFlag[iLocationArea.iAreaKnown] );
            _LIT8(KLocationAreaCode, "Location Area Code: %d" );
            iNWELog->Log((TDesC8)KLocationAreaCode, iLocationArea.iLocationAreaCode );
            _LIT8(KCellId, "Cell Id: %d" );
            iNWELog->Log((TDesC8)KCellId, iLocationArea.iCellId);
            iNWELog->Log((TDesC8)KLine );
            }
        else
            {
            _LIT8(Fail, "NotifyCurrentNetworkChange Failed" );
            iNWELog->Log((TDesC8)Fail);
            }
        }

    if ( ( 0 != iSavedDisplayTagForComparison[0].Compare( _L( "DONT_CARE_NETWORK_NAME" ) ) ) &&
        ( KErrNone == aResult ) )
        {
        if ( ( iSavedDisplayTagForComparison[0] != iNetworkInfo.iDisplayTag ) ||
            ( iSavedShortNameForComparison[0] != iNetworkInfo.iShortName ) ||
            ( iSavedLongNameForComparison[0] != iNetworkInfo.iLongName ) )
            {
            _LIT8( KResultFail, "NotifyCurrentNetworkChange - operator names do not match" );
            iNWELog->Log((TDesC8)KResultFail);
            aResult = KErrGeneral;
            }
        }

    if ( iHandleSpecifiedRequests )
        {
        if ( ( iLocationArea.iAreaKnown != iLocationAreaForComparison.iAreaKnown ) ||
            ( iLocationArea.iLocationAreaCode != iLocationAreaForComparison.iLocationAreaCode ) ||
            ( iLocationArea.iCellId != iLocationAreaForComparison.iCellId ) )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }

    if ( iNetworkModeTestOnGoing )
        {
        SetNetworkInfo( aResult, aDataPackage, EMobilePhoneNotifyCurrentNetworkChange );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyModeChange
// Notify Change of Mode
// -----------------------------------------------------------------------------
//
void CNWE::NotifyModeChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    if ( ( iNotifyList[ENotifyModeChange] ) ||
        ( iHandleSpecifiedRequests ) )
        {
        _LIT8(KNotifyModeChange, "CNWE::NotifyModeChange" );
        iNWELog->Log((TDesC8)KNotifyModeChange );

        if ( KErrNone == aResult )
            {
            aDataPackage->UnPackData( iNetworkMode );

            iNWELog->Log((TDesC8)KLine );
            _LIT8(KText, "ModeChange" );
            iNWELog->Log((TDesC8)KText);
            _LIT8(KMode, "Mode: " );
            iNWELog->Log((TDesC8)KMode );
            iNWELog->Log( NetworkMode[iNetworkMode] );
            iNWELog->Log((TDesC8)KLine );
            }
        else
            {
            _LIT8(KFail, "NotifyModeChange Failed" );
            iNWELog->Log((TDesC8)KFail);
            }
        }

    if ( iHandleSpecifiedRequests )
        {
        if ( iNetworkModeStatus[0] != iNetworkMode )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::GetCipheringIndicatorStatusL
// GetCipheringIndicatorStatus method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::GetCipheringIndicatorStatusL( CStifItemParser& aItem )
    {
    _LIT8(KGetCipheringIndicatorStatus, "CNWE::GetCipheringIndicatorStatusL" );
    iNWELog->Log((TDesC8)KGetCipheringIndicatorStatus );

    TInt ret( KErrNotFound );
    TPtrC string;

    aItem.GetNextString( string );

    if ( _L("SIM_SERV_CIPHERING_INDICATOR_DISPLAY_REQUIRED") == string )
        {
        iCipherIndicatorStatusForComparison = RMobilePhone::ECipheringDisplayOn;
        }
    else
        {
        iCipherIndicatorStatusForComparison = RMobilePhone::ECipheringDisplayOff;
        }

    aItem.GetNextString( string );

    if ( _L("SET_ONLY_COMPARISON_VALUE") == string )
        {
        // Do nothing, but set ret value, because iCipherIndicatorStatusForComparison
        // is set for comparison when EMobilePhoneGetCipheringIndicatorStatus has been
        // received after sequence:
        // NET_CIPHERING_IND received (cmmnetmesshandler and cmmcustommesshandler
        // SIM_READ_CI_REQ is sent from cmmcustommesshandler
        // SIM_READ_CI_RESP has been received and that response causes complete
        // with IPC EMobilePhoneGetCipheringIndicatorStatus
        ret = KErrNone;
        }
    else
        {
        ret = HandleRequestL( EMobilePhoneGetCipheringIndicatorStatus );
        iGetCipheringIndicatorStatusOngoing = ETrue;

        if ( KErrNone == ret )
            {
            _LIT8( KResultOk, "GetCipheringIndicatorStatus request send ok" );
            iNWELog->Log((TDesC8)KResultOk);
            }
        else
            {
            _LIT8( KResultFail, "GetCipheringIndicatorStatus request send fails: %d" );
            iNWELog->Log((TDesC8)KResultFail, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteGetCipheringIndicatorStatus
// Complete GetCipheringIndicatorStatus method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteGetCipheringIndicatorStatus
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // checking results and unpacking message
    _LIT8( KResult, "CNWE::CompleteGetCipheringIndicatorStatus result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData( &iCipherIndicatorStatus );

        iNWELog->Log((TDesC8)KLine );
        _LIT8( KCipIndStatus, "Ciphering Indicator Status: " );
        iNWELog->Log((TDesC8)KCipIndStatus );
        iNWELog->Log( TMobileCallCipheringIndicator[*iCipherIndicatorStatus] );
        iNWELog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8( KResultFail, "CompleteGetCipheringIndicatorStatus Failed: %d" );
        iNWELog->Log((TDesC8)KResultFail, aResult );
        }

    if ( ( iGetCipheringIndicatorStatusOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iGetCipheringIndicatorStatusOngoing = EFalse;

        _LIT8( KResultComplete, "GetCipheringIndicatorStatus Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        if ( ( iCipherIndicatorStatusForComparison != *iCipherIndicatorStatus ) &&
            ( KErrNone == aResult ) &&
            ( iHandleSpecifiedRequests ) )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomGetCipheringInfoL
// CustomGetCipheringInfo method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomGetCipheringInfoL( CStifItemParser& aItem )
    {
    _LIT8(KCustomGetCipheringInfo, "CNWE::CustomGetCipheringInfoL" );
    iNWELog->Log((TDesC8)KCustomGetCipheringInfo );

    TInt ret( KErrNotFound );
    TPtrC string;

    aItem.GetNextString( string );

    if ( _L("SIM_SERV_CIPHERING_INDICATOR_DISPLAY_REQUIRED") == string )
        {
        iCipherIndicatorOnOff = ETrue;
        iCipherIndicatorStatusForComparison = RMobilePhone::ECipheringDisplayOn;
        }

    ret = HandleRequestL( ECustomGetCipheringInfoIPC );
    iCustomGetCipheringInfoOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "CustomGetCipheringInfo request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "CustomGetCipheringInfo request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomGetCipheringInfo
// Complete CustomGetCipheringInfo method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomGetCipheringInfo
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // checking results and unpacking message
    _LIT8( KResult, "CNWE::CompleteCustomGetCipheringInfo result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    TBool cipherInfo(EFalse);

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData( cipherInfo );

        iNWELog->Log((TDesC8)KLine );
        _LIT8( KCipInfo, "Status of the Ciphering indication: %d" );
        iNWELog->Log((TDesC8)KCipInfo, ( TInt ) cipherInfo );
        iNWELog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8( KResultFail, "CompleteCustomGetCipheringInfo Failed: %d" );
        iNWELog->Log((TDesC8)KResultFail, aResult );
        }

    if ( ( iCustomGetCipheringInfoOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomGetCipheringInfoOngoing = EFalse;
        _LIT8( KResultComplete, "CustomGetCipheringInfo Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        if ( ( iCipherIndicatorOnOff != cipherInfo ) &&
            ( KErrNone == aResult ) &&
            ( iHandleSpecifiedRequests ) )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomNetWakeupL
// CustomNetWakeup method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomNetWakeupL( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KCustomNetWakeup, "CNWE::CustomNetWakeupL" );
    iNWELog->Log((TDesC8)KCustomNetWakeup );

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ECustomNetWakeupIPC );
    iCustomNetWakeupOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "CustomNetWakeup request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "CustomNetWakeup request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomNetWakeup
// Complete CustomNetWakeup method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomNetWakeup
    (
    TInt aResult // Completion result value
    )
    {
    _LIT8( KResult, "CNWE::CompleteCustomNetWakeup result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( ( iCustomNetWakeupOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomNetWakeupOngoing = EFalse;
        _LIT8( KResultComplete, "CustomNetWakeup Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomGetOperatorNameL
// CustomGetOperatorName method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomGetOperatorNameL( CStifItemParser& aItem )
    {
    _LIT8(KCustomGetOperatorName, "CNWE::CustomGetOperatorNameL" );
    iNWELog->Log((TDesC8)KCustomGetOperatorName );

    TPtrC string;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    aItem.GetNextString( string );
    iSavedDisplayTagForComparison[0].Zero();

    // Hard coded APAC name string:
    // - Operator name: CSL
    // - Unicode name: 0x8A0A, 0x806F, 0x96FB, 0x4FE1
    // - MCC 454, MNC 18
    if ( _L("APAC_NAME") == string )
        {
        iSavedDisplayTagForComparison[0].Append( 0x8A0A );
        iSavedDisplayTagForComparison[0].Append( 0x806F );
        iSavedDisplayTagForComparison[0].Append( 0x96FB );
        iSavedDisplayTagForComparison[0].Append( 0x4FE1 );
        }
    else
        {
        iSavedDisplayTagForComparison[0].Append( string );
        }

    aItem.GetNextString( string );
    iSavedShortNameForComparison[0].Zero();

    if ( _L("NO_SHORT_NAME") != string )
        {
        iSavedShortNameForComparison[0].Append( string );
        }

    aItem.GetNextString( string );
    iSavedLongNameForComparison[0].Zero();

    if ( _L("NO_LONG_NAME") != string )
        {
        iSavedLongNameForComparison[0].Append( string );
        }

    aItem.GetNextString( string );
    iSavedOperNameForComparison.Zero();

    // Hard coded APAC name string:
    // - Operator name: CSL
    // - Unicode name: 0x8A0A, 0x806F, 0x96FB, 0x4FE1
    // - MCC 454, MNC 18
    if ( _L("APAC_NAME") == string )
        {
        iSavedOperNameForComparison.Append( 0x8A0A );
        iSavedOperNameForComparison.Append( 0x806F );
        iSavedOperNameForComparison.Append( 0x96FB );
        iSavedOperNameForComparison.Append( 0x4FE1 );
        }
    else
        {
        iSavedOperNameForComparison.Append( string );
        }

    aItem.GetNextString( string );
    iSavedOperNameTypeForComparison.Zero();
    iSavedOperNameTypeForComparison.Append( string );

    TInt ret( KErrNotFound );

    aItem.GetNextString( string );

    if ( _L("SET_ONLY_COMPARISON_VALUE") == string )
        {
        // Set only expected values for comparison
        ret = KErrNone;
        }
    else
        {
        ret = HandleRequestL( ECustomGetOperatorNameIPC );
        iCustomGetOperatorNameOngoing = ETrue;

        if ( KErrNone == ret )
            {
            _LIT8( KResultOk, "CustomGetOperatorName request send ok" );
            iNWELog->Log((TDesC8)KResultOk);
            }
        else
            {
            _LIT8( KResultFail, "CustomGetOperatorName request send fails: %d" );
            iNWELog->Log((TDesC8)KResultFail, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomGetOperatorName
// Complete CustomGetOperatorName method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomGetOperatorName
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // checking results and unpacking message
    _LIT8( KResult, "CNWE::CompleteCustomGetOperatorName result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    RMmCustomAPI::TOperatorNameType operNameType;

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData( iOperatorName );

        iNWELog->Log((TDesC8)KLine );
        _LIT8( KOperatorNameType, "Operator name type: " );
        iNWELog->Log((TDesC8)KOperatorNameType );
        iNWELog->Log( TOperatorNameType[iOperatorName.iType] );
        _LIT8( KOperatorName, "Operator name: " );
        iNWELog->Log((TDesC8)KOperatorName );
        iNWELog->Log( iOperatorName.iName );
        iNWELog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8( KResultFail, "CompleteCustomGetOperatorName Failed: %d" );
        iNWELog->Log((TDesC8)KResultFail, aResult );
        }

    if ( ( 0 != iSavedOperNameForComparison.Compare( _L( "DONT_CARE_NETWORK_NAME" ) ) ) &&
        ( KErrNone == aResult ) )
        {
        if ( iSavedOperNameForComparison != iOperatorName.iName )
            {
            aResult = KErrGeneral;
            _LIT8( KResultFail, "CompleteCustomGetOperatorName - operator names do not match" );
            iNWELog->Log((TDesC8)KResultFail, aResult );
            }

        if ( 0 == iSavedOperNameTypeForComparison.Compare( _L( "EOperatorNameHardcodedLatin" ) ) )
            {
            operNameType = RMmCustomAPI::EOperatorNameHardcodedLatin;
            }
        else if ( 0 == iSavedOperNameTypeForComparison.Compare( _L( "EOperatorNameHardcodedUcs2" ) ) )
            {
            operNameType = RMmCustomAPI::EOperatorNameHardcodedUcs2;
            }
        else if ( 0 == iSavedOperNameTypeForComparison.Compare( _L( "EOperatorNameNitzFull" ) ) )
            {
            operNameType = RMmCustomAPI::EOperatorNameNitzFull;
            }
        else if ( 0 == iSavedOperNameTypeForComparison.Compare( _L( "EOperatorNameNitzShort" ) ) )
            {
            operNameType = RMmCustomAPI::EOperatorNameNitzShort;
            }
        else if ( 0 == iSavedOperNameTypeForComparison.Compare( _L( "EOperatorNameFlexiblePlmn" ) ) )
            {
            operNameType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
            }
        else if ( 0 == iSavedOperNameTypeForComparison.Compare( _L( "EOperatorNameCountryMcn" ) ) )
            {
            operNameType = RMmCustomAPI::EOperatorNameCountryMcn;
            }
        else if ( 0 == iSavedOperNameTypeForComparison.Compare( _L( "EOperatorNameMccMnc" ) ) )
            {
            operNameType = RMmCustomAPI::EOperatorNameMccMnc;
            }
        else
            {
            // Default value for comparison if any of values above hasn't been given in the function
            // CustomGetOperatorName call and will cause test case fail if typical operator name type
            // have been received from network (latin or ucs2).
            operNameType = RMmCustomAPI::EOperatorNameHighestPriority;
            }

        if ( operNameType != iOperatorName.iType )
            {
            aResult = KErrGeneral;
            _LIT8( KResultFail, "CompleteCustomGetOperatorName - operator name type doesn't match" );
            iNWELog->Log((TDesC8)KResultFail, aResult );
            }
        }

    if ( ( iCustomGetOperatorNameOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomGetOperatorNameOngoing = EFalse;
        _LIT8( KResultComplete, "CustomGetOperatorName Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomResetNetworkRegistrationStatusL
// CustomResetNetworkRegistrationStatus method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomResetNetworkRegistrationStatusL( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KCustomResetNetRegStat, "CNWE::CustomResetNetworkRegistrationStatusL" );
    iNWELog->Log((TDesC8)KCustomResetNetRegStat );

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ECustomResetNetServerIPC );
    iCustomResetNetworkRegistrationStatusOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "CustomResetNetworkRegistrationStatus request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "CustomResetNetworkRegistrationStatus request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomResetNetworkRegistrationStatus
// Complete CustomResetNetworkRegistrationStatus method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomResetNetworkRegistrationStatus( TInt aResult )
    {
    _LIT8( KResult, "CNWE::CompleteCustomResetNetworkRegistrationStatus result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( ( iCustomResetNetworkRegistrationStatusOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomResetNetworkRegistrationStatusOngoing = EFalse;
        _LIT8( KResultComplete, "CustomResetNetworkRegistrationStatus Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomGetSystemNetworkModesL
// CustomGetSystemNetworkModes method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomGetSystemNetworkModesL( CStifItemParser& aItem )
    {
    _LIT8(KCustomGetSystemNetworkModes, "CNWE::CustomGetSystemNetworkModesL" );
    iNWELog->Log((TDesC8)KCustomGetSystemNetworkModes );

    TInt ret( KErrNotFound );
    TPtrC string;

    aItem.GetNextString( string );

    iNetworkModeForComparison = 3; // KCapsNetworkModeDual

    if ( _L("NET_GSM_RAT") == string )
        {
        iNetworkModeForComparison = 1;
        }
    if ( _L("NET_UMTS_RAT") == string )
        {
        iNetworkModeForComparison = 2;
        }

    TBool useCurrentRats = EFalse;
    CMmDataPackage dataPackage;
    dataPackage.PackData(&useCurrentRats);

    ret = HandleRequestL(
        ECustomGetSystemNetworkModesIPC,
        &dataPackage );

    iCustomGetSystemNetworkModesOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "CustomGetSystemNetworkModes request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "CustomGetSystemNetworkModes request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomGetSystemNetworkModes
// Complete CustomGetSystemNetworkModes method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomGetSystemNetworkModes
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // checking results and unpacking message
    _LIT8( KResult, "CNWE::CompleteCustomGetSystemNetworkModes result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    TInt networkMode;

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData( networkMode );

        iNWELog->Log((TDesC8)KLine );
        _LIT8( KNetworkMode, "System Network mode: %d" );
        iNWELog->Log((TDesC8)KNetworkMode, networkMode );

        if ( networkMode == 1 )
            {
            _LIT8( KNetworkMode, "KCapsNetworkModeGsm" );
            iNWELog->Log((TDesC8)KNetworkMode );
            }
        else if ( networkMode == 2 )
            {
            _LIT8( KNetworkMode, "KCapsNetworkModeUmts" );
            iNWELog->Log((TDesC8)KNetworkMode );
            }
        else if ( networkMode == 3 )
            {
            _LIT8( KNetworkMode, "KCapsNetworkModeDual" );
            iNWELog->Log((TDesC8)KNetworkMode );
            }
        iNWELog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8( KResultFail, "CompleteCustomGetSystemNetworkModes Failed: %d" );
        iNWELog->Log((TDesC8)KResultFail, aResult );
        }

    if ( ( iCustomGetSystemNetworkModesOngoing )  ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomGetSystemNetworkModesOngoing = EFalse;
        _LIT8( KResultComplete, "CustomGetSystemNetworkModes Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        if ( ( iNetworkModeForComparison != networkMode ) &&
            ( iHandleSpecifiedRequests ) )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomSetSystemNetworkModeL
// CustomSetSystemNetworkMode method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomSetSystemNetworkModeL( CStifItemParser& aItem )
    {
    _LIT8(KCustomSetSystemNetworkMode, "CNWE::CustomSetSystemNetworkModeL" );
    iNWELog->Log((TDesC8)KCustomSetSystemNetworkMode );

    TInt ret( KErrNotFound );

    TBool selection = ETrue;
    TPtrC string;
    aItem.GetNextString( string );

    if ( _L( "NetworkGsm" ) == string )
        {
        _LIT8( KNetworkGsm, "KCapsNetworkModeGsm" );
        iNWELog->Log((TDesC8)KNetworkGsm );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeGsm;
        }
    else if ( _L ( "NetworkUmts" ) == string )
        {
        _LIT8( KNetworkUmts, "KCapsNetworkModeUmts" );
        iNWELog->Log((TDesC8)KNetworkUmts );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeUmts;
        }
    else if ( _L ( "NetworkDual" ) == string )
        {
        _LIT8( KNetworkDual, "KCapsNetworkModeDual" );
        iNWELog->Log((TDesC8)KNetworkDual );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeDual;
        }
    else
        {
        _LIT8( KFail, "Set CustomSetCapsNetworkMode not possible: %d" );
        iNWELog->Log((TDesC8)KFail, ret );
        selection = EFalse;
        }

    if ( selection )
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData(&iNetworkModeCaps);

        ret = HandleRequestL(
            ECustomSetSystemNetworkModeIPC,
            &dataPackage );

        iCustomSetSystemNetworkModeOngoing = ETrue;

        if ( KErrNone == ret )
            {
            _LIT8( KResultOk, "CustomSetSystemNetworkMode request send ok" );
            iNWELog->Log((TDesC8)KResultOk);
            }
        else
            {
            _LIT8( KResultFail, "CustomSetSystemNetworkMode request send fails: %d" );
            iNWELog->Log((TDesC8)KResultFail, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomSetSystemNetworkMode
// Complete CustomSetSystemNetworkMode method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomSetSystemNetworkMode( TInt aResult )
    {
    _LIT8( KResult, "CNWE::CompleteCustomSetSystemNetworkMode result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( ( iCustomSetSystemNetworkModeOngoing )  ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomSetSystemNetworkModeOngoing = EFalse;
        _LIT8( KResultComplete, "CustomSetSystemNetworkMode Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomGetCurrentSystemNetworkModesL
// CustomGetCurrentSystemNetworkModes method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomGetCurrentSystemNetworkModesL( CStifItemParser& aItem )
    {
    _LIT8(KCustomGetCurrentSystemNetworkModes,
        "CNWE::CustomGetCurrentSystemNetworkModesL" );
    iNWELog->Log((TDesC8)KCustomGetCurrentSystemNetworkModes );

    TInt ret( KErrNotFound );
    TPtrC string;

    aItem.GetNextString( string );

    iNetworkModeForComparison = 4; // KCapsNetworkModeDual

    if ( _L("MTC_GSM_RAT") == string )
        {
        iNetworkModeForComparison = 1;
        }

    if ( _L("MTC_UMTS_RAT") == string )
        {
        iNetworkModeForComparison = 2;
        }

    ret = HandleRequestL( ECustomGetCurrentSystemNetworkModesIPC );
    iCustomGetCurrentSystemNetworkModesOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "CustomGetCurrentSystemNetworkModes request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "CustomGetCurrentSystemNetworkModes request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomGetCurrentSystemNetworkModes
// Complete CustomGetCurrentSystemNetworkModes method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomGetCurrentSystemNetworkModes
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // checking results and unpacking message
    _LIT8( KResult, "CNWE::CompleteCustomGetCurrentSystemNetworkModes result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    TInt networkMode;

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData( networkMode );

        iNWELog->Log((TDesC8)KLine );
        _LIT8( KNetworkMode, "Current System Network mode: %d" );
        iNWELog->Log((TDesC8)KNetworkMode, networkMode );

        if ( networkMode == 1 )
            {
            _LIT8( KNetworkMode, "KCapsNetworkModeGsm" );
            iNWELog->Log((TDesC8)KNetworkMode );
            }
        else if ( networkMode == 2 )
            {
            _LIT8( KNetworkMode, "KCapsNetworkModeUmts" );
            iNWELog->Log((TDesC8)KNetworkMode );
            }
        else if ( networkMode == 4 )
            {
            _LIT8( KNetworkMode, "KCapsNetworkModeDual" );
            iNWELog->Log((TDesC8)KNetworkMode );
            }
        iNWELog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8( KResultFail, "CompleteCustomGetCurrentSystemNetworkModes Failed: %d" );
        iNWELog->Log((TDesC8)KResultFail, aResult );
        }

    if ( ( iCustomGetCurrentSystemNetworkModesOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomGetCurrentSystemNetworkModesOngoing = EFalse;
        _LIT8( KResultComplete, "CustomGetCurrentSystemNetworkModes Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        if ( ( iNetworkModeForComparison != networkMode ) &&
            ( iHandleSpecifiedRequests ) )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::NotifySignalStrengthChange
// NotifyChange of Signal Strength
// -----------------------------------------------------------------------------
//
void CNWE::NotifySignalStrengthChange
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    if ( ( iNotifyList[ENotifySignalStrengthChange] ) ||
        ( iHandleSpecifiedRequests ) )
        {
        _LIT8(KSignalStrengthChange, "CNWE::NotifySignalStrengthChange" );
        iNWELog->Log((TDesC8)KSignalStrengthChange );

        TInt8 signalBars;
        TInt32 signalStrength;

        // checking results and unpacking message
        if ( KErrNone == aResult )
            {
            aDataPackage->UnPackData( signalBars, signalStrength );

            iNWELog->Log((TDesC8)KLine );
            _LIT8(KSignalBars, "Signal Bars: %d" );
            iNWELog->Log((TDesC8)KSignalBars, signalBars );
            _LIT8(KSignalStrength, "Signal Strength in dBm: %d" );
            iNWELog->Log((TDesC8)KSignalStrength, signalStrength );
            iNWELog->Log((TDesC8)KLine );
            }
        else
            {
            _LIT8(Fail, "NotifySignalStrengthChange Failed" );
            iNWELog->Log((TDesC8)Fail);
            }
        }
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyNITZInfoChange
// NotifyChange of NITZ Info
// -----------------------------------------------------------------------------
//
void CNWE::NotifyNITZInfoChange
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    if ( ( iNotifyList[ENotifyNITZInfoChange] ) ||
        ( iHandleSpecifiedRequests ) )
        {
        _LIT8(KNITZInfoChange, "CNWE::NotifyNITZInfoChange" );
        iNWELog->Log((TDesC8)KNITZInfoChange );

        // checking results and unpacking message
        if ( KErrNone == aResult )
            {
            aDataPackage->UnPackData( iNITZInfo );

            iNWELog->Log((TDesC8)KLine );
            _LIT8(KNITZInfo, "NITZ Info and Zone Information" );
            iNWELog->Log((TDesC8)KNITZInfo );
            _LIT8(KDate, "Date info = Year, Month, Day: %d %d %d" );
            // Month: Offset from zero, so add one before displaying the month number.
            // Day: Offset from zero, so add one before displaying the day number.
            iNWELog->Log((TDesC8)KDate, iNITZInfo.Year(), iNITZInfo.Month()+1, iNITZInfo.Day()+1 );
            _LIT8(KTime, "Time info = Hour, Minute, Second, Microsecond: %d %d %d %d" );
            iNWELog->Log((TDesC8)KTime, iNITZInfo.Hour(), iNITZInfo.Minute(), iNITZInfo.Second(), iNITZInfo.MicroSecond() );
            _LIT8(KDST, "Daylight Saving Time adjustment for summer time: %d" );
            iNWELog->Log((TDesC8)KDST, iNITZInfo.iDST );
            _LIT8(KTimeZone, "Time zone, expressed in +/- 15 minute offsets from GMT: %d" );
            iNWELog->Log((TDesC8)KTimeZone, iNITZInfo.iTimeZone );
            iNWELog->Log((TDesC8)KLine );
            }

        if ( ( iHandleSpecifiedRequests ) &&
            ( ( 2008 != iNITZInfo.Year() ) ||
            ( EFebruary != iNITZInfo.Month() ) ||
            ( 2 != iNITZInfo.Day() ) ||
            ( 20 != iNITZInfo.Hour() ) ||
            ( 24 != iNITZInfo.Minute() ) ||
            ( 0 != iNITZInfo.Second() ) ||
            ( 0 != iNITZInfo.MicroSecond() ) ||
            ( 0 != iNITZInfo.iDST ) ||
            ( 0 != iNITZInfo.iTimeZone ) ) )
            {
            aResult = KErrGeneral;
            }
        else
            {
            _LIT8(Fail, "NotifyNITZInfoChange Failed" );
            iNWELog->Log((TDesC8)Fail);
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyNetworkSecurityLevelChange
// NotifyChange of NetworkSecurityLevel
// -----------------------------------------------------------------------------
//
void CNWE::NotifyNetworkSecurityLevelChange
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    if ( ( iNotifyList[ENotifyNetworkSecurityLevelChange] ) ||
        ( iHandleSpecifiedRequests ) )
        {
        _LIT8(KNetworkSecLevelChange, "CNWE::NotifyNetworkSecurityLevelChange" );
        iNWELog->Log((TDesC8)KNetworkSecLevelChange );

        // checking results and unpacking message
        if ( KErrNone == aResult )
            {
            aDataPackage->UnPackData( &iNetworkSecurityLevel );

            iNWELog->Log((TDesC8)KLine );
            _LIT8(KNetworkSecurityLevel, "Network Security Level Change Information: " );
            iNWELog->Log((TDesC8)KNetworkSecurityLevel );
            iNWELog->Log( TMobilePhoneNetworkSecurity[*iNetworkSecurityLevel] );
            iNWELog->Log((TDesC8)KLine );

            if ( ( iHandleSpecifiedRequests ) &&
                ( RMobilePhone::ECipheringGSM != *iNetworkSecurityLevel ) )
                {
                aResult = KErrGeneral;
                }
            }
        else
            {
            _LIT8(Fail, "NotifyNetworkSecurityLevelChange Failed: %d" );
            iNWELog->Log((TDesC8)Fail, aResult );
            }

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::ManualNetworkSearchL
// ManualNetworkSearch method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::ManualNetworkSearchL( CStifItemParser& aItem )
    {
    _LIT8(KManualNetworkSearch, "CNWE::ManualNetworkSearchL" );
    iNWELog->Log((TDesC8)KManualNetworkSearch );

    TPtrC string;

    aItem.GetNextString( string );
    iSavedDisplayTagForComparison[0].Zero();
    iSavedDisplayTagForComparison[0].Append( string );
    iSavedShortNameForComparison[0].Zero();
    iSavedShortNameForComparison[0].Append( string );
    iSavedLongNameForComparison[0].Zero();
    iSavedLongNameForComparison[0].Append( string );

    aItem.GetNextString( string );
    iSavedDisplayTagForComparison[1].Zero();
    iSavedDisplayTagForComparison[1].Append( string );
    iSavedShortNameForComparison[1].Zero();
    iSavedShortNameForComparison[1].Append( string );
    iSavedLongNameForComparison[1].Zero();
    iSavedLongNameForComparison[1].Append( string );

    aItem.GetNextString( string );
    iSavedDisplayTagForComparison[2].Zero();
    iSavedDisplayTagForComparison[2].Append( string );
    iSavedShortNameForComparison[2].Zero();
    iSavedShortNameForComparison[2].Append( string );
    iSavedLongNameForComparison[2].Zero();
    iSavedLongNameForComparison[2].Append( string );

    aItem.GetNextString( string );
    iSavedDisplayTagForComparison[3].Zero();
    iSavedDisplayTagForComparison[3].Append( string );
    iSavedShortNameForComparison[3].Zero();
    iSavedShortNameForComparison[3].Append( string );
    iSavedLongNameForComparison[3].Zero();
    iSavedLongNameForComparison[3].Append( string );

    if ( ( iHandleSpecifiedRequests ) &&
        ( 0 != iSavedDisplayTagForComparison[0].Compare ( _L( "DONT_CARE_NETWORK_NAME" ) ) ) )
        {
        aItem.GetNextString( string );
        iNetworkIdForComparison[0].Zero();
        iNetworkIdForComparison[0].Append( string );

        aItem.GetNextString( string );
        iCountryCodeForComparison[0].Zero();
        iCountryCodeForComparison[0].Append( string );

        aItem.GetNextString( string );
        iNetworkIdForComparison[1].Zero();
        iNetworkIdForComparison[1].Append( string );

        aItem.GetNextString( string );
        iCountryCodeForComparison[1].Zero();
        iCountryCodeForComparison[1].Append( string );

        aItem.GetNextString( string );
        iNetworkIdForComparison[2].Zero();
        iNetworkIdForComparison[2].Append( string );

        aItem.GetNextString( string );
        iCountryCodeForComparison[2].Zero();
        iCountryCodeForComparison[2].Append( string );

        aItem.GetNextString( string );
        iNetworkIdForComparison[3].Zero();
        iNetworkIdForComparison[3].Append( string );

        aItem.GetNextString( string );
        iCountryCodeForComparison[3].Zero();
        iCountryCodeForComparison[3].Append( string );
        }

    TInt ret( KErrNotFound );

    ret = HandleRequestL( EMobilePhoneGetDetectedNetworksV2Phase1 );
    iManualNetworkSearchOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "ManualNetworkSearch request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "ManualNetworkSearch request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteManualNetworkSearch
// Complete ManualNetworkSearch method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteManualNetworkSearch
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // checking results and unpacking message
    _LIT8( KResult, "CNWE::CompleteManualNetworkSearch result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        CMobilePhoneNetworkListV2* list = NULL;
        aDataPackage->UnPackData ( list );

        iNWELog->Log((TDesC8)KLine );
        _LIT8( KNetworkList, "Searched Network List: " );
        iNWELog->Log((TDesC8)KNetworkList );
        iNWELog->Log((TDesC8)KLine );

        for ( TInt i=0; i<list->Enumerate(); i++)
            {
            const RMobilePhone::TMobilePhoneNetworkInfoV2& entry = list->GetEntryL(i);

            _LIT8( KNetworkId, "Network ID: " );
            iNWELog->Log((TDesC8)KNetworkId );
            iNWELog->Log( entry.iNetworkId );

            _LIT8( KCountryCode, "Country Code: " );
            iNWELog->Log((TDesC8)KCountryCode );
            iNWELog->Log( entry.iCountryCode );

            _LIT8( KDisplayTag, "Display Tag: " );
            iNWELog->Log((TDesC8)KDisplayTag );
            iNWELog->Log( entry.iDisplayTag );

            _LIT8( KLongName, "Long Name: " );
            iNWELog->Log((TDesC8)KLongName );
            iNWELog->Log( entry.iLongName );

            _LIT8( KShortName, "Short Name: " );
            iNWELog->Log((TDesC8)KShortName );
            iNWELog->Log( entry.iShortName );

            iNWELog->Log((TDesC8)KLine );

            if ( ( 0 != iSavedDisplayTagForComparison[i].Compare ( _L( "DONT_CARE_NETWORK_NAME" ) ) ) &&
                ( KErrNone == aResult ) )
                {
                if ( ( iSavedDisplayTagForComparison[i] != entry.iDisplayTag ) ||
                    ( iSavedShortNameForComparison[i] != entry.iShortName ) ||
                    ( iSavedLongNameForComparison[i] != entry.iLongName ) )
                    {
                    _LIT8( KResultFail, "CompleteManualNetworkSearch - operator names do not match" );
                    iNWELog->Log((TDesC8)KResultFail);
                    aResult = KErrGeneral;
                    }

                if ( ( iHandleSpecifiedRequests ) &&
                    ( ( iNetworkIdForComparison[i] != entry.iNetworkId ) ||
                    ( iCountryCodeForComparison[i] != entry.iCountryCode ) ) )
                    {
                    _LIT8( KResultFail, "CompleteManualNetworkSearch - Network ID or Country Code don't match" );
                    iNWELog->Log((TDesC8)KResultFail);
                    aResult = KErrGeneral;
                    }
                }
            }

        _LIT8( KNetworkListEnd, "Searched Network List Ended" );
        iNWELog->Log((TDesC8)KNetworkListEnd );
        iNWELog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8( KResultFail, "CompleteManualNetworkSearch, Error case: %d" );
        iNWELog->Log((TDesC8)KResultFail, aResult );

        if ( KErrCancel == aResult
            && iManualNetworkSearchOngoing
            && iManualNetworkSearchCancelOngoing )
            {
            iManualNetworkSearchOngoing = EFalse;
    
            _LIT8( KResultComplete, "ManualNetworkSearch Completed with Cancel" );
            iNWELog->Log((TDesC8)KResultComplete );
    
            Signal( aResult );
            }
        }

    if ( iManualNetworkSearchOngoing && !iManualNetworkSearchCancelOngoing )
        {
        iManualNetworkSearchOngoing = EFalse;

        _LIT8( KResultComplete, "ManualNetworkSearch Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }

    if ( iNetworkModeTestOnGoing )
        {
        SetNetworkInfo( aResult, aDataPackage, EMobilePhoneGetDetectedNetworksV2Phase1 );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::ManualNetworkSearchCancelL
// ManualNetworkSearchCancel method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::ManualNetworkSearchCancelL( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KManualNetworkSearchCancel, "CNWE::ManualNetworkSearchCancelL" );
    iNWELog->Log((TDesC8)KManualNetworkSearchCancel );

    TInt ret( KErrNotFound );

    ret = HandleRequestL( EMobilePhoneGetDetectedNetworksCancel );
    iManualNetworkSearchCancelOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "ManualNetworkSearchCancel request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "ManualNetworkSearchCancel request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteManualNetworkSearchCancel
// Complete ManualNetworkSearchCancel method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteManualNetworkSearchCancel( TInt aResult )
    {
    _LIT8( KResult, "CNWE::CompleteManualNetworkSearchCancel result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( ( iManualNetworkSearchCancelOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iManualNetworkSearchCancelOngoing = EFalse;
        _LIT8( KResultComplete, "ManualNetworkSearchCancel Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::ManualNetworkSelectionL
// ManualNetworkSelection method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::ManualNetworkSelectionL( CStifItemParser& aItem )
    {
    _LIT8(KManualNetworkSelection, "CNWE::ManualNetworkSelectionL" );
    iNWELog->Log((TDesC8)KManualNetworkSelection );

    TInt ret( KErrNotFound );

    TPtrC string;

    RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection;
    aItem.GetNextString( string );
    manualSelection.iNetwork.Copy( string );
    aItem.GetNextString( string );
    manualSelection.iCountry.Copy( string );

    TBool isManual = ETrue;

    CMmDataPackage dataPackage;
    dataPackage.PackData( &isManual, &manualSelection );

    ret = HandleRequestL( EMobilePhoneSelectNetwork, &dataPackage );
    iManualNetworkSelectionOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "ManualNetworkSelection request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "ManualNetworkSelection request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteManualNetworkSelection
// Complete ManualNetworkSearchCancel method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteManualNetworkSelection( TInt aResult )
    {
    _LIT8( KResult, "CNWE::CompleteManualNetworkSelection result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( iManualNetworkSelectionOngoing && !iManualNetworkSelectionCancelOngoing )
        {
        iManualNetworkSelectionOngoing = EFalse;
        _LIT8( KResultComplete, "ManualNetworkSelection Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::ManualNetworkSelectionCancelL
// ManualNetworkSelectionCancel method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::ManualNetworkSelectionCancelL( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KManualNetworkSelectionCancel, "CNWE::ManualNetworkSelectionCancelL" );
    iNWELog->Log((TDesC8)KManualNetworkSelectionCancel );

    TInt ret( KErrNotFound );

    ret = HandleRequestL( EMobilePhoneSelectNetworkCancel );
    iManualNetworkSelectionCancelOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "ManualNetworkSelectionCancel request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "ManualNetworkSelectionCancel request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteManualNetworkSelectionCancel
// Complete ManualNetworkSelectionCancel method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteManualNetworkSelectionCancel( TInt aResult )
    {
    _LIT8( KResult, "CNWE::CompleteManualNetworkSelectionCancel result: %d" );
    iNWELog->Log((TDesC8)KResult, aResult );

    if ( ( iManualNetworkSelectionCancelOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iManualNetworkSelectionCancelOngoing = EFalse;
        _LIT8( KResultComplete, "ManualNetworkSelectionCancel Completed" );
        iNWELog->Log((TDesC8)KResultComplete );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::Notify network connection failure
// -----------------------------------------------------------------------------
//
TInt CNWE::NotifyNetworkConnectionFailure( CStifItemParser& aItem )
    {
    iNWELog->Log(_L8("CNWE::NotifyNetworkConnectionFailure") );
    iNetworkFailureTestOnGoing = ETrue;

    if ( iHandleSpecifiedRequests )
        {
        TPtrC string;

        aItem.GetNextString( string );
        nspsIsOffOnForComparison = EFalse;
        if ( _L("NSPS_IS_ON") == string )
            {
            nspsIsOffOnForComparison = ETrue;
            }

        aItem.GetNextString( string );
        if ( ( _L("NET_REG_STATUS_NOSERV_NOSIM") == string ) ||
            ( _L("NET_REG_STATUS_NOSERV_SIM_REJECTED_BY_NW") == string ) )
            {
            iNWRegStatusForComparison = RMobilePhone::ERegistrationDenied;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNWE::Complete NotifyNetworkConnectionFailure
// -----------------------------------------------------------------------------
//
void CNWE::CompleteNotifyNetworkConnectionFailure( TInt aResult/*, CMmDataPackage&*/ /*aData*/)
    {
    iNWELog->Log(_L8("CNWE::CompleteNotifyNetworkConnectionFailure, err=%d"),aResult);

    if ( ( iNetworkFailureTestOnGoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iNetworkFailureTestOnGoing = EFalse;
        iNWELog->Log(_L8("CNWE::CompleteNotifyNetworkConnectionFailure") );
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::GetNetworkInfo
// GetNetworkInfo method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::GetNetworkInfo( CStifItemParser& aItem )
    {
    _LIT8(KGetNetworkInfo, "CNWE::GetNetworkInfo" );
    iNWELog->Log((TDesC8)KGetNetworkInfo );

    TInt ret( KErrNotFound );

    iNetworkModeTestOnGoing = ETrue;

    TBool supported( ETrue );

    TPtrC function;
    TPtrC mode;
    TPtrC onlyComparisonValue;

    aItem.GetNextString( function );
    aItem.GetNextString( mode );
    aItem.GetNextString( onlyComparisonValue );
  
    // Set "supported" as "EFalse" so comparison value for complete value checking
    // is set.
    if ( _L("SET_ONLY_COMPARISON_VALUE") == onlyComparisonValue )
        {
        supported = EFalse;
        ret = KErrNone;
        iNetworkModeTestOnGoing = EFalse;
        }

    if ( function == _L( "NetworkMode" ) )
        {
        if ( _L( "NetworkModeUnknown" ) == mode )
            {
            iNetworkModeStatus[0] = RMobilePhone::ENetworkModeUnknown;
            }
        else if ( _L( "NetworkModeUnregistered" ) == mode )
            {
            iNetworkModeStatus[0] = RMobilePhone::ENetworkModeUnregistered;
            }
        else if ( _L( "NetworkModeGsm" ) == mode )
            {
            iNetworkModeStatus[0] = RMobilePhone::ENetworkModeGsm;
            }
        else if ( _L( "NetworkModeAmps" ) == mode )
            {
            iNetworkModeStatus[0] = RMobilePhone::ENetworkModeAmps;
            }
        else if ( _L( "NetworkModeCdma95" ) == mode )
            {
            iNetworkModeStatus[0] = RMobilePhone::ENetworkModeCdma95;
            }
        else if ( _L( "NetworkModeCdma2000" ) == mode )
            {
            iNetworkModeStatus[0] = RMobilePhone::ENetworkModeCdma2000;
            }
        else if ( _L( "NetworkModeWcdma" ) == mode )
            {
            iNetworkModeStatus[0] = RMobilePhone::ENetworkModeWcdma;
            }
        else
            {
            iNWELog->Log( _L8("Selected network mode not supported") );
            supported = EFalse;
            }
        }
    else if ( function == _L( "LocationArea" ) )
        {
        if ( _L( "True" ) == mode )
            {
            iLocationAreaKnown[0] = ETrue;
            }
        else if ( _L( "False" ) == mode )
            {
            iLocationAreaKnown[0] = EFalse;
            }
        else
            {
            iNWELog->Log( _L8("Selected area mode not supported") );
            supported = EFalse;
            }
        }
    else
        {
        iNWELog->Log( _L8("Selected mode not supported") );
        supported = EFalse;
        }

    if ( supported )
        {
        if ( function == _L( "NetworkMode" ) )
            {
            ret = HandleRequestL( EMobilePhoneGetNetworkRegistrationStatus );
            }
        else
            {
            ret = HandleRequestL( ECustomGetOperatorNameIPC );
            }

        if ( KErrNone == ret )
            {
            _LIT8( KResultOk, "GetNetworkInfo request send ok" );
            iNWELog->Log((TDesC8)KResultOk);
            }
        else
            {
            _LIT8( KResultFail, "GetNetworkInfo request send fails: %d" );
            iNWELog->Log((TDesC8)KResultFail, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::SetNetworkInfo
// Setting network info
// -----------------------------------------------------------------------------
//
void CNWE::SetNetworkInfo(
    TInt aResult,
    CMmDataPackage* aDataPackage,
    TInt aCompletedIpc )
    {
    _LIT8(KSetNetworkInfo, "CNWE::SetNetworkInfo" );
    iNWELog->Log((TDesC8)KSetNetworkInfo );

    iNetworkModeTestOnGoing = EFalse;

    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;

    if ( aCompletedIpc == EMobilePhoneNotifyCurrentNetworkChange )
        {
        aDataPackage->UnPackData( networkInfo, locationArea );

        _LIT8(KWAreaInfo, "Received location area known: %d" );
        iNWELog->Log((TDesC8)KWAreaInfo, locationArea.iAreaKnown );

        iLocationAreaKnown[1] = locationArea.iAreaKnown;

        _LIT8(KDAreaInfo, "Expected location area known: %d" );
        iNWELog->Log((TDesC8)KDAreaInfo, iLocationAreaKnown[0] );

        //if received values correspond to the values given manually
        if ( iNetworkModeStatus[0] == iNetworkModeStatus[1] && iLocationAreaKnown[0] == iLocationAreaKnown[1] )
            {
            iNWELog->Log( _L8("Values from the network match to the ones been set") );
            Signal( KErrNone );
            }
        else
            {
            iNWELog->Log( _L8("Values from the network doesn't match to the ones been set") );

            //self made error code that can be easily noticed
            Signal( -1111 );
            }
        }
    else if ( aCompletedIpc == EMobilePhoneNotifyModeChange )
        {
        aDataPackage->UnPackData( iNetworkModeStatus[1] );

        _LIT8(KWNetworkInfo, "Received network mode : %d" );
        iNWELog->Log((TDesC8)KWNetworkInfo, iNetworkModeStatus[1] );

        _LIT8(KDNetworkInfo, "Expected network mode : %d" );
        iNWELog->Log((TDesC8)KDNetworkInfo, iNetworkModeStatus[0] );

        //initialize values for a new test
        iNetworkModeStatus[0] = RMobilePhone::ENetworkModeUnknown;
        iNetworkModeStatus[1] = RMobilePhone::ENetworkModeUnknown;
        iLocationAreaKnown[0] = EFalse;
        iLocationAreaKnown[1] = EFalse;

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::WaitForCallStatusChange
// WaitForCallStatusChange method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::WaitForCallStatusChange( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KManualNetworkSelection, "CNWE::WaitForCallStatusChange" );
    iNWELog->Log((TDesC8)KManualNetworkSelection );

    TInt ret( KErrNone );

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCallStatusChange
// Comolete call status change
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCallStatusChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    iNWELog->Log(_L8("CNWE::CompleteCallStatusChange, err=%d"),aResult);

    RMobileCall::TMobileCallStatus* callStatus;
    aDataPackage->UnPackData( &callStatus );

    switch( *callStatus )
        {
        case RMobileCall::EStatusConnected:
            {
            _LIT8(KConnected, "Call Status: Connected" );
            iNWELog->Log((TDesC8)KConnected );
            Signal( aResult );
            break;
            }
        case RMobileCall::EStatusIdle:
            {
            _LIT8(KIdle, "Call Status: Idle" );
            iNWELog->Log((TDesC8)KIdle );
            Signal( aResult );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNWE::GetCellInfo
// GetCellInfo method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::GetCellInfo( CStifItemParser& aItem )
    {
    _LIT8(KGetCellInfo, "CNWE::GetCellInfo" );
    iNWELog->Log((TDesC8)KGetCellInfo );

    TInt ret( KErrNotFound );
    TInt temp;

    // Indexes used in parameter validation
    aItem.GetNextInt( iNmrListIndex );
    aItem.GetNextInt( iDetailedCellInfoIndex );

    // Store given parameters
    aItem.GetNextInt(
        iRefECIDInfo.iCellInfo.iWcdmaCellInfo.iPrimaryScrambilingCode );
    aItem.GetNextInt(
        iRefECIDInfo.iCellInfo.iWcdmaCellInfo.iFrequencyInfo.iFddDL );

    aItem.GetNextInt( temp );
    iRefECIDInfo.iCellInfo.iWcdmaCellInfo.iNwkMeasureReport[iNmrListIndex].
        iCarrierRSSI = temp;

    aItem.GetNextInt( temp );
    iRefECIDInfo.iCellInfo.iWcdmaCellInfo.iNwkMeasureReport[iNmrListIndex].
        iCellMeasuredResult[iDetailedCellInfoIndex].iCID = temp;

    ret = HandleRequestL( ECustomGetCellInfoIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "GetCellInfo request send ok" );
        iNWELog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8( KResultFail, "GetCellInfo request send fails: %d" );
        iNWELog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteGetCellInfo
// Complete GetCellInfo method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteGetCellInfo( TInt aResult, CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CNWE::CompleteGetCellInfo, result: %d" );
    iNWELog->Log( ( TDesC8 ) KResult, aResult );

    TECIDInfo tempECIDInfo;
    aDataPackage->UnPackData( tempECIDInfo.iCellInfo );

    // Check parameters
    if ( tempECIDInfo.iCellInfo.iWcdmaCellInfo.iPrimaryScrambilingCode !=
            iRefECIDInfo.iCellInfo.iWcdmaCellInfo.iPrimaryScrambilingCode ||

        tempECIDInfo.iCellInfo.iWcdmaCellInfo.iFrequencyInfo.iFddDL !=
            iRefECIDInfo.iCellInfo.iWcdmaCellInfo.iFrequencyInfo.iFddDL ||

        tempECIDInfo.iCellInfo.iWcdmaCellInfo.iNwkMeasureReport[iNmrListIndex].
            iCarrierRSSI !=
                iRefECIDInfo.iCellInfo.iWcdmaCellInfo.
                    iNwkMeasureReport[iNmrListIndex].iCarrierRSSI ||

        tempECIDInfo.iCellInfo.iWcdmaCellInfo.iNwkMeasureReport[iNmrListIndex].
            iCellMeasuredResult[iDetailedCellInfoIndex].iCID !=
                iRefECIDInfo.iCellInfo.iWcdmaCellInfo.
                    iNwkMeasureReport[iNmrListIndex].
                        iCellMeasuredResult[iDetailedCellInfoIndex].iCID )
        {
        aResult = KErrGeneral;
        _LIT8( KResult, "CNWE::CompleteGetCellInfo, Error: Invalid parameter" );
        iNWELog->Log( ( TDesC8 ) KResult );
        }

    Signal( aResult );
    }

// not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

// -----------------------------------------------------------------------------
// CNWE::CustomSetBandSelectionL
// CustomSetBandSelection method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomSetBandSelectionL( CStifItemParser& aItem )
    {
    _LIT8( KCustomSetBandSelection, "CNWE::CustomSetBandSelectionL" );
    iNWELog->Log( ( TDesC8 ) KCustomSetBandSelection );

    TInt ret( KErrNone );

    TPtrC modeString;
    aItem.GetNextString( modeString );

    TPtrC bandString;
    aItem.GetNextString( bandString );

    if ( _L( "NetworkUmts" ) == modeString )
        {
        _LIT8( KNetworkUmts, "KCapsNetworkModeUmts" );
        iNWELog->Log( ( TDesC8 ) KNetworkUmts );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeUmts;
        }
    else if ( _L( "NetworkDual" ) == modeString )
        {
        _LIT8( KNetworkDual, "KCapsNetworkModeDual" );
        iNWELog->Log( ( TDesC8 ) KNetworkDual );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeDual;
        }
    else
        {
        _LIT8( KFail, "CustomSetBandSelection, Unknown Mode" );
        iNWELog->Log( ( TDesC8 ) KFail );
        ret = KErrNotSupported;
        }

    if ( _L( "BandAny" ) == bandString )
        {
        _LIT8( KBandAny, "ENetworkBandAny" );
        iNWELog->Log( ( TDesC8 ) KBandAny );
        iNetworkBand = RMmCustomAPI::ENetworkBandAny;
        }
    else if ( _L( "Band850" ) == bandString )
        {
        _LIT8( KBand850, "ENetworkBandUmts850" );
        iNWELog->Log( ( TDesC8 ) KBand850 );
        iNetworkBand = RMmCustomAPI::ENetworkBandUmts850;
        }
    else if ( _L( "Band2100" ) == bandString )
        {
        _LIT8( KBand2100, "ENetworkBandUmts2100" );
        iNWELog->Log( ( TDesC8 ) KBand2100 );
        iNetworkBand = RMmCustomAPI::ENetworkBandUmts2100;
        }
    else
        {
        _LIT8( KFail, "CustomSetBandSelection, Unknown Band" );
        iNWELog->Log( ( TDesC8 ) KFail );
        ret = KErrNotSupported;
        }

    if ( KErrNone == ret )
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( &iNetworkBand, &iNetworkModeCaps );

        ret = HandleRequestL( ECustomSetBandSelectionIPC, &dataPackage );

        iCustomSetBandSelectionOngoing = ETrue;

        if ( KErrNone == ret )
            {
            _LIT8( KResultOk, "CustomSetBandSelection, Request send ok" );
            iNWELog->Log( ( TDesC8 ) KResultOk );
            }
        else
            {
            _LIT8( KResultFail,
                "CustomSetBandSelection, Request send failed: %d" );
            iNWELog->Log( ( TDesC8 ) KResultFail, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomSetBandSelection
// Complete CustomSetBandSelection method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomSetBandSelection( TInt aResult )
    {
    _LIT8( KResult, "CNWE::CompleteCustomSetBandSelection, result: %d" );
    iNWELog->Log( ( TDesC8 ) KResult, aResult );

    if ( ( iCustomSetBandSelectionOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomSetBandSelectionOngoing = EFalse;

        _LIT8( KResultComplete, "CustomSetBandSelection, Completed" );
        iNWELog->Log( ( TDesC8 ) KResultComplete );
        iNWELog->Log( ( TDesC8 ) KLine );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CNWE::CustomGetBandSelectionL
// CustomGetBandSelection method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::CustomGetBandSelectionL( CStifItemParser& aItem )
    {
    _LIT8( KCustomGetBandSelection, "CNWE::CustomGetBandSelectionL" );
    iNWELog->Log( ( TDesC8 ) KCustomGetBandSelection );

    TPtrC modeString;
    aItem.GetNextString( modeString );

    TPtrC bandString;
    aItem.GetNextString( bandString );

    TInt ret( KErrNone );

    if ( _L( "NetworkUmts" ) == modeString )
        {
        _LIT8( KNetworkUmts, "KCapsNetworkModeUmts" );
        iNWELog->Log( ( TDesC8 ) KNetworkUmts );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeUmts;
        }
    else if ( _L( "NetworkDual" ) == modeString )
        {
        _LIT8( KNetworkDual, "KCapsNetworkModeDual" );
        iNWELog->Log( ( TDesC8 ) KNetworkDual );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeDual;
        }
    else if ( _L( "NetworkGsm" ) == modeString )
        {
        _LIT8( KNetworkGsm, "KCapsNetworkModeGsm" );
        iNWELog->Log( ( TDesC8 ) KNetworkGsm );
        iNetworkModeCaps = RMmCustomAPI::KCapsNetworkModeGsm;
        }
    else
        {
        _LIT8( KFail, "CustomSetBandSelection, Unknown network mode" );
        iNWELog->Log( ( TDesC8 ) KFail );
        ret = KErrNotSupported;
        }

    if ( _L( "BandAny" ) == bandString )
        {
        _LIT8( KBandAny, "ENetworkBandAny" );
        iNWELog->Log( ( TDesC8 ) KBandAny );
        iNetworkBand = RMmCustomAPI::ENetworkBandAny;
        }
    else if ( _L( "Band850" ) == bandString )
        {
        _LIT8( KBand850, "ENetworkBandUmts850" );
        iNWELog->Log( ( TDesC8 ) KBand850 );
        iNetworkBand = RMmCustomAPI::ENetworkBandUmts850;
        }
    else if ( _L( "Band2100" ) == bandString )
        {
        _LIT8( KBand2100, "ENetworkBandUmts2100" );
        iNWELog->Log( ( TDesC8 ) KBand2100 );
        iNetworkBand = RMmCustomAPI::ENetworkBandUmts2100;
        }
    else
        {
        _LIT8( KFail, "CustomSetBandSelection, Unknown Band. Test case will not support type like this.");
        iNWELog->Log( ( TDesC8 ) KFail );
        ret = KErrNotSupported;
        }

    ret = HandleRequestL( ECustomGetBandSelectionIPC );
    iCustomGetBandSelectionOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "CustomGetBandSelection, Request send ok" );
        iNWELog->Log( ( TDesC8 ) KResultOk );
        }
    else
        {
        _LIT8( KResultFail, "CustomGetBandSelection, Request failed: %d" );
        iNWELog->Log( ( TDesC8 ) KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNWE::CompleteCustomGetBandSelection
// Complete CustomGetBandSelection method function.
// -----------------------------------------------------------------------------
//
void CNWE::CompleteCustomGetBandSelection(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    // checking results and unpacking message
    _LIT8( KResult, "CNWE::CompleteCustomGetBandSelection, result: %d" );
    iNWELog->Log( ( TDesC8 ) KResult, aResult );

    if ( KErrNone == aResult )
        {
        RMmCustomAPI::TNetworkModeCaps networkModeCaps;
        RMmCustomAPI::TBandSelection networkBand;

        aDataPackage->UnPackData( networkBand, networkModeCaps );

        iNWELog->Log( ( TDesC8 ) KLine );
        _LIT8( KBandSel, "Selected band: %d" );
        iNWELog->Log( ( TDesC8 ) KBandSel, iNetworkBand );

        if ( networkBand != iNetworkBand )
            {
            _LIT8( KNWBandError, "Network band mismatched" );
            iNWELog->Log( ( TDesC8 ) KNWBandError );
            aResult = KErrGeneral;
            }

        if ( networkModeCaps != iNetworkModeCaps )
            {
            _LIT8( KNWModeError, "Network band mode mismatched" );
            iNWELog->Log( ( TDesC8 ) KNWModeError );
            aResult = KErrArgument;
            }

        iNWELog->Log( ( TDesC8 ) KLine );
        }
    else
        {
        _LIT8( KResultFail, "CompleteCustomGetBandSelection, Failed: %d" );
        iNWELog->Log( ( TDesC8 ) KResultFail, aResult );
        }

    if ( ( iCustomGetBandSelectionOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        iCustomGetBandSelectionOngoing = EFalse;

        _LIT8( KResultComplete, "CustomGetBandSelection, Completed" );
        iNWELog->Log( ( TDesC8 ) KResultComplete );

        Signal( aResult );
        }
    }

#endif //NCP_COMMON_S60_VERSION_SUPPORT

// -----------------------------------------------------------------------------
// CNWE::NotifyPrivacyConfirmation
// EMobileCallNotifyPrivacyConfirmation
// -----------------------------------------------------------------------------
//
void CNWE::NotifyPrivacyConfirmation(
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    _LIT8( KNotifyPrivacyConfirmation, "CNWE::NotifyPrivacyConfirmation" );
    iNWELog->Log( ( TDesC8 ) KNotifyPrivacyConfirmation );

    RMobilePhone::TMobilePhonePrivacy status;
    aDataPackage->UnPackData( status );

    if ( status != RMobilePhone::EPrivacyOn )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyCipheringInfoChangeIPC
// ECustomNotifyCipheringInfoChangeIPC
// -----------------------------------------------------------------------------
//
void CNWE::NotifyCipheringInfoChangeIPC(
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    _LIT8( KNotifyCipheringInfoChangeIPC, "CNWE::NotifyCipheringInfoChangeIPC" );
    iNWELog->Log( ( TDesC8 ) KNotifyCipheringInfoChangeIPC );

    TBool cipherIndicatorStatus( EFalse );
    TBool cipheringOn( EFalse );
    aDataPackage->UnPackData( cipherIndicatorStatus, cipheringOn );

    // Expected that both are ETrue
    if ( ( !cipherIndicatorStatus ) ||
        ( !cipheringOn ) )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyNSPSStatusIPC
// ECustomNotifyNSPSStatusIPC
// -----------------------------------------------------------------------------
//
void CNWE::NotifyNSPSStatusIPC
    (
    TInt aResult, // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    _LIT8( KNotifyNSPSStatusIPC, "CNWE::NotifyNSPSStatusIPC" );
    iNWELog->Log( ( TDesC8 ) KNotifyNSPSStatusIPC );

    TBool nspsIsOn ( EFalse );
    aDataPackage->UnPackData ( nspsIsOn );

    if ( nspsIsOn != nspsIsOffOnForComparison )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CNWE::SetShortAndLongOperNameL
// SetShortAndLongOperName method function.
// -----------------------------------------------------------------------------
//
TInt CNWE::SetShortAndLongOperNameL( CStifItemParser& aItem )
    {
    _LIT8( KSetShortAndLongOperName, "CNWE::SetShortAndLongOperNameL");
    iNWELog->Log((TDesC8)KSetShortAndLongOperName);

    TPtrC string;

    aItem.GetNextString( string );
    iSavedShortNameForComparison[0].Zero();
    iSavedShortNameForComparison[0].Append( string );

    aItem.GetNextString( string );
    iSavedLongNameForComparison[0].Zero();
    iSavedLongNameForComparison[0].Append( string );
  
    return KErrNone;
    }

//  End of File
