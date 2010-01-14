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



// INCLUDES
#include <in_sock.h>
#include <etelutils.h>
#include <etelmmerr.h>
#include "cmmmessagerouter.h"
#include "cmmpacketmesshandlercontextlist.h"
#include "cmmpacketservicemesshandler.h"
#include "cmmpacketcontextmesshandler.h"
#include "cmmpacketqosmesshandler.h"
#include "cmmphonetsender.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "cmmstaticutility.h"
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <pcktcs.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <tisi.h>
#include <gpdsisi.h>
#include "tsylogger.h"
#include <gpds_sharedisi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <call_modemisi.h>

#if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
#include <cmmcommonstaticutility.h>
#endif // (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpacketcontextmesshandlerTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS

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
// CMmPacketContextMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmPacketContextMessHandler* CMmPacketContextMessHandler::NewL(
    CMmPhoNetSender* const aPhoNetSender,
    CMmPhoNetReceiver* aPhoNetReceiver,
    CMmPacketContextMesshandlerList* aMmPacketContextMesshandlerList,
    CMmPacketQoSMessHandler* aMmPacketQoSMessHandler,
    CMmMessageRouter* aMessageRouter )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::NewL." );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_NEWL, "CMmPacketContextMessHandler::NewL" );

    CMmPacketContextMessHandler* packetContextMessHandler =
        new ( ELeave ) CMmPacketContextMessHandler();

    CleanupStack::PushL( packetContextMessHandler );

    packetContextMessHandler->iPhoNetSender = aPhoNetSender;
    packetContextMessHandler->iPhoNetReceiver = aPhoNetReceiver;
    packetContextMessHandler->iContextList = aMmPacketContextMesshandlerList;
    packetContextMessHandler->iMmPacketQoSMessHandler = aMmPacketQoSMessHandler;
    packetContextMessHandler->iMessageRouter = aMessageRouter;

    packetContextMessHandler->ConstructL();

    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_ID_CREATE_RESP );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_ID_CREATE_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_LL_CONFIGURE_RESP );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_CONFIGURE_RESP );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_AUTH_RESP );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_ACTIVATING_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_ACTIVATE_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_ACTIVATE_FAIL_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_MODIFY_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_DEACTIVATING_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_DEACTIVATE_RESP );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_DEACTIVATE_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_STATUS_RESP );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_STATUS_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_ID_DELETE_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_GPDS, GPDS_CONTEXT_MODIFY_RESP);

    // for allow incoming call feature
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_MODEM_CALL, CALL_MODEM_RESOURCE_IND );
    aPhoNetReceiver->RegisterL(
        packetContextMessHandler, PN_MODEM_CALL, CALL_MODEM_RESOURCE_RESP );

    CleanupStack::Pop( packetContextMessHandler );

    return packetContextMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::CMmPacketContextMessHandler
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMmPacketContextMessHandler::CMmPacketContextMessHandler()
    {
    //None
    TFLOGSTRING("TSY: CMmPacketContextMessHandler::CMmPacketContextMessHandler" );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_CMMPACKETCONTEXTMESSHANDLER, "CMmPacketContextMessHandler::CMmPacketContextMessHandler" );

    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::ConstructL()
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::ConstructL.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_CONSTRUCTL, "CMmPacketContextMessHandler::ConstructL" );

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
     iTransIdForProxies = 0;
#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

    // for allow incoming call feature
    iCallModemResourceCallId = CALL_MODEM_ID_NONE;
    iDeactivateContextList = NULL;

    // initialize DCM drive mode flag. Default value is EFalse.
    iDriveModeFlag = EFalse;

    // create pipe controller
    iMmPipeController = CMmPipeControl::NewL(
        iPhoNetSender, iPhoNetReceiver, this, iContextList );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::~CMmPacketContextMessHandler
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmPacketContextMessHandler::~CMmPacketContextMessHandler()
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::~CMmPacketContextMessHandler." );
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_CMMPACKETCONTEXTMESSHANDLER, "CMmPacketContextMessHandler::~CMmPacketContextMessHandler" );

    // Delete owned iMmPipeControl object
    if ( iMmPipeController )
        {
        delete iMmPipeController;
        }

    // for allow incoming call feature
    if ( iDeactivateContextList )
        {
        iDeactivateContextList->Reset();
        delete iDeactivateContextList;
        }

    // delete iResourceControlMsg if allocated
    if ( iResourceControlMsg  )
        {
        delete iResourceControlMsg;
        }

    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::ReceiveMessageL
// Entry point for messages received from Domestic OS.
// Switches the message to the correct method.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::ReceiveMessageL(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::ReceiveMessageL" );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketContextMessHandler::ReceiveMessageL" );

    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    switch ( resource )
        {
        case PN_GPDS:
            {
            switch ( messageId )
                {
                // Context creation
                case GPDS_CONTEXT_ID_CREATE_RESP:
                    {
                    GpdsContextIdCreateResp( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_ID_CREATE_IND:
                    {
                    GpdsContextIdCreateInd( aIsiMessage );
                    break;
                    }
                case GPDS_LL_CONFIGURE_RESP:
                    {
                    GpdsLlConfigureResp( aIsiMessage );
                    break;
                    }
                // Context configuration
                case GPDS_CONTEXT_CONFIGURE_RESP:
                    {
                    GpdsContextConfigureResp( aIsiMessage );
                    break;
                    }
                // Context authentication
                case GPDS_CONTEXT_AUTH_RESP:
                    {
                    GpdsContextAuthResp( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_ACTIVATING_IND:
                    {
                    GpdsContextActivatingInd( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_ACTIVATE_IND:
                    {
                    GpdsContextActivateIndL( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_ACTIVATE_FAIL_IND:
                    {
                    GpdsContextActivateFailInd( aIsiMessage );
                    break;
                    }
                // Context modifying
                case GPDS_CONTEXT_MODIFY_IND:
                    {
                    GpdsContextModifyInd( aIsiMessage );
                    break;
                    }
                // Context deactivation
                case GPDS_CONTEXT_DEACTIVATING_IND:
                    {
                    GpdsContextDeactivatingInd( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_DEACTIVATE_RESP:
                    {
                    GpdsContextDeactivateRespL( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_DEACTIVATE_IND:
                    {
                    GpdsContextDeactivateInd( aIsiMessage );
                    break;
                    }
                // Context status
                case GPDS_CONTEXT_STATUS_RESP:
                    {
                    GpdsContextStatusResp( aIsiMessage );
                    break;
                    }
                // Context deletion
                case GPDS_CONTEXT_STATUS_IND:
                    {
                    GpdsContextStatusInd( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_ID_DELETE_IND:
                    {
                    GpdsContextIdDeleteInd( aIsiMessage );
                    break;
                    }
                case GPDS_CONTEXT_MODIFY_RESP:
                    {
                    GpdsContextModifyResp( aIsiMessage );
                    }
                default:
                    {

                    TFLOGSTRING("TSY: CMmPacketContextMessHandler::ReceiveMessageL. Switch messageId case default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketContextMessHandler::ReceiveMessageL" );

                    //None
                    break;
                    }
                }
            break;
            }

        // for allow incoming call feature
        case PN_MODEM_CALL:
            {
            if ( CALL_MODEM_RESOURCE_IND == messageId )
                {
                CallModemResourceIndL( aIsiMessage );
                }
            else if ( CALL_MODEM_RESOURCE_RESP == messageId )
                {
                CallModemResourceResp( aIsiMessage );
                }
            //no else
            break;
            }

        default:
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::ReceiveMessageL. Switch resource case default");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketContextMessHandler::ReceiveMessageL" );

            //None
            break;
            }
        } // switch
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextIdCreateReq
// Create an Isi-message for creating a context id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::GpdsContextIdCreateReq(
    const TUint8 aCreateContextIdTransId ) const
    {

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextIdCreateReq. TransId: %d", aCreateContextIdTransId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTIDCREATEREQ, "CMmPacketContextMessHandler::GpdsContextIdCreateReq;aCreateContextIdTransId=%hhu", aCreateContextIdTransId );

    //Append data for ISI message
    TBuf8<1> data;
    // Send Isi message via Phonet
    return iPhoNetSender->Send(
        PN_GPDS,
        aCreateContextIdTransId,
        GPDS_CONTEXT_ID_CREATE_REQ,
        data );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextIdCreateResp
// This method breaks the response to the Gpds Context Id Create request.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextIdCreateResp(
    const TIsiReceiveC& aIsiMessage )
    {

    // Get transaction id
    TUint8 createContextIdTraId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_ID_CREATE_RESP_OFFSET_UTID ) );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextIdCreateResp.CreateTraId: %d", createContextIdTraId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTIDCREATERESP, "CMmPacketContextMessHandler::GpdsContextIdCreateResp;createContextIdTraId=%hhu", createContextIdTraId );

    // context stuff exists
    TInt ret = iContextList->CreateContextIdTraIdOK( createContextIdTraId );

    if ( KErrNone == ret )
        {
        // Get context id
        TUint8 contextId( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_ID_CREATE_RESP_OFFSET_CID ) );
        // Get status
        TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_ID_CREATE_RESP_OFFSET_STATUS ) );

        if ( GPDS_OK == status )
            {
            // Store the context id to contextList
            iContextList->SetContextId( contextId, createContextIdTraId );
            }
        else
            {
            InitialiseContextPhase3( status, contextId );
            }
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextIdCreateInd
// Breaks gpds context id create indication message.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextIdCreateInd(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING( "TSY: CMmPacketContextMessHandler::GpdsContextIdCreateInd." );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTIDCREATEIND, "CMmPacketContextMessHandler::GpdsContextIdCreateInd" );

    // Get context id
    TUint8 contextId(  aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_ID_CREATE_IND_OFFSET_CID ) );

    TInfoName mmContextName;

    TInt ret( iContextList->
        GetContextNameByContextId(contextId, mmContextName) );
    TInfoName hostCID( iContextList->GetHostCidName( &mmContextName ) );

    if( 0 != hostCID.Length() )
        {
        TInt hostId( iContextList->GetContextIdByContextName( &hostCID ) );
        if( GPDS_CID_VOID != hostId )
            {
            iContextList->SetInitialiseMember( contextId, ETrue );
            }
        //no else
        }
    //no else

    // Context stuff exists in list
    if ( KErrNone == ret )
        {
         // Creation of context id succeeded -> Call
         // InitialiseContextPhase3L with GPDS_OK
        InitialiseContextPhase3( GPDS_OK, contextId );
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsLlConfigureReq
// Create an Isi-message for configuring the local link.
// This function configures a local link between the context and the pipe.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::GpdsLlConfigureReq(
    const TUint8 aTransactionId,
    const TUint8 aContextId,
    const TUint8 aPipeHandle ) const
    {

    TFLOGSTRING3( "TSY: CMmPacketContextMessHandler::GpdsLlConfigureReq. TransId: %d, ContextId: %d", aTransactionId, aContextId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSLLCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsLlConfigureReq;aTransactionId=%hhu;aContextId=%hhu", aTransactionId, aContextId );
    TFLOGSTRING2( "TSY: CMmPacketContextMessHandler::GpdsLlConfigureReq. PipeHandle: %d", aPipeHandle );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSLLCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsLlConfigureReq;aPipeHandle=%hhu", aPipeHandle );

    // Set the message data that consists of three parameters
    TBuf8<SIZE_GPDS_LL_CONFIGURE_REQ> messageData;
    messageData.Append( aContextId );
    messageData.Append( aPipeHandle );
    // Packet tsy uses always Plain mode
    messageData.Append( GPDS_LL_PLAIN );

    // Send Isi message via Phonet
    return iPhoNetSender->Send(
        PN_GPDS,
        aTransactionId,
        GPDS_LL_CONFIGURE_REQ,
        messageData );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsLlConfigureResp
// This method breaks the response to the local link configuration request.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsLlConfigureResp(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING( "TSY: CMmPacketContextMessHandler::GpdsLlConfigureResp.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSLLCONFIGURERESP, "CMmPacketContextMessHandler::GpdsLlConfigureResp" );

    // Get transaction id
    TUint8 transActionId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_LL_CONFIGURE_RESP_OFFSET_UTID ) );

    if ( KErrNone == iContextList->GpdsLlConfigureTraIdOK( transActionId ) )
        {
        TUint8 contextId( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_LL_CONFIGURE_RESP_OFFSET_CID ) );

        transActionId = iContextList->
            CreateContextIdTraIdByContextId( contextId );

        if( KTUint8NotDefined != transActionId )
            {
            // Get status
            TUint8 status( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + GPDS_LL_CONFIGURE_RESP_OFFSET_STATUS ) );

            if( GPDS_OK == status )
                {
                // Get context name by context id
                TInfoName contextName; // just to check if context stuff exists
                TInt ret( iContextList->
                    GetContextNameByContextId( contextId, contextName ) );

                if( KErrNone == ret )
                    {
                    // If all is OK this far, complete Initialisation
                    CMmDataPackage data;
                    data.PackData( &contextName );
                    iMessageRouter->Complete(
                        EPacketContextInitialiseContext,
                        &data,
                        KErrNone );
                    }
                else if( KErrArgument == ret )
                    {

                    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsLlConfigureResp. Initialisation Complete failed" );
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSLLCONFIGURERESP, "CMmPacketContextMessHandler::GpdsLlConfigureResp, Initialisation Complete failed" );
                    }
                //no else
                }
            else
                {

                // create transaction Id for pipe removal
                // one TraId is used for one context
                TUint8 pipeRemoveTraId( transActionId );
                TUint8 pipeHandle = iContextList->
                    PipeHandleByContextId( contextId );
                // Remove the pipe
                TInt ret( iMmPipeController->
                    PnsPipeRemoveReq( pipeRemoveTraId, pipeHandle ) );

                if ( KErrNone == ret )
                    {
                    // Store the trans id
                    ret = iContextList->
                        SetRemovePipeTraId( pipeRemoveTraId, pipeHandle );
                    }
                else
                    {
                    // check if context stuff exists
                    TInfoName contextName;
                    ret = iContextList->
                        GetContextNameByContextId( contextId, contextName );

                    if( KErrNone == ret )
                        {
                        // Complete initialisation; some error in gprs server
                        CMmDataPackage data;
                        data.PackData( &contextName );
                        iMessageRouter->Complete(
                            EPacketContextInitialiseContext,
                            &data,
                            KErrGeneral );
                        }
                    }
                }
            }
        // no else
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextConfigureReq
// Create an Isi-message for context configuration.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::GpdsContextConfigureReq(
    const TUint8 aTransactionId,
    const TUint8 aContextId,
    const TUint8 aPdpType,
    const TUint8 aContextType,
    const TUint8 aPrimaryContextId,
    const TUint8 aDataCompression,
    const TUint8 aHeaderCompression,
    const TBuf8<KAccessPointBuffer252>& aAccessPointName,
    CArrayFixFlat<RPacketContext::TPacketFilterV2>* aPacketFilterPtr,
    RPacketContext::CTFTMediaAuthorizationV3* aMediaAuthorization,
    TBool aSignallingIndication,
    TBool aPCFCSAddressIndication,
    TBool aDNSAddressRequestIndication )
    {

    TFLOGSTRING3("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. TransId: %d ContextId: %d", aTransactionId, aContextId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsContextConfigureReq;aTransactionId=%hhu;aContextId=%hhu", aTransactionId, aContextId );

    // Initializing ret value to KErrNone
    TInt ret( KErrNone );

    // Create isi message
    TIsiSend gpdsContextConfigureReq( iPhoNetSender->SendBufferDes() );
    gpdsContextConfigureReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_GPDS );

    // Set the Gpds Context Configure Req data
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_OFFSET_TRANSID, aTransactionId );
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_OFFSET_MESSAGEID, GPDS_CONTEXT_CONFIGURE_REQ );
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_CID,
        aContextId );
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_PDPTYPE,
        aPdpType );
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_CONTEXTTYPE,
        aContextType );
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_PRIMARYCID,
        aPrimaryContextId );
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_FILLERBYTE1,
        KGpdsPadding );

    // Initialize message offset and subblock count
    TInt currentMsgOffset(
        ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_CONFIGURE_REQ );
    TUint8 numOfSbInMessage( 0 );

    // GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO (= 0xF7)
    // An OPTIONAL sub block of Configure Req.
    // Context for dedicated signaling purpose can be configured either using
    // REL5
    // QoS sub-block or IM CN Subsystem Signaling Flag sub-block.
    if( aSignallingIndication )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. signalling indication ON");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsContextConfigureReq, signalling indication ON" );

        TBuf8<SIZE_GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO> tempSbBuffer;
        TIsiSubBlock flagInfoSubBlock(
            tempSbBuffer,
            GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO,
            EIsiSubBlockTypeId8Len8 );

        tempSbBuffer.Append( GPDS_CTX_FOR_DEDICATED_SIGN );
        tempSbBuffer.Append( KGpdsPadding );
        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, flagInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + SIZE_GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO;
        numOfSbInMessage++;
        }
    //no else

    // GPDS_DNS_ADDRESS_REQ_INFO (= 0x90)
    // An OPTIONAL sub block of Configure Req
    if( aDNSAddressRequestIndication )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. DNS address request indication indication ON");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsContextConfigureReq, DNS address request indication indication ON" );

        TInt ret( iContextList->SetDNSAddressInfoToContextInfo( aContextId ) );

        if ( KErrNone == ret )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. DNS Requested");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsContextConfigureReq, DNS Requested" );

            TBuf8<SIZE_GPDS_DNS_ADDRESS_REQ_INFO> tempDnsAddrInfo;
            TIsiSubBlock reqInfoSubBlock(
                tempDnsAddrInfo,
                GPDS_DNS_ADDRESS_REQ_INFO,
                EIsiSubBlockTypeId8Len8 );

            tempDnsAddrInfo.Append( KGpdsPadding );
            tempDnsAddrInfo.Append( KGpdsPadding );
            gpdsContextConfigureReq.CopyData(
                currentMsgOffset, reqInfoSubBlock.CompleteSubBlock() );
            // Set new offset and increase subblock count
            currentMsgOffset =
                currentMsgOffset + SIZE_GPDS_DNS_ADDRESS_REQ_INFO;
            numOfSbInMessage++;
            }
        // no else
        }
    // no else

    // GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO (= 0xF3)
    // An OPTIONAL sub block of Configure Req
    if( aPCFCSAddressIndication )
        {
        TInt ret( iContextList->
            SetPCSCFAddressInfoToContextInfo( aContextId ) );

        if ( KErrNone == ret )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. PCSCF Requested");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsContextConfigureReq, PCSCF Requested" );

            TBuf8<SIZE_GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO> tempPscfAddrInfo;
            TIsiSubBlock reqInfoSubBlock(
                tempPscfAddrInfo,
                GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO,
                EIsiSubBlockTypeId8Len8 );

            tempPscfAddrInfo.Append( KGpdsPadding );
            tempPscfAddrInfo.Append( KGpdsPadding );
            gpdsContextConfigureReq.CopyData(
                currentMsgOffset, reqInfoSubBlock.CompleteSubBlock() );
            // Set new offset and increase subblock count
            currentMsgOffset =
                currentMsgOffset + SIZE_GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO;
            numOfSbInMessage++;
            }
        // no else
        }
    //no else

    // GPDS_TFT_INFO
    // An OPTIONAL sub block of Configure Req.
    if ( aMediaAuthorization || aPacketFilterPtr )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. MediaAuthorization || PacketFilterPtr");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsContextConfigureReq, MediaAuthorization || PacketFilterPtr" );

        TBuf8<KMaximumLengthOfGpdsTftInfo> gpdsTftInfoSbBuffer;
        TIsiSubBlock gpdsTftInfoSubBlock(
            gpdsTftInfoSbBuffer,
            GPDS_TFT_INFO,
            EIsiSubBlockTypeId8Len8 );

        ret = FillGpdsTFTInfo( GPDS_TFT_CREATE_NEW, aMediaAuthorization,
                               aPacketFilterPtr, aPdpType, gpdsTftInfoSbBuffer );

        if ( KErrNone == ret )
            {
            gpdsContextConfigureReq.CopyData(
                currentMsgOffset, gpdsTftInfoSubBlock.CompleteSubBlock() );
            // Set new message offset and increase messages subblock count
            currentMsgOffset = currentMsgOffset + gpdsTftInfoSbBuffer.Length();
            numOfSbInMessage++;
            }
        }
    // no else

    // If there were no errors
    if ( KErrNone == ret )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. there were no errors");
OstTrace0( TRACE_NORMAL, DUP7_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketContextMessHandler::GpdsContextConfigureReq, there were no errors" );

        // Add GPDS_COMP_INFO subblock
        // sub block of Configure Req
        TBuf8<SIZE_GPDS_COMP_INFO> compInfo;
        TIsiSubBlock compInfoSubBlock(
            compInfo,
            GPDS_COMP_INFO,
            EIsiSubBlockTypeId8Len8 );

        compInfo.Append( aDataCompression );
        compInfo.Append( aHeaderCompression );
        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, compInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset = currentMsgOffset + SIZE_GPDS_COMP_INFO;
        numOfSbInMessage++;

        // Add GPDS_APN_INFO subblock
        // sub block of Configure Req
        TBuf8<SIZE_GPDS_APN_INFO + RPacketContext::KGSNNameLength>
            accessPointNameInfo;
        TIsiSubBlock apnInfoSubBlock(
            accessPointNameInfo,
            GPDS_APN_INFO,
            EIsiSubBlockTypeId8Len8 );

        accessPointNameInfo.Append(
            static_cast<TUint8>( aAccessPointName.Length() ) );
        accessPointNameInfo.Append( aAccessPointName );
        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, apnInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset = currentMsgOffset + accessPointNameInfo.Length();
        numOfSbInMessage++;

        // Add GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO sub block
        TBuf8< SIZE_GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO > radioActivity;
        TIsiSubBlock radioActivitySubBlock(
            radioActivity,
            GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO,
            EIsiSubBlockTypeId8Len8 );

        // contains no parameters
        radioActivity.Append( KGpdsPadding );
        radioActivity.Append( KGpdsPadding );
        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, radioActivitySubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset = currentMsgOffset +
            SIZE_GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO;
        numOfSbInMessage++;

        // Set number of subblocks in GPDS_CONTEXT_CONFIGURE_REQ message
        gpdsContextConfigureReq.Set8bit(
            ISI_HEADER_SIZE +
            GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_NUMBEROFSUBBLOCKS,
            numOfSbInMessage );
        // Send GPDS_CONTEXT_CONFIGURE_REQ message
        ret = iPhoNetSender->Send( gpdsContextConfigureReq.Complete() );
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextConfigureResp
// This method breaks the response to the context configuration request.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextConfigureResp(
    const TIsiReceiveC& aIsiMessage )
    {
    TInt ret( KErrNone );
    // Get transaction id
    TUint8 transactionId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_UTID ) );

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_CID ) );

    TUint8 proxyId( GenerateTraId( contextId ) );

    TInfoName contextName;
    ret = iContextList->GetContextNameByContextId( contextId, contextName );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextConfigureResp. GetContextNameByContextId returns:%d", ret);
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGURERESP, "CMmPacketContextMessHandler::GpdsContextConfigureResp;returns=%d", ret );

    // Check that transactionId and context exists
    if ( transactionId == proxyId && KErrNone == ret )
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( &contextName );
        TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_STATUS ) );

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextConfigureResp. status: %d", status);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGURERESP, "CMmPacketContextMessHandler::GpdsContextConfigureResp;status=%hhu", status );

        if ( GPDS_OK == status )
            {
            TInt configurationType( iContextList->
                GetContextConfigurationType( contextId ) );

            if( KTIntNotDefined != configurationType )
                {

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureResp. KTIntNotDefined != configurationType");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGURERESP, "CMmPacketContextMessHandler::GpdsContextConfigureResp, KTIntNotDefined != configurationType" );

                if( TPacketDataConfigBase::KConfigGPRS == configurationType )
                    {

                    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureResp. configurationType == KConfigGPRS");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTCONFIGURERESP, "CMmPacketContextMessHandler::GpdsContextConfigureResp, configurationType == KConfigGPRS" );

                     // authentication info included in setconfig. Send auth reg
                    RPacketContext::TContextConfigGPRS* config =
                        reinterpret_cast<RPacketContext::TContextConfigGPRS*>(
                            iContextList->GetConfig( contextId,
                                                     configurationType ) );

                    if ( RPacketContext::EProtocolPAP ==
                        config->iProtocolConfigOption.iAuthInfo.iProtocol ||
                        RPacketContext::EProtocolCHAP ==
                        config->iProtocolConfigOption.iAuthInfo.iProtocol )
                        {
                        ret = SetAuthenticationConfigReq(
                            transactionId, contextId,
                            config->iProtocolConfigOption.iAuthInfo,
                            config->iProtocolConfigOption.iResponse,
                            config->iProtocolConfigOption.iChallenge,
                            config->iProtocolConfigOption.iId );

                        if( KErrNone != ret )
                            {
                            // Error in sending authentication reg.
                            iMessageRouter->Complete(
                                EPacketContextSetConfig,
                                &dataPackage,
                                ret );
                            }
                        //no else
                        }
                    else
                        {
                        // Authentication not included, Complete with KErrNone
                        iMessageRouter->Complete(
                            EPacketContextSetConfig,
                            &dataPackage,
                            ret );
                        }
                    }
                else
                    {
                    RPacketContext::TContextConfigR99_R4* config =
                        reinterpret_cast<RPacketContext::TContextConfigR99_R4*>(
                            iContextList->GetConfig( contextId,
                                TPacketDataConfigBase::KConfigRel99Rel4 ) );

                    // authentication info included in setconfig. Send auth reg
                    if ( RPacketContext::EProtocolPAP ==
                        config->iProtocolConfigOption.iAuthInfo.iProtocol ||
                        RPacketContext::EProtocolCHAP ==
                        config->iProtocolConfigOption.iAuthInfo.iProtocol )
                        {
                        ret = SetAuthenticationConfigReq(
                            transactionId, contextId,
                            config->iProtocolConfigOption.iAuthInfo,
                            config->iProtocolConfigOption.iResponse,
                            config->iProtocolConfigOption.iChallenge,
                            config->iProtocolConfigOption.iId );

                        if( KErrNone != ret )
                            {
                            // Error in sending authentication reg.
                            iMessageRouter->Complete(
                                EPacketContextSetConfig,
                                &dataPackage,
                                ret );
                            }
                        // no else
                        }
                      else
                        {
                        // Authentication not included, Complete with KErrNone
                        iMessageRouter->Complete(
                            EPacketContextSetConfig,
                            &dataPackage,
                            ret );
                        }
                    }
                }
            else
                {
                // Configuration type not found
                iMessageRouter->Complete(
                    EPacketContextSetConfig,
                    &dataPackage,
                    KErrGeneral );
                }
            }
        else
            {
            // Some error in Gpds server, Complete with KErrGeneral
            iMessageRouter->Complete(
                EPacketContextSetConfig,
                &dataPackage,
                KErrGeneral );
            }
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextAuthResp
// This method breaks the response for setting the context authentication info.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextAuthResp(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextAuthResp." );

    TInt errorCause( KErrNone );

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_AUTH_RESP_OFFSET_CID ) );

    TInfoName contextName;

    if ( KErrNone == iContextList->
        GetContextNameByContextId( contextId, contextName ) )
        {
        TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_AUTH_RESP_OFFSET_STATUS ) );

        if ( GPDS_OK != status )
            {
            errorCause = KErrGeneral;
            }
        // no else

        CMmDataPackage dataPackage;
        dataPackage.PackData( &contextName );
        // Complete Set Config Info
        iMessageRouter->Complete(
            EPacketContextSetConfig,
            &dataPackage,
            errorCause );
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextActivateReq
// Create an Isi-message for context activation.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::GpdsContextActivateReq(
    const TUint8 aTransactionId,
    const TUint8 aContextId,
    const TDesC8& aPdpAddress,
    const TUint8 aPdpType ) const
    {

    TFLOGSTRING3("TSY: CMmPacketContextMessHandler::GpdsContextActivateReq. TransId: %d ContextId: %d", aTransactionId, aContextId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEREQ, "CMmPacketContextMessHandler::GpdsContextActivateReq;aTransactionId=%hhu;aContextId=%hhu", aTransactionId, aContextId );

    TInt ret(iContextList->SetActivationTraId( aContextId, aTransactionId ));

    if( KErrNone == ret )
        {
        // Create and set gpds isi message
        TBuf8<SIZE_GPDS_CONTEXT_ACTIVATE_REQ + KMaxLengthOfGdpsActivateReqSB>
            gpdsActivateReq;
        gpdsActivateReq.Append( aContextId );

        // Create GPDS_PDP_ADDRESS_INFO subblock
        // Max length of pdpAddressInfo is 20
        // If aPdpAddress length is more than zero, and the type of aPdpAddress
        // is IPv4 or IPv6, create GPDS_PDP_ADDRESS_INFO
        if ( 0 < aPdpAddress.Length() &&
            ( GPDS_PDP_TYPE_IPV4 == aPdpType ||
                GPDS_PDP_TYPE_IPV6 == aPdpType ) )
            {
            // Set the pdp address info
            TBuf8<KMaxLengthOfGdpsActivateReqSB> pdpAddressInfo;
            pdpAddressInfo.Zero();
            TIsiSubBlock gpdsPdpAddressInfoSb (
                pdpAddressInfo,
                GPDS_PDP_ADDRESS_INFO,
                EIsiSubBlockTypeId8Len8 );

            pdpAddressInfo.Append( KGpdsPadding );
            pdpAddressInfo.Append( aPdpAddress.Length() ); //lenght should always
                                                           //be 4 or 16
            pdpAddressInfo.Append( aPdpAddress );

            TUint8 subBlockCount( 1 );
            gpdsActivateReq.Append( subBlockCount );
            gpdsActivateReq.Append( gpdsPdpAddressInfoSb.CompleteSubBlock() );
            }
        else
            {
            gpdsActivateReq.Append( 0 ); //sub block count
            }

        // Send Isi message via Phonet
        ret = iPhoNetSender->Send( PN_GPDS,
            aTransactionId, GPDS_CONTEXT_ACTIVATE_REQ, gpdsActivateReq );
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextActivatingInd
// Breaks gpds context activating indication message.
// Indicate that context activation has started.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextActivatingInd(
    const TIsiReceiveC& aIsiMessage )
    {

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_ACTIVATING_IND_OFFSET_CID ) );

    // Get transaction id for completion
    iContextList->GetAndResetActivationTraId( contextId );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivatingInd. ContextId: %d", contextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextActivatingInd;contextId=%hhu", contextId );

    CMmDataPackage data;
    TInt dialUpContext( KTIntNotDefined );
    TInfoName contextName;
    TInt ret( KErrNone );
    TUint sbStartOffSet( 0 );

    ret = iContextList->GetContextNameByContextId( contextId, contextName );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivatingInd. GetContextNameByContextId returns: %d", ret );
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextActivatingInd;ret=%d", ret );

    // context not found, check dial-up context
    if ( KErrArgument == ret  )
        {
        dialUpContext = iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if ( ret == KErrNone )
        {
        TInt configurationType( iContextList->
            GetContextConfigurationType( contextId ) );
        TPacketDataConfigBase* configBase = iContextList->
            GetConfig( contextId, configurationType );

        if( TPacketDataConfigBase::KConfigGPRS == configurationType )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivatingInd. KConfigGPRS");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextActivatingInd, KConfigGPRS" );

            RPacketContext::TContextConfigGPRS* config =
                reinterpret_cast<RPacketContext::TContextConfigGPRS*>(
                    configBase );

            sbStartOffSet = 0;
            TPtrC8 apn;

            // Get pdp type
            TUint8 pdpType( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + GPDS_CONTEXT_ACTIVATING_IND_OFFSET_PDPTYPE ) );

            CMmStaticUtility::MapPdpTypeToClient( config->iPdpType, pdpType );

            // Get APN info sub block
            TInt retValue( aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATING_IND,
                GPDS_APN_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet ) );

            if ( KErrNone == retValue )
                {
                TUint8 nameLen ( aIsiMessage.Get8bit( sbStartOffSet +
                    GPDS_APN_INFO_OFFSET_ADDRLENGTH ) );

                // Get APN info
                apn.Set( aIsiMessage.GetData( sbStartOffSet +
                    GPDS_APN_INFO_OFFSET_APNNAME, nameLen ) );
                }
            // no else

            if ( NULL != apn.Ptr() )
                {

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivatingInd. apn is not null");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextActivatingInd, apn is not null" );

                config->iAccessPointName.Copy( apn );
                }
            else
                {
                config->iAccessPointName.Zero();
                }
            // no else

            data.PackData( &contextName, &config );
            iMessageRouter->Complete(
                EPacketContextActivate,
                &data,
                KErrNone );
            }
        else if( TPacketDataConfigBase::KConfigRel99Rel4 == configurationType )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivatingInd. KConfigRel99Rel4");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextActivatingInd, KConfigRel99Rel4" );

            RPacketContext::TContextConfigR99_R4* config =
                reinterpret_cast<RPacketContext::TContextConfigR99_R4*>(
                    configBase );

            sbStartOffSet = 0;
            TPtrC8 apn;

            // Get pdp type
            TUint8 pdpType( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + GPDS_CONTEXT_ACTIVATING_IND_OFFSET_PDPTYPE ) );

            CMmStaticUtility::MapPdpTypeToClient( config->iPdpType, pdpType );

            // Get APN info sub block
            TInt retValue( aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATING_IND,
                GPDS_APN_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet ) );

            if ( KErrNone == retValue )
                {
                TUint8 nameLen ( aIsiMessage.Get8bit( sbStartOffSet +
                    GPDS_APN_INFO_OFFSET_ADDRLENGTH ) );

                // Get APN info
                apn.Set( aIsiMessage.GetData( sbStartOffSet +
                    GPDS_APN_INFO_OFFSET_APNNAME, nameLen ) );
                }
            // no else

            if ( NULL != apn.Ptr() )
                {

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivatingInd. accesspointname is not null");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextActivatingInd, accesspointname is not null" );

                config->iAccessPointName.Copy( apn );
                }
            else if ( NULL == apn.Ptr() )
                {
                config->iAccessPointName.Zero();
                }
            // no else

            data.PackData( &contextName, &config );
            iMessageRouter->Complete(
                EPacketContextActivate,
                &data,
                KErrNone );
            }
        // no else
        }
    else if( KErrNone == dialUpContext
        && contextId == iContextList->GetDialUpContextId() )
        {
        RPacketContext::TContextConfigR99_R4 config;
        sbStartOffSet = 0;
        TPtrC8 apn;

        // Get pdp type
        TUint8 pdpType( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_ACTIVATING_IND_OFFSET_PDPTYPE ) );

        CMmStaticUtility::MapPdpTypeToClient( config.iPdpType, pdpType );

        // Get APN info sub block
        TInt retValue( aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATING_IND,
            GPDS_APN_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) );

        if ( KErrNone == retValue )
            {
            TUint8 nameLen ( aIsiMessage.Get8bit( sbStartOffSet +
                GPDS_APN_INFO_OFFSET_ADDRLENGTH ) );

            // Get APN info
            apn.Set( aIsiMessage.GetData( sbStartOffSet +
                GPDS_APN_INFO_OFFSET_APNNAME, nameLen ) );
            }
        // no else

        if ( NULL != apn.Ptr() )
            {
            config.iAccessPointName.Copy( apn );
            }
         else if ( NULL == apn.Ptr() )
            {
            config.iAccessPointName.Zero();
            }
        // no else

        RPacketContext::TContextConfigR99_R4* config1 = &config;
        data.PackData( &contextName, &config1 );
        iMessageRouter->Complete(
            EPacketContextActivate,
            &data,
            KErrNone );
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextActivateIndL
// Breaks gpds context activate indication message.
// Indicate that context activation has succeeded.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextActivateIndL(
    const TIsiReceiveC& aIsiMessage )
    {

    // Buffer for DNS data
    TBuf8<RPacketContext::KMiscProtocolBufferLength - KRemoveOtherThanDNSBytes>
        tempDNSBuffer;
    // make sure that buffer is empty
    tempDNSBuffer.Zero();

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_ACTIVATE_IND_OFFSET_CID ) );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. ContextId: %d", contextId);
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;contextId=%hhu", contextId );

    TUint sbStartOffSet( 0 );

    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. GetContextNameByContextId returns:%d", ret );
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;ret=%d", ret );

    if( KErrArgument == ret )
        {
        ret = iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if( KErrNone == ret )
        {
        RPacketContext::TContextConfigGPRS config;

        TPtrC8 pdpAddress;
        TPtrC8 apn;
        TPtrC8 pDnsAddress;
        TPtrC8 sDnsAddress;
        TUint8 dataCompression( GPDS_PDP_DCOMP_DEFAULT );
        TUint8 headerCompression( GPDS_PDP_DCOMP_DEFAULT );
        TUint8 connectionSpeed( 0 );
        TInt connectionSpeedClient( 0 );
        TPtrC8 pcScfAddress;

        // Gpds sub blocks
        sbStartOffSet = 0;

        TInt retValue( aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_PDP_ADDRESS_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) );

        if ( KErrNone == retValue )
            {
            TUint8 addrLen ( aIsiMessage.Get8bit(
                sbStartOffSet + GPDS_PDP_ADDRESS_INFO_OFFSET_ADDRLENGTH ) );

            // Get PDP address
            pdpAddress.Set( aIsiMessage.GetData(
                sbStartOffSet + GPDS_PDP_ADDRESS_INFO_OFFSET_ADDRESS,
                addrLen ) );
            }
            // no else

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. pdpAddress length:%d", pdpAddress.Length() );
OstTrace1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL; pdpAddress length=%d", pdpAddress.Length() );

        // Get PDP address
        if ( 0 < pdpAddress.Length() )
            {
            CMmStaticUtility::ConvertIPAddressToClient(
                pdpAddress,
                config.iPdpAddress );
            }
        // no else

        retValue =  aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_APN_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. find sub block. retValue:%d", retValue );
OstTrace1( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;retValue=%d", retValue );

        if ( KErrNone == retValue )
            {
            TUint8 nameLen ( aIsiMessage.Get8bit(
                sbStartOffSet + GPDS_APN_INFO_OFFSET_ADDRLENGTH ) );

            // Get APN info
            apn.Set( aIsiMessage.GetData(
                sbStartOffSet + GPDS_APN_INFO_OFFSET_APNNAME, nameLen ) );
            }
        // no else

        // Get Access point name.
        if ( NULL != apn.Ptr() )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. apn.Ptr is not null");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL, apn.Ptr is not null" );

            config.iAccessPointName.Copy( apn );
            }
        // no else

        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_QOS_NEG_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

        if ( KErrNone == retValue )
            {
            // Get connection speed
            connectionSpeed = aIsiMessage.Get8bit(
                sbStartOffSet + GPDS_QOS_NEG_INFO_OFFSET_PEAKTHROUGHPUT );
            }
        // no else

        MapConnectionSpeed( connectionSpeed, connectionSpeedClient );

        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_PDNS_ADDRESS_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

        if ( KErrNone == retValue )
            {
            TUint8 addrLen ( aIsiMessage.Get8bit(
                sbStartOffSet + GPDS_PDNS_ADDRESS_INFO_OFFSET_ADDRLENGTH ) );

            // Get Primary DNS address
            pDnsAddress.Set( aIsiMessage.GetData(
                sbStartOffSet + GPDS_PDNS_ADDRESS_INFO_OFFSET_ADDRESS,
                addrLen ) );
            }
        // no else

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. primary DNS address length:%d", pDnsAddress.Length() );
OstTrace1( TRACE_NORMAL, DUP5_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;primary DNS address length=%d", pDnsAddress.Length() );

        // Get Primary DNS address.
        if ( 0 < pDnsAddress.Length() )
            {
            CMmStaticUtility::ConvertIPAddressToClient(
                pDnsAddress,
                config.iProtocolConfigOption.iDnsAddresses.iPrimaryDns );
            }
        // no else

        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_SDNS_ADDRESS_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

        if ( KErrNone == retValue )
            {
            TUint8 addrLen ( aIsiMessage.Get8bit(
                sbStartOffSet + GPDS_SDNS_ADDRESS_INFO_OFFSET_ADDRLENGTH ) );

            // Get Secondary DNS address
            sDnsAddress.Set( aIsiMessage.GetData(
                sbStartOffSet + GPDS_SDNS_ADDRESS_INFO_OFFSET_ADDRESS,
                addrLen ) );
            }
        // no else

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. secondary DNS address length:%d", sDnsAddress.Length());
OstTrace1( TRACE_NORMAL, DUP6_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;secondary DNS address length=%d", sDnsAddress.Length() );

        // Get Secondary DNS address.
        if ( 0 < sDnsAddress.Length() )
            {
            CMmStaticUtility::ConvertIPAddressToClient(
                sDnsAddress,
                config.iProtocolConfigOption.iDnsAddresses.iSecondaryDns );
            }
        // no else

        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_COMP_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

        if ( KErrNone == retValue )
            {
            dataCompression = aIsiMessage.Get8bit(
                sbStartOffSet + GPDS_COMP_INFO_OFFSET_DATACOMPRESSION );

            headerCompression = aIsiMessage.Get8bit(
                sbStartOffSet + GPDS_COMP_INFO_OFFSET_HEADERCOMPRESSION );
            }
        // no else

        if ( GPDS_PDP_DCOMP_DEFAULT != dataCompression )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. GPDS_PDP_DCOMP_DEFAULT != dataCompression");
OstTrace0( TRACE_NORMAL, DUP7_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL, GPDS_PDP_DCOMP_DEFAULT != dataCompression" );

            if ( GPDS_PDP_DCOMP_ON == dataCompression )
                {

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. GPDS_PDP_DCOMP_ON == dataCompression");
OstTrace0( TRACE_NORMAL, DUP8_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL,  GPDS_PDP_DCOMP_ON == dataCompression" );

                if ( !( config.iPdpCompression &
                    RPacketContext::KPdpDataCompression ) )
                    {
                    config.iPdpCompression ^=
                        RPacketContext::KPdpDataCompression;
                    }
                // no else
                }
            else
                {
                if ( ( config.iPdpCompression &
                    RPacketContext::KPdpDataCompression ) )
                    {
                    config.iPdpCompression ^=
                        RPacketContext::KPdpDataCompression;
                    }
                // no else
                }
            }
        // no else

        if ( GPDS_PDP_DCOMP_DEFAULT != headerCompression )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. GPDS_PDP_DCOMP_DEFAULT != headerCompression");
OstTrace0( TRACE_NORMAL, DUP9_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateInd, GPDS_PDP_DCOMP_DEFAULT != headerCompressionL" );

            if ( GPDS_PDP_DCOMP_ON == headerCompression )
                {

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. GPDS_PDP_DCOMP_ON == headerCompression");
OstTrace0( TRACE_NORMAL, DUP10_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL, GPDS_PDP_DCOMP_ON == headerCompression" );

                if ( !( config.iPdpCompression &
                    RPacketContext::KPdpHeaderCompression ) )
                    {
                    config.iPdpCompression ^=
                        RPacketContext::KPdpHeaderCompression;
                    }
                // no else
            }
            else
                {
                if ( ( config.iPdpCompression &
                    RPacketContext::KPdpHeaderCompression ) )
                    {
                    config.iPdpCompression ^=
                        RPacketContext::KPdpHeaderCompression;
                    }
                // no else
                }
            }
        // no else

        // TLV  buffer  construction
        TInt tlvLen( 0 );
        RPacketContext::TPcoId pcoId( RPacketContext::EEtelPcktNotSupported );
        TBuf8<KDNSBufferLength>pDnsBuffer;
        pDnsBuffer.Zero();
        TBuf8<KDNSBufferLength>sDnsBuffer;
        sDnsBuffer.Zero();
        TBuf8<KCSCFBufferLength>pCSCFBuffer;
        pCSCFBuffer.Zero();
        TBuf8<KSignallingFlagBuffer>signallingFlagBuffer;
        signallingFlagBuffer.Zero();

        // If asked add to TLV struct
        if ( iContextList->GetDNSAddressInfoToContextInfo( contextId ) )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. add to TLV struct");
OstTrace0( TRACE_NORMAL, DUP11_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL, add to TLV struct" );

            // Type is always ipv6
            if ( KIpv6AddressLen == pDnsAddress.Length() )
                {
                for ( TInt i = 0; i < KIpv6AddressLen; i++ )
                    {
                    pDnsBuffer.Append( static_cast<TUint8>( pDnsAddress[i] ) );
                    }
                pDnsBuffer.SetLength( KIpv6AddressLen );
                // 2+1+16
                tlvLen = tlvLen + KTLVDataLen + KPCOIdLen + KIpv6AddressLen;
                }
            // no else
            }
        // no else

        // add secondary DNS
        if ( iContextList->GetDNSAddressInfoToContextInfo( contextId ) )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. add secondary DNS");
OstTrace0( TRACE_NORMAL, DUP12_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL, add secondary DNS" );

            // Type is allways ipv6
            if ( KIpv6AddressLen == sDnsAddress.Length() )
                {
                for ( TInt i = 0; i < KIpv6AddressLen; i++ )
                    {
                    sDnsBuffer.Append( static_cast<TUint8>( sDnsAddress[i] ) );
                    }
                sDnsBuffer.SetLength( KIpv6AddressLen );
                tlvLen = tlvLen + KTLVDataLen + KPCOIdLen + KIpv6AddressLen;
                }
            // no else
            }
        // no else

        // get P-CSCF Address sub-block: P-CSCF IP address. This sub-block is
        // ONLY present if P-CSCF address was requested in
        // GPDS_CONTEXT_CONFIGURE_REQ.
        // Although might still miss if network does not send it.
        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_SHARED_PCSCF_ADDRESS_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

        TFLOGSTRING2("TSY: GPDS_SHARED_PCSCF_ADDRESS_INFO sub-block, retValue %d.", retValue );
OstTrace1( TRACE_NORMAL, DUP13_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;GPDS_SHARED_PCSCF_ADDRESS_INFO sub-block retValue=%d", retValue );

        if( KErrNone == retValue )
            {
            TUint8 addrLen ( aIsiMessage.Get8bit(
                sbStartOffSet +
                GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_ADDRLENGTH ) );

            pcScfAddress.Set( aIsiMessage.GetData(
                ( sbStartOffSet +
                GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_PCSCFADDRESS ),
                addrLen ) );
            }
        // no else

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. PCSCF address length:%d", pcScfAddress.Length() );
OstTrace1( TRACE_NORMAL, DUP14_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;PCSCF address length=%d", pcScfAddress.Length() );

        // PCSCF address types IPv6 and IPv4 are valid
        // The length of IPv4 address is 4
        if( KIpv4AddressLen == pcScfAddress.Length() )
            {
            // get data to temp buffer
            for ( TInt index = 0; index < pcScfAddress.Length(); index++ )
                {
                TUint8 entry( static_cast<TUint8>( pcScfAddress[index] ));
                pCSCFBuffer.Append( entry );
                }
            pCSCFBuffer.SetLength( KIpv4AddressLen );
            // 2+1+4 (data length + PCO Id + data)
            tlvLen = tlvLen + KTLVDataLen + KPCOIdLen + KIpv4AddressLen;
            }
        // The length of IPv6 address is 16
        else if( KIpv6AddressLen == pcScfAddress.Length() )
            {
            //get data to temp buffer
            for ( TInt index = 0; index < pcScfAddress.Length(); index++ )
                {
                TUint8 entry( static_cast<TUint8>( pcScfAddress[index] ));
                pCSCFBuffer.Append( entry );
                }
            pCSCFBuffer.SetLength( KIpv6AddressLen );
            // 2+1+16 (data length + PCO Id + data)
            tlvLen = tlvLen + KTLVDataLen + KPCOIdLen + KIpv6AddressLen;
            }
        // no else

        retValue = aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND,
            GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

        if ( KErrNone == retValue )
            {
            signallingFlagBuffer.Append( aIsiMessage.Get8bit(
                sbStartOffSet +
                GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO_OFFSET_IMCNSIGFLAG ) );
            signallingFlagBuffer.SetLength( KSignallingFlagBuffer );
            tlvLen += KIpv4AddressLen;
            }
        // no else

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. tlv length:%d", tlvLen );
OstTrace1( TRACE_NORMAL, DUP15_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;tlvLen=%d", tlvLen );

        // Initialise buffer
        if( 0 < tlvLen )
            {
            config.iProtocolConfigOption.iMiscBuffer.SetLength( tlvLen );
            config.iProtocolConfigOption.iMiscBuffer.FillZ();

            // Create pointer to it
            TPtr8 smallPtr( config.iProtocolConfigOption.iMiscBuffer.LeftTPtr(
                config.iProtocolConfigOption.iMiscBuffer.MaxLength() ) );
            smallPtr.SetLength( 0 );

            // Create TLV struct
            TTlvStruct<RPacketContext::TPcoId,
                       RPacketContext::TPcoItemDataLength>
                       smallTLV( smallPtr, 0 );

            // Append primary DNS
            if ( KDNSBufferLength == pDnsBuffer.Length() )
                {
                TPtr8 pDnsBufferPtr(
                    const_cast<TUint8*>( pDnsBuffer.Ptr() ),
                    pDnsBuffer.Length(),
                    pDnsBuffer.Length() );
                pcoId = RPacketContext::EEtelPcktDNSServerAddressRequest;
                ret = smallTLV.AppendItemL( pcoId, pDnsBufferPtr );

                TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd.EEtelPcktDNSServerAddressRequest, Prim. DNS, AppendItemL ret=%d ",ret );
OstTrace1( TRACE_NORMAL, DUP16_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;Prim. DNS, AppendItemL ret=%d", ret );
                }
            // no else

            // Append secondary DNS
            if( KDNSBufferLength == sDnsBuffer.Length() )
                {
                TPtr8 sDnsBufferPtr(
                    const_cast<TUint8*>( sDnsBuffer.Ptr() ),
                    sDnsBuffer.Length(),
                    sDnsBuffer.Length() );
                pcoId = RPacketContext::EEtelPcktDNSServerAddressRequest;
                ret = smallTLV.AppendItemL( pcoId, sDnsBufferPtr );

                TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd.EEtelPcktDNSServerAddressRequest, Sec. DNS, AppendItemL ret=%d ",ret );
OstTrace1( TRACE_NORMAL, DUP17_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL; Sec. DNS, AppendItemL ret=%d", ret );
                }
            // no else

            // Append PCSCFBuffer address
            // (IPv6 (length 16) and IPv4 (length 4) address supported)
            if( KIpv6AddressLen == pCSCFBuffer.Length() ||
                KIpv4AddressLen == pCSCFBuffer.Length() )
                {
                TPtr8 pCSCFBufferPtr(
                    const_cast<TUint8*>( pCSCFBuffer.Ptr() ),
                    pCSCFBuffer.Length(),
                    pCSCFBuffer.Length() );
                pcoId = RPacketContext::EEtelPcktPCSCFAddressRequest;
                ret = smallTLV.AppendItemL( pcoId, pCSCFBufferPtr );

                TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd.EEtelPcktPCSCFAddressRequest AppendItemL ret=%d ",ret );
OstTrace1( TRACE_NORMAL, DUP18_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;EEtelPcktPCSCFAddressRequest AppendItemL ret=%d", ret );
                }
            // no else

            // Append IM CNMS sub system Signalling Flag
            if( KSignallingFlagBuffer == signallingFlagBuffer.Length() )
                {
                TPtr8 signallingFlagBufferPtr(
                    const_cast<TUint8*>( signallingFlagBuffer.Ptr() ),
                    signallingFlagBuffer.Length(),
                    signallingFlagBuffer.Length() );
                pcoId = RPacketContext::EEtelPcktIMCNMSSubsystemSignallingFlag;
                ret = smallTLV.AppendItemL( pcoId, signallingFlagBufferPtr );

                TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd.EEtelPcktIMCNMSSubsystemSignallingFlag AppendItemL ret=%d ",ret );
OstTrace1( TRACE_NORMAL, DUP19_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL;EEtelPcktIMCNMSSubsystemSignallingFlag AppendItemL ret=%d", ret );
                }
            // no else
            }
        // no else

        CMmDataPackage data;
        data.PackData( &contextName, &config );
        iMessageRouter->Complete(
            EPacketContextNotifyConfigChanged,
            &data,
            KErrNone );
        // CompleteNotifyConnectionSpeedChange
        data.PackData( &contextName, &connectionSpeedClient );
        iMessageRouter->Complete(
            EPacketContextNotifyConnectionSpeedChange,
            &data,
            KErrNone );
        }
    // no else

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateInd. End point.");
OstTrace0( TRACE_NORMAL, DUP20_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEINDL, "CMmPacketContextMessHandler::GpdsContextActivateIndL, End point" );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextActivateFailIndL
// Breaks gpds context activate fail indication message.
// Indicate that context activation has failed.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextActivateFailInd(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextActivateFailInd.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTACTIVATEFAILIND, "CMmPacketContextMessHandler::GpdsContextActivateFailInd" );

    // initialise struct for completion
    #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
    CMmCommonStaticUtility::TContextMisc misc;
    #else
    TContextMisc misc;
    #endif // NCP_COMMON_S60_VERSION_SUPPORT

    misc.iStatus = RPacketContext::EStatusInactive;
    misc.iRejectionCause.Zero();

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_CID ) );

    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    if ( KErrNone == ret )
        {
        TUint8 cause( aIsiMessage.Get8bit(  ISI_HEADER_SIZE +
            GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_CAUSE ) );

        TInt errorValue( KErrNone );
        TUint8 rejectionCause( KErrNone );

        if ( GPDS_CAUSE_NO_CONNECTION == cause )
            {
            errorValue = CMmStaticUtility::EpocErrorCode(
                KErrCouldNotConnect, KErrGprsActivationRejected );
            }
        else if ( GPDS_CAUSE_AUTHENTICATION == cause )
            {
            errorValue = CMmStaticUtility::PacketDataCSCauseToEpocError(
                cause );

            TUint sbStartOffSet( 0 );

            TInt retValue( aIsiMessage.FindSubBlockOffsetById(
                ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_FAIL_IND,
                GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet ) );

            if( KErrNone == retValue)
                {
                rejectionCause = aIsiMessage.Get8bit( sbStartOffSet +
                  GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO_OFFSET_REJECTIONCAUSE
                  );

                misc.iRejectionCause.Append( rejectionCause );
                }
            // no else
            }
        else if ( GPDS_CAUSE_CALL_CONTROL == cause )
            {
            errorValue = CMmStaticUtility::EpocErrorCode(
                KErrGeneral,
                KErrMMEtelActivationBlockedByCallControlNoText );
            }
        else
            {
            // Map the error cause to Epoc error
            errorValue = CMmStaticUtility::PacketDataCSCauseToEpocError(
                cause );
            }

        // Pack data
        CMmDataPackage data;
        data.PackData( &contextName, &misc);
        iMessageRouter->Complete(
            EPacketContextNotifyStatusChange,
            &data,
            errorValue );

        data.PackData( &contextName );
        iMessageRouter->Complete(
            EPacketContextNotifyConfigChanged,
            &data,
            errorValue );
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextModifyInd
// Breaks gpds context modify indication message.
// Indicate the context modification.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextModifyInd(
    const TIsiReceiveC& aIsiMessage )
    {
    TUint sbStartOffSet( 0 );

    // Get context id
    TUint8 contextId( aIsiMessage.Get8bit(
       ISI_HEADER_SIZE + GPDS_CONTEXT_MODIFY_IND_OFFSET_CID ) );
    // Get context name by context id
    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextModifyInd. ContextId:%d", contextId);
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTMODIFYIND, "CMmPacketContextMessHandler::GpdsContextModifyInd;contextId=%hhu", contextId );

    if( KErrArgument == ret )
        {
        ret = iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if ( KErrNone == ret )
        {
        // Gpds sub block
        sbStartOffSet = 0;

        TUint8 connectionSpeed( 0 );

        TInt retValue( aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_MODIFY_IND,
            GPDS_QOS_NEG_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) );

        if ( KErrNone == retValue )
            {
            // Get connection speed
            connectionSpeed = aIsiMessage.Get8bit( sbStartOffSet +
                GPDS_QOS_NEG_INFO_OFFSET_PEAKTHROUGHPUT );

            TInt connectionSpeedClient( 0 );
            MapConnectionSpeed( connectionSpeed, connectionSpeedClient );

            CMmDataPackage data;
            // CompleteNotifyConnectionSpeedChange
            data.PackData( &contextName, &connectionSpeedClient );
            iMessageRouter->Complete(
                EPacketContextNotifyConnectionSpeedChange,
                &data,
                KErrNone );
            }
        // no else
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextDeactivateReq
// Create an Isi-message for context deactivation.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::GpdsContextDeactivateReq(
    const CMmDataPackage& aDataPackage )
    {
    TInfoName* contextName = NULL;
    aDataPackage.UnPackData( &contextName );
    TUint8 contextId( iContextList->GetContextIdByContextName( contextName ) );

    if( GPDS_CID_VOID == contextId )
        {
        contextId = iContextList->GetDialUpContextIdByName( contextName );
        }
    // no else

    TUint8 transactionId( GenerateTraId( contextId ) );

    TFLOGSTRING3("TSY: CMmPacketContextMessHandler::GpdsContextDeactivateReq. TransId: %d ContextId: %d", transactionId, contextId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTDEACTIVATEREQ, "CMmPacketContextMessHandler::GpdsContextDeactivateReq;transactionId=%hhu;contextId=%hhu", transactionId, contextId );

    // Set the message data that consists of one parameter
    TBuf8<KMessageDataBufSize1> messageData;
    messageData.Append( contextId );

    // Send Isi message via Phonet
    return iPhoNetSender->Send( PN_GPDS,
        transactionId, GPDS_CONTEXT_DEACTIVATE_REQ, messageData );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextDeactivateRespL
// This method breaks the response to the context deactivation request.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextDeactivateRespL(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextDeactivateRespL.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTDEACTIVATERESPL, "CMmPacketContextMessHandler::GpdsContextDeactivateRespL" );

    // Get transaction id
    TUint8 transactionId( aIsiMessage.Get8bit(
       ISI_HEADER_SIZE + GPDS_CONTEXT_DEACTIVATE_RESP_OFFSET_UTID ) );
    TUint8 status( aIsiMessage.Get8bit(
       ISI_HEADER_SIZE + GPDS_CONTEXT_DEACTIVATE_RESP_OFFSET_STATUS ) );
    TUint8 contextId( aIsiMessage.Get8bit(
       ISI_HEADER_SIZE + GPDS_CONTEXT_DEACTIVATE_RESP_OFFSET_CID ) );
    TInt result( KErrGeneral );
    TInfoName contextName;

    if ( transactionId == GenerateTraId( contextId ) )
        {
        if ( GPDS_OK == status )
            {
            result = KErrNone;
            if ( PN_PIPE_DISABLE != iContextList->GetPipeStatus( contextId ) &&
                KErrNotFound == iContextList->
                    DialUpContextName( contextId, contextName ) )
                {
                TUint8 pipeHandle( iContextList->
                    PipeHandleByContextId( contextId ) );

                // Reset the pipe
                TInt ret( iMmPipeController->PnsPipeResetReq(
                    transactionId, pipeHandle, PN_PIPE_DISABLE ) );

                if ( KErrNone == ret )
                    {
                    // Store the trans id
                    ret = iContextList->
                        SetPipeResetTraId( contextId, transactionId );

                    if( KErrNone != ret )
                        {

                        TFLOGSTRING( "TSY: CMmPacketContextMessHandler::GpdsContextDeactivateRespL. SetPipeResetTraId failed " );
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTDEACTIVATERESPL, "CMmPacketContextMessHandler::GpdsContextDeactivateRespL, SetPipeResetTraId failed" );
                        }
                    // no else
                    }
                // no else
                }
            // no else
            }
        // no else

        TInt found( iContextList->
            GetContextNameByContextId( contextId, contextName ) );

        if( KErrArgument == found )
            {
            found = iContextList->DialUpContextName( contextId, contextName );
            }
        // no else

        if( found == KErrNone &&
            CALL_MODEM_ID_NONE == iCallModemResourceCallId )
            {
            // don't complete if allow incoming call feature is initiated
            CMmDataPackage data;
            data.PackData( &contextName );
            iMessageRouter->Complete(
                EPacketContextDeactivate,
                &data,
                result );
            }
        // no else

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextDeactivateRespL. Error(0=OK): %d ", found );
OstTrace1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTDEACTIVATERESPL, "CMmPacketContextMessHandler::GpdsContextDeactivateRespL;Error(0=OK)=%d", found );

        // check if incoming call allowation is requested
        // (for allow incoming call feature)
        if ( GPDS_OK == status &&
            CALL_MODEM_ID_NONE != iCallModemResourceCallId )
            {
            if ( iDeactivateContextList &&
                0 < iDeactivateContextList->Count() )
                {
                // remove deactivated context from the list
                iDeactivateContextList->Delete( 0 );
                }
            //no else
            AllowIncomingCallActivationL();
            }

        else if ( CALL_MODEM_ID_NONE != iCallModemResourceCallId )
            {
            // deactivation failed, can't allow call activation
            iCallModemResourceCallId = CALL_MODEM_ID_NONE;

            if ( iDeactivateContextList )
                {
                iDeactivateContextList->Reset();
                delete iDeactivateContextList;
                }
            //no else
            iDeactivateContextList = NULL;
            }
        //no else
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextDeactivatingInd
// Breaks gpds context deactivating indication message.
// Indicate that context deactivation has been started.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextDeactivatingInd(
    const TIsiReceiveC& aIsiMessage )
    {

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_DEACTIVATING_IND_OFFSET_CID ) );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextDeactivatingInd. Context id: %d", contextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTDEACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextDeactivatingInd;contextId=%hhu", contextId );

    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    if( KErrArgument == ret )
        {
        ret = iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if ( KErrNone == ret )
        {
        // initialise struct for completion
        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        CMmCommonStaticUtility::TContextMisc misc;
        #else
        TContextMisc misc;
        #endif // NCP_COMMON_S60_VERSION_SUPPORT

        misc.iStatus = RPacketContext::EStatusDeactivating;
        misc.iRejectionCause.Zero();

        CMmDataPackage data;
        data.PackData( &contextName, &misc );

        iMessageRouter->Complete(
            EPacketContextNotifyStatusChange,
            &data,
            KErrNone );
        }
    // no else

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextDeactivatingInd. Error(0=OK): %d ", ret );

OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTDEACTIVATINGIND, "CMmPacketContextMessHandler::GpdsContextDeactivatingInd;Error(0=OK)=%d", ret );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextDeactivateInd
// Breaks gpds context deactivate indication message.
// Indicate that context deactivation has been succeeded.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextDeactivateInd(
   const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextDeactivateInd.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTDEACTIVATEIND, "CMmPacketContextMessHandler::GpdsContextDeactivateInd" );

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_CID ) );

    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    if( KErrArgument == ret )
        {
        ret = iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if ( KErrNone == ret )
        {
        TUint8 cause( aIsiMessage.Get8bit(
           ISI_HEADER_SIZE + GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_CAUSE ) );

        TInt result( KErrNone );

        if ( GPDS_CAUSE_DEACT_REGULAR != cause )
            {
            result = CMmStaticUtility::PacketDataCSCauseToEpocError( cause );
            }
        // no else

        //initialise struct for completion
        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        CMmCommonStaticUtility::TContextMisc misc;
        #else
        TContextMisc misc;
        #endif // NCP_COMMON_S60_VERSION_SUPPORT

        misc.iStatus = RPacketContext::EStatusInactive;
        misc.iRejectionCause.Zero();

        CMmDataPackage data;
        data.PackData( &contextName, &misc );
        iMessageRouter->Complete(
            EPacketContextNotifyStatusChange,
            &data,
            result );
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextStatusReq
// Create an Isi-message for context status info request.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::GpdsContextStatusReq(
    const CMmDataPackage& aDataPackage )
    {
    TInfoName* contextName=NULL;
    aDataPackage.UnPackData( &contextName );

    TUint8 contextId( iContextList->
        GetContextIdByContextName( contextName ) );

    if( GPDS_CID_VOID == contextId )
        {
        contextId = iContextList->
            GetDialUpContextIdByName( contextName );
        }
    // no else

    TInt ret( KErrNotReady );
    TUint8 transactionId( GenerateTraId( contextId ) );

    TFLOGSTRING3("TSY: CMmPacketContextMessHandler::GpdsContextStatusReq. TransId: %d ContextId: %d", transactionId, contextId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTSTATUSREQ, "CMmPacketContextMessHandler::GpdsContextStatusReq;transactionId=%hhu;contextId=%hhu", transactionId, contextId );

    if(  GPDS_CID_VOID != contextId  )
        {
        // Set the message data that consists of one parameter
        TBuf8<KMessageDataBufSize1> messageData;
        messageData.Append( contextId );

        // Send Isi message via Phonet
        ret = iPhoNetSender->Send( PN_GPDS,
            transactionId, GPDS_CONTEXT_STATUS_REQ, messageData );
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextStatusResp
// This method breaks the response to the context status request.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextStatusResp(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextStatusResp.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTSTATUSRESP, "CMmPacketContextMessHandler::GpdsContextStatusResp" );

    // Get context id
    TUint8 contextId( aIsiMessage.Get8bit(
       ISI_HEADER_SIZE + GPDS_CONTEXT_STATUS_RESP_OFFSET_CID ) );
    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    if( KErrArgument == ret )
        {
        ret =  iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if ( KErrNone == ret )
        {
        RPacketContext::TDataVolume dataVolume;

        TUint8 status( aIsiMessage.Get8bit(
           ISI_HEADER_SIZE + GPDS_CONTEXT_STATUS_RESP_OFFSET_STATUS ) );
        TInt result( KErrGeneral );

        if ( GPDS_OK == status )
            {
            // Get the amount of Tx data
            dataVolume.iBytesSent = aIsiMessage.Get32bit(
                ISI_HEADER_SIZE + GPDS_CONTEXT_STATUS_RESP_OFFSET_TXBYTECOUNT );

            // Get the amount of Rx data
            dataVolume.iBytesReceived = aIsiMessage.Get32bit(
                ISI_HEADER_SIZE + GPDS_CONTEXT_STATUS_RESP_OFFSET_RXBYTECOUNT );

            result = KErrNone;
            }
        // no else

        CMmDataPackage data;
        data.PackData( &contextName, &dataVolume );

        iMessageRouter->Complete(
            EPacketContextGetDataVolumeTransferred,
            &data,
            result);
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextStatusInd
// Breaks gpds context status indication message.
// This indication includes the final amount of data transferred over airlink.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextStatusInd(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextStatusInd.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTSTATUSIND, "CMmPacketContextMessHandler::GpdsContextStatusInd" );

    // Get context id
    TUint8 contextId( aIsiMessage.Get8bit(
       ISI_HEADER_SIZE + GPDS_CONTEXT_STATUS_IND_OFFSET_CID  ) );
    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    // context not found, check dial-up context
    if ( KErrArgument == ret  )
        {
        ret = iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if ( KErrNone == ret )
        {
        RPacketContext::TDataVolume dataVolume;

        // Get the amount of Tx data
        dataVolume.iBytesSent = aIsiMessage.Get32bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_STATUS_IND_OFFSET_TXBYTECOUNT );

        // Get the amount of Rx data
        dataVolume.iBytesReceived = aIsiMessage.Get32bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_STATUS_IND_OFFSET_RXBYTECOUNT );

        CMmDataPackage data;
        data.PackData( &contextName, &dataVolume );

        iMessageRouter->Complete(
            EPacketContextNotifyDataTransferred,
            &data,
            KErrNone );
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextIdDeleteInd
// Breaks gpds context id delete indication message.
// Indicate that context id has been deleted.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextIdDeleteInd(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextIdDeleteInd.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTIDDELETEIND, "CMmPacketContextMessHandler::GpdsContextIdDeleteInd" );

    TUint8 contextId( aIsiMessage.Get8bit(
       ISI_HEADER_SIZE + GPDS_CONTEXT_ID_DELETE_IND_OFFSET_CID  ) );
    // Get transaction id
    TUint8 transactionId( iContextList->
        GetAndResetDeletionTraId( contextId ) );
    TInfoName contextName;
    TInt ret( iContextList->
        GetContextNameByContextId( contextId, contextName ) );

    // context not found, check dial-up context
    if ( KErrArgument == ret  )
        {
        ret = iContextList->DialUpContextName( contextId, contextName );
        }
    // no else

    if( KErrNone == ret )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextIdDeleteInd. Context name found.");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTIDDELETEIND, "CMmPacketContextMessHandler::GpdsContextIdDeleteInd, Context name found" );

        // initialise struct for completion
        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        CMmCommonStaticUtility::TContextMisc misc;
        #else
        TContextMisc misc;
        #endif // NCP_COMMON_S60_VERSION_SUPPORT

        misc.iStatus = RPacketContext::EStatusDeleted;
        misc.iRejectionCause.Zero();

        CMmDataPackage data;
        data.PackData( &contextName, &misc );

        iMessageRouter->Complete(
            EPacketContextNotifyStatusChange,
            &data,
            KErrNone );

        TUint8 proxyId = GenerateTraId( contextId );

        // check that delete is requested for this context
        if ( transactionId == proxyId )
            {
            RPacketContext::TContextStatus status(
                RPacketContext::EStatusDeleted );
            data.PackData( &contextName, &status );
            iMessageRouter->Complete(
                EPacketContextDelete,
                &data,
                KErrNone );
            }
        // no else
        }
    // no else

    // Reset context id and Host context id from contextmessagehandler list.
    iContextList->RemoveContextId( contextId );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextModifyReq
// Create an Isi-message for context modify request.
// Modifies parameters of an active PDP context.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::GpdsContextModifyReq(
    const CMmDataPackage* aDataPackage )
    {
    TInfoName contextName;
    TContextConfiguration configuration;
    CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>*
        mediaAuthorizationArray;

    aDataPackage->UnPackData( configuration, mediaAuthorizationArray );

    contextName = configuration.iContextName;

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextModifyReq. Context:%s", &contextName );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTMODIFYREQ, "CMmPacketContextMessHandler::GpdsContextModifyReq;contextName=%S", contextName );

    TUint8 contextId(iContextList->GetContextIdByContextName( &contextName ));

    // Initializing ret value to KErrNone
    TInt ret( KErrNone );

    if( GPDS_CID_VOID != contextId )
        {
        TUint8 modifyTraId( GenerateTraId( contextId ) );

        TFLOGSTRING3("TSY: CMmPacketContextMessHandler::GpdsContextModifyReq. TransId: %d ContextId: %d", modifyTraId, contextId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTMODIFYREQ, "CMmPacketContextMessHandler::GpdsContextModifyReq;modifyTraId=%hhu;contextId=%hhu", modifyTraId, contextId );

        TUint8 tftOperation( 0 );

        MapTftOperation( configuration.iTftOperation, tftOperation );

        TUint8 pdpType( 0 );

        MapPdpType( pdpType, configuration.iConfiguration );

        // Temporary container to context packet filter(s)
        CArrayFixFlat<RPacketContext::TPacketFilterV2>* aPacketFilterPtr =
            configuration.iTftFilterArray;

        // Create isi message
        TIsiSend gpdsContextModifyReq( iPhoNetSender->SendBufferDes() );
        gpdsContextModifyReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_GPDS );
        // Set the Gpds Context Modify Req data
        gpdsContextModifyReq.Set8bit(
            ISI_HEADER_OFFSET_TRANSID, modifyTraId );
        gpdsContextModifyReq.Set8bit(
            ISI_HEADER_OFFSET_MESSAGEID, GPDS_CONTEXT_MODIFY_REQ );
        gpdsContextModifyReq.Set8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_MODIFY_REQ_OFFSET_CID,
            contextId );

        // Initialize message offset and subblock count
        TInt currentMsgOffset(
            ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_MODIFY_REQ );
        TUint8 numOfSbInMessage( 0 );

        // If Rel99 QoS parameters are included
        if( NULL != configuration.iQosParams99 )
            {
            // Add GPDS_QOS99_REQ_INFO subblock
            TBuf8<SIZE_GPDS_QOS99_REQ_INFO> qos99ReqInfo;
            TIsiSubBlock reqInfo99SubBlock(
                qos99ReqInfo,
                GPDS_QOS99_REQ_INFO,
                EIsiSubBlockTypeId8Len8 );

            ret = FillGpdsQos99ReqInfo( *configuration.iQosParams99,
                                        qos99ReqInfo );

            if ( KErrNone == ret )
                {
                gpdsContextModifyReq.CopyData(
                    currentMsgOffset, reqInfo99SubBlock.CompleteSubBlock() );
                // Set new offset and increase subblock count
                currentMsgOffset =
                    currentMsgOffset + SIZE_GPDS_QOS99_REQ_INFO;
                numOfSbInMessage++;
                }
            }
        // no else

        // If Rel97/98 QoS parameters are included
        if( NULL != configuration.iQosParams97 )
            {
            // Add GPDS_QOS_REQ_INFO subblock
            TBuf8<SIZE_GPDS_QOS_REQ_INFO> qos97ReqInfo;
            TIsiSubBlock reqInfo97SubBlock(
                qos97ReqInfo,
                GPDS_QOS_REQ_INFO,
                EIsiSubBlockTypeId8Len8 );

            ret = FillGpdsQosReqInfo( *configuration.iQosParams97,
                                      qos97ReqInfo );

            if ( KErrNone == ret )
                {
                gpdsContextModifyReq.CopyData(
                    currentMsgOffset, reqInfo97SubBlock.CompleteSubBlock() );
                // Set new offset and increase subblock count
                currentMsgOffset =
                    currentMsgOffset + SIZE_GPDS_QOS_REQ_INFO;
                numOfSbInMessage++;
                }
            }
        // no else

        // If given aTftOperation indicates "DeletePacketFilter" and
        // DeleteID-array includes PacketFilterID's to be removed.
        // DeleteID -array may include 1 to 8 items and ID's can have values
        // from 1 to 8. Value 0xFF indicates there is no valid ID.
        if ( KTIntNotDefined == configuration.iFilterIdArray[0] &&
                GPDS_TFT_DELETE_PACKET_FILTERS == tftOperation )
            {
            // do nothing - this is the only case when no
            // GPDS_TFT_INFO is added to the message
            }
        else
            {
            // preamble
            // Add GPDS_TFT_INFO sub block
            TUint8 numOfSbsInsideSb( 0 );
            TBuf8<KMaximumLengthOfGpdsTftInfo> gpdsTftInfoSbBuffer;
            TIsiSubBlock gpdsTftInfoSubBlock(
                gpdsTftInfoSbBuffer,
                GPDS_TFT_INFO,
                EIsiSubBlockTypeId8Len8 );

            gpdsTftInfoSbBuffer.Append( tftOperation );

            if( NULL != mediaAuthorizationArray &&
                ( 0 <= mediaAuthorizationArray->Count() ) )
                {
                // Add sub blocks to GPDS_TFT_INFO sub block
                AddMediaAuthorizationInfo( mediaAuthorizationArray,
                    gpdsTftInfoSbBuffer, numOfSbsInsideSb );
                }
            // no else

            if ( NULL == aPacketFilterPtr &&  NULL != mediaAuthorizationArray )
                {
                // Complete GPDS_TFT_INFO sub block and set sub block count
                TBuf8<KSubBlockCount1> sbCountbuf;
                sbCountbuf.Append( numOfSbsInsideSb );
                gpdsTftInfoSbBuffer.Insert(
                    GPDS_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS, sbCountbuf );
                gpdsContextModifyReq.CopyData(
                    currentMsgOffset, gpdsTftInfoSubBlock.CompleteSubBlock() );
                // Set new message offset and increase messages subblock count
                currentMsgOffset =
                    currentMsgOffset + gpdsTftInfoSbBuffer.Length();
                numOfSbInMessage++;

                // Initialise message buffer again for future use
                numOfSbsInsideSb = 0;
                gpdsTftInfoSbBuffer.Delete(
                    GPDS_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS,
                    ( gpdsTftInfoSbBuffer.Length() -
                      GPDS_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS ) );
                }
            // no else

            if ( GPDS_TFT_DELETE_PACKET_FILTERS == tftOperation )
                {

                // Add GPDS_TFT_FILTER_INFO sub blocks to GPDS_TFT_INFO sub block
                // Count number of valid ID's from DeleteID array and
                // adding ID's to buffer
                for ( TUint8 i = 0; ( i < KMaxNumberOfTftFilters ) &&
                    ( KTIntNotDefined != configuration.iFilterIdArray[i] ); i++ )
                    {
                    AppendGpdsTftFilterInfo( configuration.iFilterIdArray[i],
                            gpdsTftInfoSbBuffer, numOfSbsInsideSb );
                    }

                // Add GPDS_TFT_INFO subblock, which includes tftInfo buffer and
                // number of removed filters
                // Complete GPDS_TFT_INFO sub block and set sub block count
                TBuf8<KSubBlockCount1> sbCountBuff;
                sbCountBuff.Append( numOfSbsInsideSb );
                gpdsTftInfoSbBuffer.Insert(
                    GPDS_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS, sbCountBuff );
                gpdsContextModifyReq.CopyData(
                    currentMsgOffset, gpdsTftInfoSubBlock.CompleteSubBlock() );
                // Set new message offset and increase messages subblock count
                currentMsgOffset =
                    currentMsgOffset + gpdsTftInfoSbBuffer.Length();
                numOfSbInMessage++;
                }

           // If given aTftOperation is "CreateNew" or "AddPacketFilters"
           // of "ReplacePacketFilters"
           else if ( ( GPDS_TFT_CREATE_NEW == tftOperation ) ||
                ( GPDS_TFT_ADD_PACKET_FILTERS == tftOperation ) ||
                ( GPDS_TFT_REPLACE_PACKET_FILTERS == tftOperation ) )
                {
                // If there is one or more PacketFilters
                if ( NULL != aPacketFilterPtr )
                    {
                    // Initialize variable which indicates current filter
                    TUint8 filterIndex( 0 );
                    // Copy all given PacketFilters to gpdsContextModifyReq
                    // There can be from 1 to 8 PacketFilters
                    while ( filterIndex < aPacketFilterPtr->Count() )
                        {
                        AddPacketFilterInfo(
                            pdpType,
                            &( aPacketFilterPtr->At( filterIndex ) ),
                            gpdsTftInfoSbBuffer );
                        // Increasing filterIndex by one, pointing to
                        // the next PacketFilter item
                        filterIndex++;

                        numOfSbsInsideSb++;
                        }
                    }
                // no else

                // Add GPDS_TFT_INFO subblock, which includes tftInfo
                // buffer and total number of PacketFilter items
                // Complete GPDS_TFT_INFO sub block and set sub block count
                TBuf8<KSubBlockCount1> sbCountBuff;
                sbCountBuff.Append( numOfSbsInsideSb );
                gpdsTftInfoSbBuffer.Insert(
                    GPDS_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS, sbCountBuff );
                gpdsContextModifyReq.CopyData(
                    currentMsgOffset, gpdsTftInfoSubBlock.CompleteSubBlock() );
                // Set new message offset and increase messages subblock count
                currentMsgOffset =
                    currentMsgOffset + gpdsTftInfoSbBuffer.Length();
                numOfSbInMessage++;
                }
            // no special handling is required for GPDS_TFT_NO_OPERATION and
            // GPDS_TFT_DELETE_EXISTING besides what is done in preamble
            // no else
            }

        // If there were no errors
        if ( KErrNone == ret )
            {
            // Set number of subblocs in GPDS_CONTEXT_MODIFY_REQ message
            gpdsContextModifyReq.Set8bit(
                ISI_HEADER_SIZE +
                GPDS_CONTEXT_MODIFY_REQ_OFFSET_NUMBEROFSUBBLOCKS,
                numOfSbInMessage );
            // Send GPDS_CONTEXT_MODIFY_REQ message
            ret = iPhoNetSender->Send( gpdsContextModifyReq.Complete() );
            }
        // no else
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GpdsContextModifyResp
// This method breaks the response to the context modify request.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::GpdsContextModifyResp(
    const TIsiReceiveC& aIsiMessage )
    {
    TUint8 cause ( GPDS_CAUSE_UNKNOWN );
    TUint8 rejectionCause( GPDS_CAUSE_UNKNOWN );

    // Local link configure resp message
    TUint sbStartOffSet( 0 );

    TInt errorValue( KErrNone );
    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_MODIFY_RESP_OFFSET_CID ) );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::GpdsContextModifyResp. Context id:%d", contextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTMODIFYRESP, "CMmPacketContextMessHandler::GpdsContextModifyResp;contextId=%hhu", contextId );

    sbStartOffSet = 0;

    if( GPDS_CID_VOID != contextId )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextModifyResp. Context != GPDS_CID_VOID");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTMODIFYRESP, "CMmPacketContextMessHandler::GpdsContextModifyResp, Context != GPDS_CID_VOID" );

        // Get transaction id
        TUint8 transactionId( aIsiMessage.Get8bit(
           ISI_HEADER_SIZE + GPDS_CONTEXT_MODIFY_RESP_OFFSET_UTID ) );
        TUint8 modifyTraId( GenerateTraId( contextId ) );

        if( modifyTraId == transactionId )
            {
            // Get status
            TUint8 status( aIsiMessage.Get8bit(
                ISI_HEADER_SIZE + GPDS_CONTEXT_MODIFY_RESP_OFFSET_STATUS ) );

            if ( GPDS_OK != status )
                {
                cause = aIsiMessage.Get8bit( sbStartOffSet +
                    GPDS_CONTEXT_MODIFY_RESP_OFFSET_CAUSE );
                errorValue = CMmStaticUtility::PacketDataCSCauseToEpocError(
                    cause );
                }
            // no else

            if ( GPDS_CAUSE_AUTHENTICATION == cause )
                {
                TInt retValue( aIsiMessage.FindSubBlockOffsetById(
                    ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_MODIFY_RESP,
                    GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO,
                    EIsiSubBlockTypeId8Len8,
                    sbStartOffSet ) );

                if( KErrNone == retValue)
                    {
                    rejectionCause = aIsiMessage.Get8bit( sbStartOffSet +
                    GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO_OFFSET_REJECTIONCAUSE
                   );

                    errorValue =
                        CMmStaticUtility::PacketDataCSCauseToEpocError(
                            cause );
                    }
                // no else
                }
            // no else

            // CompleteModifyActiveContext
            TInfoName contextName;
            TBuf8<1> rejectionCauseBuf;
            rejectionCauseBuf.Zero();
            rejectionCauseBuf.Append( rejectionCause );

            TFLOGSTRING4("TSY: CMmPacketContextMessHandler::GpdsContextModifyResp. status:%d, errorValue:%d, rejCause:%d", status, errorValue, rejectionCause);
OstTraceExt3( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_GPDSCONTEXTMODIFYRESP, "CMmPacketContextMessHandler::GpdsContextModifyResp;status=%hhu;errorValue=%d;rejectionCause=%hhu", status, errorValue, rejectionCause );

            // Make sure that buffer is empty if rejection code is not there
            if( KErrNone == rejectionCause )
                {
                rejectionCauseBuf.Zero();
                }
            // no else

            TInt found( iContextList->
                GetContextNameByContextId( contextId, contextName ) );

            if( KErrNone == found )
               {
                CMmDataPackage data;
                data.PackData( &contextName, &rejectionCauseBuf );
                iMessageRouter->Complete(
                    EPacketContextModifyActiveContext,
                    &data,
                    errorValue );
               }
            // no else
            }
        // no else
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AddPacketFilterInfo
// Add packet filter info to Configure or Modify Req Isi-message.
// -----------------------------------------------------------------------------
//
TUint16 CMmPacketContextMessHandler::AddPacketFilterInfo(
    const TUint8 aPdpType,
    RPacketContext::TPacketFilterV2* aFilter,
    TDes8& aBuffer ) const
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo" );

    // Initialize variable which indicates number of components
    // in PacketFilter
    TUint8 numberOfSubSubBlocks( 0 );
    // Create tftFilterInfo
    TBuf8<KMaximumLengthOfGpdsTftFilterInfo> tftFilterInfo;
    TIsiSubBlock tftFilterInfoSubBlock(
        tftFilterInfo,
        GPDS_TFT_FILTER_INFO,
        EIsiSubBlockTypeId8Len8 );
    // Add TftFilterInfo parameters into buffer
    // In API docs ID can have values from 1 to 8 and
    // in GPDS docs it can have values from 0 to 7
    TUint8 id( static_cast<TUint8>( aFilter->iId - KFilterArray ) );
    tftFilterInfo.Append( id );
    TUint8 evaluationPrecedenceIndex( static_cast<TUint8>(
        aFilter->iEvaluationPrecedenceIndex - KFilterArray ) );
    tftFilterInfo.Append( evaluationPrecedenceIndex );
    tftFilterInfo.Append( KGpdsPadding );
    tftFilterInfo.Append( KGpdsPadding );
    tftFilterInfo.Append( KGpdsPadding );

    // FlowLabel value can exist only when IPv6 Address is included.
    // Checking also is given value valid. Value 0 means Optional
    if ( ( 0 < aFilter->iFlowLabel ) && ( GPDS_PDP_TYPE_IPV6 == aPdpType ) )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. GPDS_PDP_TYPE_IPV6");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, GPDS_PDP_TYPE_IPV6" );

        AppendGpdsFilterFlowLabelInfo( *aFilter, tftFilterInfo,
                                       numberOfSubSubBlocks );
        }
    // no else

    // If TOSorTrafficClass have valid value
    // Value 0 means Optional
    if ( 0 < aFilter->iTOSorTrafficClass )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. TOSorTrafficClass have valid value");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, TOSorTrafficClass have valid value" );

        AppendGpdsFilterTosInfo( *aFilter, tftFilterInfo, numberOfSubSubBlocks );
        }
    // no else

    // If IPSecSPI have valid value
    // Value 0 means Optional
    if ( 0 < aFilter->iIPSecSPI )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. IPSecSPI have valid value");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, IPSecSPI have valid value" );

        AppendGpdsFilterSpiInfo( *aFilter, tftFilterInfo, numberOfSubSubBlocks );
        }
    // no else

    // If SrcPortMin and SrcPortMax both have valid value
    // Value 0 means Optional
    if ( ( 0 < aFilter->iSrcPortMin ) && ( 0 < aFilter->iSrcPortMax ) )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. SrcPortMin and SrcPortMax both have valid value");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, SrcPortMin and SrcPortMax both have valid value" );

        AppendGpdsFilterSrcPortRangeInfo( *aFilter, tftFilterInfo,
                                          numberOfSubSubBlocks );
        }
    // no else

    // If DestPortMin and DestPortMax both have valid value
    // Value 0 means Optional
    if ( ( 0 < aFilter->iDestPortMin ) && ( 0 < aFilter->iDestPortMax ) )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. DestPortMin and DestPortMax both have valid value");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, DestPortMin and DestPortMax both have valid value" );

        AppendGpdsFilterDstPortRangeInfo( *aFilter, tftFilterInfo,
                                          numberOfSubSubBlocks );
        }
    // no else

    // If ProtocolNumberOrNextHeader have valid value
    // Value 0 means Optional
    if ( 0 < aFilter->iProtocolNumberOrNextHeader )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. ProtocolNumberOrNextHeader have valid value");
OstTrace0( TRACE_NORMAL, DUP6_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, ProtocolNumberOrNextHeader have valid value" );

        AppendGpdsFilterProtocolInfo( *aFilter, tftFilterInfo,
                                      numberOfSubSubBlocks );
        }
    // no else

    // If given aPdpType indicates the Src Addr is type IPv4
    if ( GPDS_PDP_TYPE_IPV4 == aPdpType )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. aPdpType == GPDS_PDP_TYPE_IPV4");
OstTrace0( TRACE_NORMAL, DUP7_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, aPdpType == GPDS_PDP_TYPE_IPV4" );

        AppendGpdsFilterSrcIPv4AddressInfo( *aFilter, tftFilterInfo,
                                            numberOfSubSubBlocks );
        }
    // If given aPdpType indicates the Src Addr is type IPv6
    else if ( GPDS_PDP_TYPE_IPV6 == aPdpType )
        {
        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. aPdpType == GPDS_PDP_TYPE_IPV6");
OstTrace0( TRACE_NORMAL, DUP8_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo, aPdpType == GPDS_PDP_TYPE_IPV6" );

        AppendGpdsFilterSrcIPv6AddressInfo( *aFilter, tftFilterInfo,
                                            numberOfSubSubBlocks );
        }
    // no else

    // set number of sub sub blocks to GPDS_TFT_FILTER_INFO
    TBuf8<KSubBlockCount1> sbCountBuf;
    sbCountBuf.Append( numberOfSubSubBlocks );
    tftFilterInfo.Insert(
        GPDS_TFT_FILTER_INFO_OFFSET_NUMBEROFSUBBLOCKS, sbCountBuf );
    // Add GPDS_TFT_FILTER_INFO subblock
    aBuffer.Append( tftFilterInfoSubBlock.CompleteSubBlock() );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::AddPacketFilterInfo. Return tftFilterInfo length: %d", tftFilterInfo.Length() );
OstTrace1( TRACE_NORMAL, DUP9_CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTERINFO, "CMmPacketContextMessHandler::AddPacketFilterInfo;Return tftFilterInfo length=%d", tftFilterInfo.Length() );

    return static_cast<TUint16>( tftFilterInfo.Length() );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::SetAuthenticationConfigReq
// Create an Isi-message for seting context authentication configuration.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::SetAuthenticationConfigReq(
    const TUint8 aTransactionId,
    const TUint8 aContextId,
    RPacketContext::TAuthInfo aAuthInfo,
    RPacketContext::TAuthData aResponse,
    RPacketContext::TAuthData aChallenge,
    TUint8  aId )
    {

    TFLOGSTRING3("TSY: CMmPacketContextMessHandler::SetAuthenticationConfigReq. TransId: %d ContextId: %d", aTransactionId, aContextId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_SETAUTHENTICATIONCONFIGREQ, "CMmPacketContextMessHandler::SetAuthenticationConfigReq;aTransactionId=%hhu;aContextId=%hhu", aTransactionId, aContextId );

    TInt ret( KErrNone );

    // Create isi message
    TIsiSend gpdsContextAuthReq( iPhoNetSender->SendBufferDes() );
    gpdsContextAuthReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_GPDS );

    // Set the Gpds Context Auth Req data
    gpdsContextAuthReq.Set8bit(
        ISI_HEADER_OFFSET_TRANSID, aTransactionId );
    gpdsContextAuthReq.Set8bit(
        ISI_HEADER_OFFSET_MESSAGEID, GPDS_CONTEXT_AUTH_REQ );
    gpdsContextAuthReq.Set8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_AUTH_REQ_OFFSET_CID,
        aContextId );

    // Initialize message offset and subblock count
    TInt currentMsgOffset(
        ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_AUTH_REQ );
    TUint8 numOfSbInMessage( 0 );

    if ( RPacketContext::KMaxAuthDataLength >=
            aAuthInfo.iUsername.Length() )
        {

        TFLOGSTRING2("TSY: CMmPacketContextMessHandler::SetAuthenticationConfigReq. aAuthInfo.iUsername.Length: %d", aAuthInfo.iUsername.Length());
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_SETAUTHENTICATIONCONFIGREQ, "CMmPacketContextMessHandler::SetAuthenticationConfigReq;aAuthInfo.iUsername.Length=%d", aAuthInfo.iUsername.Length() );

        // Add GPDS_USERNAME_INFO subblock
        // Set the Username info
        // Length must correspond to KMaxAuthDataLength (+6 for ISI message)
        const TInt KBytesForISIMessage( 6 );
        TBuf8<RPacketContext::KMaxAuthDataLength + KBytesForISIMessage>
            usernameInfo;
        TIsiSubBlock usernameInfoSubBlock(
            usernameInfo,
            GPDS_USERNAME_INFO,
            EIsiSubBlockTypeId8Len8 );

        usernameInfo.Append(
            static_cast<TUint8>( aAuthInfo.iUsername.Length() ) );

        // If username length is more than zero, append authInfo.iUsername
        if ( 0 < aAuthInfo.iUsername.Length() )
            {
            usernameInfo.Append( aAuthInfo.iUsername );
            }
        // no else

        gpdsContextAuthReq.CopyData(
            currentMsgOffset, usernameInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + usernameInfo.Length();
        numOfSbInMessage++;
        }

    // Gpds limits the combined length of username + password
    if ( ( RPacketContext::EProtocolPAP == aAuthInfo.iProtocol ) &&
        ( RPacketContext::KMaxAuthDataLength >=
            aAuthInfo.iUsername.Length() ) &&
        ( RPacketContext::KMaxAuthDataLength >=
            aAuthInfo.iPassword.Length() ) )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetAuthenticationConfigReq. Protocol EProtocolPAP");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_SETAUTHENTICATIONCONFIGREQ, "CMmPacketContextMessHandler::SetAuthenticationConfigReq, Protocol EProtocolPAP" );

        // Add GPDS_PASSWORD_INFO subblock
        // Set the Password info
        // Length must correspond to KMaxAuthDataLength (+6 for isi message)
        TBuf8<RPacketContext::KMaxAuthDataLength + 6> passwordInfo;
        TIsiSubBlock pswdInfoSubBlock(
            passwordInfo,
            GPDS_PASSWORD_INFO,
            EIsiSubBlockTypeId8Len8 );

        passwordInfo.Append(
            static_cast<TUint8>( aAuthInfo.iPassword.Length() ) );

        // If password length is more than zero, append authInfo.iPassword
        if ( 0 < aAuthInfo.iPassword.Length() )
            {
            passwordInfo.Append( aAuthInfo.iPassword );
            }
        // no else

        gpdsContextAuthReq.CopyData(
            currentMsgOffset, pswdInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + passwordInfo.Length();
        numOfSbInMessage++;
        }
    // Gpds limits the combined length of username + response
    else if ( ( RPacketContext::EProtocolCHAP == aAuthInfo.iProtocol ) &&
        ( RPacketContext::KMaxAuthDataLength >=
            aAuthInfo.iUsername.Length() ) &&
        ( RPacketContext::KMaxAuthDataLength >= aResponse.Length() ) )
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetAuthenticationConfigReq. Protocol EProtocolCHAP");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_SETAUTHENTICATIONCONFIGREQ, "CMmPacketContextMessHandler::SetAuthenticationConfigReq, Protocol EProtocolCHAP" );

        // Add GPDS_CHALLENGE_INFO subblock
        // Set the Challenge info
        // Length must correspond to KMaxAuthDataLength (+6 for ISI message)
        TBuf8<RPacketContext::KMaxAuthDataLength + 6> challengeInfo;
        TIsiSubBlock challengeInfoSubBlock(
            challengeInfo,
            GPDS_CHALLENGE_INFO,
            EIsiSubBlockTypeId8Len8 );

        // Add length
        challengeInfo.Append(
            static_cast<TUint8>( aChallenge.Length() ) );

        // If challenge length is more than zero, append authInfo.iChallenge
        if ( 0 < aChallenge.Length() )
            {
            challengeInfo.Append( aChallenge );
            }
        // no else

        gpdsContextAuthReq.CopyData(
            currentMsgOffset, challengeInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + challengeInfo.Length();
        numOfSbInMessage++;

        // Add GPDS_RESPONSE_INFO subblock
        // Set the Response info
        // Length must correspond to KMaxAuthDataLength (+6 for isi message)
        TBuf8<RPacketContext::KMaxAuthDataLength + 6> responseInfo;
        TIsiSubBlock respInfoSubBlock(
            responseInfo,
            GPDS_RESPONSE_INFO,
            EIsiSubBlockTypeId8Len8 );

        // Add Id
        responseInfo.Append( aId );
        responseInfo.Append(
            static_cast<TUint8>( aResponse.Length() ) );

        // If response length is more than zero, append authInfo.iResponse
        if ( 0 < aResponse.Length() )
            {
            responseInfo.Append( aResponse );
            }
        // no else

        gpdsContextAuthReq.CopyData(
            currentMsgOffset, respInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + responseInfo.Length();
        numOfSbInMessage++;
        }
    else
        {

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetAuthenticationConfigReq. return KErrTooBig");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_SETAUTHENTICATIONCONFIGREQ, "CMmPacketContextMessHandler::SetAuthenticationConfigReq,return KErrTooBig" );

        ret = KErrTooBig;
        }

    if ( KErrNone == ret )
        {
        // Set number of subblocs in GPDS_CONTEXT_AUTH_REQ message
        gpdsContextAuthReq.Set8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_AUTH_REQ_OFFSET_NUMBEROFSUBBLOCKS,
            numOfSbInMessage );

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetAuthenticationConfigReq. Send GPDS_CONTEXT_AUTH_REQ message");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETCONTEXTMESSHANDLER_SETAUTHENTICATIONCONFIGREQ, "CMmPacketContextMessHandler::SetAuthenticationConfigReq, Send GPDS_CONTEXT_AUTH_REQ message" );

        // Send GPDS_CONTEXT_AUTH_REQ message
        ret = iPhoNetSender->Send( gpdsContextAuthReq.Complete() );
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::ExtFuncL
// Entry point for messages received from Symbian OS layer (Message manager).
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::ExtFuncL(
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {

    TFLOGSTRING2( "TSY: CMmPacketContextMessHandler::ExtFuncL. IPC: %d", aIpc );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_EXTFUNCL, "CMmPacketContextMessHandler::ExtFuncL;aIpc=%d", aIpc );

    // add the usage of ASSERT MACRO for aDataPackage when it's available
    TInt ret( KErrNotSupported );

    switch ( aIpc )
        {
        case EPacketContextGetDataVolumeTransferred:
            {
            ret = GpdsContextStatusReq( *aDataPackage );
            break;
            }
        case EPacketContextInitialiseContext:
            {
            ret = InitialiseContext( *aDataPackage );
            break;
            }
        case EPacketContextDelete:
            {
            ret = Delete( *aDataPackage );
            break;
            }
        case EPacketContextSetConfig:
            {
            ret = SetConfigL( *aDataPackage );
            break;
            }
        case EPacketContextActivate:
            {
            ret = iMmPipeController->PnsPipeEnableReq( *aDataPackage );
            break;
            }
        case EPacketContextDeactivate:
            {
            ret = GpdsContextDeactivateReq( *aDataPackage );
            break;
            }
        case EPacketContextAddPacketFilter:
            {
            ret = AddPacketFilter( aDataPackage );
            break;
            }

        case EPacketContextModifyActiveContext:
            {
            ret = GpdsContextModifyReq( aDataPackage );
            break;
            }

        case EPacketAddMediaAuthorization:
            {
            ret = AddMediaAuthorizationL( aDataPackage );
            break;
            }
        case ECustomSetDriveModeIPC:
            {
            //packed parameter: TSetDriveMode ( mode status )
            RMmCustomAPI::TSetDriveMode modeStatus;
            aDataPackage->UnPackData( modeStatus );
            // Setting DCM drive mode flag on / off.
            if ( RMmCustomAPI::EDeactivateDriveMode == modeStatus )
                {
                iDriveModeFlag = EFalse;
                iMessageRouter->Complete( ECustomSetDriveModeIPC, KErrNone );
                }
            else
                {
                iDriveModeFlag = ETrue;
                iMessageRouter->Complete( ECustomSetDriveModeIPC, KErrNone );
                }
            ret = KErrNone;
            break;
            }
        default:
            {

            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);

OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_EXTFUNCL, "CMmPacketContextMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::InitialiseContext
// This function starts the three phase context initialisation.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::InitialiseContext(
    const CMmDataPackage& aDataPackage )
    {

    TFLOGSTRING( "TSY: CMmPacketContextMessHandler::InitialiseContext");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_INITIALISECONTEXT, "CMmPacketContextMessHandler::InitialiseContext" );

    TUint8 channelId( KTUint8NotDefined );
    TInfoName* contextName = NULL;
    TInfoName* hostCidName = NULL;
    TInt ret( KErrNone );
    iInitialiseError = KErrNone;
    TUint8 objectId( 0 );

    aDataPackage.UnPackData( &contextName, &hostCidName );

    // don't allow new context creation if allowing incoming call
    // (for allow incoming call feature)
    if ( CALL_MODEM_ID_NONE != iCallModemResourceCallId )
        {
        ret = KErrServerBusy;
        }
    //no else

    if( KErrNone == ret )
        {
        ret = iContextList->GenerateProxyId( channelId );

        if( KErrNone == ret )
            {
            TBuf8<KBuffProxyIdForChannel> proxyIdForChannel;
            TInt ret = iPhoNetSender->
                GetChannelInfo( channelId, proxyIdForChannel );

            if( KErrNone == ret  )
                {
                // Object id is taken from proxyIdForChannel bytes 0 and 1.
                objectId = ( static_cast<TUint16>(
                    proxyIdForChannel[0]) << KShift8Bits ) +
                        proxyIdForChannel[1];

                ret = iContextList->AddObject( objectId, channelId,
                    // 1. object id 2. channel id ( proxy )
                    *contextName, *hostCidName );
                }
            // no else
            }
        // no else

        if( KErrNone == ret  )
            {
            TUint8 pipehandle( iContextList->
                PipeHandleByContextName( contextName ) );

            if ( KInvalidPipeHandle == pipehandle )
                {

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
                // Create unique trans id
                // This is due all context have same proxyId in testing
                // environment and valid contextId not yet available
                TInt proxyInt( static_cast<TInt>( channelId ) );
                TInt tempInt( proxyInt - iTransIdForProxies );
                channelId = static_cast<TUint8>( tempInt );
                iTransIdForProxies++;
#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

                ret = iContextList->OpenChannel( channelId );
                if ( KErrNone == ret )
                    {
                    ret = iContextList->
                        SetPipeCreateTransId( channelId, contextName );
                    }

                if( KErrNone == ret )
                    {
                    TUint8 pep1ObjectNum( iContextList->
                        ProxyIdByContextName( contextName ) );

                    ret = iMmPipeController->PnsPipeCreateReq(
                        channelId, PN_PIPE_DISABLE, objectId);

                    if ( KErrNone == ret )
                        {
                        ret = iContextList->
                            SetCreateContextIdTransId( objectId , contextName);
                        }
                    // no else
                    }
                // no else
               }
            else
                {
                // Pipe has been already created. E.g. Initialise has been
                // called once but it did not succeed, and then the pipe has
                // been tried to remove but the removing did not succeed.
                // Hence we can strike straight into InitialiseContextPhase2L.
                iContextList->
                    SetCreateContextIdTransId( objectId , contextName );

                ret = InitialiseContextPhase2( pipehandle );
                // iInitialiseError is set at InitialiseContextPhase2L() if
                // there occured error
                if ( KErrNone != ret )
                    {
                    ret = iInitialiseError;
                    }
                // no else
                }
            }
        // no else
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::InitialiseContextPhase2
// This function starts the second phase of context initialisation.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::InitialiseContextPhase2(
    TUint8 aPipeHandle )
    {

    TFLOGSTRING("TSY: CMmPacketContextGsmWcdmaExt::InitialiseContextPhase2.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_INITIALISECONTEXTPHASE2, "CMmPacketContextMessHandler::InitialiseContextPhase2" );

    TInt ret( KErrNone );
    TUint8 createContextIdTransId( iContextList->
        CreateContextIdTransIdByPipeHandle( aPipeHandle ) );

    if( KTUint8NotDefined !=  createContextIdTransId )
        {
        ret = GpdsContextIdCreateReq( createContextIdTransId );

        if ( KErrNone != ret )
            {
            iInitialiseError = ret;

            ret = iMmPipeController->PnsPipeRemoveReq(
                createContextIdTransId, aPipeHandle );
            if ( KErrNone == ret )
                {
                // Store the trans id
                ret = iContextList->
                    SetRemovePipeTraId( createContextIdTransId, aPipeHandle );
                }
            // no else

            TFLOGSTRING2("TSY: CMmPacketContextGsmWcdmaExt::InitialiseContextPhase2. Error(0=OK) %d", ret );

OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_INITIALISECONTEXTPHASE2, "CMmPacketContextMessHandler::InitialiseContextPhase2;Error(0=OK) %d", ret );
            }
        // no else
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::InitialiseContextPhase3
// This function starts the third phase of the context initialisation.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::InitialiseContextPhase3(
    const TUint8 aStatus,
    const TUint8 aContextId )
    {

    TFLOGSTRING3("TSY: CMmPacketContextGsmWcdmaExt::InitialiseContextPhase3. ContextId:%d aStatus:%d", aContextId, aStatus );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_INITIALISECONTEXTPHASE3, "CMmPacketContextMessHandler::InitialiseContextPhase3;aContextId=%d;aStatus=%d", aContextId, aStatus );

    TInt ret( KErrGeneral );

    if ( GPDS_OK == aStatus )
        {
        // get pipehandle
        TUint8 pipeHandle( iContextList->PipeHandleByContextId( aContextId ) );
        if ( KInvalidPipeHandle != pipeHandle )
            {
            TUint8 gpdsLlConfigureTraId( GenerateTraId( aContextId ) );

            if( KTUint8NotDefined != gpdsLlConfigureTraId )
                {
                ret = GpdsLlConfigureReq(
                    gpdsLlConfigureTraId, aContextId, pipeHandle );
                }
            // no else

            if ( KErrNone == ret )
                {
                ret = iContextList->
                    SetGpdsLlConfigureTraId( gpdsLlConfigureTraId, aContextId );
                }
            // no else
            }
        else
            {
            // pipehandle not found
            ret = KErrNotFound;
            }
        }
    // no else

    if ( KErrNone != ret )
        {
        TInfoName contextname;
        CMmDataPackage data;
        TInt found( iContextList->
            GetContextNameByContextId( aContextId, contextname ) );

        TFLOGSTRING2("TSY: CMmPacketContextGsmWcdmaExt::InitialiseContextPhase3. Error:%d", ret );
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_INITIALISECONTEXTPHASE3, "CMmPacketContextMessHandler::InitialiseContextPhase3;Error=%d", ret );

        iInitialiseError = ret;
        // Remove the pipe
        // Context not found->pipehandle not found
        if( KErrNotFound == ret )
            {
            TUint8 pipeRemoveTraId( GenerateTraId( aContextId ) );
            TUint8 pipeHandle( iContextList->
                PipeHandleByContextId( aContextId ) );

            ret = iMmPipeController->PnsPipeRemoveReq(
                pipeRemoveTraId, pipeHandle );

            if ( KErrNone == ret )
                {
                // Store the trans id
                ret = iContextList->
                    SetRemovePipeTraId( pipeRemoveTraId, pipeHandle );
                }
            else
                {
                if( KErrNone == found )
                    {
                    // Complete initialisation; Context not found
                    data.PackData( &contextname );
                    iMessageRouter->Complete(
                        EPacketContextInitialiseContext,
                        &data,
                        KErrNotFound );
                    }
                // no else
                }
            }
        // some error in gprs server
        else if( KErrGeneral == ret )
            {
            if( KErrNone == found )
                {
                if ( GPDS_ERROR == aStatus )
                    {
                    // If GPDS_STATUS in GPDS_CONTEXT_ID_CREATE_RESP is
                    // GPDS_ERROR, max number of contexts already in use
                    // (~=ALL_PIPES_IN_USE). => remove pipe if created
                    TUint8 pipeRemoveTraId( GenerateTraId( aContextId ) );
                    TUint8 pipeHandle( iContextList->
                        PipeHandleByContextName( &contextname ) );

                    ret = iMmPipeController->PnsPipeRemoveReq(
                        pipeRemoveTraId, pipeHandle );

                    if ( KErrNone == ret )
                        {
                        // Store the trans id
                        ret = iContextList->
                            SetRemovePipeTraId( pipeRemoveTraId, pipeHandle );
                        }
                    ret = CMmStaticUtility::EpocErrorCode(
                        KErrOverflow, KErrUmtsMaxNumOfContextExceededByPhone );
                     }
                // no else
                data.PackData( &contextname );
                iMessageRouter->Complete(
                    EPacketContextInitialiseContext,
                    &data,
                    ret );
                }
            // no else
            }
        // no else
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::CompletePipeOperation
// Handles pipe related response messages and completes operations.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::CompletePipeOperation(
    const TInt aMessageId,
    const TUint8 aTransactionId,
    const TUint8 aPipeHandle,
    const TUint8 aErrorCause )
    {

    TFLOGSTRING3("TSY: CMmPacketContextMessHandler::CompletePipeOperation. MessageId:%d Pipe handle:%d", aMessageId, aPipeHandle );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation;aMessageId=%d;aPipeHandle=%hhu", aMessageId, aPipeHandle );
    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::CompletePipeOperation. Error cause:%d", aErrorCause );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation;aErrorCause=%hhu", aErrorCause );

    TInt errorCause( KErrNone );

    if ( PN_PIPE_NO_ERROR != aErrorCause )
        {
        if ( ( PNS_PIPE_CREATE_RESP == aMessageId ) &&
             ( PN_PIPE_ERR_ALL_PIPES_IN_USE == aErrorCause ) )
            {
            // If reason is that all pipe already used then use specific error
            errorCause = CMmStaticUtility::EpocErrorCode(
                KErrOverflow, KErrUmtsMaxNumOfContextExceededByPhone );
            }
        else
            {
            // Map the Pipe error to Epoc error
            errorCause = CMmStaticUtility::PacketDataCSCauseToEpocError(
                aErrorCause, PN_PIPE );
            }
        }
    // no else

    switch ( aMessageId )
        {
        case PNS_PIPE_CREATE_RESP:
            {
            if ( PN_PIPE_NO_ERROR == aErrorCause )
                {
                // Store the pipe handle
                TInt ret( iContextList->
                    SetPipeHandle( aTransactionId, aPipeHandle ) );

                if( ret == KErrNone )
                    {
                    // Call InitialiseContextPhase2L
                    ret = InitialiseContextPhase2( aPipeHandle );
                    if ( KErrNone != ret )
                        {
                        //This is executed if ContextId Createreq or
                        //piperemovereq or setcontextidTraId fails
                        //Get Contextname from CMmPacketMesshandlerContextList
                        TUint8 contextId( iContextList->
                            ContextIdByPipeHandle( aPipeHandle ) );

                        if( GPDS_CID_VOID != contextId )
                            {
                            TInfoName contextname;
                            TInt ret2( iContextList->GetContextNameByContextId(
                                contextId, contextname ) );

                            if( KErrNone == ret2  )
                                {
                                CMmDataPackage data;
                                data.PackData( &contextname );
                                iMessageRouter->Complete(
                                    EPacketContextInitialiseContext,
                                    &data,
                                    iInitialiseError );
                                }
                            // no else

                            if( KErrNone != ret2 )
                                {

                                TFLOGSTRING("TSY: CMmPacketContextMessHandler::CompletePipeOperation. Cannot Complete Initialisation");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation, Cannot Complete Initialisation" );
                                }
                            // no else
                            }
                        // no else
                        }
                    // no else
                    }
                // no else
                }
            else
                {
                if( PN_PIPE_ERR_ALL_PIPES_IN_USE == aErrorCause )
                    {
                    TInfoName contextname;
                    // Get context name by proxy id, i.e used transactionId id
                    // pipeCreateRequest
                    TInt ret( iContextList->
                        ContextNameByProxyId( aTransactionId, contextname ) );

                    if( KErrNone == ret  )
                        {
                        // Completion because of some error in PIPE
                        CMmDataPackage data;
                        data.PackData( &contextname );
                        iMessageRouter->Complete(
                            EPacketContextInitialiseContext,
                            &data,
                            errorCause );

                        // remove context from list
                        iContextList->RemoveObjectByName( &contextname );
                        }
                    // no else
                    if( KErrNone != ret )
                        {

                        TFLOGSTRING("LTSY: CMmPacketContextMessHandler::CompletePipeOperationL. Cannot Complete failed initialisation" );
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation, Cannot Complete failed initialisation" );
                        }
                    // no else
                    }
                else if( PN_PIPE_ERR_GENERAL == aErrorCause  )
                    {
                    TInfoName contextname;
                    // Get context name by proxy id, i.e used transactionId id
                    // pipeCreateRequest
                    TInt ret( iContextList->
                        ContextNameByProxyId( aTransactionId, contextname ) );

                    if( KErrNone == ret  )
                        {

                        TFLOGSTRING2("LTSY: CMmPacketContextMessHandler::CompletePipeOperationL.Complete context initialisation with %d", errorCause );

OstTraceExt1( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation;Complete context initialisation with=%hhu", aErrorCause );
                        //Completion because of some error in PIPE
                        CMmDataPackage data;
                        data.PackData( &contextname );
                        iMessageRouter->Complete(
                            EPacketContextInitialiseContext,
                            &data,
                            errorCause );

                        iContextList->RemoveObjectByName( &contextname );
                        }
                    // no else

                    if( KErrNone != ret )
                        {

                        TFLOGSTRING("LTSY: CMmPacketContextMessHandler::CompletePipeOperationL. Cannot Complete failed initialisation");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation, Cannot Complete failed initialisation" );
                        }
                    // no else
                    }
                else
                    {
                    // Call CompleteInitialiseContext with error
                    TUint8 contextId( iContextList->
                        ContextIdByPipeHandle( aPipeHandle ) );

                    if( GPDS_CID_VOID != contextId )
                        {
                        TInfoName contextname;
                        TInt ret( iContextList->
                            GetContextNameByContextId( contextId, contextname )
                            );

                        if( KErrNone == ret  )
                            {
                            //Completion because of some error in PIPE
                            CMmDataPackage data;
                            data.PackData( &contextname );
                            iMessageRouter->Complete(
                                EPacketContextInitialiseContext,
                                &data,
                                errorCause );
                            }
                        // no else

                        if( KErrNone != ret )
                            {

                            TFLOGSTRING("TSY: CMmPacketContextMessHandler::CompletePipeOperation. Cannot Complete Initialisation");
OstTrace0( TRACE_NORMAL, DUP6_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation, Cannot Complete Initialisation" );
                            }
                        // no else
                        }
                    // no else
                    }
                }
            break;
            }
        case PNS_PIPE_ENABLE_RESP:
            {

            if ( PN_PIPE_NO_ERROR == aErrorCause )
                {
                // Pipe status is enabled
                iContextList->SetPipeStatus( aPipeHandle, PN_PIPE_ENABLE );
                // Call ActivatePhase2L
                ActivatePhase2( aPipeHandle, aTransactionId );
                }
            // no else

            // This is executed in special case when user has cancelled
            // activation, NIF's pep is desctructed and due that pipe server
            // returns an error in PNS_PIPE_ENABLE_RESP message.
            if ( PN_PIPE_ERR_GENERAL == aErrorCause )
                {
                TUint8 contextId( iContextList->
                    ContextIdByPipeHandle( aPipeHandle ) );

                TInfoName contextname;
                TInt ret( iContextList->
                    GetContextNameByContextId( contextId, contextname ) );

                if ( ret == KErrNone )
                    {

                    TFLOGSTRING("LTSY: CMmPacketContextMessHandler::CompletePipeOperationL.Complete ACTIVATION with KErrGeneral");
OstTrace0( TRACE_NORMAL, DUP7_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation, Complete ACTIVATION with KErrGeneral" );

                    CMmDataPackage data;
                    TAny* ptr = NULL;
                    data.PackData( &contextname, ptr );
                    iMessageRouter->Complete(
                        EPacketContextActivate,
                        &data,
                        KErrGeneral );
                    }
                // no else

                TUint8 pipeHandle( iContextList->
                    PipeHandleByContextName( &contextname ) );

                ret = iMmPipeController->
                    PnsPipeRemoveReq( aTransactionId, pipeHandle );

                iContextList->RemoveObjectByName( &contextname );
                }
            // no else
            break;
            }
        case PNS_PIPE_RESET_RESP:
            {
            if ( PN_PIPE_NO_ERROR == aErrorCause )
                {
                // Pipe status is disabled
                iContextList->SetPipeStatus( aPipeHandle, PN_PIPE_DISABLE );
                }
            // no else
            break;
            }
        case PNS_PIPE_REMOVE_RESP:
            {
            TUint8 channelId( iContextList->ProxyIdByPipeHandle( aPipeHandle ) );
            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::CompletePipeOperation;closing channelId=%d", channelId);
            OstTraceExt1( TRACE_NORMAL, DUP9_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation;closing channelId=%hhu", channelId );
            iContextList->CloseChannel( channelId );
            TUint8 contextId( iContextList->
                ContextIdByPipeHandle( aPipeHandle ) );

            if( iContextList->GetInitialiseMember( aPipeHandle ) )
                {
                //context id not found
                if( GPDS_CID_VOID != contextId)
                    {
                    iContextList->SetInitialiseMember( contextId, EFalse );
                    TInfoName contextName;
                    iContextList->
                        GetContextNameByContextId( contextId, contextName );

                    CMmDataPackage data;
                    data.PackData( &contextName );
                    iMessageRouter->Complete(
                        EPacketContextDelete,
                        &data,
                        errorCause );
                    }
                // no else
                }
            else
                {
                // context id not found
                if( GPDS_CID_VOID != contextId)
                    {
                    iContextList->SetInitialiseMember( contextId, EFalse );
                    TInfoName contextName;
                    iContextList->
                        GetContextNameByContextId( contextId, contextName );

                    CMmDataPackage data;
                    data.PackData( &contextName );
                    iMessageRouter->Complete(
                        EPacketContextInitialiseContext,
                        &data,
                        iInitialiseError );
                    }
                // no else
                }

            if ( PN_PIPE_NO_ERROR == aErrorCause ||
                 PN_PIPE_ERR_GENERAL == aErrorCause )
                {
                // Reset pipe handle and pipe status
                iContextList->ResetPipeHandleAndStatus( aPipeHandle );

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::CompletePipeOperation.");
OstTrace0( TRACE_NORMAL, DUP8_CMMPACKETCONTEXTMESSHANDLER_COMPLETEPIPEOPERATION, "CMmPacketContextMessHandler::CompletePipeOperation" );

                iContextList->RemoveObject( contextId );
                }
            // no else
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::Delete
// Delete context related to the network e.g. pipe deletion.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::Delete(
    const CMmDataPackage& aDataPackage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::Delete");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_DELETE, "CMmPacketContextMessHandler::Delete" );

    TInt ret( KErrNone );
    TInfoName* contextName = NULL;
    aDataPackage.UnPackData( &contextName );

    TUint8 contextId( iContextList->
        GetContextIdByContextName( contextName ) );

    if ( GPDS_CID_VOID != contextId )
        {
        // transActionId creation.
        TUint8 transId( GenerateTraId( contextId ) );
        TUint8 pipeHandle( iContextList->
            PipeHandleByContextName( contextName ) );

        ret = iContextList->SetDeletionTraId(
            contextId, transId );
        if ( ret != KErrNotFound )
            {
            ret = iMmPipeController->PnsPipeRemoveReq( transId, pipeHandle );
            }
        // no else
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::SetConfigL
// Set configuration data for the context.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::SetConfigL(
    const CMmDataPackage& aDataPackage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetConfigL");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL" );

    TInt ret( KErrNone );
    TInfoName* contextName = NULL;
    TPacketDataConfigBase* config = NULL;

    aDataPackage.UnPackData( &config, &contextName );

    TUint8 contextId( iContextList->
        GetContextIdByContextName( contextName ) );

    iContextList->SetInitialiseMember( contextId, ETrue );

    // Check that the mode given in aConfig is GPRS
    if ( TPacketDataConfigBase::KConfigGPRS == config->ExtensionId() )
        {
        if ( GPDS_CID_VOID != contextId )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetConfig -> Rel97");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL -> Rel97" );

            iContextList->SetContextConfigurationType(
                contextId, TPacketDataConfigBase::KConfigGPRS );

            RPacketContext::TContextConfigGPRS& configGPRS =
                *reinterpret_cast<RPacketContext::TContextConfigGPRS*>(
                    config );

            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::SetConfig iPrimaryDns:%S", &configGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns );

OstTraceExt1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL;iPrimaryDns=%s", configGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns );
            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::SetConfig iSecondaryDns:%S", &configGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL;iSecondaryDns=%s", configGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns );

            // save authentication data
            ret = iContextList->SaveConfig(
                contextId,
                TPacketDataConfigBase::KConfigGPRS,
                &configGPRS,
                NULL );

            // if config mode was OK
            if( KErrNone == ret )
                {
                // Map the client side Pdp type to Gpds server's pdp type
                TUint8 pdpType( 0 );
                ret = MapPdpType( pdpType, configGPRS.iPdpType );

                // Anonymous access is not supported
                if ( ( KErrNone == ret ) && ( RPacketContext::ERequired ==
                    configGPRS.iAnonymousAccessReqd ) )
                    {
                    ret = KErrNotSupported;
                    }
                // no else

                if ( KErrNone == ret )
                    {
                    // DNS addresses requested all the time
                    TBool dnsAddressRequested( ETrue );

                    // Boolean for signalling info
                    TBool pcscfAddress( EFalse );

                    // Set data compression
                    TUint8 dataCompression( GPDS_PDP_DCOMP_OFF );

                    if ( configGPRS.iPdpCompression &
                        RPacketContext::KPdpDataCompression )
                        {
                        dataCompression = GPDS_PDP_DCOMP_ON;
                        }
                    // no else

                    // Set header compression
                    TUint8 headerCompression( GPDS_PDP_HCOMP_OFF );

                    if ( configGPRS.iPdpCompression &
                        RPacketContext::KPdpHeaderCompression )
                        {
                        headerCompression = GPDS_PDP_HCOMP_ON;
                        }
                    // no else

                    TUint8 contextType( GPDS_CONT_TYPE_NORMAL );
                    // primary or secondary context
                    if( !configGPRS.iNWIContext )
                        {
                        // MO initiated context
                        contextType = iContextList->
                            GetContextTypeByName( contextName );
                        }
                    else
                        {
                        // MT initiated context
                        contextType = GPDS_CONT_TYPE_NWI;
                        }

                    TInfoName hostCidName = iContextList->
                        GetHostCidName( contextName );
                    TUint8 primaryContextId( iContextList->
                        GetContextIdByContextName( &hostCidName ) );

                    TUint8 transActionId( GenerateTraId( contextId ) );

                    // use the iMiscBuffer to check P-CSCF discovery request
                    TInt isOnMiscBuffer( KErrNotFound );
                    RPacketContext::TPcoId pcoItem(
                        RPacketContext::EEtelPcktPCSCFAddressRequest );

                    TInt len( configGPRS.iProtocolConfigOption.
                        iMiscBuffer.Length() );

                    TPtr8 ptrToTLVdata( configGPRS.iProtocolConfigOption.
                        iMiscBuffer.LeftTPtr(
                            configGPRS.iProtocolConfigOption.iMiscBuffer.
                                MaxLength() ) );
                    ptrToTLVdata.SetLength( configGPRS.
                        iProtocolConfigOption.iMiscBuffer.Length());

                    const TInt KBigEnoughBuf( 1 );
                    TBuf8<KBigEnoughBuf> smallBuffer;
                    smallBuffer.SetLength( KBigEnoughBuf );
                    smallBuffer.FillZ();

                    TTlvStruct<RPacketContext::TPcoId,
                        RPacketContext::TPcoItemDataLength>tlv(
                            ptrToTLVdata, KPtrToTLVdata );

                    TPtr8 zeroSizePtr(
                        const_cast<TUint8*>( smallBuffer.Ptr() ),
                        smallBuffer.Length(),
                        smallBuffer.MaxLength() );

                    isOnMiscBuffer=tlv.NextItemL( pcoItem, zeroSizePtr );

                    if( KErrNone == isOnMiscBuffer )
                        {
                        // if pcscfAddress info exist
                        pcscfAddress = ETrue;
                        isOnMiscBuffer = KErrNotFound;
                        tlv.ResetCursorPos();
                        }
                    // no else

                    ret = GpdsContextConfigureReq(
                        transActionId, contextId, pdpType,
                        contextType, primaryContextId,
                        dataCompression, headerCompression,
                        configGPRS.iAccessPointName,
                        NULL, NULL, EFalse, pcscfAddress,
                        dnsAddressRequested );
                    }
                // no else
                }
            // no else
            }
        else
            {
            ret = KErrNotReady;
            }
        }

    else if ( TPacketDataConfigBase::KConfigRel99Rel4 == config->ExtensionId()
        || TPacketDataConfigBase::KConfigRel5 == config->ExtensionId() )
        {
        if ( GPDS_CID_VOID != contextId )
            {
            if ( TPacketDataConfigBase::KConfigRel5 == config->ExtensionId() )
                {

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetConfig -> R5");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL -> R5" );
                }
            else
                {

                TFLOGSTRING("TSY: CMmPacketContextMessHandler::SetConfig -> R99");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL -> R99" );
                }

            iContextList->SetContextConfigurationType(
                contextId, TPacketDataConfigBase::KConfigRel99Rel4 );

            RPacketContext::TContextConfigR99_R4& configR99_R4 =
                *reinterpret_cast<RPacketContext::TContextConfigR99_R4*>(
                    config );

            // save authentication data
            // No need to save REL5 type-> more complexity later as there is no
            // own class for Rel5 Config.
            ret = iContextList->SaveConfig( contextId,
                TPacketDataConfigBase::KConfigRel99Rel4,
                NULL,
                &configR99_R4 );

            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::SetConfig iMiscBuffer:%S", &configR99_R4.iProtocolConfigOption.iMiscBuffer );
OstTraceExt1( TRACE_NORMAL, DUP6_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL;iMiscBuffer=%s", configR99_R4.iProtocolConfigOption.iMiscBuffer );
            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::SetConfig iPrimaryDns:%S", &configR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns );
OstTraceExt1( TRACE_NORMAL, DUP7_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL;iPrimaryDns=%s", configR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns );
            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::SetConfig iSecondaryDns:%S",&configR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns );
OstTraceExt1( TRACE_NORMAL, DUP8_CMMPACKETCONTEXTMESSHANDLER_SETCONFIGL, "CMmPacketContextMessHandler::SetConfigL;iSecondaryDns=%s", configR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns );

            // Config saved succesfully
            if ( KErrNone == ret )
                {
                // Boolean for signalling info
                TBool signallingIndication( EFalse );
                TBool pcscfAddress( EFalse );
                TBool dnsAddress( EFalse );

                // Map the client side Pdp type to Gpds server's pdp type
                TUint8 pdpType( 0 );
                ret = MapPdpType( pdpType, configR99_R4.iPdpType );

                if ( KErrNone == ret )
                    {
                    TUint8 contextType( GPDS_CONT_TYPE_NORMAL );
                    // primary or secondary context
                    if( !configR99_R4.iNWIContext )
                        {
                        // MO initiated context
                        contextType = iContextList->
                            GetContextTypeByName( contextName );
                        }
                    else
                        {
                        // MT initiated context
                        contextType = GPDS_CONT_TYPE_NWI;
                        }

                    TInfoName hostCidName = iContextList->
                        GetHostCidName( contextName );
                    TUint8 primaryContextId( iContextList->
                        GetContextIdByContextName( &hostCidName ) );
                    TUint8 transActionId( iContextList->
                        ProxyIdByContextName( contextName ) );

                    // if Rel5 is used, then use the iMiscBuffer to check
                    // DNS Address are requested or not
                    if ( TPacketDataConfigBase::KConfigRel5 ==
                            config->ExtensionId() )
                        {
                        TInt isOnMiscBuffer( KErrNotFound );
                        RPacketContext::TPcoId pcoItem(
                            RPacketContext::EEtelPcktPCSCFAddressRequest );

                        TInt len( configR99_R4.iProtocolConfigOption.
                            iMiscBuffer.Length() );
                        TPtr8 ptrToTLVdata( configR99_R4.iProtocolConfigOption.
                            iMiscBuffer.LeftTPtr(
                                configR99_R4.iProtocolConfigOption.iMiscBuffer.
                                    MaxLength() ) );
                        ptrToTLVdata.SetLength( configR99_R4.
                            iProtocolConfigOption.iMiscBuffer.Length());

                        const TInt KBigEnoughBuf( 1 );
                        TBuf8<KBigEnoughBuf> smallBuffer;
                        smallBuffer.SetLength( KBigEnoughBuf );
                        smallBuffer.FillZ();

                        TTlvStruct<RPacketContext::TPcoId,
                            RPacketContext::TPcoItemDataLength>tlv(
                                ptrToTLVdata, KPtrToTLVdata );

                        TPtr8 zeroSizePtr(
                            const_cast<TUint8*>( smallBuffer.Ptr() ),
                            smallBuffer.Length(),
                            smallBuffer.MaxLength() );

                        isOnMiscBuffer=tlv.NextItemL( pcoItem, zeroSizePtr );

                        if( KErrNone == isOnMiscBuffer )
                            {
                            // if pcscfAddress info exist
                            pcscfAddress = ETrue;
                            isOnMiscBuffer = KErrNotFound;
                            tlv.ResetCursorPos();
                            }
                        // no else

                        pcoItem = RPacketContext::
                            EEtelPcktIMCNMSSubsystemSignallingFlag;
                        isOnMiscBuffer = tlv.NextItemL( pcoItem, zeroSizePtr );

                        if( KErrNone == isOnMiscBuffer )
                            {
                            // if PCSCF info indication exist.
                            signallingIndication = ETrue;
                            isOnMiscBuffer = KErrNotFound;
                            tlv.ResetCursorPos();
                            }
                        // no else

                        pcoItem = RPacketContext::
                            EEtelPcktDNSServerAddressRequest;
                        isOnMiscBuffer = tlv.NextItemL( pcoItem, zeroSizePtr );

                        if( KErrNone == isOnMiscBuffer )
                            {
                            // if Dns request flag found
                            dnsAddress = ETrue;
                            isOnMiscBuffer = KErrNotFound;
                            tlv.ResetCursorPos();
                            }
                        // no else
                        }
                    else
                        // Rel99 in use.Request DNS addresses all the time
                        {
                        dnsAddress = ETrue;
                        }

                    // Call GpdsContextConfigureReq
                    ret = GpdsContextConfigureReq( transActionId,
                        contextId, pdpType, contextType, primaryContextId,
                        GPDS_PDP_HCOMP_DEFAULT, GPDS_PDP_HCOMP_DEFAULT,
                        configR99_R4.iAccessPointName, NULL,NULL,
                        signallingIndication, pcscfAddress, dnsAddress );
                    }
                // no else
                }
            // no else
            }
        else
            {
            ret = KErrNotReady;
            }
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AddPacketFilter
// Add TFT packet filter for context.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AddPacketFilter(
    const CMmDataPackage* aDataPackage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddPacketFilter" );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_ADDPACKETFILTER, "CMmPacketContextMessHandler::AddPacketFilter" );

    // includes context configuration information
    TInt contextConfigurationType;
    RPacketContext::TContextConfigGPRS config;
    CArrayFixFlat<RPacketContext::TPacketFilterV2>* filterTemporaryArray = NULL;
    const CMmPacketDataPackage* data =
        reinterpret_cast<const CMmPacketDataPackage*>( aDataPackage );

    #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
    data->GetConfigAndFilter( &config, contextConfigurationType );
    #else
    data->GetConfig( &config, contextConfigurationType );
    #endif // NCP_COMMON_S60_VERSION_SUPPORT

    TInfoName* contextName;
    data->UnPackData( &contextName, &filterTemporaryArray );

    TInt ret( KErrGeneral );
    TUint8 pdpType( 0 );
    TUint8 contextId(iContextList->GetContextIdByContextName( contextName ) );

    if( GPDS_CID_VOID != contextId && NULL != filterTemporaryArray )
        {
        iContextList->SetContextConfigurationType(
            contextId, TPacketDataConfigBase::KConfigGPRS );
        //save authentication data
        ret = iContextList->SaveConfig(
            contextId, TPacketDataConfigBase::KConfigGPRS, &config, NULL );

        TInfoName hostCidName = iContextList->
            GetHostCidName( contextName );
        TUint8 primaryContextId( iContextList->
            GetContextIdByContextName( &hostCidName ) );
        TUint8 contextType( GPDS_CONT_TYPE_NORMAL );

        // Primary Context found
        if( GPDS_CID_VOID != primaryContextId )
            {
            contextType = GPDS_CONT_TYPE_SEC;
            }

        MapPdpType( pdpType, config.iPdpType );

        TUint8 dataCompression( GPDS_PDP_DCOMP_OFF );
        TUint8 headerCompression(GPDS_PDP_HCOMP_OFF );

        if ( TPacketDataConfigBase::KConfigRel99Rel4 !=
            contextConfigurationType )
            {
            if ( config.iPdpCompression &
                RPacketContext::KPdpDataCompression )
                {
                dataCompression = GPDS_PDP_DCOMP_ON;
                }
            // no else

            if ( config.iPdpCompression &
                RPacketContext::KPdpHeaderCompression )
                {
                headerCompression = GPDS_PDP_HCOMP_ON;
                }
            // no else
            }
        else
            {
            dataCompression  = GPDS_PDP_DCOMP_DEFAULT;
            headerCompression = GPDS_PDP_HCOMP_DEFAULT;
            }

        TUint8 traId( GenerateTraId( contextId ) );

        ret = GpdsContextConfigureReq(
            traId, contextId, pdpType, contextType,
            primaryContextId, dataCompression, headerCompression,
            config.iAccessPointName, filterTemporaryArray, NULL );
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::ActivatePhase2
// This function starts the second phase of the context activation.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::ActivatePhase2(
    const TUint8 aPipeHandle,
    const TUint8 aTransactionId )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::ActivatePhase2.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_ACTIVATEPHASE2, "CMmPacketContextMessHandler::ActivatePhase2" );

    TUint8 pdpType( 0 );
    TInt ret( KErrNone );
    TUint8 contextId( iContextList->
        ContextIdByPipeHandle( aPipeHandle ) );
    TInt configurationType( iContextList->
        GetContextConfigurationType( contextId ) );

    TPacketDataConfigBase* configBase = NULL;
    configBase = iContextList->
        GetConfig( contextId, configurationType );

    if( NULL!= configBase )
        {
        if( TPacketDataConfigBase::KConfigGPRS == configurationType )
            {
            RPacketContext::TContextConfigGPRS* config = NULL;
            config = reinterpret_cast<RPacketContext::TContextConfigGPRS*>(
                configBase );
            MapPdpType( pdpType, config->iPdpType );
            ret = GpdsContextActivateReq(
                aTransactionId, contextId, config->iPdpAddress, pdpType );
            }
        else if( TPacketDataConfigBase::KConfigRel99Rel4 == configurationType )
            {
            RPacketContext::TContextConfigR99_R4* config99 = NULL;
            config99 = reinterpret_cast<RPacketContext::TContextConfigR99_R4*>(
                configBase );
            MapPdpType( pdpType, config99->iPdpType );
            ret = GpdsContextActivateReq(
                aTransactionId, contextId, config99->iPdpAddress, pdpType );
            }
        // no else

        if ( KErrNone != ret )
            {

            TFLOGSTRING2("TSY: CMmPacketContextGsmWcdmaExt::ActivatePhase2. Error: %d", ret );

OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_ACTIVATEPHASE2, "CMmPacketContextMessHandler::ActivatePhase2;Error=%d", ret );
            TInfoName contextname;
            // Context name found
            if( KErrNone == iContextList->
                GetContextNameByContextId( contextId, contextname ) )
                {
                CMmDataPackage data;
                // due succesfull case has Config in package
                TAny* ptr = NULL;
                data.PackData( &contextname, ptr );
                iMessageRouter->Complete(
                    EPacketContextActivate,
                    &data,
                    ret );
                // Complete with KErrNone -> setLastErrorCause is NOT set then.
                iMessageRouter->Complete(
                    EPacketContextNotifyConfigChanged,
                    &data,
                    ret );
                TUint8 transId( GenerateTraId( contextId ) );

                // Reset the pipe
                ret = iMmPipeController->PnsPipeResetReq(
                    transId, aPipeHandle, PN_PIPE_DISABLE );

                if ( KErrNone == ret )
                    {
                    // Store the trans id
                    iContextList->SetPipeResetTraId( contextId, transId );
                    }
                else if( KErrNotFound == ret)
                    {

                    TFLOGSTRING("TSY: CMmPacketContextMessHandler::ActivatePhase2. SetPipeResetTraId failed");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_ACTIVATEPHASE2, "CMmPacketContextMessHandler::ActivatePhase2, SetPipeResetTraId failed" );
                    }
                // no else
                }
            // no else
            }
        // no else
        }
    // no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::MapPdpType
// This function maps the client pdp type to Gpds server pdp type.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::MapPdpType(
    TUint8& aPdpTypeServer,
    const RPacketContext::TProtocolType aPdpTypeClient )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::MapPdpType");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_MAPPDPTYPE, "CMmPacketContextMessHandler::MapPdpType" );

    TInt ret( KErrNone );

    switch ( aPdpTypeClient )
        {
        case RPacketContext::EPdpTypeIPv4:
            {
            aPdpTypeServer = GPDS_PDP_TYPE_IPV4;
            break;
            }
        case RPacketContext::EPdpTypeIPv6:
            {
            aPdpTypeServer = GPDS_PDP_TYPE_IPV6;
            break;
            }
        case RPacketContext::EPdpTypePPP:
        case RPacketContext::EPdpTypeX25:
        case RPacketContext::EPdpTypeCDPD:
            {
            ret = KErrNotSupported;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::MapPdpType. Switch aPdpTypeClient case - default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_MAPPDPTYPE, "CMmPacketContextMessHandler::MapPdpType,Switch aPdpTypeClient case - default" );

            ret = KErrArgument;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::MapConnectionSpeed
// Map the Gpds server connection speed to the client connection speed.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::MapConnectionSpeed(
    const TUint8 aConnectionSpeedServer,
    TInt& aConnectionSpeedClient )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::MapConnectionSpeed");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_MAPCONNECTIONSPEED, "CMmPacketContextMessHandler::MapConnectionSpeed" );

    switch ( aConnectionSpeedServer )
        {
        // Connection speed rate varies between 0 and 2097152 bps
        case GPDS_QOS_PETC_1:
            {
            aConnectionSpeedClient = KClientSpeeed8192;
            break;
            }
        case GPDS_QOS_PETC_2:
            {
            aConnectionSpeedClient = KClientSpeeed16384;
            break;
            }
        case GPDS_QOS_PETC_3:
            {
            aConnectionSpeedClient = KClientSpeeed32768;
            break;
            }
        case GPDS_QOS_PETC_4:
            {
            aConnectionSpeedClient = KClientSpeeed65536;
            break;
            }
        case GPDS_QOS_PETC_5:
            {
            aConnectionSpeedClient = KClientSpeeed131072;
            break;
            }
        case GPDS_QOS_PETC_6:
            {
            aConnectionSpeedClient = KClientSpeeed262144;
            break;
            }
        case GPDS_QOS_PETC_7:
            {
            aConnectionSpeedClient = KClientSpeeed524288;
            break;
            }
        case GPDS_QOS_PETC_8:
            {
            aConnectionSpeedClient = KClientSpeeed1048576;
            break;
            }
        case GPDS_QOS_PETC_9:
            {
            aConnectionSpeedClient = KClientSpeeed2097152;
            break;
            }
        // case GPDS_QOS_PETC_0:
        default:
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::MapConnectionSpeed. Switch aConnectionSpeedServer case - default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_MAPCONNECTIONSPEED, "CMmPacketContextMessHandler::MapConnectionSpeed, Switch aConnectionSpeedServer case - default" );

            aConnectionSpeedClient = KClientSpeeed0;
            break;
            }
        }

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::SetConnectionSpeed. Connection Speed: %d bps", aConnectionSpeedClient );

OstTrace1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_MAPCONNECTIONSPEED, "CMmPacketContextMessHandler::MapConnectionSpeed;Connection Speed=%d bps", aConnectionSpeedClient );
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::GenerateTraId
// Generate transaction id for use of current context.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMessHandler::GenerateTraId(
    const TUint8 aContextId )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::GenerateTraId");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_GENERATETRAID, "CMmPacketContextMessHandler::GenerateTraId" );

    TUint8 proxyId( iContextList->
        ProxyIdByContextId( aContextId ) );

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
    // due all contexts have same proxyId. DialUp context uses 0xFF
    // because no proxyId found
    TInt proxyInt( static_cast<TInt>( proxyId ) );
    TInt contextIdInt( static_cast<TInt>( aContextId ) );
    TInt tempInt( proxyInt - contextIdInt );
    proxyId = static_cast<TUint8>( tempInt );
#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

    return proxyId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::MapTftOperation
// Map the Client TFT operation to the Gpds server TFT operation.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::MapTftOperation(
    const TInt aClientTypeOperation,
    TUint8& aTftOperation )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::MapTftOperation");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_MAPTFTOPERATION, "CMmPacketContextMessHandler::MapTftOperation" );

    switch( aClientTypeOperation )
        {
        case ETftCreateNew:
            {
            aTftOperation = GPDS_TFT_CREATE_NEW;
            break;
            }
        case EDeleteExisting:
            {
            aTftOperation = GPDS_TFT_DELETE_EXISTING;
            break;
            }
        case EAddPacketFilters:
            {
            aTftOperation = GPDS_TFT_ADD_PACKET_FILTERS;
            break;
            }
        case EReplacePacketFilters:
            {
            aTftOperation = GPDS_TFT_REPLACE_PACKET_FILTERS;
            break;
            }
        case EDeletePacketFilters:
            {
            aTftOperation = GPDS_TFT_DELETE_PACKET_FILTERS;
            break;
            }
        case ETftNoOperation:
            {
            aTftOperation = GPDS_TFT_NO_OPERATION;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::MapTftOperation. Switch aClientTypeOperation case - default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_MAPTFTOPERATION, "CMmPacketContextMessHandler::MapTftOperation,Switch aClientTypeOperation case - default" );

            aTftOperation = GPDS_TFT_DELETE_EXISTING;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AddMediaAuthorizationL
// Create an Isi-message for requesting media authorization params.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AddMediaAuthorizationL(
    const CMmDataPackage* aDataPackage )
    {

    TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddMediaAuthorizationL.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_ADDMEDIAAUTHORIZATIONL, "CMmPacketContextMessHandler::AddMediaAuthorizationL" );

    const CMmPacketDataPackage* data =
        reinterpret_cast<const CMmPacketDataPackage*>( aDataPackage );

    TInt ret( KErrGeneral );

    //Initialise params
    TInt contextConfigurationType;
    TUint8 pdpType( 0 );
    RPacketContext::TContextConfigGPRS config;
    RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = NULL;
    TInfoName* contextName = NULL;

    #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
    data->GetConfigAndFilter( &config, contextConfigurationType );
    #else
    data->GetConfig( &config, contextConfigurationType );
    #endif // NCP_COMMON_S60_VERSION_SUPPORT

    //Unpack data
    data->UnPackData( &contextName, &mediaAuthorization );

    //Get necessary params
    TUint8 contextId( iContextList->
        GetContextIdByContextName( contextName ) );

    TInfoName hostCidName = iContextList->GetHostCidName( contextName );

    TUint8 primaryContextId( iContextList->
        GetContextIdByContextName( &hostCidName ) );

    TUint8 contextType( GPDS_CONT_TYPE_NORMAL );

    // Primary Context found
    if( GPDS_CID_VOID != primaryContextId )
        {
        contextType = GPDS_CONT_TYPE_SEC;
        }
    // no else

    MapPdpType( pdpType, config.iPdpType );
    TUint8 traId( GenerateTraId( contextId ) );

    ret = GpdsContextConfigureReq(
        traId, contextId, pdpType, contextType,
        primaryContextId, GPDS_PDP_HCOMP_DEFAULT,
        GPDS_PDP_HCOMP_DEFAULT, config.iAccessPointName,
        NULL, mediaAuthorization );

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AddMediaAuthorizationInfo
// Create subblocks to GpdsContextConfigureOrModifyReq.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AddMediaAuthorizationInfo(
    CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>*
        aMediaAuthorizationArray,
    TDes8& aBuffer,
    TUint8& aNumOfSbs ) const
    {
    // Initialise variables
    TInt count( aMediaAuthorizationArray->Count() );
    TInt ret( 0 == count && aMediaAuthorizationArray ?
        KErrNone : KErrGeneral );
    TInt arrayIndex( 0 );

    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::AddMediaAuthorizationInfo. aMediaAuthorizationArray count: %d", count);
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_ADDMEDIAAUTHORIZATIONINFO, "CMmPacketContextMessHandler::AddMediaAuthorizationInfo;aMediaAuthorizationArray count=%d", count );

    while ( KErrNone == ret && count > 0 )
        {
        ret = AppendMediaAuthorizationUnit(
                            aMediaAuthorizationArray->At( arrayIndex ),
                            aBuffer, aNumOfSbs );
        count--;
        arrayIndex++;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendMediaAuthorizationUnit
// Create subblocks for one message authorization unit.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendMediaAuthorizationUnit(
    const RPacketContext::CTFTMediaAuthorizationV3& aMediaAuthorization,
    TDes8& aBuffer,
    TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendMediaAuthorizationUnit");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDMEDIAAUTHORIZATIONUNIT, "CMmPacketContextMessHandler::AppendMediaAuthorizationUnit" );
    //Add GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO sub block
    TInt ret = AppendGpdsSharedTFTParameterAuthTokenInfo(
            aMediaAuthorization.iAuthorizationToken, aBuffer, aNumOfSbs );

    //GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO sub block
    if ( KErrNone == ret )
        {
        ret = AppendGpdsSharedTFTParameterIPFlowInfo(
                aMediaAuthorization.iFlowIds[0], aBuffer, aNumOfSbs );
        }

    // GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO (= 0xF1)
    // Make sub block for (2 and 3)
    if ( KErrNone == ret )
        {
TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddMediaAuthorizationUnit. Make sub block for (2 and 3)");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_APPENDMEDIAAUTHORIZATIONUNIT, "CMmPacketContextMessHandler::AppendMediaAuthorizationUnit, Make sub block for (2 and 3)" );
        ret = AppendGpdsSharedTFTParameterAuthTokenInfo(
                aMediaAuthorization.iAuthorizationToken, aBuffer, aNumOfSbs );
        }
    // GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO (2 and 3)
    // make sure that there is more than one class in Array
    TInt flowIdsArrayCount( aMediaAuthorization.iFlowIds.Count() );
    if ( KErrNone == ret && KGdpsSharedTftParamIPFlowInfo2 <= flowIdsArrayCount )
        {
TFLOGSTRING("TSY: CMmPacketContextMessHandler::AddMediaAuthorizationUnit. make sure that there is more than one class in Array");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_APPENDMEDIAAUTHORIZATIONUNIT, "CMmPacketContextMessHandler::AppendMediaAuthorizationUnit, Make sure that there is more than one class in Array" );
        for (TInt flowIdsArrayIndex = KFlowIdsArrayIndex1;
             flowIdsArrayIndex < flowIdsArrayCount && KErrNone == ret;
             flowIdsArrayIndex++)
            {
            ret = AppendGpdsSharedTFTParameterIPFlowInfo(
                    aMediaAuthorization.iFlowIds[flowIdsArrayIndex],
                    aBuffer, aNumOfSbs );
            }
        }
TFLOGSTRING2("TSY:CMmPacketContextMessHandler::AppendMediaAuthorizationUnit;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_APPENDMEDIAAUTHORIZATIONUNIT, "CMmPacketContextMessHandler::AppendMediaAuthorizationUnit;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AllowIncomingCallActivationL
// This methods checks if secondary contexts needs to be deactivated
// and allows incoming call (for allow incoming call feature).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::AllowIncomingCallActivationL()
    {
TFLOGSTRING("TSY: CMmPacketContextMessHandler::AllowIncomingCallActivationL");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_ALLOWINCOMINGCALLACTIVATIONL, "CMmPacketContextMessHandler::AllowIncomingCallActivationL" );

    if ( CALL_MODEM_ID_NONE != iCallModemResourceCallId &&
        NULL == iDeactivateContextList )
        {
        TUint8 contextId( GPDS_CID_VOID );
        TInt contextCount( iContextList->NumberOfContexts() );

        // initialize context list
        iDeactivateContextList = new (ELeave) CArrayFixFlat<TUint8>(
            KMmMaxNumberOfContexts );
        iDeactivateContextList->Reset();

        TFLOGSTRING("TSY: CMmPacketContextMessHandler::AllowIncomingCallActivationL, Find secondary context");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_ALLOWINCOMINGCALLACTIVATIONL, "CMmPacketContextMessHandler::AllowIncomingCallActivationL, Find secondary context" );
        // check if there is contexts created
        if ( 1 < contextCount )
            {
            // check if there is active secondary context
            for ( TInt i = 0; i < contextCount; i++ )
                {
                contextId = iContextList->ContextIdByListIndex( i );

                if ( ( GPDS_CID_VOID != contextId ) &&
                     ( GPDS_CONT_TYPE_SEC == iContextList->
                         GetContextTypeById( contextId ) ) &&
                     ( PN_PIPE_ENABLE == iContextList->
                         GetPipeStatus( contextId ) ))
                    {

                    TFLOGSTRING2("TSY: CMmPacketContextMessHandler::AllowIncomingCallActivationL, Secondary context found, List index: %d", i );
OstTrace1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_ALLOWINCOMINGCALLACTIVATIONL, "CMmPacketContextMessHandler::AllowIncomingCallActivationL;, Secondary context found, List index=%d", i );
                    // secondary context found
                    iDeactivateContextList->AppendL( contextId );
                    // break the loop
                    i = contextCount;
                    }
                //no else
                }
            }
            //no else

        // check if there is active secondary dial-up context
        TInfoName contextName( KStringExternal2 );
        contextId = iContextList->GetDialUpContextIdByName( &contextName );
        if ( GPDS_CID_VOID != contextId )
            {

            TFLOGSTRING("TSY: CMmPacketContextMessHandler::AllowIncomingCallActivationL, Secondary dial-up context found");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_ALLOWINCOMINGCALLACTIVATIONL, "CMmPacketContextMessHandler::AllowIncomingCallActivationL, Secondary dial-up context found" );
            // secondary context found
            iDeactivateContextList->AppendL( contextId );
            }
        //no else
        }
    //no else

    if ( iDeactivateContextList &&
        CALL_MODEM_ID_NONE != iCallModemResourceCallId )
        {
        // if secondary context found => request deactivation
        if ( 0 < iDeactivateContextList->Count() )
            {
            TUint8 transactionId(
                GenerateTraId( iDeactivateContextList->At( 0 ) ) );

            TFLOGSTRING2("TSY: CMmPacketContextMessHandler::AllowIncomingCallActivationL, Deactivating context, ContextId: %d", iDeactivateContextList->At( 0 ) );

OstTrace1( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_ALLOWINCOMINGCALLACTIVATIONL, "CMmPacketContextMessHandler::AllowIncomingCallActivationL;ContextId=%d", iDeactivateContextList->At( 0 ) );
            // Set the message data that consists of one parameter
            TBuf8<1> messageData;
            // deactivate first context from the list
            messageData.Append( iDeactivateContextList->At( 0 ) );

            // Send Isi message via Phonet
            TInt ret ( iPhoNetSender->Send(
                PN_GPDS, transactionId,
                GPDS_CONTEXT_DEACTIVATE_REQ, messageData ) );
            if ( KErrNone != ret )
                {
                // couldn't send message, can't allow call activation
                iCallModemResourceCallId = CALL_MODEM_ID_NONE;
                iDeactivateContextList->Reset();
                delete iDeactivateContextList;
                iDeactivateContextList = NULL;
                CallModemResourceReqDenied();
                }
            //no else
            }
        else // no secondary contexts found => allow incoming call
            {
            CallModemResourceReqAllowed();
            iCallModemResourceCallId = CALL_MODEM_ID_NONE;
            iDeactivateContextList->Reset();
            delete iDeactivateContextList;
            iDeactivateContextList = NULL;
            }
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo
// Adds GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO to the buffer and increases
// aNumOfSbs by 1
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo(
        const TDesC8& aAuthorizationToken,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSSHAREDTFTPARAMETERAUTHTOKENINFO, "CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo" );
    TInt ret( KErrTooBig );
    TInt authTokenLength( aAuthorizationToken.Length() );

    if ( authTokenLength < ( KMaximumLengthOfGpdsTftAuthTokenInfo -
            KAuthTokenInfoChecking ) )
        {
TFLOGSTRING("TSY: CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo. Add GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO sub block");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSSHAREDTFTPARAMETERAUTHTOKENINFO, "CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo, Add GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO sub block" );
        ret = KErrNone;
        TBuf<RPacketContext::KMaxFQDNLength> tempAuthorizationTokenInfo;
        tempAuthorizationTokenInfo.Copy( aAuthorizationToken );
        TBuf8<KMaximumLengthOfGpdsTftAuthTokenInfo>
             authorizationTokenInfo;
        TIsiSubBlock authTokenSubBlock(
            authorizationTokenInfo,
            GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO,
            EIsiSubBlockTypeId8Len8 );

        authorizationTokenInfo.Append(
            static_cast<TUint8>( authTokenLength ) );
        authorizationTokenInfo.Append( KGpdsPadding );
        authorizationTokenInfo.Append( tempAuthorizationTokenInfo );
        // add subblock
        aBuffer.Append( authTokenSubBlock.CompleteSubBlock() );
        // increase subblock count
        aNumOfSbs++;
        }
TFLOGSTRING2("TSY:CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSSHAREDTFTPARAMETERAUTHTOKENINFO, "CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterAuthTokenInfo;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterIPFlowInfo
// Adds GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO to the buffer and increases
// aNumOfSbs by 1
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterIPFlowInfo(
        const RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier&
        aFlowIdentifier,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterIPFlowInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSSHAREDTFTPARAMETERIPFLOWINFO, "CMmPacketContextMessHandler::AppendGpdsSharedTFTParameterIPFlowInfo" );
    TBuf8<SIZE_GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO> tempIPFlowInfo;
    TIsiSubBlock ipFlowInfoSubBlock( tempIPFlowInfo,
        GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO,
        EIsiSubBlockTypeId8Len8 );

    TUint16 mediaComponentNumber( aFlowIdentifier.iMediaComponentNumber );
    TUint16 ipFlowNumber( aFlowIdentifier.iIPFlowNumber );

    tempIPFlowInfo.Append( KGpdsPadding );
    tempIPFlowInfo.Append( KGpdsPadding );
    tempIPFlowInfo.Append( static_cast<TUint8>( mediaComponentNumber )
            << KShift8Bits );
    tempIPFlowInfo.Append( static_cast<TUint8>( mediaComponentNumber ) );
    tempIPFlowInfo.Append( static_cast<TUint8>( ipFlowNumber )
            << KShift8Bits );
    tempIPFlowInfo.Append( static_cast<TUint8>( ipFlowNumber ) );

    // add subblock
    TDes8& completed( ipFlowInfoSubBlock.CompleteSubBlock() );
    aBuffer.Append( ipFlowInfoSubBlock.CompleteSubBlock() );
    // increase subblock count
    aNumOfSbs++;

    return KErrNone; // we dont want to mask the panic since this is coding error
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::FillGpdsTFTInfo
// Adds GPDS_TFT_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::FillGpdsTFTInfo(TUint8 aOperation,
        RPacketContext::CTFTMediaAuthorizationV3* aMediaAuthorization,
        CArrayFixFlat<RPacketContext::TPacketFilterV2>* aPacketFilterPtr,
        TUint8 aPdpType,
        TDes8& aBuffer ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::FillGpdsTFTInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_FILLGPDSTFTINFO, "CMmPacketContextMessHandler::FillGpdsTFTInfo" );
    TInt ret( KErrNone );
    TUint8 numOfSbsInsideSb( 0 );

    // setting desired operation
    aBuffer.Append( aOperation );

    // Add subblocks to GPDS_TFT_INFO subblock
    // GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO
    // and GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO of GPDS_TFT_INFO
    if ( aMediaAuthorization )
        {
TFLOGSTRING("TSY: CMmPacketContextMessHandler::AppendGpdsTFTInfo. MediaAuthorization");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_FILLGPDSTFTINFO, "CMmPacketContextMessHandler::FillGpdsTFTInfo, MediaAuthorization" );
         ret = AppendMediaAuthorizationUnit( *aMediaAuthorization, aBuffer,
                                             numOfSbsInsideSb );
        }
    // end for sub blocks GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO
    // and GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO of GPDS_TFT_INFO

    // If there is one or more PacketFilters
    // GPDS_TFT_FILTER_INFO of GPDS_TFT_INFO
    if ( KErrNone == ret && aPacketFilterPtr )
        {
        // Initialize variable which indicates current filter
        TUint8 filterIndex( 0 );
        // Initialize variable which indicates the length of this subblock.
        TUint16 lengthOfGpdsTftInfoSubBlock( 0 );

        // Copy all given PacketFilters to GPDS_TFT_INFO
        // There can be from 1 to 8 PacketFilters
        while ( filterIndex < aPacketFilterPtr->Count() )
            {
            // Counting the total length of PacketFilters
            lengthOfGpdsTftInfoSubBlock =
                lengthOfGpdsTftInfoSubBlock +
                    AddPacketFilterInfo(
                        aPdpType,
                        &( aPacketFilterPtr->At( filterIndex ) ),
                        aBuffer );

           // If the maximum length of GPDS_TFT_INFO is exceeded, return value
           // is set to KErrTooBig
            if ( lengthOfGpdsTftInfoSubBlock > KMaximumLengthOfGpdsTftInfo )
                {
                ret = KErrTooBig;
                filterIndex = static_cast<TUint8>( aPacketFilterPtr->Count() );
                }
            // no else

            // increase subblock count
            numOfSbsInsideSb++;
            // Increasing filterIndex by one, pointing to
            // the next PacketFilter item
            filterIndex++;
            }
        }
        // no else

    // Complete GPDS_TFT_INFO subblock and set subblock count
    TBuf8<1> sbCountbuf;
    sbCountbuf.Append( numOfSbsInsideSb );
    aBuffer.Insert( GPDS_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS, sbCountbuf );
TFLOGSTRING2("TSY:CMmPacketContextMessHandler::FillGpdsTFTInfo;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_FILLGPDSTFTINFO, "CMmPacketContextMessHandler::FillGpdsTFTInfo;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::FillGpdsQos99ReqInfo
// Fills in GPDS_QOS99_REQ_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::FillGpdsQos99ReqInfo(
        const RPacketQoS::TQoSR99_R4Requested& aQos99Params,
        TDes8& aBuffer)
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::FillGpdsQos99ReqInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_FILLGPDSQOS99REQINFO, "CMmPacketContextMessHandler::FillGpdsQos99ReqInfo" );
    TInt ret( KErrNone );
    CMmPacketQoSMessHandler::TQoS99Info qoS99ReqInfoServer;

    // Map classified QoS R99 requested parameter values to server
    // side struct
    ret = iMmPacketQoSMessHandler->MapQoS99InfoToServer(
        qoS99ReqInfoServer,
        aQos99Params.iReqTrafficClass,
        aQos99Params.iReqDeliveryOrderReqd,
        aQos99Params.iReqDeliverErroneousSDU,
        aQos99Params.iReqBER,
        aQos99Params.iReqSDUErrorRatio,
        aQos99Params.iReqTrafficHandlingPriority );

    // Copy numerical QoS R99 requested parameter values to server
    // side struct
    if ( KErrNone == ret )
        {
        ret = iMmPacketQoSMessHandler->CopyQoS99ValuesToServer(
            qoS99ReqInfoServer,
            aQos99Params.iReqTransferDelay,
            aQos99Params.iReqMaxSDUSize,
            aQos99Params.iReqMaxRate,
            aQos99Params.iReqGuaranteedRate );
        }
    // no else

    if ( KErrNone == ret )
        {
        aBuffer.Append( qoS99ReqInfoServer.iTrafficClass );
        aBuffer.Append( qoS99ReqInfoServer.iDeliveryOrderReqd );
        aBuffer.Append( qoS99ReqInfoServer.iDeliverErroneousSDU );
        aBuffer.Append( qoS99ReqInfoServer.iBER );
        aBuffer.Append( qoS99ReqInfoServer.iSDUErrorRatio );
        aBuffer.Append( qoS99ReqInfoServer.iTrafficHandlingPriority );

        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iTransferDelay >> KShift8Bits ) );
        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iTransferDelay ) );

        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iMaxSDUSize >> KShift8Bits ) );
        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iMaxSDUSize ) );

        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iMaxUplinkRate >> KShift8Bits ) );
        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iMaxUplinkRate ) );

        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iMaxDownlinkRate >> KShift8Bits ) );
        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iMaxDownlinkRate ) );

        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iGuaranteedUplinkRate >> KShift8Bits ) );
        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iGuaranteedUplinkRate ) );

        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iGuaranteedDownlinkRate >> KShift8Bits ) );
        aBuffer.Append( static_cast<TUint8>(
            qoS99ReqInfoServer.iGuaranteedDownlinkRate ) );
        }
TFLOGSTRING2("TSY:CMmPacketContextMessHandler::FillGpdsQos99ReqInfo;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_FILLGPDSQOS99REQINFO, "CMmPacketContextMessHandler::FillGpdsQos99ReqInfo;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::FillGpdsQosReqInfo
// Adds GPDS_QOS_REQ_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::FillGpdsQosReqInfo(
        const RPacketQoS::TQoSGPRSRequested& aQos97Params,
        TDes8& aBuffer )
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::FillGpdsQosReqInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_FILLGPDSQOSREQINFO, "CMmPacketContextMessHandler::FillGpdsQosReqInfo" );
    TInt ret( KErrNone );

    // Map QoS R97/98 requested parameter values to server
    // side struct
    CMmPacketQoSMessHandler::TQoSInfo qosInfo;

    ret = iMmPacketQoSMessHandler->MapQoSInfoToServer(
        qosInfo,
        aQos97Params.iReqPrecedence,
        aQos97Params.iReqDelay,
        aQos97Params.iReqReliability,
        aQos97Params.iReqPeakThroughput,
        aQos97Params.iReqMeanThroughput );

    if ( KErrNone == ret )
        {
        aBuffer.Append( qosInfo.iPrecedence );
        aBuffer.Append( qosInfo.iDelay );
        aBuffer.Append( qosInfo.iReliability );
        aBuffer.Append( qosInfo.iPeakThroughput );
        aBuffer.Append( qosInfo.iMeanThroughput );
        aBuffer.Append( KGpdsPadding );
        }
TFLOGSTRING2("TSY:CMmPacketContextMessHandler::FillGpdsQosReqInfo;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_FILLGPDSQOSREQINFO, "CMmPacketContextMessHandler::FillGpdsQosReqInfo;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterFlowLabelInfo
// Adds GPDS_FILT_FLOW_LABEL_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterFlowLabelInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterFlowLabelInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERFLOWLABELINFO, "CMmPacketContextMessHandler::AppendGpdsFilterFlowLabelInfo" );
    // Create flowlabelInfo buffer for parameters
    TBuf8<SIZE_GPDS_FILT_FLOW_LABEL_INFO> flowLabelInfo;
    TIsiSubBlock flowLabelInfoSubBlock(
        flowLabelInfo,
        GPDS_FILT_FLOW_LABEL_INFO,
        EIsiSubBlockTypeId8Len8 );

    flowLabelInfo.Append( KGpdsPadding );
    flowLabelInfo.Append( KGpdsPadding );
    // Adding iFlowLabel value into buffer
    // conforming to the rules of endianess
    flowLabelInfo.Append( static_cast<TUint8>(
        aFilter.iFlowLabel >> KShift24Bits ) );
    flowLabelInfo.Append( static_cast<TUint8>(
        aFilter.iFlowLabel >> KShift16Bits ) );
    flowLabelInfo.Append( static_cast<TUint8>(
        aFilter.iFlowLabel >> KShift8Bits ) );
    flowLabelInfo.Append( static_cast<TUint8>(
        aFilter.iFlowLabel ) );

    // Add GPDS_LABEL_FILTER_INFO subblock
    aBuffer.Append( flowLabelInfoSubBlock.CompleteSubBlock() );
    aNumOfSbs++;

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterTosInfo
// Adds GPDS_FILT_TOS_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterTosInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterTosInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERTOSINFO, "CMmPacketContextMessHandler::AppendGpdsFilterTosInfo" );
    // Create tosInfo buffer for parameters
    TBuf8<SIZE_GPDS_FILT_TOS_INFO> tosInfo;
    TIsiSubBlock tosInfoSubBlock(
        tosInfo,
        GPDS_FILT_TOS_INFO,
        EIsiSubBlockTypeId8Len8 );

    // Adding iTOSorTrafficClass value into buffer
    tosInfo.Append( static_cast<TUint8>(
        aFilter.iTOSorTrafficClass >> KShift8Bits ) );
    tosInfo.Append( static_cast<TUint8>(
        aFilter.iTOSorTrafficClass & KTUint8NotDefined ) );

    // Add GPDS_TOS_FILTER_INFO subblock
    aBuffer.Append( tosInfoSubBlock.CompleteSubBlock() );
    // Increasing numberOfSubSubBlocks by one
    aNumOfSbs++;

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterSpiInfo
// Adds GPDS_FILT_SPI_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterSpiInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterSpiInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERSPIINFO, "CMmPacketContextMessHandler::AppendGpdsFilterSpiInfo" );
    // Create spiInfo buffer for parameters
    TBuf8<SIZE_GPDS_FILT_SPI_INFO> spiInfo;
    TIsiSubBlock spiInfoSubBlock(
        spiInfo,
        GPDS_FILT_SPI_INFO,
        EIsiSubBlockTypeId8Len8 );

    spiInfo.Append( KGpdsPadding );
    spiInfo.Append( KGpdsPadding );
    // Adding iIPSecSPI value into buffer
    // conforming to the rules of endianess
    spiInfo.Append( static_cast<TUint8>(
        aFilter.iIPSecSPI >> KShift24Bits ) );
    spiInfo.Append( static_cast<TUint8>(
        aFilter.iIPSecSPI >> KShift16Bits ) );
    spiInfo.Append( static_cast<TUint8>(
        aFilter.iIPSecSPI >> KShift8Bits ) );
    spiInfo.Append( static_cast<TUint8>(
        aFilter.iIPSecSPI ) );

    // Add GPDS_SPI_FILTER_INFO subblock
    aBuffer.Append( spiInfoSubBlock.CompleteSubBlock() );
    // Increasing numberOfSubSubBlocks by one
    aNumOfSbs++;

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterSpiInfo
// Adds GPDS_FILT_SRC_PORT_RANGE_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterSrcPortRangeInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterSrcPortRangeInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERSRCPORTRANGEINFO, "CMmPacketContextMessHandler::AppendGpdsFilterSrcPortRangeInfo" );
    // Create srcPortRangeInfo buffer for parameters
    TBuf8<SIZE_GPDS_FILT_SRC_PORT_RANGE_INFO> srcPortRangeInfo;
    TIsiSubBlock srcPortRangeSubBlock(
        srcPortRangeInfo,
        GPDS_FILT_SRC_PORT_RANGE_INFO,
        EIsiSubBlockTypeId8Len8 );

    // Adding iSrcPortMin value into buffer
    // conforming to the rules of endianess
    srcPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iSrcPortMin >> KShift8Bits ) );
    srcPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iSrcPortMin ) );

    // Adding iSrcPortMax value into buffer
    // conforming to the rules of endianess
    srcPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iSrcPortMax >> KShift8Bits ) );
    srcPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iSrcPortMax ) );

    srcPortRangeInfo.Append( KGpdsPadding );
    srcPortRangeInfo.Append( KGpdsPadding );

    // Add GPDS_SRC_PORT_FILTER_INFO subblock
    aBuffer.Append( srcPortRangeSubBlock.CompleteSubBlock() );
    // Increasing numberOfSubSubBlocks by one
    aNumOfSbs++;

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterSpiInfo
// Adds GPDS_FILT_DST_PORT_RANGE_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterDstPortRangeInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer, TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterDstPortRangeInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERDSTPORTRANGEINFO, "CMmPacketContextMessHandler::AppendGpdsFilterDstPortRangeInfo" );
    // Create destPortRangeInfo buffer for parameters
    TBuf8<SIZE_GPDS_FILT_DST_PORT_RANGE_INFO> destPortRangeInfo;
    TIsiSubBlock destPortRangeSubBlock(
        destPortRangeInfo,
        GPDS_FILT_DST_PORT_RANGE_INFO,
        EIsiSubBlockTypeId8Len8 );

    // Adding iDestPortMin value into buffer
    // conforming to the rules of endianess
    destPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iDestPortMin >> KShift8Bits ) );
    destPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iDestPortMin ) );

    // Adding iDestPortMax value into buffer
    // conforming to the rules of endianess
    destPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iDestPortMax >> KShift8Bits ) );
    destPortRangeInfo.Append( static_cast<TUint8>(
        aFilter.iDestPortMax ) );

    destPortRangeInfo.Append( KGpdsPadding );
    destPortRangeInfo.Append( KGpdsPadding );

    // Add GPDS_DEST_PORT_RANGE_FILTER_INFO subblock
    aBuffer.Append( destPortRangeSubBlock.CompleteSubBlock() );
    // Increasing numberOfSubSubBlocks by one
    aNumOfSbs++;

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterSpiInfo
// Adds GPDS_FILT_PROTOCOL_INFO to the buffer
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterProtocolInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterProtocolInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERPROTOCOLINFO, "CMmPacketContextMessHandler::AppendGpdsFilterProtocolInfo" );
    // Create protocolInfo buffer for parameter, length 1 byte
    TBuf8<SIZE_GPDS_FILT_PROTOCOL_INFO> protocolInfo;
    TIsiSubBlock protocolInfoSubBlock(
        protocolInfo,
        GPDS_FILT_PROTOCOL_INFO,
        EIsiSubBlockTypeId8Len8 );

    TUint8 protocolNumberOrNextHeader( static_cast<TUint8>(
        aFilter.iProtocolNumberOrNextHeader ) );
    protocolInfo.Append( protocolNumberOrNextHeader );
    protocolInfo.Append( KGpdsPadding );

    // Add GPDS_FILT_PROTOCOL_INFO subblock
    aBuffer.Append( protocolInfoSubBlock.CompleteSubBlock() );
    // Increasing numberOfSubSubBlocks by one
    aNumOfSbs++;

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv4AddressInfo
// Appends GPDS_FILT_SRC_IPV4_ADDR_INFO subblock to the passed buffer.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv4AddressInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv4AddressInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERSRCIPV4ADDRESSINFO, "CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv4AddressInfo" );
    // Create ipv4AddrInfo buffer for parameters
    TBuf8<SIZE_GPDS_FILT_SRC_IPV4_ADDR_INFO> ipv4AddrInfo;
    TIsiSubBlock ipv4AddrInfoSubBlock(
        ipv4AddrInfo,
        GPDS_FILT_SRC_IPV4_ADDR_INFO,
        EIsiSubBlockTypeId8Len8 );

    if ( KErrNone == AppendIpAddressInfo(
            TPtrC8( aFilter.iSrcAddr, KIpv4AddressLen ),
            TPtrC8( aFilter.iSrcAddrSubnetMask, KIpv4AddressLen ),
            ipv4AddrInfo ) )
        {
        // Add GPDS_FILT_SRC_IPV4_ADDR_INFO subblock
        aBuffer.Append( ipv4AddrInfoSubBlock.CompleteSubBlock() );
        // Increasing numberOfSubSubBlocks by one
        aNumOfSbs++;
        }

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv6AddressInfo
// Appends GPDS_FILT_SRC_IPV4_ADDR_INFO subblock to the passed buffer.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv6AddressInfo(
        const RPacketContext::TPacketFilterV2& aFilter,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv6AddressInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSFILTERSRCIPV6ADDRESSINFO, "CMmPacketContextMessHandler::AppendGpdsFilterSrcIPv6AddressInfo" );
    // Create ipv6AddrInfo buffer for parameters
    TBuf8<SIZE_GPDS_FILT_SRC_IPV6_ADDR_INFO> ipv6AddrInfo;
    TIsiSubBlock ipv6AddrInfoSubBlock(
            ipv6AddrInfo,
        GPDS_FILT_SRC_IPV6_ADDR_INFO,
        EIsiSubBlockTypeId8Len8 );

    if ( KErrNone == AppendIpAddressInfo(
            TPtrC8( aFilter.iSrcAddr, KIpv6AddressLen ),
            TPtrC8( aFilter.iSrcAddrSubnetMask, KIpv6AddressLen ),
            ipv6AddrInfo ) )
        {
        // Add GPDS_FILT_SRC_IPV4_ADDR_INFO subblock
        aBuffer.Append( ipv6AddrInfoSubBlock.CompleteSubBlock() );
        // Increasing numberOfSubSubBlocks by one
        aNumOfSbs++;
        }

    return KErrNone; // dont mask the panic on overflow to avoid the change in
                     // the functionality
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendIpAddressInfo
// Appends GPDS IP source address information to the buffer.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendIpAddressInfo(
        const TDesC8& aInputIpAddress,
        const TDesC8& aInputIpAddressMask,
        TDes8& aBuffer ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendIpAddressInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDIPADDRESSINFO, "CMmPacketContextMessHandler::AppendIpAddressInfo" );
    TInt ret( KErrArgument );

    // check that address 0.0.0.0 is not used.
    TUint addressCounter( 0 );
    for ( TInt i = 0; i < aInputIpAddress.Length(); i++ )
        {
        addressCounter += aInputIpAddress[i];
        }

    if (0 != addressCounter)
        {
        ret = KErrNone;

        TInt ipAddressMaskLength( aInputIpAddressMask.Length() );
        // Count the number of '1' bits in Src Addr Subnet Mask
        TInt amountOfBitsMasked( 0 );
        for ( TUint i = 0; ( ( i < ipAddressMaskLength ) &&
            ( 0 < aInputIpAddressMask[i] ) ); i++ )
            {
            // If number between dots is 255 = 0xFF
            if ( KTUint8NotDefined == aInputIpAddressMask[i] )
                {
                // All are '1' bits and so the count is
                // increased by eight
                amountOfBitsMasked += KShift8Bits;
                }
            // Otherwise there is less than eight '1' bits
            else
                {
                // Initializing mask to 0x80 = 1000 0000 b
                TUint8 mask( KInitMask0x80 );
                for ( TUint k = 0; k < KShift8Bits; k++ )
                    {
                    // Count the number of '1' bits from the byte
                    if ( aInputIpAddressMask[i] & mask )
                        {
                        amountOfBitsMasked++;
                        mask = static_cast<TUint8>( mask >> KShift1Bit );
                        }
                    else
                        {
                        k = KShift8Bits;
                        }
                    }
                // Rest of the Src Addr Subnet Mask is 0's,
                // so we break the loop
                i = ipAddressMaskLength;
                }
            }

        aBuffer.Append( amountOfBitsMasked );
        aBuffer.Append( KGpdsPadding );
        aBuffer.Append( aInputIpAddress );
        }
TFLOGSTRING2("TSY:CMmPacketContextMessHandler::AppendIpAddressInfo;ret=%d", ret);
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_APPENDIPADDRESSINFO, "CMmPacketContextMessHandler::AppendIpAddressInfo;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::AppendGpdsTftFilterInfo
// Appends GPDS_TFT_FILTER_INFO subblock.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMessHandler::AppendGpdsTftFilterInfo(
        TUint8 aFilterId,
        TDes8& aBuffer,
        TUint8& aNumOfSbs ) const
    {
TFLOGSTRING("TSY:CMmPacketContextMessHandler::AppendGpdsTftFilterInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_APPENDGPDSTFTFILTERINFO, "CMmPacketContextMessHandler::AppendGpdsTftFilterInfo" );
    // Add GPDS_TFT_FILTER_INFO subblock
    TBuf8<SIZE_GPDS_TFT_FILTER_INFO> filterInfoSbBuffer;
    TIsiSubBlock gpdsFilterInfoSubBlock(
        filterInfoSbBuffer,
        GPDS_TFT_FILTER_INFO,
        EIsiSubBlockTypeId8Len8 );

    // In API docs ID can have values from 1 to 8 and
    // in GPDS docs it can have values from 0 to 7
    TUint8 filterArray( static_cast<TUint8>(
            aFilterId - KFilterArray ) );
    filterInfoSbBuffer.Append( filterArray );
    filterInfoSbBuffer.Append( KGpdsPadding );
    // add fillers
    filterInfoSbBuffer.Append( KGpdsPadding );
    filterInfoSbBuffer.Append( KGpdsPadding );
    filterInfoSbBuffer.Append( KGpdsPadding );
    aBuffer.Append(
        gpdsFilterInfoSubBlock.CompleteSubBlock() );
    aNumOfSbs++;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::CallModemResourceIndL
// This method breaks call modem resource indication message and
// initiates secondary contexts deactivation to allow incoming call
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::CallModemResourceIndL(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING( "TSY: CMmPacketContextMessHandler::CallModemResourceIndL" );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEINDL, "CMmPacketContextMessHandler::CallModemResourceIndL" );

    TUint sbStartOffset( 0 );
    //Default, 0 is unused value
    TUint resourceId( 0 );
    // Default, emergency call mode not possible in MT call
    TUint8 callMode( CALL_MODEM_MODE_EMERGENCY );
    // Make a copy of received message. Allocate heap memory.
    // Can leave if out of mem.
    iResourceControlMsg = HBufC8::NewL( aIsiMessage.GetBuffer().Length() );
    // Copy data.
    *iResourceControlMsg = aIsiMessage.GetBuffer();

TFLOGSTRING("TSY: CMmPacketContextMessHandler::CallModemResourceIndL, messagedata saved");
OstTrace0( TRACE_NORMAL, DUP9_CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEINDL, "CMmPacketContextMessHandler::CallModemResourceIndL, messagedata saved" );

    TUint8 callId = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RESOURCE_IND_OFFSET_CALLID );

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND,
        CALL_MODEM_SB_RESOURCE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        resourceId = aIsiMessage.Get16bit(
            sbStartOffset + CALL_MODEM_SB_RESOURCE_OFFSET_RES );
        }

    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND,
        CALL_MODEM_SB_MODE,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        callMode = aIsiMessage.Get8bit(
            sbStartOffset + CALL_MODEM_SB_MODE_OFFSET_MODE );
        }
    // check request
    if ( ( CALL_MODEM_RES_ID_MT_INIT == resourceId ) &&
         ( CALL_MODEM_ID_NONE != callId ) &&
         ( CALL_MODEM_MODE_SPEECH == callMode ||
           CALL_MODEM_MODE_ALS_LINE_2 == callMode ||
           CALL_MODEM_MODE_MULTIMEDIA == callMode ) )
        {
        // if DCM drive mode flag is enabled
        if ( iDriveModeFlag )
            {
TFLOGSTRING( "TSY: CMmPacketContextMessHandler::CallModemResourceIndL - DCM Drive Mode enabled" );
OstTrace0( TRACE_NORMAL, DUP8_CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEINDL, "CMmPacketContextMessHandler::CallModemResourceIndL - DCM Drive Mode enabled" );

            // Complete DCM specific functionality
            // Drive mode rejects automatically incoming calls
            CompleteDCMdrivemodeFunctionalityL();
            CallModemResourceReqDenied();
            }
        else
            {
TFLOGSTRING( "TSY: CMmPacketContextMessHandler::CallModemResourceIndL - Allow incoming call activation" );
OstTrace0( TRACE_NORMAL, DUP6_CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEINDL, "CMmPacketContextMessHandler::CallModemResourceIndL - Allow incoming call activation" );

            // allow incoming call activation
            iCallModemResourceCallId = callId;
            AllowIncomingCallActivationL();
            }
        }
    else
        {
        iCallModemResourceCallId = CALL_MODEM_ID_NONE;
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::CallModemResourceReqAllowed
// CALL_MODEM_RESOURCE_REQ is used to configure resource control
// Client shall use CALL_MODEM_RESOURCE_REQ to allow or deny resource and
// modify parameters for the resource.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::CallModemResourceReqAllowed()
    {
TFLOGSTRING( "TSY: CMmPacketContextMessHandler::CallModemResourceReqAllowed" );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEREQALLOWED, "CMmPacketContextMessHandler::CallModemResourceReqAllowed" );

    TIsiSend callModemResourceReq( iPhoNetSender->SendBufferDes() );

    // Copy message from buffer.
    const TDesC8& message( *iResourceControlMsg );

    TIsiReceiveC isimessage( ( TIsiReceiveC ) message );

    callModemResourceReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_CALL );

    // Set the CallModemResourceReq data
    callModemResourceReq.Set8bit(
        ISI_HEADER_OFFSET_MESSAGEID, CALL_MODEM_RESOURCE_REQ );

    TUint callId = isimessage.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_RESOURCE_IND_OFFSET_CALLID );

    // Set call id
    callModemResourceReq.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RESOURCE_REQ_OFFSET_CALLID, callId );

    // get subblocks from message
    TUint8 numOfSubBlocks = isimessage.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_RESOURCE_IND_OFFSET_SUBBLOCKS );

    TUint sbStartOffSet( 0 );

    // buffer for CALL_MODEM_SB_RESOURCE_STATUS subblock
    TBuf8<SIZE_CALL_MODEM_SB_RESOURCE_STATUS> resourceStatus;

    // Add CALL_MODEM_SB_RESOURCE_STATUS [M] subblock
    TIsiSubBlock resourceStatusSb(
        resourceStatus,
        CALL_MODEM_SB_RESOURCE_STATUS,
        EIsiSubBlockTypeId8Len8 );
    // resource status + filler byte
    resourceStatus.Append( CALL_MODEM_RESOURCE_ALLOWED );
    resourceStatus.Append( KCallPadding );

    TInt currentMsgOffset(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_REQ );

    callModemResourceReq.CopyData(
        currentMsgOffset, resourceStatusSb.CompleteSubBlock() );

    // Set new offset and increase subblock count
    currentMsgOffset =
        currentMsgOffset + resourceStatus.Length();

    // loop for finding all subblocks from CALL_MODEM_RESOURCE_IND
    for ( TInt sbIndex = 1; sbIndex <= numOfSubBlocks; sbIndex++ )
        {
        if ( KErrNone == isimessage.FindSubBlockOffsetByIndex(
            ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_IND,
            sbIndex,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // get length of subblock
            TUint8 sbLen = isimessage.Get8bit(
                sbStartOffSet + KSubBlockLengthOffset1 );

            callModemResourceReq.CopyData(
                currentMsgOffset,
                isimessage.GetData( sbStartOffSet, sbLen ) );
            // Check if handled sub block is CALL_MODEM_SB_MODE
            if ( CALL_MODEM_SB_MODE == isimessage.Get8bit( sbStartOffSet ) )
                {
                // Clients shall set the value to '0' when sending requests
                // to Modem Call Server.
                callModemResourceReq.Set8bit(
                    currentMsgOffset + CALL_MODEM_SB_MODE_OFFSET_MODEINFO,
                    0x00 );
                }
            // Set new offset and increase subblock count
            currentMsgOffset =
                currentMsgOffset + sbLen;
            }
        }

    callModemResourceReq.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RESOURCE_REQ_OFFSET_SUBBLOCKS,
        numOfSubBlocks + 1 );
    // Message sent via Phonet
    iPhoNetSender->Send( callModemResourceReq.Complete() );

    // Delete used message.
    delete iResourceControlMsg;
    iResourceControlMsg = NULL;

    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::CallModemResourceResp
// CALL_MODEM_RESOURCE_RESP is used to configure resource control
// When Call Server has accepted the request CALL_MODEM_RESOURCE_RESP is
// sent back to the requester.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::CallModemResourceResp(
    const TIsiReceiveC &aIsiMessage )
    {
TFLOGSTRING( "TSY: CMmPacketContextMessHandler::CallModemResourceResp" );
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCERESP, "CMmPacketContextMessHandler::CallModemResourceResp" );

    TUint8 callId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RESOURCE_RESP_OFFSET_CALLID ) );

TFLOGSTRING2("TSY: CMmPacketContextMessHandler::CallModemResourceResp, CallId %d", callId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCERESP, "CMmPacketContextMessHandler::CallModemResourceResp - CallId=%hhu", callId );

    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::CallModemResourceReqDenied
// Creates callModemResourceDenied ISI message to modem call server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::CallModemResourceReqDenied()
   {
TFLOGSTRING("TSY: CMmPacketContextMessHandler::CallModemResourceReqDenied");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEREQDENIED, "CMmPacketContextMessHandler::CallModemResourceReqDenied" );

    TIsiSend callModemResourceDenied( iPhoNetSender->SendBufferDes() );

    // Copy message from buffer.
    const TDesC8& message( *iResourceControlMsg );

    TIsiReceiveC isimessage( ( TIsiReceiveC ) message );

    callModemResourceDenied.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_CALL );

    // Set the callModemResourceDenied data
    callModemResourceDenied.Set8bit(
        ISI_HEADER_OFFSET_MESSAGEID, CALL_MODEM_RESOURCE_REQ );

    TUint callId = isimessage.Get8bit( ISI_HEADER_SIZE +
        CALL_MODEM_RESOURCE_IND_OFFSET_CALLID );

    // Set call id
    callModemResourceDenied.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RESOURCE_REQ_OFFSET_CALLID, callId );

    TInt aCurrentMsgOffset(
        ISI_HEADER_SIZE + SIZE_CALL_MODEM_RESOURCE_REQ );

    TUint8 numOfSbInMessage( 0 );

    // buffer for CALL_MODEM_SB_RESOURCE_STATUS subblock
    TBuf8<CALL_MODEM_SB_RESOURCE_STATUS> resourceStatus;

    // Add CALL_MODEM_SB_RESOURCE_STATUS subblock
    TIsiSubBlock resourceStatusSb(
        resourceStatus,
        CALL_MODEM_SB_RESOURCE_STATUS,
        EIsiSubBlockTypeId8Len8 );
    // resource status + filler byte
    resourceStatus.Append( CALL_MODEM_RESOURCE_DENIED );
    resourceStatus.Append( KCallPadding );

    callModemResourceDenied.CopyData(
        aCurrentMsgOffset, resourceStatusSb.CompleteSubBlock() );
    // Set new offset and increase subblock count
    aCurrentMsgOffset =
        aCurrentMsgOffset + resourceStatus.Length();
    // increase subblock count
    numOfSbInMessage++;

TFLOGSTRING2("TSY: CMmPacketContextMessHandler::CallModemResourceReqDenied - CALL_MODEM_SB_RESOURCE_STATUS - Subblock Count:  %d", numOfSbInMessage );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEREQDENIED, "CMmPacketContextMessHandler::CallModemResourceReqDenied- CALL_MODEM_SB_RESOURCE_STATUS - Subblock Count=%hhu", numOfSbInMessage );

    // buffer for CALL_MODEM_SB_CAUSE subblock
    TBuf8<CALL_MODEM_SB_CAUSE> modemSbCause;

    // Add CALL_MODEM_SB_CAUSE subblock
    TIsiSubBlock modemSbCauseSb(
        modemSbCause,
        CALL_MODEM_SB_CAUSE,
        EIsiSubBlockTypeId8Len8 );
    // cause type + cause
    modemSbCause.Append( CALL_MODEM_CAUSE_TYPE_CLIENT );
    modemSbCause.Append( CALL_MODEM_CAUSE_RELEASE_BY_USER );

    callModemResourceDenied.CopyData(
        aCurrentMsgOffset, modemSbCauseSb.CompleteSubBlock() );
    // Set new offset and increase subblock count
    aCurrentMsgOffset =
        aCurrentMsgOffset + modemSbCause.Length();
    // increase subblock count
    numOfSbInMessage++;

TFLOGSTRING2("TSY: CMmPacketContextMessHandler::CallModemResourceReqDenied - CALL_MODEM_SB_CAUSE - Subblock Count:  %d", numOfSbInMessage );
OstTraceExt1( TRACE_NORMAL, DUP4_CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEREQDENIED, "CMmPacketContextMessHandler::CallModemResourceReqDenied- CALL_MODEM_SB_CAUSE - Subblock Count=%hhu", numOfSbInMessage );

    // buffer for CALL_MODEM_SB_DETAILED_CAUSE subblock
    TBuf8<CALL_MODEM_SB_DETAILED_CAUSE> detailedCause;

    // Add CALL_MODEM_SB_DETAILED_CAUSE subblock
    TIsiSubBlock detailedCauseSb(
        detailedCause,
        CALL_MODEM_SB_DETAILED_CAUSE,
        EIsiSubBlockTypeId8Len8 );
    // cause length + cause values ( 4 ) + filler
    detailedCause.Append( KDiagnosticsOctet2 );
    detailedCause.Append( KDiagnosticsOctet3 );
    detailedCause.Append( KDiagnosticsOctet4 );
    detailedCause.Append( KDiagnosticsOctet5 );
    detailedCause.Append( KDiagnosticsOctet6 );
    detailedCause.Append( KCallPadding );

    callModemResourceDenied.CopyData(
        aCurrentMsgOffset, detailedCauseSb.CompleteSubBlock() );
    // Set new offset and increase subblock count
    aCurrentMsgOffset =
        aCurrentMsgOffset + detailedCause.Length();
    // increase subblock count
    numOfSbInMessage++;

TFLOGSTRING2("TSY: CMmPacketContextMessHandler::CallModemResourceReqDenied - CALL_MODEM_SB_DETAILED_CAUSE - Subblock Count:  %d", numOfSbInMessage );
OstTraceExt1( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLER_CALLMODEMRESOURCEREQDENIED, "CMmPacketContextMessHandler::CallModemResourceReqDenied- CALL_MODEM_SB_DETAILED_CAUSE - Subblock Count=%hhu", numOfSbInMessage );

    // Set number of subblokcs in CALL_MODEM_RESOURCE_REQ message
    callModemResourceDenied.Set8bit(
        ISI_HEADER_SIZE + CALL_MODEM_RESOURCE_REQ_OFFSET_SUBBLOCKS,
        numOfSbInMessage );

    // Message sent via Phonet
    iPhoNetSender->Send( callModemResourceDenied.Complete() );

    // Delete iResourceControlMsg
    delete iResourceControlMsg;
    iResourceControlMsg = NULL;

    }

// -----------------------------------------------------------------------------
// CMmPacketContextMessHandler::CompleteDCMdrivemodeFunctionalityL
// This methods completes DCM drive mode specific functionality, if drive mode
// flag is enabled.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmPacketContextMessHandler::CompleteDCMdrivemodeFunctionalityL()
    {
TFLOGSTRING("TSY: CMmPacketContextMessHandler::CompleteDCMdrivemodeFunctionalityL");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLER_COMPLETEDCMDRIVEMODEFUNCTIONALITYL, "CMmPacketContextMessHandler::CompleteDCMdrivemodeFunctionalityL" );

    TInt result( KErrNone );

    // Create call package (used for all completions)
    CCallDataPackage callData;

    // This is for call info required for incoming call
    RMobileCall::TMobileCallInfoV1 iIncomingCallId;

    // Call is first notified like normal incoming call,
    // then it is notified as disconnecting and idle
    iIncomingCallId.iStatus = RMobileCall::EStatusRinging;

    // COMPLETE INCOMING CALL
    // Pack mobile call info
    callData.PackData( &iIncomingCallId );
    // Complete IncomingCall notification
    iMessageRouter->Complete(
        EEtelLineNotifyIncomingCall,
        &callData,
        KErrNone );

    // COMPLETE MOBILE CALL INFO CHANGE
    // (Get mobile call information is always completed before
    // call status change)
    // pack mobile call info
    // pass call information to the Symbian OS layer
    callData.PackData( &iIncomingCallId );
    iMessageRouter->Complete(
        EMobileCallGetMobileCallInfo,
        &callData,
        KErrNone );

    // COMPLETE MOBILE CALL STATUS CHANGE
    // Pack call status
    callData.PackData( &iIncomingCallId.iStatus );
    // Complete status change indication
    iMessageRouter->Complete(
        EMobileCallNotifyMobileCallStatusChange,
        &callData,
        KErrNone );

    // status disconnecting
    iIncomingCallId.iStatus = RMobileCall::EStatusDisconnecting;

    // COMPLETE MOBILE CALL INFO CHANGE
    // (Get mobile call information is always completed before
    // call status change)
    // pack mobile call info
    // pass call information to the Symbian OS layer
    callData.PackData( &iIncomingCallId );
    iMessageRouter->Complete(
        EMobileCallGetMobileCallInfo,
        &callData,
        KErrNone );

    // COMPLETE MOBILE CALL STATUS CHANGE
    // Pack call status
    callData.PackData( &iIncomingCallId.iStatus );
    // Complete status change indication
    iMessageRouter->Complete(
        EMobileCallNotifyMobileCallStatusChange,
        &callData,
        result );

    // status idle
    iIncomingCallId.iStatus = RMobileCall::EStatusIdle;

    // COMPLETE MOBILE CALL INFO CHANGE
    // (Get mobile call information is always completed before
    // call status change)
    // Pack mobile call info
    // Pass call information to the Symbian OS layer
    callData.PackData( &iIncomingCallId );
    iMessageRouter->Complete(
        EMobileCallGetMobileCallInfo,
        &callData,
        KErrNone );

    // COMPLETE MOBILE CALL STATUS CHANGE
    // Pack call status
    callData.PackData( &iIncomingCallId.iStatus );
    // Complete status change indication
    iMessageRouter->Complete(
        EMobileCallNotifyMobileCallStatusChange,
        &callData,
        result );

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

    // None

// End of File
