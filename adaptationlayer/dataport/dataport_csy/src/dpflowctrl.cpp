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
#include "dpflowctrl.h"     // flow control handling
#include "dppif.h"          // dcs pipe interface
#include <iscdefinitions.h>
#include "dplog.h"          // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpflowctrlTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES
// none

// CONSTANTS
// none

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================
// none

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDpFlowCtrl::CDpFlowCtrl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDpFlowCtrl::CDpFlowCtrl(
    CDpDataPort& aDataPort ) :
    CActive( KDpFlowCtrlPriority ),
    iDataPort( aDataPort ),
    iPifDcs( iDataPort.Pif() ),
    iFlowCtrlDp2Dcs( EFlowControlOn ),
    iFlowCtrlDcs2Dp( EFlowControlOff ),
    iLastFlowControlSentToClient( EFlowControlOn ),
    iIsDataportReadyforDteFlow( EFalse )
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_CDPFLOWCTRL, "CDpFlowCtrl::CDpFlowCtrl" );
    LOGM(" CDpFlowCtrl::CDpFlowCtrl");

    iIscFlowCtrlStatus = EIscFlowControlOn;

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::~CDpFlowCtrl
// Destructor
// -----------------------------------------------------------------------------
CDpFlowCtrl::~CDpFlowCtrl()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPFLOWCTRL_CDPFLOWCTRL, "CDpFlowCtrl::~CDpFlowCtrl" );
    LOGM(" CDpFlowCtrl::~CDpFlowCtrl");
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::RunL
// This method sets water mark according Isc Api' proxy's flow
// control status, if Dcs2Dp flow control status is not already set.
// Isc Api pipe data message buffer request is renewed.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::RunL()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_RUNL, "CDpFlowCtrl::RunL" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPFLOWCTRL_RUNL, "CDpFlowCtrl:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpFlowCtrl::RunL - Port %d", iDataPort.PortUnit() );

    switch ( iIscFlowCtrlStatus )
        {
        // Dataport's Proxy's pipe data message buffer has reached HighWaterMark
        case EIscFlowControlOn:
            {
            LOG(" CDpFlowCtrl::RunL, Proxy's High WaterMarkReached");
            OstTrace0( TRACE_NORMAL, DUP2_CDPFLOWCTRL_RUNL, "CDpFlowCtrl:: Proxy's High WaterMarkReached" );

            iFlowCtrlDp2Dcs = EFlowControlOn;
            break;
            }
        // Dataport's Proxy's pipe data message buffer has reached LowWaterMark
        case EIscFlowControlOff:
            {
            LOG(" CDpFlowCtrl::RunL, Proxy's Low WaterMarkReached");
            OstTrace0( TRACE_NORMAL, DUP3_CDPFLOWCTRL_RUNL, "CDpFlowCtrl:: Proxy's Low WaterMarkReached" );

            iFlowCtrlDp2Dcs = EFlowControlOff;
            iPifDcs.SetPipeState( CDpPif::EDpPipeEnabled );
            break;
            }
        // Connection closed, data transmission ended
        case EIscTransmissionEnd:
            {
            LOG(" CDpFlowCtrl::RunL, Transmission End");
            OstTrace0( TRACE_NORMAL, DUP4_CDPFLOWCTRL_RUNL, "CDpFlowCtrl:: Transmission End" );

            // set flow control on
            iFlowCtrlDp2Dcs = EFlowControlOn;
            iFlowCtrlDcs2Dp = EFlowControlOn;
            iPifDcs.SetPipeState( CDpPif::EDpPipeDisconnected );
            iDataPort.ISAHandle().ResetBuffers();
            break;
            }
        default:
            {
            // This should newer happen, No actions needed.
            LOG(" CDpFlowCtrl::RunL, Proxy buffer reached HWM or LWM, but no action needed");
            OstTrace0( TRACE_NORMAL, DUP5_CDPFLOWCTRL_RUNL, "CDpFlowCtrl:: Proxy buffer reached HWM or LWM, but no action needed" );

            }
        }

    // inform client
    if ( iFlowControlNotify )
        {
        InformFlowControlToClient();
        }
    // no else

    // inform other modules
    Notify();

    if ( EIscTransmissionEnd != iIscFlowCtrlStatus )
        {
        // renew flow control request
        RequestIscFlowControlNotification();
        }
    else
        {
        // Check if DataPort is ready to be deleted
        if ( iDataPort.IsReadyToDestruct() )
            {
            iDataPort.DeleteDataPort();
            }
        else
            {
            RequestIscFlowControlNotification();
            }
        //no else
        }
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::RunError
// Leave in RunL() is handled here. Error code is returned,
// when internal error has occured.
// -----------------------------------------------------------------------------
//
TInt CDpFlowCtrl::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_RUNERROR, "CDpFlowCtrl::RunError" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPFLOWCTRL_RUNERROR, "CDpFlowCtrl:: Port: %u, error code: %d", iDataPort.PortUnit(), aError );

    LOGM2("CDpFlowCtrl::RunError - Port %d, error code: %d",
        iDataPort.PortUnit(), aError );

    return aError;
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::RequestIscFlowControlNotification
// This method requests Isc Api pipe data message buffer status.
// -----------------------------------------------------------------------------
//

void CDpFlowCtrl::RequestIscFlowControlNotification()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_REQUESTISCFLOWCONTROLNOTIFICATION, "CDpFlowCtrl::RequestIscFlowControlNotification" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPFLOWCTRL_REQUESTISCFLOWCONTROLNOTIFICATION, "CDpFlowCtrl:: Port: %u", iDataPort.PortUnit() );

    LOGM1(" CDpFlowCtrl::RequestIscFlowControlNotification - Port %d",
        iDataPort.PortUnit() );

    if ( !IsActive() )
        {
        iDataPort.ISAHandle().NotifyFlowControl(
            iStatus, iIscFlowCtrlStatus );
        SetActive();
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::DoCancel
// This method cancels this active object by RequestComplete().
// Must be called directly (not through Cancel()), when RunL()
// should be completed with KErrCancel.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_DOCANCEL, "CDpFlowCtrl::DoCancel" );
    LOG(" CDpFlowCtrl::DoCancel");

    // do the required signaling
    iDataPort.ISAHandle().NotifyFlowControlCancel();
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::GetDp2DcsFlowControlStatus
// Gets flowcontrol status of DP-->DCS
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::GetDp2DcsFlowControlStatus(
    TFlowControl& aFlowControl )
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_GETDP2DCSFLOWCONTROLSTATUS, "CDpFlowCtrl::GetDp2DcsFlowControlStatus" );
    LOGM(" CDpFlowCtrl::GetDp2DcsFlowControlStatus");

    aFlowControl = iFlowCtrlDp2Dcs;
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::SetFlowControlNotify
// Set the flow control notify enabled/disabled.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::SetFlowControlNotify(
    const TBool aEnableNotify )
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_SETFLOWCONTROLNOTIFY, "CDpFlowCtrl::SetFlowControlNotify" );
    LOGM(" CDpFlowCtrl::SetFlowControlNotify");

    iFlowControlNotify = aEnableNotify;
    // new flow control message is received while notification was not active.
    if ( aEnableNotify && ( iLastFlowControlSentToClient != iFlowCtrlDp2Dcs ) )
        {
        InformFlowControlToClient();
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::WaterMarkLowReached
// RX buffer or Isc Api's proxy has reached low water mark.
// Sets the local FlowCtrlDcs2Dp flow OFF and sends flow
// change to DCS, if needed. Notify flow control users
// (flow control observers).
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::WaterMarkLowReached()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_WATERMARKLOWREACHED, "CDpFlowCtrl::WaterMarkLowReached" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPFLOWCTRL_WATERMARKLOWREACHED, "CDpFlowCtrl:: Port: %u", iDataPort.PortUnit() );
    OstTrace0( TRACE_NORMAL, DUP2_CDPFLOWCTRL_WATERMARKLOWREACHED, "CDpFlowCtrl:: LOW WATER MARK" );

    LOGM1(" CDpFlowCtrl::WaterMarkLowReached - Port %d",
        iDataPort.PortUnit() );
    LOG(" CDpFlowCtrl - LOW WATER MARK");

    // flow control is set OFF, i.e. data is being transmitted
    iFlowCtrlDcs2Dp = EFlowControlOff;
    // inform flow control users
    Notify();
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::WaterMarkHighReached
// RX buffer or Isc Api's proxy has reached high water mark.
// Sets the local RX flow ON. Sends flow change to DCS if needed.
// Notify flow control users (flow control observers).
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::WaterMarkHighReached()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_WATERMARKHIGHREACHED, "CDpFlowCtrl::WaterMarkHighReached" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPFLOWCTRL_WATERMARKHIGHREACHED, "CDpFlowCtrl:: Port: %u", iDataPort.PortUnit() );
    OstTrace0( TRACE_NORMAL, DUP2_CDPFLOWCTRL_WATERMARKHIGHREACHED, "CDpFlowCtrl:: HIGH WATER MARK" );

    LOGM1(" CDpFlowCtrl::WaterMarkHighReachedL - Port %d",
        iDataPort.PortUnit() );
    LOG(" CDpFlowCtrl - HIGH WATER MARK");

    // flow control is set ON, ie data is inhibited
    iFlowCtrlDcs2Dp = EFlowControlOn;
    // inform flow control users
    Notify();
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::NotifyClientDatapotReadyforDteFlow
// Notifies client about flow control change when starting DataPort.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::NotifyClientDatapotReadyforDteFlow()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_NOTIFYCLIENTDATAPOTREADYFORDTEFLOW, "CDpFlowCtrl::NotifyClientDatapotReadyforDteFlow" );
    LOGM(" CDpFlowCtrl::NotifyClientDatapotReadyforDteFlow");

    if ( ( !iIsDataportReadyforDteFlow ) && iFlowCtrlDp2Dcs )
        {
        InformFlowControlToClient();
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::InformFlowControlToClient
// Informs client about the flow control change.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::InformFlowControlToClient()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_INFORMFLOWCONTROLTOCLIENT, "CDpFlowCtrl::InformFlowControlToClient" );
    LOGM(" CDpFlowCtrl::InformFlowControlToClient");

    if ( EFlowControlOn == iFlowCtrlDp2Dcs )
        {
        LOG("  CDpFlowCtrl::InformFlowControlToClient, EFlowControlOn (completed)");
        OstTrace0( TRACE_NORMAL, DUP1_CDPFLOWCTRL_INFORMFLOWCONTROLTOCLIENT, "CDpFlowCtrl:: EFlowControlOn (completed)" );
        }
    else
        {
        LOG("  CDpFlowCtrl::InformFlowControlToClient, EFlowControlOff (completed)");
        OstTrace0( TRACE_NORMAL, DUP2_CDPFLOWCTRL_INFORMFLOWCONTROLTOCLIENT, "CDpFlowCtrl:: EFlowControlOff (completed)" );
        }

    iDataPort.FlowControlChangeCompleted( iFlowCtrlDp2Dcs, KErrNone );
    iLastFlowControlSentToClient = iFlowCtrlDp2Dcs;

    iIsDataportReadyforDteFlow = ETrue;
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::Attach
// Attach observer to CDpSubject.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::Attach( CDpObserver& aObserver )
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_ATTACH, "CDpFlowCtrl::Attach" );
    iDpSubject.Attach( aObserver );
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::Detach
// Detach observer from CDpSubject.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::Detach( CDpObserver& aObserver )
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_DETACH, "CDpFlowCtrl::Detach" );
    iDpSubject.Detach( aObserver );
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::Notify
// Notify all observers of CDpSubject object.
// -----------------------------------------------------------------------------
//
void CDpFlowCtrl::Notify()
    {
    OstTrace0( TRACE_NORMAL, CDPFLOWCTRL_NOTIFY, "CDpFlowCtrl::Notify" );
    iDpSubject.Notify();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// none

//  End of File
