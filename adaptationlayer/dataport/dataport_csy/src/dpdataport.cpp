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
#include "dpdef.h"               // dataport definitions
#include <e32svr.h>
#include <e32std.h>
#include <e32uid.h>              // epoc32 uids

#include <iscapi.h>
#include <iscnokiadefinitions.h>

#include "dpdataport.h"          // dataport main and c32 interface
#include "dpstd.h"               // fault codes etc.
#include "dpdatabuffer.h"        // base class for rx and tx buffers
#include "dpdataconfig.h"        // port configuration store
#include "dpflowctrl.h"          // flow control handling
#include "dpmif.h"               // message interface mif
#include "dpdte2tx.h"            // moves data from client to tx buffer
#include "dptx2pn.h"             // moves data from tx buffer to isaapi
#include "dppn2rx.h"             // moves data from isaapi to rx buffer
#include "dprx2dte.h"            // moves data from rx buffer to client
#include "dpcommpep.h"           // comm pep interface. interaction with mif
#include "dpescdetect.h"         // escape sequence detection
#include "dpbreak.h"             // break signal handling
#include "dpparityhandler.h"     // parity bit handling in 7-bit data mode
#include "dppif.h"               // pipe interface pif
#include "dptermdetect.h"        // terminal bytes detection
#include "pep_comm_types.h"      // structures for isi-message interface
#include "dplog.h"               // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpdataportTraces.h"
#endif

// LOCAL FUNCTION PROTOTYPES
static void CloseObject( TAny* aObject );

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// CloseObject Close an object from the cleanup stack
// Status : Proposal
// ---------------------------------------------------------
//
void CloseObject(
    TAny* aObject )
    {
    OstTrace0( TRACE_NORMAL, LOCAL_CLOSEOBJECT, "CloseObject" );
    ( ( CObject* ) aObject )->Close();
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDpDataPortFactory::CDpDataPortFactory
// C++ default constructor
// ---------------------------------------------------------
CDpDataPortFactory::CDpDataPortFactory()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORTFACTORY_CDPDATAPORTFACTORY, "CDpDataPortFactory::CDpDataPortFactory" );
    LOGM(" CDpDataPortFactory::CDpDataPortFactory");
    }

// ---------------------------------------------------------
// CDpDataPortFactory::ConstructL
// Symbian 2nd phase constructor
// ---------------------------------------------------------
void CDpDataPortFactory::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORTFACTORY_CONSTRUCTL, "CDpDataPortFactory::ConstructL" );
    LOGM(" CDpDataPortFactory::ConstructL");

    TName name( KDataPortModule ); // Used in RComm::Open()
    SetNameL( &name );

    //A version is defined by a set of three numbers
    iVersion = TVersion( EMajorVersion, EMinorVersion, EBuild );

    //Create a parity handler object
    iParityHandler = new( ELeave ) CDpParityHandler();
    }

// ---------------------------------------------------------
// CDpDataPortFactory::NewL
// Static constructor
// ---------------------------------------------------------
CDpDataPortFactory* CDpDataPortFactory::NewL()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORTFACTORY_NEWL, "CDpDataPortFactory::NewL" );
    LOGM("CDpDataPortFactory::NewL");

    CDpDataPortFactory* self = new( ELeave ) CDpDataPortFactory;

    TCleanupItem closePort( CloseObject, self );
    CleanupStack::PushL( closePort );
    self->ConstructL();
    CleanupStack::Pop(); //closePort

    return self;
    }

// ---------------------------------------------------------
// CDpDataPortFactory::~CDpDataPortFactory
// Destructor
// ---------------------------------------------------------
CDpDataPortFactory::~CDpDataPortFactory()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORTFACTORY_CDPDATAPORTFACTORY, "CDpDataPortFactory::~CDpDataPortFactory" );
    LOGM(" CDpDataPortFactory::~CDpDataPortFactory");

    if ( iParityHandler )
        {
        delete iParityHandler;
        }
    // no else
    }

// ---------------------------------------------------------
// CDpDataPortFactory::NewPortL
// This method creates a new port. This function leaves with
// KErrNotSupported, if unit number is not between
// KDataPortCommLowUnit and KDataPortCommHighUnit.
// ---------------------------------------------------------
//
CPort* CDpDataPortFactory::NewPortL(
    const TUint aUnit )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORTFACTORY_NEWPORTL, "CDpDataPortFactory::NewPortL" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPDATAPORTFACTORY_NEWPORTL, "CDpDataPortFactory:: aUnit: %u", aUnit );

    LOGM1("CDpDataPortFactory::NewPortL, unit: %d", aUnit );

    __ASSERT_ALWAYS( KDataPortCommHighUnit >= aUnit,
        User::Leave( KErrNotSupported ) );

    //Create a CDpDataPort object
    return reinterpret_cast<CPort *>(
        CDpDataPort::NewL( aUnit, iParityHandler ) );
    }

// ---------------------------------------------------------
// CDpDataPortFactory::Info
// This method fills in the supplied serial info structure.
// ---------------------------------------------------------
//
void CDpDataPortFactory::Info(
    TSerialInfo &aSerialInfo )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORTFACTORY_INFO, "CDpDataPortFactory::Info" );
    LOGM("CDpDataPortFactory::Info");

    aSerialInfo.iDescription = KDataPortDescription;
    aSerialInfo.iName = KDataPortModule;
    aSerialInfo.iLowUnit = KDataPortCommLowUnit;
    aSerialInfo.iHighUnit = KDataPortCommHighUnit;
    }

// ---------------------------------------------------------
// CDpDataPortFactory::PortPlatSecCapability
// This method returns capabilities for requested port.
// ---------------------------------------------------------
//
TSecurityPolicy CDpDataPortFactory::PortPlatSecCapability(
    TUint aPort ) const
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORTFACTORY_PORTPLATSECCAPABILITY, "CDpDataPortFactory::PortPlatSecCapability" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPDATAPORTFACTORY_PORTPLATSECCAPABILITY, "CDpDataPortFactory:: aPort: %u", aPort );

    LOGM1("CDpDataPortFactory::PortPlatSecCapability - port %d",
        aPort );

    TSecurityPolicy csySecurityPolicy;

    // Support for ports 0, 1, (2, 3,) 4, 5 & 6
    switch ( aPort )
        {
        case KDpCsdPort:
            {
            csySecurityPolicy = TSecurityPolicy(
                ECapabilityNetworkServices,
                ECapabilityNetworkControl,
                ECapabilityCommDD );
            break;
            }
        case KDpDialUpPort:
            {
            csySecurityPolicy = TSecurityPolicy(
                ECapabilityNetworkServices,
                ECapabilityNetworkControl,
                ECapabilityCommDD );
            break;
            }
        case KDpPort4:
        case KDpPort5:
        case KDpPort6:
            {
            csySecurityPolicy = TSecurityPolicy(
                ECapabilityNetworkServices,
                ECapabilityNetworkControl,
                ECapabilityCommDD );
            break;
            }
        case KDpBtAccHfPort:
        case KDpBtAccHsPort:
        // These ports 2 and 3 are implemented in Dataport and ISA for BT
        // handsfree and headset but they are in fact not used in S60 3.X ->
        default:
            {
            csySecurityPolicy = TSecurityPolicy(
                TSecurityPolicy::EAlwaysFail );
            break;
            }
        }

    return csySecurityPolicy;
    }

//===========================================================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDpDataPort::CDpDataPort
// C++ default constructor
// ---------------------------------------------------------
CDpDataPort::CDpDataPort(
    CDpParityHandler* aParityHandler,
    const TInt aUnit ) :
    iRole( ECommRoleDTE ),
    iUnit( aUnit ),
    iParityHandler( aParityHandler )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_CDPDATAPORT, "CDpDataPort::CDpDataPort" );
    LOGM(" CDpDataPort::CDpDataPort");
    }

// ---------------------------------------------------------
// CDpDataPort::NewL
// Static constructor
// ---------------------------------------------------------
CDpDataPort* CDpDataPort::NewL(
    const TUint aUnit,
    CDpParityHandler* aParityHandler )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NEWL, "CDpDataPort::NewL" );
    LOGM("CDpDataPort::NewL");

    CDpDataPort *self = new( ELeave ) CDpDataPort( aParityHandler, aUnit );
    TCleanupItem closePort( CloseObject, self );
    CleanupStack::PushL( closePort );

    // closePort is enough for cleaning purposes,
    // because CPort::Close() calls Destruct()
    self->InitL();

    TName name;
    _LIT( KUnitFormat, "%d" );
    name.Format( KUnitFormat, aUnit );
    self->SetName( &name );

    CleanupStack::Pop(); //closePort

    return self;
    }

// ---------------------------------------------------------
// CDpDataPort::~CDpDataPort
// Destructor
// ---------------------------------------------------------
CDpDataPort::~CDpDataPort()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORT_CDPDATAPORT, "CDpDataPort::~CDpDataPort" );
    OstTrace1( TRACE_NORMAL, DUP2_CDPDATAPORT_CDPDATAPORT, "CDpDataPort:: Port: %u", iUnit );

    LOGM1(" CDpDataPort::~CDpDataPort - Port %d", iUnit );

    DeleteCommonObjects();
    }

// ---------------------------------------------------------
// CDpDataPort::DeleteDataPort
// This method deletes DataPort
// ---------------------------------------------------------
//
void CDpDataPort::DeleteDataPort()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DELETEDATAPORT, "CDpDataPort::DeleteDataPort" );
    if ( this )
        {
        delete this;
        }
    //no else
    }

// ---------------------------------------------------------
// CDpDataPort::IsReadyToDestruct
// This method finds out, whether DataPort is ready to be destructed.
// ---------------------------------------------------------
//
TBool CDpDataPort::IsReadyToDestruct()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_ISREADYTODESTRUCT, "CDpDataPort::IsReadyToDestruct" );
    return iReadyToDestruct;
    }

// ---------------------------------------------------------
// CDpDataPort::DeleteCommonObjects
// This method deletes other objects and starts pipe
// termination. All operations are cancelled before
// deletion.
// ---------------------------------------------------------
//
void CDpDataPort::DeleteCommonObjects()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DELETECOMMONOBJECTS, "CDpDataPort::DeleteCommonObjects" );
    LOGM(" CDpDataPort::DeleteCommonObjects");

    // Close the pipe only if it has not been closed before
    if ( iPifDcs && !iReadyToDestruct )
        {
        TRAP_IGNORE( iPifDcs->ClosePipeL( iUnit ) );
        }
    //no else

    // Cancel all operations
    if ( iTx2Pn )
        {
        iTx2Pn->Cancel();
        }
    //no else

    if ( iPn2Rx )
        {
        iPn2Rx->Cancel();
        }
    //no else

    if ( iRx2Dte )
        {
        iRx2Dte->Cancel();
        }
    //no else

    if ( iDte2Tx )
        {
        iDte2Tx->Cancel();
        }
    //no else

    if ( iPifDcs )
        {
        iPifDcs->Cancel();
        }
    //no else

    if ( iMif )
        {
        iMif->Cancel();
        }
    //no else

    if ( iEscDetect )
        {
        iEscDetect->Cancel();
        }
    //no else

    if ( iFlowCtrl )
        {
        iFlowCtrl->Cancel();
        }
    //no else

    // Deallocate subjects before observers

    if ( iISAHandle )
        {
        iISAHandle->Close();
        }
    //no else

    if ( iDte2Tx )
        {
        delete iDte2Tx;
        iDte2Tx = NULL;
        }
    //no else

    if ( iTx2Pn )
        {
        delete iTx2Pn;
        iTx2Pn = NULL;
        }
    //no else

    if ( iPn2Rx )
        {
        delete iPn2Rx;
        iPn2Rx = NULL;
        }
    //no else

    if ( iPifDcs )
        {
        delete iPifDcs;
        iPifDcs =NULL;
        }
    //no else

    if ( iFlowCtrl )
        {
        delete iFlowCtrl;
        iFlowCtrl = NULL;
        }
    //no else

    if ( iMif )
        {
        delete iMif;
        iMif = NULL;
        }
    //no else

    if ( iRx2Dte )
        {
        delete iRx2Dte;
        iRx2Dte = NULL;
        }
    //no else

    if ( iDataConfig )
        {
        delete iDataConfig;
        iDataConfig = NULL;
        }
    //no else

    if ( iCommPep )
        {
        delete iCommPep;
        iCommPep = NULL;
        }
    //no else

    if ( iEscDetect )
        {
        delete iEscDetect;
        iEscDetect = NULL;
        }
    //no else

    if ( iTermDetect )
        {
        delete iTermDetect;
        iTermDetect = NULL;
        }
    //no else

    if ( iBreak )
        {
        delete iBreak;
        iBreak = NULL;
        }
    //no else

    if ( iRx )
        {
        delete iRx;
        iRx = NULL;
        }
    //no else

    if ( iTx )
        {
        delete iTx;
        iTx = NULL;
        }
    //no else

    if ( iISAHandle )
        {
        delete iISAHandle;
        iISAHandle = NULL;
        }
    //no else
    }

// ---------------------------------------------------------
// CDpDataPort::InitL
// This method opens Isc Api handle and creates other objects.
// Status : Proposal
// ---------------------------------------------------------
//
void CDpDataPort::InitL()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_INITL, "CDpDataPort::InitL" );
#ifdef USE_FILE_SERIAL_TRUST_DEBUG // for debugging purposes
    iDebugBytesFromDte   = 0;
    iDebugBytesToPn      = 0;
    iDebugBytesFromPn    = 0;
    iDebugBytesToDte     = 0;
    iDebugDteWrites      = 0;
    iDebugDteReads       = 0;
    iDebugPnWrites       = 0;
    iDebugPnReads        = 0;

#ifdef USE_FILE_DEBUG
    User::LeaveIfError( iFileServer.Connect( ) );
    User::LeaveIfError(
        iFile.Replace( iFileServer, KDataPortFileName, EFileWrite ) );
#endif // USE_FILE_DEBUG
#endif // USE_FILE_DEBUG || USE_SERIAL_DEBUG

    OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORT_INITL, "CDpDataPort:: *" );
    OstTraceExt2( TRACE_NORMAL, DUP2_CDPDATAPORT_INITL, "CDpDataPort:: ************* New Port %u %x ***********", iUnit, (TUint)this );

    LOGM(" CDpDataPort::InitL");
    LOG("*");
    LOG2("************* New Port %d %x ***********", iUnit, this );

    // Pull IscApi Up
    iISAHandle = new( ELeave ) RIscApi();

    // Set object ID
    TUint16 obj( 0 );

    switch ( iUnit )
        {
        // port number that is opened by dataport
        case KDpCsdPort:
            {
            obj = EIscNokiaDataport1;
            break;
            }
        case KDpDialUpPort:
            {
            obj = EIscNokiaDataport2;
            break;
            }

        case KDpBtAccHfPort:
        case KDpBtAccHsPort:
            {
            obj = EIscNokiaBtAppl;
            break;
            }
        case KDpPort4:
            {
            obj = EIscNokiaDataport3;
            break;
            }
        case KDpPort5:
            {
            obj = EIscNokiaDataport4;
            break;
            }
        case KDpPort6:
            {
            obj = EIscNokiaDataport5;
            break;
            }
        default:
            {
            User::Leave( KErrGeneral );
            break;
            }
        }

    //Initialisation of the AO's status
    TRequestStatus status( KRequestPending );
    iISAHandle->Open( obj, status, EOwnerThread, NULL );

    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );

    LOG("  CDpDataPort::InitL, iISCHandle opened");
    OstTrace0( TRACE_NORMAL, DUP3_CDPDATAPORT_INITL, "CDpDataPort:: iISCHandle opened" );

    // Create necessary serial objects.
    CreateObjectsL();
    }

// ---------------------------------------------------------
// CDpDataPort::InitializeObjVars()
// Initialize internal object variables that are not initialized.
// ---------------------------------------------------------
//
void CDpDataPort::InitializeObjVars()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_INITIALIZEOBJVARS, "CDpDataPort::InitializeObjVars" );
    LOGM("  CDpDataPort::InitializeObjVars, initialize objects variables");

    iBreak->Init();
    }

// ---------------------------------------------------------
// CDpDataPort::CreateObjectsL
// This function creates other objects, starts read from
// MIF and starts pipe initiation.
// ---------------------------------------------------------
//
void CDpDataPort::CreateObjectsL()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_CREATEOBJECTSL, "CDpDataPort::CreateObjectsL" );
    LOGM(" CDpDataPort::CreateObjectsL");

    // Get maximum data size from ISC API
    TInt maxDataSize( iISAHandle->MaximumDataSize() );

    // Create RX buffer
    iRx = CDpDataBuffer::NewL( maxDataSize, 4 * maxDataSize );

    // Create TX buffer
    iTx = CDpDataBuffer::NewL( maxDataSize, 4 * maxDataSize );

    if ( KDpMaximumEchoDataSize < maxDataSize )
        {
        LOG(" CDpDataPort::CreateObjectsL KDpMaximumEchoDataSize < maxDataSize");
        OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORT_CREATEOBJECTSL, "CDpDataPort:: KDpMaximumEchoDataSize < maxDataSize" );
        }
    //no else

    // Create Break handler
    iBreak = new( ELeave ) CDpBreak( *this );

    // Create pipe interface (PIF)
    iPifDcs = new( ELeave ) CDpPif( *this );

    // Create port configuration handler (DataConfig)
    iDataConfig = CDpDataConfig::NewL( *this );

    // Create flow control handler (FlowCtrl)
    iFlowCtrl = new (ELeave) CDpFlowCtrl( *this );

    // Create COMM PEP
    iCommPep = new (ELeave) CDpCommPep( *this );

     // Create message interface (MIF)
    iMif = CDpMif::NewL( *this );

    InitializeObjVars();

    // Serious if read fails => leave
    User::LeaveIfError( iMif->Read() );

    // Start listening ISC Flow Control Changes
    iFlowCtrl->RequestIscFlowControlNotification();

    // Initiate pipe
    User::LeaveIfError( iPifDcs->InitiatePipe( iUnit ) );

    // Create escape sequence detector (EscDetect)
    iEscDetect = new( ELeave ) CDpEscDetect( *this );

    // Create termination characters detector (TermDetect)
    iTermDetect = new( ELeave ) CDpTermDetect( *iDataConfig );

    // Create Rx2Dte
    iRx2Dte = CDpRx2Dte::NewL( *this );

    // Create Dte2Tx
    iDte2Tx = CDpDte2Tx::NewL( *this );

    // Create Tx2Pn
    iTx2Pn = CDpTx2Pn::NewL( *this );

    // Create Pn2Rx
    iPn2Rx = CDpPn2Rx::NewL( *this  );
    }

// ---------------------------------------------------------
// CDpDataPort::StartRead
// This method requests read.
// ---------------------------------------------------------
//
void CDpDataPort::StartRead(
    const TAny* aClientBuffer,
    TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_STARTREAD, "CDpDataPort::StartRead" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPDATAPORT_STARTREAD, "CDpDataPort:: Port: %u, aLength: %d", iUnit, aLength );

    LOGM2(" CDpDataPort::StartRead - Port %d, aLength = %d",
        iUnit, aLength );

    if ( iDataConfig->IsLineFail( iRole ) )
        {
        ReadCompleted( KErrCommsLineFail );
        }
    //check if there is data
    else if ( 0 == aLength )
        {
        ReadCompleted( KErrNone );
        }
    else
        {
        TInt ret( iRx2Dte->SetDteRead( aClientBuffer, aLength ) );

        if ( KErrNone != ret  )
            {
            ReadCompleted( ret );
            }
        //no else
        }
    }

// ---------------------------------------------------------
// CDpDataPort::ReadCancel
// This method cancels pending read operation.
// ---------------------------------------------------------
//
void CDpDataPort::ReadCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_READCANCEL, "CDpDataPort::ReadCancel" );
    LOGM("CDpDataPort::ReadCancel");

    iRx2Dte->ResetDteRead();
    }

// ---------------------------------------------------------
// CDpDataPort::QueryReceiveBuffer
// This method queries the count of bytes in receive buffer
// (RX or echo buffer).
// ---------------------------------------------------------
//
TInt CDpDataPort::QueryReceiveBuffer(
    TInt& aLength ) const
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_QUERYRECEIVEBUFFER, "CDpDataPort::QueryReceiveBuffer" );
    LOGM("CDpDataPort::QueryReceiveBuffer");

    // We have bytes to echo --> set aLength
    // NOTE: we could also set aLength with echoBytes + usedBytes,
    // but then we might have problem, because they are
    // actually two different buffers and we cannot write
    // them in same operation.
    if ( 0 < iRx2Dte->EchoBytes() )
        {
        aLength = iRx2Dte->EchoBytes();
        }
    else
        {
        // setaLength with rx buffer count
        aLength = iRx->UsedBytes();
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataPort::ResetBuffers
// This method resets RX and TX buffers.
// ---------------------------------------------------------
//
void CDpDataPort::ResetBuffers(
    TUint aFlags ) // Flags describing, which buffers are reseted
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_RESETBUFFERS, "CDpDataPort::ResetBuffers" );
    LOGM("CDpDataPort::ResetBuffers");

    //True if Reading is pending
    TBool readPending( EFalse );

    //  Flush RX buffer
    if ( aFlags & KCommResetRx )
        {
        // No pending reads allowed
        // C32 panics if RComm::ResetBuffers() call made, when another request
        // already in progress.
        if ( iRx2Dte->ReadPending() )
            {
            readPending = ETrue;
            }
        else
            {
            // Read / ISC DataReceive is cancelled before setting receive
            // buffer length, because pointer to write element might be
            // different after reset.

            // Cancel ISC DataReceive, release write element
            iPn2Rx->Cancel();
            // Flush real RX buffer
            iRx->Flush();
            // Flush echo data bytes
            iRx2Dte->ResetEchoBytes();

            // if previous state was high watermark then set back to lowwatermark
            // to turn flowcontrol off ISC->DP
            if ( EFlowControlOn == iFlowCtrl->FlowCtrlDcs2Dp() )
                {
                LOG("Turn Off Flow Control ISC->DP");
                OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORT_RESETBUFFERS, "CDpDataPort:: Turn Off Flow Control ISC->DP" );

                //Set flow control to EFlowControlOff
                iFlowCtrl->WaterMarkLowReached();
                }
            //no else

            // Restart ISC DataReceive
            iPn2Rx->Read();
            }
        }
    //no else

    // Flush TX buffer
    if ( ( aFlags & KCommResetTx ) && ( !readPending ) )
        {
        iTx->Flush();
        }
    //no else
    }

// ---------------------------------------------------------
// CDpDataPort::StartWrite
// This method requests write. We can complete right away, if
// we have space in TX buffer. Otherwise we have to put write
// pending and complete it later. There can be only one write
// pending at time.
// ---------------------------------------------------------
//
void CDpDataPort::StartWrite(
    const TAny* aClientBuffer,
    TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_STARTWRITE, "CDpDataPort::StartWrite" );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPDATAPORT_STARTWRITE, "CDpDataPort:: Port: %u, aLength: %d", iUnit, aLength );

    LOGM2(" CDpDataPort::StartWrite - Port %d, aLength: %d",
        iUnit, aLength );

    if ( iDataConfig->IsLineFail( iRole ) )
        {
        LOG(" ERROR, DataPort::StartWrite, Write completed with LINE FAIL.");
        OstTrace0( TRACE_NORMAL, DUP2_CDPDATAPORT_STARTWRITE, "ERROR, DataPort::StartWrite, Write completed with LINE FAIL" );

        WriteCompleted( KErrCommsLineFail );
        }
    else
        {
        TInt ret( iDte2Tx->WriteTx( aClientBuffer, aLength ) );
        switch ( ret )
            {
            case KErrNone:
                {
                LOG(" DataPort::StartWrite, Dte write completed.");
                OstTrace0( TRACE_NORMAL, DUP3_CDPDATAPORT_STARTWRITE, "CDpDataPort:: Dte write completed" );

                WriteCompleted( KErrNone );
                break;
                }
            case KErrNotReady:
                {
                // Write is pending, do not complete write
                break;
                }
            default:
                {
                // Something has gone wrong in WriteTx() (e.g. element reservation)
                // and this information is forwarded to the client interface
                LOG1(" ERROR, DataPort::StartWrite, Write completed with error: %d",
                    ret );
                OstTrace1( TRACE_NORMAL, DUP4_CDPDATAPORT_STARTWRITE, "ERROR, DataPort::StartWrite, Write completed with error: %d", ret );

                WriteCompleted( ret );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CDpDataPort::WriteCancel
// This method cancels pending write operation.
// ---------------------------------------------------------
//
void CDpDataPort::WriteCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_WRITECANCEL, "CDpDataPort::WriteCancel" );
    LOGM("CDpDataPort::WriteCancel");

    iDte2Tx->CancelWrite();
    }

// ---------------------------------------------------------
// CDpDataPort::Break
// This method sends break to DCS. DCS has configured type of
// break signal. Break time is not used. The implementation
// of break sending is in SendBreak() method.
// ---------------------------------------------------------
//
void CDpDataPort::Break(
    TInt /*aTime*/ )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_BREAK, "CDpDataPort::Break" );
    LOGM("CDpDataPort::Break");

    TInt ret( KErrNone );

    if ( iDataConfig->IsLineFail( iRole ) )
        {
        BreakCompleted( KErrCommsLineFail );
        }
    else
        {
        switch ( iDataConfig->DteTypeOfBreakSignal() )
            {
            case PEP_COMM_BREAK_DESTRUCT:
                {
                // Send break to DCS.
                ret = iBreak->SendBreak();
                // Clear TX buffer.
                iTx->Flush();
                BreakCompleted( ret );
                break;
                }
            case PEP_COMM_BREAK_EXPEDITED:
                {
                // Send break, then data.
                ret = iBreak->SendBreak();
                BreakCompleted( ret );
                break;
                }
            case PEP_COMM_BREAK_NON_EXPEDITED:
                {
                // Send TX data, then break.
                if ( 0 < iTx->UsedBytes() )
                    {
                    // There is data before break, put break pending.
                    iTx2Pn->SetBreakBytes( iTx->UsedBytes() );
                    }
                else
                    {
                    // Send break right now.
                    ret = iBreak->SendBreak();
                    BreakCompleted( ret );
                    }
                break;
                }
            default:
                {
                // Not supported break signal
                BreakCompleted( KErrNotSupported );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CDpDataPort::BreakCancel
// This method cancels pending break operation.
// ---------------------------------------------------------
//
void CDpDataPort::BreakCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_BREAKCANCEL, "CDpDataPort::BreakCancel" );
    //ASK ARTO QUESTION
    LOGM("CDpDataPort::BreakCancel");

    // Simply send message with breaktime 0
    Break( 0 );

    // Cancel pending break
    iTx2Pn->CancelBreakBytes();
    }

// ---------------------------------------------------------
// CDpDataPort::GetConfig
// This method gets current port configuration.
// ---------------------------------------------------------
//
TInt CDpDataPort::GetConfig(
    TDes8& aPackage ) const
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_GETCONFIG, "CDpDataPort::GetConfig" );
    LOGM("CDpDataPort::GetConfig");
    DEBUG( "CDpDataPort::GetConfig" )

    aPackage.FillZ( aPackage.MaxLength() );
    TInt len( Min( aPackage.MaxLength(), sizeof( TCommConfigV01 ) ) );
    aPackage.Copy( reinterpret_cast<TUint8*>(
        const_cast<TCommConfigV01*>( &iDataConfig->CommConfig() ) ), len );

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataPort::SetConfig
// This method sets current port configuration.
// ---------------------------------------------------------
//
TInt CDpDataPort::SetConfig(
    const TDesC8& aPackage )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SETCONFIG, "CDpDataPort::SetConfig" );
    LOGM("CDpDataPort::SetConfig");

    TCommConfigV01 commConfigV01;
    TInt ret(0);

    if ( aPackage.Size() == sizeof(TCommConfigV01) )
        {
        Mem::FillZ( &commConfigV01, sizeof( TCommConfigV01 ) );
        TInt len( Min( aPackage.Length(), sizeof( TCommConfigV01 ) ) );
        Mem::Copy( &commConfigV01, aPackage.Ptr(), len );

        // SetConfig back to sort the comms line out
        // once the comms line has failed

        TCommConfigV01 origConfig = iDataConfig->CommConfig();
        ret = iDataConfig->SetConfig( commConfigV01 );
        if ( KErrNone == ret )
            {
            if ( iDataConfig->IsLineFail( iRole ) )
                {
                // Set back to what it was
                iDataConfig->SetConfig( origConfig );
                ret = KErrCommsLineFail;
                }
            //no else
            }
        //no else
        }
    else
        {
        LOG(" ERROR, CDpDataPort::SetConfig, Wrong SetConfig argument");
        OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORT_SETCONFIG, "ERROR, CDpDataPort::SetConfig, Wrong SetConfig argument" );

        ret = KErrArgument;
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::SetServerConfig
// NOT supported by DataPort
// ---------------------------------------------------------
//
TInt CDpDataPort::SetServerConfig(
    const TDesC8& /*aPackage*/ )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SETSERVERCONFIG, "CDpDataPort::SetServerConfig" );
    LOGM("CDpDataPort::SetServerConfig KErrNotSupported");

    return KErrNotSupported;
    }

// ---------------------------------------------------------
// CDpDataPort::GetServerConfig
// NOT supported by DataPort
// ---------------------------------------------------------
//
TInt CDpDataPort::GetServerConfig(
    TDes8& /*aPackage*/ )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_GETSERVERCONFIG, "CDpDataPort::GetServerConfig" );
    LOGM("CDpDataPort::GetServerConfig KErrNotSupported");

    return KErrNotSupported;
    }

// ---------------------------------------------------------
// CDpDataPort::GetCaps
// This method gets port capabilities. Capabilities are
// returned on TCommCapsV01 orTCommCapsV02 depending on the
// package size.
// ---------------------------------------------------------
//
TInt CDpDataPort::GetCaps(
    TDes8& aPackage )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_GETCAPS, "CDpDataPort::GetCaps" );
    LOGM("CDpDataPort::GetCaps");

    aPackage.FillZ( aPackage.MaxLength() );

    if ( aPackage.Size() == sizeof( TCommCapsV02 ) )
        {
        // Get V02 Capabilities
        TCommCapsV02 commCapsV02;
        iDataConfig->GetCommCapsV02( commCapsV02 );

        TCommCaps2 capsBuf2( commCapsV02 );
        aPackage = capsBuf2;
        }
    else if ( aPackage.Size() == sizeof( TCommCapsV01 ) )
        {
        // Get V01 Capabilities
        TCommCapsV01 commCaps;
        iDataConfig->GetCommCaps( commCaps );
        TCommCaps capsBuf( commCaps );
        aPackage = capsBuf;
        }
    else
        {
        LOG(" ERROR, CDpDataPort::GetCaps, Unsupported TCommCaps");
        OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORT_GETCAPS, "ERROR, CDpDataPort::GetCaps, Unsupported TCommCaps" );
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataPort::GetSignals
// This methods gets current status of signals.
// ---------------------------------------------------------
//
TInt CDpDataPort::GetSignals(
    TUint& aSignals )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_GETSIGNALS, "CDpDataPort::GetSignals" );
    LOGM("CDpDataPort::GetSignals");

    iDataConfig->GetSignals( aSignals );

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataPort::SetSignalsToMark
// This method sets signals to mark i.e. set signals ON.
// This method uses CDpDataConfig::SetSignals2Cmt() method
// to set signals. Updated signals and information about
// changed signals are given to
// CDpDataConfig::SetSignals2Cmt().
// ---------------------------------------------------------
//
TInt CDpDataPort::SetSignalsToMark(
    TUint aSignals )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SETSIGNALSTOMARK, "CDpDataPort::SetSignalsToMark" );
    LOGM("CDpDataPort::SetSignalsToMark");
    DEBUG( "CDpDataPort::SetSignalsToMark" )

    TInt ret ( KErrNone );

    if ( aSignals )
        {
        TUint currentSignals;

        if ( iRole == ECommRoleDTE )
            {
            aSignals &= KSignalDTEOutputs;
            }
        else
            {
            aSignals &= KSignalDCEOutputs;
            }

        iDataConfig->GetSignals( currentSignals );

        if ( aSignals )
            {
             // Set signal bit
            currentSignals |= aSignals;
            // Set changed bit
            currentSignals |= aSignals * KSignalChanged;

            ret = iDataConfig->SetSignals2Cmt( currentSignals );
            }
        //no else
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::SetSignalsToSpace
// This method sets signals to space i.e. set signals OFF.
// This method uses CDpDataConfig::SetSignals2Cmt() method
// to set signals. Updated signals and information about
// changed signals are given to
// CDpDataConfig::SetSignals2Cmt().
// ---------------------------------------------------------
//
TInt CDpDataPort::SetSignalsToSpace(
    TUint aSignals )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SETSIGNALSTOSPACE, "CDpDataPort::SetSignalsToSpace" );
    LOGM("CDpDataPort::SetSignalsToSpace");

    TInt ret ( KErrNone );

    if ( aSignals )
        {
        TUint currentSignals( 0 );

        if (  ECommRoleDTE == iRole )
            {
            aSignals &= KSignalDTEOutputs;
            }
        else
            {
            aSignals &= KSignalDCEOutputs;
            }

        iDataConfig->GetSignals( currentSignals );

        if ( aSignals )
            {
            // Clear signal bit
            currentSignals &= ~aSignals;
            // Set changed bit
            currentSignals |= aSignals * KSignalChanged;
            ret = iDataConfig->SetSignals2Cmt( currentSignals );
            }
        //no else
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::GetReceiveBufferLength
// This method gets the size of receive (RX) buffer.
// ---------------------------------------------------------
//
TInt CDpDataPort::GetReceiveBufferLength(
    TInt& aLength ) const
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_GETRECEIVEBUFFERLENGTH, "CDpDataPort::GetReceiveBufferLength" );
    LOGM("CDpDataPort::GetReceiveBufferLength");

    aLength = iRx->Size();

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataPort::SetReceiveBufferLength
// This method sets the size of receive (RX) buffer.
// ---------------------------------------------------------
//
TInt CDpDataPort::SetReceiveBufferLength(
    TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SETRECEIVEBUFFERLENGTH, "CDpDataPort::SetReceiveBufferLength" );
    LOGM("CDpDataPort::SetReceiveBufferLength");

    TInt ret( KErrNone );

    // Read / ISC DataReceive is cancelled before setting receive
    // buffer length, because pointer to write element might be
    // different after seting length.

    // Cancel ISC DataReceive, release write element
    iPn2Rx->Cancel();

    // Client cannot change RX buffer size smaller than initial size,
    // because of slow response to pipe flow control.
    if ( KDpRxBufferInitialSize > aLength ||
        KDpMaximumSetReceiveBufferLength < aLength )
        {
        ret = KErrArgument;
        }
    else
        {
        ret = iRx->SetLength( aLength );
        }

    // Restart ISC DataReceive
    iPn2Rx->Read();

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::Destruct
// This method deletes CDpDataPort port.
// ---------------------------------------------------------
//
void CDpDataPort::Destruct()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DESTRUCT, "CDpDataPort::Destruct" );
    OstTrace0( TRACE_NORMAL, DUP1_CDPDATAPORT_DESTRUCT, "********** CDpDataPort::Destruct **********" );

    LOGM("CDpDataPort::Destruct");
    LOG("********** CDpDataPort::Destruct **********");

    // Close the pipe and start disconnection sequence
    if ( iPifDcs && CDpPif::EDpPifInitial != iPifDcs->PifState() )
        {
        if ( iPn2Rx->IsHighWaterMarkReached() )
            {
            ResetBuffers( KCommResetRx | KCommResetTx );
            }
        //no else
        iReadyToDestruct = ETrue;
        TRAP_IGNORE( iPifDcs->ClosePipeL( iUnit ) );
        }
    // Delete DataPort
    else
        {
        delete this;
        }
    }

// ---------------------------------------------------------
// CDpDataPort::FreeMemory()
// NOT supported by DataPort
// ---------------------------------------------------------
//
void CDpDataPort::FreeMemory()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_FREEMEMORY, "CDpDataPort::FreeMemory" );
    LOGM("CDpDataPort::FreeMemory- Do nothing");
    }

// ---------------------------------------------------------
// CDpDataPort::ISAHandle
// This method returns Isc Api handle (RIscApi).
// ---------------------------------------------------------
//
RIscApi& CDpDataPort::ISAHandle()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_ISAHANDLE, "CDpDataPort::ISAHandle" );
    LOGM("CDpDataPort::ISAHandle");

    return *iISAHandle;
    }

// ---------------------------------------------------------
// CDpDataPort::BreakHandler
// This method returns reference to break signal handler
// (CDpBreak).
// ---------------------------------------------------------
//
CDpBreak& CDpDataPort::BreakHandler()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_BREAKHANDLER, "CDpDataPort::BreakHandler" );
    LOGM(" CDpDataPort::BreakHandler");

    return *iBreak;
    }

// ---------------------------------------------------------
// CDpDataPort::CommPep
// This method returns reference to COMM PEP
// (CDpCommPep).
// ---------------------------------------------------------
//
CDpCommPep& CDpDataPort::CommPep()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_COMMPEP, "CDpDataPort::CommPep" );
    LOGM(" CDpDataPort::CommPep");

    return *iCommPep;
    }

// ---------------------------------------------------------
// CDpDataPort::DataConfig
// This method returns reference to data configuration
// (CDpDataConfig).
// ---------------------------------------------------------
//
CDpDataConfig& CDpDataPort::DataConfig()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DATACONFIG, "CDpDataPort::DataConfig" );
    LOGM(" CDpDataPort::DataConfig");

    return *iDataConfig;
    }

// ---------------------------------------------------------
// CDpDataPort::EscDetect
// This method returns reference to escape sequence
// detector (CDpEscDetect).
// ---------------------------------------------------------
//
CDpEscDetect& CDpDataPort::EscDetect()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_ESCDETECT, "CDpDataPort::EscDetect" );
    LOGM(" CDpDataPort::EscDetect");

    return *iEscDetect;
    }

// ---------------------------------------------------------
// CDpDataPort::FlowCtrl
// This method returns reference to flow control handler
// (CDpFlowCtrl).
// ---------------------------------------------------------
//
CDpFlowCtrl& CDpDataPort::FlowCtrl()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_FLOWCTRL, "CDpDataPort::FlowCtrl" );
    LOGM(" CDpDataPort::FlowCtrl");

    return *iFlowCtrl;
    }

// ---------------------------------------------------------
// CDpDataPort::Mif
// This method returns reference to message interface (MIF).
// ---------------------------------------------------------
//
CDpMif& CDpDataPort::Mif()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_MIF, "CDpDataPort::Mif" );
    LOGM(" CDpDataPort::Mif");

    return *iMif;
    };

// ---------------------------------------------------------
// CDpDataPort::ParityHandler
// This method returns reference to parity handler
// (CDpParityHandler).
// ---------------------------------------------------------
//
CDpParityHandler& CDpDataPort::ParityHandler()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_PARITYHANDLER, "CDpDataPort::ParityHandler" );
    LOGM(" CDpDataPort::ParityHandler");

    return *iParityHandler;
    }

// ---------------------------------------------------------
// CDpDataPort::Pif
// This method returns reference to pipe interface (PIF).
// ---------------------------------------------------------
//
CDpPif& CDpDataPort::Pif()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_PIF, "CDpDataPort::Pif" );
    LOGM(" CDpDataPort::Pif");

    return *iPifDcs;
    }

// ---------------------------------------------------------
// CDpDataPort::Rx
// This method returns reference to to RX buffer.
// ---------------------------------------------------------
//
CDpDataBuffer& CDpDataPort::Rx()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_RX, "CDpDataPort::Rx" );
    LOGM(" CDpDataPort::Rx");

    return *iRx;
    }

// ---------------------------------------------------------
// CDpDataPort::Rx2Dte
// This method returns reference to Rx2Dte.
// ---------------------------------------------------------
//
CDpRx2Dte& CDpDataPort::Rx2Dte()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_RX2DTE, "CDpDataPort::Rx2Dte" );
    LOGM(" CDpDataPort::Rx2Dte");

    return *iRx2Dte;
    }

// ---------------------------------------------------------
// CDpDataPort::TermDetect
// This method returns reference to terminator characters
// detector (CDpTermDetect).
// ---------------------------------------------------------
//
CDpTermDetect& CDpDataPort::TermDetect()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_TERMDETECT, "CDpDataPort::TermDetect" );
    LOGM(" CDpDataPort::TermDetect");

    return *iTermDetect;
    }

// ---------------------------------------------------------
// CDpDataPort::Tx
// This method returns reference to to TX buffer.
// ---------------------------------------------------------
//
CDpDataBuffer& CDpDataPort::Tx()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_TX, "CDpDataPort::Tx" );
    LOGM(" CDpDataPort::Tx");

    return *iTx;
    }

// ---------------------------------------------------------
// CDpDataPort::SignalTx2Pn
// This method signals pipe interface (PIF). PIF is not
// signalled, if the request is pending.
// ---------------------------------------------------------
//
TInt CDpDataPort::SignalTx2Pn()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SIGNALTX2PN, "CDpDataPort::SignalTx2Pn" );
    LOGM(" CDpDataPort::SignalTx2Pn");
    DEBUG( "Signal Tx2Pn" )

    TInt ret( KErrNone );

    if ( iTx2Pn && ( iTx2Pn->iStatus == KRequestPending ) )
        {
        iTx2Pn->ReleaseNotify();
        }
    else
        {
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::SignalPif
// This method signals pipe interface (PIF). PIF is not
// signalled, if the request is pending.
// ---------------------------------------------------------
//
TInt CDpDataPort::SignalPif(
    const TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SIGNALPIF, "CDpDataPort::SignalPif" );
    LOGM(" CDpDataPort::SignalPif");
    DEBUG( "Signal Pif" )

    TInt ret( KErrNone );

    if ( iPifDcs && ( iPifDcs->iStatus == KRequestPending ) )
        {
        TRequestStatus* status = &iPifDcs->iStatus;
        User::RequestComplete( status, aError );
        }
    else
        {
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::SignalDte2Tx
// This method signals Dte2Tx active object (CDte2Tx). Dte2Tx
// is not signalled, if the request is pending.
// ---------------------------------------------------------
//
TInt CDpDataPort::SignalDte2Tx()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SIGNALDTE2TX, "CDpDataPort::SignalDte2Tx" );
    LOGM(" CDpDataPort::SignalDte2Tx");
    DEBUG( "Signal Dte2Tx" )

    TInt ret( KErrNone );

    if ( iDte2Tx && ( iDte2Tx->iStatus == KRequestPending ) )
        {
        iDte2Tx->ReleaseNotify();
        }
    else
        {
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::SignalRx2Dte
// This method signals Rx2Dte active object (CDpRx2Dte).
// Rx2Dte is not signalled, if the request is pending. Rx2Dte
// is also not signalled, if there is not enough bytes to
// write and terminator count is zero. There is not enough
// bytes, when the amount of bytes, which still needs to be
// written to DTE (user length - write offset), is greater
// than amount of bytes in RX buffer (including echo bytes).
// ---------------------------------------------------------
//
TInt CDpDataPort::SignalRx2Dte()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SIGNALRX2DTE, "CDpDataPort::SignalRx2Dte" );
    LOGM(" CDpDataPort::SignalRx2Dte");
    DEBUG( "SignalRx2Dte" )

    // Optimaze in order to avoid unnecessary signaling.
    TInt userLength( iRx2Dte->LengthWanted() );
    TInt ret( KErrNone );
    TBool notEnoughByte( EFalse );

    //user length can be negative
    if ( 0 <= userLength )
        {
        // Client reads with length and timeout.
        TInt bytesInRx( iRx->UsedBytes() + iRx2Dte->EchoBytes() );

        // Start discharging Rx buffer, if high water mark is reached
        if ( ( ( bytesInRx + iRx2Dte->IPCWriteOffset() < userLength ) &&
            iDataConfig->TerminatorCount() == 0 ) &&
            !( iPn2Rx->IsHighWaterMarkReached() ) )
            {
            // Not enough bytes
            notEnoughByte = ETrue;
            }
        //no else
        }
    //no else

    // Read one or more' or terminator read
    // At this point we know there is enough data in the buffer
    // or we are using terminator read.
    if ( !notEnoughByte )
        {
        if ( iRx2Dte && ( iRx2Dte->iStatus == KRequestPending ) )
            {
            iRx2Dte->ReleaseNotify();
            }
        else
            {
            ret = KErrNotReady;
            }
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::IsRx2DteReadPending
// This method finds out, whether client's read is pending
// in Rx2Dte (CDpRx2Dte).
// ---------------------------------------------------------
//
TBool CDpDataPort::IsRx2DteReadPending()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_ISRX2DTEREADPENDING, "CDpDataPort::IsRx2DteReadPending" );
    LOGM(" CDpDataPort::IsRx2DteReadPending");

    return iRx2Dte->ReadPending();
    }

// ---------------------------------------------------------
// CDpDataPort::NotifySignalChange
// This method sets up flag to indicate that we must notify
// C32 about signal changes.
// ---------------------------------------------------------
//
void CDpDataPort::NotifySignalChange(
    TUint aSignalMask )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYSIGNALCHANGE, "CDpDataPort::NotifySignalChange" );
    LOGM("CDpDataPort::NotifySignalChange");

    iDataConfig->SetSignalNotify( ETrue );
    iDataConfig->SetNotifySignalMask( aSignalMask );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifySignalChangeCancel
// This method cancels an outstanding NotifySignalChange
// request.
// ---------------------------------------------------------
//
void CDpDataPort::NotifySignalChangeCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYSIGNALCHANGECANCEL, "CDpDataPort::NotifySignalChangeCancel" );
    LOGM("CDpDataPort::NotifySignalChangeCancel");
    DEBUG( "CDpDataPort::NotifySignalChangeCancel" )

    iDataConfig->SetSignalNotify( EFalse );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyConfigChange
// This method sets up flag to indicate that we must notify
// C32 about port configuration changes of DCS.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyConfigChange()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYCONFIGCHANGE, "CDpDataPort::NotifyConfigChange" );
    LOGM("CDpDataPort::NotifyConfigChange");
    DEBUG( "CDpDataPort::NotifyConfigChange" )

    iDataConfig->SetConfigNotify( ETrue );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyConfigChangeCancel
// This method cancels an outstanding NotifyConfigChange
// request.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyConfigChangeCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYCONFIGCHANGECANCEL, "CDpDataPort::NotifyConfigChangeCancel" );
    LOGM("CDpDataPort::NotifyConfigChangeCancel");
    DEBUG( "CDpDataPort::NotifyConfigChangeCancel" )

    iDataConfig->SetConfigNotify( EFalse );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyFlowControlChange
// This method sets up flag to indicate that we must notify
// C32 about flow control changes between DP and DCS.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyFlowControlChange()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYFLOWCONTROLCHANGE, "CDpDataPort::NotifyFlowControlChange" );
    LOGM("CDpDataPort::NotifyFlowControlChange");
    DEBUG( "CDpDataPort::NotifyFlowControlChange" )

    iFlowCtrl->SetFlowControlNotify( ETrue );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyFlowControlChangeCancel
// This method cancels an outstanding NotifyFlowControlChange
// request.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyFlowControlChangeCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYFLOWCONTROLCHANGECANCEL, "CDpDataPort::NotifyFlowControlChangeCancel" );
    LOGM("CDpDataPort::NotifyFlowControlChangeCancel");
    DEBUG( "CDpDataPort::NotifyFlowControlChangeCancel" )

    iFlowCtrl->SetFlowControlNotify( EFalse );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyBreak
// This method sets up flag to indicate that we must notify
// C32, when break signal from DCS is detected.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyBreak()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYBREAK, "CDpDataPort::NotifyBreak" );
    LOGM("CDpDataPort::NotifyBreak");
    DEBUG( "CDpDataPort::NotifyBreak" )

    iBreak->SetBreakNotify( ETrue );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyBreakCancel
// This method cancels an outstanding NotifyBreak request.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyBreakCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYBREAKCANCEL, "CDpDataPort::NotifyBreakCancel" );
    LOGM("CDpDataPort::NotifyBreakCancel");
    DEBUG( "CDpDataPort::NotifyBreakCancel" )

    iBreak->SetBreakNotify( EFalse );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyDataAvailable
// This method sets up flag to indicate that we must notify
// C32, when there has appeared data into receive (RX)
// buffer.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyDataAvailable()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYDATAAVAILABLE, "CDpDataPort::NotifyDataAvailable" );
    LOGM("CDpDataPort::NotifyDataAvailable");

    iPn2Rx->SetDataAvailableNotification( ETrue );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyDataAvailableCancel
// This method cancels an outstanding NotifyDataAvailable
// request.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyDataAvailableCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYDATAAVAILABLECANCEL, "CDpDataPort::NotifyDataAvailableCancel" );
    LOGM("CDpDataPort::NotifyDataAvailableCancel");

    iPn2Rx->SetDataAvailableNotification( EFalse );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyOutputEmpty
// This method sets up flag to indicate that we must notify
// C32, when transmit (TX) buffer is emptied.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyOutputEmpty()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYOUTPUTEMPTY, "CDpDataPort::NotifyOutputEmpty" );
    LOGM("CDpDataPort::NotifyOutputEmpty");
    DEBUG( "CDpDataPort::NotifyOutputEmpty" )

    iTx2Pn->SetOutputBufferEmptyNotification( ETrue );
    }

// ---------------------------------------------------------
// CDpDataPort::NotifyOutputEmptyCancel
// This method cancels an outstanding NotifyOutputEmpty
// request.
// ---------------------------------------------------------
//
void CDpDataPort::NotifyOutputEmptyCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_NOTIFYOUTPUTEMPTYCANCEL, "CDpDataPort::NotifyOutputEmptyCancel" );
    LOGM("CDpDataPort::NotifyOutputEmptyCancel");
    DEBUG( "CDpDataPort::NotifyOutputEmptyCancel" )

    iTx2Pn->SetOutputBufferEmptyNotification( EFalse );
    }

// ---------------------------------------------------------
// CDpDataPort::GetFlowControlStatus
// This method gets current DP->DCS flow control status.
// ---------------------------------------------------------
//
TInt CDpDataPort::GetFlowControlStatus(
    TFlowControl& aFlowControl)
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_GETFLOWCONTROLSTATUS, "CDpDataPort::GetFlowControlStatus" );
    LOGM("CDpDataPort::GetFlowControlStatus");

    iFlowCtrl->GetDp2DcsFlowControlStatus( aFlowControl );

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataPort::SetRole
// This method sets role (DCE/DTE) of DataPort.
// ---------------------------------------------------------
//
TInt CDpDataPort::SetRole(
    TCommRole aRole )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SETROLE, "CDpDataPort::SetRole" );
    LOGM("CDpDataPort::SetRole");
    DEBUG( "CDpDataPort::SetRole" )

    TInt result( KErrNone );

    switch ( aRole )
        {
        case ECommRoleDCE: // Fall through
        case ECommRoleDTE:
            {
            // DTE/DCE is supported
            iRole = aRole;
            // configuring role capabilities
            result = iDataConfig->SetRoleCap( aRole );
            break;
            }
        default:
            {
            result = KErrNotSupported;
            }
        }

    return result;
    }

// ---------------------------------------------------------
// CDpDataPort::GetRole
// This method gets role (DTE/DCE) of DataPort.
// ---------------------------------------------------------
//
TInt CDpDataPort::GetRole(
    TCommRole& aRole )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_GETROLE, "CDpDataPort::GetRole" );
    LOGM("CDpDataPort::GetRole");

    aRole = iRole;

    return KErrNone;
    }

// ---------------------------------------------------------
// CDpDataPort::ActivePipeHandle
// This method gives pipe handle to enabled or disabled pipe.
// ---------------------------------------------------------
//
TInt CDpDataPort::ActivePipeHandle(
    TUint8& aHandle )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_ACTIVEPIPEHANDLE, "CDpDataPort::ActivePipeHandle" );
    LOGM(" CDpDataPort::ActivePipeHandle");

    TInt ret( KErrNone );

    CDpPif::TDpPipeState pipeState = iPifDcs->PipeState();
    if ( CDpPif::EDpPipeEnabled == pipeState ||
        CDpPif::EDpPipeDisabled == pipeState )
        {
        aHandle = iPifDcs->PipeHandle();
        }
    else
        {
        aHandle = 0;

        LOG1("  ERROR, CDpDataPort::ActivePipeHandle, Invalid pipe state: %d.",
            pipeState);
        OstTrace1( TRACE_NORMAL, DUP1_CDPDATAPORT_ACTIVEPIPEHANDLE, "ERROR, CDpDataPort::ActivePipeHandle, Invalid pipe state: %{TDpPipeState}", pipeState );

        ret = KErrGeneral;
        }
    return ret;
    }

// ---------------------------------------------------------
// CDpDataPort::SetRxBreakBytes
// This method sets amount bytes to be sent before pending
// break.
// ---------------------------------------------------------
//
TInt CDpDataPort::SetRxBreakBytes(
    const TInt aBreakBytes )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_SETRXBREAKBYTES, "CDpDataPort::SetRxBreakBytes" );
    LOGM(" CDpDataPort::SetRxBreakBytes");
    DEBUG( "CDpDataPort::SetRxBreakBytes()" )

    iRx2Dte->SetBreakBytes( aBreakBytes );

    return KErrNone;
    }

#ifdef USE_LOGGER
// ---------------------------------------------------------
// CDpDataPort::WriteStateLog
// This method writes a state log.
// ---------------------------------------------------------
//
void CDpDataPort::WriteStateLog()
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_WRITESTATELOG, "CDpDataPort::WriteStateLog" );
    LOG(" CDpDataPort::WriteStateLog");
#ifdef USE_BYTE_COUNT_DEBUG

    TInt rxlen( iRx->UsedBytes() );
    TInt txlen( iTx->UsedBytes() );

    #ifdef USE_FILE_DEBUG

    OstTrace0( TRACE_DETAILED, DUP1_CDPDATAPORT_WRITESTATELOG, "CDpDataPort:: --- DATA LOG: ----------------------------------------------" );
    OstTraceExt2( TRACE_DETAILED, DUP2_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugDteWrites=    %d iDebugBytesFromDte=    %d", iDebugDteWrites, iDebugBytesFromDte );
    OstTraceExt2( TRACE_DETAILED, DUP3_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugDteReads=     %d iDebugBytesToDte=      %d", iDebugDteReads, iDebugBytesToDte );
    OstTraceExt2( TRACE_DETAILED, DUP4_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugPnWrites=     %d iDebugBytesToPn=       %d", iDebugPnWrites, iDebugBytesToPn );
    OstTraceExt2( TRACE_DETAILED, DUP5_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugPnReads=      %d iDebugBytesFromPn=     %d", iDebugPnReads, iDebugBytesFromPn );
    OstTrace0( TRACE_DETAILED, DUP6_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|" );
    OstTraceExt2( TRACE_DETAILED, DUP7_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|rxlen= %d txlen= %d", rxlen, txlen );
    OstTrace0( TRACE_DETAILED, DUP8_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|" );
    OstTraceExt2( TRACE_DETAILED, DUP9_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iRx->iHead=     %d iRx->iTail=  %d", iRx->iHead, iRx->iTail );
    OstTraceExt2( TRACE_DETAILED, DUP10_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iRx->iTailWrp=  %d iRx->iEnd=   %d", iRx->iTailWhenWrappedAround, iRx->iEnd );
    OstTraceExt2( TRACE_DETAILED, DUP11_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iTx->iHead=     %d iTx->iTail=  %d", iTx->iHead, iTx->iTail );
    OstTraceExt2( TRACE_DETAILED, DUP12_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iTx->iTailWrp=  %d iTx->iEnd=   %d", iTx->iTailWhenWrappedAround, iTx->iEnd );
    OstTrace0( TRACE_DETAILED, DUP13_CDPDATAPORT_WRITESTATELOG, "CDpDataPort:: ------------------------------------------------------------" );

    BDEBUG("\n  DATA LOG:------------------------------------------------");
    BFDEBUG2("|iDebugDteWrites=    %d\tiDebugBytesFromDte=    %d",
        iDebugDteWrites, iDebugBytesFromDte );
    BFDEBUG2("|iDebugDteReads=     %d\tiDebugBytesToDte=      %d",
        iDebugDteReads, iDebugBytesToDte );

    BFDEBUG2("|iDebugPnWrites=     %d\tiDebugBytesToPn=       %d",
        iDebugPnWrites, iDebugBytesToPn );
    BFDEBUG2("|iDebugPnReads=      %d\tiDebugBytesFromPn=     %d",
        iDebugPnReads, iDebugBytesFromPn );
    BDEBUG("|");
    BFDEBUG2("|rxlen= %d\ttxlen= %d", rxlen, txlen );
    BDEBUG("|");
    BFDEBUG2("|iRx->iHead=     %d\tiRx->iTail=  %d", iRx->iHead, iRx->iTail );
    BFDEBUG2("|iRx->iTailWrp=  %d\tiRx->iEnd=   %d",
        iRx->iTailWhenWrappedAround, iRx->iEnd );

    BFDEBUG2("|iTx->iHead=     %d\tiTx->iTail=  %d", iTx->iHead, iTx->iTail );
    BFDEBUG2("|iTx->iTailWrp=  %d\tiTx->iEnd=   %d",
        iTx->iTailWhenWrappedAround, iTx->iEnd );

    BDEBUG(" ------------------------------------------------------------");

    #endif  // USE_FILE_DEBUG

    #if ( defined USE_SERIAL_DEBUG || defined USE_TRUST_DEBUG )

    OstTrace0( TRACE_DETAILED, DUP14_CDPDATAPORT_WRITESTATELOG, "CDpDataPort:: --- DATA LOG: ----------------------------------------------" );
    OstTrace1( TRACE_DETAILED, DUP15_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugDteWrites=    %d", iDebugDteWrites );
    OstTrace1( TRACE_DETAILED, DUP16_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugBytesFromDte= %d", iDebugBytesFromDte );
    OstTrace1( TRACE_DETAILED, DUP17_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugDteReads=     %d", iDebugDteReads );
    OstTrace1( TRACE_DETAILED, DUP18_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugBytesToDte=   %d", iDebugBytesToDte );
    OstTrace1( TRACE_DETAILED, DUP19_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugPnWrites=     %d", iDebugPnWrites );
    OstTrace1( TRACE_DETAILED, DUP20_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugBytesToPn=    %d", iDebugBytesToPn );
    OstTrace1( TRACE_DETAILED, DUP21_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugPnReads=      %d", iDebugPnReads );
    OstTrace1( TRACE_DETAILED, DUP22_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|iDebugBytesFromPn=  %d", iDebugBytesFromPn );
    OstTrace0( TRACE_DETAILED, DUP23_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|" );
    OstTrace1( TRACE_DETAILED, DUP24_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|rxlen= %d", rxlen );
    OstTrace1( TRACE_DETAILED, DUP25_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|txlen= %d", txlen );
    OstTrace0( TRACE_DETAILED, DUP26_CDPDATAPORT_WRITESTATELOG, "CDpDataPort::|" );
    OstTrace0( TRACE_DETAILED, DUP27_CDPDATAPORT_WRITESTATELOG, "CDpDataPort:: ------------------------------------------------------------" );

    RDEBUG("\n  DATA LOG:------------------------------------------------");
    FRDEBUG("|iDebugDteWrites=    %d", iDebugDteWrites );
    FRDEBUG("|iDebugBytesFromDte= %d", iDebugBytesFromDte );
    FRDEBUG("|iDebugDteReads=     %d", iDebugDteReads );
    FRDEBUG("|iDebugBytesToDte=   %d", iDebugBytesToDte );
    FRDEBUG("|iDebugPnWrites=     %d", iDebugPnWrites );
    FRDEBUG("|iDebugBytesToPn=    %d", iDebugBytesToPn );
    FRDEBUG("|iDebugPnReads=      %d", iDebugPnReads );
    FRDEBUG("|iDebugBytesFromPn=  %d", iDebugBytesFromPn );

    RDEBUG("|");
    FRDEBUG("|rxlen= %d", rxlen );
    FRDEBUG("|txlen= %d", txlen );
    RDEBUG("|");
    RDEBUG(" ------------------------------------------------------------");

    #endif  // USE_SERIAL_DEBUG || USE_TRUST_DEBUG

    #endif  // USE_BYTE_COUNT_DEBUG
    }
#endif  // USE_LOGGER


#ifdef USE_FILE_SERIAL_TRUST_DEBUG
// ---------------------------------------------------------
// CDpDataPort::DebugBytesFromDte
// This method stores amount of bytes received from DTE.
// ---------------------------------------------------------
//
void CDpDataPort::DebugBytesFromDte(
    const TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DEBUGBYTESFROMDTE, "CDpDataPort::DebugBytesFromDte" );
    LOG(" CDpDataPort::DebugBytesFromDte");

    // Amount of bytes is increased by this length
    iDebugBytesFromDte += aLength;
    iDebugDteWrites++;
    }

// ---------------------------------------------------------
// CDpDataPort::DebugBytesFromPn
// This method stores amount of bytes received from IsaApi
// (DCS).
// ---------------------------------------------------------
//
void CDpDataPort::DebugBytesFromPn(
    const TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DEBUGBYTESFROMPN, "CDpDataPort::DebugBytesFromPn" );
    LOG(" CDpDataPort::DebugBytesFromPn");

    // Amount of bytes is increased by this length
    iDebugBytesFromPn += aLength;
    iDebugPnReads++;
    }

// ---------------------------------------------------------
// CDpDataPort::DebugBytesToDte
// This method stores amount of bytes sent to DTE.
// ---------------------------------------------------------
//
void CDpDataPort::DebugBytesToDte(
    const TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DEBUGBYTESTODTE, "CDpDataPort::DebugBytesToDte" );
    LOG(" CDpDataPort::DebugBytesToDte");

    // Amount of bytes is increased by this length
    iDebugBytesToDte += aLength;
    iDebugDteReads++;
    }

// ---------------------------------------------------------
// CDpDataPort::DebugBytesToPn
// This method stores amount of bytes sent to IsaApi (DCS).
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
void CDpDataPort::DebugBytesToPn(
    const TInt aLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAPORT_DEBUGBYTESTOPN, "CDpDataPort::DebugBytesToPn" );
    LOG(" CDpDataPort::DebugBytesToPn");

    // Amount of bytes is increased by this length
    iDebugBytesToPn += aLength;
    iDebugPnWrites++;
    }
#endif  // USE_FILE_SERIAL_TRUST_DEBUG


// ================= OTHER EXPORTED FUNCTIONS ==============

extern "C"
    {
    IMPORT_C CSerial * LibEntry(); // Force export
    }

// ---------------------------------------------------------
// LibEntry
// This method implements main entry point of the library.
// Returns: CSerial*: CSerial Pointer to DataPort factory
// ---------------------------------------------------------
EXPORT_C CSerial * LibEntry()
    {
    OstTrace0( TRACE_NORMAL, LIBENTRY, "LibEntry" );
    CDpDataPortFactory* dataPortFactory = NULL;

    TRAPD( ret, dataPortFactory = CDpDataPortFactory::NewL() );

    if ( KErrNone != ret )
        {
        dataPortFactory = NULL;
        }
    //no else

    return dataPortFactory;
    }

//  End of File

