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

#include <iscnokiadefinitions.h>
#include <pn_eventmodem_extisi.h>         // PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_...
#include <phonetisi.h>                    // For ISI_HEADER_SIZE
#include <pn_const.h>                     // For PN_HEADER_SIZE
#include "isiindicationhandler.h"         // For DISIIndicationHandler
#include "isicommunicationmanagertrace.h" // For C_TRACE..
#include "misiobjectrouterif.h"           // For DISICommunicationManager
#include "memapi.h"                       // For MemApi
#include "iadhelpers.h"                   // For SET_RECEIVER_OBJ...
#include "nsisi.h"                        // For PN_MEDIA_ROUTING_REQ
#include "ape_commgrisi.h"                // For APE_COMMGR..


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

#define GETB32(p) \
    (((TUint32) GETB16(p)<<16) | (TUint32) GETB16((TUint8 *)(p)+2))

// Faults
enum TISIIndincationHandlerFaults
    {
    EISIIndicationHandlerWrongParameter = 0x01,
    EISIIndicationHandlerMemAllocFailure,
    EISIIndicationHandlerMemAllocFailure1,
    EISIIndicationHandlerMemAllocFailure2,
    EISIIndicationHandlerMemAllocFailure3,
    };


// -----------------------------------------------------------------------------
// DISIIndicationHandler::
//
// -----------------------------------------------------------------------------
//
DISIIndicationHandler::DISIIndicationHandler(
        MISIObjectRouterIf* aRouter
        ) : iRouter( aRouter )
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
        C_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler device: 0x%x"), deviceLink->iDeviceId ) );
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        while( serverAnchor != serverNext )
            {
            serverLink = _LOFF( serverNext, TServerIdLink, iServerLink );
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            while( indicationAnchor != indicationNext )
                {
                indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
                C_TRACE( ( _T("DISIIndicationHandler::~DISIIndicationHandler indication: 0x%x"), indicationLink->iIndication ));
                // get next link already in case if particular link is to be deleted
                indicationNext = indicationNext->iNext;
                subscriberAnchor = &indicationLink->iSubscriberQ.iA;
                subscriberNext = subscriberAnchor->iNext;
                while( subscriberAnchor != subscriberNext )
                    {
                    subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
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
TInt DISIIndicationHandler::Subscribe(
        TDes8& aSubscriptionReq
        )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::Subscribe 0x%x>" ), &aSubscriptionReq ) );
    
    TUint8* msgPtr( const_cast<TUint8*>( aSubscriptionReq.Ptr() ) );
    
    TInt msgLength( aSubscriptionReq.Length() ); 
    TUint32 resourceId( 0 );
    TUint8 deviceId( 0 );
    TUint8 indication( 0 );
    TUint16 objId( 0 );
      
    objId = msgPtr[ ISI_HEADER_OFFSET_SENDEROBJECT ];
    RemoveSubscription( objId );
    for( TInt subBlockOffset( ISI_HEADER_SIZE + SIZE_APE_COMMGR_SUBSCRIBE_REQ ); subBlockOffset < msgLength; subBlockOffset += ( SIZE_APE_COMMGR_SUBSCRIBE_SB ) )
        {
        deviceId = msgPtr[ subBlockOffset + APE_COMMGR_SUBSCRIBE_SB_OFFSET_DEVICEID ];
        resourceId = GETB32( &msgPtr[ subBlockOffset + APE_COMMGR_SUBSCRIBE_SB_OFFSET_FILLERBYTE1 ] ); //resource offset
        indication = msgPtr[ subBlockOffset + APE_COMMGR_SUBSCRIBE_SB_OFFSET_RESOURCEID ];
        C_TRACE( ( _T("DISIIndicationHandler indication 0x%x objId 0x%x resource 0x%x device 0x%x"), indication, objId, resourceId, deviceId ) );
        TDeviceIdLink* deviceLink = GetDeviceLink( deviceId );
        if( !deviceLink )
            {
            C_TRACE( ( _T("DISIIndicationHandler::Subscribe device not found yet -> create: 0x%x"), deviceId ) );
            deviceLink = new TDeviceIdLink();
            ASSERT_RESET_ALWAYS( deviceLink, ( EISIIndicationHandlerMemAllocFailure | EDISIIndicationHandlerTraceId << KClassIdentifierShift ) );
            deviceLink->iDeviceId = deviceId;
            iDeviceQ.Add( &( deviceLink->iDeviceLink ) );
            }
        AddServer( *deviceLink, resourceId, indication, objId );
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
        C_TRACE( ( _T("DISIIndicationHandler::Subscribe device: 0x%x"), deviceLink->iDeviceId ) );
        if( deviceLink->iDeviceId != PN_DEV_OWN )
            {
            C_TRACE( ( _T("DISIIndicationHandler::Subscribe send subscription to device: 0x%x"), deviceLink->iDeviceId ) );
            SendSubscription( *deviceLink );
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DISIIndicationHandler::Subscribe<" ) ) );
    return KErrNone;
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
        C_TRACE( ( _T( "DISIIndicationHandler::GetDeviceLink device: 0x%x"), tmpDeviceLink->iDeviceId ) );
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
        C_TRACE( ( _T( "DISIIndicationHandler::GetServerLink server: 0x%x"), tmpServerLink->iResourceId ) );
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
        tmpSubscriberLink= _LOFF( next, TSubscriberLink, iSubscriberLink );
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
        C_TRACE( ( _T("DISIIndicationHandler::PrintSubscriptions device: 0x%x"), deviceLink->iDeviceId ) );
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        while( serverAnchor != serverNext )
            {
            serverLink = _LOFF( serverNext, TServerIdLink, iServerLink );
            C_TRACE( ( _T("DISIIndicationHandler::PrintSubscriptions server: 0x%x"), serverLink->iResourceId ) );
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            while( indicationAnchor != indicationNext )
                {
                indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
                C_TRACE( ( _T("DISIIndicationHandler::PrintSubscriptions indication: 0x%x"), indicationLink->iIndication ) );
                subscriberAnchor = &indicationLink->iSubscriberQ.iA;
                subscriberNext = subscriberAnchor->iNext;
                while( subscriberAnchor != subscriberNext)
                    {
                    subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                    subscriberLink = subscriberLink;
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
        C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription device: 0x%x"), deviceLink->iDeviceId ) );
        serverAnchor = &deviceLink->iServerQ.iA;
        serverNext = serverAnchor->iNext;
        while( serverAnchor != serverNext )
            {
            serverLink = _LOFF( serverNext, TServerIdLink, iServerLink );
            indicationAnchor = &serverLink->iIndicationQ.iA;
            indicationNext = indicationAnchor->iNext;
            while( indicationAnchor != indicationNext )
                {
                indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
                C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription indication: 0x%x"), indicationLink->iIndication ));
                // get next link already in case if particular link is to be deleted
                indicationNext = indicationNext->iNext;
                subscriberAnchor = &indicationLink->iSubscriberQ.iA;
                subscriberNext = subscriberAnchor->iNext;
                while( subscriberAnchor != subscriberNext )
                    {
                    subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                    C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription objid: 0x%x"), subscriberLink->iObjId ));
                    if( subscriberLink->iObjId == aObjId )
                        {
                        C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove objid: 0x%x"), subscriberLink->iObjId ));
                        TBool onlySubscriber( subscriberLink->iSubscriberLink.Alone() );
                        subscriberLink->iSubscriberLink.Deque();
                        delete subscriberLink;
                        subscriberLink = NULL;
                        if( onlySubscriber )
                            {
                            C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove indication: 0x%x"), indicationLink->iIndication ));
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
                C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove server: 0x%x"), serverLink->iResourceId ));
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
            C_TRACE( ( _T("DISIIndicationHandler::RemoveSubscription remove device: 0x%x"), deviceLink->iDeviceId ));
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
TInt DISIIndicationHandler::SendSubscription( TDeviceIdLink& aDevice )
    {
    C_TRACE( ( _T( "DISIIndicationHandler::SendSubscription>" ) ) );
    const TUint8 KMaxAmountOfResources( 255 );
    const TUint8 KPnsSubscribedResourcesExtendLength( 12 );
    const TUint8 KResourceArraySize( 4 );
    
    TDes8& desPtr = MemApi::AllocBlock( ISI_HEADER_SIZE + SIZE_PNS_SUBSCRIBED_RESOURCES_EXTEND_IND + ( KMaxAmountOfResources * KResourceArraySize ) );

    TUint8* ptr( const_cast<TUint8*>( desPtr.Ptr() ) );
    ptr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_ROUTING_REQ;
    SET_RECEIVER_DEV( ptr, aDevice.iDeviceId );
    SET_SENDER_DEV( ptr, PN_DEV_OWN );
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
        TUint32 resourceId = serverLink->iResourceId;
        C_TRACE( ( _T("DISIIndicationHandler::SendSubscription Server: 0x%08x"), resourceId ) );
        PUTB32( &ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCELIST ], resourceId );
        resourceCount++;
        next = next->iNext;
        }
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCECOUNT ] = resourceCount;
    TInt messageLength( ( resourceCount * KResourceArraySize ) + KPnsSubscribedResourcesExtendLength );
    while( messageLength % 4 != 0 )
        {
        TRACE_ASSERT_INFO(0, messageLength);
        messageLength++;
        }
    desPtr.SetLength( messageLength );
    SET_LENGTH( ptr, messageLength - PN_HEADER_SIZE );
      
    for( TInt i( 0 ); i < desPtr.Length(); i++ )
        {
        C_TRACE( ( _T( "index[ %d ] data 0x%x"), i, desPtr.Ptr()[i] ) );
        }
    TInt error = iRouter->Send( desPtr, PN_OBJ_EVENT_MULTICAST );
    C_TRACE( ( _T( "DISIIndicationHandler::SendSubscription<" ) ) );  
    return error;
    }

// End of file.

