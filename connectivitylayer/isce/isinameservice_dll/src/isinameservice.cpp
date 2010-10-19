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
#include "misiobjectrouterif.h"   // For MISIObjectRouterIf
#include "isihelpers.h"           // For SET_RECEIVER_OBJ...
#include "namerecords.h"          // For DNameRecords
#include "routerisi.h"            // For PN_ROUTER
#include <commisi.h>              // For SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP

#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
#include "ape_commgrisi.h"          // For APE_COMMGR
#define ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
//#include "isinameserviceextended.h" // For DISINameServiceExtended
#endif

// Faults
enum TISINameServiceFaults
    {
    EISINameServiceMemAllocFailure = 0x01,
    EISINameServiceNullPointer,
    EISINameServiceNullPointer1,
    EISINameServiceNullPointer2,
    EISINameServiceNullPointer6,
    EISINameServiceNullPointer7,
    EISINameServiceNullPointer8,
    EISINameServiceNullPointer9,
    EISINameServiceNullPointer10,
    EISINameServiceNullPointer11,
    EISINameServiceNullPointer12,
    EISINameServiceNullPointer13,
    EISINameServiceNullPointer14,
    EISINameServiceNullPointer15,
    EISINameServiceNullPointer16,
    EISINameServiceNullPointer17,
    EISINameServiceNullPointer18,
    EISINameServiceNullPointer19,
    EISINameServiceWrongLength,
    EISINameServiceWrongLength1,
    EISINameServiceWrongLength6,
    EISINameServiceWrongLength7,
    EISINameServiceWrongLength8,
    EISINameServiceWrongLength9,
    EISINameServiceWrongLength10,
    EISINameServiceWrongLength11,
    EISINameServiceWrongLength12,
    EISINameServiceUnknownMessage,
    EISINameServiceMutexCreateFailed,
    EISINameServiceMutexWaitFailed,
    EISINameServiceSendFailed,
    EISINameServiceFatalError,
#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    EISINameServiceWrongLength2,
    EISINameServiceWrongLength3,
    EISINameServiceNullPointer3,
    EISINameServiceNullPointer4,
    EISINameServiceNullPointer5,
    EISINameServiceNameCreateFailed,
    EISINameServiceNameCreateFailed1,
    EISINameServiceNameCreateFailed2,
    EISINameServiceNameCreateFailed3,
    EISINameServiceNameCreateFailed4,
    EISINameServiceNameCreateFailed5,
    EISINameServiceNameCreateFailed6,
    EISINameServiceNameCreateFailed7,
    EISINameServiceNameCreateFailed8,
    EISINameServiceNameCreateFailed9,
    EISINameServiceNameCreateFailed10,
    EISINameServiceNameCreateFailed11,
    EISINameServiceNameCreateFailed12,
    EISINameServiceNameCreateFailed13,
    EISINameServiceNameCreateFailed14,
    EISINameServiceNameCreateFailed15,
    EISINameServiceNameCreateFailed16,
    EISINameServiceNameCreateFailed17,
    EISINameServiceNameCreateFailed18,
    EISINameServiceWrongResponse,
    EISINameServiceNameExtendedCreateFailed,
    EISINameServiceExtendedNullPointer,
    EISINameServiceNameNotCommgrResource,
    EISINameServiceNameNotCommgrDevice,
    EISINameServiceLookUpForPhoNetAddressAndFlagsFailed,
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    };

const TUint32 KNameServiceUID( 0x2002A5A1 );
const TUint8 KFiller( 0 );
const TUint8 KAlwaysAssert( 0x00 );

const TUint16 KOneMatch( 0x00001 );

DISINameService* DISINameService::iThisptr = NULL;
DMutex* DISINameService::iNameServiceMutex = NULL;
_LIT8( KNameServiceMutex, "KNameServiceMutex" );
const TUint8 KEightBitShift( 8 );
const TUint8 KSixteenBitShift( 16 );
const TUint8 KDeviceIdentifierPhoNetAddressShift( KEightBitShift );
const TUint16 KUninitializedPhoNetAddress( 0x0000 );
const TUint8 KRouterObjId( 0xFA ); // Only internally agreed inside isi router

DISINameService::DISINameService(
        // None
        )
    : iObjId( 0x00 )
    {
    C_TRACE( ( _T( "DISINameService::DISINameService 0x%x 0x%x>" ), iObjId, iRouter ) );
    iNameRecords = new DNameRecords();
    ASSERT_RESET_ALWAYS( ( iNameRecords ), 
                         ( EISINameServiceMemAllocFailure | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TInt err( Kern::MutexCreate( iNameServiceMutex, KNameServiceMutex, KMutexOrdGeneral0 ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), 
                         ( EISINameServiceMutexCreateFailed | EDISINameServiceTraceId << KClassIdentifierShift ) );
    iRouter = MISIObjectRouterIf::GetIf();
    ASSERT_RESET_ALWAYS( ( iRouter ),
                         ( EISINameServiceNullPointer | EDISINameServiceTraceId << KClassIdentifierShift ) );
    iRouter->Connect( KNameServiceUID, iObjId, this );
    C_TRACE( ( _T( "DISINameService::DISINameService 0x%x" ), iObjId ) );
    ASSERT_RESET_ALWAYS( ( iObjId == PN_OBJ_ROUTING_REQ ), 
                         ( EISINameServiceFatalError | EDISINameServiceTraceId << KClassIdentifierShift ) );
    // Starting from here we add the PN_ROUTER object 'statically' to Name Service
    TUint32 name = PN_ROUTER;
    TUint16 phonetAddr = ( ( PN_DEV_OWN << KDeviceIdentifierPhoNetAddressShift ) | KRouterObjId );
    TUint8 flags = PN_NAME_NOCHG;
    err = iNameRecords->AddName( name, phonetAddr, flags );
#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    iISINameServiceExtended = new DISINameServiceExtended( this );
    ASSERT_RESET_ALWAYS( ( iISINameServiceExtended ), ( EISINameServiceNameExtendedCreateFailed | EDISINameServiceTraceId << KClassIdentifierShift ) );
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    iThisptr = this;
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
#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    if( iISINameServiceExtended )
        {
        delete iISINameServiceExtended;
        iISINameServiceExtended = NULL;
        }
#endif
    C_TRACE( ( _T( "DISINameService::~DISINameService<" ) ) );
    }

void DISINameService::Receive(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISINameService::Receive 0x%x>" ), &aMessage ) );
    // This to be signaled (and only) when send back.
    LockNameServiceDMutex();
    TUint8* msgPtr( (TUint8*)aMessage.Ptr() );
    TUint32 resource = GetResourceUsedInMessage( aMessage );
    if( resource == PN_NAMESERVICE 
        )
        {
        C_TRACE( ( _T( "DISINameService::Receive to PN_NAMESERVICE" ) ) );
        HandlePnsNameMessage( aMessage );
        }

    // Messages which are to be Resource routed.
    else
        {
        C_TRACE( ( _T( "DISINameService::Receive to Resource : 0x%x" ),resource  ) );
        TUint16 phoNetAddress( KUninitializedPhoNetAddress );
        TInt32 error = iNameRecords->LookupPhonetAddress( resource, &phoNetAddress );
        if( KErrNone == error  )
            {
            // Change phonet addr to current message from table entries.
            msgPtr[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] = (TUint8) ( phoNetAddress >> KEightBitShift );
            msgPtr[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] = (TUint8) ( phoNetAddress );
            TDes8* messagePtr = reinterpret_cast<TDes8*>( const_cast<TDesC8*>( &aMessage ) );
            C_TRACE( ( _T( "DISINameService message to recDev: 0x%x recObj: 0x%x" ), msgPtr[ ISI_HEADER_OFFSET_RECEIVERDEVICE ], msgPtr[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] ) );
            SendToRouter( *messagePtr );
            }
        else
            {
            C_TRACE( ( _T( "DISINameService::Receive no Resource found : 0x%x" ), resource  ) );

            TRACE_ASSERT_ALWAYS( KAlwaysAssert );
            BuildAndSendCommIsaEntityNotReachableResp(aMessage);

            }
        }
    C_TRACE( ( _T( "DISINameService::Receive 0x%x<" ), &aMessage ) );
    }

void DISINameService::HandleNameQueryReq(
        const TDesC8& aPnsNameQueryReq
        )
    {
    C_TRACE( ( _T( "DISINameService::HandleNameQueryReq 0x%x>" ) ,&aPnsNameQueryReq ) );
    const TUint8* msgPtr = aPnsNameQueryReq.Ptr();
    ASSERT_RESET_ALWAYS( msgPtr, ( EISINameServiceNullPointer6 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint32 name = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_NAME ] );
    TUint32 mask = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_BITMASK ] );
    RArray <TNameTableEntry*> nameTable;
    iNameRecords->NameQuery( name, mask, &nameTable );
    TUint32 count( nameTable.Count() );
    // Hoks! not needed ISI_HEADER_SIZE because in below resp contains assumption that there is one entry always.
    TUint16 msgLength = ( SIZE_PNS_NAME_QUERY_RESP + ( count * SIZE_PN_NAME_SRV_ITEM_STR ) );
    TDes8& respMsg = MemApi::AllocBlock( msgLength );
    respMsg.SetLength( msgLength );
    TUint8* respPtr = const_cast<TUint8*>( respMsg.Ptr() );
    ASSERT_RESET_ALWAYS( respPtr, ( EISINameServiceNullPointer7 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    SetIsiHeaderForResp( respMsg, aPnsNameQueryReq, PNS_NAME_QUERY_RESP );
    PUTB16( &respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_TOTALMATCHES ], count );
    PUTB16( &respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_MATCHESINMSG ], count );
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_RESERVED1 ] = KFiller;
    respPtr[ ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_RESERVED2 ] = KFiller;
    TUint16 index( ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_NAMEENTRYTBL );
    C_TRACE( ( _T( "DISINameService::HandleNameQueryReq 0x%x entry count %d" ) ,&aPnsNameQueryReq, count ) );
    for( TInt i( 0 ); i < count; i++ )
        {
        ASSERT_RESET_ALWAYS( ( respMsg.Length() > index + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ),
                             ( EISINameServiceWrongLength7 | EDISINameServiceTraceId << KClassIdentifierShift ) );
        TNameTableEntry* tmp = ( nameTable )[ i ];
        if ( ( (TUint8)( tmp->iPhonetAddress ) == PN_OBJ_ROUTING_REQ ) && 
             ( tmp->iName == name ) )
            {
            TRACE_ASSERT_ALWAYS( KAlwaysAssert );
            Kern::Printf("PNS_NAME_QUERY_REQ NOTE! Obj Id 0 not valid!! name 0x%08x txDev 0x%02x txObj 0x%02x", name, msgPtr[ISI_HEADER_OFFSET_SENDERDEVICE], msgPtr[ISI_HEADER_OFFSET_SENDEROBJECT] );
            }
        PUTB32( &respPtr[ index + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ],  tmp->iName );
        respPtr[ index + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ] = (TUint8)( tmp->iPhonetAddress >> KEightBitShift );
        respPtr[ index + PN_NAME_SRV_ITEM_STR_OFFSET_OBJ ] = (TUint8)( tmp->iPhonetAddress );
        respPtr[ index + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ] = tmp->iFlags;
        respPtr[ index + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ] = KFiller;
        index += SIZE_PN_NAME_SRV_ITEM_STR;
        }
    SendToRouter( respMsg );
    TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aPnsNameQueryReq) );
    MemApi::DeallocBlock( *block );
    C_TRACE( ( _T( "DISINameService::HandleNameQueryReq 0x%x<" ) ,&aPnsNameQueryReq  ) );
    }

void DISINameService::HandleNameRemoveReq(
        const TDesC8& aPnsNameRemoveReq
        )
    {
    C_TRACE( ( _T( "DISINameService::HandleNameRemoveReq 0x%x>" ), &aPnsNameRemoveReq ) );
    const TUint8* msgPtr = aPnsNameRemoveReq.Ptr();
    ASSERT_RESET_ALWAYS( msgPtr, ( EISINameServiceNullPointer8 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint32 name = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_REQ_OFFSET_NAME ] );
    TUint16 phonetAddress( KUninitializedPhoNetAddress );
    TUint8 flags( 0x00 );
    // Need to get these values in here to guarantee that they are the same in IND if needed.
    TInt error = iNameRecords->LookupNamePhonetAddressAndFlags( name, &phonetAddress, &flags );
    TRACE_ASSERT_ALWAYS( ( KErrNone == error ) );
    const TUint8 err = iNameRecords->RemoveName( name );
    C_TRACE( ( _T( "DISINameService::HandleNameRemoveReq PNS_NAME_REMOVE_RESP name 0x%x err 0x%x" ), name, err ) );
    TUint16 msgLength( ISI_HEADER_SIZE + SIZE_PNS_NAME_REMOVE_RESP );
    TDes8& respMsg = MemApi::AllocBlock( msgLength );
    respMsg.SetLength( msgLength );
    TUint8* respPtr = const_cast<TUint8*>( respMsg.Ptr() );
    ASSERT_RESET_ALWAYS( respPtr, ( EISINameServiceNullPointer9 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( respMsg.Length() > ISI_HEADER_SIZE + PNS_NAME_REMOVE_RESP_OFFSET_REASON ),
                         ( EISINameServiceWrongLength6 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    SetIsiHeaderForResp( respMsg, aPnsNameRemoveReq, PNS_NAME_REMOVE_RESP );
    if( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN )
        {
        C_TRACE( ( _T( "DISINameService::HandleNameRemoveReq from own device err 0x%x" ) ) );
        respPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_RESP_OFFSET_REASON ] = err;
        SendToRouter( respMsg );
        if( PN_NAME_OK == err )
            {
            TxPnsNameRemoveInd( aPnsNameRemoveReq, name, phonetAddress, flags );
            }
        // No need to send indication if removal did not succeed as state is the same.
        }
    else
        {
        C_TRACE( ( _T( "DISINameService::HandleNameRemoveReq PNS_NAME_REMOVE_REQ from other device not allowed" ) ) );
        respPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_RESP_OFFSET_REASON ] = PN_NAME_NOT_ALLOWED;
        SendToRouter( respMsg );
        }
    TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aPnsNameRemoveReq) );
    MemApi::DeallocBlock( *block );
    C_TRACE( ( _T( "DISINameService::HandleNameRemoveReq 0x%x<" ), &aPnsNameRemoveReq ) );
    }

void DISINameService::SendToRouter(
        TDes8& aMessage
        )
    {
    C_TRACE( ( _T( "DISINameService::SendToRouter 0x%x>" ), &aMessage ) );
    FreeNameServiceDMutex();
    TInt value = iRouter->Send( aMessage, iObjId );
    ASSERT_RESET_ALWAYS( value == KErrNone, ( EISINameServiceSendFailed | EDISINameServiceTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISINameService::SendToRouter 0x%x<" ), &aMessage ) );
    }

void DISINameService::HandlePnsNameMessage( const TDesC8& aMessage )
    {
    C_TRACE( ( _T( "DISINameService::HandlePnsNameMessage 0x%x>" ), &aMessage ) );
    const TUint8* msgPtr( aMessage.Ptr() );
    switch( msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] )
        {
        case PNS_NAME_QUERY_REQ:
            {
            C_TRACE( ( _T( "DISINameService::HandlePnsNameMessage PNS_NAME_QUERY_REQ" ) ) );
            HandleNameQueryReq( aMessage );
            break;
            }
        case PNS_NAME_ADD_REQ:
            {
            C_TRACE( ( _T( "DISINameService::HandlePnsNameMessage PNS_NAME_ADD_REQ" ) ) );
            HandleNameAddReq( aMessage );
            break;
            }
        case PNS_NAME_REMOVE_REQ:
            {
            C_TRACE( ( _T( "DISINameService::HandlePnsNameMessage PNS_NAME_REMOVE_REQ" ) ) );
            HandleNameRemoveReq( aMessage );
            break;
            }
        case COMMON_MESSAGE:
            {
            C_TRACE( ( _T( "DISINameService::HandlePnsNameMessage COMMON_MESSAGE" ) ) );
            HandleCommonMessage( aMessage );
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS( KAlwaysAssert );
            TDes8& commonMessage = reinterpret_cast<TDes8&>( const_cast<TDesC8&>( aMessage ) );    
            SendCommServiceNotIdentifiedResp( commonMessage );
            C_TRACE( ( _T( "DISINameService::HandlePnsNameMessage unknown NAMESERVICE message" )  ) );
            }
        }
    C_TRACE( ( _T( "DISINameService::HandlePnsNameMessage 0x%x<" ), &aMessage ) );
    }
    
void DISINameService::HandleNameAddReq(
        const TDesC8& aPnsNameAddReq
        )
    {
    C_TRACE( ( _T( "DISINameService::HandleNameAddReq 0x%x>" ), &aPnsNameAddReq ) );
    TInt err( KErrArgument );
    TUint16 respMsgLength( ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_RESP );
    TDes8& respMsg = MemApi::AllocBlock( respMsgLength );
    respMsg.SetLength( respMsgLength );
    TUint8* respPtr = const_cast<TUint8*>( respMsg.Ptr() );
    const TUint8* msgPtr = const_cast<TUint8*>( aPnsNameAddReq.Ptr() );
    SetIsiHeaderForResp( respMsg, aPnsNameAddReq, PNS_NAME_ADD_RESP );
    if( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN )
        {
        C_TRACE( ( _T( "DISINameService::HandleNameAddReq PNS_NAME_ADD_REQ from this device" ) ) );
        TUint32 name = GETB32( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY ] );
        TUint16 phonetAddr = GETB16( &msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ] );
        TUint8 flags = msgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ];
        err = iNameRecords->AddName( name, phonetAddr, flags );
        respPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] = err;
        SendToRouter( respMsg );
        if( err == PN_NAME_OK )
            {
            TxPnsNameAddInd( aPnsNameAddReq, name, phonetAddr, flags );
            }
        // No need for else as state is not changed and indication is not needed to send.
        }
    else
        {
        C_TRACE( ( _T( "DISINameService::HandleNameAddReq PNS_NAME_ADD_REQ from other device" ) ) );
        respPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] = PN_NAME_NOT_ALLOWED;
        SendToRouter( respMsg );
        }
    TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aPnsNameAddReq) );
    MemApi::DeallocBlock( *block );
    C_TRACE( ( _T( "DISINameService::HandleNameAddReq 0x%x<" ), &aPnsNameAddReq ) );
    }

EXPORT_C TBool MISICommunicationManagerIf::IsValidResource(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "MISICommunicationManagerIf::IsValidResource 0x%x>" ), &aMessage ) );
    DISINameService* self = DISINameService::GetISINameServiceF();
    TBool found = self->HandleResourceValidationCheck( aMessage );
    C_TRACE( ( _T( "MISICommunicationManagerIf::IsValidResource 0x%x %d<" ), &aMessage, found ) );
    return found;
    }

EXPORT_C TUint8 MISICommunicationManagerIf::GetDeviceIdentifierByResource(
        const TUint32 aResourceIdentifier
        )
    {
    C_TRACE( ( _T( "MISICommunicationManagerIf::GetDeviceIdentifierByResource 0x%x>" ), aResourceIdentifier ) );
    DISINameService* self = DISINameService::GetISINameServiceF();
    TUint8 deviceIdentifier = self->GetDeviceIdByResource( aResourceIdentifier );
    C_TRACE( ( _T( "MISICommunicationManagerIf::GetDeviceIdentifierByResource 0x%x 0x%x<" ), aResourceIdentifier, deviceIdentifier ) );
    return deviceIdentifier;
    }

DECLARE_STANDARD_EXTENSION()
    {
    Kern::Printf( "ISI Name Service extension>" );
    // Create a container extension
    DISINameService* extension = new DISINameService();
    TRACE_ASSERT( extension );
    Kern::Printf( "ISI Name Service extension<" );
    return extension ? KErrNone : KErrNoMemory;
    }

void DISINameService::BuildAndSendCommIsaEntityNotReachableResp(
        const TDesC8& aMsg
        )
    {
    C_TRACE( ( _T( "DISINameService::ConstructCommIsaEntityNotReachableResp 0x%x>" ), &aMsg ) );
    // Make channel opening request followinfg COMM specification: 000.026
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
    // Transactionid. 
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
    tempPtr.Append( KFiller );
    // Filler
    tempPtr.Append( KFiller );
    // Filler
    tempPtr.Append( KFiller );
    //send data to router for handling
    SendToRouter( tempPtr );
    TDes8* block = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMsg) );
    MemApi::DeallocBlock( *block );
    C_TRACE( ( _T( "DISINameService::SendCommIsaEntityNotReachableResp 0x%x<" ), &aMsg ) );
    }

void DISINameService::HandleCommonMessage(
        const TDesC8& aCommonMessage
        )
    {

    C_TRACE( ( _T( "DISINameService::HandleCommonMessage 0x%x>" ), &aCommonMessage ) );
    TDes8& commonMessage = reinterpret_cast<TDes8&>( const_cast<TDesC8&>(aCommonMessage) );
    const TUint8 subMessageIdentifier = commonMessage.Ptr()[ ISI_HEADER_OFFSET_SUBMESSAGEID ];
    switch( subMessageIdentifier )
        {
        case COMM_ISA_ENTITY_NOT_REACHABLE_RESP:
            {
            const TUint8 notDeliveredMessageIndex = ( ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_NOTDELIVEREDMESSAGEID );
            const TUint8 notDeliveredMessageIdentifier = commonMessage.Ptr()[ notDeliveredMessageIndex ];
            // If some of name service _RESP worryig behaviour from client. If _IND it does not matter so much, although unsubscription for indications is not working then.
            C_TRACE( ( _T( "DISINameService::HandleCommonMessage COMM_ISA_ENTITY_NOT_REACHABLE_RESP not delivered message 0x%x" ), notDeliveredMessageIdentifier ) );
            TRACE_ASSERT_ALWAYS( KAlwaysAssert );
            // Dellocate here, because this is not sending a reply at all to avoid never ending loop.
            MemApi::DeallocBlock( commonMessage );
            // Signal here because it does not send a reply.
            FreeNameServiceDMutex();
            break;
            }
        case COMM_ISI_VERSION_GET_REQ:
            {
            C_TRACE( ( _T( "DISINameService::HandleCommonMessage version get req" ) ) );
            SendCommIsiVersionGetResp( commonMessage );
            break;
            }
        default:
            {
            C_TRACE( ( _T( "DISINameService::HandleCommonMessage unknown common message" ) ) );
            TRACE_ASSERT_ALWAYS( KAlwaysAssert );
            SendCommServiceNotIdentifiedResp( commonMessage );
            break;
            }
        }
    C_TRACE( ( _T( "DISINameService::HandleCommonMessage 0x%x<" ), &aCommonMessage ) );
    }

void DISINameService::SendCommServiceNotIdentifiedResp(
        TDes8& aCommServiceNotIdentifiedReq
        )
    {
    C_TRACE( ( _T( "DISINameService::SendCommServiceNotIdentifiedResp 0x%x>" ), &aCommServiceNotIdentifiedReq ) );
    TUint8 length( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP );
    TDes8& respMsg = MemApi::AllocBlock( length );
    respMsg.SetLength(ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP );
    TUint8* ptr( const_cast<TUint8*>( respMsg.Ptr() ) );
    ASSERT_RESET_ALWAYS( ptr, ( EISINameServiceNullPointer1 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( respMsg.Length() >= (ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE2) ),
                         ( EISINameServiceWrongLength | EDISINameServiceTraceId << KClassIdentifierShift ) );
    const TUint8* msgPtr( aCommServiceNotIdentifiedReq.Ptr() );
    SetIsiHeaderForCommonResp( respMsg, aCommServiceNotIdentifiedReq );
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_SUBMESSAGEID ] = COMM_SERVICE_NOT_IDENTIFIED_RESP;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDMESSAGEID ] = msgPtr[ ISI_HEADER_OFFSET_MESSAGEID ];
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE1 ] = KFiller;// ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_MESSAGEEXTENSION ] = KNoExtension;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDSUBMESSAGEID ] = msgPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ];
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE1 ] = KFiller;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE2 ] = KFiller;
    SendToRouter( respMsg  );
    MemApi::DeallocBlock( aCommServiceNotIdentifiedReq );
    C_TRACE( ( _T( "DISINameService::SendCommServiceNotIdentifiedResp 0x%x<" ), &aCommServiceNotIdentifiedReq ) );
    }

void DISINameService::SendCommIsiVersionGetResp(
        TDes8& aCommIsiVersionGetReq
        )
    {
    C_TRACE( ( _T( "DISINameService::SendCommIsiVersionGetResp 0x%x>" ), &aCommIsiVersionGetReq ) );
    TUint8 length( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP );
    TDes8& respMsg = MemApi::AllocBlock( length );
    respMsg.SetLength(ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP);
    TUint8* ptr( const_cast<TUint8*>( respMsg.Ptr() ) );
    ASSERT_RESET_ALWAYS( ptr, ( EISINameServiceNullPointer2 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( respMsg.Length() >= (ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE3) ),
                         ( EISINameServiceWrongLength1 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    const TUint8* msgPtr( aCommIsiVersionGetReq.Ptr() );
    SetIsiHeaderForCommonResp( respMsg, aCommIsiVersionGetReq );
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_SUBMESSAGEID ] = COMM_ISI_VERSION_GET_RESP;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONZZZ ] = NAME_SERVICE_ISI_VERSION_Z;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONYYY ] = NAME_SERVICE_ISI_VERSION_Y;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE1 ] = KFiller;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE2 ] = KFiller;
    ptr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE3 ] = KFiller;    
    SendToRouter( respMsg  );
    MemApi::DeallocBlock( aCommIsiVersionGetReq );
    C_TRACE( ( _T( "DISINameService::SendCommIsiVersionGetResp 0x%x<" ), &aCommIsiVersionGetReq ) );
    }

void DISINameService::LockNameServiceDMutex()
    {
    C_TRACE( ( _T( "DISINameService::LockNameServiceDMutex 0x%x>" ), &iNameServiceMutex ) );
    TInt err( Kern::MutexWait( *iNameServiceMutex ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISINameServiceMutexWaitFailed | EDISINameServiceTraceId << KClassIdentifierShift ) );        
    C_TRACE( ( _T( "DISINameService::LockNameServiceDMutex 0x%x<" ), &iNameServiceMutex ) );
    }

void DISINameService::FreeNameServiceDMutex()
    {
    C_TRACE( ( _T( "DISINameService::FreeNameServiceDMutex 0x%x>" ), &iNameServiceMutex ) );
    Kern::MutexSignal( *iNameServiceMutex );
    C_TRACE( ( _T( "DISINameService::FreeNameServiceDMutex 0x%x<" ), &iNameServiceMutex ) );
    }

TBool DISINameService::HandleResourceValidationCheck(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISINameService::HandleResourceValidationCheck 0x%x>" ), &aMessage ) );
    LockNameServiceDMutex();
    TUint8* msgPtr = const_cast<TUint8*>( aMessage.Ptr() );
    TUint16 phonetAddress( KUninitializedPhoNetAddress );
    // Extended resource identifier support for NameService
    TUint32 resource = GetResourceUsedInMessage( aMessage );
    iNameRecords->LookupPhonetAddress( resource, &phonetAddress );
    FreeNameServiceDMutex();
    TBool found( EFalse );
    TUint16 phonetAddressFromMsg = ( ( GET_SENDER_DEV( msgPtr ) << KDeviceIdentifierPhoNetAddressShift ) | GET_SENDER_OBJ( msgPtr ) );
    if( phonetAddress == phonetAddressFromMsg )
        {
        C_TRACE( ( _T( "DISINameService::IsValidResource phonetAddress found 0x%x" ), phonetAddress ) );
        found = ETrue;
        }
    // No need for else, assert tells that somebody violates indication sending
    TRACE_ASSERT( found );
    C_TRACE( ( _T( "DISINameService::HandleResourceValidationCheck 0x%x %d<" ), &aMessage, found ) );
    return found;
    }

// Extended resource identifier support for NameService
TUint32 DISINameService::GetResourceUsedInMessage(
        const TDesC8& aMessageToBeResourceRouted
        )
    {
    C_TRACE( ( _T( "DISINameService::GetResourceUsedInMessage 0x%x>" ), &aMessageToBeResourceRouted ) );
    const TUint8* msgPtr( aMessageToBeResourceRouted.Ptr() );
    ASSERT_RESET_ALWAYS( ( msgPtr ), 
                         ( EISINameServiceNullPointer11 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint32 resource = msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ];
#ifdef ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
    // Extended Resource Routing
    if( PN_PREFIX == msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] )
        {
        C_TRACE( ( _T( "DISINameService::GetResourceUsedInMessage 0x%x extended resource 0x%x" ), &aMessageToBeResourceRouted, resource ) );
        resource = ( resource | (TUint32)( msgPtr[ ISI_HEADER_OFFSET_TYPE ] << KEightBitShift ) );
        resource = ( resource | (TUint32)( msgPtr[ ISI_HEADER_OFFSET_SUBTYPE ] << KSixteenBitShift ) );
        }

#endif // ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
    // No need for else as first 8 bits are already used.
    C_TRACE( ( _T( "DISINameService::GetResourceUsedInMessage 0x%x resource 0x%x<" ), &aMessageToBeResourceRouted, resource ) );
    return resource;
    }

TUint8 DISINameService::GetDeviceIdByResource(
        const TUint32 aResourceIdentifier
        )
    {
    C_TRACE( ( _T( "DISINameService::GetDeviceIdByResource 0x%x>" ), aResourceIdentifier ) );
    TUint8 deviceIdentifier = PN_DEV_OWN;
#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    C_TRACE( ( _T( "DISINameService::GetDeviceIdByResource 0x%x ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE" ), aResourceIdentifier ) );
    LockNameServiceDMutex();
    TUint16 phonetAddress( KUninitializedPhoNetAddress );
    TUint32 result = iNameRecords->LookupPhonetAddress( aResourceIdentifier, &phonetAddress );
    FreeNameServiceDMutex();
    if( result == KErrNone )
        {
        C_TRACE( ( _T( "DISINameService::GetDeviceIdByResource 0x%x old device 0x%x phonetAddress 0x%x" ), aResourceIdentifier, deviceIdentifier, phonetAddress ) );
        deviceIdentifier = ( (TUint8)( phonetAddress >> KEightBitShift ) );
        C_TRACE( ( _T( "DISINameService::GetDeviceIdByResource 0x%x new device 0x%x phonetAddress 0x%x" ), aResourceIdentifier, deviceIdentifier, phonetAddress ) );
        }
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE  
    // No need for else as going to default
    C_TRACE( ( _T( "DISINameService::GetDeviceIdByResource 0x%x 0x%x<" ), aResourceIdentifier, deviceIdentifier ) );
    return deviceIdentifier;
    }

void DISINameService::TxPnsNameAddInd(
        const TDesC8& aPnsNameAddReq,
        const TUint32 aName,
        const TUint16 aPhoNetAddress,
        const TUint8 aFlags
        )
    {
    C_TRACE( ( _T( "DISINameService::TxPnsNameAddInd 0x%x>" ), &aPnsNameAddReq ) );
    LockNameServiceDMutex();
    const TUint8* msgPtr( aPnsNameAddReq.Ptr() );
    ASSERT_RESET_ALWAYS( msgPtr,
                       ( EISINameServiceNullPointer13 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint32 msgLength = ( ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_IND );
    TDes8& indMsg = MemApi::AllocBlock( msgLength );
    indMsg.SetLength( msgLength );
    ASSERT_RESET_ALWAYS( ( indMsg.Length() > ( ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ) ),
                         ( EISINameServiceWrongLength8 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint8* indPtr = const_cast<TUint8*>( indMsg.Ptr() );
    SetIsiHeaderForIndication( indMsg, aPnsNameAddReq, PNS_NAME_ADD_IND );
    PUTB16( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_TOTALMATCHES ], KOneMatch );
    PUTB16( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_MATCHESINMSG ], KOneMatch );
    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_RESERVED1 ] = KFiller;
    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_RESERVED2 ] = KFiller;
    PUTB32( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], aName );
    PUTB16( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ], aPhoNetAddress );
    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ] = aFlags;
    indPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ] = KFiller;
    SendToRouter( indMsg );

    C_TRACE( ( _T( "DISINameService::TxPnsNameAddInd 0x%x<" ), &aPnsNameAddReq ) );
    }

void DISINameService::TxPnsNameRemoveInd(
        const TDesC8& aPnsNameRemoveReq,
        const TUint32 aName,
        const TUint16 aPhoNetAddress,
        const TUint8 aFlags
        )
    {
    C_TRACE( ( _T( "DISINameService::TxPnsNameRemoveInd 0x%x>" ), &aPnsNameRemoveReq ) );
    LockNameServiceDMutex();
    const TUint8* msgPtr( aPnsNameRemoveReq.Ptr() );
    ASSERT_RESET_ALWAYS( msgPtr,
                       ( EISINameServiceNullPointer14 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint32 msgLength = ( ISI_HEADER_SIZE + SIZE_PNS_NAME_REMOVE_IND );
    TDes8& indMsg = MemApi::AllocBlock( msgLength );
    indMsg.SetLength( msgLength );
    ASSERT_RESET_ALWAYS( ( indMsg.Length() > ( ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ) ),
                         ( EISINameServiceWrongLength8 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint8* indPtr = const_cast<TUint8*>( indMsg.Ptr() );
    ASSERT_RESET_ALWAYS( indPtr, ( EISINameServiceNullPointer10 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    SetIsiHeaderForIndication( indMsg, aPnsNameRemoveReq, PNS_NAME_REMOVE_IND );
    C_TRACE( ( _T( "DISINameService::TxPnsNameRemoveInd 0x%x name 0x%x phonetAddress 0x%x flags 0x%x" ), &aPnsNameRemoveReq, aName, aPhoNetAddress, aFlags ) );
    PUTB16( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_TOTALMATCHES ], KOneMatch );
    PUTB16( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_MATCHESINMSG ], KOneMatch );
    indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_RESERVED1 ] = KFiller;
    indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_RESERVED2 ] = KFiller;
    PUTB32( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], aName );
    PUTB16( &indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ], aPhoNetAddress );
    indPtr[ ISI_HEADER_SIZE + PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ] = aFlags;
    SendToRouter( indMsg );
    C_TRACE( ( _T( "DISINameService::TxPnsNameRemoveInd 0x%x<" ), &aPnsNameRemoveReq ) );
    }

DISINameService* DISINameService::GetISINameServiceF(
        // None
        )
    {
    ASSERT_RESET_ALWAYS( iThisptr,
                       ( EISINameServiceNullPointer12 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    return iThisptr;
    }

DNameRecords* DISINameService::GetNameRecordsF(
        // None
        )
    {
    ASSERT_RESET_ALWAYS( iNameRecords,
                       ( EISINameServiceNullPointer15 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    return iNameRecords;
    }

void DISINameService::SetIsiHeaderForResp(
        const TDesC8& aResponseMessage,
        const TDesC8& aRequestMessage,
        const TUint8 aResponseMessageIdentifier
        )
    {
    C_TRACE( ( _T( "DISINameService::SetIsiHeaderForResp 0x%x 0x%x 0x%x>" ), &aResponseMessage, &aRequestMessage, aResponseMessageIdentifier ) );
    const TUint8* reqMsgPtr = aRequestMessage.Ptr();
    ASSERT_RESET_ALWAYS( reqMsgPtr, ( EISINameServiceNullPointer16 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint8* respMsgPtr = const_cast<TUint8*>( aResponseMessage.Ptr() );
    ASSERT_RESET_ALWAYS( respMsgPtr, ( EISINameServiceNullPointer17 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aResponseMessage.Length() > ISI_HEADER_OFFSET_MESSAGEID ),
                         ( EISINameServiceWrongLength9 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aRequestMessage.Length() > ISI_HEADER_OFFSET_TRANSID ),
                         ( EISINameServiceWrongLength10 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    respMsgPtr[ ISI_HEADER_OFFSET_MEDIA ] = reqMsgPtr[ ISI_HEADER_OFFSET_MEDIA ];
    SET_RECEIVER_DEV( respMsgPtr, GET_SENDER_DEV( reqMsgPtr ) );
    SET_SENDER_DEV( respMsgPtr, PN_DEV_OWN );
    respMsgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
    SET_LENGTH( respMsgPtr, ( aResponseMessage.Length() - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( respMsgPtr, GET_SENDER_OBJ( reqMsgPtr ) );
    SET_SENDER_OBJ( respMsgPtr, PN_OBJ_ROUTER );
    respMsgPtr[ ISI_HEADER_OFFSET_TRANSID ] = reqMsgPtr[ ISI_HEADER_OFFSET_TRANSID ];
    respMsgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] = aResponseMessageIdentifier;
    C_TRACE( ( _T( "DISINameService::SetIsiHeaderForResp 0x%x 0x%x 0x%x<" ), &aResponseMessage, &aRequestMessage, aResponseMessageIdentifier ) );
    }

void DISINameService::SetIsiHeaderForCommonResp(
        const TDesC8& aResponseMessage,
        const TDesC8& aRequestMessage
        )
    {
    C_TRACE( ( _T( "DISINameService::SetIsiHeaderForCommonResp 0x%x 0x%x>" ), &aResponseMessage, &aRequestMessage ) );
    const TUint8* reqMsgPtr = aRequestMessage.Ptr();
    ASSERT_RESET_ALWAYS( reqMsgPtr, ( EISINameServiceNullPointer18 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint8* respMsgPtr = const_cast<TUint8*>( aResponseMessage.Ptr() );
    ASSERT_RESET_ALWAYS( respMsgPtr, ( EISINameServiceNullPointer19 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aResponseMessage.Length() > ISI_HEADER_OFFSET_MESSAGEID ),
                         ( EISINameServiceWrongLength11 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aRequestMessage.Length() > ISI_HEADER_OFFSET_TRANSID ),
                         ( EISINameServiceWrongLength12 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    respMsgPtr[ ISI_HEADER_OFFSET_MEDIA ] = ( reqMsgPtr[ ISI_HEADER_OFFSET_MEDIA ] );
    SET_RECEIVER_DEV( respMsgPtr, GET_SENDER_DEV( reqMsgPtr ) );
    SET_SENDER_DEV( respMsgPtr, PN_DEV_OWN );
    respMsgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = ( reqMsgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] );
    SET_LENGTH( respMsgPtr, ( aResponseMessage.Length() - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( respMsgPtr, GET_SENDER_OBJ( reqMsgPtr ) );
    SET_SENDER_OBJ( respMsgPtr, GET_RECEIVER_OBJ( reqMsgPtr ) );
    respMsgPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_TRANSACTIONID ] = ( reqMsgPtr[ ISI_HEADER_OFFSET_TRANSID ] );
    respMsgPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_MESSAGEID ] = COMMON_MESSAGE;
    C_TRACE( ( _T( "DISINameService::SetIsiHeaderForCommonResp 0x%x 0x%x<" ), &aResponseMessage, &aRequestMessage ) );
    }

void DISINameService::SetIsiHeaderForIndication(
        const TDesC8& aIndicationToModify,
        const TDesC8& aPnsNameRequest,
        const TUint8 aIndicationMessageIdentifier
        )
    {
    C_TRACE( ( _T( "DISINameService::aIndicationToModify 0x%x 0x%x 0x%x>" ), &aIndicationToModify, &aPnsNameRequest, aIndicationMessageIdentifier ) );
    const TUint8* reqMsgPtr = aPnsNameRequest.Ptr();
    ASSERT_RESET_ALWAYS( reqMsgPtr, ( EISINameServiceNullPointer18 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    TUint8* indMsgPtr = const_cast<TUint8*>( aIndicationToModify.Ptr() );
    ASSERT_RESET_ALWAYS( indMsgPtr, ( EISINameServiceNullPointer19 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aIndicationToModify.Length() > ISI_HEADER_OFFSET_MESSAGEID ),
                         ( EISINameServiceWrongLength11 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aPnsNameRequest.Length() > ISI_HEADER_OFFSET_TRANSID ),
                         ( EISINameServiceWrongLength12 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    indMsgPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
    SET_RECEIVER_DEV( indMsgPtr, GET_SENDER_DEV( reqMsgPtr ) );
    SET_SENDER_DEV( indMsgPtr, PN_DEV_OWN );
    indMsgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
    SET_LENGTH( indMsgPtr, ( aIndicationToModify.Length() - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( indMsgPtr, PN_OBJ_EVENT_MULTICAST );
    SET_SENDER_OBJ( indMsgPtr, PN_OBJ_ROUTER );
    indMsgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_UTID ] = 0x00;
    indMsgPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_IND_OFFSET_SUBFUNCTION ]= aIndicationMessageIdentifier;
    C_TRACE( ( _T( "DISINameService::aIndicationToModify 0x%x 0x%x 0x%x<" ), &aIndicationToModify, &aPnsNameRequest, aIndicationMessageIdentifier ) );
    }
DISINameServiceExtended::DISINameServiceExtended(
        DISINameService* aNameService
        )
    {
    C_TRACE( ( _T( "DISINameServiceExtended::DISINameServiceExtended 0x%x>" ), iISINameService ) );
    iISINameService = aNameService;
    ASSERT_RESET_ALWAYS( iISINameService, ( EISINameServiceExtendedNullPointer | EDISINameServiceTraceId << KClassIdentifierShift ) );
    AddModemResources();
    //AddPnDevDspMainResources();
    // iShNameAddIndSubscriptionState = ENotSubscribed;

    //
    // flag: ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    // 
    //
    // - Extended Resource routing supported by Symbian SW Name Service (Design Change)
    // - PN_COMMGR by default routed to RAPU, by device id 0x6c routed to Symbian SW PN_COMMGR (Design Change)
    //     - Internally in ISCE PN_COMMGR name is handled : [0] PN_COMMGR, [1] deviceidentifier, [2] don't care, [3] don't care only visible in NameRecords table
    // - PN_NAMESERVICE by default routed to RAPU by device id 0x6c routed to Symbian SW PN_NAMESERVICE
    // - Symbian SW wider name service subscribes PNS_NAME_ADD_INDs from RAPU through Symbian SW PN_APE_COMMGR (REQ, RESP)
    // - Symbian SW Name Service contains: Symbian SW Resources added with PNS_NAME_ADD_REQ dev id PN_DEV_OWN (Phonet address 0x6c[own for all], TD MODEM Resources (Phonet address 0x6000), PN_DSP_MAIN, Symbian SW Resource stored in RAPU Name Service (dynamically added after PNS_NAME_ADD_IND received PhoNet address as got from indication), other messages are routed to RAPU (Design Change)
    // - Communication Manager calls ::GetDeviceIdentifierByResouce to get appropriate device identifier to indication subscription
    //
    // worry later the usage of shared resource such as PN_COMMGR, PN_NAME_SERVICE etc.. others that shall pop up due to missing ISA system design, use rxdev from message vs. nametable entry to differentiate them

    C_TRACE( ( _T( "DISINameServiceExtended::DISINameServiceExtended 0x%x<" ), iISINameService ) );
    }

DISINameServiceExtended::~DISINameServiceExtended(
        // None
        )
    {
    C_TRACE( ( _T( "DISINameServiceExtended::~DISINameServiceExtended<>" ) ) );
    }
void DISINameServiceExtended::AddModemResources(
        // None
        )
    {
    C_TRACE( ( _T( "DISINameServiceExtended::AddModemResources>" ) ) );
    TUint8 err( PN_NAME_OK );
    TUint8 flags = PN_NAME_NOCHG;
    TUint16 phonetAddressModemRoutingReq = ( ( PN_DEV_MODEM << KDeviceIdentifierPhoNetAddressShift ) | PN_OBJ_ROUTING_REQ );
    
    DNameRecords* nameRecords = iISINameService->GetNameRecordsF();
	
    err = nameRecords->AddName( PN_SMS, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed1 | EDISINameServiceTraceId << KClassIdentifierShift ) );
        
    err = nameRecords->AddName( PN_SS, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed2 | EDISINameServiceTraceId << KClassIdentifierShift ) );
        
    err = nameRecords->AddName( PN_CSD, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed3 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_GPDS, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed4 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_GSS, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed5 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_UICC, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed6 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_AT_MODEM, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed7 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_MODEM_LCS, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed8 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_MODEM_TEST, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed9 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_MODEM_NVD, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed10 | EDISINameServiceTraceId << KClassIdentifierShift ) ); 
    
    err = nameRecords->AddName( PN_MODEM_MCE, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed11 | EDISINameServiceTraceId << KClassIdentifierShift ) ); 
    
    err = nameRecords->AddName( PN_MODEM_INFO, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed12 | EDISINameServiceTraceId << KClassIdentifierShift ) ); 
    
    err = nameRecords->AddName( PN_MODEM_NETWORK, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed13 | EDISINameServiceTraceId << KClassIdentifierShift ) ); 
    
    err = nameRecords->AddName( PN_MODEM_CALL, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed14 | EDISINameServiceTraceId << KClassIdentifierShift ) ); 
    
    err = nameRecords->AddName( PN_WCDMA_RF_TEST, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed15 | EDISINameServiceTraceId << KClassIdentifierShift ) ); // EISINameServiceNameCreateFailed15...  
    
    err = nameRecords->AddName( PN_GSM_RF_TEST, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed16 | EDISINameServiceTraceId << KClassIdentifierShift ) ); 
    
    err = nameRecords->AddName( PN_RF_HAL, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed17 | EDISINameServiceTraceId << KClassIdentifierShift ) );
    
    err = nameRecords->AddName( PN_COMMON_RF_TEST, phonetAddressModemRoutingReq, flags );
    ASSERT_RESET_ALWAYS( ( PN_NAME_OK == err ), ( EISINameServiceNameCreateFailed18 | EDISINameServiceTraceId << KClassIdentifierShift ) ); 
    
    C_TRACE( ( _T( "DISINameServiceExtended::AddModemResources<" ) ) );
    }
