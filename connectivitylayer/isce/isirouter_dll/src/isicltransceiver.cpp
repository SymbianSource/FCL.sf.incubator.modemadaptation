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
#include "isirouter.h"            // For DISIRouter
#include "isiroutertrace.h"       // For C_TRACE, ASSERT_RESET.. and fault codes
#include "misirouterlinkif.h"     // For MISIRouterLinkIf
#include "isihelpers.h"           // For GET_SENDER_DEV...
#include "memapi.h"               // For MemApi
#include <trxdefs.h>              // For ETrxTest...
#include "isicltransceiver.h"     // For DISICLTransceiver


#define PN_MEDIA_TEST 0xBB //not real
#define PN_DEV_MODEM 0x12
#define PN_DEV_DUMMYIST 0x13
#define PN_DEV_APE 0x14

// Faults
enum TISICLTransceiverFaults
    {
    EISICLTransceiverMemAllocFailure = 0x01,
    EISICLTransceiverMemAllocFailure1,
    EISICLTransceiverNotSupportedMedia,
    EISICLTransceiverOverTheLimits,
    EISICLTransceiverNULLPtr,
    EISICLTransceiverNotSupportedDevice,
    };

DISICLTransceiver::DISICLTransceiver( DISIRouter* aPtr )
    : iRouter( aPtr )
    {
    C_TRACE( ( _T( "DISICLTransceiver::DISICLTransceiver>" ) ) );
    iLinksArray = new MISIRouterLinkIf*[ EISIAmountOfMedias ];
    ASSERT_RESET_ALWAYS( iLinksArray, ( EISICLTransceiverMemAllocFailure | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    // Initialize links
    for( TInt i( 0 ); i < EISIAmountOfMedias; i++ )
        {
        iLinksArray[ i ] = NULL;
        }
    //TODO add real media and trx when known
    iLinksArray[ EISIMediaTest ] = MISIRouterLinkIf::CreateLinkF( this, PN_MEDIA_TEST, ETrxTest );
    C_TRACE( ( _T( "DISICLTransceiver::DISICLTransceiver<" ) ) );
    }

DISICLTransceiver::~DISICLTransceiver()
    {
    C_TRACE( ( _T( "DISICLTransceiver::~DISICLTransceiver>" ) ) );
    for( TUint8 i( 0 ); i < EISIAmountOfMedias; i++ )
        {
        MISIRouterLinkIf* tmp = iLinksArray[ i ];
        if( tmp )
            {
            tmp->Release();
            tmp = NULL;
            iLinksArray[ i ] = NULL;
            }
        }
    delete []iLinksArray;
    C_TRACE( ( _T( "DISICLTransceiver::~DISICLTransceiver<" ) ) );
    }
    
void DISICLTransceiver::SendCommIsaEntityNotReachableResp( TDes8& aMessage )
    {
    C_TRACE( ( _T( "DISICLTransceiver::SendCommIsaEntityNotReachableResp 0x%x>" ), &aMessage ) );
    const TUint8* msgTmpPtr( aMessage.Ptr() );
    // To avoid COMM_ISA_ENTITY_NOT_REACHABLE_RESP loop
    if( msgTmpPtr[ ISI_HEADER_OFFSET_MESSAGEID ] != COMMON_MESSAGE &&
        msgTmpPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ] != COMM_ISA_ENTITY_NOT_REACHABLE_RESP )
        {
        // Make channel opening request followinfg COMM specification: 000.026
        // Length is sixteen bytes.
		    TUint8 length( 16 );
		    TDes8& tempPtr = MemApi::AllocBlock( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP );
		    ASSERT_RESET_ALWAYS( &tempPtr, ( EISICLTransceiverMemAllocFailure1 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
  	    ASSERT_RESET_ALWAYS( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP > ISI_HEADER_OFFSET_MESSAGEID, ( EISICLTransceiverOverTheLimits | EDISICLTransceiverTraceId << KClassIdentifierShift ) );    
  	    TUint8* ptr = const_cast<TUint8*>( tempPtr.Ptr() );
  	    // We start to append from transaction id.
  	    tempPtr.SetLength( ISI_HEADER_OFFSET_TRANSID );
  	    // Get the header until messageid from prev. message.
		    // Just turn receiver and sender device and object vice versa.
		    ptr[ ISI_HEADER_OFFSET_MEDIA ] = msgTmpPtr[ ISI_HEADER_OFFSET_MEDIA ];
		    SET_RECEIVER_DEV( ptr, GET_SENDER_DEV( aMessage ) );
		    SET_SENDER_DEV  ( ptr, GET_RECEIVER_DEV( aMessage ) );
		    ptr[ ISI_HEADER_OFFSET_RESOURCEID ] = msgTmpPtr[ ISI_HEADER_OFFSET_RESOURCEID ];
		    SET_LENGTH( ptr, length - PN_HEADER_SIZE );
		    SET_RECEIVER_OBJ( ptr, GET_SENDER_OBJ( aMessage ) );
		    SET_SENDER_OBJ( ptr, GET_RECEIVER_OBJ( aMessage ) );
		    // Transactionid. Set to 0x01 since this is the first.
		    tempPtr.Append( msgTmpPtr[ ISI_HEADER_OFFSET_TRANSID ] );
		    // Message ID
		    tempPtr.Append( ISI_HEADER_OFFSET_MESSAGEID );
		    // Sub message ID.
		    tempPtr.Append( COMM_ISA_ENTITY_NOT_REACHABLE_RESP );
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
		    RouteISIMessage( tempPtr );
		    }
		else
		    {
		    C_TRACE( ( _T( "DISICLTransceiver Not sending another CommIsaEntityNotReachableResp" ) ) );
		    }
    C_TRACE( ( _T( "DISICLTransceiver::SendCommIsaEntityNotReachableResp 0x%x<" ), &aMessage ) );
    }

void DISICLTransceiver::RouteISIMessage( TDes8& aMessage )
    {
	  C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage 0x%x>" ), &aMessage ) );
	  TInt error( ValidateISIMessage( aMessage ) ); //TODO what to do with error?
	 	TBool sendOk( EFalse );
	 	TUint8* messageBlockPtr( const_cast<TUint8*>( aMessage.Ptr() ) );
	  switch( messageBlockPtr[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] )
        {
        case PN_DEV_APE:
            {
            C_TRACE( ( _T( "DISIRouter msg to PN_DEV_APE" ) ) );
            if( messageBlockPtr[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] == PN_OBJ_ROUTING_REQ )
                {
                //route with resource and nameservice
                }
            else{
            	  sendOk = iRouter->Receive( aMessage, messageBlockPtr[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] );
                C_TRACE( ( _T( "DISIRouter router sendOk %d" ), sendOk ) );
                }
            break;
            }
        default:
            {
            C_TRACE( ( _T( "DISIRouter msg to other device" ) ) );
            TUint8 linkId = MapDeviceToMedia( messageBlockPtr[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] );
	          ASSERT_RESET_ALWAYS( linkId < EISIAmountOfMedias, ( EISICLTransceiverNotSupportedMedia | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
            MISIRouterLinkIf* link = iLinksArray[ linkId ];
            ASSERT_RESET_ALWAYS( link, ( EISICLTransceiverNULLPtr | EDISICLTransceiverTraceId << KClassIdentifierShift ) );  
            sendOk = link->Send( aMessage );
            C_TRACE( ( _T( "DISIRouter link sendOk %d" ), sendOk ) );
            break;
            }
        }
    if( !sendOk )
        {
        SendCommIsaEntityNotReachableResp( aMessage );
        MemApi::DeallocBlock( aMessage );
        }   
	  C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage 0x%x<" ), &aMessage ) );
    }

// KErrBadDescriptor, if message length too small
// KErrUnderFlow, if message length too big.
// KErrCouldNotConnect, if receiver object is out of scope.
TInt DISICLTransceiver::ValidateISIMessage(
        TDes8& aMessage
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::ValidateISIMessage 0x%x>" ), &aMessage ) );
    const TUint16 descLength( aMessage.Length() );
    TInt msgOk( KErrNone );
    msgOk = ( ISI_HEADER_OFFSET_MESSAGEID >= descLength ) ? KErrBadDescriptor : msgOk;
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    // Get ISI message length after known that the descriptor is big enough.
    const TUint8* msgPtr( aMessage.Ptr() );
    const TUint16 isiMsgLength( GET_LENGTH( msgPtr ) + PN_HEADER_SIZE );
    // If the descriptor length is less than ISI message length.
    msgOk = ( ( msgOk == KErrNone && isiMsgLength > descLength ) ? KErrUnderflow : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    // If the ISI message length is bigger that the largest supported.
    msgOk = ( ( msgOk == KErrNone && isiMsgLength > KMaxISIMsgSize ) ? KErrUnderflow : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    // If the ISI message length with PN_HEADER_SIZE is less or equal than ISI_HEADER_OFFSET_MESSAGEID. 
    msgOk = ( ( msgOk == KErrNone && isiMsgLength <= ISI_HEADER_OFFSET_MESSAGEID ) ? KErrUnderflow : msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    TRACE_ASSERT_INFO( msgOk == KErrNone, isiMsgLength );
    TRACE_ASSERT_INFO( msgOk == KErrNone, descLength );
    TRACE_ASSERT_INFO( msgOk == KErrNone, msgOk );
    C_TRACE( ( _T( "DISICLTransceiver::ValidateISIMessage %d<" ), msgOk ) );
    return msgOk;
    }

TUint8 DISICLTransceiver::MapDeviceToMedia(
        const TUint8 aDevice
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::MapDeviceToMedia aDevice %d>" ), aDevice ) );
    TUint8 media( KErrNone );
    switch( aDevice )
        {
        case PN_DEV_MODEM:
            {
            media = EISIMediaSharedMemory;
            break;
            }
        case PN_DEV_DUMMYIST:
            {
            media = EISIMediaTest;
            break;
            }
        // Not supported device
        default:
            {
           	ASSERT_RESET_ALWAYS( 0, ( EISICLTransceiverNotSupportedDevice | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
            break;
            }
        }
    C_TRACE( ( _T( "DISICLTransceiver::MapDeviceToMedia media %d<" ), media ) );
    return media;
    }

