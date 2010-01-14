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
#include "satmessaging.h"       // csatmessaging class
#include "satcc.h"              // sat call control class
#include "sattimer.h"           // sat timer class
#include "sateventdownload.h"   // sat event download class
#include "satdatadownload.h"    // sat data download class
#include "saticon.h"            // sat icon class
#include "satmosmsctrl.h"       // sat mo sms control class
#include "satnotifications.h"   // sat notifications class
#include "ber_tlv.h"            // sat ber-tlv classes
#include "satutil.h"            // sat utility class
#include "satflightmodestatus.h"// sat flight mode status class
#include "satnumberofslots.h"   // multimode tsy slot definitions

#include <satcs.h>              // etel sat ipc request definitions
#include <ctsy/serviceapi/cmmsmsutility.h>      // ton/npi constants, tsendsmsdataandattributes
#include "cmmmessagerouter.h"   // cmessagerouter class of nokiatsy component
#include <ctsy/pluginapi/cmmdatapackage.h>     // parameter packing
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>      // for ipc
#include <tisi.h>               // isi message
#include <infoisi.h>            // isa cellmo info server
#include <uiccisi.h>            // UICC server

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satmessagingTraces.h"
#endif

#if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
#include <cmmsmstsy.h>          // sms functionality
#define __TFLOGGER_H__
#include <cmmphonetsy.h>

#undef __TFLOGGER_H__
#endif //NCP_COMMON_S60_VERSION_SUPPORT

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
const TUint8 KHelp          = 1;
const TUint8 KNoHelp        = 0;

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ============================= LOCAL FUNCTIONS ===============================
    //None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTsySatMessaging::CTsySatMessaging
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
#if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
CTsySatMessaging::CTsySatMessaging
        (
        CMmPhoneTsy*        aMmPhone,
        CMmPhoNetSender*    aPnSend,
        CMmPhoNetReceiver*  aPnReceive,
        TName               aName
        ) : CTsySatMessagingBase(),
        iMmPhone( aMmPhone ),
        iPnSend( aPnSend ),
        iPnReceive( aPnReceive ),
        iSatAppName( aName ),
        iSendSmsTsyReqHandle( NULL )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_CTSYSATMESSAGING, "CTsySatMessaging::CTsySatMessaging" );
    TFLOGSTRING("TSY: CTsySatMessaging::CTsySatMessaging");
    /* None */
    }
#else
CTsySatMessaging::CTsySatMessaging
        (
        CMmPhoNetSender*    aPnSend,
        CMmPhoNetReceiver*  aPnReceive,
        CMmMessageRouter* aRouter
        )

    {
    OstTrace0( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_CTSYSATMESSAGING, "CTsySatMessaging::CTsySatMessaging" );

        iPnSend = aPnSend;
        iPnReceive = aPnReceive;
        iRouter = aRouter;
        iSendSmsTsyReqHandle = NULL;

    TFLOGSTRING("TSY: CTsySatMessaging::CTsySatMessaging");
    /* None */
    }
#endif //NCP_COMMON_S60_VERSION_SUPPORT


// -----------------------------------------------------------------------------
// CTsySatMessaging::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_CONSTRUCTL, "CTsySatMessaging::ConstructL" );
    TFLOGSTRING("TSY: CTsySatMessaging::ConstructL");

    // Create message handler object, to receive ISI messages from ISA CellMo
    iSatMessHandler = CSatMessHandler::NewL( this, iPnSend );

    // Phonet receiver has to know the SatMessHandler.
    iPnReceive->SetSatMessHandler( iSatMessHandler );

    // Initialize Flight Mode Status handler
    iSatFlightModeStatus = CSatFlightModeStatus::NewL( iSatMessHandler, this );

    // Initialize SAT Notifications objects
    iNotifyMoSmControlRequest =
        new( ELeave )CSatNotifyMoSmControlRequest( iSatMessHandler, this );
    iNotifyDisplayText =
        new( ELeave )CSatNotifyDisplayText( iSatMessHandler, this );
    iNotifyGetInkey =
        new( ELeave )CSatNotifyGetInkey( iSatMessHandler, this );
    iNotifyGetInput =
        new( ELeave )CSatNotifyGetInput( iSatMessHandler, this );
    iNotifyPlayTone =
        new( ELeave )CSatNotifyPlayTone( iSatMessHandler, this );
    iNotifySetUpMenu =
        new( ELeave )CSatNotifySetUpMenu( iSatMessHandler, this );
    iNotifySelectItem =
        new( ELeave )CSatNotifySelectItem( iSatMessHandler, this );
    iNotifySendSm =
        new( ELeave )CSatNotifySendSm( iSatMessHandler, this );
    iNotifySendSs =
        new( ELeave )CSatNotifySendSs( iSatMessHandler, this );
    iNotifySendUssd =
        new( ELeave )CSatNotifySendUssd( iSatMessHandler, this );
    iNotifySetUpCall =
        new( ELeave )CSatNotifySetUpCall( iSatMessHandler, this );
    iNotifyRefresh =
        new( ELeave )CSatNotifyRefresh( iSatMessHandler, this );
    iNotifySimSessionEnd =
        new( ELeave )CSatNotifySimSessionEnd( iSatMessHandler, this );
    iNotifySetUpIdleModeText =
        new( ELeave )CSatNotifySetUpIdleModeText( iSatMessHandler, this );
    iNotifyLaunchBrowser =
        new( ELeave )CSatNotifyLaunchBrowser( iSatMessHandler, this );
    iNotifyCallControlRequest =
        new( ELeave )CSatNotifyCallControlRequest( iSatMessHandler, this );
    iNotifyPollInterval =
        new( ELeave )CSatNotifyPollInterval( iSatMessHandler, this );
    iNotifySendDtmf =
        new( ELeave )CSatNotifySendDtmf( iSatMessHandler, this );
    iNotifySetUpEventList =
        new( ELeave )CSatNotifySetUpEventList( iSatMessHandler, this );
    iNotifyPollingOff =
        new( ELeave )CSatNotifyPollingOff( iSatMessHandler, this );
    iNotifyLocalInfo =
        new( ELeave )CSatNotifyLocalInfo( iSatMessHandler, this );
    iNotifyTimerMgmt =
        new( ELeave )CSatNotifyTimerMgmt( iSatMessHandler, this );
    iNotifyMoreTime =
        new( ELeave )CSatNotifyMoreTime( iSatMessHandler, this );
    iNotifyOpenChannel =
        new( ELeave )CSatNotifyOpenChannel( iSatMessHandler, this );
    iNotifyLanguageNotification =
        new( ELeave )CSatNotifyLanguageNotification( iSatMessHandler, this );
    iNotifyGetChannelStatus =
        new( ELeave )CSatNotifyGetChannelStatus( iSatMessHandler, this );
    iNotifyCloseChannel =
        new( ELeave )CSatNotifyCloseChannel( iSatMessHandler, this );
    iNotifyReceiveData =
        new( ELeave )CSatNotifyReceiveData( iSatMessHandler, this );
    iNotifySendData =
        new( ELeave )CSatNotifySendData( iSatMessHandler, this );

    // Transaction id handling
    iTransIdMsg = new( ELeave )TTransIdMessage();

    // Initialize event download handler
    iEventDownload = CSatEventDownload::NewL( iSatMessHandler, this );

    // Initialize data download handler
    iSatDataDownload = CSatDataDownload::NewL( iSatMessHandler, this );

    // Initialize call control handler
    iSatCC = CSatCC::NewL( iSatMessHandler, this );

    // Initialize MO-SMS Control handler
    iSatMoSmsCtrl = CSatMoSmsCtrl::NewL( iSatMessHandler, this );

    // Initialize timer, this is used for implementing timer management
    iSatTimer = CSatTimer::NewL( iSatMessHandler, this );

    // Initialize SAT icon support implementation
    iSatIcon = CSatIcon::NewL( iSatMessHandler, this );

    // Request IMEI code. Needed in provide local info proactive command.
    iSatMessHandler->InfoSerialNumberReadReq(
        GetTransactionId(), INFO_SB_SN_IMEI_PLAIN );

    // Request IMEI-SV code. Needed in provide local info proactive command.
    iSatMessHandler->InfoSerialNumberReadReq(
        GetTransactionId(), INFO_SN_IMEI_SV_TO_NET );

    // Request Network status
    iSatMessHandler->NetCellInfoGetReq(
        GetTransactionId() );

    //Initalize flag, this is used to check if proactiveCommand is ongoing
    iSatTimer->SetProactiveCommandOnGoingStatus( EFalse );

    // Request access technology type
    iSatMessHandler->NetRatReq( GetTransactionId() );

#ifdef INFO_PP_SIM_OLD_POLL_INTERVAL
    // Check old poll interval is supported by ME's Product Profile
    iSatMessHandler->InfoPpReadReq( INFO_PP_SIM_OLD_POLL_INTERVAL );
#endif

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
    // Check INFO_PP_ENHANCED_NETWORK_SELECTION status in product profile
    iSatMessHandler->InfoPpReadReq( INFO_PP_ENHANCED_NETWORK_SELECTION );
#endif
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
#if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
EXPORT_C CTsySatMessaging* CTsySatMessaging::NewL
        (
        CMmPhoneTsy*            aMmPhone,
        CMmMessageRouterBase*   aRouter,
        TName                   aName
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_NEWL, "CTsySatMessaging::NewL" );

    TFLOGSTRING("TSY: CTsySatMessaging::NewL");
    // First check that given arguments are not NULL pointers
    if ( ( NULL == aMmPhone ) || ( NULL == aRouter ) )
        {
        TFLOGSTRING("TSY: CTsySatMessaging::NewL, NULL pointers were given \
            leaving!");
        OstTrace0( TRACE_NORMAL, DUP2_CTSYSATMESSAGING_NEWL, "CTsySatMessaging::NewL, NULL pointers were given leaving!" );

        User::Leave( KErrArgument );
        }
    CMmPhoNetSender* pnSend = NULL;
    CMmPhoNetReceiver* pnReceive = NULL;
    // Get the ISI message router (to handle ISA CellMo messages)
    CMmMessageRouter* licenseeMessageRouter =
        static_cast<CMmMessageRouter*>( aRouter );
    // Get the ISI message sender
    pnSend = licenseeMessageRouter->GetPhoNetSender();
    // Get the ISI message receivde
    pnReceive = licenseeMessageRouter->GetPhoNetReceiver();

    if ( ( NULL == pnSend ) || ( NULL == pnReceive ) )
        {
        // PhoNet is not correctly initialized. No conditions to survive.
        // This may happen if LicenseeTSY stub is used with real SIM_ATK_TSY
        TFLOGSTRING("TSY: CTsySatMessaging::NewL, Construction failed \
            PhoNet is not initialized, leaving!");
        OstTrace0( TRACE_NORMAL, DUP5_CTSYSATMESSAGING_NEWL, "CTsySatMessaging::NewL, Construction failed PhoNet is not initialized, leaving!" );

        User::Leave( KErrArgument );
        }
    // Create a SatMessaging session
    CTsySatMessaging* subsession = new (ELeave) CTsySatMessaging( aMmPhone,
        pnSend, pnReceive, aName );
    CleanupStack::PushL( subsession );
    subsession->ConstructL();
    CleanupStack::Pop( subsession );
    return subsession;
    }
#else
EXPORT_C CTsySatMessaging* CTsySatMessaging::NewL
        (
        CMmMessageRouter*   aRouter
        )
    {
    OstTrace0( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_NEWL, "CTsySatMessaging::NewL" );

    TFLOGSTRING("TSY: CTsySatMessaging::NewL");
    // First check that given arguments are not NULL pointers
    if ( NULL == aRouter )
        {
        TFLOGSTRING("TSY: CTsySatMessaging::NewL, NULL pointers were given \
            leaving!");
        OstTrace0( TRACE_NORMAL, DUP3_CTSYSATMESSAGING_NEWL, "CTsySatMessaging::NewL, NULL pointers were given leaving!" );

        User::Leave( KErrArgument );
        }
    CMmPhoNetSender* pnSend = NULL;
    CMmPhoNetReceiver* pnReceive = NULL;
    // Get the ISI message router (to handle ISA CellMo messages)

    // Get the ISI message sender
    pnSend = aRouter->GetPhoNetSender();
    // Get the ISI message receivde
    pnReceive = aRouter->GetPhoNetReceiver();

    if ( ( NULL == pnSend ) || ( NULL == pnReceive ) )
        {
        // PhoNet is not correctly initialized. No conditions to survive.
        // This may happen if LicenseeTSY stub is used with real SIM_ATK_TSY
        TFLOGSTRING("TSY: CTsySatMessaging::NewL, Construction failed \
            PhoNet is not initialized, leaving!");
        OstTrace0( TRACE_NORMAL, DUP4_CTSYSATMESSAGING_NEWL, "CTsySatMessaging::NewL, Construction failed PhoNet is not initialized, leaving!" );

        User::Leave( KErrArgument );
        }
    // Create a SatMessaging session
    CTsySatMessaging* subsession = new (ELeave) CTsySatMessaging(
        pnSend, pnReceive, aRouter );
    CleanupStack::PushL( subsession );
    subsession->ConstructL();
    CleanupStack::Pop( subsession );
    return subsession;
    }
#endif //NCP_COMMON_S60_VERSION_SUPPORT

// -----------------------------------------------------------------------------
// CTsySatMessaging::~CTsySatMessaging
// Destructor.
// -----------------------------------------------------------------------------
//
CTsySatMessaging::~CTsySatMessaging()
    {
    OstTrace0( TRACE_NORMAL, DUP2_CTSYSATMESSAGING_CTSYSATMESSAGING, "CTsySatMessaging::~CTsySatMessaging" );
    TFLOGSTRING("TSY: CTsySatMessaging::~CTsySatMessaging");

    // Delete message handler object, SAT SMS sending
    delete iSatMessHandler;

    // Set pointer value to Null
    if( iPnReceive )
        {
        iPnReceive->SetSatMessHandler( NULL );
        }

    // Delete internal components
    delete iSatFlightModeStatus;
    delete iTransIdMsg;
    delete iEventDownload;
    delete iSatDataDownload;
    delete iSatCC;
    delete iSatMoSmsCtrl;
    delete iSatTimer;
    delete iSatIcon;

    // Delete Notifications objects
    delete iNotifyMoSmControlRequest;
    delete iNotifyDisplayText;
    delete iNotifyGetInkey;
    delete iNotifyGetInput;
    delete iNotifyPlayTone;
    delete iNotifySetUpMenu;
    delete iNotifySelectItem;
    delete iNotifySendSm;
    delete iNotifySendSs;
    delete iNotifySendUssd;
    delete iNotifySetUpCall;
    delete iNotifyRefresh;
    delete iNotifySimSessionEnd;
    delete iNotifySetUpIdleModeText;
    delete iNotifyLaunchBrowser;
    delete iNotifyCallControlRequest;
    delete iNotifyPollInterval;
    delete iNotifySendDtmf;
    delete iNotifySetUpEventList;
    delete iNotifyPollingOff;
    delete iNotifyLocalInfo;
    delete iNotifyTimerMgmt;
    delete iNotifyMoreTime;
    delete iNotifyOpenChannel;
    delete iNotifyLanguageNotification;
    delete iNotifyGetChannelStatus;
    delete iNotifyCloseChannel;
    delete iNotifyReceiveData;
    delete iNotifySendData;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::OpenNewObjectByNameL
// Opens a new object identified by a name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTelObject* CTsySatMessaging::OpenNewObjectByNameL
        (
        const TDesC& /* aName */    // Name of the object
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_OPENNEWOBJECTBYNAMEL, "CTsySatMessaging::OpenNewObjectByNameL" );
    TFLOGSTRING("TSY: CTsySatMessaging::OpenNewObjectByNameL");
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::OpenNewObjectL
// Opens a new object identified by new name. Can leave. Not in use.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTelObject* CTsySatMessaging::OpenNewObjectL
        (
        TDes& /*aNewName*/  // Name of the new object
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_OPENNEWOBJECTL, "CTsySatMessaging::OpenNewObjectL" );
    TFLOGSTRING("TSY: CTsySatMessaging::OpenNewObjectL");
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::ExtFunc
// Command handler for ETel server messages. The IPC identifies
// the command to be executed if supported.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::ExtFunc
        (
        const TTsyReqHandle aTsyReqHandle,  // Request handle
        const TInt          aIpc,           // IPC number of request
        const TDataPackage& aPackage        // Contains parameters for request
        )
    {
    OstTraceExt2( TRACE_NORMAL, CTSYSATMESSAGING_EXTFUNC, "CTsySatMessaging::ExtFunc ReqHandle: %d IPC: %d", TInt(aTsyReqHandle), aIpc );

    TFLOGSTRING3("TSY: CTsySatMessaging::ExtFunc ReqHandle=%d IPC=%d",
        aTsyReqHandle, aIpc);
    // some SAT functions need trapping, use two level function where
    // first one is just a trap harness
    // It is responsibility of IPC handling functions to always handle
    // calling ReqComplete, ( it must either call it, or arrange it to
    // be called in some way), except in the case of Leave. It is also
    // assumed that after IPC handling function has called ReqComplete
    // it doesn't Leave.
    TInt trapError( KErrNone );
    TRAP( trapError, DoExtFuncL( aTsyReqHandle, aIpc, aPackage ) );
    if ( trapError != KErrNone )
        {
        ReqCompleted( aTsyReqHandle, trapError );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::DoExtFuncL
// Handles extended ETel SAT client requests, received from ExtFunc
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::DoExtFuncL
        (
        const TTsyReqHandle aTsyReqHandle,  // Request handle
        const TInt          aIpc,           // IPC number of request
        const TDataPackage& aPackage        // Contains parameters for request
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_DOEXTFUNCL, "CTsySatMessaging::DoExtFuncL" );
    TFLOGSTRING("TSY: CTsySatMessaging::DoExtFuncL");
    TAny* dataPtr=aPackage.Ptr1();
    TAny* dataPtr2=aPackage.Ptr2();
    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
            {
            iNotifyDisplayText->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySelectItemPCmd:
            {
            iNotifySelectItem->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyGetInkeyPCmd:
            {
            iNotifyGetInkey->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyGetInputPCmd:
            {
            iNotifyGetInput->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyPlayTonePCmd:
            {
            iNotifyPlayTone->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySetUpMenuPCmd:
            {
            iNotifySetUpMenu->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySendSmPCmd:
            {
            iNotifySendSm->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySendSsPCmd:
            {
            iNotifySendSs->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySendUssdPCmd:
            {
            iNotifySendUssd->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySetUpCallPCmd:
            {
            iNotifySetUpCall->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyRefreshPCmd:
            {
            iNotifyRefresh->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyRefreshRequiredParam:
            {
            iNotifyRefresh->NotifyRefreshRequired( aTsyReqHandle,
                aPackage.Des1n() );
            break;
            }
        case ESatRefreshAllowed:
            {
            ReqCompleted( aTsyReqHandle,
                iNotifyRefresh->RefreshAllowed( aPackage.Des1n() ) );
            break;
            }
        case ESatNotifyProactiveSimSessionEnd:
            {
            iNotifySimSessionEnd->Notify( aTsyReqHandle, NULL );
            break;
            }
        case ESatNotifySetUpIdleModeTextPCmd:
            {
            iNotifySetUpIdleModeText->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyLaunchBrowserPCmd:
            {
            iNotifyLaunchBrowser->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyCallControlRequest:
            {
            iNotifyCallControlRequest->Notify( aTsyReqHandle,
                aPackage.Des1n() );
            break;
            }
        case ESatNotifySendDtmfPCmd:
            {
            iNotifySendDtmf->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySetUpEventListPCmd:
            {
            iNotifySetUpEventList->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyOpenChannelPCmd:
            {
            iNotifyOpenChannel->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyCloseChannelPCmd:
            {
            iNotifyCloseChannel->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyReceiveDataPCmd:
            {
            iNotifyReceiveData->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifySendDataPCmd:
            {
            iNotifySendData->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatEventDownload:
            {
            iEventDownload->ExtFunc( aTsyReqHandle, aPackage );
            break;
            }
        // All IPC functions could be done the same way as Notifies are done:
        // a class instead of a bare function.
        case ESatTerminalRsp:
            {
            TerminalResponse( REINTERPRET_CAST( RSat::TPCmd*, dataPtr ),
                aPackage.Des2n(),
                aTsyReqHandle );
            break;
            }
        case ESatMenuSelection:
            {
            MenuSelection( aPackage.Des1n(), aTsyReqHandle );
            break;
            }
        case ESatNotifyCbDownload:
            {
            NotifyCbDownload( aPackage.Des1n(), aTsyReqHandle );
            break;
            }
        case ESatNotifySmsPpDownload:
            {
            NotifySmsPpDownload( aPackage.Des1n(), aTsyReqHandle );
            break;
            }
        case ESatSendMessageNoLogging:
            {
            SendMessageNoLoggingL( aPackage.Des1n(),
                REINTERPRET_CAST( TUint16*, dataPtr2 ),
                aTsyReqHandle );
            break;
            }
        case ESatGetIcon:
            {
            iSatIcon->GetIcon( REINTERPRET_CAST( TUint8*, dataPtr ),
                REINTERPRET_CAST( RSat::TIcon*, dataPtr2 ),
                aTsyReqHandle );
            break;
            }
        case ESatGetImageInstance:
            {
            iSatIcon->GetImageInstance( aPackage.Des1n(),
                aPackage.Des2n(), aTsyReqHandle );
            break;
            }
        case ESatGetClut:
            {
            iSatIcon->GetClut(
                REINTERPRET_CAST( RSat::TInstanceNumberAndOffset*, dataPtr ),
                aPackage.Des2n(),
                aTsyReqHandle);
            break;
            }
        case ESatNotifyLanguageNotificationPCmd:
            {
            iNotifyLanguageNotification->Notify( aTsyReqHandle,
                aPackage.Des1n() );
            break;
            }
        case ESatNotifyLocalInfoPCmd:
            {
            iNotifyLocalInfo->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyGetChannelStatusPCmd:
            {
            iNotifyGetChannelStatus->Notify( aTsyReqHandle, aPackage.Des1n() );
            break;
            }
        case ESatNotifyMoSmControlRequest:
            {
            iNotifyMoSmControlRequest->Notify( aTsyReqHandle,
                aPackage.Des1n() );
            break;
            }
        case ESatUsatClientReadyIndication:
            {
            UsatClientReadyIndication( aTsyReqHandle );
            break;
            }
        // Set ret to KErrNotSupported, function not supported
        default:
            {
            TFLOGSTRING2("TSY: CTsySatMessaging::DoExtFuncL unsupported IPC %d", aIpc);
            OstTrace1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_DOEXTFUNCL, "CTsySatMessaging::DoExtFuncL unsupported IPC %d", aIpc );

            ReqCompleted( aTsyReqHandle, KErrNotSupported );
            break;
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::CancelService
// Cancels the ongoing service
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::CancelService
        (
        const TInt          aIpc,           // IPC number of request
        const TTsyReqHandle aTsyReqHandle   // Request handle
        )
    {
    OstTraceExt2( TRACE_NORMAL, CTSYSATMESSAGING_CANCELSERVICE, "CTsySatMessaging::CancelService ReqHandle=%d, IPC=%d", TInt(aTsyReqHandle), aIpc );
    TFLOGSTRING3("TSY: CTsySatMessaging::CancelService ReqHandle=%d IPC=%d",aTsyReqHandle, aIpc);
    TInt ret( KErrNone );
    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
            {
            ret = iNotifyDisplayText->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyGetInkeyPCmd:
            {
            ret = iNotifyGetInkey->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyGetInputPCmd:
            {
            ret = iNotifyGetInput->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyPlayTonePCmd:
            {
            ret = iNotifyPlayTone->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySetUpMenuPCmd:
            {
            ret =  iNotifySetUpMenu->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySelectItemPCmd:
            {
            ret = iNotifySelectItem->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySendSmPCmd:
            {
            ret = iNotifySendSm->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySendSsPCmd:
            {
            ret = iNotifySendSs->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySendUssdPCmd:
            {
            ret = iNotifySendUssd->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySetUpCallPCmd:
            {
            ret = iNotifySetUpCall->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyRefreshPCmd:
            {
            ret = iNotifyRefresh->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyRefreshRequiredParam:
            {
            ret = iNotifyRefresh->CancelRefreshRequiredNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifyProactiveSimSessionEnd:
            {
            ret = iNotifySimSessionEnd->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySetUpIdleModeTextPCmd:
            {
            ret = iNotifySetUpIdleModeText->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyLaunchBrowserPCmd:
            {
            ret = iNotifyLaunchBrowser->CancelNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifyCallControlRequest:
            {
            ret = iNotifyCallControlRequest->CancelNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifySendDtmfPCmd:
            {
            ret = iNotifySendDtmf->CancelNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifySetUpEventListPCmd:
            {
            ret = iNotifySetUpEventList->CancelNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifyLanguageNotificationPCmd:
            {
            ret = iNotifyLanguageNotification->CancelNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifyLocalInfoPCmd:
            {
            ret = iNotifyLocalInfo->CancelNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifyOpenChannelPCmd:
            {
            iNotifyOpenChannel->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyMoSmControlRequest:
            {
            ret = iNotifyMoSmControlRequest->CancelNotification(
                aTsyReqHandle );
            break;
            }
        case ESatNotifyCloseChannelPCmd:
            {
            iNotifyCloseChannel->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyReceiveDataPCmd:
            {
            iNotifyReceiveData->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySendDataPCmd:
            {
            iNotifySendData->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyGetChannelStatusPCmd:
            {
            iNotifyGetChannelStatus->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatEventDownload:
            {
            ret = iEventDownload->Cancel( aTsyReqHandle );
            break;
            }
        case ESatTerminalRsp:
        case ESatMenuSelection:
        case ESatSendMessageNoLogging:
        case ESatUsatClientReadyIndication:
            {
            ret = KErrNone;
            break;
            }
        // Following functions are not supported, hence they are completed
        // by DoExtFuncL function at once.
        // ETel server will not pass cancel function to MMSAT, because the
        // request is not 'ON' in ETel.
        // They are listed here for future reference.
        //
        // ESatGetMeSideSatProfile
        default:
            {
            TFLOGSTRING2("TSY: CTsySatMessaging::CancelService invalid IPC %d",aIpc);
            OstTrace1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_CANCELSERVICE, "CTsySatMessaging::CancelService invalid IPC %d", aIpc );

            ret = KErrGeneral;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::ReqModeL
// ReqMode method for checking what modes this component supports
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTelObject::TReqMode CTsySatMessaging::ReqModeL
        (
        const TInt aIpc     // IPC number of request
        )
    {
    OstTrace1( TRACE_NORMAL, CTSYSATMESSAGING_REQMODEL, "CTsySatMessaging::ReqModeL, IPC=%d", aIpc );
    TFLOGSTRING2("TSY: CTsySatMessaging::ReqModeL, IPC = %d",aIpc);
    CTelObject::TReqMode ret = 0;
    switch ( aIpc )
        {
        // Multiple Completion Services
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:
        case ESatNotifySetUpCallPCmd:
        case ESatNotifyRefreshPCmd:
        case ESatNotifyCallControlRequest:
        case ESatNotifySendDtmfPCmd:
        case ESatNotifyMoSmControlRequest:
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyPollingIntervalPCmd:
        case ESatNotifyPollingOffPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifyReceiveDataPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:
            {
            ret = KReqModeMultipleCompletionEnabled;
            break;
            }
        // Non-flow Controlled Services
        case ESatTerminalRsp:
        case ESatMenuSelection:
        case ESatEventDownload:
        case ESatGetIcon:
        case ESatGetImageInstance:
        case ESatGetClut:
        case ESatGetMeSideSatProfile:
        case ESatClientSatProfileIndication:
        case ESatSendMessageNoLogging:
        case ESatUsatClientReadyIndication:
            {
            break;
            }
        default:
            {
            TFLOGSTRING2( "TSY: CTsySatMessaging::ReqModeL unsupported IPC %d", aIpc );
            OstTrace1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_REQMODEL, "CTsySatMessaging::ReqModeL unsupported IPC %d", aIpc );

            User::Leave( KErrNotSupported );
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::Init
// Initializer method that is called by ETel Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::Init()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_INIT, "CTsySatMessaging::Init" );
    TFLOGSTRING("TSY: CTsySatMessaging::Init");
    // none
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::RegisterNotification
// Register given command, checks if the given command is supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::RegisterNotification
        (
        const TInt aIpc     // IPC number of request
        )
    {
    OstTrace1( TRACE_NORMAL, CTSYSATMESSAGING_REGISTERNOTIFICATION, "CTsySatMessaging::RegisterNotification, IPC = %d", aIpc );
    TFLOGSTRING2("TSY: CTsySatMessaging::RegisterNotification, IPC = %d",aIpc);
    // Initialize return value
    TInt ret( KErrNone );
    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:
        case ESatNotifySetUpCallPCmd:
        case ESatNotifyRefreshPCmd:
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyCallControlRequest:
        case ESatNotifySendDtmfPCmd:
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifyReceiveDataPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyMoSmControlRequest:
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:
            // ret already set to KErrNone
            break;
        // Following notifications are not supported,
        // hence listed here for future reference.
        //
        default:
            {
            // Unknown or invalid IPC
            TFLOGSTRING2("TSY: CTsySatMessaging::RegisterNotification, unsupported IPC %d", aIpc);
            OstTrace1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_REGISTERNOTIFICATION, "CTsySatMessaging::RegisterNotification unsupported IPC %d", aIpc );

            ret = KErrNotSupported;
            break;
            }
        }
    return ret;

    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::DeregisterNotification
// DeRegister given command, checks if the given command is supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::DeregisterNotification
        (
        const TInt aIpc     // IPC number of request
        )
    {
    OstTrace1( TRACE_NORMAL, CTSYSATMESSAGING_DEREGISTERNOTIFICATION, "CTsySatMessaging::DeregisterNotification, IPC = %d", aIpc );
    TFLOGSTRING2( "TSY: CTsySatMessaging::DeregisterNotification\
        IPC = %d", aIpc );
    // Initialize return value
    TInt ret( KErrNone );
    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:
        case ESatNotifySetUpCallPCmd:
        case ESatNotifyRefreshPCmd:
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyCallControlRequest:
        case ESatNotifySendDtmfPCmd:
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifyReceiveDataPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyMoSmControlRequest:
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:
            // ret already set to KErrNone
            break;
        // Following notifications are not supported,
        // hence listed here for future reference.
        //
        default:
            {
            // Unknown or invalid IPC
            TFLOGSTRING2("TSY: CTsySatMessaging::DeregisterNotification \
            unsupported IPC %d", aIpc);
            OstTrace1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_DEREGISTERNOTIFICATION, "CTsySatMessaging::DeregisterNotification unsupported IPC %d", aIpc );

            ret = KErrNotSupported;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NumberOfSlotsL
// Returns number of slots to be used for given IPC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::NumberOfSlotsL
        (
        const TInt aIpc        // IPC number of request
        )
    {
    OstTrace1( TRACE_NORMAL, CTSYSATMESSAGING_NUMBEROFSLOTSL, "CTsySatMessaging::NumberOfSlotsL IPC %d", aIpc );
    TFLOGSTRING2("TSY: CTsySatMessaging::NumberOfSlotsL IPC %d", aIpc);
    TInt numberOfSlots( 1 );
    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
            numberOfSlots = KSatNotifyDisplayTextPCmdSlots;
            break;
        case ESatNotifyGetInkeyPCmd:
            numberOfSlots = KSatNotifyGetInkeyPCmdSlots;
            break;
        case ESatNotifyGetInputPCmd:
            numberOfSlots = KSatNotifyGetInputPCmdSlots;
            break;
        case ESatNotifyPlayTonePCmd:
            numberOfSlots = KSatNotifyPlayTonePCmdSlots;
            break;
        case ESatNotifySetUpMenuPCmd:
            numberOfSlots = KSatNotifySetUpMenuPCmdSlots;
            break;
        case ESatNotifySelectItemPCmd:
            numberOfSlots = KSatNotifySelectItemPCmdSlots;
            break;
        case ESatNotifySendSmPCmd:
            numberOfSlots = KSatNotifySendSmPCmdSlots;
            break;
        case ESatNotifySendSsPCmd:
            numberOfSlots = KSatNotifySendSsPCmdSlots;
            break;
        case ESatNotifySendUssdPCmd:
            numberOfSlots = KSatNotifySendUssdPCmdSlots;
            break;
        case ESatNotifySetUpCallPCmd:
            numberOfSlots = KSatNotifySetUpCallPCmdSlots;
            break;
        case ESatNotifyRefreshPCmd:
            numberOfSlots = KSatNotifyRefreshPCmdSlots;
            break;
        case ESatNotifyProactiveSimSessionEnd:
            numberOfSlots = KSatNotifyProactiveSimSessionEndSlots;
            break;
        case ESatNotifySetUpIdleModeTextPCmd:
            numberOfSlots = KSatNotifySetUpIdleModeTextPCmdSlots;
            break;
        case ESatNotifyLaunchBrowserPCmd:
            numberOfSlots = KSatNotifyLaunchBrowserSlots;
            break;
        case ESatNotifyCallControlRequest:
            numberOfSlots = KSatNotifyCallControlRequestSlots;
            break;
        case ESatNotifySendDtmfPCmd:
            numberOfSlots = KSatNotifySendDtmfSlots;
            break;
        case ESatNotifySetUpEventListPCmd:
            numberOfSlots = KSatNotifySetUpEventListSlots;
            break;
        case ESatNotifyLanguageNotificationPCmd:
            numberOfSlots = KSatNotifySetUpEventListSlots;
            break;
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifyReceiveDataPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyMoSmControlRequest:
            numberOfSlots = KSatNotifySetUpEventListSlots;
            break;
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:
            // default value: 1 slot
            break;
        // Following notifications are not supported,
        // hence listed here for future reference.
        //
        // For above, use numberOfSlots = 2
        default:
            {
            // Unknown or invalid IPC
            TFLOGSTRING2( "TSY: CTsySatMessaging::NumberOfSlotsL\
                unsupported IPC %d", aIpc );
            OstTrace1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_NUMBEROFSLOTSL, "CTsySatMessaging::NumberOfSlotsL unsupported IPC %d", aIpc );

            User::Leave( KErrNotSupported );
            break;
            }
        }
    return numberOfSlots;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetSatMessHandler
// Returns pointer to the SatMessHandler instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatMessHandler* CTsySatMessaging::GetSatMessHandler()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETSATMESSHANDLER, "CTsySatMessaging::GetSatMessHandler" );
    TFLOGSTRING("TSY:CTsySatMessaging::GetSatMessHandler");
    // Pointer to SAT message handler
    return iSatMessHandler;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetSatFlightModeStatus
// Returns pointer to the CSatFlightModeStatus instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatFlightModeStatus* CTsySatMessaging::GetSatFlightModeStatus()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETSATFLIGHTMODESTATUS, "CTsySatMessaging::GetSatFlightModeStatus" );
    TFLOGSTRING("TSY:CTsySatMessaging::GetSatFlightModeStatus");
    // Pointer to SAT flight mode status
    return iSatFlightModeStatus;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::CompleteSendSmsMessage
// Completes a SAT Originated SMS sending request to Etel SAT client
// with the corresponding outcome.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::CompleteSendSmsMessage
        (
        TInt aStatus  // Message sending status from MMSMS
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_COMPLETESENDSMSMESSAGE, "CTsySatMessaging::CompleteSendSmsMessage" );
    TFLOGSTRING("TSY: CTsySatMessaging::CompleteSendSmsMessage, entered");
    if ( iSendSmsTsyReqHandle )
        {
        ReqCompleted( iSendSmsTsyReqHandle, aStatus );
        iSendSmsTsyReqHandle = NULL;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::MenuSelection
// Menu selection command handler from ETel SAT client application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::MenuSelection
        (
        TDes8*          aSelection,     // Contains menu selection parameters
        TTsyReqHandle   aTsyReqHandle   // Request handle
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_MENUSELECTION, "CTsySatMessaging::MenuSelection" );
    TFLOGSTRING("TSY:CTsySatMessaging::MenuSelection");
    TInt ret( KErrNone );
    RSat::TMenuSelectionV1Pckg* aPckg =
            REINTERPRET_CAST( RSat::TMenuSelectionV1Pckg*, aSelection );
    RSat::TMenuSelectionV1& selectionV1 = ( *aPckg ) ();
    TFLOGSTRING3("TSY:CTsySatMessaging::MenuSelection, iItemId %x, Help: %c",
        selectionV1.iItemId, selectionV1.iHelp );
    OstTraceExt1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_MENUSELECTION, "CTsySatMessaging::MenuSelection iItemId: %hhu", selectionV1.iItemId );
    OstTrace1( TRACE_NORMAL, DUP2_CTSYSATMESSAGING_MENUSELECTION, "CTsySatMessaging::MenuSelection Help: %{THelpRequest}", selectionV1.iHelp );

    // Creating the menu selection message
    if ( selectionV1.iHelp == RSat::EHelpRequested )
        {
        // Help requested
        iSatMessHandler->MenuSelectionInd(
            GetTransactionId(),
            selectionV1.iItemId, KHelp ); // KHelp: 1
        }
    else
        {
        // No help requested
        iSatMessHandler->MenuSelectionInd(
            GetTransactionId(),
            selectionV1.iItemId, KNoHelp ); // KNoHelp: 0
        }
    ReqCompleted( aTsyReqHandle, ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NotifyCbDownload
// Cell broadcast data download from ETel SAT client application. Not in use.
// CellBroadcast DDL is handled in CSatDataDownload class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::NotifyCbDownload
        (
        TDes8*          /*aPackage*/,     //package containing pointer to client
        TTsyReqHandle   aTsyReqHandle     //tsy request handle
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_NOTIFYCBDOWNLOAD, "CTsySatMessaging::NotifyCbDownload" );
    TFLOGSTRING("TSY:CTsySatMessaging::NotifyCbDownload");
    TInt ret( KErrNone );
    ReqCompleted( aTsyReqHandle, ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NotifySmsPpDownload
// Sms-Pp data download from ETel SAT client application. Not in use.
// Sms-Pp DDL is handled in CSatDataDownload class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::NotifySmsPpDownload
        (
        TDes8*          /*aPackage*/,   //package containing pointer to client
        TTsyReqHandle   aTsyReqHandle   //tsy request handle
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_NOTIFYSMSPPDOWNLOAD, "CTsySatMessaging::NotifySmsPpDownload" );
    TFLOGSTRING("TSY:CTsySatMessaging::NotifySmsPpDownload");
    TInt ret( KErrNone );
    ReqCompleted( aTsyReqHandle, ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::SendMessageNoLoggingL
// Send SMS No Logging command handler from client application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::SendMessageNoLoggingL
        (
        TDes8*          aMsg,           // Pointer to message
        TUint16*        /*aMsgRef*/,    // Message reference
        TTsyReqHandle   aTsyReqHandle   // Request handle
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_SENDMESSAGENOLOGGINGL, "CTsySatMessaging::SendMessageNoLoggingL" );
    TFLOGSTRING("TSY: CTsySatMessaging::SendMessageNoLoggingL");
    TInt ret( KErrNone );
    // Save Reqhandle for CompleteSendSmsMessage
    iSendSmsTsyReqHandle = aTsyReqHandle;
    RSat::TSatSmsV1Pckg* aPckg = REINTERPRET_CAST( RSat::TSatSmsV1Pckg*, aMsg );
    RSat::TSatSmsV1& satSmsV1 = ( *aPckg ) ();
    // SMS GSM extension checks that SCA length is not over the max limit
    // that SMS allows
    // Check that we have a SCA address
    if ( 0x0 < satSmsV1.iServiceCenter.iTelNumber.Length() )
        {
        // SCA number conversion from unicode to GSM 3.40 format
        // is done by MMSMS dll
        // Set SCA pointer
        TDes16* sca;
        sca = &satSmsV1.iServiceCenter.iTelNumber;
        // Type of Number, mapping from ETelSat format to MMETel format
        RMobilePhone::TMobileTON numberType;
        switch ( satSmsV1.iServiceCenter.iTypeOfNumber )
            {
            case RSat::ETypeOfNumberNotSet:
            case RSat::EUnknownNumber:
                numberType = RMobilePhone::EUnknownNumber;
                break;
            case RSat::EInternationalNumber:
                numberType = RMobilePhone::EInternationalNumber;
                break;
            case RSat::ENationalNumber:
                numberType = RMobilePhone::ENationalNumber;
                break;
            case RSat::ENetworkSpecificNumber:
                numberType = RMobilePhone::ENetworkSpecificNumber;
                break;
            case RSat::EDedicatedNumber:
                numberType = RMobilePhone::ESubscriberNumber;
                break;
            default:
                // Unknown or invalid Type of Number
                numberType = RMobilePhone::EUnknownNumber;
                break;
            }
        // Numbering Plan, mapping from ETelSat format to MMETel format
        RMobilePhone::TMobileNPI numberPlan;
        switch ( satSmsV1.iServiceCenter.iNumberPlan )
            {
            case RSat::ENumberingPlanNotSet:
            case RSat::EUnknownNumberingPlan:
                numberPlan = RMobilePhone::EUnknownNumberingPlan;
                break;
            case RSat::EIsdnNumberPlan:
                numberPlan = RMobilePhone::EIsdnNumberPlan;
                break;
            case RSat::EDataNumberPlan:
                numberPlan = RMobilePhone::EDataNumberPlan;
                break;
            case RSat::ETelexNumberPlan:
                numberPlan = RMobilePhone::ETelexNumberPlan;
                break;
            case RSat::ENationalNumberPlan:
                numberPlan = RMobilePhone::ENationalNumberPlan;
                break;
            case RSat::EPrivateNumberPlan:
                numberPlan = RMobilePhone::EPrivateNumberPlan;
                break;
            default:
                // Unknown or invalid Numbering Plan
                numberPlan = RMobilePhone::EUnknownNumberingPlan;
                break;
            }
        // SMS message, Packet Data Unit
        TDes8* pdu;
        pdu = &satSmsV1.iBuf;

        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        // Get SMS messaging class pointer for sending sms messages
        CMmSmsTsy* mmSmsTsy = iMmPhone->SmsSession();
        TFLOGSTRING2("TSY: CTsySatMessaging::SendMessageNoLoggingL, \
            mmSmsTsy: %x", mmSmsTsy );
        OstTrace1( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_SENDMESSAGENOLOGGINGL, "CTsySatMessaging::SendMessageNoLoggingL, mmSmsTsy: %x", mmSmsTsy );

        if ( mmSmsTsy ) // Check that SMS session exist
            {
            // Call SMS sending function from MMSMS
            ret = mmSmsTsy->SendSatMessage( this, pdu, sca, &numberType,
                &numberPlan, EFalse, aTsyReqHandle );
            TFLOGSTRING2("TSY: CTsySatMessaging::SendMessageNoLoggingL, \
                SendSmsMessage done, ret: %x", ret );
            OstTrace1( TRACE_NORMAL, DUP2_CTSYSATMESSAGING_SENDMESSAGENOLOGGINGL, "CTsySatMessaging::SendMessageNoLoggingL, SendSmsMessage done, ret: %d", ret );
            }
        else
            {
            // No SMS session.
            ret = KErrNotReady;
            TFLOGSTRING("TSY: CTsySatMessaging::SendMessageNoLoggingL, \
            no SMS session!");
            OstTrace0( TRACE_NORMAL, DUP3_CTSYSATMESSAGING_SENDMESSAGENOLOGGINGL, "CTsySatMessaging::SendMessageNoLoggingL, no SMS session!" );
            }
        #else //NCP_COMMON_S60_VERSION_SUPPORT
        // Deliver attributes to message handler by using
        // TMobileSmsSendAttributesV1 class.
        RMobileSmsMessaging::TMobileSmsSendAttributesV1 msgAttrib;

        msgAttrib.iFlags = ( RMobileSmsMessaging::KSmsDataFormat |
        RMobileSmsMessaging::KGsmServiceCentre |
        RMobileSmsMessaging::KMoreToSend );

        msgAttrib.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;

        // if service center address length  > 20 digits plus "+" sign
        if ( sca->Length() > ( KMaxAmountOfDigits + 1 ) )
            {
            ret = KErrArgument;
            }
        else
            {
            msgAttrib.iGsmServiceCentre.iTelNumber.Copy( *sca );
            msgAttrib.iGsmServiceCentre.iTypeOfNumber = numberType;
            msgAttrib.iGsmServiceCentre.iNumberPlan = numberPlan;

            msgAttrib.iMore = EFalse;
            // create package
            CMmDataPackage package;

            // structure for all sms parameters and data
            TSendSmsDataAndAttributes sendData;

            sendData.iAttributes = &msgAttrib;
            sendData.iMsgData = pdu;
            sendData.iIpc = EMmTsySmsSendSatMessage;

            // Pack parameters
            package.PackData( &sendData );

            // send request to NTSY
            ret = iRouter->ExtFuncL( EMmTsySmsSendSatMessage, &package );
            }
        #endif //NCP_COMMON_S60_VERSION_SUPPORT
        }
    else // SCA missing
        {
        ret = KErrGeneral;
        TFLOGSTRING("TSY: CTsySatMessaging::SendMessageNoLoggingL, \
        SCA missing");
        OstTrace0( TRACE_NORMAL, DUP4_CTSYSATMESSAGING_SENDMESSAGENOLOGGINGL, "CTsySatMessaging::SendMessageNoLoggingL, SCA missing" );

        }
    // Failure in sending of SAT SMS, call complete method
    if ( KErrNone != ret )
        {
        CompleteSendSmsMessage( ret );
        TFLOGSTRING("TSY: CTsySatMessaging::SendMessageNoLoggingL, \
        failure sending SAT SMS, complete");
        OstTrace0( TRACE_NORMAL, DUP5_CTSYSATMESSAGING_SENDMESSAGENOLOGGINGL, "CTsySatMessaging::SendMessageNoLoggingL, failure sending SAT SMS, complete" );

        }
    return ret;

    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::TerminalResponse
// Terminal response command handler from ETel SAT client application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::TerminalResponse
        (
        RSat::TPCmd*    aPCmd,          // SAT command identifier
        TDes8*          aRsp,           // Response
        TTsyReqHandle   aTsyReqHandle   // Request handle
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_TERMINALRESPONSE, "CTsySatMessaging::TerminalResponse" );
    TFLOGSTRING("TSY: CTsySatMessaging::TerminalResponse");
    TInt ret( KErrNone );
    switch ( *aPCmd )
        {
        case RSat::EDisplayText:
            ret = iNotifyDisplayText->TerminalResponse( aRsp );
            break;
        case RSat::EGetInkey:
            ret = iNotifyGetInkey->TerminalResponse( aRsp );
            break;
        case RSat::EGetInput:
            ret = iNotifyGetInput->TerminalResponse( aRsp );
            break;
        case RSat::EPlayTone:
            ret = iNotifyPlayTone->TerminalResponse( aRsp );
            break;
        case RSat::ESetUpMenu:
            ret = iNotifySetUpMenu->TerminalResponse( aRsp );
            break;
        case RSat::ESelectItem:
            ret = iNotifySelectItem->TerminalResponse( aRsp );
            break;
        case RSat::ESendSm:
            ret = iNotifySendSm->TerminalResponse( aRsp );
            break;
        case RSat::ESendSs:
            ret = iNotifySendSs->TerminalResponse( aRsp );
            break;
        case RSat::ESendUssd:
            ret = iNotifySendUssd->TerminalResponse( aRsp );
            break;
        case RSat::ESetUpCall:
            ret = iNotifySetUpCall->TerminalResponse( aRsp );
            break;
        case RSat::ERefresh:
            ret = iNotifyRefresh->TerminalResponse( aRsp );
            break;
        case RSat::ESetUpIdleModeText:
            ret = iNotifySetUpIdleModeText->TerminalResponse(aRsp );
            break;
        case RSat::ELaunchBrowser:
            ret = iNotifyLaunchBrowser->TerminalResponse( aRsp );
            break;
        case RSat::ESendDtmf:
            ret = iNotifySendDtmf->TerminalResponse( aRsp );
            break;
        case RSat::ESetUpEventList:
            ret = iNotifySetUpEventList->TerminalResponse( aRsp );
            break;
        case RSat::ELocalInfo:
            ret = iNotifyLocalInfo->TerminalResponse( aRsp );
            break;
        case RSat::ELanguageNotification:
            ret = iNotifyLanguageNotification->TerminalResponse( aRsp );
            break;
        case RSat::EOpenChannelCs:
        case RSat::EOpenChannelGprs:
        case RSat::EOpenChannelLocal:
            ret = this->iNotifyOpenChannel->TerminalResponse( aRsp );
            break;
        case RSat::EGetChannelStatus:
            ret = iNotifyGetChannelStatus->TerminalResponse( aRsp );
            break;
        case RSat::ECloseChannel:
             ret = iNotifyCloseChannel->TerminalResponse( aRsp );
            break;
        case RSat::EReceiveData:
            ret = iNotifyReceiveData->TerminalResponse( aRsp );
            break;
        case RSat::ESendData:
            ret = iNotifySendData->TerminalResponse( aRsp );
            break;
        default:
            TFLOGSTRING("TSY: CTsySatMessaging::TerminalResponse, \
            PCmd not supported");
            OstTrace0( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_TERMINALRESPONSE, "CTsySatMessaging::TerminalResponse, PCmd not supported" );

            ret =  KErrGeneral;
            break;
        }
    ReqCompleted( aTsyReqHandle, ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::PCmdReceivedL
// Handles an incoming proactive command from ISA CellMo SIM server
// Called by CSatMessHandler when it receives proactive command ISI msg
// originated by ISA CellMo SIM server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::PCmdReceivedL( const TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_PCMDRECEIVEDL, "CTsySatMessaging::PCmdReceivedL" );
    TFLOGSTRING("TSY: CTsySatMessaging::PCmdReceivedL");
    CBerTlv berTlv;
    berTlv.BerTlv( aIsiMessage );
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    //set flag on to indicate that proactive command is ongoing
    iSatTimer->SetProactiveCommandOnGoingStatus( ETrue );
    // Routes to a suitable MessageReceived method, in order to
    // process the PCmd ISI message.
    switch( commandDetails.GetShortInfo( ETLV_TypeOfCommand ) )
        {
        case KDisplayText:
            iNotifyDisplayText->MessageReceived( aIsiMessage );
            break;
        case KGetInkey:
            iNotifyGetInkey->MessageReceived( aIsiMessage );
            break;
        case KGetInput:
            iNotifyGetInput->MessageReceived( aIsiMessage );
            break;
        case KPlayTone:
            iNotifyPlayTone->MessageReceived( aIsiMessage );
            break;
        case KSetUpMenu:
            iNotifySetUpMenu->MessageReceived( aIsiMessage );
            break;
        case KSelectItem:
            iNotifySelectItem->MessageReceived( aIsiMessage );
            break;
        case KSendShortMessage:
            iNotifySendSm->MessageReceived( aIsiMessage );
            break;
        case KSendSs:
            iNotifySendSs->MessageReceived( aIsiMessage );
            break;
        case KSendUssd:
            iNotifySendUssd->MessageReceived( aIsiMessage );
            break;
        case KSetUpCall:
            iNotifySetUpCall->MessageReceived( aIsiMessage );
            break;
        case KRefresh:
            iNotifyRefresh->MessageReceived( aIsiMessage );
            break;
        case KSetUpIdleModeText:
            iNotifySetUpIdleModeText->MessageReceived( aIsiMessage );
            break;
        case KPollInterval:
            iNotifyPollInterval->MessageReceived( aIsiMessage );
            break;
        case KSendDtmf:
            iNotifySendDtmf->MessageReceived( aIsiMessage );
            break;
        case KSetUpEventList:
            iNotifySetUpEventList->MessageReceived( aIsiMessage );
            break;
        case KPollingOff:
            iNotifyPollingOff->MessageReceived( aIsiMessage );
            break;
        case KProvideLocalInformation:
            iNotifyLocalInfo->MessageReceived( aIsiMessage );
            break;
        case KTimerManagement:
            iNotifyTimerMgmt->MessageReceived( aIsiMessage );
            break;
        case KMoreTime:
            iNotifyMoreTime->MessageReceived( aIsiMessage );
            break;
        case KLaunchBrowser:
            iNotifyLaunchBrowser->MessageReceived( aIsiMessage );
            break;
        case KLanguageNotification:
            iNotifyLanguageNotification->MessageReceived( aIsiMessage );
            break;
        case KGetChannelStatus:
            iNotifyGetChannelStatus->MessageReceived( aIsiMessage );
            break;
        case KOpenChannel:
            iNotifyOpenChannel->MessageReceived( aIsiMessage );
            break;
        case KCloseChannel:
            iNotifyCloseChannel->MessageReceived( aIsiMessage );
            break;
        case KReceiveData:
            iNotifyReceiveData->MessageReceived( aIsiMessage );
            break;
        case KSendData:
            iNotifySendData->MessageReceived( aIsiMessage );
            break;
        default:
            {
            TFLOGSTRING("TSY: CTsySatMessaging::PCmdReceivedL, PCmd not supported");
            OstTrace0( TRACE_NORMAL, DUP1_CTSYSATMESSAGING_PCMDRECEIVEDL, "CTsySatMessaging::PCmdReceivedL, PCmd not supported" );

            // If proactive command is not supported, terminal response
            // with "Command is beyond ME's capabilities" information
            // will be send. ProactiveCommandOngoing status will be set
            // to EFalse in SendTerminalResponseL().
            TBuf8<5> cmdDetails;
            cmdDetails.Copy( commandDetails.Data() );
            TTlv tlvSpecificData;
            // Create General Result TLV here
            tlvSpecificData.AddTag( KTlvResultTag );
            // General result
            tlvSpecificData.AddByte( KCommandBeyondMeCapabilities );
            iSatMessHandler->UiccCatReqTerminalResponse(
                cmdDetails,
                tlvSpecificData.GetDataWithoutTopLevelTag() ,
                aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::TimerExpiration
// Timer Expiration handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::TimerExpiration
        (
        TInt aTimerId,
        TUint32 aTimerValue
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_TIMEREXPIRATION, "CTsySatMessaging::TimerExpiration" );
    TFLOGSTRING("TSY: CTsySatMessaging::TimerExpiration");
    TUint8 time[3] = {0, 0, 0};
    TInt num[3];
    num[0] = aTimerValue/3600;         //hours
    num[1] = ( aTimerValue%3600 )/60;  //minutes
    num[2] = ( aTimerValue%3600 )%60;  //seconds
    for ( TInt i = 0; i < 3; i++)
        {
        //convert to semi-octet
        time[i] = TSatUtility::ConvertToSemiOctet( num[i] );
        }
    return iSatMessHandler->TimerExpirationInd(
            GetTransactionId(),
            (TUint8)aTimerId,
            time );

    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetSatTimer
// Returns pointer to the SatTimer instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatTimer* CTsySatMessaging::GetSatTimer()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETSATTIMER, "CTsySatMessaging::GetSatTimer" );
    // Pointer to SAT timer
    return iSatTimer;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetNotifyRefresh
// Returns pointer to the CSatNotifyRefresh instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatNotifyRefresh* CTsySatMessaging::GetNotifyRefresh()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETNOTIFYREFRESH, "CTsySatMessaging::GetNotifyRefresh" );
    return iNotifyRefresh;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetDataDownload
// Returns pointer to the CSatDataDownload instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatDataDownload* CTsySatMessaging::GetDataDownload()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETDATADOWNLOAD, "CTsySatMessaging::GetDataDownload" );
    return iSatDataDownload;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetNotifyPollInterval
// Returns pointer to the NotifyPollInterval instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatNotifyPollInterval* CTsySatMessaging::GetNotifyPollInterval()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETNOTIFYPOLLINTERVAL, "CTsySatMessaging::GetNotifyPollInterval" );
    return iNotifyPollInterval;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetMoSmsCtrl
// Returns pointer to the CSatMoSmsCtrl instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatMoSmsCtrl* CTsySatMessaging::GetMoSmsCtrl()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETMOSMSCTRL, "CTsySatMessaging::GetMoSmsCtrl" );
    return iSatMoSmsCtrl;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetEventDownload
// Returns pointer to the CSatEventDownload instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatEventDownload* CTsySatMessaging::GetEventDownload()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETEVENTDOWNLOAD, "CTsySatMessaging::GetEventDownload" );
    return iEventDownload;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetNotifyPollingOff
// Returns pointer to the NotifyPollingOff instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatNotifyPollingOff* CTsySatMessaging::GetNotifyPollingOff()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETNOTIFYPOLLINGOFF, "CTsySatMessaging::GetNotifyPollingOff" );
    return iNotifyPollingOff;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetNotifyLocalInfo
// Returns pointer to the NotifyLocalInfo instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatNotifyLocalInfo* CTsySatMessaging::GetNotifyLocalInfo()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETNOTIFYLOCALINFO, "CTsySatMessaging::GetNotifyLocalInfo" );
    return iNotifyLocalInfo;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::EventDownloadReceived
// Processes ISI messages that are relevant for event download
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::EventDownloadReceived
        (
        const TIsiReceiveC& aIsiMessage //Event download message
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_EVENTDOWNLOADRECEIVED, "CTsySatMessaging::EventDownloadReceived" );
    iEventDownload->MessageReceived( aIsiMessage );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::SetUpEventList
// Set up requested events that have to be reported to the SIM
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::SetUpEventList
        (
        TUint32 aEvents //events to be monitored
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_SETUPEVENTLIST, "CTsySatMessaging::SetUpEventList" );
    iEventDownload->SetUpEventList( aEvents );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::SessionEnd
// Notifies client about end of session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::SessionEnd
        (
        const TIsiReceiveC& aIsiMessage //message ending session
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_SESSIONEND, "CTsySatMessaging::SessionEnd" );
    iNotifySimSessionEnd->MessageReceived( aIsiMessage );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NotifyClientAboutCallControlEvent
// Notifies Etel SAT client about call control event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::NotifyClientAboutCallControlEvent
        (
        TDesC&                  aAlphaId, //Alpha id of call control event
        RSat::TControlResult    aResult
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_NOTIFYCLIENTABOUTCALLCONTROLEVENT, "CTsySatMessaging::NotifyClientAboutCallControlEvent" );
    iNotifyCallControlRequest->CompleteNotification( aAlphaId, aResult );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NotifyClientAboutCallControlEventL
// Notifies Etel SAT client about call control event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::NotifyClientAboutCallControlEventL
        (
        const TUint8 aCcResult,
        TPtrC8 aEnvelopeResponse
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_NOTIFYCLIENTABOUTCALLCONTROLEVENTL, "CTsySatMessaging::NotifyClientAboutCallControlEventL" );
    iNotifyCallControlRequest->CompleteNotificationL( aCcResult, aEnvelopeResponse );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NotifyClientAboutGprsCallControlEvent
// Notifies Etel SAT client about GPRS call control event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::NotifyClientAboutGprsCallControlEvent
        (
        const TDesC&                aAlphaId,
        const RSat::TControlResult  aResult
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_NOTIFYCLIENTABOUTGPRSCALLCONTROLEVENT, "CTsySatMessaging::NotifyClientAboutGprsCallControlEvent" );
    iNotifyCallControlRequest->CompleteNotification(
        aAlphaId,
        aResult );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::NotifyClientAboutMoSmControlEvent
// Notifies client about Mo-Sms control event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::NotifyClientAboutMoSmControlEvent
        (
        TDesC&                  aAlphaId, //Alpha id of MO-SMS control event
        RSat::TControlResult    aResult
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_NOTIFYCLIENTABOUTMOSMCONTROLEVENT, "CTsySatMessaging::NotifyClientAboutMoSmControlEvent" );
    iNotifyMoSmControlRequest->CompleteNotification( aAlphaId, aResult );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetTransactionId
// Get new transaction id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CTsySatMessaging::GetTransactionId()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETTRANSACTIONID, "CTsySatMessaging::GetTransactionId" );
    return iTransIdMsg->GetTransactionId( );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::SatReady
// Sat ready indication
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::SatReady
        (
        TUint8 aMsg // Message to be retrieved
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_SATREADY, "CTsySatMessaging::SatReady" );
    return iPnReceive->SatReady( aMsg );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::ClearCCArrays
// Clears the array containing call control event data, due to
// a refresh PCmd.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::ClearCCArrays()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_CLEARCCARRAYS, "CTsySatMessaging::ClearCCArrays" );
    iSatCC->ClearArraysForRefresh();
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::CallControlReceivedL
// Processes ISI messages that are relevant for call control
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::CallControlReceivedL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_CALLCONTROLRECEIVEDL, "CTsySatMessaging::CallControlReceivedL" );
    iSatCC->MessageReceivedL( aIsiMessage );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::DataDownloadReceivedL
// Processes ISI messages that are relevant for call control
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::DataDownloadReceivedL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_DATADOWNLOADRECEIVEDL, "CTsySatMessaging::DataDownloadReceivedL" );
    iSatDataDownload->MessageReceivedL( aIsiMessage );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::MoSmsControlReceived
// Processes ISI messages that are relevant for MoSm control
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::MoSmsControlReceived
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_MOSMSCONTROLRECEIVED, "CTsySatMessaging::MoSmsControlReceived" );
    if ( iSatMoSmsCtrl->IsActivated() )
        {
        iSatMoSmsCtrl->MessageReceived( aIsiMessage );
        }
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::IsMoSmControlBySimActivated
// This methods returns ETrue if MoSmControl
// is activated. Otherwise, it returns EFalse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CTsySatMessaging::IsMoSmControlBySimActivated()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_ISMOSMCONTROLBYSIMACTIVATED, "CTsySatMessaging::IsMoSmControlBySimActivated" );
    return iSatMoSmsCtrl->IsActivated();
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::GetSatIcon
// Returns pointer to the SatIcon instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSatIcon* CTsySatMessaging::GetSatIcon()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETSATICON, "CTsySatMessaging::GetSatIcon" );
    // Pointer to SAT timer
    return iSatIcon;
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::StoreCallConnectedEvent
// Cache call connected envelope
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::StoreCallConnectedEvent
        (
        const TDesC8& aEnvelope
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_STORECALLCONNECTEDEVENT, "CTsySatMessaging::StoreCallConnectedEvent" );
    iNotifySetUpCall->StoreCallConnectedEvent( aEnvelope );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::SetSetUpCallStatus
// Updates the status of SetUpCall command in SatEventDownload instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::SetSetUpCallStatus
        (
        const TBool aStatus
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_SETSETUPCALLSTATUS, "CTsySatMessaging::SetSetUpCallStatus" );
    iEventDownload->SetSetUpCallStatus( aStatus );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::SetStatusOfUssdSupport
// Set CSatCC internal flag according to EF-SimServiceTable.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::SetStatusOfUssdSupport
        (
        const TBool aStatus
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_SETSTATUSOFUSSDSUPPORT, "CTsySatMessaging::SetStatusOfUssdSupport" );
    iSatCC->SetStatusOfUssdSupport( aStatus );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::SetTonNpi
// Set CSatCC internal Ton and Npi
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::SetTonNpi
        (
        const TUint8 aTonNpi
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_SETTONNPI, "CTsySatMessaging::SetTonNpi" );
    iSatCC->SetTonNpi( aTonNpi );
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::StoreSms
// Request the storing of SMS from CommonTsy
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTsySatMessaging::StoreSmsL
        (
        RMobileSmsStore::TMobileGsmSmsEntryV1& aSmsEntry
        )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_STORESMSL, "CTsySatMessaging::StoreSmsL" );
    #if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsPckg( aSmsEntry );

    // Pack data and forward request to DOS
    TDesC8* data = reinterpret_cast<TDesC8*>( &smsPckg );

    CMmDataPackage package;
    package.PackData( &data, &aSmsEntry.iIndex );

    TRAPD( ret, ret = iMmPhone->MessageManager()->HandleRequestL(
        EMobilePhoneStoreWrite, &package ) );

    // Return leave code or return status of method.
    return ret;
    #else
    RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsPckg( aSmsEntry );
    TInt ret = KErrNone;

    // Pack data and forward request to DOS
    TDesC8* data = reinterpret_cast<TDesC8*>( &smsPckg );

    CMmDataPackage package;
    package.PackData( &data, &aSmsEntry.iIndex );

    ret = iRouter->ExtFuncL( EMobilePhoneStoreWrite, &package );

    return ret;
    #endif //NCP_COMMON_S60_VERSION_SUPPORT
    }

// -----------------------------------------------------------------------------
// CTsySatMessaging::UsatClientReadyIndication
// Inform SIMSON that (U)SAT client has sent all notifications and is ready to
// receive proactive commands or that flight mode is on and (U)SAT client cannot
// receive proactive commands.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTsySatMessaging::UsatClientReadyIndication( TTsyReqHandle aTsyReqHandle )
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_USATCLIENTREADYINDICATION, "CTsySatMessaging::UsatClientReadyIndication" );
    TFLOGSTRING("TSY: CTsySatMessaging::UsatClientReadyIndication");

    // Set SAT ready flag in satmesshandler
    iSatMessHandler->SetSatReadyStatus( ETrue );

    // If UICC server is ready, send command to enable or disable proactive
    // commands
    if ( iSatMessHandler->GetTerminalProfileStatus() )
        {
        if ( ! iSatFlightModeStatus->FlightModeStatusValue() )
            {
            iSatMessHandler->UiccCatReq( UICC_CAT_ENABLE );
            }
        else
            {
            iSatMessHandler->UiccCatReq( UICC_CAT_DISABLE );
            }
        }

    // Complete ReqHandle immediately for preventing blocking of Etel server
    // Client do not care about return value
    ReqCompleted( aTsyReqHandle, KErrNone );
    }

#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
// -----------------------------------------------------------------------------
// CTsySatMessaging::GetMessageRouter
// Returns pointer to message router
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
CMmMessageRouter* CTsySatMessaging::GetMessageRouter()
    {
    OstTrace0( TRACE_NORMAL, CTSYSATMESSAGING_GETMESSAGEROUTER, "CTsySatMessaging::GetMessageRouter" );
    return iRouter;
    }
#endif

// ================= OTHER EXPORTED FUNCTIONS ==================================

//  End of File
