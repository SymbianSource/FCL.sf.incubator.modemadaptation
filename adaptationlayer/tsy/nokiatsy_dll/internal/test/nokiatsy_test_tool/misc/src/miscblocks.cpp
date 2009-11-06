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
* Description:   Specific class for Miscellaneous tests
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "misc.h"
// CONSTANTS
const TPtrC BooleanFlag[] =
  {
  _L("EFalse"),
  _L("ETrue")
  };

_LIT8( KLine, "---------------------------------" );

// local functions

static TInt GetSecurityCode(CStifItemParser& aItem, RMobilePhone::TMobilePhoneSecurityCode &aSecurityCode)
    {
     //  security code enum
    TPtrC string;
    TInt ret(aItem.GetNextString(string));
    if (KErrNone == ret)
        {
        if (string == _L("ESecurityCodePin1"))
            {
            aSecurityCode = RMobilePhone::ESecurityCodePin1;
            }
        else if (string == _L("ESecurityCodePin2"))
            {
            aSecurityCode = RMobilePhone::ESecurityCodePin2;
            }
        else if (string == _L("ESecurityCodePuk1"))
            {
            aSecurityCode = RMobilePhone::ESecurityCodePuk1;
            }
        else if (string == _L("ESecurityCodePuk2"))
            {
            aSecurityCode = RMobilePhone::ESecurityCodePuk2;
            }
        else if (string == _L("ESecurityCodePhonePassword"))
            {
            aSecurityCode = RMobilePhone::ESecurityCodePhonePassword;
            }
        else if (string == _L("ESecurityCodeSPC"))
            {
            aSecurityCode = RMobilePhone::ESecurityCodeSPC;
            }
        else if (string == _L("ESecurityHiddenKey"))
            {
            aSecurityCode = RMobilePhone::ESecurityHiddenKey;
            }
        else if (string == _L("ESecurityUSIMAppPin"))
            {
            aSecurityCode = RMobilePhone::ESecurityUSIMAppPin;
            }
        else if (string == _L("ESecuritySecondUSIMAppPin"))
            {
            aSecurityCode = RMobilePhone::ESecuritySecondUSIMAppPin;
            }
        else if (string == _L("ESecurityUniversalPin"))
            {
            aSecurityCode = RMobilePhone::ESecurityUniversalPin;
            }
        else if (string == _L("ESecurityUniversalPuk"))
            {
            aSecurityCode = RMobilePhone::ESecurityUniversalPuk;
            }
        else
           {
           ret = KErrArgument;
           }
        }
    return ret;
    }

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMisc::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CMisc::Delete()
    {
    }

// -----------------------------------------------------------------------------
// CMisc::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMisc::RunMethodL(
    CStifItemParser& aItem )
    {
   _LIT8 (KLog, "Misc: RunMethodL" );
   iMiscLog->Log((TDesC8)KLog);

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "MobilePhoneNotifyBatteryInfoChange", CMisc::MobilePhoneNotifyBatteryInfoChange),
        ENTRY( "SetACLStatus", CMisc::SetACLStatus),
        ENTRY( "GetACLStatus", CMisc::GetACLStatus),
        ENTRY( "WriteACLEntry", CMisc::WriteACLEntryL),
        ENTRY( "DeleteACLEntry", CMisc::DeleteACLEntry),
        ENTRY( "VerifySecurityCode", CMisc::VerifySecurityCodeL),
        ENTRY( "EnumerateAPNEntries", CMisc::EnumerateAPNEntries),
        ENTRY( "GetAPNName", CMisc::GetAPNname),
        ENTRY( "SetAlwaysOnModeSelection", CMisc::SetAlwaysOnModeSelectionL),
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC", CCore::SetExpectedCompleteIPC),
        ENTRY( "RunIscTestCase", CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase",CCore::SyncIscTestCase),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CMisc::MobilePhoneNotifyBatteryInfoChange
// Start test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMisc::MobilePhoneNotifyBatteryInfoChange( CStifItemParser& aItem )
    {
    TInt ret ( KErrNone );
    TPtrC status;
    TInt chargeLevel( 0 );

    aItem.GetNextString ( status );
    aItem.GetNextInt ( chargeLevel );

    if ( _L("EPowerStatusUnknown") == status )
        {
        iBatteryInfo.iStatus = RMobilePhone::EPowerStatusUnknown;
        }
    else if ( _L("EBatteryConnectedButExternallyPowered") == status )
        {
        iBatteryInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
        }
    else
        {
        _LIT8(KNotSupp, "CMisc::MobilePhoneNotifyBatteryInfoChange / selected status not supported");
        iMiscLog->Log((TDesC8)KNotSupp);
        ret = KErrNotSupported;
        }

    iBatteryInfo.iChargeLevel = chargeLevel;

    return ret;
    }

// -----------------------------------------------------------------------------
// CMisc::CompleteMobilePhoneNotifyBatteryInfoChange
// Completion method (end testcase)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMisc::CompleteMobilePhoneNotifyBatteryInfoChange
        (
        CMmDataPackage* aDataPackage, // aDataPackage
        TInt aResult                  // Completion result value
        )
    {
    iMiscLog->Log(_L("CMisc::CompleteMobilePhoneNotifyBatteryInfoChange"));

    _LIT8(KResult, "CompleteMobilePhoneNotifyBatteryInfoChange request complete with aResult %d" );
    iMiscLog->Log((TDesC8)KResult, aResult);

    if ( KErrNone == aResult )
        {
        // unpack data...
        RMobilePhone::TMobilePhoneBatteryInfoV1* batteryInfo = NULL;
        aDataPackage->UnPackData(&batteryInfo);

        if ( ( batteryInfo->iStatus != iBatteryInfo.iStatus ) ||
             ( batteryInfo->iChargeLevel != iBatteryInfo.iChargeLevel ) )
            {
            aResult = KErrGeneral;
            }
        _LIT8(KFmt, "Values from Complete: Status=%d, ChargeLevel=%d");
        iMiscLog->Log(KFmt, batteryInfo->iStatus, batteryInfo->iChargeLevel);
        _LIT8(KExpected, "Expected values: Status=%d, ChargeLevel=%d");
        iMiscLog->Log(KExpected, iBatteryInfo.iStatus, iBatteryInfo.iChargeLevel);
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CMisc::SetACLStatus
// Start test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMisc::SetACLStatus( CStifItemParser& aItem)
    {
    //need to verify PIN2 first

    // Print to UI
    TestModuleIf().Printf( 0, _L("CMisc::SetACLStatus"), _L("In EMobilePhoneSetAPNControlListServiceStatus") );
    // Print to log file
    iMiscLog->Log(_L("CMisc::SetACLStatus EMobilePhoneSetAPNControlListServiceStatus"));

    TInt ret ( KErrNone );
    TPtrC string;
    RMobilePhone::TAPNControlListServiceStatus status;

    aItem.GetNextString(string);

    if (KErrNone == ret)
        {
        if (string == _L("EAclStatusDisabled"))
            {
            status = RMobilePhone::EAPNControlListServiceDisabled;
            }
        else if (string == _L("EAclStatusEnabled"))
            {
            status = RMobilePhone::EAPNControlListServiceEnabled;
            }
        else
           {
           _LIT8(KNotSupp, "CMisc::SetACLStatus / selected status not supported");
           iMiscLog->Log((TDesC8)KNotSupp);
           ret = KErrArgument;
           }
        }

    // call NTSY
    if (KErrNone == ret)
        {
        CMmDataPackage package;
        package.PackData( &status );
        ret = HandleRequestL(EMobilePhoneSetAPNControlListServiceStatus, &package );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMisc::CompleteSetACLStatus
// Completion method (end testcase)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMisc::CompleteSetACLStatus
        (
        CMmDataPackage* /*aDataPackage*/, // empty
        TInt aResult    // //Completion result value
        )
    {
    iMiscLog->Log(_L8("CMisc::CompleteSetACLStatus aResult=%d"),TInt(aResult));

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CMisc::GetACLStatus
// Start test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMisc::GetACLStatus( CStifItemParser& aItem )
    {

    iMiscLog->Log(_L("CMisc::GetACLStatus EMobilePhoneGetAPNControlListServiceStatus"));

    TInt ret ( KErrNone );
    TPtrC status;

    aItem.GetNextString ( status );

    if ( _L("EAPNControlListServiceDisabled") == status )
        {
        iACLStatus = RMobilePhone::EAPNControlListServiceDisabled;
        }
    else
        {
        _LIT8(KNotSupp, "CMisc::GetACLStatus / selected status not supported");
        iMiscLog->Log((TDesC8)KNotSupp);
        ret = KErrNotSupported;
        }

    // call NTSY
    if (KErrNone == ret)
        {
        CMmDataPackage package;
        //package.PackData( &status );
        TInt ret = HandleRequestL(EMobilePhoneGetAPNControlListServiceStatus, &package );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMisc::CompleteGetACLStatus
// Completion method (end testcase)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMisc::CompleteGetACLStatus
        (
        CMmDataPackage* aDataPackage, // aDataPackage
        TInt aResult                  // Completion result value
        )
    {
    iMiscLog->Log(_L8("CMisc::CompleteGetACLStatus aResult=%d"),TInt(aResult));

    _LIT8(KResultOk, "CompleteGetACLStatus request complete" );
    iMiscLog->Log((TDesC8)KResultOk);
    if ( KErrNone == aResult )
        {
        RMobilePhone::TAPNControlListServiceStatus ACLstatus;
        aDataPackage->UnPackData(ACLstatus);

        if ( ACLstatus != iACLStatus )
            {
            aResult = KErrGeneral;
            }
        iMiscLog->Log(_L8("CMisc::CompleteGetACLStatus ACL status = %d"),TInt(ACLstatus));
        iMiscLog->Log(_L8("CMisc::Expected ACL status = %d"),TInt(iACLStatus));
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CMisc::DeleteACLEntry
// Start test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMisc::DeleteACLEntry( CStifItemParser& aItem )
    {

    // Print to UI
    TestModuleIf().Printf( 0, _L("CMisc::DeleteACLEntry"), _L("In EMobilePhoneDeleteAPNName") );
    // Print to log file
    iMiscLog->Log(_L("CMisc::DeleteACLEntry EMobilePhoneDeleteAPNName"));

    TUint index;
    TInt ret(aItem.GetNextInt(index));
    // test case running...

    // call NTSY
    CMmDataPackage package;

    if( KErrNone == ret)
        {
        //TODO: package.PackData( &status );
        package.PackData( &index );
        ret = HandleRequestL(EMobilePhoneDeleteAPNName, &package );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMisc::CompleteDeleteACLEntry
// Completion method (end testcase)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMisc::CompleteDeleteACLEntry
        (
        TInt aResult                  // Completion result value
        )
    {
    iMiscLog->Log(_L8("CMisc::CompleteDeleteACLEntry ACL aResult = %d"),TInt(aResult));
    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CMisc::WriteACLEntryL
// Start test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMisc::WriteACLEntryL( CStifItemParser& aItem )
    {

    // Print to UI
    TestModuleIf().Printf( 0, _L("CMisc::WriteACLEntryL"), _L("In EMobilePhoneAppendAPNName") );
    // Print to log file
    iMiscLog->Log(_L("CMisc::WriteACLEntryL EMobilePhoneAppendAPNName"));

    TInt ret = 0;

    TPtrC string;
    ret = aItem.GetNextString(string);

    RMobilePhone::TAPNEntryV3 data;
    data.iApn.Append( string );

    // call NTSY
    CMmDataPackage package;
    //package data
    package.PackData( &data );

    ret = HandleRequestL(EMobilePhoneAppendAPNName, &package );
    return ret;
    }


// -----------------------------------------------------------------------------
// CMisc::CompleteWriteACLEntry
// Completion method (end testcase)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMisc::CompleteWriteACLEntry
        (
        TInt aResult                  // Completion result value
        )
    {
    iMiscLog->Log(_L8("CMisc::CompleteWriteACLEntry ACL aResult = %d"),TInt(aResult));
    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CMisc::EnumerateAPNEntries
// Start test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMisc::EnumerateAPNEntries( CStifItemParser& aItem )
    {

    // Print to UI
    TestModuleIf().Printf( 0, _L("Misc"), _L("In EMobilePhoneEnumerateAPNEntries") );
    // Print to log file
    iMiscLog->Log(_L("EMobilePhoneEnumerateAPNEntries"));

    TUint entries;
    aItem.GetNextInt(entries);
    iAPNEntries = entries;
  // call NTSY
    CMmDataPackage package;
    //TODO: package.PackData( &status );
    TInt ret = HandleRequestL(EMobilePhoneEnumerateAPNEntries, &package );
    return ret;
    }


// -----------------------------------------------------------------------------
// CMisc::CompleteEnumerateAPNEntries
// Completion method (end testcase)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMisc::CompleteEnumerateAPNEntries
        (
        CMmDataPackage* aDataPackage, // aDataPackage
        TInt aResult                  // Completion result value
        )
    {
    iMiscLog->Log(_L8("CMisc::CompleteEnumerateAPNEntries aResult = %d"),TInt(aResult));

    if ( KErrNone == aResult )
        {
        TUint32* apnEntries( NULL );
        aDataPackage->UnPackData( &apnEntries );
        iMiscLog->Log(_L8("CMisc::CompleteEnumerateAPNEntries apnEntries = %d"),TInt(*apnEntries));
        // If iAPNEntries is 0 the return value is not checked (used during HW
        // testing and that's why test person don't have to set right value to phone)
        if ( ( 0 != iAPNEntries ) &&
             ( iAPNEntries != *apnEntries ) )
            {
            aResult = KErrGeneral;
            }
        }

    Signal( aResult );

    }


// -----------------------------------------------------------------------------
// CMisc::GetAPNname
// Start test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMisc::GetAPNname( CStifItemParser& aItem )
    {

    // Print to UI
    TestModuleIf().Printf( 0, _L("Misc"), _L("In EMobilePhoneGetAPNname") );
    // Print to log file
    iMiscLog->Log(_L("EMobilePhoneGetAPNname"));

    //TUint32* index = 0;
    TUint index;
    aItem.GetNextInt(index);

    TPtrC apnName;
    aItem.GetNextString ( apnName );
    iAPNEntryName.iApn.Zero();
    if ( _L("") != apnName )
      {
      iAPNEntryName.iApn.Append( apnName );
      }

    CMmDataPackage package;
    package.PackData( &index );

    TInt ret = HandleRequestL(EMobilePhoneGetAPNname, &package );
    return ret;
    }


// -----------------------------------------------------------------------------
// CMisc::CompleteGetAPNname
// Completion method (end testcase)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMisc::CompleteGetAPNname
        (
        CMmDataPackage* aDataPackage, // aDataPackage
        TInt aResult                  // Completion result value
        )
    {
    iMiscLog->Log(_L8("CMisc::CompleteGetAPNname aResult = %d"),TInt(aResult));

    if ( KErrNone == aResult )
        {
        // APN Entry
        RMobilePhone::TAPNEntryV3* apnEntryName( NULL );
        // Unpack data
        aDataPackage->UnPackData( &apnEntryName );
        TBuf8<100> dontcare;
        dontcare.Zero();

        if ( ( dontcare != iAPNEntryName.iApn ) &&
             (  iAPNEntryName.iApn != apnEntryName->iApn ) )
            {
            aResult = KErrGeneral;
            }
        iMiscLog->Log(_L8("CMisc::CompleteGetAPNname apnEntryName = %S"), &apnEntryName->iApn);
      }
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CMisc::VerifySecurityCodeL
// -----------------------------------------------------------------------------
//
TInt CMisc::VerifySecurityCodeL( CStifItemParser& aItem )
    {
    iMiscLog->Log(_L8("CMisc::VerifySecurityCodeL"));

    TInt ret(KErrNone);
    TPtrC string;

    RMobilePhone::TMobilePhoneSecurityCode type;
    RMobilePhone::TCodeAndUnblockCode codes;

    // 1st parameter: security code type
    ret = GetSecurityCode(aItem, type);

    // 2nd parameter code
    if (KErrNone == ret)
        {
        ret = aItem.GetNextString(string);
        if (KErrNone == ret)
            {
            codes.iCode.Copy(string);
            }
        }

    // 3rd parameter: unblock code
    if (KErrNone == ret)
        {
        ret = aItem.GetNextString(string);
        if (KErrNone == ret)
            {
            codes.iUnblockCode.Copy(string);
            }
        }

    if ( KErrNone == ret)
        {
        CMmDataPackage package;
        package.PackData(&type, &codes);
        ret = HandleRequestL ( EMobilePhoneVerifySecurityCode, &package );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMisc::CompleteVerifySecurityCode
// -----------------------------------------------------------------------------
//
void CMisc::CompleteVerifySecurityCode(TInt aErr, CMmDataPackage& /*aData*/ )
    {
    iMiscLog->Log(_L8("CSecurity::CompleteVerifySecurityCode, err = %d"), aErr);
    iMiscLog->Log(_L8("CSecurity::CompleteVerifySecurityCode"));
    Signal(aErr);
    }


// -----------------------------------------------------------------------------
// CMisc::SetAlwaysOnModeSelection
// SetAlwaysOnModeSelection method function.
// -----------------------------------------------------------------------------
//
TInt CMisc::SetAlwaysOnModeSelectionL( CStifItemParser& aItem )
    {
    _LIT8(KSetAlwaysOnModeSelection, "CMisc::SetAlwaysOnModeSelection");
    iMiscLog->Log((TDesC8)KSetAlwaysOnModeSelection);

    TInt ret( KErrNotFound );
    TBool selection = ETrue;
    TPtrC string;
    aItem.GetNextString ( string );

    // Set Always On Mode
    if ( _L("HPLMN") == string )
        {
        iMiscLog->Log( _L("HPLMN.") );
        iAlwaysOnMode = RMmCustomAPI::EAlwaysModeHPLMN;
        }
    else if ( _L("VPLMN") == string )
        {
        iMiscLog->Log( _L("VPLMN.") );
        iAlwaysOnMode = RMmCustomAPI::EAlwaysModeVPLMN;
        }
    else if ( _L("Active") == string )
        {
        iMiscLog->Log( _L("Active.") );
        iAlwaysOnMode = RMmCustomAPI::EAlwaysModeBoth;
        }
    else if ( _L("NotActive") == string )
        {
        iMiscLog->Log( _L("NotActive.") );
        iAlwaysOnMode = RMmCustomAPI::EAlwaysModeNeither;
        }
    else
        {
        selection = EFalse;
        iMiscLog->Log( _L("Set Always on Mode not possible: %d"), ret );
        }

    if ( selection )
        {
        
        CMmDataPackage dataPackage;
        dataPackage.PackData(&iAlwaysOnMode);
        
        ret = HandleRequestL( ECustomSetAlwaysOnMode,
                               &dataPackage );
        
        
        if ( KErrNone == ret )
        
            {
            _LIT8( KSendOk, "Set AlwaysOnMode request send ok" );
            iMiscLog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "Set AlwaysOnMode request send failed: %d" );
            iMiscLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }
    
    return ret;

    }

// -----------------------------------------------------------------------------
// CMisc::CompleteSetAlwaysOnModeSelection
// Complete SetNetworkSelection method function.
// -----------------------------------------------------------------------------
//
void CMisc::CompleteSetAlwaysOnModeSelection
    (
    TInt aResult
    )
  {
  _LIT8( KResult, "CMisc::CompleteSetAlwaysOnModeSelection result: %d" );
  iMiscLog->Log((TDesC8)KResult, aResult );

  _LIT8( KResultComplete, "iSetAlwaysOnModeOngoing Completed" );
  iMiscLog->Log((TDesC8)KResultComplete );

  Signal( aResult );
  }
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None
//  End of File
