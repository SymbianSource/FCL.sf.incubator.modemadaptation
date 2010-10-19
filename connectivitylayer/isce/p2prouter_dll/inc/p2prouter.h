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



#ifndef __P2PROUTER_H__
#define __P2PROUTER_H__

#include "p2prouterlinkifs.h"       // For MP2PRouterLinkIf
#include "p2prouterchannelifs.h"    // For MP2PChRouterIf and MP2PRouterChIf
#include "p2pdefs.h"                // For EP2PAmountOfProtocols

class DMsgQueue;
class DP2PLink;
class MP2PRouterChIf;

/*
* Point-to-point router.
*/
NONSHARABLE_CLASS( DP2PRouter ) : public DBase, public MP2PLinkRouterIf, public MP2PChRouterIf
    {

    public:

        /*
        * Default contructor
        */
        DP2PRouter();

        /*
        * Destructor
        */
        ~DP2PRouter();

        // From MP2PLinkRouterIf start
        /*
        * See comments from MP2PLinkRouterIf
        */
        void NotifyTrxPresenceEnqueDfc( TBool aPresent );

        /*
        * See comments from MP2PLinkRouterIf
        */
        void Receive( TDes8& aMsg, const TUint8 aProtocolId );
        // From MP2PLinkRouterIf end

        // From MP2PChRouterIf start
        /*
        * See comments from MP2PChRouterIf
        */
        void Close( const TUint8 aProtocolId );

        /*
        * See comments from MP2PChRouterIf
        */
        TBool ConnectionExist( const TUint8 aProtocolId );

        /*
        * See comments from MP2PChRouterIf
        */
        TDfcQue* GetDfcThread( const TP2PDfcThread anIndex );

        /*
        * See comments from MP2PChRouterIf
        */
        void Connect( const TUint8 aProtocolId, MP2PRouterChIf* aCallback );

        /*
        * See comments from MP2PChRouterIf
        */
        TInt Send( TDes8& aMessage, const TUint8 aProtocolId );
        // From MP2PChRouterIf end

        /*
        * Returns pointer to router it self, called only from MP2PChRouterIf::GetIf
        * @return pointer to router.
        */
        static DP2PRouter* GetRouter(){ return iSelfPtr; };

    private:

        /*
        * Handles initialization of this class.
        * Called in own thread context not supervisors
        */
        void Init();

        static void InitDfc( TAny* aPtr );

        static void TrxPrecentDfc( TAny* aPtr );

        static void TrxNotPrecentDfc( TAny* aPtr );

    private:

        // Added convention:
        // Prefix "iSh" means shared member variable
        // Prefix "gSh" means shared global variable

        NONSHARABLE_CLASS( TP2PChannel )
            {
            public:
                MP2PRouterChIf*   iChannel;
                MP2PRouterChIf*   iWaitingChannel;
                TBool             iTrxConnectionStatus;
            };

        // Guards class shared state of this class, owned
        NFastMutex*         iShChannelTableFastMutex;
        // One channel / P2P protocol
        TP2PChannel         iShChannelTable[ EP2PAmountOfProtocols ];
        // One link / P2P protocol (cause P2P protocols can use only one transceiver (p2p))
        // Owned, written only once in construction from one thread no need to sync reading.
        MP2PRouterLinkIf**  iLinksArray;
        // Owned, written only once in construction from one thread no need to sync reading.
        static TDfcQue*     iP2PDfcQueList[ MP2PChRouterIf::EAmountOfP2PDfcThreads ];
        // Owned
        TDfc*               iInitDfc;
        // Owned
        TDfc*               iTrxPrecentDfc;
        // Owned
        TDfc*               iTrxNotPrecentDfc;
        // Written only once no need to synch.
        static DP2PRouter*  iSelfPtr;

    };

#endif /* __P2PROUTER_H__ */
