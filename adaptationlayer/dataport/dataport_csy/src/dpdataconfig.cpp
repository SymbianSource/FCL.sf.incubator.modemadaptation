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
#include "dpdataconfig.h"   // configuration store
#include "dppif.h"          // dcs pipe interface
#include "dpdatabuffer.h"   // base class for rx and tx buffers
#include "dpmif.h"          // message interface mif
#include "dplog.h"          // dataport logging

#include <pipe_sharedisi.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpdataconfigTraces.h"
#endif


// LOCAL FUNCTION PROTOTYPES
// none

// ==================== LOCAL FUNCTIONS ====================
// none

// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// DPDataConfig::NewL
// Creates a new  DPDataConfig object instance
// -----------------------------------------------------------------------------
CDpDataConfig *CDpDataConfig::NewL(
    CDpDataPort& aDataPort )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_NEWL, "CDpDataConfig::NewL" );
    CDpDataConfig* self = new( ELeave ) CDpDataConfig( aDataPort );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// DPDataConfig::ConstructL
// Initialises object attributes
// -----------------------------------------------------------------------------
void CDpDataConfig::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_CONSTRUCTL, "CDpDataConfig::ConstructL" );
    // set role capabilities
    User::LeaveIfError( SetRoleCap( iRole ) );
    }

// -----------------------------------------------------------------------------
// CDpDataConfig::CDpDataConfig
// C++ default constructor.
// -----------------------------------------------------------------------------
CDpDataConfig::CDpDataConfig(
    CDpDataPort& aDataPort ) :
    iDataPort( aDataPort )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_CDPDATACONFIG, "CDpDataConfig::CDpDataConfig" );
    LOGM(" CDpDataConfig::CDpDataConfig");

    iDataPort.GetRole( iRole );
    iDteParityBitWhen7Mode = PEP_COMM_DATA_WITH_PARITY;

    // set default configuration
    iConfig.iRate = EBps9600;

    // default data format 8N1
    iConfig.iDataBits = EData8;
    iConfig.iStopBits = EStop1;
    iConfig.iParity = EParityNone;

    // DTE defaults
    iConfig.iHandshake = KConfigObeyCTS;
    // Other options to handshake default:
    // KConfigObeyXoff
    // KConfigSendXoff
    // KConfigObeyCTS;
    // KConfigObeyDSR
    // KConfigObeyDCD
    // KConfigFreeRTS
    // KConfigFreeDTR

    // not really applicable
    iConfig.iFifo = EFifoEnable;

    // use no special rate
    iConfig.iSpecialRate = 0;

    // use no terminators
    iConfig.iTerminatorCount = 0;

    // set XON/XOFF characters into 'typical' values
    iConfig.iXonChar = KDcsDefaultXonChar;
    iConfig.iXoffChar = KDcsDefaultXoffChar;

    // ignore parity errors
    iConfig.iParityError = KConfigParityErrorIgnore;

    // ignoring parity errors
    iConfig.iParityErrorChar = 0;

    // we do no IR
    iConfig.iSIREnable = ESIRDisable;
    iConfig.iSIRSettings = 0;

    // flowcontrol extraction
    iFlowCtrlCharExtraction = ETrue;

    iSignals = 0;
    // signals are low, but signal changed bits are up
    // that is because otherwise we might miss clients first signal change,
    // case of signal change is up->low.

    // set capabilities
    // data rate
    iCapsV02.iRate =
        KCapsBps2400 |
        KCapsBps4800 |
        KCapsBps9600 |
        KCapsBps19200 |
        KCapsBps38400 |
        KCapsBps57600 |
        KCapsBps115200;
        //| KCapsBpsSpecial; // unsupported currently

    // data bits
    iCapsV02.iDataBits = KCapsData7 | KCapsData8;

    // stop bits
    iCapsV02.iStopBits = KCapsStop1 | KCapsStop2;

    // parity
    iCapsV02.iParity =
        KCapsParityNone |
        KCapsParityOdd |
        KCapsParityEven |
        KCapsParityMark |
        KCapsParitySpace;

    iCapsV02.iSignals =
        KCapsSignalDSRSupported |
        KCapsSignalDCDSupported |
        KCapsSignalDTRSupported |
        KCapsSignalRNGSupported;

    // not really applicable
    // KCapsHasFifo;
    iCapsV02.iFifo = 0;

    // no IR
    iCapsV02.iSIR = 0;

    // supported notifications
    iCapsV02.iNotificationCaps =
        KNotifySignalsChangeSupported |
        KNotifyRateChangeSupported |
        KNotifyDataFormatChangeSupported |
        KNotifyHandshakeChangeSupported |
        KNotifyBreakSupported |
        KNotifyFlowControlChangeSupported |
        KNotifyDataAvailableSupported |
        KNotifyOutputEmptySupported;

    iCapsV02.iRoleCaps = KCapsRoleSwitchSupported;

    // port supports retrieval of flow control status
    iCapsV02.iFlowControlCaps = KCapsFlowControlStatusSupported;

    // '+'
    iEscChar = KEscChar;
    // in 50 millisec note: previous comment was in 20 milliseconds
    iEscTime = KEscTime50millisec;
    }

// -----------------------------------------------------------------------------
// CDpDataConfig::~CDpDataConfig
// Destructor
// -----------------------------------------------------------------------------
CDpDataConfig::~CDpDataConfig()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPDATACONFIG_CDPDATACONFIG, "CDpDataConfig::~CDpDataConfig" );
    LOGM(" CDpDataConfig::~CDpDataConfig");
    }

// ---------------------------------------------------------
// CDpDataConfig::SetRoleCap
// Sets role Capabalities
// ---------------------------------------------------------
//
TInt CDpDataConfig::SetRoleCap(
    TCommRole aRole )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETROLECAP, "CDpDataConfig::SetRoleCap" );
    TInt ret( KErrNone );

    switch ( aRole )
        {
        case ECommRoleDTE:
            {
            // we cope with both sw & hw flow control
            iCapsV02.iHandshake =
                KCapsObeyXoffSupported |
                KCapsSendXoffSupported |
                KCapsObeyCTSSupported |
                KCapsObeyDSRSupported |
                KCapsObeyDCDSupported |
                KCapsFreeDTRSupported |
                KCapsFailDCDSupported |
                KCapsFreeRTSSupported;
            break;
            }
        case ECommRoleDCE:
            {
            // we cope with sw flow control and RTS DTR
            iCapsV02.iHandshake =
                KCapsObeyXoffSupported |
                KCapsSendXoffSupported |
                KCapsObeyCTSSupported |
                KCapsObeyDTRSupported |
                0x1000 | // KCapsObeyRTSSupported
                0x4000 | // KCapsFreeDSRSupported
                0x10000; // KCapsFreeDCDSupported
            break;
            }
        default:
            {
            ret = KErrGeneral;
            break;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::GetCommCaps
// This method gets port capabilities in TCommCapsV01.
// ---------------------------------------------------------
//
void CDpDataConfig::GetCommCaps(
    TCommCapsV01& aCommCaps ) const
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_GETCOMMCAPS, "CDpDataConfig::GetCommCaps" );
    LOGM(" CDpDataConfig::GetCommCaps");

    aCommCaps.iRate = iCapsV02.iRate;
    aCommCaps.iDataBits = iCapsV02.iDataBits;
    aCommCaps.iStopBits = iCapsV02.iStopBits;
    aCommCaps.iParity = iCapsV02.iParity;
    aCommCaps.iHandshake = iCapsV02.iHandshake;
    aCommCaps.iSignals = iCapsV02.iSignals;
    aCommCaps.iFifo = iCapsV02.iFifo;
    aCommCaps.iSIR = iCapsV02.iSIR;
    }

// ---------------------------------------------------------
// CDpDataConfig::GetCommCapsV02
// This method gets port capabilities in TCommCapsV02.
// ---------------------------------------------------------
//
void CDpDataConfig::GetCommCapsV02(
    TCommCapsV02& aCommCaps2 ) const
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_GETCOMMCAPSV02, "CDpDataConfig::GetCommCapsV02" );
    LOGM(" CDpDataConfig::GetCommCapsV02");

    GetCommCaps( aCommCaps2 );

    aCommCaps2.iNotificationCaps = iCapsV02.iNotificationCaps;
    aCommCaps2.iRoleCaps = iCapsV02.iRoleCaps;
    aCommCaps2.iFlowControlCaps = iCapsV02.iFlowControlCaps;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetConfig
// This method sets DTE interface port configuration
// settings.
// ---------------------------------------------------------
//
TInt CDpDataConfig::SetConfig(
    TCommConfigV01& aConfig )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETCONFIG, "CDpDataConfig::SetConfig" );
    LOGM(" CDpDataConfig::SetConfig");

    TInt ret( KErrNone );

    //Initialise these values to 0
    TUint8 rate( 0 );
    TUint8 dataBits( 0 );
    TUint8 stopBits( 0 );
    TUint8 parity( 0 );
    TBool saveSettings ( EFalse );

    // Check against supported values, NOTE: caps is a bitmask,
    // when config is enumerated.
    if ( !( iCapsV02.iRate & ( 1 << aConfig.iRate ) ) )
        {
        ret = KErrNotSupported;
        }
    else if ( !( iCapsV02.iDataBits & ( 1 << aConfig.iDataBits ) ) )
        {
        ret = KErrNotSupported;
        }
    else if ( !( iCapsV02.iStopBits & ( 1 << aConfig.iStopBits ) ) )
        {
        ret = KErrNotSupported;
        }
    else if ( !( iCapsV02.iParity & ( 1 << aConfig.iParity ) ) )
        {
        ret = KErrNotSupported;
        }
    else if ( ( ~iCapsV02.iHandshake & aConfig.iHandshake ) &&
        ( 0 != aConfig.iHandshake ) )
        {
        ret = KErrNotSupported;
        }
    else
        {
        // We do not parity check, disable fifo, do special rate nor IR.
        if ( ( aConfig.iParityError != KConfigParityErrorIgnore ) ||
            ( aConfig.iFifo != EFifoEnable ) ||
            aConfig.iSpecialRate ||
            aConfig.iParityErrorChar ||
            ( aConfig.iSIREnable != ESIRDisable ) ||
            aConfig.iSIRSettings )
            {
            LOG("  ERROR, SetConfig - KErrNotSupported");
            OstTrace0( TRACE_NORMAL, DUP1_CDPDATACONFIG_SETCONFIG, "CDpDataConfig:: ERROR, SetConfig - KErrNotSupported" );

            ret = KErrNotSupported;
            }
        else if ( KConfigMaxTerminators < aConfig.iTerminatorCount )
            {
            LOG("  ERROR, CDpDataConfig::SetConfig, KErrArgument");
            OstTrace0( TRACE_NORMAL, DUP2_CDPDATACONFIG_SETCONFIG, "CDpDataConfig:: ERROR, SetConfig - KErrArgument" );

            ret = KErrArgument;
            }
        // We care only databits, stopbits and parity changes.
        // We don't send message to CMT when only speed has changed.
        // CMT is not using speed setting at all.
        else if ( ( iConfig.iDataBits == aConfig.iDataBits ) &&
            ( iConfig.iStopBits == aConfig.iStopBits ) &&
            ( iConfig.iParity == aConfig.iParity ) )
            {
            // Save settings, to return same
            // settings when client is asking them.
            iConfig = aConfig;
            saveSettings = ETrue;
            }
        // Do parameter mapping.
        // check data rate
        else if ( KErrNone != Rate( aConfig, rate ) )
            {
            ret = Rate( aConfig, rate );
            }
        // check data bits
        else if ( KErrNone != DataBits( aConfig, dataBits ) )
            {
            ret = DataBits( aConfig, dataBits );
            }
        // check stop bits
        else if ( KErrNone != StopBits( aConfig, stopBits ) )
            {
            ret = StopBits( aConfig, stopBits );
            }
        // check parity
        else if ( KErrNone != Parity( aConfig, parity ) )
            {
            ret = Parity( aConfig, parity );
            }
        //no else

        if ( KErrNone == ret && !saveSettings )
            {
            // this is for if Client uses SetConfig before we have pipe up.
            if ( CDpPif::EDpPipeEnabled != iDataPort.Pif().PipeState() )
                {
                ret = KErrNotReady;
                }
            else
                {
                // Create PNS_PEP_STATUS_IND : PEP_COMM_IND_ID_PARAM
                // (Indication data, PEP_COMM_CTRL_ID_PARAM_STR size = 4 bytes)
                TBuf8<SIZE_PNS_PEP_STATUS_IND + 4 > messageData;
                // Pipe Handle
                messageData.Append( iDataPort.Pif().PipeHandle() );
                // PEP Type
                messageData.Append( PN_PEP_TYPE_COMM );
                // Indication ID
                messageData.Append( PEP_COMM_IND_ID_PARAM );
                // Filler bytes
                messageData.Append( KDpPadding );
                messageData.Append( KDpPadding );
                // Indication data (PEP_COMM_CTRL_ID_PARAM_STR)
                messageData.Append( dataBits );
                messageData.Append( stopBits );
                messageData.Append( rate );
                messageData.Append( parity );

                // Send messages
                TInt result = iDataPort.Mif().SendMessage(
#ifdef PIPECAMP_DATAPORT_PNS_PEP_STATUS_IND_PHONET_ADDRESS_FROM_PNS_PEP_CTRL_REQ // 20100523_1300
       							iDataPort.Pif().GetPipeControllerDeviceIdentifier(),
       							iDataPort.Pif().GetPipeControllerObjectIdentifier(),
#endif                
                    PN_PIPE,
                    iDataPort.CreateTransactionId(),
                    PNS_PEP_STATUS_IND,
                    messageData );

                LOG("  <== PNS_PEP_STATUS_IND ind sent (PEP_COMM_IND_ID_PARAM)");
                OstTrace0( TRACE_NORMAL, DUP3_CDPDATACONFIG_SETCONFIG, "CDpDataConfig:: <== PNS_PEP_STATUS_IND ind sent (PEP_COMM_IND_ID_PARAM)" );

#if defined(_DEBUG)
                if ( KErrNone != result )
                    {
                    LOG1("Error isc api send %d", result );
                    OstTrace1( TRACE_NORMAL, DUP4_CDPDATACONFIG_SETCONFIG, "CDpDataConfig:: Error isc api send %d", result );
                    }
                //no else
#endif
                // accept new values
                iConfig = aConfig;
                }
            }
        //no else
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::GetSignals
// This method gets status of signals.
////  On return, contains signals
// ---------------------------------------------------------
//
void CDpDataConfig::GetSignals(
    TUint& aSignals )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_GETSIGNALS, "CDpDataConfig::GetSignals" );
    LOGM(" CDpDataConfig::GetSignals");

    aSignals = iSignals;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetSignals2Epoc
// Signal cahnges from CMT. This method sets signal values.
// ---------------------------------------------------------
//
TInt CDpDataConfig::SetSignals2Epoc(
    const TUint aSignals )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETSIGNALS2EPOC, "CDpDataConfig::SetSignals2Epoc" );
    LOGM(" CDpDataConfig::SetSignals2Epoc");

    TCommRole role;
    TUint newSignals( 0 );
    iDataPort.GetRole( role );

    // accept new signals
    iSignals = aSignals;

    // signal change came from DCS

    if ( iSignalNotify )
        {
        newSignals = iSignals & (
            iNotifySignalMask | iNotifySignalMask * KSignalChanged );

        if ( ECommRoleDTE == role )
            {
            newSignals &= ( KSignalDTEInputs | KDTEDeltaInput );
            }
        else
            {
            newSignals &= ( KSignalDCEInputs | KDCEDeltaInput );
            }

        // we should indicate client
        CompleteSignalChangeNotify( newSignals );
        }
    //no else

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetSignals2Cmt
// Signal changes from the C32 client.
// This method sends mapped signal values to CMT and sets
// signal values.
// ---------------------------------------------------------
//
TInt CDpDataConfig::SetSignals2Cmt(
    const TUint aSignals )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETSIGNALS2CMT, "CDpDataConfig::SetSignals2Cmt" );
    LOGM(" CDpDataConfig::SetSignals2Cmt");

    TCommRole role;
    iDataPort.GetRole( role );

    // signal change came from client

    TInt ret( MapSignalsEpoc2Cmt( aSignals, role ) );

    if ( KErrNone == ret )
        {
        // accept new signals
        iSignals = aSignals;

        TUint newSignals( 0 );

        if ( iSignalNotify )
            {
            newSignals = iSignals & (
                iNotifySignalMask | iNotifySignalMask * KSignalChanged );
            if ( ECommRoleDTE == role )
                {
                newSignals &= ( KSignalDCEInputs | KDCEDeltaInput );
                }
            else
                {
                newSignals &= ( KSignalDTEInputs | KDTEDeltaInput );
                }

            // we should indicate client
            CompleteSignalChangeNotify( newSignals );
            }
        //no else
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetSignalNotify
// Activate/deactivate signal change notify.
// ---------------------------------------------------------
//
void CDpDataConfig::SetSignalNotify(
    const TBool aEnableNotify )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETSIGNALNOTIFY, "CDpDataConfig::SetSignalNotify" );
    LOGM(" CDpDataConfig::SetSignalNotify");

    iSignalNotify = aEnableNotify;

    // Forget signal changes
    iSignals &= ( KSignalDTEOutputs | KSignalDTEInputs );
    }

// ---------------------------------------------------------
// CDpDataConfig::SetNotifySignalMask
// Client sets a signal mask. i.e. Client can give a mask
// and we only notify client when one of the signals given
// in mask changes.
// ---------------------------------------------------------
//
void CDpDataConfig::SetNotifySignalMask(
    const TUint aSignalMask )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETNOTIFYSIGNALMASK, "CDpDataConfig::SetNotifySignalMask" );
    LOGM(" CDpDataConfig::SetNotifySignalMask");

    iNotifySignalMask = aSignalMask;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetConfigNotify
// Client is requesting to notify when configuration changes
// ---------------------------------------------------------
//
void CDpDataConfig::SetConfigNotify(
    const TBool aEnableNotify )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETCONFIGNOTIFY, "CDpDataConfig::SetConfigNotify" );
    LOGM(" CDpDataConfig::SetConfigNotify");

    iConfigNotify = aEnableNotify;
    }

// ---------------------------------------------------------
// CDpDataConfig::MapSignalsCmt2Epoc
// This method map signals from CMT messages to C32 signals.
// ---------------------------------------------------------
//
TInt CDpDataConfig::MapSignalsCmt2Epoc(
    TDesC8& aIndData )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig::MapSignalsCmt2Epoc" );
    LOGM(" CDpDataConfig::MapSignalsCmt2Epoc");

    TUint signalsC32( iSignals );
    TInt ret( KErrNone );
    TBool noSignalChanged( EFalse );

    PEP_COMM_IND_ID_V24_CHANGE_STR& pepParam = *(
        ( PEP_COMM_IND_ID_V24_CHANGE_STR* ) aIndData.Ptr() );

    // DTE inputs from CMT side
    if ( PEP_COMM_SIGNAL_RI_ON == pepParam.new_state )
        {
        LOG(" PEP_COMM_SIGNAL_RI_ON");
        OstTrace0( TRACE_NORMAL, DUP1_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: PEP_COMM_SIGNAL_RI_ON" );

        if ( ( signalsC32 & KSignalRNG ) != KSignalRNG )
            {
            signalsC32 |= KSignalRNG | KRNGChanged;
            }
        else
            {
            // No signals has changed
            noSignalChanged = ETrue;
            }
        }
    else if ( PEP_COMM_SIGNAL_RI_OFF == pepParam.new_state )
        {
        LOG(" PEP_COMM_SIGNAL_RI_OFF");
        OstTrace0( TRACE_NORMAL, DUP2_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: PEP_COMM_SIGNAL_RI_OFF" );

        if ( ( signalsC32 & KSignalRNG ) == KSignalRNG )
            {
            signalsC32 &= ~KSignalRNG;
            signalsC32 |= KRNGChanged;
            }
        else
            {
            // No signals has changed
            noSignalChanged = ETrue;
            }
        }
    else if ( PEP_COMM_SIGNAL_DCD_ON == pepParam.new_state )
        {
        LOG(" PEP_COMM_SIGNAL_DCD_ON");
        OstTrace0( TRACE_NORMAL, DUP3_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: PEP_COMM_SIGNAL_DCD_ON" );

        if ( ( signalsC32 & KSignalDCD ) != KSignalDCD )
            {
            LOG("DCD_ON");
            OstTrace0( TRACE_NORMAL, DUP4_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: DCD_ON" );

            signalsC32 |= KSignalDCD | KDCDChanged;
            }
        else
            {
            // No signals has changed
            noSignalChanged = ETrue;
            }
        }
    else if ( PEP_COMM_SIGNAL_DCD_OFF == pepParam.new_state )
        {
        LOG(" PEP_COMM_SIGNAL_DCD_OFF");
        OstTrace0( TRACE_NORMAL, DUP5_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: PEP_COMM_SIGNAL_DCD_OFF" );

        if ( ( signalsC32 & KSignalDCD ) == KSignalDCD )
            {
            LOG("DCD_OFF");
            OstTrace0( TRACE_NORMAL, DUP6_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: DCD_OFF" );

            signalsC32 &= ~KSignalDCD;
            signalsC32 |= KDCDChanged;
            }
        else
            {
            // No signals has changed
            noSignalChanged = ETrue;
            }
        }
    else if ( PEP_COMM_SIGNAL_DSR_ON == pepParam.new_state )
        {
        LOG(" PEP_COMM_SIGNAL_DSR_ON");
        OstTrace0( TRACE_NORMAL, DUP7_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: PEP_COMM_SIGNAL_DSR_ON" );

        if ( ( signalsC32 & KSignalDSR ) != KSignalDSR )
            {
            LOG("DSR_ON");
            OstTrace0( TRACE_NORMAL, DUP8_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: DSR_ON" );

            signalsC32 |= KSignalDSR | KDSRChanged;
            }
        else
            {
            // No signals has changed
            noSignalChanged = ETrue;
            }
        }
    else if ( PEP_COMM_SIGNAL_DSR_OFF == pepParam.new_state )
        {
        LOG(" PEP_COMM_SIGNAL_DSR_OFF");
        OstTrace0( TRACE_NORMAL, DUP9_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: PEP_COMM_SIGNAL_DSR_OFF" );

        if ( ( signalsC32 & KSignalDSR ) == KSignalDSR )
            {
            LOG("DSR_OFF");
            OstTrace0( TRACE_NORMAL, DUP10_CDPDATACONFIG_MAPSIGNALSCMT2EPOC, "CDpDataConfig:: DSR_OFF" );

            signalsC32 &= ~KSignalDSR;
            signalsC32 |= KDSRChanged;
            }
        else
            {
            // No signals has changed
            noSignalChanged = ETrue;
            }
        }
    else
        {
        // No signals has changed
        noSignalChanged = ETrue;
        }

    if ( !noSignalChanged )
        {
        ret = SetSignals2Epoc( signalsC32 );
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::MapSignalsEpoc2Cmt
// Map signals to CMT and send signal change message.
// ---------------------------------------------------------
//
TInt CDpDataConfig::MapSignalsEpoc2Cmt(
    const TUint aSignals,
    const TCommRole aRole )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_MAPSIGNALSEPOC2CMT, "CDpDataConfig::MapSignalsEpoc2Cmt" );
    LOGM(" CDpDataConfig::MapSignalsEpoc2Cmt");

    TInt ret( KErrNone );

    // PEP_COMM_IND_ID_V24_CHANGE_STR
    TBuf8<1> data;

    switch ( aRole )
        {
        case ECommRoleDTE:
            {
            HandleDteSignalMapping( aSignals, data );
            break;
            }
        case ECommRoleDCE:
            {
            HandleDceSignalMapping( aSignals, data );
            break;
            }
       default:
            {
            LOG("  ERROR, CDpDataConfig::MapSignalsEpoc2Cmt, Unsupported role");
            OstTrace0( TRACE_NORMAL, DUP1_CDPDATACONFIG_MAPSIGNALSEPOC2CMT, "ERROR, CDpDataConfig::MapSignalsEpoc2Cmt, Unsupported role" );

            ret = KErrNotSupported;
            }
        }

    //if data = 0, do not send
    if ( ( 0 != data.Length() ) && ( KErrNone == ret ) )
        {
        // Create PNS_PEP_STATUS_IND
        TBuf8<SIZE_PNS_PEP_STATUS_IND> messageData;
        // Pipe Handle
        messageData.Append( iDataPort.Pif().PipeHandle() );
        // PEP Type
        messageData.Append( PN_PEP_TYPE_COMM );
        // Indication ID
        messageData.Append( PEP_COMM_IND_ID_V24_CHANGE );
        // Filler bytes
        messageData.Append( KDpPadding );
        messageData.Append( KDpPadding );
        // Indication data
        messageData.Append( data );

        // Send message
#if defined(_DEBUG)
        TInt err =
#endif
        iDataPort.Mif().SendMessage(
            PN_PIPE,
            iDataPort.CreateTransactionId(),
            PNS_PEP_STATUS_IND,
            messageData );

        LOG("  <== PNS_PEP_STATUS_IND sent (PEP_COMM_IND_ID_V24_CHANGE)");
        OstTrace0( TRACE_NORMAL, DUP2_CDPDATACONFIG_MAPSIGNALSEPOC2CMT, "CDpDataConfig:: <== PNS_PEP_STATUS_IND sent (PEP_COMM_IND_ID_V24_CHANGE)" );

#if defined(_DEBUG)
        if ( KErrNone != err )
            {
            LOG1("Error isc api send %d", err );
            OstTrace1( TRACE_NORMAL, DUP3_CDPDATACONFIG_MAPSIGNALSEPOC2CMT, "CDpDataConfig:: Error isc api send %d", err );
            }
        //no else
#endif
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::CompleteSignalChangeNotify
// This method completes signal change notify, if there are
// new signals. New signals are already mapped with notify
// mask. This method disables notifying after completion.
// ---------------------------------------------------------
//
void CDpDataConfig::CompleteSignalChangeNotify(
    const TUint aNewSignals)
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_COMPLETESIGNALCHANGENOTIFY, "CDpDataConfig::CompleteSignalChangeNotify" );
    LOGM(" CDpDataConfig::CompleteSignalChangeNotify");

    if ( aNewSignals )
        {
        iDataPort.SignalChangeCompleted( aNewSignals, KErrNone );

        SetSignalNotify( EFalse );
        }
    //no else

    // otherwise client is not interested in these signals
    }

// ---------------------------------------------------------
// CDpDataConfig::CompleteConfigChangeNotify
// Check if client has 'ordered' config change notify and there
// is something changed. Complete config change to client.
// ---------------------------------------------------------
//
void CDpDataConfig::CompleteConfigChangeNotify()
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_COMPLETECONFIGCHANGENOTIFY, "CDpDataConfig::CompleteConfigChangeNotify" );
    LOGM(" CDpDataConfig::CompleteConfigChangeNotify");

    if ( ( iConfigNotify ) && ( 0 < iPeerConfig.iChangedMembers ) )
        {
        TPckg<TCommNotificationV01> cfg( iPeerConfig );
        iDataPort.ConfigChangeCompleted( cfg, KErrNone );
        iConfigNotify = EFalse;
        iPeerConfig.iChangedMembers = 0;
        }
    //no else
    }

// ---------------------------------------------------------
// CDpDataConfig::SetFlowControlConfig
// This method sets flow control configuration. This is
// called from COMM PEP.
// ---------------------------------------------------------
//
TInt CDpDataConfig::SetFlowControlConfig(
    const TUint8 aFlowCtrlInwards,
    const TUint8 aFlowCtrlOutwards,
    const TUint8 aExtract )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETFLOWCONTROLCONFIG, "CDpDataConfig::SetFlowControlConfig" );
    LOGM(" CDpDataConfig::SetFlowControlConfig");

    TUint handshake( iConfig.iHandshake );
    TInt ret( KErrNone );
    TCommRole role;
    iDataPort.GetRole( role );

    // clear peer configuration notify flags
    iPeerConfig.iChangedMembers = 0;

    // Inwards=DCS->DP data flow
    switch ( aFlowCtrlInwards )
        {
        case PEP_COMM_FLOW_CTRL_SW:
            {
            if ( role == ECommRoleDTE )
                {
                handshake |= KConfigObeyXoff;
                handshake &= ~KConfigObeyDSR;
                handshake &= ~KConfigObeyDCD;
                handshake &= ~KConfigObeyCTS;
                }
            else
                {
                handshake |= KConfigObeyXoff;
                handshake &= ~KConfigObeyDTR;
                handshake &= ~KConfigObeyRTS;
                }
            break;
            }
        case PEP_COMM_FLOW_CTRL_HW:
            {
            if ( role == ECommRoleDTE )
                {
                handshake |= KConfigObeyCTS;
                handshake &= ~KConfigObeyDCD;
                handshake &= ~KConfigObeyDSR;
                handshake &= ~KConfigObeyXoff;
                }
            else
                {
                handshake |= KConfigObeyRTS;
                handshake &= ~KConfigObeyXoff;
                handshake &= ~KConfigObeyDTR;
                }
            break;
            }
        case PEP_COMM_FLOW_CTRL_BOTH:
            {
            if ( role == ECommRoleDTE )
                {
                handshake |= KConfigObeyXoff;
                handshake |= KConfigObeyCTS;
                handshake &= ~KConfigObeyDCD;
                handshake &= ~KConfigObeyDSR;
                }
            else
                {
                handshake |= KConfigObeyXoff;
                handshake |= KConfigObeyRTS;
                handshake &= ~KConfigObeyDTR;
                }
            break;
            }
        case PEP_COMM_FLOW_CTRL_NONE:
            {
            if ( role == ECommRoleDTE )
                {
                handshake &= ~KConfigObeyXoff;
                handshake &= ~KConfigObeyCTS;
                handshake &= ~KConfigObeyDCD;
                handshake &= ~KConfigObeyDSR;
                }
            else
                {
                handshake &= ~KConfigObeyXoff;
                handshake &= ~KConfigObeyRTS;
                handshake &= ~KConfigObeyDTR;
                }
            break;
            }
        default:
            {
            LOG1("  ERROR, CDpDataConfig::SetFlowControlConfig, Unsupported flowCtrl status: %d",
                aFlowCtrlInwards );
            OstTraceExt1( TRACE_NORMAL, DUP1_CDPDATACONFIG_SETFLOWCONTROLCONFIG, "CDpDataConfig:: ERROR, CDpDataConfig::SetFlowControlConfig, Unsupported flowCtrl status: %hhu", aFlowCtrlInwards );

            ret = KErrNotSupported;
            }
        }

    if ( KErrNone == ret )
        {
        // Outwards=DP->DCS data flow
        switch ( aFlowCtrlOutwards )
            {
            case PEP_COMM_FLOW_CTRL_SW:
                {
                if ( role == ECommRoleDTE )
                    {
                    handshake |= KConfigSendXoff;
                    handshake |= KConfigFreeDTR;
                    handshake |= KConfigFreeRTS;
                    }
                else
                    {
                    handshake |= KConfigSendXoff;
                    handshake |= KConfigFreeDSR;
                    handshake |= KConfigFreeDCD;
                    }
                break;
                }
            case PEP_COMM_FLOW_CTRL_HW:
                {
                if ( role == ECommRoleDTE )
                    {
                    handshake &= ~KConfigFreeRTS;
                    handshake |= KConfigFreeDTR;
                    handshake &= ~KConfigSendXoff;
                    }
                else
                    {
                    handshake |= KConfigFreeDCD;
                    handshake |= KConfigFreeDSR;
                    handshake &= ~KConfigSendXoff;
                    }
                break;
                }
            case PEP_COMM_FLOW_CTRL_BOTH:
                {
                if ( role == ECommRoleDTE )
                    {
                    handshake |= KConfigSendXoff;
                    handshake &= ~KConfigFreeRTS;
                    handshake |= KConfigFreeDTR;
                    }
                else
                    {
                    handshake |= KConfigSendXoff;
                    handshake |= KConfigFreeDCD;
                    handshake |= KConfigFreeDSR;
                    }
                break;
                }
            case PEP_COMM_FLOW_CTRL_NONE:
                {
                if ( role == ECommRoleDTE )
                    {
                    handshake &= ~KConfigSendXoff;
                    handshake |= KConfigFreeRTS;
                    handshake |= KConfigFreeDTR;
                    }
                else
                    {
                    handshake &= ~KConfigSendXoff;
                    handshake |= KConfigFreeDSR;
                    handshake |= KConfigFreeDCD;
                    }
                break;
                }
            default:
                {
                LOG1("  ERROR, CDpDataConfig::SetFlowControlConfig, Unsupported flowCtrl status: %d",
                    aFlowCtrlInwards );
                OstTraceExt1( TRACE_NORMAL, DUP2_CDPDATACONFIG_SETFLOWCONTROLCONFIG, "ERROR, CDpDataConfig::SetFlowControlConfig, Unsupported flowCtrl status: %hhu", aFlowCtrlInwards );

                ret = KErrNotSupported;
                }
            }

        if ( KErrNone == ret )
            {
            // set flow control char extraction
            switch ( aExtract )
                {
                case PEP_COMM_FLOW_CTRL_EXTRACT:
                    {
                    iFlowCtrlCharExtraction = ETrue;
                    break;
                    }
                case PEP_COMM_FLOW_CTRL_NO_EXTRACT:
                    {
                    iFlowCtrlCharExtraction = EFalse;
                    break;
                    }
                default:
                    {
                    LOG1("  ERROR unsupported extract status: %d", aExtract );
                    OstTraceExt1( TRACE_NORMAL, DUP3_CDPDATACONFIG_SETFLOWCONTROLCONFIG, "CDpDataConfig:: ERROR unsupported extract status: %hhu", aExtract );

                    ret = KErrNotSupported;
                    }
                }

            if ( KErrNone == ret )
                {
                if ( iPeerConfig.iHandshake != handshake )
                    {
                    // turn change flags on
                    iPeerConfig.iChangedMembers |= KHandshakeChanged;

                    iPeerConfig.iHandshake = handshake;
                    iConfig.iHandshake = iPeerConfig.iHandshake;

                    // we should indicate client
                    CompleteConfigChangeNotify();
                    }
                //no else
                }
            //no else
            }
        //no else
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetEchoDte
// This method sets DTE interface echo mode. This method is
// called from COMM PEP.
// ---------------------------------------------------------
//
TInt CDpDataConfig::SetEchoDte(
    const TUint8 aEcho)
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_SETECHODTE, "CDpDataConfig::SetEchoDte" );
    LOGM1("  CDpDataConfig::SetEchoDte, aEcho: %d", aEcho );

    TInt ret( KErrNone );

    switch ( aEcho )
        {
        case PEP_COMM_ECHO_ON:
            {
            iEchoDte = ETrue;
            break;
            }
        case PEP_COMM_ECHO_OFF:
            {
            iEchoDte = EFalse;
            break;
            }
        default:
            {
            LOG1("  ERROR, CDpDataConfig::SetEchoDte, Not supported echo value %d",
                aEcho );
            OstTraceExt1( TRACE_NORMAL, DUP1_CDPDATACONFIG_SETECHODTE, "ERROR, CDpDataConfig::SetEchoDte, Not supported echo value %hhu", aEcho );

            ret = KErrNotSupported;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::WaterMarkHighSize
// Size of xx% of buffer size (typically 75% or 50% of buffer
// size) e.g. if buffer size is 1024 and '%' is 50% returns
// 512.
// ---------------------------------------------------------
//
TInt CDpDataConfig::WaterMarkHighSize(
    CDpDataBuffer& aBuffer )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_WATERMARKHIGHSIZE, "CDpDataConfig::WaterMarkHighSize" );
    LOGM(" CDpDataConfig::WaterMarkHighSize");

    return ( ( aBuffer.Size() * KDpFlowCtrlWaterMarkHighProsOfBuff ) / 100 );
    }

// ---------------------------------------------------------
// CDpDataConfig::WaterMarkLowSize
// Size of x% of buffer size (typically 25% of buffer size).
// ---------------------------------------------------------
//
TInt CDpDataConfig::WaterMarkLowSize(
    CDpDataBuffer& aBuffer )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_WATERMARKLOWSIZE, "CDpDataConfig::WaterMarkLowSize" );
    LOGM(" CDpDataConfig::WaterMarkLowSize");

    return ( ( aBuffer.Size() * KDpFlowCtrlWaterMarkLowProsOfBuff ) / 100 );
    }

// ---------------------------------------------------------
// CDpDataConfig::IsLineFail
// True if there is line fail. i.e. Client has configured
// some signal example CTS --> KConfigFailCTS. Now when CTS
// goes down it is a line fail.
// ---------------------------------------------------------
//
TBool CDpDataConfig::IsLineFail(
    TCommRole aRole ) const
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_ISLINEFAIL, "CDpDataConfig::IsLineFail" );
    LOGM(" CDpDataConfig::IsLineFail");

    TBool ret( EFalse );

    if ( ECommRoleDTE == aRole )
        {
        if ( ( !( iSignals & KSignalDSR ) &&
                ( iConfig.iHandshake & KConfigFailDSR ) ) ||
            ( !( iSignals & KSignalDCD ) &&
                ( iConfig.iHandshake & KConfigFailDCD ) ) )
            {
            ret = ETrue;
            }
        //no else
        }
    else
        {
        if ( ( !( iSignals & KSignalRTS ) &&
                ( iConfig.iHandshake & KConfigFailRTS ) ) ||
            ( !( iSignals & KSignalDTR ) &&
                ( iConfig.iHandshake & KConfigFailDTR ) ) )
            {
            ret = ETrue;
            }
        //no else
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::Terminator
// This method gets DTE interface terminator character.
// ---------------------------------------------------------
//
TText8 CDpDataConfig::Terminator(
    const TUint aIndex )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_TERMINATOR, "CDpDataConfig::Terminator" );
    LOGM(" CDpDataConfig::Terminator");

    return iConfig.iTerminator[aIndex];
    }

// ---------------------------------------------------------
// CDpDataConfig::Rate
// Solves out the bit rate
// ---------------------------------------------------------
//
TInt CDpDataConfig::Rate(
    TCommConfigV01& aConfig,
    TUint8& aRate)
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_RATE, "CDpDataConfig::Rate" );
    OstTraceExt1( TRACE_NORMAL, DUP1_CDPDATACONFIG_RATE, "CDpDataConfig::Rate, Config rate: %hhu", aRate );

    LOGM1(" CDpDataConfig::Rate, Config rate: %d.", aRate );

    TInt ret( KErrNone );

    switch ( aConfig.iRate )
        {
        case EBps2400:
            {
            aRate = EPEP_COMM_RATE_2400;
            break;
            }
        case EBps4800:
            {
            aRate = EPEP_COMM_RATE_4800;
            break;
            }
        case EBps9600:
            {
            aRate = EPEP_COMM_RATE_9600;
            break;
            }
        case EBps19200:
            {
            aRate = EPEP_COMM_RATE_19200;
            break;
            }
        case EBps38400:
            {
            aRate = EPEP_COMM_RATE_38400;
            break;
            }
        case EBps57600:
            {
            aRate = EPEP_COMM_RATE_57600;
            break;
            }
        case EBps115200:
            {
            aRate = EPEP_COMM_RATE_115200;
            break;
            }
        default:
            {
            LOG1("  ERROR, CDpDataConfig::Rate, Unsupported rate value: %d.",
                aRate );
            OstTraceExt1( TRACE_NORMAL, DUP2_CDPDATACONFIG_RATE, "CDpDataConfig:: ERROR, Unsupported rate value:%hhu", aRate );

            ret = KErrNotSupported;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::DataBits
// Solves out data bits.
// ---------------------------------------------------------
//
TInt CDpDataConfig::DataBits(
    TCommConfigV01& aConfig,
    TUint8& aDataBits)
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_DATABITS, "CDpDataConfig::DataBits" );
    OstTraceExt1( TRACE_NORMAL, DUP1_CDPDATACONFIG_DATABITS, "CDpDataConfig::DataBits: %hhu in DataConfig::DataBits", aDataBits );

    LOGM(" CDpDataConfig::DataBits");
    LOGM1("  DataBits: %d in DataConfig::DataBits.", aDataBits );

    TInt ret( KErrNone );

    switch ( aConfig.iDataBits )
        {
        case EData7:
            {
            aDataBits = PEP_COMM_DATA_BITS_7;
            break;
            }
        case EData8:
            {
            aDataBits = PEP_COMM_DATA_BITS_8;
            break;
            }
        default:
            {
            LOG1("  ERROR, CDpDataConfig::DataBits, Unsupported DataBits: %d.",
                aDataBits);
            OstTraceExt1( TRACE_NORMAL, DUP2_CDPDATACONFIG_DATABITS, "CDpDataConfig:: ERROR, Unsupported DataBits: %hhu", aDataBits );

            ret =KErrNotSupported;
            }
         }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::StopBits
// Solves out the stop bits
// ---------------------------------------------------------
//
TInt CDpDataConfig::StopBits(
    TCommConfigV01& aConfig,
    TUint8& aStopBits)
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_STOPBITS, "CDpDataConfig::StopBits" );
    OstTraceExt1( TRACE_NORMAL, DUP2_CDPDATACONFIG_STOPBITS, "CDpDataConfig::StopBits: %hhu", aStopBits );

    LOGM1(" CDpDataConfig::StopBits, StopBits: %d.", aStopBits );

    TInt ret( KErrNone );

    switch ( aConfig.iStopBits )
        {
        case EStop1:
            {
            aStopBits = PEP_COMM_STOP_BITS_1;
            break;
            }
        case EStop2:
            {
            aStopBits = PEP_COMM_STOP_BITS_2;
            break;
            }
        default:
            {
            LOG1("  ERROR, CDpDataConfig::StopBits, Unsupported StopBits: %d.",
                aStopBits );
            OstTraceExt1( TRACE_NORMAL, DUP1_CDPDATACONFIG_STOPBITS, "ERROR, CDpDataConfig::StopBits, Unsupported StopBits: %hhu", aStopBits );

            ret = KErrNotSupported;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::Parity
// Solves out the parity bit
// ---------------------------------------------------------
//
TInt CDpDataConfig::Parity(
    TCommConfigV01& aConfig,
    TUint8& aParity )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_PARITY, "CDpDataConfig::Parity" );
    OstTraceExt1( TRACE_NORMAL, DUP1_CDPDATACONFIG_PARITY, "CDpDataConfig::Parity: %hhu", aParity );

    LOGM1(" CDpDataConfig::Parity, Parity: %d.", aParity );

    TInt ret( KErrNone );

    switch ( aConfig.iParity )
        {
        case EParityNone:
            {
            aParity = PEP_COMM_PARITY_NONE;
            break;
            }
        case EParityEven:
            {
            aParity = PEP_COMM_PARITY_EVEN;
            break;
            }
        case EParityOdd:
            {
            aParity = PEP_COMM_PARITY_ODD;
            break;
            }
        case EParityMark:
            {
            aParity = PEP_COMM_PARITY_MARK;
            break;
            }
        case EParitySpace:
            {
            aParity = PEP_COMM_PARITY_SPACE;
            break;
            }
        default:
            {
            LOG1("  ERROR, CDpDataConfig::Parity, Unsupported parity: %d.",
                aParity );
            OstTraceExt1( TRACE_NORMAL, DUP2_CDPDATACONFIG_PARITY, "ERROR, CDpDataConfig::Parity, Unsupported parity: %hhu", aParity );

            ret = KErrNotSupported;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataConfig::HandleDteSignalMapping
// Maps the DTE type signals.
// ---------------------------------------------------------
//
void CDpDataConfig::HandleDteSignalMapping(
    const TUint aSignals,
    TDes8& aData )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_HANDLEDTESIGNALMAPPING, "CDpDataConfig::HandleDteSignalMapping" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPDATACONFIG_HANDLEDTESIGNALMAPPING, "CDpDataConfig:: aSignals: %u\n", aSignals );
    OstTrace1( TRACE_NORMAL, DUP2_CDPDATACONFIG_HANDLEDTESIGNALMAPPING, "CDpDataConfig:: aHandshake: %u\n", iConfig.iHandshake );

    LOGM(" CDpDataConfig::HandleDteSignalMapping");
    LOG1("  HandleDteSignalsMapping - aSignals: %x\n", aSignals );
    LOG1("  HandleDteSignalsMapping - aHandshake: %x\n", iConfig.iHandshake );

    TUint8 state( 0 );

    if ( ( iConfig.iHandshake & KConfigFreeDTR ) )
        {
        LOG("  CDpDataConfig::HandleDteSignalMapping, iConfig.iHandshake & KConfigFreeDTR == TRUE");
        OstTrace0( TRACE_NORMAL, DUP3_CDPDATACONFIG_HANDLEDTESIGNALMAPPING, "CDpDataConfig:: iConfig.iHandshake and KConfigFreeDTR == TRUE" );

        // This is little bit simplified. Win2000 doesn't send DTR signal properly
        if ( ( aSignals & KDTRChanged ) )
            {
            if ( aSignals & KSignalDTR )
                {
                LOG("  DTR signal ON");
                OstTrace0( TRACE_NORMAL, DUP4_CDPDATACONFIG_HANDLEDTESIGNALMAPPING, "CDpDataConfig:: DTR signal ON" );

                state = PEP_COMM_SIGNAL_DTR_ON;
                }
            else
                {
                LOG("  DTR signal OFF");
                OstTrace0( TRACE_NORMAL, DUP5_CDPDATACONFIG_HANDLEDTESIGNALMAPPING, "CDpDataConfig:: DTR signal OFF" );

                state = PEP_COMM_SIGNAL_DTR_OFF;
                }
            }
        }
    //no else

    aData.Append( state );
    }

// ---------------------------------------------------------
// CDpDataConfig::HandleDceSignalMapping
// Maps the DCE type signals
// ---------------------------------------------------------
//
void CDpDataConfig::HandleDceSignalMapping(
    const TUint aSignals,
    TDes8& aData )
    {
    OstTrace0( TRACE_NORMAL, CDPDATACONFIG_HANDLEDCESIGNALMAPPING, "CDpDataConfig::HandleDceSignalMapping" );
    LOGM(" CDpDataConfig::HandleDceSignalMapping");

    TUint8 state( 0 );

    if ( ( iConfig.iHandshake & KConfigFreeDSR ) )
        {
        if ( ( ( aSignals & KSignalDSR) != (iSignals & KSignalDSR) ) ||
            ( ( aSignals & KDSRChanged ) && ( aSignals & KSignalDSR ) ) )
            {
            if ( aSignals & KSignalDSR )
                {
                state |= PEP_COMM_SIGNAL_DSR_ON;
                }
            else
                {
                state |= PEP_COMM_SIGNAL_DSR_OFF;
                }
            }
       //no else
       }
    //no else

    if ( ( iConfig.iHandshake & KConfigFreeDCD ) )
        {
        if ( ( ( aSignals & KSignalDCD ) != (iSignals & KSignalDCD) ) ||
            ( ( aSignals & KDCDChanged ) && ( aSignals & KSignalDCD) ) )
            {
            if ( aSignals & KSignalDCD )
                {
                state |= PEP_COMM_SIGNAL_DCD_ON;
                }
            else
                {
                state |= PEP_COMM_SIGNAL_DCD_OFF;
                }
            }
        //no else
        }
    //no else

    if ( ( iConfig.iHandshake & KConfigFreeRI ) )
        {
        if ( ( ( aSignals & KSignalRNG ) != ( iSignals & KSignalRNG ) ) ||
            ( ( aSignals & KRNGChanged ) && ( aSignals & KSignalRNG) ) )
            {
            if ( aSignals & KSignalRNG )
                {
                state |= PEP_COMM_SIGNAL_RI_ON;
                }
            else
                {
                state |= PEP_COMM_SIGNAL_RI_OFF;
                }
            }
        //no else
        }
    //no else

    aData.Append( state );
    }

//  OTHER EXPORTED FUNCTIONS
//  none

//  End of File
