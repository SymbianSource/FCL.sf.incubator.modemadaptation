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



#ifndef INC_DINDICATIONHANDLER_H
#define INC_DINDICATIONHANDLER_H

#include <klib.h> // For DBase

class DRouter;
class SDblQue;

NONSHARABLE_CLASS( DIndicationHandler ) : public DBase
    {

    public:

        // Class to queue subscribed servers.
        class TServerIdLink
            {
            public:
                TUint32      iResourceId;
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
                TUint8      iChannelId;
                SDblQueLink iSubscriberLink;
            };

        DIndicationHandler( DRouter& aRouter );

        ~DIndicationHandler();

        /*
        * Multicast and Subscribe must be called in same DFC thread.
        * If not then take a deep look at synchronization.
        */
        void Multicast( TDes8& aIndication );

        /*
        * Multicast and Subscribe must be called in same DFC thread.
        * If not then take a deep look at synchronization.
        */
        TInt Subscribe( TDes8& anOrder, const TUint16 aCh, const TBool a32BitResource );

    private:

        void AddIndication( TServerIdLink& aServerLink, const TUint8 aIndication, const TUint16 aCh );

        void AddSubscriber( TIndicationLink& aIndicationLink, const TUint16 aCh );

        TServerIdLink* GetServerLink( const TUint32 aResourceId );

        TIndicationLink* GetIndicationLink( TServerIdLink& aServerLink, const TUint8 aIndication );

        TSubscriberLink* GetSubscriberLink( TIndicationLink& aIndicationLink, const TUint16 aCh );

        void PrintSubscriptions();

        void RemoveSubscription(const TUint16 aCh );

        TInt SendSubscription();

    private:

        // owned
        SDblQue         iServerQ;
        // not owned
        DRouter&        iRouter;

    };

#endif /* INC_DINDICATIONHANDLER_H */
