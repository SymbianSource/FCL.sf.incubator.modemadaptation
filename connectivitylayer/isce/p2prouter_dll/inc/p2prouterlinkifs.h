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



#ifndef __P2PROUTERLINKIFS_H__
#define __P2PROUTERLINKIFS_H__

#include <e32def.h>                 // For TInt, TAny

/*
* Abstract interface for link to use router services.
*/
class MP2PLinkRouterIf
    {

    public:

        /*
        * Shall enque a DFC handling transceiver presence changes.
        * Called with FM held, no blocking no nesting of FMs and no allocation.
        */
        virtual void NotifyTrxPresenceEnqueDfc( TBool aPresent ) = 0;

        /*
        * Receive message from link.
        * Called without FM held.
        * @param aMsg, message to be received
        * @param aProtocolId, links unique P2P protocol id
        */
        virtual void Receive( TDes8& aMsg, const TUint8 aProtocolId ) = 0;

    };

/*
* Abstract interface for router to use link services.
*/
class MP2PRouterLinkIf
    {

    public:

        /* 
        * Allocated memory for new link interface and returns it to caller.
        * Throws kern fault if not possible to create.
        * Created interface must be released with MP2PRouterLinkIf::Release
        */
        static MP2PRouterLinkIf* CreateLinkF( MP2PLinkRouterIf* aRouter, const TUint8 aProtocolId, const TUint8 aTrxId );

        /*
        * Returns the state information of links transceiver presence.
        * @return ETrue if links transceiver is present.
        */
        virtual TBool TrxPresent() = 0;

        /*
        * Releases resources reserved for interface implementation.
        */
        virtual void Release() = 0;

        /*
        * Sends a P2P message with given priority
        * @param aMsg, message to be send
        */
        virtual void Send( TDes8& aMsg ) = 0;

    };

#endif /* __P2PROUTERLINKIFS_H__ */
