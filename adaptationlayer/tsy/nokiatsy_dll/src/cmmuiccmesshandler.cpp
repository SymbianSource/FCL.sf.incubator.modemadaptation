/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



// INCLUDE FILES
#include "cmmuiccmesshandler.h"
#include "cmmphonetsender.h"
#include "tsylogger.h"
#include "cmmmessagerouter.h"

#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <e32cmn.h>
#include <pn_const.h>
#include <uiccisi.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmuiccmesshandlerTraces.h"
#endif


// LOCAL CONSTANTS AND MACROS
const TUint8 KUiccPadding( 0 );
const TUint8 KUiccSbApplPathSize( SIZE_UICC_SB_APPL_PATH + KFilePathLength );
const TUint8 KUiccSbFileDataSize( SIZE_UICC_SB_FILE_DATA + KFileDataLength );
const TUint8 KUiccApplCmdReqOffset( ISI_HEADER_SIZE + SIZE_UICC_APPL_CMD_REQ );
const TUint8 KUiccRecordNotDefined( 0 );
const TUint16 KUiccFileIdNotDefined( 0 );
const TUint8 KUiccServiceTypeNotDefined( 0 );
const TUint8 KUiccApduReqSubblockOffset(ISI_HEADER_SIZE + SIZE_UICC_APDU_REQ);
const TUint8 KUiccSbApduLengthOffset( 6 );
const TUint8 KUiccSbApduDataOffset( 8 );
const TUint16 KUiccSbApduSize( SIZE_UICC_SB_APDU + KApduDataLength );


// ============================= LOCAL FUNCTIONS ===============================

void CMmUiccMessHandler::SendTerminalProfile()
    {
    // Temporary for testing purposes, terminal profile sending will be moved
    // to SAT TSY
TFLOGSTRING("TSY: CMmUiccMessHandler::SendTerminalProfile TEMPORARY SOLUTION FOR TESTING");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_SENDTERMINALPROFILE, "CMmUiccMessHandler::SendTerminalProfile TEMPORARY SOLUTION FOR TESTING" );

    // Create UICC_CAT_TERMINAL_PROFILE message
    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit( ISI_HEADER_SIZE, UICC_CAT_REQ_OFFSET_TRANSID ); // transaction id
    isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_MESSAGEID, UICC_CAT_REQ );
    isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_SERVICETYPE, UICC_CAT_TERMINAL_PROFILE );
    isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_CAT_REQ_OFFSET_NSB, 1 ); // num of subblocks

    // Create subblock
    TBuf8<30> terminalProfileBuf( 0 );
    TIsiSubBlock uiccSbTerminalProfile(
        terminalProfileBuf,
        UICC_SB_TERMINAL_PROFILE,
        EIsiSubBlockTypeId16Len16 );

    // 2x filler
    terminalProfileBuf.Append( KUiccPadding );
    terminalProfileBuf.Append( KUiccPadding );

    // Terminal profile length (16-bit)
    terminalProfileBuf.Append( KUiccPadding );
    terminalProfileBuf.Append( 1 );
    terminalProfileBuf.Append( 0 );

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + 4,
        uiccSbTerminalProfile.CompleteSubBlock() );

    iPhoNetSender->Send( isiMsg.Complete() );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TUiccParamsBase::TUiccParamsBase
// Constructor
// -----------------------------------------------------------------------------
//
TUiccParamsBase::TUiccParamsBase()
    {
    // Initialize data
    messHandlerPtr = NULL;
    trId = ETrIdNotDefined;
    fileId = KUiccFileIdNotDefined;
    fileIdSfi = UICC_SFI_NOT_PRESENT;
    serviceType = KUiccServiceTypeNotDefined;
    filePath.Append( KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// TUiccApplFileInfo::TUiccApplFileInfo
// Constructor
// -----------------------------------------------------------------------------
//
TUiccApplFileInfo::TUiccApplFileInfo()
    {
    // None
    }

// -----------------------------------------------------------------------------
// TUiccReadTransparent::TUiccReadTransparent
// Constructor
// -----------------------------------------------------------------------------
//
TUiccReadTransparent::TUiccReadTransparent()
    {
    dataOffset = 0;
    dataAmount = 0;
    }

// -----------------------------------------------------------------------------
// TUiccWriteTransparent::TUiccWriteTransparent
// Constructor
// -----------------------------------------------------------------------------
//
TUiccWriteTransparent::TUiccWriteTransparent()
    {
    fileData.Append( KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// TUiccReadLinearFixed::TUiccReadLinearFixed
// Constructor
// -----------------------------------------------------------------------------
//
TUiccReadLinearFixed::TUiccReadLinearFixed()
    {
    record = KUiccRecordNotDefined;
    }

// -----------------------------------------------------------------------------
// TUiccWriteLinearFixed::TUiccWriteLinearFixed
// Constructor
// -----------------------------------------------------------------------------
//
TUiccWriteLinearFixed::TUiccWriteLinearFixed()
    {
    fileData.Append( KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// TUiccSendApdu::TUiccSendApdu
// Constructor
// -----------------------------------------------------------------------------
//
TUiccSendApdu::TUiccSendApdu():TUiccParamsBase()
    {
    apdu.Append( KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// TUiccParamsApduReq::TUiccParamsApduReq
// Constructor
// -----------------------------------------------------------------------------
//
TUiccParamsApduReq::TUiccParamsApduReq():TUiccParamsBase()
    {
    action = KUiccRecordNotDefined;
    apduData.Append( KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CMmUiccMessHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmUiccMessHandler::CMmUiccMessHandler()
    {
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::ConstructL(
    CMmPhoNetSender* aPhoNetSender,
    CMmPhoNetReceiver* aPhoNetReceiver)
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CONSTRUCTL, "CMmUiccMessHandler::ConstructL" );

    // Reset the pointer array
    iMessHandlerPrtList.Reset();
    iPhoNetSender = aPhoNetSender;
    aPhoNetReceiver->RegisterL( this, PN_UICC );

    iCardType = UICC_CARD_TYPE_UNKNOWN;
    iApplicationId = UICC_APPL_ID_UNKNOWN;
    iApplicationType = UICC_APPL_TYPE_UNKNOWN;
    iApplicationStatus = UICC_STATUS_APPL_NOT_ACTIVE;
    iUiccServerStatus = UICC_STATUS_NOT_READY;
    iUiccClientId = 0;
    iUiccIsimClientId = 0;
    iIsimApplicationId = UICC_APPL_ID_UNKNOWN;
    iIsimApplicationStatus = UICC_STATUS_APPL_NOT_ACTIVE;
    iIsimApplicationFound = EFalse;
    iPin1Id = 0;
    iPin2Id = 0;
    iActivePin = RMobilePhone::ESecurityCodePin1;
    iAid.Zero();

    // Initialized for USIM application
    iApplFileId.Append( 0x7F );
    iApplFileId.Append( 0xFF );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmUiccMessHandler* CMmUiccMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender,
    CMmPhoNetReceiver* aPhoNetReceiver,
    CMmMessageRouter* aMessageRouter )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::NewL");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_NEWL, "CMmUiccMessHandler::NewL" );

    CMmUiccMessHandler* uiccMessHandler( new ( ELeave ) CMmUiccMessHandler() );
    CleanupStack::PushL( uiccMessHandler );
    uiccMessHandler->iMessageRouter = aMessageRouter;
    uiccMessHandler->ConstructL( aPhoNetSender, aPhoNetReceiver );
    CleanupStack::Pop( uiccMessHandler );
    return uiccMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::~CMmUiccMessHandler()
// Destructor
// -----------------------------------------------------------------------------
//
CMmUiccMessHandler::~CMmUiccMessHandler()
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::~CMmUiccMessHandler");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CMMUICCMESSHANDLER, "CMmUiccMessHandler::~CMmUiccMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::ReceiveMessageL
// Receive ISI messages from phonet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::ReceiveMessageL( const TIsiReceiveC& aIsiMsg )
    {
    TInt status( KErrNone );
    TUint8 messageId( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );
    TUint8 trId( aIsiMsg.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    TUint8 serviceType( 0 );
    TPtrC8 fileData;

TFLOGSTRING3("TSY: CMmUiccMessHandler::ReceiveMessageL, message ID :0x%x, transaction ID: %d", messageId, trId );
OstTraceExt2( TRACE_NORMAL, CMMUICCMESSHANDLER_RECEIVEMESSAGEL, "CMmUiccMessHandler::ReceiveMessageL;messageId=%hhx;trId=%hhu", messageId, trId );

    // Get the correct message handler for this operation
    MUiccOperationBase* messHandler( iMessHandlerPrtList.At( trId ) );

    switch( messageId )
        {
        case UICC_CAT_IND:
            {
            // Send terminal profile ( to be moved to SAT TSY later )
            SendTerminalProfile();
            break;
            }
        case UICC_CAT_RESP:
            {
            // To be moved to SAT TSY later
            break;
            }
        case UICC_CARD_IND:
            {
            serviceType = aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_CARD_IND_OFFSET_SERVICETYPE );
            // Start application activation procedure if the card is ready and
            // the application is not active yet
            if ( UICC_CARD_READY == serviceType &&
                UICC_STATUS_APPL_ACTIVE != iApplicationStatus )
                {
                TUiccParamsBase params;
                params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
                params.trId = ETrIdGetUiccApplication;
                CreateUiccApplicationReq( params, UICC_APPL_LIST );
                }
            // No else. Application was active already
            break;
            }
        case UICC_RESP:
            {
            iUiccServerStatus = HandleUiccResp( aIsiMsg );
            // Start application activation procedure if the card is ready and
            // the application is not active yet
            if ( UICC_STATUS_START_UP_COMPLETED == iUiccServerStatus &&
                UICC_STATUS_APPL_ACTIVE != iApplicationStatus)
                {
                // Read the application ID
                TUiccParamsBase params;
                params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
                params.trId = ETrIdGetUiccApplication;
                CreateUiccApplicationReq( params, UICC_APPL_LIST );
                }
            break;
            }
        case UICC_APPLICATION_RESP:
            {
            status = HandleUiccApplicationResp( aIsiMsg );
            break;
            }
        case UICC_APPLICATION_IND:
            {
            TInt8 serviceType( aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_APPLICATION_IND_OFFSET_SERVICETYPE ) );

            iApplicationStatus = UICC_STATUS_APPL_ACTIVE;
            // Application activation indication with service type
            // UICC_APPL_ACTIVATED, as part of PowerOnSim sequence.
            // Transaction id is set to ETrIdSimPowerOn and operation
            // is added to list and flag is disabled.
            if( UICC_APPL_ACTIVATED == serviceType )
                {
                iDoNotRemoveTransactionID = EFalse;
                status = KErrNone;
                trId = ETrIdSimPowerOn;
                messHandler = iMessHandlerPrtList.At( trId );
                }
            break;
            }
        case UICC_APPL_CMD_RESP:
            {
            status = aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_APPL_CMD_RESP_OFFSET_STATUS );
            TInt8 serviceType( aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_APPL_CMD_RESP_OFFSET_SERVICETYPE ) );

            if ( UICC_STATUS_OK == status )
                {
                // If READ operation was required, read data from subblock
                if ( UICC_APPL_READ_TRANSPARENT == serviceType ||
                   UICC_APPL_READ_LINEAR_FIXED == serviceType )
                    {
                    fileData.Set( GetFileData( aIsiMsg ) );
                    }
                if ( UICC_APPL_FILE_INFO == serviceType )
                    {
                    fileData.Set( GetFileFCI( aIsiMsg ) );
                    }
                if ( UICC_APPL_APDU_SEND == serviceType )
                    {
                    fileData.Set( GetApduData( aIsiMsg ) );
                    }
                }
            else
                {
TFLOGSTRING2("TSY: CMmUiccMessHandler::ReceiveMessageL, case UICC_APPL_CMD_RESP, status: %d", status );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_RECEIVEMESSAGEL, "CMmUiccMessHandler::ReceiveMessageL, case UICC_APPL_CMD_RESP;status=%hhu", status );
                }
            break;
            }
        case UICC_APDU_RESP:
            {
            status = aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_APDU_RESP_OFFSET_STATUS );
            TInt8 serviceType( aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_APDU_RESP_OFFSET_SERVICETYPE ) );

            // Extract actual APDU from SB_APDU
            if( UICC_STATUS_OK == status )
                {
                if( UICC_APDU_ATR_GET == serviceType
                    || UICC_APDU_SEND == serviceType )
                    {
                    fileData.Set( GetApduData( aIsiMsg ) );
                    }
                }
            break;
            }
        case UICC_APDU_RESET_IND:
            {
            TInt8 serviceType( aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_APDU_RESET_IND_OFFSET_SERVICETYPE ) );

            // Indication that SAP APDU interface is activated
            if( UICC_READY == serviceType )
                {
                // Disable flag
                iDoNotRemoveTransactionID = EFalse;
                status = KErrNone;
                }
            break;
            }
        case UICC_CONNECTOR_RESP:
            {
            status = aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_CONNECTOR_RESP_OFFSET_STATUS );
            }
            break;
        case UICC_CARD_RESP:
            {
            status = aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_CARD_RESP_OFFSET_STATUS );
            TUint8 serviceType( aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_CARD_RESP_OFFSET_SERVICETYPE ) );

            if( UICC_STATUS_OK == status
                && UICC_CARD_STATUS_GET  == serviceType )
                {
                // File data from SIZE_UICC_SB_CARD_STATUS
                TUint uiccSbFileDataOffset( 0 );
                TInt cardStatusFieldLength( 1 );

                if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_UICC_CARD_RESP,
                    UICC_SB_CARD_STATUS,
                    EIsiSubBlockTypeId16Len16,
                    uiccSbFileDataOffset ) )
                    {
                    fileData.Set(aIsiMsg.GetData(
                        uiccSbFileDataOffset +
                        UICC_SB_CARD_STATUS_OFFSET_CARDSTATUS,
                        cardStatusFieldLength) );
                    }
                }
            }
            break;
        default:
            {
TFLOGSTRING("TSY: CMmUiccMessHandler::ReceiveMessageL, default" );
OstTrace0( TRACE_NORMAL, DUP2_CMMUICCMESSHANDLER_RECEIVEMESSAGEL, "CMmUiccMessHandler::ReceiveMessageL, default" );
            break;
            }
        }

    if ( NULL != messHandler )
        {
        if( !iDoNotRemoveTransactionID )
            {
            iMessHandlerPrtList[trId] = NULL;
            }
        messHandler->ProcessUiccMsg( trId, status, fileData );
        }
    else
        {
TFLOGSTRING("TSY: CMmUiccMessHandler::ReceiveMessageL, message handler not found -> no further handling of this message" );
OstTrace0( TRACE_NORMAL, DUP3_CMMUICCMESSHANDLER_RECEIVEMESSAGEL, "CMmUiccMessHandler::ReceiveMessageL, message handler not found -> no further handling of this message" );
        }
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccApplCmdReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmUiccMessHandler::CreateUiccApplCmdReq(
    const TUiccParamsBase& aParams,
    const TUint8 aApplType )
    {
    TInt ret( KErrNone );
    TInt trId( aParams.trId );
    TInt8 serviceType( aParams.serviceType );
    TUint8 numOfSubblocks( 0 );
    TUint8 sizeOfApplPathSubblock( 0 );

TFLOGSTRING2("TSY: CMmUiccMessHandler::CreateUiccApplCmdReq, transaction ID: %d", trId );
OstTrace1( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCAPPLCMDREQ, "CMmUiccMessHandler::CreateUiccApplCmdReq;trId=%d", trId );

    TUint8 applId( aApplType == UICC_APPL_TYPE_UICC_ISIM ?
        iIsimApplicationId : iApplicationId );

    // If there is the previous request ongoing (pointer was already set for
    // this operation)  we can't handle this request.
    if ( NULL == iMessHandlerPrtList.At( trId ) )
        {
        // Save pointer to message handler, it is needed when receiving
        // respone for this operation
        iMessHandlerPrtList[trId] = aParams.messHandlerPtr;
        // Create UICC_APPL_CMD_REQ message
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_TRANSID,
            trId );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_MESSAGEID,
            UICC_APPL_CMD_REQ );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SERVICETYPE,
            serviceType );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_APPLID,
            applId );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_SESSIONID,
            UICC_SESSION_ID_NOT_USED );
        isiMsg.Set16bit(
            ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_FILLERBYTE1,
            KUiccPadding );

        switch ( serviceType )
            {
            case UICC_APPL_READ_TRANSPARENT:
                {
                numOfSubblocks = 3;
                // Create and append UICC_SB_CLIENT
                CreateUiccSbClient( isiMsg, KUiccApplCmdReqOffset );
                // Create and append UICC_SB_TRANSPARENT
                CreateUiccSbTransparent(
                    static_cast <const TUiccReadTransparent&>( aParams ),
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT );
                // Create and append UICC_SB_APPL_PATH
                CreateUiccSbApplPath(
                    aParams,
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
                    SIZE_UICC_SB_TRANSPARENT,
                    sizeOfApplPathSubblock );
                break;
                }
            case UICC_APPL_UPDATE_TRANSPARENT:
                {
                numOfSubblocks = 4;
                // Create and append UICC_SB_CLIENT
                CreateUiccSbClient( isiMsg, KUiccApplCmdReqOffset );
                // Create and append UICC_SB_TRANSPARENT
                CreateUiccSbTransparent(
                    static_cast <const TUiccWriteTransparent&>( aParams ),
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT );
                // Create and append UICC_SB_APPL_PATH
                CreateUiccSbApplPath(
                    static_cast <const TUiccWriteTransparent&>( aParams ),
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
                    SIZE_UICC_SB_TRANSPARENT,
                    sizeOfApplPathSubblock );
                // Create and append UICC_SB_FILE_DATA
                CreateUiccSbFileData(
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
                    SIZE_UICC_SB_TRANSPARENT + sizeOfApplPathSubblock,
                    static_cast
                    <const TUiccWriteTransparent&>( aParams ).fileData );
                break;
                }
            case UICC_APPL_READ_LINEAR_FIXED:
                {
                numOfSubblocks = 3;
                // Create and append UICC_SB_CLIENT
                CreateUiccSbClient( isiMsg, KUiccApplCmdReqOffset );
                // Create and append UICC_SB_LINEAR_FIXED
                CreateUiccSbLinearFixed(
                    static_cast <const TUiccReadLinearFixed&>( aParams ),
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT );
                // Create and append UICC_SB_APPL_PATH
                CreateUiccSbApplPath(
                    static_cast <const TUiccReadLinearFixed&>( aParams ),
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
                    SIZE_UICC_SB_LINEAR_FIXED,
                    sizeOfApplPathSubblock );
                break;
                }
            case UICC_APPL_UPDATE_LINEAR_FIXED:
                {
                // UICC_SB_LINEAR_FIXED, UICC_SB_APPL_PATH, UICC_SB_FILE_DATA
                numOfSubblocks = 4;
                // Create and append UICC_SB_CLIENT
                CreateUiccSbClient( isiMsg, KUiccApplCmdReqOffset );
                // Create and append UICC_SB_LINEAR_FIXED
                CreateUiccSbLinearFixed(
                    static_cast <const TUiccWriteLinearFixed&>( aParams ),
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT );
                // Create and append UICC_SB_APPL_PATH
                CreateUiccSbApplPath(
                    static_cast <const TUiccWriteLinearFixed&>( aParams ),
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
                    SIZE_UICC_SB_LINEAR_FIXED,
                    sizeOfApplPathSubblock );
                // Create and append UICC_SB_FILE_DATA
                CreateUiccSbFileData(
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
                    SIZE_UICC_SB_TRANSPARENT + sizeOfApplPathSubblock,
                    static_cast
                        <const TUiccWriteLinearFixed&>( aParams ).fileData);
                break;
                }
            case UICC_APPL_FILE_INFO:
                {
                numOfSubblocks = 1;
                // Create and append UICC_SB_APPL_PATH
                CreateUiccSbApplPath(
                    aParams,
                    isiMsg,
                    KUiccApplCmdReqOffset,
                    sizeOfApplPathSubblock );
                break;
                }
            case UICC_APPL_APDU_SEND:
                {
                numOfSubblocks = 3;
                // Create and append UICC_SB_CLIENT
                CreateUiccSbClient( isiMsg, KUiccApplCmdReqOffset, aApplType );
                // Create and append UICC_SB_APPL_PATH
                CreateUiccSbApplPath(
                    aParams,
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT,
                    sizeOfApplPathSubblock );
                // Create and append UICC_SB_APDU
                const TUiccSendApdu* tmpPtr = static_cast<const TUiccSendApdu*>( &aParams );
                CreateUiccSbApdu(
                    isiMsg,
                    KUiccApplCmdReqOffset + SIZE_UICC_SB_CLIENT +
                    sizeOfApplPathSubblock,
                    tmpPtr->apdu );
                break;
                }
            default:
                {
                ret = KErrArgument;
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccApplCmdReq, unknown service type!" );
OstTrace0( TRACE_NORMAL, DUP2_CMMUICCMESSHANDLER_CREATEUICCAPPLCMDREQ, "CMmUiccMessHandler::CreateUiccApplCmdReq, unknown service type!" );
                break;
                }
            } // End of switch ( serviceType )

        if ( KErrNone == ret )
            {
            // Set number of subblocks
            isiMsg.Set8bit(
                ISI_HEADER_SIZE + UICC_APPL_CMD_REQ_OFFSET_NSB,
                numOfSubblocks );
            return( iPhoNetSender->Send( isiMsg.Complete() ) );
            }
        } // End of if ( NULL == iMessHandlerPrtList.At( trId ) )
    else
        {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccApplCmdReq, cannot handle the request, server busy!" );
OstTrace0( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_CREATEUICCAPPLCMDREQ, "CMmUiccMessHandler::CreateUiccApplCmdReq, cannot handle the request, server busy!" );
        ret = KErrServerBusy;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccApplicationReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmUiccMessHandler::CreateUiccApplicationReq(
    const TUiccParamsBase& aParams,
    TUint8 aServiceType,
    TUint8 aApplType )
    {
TFLOGSTRING2("TSY: CMmUiccMessHandler::CreateUiccApplicationReq, service type: %d", aServiceType );
OstTraceExt1( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCAPPLICATIONREQ, "CMmUiccMessHandler::CreateUiccApplicationReq;aServiceType=%hhu", aServiceType );

    TInt ret (KErrGeneral);
    TUiccTrId trId( aParams.trId );
    TUint8 numOfSubblocks( 0 );

    // If there is the previous request ongoing (pointer was already set for
    // this operation)  we can't handle this request.
    if ( NULL == iMessHandlerPrtList.At( trId ) )
        {
        // Save pointer to message handler, it is needed when receiving
        // response for this operation
        iMessHandlerPrtList[trId] = aParams.messHandlerPtr;

        // Create the ISI message
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
        isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_APPLICATION_REQ_OFFSET_MESSAGEID,
            UICC_APPLICATION_REQ );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPLICATION_REQ_OFFSET_SERVICETYPE,
            aServiceType );

        switch ( aServiceType )
            {
            case UICC_APPL_LIST:
                {
                // No subblocks in this case
                break;
                }
            case UICC_APPL_HOST_ACTIVATE:
                {
                numOfSubblocks = 1;
                CreateUiccSbApplication(
                    isiMsg,
                    ISI_HEADER_SIZE + SIZE_UICC_APPLICATION_REQ,
                    aApplType );
                break;
                }
            default:
                {
                break;
                }
            }

        // Set transaction ID and number of subblocks to ISI message
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPLICATION_REQ_OFFSET_TRANSID,
            trId );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APPLICATION_REQ_OFFSET_NSB,
            numOfSubblocks );
        ret = iPhoNetSender->Send( isiMsg.Complete() );
        }
    else
        {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccApplicationReq, cannot handle the request, server busy!" );
OstTrace0( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_CREATEUICCAPPLICATIONREQ, "CMmUiccMessHandler::CreateUiccApplicationReq, cannot handle the request, server busy!" );
        ret = KErrServerBusy;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetApplicationId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CMmUiccMessHandler::GetApplicationId() const
    {
TFLOGSTRING2("TSY: CMmUiccMessHandler::GetApplicationId, application ID: %d", iApplicationId );
OstTraceExt1( TRACE_NORMAL, CMMUICCMESSHANDLER_GETAPPLICATIONID, "CMmUiccMessHandler::GetApplicationId;aApplId=%hhu", iApplicationId );
    return iApplicationId;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetApplicationType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CMmUiccMessHandler::GetApplicationType() const
    {
TFLOGSTRING2("TSY: CMmUiccMessHandler::GetApplicationType, application type: %d", iApplicationType );
OstTraceExt1( TRACE_NORMAL, CMMUICCMESSHANDLER_GETAPPLICATIONTYPE, "CMmUiccMessHandler::GetApplicationType;aApplType=%hhu", iApplicationType );
    return iApplicationType;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetApplicationFileId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CMmUiccMessHandler::GetApplicationFileId() const
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetApplicationFileId" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETAPPLICATIONFILEID, "CMmUiccMessHandler::GetApplicationFileId" );
    return iApplFileId;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetCardType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CMmUiccMessHandler::GetCardType() const
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetCardType" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETCARDTYPE, "CMmUiccMessHandler::GetCardType" );
    return iCardType;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetIsimApplicationStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CMmUiccMessHandler::GetIsimApplicationStatus() const
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetIsimApplicationStatus" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETISIMAPPLICATIONSTATUS, "CMmUiccMessHandler::GetIsimApplicationStatus" );
    return iIsimApplicationStatus;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::IsIsimApplicationFound
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMmUiccMessHandler::IsIsimApplicationFound() const
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::IsIsimApplicationFound" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_ISISIMAPPLICATIONFOUND, "CMmUiccMessHandler::IsIsimApplicationFound" );
    return iIsimApplicationFound;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetAid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const RMobilePhone::TAID& CMmUiccMessHandler::GetAid() const
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetAid" );
OstTrace0( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_GETAPPLICATIONFILEID, "CMmUiccMessHandler::GetApplicationFileId" );
    return iAid;
    }
// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbApplPath
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbApplPath(
    const TUiccParamsBase& aParams,
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset,
    TUint8& aSizeOfSubblock )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbApplPath" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCSBAPPLPATH, "CMmUiccMessHandler::CreateUiccSbApplPath" );

    // UICC_SB_APPL_PATH subblock
    TBuf8<KUiccSbApplPathSize> uiccSbApplPathBuf( 0 );
    TIsiSubBlock uiccSbApplPath(
        uiccSbApplPathBuf,
        UICC_SB_APPL_PATH,
        EIsiSubBlockTypeId16Len16 );

    // File ID, 16-bit
    uiccSbApplPathBuf.Append( static_cast<TUint8>( aParams.fileId >> 8 ) );
    uiccSbApplPathBuf.Append( static_cast<TUint8>( aParams.fileId ) );
    // File ID SFI, 8-bit
    uiccSbApplPathBuf.Append( aParams.fileIdSfi );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Length of the DF path, 8-bit
    uiccSbApplPathBuf.Append( aParams.filePath.Length() );
    // Filler, 8-bit
    uiccSbApplPathBuf.Append( KUiccPadding );
    // Application file ID depends on card type, USIM or SIM
    uiccSbApplPathBuf.Append( aParams.filePath );
    // Append subblock to ISI message
    aIsiMsg.CopyData( aMsgOffset, uiccSbApplPath.CompleteSubBlock() );

    aSizeOfSubblock = uiccSbApplPathBuf.Length();
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbClient
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbClient(
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset,
    const TUint8 aApplType )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbClient" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCSBCLIENT, "CMmUiccMessHandler::CreateUiccSbClient" );

    // UICC_SB_CLIENT subblock
    TBuf8<SIZE_UICC_SB_CLIENT> uiccSbClientBuf( 0 );
    TIsiSubBlock uiccSbClient(
        uiccSbClientBuf,
        UICC_SB_CLIENT,
        EIsiSubBlockTypeId16Len16 );

    // 3x filler
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );
    uiccSbClientBuf.Append( KUiccPadding );

    if( UICC_APPL_TYPE_UICC_USIM == aApplType ||
        UICC_APPL_TYPE_ICC_SIM == aApplType )
        {
        // Client ID
        uiccSbClientBuf.Append( iUiccClientId );

        // Append subblock to ISI message
        aIsiMsg.CopyData( aMsgOffset, uiccSbClient.CompleteSubBlock() );
        }
    else if( UICC_APPL_TYPE_UICC_ISIM == aApplType )
        {
        // Client ID
        uiccSbClientBuf.Append( iUiccIsimClientId );

        // Append subblock to ISI message
        aIsiMsg.CopyData( aMsgOffset, uiccSbClient.CompleteSubBlock() );
        }
    else
        {
TFLOGSTRING2("TSY: CMmUiccMessHandler::CreateUiccSbClient: unknown appl type: 0x%x", aApplType );
OstTrace1( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_CREATEUICCSBCLIENT, "CMmUiccMessHandler::CreateUiccSbClient: unknown appl type: 0x%x", aApplType );
        }
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbTransparent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbTransparent(
    const TUiccReadTransparent& aParams,
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbTransparent" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCSBTRANSPARENT, "CMmUiccMessHandler::CreateUiccSbTransparent" );

    // UICC_SB_TRANSPARENT subblock
    TBuf8<SIZE_UICC_SB_TRANSPARENT> uiccSbTransparentBuf( 0 );
    TIsiSubBlock uiccSbTransparent(
        uiccSbTransparentBuf,
        UICC_SB_TRANSPARENT,
        EIsiSubBlockTypeId16Len16 );

    // File offset, 16-bit
    uiccSbTransparentBuf.Append(
        static_cast<TInt8>( aParams.dataOffset >> 8 ) );
    uiccSbTransparentBuf.Append( static_cast<TInt8>( aParams.dataOffset ) );
    // Data amount, 16-bit, only used in read case
    uiccSbTransparentBuf.Append(
        static_cast<TInt8>( aParams.dataAmount >> 8 ) );
    uiccSbTransparentBuf.Append( static_cast<TInt8>( aParams.dataAmount ) );

    // Append subblock to ISI message
    aIsiMsg.CopyData( aMsgOffset, uiccSbTransparent.CompleteSubBlock() );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbLinearFixed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbLinearFixed(
    const TUiccReadLinearFixed& aParams,
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbLinearFixed" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCSBLINEARFIXED, "CMmUiccMessHandler::CreateUiccSbLinearFixed" );

    // UICC_SB_LINEAR_FIXED subblock
    TBuf8<SIZE_UICC_SB_LINEAR_FIXED> uiccSbLinearFixedBuf( 0 );
    TIsiSubBlock uiccSbLinearFixed(
        uiccSbLinearFixedBuf,
        UICC_SB_LINEAR_FIXED,
        EIsiSubBlockTypeId16Len16 );

    // File record, 8-bit
    uiccSbLinearFixedBuf.Append( aParams.record );
    // Record offset in bytes, 8-bit
    uiccSbLinearFixedBuf.Append( static_cast<TInt8>( aParams.dataOffset ) );
    // Data amount, 8-bit
    uiccSbLinearFixedBuf.Append( static_cast<TInt8>( aParams.dataAmount ) );

    // Append subblock to ISI message
    aIsiMsg.CopyData( aMsgOffset, uiccSbLinearFixed.CompleteSubBlock() );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbFileData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbFileData(
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset,
    const TDesC8& aUpdateData
    )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbFileData" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCSBFILEDATA, "CMmUiccMessHandler::CreateUiccSbFileData" );

    // UICC_SB_FILE_DATA subblock
    TBuf8<KUiccSbFileDataSize> uiccSbFileDataBuf( 0 );
    TIsiSubBlock uiccSbFileData(
        uiccSbFileDataBuf,
        UICC_SB_FILE_DATA,
        EIsiSubBlockTypeId16Len16 );

    // Length of file data, 32-bit
    TInt length( aUpdateData.Length() );
    uiccSbFileDataBuf.Append( static_cast<TInt8>( length >> 24 ) );
    uiccSbFileDataBuf.Append( static_cast<TInt8>( length >> 16 ) );
    uiccSbFileDataBuf.Append( static_cast<TInt8>( length >> 8 ) );
    uiccSbFileDataBuf.Append( static_cast<TInt8>( length ) );

    // Data
    uiccSbFileDataBuf.Append( aUpdateData );

    // Append subblock to ISI message
    aIsiMsg.CopyData( aMsgOffset, uiccSbFileData.CompleteSubBlock() );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbApplication
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbApplication(
    TIsiSend& aIsiMsg,
    TUint8 aMsgOffset,
    TUint8 aApplType )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbApplication" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCSBAPPLICATION, "CMmUiccMessHandler::CreateUiccSbApplication" );

    // SIZE_UICC_SB_APPLICATION_STR
    TBuf8<SIZE_UICC_SB_APPLICATION> uiccSbApplicationBuf( 0 );
    TIsiSubBlock uiccSbApplication(
        uiccSbApplicationBuf,
        UICC_SB_APPLICATION,
        EIsiSubBlockTypeId16Len16 );

    // 2x filler
    uiccSbApplicationBuf.Append( KUiccPadding );
    uiccSbApplicationBuf.Append( KUiccPadding );

    if( UICC_APPL_TYPE_UICC_USIM == aApplType ||
        UICC_APPL_TYPE_ICC_SIM == aApplType )
        {
        // Application type (8-bit)
        uiccSbApplicationBuf.Append( iApplicationType );

        // Application ID (8-bit)
        uiccSbApplicationBuf.Append( iApplicationId );

        // Append subblock to ISI message
        aIsiMsg.CopyData( aMsgOffset, uiccSbApplication.CompleteSubBlock() );
        }
    else if( UICC_APPL_TYPE_UICC_ISIM == aApplType )
        {
        // Application type (8-bit)
        uiccSbApplicationBuf.Append( aApplType );

        // Application ID (8-bit)
        uiccSbApplicationBuf.Append( iIsimApplicationId );

        // Append subblock to ISI message
        aIsiMsg.CopyData( aMsgOffset, uiccSbApplication.CompleteSubBlock() );
        }
    else
        {
TFLOGSTRING2("TSY: CMmUiccMessHandler::CreateUiccSbApplication: unknown application type: 0x%x", aApplType );
OstTrace1( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_CREATEUICCSBAPPLICATION, "CMmUiccMessHandler::CreateUiccSbApplication: unknown application type: 0x%x", aApplType );
        }
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetFileData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPtrC8 CMmUiccMessHandler::GetFileData( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetFileData" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETFILEDATA, "CMmUiccMessHandler::GetFileData" );

    TPtrC8 data( KNullDesC8 );

    // File data from UICC_SB_FILE_DATA
    TUint uiccSbFileDataOffset( 0 );
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_APPL_CMD_RESP,
        UICC_SB_FILE_DATA,
        EIsiSubBlockTypeId16Len16,
        uiccSbFileDataOffset ) )
        {
        TInt fileDataLength( aIsiMsg.Get32bit(
            uiccSbFileDataOffset + UICC_SB_FILE_DATA_OFFSET_DATALENGTH ) );
        data.Set( aIsiMsg.GetData(
            uiccSbFileDataOffset + UICC_SB_FILE_DATA_OFFSET_DATA,
            fileDataLength ) );
        }
    return data;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::HandleUiccResp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CMmUiccMessHandler::HandleUiccResp( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::HandleUiccResp" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_HANDLEUICCRESP, "CMmUiccMessHandler::HandleUiccResp" );

    TUint8 serverStatus( UICC_STATUS_NOT_READY );
    // Check the service type
    TUint8 serviceType( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + UICC_RESP_OFFSET_SERVICETYPE ) );
    if ( UICC_STATUS_GET  == serviceType )
        {
        // Get the status
        TUint8 status(
            aIsiMsg.Get8bit( ISI_HEADER_SIZE + UICC_RESP_OFFSET_STATUS ) );
        if ( UICC_STATUS_OK == status )
            {
            // Get the UICC server status
            serverStatus = aIsiMsg.Get8bit(
                ISI_HEADER_SIZE + UICC_RESP_OFFSET_SERVERSTATUS );
            }
        }
    return serverStatus;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::HandleUiccApplicationResp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmUiccMessHandler::HandleUiccApplicationResp(
    const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::HandleUiccApplicationResp" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_HANDLEUICCAPPLICATIONRESP, "CMmUiccMessHandler::HandleUiccApplicationResp" );

    TInt ret( KErrNone );
    // Get service type and status
    TUint8 serviceType( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + UICC_APPLICATION_RESP_OFFSET_SERVICETYPE ) );
    TUint8 status( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + UICC_APPLICATION_RESP_OFFSET_STATUS ) );
    iCardType = aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + UICC_APPLICATION_RESP_OFFSET_CARDTYPE );
    TUint8 trId( aIsiMsg.Get8bit(
        ISI_HEADER_SIZE + UICC_APPLICATION_RESP_OFFSET_TRANSID ) );
    TUint16 startIndex( ISI_HEADER_SIZE + SIZE_UICC_APPLICATION_RESP );

    if ( UICC_STATUS_OK == status )
        {
        switch( serviceType )
            {
            case UICC_APPL_LIST:
                {
                // Check if UICC_SB_APPL_DATA_OBJECT is included => UICC application
                TUint uiccSbApplDataObjectOffset( 0 );
                TUint8 applicationType( 0 );
                TUint16 sbLen( 0 );

                TBool usimApplFound( EFalse );

                while ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
                    startIndex,
                    UICC_SB_APPL_DATA_OBJECT,
                    EIsiSubBlockTypeId16Len16,
                    uiccSbApplDataObjectOffset ) )
                    {
                    applicationType = aIsiMsg.Get8bit(
                        uiccSbApplDataObjectOffset +
                        UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLICATIONTYPE );

                    sbLen = aIsiMsg.Get16bit(
                        uiccSbApplDataObjectOffset +
                        UICC_SB_APPL_DATA_OBJECT_OFFSET_SBLEN );

                    if ( UICC_APPL_TYPE_UICC_USIM == applicationType &&
                        EFalse == usimApplFound )
                        {
                        iApplicationType = applicationType;

                        iApplicationId = aIsiMsg.Get8bit(
                            uiccSbApplDataObjectOffset +
                            UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLID );
                        iApplicationStatus = aIsiMsg.Get8bit(
                            uiccSbApplDataObjectOffset +
                            UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLSTATUS );

                        TUint8 length( aIsiMsg.Get8bit(
                            uiccSbApplDataObjectOffset +
                            UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLDOLEN ) );

                        // Application data object contains EF dir data,
                        // 5 bytes are mandatory. See TS 102.221 V7.11.0 table 13.2
                        if ( 5 <= length )
                            {
                            TPtrC8 applDataObject( aIsiMsg.GetData(
                                uiccSbApplDataObjectOffset +
                                UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLDO,
                                length ) );
                            // Length of AID is located in index 3
                            TUint8 aidLength( applDataObject[3] );
                            // Copy actual AID
                            iAid.Copy( applDataObject.Mid( 4, aidLength ) );
                            }

                        usimApplFound = ETrue;
                        }
                    else if( UICC_APPL_TYPE_UICC_ISIM == applicationType &&
                        EFalse == iIsimApplicationFound )
                        {
                        iIsimApplicationFound = ETrue;

                        iIsimApplicationId = aIsiMsg.Get8bit(
                            uiccSbApplDataObjectOffset +
                            UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLID );
                        iIsimApplicationStatus = aIsiMsg.Get8bit(
                            uiccSbApplDataObjectOffset +
                            UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLSTATUS );
                        }
                    startIndex = uiccSbApplDataObjectOffset + sbLen;
                    }
                if ( EFalse == usimApplFound ) // ICC application
                    {
                    iApplicationType = UICC_APPL_TYPE_ICC_SIM;
                    iApplicationId =  UICC_APPL_ID_UNKNOWN;
                    // Application file ID for GSM
                    TBuf8<2> gsmFileId;
                    gsmFileId.Append( 0x7F );
                    gsmFileId.Append( 0x20 );
                    iApplFileId.Copy( gsmFileId );
                    }
                // Activate the application
                if ( UICC_STATUS_APPL_ACTIVE != iApplicationStatus )
                    {
                    TUiccParamsBase params;
                    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
                    params.trId = ETrIdActivateUiccApplication;
                    ret = CreateUiccApplicationReq( params, UICC_APPL_HOST_ACTIVATE );
                    }
                break;
                }
            case UICC_APPL_HOST_ACTIVATE:
                {
                TUint uiccSbClientOffset( 0 );
                if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
                    startIndex,
                    UICC_SB_CLIENT,
                    EIsiSubBlockTypeId16Len16,
                    uiccSbClientOffset ) )
                    {
                    if( ETrIdActivateUiccApplication == trId )
                        {
                        iUiccClientId = aIsiMsg.Get8bit(
                            uiccSbClientOffset +
                            UICC_SB_CLIENT_OFFSET_CLIENTID );
                        }
                    else if( ETrIdActivateIsimApplication == trId )
                        {
                        iUiccIsimClientId = aIsiMsg.Get8bit(
                            uiccSbClientOffset +
                            UICC_SB_CLIENT_OFFSET_CLIENTID );
                        }
                    }
                // UICC_SB_FCI contains PIN key references
                TUint uiccSbFciOffset( 0 );
                if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_UICC_APPLICATION_RESP,
                    UICC_SB_FCI,
                    EIsiSubBlockTypeId16Len16,
                    uiccSbFciOffset ) )
                    {
                    TInt fileDataLength( aIsiMsg.Get16bit(
                        uiccSbFciOffset + UICC_SB_FCI_OFFSET_FCILENGTH ) );
                    if ( 0 < fileDataLength )
                        {
                        StorePinKeyReferences( aIsiMsg.GetData(
                            uiccSbFciOffset + UICC_SB_FCI_OFFSET_FCI,
                            fileDataLength ) );
                        }
                    }
                // In case of ICC there is two UICC_SB_CHV subblocks
                // that contain PIN IDs for ICC application
                TUint uiccSbChvOffset( 0 );
                while ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
                    startIndex,
                    UICC_SB_CHV,
                    EIsiSubBlockTypeId16Len16,
                    uiccSbChvOffset ) )
                    {
                    TUint8 chvQualifier( aIsiMsg.Get8bit(
                        uiccSbChvOffset +
                        UICC_SB_CHV_OFFSET_CHVQUALIFIER ) );
                    if ( UICC_CHV1 == chvQualifier ) // PIN1
                        {
                        iPin1Id = aIsiMsg.Get8bit(
                            uiccSbChvOffset + UICC_SB_CHV_OFFSET_PINID );
                        }
                    else // PIN2
                        {
                        iPin2Id = aIsiMsg.Get8bit(
                            uiccSbChvOffset + UICC_SB_CHV_OFFSET_PINID );
                        }
                    startIndex = uiccSbChvOffset + SIZE_UICC_SB_CHV;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else // if ( UICC_STATUS_OK == status )
        {
        ret = KErrGeneral;
        TUint8 details( aIsiMsg.Get8bit(
            ISI_HEADER_SIZE + UICC_APPLICATION_RESP_OFFSET_DETAILS ) );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetFileFCI
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPtrC8 CMmUiccMessHandler::GetFileFCI( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetFileFCI");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETFILEFCI, "CMmUiccMessHandler::GetFileFCI" );

    TPtrC8 data( KNullDesC8 );
    TUint uiccSbFileDataOffset( 0 );

    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_APPL_CMD_RESP,
        UICC_SB_FCI,
        EIsiSubBlockTypeId16Len16,
        uiccSbFileDataOffset ) )
        {
        TInt fileDataLength( aIsiMsg.Get16bit(
            uiccSbFileDataOffset + UICC_SB_FCI_OFFSET_FCILENGTH ) );
        data.Set( aIsiMsg.GetData(
            uiccSbFileDataOffset + UICC_SB_FCI_OFFSET_FCI,
            fileDataLength ) );
        }
    return data;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccConnectorReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmUiccMessHandler::CreateUiccConnectorReq(
    const TUiccParamsBase& aParams )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccConnectorReq");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCCONNREQ, "CMmUiccMessHandler::CreateUiccConnectorReq" );

    TInt ret (KErrGeneral);
    TInt trId( aParams.trId );
    TInt8 serviceType( aParams.serviceType );
    TUint8 numOfSubblocks( 0 );

    // If there is the previous request ongoing (pointer was already set for
    // this operation)  we can't handle this request.
    if ( NULL == iMessHandlerPrtList.At( trId ) )
        {
        // Save pointer to message handler, it is needed when receiving
        // response for this operation
        iMessHandlerPrtList[trId] = aParams.messHandlerPtr;

        // Create UICC_CONNECTOR_REQ message
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_CONNECTOR_REQ_OFFSET_TRANSID, trId );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_CONNECTOR_REQ_OFFSET_MESSAGEID,
            UICC_CONNECTOR_REQ );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_CONNECTOR_REQ_OFFSET_SERVICETYPE,
            serviceType );

        switch( serviceType )
            {
            case UICC_CONNECT:
                {
                // Set status to not active, since UICC connect request
                // resets smartcard.
                iApplicationStatus = UICC_STATUS_APPL_NOT_ACTIVE;

                // Sets flag, that when the response for this request is
                // received traid is not removed from the list, until
                // UICC_APPLICATION_IND is received.
                iDoNotRemoveTransactionID = ETrue;
                break;
                }
            default:
                break;
            }

        isiMsg.Set8bit(
                ISI_HEADER_SIZE + UICC_CONNECTOR_REQ_OFFSET_NSB,
                numOfSubblocks );

        return( iPhoNetSender->Send( isiMsg.Complete() ) );
        }
    else
        {
        ret = KErrServerBusy;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccApduReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmUiccMessHandler::CreateUiccApduReq( const TUiccParamsApduReq& aParams )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccApduReq");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCAPDUREQ, "CMmUiccMessHandler::CreateUiccApduReq" );

    TInt ret (KErrGeneral);
    TInt trId( aParams.trId );
    TInt8 serviceType( aParams.serviceType );
    TUint8 numOfSubblocks( 0 );
    TUint8 action( aParams.action );

    // If there is the previous request ongoing (pointer was already set for
    // this operation)  we can't handle this request.
    if ( NULL == iMessHandlerPrtList.At( trId ) )
        {
        // Save pointer to message handler, it is needed when receiving
        // response for this operation
        iMessHandlerPrtList[trId] = aParams.messHandlerPtr;
        // Create UICC_APDU_REQ
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_APDU_REQ_OFFSET_TRANSID, trId );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APDU_REQ_OFFSET_MESSAGEID,
            UICC_APDU_REQ );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APDU_REQ_OFFSET_SERVICETYPE,
            serviceType );

        switch( serviceType )
            {
            case UICC_APDU_CONTROL:
                {
                if( UICC_CONTROL_CARD_ACTIVATE == action )
                    {
                    // Sets flag, that when the response for this request is
                    // received traid is not removed from the list, until
                    // UICC_APDU_RESET_IND is received.
                    iDoNotRemoveTransactionID = ETrue;
                    }
                // Create subblock UICC_SB_APDU_ACTIONS.
                CreateUiccSbApduActions(
                    isiMsg,
                    action );
                numOfSubblocks = 1;
                break;
                }
            case UICC_APDU_SEND:
                {
                // create SB UICC_SB_APDU.
                CreateUiccSbApdu(
                    isiMsg,
                    KUiccApduReqSubblockOffset,
                    aParams .apduData );
                numOfSubblocks = 1;
                break;
                }
            default:
                {
                break;
                }
            }

        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_APDU_REQ_OFFSET_NSB,
            numOfSubblocks );

        ret = iPhoNetSender->Send( isiMsg.Complete() );
        }
    else
        {
        ret = KErrServerBusy;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbApduActions
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbApduActions(
    TIsiSend& aIsiMsg,
    TUint8 aAction )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbApduActions");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATEUICCASBPDUACTION, "CMmUiccMessHandler::CreateUiccSbApduActions" );

    // SIZE_UICC_SB_APPLICATION_STR
    TBuf8<SIZE_UICC_SB_APDU_ACTIONS> uiccSbApduActionBuf( 0 );
    TIsiSubBlock uiccSbApduAction(
        uiccSbApduActionBuf,
        UICC_SB_APDU_ACTIONS,
        EIsiSubBlockTypeId16Len16 );

    // Action
    uiccSbApduActionBuf.Append( aAction );
    // 3x filler
    uiccSbApduActionBuf.Append( KUiccPadding );
    uiccSbApduActionBuf.Append( KUiccPadding );
    uiccSbApduActionBuf.Append( KUiccPadding );

    // Append subblock to ISI message
    aIsiMsg.CopyData(
        KUiccApduReqSubblockOffset,
        uiccSbApduAction.CompleteSubBlock() );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetAPduData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPtrC8 CMmUiccMessHandler::GetApduData( const TIsiReceiveC& aIsiMsg )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetApduData");
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETAPDUDATA, "CMmUiccMessHandler::GetApduData" );

    TPtrC8 data( KNullDesC8 );

    // File data from UICC_SB_APDU
    TUint uiccSbApduOffset( 0 );
    if ( KErrNone == aIsiMsg.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_APPL_CMD_REQ,
        UICC_SB_APDU,
        EIsiSubBlockTypeId16Len16,
        uiccSbApduOffset ) )
        {
        TInt apduDataLength( aIsiMsg.Get16bit(
            uiccSbApduOffset + KUiccSbApduLengthOffset ) );

        data.Set( aIsiMsg.GetData(
            uiccSbApduOffset + KUiccSbApduDataOffset,
            apduDataLength ) );
        }
    return data;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccSbApdu
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CreateUiccSbApdu(
        TIsiSend& aIsiMsg,
        TUint8 aMsgOffset,
        const TDesC8& aAPDUData)
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccSbApdu" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATESBAPDU, "CMmUiccMessHandler::CreateUiccSbApdu" );

    // UICC_SB_APDU subblock
    TBuf8<KUiccSbApduSize> uiccSbApduBuf( 0 );
    TIsiSubBlock uiccSbApdu(
        uiccSbApduBuf,
        UICC_SB_APDU,
        EIsiSubBlockTypeId16Len16 );

    // Force APDU command
    uiccSbApduBuf.Append( UICC_APDU_CMD_FORCE_NOT_USED );

    // Filler KUiccPadding
    uiccSbApduBuf.Append( KUiccPadding );

    // Length of APDU command data, 16-bit
    TInt length( aAPDUData.Length() );
    uiccSbApduBuf.Append( static_cast<TInt8>( length >> 8 ) );
    uiccSbApduBuf.Append( static_cast<TInt8>( length ) );

    // Data
    uiccSbApduBuf.Append( aAPDUData );

    // Append subblock to ISI message
    aIsiMsg.CopyData( aMsgOffset, uiccSbApdu.CompleteSubBlock() );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CreateUiccCardReq
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmUiccMessHandler::CreateUiccCardReq( const TUiccParamsBase& aParams )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CreateUiccCardReq" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CREATECARDREQ, "CMmUiccMessHandler::CreateUiccCardReq" );

    TInt ret ( KErrGeneral );
    TInt trId( aParams.trId );
    TInt8 serviceType( aParams.serviceType );

    // If there is the previous request ongoing (pointer was already set for
    // this operation)  we can't handle this request.
    if ( NULL == iMessHandlerPrtList.At( trId ) )
        {
        // Save pointer to message handler, it is needed when receiving
        // respone for this operation
        iMessHandlerPrtList[trId] = aParams.messHandlerPtr;

        // Create UICC_CARD_REQ message
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_CARD_REQ_OFFSET_TRANSID,
            trId );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_CARD_REQ_OFFSET_MESSAGEID,
            UICC_CARD_REQ );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_CARD_REQ_OFFSET_SERVICETYPE,
            serviceType );
        isiMsg.Set8bit( ISI_HEADER_SIZE + UICC_CARD_REQ_OFFSET_CARDTYPE,
            iCardType );

        return( iPhoNetSender->Send( isiMsg.Complete() ) );
        }
    else
        {
        ret = KErrServerBusy;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::InitializeSimServiceTableCache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::InitializeSimServiceTableCache()
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::InitializeSimServiceTableCache" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_INITIALIZESIMSERVICETABLECACHE, "CMmUiccMessHandler::InitializeSimServiceTableCache" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdSstCache;
    params.dataAmount = 0;
    params.dataOffset = 0;
    params.fileId = KElemSimServiceTable;

    // If card type is UICC, we need to read EFust, otherwise EFsst
    // EFust contains SFI identifier (value is 4), but EFsst doesn't
    // contain SFI, so in that case we need to use UICC_SFI_NOT_PRESENT
    params.fileIdSfi = iCardType == UICC_CARD_TYPE_UICC ? 4 : UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( GetApplicationFileId() );

    CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::SimServiceTableCacheResp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::SimServiceTableCacheResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::SimServiceTableCacheResp" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_SIMSERVICETABLECACHERESP, "CMmUiccMessHandler::SimServiceTableCacheResp" );

    if ( UICC_STATUS_OK == aStatus )
        {
        if ( UICC_CARD_TYPE_UICC == iCardType )
            {
            // fill iUstFile
            iUSTFile.Copy( aFileData );
            }
        else if ( UICC_CARD_TYPE_ICC == iCardType )
            {
            // fill iSstFile
            iSSTFile.Copy( aFileData );
            }
        else
            {
TFLOGSTRING("TSY: CMmUiccMessHandler::SimServiceTableCacheResp unknown card type, cache not done" );
OstTrace0( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_SIMSERVICETABLECACHERESP, "CMmUiccMessHandler::SimServiceTableCacheResp unknown card type, cache not done" );
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmUiccMessHandler::SimServiceTableCacheResp: reading failed (0x%x)", aStatus );
OstTrace1( TRACE_NORMAL, DUP2_CMMUICCMESSHANDLER_SIMSERVICETABLECACHERESP, "CMmUiccMessHandler::SimServiceTableCacheResp: reading failed (%x)", aStatus );
        }

    //Complete Notify SIM Ready
    iMessageRouter->Complete(
        EMmTsyBootNotifySimStatusReadyIPC,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetServiceStatus
// (other items were commented in a header).
//
TBool CMmUiccMessHandler::GetServiceStatus( TUint8 aServiceNo )
    {
TFLOGSTRING2("TSY: CMmUiccMessHandler::GetServiceStatus for card: ox%x", iCardType );
OstTrace1( TRACE_NORMAL, CMMUICCMESSHANDLER_GETSERVICESTATUS, "CMmUiccMessHandler::GetServiceStatus for card: ox%x", iCardType );

    TBool ret( EFalse );

    if( UICC_CARD_TYPE_UICC == iCardType )
        {
        // card type UICC, so service status info is in
        // EFust. Service info is coded as one bit,
        // for example first byte contains services 1-8 as:
        // bit:        b8  b7  b6  b5  b4  b3  b2  b1
        // service no: 08  07  06  05  04  03  02  01
        // If bit is 1, service is supported
        // For more info about EFust, see 3GPP spec 31.102

        // let's calculate index to get correct byte
        // which contains info about service requested
        TUint8 index( ( aServiceNo - 1 ) / 8 );

        //let's calculate the bit number which contains
        // service info
        TUint8 bit( ( aServiceNo - 1 ) % 8 );

        // form mask to get is service supported or not
        TUint8 mask( 1<<bit );

        if( iUSTFile.Length() > index )
            {
            // If result is > 0, service is supported
            if( iUSTFile[index] & mask )
                {
TFLOGSTRING2("TSY: CMmUiccMessHandler::GetServiceStatus: UICC service (%d) supported", aServiceNo );
OstTrace1( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_GETSERVICESTATUS, "CMmUiccMessHandler::GetServiceStatus: UICC service (%d) supported", aServiceNo );
                ret = ETrue;
                }
            }
        }
    else if( UICC_CARD_TYPE_ICC == iCardType )
        {
        // card type ICC, so service status info is in
        // EFsst. Service info is coded as two bits,
        // for example first byte contains services 1-4 as:
        // bit:        b8  b7  b6  b5  b4  b3  b2  b1
        // service no: 04  04  03  03  02  02  01  01

        // first bit describes is service allocated
        // second bit describes is service activated
        // for example service no 4: first bit is b7
        // and second bit is b8. Both bits needs to be
        // 1 so that service is supported. For more
        // info about EFsst, see 3GPP spec 51.011

        // let's calculate index to get correct byte
        // which contains info about service requested
        TUint8 index( ( aServiceNo - 1 ) / 4 );

        //let's calculate the bit number which contains
        // service info
        TUint8 bit( ( aServiceNo - 1 ) % 4 );

        // form mask to get is service supported or not.
        TUint8 mask( 3<<( bit * 2 ) );

        if( iSSTFile.Length() > index )
            {
            // If both bits are 1, service is supported
            if( ( iSSTFile[index] & mask ) == mask )
                {
TFLOGSTRING2("TSY: CMmUiccMessHandler::GetServiceStatus: ICC service (%d) supported", aServiceNo );
OstTrace1( TRACE_NORMAL, DUP2_CMMUICCMESSHANDLER_GETSERVICESTATUS, "CMmUiccMessHandler::GetServiceStatus: ICC service (%d) supported", aServiceNo );
                ret = ETrue;
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::ProcessUiccMsg
// Handles data received from UICC server
// -----------------------------------------------------------------------------
//
TInt CMmUiccMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY: CMmUiccMessHandler::ProcessUiccMsg, transaction ID: %d, status: %d", aTraId, aStatus );
OstTrace1( TRACE_NORMAL, CMMUICCMESSHANDLER_PROCESSUICCMSG, "CMmUiccMessHandler::ProcessUiccMsg, transaction ID: %d", aTraId );
OstTrace1( TRACE_NORMAL, DUP2_CMMUICCMESSHANDLER_PROCESSUICCMSG, "CMmUiccMessHandler::ProcessUiccMsg, status: %d", aStatus );

    TInt ret( KErrNone );

    switch( aTraId )
        {
        case ETrIdSstCache:
            {
            SimServiceTableCacheResp( aStatus, aFileData );
            break;
            }
        case ETrIdCphsCache:
            {
            CphsInformationCacheResp( aStatus, aFileData );
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmUiccMessHandler::ProcessUiccMsg, unknown transaction ID" );
OstTrace0( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_PROCESSUICCMSG, "CMmUiccMessHandler::ProcessUiccMsg, unknown transaction ID" );
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::StorePinKeyReferences
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::StorePinKeyReferences( const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::StorePinKeyReferences" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_STOREPINKEYREFERENCES, "CMmUiccMessHandler::StorePinKeyReferences" );
    // See ETSI TS 102 221 V7.11.0 (2008-07)chapter 9.5.2
    // There is only one PIN1/PIN2/UPIN per application
    TUint8 lengthOfPs( aFileData[3] ); // Number of PS bytes is in index 3

    // Get PS_DO for PIN statuses
    TPtrC8 psDo;
    psDo.Set( aFileData.Mid( 4, lengthOfPs ) );

    // Update index to point the first PIN
    TUint8 index( 4 + lengthOfPs );
    // Copy PIN related data to new buffer
    TPtrC8 pinDataBuffer;
    pinDataBuffer.Set( aFileData.Mid( index ) );
    TInt length( pinDataBuffer.Length() );
    TUint8 pinId( 0 );
    TUint8 orderNum( 0 ); // Used for shifting PIN status byte
    TBool upinExists( EFalse );
    TBool pin1Active( EFalse );

    index = 0; // Start of new buffer
    while( length )
        {
        // Check if usage qualifier exists ( tag '95'). In that case skip it
        if ( 0x95 == pinDataBuffer[index] )
            {
            index += 3;
            length -= 3;
            }
        // PIN key reference
        pinId = pinDataBuffer[index + 2];
        index += 3; // Skip PIN key reference data element
        length -= 3;

        // Store PIN ID
        if ( 1 <= pinId && 8 >= pinId ) // It is PIN1
            {
            iPin1Id = pinId;
            // Check if PIN1 is enabled ( the active PIN )
            if ( 0x80 & ( psDo[0] << orderNum ) )
                {
                iActivePin = RMobilePhone::ESecurityCodePin1;
                pin1Active = ETrue;
                }
            }
        else if ( 0x81 <= pinId && 0x88 >= pinId ) // It is PIN2
            {
            iPin2Id = pinId;
            orderNum++;
            }
        else if ( 0x11 == pinId )
            {
            upinExists = ETrue;
            orderNum++;
            }
        // No else
        }

    // If PIN1 is not set as active and UPIN exists, UPIN is the active PIN
    if ( upinExists && !pin1Active )
        {
        iActivePin = RMobilePhone::ESecurityUniversalPin;
        }
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetPin1KeyReference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CMmUiccMessHandler::GetPin1KeyReference()
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetPin1KeyReference" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETPIN1KEYREFERENCE, "CMmUiccMessHandler::GetPin1KeyReference" );
    return iPin1Id;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetPin1KeyReference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CMmUiccMessHandler::GetPin2KeyReference()
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetPin2KeyReference" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETPIN2KEYREFERENCE, "CMmUiccMessHandler::GetPin2KeyReference" );
    return iPin2Id;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetActivePin
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RMobilePhone::TMobilePhoneSecurityCode CMmUiccMessHandler::GetActivePin()
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetActivePin" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_GETACTIVEPIN, "CMmUiccMessHandler::GetActivePin" );
    return iActivePin;
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::ChangeActivePin()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::ChangeActivePin()
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::ChangeActivePin" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CHANGEACTIVEPIN, "CMmUiccMessHandler::ChangeActivePin" );
    // Change the internal flag indicating which PIN is active
    if ( RMobilePhone::ESecurityCodePin1 == iActivePin )
        {
        iActivePin = RMobilePhone::ESecurityUniversalPin;
        }
    else if ( RMobilePhone::ESecurityUniversalPin == iActivePin )
        {
        iActivePin = RMobilePhone::ESecurityCodePin1;
        }
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::InitializeCphsInformationCache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::InitializeCphsInformationCache()
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::InitializeCphsInformationCache" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_INITIALIZECPHSINFORMATIONCACHE, "CMmUiccMessHandler::InitializeCphsInformationCache" );

    // CPHS information is read only in case of ICC card
    if( UICC_CARD_TYPE_ICC == iCardType )
        {
        // Set parameters for UICC_APPL_CMD_REQ message
        TUiccReadTransparent params;
        params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        params.trId = ETrIdCphsCache;
        params.dataAmount = 0;
        params.dataOffset = 0;
        params.fileId = KElemCphsInformation;
        params.fileIdSfi = UICC_SFI_NOT_PRESENT;
        params.serviceType = UICC_APPL_READ_TRANSPARENT;

        // File id path
        params.filePath.Append( KMasterFileId >> 8 );
        params.filePath.Append( KMasterFileId );
        params.filePath.Append( GetApplicationFileId() );

        CreateUiccApplCmdReq( params );
        }
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::CphsInformationCacheResp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmUiccMessHandler::CphsInformationCacheResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::CphsInformationCacheResp" );
OstTrace0( TRACE_NORMAL, CMMUICCMESSHANDLER_CPHSINFORMATIONCACHERESP, "CMmUiccMessHandler::CphsInformationCacheResp" );

    if ( UICC_STATUS_OK == aStatus )
        {
        // First byte of CPHS information is for CPHS phase, lbut
        // we are interested in only CPHS table, so we skip the 
        // first byte and we just copy the CPHS table data
        TUint8 dataSize( aFileData.Length() - 1 );
        if( KEfCphsInfoSize >= dataSize )
            {
            iCPHSInformation.Copy( aFileData.Mid( 1, dataSize ) );
            }
        else
            {
TFLOGSTRING3("TSY: CMmUiccMessHandler::CphsInformationCacheResp: buffer too small (expected: %d, actual: %d), cannot store CPHS info", dataSize, iCPHSInformation.Length() );
OstTrace0( TRACE_NORMAL, DUP3_CMMUICCMESSHANDLER_CPHSINFORMATIONCACHERESP, "CMmUiccMessHandler::CphsInformationCacheResp: buffer too small, cannot store CPHS info" );
OstTrace1( TRACE_NORMAL, DUP2_CMMUICCMESSHANDLER_CPHSINFORMATIONCACHERESP, "CMmUiccMessHandler::CphsInformationCacheResp: expected size: %d", dataSize );
OstTrace1( TRACE_NORMAL, DUP4_CMMUICCMESSHANDLER_CPHSINFORMATIONCACHERESP, "CMmUiccMessHandler::CphsInformationCacheResp: actual size: %d", iCPHSInformation.Length() );
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmUiccMessHandler::CphsInformationCacheResp: reading failed (0x%x)", aStatus );
OstTrace1( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_CPHSINFORMATIONCACHERESP, "CMmUiccMessHandler::CphsInformationCacheResp: reading failed (0x%x)", aStatus );
        }
    }

// -----------------------------------------------------------------------------
// CMmUiccMessHandler::GetCphsInformationStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMmUiccMessHandler::GetCphsInformationStatus( TUint8 aServiceNo )
    {
TFLOGSTRING("TSY: CMmUiccMessHandler::GetCphsInformationStatus" );
OstTrace0( TRACE_NORMAL, DUP1_CMMUICCMESSHANDLER_GETCPHSINFORMATIONSTATUS, "CMmUiccMessHandler::GetCphsInformationStatus" );

    TBool ret( EFalse );

    // CPHS info is coded as two bits,
    // for example first byte contains services 1-4 as:
    // bit:        b8  b7  b6  b5  b4  b3  b2  b1
    // service no: 04  04  03  03  02  02  01  01

    // first bit describes is service allocated
    // second bit describes is service activated
    // for example service no 4: first bit is b7
    // and second bit is b8. Both bits needs to be
    // 1 so that service is supported. For more
    // info about CPHS info, see CPHS Phase 2 specification

    // let's calculate index to get correct byte
    // which contains info about service requested
    TUint8 index( ( aServiceNo - 1 ) / 4 );

    // let's calculate the bit number which contains
    // service info
    TUint8 bit( ( aServiceNo - 1 ) % 4 );

    // form mask to get is service supported or not.
    TUint8 mask( 3<<( bit * 2 ) );

    if( iCPHSInformation.Length() > index )
        {
        // If both bits are 1, service is supported
        if( ( iCPHSInformation[index] & mask ) == mask )
            {
TFLOGSTRING2("TSY: CMmUiccMessHandler::GetCphsInformationStatus: CPHS service (%d) supported", aServiceNo );
OstTrace1( TRACE_NORMAL, CMMUICCMESSHANDLER_GETCPHSINFORMATIONSTATUS, "CMmUiccMessHandler::GetCphsInformationStatus: CPHS service (%d) supported", aServiceNo );
            ret = ETrue;
            }
        }
    return ret;
    }


//  End of File
