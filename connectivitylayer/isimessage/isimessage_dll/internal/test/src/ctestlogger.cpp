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
//#include <pn_obj_auto.h>                // For PN_OBJ_EPOC_MONSERV_SERV
//#include "tisimsgerrcodestest.h"        // For isimsgtesterrors
//#include "tisimsghexstest.h"            // For 0x00 etc..
#include "ctestlogger.h"
#include "tisimsgtestconstants.h"       // For tisimsgtestconstants

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
const TInt KReqRespLength( 10 );
_LIT( KRequest, "Request:  ");
_LIT( KResponse, "Response:  ");

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
// CTestLogger::NewL
// Second phase static constuctor.
// -----------------------------------------------------------------------------
//
CTestLogger* CTestLogger::NewL
        (
        // None
        )
    {

    // Construct new CCommSubBlockTest instance.
    CTestLogger* self = CTestLogger::NewLC();
    CleanupStack::Pop( self );
    return self;
  
    }

// -----------------------------------------------------------------------------
// CTestLogger::NewLC
// Second phase static constuctor.
// -----------------------------------------------------------------------------
//
CTestLogger* CTestLogger::NewLC
        (
        // None
        )
    {

    // Construct new CCommSubBlockTest instance.
    CTestLogger* self = new( ELeave )CTestLogger();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
  
    }

// -----------------------------------------------------------------------------
// CTestLogger::~CTestLogger
// Destructor
// -----------------------------------------------------------------------------
//    
CTestLogger::~CTestLogger
        (
        // None
        )
    {

    // None

    }

// -----------------------------------------------------------------------------
// CTestLogger::CTestLogger
// Default constructor.
// -----------------------------------------------------------------------------
//
CTestLogger::CTestLogger
        (
        // None
        )
    {

    // None

    }

// -----------------------------------------------------------------------------
// CTestLogger::PrintHexL
// Writes IsiMsg as HEX data to log file.
// -----------------------------------------------------------------------------
//
void CTestLogger::PrintHexL
        (
        TExecutionAndDebugInfo& aInfo,  // Capsulates the exec. and debug info.
        const TDesC8& aBuf,             // The message
        const TMsgType aType            // Request / response.
        )
    {

    // The length for HEX message.
    const TInt length( aBuf.Length() );
    TInt hexLength( length * KTwo );
    if( hexLength > KMaxLogData )
        {
        hexLength = KMaxLogData;
        }

    TBuf<KReqRespLength> reqResp;
    if( ERequest == aType )
        {
        reqResp.Append( KRequest );
        }
    else 
        {
        reqResp.Append( KResponse );
        }
    // Write the IsiMsg HEX's to log.
    aInfo.iLogger.Log( CStifLogger::ECursive, reqResp );

    HBufC* hexBuf = HBufC::NewLC( hexLength );
    for( TInt i( KErrNone ); i < length; i++ )
        {
        hexBuf->Des().AppendNumUC( aBuf[ i ], EHex );    
        }
    // Write the IsiMsh HEX's to log.
    aInfo.iLogger.Log( CStifLogger::EBlue, *hexBuf );
    CleanupStack::PopAndDestroy( hexBuf );

    //REMOVE
    TBuf<2> remove;
    remove.Append( 0x00 );
    remove.Append( 0x00 );
    TBuf<4> remove2;
    remove2.AppendNumUC( 0x00, EHex );
    remove2.AppendNumUC( 0x00, EHex );
    aInfo.iLogger.Log( CStifLogger::EBlue, remove2 );
    //REMOVe

    }

// -----------------------------------------------------------------------------
// CAccSubBlockTest::SetCaseProgress
// Set the status failed / passed of the case.
// -----------------------------------------------------------------------------
//
void CTestLogger::PrintCaseProgress
        (
        TExecutionAndDebugInfo& aInfo,
        TInt aResult,
        const TDesC& aComment
        )
    {

    __UHEAP_MARK;
    // If PASSED.
    if( KErrNone == aResult )
        {
        // Print that test ok. For ATS.
        aInfo.iTestResult.iResultDes.Copy( aComment );
        // For STIF.
        aInfo.iTestInterface.Printf( KErrNone, aComment, KPass );
        // For Logger.
        aInfo.iLogger.Log( CStifLogger::ENoStyle, aComment );
        }
    // If FAILED
    else
        {
        // Print that test Fail. For ATS.
        aInfo.iTestResult.iResultDes.Copy( aComment );
        // For STIF.
        aInfo.iTestInterface.Printf( KErrNone, aComment, KFail );
        // For Logger.
        aInfo.iLogger.Log( CStifLogger::EError, aComment );
        }
    // Test case Failed / Passed.
    aInfo.iTestResult.iResult = aResult;
    __UHEAP_MARKEND;

    }

// -----------------------------------------------------------------------------
// CAccSubBlockTest::SetCaseStatus
// Set the status failed / passed of the case.
// -----------------------------------------------------------------------------
//
void CTestLogger::PrintCaseStatus
        (
        TExecutionAndDebugInfo& aInfo,
        TInt aResult,
        const TDesC& aFunctionName,
        const TStatusType aType
        )
    {

    __UHEAP_MARK;
    // If case ended.
    if( aType == EEnd )
        {
        // If PASSED.
        if( KErrNone == aResult )
            {
            TBuf<KPassFailLength> passed( KPass );
            // Print that test ok. For ATS.
            aInfo.iTestResult.iResultDes.Copy( KPass );
            // For STIF.
            aInfo.iTestInterface.Printf( KErrNone, aFunctionName, passed );
            // For Logger.
            aInfo.iLogger.Log( CStifLogger::EBold, aFunctionName );
            aInfo.iLogger.Log( CStifLogger::EGreen, passed );
            }
        // If FAILED
        else
            {
            TBuf<KPassFailLength> failed( KFail );
            // Print that test ok. For ATS.
            aInfo.iTestResult.iResultDes.Copy( KFail );
            // For STIF.
            aInfo.iTestInterface.Printf( KErrNone, aFunctionName, failed );
            // For Logger.
            aInfo.iLogger.Log( CStifLogger::EBold, aFunctionName );
            aInfo.iLogger.Log( CStifLogger::ERed, failed );
            }
        }
    // If case started.
    else
        {
        TBuf<KStartLength> start( KStart );
        // Print that test ok. For ATS.
        aInfo.iTestResult.iResultDes.Copy( KStart );
        // For STIF.
        aInfo.iTestInterface.Printf( KErrNone, aFunctionName, KStart );
        // For Logger.
        aInfo.iLogger.Log( CStifLogger::EGreen, start );
        aInfo.iLogger.Log( CStifLogger::EBold, aFunctionName );
        }
    // Test case Failed / Passed.
    aInfo.iTestResult.iResult = aResult;
    __UHEAP_MARKEND;

    }

// -----------------------------------------------------------------------------
// CTestLogger::ConstructL
// Symbian OS second phase constructor.
// -----------------------------------------------------------------------------
//
void CTestLogger::ConstructL
        (
        // None
        )
    {

    // None

    }
    //end of file
    
