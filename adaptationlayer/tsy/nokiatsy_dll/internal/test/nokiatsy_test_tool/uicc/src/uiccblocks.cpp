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
* Description:  Specific class for UICC tests
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <etelmm.h>
#include <ctsy/serviceapi/gsmerror.h>
#include "uicc.h"


// CONSTANTS
_LIT8( KLine, "---------------------------------" );
_LIT8( KAdnPhoneBook, "AdnPhoneBook" );
_LIT8( KBdnPhoneBook, "BdnPhoneBook" );
_LIT8( KSdnPhoneBook, "SdnPhoneBook" );
_LIT8( KFdnPhoneBook, "FdnPhoneBook" );
_LIT8( KMBDN, "MBDN" );
_LIT8( KVoiceMailBox, "VoiceMailBox" );

_LIT( KViagInactive, "Inactive" );
_LIT( KViagCZActive, "CZActive" );
_LIT( KViagHZActive, "HZActive" );


// ============================ LOCAL FUNCTIONS ===============================
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

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUICC::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CUICC::Delete()
    {
    if(iRefPBStoreInfoData)
        {
        delete iRefPBStoreInfoData;
        iRefPBStoreInfoData = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUICC::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CUICC::RunMethodL(
    CStifItemParser& aItem )
    {
    _LIT8 (KLog, "UICC: RunMethodL" );
    iUICCLog->Log((TDesC8)KLog);

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "PoweringSimOn",                             CUICC::PoweringSimOn ),
        ENTRY( "GetLifeTimer",                              CUICC::GetLifeTimer),
        ENTRY( "RunIscTestCase",                            CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase",                           CCore::SyncIscTestCase),
        ENTRY( "RunIscTestCaseStartNTSYAfterTestToolStart",
                CCore::RunIscTestCaseStartNTSYAfterTestToolStart),
        ENTRY( "SyncIscTestCaseStartNTSYAfterTestToolStart",
                CCore::SyncIscTestCaseStartNTSYAfterTestToolStart),
        ENTRY( "HandleSpecifiedRequests",                   CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable",                 CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC",                    CCore::SetExpectedCompleteIPC),
        ENTRY( "GetSubscriberId",                           CUICC::GetSubscriberId ),
        ENTRY( "GetCiStatus",                               CUICC::GetCiStatus),
        ENTRY( "SetCallFwdStatus",                          CUICC::SetCallFwdStatus),
        ENTRY( "GetEhplmn",                                 CUICC::GetEhplmn),
        ENTRY( "PbInit",                                    CUICC::PbInitL ),
        ENTRY( "GetServiceProviderName",                    CUICC::GetServiceProviderName ),
        ENTRY( "GetCustomerServiceProfile",                 CUICC::GetCustomerServiceProfile ),
        ENTRY( "ReadSimFile",                               CUICC::ReadSimFile ),
        ENTRY( "GetMailboxIdentifier",                      CUICC::GetMailboxIdentifier ),
        ENTRY( "ReadCBSSimTopics",                          CUICC::ReadCBSSimTopics ),
        ENTRY( "DeleteCBSSimTopic",                         CUICC::DeleteCBSSimTopic ),
        ENTRY( "GetCiStatusCustom",                         CUICC::GetCiStatusCustom ),
        ENTRY( "GetNetworkProviderName",                    CUICC::GetNetworkProviderName ),
        ENTRY( "ViagHomeZoneReadParam",                     CUICC::ViagHomeZoneReadParamL),
        ENTRY( "ViagHomeZoneReadCache",                     CUICC::ViagHomeZoneReadCacheL),
        ENTRY( "ViagHomeZoneWriteSettings",                 CUICC::ViagHomeZoneWriteSettingsL),
        ENTRY( "ViagHomeZoneWriteCache",                    CUICC::ViagHomeZoneWriteCacheL),
        ENTRY( "ReadVoiceMsgFlagsRel4",                     CUICC::ReadVoiceMsgFlagsRel4),
        ENTRY( "WriteVoiceMsgFlagsRel4",                    CUICC::WriteVoiceMsgFlagsRel4),
        ENTRY( "ReadVoiceMsgFlagsCphs",                     CUICC::ReadVoiceMsgFlagsCphs),
        ENTRY( "WriteVoiceMsgFlagsCphs",                    CUICC::WriteVoiceMsgFlagsCphs),
        ENTRY( "ReadDynamicFlags",                          CUICC::ReadDynamicFlags),
        ENTRY( "WriteDynamicFlags",                         CUICC::WriteDynamicFlags),
        ENTRY( "ReadDynamic2Flags",                         CUICC::ReadDynamic2Flags),
        ENTRY( "WriteDynamic2Flags",                        CUICC::WriteDynamic2Flags),
        ENTRY( "GetUiccStatus",                             CUICC::GetUiccStatus),
        ENTRY( "PowerOffSim",                               CUICC::PowerOffSim ),
        ENTRY( "SimWarmReset",                              CUICC::SimWarmReset ),
        ENTRY( "SimGetATR",                                 CUICC::SimGetATR ),
        ENTRY( "PowerOnSim",                                CUICC::PowerOnSim ),
        ENTRY( "GetRfStatus",                               CUICC::GetRfStatus),
        ENTRY( "SendAPDUv2",                                CUICC::SendAPDUv2 ),
        ENTRY( "GetCardReaderStatus",                       CUICC::GetCardReaderStatus ),
        ENTRY( "SIMCardInd",                                CUICC::UiccCardInd ),
        ENTRY( "SendAPDU",                                  CUICC::SendAPDU ),
        ENTRY( "NotifySecurityEvent",                       CUICC::NotifySecurityEvent ),
        ENTRY( "VerifySecurityCode",                        CUICC::VerifySecurityCode ),
        ENTRY( "GetPinInfo",                                CUICC::GetPinInfo ),
        ENTRY( "ChangePinCode",                             CUICC::ChangePinCode ),
        ENTRY( "SetLockSetting",                            CUICC::SetLockSetting ),
        ENTRY( "GetPinAttemptsLeft",                        CUICC::GetPinAttemptsLeft ),
        ENTRY( "GetIccType",                                CUICC::GetIccType ),
        ENTRY( "GetCurrentActiveUSIMApplication",           CUICC::GetCurrentActiveUSIMApplication ),
        ENTRY( "GetActivePin",                              CUICC::GetActivePin ),
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CUICC::PoweringSIMOn
// Powering SIM on
// -----------------------------------------------------------------------------
//
TInt CUICC::PoweringSimOn( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KPbInit, "CUICC::PoweringSimOn");
    iUICCLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ECustomPowerSimOnIPC  );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "PoweringSimOn request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "PoweringSimOn request send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::CompletePoweringSIMOn
// Complete PoweringSIMOn method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompletePoweringSimOn( TInt aResult )
    {
    _LIT8( KResult, "CompletePoweringSimOn result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePoweringSimOn Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CVoice::GetLifeTimer
// Request Get Call Life Timer value.
// -----------------------------------------------------------------------------
//
TInt CUICC::GetLifeTimer( CStifItemParser& aItem )
    {
    TInt ret( KErrNone );
    _LIT8(KGetLifeTimer, "CVoice::GetLifeTimer");
    iUICCLog->Log((TDesC8)KGetLifeTimer);

    TUint hours( 0 );
    TUint minutes( 0 );

    aItem.GetNextInt( hours );
    aItem.GetNextInt( minutes );

    iLifeTimerForComparison.iHours = hours;
    iLifeTimerForComparison.iMinutes = static_cast<TUint8>( minutes );

    ret = HandleRequestL( ECustomGetLifeTimeIPC );

    if ( KErrNone != ret )
        {
        // Signal ( ret );
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteGetLifeTimer
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetLifeTimer( TInt aResult, CMmDataPackage* aDataPackage )
    {
    RMmCustomAPI::TLifeTimeData timeInfo;

    _LIT8( KCompleteGetLifeTimer, "CVoice::CompleteGetLifeTimer: %d" );
    iUICCLog->Log((TDesC8)KCompleteGetLifeTimer, aResult );

    aDataPackage->UnPackData( timeInfo );

    if ( ( ( iLifeTimerForComparison.iHours != timeInfo.iHours ) ||
           ( iLifeTimerForComparison.iMinutes != timeInfo.iMinutes ) ) &&
         ( KErrNone == aResult ) )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::GetSubscriberId
// Get Subscriber Id (IMSI)
// -----------------------------------------------------------------------------
//
TInt CUICC::GetSubscriberId( CStifItemParser& aItem )
    {
    _LIT8(KIMSI, "UICC::GetSubscriberId");
    iUICCLog->Log((TDesC8)KIMSI);

    TInt ret( KErrNotFound );

    // store given parameters for validity checking
    if ( iHandleSpecifiedRequests )
        {
        TPtrC temp;
        aItem.GetNextString( temp );
        iImsi.Append(temp);
        }

    ret = HandleRequestL( EMobilePhoneGetSubscriberId );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetSubscriberId request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetSubscriberId request send failed: %d" );
         iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetSubscriberId
// Complete GetSubscriberId method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetSubscriberId( TInt aResult, CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CompleteGetSubscriberId result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        TBuf8<RMobilePhone::KIMSISize> imsi;
        aDataPackage->UnPackData( imsi );
        iUICCLog->Log((TDesC8)KLine );
        _LIT8( KIMSI, "IMSI:" );
        for( TInt i( 0 ); i < RMobilePhone::KIMSISize; i++)
            {
            iUICCLog->Log((TDesC8)KIMSI );
            _LIT8( KIMSI2, " %d" );
            iUICCLog->Log((TDesC8)KIMSI2, imsi[i] );
            }
        iUICCLog->Log((TDesC8)KLine );

        // Validity checking
        if ( iHandleSpecifiedRequests )
            {
            if( imsi != iImsi )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetSubscriberId Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::GetCiStatus
// Get ciphering indicator status
// -----------------------------------------------------------------------------
//
TInt CUICC::GetCiStatus( CStifItemParser& aItem )
    {
    _LIT8(KCI, "UICC::GetCiStatus");
    iUICCLog->Log((TDesC8)KCI);

    TInt ret( KErrNone );

    TInt ciStatus( 0 );
    aItem.GetNextInt( ciStatus );
    iCipherIndicatorStatus =
        static_cast<RMobilePhone::TMobileCallCipheringIndicator>( ciStatus );

    ret = HandleRequestL( EMobilePhoneGetCipheringIndicatorStatus );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetCiStatus request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetCiStatus request send failed: %d" );
         iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::GetCiStatus
// Get ciphering indicator status
// -----------------------------------------------------------------------------
//
TInt CUICC::GetCiStatusCustom( CStifItemParser& aItem )
    {
    _LIT8(KCI, "UICC::GetCiStatusCustom");
    iUICCLog->Log((TDesC8)KCI);

    TInt ret( KErrNone );

    TInt ciStatus( 0 );
    aItem.GetNextInt( ciStatus );
    iCipherIndicatorStatus =
        static_cast<RMobilePhone::TMobileCallCipheringIndicator>( ciStatus );

    ret = HandleRequestL( ECustomGetCipheringInfoIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetCiStatus request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetCiStatus request send failed: %d" );
         iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::CompleteGetCiStatus
// Complete CompleteGetCiStatus method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetCiStatus( TInt aResult, CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CompleteGetCiStatus result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        RMobilePhone::TMobileCallCipheringIndicator cipherIndicatorStatus;
        aDataPackage->UnPackData( cipherIndicatorStatus );

        if ( iCipherIndicatorStatus != cipherIndicatorStatus )
            {
            aResult = KErrGeneral;
            }

        Signal( aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteGetCiStatus failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::SetCallFwdStatus
// Set call forwarding status
// -----------------------------------------------------------------------------
//
TInt CUICC::SetCallFwdStatus( CStifItemParser& aItem )
    {
    _LIT8(KCallFwd, "CUICC::SetCallForwardingStatus");
    iUICCLog->Log((TDesC8)KCallFwd);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    TPtrC groupstring;
    TPtrC actionstring;
    TPtrC numberstring;
    TPtrC timestring;
    TPtrC typestring;
    TPtrC planstring;

    aItem.GetNextString( string );
    aItem.GetNextString( groupstring );
    aItem.GetNextString( actionstring );
    aItem.GetNextString( typestring );
    aItem.GetNextString( planstring );
    aItem.GetNextString( numberstring );
    aItem.GetNextString( timestring );

    RMobilePhone::TMobilePhoneCFCondition condition;

    // Save CF indicators for parameter checking
    aItem.GetNextInt( iIndicator );

    if ( _L("CallForwardingUnspecified") == string )
        {
        condition = RMobilePhone::ECallForwardingUnspecified;
        }
    else if ( _L("CallForwardingUnconditional") == string )
        {
        condition = RMobilePhone::ECallForwardingUnconditional;
        }
    else if ( _L("CallForwardingBusy") == string )
        {
        condition = RMobilePhone::ECallForwardingBusy;
        }
    else if ( _L("CallForwardingNoReply") == string )
        {
        condition = RMobilePhone::ECallForwardingNoReply;
        }
    else if ( _L("CallForwardingNotReachable") == string )
        {
        condition = RMobilePhone::ECallForwardingNotReachable;
        }
    else if ( _L("CallForwardingAllCases") == string )
        {
        condition = RMobilePhone::ECallForwardingAllCases;
        }
    else if ( _L("CallForwardingAllConditionalCases") == string )
        {
        condition = RMobilePhone::ECallForwardingAllConditionalCases;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected condition for call forwarding not supported");
        iUICCLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported && iHandleSpecifiedRequests )
        {
        iCFCondition = condition;
        }

    RMobilePhone::TMobilePhoneCFChangeV1 group;

    if ( _L("ServiceUnspecified") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EServiceUnspecified;
        }
    else if ( _L("VoiceService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EVoiceService;
        }
    else if ( _L("AuxVoiceService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAuxVoiceService;
        }
    else if ( _L("CircuitDataService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::ECircuitDataService;
        }
    else if ( _L("PacketDataService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EPacketDataService;
        }
    else if ( _L("FaxService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EFaxService;
        }
    else if ( _L("ShortMessageService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EShortMessageService;
        }
    else if ( _L("AllServices") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllServices;
        }
    else if ( _L("AllTele") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllTele;
        }
    else if ( _L("Telephony") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::ETelephony;
        }
    else if ( _L("AllDataTele") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllDataTele;
        }
     else if ( _L("AllDataExSms") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllDataExSms;
        }
    else if ( _L("AllTeleExcSms") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllTeleExcSms;
        }
    else
        {
        _LIT8(KNotSupp, "UICC::Selected set call forwarding servicegroup not supported");
        iUICCLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( _L("ServiceActionRegister") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionRegister;
        }
    else if ( _L("ServiceActionActivate") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionActivate;
        }
    else if ( _L("ServiceActionDeactivate") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionDeactivate;
        }
    else if ( _L("ServiceActionErase") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionErase;
        }
    else
        {
        _LIT8(KNotSupp, "UICC::Selected set call forwarding action not supported");
        iUICCLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( _L("UnknownNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
        }
    else if ( _L("InternationalNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
        }
    else if ( _L("NationalNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
        }
    else if ( _L("NetworkSpecificNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::ENetworkSpecificNumber;
        }
    else if ( _L("SubscriberNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::ESubscriberNumber;
        }
    else if ( _L("AlphanumericNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
        }
    else if ( _L("AbbreviatedNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EAbbreviatedNumber;
        }
    else
        {
        _LIT8(KNotSupp, "UICC::Selected set call forwarding type of number not supported");
        iUICCLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( _L("UnknownNumberingPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
        }
    else if ( _L("IsdnNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
        }
    else if ( _L("DataNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EDataNumberPlan;
        }
    else if ( _L("TelexNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::ETelexNumberPlan;
        }
    else if ( _L("ServiceCentreSpecificPlan1") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EServiceCentreSpecificPlan1;
        }
    else if ( _L("ServiceCentreSpecificPlan2") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EServiceCentreSpecificPlan2;
        }
    else if ( _L("NationalNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
        }
    else if ( _L("PrivateNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EPrivateNumberPlan;
        }
    else if ( _L("ERMESNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EERMESNumberPlan;
        }
    else
        {
        _LIT8(KNotSupp, "UICC::Selected set call forwarding number plan not supported");
        iUICCLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    group.iNumber.iTelNumber.Copy(numberstring);

    if ( _L("CallForwardingNoReply") == string && _L("ServiceActionRegister") == actionstring)
        {

        TLex lex(timestring);

        TInt dataValue = 0;
        lex.Val(dataValue);

        for(TInt i = 5; i < 30; i = i+5)
            {
            if( dataValue == i )
                {
                group.iTimeout = i;
                }

            if( dataValue != i && i == 30 )
                {
                _LIT8(KNotSupp, "UICC::Selected timeout is not supported");
                iUICCLog->Log((TDesC8)KNotSupp);
                supported = EFalse;
                }
            }
        }
    else
        {
        group.iTimeout = -1;
        }

    if( supported )
        {
        RMobilePhone::TMobilePhoneCFChangeV1* groupPtr = &group;

         //Create package
        CMmDataPackage package;
        // Set package data
        package.PackData( &condition, &groupPtr);

        ret = HandleRequestL( EMobilePhoneSetCallForwardingStatus, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetCallForwardingStatus request send ok" );
            iUICCLog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "SetCallForwardingStatus request send failed: %d" );
            iUICCLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::CompleteSetCallFwdStatus
// Complete CompleteSetCallFwdStatus method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteSetCallFwdStatus(
    TInt aResult, CMmDataPackage* /*aDataPackage*/ )
    {
    _LIT8( KResult, "CompleteSetCallFwdStatus result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        Signal( aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteSetCallFwdStatus failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteNotifyIccCallForwardingStatusChange
// Complete CompleteNotifyIccCallForwardingStatusChange method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteNotifyIccCallForwardingStatusChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CompleteNotifyIccCallForwardingStatusChange result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    RMmCustomAPI::TCFIndicators iTCFIndicators;
    aDataPackage->UnPackData( iTCFIndicators );

    if( KErrNone == aResult )
        {
        if( iIndicator != iTCFIndicators.iIndicator )
            {
            aResult = KErrGeneral;
            }
        Signal( aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteNotifyIccCallForwardingStatusChange failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::GetEhplmn
// -----------------------------------------------------------------------------
//
TInt CUICC::GetEhplmn( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KLog, "UICC::GetEhplmn");
    iUICCLog->Log((TDesC8)KLog);

    TInt ret( KErrNone );
    ret = HandleRequestL( EMobilePhoneGetHomeNetwork );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetEhplmn request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetEhplmn request send failed: %d" );
         iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::CompleteGetGetEhplmn
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetEhplmn( TInt aResult, CMmDataPackage* /*aDataPackage*/ )
    {
    _LIT8( KResult, "CompleteGetGetEhplmn result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        Signal( aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteGetCiStatus failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::PbInitL
// Phonebook initialization
// -----------------------------------------------------------------------------
//
TInt CUICC::PbInitL( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "UICC::PbInit");
    iUICCLog->Log((TDesC8)KPbInit);
    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString( string );
    if ( _L("Adn") == string )
        {
        iUICCLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        if ( iHandleSpecifiedRequests )
            {
            TInt value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iIsPhonebookInitialized = ( TBool )value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iIsPhonebookInitializeFailed = ( TBool )value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iADNNumOfEntries = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iADNTextLengthMax = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iADNNumberLengthMax = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iSNENumOfEntries = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iSNENumOfEntriesPerEntry = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iSNETextLengthMax = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iEmailNumOfEntries = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iEmailNumOfEntriesPerEntry = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iEmailTextLengthMax = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iGRPNumOfEntriesPerEntry = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iANRNumOfEntries = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iANRNumOfEntriesPerEntry = value;
            aItem.GetNextInt( value );
            iRefPBStoreInfoData->iANRNumLengthMax = value;
            aItem.GetNextInt( value );
            iPbType[value] = TPhoneBookTypeAdn;
            }
        }
    else if ( _L("Bdn") == string )
        {
        iUICCLog->Log((TDesC8)KBdnPhoneBook);
        iPhoneBookName = KETelIccBdnPhoneBook;
        }
    else if ( _L("Sdn") == string )
        {
        iUICCLog->Log((TDesC8)KSdnPhoneBook);
        iPhoneBookName = KETelIccSdnPhoneBook;
        if ( iHandleSpecifiedRequests )
            {
            TInt value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iIsPhonebookInitialized = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iIsPhonebookInitializeFailed = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXNumberLengthMax = value;
            aItem.GetNextInt( value );
            iPbType[value] = TPhoneBookTypeSdn;
            }
        }
    else if ( _L("Fdn") == string )
        {
        iUICCLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        if ( iHandleSpecifiedRequests )
            {
            TInt value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iIsPhonebookInitialized = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iIsPhonebookInitializeFailed = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXNumberLengthMax = value;
            aItem.GetNextInt( value );
            iPbType[value] = TPhoneBookTypeFdn;
            }
        }
    else if ( _L("VoiceMailBox") == string )
        {
        iUICCLog->Log((TDesC8)KVoiceMailBox);
        iPhoneBookName = KETelIccVoiceMailBox;
        }
    else
        {
        _LIT8(KNotSupp, "UICC::Selected PB not supported");
        iUICCLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }
    if ( supported )
        {
        CPhoneBookDataPackage package;
        package.SetPhoneBookName( iPhoneBookName );
        ret = HandleRequestL( EMmTsyPhoneBookStoreInitIPC, &package );
        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "PbInit request send ok" );
            iUICCLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "PbInit request send failed: %d" );
             iUICCLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompletePbInit
// Complete PbInit method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompletePbInit
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompletePbInit result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );
    // Response information
    CStorageInfoData* pbInitResp;
    TInt i;

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData ( &pbInitResp );
        PrintPbInitInfo( pbInitResp );

        if ( iHandleSpecifiedRequests )
            {
            // select used phonebook type for data validation
            TPhoneBookType pbType( TPhoneBookTypeUnknown );
            for ( i = 0; i < 2; i++)
                {
                if ( iPbType[i] != TPhoneBookTypeUnknown )
                    {
                    pbType = iPbType[i];
                    break;
                    }
                }
            aResult = CheckPbData( pbInitResp, pbType );
            // reset phonebook type
            iPbType[i] = TPhoneBookTypeUnknown;
            }
        }
    else
        {
        _LIT8( KFail, "CompletePbInit Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    // in case of failure, send result immediately
    if( KErrNone != aResult )
        {
        Signal( aResult );
        }

    // in successful case, send result if test is not ongoing
    else if( ! iTestOngoing )
        {
        Signal( aResult );
        }

    iTestOngoing = EFalse;
    }

// -----------------------------------------------------------------------------
// CUICC::CheckPbData
// Check that phonebook data is as expected
// -----------------------------------------------------------------------------
//
TInt CUICC::CheckPbData( CStorageInfoData* aPbData, TPhoneBookType aPbType )
    {
    TInt error( KErrNone );

    if ( TPhoneBookTypeAdn == aPbType )
        {
/*        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataAdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataAdn.iIsPhonebookInitializeFailed ||
             aPbData->iADNNumOfEntries != iPbStorageDataAdn.iXXXNumOfEntries ||
             aPbData->iADNTextLengthMax != iPbStorageDataAdn.iXXXTextLengthMax ||
             aPbData->iADNNumberLengthMax != iPbStorageDataAdn.iXXXNumberLengthMax )
            {
            error = KErrGeneral;
            }*/
        if ( aPbData->iIsPhonebookInitialized != iRefPBStoreInfoData->iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iRefPBStoreInfoData->iIsPhonebookInitializeFailed ||
             aPbData->iADNNumOfEntries != iRefPBStoreInfoData->iADNNumOfEntries ||
             aPbData->iADNTextLengthMax != iRefPBStoreInfoData->iADNTextLengthMax ||
             aPbData->iADNNumberLengthMax != iRefPBStoreInfoData->iADNNumberLengthMax ||
             aPbData->iSNENumOfEntries != iRefPBStoreInfoData->iSNENumOfEntries ||
             aPbData->iSNENumOfEntriesPerEntry != iRefPBStoreInfoData->iSNENumOfEntriesPerEntry ||
             aPbData->iSNETextLengthMax != iRefPBStoreInfoData->iSNETextLengthMax ||
             aPbData->iEmailNumOfEntries != iRefPBStoreInfoData->iEmailNumOfEntries ||
             aPbData->iEmailNumOfEntriesPerEntry != iRefPBStoreInfoData->iEmailNumOfEntriesPerEntry ||
             aPbData->iEmailTextLengthMax != iRefPBStoreInfoData->iEmailTextLengthMax ||
             aPbData->iGRPNumOfEntriesPerEntry != iRefPBStoreInfoData->iGRPNumOfEntriesPerEntry ||
             aPbData->iANRNumOfEntries != iRefPBStoreInfoData->iANRNumOfEntries ||
             aPbData->iANRNumOfEntriesPerEntry != iRefPBStoreInfoData->iANRNumOfEntriesPerEntry ||
             aPbData->iANRNumLengthMax != iRefPBStoreInfoData->iANRNumLengthMax )
            {
            error = KErrGeneral;
            }
        }
    else if ( TPhoneBookTypeFdn == aPbType )
        {
        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataFdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataFdn.iIsPhonebookInitializeFailed ||
             aPbData->iFDNNumOfEntries != iPbStorageDataFdn.iXXXNumOfEntries ||
             aPbData->iFDNTextLengthMax != iPbStorageDataFdn.iXXXTextLengthMax ||
             aPbData->iFDNNumberLengthMax != iPbStorageDataFdn.iXXXNumberLengthMax )
            {
            error = KErrGeneral;
            }
        }
    else if ( TPhoneBookTypeSdn == aPbType )
        {
        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataSdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataSdn.iIsPhonebookInitializeFailed ||
             aPbData->iFDNNumOfEntries != iPbStorageDataSdn.iXXXNumOfEntries ||
             aPbData->iFDNTextLengthMax != iPbStorageDataSdn.iXXXTextLengthMax ||
             aPbData->iFDNNumberLengthMax != iPbStorageDataSdn.iXXXNumberLengthMax )
            {
            error = KErrGeneral;
            }
        }
    else if ( TPhoneBookTypeMbdn == aPbType )
        {
        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataMbdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataMbdn.iIsPhonebookInitializeFailed ||
             aPbData->iFDNNumOfEntries != iPbStorageDataMbdn.iXXXNumOfEntries ||
             aPbData->iFDNTextLengthMax != iPbStorageDataMbdn.iXXXTextLengthMax ||
             aPbData->iFDNNumberLengthMax != iPbStorageDataMbdn.iXXXNumberLengthMax )
            {
            error = KErrGeneral;
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CUICC::PrintPbInitInfo
// Prints Pb info
// -----------------------------------------------------------------------------
//
void CUICC::PrintPbInitInfo
        (
        CStorageInfoData* aPbInitInfo
        )
    {
    iUICCLog->Log((TDesC8)KLine );
    _LIT8( KText, "Pb Init Info" );
    iUICCLog->Log((TDesC8)KText );

    _LIT8( KADNNumOfEntries, "ADNNumOfEntries: %d" );
    iUICCLog->Log((TDesC8)KADNNumOfEntries, aPbInitInfo->iADNNumOfEntries );

    _LIT8( KADNTextLengthMax, "ADNTextLengthMax: %d" );
    iUICCLog->Log((TDesC8)KADNTextLengthMax, aPbInitInfo->iADNTextLengthMax );

    _LIT8( KADNNumberLengthMax, "ADNNumberLengthMax: %d" );
    iUICCLog->Log((TDesC8)KADNNumberLengthMax, aPbInitInfo->iADNNumberLengthMax );

    _LIT8( KSNENumOfEntries, "SNENumOfEntries: %d" );
    iUICCLog->Log((TDesC8)KSNENumOfEntries, aPbInitInfo->iSNENumOfEntries );

    _LIT8( KSNENumOfEntriesPerEntry, "SNENumOfEntriesPerEntry: %d" );
    iUICCLog->Log((TDesC8)KSNENumOfEntriesPerEntry, aPbInitInfo->iSNENumOfEntriesPerEntry );

    _LIT8( KSNENumLengthMax, "SNETextLengthMax: %d" );
    iUICCLog->Log((TDesC8)KSNENumLengthMax, aPbInitInfo->iSNETextLengthMax );

    _LIT8( KEmailNumOfEntries, "EmailNumOfEntries: %d" );
    iUICCLog->Log((TDesC8)KEmailNumOfEntries, aPbInitInfo->iEmailNumOfEntries );

    _LIT8( KEMailNumOfEntriesPerEntry, "EmailNumOfEntriesPerEntry: %d" );
    iUICCLog->Log((TDesC8)KEMailNumOfEntriesPerEntry, aPbInitInfo->iEmailNumOfEntriesPerEntry );

    _LIT8( KEmailNumLengthMax, "EmailTextLengthMax: %d" );
    iUICCLog->Log((TDesC8)KEmailNumLengthMax, aPbInitInfo->iEmailTextLengthMax );

    _LIT8( KGRPNumOfEntriesPerEntry, "GRPNumOfEntriesPerEntry: %d" );
    iUICCLog->Log((TDesC8)KGRPNumOfEntriesPerEntry, aPbInitInfo->iGRPNumOfEntriesPerEntry );

    _LIT8( KANRNumOfEntries, "ANRNumOfEntries: %d" );
    iUICCLog->Log((TDesC8)KANRNumOfEntries, aPbInitInfo->iANRNumOfEntries );

    _LIT8( KANRNumOfEntriesPerEntry, "ANRNumOfEntriesPerEntry: %d" );
    iUICCLog->Log((TDesC8)KANRNumOfEntriesPerEntry, aPbInitInfo->iANRNumOfEntriesPerEntry );

    _LIT8( KANRNumLengthMax, "ANRNumLengthMax: %d" );
    iUICCLog->Log((TDesC8)KANRNumLengthMax, aPbInitInfo->iANRNumLengthMax );

    _LIT8( KFDNNumOfEntries, "FDNNumOfEntries: %d" );
    iUICCLog->Log((TDesC8)KFDNNumOfEntries, aPbInitInfo->iFDNNumOfEntries );

    _LIT8( KFDNTextLengthMax, "FDNTextLengthMax: %d" );
    iUICCLog->Log((TDesC8)KFDNTextLengthMax, aPbInitInfo->iFDNTextLengthMax );

    _LIT8( KFDNNumberLengthMax, "FDNNumberLengthMax: %d" );
    iUICCLog->Log((TDesC8)KFDNNumberLengthMax, aPbInitInfo->iFDNNumberLengthMax );

    _LIT8( KSDNNumOfEntries, "SDNNumOfEntries: %d" );
    iUICCLog->Log((TDesC8)KSDNNumOfEntries, aPbInitInfo->iSDNNumOfEntries );

    _LIT8( KSDNTextLengthMax, "SDNTextLengthMax: %d" );
    iUICCLog->Log((TDesC8)KSDNTextLengthMax, aPbInitInfo->iSDNTextLengthMax );

    _LIT8( KSDNNumberLengthMax, "SDNNumberLengthMax: %d" );
    iUICCLog->Log((TDesC8)KSDNNumberLengthMax, aPbInitInfo->iSDNNumberLengthMax );

    _LIT8( KVMBXNumOfEntries, "VMBXNumOfEntries: %d" );
    iUICCLog->Log((TDesC8)KVMBXNumOfEntries, aPbInitInfo->iVMBXNumOfEntries );

    _LIT8( KVMBXTextLengthMax, "VMBXTextLengthMax: %d" );
    iUICCLog->Log((TDesC8)KVMBXTextLengthMax, aPbInitInfo->iVMBXTextLengthMax );

    _LIT8( KVMBXNumberLengthMax, "VMBXNumberLengthMax: %d" );
    iUICCLog->Log((TDesC8)KVMBXNumberLengthMax, aPbInitInfo->iVMBXNumberLengthMax );

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    _LIT8( KANRTextLengthMax, "ANRTextLengthMax (AAS): %d" );
    iUICCLog->Log((TDesC8)KANRTextLengthMax, aPbInitInfo->iANRTextLengthMax );

    _LIT8( KNumOfAasMax, "NumOfAasMax: %d" );
    iUICCLog->Log((TDesC8)KNumOfAasMax, aPbInitInfo->iNumOfAasMax );

    _LIT8( KGRPTextLengthMax, "GRPTextLengthMax: %d" );
    iUICCLog->Log((TDesC8)KGRPTextLengthMax, aPbInitInfo->iGRPTextLengthMax );

    _LIT8( KNumOfGasMax, "NumOfGasMax: %d" );
    iUICCLog->Log((TDesC8)KNumOfGasMax, aPbInitInfo->iNumOfGasMax );
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    }

// -----------------------------------------------------------------------------
// CUICC::GetServiceProviderName
// GetServiceProviderName
// -----------------------------------------------------------------------------
//
TInt CUICC::GetServiceProviderName( CStifItemParser& aItem )
    {
    _LIT8(KSPN, "UICC::GetServiceProviderName");
    iUICCLog->Log((TDesC8)KSPN);

    TInt ret( KErrNotFound );

    if( iHandleSpecifiedRequests )
        {
        TInt temp;
        aItem.GetNextInt( temp );
        iServiceProviderName.iDisplayReq = temp;
        TPtrC string;
        aItem.GetNextString( string );
        iServiceProviderName.iSPName = string;
        aItem.GetNextInt( temp );
        iServiceProviderName.iPLMNField.Append(temp);
        aItem.GetNextInt( temp );
        iServiceProviderName.iPLMNField.Append(temp);
        aItem.GetNextInt( temp );
        iServiceProviderName.iPLMNField.Append(temp);
        aItem.GetNextInt( temp );
        iServiceProviderName.iPLMNField.Append(temp);
        aItem.GetNextInt( temp );
        iServiceProviderName.iPLMNField.Append(temp);
        aItem.GetNextInt( temp );
        iServiceProviderName.iPLMNField.Append(temp);
        }

    ret = HandleRequestL( EMobilePhoneGetServiceProviderName );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetServiceProviderName request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetServiceProviderName request send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetServiceProviderName
// Complete GetServiceProviderName method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetServiceProviderName(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CompleteGetServiceProviderName result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneServiceProviderNameV2 serviceProviderName;
        aDataPackage->UnPackData( serviceProviderName );

        TBuf8<255> drBuffer;
        _LIT8(KRowFormatter1, "Display req: %d");
        drBuffer.Format(KRowFormatter1, &serviceProviderName.iDisplayReq);
        iUICCLog->Log(drBuffer);

        TBuf8<255> nameBuffer;
        _LIT8(KRowFormatter2, "Service provider name: %S");
        nameBuffer.Format(KRowFormatter2, &serviceProviderName.iSPName);
        iUICCLog->Log(nameBuffer);

        TBuf8<255> plmnBuffer;
        _LIT8(KRowFormatter3, "PLMN field: %S");
        plmnBuffer.Format(KRowFormatter3, &serviceProviderName.iPLMNField);
        iUICCLog->Log(plmnBuffer);

        if( iHandleSpecifiedRequests )
            {
            if( iServiceProviderName.iDisplayReq !=
                serviceProviderName.iDisplayReq )
                {
                aResult = KErrGeneral;
                }
            if( iServiceProviderName.iSPName !=
                serviceProviderName.iSPName )
                {
                aResult = KErrGeneral;
                }
            if( iServiceProviderName.iPLMNField !=
                serviceProviderName.iPLMNField )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetServiceProviderName Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::GetCustomerServiceProfile
// Get Customer Service Profile
// -----------------------------------------------------------------------------
//
TInt CUICC::GetCustomerServiceProfile( CStifItemParser& aItem )
    {
    _LIT8(KGetCSP, "UICC::GetCustomerServiceProfile");
    iUICCLog->Log((TDesC8)KGetCSP);

    TInt ret( KErrNotFound );

    // Store given parameters for validity checking
    if( iHandleSpecifiedRequests )
        {
        TInt temp;
        aItem.GetNextInt( temp );
        iCspFile.iCallOfferingServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCallRestrictionServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iOtherSuppServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCallCompletionServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iTeleservices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCphsTeleservices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCphsFeatures = temp;
        aItem.GetNextInt( temp );
        iCspFile.iNumberIdentServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iPhase2PlusServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iValueAddedServices = temp;
        }

    ret = HandleRequestL( EMobilePhoneGetCustomerServiceProfile );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetCustomerServiceProfile request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetCustomerServiceProfile request send failed: %d" );
         iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetCustomerServiceProfile
// Complete Get Customer Service Profile
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetCustomerServiceProfile
    (
    TInt aResult,
    CMmDataPackage* aDataPackage
    )
    {
    _LIT8( KResult, "CompleteGetCustomerServiceProfile result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneCspFileV1 cspFile;

         if( aDataPackage )
            {
            aDataPackage->UnPackData( cspFile );

            _LIT8( KCOS, "CallOfferingServices: %d" );
            iUICCLog->Log((TDesC8)KCOS, cspFile.iCallOfferingServices );
            _LIT8( KCRS, "CallRestrictionServices: %d" );
            iUICCLog->Log((TDesC8)KCRS, cspFile.iCallRestrictionServices );
            _LIT8( KOSS, "OtherSuppServices: %d" );
            iUICCLog->Log((TDesC8)KOSS, cspFile.iOtherSuppServices );
            _LIT8( KCCS, "CallCompletionServices: %d" );
            iUICCLog->Log((TDesC8)KCCS, cspFile.iCallCompletionServices );
            _LIT8( KTS, "Teleservices: %d" );
            iUICCLog->Log((TDesC8)KTS, cspFile.iTeleservices );
            _LIT8( KCTS, "CphsTeleservices: %d" );
            iUICCLog->Log((TDesC8)KCTS, cspFile.iCphsTeleservices );
            _LIT8( KCF, "CphsFeatures: %d" );
            iUICCLog->Log((TDesC8)KCF, cspFile.iCphsFeatures );
            _LIT8( KNIS, "NumberIdentServices: %d" );
            iUICCLog->Log((TDesC8)KNIS, cspFile.iNumberIdentServices );
            _LIT8( KPPS, "Phase2PlusServices: %d" );
            iUICCLog->Log((TDesC8)KPPS, cspFile.iPhase2PlusServices );
            _LIT8( KVAS, "ValueAddedServices: %d" );
            iUICCLog->Log((TDesC8)KVAS, cspFile.iValueAddedServices );

            // Validity checking
            if( iHandleSpecifiedRequests )
                {
                if( iCspFile.iCallOfferingServices != cspFile.iCallOfferingServices ||
                    iCspFile.iCallRestrictionServices != cspFile.iCallRestrictionServices ||
                    iCspFile.iOtherSuppServices != cspFile.iOtherSuppServices ||
                    iCspFile.iCallCompletionServices != cspFile.iCallCompletionServices ||
                    iCspFile.iTeleservices != cspFile.iTeleservices ||
                    iCspFile.iCphsTeleservices != cspFile.iCphsTeleservices ||
                    iCspFile.iCphsFeatures != cspFile.iCphsFeatures ||
                    iCspFile.iNumberIdentServices != cspFile.iNumberIdentServices ||
                    iCspFile.iPhase2PlusServices != cspFile.iPhase2PlusServices ||
                    iCspFile.iValueAddedServices != cspFile.iValueAddedServices )
                    {
                    aResult = KErrGeneral;
                    }
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetCustomerServiceProfile Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::ReadSimFile
// ReadSimFile
// -----------------------------------------------------------------------------
//
TInt CUICC::ReadSimFile( CStifItemParser& aItem )
    {
    _LIT8(KReadSimFile, "UICC::ReadSimFile");
    iUICCLog->Log((TDesC8)KReadSimFile);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;

    RMmCustomAPI::TSimFileInfo simFileInfo;

    // File ID
    TBuf8<8> path; // File path in SIM where file is read
    TInt offSet; // Offset from beginning of the file
    TInt size; // Amount of data to read

    TInt masterFile;  // Master File
    TInt dedicatedFile1;  // Dedicated File

    aItem.GetNextInt( masterFile );
    aItem.GetNextInt( dedicatedFile1 );
    aItem.GetNextInt( offSet );
    aItem.GetNextInt( size );

    path.Append(masterFile>>8);
    path.Append(masterFile);
    path.Append(dedicatedFile1>>8);
    path.Append(dedicatedFile1);

    simFileInfo.iPath = path;
    simFileInfo.iOffSet = (TUint16)offSet;
    simFileInfo.iSize = (TUint16)size;

    //Create package
    CMmDataPackage dataPackage;
    dataPackage.PackData( &simFileInfo );

    ret = HandleRequestL( ECustomReadSimFileIPC, &dataPackage );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ReadSimFile request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "ReadSimFile request send failed: %d" );
         iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CUICC::CompleteReadSimFile
// Complete ReadSimFile method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteReadSimFile( TInt aResult, CMmDataPackage* /*aDataPackage*/ )
    {
    _LIT8( KResult, "CompleteReadSimFile result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        _LIT8( KPass, "CompleteReadSimFile Passed: %d" );
        iUICCLog->Log((TDesC8)KPass, aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteReadSimFile Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::ReadMailboxIdentifier
// Read EF MBI 6FC9
// -----------------------------------------------------------------------------
//
TInt CUICC::GetMailboxIdentifier( CStifItemParser& aItem )
    {
    _LIT8(KGetMailboxIdentifier, "UICC::GetMailboxIdentifier");
    iUICCLog->Log((TDesC8)KGetMailboxIdentifier);

    TInt ret( KErrNotFound );

    TInt temp;
    aItem.GetNextInt( temp );
    iVoicemailIds.iVoice = temp;
    aItem.GetNextInt( temp );
    iVoicemailIds.iFax = temp;
    aItem.GetNextInt( temp );
    iVoicemailIds.iData = temp;
    aItem.GetNextInt( temp );
    iVoicemailIds.iOther = temp;

    ret = HandleRequestL( EMobilePhoneGetMailboxNumbers );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetMailboxIdentifier request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetMailboxIdentifier request send failed: %d" );
         iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetMailboxIdentifier
// Complete GetMailboxIdentifier
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetMailboxIdentifier(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CompleteGetMailboxIdentifier result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    RMobilePhone::TMobilePhoneVoicemailIdsV3* voicemailIds;
    aDataPackage->UnPackData( voicemailIds );

    if( KErrNone == aResult )
        {
        if ( voicemailIds->iVoice != iVoicemailIds.iVoice ||
            voicemailIds->iFax != iVoicemailIds.iFax ||
            voicemailIds->iData != iVoicemailIds.iData ||
            voicemailIds->iOther != iVoicemailIds.iOther )
            {
            aResult = KErrGeneral;
            _LIT8( KFail, "CompleteGetMailboxIdentifier Failed: %d" );
            iUICCLog->Log((TDesC8)KFail, aResult );
            }
        else
            {
            _LIT8( KPass, "CompleteGetMailboxIdentifier Passed: %d" );
            iUICCLog->Log((TDesC8)KPass, aResult );
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetMailboxIdentifier Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::ReadCBSSimTopics
// -----------------------------------------------------------------------------
//
TInt CUICC::ReadCBSSimTopics( CStifItemParser& aItem )
    {
    iUICCLog->Log(_L8("CUICC::ReadCBSSimTopics"));
    TInt ret( KErrNone );

    TInt temp;
    aItem.GetNextInt( temp );
    iSimCbTopic1.iNumber = temp;
    aItem.GetNextInt( temp );
    iSimCbTopic2.iNumber = temp;
    aItem.GetNextInt( temp );
    iSimCbTopic3.iNumber = temp;

    ret = HandleRequestL( ECustomStartSimCbTopicBrowsingIPC );

    if ( KErrNone != ret )
        {
        iUICCLog->Log(_L8("Error in reading CBS SIM topics: %d"), ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::CompleteReadCBSSimTopics
// -----------------------------------------------------------------------------
//
void CUICC::CompleteReadCBSSimTopics( TInt aErr, CMmDataPackage* aData )
    {
    iUICCLog->Log(_L8("CUICC::CompleteReadCBSSimTopics, err=%d"),aErr);

    if ( KErrNone == aErr )
        {
        CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* topics;
        aData->UnPackData( topics );
        TInt count( topics->Count() );
        if ( 0 < count )
            {
            iUICCLog->Log(_L8("%d topics read."), count);
            for ( TInt i( 0 ); i < count; i++ )
                {
                iUICCLog->Log(_L8("  topic[%d]=%d."),i,topics->At( i ).iNumber);
                }
            }
        if ( iSimCbTopic1.iNumber != topics->At(0).iNumber ||
             iSimCbTopic2.iNumber != topics->At(1).iNumber ||
             iSimCbTopic3.iNumber != topics->At(2).iNumber )
            {
            aErr = KErrGeneral;
            }
        }
        Signal(aErr);
    }


// -----------------------------------------------------------------------------
// CUICC::DeleteCBSSimTopic
// -----------------------------------------------------------------------------
//
TInt CUICC::DeleteCBSSimTopic( CStifItemParser& aItem )
    {
    iUICCLog->Log(_L8("CUICC::ReadCBSSimTopics"));
    TInt ret( KErrNone );

    TInt temp;
    aItem.GetNextInt( temp );
    TUint simCbTopicIndexToBeDeleted( temp );
    TBool topicInSimMemoryDelete ( ETrue );
    CMmDataPackage dataPackage;
    dataPackage.PackData( &simCbTopicIndexToBeDeleted, &topicInSimMemoryDelete );

    ret = HandleRequestL( ECustomDeleteSimCbTopicIPC, &dataPackage );

    if ( KErrNone != ret )
        {
        iUICCLog->Log(_L8("Error in deleting CBS SIM topics: %d"), ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteDeleteCBSSimTopic
// -----------------------------------------------------------------------------
//
void CUICC::CompleteDeleteCBSSimTopic( TInt aErr )
    {
    iUICCLog->Log(_L8("CUICC::CompleteDeleteCBSSimTopic, err=%d"),aErr);
    Signal(aErr);
    }

// -----------------------------------------------------------------------------
// CUICC::GetNetworkProviderName
// GetNetworkProviderName
// -----------------------------------------------------------------------------
//
TInt CUICC::GetNetworkProviderName( CStifItemParser& aItem )
    {
    _LIT8(KWriteONStore, "UICC::GetNetworkProviderName");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );

    // for validity checking
    if( iHandleSpecifiedRequests )
        {
        TPtrC temp;
        aItem.GetNextString( temp );
        if ( temp == _L("Orange") )
            {
            iName.Append( _L(" Orange") );
            }
        }

    ret = HandleRequestL( ECustomGetNetworkProviderNameIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetNetworkProviderName request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "GetNetworkProviderName request send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetNetworkProviderName
// Complete GetNetworkProviderName method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetNetworkProviderName(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CompleteGetNetworkProviderName result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        //network provider name
        TDesC* name;

        aDataPackage->UnPackData( name );

        _LIT8( KName, "Name:" );
        iUICCLog->Log((TDesC8)KName);
        iUICCLog->Log( *name );

        // validity checking
        if( iHandleSpecifiedRequests )
            {
            TBuf<20> tempName;
            tempName.Append( *name );
            if ( iName != tempName )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetNetworkProviderName Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::ViagHomeZoneReadParam
// -----------------------------------------------------------------------------
//
TInt CUICC::ViagHomeZoneReadParamL( CStifItemParser& aItem )
    {
    _LIT8( KViagHomeZoneReadParam, "CUICC::ViagHomeZoneReadParam" );
    iUICCLog->Log((TDesC8)KViagHomeZoneReadParam);

    // SMSC
    TPtrC ptrSmsC;
    User::LeaveIfError( aItem.GetNextString( ptrSmsC ) );
    iSavedViagParamsForComparison.iSmsC = ptrSmsC;

    // SCP
    TPtrC ptrScp;
    User::LeaveIfError(
        aItem.GetNextString( ptrScp ) );
    iSavedViagParamsForComparison.iScp = ptrScp;

    // Subscribed zone and version
    TUint zoneAndVersion( 0 );
    User::LeaveIfError( aItem.GetNextInt( zoneAndVersion, EHex ) );
    iSavedViagParamsForComparison.iSubscribedZoneAndVersion = zoneAndVersion;

    // Creating elements
    iSavedViagElementsForComparison =
        new(ELeave) RMmCustomAPI::TViagElements(
            RMmCustomAPI::KViagElementCount );

    TPtrC homezoneFlag;
    for ( TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++ )
        {
        RMmCustomAPI::TViagElement element;

        // Fetching X coordinate
        TUint xCoord( 0 );
        User::LeaveIfError( aItem.GetNextInt( xCoord, EHex ) );
        element.iCoordinates.iX = xCoord;
        // Fetching Y coordinate
        TUint yCoord( 0 );
        User::LeaveIfError( aItem.GetNextInt( yCoord, EHex ) );
        element.iCoordinates.iY = yCoord;
        // Fetching R2
        TUint r2;
        User::LeaveIfError( aItem.GetNextInt( r2, EHex ) );
        element.iCoordinates.iR2 = r2;
        // Fetching zone id
        TUint zoneId( 0 );
        User::LeaveIfError( aItem.GetNextInt( zoneId, EHex ) );
        element.iCoordinates.iZoneId = zoneId;
        // Fetching homezone status flag
        User::LeaveIfError( aItem.GetNextString( homezoneFlag ) );
        if ( 0 == homezoneFlag.Compare( KViagInactive ) )
            {
            element.iActiveFlag = RMmCustomAPI::ENotActive;
            }
        else if ( 0 == homezoneFlag.Compare( KViagHZActive ) )
            {
            element.iActiveFlag = RMmCustomAPI::EHomeZone;
            }
        else if ( 0 == homezoneFlag.Compare( KViagCZActive ) )
            {
            element.iActiveFlag = RMmCustomAPI::ECityZone;
            }
        else
            {
            User::Leave( KErrArgument );
            }

        // Fetching tag
        TPtrC name;
        User::LeaveIfError( aItem.GetNextString( name ) );
        element.iName = name;
        iSavedViagElementsForComparison->AppendL( element );
        }

    TInt ret( HandleRequestL( EReadViagHomeZoneParamsIPC ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EReadViagHomeZoneParamsIPC request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "EReadViagHomeZoneParamsIPC request send failed: %d" );
         iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteViagHomeZoneReadParam
// -----------------------------------------------------------------------------
//
void CUICC::CompleteViagHomeZoneReadParam(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CUICC::CompleteViagHomeZoneReadParam - result: %d" );
    iUICCLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        RMmCustomAPI::TViagParams viagParams;
        RMmCustomAPI::TViagElements* viagElements( NULL );

        aDataPackage->UnPackData( viagParams, viagElements );

        // match parameters
        if ( viagParams.iScp != iSavedViagParamsForComparison.iScp
            || viagParams.iSmsC != iSavedViagParamsForComparison.iSmsC
            || viagParams.iSubscribedZoneAndVersion !=
                iSavedViagParamsForComparison.iSubscribedZoneAndVersion )
            {
            aResult = KErrGeneral;
            }

        // match elements
        for ( TInt i = 0;
            i < RMmCustomAPI::KViagElementCount && KErrNone == aResult;
            i++)
            {
            RMmCustomAPI::TViagElement& element =
                viagElements->At( i );
            RMmCustomAPI::TViagElement& expected =
                iSavedViagElementsForComparison->At( i );

            if ( element.iCoordinates.iX != expected.iCoordinates.iX
                || element.iCoordinates.iY != expected.iCoordinates.iY
                || element.iCoordinates.iR2 != expected.iCoordinates.iR2
                || element.iCoordinates.iZoneId != expected.iCoordinates.iZoneId
                || element.iName != expected.iName
                || element.iActiveFlag != expected.iActiveFlag )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CUICC::CompleteViagHomeZoneReadParam failed: %d" );
        iUICCLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }


// -----------------------------------------------------------------------------
// CUICC::ViagHomeZoneReadCache
// Read ViagHomeZone cache
// {cacheid, recnum, lac, cellid}
// -----------------------------------------------------------------------------
//
TInt CUICC::ViagHomeZoneReadCacheL( CStifItemParser& aItem )
    {
    _LIT8(KViagHomeZoneReadCache, "CUICC::ViagHomeZoneReadCache");
    iUICCLog->Log((TDesC8)KViagHomeZoneReadCache);

    RMmCustomAPI::TViagCacheRecordId record;

    // fetching cacheid for request
    TUint cacheid;
    User::LeaveIfError( aItem.GetNextInt( cacheid, EHex ) );
    record.iCacheId = cacheid;
    // fetching recnum for request
    TUint recnum;
    User::LeaveIfError(
        aItem.GetNextInt( recnum, EHex ) );
    // XML data and CFG data should match
    record.iRecordId = recnum - 1;

    // fetching LAC for response
    TUint lac( 0 );
    User::LeaveIfError( aItem.GetNextInt( lac, EHex ) );
    iSavedRecordForComparison.iLac = lac;
    // fetching CellId for response
    TUint cellId( 0 );
    User::LeaveIfError( aItem.GetNextInt( cellId, EHex ) );
    iSavedRecordForComparison.iCellId = cellId;

    CMmDataPackage package;
    package.PackData( &record );
    TInt ret( HandleRequestL( EReadViagHomeZoneCacheIPC, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EReadViagHomeZoneCacheIPC request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "EReadViagHomeZoneCacheIPC request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::ViagHomeZoneWriteSettings
// Write ViagHomeZone settings
// {settings, version}
// -----------------------------------------------------------------------------
//
TInt CUICC::ViagHomeZoneWriteSettingsL( CStifItemParser& aItem )
    {
    _LIT8( KViagHomeZoneWriteSettings, "CUICC::ViagHomeZoneWriteSettings" );
    iUICCLog->Log((TDesC8)KViagHomeZoneWriteSettings);

    CMmDataPackage package;
    RMmCustomAPI::TViagUHZIUESettings settings;

    // fetching settings
    TUint set( 0 );
    User::LeaveIfError( aItem.GetNextInt( set, EHex ) );
    settings.iSettings = set;

    // fetching version
    TUint ver( 0 );
    User::LeaveIfError( aItem.GetNextInt( ver, EHex ) );
    settings.iVersion = ver;

    package.PackData( &settings );
    TInt ret( HandleRequestL( EWriteViagHomeZoneUHZIUESettingsIPC, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EWriteViagHomeZoneUHZIUESettingsIPC request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "EWriteViagHomeZoneUHZIUESettingsIPC request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::ViagHomeZoneWriteCache
// Write ViagHomeZone cache
// {cacheid, recnum, lac, cellid}
// -----------------------------------------------------------------------------
//
TInt CUICC::ViagHomeZoneWriteCacheL( CStifItemParser& aItem )
    {
    _LIT8( KViagHomeZoneWriteCache, "CUICC::ViagHomeZoneWriteCache" );
    iUICCLog->Log( (TDesC8)KViagHomeZoneWriteCache );

    CMmDataPackage package;
    RMmCustomAPI::TViagCacheRecordId recordId;
    RMmCustomAPI::TViagCacheRecordContent recordContent;

    // fetching cacheId
    TUint cacheid( 0 );
    User::LeaveIfError( aItem.GetNextInt( cacheid, EHex ) );
    recordId.iCacheId = cacheid;

    // fetching record number
    TUint recnum( 0 );
    User::LeaveIfError( aItem.GetNextInt( recnum, EHex ) );
    // XML and CFG should match
    recordId.iRecordId = recnum - 1;

    // fetching lac
    TUint lac( 0 );
    User::LeaveIfError( aItem.GetNextInt( lac, EHex ) );
    recordContent.iLac = lac;

    // fetching CellId
    TUint cellid( 0 );
    User::LeaveIfError( aItem.GetNextInt( cellid, EHex ) );
    recordContent.iCellId = cellid;

    package.PackData( &recordId, &recordContent );
    TInt ret( HandleRequestL( EWriteViagHomeZoneCacheIPC, &package) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EWriteViagHomeZoneCacheIPC request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
        _LIT8( KRequestFailed, "EWriteViagHomeZoneCacheIPC request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteViagHomeZoneReadCache
// Performs verification of the data returned when
// EReadViagHomeZoneCacheIPC is completed
// -----------------------------------------------------------------------------
//
void CUICC::CompleteViagHomeZoneReadCache(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CUICC::CompleteViagHomeZoneReadCache - result: %d" );
    iUICCLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        RMmCustomAPI::TViagCacheRecordContent record;

        aDataPackage->UnPackData( record );

        if ( iSavedRecordForComparison.iLac != record.iLac
            || iSavedRecordForComparison.iCellId != record.iCellId )
            {
            aResult = KErrGeneral;
            }
        }
    else
        {
        _LIT8( KFail, "CUICC::CompleteViagHomeZoneReadCache failed: %d" );
        iUICCLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CUICC::ReadVoiceMsgFlagsRel4
// -----------------------------------------------------------------------------
//
TInt CUICC::ReadVoiceMsgFlagsRel4( CStifItemParser& aItem )
    {
    _LIT8( KReadVoiceMsgFlagsRel4, "CUICC::ReadVoiceMsgFlagsRel4" );
    iUICCLog->Log( (TDesC8)KReadVoiceMsgFlagsRel4 );

    // Fetch parameters for comparison
    TUint temp( 0 );
    aItem.GetNextInt( temp );
    iMsgWaiting.iDisplayStatus = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iVoiceMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iFaxMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iEmailMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iOtherMsgs = temp;

    TInt ret( HandleRequestL( EMobilePhoneGetIccMessageWaitingIndicators ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "ReadVoiceMsgFlagsRel4 request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "ReadVoiceMsgFlagsRel4 request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteReadVoiceMsgFlagsRel4
// -----------------------------------------------------------------------------
//
void CUICC::CompleteReadVoiceMsgFlags(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CUICC::CompleteReadVoiceMsgFlagsRel4 - result: %d" );
    iUICCLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult || KErrCPHSInUseBySIM == aResult )
        {
        RMobilePhone::TMobilePhoneMessageWaitingV1 VMIndicators;
        aDataPackage->UnPackData( VMIndicators );

        aResult = KErrNone;

        if ( VMIndicators.iDisplayStatus != iMsgWaiting.iDisplayStatus ||
            VMIndicators.iVoiceMsgs != iMsgWaiting.iVoiceMsgs ||
            VMIndicators.iFaxMsgs != iMsgWaiting.iFaxMsgs ||
            VMIndicators.iEmailMsgs != iMsgWaiting.iEmailMsgs ||
            VMIndicators.iOtherMsgs != iMsgWaiting.iOtherMsgs )
            {
            aResult = KErrGeneral;
            }
        }
    else
        {
        _LIT8( KFail, "CUICC::CompleteReadVoiceMsgFlagsRel4 failed: %d" );
        iUICCLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CUICC::WriteVoiceMsgFlagsRel4
// -----------------------------------------------------------------------------
//
TInt CUICC::WriteVoiceMsgFlagsRel4( CStifItemParser& aItem )
    {
    _LIT8( KWriteVoiceMsgFlagsRel4, "CUICC::WriteVoiceMsgFlagsRel4" );
    iUICCLog->Log( (TDesC8)KWriteVoiceMsgFlagsRel4 );

    CMmDataPackage package;

    // Fetch parameters for data package and comparision
    TUint temp( 0 );
    aItem.GetNextInt( temp );
    iMsgWaiting.iDisplayStatus = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iVoiceMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iFaxMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iEmailMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iOtherMsgs = temp;

    package.PackData( &iMsgWaiting );

    TInt ret( HandleRequestL( EMobilePhoneSetIccMessageWaitingIndicators, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "WriteVoiceMsgFlagsRel4 request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "WriteVoiceMsgFlagsRel4 request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::ReadVoiceMsgFlagsCphs
// -----------------------------------------------------------------------------
//
TInt CUICC::ReadVoiceMsgFlagsCphs( CStifItemParser& aItem )
    {
    _LIT8( KReadVoiceMsgFlagsCphs, "CUICC::ReadVoiceMsgFlagsCphs" );
    iUICCLog->Log( (TDesC8)KReadVoiceMsgFlagsCphs );

    // Fetch parameters for comparison
    TUint temp( 0 );
    aItem.GetNextInt( temp );
    iMsgWaiting.iDisplayStatus = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iVoiceMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iFaxMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iEmailMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iOtherMsgs = temp;

    TInt ret( HandleRequestL( EMobilePhoneGetIccMessageWaitingIndicators ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "ReadVoiceMsgFlagsCphs request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "ReadVoiceMsgFlagsCphs request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::WriteVoiceMsgFlagsCphs
// -----------------------------------------------------------------------------
//
TInt CUICC::WriteVoiceMsgFlagsCphs( CStifItemParser& aItem )
    {
    _LIT8( KWriteVoiceMsgFlagsCphs, "CUICC::WriteVoiceMsgFlagsCphs" );
    iUICCLog->Log( (TDesC8)KWriteVoiceMsgFlagsCphs );

    CMmDataPackage package;

    // Fetch parameters for data package and comparision
    TUint temp( 0 );
    aItem.GetNextInt( temp );
    iMsgWaiting.iDisplayStatus = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iVoiceMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iFaxMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iEmailMsgs = temp;
    aItem.GetNextInt( temp );
    iMsgWaiting.iOtherMsgs = temp;

    package.PackData( &iMsgWaiting );

    TInt ret( HandleRequestL( EMobilePhoneSetIccMessageWaitingIndicators, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "WriteVoiceMsgFlagsCphs request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "WriteVoiceMsgFlagsCphs request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteNotifyMwiChange
// -----------------------------------------------------------------------------
//
void CUICC::CompleteNotifyMwiChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CUICC::CompleteNotifyMwiChange - result: %d" );
    iUICCLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneMessageWaitingV1 VMIndicators;
        aDataPackage->UnPackData( VMIndicators );

        if ( VMIndicators.iDisplayStatus != iMsgWaiting.iDisplayStatus ||
            VMIndicators.iVoiceMsgs != iMsgWaiting.iVoiceMsgs ||
            VMIndicators.iFaxMsgs != iMsgWaiting.iFaxMsgs ||
            VMIndicators.iEmailMsgs != iMsgWaiting.iEmailMsgs ||
            VMIndicators.iOtherMsgs != iMsgWaiting.iOtherMsgs )
            {
            aResult = KErrGeneral;
            }
        }
    else
        {
        _LIT8( KFail, "CUICC::CompleteNotifyMwiChange failed: %d" );
        iUICCLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }


// -----------------------------------------------------------------------------
// CUICC::ReadDynamicFlags
// -----------------------------------------------------------------------------
//
TInt CUICC::ReadDynamicFlags( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::ReadDynamicFlags" );
    iUICCLog->Log( (TDesC8)KLog );

    CMmDataPackage package;

    // Fetch parameters for comparision
    TInt temp( 0 );
    aItem.GetNextInt( temp );
    iAlsLine = static_cast<RMobilePhone::TMobilePhoneALSLine>( temp );

    TInt ret( HandleRequestL( EMobilePhoneGetALSLine ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "ReadDynamicFlags request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "ReadDynamicFlags request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteReadDynamicFlags
// -----------------------------------------------------------------------------
//
void CUICC::CompleteReadDynamicFlags(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CUICC::CompleteReadDynamicFlags - result: %d" );
    iUICCLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneALSLine alsLine;
        aDataPackage->UnPackData( alsLine );

        if ( iAlsLine != alsLine )
            {
            aResult = KErrGeneral;
            }
        }
    else
        {
        _LIT8( KFail, "CUICC::CompleteReadDynamicFlags failed: %d" );
        iUICCLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }


// -----------------------------------------------------------------------------
// CUICC::WriteDynamicFlags
// -----------------------------------------------------------------------------
//
TInt CUICC::WriteDynamicFlags( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::WriteDynamicFlags" );
    iUICCLog->Log( (TDesC8)KLog );

    CMmDataPackage package;

    TInt temp( 0 );
    aItem.GetNextInt( temp );
    iAlsLine = static_cast<RMobilePhone::TMobilePhoneALSLine>( temp );

    package.PackData( &iAlsLine );

    TInt ret( HandleRequestL( EMobilePhoneSetALSLine, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "WriteDynamicFlags request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "WriteDynamicFlags request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::ReadDynamic2Flags
// -----------------------------------------------------------------------------
//
TInt CUICC::ReadDynamic2Flags( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::ReadDynamic2Flags" );
    iUICCLog->Log( (TDesC8)KLog );

    CMmDataPackage package;

    // Fetch parameters for comparision
    TInt temp( 0 );
    aItem.GetNextInt( temp );
    iBlockStatus = static_cast<RMmCustomAPI::TGetAlsBlockStatus>( temp );

    TInt ret( HandleRequestL( ECustomGetAlsBlockedIPC ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "ReadDynamic2Flags request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
        _LIT8( KRequestFailed, "ReadDynamic2Flags request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteReadDynamic2Flags
// -----------------------------------------------------------------------------
//
void CUICC::CompleteReadDynamic2Flags(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CUICC::CompleteReadDynamic2Flags - result: %d" );
    iUICCLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        RMmCustomAPI::TGetAlsBlockStatus blockStatus;
        aDataPackage->UnPackData( blockStatus );

        if ( iBlockStatus != blockStatus )
            {
            aResult = KErrGeneral;
            }
        }
    else
        {
        _LIT8( KFail, "CUICC::CompleteReadDynamicFlags failed: %d" );
        iUICCLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }


// -----------------------------------------------------------------------------
// CUICC::WriteDynamic2Flags
// -----------------------------------------------------------------------------
//
TInt CUICC::WriteDynamic2Flags( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::WriteDynamic2Flags" );
    iUICCLog->Log( (TDesC8)KLog );

    CMmDataPackage package;
    RMmCustomAPI::TSetAlsBlock blockStatus;

    TInt temp( 0 );
    aItem.GetNextInt( temp );
    blockStatus = static_cast<RMmCustomAPI::TSetAlsBlock>( temp );

    package.PackData( &blockStatus );

    TInt ret( HandleRequestL( ECustomSetAlsBlockedIPC, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "WriteDynamic2Flags request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "WriteDynamic2Flags request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::GetUiccStatus
// -----------------------------------------------------------------------------
//
TInt CUICC::GetUiccStatus( CStifItemParser& /*aItem*/ )
    {
    _LIT8( KLog, "CUICC::GetUiccStatus" );
    iUICCLog->Log( (TDesC8)KLog );

    TInt ret( HandleRequestL( EMmTsyBootNotifySimStatusReadyIPC ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "GetUiccStatus request send ok" );
        iUICCLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "GetUiccStatus request send failed: %d" );
        iUICCLog->Log( (TDesC8)KRequestFailed, ret );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::PowerOffSim
//
// -----------------------------------------------------------------------------
//
TInt CUICC::PowerOffSim()
    {
    _LIT8(KWriteONStore, "CUICC::PowerOffSim ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ECustomPowerSimOffIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SimPowerOffRequest request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "SimPowerOffRequestrequest send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompletePowerOffSim
//
// -----------------------------------------------------------------------------
//
void CUICC::CompletePowerOffSim( TInt aResult )
    {
    _LIT8( KResult, "CUICC::CompletePowerOffSim : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::SimWarmReset
//
// -----------------------------------------------------------------------------
//
TInt CUICC::SimWarmReset()
    {
    _LIT8(KWriteONStore, "CUICC::SimWarmReset ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ECustomSimWarmResetIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SimWarmReset request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "SimWarmReset send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteSimWarmReset
//
// -----------------------------------------------------------------------------
//
void CUICC::CompleteSimWarmReset( TInt aResult )
    {
    _LIT8( KResult, "CUICC::CompleteSimWarmReset : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::SimGetATR
//
// -----------------------------------------------------------------------------
//
TInt CUICC::SimGetATR(CStifItemParser& /*aItem*/)
    {
    _LIT8(KWriteONStore, "CUICC::SimGetATR ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        iAPDU.Append(0x3b);
        iAPDU.Append(0x97);
        iAPDU.Append(0x95);
        iAPDU.Append(0x91);
        iAPDU.Append(0x81);
        iAPDU.Append(0xb1);
        iAPDU.Append(0xfe);
        iAPDU.Append(0x00);
        iAPDU.Append(0x1f);
        iAPDU.Append(0x42);
        iAPDU.Append(0x80);
        iAPDU.Append(0x31);
        iAPDU.Append(0xa0);
        iAPDU.Append(0x73);
        iAPDU.Append(0xbe);
        iAPDU.Append(0x21);
        iAPDU.Append(0x00);
        iAPDU.Append(0x00);
        _LIT8(KAPDU, "CUICC::SimGetATR APDU");
        iUICCLog->Log(iAPDU );
        }

    ret = HandleRequestL( ECustomGetATRIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SimGetATR request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "SimGetATR send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteSimGetATR
//
// -----------------------------------------------------------------------------
//
void CUICC::CompleteSimGetATR( TInt aResult,
        CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CUICC::CompleteSimGetATR : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        TPtrC8* tPtr;
        aDataPackage->UnPackData( tPtr );

        if( iHandleSpecifiedRequests )
            {
            if ( iAPDU != *tPtr)
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::PowerOnSim
//
// -----------------------------------------------------------------------------
//
TInt CUICC::PowerOnSim()
    {
    _LIT8(KWriteONStore, "CUICC::PowerOnSim ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ECustomPowerSimOnIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SimPowerOnRequest request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "SimPowerOnRequestrequest send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompletePowerOnSim
//
// -----------------------------------------------------------------------------
//
void CUICC::CompletePowerOnSim( TInt aResult )
    {
    _LIT8( KResult, "CUICC::CompletePowerOnSim : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::GetRfStatus
// -----------------------------------------------------------------------------
//
TInt CUICC::GetRfStatus( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::GetRfStatus" );
    iUICCLog->Log( (TDesC8)KLog );

    // Just save parameter for validity checking
    TInt temp( 0 );
    aItem.GetNextInt( temp );
    iStatusInfo = static_cast<TRfStateInfo>( temp );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetRfStatus
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetRfStatus(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CUICC::CompleteGetRfStatus - result: %d" );
    iUICCLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        TRfStateInfo statusInfo;
        aDataPackage->UnPackData( statusInfo );

        if ( iStatusInfo != statusInfo )
            {
            aResult = KErrGeneral;
            }
        }
    else
        {
        _LIT8( KFail, "CUICC::CompleteGetRfStatus failed: %d" );
        iUICCLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CUICC::SendAPDUv2
//
// -----------------------------------------------------------------------------
//
TInt CUICC::SendAPDUv2()
    {
    _LIT8(KWriteONStore, "CUICC::SendAPDUv2 ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );


    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        iAPDU.Append(0x3b);
        iAPDU.Append(0x97);
        iAPDU.Append(0x95);
        iAPDU.Append(0x91);
        }

    RMmCustomAPI::TApduParameters apduParameters;

    // APDU command
    apduParameters.iCmdData.Append(0xa0);
    apduParameters.iCmdData.Append(0xc0);
    apduParameters.iCmdData.Append(0x00);
    apduParameters.iCmdData.Append(0x00);

    //pack aDataPackage
    CMmDataPackage dataPackage;
    dataPackage.PackData( &apduParameters );

    ret = HandleRequestL( ECustomSendAPDUReqV2IPC, &dataPackage );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SendAPDUv2 request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "SendAPDUv2 send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteSendAPDUv2
//
// -----------------------------------------------------------------------------
//
void CUICC::CompleteSendAPDUv2( TInt aResult,
        CMmDataPackage* aDataPackage )
    {

    _LIT8( KResult, "CUICC::CompleteSendAPDUv2 : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        TPtrC8* tPtr;
        aDataPackage->UnPackData( tPtr );

        if( iHandleSpecifiedRequests )
            {
            if ( iAPDU != *tPtr)
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::GetCardReaderStatus
//
// -----------------------------------------------------------------------------
//
TInt CUICC::GetCardReaderStatus()
    {
    _LIT8(KWriteONStore, "CUICC::GetCardReaderStatus ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );
    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        iCardReaderStatusByte.Append(0xD0);
        }

    ret = HandleRequestL( ECustomGetSimCardReaderStatusIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetCardReaderStatus request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "GetCardReaderStatus send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetCardReaderStatus
//
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetCardReaderStatus( TInt aResult,
        CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CUICC::CompleteGetCardReaderStatus : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        TBuf8<1> tBuf;
        //unpack data
        TPtrC8* ptr;

        aDataPackage->UnPackData( &ptr );

        tBuf.Copy(ptr[0]);

        if( iHandleSpecifiedRequests )
            {
            if ( iCardReaderStatusByte != tBuf)
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::UiccCardInd
//
// -----------------------------------------------------------------------------
//
TInt CUICC::UiccCardInd()
    {
    _LIT8(KWriteONStore, "CUICC::UiccCardInd ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNone);
    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        iCardStatus = RMmCustomAPI::SimCardRemoved;
        iUiccCardIndTest = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteUiccCardInd
//
// -----------------------------------------------------------------------------
//
void CUICC::CompleteUiccCardInd( TInt aResult, CMmDataPackage* aDataPackage )
    {
     _LIT8( KResult, "CUICC::CompleteUiccCardInd : %d" );
     iUICCLog->Log((TDesC8)KResult, aResult );

     if ( KErrNone == aResult )
         {
         RMmCustomAPI::TSIMCardStatus returnStatus;

         aDataPackage->UnPackData( returnStatus );

         if( iHandleSpecifiedRequests )
             {
             if ( iCardStatus != returnStatus)
                 {
                 aResult = KErrGeneral;
                 }
             }
         }
     else
         {
         aResult = KErrGeneral;
         }
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::SendAPDU
//
// -----------------------------------------------------------------------------
//
TInt CUICC::SendAPDU()
    {
    _LIT8(KWriteONStore, "CUICC::SendAPDU ");
    iUICCLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        iAPDU.Append(0x3b);
        iAPDU.Append(0x97);
        iAPDU.Append(0x95);
        iAPDU.Append(0x91);
        }

    RMmCustomAPI::TApduParameters apduParameters;

    // APDU command
    apduParameters.iCmdData.Append(0xa0);
    apduParameters.iCmdData.Append(0xc0);
    apduParameters.iCmdData.Append(0x00);
    apduParameters.iCmdData.Append(0x00);

    // packed parameter: RMmCustomAPI::TApdu
    RMmCustomAPI::TApdu apduData;

    apduData.iData = &( apduParameters.iCmdData );

    //pack aDataPackage
    CMmDataPackage dataPackage;
    dataPackage.PackData( &apduData );

    ret = HandleRequestL( ECustomSendAPDUReqIPC, &dataPackage );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SendAPDU request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "SendAPDU send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteSendAPDU
// -----------------------------------------------------------------------------
//
void CUICC::CompleteSendAPDU( TInt aResult,
        CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CUICC::CompleteSendAPDU : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        TPtrC8* tPtr;
        aDataPackage->UnPackData( tPtr );

        if( iHandleSpecifiedRequests )
            {
            if ( iAPDU != *tPtr)
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        aResult = KErrGeneral;
        }
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CUICC::NotifySecurityEvent
// -----------------------------------------------------------------------------
//
TInt CUICC::NotifySecurityEvent( CStifItemParser& aItem )
    {
    _LIT8( KLog, "NotifySecurityEvent" );
    iUICCLog->Log((TDesC8)KLog );

    // Just save parameter for validity checking
    TInt temp( 0 );
    aItem.GetNextInt( temp );
    iEvent = static_cast<RMobilePhone::TMobilePhoneSecurityEvent>( temp );

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CUICC::CompleteNotifySecurityEvent
// -----------------------------------------------------------------------------
//
void CUICC::CompleteNotifySecurityEvent(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    RMobilePhone::TMobilePhoneSecurityEvent event;
    aDataPackage->UnPackData( event );

    _LIT8( KResult, "CUICC::CompleteNotifySecurityEvent : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if ( iEvent != event )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::VerifySecurityCode
// -----------------------------------------------------------------------------
//
TInt CUICC::VerifySecurityCode( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::VerifySecurityCode" );
    iUICCLog->Log( ( TDesC8 ) KLog );

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
        }

    // 3rd parameter: unblock code
    if ( KErrNone == ret )
        {
        ret = aItem.GetNextString( string );
        if ( KErrNone == ret )
            {
            codes.iUnblockCode.Copy( string );
            }
        }

    CMmDataPackage package;
    package.PackData( &type, &codes );
    ret = HandleRequestL( EMobilePhoneVerifySecurityCode, &package );
    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::GetPinInfo
// -----------------------------------------------------------------------------
//
TInt CUICC::GetPinInfo( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::GetPinInfo" );
    iUICCLog->Log( ( TDesC8 ) KLog );

    RMobilePhone::TMobilePhoneLock lock;
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
    lockInfo.iStatus = RMobilePhone::EStatusLockUnknown;
    lockInfo.iSetting = RMobilePhone::ELockSetUnknown;

    TInt ret( KErrNone );
    TPtrC string;
    aItem.GetNextString( string );
    if ( string == _L("ELockICC") )
        {
        lock = RMobilePhone::ELockICC;
        }
    else if ( string == _L("ELockPin2") )
        {
        lock = RMobilePhone::ELockPin2;
        }
    else if ( string == _L("ELockUniversalPin") )
        {
        lock = RMobilePhone::ELockUniversalPin;
        }
    else
        {
        ret = KErrArgument;
        }

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
    else if ( string == _L("Blocked") )
        {
        iLockInfo.iStatus = RMobilePhone::EStatusBlocked;
        }

    CMmDataPackage package;
    package.PackData( &lock, &lockInfo );
    ret = HandleRequestL( EMobilePhoneGetLockInfo, &package );
    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetPinInfo
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetPinInfo(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CUICC::CompleteGetPinInfo : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    //unpack the data
    RMobilePhone::TMobilePhoneLockStatus* status = NULL;
    RMobilePhone::TMobilePhoneLockSetting* setting = NULL;

    aDataPackage->UnPackData( &status, &setting );

    if ( iLockInfo.iStatus != *status  )
        {
        aResult = KErrGeneral;
        }

    if ( iLockInfo.iSetting != *setting )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::ChangePinCode
// -----------------------------------------------------------------------------
//
TInt CUICC::ChangePinCode( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUicc::ChangePinCode" );
    iUICCLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );
    TPtrC  string;

    RMobilePhone::TMobilePhoneSecurityCode securityCode;
    RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;

    // 1st parameter: code type
    ret = GetSecurityCode( aItem, securityCode );
    // 2nd parameter: old password
    ret = aItem.GetNextString( string );
    passwordChange.iOldPassword.Copy( string );
    // 3rd parameter: new password
    ret = aItem.GetNextString( string );
    passwordChange.iNewPassword.Copy( string );

    CMmDataPackage package;
    package.PackData( &securityCode, &passwordChange );
    ret = HandleRequestL( EMobilePhoneChangeSecurityCode, &package );
    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::SetLockSetting
// -----------------------------------------------------------------------------
//
TInt CUICC::SetLockSetting(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::SetLockSettingL" );
    iUICCLog->Log( ( TDesC8 ) KLog );

    RMobilePhone::TMobilePhoneLock lock;
    RMobilePhone::TMobilePhoneLockSetting setting;
    RMobilePhone::TMobilePassword password;
    TPtrC string;

    // 1st parameter: lock
    aItem.GetNextString( string );
    if ( string == _L("ELockICC") )
        {
        lock = RMobilePhone::ELockICC;
        }
    else if ( string == _L("ELockPin2") )
        {
        lock = RMobilePhone::ELockPin2;
        }
    else if ( string == _L("ELockUniversalPin") )
        {
        lock = RMobilePhone::ELockUniversalPin;
        }

    // 2nd parameter: lock setting
    aItem.GetNextString( string );
    if ( _L("ELockSetEnabled") == string )
        {
        setting = RMobilePhone::ELockSetEnabled;
        }
    else if ( _L("ELockSetDisabled") == string )
        {
        setting = RMobilePhone::ELockSetDisabled;
        }
    else if ( _L("ELockSetReplaced") == string )
        {
        setting = RMobilePhone::ELockReplaced;
        }

    // 3rd parameter: code
    aItem.GetNextString( string );
    password.Copy( string );

    CMmDataPackage package;
    TLockAndSetting lockAndSetting;
    lockAndSetting.iLock = &lock;
    lockAndSetting.iSetting = &setting;

    package.PackData( &lockAndSetting, &password );
    return HandleRequestL( EMobilePhoneSetLockSetting, &package );
    }


// -----------------------------------------------------------------------------
// CUICC::GetPinAttemptsLeft
// -----------------------------------------------------------------------------
//
TInt CUICC::GetPinAttemptsLeft( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::GetPinAttemptsLeft" );
    iUICCLog->Log( ( TDesC8 ) KLog );

    RMobilePhone::TMobilePhoneSecurityCode type;

    TInt ret( KErrNone );
    TPtrC string;
    aItem.GetNextString( string );
    if ( string == _L("ESecurityCodePin1") )
        {
        type = RMobilePhone::ESecurityCodePin1;
        }
    else if ( string == _L("ESecurityCodePin2") )
        {
        type = RMobilePhone::ESecurityCodePin2;
        }
    else if ( string == _L("ESecurityUniversalPin") )
        {
        type = RMobilePhone::ESecurityUniversalPin;
        }
    else
        {
        ret = KErrArgument;
        }

    aItem.GetNextInt( iPinAttemptsLeft );

    if ( KErrNone == ret )
        {
        CMmDataPackage package;
        package.PackData( &type );
        ret = HandleRequestL( EMobilePhoneGetSecurityCodeInfo, &package );
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::CompleteGetPinInfo
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetPinAttemptsLeft(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CUICC::CompleteGetPinInfo : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    RMobilePhone::TMobilePhoneSecurityCode* codeType;
    RMobilePhone::TMobilePhoneSecurityCodeInfoV5* securityInfo;

    aDataPackage->UnPackData( &codeType, &securityInfo );

    if ( iPinAttemptsLeft != securityInfo->iRemainingEntryAttempts )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::GetIccType
// -----------------------------------------------------------------------------
//
TInt CUICC::GetIccType( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSecurity::GetICCType" );
    iUICCLog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

    TInt temp;
    aItem.GetNextInt( temp );
    iIccType = ( TICCType ) temp;

    ret = HandleRequestL( EMmTsySimGetICCType );

    return ret;
    }


// -----------------------------------------------------------------------------
// CUICC::CompleteGetIccType
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetIccType( TInt aResult, CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CUICC::CompleteGetIccType : %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    TICCType* iccType = NULL;
    aDataPackage->UnPackData( &iccType );

    if ( iIccType != *iccType )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::GetCurrentActiveUSIMApplication
// Get current active USIM application
// -----------------------------------------------------------------------------
//
TInt CUICC::GetCurrentActiveUSIMApplication( CStifItemParser& aItem )
    {
    iUICCLog->Log(_L8("CUICC::GetCurrentActiveUSIMApplication"));
    TPtrC string;
    aItem.GetNextString( string );

    TInt ret( KErrNone );

    if ( _L("147377136") == string)
        {
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

    ret = HandleRequestL( EMobilePhoneGetCurrentActiveUSimApplication );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetCurrentActiveUSIMApplication request send ok" );
        iUICCLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "GetCurrentActiveUSIMApplication request send failed: %d" );
        iUICCLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteGetCurrentActiveUSIMApplication
// Complete GetCurrentActiveUSIMApplication method function.
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetCurrentActiveUSIMApplication
    (
    TInt aResult,
    CMmDataPackage* aDataPackage
    )
    {
    _LIT8( KResult, "CompleteGetCurrentActiveUSIMApplication result: %d" );
    iUICCLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        RMobilePhone::TAID* activeAID;
        //unpack data
        aDataPackage->UnPackData( &activeAID );

        _LIT8( KActiveAID, "ActiveAID = %d" );
        iUICCLog->Log((TDesC8)KActiveAID, activeAID  );
        if ( *activeAID != iActiveID)
            {
            aResult = KErrGeneral;
            }
        }
    else if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetCurrentActiveUSIMApplication Failed: %d" );
        iUICCLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CUICC::GetActivePin
// -----------------------------------------------------------------------------
//
TInt CUICC::GetActivePin(
    CStifItemParser& aItem )
    {
    _LIT8( KLog, "CUICC::GetActivePin" );
    iUICCLog->Log( ( TDesC8 ) KLog );

    TInt temp( 0);
    aItem.GetNextInt( temp );
    iActivePin = (RMobilePhone::TMobilePhoneSecurityCode) temp;

    TInt ret( KErrNone );

    ret = HandleRequestL( EMmTsySecurityGetSimActivePinStateIPC );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUICC::CompleteSecurityGetSimActivePinState
// -----------------------------------------------------------------------------
//
void CUICC::CompleteGetActivePin(
    TInt aErr,
    CMmDataPackage& aData )
    {
    _LIT8( KLog, "CUICC::CompleteGetActivePin, err = %d" );
    iUICCLog->Log( ( TDesC8 ) KLog, aErr );

    RMobilePhone::TMobilePhoneSecurityCode* activePin = NULL;
    // Get the new data
    aData.UnPackData( &activePin );

    switch ( *activePin )
        {
        case RMobilePhone::ESecurityCodePin1:
            {
            _LIT8( KActive,
                "CUICC::CompleteGetActivePin -- active pin = ESecurityCodePin1" );
            iUICCLog->Log( ( TDesC8 ) KActive );
            break;
            }
        case RMobilePhone::ESecurityUniversalPin:
            {
            _LIT8( KActive,
                "CUICC::CompleteGetActivePin -- active pin = ESecurityUniversalPin" );
            iUICCLog->Log( ( TDesC8 ) KActive );
            break;
            }
        default:
            {
            _LIT8( KActive,
                "CUICC::CompleteSecurityGetSimActivePinState, bad pin (%d)" );
            iUICCLog->Log( ( TDesC8 ) KActive, TInt( *activePin ) );
            aErr = KErrGeneral;
            break;
            }
        }

    if ( iActivePin != *activePin )
        {
        aErr = KErrGeneral;
        }

    Signal( aErr );
    }




// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


//  End of File

