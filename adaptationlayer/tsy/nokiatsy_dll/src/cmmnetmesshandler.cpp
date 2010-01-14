/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmnetmesshandler.h"
#include "cmmnetoperatornamehandler.h"
#include "toperatornames.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h"
#include "cmmmessagerouter.h"
#include "cmmpacketmesshandlercontextlist.h"
#include "cmmpacketservicemesshandler.h"
#include "tsylogger.h" // Logging
#include "cmmuiccmesshandler.h"

#include <ctsy/serviceapi/mmgsmwcdmautils.h> // For mapping utilities
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_defaults.h> // KMmTsyNITZCaps needed
#include <etelmm.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <net_modemisi.h>
#include <infoisi.h>
#include <tisi.h>
#include <gpdsisi.h>
//#include <permisi.h> To be done in CPS
#include <uiccisi.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmnetmesshandlerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
// Hard-coded transaction id for ISA messages
const TUint8 KNetTransId = 4;
// const TUint8 KNbImsSubBlocks = 3; Compiler warning removal

// Default value for registered in another network in GSM
const TUint8 KDefaultRegInAnotherNwGsm = 0;

// Operator code for (MCC MNC): 262 07
_LIT8( KO2NetworkId, "\x62\xF2\x70" );
// HPLMN read from SIM (MCC MNC): 262 08
_LIT8( KO2Hplmn, "\x62\xF2\x80" );

#ifdef OST_TRACE_COMPILER_IN_USE
const TUint8 KBufferForOSTTracing = 100;
#endif
#ifdef TF_LOGGING_ENABLED
const TUint8 KBufferForTFLogging = 130;
#endif

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
// limit values for mapping RSSI percentage to number of signal strength bars
const TUint8 netRssiLimitsTable[KMaxSignalStrengthBars] =
     {
     1, 16, 33, 49, 66, 83, 99
     };

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TFLOG_PRINT_ISIMESSAGE
// local function for printing queued Net message contents
// -----------------------------------------------------------------------------
//
#if defined (TF_LOGGING_ENABLED) || defined (OST_TRACE_COMPILER_IN_USE)
static void TFLOG_PRINT_ISIMESSAGE( const TDesC8& aBuf )
    {
    _LIT( KTraceSpace,           " " );

    // Get the length of the ISI message.
    TInt length = aBuf.Length();

#ifdef TF_LOGGING_ENABLED
    TBuf<KBufferForTFLogging> msg;
    _LIT( KTraceQueuedMessage,  "TSY:Queued NET message:  [ " );
    _LIT( KTraceBracketClose,    "]" );

    msg.Append( KTraceQueuedMessage );

    if ( 250 < length )
        {
        // Print only 250 first bytes.
        length = 250;
        }

    for ( TInt i = 0; i < length; i++ )
        {
        msg.AppendNumFixedWidthUC( aBuf[i], EHex, 2 );
        msg.Append( KTraceSpace );
        if ( KLogSizeOfBuffer-10 < msg.Length() )
            {
            if ( i == ( length - 1 ) )
                {
                // Last number, append "]" character.
                msg.Append( KTraceBracketClose );
                }

            TFLOGTEXT( msg );
            msg.SetLength( 0 );
            }
        }

    if ( 0 < msg.Length() )
        {
        // Buffer contains data. Print also last line.
        msg.Append( KTraceBracketClose );
        TFLOGTEXT( msg );
        }
#endif // TF_LOGGING_ENABLED

    // Ost tracing.
#ifdef OST_TRACE_COMPILER_IN_USE
    TBuf8<KBufferForOSTTracing> trace_msg;
    TUint8 counter = 1;
    TBool firstTime = ETrue;

    for ( TInt i = 0; i < length; i++ )
        {
        trace_msg.AppendNumFixedWidthUC( aBuf[i], EHex, 2 );
        trace_msg.Append( KTraceSpace );

        if ( ( counter > 24 ) ||     // 25 bytes / line.
            ( ( i + 1 ) == length ) ) // All bytes collected.
            {
            if ( ( firstTime ) &&
                ( ( i + 1 ) == length ) ) // All bytes collected and traced.
                {
                firstTime = EFalse;
OstTraceExt1( TRACE_NORMAL, DUP6__TFLOG_PRINT_ISIMESSAGE, "TSY:Queued NET message: [ %s]", trace_msg );
                trace_msg.SetLength( 0 );
                }
            else if ( firstTime ) // 1st line of the trace.
                {
                firstTime = EFalse;
OstTraceExt1( TRACE_NORMAL, DUP7__TFLOG_PRINT_ISIMESSAGE, "TSY:Queued NET message: [ %s", trace_msg );
                trace_msg.SetLength( 0 );
                }
            else if ( ( i + 1 ) == length ) // The last line.
                {
OstTraceExt1( TRACE_NORMAL, DUP8__TFLOG_PRINT_ISIMESSAGE, "TSY:Queued NET message:   %s]", trace_msg );
                trace_msg.SetLength( 0 );
                }
            else // just print bytes.
                {
OstTraceExt1( TRACE_NORMAL, DUP9__TFLOG_PRINT_ISIMESSAGE, "TSY:Queued NET message:   %s", trace_msg );
                trace_msg.SetLength( 0 );
                }
            counter = 0;
            }
        counter++;
        }
#endif // OST_TRACE_COMPILER_IN_USE
    }

#else // TF_LOGGING_ENABLED || OST_TRACE_COMPILER_IN_USE

#define TFLOG_PRINT_ISIMESSAGE(x)

#endif // TF_LOGGING_ENABLED || OST_TRACE_COMPILER_IN_USE

// -----------------------------------------------------------------------------
// CalculateNetworkInformationSbStartOffset
// Calculates starting subblocks offset for NET_MODEM_REG_STATUS_IND,
// NET_SET_RESP and NET_MODEM_REG_STATUS_GET_RESP
// @param aIsiMessage ISI-message to process
// @param aSbStartOffset subblock start offset to initialize
// @return TInt KErrNone or KErrNotSupported
// -----------------------------------------------------------------------------
//
static TInt CalculateNetworkInformationSbStartOffset(
    const TIsiReceiveC& aIsiMessage,
    TUint& aSbStartOffset
    )
    {
    TInt ret( KErrNone );

    TFLOGSTRING("TSY: CalculateNetworkInformationSbStartOffset");
    OstTrace0( TRACE_NORMAL, _CALCULATENETWORKINFORMATIONSBSTARTOFFSET, "CalculateNetworkInformationSbStartOffset" );

    TUint8 messageId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );
    if ( NET_MODEM_REG_STATUS_IND == messageId )
        {
        TFLOGSTRING("TSY: CalculateNetworkInformationSbStartOffset; NET_REG_STATUS_IND");
        OstTrace0( TRACE_NORMAL, DUP1__CALCULATENETWORKINFORMATIONSBSTARTOFFSET, "CalculateNetworkInformationSbStartOffset; NET_REG_STATUS_IND" );
        aSbStartOffset = SIZE_NET_MODEM_REG_STATUS_IND;
        }
    else if ( NET_MODEM_REG_STATUS_GET_RESP == messageId )
        {
        TFLOGSTRING("TSY: CalculateNetworkInformationSbStartOffset; NET_REG_STATUS_GET_RESP");
        OstTrace0( TRACE_NORMAL, DUP2__CALCULATENETWORKINFORMATIONSBSTARTOFFSET, "CalculateNetworkInformationSbStartOffset; NET_REG_STATUS_GET_RESP" );
        aSbStartOffset = SIZE_NET_MODEM_REG_STATUS_GET_RESP;
        }
    else if ( NET_SET_RESP == messageId )
        {
        TFLOGSTRING("TSY: CalculateNetworkInformationSbStartOffset; NET_SET_RESP");
        OstTrace0( TRACE_NORMAL, DUP3__CALCULATENETWORKINFORMATIONSBSTARTOFFSET, "CalculateNetworkInformationSbStartOffset; NET_SET_RESP" );
        aSbStartOffset = SIZE_NET_SET_RESP;
        }
    else
        {
        TFLOGSTRING2("TSY: CalculateNetworkInformationSbStartOffset;unknown messageId=%x", messageId);
        OstTrace1( TRACE_NORMAL, DUP4__CALCULATENETWORKINFORMATIONSBSTARTOFFSET, "CalculateNetworkInformationSbStartOffset;unknown messageId=%x", messageId );
        ret = KErrNotSupported;
        }

    return ret;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmNetMessHandler::CMmNetMessHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmNetMessHandler::CMmNetMessHandler()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::CMmNetMessHandler() - Start");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_CMMNETMESSHANDLER, "CMmNetMessHandler::CMmNetMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmNetMessHandler* CMmNetMessHandler::NewL
        (
        CMmPhoNetSender* aPhoNetSender, // Pointer to the phonet sender
        CMmPhoNetReceiver* aPhoNetReceiver, // Pointer to the phonet receiver
        CMmMessageRouter* aMessageRouter, // Pointer to the message router
        CMmUiccMessHandler* aUiccMessHandler
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NEWL, "CMmNetMessHandler::NewL" );

    CMmNetMessHandler* netMessHandler = new ( ELeave ) CMmNetMessHandler();

    CleanupStack::PushL( netMessHandler );

    netMessHandler->iPhoNetSender = aPhoNetSender;
    netMessHandler->iMessageRouter = aMessageRouter;
    netMessHandler->iMmUiccMessHandler = aUiccMessHandler;
    netMessHandler->ConstructL();

    // NETWORK
    aPhoNetReceiver->RegisterL(
        netMessHandler,
        PN_MODEM_NETWORK );
    // GPDS
    aPhoNetReceiver->RegisterL(
        netMessHandler,
        PN_GPDS,
        GPDS_RADIO_ACTIVITY_IND );
    aPhoNetReceiver->RegisterL(
        netMessHandler,
        PN_GPDS,
        GPDS_CONTEXT_DEACTIVATE_IND );
    // INFO
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    aPhoNetReceiver->RegisterL(
        netMessHandler,
        PN_INFO,
        INFO_PP_READ_RESP );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
/*  To be replaced with INFO_PP_DATA_READ_RESP
    aPhoNetReceiver->RegisterL(
        netMessHandler,
        PN_MODEM_INFO,
        INFO_PP_READ_RESP );
*/
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    // PERMANENT_DATA
    /*aPhoNetReceiver->RegisterL(
        netMessHandler,
        PN_PERMANENT_DATA,
        PERM_PM_RECORD_WRITE_RESP );
    aPhoNetReceiver->RegisterL(
        netMessHandler,
        PN_PERMANENT_DATA,
        PERM_PM_RECORD_READ_RESP ); To be done in CPS */

    CleanupStack::Pop( netMessHandler );

    return netMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::ConstructL()");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_CONSTRUCTL, "CMmNetMessHandler::ConstructL" );

    iNetOperatorNameHandler = CMmNetOperatorNameHandler::NewL(
        this,
        iMmUiccMessHandler );

    // Delay timer for completion of Packet transfer state
    iDelayTimer = CDelayTimer::NewL( this );

    // Selection method is at this point unknown
    iSelectionMethod = NET_SELECT_MODE_UNKNOWN;

    // No cancel request has been issued yet
    iSelectNetworkCancelIssued = EFalse;
    iSearchNetworkCancelIssued = EFalse;

    // Initialize to EFalse
    iImsPinVerifyRequired = EFalse;
    iIsimUsed = EFalse;
    iMNClength = 0;

    iNetworkMode = RMobilePhone::ENetworkModeUnknown;
    iNetworkAccess = RMobilePhone::ENetworkAccessUnknown;
    iCell_DCH = EFalse;
    iPacketTransfer = EFalse;

    iNetMessageHandlingOngoing = EFalse;

    // Request network signal strength (RSSI)
    NetRssiGetReq();
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::~CMmNetMessHandler
// Destructor
// -----------------------------------------------------------------------------
//
CMmNetMessHandler::~CMmNetMessHandler()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::~CMmNetMessHandler() - End");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_CMMNETMESSHANDLER, "CMmNetMessHandler::~CMmNetMessHandler" );

    delete iNetOperatorNameHandler;

    for ( TInt i = 0; i < iNetMessageQueue.Count(); i++ )
        {
        delete iNetMessageQueue[i];
        }

    iNetMessageQueue.Close();

    // Delete delay timer
    if ( iDelayTimer )
        {
        delete iDelayTimer;
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::ProcessUiccMsg
// Handles data received from UICC server
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY: CMmNetMessHandler::ProcessUiccMsg, transaction ID: %d, status: %d", aTraId, aStatus );
OstTraceExt2( TRACE_NORMAL, CMMNETMESSHANDLER_PROCESSUICCMSG, "CMmNetMessHandler::ProcessUiccMsg;aTraId=%d;aStatus=%d", aTraId, aStatus );

    TInt ret( KErrNone );

    switch( aTraId )
        {
        case ETrIdReadCipheringIndicatorStatus:
            {
            UiccReadCiResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadCsDataEhplmn:
            {
            UiccCsReadEhpmlnResp( aStatus, aFileData );
            break;
            }
        case ETrIdReadCsDataHplmn:
            {
            UiccCsReadHpmlnResp( aStatus, aFileData );
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmNetMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_PROCESSUICCMSG, "CMmNetMessHandler::ProcessUiccMsg - - unknown transaction ID" );
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::ExtFuncL
// Dispatches Etel requests to DOS level handlers
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::ExtFuncL
        (
        TInt aIpc, // Ipc number
        const CMmDataPackage* aDataPackage // Packed data
        )
    {
TFLOGSTRING2("TSY: CMmNetMessHandler::ExtFuncL - Ipc: %d", aIpc);
OstTrace1( TRACE_NORMAL, CMMNETMESSHANDLER_EXTFUNCL, "CMmNetMessHandler::ExtFuncL;aIpc=%d", aIpc );

    //*************************************************************//
    // NOTE:
    //
    // LICENSEE SPECIFIC MESSAGE HANDLER IMPLEMENTATION STARTS HERE
    //
    //*************************************************************//

    TInt ret( KErrNone );

    TUint8 transId( KNetTransId );

      switch ( aIpc )
        {
        case EMobilePhoneSetNetworkSelectionSetting:
            {
            // Unpack the parameters for this case
            RMobilePhone::TMobilePhoneNetworkSelectionV1* networkSetting( NULL );
            aDataPackage->UnPackData( &networkSetting );

            // Forward the request to specific method
            NetNetworkSelectModeSetReq( transId, *networkSetting );
            break;
            }
        case EMobilePhoneSelectNetwork:
            {
            // Unpack parameters
            TBool* isManual( NULL );
            RMobilePhone::TMobilePhoneNetworkManualSelection* manualSelection( NULL );
            aDataPackage->UnPackData( &isManual, &manualSelection );

            // Forward the request to the specific method
            NetSetReq( transId,
                *isManual,
                manualSelection->iNetwork.Left( 4 ), // Operator code
                manualSelection->iCountry.Left( 3 ) ); // Country code
            break;
            }
        case EMobilePhoneSelectNetworkCancel:
            {
            NetSetCancelReq( transId );
            break;
            }
        case EMobilePhoneGetDetectedNetworksV2Phase1:
            {
            NetModemAvailableGetReq( transId );
            break;
            }
        case EMobilePhoneGetDetectedNetworksCancel:
            {
            NetAvailableCancelReq( transId );
            break;
            }
        case EMobilePhoneGetNetworkRegistrationStatus:
            {
            // We need to get the network mode before we get the registration
            // status as this second message doesn't inform in which mode
            // the network is working.
            // The net server only serves one request at a time, so there won't
            // be any race condition and mode will be available always in first
            // place.
            NetRatReq( transId );
            NetModemRegStatusGetReq( transId );
            break;
            }
        case EMobilePhoneGetHomeNetwork:
            {
            UiccCsReadReq();
            break;
            }
        case EMobilePhoneGetCipheringIndicatorStatus:
            {
            UiccReadCiReq();
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmNetMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_EXTFUNCL, "CMmNetMessHandler::ExtFuncL;Unknown IPC=%d", aIpc );
            ret = KErrNotSupported;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::ReceiveMessageL
// Called when an ISI message has been received.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::ReceiveMessageL
        (
        const TIsiReceiveC& aIsiMessage  // ISI-message received
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::ReceiveMessageL");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_RECEIVEMESSAGEL, "CMmNetMessHandler::ReceiveMessageL" );

    // Resource and message id
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

TFLOGSTRING3("TSY: CMmNetMessHandler::ReceiveMessageL - MessageId:%d. Resource: %d", messageId, resource);
OstTraceExt2( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_RECEIVEMESSAGEL, "CMmNetMessHandler::ReceiveMessageL;messageId=%d;resource=%d", messageId, resource );

    // Transaction id.
    TUint8 transactId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // NET functionality is implemented in NET server and SIM Server
    switch ( resource )
        {
        case PN_MODEM_NETWORK:
            {
            switch( messageId )
                {
                case NET_MODEM_REG_STATUS_IND:
                case NET_MODEM_REG_STATUS_GET_RESP:
                case NET_SET_RESP:
                    {
                    QueueNetModemRegStatusMessagesL( aIsiMessage );
                    break;
                    }
                case NET_MODEM_AVAILABLE_GET_RESP:
                    {
                    NetModemAvailableGetRespL( aIsiMessage );
                    break;
                    }
                case NET_AVAILABLE_CANCEL_RESP:
                    {
                    NetAvailableCancelResp( aIsiMessage );
                    break;
                    }
                case NET_SET_CANCEL_RESP:
                    {
                    NetSetCancelResp( aIsiMessage );
                    break;
                    }
                case NET_RAT_IND:
                    {
                    NetRatInd( aIsiMessage );
                    break;
                    }
                case NET_RAT_RESP:
                    {
                    NetRatResp( aIsiMessage );
                    break;
                    }
                case NET_NETWORK_SELECT_MODE_SET_RESP:
                    {
                    NetNetworkSelectModeSetResp( aIsiMessage );
                    break;
                    }
                case NET_RSSI_GET_RESP:
                    {
                    NetRssiGetResp( aIsiMessage );
                    break;
                    }
                case NET_RSSI_IND:
                    {
                    NetRssiInd( aIsiMessage );
                    break;
                    }
                case NET_TIME_IND:
                    {
                    NetTimeInd( aIsiMessage );
                    break;
                    }
                case NET_CIPHERING_IND:
                    {
                    NetCipheringInd( aIsiMessage );
                    break;
                    }
                case NET_RADIO_INFO_IND:
                    {
                    NetRadioInfoInd( aIsiMessage );
                    break;
                    }
                case NET_NITZ_NAME_IND:
                    {
                    iNetOperatorNameHandler->NetNitzNameInd(
                        aIsiMessage );
                    break;
                    }
                default:
                    {
                    // No appropriate handler methods for ISI-message found.
TFLOGSTRING("TSY: CMmNetMessHandler::ReceiveMessageL - no handler for the NETWORK message id");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_RECEIVEMESSAGEL, "CMmNetMessHandler::ReceiveMessageL - no handler for the NETWORK message id" );
                    // Server not known
                    break;
                    }
                } // End switch ( messageId )
            break; // End case PN_MODEM_NETWORK
            }
        case PN_GPDS:
            {
            if ( GPDS_RADIO_ACTIVITY_IND == messageId )
                {
                GpdsRadioActivityInd( aIsiMessage );
                }
            else if ( GPDS_CONTEXT_DEACTIVATE_IND == messageId )
                {
                GpdsContextDeactivateInd( aIsiMessage );
                }
            // No else
            break;
            }
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case PN_INFO:
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        case PN_MODEM_INFO:
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            {
            switch ( messageId )
                {
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
                // To be replaced with INFO_PP_DATA_READ_RESP
                case INFO_PP_READ_RESP:
                    {
                    if ( KNetTransId == transactId )
                        {
                        iNetOperatorNameHandler->InfoPpReadResp(
                            aIsiMessage );
                        }
                    break;
                    }
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
                default:
                    {
                    // No appropriate handler methods for ISI-message found.
TFLOGSTRING("TSY: CMmNetMessHandler::ReceiveMessageL - no handler for the INFO message id");
OstTrace0( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_RECEIVEMESSAGEL, "CMmNetMessHandler::ReceiveMessageL - no handler for the INFO message id" );
                    break;
                    }
                }
            break;
            }
        /*case PN_PERMANENT_DATA:
            {
            switch ( messageId )
                {
                case PERM_PM_RECORD_WRITE_RESP:
                    {
                    if ( KNetTransId == transactId )
                        {
                        iNetOperatorNameHandler->PermPmRecordWriteResp(
                            aIsiMessage );
                        }
                    break;
                    }
                case PERM_PM_RECORD_READ_RESP:
                    {
                    if ( KNetTransId == transactId )
                        {
                        iNetOperatorNameHandler->PermPmRecordReadResp(
                            aIsiMessage );
                        }
                    break;
                    }
                default:
                    {
                    // No appropriate handler methods for ISI-message found.
TFLOGSTRING("TSY: CMmNetMessHandler::ReceiveMessageL - no handler for the PERMANENT_DATA message id");
OstTrace0( TRACE_NORMAL, DUP5_CMMNETMESSHANDLER_RECEIVEMESSAGEL, "CMmNetMessHandler::ReceiveMessageL - no handler for the PERMANENT_DATA message id" );
                    break;
                    }
                }
            break;
            } To be done in CPS */
        default:
            {
TFLOGSTRING("TSY: CMmNetMessHandler::ReceiveMessageL - Switch resource case default");
OstTrace0( TRACE_NORMAL, DUP6_CMMNETMESSHANDLER_RECEIVEMESSAGEL, "CMmNetMessHandler::ReceiveMessageL - Switch resource case default" );
            break; // Server not known
            }
        } // End switch ( resource )
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetModemRegStatusInd
// Breaks a NET_MODEM_REG_STATUS_IND ISI-message.
// Completes NotifyCurrentNetworkChange, NotifyNetworkRegistrationStatusChange,
// NotifyNetworkSelectionSettingChange notifications (if required information
// is gathered) and EMobilePhoneSelectNetwork to SOS Tsy.
// Handles also ECustomGetOperatorNameIPC and completes it.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetModemRegStatusInd
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemRegStatusInd");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETMODEMREGSTATUSIND, "CMmNetMessHandler::NetModemRegStatusInd" );

    // Assistant for packaging.
    CMmDataPackage dataPackage;

    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;

    // Buffer for Operator code, length is 3.
    TBuf8<KBCDLength> operatorCode;

    // Network type initialization.
    TUint8 networkType( NET_GSM_NO_PLMN_AVAIL );

    // Camped in HPLMN value initialization.
    TBool campedInHplmn( EFalse );

    TUint sbStartOffSet( 0 );

    // Network mode received via NET_RAT_IND/NET_RAT_RESP.
    networkInfo.iMode = iNetworkMode;
        // look for subblock offset to get operator info from
    TInt returnValue( FindOperatorInfoSubBlock(
        aIsiMessage,
        sbStartOffSet ) );

    // If the returnValue is KErrNone then.
    if ( KErrNone == returnValue )
        {
        // Get the Location Area Code.
        locationArea.iLocationAreaCode = aIsiMessage.Get16bit(
            sbStartOffSet + NET_MODEM_CURRENT_CELL_INFO_OFFSET_CURRENTLAC );
        // Get the Cell ID.
        locationArea.iCellId = aIsiMessage.Get32bit(
            sbStartOffSet +
            NET_MODEM_CURRENT_CELL_INFO_OFFSET_CURRENTCELLID );
        // Area information is known.
        locationArea.iAreaKnown = ETrue;
        // Set status to current.
        networkInfo.iStatus = RMobilePhone::ENetworkStatusCurrent;
        // Band info is cdma specific.
        networkInfo.iBandInfo = RMobilePhone::EBandUnknown;
        // Sid is cdma specific, set it to default value.
        networkInfo.iCdmaSID.Zero();
        // Analog SID is cdma specific, set it to default value.
        networkInfo.iAnalogSID.Zero();
        // Get the operator code. Size is 3 bytes.
        operatorCode = aIsiMessage.GetData(
            sbStartOffSet + NET_MODEM_CURRENT_CELL_INFO_OFFSET_OPERATORCODE,
            3 );
            // Map the operator and country code to symbian values
            MapOperatorAndCountryCode(
                operatorCode,
                networkInfo.iNetworkId,
                networkInfo.iCountryCode );

        // Access technology received via NET_RAT_IND/NET_RAT_RESP.
        networkInfo.iAccess = iNetworkAccess;

        // Network type.
        networkType = aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_CURRENT_CELL_INFO_OFFSET_NETWORKTYPE );

        // Camped in HPLMN.
        campedInHplmn = aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_CURRENT_CELL_INFO_OFFSET_CAMPEDINHPLMN );

        // HSDPA availability indicator.
        // If TRUE then the cell supports HSDPA.
        networkInfo.iHsdpaAvailableIndicator = aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_CURRENT_CELL_INFO_OFFSET_HSDPAAVAILABLEINCELL );

TFLOGSTRING2("TSY: CMmNetMessHandler::NetModemRegStatusInd, HSDPA Available Indicator = %d.", networkInfo.iHsdpaAvailableIndicator);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_NETMODEMREGSTATUSIND, "CMmNetMessHandler::NetModemRegStatusInd, HSDPA Available Indicator=%hhu", networkInfo.iHsdpaAvailableIndicator );

// This is done for SUB 403-7200: NokiaTSY: HSUPA Cell Indicator for S60 (S60 3.2.2 / S_CPR8.1).
// This can be removed when actual implementation for HSUPA Cell Indicator is done.
// CASW_HSXPA_INDICATOR_SUPPORTED flag.
#if defined ( CASW_HSXPA_INDICATOR_SUPPORTED ) || defined ( INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT )

        TBool hsdpaAvailableIndicator( EFalse );
        TBool hsupaAvailableIndicator( EFalse );

        hsdpaAvailableIndicator = aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_CURRENT_CELL_INFO_OFFSET_HSDPAAVAILABLEINCELL );

        hsupaAvailableIndicator = aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_CURRENT_CELL_INFO_OFFSET_HSUPAAVAILABLEINCELL );

        if ( hsdpaAvailableIndicator || hsupaAvailableIndicator )
            {
            networkInfo.iHsdpaAvailableIndicator = ETrue;

TFLOGSTRING2("TSY: CMmNetMessHandler::NetModemRegStatusInd, HSDPA Available Indicator (HSDPA or HSUPA available) = %d.", networkInfo.iHsdpaAvailableIndicator);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_NETMODEMREGSTATUSIND, "CMmNetMessHandler::NetModemRegStatusInd, HSDPA Available Indicator (HSDPA or HSUPA available)=%hhu", networkInfo.iHsdpaAvailableIndicator );
            }
#endif // defined ( CASW_HSXPA_INDICATOR_SUPPORTED ) || defined ( INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT )
        }

    sbStartOffSet = 0;

    TUint8 registrationStatus( 0 );
    TInt retValue( ExtractNetworkRegistrationStatus(
        aIsiMessage,
        operatorCode,
        registrationStatus,
        sbStartOffSet ) );

    if ( KErrNone == retValue )
        {
        RMobilePhone::TMobilePhoneRegistrationStatus regStatus;

        // Map the selection mode to symbian values.
        MapNWAndRegStatus(
            registrationStatus,
            regStatus,
            networkInfo.iStatus );

        // Get network selection mode. Will be handled after CAUSE EXTENSION
        // handling.
        TUint8 networkSelectionMode( aIsiMessage.Get8bit(
            sbStartOffSet +
            NET_MODEM_REG_INFO_COMMON_OFFSET_NETWORKSELECTIONMODE ) );

        sbStartOffSet = 0;
        // Check whether NET_MODEM_CAUSE_EXTENSION subblock exists and get
        // the GSM specific error code from there.
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_MODEM_REG_STATUS_IND,
            NET_MODEM_CAUSE_EXTENSION,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // Get the gsm specific error code.
            TUint8 successCode ( aIsiMessage.Get8bit(
                sbStartOffSet +
                NET_MODEM_CAUSE_EXTENSION_OFFSET_REJECTCODE ) );

            // Map ISA error to Symbian values.
            retValue = CMmStaticUtility::CSCauseToEpocError(
                PN_MODEM_NETWORK,
                KTsyNetCauseGsm,
                successCode );
            }

        // Pack the data for sending to the manager.
        dataPackage.PackData( &regStatus, &networkInfo.iStatus );
        // Inform the manager that the network selection mode has changed.
        iMessageRouter->Complete(
            EMobilePhoneNotifyNetworkRegistrationStatusChange,
            &dataPackage,
            retValue );

        // Latest network selection mode saved and it will be used later,
        // when making NET_SET_REQuest.
        iSelectionMethod = networkSelectionMode;

        RMobilePhone::TMobilePhoneNetworkSelectionV1 newSelection;

        // Map the selection mode.
        MapNWSelectionMode( networkSelectionMode, newSelection );

        // Pack the data for sending to the manager.
        dataPackage.PackData( &newSelection );
        // Inform the manager that the network selection mode has changed.
        iMessageRouter->Complete(
            EMobilePhoneNotifyNetworkSelectionSettingChange,
            &dataPackage,
            retValue );

        // Check registration status. If phone is registered some network
        // then get correct operator name to show.
        //
        // Otherwise complete EMobilePhoneNotifyCurrentNetworkChange without
        // operator names.
        if ( ( NET_REG_STATUS_HOME == registrationStatus )
            || ( NET_REG_STATUS_ROAM == registrationStatus ) )
            {
            // Get correct name to show.
            iNetOperatorNameHandler->GetOperatorName(
                campedInHplmn,
                networkType,
                operatorCode,
                locationArea,
                networkInfo );
            }
        else
            {
            // Phone is not registered to any network.
            // Complete EMobilePhoneNotifyCurrentNetworkChange without
            // operator names.
            CompleteMobilePhoneNotifyCurrentNetworkChange(
                locationArea,
                networkInfo );
            }
        }
    else
        {
        // No NET_MODEM_REG_INFO_COMMON subblock found,
        // handling of message ends.

        // Continue handling queued messages.
        ContinueHandlingNetModemRegStatusMessages();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetModemRegStatusGetReq
// Called by ExtFuncL to construct a NET_MODEM_REG_STATUS_GET_REQ ISI
// message and send it to Phonet.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetModemRegStatusGetReq
        (
        TUint8 aTransactionId // Transaction id
        ) const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemRegStatusGetReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETMODEMREGSTATUSGETREQ, "CMmNetMessHandler::NetModemRegStatusGetReq" );

    // Data buffer length is 2
    TBuf8<2> data;
    data.Append( aTransactionId );
    data.Append( NET_MODEM_REG_STATUS_GET_REQ );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_NETWORK );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    return iPhoNetSender->Send( isimsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetModemAvailableGetReq
// Called by ExtFuncL to construct a NET_MODEM_AVAILABLE_GET_REQ ISI message
// and send it to Phonet.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetModemAvailableGetReq
        (
        TUint8 aTransactionId // Transaction id
        ) const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemAvailableGetReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETMODEMAVAILABLEGETREQ, "CMmNetMessHandler::NetModemAvailableGetReq" );

    // Append data for ISI message. Has room for mode, num of subBlocks
    // and NET_MODEM_GSM_BAND_INFO sb.
    TBuf8<2 + SIZE_NET_MODEM_GSM_BAND_INFO> netModemAvailableGetReq;
    netModemAvailableGetReq.Append( NET_MANUAL_SEARCH );

    // 1 subBlock
    netModemAvailableGetReq.Append( 1 );

    // Data buffer for net gsm band info sb.
    TBuf8<SIZE_NET_MODEM_GSM_BAND_INFO> netModemGsmBandInfoBuf( 0 );

    TIsiSubBlock netModemGsmBandInfoSb(
        netModemGsmBandInfoBuf,
        NET_MODEM_GSM_BAND_INFO,
        EIsiSubBlockTypeId8Len8 );

    netModemGsmBandInfoBuf.Append( NET_GSM_BAND_ALL_SUPPORTED_BANDS );

    netModemAvailableGetReq.Append( netModemGsmBandInfoSb.CompleteSubBlock() );

    return iPhoNetSender->Send(
        PN_MODEM_NETWORK,
        aTransactionId,
        NET_MODEM_AVAILABLE_GET_REQ,
        netModemAvailableGetReq );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetModemAvailableGetRespL
// Breaks a NET_MODEM_AVAILABLE_GET_RESP ISI-message.
// Creates a CMobilePhoneNetworkListV2 with the information of the ISI and
// completes EMobilePhoneGetDetectedNetworksV2Phase1 to SOS layer.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetModemAvailableGetRespL
        (
        const TIsiReceiveC& aIsiMessage
        ) const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemAvailableGetRespL");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETMODEMAVAILABLEGETRESPL, "CMmNetMessHandler::NetModemAvailableGetRespL" );

    // Check the success of the operation.
    TUint8 successCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_MODEM_AVAILABLE_GET_RESP_OFFSET_SUCCESSCODE ) );

    TUint8 nbOfSubBlocks( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_MODEM_AVAILABLE_GET_RESP_OFFSET_SUBBLOCKCOUNT ) );

    // Check if we have sub blocks in the message.
    if ( 0 < nbOfSubBlocks && NET_CAUSE_OK == successCode )
        {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemAvailableGetRespL - Success case");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_NETMODEMAVAILABLEGETRESPL, "CMmNetMessHandler::NetModemAvailableGetRespL - Success case" );

        // Handle NET_MODEM_AVAILABLE_GET_RESP ISI-message success case.
        iNetOperatorNameHandler->HandleManualSearchSuccessCaseL( aIsiMessage );
        }
    else
        {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemAvailableGetRespL - Error case");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_NETMODEMAVAILABLEGETRESPL, "CMmNetMessHandler::NetModemAvailableGetRespL - Error case" );

        TInt epocError( KErrGeneral );

        if ( !iSearchNetworkCancelIssued &&
            NET_CAUSE_REQUEST_INTERRUPTED == successCode )
            {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemAvailableGetRespL - Error case - MT call during network search");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_NETMODEMAVAILABLEGETRESPL, "CMmNetMessHandler::NetModemAvailableGetRespL - Error case - MT call during network search" );
            // In case of MT call during network search, NET server can
            // interrupt network search before client has chance to cancel
            // the search request. Thus in this case we need to convert the
            // error to more suitable like KErrAbort.
            epocError = CMmStaticUtility::EpocErrorCode(
                KErrAbort,
                KErrGsmNetCauseCallActive );
            }
        else
            {
TFLOGSTRING("TSY: CMmNetMessHandler::NetModemAvailableGetRespL - Error case - Cancel or Other error");
OstTrace0( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_NETMODEMAVAILABLEGETRESPL, "CMmNetMessHandler::NetModemAvailableGetRespL - Error case - Cancel or Other error" );
            // If some other error or cancel is called, then complete
            // with mapped error code.
            epocError = CMmStaticUtility::CSCauseToEpocError(
                PN_MODEM_NETWORK,
                KTsyNetCauseCommon,
                successCode );
            }

        // Complete client request.
        iMessageRouter->Complete(
            EMobilePhoneGetDetectedNetworksV2Phase1,
            epocError );
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetAvailableCancelReq
// Called by ExtFuncL to construct a NET_AVAILABLE_CANCEL_REQ ISI
// message and send it to Phonet. Enables the flag iSearchNetworkCancelIssued.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetAvailableCancelReq
        (
        TUint8 aTransactionId // Transaction id
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetAvailableCancelReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETAVAILABLECANCELREQ, "CMmNetMessHandler::NetAvailableCancelReq" );

    // Set the network search flag to ETrue.
    iSearchNetworkCancelIssued = ETrue;

    // Data buffer length is 2. Has room for transaction id and message id.
    TBuf8<2> data;
    data.Append( aTransactionId );
    data.Append( NET_AVAILABLE_CANCEL_REQ );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_NETWORK );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    return iPhoNetSender->Send( isimsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetAvailableCancelResp
// Breaks a NET_AVAILABLE_CANCEL_RESP ISI. Completes
// EMobilePhoneGetDetectedNetworksCancel to SOS layer. Disables the
// iSearchNetworkCancelIssued flag.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetAvailableCancelResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetAvailableCancelResp");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETAVAILABLECANCELRESP, "CMmNetMessHandler::NetAvailableCancelResp" );

    // Get success code
    TUint8 successCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_AVAILABLE_CANCEL_RESP_OFFSET_SUCCESSCODE ) );

    // Convert the error to Epoc Error
    TInt error( CMmStaticUtility::CSCauseToEpocError(
        PN_MODEM_NETWORK,
        KTsyNetCauseCommon,
        successCode ) );

    iMessageRouter->Complete(
        EMobilePhoneGetDetectedNetworksCancel,
        error );

    // The cancel has been completed,
    // therefore set the cancel issued flag off.
    iSearchNetworkCancelIssued = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetSetReq
// Called by ExtFuncL to construct a NET_SET_REQ ISI
// message and send it to Phonet. The correct selection mode is choosen
// according to previous network selections. The network is selected by MNC/MCC
// codes and not by index.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetSetReq
        (
        TUint8 aTransactionId,
        TBool aIsManual,
        const TOperatorCode& aMnc,
        const TCountryCode& aMcc
        ) const
    {
    // NW selection mode: Manual or reselection. Initialise variable.
    TUint8 networkSelectionMode( NET_SELECT_MODE_NO_SELECTION );

TFLOGSTRING2("TSY: CMmNetMessHandler::NetSetReq -- Manual NW selection(T/F): %d", aIsManual);
OstTrace1( TRACE_NORMAL, CMMNETMESSHANDLER_NETSETREQ, "CMmNetMessHandler::NetSetReq;aIsManual=%d", aIsManual );

TFLOGSTRING3("TSY: CMmNetMessHandler::NetSetReq -- MNC: %S MCC: %S", &aMnc, &aMcc);
OstTraceExt2( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_NETSETREQ, "CMmNetMessHandler::NetSetReq;aMnc=%S;aMcc=%S", aMnc, aMcc );

    // If NW selection mode is changed from manual to automatic
    if ( NET_SELECT_MODE_MANUAL == iSelectionMethod
        && !aIsManual )
        {
        networkSelectionMode = NET_SELECT_MODE_AUTOMATIC;
        }
    // If NW selection is set from automatic to automatic
    else if ( !aIsManual )
        {
        networkSelectionMode = NET_SELECT_MODE_USER_RESELECTION;
        }
    // If NW selection mode is set manual
    else if ( aIsManual )
        {
        networkSelectionMode = NET_SELECT_MODE_MANUAL;
        }

TFLOGSTRING2("TSY: CMmNetMessHandler::NetSetReq -- NW Selection Mode: %d", networkSelectionMode);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_NETSETREQ, "CMmNetMessHandler::NetSetReq;networkSelectionMode=%hhu", networkSelectionMode );

    // Append data for ISI message.
    // Has room for registered in another protocol, num of subBlocks,
    // net operator info common sb and net gsm operator info sb.
    TBuf8<2 + SIZE_NET_OPERATOR_INFO_COMMON + SIZE_NET_MODEM_GSM_OPERATOR_INFO>
        netSetReqData;

    // Registered in another protocol
    // if network mode has just changed, this should be 1.
    // GSM always 0.
    netSetReqData.Append( KDefaultRegInAnotherNwGsm );

    // Add number of Subblock
    if ( NET_SELECT_MODE_USER_RESELECTION != networkSelectionMode )
        {
        // 2 subBlocks
        netSetReqData.Append( 2 );
        }
    else
        {
        // 1 subBlock
        netSetReqData.Append( 1 );
        }

    if ( NET_SELECT_MODE_USER_RESELECTION != networkSelectionMode )
        {
TFLOGSTRING("TSY: CMmNetMessHandler::NetSetReq -- Adding NET_MODEM_GSM_OPERATOR_INFO SubBlock");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_NETSETREQ, "CMmNetMessHandler::NetSetReq, Adding NET_MODEM_GSM_OPERATOR_INFO SubBlock" );
        // We add NET_MODEM_GSM_OPERATOR_INFO subblock to ISI.
        // The information about the Band is not send trough MM ETEL Api, thus
        // the value is set to unknown

        // Size is 3 bytes for operator code
        TBuf8<3> operatorCode;
        CMmStaticUtility::FormOperatorCode( operatorCode, &aMnc, &aMcc );

        // Data buffer for NET_MODEM_GSM_OPERATOR_INFO sb.
        TBuf8<SIZE_NET_MODEM_GSM_OPERATOR_INFO> netModemGsmOperatorInfoBuf;
        TIsiSubBlock netModemGsmOperatorInfoSb( netModemGsmOperatorInfoBuf,
            NET_MODEM_GSM_OPERATOR_INFO,
            EIsiSubBlockTypeId8Len8 );

        netModemGsmOperatorInfoBuf.Append( operatorCode );
        netModemGsmOperatorInfoBuf.Append( NET_GSM_BAND_INFO_NOT_AVAIL );

        netSetReqData.Append( netModemGsmOperatorInfoSb.CompleteSubBlock() );
        }

TFLOGSTRING("TSY: CMmNetMessHandler::NetSetReq -- Adding NET_OPERATOR_INFO_COMMON SubBlock");
OstTrace0( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_NETSETREQ, "CMmNetMessHandler::NetSetReq, Adding NET_OPERATOR_INFO_COMMON SubBlock" );

    // Data buffer for net operator info common sb.
    TBuf8<SIZE_NET_OPERATOR_INFO_COMMON> operatorInfoCommonBuf;
    TIsiSubBlock operatorInfoCommonSb( operatorInfoCommonBuf,
        NET_OPERATOR_INFO_COMMON,
        EIsiSubBlockTypeId8Len8 );

    operatorInfoCommonBuf.Append( networkSelectionMode );
    operatorInfoCommonBuf.Append( NET_INDEX_NOT_USED );
    netSetReqData.Append( operatorInfoCommonSb.CompleteSubBlock() );

    return iPhoNetSender->Send(
        PN_MODEM_NETWORK,
        aTransactionId,
        NET_SET_REQ,
        netSetReqData );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetSetCancelReq
// Called by ExtFuncL to construct a NET_SET_CANCEL_REQ ISI
// message and send it to Phonet. Enables the flag iSelectNetworkCancelIssued.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetSetCancelReq
        (
        TUint8 aTransactionId
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetSetCancelReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETSETCANCELREQ, "CMmNetMessHandler::NetSetCancelReq" );

    // Set the network select flag to ETrue.
    // This flag is checked in NetSetCancelResp.
    iSelectNetworkCancelIssued = ETrue;

    // Data buffer length is 2.
    // Has room for transaction id and message id.
    TBuf8<2> data;
    data.Append( aTransactionId );
    data.Append( NET_SET_CANCEL_REQ );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_NETWORK );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    return iPhoNetSender->Send( isimsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetSetCancelResp
// Breaks a NET_SET_CANCEL_RESP ISI-message and completes
// EMobilePhoneSelectNetworkCancel to SOS Layer. Disables the
// iSelectNetworkCancelIssued flag.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetSetCancelResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetSetCancelResp");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETSETCANCELRESP, "CMmNetMessHandler::NetSetCancelResp" );

    // Get success code
    TUint8 successCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_SET_CANCEL_RESP_OFFSET_SUCCESSCODE ) );

    // Get correct error code from utility
    TInt errorCode( CMmStaticUtility::CSCauseToEpocError(
        PN_MODEM_NETWORK,
        KTsyNetCauseCommon,
        successCode ) );

    iMessageRouter->Complete( EMobilePhoneSelectNetworkCancel, errorCode );

    // The cancel has been completed,
    // therefore set the cancel issued flag off.
    iSelectNetworkCancelIssued = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetRatInd
// Breaks a NET_RAT_IND ISI-message.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetRatInd
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRatInd");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETRATIND, "CMmNetMessHandler::NetRatInd" );

    // Call NetRatResp to do the common job
    NetRatResp( aIsiMessage );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetRatReq
// Called by ExtFuncL to construct a NET_RAT_REQ ISI
// message and send it to Phonet.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetRatReq
        (
        TUint8 /* aTransactionId */
        ) const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRatReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETRATREQ, "CMmNetMessHandler::NetRatReq" );

    // Message data size is 1
    TBuf8<1> data;

    // we need to know the current RAT
    data.Append( NET_CURRENT_RAT );

    // Create the message. NOTE: the transId is set to DC because of
    // packet data is listening for the response to this message with that
    // transId the CMmNetMessHandler doesn't make transaction ID checking
    // anymore so this it's value is not relevant.
    // Message sent via Phonet and return.
    return iPhoNetSender->Send( PN_MODEM_NETWORK, 0xDC, NET_RAT_REQ, data );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetRatResp
// Breaks a NET_RAT_RESP ISI-message. Fetches the access technnology
// and sets the value in iNetworkMode and iNetworkAccess. Fetches the
// network mode and completes EMobilePhoneNotifyModeChange to SOS Tsy.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetRatResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRatResp");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETRATRESP, "CMmNetMessHandler::NetRatResp" );
    TUint sbStartOffSet( 0 );

    // NET_RAT_INFO sub block
    TInt retValue( aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_NET_RAT_RESP,
        NET_RAT_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) );

    // Check if NET_RAT_INFO sub block is present
    if( KErrNone == retValue )
        {
        // Get ratName
        TUint8 ratName( aIsiMessage.Get8bit(
            sbStartOffSet + NET_RAT_INFO_OFFSET_RATNAME ) );

        switch ( ratName )
            {
            case NET_GSM_RAT:
                {
                iNetworkMode = RMobilePhone::ENetworkModeGsm;
                iNetworkAccess = RMobilePhone::ENetworkAccessGsm;
TFLOGSTRING("TSY: CMmNetMessHandler::NetRatResp - network mode GSM");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_NETRATRESP, "CMmNetMessHandler::NetRatResp - network mode GSM" );
                break;
                }
            case NET_UMTS_RAT:
                {
                iNetworkMode = RMobilePhone::ENetworkModeWcdma;
                iNetworkAccess = RMobilePhone::ENetworkAccessUtran;
TFLOGSTRING("TSY: CMmNetMessHandler::NetRatResp - network mode UMTS");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_NETRATRESP, "CMmNetMessHandler::NetRatResp - network mode UMTS" );
                break;
                }
            default:
                {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRatResp. default network mode");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_NETRATRESP, "CMmNetMessHandler::NetRatResp, default network mode" );
                iNetworkMode = RMobilePhone::ENetworkModeUnknown;
                iNetworkAccess = RMobilePhone::ENetworkAccessUnknown;
                break;
                }
            }

        CMmDataPackage dataPackage;
        dataPackage.PackData( &iNetworkMode );

        iMessageRouter->Complete(
            EMobilePhoneNotifyModeChange,
            &dataPackage,
            KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetNetworkSelectModeSetReq
// Called by ExtFuncL to construct a NET_NETWORK_SELECT_MODE_SET_REQ ISI
// message and send it to Phonet.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetNetworkSelectModeSetReq
        (
        // Transaction id
        TUint8 aTransactionId,
        // Network selection
        const RMobilePhone::TMobilePhoneNetworkSelectionV1& aNetworkSelection
        ) const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetNetworkSelectModeSetReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETNETWORKSELECTMODESETREQ, "CMmNetMessHandler::NetNetworkSelectModeSetReq" );

    TInt ret( KErrNone );

    // Check the selection Mode from the selection structure
    TUint8 networkSelectionMode( NET_SELECT_MODE_UNKNOWN );

    if ( RMobilePhone::ENetworkSelectionAutomatic ==
        aNetworkSelection.iMethod )
        {
        networkSelectionMode = NET_SELECT_MODE_AUTOMATIC;
        }
    else if ( RMobilePhone::ENetworkSelectionManual ==
        aNetworkSelection.iMethod )
        {
        networkSelectionMode = NET_SELECT_MODE_MANUAL;
        }
    else
        {
        // All other settings are not supported
        ret = KErrNotSupported;
        }

    // Continue only if mode is correct
    if ( KErrNone == ret )
        {
        // Create network select mode set request
        // Message data size is 1 (network selection mode)
        TBuf8<1> data;
        data.Append( networkSelectionMode );

        // Send message via Phonet
        ret = iPhoNetSender->Send(
            PN_MODEM_NETWORK,
            aTransactionId,
            NET_NETWORK_SELECT_MODE_SET_REQ,
            data );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetNetworkSelectModeSetResp
// Breaks a NET_NETWORK_SELECT_MODE_SET_RESP ISI-message and completes
// EMobilePhoneSetNetworkSelectionSetting to SOS layer
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetNetworkSelectModeSetResp
        (
        const TIsiReceiveC& aIsiMessage
        ) const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetNetworkSelectModeSetResp");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETNETWORKSELECTMODESETRESP, "CMmNetMessHandler::NetNetworkSelectModeSetResp" );

    // Get success code
    TUint8 successCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE +
        NET_NETWORK_SELECT_MODE_SET_RESP_OFFSET_SUCCESSCODE ) );

    // Convert the error to Epoc Error
    TInt epocError( CMmStaticUtility::CSCauseToEpocError(
        PN_MODEM_NETWORK,
        KTsyNetCauseCommon,
        successCode ) );

    iMessageRouter->Complete(
        EMobilePhoneSetNetworkSelectionSetting,
        epocError );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetRssiGetReq
// Constructs a NET_RSSI_GET_REQ ISI message and sends it to Phonet.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::NetRssiGetReq() const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRssiGetReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETRSSIGETREQ, "CMmNetMessHandler::NetRssiGetReq" );

    // Data buffer length is 2 (message size minus transId, msgId)
    TBuf8< SIZE_NET_RSSI_GET_REQ - 2 > reqData;
    reqData.Append( NET_CS_GSM );            // CS type
    reqData.Append( NET_CURRENT_CELL_RSSI ); // measurement type

    return iPhoNetSender->Send(
        PN_MODEM_NETWORK,
        KNetTransId,
        NET_RSSI_GET_REQ,
        reqData );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetRssiGetResp
// Breaks a NET_RSSI_GET_RESP ISI message and calls HandleRssi.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetRssiGetResp
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    TUint8 successCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_RSSI_GET_RESP_OFFSET_SUCCESSCODE ) );

TFLOGSTRING2("TSY: CMmNetMessHandler::NetRssiGetResp, successcode=%d", successCode);
OstTraceExt1( TRACE_NORMAL, CMMNETMESSHANDLER_NETRSSIGETRESP, "CMmNetMessHandler::NetRssiGetResp;successCode=%hhu", successCode );

    if ( NET_CAUSE_OK == successCode )
        {
        TUint sbOffset( 0 );

        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_NET_RSSI_GET_RESP,
            NET_RSSI_CURRENT,
            EIsiSubBlockTypeId8Len8,
            sbOffset ) )
            {
            // Note: the unsigned value gathered from the ISI is percentages
            TUint8 bars( aIsiMessage.Get8bit(
                sbOffset + NET_RSSI_CURRENT_OFFSET_SIGNALBARS ) );

            // Get RSSI in dBm with inverse sign
            TUint8 rssiInDbm( aIsiMessage.Get8bit(
                sbOffset + NET_RSSI_CURRENT_OFFSET_RSSIINDBM ) );

            HandleRssi( bars, rssiInDbm );
            }
        } // No else, ignore missing RSSI data
    // No else, ignore failed RSSI measurement
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetRssiInd
// Breaks a NET_RSSI_IND ISI message and calls HandleRssi.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetRssiInd
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRssiInd");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETRSSIIND, "CMmNetMessHandler::NetRssiInd" );

    // Note: the unsigned value gathered from the ISI is percentages
    TUint8 bars( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_RSSI_IND_OFFSET_SIGNALSBAR ) );

    // Get RSSI in dBm with inverse sign
    TUint8 rssiInDbm( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_RSSI_IND_OFFSET_RSSIINDBM ) );

    HandleRssi( bars, rssiInDbm );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::HandleRssi
// Completes EMobilePhoneNotifySignalStrengthChange to SOS layer
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::HandleRssi
        (
        TUint8 aBarsIsi, // Bar percentage (ISI value)
        TUint8 aRSSIIsi // RSSI in dBm with inverse sign (ISI value)
        ) const
    {
    TInt8 i;
    TInt8 signalBars( 0 );

    // Bar percentage (0-100%) is converted to number of signal strength bars
    for ( i = KMaxSignalStrengthBars; i > 0; i-- )
        {
        if ( aBarsIsi >= netRssiLimitsTable[i-1] )
            {
            signalBars = i;
            break;
            }
        }

    // Copy RSSI in dBm into 32 bit integer
    TInt32 rssiInDbm( aRSSIIsi );

TFLOGSTRING3("TSY: CMmNetMessHandler::HandleRssi -- signalBars=%d rssiInDbm=%d", signalBars, aRSSIIsi);
OstTraceExt2( TRACE_NORMAL, CMMNETMESSHANDLER_HANDLERSSI, "CMmNetMessHandler::HandleRssi;signalBars=%hhd;rssiInDbm=%hhu", signalBars, aRSSIIsi );

    CMmDataPackage dataPackage;
    dataPackage.PackData( &signalBars, &rssiInDbm );

    iMessageRouter->Complete(
        EMobilePhoneNotifySignalStrengthChange,
        &dataPackage,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetTimeInd
// Breaks a NET_TIME_IND ISI and completes EMobilePhoneNotifyNITZInfoChange
// to SOS layer
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetTimeInd
        (
        const TIsiReceiveC& aIsiMessage
        ) const
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetTimeInd");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETTIMEIND, "CMmNetMessHandler::NetTimeInd" );

    TUint sbStartOffSet( 0 );

    TInt ret( aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_NET_TIME_IND,
        NET_TIME_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) );

    if ( KErrNone == ret )
        {
        // Get a year
        TUint8 year( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_YEAR ) );
        // Get a month
        TUint8 month( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_MONTH ) );
        // Get a day
        TUint8 day( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_DAY ) );
        // Get a hour
        TUint8 hour( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_HOUR ) );
        // Get a minute
        TUint8 minute( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_MINUTE ) );
        // Get a second
        TUint8 second( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_SECOND ) );
        // Get timezone
        TUint8 timezone( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_TIMEZONE ) );
        // Get daylight saving time (DTS)
        TUint8 dst( aIsiMessage.Get8bit(
            sbStartOffSet + NET_TIME_INFO_OFFSET_DAYLIGHTSAVINGTIME ) );

        // Symbian OS SDK: Using TDateTime
        // When setting the day and month, subtract one because the ranges are
        // offset from zero.
        month--;
        day--;
        // Year value changing to four digits
        const TInt KNitzYearOffset( 2000 ); // Adjusting the year value received
        TInt totalYear; // Four digits year

        totalYear = KNitzYearOffset + year;

TFLOGSTRING2("TSY: CMmNetMessHandler::NetTimeInd year: %d", totalYear );
OstTrace1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_NETTIMEIND, "CMmNetMessHandler::NetTimeInd;NetTimeInd year=%d", totalYear );

        // NITZ information
        RMobilePhone::TMobilePhoneNITZ timeDate;
        // Default caps, will be updated below if not valid.
        timeDate.iNitzFieldsUsed = KMmTsyNITZCaps;

        // Use Set method instead of constructor as the constructor
        // panics if data is incorrect. Set returns KErrGeneral in that case
        TInt retValue( timeDate.Set(
            totalYear,
            ( TMonth )month,
            day,
            hour,
            minute,
            second,
            0x00 ) ); // A micro second = 00

        // Check result of set
        if ( KErrNone != retValue )
            {
            // Set default values
            timeDate.Set( 0x00, ( TMonth ) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 );
            timeDate.iNitzFieldsUsed &= ~( RMobilePhone::KCapsTimeAvailable );
            }
        // If DST is valid, set it
        if ( NET_DST_INFO_NOT_AVAIL != dst )
            {
            timeDate.iDST = dst;
            }
        // If not, reset DST and remove it from caps
        else
            {
            timeDate.iDST = 0x00;
            timeDate.iNitzFieldsUsed &= ~( RMobilePhone::KCapsDSTAvailable );
            }

        // If Time Zone is valid, set it.
        // If the time zone is invalid or time zone is unknown,
        // all bits are set to 1 (including B6).
        if ( 0xFF != timezone )
            {
            timeDate.iTimeZone = timezone;
            }
        // If not, reset Time Zone and remove it from caps
        else
            {
            timeDate.iTimeZone = 0x00;
            timeDate.iNitzFieldsUsed &=
                ~( RMobilePhone::KCapsTimezoneAvailable );
            }

        CMmDataPackage dataPackage;
        dataPackage.PackData( &timeDate );

        iMessageRouter->Complete(
            EMobilePhoneNotifyNITZInfoChange,
            &dataPackage,
            retValue );
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::MapNWAndRegStatus
// Maps an ISA network registration status into a TMobilePhoneRegistrationStatus
// and a TMobilePhoneNetworkStatus symbian types
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::MapNWAndRegStatus
        (
        TUint8 aISARegistrationStatus, // Isa network registration status
        RMobilePhone::TMobilePhoneRegistrationStatus& aRegStatus, // Registration status
        RMobilePhone::TMobilePhoneNetworkStatus& aNWStatus // Network status
        )
    {
TFLOGSTRING2("TSY: CMmNetMessHandler::MapNWAndRegStatus. ISARegistrationStatus:%d", aISARegistrationStatus);
OstTrace1( TRACE_NORMAL, CMMNETMESSHANDLER_MAPNWANDREGSTATUS, "CMmNetMessHandler::MapNWAndRegStatus;aRegStatus=%d", aRegStatus );

     switch ( aISARegistrationStatus )
        {
        // Registered home
        case NET_REG_STATUS_HOME:
            {
            aNWStatus = RMobilePhone::ENetworkStatusCurrent;
            aRegStatus = RMobilePhone::ERegisteredOnHomeNetwork;
            break;
            }
        // Registered roaming
        case NET_REG_STATUS_ROAM:
            {
            aNWStatus = RMobilePhone::ENetworkStatusCurrent;
            aRegStatus = RMobilePhone::ERegisteredRoaming;
            break;
            }
        // Not in service
        case NET_REG_STATUS_NOSERV:
        case NET_REG_STATUS_NOSERV_SEARCHING:
            {
            aNWStatus = RMobilePhone::ENetworkStatusUnknown;
            aRegStatus = RMobilePhone::ENotRegisteredSearching;
            break;
            }
        // Not is service and not searching
        case NET_REG_STATUS_NOSERV_NOTSEARCHING:
        case NET_REG_STATUS_NSPS:
        case NET_REG_STATUS_NSPS_NO_COVERAGE:
            {
            aNWStatus = RMobilePhone::ENetworkStatusUnknown;
            aRegStatus = RMobilePhone::ENotRegisteredNoService;
            break;
            }
        // Not in service and no sim card
        case NET_REG_STATUS_NOSERV_NOSIM:
        case NET_REG_STATUS_NOSERV_SIM_REJECTED_BY_NW:
            {
            aNWStatus = RMobilePhone::ENetworkStatusUnknown;
            aRegStatus = RMobilePhone::ERegistrationDenied;
            break;
            }
        // Power off
        case NET_REG_STATUS_POWER_OFF:
            {
            aNWStatus = RMobilePhone::ENetworkStatusUnknown;
            aRegStatus = RMobilePhone::ERegistrationUnknown;
            break;
            }
        // Defaul case
        default:
            {
TFLOGSTRING("TSY: CMmNetMessHandler::MapNWAndRegStatus. default registration status");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_MAPNWANDREGSTATUS, "CMmNetMessHandler::MapNWAndRegStatus, default registration status" );
            aNWStatus = RMobilePhone::ENetworkStatusUnknown;
            aRegStatus = RMobilePhone::ERegistrationUnknown;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::MapNWSelectionMode
// Maps a ISA Network Selection mode into a TMobilePhoneNetworkSelectionV1
// Symbian type
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::MapNWSelectionMode
        (
        TUint8 aISASelectionMode, // Isa network selection mode
        RMobilePhone::TMobilePhoneNetworkSelectionV1& aSelectionMode // Network selection mode
        )
    {
TFLOGSTRING2("TSY: CMmNetMessHandler::MapNWSelectionMode. ISA selectionmode:%d", aISASelectionMode);
OstTraceExt1( TRACE_NORMAL, CMMNETMESSHANDLER_MAPNWSELECTIONMODE, "CMmNetMessHandler::MapNWSelectionMode;aISASelectionMode=%hhu", aISASelectionMode );

    if ( NET_SELECT_MODE_AUTOMATIC == aISASelectionMode )
        {
        aSelectionMode.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
        }
    else if ( NET_SELECT_MODE_MANUAL == aISASelectionMode )
        {
        aSelectionMode.iMethod = RMobilePhone::ENetworkSelectionManual;
        }
    else
        {
        aSelectionMode.iMethod = RMobilePhone::ENetworkSelectionUnknown;
        }

    // This is gsm specific function, set the default values for the CDMA
    // specific parameters
    aSelectionMode.iBandClass = RMobilePhone::ENetworkBandClassUnknown;
    aSelectionMode.iOperationMode = RMobilePhone::ENetworkOperationUnknown;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::MapOperatorAndCountryCode
// Maps a ISA operator code into a TMobilePhoneNetworkIdentity symbian type
// -----------------------------------------------------------------------------
//
TBool CMmNetMessHandler::MapOperatorAndCountryCode
        (
        const TDesC8& aIsaOperatorAndCountryCode,
        RMobilePhone::TMobilePhoneNetworkIdentity& aOperatorCode,
        RMobilePhone::TMobilePhoneNetworkCountryCode& aCountryCode,
        TInt aMNCLength
        )
    {
    TFLOGSTRING2("TSY: CMmNetMessHandler::MapOperatorAndCountryCode - MNCLenght: %d",
        aMNCLength);
OstTrace1( TRACE_NORMAL, CMMNETMESSHANDLER_MAPOPERATORANDCOUNTRYCODE, "CMmNetMessHandler::MapOperatorAndCountryCode;aMNCLength=%d", aMNCLength );

    // Country code mapping
    TUint16 mcc1( 0 );
    TUint16 mcc2( 0 );
    TUint16 mcc3( 0 );

    // Get the country code
    CMmStaticUtility::GetMccCodes(
        aIsaOperatorAndCountryCode,
        &mcc1,
        &mcc2,
        &mcc3 );

    if ( mcc1 < 10 ) //max one digit
        {
        aCountryCode.AppendNum( mcc1, EDecimal );
        }
    if ( mcc2 < 10 ) //max one digit
        {
        aCountryCode.AppendNum( mcc2, EDecimal );
        }
    if ( mcc3 < 10 ) //max one digit
        {
        aCountryCode.AppendNum( mcc3, EDecimal );
        }

    // Operator code mapping
    TUint16 mnc1( 0 );
    TUint16 mnc2( 0 );
    TUint16 mnc3( 0 );

    TBool networkCodeMapped( EFalse );

    // Get the network code
    CMmStaticUtility::GetMncCodes( aIsaOperatorAndCountryCode,
        &mnc1,
        &mnc2,
        &mnc3 );

   if ( mnc1 < 10 ) // Max one digit
        {
        aOperatorCode.AppendNum( mnc1, EDecimal );
        }
    if ( mnc2 < 10 && aMNCLength != 1 )
        {
        aOperatorCode.AppendNum( mnc2, EDecimal );
        }
    if ( mnc3 < 10 && aMNCLength != 1 && aMNCLength != 2 )
        {
        aOperatorCode.AppendNum( mnc3, EDecimal );
        }
    // This added due some operators in USA and Canada sends wrong
    // MNC codes, using the F value when it should be 0
    // e.g. anyone sends 17F instead of 170.
    else if ( mnc3 == 0x0F )
        {
        // Put mcc codes together
        TUint16 countryCode( (TUint16) ( 100 * mcc1 ) + ( 10 * mcc2 ) + mcc3 );

        // Check if the country code is USA (310) or Canada (302)
        if ( countryCode == 310 ||
            countryCode == 302 )
            {
            // Add 0 into the end
            aOperatorCode.AppendNum( 0, EDecimal );

            networkCodeMapped = ETrue;
            }
        }
    return networkCodeMapped;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::MapNwStatus
// Maps a ISA network status value into a TMobilePhoneNetworkStatus Symbian
// value.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::MapNwStatus
        (
        TUint8 aISANetworkStatus, // Isa network status value
        RMobilePhone::TMobilePhoneNetworkStatus& aNWStatus // Network status
        )
    {
TFLOGSTRING2("TSY: CMmNetMessHandler::MapNwStatus. ISANetworkStatus:%d", aISANetworkStatus);
OstTraceExt1( TRACE_NORMAL, CMMNETMESSHANDLER_MAPNWSTATUS, "CMmNetMessHandler::MapNwStatus;aISANetworkStatus=%hhu", aISANetworkStatus );

     switch ( aISANetworkStatus )
        {
        case NET_OPER_STATUS_UNKNOWN:
            {
            aNWStatus = RMobilePhone::ENetworkStatusUnknown;
            break;
            }
        case NET_OPER_STATUS_AVAILABLE:
            {
            aNWStatus = RMobilePhone::ENetworkStatusAvailable;
            break;
            }
        case NET_OPER_STATUS_CURRENT:
            {
            aNWStatus = RMobilePhone::ENetworkStatusCurrent;
            break;
            }
        case NET_OPER_STATUS_FORBIDDEN:
            {
            aNWStatus = RMobilePhone::ENetworkStatusForbidden;
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmNetMessHandler::MapNwStatus. mapped default network status");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_MAPNWSTATUS, "CMmNetMessHandler::MapNwStatus, mapped default network status" );
            aNWStatus = RMobilePhone::ENetworkStatusUnknown;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetCipheringInd
// Breaks a NET_CIPHERING_IND ISI and completes
// EMobileCallNotifyPrivacyConfirmation to SOS layer
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetCipheringInd
        (
        const TIsiReceiveC& aIsiMessage
        ) const
    {
    RMobilePhone::TMobilePhonePrivacy status =
        RMobilePhone::EPrivacyUnspecified;
    RMobilePhone::TMobilePhoneNetworkSecurity securityLevel =
        RMobilePhone::ECipheringOff;

    TUint8 cipheringStatus( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + NET_CIPHERING_IND_OFFSET_CIPHERINGSTATUS ) );

TFLOGSTRING2("TSY: CMmNetMessHandler::NetCipheringInd. CipheringStatus:%d", cipheringStatus);
OstTraceExt1( TRACE_NORMAL, CMMNETMESSHANDLER_NETCIPHERINGIND, "CMmNetMessHandler::NetCipheringInd;cipheringStatus=%hhu", cipheringStatus );

    switch( cipheringStatus )
        {
        case NET_CIPHERING_INDICATOR_OFF:
            {
            status = RMobilePhone::EPrivacyOff;
            //securityLevel is already initialized
            break;
            }
        case NET_CIPHERING_INDICATOR_ON:
            {
            status = RMobilePhone::EPrivacyOn;
            securityLevel = RMobilePhone::ECipheringGSM;
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmNetMessHandler::NetCipheringInd. no ciphering status");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_NETCIPHERINGIND, "CMmNetMessHandler::NetCipheringInd, no ciphering status" );
            break;
            }
        }

    CMmDataPackage package;
    package.PackData( &status );

    iMessageRouter->Complete(
        EMobileCallNotifyPrivacyConfirmation,
        &package,
        KErrNone );

    package.PackData( &securityLevel );

    iMessageRouter->Complete(
        EMobilePhoneNotifyNetworkSecurityLevelChange,
        &package,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::NetRadioInfoInd
// Breaks a NET_RADIO_INFO_IND ISI message. Saves UTRAN radio state information
// and completes NotifyCurrentNetworkChange if state changed.
// Location info is invalid (=empty) during Cell_DCH state.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::NetRadioInfoInd
        (
        const TIsiReceiveC& aIsiMessage    // Received ISI message
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRadioInfoInd");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd" );

    TBool oldCell_DCH = iCell_DCH;

    // Find NET_UTRAN_RADIO_INFO sub block
    TUint sbStartOffSet( 0 );

    TInt err( aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_NET_RADIO_INFO_IND,
        NET_UTRAN_RADIO_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) );

    if ( KErrNone == err )
        {
        TUint8 radioState( aIsiMessage.Get8bit(
            sbStartOffSet + NET_UTRAN_RADIO_INFO_OFFSET_RADIOSTATE ) );

        // iCell_DCH is true if and only if UTRAN radio state is Cell_DCH
TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, UTRAN radio state=%d.", radioState);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;UTRAN radioState=%hhu", radioState );

        iCell_DCH = ( ( NET_UTRAN_RADIO_CELL_DCH == radioState ) ?
            ETrue : EFalse );
        }
    else
        {
TFLOGSTRING("TSY: CMmNetMessHandler::NetRadioInfoInd, no UTRAN radio state info.");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd, no UTRAN radio state info" );
        iCell_DCH = EFalse;
        }

    // State changed?
TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, iCell_DCH = %d.", iCell_DCH);
TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, oldCell_DCH = %d.", oldCell_DCH);
OstTrace1( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;oldCell_DCH=%d", oldCell_DCH );

    if ( iCell_DCH != oldCell_DCH )
        {
        if ( iCell_DCH )
            {
            // State change Idle ==> Cell_DCH.
            // Complete with empty location info.
            RMobilePhone::TMobilePhoneLocationAreaV1 location;
            location.iAreaKnown = EFalse;
            location.iLocationAreaCode = iLastLocInfo.iLocationAreaCode;
            location.iCellId = iLastLocInfo.iCellId;

TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, location.iAreaKnown = %d.", location.iAreaKnown);
OstTrace1( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;location.iAreaKnown=%d", location.iAreaKnown );
TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, location.iLocationAreaCode = %d.", location.iLocationAreaCode);
OstTrace1( TRACE_NORMAL, DUP5_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;location.iLocationAreaCode=%d", location.iLocationAreaCode );
TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, location.iCellId = %d.", location.iCellId );
OstTrace1( TRACE_NORMAL, DUP6_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;location.iCellId=%d", location.iCellId );

            CMmDataPackage dataPackage;
            dataPackage.PackData( &iLastNWInfo, &location );

            iMessageRouter->Complete(
                EMobilePhoneNotifyCurrentNetworkChange,
                &dataPackage,
                KErrNone );
            }
        else
            {
            // State change Cell_DCH ==> Idle.
            // Complete with last saved location info.
            CMmDataPackage dataPackage;
            dataPackage.PackData( &iLastNWInfo, &iLastLocInfo );

            iMessageRouter->Complete(
                EMobilePhoneNotifyCurrentNetworkChange,
                &dataPackage,
                KErrNone );
            }
        }

TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, iLastLocInfo.iAreaKnown = %d.", iLastLocInfo.iAreaKnown);
OstTrace1( TRACE_NORMAL, DUP7_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;iLastLocInfo.iAreaKnown=%d", iLastLocInfo.iAreaKnown );
TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, iLastLocInfo.iLocationAreaCode = %d.", iLastLocInfo.iLocationAreaCode);
OstTrace1( TRACE_NORMAL, DUP8_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;iLastLocInfo.iLocationAreaCode=%d", iLastLocInfo.iLocationAreaCode );
TFLOGSTRING2("TSY: CMmNetMessHandler::NetRadioInfoInd, iLastLocInfo.iCellId = %d.", iLastLocInfo.iCellId);
OstTrace1( TRACE_NORMAL, DUP9_CMMNETMESSHANDLER_NETRADIOINFOIND, "CMmNetMessHandler::NetRadioInfoInd;iLastLocInfo.iCellId=%d", iLastLocInfo.iCellId );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::UiccReadCiReq
// Read ciphering information status from UICC
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::UiccReadCiReq()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::UiccReadCiReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_UICCREADCIREQ, "CMmNetMessHandler::UiccReadCiReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadCipheringIndicatorStatus;
    // We need to know the CI status info which is located in byte 3 in EF ad
    // b1=0: ciphering indicator feature disabled
    // b1=1: ciphering indicator feature enabled
    params.dataAmount = 1;
    params.dataOffset = 2;
    params.fileId = KElemFileAdministrativeData;
    params.fileIdSfi = 3;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::UiccReadCiResp
// Complete ciphering indicator status
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::UiccReadCiResp( TInt aStatus, const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::UiccReadCiResp");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_UICCREADCIRESP, "CMmNetMessHandler::UiccReadCiResp" );

    // Initialize ret value
    TInt ret( KErrNone );

    // Ciphering indicator information
    RMobilePhone::TMobileCallCipheringIndicator cipherIndicatorStatus
        ( RMobilePhone::ECipheringDisplayOff );

    if( UICC_STATUS_OK == aStatus  )
        {
        // Get ciphering indicator info ( bit 0)
        TUint8 cipheringIndicator(aFileData[0] & 0x01 );
        if ( cipheringIndicator )
            {
            cipherIndicatorStatus = RMobilePhone::ECipheringDisplayOn;
            }
        }
    else
        {
        ret = KErrNotFound;
        }
    CMmDataPackage package;
    package.PackData( &cipherIndicatorStatus );

    iMessageRouter->Complete( EMobilePhoneGetCipheringIndicatorStatus,
        &package,
        ret );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::UiccCsReadReq
// Read EHPLMN/HPLMN
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::UiccCsReadReq()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::UiccCsReadReq");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_UICCCSREADREQ, "CMmNetMessHandler::UiccCsReadReq" );

    // At first we try to read EHPLMN, SIM file '6FD9'
    // When response is received and if EHPLMN is not supported
    // we need to read HPLMN from IMSI (EF '6F07')

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadCsDataEhplmn;
    // Read EHPLMN, bytes 1-3 from '6FD9'
    params.dataAmount = 3;
    params.dataOffset = 0;
    params.fileId = KElemFileEhplmn;
    params.fileIdSfi = 0x1D;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::UiccCsReadEhpmlnResp
// Complet EHPLMN
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::UiccCsReadEhpmlnResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::UiccCsReadEhpmlnResp");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_UICCCSREADEHPMLNRESP, "CMmNetMessHandler::UiccCsReadEhpmlnResp" );

    // If status was OK, EHPLMN was supported
    if ( KErrNone ==  aStatus )
        {
        iHplmn = aFileData;
        }
    else // EHPLMN was not supported, read HPLMN
        {
        TUiccReadTransparent params;
        params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        params.trId = ETrIdReadCsDataHplmn;
        params.dataAmount = 4; // Bytes 2-5 contain MCC/MNC
        params.dataOffset = 1; // Read from second byte
        params.fileId = KElemFileImsi;
        params.fileIdSfi = 7;
        params.serviceType = UICC_APPL_READ_TRANSPARENT;

        // File id path
        params.filePath.Append( KMasterFileId >> 8 );
        params.filePath.Append( KMasterFileId );
        params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

        iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }

    if ( 0 != iHplmn.Length() )
        {
        SetSimNetworkInfo();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::UiccCsReadHpmlnResp
// Complete HPLMN
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::UiccCsReadHpmlnResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::UiccCsReadHpmlnResp");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_UICCCSREADHPMLNRESP, "CMmNetMessHandler::UiccCsReadHpmlnResp" );

    if ( KErrNone ==  aStatus )
        {
        TUint8 tempByte( 0 );
        // Coding of HPMLN is defined in TS 24.008
        // First byte is MCC2|MCC1
        tempByte = ( ( aFileData[1] & 0x0F ) << 4 ) |
            ( ( aFileData[0] & 0xF0 ) >> 4 );
        iHplmn.Append( tempByte );

        // Second byte is MNC3|MCC3
        tempByte = ( ( aFileData[3] & 0x0F ) << 4 ) |
            ( ( aFileData[1] & 0xF0 ) >> 4 );
        iHplmn.Append( tempByte );

        // Third byte is MNC2|MNC1
        tempByte = ( aFileData[2] & 0xF0 ) |
            ( aFileData[2] & 0x0F ) ;
        iHplmn.Append( tempByte );
        }

    if ( 0 != iHplmn.Length() )
        {
        SetSimNetworkInfo();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::SimNetworkInfo
// Breaks home PLMN information and starts procedure for getting network name
// information.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::SetSimNetworkInfo()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::SetSimNetworkInfo");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_SETSIMNETWORKINFO, "CMmNetMessHandler::SetSimNetworkInfo" );

    // iHomeInfo is declared in cmmnetmesshandler.h (class private parameter
    // to store the network information during the operator name is requested.

    // Remember to zero-fill the buffers before appending.
    iHomeInfo.iCountryCode.FillZ( 0 );
    iHomeInfo.iNetworkId.FillZ( 0 );
    iHomeInfo.iDisplayTag.FillZ( 0 );
    iHomeInfo.iLongName.FillZ( 0 );
    iHomeInfo.iShortName.FillZ( 0 );

    // Fill the mode, RAT and status, always the default ones.
    iHomeInfo.iMode = RMobilePhone::ENetworkModeGsm;
    iHomeInfo.iAccess = RMobilePhone::ENetworkAccessUnknown;
    iHomeInfo.iStatus = RMobilePhone::ENetworkStatusUnknown;

    // Make sure that hplmn is not empty.
    if ( 0 != iHplmn.Length() )
        {
        MapOperatorAndCountryCode(
            iHplmn,
            iHomeInfo.iNetworkId,
            iHomeInfo.iCountryCode,
            iMNClength );
        }

    // Initialize.
    TUint mccNumber( 0 ); // Country Code.
    TUint mncNumber( 0 ); // Network Code.

    // Convert descriptor contained number to integer.
    CMmStaticUtility::GetIntFromDescriptor(
        mccNumber,
        iHomeInfo.iCountryCode );
    CMmStaticUtility::GetIntFromDescriptor(
        mncNumber,
        iHomeInfo.iNetworkId );

TFLOGSTRING2("TSY: CMmNetMessHandler::SetSimNetworkInfo - CountryCode: %S", &iHomeInfo.iCountryCode);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_SETSIMNETWORKINFO, "CMmNetMessHandler::SetSimNetworkInfo - CountryCode=%S", iHomeInfo.iCountryCode );
TFLOGSTRING2("TSY: CMmNetMessHandler::SetSimNetworkInfo - NetworkId: %S", &iHomeInfo.iNetworkId);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_SETSIMNETWORKINFO, "CMmNetMessHandler::SetSimNetworkInfo - NetworkId=%S", iHomeInfo.iNetworkId );

    // Buffer for Display tag.
    TBuf<KDisplayTagLength> alphaTag;

    // Get Hard coded name from operators.h.
    TOperatorNames::GetName(
        mccNumber,
        mncNumber,
        alphaTag );

TFLOGSTRING2("TSY: CMmNetMessHandler::SetSimNetworkInfo - Name: %S", &alphaTag);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_SETSIMNETWORKINFO, "CMmNetMessHandler::SetSimNetworkInfo - Name=%S", alphaTag );

    // Copy hard coded name to iDisplayTag. Long and short names not handled
    // in this case and they will left empty.
    iHomeInfo.iDisplayTag.Copy( alphaTag );

    // Complete.
    CompleteMobilePhoneGetHomeNetwork();
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::HandleError
// Handles CMmNetMessHandler's errors that comes via PhoNetReceiver RunError
// method.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::HandleError
        (
        const TIsiReceiveC& /*aIsiMessage*/,    // Isi message
        TInt /*aError*/    // Error code
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::HandleError");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_HANDLEERROR, "CMmNetMessHandler::HandleError" );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::GetImsPinVerifyStatus
// Return status of SIM PIN verify
// -----------------------------------------------------------------------------
//
TBool CMmNetMessHandler::GetImsPinVerifyStatus()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::GetImsPinVerifyStatus");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_GETIMSPINVERIFYSTATUS, "CMmNetMessHandler::GetImsPinVerifyStatus" );

    return iImsPinVerifyRequired;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::GpdsRadioActivityInd
// Breaks a GPDS_RADIO_ACTIVITY_IND ISI message.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::GpdsRadioActivityInd
        (
        const TIsiReceiveC& aIsiMessage // Received ISI message
        )
    {
TFLOGSTRING2("TSY: CMmNetMessHandler::GpdsRadioActivityInd, NetworkMode = %d.", iNetworkMode);
OstTrace1( TRACE_NORMAL, CMMNETMESSHANDLER_GPDSRADIOACTIVITYIND, "CMmNetMessHandler::GpdsRadioActivityInd;iNetworkMode=%d", iNetworkMode );

    // Should be ignored in 3G (information not valid).
    if ( RMobilePhone::ENetworkModeGsm == iNetworkMode )
        {
        TUint8 radioActivityStatus( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_RADIO_ACTIVITY_IND_OFFSET_STATUS ) );
        // iPacketTransfer is true if and only
        // if radio activity state is GPDS_RADIO_ON
TFLOGSTRING2("TSY: CMmNetMessHandler::GpdsRadioActivityInd, radio activity state=%d.", radioActivityStatus);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_GPDSRADIOACTIVITYIND, "CMmNetMessHandler::GpdsRadioActivityInd;radio activity state=%hhu", radioActivityStatus );

        TBool newPacketTransfer = ( ( GPDS_RADIO_ON == radioActivityStatus ) ?
            ETrue : EFalse );

        if ( GPDS_RADIO_OFF == radioActivityStatus )
            {
            // GPDS_RADIO_OFF => delayed completion
            iDelayTimer->StartDelayTimer( newPacketTransfer,
                KGPDSRAICompleteDelay );
            }
        else
            {
            // Cancel timer
            iDelayTimer->Cancel();
            // GPDS_RADIO_ON => complete
            CompleteGpdsRadioActivityInd( newPacketTransfer );
            }
        }
    else
        {
        // Cancel timer
        iDelayTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::GpdsContextDeactivateInd
// Breaks a GPDS_CONTEXT_DEACTIVATE_IND ISI message.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::GpdsContextDeactivateInd
        (
        const TIsiReceiveC& /*aIsiMessage*/
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::GpdsContextDeactivateInd");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_GPDSCONTEXTDEACTIVATEIND, "CMmNetMessHandler::GpdsContextDeactivateInd" );
    // GPDS_RADIO_ACTIVITY_IND / RADIO_OFF is not received when
    // last context is closed => iPacketTransfer remains to ETrue
    // => simulate GPDS_RADIO_OFF from GPDS_CONTEXT_DEACTIVATE_IND.
    //
    // When PDP context is deactivated, it is still on the list at this point.
    if ( 1 >= iMessageRouter->GetPacketServiceMessHandler()->
        PacketContextList()->NumberOfContexts() )
        {
        iDelayTimer->StartDelayTimer( EFalse, KGPDSRAICompleteDelay );
        }
    // No else

    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::CompleteGpdsRadioActivityInd
// Saves GPDS radio activity information and
// completes NotifyCurrentNetworkChange if state changed.
// Location info is invalid (=empty) during Packet Transfer state.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::CompleteGpdsRadioActivityInd
        (
        TBool aNewPacketTrancferStatus
        )
    {
    // State changed?
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, NEW transfer status = %d.", aNewPacketTrancferStatus);
OstTrace1( TRACE_NORMAL, CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;NEW transfer status=%d", aNewPacketTrancferStatus );
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, OLD transfer status = %d.", iPacketTransfer);
OstTrace1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;OLD transfer status=%d", iPacketTransfer );

    if ( iPacketTransfer != aNewPacketTrancferStatus )
        {
        // Save new state
        iPacketTransfer = aNewPacketTrancferStatus;

        if ( iPacketTransfer )
            {
            // State change Packet Idle ==> Packet Transfer.
            // Complete with empty location info.
            RMobilePhone::TMobilePhoneLocationAreaV1 location;
            location.iAreaKnown = EFalse;
            location.iLocationAreaCode = iLastLocInfo.iLocationAreaCode;
            location.iCellId = iLastLocInfo.iCellId;

TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, location.iAreaKnown = %d.", location.iAreaKnown);
OstTrace1( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;location.iAreaKnown=%d", location.iAreaKnown );
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, location.iLocationAreaCode = %d.", location.iLocationAreaCode);
OstTrace1( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;location.iLocationAreaCode=%d", location.iLocationAreaCode );
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, location.iCellId = %d.", location.iCellId);
OstTrace1( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;location.iCellId=%d", location.iCellId );

            CMmDataPackage dataPackage;
            dataPackage.PackData( &iLastNWInfo, &location );
            iMessageRouter->Complete(
                EMobilePhoneNotifyCurrentNetworkChange,
                &dataPackage,
                KErrNone );
            }
        else
            {
            // State change Packet Transfer ==> Packet Idle.
            // Complete with last saved location info.
            CMmDataPackage dataPackage;
            dataPackage.PackData( &iLastNWInfo, &iLastLocInfo );
            iMessageRouter->Complete(
                EMobilePhoneNotifyCurrentNetworkChange,
                &dataPackage,
                KErrNone );
            }
        }
    // No else

TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, iLastLocInfo.iAreaKnown = %d.", iLastLocInfo.iAreaKnown);
OstTrace1( TRACE_NORMAL, DUP5_CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;iLastLocInfo.iAreaKnown=%d", iLastLocInfo.iAreaKnown );
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, iLastLocInfo.iLocationAreaCode = %d.", iLastLocInfo.iLocationAreaCode);
OstTrace1( TRACE_NORMAL, DUP6_CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;iLastLocInfo.iLocationAreaCode=%d", iLastLocInfo.iLocationAreaCode );
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteGpdsRadioActivityInd, iLastLocInfo.iCellId = %d.", iLastLocInfo.iCellId);
OstTrace1( TRACE_NORMAL, DUP7_CMMNETMESSHANDLER_COMPLETEGPDSRADIOACTIVITYIND, "CMmNetMessHandler::CompleteGpdsRadioActivityInd;iLastLocInfo.iCellId=%d", iLastLocInfo.iCellId );
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork
// This function completes EMobilePhoneGetHomeNetwork.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_COMPLETEMOBILEPHONEGETHOMENETWORK, "CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork" );

TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork - Operator display tag: %S", &iHomeInfo.iDisplayTag);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_COMPLETEMOBILEPHONEGETHOMENETWORK, "CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork;Operator display tag=%S", iHomeInfo.iDisplayTag );
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork - Operator short name: %S", &iHomeInfo.iShortName);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_COMPLETEMOBILEPHONEGETHOMENETWORK, "CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork;Operator short name=%S", iHomeInfo.iShortName );
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork - Operator long name: %S", &iHomeInfo.iLongName);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_COMPLETEMOBILEPHONEGETHOMENETWORK, "CMmNetMessHandler::CompleteMobilePhoneGetHomeNetwork;Operator long name=%S", iHomeInfo.iLongName );

    // Pack the data
    CMmDataPackage package;
    package.PackData( &iHomeInfo );

    // Complete to DOS side
    iMessageRouter->Complete(
        EMobilePhoneGetHomeNetwork,
        &package,
        KErrNone );

    iMNClength = 0;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::FindOperatorInfoSubBlock
// Finds offset to NET_MODEM_GSM_REG_INFO or NET_MODEM_CURRENT_CELL_INFO
// in NET_MODEM_REG_STATUS_IND, NET_SET_RESP or NET_MODEM_REG_STATUS_GET_RESP
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::FindOperatorInfoSubBlock(
    const TIsiReceiveC& aIsiMessage,
    TUint& aSbOffset
    ) const
    {
    TUint sbStartOffSet( 0 );

    TFLOGSTRING("TSY: CMmNetMessHandler::FindOperatorInfoSubBlock");
    OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_FINDOPERATORINFOSUBBLOCK, "CMmNetMessHandler::FindOperatorInfoSubBlock" );

    TInt ret( CalculateNetworkInformationSbStartOffset(
        aIsiMessage, sbStartOffSet ) );

    if ( KErrNone == ret )
        {
        // look for NET_MODEM_CURRENT_CELL_INFO subblock
        ret = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + sbStartOffSet,
            NET_MODEM_CURRENT_CELL_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );
        if ( KErrNone != ret )
            { // look for NET_MODEM_GSM_REG_INFO subblock
            OstTrace0( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_FINDOPERATORINFOSUBBLOCK, "CMmNetMessHandler::FindOperatorInfoSubBlock - NET_MODEM_CURRENT_CELL_INFO NOT FOUND" );
            TFLOGSTRING("TSY: CMmNetMessHandler::FindOperatorInfoSubBlock - NET_MODEM_CURRENT_CELL_INFO NOT FOUND");
            ret = aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + sbStartOffSet,
                NET_MODEM_GSM_REG_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet );
            }
        if ( KErrNone == ret )
            {
            aSbOffset = sbStartOffSet;
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_FINDOPERATORINFOSUBBLOCK, "CMmNetMessHandler::FindOperatorInfoSubBlock - NOTHING FOUND!" );
            TFLOGSTRING("TSY: CMmNetMessHandler::FindOperatorInfoSubBlock - NOTHING FOUND!");
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::ExtractNetworkRegistrationStatus
// Extracts network registration status from NET_MODEM_REG_INFO_COMMON
// subblock of NET_MODEM_REG_STATUS_IND, NET_SET_RESP or
// NET_MODEM_REG_STATUS_GET_RESP.
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::ExtractNetworkRegistrationStatus(
    const TIsiReceiveC& aIsiMessage,
    const TDesC8& aOperatorCode,
    TUint8& aRegistrationStatus,
    TUint& aSbStartOffset
    ) const
    {
    TFLOGSTRING("TSY: CMmNetMessHandler::ExtractNetworkRegistrationStatus");
    OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_EXTRACTNETWORKREGISTRATIONSTATUS, "CMmNetMessHandler::ExtractNetworkRegistrationStatus" );
    TUint sbStartOffSet( 0 );

    TInt ret( CalculateNetworkInformationSbStartOffset(
        aIsiMessage, sbStartOffSet ) );

    if ( KErrNone == ret )
        {
        ret = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + sbStartOffSet,
            NET_MODEM_REG_INFO_COMMON,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );
        }
    if ( KErrNone == ret )
        {
        aSbStartOffset = sbStartOffSet;
        aRegistrationStatus = aIsiMessage.Get8bit( sbStartOffSet +
             NET_MODEM_REG_INFO_COMMON_OFFSET_REGISTRATIONSTATUS );
        TFLOGSTRING2("TSY: CMmNetMessHandler::ExtractNetworkRegistrationStatus;registrationStatus=%d", aRegistrationStatus);
        OstTraceExt1( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_EXTRACTNETWORKREGISTRATIONSTATUS, "CMmNetMessHandler::ExtractNetworkRegistrationStatus;registrationStatus=%hhu", aRegistrationStatus );
        // patch registration status if this is SIM-card
        if ( 0 < iHplmn.Length() )
            {
            if ( NET_REG_STATUS_ROAM == aRegistrationStatus &&
                 aOperatorCode == KO2NetworkId &&
                 iHplmn == KO2Hplmn
                 )
                {
                TFLOGSTRING("TSY: CMmNetMessHandler::ExtractNetworkRegistrationStatus; NET_REG_STATUS_ROAM --> NET_REG_STATUS_HOME");
                OstTrace0( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_EXTRACTNETWORKREGISTRATIONSTATUS, "CMmNetMessHandler::ExtractNetworkRegistrationStatus; NET_REG_STATUS_ROAM --> NET_REG_STATUS_HOME" );
                aRegistrationStatus = NET_REG_STATUS_HOME;
                }
            }
        else
            {
            TFLOGSTRING("TSY: CMmNetMessHandler::ExtractNetworkRegistrationStatus; cant patch, HPLMN is empty");
            OstTrace0( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_EXTRACTNETWORKREGISTRATIONSTATUS, "CMmNetMessHandler::ExtractNetworkRegistrationStatus - cant patch, HPLMN is empty" );
            }
        }
    else
        {
        TFLOGSTRING("TSY: CMmNetMessHandler::ExtractNetworkRegistrationStatus; NET_MODEM_REG_INFO_COMMON not found");
        OstTrace0( TRACE_NORMAL, DUP5_CMMNETMESSHANDLER_EXTRACTNETWORKREGISTRATIONSTATUS, "CMmNetMessHandler::ExtractNetworkRegistrationStatus; NET_MODEM_REG_INFO_COMMON not found" );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange
// Completes EMobilePhoneNotifyCurrentNetworkChange and
// EMobilePhoneGetNetworkRegistrationStatus IPCs.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange
        (
        const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationAreaData, // LAC info
        const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfoData // Network info
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_COMPLETEMOBILEPHONENOTIFYCURRENTNETWORKCHANGE, "CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange" );

    // Assistant for packaging.
    CMmDataPackage dataPackage;

    // Take possible Cell_DCH or packet transfer state into account
    // when completing current network change.
    iLastNWInfo = aNetworkInfoData;
    iLastLocInfo = aLocationAreaData;

    if ( iCell_DCH ||
        ( iPacketTransfer &&
        RMobilePhone::ENetworkModeGsm == iNetworkMode ) )
        {
        // Pack the data for sending to the manager,
        // location info is invalid (empty).
        RMobilePhone::TMobilePhoneLocationAreaV1 location;
        location.iAreaKnown = EFalse;
        location.iLocationAreaCode = iLastLocInfo.iLocationAreaCode;
        location.iCellId = iLastLocInfo.iCellId;

TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - location.iAreaKnown = %d", location.iAreaKnown);
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - location.iLocationAreaCode = %d", location.iLocationAreaCode);
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - location.iCellId = %d", location.iCellId);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_COMPLETEMOBILEPHONENOTIFYCURRENTNETWORKCHANGE, "CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - location.iAreaKnown=%hhu", location.iAreaKnown );
OstTrace1( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_COMPLETEMOBILEPHONENOTIFYCURRENTNETWORKCHANGE, "CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - location.iLocationAreaCode=%u", location.iLocationAreaCode );
OstTrace1( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_COMPLETEMOBILEPHONENOTIFYCURRENTNETWORKCHANGE, "CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - location.iCellId=%u", location.iCellId );

        dataPackage.PackData( &iLastNWInfo, &location );
        }
    else
        {
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - iLastLocInfo.iAreaKnown = %d", iLastLocInfo.iAreaKnown);
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - iLastLocInfo.iLocationAreaCode = %d", iLastLocInfo.iLocationAreaCode);
TFLOGSTRING2("TSY: CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - iLastLocInfo.iCellId = %d", iLastLocInfo.iCellId);
OstTraceExt1( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_COMPLETEMOBILEPHONENOTIFYCURRENTNETWORKCHANGE, "CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - iLastLocInfo.iAreaKnown=%hhu", iLastLocInfo.iAreaKnown );
OstTrace1( TRACE_NORMAL, DUP5_CMMNETMESSHANDLER_COMPLETEMOBILEPHONENOTIFYCURRENTNETWORKCHANGE, "CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - iLastLocInfo.iLocationAreaCode=%u", iLastLocInfo.iLocationAreaCode );
OstTrace1( TRACE_NORMAL, DUP7_CMMNETMESSHANDLER_COMPLETEMOBILEPHONENOTIFYCURRENTNETWORKCHANGE, "CMmNetMessHandler::CompleteMobilePhoneNotifyCurrentNetworkChange - iLastLocInfo.iCellId=%u", iLastLocInfo.iCellId );

        // Pack the data for sending to the manager.
        dataPackage.PackData( &iLastNWInfo, &iLastLocInfo );
        }

    // Inform that the network info has changed.
    iMessageRouter->Complete(
        EMobilePhoneNotifyCurrentNetworkChange,
        &dataPackage,
        KErrNone );

    iMessageRouter->Complete(
        EMobilePhoneGetNetworkRegistrationStatus,
        KErrNone );

    // Continue handling queued messages.
    ContinueHandlingNetModemRegStatusMessages();
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::QueueNetModemRegStatusMessagesL
// Queue functionality for NET_MODEM_REG_STATUS_IND,
// NET_MODEM_REG_STATUS_GET_RESP and NET_SET_RESP ISI-messages.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::QueueNetModemRegStatusMessagesL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
TFLOGSTRING("TSY: CMmNetMessHandler::QueueNetModemRegStatusMessagesL");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_QUEUENETMODEMREGSTATUSMESSAGESL, "CMmNetMessHandler::QueueNetModemRegStatusMessagesL" );

    // Make a copy of received message. Allocate heap memory.
    // Can leave if out of mem.
    HBufC8* tempHBuf = HBufC8::NewL( aIsiMessage.GetBuffer().Length() );
    // Copy data.
    *tempHBuf = aIsiMessage.GetBuffer();
    // Catch possible error in append.
    TInt errValue( iNetMessageQueue.Append( tempHBuf ) );

    // Check possible error.
    if ( KErrNone == errValue )
        {
        // Start handling queued messages.
        StartHandlingNetModemRegStatusMessages();
        }
    else
        {
        // There was error.
        delete tempHBuf;
        tempHBuf = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::StartHandlingNetModemRegStatusMessages
// Start handling queued messages.
// Can complete next IPCs if needed ECustomGetOperatorNameIPC,
// EMobilePhoneGetNetworkRegistrationStatus and EMobilePhoneSelectNetwork.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::StartHandlingNetModemRegStatusMessages()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::StartHandlingNetModemRegStatusMessages");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_STARTHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::StartHandlingNetModemRegStatusMessages" );

    // Continue handling if there is messages in queue
    // and handling not already ongoing.
    if ( !iNetMessageHandlingOngoing
        && 0 < iNetMessageQueue.Count() )
        {
TFLOGSTRING("TSY: CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - Start handling next queued message");
OstTrace0( TRACE_NORMAL, DUP1_CMMNETMESSHANDLER_STARTHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - Start handling next queued message" );

        // Set flag ON as message handling start.
        iNetMessageHandlingOngoing = ETrue;

        // Copy message from array.
        const TDesC8& message( *iNetMessageQueue[0] );

        // Print message to log.
        TFLOG_PRINT_ISIMESSAGE( message );

        // Save buffer pointer.
        HBufC8* tempHBuf = iNetMessageQueue[0];
        // Remove used message from array and compress.
        iNetMessageQueue.Remove( 0 );
        iNetMessageQueue.Compress();

        TIsiReceiveC isimessage( ( TIsiReceiveC ) message );

        // Get message Id.
        TInt messageId( isimessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

        // Check message id.
        if ( NET_MODEM_REG_STATUS_IND == messageId )
            {
TFLOGSTRING("TSY: CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_MODEM_REG_STATUS_IND");
OstTrace0( TRACE_NORMAL, DUP2_CMMNETMESSHANDLER_STARTHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_MODEM_REG_STATUS_IND" );

            // Start handling NET_MODEM_REG_STATUS_IND message.
            NetModemRegStatusInd( isimessage );
            }
        else if ( NET_MODEM_REG_STATUS_GET_RESP == messageId )
            {
TFLOGSTRING("TSY: CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_MODEM_REG_STATUS_GET_RESP");
OstTrace0( TRACE_NORMAL, DUP3_CMMNETMESSHANDLER_STARTHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_MODEM_REG_STATUS_GET_RESP" );

            // Get the success code.
            TUint8 successCode( isimessage.Get8bit(
                ISI_HEADER_SIZE +
                NET_MODEM_REG_STATUS_GET_RESP_OFFSET_SUCCESSCODE ) );

            if ( NET_CAUSE_OK == successCode )
                {
                // Data to be read from the NET_MODEM_REG_STATUS_GET_RESP
                // message is the same as in the NET_MODEM_REG_STATUS_IND
                // indication.
                NetModemRegStatusInd( isimessage );
                }
            else
                {
                // Map ISA error to symbian value.
                TInt error( CMmStaticUtility::CSCauseToEpocError(
                    PN_MODEM_NETWORK,
                    KTsyNetCauseCommon,
                    successCode ) );

TFLOGSTRING2("TSY: CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_MODEM_REG_STATUS_GET_RESP - Error value: %d", error);
OstTrace1( TRACE_NORMAL, DUP4_CMMNETMESSHANDLER_STARTHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_MODEM_REG_STATUS_GET_RESP - Error value=%d", error );

                // Initialize default return values to ECustomGetOperatorNameIPC
                // complete.
                RMmCustomAPI::TOperatorNameInfo operNameInfo;
                operNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
                operNameInfo.iName.Zero();

                // Complete ECustomGetOperatorNameIPC with error.
                // Packed parameter: a RMmCustomAPI::TOperatorNameInfo.
                CMmDataPackage dataPackage;
                dataPackage.PackData( &operNameInfo );

                iMessageRouter->Complete(
                    ECustomGetOperatorNameIPC,
                    &dataPackage,
                    error );

                // Complete EMobilePhoneGetNetworkRegistrationStatus with error.
                iMessageRouter->Complete(
                    EMobilePhoneGetNetworkRegistrationStatus,
                    error );

                // Continue handling queued messages. As previous message has
                // completed with error.
                ContinueHandlingNetModemRegStatusMessages();
                }
            }
        else if ( NET_SET_RESP == messageId )
            {
TFLOGSTRING("TSY: CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_SET_RESP");
OstTrace0( TRACE_NORMAL, DUP5_CMMNETMESSHANDLER_STARTHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_SET_RESP" );

            // Check if the NW selection was done ok.
            TUint8 successCode( isimessage.Get8bit(
                ISI_HEADER_SIZE + NET_SET_RESP_OFFSET_SUCCESSCODE ) );

            // Map ISA error to Symbian values.
            TInt error( CMmStaticUtility::CSCauseToEpocError(
                PN_MODEM_NETWORK,
                KTsyNetCauseCommon,
                successCode ) );

            if ( KErrNone == error )
                {
                // Check subblocks.
                if ( 0 < isimessage.Get8bit(
                    ISI_HEADER_SIZE + NET_SET_RESP_OFFSET_SUBBLOCKCOUNT ) )
                    {
                    // Data to be read from the NET_SET_RESP message is the
                    // same as in the NET_MODEM_REG_STATUS_IND indication.
                    NetModemRegStatusInd( isimessage );

                    // Inform that the network selection is completed.
                    iMessageRouter->Complete(
                        EMobilePhoneSelectNetwork,
                        NULL,
                        error );
                    }
                else
                    {
                    // If no subblocks, continue handling queued messages.
                    ContinueHandlingNetModemRegStatusMessages();
                    }
                }
            else
                {
TFLOGSTRING2("TSY: CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_SET_RESP - Error cause value: %d", successCode);
OstTraceExt1( TRACE_NORMAL, DUP6_CMMNETMESSHANDLER_STARTHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::StartHandlingNetModemRegStatusMessages - NET_SET_RESP - Error cause value=%hhu", successCode );

                // SuccessCode is not ok, check also for GSM specific error code.

                TUint sbStartOffSet( 0 );

                // Check if interruption request is not already ongoing for the
                // network selection.
                if ( NET_CAUSE_REQUEST_INTERRUPTED != successCode
                    && !iSelectNetworkCancelIssued )
                    {
                    // If the subblock exists.
                    if ( KErrNone == isimessage.FindSubBlockOffsetById(
                        ISI_HEADER_SIZE + SIZE_NET_SET_RESP,
                        NET_MODEM_CAUSE_EXTENSION,
                        EIsiSubBlockTypeId8Len8,
                        sbStartOffSet ) )
                        {
                        // Get the gsm specific error code.
                        successCode = isimessage.Get8bit(
                            sbStartOffSet +
                            NET_MODEM_CAUSE_EXTENSION_OFFSET_REJECTCODE );

                        // Map ISA error to Symbian values.
                        error = CMmStaticUtility::CSCauseToEpocError(
                            PN_MODEM_NETWORK,
                            KTsyNetCauseGsm,
                            successCode );
                        }

                    // Inform that there was an error in
                    // network selection.
                    iMessageRouter->Complete(
                        EMobilePhoneSelectNetwork,
                        error );
                    }
                else if ( NET_CAUSE_REQUEST_INTERRUPTED == successCode )
                    {
                    iMessageRouter->Complete(
                        EMobilePhoneSelectNetwork,
                        NULL,
                        KErrNone );
                    }

                // Continue handling queued messages. As previous message has
                // completed with error.
                ContinueHandlingNetModemRegStatusMessages();
                }
            }

        // Delete used message.
        delete tempHBuf;
        }
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::ContinueHandlingNetModemRegStatusMessages
// Continue handling of queued messages if available.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::ContinueHandlingNetModemRegStatusMessages()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::ContinueHandlingNetModemRegStatusMessages - Continue handling queued messages");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_CONTINUEHANDLINGNETMODEMREGSTATUSMESSAGES, "CMmNetMessHandler::ContinueHandlingNetModemRegStatusMessages - Continue handling queued messages" );

    // Message handling done.
    // Reset flag to OFF as message handling is finished.
    iNetMessageHandlingOngoing = EFalse;

    // Start handling queued messages.
    StartHandlingNetModemRegStatusMessages();
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::InitializeOnsAndOplReading
// Starts reading Operator Name String and Operator PLMN List rules
// when SIM is ready.
// -----------------------------------------------------------------------------
//
void CMmNetMessHandler::InitializeOnsAndOplReading()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::InitializeOnsAndOplReading");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_INITIALIZEONSANDOPLREADING, "CMmNetMessHandler::InitializeOnsAndOplReading" );

    // Set iNetMessageHandlingOngoing flag to ETrue.
    iNetMessageHandlingOngoing = ETrue;

    // Start Operator Name String (ONS Name) reading.
    iNetOperatorNameHandler->UiccOperatorReq();

    // Start Operator PLMN List (OPL) rules reading by reading the record count
    iNetOperatorNameHandler->UiccReadOplRecordCount();
    }

// ---------------------------------------------------------------------------
// CMmNetMessHandler::GetMessageRouter
// Gets pointer to CMmMessageRouter object.
// ---------------------------------------------------------------------------
//
CMmMessageRouter* CMmNetMessHandler::GetMessageRouter()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::GetMessageRouter");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_GETMESSAGEROUTER, "CMmNetMessHandler::GetMessageRouter" );
    return iMessageRouter;
    }

// ---------------------------------------------------------------------------
// CMmNetMessHandler::GetPhoNetSender
// Gets pointer to PhoNetSender object.
// ---------------------------------------------------------------------------
//
CMmPhoNetSender* CMmNetMessHandler::GetPhoNetSender()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::GetPhoNetSender");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_GETPHONETSENDER, "CMmNetMessHandler::GetPhoNetSender" );
    return iPhoNetSender;
    }

// -----------------------------------------------------------------------------
// CMmNetMessHandler::ExtractNetworkRegistrationStatus
// Extracts network registration status from NET_MODEM_REG_STATUS_IND,
// NET_SET_RESP or NET_MODEM_REG_STATUS_GET_RESP
// -----------------------------------------------------------------------------
//
TInt CMmNetMessHandler::ExtractNetworkRegistrationStatus(
    const TIsiReceiveC& aIsiMessage,
    TUint8& aRegistrationStatus
    ) const
    {
    OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_EXTRACTNETWORKREGISTRATIONSTATUS, "CMmNetMessHandler::ExtractNetworkRegistrationStatus" );
    TFLOGSTRING("TSY: CMmNetMessHandler::ExtractNetworkRegistrationStatus");

    TUint sbStartOffSet( 0 );
    // find offset to subblock from which operator code can be read
    TInt returnValue( FindOperatorInfoSubBlock(
        aIsiMessage,
        sbStartOffSet ) );

    if ( KErrNone == returnValue )
        {
        TPtrC8 operatorCode( aIsiMessage.GetData(
            sbStartOffSet + NET_MODEM_CURRENT_CELL_INFO_OFFSET_OPERATORCODE,
            KBCDLength ) );
        TUint8 registrationStatus( 0 );
        returnValue = ExtractNetworkRegistrationStatus(
            aIsiMessage,
            operatorCode,
            registrationStatus,
            sbStartOffSet
            );

        if ( KErrNone == returnValue )
            {
            aRegistrationStatus = registrationStatus;
            }
        }

    return returnValue;
    }

// ---------------------------------------------------------------------------
// CMmNetMessHandler::GetNetOperatorNameHandler
// Gets pointer to NetOperatorNameHandler object.
// ---------------------------------------------------------------------------
//
CMmNetOperatorNameHandler* CMmNetMessHandler::GetNetOperatorNameHandler()
    {
TFLOGSTRING("TSY: CMmNetMessHandler::GetNetOperatorNameHandler");
OstTrace0( TRACE_NORMAL, CMMNETMESSHANDLER_GETNETOPERATORNAMEHANDLER, "CMmNetMessHandler::GetNetOperatorNameHandler" );
    return iNetOperatorNameHandler;
    }

// =============================================================================
// CDelayTimer: Used for delaying for completion of Packet transfer state
// from GPDS_RADIO_AVTIVITY_IND.

// -----------------------------------------------------------------------------
// CDelayTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDelayTimer* CDelayTimer::NewL(
    CMmNetMessHandler* aNetMessHandler )
    {
TFLOGSTRING("TSY: CDelayTimer::NewL");
OstTrace0( TRACE_NORMAL, CDELAYTIMER_NEWL, "CDelayTimer::NewL" );

    CDelayTimer* self = new( ELeave ) CDelayTimer();

    CleanupStack::PushL( self );

    self->iNetMessHandler = aNetMessHandler;
    self->ConstructL();

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDelayTimer::CDelayTimer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDelayTimer::CDelayTimer() : CTimer( CActive::EPriorityStandard )
    {
TFLOGSTRING("TSY: CDelayTimer::CDelayTimer");
OstTrace0( TRACE_NORMAL, CDELAYTIMER_CDELAYTIMER, "CDelayTimer::CDelayTimer" );
    }

// -----------------------------------------------------------------------------
// CDelayTimer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDelayTimer::ConstructL()
    {
TFLOGSTRING("TSY: CDelayTimer::ConstructL");
OstTrace0( TRACE_NORMAL, CDELAYTIMER_CONSTRUCTL, "CDelayTimer::ConstructL" );

    iNewPacketTransferState = ETrue;

    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDelayTimer::~CDelayTimer()
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CDelayTimer::~CDelayTimer()
    {
TFLOGSTRING("TSY: CDelayTimer::~CDelayTimer");
OstTrace0( TRACE_NORMAL, DUP1_CDELAYTIMER_CDELAYTIMER, "CDelayTimer::~CDelayTimer" );
    Cancel();
    }

// -----------------------------------------------------------------------------
// CDelayTimer::StartDelayTimer()
// Start delay timer.
// -----------------------------------------------------------------------------
//
void CDelayTimer::StartDelayTimer(
    TBool aNewPacketTransferState, TInt aTimeOut )
    {
    if ( iNewPacketTransferState != aNewPacketTransferState )
        {
TFLOGSTRING("TSY: CDelayTimer::StartDelayTimer, Restart.");
OstTrace0( TRACE_NORMAL, CDELAYTIMER_STARTDELAYTIMER, "CDelayTimer::StartDelayTimer,Restart" );
        Cancel();
        iNewPacketTransferState = aNewPacketTransferState;
        After( aTimeOut );
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CDelayTimer::RunL()
// Called when operation completes.
// -----------------------------------------------------------------------------
//
void CDelayTimer::RunL()
    {
TFLOGSTRING2("TSY: CDelayTimer::RunL, status = %d.", iStatus.Int());
OstTrace1( TRACE_NORMAL, CDELAYTIMER_RUNL, "CDelayTimer::RunL;iStatus.Int=%d", iStatus.Int() );

    // Timer request has completed, so notify the timer's owner
    if ( iStatus == KErrNone )
        {
        iNetMessHandler->CompleteGpdsRadioActivityInd(
            iNewPacketTransferState );
        }
    // No else

    iNewPacketTransferState = ETrue;
    }

// -----------------------------------------------------------------------------
// CDelayTimer::DoCancel()
// Called when operation is cancelled.
// -----------------------------------------------------------------------------
//
void CDelayTimer::DoCancel()
    {
TFLOGSTRING("TSY: CDelayTimer::DoCancel");
OstTrace0( TRACE_NORMAL, CDELAYTIMER_DOCANCEL, "CDelayTimer::DoCancel" );

    CTimer::DoCancel();

    iNewPacketTransferState = ETrue;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
    //None

//  End of File
