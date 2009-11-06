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
* Description:   Specific class for Voice tests
*
*/



// INCLUDE FILES
#include <stiftestinterface.h>
#include "voice.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVoice::CVoice
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVoice::CVoice(
    CTestModuleIf& aTestModuleIf ):
        CCore( aTestModuleIf ), iCallParamsPckg( iCallParams )
    {

    }

// -----------------------------------------------------------------------------
// CVoice::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVoice::VoiceConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif
    iVoiceLog = CStifLogger::NewL( KVoiceLogPath,
                          KVoiceLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    iTestOngoing     = ENoTestOngoing;
    iCallId = -1;
    iAnswerIncomingCall = EFalse;
    iIndicationCounter = 0;
    iNumberOfCallsInCC = 0;
    iDontAnswerCall = EFalse;
    iVerifyConnectedStatus = EUnknown;
    iVoiceParameterCheck = EFalse;
    iTestCaseNumber = ETestCaseNotDefined;
    iDTMFState = ENotSet;

    InitializeIscControlL(KIscControlTestCaseFileCVoice);

    _LIT8(Voice, "Voice: ConstructL");
    iVoiceLog->Log((TDesC8)Voice );

    }

// -----------------------------------------------------------------------------
// CVoice::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVoice* CVoice::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CVoice* self = new (ELeave) CVoice( aTestModuleIf );

    CleanupStack::PushL( (CScriptBase*)self );
    self->VoiceConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CVoice::~CVoice()
    {

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iVoiceLog;

    }

// -----------------------------------------------------------------------------
// CVoice::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CVoice::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CVoice::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVoice::RunMethodL( CStifItemParser& aItem )
    {

    _LIT8 (KLog, "Voice: RunMethodL" );
    iVoiceLog->Log((TDesC8)KLog);

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CallDial", CVoice::CallDialL),
        ENTRY( "CallHangUp", CVoice::CallHangUpL),
        ENTRY( "SendDTMFTones", CVoice::SendDTMFTonesL),
        ENTRY( "ContinueSendDTMFTones", CVoice::ContinueSendDTMFTonesL),
        ENTRY( "TestIncomingCall", CVoice::TestIncomingCall),
        ENTRY( "TestHoldCall", CVoice::TestHoldCallL),
        ENTRY( "RemotePartyHangUp", CVoice::RemotePartyHangUp),
        ENTRY( "TestResumeCall", CVoice::TestResumeCallL),
        ENTRY( "TestSwapCalls", CVoice::TestSwapCallsL),
        ENTRY( "TestCallTransfer", CVoice::TestCallTransferL),
        ENTRY( "ClearBlacklist", CVoice::ClearBlacklistL),
        ENTRY( "RunIscTestCase", CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase", CCore::SyncIscTestCase),
        ENTRY( "RunIscTestCaseStartNTSYAfterTestToolStart",
                CCore::RunIscTestCaseStartNTSYAfterTestToolStart),
        ENTRY( "SyncIscTestCaseStartNTSYAfterTestToolStart",
                CCore::SyncIscTestCaseStartNTSYAfterTestToolStart),
        ENTRY( "DialEmergencyCall", CVoice::DialEmergencyCall),
        ENTRY( "ActivateUUS", CVoice::ActivateUUSL),
        ENTRY( "IncomingCallWithUUS", CVoice::IncomingCallWithUUS),

        // Video Call
        ENTRY( "VideoCallDial", CVoice::VideoCallDialL),
        ENTRY( "VideoCallHangUp", CVoice::VideoCallHangUpL),
        ENTRY( "AnswerIncomingVideoCall", CVoice::AnswerIncomingVideoCallL),

        //Conference Call
        ENTRY( "CreateConference", CVoice::CreateConference),
        ENTRY( "HangUpCC", CVoice::HangUpCC),
        ENTRY( "AddCallToCC", CVoice::AddCallToCC),
        ENTRY( "HoldConference", CVoice::HoldConference),
        ENTRY( "ResumeConference", CVoice::ResumeConference),
        ENTRY( "SwapConference", CVoice::SwapConference),
        ENTRY( "GoOneToOne", CVoice::GoOneToOne),
        ENTRY( "VerifyRemoteAlertingToneStatus", CVoice::VerifyRemoteAlertingToneStatus),

        ENTRY( "RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined",
                CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined),
        ENTRY( "SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined",
                CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined),

        // Used for handling of waited IPCs
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC", CCore::SetExpectedCompleteIPC),
        ENTRY( "SetVoiceParameterCheck", CVoice::SetVoiceParameterCheck),
        ENTRY( "SetVoiceReferenceParameters", CVoice::SetVoiceReferenceParameters),
        ENTRY( "UpdateLifeTimer", CVoice::UpdateLifeTimer),
        ENTRY( "GetLifeTimer", CVoice::GetLifeTimer),
        };

    const TInt count( sizeof( KFunctions ) / sizeof( TStifFunctionInfo ) );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVoice::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CVoice::RouteCompletion
        (
        TInt aIpc, // IPC value
        CMmDataPackage* aDataPackage, //pointer to DataPackage
        TInt aResult //Completion result
        )
    {

    _LIT8(KResult,"CVoice::RouteCompletion result: %d");
    iVoiceLog->Log((TDesC8)KResult, aResult);

    if ( !iHandleSpecifiedRequests )
        {
        switch(aIpc)
            {
            case EMobilePhoneSendDTMFTones:
                {
                CompleteSendDTMFTones( aResult );
                break;
                }

            case EEtelLineNotifyIncomingCall:
                {
                if ( EIncomingCallWithUUSTestOngoing == iTestOngoing )
                    {
                    CheckIncomingCallUUSCompletions( aResult, aDataPackage, aIpc );
                    }
                else
                    {
                    CompleteNotifyIncomingCall( aResult, aDataPackage );
                    }
                break;
                }

            case EMobileCallNotifyMobileCallStatusChange:
                {
                if ( EIncomingCallWithUUSTestOngoing == iTestOngoing )
                    {
                    CheckIncomingCallUUSCompletions( aResult, aDataPackage, aIpc );
                    }

                    CompleteCallStatusChange( aResult, aDataPackage );
                    break;
                    }
            case ECustomClearCallBlackListIPC:
                {
                CompleteClearBlacklist( aResult );
                break;
                }

            case EMobileCallHold:
            case EMobileCallResume:
            case EMobileCallSwap:
            case EMobileCallTransfer:
                {
                //always fail if these completes comes...
                Signal( aResult );
                iTestOngoing = ENoTestOngoing;
                break;
                }

            case ECustomGetRemoteAlertingToneStatusIPC:
                {
                CompleteVerifyRemoteAlertingToneStatus( aResult, aDataPackage );
                break;
                }

            case EMobileCallReceiveUUI:
            case EMobileCallGetMobileCallInfo:
                {
                if ( iTestOngoing == EIncomingCallWithUUSTestOngoing )
                    {
                    CheckIncomingCallUUSCompletions( aResult, aDataPackage, aIpc );
                    }
                // No else
                break;
                }

            case EEtelCallDial:
                {
                CompleteCallDial( aResult, aDataPackage );
                break;
                }

            case EEtelCallAnswer:
                {
                CompleteCallAnswer( aResult, aDataPackage );
                break;
                }

            default:
                {
                _LIT8(KComplete, "CVoice::RouteCompletion - Default, aIpc: %d" );
                iVoiceLog->Log((TDesC8)KComplete, aIpc );
                break;
                }
            }
        }
    else
        {
        TBool isIPCHandled = EFalse;
        switch(aIpc)
            {

            case EMobilePhoneSendDTMFTones:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneSendDTMFTones" ) ) )
                    {
                    CompleteSendDTMFTones( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneNotifyStopInDTMFString:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneNotifyStopInDTMFString" ) ) )
                    {
                    CompleteNotifyStopInDTMFString( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneNotifyDTMFEvent:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneNotifyDTMFEvent" ) ) )
                    {
                    CompleteNotifyDTMFEvent( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case ECustomClearCallBlackListIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "ECustomClearCallBlackListIPC" ) ) )
                    {
                    CompleteClearBlacklist( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileCallNotifyMobileCallStatusChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileCallNotifyMobileCallStatusChange" ) ) )
                    {
                    CompleteCallStatusChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomTerminateCallIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "ECustomTerminateCallIPC" ) ) )
                    {
                    CompleteCallTermination( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetRemoteAlertingToneStatusIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "ECustomGetRemoteAlertingToneStatusIPC" ) ) )
                    {
                    CompleteVerifyRemoteAlertingToneStatus( aResult,
                                                            aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileCallGetMobileCallInfo:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileCallGetMobileCallInfo" ) ) )
                    {
                    const CCallDataPackage* callData =
                        reinterpret_cast<const CCallDataPackage*>( aDataPackage );

                    CompleteMobileCallGetMobileCallInfo(
                        aResult,
                        callData );

                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileConferenceCallHangUp:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileConferenceCallHangUp" ) ) )
                    {

                    _LIT8(KComplete, "CVoice::RouteCompletion - EMobileConferenceCallHangUp, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetDiagnosticOctetsIPC:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "ECustomGetDiagnosticOctetsIPC" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - ECustomGetDiagnosticOctetsIPC, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EEtelCallHangUp:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EEtelCallHangUp" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - ECustomGetDiagnosticOctetsIPC, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileConferenceCallNotifyConferenceEvent:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileConferenceCallNotifyConferenceEvent" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EMobileConferenceCallNotifyConferenceEvent, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileConferenceCallCreateConference:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileConferenceCallCreateConference" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EMobileConferenceCallCreateConference, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileCallResume:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileCallResume" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EMobileCallResume, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileConferenceCallAddCall:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileConferenceCallAddCall" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EMobileConferenceCallAddCall, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }


            case EEtelLineNotifyIncomingCall:
                {
                 if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EEtelLineNotifyIncomingCall" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EEtelLineNotifyIncomingCall, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    CompleteNotifyIncomingCall( aResult, aDataPackage );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobileCallReceiveUUI:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileCallReceiveUUI" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EMobileCallReceiveUUI, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    CheckIncomingCallUUSCompletions( aResult, aDataPackage, aIpc );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMmTsyUpdateLifeTimeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMmTsyUpdateLifeTimeIPC" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EMmTsyUpdateLifeTimeIPC, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    Signal( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case ECustomGetLifeTimeIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "ECustomGetLifeTimeIPC" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - ECustomGetLifeTimeIPC, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    CompleteGetLifeTimer( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EEtelCallDial:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EEtelCallDial" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EEtelCallDial, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    CompleteCallDial( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EEtelCallAnswer:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EEtelCallAnswer" ) ) )
                    {
                    _LIT8(KComplete, "CVoice::RouteCompletion - EEtelCallAnswer, result: %d" );
                    iVoiceLog->Log((TDesC8)KComplete, aResult );

                    CompleteCallAnswer( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            default:
                {
                _LIT8(KComplete, "CVoice::RouteCompletion - Default, aIpc: %d" );
                iVoiceLog->Log((TDesC8)KComplete, aIpc );

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

    return ( CScriptBase* ) CVoice::NewL( aTestModuleIf );

    }

//  End of File
