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



// INCLUDES
#include "cmmpipecontrol.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmpacketservicemesshandler.h"
#include "cmmpacketmesshandlercontextlist.h"
#include <tisi.h>
#include <nsisi.h>
#include "tsylogger.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpipecontrolTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
    // None

// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// ==================== LOCAL FUNCTIONS =======================================

    // None

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CMmPipeControl::CMmPipeControl
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMmPipeControl::CMmPipeControl()
    {

    TFLOGSTRING("TSY: CMmPipeControl::CMmPipeControl");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_CMMPIPECONTROL_TD, "CMmPipeControl::CMmPipeControl" );
    }

// -----------------------------------------------------------------------------
// CMmPipeControl::~CMmPipeControl
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmPipeControl::~CMmPipeControl()
    {

    TFLOGSTRING("TSY: CMmPipeControl::~CMmPipeControl");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_CMMPIPECONTROL_TD, "CMmPipeControl::~CMmPipeControl" );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMmPipeControl* CMmPipeControl::NewL(
    CMmPhoNetSender* aPhoNetSender,
    CMmPhoNetReceiver* aPhoNetReceiver,
    CMmPacketContextMessHandler* aMmPacketContextMessHandler,
    CMmPacketContextMesshandlerList* aMmPacketContextMesshandlerList)
    {

    TFLOGSTRING("TSY: CMmPipeControl::NewL");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_NEWL_TD, "CMmPipeControl::NewL" );

    CMmPipeControl* mmPipeControl = new ( ELeave ) CMmPipeControl();

    CleanupStack::PushL( mmPipeControl );

    mmPipeControl->iPhoNetSender = aPhoNetSender;
    mmPipeControl->iContextMessHandler = aMmPacketContextMessHandler;
    mmPipeControl->iContextList = aMmPacketContextMesshandlerList;

    mmPipeControl->ConstructL();

    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PIPE_CREATE_RESP );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PIPE_ENABLE_RESP );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PIPE_RESET_RESP );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PIPE_REMOVE_RESP );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PIPE_REDIRECT_RESP );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_CTRL_RESP );

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_CONNECT_REQ );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_DISCONNECT_REQ );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_RESET_REQ );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_ENABLE_REQ );

#endif //DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS


    CleanupStack::Pop( mmPipeControl );

    return mmPipeControl;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::ConstructL()
    {

    TFLOGSTRING("TSY: CMmPipeControl::ConstructL");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_CONSTRUCTL_TD, "CMmPipeControl::ConstructL" );

    iSecondPepDeviceId = KSecondPepDevice; // Default Device Id of second PEP
    iSecondPepObjectId = KSecondPepObject; // Default Object Id of second PEP

    InitLists();
    }

// -----------------------------------------------------------------------------
// CMmPipeControl::ReceiveMessageL
// Entry point for messages received from Domestic OS.
// Switches the message to the correct method.
// -----------------------------------------------------------------------------
//
void CMmPipeControl::ReceiveMessageL( const TIsiReceiveC& aIsiMessage )
    {
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    TFLOGSTRING3("TSY: CMmPipeControl::ReceiveMessageL - resource: %d, msgId: %d", resource, messageId);
OstTraceExt2( TRACE_NORMAL,  CMMPIPECONTROL_RECEIVEMESSAGEL_TD, "CMmPipeControl::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch ( resource )
        {
        case PN_PIPE:
            {
            switch( messageId )
                {
                case PNS_PIPE_CREATE_RESP:
                    {
                    PnsPipeCreateResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_ENABLE_RESP:
                    {
                    PnsPipeEnableResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_RESET_RESP:
                    {
                    PnsPipeResetResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_REMOVE_RESP:
                    {
                    PnsPipeRemoveResp( aIsiMessage );
                    break;
                    }
                case PNS_PIPE_REDIRECT_RESP:
                    {
                    PnsPipeRedirectResp( aIsiMessage );
                    break;
                    }
                case PNS_PEP_CTRL_RESP:
                    {
                    PnsPepCtrlResp( aIsiMessage );
                    break;
                    }

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
                case PNS_PEP_CONNECT_REQ:
                    {
                    PnsPepConnectReq( aIsiMessage );
                    break;
                    }
                case PNS_PEP_DISCONNECT_REQ:
                    {
                    PnsPepDisconnectReq( aIsiMessage );
                    break;
                    }
                case PNS_PEP_RESET_REQ:
                    {
                    PnsPepResetReq( aIsiMessage );
                    break;
                    }
                case PNS_PEP_ENABLE_REQ:
                    {
                    PnsPepEnableReq( aIsiMessage );
                    break;
                    }
#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

                default:
                    {

                    TFLOGSTRING2("TSY: CMmPipeControl::ReceiveMessageL - PN_PIPE - unknown msgId: %d", messageId);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_RECEIVEMESSAGEL_TD, "CMmPipeControl::ReceiveMessageL;unknown messageId=%d", messageId );
                    break;
                    }
                }
            break;
            }
        default:
            {

            TFLOGSTRING2("TSY: CMmPipeControl::ReceiveMessageL - unknown resource: %d", resource);
OstTrace1( TRACE_NORMAL,  DUP3_CMMPIPECONTROL_RECEIVEMESSAGEL_TD, "CMmPipeControl::ReceiveMessageL;unknown resource=%d", resource );
            break; // server not known
            }
        } // end of switch
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::InitLists
// Initialize used lists and attributes.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::InitLists()
    {

    TFLOGSTRING("TSY: CMmPipeControl::InitLists");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_INITLISTS_TD, "CMmPipeControl::InitLists" );

    // Find PipeHandle id from Pipe Operation by PipeHandle List
    for ( TInt i = 0; i < KMmMaxNumberOfPipeHandles; i++ )
        {
        // Initialize pipe operation array
        iPipeOperationArray[i].iPipeHandle = KInvalidPipeHandle;

        // Use zero as default value
        iPipeOperationArray[i].iTransId = 0;
        }
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::SetPipeHandle
// Set Pipe Handle value to the list.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::SetPipeHandle(
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING("TSY: CMmPipeControl::SetPipeHandle");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_SETPIPEHANDLE_TD, "CMmPipeControl::SetPipeHandle" );

    // Find PipeHandle id from Pipe Operation by PipeHandle List
    for ( TInt i = 0; i < KMmMaxNumberOfPipeHandles; i++ )
        {
        // If KInvalidPipeHandle is same as in iPipeOperationArray list
        if ( KInvalidPipeHandle == iPipeOperationArray[i].iPipeHandle )
            {
            // Set pipehandle to the pipe operation list
            iPipeOperationArray[i].iPipeHandle = aPipeHandle;
            // Pipe handle found, break
            i = KMmMaxNumberOfPipeHandles;
            }
        //no else
        }
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::SetTransactionId
// If pipehandle found set transaction id value to the list.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::SetTransactionId(
    const TUint8 aPipeHandle,
    const TUint8 aTransId )
    {

    TFLOGSTRING("TSY: CMmPipeControl::SetTransactionId");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_SETTRANSACTIONID_TD, "CMmPipeControl::SetTransactionId" );

    // Find PipeHandle id from Pipe Operation by PipeHandle List
    for ( TInt i = 0; i < KMmMaxNumberOfPipeHandles; i++ )
        {
        // If aPipeHandle is same as in iPipeOperationArray list
        if ( aPipeHandle == iPipeOperationArray[i].iPipeHandle )
            {
            // Set aTransId to the pipe operation list
            iPipeOperationArray[i].iTransId = aTransId;
            // Pipe handle found, break
            i = KMmMaxNumberOfPipeHandles;
            }
        //no else
        }
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::TransactionId
// Returns Transaction id by PipeHandle.
// ----------------------------------------------------------------------------
//
TUint8 CMmPipeControl::TransactionId(
    const TUint8 aPipeHandle ) const
    {

    TFLOGSTRING("TSY: CMmPipeControl::TransactionId");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_TRANSACTIONID_TD, "CMmPipeControl::TransactionId" );

    TUint8 transId( 0 );

    // Find PipeHandle from Pipe Operation by PipeHandle List
    for ( TInt i = 0; i < KMmMaxNumberOfPipeHandles; i++ )
        {
        // If aPipeHandle is same as in iPipeOperationArray list
        if ( aPipeHandle == iPipeOperationArray[i].iPipeHandle )
            {
            // Get transaction id from the PipeOperationArray list
            transId = iPipeOperationArray[i].iTransId;
            // Pipe handle found, break
            i = KMmMaxNumberOfPipeHandles;
            }
        //no else
        }
    // Return Transaction id
    return transId;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::ResetPipeHandle
// Remove Pipehandle and Transaction id from the list.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::ResetPipeHandle(
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING("TSY: CMmPipeControl::ResetPipeHandle");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_RESETPIPEHANDLE_TD, "CMmPipeControl::ResetPipeHandle" );

    // Find PipeHandle id from Pipe Operation by PipeHandle List
    for ( TInt i = 0; i < KMmMaxNumberOfPipeHandles; i++ )
        {
        // If aPipeHandle is same as in iPipeOperationArray list
        if ( aPipeHandle == iPipeOperationArray[i].iPipeHandle )
            {
            // Initialize Pipehandle to KInvalidPipeHandle
            iPipeOperationArray[i].iPipeHandle = KInvalidPipeHandle;
            // initialize TransactionId to zero
            iPipeOperationArray[i].iTransId = 0;
            // Pipe handle found, break
            i = KMmMaxNumberOfPipeHandles;
            }
        //no else
        }
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::IsOperationCalled
// Check if pipe operation for given pipe is outgoing
// ----------------------------------------------------------------------------
//
TBool CMmPipeControl::IsOperationCalled(
    const TUint8 aPipeHandle ) const
    {

    TFLOGSTRING("TSY: CMmPipeControl::IsOperationCalled");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_ISOPERATIONCALLED_TD, "CMmPipeControl::IsOperationCalled" );

    TBool ret( EFalse);

    // Find PipeHandle id from PipeHandle List
    for ( TInt i = 0; i < KMmMaxNumberOfPipeHandles; i++ )
        {
        if ( aPipeHandle == iPipeOperationArray[i].iPipeHandle )
            {
            // PipeHandle id found, return true
            ret = ETrue;
            }
        }
    // PipeHandle id not found, return false
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeCreateReq
// Construct a PNS_PIPE_CREATE_REQ ISI-message
// ----------------------------------------------------------------------------
//
TInt CMmPipeControl::PnsPipeCreateReq(
    const TUint8 aTransId,
    const TUint8 aPipeState,
    const TUint8 aFirstPepObject ) const
    {

    TFLOGSTRING2("TSY: CMmPipeControl::PnsPipeCreateReq. TransactionId: %d", aTransId );
OstTraceExt1( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPECREATEREQ_TD, "CMmPipeControl::PnsPipeCreateReq;aTransId=%hhu", aTransId );

    // create buffer for isi msg data
    TBuf8<SIZE_PNS_PIPE_CREATE_REQ> data;

    // set message data
    data.Append( aPipeState );
    data.Append( KGpdsPadding );
    data.Append( KFirstPepDevice );
    data.Append( aFirstPepObject );
    data.Append( KFirstPepType );
    data.Append( KGpdsPadding );
    data.Append( iSecondPepDeviceId );
    data.Append( iSecondPepObjectId );
    data.Append( KSecondPepType );
    data.Append( KGpdsPadding );

    // Sending message to phonet
    return iPhoNetSender->Send(
        PN_PIPE, aTransId, PNS_PIPE_CREATE_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeCreateResp
// Breaks a PNS_PIPE_CREATE_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPipeCreateResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmPipeControl::PnsPipeCreateResp");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPECREATERESP_TD, "CMmPipeControl::PnsPipeCreateResp" );

    // Get transaction Id
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE ) );
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_ERRORCODE ) );

        TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeCreateResp. TransactionId: %d, PipeHandle: %d, error code: %d", transId, pipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPIPECREATERESP_TD, "CMmPipeControl::PnsPipeCreateResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, pipeHandle, errorCode );

        // Check PIPE Error Code
        if ( PN_PIPE_ERR_GENERAL == errorCode )
            {
            TUint8 pep1ErrorCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PEP1ERRORCODE ) );
            TUint8 pep2ErrorCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PEP2ERRORCODE ) );

             // Check PEP Error Codes
           if ( pep1ErrorCode == PN_PIPE_ERR_PEP_IN_USE ||
                 pep1ErrorCode == PN_PIPE_ERR_ALL_PIPES_IN_USE ||
                 pep2ErrorCode == PN_PIPE_ERR_PEP_IN_USE ||
                 pep2ErrorCode == PN_PIPE_ERR_ALL_PIPES_IN_USE )
                {

                TFLOGSTRING("TSY: CMmPipeControl::PnsPipeCreateResp. ErrorCode changed to PN_PIPE_ERR_ALL_PIPES_IN_USE" );
OstTrace0( TRACE_NORMAL,  DUP2_CMMPIPECONTROL_PNSPIPECREATERESP_TD, "CMmPipeControl::PnsPipeCreateResp, ErrorCode changed to PN_PIPE_ERR_ALL_PIPES_IN_USE" );
                // PEP can't create more pipes (all pipes in use),
                // error value is updated here
                errorCode = PN_PIPE_ERR_ALL_PIPES_IN_USE;
                }
            //no else
            }
        //no else

        iContextMessHandler->CompletePipeOperation(
            PNS_PIPE_CREATE_RESP,
            transId,
            pipeHandle,
            errorCode );
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeEnableReq
// Construct a PNS_PIPE_ENABLE_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmPipeControl::PnsPipeEnableReq(
    TUint8 aPipeHandle,
    TUint8 aTransId ) const
    {
    TFLOGSTRING3("TSY: CMmPipeControl::PnsPipeEnableReq( aPipeHandle:0x%x, aTransId:0x%x )", aPipeHandle, aTransId );
    OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPEENABLEREQ_TD, "CMmPipeControl::PnsPipeEnableReq" );

    TInt ret( KErrServerBusy );
    // Check if Pipe operation is not outstanding. IsOperationCalled is EFalse
    if ( !IsOperationCalled( aPipeHandle ) )
        {
        // Create buffer for isi msg data
        TBuf8<SIZE_PNS_PIPE_ENABLE_REQ> data;
        data.Append( aPipeHandle );
        data.Append( KGpdsPadding );
        // Send Isi message via phonet
        ret = iPhoNetSender->Send(
            PN_PIPE, aTransId, PNS_PIPE_ENABLE_REQ, data );
        }
    //no else

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeEnableResp
// Breaks a PNS_PIPE_ENABLE_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPipeEnableResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmPipeControl::PnsPipeEnableResp");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPIPEENABLERESP_TD, "CMmPipeControl::PnsPipeEnableResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // pipe control handles only messages for NIF pipes
    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
        // Get Pipehandle from the ISI message
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_ENABLE_RESP_OFFSET_PIPEHANDLE ) );
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_ENABLE_RESP_OFFSET_ERRORCODE ) );

TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeEnableResp - traId: %d, PipeHandle: %d, ErrorCode: %d", transId, pipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPEENABLERESP_TD, "CMmPipeControl::PnsPipeEnableResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, pipeHandle, errorCode );

        iContextMessHandler->CompletePipeOperation(
            PNS_PIPE_ENABLE_RESP,
            transId,
            pipeHandle,
            errorCode );
        }
    // no else
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeResetReq
// Construct a PNS_PIPE_RESET_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmPipeControl::PnsPipeResetReq(
    const TUint8 aTransId,
    const TUint8 aPipeHandle,
    const TUint8 aStateAfterReset )
    {
TFLOGSTRING2("TSY: CMmPipeControl::PnsPipeResetReq. PipeHandle: %d", aPipeHandle );
OstTraceExt1( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPERESETREQ_TD, "CMmPipeControl::PnsPipeResetReq;aPipeHandle=%hhu", aPipeHandle );

    // Set ret to KErrServerBusy
    TInt ret( KErrServerBusy );

    // Check if Pipe operation is not outstanding. IsOperationCalled is EFalse
    if ( !IsOperationCalled( aPipeHandle ) )
        {
        // Create buffer for isi msg data
        TBuf8<SIZE_PNS_PIPE_RESET_REQ> data;

        data.Append( aPipeHandle );
        data.Append( aStateAfterReset );

        // Send Isi message via phonet
        ret = iPhoNetSender->Send(
            PN_PIPE, aTransId, PNS_PIPE_RESET_REQ, data );

        if ( KErrNone == ret )
            {
            SetPipeHandle( aPipeHandle );
            }
        //no else
        }
    //no else

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeResetResp
// Breaks a PNS_PIPE_RESET_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPipeResetResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmPipeControl::PnsPipeResetResp");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPIPECONTROL_PNSPIPERESETRESP_TD, "CMmPipeControl::PnsPipeResetResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // pipe control handles only messages for NIF pipes
    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
        // Get Pipehandle from the ISI message
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_RESET_RESP_OFFSET_PIPEHANDLE ) );

TFLOGSTRING3("TSY: CMmPipeControl::PnsPipeResetResp - traId: %d, pipe handle: %d", transId, pipeHandle);
OstTraceExt2( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPERESETRESP_TD, "CMmPipeControl::PnsPipeResetResp;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

        // Call TransactionId
        TUint8 removeTransId( TransactionId( pipeHandle ) );
        // Call ResetPipeHandle -method from CMmPipeControl
        ResetPipeHandle( pipeHandle );

        if ( KUnknownTransID != removeTransId )
            {
            // Context has called the PnsPipeRemoveReq that was buffered
            PnsPipeRemoveReq( removeTransId, pipeHandle );
            }
        //no else

        // Check req is valid by Transaction id
        TInt ret( iContextList->CheckAndResetPipeResetTraId( transId ) );

        if ( KErrNone == ret )
            {
            TUint8 errorCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + PNS_PIPE_RESET_RESP_OFFSET_ERRORCODE ) );

TFLOGSTRING3("TSY: CMmPipeControl::PnsPipeResetResp. PipeHandle: %d ErrorCode: %d", pipeHandle, errorCode );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPIPERESETRESP_TD, "CMmPipeControl::PnsPipeResetResp;pipeHandle=%hhu;errorCode=%hhu", pipeHandle, errorCode );
            iContextMessHandler->CompletePipeOperation(
                PNS_PIPE_RESET_RESP,
                transId,
                pipeHandle,
                errorCode );
            }
        //no else
        }
    // no else
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeRemoveReq
// Construct a PNS_PIPE_REMOVE_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmPipeControl::PnsPipeRemoveReq(
    const TUint8 aTransId,
    const TUint8 aPipeHandle )
    {
TFLOGSTRING2("TSY: CMmPipeControl::PnsPipeRemoveReq. PipeHandle: %d", aPipeHandle );
OstTraceExt1( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPEREMOVEREQ_TD, "CMmPipeControl::PnsPipeRemoveReq;aPipeHandle=%hhu", aPipeHandle );

    // Set ret to KErrNone
    TInt ret( KErrNone );

    // Check if Pipe operation is not outstanding. IsOperationCalled is EFalse
    if ( !IsOperationCalled( aPipeHandle ) )
        {
        // Create buffer for isi msg data
        TBuf8<SIZE_PNS_PIPE_REMOVE_REQ> data;
        data.Append( aPipeHandle );
        data.Append( KGpdsPadding );

        // Send Isi message via phonet
        ret = iPhoNetSender->Send(
            PN_PIPE,
            aTransId,
            PNS_PIPE_REMOVE_REQ,
            data );

        if ( KErrNone == ret )
            {
            SetPipeHandle( aPipeHandle );
            }
        //no else
        }
    else
        {
        // Remove req is buffered by transaction id
        SetTransactionId( aPipeHandle, aTransId );
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeRemoveResp
// Breaks a PNS_PIPE_REMOVE_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPipeRemoveResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmPipeControl::PnsPipeRemoveResp");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPIPEREMOVERESP_TD, "CMmPipeControl::PnsPipeRemoveResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // pipe control handles only messages for NIF pipes
    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
        // Get Pipehandle from the ISI message
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_REMOVE_RESP_OFFSET_PIPEHANDLE ) );
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_REMOVE_RESP_OFFSET_ERRORCODE ) );

        TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeRemoveResp - traId: %d, PipeHandle: %d, ErrorCode: %d", transId, pipeHandle, errorCode );
    OstTraceExt3( TRACE_NORMAL,  CMMPIPECONTROL_PNSPIPEREMOVERESP_TD, "CMmPipeControl::PnsPipeRemoveResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, pipeHandle, errorCode );

        // Call ResetPipeHandle -method from CMmPipeControl
        ResetPipeHandle( pipeHandle );

        iContextMessHandler->CompletePipeOperation(
            PNS_PIPE_REMOVE_RESP,
            transId,
            pipeHandle,
            errorCode );
        }
    // no else
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeRedirectReq
// Construct a PNS_PIPE_REDIRECT_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmPipeControl::PnsPipeRedirectReq(
    TUint8 aTransId,
    TUint8 aPipeHandle,
    TUint8 aOldPepDevId,
    TUint8 aOldPepObjId,
    TUint8 aReplacementPepDevId,
    TUint8 aReplacementPepObjId)
    {
    TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeRedirectReq(aTransId:0x%x, aPipeHandle:0x%x, aOldPepDevId:0x%x )", aTransId, aPipeHandle, aOldPepDevId );
    TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeRedirectReq( aOldPepObjId:0x%x, aReplacementPepDevId:0x%x, aReplacementPepObjId:0x%x)", aOldPepObjId, aReplacementPepDevId, aReplacementPepObjId );
    OstTraceExt4( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPEREDIRECTREQ_TD, "CMmPipeControl::PnsPipeRedirectReq;aTransId=%hhu;aPipeHandle=%hhu;aOldPepDevId=%hhu;aOldPepObjId=%hhu", aTransId, aPipeHandle, aOldPepDevId, aOldPepObjId );
    OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPIPEREDIRECTREQ_TD, "CMmPipeControl::PnsPipeRedirectReq;aReplacementPepDevId=%hhu;aReplacementPepObjId=%hhu", aReplacementPepDevId, aReplacementPepObjId );

    TInt ret(KErrNone);

    TBuf8<SIZE_PNS_PIPE_REDIRECT_REQ> data;
    data.Append( aPipeHandle );
    data.Append( PN_PEP_DISABLE );
    data.Append( aOldPepDevId );
    data.Append( aOldPepObjId );
    data.Append( PN_PEP_TYPE_COMMON );
    data.Append( KGpdsPadding );
    data.Append( aReplacementPepDevId );
    data.Append( aReplacementPepObjId );
    data.Append( PN_PEP_TYPE_COMMON );
    data.Append( KGpdsPadding );

    // Send Isi message via phonet
    ret = iPhoNetSender->Send(
        PN_PIPE,
        aTransId,
        PNS_PIPE_REDIRECT_REQ,
        data );

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPipeRedirectResp
// Breaks a PNS_PIPE_REDIRECT_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPipeRedirectResp(
    const TIsiReceiveC& aIsiMessage )
    {
    TFLOGSTRING("TSY: CMmPipeControl::PnsPipeRedirectResp");
    OstTrace0( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPIPEREDIRECTRESP_TD, "CMmPipeControl::PnsPipeRedirectResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE ) );
    // Get Errorcode from the ISI message
    TUint8 errorCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_RESP_OFFSET_ERRORCODE ) );

    TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeRedirectResp - transId: 0x%x, PipeHandle: 0x%x, ErrorCode: 0x%x", transId, pipeHandle, errorCode );
    OstTraceExt3( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPEREDIRECTRESP_TD, "CMmPipeControl::PnsPipeRedirectResp;transId=%u;pipeHandle=%u;errorCode=%u", transId, pipeHandle, errorCode );

    iContextMessHandler->CompletePipeOperation(
        PNS_PIPE_REDIRECT_RESP,
        transId,
        pipeHandle,
        errorCode );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepCtrlReq
// Construct a PNS_PIPE_REDIRECT_REQ ISI-message.
// ----------------------------------------------------------------------------
//
TInt CMmPipeControl::PnsPepCtrlReq(
    TUint8 aDevId,
    TUint8 aObjId,
    TUint8 aTransId,
    TUint8 aPipeHandle,
    TUint8 aControlId,
    TUint8 aStatus,
    TUint8 aChannelId )
    {
    TFLOGSTRING4("TSY: CMmPipeControl::PnsPepCtrlReq( aTransId:0x%x, aPipeHandle:0x%x, aControlId:0x%x)", aTransId, aPipeHandle, aControlId );
    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepCtrlReq( aStatus:0x%x, aChannelId:0x%x)", aStatus, aChannelId );
    OstTraceExt5( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPCTRLREQ_TD, "CMmPipeControl::PnsPepCtrlReq;aTransId=%hhu;aPipeHandle=%hhu;aControlId=%hhu;aStatus=%hhu;aChannelId=%hhu", aTransId, aPipeHandle, aControlId, aStatus, aChannelId );
    
    TInt ret(KErrNone);
        
    TInt length( ISI_HEADER_SIZE + SIZE_PNS_PEP_CTRL_REQ + 7 );
    HBufC8* message = HBufC8::New( length );
    if( message )
        {
        TPtr8 messageptr = message->Des();
        TIsiSend isimessage( messageptr, length );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, aDevId );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, aObjId );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_UTID, aTransId );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_SUBFUNCTION, PNS_PEP_CTRL_REQ );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PIPEHANDLE , aPipeHandle );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPTYPE , PN_PEP_TYPE_BCA );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLID, aControlId );
        
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA, aStatus );
        
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + 1, aChannelId );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + 2, PN_PEP_SUB_TYPE_SYMBIAN_INTERNAL );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + 3, PN_PEP_TYPE_COMMON );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + 4, PN_PIPE );
        
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + 5, aDevId );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + 6, aObjId );

        ret = iPhoNetSender->Send( isimessage.Complete() );

        delete message;
        }
          
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepCtrlResp
// Breaks a PNS_PEP_CTRL_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepCtrlResp(
    const TIsiReceiveC& aIsiMessage )
    {
    TFLOGSTRING("TSY: CMmPipeControl::PnsPepCtrlResp");
    OstTrace0( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPEPCTRLRESP_TD, "CMmPipeControl::PnsPepCtrlResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_PIPEHANDLE ) );
    // Get Errorcode from the ISI message
    TUint8 errorCode( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_CTRL_RESP_OFFSET_ERRORCODE ) );

    TFLOGSTRING4("TSY: CMmPipeControl::PnsPepCtrlResp - transId: %d, PipeHandle: %d, ErrorCode: %d", transId, pipeHandle, errorCode );
    OstTraceExt3( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPCTRLRESP_TD, "CMmPipeControl::PnsPepCtrlResp;transId=%x;pipeHandle=%x;errorCode=%x", transId, pipeHandle, errorCode );

    iContextMessHandler->CompletePipeOperation(
        PNS_PEP_CTRL_RESP,
        transId,
        pipeHandle,
        errorCode );
    }

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepConnectReq
// Breaks a PNS_PEP_CONNECT_REQ ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepConnectReq(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPipeControl::PnsPepConnectReq");
OstTrace0( TRACE_NORMAL,  CMMPIPECONTROL_PNSPEPCONNECTREQ_TD, "CMmPipeControl::PnsPepConnectReq" );

    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    // Get Pipehandle from the ISI message
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepConnectReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPEPCONNECTREQ_TD, "CMmPipeControl::PnsPepConnectReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

    PnsPepConnectResp( transId, pipeHandle );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepConnectResp
// Construct a PNS_PEP_CONNECT_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepConnectResp(
    const TUint8 aTransId,
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepConnectResp - traId: %d, pipe handle: %d", aTransId, aPipeHandle);
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPEPCONNECTRESP_TD, "CMmPipeControl::PnsPepConnectResp;aTransId=%hhu;aPipeHandle=%hhu", aTransId, aPipeHandle );

    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_CONNECT_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_PNS_PEP_CONNECT_RESP> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
   // Set message data
    data.Append( aPipeHandle );
    data.Append( PN_PIPE_NO_ERROR );

    // Send Isi message via phonet
    TInt ret( iPhoNetSender->Send(
        PN_PIPE,
        aTransId,
        PNS_PEP_CONNECT_RESP,
        data ) );

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepConnectResp. PipeHandle: %d RetFromPhonetSender: %d", aPipeHandle, ret );

OstTraceExt2( TRACE_NORMAL,  DUP2_CMMPIPECONTROL_PNSPEPCONNECTRESP_TD, "CMmPipeControl::PnsPepConnectResp;aPipeHandle=%hhu;RetFromPhonetSender=%d", aPipeHandle, ret );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepDisconnectReq
// Breaks a PNS_PEP_DISCONNECT_REQ ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepDisconnectReq(
    const TIsiReceiveC& aIsiMessage )
    {
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    // Get Pipehandle from the ISI message
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_DISCONNECT_REQ_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_DISCONNECT_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepDisconnectReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL,  CMMPIPECONTROL_PNSPEPDISCONNECTREQ_TD, "CMmPipeControl::PnsPepDisconnectReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

    PnsPepDisconnectResp( transId, pipeHandle );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepDisconnectResp
// Construct a PNS_PEP_DISCONNECT_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepDisconnectResp(
    const TUint8 aTransId,
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepDisconnectResp - traId: %d, pipe handle: %d", aTransId, aPipeHandle);
OstTraceExt2( TRACE_NORMAL,  CMMPIPECONTROL_PNSPEPDISCONNECTRESP_TD, "CMmPipeControl::PnsPepDisconnectResp;aTransId=%hhu;aPipeHandle=%hhu", aTransId, aPipeHandle );

    //Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_DISCONNECT_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_PNS_PEP_DISCONNECT_RESP> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // Set message data
    data.Append( aPipeHandle );
    data.Append( PN_PIPE_NO_ERROR );

    // Send Isi message via phonet
    TInt ret( iPhoNetSender->Send(
        PN_PIPE, aTransId, PNS_PEP_DISCONNECT_RESP, data ) );

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepDisconnectResp. PipeHandle: %d RetFromPhonetSender: %d", aPipeHandle, ret );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPEPDISCONNECTRESP_TD, "CMmPipeControl::PnsPepDisconnectResp;aPipeHandle=%hhu;RetFromPhonetSender=%d", aPipeHandle, ret );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepResetReq
// Breaks a PNS_PEP_RESET_REQ ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepResetReq(
    const TIsiReceiveC& aIsiMessage )
    {
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    // Get Pipehandle from the ISI message
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_RESET_REQ_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_RESET_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepResetReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL,  CMMPIPECONTROL_PNSPEPRESETREQ_TD, "CMmPipeControl::PnsPepResetReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

    PnsPepResetResp( transId, pipeHandle );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepResetResp
// Construct a PNS_PEP_RESET_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepResetResp(
    const TUint8 aTransId,
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepResetResp - traId: %d, pipe handle: %d", aTransId, aPipeHandle);
OstTraceExt1( TRACE_NORMAL,  CMMPIPECONTROL_PNSPEPRESETRESP_TD, "CMmPipeControl::PnsPepResetResp;aTransId=%hhu", aTransId );

    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_RESET_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_PNS_PEP_RESET_RESP> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // Set message data
    data.Append( aPipeHandle );
    data.Append( PN_PIPE_NO_ERROR );

    // Send Isi message via phonet
    TInt ret( iPhoNetSender->Send(
        PN_PIPE, aTransId, PNS_PEP_RESET_RESP, data ) );

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepResetResp. PipeHandle: %d RetFromPhonetSender: %d", aPipeHandle, ret );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPEPRESETRESP_TD, "CMmPipeControl::PnsPepResetResp;aPipeHandle=%hhu;ret=%d", aPipeHandle, ret );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepEnableReq
// Breaks a PNS_PEP_ENABLE_REQ ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepEnableReq(
    const TIsiReceiveC& aIsiMessage )
    {

    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    // Get Pipehandle from the ISI message
    TUint8 pipeHandle( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        ISI_HEADER_SIZE + PNS_PEP_ENABLE_REQ_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        ISI_HEADER_SIZE + PNS_PEP_ENABLE_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepEnableReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL,  CMMPIPECONTROL_PNSPEPENABLEREQ_TD, "CMmPipeControl::PnsPepEnableReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

    PnsPepEnableResp( transId, pipeHandle );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepEnableResp
// Construct a PNS_PEP_ENABLE_RESP ISI-message.
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsPepEnableResp(
    const TUint8 aTransId,
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepEnableResp - traId: %d, pipe handle: %d", aTransId, aPipeHandle);
OstTraceExt2( TRACE_NORMAL,  CMMPIPECONTROL_PNSPEPENABLERESP_TD, "CMmPipeControl::PnsPepEnableResp;aTransId=%hhu;aPipeHandle=%hhu", aTransId, aPipeHandle );

    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_ENABLE_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_PNS_PEP_ENABLE_RESP> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // Set message data
    data.Append( aPipeHandle );
    data.Append( PN_PIPE_NO_ERROR );

    // Send Isi message via phonet
    TInt ret( iPhoNetSender->Send(
        PN_PIPE,
        aTransId,
        PNS_PEP_ENABLE_RESP,
        data ) );

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepEnableResp. PipeHandle: %d RetFromPhonetSender: %d", aPipeHandle, ret );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMPIPECONTROL_PNSPEPENABLERESP_TD, "CMmPipeControl::PnsPepEnableResp;aPipeHandle=%hhu;RetFromPhonetSender=%d", aPipeHandle, ret );
    }

#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

// ================= OTHER EXPORTED FUNCTIONS ===============================

    //None

//  End of File
