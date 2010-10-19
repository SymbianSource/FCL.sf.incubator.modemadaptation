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


#include <pn_eventmodem_extisi.h>         // PN_COMMG, PNS_SUBSCRIBED_RESOURCES_...
#include <phonetisi.h>                    // For ISI_HEADER_SIZE
#include <pn_const.h>                     // For PN_HEADER_SIZE
#include <commisi.h>                      // For common ISI messages

#include "isiindicationhandler.h"         // For DISIIndicationHandler
#include "isicommunicationmanagertrace.h" // For C_TRACE..
#include "misiobjectrouterif.h"           // For DISICommunicationManager
#include "memapi.h"                       // For MemApi
#include "isihelpers.h"                  // For SET_RECEIVER_OBJ...

// HOX! nsisi.h is also exported by ??? 
#include "nsisi.h"                        // For PN_MEDIA_ROUTING_REQ

#include "ape_commgrisi.h"                // For APE_COMMGR..
#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
#include "misicommunicationmanagerif.h"   // For MISICommunicationManagerIf
#define ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED // May be used without wider name service too
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE

#ifdef ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
const TUint8 KEightBitShift( 8 );
const TUint8 KSixteenBitShift( 16 );
#endif // ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED

const TUint8 KMaxAmountOfResources( 255 );
const TUint8 KPnsSubscribedResourcesExtendLength( 12 );
const TUint8 KResourceArraySize( 4 );

// Faults
enum TISIIndincationHandlerFaults
    {
    EISIIndicationHandlerWrongParameter = 0x01,
    EISIIndicationHandlerMemAllocFailure,
    EISIIndicationHandlerMemAllocFailure1,
    EISIIndicationHandlerMemAllocFailure2,
    EISIIndicationHandlerMemAllocFailure3,
    EISIIndicationHandlerMemAllocFailure4,
    EISIIndicationHandlerMemAllocFailure5,
    EISIIndicationHandlerMemAllocFailure6,
    EISIIndicationHandlerQueNULLPointer,
    EISIIndicationHandlerQueNULLPointer1,
    EISIIndicationHandlerQueNULLPointer2,
    EISIIndicationHandlerQueNULLPointer3,
    EISIIndicationHandlerQueNULLPointer4,
    EISIIndicationHandlerQueNULLPointer5,
    EISIIndicationHandlerQueNULLPointer6,
    EISIIndicationHandlerQueNULLPointer7,
    EISIIndicationHandlerQueNULLPointer8,
    EISIIndicationHandlerQueNULLPointer9,
    EISIIndicationHandlerQueNULLPointer10,
    EISIIndicationHandlerQueNULLPointer11,
    EISIIndicationHandlerQueNULLPointer12,
    EISIIndicationHandlerQueNULLPointer13,
    EISIIndicationHandlerQueNULLPointer14,
    EISIIndicationHandlerQueNULLPointer15,
    EISIIndicationHandlerQueNULLPointer16,
    EISIIndicationHandlerQueNULLPointer17,
    EISIIndicationHandlerQueNULLPointer18,
    EISIIndicationHandlerQueNULLPointer19,
    EISIIndicationHandlerQueNULLPointer20,
    EISIIndicationHandlerQueNULLPointer21,
    EISIIndicationHandlerQueNULLPointer22,
    EISIIndicationHandlerQueNULLPointer23
    };


// -----------------------------------------------------------------------------
// DISIIndicationHandler::
//
// -----------------------------------------------------------------------------
//
DISIIndicationHandler::DISIIndicationHandler(
        MISIObjectRouterIf* aRouter
        ) : iRouter( aRouter )
          , iSubscriberIsAnExternalDevice( EFalse )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::DISIIndicationHandler 0x%x>" ), &iRouter ) );
    C_TRACE( ( _T( "DISIIndicationHandler::DISIIndicationHandler 0x%x<" ), &iRouter ) );
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::~DISIIndicationHandler
//
// -----------------------------------------------------------------------------
//
DISIIndicationHandler::~DISIIndicationHandler(
        // None
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::~DISIIndicationHandler>" ) ) );
    
    SDblQueLink*    deviceAnchor = &iDeviceQ.iA;
    SDblQueLink*    next = deviceAnchor->iNext;
    TDeviceIdLink*  deviceLink = NULL;
    
    SDblQueLink*        serverAnchor = NULL;
    SDblQueLink*        serverNext = NULL;
    TServerIdLink*      serverLink = NULL;

    SDblQueLink*        indicationAnchor = NULL;
    SDblQueLink*        indicationNext = NULL;
    TIndicationLink*    indicationLink = NULL;

    SDblQueLink*        subscriberAnchor = NULL;
    SDblQueLink*        subscriberNext = NULL;
    TSubscriberLink*    subscriberLink = NULL;
    
    while( deviceAnchor != next )
        {
        deviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
        ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerQueNULLPointer1 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        C_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler device: 0x%x"), deviceLink->iDeviceId ) );
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        while( serverAnchor != serverNext )
            {
            serverLink = _LOFF( serverNext, TServerIdLink, iServerLink );
            ASSERT_RESET_ALWAYS( serverLink, ( EISIIndicationHandlerQueNULLPointer2 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            while( indicationAnchor != indicationNext )
                {
                indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
                ASSERT_RESET_ALWAYS( indicationLink, ( EISIIndicationHandlerQueNULLPointer3 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                C_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler indication: 0x%x"), indicationLink->iIndication ));
                // get next link already in case if particular link is to be deleted
                indicationNext = indicationNext->iNext;
                subscriberAnchor = &indicationLink->iSubscriberQ.iA;
                subscriberNext = subscriberAnchor->iNext;
                while( subscriberAnchor != subscriberNext )
                    {
                    subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                    ASSERT_RESET_ALWAYS( subscriberLink, ( EISIIndicationHandlerQueNULLPointer4 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                    C_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler subscriber: 0x%x"), subscriberLink->iObjId ));
                    TBool onlySubscriber( subscriberLink->iSubscriberLink.Alone() );
                    subscriberLink->iSubscriberLink.Deque();
                    delete subscriberLink;
                    subscriberLink = NULL;
                    if( onlySubscriber )
                        {
                        C_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler remove indication: 0x%x"), indicationLink->iIndication ));
                        indicationLink->iIndicationLink.Deque();
                        delete indicationLink;
                        indicationLink = NULL;
                        }
                    // No need to go through rest of subscriberlinks since this one already found and deleted
                    subscriberNext = subscriberAnchor;      
                    }
                }
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            serverNext = serverNext->iNext;
            if( indicationAnchor == indicationNext )
                {
                // No more indications on the server -> server link can be deleted
                C_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler remove server: 0x%x"), serverLink->iResourceId ));
                serverLink->iServerLink.Deque();
                delete serverLink;
                serverLink = NULL;
                }
            }
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        next = next->iNext;
        if( serverAnchor == serverNext )
            {
            // No more servers on the device -> device link can be deleted
            C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove device: 0x%x"), deviceLink->iDeviceId ));
            deviceLink->iDeviceLink.Deque();
            delete deviceLink;
            deviceLink = NULL;
            }
        }
    C_TRACE( ( _T( "DISIIndicationHandler::~DISIIndicationHandler<" ) ) );
    }


// -----------------------------------------------------------------------------
// DISIIndicationHandler::Subscribe
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::Subscribe(
        const TDesC8& aSubscriptionMessage
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::Subscribe 0x%x>" ), &aSubscriptionMessage ) );
    TUint8* msgPtr( const_cast<TUint8*>( aSubscriptionMessage.Ptr() ) );
    TInt msgLength( aSubscriptionMessage.Length() );
    TUint32 resourceId( 0 );
    TUint8 deviceId( 0 );
    TUint16 objId( 0 );
    objId = GET_SENDER_OBJ( aSubscriptionMessage );
    if( msgPtr[ ISI_HEADER_OFFSET_SENDERDEVICE ] == PN_DEV_OWN )
        {
        iSubscriberIsAnExternalDevice = EFalse;
        RemoveSubscription( objId );
        //If any subblocks
        if( msgPtr[ ISI_HEADER_SIZE + APE_COMMGR_SUBSCRIBE_REQ_OFFSET_FILLERBYTE1 ] != 0 )
            {
            TUint8 indication( 0 );
            for( TInt subBlockOffset( ISI_HEADER_SIZE + SIZE_APE_COMMGR_SUBSCRIBE_REQ ); subBlockOffset < msgLength; subBlockOffset += ( SIZE_APE_COMMGR_SUBSCRIBE_SB ) )
                {
                deviceId = msgPtr[ subBlockOffset + APE_COMMGR_SUBSCRIBE_SB_OFFSET_DEVICEID ];
                resourceId = GETB32( &msgPtr[ subBlockOffset + APE_COMMGR_SUBSCRIBE_SB_OFFSET_FILLERBYTE1 ] ); //resource offset
#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
                C_TRACE( ( _T( "DISIIndicationHandler::Subscribe 0x%x ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE device id to be overwritten 0x%x resource 0x%x" ), &aSubscriptionMessage, deviceId, resourceId ) );
                deviceId = MISICommunicationManagerIf::GetDeviceIdentifierByResource( resourceId );
                C_TRACE( ( _T( "DISIIndicationHandler::Subscribe 0x%x ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE new device id 0x%x resource 0x%x" ), &aSubscriptionMessage, deviceId, resourceId ) );
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
                indication = msgPtr[ subBlockOffset + APE_COMMGR_SUBSCRIBE_SB_OFFSET_RESOURCEID ];
                E_TRACE( ( _T("DISIIndicationHandler indication 0x%x objId 0x%x resource 0x%x device 0x%x"), indication, objId, resourceId, deviceId ) );
                TDeviceIdLink* deviceLink = GetDeviceLink( deviceId );
                if( !deviceLink )
                    {
                    E_TRACE( ( _T("DISIIndicationHandler::Subscribe device not found yet -> create: 0x%x"), deviceId ) );
                    deviceLink = new TDeviceIdLink();
                    ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerMemAllocFailure | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                    deviceLink->iDeviceId = deviceId;
                    iDeviceQ.Add( &( deviceLink->iDeviceLink ) );
                    }
                AddServer( *deviceLink, resourceId, indication, objId );
                }
            }
#ifdef _DEBUG
#ifdef COMPONENT_TRACE_FLAG
        PrintSubscriptions();
#endif // COMPONENT_TRACE_FLAG
#endif // _DEBUG
    
        TDeviceIdLink* deviceLink = NULL;
        SDblQueLink* anchor = &iDeviceQ.iA;
        SDblQueLink* next = anchor->iNext;
        while( anchor != next )
            {
            deviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
            ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerQueNULLPointer5 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
            E_TRACE( ( _T("DISIIndicationHandler::Subscribe device: 0x%x"), deviceLink->iDeviceId ) );
            if( deviceLink->iDeviceId != PN_DEV_OWN )
                {
                E_TRACE( ( _T("DISIIndicationHandler::Subscribe send subscription to device: 0x%x"), deviceLink->iDeviceId ) );
                SendSubscription( *deviceLink );
                }
            next = next->iNext;
            }
        }
    else
        {
        deviceId = GET_SENDER_DEV( aSubscriptionMessage );
        iSubscriberIsAnExternalDevice = ETrue;
        RemoveSubscription( objId );
        TDeviceIdLink* deviceLink = GetExternalDeviceLink( deviceId );
        if( !deviceLink )
            {
            E_TRACE( ( _T("DISIIndicationHandler::Subscribe external device not found yet -> create: 0x%x"), deviceId ) );
            deviceLink = new TDeviceIdLink();
            ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerMemAllocFailure4 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
            deviceLink->iDeviceId = deviceId;
            iExternalDeviceQ.Add( &( deviceLink->iDeviceLink ) );
            }
        else
            {
            SDblQueLink* serverAnchor = NULL;
            SDblQueLink* serverNext = NULL;
            TServerIdLink* serverLink = NULL;
            serverAnchor = &deviceLink->iServerQ.iA;
            serverNext = serverAnchor->iNext;
            while( serverAnchor != serverNext )
                {
                serverLink = _LOFF( serverNext, TServerIdLink, iServerLink );
                ASSERT_RESET_ALWAYS( serverLink, ( EISIIndicationHandlerQueNULLPointer6 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                E_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler remove server: 0x%x"), serverLink->iResourceId ));
                serverLink->iServerLink.Deque();
                delete serverLink;
                serverLink = NULL;
                serverNext = serverNext->iNext;           
                }
            }
        for( TInt subBlockOffset( ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCELIST );
            subBlockOffset < msgLength; subBlockOffset += KResourceArraySize )
            {
            resourceId = GETB32( &msgPtr[ subBlockOffset ] );
            E_TRACE( ( _T("DISIIndicationHandler resource 0x%x device 0x%x"), resourceId, deviceId ) );
            AddServer( *deviceLink, resourceId );
            }
        }
    C_TRACE( ( _T( "DISIIndicationHandler::Subscribe<" ) ) );
    }

// -----------------------------------------------------------------------------
// DIndicationHandler::
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::Multicast(
        const TDesC8& aIndication
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::Multicast 0x%x>" ), &aIndication ) );
    TUint8* ptr = const_cast<TUint8*>( aIndication.Ptr() );
    TDeviceIdLink* deviceLink = NULL;
    SDblQueLink* anchor = &iDeviceQ.iA;
    SDblQueLink* next = anchor->iNext;
    while( anchor != next )
        {
        deviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
        ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerQueNULLPointer20 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        C_TRACE( ( _T("DISIIndicationHandler::Multicast device: 0x%x"), deviceLink->iDeviceId ) );
        TUint32 resource = ptr[ ISI_HEADER_OFFSET_RESOURCEID ];
#ifdef ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
        // Extended Resource Routing
        if( PN_PREFIX == ptr[ ISI_HEADER_OFFSET_RESOURCEID ] )
            {
            resource = ( resource | (TUint32)( ptr[ ISI_HEADER_OFFSET_TYPE ] << KEightBitShift ) );
            resource = ( resource | (TUint32)( ptr[ ISI_HEADER_OFFSET_SUBTYPE ] << KSixteenBitShift ) );
            C_TRACE( ( _T( "DISIIndicationHandler::Multicast 0x%x extended resource 0x%x" ), &aIndication, resource ) );
            }
#endif // ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
        TServerIdLink* serverLink = GetServerLink( *deviceLink, resource );
        //TServerIdLink* serverLink = GetServerLink( *deviceLink, ptr[ ISI_HEADER_OFFSET_RESOURCEID ] );
        TIndicationLink* indicationLink = NULL;
        TSubscriberLink* subscriberLink = NULL;
        if( serverLink )
            {
            indicationLink = GetIndicationLink( *serverLink, ptr[ ISI_HEADER_OFFSET_MESSAGEID ] );
            }
        if( indicationLink )
            {
            SDblQueLink* subscriberAnchor = NULL;
            SDblQueLink* subscriberNext = NULL;
            subscriberAnchor = &indicationLink->iSubscriberQ.iA;
            subscriberNext = subscriberAnchor->iNext;
            while( subscriberAnchor != subscriberNext )
                {
                subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                ASSERT_RESET_ALWAYS( subscriberLink, ( EISIIndicationHandlerQueNULLPointer21 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                C_TRACE( ( _T("DISIIndicationHandler::Multicast Subscriber objid: 0x%x"), subscriberLink->iObjId ));
                // Make a copy to every subscriber
                TDes8& tempPtr = MemApi::AllocBlock( aIndication.Length() );
                tempPtr.Copy( ptr, aIndication.Length() );
                TUint8* copyPtr = const_cast<TUint8*>( tempPtr.Ptr() );
                SET_RECEIVER_OBJ( copyPtr, subscriberLink->iObjId );
                iRouter->Send( tempPtr, PN_OBJ_EVENT_MULTICAST );
                subscriberNext = subscriberNext->iNext;
                }
            }
        next = next->iNext;
        }
    deviceLink = NULL;
    anchor = &iExternalDeviceQ.iA;
    next = anchor->iNext;
    while( anchor != next )
        {
        deviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
        ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerQueNULLPointer22 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        C_TRACE( ( _T("DISIIndicationHandler::Multicast external device: 0x%x"), deviceLink->iDeviceId ) );
        TUint32 resource = ptr[ ISI_HEADER_OFFSET_RESOURCEID ];
#ifdef ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
        // Extended Resource Routing
        if( PN_PREFIX == ptr[ ISI_HEADER_OFFSET_RESOURCEID ] )
            {
            resource = ( resource | (TUint32)( ptr[ ISI_HEADER_OFFSET_TYPE ] << KEightBitShift ) );
            resource = ( resource | (TUint32)( ptr[ ISI_HEADER_OFFSET_SUBTYPE ] << KSixteenBitShift ) );
            C_TRACE( ( _T( "DISIIndicationHandler::Multicast 0x%x extended resource 0x%x, external device 0x%x" ), &aIndication, resource, deviceLink->iDeviceId ) );
            }
#endif // ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED        
        TServerIdLink* serverLink = GetServerLink( *deviceLink, resource );
        if( serverLink )
            {
            C_TRACE( ( _T("DISIIndicationHandler::Multicast resource: 0x%x"), serverLink->iResourceId ));
            // Make a copy to every external subscriber
            TDes8& tempPtr2 = MemApi::AllocBlock( aIndication.Length() );
            tempPtr2.Copy( ptr, aIndication.Length() );
            TUint8* copyPtr2 = const_cast<TUint8*>( tempPtr2.Ptr() );
            SET_RECEIVER_DEV( tempPtr2, deviceLink->iDeviceId );
            tempPtr2[ ISI_HEADER_OFFSET_RESOURCEID ] = ptr[ ISI_HEADER_OFFSET_RESOURCEID ];
            SET_RECEIVER_OBJ( tempPtr2, PN_OBJ_EVENT_MULTICAST );
            SET_SENDER_OBJ( tempPtr2, ptr[ ISI_HEADER_OFFSET_SENDEROBJECT ] );
            iRouter->Send( tempPtr2, PN_OBJ_EVENT_MULTICAST );
            }
        next = next->iNext;
        }    
    C_TRACE( ( _T( "DISIIndicationHandler::Multicast 0x%x<" ), &aIndication ) );
    }


// -----------------------------------------------------------------------------
// DISIIndicationHandler::AddServer
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::AddServer(
        TDeviceIdLink& aDeviceLink,
        const TUint32 aResourceId,
        const TUint8 aIndication,
        const TUint16 aObjId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::AddServer 0x%x 0x%x 0x%x 0x%x>" ), &aDeviceLink, aResourceId, aIndication, aObjId ) );
    
    TServerIdLink* serverLink = GetServerLink( aDeviceLink, aResourceId );
    if( !serverLink )
        {
        C_TRACE( ( _T("DISIIndicationHandler::AddServer resource: 0x%x not found, create"), aResourceId ));
        serverLink = new TServerIdLink();
        ASSERT_RESET_ALWAYS( serverLink, ( EISIIndicationHandlerMemAllocFailure1 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        serverLink->iResourceId = aResourceId;
        aDeviceLink.iServerQ.Add( &( serverLink->iServerLink ) );
        }
    AddIndication( *serverLink, aIndication, aObjId );
    C_TRACE( ( _T( "DISIIndicationHandler::AddServer 0x%x 0x%x 0x%x 0x%x<" ), &aDeviceLink, aResourceId, aIndication, aObjId ) );
    }
    

// -----------------------------------------------------------------------------
// DISIIndicationHandler::AddServer
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::AddServer(
        TDeviceIdLink& aDeviceLink,
        const TUint32 aResourceId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::AddServer external 0x%x 0x%x>" ), &aDeviceLink, aResourceId ) );
    TServerIdLink* serverLink = GetServerLink( aDeviceLink, aResourceId );
    if( !serverLink )
        {
        C_TRACE( ( _T("DISIIndicationHandler::AddServer external resource: 0x%x not found, create"), aResourceId ));
        serverLink = new TServerIdLink();
        ASSERT_RESET_ALWAYS( serverLink, ( EISIIndicationHandlerMemAllocFailure5 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        serverLink->iResourceId = aResourceId;
        aDeviceLink.iServerQ.Add( &( serverLink->iServerLink ) );
        }
    C_TRACE( ( _T( "DISIIndicationHandler::AddServer 0x%x 0x%x<" ), &aDeviceLink, aResourceId ) );
    }


// -----------------------------------------------------------------------------
// DISIIndicationHandler::AddIndication
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::AddIndication(
        TServerIdLink& aServerLink,
        const TUint8 aIndication,
        const TUint16 aObjId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::AddIndication 0x%x 0x%x 0x%x>" ), &aServerLink, aIndication, aObjId ) );
    TIndicationLink* indicationLink = GetIndicationLink( aServerLink, aIndication );
    if( !indicationLink )
        {
        C_TRACE( ( _T("DISIIndicationHandler::AddIndication Indication: 0x%x not found, create"), aIndication ));
        indicationLink = new TIndicationLink();
        ASSERT_RESET_ALWAYS( indicationLink, ( EISIIndicationHandlerMemAllocFailure3 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        indicationLink->iIndication = aIndication;
        aServerLink.iIndicationQ.Add( &( indicationLink->iIndicationLink ) );
        }
    AddSubscriber( *indicationLink, aObjId );
    C_TRACE( ( _T( "DISIIndicationHandler::AddIndication 0x%x 0x%x 0x%x<" ), &aServerLink, aIndication, aObjId ) );
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::AddSubscriber
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::AddSubscriber(
        TIndicationLink& aIndicationLink,
        const TUint16 aObjId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::AddSubscriber 0x%x, 0x%x>" ), &aIndicationLink, aObjId ) );
    TSubscriberLink* subscriberLink = GetSubscriberLink( aIndicationLink, aObjId );
    if( !subscriberLink )
        {
        C_TRACE( ( _T("DISIIndicationHandler::AddSubscriber Subscriber not found: 0x%x 0x%x"), aIndicationLink.iIndication, aObjId ) );
        subscriberLink = new TSubscriberLink();
        ASSERT_RESET_ALWAYS( subscriberLink, ( EISIIndicationHandlerMemAllocFailure2 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        subscriberLink->iObjId = aObjId;
        aIndicationLink.iSubscriberQ.Add( &( subscriberLink->iSubscriberLink ) );
        }
    else
        {
        TRACE_ASSERT_INFO(0, (TUint8)aObjId<<KObjIdShift);
        C_TRACE( ( _T( "DISIIndicationHandler::AddSubscriber double subscription 0x%x" ), aObjId ) );
        }
    C_TRACE( ( _T( "DISIIndicationHandler::AddSubscriber 0x%x 0x%x<" ), &aIndicationLink, aObjId ) );
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::GetDeviceLink
//
// -----------------------------------------------------------------------------
//
DISIIndicationHandler::TDeviceIdLink* DISIIndicationHandler::GetDeviceLink(
        const TUint8 aDeviceId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::GetDeviceLink 0x%x ->" ), aDeviceId ) );
    TDeviceIdLink* deviceLink = NULL;
    TDeviceIdLink* tmpDeviceLink = NULL;
    SDblQueLink* anchor = &iDeviceQ.iA;
    SDblQueLink* next = anchor->iNext;
    while( anchor != next )
        {
        tmpDeviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
        ASSERT_RESET_ALWAYS( tmpDeviceLink, ( EISIIndicationHandlerQueNULLPointer7 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        E_TRACE( ( _T( "DISIIndicationHandler::GetDeviceLink device: 0x%x"), tmpDeviceLink->iDeviceId ) );
        if( tmpDeviceLink->iDeviceId == aDeviceId )
            {
            deviceLink = tmpDeviceLink;
            break;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DISIIndicationHandler::GetDeviceLink 0x%x <-" ), deviceLink ) );
    return deviceLink;
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::GetExternalDeviceLink
//
// -----------------------------------------------------------------------------
//
DISIIndicationHandler::TDeviceIdLink* DISIIndicationHandler::GetExternalDeviceLink(
        const TUint8 aDeviceId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::GetExternalDeviceLink 0x%x ->" ), aDeviceId ) );
    TDeviceIdLink* deviceLink = NULL;
    TDeviceIdLink* tmpDeviceLink = NULL;
    SDblQueLink* anchor = &iExternalDeviceQ.iA;
    SDblQueLink* next = anchor->iNext;
    while( anchor != next )
        {
        tmpDeviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
        ASSERT_RESET_ALWAYS( tmpDeviceLink, ( EISIIndicationHandlerQueNULLPointer8 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        E_TRACE( ( _T( "DISIIndicationHandler::GetExternalDeviceLink device: 0x%x"), tmpDeviceLink->iDeviceId ) );
        if( tmpDeviceLink->iDeviceId == aDeviceId )
            {
            deviceLink = tmpDeviceLink;
            break;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DISIIndicationHandler::GetExternalDeviceLink 0x%x <-" ), deviceLink ) );
    return deviceLink;
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::GetServerLink
//
// -----------------------------------------------------------------------------
//

DISIIndicationHandler::TServerIdLink* DISIIndicationHandler::GetServerLink(
        TDeviceIdLink& aDeviceLink,
        const TUint32 aResourceId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::GetServerLink 0x%x 0x%x ->" ), &aDeviceLink, aResourceId ) );
    TServerIdLink* serverLink = NULL;
    TServerIdLink* tmpServerLink = NULL;
    SDblQueLink* anchor = &aDeviceLink.iServerQ.iA;
    SDblQueLink* next = anchor->iNext;
    while( anchor != next ) // servers
        {
        tmpServerLink = _LOFF( next, TServerIdLink, iServerLink );
        ASSERT_RESET_ALWAYS( tmpServerLink, ( EISIIndicationHandlerQueNULLPointer9 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        E_TRACE( ( _T( "DISIIndicationHandler::GetServerLink server: 0x%x"), tmpServerLink->iResourceId ) );
        if( tmpServerLink->iResourceId == aResourceId )
            {
            serverLink = tmpServerLink;
            break;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DISIIndicationHandler::GetServerLink 0x%x <-" ), serverLink ) );
    return serverLink;
    }
    

// -----------------------------------------------------------------------------
// DISIIndicationHandler::GetIndicationLink
//
// -----------------------------------------------------------------------------
//
DISIIndicationHandler::TIndicationLink* DISIIndicationHandler::GetIndicationLink(
        TServerIdLink& aServerLink,
        const TUint8 aIndication
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::GetIndicationLink 0x%x 0x%x ->" ), &aServerLink, aIndication ) );
    TIndicationLink* indicationLink = NULL;
    TIndicationLink* tmpIndicationLink = NULL;
    SDblQueLink* anchor = &aServerLink.iIndicationQ.iA;
    SDblQueLink* next = anchor->iNext;
    while( anchor != next )
        {
        tmpIndicationLink = _LOFF( next, TIndicationLink, iIndicationLink );
        ASSERT_RESET_ALWAYS( tmpIndicationLink, ( EISIIndicationHandlerQueNULLPointer10 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        if( tmpIndicationLink->iIndication == aIndication )
            {
            indicationLink = tmpIndicationLink;
            break;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DISIIndicationHandler::GetIndicationLink 0x%x <-" ), indicationLink ) );
    return indicationLink;
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::GetSubscriberLink
//
// -----------------------------------------------------------------------------
//
DISIIndicationHandler::TSubscriberLink* DISIIndicationHandler::GetSubscriberLink(
        TIndicationLink& aIndicationLink,
        const TUint16 aObjId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::GetSubscriberLink 0x%x, 0x%x ->" ), &aIndicationLink, aObjId ) );
    TSubscriberLink* tmpSubscriberLink = NULL;
    TSubscriberLink* subscriberLink = NULL;
    SDblQueLink* anchor = &aIndicationLink.iSubscriberQ.iA;
    SDblQueLink* next = anchor->iNext;
    while( anchor != next )
        {
        tmpSubscriberLink = _LOFF( next, TSubscriberLink, iSubscriberLink );
        ASSERT_RESET_ALWAYS( tmpSubscriberLink, ( EISIIndicationHandlerQueNULLPointer11 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        if ( tmpSubscriberLink->iObjId == aObjId )
            {
            subscriberLink = tmpSubscriberLink;
            break;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DISIIndicationHandler::GetSubscriberLink 0x%x <-" ), subscriberLink ) );
    return subscriberLink;
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::PrintSubscriptions
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::PrintSubscriptions(
        // None
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::PrintSubscriptions>" ) ) );
    TDeviceIdLink* deviceLink = NULL;
    SDblQueLink* anchor = &iDeviceQ.iA;
    SDblQueLink* next = anchor->iNext;
    SDblQueLink* serverAnchor = NULL;
    SDblQueLink* serverNext = NULL;
    TServerIdLink* serverLink = NULL;
    SDblQueLink* indicationAnchor = NULL;
    SDblQueLink* indicationNext = NULL;
    TIndicationLink* indicationLink = NULL;
    SDblQueLink* subscriberAnchor = NULL;
    SDblQueLink* subscriberNext = NULL;
    TSubscriberLink* subscriberLink = NULL;
    while( anchor != next )
        {
        deviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
        ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerQueNULLPointer12 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        C_TRACE( ( _T("DISIIndicationHandler::PrintSubscriptions device: 0x%x"), deviceLink->iDeviceId ) );
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        while( serverAnchor != serverNext )
            {
            serverLink = _LOFF( serverNext, TServerIdLink, iServerLink );
            ASSERT_RESET_ALWAYS( serverLink, ( EISIIndicationHandlerQueNULLPointer13 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
            C_TRACE( ( _T("DISIIndicationHandler::PrintSubscriptions server: 0x%x"), serverLink->iResourceId ) );
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            while( indicationAnchor != indicationNext )
                {
                indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
                ASSERT_RESET_ALWAYS( indicationLink, ( EISIIndicationHandlerQueNULLPointer14 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                C_TRACE( ( _T("DISIIndicationHandler::PrintSubscriptions indication: 0x%x"), indicationLink->iIndication ) );
                subscriberAnchor = &indicationLink->iSubscriberQ.iA;
                subscriberNext = subscriberAnchor->iNext;
                while( subscriberAnchor != subscriberNext )
                    {
                    subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                    ASSERT_RESET_ALWAYS( subscriberLink, ( EISIIndicationHandlerQueNULLPointer15 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                    C_TRACE( ( _T("DISIIndicationHandler::PrintSubscriptions objid: 0x%x"), subscriberLink->iObjId ) );
                    subscriberNext = subscriberNext->iNext;
                    }
                indicationNext = indicationNext->iNext;
                }
            serverNext = serverNext->iNext;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DISIIndicationHandler::PrintSubscriptions<" ) ) );
    }

// -----------------------------------------------------------------------------
// DISIIndicationHandler::RemoveSubscription
// Removes all subscriptions from objectid
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::RemoveSubscription(
        const TUint16 aObjId
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::RemoveSubscription 0x%x>" ), aObjId ) );

    SDblQueLink*    deviceAnchor;
    if( iSubscriberIsAnExternalDevice )
        {
        deviceAnchor = &iExternalDeviceQ.iA;  
        iSubscriberIsAnExternalDevice = EFalse;
        }
    else
        {
        deviceAnchor = &iDeviceQ.iA;
        }
    SDblQueLink*    next = deviceAnchor->iNext;
    TDeviceIdLink*  deviceLink = NULL;

    SDblQueLink*        serverAnchor = NULL;
    SDblQueLink*        serverNext = NULL;
    TServerIdLink*      serverLink = NULL;

    SDblQueLink*        indicationAnchor = NULL;
    SDblQueLink*        indicationNext = NULL;
    TIndicationLink*    indicationLink = NULL;

    SDblQueLink*        subscriberAnchor = NULL;
    SDblQueLink*        subscriberNext = NULL;
    TSubscriberLink*    subscriberLink = NULL;

    while( deviceAnchor != next )
        {
        deviceLink = _LOFF( next, TDeviceIdLink, iDeviceLink );
        ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerQueNULLPointer16 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        E_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription device: 0x%x"), deviceLink->iDeviceId ) );
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        while( serverAnchor != serverNext )
            {
            serverLink = _LOFF( serverNext, TServerIdLink, iServerLink );
            ASSERT_RESET_ALWAYS( serverLink, ( EISIIndicationHandlerQueNULLPointer17 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            while( indicationAnchor != indicationNext )
                {
                indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
                ASSERT_RESET_ALWAYS( indicationLink, ( EISIIndicationHandlerQueNULLPointer18 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                E_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription indication: 0x%x"), indicationLink->iIndication ));
                // get next link already in case if particular link is to be deleted
                indicationNext = indicationNext->iNext;
                subscriberAnchor = &indicationLink->iSubscriberQ.iA;
                subscriberNext = subscriberAnchor->iNext;
                while( subscriberAnchor != subscriberNext )
                    {
                    subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                    ASSERT_RESET_ALWAYS( subscriberLink, ( EISIIndicationHandlerQueNULLPointer19 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
                    E_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription objid: 0x%x"), subscriberLink->iObjId ));
                    if( subscriberLink->iObjId == aObjId )
                        {
                        E_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove objid: 0x%x"), subscriberLink->iObjId ));
                        TBool onlySubscriber( subscriberLink->iSubscriberLink.Alone() );
                        subscriberLink->iSubscriberLink.Deque();
                        delete subscriberLink;
                        subscriberLink = NULL;
                        if( onlySubscriber )
                            {
                            E_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove indication: 0x%x"), indicationLink->iIndication ));
                            indicationLink->iIndicationLink.Deque();
                            delete indicationLink;
                            indicationLink = NULL;
                            }
                        // No need to go through rest of subscriberlinks since this one already found and deleted
                        subscriberNext = subscriberAnchor;
                        }
                    else
                        {
                        subscriberNext = subscriberNext->iNext;
                        }
                    }
                }
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            serverNext = serverNext->iNext;
            if( indicationAnchor == indicationNext )
                {
                // No more indications for the server -> server link can be deleted
                E_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove server: 0x%x"), serverLink->iResourceId ));
                serverLink->iServerLink.Deque();
                delete serverLink;
                serverLink = NULL;
                }
            }
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        next = next->iNext;
        if( serverAnchor == serverNext )
            {
            // No more servers for the device -> device link can be deleted
            E_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove device: 0x%x"), deviceLink->iDeviceId ));
            deviceLink->iDeviceLink.Deque();
            delete deviceLink;
            deviceLink = NULL;
            }
        }
    C_TRACE( ( _T( "DISIIndicationHandler::RemoveSubscription 0x%x<" ), aObjId ) );
    }
    
// -----------------------------------------------------------------------------
// DISIIndicationHandler::SendSubscription
//
// -----------------------------------------------------------------------------
//
void DISIIndicationHandler::SendSubscription( TDeviceIdLink& aDevice )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::SendSubscription>" ) ) );    
    TDes8& desPtr = MemApi::AllocBlock( ISI_HEADER_SIZE + SIZE_PNS_SUBSCRIBED_RESOURCES_EXTEND_IND + ( KMaxAmountOfResources * KResourceArraySize ) );

    TUint8* ptr( const_cast<TUint8*>( desPtr.Ptr() ) );
    C_TRACE( ( _T( "DISIIndicationHandler::SendSubscription SET_SENDER_DEV to PN_DEV_OWN = 0x%x and SET_RECEIVER_DEV to 0x%x" ), PN_DEV_OWN, aDevice.iDeviceId ) ); 
    ptr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
    SET_SENDER_DEV( ptr, PN_DEV_OWN );
    SET_RECEIVER_DEV( ptr, aDevice.iDeviceId );
    ptr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_COMMGR;
    SET_RECEIVER_OBJ( ptr, PN_OBJ_ROUTER );
    SET_SENDER_OBJ( ptr, PN_OBJ_EVENT_MULTICAST );
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_TRANSACTIONID ] = 0x00;
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_MESSAGEID ] = PNS_SUBSCRIBED_RESOURCES_EXTEND_IND;
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_FILLER ] = 0x00;
    
    TServerIdLink* serverLink = NULL;
    SDblQueLink* anchor = &(aDevice.iServerQ.iA);
    SDblQueLink* next = anchor->iNext;

    TUint8 resourceCount( 0x00 );
    while( anchor != next )
        {
        serverLink = _LOFF( next, TServerIdLink, iServerLink );
        ASSERT_RESET_ALWAYS( serverLink, ( EISIIndicationHandlerQueNULLPointer23 | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
        TUint32 resourceId = serverLink->iResourceId;
        C_TRACE( ( _T("DISIIndicationHandler::SendSubscription Server: 0x%08x"), resourceId ) );
        PUTB32( &ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCELIST + ( resourceCount * KResourceArraySize ) ], resourceId );
        resourceCount++;
        next = next->iNext;
        }
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCECOUNT ] = resourceCount;
    TInt messageLength( ( resourceCount * KResourceArraySize ) + KPnsSubscribedResourcesExtendLength );
    desPtr.SetLength( messageLength );
    SET_LENGTH( ptr, messageLength - PN_HEADER_SIZE );
    iRouter->Send( desPtr, PN_OBJ_EVENT_MULTICAST );
    C_TRACE( ( _T( "DISIIndicationHandler::SendSubscription<" ) ) );  
    }

// End of file.

