/*
* ==============================================================================
*  Name:           sa_common_isc_service.cpp
*  Part of:        System adaptation
*  Description:    Implementation of CSACommonIscService class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:59:32 2009 %
*
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
* Implementation of CSACommonIscService class
*
*/


// INCLUDE FILES
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE 
#include "sa_common_isc_serviceTraces.h"
#endif
#include <iscapi.h>
#include <iscnokiadefinitions.h>    // For NokiaISC
#include "sa_common_isc_service.h"
#include "sa_common_isc_service_channel_listener.h"
#include "sa_common_trace.h"
#include "msa_isc_service.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// Minimum received message length
// ( = Phonet header + ISI transaction id + ISI message id)
const TUint8 KSASMinIscMessageLength = 10;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSACommonIscService::CSACommonIscService
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSACommonIscService::CSACommonIscService( 
    MSAIscService& aAdaptation,
    const TUint16 aChannelNumber,
    const TUint16 aClientISIMessageSize )
    : CActive( CActive::EPriorityStandard )
    , iAdaptation( aAdaptation )
    , iChannelNumber( aChannelNumber )
    , iClientISIMessageSize( aClientISIMessageSize )
    , iMessageLength( 0 )
    , iBuffer( NULL )
    , iBufferPtr( NULL, 0 )
    {
    OstTraceExt3( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_1, 
        "ISCSERVICE # CSACommonIscService::CSACommonIscService(0x%x,0x%x,0x%x)", 
        (TUint)&aAdaptation, (TUint)aChannelNumber, (TUint)aClientISIMessageSize );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::CSACommonIscService(0x%x,0x%x,0x%x)" ), 
        &aAdaptation, aChannelNumber, aClientISIMessageSize ) );

    CActiveScheduler::Add( this );

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_2, 
        "ISCSERVICE # CSACommonIscService::CSACommonIscService - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::CSACommonIscService - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSACommonIscService::ConstructL( TDes8* aEvents )
    {
    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_3, 
        "ISCSERVICE # CSACommonIscService::ConstructL(0x%x)", aEvents );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::ConstructL(0x%x)" ), aEvents ) );

    TRequestStatus status;

    /* Open ISC channel */
    OstTrace0( TRACE_API, SA_COMMON_ISC_SERVICE_AUTOMATED_4, 
        "ISCSERVICE - CSACommonIscService::ConstructL() - Opening ISC channel" );
    API_TRACE( ( _L( "ISCSERVICE - CSACommonIscService::ConstructL() - Opening ISC channel" ) ) );
    iIscApi.Open( iChannelNumber, status );
    User::WaitForRequest( status );

    if ( status.Int() != KErrNone )
        {
        ASSERT_ALWAYS_TRACE( _L( "ISCSERVICE - CSACommonIscService::ConstructL() - Failed to open ISC channel!") );
        User::LeaveIfError( status.Int() );
        }

    if(aEvents)
        {
        /* Subscribe reception of events via ISC channel */
        OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_5, 
            "ISCSERVICE - CSACommonIscService::ConstructL() - Subscribe events" );
        INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscService::ConstructL() - Subscribe events" ) ) );
        SubscribeEventsL( aEvents );
        }

    /* Start receiving messages via ISC channel */
    ReceiveMessageL( iClientISIMessageSize );

    /* Create ISC channel listener to monitor the state of ISC channel */
    iSACommonIscServiceChannelListener = CSACommonIscServiceChannelListener::NewL( iAdaptation, iIscApi );

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_6, 
        "ISCSERVICE # CSACommonIscService::ConstructL - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::ConstructL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSACommonIscService* CSACommonIscService::NewL( MSAIscService& aAdaptation,
    const TUint16 aChannelNumber,
    TDes8* aEvents,
    const TUint16 aClientISIMessageSize )
    {
    OstTraceExt4( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_7, 
        "ISCSERVICE # CSACommonIscService::NewL(0x%x,0x%x,0x%x,0x%x)", 
        (TUint)&aAdaptation, (TUint)aChannelNumber, (TUint)aEvents, (TUint)aClientISIMessageSize );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::NewL(0x%x,0x%x,0x%x,0x%x)" ), 
        &aAdaptation, aChannelNumber, aEvents, aClientISIMessageSize ) );

    CSACommonIscService* self = new( ELeave ) CSACommonIscService( 
        aAdaptation, 
        aChannelNumber, 
        aClientISIMessageSize );

    CleanupStack::PushL( self );
    self->ConstructL( aEvents );
    CleanupStack::Pop( self );

    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_8, 
        "ISCSERVICE # CSACommonIscService::NewL - return 0x%x", self );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::NewL - return 0x%x" ), self ) );

    return self;
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::~CSACommonIscService
// Destructor.
// -----------------------------------------------------------------------------
//
CSACommonIscService::~CSACommonIscService()
    {
    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_9, 
        "ISCSERVICE # CSACommonIscService::~CSACommonIscService()" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::~CSACommonIscService()" ) ) );

    /* Cancel any outstanding request */
    Cancel();

    /* Delete message buffer */
    delete iBuffer;
    iBuffer = NULL;

    /* Cancel monitoring ISC channel state */
    delete iSACommonIscServiceChannelListener;
    iSACommonIscServiceChannelListener = NULL;

    /* Close ISC channel */
    OstTrace0( TRACE_API, SA_COMMON_ISC_SERVICE_AUTOMATED_10, 
        "ISCSERVICE - CSACommonIscService::~CSACommonIscService - Close ISC channel" );
    API_TRACE( ( _L( "ISCSERVICE - CSACommonIscService::~CSACommonIscService - Close ISC channel" ) ) );
    iIscApi.Close();

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_11, 
        "ISCSERVICE # CSACommonIscService::~CSACommonIscService - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::~CSACommonIscService - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::SendMessageL
// This function is used to send a message via ISC channel
// -----------------------------------------------------------------------------
//
void CSACommonIscService::SendMessageL(
    TDesC8& aMessage )
    {
    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_12, 
        "ISCSERVICE # CSACommonIscService::SendMessageL(0x%x)", (TUint)&aMessage );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::SendMessageL(0x%x)" ), (TUint)&aMessage ) );

    OstTrace1( TRACE_API, SA_COMMON_ISC_SERVICE_AUTOMATED_13, 
        "ISCSERVICE - CSACommonIscService::SendMessageL() - Sending ISI Message;aData=%x",
        (TUint)&aMessage );
    ISIMSG_API_TRACE( _L("ISCSERVICE # CSACommonIscService::SendMessageL() - Sending ISI Message: "), aMessage);

    User::LeaveIfError( iIscApi.Send( aMessage ) );

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_14, 
        "ISCSERVICE # CSACommonIscService::SendMessageL - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::SendMessageL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::ReceiveMessageL
// This function starts receiving messages via ISC channel
// -----------------------------------------------------------------------------
//
void CSACommonIscService::ReceiveMessageL( TUint16 aMessageLength )
    {
    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_16, 
        "ISCSERVICE # CSACommonIscService::ReceiveMessageL(0x%x)", aMessageLength );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::ReceiveMessageL(0x%x)" ), 
        aMessageLength ) );

    if ( !IsActive() )
        {

        OstTrace0( TRACE_API, SA_COMMON_ISC_SERVICE_AUTOMATED_17, 
            "ISCSERVICE - CSACommonIscService::ReceiveMessageL() - Receiving message" );
        API_TRACE( ( _L( "ISCSERVICE - CSACommonIscService::ReceiveMessageL() - Receiving message" ) ) );

        /* Reserve space for the received message buffer */
        delete iBuffer;
        iBuffer = NULL;
        iBuffer = HBufC8::NewL( aMessageLength );
        iBufferPtr.Set( iBuffer->Des() );

        /* Start receiving messages via ISC channel */
        iMessageLength = 0;
        iIscApi.Receive( iStatus, iBufferPtr, iMessageLength );
        SetActive();
        }
    else
        {
        /* Receiving request pending */
        ASSERT_ALWAYS_TRACE( _L( "ISCSERVICE - CSACommonIscService::ReceiveMessageL() - Request pending") );
        }

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_18, 
        "ISCSERVICE # CSACommonIscService::ReceiveMessageL - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::ReceiveMessageL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::RunL
// This function is called when a message is received via ISC channel
// -----------------------------------------------------------------------------
//
void CSACommonIscService::RunL()
    {
    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_19, 
        "ISCSERVICE # CSACommonIscService::RunL()" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::RunL()" ) ) );

    TInt err( KErrNone );
    TUint16 length( iClientISIMessageSize );

    switch ( iStatus.Int() )
        {
        case KErrNone:
            if ( iBufferPtr.Length() >= KSASMinIscMessageLength )
                {
                /* Forward receive message to System Adaptation proxy */
                OstTrace1( TRACE_API, SA_COMMON_ISC_SERVICE_AUTOMATED_20, 
                    "ISCSERVICE - CSACommonIscService::RunL() - Receive ISI Message;iBufferPtr=%x", 
                    (TUint)&(iBufferPtr) );
                ISIMSG_API_TRACE( _L("ISCSERVICE - CSACommonIscService::RunL() - Receive ISI Message: " ), 
                    iBufferPtr);

                TRAP( err, iAdaptation.ReceiveIsiMessageL( iBufferPtr ) );
                ASSERT_TRACE( _L( "ISCSERVICE - CSACommonIscService::RunL() - Processing message failed" ), 
                    ( err == KErrNone ) );
                }
            else
                {
                ASSERT_ALWAYS_TRACE( _L( "ISCSERVICE - CSACommonIscService::RunL() - Invalid message" ) );
                }
            break;

        case KErrNoMemory:
            OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_21, 
                "ISCSERVICE # CSACommonIscService::RunL() - Reallocate message buffer" );
            INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::RunL() - Reallocate message buffer" ) ) );
            /* Reserved message buffer overflow -
               Reallocate message buffer and receive the message again
               ( iMessageLength contains the required buffer length for the message ) */
            length = iMessageLength;
            break;

        default:
            /* Other errors are ignored - Continue receiving messages via ISC channel */
            ASSERT_ALWAYS_TRACE( _L( "ISCSERVICE - CSACommonIscService::RunL() - Other error" ) );
            break;
        }

    /* Continue receiving messages via ISC channel */
    TRAP( err, ReceiveMessageL( length ) );

    if ( err != KErrNone )
        {
        ASSERT_ALWAYS_TRACE( _L( "ISCSERVICE - CSACommonIscService::RunL() - Memory allocation failed" ) );
        User::Panic( _L( "ISCSERVICE - CSACommonIscService" ), err );
        }

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_22, 
        "ISCSERVICE # CSACommonIscService::RunL - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::RunL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::DoCancel
// This function cancels the receiving messages via ISC channel
// -----------------------------------------------------------------------------
//
void CSACommonIscService::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_23, 
        "ISCSERVICE # CSACommonIscService::DoCancel()" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::DoCancel()" ) ) );

    iIscApi.ReceiveCancel();

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_24, 
        "ISCSERVICE # CSACommonIscService::DoCancel - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::DoCancel - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscService::SubscribeEvents
// This function subscribes reception of events via ISC channel
// -----------------------------------------------------------------------------
//
void CSACommonIscService::SubscribeEventsL( TDes8 *aEvents )
    {
    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_25, 
        "ISCSERVICE # CSACommonIscService::SubscribeEventsL(0x%x)", aEvents );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::SubscribeEventsL(0x%x)" ), aEvents ) );

    User::LeaveIfError( iIscApi.CustomFunction( EIscNokiaEventSubscribe, aEvents ) );

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_AUTOMATED_26, 
        "ISCSERVICE # CSACommonIscService::SubscribeEventsL - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE # CSACommonIscService::SubscribeEventsL - return void" ) ) );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
