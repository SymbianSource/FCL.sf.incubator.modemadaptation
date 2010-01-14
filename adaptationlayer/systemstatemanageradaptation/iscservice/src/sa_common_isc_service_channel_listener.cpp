/*
* ==============================================================================
*  Name:           sa_common_isc_service_channel_listener.cpp
*  Part of:        System adaptation
*  Description:    Implementation of CSACommonIscServiceChannelListener class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:59:35 2009 %
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
* Implementation of CSACommonIscServiceChannelListener class
*
*/


// INCLUDE FILES
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE 
#include "sa_common_isc_service_channel_listenerTraces.h"
#endif
#include <iscapi.h>
#include <iscnokiadefinitions.h>    // For NokiaISC
#include "sa_common_trace.h"
#include "sa_common_isc_service_channel_listener.h"
#include "msa_isc_service.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSACommonIscServiceChannelListener::CSACommonIscServiceChannelListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSACommonIscServiceChannelListener::CSACommonIscServiceChannelListener(
    MSAIscService& aAdaptation,
    RIscApi& aIscApi )
    : CActive( CActive::EPriorityStandard )
    , iAdaptation( aAdaptation )
    , iIscApi( aIscApi )
    {
    OstTraceExt2( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_1,
        "ISCSERVICE - CSACommonIscServiceChannelListener::CSACommonIscServiceChannelListener(0x%x,0x%x)",
        (TUint)&aAdaptation, (TUint)&aIscApi );
    INTERNAL_TRACE( ( 
        _L( "ISCSERVICE - CSACommonIscServiceChannelListener::CSACommonIscServiceChannelListener(0x%x,0x%x)" ), 
        &aAdaptation, &aIscApi ) );

    CActiveScheduler::Add( this );

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_2,
        "ISCSERVICE - CSACommonIscServiceChannelListener::CSACommonIscServiceChannelListener - return void" );
    INTERNAL_TRACE( ( 
        _L( "ISCSERVICE - CSACommonIscServiceChannelListener::CSACommonIscServiceChannelListener - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CSACommonIscServiceChannelListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSACommonIscServiceChannelListener::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_3,
        "ISCSERVICE - CSACommonIscServiceChannelListener::ConstructL()" );
    INTERNAL_TRACE( ( 
        _L( "ISCSERVICE - CSACommonIscServiceChannelListener::ConstructL()" ) ) );

    iIscApi.NotifyConnectionStatus( iStatus, iConnectionStatus );
    SetActive();

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_4,
        "ISCSERVICE - CSACommonIscServiceChannelListener::ConstructL - return void" );
    INTERNAL_TRACE( ( 
        _L( "ISCSERVICE - CSACommonIscServiceChannelListener::ConstructL - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CSACommonIscServiceChannelListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSACommonIscServiceChannelListener* CSACommonIscServiceChannelListener::NewL(
    MSAIscService& aAdaptation, 
    RIscApi& aIscApi)
    {
    OstTraceExt2( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_5,
        "ISCSERVICE - CSACommonIscServiceChannelListener::NewL(0x%x,0x%x)",
        (TUint)&aAdaptation, (TUint)&aIscApi );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::NewL(0x%x,0x%x)" ),
        &aAdaptation, &aIscApi ) );

    CSACommonIscServiceChannelListener* self = new( ELeave ) CSACommonIscServiceChannelListener( 
        aAdaptation, 
        aIscApi );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_6,
        "ISCSERVICE - CSACommonIscServiceChannelListener::NewL - return 0x%x", self );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::NewL - return 0x%x" ),
        self ) );

    return self;
    }


// -----------------------------------------------------------------------------
// CSACommonIscServiceChannelListener::~CSACommonIscServiceChannelListener
// Destructor.
// -----------------------------------------------------------------------------
//
CSACommonIscServiceChannelListener::~CSACommonIscServiceChannelListener()
    {
    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_7, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::~CSACommonIscServiceChannelListener()" );
    INTERNAL_TRACE( ( 
        _L( "ISCSERVICE - CSACommonIscServiceChannelListener::~CSACommonIscServiceChannelListener()" ) ) );

    /* Cancel outstanding request */
    Cancel();

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_8, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::~CSACommonIscServiceChannelListener - return void" );
    INTERNAL_TRACE( ( 
        _L( "ISCSERVICE - CSACommonIscServiceChannelListener::~CSACommonIscServiceChannelListener - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CSACommonIscServiceChannelListener::RunL
// This function is called by Kernel when ISC API completes outstanding request
// -----------------------------------------------------------------------------
//
void CSACommonIscServiceChannelListener::RunL()
    {
    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_9, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::RunL()" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunL()" ) ) );

    ASSERT_TRACE( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunL" ),
        ( iStatus.Int() == KErrNone ) );
    ASSERT_TRACE( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunL" ),
        ( iConnectionStatus == EIscConnectionNotOk ) );

    if(iConnectionStatus != EIscConnectionOk)
        {
        iAdaptation.ProcessConnectionClosedL( iConnectionStatus );
        INTERNAL_TRACE( ( 
            _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunL - Isc channel closed!" ) ) );
        }

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_10, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::RunL - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunL - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CSACommonIscServiceChannelListener::RunError
// This function is called if RunL leaves
// -----------------------------------------------------------------------------
//
TInt CSACommonIscServiceChannelListener::RunError( TInt aError )
    {
    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_11, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::RunError(ox%x)", aError );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunError(ox%x)" ), 
        aError ) );

    aError = aError;

    ASSERT_ALWAYS_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunError" ) ) );

    OstTrace1( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_12, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::RunL - return 0x%x", KErrNone );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::RunL - return 0x%x" ), 
        KErrNone ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSACommonIscServiceChannelListener::DoCancel
// This function is called from CActive objects Cancel method.
// -----------------------------------------------------------------------------
//
void CSACommonIscServiceChannelListener::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_13, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::DoCancel()" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::DoCancel()" ) ) );

    iIscApi.NotifyConnectionStatusCancel();

    OstTrace0( TRACE_NORMAL, SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_AUTOMATED_14, 
        "ISCSERVICE - CSACommonIscServiceChannelListener::DoCancel - return void" );
    INTERNAL_TRACE( ( _L( "ISCSERVICE - CSACommonIscServiceChannelListener::DoCancel - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
