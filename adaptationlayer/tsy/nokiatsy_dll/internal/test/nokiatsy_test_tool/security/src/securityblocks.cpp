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
* Description:   Specific class for Security tests
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "security.h"
#include <ctsy/serviceapi/cmmsmsutility.h>
#include <ctsy/rmmcustomapi.h>

// TICCType enumerates the SIM card types
enum TICCType
    {
    EICCTypeSim2GGsm,
    EICCTypeSim3G,
    EICCTypeSimUnknown
    };


// CONSTANTS
    // copied from CMmCustomSecurityTsy.h
    const TUint8 KSecCodeNone = 0x00;
    const TUint8 KSecCodeSecurity = 0x01;
    const TUint8 KSecCodePin = 0x02;
    const TUint8 KSecCodePuk = 0x03;
    const TUint8 KSecCodePin2 = 0x04;
    const TUint8 KSecCodePuk2 = 0x05;
    const TUint8 KSecCodeLock = 0x06;
    const TUint8 KSecCodeUpin = 0x07;
    const TUint8 KSecCodeUpuk = 0x08;
    const TUint8 KSecCodeSubstitutePin = 0x09;
    const TUint8 KSecCodeSubstitutePinUniversal = 0x0A;
    const TUint8 KSecCodeNotApplicable = 0x63;

// LOCAL FUNCTIONS

// -----------------------------------------------------------------------------
// GetSecurityCode
// -----------------------------------------------------------------------------
//
static TInt GetSecurityCode(
    CStifItemParser& aItem,
    RMobilePhone::TMobilePhoneSecurityCode &aSecurityCode )
    {
     //  security code enum
    TPtrC string;
    TInt ret( aItem.GetNextString( string ) );

    if ( KErrNone == ret )
        {
        if ( string == _L("ESecurityCodePin1") )
            {
            aSecurityCode = RMobilePhone::ESecurityCodePin1;
            }
        else if ( string == _L("ESecurityCodePin2") )
            {
            aSecurityCode = RMobilePhone::ESecurityCodePin2;
            }
        else if ( string == _L("ESecurityCodePuk1") )
            {
            aSecurityCode = RMobilePhone::ESecurityCodePuk1;
            }
        else if ( string == _L("ESecurityCodePuk2") )
            {
            aSecurityCode = RMobilePhone::ESecurityCodePuk2;
            }
        else if ( string == _L("ESecurityCodePhonePassword") )
            {
            aSecurityCode = RMobilePhone::ESecurityCodePhonePassword;
            }
        else if ( string == _L("ESecurityCodeSPC") )
            {
            aSecurityCode = RMobilePhone::ESecurityCodeSPC;
            }
        else if ( string == _L("ESecurityHiddenKey") )
            {
            aSecurityCode = RMobilePhone::ESecurityHiddenKey;
            }
        else if ( string == _L("ESecurityUSIMAppPin") )
            {
            aSecurityCode = RMobilePhone::ESecurityUSIMAppPin;
            }
        else if ( string == _L("ESecuritySecondUSIMAppPin") )
            {
            aSecurityCode = RMobilePhone::ESecuritySecondUSIMAppPin;
            }
        else if ( string == _L("ESecurityUniversalPin") )
            {
            aSecurityCode = RMobilePhone::ESecurityUniversalPin;
            }
        else if ( string == _L("ESecurityUniversalPuk") )
            {
            aSecurityCode = RMobilePhone::ESecurityUniversalPuk;
            }
        else
           {
           ret = KErrArgument;
           }
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// GetCodeID
// -----------------------------------------------------------------------------
//
static TInt GetCodeID(
    CStifItemParser& aItem,
    TUint8 &aCode )
    {
    TPtrC string;
    TInt ret( aItem.GetNextString( string ) );

    if ( KErrNone == ret )
        {
        if ( _L("KSecCodeNone") == string )
            {
            aCode = KSecCodeNone;
            }
        else if ( _L("KSecCodeSecurity") == string )
            {
            aCode = KSecCodeSecurity;
            }
        else if ( _L("KSecCodePin") == string )
            {
            aCode = KSecCodePin;
            }
        else if ( _L("KSecCodePuk") == string )
            {
            aCode = KSecCodePuk;
            }
        else if ( _L("KSecCodePin2") == string )
            {
            aCode = KSecCodePin2;
            }
        else if ( _L("KSecCodePuk2") == string )
            {
            aCode = KSecCodePuk2;
            }
        else if ( _L("KSecCodeLock") == string )
            {
            aCode = KSecCodeLock;
            }
        else if ( _L("KSecCodeUpin") == string )
            {
            aCode = KSecCodeUpin;
            }
        else if ( _L("KSecCodeUpuk") == string )
            {
            aCode = KSecCodeUpuk;
            }
        else if ( _L("KSecCodeSubstitutePin") == string )
            {
            aCode = KSecCodeSubstitutePin;
            }
        else if ( _L("KSecCodeSubstitutePinUniversal") == string )
            {
            aCode = KSecCodeSubstitutePinUniversal;
            }
        else if ( _L("KSecCodeNotApplicable") == string )
            {
            aCode = KSecCodeNotApplicable;
            }
        else
            {
            ret = KErrArgument;
            }
        }
    //no else

    return ret;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSecurity::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CSecurity::Delete()
    {
   _LIT8( KLog, "CSecurity::Delete" );
   iSecurityLog->Log( ( TDesC8 ) KLog );
    }

// -----------------------------------------------------------------------------
// CSecurity::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSecurity::RunMethodL(
    CStifItemParser& aItem )
    {
   _LIT8( KLog, "CSecurity::RunMethodL" );
   iSecurityLog->Log( ( TDesC8 ) KLog );

    static TStifFunctionInfo const KFunctions[] =
        {
        ENTRY( "RunIscTestCase",               CCore::RunIscTestCase ),
        ENTRY( "SyncIscTestCase",              CCore::SyncIscTestCase ),
        ENTRY( "HandleSpecifiedRequests",      CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable",    CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC",       CCore::SetExpectedCompleteIPC),
        ENTRY( "DeliverCode",                  CSecurity::DeliverCodeL ),
        ENTRY( "IsBlocked",                    CSecurity::IsBlockedL ),
        ENTRY( "CheckSecurityCode",            CSecurity::CheckSecurityCodeL ),
        ENTRY( "CheckSecurityCodeCancel",      CSecurity::CheckSecurityCodeCancelL ),
        ENTRY( "DisablePhoneLock",             CSecurity::DisablePhoneLockL ),
        ENTRY( "SimGetICCType",                CSecurity::SimGetICCTypeL ),
        ENTRY( "BootNotifyModemStatusReady",   CSecurity::BootNotifyModemStatusReady ),
        ENTRY( "BootNotifySimStatusReady",     CSecurity::BootNotifySimStatusReady ),
        ENTRY( "BootNotifySecurityReady",      CSecurity::BootNotifySecurityReady ),
        ENTRY( "BootGetRFStatus",              CSecurity::BootGetRFStatusL ),
        ENTRY( "PhoneGetPin1DisableSupported", CSecurity::PhoneGetPin1DisableSupportedL ),
        ENTRY( "SecurityGetSimActivePinState", CSecurity::SecurityGetSimActivePinStateL ),
        ENTRY( "GetLockInfo",                  CSecurity::GetLockInfoL ),
        ENTRY( "NotifyLockInfoChange",         CSecurity::NotifyLockInfoChange ),
        ENTRY( "SetLockSetting",               CSecurity::SetLockSettingL ),
        ENTRY( "ChangeSecurityCode",           CSecurity::ChangeSecurityCodeL ),
        ENTRY( "NotifySecurityEvent",          CSecurity::NotifySecurityEvent ),
        ENTRY( "VerifySecurityCode",           CSecurity::VerifySecurityCodeL ),
        ENTRY( "AbortSecurityCode",            CSecurity::AbortSecurityCodeL ),
        ENTRY( "SimGetFdnStatus",              CSecurity::SimGetFdnStatusL ),
        ENTRY( "SimSetFdnStatus",              CSecurity::SimSetFdnStatusL ),
        ENTRY( "SecurityInfo",                 CSecurity::SecurityInfoL ),
        ENTRY( "GetCurrentActiveUSIMApplication",       CSecurity::GetCurrentActiveUSIMApplication),

        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CSecurity::DeliverCodeL
// -----------------------------------------------------------------------------
//
TInt CSecurity::DeliverCodeL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::DeliverCodeL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TPtrC  string;
    // 1st parameter: code to deliver
    TInt ret( aItem.GetNextString( string ) );

    if ( KErrNone == ret )
        {
        CMmDataPackage package;
        iSecCode.Copy( string );
        package.PackData( &iSecCode );
        ret = HandleRequestL( ECustomSecurityDeliverCodeIPC, &package );
        }
    //no else

    // Signal completion, because deliver doesn't have corresponding complete
    Signal( ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::IsBlockedL
// -----------------------------------------------------------------------------
//
TInt CSecurity::IsBlockedL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::IsBlockedL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    // 1st parameter: codeID
    // commontsy can send (const TUint8):
    //  KSecCodeSecurity = 0x01
    //  KSecCodePin = 0x02
    //  KSecCodePin2 = 0x04
    //  KSecCodeUpin = 0x07
    ret = GetCodeID( aItem, iCodeID );

    // 2nd parameter: expected result
    // NokiaTSY sends: ETrue or EFalse
    TPtrC string;
    ret = aItem.GetNextString( string );
    if ( KErrNone == ret )
        {
        if ( string == _L("ETrue") )
            {
            iIsBlockedExpected = ETrue;
            }
        else if ( string == _L("EFalse") )
            {
            iIsBlockedExpected = EFalse;
            }
        else
            {
            ret = KErrArgument;
            }
        }
    //no else

    if ( KErrNone == ret )
        {
        CMmDataPackage package;
        package.PackData( &iCodeID );
        ret = HandleRequestL( ECustomIsBlockedIPC, &package );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteIsBlocked
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteIsBlocked(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteIsBlocked, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( KErrNone == aErr )
        {
        TBool isBlocked( EFalse );
        aData.UnPackData ( isBlocked );
        if ( iIsBlockedExpected != isBlocked )
            {
            _LIT8( KFail,
                "CSecurity::CompleteIsBlocked, Failed, wrong result (%d), expected (%d)" );
            iSecurityLog->Log( ( TDesC8 ) KFail,
                TInt( isBlocked ), TInt( iIsBlockedExpected ) );

            aErr = KErrGeneral;
            }
        //no else
        }
    else
        {
        _LIT8( KLog, "CSecurity::CompleteIsBlocked, err = %d" );
        iSecurityLog->Log( ( TDesC8 ) KLog, aErr );
        }

    Signal( aErr );
    }

// -----------------------------------------------------------------------------
// CSecurity::CheckSecurityCodeL
// -----------------------------------------------------------------------------
//
TInt CSecurity::CheckSecurityCodeL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::CheckSecurityCodeL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    // 1st parameter: codeID
    // commontsy can send (const TUint8):
    //  KSecCodeSecurity = 0x01
    //  KSecCodePin      = 0x02
    //  KSecCodePin2     = 0x04
    //  KSecCodePuk      = 0x03
    //  KSecCodePuk2     = 0x05
    ret = GetCodeID( aItem, iCodeID );

    // 2nd argument, expected result
    if ( KErrNone == ret )
        {
        CMmDataPackage package;
        package.PackData( &iCodeID, &iSecCode );
        ret = HandleRequestL( ECustomCheckSecurityCodeIPC, &package );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteCheckSecurityCode
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteCheckSecurityCode(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteCheckSecurityCode, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    // If error is not KErrNone, then security code verify has failed and
    // we need to cancel SEC_CODE_GET_REQ.
    // If error is KErrCancel, we have already cancelled SEC_CODE_GET_REQ.
    if ( ( KErrNone != aErr ) && ( KErrCancel != aErr ) && ( KErrAbort != aErr ))
        {
        _LIT8( KFailed, "CSecurity::CompleteCheckSecurityCode, Failed, call cancel" );
        iSecurityLog->Log( ( TDesC8 ) KFailed );

        TBool cancelFalse( EFalse );
        CMmDataPackage package;
        package.PackData( &iCodeID, &cancelFalse );
        TRAP_IGNORE(
            HandleRequestL( ECustomCheckSecurityCodeCancelIPC, &package );
            );
        }
    else
        {
        _LIT8( KComplete, "CSecurity::CompleteCheckSecurityCode, completed" );
        iSecurityLog->Log( ( TDesC8 ) KComplete );
        }

    Signal( aErr );
    }

// -----------------------------------------------------------------------------
// CSecurity::DisablePhoneLockL
// -----------------------------------------------------------------------------
//
TInt CSecurity::DisablePhoneLockL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::DisablePhoneLockL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    /** the standard password used by mobile phones for secure access
    to services (phone lock, SIM lock, call barring password). The password
    is coded as Unicode characters. */
    TPtrC string;
    ret = aItem.GetNextString( string );

    if ( KErrNone == ret )
        {
        RMobilePhone::TMobilePassword verifyCode;
        verifyCode.Copy( string );

        CMmDataPackage package;
        package.PackData( &verifyCode );

        ret = HandleRequestL( ECustomDisablePhoneLockIPC, &package );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteDisablePhoneLock
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteDisablePhoneLock(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteDisablePhoneLock, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    Signal( aErr );
    }

// -----------------------------------------------------------------------------
// CSecurity::CheckSecurityCodeL
// -----------------------------------------------------------------------------
//
TInt CSecurity::CheckSecurityCodeCancelL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::CheckSecurityCodeCancelL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    // 1st parameter: codeID
    ret = GetCodeID( aItem, iCodeID );

    if ( KErrNone == ret )
        {
        // Send message to Security Server to check security code  and
        // intialize ret value. This is cancel request, so 3rd parameter
        // is true to indicate that.
        CMmDataPackage package;
        TBool trueCancel( ETrue );
        package.PackData( &iCodeID, &trueCancel );
        HandleRequestL( ECustomCheckSecurityCodeCancelIPC, &package );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteCheckSecurityCodeCancel
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteCheckSecurityCodeCancel(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteCheckSecurityCodeCancel, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    Signal( aErr );
    }

// -----------------------------------------------------------------------------
// CSecurity::SimGetICCTypeL
// -----------------------------------------------------------------------------
//
TInt CSecurity::SimGetICCTypeL(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::SimGetICCTypeL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    ret = HandleRequestL( EMmTsySimGetICCType );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteGetICCType
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteSimGetICCType(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteGetSimICCType, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    //unpack the data
    TICCType* simType = NULL;
    aData.UnPackData( &simType );

    switch( *simType )
        {
        case EICCTypeSim2GGsm:
            {
            _LIT8( KICCType,
                "CSecurity::CompleteGetSimICCType, type = EICCTypeSim2GGsm" );
            iSecurityLog->Log( ( TDesC8 ) KICCType );
            break;
            }
        case EICCTypeSim3G:
            {
            _LIT8( KICCType,
                "CSecurity::CompleteGetSimICCType, type = EICCTypeSim3G" );
            iSecurityLog->Log( ( TDesC8 ) KICCType );
            break;
            }
        case EICCTypeSimUnknown:
            {
            _LIT8( KICCType,
                "CSecurity::CompleteGetSimICCType, type = EICCTypeSimUnknown" );
            iSecurityLog->Log( ( TDesC8 ) KICCType );
            break;
            }
        default:
            {
            _LIT8( KICCType,
                "CSecurity::CompleteGetSimICCType, type unknown = %d" );
            iSecurityLog->Log( ( TDesC8 ) KICCType, TInt( *simType ) );
            aErr = KErrGeneral;
            break;
            }
        }

    Signal( aErr );
    }
    
// -----------------------------------------------------------------------------
// CSecurity::GetCurrentActiveUSIMApplication
// Get current active USIM application
// -----------------------------------------------------------------------------
//
TInt CSecurity::GetCurrentActiveUSIMApplication( CStifItemParser& aItem )
    {
    iSecurityLog->Log(_L8("CSecurity::GetCurrentActiveUSIMApplication"));
    TPtrC string;
    aItem.GetNextString( string );
    
    TInt ret( KErrNone );
    
        ret = HandleRequestL( EMobilePhoneGetCurrentActiveUSimApplication );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "GetCurrentActiveUSIMApplication request send ok" );
            iSecurityLog->Log((TDesC8)KSendOk );
            if ( _L("147377136") == string)
                {
                iAIDCheckOngoing = ETrue;
                iActiveID.Append(0xA0);
                iActiveID.Append(0x00);
                iActiveID.Append(0x00);
                iActiveID.Append(0x00);
                iActiveID.Append(0x87);
                iActiveID.Append(0x10);
                iActiveID.Append(0x02);
                iActiveID.Append(0xFF);
                iActiveID.Append(0xFF);
                iActiveID.Append(0xFF);
                iActiveID.Append(0xFF);
                iActiveID.Append(0x89);
                iActiveID.Append(0x04);
                iActiveID.Append(0x03);
                iActiveID.Append(0x00);
                iActiveID.Append(0xFF);
                }
            }
        else
            {
             _LIT8( KSendFailed, "GetCurrentActiveUSIMApplication request send failed: %d" );
            iSecurityLog->Log((TDesC8)KSendFailed, ret );

            }

    return ret;

    }
// -----------------------------------------------------------------------------
// CSecurity::CompleteGetCurrentActiveUSIMApplication
// Complete GetCurrentActiveUSIMApplication method function.
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteGetCurrentActiveUSIMApplication
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetCurrentActiveUSIMApplication result: %d" );
    iSecurityLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult && iAIDCheckOngoing )
        {
        // Response information
        RMobilePhone::TAID* activeAID;
        //unpack data
        aDataPackage->UnPackData( &activeAID );

        _LIT8( KActiveAID, "ActiveAID = %d" );
        iSecurityLog->Log((TDesC8)KActiveAID, activeAID  );
        iAIDCheckOngoing = EFalse;
        if ( *activeAID != iActiveID)
            {
            aResult = KErrGeneral;
            }
        }
    else if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetCurrentActiveUSIMApplication Failed: %d" );
        iSecurityLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }
    
// -----------------------------------------------------------------------------
// CSecurity::BootNotifyModemStatusReady
// -----------------------------------------------------------------------------
//
TInt CSecurity::BootNotifyModemStatusReady(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::BootNotifyModemStatusReady" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    iBootNotifyModemStatusReadyOngoing = ETrue;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteBootNotifyModemStatusReady
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteBootNotifyModemStatusReady(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteBootNotifyModemStatusReady, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iBootNotifyModemStatusReadyOngoing ||
        iHandleSpecifiedRequests )
        {
        iBootNotifyModemStatusReadyOngoing = EFalse;

        _LIT8( KComplete, "CSecurity::CompleteBootNotifyModemStatusReady, completed" );
        iSecurityLog->Log( ( TDesC8 ) KComplete );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::BootNotifySimStatusReady
// -----------------------------------------------------------------------------
//
TInt CSecurity::BootNotifySimStatusReady(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::BootNotifySimStatusReady" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    iBootNotifySimStatusReadyOngoing = ETrue;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteBootNotifySimStatusReady
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteBootNotifySimStatusReady(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteBootNotifySimStatusReady, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iBootNotifySimStatusReadyOngoing ||
        iHandleSpecifiedRequests )
        {
        iBootNotifySimStatusReadyOngoing = EFalse;
        _LIT8( KComplete, "CSecurity::CompleteBootNotifySimStatusReady, completed" );
        iSecurityLog->Log( ( TDesC8 ) KComplete );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::BootNotifySecurityReady
// -----------------------------------------------------------------------------
//
TInt CSecurity::BootNotifySecurityReady(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::BootNotifySecurityReady" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    iBootNotifySecurityReadyOngoing = ETrue;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteBootSecurityReady
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteBootNotifySecurityReady(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteBootSecurityReady, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iBootNotifySecurityReadyOngoing ||
        iHandleSpecifiedRequests )
        {
        iBootNotifySecurityReadyOngoing = EFalse;
        _LIT8( KComplete, "CSecurity::CompleteBootSecurityReady, completed" );
        iSecurityLog->Log( ( TDesC8 ) KComplete );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::BootGetRFStatusL
// -----------------------------------------------------------------------------
//
TInt CSecurity::BootGetRFStatusL(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::BootGetRFStatus" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    iBootGetRFStatusOngoing = ETrue;

    TInt ret( HandleRequestL( EMmTsyBootGetRFStatusIPC ) );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteBootGetRFStatus
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteBootGetRFStatus(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteBootGetRFStatus, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iBootGetRFStatusOngoing ||
        iHandleSpecifiedRequests )
        {
        iBootGetRFStatusOngoing = EFalse;
        TRfStateInfo* rfInfo = NULL;
        aData.UnPackData( &rfInfo );

        if ( *rfInfo == ERfsStateInfoInactive )
            {
            _LIT8( KState,
                "CSecurity::CompleteBootGetRFStatus, state = ERfsStateInfoInactive" );
            iSecurityLog->Log( ( TDesC8 ) KState );
            }
        else if ( *rfInfo == ERfsStateInfoNormal )
            {
            _LIT8( KState,
                "CSecurity::CompleteBootGetRFStatus, state = ERfsStateInfoNormal" );
            iSecurityLog->Log( ( TDesC8 ) KState );
            }
        else
            {
            _LIT8( KState,
                "CSecurity::CompleteBootGetRFStatus, state = bad state (%d)" );
            iSecurityLog->Log( ( TDesC8 ) KState, TInt( *rfInfo ) );
            aErr = KErrGeneral;
            }

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::PhoneGetPin1DisableSupportedL
// -----------------------------------------------------------------------------
//
TInt CSecurity::PhoneGetPin1DisableSupportedL(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::PhoneGetPin1DisableSupportedL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    ret = HandleRequestL( EMmTsyPhoneGetPin1DisableSupportedIPC );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompletePhoneGetPin1DisableSupported
// -----------------------------------------------------------------------------
//
void CSecurity::CompletePhoneGetPin1DisableSupported(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompletePhoneGetPin1DisableSupported, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    // packed parameter: TBool (supported or not)
    TBool supported( EFalse );
    aData.UnPackData( supported );

    if ( supported )
        {
        _LIT8( KSupport,
            "CSecurity::CompletePhoneGetPin1DisableSupported -- SUPPORTED" );
        iSecurityLog->Log( ( TDesC8 ) KSupport );
        }
    else
        {
        _LIT8( KSupport,
            "CSecurity::CompletePhoneGetPin1DisableSupported -- NOT SUPPORTED" );
        iSecurityLog->Log( ( TDesC8 ) KSupport );
        }

    Signal( aErr );
    }

// -----------------------------------------------------------------------------
// CSecurity::SecurityGetSimActivePinStateL
// -----------------------------------------------------------------------------
//
TInt CSecurity::SecurityGetSimActivePinStateL(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::SecurityGetSimActivePinStateL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    ret = HandleRequestL( EMmTsySecurityGetSimActivePinStateIPC );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteSecurityGetSimActivePinState
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteSecurityGetSimActivePinState(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteSecurityGetSimActivePinState, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    RMobilePhone::TMobilePhoneSecurityCode* activePin = NULL;
    // Get the new data
    aData.UnPackData( &activePin );

    switch ( *activePin )
        {
        case RMobilePhone::ESecurityCodePin1:
            {
            _LIT8( KActive,
                "CSecurity::CompleteSecurityGetSimActivePinState -- active pin = ESecurityCodePin1" );
            iSecurityLog->Log( ( TDesC8 ) KActive );
            break;
            }
        case RMobilePhone::ESecurityUniversalPin:
            {
            _LIT8( KActive,
                "CSecurity::CompleteSecurityGetSimActivePinState -- active pin = ESecurityUniversalPin" );
            iSecurityLog->Log( ( TDesC8 ) KActive );
            break;
            }
        default:
            {
            _LIT8( KActive,
                "CSecurity::CompleteSecurityGetSimActivePinState, bad pin (%d)" );
            iSecurityLog->Log( ( TDesC8 ) KActive, TInt( *activePin ) );
            aErr = KErrGeneral;
            break;
            }
        }

    Signal( aErr );
    }

// -----------------------------------------------------------------------------
// CSecurity::GetLockInfoL
// -----------------------------------------------------------------------------
//
TInt CSecurity::GetLockInfoL(
    CStifItemParser&  aItem )
    {
    _LIT8( KLog, "CSecurity::GetLockInfoL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    RMobilePhone::TMobilePhoneLock lock;
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
    lockInfo.iStatus = RMobilePhone::EStatusLockUnknown;
    lockInfo.iSetting = RMobilePhone::ELockSetUnknown;

    TInt ret( KErrNone );
    TPtrC string;

    ret = aItem.GetNextString( string );

    if ( KErrNone == ret )
        {
        if ( string == _L("ELockPhoneDevice") )
            {
            lock = RMobilePhone::ELockPhoneDevice;
            }
        else if ( string == _L("ELockICC") )
            {
            lock = RMobilePhone::ELockICC;
            }
        else if ( string == _L("ELockPhoneToICC") )
            {
            lock = RMobilePhone::ELockPhoneToICC;
            }
        else if ( string == _L("ELockPhoneToFirstICC") )
            {
            lock = RMobilePhone::ELockPhoneToFirstICC;
            }
        else if ( string == _L("ELockPhoneToFirstICC") )
            {
            lock = RMobilePhone::ELockPhoneToFirstICC;
            }
        else if ( string == _L("ELockOTA") )
            {
            lock = RMobilePhone::ELockOTA;
            }
        else if ( string == _L("ELockPin2") )
            {
            lock = RMobilePhone::ELockPin2;
            }
        else if ( string == _L("ELockHiddenKey") )
            {
            lock = RMobilePhone::ELockHiddenKey;
            }
        else if ( string == _L("ELockUSimApp") )
            {
            lock = RMobilePhone::ELockUSimApp;
            }
        else if ( string == _L("ELockSecondUSimApp") )
            {
            lock = RMobilePhone::ELockSecondUSimApp;
            }
        else if ( string == _L("ELockUniversalPin") )
            {
            lock = RMobilePhone::ELockUniversalPin;
            }
        else if ( string == _L("ELockNam") )
            {
            lock = RMobilePhone::ELockNam;
            }
        else
            {
            ret = KErrArgument;
            }
        }
    //no else

    aItem.GetNextString( string );
    if ( string == _L("SIMLockDisabled") )
        {
        iLockInfo.iSetting = RMobilePhone::ELockSetDisabled;
        }

    else if ( string == _L("SIMLockEnabled") )
        {
        iLockInfo.iSetting = RMobilePhone::ELockSetEnabled;
        }

    aItem.GetNextString( string );
    if ( string == _L("Unlocked") )
        {
        iLockInfo.iStatus = RMobilePhone::EStatusUnlocked;
        }

    else if ( string == _L("Locked") )
        {
        iLockInfo.iStatus = RMobilePhone::EStatusLocked;
        }

    if ( KErrNone == ret )
        {
        iGetLockInfoOngoing = ETrue;
        CMmDataPackage package;
        package.PackData( &lock, &lockInfo );

        ret = HandleRequestL( EMobilePhoneGetLockInfo, &package );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteGetLockInfo
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteGetLockInfo(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteGetLockInfo, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iGetLockInfoOngoing ||
        iHandleSpecifiedRequests )
        {
        iGetLockInfoOngoing = EFalse;

        //unpack the data
        RMobilePhone::TMobilePhoneLockStatus* status = NULL;
        RMobilePhone::TMobilePhoneLockSetting* setting = NULL;

        aData.UnPackData( &status, &setting );

        switch( *status )
            {
            case RMobilePhone::EStatusLockUnknown:
                {
                _LIT8( KStatus,
                    "CSecurity::CompleteGetLockInfo, status = EStatusLockUnknown" );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            case RMobilePhone::EStatusLocked:
                {
                _LIT8( KStatus,
                    "CSecurity::CompleteGetLockInfo, status = EStatusLocked" );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            case RMobilePhone::EStatusUnlocked:
                {
                _LIT8( KStatus,
                    "CSecurity::CompleteGetLockInfo, status = EStatusUnlocked" );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            case RMobilePhone::EStatusBlocked:
                {
                _LIT8( KStatus,
                    "CSecurity::CompleteGetLockInfo, status = EStatusBlocked" );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            default:
                {
                _LIT8( KStatus,
                    "CSecurity::CompleteGetLockInfo, bad status (%d)" );
                iSecurityLog->Log( ( TDesC8 ) KStatus, TInt( *status ) );
                aErr = KErrGeneral;
                break;
                }
            }

        switch( *setting )
            {
            case RMobilePhone::ELockSetUnknown:
                {
                _LIT8( KSetting,
                    "CSecurity::CompleteGetLockInfo, setting = ELockSetUnknown" );
                iSecurityLog->Log( ( TDesC8 ) KSetting );
                break;
                }
            case RMobilePhone::ELockSetEnabled:
                {
                _LIT8( KSetting,
                    "CSecurity::CompleteGetLockInfo, setting = ELockSetEnabled" );
                iSecurityLog->Log( ( TDesC8 ) KSetting );
                break;
                }
            case RMobilePhone::ELockSetDisabled:
                {
                _LIT8( KSetting,
                    "CSecurity::CompleteGetLockInfo, setting = ELockSetDisabled" );
                iSecurityLog->Log( ( TDesC8 ) KSetting );
                break;
                }
            case RMobilePhone::ELockReplaced:
                {
                _LIT8( KSetting,
                    "CSecurity::CompleteGetLockInfo, setting = ELockReplaced" );
                iSecurityLog->Log( ( TDesC8 ) KSetting );
                break;
                }
            default:
                {
                _LIT8( KSetting,
                    "CSecurity::CompleteGetLockInfo, bad setting (%d)" );
                iSecurityLog->Log( ( TDesC8 ) KSetting, TInt( *setting ) );
                aErr = KErrGeneral;
                break;
                }
            }
        if ( RMobilePhone::EStatusLockUnknown != iLockInfo.iStatus )
            {
            if ( iLockInfo.iStatus != *status )
                {
                aErr = KErrGeneral;
                }
            }
 
        if ( RMobilePhone::ELockSetUnknown != iLockInfo.iSetting )
            {
            if ( iLockInfo.iSetting != *setting )
                {
                aErr = KErrGeneral;
                }
            }

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::NotifyLockInfoChange
// -----------------------------------------------------------------------------
//
TInt CSecurity::NotifyLockInfoChange(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::NotifyLockInfoChange" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    ret = GetCodeID( aItem, iCodeID );

    if ( KErrNone == ret )
        {
        iNotifyLockInfoChangeOngoing = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteNotifyLockInfoChange
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteNotifyLockInfoChange(
    TInt aErr,
    CMmDataPackage& aData )
    {
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
    RMobilePhone::TMobilePhoneLock lock;
    aData.UnPackData( lockInfo, lock );

    _LIT8( KLog, "CSecurity::CompleteNotifyLockInfoChange, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    _LIT8( KInfo,
        "    -- lockInfo.iStatus = %d, lockInfo.iSetting = %d, lock = %d" );
    iSecurityLog->Log( ( TDesC8 ) KInfo,
        TInt( lockInfo.iStatus ), TInt( lockInfo.iSetting ), TInt( lock ) );

    if ( iNotifyLockInfoChangeOngoing )
        {
        iNotifyLockInfoChangeOngoing = EFalse;
        if ( RMobilePhone::ELockICC != lock ||
            RMobilePhone::EStatusUnlocked != lockInfo.iStatus ||
            RMobilePhone::ELockSetEnabled != lockInfo.iSetting )
            {
            _LIT8( KFailed, "CSecurity::CompleteNotifyLockInfoChange, Failed" );
            iSecurityLog->Log( ( TDesC8 ) KFailed );
            aErr = KErrGeneral;
            }
        else
            {
            _LIT8( KComplete, "CSecurity::CompleteNotifyLockInfoChange, completed" );
            iSecurityLog->Log( ( TDesC8 ) KComplete );
            }
        }

    if ( iNotifyLockInfoChangeOngoing ||
        iHandleSpecifiedRequests )
        {
        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::SetLockSettingL
// -----------------------------------------------------------------------------
//
TInt CSecurity::SetLockSettingL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::SetLockSettingL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    RMobilePhone::TMobilePhoneLock lock;
    RMobilePhone::TMobilePhoneLockSetting setting;

    TInt ret( KErrNone );
    TPtrC string;

    // 1st parameter: lock
    ret = aItem.GetNextString( string );

    if ( KErrNone == ret )
        {
        if ( string == _L("ELockPhoneDevice") )
            {
            lock = RMobilePhone::ELockPhoneDevice;
            }
        else if ( string == _L("ELockICC") )
            {
            lock = RMobilePhone::ELockICC;
            iCodeID = KSecCodePin;
            }
        else if ( string == _L("ELockPhoneToICC") )
            {
            lock = RMobilePhone::ELockPhoneToICC;
            }
        else if ( string == _L("ELockPhoneToFirstICC") )
            {
            lock = RMobilePhone::ELockPhoneToFirstICC;
            }
        else if ( string == _L("ELockPhoneToFirstICC") )
            {
            lock = RMobilePhone::ELockPhoneToFirstICC;
            }
        else if ( string == _L("ELockOTA") )
            {
            lock = RMobilePhone::ELockOTA;
            }
        else if ( string == _L("ELockPin2") )
            {
            lock = RMobilePhone::ELockPin2;
            }
        else if ( string == _L("ELockHiddenKey") )
            {
            lock = RMobilePhone::ELockHiddenKey;
            }
        else if ( string == _L("ELockUSimApp") )
            {
            lock = RMobilePhone::ELockUSimApp;
            }
        else if ( string == _L("ELockSecondUSimApp") )
            {
            lock = RMobilePhone::ELockSecondUSimApp;
            }
        else if ( string == _L("ELockUniversalPin") )
            {
            lock = RMobilePhone::ELockUniversalPin;
            }
        else if ( string == _L("ELockNam") )
            {
            lock = RMobilePhone::ELockNam;
            }
        else
            {
            ret = KErrArgument;
            }
        }
    //no else

    // 2nd parameter: lock setting
    ret = aItem.GetNextString( string );

    if ( KErrNone == ret )
        {
        if ( _L("ELockSetUnknown") == string )
            {
            setting = RMobilePhone::ELockSetUnknown;
            }
        else if ( _L("ELockSetEnabled") == string )
            {
            setting = RMobilePhone::ELockSetEnabled;
            }
        else if ( _L("ELockSetDisabled") == string )
            {
            setting = RMobilePhone::ELockSetDisabled;
            }
        else if ( _L("ELockReplaced") == string )
            {
            setting = RMobilePhone::ELockReplaced;
            //made for checking right complete value in SEC_EVENT IND
            if ( RMobilePhone::ELockICC == lock )
                {
                iLockReplaceTestOngoing = ETrue;
                iCodeID = KSecCodeSubstitutePin;
                }
            else if ( RMobilePhone::ELockUniversalPin == lock )
                {
                iLockReplaceTestOngoing = ETrue;
                iCodeID = KSecCodeSubstitutePinUniversal;
                }
            }
        else
            {
            ret = KErrArgument;
            }
        }
    //no else

    if ( KErrNone == ret )
        {
        CMmDataPackage package;
        TLockAndSetting lockAndSetting;
        lockAndSetting.iLock = &lock;
        lockAndSetting.iSetting = &setting;

        //check if there is a code available for this message
        //some lock settings requires code
        if ( iSecCode.Length() != 0 )
            {
            //re-package the original package and
            //include the code
            _LIT16( KCode, "CSecurity::SetLockSettingL, security code = %S" );
            iSecurityLog->Log( ( TDesC16 ) KCode, &iSecCode );

            package.PackData( &lockAndSetting, &iSecCode );
            }
        else
            {
            //pack only the lock and the setting, the code will be null
            package.PackData( &lockAndSetting );
            }

        iSetLockSettingOngoing = ETrue;
        ret = HandleRequestL( EMobilePhoneSetLockSetting, &package );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteSetLockSetting
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteSetLockSetting(
    TInt aErr, CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteSetLockSetting, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iSetLockSettingOngoing ||
        iHandleSpecifiedRequests )
        {
        iSetLockSettingOngoing = EFalse;

        RMobilePhone::TMobilePhoneLockStatus status;
        RMobilePhone::TMobilePhoneLockSetting setting;
        aData.UnPackData( status, setting );

        _LIT8( KComplete,
            "CSecurity::CompleteSetLockSetting, status = %d setting = %d");
        iSecurityLog->Log( ( TDesC8 ) KComplete,
            TInt( status ), TInt( setting ) );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::ChangeSecurityCodeL
// -----------------------------------------------------------------------------
//
TInt CSecurity::ChangeSecurityCodeL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::ChangeSecurityCodeL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );
    TPtrC  string;

    RMobilePhone::TMobilePhoneSecurityCode securityCode;
    RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;

    // 1st parameter: security code enum
    ret = GetSecurityCode( aItem, securityCode );

    // 2nd parameter: old password
    if ( KErrNone == ret )
        {
        // get oldpassword
        ret = aItem.GetNextString( string );

        if ( KErrNone == ret )
            {
            passwordChange.iOldPassword.Copy( string );
            }
        //no else
        }
    //no else

    // 3rd parameter: newpassword
    if ( KErrNone == ret )
        {
        ret = aItem.GetNextString( string );

        if ( KErrNone == ret )
            {
            passwordChange.iNewPassword.Copy( string );
            }
        //no else
        }
    //no else

    if ( KErrNone == ret )
        {
        CMmDataPackage package;
        package.PackData( &securityCode, &passwordChange );
        iChangeSecurityCodeOngoing = ETrue;
        ret = HandleRequestL ( EMobilePhoneChangeSecurityCode, &package );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteChangeSecurityCode
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteChangeSecurityCode(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteChangeSecurityCode, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iChangeSecurityCodeOngoing ||
        iHandleSpecifiedRequests )
        {
        iChangeSecurityCodeOngoing = EFalse;

        _LIT8( KComplete, "CSecurity::CompleteChangeSecurityCode, completed" );
        iSecurityLog->Log( ( TDesC8 ) KComplete );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::NotifySecurityEvent
// -----------------------------------------------------------------------------
//
TInt CSecurity::NotifySecurityEvent(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::NotifySecurityEvent" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    // 1st parameter: codeID
    ret = GetCodeID( aItem, iCodeID );

    if ( KErrNone == ret )
        {
        iNotifySecurityEventOngoing = ETrue;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSecurity::CompleteNotifySecurityEvent
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteNotifySecurityEvent(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteNotifySecurityEvent, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    RMobilePhone::TMobilePhoneSecurityEvent* event = NULL;
    //unpack the data
    aData.UnPackData( &event );

    switch( *event )
        {
        case RMobilePhone::ENoICCFound:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = ENoICCFound" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EICCTerminated:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EICCTerminated" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EPin1Required:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPin1Required" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );

            if ( KSecCodePin != iCodeID )
                {
                aErr = KErrGeneral;
                }
            break;
            }
        case RMobilePhone::EPin2Required:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPin2Required" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EPuk2Required:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPuk2Required" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EPhonePasswordRequired:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPhonePasswordRequired" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::ESPCRequired:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = ESPCRequired" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EPin1Verified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPin1Verified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            
            if ( iLockReplaceTestOngoing )
                {
                if ( KSecCodeSubstitutePin != iCodeID )
                    {
                    aErr = KErrGeneral; 
                    }
                iLockReplaceTestOngoing = EFalse;
                }
            else if ( KSecCodePin != iCodeID)
                {
                aErr = KErrGeneral;
                }
            break;
            }
        case RMobilePhone::EPin2Verified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPin2Verified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EPuk1Verified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPuk1Verified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EPuk2Verified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPuk2Verified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EPhonePasswordVerified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EPhonePasswordVerified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::ESPCVerified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = ESPCVerified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EHiddenKeyRequired:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EHiddenKeyRequired" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EHiddenKeyVerified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EHiddenKeyVerified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EUSIMAppPinRequired:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EUSIMAppPinRequired" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EUSIMAppPinVerified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EUSIMAppPinVerified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EUniversalPinRequired:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EUniversalPinRequired" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::ESPCChanged:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = ESPCChanged" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EUniversalPukRequired:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EUniversalPukRequired" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EUniversalPukVerified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EUniversalPukVerified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            break;
            }
        case RMobilePhone::EUniversalPinVerified:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, event = EUniversalPinVerified" );
            iSecurityLog->Log( ( TDesC8 ) KEvent );
            if ( KSecCodeSubstitutePinUniversal != iCodeID)
                {
                aErr = KErrGeneral;
                }
            break;
            }
        default:
            {
            _LIT8( KEvent, "CSecurity::CompleteNotifySecurityEvent, bad event (%d)" );
            iSecurityLog->Log( ( TDesC8 ) KEvent, TInt( *event ) );
            aErr = KErrGeneral;
            break;
            }
        }

    if ( iNotifySecurityEventOngoing ||
        iHandleSpecifiedRequests )
        {
        iNotifySecurityEventOngoing = EFalse;

        _LIT8( KComplete, "CSecurity::CompleteNotifySecurityEvent, completed, err = %d" );
        iSecurityLog->Log( ( TDesC8 ) KComplete, aErr );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::VerifySecurityCodeL
// -----------------------------------------------------------------------------
//
TInt CSecurity::VerifySecurityCodeL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::VerifySecurityCodeL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );
    TPtrC string;

    RMobilePhone::TMobilePhoneSecurityCode type;
    RMobilePhone::TCodeAndUnblockCode codes;

    // 1st parameter: security code type
    ret = GetSecurityCode( aItem, type );

    // 2nd parameter code
    if ( KErrNone == ret )
        {
        ret = aItem.GetNextString( string );
        if ( KErrNone == ret )
            {
            codes.iCode.Copy( string );
            }
        //no else
        }
    //no else

    // 3rd parameter: unblock code
    if ( KErrNone == ret )
        {
        ret = aItem.GetNextString( string );
        if ( KErrNone == ret )
            {
            codes.iUnblockCode.Copy( string );
            }
        //no else
        }
    //no else

    // if 3rd parameter has been left empty (no new code), ret == -1
    if ( KErrNone == ret || -1 == ret )
        {
        if ( RMobilePhone::ESecurityCodePin1 == type )
            {
            iCodeID = KSecCodePin;
            }

        CMmDataPackage package;
        package.PackData( &type, &codes );

        ret = HandleRequestL( EMobilePhoneVerifySecurityCode, &package );
        iVerifySecurityCodeOngoing = ETrue;
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteVerifySecurityCode
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteVerifySecurityCode(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteVerifySecurityCode, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iVerifySecurityCodeOngoing ||
        iHandleSpecifiedRequests )
        {
        iVerifySecurityCodeOngoing = EFalse;
        _LIT8( KComplete, "CSecurity::CompleteVerifySecurityCode, completed" );
        iSecurityLog->Log( ( TDesC8 ) KComplete );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::AbortSecurityCodeL
// -----------------------------------------------------------------------------
//
TInt CSecurity::AbortSecurityCodeL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::AbortSecurityCodeL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );
    RMobilePhone::TMobilePhoneSecurityCode securityCode;

    // 1st parameter: security code type
    ret = GetSecurityCode( aItem, securityCode );

    if ( KErrNone == ret )
        {
        CMmDataPackage package;
        package.PackData( &securityCode );

        ret = HandleRequestL( EMobilePhoneAbortSecurityCode, &package );
        iAbortSecurityCodeOngoing = ETrue;
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteAbortSecurityCode
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteAbortSecurityCode(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteAbortSecurityCode, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iAbortSecurityCodeOngoing ||
        iHandleSpecifiedRequests )
        {
        iAbortSecurityCodeOngoing = EFalse;

        _LIT8( KComplete, "CSecurity::CompleteAbortSecurityCode, completed" );
        iSecurityLog->Log( ( TDesC8 ) KComplete );

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::SimGetFdnStatusL
// -----------------------------------------------------------------------------
//
TInt CSecurity::SimGetFdnStatusL(
    CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CSecurity::SimGetFdnStatusL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    iGetFdnStatusOngoing = ETrue;
    ret = HandleRequestL( EMobilePhoneGetFdnStatus );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteGetFdnStatus
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteGetFdnStatus(
    TInt aErr, CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteGetFdnStatus, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iGetFdnStatusOngoing ||
        iHandleSpecifiedRequests )
        {
        iGetFdnStatusOngoing = EFalse;

        //unpack and print status
        RMobilePhone::TMobilePhoneFdnStatus fdnStatus = RMobilePhone::EFdnUnknown;
        aData.UnPackData( fdnStatus );

        switch ( fdnStatus )
            {
            case RMobilePhone::EFdnNotActive:
                {
                _LIT8( KStatus, "CSecurity::CompleteGetFdnStatus, FDN Not Active." );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            case RMobilePhone::EFdnActive:
                {
                _LIT8( KStatus, "CSecurity::CompleteGetFdnStatus, FDN Active." );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            case RMobilePhone::EFdnPermanentlyActive:
                {
                _LIT8( KStatus, "CSecurity::CompleteGetFdnStatus, FDN Permanently Active." );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            case RMobilePhone::EFdnNotSupported:
                {
                _LIT8( KStatus, "CSecurity::CompleteGetFdnStatus, FDN Not Supported." );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            case RMobilePhone::EFdnUnknown:
            default:
                {
                _LIT8( KStatus, "CSecurity::CompleteGetFdnStatus, FDN Status Unknown." );
                iSecurityLog->Log( ( TDesC8 ) KStatus );
                break;
                }
            }

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::SimSetFdnStatusL
// -----------------------------------------------------------------------------
//
TInt CSecurity::SimSetFdnStatusL(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::SimSetFdnStatusL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    TInt fdnInt( 0 );
    TInt ret( aItem.GetNextInt( fdnInt ) );

    if ( KErrNone == ret )
        {
        RMobilePhone::TMobilePhoneFdnSetting fdnSetting =
            static_cast< RMobilePhone::TMobilePhoneFdnSetting >( fdnInt );

        if ( RMobilePhone::EFdnSetOn == fdnSetting )
            {
            _LIT8( KSetting, "CSecurity::SimSetFdnStatusL: Setting FDN Active." );
            iSecurityLog->Log( ( TDesC8 ) KSetting );
            }
        else
            {
            _LIT8( KSetting, "CSecurity::SimSetFdnStatusL: Setting FDN Inactive." );
            iSecurityLog->Log( ( TDesC8 ) KSetting );
            }

        iSetFdnStatusOngoing = ETrue;

        CMmDataPackage package;
        package.PackData( &fdnSetting );

        ret = HandleRequestL( EMobilePhoneSetFdnSetting, &package );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteSetFdnStatus
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteSetFdnStatus(
    TInt aErr,
    CMmDataPackage& /*aData*/ )
    {
    _LIT8( KLog, "CSecurity::CompleteSetFdnStatus, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iSetFdnStatusOngoing ||
        iHandleSpecifiedRequests )
        {
        iSetFdnStatusOngoing = EFalse;

        Signal( aErr );
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CSecurity::SecurityInfoL
// -----------------------------------------------------------------------------
//
TInt CSecurity::SecurityInfoL( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::SecurityInfoL" );
    iSecurityLog->Log( ( TDesC8 ) KLog );

    RMobilePhone::TMobilePhoneSecurityCode securityCode;

    TInt ret( KErrNone );

    // 1st parameter: security code enum
    ret = GetSecurityCode( aItem, securityCode );

    if ( ret == KErrNone )
        {
        CMmDataPackage package;
        package.PackData( &securityCode );
        iRemainingAttempsLeftOngoing = ETrue;
        ret = HandleRequestL( EMobilePhoneGetSecurityCodeInfo, &package );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSecurity::SecurityInfoComplete
// -----------------------------------------------------------------------------
//
void CSecurity::SecurityInfoComplete( TInt aErr, CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::SecurityInfoComplete, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    if ( iRemainingAttempsLeftOngoing ||
        iHandleSpecifiedRequests )
        {
        RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityInfo;
        RMobilePhone::TMobilePhoneSecurityCode type;

        // unpack the data
        aData.UnPackData( type, securityInfo );

        iRemainingAttempsLeftOngoing = EFalse;

        _LIT8( KAttempts, "CSecurity::SecurityInfoComplete, value for attempts = %d" );
        iSecurityLog->Log( ( TDesC8 ) KAttempts, securityInfo.iRemainingEntryAttempts );

        Signal( aErr );
        }
    }

// -----------------------------------------------------------------------------
// CSecurity::CompleteSecurityCodeInfoChange
// -----------------------------------------------------------------------------
//
void CSecurity::CompleteSecurityCodeInfoChange(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CSecurity::CompleteSecurityCodeInfoChange, err = %d" );
    iSecurityLog->Log( ( TDesC8 ) KLog, aErr );

    // This should be called after veriying/changing security code

    if ( iVerifySecurityCodeOngoing ||
        iHandleSpecifiedRequests )
        {
        iVerifySecurityCodeOngoing = EFalse;

        RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityInfo;
        RMobilePhone::TMobilePhoneSecurityCode type;
        // unpack the data
        aData.UnPackData( type, securityInfo );

        _LIT8( KAttempts, "CSecurity::CompleteSecurityCodeInfoChange, value for attempts = %d" );
        iSecurityLog->Log( ( TDesC8 ) KAttempts, securityInfo.iRemainingEntryAttempts );

        if ( 3 != securityInfo.iRemainingEntryAttempts )
            {
            _LIT8( KComplete, "CSecurity::CompleteSecurityCodeInfoChange, Failed" );
            iSecurityLog->Log( ( TDesC8 ) KComplete );
            aErr = KErrGeneral;
            }
        else
            {
            _LIT8( KComplete, "CSecurity::CompleteSecurityCodeInfoChange, completed" );
            iSecurityLog->Log( ( TDesC8 ) KComplete );
            }

        Signal( aErr );
        }
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
