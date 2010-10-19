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
#include <pipeisi.h>        // isi pipe
#include "pep_comm_types.h" // structures for isi-message interface

#include "dpdataport.h"     // dataport main and c32 interface
#include "dpbreak.h"        // break handling
#include "dpdataconfig.h"   // configuration store
#include "dpdatabuffer.h"   // base class for rx and tx buffers
#include "dpmif.h"          // message interface
#include "dppif.h"          // pipe interface
#include "dplog.h"          // dataport logging

#include <pipe_sharedisi.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpbreakTraces.h"
#endif


// LOCAL FUNCTION PROTOTYPES
// none

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDpBreak::CDpBreak
// C++ default constructor.
//
// ---------------------------------------------------------
CDpBreak::CDpBreak(
    CDpDataPort& aDataPort ) :
    iDataPort( aDataPort ),
    iRx( iDataPort.Rx() ),
    iBreakNotify( EFalse )
    {
    OstTrace0( TRACE_NORMAL, CDPBREAK_CDPBREAK, "CDpBreak::CDpBreak" );
    LOGM(" CDpBreak::CDpBreak");

    }

// ---------------------------------------------------------
// CDpBreak::~CDpBreak
// Destructor
// ---------------------------------------------------------
CDpBreak::~CDpBreak()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPBREAK_CDPBREAK, "CDpBreak::~CDpBreak" );
    LOGM("  CDpBreak::~CDpBreak");

    }

// ---------------------------------------------------------
// CDpBreak::InitL
// This method initializes member variables
// ---------------------------------------------------------
//
void CDpBreak::Init()
    {
    OstTrace0( TRACE_NORMAL, CDPBREAK_INIT, "CDpBreak::Init" );
    LOGM("  CDpBreak::Init");

    }

// ---------------------------------------------------------
// CDpBreak::CompleteBreakNotify
// Break signal has been received from DCS and now we inform
// the client about that by completing the nofify request.
// ---------------------------------------------------------
//
TInt CDpBreak::CompleteBreakNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPBREAK_COMPLETEBREAKNOTIFY, "CDpBreak::CompleteBreakNotify" );
    LOGM(" CDpBreak::CompleteBreakNotify");

    TUint8 signalType( iDataPort.DataConfig().DteTypeOfBreakSignal() );
    TInt ret( KErrNone );

    switch ( signalType )
        {
        case PEP_COMM_BREAK_DESTRUCT:
            {
            // Send break to client, clear DTE transmit buffers
            iDataPort.BreakNotifyCompleted( KErrNone );
            SetBreakNotify( EFalse );
            iRx.Flush();
            break;
            }
        case PEP_COMM_BREAK_EXPEDITED:
            {
            // Send break, then data
            iDataPort.BreakNotifyCompleted( KErrNone );
            SetBreakNotify( EFalse );
            break;
            }
        case PEP_COMM_BREAK_NON_EXPEDITED:
            {
            // Send all RX data buffered before break, then break
            iDataPort.SetRxBreakBytes( iRx.UsedBytes() );
            break;
            }
        default:
            {
            LOG1(" ERROR - Unsupported break signal type: %d in Break::CompleteBreakNotify",
                signalType );
            OstTraceExt1( TRACE_NORMAL, DUP1_CDPBREAK_COMPLETEBREAKNOTIFY, "CDpBreak:: ERROR - Unsupported break signal type: %hhu in Break::CompleteBreakNotify", signalType );
            ret = KErrGeneral;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpBreak::GetBreakNotify
// CDpCommPep::HandlePepStatusInd() method uses this to find
// out, are we requested to notify the client about incoming
// (DCS->Client) break signal.
// ---------------------------------------------------------
//
void CDpBreak::GetBreakNotify(
    TBool& aBreakNotify ) const
    {
    OstTrace0( TRACE_NORMAL, CDPBREAK_GETBREAKNOTIFY, "CDpBreak::GetBreakNotify" );
    LOGM(" CDpBreak::GetBreakNotify");

    aBreakNotify = iBreakNotify;
    }

// ---------------------------------------------------------
// CDpBreak::SetBreakNotify
// This method sets break notify status active or deactive.
// Break notify status indicates, whether a received break
// signal must be notified to the client.
// ---------------------------------------------------------
//
void CDpBreak::SetBreakNotify(
    const TBool aBreakNotify )
    {
    OstTrace0( TRACE_NORMAL, CDPBREAK_SETBREAKNOTIFY, "CDpBreak::SetBreakNotify" );
    LOGM(" CDpBreak::SetBreakNotify");

    iBreakNotify = aBreakNotify;
    }

// ---------------------------------------------------------
// CDpDataPort::SendBreak
// This method sends break to DCS. This method is called from
// CDpDataPort::Break() and CDpTx2Pn::HandleReturnValueL().
// NOTE: This method does not complete client's break request
// ---------------------------------------------------------
//
TInt CDpBreak::SendBreak()
    {
    OstTrace0( TRACE_NORMAL, CDPBREAK_SENDBREAK, "CDpBreak::SendBreak" );
    LOGM(" CDpBreak::SendBreak");

    // Create PNS_PEP_STATUS_IND : PEP_COMM_IND_ID_BREAK message
    TBuf8<SIZE_PNS_PEP_STATUS_IND> messageData;
    // Pipe Handle
    messageData.Append( iDataPort.Pif().PipeHandle() );
    // PEP Type
    messageData.Append( PN_PEP_TYPE_COMM );
    // Indication ID
    messageData.Append( PEP_COMM_IND_ID_BREAK );
    // Filler bytes
    messageData.Append( KDpPadding );
    messageData.Append( KDpPadding );

    // Send message
#ifdef _DEBUG
    TInt ret =
#endif
    iDataPort.Mif().SendMessage(
#ifdef PIPECAMP_DATAPORT_PNS_PEP_STATUS_IND_PHONET_ADDRESS_FROM_PNS_PEP_CTRL_REQ // 20100523_1300
       iDataPort.Pif().GetPipeControllerDeviceIdentifier(),
       iDataPort.Pif().GetPipeControllerObjectIdentifier(),
#endif    
        PN_PIPE,
        iDataPort.CreateTransactionId(),
        PNS_PEP_STATUS_IND,
        messageData );

    LOG("  <== PNS_PEP_STATUS_IND sent (PEP_COMM_IND_ID_BREAK)");
    OstTrace0( TRACE_NORMAL, DUP1_CDPBREAK_SENDBREAK, "CDpBreak:: <== PNS_PEP_STATUS_IND sent (PEP_COMM_IND_ID_BREAK)" );


#ifdef _DEBUG
    if ( KErrNone != ret )
        {
        LOG1("Error isc api send %d", ret );
        OstTrace1( TRACE_NORMAL, DUP2_CDPBREAK_SENDBREAK, "CDpBreak:: Error isc api send %d", ret );

        }
    // no else
#endif

    return KErrNone;
}

// ================= OTHER EXPORTED FUNCTIONS ==============
//  none

//  End of File
