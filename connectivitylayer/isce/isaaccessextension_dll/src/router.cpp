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
* Description: 
*
*/


#include <phonetisi.h>                  // For ISI_HEADER_SIZE
#include <pn_const.h>                   // For PN_HEADER_SIZE
#include <pipeisi.h>                    // For PNS_PIPE_DATA_OFFSET_DATA
#ifndef NCP_COMMON_BRIDGE_FAMILY
#include <mediaisi.h>                   // For PNS_MEDIA_SPECIFIC_REQ/RESP
#include <nsisi.h>                      // For PNS_NAME...
#endif // NCP_COMMON_BRIDGE_FAMILY
#include <commisi.h>                    // For SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP
#include "router.h"
#include "iadtrace.h"                   // For C_TRACE..
#include "isaaccessextension.h"         // For DIsaAccessExtension
#include "indicationhandler.h"          // For DIndicationHandler
#include "queue.h"                      // For DQueue
#include "iadinternaldefinitions.h"     // For EIADAsync...
#include "iadhelpers.h"                 // For GET_RECEIVER

#include "pipehandler.h"                // For PipeHandler
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "routerTraces.h"
#endif

//#define MODEM_MCE_DOES_NOT_WORK_AS_IT_SHOULD // TODO: to be removed when Bridge Modem SW is ok

// ISCE
#include "memapi.h"                     // For MemApi
#include "trxdefs.h"                    // For ETrx...
// ISCE
// ISCE #include "miad2istapi.h" // For MIAD2ISTApi



// CONSTS
#ifndef NCP_COMMON_BRIDGE_FAMILY

// TODO : spec this to some document
/**** IAD<->SOS_MM ****/
const TUint8 KIADObjId( 0xfe );
/* CTRL CONF REQ START *
[0]     media,          PN_MEDIA_SOS;
[1]     recdev,         PN_MEDIA_ROUTING_REQ
[2]     senderdev,      PN_DEV_SOS
[3]     function,       PN_MEDIA_CONTROL
[4,5]   length,         8
[6]     recobj,         PN_OBJ_ROUTER
[7]     senderobj,      KIADObjId
[8]     transaction     0x00
[9]     messageid       PNS_MEDIA_SPECIFIC_REQ
[10]    media2control   PN_MEDIA_SOS
[11]    ctrl msg id     KIADCtrlConfReq (0x05)
[12]    filler          0x00
[13]    filler          0x00
*  CTRL CONF REQ END */
const TUint8 KIADCtrlConfReq( 0x5 );
/* CTRL CONF RESP START *
[0]     media,          PN_MEDIA_SOS;
[1]     recdev,         PN_DEV_SOS
[2]     senderdev,      PN_MEDIA_ROUTING_REQ
[3]     function,       PN_MEDIA_CONTROL
[4,5]   length,         10
[6]     recobj,         KIADObjId
[7]     senderobj,      PN_OBJ_ROUTER
[8]     transaction     0x00
[9]     messageid       PNS_MEDIA_SPECIFIC_REQ
[10]    media2control   PN_MEDIA_SOS
[11]    media error     PN_MCTRL_NO_ERROR
[12]    ctrl msg id     KIADCtrlConfResp (0x06)
[13]    conf size       8 bytes
[14]    conf size       8 bytes
[15]    filler          0x00
*  CTRL CONF RESP END */
const TUint8 KIADCtrlConfResp( 0x6 );
/* CTRL DRM REQ START *
[0]     media,          PN_MEDIA_SOS;
[1]     recdev,         PN_MEDIA_ROUTING_REQ
[2]     senderdev,      PN_DEV_SOS
[3]     function,       PN_MEDIA_CONTROL
[4,5]   length,         8 ( + 6 = KIADCtrlDrmReqLength )
[6]     recobj,         PN_OBJ_ROUTER
[7]     senderobj,      KIADObjId
[8]     channel number  IAD client's channel number
[9]     messageid       PNS_MEDIA_SPECIFIC_REQ
[10]    mediatocontrol  PN_MEDIA_SOS
[11]    ctrl msg id     KIADCtrlDrmReq (0x07)
[12]    ch is 2 be reg  IAD client's channel number
[13]    filler          0x00
*  CTRL DRM REQ END */
const TUint8 KIADCtrlDrmReq( 0x7 );
const TInt KIADCtrlDrmReqLength( 14 );
/* CTRL DRM RESP START *
[0]     media,          PN_MEDIA_SOS;
[1]     recdev,         PN_DEV_SOS
[2]     senderdev,      PN_MEDIA_ROUTING_REQ
[3]     function,       PN_MEDIA_CONTROL
[4,5]   length,         8
[6]     recobj,         KIADObjId
[7]     senderobj,      PN_OBJ_ROUTER
[8]     channel number  IAD client's channel number
[9]     messageid       PNS_MEDIA_SPECIFIC_RESP
[10]    mediatocontrol  PN_MEDIA_SOS
[11]    media error     PN_MCTRL_NO_ERROR
[12]    ctrl msg id     KIADCtrlDrmResp (0x08)
[13]    filler          0x00
* CTRL DRM REQ END */
const TUint8 KIADCtrlDrmResp( 0x8 );

#define CTRL_REQ_OFFSET_MESSAGEID ( ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_REQ_OFFSET_DATA )
#define CTRL_RESP_OFFSET_MESSAGEID ( ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_RESP_OFFSET_DATA )
#define CTRL_CONF_RESP_OFFSET_SIZE_BYTE1 ( CTRL_RESP_OFFSET_MESSAGEID + 1 )
#define CTRL_CONF_RESP_OFFSET_SIZE_BYTE2 ( CTRL_CONF_RESP_OFFSET_SIZE_BYTE1 + 1 )
#define CTR_DRM_REQ_RESP_OFFSET_CHANNEL_ID ( ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_RESP_OFFSET_UTID )
#define CTRL_DRM_REQ_OFFSET_REQUIRED_CHANNEL_ID ( CTRL_REQ_OFFSET_MESSAGEID + 1 )
#endif // NCP_COMMON_BRIDGE_FAMILY

// TODO: change this to use UnuqueID instead and to extension..
void DRouter::CheckDfc()
{
    OstTrace0( TRACE_NORMAL, DROUTER_CHECKDFC_ENTRY, ">DRouter::CheckDfc" );

    DObject* tempObj = reinterpret_cast<DObject*>( &Kern::CurrentThread() );
    TUint8* buffer = ( TUint8* )Kern::Alloc( 100 );
    TPtr8* bufferPtr = new ( TPtr8 )( buffer, 100 );
    tempObj->Name( *bufferPtr );
    C_TRACE( ( _T( "DRouter::CheckDfc" ) ) );
    if ( bufferPtr->Compare( KIADExtensionDfc ) != KErrNone ) 
        {
        for( TInt i( 0 );i < bufferPtr->Length(); ++i )
           {
           Kern::Printf( "%c", i, bufferPtr->Ptr()[ i ]);
           }
        ASSERT_RESET_ALWAYS( 0, EIADWrongDFCQueueUsed | EIADFaultIdentifier2 << KFaultIdentifierShift  );
        }
    Kern::Free( buffer );
    delete bufferPtr;
    bufferPtr = NULL;
    
    OstTrace0( TRACE_NORMAL, DROUTER_CHECKDFC_EXIT, "<DRouter::CheckDfc" );
}

#ifdef _DEBUG
#define    ASSERT_DFCTHREAD_INEXT() CheckDfc()
#else
#define    ASSERT_DFCTHREAD_INEXT()
#endif

DRouter::DRouter():
    iConnectionStatus( EIADConnectionNotOk )
    , iBootDone( EFalse )
    {
    OstTrace0( TRACE_NORMAL, DROUTER_DROUTER_ENTRY, "<DRouter::DRouter" );
    
    C_TRACE( ( _T( "DRouter::DRouter ->" ) ) );
    // owned
#ifndef NCP_COMMON_BRIDGE_FAMILY
    iPipeHandler = new DPipeHandler( *this );
#endif
    iIndicationHandler = new DIndicationHandler( *this );
    iCommonRxQueue = new DQueue( KIADExtensionRxQueueSize );
    ASSERT_RESET_ALWAYS( /*iPipeHandler &&*/ iIndicationHandler && iCommonRxQueue, EIADMemoryAllocationFailure | EIADFaultIdentifier17 << KFaultIdentifierShift );
    // Initialize channels to NULL when channel is opened !NULL.
    for( TInt i( 0 ); i < EIADSizeOfChannels; ++i )
        {
        iChannelTable[ i ].iChannel = NULL;
        iChannelTable[ i ].iWaitingChannel = NULL;
        iChannelTable[ i ].iType = ENormalOpen;
        }

    iCommonRxDfc = new TDfc( CommonRxDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADExtCommonRxPriori );
    // not owned, just using.
    // Set null when not registered.
    iInitCmtDfc = new TDfc( InitCmtDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADExtInitCmtPriori );
    iConnStatDfc = new TDfc( NotifyObjLayerConnStatDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADExtConnStatPriori );
    ASSERT_RESET_ALWAYS( iCommonRxDfc && iInitCmtDfc && iConnStatDfc, EIADMemoryAllocationFailure | EIADFaultIdentifier18 << KFaultIdentifierShift );
    iMaxFrameSize = 0x0000;
    // ISCE
    iLinksArray = new MISIRouterLinkIf*[ DRouter::EISIAmountOfMedias ];
    // TODO: fault codes and ids
    ASSERT_RESET_ALWAYS( iLinksArray, ( EIADMemoryAllocationFailure | EIADFaultIdentifier17 << KFaultIdentifierShift ) );
    // Initialize links
    for( TInt i( 0 ); i < DRouter::EISIAmountOfMedias; i++ )
        {
        iLinksArray[ i ] = NULL;
        C_TRACE( ( _T( "DRouter::DRouter %d" ), i ) );
        }
    // Configuration of ISI links. TODO: devices and link configurations for coming platforms are unknown and to be done later.
#ifndef NCP_COMMON_BRIDGE_FAMILY
    iLinksArray[ DRouter::EISIMediaSOS ] = MISIRouterLinkIf::CreateLinkF( this, PN_MEDIA_SOS, ETrxPartnerOS );
#else
    // Bridge media = PN_MEDIA_MODEM_HOST_IF
    iLinksArray[ DRouter::EISIMediaHostSSI ] = MISIRouterLinkIf::CreateLinkF( this, PN_MEDIA_MODEM_HOST_IF, ETrxSharedMemory );
#endif // NCP_COMMON_BRIDGE_FAMILY
    // ISCE
    C_TRACE( ( _T( "DRouter::DRouter 0x%x <-" ), this ) );
    OstTrace1( TRACE_NORMAL, DROUTER_DROUTER_EXIT, "<DRouter::DRouter;this=%x", this );
    }
    
DRouter::~DRouter(
        // None
       )
    {
    OstTrace0( TRACE_NORMAL, DUP1_DROUTER_DROUTER_ENTRY, "<DRouter::~DRouter" );
//ISCE
    // owning so deleting
    for( TInt i( 0 ); i < EISIAmountOfMedias; i++ )
        {
        MISIRouterLinkIf* temp = iLinksArray[ i ];
        temp->Release();
        temp = NULL;
        iLinksArray[ i ] = NULL;
        }
//ISCE
    C_TRACE( ( _T( "DRouter::~DRouter 0x%x ->" ), this ) );
    // Initialize channels to NULL when channel is opened !NULL.
    for( TInt i( 0 ); i < EIADSizeOfChannels; ++i )
        {
        iChannelTable[ i ].iChannel = NULL;
        iChannelTable[ i ].iWaitingChannel = NULL;
        iChannelTable[ i ].iType = ENormalOpen;
        }
#ifndef NCP_COMMON_BRIDGE_FAMILY
    if( iPipeHandler )
        {
        delete iPipeHandler;
        iPipeHandler = NULL;
        }
#endif
    if( iIndicationHandler )
        {
        delete iIndicationHandler;
        iIndicationHandler = NULL;
        }
    if( iCommonRxQueue )
        {
        delete iCommonRxQueue;
        iCommonRxQueue = NULL;
        }
    if( iCommonRxDfc )
        {
        iCommonRxDfc->Cancel();
        delete iCommonRxDfc;
        iCommonRxDfc = NULL;
        }
    if( iInitCmtDfc )
        {
        delete iInitCmtDfc;
        iInitCmtDfc = NULL;
        }
    if( iConnStatDfc )
        {
        delete iConnStatDfc;
        iConnStatDfc = NULL;
        }
    C_TRACE( ( _T( "DRouter::~DRouter 0x%x <-" ), this ) );

    OstTrace1( TRACE_NORMAL, DUP1_DROUTER_DROUTER_EXIT, "<DRouter::~DRouter;this=%x", this );
    }

// From   start
// Do not call from ISR context!! Can only be called from Extension DFCThread context.
EXPORT_C TDes8& DRouter::AllocateBlock(
        const TUint16 aSize
        )
    {
    OstTraceExt1( TRACE_NORMAL, DROUTER_ALLOCATEBLOCK_ENTRY, "<>DRouter::AllocateBlock;aSize=%hu", aSize );

    C_TRACE( ( _T( "DRouter::AllocateBlock %d <->" ), aSize ) );
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    ASSERT_DFCTHREAD_INEXT();
    ASSERT_RESET_ALWAYS( aSize, EIADWrongParameter | EIADFaultIdentifier9 << KFaultIdentifierShift );
// ISCE
//    ASSERT_RESET_ALWAYS( iIST, EIADNullParameter | EIADFaultIdentifier17 << KFaultIdentifierShift  );
//    return iIST->AllocateBlock( aSize );// TODO: Print ptr!!!
    return MemApi::AllocBlock( aSize );
// ISCE
    }

EXPORT_C TDes8& DRouter::AllocateDataBlock(
        const TUint16 aSize
        )
    {
    OstTraceExt1( TRACE_NORMAL, DROUTER_ALLOCATEDATABLOCK_ENTRY, ">DRouter::AllocateDataBlock;aSize=%hu", aSize );

    C_TRACE( ( _T( "DRouter::AllocateDataBlock %d <->" ), aSize ) );
    TUint32 neededLength( aSize + ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_DATA );
    TDes8& tmp = this->AllocateBlock( neededLength );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    tmp.SetLength( neededLength );
    TUint8* msgPtr = const_cast<TUint8*>( tmp.Ptr() );
    SET_RECEIVER_DEV( msgPtr, PN_DEV_DONT_CARE );    
    ASSERT_RESET_ALWAYS( neededLength > ISI_HEADER_OFFSET_MESSAGEID, EIADOverTheLimits | EIADFaultIdentifier1 << KFaultIdentifierShift  );
    msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_PIPE;
    msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] = PNS_PIPE_DATA;
    SET_RECEIVER_OBJ( msgPtr, PN_OBJ_ROUTER );
    SET_LENGTH( tmp, ( tmp.Length() - PN_HEADER_SIZE ) );
#endif
    OstTrace1( TRACE_NORMAL, DROUTER_ALLOCATEDATABLOCK_EXIT, "<DRouter::AllocateDataBlock;tmp=%x",( TUint )&( tmp ) );    
    return tmp;
    }


EXPORT_C void DRouter::Close(
        const TUint16 aChannelId
        )
    {

    OstTraceExt1( TRACE_NORMAL, DROUTER_CLOSE_ENTRY, ">DRouter::Close;aChannelId=%hx", aChannelId );
    C_TRACE( ( _T( "DRouter::Close 0x%x ->" ), aChannelId ) );
    // Channel must be from appropiate length and although it is sixteenbit value it must contain only 8-bit values. If over 8-bit changes needed.
    ASSERT_RESET_ALWAYS( aChannelId < EIADSizeOfChannels || aChannelId < 0xff, EIADWrongParameter | EIADFaultIdentifier10 << KFaultIdentifierShift );
    ASSERT_DFCTHREAD_INEXT();
    // If channel open (!NULL) set as closed (NULL) or if channel open is pending.
    ASSERT_RESET_ALWAYS( aChannelId < EIADSizeOfChannels, EIADWrongParameter | (TUint8)aChannelId << KChannelNumberShift | EIADFaultIdentifier21 << KFaultIdentifierShift );
    if( iChannelTable[ aChannelId ].iChannel || iChannelTable[ aChannelId ].iWaitingChannel )
        {
        C_TRACE( ( _T( "DRouter::Close open->close 0x%x" ), aChannelId ) );
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
        if( iChannelTable[ aChannelId ].iType != ELoan )
            {
#endif
            iChannelTable[ aChannelId ].iChannel = NULL;
            iChannelTable[ aChannelId ].iWaitingChannel = NULL;
            iChannelTable[ aChannelId ].iType = ENormalOpen;
            const TInt indicationCancelOrderSize( 2 );
            TDes8& cancelOrder = AllocateBlock( indicationCancelOrderSize );
            cancelOrder.Append( 0x00 );
            cancelOrder.Append( 0x00 );
            // Order internally, so no return values above ::Close { 0x00, 0x00 } 8-bit cancel indication.
            TInt error( OrderIndication( cancelOrder, aChannelId, EFalse ) );
            C_TRACE( ( _T( "DRouter::Close open->close indication order returned %d 0x%x" ), error, aChannelId ) );
            OstTraceExt2( TRACE_NORMAL, DROUTER_CLOSE, "DRouter::Close open->close indication order returned;error=%d;aChannelId=%x", error, aChannelId );
            ASSERT_RESET_ALWAYS( KErrNone == error, EIADIndicationOrderFailed | static_cast<TUint8>( ( aChannelId << KChannelNumberShift ) ) );
            DeAllocateBlock( cancelOrder );
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
            }
        else
            {
            C_TRACE( ( _T( "DRouter::Close open->close loaned channel 0x%x" ), aChannelId ) );
            OstTrace1( TRACE_NORMAL, DROUTER_CLOSE_2, "DRouter::Close open->close loaned channel;aChannelId=%x", aChannelId );
            iChannelTable[ aChannelId ].iType = ENormalOpen;
            iChannelTable[ aChannelId ].iChannel = iChannelTable[ aChannelId ].iWaitingChannel;
            iChannelTable[ aChannelId ].iWaitingChannel = NULL;
            }
#endif
        }
    C_TRACE( ( _T( "DRouter::Close 0x%x <-" ), aChannelId ) );
    OstTrace0( TRACE_NORMAL, DROUTER_CLOSE_EXIT, "<DRouter::Close" );

    }

// Do not call from ISR context!! Can only be called from Extension DFCThread context.
EXPORT_C void DRouter::DeAllocateBlock(
        TDes8& aBlock
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_DEALLOCATEBLOCK_ENTRY, ">DRouter::DeAllocateBlock;aBlock=%x", ( TUint )&( aBlock ) );

    C_TRACE( ( _T( "DRouter::DeAllocateBlock 0x%x ->" ), &aBlock ) );
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    ASSERT_DFCTHREAD_INEXT();
// ISCE
//    ASSERT_RESET_ALWAYS( iIST, EIADNullParameter | EIADFaultIdentifier18 << KFaultIdentifierShift  );
//    iIST->DeallocateBlock( aBlock );
    MemApi::DeallocBlock( aBlock );
// ISCE
    C_TRACE( ( _T( "DRouter::DeAllocateBlock 0x%x <-" ), &aBlock ) );

    OstTrace0( TRACE_NORMAL, DROUTER_DEALLOCATEBLOCK_EXIT, "<DRouter::DeAllocateBlock" );    
    }

EXPORT_C TInt DRouter::GetConnectionStatus(
            // None
            )
    {
    OstTrace0( TRACE_NORMAL, DROUTER_GETCONNECTIONSTATUS_ENTRY, "<DRouter::GetConnectionStatus" );

    C_TRACE( ( _T( "DRouter::GetConnectionStatus %d <->" ), iConnectionStatus ) );
    ASSERT_DFCTHREAD_INEXT();
    
    OstTrace1( TRACE_NORMAL, DROUTER_GETCONNECTIONSTATUS, "<DRouter::GetConnectionStatus;iConnectionStatus=%x", iConnectionStatus );    
    return iConnectionStatus;
    }

EXPORT_C TInt DRouter::GetFlowControlStatus()
{
    OstTrace0( TRACE_NORMAL, DROUTER_GETFLOWCONTROLSTATUS_ENTRY, "<>DRouter::GetFlowControlStatus" );
    ASSERT_DFCTHREAD_INEXT();
    return KErrNotSupported; // TODO
}

EXPORT_C TInt DRouter::GetMaxDataSize(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DROUTER_GETMAXDATASIZE_ENTRY, ">DRouter::GetMaxDataSize" );

    C_TRACE( ( _T( "DRouter::GetMaxDataSize ->" ) ) );
    ASSERT_DFCTHREAD_INEXT();
    TInt excludeSize( iMaxFrameSize - ( ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_DATA ) );
    // Set the data to be divisible by four.
    TInt allowedCount( excludeSize - ( excludeSize % 4 ) );
    C_TRACE( ( _T( "DRouter::GetMaxDataSize %d <-" ), allowedCount ) );

    OstTrace1( TRACE_NORMAL, DROUTER_GETMAXDATASIZE_EXIT, "<DRouter::GetMaxDataSize;allowedCount=%d", allowedCount );
    return allowedCount;
    }

// KErrNone, channelid with channel object open ok
// KErrNotSupported, resource not supported or in use causes name_add_req to fail
// KErrInUse, channelid opened by same channel object earlier
// KErrAlreadyExists, channel opened by some other channel object earlier
// In completechannelreq, set ~channel number  as ~~channelnumber only if KErrNone or KErrInUse
EXPORT_C void DRouter::Open(
        const TUint16 aChannel,
        const TUint16 aRequest,
        MIAD2ChannelApi* aCallback
        )
    {
    OstTraceExt3( TRACE_NORMAL, DROUTER_OPEN_ENTRY, ">DRouter::Open;aChannel=%hx;aRequest=%hu;aCallback=%x", aChannel, aRequest, ( TUint )( aCallback ) );
    

    C_TRACE( ( _T( "DRouter::Open 0x%x %d 0x%x ->" ), aChannel, aRequest, aCallback ) );
    ASSERT_RESET_ALWAYS( aCallback, EIADNullParameter | EIADFaultIdentifier19 << KFaultIdentifierShift  );
    ASSERT_DFCTHREAD_INEXT();
    ASSERT_RESET_ALWAYS( aChannel < EIADSizeOfChannels, EIADWrongParameter | (TUint8)aChannel << KChannelNumberShift | EIADFaultIdentifier26 << KFaultIdentifierShift );
    if( iConnectionStatus == EIADConnectionOk )
        {
        // If not null channel is allready open.
        if( iChannelTable[ aChannel ].iChannel || 
            iChannelTable[ aChannel ].iWaitingChannel )
            {
            // If another channel tries to open already open channel.
            TRACE_WARNING( iChannelTable[ aChannel ].iChannel == aCallback, (TUint8)aChannel << KChannelNumberShift );
            // If same object calling KErrInUser if other but same channel KErrAlreadyExists
            aCallback->CompleteChannelRequest( aRequest, ( iChannelTable[ aChannel ].iChannel == aCallback ? KErrInUse : KErrAlreadyExists ) );
            }
        // Null so channel is not open, set !null to mark opened channel.
        else
            {
#ifndef NCP_COMMON_BRIDGE_FAMILY
            const TBool drmChannel( ( aChannel == EIADNokiaDRM || aChannel == EIADNokiaSecurityDriver ) ? ETrue : EFalse );
            if( drmChannel )
                {
                C_TRACE( ( _T( "DRouter::Open channel 0x%x drm 0x%x" ), aChannel, aCallback ) );
                OstTraceExt1( TRACE_NORMAL, DROUTER_OPEN, "DRouter::Open DRM;aChannel=%hx", aChannel );
                iChannelTable[ aChannel ].iWaitingChannel = aCallback;
                iChannelTable[ aChannel ].iType = EDrmOpen;
                SendDrmReq( aChannel );
                }
            else
                {
#endif // NCP_COMMON_BRIDGE_FAMILY
                C_TRACE( ( _T( "DRouter::Open channel 0x%x normal 0x%x" ), aChannel, aCallback ) );
                OstTraceExt1( TRACE_NORMAL, DUP1_DROUTER_OPEN, "DRouter::Open normal;aChannel=%hx", aChannel );
                iChannelTable[ aChannel ].iChannel = aCallback;
                aCallback->CompleteChannelRequest( aRequest, KErrNone );
#ifndef NCP_COMMON_BRIDGE_FAMILY
                }
#endif // NCP_COMMON_BRIDGE_FAMILY
            }
        }
    else
        {
        C_TRACE( ( _T( "DRouter::Open not ready" ) ) );
        OstTrace0( TRACE_NORMAL, DUP2_DROUTER_OPEN, "DRouter::Open Not ready" );        
        ASSERT_RESET_ALWAYS( !iChannelTable[ aChannel ].iWaitingChannel, EIADWrongRequest | EIADFaultIdentifier15 << KFaultIdentifierShift  );
        iChannelTable[ aChannel ].iWaitingChannel = aCallback;
        iChannelTable[ aChannel ].iType = ( ( aChannel == EIADNokiaDRM || aChannel == EIADNokiaSecurityDriver ) ?
#ifndef NCP_COMMON_BRIDGE_FAMILY
EDrmOpen : ENormalOpen );
#else
ENormalOpen : ENormalOpen );
#endif // NCP_COMMON_BRIDGE_FAMILY
        }
    C_TRACE( ( _T( "DRouter::Open 0x%x <-" ), aChannel ) );

    OstTraceExt1( TRACE_NORMAL, DROUTER_OPEN_EXIT, "<DRouter::Open;aChannel=%hx", aChannel );
    
    }

// With resource and media
EXPORT_C void DRouter::Open(
        const TUint16 aChannel,
        const TUint16 aRequest,
        const TDesC8& aOpenInfo,
        MIAD2ChannelApi* aCallback
        )
    {
    OstTraceExt4( TRACE_NORMAL, DUP1_DROUTER_OPEN_ENTRY, ">DRouter::Open;aChannel=%hx;aRequest=%hu;aOpenInfo=%x;aCallback=%x", aChannel, aRequest, ( TUint )&( aOpenInfo ), ( TUint )( aCallback ) );
//TODO: open with resource: change not to be permit one and in closing pn_name_remove_req.
    C_TRACE( ( _T( "DRouter::Open 0x%x %d 0x%x 0x%x ->" ), aChannel, aRequest, &aOpenInfo, aCallback ) );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    ASSERT_RESET_ALWAYS( aCallback, EIADNullParameter | EIADFaultIdentifier20 << KFaultIdentifierShift  );
    ASSERT_DFCTHREAD_INEXT();
    ASSERT_RESET_ALWAYS( aChannel < EIADSizeOfChannels, EIADWrongParameter | (TUint8)aChannel << KChannelNumberShift | EIADFaultIdentifier27 << KFaultIdentifierShift );
    if( iConnectionStatus == EIADConnectionOk )
        {
        // If not null channel is allready open.
        if( iChannelTable[ aChannel ].iChannel ||
            iChannelTable[ aChannel ].iWaitingChannel )
            {
            // If another channel tries to open already open channel.
            OstTrace0( TRACE_NORMAL, DUP4_DROUTER_OPEN, "DRouter::Open already open channel" );            
            TRACE_WARNING( iChannelTable[ aChannel ].iChannel == aCallback, (TUint8)aChannel << KChannelNumberShift );
            aCallback->CompleteChannelRequest( aRequest, ( iChannelTable[ aChannel ].iChannel == aCallback ? KErrInUse : KErrAlreadyExists ) );
            }
        // Null so channel is not open, set !null to mark opened channel.
        else
            {
            OstTrace0( TRACE_NORMAL, DUP3_DROUTER_OPEN, "DRouter::Open with resource" );            
            C_TRACE( ( _T( "DRouter::Open channel with resource 0x%x 0x%x" ), aChannel, aCallback ) );
            iChannelTable[ aChannel ].iWaitingChannel = aCallback;
            iChannelTable[ aChannel ].iType = ENameAddOpen;
            SendPnsNameAddReq( aChannel, aOpenInfo );
            }
        }
    else
        {
        C_TRACE( ( _T( "DRouter::Open with resource not ready" ) ) );
        // Opening with resource Id is not supported before IAD is initialized. 
        // If needed, client can open channel without resource Id and send PNS_NAME_ADD_REQ after 
        // channel open is actually completed
        TRACE_ASSERT_INFO( 0, (TUint8)aChannel<<KChannelNumberShift );
        ASSERT_RESET_ALWAYS( iBootDone, EIADNotSupported | EIADFaultIdentifier5 << KFaultIdentifierShift | (TUint8)aChannel<<KChannelNumberShift );        
        }
#else // NCP_COMMON_BRIDGE_FAMILY
    // Some maniac from modem sw decided to remove name service in the last meters, hip-hip hurray inform which clients use resource from open to help debug!
    TRACE_ASSERT_INFO( 0, (TUint8)aChannel<<KChannelNumberShift );
    // Treat as normal open to enable the sending NOTE! resource is not functioning!!
    Open( aChannel, aRequest, aCallback );
#endif // NCP_COMMON_BRIDGE_FAMILY

    C_TRACE( ( _T( "DRouter::Open 0x%x <-" ), aChannel ) );

    OstTraceExt1( TRACE_NORMAL, DUP1_DROUTER_OPEN_EXIT, "<DRouter::Open;aChannel=%hx", aChannel );
    }

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
EXPORT_C TInt DRouter::Loan(
        const TUint16 aChannel,
        const TUint16 aRequest,
        MIAD2ChannelApi* aCallback
        )
    {

    C_TRACE( ( _T( "DRouter::Loan 0x%x %d 0x%x ->" ), aChannel, aRequest, aCallback ) );
    OstTraceExt3( TRACE_NORMAL, DUP1_DROUTER_LOAN_ENTRY, ">DRouter::Loan;aChannel=%hx;aRequest=%hu;aCallback=%x", aChannel, aRequest, ( TUint )( aCallback ) );
    ASSERT_RESET_ALWAYS( aCallback, EIADNullParameter | EIADFaultIdentifier19 << KFaultIdentifierShift  );// TODO
    ASSERT_RESET_ALWAYS( aChannel < EIADSizeOfChannels, EIADWrongParameter | EIADFaultIdentifier11 << KFaultIdentifierShift );// TODO
    ASSERT_DFCTHREAD_INEXT();
    ASSERT_RESET_ALWAYS( aChannel < EIADSizeOfChannels, aChannel );
    TInt error( KErrNone );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    // Is connection lost.
    error = ( iConnectionStatus == EIADConnectionNotOk ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is loaning a resticted channel.
    error = ( KErrNone == error && ( aChannel == EIADNokiaDRM || aChannel == EIADNokiaSecurityDriver ) ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is loaning it self?
    error = ( KErrNone == error && ( iChannelTable[ aChannel ].iChannel == aCallback ) ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is loaning currently not open channel.
    error = ( KErrNone == error && ( !iChannelTable[ aChannel ].iChannel ) ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is loaning already loaned channel.
    error = ( KErrNone == error && ( iChannelTable[ aChannel ].iType == ELoan ) ) ? KErrAlreadyExists : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    if( KErrNone == error )
        {
        // Who is loaning whose channel.
        C_TRACE( ( _T( "DRouter::Loan loaning ch 0x%x from chObj 0x%x to chObj 0x%x setting previous as waiting" ), aChannel, iChannelTable[ aChannel ].iChannel, aCallback ) );
        OstTraceExt3( TRACE_NORMAL, DUP1_DROUTER_LOAN, "DRouter::Loan loaning;aChannel=%hx;channelPtr=%hx;callbackPtr=%hx", aChannel, (TUint)iChannelTable[ aChannel ].iChannel, (TUint)aCallback );
        iChannelTable[ aChannel ].iWaitingChannel = iChannelTable[ aChannel ].iChannel;// TEST WITH WAITING CHANNEL FIRST
        iChannelTable[ aChannel ].iType = ELoan;
        iChannelTable[ aChannel ].iChannel = aCallback;
        iPipeHandler->PipeLoaned( aChannel, aCallback );
        }
#endif
    C_TRACE( ( _T( "DRouter::Loan 0x%x %d<-" ), aChannel, error ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_DROUTER_LOAN_EXIT, "<DRouter::Loan;aChannel=%hx;error=%d", aChannel, error );
    return error;

    }

EXPORT_C TInt DRouter::ReturnLoan(
        const TUint16 aChannel,
        const TUint16 aRequest,
        MIAD2ChannelApi* aCallback
        )
    {

    C_TRACE( ( _T( "DRouter::ReturnLoan 0x%x %d 0x%x ->" ), aChannel, aRequest, aCallback ) );
    OstTraceExt3( TRACE_NORMAL, DUP1_DROUTER_RETURNLOAN_ENTRY, ">DRouter::ReturnLoan;aChannel=%hx;aRequest=%hu;aCallback=%x", aChannel, aRequest, ( TUint )( aCallback ) );
    ASSERT_RESET_ALWAYS( aCallback, EIADNullParameter | EIADFaultIdentifier19 << KFaultIdentifierShift  );// TODO
    ASSERT_RESET_ALWAYS( aChannel < EIADSizeOfChannels, EIADWrongParameter | EIADFaultIdentifier11 << KFaultIdentifierShift );// TODO
    ASSERT_DFCTHREAD_INEXT();
    ASSERT_RESET_ALWAYS( aChannel < EIADSizeOfChannels, aChannel );
    TInt error( KErrNone );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    // Is connection lost.
    error = ( iConnectionStatus == EIADConnectionNotOk ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is loaning a resticted channel.
    error = ( KErrNone == error && ( aChannel == EIADNokiaDRM || aChannel == EIADNokiaSecurityDriver ) ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is retuning someone elses loan?
    error = ( KErrNone == error && ( iChannelTable[ aChannel ].iChannel != aCallback ) ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is returning currently not open channel.
    error = ( KErrNone == error && ( !iChannelTable[ aChannel ].iChannel ) ) ? KErrNotSupported : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    // Is returning a not loaned channel.
    error = ( KErrNone == error && ( iChannelTable[ aChannel ].iType != ELoan ) ) ? KErrAlreadyExists : error;
    TRACE_ASSERT_INFO( KErrNone == error, error );
    if( KErrNone == error )
        {
        // Who is loaning whose channel.
        C_TRACE( ( _T( "DRouter::ReturnLoan returning loan ch 0x%x to chObj 0x%x from chObj 0x%x setting previous as waiting" ), aChannel, iChannelTable[ aChannel ].iWaitingChannel, aCallback ) );
        OstTraceExt3( TRACE_NORMAL, DUP1_DROUTER_RETURNLOAN, "DRouter::Loan loaning;aChannel=%hx;waitingchannelPtr=%hx;callbackPtr=%hx", aChannel, (TUint)iChannelTable[ aChannel ].iWaitingChannel, (TUint)aCallback );
        iPipeHandler->PipeLoanReturned( aChannel, iChannelTable[ aChannel ].iWaitingChannel );
        }
#endif
    C_TRACE( ( _T( "DRouter::ReturnLoan 0x%x %d<-" ), aChannel, error ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_DROUTER_RETURNLOAN_EXIT, "<DRouter::ReturnLoan;aChannel=%hx;error=%d", aChannel, error );
    return error;

    }
#endif

EXPORT_C TInt DRouter::OrderIndication(
        TDes8& anOrder,
        const TUint16 aCh,
        const TBool a32Bit
        )
    {
    OstTraceExt3( TRACE_NORMAL, DROUTER_ORDERINDICATION_ENTRY, ">DRouter::OrderIndication;anOrder=%x;aCh=%hx;a32Bit=%d", ( TUint )&( anOrder ), aCh, a32Bit );

    C_TRACE( ( _T( "DRouter::OrderIndication 0x%x 0x%x %d ->" ), &anOrder, aCh, a32Bit ) );
    ASSERT_DFCTHREAD_INEXT();
    TInt orderLength( anOrder.Length() );
    TInt msgOk = ( 0 == orderLength ) ? KErrBadDescriptor : KErrNone;
    TRACE_ASSERT_INFO( msgOk == KErrNone, (TUint8)aCh<<KChannelNumberShift );
    // Should be divisible by two if not 32 bit.
    msgOk = ( ( msgOk == KErrNone && ( !a32Bit && 0 != orderLength % 2 ) ) ? KErrBadDescriptor : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, (TUint8)aCh<<KChannelNumberShift );
    // Should be divisible by five if 32 bit.
    msgOk = ( ( msgOk == KErrNone && ( a32Bit && 0 != orderLength % 5 ) ) ? KErrBadDescriptor : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, (TUint8)aCh<<KChannelNumberShift );
    msgOk = ( ( msgOk == KErrNone && ( orderLength > GetMaxDataSize() ) ) ? KErrUnderflow : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, (TUint8)aCh<<KChannelNumberShift );
    if( KErrNone == msgOk )
        {
        msgOk = iIndicationHandler->Subscribe( anOrder, aCh, a32Bit );
        TRACE_ASSERT_INFO( msgOk == KErrNone, (TUint8)aCh<<KChannelNumberShift );
        }
    C_TRACE( ( _T( "DRouter::OrderIndication 0x%x 0x%x %d %d <-" ), &anOrder, aCh, a32Bit, msgOk ) );
    
    OstTrace1( TRACE_NORMAL, DROUTER_ORDERINDICATION, "<DRouter::OrderIndication;msgOk=%x", msgOk );    
    return msgOk;
    }

EXPORT_C TInt DRouter::SendMessage(
        TDes8& aMessage,
        const TUint16 aCh
        )
    {

    OstTraceExt2( TRACE_NORMAL, DROUTER_SENDMESSAGE_ENTRY, "<DRouter::SendMessage;aMessage=%x;aCh=%hx", ( TUint )&( aMessage ), aCh );
    OstTraceExt1( TRACE_NORMAL, DUP4_DROUTER_SENDMESSAGE, "DRouter::SendMessage;aCh=%hx", aCh );
    C_TRACE( ( _T( "DRouter::SendMessage 0x%x 0x%x ->" ), &aMessage, aCh ) );
    ASSERT_RESET_ALWAYS( iMaxFrameSize != 0x0000, EIADCmtConnectionNotInit );
    TInt error( ValiDateIsiMessage( aMessage ) );
    error = ( KErrNone == error && iConnectionStatus != EIADConnectionOk ) ? KErrNotReady : error;
    if( KErrNone == error )
        {
        ASSERT_RESET_ALWAYS( aCh != KNotInitializedChannel, EIADWrongParameter | EIADFaultIdentifier12 << KFaultIdentifierShift );
        C_TRACE( ( _T( "DRouter::SendMessage sending 0x%x" ), &aMessage ) );
        OstTraceExt2( TRACE_NORMAL, DROUTER_SENDMESSAGE, "DRouter::SendMessage;error=%d;aCh=%hx", error, aCh );
        
        SetSenderInfo( aMessage, aCh );
        const TUint8* msgBlockPtr( aMessage.Ptr() );
        // TODO: Simplify this        
        ASSERT_RESET_ALWAYS( aMessage.Length() > ISI_HEADER_OFFSET_RESOURCEID, EIADOverTheLimits | EIADFaultIdentifier2 << KFaultIdentifierShift  );
        if( msgBlockPtr[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_PIPE && msgBlockPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == THIS_DEVICE )
            {
#ifndef NCP_COMMON_BRIDGE_FAMILY
            // This shall send the message and deallocate it too.
            error = iPipeHandler->SendPipeMessage( aMessage, aCh );
#endif
            if( error != KErrNone )
                {
                // Deallocate the block.
                this->DeAllocateBlock( aMessage );
                }
            }
        else
            {
            // Route indication to possible subscribers in this device
            if( GET_RECEIVER_OBJ( msgBlockPtr ) == PN_OBJ_EVENT_MULTICAST &&
                IS_DEV_HOST( msgBlockPtr[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] ) )
                {
                DATA_DUMP_TRACE( aMessage, ETrue );
                OstTraceData( TRACE_ISIMSG, DROUTER_SENDMESSAGE_DATA, "DRouter::SendMsg indication TX: %hx", aMessage.Ptr(), aMessage.Length() );
                C_TRACE( ( _T( "DRouter::SendMessage routing indication to device PN_DEV_SOS 0x%x" ), &aMessage ) );
                OstTrace0( TRACE_NORMAL, DUP2_DROUTER_SENDMESSAGE, "DRouter::SendMessage indication to PN_DEV_SOS" );
                TDes8& tmpIndication = AllocateBlock( aMessage.Length() );
                tmpIndication.SetLength( aMessage.Length() );
                tmpIndication.Copy( aMessage );
                TUint8* indicationPtr = const_cast<TUint8*>( tmpIndication.Ptr() );
                SET_RECEIVER_DEV( indicationPtr, THIS_DEVICE );
                SET_RECEIVER_OBJ( indicationPtr, KIADEventSubscriptionObjId );
                this->MessageReceived( tmpIndication );
                }
            // The IST shall deallocate the block when it's approriate to do.
            C_TRACE( ( _T( "DRouter::SendMessage sending 0x%x" ), &aMessage ) );
            OstTrace1( TRACE_NORMAL, DUP3_DROUTER_SENDMESSAGE, "DRouter::SendMessage;aMessage=%x", (TUint)&(aMessage ));
            error = SendMsg( aMessage );
            }//PIPE
        }
    else
        {
        C_TRACE( ( _T( "DRouter::SendMessage not sending due error 0x%x %d %d" ), &aMessage, aCh, error ) );
        OstTraceExt2( TRACE_NORMAL, DUP1_DROUTER_SENDMESSAGE, "DRouter::SendMessage;aCh=%hx;error=%d", aCh, error );
        // Deallocate the block.
        TRACE_ASSERT_INFO( 0, (TUint8)aCh<<KChannelNumberShift | (TUint8)error );
        this->DeAllocateBlock( aMessage );
        // TODO: who should NULL the block? IST or IAD
        }
    C_TRACE( ( _T( "DRouter::SendMessage 0x%x %d %d <-" ), &aMessage, aCh, error ) );
    OstTraceExt2( TRACE_NORMAL, DROUTER_SENDMESSAGE_EXIT, "<DRouter::SendMessage;aCh=%hx;error=%d", aCh, error );
    return error;

    }

EXPORT_C TInt DRouter::SendIndication(
        TDes8& aMessage,
        const TUint16 aCh
        )
    {

    OstTraceExt2( TRACE_NORMAL, DROUTER_SENDINDICATION_ENTRY, ">DRouter::SendIndication;aMessage=%x;aCh=%hx", ( TUint )&( aMessage ), aCh );
    C_TRACE( ( _T( "DRouter::SendIndication 0x%x 0x%x ->" ), &aMessage, aCh ) );
    TUint8* msgPtr = const_cast<TUint8*>( aMessage.Ptr() );
    SET_RECEIVER_DEV( msgPtr,OTHER_DEVICE_1 );
    SET_RECEIVER_OBJ( msgPtr,PN_OBJ_EVENT_MULTICAST );
    TInt error( SendMessage( aMessage, aCh ) );
    C_TRACE( ( _T( "DRouter::SendIndication 0x%x 0x%x %d <-" ), &aMessage, aCh, error ) );
    OstTrace1( TRACE_NORMAL, DROUTER_SENDINDICATION_EXIT, "<DRouter::SendIndication;error=%d", error );
    return error;

    }

// From   end

void DRouter::NotifyObjLayerConnStatDfc(
        TAny* aPtr
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_NOTIFYOBJLAYERCONNSTATDFC_ENTRY, ">DRouter::NotifyObjLayerConnStatDfc;aPtr=%x", ( TUint )( aPtr ) );

    C_TRACE( ( _T( "DRouter::NotifyObjLayerConnStatDfc ->" ) ) );
    DRouter* self = reinterpret_cast<DRouter*>( aPtr );
    self->NotifyObjLayerConnStat( self->iConnectionStatus );
    C_TRACE( ( _T( "DRouter::NotifyObjLayerConnStatDfc to %d <-" ), self->iConnectionStatus ) );

    OstTrace1( TRACE_NORMAL, DROUTER_NOTIFYOBJLAYERCONNSTATDFC_EXIT, "<DRouter::NotifyObjLayerConnStatDfc;self->iConnectionStatus=%x", self->iConnectionStatus );    
    }

void DRouter::NotifyObjLayerConnStat(
        const TIADConnectionStatus aStatus
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_NOTIFYOBJLAYERCONNSTAT_ENTRY, ">DRouter::NotifyObjLayerConnStat;aStatus=%x", ( TUint )&( aStatus ) );

    C_TRACE( ( _T( "DRouter::NotifyObjLayerConnStat %d ->" ), aStatus ) );
    ASSERT_DFCTHREAD_INEXT();
    for( TUint16 i( 0 ); i < EIADSizeOfChannels; ++i )
        {
        if( iChannelTable[ i ].iChannel )
            {
            C_TRACE( ( _T( "DRouter::NotifyObjLayerConnStat ch %d ptr 0x%x" ), i, iChannelTable[ i ].iChannel ) );
            OstTraceExt2( TRACE_NORMAL, DUP1_DROUTER_NOTIFYOBJLAYERCONNSTAT, "DRouter::NotifyObjLayerConnStat;i=%hx;iChannelTable[ i ].iChannel=%x", i,( TUint ) ( iChannelTable[ i ].iChannel ) );
            iChannelTable[ i ].iChannel->NotifyConnectionStatus( aStatus );
            }
        // TODO: If we need open to be waiting until connection status is ok, set waiting here.
        }
    C_TRACE( ( _T( "DRouter::NotifyObjLayerConnStat %d <-" ), aStatus ) );
    OstTrace0( TRACE_NORMAL, DROUTER_NOTIFYOBJLAYERCONNSTAT_EXIT, "<DRouter::NotifyObjLayerConnStat" );
    }

// Used internally in IAD.
// Can be called in 1..N thread contextes and returns immediately.
void DRouter::MessageReceived(
        TDes8& aMsg
        )
    {

    OstTrace1( TRACE_NORMAL, DROUTER_MESSAGERECEIVED_ENTRY, ">DRouter::MessageReceived;aMsg=%x", ( TUint )&( aMsg ) );
    C_TRACE( ( _T( "DRouter::MessageReceived 0x%x ->" ), &aMsg ) );
    // Let MaxLength to be the length of the allocated block. (IST (SSI&SISA) can work differently.
    // SISA set length and maxlength according to ISI msg length, but SSI set length according to block length
    // Block length in SSI at the moment always %4.
    // Set the length of the descriptor to ISI message length + PN_HEADER_SIZE
    // TODO : harmonize these IST differencies.
    TInt len( GET_LENGTH( aMsg ) );
    len += PN_HEADER_SIZE;
    aMsg.SetLength( len );
    iCommonRxQueue->Add( aMsg );
    iCommonRxDfc->Enque();
    C_TRACE( ( _T( "DRouter::MessageReceived 0x%x <-" ), &aMsg ) );
    OstTrace0( TRACE_NORMAL, DROUTER_MESSAGERECEIVED_EXIT, "<DRouter::MessageReceived" );

    }

//PRIVATES

void DRouter::HandleIsiMessage(
        TDes8& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_HANDLEISIMESSAGE_ENTRY, ">DRouter::HandleIsiMessage;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DRouter::HandleIsiMessage 0x%x ->" ), &aMsg ) );
    const TUint16 rcvObjId( GET_RECEIVER_OBJ( aMsg ) );
    C_TRACE( ( _T( "DRouter::HandleIsiMessage rcvObjId 0x%x" ), rcvObjId ) );
    ASSERT_RESET_ALWAYS( rcvObjId < EIADSizeOfChannels, EIADWrongParameter | (TUint8)rcvObjId << KChannelNumberShift | EIADFaultIdentifier22 << KFaultIdentifierShift );
    if( iChannelTable[ rcvObjId ].iChannel )
        {
        iChannelTable[ rcvObjId ].iChannel->ReceiveMsg( aMsg );
        // DeAllocation done by the channel after writing to client's address space.
        }
    else
        {
        SendCommIsaEntityNotReachableResp( aMsg );
        // Not going to anywhere deallocate.
        DeAllocateBlock( aMsg );
        }
    C_TRACE( ( _T( "DRouter::HandleIsiMessage 0x%x <-" ), &aMsg ) );

    OstTrace0( TRACE_NORMAL, DROUTER_HANDLEISIMESSAGE_EXIT, "<DRouter::HandleIsiMessage" );    
    }


void DRouter::HandlePipeMessage(
        TDes8& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_HANDLEPIPEMESSAGE_ENTRY, ">DRouter::HandlePipeMessage;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DRouter::HandlePipeMessage 0x%x ->" ), &aMsg ) );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    const TUint16 rcvObjId( GET_RECEIVER_OBJ( aMsg ) );
    C_TRACE( ( _T( "DRouter::HandlePipeMessage rcvObjId 0x%x" ), rcvObjId ) );
    ASSERT_RESET_ALWAYS( rcvObjId < EIADSizeOfChannels, EIADWrongParameter | (TUint8)rcvObjId << KChannelNumberShift| EIADFaultIdentifier25 << KFaultIdentifierShift );    
    MIAD2ChannelApi* openChannel = iChannelTable[ rcvObjId ].iChannel;
    if( openChannel )
        {
        // TODO: is it ok to give channel from here?
        iPipeHandler->ReceivePipeMessage( aMsg, openChannel );
        // DeAllocation done by the pipehandler or someone who it frwd's the message.
        }
    else
        {
        // COMM_ISA_ENTITY_NOT_REACHABLE_RESP NOT SEND TO PIPE MESSAGES
        TRACE_ASSERT_INFO( 0, (TUint8)rcvObjId<<KChannelNumberShift );
        // Not going to anywhere deallocate.
        for( TInt i( 0 ); i < aMsg.Length(); i++ )
            {
            Kern::Printf( "%d 0x%x", i, aMsg.Ptr()[ i ] );
            }
        DeAllocateBlock( aMsg );
        }
#endif
    C_TRACE( ( _T( "DRouter::HandlePipeMessage 0x%x <-" ), &aMsg ) );

    OstTrace0( TRACE_NORMAL, DROUTER_HANDLEPIPEMESSAGE_EXIT, "<DRouter::HandlePipeMessage" );    
    }

void DRouter::HandleMediaMessage(
        TDes8& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_HANDLEMEDIAMESSAGE_ENTRY, ">DRouter::HandleMediaMessage;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DRouter::HandleMediaMessage 0x%x ->" ), &aMsg ) );
    TUint8 rcvObjId( 0x00);
    ASSERT_RESET_ALWAYS( aMsg.Length() > ISI_HEADER_OFFSET_MEDIA, EIADOverTheLimits | EIADFaultIdentifier3 << KFaultIdentifierShift  );
    switch( aMsg[ ISI_HEADER_OFFSET_MEDIA ] )
        {
#ifndef NCP_COMMON_BRIDGE_FAMILY
        case PN_MEDIA_USB:
            {
            C_TRACE( ( _T( "DRouter::HandleMediaMessage USB" ) ) );
            OstTrace0( TRACE_NORMAL, DROUTER_HANDLEMEDIAMESSAGE_USB, "DRouter::HandleMediaMessage USB" );            
            rcvObjId = EIADNokiaUsbPhonetLink;
            break;
            }
        case PN_MEDIA_BT:
            {
            C_TRACE( ( _T( "DRouter::HandleMediaMessage BT" ) ) );
            OstTrace0( TRACE_NORMAL, DROUTER_HANDLEMEDIAMESSAGE_BT, "DRouter::HandleMediaMessage BT" );            
            rcvObjId = EIADNokiaBtPhonetLink;
            break;
            }
        //TBR AFTER CMT ERROR CORRECTION : wk49 cellmo has correction, so remove this later on
        case PN_MEDIA_ROUTING_REQ:
            {
            // TODO: write an error! Someone is sending to APE with wrong media.
            // USB PDD
            TRACE_ASSERT_ALWAYS;
            rcvObjId = GET_RECEIVER_OBJ( aMsg );
            Kern::Printf("Unknown message 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", aMsg[ 0 ], aMsg[ 1 ], aMsg[ 2 ], aMsg[ 3 ], aMsg[ 4 ], aMsg[ 5 ], aMsg[ 6 ], aMsg[ 7 ], aMsg[ 8 ], aMsg[ 9 ] );
            break;
            }
#endif
        //TBR AFTER CMT ERROR CORRECTION
        default:
            {
            rcvObjId = GET_RECEIVER_OBJ( aMsg );
            ASSERT_RESET_ALWAYS( 0, EIADUnkownMedia | ( rcvObjId << KChannelNumberShift ) );
            break;
            }
        }
    // TODO: UNIQUE
    ASSERT_RESET_ALWAYS( rcvObjId < EIADSizeOfChannels, EIADWrongParameter | (TUint8)rcvObjId << KChannelNumberShift | EIADFaultIdentifier3 << KFaultIdentifierShift  );
    if( iChannelTable[ rcvObjId ].iChannel )
        {
        iChannelTable[ rcvObjId ].iChannel->ReceiveMsg( aMsg );
        // DeAllocation done by the channel after writing to client's address space.
        }
    else
        {
        SendCommIsaEntityNotReachableResp( aMsg );
        // Not going to anywhere deallocate.
        DeAllocateBlock( aMsg );
        }
    C_TRACE( ( _T( "DRouter::HandleMediaMessage 0x%x <-" ), &aMsg ) );

    OstTrace0( TRACE_NORMAL, DROUTER_HANDLEMEDIAMESSAGE_EXIT, "<DRouter::HandleMediaMessage" );
    }

void DRouter::HandleControlMessage(
        TDes8& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_HANDLECONTROLMESSAGE_ENTRY, ">DRouter::HandleControlMessage;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DRouter::HandleControlMessage 0x%x ->" ), &aMsg ) );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    const TUint8* msgPtr( aMsg.Ptr() );
    // Check legal msgs
    ASSERT_RESET_ALWAYS( msgPtr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_RESP_OFFSET_SUBFUNCTION ] == PNS_MEDIA_SPECIFIC_RESP, EIADInvalidCtrlMessage | EIADFaultIdentifier1 << KFaultIdentifierShift  );
    ASSERT_RESET_ALWAYS( msgPtr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_RESP_OFFSET_MEDIATOCTRL ] == PN_MEDIA_SOS, EIADInvalidCtrlMessage | EIADFaultIdentifier2 << KFaultIdentifierShift  );
    ASSERT_RESET_ALWAYS( msgPtr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_RESP_OFFSET_ERRORCODE ] == PN_MCTRL_NO_ERROR, EIADInvalidCtrlMessage | EIADFaultIdentifier3 << KFaultIdentifierShift  );
    ASSERT_RESET_ALWAYS( aMsg.Length() > CTRL_RESP_OFFSET_MESSAGEID,  EIADOverTheLimits | EIADFaultIdentifier4 << KFaultIdentifierShift );
    switch( msgPtr[ CTRL_RESP_OFFSET_MESSAGEID ] )
        {
        case KIADCtrlConfResp:
            {
            // In MINI_OS case take the maximumdatasize amount from configuration message if bigger than "hat" constant.
            iMaxFrameSize = KIADMaxIsiMsgSize;
#ifdef NCP_COMMON_MINIOS
            TUint16 confMsgSize = ( ( ( ( TUint16 )( msgPtr[ CTRL_CONF_RESP_OFFSET_SIZE_BYTE1 ] ) ) << 8 ) | ( ( TUint16 )( msgPtr[ CTRL_CONF_RESP_OFFSET_SIZE_BYTE2 ] ) ) );
            // NOTE! This hat constant has relation to cmt side (sos_mm) if the const is changed in either place harmonize!!
            const TUint16 KSosProxyHatConst( 2140 );
            iMaxFrameSize = ( confMsgSize > KSosProxyHatConst ? confMsgSize : KIADMaxIsiMsgSize );
            ASSERT_RESET_ALWAYS( iMaxFrameSize <= confMsgSize, EIADConfigurationInvalid );
#endif
            // Datalink layer initialized, router layer is this and knows it, now inform obj layer clients.
            iConnectionStatus = EIADConnectionOk;
            iBootDone = ETrue;
            // Could call straight cause the context is the same thread (extension dfc)
            NotifyObjLayerConnStat( EIADConnectionOk );
            // TODO: pns_name_add_req
            // Initialize channels to NULL when channel is opened !NULL.
            for( TInt i( 0 ); i < EIADSizeOfChannels; ++i )
                {
                ASSERT_RESET_ALWAYS( !iChannelTable[ i ].iChannel, EIADChannelOpenedBeforePhysicalLayerInit );
                C_TRACE( ( _T( "DRouter::HandleControlMessage loop TBR 0x%x" ), i ) );
                if( iChannelTable[ i ].iWaitingChannel )
                    {
                    switch( iChannelTable[ i ].iType )
                        {
                        case ENormalOpen:
                            {
                            C_TRACE( ( _T( "DRouter::HandleControlMessage booting ENormalOpen 0x%x" ), i ) );
                            OstTrace1( TRACE_NORMAL, DROUTER_HANDLECONTROLMESSAGE_NORMAL_OPEN, "DRouter::HandleControlMessage;i=%x", i );
                            
                            MIAD2ChannelApi* tmpChannel = iChannelTable[ i ].iWaitingChannel;
                            iChannelTable[ i ].iChannel = tmpChannel;
                            iChannelTable[ i ].iWaitingChannel = NULL;
                            iChannelTable[ i ].iChannel->CompleteChannelRequest( EIADAsyncOpen, KErrNone );
                            break;
                            }
                        case EDrmOpen:
                            {
                            C_TRACE( ( _T( "DRouter::HandleControlMessage booting EDrmOpen 0x%x" ), i ) );
                            OstTrace1( TRACE_NORMAL, DROUTER_HANDLECONTROLMESSAGE_DRM_OPEN, "DRouter::HandleControlMessage EDrmOpen;i=%x", i );
                            
                            SendDrmReq( i );
                            break;
                            }
                        case ENameAddOpen:
                            {
                            C_TRACE( ( _T( "DRouter::HandleControlMessage booting ENameAddOpen 0x%x" ), i ) );
                            OstTrace1( TRACE_NORMAL, DROUTER_HANDLECONTROLMESSAGE_NAMEADD_OPEN, "DRouter::HandleControlMessage ENameAddOpen;i=%x", i );
                            
                            // TODO: Not done yet, problem info get as param, channel to allocate deallocate after open complete?
                            ASSERT_RESET_ALWAYS( 0, EIADWrongParameter | EIADFaultIdentifier13 << KFaultIdentifierShift );
                            break;
                            }
                        default:
                            {
                            ASSERT_RESET_ALWAYS( 0, EIADWrongParameter | EIADFaultIdentifier14 << KFaultIdentifierShift  );
                            break;
                            }
                        }
                    }
                }
            C_TRACE( ( _T( "DRouter::HandleControlMessage conf %d" ), iMaxFrameSize ) );
            break;
            }
        case KIADCtrlDrmResp:
            {
            const TUint16 channelId( msgPtr[ CTR_DRM_REQ_RESP_OFFSET_CHANNEL_ID ] );
            C_TRACE( ( _T( "DRouter::HandleControlMessage drm resp 0x%x" ), channelId ) );
            OstTrace0( TRACE_NORMAL, DROUTER_HANDLECONTROLMESSAGE_DRM_RESP, "DRouter::HandleControlMessage drm resp");
            
            // Check is this waiting
            ASSERT_RESET_ALWAYS( channelId < EIADSizeOfChannels, EIADWrongParameter | (TUint8)channelId << KChannelNumberShift | EIADFaultIdentifier28 << KFaultIdentifierShift );
            TRACE_ASSERT_INFO( iChannelTable[ channelId ].iWaitingChannel, (TUint8)channelId<<KChannelNumberShift );
            TRACE_ASSERT_INFO( !iChannelTable[ channelId ].iChannel, (TUint8)channelId<<KChannelNumberShift );
            ASSERT_RESET_ALWAYS( iChannelTable[ channelId ].iType == EDrmOpen, EIADWrongTypeOfOpenPending | EIADFaultIdentifier1 << KFaultIdentifierShift );
            MIAD2ChannelApi* tmpChannel = iChannelTable[ channelId ].iWaitingChannel;
            iChannelTable[ channelId ].iChannel = tmpChannel;
            iChannelTable[ channelId ].iWaitingChannel = NULL;
            C_TRACE( ( _T( "DRouter::HandleControlMessage drm resp 0x%x 0x%x 0x%x 0x%x" ), channelId, tmpChannel, iChannelTable[ channelId ].iChannel, &aMsg ) );
            iChannelTable[ channelId ].iChannel->CompleteChannelRequest( EIADAsyncOpen, KErrNone );
            break;
            }
        default:
            {
            // If wrong message reset! IF changed take care of completion of requests.
            ASSERT_RESET_ALWAYS( 0, EIADInvalidCtrlMessage | EIADFaultIdentifier5 << KFaultIdentifierShift  );
            break;
            }
        } // switch
#endif
    // De-allocate cause not going anywhere else.
    DeAllocateBlock( aMsg );
    C_TRACE( ( _T( "DRouter::HandleControlMessage 0x%x <-" ), &aMsg ) );

    OstTrace0( TRACE_NORMAL, DROUTER_HANDLECONTROLMESSAGE_EXIT, "<DRouter::HandleControlMessage" );
    }

// KErrBadDescriptor, if message length too small
// KErrUnderFlow, if message length too big.
// KErrCouldNotConnect, if receiver object is out of scope.
TInt DRouter::ValiDateIsiMessage(
        TDes8& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_VALIDATEISIMESSAGE_ENTRY, ">DRouter::ValiDateIsiMessage;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DRouter::ValiDateIsiMessage ->" ) ) );
    const TUint16 descLength( aMsg.Length() );
    TInt msgOk( KErrNone );
    msgOk = ( ISI_HEADER_OFFSET_MESSAGEID >= descLength ) ? KErrBadDescriptor : msgOk;
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    // Get ISI message length after known that the descriptor is big enough.
    const TUint8* msgPtr( aMsg.Ptr() );
    const TUint16 isiMsgLength( GET_LENGTH( msgPtr ) + PN_HEADER_SIZE );
    // TODO: Do we need the underflow information or is it BadDescriptor as good? If so remove msgok == KErrNone...
    // If the descriptor length is less than ISI message length.
    msgOk = ( ( msgOk == KErrNone && isiMsgLength > descLength ) ? KErrUnderflow : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    // If the ISI message length is bigger that the largest supported.
    msgOk = ( ( msgOk == KErrNone && isiMsgLength > KIADMaxIsiMsgSize ) ? KErrUnderflow : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    // If the ISI message length with PN_HEADER_SIZE is less or equal than ISI_HEADER_OFFSET_MESSAGEID. 
    msgOk = ( ( msgOk == KErrNone && isiMsgLength <= ISI_HEADER_OFFSET_MESSAGEID ) ? KErrUnderflow : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, isiMsgLength );
    TRACE_ASSERT_INFO( msgOk == KErrNone, descLength );
    
    // Check is receiver object valid if going from SOS<->SOS.
    msgOk = ( ( msgOk == KErrNone && ( GET_RECEIVER_DEV( msgPtr ) == THIS_DEVICE && GET_RECEIVER_OBJ( msgPtr ) > EIADSizeOfChannels ) ) ? KErrCouldNotConnect : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    // TODO: checking that receiver object is ok in SOS<->ISA cases.
    C_TRACE( ( _T( "DRouter::ValiDateIsiMessage <- %d" ), msgOk ) );

    
    OstTrace1( TRACE_NORMAL, DROUTER_VALIDATEISIMESSAGE_EXIT, "<DRouter::ValiDateIsiMessage;msgOk=%x", msgOk );
    return msgOk;
    }

void DRouter::CheckRouting(
        DRouter& aTmp,
        TDes8& aMsg
        )
    {

    ASSERT_DFCTHREAD_INEXT();
    const TUint8* msg( aMsg.Ptr() );
    OstTrace1( TRACE_NORMAL, DROUTER_CHECKROUTING_ENTRY, ">DRouter::CheckRouting;aMsg=%x", ( TUint )( msg ) );
    C_TRACE( ( _T( "DRouter::CheckRouting 0x%x 0x%x ->" ), msg, msg[ ISI_HEADER_OFFSET_RESOURCEID ] ) );
    TRoutingRule route( ENotKnownMsg );
    // Message to symbian side (media sos).
    if( msg[ ISI_HEADER_OFFSET_MEDIA ] == PN_MEDIA_SOS )
        {
        if( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] == THIS_DEVICE )
            {
            switch( msg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] )
                {
#ifndef NCP_COMMON_BRIDGE_FAMILY
                // Router <-> SOSMM
                case KIADObjId:
                    {
                    // Name add resp, drm or conf
                    ASSERT_RESET_ALWAYS( msg[  ISI_HEADER_OFFSET_RESOURCEID ] == PN_NAMESERVICE || msg[  ISI_HEADER_OFFSET_RESOURCEID ] == PN_MEDIA_CONTROL, EIADWrongParameter | msg[  ISI_HEADER_OFFSET_RESOURCEID ] );
                    route = ( msg[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_NAMESERVICE ) ? EPnNameAddRespMsg : EControlMsg;
                    break;
                    }
#endif
                // Indication
                case KIADEventSubscriptionObjId:
                    {
                    ASSERT_RESET_ALWAYS( msg[  ISI_HEADER_OFFSET_RESOURCEID ] != PN_PIPE, EIADWrongParameter | msg[  ISI_HEADER_OFFSET_RESOURCEID ] );
                    route = EIndicationMsg;
                    break;
                    }
                // Any other message.
                default:
                    {
                    // Indication message. Receiver object equals to event subscription obj id (0xfc).
                    route = ( msg[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_PIPE ) ? EPipeMsg : EIsiMsg;
                    break;
                    }
                }
            }
        else if ( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] == PN_DEV_GLOBAL )// TODO: This (PN_DEV_GLOBAL) should be removed from Bridge when modem SW MCE Server is ok
            {
#ifdef MODEM_MCE_DOES_NOT_WORK_AS_IT_SHOULD // TODO: To be removed depends on modem sw version MCE server
            if ( aTmp.iBootDone )
               {
#endif // MODEM_MCE_DOES_NOT_WORK_AS_IT_SHOULD
            C_TRACE( ( _T( "DRouter::CheckRouting message to PN_DEV_GLOBAL -> EIndicationMsg" ) ) );
            OstTrace0( TRACE_NORMAL, DROUTER_CHECKROUTING_PN_DEV_GLOBAL, "DRouter::CheckRouting PN_DEV_GLOBAL -> EIndicationMsg");
            TUint8* ptr = const_cast<TUint8*>( msg );    
            SET_RECEIVER_DEV( ptr, THIS_DEVICE );
            route = EIndicationMsg;
            }
#ifdef MODEM_MCE_DOES_NOT_WORK_AS_IT_SHOULD
#include <mceisi.h>
            else
                {
                Kern::Printf("0x%x 0x%x", msg[ ISI_HEADER_SIZE + MCE_MODEM_STATE_IND_OFFSET_MESSAGEID ], msg[ ISI_HEADER_SIZE + MCE_MODEM_STATE_IND_OFFSET_ACTION ] );
                if ( ( msg[ ISI_HEADER_SIZE + MCE_MODEM_STATE_IND_OFFSET_MESSAGEID ] == MCE_MODEM_STATE_IND ) && 
                   ( msg[ ISI_HEADER_SIZE + MCE_MODEM_STATE_IND_OFFSET_ACTION ] == MCE_READY ) )
                    {
                    //15:08:55.563 xti: MASTER_ASCII_PRINTF; string:M_HSI: TX common: 26ff60c2 06000014 00000004 ffffffff
                    Kern::Printf("Connection OK");
                    aTmp.InitConnectionOk();
                    }
                else
                    {
                    // just ignore and wait for MCE_READY_IND DEALLOC????
                    Kern::Printf("Connection NOK");
                    }
                }
            }
#endif // MODEM_MCE_DOES_NOT_WORK_AS_IT_SHOULD
        else
            {
            C_TRACE( ( _T( "DRouter::CheckRouting Unknown message" ) ) );
            TRACE_ASSERT_ALWAYS;
            Kern::Printf("Unknown message 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", msg[ 0 ], msg[ 1 ], msg[ 2 ], msg[ 3 ], msg[ 4 ], msg[ 5 ], msg[ 6 ], msg[ 7 ], msg[ 8 ], msg[ 9 ] );
            }
        }
    // Message to other media than sos in symbian side.
    else
        {
        route = EMediaMsg;
        }
    C_TRACE( ( _T( "DRouter::CheckRouting msg 0x%x route %d" ), msg, route ) );
    OstTraceExt2( TRACE_NORMAL, DROUTER_CHECKROUTING_ROUTE, "DRouter::CheckRouting;aMsg=0x%x;route=%d", ( TUint )msg, route );
    switch( route )
        {
        case EIsiMsg:
            {
            aTmp.HandleIsiMessage( aMsg );
            break;
            }
        case EPipeMsg:
            {
            aTmp.HandlePipeMessage( aMsg );
            break;
            }
        case EMediaMsg:
            {
            aTmp.HandleMediaMessage( aMsg );
            break;
            }
        case EIndicationMsg:
            {
            aTmp.iIndicationHandler->Multicast( aMsg );
            // De-allocate, message is multicasted to subsribers as new
            // message and the original is ready to be deallocated.
            aTmp.DeAllocateBlock( aMsg );
            break;
            }
#ifndef NCP_COMMON_BRIDGE_FAMILY
        case EControlMsg:
            {
            aTmp.HandleControlMessage( aMsg );
            break;
            }
        case EPnNameAddRespMsg:
            {
            aTmp.HandlePnsNameAddResp( aMsg );
            break;
            }
#endif // NCP_COMMON_BRIDGE_FAMILY
        case ENotKnownMsg:
            {
            // Not going to anywhere deallocate.
            aTmp.DeAllocateBlock( aMsg );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADWrongParameter | EIADFaultIdentifier16 << KFaultIdentifierShift  );
            break;
            }
        }
    C_TRACE( ( _T( "DRouter::CheckRouting 0x%x<-" ), msg ) );
    OstTrace1( TRACE_NORMAL, DROUTER_CHECKROUTING_EXIT, "<DRouter::CheckRouting;msg=0x%x", (TUint)msg );

    }

void DRouter::CommonRxDfc(
        TAny* aPtr  // Pointer to this object.
        )
    {
    
    OstTrace1( TRACE_NORMAL, DROUTER_COMMONRXDFC_ENTRY, ">DRouter::CommonRxDfc;aPtr=%x", ( TUint )( aPtr ) );
    C_TRACE( ( _T( "DRouter::CommonRxDfc ->" ) ) );
    DRouter& tmp = *reinterpret_cast<DRouter*>( aPtr );
    ASSERT_DFCTHREAD_INEXT();
    if( tmp.iCommonRxQueue->Count() > KErrNone )
        {
        TDes8& msg( tmp.iCommonRxQueue->Get() );
        DATA_DUMP_TRACE( msg, EFalse );// TODO: this causes problems in legacy flowcontrol causing main rx to overflow!!
        OstTraceData( TRACE_ISIMSG, DROUTER_COMMONRXDFC_DATA, "DRouter::CommonRxDfc RX: 0x%hx", msg.Ptr(), msg.Length() );
        CheckRouting( tmp, msg );
        // Check here too to avoid unnecessary dfc queuing.
        if( tmp.iCommonRxQueue->Count() > KErrNone )
            {
            C_TRACE( ( _T( "DRouter::CommonRxDfc enque commonrxdfc" ) ) );
            tmp.iCommonRxDfc->Enque();
            }
        }
    C_TRACE( ( _T( "DRouter::CommonRxDfc <-" ) ) );
    OstTrace0( TRACE_NORMAL, DROUTER_COMMONRXDFC_EXIT, "<DRouter::CommonRxDfc" );

    }

void DRouter::SendCommIsaEntityNotReachableResp(
        const TDesC8& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_SENDCOMMISAENTITYNOTREACHABLERESP_ENTRY, ">DRouter::SendCommIsaEntityNotReachableResp;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DRouter::SendCommIsaEntityNotReachableResp 0x%x ->" ), &aMsg ) );
    // Make channel opening request followinfg COMM specification: 000.026
    // Length is sixteen bytes.
    TUint8 length( 16 );
    TDes8& tempPtr = AllocateBlock( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP );
    ASSERT_RESET_ALWAYS( &tempPtr, EIADMemoryAllocationFailure | EIADFaultIdentifier19 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP > ISI_HEADER_OFFSET_MESSAGEID, EIADOverTheLimits | EIADFaultIdentifier5 << KFaultIdentifierShift ); 
    TUint8* ptr = const_cast<TUint8*>( tempPtr.Ptr() );
    // We start to append from transaction id.
    tempPtr.SetLength( ISI_HEADER_OFFSET_TRANSID );
    // Get the header until messageid from prev. message.
    // Just turn receiver and sender device and object vice versa.
    const TUint8* msgTmpPtr( aMsg.Ptr() );
    ptr[ ISI_HEADER_OFFSET_MEDIA ] = msgTmpPtr[ ISI_HEADER_OFFSET_MEDIA ];
    SET_RECEIVER_DEV( ptr, GET_SENDER_DEV( aMsg ) );
    SET_SENDER_DEV  ( ptr, GET_RECEIVER_DEV( aMsg ) );
    ptr[ ISI_HEADER_OFFSET_RESOURCEID ] = msgTmpPtr[ ISI_HEADER_OFFSET_RESOURCEID ];
    SET_LENGTH( ptr, length - PN_HEADER_SIZE );
    SET_RECEIVER_OBJ( ptr, GET_SENDER_OBJ( aMsg ) );
    SET_SENDER_OBJ( ptr, GET_RECEIVER_OBJ( aMsg ) );
    // Transactionid. Set to 0x01 since this is the first.
    tempPtr.Append( msgTmpPtr[ ISI_HEADER_OFFSET_TRANSID ] );
    // Message ID
    tempPtr.Append( 0xF0 );
    // Sub message ID.
    tempPtr.Append( 0x14 );
    // Not Delivered Message from original req.
    tempPtr.Append( msgTmpPtr[ ISI_HEADER_OFFSET_MESSAGEID ] );
    // Status - COMM_ISA_ENTITY_NOT_AVAILABLE
    tempPtr.Append( 0x00 );
    // Filler
    tempPtr.Append( 0x00 );
    // Filler
    tempPtr.Append( 0x00 );
    // Filler
    tempPtr.Append( 0x00 );

//    ASSERT_RESET_ALWAYS( iConnectionStatus == EIADConnectionOk, EIADCmtConnectionLost | EIADFaultIdentifier2 << KFaultIdentifierShift );
    SendMsg( tempPtr );
    C_TRACE( ( _T( "DRouter::SendCommIsaEntityNotReachableResp 0x%x <-" ), &aMsg ) );

    OstTrace0( TRACE_NORMAL, DROUTER_SENDCOMMISAENTITYNOTREACHABLERESP_EXIT, "<DRouter::SendCommIsaEntityNotReachableResp" );
    }

void DRouter::InitCmtDfc(
        TAny* aPtr
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_INITCMTDFC_ENTRY, ">DRouter::InitCmtDfc;aPtr=%x", ( TUint )( aPtr ) );

    C_TRACE( ( _T( "DRouter::InitCmtDfc ->" ) ) );
    DRouter& tmp = *reinterpret_cast<DRouter*>( aPtr );
    if( !tmp.iBootDone )
        {
#ifdef NCP_COMMON_BRIDGE_FAMILY
#ifndef MODEM_MCE_DOES_NOT_WORK_AS_IT_SHOULD
        tmp.InitConnectionOk();
#endif // MODEM_MCE_DOES_NOT_WORK_AS_IT_SHOULD
#else
        tmp.InitCmtConnection();
#endif
        }

// ISCE    tmp.InitCmtConnection();
    C_TRACE( ( _T( "DRouter::InitCmtDfc <-" ) ) );

    OstTrace0( TRACE_NORMAL, DROUTER_INITCMTDFC_EXIT, "<DRouter::InitCmtDfc" );
    }

#ifdef NCP_COMMON_BRIDGE_FAMILY
void DRouter::InitConnectionOk()
    {

    C_TRACE( ( _T( "DRouter::InitConnectionOk ->" ) ) );
    iMaxFrameSize = KIADMaxIsiMsgSize;
    iConnectionStatus = EIADConnectionOk;
    iBootDone = ETrue;
    NotifyObjLayerConnStat( EIADConnectionOk );
    // Initialize channels to NULL when channel is opened !NULL.
    for( TInt i( 0 ); i < EIADSizeOfChannels; ++i )
        {
        ASSERT_RESET_ALWAYS( !iChannelTable[ i ].iChannel, EIADChannelOpenedBeforePhysicalLayerInit );
        C_TRACE( ( _T( "DRouter::InitConnectionOk %d" ), i ) );
        if( iChannelTable[ i ].iWaitingChannel )
            {
            switch( iChannelTable[ i ].iType )
                {
                case ENormalOpen:
                    {
                    C_TRACE( ( _T( "DRouter::InitConnectionOk booting ENormalOpen 0x%x" ), i ) );
                    MIAD2ChannelApi* tmpChannel = iChannelTable[ i ].iWaitingChannel;
                    iChannelTable[ i ].iChannel = tmpChannel;
                    iChannelTable[ i ].iWaitingChannel = NULL;
                    iChannelTable[ i ].iChannel->CompleteChannelRequest( EIADAsyncOpen, KErrNone );
                    break;
                    }
                default:
                    {
                    ASSERT_RESET_ALWAYS( 0, -1111 );
                    break;
                    }
                }
            }
        }
    C_TRACE( ( _T( "DRouter::InitConnectionOk <-" ) ) );
    }
#endif // NCP_COMMON_BRIDGE_FAMILY

void DRouter::InitCmtConnection(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DROUTER_INITCMTCONNECTION_ENTRY, ">DRouter::InitCmtConnection" );

    C_TRACE( ( _T( "DRouter::InitCmtConnection ->" ) ) );
#ifndef NCP_COMMON_BRIDGE_FAMILY

    // send configuration request to proxy
    const TInt KCtrlConfReqLength( 14 );
    //ASSERT_RESET_ALWAYS( iIST, EIADNullParameter | EIADFaultIdentifier16 << KFaultIdentifierShift  );
    TDes8& temp = AllocateBlock( KCtrlConfReqLength );
    ASSERT_RESET_ALWAYS( ( KCtrlConfReqLength > CTRL_REQ_OFFSET_MESSAGEID + 2 ), EIADOverTheLimits | EIADFaultIdentifier6 << KFaultIdentifierShift  );
    temp.SetLength( KCtrlConfReqLength );
    TUint8* tmpPtr( const_cast<TUint8*>( temp.Ptr() ) );
    tmpPtr[ ISI_HEADER_OFFSET_MEDIA ]                                       = PN_MEDIA_SOS;
    SET_RECEIVER_DEV( tmpPtr, PN_MEDIA_ROUTING_REQ );
    SET_SENDER_DEV( tmpPtr, THIS_DEVICE );
    tmpPtr[ ISI_HEADER_OFFSET_RESOURCEID ]                                  = PN_MEDIA_CONTROL;
    SET_LENGTH( tmpPtr, ( KCtrlConfReqLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( tmpPtr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( tmpPtr, KIADObjId );
    tmpPtr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_REQ_OFFSET_UTID ]          = 0x00;
    tmpPtr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_REQ_OFFSET_SUBFUNCTION ]   = PNS_MEDIA_SPECIFIC_REQ;
    tmpPtr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_REQ_OFFSET_MEDIATOCTRL ]   = PN_MEDIA_SOS;
    tmpPtr[ CTRL_REQ_OFFSET_MESSAGEID ]                                     = KIADCtrlConfReq;
    tmpPtr[ CTRL_REQ_OFFSET_MESSAGEID + 1 ]                                 = 0x00; // Filler
    tmpPtr[ CTRL_REQ_OFFSET_MESSAGEID +2 ]                                  = 0x00; // Filler
    temp.SetLength( KCtrlConfReqLength );
    SendMsg( temp );
#endif // NCP_COMMON_BRIDGE_FAMILY

    C_TRACE( ( _T( "DRouter::InitCmtConnection <-" ) ) );

    OstTrace0( TRACE_NORMAL, DROUTER_INITCMTCONNECTION_EXIT, "<DRouter::InitCmtConnection" );
    }

void DRouter::SendDrmReq(
        const TUint16 aChannelId
        )
    {
    OstTraceExt1( TRACE_NORMAL, DROUTER_SENDDRMREQ_ENTRY, ">DRouter::SendDrmReq;aChannelId=%hx", aChannelId );

    C_TRACE( ( _T( "DRouter::SendDrmReq 0x%x ->" ), aChannelId) );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    // DRM REQ/RESP messages are spec so that only 8-bit values for channelid are possible.
    ASSERT_RESET_ALWAYS( aChannelId <= 0xff, EIADTooManyBytesToPresent );
    TDes8& block = AllocateBlock( KIADCtrlDrmReqLength );
    ASSERT_RESET_ALWAYS( ( KIADCtrlDrmReqLength > CTRL_DRM_REQ_OFFSET_REQUIRED_CHANNEL_ID ), EIADOverTheLimits | EIADFaultIdentifier7 << KFaultIdentifierShift  );;
    block.SetLength( KIADCtrlDrmReqLength );
    TUint8* ptr( const_cast<TUint8*>( block.Ptr() ) );
    ptr[ ISI_HEADER_OFFSET_MEDIA ]                                      = PN_MEDIA_SOS;
    SET_RECEIVER_DEV( ptr, PN_MEDIA_ROUTING_REQ );
    SET_SENDER_DEV( ptr, THIS_DEVICE );
    ptr[ ISI_HEADER_OFFSET_RESOURCEID ]                                 = PN_MEDIA_CONTROL;
    SET_LENGTH( ptr, ( KIADCtrlDrmReqLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( ptr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( ptr, KIADObjId );
    ptr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_REQ_OFFSET_UTID ]         = aChannelId;
    ptr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_REQ_OFFSET_SUBFUNCTION ]  = PNS_MEDIA_SPECIFIC_REQ;
    ptr[ ISI_HEADER_SIZE + PNS_MEDIA_SPECIFIC_REQ_OFFSET_MEDIATOCTRL ]  = PN_MEDIA_SOS;
    ptr[ CTRL_REQ_OFFSET_MESSAGEID ]                                    = KIADCtrlDrmReq;
    ptr[ CTRL_DRM_REQ_OFFSET_REQUIRED_CHANNEL_ID ]                      = aChannelId;
    ASSERT_RESET_ALWAYS( iConnectionStatus == EIADConnectionOk, EIADCmtConnectionLost | EIADFaultIdentifier3 << KFaultIdentifierShift );
    SendMsg( block );
#endif // NCP_COMMON_BRIDGE_FAMILY
    C_TRACE( ( _T( "DRouter::SendDrmReq 0x%x <-" ), aChannelId) );

    OstTrace0( TRACE_NORMAL, DROUTER_SENDDRMREQ_EXIT, "<DRouter::SendDrmReq" );
    }

void DRouter::SendPnsNameAddReq( 
        const TUint16 aChannel, 
        const TDesC8& aOpenInfo
        )
    {
    OstTraceExt2( TRACE_NORMAL, DROUTER_SENDPNSNAMEADDREQ_ENTRY, ">DRouter::SendPnsNameAddReq;aChannel=%hx;aOpenInfo=%x", aChannel, ( TUint )&( aOpenInfo ) );

    C_TRACE( ( _T( "DRouter::SendPnsNameAddReq 0x%x 0x%x ->" ), aChannel, &aOpenInfo ) );
#ifndef NCP_COMMON_BRIDGE_FAMILY

    TUint16 msgLength( ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_REQ );
    TDes8& block = AllocateBlock( msgLength );
    ASSERT_RESET_ALWAYS( ( msgLength > ( ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 7 ) ), EIADOverTheLimits | EIADFaultIdentifier8 << KFaultIdentifierShift );
    block.SetLength( msgLength );
    TUint8* ptr = const_cast<TUint8*>( block.Ptr() );

    ptr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_SOS;
    SET_RECEIVER_DEV( ptr, OTHER_DEVICE_1 );
    SET_SENDER_DEV( ptr, THIS_DEVICE );
    ptr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
    SET_LENGTH( ptr, ( msgLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( ptr, PN_OBJ_ROUTER);
    SET_SENDER_OBJ( ptr, KIADObjId );
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_UTID ] = aChannel; 
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_SUBFUNCTION ] = PNS_NAME_ADD_REQ;
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED1 ] = 0x00;
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED2 ] = 0x00;

    TUint8* resourcePtr = const_cast<TUint8*>( aOpenInfo.Ptr() );
    /*
    //TBR
    for( TInt i( 0 ); i < aOpenInfo.Length(); i++ )
        {
        Kern::Printf( "resource[%d]0x%x", i, resourcePtr[i] );
        }
    //TBR
    */
    switch( aOpenInfo.Length() )
        {
        // Resource bigendian 32-bit, TODO : how about winscw?
        case 4:
            {
            C_TRACE( ( _T( "DRouter::SendPnsNameAddReq 32-bit resourceid used 0x%x 0x%x" ), aChannel, &aOpenInfo ) );
            OstTraceExt5( TRACE_NORMAL, DROUTER_SENDPNSNAMEADDREQ_32BIT, "DRouter::SendPnsNameAddReq;aChannel=%hx;resourcePtr[0]=%hhx;resourcePtr[1]=%hhx;resourcePtr[2]=%hhx;resourcePtr[3]=%hhx", aChannel, resourcePtr[0], resourcePtr[1], resourcePtr[2], resourcePtr[3] );
            
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY ] = resourcePtr[ 0 ];
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 1 ] = resourcePtr[ 1 ];
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 2 ] = resourcePtr[ 2 ];
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 3 ] = resourcePtr[ 3 ];
            break;
            }
        case 1:
            {
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY ] = 0x00;
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 1 ] = 0x00;
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 2 ] = 0x00;
            ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 3 ] = resourcePtr[ 0 ];
            OstTraceExt2( TRACE_NORMAL, DROUTER_SENDPNSNAMEADDREQ_8BIT, "DRouter::SendPnsNameAddReq;aChannel=%hx;resourcePtr[0]=%hhx", aChannel, resourcePtr[0] );
            C_TRACE( ( _T( "DRouter::SendPnsNameAddReq 8-bit resourceid used 0x%x 0x%x" ), aChannel, &aOpenInfo ) );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADWrongParameter | EIADFaultIdentifier17 << KFaultIdentifierShift  );
            break;
            }
        }
    // Phonet device id associated with name.
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 4 ] = THIS_DEVICE;
    // Phonet object id associated with name.
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 5 ] = aChannel;
    // Record flags. This entry cannot be changed.
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 6 ] = PN_NAME_NOCHG;
    ptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + 7 ] = 0x00;
    ASSERT_RESET_ALWAYS( iConnectionStatus == EIADConnectionOk, EIADCmtConnectionLost | EIADFaultIdentifier4 << KFaultIdentifierShift );
    SendMsg( block );
#endif // NCP_COMMON_BRIDGE_FAMILY
    C_TRACE( ( _T( "DRouter::SendPnsNameAddReq 0x%x 0x%x <-" ), aChannel, &aOpenInfo ) );

    OstTrace0( TRACE_NORMAL, DROUTER_SENDPNSNAMEADDREQ_EXIT, "<DRouter::SendPnsNameAddReq" );
    }

void DRouter::HandlePnsNameAddResp(
        TDes8& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_HANDLEPNSNAMEADDRESP_ENTRY, ">DRouter::HandlePnsNameAddResp;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DRouter::HandlePnsNameAddResp 0x%x ->" ), &aMsg ) );
#ifndef NCP_COMMON_BRIDGE_FAMILY

    // Channel id from Transaction ID.
    const TUint8* cptr( aMsg.Ptr() );
    ASSERT_RESET_ALWAYS( ( ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ) < aMsg.Length(), EIADOverTheLimits | EIADFaultIdentifier9 << KFaultIdentifierShift  );
    TUint8 channelId( cptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_UTID ] );
    ASSERT_RESET_ALWAYS( channelId < EIADSizeOfChannels, EIADWrongParameter | (TUint8)channelId << KChannelNumberShift | EIADFaultIdentifier23 << KFaultIdentifierShift );
    if( iChannelTable[ channelId ].iWaitingChannel )
        {
        ASSERT_RESET_ALWAYS( iChannelTable[ channelId ].iType == ENameAddOpen, EIADWrongTypeOfOpenPending | EIADFaultIdentifier2 << KFaultIdentifierShift );
        if( cptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] == PN_NAME_OK )
            {
            MIAD2ChannelApi* tmpChannel = iChannelTable[ channelId ].iWaitingChannel;
            iChannelTable[ channelId ].iChannel = tmpChannel;
            iChannelTable[ channelId ].iWaitingChannel = NULL;
            C_TRACE( ( _T( "DRouter::HandlePnsNameAddResp name add resp ok 0x%x 0x%x 0x%x 0x%x" ), channelId, tmpChannel, iChannelTable[ channelId ].iChannel, &aMsg ) );

            OstTraceExt2( TRACE_NORMAL, DROUTER_HANDLEPNSNAMEADDRESP_OK, "DRouter::HandlePnsNameAddResp OK;channelId=%hhx;tmpChannel=%x", channelId, (TUint) ( tmpChannel ) );
            
            iChannelTable[ channelId ].iChannel->CompleteChannelRequest( EIADAsyncOpen, KErrNone );
            }
        else
            {
            C_TRACE( ( _T( "DRouter::HandlePnsNameAddResp name add resp NOK 0x%x 0x%x" ), channelId, &aMsg ) );
            OstTraceExt1( TRACE_NORMAL, DROUTER_HANDLEPNSNAMEADDRESP_NOK, "DRouter::HandlePnsNameAddResp;channelId=%hhx", channelId );
            
            TRACE_ASSERT( cptr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] == PN_NAME_OK );
            iChannelTable[ channelId ].iWaitingChannel->CompleteChannelRequest( EIADAsyncOpen, KErrNotSupported );
            // Set channel open waiting to null, open was failed.
            iChannelTable[ channelId ].iWaitingChannel = NULL;
            }
        }
    else
        {
        // Check is this waiting
        // TODO: Does pns_name_add_resp need to be informed with comm_isa_entity_not_reachable_resp if channel is closed?
        TRACE_ASSERT_INFO( !iChannelTable[ channelId ].iChannel, channelId<<KChannelNumberShift );
        TRACE_ASSERT_INFO( iChannelTable[ channelId ].iWaitingChannel, channelId<<KChannelNumberShift ); // TODO: This will come when open cancel called with resource.
        }
    // De-allocate cause not going anywhere else.
    DeAllocateBlock( aMsg );
#endif // NCP_COMMON_BRIDGE_FAMILY

    C_TRACE( ( _T( "DRouter::HandlePnsNameAddResp 0x%x <-" ), &aMsg ) );

    OstTrace0( TRACE_NORMAL, DROUTER_HANDLEPNSNAMEADDRESP_EXIT, "<DRouter::HandlePnsNameAddResp" );
    }

// router and handler (pipe and indication)
TInt DRouter::SendMsg(
         TDes8& aMsg
         )
    {
    OstTrace1( TRACE_NORMAL, DROUTER_SENDMSG_ENTRY, ">DRouter::SendMsg;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( " DRouter::SendMsg 0x%x ->" ), &aMsg ) );    
    // The IST shall deallocate the block when it's approriate to do.    
    DATA_DUMP_TRACE( aMsg, ETrue );
    OstTraceData( TRACE_ISIMSG, DROUTER_SENDMSG_DATA, "DRouter::SendMsg TX: %{hex8[]}", aMsg.Ptr(), aMsg.Length() );

    //ISCE
    //TInt error = iIST->SendMessage( aMsg, MIAD2ISTApi::EISTPriorityDefault );// priority 0
    // TODO error codes and ids
    TUint8 linkId = MapMediaToLinkId( aMsg.Ptr()[ ISI_HEADER_OFFSET_MEDIA ] );
    ASSERT_RESET_ALWAYS( linkId < DRouter::EISIAmountOfMedias, -1000 );
    MISIRouterLinkIf* link = iLinksArray[ linkId ];
    ASSERT_RESET_ALWAYS( link, -999 );
    TInt error( KErrNone );
    if( link->TrxPresent() )
        {
        link->Send( aMsg );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        // Discard send block if connection lost
        MemApi::DeallocBlock( aMsg );
        error = KErrNotReady;
        }
    //ISCE
    OstTraceExt2( TRACE_NORMAL, DROUTER_SENDMSG_EXIT, "<DRouter::SendMsg;aMsg=%x;error=%d", ( TUint )&( aMsg ), error );
    return error;
    }

//pipehandler
MIAD2ChannelApi* DRouter::GetChannel(
        const TUint16 aChannel
        )
    {
    OstTraceExt1( TRACE_NORMAL, DROUTER_GETCHANNEL_ENTRY, ">DRouter::GetChannel;aChannel=%hx", aChannel );

    ASSERT_RESET_ALWAYS( aChannel < EIADSizeOfChannels, EIADWrongParameter | (TUint8)aChannel << KChannelNumberShift | EIADFaultIdentifier24 << KFaultIdentifierShift );
    
    MIAD2ChannelApi* channelApi = iChannelTable[ aChannel ].iChannel;

    OstTrace1( TRACE_NORMAL, DROUTER_GETCHANNEL_EXIT, "<DRouter::GetChannel;channelApi=%x", ( TUint )( channelApi ) );
    return channelApi;
    }
//pipehandler

void DRouter::SetSenderInfo(
         TDes8& aMessage,
         const TUint16 aCh
         )
    {
    OstTraceExt2( TRACE_NORMAL, DROUTER_SETSENDERINFO_ENTRY, ">DRouter::SetSenderInfo;aMessage=%x;aCh=%hx", ( TUint )&( aMessage ), aCh );

    C_TRACE( ( _T( "DRouter::SetSenderInfo 0x%x ->" ), &aMessage ) );    
    TUint8* msgBlockPtr = const_cast<TUint8*>( aMessage.Ptr() );
    ASSERT_RESET_ALWAYS( aMessage.Length() > ISI_HEADER_OFFSET_MEDIA, EIADOverTheLimits | EIADFaultIdentifier10 << KFaultIdentifierShift  );
    switch( aCh )
        {
#ifndef NCP_COMMON_BRIDGE_FAMILY
        case EIADNokiaBtPhonetLink:
            {
            OstTrace0( TRACE_NORMAL, DROUTER_SETSENDERINFO_BT, "DRouter::SetSenderInfo BT" );            
            msgBlockPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_BT;
            break;
            }
        case EIADNokiaUsbPhonetLink:
            {
            OstTrace0( TRACE_NORMAL, DROUTER_SETSENDERINFO_USB, "DRouter::SetSenderInfo USB" );            
            msgBlockPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_USB;
            break;
            }
#endif
        default:
            {
            msgBlockPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_SOS;
            SET_SENDER_DEV( msgBlockPtr, THIS_DEVICE );
            SET_RECEIVER_DEV( msgBlockPtr, OTHER_DEVICE_1 );
            SET_SENDER_OBJ( msgBlockPtr, aCh );
            break;
            }
        }
    C_TRACE( ( _T( "DRouter::SetSenderInfo 0x%x <-" ), &aMessage ) );        
    
    OstTrace0( TRACE_NORMAL, DROUTER_SETSENDERINFO_EXIT, "<DRouter::SetSenderInfo" );
    }
    
void DRouter::CheckSameThreadContext()
    {
    OstTrace0( TRACE_NORMAL, DROUTER_CHECKSAMETHREADCONTEXT_ENTRY, ">DRouter::CheckSameThreadContext" );

    DObject* tempObj = reinterpret_cast<DObject*>( &Kern::CurrentThread() ); 
    // If Null set current thread otherwise ignore
    iThreadPtr = !iThreadPtr ? tempObj : iThreadPtr;

    IAD_ASSERT_RESET_ALWAYS( ( iThreadPtr == tempObj ? ETrue : EFalse ) , -1112, "NOTSAMETHREAD" );
    
    OstTrace0( TRACE_NORMAL, DROUTER_CHECKSAMETHREADCONTEXT_EXIT, "<DRouter::CheckSameThreadContext" );
    }

// FM held, must not block and nor acquire locks.
void DRouter::NotifyTrxPresenceEnqueDfc(
        TBool aPresent
        )
    {

    C_TRACE( ( _T( "DRouter::NotifyTrxPresenceEnqueDfc %d ->" ), aPresent ) );
    if( !iBootDone )
        {
        iInitCmtDfc->Enque();
        }
    else
        {
        iConnectionStatus = ( aPresent ) ? EIADConnectionOk : EIADConnectionNotOk;// TODO: atomicity check
        iConnStatDfc->Enque();
        }
    C_TRACE( ( _T( "DRouter::NotifyTrxPresenceEnqueDfc %d <-" ), aPresent ) );

    }

void DRouter::Receive(
        TDes8& aMsg
        )
    {

    C_TRACE( ( _T( "DRouter::Receive 0x%x ->" ), &aMsg ) );
    MessageReceived( aMsg );
    C_TRACE( ( _T( "DRouter::Receive 0x%x <-" ), &aMsg ) );

    }

EXPORT_C void DRouter::DummyDoNothing()
    {

    ASSERT_RESET_ALWAYS( 0, -1001 );

    }

EXPORT_C void DRouter::DummyDoNothing2()
    {

    ASSERT_RESET_ALWAYS( 0, -1000 );

    }


TUint8 DRouter::MapMediaToLinkId(
        const TUint8 aMedia
        )
    {

    TUint8 linkdId( DRouter::EISIAmountOfMedias );
    switch( aMedia )
        {
        case PN_MEDIA_SOS:
            {
#ifndef NCP_COMMON_BRIDGE_FAMILY
            linkdId = EISIMediaSOS;
#else
            linkdId = EISIMediaHostSSI;
#endif
            break;
            }
        // Not supported media
        default:
            {
            ASSERT_RESET_ALWAYS( 0, -998 );
            break;
            }
        }
    return linkdId;

    }

// End of file.

