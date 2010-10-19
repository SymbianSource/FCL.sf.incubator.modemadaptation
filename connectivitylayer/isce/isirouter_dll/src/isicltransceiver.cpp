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



#include <kernel.h>                 // For Kern
#include <pn_const.h>               // For PN_OBJ_ROUTING_REQ
#include <phonetisi.h>              // For ISI_HEADER_OFFSET_...
#include <commisi.h>                // For COMMON_ISA_...

#include "isirouter.h"              // For DISIRouter
#include "isiroutertrace.h"         // For C_TRACE, ASSERT_RESET.. and fault codes
#include "misirouterlinkif.h"       // For MISIRouterLinkIf
#include "isihelpers.h"             // For GET_SENDER_DEV...
#include "memapi.h"                 // For MemApi
#include <trxdefs.h>                // For ETrxTest...
#include "isicltransceiver.h"       // For DISICLTransceiver
#include <nsisi.h>                  // For PN_NAMESERVICE...
#include "isirouterservice.h"       // For DIsiRouterService
#include "isimsgqueue.h"            // For DISIMsgQueue
#include "iscnokiadefinitions.h"    // For EIscNokiaUsbPhonetLink
#ifdef USE_MEDIAAPI  
#include <pn_isimedia_const.h>      // For PN_DEV_PC 
#endif /* USE_MEDIAAPI */

// Faults
enum TISICLTransceiverFaults
    {
    EISICLTransceiverMemAllocFailure = 0x01,
    EISICLTransceiverMemAllocFailure1,
    EISICLTransceiverMemAllocFailure2,
    EISICLTransceiverMemAllocFailure3,
    EISICLTransceiverMemAllocFailure4,
    EISICLTransceiverMemAllocFailure5,
    EISICLTransceiverMemAllocFailure6,
    EISICLTransceiverMemAllocFailure7,
    EISICLTransceiverMemAllocFailure8,
#ifdef USE_MEDIAAPI 	
    EISICLTransceiverMemAllocFailure9,
    EISICLTransceiverMemAllocFailure10,  
#endif /* USE_MEDIAAPI */  
    EISICLTransceiverNotSupportedMedia,
    EISICLTransceiverOverTheLimits,
    EISICLTransceiverNULLPtr,
    EISICLTransceiverNULLPtr2,
    EISICLTransceiverNULLPtr3,
    EISICLTransceiverNULLPtr4,
    EISICLTransceiverNULLPtr5,
    EISICLTransceiverNULLPtr6,
    EISICLTransceiverNULLPtr7,
    EISICLTransceiverNotSupportedDevice,
    EISICLTransceiverDeviceConnection,
    EISICLTransceiverCommIsaEntityNotReachableResp,
    EISICLTransceiverNotThreadContext,
    EISICLTransceiverMutexCreateFailed,
    EISICLTransceiverMutexWaitFailed,
    EISICLTransceiverMutexWaitFailed2,
    };

// Constants
const TInt KDfcPriority( 5 );

DMutex* DISICLTransceiver::iDynamicDeviceTableMutex = NULL;
_LIT8( KISICLTransceiverMutex, "ISICLTransceiverMutex" );

DISICLTransceiver::DISICLTransceiver(
        DISIRouter& aRouter,
        TDfcQue* aThreadPtr
        )
    : iShRouter( aRouter )
    {
    C_TRACE( ( _T( "DISICLTransceiver::DISICLTransceiver>" ) ) );
    ASSERT_RESET_ALWAYS( aThreadPtr, ( EISICLTransceiverNULLPtr7 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    iShStateChangedDfc = new TDfc( StateChangedDfc, this, aThreadPtr, KDfcPriority );
    ASSERT_RESET_ALWAYS( iShStateChangedDfc, ( EISICLTransceiverMemAllocFailure6 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    
    iRxQueueDfc = new TDfc( RxQueueDfc, this, aThreadPtr, KDfcPriority );
    ASSERT_RESET_ALWAYS( iShStateChangedDfc, ( EISICLTransceiverMemAllocFailure8 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
            
    DISIDevice* dev = new DISIDevice( PN_DEV_MODEM,
                                      MISIRouterLinkIf::CreateLinkF( this, PN_MEDIA_MODEM_HOST_IF, ETrxSharedMemory)
                                      );
    ASSERT_RESET_ALWAYS( dev, ( EISICLTransceiverMemAllocFailure3 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    iShDevices.Append( dev );
#ifdef USE_MEDIAAPI 
    DISIDevice* dev2 = new DISIDevice( PN_DEV_PC,  // append to dynamic table if needed
                                       MISIRouterLinkIf::CreateLinkF( this, PN_MEDIA_USB, ETrxUSB )
                                      );
    ASSERT_RESET_ALWAYS( dev2, ( EISICLTransceiverMemAllocFailure9 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    iShDevices.Append( dev2 );
    
    DISIDevice* dev4 = new DISIDevice( PN_DEV_PC,  // append to dynamic table if needed
                                       MISIRouterLinkIf::CreateLinkF( this, PN_MEDIA_BT, ETrxBT )
                                      );
    ASSERT_RESET_ALWAYS( dev4, ( EISICLTransceiverMemAllocFailure10 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    iShDevices.Append( dev4 );
 #endif /* USE_MEDIAAPI */
	  DISIDevice* dev3 = new DISIDevice( PN_DEV_DUMMYIST,
                                       MISIRouterLinkIf::CreateLinkF( this, PN_MEDIA_TEST, ETrxTest )
                                      );                                      
    ASSERT_RESET_ALWAYS( dev3, ( EISICLTransceiverMemAllocFailure5 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    iShDevices.Append( dev3 );

    iRouterService = new DIsiRouterService( *this );
    ASSERT_RESET_ALWAYS( iRouterService, ( EISICLTransceiverMemAllocFailure2 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    
    iRxQueue = new DISIMsgQueue( KISIMainRxQueueSize );
    ASSERT_RESET_ALWAYS( iRxQueue, ( EISICLTransceiverMemAllocFailure7 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    
    TInt err( Kern::MutexCreate( iDynamicDeviceTableMutex, KISICLTransceiverMutex, KMutexOrdGeneral0 ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISICLTransceiverMutexCreateFailed | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
#ifdef USE_MEDIAAPI 
    iDevPcLastSendTrxId = EAmountOfTrxs;  //put a maxvalue	
    iDevPcLastActiveTrxId = EAmountOfTrxs;  //put a maxvalue
#endif /* USE_MEDIAAPI */
    C_TRACE( ( _T( "DISICLTransceiver::DISICLTransceiver<" ) ) );
    }

DISICLTransceiver::~DISICLTransceiver()
    {
    C_TRACE( ( _T( "DISICLTransceiver::~DISICLTransceiver>" ) ) );
    
    iShDevices.ResetAndDestroy();
    
    if ( iRouterService )
        {            
        delete iRouterService;
        iRouterService = NULL;
        }
        
    if ( iShStateChangedDfc )
        { 
        iShStateChangedDfc->Cancel();
        delete iShStateChangedDfc;
        iShStateChangedDfc = NULL;
        }
    
    if( iRxQueue )
        {
        C_TRACE( ( _T( "DISICLTransceiver::ResetQueues 0x%x iRxQueue 0x%x" ), this, iRxQueue ) );
        while( iRxQueue->Count() )
            {
            MemApi::DeallocBlock( iRxQueue->Get() );
            }
        }
        
    if( iRxQueueDfc )
        {
        C_TRACE( ( _T( "DISIUserChannel::~DISIUserChannel 0x%x iRxQueueDfc 0x%x" ), this, iRxQueueDfc ) );
        iRxQueueDfc->Cancel();
        delete iRxQueueDfc;
        iRxQueueDfc = NULL;
        }

    C_TRACE( ( _T( "DISICLTransceiver::~DISICLTransceiver<" ) ) );

    }

TInt DISICLTransceiver::SendCommIsaEntityNotReachableResp(
        TDes8& aNotDeliveredMessage
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::SendCommIsaEntityNotReachableResp 0x%x>" ), &aNotDeliveredMessage ) );
    const TUint8* notDeliveredMsgPtr( aNotDeliveredMessage.Ptr() );
    TInt error = KErrAlreadyExists;
    // Avoid COMM_ISA_ENTITY_NOT_REACHABLE_RESP loop.
    if( ( notDeliveredMsgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] == COMMON_MESSAGE ) &&
        ( ( notDeliveredMsgPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ] == COMM_ISA_ENTITY_NOT_REACHABLE_RESP ) || 
          ( notDeliveredMsgPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ] == COMM_SERVICE_NOT_IDENTIFIED_RESP ) ) )
        {
        C_TRACE( ( _T( "DISICLTransceiver Not sending another CommIsaEntityNotReachableResp 0x%x 0x%x" ), &aNotDeliveredMessage, notDeliveredMsgPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ] ) );
        }
    else
        {
        // Follows COMM specification: 000.031
        TUint8 length( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP );
        TDes8& respMsg = MemApi::AllocBlock( length );
        ASSERT_RESET_ALWAYS( length > ISI_HEADER_OFFSET_MESSAGEID, ( EISICLTransceiverOverTheLimits | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
        TUint8* respMsgPtr = const_cast<TUint8*>( respMsg.Ptr() );
        // We start to append from transaction id.
        respMsg.SetLength( ISI_HEADER_OFFSET_TRANSID );
        // Get the header until messageid from prev. message.
        // Just turn receiver and sender device and object vice versa.
        respMsgPtr[ ISI_HEADER_OFFSET_MEDIA ] = notDeliveredMsgPtr[ ISI_HEADER_OFFSET_MEDIA ];
        SET_RECEIVER_DEV( respMsgPtr, GET_SENDER_DEV( aNotDeliveredMessage ) );
        SET_SENDER_DEV  ( respMsgPtr, GET_RECEIVER_DEV( aNotDeliveredMessage ) );
        respMsgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = notDeliveredMsgPtr[ ISI_HEADER_OFFSET_RESOURCEID ];
        SET_LENGTH( respMsgPtr, ( length - PN_HEADER_SIZE ) );
        SET_RECEIVER_OBJ( respMsgPtr, GET_SENDER_OBJ( aNotDeliveredMessage ) );
        SET_SENDER_OBJ( respMsgPtr, GET_RECEIVER_OBJ( aNotDeliveredMessage ) );
        // Set from undelivered message
        respMsg.Append( notDeliveredMsgPtr[ ISI_HEADER_OFFSET_TRANSID ] );
        // Message Identifier
        respMsg.Append( COMMON_MESSAGE );
        // Sub message Identifier.
        respMsg.Append( COMM_ISA_ENTITY_NOT_REACHABLE_RESP );
        // Not Delivered Message from original message.
        respMsg.Append( notDeliveredMsgPtr[ ISI_HEADER_OFFSET_MESSAGEID ] );
        // Status
        respMsg.Append( COMM_ISA_ENTITY_NOT_AVAILABLE );//  different status in a case of device not existing
        // Filler
        const TUint8 KFiller( 0x00 );
        respMsg.Append( KFiller );
        // Filler
        respMsg.Append( KFiller );
        // Filler
        respMsg.Append( KFiller );
        error = RouteISIMessage( respMsg, EFalse );
        // Programming error in this function if below assert is raised
        ASSERT_RESET_ALWAYS( KErrNone == error, ( EISICLTransceiverCommIsaEntityNotReachableResp | EDISICLTransceiverTraceId << KClassIdentifierShift ) );        
        }
    MemApi::DeallocBlock( aNotDeliveredMessage );
    C_TRACE( ( _T( "DISICLTransceiver::SendCommIsaEntityNotReachableResp 0x%x<" ), &aNotDeliveredMessage ) );
    return error;

    }

// This is called in 1...N thread contextes
void DISICLTransceiver::ReceiveISIMessage(
        const TDesC8& aMessage,
        const TUint8 aTrxId
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::ReceiveMsg 0x%x 0x%x 0x%x>" ), this, &aMessage, aTrxId ) );
    // Can only be called from thread context.
    ASSERT_THREAD_CONTEXT_ALWAYS( ( EISICLTransceiverNotThreadContext | EDISIUserChannelTraceId << KClassIdentifierShift ) );
    
    TUint8 txDevId = GET_SENDER_DEV( aMessage.Ptr() );    
    if ( DynamicDevice( txDevId ) )
        {
        TUint8 staticDevId(0);
        const TInt count( iShDevices.Count() );        
        TInt index(0);
        TBool staticDeviceFound(EFalse);
        MISIRouterLinkIf* Link = NULL;
        while( !staticDeviceFound && ( index < count ) )
            {
            C_TRACE( ( _T( "DISICLTransceiver::ReceiveMsg find static device for trx 0x%x index %d" ), aTrxId, index ) );
            DISIDevice* tmpDevice = iShDevices[ index ];
            ASSERT_RESET_ALWAYS( tmpDevice, ( EISICLTransceiverNULLPtr4 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
            tmpDevice->LockDeviceFM();
            Link = tmpDevice->GetLink();
            if( Link->GetTrxId() == aTrxId )
                {                
                staticDevId = tmpDevice->GetDeviceIdentifier();
                tmpDevice->FreeDeviceFM();
                C_TRACE( ( _T( "DISICLTransceiver::ReceiveMsg static device 0x%x trx 0x%x" ), this, &aMessage, aTrxId ) );
                staticDeviceFound = ETrue;
                }
            else
                {
                tmpDevice->FreeDeviceFM();
                }
            index++;
            } 
#ifdef USE_MEDIAAPI 		 
        if ( PN_DEV_PC == GET_SENDER_DEV( aMessage.Ptr() ))
            {
            C_TRACE( ( _T( "DISICLTransceiver::ReceiveMsg 0x%x 0x%x trx id = %d<" ), this, &aMessage, (Link->GetTrxId())  ) ); 
            iDevPcLastActiveTrxId = iDevPcLastSendTrxId;                  	
            iDevPcLastSendTrxId =	Link->GetTrxId();                       	    
            }		     
#endif /* USE_MEDIAAPI */  			
        UpdateDynamicDeviceTable( txDevId, staticDevId );
        }
    // else static device. No need to save trxId

    iRxQueue->Add( aMessage );
    iRxQueueDfc->Enque();
    C_TRACE( ( _T( "DISICLTransceiver::ReceiveMsg 0x%x 0x%x 0x%x<" ), this, &aMessage, aTrxId ) );
    }

void DISICLTransceiver::RxQueueDfc(
        TAny* aPtr  // Pointer to this object.
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::RxQueueDfc ->" ) ) );
    
    DISICLTransceiver& tmp = *reinterpret_cast<DISICLTransceiver*>( aPtr );
    if( tmp.iRxQueue->Count() > 0 )
        {
        TDes8& msg( tmp.iRxQueue->Get() );
        
        tmp.RouteISIMessage( msg, EFalse );
        // Check here too to avoid unnecessary dfc queuing.
        if( tmp.iRxQueue->Count() > 0 )
            {
            C_TRACE( ( _T( "DISICLTransceiver::RxQueueDfc enque RxQueueDfc" ) ) );
            tmp.iRxQueueDfc->Enque();
            }
        }
    C_TRACE( ( _T( "DISICLTransceiver::RxQueueDfc <-" ) ) );
    }
    

TInt DISICLTransceiver::RouteISIMessage(
        TDes8& aMessage,
        TBool aDynamicSenderCheckNeeded
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage 0x%x 0x%x> len %d" ), &aMessage, aDynamicSenderCheckNeeded, aMessage.Length() ) );
    const TUint8* msgPtr = aMessage.Ptr();
    ISIMESSAGE_TRACE( aMessage );
    TInt error( ValidateISIMessage( aMessage ) );
    if( KErrNone != error )
        {
        TRACE_ASSERT_ALWAYS;
        C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage invalid message 0x%x" ), &aMessage ) );
        MemApi::DeallocBlock( aMessage );
        }
    else
        {
        TBool sendOk( EFalse );
        if ( aDynamicSenderCheckNeeded )// Save dynamic device ids from PN_DEV_OWN
            {
            C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage from ISI Router" ) ) );
            TUint8 txDeviceIdentifier = GET_SENDER_DEV( aMessage );
            if ( DynamicDevice( txDeviceIdentifier ) )
                {
                C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage from ISI Router Dynamic Dev" ) ) );                
                UpdateDynamicDeviceTable( txDeviceIdentifier, PN_DEV_OWN );
#ifndef USE_MEDIAAPI 				
                ASSERT_RESET_ALWAYS( txDeviceIdentifier == PN_DEV_PC, 0xdeaddead );// only supported PN_DEV_PC with EIscNokiaUsbPhonetLink
#endif /* USE_MEDIAAPI */  					
                }
            }

        TUint8 rxDeviceIdentifier = GET_RECEIVER_DEV( aMessage );
        C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage rxDeviceIdentifier 0x%x" ), rxDeviceIdentifier ) );
        
        if ( DynamicDevice( rxDeviceIdentifier ) )
            {            
            DynamicToStaticDevice( rxDeviceIdentifier );            
            C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage rxDeviceIdentifier after 0x%x" ), rxDeviceIdentifier ) );
            }


        if( rxDeviceIdentifier == PN_DEV_OWN )
            {
            C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage PN_DEV_OWN 0x%x" ), &aMessage ) );
            TUint16 objId(0xffff);
#ifndef USE_MEDIAAPI 
            if ( GET_RECEIVER_DEV( aMessage ) == PN_DEV_PC )
                {
                C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage to PN_DEV_PC in PN_DEV_OWN" )) );
                objId = EIscNokiaUsbPhonetLink;
                }
            else
                {
                objId = GET_RECEIVER_OBJ( aMessage );
                }
#endif /* USE_MEDIAAPI */  					
#ifdef USE_MEDIAAPI 
            objId = GET_RECEIVER_OBJ( aMessage );
#endif /* USE_MEDIAAPI */  				
            sendOk = iShRouter.Receive( aMessage, objId );
            C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage PN_DEV_OWN 0x%x ok %d" ), &aMessage, sendOk ) );
            }        
        else
            {
            C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage Not PN_DEV_OWN 0x%x" ), &aMessage ) );
            MISIRouterLinkIf* link = NULL;


            const TInt count( iShDevices.Count() );
            for( TInt index( 0 ); index < count; index++ )
                {
                DISIDevice* tmpDevice = iShDevices[ index ];
                ASSERT_RESET_ALWAYS( tmpDevice, ( EISICLTransceiverNULLPtr4 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
                tmpDevice->LockDeviceFM();
                if( tmpDevice->GetDeviceIdentifier() == rxDeviceIdentifier ) 
                    {
                    link = tmpDevice->GetLink();
                    tmpDevice->FreeDeviceFM();
#ifndef USE_MEDIAAPI 					
                    sendOk = link->Send( aMessage );
                    break;
#endif /* USE_MEDIAAPI */  	
#ifdef USE_MEDIAAPI 						
                    if ( GET_RECEIVER_DEV( aMessage ) == PN_DEV_PC )
                        {
                        C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage PN_DEV_PC %d, %d" ), link->GetTrxId(), iDevPcLastSendTrxId ) ); 	
                        if(iDevPcLastSendTrxId < EAmountOfTrxs)
                            {	
                            if ( link->GetTrxId() ==	iDevPcLastSendTrxId )
                                {
                                if (link->StateConnected())
                                    {
                                    C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage PN_DEV_PC %d, Msg mediaid 0x%x" ), link->GetTrxId(), msgPtr[ ISI_HEADER_OFFSET_MEDIA ] ) ); 
                                    sendOk = link->Send( aMessage );
                                    break;
                                    }
                                }
                            }    
                        else
                            {
                            break;	
                            }		   	
		                 }
                    else
                    	{    			
                        sendOk = link->Send( aMessage );
                        break;
                        }
#endif /* USE_MEDIAAPI */  							
                    }    
                else
                    {
                    tmpDevice->FreeDeviceFM();
                    }
                }
            C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage Not PN_DEV_OWN ok %d" ), &aMessage, sendOk ) );
            }
        if( !sendOk )
            {
            TRACE_ASSERT_ALWAYS;
            error = SendCommIsaEntityNotReachableResp( aMessage );
            }
        }
    C_TRACE( ( _T( "DISICLTransceiver::RouteISIMessage 0x%x<" ), &aMessage ) );
    return error;
    }

void DISICLTransceiver::GetDeviceConnectionStates(
        RArray<TUint8>& aDeviceIdentifierList,
        const TBool aConnectedDevice
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::GetDeviceConnectionStates 0x%x 0x%x>" ), &aDeviceIdentifierList, aConnectedDevice ) );

    const TInt count( iShDevices.Count() );
    for( TInt index( 0 ); index < count; index++ )
        {
        DISIDevice* tmpDevice = iShDevices[ index ];
        ASSERT_RESET_ALWAYS( tmpDevice, ( EISICLTransceiverNULLPtr3 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
        tmpDevice->LockDeviceFM();
        if( tmpDevice->GetDeviceState() == aConnectedDevice )
            {
            const TUint8 deviceIdentifier( tmpDevice->GetDeviceIdentifier() );
            tmpDevice->FreeDeviceFM();
            aDeviceIdentifierList.Append( deviceIdentifier );
            }
        else
            {
            tmpDevice->FreeDeviceFM();
            }
        }
    C_TRACE( ( _T( "DISICLTransceiver::GetDeviceConnectionStates 0x%x 0x%x<" ), &aDeviceIdentifierList, aConnectedDevice ) );
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
    TRACE_ASSERT_INFO( msgOk == KErrNone, isiMsgLength );
    TRACE_ASSERT_INFO( msgOk == KErrNone, descLength );
    C_TRACE( ( _T( "DISICLTransceiver::ValidateISIMessage %d<" ), msgOk ) );
    return msgOk;
    }

// NOTE!! Called with FM held. Not allowed to block or nothing. Not even print traces!
// Called in N Thread context
void DISICLTransceiver::StateChanged(
        // None
        )
    {

    iShStateChangedDfc->Enque();

    }

void DISICLTransceiver::StateChangedDfc(
        TAny* aPtr
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::StateChangedDfc>" ) ) );
    DISICLTransceiver* self = reinterpret_cast<DISICLTransceiver*>( aPtr );
    ASSERT_RESET_ALWAYS( self, ( EISICLTransceiverNULLPtr5 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    self->ReadStateChanges();
    C_TRACE( ( _T( "DISICLTransceiver::StateChangedDfc<" ) ) );
    }

void DISICLTransceiver::ReadStateChanges(
        // None
        )
    {

    C_TRACE( ( _T( "DISICLTransceiver::ReadStateChanges>" ) ) );

    const TInt count( iShDevices.Count() );
    for( TInt index( 0 ); index < count; index++ )
        {
        DISIDevice* tmpDevice = iShDevices[ index ];
        ASSERT_RESET_ALWAYS( tmpDevice, ( EISICLTransceiverNULLPtr6 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
        tmpDevice->LockDeviceFM();
        const TBool connected = tmpDevice->GetDeviceState();//  tee paremmaksi
	
#ifdef USE_MEDIAAPI 		
        MISIRouterLinkIf* link = NULL;     
        link = tmpDevice->GetLink();
#endif /* USE_MEDIAAPI */                       	
        if( connected != tmpDevice->GetDeviceOldState() )
            {
            tmpDevice->SetDeviceOldState( connected );
            const TUint8 deviceIdentifier = tmpDevice->GetDeviceIdentifier();
            tmpDevice->FreeDeviceFM();
#ifdef USE_MEDIAAPI 
            C_TRACE( ( _T( "DISICLTransceiver::ReadStateChanges trxid 0x%x state changed>" ),link->GetTrxId() ) );   
            if( (link->GetTrxId() == iDevPcLastSendTrxId) && (!connected) )
                {
                C_TRACE( ( _T( "DISICLTransceiver::ReadStateChanges trxid 0x%x disconneted>" ),link->GetTrxId() ) );   	
         	      iDevPcLastSendTrxId = iDevPcLastActiveTrxId;
         	      iDevPcLastActiveTrxId = EAmountOfTrxs;  //put a maxvalue	
                } 
#endif /* USE_MEDIAAPI */   					 
            iRouterService->SendDeviceConnectionStateChangedInd( connected, deviceIdentifier );
            }
        else
            {
            tmpDevice->FreeDeviceFM();
            }
        }
    C_TRACE( ( _T( "DISICLTransceiver::ReadStateChanges<" ) ) );
    }

TBool DISICLTransceiver::DynamicDevice( const TUint8 aDevId )
    {
    C_TRACE( ( _T( "DISICLTransceiver::DynamicDevice 0x%x>" ), aDevId ) );
    TBool dynamicDevice(EFalse);
    switch( aDevId )
        {        
        case PN_DEV_MODEM:
        case PN_DEV_DUMMYIST:
        case PN_DEV_OWN:
            {
            //already set as EFalse
            break;
            }
        default:
            {
            dynamicDevice = ETrue;
            break;
            }
        }
    C_TRACE( ( _T( "DISICLTransceiver::DynamicDevice 0x%x<" ), dynamicDevice ) );
    return dynamicDevice;
    }
    
void DISICLTransceiver::UpdateDynamicDeviceTable( const TUint8 aDynamicDevId, const TUint8 aStaticDevId )
    {
    C_TRACE( ( _T( "DISICLTransceiver::UpdateDynamicDeviceTable 0x%x 0x%x>" ), aDynamicDevId, aStaticDevId) );
    TBool deviceExist(EFalse);
    TInt count(iDynamicDeviceTable.Count());
    TInt err( Kern::MutexWait( *iDynamicDeviceTableMutex ) );
    ASSERT_RESET_ALWAYS( ( err == KErrNone ), ( EISICLTransceiverMutexWaitFailed2 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    for( TUint8 i = 0; i < count; i++ )
        {
        C_TRACE( ( _T( "DISICLTransceiver::UpdateDynamicDeviceTable find dynamic device %d" ), i) );
        if( iDynamicDeviceTable[ i ]->iDynamicDevId == aDynamicDevId )
            {                    
            C_TRACE( ( _T( "DISICLTransceiver::UpdateDynamicDeviceTable dyn dev exist i = %d aDynamicDevId 0x%x staticDevId 0x%x previous 0x%x" ), i, iDynamicDeviceTable[ i ]->iDynamicDevId, aStaticDevId, iDynamicDeviceTable[ i ]->iStaticDevId ) );
            iDynamicDeviceTable[ i ]->iStaticDevId = aStaticDevId;
            deviceExist = ETrue;
            break;
            }
        }
        
    if ( !deviceExist )
        {
        C_TRACE( ( _T( "DISICLTransceiver::UpdateDynamicDeviceTable new dynamic device added 0x%x 0x%x" ), aDynamicDevId, aStaticDevId ) );
        iDynamicDeviceTable.Append( new TDynamicDevice( aDynamicDevId, aStaticDevId ) );
        }
    Kern::MutexSignal( *iDynamicDeviceTableMutex );
    C_TRACE( ( _T( "DISICLTransceiver::UpdateDynamicDeviceTable 0x%x 0x%x<" ), aDynamicDevId, aStaticDevId) );
    }

void DISICLTransceiver::DynamicToStaticDevice( TUint8& aRxDev )
    {
    C_TRACE( ( _T( "DISICLTransceiver::DynamicToStaticDevice aRxDev>" ), &aRxDev ) );
    TInt err( Kern::MutexWait( *iDynamicDeviceTableMutex ) );
    ASSERT_RESET_ALWAYS( ( err == KErrNone ), ( EISICLTransceiverMutexWaitFailed | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    for( TUint8 i = 0; i < iDynamicDeviceTable.Count(); i++ )
        {
        if( iDynamicDeviceTable[ i ]->iDynamicDevId == aRxDev )
            {                    
            C_TRACE( ( _T( "DISICLTransceiver::DynamicToStaticDevice dyn dev exist i = %d iDynamicDevId 0x%x iStaticDevId 0x%x" ), i, iDynamicDeviceTable[ i ]->iDynamicDevId, iDynamicDeviceTable[ i ]->iStaticDevId) );
            aRxDev = iDynamicDeviceTable[ i ]->iStaticDevId;
            break;
            }
        }
    Kern::MutexSignal( *iDynamicDeviceTableMutex );
    C_TRACE( ( _T( "DISICLTransceiver::DynamicToStaticDevice aRxDev<" ), &aRxDev ) );
    }
        

DISICLTransceiver::DISIDevice::DISIDevice(
        const TUint8 aDeviceIdentifier,
        MISIRouterLinkIf* aLink
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::DISIDevice 0x%x 0x%x 0x%x>" ), aDeviceIdentifier, aLink ) );
    iShDeviceIdentifier = aDeviceIdentifier;
    iShOldDeviceConnectedState = EFalse;
    ASSERT_RESET_ALWAYS( aLink, ( EISICLTransceiverNULLPtr2 | EDISICLTransceiverTraceId << KClassIdentifierShift ) );
    iShLink = aLink;
    iShDeviceMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iShDeviceMutex, EISICLTransceiverNULLPtr6 | EDISICLTransceiverTraceId << KClassIdentifierShift );
    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::DISIDevice<" ) ) );
    }

DISICLTransceiver::DISIDevice::~DISIDevice()
    {
    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::~DISIDevice>" ) ) );
    iShLink->Release();
    iShLink = NULL;
    // If not exists faulted already.
    delete iShDeviceMutex;
    iShDeviceMutex = NULL;
    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::~DISIDevice<" ) ) );
    }

MISIRouterLinkIf* DISICLTransceiver::DISIDevice::GetLink()
    {
//    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::GetLink<> 0x%x" ), iShLink ) );
    return iShLink;
    }

TUint8 DISICLTransceiver::DISIDevice::GetDeviceIdentifier()
    {
//    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::GetDeviceIdentifier<> 0x%x" ), iShDeviceIdentifier ) );
    return iShDeviceIdentifier;
    }

TBool DISICLTransceiver::DISIDevice::GetDeviceState()
    {
//    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::GetDeviceState<>" ) ) );
    return iShLink->StateConnected();
    }

TBool DISICLTransceiver::DISIDevice::GetDeviceOldState()
    {
//    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::GetDeviceOldState<> 0x%x" ), iShOldDeviceConnectedState ) );
    return iShOldDeviceConnectedState;
    }

void DISICLTransceiver::DISIDevice::SetDeviceOldState(
        TBool aConnected
        )
   {
//   C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::SetDeviceOldState<> 0x%x 0x%x" ), iShOldDeviceConnectedState, aConnected ) );
   iShOldDeviceConnectedState = aConnected;    
   }

void DISICLTransceiver::DISIDevice::LockDeviceFM(
        // None
        )
    {
    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::LockDeviceFM<>" ) ) );    
    NKern::FMWait( iShDeviceMutex );
    }

void DISICLTransceiver::DISIDevice::FreeDeviceFM(
        // None
        )
    {
    NKern::FMSignal( iShDeviceMutex );
    C_TRACE( ( _T( "DISICLTransceiver::DISIDevice::FreeDeviceFM<>" ) ) );    
    }
