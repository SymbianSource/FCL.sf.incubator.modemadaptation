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
#include <tisi.h>
#include <nsisi.h>      // isi name
#include <pipeisi.h>    // isi pipe
#include <pipe_sharedisi.h>

#include "dpdef.h"      // dataport definitions
#include "dpdataport.h" // dataport main and c32 interface
#include "dpmif.h"      // message interface mif
#include "dppif.h"      // pipe interface pif
#include "dpcommpep.h"  // comm pep interface. interaction with mif
#include "dplog.h"      // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpmifTraces.h"
#endif


// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES
// none

// CONSTANTS
#ifdef OST_TRACE_COMPILER_IN_USE
const TUint8 KBufferForOSTTracing = 100;
#endif

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// ============================= LOCAL FUNCTIONS ===============================

// Local function for printing ISI message in OST
#ifdef OST_TRACE_COMPILER_IN_USE
static void OstPrintIsiMessage( const TDesC8& aBuf )
    {
    _LIT( KTraceSpace, " " );
    TInt length = aBuf.Length();
    TBuf8<KBufferForOSTTracing> trace_msg;
    TUint counter = 1;
    TBool firstTime = ETrue;
    for ( TInt i = 0; i < length; i++ )
        {
        trace_msg.AppendNumFixedWidthUC( aBuf[i], EHex, 2 );
        trace_msg.Append( KTraceSpace );

        if ( ( counter > 24 ) ||     // 25 bytes / line
           ( ( i + 1 ) == length ) ) // All bytes collected
            {
            if ( ( firstTime ) &&
               ( ( i + 1 ) == length ) ) // All bytes collected and traced
                {
                firstTime = EFalse;
                OstTraceExt1( TRACE_NORMAL, DUP_OSTPRINTISIMESSAGE, "CDpMif:: ==> [ %s]", trace_msg );
                trace_msg.SetLength( 0 );
                }
            else if ( firstTime ) // 1st line of the trace
                {
                firstTime = EFalse;
                OstTraceExt1( TRACE_NORMAL, DUP1_OSTPRINTISIMESSAGE, "CDpMif:: ==> [ %s", trace_msg );
                trace_msg.SetLength( 0 );
                }
            else if ( ( i + 1 ) == length ) // The last line
                {
                OstTraceExt1( TRACE_NORMAL, DUP2_OSTPRINTISIMESSAGE, "CDpMif:: ==>   %s]", trace_msg );
                trace_msg.SetLength( 0 );
                }
            else // just print bytes
                {
                OstTraceExt1( TRACE_NORMAL, DUP3_OSTPRINTISIMESSAGE, "CDpMif:: ==>   %s", trace_msg );
                trace_msg.SetLength( 0 );
                }
            counter = 0;
            }
        counter++;
        }
    }

#else // OST_TRACE_COMPILER_IN_USE

#define OstPrintIsiMessage(x)

#endif // OST_TRACE_COMPILER_IN_USE



// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CDpMif::NewL
// Static constructor.
// ---------------------------------------------------------
CDpMif* CDpMif::NewL(
    CDpDataPort& aDataPort )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_NEWL, "CDpMif::NewL" );
    LOGM(" CDpMif::NewL");

    CDpMif* self = new ( ELeave ) CDpMif( aDataPort );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CDpMif::ConstructL
// Symbian 2nd phase constructor.
// ---------------------------------------------------------
void CDpMif::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_CONSTRUCTL, "CDpMif::ConstructL" );
    LOGM(" CDpMif::ConstructL");

    iMsgReceiveBuffer = HBufC8::NewL( ISI_HEADER_SIZE + KDpMaxMsgReceiveSize );
    _LIT(KPanicStr, "Dataport - CDpMif::ConstructL");
    __ASSERT_ALWAYS( iMsgReceiveBuffer,
        User::Panic( KPanicStr, KErrNoMemory ) );
    iReceiveMessage.Set( iMsgReceiveBuffer->Des() );
    iReceiveMessage.SetLength( 0 );

    iMessageSendBuffer = HBufC8::NewL( ISI_HEADER_SIZE + KDpMaxMsgSendSize );
    __ASSERT_ALWAYS( iMessageSendBuffer,
        User::Panic( KPanicStr, KErrNoMemory ) );
    iSendMessage.Set( iMessageSendBuffer->Des() );
    iSendMessage.SetLength( 0 );
    }

// ---------------------------------------------------------
// CDpMif::CDpMif
// C++ default constructor.
// ---------------------------------------------------------
CDpMif::CDpMif(
    CDpDataPort& aDataPort ) :
    CActive( KDpMifPriority ),
    iDataPort( aDataPort ),
    iPifDcs( iDataPort.Pif() ),
    iCommPep( iDataPort.CommPep() ),
    iMsgReceiveBuffer( NULL ),
    iReceiveMessage( NULL, 0 ),
    iMessageSendBuffer( NULL ),
    iSendMessage( NULL, 0 )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_CDPMIF, "CDpMif::CDpMif" );
    LOGM(" CDpMif::CDpMif");

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CDpMif::~CDpMif
// Destructor
// ---------------------------------------------------------
CDpMif::~CDpMif()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPMIF_CDPMIF, "CDpMif::~CDpMif" );
    LOGM(" CDpMif::~CDpMif");

    if ( iMsgReceiveBuffer )
        {
        delete iMsgReceiveBuffer;
        }
    //no else
    iMsgReceiveBuffer = NULL;
    iReceiveMessage.Set( NULL, 0, 0 );

    if ( iMessageSendBuffer )
        {
        delete iMessageSendBuffer;
        }
    //no else
    iMessageSendBuffer = NULL;
    iSendMessage.Set( NULL, 0, 0 );
    }

// ---------------------------------------------------------
// CDpMif::RunL
// This method handles received message from Isc Api and
// renews read from Isc Api.
// ---------------------------------------------------------
//
void CDpMif::RunL()
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_RUNL, "CDpMif::RunL" );
    LOGM1("CDpMif::RunL - Port %d", iDataPort.PortUnit() );

    // we are always signaled from PhoNet ReceiveMessage
    if ( KErrNoMemory == iStatus.Int() )
        {
        LOGM(" ==> KErrNoMemory");
        OstTrace0( TRACE_NORMAL, DUP1_CDPMIF_RUNL, "CDpMif:: ==> KErrNoMemory" );

        // deallocate current message
        if ( iMsgReceiveBuffer )
            {
            delete iMsgReceiveBuffer;
            iMsgReceiveBuffer = NULL;
            iReceiveMessage.Set( NULL, 0, 0 );
            }
        //no else

        // ISA Handle receive
        // Trying to allocate a new message
        iMsgReceiveBuffer = HBufC8::NewL( ISI_HEADER_SIZE + iLen );
        _LIT( KPanicStr, "Dataport - CDpMif::RunL" );
        __ASSERT_ALWAYS( iMsgReceiveBuffer,
            User::Panic( KPanicStr, KErrNoMemory ) );
        iReceiveMessage.Set( iMsgReceiveBuffer->Des() );
        iLen = 0;

        if ( !IsActive() )
            {
            iDataPort.ISAHandle().Receive(
                iStatus, iReceiveMessage, iLen );

            SetActive();
            }
        //no else
        }
     else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CDPMIF_RUNL, "CDpMif:: ==> ISI-message received" );
        OstPrintIsiMessage( iReceiveMessage );

        LOGM(" ==> ISI-message received");
        LOGMESSAGE( iReceiveMessage );

        TIsiReceiveC receivedMsg( iReceiveMessage );

        TUint8 resource_id( receivedMsg.Get8bit(
            ISI_HEADER_OFFSET_RESOURCEID ) );
        TUint8 message_id( receivedMsg.Get8bit(
            ISI_HEADER_OFFSET_MESSAGEID ) );

        switch ( resource_id )
            {
            case PN_NAMESERVICE:
                {
                switch ( message_id )
                    {
                    case PNS_NAME_ADD_RESP:
                        {
                        LOG(" ==> PNS_NAME_ADD_RESP received");
                        OstTrace0( TRACE_NORMAL, DUP4_CDPMIF_RUNL, "CDpMif:: ==> PNS_NAME_ADD_RESP received" );

                        TUint8 reason( receivedMsg.Get8bit( ISI_HEADER_SIZE +
                            PNS_NAME_ADD_RESP_OFFSET_REASON ) );
                        TInt error( KErrNone );

                        switch ( reason )
                            {
                            case PN_NAME_OK:
                                {
                                LOG(" ==> PN_NAME_OK");
                                OstTrace0( TRACE_NORMAL, DUP5_CDPMIF_RUNL, "CDpMif:: ==> PN_NAME_OK" );

                                error = KErrNone;
                                break;
                                }
                            default:
                                {
                                LOG1(" ==> reason: %d", reason );
                                OstTraceExt1( TRACE_NORMAL, DUP6_CDPMIF_RUNL, "CDpMif:: ==> reason: %hhu", reason );

                                error = KErrGeneral;
                                }
                            }
                            iDataPort.SignalPif( error );
                        break;
                        }
                    case PNS_NAME_REMOVE_RESP:
                        {
                        LOG(" ==> PNS_NAME_REMOVE_RESP received");
                        OstTrace0( TRACE_NORMAL, DUP7_CDPMIF_RUNL, "CDpMif:: ==> PNS_NAME_REMOVE_RESP received" );
                        break;
                        }
                    default:
                        {
                        LOG1(" ==> Unknown message received. ID: %d",
                            message_id );
                        OstTraceExt1( TRACE_NORMAL, DUP8_CDPMIF_RUNL, "CDpMif:: ==> Unknown message received. ID: %hhu", message_id );
                        }
                    }
                break;
                }
            case PN_PIPE:
                {
                switch ( message_id )
                    {
                    case PNS_PEP_CTRL_REQ:
                        {
                        LOG(" ==> PNS_PEP_CTRL_REQ received");
                        OstTrace0( TRACE_NORMAL, DUP9_CDPMIF_RUNL, "CDpMif:: ==> PNS_PEP_CTRL_REQ received" );

                        HandlePepCtrlReq( receivedMsg );
                        break;
                        }
                    case PNS_PEP_STATUS_IND:
                        {
                        LOG(" ==> PNS_PEP_STATUS_IND received");
                        OstTrace0( TRACE_NORMAL, DUP10_CDPMIF_RUNL, "CDpMif:: ==> PNS_PEP_STATUS_IND received" );

                        HandlePepStatusInd( receivedMsg );
                        break;
                        }
                    default:
                        {
                        LOG1(" ==> Unknown message received. ID: %d",
                            message_id );
                        OstTraceExt1( TRACE_NORMAL, DUP11_CDPMIF_RUNL, "CDpMif:: ==> Unknown message received. ID: %hhu", message_id );
                        }
                    }
                break;
                }
            default:
                {
                LOG1(" ==> Unknown message received. ID: %d", message_id );
                OstTraceExt1( TRACE_NORMAL, DUP12_CDPMIF_RUNL, "CDpMif:: ==> Unknown message received. ID: %hhu", message_id );
                }
            }

        iReceiveMessage.Zero();

        // Check if pipe is removed
        if ( PNS_NAME_REMOVE_RESP == message_id &&
            CDpPif::EDpPipeDisconnected == iPifDcs.PipeState() )
            {
            // Check if DataPort is ready to be deleted
            if ( iDataPort.IsReadyToDestruct() )
                {
                iDataPort.DeleteDataPort();
                }
            //no else
            }
        else
            {
            // Renew read
            User::LeaveIfError( Read() );
            }
        }
    }

// ---------------------------------------------------------
// CDpMif::RunError
// Leave in RunL() is handled here. Error code is returned,
// when internal error has occured.
// ---------------------------------------------------------
//
TInt CDpMif::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_RUNERROR, "CDpMif::RunError" );
    LOGM2("CDpMif::RunError - Port %d, error code: %d",
        iDataPort.PortUnit(), aError );
    OstTraceExt2( TRACE_NORMAL, DUP1_CDPMIF_RUNERROR, "CDpMif:: Port: %u, error code: %d", iDataPort.PortUnit(), aError );

    // Message was not necessarily freed, if leave happened.
    iReceiveMessage.Zero();

    return aError;
    }

// ---------------------------------------------------------
// CDpMif::DoCancel
// This method cancels Isc Api receive.
// ---------------------------------------------------------
//
void CDpMif::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_DOCANCEL, "CDpMif::DoCancel" );
    LOGM(" CDpMif::DoCancel");

    iDataPort.ISAHandle().ReceiveCancel();
    }

// ---------------------------------------------------------
// CDpMif::Read
// Makes a receive request to ISAHandle
// ---------------------------------------------------------
//
TInt CDpMif::Read()
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_READ, "CDpMif::Read" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPMIF_READ, "CDpMif:: Port: %u", iDataPort.PortUnit() );

    LOGM1("CDpMif::Read - Port %d", iDataPort.PortUnit() );

    TInt ret( KErrNone );

    // length of a message ptr indicates we are already reading
    if ( iReceiveMessage.Length() || IsActive() )
        {
        LOG(" CDpMif::Read, Fail - already reading");
        OstTrace0( TRACE_NORMAL, DUP2_CDPMIF_READ, "CDpMif:: Fail - already reading" );

        ret = KErrGeneral;
        }
    else
        {
        // IscApi receive
        iDataPort.ISAHandle().Receive(
            iStatus, iReceiveMessage, iLen );

        SetActive();
         }

    return ret;
    }

// ---------------------------------------------------------
// CDpMif::HandlePepCtrlReq
// Handles PepCtrlReq messages
// ---------------------------------------------------------
//
void CDpMif::HandlePepCtrlReq(
    TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_HANDLEPEPCTRLREQ, "CDpMif::HandlePepCtrlReq" );
    LOGM(" CDpMif::HandlePepCtrlReq");

    // Transaction id
    TUint8 transactionId( aIsiMessage.Get8bit(
        ISI_HEADER_OFFSET_TRANSID ) );

    // Pipe Handle
    if ( PN_PIPE_INVALID_HANDLE == iPifDcs.PipeHandle() )
        {
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PIPEHANDLE) );
        iPifDcs.SetPipeHandle( pipeHandle );
        }

    // PEP Type
    TUint8 pepType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPTYPE ) );
    // PEP Control ID
    TUint8 ctrlId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLID ) );
    // PEP Control Data
    TPtrC8 ctrlData;
    // Check if there is data (there is no data for all control id's)
    if ( ( ISI_HEADER_SIZE + SIZE_PNS_PEP_CTRL_REQ ) <=
         aIsiMessage.GetBuffer().Length() )
        {
        // Control Data length is 1 byte
        TInt ctrlDataLen( 1 );
        ctrlData.Set( aIsiMessage.GetData(
            ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA,
            ctrlDataLen ) );
        }
    // Get sender device
    TUint8 senderDevice( aIsiMessage.Get8bit(
        ISI_HEADER_OFFSET_SENDERDEVICE ) );
    // Get sender object
    TUint8 senderObject( aIsiMessage.Get8bit(
        ISI_HEADER_OFFSET_SENDEROBJECT ) );

    // E32 return value
    TInt return_E32( KErrNone );
    // PN_PIPE return value
    TInt ret( KErrNone );

    switch ( pepType )
        {
        case PN_PEP_TYPE_COMM:
        case PN_PEP_TYPE_BT_ACC:
            {
            // comm_pep handles ctrl_req
            // Something can go wrong in here. Leaving is not the best case in here.
            // We can send an response message with error value to NOS side and
            // hope for the best.
            return_E32 = iCommPep.HandlePepCtrlReq( ctrlId, ctrlData );
            break;
            }

        default: // Not supported pepType
            {
            LOG1("  ERROR - Unsupported pep type: %d", pepType );
            OstTraceExt1( TRACE_NORMAL, DUP1_CDPMIF_HANDLEPEPCTRLREQ, "CDpMif:: ERROR - Unsupported pep type: %hhu", pepType );

            return_E32 = KErrArgument;
            break;
            }
        }

    // do mapping from E32 error codes => PN_PIPE error codes
    switch ( return_E32 )
        {
        case KErrNone:
            {
            ret = PN_PIPE_NO_ERROR;
            break;
            }
        case KErrNotSupported:
            {
            // this error code should NOT be used according to specification
            LOG("  ERROR - Unsupported config (KErrNotSupported)");
            OstTrace0( TRACE_NORMAL, DUP2_CDPMIF_HANDLEPEPCTRLREQ, "CDpMif:: ERROR - Unsupported config (KErrNotSupported)" );

            ret = PN_PIPE_ERR_NOT_SUPPORTED;
            break;
            }
        case KErrArgument:
            {
            // this error code should NOT be used according to specification
            LOG("  ERROR - Invalid argument (KErrArgument)");
            OstTrace0( TRACE_NORMAL, DUP3_CDPMIF_HANDLEPEPCTRLREQ, "CDpMif:: ERROR - Invalid argument (KErrArgument)" );

            ret = PN_PIPE_ERR_INVALID_PARAM;
            break;
            }
        case KErrGeneral:
            {
            LOG("  ERROR - Invalid parameter (KErrGeneral)");
            OstTrace0( TRACE_NORMAL, DUP4_CDPMIF_HANDLEPEPCTRLREQ, "CDpMif:: ERROR - Invalid parameter (KErrGeneral)" );

            ret = PN_PIPE_ERR_INVALID_CTRL_ID;
            break;
            }
        default:
            {
            // this error code should NOT be used according to specification
            LOG1("  ERROR - Other error: %d", return_E32 );
            OstTrace1( TRACE_NORMAL, DUP5_CDPMIF_HANDLEPEPCTRLREQ, "CDpMif:: ERROR - Other error: %d", return_E32 );

            ret = PN_PIPE_ERR_GENERAL;
            break;
            }
        }

    // Create PNS_PEP_CTRL_RESP response
    TBuf8<SIZE_PNS_PEP_CTRL_RESP> messageData;
    // Pipe Handle
    messageData.Append( iPifDcs.PipeHandle() );
    // PEP Control ID
    messageData.Append( ctrlId );
    // PEP Type
    messageData.Append( pepType );
    // Error Code
    messageData.Append( static_cast<TUint8>( ret  ) );
    // Filler bytes
    messageData.Append( KDpPadding );
    messageData.Append( KDpPadding );

    // Send message, respond to sender
    return_E32 = SendMessage(
        senderDevice,
        senderObject,
        PN_PIPE,
        transactionId,
        PNS_PEP_CTRL_RESP,
        messageData );

    LOG1("  <== PNS_PEP_CTRL_RESP resp sent, error code = %d", ret );
    OstTrace1( TRACE_NORMAL, DUP6_CDPMIF_HANDLEPEPCTRLREQ, "CDpMif:: <== PNS_PEP_CTRL_RESP resp sent, error code = %d", ret );

#if defined(_DEBUG)
    if (KErrNone != return_E32 )
        {
        LOG1("  Error isc api send %d", return_E32 );
        OstTrace1( TRACE_NORMAL, DUP7_CDPMIF_HANDLEPEPCTRLREQ, "CDpMif:: Error isc api send %d", return_E32 );
        }
    //no else
#endif

    }

// ---------------------------------------------------------
// CDpMif::HandlePepStatusInd
// Handles PepStatusInd messages
// ---------------------------------------------------------
//
TInt CDpMif::HandlePepStatusInd(
    TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_HANDLEPEPSTATUSIND, "CDpMif::HandlePepStatusInd" );
    LOGM(" CDpMif::HandlePepStatusInd");

    // PEP Type
    TUint8 pepType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ) );
    // Indication ID
    TUint8 indId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_INDICATIONID ) );

    // Indication Data
    TPtrC8 indData;
    TInt ret( KErrNone );

    if ( 15 < aIsiMessage.GetBuffer().Length() )
        {
        // Indication data length is 1 byte
        indData.Set( aIsiMessage.GetData(
            ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_INDICATIONDATA, 1 ) );
        }
    else
        {
        ret = KErrGeneral;
        }

    if ( KErrNone == ret )
        {
        TInt result( KErrNone );
        switch ( pepType )
            {
            case PN_PEP_TYPE_COMM:
            case PN_PEP_TYPE_BT_ACC:
                {
                // comm_pep handles status_ind.
                // If dataport doesn't understand the status msg
                // it can't do nothing about it.
                result = iCommPep.HandlePepStatusInd( indId, indData );
                break;
                }
            case PN_PEP_TYPE_COMMON:
                {
                // DO NOTHING
                // This is handled in ISC
                break;
                }
            default:
                {
                // Dataport doesn't support the PNS_PEP_STATUS pep type
                // Life's hard, can't do nothing. Don't need to leave
                // in release code.
                LOG1(" ERROR - Unsupported pep type: %d", result );
                OstTrace1( TRACE_NORMAL, DUP1_CDPMIF_HANDLEPEPSTATUSIND, "CDpMif:: ERROR - Unsupported pep type: %d", result );

                ret = KErrGeneral;
                }
            }

        if ( KErrNone == result )
            {
            LOG1(" CDpMif::HandlePepStatusInd: %d", result );
            OstTrace1( TRACE_NORMAL, DUP2_CDPMIF_HANDLEPEPSTATUSIND, "CDpMif:: HandlePepStatusInd: %d", result );
            }
        //no else
        }
    //no else

    return ret;
    }

// ---------------------------------------------------------
// CDpMif::SendNameAddReqL
// This method sends name add request to Name Service.
// ---------------------------------------------------------
//
void CDpMif::SendNameAddReqL(
    const TUint aUnit )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_SENDNAMEADDREQL, "CDpMif::SendNameAddReqL" );
    LOGM(" CDpMif::SendNameAddReq");

    __ASSERT_ALWAYS( aUnit <= KDataPortCommHighUnit,
        User::Leave( KErrNotSupported ) );

    TBuf8<SIZE_PNS_NAME_ADD_REQ> messageData;

    // Add reserved bytes
    messageData.Append( KDpPadding );
    messageData.Append( KDpPadding );

    // Add 32bit name (Big-endian)
    switch ( aUnit )
        {
        case KDpCsdPort:
        case KDpPort6:
            {
            messageData.Append( aUnit );                //name
            messageData.Append( PN_PEP_SUB_TYPE_SYMBIAN_INTERNAL ); //name
            messageData.Append( PN_PEP_TYPE_COMM );     //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        case KDpDialUpPort:
        case KDpPort4:
        case KDpPort5:
            {
            messageData.Append( aUnit );                //name
            messageData.Append( KDpPadding );           //name
            messageData.Append( PN_PEP_TYPE_COMM );     //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        case KDpBtAccHfPort:
            {
            messageData.Append( KDpPadding );           //name
            messageData.Append( PN_PEP_SUB_TYPE_BT_ACC_HF ); //name
            messageData.Append( PN_PEP_TYPE_BT_ACC );   //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        case KDpBtAccHsPort:
            {
            messageData.Append( KDpPadding );           //name
            messageData.Append( PN_PEP_SUB_TYPE_BT_ACC_HS ); //name
            messageData.Append( PN_PEP_TYPE_BT_ACC );   //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        default:
            {
            User::Leave( KErrGeneral );
            }
        }

    // Add Device id
    messageData.Append( THIS_DEVICE );

    // Create Object id
    TUint8 channelId( 0 );

    switch ( aUnit )
        {
        case KDpCsdPort:
            {
            channelId = EIscNokiaDataport1;
            break;
            }
        case KDpDialUpPort:
            {
            channelId = EIscNokiaDataport2;
            break;
            }
        case KDpBtAccHfPort:
        case KDpBtAccHsPort:
            {
            channelId = EIscNokiaBtAppl;
            break;
            }
        case KDpPort4:
            {
            channelId = EIscNokiaDataport3;
            break;
            }
        case KDpPort5:
            {
            channelId = EIscNokiaDataport4;
            break;
            }
        case KDpPort6:
            {
            channelId = EIscNokiaDataport5;
            break;
            }
        default:
            {
            //Do nothing
            break;
            }
        }

    TBuf8<2> data;
    TInt ret( iDataPort.ISAHandle().GetChannelInfo( channelId, data ) );

    _LIT(KPanicStr, "Dataport::SendNameAddReqL");
    __ASSERT_ALWAYS( KErrNone == ret, User::Panic( KPanicStr, 0 ) );

    // Set obj id
    TUint16 obj( static_cast<TUint16>(
        ( static_cast<TUint16>( data[0] ) << 8 ) + data[1] ) );

    LOG1(" Dataport::SendNameAddReqL obj = 0x%x", obj );
    OstTraceExt1( TRACE_NORMAL, DUP1_CDPMIF_SENDNAMEADDREQL, "CDpMif:: obj = 0x%hx", obj );

    // Add Object id
    messageData.Append( obj );
    // Add Record flags
    messageData.Append( PN_NAME_CLEARED );
    //fill
    messageData.Append( KDpPadding );

#if defined(_DEBUG)
    ret =
#endif
    SendMessage(
        PN_NAMESERVICE,
        iDataPort.CreateTransactionId(),
        PNS_NAME_ADD_REQ,
        messageData );

    LOG1("  <== PNS_NAME_ADD_REQ req sent, obj id: %x", obj );
    OstTraceExt1( TRACE_NORMAL, DUP2_CDPMIF_SENDNAMEADDREQL, "CDpMif:: <== PNS_NAME_ADD_REQ req sent, obj id: %hx", obj );

#if defined(_DEBUG)
    if (KErrNone != ret )
        {
        LOG1("  Error isc api send %d", ret );
        OstTrace1( TRACE_NORMAL, DUP3_CDPMIF_SENDNAMEADDREQL, "CDpMif:: Error isc api send %d", ret );
        }
    //no else
#endif
    }

// ---------------------------------------------------------
// CDpMif::SendNameRemoveReqL
// This method sends name remove request to Name Service.
// ---------------------------------------------------------
//
void CDpMif::SendNameRemoveReqL(
    const TUint aUnit )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_SENDNAMEREMOVEREQL, "CDpMif::SendNameRemoveReqL" );
    LOGM(" CDpMif::SendNameRemoveReq");

    TBuf8<SIZE_PNS_NAME_REMOVE_REQ> messageData;

    // Add reserved bytes
    messageData.Append( KDpPadding );
    messageData.Append( KDpPadding );

    // Add 32bit name (Big-endian)
    switch ( aUnit )
        {
        case KDpCsdPort:
        case KDpPort6:
            {
            messageData.Append( aUnit );                //name
            messageData.Append( PN_PEP_SUB_TYPE_SYMBIAN_INTERNAL ); //name
            messageData.Append( PN_PEP_TYPE_COMM );     //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        case KDpDialUpPort:
        case KDpPort4:
        case KDpPort5:
            {
            messageData.Append( aUnit );                //name
            messageData.Append( KDpPadding );           //name
            messageData.Append( PN_PEP_TYPE_COMM );     //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        case KDpBtAccHfPort:
            {
            messageData.Append( KDpPadding );           //name
            messageData.Append( PN_PEP_SUB_TYPE_BT_ACC_HF ); //name
            messageData.Append( PN_PEP_TYPE_BT_ACC );   //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        case KDpBtAccHsPort:
            {
            messageData.Append( KDpPadding );           //name
            messageData.Append( PN_PEP_SUB_TYPE_BT_ACC_HS ); //name
            messageData.Append( PN_PEP_TYPE_BT_ACC );   //name
            messageData.Append( PN_PIPE );              //name
            break;
            }
        default:
            {
            User::Leave( KErrGeneral );
            }
        }

#if defined(_DEBUG)
    TInt ret =
#endif
    SendMessage(
        PN_NAMESERVICE,
        iDataPort.CreateTransactionId(),
        PNS_NAME_REMOVE_REQ,
        messageData );

    LOG("  <== PNS_NAME_REMOVE_REQ req sent");
    OstTrace0( TRACE_NORMAL, DUP1_CDPMIF_SENDNAMEREMOVEREQL, "CDpMif:: <== PNS_NAME_REMOVE_REQ req sent" );

#if defined(_DEBUG)
    if ( KErrNone != ret )
        {
        LOG1("Error isc api send %d", ret );
        OstTrace1( TRACE_NORMAL, DUP2_CDPMIF_SENDNAMEREMOVEREQL, "CDpMif:: Error isc api send %d", ret );
        }
    //no else
#endif
    }

// ---------------------------------------------------------
// CDpMif::SendMessage
// Send ISI message, given resource, transaction id,
// message id and message.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CDpMif::SendMessage(
    TUint8 aResource,
    TUint8 aTransactionId,
    TUint8 aMessageId,
    const TDesC8& aMessage )
    {
    OstTrace0( TRACE_NORMAL, CDPMIF_SENDMESSAGE, "CDpMif::SendMessage" );
    OstTrace1( TRACE_NORMAL, DUP2_CDPMIF_SENDMESSAGE, "CDpMif:: Port: %u", iDataPort.PortUnit() );
    OstTraceExt1( TRACE_NORMAL, DUP3_CDPMIF_SENDMESSAGE, "CDpMif:: ==> resource: 0x%hhx", aResource );
    OstTraceExt1( TRACE_NORMAL, DUP4_CDPMIF_SENDMESSAGE, "CDpMif:: ==> tra id: 0x%hhx", aTransactionId );
    OstTraceExt1( TRACE_NORMAL, DUP5_CDPMIF_SENDMESSAGE, "CDpMif:: ==> msg id: 0x%hhx", aMessageId );

    LOGM1(" CDpMif::SendMessage - Port %d", iDataPort.PortUnit() );
    LOGM1("  ==> resource: 0x%x", aResource );
    LOGM1("  ==> tra id: 0x%x", aTransactionId );
    LOGM1("  ==> msg id: 0x%x", aMessageId );

    // We shall use send buffer
    iSendMessage.Zero();
    TIsiSend msg( iSendMessage );
    msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, aResource );
    msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, aTransactionId );
    msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, aMessageId );
    // Copy message data (+2 => after msgid and traid)
    msg.CopyData( ISI_HEADER_SIZE + 2, aMessage );

    // Add paddings
    while ( iSendMessage.Length() % 4 )
        {
        iSendMessage.Append( 0x00 );
        }
    OstTrace1( TRACE_NORMAL, DUP6_CDPMIF_SENDMESSAGE, "CDpMif:: ==> message length: %d", iSendMessage.Length() );
    OstPrintIsiMessage( iSendMessage );

    LOGM1("  ==> message length: %d", iSendMessage.Length() );
    LOGMESSAGE( iSendMessage );

    return iDataPort.ISAHandle().Send( msg.Complete() );
    }

// ---------------------------------------------------------
// CDpMif::SendMessage
// Send ISI message, given receiver device, receiver object,
// resource, transaction id, message id and message.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CDpMif::SendMessage(
    TUint8 aRecDevice,
    TUint8 aRecObject,
    TUint8 aResource,
    TUint8 aTransactionId,
    TUint8 aMessageId,
    const TDesC8& aMessage )
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPMIF_SENDMESSAGE, "CDpMif::SendMessage" );
    OstTrace1( TRACE_NORMAL, DUP15_CDPMIF_SENDMESSAGE, "CDpMif:: Port: %u", iDataPort.PortUnit() );
    OstTraceExt1( TRACE_NORMAL, DUP14_CDPMIF_SENDMESSAGE, "CDpMif:: ==> receiver dev.: 0x%hhx", aRecDevice );
    OstTraceExt1( TRACE_NORMAL, DUP13_CDPMIF_SENDMESSAGE, "CDpMif:: ==> receiver obj.: 0x%hhx", aRecObject );
    OstTraceExt1( TRACE_NORMAL, DUP12_CDPMIF_SENDMESSAGE, "CDpMif:: ==> resource: 0x%hhx", aResource );
    OstTraceExt1( TRACE_NORMAL, DUP11_CDPMIF_SENDMESSAGE, "CDpMif:: ==> tra id: 0x%hhx", aTransactionId );
    OstTraceExt1( TRACE_NORMAL, DUP10_CDPMIF_SENDMESSAGE, "CDpMif:: ==> msg id: 0x%hhx", aMessageId );

    LOGM1(" CDpMif::SendMessage - Port %d", iDataPort.PortUnit() );
    LOGM1("  ==> receiver dev.: 0x%x", aRecDevice );
    LOGM1("  ==> receiver obj.: 0x%x", aRecObject );
    LOGM1("  ==> resource: 0x%x", aResource );
    LOGM1("  ==> tra id: 0x%x", aTransactionId );
    LOGM1("  ==> msg id: 0x%x", aMessageId );

    // We shall use send buffer
    iSendMessage.Zero();
    TIsiSend msg( iSendMessage );
    msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, aRecDevice );
    msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, aResource );
    msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, aRecObject );
    msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, aTransactionId );
    msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, aMessageId );
    // Copy message data (+2 => after msgid and traid)
    msg.CopyData( ISI_HEADER_SIZE + 2, aMessage );

    // Add paddings
    while ( iSendMessage.Length() % 4 )
        {
        iSendMessage.Append( 0x00 );
        }

    OstTrace1( TRACE_NORMAL, DUP8_CDPMIF_SENDMESSAGE, "CDpMif:: ==> message length: %d", iSendMessage.Length() );
    OstPrintIsiMessage( iSendMessage );

    LOGM1("  ==> message length: %d", iSendMessage.Length() );
    LOGMESSAGE( iSendMessage );

    return iDataPort.ISAHandle().Send( msg.Complete() );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
// none

//  End of File
