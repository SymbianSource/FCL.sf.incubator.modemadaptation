/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmdataporthandler.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagerouter.h"
#include "cmmstaticutility.h"
#include "tsylogger.h"

#include <commdb.h>
#include <etelmmerr.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdataporthandlerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// ============================= LOCAL FUNCTIONS ==============================

    //None

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CMmDataPortHandler::CMmDataPortHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMmDataPortHandler::CMmDataPortHandler()
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::CMmDataPortHandler");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_CMMDATAPORTHANDLER_TD, "CMmDataPortHandler::CMmDataPortHandler" );

    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMmDataPortHandler::ConstructL()
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::ConstructL");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_CONSTRUCTL_TD, "CMmDataPortHandler::ConstructL" );

    iDP0LoanedToClient = EFalse;
    iDP1LoanedToClient = EFalse;
    iDP4LoanedToClient = EFalse;
    iDP5LoanedToClient = EFalse;

    // Reset internal attributes
    ResetInternalAttributes();

    InitDataParamsL();
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMmDataPortHandler* CMmDataPortHandler::NewL(
    CMmMessageRouter* aMessageRouter )
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::NewL");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_NEWL_TD, "CMmDataPortHandler::NewL" );

    CMmDataPortHandler* dataHandler = new ( ELeave ) CMmDataPortHandler();

    CleanupStack::PushL( dataHandler );
    dataHandler->iMessageRouter = aMessageRouter;

    dataHandler->ConstructL();
    CleanupStack::Pop( dataHandler );

    return dataHandler;
    }

// -----------------------------------------------------------------------------
// CMmDataPortHandler::~CMmDataPortHandler
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmDataPortHandler::~CMmDataPortHandler()
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::~CMmDataPortHandler");
OstTrace0( TRACE_NORMAL,  DUP1_CMMDATAPORTHANDLER_CMMDATAPORTHANDLER_TD, "CMmDataPortHandler::~CMmDataPortHandler" );

    // Close Comms server connections
    if ( ECommsDataportOpened <= iCommsStatus )
        {
        iDataPort.Close();
        }
    //no else

    if ( ECommsModuleLoaded <= iCommsStatus )
        {
        iCommServer.UnloadCommModule( iCommPort.iCsy );
        }
    //no else

    if ( ECommsServerConnected <= iCommsStatus )
        {
        iCommServer.Close();
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::ResetInternalAttributes
// Initialises owner, bearer service and data call info attributes.
// ----------------------------------------------------------------------------
//
void CMmDataPortHandler::ResetInternalAttributes()
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::ResetInternalAttributes");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_RESETINTERNALATTRIBUTES_TD, "CMmDataPortHandler::ResetInternalAttributes" );

    iBearerService.iBearerCaps = ( RCall::KBearerCapsCompressionNone |
        RCall::KBearerCapsProtocolUnknown );
    iBearerService.iBearerSpeed = RCall::EBearerDataUnknown;

    // Call info
    iCallType = ECallTypeUnknown;
    iCallDirection = RMobileCall::EDirectionUnknown;
    iIsMultimedia = EFalse;
    iIsWaitingCall = EFalse;
    iMobileCallInfo.iRemoteParty.iDirection = iCallDirection;
    iMobileCallInfo.iService = RMobilePhone::EServiceUnspecified;
    iMobileCallInfo.iStatus = RMobileCall::EStatusUnknown;
    iMobileCallInfo.iCallId = -1;
    iMobileCallInfo.iEmergency = EFalse;
    iMobileCallInfo.iForwarded = EFalse;
    iMobileCallInfo.iRemoteParty.iRemoteIdStatus =
        RMobileCall::ERemoteIdentityUnknown;
    iMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
        RMobilePhone::EUnknownNumber;
    iMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
        RMobilePhone::EUnknownNumberingPlan;
    iMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Zero();
    iMobileCallInfo.iRemoteParty.iCallingName.Zero();
    iMobileCallInfo.iDialledParty.iTypeOfNumber =
        RMobilePhone::EUnknownNumber;
    iMobileCallInfo.iDialledParty.iNumberPlan =
        RMobilePhone::EUnknownNumberingPlan;
    iMobileCallInfo.iDialledParty.iTelNumber.Zero();
    iMobileCallInfo.iAlternatingCall =
        RMobilePhone::EAlternatingModeUnspecified;
    iMobileCallInfo.iValid = 0;

    iCSDParams.iService = RMobileCall::EServiceDataCircuitAsync;
    iCSDParams.iSpeed = RMobileCall::ESpeedUnspecified;
    iCSDParams.iProtocol = RMobileCall::EProtocolUnspecified;
    iCSDParams.iQoS = RMobileCall::EQoSNonTransparent;

    iCurrentHSCSDInfo.iAiur = RMobileCall::EAiurBpsUnspecified;
    iCurrentHSCSDInfo.iRxTimeSlots = 0;
    iCurrentHSCSDInfo.iTxTimeSlots = 0;
    iCurrentHSCSDInfo.iCodings = RMobileCall::ETchCodingUnspecified;

    // If false, notifies the client about incoming call
    iAlreadyNotified = EFalse;

    iIPC = EEtelCallDial;
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::InitDataParamsL
// Initialises data parameters
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMmDataPortHandler::InitDataParamsL()
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::InitDataParamsL");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_INITDATAPARAMSL_TD, "CMmDataPortHandler::InitDataParamsL" );

    // Open dataport for AT commands.
    if ( ECommsNotConnected == iCommsStatus )
        {
        iCommPort.iCsy  = KDataPortName;
        iCommPort.iPort = KDataPortPort0;
        TRAPD( trapError, OpenDataPortL() );
        // If trap caught error, there is nothing else to do but call
        // HandleDataPortError.
        if ( KErrNone != trapError )
            {

            TFLOGSTRING2("TSY: CMmDataPortHandler::OpenDataPortL - Left with error: %d", trapError);
OstTrace1( TRACE_NORMAL,  DUP1_CMMDATAPORTHANDLER_INITDATAPARAMSL_TD, "CMmDataPortHandler::InitDataParamsL;Left with error=%d", trapError );
            }
        //no else

        User::LeaveIfError( trapError );
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::OpenDataPortL
// Opens dataport for TSY. Loads device for dataport, opens dataport,
// sets dataport configurations and sends AT commands for initialisation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMmDataPortHandler::OpenDataPortL()
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::OpenDataPortL");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_OPENDATAPORTL_TD, "CMmDataPortHandler::OpenDataPortL" );

    TInt ret( KErrNone );

#ifdef __WINS__
    ret = User::LoadPhysicalDevice( KPddName );
    if ( KErrNone != ret && KErrAlreadyExists != ret )
        {
        User::Leave( ret );
        }
    //no else

    ret = User::LoadLogicalDevice( KLddName );
    if ( KErrNone != ret && KErrAlreadyExists != ret )
        {
        User::Leave( ret );
        }
    //no else
#endif // __WINS__

    ret = StartC32();
    if ( KErrNone != ret && KErrAlreadyExists != ret )
        {
        User::Leave( ret );
        }
    //no else

    // Connect to Comms server
    User::LeaveIfError( iCommServer.Connect() );
    iCommsStatus = ECommsServerConnected;

    TFLOGSTRING2("TSY: CMmDataPortHandler::OpenDataPortL - iCommPort.iCsy: %S", &iCommPort.iCsy );
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMDATAPORTHANDLER_OPENDATAPORTL_TD, "CMmDataPortHandler::OpenDataPortL;iCommPort.iCsy=%S", iCommPort.iCsy );

    // Load dataport module
    User::LeaveIfError( iCommServer.LoadCommModule( iCommPort.iCsy ) );
    iCommsStatus = ECommsModuleLoaded;

    TFLOGSTRING2("TSY: CMmDataPortHandler::OpenDataPortL - iCommPort.iPort: %S", &iCommPort.iPort);
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMDATAPORTHANDLER_OPENDATAPORTL_TD, "CMmDataPortHandler::OpenDataPortL;iCommPort.iPort=%S", iCommPort.iPort );

    // Open dataport
    User::LeaveIfError( iDataPort.Open( iCommServer, iCommPort.iPort,
        ECommShared, ECommRoleDTE ) );
    iCommsStatus = ECommsDataportOpened;

    RPhone::TStatus modemStatus;
    modemStatus.iModemDetected = RPhone::EDetectedPresent;
    modemStatus.iMode          = RPhone::EModeIdle;

    // create data package
    CMmDataPackage dataPackage;
    // pack modem status
    dataPackage.PackData( &modemStatus );
    // complete modem detected notification
    iMessageRouter->Complete(
        EEtelPhoneNotifyModemDetected,
        &dataPackage,
        KErrNone );

    // Set dataport configurations
    iDataPort.Config( iDataPortSettings );
    // The line feed character is the single terminator
    iDataPortSettings().iTerminator[0]   = KLinefeed()[0];
    iDataPortSettings().iTerminatorCount = KLinefeed().Length();
    // Write the config into DP
    User::LeaveIfError( iDataPort.SetConfig( iDataPortSettings ) );

    // Set the receive buffer to the same amount as rate demands -> 5760kb
    iDataPort.SetReceiveBufferLength( KDPReceiveBufLen );
    // Reset both TX and RX buffers
    iDataPort.ResetBuffers();
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::LoanDataPort
// Loans dataport to the client.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMmDataPortHandler::LoanDataPort(
    const CCallDataPackage* aCallDataPackage )
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::LoanDataPort");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_LOANDATAPORT_TD, "CMmDataPortHandler::LoanDataPort" );

    TInt ret( KErrNone );
    TInt callId( KCallIdNone );
    RMobilePhone::TMobileService callMode( RMobilePhone::EServiceUnspecified );
    RCall::TCommPort* commPort = NULL;

    aCallDataPackage->GetCallIdAndMode( callId, callMode );
    aCallDataPackage->UnPackData( &commPort );

    if ( commPort && ECommsDataportOpened <= iCommsStatus )
        {

        TFLOGSTRING2("TSY: CMmDataPortHandler::LoanDataPort - Client taking control: %S", &commPort->iPort);
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMDATAPORTHANDLER_LOANDATAPORT_TD, "CMmDataPortHandler::LoanDataPort;commPort->iPort=%S", commPort->iPort );
        // Check port and mode
        if ( ( commPort->iPort == KDataPortPort1 ||
            commPort->iPort == KDataPortPort4 ||
            commPort->iPort == KDataPortPort5 ||
            commPort->iPort == KDataPortPortDUN ) &&
            callMode == RMobilePhone::ECircuitDataService )
            {
            if ( commPort->iPort == KDataPortPortDUN )
                {
                // Dynamic port allocation for DUN
                ret = GetNextFreeDialUpPort( commPort );
                }
            //no else
            if ( KErrNone == ret)
                {
                ret = LoanDialUpPort( commPort );
                }
            //no else
            }
        else
            {
            if ( commPort->iPort != KDataPortPort0 )
                {
                commPort->iPort = KDataPortPort0;
                }
            //no else

            if ( !iDP0LoanedToClient )
                {
                commPort->iCsy.Copy( KDataPortName );
                iDP0LoanedToClient = ETrue;
                TFLOGSTRING2("TSY: CMmDataPortHandler::LoanDataPort - %S Loaned", &commPort->iPort );
OstTraceExt1( TRACE_NORMAL,  DUP3_CMMDATAPORTHANDLER_LOANDATAPORT_TD, "CMmDataPortHandler::LoanDataPort;commPort->iPort=%S", commPort->iPort );

                }
            else
                {
                ret = KErrEtelPortAlreadyLoaned;
                }
            }
        }
    else
        {
        ret = KErrNotReady;
        }

    TFLOGSTRING2("TSY: CMmDataPortHandler::LoanDataPort - Result: %d", ret );
OstTrace1( TRACE_NORMAL,  DUP1_CMMDATAPORTHANDLER_LOANDATAPORT_TD, "CMmDataPortHandler::LoanDataPort;ret=%d", ret );

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::GetNextFreeDialUpPort
// Get next free dial-up port.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMmDataPortHandler::GetNextFreeDialUpPort(
    RCall::TCommPort* aCommPort ) const
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::GetNextFreeDialUpPort");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_GETNEXTFREEDIALUPPORT_TD, "CMmDataPortHandler::GetNextFreeDialUpPort" );

    TInt result( KErrNone );

    if ( !iDP1LoanedToClient )
        {
        aCommPort->iPort.Copy( KDataPortPort1 );

        TFLOGSTRING2("TSY: CMmDataPortHandler::GetNextFreeDialUpPort - %S Free", &aCommPort->iPort );

OstTraceExt1( TRACE_NORMAL,  DUP3_CMMDATAPORTHANDLER_GETNEXTFREEDIALUPPORT_TD, "CMmDataPortHandler::GetNextFreeDialUpPort;aCommPort->iPort=%S Free", aCommPort->iPort );
        }
    else if ( !iDP4LoanedToClient )
        {
        aCommPort->iPort.Copy( KDataPortPort4 );

        TFLOGSTRING2("TSY: CMmDataPortHandler::GetNextFreeDialUpPort - %S Free", &aCommPort->iPort );

OstTraceExt1( TRACE_NORMAL,  DUP1_CMMDATAPORTHANDLER_GETNEXTFREEDIALUPPORT_TD, "CMmDataPortHandler::GetNextFreeDialUpPort;aCommPort->iPort=%S Free", aCommPort->iPort );
        }
    else if ( !iDP5LoanedToClient )
        {
        aCommPort->iPort.Copy( KDataPortPort5 );

        TFLOGSTRING2("TSY: CMmDataPortHandler::GetNextFreeDialUpPort - %S Free", &aCommPort->iPort );

OstTraceExt1( TRACE_NORMAL,  DUP2_CMMDATAPORTHANDLER_GETNEXTFREEDIALUPPORT_TD, "CMmDataPortHandler::GetNextFreeDialUpPort;aCommPort->iPort=%S - Free", aCommPort->iPort );
        }
    else
        {
        result = KErrEtelPortAlreadyLoaned;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::LoanDialUpPort
// Loans dial-up port to the client.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMmDataPortHandler::LoanDialUpPort(
    RCall::TCommPort* aCommPort )
    {

    TFLOGSTRING("TSY: CMmDataPortHandler::LoanDialUpPort");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_LOANDIALUPPORT_TD, "CMmDataPortHandler::LoanDialUpPort" );

    TInt result( KErrNone );

    if ( aCommPort->iPort == KDataPortPort1 )
        {
        if ( !iDP1LoanedToClient )
            {
            aCommPort->iCsy.Copy( KDataPortName );
            iDP1LoanedToClient = ETrue;

            TFLOGSTRING2("TSY: CMmDataPortHandler::LoanDialUpPort - %S Loaned", &aCommPort->iPort );

OstTraceExt1( TRACE_NORMAL,  DUP1_CMMDATAPORTHANDLER_LOANDIALUPPORT_TD, "CMmDataPortHandler::LoanDialUpPort;aCommPort->iPort=%S Loaned", aCommPort->iPort );
            }
         else
            {
            result = KErrEtelPortAlreadyLoaned;
            }
        }
    else if ( aCommPort->iPort == KDataPortPort4 )
        {
        if ( !iDP4LoanedToClient )
            {
            aCommPort->iCsy.Copy( KDataPortName );
            iDP4LoanedToClient = ETrue;

            TFLOGSTRING2("TSY: CMmDataPortHandler::LoanDialUpPort - %S Loaned", &aCommPort->iPort );

OstTraceExt1( TRACE_NORMAL,  DUP2_CMMDATAPORTHANDLER_LOANDIALUPPORT_TD, "CMmDataPortHandler::LoanDialUpPort;aCommPort->iPort=%S Loaned", aCommPort->iPort );
            }
         else
            {
            result = KErrEtelPortAlreadyLoaned;
            }
        }
    else if ( aCommPort->iPort == KDataPortPort5 )
        {
        if ( !iDP5LoanedToClient )
            {
            aCommPort->iCsy.Copy( KDataPortName );
            iDP5LoanedToClient = ETrue;

            TFLOGSTRING2("TSY: CMmDataPortHandler::LoanDialUpPort - %S Loaned", &aCommPort->iPort );

OstTraceExt1( TRACE_NORMAL,  DUP3_CMMDATAPORTHANDLER_LOANDIALUPPORT_TD, "CMmDataPortHandler::LoanDialUpPort;aCommPort->iPort=%S Loaned", aCommPort->iPort );
            }
         else
            {
            result = KErrEtelPortAlreadyLoaned;
            }
        }
    else
        {
        // Port number not supported
        result = KErrEtelConfigPortFailure;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMmDataPortHandler::RecoverDataPort
// Recovers dataport from the client.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMmDataPortHandler::RecoverDataPort(
    const CCallDataPackage* aCallDataPackage )
    {
TFLOGSTRING("TSY: CMmDataPortHandler::RecoverDataPort");
OstTrace0( TRACE_NORMAL,  CMMDATAPORTHANDLER_RECOVERDATAPORT_TD, "CMmDataPortHandler::RecoverDataPort" );

    TInt ret( KErrNone );
    RCall::TCommPort* commPort = NULL;

    aCallDataPackage->UnPackData( &commPort );

    if ( commPort && ECommsDataportOpened <= iCommsStatus )
        {

TFLOGSTRING2("TSY: CMmDataPortHandler::RecoverDataPort - Client returning control: %S", &commPort->iPort );
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMDATAPORTHANDLER_RECOVERDATAPORT_TD, "CMmDataPortHandler::RecoverDataPort;Client returning control=%S", commPort->iPort );
        // Check if port 0 has been loaned
        if ( iDP0LoanedToClient && commPort->iPort == KDataPortPort0 )
            {
            iDP0LoanedToClient = EFalse;
            // noone needs remaining data in buffers
            ret = iDataPort.ResetBuffers();
TFLOGSTRING2("TSY: CMmDataPortHandler::RecoverDataPort - %S Recovered", &commPort->iPort );
OstTraceExt1( TRACE_NORMAL,  DUP3_CMMDATAPORTHANDLER_RECOVERDATAPORT_TD, "CMmDataPortHandler::RecoverDataPort;commPort->iPort=%S Recovered", commPort->iPort );
            }
        else if ( iDP1LoanedToClient && commPort->iPort == KDataPortPort1 )
            {
            iDP1LoanedToClient = EFalse;
TFLOGSTRING2("TSY: CMmDataPortHandler::RecoverDataPort - %S Recovered", &commPort->iPort );
OstTraceExt1( TRACE_NORMAL,  DUP4_CMMDATAPORTHANDLER_RECOVERDATAPORT_TD, "CMmDataPortHandler::RecoverDataPort;commPort->iPort=%S Recovered", commPort->iPort );
            }
        else if ( iDP4LoanedToClient && commPort->iPort == KDataPortPort4 )
            {
            iDP4LoanedToClient = EFalse;
TFLOGSTRING2("TSY: CMmDataPortHandler::RecoverDataPort - %S Recovered", &commPort->iPort );
OstTraceExt1( TRACE_NORMAL,  DUP5_CMMDATAPORTHANDLER_RECOVERDATAPORT_TD, "CMmDataPortHandler::RecoverDataPort;commPort->iPort=%S Recovered", commPort->iPort );
            }
        else if ( iDP5LoanedToClient && commPort->iPort == KDataPortPort5 )
            {
            iDP5LoanedToClient = EFalse;
TFLOGSTRING2("TSY: CMmDataPortHandler::RecoverDataPort - %S Recovered", &commPort->iPort );
OstTraceExt1( TRACE_NORMAL,  DUP6_CMMDATAPORTHANDLER_RECOVERDATAPORT_TD, "CMmDataPortHandler::RecoverDataPort;commPort->iPort=%S Recovered", commPort->iPort );
            }
        else
            {
            ret = KErrEtelPortNotLoanedToClient;
            }
        }
    else
        {
        ret = KErrNotReady;
        }

    return ret;
    }

// ========================== OTHER EXPORTED FUNCTIONS ========================

    //None

// End of File
