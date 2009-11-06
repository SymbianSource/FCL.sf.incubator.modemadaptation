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



#ifndef __P2PLINK_H__
#define __P2PLINK_H__

// Includes
#include "mmuxlinkif.h"         // For MMuxLinkIf
#include "p2prouterlinkifs.h"   // For MP2PRouterLinkIf

// Forward declarations
class MMuxLinkIf;
class MLinkMuxIf;

/*
* Class repsenting point-to-point router link to transceiver.
* Each link has one transceiver and one protocol.
* Responsible of storing the transceiver related state information.
*/
NONSHARABLE_CLASS( DP2PLink ) : public DBase, public MMuxLinkIf, public MP2PRouterLinkIf
    {

    public:

        /*
        * Default contructor
        */
        DP2PLink();

        /*
        * Contructor
        * @param aRouter, router interface
        * @param aProtocolId, protocol id of this link
        * @param aTrxId, transceiver id of this link.
        */
        DP2PLink( MP2PLinkRouterIf* aRouter, const TUint8 aProtocolId, const TUint8 aTrxId );

        /*
        * Destructor
        */
        ~DP2PLink();

        // From MMuxLinkIf start
        /*
        * See comments from MMuxLinkIf.
        */
        void Receive( TDes8& aMsg );

        /*
        * See comments from MMuxLinkIf.
        */
        void EnqueTrxPresenceChangedDfc( TBool aPresent );
        // From MMuxLinkIf end

        // From MP2PRouterLinkIf start
        /*
        * See comments from MP2PRouterLinkIf.
        */
        TBool TrxPresent();

        /*
        * See comments from MP2PRouterLinkIf.
        */
        void Release();

        /*
        * See comments from MP2PRouterLinkIf.
        */
        void Send( TDes8& aMsg );
        // From MP2PRouterLinkIf end

    private:

        // Added convention:
        // Prefix "iSh" means shared member variable
        // Prefix "gSh" means shared global variable

        // Not owned, can be called from 1...N thread, router responsible of it's own states guarding.
        MP2PLinkRouterIf*   iRouter;
        // Not owned, called always from one router thread.
        MLinkMuxIf*         iMux;
        // Links P2P protocol unique identifier.
        // Written only once fron one thread no need to sync.
        TUint8              iProtocolId;
        // Links transceiver unique identifier.
        TUint8              iShTrxId;

    };



#endif /* __P2PLINK_H__ */
