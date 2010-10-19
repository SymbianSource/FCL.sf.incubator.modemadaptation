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
#include <phonetisi.h>                    // For ISI_HEADER_SIZE
#include <pn_const.h>                     // For PN_OBJ_ROUTING_REQ...
#include <commisi.h>                      // For Common messages
#include <pn_eventmodem_extisi.h>         // PN_COMMG, PNS_SUBSCRIBED_RESOURCES_...
#include "isicommunicationmanager.h"      // For DISICommunicationManager
#include "misiobjectrouterif.h"           // For MISIObjectRouterIf
#include "isicommunicationmanagertrace.h" // For C_TRACE, ASSERT_RESET.. and fault codes
#include "memapi.h"                       // For MemApi

// HOX! nsisi.h is this also exported by isimsg
#include "nsisi.h"                        // For PN_NAMESERVICE.. 

#include "isihelpers.h"                   // For SET_RECEIVER_OBJ...

#include "isirouter.h"                    // For GetDFCThread

#include "isiindicationhandler.h"         // For DISIIndicationHandler
#include "ape_commgrisi.h"                // For APE_COMMGR..
#include "misicommunicationmanagerif.h"   // For MISICommunicationManagerIf


//#define ISCE_NO_NEED_TO_VALIDATE_RESOURCE // to skip resource validity check

// Faults
enum TISICommunicationManagerFaults
    {
    EISICommunicationManagerMemAllocFailure = 0x01,
    EISICommunicationManagerMemAllocFailure1,
    EISICommunicationManagerNullPointer,
    EISICommunicationManagerNullPointer1,
    EISICommunicationManagerNullPointer2,
    EISICommunicationManagerNullPointer3,
    EISICommunicationManagerNullPointer4,
    EISICommunicationManagerNullPointer5,
    EISICommunicationManagerNullPointer6,
    EISICommunicationManagerNullPointer7,
    EISICommunicationManagerNullPointer8,
    EISICommunicationManagerNullPointer9,
    EISICommunicationManagerNullPointer10,
    EISICommunicationManagerNullPointer11,
    EISICommunicationManagerNullPointer12,
    EISICommunicationManagerNullPointer13,
    EISICommunicationManagerNullPointer14,
    EISICommunicationManagerNullPointer15,
    EISICommunicationManagerNullPointer16,        
    EISICommunicationManagerUnknownMessage,
    EISICommunicationManagerMutexCreateFailed,
    EISICommunicationManagerMutexWaitFailed,
    EISICommunicationManagerWrongMessageOrder,
    EISICommunicationManagerWrongMessageOrder2,
    EISICommunicationManagerWrongMessage,
    EISICommunicationManagerFatalError,
    EISICommunicationInvalidLength,
    EISICommunicationInvalidLength1,
    EISICommunicationInvalidLength2,
    EISICommunicationInvalidLength3,
    EISICommunicationInvalidLength4,
    EISICommunicationInvalidLength5,
    EISICommunicationInvalidLength6,
    EISICommunicationInvalidLength7,
    EISICommunicationInvalidLength8,
    EISICommunicationInvalidLength9,
    EISICommunicationInvalidLength10,
    EISICommunicationInvalidLength11,
    EISICommunicationInvalidLength12,
    EISICommunicationInvalidLength13,
    EISICommunicationInvalidLength14,
    EISICommunicationManagerSendFailed,
    EISICommunicationManagerSendFailed1,
    EISICommunicationManagerSendFailed2,
    EISICommunicationManagerSendFailed3,
    EISICommunicationManagerSendFailed4,
    };

DMutex* DISICommunicationManager::iCommunicationManagerMutex = NULL;
_LIT8( KCommunicationManagerMutex, "KCommunicationManagerMutex" );

const TUint32 KCommunicationManagerUID( 0x2002B3D0 );
const TUint8 KFiller( 0 );
const TInt KInitDfcPriority( 7 );
const TUint8 KDone( 0 );
const TUint8 KDefaultTransactionIdentifier( 0x00 );
const TUint8 KUnitializedObjectIdentifier( 0xff );
const TUint8 KNumberOfBootMessage( 0x04 );

DISICommunicationManager::DISICommunicationManager(
        // None
        )
    : iObjectIdentifier( KUnitializedObjectIdentifier ),
      iBootMsgCount( KNumberOfBootMessage )
    {
    C_TRACE( ( _T( "DISICommunicationManager::DISICommunicationManager 0x%x 0x%x>" ), iObjectIdentifier, iRouter ) );
    iRouter = MISIObjectRouterIf::GetIf();    
    ASSERT_RESET_ALWAYS( iRouter, ( EISICommunicationManagerNullPointer | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    iRouter->Connect( KCommunicationManagerUID, iObjectIdentifier, this );
    ASSERT_RESET_ALWAYS( iObjectIdentifier == PN_OBJ_EVENT_MULTICAST, ( EISICommunicationManagerFatalError | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    // Ordinal three as there is zero, one and two used in other places of ISCE.
    TInt err( Kern::MutexCreate( iCommunicationManagerMutex, KCommunicationManagerMutex, KMutexOrdGeneral3 ) );
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISICommunicationManagerMutexCreateFailed | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    iInitDfc = new TDfc( InitDfc, this, iRouter->GetDfcThread( MISIObjectRouterIf::EISIKernelMainThread ), KInitDfcPriority );
    ASSERT_RESET_ALWAYS( iInitDfc, ( EISICommunicationManagerMemAllocFailure | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    iISIIndicationHandler = new DISIIndicationHandler( iRouter );
    ASSERT_RESET_ALWAYS( iISIIndicationHandler, ( EISICommunicationManagerMemAllocFailure1 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    iInitDfc->Enque();
    C_TRACE( ( _T( "DISICommunicationManager::DISICommunicationManager 0x%x 0x%x<" ), iObjectIdentifier, iRouter ) );
    }

void DISICommunicationManager::InitDfc(
        TAny* aPtr  // Pointer to this object.
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::InitDfc 0x%x>" ), aPtr ) );
    DISICommunicationManager* tmp = reinterpret_cast<DISICommunicationManager*>( aPtr );
    ASSERT_RESET_ALWAYS( tmp, ( EISICommunicationManagerNullPointer3 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    tmp->SendNameAddReqs();
    C_TRACE( ( _T( "DISICommunicationManager::InitDfc 0x%x<" ), aPtr ) );
    }

void DISICommunicationManager::SendNameAddReqs(
        // None
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendNameAddReqs>" ) ) );
    //PN_COMMGR name
    TUint16 msgLength( ISI_HEADER_SIZE + SIZE_PNS_NAME_ADD_REQ );
    TDes8& addMsg = MemApi::AllocBlock( msgLength );
    addMsg.SetLength( msgLength );
    TUint8* addPtr = const_cast<TUint8*>( addMsg.Ptr() );
    ASSERT_RESET_ALWAYS( addPtr, ( EISICommunicationManagerNullPointer1 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( addMsg.Length() > ( ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ) ),
                         ( EISICommunicationInvalidLength | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    addPtr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
    SET_RECEIVER_DEV( addPtr, PN_DEV_OWN );
    SET_SENDER_DEV( addPtr, PN_DEV_OWN );
    addPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_NAMESERVICE;
    SET_LENGTH( addPtr, ( msgLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( addPtr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( addPtr, iObjectIdentifier );
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_UTID ] = KDefaultTransactionIdentifier;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_SUBFUNCTION ] = PNS_NAME_ADD_REQ;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED1 ] = KFiller;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_RESERVED2 ] = KFiller;
    PUTB32( &addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], PN_COMMGR );
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_DEV ] = PN_DEV_OWN;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_OBJ ] = iObjectIdentifier;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS ] = PN_NAME_NOCHG;
    addPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED ] = KFiller;
    //PN_APE_COMMGR name
    TDes8& addMsg2 = MemApi::AllocBlock( msgLength );
    addMsg2.SetLength( msgLength );
    addMsg2.Copy( addMsg );
    TUint8* addPtr2 = const_cast<TUint8*>( addMsg2.Ptr() );
    ASSERT_RESET_ALWAYS( addPtr2, ( EISICommunicationManagerNullPointer2 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( addMsg2.Length() > ( ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ) ),
                         ( EISICommunicationInvalidLength1 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    PUTB32( &addPtr2[ ISI_HEADER_SIZE + PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY + PN_NAME_SRV_ITEM_STR_OFFSET_NAME ], PN_APE_COMMGR );
    TInt sendResult = iRouter->Send( addMsg, iObjectIdentifier );
    ASSERT_RESET_ALWAYS( ( sendResult == KErrNone ), ( EISICommunicationManagerSendFailed3 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );    
    sendResult = iRouter->Send( addMsg2, iObjectIdentifier );
    ASSERT_RESET_ALWAYS( ( sendResult == KErrNone ), ( EISICommunicationManagerSendFailed4 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );    
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
    if ( iInitDfc )
        {
        iInitDfc->Cancel();
        delete iInitDfc;
        iInitDfc = NULL;
        }
    if( iCommunicationManagerMutex )
        {
        ((DObject*)iCommunicationManagerMutex)->Close( NULL );
        iCommunicationManagerMutex = NULL;
        }
    
    C_TRACE( ( _T( "DISICommunicationManager::~DISICommunicationManager<" ) ) );
    }

void DISICommunicationManager::Receive(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::Receive 0x%x>" ), &aMessage ) );
    TInt err( Kern::MutexWait( *iCommunicationManagerMutex ) );//! check usage of these mutexes (be aware of ordering violations)
    ASSERT_RESET_ALWAYS( ( KErrNone == err ), ( EISICommunicationManagerMutexWaitFailed | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    const TUint8* msgPtr( aMessage.Ptr() );
    TDes8* blockPtr = reinterpret_cast<TDes8*>( const_cast<TDesC8*>(&aMessage) );
    if( iBootMsgCount == KDone )
        {
        switch(  msgPtr[ ISI_HEADER_OFFSET_RESOURCEID ] )
            {
            case PN_APE_COMMGR:
                {
                HandleApeCommunicationManagerMessage( *blockPtr );
                break;
                }
            case PN_COMMGR:
                {
                HandlePnCommgrMessage( *blockPtr );
                break;
                }
            default:
                {
                HandleMessageMulticast( *blockPtr );
                break;
                }
            }
        }
    else
        {
        C_TRACE( ( _T( "DISICommunicationManager booting %d" ), iBootMsgCount ) );
        HandleBootMessage( *blockPtr );
        }
    MemApi::DeallocBlock( *blockPtr );
    Kern::MutexSignal( *iCommunicationManagerMutex );
    C_TRACE( ( _T( "DISICommunicationManager::Receive 0x%x<" ), &aMessage ) );
    }

void DISICommunicationManager::HandleBootMessage(
        const TDesC8& aMessageDuringBoot
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::HandleBootMessage 0x%x>" ), &aMessageDuringBoot ) );
    const TUint8* messageDuringBootPtr = aMessageDuringBoot.Ptr();
    ASSERT_RESET_ALWAYS( messageDuringBootPtr, ( EISICommunicationManagerNullPointer7 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aMessageDuringBoot.Length() > ( ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ) ),
                         ( EISICommunicationInvalidLength5 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );    
    if( messageDuringBootPtr[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_NAMESERVICE )
        {
        switch( messageDuringBootPtr[ ISI_HEADER_OFFSET_MESSAGEID ] )
            {
            case PNS_NAME_ADD_IND :
                {
                C_TRACE( ( _T( "DISICommunicationManager::HandleBootMessage PNS_NAME_ADD_IND" ) ) );
                iBootMsgCount--;
                break;
                }
            case PNS_NAME_ADD_RESP :
                {
                C_TRACE( ( _T( "DISICommunicationManager::HandleBootMessage PNS_NAME_ADD_RESP" ) ) );
                ASSERT_RESET_ALWAYS( ( messageDuringBootPtr[ ISI_HEADER_SIZE + PNS_NAME_ADD_RESP_OFFSET_REASON ] == PN_NAME_OK ), ( EISICommunicationManagerWrongMessage | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
                iBootMsgCount--;
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, ( EISICommunicationManagerWrongMessageOrder2 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
                break;
                }
            }
        }
    else
        {
        ASSERT_RESET_ALWAYS( 0, ( EISICommunicationManagerWrongMessageOrder | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
        }
    C_TRACE( ( _T( "DISICommunicationManager::HandleBootMessage 0x%x<" ), &aMessageDuringBoot ) );
    }

void DISICommunicationManager::HandleApeCommunicationManagerMessage( 
        const TDesC8& aMessageToCommunicationManager
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::HandleApeCommunicationManagerMessage 0x%x>" ), &aMessageToCommunicationManager ) );
    const TUint8* messagePtr = aMessageToCommunicationManager.Ptr();
    ASSERT_RESET_ALWAYS( messagePtr, ( EISICommunicationManagerNullPointer9 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aMessageToCommunicationManager.Length() > ( ISI_HEADER_OFFSET_MESSAGEID ) ),
                         ( EISICommunicationInvalidLength7 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );    
    if( messagePtr[ ISI_HEADER_OFFSET_MESSAGEID ] == COMMON_MESSAGE )
        {
        HandleCommunicationManagerCommonMessage( aMessageToCommunicationManager );		
        }
    else
        {
        C_TRACE( ( _T( "DISICommunicationManager::HandleApeCommunicationManagerMessage subscription x%x" ), &aMessageToCommunicationManager ) );
        switch( messagePtr[ ISI_HEADER_OFFSET_MESSAGEID ] )
            {
            case APE_COMMGR_SUBSCRIBE_REQ:
                {
                SendApeCommgrSubscribeResp( aMessageToCommunicationManager );
                iISIIndicationHandler->Subscribe( aMessageToCommunicationManager );        
                break;
                }
            default:
                {
                TRACE_ASSERT_ALWAYS;
                SendCommServiceNotIdentifiedResp( aMessageToCommunicationManager );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DISICommunicationManager::HandleApeCommunicationManagerMessage 0x%x<" ), &aMessageToCommunicationManager ) );	
    }

void DISICommunicationManager::HandlePnCommgrMessage(
        const TDesC8& aMessageToCommunicationManager
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::HandleCellmoCommunicationManagerMessage 0x%x>" ), &aMessageToCommunicationManager ) );
    const TUint8* messagePtr = aMessageToCommunicationManager.Ptr();
    ASSERT_RESET_ALWAYS( messagePtr, ( EISICommunicationManagerNullPointer15 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aMessageToCommunicationManager.Length() > ( ISI_HEADER_OFFSET_MESSAGEID ) ),
                         ( EISICommunicationInvalidLength13 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );    
    if( messagePtr[ ISI_HEADER_OFFSET_MESSAGEID ] == COMMON_MESSAGE )
        {
        HandleCommunicationManagerCommonMessage( aMessageToCommunicationManager );	
        }
    else
        {
        C_TRACE( ( _T( "DISICommunicationManager::HandleCellmoCommunicationManagerMessage subscription x%x" ), &aMessageToCommunicationManager ) );
        switch( messagePtr[ ISI_HEADER_OFFSET_MESSAGEID ] )
             {
             case PNS_SUBSCRIBED_RESOURCES_EXTEND_IND: //PNS_SUBSCRIBED_RESOURCES_EXTEND_IND:
                {
                iISIIndicationHandler->Subscribe( aMessageToCommunicationManager );          		  	
                break;
                }
            default:
               {
               TRACE_ASSERT_ALWAYS;
               SendCommServiceNotIdentifiedResp( aMessageToCommunicationManager );              
               break;
               }
            }
        }
    C_TRACE( ( _T( "DISICommunicationManager::HandleCellmoCommunicationManagerMessage 0x%x<" ), &aMessageToCommunicationManager ) );
    }

void DISICommunicationManager::HandleCommunicationManagerCommonMessage(
        const TDesC8& aMessageToCommunicationManager
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::HandleCommunicationManagerCommonMessage 0x%x>" ), &aMessageToCommunicationManager ) );
    const TUint8* messagePtr = aMessageToCommunicationManager.Ptr();
    ASSERT_RESET_ALWAYS( messagePtr, ( EISICommunicationManagerNullPointer16 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aMessageToCommunicationManager.Length() > ( ISI_HEADER_OFFSET_MESSAGEID ) ),
                         ( EISICommunicationInvalidLength14 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    if( messagePtr[ ISI_HEADER_OFFSET_MESSAGEID ] == COMMON_MESSAGE )
        {
        ASSERT_RESET_ALWAYS( ( aMessageToCommunicationManager.Length() > ( ISI_HEADER_OFFSET_SUBMESSAGEID ) ),
                         ( EISICommunicationInvalidLength9 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
        switch( messagePtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ] )
            {
            case COMM_ISA_ENTITY_NOT_REACHABLE_RESP:
                {
                C_TRACE( ( _T( "DISICommunicationManager::HandleCommunicationManagerCommonMessage COMM_ISA_ENTITY_NOT_REACHABLE_RESP0 x%x" ), &aMessageToCommunicationManager ) );
                break;
                }
            case COMM_ISI_VERSION_GET_REQ:
                {
                C_TRACE( ( _T( "DISICommunicationManager::HandleCommunicationManagerCommonMessage COMM_ISI_VERSION_GET_REQ x%x" ), &aMessageToCommunicationManager ) );
                SendCommIsiVersionGetResp( aMessageToCommunicationManager );
                break;
                }
            default:
                {
                C_TRACE( ( _T( "DISICommunicationManager::HandleCommunicationManagerCommonMessage unknown COMMON_MESSAGE x%x" ), &aMessageToCommunicationManager ) );
                SendCommServiceNotIdentifiedResp( aMessageToCommunicationManager );
                break;
                }
             }
        }
    }

void DISICommunicationManager::HandleMessageMulticast(
        const TDesC8& aMessageToMulticast
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::HandleMessageMulticast 0x%x>" ), &aMessageToMulticast ) );
    const TUint8* messageToMulticastPtr = aMessageToMulticast.Ptr();
    ASSERT_RESET_ALWAYS( messageToMulticastPtr, ( EISICommunicationManagerNullPointer10 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aMessageToMulticast.Length() > ( ISI_HEADER_OFFSET_SENDERDEVICE ) ),
                         ( EISICommunicationInvalidLength8 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );    
    if( ( messageToMulticastPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] != PN_DEV_OWN ) ||
#ifndef ISCE_NO_NEED_TO_VALIDATE_RESOURCE
        ( MISICommunicationManagerIf::IsValidResource( aMessageToMulticast ) )
#else
        ETrue
#endif
      )
        {
        C_TRACE( ( _T( "DISICommunicationManager::HandleMessageMulticast multicasting 0x%x" ), &aMessageToMulticast ) );
        iISIIndicationHandler->Multicast( aMessageToMulticast );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        C_TRACE( ( _T( "DISICommunicationManager::HandleMessageMulticast ILLEGAL Resource from APE in message 0x%x" ), &aMessageToMulticast ) );
        }
    C_TRACE( ( _T( "DISICommunicationManager::HandleMessageMulticast 0x%x<" ), &aMessageToMulticast ) );
    }

void DISICommunicationManager::SendApeCommgrSubscribeResp(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendApeCommgrSubscribeResp 0x%x>" ), &aMessage ) );
    TUint16 msgLength( ISI_HEADER_SIZE + SIZE_APE_COMMGR_SUBSCRIBE_RESP );
    TDes8& pnsSubscribeResp = MemApi::AllocBlock( msgLength );
    pnsSubscribeResp.SetLength( msgLength );
    TUint8* msgPtr = const_cast<TUint8*>( aMessage.Ptr() );
    ASSERT_RESET_ALWAYS( msgPtr, ( EISICommunicationManagerNullPointer4 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aMessage.Length() > ( ISI_HEADER_OFFSET_TRANSID ) ),
                         ( EISICommunicationInvalidLength2 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    TUint8* pnsSubscribeRespPtr = const_cast<TUint8*>( pnsSubscribeResp.Ptr() );
    ASSERT_RESET_ALWAYS( pnsSubscribeRespPtr, ( EISICommunicationManagerNullPointer5 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( pnsSubscribeResp.Length() > ( ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_RESULT ) ),
                         ( EISICommunicationInvalidLength3 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    pnsSubscribeRespPtr[ ISI_HEADER_OFFSET_MEDIA ] = msgPtr[ ISI_HEADER_OFFSET_MEDIA ];
    SET_RECEIVER_DEV( pnsSubscribeRespPtr, GET_SENDER_DEV( aMessage ) );
    SET_SENDER_DEV( pnsSubscribeRespPtr, PN_DEV_OWN );
    pnsSubscribeRespPtr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_APE_COMMGR;
    SET_LENGTH( pnsSubscribeRespPtr, ( msgLength - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( pnsSubscribeRespPtr, GET_SENDER_OBJ( aMessage ) );
    SET_SENDER_OBJ( pnsSubscribeRespPtr, iObjectIdentifier );
    pnsSubscribeRespPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_TRANSID ] = msgPtr[ ISI_HEADER_OFFSET_TRANSID ];
    pnsSubscribeRespPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_MESSAGEID ] = APE_COMMGR_SUBSCRIBE_RESP;
    pnsSubscribeRespPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_RESULT ] = msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN ? APE_COMMGR_OK : APE_COMMGR_NOT_ALLOWED;
    TRACE_ASSERT( APE_COMMGR_OK == pnsSubscribeRespPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_RESP_OFFSET_RESULT ] );
    TInt sendResult = iRouter->Send( pnsSubscribeResp, iObjectIdentifier );
    ASSERT_RESET_ALWAYS( ( sendResult == KErrNone ), ( EISICommunicationManagerSendFailed2 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );    
    C_TRACE( ( _T( "DISICommunicationManager::SendApeCommgrSubscribeResp 0x%x<" ), &aMessage ) );
    }

void DISICommunicationManager::SendCommServiceNotIdentifiedResp(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendCommServiceNotIdentifiedResp 0x%x>" ), &aMessage ) );
    //  check if possible to go in wild loop if incoming message is not_reachable_resp 
    TInt commonServiceNotIdentifiedLength( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP );       
    TDes8& commonServiceNotIdentifiedResp = MemApi::AllocBlock( commonServiceNotIdentifiedLength );
    commonServiceNotIdentifiedResp.SetLength( commonServiceNotIdentifiedLength );
    TUint8* commonServiceNotIdentifiedRespPtr( const_cast<TUint8*>( commonServiceNotIdentifiedResp.Ptr() ) );
    ASSERT_RESET_ALWAYS( commonServiceNotIdentifiedRespPtr, ( EISICommunicationManagerNullPointer6 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( commonServiceNotIdentifiedResp.Length() > ( ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE3 ) ),
                         ( EISICommunicationInvalidLength4 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    const TUint8* receivedMsgPtr( aMessage.Ptr() );
    ASSERT_RESET_ALWAYS( receivedMsgPtr, ( EISICommunicationManagerNullPointer11 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aMessage.Length() > ( ISI_HEADER_OFFSET_SUBMESSAGEID ) ),
                         ( EISICommunicationInvalidLength10 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    SetIsiHeaderForCommonMessage( commonServiceNotIdentifiedResp, aMessage );    
    commonServiceNotIdentifiedRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_SUBMESSAGEID ] = COMM_SERVICE_NOT_IDENTIFIED_RESP;
    commonServiceNotIdentifiedRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDMESSAGEID ] = receivedMsgPtr[ ISI_HEADER_OFFSET_MESSAGEID ];
    commonServiceNotIdentifiedRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE1 ] = KFiller;
    commonServiceNotIdentifiedRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDSUBMESSAGEID ] = receivedMsgPtr[ ISI_HEADER_OFFSET_SUBMESSAGEID ];
    commonServiceNotIdentifiedRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE2 ] = KFiller;
    commonServiceNotIdentifiedRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE3 ] = KFiller;
    TInt sendResult = iRouter->Send( commonServiceNotIdentifiedResp, iObjectIdentifier );
    ASSERT_RESET_ALWAYS( ( sendResult == KErrNone ), ( EISICommunicationManagerSendFailed | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISICommunicationManager::SendCommServiceNotIdentifiedResp 0x%x<" ), &aMessage ) );
    }

void DISICommunicationManager::SendCommIsiVersionGetResp(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SendCommIsiVersionGetResp 0x%x>" ), &aMessage ) );
    TInt commonIsiVersionGetRespLength( ISI_HEADER_SIZE + SIZE_COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP );
    TDes8& commonIsiVersionGetResp = MemApi::AllocBlock( commonIsiVersionGetRespLength );
    commonIsiVersionGetResp.SetLength( commonIsiVersionGetRespLength );
    TUint8* commonIsiVersionGetRespPtr( const_cast<TUint8*>( commonIsiVersionGetResp.Ptr() ) );
    ASSERT_RESET_ALWAYS( commonIsiVersionGetRespPtr, ( EISICommunicationManagerNullPointer8 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( commonIsiVersionGetResp.Length() > ( ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE3 ) ),
                         ( EISICommunicationInvalidLength6 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    const TUint8* receivedMsgPtr( aMessage.Ptr() );
    ASSERT_RESET_ALWAYS( receivedMsgPtr, ( EISICommunicationManagerNullPointer12 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    SetIsiHeaderForCommonMessage( commonIsiVersionGetResp, aMessage );    
    commonIsiVersionGetRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_SUBMESSAGEID ] = COMM_ISI_VERSION_GET_RESP;
    commonIsiVersionGetRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONZZZ ] = APE_COMMUNICATION_MANAGER_SERVER_ISI_VERSION_Z;
    commonIsiVersionGetRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONYYY ] = APE_COMMUNICATION_MANAGER_SERVER_ISI_VERSION_Y;
    commonIsiVersionGetRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE1 ] = KFiller;
    commonIsiVersionGetRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE2 ] = KFiller;
    commonIsiVersionGetRespPtr[ ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE3 ] = KFiller;
    TInt sendResult = iRouter->Send( commonIsiVersionGetResp, iObjectIdentifier );
    ASSERT_RESET_ALWAYS( ( sendResult == KErrNone ), ( EISICommunicationManagerSendFailed1 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISICommunicationManager::SendCommIsiVersionGetResp 0x%x<" ), &aMessage ) );
    }

void DISICommunicationManager::SetIsiHeaderForCommonMessage(
        TDes8& aCommonResponse,
        const TDesC8& aCommonRequest
        )
    {
    C_TRACE( ( _T( "DISICommunicationManager::SetIsiHeaderForCommonMessage 0x%x 0x%x>" ), &aCommonResponse, &aCommonRequest ) );
    const TUint8* commonRequestPtr = aCommonRequest.Ptr();
    TUint8* commonResponsePtr = const_cast<TUint8*>( aCommonResponse.Ptr() );
    ASSERT_RESET_ALWAYS( commonRequestPtr, ( EISICommunicationManagerNullPointer13 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( commonResponsePtr, ( EISICommunicationManagerNullPointer14 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );     
    ASSERT_RESET_ALWAYS( ( aCommonRequest.Length() > ISI_HEADER_OFFSET_TRANSID ),
                         ( EISICommunicationInvalidLength11 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( ( aCommonResponse.Length() > ISI_HEADER_OFFSET_MESSAGEID ),
                         ( EISICommunicationInvalidLength12 | EDISICommunicationManagerTraceId << KClassIdentifierShift ) );
    commonResponsePtr[ ISI_HEADER_OFFSET_MEDIA ] = commonRequestPtr[ ISI_HEADER_OFFSET_MEDIA ];
    SET_RECEIVER_DEV( commonResponsePtr, GET_SENDER_DEV( commonRequestPtr ) );
    SET_SENDER_DEV( commonResponsePtr, PN_DEV_OWN );
    commonResponsePtr[ ISI_HEADER_OFFSET_RESOURCEID ] = commonRequestPtr[ ISI_HEADER_OFFSET_RESOURCEID ];
    SET_LENGTH( commonResponsePtr, ( aCommonResponse.Length() - PN_HEADER_SIZE ) );
    SET_RECEIVER_OBJ( commonResponsePtr, GET_SENDER_OBJ( commonRequestPtr ) );
    SET_SENDER_OBJ( commonResponsePtr, iObjectIdentifier );
    commonResponsePtr[ ISI_HEADER_OFFSET_TRANSID ] = commonRequestPtr[ ISI_HEADER_OFFSET_TRANSID ];
    commonResponsePtr[ ISI_HEADER_OFFSET_MESSAGEID ] = COMMON_MESSAGE;
    C_TRACE( ( _T( "DISICommunicationManager::SetIsiHeaderForCommonMessage 0x%x 0x%x<" ), &aCommonResponse, &aCommonRequest ) );
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


