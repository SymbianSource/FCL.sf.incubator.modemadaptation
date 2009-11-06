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
* Description: 
*
*/


// INCLUDES             
#include <pn_obj_auto.h>                // For PN_OBJ_EPOC_MONSERV_SERV
#include <isimsg.h>                     // For CIsiMsg
#include <isi_sos_bluetooth.h>          // For CBtIsiMsg
#include <isi_call.h>                   // For CCallIsiMsg
#include <isi_sim.h>                    // For CSimIsiMsg
#include <isi_tone.h>                   // For CToneIsiMsg
#include <f32file.h>                    // For RDebug
#include "tisimsgerrcodestest.h"        // For IsiMsgTestErrors
#include "tisimsghexstest.h"            // For 0x00 etc..
#include "ctestlogger.h"                // For CTestLogger
#include "tisimsgtestconstants.h"
#include "cisimsgbasetest.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
_LIT( KOpen, "CIsiMsgBaseTest::OpenIsaApiAndGetIsiMsgL" );
_LIT( KReceive, "CIsiMsgBaseTest::ReceiveIsiMsgL" );
_LIT( KSend, "CIsiMsgBaseTest::SendIsiMsgL" );
_LIT( KSendError, "*** Send: Wrong Message ID ***" );

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================
// None

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::NewL
// Second phase static constuctor.
// -----------------------------------------------------------------------------
//
CIsiMsgBaseTest* CIsiMsgBaseTest::NewL
        (
        TExecutionAndDebugInfo& aInfo,
        TInt& aResult
        )
    {

    // Construct new CCommSubBlockTest instance.
    CIsiMsgBaseTest* self = CIsiMsgBaseTest::NewLC( aInfo, aResult );
    CleanupStack::Pop( self );
    return self;
  
    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::NewLC
// Second phase static constuctor.
// -----------------------------------------------------------------------------
//
CIsiMsgBaseTest* CIsiMsgBaseTest::NewLC
        (
        TExecutionAndDebugInfo& aInfo,
        TInt& aResult
        )
    {

    // Construct new CCommSubBlockTest instance.
    CIsiMsgBaseTest* self = new ( ELeave )CIsiMsgBaseTest( aInfo, aResult );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
  
    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::~CIsiMsgBaseTest
// Destructor
// -----------------------------------------------------------------------------
//    
CIsiMsgBaseTest::~CIsiMsgBaseTest
        (
        // None
        )
    {

    // Delete and free local variables.
    ClearReceivedMsg();
    delete iIsaApiMsgExc;
    iIsaApiMsgExc = NULL;    
    delete iTestLog;
    iTestLog = NULL;

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::OpenIsaApiAndGetIsiMsgL
// Opens the IsaApi and creates new IsiMsg.
//
// If the IsaApi connection was already open, tries again.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::OpenIsaApiAndGetIsiMsgL
        (
        // None
        )
    {

    // Prints that case started for STIF, ATS and Logger.
    iTestLog->PrintCaseStatus( iInfo, iResult, iInfo.iCaseName, EStart );
    // For debug.
    RDebug::Print( KOpen );

    // Fo until connection OK.
    for( TInt i( 0 ), result = KErrAlreadyExists;
         KErrNone != result;
         i++ )
        {
        result = iIsaApi.Open( PN_OBJ_EPOC_MONSERV_SERV ); // 50
        // If exist allready try again until previous connection is closed.
        if( KErrAlreadyExists == result )
            {
            iResult = result;
            PrintCaseProgress( result, KIsaApiNotReady );
            // Wait for one second.
            RDebug::Print( KIsaApiNotReady );
            User::After( KOneSecond );
            }
        // If connection couldn't be opened and not open connection already
        // panic and leave.
        else if( KErrNone != result )
            {
            iResult = result;
            PrintCaseProgress( iResult, KIsaApiPanic );
            RDebug::Print( KIsaApiPanic );
            User::Panic( KIsaApiPanic, KZero );
            }
        else
            {
            // Nothing.
            }
        }

    iIsaApiMsgExc = CPnMsg::NewL( KLengthOfMessage - KSix );
    iIsaApiMsgExc->Ptr().FillZ();
    // If empty panic.
    if( !iIsaApiMsgExc )
        {
        iResult = KErrNoMemory;
        PrintCaseProgress( iResult, KIsaApiMsgEmpty );
        RDebug::Print( KIsaApiMsgEmpty );
        User::Panic( KIsaApiMsgEmpty, KOne );
        }

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::CloseIsaApi
// Closes the IsaApi connection down.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::CloseIsaApi
        (
        // None
        )
    {

    iIsaApi.Close();
    // Writes the end Status of case to STIF UI, ATS and log file.
    iTestLog->PrintCaseStatus( iInfo, iResult, iInfo.iCaseName, EEnd );

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::ClearReceivedMsg
// Clears the memory reserved for PnMsg.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::ClearReceivedMsg
        (
        // None
        )
    {

    delete iPnMsg;
    iPnMsg = NULL;
    
    }

// -----------------------------------------------------------------------------
// CAccSubBlockTest::SetCaseProgress
// Set the status failed / passed of the case.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::PrintCaseProgress
        (
        TInt aResult,
        const TDesC& aComment
        )
    {

    iResult = aResult;
    iTestLog->PrintCaseProgress( iInfo, iResult, aComment ); 

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::ReceiveIsiMsgL
// Receveis an Isi message.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::ReceiveIsiMsgL
        (
        CIsiMsg& aCopyRcvMsg
        )
    {

    // For debug print.
    RDebug::Print( KReceive );

    iPnMsg = CPnMsg::NewL( KPnMsgSize ); 
    CIsiMsg isaApiRcvMsg;
    isaApiRcvMsg.SetMessage( *iPnMsg );

    TRequestStatus localStatus;
    TPckgBuf<TUint16> len;
    iResult = iIsaApi.Receive( localStatus, isaApiRcvMsg, len );
    if( KErrNone != iResult )
        {
        PrintCaseProgress( iResult, KSendFail );
        RDebug::Print( KSendFail );
        User::Panic( KSendFail, KTwo );
        }
    RTimer timer;
    timer.CreateLocal();
    // Set timer
    TRequestStatus timerStatus;
    // Wait for response is expired.
    timer.After( timerStatus, RESP_EXPIRE_TIME );
    // Wait for timer or response.
    User::WaitForRequest( timerStatus, localStatus );

    // When either one has returned, check if iStatus is the one
    // that didn't return, thus meaning the timer did.
    if( localStatus == KRequestPending )
        {
        iIsaApi.ReceiveCancel();
        iResult = KErrTimedOut;
        PrintCaseProgress( iResult, KTimeOut );
        User::WaitForRequest( localStatus );
        }
    else
        {
        timer.Cancel();
        User::WaitForRequest( timerStatus );
        PrintCaseProgress( iResult, KInTime );
        }
    timer.Close();
    // Copy the received message to icommIsiMsg
    isaApiRcvMsg.CopyAndNull( &aCopyRcvMsg );

    /*FOR HEX's
    TRAPD( err, iTestLog->PrintHexL( iInfo, ERequest ) );
    if( KErrNone != err )
        {
        PrintCaseProgress( err, KHexPrintFail );
        }
    */

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::Receive16BitIsiMsgL
// Receveis an Isi message.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::Receive16BitIsiMsgL
        (
        CIsiMsg& aCopyRcvMsg
        )
    {

    // For debug print.
    RDebug::Print( KReceive );

    iPnMsg = CPnMsg::NewL( KPnMsgSize ); 
    CSimIsiMsg isaApiRcvMsg;
    isaApiRcvMsg.SetMessage16( *iPnMsg );

    TRequestStatus localStatus;
    TPckgBuf<TUint16> len;
    iResult = iIsaApi.Receive( localStatus, isaApiRcvMsg, len );
    if( KErrNone != iResult )
        {
        PrintCaseProgress( iResult, KSendFail );
        RDebug::Print( KSendFail );
        User::Panic( KSendFail, KTwo );
        }
    RTimer timer;
    timer.CreateLocal();
    // Set timer
    TRequestStatus timerStatus;
    // Wait for response is expired.
    timer.After( timerStatus, RESP_EXPIRE_TIME );
    // Wait for timer or response.
    User::WaitForRequest( timerStatus, localStatus );

    // When either one has returned, check if iStatus is the one
    // that didn't return, thus meaning the timer did.
    if( localStatus == KRequestPending )
        {
        iIsaApi.ReceiveCancel();
        iResult = KErrTimedOut;
        User::WaitForRequest( localStatus );
        PrintCaseProgress( iResult, KTimeOut );
        }
    else
        {
        timer.Cancel();
        PrintCaseProgress( iResult, KInTime );
        User::WaitForRequest( timerStatus );
        }
    timer.Close();
    // Copy the received message to icommIsiMsg
    isaApiRcvMsg.CopyAndNull( &aCopyRcvMsg );

    /*FOR HEX's
    TRAPD( err, iTestLog->PrintHexL( iInfo, ERequest ) );
    if( KErrNone != err )
        {
        PrintCaseProgress( err, KHexPrintFail );
        }
    */

    }


// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::SendIsiMsgL
// Send an Isi message.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::SendIsiMsgL
        (
        const TUint8 aFunctionType,
        const TUint8 aMessageId,
        CIsiMsg& aMessage,
        TDesC8& aData
        )
    {

    // For debug print.
    RDebug::Print( KSend );

    // Set message to point send buffer.
    aMessage.SetMessage( *iIsaApiMsgExc );

    if( PN_CALL == aFunctionType )
        {
        CCallIsiMsg& message = static_cast<CCallIsiMsg&>(aMessage);
        message.Create( 0x01, aMessageId, aData[0] );
        }
    else if( PN_TONE == aFunctionType )
        {
        CToneIsiMsg& message = static_cast<CToneIsiMsg&>(aMessage);
        message.Create( 0x01, aMessageId, &aData );
        }
    else
        {
        aMessage.Create( 0x01, aMessageId, &aData );
        }
    aMessage.Create( 0x01, aMessageId, &aData );

/*FOR HEX's
    TRAPD( err, iTestLog->PrintHexL( iInfo, aData, ERequest ) );
    if( KErrNone != err )
        {
        PrintCaseProgress( err, KHexPrintFail );
        }
*/

    aMessage.SetFunction( aFunctionType );
    iResult = iIsaApi.Send( aMessage );
    if( KErrNone != iResult )
        {
        PrintCaseProgress( iResult, KReceiveFail );
        RDebug::Print( KReceiveFail );
        User::Panic( KReceiveFail, KTwo );
        }   
    // Wait until 1sec*aSeconds has gone
    WaitASecond();

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::SendIsiMsgL
// Send an Isi message.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::Send16BitIsiMsgL
        (
        const TUint8 aFunctionType,
        const TUint8 aMessageId,
        CIsiMsg& aMessage,
        TDesC8& aData
        )
    {

    // For debug print.
    RDebug::Print( KSend );

    if( aFunctionType != PN_SIM )
        {
        iResult = KErrNotSupported;
        // For debug print.
        RDebug::Print( KSend );
        RDebug::Print( KSendError );
        User::Leave( KErrNotSupported );
        }

    CSimIsiMsg& simIsiMsg = static_cast<CSimIsiMsg&>( aMessage );
    // Set message to point send buffer.
    simIsiMsg.SetMessage16( *iIsaApiMsgExc );
    simIsiMsg.Create16( 0x01, aMessageId, &aData );

/*FOR HEX's
    TRAPD( err, iTestLog->PrintHexL( iInfo, aData, ERequest ) );
    if( KErrNone != err )
        {
        PrintCaseProgress( err, KHexPrintFail );
        }
*/

    simIsiMsg.SetFunction( aFunctionType );
    iResult = iIsaApi.Send( aMessage );
    if( KErrNone != iResult )
        {
        PrintCaseProgress( iResult, KReceiveFail );
        RDebug::Print( KReceiveFail );
        User::Panic( KReceiveFail, KTwo );
        }   
    // Wait until 1sec*aSeconds has gone
    WaitASecond();

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::SendIsiMsgL
// Send an Isi message.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::CreateIsiMsgL
        (
        const TUint8 aFunctionType,
        const TUint8 aMessageId,
        CIsiMsg& aMessage,
        TDesC8& aData
        )
    {

    // If used with wrong messageID
    if( aFunctionType != PN_BLUETOOTH_CORE && aFunctionType != PN_LOCATION
        && aFunctionType != PN_FM_RADIO && aFunctionType != PN_KEY
        && aFunctionType != PN_TUNEPLAYER && aFunctionType != PN_TONE
        && aFunctionType != PN_TIME && aFunctionType != PN_GSS 
        && aFunctionType != PN_SELFTEST && aFunctionType != PN_EPOC_INFO
        && aFunctionType != PN_SIM && aFunctionType != PN_DSP_AUDIO 
        && aFunctionType != PN_SMS_MEMORY )
        {
        iResult = KErrNotSupported;
        // For debug print.
        RDebug::Print( KSend );
        RDebug::Print( KSendError );
        User::Leave( KErrNotSupported );
        }
    // For debug print.
    RDebug::Print( KSend );

    // Set message to point send buffer.
    aMessage.SetMessage( *iIsaApiMsgExc );
    aMessage.Create( 0x01, aMessageId, &aData );
    aMessage.SetFunction( aFunctionType );

/*FOR HEX's
    TRAPD( err, iTestLog->PrintHexL( iInfo, aData, ERequest ) );
    if( KErrNone != err )
        {
        PrintCaseProgress( err, KHexPrintFail );
        }
*/

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::SendIsiMsgL
// Send an Isi message.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::SendCreatedIsiMsgL
        (
        const TUint8 aFunctionType,
        const TUint8, //aMessageId,
        CIsiMsg& aMessage,
        TDesC8& //aData
        )
    {

    // If used with wrong messageID
    if( aFunctionType != PN_BLUETOOTH_CORE && aFunctionType != PN_LOCATION
        && aFunctionType != PN_FM_RADIO && aFunctionType != PN_KEY
        && aFunctionType != PN_TUNEPLAYER && aFunctionType != PN_TONE
        && aFunctionType != PN_TIME && aFunctionType != PN_GSS
        && aFunctionType != PN_SELFTEST && aFunctionType != PN_EPOC_INFO
        && aFunctionType != PN_SIM && aFunctionType != PN_DSP_AUDIO 
        && aFunctionType != PN_SMS_MEMORY )
        {
        iResult = KErrNotSupported;
        // For debug print.
        RDebug::Print( KSend );
        RDebug::Print( KSendError );
        User::Leave( KErrNotSupported );
        }

    iResult = iIsaApi.Send( aMessage );
    if( KErrNone != iResult )
        {
        PrintCaseProgress( iResult, KReceiveFail );
        RDebug::Print( KReceiveFail );
        User::Panic( KReceiveFail, KTwo );
        }   
    // Wait until 1sec*aSeconds has gone
    WaitASecond();

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::CIsiMsgBaseTest
// Default constructor.
// -----------------------------------------------------------------------------
//
CIsiMsgBaseTest::CIsiMsgBaseTest
        (
        TExecutionAndDebugInfo& aInfo,
        TInt& aResult
        ):iInfo( aInfo ), iResult( aResult )
    {

    // None

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::ConstructL
// Symbian OS second phase constructor.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::ConstructL
        (
        // None
        )
    {

    // Writes notices to STIF UI, ATS, and logfile.
    iTestLog = CTestLogger::NewL();
    iIsaApiMsgExc = NULL;
    iPnMsg = NULL;

    }

// -----------------------------------------------------------------------------
// CIsiMsgBaseTest::WaitASecond
// Waits a second.
// -----------------------------------------------------------------------------
//
void CIsiMsgBaseTest::WaitASecond
        (
        // None
        )
    {

    // Wait until 1sec*aSeconds has gone
    RTimer timer;
    TRequestStatus timerStatus; 
    timer.CreateLocal(); 
    timer.After( timerStatus, KThousand * AUTOMATIC_MODE_SWITCH_DELAY );
    User::WaitForRequest( timerStatus ); 
    timer.Close();

    }
