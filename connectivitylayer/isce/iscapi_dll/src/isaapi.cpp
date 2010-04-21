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


/*****************************************************************************
   R&D NOTES ARE:
   TODO:     Something needs to be done
   TBR:        Something could be removed between TBR's
*****************************************************************************/

// INCLUDE FILES
#include <iscapi.h>                 // For RIscApi
#include "iaduserapitrace.h"        // For C_TRACE...
#include "iadinternaldefinitions.h" // For EIAD
#include "iadnokiadefinitions.h"    // For EIADCustom
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "isaapiTraces.h"
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TUint8 KOneParam( 1 );
const TUint8 KTwoParams( 2 );
const TUint8 KThreeParams( 3 );

const TUint8 KFirstParam( 0 );
const TUint8 KSecondParam( 1 );
const TUint8 KThirdParam( 2 );
const TInt KInfoLength( 1 );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RIscApi::RIscApi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RIscApi::RIscApi()
    :iNeededLen( NULL, 0 ),
     iNeededDataLen( NULL, 0 ),
     iConnectionStatus( NULL, 0 ),
     iFlowControlStatus( NULL, 0 ),
     iOpen( EFalse ),
     iChannelNumber( KNotInitializedChannel )
    {
    OstTrace0( TRACE_NORMAL, RISCAPI_RISCAPI_ENTRY, "<>RIscApi::RIscApi" );

    C_TRACE( ( _T( "RIscApi::RIscApi <->" ) ) );

    }


EXPORT_C RIscApi::~RIscApi()
    {
    OstTraceExt1( TRACE_NORMAL, DUP1_RISCAPI_RISCAPI_ENTRY, ">RIscApi::~RIscApi;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::~RIscApi 0x%x ->" ), iChannelNumber ) );
    Close();
    C_TRACE( ( _T( "RIscApi::~RIscApi 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DUP1_RISCAPI_RISCAPI_EXIT, "<RIscApi::~RIscApi" );
    }


// -----------------------------------------------------------------------------
// RIscApi::Initialize
// Initialize the ISC Interface
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::InitializeModemInterface(
        TRequestStatus& aStatus
        )
    {
    OstTraceExt2( TRACE_NORMAL, RISCAPI_INITIALIZEMODEMINTERFACE_ENTRY, ">RIscApi::InitializeModemInterface;aStatus=%x;iChannelNumber=%hx", ( TUint )&( aStatus ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::InitializeModemInterface 0x%x 0x%x ->" ), iChannelNumber, &aStatus ) );
    TRACE_ASSERT_ALWAYS;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    C_TRACE( ( _T( "RIscApi::InitializeModemInterfaceL 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_INITIALIZEMODEMINTERFACE_EXIT, "<RIscApi::InitializeModemInterface" );
    }

// -----------------------------------------------------------------------------
// RIscApi::InitializeCancel
// Cancel ISC Initialization sequence
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::InitializeCancel(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, RISCAPI_INITIALIZECANCEL_ENTRY, ">RIscApi::InitializeCancel" );

    C_TRACE( ( _T( "RIscApi::InitializeCancel ->" ) ) );
    TRACE_ASSERT_ALWAYS;
    C_TRACE( ( _T( "RIscApi::InitializeCancel  <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_INITIALIZECANCEL_EXIT, "<RIscApi::InitializeCancel" );
    }

// -----------------------------------------------------------------------------
// RIscApi::LoadL
// Load required drivers using Isc_config.ini file
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void RIscApi::LoadL( 
        const TDesC8&, //aMultiplexerInit,     // Multiplexer init. 
        const TDesC8& //aDataTransmissionInit // Datatransmission init.
        ) const
    {
    OstTrace0( TRACE_NORMAL, RISCAPI_LOADL_ENTRY, ">RIscApi::LoadL" );
    

    C_TRACE( ( _T( "RIscApi::LoadL ->" ) ) );
    TRACE_ASSERT_ALWAYS;
    C_TRACE( ( _T( "RIscApi::LoadL <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_LOADL_EXIT, "<RIscApi::LoadL" );
    }

// -----------------------------------------------------------------------------
// RIscApi::Open
// Open ISC channel asyncronously
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::Open(
        const TUint16 aChannelNumber,
        TRequestStatus& aStatus,
        TOwnerType aType,
        const TDesC8* aOpenParams
        )
    {
    OstTraceExt4( TRACE_NORMAL, RISCAPI_OPEN_ENTRY, ">RIscApi::Open;aChannelNumber=%hu;aStatus=%x;aType=%x;aOpenParams=%x", aChannelNumber, ( TUint )&( aStatus ), ( TUint )&( aType ), ( TUint )( aOpenParams ) );

    C_TRACE( ( _T( "RIscApi::Open 0x%x, 0x%x, 0x%x, 0x%x this 0x%x ->" ),
                   aChannelNumber, &aStatus, aType, aOpenParams, this ) );
    // Driver is tried to load everytime.
    TInt loadStatus( User::LoadLogicalDevice( KNameOfIsaAccessDriverLdd ) );
    C_TRACE( ( _T( "RIscApi::Open devicedriver loaded %d" ), loadStatus ) );
    ASSERT_PANIC_ALWAYS( ( KErrNone == loadStatus || KErrAlreadyExists == loadStatus), EIADDriverLoadFailed );
    TInt error( KErrInUse );
    if( KNotInitializedChannel == iChannelNumber )
        {
        HBufC8* buffer = NULL;
        TRAPD( err, buffer = HBufC8::NewL( KInfoLength ) );        
        ASSERT_PANIC_ALWAYS( err == KErrNone, KErrNoMemory );
        
        TPtr8 info = buffer->Des();
        ASSERT_PANIC_ALWAYS( ( aChannelNumber < EIADNokiaLastUserChannel ), EIADChannelNumberOutofRange );
        C_TRACE( ( _T( "RIscApi::Open ldd" ) ) );
        OstTrace0( TRACE_NORMAL, RISCAPI_OPEN, "RIscApi::Open ldd" );        
        info.Append( aChannelNumber );
        error = DoCreate( KIADDriverName, 
                          TVersion(), 
                          KNullUnit, 
                          NULL, 
                          &info, 
                          aType );
        delete buffer;
        }
    if( KErrNone != error )
        {
        C_TRACE( ( _T( "RIscApi::Open  - failed 0x%x error %d" ), aChannelNumber, error ) );
        OstTraceExt2( TRACE_NORMAL, RISCAPI_OPEN_FAILED, "RIscApi::Open - failed;aChannelNumber=%hx;error=%d", aChannelNumber, error );
        
        TRequestStatus* ptrStatus = &aStatus;
        User::RequestComplete( ptrStatus, error );
        }
    else
        {
        C_TRACE( ( _T( "RIscApi::Open iChannelNumber %d aChannelNumber %d " ), iChannelNumber, aChannelNumber ) );
        OstTraceExt2( TRACE_NORMAL, DUP1_RISCAPI_OPEN, "RIscApi::Open;iChannelNumber=%hx;aChannelNumber=%hx", iChannelNumber, aChannelNumber );        
        aStatus = KRequestPending;
        TRequestStatus status = KRequestPending;
        TAny* params[ KThreeParams ];
        params[ KFirstParam ] = reinterpret_cast< TAny* >( &status );
        params[ KSecondParam ] = reinterpret_cast< TAny* >( &iChannelNumber );
        // If opened with resource (aOpenParams).
        params[ KThirdParam ] = reinterpret_cast< TAny* >( const_cast<TDesC8*>( aOpenParams ) );
        error = DoControl( EIADAsyncOpen, params );
        TRACE_ASSERT_ALWAYS_COND( KErrNone == error );
        User::WaitForRequest(status);
        // In case of KErrAlreadyExists as a result to open we must close the handle after open.
        if( status.Int() == KErrAlreadyExists )
            {
            C_TRACE(  (  _T(  "RIscApi::Open close handle KErrAlreadyExists" ) ) );
            OstTraceExt1( TRACE_NORMAL, RISCAPI_OPEN_ALREADY_EXIST, "RIscApi::Open;aChannelNumber=%hx", aChannelNumber );
            RHandleBase::Close();
            }
        TRequestStatus* statusPtr=&aStatus;

        User::RequestComplete(statusPtr, status.Int());
        }
    C_TRACE(  (  _T(  "RIscApi::Open <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_OPEN_EXIT, "<RIscApi::Open" );
    }

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
EXPORT_C TInt RIscApi::Loan( 
        const TUint16 aChannelNumber,
        TOwnerType aHandleType
        // None
        )
    {

    C_TRACE( ( _T( "RIscApi::Loan 0x%x ->" ), iChannelNumber ) );
    OstTraceExt1( TRACE_NORMAL, RISCAPI_LOAN_ENTRY, ">RIscApi::Loan;iChannelNumber=%hx", iChannelNumber );
    // TODO: panic if open?
    //    PanicIfNotOpen();
    // Driver is tried to load everytime.
    TInt loadStatus( User::LoadLogicalDevice( KNameOfIsaAccessDriverLdd ) );
    C_TRACE( ( _T( "RIscApi::Open devicedriver loaded %d" ), loadStatus ) );
    OstTrace1( TRACE_NORMAL, DUY1_RISCAPI_LOAN, "RIscApi::Loan;loaded=%d", loadStatus );
    ASSERT_PANIC_ALWAYS( ( KErrNone == loadStatus || KErrAlreadyExists == loadStatus), EIADDriverLoadFailed );
    TInt error( KErrInUse );
    if( KNotInitializedChannel == iChannelNumber )
        {
        TBuf8<KInfoLength> info;
        ASSERT_PANIC_ALWAYS( ( aChannelNumber < EIADNokiaLastUserChannel ), EIADChannelNumberOutofRange );
        C_TRACE( ( _T( "RIscApi::Loan ldd" ) ) );
        info.Append( aChannelNumber );
        error = DoCreate( KIADDriverName, 
                          TVersion(), 
                          KNullUnit, 
                          NULL, 
                          &info, 
                          aHandleType );
        }
    if( KErrNone == error )
        {
        C_TRACE( ( _T( "RIscApi::Loan iChannelNumber %d aChannelNumber %d " ), iChannelNumber, aChannelNumber ) );
        OstTraceExt2( TRACE_NORMAL, DUY2_RISCAPI_LOAN, "RIscApi::Loan;iChannelNumber=%d;aChannelNumber=%d", iChannelNumber, aChannelNumber );
        TAny* params[ KOneParam ];
        params[ KFirstParam ] = reinterpret_cast< TAny* >( &iChannelNumber );
        error = DoControl( EIADLoan, params );
        TRACE_ASSERT_ALWAYS_COND( KErrNone == error );
        if( KErrNone != error )
            {
            C_TRACE(  (  _T(  "RIscApi::Loan loan failed. closing the handle.." ) ) );
            OstTrace0( TRACE_NORMAL, DUY3_RISCAPI_LOAN, "RIscApi::Loan loan failed. closing the handle.." );
            RHandleBase::Close();
            }
        }
    TRACE_ASSERT_ALWAYS_COND( KErrNone == error );
    OstTraceExt1( TRACE_NORMAL, RISCAPI_LOAN_EXIT, "<RIscApi::Loan;iChannelNumber=%hx", iChannelNumber );
    C_TRACE( ( _T( "RIscApi::Loan 0x%x %d<-" ), iChannelNumber, error ) );
    return error;

    }

EXPORT_C void RIscApi::ReturnLoan( 
        const TUint16 // aChannelNumber
        // None
        )
    {

    C_TRACE( ( _T( "RIscApi::ReturnLoan 0x%x ->" ), iChannelNumber ) );
    OstTraceExt1( TRACE_NORMAL, RISCAPI_RETURNLOAN_ENTRY, ">RIscApi::Loan;iChannelNumber=%hx", iChannelNumber );
    PanicIfNotOpen();
    if( iChannelNumber < EIADNokiaLastUserChannel )
        {
        TInt ret( DoControl( EIADReturnLoan ) );
        ASSERT_PANIC_ALWAYS( KErrNone == ret, EIADReturnLoan );
        iChannelNumber = KNotInitializedChannel;
        RHandleBase::Close();// Close the channel handle.
        }
    OstTraceExt1( TRACE_NORMAL, RISCAPI_RETURNLOAN_EXIT, "<RIscApi::Loan;iChannelNumber=%hx", iChannelNumber );
    C_TRACE( ( _T( "RIscApi::ReturnLoan 0x%x %d<-" ), iChannelNumber ) );

    }
#endif

// -----------------------------------------------------------------------------
// RIscApi::OpenCancel
// Cancel asynchronous channel opening
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::OpenCancel(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_OPENCANCEL_ENTRY, ">RIscApi::OpenCancel;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::OpenCancel 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    DoCancel( EIADAsyncOpen );
    Close();
    C_TRACE( ( _T( "RIscApi::OpenCancel 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_OPENCANCEL_EXIT, "<RIscApi::OpenCancel" );
    }

// -----------------------------------------------------------------------------
// RIscApi::Close
// Close ISC Channel
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::Close(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_CLOSE_ENTRY, ">RIscApi::Close;iChannelNumber=%hx", iChannelNumber );


    C_TRACE( ( _T( "RIscApi::Close 0x%x 0x%x ->" ), iChannelNumber, this ) );
    if( iChannelNumber < EIADNokiaLastUserChannel )
        {
        PanicIfNotOpen();
        TInt ret( DoControl( EIADClose ) );
        ASSERT_PANIC_ALWAYS( KErrNone == ret, EIADCloseFailed );
        iChannelNumber = KNotInitializedChannel;
        C_TRACE(  (  _T(  "RIscApi::Close close handle" ) ) );
        OstTrace0( TRACE_NORMAL, RISCAPI_CLOSE, "RIscApi::Close close handle" );        
        RHandleBase::Close();
        }
    else
        {
        C_TRACE((_T("RIscApi::Close error: channel number out of range")));
        OstTrace0( TRACE_NORMAL, DUP1_RISCAPI_CLOSE, "RIscApi::Close channelnumber out of range" );
//        TRACE_ASSERT_ALWAYS;        TODO: Trace assert?
        }
    C_TRACE( ( _T( "RIscApi::Close 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_CLOSE_EXIT, "<RIscApi::Close" );
    }

// -----------------------------------------------------------------------------
// RIscApi::Close
// Close ISC Channel
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::Close(
        TRequestStatus& aStatus
        )
    {
    OstTraceExt2( TRACE_NORMAL, DUP1_RISCAPI_CLOSE_ENTRY, ">RIscApi::Close;aStatus=%x;iChannelNumber=%hx", ( TUint )&( aStatus ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::Close 0x%x 0x%x ->" ), iChannelNumber, &aStatus ) );
    if( iChannelNumber < EIADNokiaLastUserChannel )
        {
        PanicIfNotOpen();
        aStatus = KRequestPending;
        TAny* params[ KOneParam ];
        params[ KFirstParam ] = reinterpret_cast< TAny* >( &aStatus );
        TInt ret( DoControl( EIADAsyncClose, params ) );
        ASSERT_PANIC_ALWAYS( KErrNone == ret, EIADCloseFailed );
        iChannelNumber = KNotInitializedChannel;
        C_TRACE(  (  _T(  "RIscApi::Close a close handle" ) ) );
        RHandleBase::Close();// Added handle close
        }
    else
        {
        C_TRACE((_T("RIscApi::Close error: channel number out of range")));
        TRequestStatus* ptrStatus = &aStatus;
        User::RequestComplete( ptrStatus, KErrNotReady );
        }
    C_TRACE( ( _T( "RIscApi::Close 0x%x 0x%x <-" ), iChannelNumber, &aStatus ) );

    OstTrace0( TRACE_NORMAL, DUP1_RISCAPI_CLOSE_EXIT, "<RIscApi::Close" );
    }

// -----------------------------------------------------------------------------
// RIscApi::ResetBuffers
// Resets buffers.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::ResetBuffers(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_RESETBUFFERS_ENTRY, ">RIscApi::ResetBuffers;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::ResetBuffers 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    TRACE_ASSERT_ALWAYS_COND( KErrNone == DoControl( EIADResetQueues ) );
    C_TRACE( ( _T( "RIscApi::ResetBuffers <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_RESETBUFFERS_EXIT, "<RIscApi::ResetBuffers"  );
    }

// -----------------------------------------------------------------------------
// RIscApi::Send
// Send data to Domestic OS ( asynchronous )
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::Send(
        TRequestStatus& aStatus, 
        const TDesC8& aData
        )
    {
    OstTraceExt2( TRACE_NORMAL, RISCAPI_SEND_ENTRY, ">RIscApi::Send;aStatus=%x;aData=%x", ( TUint )&( aStatus ), ( TUint )&( aData ) );
    
    C_TRACE( ( _T( "RIscApi::Send a 0x%x 0x%x 0x%x this 0x%x ->" ),
                   iChannelNumber, &aStatus, &aData, this ) );
    PanicIfNotOpen();
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( const_cast<TDesC8*>( &aData ));
    aStatus = KRequestPending;
    DoControl( EIADAsyncSend, params );
    C_TRACE( ( _T( "RIscApi::Send <-" )  ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_SEND_EXIT, "<RIscApi::Send" );
    }

// -----------------------------------------------------------------------------
// RIscApi::Send
// Send data to Domestic OS ( synchronous )
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIscApi::Send(
        const TDesC8& aData
        )
    {
    OstTraceExt2( TRACE_NORMAL, DUP1_RISCAPI_SEND_ENTRY, ">RIscApi::Send;aData=%x;iChannelNumber=%hx", ( TUint )&( aData ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::Send 0x%x 0x%x 0x%x ->" ), iChannelNumber, &aData, this ) );
    PanicIfNotOpen();
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &( const_cast<TDesC8&>( aData ) ) );
    TInt error( DoControl( EIADSend, params ) );
    C_TRACE( ( _T( "RIscApi::Send 0x%x 0x%x 0x%x %d <-" ), iChannelNumber, &aData, this, error ) );

    OstTrace1( TRACE_NORMAL, DUP1_RISCAPI_SEND_EXIT, "<RIscApi::Send;return=%d", error );
    return error;
    }

// -----------------------------------------------------------------------------
// RIscApi::SendCancel
// Cancel asynchronous Send request
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::SendCancel(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_SENDCANCEL_ENTRY, ">RIscApi::SendCancel;iChannelNumber=%hx", iChannelNumber );

    C_TRACE(  (  _T(  "RIscApi::SendCancel 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    DoCancel( EIADAsyncSend );
    C_TRACE( ( _T( "RIscApi::SendCancel <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_SENDCANCEL_EXIT, "<RIscApi::SendCancel");
    }

// -----------------------------------------------------------------------------
// RIscApi::Receive
// Receive data from Domestic OS
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::Receive(
        TRequestStatus& aStatus,
        TDes8& aData,
        TUint16& aNeededBufLen
        )
    {
    OstTraceExt4( TRACE_NORMAL, RISCAPI_RECEIVE_ENTRY, ">RIscApi::Receive;aStatus=%x;aData=%x;aNeededBufLen=%hu;iChannelNumber=%hx", ( TUint )&( aStatus ), ( TUint )&( aData ), aNeededBufLen, iChannelNumber );

    C_TRACE( ( _T( "RIscApi::Receive 0x%x, 0x%x, 0x%x 0x%x this 0x%x ->" ), &aStatus, &aData, aNeededBufLen, iChannelNumber, this ) );
    PanicIfNotOpen();
    TAny* params[ KThreeParams ];
    // Set status pending, just in case client haven't done it.
    aStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aData );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( &aNeededBufLen );
    DoControl( EIADAsyncReceive, params );
    C_TRACE( ( _T( "RIscApi::Receive 0x%x this 0x%x <-" ), iChannelNumber, this ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_RECEIVE_EXIT, "<RIscApi::Receive" );
    }

// -----------------------------------------------------------------------------
// RIscApi::ReceiveCancel
// Cancel data receiving from Domestic OS
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::ReceiveCancel(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_RECEIVECANCEL_ENTRY, ">RIscApi::ReceiveCancel;iChannelNumber=%hx", iChannelNumber );
	
    C_TRACE( ( _T( "RIscApi::ReceiveCancel 0x%x ->"), iChannelNumber ) );
    PanicIfNotOpen();
    DoCancel( EIADAsyncReceive );
    C_TRACE( ( _T( "RIscApi::ReceiveCancel <-") ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_RECEIVECANCEL_EXIT, "<RIscApi::ReceiveCancel" );
    }

// -----------------------------------------------------------------------------
// RIscApi::DataReceiveCancel
// Cancel data receiving from Domestic OS
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::DataReceiveCancel(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_DATARECEIVECANCEL_ENTRY, ">RIscApi::DataReceiveCancel;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::DataReceiveCancel 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    DoCancel( EIADAsyncDataReceive );
    C_TRACE( ( _T( "RIscApi::DataReceiveCancel <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_DATARECEIVECANCEL_EXIT, "<RIscApi::DataReceiveCancel" );
    }

// -----------------------------------------------------------------------------
// RIscApi::ConnectionStatus
// Get the current connection status
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIscApi::ConnectionStatus(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_CONNECTIONSTATUS_ENTRY, ">RIscApi::ConnectionStatus;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::ConnectionStatus 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    TInt connStatus( DoControl( EIADGetConnectionStatus ) );
    C_TRACE( ( _T( "RIscApi::ConnectionStatus 0x%x %d <-" ), iChannelNumber, connStatus ) );

    OstTrace1( TRACE_NORMAL, RISCAPI_CONNECTIONSTATUS_EXIT, "<RIscApi::ConnectionStatus;return=%d", connStatus );
    return connStatus;
    }

// -----------------------------------------------------------------------------
// RIscApi::NotifyConnectionStatus
// Listen for connection status changes
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::NotifyConnectionStatus(
        TRequestStatus& aStatus,
        TInt& aConnectionStatus
        )
    {
    OstTraceExt3( TRACE_NORMAL, RISCAPI_NOTIFYCONNECTIONSTATUS_ENTRY, ">RIscApi::NotifyConnectionStatus;aStatus=%x;aConnectionStatus=%d;iChannelNumber=%hx", ( TUint )&( aStatus ), aConnectionStatus, iChannelNumber );

    C_TRACE( ( _T( "RIscApi::NotifyConnectionStatus 0x%x, 0x%x 0x%x ->" ), &aStatus, aConnectionStatus, iChannelNumber ) );
    PanicIfNotOpen();
    TAny* params[ KTwoParams ];
    aStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aConnectionStatus );
    DoControl( EIADAsyncNotifyConnectionStatus, params );
    C_TRACE( ( _T( "RIscApi::NotifyConnectionStatus <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_NOTIFYCONNECTIONSTATUS_EXIT, "<RIscApi::NotifyConnectionStatus" );
    }

// -----------------------------------------------------------------------------
// RIscApi::NotifyConnectionStatusCancel
// Stop listening for connection status changes
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::NotifyConnectionStatusCancel(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_NOTIFYCONNECTIONSTATUSCANCEL_ENTRY, ">RIscApi::NotifyConnectionStatusCancel;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::NotifyConnectionStatusCancel 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    DoCancel( EIADAsyncNotifyConnectionStatus );
    C_TRACE( ( _T( "RIscApi::NotifyConnectionStatusCancel 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_NOTIFYCONNECTIONSTATUSCANCEL_EXIT, "<RIscApi::NotifyConnectionStatusCancel" );
    }


// -----------------------------------------------------------------------------
// RIscApi::CustomFunction
// Asynchronous API extension function
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::CustomFunction(
        TRequestStatus& aStatus,
        const TUint16 aOperation,
        TAny* aParameters
        )
    {
    OstTraceExt4( TRACE_NORMAL, RISCAPI_CUSTOMFUNCTION_ENTRY, ">RIscApi::CustomFunction;aStatus=%x;aOperation=%hu;aParameters=%x;iChannelNumber=%hx", ( TUint )&( aStatus ), aOperation, ( TUint )( aParameters ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::CustomFunction a %d 0x%x 0x%x 0x%x 0x%x ->"), aOperation, aParameters, &aStatus, iChannelNumber, this ) );
    PanicIfNotOpen();
    TAny* params[ KTwoParams ];
    aStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = aParameters;
    TUint16 operation( 0xffff );
    switch( aOperation )
        {
        case EIADCustomAsyncSubscribeIndications:
            {
            operation = EIADAsyncSubscribeIndications;
            break;
            }
        case EIADCustomAsyncSubscribeIndications32Bit:
            {
            operation = EIADAsyncSubscribeIndications32Bit;
            break;
            }
        case EIADCustomAsyncSendIndication:
            {
            operation = EIADAsyncSendIndication;
            break;
            }
        default:
            {
            ASSERT_PANIC_ALWAYS( 0, EIADWrongCustomFunction );
            break;
            }
        }
    DoControl( operation, params );
    C_TRACE( ( _T( "RIscApi::CustomFunction %d 0x%x 0x%x 0x%x 0x%x <-"), aOperation, aParameters, &aStatus, iChannelNumber, this ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_CUSTOMFUNCTION_EXIT, "<RIscApi::CustomFunction" );
    }

// -----------------------------------------------------------------------------
// RIscApi::CustomFunction
// Synchronous API extension function
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIscApi::CustomFunction(
        const TUint16 aOperation, 
        TAny* aParameters
        )
    {
    OstTraceExt3( TRACE_NORMAL, DUP1_RISCAPI_CUSTOMFUNCTION_ENTRY, ">RIscApi::CustomFunction;aOperation=%hu;aParameters=%x;iChannelNumber=%hx", aOperation, ( TUint )( aParameters ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::CustomFunction %d 0x%x 0x%x 0x%x ->"), aOperation, aParameters, iChannelNumber, this ) );
    PanicIfNotOpen();
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = aParameters;
    TUint16 operation( 0xffff );
    switch( aOperation )
        {
        case EIADCustomSubscribeIndications:
            {
            operation = EIADSubscribeIndications;
            break;
            }
        case EIADCustomSubscribeIndications32Bit:
            {
            operation = EIADSubscribeIndications32Bit;
            break;
            }
        case EIADCustomSendIndication:
            {
            operation = EIADSendIndication;
            break;
            }
        default:
            {
            ASSERT_PANIC_ALWAYS( 0, EIADWrongCustomFunction );
            break;
            }
        }
    TInt error( DoControl( operation, params ) );
    C_TRACE( ( _T( "RIscApi::CustomFunction %d 0x%x %d 0x%x <-" ), aOperation, iChannelNumber, error, this ) );

    OstTrace1( TRACE_NORMAL, DUP1_RISCAPI_CUSTOMFUNCTION_EXIT, "<RIscApi::CustomFunction;return=%d", error );
    return error;
    }

// -----------------------------------------------------------------------------
// RIscApi::CustomFunctionCancel
// Cancel the execution of the asynchronous API extension function
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::CustomFunctionCancel(
        const TUint16 aOperation
        )
    {
    OstTraceExt2( TRACE_NORMAL, RISCAPI_CUSTOMFUNCTIONCANCEL_ENTRY, ">RIscApi::CustomFunctionCancel;aOperation=%hu;iChannelNumber=%hx", aOperation, iChannelNumber );

    C_TRACE( ( _T( "RIscApi::CustomFunctionCancel 0x%x 0x%x ->" ), aOperation, iChannelNumber ) );
    PanicIfNotOpen();
    TUint16 operation( 0xffff );
    switch( aOperation )
        {
        case EIADCancelCustomAsyncSubscribeIndications:
            {
            operation = EIADAsyncSubscribeIndications;
            break;
            }
        case EIADCancelCustomAsyncSubscribeIndications32Bit:
            {
            operation = EIADAsyncSubscribeIndications32Bit;
            break;
            }
        case EIADCancelCustomAsyncSendIndication:
            {
            operation = EIADAsyncSendIndication;
            break;
            }
        default:
            {
            ASSERT_PANIC_ALWAYS( 0, EIADWrongCustomFunction );
            break;
            }
        }
    DoCancel( operation );
    C_TRACE( ( _T( "RIscApi::CustomFunctionCancel <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_CUSTOMFUNCTIONCANCEL_EXIT, "<RIscApi::CustomFunctionCancel" );
    }

// -----------------------------------------------------------------------------
// RIscApi::DataSend
// Send data to Domestic OS ( asynchronous )
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::DataSend( 
        TRequestStatus& aStatus,
        const TDesC8& aData
        )
    {
    OstTraceExt3( TRACE_NORMAL, RISCAPI_DATASEND_ENTRY, ">RIscApi::DataSend;aStatus=%x;aData=%x;iChannelNumber=%hx", ( TUint )&( aStatus ), ( TUint )&( aData ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::DataSend a 0x%x 0x%x 0x%x ->" ), &aStatus, &aData, iChannelNumber ) );
    PanicIfNotOpen();
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( const_cast<TDesC8*>( &aData ));
    aStatus = KRequestPending;
    DoControl( EIADAsyncDataSend, params );
    C_TRACE( ( _T( "RIscApi::DataSend <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_DATASEND_EXIT, "<RIscApi::DataSend" );
    }

// -----------------------------------------------------------------------------
// RIscApi::DataSend
// Send data to Domestic OS ( synchronous )
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIscApi::DataSend(
        const TDesC8& aData
        )
    {
    OstTraceExt2( TRACE_NORMAL, DUP1_RISCAPI_DATASEND_ENTRY, ">RIscApi::DataSend;aData=%x;iChannelNumber=%hx", ( TUint )&( aData ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::DataSend 0x%x 0x%x" ), &aData, iChannelNumber ) );
    PanicIfNotOpen();
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &( const_cast<TDesC8&>( aData ) ) );
    TInt error( DoControl( EIADDataSend, params ) );
    C_TRACE( ( _T( "RIscApi::DataSend %d <-" ), error ) );

    OstTrace1( TRACE_NORMAL, DUP1_RISCAPI_DATASEND_EXIT, "<RIscApi::DataSend;return=%d", error );
    return error;
    }

// -----------------------------------------------------------------------------
// RIscApi::DataSendCancel
// Cancel asynchronous DataSend request
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::DataSendCancel(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_DATASENDCANCEL_ENTRY, ">RIscApi::DataSendCancel;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::DataSendCancel 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    DoCancel( EIADAsyncDataSend );
    C_TRACE( ( _T( "RIscApi::DataSendCancel <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_DATASENDCANCEL_EXIT, "<RIscApi::DataSendCancel" );
    }

// -----------------------------------------------------------------------------
// RIscApi::DataReceive
// Receive data from Domestic OS
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::DataReceive(
        TRequestStatus& aStatus,
        TDes8& aData,
        TUint16& aNeededBufLen
        )
    {
    OstTraceExt4( TRACE_NORMAL, RISCAPI_DATARECEIVE_ENTRY, ">RIscApi::DataReceive;aStatus=%x;aData=%x;aNeededBufLen=%hu;iChannelNumber=%hx", ( TUint )&( aStatus ), ( TUint )&( aData ), aNeededBufLen, iChannelNumber );

    C_TRACE( ( _T( "RIscApi::DataReceive 0x%x 0x%x 0x%x 0x%x ->" ), &aStatus, &aData, aNeededBufLen, iChannelNumber ) );
    PanicIfNotOpen();
    TAny* params[ KThreeParams ];
    // Set status pending, just in case client haven't done it.
    aStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aData );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( &aNeededBufLen );
    DoControl( EIADAsyncDataReceive, params );
    C_TRACE( ( _T( "RIscApi::DataReceive 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_DATARECEIVE_EXIT, "<RIscApi::DataReceive" );
    }

// -----------------------------------------------------------------------------
// RIscApi::FlowControlStatus
// Get the current flow control status ( in uplink-direction )
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIscApi::FlowControlStatus(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_FLOWCONTROLSTATUS_ENTRY, ">RIscApi::FlowControlStatus;iChannelNumber=%hx", iChannelNumber );


    C_TRACE( ( _T( "RIscApi::FlowControlStatus 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    TInt error( DoControl( EIADGetFlowControlStatus ) );
    C_TRACE( ( _T( "RIscApi::FlowControlStatus 0x%x %d <-" ), iChannelNumber, error ) );

    OstTrace1( TRACE_NORMAL, RISCAPI_FLOWCONTROLSTATUS_EXIT, "<RIscApi::FlowControlStatus;return=%d", error );
    return error;
    }


// -----------------------------------------------------------------------------
// RIscApi::NotifyFlowControl
// Listen for flow control status changes
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::NotifyFlowControl(
        TRequestStatus& aStatus,
        TInt& aFlowControlStatus
        )
    {
    OstTraceExt3( TRACE_NORMAL, RISCAPI_NOTIFYFLOWCONTROL_ENTRY, ">RIscApi::NotifyFlowControl;aStatus=%x;aFlowControlStatus=%d;iChannelNumber=%hx", ( TUint )&( aStatus ), aFlowControlStatus, iChannelNumber );

    C_TRACE( ( _T( "RIscApi::NotifyFlowControl 0x%x 0x%x ch 0x%x ->" ),
                   &aStatus, aFlowControlStatus, iChannelNumber ) );
    PanicIfNotOpen();
    TAny* params[ KTwoParams ];
    aStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aFlowControlStatus );
    DoControl( EIADAsyncNotifyFlowControlStatus, params );
    C_TRACE( ( _T( "RIscApi::NotifyFlowControl <-" ) ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_NOTIFYFLOWCONTROL_EXIT, "<RIscApi::NotifyFlowControl" );
    }


// -----------------------------------------------------------------------------
// RIscApi::NotifyFlowControlCancel
// Stop listening for flow control status changes
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C void RIscApi::NotifyFlowControlCancel(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_NOTIFYFLOWCONTROLCANCEL_ENTRY, ">RIscApi::NotifyFlowControlCancel;iChannelNumber=%hx", iChannelNumber );

    C_TRACE(  (  _T(  "RIscApi::NotifyFlowControlCancel 0x%x -> "), iChannelNumber ) );
    PanicIfNotOpen();
    DoCancel( EIADAsyncNotifyFlowControlStatus );
    C_TRACE( ( _T( "RIscApi::NotifyFlowControlCancel 0x%x <-"), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, RISCAPI_NOTIFYFLOWCONTROLCANCEL_EXIT, "<RIscApi::NotifyFlowControlCancel" );
    }

// -----------------------------------------------------------------------------
// RIscApi::MaximumDataSize
// Get maximun data size ( for sending and receiving )
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIscApi::MaximumDataSize(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, RISCAPI_MAXIMUMDATASIZE_ENTRY, ">RIscApi::MaximumDataSize;iChannelNumber=%hx", iChannelNumber );

    C_TRACE( ( _T( "RIscApi::MaximumDataSize 0x%x ->" ), iChannelNumber ) );
    PanicIfNotOpen();
    TInt returnCode( DoControl( EIADGetMaxDataSize ) );
    // If positive the maximum amount of data in ISI or data message to be send
    // If negative, one of the error codes. TODO: add to documentation, check possible error codes.
    C_TRACE( ( _T( "RIscApi::MaximumDataSize 0x%x %d <-" ), iChannelNumber, returnCode ) );

    OstTrace1( TRACE_NORMAL, RISCAPI_MAXIMUMDATASIZE_EXIT, "<RIscApi::MaximumDataSize;return=%d", returnCode );
    return returnCode;
    }

// -----------------------------------------------------------------------------
// RIscApi::GetChannelInfo
// Get channel info from multiplexer
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIscApi::GetChannelInfo(
        const TUint16 aChannel, 
        TDes8& aInfo
        )
    {
    OstTraceExt3( TRACE_NORMAL, RISCAPI_GETCHANNELINFO_ENTRY, ">RIscApi::GetChannelInfo;aChannel=%hu;aInfo=%x;iChannelNumber=%hx", aChannel, ( TUint )&( aInfo ), iChannelNumber );

    C_TRACE( ( _T( "RIscApi::GetChannelInfo 0x%x 0x%x ->" ), aChannel, &aInfo ) );
    PanicIfNotOpen();
    const TInt KChannelInfoMax( 2 );
    TInt error( aInfo.MaxLength() >= KChannelInfoMax ? KErrNone : KErrArgument );
    // TODO: Returns information of user channels only, not possible to get information
    // about kernel channels at the moment.
    // TODO: error if channel is out of range?
    if( KErrArgument != error )
        {
        aInfo.SetLength( KChannelInfoMax );
        aInfo[ KFirstParam ] = static_cast<TUint8>( aChannel >> 8 );
        aInfo[ KSecondParam ] = static_cast<TUint8>( aChannel );
        }
    C_TRACE( ( _T( "RIscApi::GetChannelInfo 0x%x 0x%x %d <-" ), aChannel, &aInfo, error ) );
    
    OstTrace1( TRACE_NORMAL, RISCAPI_GETCHANNELINFO_EXIT, "<RIscApi::GetChannelInfo;return=%d", error );
    return error;
    }

// -----------------------------------------------------------------------------
// IscApi::PanicIfNotOpen
// Panic if channel is not opened yet
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void RIscApi::PanicIfNotOpen(
        // None
        ) const
    {
    //OstTraceExt1( TRACE_NORMAL, RISCAPI_PANICIFNOTOPEN_ENTRY, ">RIscApi::PanicIfNotOpen;iChannelNumber=%hx", iChannelNumber );
	
    C_TRACE( ( _T( "RIscApi::PanicIfNotOpen 0x%x>" ), iChannelNumber ) );
    ASSERT_PANIC_ALWAYS( KNotInitializedChannel != iChannelNumber, EIADChannelNotOpen );
    C_TRACE( ( _T( "RIscApi::PanicIfNotOpen<" )) );

    //OstTrace0( TRACE_NORMAL, RISCAPI_PANICIFNOTOPEN_EXIT, "<RIscApi::PanicIfNotOpen" );
    }

//  End of File  
