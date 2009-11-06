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
* Description:   Specific class for Conference call tests
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "voice.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CVoice::CreateConference
// CreateConference method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::CreateConference( CStifItemParser& /*aItem*/ )
    {
    _LIT8 (KCC, "Voice::CreateConference" );
    iVoiceLog->Log((TDesC8)KCC);

    TInt ret( KErrNotFound );
    ret = HandleRequestL( EMobileConferenceCallCreateConference );
    if ( KErrNone == ret )
        {
        _LIT8(KCallOk, " CreateConference call successful" );
        iVoiceLog->Log((TDesC8)KCallOk );
        iTestOngoing = ECreateCCTestOngoing;
        iNumberOfCallsInCC = 2;
        }
    else
        {
        _LIT8(KCallFailed, " CreateConference call fails: %d" );
        iVoiceLog->Log((TDesC8)KCallFailed );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::HangUpCC
// HangUpCC method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::HangUpCC( CStifItemParser& /*aItem*/ )
    {
    _LIT8 (KCC, "Voice::HangUpCC" );
    iVoiceLog->Log((TDesC8)KCC);

    TInt ret( KErrNotFound );
    ret = HandleRequestL( EMobileConferenceCallHangUp );
    if ( KErrNone == ret )
        {
        _LIT8(KCallOk, " HangUpCC call successful" );
        iVoiceLog->Log((TDesC8)KCallOk );
        iTestOngoing = EHangUpCCTestOngoing;
        }
    else
        {
        _LIT8(KCallFailed, " HangUpCC call fails: %d" );
        iVoiceLog->Log((TDesC8)KCallFailed );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::AddCallToCC
// AddCallToCC method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::AddCallToCC( CStifItemParser& aItem )
    {
    _LIT8 (KCC, "Voice::AddCallToCC" );
    iVoiceLog->Log((TDesC8)KCC);

    TInt ret (KErrNotFound);
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    TInt callId;
    aItem.GetNextInt( callId );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( callId, mode );

    ret = HandleRequestL( EMobileConferenceCallAddCall, &package );
    if ( KErrNone == ret )
        {
        _LIT8(KCallOk, " AddCallToCC call successful" );
        iVoiceLog->Log((TDesC8)KCallOk );
        iTestOngoing = EAddCallToCCTestOngoing;
        iNumberOfCallsInCC++;
        }
    else
        {
        _LIT8(KCallFailed, " AddCallToCC call fails: %d" );
        iVoiceLog->Log((TDesC8)KCallFailed );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::HoldConference
// CreateConference method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::HoldConference( CStifItemParser& aItem )
    {
    _LIT8 (KCC, "Voice::HoldConference" );
    iVoiceLog->Log((TDesC8)KCC);

    TInt ret (KErrNotFound);
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    TInt callId;
    aItem.GetNextInt( callId );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( callId, mode );

    ret = HandleRequestL( EMobileCallHold, &package );
    if ( KErrNone == ret )
        {
        _LIT8(KCallOk, " HoldConference call successful" );
        iVoiceLog->Log((TDesC8)KCallOk );
        iTestOngoing = EHoldCCTestOngoing;
        }
    else
        {
        _LIT8(KCallFailed, " HoldConference call fails: %d" );
        iVoiceLog->Log((TDesC8)KCallFailed );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::ResumeConference
// ResumeConference method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::ResumeConference( CStifItemParser& aItem )
    {
    _LIT8 (KCC, "Voice::ResumeConference" );
    iVoiceLog->Log((TDesC8)KCC);

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
        _LIT8(KCallOk, " ResumeConference call successful" );
        iVoiceLog->Log((TDesC8)KCallOk );
        iTestOngoing = EResumeCCTestOngoing;
        }
    else
        {
        _LIT8(KCallFailed, "ResumeConference call fails: %d" );
        iVoiceLog->Log((TDesC8)KCallFailed );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::SwapConference
// CreateConference method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::SwapConference( CStifItemParser& /*aItem*/ )
    {
    _LIT8 (KCC, "Voice::SwapConference" );
    iVoiceLog->Log((TDesC8)KCC);

    TInt ret (KErrNotFound);

    ret = HandleRequestL( EMobileConferenceCallSwap );
    if ( KErrNone == ret )
        {
        _LIT8(KCallOk, " SwapConference call successful" );
        iVoiceLog->Log((TDesC8)KCallOk );
        iTestOngoing = ESwapCCTestOngoing;
        }
    else
        {
        _LIT8(KCallFailed, " SwapConference call fails: %d" );
        iVoiceLog->Log((TDesC8)KCallFailed );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::GoOneToOne
// CreateConference method function.
// -----------------------------------------------------------------------------
//
TInt CVoice::GoOneToOne( CStifItemParser& aItem )
    {
    _LIT8 (KCC, "Voice::GoOneToOne" );
    iVoiceLog->Log((TDesC8)KCC);

    TInt ret (KErrNotFound);
    RMobilePhone::TMobileService mode( RMobilePhone::EVoiceService );

    TInt callId;
    aItem.GetNextInt( callId );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( callId, mode );

    ret = HandleRequestL( EMobileCallGoOneToOne, &package );
    if ( KErrNone == ret )
        {
        _LIT8(KCallOk, " GoOneToOne call successful" );
        iVoiceLog->Log((TDesC8)KCallOk );
        iTestOngoing = EGoOneToOneTestOngoing;
        }
    else
        {
        _LIT8(KCallFailed, " GoOneToOne call fails: %d" );
        iVoiceLog->Log((TDesC8)KCallFailed );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVoice::CompleteMobileCallGetMobileCallInfo
// Completes
// -----------------------------------------------------------------------------
//
void CVoice::CompleteMobileCallGetMobileCallInfo(
    TInt aResult,
    const CCallDataPackage* aCallData )
    {
    TInt callId( 0 );
    RMobilePhone::TMobileService callMode( RMobilePhone::EServiceUnspecified );

    aCallData->GetCallIdAndMode( callId, callMode );

    _LIT8(KId, "CVoice::CompleteMobileCallGetMobileCallInfo - callId: %d" );
    _LIT8(KMode, "CVoice::CompleteMobileCallGetMobileCallInfo - callMode: %d" );
    iVoiceLog->Log((TDesC8)KId, callId );
    iVoiceLog->Log((TDesC8)KMode, callMode );

    // Parameter checking
    if ( iVoiceParameterCheck &&
        ( ETestCaseVoice032 == iTestCaseNumber ||
          ETestCaseVoice033 == iTestCaseNumber ) )
        {
        if ( callId != iRefCallId || callMode != iRefCallMode )
            {
            aResult = KErrGeneral;
            }
        }

    Signal( aResult);
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None
//  End of File
