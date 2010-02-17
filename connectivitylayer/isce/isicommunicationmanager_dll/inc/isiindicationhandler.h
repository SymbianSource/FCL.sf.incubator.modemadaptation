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



#ifndef __ISIINDICATIONHANDLER_H__
#define __ISIINDICATIONHANDLER_H__

#include <klib.h> // For DBase

class SDblQue;
class MISIObjectRouterIf;

NONSHARABLE_CLASS( DISIIndicationHandler ) : public DBase
    {

    private:

        class TDeviceIdLink
            {
            public:
                TUint8      iDeviceId;
                SDblQue     iServerQ;
                SDblQueLink iDeviceLink;
            };
        
        class TServerIdLink
            {
            public:
                TUint32     iResourceId;
                SDblQue     iIndicationQ;
                SDblQueLink iServerLink;
            };

        class TIndicationLink
            {
            public:
                TUint8      iIndication;
                SDblQue     iSubscriberQ;
                SDblQueLink iIndicationLink;
            };

        class TSubscriberLink
            {
            public:
                TUint8      iObjId;
                SDblQueLink iSubscriberLink;
            };
    
    public:
    
        /*
        * Constructor
        */
        DISIIndicationHandler( MISIObjectRouterIf* aRouter );

        /*
        * Destructor
        */
        ~DISIIndicationHandler();

        /*
        * Multicast and Subscribe must be called in same DFC thread.
        * If not then take a deep look at synchronization.
        */
        void Multicast( TDes8& aIndication );

        /*
        * Multicast and Subscribe must be called in same DFC thread.
        * If not then take a deep look at synchronization.
        */
        void Subscribe( TDes8& aSubscriptionReq );



    private:

        void AddIndication( TServerIdLink& aServerLink, const TUint8 aIndication, const TUint16 aObjId );

        void AddSubscriber( TIndicationLink& aIndicationLink, const TUint16 aObjId );

        void AddServer( TDeviceIdLink& aDeviceLink, const TUint32 aResourceId, const TUint8 aIndication, const TUint16 aObjId );

        void AddServer( TDeviceIdLink& aDeviceLink, const TUint32 aResourceId );

        TDeviceIdLink* GetDeviceLink( const TUint8 aDeviceId );

        TDeviceIdLink* GetExternalDeviceLink( const TUint8 aDeviceId );

        TServerIdLink* GetServerLink( TDeviceIdLink& aDeviceLink, const TUint32 aResourceId );

        TIndicationLink* GetIndicationLink( TServerIdLink& aServerLink, const TUint8 aIndication );

        TSubscriberLink* GetSubscriberLink( TIndicationLink& aIndicationLink, const TUint16 aObjId );

        void PrintSubscriptions();

        void RemoveSubscription( const TUint16 aObjId );

        void SendSubscription( TDeviceIdLink& aDevice );

    private:

        // Owned
        SDblQue         iDeviceQ;
        SDblQue         iExternalDeviceQ;
        // Not owned
        MISIObjectRouterIf* iRouter;

    };

#endif /* __ISIINDICATIONHANDLER_H__ */
