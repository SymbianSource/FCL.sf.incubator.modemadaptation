/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <ctsy/serviceapi/gsmerror.h>
#include "voice.h"

// CONSTANTS
_LIT8( KLine, "---------------------------------" );


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSMS::SetVoiceParameterCheck
// Set voice parameter checking and test case number
// -----------------------------------------------------------------------------
//
TInt CVoice::SetVoiceParameterCheck( CStifItemParser& aItem )
    {
    TInt ret( KErrNone );
    _LIT8(KSetVoiceParameterCheck, "CVoice::SetVoiceParameterCheck");
    iVoiceLog->Log((TDesC8)KSetVoiceParameterCheck);

    // Is parameter checking required
    TPtrC string;
    aItem.GetNextString( string );

    if ( _L("ON") == string )
        {
        iVoiceParameterCheck = ETrue;
        }
    else
        {
        iVoiceParameterCheck = EFalse;
        }

    // Test case number, used in parameter checking
    TInt testCaseNumber;
    aItem.GetNextInt( testCaseNumber );
    iTestCaseNumber = (TTestCase) testCaseNumber;

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::SetVoiceReferenceParameters
// Sets voice parameter checking and test case number
// -----------------------------------------------------------------------------
//
TInt CVoice::SetVoiceReferenceParameters( CStifItemParser& aItem )
    {
    TInt ret( KErrNone );
    _LIT8(KSetVoiceReferenceParameters, "CVoice::SetVoiceReferenceParameters");
    iVoiceLog->Log((TDesC8)KSetVoiceReferenceParameters);

    // Set reference parameters depending on test case number
    if ( ETestCaseVoice032 == iTestCaseNumber ||
        ETestCaseVoice033 == iTestCaseNumber)
        {
        TInt callId;
        aItem.GetNextInt( callId );
        iRefCallId = callId;

        TInt callMode;
        aItem.GetNextInt( callMode );
        iRefCallMode = ( RMobilePhone::TMobileService ) callMode;

        TInt callStatus;
        aItem.GetNextInt( callStatus );
        iRefCallStatus = ( RMobileCall::TMobileCallStatus ) callStatus;
        }

    else if ( ETestCaseVoice050 == iTestCaseNumber )
        {
        TPtrC aTones;
        aItem.GetNextString(aTones);
        iRefDTMFStringToContinue.Copy( aTones );
        }


    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::CallDialL
// CallDial method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::CallDialL( CStifItemParser& aItem )
    {
    _LIT8 (KCallDial, "Voice::CallDial" );
    iVoiceLog->Log((TDesC8)KCallDial);

    TInt callId;
    TInt ret( KErrNotFound );

    ret = aItem.GetNextInt( callId );
    if ( KErrNone == ret )
        {
        ret = SetMobileCallParams( aItem );
        }

    if ( KErrNone == ret )
        {
        CCallDataPackage dataPackage;

        SetMobileCallInfo();

        aItem.GetNextInt( iDontAnswerCall );

        //Set call id and call mode
        dataPackage.SetCallIdAndMode(
                callId,
                RMobilePhone::EVoiceService );

        dataPackage.PackData(&iCallParamsPckg, &iMobileCallInfoV1);

        ret = HandleRequestL( EEtelCallDial, &dataPackage );

        if ( KErrNone == ret )
            {
            _LIT8(KCallOk, " Voice call successful" );
            iVoiceLog->Log((TDesC8)KCallOk );

            // If UUS test is ongoing, status is not allowed to be changed.
            // ECallWithUUSTestOngoing has been set during UUS activation.
            if ( ECallWithUUSTestOngoing != iTestOngoing )
                {
                iTestOngoing = ECallDialTestOngoing;
                }
            }
        else
            {
            _LIT8(KCallFailed, " Voice call fails: %d" );
            iVoiceLog->Log((TDesC8)KCallFailed );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::VideoCallDialL
// DataCallDial method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::VideoCallDialL( CStifItemParser& aItem )
    {
    _LIT8 (KDataCallDial, "Voice::VideoCallDial" );
    iVoiceLog->Log((TDesC8)KDataCallDial);

    TInt callId;
    TInt ret( KErrNotFound );

    ret = aItem.GetNextInt( callId );
    if ( KErrNone == ret )
        {
        ret = SetMobileCallParams( aItem );
        }

    if ( KErrNone == ret )
        {
        CCallDataPackage dataPackage;

        SetMobileCallInfo();

        aItem.GetNextInt( iDontAnswerCall );

        //Set call id and call mode
        dataPackage.SetCallIdAndMode(
                callId,
                RMobilePhone::ECircuitDataService );

        dataPackage.PackData(&iCallParamsPckg, &iMobileCallInfoV1);

        ret = HandleRequestL( EEtelCallDial, &dataPackage );

        if ( KErrNone == ret )
            {
            _LIT8(KDataCallOk, " Video call successful" );
            iVoiceLog->Log((TDesC8)KDataCallOk );

            // If UUS test is ongoing, status is not allowed to be changed.
            // ECallWithUUSTestOngoing has been set during UUS activation.
            if ( ECallWithUUSTestOngoing != iTestOngoing )
                {
                iTestOngoing = ECallDialTestOngoing;
                }
            }
        else
            {
            _LIT8(KDataCallFailed, " Video call fails: %d" );
            iVoiceLog->Log((TDesC8)KDataCallFailed );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteCallDial
// Complete Call Dial
// -----------------------------------------------------------------------------
//
void CVoice::CompleteCallDial(TInt aResult, CMmDataPackage* aDataPackage)
    {
    _LIT8(KResult, "CompleteCallDial result: %d");
    iVoiceLog->Log((TDesC8)KResult, aResult);

    if ( iHandleSpecifiedRequests )
        {
        CCallDataPackage& callData = *static_cast<CCallDataPackage*>(aDataPackage);
        // this IPC never completed for voice calls
        TInt callId;
        RMobilePhone::TMobileService mode;
        callData.GetCallIdAndMode(callId, mode);
        if ( RMobilePhone::ECircuitDataService != mode )
            {
            aResult = KErrGeneral;
            }
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteSendDTMFTones
// CompleteSendDTMFTones method function.
// -----------------------------------------------------------------------------
//
void CVoice::CompleteSendDTMFTones
        (
        TInt aResult //Completion result value
        )
    {
    _LIT8(KResult, "CompleteSendDTMFTones result: %d");
    iVoiceLog->Log((TDesC8)KResult, aResult);

    Signal ( aResult);
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteNotifyStopInDTMFString
// CompleteNotifyStopInDTMFString method function.
// -----------------------------------------------------------------------------
//
void CVoice::CompleteNotifyStopInDTMFString
    (
    TInt aResult,
    CMmDataPackage* aDataPackage
    )
    {
    _LIT8(KResult, "CompleteNotifyStopInDTMFString result: %d");
    iVoiceLog->Log((TDesC8)KResult, aResult);

    if ( ETestCaseVoice050 == iTestCaseNumber )
        {
        TBuf<256> DTMFStringAfterW;
        aDataPackage->UnPackData( DTMFStringAfterW );
        if ( DTMFStringAfterW != iRefDTMFStringToContinue )
            {
            aResult = KErrGeneral;
            }
        }
    Signal( aResult);
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteNotifyDTMFEvent
// CompleteNotifyDTMFEvent method function.
// -----------------------------------------------------------------------------
//
void CVoice::CompleteNotifyDTMFEvent
    (
    TInt aResult,
    CMmDataPackage* aDataPackage
    )
    {
    _LIT8(KResult, "CompleteNotifyDTMFEvent: %d");
    iVoiceLog->Log((TDesC8)KResult, aResult);

    RMobilePhone::TMobilePhoneDTMFEvent event;
    aDataPackage->UnPackData( event );

    if ( RMobilePhone::EStartDtmfTone == event
        && EActive == iDTMFState )
        {
        _LIT8(KStartEvent, "CompleteNotifyDTMFEvent DTMF sending started: %d");
        iVoiceLog->Log((TDesC8)KStartEvent, event);
        iDTMFState = EIdle;
        }
    else if ( RMobilePhone::EStopDtmfTone == event
        && EIdle == iDTMFState )
        {
        _LIT8(KStopEvent, "CompleteNotifyDTMFEvent DTMF sending stopped: %d");
        iVoiceLog->Log((TDesC8)KStopEvent, event);
        iDTMFState = ENotSet;
        }
    else
        {
        _LIT8(KError, "CompleteNotifyDTMFEvent: Unexpected event! %d");
        iVoiceLog->Log((TDesC8)KError, event);
        iDTMFState = ENotSet;
        aResult = KErrGeneral;
        }

    Signal( aResult);
    }

// -----------------------------------------------------------------------------
// CVoice::CallHangUpL
// CallHangUp method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::CallHangUpL(CStifItemParser& aItem)
    {
    TInt symbianHangUpCause (KErrGsmReleaseByUser);
    TInt ret( KErrNotFound );
    TInt callId(1);
    TBool autoStChangeDisable = ETrue;

    aItem.GetNextInt(callId);

    //Create package
    CCallDataPackage dataPackage;

    //Set mobilecall info
    SetMobileCallInfo();

    //Set call id and call mode
    dataPackage.SetCallIdAndMode( callId, RMobilePhone::EVoiceService );

    //Pack call parameters and mobile call info
    dataPackage.PackData(&symbianHangUpCause, &autoStChangeDisable);

    // HangUp call
    ret = HandleRequestL (EEtelCallHangUp,&dataPackage );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "HangUp successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        iTestOngoing = EHangUpTestOngoig;
        }
    else
        {
        _LIT8(KResultFail, "HangUp fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CVoice::VideoCallHangUpL
// VideoCallHangUp method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::VideoCallHangUpL(CStifItemParser& aItem)
    {
    TInt symbianHangUpCause (KErrGsmReleaseByUser);
    TInt ret( KErrNotFound );
    TInt callId(1);
    TBool autoStChangeDisable = ETrue;

    aItem.GetNextInt(callId);

    //Create package
    CCallDataPackage dataPackage;

    //Set mobilecall info
    SetMobileCallInfo();

    //Set call id and call mode
    dataPackage.SetCallIdAndMode( callId, RMobilePhone::ECircuitDataService );

    //Pack call parameters and mobile call info
    dataPackage.PackData(&symbianHangUpCause, &autoStChangeDisable);

    // HangUp call
    ret = HandleRequestL (EEtelCallHangUp,&dataPackage );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "HangUp successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        iTestOngoing = EHangUpTestOngoig;
        }
    else
        {
        _LIT8(KResultFail, "HangUp fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CVoice::SendDTMFTonesL
// SendDTMFTones method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::SendDTMFTonesL(CStifItemParser& aItem)
    {
    TInt ret (KErrNotFound);
    TPtrC aTones;

    aItem.GetNextString(aTones);


    //Create package
    CCallDataPackage dataPackage;

    //Pack DTMF parameters
    dataPackage.PackData(&aTones);

    ret = HandleRequestL (EMobilePhoneSendDTMFTones, &dataPackage);

    if ( KErrNone == ret )
            {
            _LIT8(KResultOk, "DTMF sending successful" );
            iVoiceLog->Log((TDesC8)KResultOk);
            // Expected DTMF event is active
            iDTMFState = EActive;
            }
        else
            {
            _LIT8(KResultFail, "DTMF sending fails" );
            iVoiceLog->Log((TDesC8)KResultFail);
            iDTMFState = ENotSet;
            }

    return ret;

}

// -----------------------------------------------------------------------------
// CVoice::ContinueSendDTMFTonesL
// ContinueSendDTMFTonesL method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::ContinueSendDTMFTonesL(CStifItemParser& aItem)
    {
    TInt ret( KErrNotFound );
    TPtrC aTones;

    aItem.GetNextString( aTones );

    //Create package
    CCallDataPackage dataPackage;

    //Pack DTMF parameters
    dataPackage.PackData( &aTones );

    ret = HandleRequestL( EMobilePhoneContinueDTMFStringSending, &dataPackage );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Continue DTMF sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        // Expected DTMF event is active
        iDTMFState = EActive;
        }
    else
        {
        _LIT8(KResultFail, "Continue DTMF sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        iDTMFState = ENotSet;
        }

    return ret;

}

// -----------------------------------------------------------------------------
// CVoice::SetMobileCallInfo
// SetMobileCallInfo method function.
// -----------------------------------------------------------------------------
//
void CVoice::SetMobileCallInfo()
    {
    iMobileCallInfoV1.iValid = 0;
    _LIT(name,"Call1");
    iMobileCallInfoV1.iCallName.Copy (name);
    iMobileCallInfoV1.iLineName.Copy (_L("Line1"));
    iMobileCallInfoV1.iStatus = RMobileCall::EStatusIdle;
    iMobileCallInfoV1.iStartTime.Set (2005,(TMonth)06,03,05,10,15,0);
    iMobileCallInfoV1.iDuration = 0;
    iMobileCallInfoV1.iValid |= RMobileCall::KCallStartTime | RMobileCall::KCallDuration;
    iMobileCallInfoV1.iCallId = 0;
    iMobileCallInfoV1.iValid |= RMobileCall::KCallId;
    iMobileCallInfoV1.iExitCode = 0;
    iMobileCallInfoV1.iValid |= RMobileCall::KCallExitCode;
    iMobileCallInfoV1.iEmergency = 0;
    iMobileCallInfoV1.iValid |= RMobileCall::KCallEmergency;
    iMobileCallInfoV1.iAlternatingCall = RMobilePhone::EAlternatingModeUnspecified;
    iMobileCallInfoV1.iDialledParty.iTypeOfNumber = RMobilePhone::ENationalNumber;
    iMobileCallInfoV1.iDialledParty.iNumberPlan = RMobilePhone::ENationalNumberPlan;
    iMobileCallInfoV1.iForwarded = EFalse;
    iMobileCallInfoV1.iService = RMobilePhone::EVoiceService;
    iMobileCallInfoV1.iValid |= RMobilePhone::EVoiceService;
    }

// -----------------------------------------------------------------------------
// CVoice::SetMobileCallParams
// SetMobileCallParams method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::SetMobileCallParams( CStifItemParser& aItem )
    {
    TInt ret( KErrNotFound );
    TInt speakerControl(4);
    ret = aItem.GetNextInt( speakerControl );
    if( KErrNone == ret )
        {
        TInt speakerVolume(4);
        ret = aItem.GetNextInt( speakerVolume );
        if( KErrNone == ret )
            {
            TUint interval;
            ret = aItem.GetNextInt( interval, EHex );
            if( KErrNone == ret )
                {
                TInt waitForDialTone;
                ret = aItem.GetNextInt( waitForDialTone );
                if( KErrNone == ret )
                    {
                    TInt idRestrict;
                    ret = aItem.GetNextInt( idRestrict );
                    if( KErrNone == ret )
                        {
                        TInt explicitInvoke;
                        ret = aItem.GetNextInt( explicitInvoke );
                        if( KErrNone == ret )
                            {
                            TPtrC cugIndex;
                            ret = aItem.GetNextString( cugIndex );
                            if( KErrNone == ret )
                                {
                                TInt suppressOA;
                                ret = aItem.GetNextInt( suppressOA );
                                if( KErrNone == ret )
                                    {
                                    TInt suppressPrefCug;
                                    ret = aItem.GetNextInt( suppressPrefCug );
                                    if( KErrNone == ret )
                                        {
                                        TInt autoRedial;
                                        ret = aItem.GetNextInt( autoRedial );
                                        if( KErrNone == ret )
                                            {
                                            TPtrC telNum;
                                            ret = aItem.GetNextString( telNum );
                                            if( KErrNone == ret )
                                                {
                                                // SpeakerControl
                                                iCallParams.iSpeakerControl= ( RCall::TMonitorSpeakerControl )speakerControl;

                                                // SpeakerVolume
                                                iCallParams.iSpeakerVolume = ( RCall::TMonitorSpeakerVolume )speakerVolume;

                                                // Interval
                                                iCallParams.iInterval = interval;

                                                // WaitForDialTone
                                                iCallParams.iWaitForDialTone = ( RCall::TWaitForDialTone )waitForDialTone;

                                                // IdRestrict
                                                iCallParams.iIdRestrict = ( RMobileCall::TMobileCallIdRestriction )idRestrict;

                                                // ExplicitInvoke
                                                iCallParams.iCug.iExplicitInvoke = explicitInvoke;

                                                // CugIndex
                                                TLex cugLex;
                                                TInt cugInt(0);
                                                cugLex = cugIndex;
                                                cugLex.Val(cugInt);

                                                iCallParams.iCug.iCugIndex = cugInt;

                                                // SuppressOA
                                                iCallParams.iCug.iSuppressOA = suppressOA;

                                                // SuppressPrefCug
                                                iCallParams.iCug.iSuppressPrefCug = suppressPrefCug;

                                                // AutoRedial
                                                iCallParams.iAutoRedial = autoRedial;

                                                //TelNumber
                                                iMobileCallInfoV1.iDialledParty.iTelNumber = telNum;

                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::TestIncomingCall
// Test incoming call
// -----------------------------------------------------------------------------
//
TInt CVoice::TestIncomingCall(CStifItemParser& aItem)
    {
    _LIT8(KIncomingCall, "Test IncomingCall   --->   make incoming call to the test phone" );
    iVoiceLog->Log((TDesC8)KIncomingCall );

    aItem.GetNextInt( iAnswerIncomingCall );

    //set flag on indicating that there is an incoming call test ongoing
    iTestOngoing = EIncomingCallTestOngoing;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNWE::NotifyModeChange
// Complete notify incoming call
// -----------------------------------------------------------------------------
//
void CVoice::CompleteNotifyIncomingCall
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8(KIncomingCall, "CompleteIncomingCall" );
    iVoiceLog->Log((TDesC8)KIncomingCall );

    if( 0 == aResult )
        {
        RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );
        CCallDataPackage* callDataPackage =
        reinterpret_cast<CCallDataPackage*>(aDataPackage);
        callDataPackage->GetCallIdAndMode( iCallId, callMode );
        iVoiceLog->Log((TDesC8)KLine );
        _LIT8(KText, "CallId %d" );
        iVoiceLog->Log( (TDesC8)KText, iCallId );
        _LIT8(KMode, "CallMode %d" );
        iVoiceLog->Log( (TDesC8)KMode, callMode );
        iVoiceLog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8(KFail, "NotifyModeChange Failed" );
        iVoiceLog->Log((TDesC8)KFail);
        }
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteCallStatusChange
// Complete call status change
// -----------------------------------------------------------------------------
//
void CVoice::CompleteCallStatusChange
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KStatus, "CompleteCallStatusChange" );
    iVoiceLog->Log( (TDesC8)KStatus );

    RMobileCall::TMobileCallStatus* callStatus;
    aDataPackage->UnPackData( &callStatus );

    TInt callId;
    RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );
    CCallDataPackage* callDataPackage =
    reinterpret_cast<CCallDataPackage*>(aDataPackage);
    callDataPackage->GetCallIdAndMode( callId, callMode );

    _LIT8( KCallId, "Call Id: %d" );
    iVoiceLog->Log( (TDesC8)KCallId, callId );

    if ( !iHandleSpecifiedRequests )
        {
        switch(*callStatus)
            {
            case RMobileCall::EStatusDialling:
                {
                _LIT8( KDialling, "Call Status: Dialling" );
                iVoiceLog->Log( (TDesC8)KDialling );
                if ( ECallWithUUSTestOngoing == iTestOngoing )
                    {
                    _LIT8( KUUSOk, "MO UUS call establishing started ok" );
                    iVoiceLog->Log( (TDesC8)KUUSOk );
                    // MO UUS call is simulated only to EStatusDialling,
                    // because rest of call establishment doesn't depend on UUS
                    iTestOngoing = ENoTestOngoing;
                    Signal( aResult );
                    }
                break;
                }
            case RMobileCall::EStatusConnecting:
                {
                _LIT8( KConnecting, "Call Status: Connecting" );
                iVoiceLog->Log( (TDesC8)KConnecting );
                if ( (ECallDialTestOngoing == iTestOngoing
                    && iDontAnswerCall ) || iVerifyConnectedStatus == EConnecting )
                    {
                    iVerifyConnectedStatus = EUnknown;
                    iTestOngoing = ENoTestOngoing;
                    Signal( aResult );
                    iDontAnswerCall = EFalse;
                    }
                break;
                }
            case RMobileCall::EStatusRinging:
                {
                _LIT8( KRinging, "Call Status: Ringing" );
                iVoiceLog->Log( (TDesC8)KRinging );
                //if test ongoing answer the incoming call
                if ( EIncomingCallTestOngoing == iTestOngoing )
                    {
                    if ( iAnswerIncomingCall )
                        {
                        if ( RMobilePhone::ECircuitDataService == callMode )
                            {
                            AnswerIncomingVideoCallL();
                            }
                        else
                            {
                            AnswerIncomingCallL();
                            }
                        }
                    else
                        {
                        iTestOngoing = ENoTestOngoing;
                        Signal( aResult );
                        }
                    }
                else if ( EIncomingCallWithUUSTestOngoing == iTestOngoing )
                    {
                    _LIT8( KUUSOk, "MT UUS call establishing started ok - status ringing" );
                    iVoiceLog->Log( (TDesC8)KUUSOk );
                    }
                break;
                }
            case RMobileCall::EStatusAnswering:
                {
                _LIT8( KAnswering, "Call Status: Answering" );
                iVoiceLog->Log( (TDesC8)KAnswering );
                break;
                }
            case RMobileCall::EStatusConnected:
                {
                _LIT8( KConnected, "Call Status: Connected" );
                iVoiceLog->Log( (TDesC8)KConnected );
                //complete incoming call test if ongoing
                if ( EIncomingCallTestOngoing == iTestOngoing
                    || ECallDialTestOngoing == iTestOngoing
                    || EResumeCallTestOngoing == iTestOngoing
                    || ECallDialEmergencyCallTestOngoing == iTestOngoing )
                    {
                    iTestOngoing = ENoTestOngoing;
                    iCallId = -1;
                    Signal( aResult );
                    }
                else if ( ECreateCCTestOngoing == iTestOngoing
                    || EAddCallToCCTestOngoing == iTestOngoing
                    || ESwapCCTestOngoing == iTestOngoing
                    || EResumeCCTestOngoing == iTestOngoing
                    || EGoOneToOneTestOngoing == iTestOngoing )
                    {
                    iIndicationCounter++;
                    if ( iNumberOfCallsInCC == iIndicationCounter )
                        {
                        iIndicationCounter = 0;
                        iTestOngoing = ENoTestOngoing;
                        Signal( aResult );
                        if ( EGoOneToOneTestOngoing == iTestOngoing
                            && KErrNone == aResult )
                            {
                            iNumberOfCallsInCC--;
                            }
                        }
                    }
                break;
                }
            case RMobileCall::EStatusHold:
                {
                _LIT8( KHold, "Call Status: Hold" );
                iVoiceLog->Log( (TDesC8)KHold );
                //complete
                if ( EHoldCallTestOngoing == iTestOngoing
                    || ( 2 == callId && ESwapCallTestOngoing == iTestOngoing ) )
                    {
                    iTestOngoing = ENoTestOngoing;
                    Signal( aResult );
                    }
                else if ( EHoldCCTestOngoing == iTestOngoing
                    || ESwapCCTestOngoing == iTestOngoing
                    || EGoOneToOneTestOngoing == iTestOngoing )
                    {
                    iIndicationCounter++;
                    if ( iNumberOfCallsInCC == iIndicationCounter )
                        {
                        iIndicationCounter = 0;
                        iTestOngoing = ENoTestOngoing;
                        Signal( aResult );
                        }
                    }
                break;
                }
            case RMobileCall::EStatusDisconnecting:
                {
                _LIT8( KDisconnecting, "Call Status: Disconnecting" );
                iVoiceLog->Log( (TDesC8)KDisconnecting );
                break;
                }
            case RMobileCall::EStatusIdle:
                {
                _LIT8( KIdle, "Call Status: Idle" );
                iVoiceLog->Log( (TDesC8)KIdle );
                //complete hang up test if ongoing
                if ( EHangUpTestOngoig == iTestOngoing
                    || ERemotePartyHangUpTestOngoing == iTestOngoing
                    || ( 2 == callId &&
                    ECallTransferTestOngoing == iTestOngoing ) )
                    {
                    iTestOngoing = ENoTestOngoing;
                    Signal( aResult );
                    }
                else if ( EHangUpCCTestOngoing == iTestOngoing)
                    {
                    iIndicationCounter++;
                    if ( iNumberOfCallsInCC == iIndicationCounter )
                        {
                        iIndicationCounter = 0;
                        iTestOngoing = ENoTestOngoing;
                        Signal( aResult );
                        }
                    }
                break;
                }
            default:
                {
                _LIT8( KDefault, "Call Status: Unknown" );
                iVoiceLog->Log( (TDesC8)KDefault );
                break;
                }
            }
        }
    else
        {
        // Check parameters depending on test case
        if ( iVoiceParameterCheck &&
            ( ETestCaseVoice032 == iTestCaseNumber ||
            ETestCaseVoice033 == iTestCaseNumber ) )
            {
            if ( *callStatus != iRefCallStatus )
                {
                aResult = KErrGeneral;
                }
            }

        switch(*callStatus)
            {
            case RMobileCall::EStatusDialling:
                {
                _LIT8( KDialling, "Call Status: Dialling" );
                iVoiceLog->Log( (TDesC8)KDialling );
                Signal( aResult );
                break;
                }
            case RMobileCall::EStatusConnecting:
                {
                _LIT8( KConnecting, "Call Status: Connecting" );
                iVoiceLog->Log( (TDesC8)KConnecting );
                Signal( aResult );
                break;
                }
            case RMobileCall::EStatusRinging:
                {
                _LIT8( KRinging, "Call Status: Ringing" );
                iVoiceLog->Log( (TDesC8)KRinging );
                if ( EIncomingCallTestOngoing == iTestOngoing )
                    {
                    iTestOngoing = ENoTestOngoing;
                    if ( iAnswerIncomingCall )
                        {
                        if ( RMobilePhone::ECircuitDataService == callMode )
                            {
                            AnswerIncomingVideoCallL();
                            }
                        else
                            {
                            AnswerIncomingCallL();
                            }
                        }
                    }
                Signal( aResult );
                break;
                }
            case RMobileCall::EStatusAnswering:
                {
                _LIT8( KAnswering, "Call Status: Answering" );
                iVoiceLog->Log( (TDesC8)KAnswering );
                Signal( aResult );
                break;
                }
            case RMobileCall::EStatusConnected:
                {
                _LIT8( KConnected, "Call Status: Connected" );
                iVoiceLog->Log( (TDesC8)KConnected );
                Signal( aResult );
                break;
                }
            case RMobileCall::EStatusHold:
                {
                _LIT8( KHold, "Call Status: Hold" );
                iVoiceLog->Log( (TDesC8)KHold );
                Signal( aResult );
                break;
                }
            case RMobileCall::EStatusDisconnecting:
                {
                _LIT8( KDisconnecting, "Call Status: Disconnecting" );
                iVoiceLog->Log( (TDesC8)KDisconnecting );
                Signal( aResult );
                break;
                }
            case RMobileCall::EStatusIdle:
                {
                _LIT8( KIdle, "Call Status: Idle" );
                iVoiceLog->Log( (TDesC8)KIdle );
                Signal( aResult );
                break;
                }
            default:
                {
                _LIT8( KDefault, "Call Status: Unknown" );
                iVoiceLog->Log( (TDesC8)KDefault );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVoice::AnswerIncomingCallL
// Answer incoming call
// -----------------------------------------------------------------------------
//
TInt CVoice::AnswerIncomingCallL()
    {
    _LIT8(KStatus, "AnswerIncomingCall" );
    iVoiceLog->Log((TDesC8)KStatus );

    TInt ret (KErrNotFound);
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( iCallId, mode );

    ret = HandleRequestL (EEtelCallAnswer, &package);

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "AnswerIncomingCallL sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "AnswerIncomingCallL sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        Signal ( ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::AnswerIncomingDataCallL
// Answer incoming data call
// -----------------------------------------------------------------------------
//
TInt CVoice::AnswerIncomingVideoCallL()
    {
    _LIT8(KStatus, "AnswerIncomingDataCall" );
    iVoiceLog->Log((TDesC8)KStatus );

    TInt ret (KErrNotFound);
    RMobilePhone::TMobileService mode( RMobilePhone::ECircuitDataService );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( iCallId, mode );

    ret = HandleRequestL (EEtelCallAnswer, &package);

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "AnswerIncomingDataCallL sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "AnswerIncomingDataCallL sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        Signal ( ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteCallAnswer
// Complete Call Answer
// -----------------------------------------------------------------------------
//
void CVoice::CompleteCallAnswer(TInt aResult, CMmDataPackage* aDataPackage)
    {
    _LIT8(KCompleteCallAnswer, "CVoice::CompleteCallAnswer");
    iVoiceLog->Log((TDesC8)KCompleteCallAnswer );

    if ( iHandleSpecifiedRequests )
        {
        CCallDataPackage& callData = *static_cast<CCallDataPackage*>(aDataPackage);
        // this IPC never completed for voice calls
        TInt callId;
        RMobilePhone::TMobileService mode;
        callData.GetCallIdAndMode(callId, mode);
        if ( RMobilePhone::ECircuitDataService != mode )
            {
            aResult = KErrGeneral;
            }
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CVoice::TestHoldCallL
// Test hold call
// -----------------------------------------------------------------------------
//
TInt CVoice::TestHoldCallL(CStifItemParser& aItem)
    {
    _LIT8(KHoldCall, "Test HoldCall" );
    iVoiceLog->Log((TDesC8)KHoldCall );

    TInt ret (KErrNotFound);
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    TInt callId;
    aItem.GetNextInt( callId );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( callId, mode );

    ret = HandleRequestL (EMobileCallHold, &package);

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "TestHoldCallL sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        //set flag on indicating that there is an hold call test ongoing
        iTestOngoing = EHoldCallTestOngoing;
        }
    else
        {
        _LIT8(KResultFail, "TestHoldCallL sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        Signal ( ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::RemotePartyHangUp
// Test incoming call
// -----------------------------------------------------------------------------
//
TInt CVoice::RemotePartyHangUp(CStifItemParser& /*aItem*/)
    {
    _LIT8(KRemotePartyHangUp, "Test RemotePartyHangUp  ---> HangUp the call" );
    iVoiceLog->Log((TDesC8)KRemotePartyHangUp );
    //set flag on indicating that there is an incoming call test ongoing
    iTestOngoing = ERemotePartyHangUpTestOngoing;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVoice::TestResumeCallL
// Test Resume call
// -----------------------------------------------------------------------------
//
TInt CVoice::TestResumeCallL(CStifItemParser& aItem)
    {
    _LIT8(KResumeCall, "Test ResumeCall" );
    iVoiceLog->Log((TDesC8)KResumeCall );

    TInt ret (KErrNotFound);
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    TInt callId;
    aItem.GetNextInt( callId );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( callId, mode );

    ret = HandleRequestL( EMobileCallResume, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "TestResumeCallL sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        //set flag on indicating that there is an resume call test ongoing
        iTestOngoing = EResumeCallTestOngoing;
        }
    else
        {
        _LIT8(KResultFail, "TestResumeCallL sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        Signal ( ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::TestSwapCallsL
// Test Swap active and hold calls
// -----------------------------------------------------------------------------
//
TInt CVoice::TestSwapCallsL(CStifItemParser& aItem)
    {
    _LIT8(KSwapCalls, "Test SwapCalls" );
    iVoiceLog->Log((TDesC8)KSwapCalls );

    TInt ret( KErrNotFound );
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    TInt callId;
    aItem.GetNextInt( callId );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( callId, mode );

    ret = HandleRequestL( EMobileCallSwap, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "TestSwapCallsL sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        //set flag on indicating that there is an swap calls test ongoing
        iTestOngoing = ESwapCallTestOngoing;
        }
    else
        {
        _LIT8(KResultFail, "TestSwapCallsL sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        Signal ( ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::TestCallTransferL
// Test call transfering
// -----------------------------------------------------------------------------
//
TInt CVoice::TestCallTransferL(CStifItemParser& aItem)
    {
    _LIT8(KCallTransfer, "Test CallTransfer" );
    iVoiceLog->Log((TDesC8)KCallTransfer );

    TInt ret( KErrNotFound );
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    TInt callId;
    aItem.GetNextInt( callId );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( callId, mode );

    ret = HandleRequestL( EMobileCallTransfer, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "TestCallTransferL sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        //set flag on indicating that there is an transfer calls test ongoing
        iTestOngoing = ECallTransferTestOngoing;
        }
    else
        {
        _LIT8(KResultFail, "TestCallTransferL sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        Signal ( ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::ClearBlacklistL
//
// -----------------------------------------------------------------------------
//
TInt CVoice::ClearBlacklistL(CStifItemParser& /*aItem*/)
    {
    _LIT8(KCallClearBlacklist, "Test ClearBlacklist" );
    iVoiceLog->Log((TDesC8)KCallClearBlacklist );

    TInt ret( KErrNotFound );
    RMobilePhone::TMobileService dummyMode( RMobilePhone::EVoiceService );
    TInt dummyCallId( 0 );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode (both not used)
    package.SetCallIdAndMode( dummyCallId, dummyMode );

    ret = HandleRequestL( ECustomClearCallBlackListIPC, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "ClearBlacklistL sending successful" );
        iVoiceLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "ClearBlacklistL sending fails" );
        iVoiceLog->Log((TDesC8)KResultFail);
        Signal ( ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CVoice::CompleteClearBlacklist
// CompleteClearBlacklist method function.
// -----------------------------------------------------------------------------
//
void CVoice::CompleteClearBlacklist
        (
        TInt aResult //Completion result value
        )
    {
    _LIT8(KResult, "CompleteClearBlacklist result: %d");
    iVoiceLog->Log((TDesC8)KResult, aResult);

    Signal ( aResult);
    }


// -----------------------------------------------------------------------------
// CVoice::VerifyRemoteAlertingToneStatus
// -----------------------------------------------------------------------------
//
TInt CVoice::VerifyRemoteAlertingToneStatus( CStifItemParser& aItem )
    {
    iVoiceLog->Log(_L8("CVoice::VerifyRemoteAlertingToneStatus test started"));

    TPtrC connectionVerify;
    aItem.GetNextString( connectionVerify );

    TBool supported( ETrue );

    if ( connectionVerify == _L("Connecting") )
        {
        iVerifyConnectedStatus = EConnecting;
        _LIT8( KVerifyModeEConnecting, "CVoice::Verifying connection status CONNECTING started");
        iVoiceLog->Log( (TDesC8)KVerifyModeEConnecting );
        }
    else if ( connectionVerify == _L("None") )
        {
        iVerifyConnectedStatus = EUnknown;
        _LIT8( KVerifyModeEUnknown, "CVoice::Verifying remote alerting tone generation");
        iVoiceLog->Log( (TDesC8)KVerifyModeEUnknown );
        }
    else if ( connectionVerify == _L("LocalTone") )
        {
        iVerifyConnectedStatus = EConnecting;
        _LIT8( KVerifyModeLocalTone, "CVoice::Verifying LocalTone alerting tone generation");
        iVoiceLog->Log( (TDesC8)KVerifyModeLocalTone );
        }
    else if ( connectionVerify == _L("RemoteTone") )
        {
        iVerifyConnectedStatus = EUnknown;
        _LIT8( KVerifyModeRemoteTone, "CVoice::Verifying RemoteTone alerting tone generation");
        iVoiceLog->Log( (TDesC8)KVerifyModeRemoteTone );
        }
    else
        {
        _LIT8( KVerifyMode, "CVoice::Connection verify mode not supported" );
        iVoiceLog->Log( (TDesC8)KVerifyMode );
        supported = EFalse;
        }

    if ( supported )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotSupported;
        }
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteVerifyRemoteAlertingToneStatus
// -----------------------------------------------------------------------------
//
void CVoice::CompleteVerifyRemoteAlertingToneStatus
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteVerifyRemoteAlertingToneStatus result: %d" );
    iVoiceLog->Log( (TDesC8)KResult, aResult );

    TBool playRemoteAlertToneLocally;

    if ( iHandleSpecifiedRequests )
        {
        aDataPackage->UnPackData( playRemoteAlertToneLocally );

        if ( playRemoteAlertToneLocally )
            {
            iVoiceLog->Log( _L8("Remote alerting tone is generated locally") );
            if ( EConnecting != iVerifyConnectedStatus )
                {
                aResult = KErrGeneral;
                }
            }
        else
            {
            iVoiceLog->Log( _L8("Remote alerting tone is not generated locally") );
            if ( EUnknown != iVerifyConnectedStatus )
                {
                aResult = KErrGeneral;
                }
            }

        Signal( aResult );
        }
    else
        {
        iVoiceLog->Log( _L8("No Signal sent / iHandleSpecifiedRequests is not set") );
        }
    }

// -----------------------------------------------------------------------------
// CVoice::DialEmergencyCall
// Call DialEmergencyCall method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::DialEmergencyCall( CStifItemParser& aItem )
    {
    _LIT8 (KCallDial, "Voice::DialEmergencyCall" );
    iVoiceLog->Log((TDesC8)KCallDial);

    TInt ret( KErrNotFound );
    TPtrC aEmergTelNumber;

    ret = aItem.GetNextString( aEmergTelNumber );

    if ( KErrNone == ret )
        {

        CMmDataPackage dataPackage;

        dataPackage.PackData( &aEmergTelNumber );

        ret = HandleRequestL( EMobileCallDialEmergencyCall, &dataPackage );

        if ( KErrNone == ret )
            {
            _LIT8(KCallOk, "EmergencyCall req send successful" );
            iVoiceLog->Log( (TDesC8)KCallOk );
            iTestOngoing = ECallDialEmergencyCallTestOngoing;
            }
        else
            {
            _LIT8(KCallFailed, "EmergencyCall req send fails: %d" );
            iVoiceLog->Log((TDesC8)KCallFailed );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::ActivateUUSL
// Function for testing of UUS activation
// -----------------------------------------------------------------------------
//
TInt CVoice::ActivateUUSL( CStifItemParser& aItem )
    {
    _LIT8 ( KActivateUUS, "Voice::ActivateUUSL" );
    iVoiceLog->Log( (TDesC8)KActivateUUS );

    TInt serviceReq;
    TPtrC UUI;
    TInt ret( KErrNotFound );

    ret = aItem.GetNextInt( serviceReq );

    if ( KErrNone == ret )
        {
        ret = aItem.GetNextString( UUI );
        }

    if ( KErrNone == ret )
        {
        // fill out TMobileCallUUSRequestV1
        RMobileCall::TMobileCallUUSRequestV1 mobileCallUUSRequestV1;

        if ( 1 == serviceReq )
            {
            mobileCallUUSRequestV1.iServiceReq = RMobileCall::KUUS1Implicit;
            }

        else
            {
            mobileCallUUSRequestV1.iServiceReq = serviceReq;
            }

        mobileCallUUSRequestV1.iUUI = UUI;

        CMmDataPackage dataPackage;

        dataPackage.PackData( &mobileCallUUSRequestV1 );

        ret = HandleRequestL( EMobileCallActivateUUS, &dataPackage );

        if ( KErrNone == ret )
            {
            _LIT8( KUUSOk, "UUS Activation successfull" );
            iVoiceLog->Log( (TDesC8)KUUSOk );
            iTestOngoing = ECallWithUUSTestOngoing;
            }
        else
            {
            _LIT8( KUUSFailed, "UUS Activation failed" );
            iVoiceLog->Log( (TDesC8)KUUSFailed );
            }

        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CVoice::TestIncomingCallwithUUS
// Test incoming call
// -----------------------------------------------------------------------------
//
TInt CVoice::IncomingCallWithUUS(CStifItemParser& aItem )
    {
    _LIT8( KIncomingCall, "Voice:IncomingCallwithUUS" );
    iVoiceLog->Log((TDesC8)KIncomingCall );

    TPtrC string;

    if ( iHandleSpecifiedRequests )
        {
        aItem.GetNextString( string );
        iSavedUUSNameForComparison.Zero();
        iSavedUUSNameForComparison.Append( string );
        }
    // set flag on indicating that there is a test ongoing
    iTestOngoing = EIncomingCallWithUUSTestOngoing;
    // set completion state to waiting.
    // See CVoice::CheckIncomingCallUUSCompletions for more information
    iUUSCompletionState = EWaiting;

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CVoice::CheckIncomingCallUUSCompletions
// Checks, that incoming call with UUS triggers all needed completions in
// defined order:
// EEtelLineNotifyIncomingCall
// EMobileCallReceiveUUI
// EMobileCallGetMobileCallInfo
// EMobileCallNotifyMobileCallStatusChange
//
// Test will be signaled to be succesfully ended only if completions mentioned
// above are received in mentioned order.
//
// (other items were commented in header)
// -----------------------------------------------------------------------------
//
void CVoice::CheckIncomingCallUUSCompletions( TInt aResult,
    CMmDataPackage* aDataPackage, TInt aIpc)
    {

    if ( ENotifyMobileCallStatusChangeCompleted != iUUSCompletionState
        && KErrNone == aResult )
        {
         switch ( aIpc )
            {
            case EEtelLineNotifyIncomingCall:
                {
                _LIT8( KCompleteReceived, "Voice:CheckIncomingCallUUSCompletions - EEtelLineNotifyIncomingCall received" );
                iVoiceLog->Log( (TDesC8) KCompleteReceived );

                if ( EWaiting == iUUSCompletionState )
                    {
                    iUUSCompletionState = ELineNotifyIncomingCallCompleted;
                    }
                else
                    {
                    _LIT8( KWrongCompletion, "Voice:CheckIncomingCallUUSCompletions - unwaited completion" );
                    iVoiceLog->Log( (TDesC8) KWrongCompletion );

                    iTestOngoing = ENoTestOngoing;
                    Signal( KErrGeneral );
                    }

                break;
                }

            case EMobileCallReceiveUUI:
                {
                _LIT8( KCompleteReceived, "Voice:CheckIncomingCallUUSCompletions - EMobileCallReceiveUUI received" );
                iVoiceLog->Log( (TDesC8) KCompleteReceived );

                if ( ( ELineNotifyIncomingCallCompleted == iUUSCompletionState ) ||
                     ( iHandleSpecifiedRequests ) )
                    {
                    RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );
                    TInt callId = 0;

                    iUUSCompletionState = EReceiveUUICompleted;

                    // Take data and call id from completion.
                    CCallDataPackage* callDataPackage =
                        reinterpret_cast<CCallDataPackage*>(aDataPackage);
                    callDataPackage->GetCallIdAndMode( callId, callMode );

                    _LIT8( KText, "CallId in completion after UUS: %d" );
                    iVoiceLog->Log( (TDesC8) KText, callId );

                    RMobileCall::TMobileCallUUI UUIData;

                    callDataPackage->UnPackData( UUIData );

                    _LIT8( KText2, "UUS data in completion (next row):" );
                    iVoiceLog->Log( (TDesC8) KText2 );
                    iVoiceLog->Log( UUIData );

                    if ( iHandleSpecifiedRequests )
                        {
                        if ( ( 0 != iSavedUUSNameForComparison.Compare ( UUIData ) ) )
                            {
                            aResult = KErrGeneral;
                            }
                        Signal( aResult );
                        }

                    }
                else
                    {
                    _LIT8( KWrongCompletion, "Voice:CheckIncomingCallUUSCompletions - unwaited completion" );
                    iVoiceLog->Log( (TDesC8)KWrongCompletion );

                    iTestOngoing = ENoTestOngoing;
                    Signal( KErrGeneral );
                    }

                break;
                }

            case EMobileCallGetMobileCallInfo:
                {
                _LIT8( KCompleteReceived, "Voice:CheckIncomingCallUUSCompletions - EMobileCallGetMobileCallInfo received" );
                iVoiceLog->Log( (TDesC8) KCompleteReceived );

                if ( EReceiveUUICompleted == iUUSCompletionState )
                    {
                    iUUSCompletionState = EGetMobileCallInfoCompleted;
                    }
                else
                    {
                    _LIT8( KWrongCompletion, "Voice:CheckIncomingCallUUSCompletions - unwaited completion" );
                    iVoiceLog->Log( (TDesC8) KWrongCompletion );

                    iTestOngoing = ENoTestOngoing;
                    Signal( KErrGeneral );
                    }

                break;
                }

            case EMobileCallNotifyMobileCallStatusChange:
                {
                _LIT8( KCompleteReceived, "Voice:CheckIncomingCallUUSCompletions - EMobileCallNotifyMobileCallStatusChange received" );
                iVoiceLog->Log( (TDesC8) KCompleteReceived );

                if ( EGetMobileCallInfoCompleted == iUUSCompletionState )
                    {
                    iUUSCompletionState = ENotifyMobileCallStatusChangeCompleted;

                    _LIT8( KAllCompletionsReceived, "Voice:CheckIncomingCallUUSCompletions - all needed completions received" );
                    iVoiceLog->Log( (TDesC8) KAllCompletionsReceived );

                    // Signal test successfully ended.
                    iTestOngoing = ENoTestOngoing;
                    Signal( KErrNone );
                    }
                else
                    {
                    _LIT8( KWrongCompletion, "Voice:CheckIncomingCallUUSCompletions - unwaited completion" );
                    iVoiceLog->Log( (TDesC8) KWrongCompletion );

                    iTestOngoing = ENoTestOngoing;
                    Signal( KErrGeneral );
                    }

                break;
                }

            default:
                {
                // For reason or other function has been called with unknown IPC.
                _LIT8( KText, "Voice:CheckIncomingCallUUSCompletions - switch IPC - unknown IPC" );
                iVoiceLog->Log( (TDesC8) KText );

                iTestOngoing = ENoTestOngoing;
                    Signal( KErrGeneral );

                break;
                }

            } // switch
        } // if (ENotifyMobileCallStatusChangeCompleted ! = iUUSCompletionState && KErrNone == aResult)

    // no else

    }

// -----------------------------------------------------------------------------
// CVoice::CompleteCallTermination
// -----------------------------------------------------------------------------
//
void CVoice::CompleteCallTermination( TInt aResult )
    {

    _LIT8( KResult, "CompleteCallTermination result: %d" );
    iVoiceLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CVoice::UpdateLifeTimer
// Request Call Life Timer update wiht a new value.
// -----------------------------------------------------------------------------
//
TInt CVoice::UpdateLifeTimer( CStifItemParser& aItem )
    {
    TInt ret( KErrNone );
    TInt timerValue( 0 );

    _LIT8(KUpdateLifeTimer, "CVoice::UpdateLifeTimer");
    iVoiceLog->Log((TDesC8)KUpdateLifeTimer);

    aItem.GetNextInt( timerValue );

    CMmDataPackage dataPackage;

    TTimeIntervalSeconds time ( timerValue );

    dataPackage.PackData( &time );

    ret = HandleRequestL( EMmTsyUpdateLifeTimeIPC, &dataPackage );

    if ( KErrNone != ret )
        {
        Signal ( ret );
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::GetLifeTimer
// Request Get Call Life Timer value.
// -----------------------------------------------------------------------------
//
TInt CVoice::GetLifeTimer( CStifItemParser& aItem )
    {
    TInt ret( KErrNone );
    _LIT8(KGetLifeTimer, "CVoice::GetLifeTimer");
    iVoiceLog->Log((TDesC8)KGetLifeTimer);

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
// CVoice::CompleteCallTermination
// -----------------------------------------------------------------------------
//
void CVoice::CompleteGetLifeTimer(
         TInt aResult,
         CMmDataPackage* aDataPackage )
    {

    RMmCustomAPI::TLifeTimeData timeInfo;

    _LIT8( KCompleteGetLifeTimer, "CVoice::CompleteGetLifeTimer: %d" );
    iVoiceLog->Log((TDesC8)KCompleteGetLifeTimer, aResult );

    aDataPackage->UnPackData( timeInfo );

    if ( ( ( iLifeTimerForComparison.iHours != timeInfo.iHours ) ||
           ( iLifeTimerForComparison.iMinutes != timeInfo.iMinutes ) ) &&
         ( KErrNone == aResult ) )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
