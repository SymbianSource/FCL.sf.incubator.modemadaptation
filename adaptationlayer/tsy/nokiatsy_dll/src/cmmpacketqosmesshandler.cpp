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
#include "cmmpacketqosmesshandler.h"
#include "cmmpacketservicemesshandler.h"
#include "cmmpacketmesshandlercontextlist.h"
#include "cmmmessagerouter.h"
#include <gpdsisi.h>
#include <tisi.h>
#include <pn_const.h>
#include "cmmphonetsender.h"
#include <pcktcs.h>
#include "tsylogger.h"

#include "nokiatsy_internal_variation.h"

#include <gpds_sharedisi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpacketqosmesshandlertraces.h"
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
// CMmPacketQoSMessHandler::CMmPacketQoSMessHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMmPacketQoSMessHandler::CMmPacketQoSMessHandler()
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::C++ constructor");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_CMMPACKETQOSMESSHANDLER, "CMmPacketQoSMessHandler::CMmPacketQoSMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::~CMmPacketQoSMessHandler
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmPacketQoSMessHandler::~CMmPacketQoSMessHandler()
    {

    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::~CMmPacketQoSMessHandler." );
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_CMMPACKETQOSMESSHANDLER, "CMmPacketQoSMessHandler::~CMmPacketQoSMessHandler" );

    // if for some reasons, pointers haven't been deleted in SetProfileParamtersL
    // do it here
    if( iQos.iQoSMinInfo )
        {
        delete iQos.iQoSMinInfo;
        }
    //no else

    if( iQos.iQoS99MinInfo )
        {
        delete iQos.iQoS99MinInfo;
        }
    //no else

   if( iQos.iQoS99ReqInfo )
        {
        delete iQos.iQoS99ReqInfo;
        }
    //no else

    if( iQos.iQoSReqInfo )
        {
        delete  iQos.iQoSReqInfo;
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmPacketQoSMessHandler* CMmPacketQoSMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender,
    CMmPhoNetReceiver* aPhoNetReceiver,
    CMmPacketContextMesshandlerList* aMmPacketContextMesshandlerList,
    CMmMessageRouter* aMessageRouter )
    {

    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::NewL.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_NEWL, "CMmPacketQoSMessHandler::NewL" );

    CMmPacketQoSMessHandler* packetQoSMessHandler =
        new ( ELeave ) CMmPacketQoSMessHandler();

    CleanupStack::PushL( packetQoSMessHandler );

    packetQoSMessHandler->iPhoNetSender =  aPhoNetSender;
    packetQoSMessHandler->iMmPacketContextMesshandlerList =
        aMmPacketContextMesshandlerList;
    packetQoSMessHandler->iMessageRouter = aMessageRouter;

    packetQoSMessHandler->ConstructL();

    aPhoNetReceiver->RegisterL(
        packetQoSMessHandler, PN_GPDS, GPDS_CONTEXT_CONFIGURE_RESP );
    aPhoNetReceiver->RegisterL(
        packetQoSMessHandler, PN_GPDS, GPDS_CONTEXT_ACTIVATE_IND );
    aPhoNetReceiver->RegisterL(
        packetQoSMessHandler, PN_GPDS, GPDS_CONTEXT_MODIFY_IND );

    CleanupStack::Pop( packetQoSMessHandler );

    return packetQoSMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::ConstructL()
    {

    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::ConstructL.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_CONSTRUCTL, "CMmPacketQoSMessHandler::ConstructL" );

    // Initialise to NULL
    iQos.iQoSMinInfo = NULL;
    iQos.iQoSReqInfo = NULL;
    iQos.iQoS99MinInfo = NULL;
    iQos.iQoS99ReqInfo = NULL;
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::ReceiveMessageL
// Entry point for messages received from Domestic OS.
// Switches the message to the correct method.
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::ReceiveMessageL(
    const TIsiReceiveC& aIsiMessage )
    {

    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::ReceiveMessageL." );
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketQoSMessHandler::ReceiveMessageL" );

    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    if ( PN_GPDS == resource )
        {
        switch ( messageId )
            {
            case GPDS_CONTEXT_CONFIGURE_RESP:
                {
                
                TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureResp." );
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketQoSMessHandler::ReceiveMessageL, GpdsContextConfigureResp" );
                GpdsContextConfigureResp( aIsiMessage );
                break;
                }
            case GPDS_CONTEXT_ACTIVATE_IND:
                {
                
                TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextActivateInd." );
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketQoSMessHandler::ReceiveMessageL, GpdsContextActivateInd" );
                GpdsContextActivateOrModifyInd( aIsiMessage );
                break;
                }
            case GPDS_CONTEXT_MODIFY_IND:
                {
                
                TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextModifyInd." );
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketQoSMessHandler::ReceiveMessageL, GpdsContextModifyInd" );
                GpdsContextActivateOrModifyInd( aIsiMessage );
                break;
                }
            default:
                {
                
                TFLOGSTRING("TSY: CMmPacketQoSMessHandler::ReceiveMessageL. Default case: Do nothing");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_RECEIVEMESSAGEL, "CMmPacketQoSMessHandler::ReceiveMessageL, Default case: Do nothing" );
                break;
                }
            }
        }
    // No else
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::GpdsContextConfigureReq
// Construct a GPDS_CONTEXT_CONFIGURE_REQ ISI-message
// -----------------------------------------------------------------------------
//
TInt CMmPacketQoSMessHandler::GpdsContextConfigureReq(
    const TUint8 aContextId,
    const TUint8 aPdpType,
    const TUint8 aContextType,
    const TUint8 aPrimaryContextId,
    const TQoS& aQoS,
    const TBool aSignallingIndication,
    const TBool aSourceStatisticsDescriptor ) const
    {
   
    TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureReq.  \t contextId:%d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketQoSMessHandler::GpdsContextConfigureReq;aContextId=%hhu", aContextId );

    TInt ret( KErrArgument );

    // Create isi message
    TIsiSend gpdsContextConfigureReq( iPhoNetSender->SendBufferDes() );
    gpdsContextConfigureReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_GPDS );

    // Set the Gpds Context Configure Req data
    gpdsContextConfigureReq.Set8bit(
        ISI_HEADER_OFFSET_TRANSID, GPDS_CONTEXT_CONFIGURE_REQ );
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

    // These subblocks are to be included valid only in rel5 .
    if ( TPacketDataConfigBase::KConfigRel5 == aQoS.iExtensionId )
        {
        // Add GPDS_SHARED_REL5_QOS_INFO sub block
        if( aSignallingIndication || aSourceStatisticsDescriptor )
            {
            TBuf8<SIZE_GPDS_SHARED_REL5_QOS_INFO> rel5QosInfoBuf;
            TIsiSubBlock rel5QosInfoSubBlock( 
                rel5QosInfoBuf,
                GPDS_SHARED_REL5_QOS_INFO, 
                EIsiSubBlockTypeId8Len8 );

            rel5QosInfoBuf.Append( aSourceStatisticsDescriptor );
            rel5QosInfoBuf.Append( aSignallingIndication );
            gpdsContextConfigureReq.CopyData(
                currentMsgOffset, rel5QosInfoSubBlock.CompleteSubBlock() );
            // Set new offset and increase subblock count
            currentMsgOffset =
                currentMsgOffset + SIZE_GPDS_SHARED_REL5_QOS_INFO;
            numOfSbInMessage++;
            }
        //no else
        }
    //no else

    if ( TPacketDataConfigBase::KConfigGPRS == aQoS.iExtensionId )
        {
        
        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureReq.  KConfigGPRS" );
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketQoSMessHandler::GpdsContextConfigureReq, KConfigGPRS" );
        // Add GPDS_DNS_ADDRESS_REQ_INFO sub block
        if( iMmPacketContextMesshandlerList->
            GetDNSAddressInfoToContextInfo(aContextId) )
            {
            
            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureReq. DNS Addresses REQUESTED" );

            TBuf8<SIZE_GPDS_DNS_ADDRESS_REQ_INFO> dnsAddrInfoBuf;
            TIsiSubBlock dnsAddrInfoSubBlock( 
                dnsAddrInfoBuf,
                GPDS_DNS_ADDRESS_REQ_INFO, 
                EIsiSubBlockTypeId8Len8 );

            dnsAddrInfoBuf.Append( KGpdsPadding );
            dnsAddrInfoBuf.Append( KGpdsPadding );
            gpdsContextConfigureReq.CopyData(
                currentMsgOffset, dnsAddrInfoSubBlock.CompleteSubBlock() );
            // Set new offset and increase subblock count
            currentMsgOffset =
                currentMsgOffset + SIZE_GPDS_DNS_ADDRESS_REQ_INFO;
            numOfSbInMessage++;
            }
        //no else

        // add GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO sub block
        // An OPTIONAL sub block of Configure Req
        if( iMmPacketContextMesshandlerList->
            GetPCSCFAddressInfoToContextInfo( aContextId ) )
			{
			
            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. PCSCF Addresses Requested");

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

        // Add GPDS_QOS_REQ_INFO sub block
        TBuf8<SIZE_GPDS_QOS_REQ_INFO> qosReqInfoBuf;
        TIsiSubBlock qosReqInfoSubBlock( 
            qosReqInfoBuf,
            GPDS_QOS_REQ_INFO, 
            EIsiSubBlockTypeId8Len8 );

        qosReqInfoBuf.Append( aQoS.iQoSReqInfo->iPrecedence );
        qosReqInfoBuf.Append( aQoS.iQoSReqInfo->iDelay );
        qosReqInfoBuf.Append( aQoS.iQoSReqInfo->iReliability );
        qosReqInfoBuf.Append( aQoS.iQoSReqInfo->iPeakThroughput );
        qosReqInfoBuf.Append( aQoS.iQoSReqInfo->iMeanThroughput );
        qosReqInfoBuf.Append( KGpdsPadding );
        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, qosReqInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + SIZE_GPDS_QOS_REQ_INFO;
        numOfSbInMessage++;

        // Add GPDS_QOS_MIN_INFO sub block
        TBuf8<SIZE_GPDS_QOS_MIN_INFO> qosMinInfoBuf;
        TIsiSubBlock qosMinInfoSubBlock( 
            qosMinInfoBuf,
            GPDS_QOS_MIN_INFO, 
            EIsiSubBlockTypeId8Len8 );

        qosMinInfoBuf.Append( aQoS.iQoSMinInfo->iPrecedence );
        qosMinInfoBuf.Append( aQoS.iQoSMinInfo->iDelay );
        qosMinInfoBuf.Append( aQoS.iQoSMinInfo->iReliability );
        qosMinInfoBuf.Append( aQoS.iQoSMinInfo->iPeakThroughput );
        qosMinInfoBuf.Append( aQoS.iQoSMinInfo->iMeanThroughput );
        qosMinInfoBuf.Append( KGpdsPadding );
        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, qosMinInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + SIZE_GPDS_QOS_MIN_INFO;
        numOfSbInMessage++;
        }
        
    else if ( TPacketDataConfigBase::KConfigRel99Rel4 == aQoS.iExtensionId
        || TPacketDataConfigBase::KConfigRel5 == aQoS.iExtensionId )
        {
        
        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureReq.  KConfigRel99Rel4" );
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketQoSMessHandler::GpdsContextConfigureReq, KConfigRel99Rel4" );

        // Add GPDS_DNS_ADDRESS_REQ_INFO sub block
        if( iMmPacketContextMesshandlerList->GetDNSAddressInfoToContextInfo(aContextId) )
            {
            
            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureReq.  DNS Addresses REQUESTED" );
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketQoSMessHandler::GpdsContextConfigureReq, DNS Addresses REQUESTED" );
            TBuf8<SIZE_GPDS_DNS_ADDRESS_REQ_INFO> dnsAddrInfoBuf;
            TIsiSubBlock dnsAddrInfoSubBlock( 
                dnsAddrInfoBuf,
                GPDS_DNS_ADDRESS_REQ_INFO, 
                EIsiSubBlockTypeId8Len8 );

            dnsAddrInfoBuf.Append( KGpdsPadding );
            dnsAddrInfoBuf.Append( KGpdsPadding );
            gpdsContextConfigureReq.CopyData(
                currentMsgOffset, dnsAddrInfoSubBlock.CompleteSubBlock() );
            // Set new offset and increase subblock count
            currentMsgOffset =
                currentMsgOffset + SIZE_GPDS_DNS_ADDRESS_REQ_INFO;
            numOfSbInMessage++;
            }
        //no else

        // add GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO sub block
        // An OPTIONAL sub block of Configure Req
        if( iMmPacketContextMesshandlerList->
            GetPCSCFAddressInfoToContextInfo( aContextId ) )
			{
			
            TFLOGSTRING("TSY: CMmPacketContextMessHandler::GpdsContextConfigureReq. PCSCF Addresses Requested");
OstTrace0( TRACE_NORMAL, DUP6_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTCONFIGUREREQ, "CMmPacketQoSMessHandler::GpdsContextConfigureReq, PCSCF Addresses Requested" );

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

        // Add GPDS_QOS99_REQ_INFO sub block
        TBuf8<SIZE_GPDS_QOS99_REQ_INFO> qos99ReqInfoBuf;
        TIsiSubBlock qos99ReqInfoSubBlock( 
            qos99ReqInfoBuf,
            GPDS_QOS99_REQ_INFO, 
            EIsiSubBlockTypeId8Len8 );

        qos99ReqInfoBuf.Append( aQoS.iQoS99ReqInfo->iTrafficClass );
        qos99ReqInfoBuf.Append( aQoS.iQoS99ReqInfo->iDeliveryOrderReqd );
        qos99ReqInfoBuf.Append( aQoS.iQoS99ReqInfo->iDeliverErroneousSDU );
        qos99ReqInfoBuf.Append( aQoS.iQoS99ReqInfo->iBER );
        qos99ReqInfoBuf.Append( aQoS.iQoS99ReqInfo->iSDUErrorRatio );
        qos99ReqInfoBuf.Append( aQoS.iQoS99ReqInfo->iTrafficHandlingPriority );

        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iTransferDelay >> KShift8Bits ) );
        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iTransferDelay ) );

        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iMaxSDUSize >> KShift8Bits ) );
        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iMaxSDUSize ) );

        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iMaxUplinkRate >> KShift8Bits ) );
        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iMaxUplinkRate ) );

        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iMaxDownlinkRate >> KShift8Bits ) );
        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iMaxDownlinkRate ) );

        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iGuaranteedUplinkRate >> KShift8Bits ) );
        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iGuaranteedUplinkRate ) );

        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iGuaranteedDownlinkRate >> KShift8Bits ) );
        qos99ReqInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99ReqInfo->iGuaranteedDownlinkRate ) );

        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, qos99ReqInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + SIZE_GPDS_QOS99_REQ_INFO;
        numOfSbInMessage++;

        // Add GPDS_QOS99_MIN_INFO sub block
        TBuf8<SIZE_GPDS_QOS99_MIN_INFO> qos99MinInfoBuf;
        TIsiSubBlock qos99MinInfoSubBlock( 
            qos99MinInfoBuf,
            GPDS_QOS99_MIN_INFO, 
            EIsiSubBlockTypeId8Len8 );
            
        qos99MinInfoBuf.Append( aQoS.iQoS99MinInfo->iTrafficClass );
        qos99MinInfoBuf.Append( aQoS.iQoS99MinInfo->iDeliveryOrderReqd );
        qos99MinInfoBuf.Append( aQoS.iQoS99MinInfo->iDeliverErroneousSDU );
        qos99MinInfoBuf.Append( aQoS.iQoS99MinInfo->iBER );
        qos99MinInfoBuf.Append( aQoS.iQoS99MinInfo->iSDUErrorRatio );
        qos99MinInfoBuf.Append( aQoS.iQoS99MinInfo->iTrafficHandlingPriority );

        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iTransferDelay >> KShift8Bits ) );
        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iTransferDelay ) );

        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iMaxSDUSize >> KShift8Bits ) );
        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iMaxSDUSize ) );

        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iMaxUplinkRate >> KShift8Bits ) );
        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iMaxUplinkRate ) );

        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iMaxDownlinkRate >> KShift8Bits ) );
        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iMaxDownlinkRate ) );

        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iGuaranteedUplinkRate >> KShift8Bits ) );
        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iGuaranteedUplinkRate ) );

        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iGuaranteedDownlinkRate >> KShift8Bits ) );
        qos99MinInfoBuf.Append( static_cast<TUint8>(
            aQoS.iQoS99MinInfo->iGuaranteedDownlinkRate ) );

        gpdsContextConfigureReq.CopyData(
            currentMsgOffset, qos99MinInfoSubBlock.CompleteSubBlock() );
        // Set new offset and increase subblock count
        currentMsgOffset =
            currentMsgOffset + SIZE_GPDS_QOS99_MIN_INFO;
        numOfSbInMessage++;
        }
    //no else

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
    currentMsgOffset = 
        currentMsgOffset + SIZE_GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO;
        numOfSbInMessage++;

    if ( TPacketDataConfigBase::KConfigGPRS == aQoS.iExtensionId ||
         TPacketDataConfigBase::KConfigRel99Rel4 == aQoS.iExtensionId ||
         TPacketDataConfigBase::KConfigRel5 == aQoS.iExtensionId )
        {
        // Set number of subblocs in GPDS_CONTEXT_CONFIGURE_REQ message
        gpdsContextConfigureReq.Set8bit(
            ISI_HEADER_SIZE + 
            GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_NUMBEROFSUBBLOCKS,
            numOfSbInMessage );

        // Send GPDS_CONTEXT_CONFIGURE_REQ message
        ret = iPhoNetSender->Send( gpdsContextConfigureReq.Complete() );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd
// Breaks a GPDS_CONTEXT_ACTIVATE_IND and GPDS_CONTEXT_MODIFY_IND
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd(
    const TIsiReceiveC& aIsiMessage )
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd." );
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_GPDSCONTEXTACTIVATEORMODIFYIND, "CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd" );

    TUint8 messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    TUint8 subBlockDataOffset( 0 );

    // Get context identifier
    TUint8 contextId( GPDS_CID_VOID );

    if ( GPDS_CONTEXT_ACTIVATE_IND == messageId )
        {
        contextId = aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_ACTIVATE_IND_OFFSET_CID );

        subBlockDataOffset = ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_ACTIVATE_IND;
        }
    else
        {
        contextId = aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_MODIFY_IND_OFFSET_CID );

        subBlockDataOffset = ISI_HEADER_SIZE + SIZE_GPDS_CONTEXT_MODIFY_IND;
        }

    TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd.  \t ContextId:%d", contextId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTACTIVATEORMODIFYIND, "CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd;contextId=%hhu", contextId );

    TUint sbStartOffSet( 0 );

    TInfoName contextName;

    // Get context list and find the correct context and qos
    TInt ret = iMmPacketContextMesshandlerList->
        GetContextNameByContextId( contextId, contextName );

    // if context is defined
    if ( KErrNone == ret )
        {
        TQoSInfo qoSNegotiated;
        TQoS99Info qoS99Negotiated;
        TBool qoSNegValid( EFalse );
        TBool qoS99NegValid( EFalse );

        // Set pointer subBlock to GPDS_QOS_NEG_INFO subblock
        ret = aIsiMessage.FindSubBlockOffsetById(
            subBlockDataOffset,
            GPDS_QOS_NEG_INFO,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet );

            // Get QoS params from sublock if R97/98 parameters are in use
            if ( KErrNone == ret )
                {
                qoSNegotiated.iPrecedence = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS_NEG_INFO_OFFSET_PRECEDENCE );

                qoSNegotiated.iDelay = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS_NEG_INFO_OFFSET_DELAY );

                qoSNegotiated.iReliability = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS_NEG_INFO_OFFSET_RELIABILITY );

                qoSNegotiated.iPeakThroughput = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS_NEG_INFO_OFFSET_PEAKTHROUGHPUT );

                qoSNegotiated.iMeanThroughput = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS_NEG_INFO_OFFSET_MEANTHROUGHPUT );

                qoSNegValid = ETrue;
                }
            //no else

            // Set pointer subBlock to GPDS_QOS99_NEG_INFO subblock
            ret = aIsiMessage.FindSubBlockOffsetById(
                subBlockDataOffset,
                GPDS_QOS99_NEG_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet );

            // Get QoS parameters from sublock if R99 parameters are in use
            if ( KErrNone == ret )
                {
                qoS99Negotiated.iTrafficClass = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_TRAFFICCLASS );

                qoS99Negotiated.iDeliveryOrderReqd = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_DELIVERYORDER );

                qoS99Negotiated.iDeliverErroneousSDU = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_DELIVERYOFERRONEUSSDU );

                qoS99Negotiated.iBER = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_RESIDUALBER );

                qoS99Negotiated.iSDUErrorRatio = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_ERRORRATIO );

                qoS99Negotiated.iTrafficHandlingPriority = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_PRIORITY );

                qoS99Negotiated.iTransferDelay = aIsiMessage.Get16bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_TRANSFERDELAY );

                qoS99Negotiated.iMaxSDUSize = aIsiMessage.Get16bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_MAXSDUSIZE );

                qoS99Negotiated.iMaxUplinkRate = aIsiMessage.Get16bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_MAXBITRATEUPLINK );

                qoS99Negotiated.iMaxDownlinkRate = aIsiMessage.Get16bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_MAXBITRATEDOWNLINK );

                qoS99Negotiated.iGuaranteedUplinkRate = aIsiMessage.Get16bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_GUABITRATEUPLINK );

                // if CASW_INITIAL_DCH_RATE flag is on and GPDS_SHARED_INITIAL_DL_DCH_RATE subblock
                // exists, GPDS_QOS99_NEG_INFO_OFFSET_GUABITRATEDOWNLINK subblock will be overwritten
                // with GPDS_SHARED_INITIAL_DL_DCH_RATE_OFFSET_MAXBITRATEDOWNLINK subblock
                qoS99Negotiated.iGuaranteedDownlinkRate = aIsiMessage.Get16bit(
                    sbStartOffSet + GPDS_QOS99_NEG_INFO_OFFSET_GUABITRATEDOWNLINK );

// INTERNAL_TESTING_CASW_INITIAL_DCH_RATE is only for winscw usage
#if defined ( CASW_INITIAL_DCH_RATE ) || defined ( INTERNAL_TESTING_CASW_INITIAL_DCH_RATE )
				// ------------------------------------------------------------------------
				// This NCP Feature flag ( CASW_INITIAL_DCH_RATE ) is Hutchinson specific flag
				// and need to be set OFF as default. The feature is BINARY VARIATED.
				// Initial DCH Rate is needed in the Streaming Application during
				// stream negotiation with server.
				// Set pointer subblock to GPDS_SHARED_INITIAL_DL_DCH_RATE (0xFC) subblock

                TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd. Flag CASW_INITIAL_DCH_RATE is defined" );
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTACTIVATEORMODIFYIND, "CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd, Flag CASW_INITIAL_DCH_RATE is defined" );

                ret = aIsiMessage.FindSubBlockOffsetById(
	               subBlockDataOffset,
	               GPDS_SHARED_INITIAL_DL_DCH_RATE,
	               EIsiSubBlockTypeId8Len8,
	               sbStartOffSet );

	            if ( KErrNone == ret )
		            {
		            // Get initial DCH parameters from subblock if CASW_INITIAL_DCH_RATE is defined
					qoS99Negotiated.iGuaranteedDownlinkRate = aIsiMessage.Get16bit(
                        sbStartOffSet + GPDS_SHARED_INITIAL_DL_DCH_RATE_OFFSET_MAXBITRATEDOWNLINK );
                        
                    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd. Sub block GPDS_SHARED_INITIAL_DL_DCH_RATE exists." );
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTACTIVATEORMODIFYIND, "CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd, Sub block GPDS_SHARED_INITIAL_DL_DCH_RATE exists" );
                    TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd. qoS99Negotiated.iGuaranteedDownlinkRate = %d", qoS99Negotiated.iGuaranteedDownlinkRate );
OstTraceExt1( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTACTIVATEORMODIFYIND, "CMmPacketQoSMessHandler::GpdsContextActivateOrModifyInd;qoS99Negotiated.iGuaranteedDownlinkRate=%hhu", qoS99Negotiated.iGuaranteedDownlinkRate );
		            }

#endif // defined ( CASW_INITIAL_DCH_RATE ) || defined ( INTERNAL_CASW_INITIAL_DCH_RATE )
				// ------------------------------------------------------------------------

                qoS99NegValid = ETrue;
                }
            //no else

            ret = aIsiMessage.FindSubBlockOffsetById(
                subBlockDataOffset,
                GPDS_SHARED_REL5_QOS_INFO,
                EIsiSubBlockTypeId8Len8,
                sbStartOffSet );

            qoS99Negotiated.iSignallingIndication= 0x00;
            qoS99Negotiated.iSourceStatisticsDescriptor =
                RPacketQoS::ESourceStatisticsDescriptorUnknown;

            if( KErrNone == ret )
                {
                qoS99Negotiated.iSignallingIndication = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_SHARED_REL5_QOS_INFO_OFFSET_SIGINDFLAG );

                qoS99Negotiated.iSourceStatisticsDescriptor = aIsiMessage.Get8bit(
                    sbStartOffSet + GPDS_SHARED_REL5_QOS_INFO_OFFSET_SRCSTATDESC );
                }
            //no else

            // Complete the ind
            CompleteNotifyProfileChanged( 
                &qoSNegotiated, 
                &qoS99Negotiated,
                qoSNegValid, 
                qoS99NegValid, 
                contextId);
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::GpdsContextConfigureResp
// Breaks a GPDS_CONTEXT_CONFIGURE_RESP ISI-message
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::GpdsContextConfigureResp(
    const TIsiReceiveC& aIsiMessage )
    {
    // Boolean used for checking that this is response for GpdsContextConfigureReq
    TBool called( EFalse );

    // Create GPDS_CONTEXT_CONFIGURE_RESP

    TUint8 contextId( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_CID ));

    TUint8 transId( aIsiMessage.Get8bit(
        ISI_HEADER_OFFSET_TRANSID ));

    TFLOGSTRING3("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureResp.\t contextId: %d, transId: %d", contextId, transId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_GPDSCONTEXTCONFIGURERESP, "CMmPacketQoSMessHandler::GpdsContextConfigureResp;contextId=%hhu;transId=%hhu", contextId, transId );

    if ( GPDS_CONTEXT_CONFIGURE_REQ == transId )
        {
        called = ETrue;
        }

    if ( called )
        {

        TUint8 status( aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_STATUS ));

        TInt errorValue( KErrNone );

        if ( GPDS_OK != status )
            {
            errorValue = KErrGeneral;
            }

        // Get ContextName and pack it for completion
        TInfoName contextName;
        TInt ret = iMmPacketContextMesshandlerList->
            GetContextNameByContextId( contextId, contextName );

        if( KErrNone == ret )
            {
            CMmDataPackage data;
            data.PackData( &contextName );
            iMessageRouter->Complete( 
                EPacketQoSSetProfileParams, 
                &data, 
                errorValue );
            }

        TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::GpdsContextConfigureResp.\t Status:%d",status);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_GPDSCONTEXTCONFIGURERESP, "CMmPacketQoSMessHandler::GpdsContextConfigureResp;status=%hhu", status );
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::ExtFuncL
// Entry point for messages received from Symbian OS layer (Message manager).
// -----------------------------------------------------------------------------
//
TInt CMmPacketQoSMessHandler::ExtFuncL(
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {

    TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::ExtFuncL. IPC:%d", aIpc);
OstTrace1( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_EXTFUNCL, "CMmPacketQoSMessHandler::ExtFuncL;aIpc=%d", aIpc );

    TInt ret( KErrNotSupported );

    switch ( aIpc )
        {
        case EPacketQoSSetProfileParams:
            {
            ret = SetProfileParametersL( aDataPackage );
            break;
            }
        default:
            {
            
            TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_EXTFUNCL, "CMmPacketQoSMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::SetProfileParametersL
// Sets QoS profile parameters.
// -----------------------------------------------------------------------------
//
TInt CMmPacketQoSMessHandler::SetProfileParametersL(
    const CMmDataPackage* aDataPackage )
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::SetProfileParametersL.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_SETPROFILEPARAMETERSL, "CMmPacketQoSMessHandler::SetProfileParametersL" );

    // error value
    TInt ret( KErrNone );

    TBool supported( EFalse );
    TBool signallingIndication( EFalse );

    // Params for unpacking
    TContextParams contextparams;
    TDes8* aprofile = NULL;

    // Rel5 statistic Descriptor, initialised default EFalse
    RPacketQoS::TSourceStatisticsDescriptor sourceStatisticsDescriptor(
        RPacketQoS::ESourceStatisticsDescriptorUnknown );
    // unpack
    aDataPackage->UnPackData( contextparams, aprofile );

    TPckg<TPacketDataConfigBase>* profilePckg =
        reinterpret_cast<TPckg<TPacketDataConfigBase>*>( aprofile );

    TPacketDataConfigBase& profile = ( *profilePckg )();

    // get contextId by contextname
    TUint8 contextId = iMmPacketContextMesshandlerList->
        GetContextIdByContextName( &(contextparams.iContextName ) );

    // get primarycontextId by primarycontextname
    TUint8 primarycontextId = iMmPacketContextMesshandlerList->
        GetContextIdByContextName( &(contextparams.iPrimaryContextName) );

    // Get extension id
    iQos.iExtensionId = profile.ExtensionId();

    if ( TPacketDataConfigBase::KConfigGPRS == iQos.iExtensionId )
        {
        
        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::SetProfileParametersL. => Rel97");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_SETPROFILEPARAMETERSL, "CMmPacketQoSMessHandler::SetProfileParametersL => Rel97" );
        supported = ETrue;
        TPckg<RPacketQoS::TQoSGPRSRequested >* gprsProfilePckg =
            reinterpret_cast<TPckg<RPacketQoS::TQoSGPRSRequested>*>(
                aprofile );
        RPacketQoS::TQoSGPRSRequested& gprsProfile = ( *gprsProfilePckg )();

            // If context status equals neither active nor activating
            if ( RPacketContext::EStatusActivating !=
                     contextparams.iContextStatus &&
                 RPacketContext::EStatusActive !=
                     contextparams.iContextStatus )
                {
                if ( NULL == iQos.iQoSMinInfo )
                    {
                    iQos.iQoSMinInfo = new ( ELeave )
                        CMmPacketQoSMessHandler::TQoSInfo;
                    }
                //no else

                // Map QoS R97/98 minimal parameter values to server
                // side struct
                ret = MapQoSInfoToServer(
                    *( iQos.iQoSMinInfo ),
                    gprsProfile.iMinPrecedence,
                    gprsProfile.iMinDelay,
                    gprsProfile.iMinReliability,
                    gprsProfile.iMinPeakThroughput,
                    gprsProfile.iMinMeanThroughput );
                }
            else if ( NULL != iQos.iQoSMinInfo )
                {
                delete iQos.iQoSMinInfo;
                iQos.iQoSMinInfo = NULL;
                }

        if ( KErrNone == ret )
            {
            if ( NULL == iQos.iQoSReqInfo )
                {
                iQos.iQoSReqInfo = new ( ELeave )
                    CMmPacketQoSMessHandler::TQoSInfo;
                }
            //no else

            // Map QoS R97/98 requested parameter values to server
            // side struct
            ret = MapQoSInfoToServer(
                *( iQos.iQoSReqInfo ),
                gprsProfile.iReqPrecedence,
                gprsProfile.iReqDelay,
                gprsProfile.iReqReliability,
                gprsProfile.iReqPeakThroughput,
                gprsProfile.iReqMeanThroughput );
             }

        // Delete QoS R99 info structs
        if ( NULL != iQos.iQoS99MinInfo )
            {
            delete iQos.iQoS99MinInfo;
            iQos.iQoS99MinInfo = NULL;
            }
        //no else

        if ( NULL != iQos.iQoS99ReqInfo )
            {
            delete iQos.iQoS99ReqInfo;
            iQos.iQoS99ReqInfo = NULL;
            }
        //no else
        }
        
    // If QoS R99 parameters in use
    else if ( TPacketDataConfigBase::KConfigRel99Rel4 == profile.ExtensionId() )
        {
        
        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::SetProfileParametersL. => Rel99");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_SETPROFILEPARAMETERSL, "CMmPacketQoSMessHandler::SetProfileParametersL => Rel99" );
        supported = ETrue;

        TPckg<RPacketQoS::TQoSR99_R4Requested >* gprsProfilePckg =
            reinterpret_cast<TPckg<RPacketQoS::TQoSR99_R4Requested>*>(
                aprofile );
        RPacketQoS::TQoSR99_R4Requested& gprsProfile = ( *gprsProfilePckg )();

        // If context status equals neither active nor activating
        if ( RPacketContext::EStatusActivating !=
                contextparams.iContextStatus &&
             RPacketContext::EStatusActive !=
                contextparams.iContextStatus )
            {
            if ( NULL == iQos.iQoS99MinInfo )
                {
                iQos.iQoS99MinInfo = new ( ELeave )
                    CMmPacketQoSMessHandler::TQoS99Info;
                }
            //no else

            // Map classified QoS R99 minimal parameter values to server
            // side struct
            ret = MapQoS99InfoToServer(
                *( iQos.iQoS99MinInfo ),
                gprsProfile.iMinTrafficClass,
                gprsProfile.iMinDeliveryOrderReqd,
                gprsProfile.iMinDeliverErroneousSDU,
                gprsProfile.iMaxBER,
                gprsProfile.iMaxSDUErrorRatio,
                gprsProfile.iMinTrafficHandlingPriority );

            // Copy numerical QoS R99 minimal parameter values to server
            // side struct
            if ( KErrNone == ret )
                {
                ret = CopyQoS99ValuesToServer(
                    *( iQos.iQoS99MinInfo ),
                    gprsProfile.iMaxTransferDelay,
                    gprsProfile.iMinAcceptableMaxSDUSize,
                    gprsProfile.iMinAcceptableMaxRate,
                    gprsProfile.iMinGuaranteedRate );
                }
            else
                {
                if ( NULL != iQos.iQoS99MinInfo )
                    {
                    delete iQos.iQoS99MinInfo;
                    iQos.iQoS99MinInfo = NULL;
                    }
                }
            }
        if ( KErrNone == ret )
            {
            if ( NULL == iQos.iQoS99ReqInfo )
                {
                iQos.iQoS99ReqInfo = new ( ELeave )
                    CMmPacketQoSMessHandler::TQoS99Info;
                }
            //no else

            // Map classified QoS R99 requested parameter values to server
            // side struct
            ret = MapQoS99InfoToServer(
                *( iQos.iQoS99ReqInfo ),
                gprsProfile.iReqTrafficClass,
                gprsProfile.iReqDeliveryOrderReqd,
                gprsProfile.iReqDeliverErroneousSDU,
                gprsProfile.iReqBER,
                gprsProfile.iReqSDUErrorRatio,
                gprsProfile.iReqTrafficHandlingPriority );

            // Copy numerical QoS R99 requested parameter values to server
            // side struct
            if ( KErrNone == ret )
                {
                ret = CopyQoS99ValuesToServer(
                    *( iQos.iQoS99ReqInfo ),
                    gprsProfile.iReqTransferDelay,
                    gprsProfile.iReqMaxSDUSize,
                    gprsProfile.iReqMaxRate,
                    gprsProfile.iReqGuaranteedRate );
                }
            //no else
            }

        // Delete QoS R97/98 info structs
        if ( NULL != iQos.iQoSMinInfo )
            {
            delete iQos.iQoSMinInfo;
            iQos.iQoSMinInfo = NULL;
            }
        //no else

        if ( NULL != iQos.iQoSReqInfo )
            {
            delete iQos.iQoSReqInfo;
            iQos.iQoSReqInfo = NULL;
            }
        //no else
        }
        
    // If QoS R5 parameters in use
    else if ( TPacketDataConfigBase::KConfigRel5 == profile.ExtensionId() )
        {
        
        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::SetProfileParametersL.=>R5");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_SETPROFILEPARAMETERSL, "CMmPacketQoSMessHandler::SetProfileParametersL => R5" );
        supported = ETrue;
        
        TPckg<RPacketQoS::TQoSR5Requested >* gprsProfilePckg =
            reinterpret_cast<TPckg<RPacketQoS::TQoSR5Requested>*>( aprofile );
        RPacketQoS::TQoSR5Requested& gprsProfile = ( *gprsProfilePckg )();

        // If context status equals neither active nor activating
        if ( RPacketContext::EStatusActivating !=
                contextparams.iContextStatus &&
             RPacketContext::EStatusActive !=
                contextparams.iContextStatus )
            {
            if ( NULL == iQos.iQoS99MinInfo )
                {
                iQos.iQoS99MinInfo = new ( ELeave )
                    CMmPacketQoSMessHandler::TQoS99Info;
                }
            //no else

            // Map classified QoS R99 minimal parameter values to server
            // side struct
            ret = MapQoS99InfoToServer(
                *( iQos.iQoS99MinInfo ),
                gprsProfile.iMinTrafficClass,
                gprsProfile.iMinDeliveryOrderReqd,
                gprsProfile.iMinDeliverErroneousSDU,
                gprsProfile.iMaxBER,
                gprsProfile.iMaxSDUErrorRatio,
                gprsProfile.iMinTrafficHandlingPriority );

            // Copy numerical QoS R99 minimal parameter values to server
            // side struct
            if ( KErrNone == ret )
                {
                ret = CopyQoS99ValuesToServer(
                    *( iQos.iQoS99MinInfo ),
                    gprsProfile.iMaxTransferDelay,
                    gprsProfile.iMinAcceptableMaxSDUSize,
                    gprsProfile.iMinAcceptableMaxRate,
                    gprsProfile.iMinGuaranteedRate );
                }
            else
                {
                if ( NULL != iQos.iQoS99MinInfo )
                    {
                    delete iQos.iQoS99MinInfo;
                    iQos.iQoS99MinInfo = NULL;
                    }
                //no else
                }
            }

        if ( KErrNone == ret )
            {
            if ( NULL == iQos.iQoS99ReqInfo )
                {
                iQos.iQoS99ReqInfo = new ( ELeave )
                    CMmPacketQoSMessHandler::TQoS99Info;
                }
            //no else

            // Map classified QoS R99 requested parameter values to server
            // side struct
            ret = MapQoS99InfoToServer(
                *( iQos.iQoS99ReqInfo ),
                gprsProfile.iReqTrafficClass,
                gprsProfile.iReqDeliveryOrderReqd,
                gprsProfile.iReqDeliverErroneousSDU,
                gprsProfile.iReqBER,
                gprsProfile.iReqSDUErrorRatio,
                gprsProfile.iReqTrafficHandlingPriority );

            // Copy numerical QoS R99 requested parameter values to server
            // side struct
            if ( KErrNone == ret )
                {
                ret = CopyQoS99ValuesToServer(
                    *( iQos.iQoS99ReqInfo ),
                    gprsProfile.iReqTransferDelay,
                    gprsProfile.iReqMaxSDUSize,
                    gprsProfile.iReqMaxRate,
                    gprsProfile.iReqGuaranteedRate );
                }
            }

        signallingIndication = gprsProfile.iSignallingIndication;
        sourceStatisticsDescriptor = gprsProfile.iSourceStatisticsDescriptor;

        TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::SetProfileParametersL. signallingIndication:%d", signallingIndication );

OstTraceExt1( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_SETPROFILEPARAMETERSL, "CMmPacketQoSMessHandler::SetProfileParametersL;signallingIndication=%hhu", signallingIndication );
        TFLOGSTRING2("TSY: CMmPacketQoSMessHandler::SetProfileParametersL. sourceStatisticsDescriptor:%d", sourceStatisticsDescriptor );
OstTraceExt1( TRACE_NORMAL, DUP5_CMMPACKETQOSMESSHANDLER_SETPROFILEPARAMETERSL, "CMmPacketQoSMessHandler::SetProfileParametersL;sourceStatisticsDescriptor=%hhu", sourceStatisticsDescriptor );

        // Delete QoS R97/98 info structs
        if ( NULL != iQos.iQoSMinInfo )
            {
            delete iQos.iQoSMinInfo;
            iQos.iQoSMinInfo = NULL;
            }
        //no else

        if ( NULL != iQos.iQoSReqInfo )
            {
            delete iQos.iQoSReqInfo;
            iQos.iQoSReqInfo = NULL;
            }
        //no else
        }

    TBool contextConfiguration( EFalse );

    if ( supported )
        {
        if ( KErrNone == ret &&
             RPacketContext::EStatusActivating != contextparams.iContextStatus &&
             RPacketContext::EStatusActive != contextparams.iContextStatus )
            {
            contextConfiguration = ETrue;

            TUint8 pdpTypeServer;

            switch ( contextparams.iPdpType )
                {
                case RPacketContext::EPdpTypeIPv4:
                    {
                    pdpTypeServer = GPDS_PDP_TYPE_IPV4;
                    break;
                    }

                case RPacketContext::EPdpTypeIPv6:
                    {
                    pdpTypeServer = GPDS_PDP_TYPE_IPV6;
                    break;
                    }

                case RPacketContext::EPdpTypePPP:
                case RPacketContext::EPdpTypeX25:
                case RPacketContext::EPdpTypeCDPD:
                    {
                    pdpTypeServer = GPDS_PDP_TYPE_DEFAULT;
                    break;
                    }
                default:
                    {
                    pdpTypeServer = GPDS_PDP_TYPE_DEFAULT;
                    break;
                    }
                }

            // Get context configuration
            TInt configurationType = iMmPacketContextMesshandlerList->
                GetContextConfigurationType( contextId );

            TUint8 contextType( GPDS_CONT_TYPE_NORMAL );

            // Get context configuration
            if ( TPacketDataConfigBase::KConfigGPRS ==
                configurationType )
                {
                RPacketContext::TContextConfigGPRS* config =
                    reinterpret_cast<RPacketContext::TContextConfigGPRS*>(
                        iMmPacketContextMesshandlerList->
                            GetConfig( contextId, configurationType ) );
                // Check if mobile or network initiated context
                if ( ( config ) &&  ( config->iNWIContext ) )
                    {
                    // Network initiated context
                    contextType = GPDS_CONT_TYPE_NWI;
                    }
                //no else
                }
            else
                {
                RPacketContext::TContextConfigR99_R4* config_R99 =
                    reinterpret_cast<RPacketContext::TContextConfigR99_R4*>(
                        iMmPacketContextMesshandlerList->
                            GetConfig( contextId, configurationType ) );
                // Check if mobile or network initiated context
                if ( ( config_R99 ) && ( config_R99->iNWIContext ) )
                    {
                    // Network initiated context
                    contextType = GPDS_CONT_TYPE_NWI;
                    }
                //no else
                }

            if( contextType != GPDS_CONT_TYPE_NWI )
                {
                // Mobile initiated context
                // => get type (primary or secondary)
                TInfoName contextName;

                iMmPacketContextMesshandlerList->
                    GetContextNameByContextId( contextId, contextName );

                contextType = iMmPacketContextMesshandlerList->
                    GetContextTypeByName( &contextName );
                }
                //no else

            // Call GpdsContextConfigureReq
            ret = GpdsContextConfigureReq(
                contextId, pdpTypeServer,
                contextType, primarycontextId, iQos, signallingIndication,
                sourceStatisticsDescriptor );

            ResetRequestedQoS( iQos );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    if ( KErrNone != ret || contextConfiguration )
        {
        ResetRequestedQoS( iQos );
        }
    //no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::MapQoSInfoToServer
// Maps parameters to server.
// -----------------------------------------------------------------------------
//
TInt CMmPacketQoSMessHandler::MapQoSInfoToServer(
    TQoSInfo& aQoSInfo,
    const RPacketQoS::TQoSPrecedence& aPrecedence,
    const RPacketQoS::TQoSDelay& aDelay,
    const RPacketQoS::TQoSReliability& aReliability,
    const RPacketQoS::TQoSPeakThroughput& aPeakThroughput,
    const RPacketQoS::TQoSMeanThroughput& aMeanThroughput )
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToServer.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOSERVER, "CMmPacketQoSMessHandler::MapQoSInfoToServer" );

    TInt ret( KErrNone );

    switch ( aPrecedence )
        {
        case RPacketQoS::EUnspecifiedPrecedence:
            {
            aQoSInfo.iPrecedence = GPDS_QOS_PREC_0;
            break;
            }
        case RPacketQoS::EPriorityHighPrecedence:
            {
            aQoSInfo.iPrecedence = GPDS_QOS_PREC_1;
            break;
            }
        case RPacketQoS::EPriorityMediumPrecedence:
            {
            aQoSInfo.iPrecedence = GPDS_QOS_PREC_2;
            break;
            }
        case RPacketQoS::EPriorityLowPrecedence:
            {
            aQoSInfo.iPrecedence = GPDS_QOS_PREC_3;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToServer. aPrecedence -> default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOSERVER, "CMmPacketQoSMessHandler::MapQoSInfoToServer,aPrecedence -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aDelay )
        {
        case RPacketQoS::EUnspecifiedDelayClass:
            {
            aQoSInfo.iDelay = GPDS_QOS_DELC_0;
            break;
            }
        case RPacketQoS::EDelayClass1:
            {
            aQoSInfo.iDelay = GPDS_QOS_DELC_1;
            break;
            }
        case RPacketQoS::EDelayClass2:
            {
            aQoSInfo.iDelay = GPDS_QOS_DELC_2;
            break;
            }
        case RPacketQoS::EDelayClass3:
            {
            aQoSInfo.iDelay = GPDS_QOS_DELC_3;
            break;
            }
        case RPacketQoS::EDelayClass4:
            {
            aQoSInfo.iDelay = GPDS_QOS_DELC_4;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToServer. aDelay -> default");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOSERVER, "CMmPacketQoSMessHandler::MapQoSInfoToServer, aDelay -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aReliability )
        {
        case RPacketQoS::EUnspecifiedReliabilityClass:
            {
            aQoSInfo.iReliability = GPDS_QOS_RELC_0;
            break;
            }
        case RPacketQoS::EReliabilityClass1:
            {
            aQoSInfo.iReliability = GPDS_QOS_RELC_1;
            break;
            }
        case RPacketQoS::EReliabilityClass2:
            {
            aQoSInfo.iReliability = GPDS_QOS_RELC_2;
            break;
            }
        case RPacketQoS::EReliabilityClass3:
            {
            aQoSInfo.iReliability = GPDS_QOS_RELC_3;
            break;
            }
        case RPacketQoS::EReliabilityClass4:
            {
            aQoSInfo.iReliability = GPDS_QOS_RELC_4;
            break;
            }
        case RPacketQoS::EReliabilityClass5:
            {
            aQoSInfo.iReliability = GPDS_QOS_RELC_5;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToServer. aReliability -> default");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOSERVER, "CMmPacketQoSMessHandler::MapQoSInfoToServer, aReliability -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aPeakThroughput )
        {
        case RPacketQoS::EUnspecifiedPeakThroughput:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_0;
            break;
            }
        case RPacketQoS::EPeakThroughput1000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_1;
            break;
            }
        case RPacketQoS::EPeakThroughput2000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_2;
            break;
            }
        case RPacketQoS::EPeakThroughput4000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_3;
            break;
            }
        case RPacketQoS::EPeakThroughput8000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_4;
            break;
            }
        case RPacketQoS::EPeakThroughput16000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_5;
            break;
            }
        case RPacketQoS::EPeakThroughput32000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_6;
            break;
            }
        case RPacketQoS::EPeakThroughput64000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_7;
            break;
            }
        case RPacketQoS::EPeakThroughput128000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_8;
            break;
            }
        case RPacketQoS::EPeakThroughput256000:
            {
            aQoSInfo.iPeakThroughput = GPDS_QOS_PETC_9;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToServer. aPeakThroughput -> default");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOSERVER, "CMmPacketQoSMessHandler::MapQoSInfoToServer, aPeakThroughput -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aMeanThroughput )
        {
        case RPacketQoS::EUnspecifiedMeanThroughput:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_0;
            break;
            }
        case RPacketQoS::EMeanThroughput100:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_1;
            break;
            }
        case RPacketQoS::EMeanThroughput200:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_2;
            break;
            }
        case RPacketQoS::EMeanThroughput500:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_3;
            break;
            }
        case RPacketQoS::EMeanThroughput1000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_4;
            break;
            }
        case RPacketQoS::EMeanThroughput2000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_5;
            break;
            }
        case RPacketQoS::EMeanThroughput5000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_6;
            break;
            }
        case RPacketQoS::EMeanThroughput10000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_7;
            break;
            }
        case RPacketQoS::EMeanThroughput20000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_8;
            break;
            }
        case RPacketQoS::EMeanThroughput50000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_9;
            break;
            }
        case RPacketQoS::EMeanThroughput100000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_10;
            break;
            }
        case RPacketQoS::EMeanThroughput200000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_11;
            break;
            }
        case RPacketQoS::EMeanThroughput500000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_12;
            break;
            }
        case RPacketQoS::EMeanThroughput1000000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_13;
            break;
            }
        case RPacketQoS::EMeanThroughput2000000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_14;
            break;
            }
        case RPacketQoS::EMeanThroughput5000000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_15;
            break;
            }
        case RPacketQoS::EMeanThroughput10000000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_16;
            break;
            }
        case RPacketQoS::EMeanThroughput20000000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_17;
            break;
            }
        case RPacketQoS::EMeanThroughput50000000:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_18;
            break;
            }
        case RPacketQoS::EMeanThroughputBestEffort:
            {
            aQoSInfo.iMeanThroughput = GPDS_QOS_METC_31;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToServer. aMeanThroughput -> default");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOSERVER, "CMmPacketQoSMessHandler::MapQoSInfoToServer, aMeanThroughput -> default" );

            ret = KErrArgument;
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::MapQoS99InfoToServer
// Maps parameters to server.
// -----------------------------------------------------------------------------
//
TInt CMmPacketQoSMessHandler::MapQoS99InfoToServer(
    CMmPacketQoSMessHandler::TQoS99Info& aQoS99Info,
    const RPacketQoS::TTrafficClass& aTrafficClass,
    const RPacketQoS::TDeliveryOrder& aDeliveryOrderReqd,
    const RPacketQoS::TErroneousSDUDelivery& aDeliverErroneousSDU,
    const RPacketQoS::TBitErrorRatio& aBER, // Bit error ratio
    const RPacketQoS::TSDUErrorRatio& aSDUErrorRatio,
    const RPacketQoS::TTrafficHandlingPriority& aTrafficHandlingPriority )
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToServer.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOSERVER, "CMmPacketQoSMessHandler::MapQoS99InfoToServer" );
    TInt ret( KErrNone );

    switch ( aTrafficClass )
        {
        case RPacketQoS::ETrafficClassUnspecified:
            {
            aQoS99Info.iTrafficClass = GPDS_QOS99_TRAC_SUBSCRIBED;
            break;
            }
        case RPacketQoS::ETrafficClassConversational:
            {
            aQoS99Info.iTrafficClass = GPDS_QOS99_TRAC_CONVERSATIONAL;
            break;
            }
        case RPacketQoS::ETrafficClassStreaming:
            {
            aQoS99Info.iTrafficClass = GPDS_QOS99_TRAC_STREAMING;
            break;
            }
        case RPacketQoS::ETrafficClassInteractive:
            {
            aQoS99Info.iTrafficClass = GPDS_QOS99_TRAC_INTERACTIVE;
            break;
            }
        case RPacketQoS::ETrafficClassBackground:
            {
            aQoS99Info.iTrafficClass = GPDS_QOS99_TRAC_BACKGROUND;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToServer. aTrafficClass -> default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOSERVER, "CMmPacketQoSMessHandler::MapQoS99InfoToServer, aTrafficClass -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aDeliveryOrderReqd )
        {
        case RPacketQoS::EDeliveryOrderUnspecified:
            {
            aQoS99Info.iDeliveryOrderReqd = GPDS_QOS99_DELO_SUBSCRIBED;
            break;
            }
        case RPacketQoS::EDeliveryOrderRequired:
            {
            aQoS99Info.iDeliveryOrderReqd = GPDS_QOS99_DELO_YES;
            break;
            }
        case RPacketQoS::EDeliveryOrderNotRequired:
            {
            aQoS99Info.iDeliveryOrderReqd = GPDS_QOS99_DELO_NO;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToServer. aDeliveryOrderReqd -> default");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOSERVER, "CMmPacketQoSMessHandler::MapQoS99InfoToServer, aDeliveryOrderReqd -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aDeliverErroneousSDU )
        {
        case RPacketQoS::EErroneousSDUDeliveryUnspecified:
            {
            aQoS99Info.iDeliverErroneousSDU = GPDS_QOS99_DOES_SUBSCRIBED;
            break;
            }
        case RPacketQoS::EErroneousSDUNoDetection:
            {
            aQoS99Info.iDeliverErroneousSDU = GPDS_QOS99_DOES_NO_DETECT;
            break;
            }
        case RPacketQoS::EErroneousSDUDeliveryRequired:
            {
            aQoS99Info.iDeliverErroneousSDU = GPDS_QOS99_DOES_YES;
            break;
            }
        case RPacketQoS::EErroneousSDUDeliveryNotRequired:
            {
            aQoS99Info.iDeliverErroneousSDU = GPDS_QOS99_DOES_NO;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToServer. aDeliverErroneousSDU -> default");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOSERVER, "CMmPacketQoSMessHandler::MapQoS99InfoToServer, aDeliverErroneousSDU -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aBER )
        {
        case RPacketQoS::EBERUnspecified:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_SUBSCRIBED;
            break;
            }
        case RPacketQoS::EBERFivePerHundred:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_1;
            break;
            }
        case RPacketQoS::EBEROnePerHundred:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_2;
            break;
            }
        case RPacketQoS::EBERFivePerThousand:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_3;
            break;
            }
        case RPacketQoS::EBERFourPerThousand:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_4;
            break;
            }
        case RPacketQoS::EBEROnePerThousand:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_5;
            break;
            }
        case RPacketQoS::EBEROnePerTenThousand:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_6;
            break;
            }
        case RPacketQoS::EBEROnePerHundredThousand:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_7;
            break;
            }
        case RPacketQoS::EBEROnePerMillion:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_8;
            break;
            }
        case RPacketQoS::EBERSixPerHundredMillion:
            {
            aQoS99Info.iBER = GPDS_QOS99_RBER_9;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToServer. aBER -> default");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOSERVER, "CMmPacketQoSMessHandler::MapQoS99InfoToServer, aBER -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aSDUErrorRatio )
        {
        case RPacketQoS::ESDUErrorRatioUnspecified:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_SUBSCRIBED;
            break;
            }
        case RPacketQoS::ESDUErrorRatioOnePerTen:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_7;
            break;
            }
        case RPacketQoS::ESDUErrorRatioOnePerHundred:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_1;
            break;
            }
        case RPacketQoS::ESDUErrorRatioSevenPerThousand:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_2;
            break;
            }
        case RPacketQoS::ESDUErrorRatioOnePerThousand:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_3;
            break;
            }
        case RPacketQoS::ESDUErrorRatioOnePerTenThousand:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_4;
            break;
            }
        case RPacketQoS::ESDUErrorRatioOnePerHundredThousand:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_5;
            break;
            }
        case RPacketQoS::ESDUErrorRatioOnePerMillion:
            {
            aQoS99Info.iSDUErrorRatio = GPDS_QOS99_SDER_6;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToServer. aSDUErrorRatio -> default");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOSERVER, "CMmPacketQoSMessHandler::MapQoS99InfoToServer, aSDUErrorRatio -> default" );

            ret = KErrArgument;
            break;
            }
        }

    switch ( aTrafficHandlingPriority )
        {
        case RPacketQoS::ETrafficPriorityUnspecified:
            {
            aQoS99Info.iTrafficHandlingPriority = GPDS_QOS99_TRHP_SUBSCRIBED;
            break;
            }
        case RPacketQoS::ETrafficPriority1:
            {
            aQoS99Info.iTrafficHandlingPriority = GPDS_QOS99_TRHP_1;
            break;
            }
        case RPacketQoS::ETrafficPriority2:
            {
            aQoS99Info.iTrafficHandlingPriority = GPDS_QOS99_TRHP_2;
            break;
            }
        case RPacketQoS::ETrafficPriority3:
            {
            aQoS99Info.iTrafficHandlingPriority = GPDS_QOS99_TRHP_3;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToServer. aTrafficHandlingPriority -> default");
OstTrace0( TRACE_NORMAL, DUP6_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOSERVER, "CMmPacketQoSMessHandler::MapQoS99InfoToServer, aTrafficHandlingPriority -> default" );

            ret = KErrArgument;
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::CopyQoS99ValuesToServer
// Copies QoS99 parameter values to server
// -----------------------------------------------------------------------------
//
TInt CMmPacketQoSMessHandler::CopyQoS99ValuesToServer(
    TQoS99Info& aQoS99Info,
    const TInt aMaxTransferDelay,
    const TInt aMaxSDUSize,
    const RPacketQoS::TBitRate& aMaxRate,
    const RPacketQoS::TBitRate& aGuaranteedRate )
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::CopyQoS99ValuesToServer.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_COPYQOS99VALUESTOSERVER, "CMmPacketQoSMessHandler::CopyQoS99ValuesToServer" );

    aQoS99Info.iTransferDelay = static_cast<TUint16>( aMaxTransferDelay );
    aQoS99Info.iMaxSDUSize = static_cast<TUint16>( aMaxSDUSize );

    aQoS99Info.iMaxUplinkRate = static_cast<TUint16>( aMaxRate.iUplinkRate );
    aQoS99Info.iMaxDownlinkRate = static_cast<TUint16>(
        aMaxRate.iDownlinkRate );

    aQoS99Info.iGuaranteedUplinkRate = static_cast<TUint16>(
        aGuaranteedRate.iUplinkRate );
    aQoS99Info.iGuaranteedDownlinkRate = static_cast<TUint16>(
        aGuaranteedRate.iDownlinkRate );

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::ResetRequestedQoS
// Resets QoS parameters.
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::ResetRequestedQoS(
    TQoS& aQoS )
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::ResetRequestedQoS.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_RESETREQUESTEDQOS, "CMmPacketQoSMessHandler::ResetRequestedQoS" );

    if ( NULL != aQoS.iQoSMinInfo )
        {
        delete aQoS.iQoSMinInfo;
        aQoS.iQoSMinInfo = NULL;
        }

    if ( NULL != aQoS.iQoSReqInfo )
        {
        delete aQoS.iQoSReqInfo;
        aQoS.iQoSReqInfo = NULL;
        }

    if ( NULL != aQoS.iQoS99MinInfo )
        {
        delete aQoS.iQoS99MinInfo;
        aQoS.iQoS99MinInfo = NULL;
        }

    if ( NULL != aQoS.iQoS99ReqInfo )
        {
        delete aQoS.iQoS99ReqInfo;
        aQoS.iQoS99ReqInfo = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::CompleteNotifyProfileChanged
// Completes profile changed notification
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::CompleteNotifyProfileChanged(
    TQoSInfo*  aQoSNegotiated,
    TQoS99Info*  aQoS99Negotiated,
    const TBool aQoSNegValid,     // Negotiated QoS 97 parameter indicator
    const TBool aQoS99NegValid,
    TUint8 aContextId )
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::CompleteNotifyProfileChanged.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_COMPLETENOTIFYPROFILECHANGED, "CMmPacketQoSMessHandler::CompleteNotifyProfileChanged" );

    TInt ret( KErrGeneral );
    TInfoName mmPacketContextName;
    CMmPacketDataPackage data;
    TBool completeToClient( EFalse );

    if ( aQoSNegValid )
        {
        MapQoSInfoToClient( *aQoSNegotiated,
            iQoSGPRSNegotiated.iPrecedence,
            iQoSGPRSNegotiated.iDelay,
            iQoSGPRSNegotiated.iReliability,
            iQoSGPRSNegotiated.iPeakThroughput,
            iQoSGPRSNegotiated.iMeanThroughput );

        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::CompleteNotifyProfileChanged.  R97/98 QoS Parameters set in TSY");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_COMPLETENOTIFYPROFILECHANGED, "CMmPacketQoSMessHandler::CompleteNotifyProfileChanged, R97/98 QoS Parameters set in TSY" );
        }
    //no else

    if ( aQoS99NegValid )
        {
        MapQoS99InfoToClient( *aQoS99Negotiated, 
            iQoSR99_R4Negotiated, 
            iQoSR5_Negotiated );

        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::CompleteNotifyProfileChanged.  R99/R4 QoS Parameters set in TSY");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_COMPLETENOTIFYPROFILECHANGED, "CMmPacketQoSMessHandler::CompleteNotifyProfileChanged, R99/R4 QoS Parameters set in TSY" );
        }
    //no else

    if ( aQoSNegValid )
        {
        completeToClient = ETrue;

        ret = iMmPacketContextMesshandlerList->
            GetContextNameByContextId( aContextId, mmPacketContextName );

        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::CompleteNotifyProfileChanged.  R97/98 QoS Parameters in use");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_COMPLETENOTIFYPROFILECHANGED, "CMmPacketQoSMessHandler::CompleteNotifyProfileChanged, R97/98 QoS Parameters in use" );
        }
     //no else

    // if client uses R99 QoS parameters and aQoS99Negotiated
    // have been set
    if ( aQoS99NegValid )
        {
        completeToClient = ETrue;

        ret = iMmPacketContextMesshandlerList->
            GetContextNameByContextId( aContextId, mmPacketContextName );

        TFLOGSTRING("TSY: CMmPacketQoSMessHandler::CompleteNotifyProfileChanged.  R99/R4 QoS Parameters in use");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_COMPLETENOTIFYPROFILECHANGED, "CMmPacketQoSMessHandler::CompleteNotifyProfileChanged, R99/R4 QoS Parameters in use" );
        }
     //no else

    if ( completeToClient )
        {
        if ( KErrNone == ret )
            {
            // Pack Qos params
            data.SetProfileChanged( &iQoSGPRSNegotiated, 
                &iQoSR99_R4Negotiated, &iQoSR5_Negotiated );
            // Pack completion params
            data.PackData( &mmPacketContextName );
            iMessageRouter->Complete( 
                EPacketQoSNotifyProfileChanged,
                &data, 
                KErrNone );
            }
        //no else
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::MapQoSInfoToClient
// Maps parameters to client.
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::MapQoSInfoToClient(
    const TQoSInfo& aQoSInfo,
    RPacketQoS::TQoSPrecedence& aPrecedence,
    RPacketQoS::TQoSDelay& aDelay,
    RPacketQoS::TQoSReliability& aReliability,
    RPacketQoS::TQoSPeakThroughput& aPeakThroughput,
    RPacketQoS::TQoSMeanThroughput& aMeanThroughput )
    {

    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToClient.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoSInfoToClient" );

    switch ( aQoSInfo.iPrecedence )
        {
        // case GPDS_QOS_PREC_0 is default value
       
        case GPDS_QOS_PREC_1:
            {
            aPrecedence = RPacketQoS::EPriorityHighPrecedence;
            break;
            }
        case GPDS_QOS_PREC_2:
            {
            aPrecedence = RPacketQoS::EPriorityMediumPrecedence;
            break;
            }
        case GPDS_QOS_PREC_3:
            {
            aPrecedence = RPacketQoS::EPriorityLowPrecedence;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToClient. Switch Precedence case default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoSInfoToClient, Switch Precedence case default" );

            aPrecedence = RPacketQoS::EUnspecifiedPrecedence;
            break;
            }
        }

    switch ( aQoSInfo.iDelay )
        {
        // case GPDS_QOS_DELC_0 is default value
        
        case GPDS_QOS_DELC_1:
            {
            aDelay = RPacketQoS::EDelayClass1;
            break;
            }
        case GPDS_QOS_DELC_2:
            {
            aDelay = RPacketQoS::EDelayClass2;
            break;
            }
        case GPDS_QOS_DELC_3:
            {
            aDelay = RPacketQoS::EDelayClass3;
            break;
            }
        case GPDS_QOS_DELC_4:
            {
            aDelay = RPacketQoS::EDelayClass4;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToClient. Switch Delay case default");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoSInfoToClient, Switch Delay case default" );

            aDelay = RPacketQoS::EUnspecifiedDelayClass;
            break;
            }
        }

    switch ( aQoSInfo.iReliability )
        {
        // case GPDS_QOS_RELC_0 is default value
        
        case GPDS_QOS_RELC_1:
            {
            aReliability = RPacketQoS::EReliabilityClass1;
            break;
            }
        case GPDS_QOS_RELC_2:
            {
            aReliability = RPacketQoS::EReliabilityClass2;
            break;
            }
        case GPDS_QOS_RELC_3:
            {
            aReliability = RPacketQoS::EReliabilityClass3;
            break;
            }
        case GPDS_QOS_RELC_4:
            {
            aReliability = RPacketQoS::EReliabilityClass4;
            break;
            }
        case GPDS_QOS_RELC_5:
            {
            aReliability = RPacketQoS::EReliabilityClass5;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToClient. Switch Reliability case default");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoSInfoToClient, Switch Reliability case default" );

            aReliability = RPacketQoS::EUnspecifiedReliabilityClass;
            break;
            }
        }

    switch ( aQoSInfo.iPeakThroughput )
        {
        // case GPDS_QOS_PELC_0 is default value

        case GPDS_QOS_PETC_1:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput1000;
            break;
            }
        case GPDS_QOS_PETC_2:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput2000;
            break;
            }
        case GPDS_QOS_PETC_3:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput4000;
            break;
            }
        case GPDS_QOS_PETC_4:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput8000;
            break;
            }
        case GPDS_QOS_PETC_5:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput16000;
            break;
            }
        case GPDS_QOS_PETC_6:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput32000;
            break;
            }
        case GPDS_QOS_PETC_7:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput64000;
            break;
            }
        case GPDS_QOS_PETC_8:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput128000;
            break;
            }
        case GPDS_QOS_PETC_9:
            {
            aPeakThroughput = RPacketQoS::EPeakThroughput256000;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToClient. Switch PeakThroughput case default");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoSInfoToClient, Switch PeakThroughput case default" );

            aPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
            break;
            }
        }

    switch ( aQoSInfo.iMeanThroughput )
        {
        // case GPDS_QOS_METC_0 is default value

        case GPDS_QOS_METC_1:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput100;
            break;
            }
        case GPDS_QOS_METC_2:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput200;
            break;
            }
        case GPDS_QOS_METC_3:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput500;
            break;
            }
        case GPDS_QOS_METC_4:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput1000;
            break;
            }
        case GPDS_QOS_METC_5:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput2000;
            break;
            }
        case GPDS_QOS_METC_6:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput5000;
            break;
            }
        case GPDS_QOS_METC_7:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput10000;
            break;
            }
        case GPDS_QOS_METC_8:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput20000;
            break;
            }
        case GPDS_QOS_METC_9:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput50000;
            break;
            }
        case GPDS_QOS_METC_10:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput100000;
            break;
            }
        case GPDS_QOS_METC_11:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput200000;
            break;
            }
        case GPDS_QOS_METC_12:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput500000;
            break;
            }
        case GPDS_QOS_METC_13:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput1000000;
            break;
            }
        case GPDS_QOS_METC_14:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput2000000;
            break;
            }
        case GPDS_QOS_METC_15:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput5000000;
            break;
            }
        case GPDS_QOS_METC_16:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput10000000;
            break;
            }
        case GPDS_QOS_METC_17:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput20000000;
            break;
            }
        case GPDS_QOS_METC_18:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughput50000000;
            break;
            }
        case GPDS_QOS_METC_31:
            {
            aMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoSInfoToClient. Switch MeanThroughput case default");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETQOSMESSHANDLER_MAPQOSINFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoSInfoToClient, Switch MeanThroughput case default" );

            aMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketQoSMessHandler::MapQoS99InfoToClient
// Maps QoS R99 parameters to client.
// -----------------------------------------------------------------------------
//
void CMmPacketQoSMessHandler::MapQoS99InfoToClient(
    const TQoS99Info& aQoS99Info,       // Server side QoS R99 info packet
    RPacketQoS::TQoSR99_R4Negotiated& aQoS99Negotiated,
    RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated ) // Client side QoS R99 info packet
    {
    
    TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient.");
OstTrace0( TRACE_NORMAL, CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient" );

    switch ( aQoS99Info.iTrafficClass )
        {
        //case GPDS_QOS99_TRAC_SUBSCRIBED is defauld value
        
        case GPDS_QOS99_TRAC_CONVERSATIONAL:
            {
            aQoSR5Negotiated.iTrafficClass
                = RPacketQoS::ETrafficClassConversational;
            break;
            }
        case GPDS_QOS99_TRAC_STREAMING:
            {
            aQoSR5Negotiated.iTrafficClass
                = RPacketQoS::ETrafficClassStreaming;
            break;
            }
        case GPDS_QOS99_TRAC_INTERACTIVE:
            {
            aQoSR5Negotiated.iTrafficClass
                = RPacketQoS::ETrafficClassInteractive;
            break;
            }
        case GPDS_QOS99_TRAC_BACKGROUND:
            {
            aQoSR5Negotiated.iTrafficClass
                = RPacketQoS::ETrafficClassBackground;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient. Switch TrafficClass case default");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient, Switch TrafficClass case default" );

            aQoSR5Negotiated.iTrafficClass
                = RPacketQoS::ETrafficClassUnspecified;
            break;
            }
        }

    switch ( aQoS99Info.iDeliveryOrderReqd )
        {
        //case GPDS_QOS99_DELO_SUBSCRIBED is defauld value:

        case GPDS_QOS99_DELO_YES:
            {
            aQoSR5Negotiated.iDeliveryOrderReqd
                = RPacketQoS::EDeliveryOrderRequired;
            break;
            }
        case GPDS_QOS99_DELO_NO:
            {
            aQoSR5Negotiated.iDeliveryOrderReqd
                = RPacketQoS::EDeliveryOrderNotRequired;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient. Switch DeliveryOrderReqd case default");
OstTrace0( TRACE_NORMAL, DUP2_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient, Switch DeliveryOrderReqd case default" );

            aQoSR5Negotiated.iDeliveryOrderReqd
                = RPacketQoS::EDeliveryOrderUnspecified;
            break;
            }
        }

    switch ( aQoS99Info.iDeliverErroneousSDU )
        {
        //case GPDS_QOS99_DOES_SUBSCRIBED is defauld value:

        case GPDS_QOS99_DOES_NO_DETECT:
            {
            aQoSR5Negotiated.iDeliverErroneousSDU
                = RPacketQoS::EErroneousSDUNoDetection;
            break;
            }
        case GPDS_QOS99_DOES_YES:
            {
            aQoSR5Negotiated.iDeliverErroneousSDU
                = RPacketQoS::EErroneousSDUDeliveryRequired;
            break;
            }
        case GPDS_QOS99_DOES_NO:
            {
            aQoSR5Negotiated.iDeliverErroneousSDU
                = RPacketQoS::EErroneousSDUDeliveryNotRequired;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient. Switch DeliverErroneousSDU case default");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient, Switch DeliverErroneousSDU case default" );

            aQoSR5Negotiated.iDeliverErroneousSDU
                = RPacketQoS::EErroneousSDUDeliveryUnspecified;
            break;
            }
        }

    switch ( aQoS99Info.iBER )
        {
        //case GPDS_QOS99_RBER_SUBSCRIBED is defauld value:

        case GPDS_QOS99_RBER_1:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBERFivePerHundred;
            break;
            }
        case GPDS_QOS99_RBER_2:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBEROnePerHundred;
            break;
            }
        case GPDS_QOS99_RBER_3:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBERFivePerThousand;
            break;
            }
        case GPDS_QOS99_RBER_4:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBERFourPerThousand;
            break;
            }
        case GPDS_QOS99_RBER_5:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBEROnePerThousand;
            break;
            }
        case GPDS_QOS99_RBER_6:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBEROnePerTenThousand;
            break;
            }
        case GPDS_QOS99_RBER_7:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBEROnePerHundredThousand;
            break;
            }
        case GPDS_QOS99_RBER_8:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBEROnePerMillion;
            break;
            }
        case GPDS_QOS99_RBER_9:
            {
            aQoSR5Negotiated.iBER
                = RPacketQoS::EBERSixPerHundredMillion;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient. Switch BER case default");
OstTrace0( TRACE_NORMAL, DUP4_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient, Switch BER case default" );

            aQoSR5Negotiated.iBER
                = RPacketQoS::EBERUnspecified;
            break;
            }
        }

    switch ( aQoS99Info.iSDUErrorRatio )
        {
        //case GPDS_QOS99_SDER_SUBSCRIBED is defauld value:

        case GPDS_QOS99_SDER_7:
            {
            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioOnePerTen;
            break;
            }
        case GPDS_QOS99_SDER_1:
            {
            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioOnePerHundred;
            break;
            }
        case GPDS_QOS99_SDER_2:
            {
            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioSevenPerThousand;
            break;
            }
        case GPDS_QOS99_SDER_3:
            {
            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioOnePerThousand;
            break;
            }
        case GPDS_QOS99_SDER_4:
            {
            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
            break;
            }
        case GPDS_QOS99_SDER_5:
            {
            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioOnePerHundredThousand;
            break;
            }
        case GPDS_QOS99_SDER_6:
            {
            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioOnePerMillion;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient. Switch SDUErrorRatio case default");
OstTrace0( TRACE_NORMAL, DUP5_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient, Switch SDUErrorRatio case default" );

            aQoSR5Negotiated.iSDUErrorRatio
                = RPacketQoS::ESDUErrorRatioUnspecified;
            break;
            }
        }

    switch ( aQoS99Info.iTrafficHandlingPriority )
        {
        //case GPDS_QOS99_TRHP_SUBSCRIBED is defauld value:

        case GPDS_QOS99_TRHP_1:
            {
            aQoSR5Negotiated.iTrafficHandlingPriority
                = RPacketQoS::ETrafficPriority1;
            break;
            }
        case GPDS_QOS99_TRHP_2:
            {
            aQoSR5Negotiated.iTrafficHandlingPriority
                = RPacketQoS::ETrafficPriority2;
            break;
            }
        case GPDS_QOS99_TRHP_3:
            {
            aQoSR5Negotiated.iTrafficHandlingPriority
                = RPacketQoS::ETrafficPriority3;
            break;
            }
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient. Switch TrafficHandlingPriority case default");
OstTrace0( TRACE_NORMAL, DUP6_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient, Switch TrafficHandlingPriority case default" );

            aQoSR5Negotiated.iTrafficHandlingPriority
                = RPacketQoS::ETrafficPriorityUnspecified;
            break;
            }
        }

    aQoSR5Negotiated.iTransferDelay = aQoS99Info.iTransferDelay;
    aQoSR5Negotiated.iMaxSDUSize = aQoS99Info.iMaxSDUSize;
    aQoSR5Negotiated.iMaxRate.iUplinkRate = aQoS99Info.iMaxUplinkRate;
    aQoSR5Negotiated.iMaxRate.iDownlinkRate = aQoS99Info.iMaxDownlinkRate;
    aQoSR5Negotiated.iGuaranteedRate.iUplinkRate
        = aQoS99Info.iGuaranteedUplinkRate;
    aQoSR5Negotiated.iGuaranteedRate.iDownlinkRate
        = aQoS99Info.iGuaranteedDownlinkRate;

    aQoS99Negotiated = aQoSR5Negotiated;
    aQoSR5Negotiated.iSignallingIndication =
        aQoS99Info.iSignallingIndication;

    switch ( aQoS99Info.iSourceStatisticsDescriptor )
        {
        case GPDS_SOURCE_UNKNOWN:
            {
            aQoSR5Negotiated.iSourceStatisticsDescriptor
                = RPacketQoS::ESourceStatisticsDescriptorUnknown;
            break;
            }
        case GPDS_SOURCE_SPEECH:
            {
            aQoSR5Negotiated.iSourceStatisticsDescriptor
                = RPacketQoS::ESourceStatisticsDescriptorSpeech;
            break;
            }
        case GPDS_SOURCE_DEFAULT:
        default:
            {

            TFLOGSTRING("TSY: CMmPacketQoSMessHandler::MapQoS99InfoToClient. Switch SourceStatisticsDescriptor case default");
OstTrace0( TRACE_NORMAL, DUP7_CMMPACKETQOSMESSHANDLER_MAPQOS99INFOTOCLIENT, "CMmPacketQoSMessHandler::MapQoS99InfoToClient, Switch SourceStatisticsDescriptor case default" );

            aQoSR5Negotiated.iSourceStatisticsDescriptor
                = RPacketQoS::ESourceStatisticsDescriptorUnknown;
            break;
            }
        }
    }

//=============================================================================

// OTHER EXPORTED FUNCTIONS

    // None

// End of File
