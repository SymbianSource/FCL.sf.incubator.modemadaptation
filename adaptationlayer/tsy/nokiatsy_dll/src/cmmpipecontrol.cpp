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
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
#include <nsisi.h>
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include "tsylogger.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpipecontroltraces.h"
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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_CMMPIPECONTROL, "CMmPipeControl::CMmPipeControl" );
    }

// -----------------------------------------------------------------------------
// CMmPipeControl::~CMmPipeControl
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmPipeControl::~CMmPipeControl()
    {

    TFLOGSTRING("TSY: CMmPipeControl::~CMmPipeControl");
OstTrace0( TRACE_NORMAL, DUP1_CMMPIPECONTROL_CMMPIPECONTROL, "CMmPipeControl::~CMmPipeControl" );
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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_NEWL, "CMmPipeControl::NewL" );

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

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_CONNECT_REQ );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_DISCONNECT_REQ );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_RESET_REQ );
    aPhoNetReceiver->RegisterL( mmPipeControl, PN_PIPE, PNS_PEP_ENABLE_REQ );

#endif //DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
   aPhoNetReceiver->
        RegisterL( mmPipeControl, PN_NAMESERVICE, PNS_NAME_QUERY_RESP );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_CONSTRUCTL, "CMmPipeControl::ConstructL" );

    iSecondPepDeviceId = KSecondPepDevice; // Default Device Id of second PEP
    iSecondPepObjectId = KSecondPepObject; // Default Object Id of second PEP

    InitLists();
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
   PnsNameQueryReq();
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
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
OstTraceExt2( TRACE_NORMAL, CMMPIPECONTROL_RECEIVEMESSAGEL, "CMmPipeControl::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

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
OstTrace1( TRACE_NORMAL, DUP1_CMMPIPECONTROL_RECEIVEMESSAGEL, "CMmPipeControl::ReceiveMessageL;unknown messageId=%d", messageId );
                    break;
                    }
                }
            break;
            }
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case PN_NAMESERVICE:
            {
            switch( messageId )
                {
                case PNS_NAME_QUERY_RESP:
                    {
                    PnsNameQueryResp( aIsiMessage );
                    break;
                    }
                default:
                    {

                    TFLOGSTRING2("TSY: CMmPipeControl::ReceiveMessageL - PN_NAMESERVICE - unknown msgId: %d", messageId);
OstTrace1( TRACE_NORMAL, DUP2_CMMPIPECONTROL_RECEIVEMESSAGEL, "CMmPipeControl::ReceiveMessageL;unknown messageId=%d", messageId );
                    break;
                    }
                }
            break;
            }
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        default:
            {

            TFLOGSTRING2("TSY: CMmPipeControl::ReceiveMessageL - unknown resource: %d", resource);
OstTrace1( TRACE_NORMAL, DUP3_CMMPIPECONTROL_RECEIVEMESSAGEL, "CMmPipeControl::ReceiveMessageL;unknown resource=%d", resource );
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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_INITLISTS, "CMmPipeControl::InitLists" );

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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_SETPIPEHANDLE, "CMmPipeControl::SetPipeHandle" );

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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_SETTRANSACTIONID, "CMmPipeControl::SetTransactionId" );

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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_TRANSACTIONID, "CMmPipeControl::TransactionId" );

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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_RESETPIPEHANDLE, "CMmPipeControl::ResetPipeHandle" );

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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_ISOPERATIONCALLED, "CMmPipeControl::IsOperationCalled" );

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

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
// ----------------------------------------------------------------------------
// CMmPipeControl::PnsNameQueryReq
// Construct a PNS_NAME_QUERY_REQ ISI-message
// ----------------------------------------------------------------------------
//
TInt CMmPipeControl::PnsNameQueryReq() const
    {

    TFLOGSTRING("TSY: CMmPipeControl::PnsNameQueryReq");
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_PNSNAMEQUERYREQ, "CMmPipeControl::PnsNameQueryReq" );

    // create buffer for isi msg data
    TBuf8<SIZE_PNS_NAME_QUERY_REQ> data;
    // set message data
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( PN_GPRS  ); // Gprs server
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( KGpdsPadding ); // Padding byte
    data.Append( PN_GPRS  ); // Gprs server

    // set dummyTransactionId value to 0x0F
    TUint8 dummyTransactionId( KDummyTrId );

    // Sending message to phonet
    return iPhoNetSender->Send(
        PN_NAMESERVICE, dummyTransactionId, PNS_NAME_QUERY_REQ, data );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsNameQueryResp
// Breaks a PNS_NAME_QUERY_RESP ISI-message
// ----------------------------------------------------------------------------
//
void CMmPipeControl::PnsNameQueryResp(
    const TIsiReceiveC& aIsiMessage )
    {

    TUint16 matchesInThisMsg( aIsiMessage.Get16bit(
        ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_MATCHESINMSG ) );

    TFLOGSTRING2("TSY: CMmPipeControl::PnsNameQueryResp - matches in this msg: %d", matchesInThisMsg);
OstTraceExt1( TRACE_NORMAL, CMMPIPECONTROL_PNSNAMEQUERYRESP, "CMmPipeControl::PnsNameQueryResp;matchesInThisMsg=%hu", matchesInThisMsg );

    if ( 0 < matchesInThisMsg )
        {
        for ( TInt i = 0; i < matchesInThisMsg; i++ )
            {
            // each match is represented as 8-byte record =>
            // recordIndex = i * SIZE_PN_NAME_SRV_ITEM_STR
            // name is 4 bytes =>
            // last byte index = PN_NAME_SRV_ITEM_STR_OFFSET_NAME + KLastByteIndex
            TInt recordIndex( i * SIZE_PN_NAME_SRV_ITEM_STR );
            TUint8 name( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_NAMEENTRYTBL +
                recordIndex + ( PN_NAME_SRV_ITEM_STR_OFFSET_NAME + KLastByteIndex ) ) );
            if ( PN_GPRS == name )
                {
                // Get and store SecondPepDevice id and SecondPepObject id
                iSecondPepDeviceId = aIsiMessage.Get8bit(
                    ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_NAMEENTRYTBL +
                    recordIndex + PN_NAME_SRV_ITEM_STR_OFFSET_DEV );
                iSecondPepObjectId = aIsiMessage.Get8bit(
                    ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_NAMEENTRYTBL +
                    recordIndex + PN_NAME_SRV_ITEM_STR_OFFSET_OBJ );
                // then find right server, stop finding
                i = matchesInThisMsg;

                TFLOGSTRING3("TSY: CMmPipeControl::PnsNameQueryResp. SecondPepDeviceId: %d SecondPepObjectId: %d", iSecondPepDeviceId, iSecondPepObjectId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSNAMEQUERYRESP, "CMmPipeControl::PnsNameQueryResp;iSecondPepDeviceId=%hhu;iSecondPepObjectId=%hhu", iSecondPepDeviceId, iSecondPepObjectId );

                }
            //no else
            }
        }
    //no else
    }

#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

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
OstTraceExt1( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPECREATEREQ, "CMmPipeControl::PnsPipeCreateReq;aTransId=%hhu", aTransId );

    // create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PIPE_CREATE_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PIPE_CREATE_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPECREATERESP, "CMmPipeControl::PnsPipeCreateResp" );

    // Get transaction Id
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE ) );
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE ) );
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

        TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeCreateResp. TransactionId: %d, PipeHandle: %d, error code: %d", transId, pipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPIPECREATERESP, "CMmPipeControl::PnsPipeCreateResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, pipeHandle, errorCode );

        // Check PIPE Error Code
        if ( PN_PIPE_ERR_GENERAL == errorCode )
            {
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            TUint8 pep1ErrorCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PEP1ERRORCODE ) );
            TUint8 pep2ErrorCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + PNS_PIPE_CREATE_RESP_OFFSET_PEP2ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            TUint8 pep1ErrorCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_PEP1ERRORCODE ) );
            TUint8 pep2ErrorCode( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + CM_PIPE_CREATE_RESP_OFFSET_PEP2ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
             // Check PEP Error Codes
           if ( pep1ErrorCode == PN_PIPE_ERR_PEP_IN_USE ||
                 pep1ErrorCode == PN_PIPE_ERR_ALL_PIPES_IN_USE ||
                 pep2ErrorCode == PN_PIPE_ERR_PEP_IN_USE ||
                 pep2ErrorCode == PN_PIPE_ERR_ALL_PIPES_IN_USE )
                {

                TFLOGSTRING("TSY: CMmPipeControl::PnsPipeCreateResp. ErrorCode changed to PN_PIPE_ERR_ALL_PIPES_IN_USE" );
OstTrace0( TRACE_NORMAL, DUP2_CMMPIPECONTROL_PNSPIPECREATERESP, "CMmPipeControl::PnsPipeCreateResp, ErrorCode changed to PN_PIPE_ERR_ALL_PIPES_IN_USE" );
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
    const CMmDataPackage& aPackage ) const
    {
TFLOGSTRING("TSY: CMmPipeControl::PnsPipeEnableReq");
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPEENABLEREQ, "CMmPipeControl::PnsPipeEnableReq" );

    TInfoName* contextName = NULL;
    aPackage.UnPackData( &contextName );

    TInt ret( KErrServerBusy );

    if( NULL != contextName )
        {

        TFLOGSTRING2("TSY: CMmPipeControl::PnsPipeEnableReq - context name: %S", contextName);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPIPEENABLEREQ, "CMmPipeControl::PnsPipeEnableReq;contextName=%S", *contextName );

        TUint8 pipehandle( iContextList->PipeHandleByContextName(contextName) );

        TUint8 traId( iContextList->ProxyIdByContextName( contextName ) );

        TFLOGSTRING2("TSY: CMmPipeControl::PnsPipeEnableReq. PipeHandle: %d", pipehandle );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPIPECONTROL_PNSPIPEENABLEREQ, "CMmPipeControl::PnsPipeEnableReq;pipehandle=%hhu", pipehandle );

        // Check if Pipe operation is not outstanding. IsOperationCalled is EFalse
        if ( !IsOperationCalled( pipehandle ) )
            {
            // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            TBuf8<SIZE_PNS_PIPE_ENABLE_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            TBuf8<SIZE_CM_PIPE_ENABLE_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            data.Append( pipehandle );
            data.Append( KGpdsPadding );

            // Send Isi message via phonet
            ret = iPhoNetSender->Send(
                PN_PIPE, traId, PNS_PIPE_ENABLE_REQ, data );
            }
        //no else
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
OstTrace0( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPIPEENABLERESP, "CMmPipeControl::PnsPipeEnableResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // pipe control handles only messages for NIF pipes
    // TODO: See TelAd-110
    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
        // Get Pipehandle from the ISI message
        TUint8 pipeHandle( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_ENABLE_RESP_OFFSET_PIPEHANDLE ) );
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_ENABLE_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_ENABLE_RESP_OFFSET_PIPEHANDLE ) );
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CM_PIPE_ENABLE_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeEnableResp - traId: %d, PipeHandle: %d, ErrorCode: %d", transId, pipeHandle, errorCode );
OstTraceExt3( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPEENABLERESP, "CMmPipeControl::PnsPipeEnableResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, pipeHandle, errorCode );

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
OstTraceExt1( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPERESETREQ, "CMmPipeControl::PnsPipeResetReq;aPipeHandle=%hhu", aPipeHandle );

    // Set ret to KErrServerBusy
    TInt ret( KErrServerBusy );

    // Check if Pipe operation is not outstanding. IsOperationCalled is EFalse
    if ( !IsOperationCalled( aPipeHandle ) )
        {
        // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        TBuf8<SIZE_PNS_PIPE_RESET_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        TBuf8<SIZE_CM_PIPE_RESET_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
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
OstTrace0( TRACE_NORMAL, DUP2_CMMPIPECONTROL_PNSPIPERESETRESP, "CMmPipeControl::PnsPipeResetResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // pipe control handles only messages for NIF pipes
    // TODO: See TelAd-110
    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
        // Get Pipehandle from the ISI message
        TUint8 pipeHandle( aIsiMessage.Get8bit(
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            ISI_HEADER_SIZE + PNS_PIPE_RESET_RESP_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            ISI_HEADER_SIZE + CM_PIPE_RESET_RESP_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING3("TSY: CMmPipeControl::PnsPipeResetResp - traId: %d, pipe handle: %d", transId, pipeHandle);
OstTraceExt2( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPERESETRESP, "CMmPipeControl::PnsPipeResetResp;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

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
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
                ISI_HEADER_SIZE + PNS_PIPE_RESET_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
                ISI_HEADER_SIZE + CM_PIPE_RESET_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

TFLOGSTRING3("TSY: CMmPipeControl::PnsPipeResetResp. PipeHandle: %d ErrorCode: %d", pipeHandle, errorCode );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPIPERESETRESP, "CMmPipeControl::PnsPipeResetResp;pipeHandle=%hhu;errorCode=%hhu", pipeHandle, errorCode );
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
OstTraceExt1( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPEREMOVEREQ, "CMmPipeControl::PnsPipeRemoveReq;aPipeHandle=%hhu", aPipeHandle );

    // Set ret to KErrNone
    TInt ret( KErrNone );

    // Check if Pipe operation is not outstanding. IsOperationCalled is EFalse
    if ( !IsOperationCalled( aPipeHandle ) )
        {
        // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        TBuf8<SIZE_PNS_PIPE_REMOVE_REQ> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        TBuf8<SIZE_CM_PIPE_REMOVE_REQ> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
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
OstTrace0( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPIPEREMOVERESP, "CMmPipeControl::PnsPipeRemoveResp" );
    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );

    // pipe control handles only messages for NIF pipes
    // TODO: See TelAd-110
    TInt ret = iContextList->PipeCreateTransIdOkByTransId( transId );

    if ( KErrNone == ret )
        {
        // Get Pipehandle from the ISI message
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_REMOVE_RESP_OFFSET_PIPEHANDLE ) );
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + PNS_PIPE_REMOVE_RESP_OFFSET_ERRORCODE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        TUint8 pipeHandle( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CM_PIPE_REMOVE_RESP_OFFSET_PIPEHANDLE ) );
        // Get Errorcode from the ISI message
        TUint8 errorCode( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + CM_PIPE_REMOVE_RESP_OFFSET_ERRORCODE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

        TFLOGSTRING4("TSY: CMmPipeControl::PnsPipeRemoveResp - traId: %d, PipeHandle: %d, ErrorCode: %d", transId, pipeHandle, errorCode );
    OstTraceExt3( TRACE_NORMAL, CMMPIPECONTROL_PNSPIPEREMOVERESP, "CMmPipeControl::PnsPipeRemoveResp;transId=%hhu;pipeHandle=%hhu;errorCode=%hhu", transId, pipeHandle, errorCode );

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
OstTrace0( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPCONNECTREQ, "CMmPipeControl::PnsPepConnectReq" );

    // Get Transaction Id from the ISI message
    TUint8 transId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    // Get Pipehandle from the ISI message
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE ) );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TUint8 pipeHandle( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CM_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepConnectReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPEPCONNECTREQ, "CMmPipeControl::PnsPepConnectReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

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
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPEPCONNECTRESP, "CMmPipeControl::PnsPepConnectResp;aTransId=%hhu;aPipeHandle=%hhu", aTransId, aPipeHandle );

    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_CONNECT_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PEP_CONNECT_RESP> data;
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

OstTraceExt2( TRACE_NORMAL, DUP2_CMMPIPECONTROL_PNSPEPCONNECTRESP, "CMmPipeControl::PnsPepConnectResp;aPipeHandle=%hhu;RetFromPhonetSender=%d", aPipeHandle, ret );
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
        ISI_HEADER_SIZE + CM_PEP_DISCONNECT_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepDisconnectReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPDISCONNECTREQ, "CMmPipeControl::PnsPepDisconnectReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

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
OstTraceExt2( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPDISCONNECTRESP, "CMmPipeControl::PnsPepDisconnectResp;aTransId=%hhu;aPipeHandle=%hhu", aTransId, aPipeHandle );

    //Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_DISCONNECT_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PEP_DISCONNECT_RESP> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // Set message data
    data.Append( aPipeHandle );
    data.Append( PN_PIPE_NO_ERROR );

    // Send Isi message via phonet
    TInt ret( iPhoNetSender->Send(
        PN_PIPE, aTransId, PNS_PEP_DISCONNECT_RESP, data ) );

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepDisconnectResp. PipeHandle: %d RetFromPhonetSender: %d", aPipeHandle, ret );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPEPDISCONNECTRESP, "CMmPipeControl::PnsPepDisconnectResp;aPipeHandle=%hhu;RetFromPhonetSender=%d", aPipeHandle, ret );
    }

// ----------------------------------------------------------------------------
// CMmPipeControl::PnsPepResetReq
// Breaks a CM_PEP_RESET_REQ ISI-message.
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
        ISI_HEADER_SIZE + CM_PEP_RESET_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepResetReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPRESETREQ, "CMmPipeControl::PnsPepResetReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

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
OstTraceExt1( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPRESETRESP, "CMmPipeControl::PnsPepResetResp;aTransId=%hhu", aTransId );

    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_RESET_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PEP_RESET_RESP> data;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // Set message data
    data.Append( aPipeHandle );
    data.Append( PN_PIPE_NO_ERROR );

    // Send Isi message via phonet
    TInt ret( iPhoNetSender->Send(
        PN_PIPE, aTransId, PNS_PEP_RESET_RESP, data ) );

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepResetResp. PipeHandle: %d RetFromPhonetSender: %d", aPipeHandle, ret );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPEPRESETRESP, "CMmPipeControl::PnsPepResetResp;aPipeHandle=%hhu;ret=%d", aPipeHandle, ret );
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
        ISI_HEADER_SIZE + CM_PEP_ENABLE_REQ_OFFSET_PIPEHANDLE ) );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    TFLOGSTRING3("TSY: CMmPipeControl::PnsPepEnableReq - traId: %d, PipeHandle: %d", transId, pipeHandle );
OstTraceExt2( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPENABLEREQ, "CMmPipeControl::PnsPepEnableReq;transId=%hhu;pipeHandle=%hhu", transId, pipeHandle );

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
OstTraceExt2( TRACE_NORMAL, CMMPIPECONTROL_PNSPEPENABLERESP, "CMmPipeControl::PnsPepEnableResp;aTransId=%hhu;aPipeHandle=%hhu", aTransId, aPipeHandle );

    // Create buffer for isi msg data
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    TBuf8<SIZE_PNS_PEP_ENABLE_RESP> data;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    TBuf8<SIZE_CM_PEP_ENABLE_RESP> data;
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
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPIPECONTROL_PNSPEPENABLERESP, "CMmPipeControl::PnsPepEnableResp;aPipeHandle=%hhu;RetFromPhonetSender=%d", aPipeHandle, ret );
    }

#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

// ================= OTHER EXPORTED FUNCTIONS ===============================

    //None

//  End of File
