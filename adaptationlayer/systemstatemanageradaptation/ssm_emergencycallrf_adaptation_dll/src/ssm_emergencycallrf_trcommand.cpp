/*
* ==============================================================================
*  Name:           ssm_emergencycallrf_trcommand.cpp
*  Part of:        SSMA EmergencyCallRfAdaptation
*  Interface:      N/A
*  Description:    Implementation of CEmergencyCallRfTrCommand class
*  %version:       2 %
*  %date_modified: Thu Feb 04 15:16:42 2010 %
*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution
*
* Contributors:
*
* Description:
* Implementation of ssm_emergencycallrf_adaptation
*/
// INCLUDE FILES
#include "OstTraceDefinitions.h" 
#ifdef OST_TRACE_COMPILER_IN_USE 
#include "ssm_emergencycallrf_trcommandtraces.h"
#endif
#include <ssm/startupadaptationcommands.h>
#include <mceisi.h>
#include <tisi.h>
#include <pn_const.h>
#include "ssm_emergencycallrf_trcommand.h"
#include "sa_common_isc_service.h"
#include "sa_common_trace.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEmergencyCallRfAdaptation;

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::TransactionId
// -----------------------------------------------------------------------------
//
TUint8 CEmergencyCallRfTrCommand::TransactionId() const
    {
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_TRANSACTIONID_1,
               "SSMA # CEmergencyCallRfTrCommand::TransactionId - return 0x%x",
               iTransactionId );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::TransactionId - return 0x%x"),
                       iTransactionId));

    return iTransactionId;
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::TransactionState
// -----------------------------------------------------------------------------
//
TUint8 CEmergencyCallRfTrCommand::TransactionState() const
    {
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_TRANSACTIONSTATE_1,
               "SSMA # CEmergencyCallRfTrCommand::iTransactionState - return 0x%02x",
               iTransactionState );
    INTERNAL_TRACE ((_L("SSMA # CEmergencyCallRfTrCommand::iTransactionState - return 0x%02x"),
                        iTransactionState));

    return iTransactionState;
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::NewL
// -----------------------------------------------------------------------------
//
CEmergencyCallRfTrCommand* CEmergencyCallRfTrCommand::NewL( CEmergencyCallRfAdaptation &aAdaptation,
                                                            TUint8 aTransactionId )
    {
    OstTraceExt2( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_NEWL_1,
                  "CEmergencyCallRfTrCommand::NewL(0x%x, %d)",
                  (TUint)&aAdaptation, aTransactionId);
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::NewL(0x%x, %d)"),
                        &aAdaptation, aTransactionId));

    CEmergencyCallRfTrCommand* self = new(ELeave) CEmergencyCallRfTrCommand( aAdaptation, aTransactionId);
        
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_NEWL_2,
               "SSMA # CEmergencyCallRfTrCommand::NewL() - return 0x%x",
               self );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::NewL() - return 0x%x"),
                       self));

    return self;
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::CEmergencyCallRfTrCommand
// -----------------------------------------------------------------------------
//
CEmergencyCallRfTrCommand::CEmergencyCallRfTrCommand( CEmergencyCallRfAdaptation &aAdaptation,
                                                      TUint8 aTransactionId )
    : iAdaptation(aAdaptation)
    , iTransactionId( aTransactionId )
    , iTransactionState( EMERGENCYCALLRF_TR_INIT )
    {
    OstTraceExt2( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_CEMERGENCYCALLRFTRCOMMAND_1,
                  "SSMA # CEmergencyCallRfTrCommand::CEmergencyCallRfTrCommand(0x%x ,%d)",
                  (TUint)&aAdaptation, aTransactionId );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::CEmergencyCallRfTrCommand(0x%x ,%d)"),
                       &aAdaptation, aTransactionId ));

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_CEMERGENCYCALLRFTRCOMMAND_2,
               "SSMA # CEmergencyCallRfTrCommand::CEmergencyCallRfTrCommand - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::CEmergencyCallRfTrCommand - return void")));
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::~CEmergencyCallRfTrCommand
// -----------------------------------------------------------------------------
//
CEmergencyCallRfTrCommand::~CEmergencyCallRfTrCommand()
    {
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_CEMERGENCYCALLRFTRCOMMAND_D1,
               "SSMA # CEmergencyCallRfTrCommand::~CEmergencyCallRfTrCommand - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::~CEmergencyCallRfTrCommand - return void")));
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::ProcessMessageL  
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfTrCommand::ProcessMessageL( const TInt aCommandId, TDesC8 &aData )
    {
    OstTraceExt2( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_PROCESSMESSAGE_1,
               "SSMA # CEmergencyCallRfTrCommand::ProcessMessageL(0x%x, 0x%x)",
               aCommandId, (TUint)&aData );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ProcessMessageL(0x%x,0x%x)"),
                       aCommandId, (TUint)&aData ));

    switch ( iTransactionState )
        {
        case EMERGENCYCALLRF_TR_INIT:
            {
            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_PROCESSMESSAGE_2,
                       "SSMA # CEmergencyCallRfTrCommand::ProcessMessageL - EMERGENCYCALLRF_TR_INIT" );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ProcessMessageL - EMERGENCYCALLRF_TR_INIT")));

            // This is the new transaction, so update the state value
            iTransactionState = EMERGENCYCALLRF_TR_ONGOING;

            switch (aCommandId)
                {
                case StartupAdaptation::EActivateRfForEmergencyCall:
                    {
                    //Build up the isi message
                    MCE_RF_STATE_REQL( MCE_RF_ON );
                    }
                    break;

                case StartupAdaptation::EDeactivateRfAfterEmergencyCall:
                    {
                    //Build up the isi message
                    MCE_RF_STATE_REQL( MCE_RF_OFF );
                    }
                    break;

                default:
                    {
                    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_PROCESSMESSAGE_3,
                               "SSMA - CEmergencyCallRfTrCommand::ProcessMessageL - Wrong command->Transaction failed" );
                    ASSERT_ALWAYS_TRACE((_L("SSMA - CEmergencyCallRfTrCommand::ProcessMessageL - Wrong command->Transaction failed")));
                    iTransactionState = EMERGENCYCALLRF_TR_FAILED;
                    User::Leave( KErrGeneral );
                    }
                    break;
                }
            }
            break;

        case EMERGENCYCALLRF_TR_ONGOING:
            {
            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_PROCESSMESSAGE_4,
                       "SSMA # CEmergencyCallRfTrCommand::ProcessMessageL - EMERGENCYCALLRF_TR_ONGOING" );
            INTERNAL_TRACE((_L("SSMA - CEmergencyCallRfTrCommand::ProcessMessageL - EMERGENCYCALLRF_TR_ONGOING")));

            //Parse the received response
            ParseMCE_RF_STATE_RESPL( aData );
            }
            break; 

        default:
            {
            ASSERT_ALWAYS_TRACE((_L("CEmergencyCallRfTrCommand::ProcessMessageL - Wrong state->Transaction failed")));
            iTransactionState = EMERGENCYCALLRF_TR_FAILED;
            User::Leave( KErrGeneral );
            break;
            }
        }

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_PROCESSMESSAGE_5,
               "SSMA # CEmergencyCallRfTrCommand::ProcessMessageL - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ProcessMessageL - return void")));
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::MCE_RF_STATE_REQL
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfTrCommand::MCE_RF_STATE_REQL( TUint8 aRequestInfo )
    {
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_REQL_1,
               "SSMA # CEmergencyCallRfTrCommand::MCE_RF_STATE_REQL(0x%x)", aRequestInfo );
    INTERNAL_TRACE((_L( "SSMA # CEmergencyCallRfTrCommand::MCE_RF_STATE_REQL(0x%x)"), aRequestInfo));

    /* Create MCE_RF_STATE_REQ message */
    TBuf8<SIZE_MCE_RF_STATE_REQ> msgData;

    msgData.Append( iTransactionId );                   // Transaction Id
    msgData.Append( MCE_RF_STATE_REQ );                 // Message Id
    msgData.Append( aRequestInfo );                     // Request info high bits
    msgData.Append( 0x00 );                             // Filler

    /* Send ISI message */
    iAdaptation.SendIsiMessageL( msgData );

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_REQL_3,
               "SSMA # CEmergencyCallRfTrCommand::MCE_RF_STATE_REQL - return void" );
    INTERNAL_TRACE((_L( "SSMA # CEmergencyCallRfTrCommand::MCE_RF_STATE_REQL - return void")));
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL
// ----------------------------------------------------------------------------- 
void  CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL( TDesC8 &aData )
    {
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_1,
               "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - aData is: (%x)",
               &aData );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - aData is: (%x)"),
                        &aData));

    TInt   errorCode(KErrNone);
    TUint8 status( 0 );

    TIsiReceiveC isiMsg( aData );

    // start parsing the message

    //Read status
    status = isiMsg.Get8bit( ISI_HEADER_SIZE + 
                              MCE_RF_STATE_RESP_OFFSET_STATUS );

    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_2,
               "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status(%d)",
               status );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status(%d)"),
                        status));

    switch( status )
        {
        case MCE_OK: // 0
            { 
            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_3,
                       "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_OK" );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_OK")));
            errorCode = KErrNone;
            iTransactionState = EMERGENCYCALLRF_TR_COMPLETED;
            }
            break;

        case MCE_FAIL: // 0x01
            {
            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_4,
                       "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_FAIL" );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_FAIL")));
            errorCode = KErrNotFound;
            iTransactionState = EMERGENCYCALLRF_TR_FAILED;
            }
            break;

        case MCE_ALREADY_ACTIVE: // 0x06
            {
            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_5,
                       "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_ALREADY_ACTIVE" );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_ALREADY_ACTIVE")));
            errorCode = KErrArgument;
            iTransactionState = EMERGENCYCALLRF_TR_FAILED;
            }
            break;

        case MCE_TRANSITION_ONGOING: // 0x16
            {
            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_6,
                       "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_TRANSITION_ONGOING" );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_TRANSITION_ONGOING")));
            errorCode = KErrServerBusy;
            iTransactionState = EMERGENCYCALLRF_TR_FAILED;
            }
            break;

        case MCE_RESTART_REQUIRED: // 0x17
            {
            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_7,
                       "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_RESTART_REQUIRED" );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status = MCE_RESTART_REQUIRED")));
            errorCode = KErrCompletion;
            iTransactionState = EMERGENCYCALLRF_TR_FAILED;
            }
            break;

        default:
            {
            ASSERT_ALWAYS_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - ")));
            OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_8,
                       "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status(%d)", 
                        status );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL() - Status(%d)"),
                                status));
            }
            break;
        }

    iAdaptation.ProcessResponseL( errorCode );

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFTRCOMMAND_MCE_RF_STATE_RESP_9,
               "SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL - return " );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfTrCommand::ParseMCE_RF_STATE_RESPL - return ")));
    }
    
// End of file

