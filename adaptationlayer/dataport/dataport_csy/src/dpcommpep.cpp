/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "dpdef.h"          // dataport definitions
#include "dpdataport.h"     // dataport main and c32 interface
#include "dpcommpep.h"      // comm pep interface. interaction with mif
#include "dpdataconfig.h"   // configuration store
#include "dpflowctrl.h"     // flow control handling
#include "dpbreak.h"        // break signal handling
#include "dpstruct.h"       // structures for common pep type
#include "dplog.h"          // dataport logging

#include "pep_comm_types.h" // structures for isi-message interface
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpcommpepTraces.h"
#endif

// LOCAL FUNCTION PROTOTYPES
// none

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDpCommPep::CDpCommPep
// C++ default constructor.
// ---------------------------------------------------------
CDpCommPep::CDpCommPep(
    CDpDataPort& aDataPort ) :
    iDataPort( aDataPort ),
    iDataConfig( iDataPort.DataConfig() ),
    iBreak( iDataPort.BreakHandler() ),
    iFlowCtrl( iDataPort.FlowCtrl() )
    {
    OstTrace0( TRACE_NORMAL, CDPCOMMPEP_CDPCOMMPEP, "CDpCommPep::CDpCommPep" );
    LOGM(" CDpCommPep::CDpCommPep");
    }

// ---------------------------------------------------------
// CDpCommPep::~CDpCommPep
// Destructor
// ---------------------------------------------------------
CDpCommPep::~CDpCommPep()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPCOMMPEP_CDPCOMMPEP, "CDpCommPep::~CDpCommPep" );
    LOGM(" CDpCommPep::~CDpCommPep");
    }

// ---------------------------------------------------------
// CDpCommPep::HandlePepCtrlReq
// This method handles PEP control requests, which are
// received from DCS. This method forwars handling to
// CDpDataConfig (data configuration).
// ---------------------------------------------------------
//
TInt CDpCommPep::HandlePepCtrlReq(
    const TUint8 aCtrlId,
    TDesC8& aCtrlData )
    {
    OstTrace0( TRACE_NORMAL, CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep::HandlePepCtrlReq" );
    LOGM(" CDpCommPep::HandlePepCtrlReq");

    TInt ret( KErrNone );

    // Handle control data here
    switch ( aCtrlId )
        {
        case PEP_COMM_CTRL_ID_PARAM:
            {
            LOG(" PEP_COMM_CTRL_ID_PARAM - CommPep Ctrl REQ");
            OstTrace0( TRACE_NORMAL, DUP1_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_PARAM - CommPep Ctrl REQ" );

            // We skip baudrate,data,stop,parity configurations from CMT.
            // CMT even should't send them.
            break;
            }
        case PEP_COMM_CTRL_ID_FLOW_CTRL:
            {
            LOG(" PEP_COMM_CTRL_ID_FLOW_CTRL - CommPep Ctrl REQ" );
            OstTrace0( TRACE_NORMAL, DUP2_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_FLOW_CTRL - CommPep Ctrl REQ" );

            // Configuring DTE interface flow control format
            PEP_COMM_CTRL_ID_FLOW_CTRL_STR& pepParam =
                *(PEP_COMM_CTRL_ID_FLOW_CTRL_STR*)aCtrlData.Ptr();

            // The most common error message here is KErrNotSupported. This
            // message is forwarded and handled where HandlePepCtrlReq was
            // called.
            ret = iDataConfig.SetFlowControlConfig(
                pepParam.inwards, pepParam.outwards, pepParam.extract);
            break;
            }
        case PEP_COMM_CTRL_ID_BREAK:
            {
            PEP_COMM_CTRL_ID_BREAK_STR& pepParam =
                *(PEP_COMM_CTRL_ID_BREAK_STR*)aCtrlData.Ptr();

            LOG1(" PEP_COMM_CTRL_ID_BREAK - CommPep Ctrl REQ, type: %d",
                pepParam.type);
            OstTraceExt1( TRACE_NORMAL, DUP3_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_BREAK - CommPep Ctrl REQ, type: %hhu", pepParam.type );

            iDataConfig.SetDteTypeOfBreakSignal( pepParam.type );
            break;
            }
        case PEP_COMM_CTRL_ID_AUTOBAUD:
            {
            LOG(" PEP_COMM_CTRL_ID_AUTOBAUD -  CommPep Ctrl REQ");
            OstTrace0( TRACE_NORMAL, DUP4_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_AUTOBAUD -  CommPep Ctrl REQ" );

            PEP_COMM_CTRL_ID_AUTOBAUD_STR& pepParam =
                *(PEP_COMM_CTRL_ID_AUTOBAUD_STR*)aCtrlData.Ptr();

            iDataConfig.SetDteParityBitWhen7Mode( pepParam.parity );
            iDataConfig.SetDteAutoBauding( pepParam.mode );
            break;
            }
        case PEP_COMM_CTRL_ID_ESCAPE:
            {
            LOG(" PEP_COMM_CTRL_ID_ESCAPE - CommPep Ctrl REQ");
            OstTrace0( TRACE_NORMAL, DUP5_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_ESCAPE - CommPep Ctrl REQ" );

            // Configuring DTE interface escape sequence
            PEP_COMM_CTRL_ID_ESCAPE_STR& pepParam =
                *(PEP_COMM_CTRL_ID_ESCAPE_STR*)aCtrlData.Ptr();

            // Set escape sequence parameters in DataConfig-object
            iDataConfig.SetEscChar( pepParam.character );
            iDataConfig.SetEscTime( pepParam.time );
            break;
            }
        case PEP_COMM_CTRL_ID_SEND_PNP:
            {
            LOG(" PEP_COMM_CTRL_ID_SEND_PNP - CommPep Ctrl REQ");
            OstTrace0( TRACE_NORMAL, DUP6_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_SEND_PNP - CommPep Ctrl REQ" );

            // With this control data a PEP is requested to send the contents
            // of the Plug and Play table to the DTE.
            break;
            }
        case PEP_COMM_CTRL_ID_DTR_STATUS:
            {
            LOG(" PEP_COMM_CTRL_ID_DTR_STATUS - CommPep Ctrl REQ");
            OstTrace0( TRACE_NORMAL, DUP7_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_DTR_STATUS - CommPep Ctrl REQ" );

            // With this control data the state of DTR signal can be asked from
            // Media PEP. this request is not supported.
            break;
            }
        case PEP_COMM_CTRL_ID_ECHO:
            {
            LOG(" PEP_COMM_CTRL_ID_ECHO - CommPep Ctrl REQ" );
            OstTrace0( TRACE_NORMAL, DUP8_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: PEP_COMM_CTRL_ID_ECHO - CommPep Ctrl REQ" );

            // With this control data a PEP is requested to set echoing of
            // characters received from DTE on or off.
            PEP_COMM_CTRL_ID_ECHO_STR& pepParam =
                *(PEP_COMM_CTRL_ID_ECHO_STR*)aCtrlData.Ptr();
            iDataConfig.SetEchoDte( pepParam.mode );
            break;
            }
        // Unknown control ID - return an error message.
        default:
            {
            LOG(" ERROR - Unknown CommPep Ctrl REQ" );
            OstTrace0( TRACE_NORMAL, DUP9_CDPCOMMPEP_HANDLEPEPCTRLREQ, "CDpCommPep:: ERROR - Unknown CommPep Ctrl REQ" );

            ret = KErrGeneral;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpCommPep::HandlePepStatusInd
// This method handles COMM type PEP status indications,
// which are received from DCS.
// ---------------------------------------------------------
//
TInt CDpCommPep::HandlePepStatusInd(
    const TUint8 aIndId,
    TDesC8& aIndData )
    {
    OstTrace0( TRACE_NORMAL, CDPCOMMPEP_HANDLEPEPSTATUSIND, "CDpCommPep::HandlePepStatusInd" );
    LOGM(" CDpCommPep::HandlePepStatusInd");

    TInt ret( KErrNone );

    // Handle control data here.
    switch ( aIndId )
        {
        case PEP_COMM_IND_ID_PARAM:
            {
            LOG(" PEP_COMM_IND_ID_PARAM - CommPep status indication");
            OstTrace0( TRACE_NORMAL, DUP1_CDPCOMMPEP_HANDLEPEPSTATUSIND, "CDpCommPep:: PEP_COMM_IND_ID_PARAM - CommPep status indication" );

            // DTE should not get these messages.
            break;
            }
        case PEP_COMM_IND_ID_FLOW_CTRL:
            {
            LOG(" PEP_COMM_IND_ID_FLOWCTRL - CommPep status indication");
            OstTrace0( TRACE_NORMAL, DUP2_CDPCOMMPEP_HANDLEPEPSTATUSIND, "CDpCommPep:: PEP_COMM_IND_ID_FLOWCTRL - CommPep status indication" );

            // DTE should not get these messages.
            break;
            }
        case PEP_COMM_IND_ID_V24_CHANGE:
            {
            LOG(" PEP_COMM_IND_ID_V24_CHANGE - CommPep status indication");
            OstTrace0( TRACE_NORMAL, DUP3_CDPCOMMPEP_HANDLEPEPSTATUSIND, "CDpCommPep:: PEP_COMM_IND_ID_V24_CHANGE - CommPep status indication" );

            // Signal state change
            ret = iDataConfig.MapSignalsCmt2Epoc( aIndData );

            // We must notify client when starting
            // i.e. DataPort is ready to receive data from DTE.
            iFlowCtrl.NotifyClientDatapotReadyforDteFlow();
            break;
            }
        case PEP_COMM_IND_ID_BREAK:
            {
            LOG(" PEP_COMM_IND_ID_BREAK - CommPep status indication" );
            OstTrace0( TRACE_NORMAL, DUP4_CDPCOMMPEP_HANDLEPEPSTATUSIND, "CDpCommPep:: PEP_COMM_IND_ID_BREAK - CommPep status indication" );

            // Set BREAK towards DTE
            TBool notifyValue;

            // Find out should we notify client
            iBreak.GetBreakNotify( notifyValue );

            if ( notifyValue )
                {
                // Notify client, there is no way to tell signal length
                // to client so dump pepParam.signalLength here.
                LOG("  CDpCommPep::HandlePepStatusInd, Complete break notify");
                OstTrace0( TRACE_NORMAL, DUP5_CDPCOMMPEP_HANDLEPEPSTATUSIND, "CDpCommPep:: HandlePepStatusInd, Complete break notify" );

                iBreak.CompleteBreakNotify();
                }
            break;
            }
        default:
            {
            LOG1(" ERROR - Unknown CommPep status indication: %d", aIndId );
            OstTraceExt1( TRACE_NORMAL, DUP6_CDPCOMMPEP_HANDLEPEPSTATUSIND, "CDpCommPep:: ERROR - Unknown CommPep status indication: %hhu", aIndId );

            ret = KErrGeneral;
            }
        }

    return ret;
    }


// ================= OTHER EXPORTED FUNCTIONS ==============
// none

//  End of File
