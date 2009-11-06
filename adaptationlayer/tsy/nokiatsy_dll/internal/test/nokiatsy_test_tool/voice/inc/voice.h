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



#ifndef VOICE_H
#define VOICE_H

//  INCLUDES
#include "core.h"

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <ctsy/serviceapi/gsmerror.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#else
#include <ctsy/rmmcustomapi.h>
#endif

// MACROS
// Logging path
_LIT( KVoiceLogPath, "\\logs\\testframework\\Voice\\" );
// Log file
_LIT( KVoiceLogFile, "Voice.txt" );

_LIT(KIscControlTestCaseFileCVoice, "nokiatsy_voice.xml");

//CONSTANTS

enum TOngoingTest
    {
    ENoTestOngoing,
    ECallDialTestOngoing,
    EIncomingCallTestOngoing,
    EHoldCallTestOngoing,
    EHangUpTestOngoig,
    ERemotePartyHangUpTestOngoing,
    EResumeCallTestOngoing,
    ESwapCallTestOngoing,
    ECallTransferTestOngoing,
    ECreateCCTestOngoing,
    EHangUpCCTestOngoing,
    EHoldCCTestOngoing,
    EResumeCCTestOngoing,
    EAddCallToCCTestOngoing,
    ESwapCCTestOngoing,
    EGoOneToOneTestOngoing,
    ECallDialEmergencyCallTestOngoing,
    ECallWithUUSTestOngoing,
    EIncomingCallWithUUSTestOngoing
    };

enum TConnectionState
    {
    EConnecting,
    EUnknown
    };

enum TUUSCompletionState
    {
    EWaiting,
    ELineNotifyIncomingCallCompleted,
    EReceiveUUICompleted,
    EGetMobileCallInfoCompleted,
    ENotifyMobileCallStatusChangeCompleted
    };

// Test case numbers can be used for defining parameter checks for
// certain test cases. Can be set by function SetSmsParameterCheck.
enum TTestCase
    {
    ETestCaseNotDefined = 0,
    ETestCaseVoice032 = 32,
    ETestCaseVoice033 = 33,
    ETestCaseVoice050 = 50
    };

enum TDTMFState
    {
    ENotSet,
    EActive,
    EIdle
    };

// time related constants
const TUint16 KSecsInHour = 3600;
const TUint8 KMinsInHour = 60;

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;
class CVoice;

// CLASS DECLARATION
class CVoice : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVoice* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVoice();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

         /**
        * Completes a request or notification via the specific Tsy object,
        * based on IPC number and data package
        * @since Series60_ver 2.8
        * @param aIpc: IPC number
        *        aDataPackage: Data package
        *        aResult: result
        * @return None
        */
        IMPORT_C virtual void RouteCompletion( TInt aIpc,
            CMmDataPackage* aDataPackage, TInt aResult );


    protected:  // New functions

        /**
        * Set voice parameter check ON/OFF and test case number
        * @since Series60_ver 5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetVoiceParameterCheck( CStifItemParser& aItem );

        /**
        * Set SMS reference parameters for parameter checking
        * @since Series60_ver 5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetVoiceReferenceParameters( CStifItemParser& aItem );

        /**
        * Call Dial
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CallDialL( CStifItemParser& aItem );

        /**
        * Video Call Dial
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt VideoCallDialL( CStifItemParser& aItem );

        /**
        * Complete Call Dial
        * @param aResult
        * @param aDataPackage
        * @return None
        */
        void CompleteCallDial(TInt aResult, CMmDataPackage* aDataPackage);

        /**
        * Call HangUp
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CallHangUpL( CStifItemParser& aItem );

        /**
        * Video Call HangUp
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt VideoCallHangUpL( CStifItemParser& aItem );

        /**
        * SendDTMFTones
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendDTMFTonesL (CStifItemParser& aItem);

        /**
         * ContinueSendDTMFTonesL
         * @param aItem Script line containing method name and parameters
         * @return Symbian OS error code.
         */
         TInt ContinueSendDTMFTonesL (CStifItemParser& aItem);

        /**
        * CompleteSendDTMFTones
        * @since Series60_ver 2.8
        * @param aResult
        * @return None
        */
        void CompleteSendDTMFTones( TInt aResult );


        /**
        * CompleteNotifyStopInDTMFString
        * @param aResult
        * @param aDataPackage
        * @return None
        */
        void CompleteNotifyStopInDTMFString(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * CompleteNotifyStopInDTMFString
        * @param aResult
        * @param aDataPackage
        * @return None
        */
        void CompleteNotifyDTMFEvent(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Test IncomingCall
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt TestIncomingCall( CStifItemParser& aItem );

        /**
        * Complete Notify of IncomingCall
        * @since Series60_ver 2.8
        * @param aResult
                 aDataPackage
        * @return none.
        */
        void CompleteNotifyIncomingCall( TInt aResult,
            CMmDataPackage* aDataPackage);

        /**
        * Complete CallStatusChange
        * @since Series60_ver 2.8
        * @param aResult
                 aDataPackage
        * @return none.
        */
        void CompleteCallStatusChange( TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Test HoldCall
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt TestHoldCallL( CStifItemParser& aItem );

        /**
        * Test RemoteParty HangUp
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt RemotePartyHangUp( CStifItemParser& aItem );

        /**
        * Test ResumeCall
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt TestResumeCallL( CStifItemParser& aItem );

        /**
        * Test SwapCalls
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt TestSwapCallsL( CStifItemParser& aItem );

         /**
        * Test CallTransfer
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt TestCallTransferL( CStifItemParser& aItem );

        /**
        * Test ClearBlacklist
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ClearBlacklistL( CStifItemParser& aItem );

        /**
        * Complete ClearBlacklist
        * @since Series60_ver 3.0
        * @param aResult
                 aDataPackage
        * @return none.
        */
        void CompleteClearBlacklist( TInt aResult );

        /**
        * Test CreateConference
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CreateConference( CStifItemParser& aItem );

        /**
        * Test HoldConference
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt HoldConference( CStifItemParser& aItem );

        /**
        * Test ResumeConference
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ResumeConference( CStifItemParser& aItem );

        /**
        * Test SwapConference
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SwapConference( CStifItemParser& aItem );

        /**
        * Test HangUpCC
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt HangUpCC( CStifItemParser& aItem );

        /**
        * Test AddCallToCC
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt AddCallToCC( CStifItemParser& aItem );

        /**
        * Test GoOneToOne
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GoOneToOne( CStifItemParser& aItem );


        /**
        * VerifyRemoteAlertingToneStatus
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt VerifyRemoteAlertingToneStatus( CStifItemParser& aItem );

        /**
        * Complete VerifyRemoteAlertingToneStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteVerifyRemoteAlertingToneStatus( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Test DialEmergencyCall
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DialEmergencyCall( CStifItemParser& aItem );

        /**
        * Test ActivateUUSL
        * @since Series60_ver 3.2
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CVoice::ActivateUUSL( CStifItemParser& aItem );

        /**
        * Test IncomingCallWithUUS
        * @since Series60_ver 3.2
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CVoice::IncomingCallWithUUS( CStifItemParser& aItem );

        /**
        * Checks completions and their order to incoming UUS call
        * @since Series60_ver 3.2
        * @param aResult Completion result
                 aDataPackage Datapackage included to completion
                 aIpc IPC number of completion
        * @return none
        */
        void CVoice::CheckIncomingCallUUSCompletions( TInt aResult,
            CMmDataPackage* aDataPackage, TInt aIpc );

        /**
        * Complete CallTermination
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteCallTermination( TInt aResult );

        /**
        * Completes get mobile call info
        * @since Series60_ver 3.2
        * @param aResult     Completion result from NTSY
        * @param aCallData   Data package from NTSY
        * @return None
        */
        void CompleteMobileCallGetMobileCallInfo(
            TInt aResult,
            const CCallDataPackage* aCallData );
        /**
        * Test UpdateLifeTimer
        * @since Series60_ver 3.2
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CVoice::UpdateLifeTimer( CStifItemParser& aItem );

        /**
        * Test GetLifeTimer
        * @since Series60_ver 3.2
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CVoice::GetLifeTimer( CStifItemParser& aItem );

        /**
        * Complete Life Timer value request
        * @since Series60_ver 2.8
        * @param aResult
                 aDataPackage
        * @return none.
        */
        void CompleteGetLifeTimer(
                 TInt aResult,
                 CMmDataPackage* aDataPackage);

    protected:  // Functions from base classes

        /**
        * C++ default constructor.
        */
        CVoice( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void VoiceConstructL();

    private:

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below.
        */


        /**
        * Set MobileCallInfo
        * @since Series60_ver 2.8
        * @param aNetworkInfo
        * @return None.
        */
        void SetMobileCallInfo();

        /**
        * Sets MobileCallParams
        * @since Series60_ver 2.8
        * @param aNetworkInfo
        * @return Symbian OS error code.
        */
        TInt SetMobileCallParams( CStifItemParser& aItem );

        /**
        * Answer incoming call
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt AnswerIncomingCallL();

        /**
        * Answer incoming Video call
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt AnswerIncomingVideoCallL();

        /**
        * Complete Call Answer
        * @param aResult
        * @param aDataPackage
        * @return None
        */
        void CompleteCallAnswer(TInt aResult, CMmDataPackage* aDataPackage);

    public:     // Data

    protected:  // Data

    private:    // Data
        /**
        * Pointer to logger
        */
        CStifLogger*                iVoiceLog;

        RMobilePhone::TMobileService            iMobileService;

        RMobileCall::TMobileCallParamsV1        iCallParams;
        RMobileCall::TMobileCallParamsV1Pckg    iCallParamsPckg;

        RMobileCall::TMobileCallInfoV1          iMobileCallInfoV1;

        // Any test ongoing
        TOngoingTest                            iTestOngoing;

        //call id
        TInt                                    iCallId;

        //Incoming UUS call completion phase.
        TUUSCompletionState                     iUUSCompletionState;

        TBool                                   iAnswerIncomingCall;

        TUint8                                  iIndicationCounter;
        TUint8                                  iNumberOfCallsInCC;

        TBool                                   iDontAnswerCall;

        TBool                                   iInitializationOngoing;

        TConnectionState                        iVerifyConnectedStatus;

        TBuf<100>                               iSavedUUSNameForComparison;
        // For parameter checking
        TBool                                   iVoiceParameterCheck;

        // Test case number for parameter checking
        TTestCase                               iTestCaseNumber;

        // Reference parameters for parameter checking
        RMobilePhone::TMobileService            iRefCallMode;
        RMobileCall::TMobileCallStatus          iRefCallStatus;
        TInt                                    iRefCallId;
        TBuf<256>                               iRefDTMFStringToContinue;

        RMmCustomAPI::TLifeTimeData             iLifeTimerForComparison;

        TDTMFState                                   iDTMFState;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };



#endif      // VOICE_H



// End of File