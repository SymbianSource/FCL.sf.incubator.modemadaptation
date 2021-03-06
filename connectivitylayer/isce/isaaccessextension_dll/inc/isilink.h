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



#ifndef __ISILINK_H__
#define __ISILINK_H__

// Includes
#include "mmuxlinkif.h"         // For MMuxLinkIf
#include "isirouterlinkifs.h"   // For MISIRouterLinkIf

// Forward declarations
class MMuxLinkIf;
class MLinkMuxIf;

/*
* Class repsenting ISI router link to transceiver.
* Each link has one transceiver and one protocol.
* Responsible of storing the transceiver related state information.
*/
NONSHARABLE_CLASS( DISILink ) : public DBase, public MMuxLinkIf, public MISIRouterLinkIf
    {

    public:

        /*
        * Default contructor
        */
        DISILink();

        /*
        * Contructor
        * @param aRouter, router interface
        * @param aMediaId, protocol id of this link
        * @param aTrxId, transceiver id of this link.
        */
        DISILink( MISILinkRouterIf* aRouter, const TUint8 aMediaId, const TUint8 aTrxId );

        /*
        * Destructor
        */
        ~DISILink();

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

        // From MISIRouterLinkIf start
        /*
        * See comments from MISIRouterLinkIf.
        */
        TBool TrxPresent();

        /*
        * See comments from MISIRouterLinkIf.
        */
        void Release();

        /*
        * See comments from MISIRouterLinkIf.
        */
        void Send( TDes8& aMsg );
        // From MISIRouterLinkIf end

    private:

        // Added convention:
        // Prefix "iSh" means shared member variable
        // Prefix "gSh" means shared global variable

        // Not owned, can be called from 1...N thread, router responsible of it's own states guarding.
        MISILinkRouterIf*   iRouter;
        // Not owned, called always from one router thread.
        MLinkMuxIf*         iMux;
        // Links unique meida identifier.
        // Written only once fron one thread no need to sync.
        TUint8              iMediaId;
        // Links transceiver unique identifier.
        TUint8              iShTrxId;

    };



#endif /* __ISILINK_H__ */
