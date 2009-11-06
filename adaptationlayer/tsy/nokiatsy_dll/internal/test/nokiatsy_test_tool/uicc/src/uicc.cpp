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
#include <stiftestinterface.h>
#include "uicc.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUICC::CUICC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUICC::CUICC( CTestModuleIf& aTestModuleIf ): CCore( aTestModuleIf )
    {
    // Initial values
    iSavedViagElementsForComparison = NULL;
    }

// -----------------------------------------------------------------------------
// CUICC::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUICC::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iUICCLog = CStifLogger::NewL( KSSLogPath,
                          KSSLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    iRefPBStoreInfoData = new( ELeave ) CStorageInfoData();
    InitializeIscControlL(KIscControlTestCaseFileUICC);

    _LIT8(KSS, "CUICC: ConstructL");
    iUICCLog->Log((TDesC8)KSS );

    }

// -----------------------------------------------------------------------------
// CUICC::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUICC* CUICC::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CUICC* self = new (ELeave) CUICC( aTestModuleIf );

    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CUICC::~CUICC
// Destructor
// -----------------------------------------------------------------------------
//
CUICC::~CUICC()
    {
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iUICCLog;

    if ( iSavedViagElementsForComparison )
        {
        delete iSavedViagElementsForComparison;
        iSavedViagElementsForComparison = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUICC::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CUICC::RouteCompletion(
    TInt aIpc,
    CMmDataPackage* aDataPackage,
    TInt aResult )
    {
    // Process the ISI msg according to IPC requested.
    _LIT8(KResult, "CUICC::RouteCompletion result: %d");
    iUICCLog->Log((TDesC8)KResult, aResult );

    if ( !iHandleSpecifiedRequests )
        {
        _LIT8(KDefault, "CUICC::RouteCompletion - only Specified requests are handled");
        iUICCLog->Log((TDesC8)KDefault );
        }
    else
        {
        switch(aIpc)
            {
            case EMobilePhoneGetCustomerServiceProfile:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneGetCustomerServiceProfile" ) ) )
                    {
                    CompleteGetCustomerServiceProfile( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomReadSimFileIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomReadSimFileIPC" ) ) )
                    {
                    CompleteReadSimFile( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneGetMailboxNumbers:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetMailboxNumbers" ) ) )
                    {
                    CompleteGetMailboxIdentifier( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneGetHomeNetwork:
                {
                Signal( aResult );
                break;
                }
/*            case ECustomPowerSimOnIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomPowerSimOnIPC" ) ) )
                    {
                    CompletePoweringSimOn( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }*/

            case ECustomGetLifeTimeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "ECustomGetLifeTimeIPC" ) ) )
                    {
                    CompleteGetLifeTimer( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneGetSubscriberId:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneGetSubscriberId" ) ) )
                    {
                    CompleteGetSubscriberId( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneGetCipheringIndicatorStatus:
            case ECustomGetCipheringInfoIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetCipheringIndicatorStatus" ) ) ||
                    0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomGetCipheringInfoIPC" ) ) )
                    {
                    CompleteGetCiStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneSetCallForwardingStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneSetCallForwardingStatus" ) ) )
                    {
                    CompleteSetCallFwdStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomNotifyIccCallForwardingStatusChangeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomNotifyIccCallForwardingStatusChangeIPC" ) ) )
                    {
                    CompleteNotifyIccCallForwardingStatusChange(
                        aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneGetServiceProviderName:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneGetServiceProviderName" ) ) )
                    {
                    CompleteGetServiceProviderName( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMmTsyPhoneBookStoreInitIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMmTsyPhoneBookStoreInitIPC" ) ) )
                    {
                    CompletePbInit( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomStartSimCbTopicBrowsingIPC:
                {
                CompleteReadCBSSimTopics( aResult, aDataPackage );
                break;
                }
            case ECustomDeleteSimCbTopicIPC:
                {
                CompleteDeleteCBSSimTopic( aResult );
                break;
                }
            case ECustomGetNetworkProviderNameIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "ECustomGetNetworkProviderNameIPC" ) ) )
                    {
                    CompleteGetNetworkProviderName( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EReadViagHomeZoneParamsIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EReadViagHomeZoneParamsIPC" ) ) )
                    {
                    CompleteViagHomeZoneReadParam( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EReadViagHomeZoneCacheIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EReadViagHomeZoneCacheIPC" ) ) )
                    {
                    CompleteViagHomeZoneReadCache( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EWriteViagHomeZoneCacheIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EWriteViagHomeZoneCacheIPC" ) ) )
                    {
                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EWriteViagHomeZoneUHZIUESettingsIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EWriteViagHomeZoneUHZIUESettingsIPC" ) ) )
                    {
                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneGetIccMessageWaitingIndicators:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetIccMessageWaitingIndicators" ) ) )
                    {
                    CompleteReadVoiceMsgFlags( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneSetIccMessageWaitingIndicators:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneSetIccMessageWaitingIndicators" ) ) )
                    {
                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneNotifyIccMessageWaitingIndicatorsChange" ) ) )
                    {
                    CompleteNotifyMwiChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneGetALSLine:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetALSLine" ) ) )
                    {
                    CompleteReadDynamicFlags( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobilePhoneSetALSLine:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneSetALSLine" ) ) )
                    {
                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomGetAlsBlockedIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomGetAlsBlockedIPC" ) ) )
                    {
                    CompleteReadDynamic2Flags( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomSetAlsBlockedIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomSetAlsBlockedIPC" ) ) )
                    {
                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMmTsyBootNotifySimStatusReadyIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyBootNotifySimStatusReadyIPC" ) ) )
                    {
                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomPowerSimOffIPC:
                {
                CompletePowerOffSim( aResult );
                break;
                }
            case ECustomSimWarmResetIPC:
                {
                CompleteSimWarmReset( aResult );
                break;
                }
            case ECustomGetATRIPC:
                {
                CompleteSimGetATR( aResult, aDataPackage );
                break;
                }
            case ECustomPowerSimOnIPC:
                {
                CompletePowerOnSim( aResult );
                break;
                }
            case EMmTsyBootNotifyModemStatusReadyIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyBootNotifyModemStatusReadyIPC" ) ) )
                    {
                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMmTsyBootGetRFStatusIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyBootGetRFStatusIPC" ) ) )
                    {
                    CompleteGetRfStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomSendAPDUReqV2IPC:
                {
                CompleteSendAPDUv2( aResult, aDataPackage );
                break;
                }
            case ECustomGetSimCardReaderStatusIPC:
                {
                CompleteGetCardReaderStatus( aResult, aDataPackage );
                break;
                }
            case ECustomNotifySimCardStatusIPC:
                {
                if(iUiccCardIndTest)
                    {
                    CompleteUiccCardInd( aResult, aDataPackage );
                    }
                CCore::GetNextExpectedCompleteIPC();
                break;
                }
            case ECustomSendAPDUReqIPC:
                {
                CompleteSendAPDU( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneNotifySecurityEvent:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneNotifySecurityEvent" ) ) )
                    {
                    CompleteNotifySecurityEvent( aResult, aDataPackage );
                    }
                break;
                }
            case EMobilePhoneVerifySecurityCode:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneVerifySecurityCode" ) ) )
                    {
                    Signal( aResult );
                    }
                break;
                }
            case EMobilePhoneGetLockInfo:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetLockInfo" ) ) )
                    {
                    CompleteGetPinInfo( aResult, aDataPackage );
                    break;
                    }
                break;
                }
            case EMobilePhoneChangeSecurityCode:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneChangeSecurityCode" ) ) )
                    {
                    Signal( aResult );
                    }
                break;
                }
            case EMobilePhoneSetLockSetting:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneSetLockSetting" ) ) )
                    {
                    Signal( aResult );
                    }
                break;
                }
            case EMobilePhoneGetSecurityCodeInfo:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetSecurityCodeInfo" ) ) )
                    {
                    CompleteGetPinAttemptsLeft( aResult, aDataPackage );
                    }
                break;
                }
            case EMmTsySimGetICCType:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsySimGetICCType" ) ) )
                    {
                    CompleteGetIccType( aResult, aDataPackage );
                    }
                break;
                }
            case EMobilePhoneGetCurrentActiveUSimApplication :
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneGetCurrentActiveUSimApplication" ) ) )
                    {
                    CompleteGetCurrentActiveUSIMApplication( aResult, aDataPackage );
                    }
                break;
                }
            case EMmTsySecurityGetSimActivePinStateIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsySecurityGetSimActivePinStateIPC" ) ) )
                    {
                    CompleteGetActivePin( aResult, *aDataPackage );
                    }
                break;
                }
            default:
                {
                _LIT8(KDefault, "CUICC::RouteCompletion default");
                iUICCLog->Log((TDesC8)KDefault );
                break;
                }
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

    return ( CScriptBase* ) CUICC::NewL( aTestModuleIf );

    }

//  End of File
