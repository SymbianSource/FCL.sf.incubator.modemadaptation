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



#ifndef NCP_COMMON_BRIDGE_FAMILY
#include <cmisi.h>                      // PN_COMMMG, PNS_SUBSCRIBED_RESOURCES_...
#else
#include <pn_eventmodem_extisi.h>       // PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_...
#endif
#include <phonetisi.h>                  // For ISI_HEADER_SIZE
#include <pn_const.h>                   // For PN_HEADER_SIZE
#ifndef NCP_COMMON_BRIDGE_FAMILY
#include <mediaisi.h>                   // For PNS_MEDIA_SOS
#endif
#include "indicationhandler.h"
#include "iadtrace.h"                   // For C_TRACE..
#include "router.h"                     // For DRouter
#include "iadhelpers.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "indicationhandlerTraces.h"
#endif
                 // For GET_RECEIVER

DIndicationHandler::DIndicationHandler(
        DRouter& aRouter
        )
        : iRouter( aRouter )
    {
    OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_DINDICATIONHANDLER_ENTRY, ">DIndicationHandler::DIndicationHandler;aRouter=%x", ( TUint )&( aRouter ) );

    C_TRACE( ( _T( "DIndicationHandler::DIndicationHandler 0x%x ->" ), &iRouter ) );
    C_TRACE( ( _T( "DIndicationHandler::DIndicationHandler 0x%x <-" ), &iRouter ) );

    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_DINDICATIONHANDLER_EXIT, "<DIndicationHandler::DIndicationHandler" );
    }

DIndicationHandler::~DIndicationHandler(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_DINDICATIONHANDLER_DES_ENTRY, ">DIndicationHandler::~DIndicationHandler" );

    C_TRACE( ( _T( "DIndicationHandler::~DIndicationHandler ->" ) ) );
    C_TRACE( ( _T( "DIndicationHandler::~DIndicationHandler <-" ) ) );

    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_DINDICATIONHANDLER_DES_EXIT, "<DIndicationHandler::~DIndicationHandler" );
    }


void DIndicationHandler::Multicast(
        TDes8& aIndication
        )
    {

    OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_MULTICAST_ENTRY, ">DIndicationHandler::Multicast;aIndication=%x", ( TUint )&( aIndication ) );
    // TODO: ! NOTE ! This is run in commonrx dfc dfc queu, how long will this take? latency to dfc thread and commondfc switch to own dfc instead?
    C_TRACE( ( _T( "DIndicationHandler::Multicast 0x%x ->" ), &aIndication ) );
//TODO: change there const to offsets?
    const TUint8 KPnMsgExtendedType( 9 );
    const TUint8 KPnMsgExtendedSubType( 10 );
    TUint8* ptr = const_cast<TUint8*>( aIndication.Ptr() );
    TUint32 resourceId( 0x00000000 );
    if( ptr[ ISI_HEADER_OFFSET_RESOURCEID ] == PN_PREFIX )
        {
#ifdef NCP_COMMON_BRIDGE_FAMILY
        TRACE_ASSERT_ALWAYS;// So called "perävalotakuu" for indications PN_PREFIX defined to PN_COMMGR see iscnokiadefinitions.h
#endif
        C_TRACE( ( _T( "DIndicationHandler::Multicast PN_PREFIX 0x%x" ), &aIndication ) );
        OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_MULTICAST_PN_PREFIX, "DIndicationHandler::Multicast extended resource;aIndication=%x", (TUint)&(aIndication) );
        
        ASSERT_RESET_ALWAYS( aIndication.Length() > KPnMsgExtendedType , EIADOverTheLimits | EIADFaultIdentifier40 << KFaultIdentifierShift );                                                
        resourceId = ( ptr [ KPnMsgExtendedSubType ] << 16 ) + ( ptr [ KPnMsgExtendedType ] << 8 ) +  ( PN_PREFIX );
        }
    else 
        {
        ASSERT_RESET_ALWAYS( aIndication.Length() > ISI_HEADER_OFFSET_RESOURCEID , EIADOverTheLimits | EIADFaultIdentifier41 << KFaultIdentifierShift );                                                            
        resourceId = ptr[ ISI_HEADER_OFFSET_RESOURCEID ];
        }
    TServerIdLink* serverLink = GetServerLink( resourceId );
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
        while( subscriberAnchor != subscriberNext)
            {
            subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
            C_TRACE( ( _T("DIndicationHandler::Multicast Subscriber channel: 0x%x"), subscriberLink->iChannelId ));
            // Make a copy to another subscriber
            TDes8& tempPtr = iRouter.AllocateBlock( aIndication.Length() );
            tempPtr.Copy( ptr, aIndication.Length() );
            TUint8* ptr = const_cast<TUint8*>( tempPtr.Ptr() );
            SET_RECEIVER_OBJ( ptr, subscriberLink->iChannelId );
            ASSERT_RESET_ALWAYS( subscriberLink->iChannelId != KIADEventSubscriptionObjId, 0 );
            iRouter.CheckRouting( iRouter, tempPtr );
            subscriberNext = subscriberNext->iNext;
            }
        }
    C_TRACE( ( _T( "DIndicationHandler::Multicast 0x%x <-" ), &aIndication ) );
    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_MULTICAST_EXIT, "<DIndicationHandler::Multicast" );
    }

TInt DIndicationHandler::Subscribe(
        TDes8& anOrder,
        const TUint16 aCh,
        const TBool a32BitResource
        )
    {
    OstTraceExt3( TRACE_NORMAL, DINDICATIONHANDLER_SUBSCRIBE_ENTRY, ">DIndicationHandler::Subscribe;anOrder=%x;aCh=%hx;a32BitResource=%d", ( TUint )&( anOrder ), aCh, a32BitResource );

    C_TRACE( ( _T( "DIndicationHandler::Subscribe 0x%x ->" ), aCh ) );
    RemoveSubscription( aCh );
    TInt eventTableLength( anOrder.Length() );
    TUint32 resourceId( 0x00000000 );
    TUint8 extendedLength( a32BitResource ? 0x03 : 0x00 );
    TUint8* orderPtr( const_cast<TUint8*>( anOrder.Ptr() ) );

    TInt remove( 0 );
    // Deorder of indication 8-bit { 0x00, 0x00 } and 32-bit { 0x00, 0x00, 0x00, 0x00, 0x00 }
    for( TInt i( 0 ); i < eventTableLength; ++i )
        {
        remove += ( orderPtr[ i ] == 0x00 ? 1 : 0 );           
        }
    eventTableLength = ( eventTableLength == remove ? 0 : eventTableLength );
    C_TRACE( ( _T( "DIndicationHandler::Subscribe 0x%x length %d 32-bit %d" ), aCh, eventTableLength, a32BitResource ) );

    OstTraceExt4( TRACE_NORMAL, DINDICATIONHANDLER_SUBSCRIBE, "DIndicationHandler::Subscribe;anOrder=%x;aCh=%hx;a32BitResource=%d;eventTableLength=%hhu", ( TUint )&( anOrder ), aCh, a32BitResource, eventTableLength );
    

    /*
    for( TInt i( 0 ); i < anOrder.Length(); i++ )
        {
        Kern::Printf( "subscribe order[%d]0x%x", i, anOrder.Ptr()[ i ] );
        }
    */

    for( TInt i( 0 ); i < eventTableLength; i += ( 2 + extendedLength ) )
        {
        if( a32BitResource )
            {
            ASSERT_RESET_ALWAYS( ( KErrNone == eventTableLength % 5 ), EIADWrongParameter | ( (TUint8)aCh << KChannelNumberShift ) | EIADFaultIdentifier1 << KFaultIdentifierShift );
            resourceId = orderPtr[ i ] << 24 | orderPtr[ i + 1 ] << 16 | orderPtr[ i + 2 ] << 8 | orderPtr[ i + 3 ];
            C_TRACE( ( _T("DIndicationHandler::Subscribe 32-bit Channel: 0x%x Server: 0x%x Indication 0x%x"), aCh, resourceId, orderPtr[ i + 4 ] ) );
            OstTraceExt3( TRACE_DETAILED, DUP1_DINDICATIONHANDLER_SUBSCRIBE_32BIT, "DIndicationHandler::Subscribe 32bit;aCh=%hx;resourceId=%x;indication=%hhx", aCh, resourceId, orderPtr[ i + 4 ] );
            }
        else 
            {
            ASSERT_RESET_ALWAYS( ( KErrNone == eventTableLength % 2 ), EIADWrongParameter | ( (TUint8)aCh << KChannelNumberShift ) | EIADFaultIdentifier2 << KFaultIdentifierShift );
            resourceId = orderPtr[ i ];
            C_TRACE( ( _T("DIndicationHandler::Subscribe Channel: 0x%x Server: 0x%x Indication 0x%x"), aCh, resourceId, orderPtr[ i + 1 ] ) );
            OstTraceExt3( TRACE_DETAILED, DUP1_DINDICATIONHANDLER_SUBSCRIBE_8BIT, "DIndicationHandler::Subscribe 8bit;aCh=%hx;resourceId=%x;indication=%hhx", aCh, resourceId, orderPtr[ i + 1 ] );
            }
        // TODO: Ensure that ownership is not given! If given leaks memory below in allocation..!
        TServerIdLink* serverLink = GetServerLink( resourceId );
        if( !serverLink )
            {
            C_TRACE( ( _T("DIndicationHandler::Subscribe ServerIdLink not found yet -> create: 0x%x"), resourceId ) );
            OstTrace1( TRACE_DETAILED, DINDICATIONHANDLER_SUBSCRIBE_SERVERLINK, "DIndicationHandler::Subscribe serverlink not yet created;resourceId=%x", resourceId );
            
            serverLink = new TServerIdLink();
            ASSERT_RESET_ALWAYS( serverLink, EIADMemoryAllocationFailure | EIADFaultIdentifier1 << KFaultIdentifierShift );
            serverLink->iResourceId = resourceId;
            iServerQ.Add( &( serverLink->iServerLink ) );
            }
        AddIndication( *serverLink, orderPtr[ i + ( 1 + extendedLength ) ], aCh );
        }

#ifdef _DEBUG
#ifdef COMPONENT_TRACE
    PrintSubscriptions();
#endif // COMPONENT_TRACE
#endif // _DEBUG
    C_TRACE( ( _T( "DIndicationHandler::Subscribe 0x%x <-" ), aCh ) );
    TInt error = SendSubscription();


    OstTrace1( TRACE_NORMAL, DUP1_DINDICATIONHANDLER_SUBSCRIBE, "DIndicationHandler::Subscribe<;error=%d", error );
    return error;
    }

void DIndicationHandler::AddIndication(
        TServerIdLink& aServerLink,
        const TUint8 aIndication,
        const TUint16 aCh
        )
    {
    OstTraceExt3( TRACE_NORMAL, DINDICATIONHANDLER_ADDINDICATION_ENTRY, ">DIndicationHandler::AddIndication;aServerLink=%x;aIndication=%hhu;aCh=%hx", ( TUint )&( aServerLink ), aIndication, aCh );

    C_TRACE( ( _T( "DIndicationHandler::AddIndication 0x%x 0x%x ->" ), aIndication, aCh ) );

    // TODO: ownership given or not?
    TIndicationLink* indicationLink = GetIndicationLink( aServerLink, aIndication );
    if( !indicationLink )
        {
        C_TRACE( ( _T("DIndicationHandler::AddIndication Indication: 0x%x not found, create"), aIndication ));
        OstTraceExt1( TRACE_NORMAL, DINDICATIONHANDLER_ADDINDICATION, "DIndicationHandler::AddIndication indicationLink not found -> create;aIndication=%hhx", aIndication );
        
        indicationLink = new TIndicationLink();
        ASSERT_RESET_ALWAYS( indicationLink, EIADMemoryAllocationFailure | EIADFaultIdentifier2 << KFaultIdentifierShift );
        indicationLink->iIndication = aIndication;
        aServerLink.iIndicationQ.Add( &( indicationLink->iIndicationLink ) );
        }
    AddSubscriber( *indicationLink, aCh );
    C_TRACE( ( _T( "DIndicationHandler::AddIndication 0x%x 0x%x <-" ), aIndication, aCh ) );

    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_ADDINDICATION_EXIT, "<DIndicationHandler::AddIndication" );
    }

void DIndicationHandler::AddSubscriber(
        TIndicationLink& aIndicationLink,
        const TUint16 aCh
        )
    {
    OstTraceExt2( TRACE_NORMAL, DINDICATIONHANDLER_ADDSUBSCRIBER_ENTRY, ">DIndicationHandler::AddSubscriber;aIndicationLink=%x;aCh=%hx", ( TUint )&( aIndicationLink ), aCh );

    C_TRACE( ( _T( "DIndicationHandler::AddSubscriber 0x%x, 0x%x ->" ), &aIndicationLink, aCh ) );
    TSubscriberLink* subscriberLink = GetSubscriberLink( aIndicationLink, aCh );
    if( !subscriberLink )
        {
        C_TRACE( ( _T("DIndicationHandler::AddSubscriber Subscriber not found: 0x%x 0x%x"), aIndicationLink.iIndication, aCh ) );
        OstTraceExt2( TRACE_NORMAL, DINDICATIONHANDLER_ADDSUBSCRIBER, "DIndicationHandler::AddSubscriber subscriber not found -> create;aIndicationLink.iIndication=%hhu;aCh=%hx", aIndicationLink.iIndication, aCh );
        
        subscriberLink = new TSubscriberLink();
        ASSERT_RESET_ALWAYS( subscriberLink, EIADMemoryAllocationFailure | EIADFaultIdentifier3 << KFaultIdentifierShift );
        subscriberLink->iChannelId = aCh;
        aIndicationLink.iSubscriberQ.Add( &( subscriberLink->iSubscriberLink ) );
        }
    else
        {
        TRACE_ASSERT_INFO(0, (TUint8)aCh<<KChannelNumberShift);
        C_TRACE( ( _T( "DIndicationHandler::AddSubscriber double subscription 0x%x" ), aCh ) );
        OstTraceExt1( TRACE_NORMAL, DINDICATIONHANDLER_ADDSUBSCRIBER_DOUBLE_SUB, "DIndicationHandler::AddSubscriber double subscription;aCh=%hx", aCh );        
        }
    C_TRACE( ( _T( "DIscIsiEventHandler::AddSubscriber <-" ) ) );

    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_ADDSUBSCRIBER_EXIT, "<DIndicationHandler::AddSubscriber" );
    }

DIndicationHandler::TServerIdLink* DIndicationHandler::GetServerLink(
        const TUint32 aResourceId
        )
    {
    OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_GETSERVERLINK_ENTRY, ">DIndicationHandler::GetServerLink;aResourceId=%u", ( TUint )( aResourceId ) );

    C_TRACE( ( _T( "DIndicationHandler::GetServerLink 0x%x ->" ), aResourceId ) );

    TServerIdLink* serverLink = NULL;
    TServerIdLink* tmpServerLink = NULL;
    SDblQueLink* anchor = &iServerQ.iA;
    SDblQueLink* next = anchor->iNext;

    while( anchor != next ) // servers
        {
        tmpServerLink = _LOFF( next, TServerIdLink, iServerLink );
        C_TRACE( ( _T( "DIndicationHandler::GetServerLink Server: 0x%x"), tmpServerLink->iResourceId ) );
        OstTrace1( TRACE_DETAILED, DINDICATIONHANDLER_GETSERVERLINK, "DIndicationHandler::GetServerLink;tmpServerLink->iResourceId=%u", ( TUint )( tmpServerLink->iResourceId ) );
        if( tmpServerLink->iResourceId == aResourceId )
            {
            serverLink = tmpServerLink;
            break;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DIndicationHandler::GetServerLink 0x%x <-" ), serverLink ) );

    OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_GETSERVERLINK_EXIT, "DIndicationHandler::GetServerLink<;serverLink=%x", serverLink );
    return serverLink;
    }

DIndicationHandler::TIndicationLink* DIndicationHandler::GetIndicationLink(
        TServerIdLink& aServerLink,
        const TUint8 aIndication
        )
    {
    OstTraceExt2( TRACE_NORMAL, DINDICATIONHANDLER_GETINDICATIONLINK_ENTRY, ">DIndicationHandler::GetIndicationLink;aServerLink=%x;aIndication=%hhu", ( TUint )&( aServerLink ), aIndication );

    C_TRACE( ( _T( "DIndicationHandler::GetIndicationLink 0x%x ->" ), aIndication ) );
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
    C_TRACE( ( _T( "DIndicationHandler::GetIndicationLink 0x%x <-" ), indicationLink ) );


    OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_GETINDICATIONLINK_EXIT, "<DIndicationHandler::GetIndicationLink;indicationLink=%x", indicationLink );
    return indicationLink;
    }

DIndicationHandler::TSubscriberLink* DIndicationHandler::GetSubscriberLink(
        TIndicationLink& aIndicationLink,
        const TUint16 aCh
        )
    {
    OstTraceExt2( TRACE_NORMAL, DINDICATIONHANDLER_GETSUBSCRIBERLINK_ENTRY, ">DIndicationHandler::GetSubscriberLink;aIndicationLink=%x;aCh=%hx", ( TUint )&( aIndicationLink ), aCh );

    C_TRACE( ( _T( "DIndicationHandler::GetSubscriberLink 0x%x, 0x%x ->" ), &aIndicationLink, aCh ) );
    TSubscriberLink* tmpSubscriberLink = NULL;
    TSubscriberLink* subscriberLink = NULL;

    SDblQueLink* anchor = &aIndicationLink.iSubscriberQ.iA;
    SDblQueLink* next = anchor->iNext;

    while( anchor != next )
        {
        tmpSubscriberLink= _LOFF( next, TSubscriberLink, iSubscriberLink );
        if ( tmpSubscriberLink->iChannelId == aCh )
            {
            subscriberLink = tmpSubscriberLink;
            break;
            }
        next = next->iNext;
        }

    C_TRACE( ( _T( "DIndicationHandler::GetSubscriberLink 0x%x <-" ), subscriberLink ) );
    
    OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_GETSUBSCRIBERLINK_EXIT, "<DIndicationHandler::GetSubscriberLink;subscriberLink=%x", subscriberLink );
    return subscriberLink;
    }

void DIndicationHandler::PrintSubscriptions(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_PRINTSUBSCRIPTIONS_ENTRY, ">DIndicationHandler::PrintSubscriptions" );

    C_TRACE( ( _T( "DIndicationHandler::PrintSubscriptions ->" ) ) );
    TServerIdLink* serverLink = NULL;//_LOFF(listElement, TServerIdLink, iServerLink);
    SDblQueLink* anchor = &iServerQ.iA;
    SDblQueLink* next = anchor->iNext;
    SDblQueLink* indicationAnchor = NULL;
    SDblQueLink* indicationNext = NULL;
    TIndicationLink* indicationLink = NULL;
    SDblQueLink* subscriberAnchor = NULL;
    SDblQueLink* subscriberNext = NULL;
    TSubscriberLink* subscriberLink = NULL;
    while( anchor != next ) // servers
        {
        serverLink = _LOFF( next, TServerIdLink, iServerLink );
        C_TRACE( ( _T("DIndicationHandler::PrintSubscriptions Server: 0x%x"), serverLink->iResourceId));
        OstTrace1( TRACE_DETAILED, DINDICATIONHANDLER_PRINTSUBSCRIPTIONS, "DIndicationHandler::PrintSubscriptions;resource=%x", (TUint)(serverLink->iResourceId) );// todo check these prints
        
        indicationAnchor = &serverLink->iIndicationQ.iA;
        indicationNext = indicationAnchor->iNext;
        while( indicationAnchor != indicationNext )
            {
            indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
            C_TRACE( ( _T("DIndicationHandler::PrintSubscriptions Indication: 0x%x"), indicationLink->iIndication ) );
            OstTraceExt1( TRACE_DETAILED, DINDICATIONHANDLER_PRINTSUBSCRIPTIONS_INDICATION, "DIndicationHandler::PrintSubscriptions;indication=%hhx", indicationLink->iIndication );
            
            subscriberAnchor = &indicationLink->iSubscriberQ.iA;
            subscriberNext = subscriberAnchor->iNext;
            while( subscriberAnchor != subscriberNext)
                {
                subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                subscriberLink = subscriberLink;
                C_TRACE( ( _T("DIndicationHandler::PrintSubscriptions Channel: 0x%x"), subscriberLink->iChannelId ) );
                OstTraceExt1( TRACE_DETAILED, DUP1_DINDICATIONHANDLER_PRINTSUBSCRIPTIONS_CHANNEL, "DIndicationHandler::PrintSubscriptions;subscriberLink->iChannelId=%hx", subscriberLink->iChannelId );
                
                subscriberNext = subscriberNext->iNext;
                }
            indicationNext = indicationNext->iNext;
            }
        next = next->iNext;
        }
    C_TRACE( ( _T( "DIndicationHandler::PrintSubscriptions <-" ) ) );

    OstTrace0( TRACE_DETAILED, DUP1_DINDICATIONHANDLER_PRINTSUBSCRIPTIONS_EXIT, "<DIndicationHandler::PrintSubscriptions" );
    }


void DIndicationHandler::RemoveSubscription(
        const TUint16 aCh
        )
    {
    OstTraceExt1( TRACE_NORMAL, DINDICATIONHANDLER_REMOVESUBSCRIPTION_ENTRY, ">DIndicationHandler::RemoveSubscription;aCh=%hx", aCh );

    C_TRACE( ( _T( "DIndicationHandler::RemoveSubscription 0x%x ->" ), aCh ) );
    // enter semaphore

    SDblQueLink*    serverAnchor = &iServerQ.iA;
    SDblQueLink*    next = serverAnchor->iNext;
    TServerIdLink*  serverLink = NULL;

    SDblQueLink*        indicationAnchor = NULL;
    SDblQueLink*        indicationNext = NULL;
    TIndicationLink*    indicationLink = NULL;

    SDblQueLink*        subscriberAnchor = NULL;
    SDblQueLink*        subscriberNext = NULL;
    TSubscriberLink*    subscriberLink = NULL;

    while( serverAnchor != next ) // servers
        {
        serverLink = _LOFF( next, TServerIdLink, iServerLink );
        C_TRACE( ( _T("DIndicationHandler::RemoveSubscription Server: 0x%x"), serverLink->iResourceId ) );
        OstTrace1( TRACE_NORMAL, DINDICATIONHANDLER_REMOVESUBSCRIPTION, "DIndicationHandler::RemoveSubscription;resource=%x", serverLink->iResourceId );
        
        indicationAnchor = &serverLink->iIndicationQ.iA;
        indicationNext = indicationAnchor->iNext;
        while( indicationAnchor != indicationNext )
            {
            indicationLink = _LOFF( indicationNext, TIndicationLink, iIndicationLink );
            // get next link already in case if particular link is to be deleted
            indicationNext = indicationNext->iNext;
            subscriberAnchor = &indicationLink->iSubscriberQ.iA;
            subscriberNext = subscriberAnchor->iNext;
            while( subscriberAnchor != subscriberNext )
                {
                subscriberLink = _LOFF( subscriberNext, TSubscriberLink, iSubscriberLink );
                C_TRACE( ( _T("DIndicationHandler::RemoveSubscription Subscriber channel: 0x%x"), subscriberLink->iChannelId ));
                OstTraceExt1( TRACE_NORMAL, DUP1_DINDICATIONHANDLER_REMOVESUBSCRIPTION_CHANNEL, "DIndicationHandler::RemoveSubscription;channelId=%hx", subscriberLink->iChannelId );
                
                if( subscriberLink->iChannelId == aCh )
                    {
                    TBool onlySubscriber( subscriberLink->iSubscriberLink.Alone() );
                    subscriberLink->iSubscriberLink.Deque();
                    delete subscriberLink;
                    subscriberLink = NULL;
                    if( onlySubscriber )
                        {
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
        next = next->iNext;
        if( indicationAnchor == indicationNext )
            {
            // No more indications for the server -> server link can be deleted
            serverLink->iServerLink.Deque();
            delete serverLink;
            serverLink = NULL;
            }
        }
    // exit semaphore
    C_TRACE( ( _T( "DIndicationHandler::RemoveSubscription 0x%x <-" ), aCh ) );

    OstTraceExt1( TRACE_NORMAL, DINDICATIONHANDLER_REMOVESUBSCRIPTION_EXIT, "<DIndicationHandler::RemoveSubscription;aCh=%hx", aCh );
    }


TInt DIndicationHandler::SendSubscription(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DINDICATIONHANDLER_SENDSUBSCRIPTION_ENTRY, ">DIndicationHandler::SendSubscription" );

    C_TRACE( ( _T( "DIndicationHandler::SendSubscription ->" ) ) );
    const TInt KSubsriptionLength( 512 );
    const TUint8 KPnsSubscribedResourcesExtendLength( 12 );
    const TUint8 KIADEventSubscriptionObjId( 0xfc );
    // TODO: Assertion to somewhere else?
    TDes8& desPtr =  iRouter.AllocateBlock( KSubsriptionLength );
    ASSERT_RESET_ALWAYS( KSubsriptionLength > ( ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCECOUNT ) , EIADOverTheLimits | EIADFaultIdentifier42 << KFaultIdentifierShift );                                                        	

    TUint8* ptr( const_cast<TUint8*>( desPtr.Ptr() ) );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    ptr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_SOS;
#else
    ptr[ ISI_HEADER_OFFSET_MEDIA ] = PN_MEDIA_MODEM_HOST_IF;
#endif
    SET_RECEIVER_DEV( ptr, OTHER_DEVICE_1 );
    SET_SENDER_DEV( ptr, THIS_DEVICE );
    ptr[ ISI_HEADER_OFFSET_RESOURCEID ] = PN_COMMGR;
    SET_RECEIVER_OBJ( ptr, PN_OBJ_ROUTER );

    SET_SENDER_OBJ( ptr, KIADEventSubscriptionObjId );
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_TRANSACTIONID ] = 0x00;
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_MESSAGEID ] = PNS_SUBSCRIBED_RESOURCES_EXTEND_IND;
    desPtr.SetLength( KPnsSubscribedResourcesExtendLength );

    TServerIdLink* serverLink = NULL;
    SDblQueLink* anchor = &iServerQ.iA;
    SDblQueLink* next = anchor->iNext;

    TUint8 serverIndex( 0x00 );
    TUint8 resourceCount( 0x00 );
    while( anchor != next ) // servers
        {
        serverLink = _LOFF( next, TServerIdLink, iServerLink );
        TUint32 resourceId = serverLink->iResourceId;
        C_TRACE( ( _T("DIndicationHandler::SendSubscription Server: 0x%08x"), resourceId ) );
        OstTrace1( TRACE_DETAILED, DINDICATIONHANDLER_SENDSUBSCRIPTION, "DIndicationHandler::SendSubscription;resourceId=%x", resourceId );
        
        desPtr.Append( static_cast<TUint8>( resourceId >> 24 ) );
        desPtr.Append( static_cast<TUint8>( resourceId >> 16 ) );
        desPtr.Append( static_cast<TUint8>( resourceId >> 8 ) );
        desPtr.Append( static_cast<TUint8>( resourceId ) );
        resourceCount++;
        next = next->iNext;
        serverIndex++;
        }
    ptr[ ISI_HEADER_SIZE + PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCECOUNT ] = resourceCount;

    TInt messageLength( ( resourceCount * 4 ) + KPnsSubscribedResourcesExtendLength );
    while( messageLength % 4 != 0 )
        {
        TRACE_ASSERT_ALWAYS;
        messageLength++;
        }
    // TODO : does this work? setting of length after append?
    desPtr.SetLength( messageLength );
    SET_LENGTH( ptr, messageLength - PN_HEADER_SIZE );
    C_TRACE( ( _T( "DIndicationHandler::SendSubscription <-" ) ) );    
    TInt error = iRouter.SendMessage( desPtr, KIADEventSubscriptionObjId );

    OstTrace1( TRACE_DETAILED, DINDICATIONHANDLER_SENDSUBSCRIPTION_EXIT, "<DIndicationHandler::SendSubscription<;error=%d", error );
    return error;
    }


// End of file.


