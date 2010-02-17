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


#include <kernel.h>                       // For Kern
#include "isicommunicationmanager.h"      // For DISICommunicationManager
#include "misiobjectrouterif.h"           // For MISIObjectRouterIf
#include "isicommunicationmanagertrace.h" // For C_TRACE, ASSERT_RESET.. and fault codes
#include "memapi.h"                       // For MemApi
#include <phonetisi.h>                    // For ISI_HEADER_SIZE
#include <pn_const.h>                     // For PN_OBJ_ROUTING_REQ...
#include "nsisi.h"                        // For PN_NAMESERVICE
#ifdef INTERNAL_FLAG_ISI_ROUTER_IN_USE
#include "isihelpers.h"                   // For SET_RECEIVER_OBJ...
#include "isirouter.h"                    // For GetDFCThread
#else
//#include "misiobjectrouterif.h"           // For MISIObjectRouterIf
#include "isaaccessextension.h"           // For DIsaAccessExtension

#include "iadhelpers.h"                   // For SET_RECEIVER_OBJ...
#endif
#include <iscnokiadefinitions.h>          // For THIS_DEVICE
#include "isiindicationhandler.h"         // For DISIIndicationHandler
#include "ape_commgrisi.h"                // For APE_COMMGR..
#include "misicommunicationmanagerif.h"   // For MISICommunicationManagerIf
#include <commisi.h>                      // For Common messages


// Faults
enum TISICommunicationManagerFaults
    {
    EISICommunicationManagerMemAllocFailure = 0x01,
    EISICommunicationManagerMemAllocFailure1,
    EISICommunicationManagerNullPointer,
    EISICommunicationManagerUnknownMessage,
    EISICommunicationManagerMutexCreateFailed,
    EISICommunicationManagerMutexWaitFailed,
    EISICommunicationManagerWrongMessageOrder,
    };


/* PUTB8 macro writes byte value to given address.
 * This macro is used mainly by other PUTBXX macros.
 */
#define PUTB8(p,v) \
    {(*(TUint8 *)(p)) = ((TUint8)(v));}


/* PUTB16 macro writes 16-bit value in Big Endian format
 * to given address. 16-bit value is written as two separate
 * bytes, and so this macro can write 16-bit value to whatever
 * address, regardless of the processor alignment restrictions
 */
#define PUTB16(p,v) \
    {PUTB8((p),(TUint16)(v)>>8); PUTB8((TUint8*)(p)+1,v);}


/* PUTB32 macro writes 32-bit value in Big Endian format
 * to given address. 32-bit value is written as four separate
 * bytes, and so this macro can write 32-bit value to whatever
 * address, regardless of the processor alignment restrictions
 */
#define PUTB32(p,v) \
    {PUTB16((p),(TUint32)(v)>>16); PUTB16((TUint8*)(p)+2,(TUint32)(v));}


/**
 *    Big Endian to local endian type
 */
 /* GETB8 macro returns byte value from given address.
 * This macro is used mainly by other GETBXX macros.
 */
#define GETB8(p) \
    (*(TUint8 *)(p))


/* GETB16 macro reads 16-bit value in Big Endian format
 * from given address. 16-bit value is read as two separate
 * bytes, and so this macro can read 16-bit value from whatever
 * address, regardless of the processor alignment restrictions
 */
#define GETB16(p) \
    (((TUint16) GETB8(p)<<8) | (TUint16) GETB8((TUint8 *)(p)+1))


/* GETB32 macro reads 32-bit value in Big Endian format
 * from given address. 32-bit value is read as four separate
 * bytes, and so this macro can read 32-bit value from whatever
 * address, regardless of the processor alignment restrictions
 */
#define GETB32(p) \
    (((TUint32) GETB16(p)<<16) | (TUint32) GETB16((TUint8 *)(p)+2))


DMutex* DISICommunicationManager::iCommunicationManagerMutex = NULL;
_LIT8( KCommunicationManagerMutex, "KCommunicationManagerMutex" );

const TUint32 KCommunicationManagerUID( 0x2002B3D0 );
const TUint8 KFiller( 0 );
const TInt KInitDfcPriority( 7 );
const TUint8 KDone( 0 );

DISICommunicationManager::DISICommunicationManager(
        // None
        )
    : iObjId( 0x00 ),
      iBootMsgCount( 0x04 ) // 2xRESP, 2xIND
    {
    C_TRACE( ( _T( "DISICommunicationManager::DISICommunicationManager 0x%x 0x%x>" ), iObjId, iRouter ) );
    iRouter = MISIObjectRouterIf::Connect( KCommunicationManagerUID, iObjId, this );
    ASSERT_RESET_ALWAYS( iRouter, ( EISICommunicationManagerNullPointer | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    // Must be > KMutexOrdGeneral1 for nesting purposes because trx shared memory uses KMutexOrdGeneral1
    TInt err( Kern::MutexCreate( iCommunicationManagerMutex, KCommunicationManagerMutex, KMutexOrdGeneral2 ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISICommunicationManagerMutexCreateFailed | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
#ifdef INTERNAL_FLAG_ISI_ROUTER_IN_USE    
    iInitDfc = new TDfc( InitDfc, this, iRouter->GetDfcThread( MISIObjectRouterIf::EISIKernelMainThread ), KInitDfcPriority );
#else
    iInitDfc = new TDfc( InitDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KInitDfcPriority );
#endif    
    ASSERT_RESET_ALWAYS( iInitDfc, ( EISICommunicationManagerMemAllocFailure | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    iISIIndicationHandler = new DISIIndicationHandler( iRouter );
    ASSERT_RESET_ALWAYS( iISIIndicationHandler, ( EISICommunicationManagerMemAllocFailure1 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    iInitDfc->Enque();
    C_TRACE( ( _T( "DISICommunicationManager::DISICommunicationManager 0x%x 0x%x<" ), iObjId, iRouter ) );
    }

void DISICommunicationManager::InitDfc(
        TAny* aPtr  // Pointer to this object.
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::InitDfc 0x%x>" ), aPtr ) );
    DISICommunicationManager* tmp = reinterpret_cast<DISICommunicationManager*>( aPtr );
    tmp->SendNameAddReqs();
    C_TRACE( ( _T( "DISICommunicationManager::InitDfc<" ) ) );
    }

void DISICommunicationManager::SendNameAddReqs()
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendNameAddReqs>" ) ) );
    //PN_COMMGR name
    TUint16 msgLength( ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_REQ );
    TDes8& addMsg = MemApi::AllocBlock( msgLength );
    addMsg.SetLength( msgLength );
    TUint8* addPtr = const_cast<TUint8*>( addMsg.Ptr() );
    addPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
    SET_RECEIVER_DEV( addPtr, PN_DEV_OWN );
    SET_SENDER_DEV( addPtr, PN_DEV_OWN );
    addPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
    SET_LENGTH( addPtr, ( msgLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( addPtr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( addPtr, PN_OBJ_EVENT_MULTICAST );
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_UTID ] = 0x00;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_SUBFUNCTION ] = PNS_NAME_ADD_REQ;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED1 ] = KFiller;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED2 ] = KFiller;
    PUTB32( &addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], PN_COMMGR );
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ] = PN_DEV_OWN;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_OBJ ] = PN_OBJ_EVENT_MULTICAST;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ] = PN_NAME_UNDEF;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ] = KFiller;
    //PN_APE_COMMGR name
    TDes8& addMsg2 = MemApi::AllocBlock( msgLength );
    addMsg2.SetLength( msgLength );
    addMsg2.Copy( addMsg );
    TUint8* addPtr2 = const_cast<TUint8*>( addMsg2.Ptr() );
    PUTB32( &addPtr2[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], PN_APE_COMMGR );
    //Register PN_APE_COMMGR and PN_COMMGR
    iRouter->Send( addMsg, PN_OBJ_EVENT_MULTICAST );
    iRouter->Send( addMsg2, PN_OBJ_EVENT_MULTICAST );
    C_TRACE( ( _T( "DISICommunicationManager::SendNameAddReqs<" ) ) );    
    }


DISICommunicationManager::~DISICommunicationManager(
        // None
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::~DISICommunicationManager>" ) ) );
    if( iISIIndicationHandler )
        {
        delete iISIIndicationHandler;
        iISIIndicationHandler = NULL;
        }
    iInitDfc->Cancel();
    delete iInitDfc;
    iInitDfc = NULL;
    if( iCommunicationManagerMutex )
        {
        ((DObject*)iCommunicationManagerMutex)->Close( NULL );
        }
    iCommunicationManagerMutex = NULL;
    C_TRACE( ( _T( "DISICommunicationManager::~DISICommunicationManager<" ) ) );
    }


void DISICommunicationManager::Receive( const TDesC8& aMessage )
    {
    C_TRACE( ( _T( "DISICommunicationManager::Receive 0x%x>" ), &aMessage ) );
    TInt err( Kern::MutexWait( *iCommunicationManagerMutex ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISICommunicationManagerMutexWaitFailed | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    const TUint8* msgPtr( aMessage.Ptr() );
    TDes8* blockPtr = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMessage) );
    if( iBootMsgCount == KDone )
        {
        if( msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] == COMMON_MESSAGE )
            {
            switch( msgPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ] )
                {
                case COMM_ISA_ENTITY_NOT_REACHABLE_RESP:
                    {
                    C_TRACE( ( _T( "DISICommunicationManager::Received not reachable resp" ) ) );
                    break;
                    }
                case COMM_ISI_VERSION_GET_REQ:
                    {
                    C_TRACE( ( _T( "DISICommunicationManager::Received version get req" ) ) );
                    SendCommIsiVersionGetResp( *blockPtr );
                    break;
                    }
                default:
                    {
                    C_TRACE( ( _T( "DISICommunicationManager::Received unknown common message" ) ) );
                    SendCommServiceNotIdentifiedResp( *blockPtr );
                    break;
                    }
                }
            }
        else if( ( msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_APE_COMMGR )
            || ( msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_COMMGR ) )
            {
            C_TRACE( ( _T( "DISICommunicationManager::Receive subscription" ) ) );
            SendPnsSubscribeResp( *blockPtr );
            if( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN )
                {
       	        C_TRACE( ( _T( "DISICommunicationManager PNS_SUBSCRIBE_REQ from APE" ) ) );
                iISIIndicationHandler->Subscribe( *blockPtr );
                }
            }
        else
            {
            if( ( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] != PN_DEV_OWN )
                || ( MISICommunicationManagerIf::IsValidResource( *blockPtr ) ) )
                {
                C_TRACE( ( _T( "DISICommunicationManager::Receive indication" ) ) );
                iISIIndicationHandler->Multicast( *blockPtr );
                }
            else
                {
                C_TRACE( ( _T( "DISICommunicationManager not allowed resource from APE" ) ) );
                }
            }
        }
    else
        {
        C_TRACE( ( _T( "DISICommunicationManager not indication %d" ), iBootMsgCount ) );
        // From PN_NAMESERVICE && ( IND || successfull RESP )
        if( ( msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_NAMESERVICE )
            && ( msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_SUBFUNCTION ]== PNS_NAME_ADD_IND
            || ( msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_SUBFUNCTION ]== PNS_NAME_ADD_RESP 
            && msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] == PN_NAME_OK ) ) )
            {
            C_TRACE( ( _T( "DISICommunicationManager::Receive from NAMESERVICE message id 0x%x" ), msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_SUBFUNCTION ]  ) );
            iBootMsgCount--;
            }
        else
            {
            ASSERT_RESET_ALWAYS( 0, ( EISICommunicationManagerWrongMessageOrder | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
            }
        }
    MemApi::DeallocBlock( *blockPtr );
    Kern::MutexSignal( *iCommunicationManagerMutex );
    C_TRACE( ( _T( "DISICommunicationManager::Receive<" ) ) );
    }

void DISICommunicationManager::SendPnsSubscribeResp( const TDesC8& aMessage )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendPnsSubscribeResp 0x%x>" ), &aMessage ) );
    TUint16 msgLength( ISI_HEADER_SIZE + SIZE_APE_COMMGR_SUBSCRIBE_RESP );
    TDes8& respMsg = MemApi::AllocBlock( msgLength );
    respMsg.SetLength( msgLength );
    TUint8* msgPtr = const_cast<TUint8*>( aMessage.Ptr() );
    TUint8* respPtr = const_cast<TUint8*>( respMsg.Ptr() );
    respPtr[ ISI_HEADER_OFFSET_MEDIA ] = msgPtr[ ISI_HEADER_OFFSET_MEDIA ];
    SET_RECEIVER_DEV( respPtr, GET_SENDER_DEV( aMessage ) );
    SET_SENDER_DEV( respPtr, PN_DEV_OWN );
    respPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_APE_COMMGR;
    SET_LENGTH( respPtr, ( msgLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( respPtr, GET_SENDER_OBJ( aMessage ) );
    SET_SENDER_OBJ( respPtr, PN_OBJ_ROUTER );
    respPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_TRANSID ] = msgPtr[ ISI_HEADER_OFFSET_TRANSID ];
    respPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_MESSAGEID ] = APE_COMMGR_SUBSCRIBE_RESP;
    if( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN )
        {
        respPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_RESULT ] = APE_COMMGR_OK;
        }
    else
        {
        respPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_RESULT ] = APE_COMMGR_NOT_ALLOWED;	
        }    
    iRouter->Send( respMsg, PN_OBJ_EVENT_MULTICAST );
	  C_TRACE( ( _T( "DISICommunicationManager::SendPnsSubscribeResp<" ) ) );
    }


void DISICommunicationManager::SendCommServiceNotIdentifiedResp( const TDesC8& aMessage )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendCommServiceNotIdentifiedResp 0x%x>" ), &aMessage ) );
    TDes8& respMsg = MemApi::AllocBlock( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP );
    TUint8* ptr( const_cast<TUint8*>( respMsg.Ptr() ) );
    const TUint8* msgPtr( aMessage.Ptr() );
    ptr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
    SET_RECEIVER_DEV( ptr, GET_SENDER_DEV( aMessage ) );
    SET_SENDER_DEV( ptr, PN_DEV_OWN );
    ptr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_APE_COMMGR;
    SET_RECEIVER_OBJ( ptr, GET_SENDER_OBJ( aMessage ) );
    SET_SENDER_OBJ( ptr, PN_OBJ_EVENT_MULTICAST );
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_TRANSACTIONID ] = 0x00;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_MESSAGEID ] = COMMON_MESSAGE;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_SUBMESSAGEID ] = COMM_SERVICE_NOT_IDENTIFIED_RESP;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDMESSAGEID ] = msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ];
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE1 ] = KFiller;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDSUBMESSAGEID ] = msgPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ];
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE2 ] = KFiller;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE3 ] = KFiller;
    iRouter->Send( respMsg, PN_OBJ_EVENT_MULTICAST );
    C_TRACE( ( _T( "DISICommunicationManager::SendCommServiceNotIdentifiedResp<" ) ) );
    }


void DISICommunicationManager::SendCommIsiVersionGetResp( const TDesC8& aMessage )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendCommIsiVersionGetResp 0x%x>" ), &aMessage ) );
    TDes8& respMsg = MemApi::AllocBlock( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP );
    TUint8* ptr( const_cast<TUint8*>( respMsg.Ptr() ) );
    const TUint8* msgPtr( aMessage.Ptr() );
    ptr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
    SET_RECEIVER_DEV( ptr, GET_SENDER_DEV( aMessage ) );
    SET_SENDER_DEV( ptr, PN_DEV_OWN );
    ptr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_APE_COMMGR;
    SET_RECEIVER_OBJ( ptr, GET_SENDER_OBJ( aMessage ) );
    SET_SENDER_OBJ( ptr, PN_OBJ_EVENT_MULTICAST );
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_TRANSACTIONID ] = 0x00;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_MESSAGEID ] = COMMON_MESSAGE;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_SUBMESSAGEID ] = COMM_ISI_VERSION_GET_RESP;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONZZZ ] = APE_COMMUNICATION_MANAGER_SERVER_ISI_VERSION_Z;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONYYY ] = APE_COMMUNICATION_MANAGER_SERVER_ISI_VERSION_Y;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE1 ] = KFiller;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE2 ] = KFiller;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE3 ] = KFiller;
    iRouter->Send( respMsg, PN_OBJ_EVENT_MULTICAST );
    C_TRACE( ( _T( "DISICommunicationManager::SendCommIsiVersionGetResp<" ) ) );
    }


DECLARE_STANDARD_EXTENSION()
    {
    Kern::Printf( "ISI Communication Manager extension>" );
    // Create a container extension
    DISICommunicationManager* extension = new DISICommunicationManager();
    TRACE_ASSERT( extension );
    Kern::Printf( "ISI Communication Manager extension<" );
    return extension ? KErrNone : KErrNoMemory;
    }


