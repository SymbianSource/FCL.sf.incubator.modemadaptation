/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for WIM tests
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "wim.h"


// CONSTANTS
_LIT8( KLine, "---------------------------------" );



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWIM::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CWIM::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CWIM::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CWIM::RunMethodL(
    CStifItemParser& aItem )
    {
    _LIT8 (KLog, "WIM: RunMethodL" );
    iWIMLog->Log((TDesC8)KLog);

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "PoweringSimOn",             CWIM::PoweringSimOn ),
        ENTRY( "PoweringSimOff",            CWIM::PoweringSimOff ),
        ENTRY( "SIMWarmReset",              CWIM::SIMWarmReset ),
        ENTRY( "GetAnswerToResetStatus",    CWIM::GetAnswerToResetStatus ),
        ENTRY( "GetSIMCardReaderStatus",    CWIM::GetSIMCardReaderStatus ),
        ENTRY( "SendAPDU",                  CWIM::SendAPDU ),
        ENTRY( "SendAPDURequest",           CWIM::SendAPDURequest ),
        ENTRY( "RunIscTestCase",            CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase",           CCore::SyncIscTestCase),
        ENTRY( "HandleSpecifiedRequests",   CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC",    CCore::SetExpectedCompleteIPC),

        };


    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }



// -----------------------------------------------------------------------------
// CWIM::RunIscTestCase
// Start ISC Test Control test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWIM::RunIscTestCase( CStifItemParser& aItem )
    {
    TInt testCaseId(0);
    TInt ret = aItem.GetNextInt(testCaseId);
    if (KErrNone == ret)
        {
        RunIscControlTestCase(1, testCaseId);
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CWIM::SyncIscTestCase
// Syncronize with ISC Test Control
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWIM::SyncIscTestCase( CStifItemParser& /*aItem*/ )
    {
    SyncIscControl();
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CWIM::PoweringSIMOn
// Powering SIM on
// -----------------------------------------------------------------------------
//
TInt CWIM::PoweringSimOn( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KPbInit, "CWIM::PoweringSimOn");
    iWIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );




    ret = HandleRequestL( ECustomPowerSimOnIPC  );

    if ( KErrNone == ret )
        {
            _LIT8( KSendOk, "PoweringSimOn request send ok" );
            iWIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
            _LIT8( KSendFailed, "PoweringSimOn request send failed: %d" );
            iWIMLog->Log((TDesC8)KSendFailed, ret );
        }



    return ret;

    }


// -----------------------------------------------------------------------------
// CWIM::CompletePoweringSIMOn
// Complete PoweringSIMOn method function.
// -----------------------------------------------------------------------------
//
void CWIM::CompletePoweringSimOn
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CompletePoweringSimOn result: %d" );
    iWIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePoweringSimOn Failed: %d" );
        iWIMLog->Log((TDesC8)KFail, aResult );
        }


    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CWIM::PoweringSIMOff
// Powering SIM off
// -----------------------------------------------------------------------------
//
TInt CWIM::PoweringSimOff( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KPbInit, "CWIM::PoweringSimOff");
    iWIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );


    ret = HandleRequestL( ECustomPowerSimOffIPC  );

    if ( KErrNone == ret )
        {
            _LIT8( KSendOk, "PoweringSimOff request send ok" );
            iWIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
            _LIT8( KSendFailed, "PoweringSimOff request send failed: %d" );
            iWIMLog->Log((TDesC8)KSendFailed, ret );
        }



    return ret;

    }


// -----------------------------------------------------------------------------
// CWIM::PoweringSIMOff
// Complete PoweringSIMOff method function.
// -----------------------------------------------------------------------------
//
void CWIM::CompletePoweringSimOff
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CompletePoweringSimOff result: %d" );
    iWIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePoweringSimOff Failed: %d" );
        iWIMLog->Log((TDesC8)KFail, aResult );
        }


    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CWIM::SIMWarmReset
// SIM Warm Reset
// -----------------------------------------------------------------------------
//
TInt CWIM::SIMWarmReset( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KPbInit, "CWIM::SIMWarmReset");
    iWIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );


    ret = HandleRequestL( ECustomSimWarmResetIPC  );

    if ( KErrNone == ret )
        {
            _LIT8( KSendOk, "SIMWarmReset request send ok" );
            iWIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
            _LIT8( KSendFailed, "SIMWarmReset request send failed: %d" );
            iWIMLog->Log((TDesC8)KSendFailed, ret );
        }


    return ret;

    }


// -----------------------------------------------------------------------------
// CWIM::CompleteSIMWarmReset
// Complete SIMWarmReset method function.
// -----------------------------------------------------------------------------
//
void CWIM::CompleteSIMWarmReset
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CompleteSIMWarmReset result: %d" );
    iWIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSIMWarmReset Failed: %d" );
        iWIMLog->Log((TDesC8)KFail, aResult );
        }


    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CWIM::GetAnswerToResetStatus
// SIM Warm Reset
// -----------------------------------------------------------------------------
//
TInt CWIM::GetAnswerToResetStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "CWIM::GetAnswerToResetStatus");
    iWIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );

    TPtrC string;
    aItem.GetNextString ( string );
    iAnswerToResetStatus.Zero();
    iAnswerToResetStatus.Append( string );

    ret = HandleRequestL( ECustomGetATRIPC );

    if ( KErrNone == ret )
        {
            _LIT8( KSendOk, "GetAnswerToResetStatus request send ok" );
            iWIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
            _LIT8( KSendFailed, "GetAnswerToResetStatus request send failed: %d" );
            iWIMLog->Log((TDesC8)KSendFailed, ret );
        }



    return ret;

    }


// -----------------------------------------------------------------------------
// CWIM::CompleteGetAnswerToResetStatus
// Complete GetAnswerToResetStatus method function.
// -----------------------------------------------------------------------------
//
void CWIM::CompleteGetAnswerToResetStatus
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    TBuf8<255> answerStatus;

    aDataPackage->UnPackData( answerStatus );

    if ( iAnswerToResetStatus != answerStatus )
        {
        aResult = KErrGeneral;
        }

    _LIT8( KResult, "CompleteGetAnswerToResetStatus result: %d" );
    iWIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetAnswerToResetStatus Failed: %d" );
        iWIMLog->Log((TDesC8)KFail, aResult );
        }


    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CWIM::GetSIMCardReaderStatus
// Get SIM card reader status
// -----------------------------------------------------------------------------
//
TInt CWIM::GetSIMCardReaderStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "CWIM::GetSIMCardReaderStatus");
    iWIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TInt value;
    TInt i;
    TInt byteCount;

    iSIMCardReaderStatus.Zero();

    aItem.GetNextInt ( byteCount );

    for( i = 0; i < byteCount; i++ )
        {
        aItem.GetNextInt ( value );
        iSIMCardReaderStatus.Append( value );
        }
        
    ret = HandleRequestL( ECustomGetSimCardReaderStatusIPC );

    if ( KErrNone == ret )
        {
            _LIT8( KSendOk, "GetSIMCardReaderStatus request send ok" );
            iWIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
            _LIT8( KSendFailed, "GetSIMCardReaderStatus request send failed: %d" );
            iWIMLog->Log((TDesC8)KSendFailed, ret );
        }



    return ret;

    }


// -----------------------------------------------------------------------------
// CWIM::CompleteGetSIMCardReaderStatus
// Complete GetSIMCardReaderStatus method function.
// -----------------------------------------------------------------------------
//
void CWIM::CompleteGetSIMCardReaderStatus
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {

    TBuf8<255> SIMCardReaderStatus;

    aDataPackage->UnPackData( SIMCardReaderStatus );

    if( iSIMCardReaderStatus != SIMCardReaderStatus )
        {
        aResult = KErrGeneral;
        }

    _LIT8( KResult, "CompleteGetSIMCardReaderStatus result: %d" );
    iWIMLog->Log((TDesC8)KResult, aResult );


    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetSIMCardReaderStatus Failed: %d" );
        iWIMLog->Log((TDesC8)KFail, aResult );
        }


    Signal( aResult );

    }



// -----------------------------------------------------------------------------
// CWIM::SendAPDU
// Send Application Data Unit (APDU)
// -----------------------------------------------------------------------------
//
TInt CWIM::SendAPDU( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "CWIM::SendAPDU");
    iWIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );

    TInt CLA;
    TInt INS;
    TInt P1;
    TInt P2;
    TInt Lc;

    TInt cardreaderid;

    aItem.GetNextInt( cardreaderid );

    aItem.GetNextInt ( CLA );
    aItem.GetNextInt( INS );
    aItem.GetNextInt( P1 );
    aItem.GetNextInt( P2 );
    aItem.GetNextInt( Lc );

    iParams.iCmdData.Append(CLA);
    iParams.iCmdData.Append(INS);
    iParams.iCmdData.Append(P1);
    iParams.iCmdData.Append(P2);
    iParams.iCmdData.Append(Lc);



    TInt tmp;

    //append data + Le (if present)
    while( aItem.GetNextInt(tmp) == KErrNone )
        iParams.iCmdData.Append(tmp);


    //iParams.iCardReaderId = 1;
    iParams.iCardReaderId = cardreaderid;

    iParams.iRspData = _L8("");


    RMmCustomAPI::TApduParameters* paramsPtr = &iParams;


    CMmDataPackage package;

    // Set package data
    package.PackData( &paramsPtr );

    ret = HandleRequestL( ECustomSendAPDUReqV2IPC, &package );

    if ( KErrNone == ret )
        {
            _LIT8( KSendOk, "SendAPDU request send ok" );
            iWIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
            _LIT8( KSendFailed, "SendAPDU request send failed: %d" );
            iWIMLog->Log((TDesC8)KSendFailed, ret );
        }



    return ret;

    }


// -----------------------------------------------------------------------------
// CWIM::CompleteSendAPDU
// Complete SendAPDU method function.
// -----------------------------------------------------------------------------
//
void CWIM::CompleteSendAPDU
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteSendAPDU result: %d" );
    iWIMLog->Log((TDesC8)KResult, aResult );

    TBuf8<255> params;

    aDataPackage->UnPackData( params );
    
    if( params != iParams.iCmdData )
        {
        aResult = KErrGeneral;
        }
        
    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSendAPDU Failed: %d" );
        iWIMLog->Log((TDesC8)KFail, aResult );
        }


    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CWIM::SendAPDURequest
// Send Application Data Unit (APDU) Request
// -----------------------------------------------------------------------------
//
TInt CWIM::SendAPDURequest( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "CWIM::SendAPDURequest");
    iWIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TInt servicetype;
    TInt crIdentity;
    TInt type;
    TInt sPAC;
    TInt length;

    TInt CLA;
    TInt INS;
    TInt P1;
    TInt P2;
    TInt Lc;


    aItem.GetNextInt ( servicetype );
    aItem.GetNextInt( crIdentity );
    aItem.GetNextInt( type );


    iInfo.Append(servicetype);
    iInfo.Append(crIdentity);
    iInfo.Append(type);

    if ( 0x4F == servicetype )
        {

        aItem.GetNextInt( sPAC );
        aItem.GetNextInt( length );

        aItem.GetNextInt( CLA );
        aItem.GetNextInt( INS );
        aItem.GetNextInt( P1 );
        aItem.GetNextInt( P2 );
        aItem.GetNextInt( Lc );

        iInfo.Append(sPAC);
        iInfo.Append(length);


        iData.Append(CLA);
        iData.Append(INS);
        iData.Append(P1);
        iData.Append(P2);
        iData.Append(Lc);

        TInt tmp;

        //append data + Le (if present)
        while( aItem.GetNextInt(tmp) == KErrNone )
            iData.Append(tmp);


        }

    RMmCustomAPI::TApdu           apduReg;
    apduReg.iInfo = &iInfo;
    apduReg.iData = &iData;


    if( supported )
    {


        CMmDataPackage package;

        // Set package data
        package.PackData( &apduReg );

        ret = HandleRequestL( ECustomSendAPDUReqIPC, &package );

        if ( KErrNone == ret )
            {
                _LIT8( KSendOk, "SendAPDURequest request send ok" );
                iWIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
                _LIT8( KSendFailed, "SendAPDURequest request send failed: %d" );
                iWIMLog->Log((TDesC8)KSendFailed, ret );
            }

    }

    return ret;

    }


// -----------------------------------------------------------------------------
// CWIM::CompleteSendAPDURequest
// Complete SendAPDURequest method function.
// -----------------------------------------------------------------------------
//
void CWIM::CompleteSendAPDURequest
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteSendAPDURequest result: %d" );
    iWIMLog->Log((TDesC8)KResult, aResult );

    TBuf8<255> info;
    TBuf8<255> data;

    aDataPackage->UnPackData( info, data );

    if( iData != data )
        {
        aResult = KErrGeneral;
        }
        
    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSendAPDURequest Failed: %d" );
        iWIMLog->Log((TDesC8)KFail, aResult );
        }


    Signal( aResult );

    }



// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


//  End of File

