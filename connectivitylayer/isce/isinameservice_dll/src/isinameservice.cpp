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


#include <kernel.h>               // For Kern
#include "isinameservice.h"       // For DISINameService
#include "misiobjectrouterif.h"   // For MISIObjectRouterIf
#include "isinameservicetrace.h"  // For C_TRACE, ASSERT_RESET.. and fault codes
#include "memapi.h"               // For MemApi
#include <phonetisi.h>            // For ISI_HEADER_SIZE
#include <pn_const.h>             // For PN_NAMESERVICE
#include <nsisi.h>                // For PN

#ifdef INTERNAL_FLAG_ISI_ROUTER_IN_USE
#include "isihelpers.h"           // For SET_RECEIVER_OBJ...
#else
#include "iadhelpers.h"           // For SET_RECEIVER_OBJ...
#endif
#include "namerecords.h"          // For DNameRecords
#include <iscnokiadefinitions.h>  // For THIS_DEVICE
#include <commisi.h>              // For SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP
// Faults
enum TISINameServiceFaults
    {
    EISINameServiceMemAllocFailure = 0x01,
    EISINameServiceNullPointer,
    EISINameServiceUnknownMessage,
    EISINameServiceMutexCreateFailed,
    EISINameServiceMutexWaitFailed,
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


const TUint32 KNameServiceUID( 0x2002A5A1 );
const TUint8 KFiller( 0 );

DISINameService* DISINameService::iThisptr = NULL;
DMutex* DISINameService::iNameServiceMutex = NULL;
_LIT8( KNameServiceMutex, "KNameServiceMutex" );


DISINameService::DISINameService(
        // None
        )
    : iObjId( 0x00 )
    {
    C_TRACE( ( _T( "DISINameService::DISINameService 0x%x 0x%x>" ), iObjId, iRouter ) );
    iRouter = MISIObjectRouterIf::Connect( KNameServiceUID, iObjId, this );
    ASSERT_RESET_ALWAYS( iRouter, ( EISINameServiceNullPointer | EDISINameServiceTraceId << KClassIdentifierShift ) );
    iNameRecords = new DNameRecords();
    ASSERT_RESET_ALWAYS( iNameRecords, ( EISINameServiceMemAllocFailure | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TInt err( Kern::MutexCreate( iNameServiceMutex, KNameServiceMutex, KMutexOrdGeneral0 ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISINameServiceMutexCreateFailed | EDISINameServiceTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISINameService::DISINameService 0x%x 0x%x<" ), iObjId, iRouter ) );
    }

DISINameService::~DISINameService(
        // None
        )
    {
    C_TRACE( ( _T( "DISINameService::~DISINameService>" ) ) );
    if( iNameRecords )
        {
        delete iNameRecords;
        iNameRecords = NULL;
        }

    C_TRACE( ( _T( "DISINameService::~DISINameService<" ) ) );
    }


void DISINameService::Receive( const TDesC8& aMessage )
    {
    C_TRACE( ( _T( "DISINameService::Receive 0x%x>" ), &aMessage ) );
    TInt err( Kern::MutexWait( *iNameServiceMutex ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISINameServiceMutexWaitFailed | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint8* msgPtr( (TUint8*)aMessage.Ptr() );

    TUint8 resource = msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ];
    if(  resource == PN_NAMESERVICE )
        {
        C_TRACE( ( _T( "DISINameService message to PN_NAMESERVICE" ) ) );
        HandlePNSNameMessage( aMessage );
        }
    else
        {
        //RouterByResource( aMessage );

        C_TRACE( ( _T( "DISINameService message to RESOURCE : %d" ),resource  ) );
        //look for nametable by resource in message
        TUint16 phonetAddress = 0;
        TInt32 error = iNameRecords->LookupPhonetAddress(resource, &phonetAddress);

        if( KErrNone == error  )
            {
            //change phonet addr to current message from table entries


            msgPtr[ISI_HEADER_OFFSET_RECEIVERDEVICE] = (TUint8)( (phonetAddress & 0xFF00) >> 8);
            msgPtr[ISI_HEADER_OFFSET_RECEIVEROBJECT] = (TUint8) (phonetAddress & 0xFF);
            TDes8* messagePtr = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMessage) );

            C_TRACE( ( _T( "<-DISINameService message to recDev: 0x%x" ),msgPtr[ISI_HEADER_OFFSET_RECEIVERDEVICE]  ) );
            C_TRACE( ( _T( "<-DISINameService message to recObj: 0x%x" ),msgPtr[ISI_HEADER_OFFSET_RECEIVEROBJECT]  ) );

            //send data to router for handling
            iRouter->Send( *messagePtr, iObjId);
            }
        else
            {
            //construct not reachable resp message
            C_TRACE( ( _T( "DISINameService::Receive **ELSE** !!" )  ) );
            BuildAndSendCommIsaEntityNotReachableResp(aMessage);
            }
        }
    Kern::MutexSignal( *iNameServiceMutex );
    C_TRACE( ( _T( "DISINameService::DISINameService<" ) ) );
    }
void DISINameService::HandleNameQueryResp( const TDesC8& aMessage, const TUint8* msgPtr )
    {
    C_TRACE( ( _T( "DISINameService::HandleNameQueryResp>" ) ) );
    TUint32 name = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_NAME ] );
    TUint32 mask = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_BITMASK ] );
    RArray <TNameTable*> nameTable;
    TInt32 count = iNameRecords->NameQuery( name, mask, &nameTable );
    TUint16 msgLength = ( SIZE_PNS_NAME_QUERY_RESP + ( count * SIZE_PN_NAME_SRV_ITEM_STR ) );
    TDes8& respMsg = MemApi::AllocBlock( msgLength );
    respMsg.SetLength( msgLength );
    TUint8* respPtr = const_cast<TUint8*>( respMsg.Ptr() );

    respPtr[ ISI_HEADER_OFFSET_MEDIA ] = msgPtr[ ISI_HEADER_OFFSET_MEDIA ];
    SET_RECEIVER_DEV( respPtr, msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] );
    SET_SENDER_DEV( respPtr, PN_DEV_OWN );
    respPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
    SET_LENGTH( respPtr, ( msgLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( respPtr, msgPtr[ ISI_HEADER_OFFSET_SENDEROBJECT ] );
    SET_SENDER_OBJ( respPtr, PN_OBJ_ROUTER );
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_UTID ] = msgPtr[ ISI_HEADER_OFFSET_TRANSID ];
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_SUBFUNCTION ]= PNS_NAME_QUERY_RESP;
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_TOTALMATCHES ] = count >> 8;
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_TOTALMATCHES + 1 ] = count;
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_MATCHESINMSG ] = count >> 8;
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_MATCHESINMSG + 1 ] = count;
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_RESERVED1 ] = KFiller;
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_RESERVED2 ] = KFiller;

    TUint16 index = ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_NAMEENTRYTBL;
    for( int i = 0; i < count; i++)
        {
        //AddNameTable();
        TNameTable* tmp = (nameTable)[ i ];

        // Name
        respPtr[ index++ ] = (TUint8)(tmp->iName >> 24);
        respPtr[ index++ ] = (TUint8)(tmp->iName >> 16);
        respPtr[ index++ ] = (TUint8)(tmp->iName >> 8);
        respPtr[ index++ ] = (TUint8)(tmp->iName);
        // DevId
        respPtr[ index++ ] = (TUint8)(tmp->iPhonetAddress >> 8);
        // ObjId
        respPtr[ index++ ] = (TUint8)(tmp->iPhonetAddress);
        // Record flags
        respPtr[ index++ ] = tmp->iFlags;
        respPtr[ index++ ] = KFiller; // Reserved
        }

    iRouter->Send( respMsg, iObjId );
    TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMessage) );
    MemApi::DeallocBlock( *block );
    C_TRACE( ( _T( "DISINameService::HandleNameQueryResp<" ) ) );
    }

void DISINameService::HandleNameRemoveResp( const TDesC8& aMessage, const TUint8* msgPtr )
    {
    C_TRACE( ( _T( "DISINameService::HandleNameRemoveResp>" ) ) );
    TUint32 name = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_REQ_OFFSET_NAME ] );

    TInt8 err = iNameRecords->RemoveName( name );
    C_TRACE( ( _T( "DISINameService PNS_NAME_REMOVE_RESP 0x%x>" ), name ) );

    TUint16 msgLength( ISI_HEADER_SIZE + SIZE_PNS_NAME_REMOVE_RESP );
    TDes8& respMsg = MemApi::AllocBlock( msgLength );
    respMsg.SetLength( msgLength );
    TUint8* respPtr = const_cast<TUint8*>( respMsg.Ptr() );

    respPtr[ ISI_HEADER_OFFSET_MEDIA ] = msgPtr[ ISI_HEADER_OFFSET_MEDIA ];
    SET_RECEIVER_DEV( respPtr, msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] );
    SET_SENDER_DEV( respPtr, PN_DEV_OWN );
    respPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
    SET_LENGTH( respPtr, ( msgLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( respPtr, msgPtr[ ISI_HEADER_OFFSET_SENDEROBJECT ] );
    SET_SENDER_OBJ( respPtr, PN_OBJ_ROUTER );
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_RESP_OFFSET_UTID ] = msgPtr[ ISI_HEADER_OFFSET_TRANSID ];
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_RESP_OFFSET_SUBFUNCTION ] = PNS_NAME_REMOVE_RESP;
    if( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN )
        {
        respPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_RESP_OFFSET_REASON ] = err;
        iRouter->Send( respMsg, iObjId );

        if( PN_NAME_OK == err )
            {
            C_TRACE( ( _T( "DISINameService PNS_NAME_REMOVE_IND>" ) ) );
            msgLength = ( SIZE_PNS_NAME_REMOVE_IND + PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL );
            TDes8& indMsg = MemApi::AllocBlock( msgLength );
            indMsg.SetLength( msgLength );
            TUint8* indPtr = const_cast<TUint8*>( indMsg.Ptr() );

            indPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
            SET_RECEIVER_DEV( indPtr, msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] );
            SET_SENDER_DEV( indPtr, PN_DEV_OWN );
            indPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
            SET_LENGTH( indPtr, ( msgLength - PN_HEADER_SIZE ) );
            SET_RECEIVER_OBJ( indPtr, PN_OBJ_EVENT_MULTICAST ); //communicationmanager
            SET_SENDER_OBJ( indPtr, PN_OBJ_ROUTER );
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_UTID ] = 0x00;
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_SUBFUNCTION ]= PNS_NAME_REMOVE_IND;
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_TOTALMATCHES ] = 0x00;
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_TOTALMATCHES + 1 ] = 0x01;
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_MATCHESINMSG ] = 0x00;
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_MATCHESINMSG + 1 ] = 0x01;
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_RESERVED1 ] = KFiller;
            indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_RESERVED2 ] = KFiller;
            PUTB32( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], name );

            iRouter->Send( indMsg, iObjId );
            TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMessage) );
            MemApi::DeallocBlock( *block );
            C_TRACE( ( _T( "DISINameService PNS_NAME_QUERY_IND<" ) ) );
            }
        C_TRACE( ( _T( "DISINameService PNS_NAME_REMOVE_RESP<" ) ) );
        }
    else
        {
        C_TRACE( ( _T( "DISINameService::HandlePNSNameMessage PNS_NAME_REMOVE_REQ from other device, PN_NAME_NOT_ALLOWED" ) ) );
        respPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_RESP_OFFSET_REASON ] = PN_NAME_NOT_ALLOWED;
        iRouter->Send( respMsg, iObjId );
        }
    C_TRACE( ( _T( "DISINameService::HandleNameRemoveResp<" ) ) );
    }

void DISINameService::HandlePNSNameMessage( const TDesC8& aMessage )
    {
    C_TRACE( ( _T( "DISINameService::HandlePNSNameMessage 0x%x>" ), &aMessage ) );
    const TUint8* msgPtr( aMessage.Ptr() );
    TInt8 err( KErrArgument );
    switch( msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] )
        {
        case PNS_NAME_QUERY_REQ:
            {
            C_TRACE( ( _T( "DISINameService PNS_NAME_QUERY_REQ>" ) ) );
            HandleNameQueryResp( aMessage, msgPtr );
            C_TRACE( ( _T( "DISINameService PNS_NAME_QUERY_REQ<" ) ) );
            break;
            }
        case PNS_NAME_ADD_REQ:
            {
            C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_REQ>" ) ) );

            TUint16 msgLength( ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_RESP );
            TDes8& respMsg = MemApi::AllocBlock( msgLength );
            respMsg.SetLength( msgLength );
            TUint8* respPtr = const_cast<TUint8*>( respMsg.Ptr() );

            respPtr[ ISI_HEADER_OFFSET_MEDIA ] = msgPtr[ ISI_HEADER_OFFSET_MEDIA ];
            SET_RECEIVER_DEV( respPtr, msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] );
            SET_SENDER_DEV( respPtr, PN_DEV_OWN );
            respPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
            SET_LENGTH( respPtr, ( msgLength - PN_HEADER_SIZE ) );
            SET_RECEIVER_OBJ( respPtr, msgPtr[ ISI_HEADER_OFFSET_SENDEROBJECT ] );
            SET_SENDER_OBJ( respPtr, PN_OBJ_ROUTER );
            respPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_UTID ] = msgPtr[ ISI_HEADER_OFFSET_TRANSID ];
            respPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_SUBFUNCTION ] = PNS_NAME_ADD_RESP;

            if( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN )
                {
                C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_REQ from this device" ) ) );
                TUint32 name = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY ] );
                TUint16 phonetAddr = GETB16( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ] );
                TUint8 flags = msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ];
                err = iNameRecords->AddName( name, phonetAddr, flags );
                respPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] = err;
                iRouter->Send( respMsg, iObjId );
                C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_RESP from this device<" ) ) );
                if( err == PN_NAME_OK )
                    {
                    C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_IND>" ) ) );
                    msgLength = ( SIZE_PNS_NAME_ADD_IND + ( SIZE_PN_NAME_SRV_ITEM_STR ) );
                    TDes8& indMsg = MemApi::AllocBlock( msgLength );
                    indMsg.SetLength( msgLength );
                    TUint8* indPtr = const_cast<TUint8*>( indMsg.Ptr() );
                    indPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
                    SET_RECEIVER_DEV( indPtr, msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] );
                    SET_SENDER_DEV( indPtr, PN_DEV_OWN );
                    indPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
                    SET_LENGTH( indPtr, ( msgLength - PN_HEADER_SIZE ) );
                    SET_RECEIVER_OBJ( indPtr, PN_OBJ_EVENT_MULTICAST ); //communicationmanager
                    SET_SENDER_OBJ( indPtr, PN_OBJ_ROUTER );
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_UTID ] = 0x00;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_SUBFUNCTION ]= PNS_NAME_ADD_IND;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_TOTALMATCHES ] = 0x00;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_TOTALMATCHES + 1 ] = 0x01;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_MATCHESINMSG ] = 0x00;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_MATCHESINMSG + 1 ] = 0x01;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_RESERVED1 ] = KFiller;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_RESERVED2 ] = KFiller;
                    PUTB32( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], name );
                    PUTB16( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ], phonetAddr );
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ] = flags;
                    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ] = KFiller;
                    iRouter->Send( indMsg, iObjId );
                    C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_IND<" ) ) );
                    }
                }
            else
                {
                C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_REQ from other device>" ) ) );
                respPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] = PN_NAME_NOT_ALLOWED;
                iRouter->Send( respMsg, iObjId );
                C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_RESP from other device<" ) ) );
                }
            TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMessage) );
            MemApi::DeallocBlock( *block );
            C_TRACE( ( _T( "DISINameService PNS_NAME_ADD_REQ<" ) ) );
            break;
            }
        case PNS_NAME_REMOVE_REQ:
            {
            C_TRACE( ( _T( "DISINameService PNS_NAME_REMOVE_REQ>" ) ) );
            HandleNameRemoveResp( aMessage, msgPtr );
            C_TRACE( ( _T( "DISINameService PNS_NAME_REMOVE_REQ<" ) ) );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, ( EISINameServiceUnknownMessage | EDISINameServiceTraceId << KClassIdentifierShift ) );
            C_TRACE( ( _T( "DISINameService::HandlePNSNameMessage unknown NAMESERVICE message" )  ) );
            }
        }
    C_TRACE( ( _T( "DISINameService::HandlePNSNameMessage<" ) ) );
    }


EXPORT_C TBool MISICommunicationManagerIf::IsValidResource(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISINameService::IsValidResource 0x%x>" ), &aMessage ) );
    TInt err( Kern::MutexWait( *DISINameService::iThisptr->iNameServiceMutex ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISINameServiceMutexWaitFailed | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint8* msgPtr = const_cast<TUint8*>( aMessage.Ptr() );
    TUint16 phonetAddress( 0 );
    DISINameService::iThisptr->iNameRecords->LookupPhonetAddress( msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ], &phonetAddress );
    Kern::MutexSignal( *DISINameService::iThisptr->iNameServiceMutex );
    if( phonetAddress == GET_SENDER_OBJ( aMessage ) )
        {
        C_TRACE( ( _T( "DISINameService::IsValidResource phonetAddress found 0x%x" ), phonetAddress ) );
        return ETrue;
        }
    else
        {
        C_TRACE( ( _T( "DISINameService::IsValidResource phonetAddress not found 0x%x" ), phonetAddress ) );
        return EFalse;
        }
    }


DECLARE_STANDARD_EXTENSION()
    {
    Kern::Printf( "ISI Name Service extension>" );
    // Create a container extension
    DISINameService* extension = new DISINameService();
    TRACE_ASSERT( extension );
    DISINameService::iThisptr = static_cast< DISINameService* >( extension );
    Kern::Printf( "ISI Name Service extension<" );
    return extension ? KErrNone : KErrNoMemory;
    }

void DISINameService::BuildAndSendCommIsaEntityNotReachableResp(
        const TDesC8& aMsg
        )
    {
    C_TRACE( ( _T( "DISINameService::ConstructCommIsaEntityNotReachableResp 0x%x ->" ), &aMsg ) );
    // Make channel opening request followinfg COMM specification: 000.026
    // Length is sixteen bytes.
    TUint8 length( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP );
    TDes8& tempPtr = MemApi::AllocBlock( length );
    ASSERT_RESET_ALWAYS( &tempPtr, EISINameServiceMemAllocFailure  ); //lisää jotakin infoa jotta löytyy ilman tracea...pelkän luvun perusteella??
    TUint8* ptr = const_cast<TUint8*>( tempPtr.Ptr() );
    // We start to append from transaction id.
    tempPtr.SetLength( SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP );
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
    tempPtr.Append( COMMON_MESSAGE ); // 0xF0
    // Sub message ID.
    tempPtr.Append( COMM_ISA_ENTITY_NOT_REACHABLE_RESP ); // 0x14
    // Not Delivered Message from original req.
    tempPtr.Append( msgTmpPtr[ ISI_HEADER_OFFSET_MESSAGEID ] );
    // Status - COMM_ISA_ENTITY_NOT_EXISTING
    tempPtr.Append( COMM_ISA_ENTITY_NOT_EXISTING); //0x01
    // Filler
    tempPtr.Append( 0x00 );
    // Filler
    tempPtr.Append( 0x00 );
    // Filler
    tempPtr.Append( 0x00 );

    //send data to router for handling
    iRouter->Send( tempPtr, iObjId);

    TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMsg) );
    MemApi::DeallocBlock( *block );
    C_TRACE( ( _T( "DISINameService::SendCommIsaEntityNotReachableResp 0x%x <-" ), &aMsg ) );
    }


