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



#ifndef __MULTIPLEXER_H__
#define __MULTIPLEXER_H__

#include "trxdefs.h"    // For EAmountOfTrxs

// Forward declarations
class MMuxTrxIf;
class MMuxLinkIf;
class MLinkMuxIf;
class MTrxMuxIf;
class DMux;

/*
* Container class for all the multiplexers.
*/
NONSHARABLE_CLASS( DMultiplexer ) : public DBase
    {

    public:

        /*
        * Default contructor
        */
        DMultiplexer();

        /*
        * Destructor
        */
        ~DMultiplexer();

        /*
        * Registers a transceiver with unique transceiver identifier to multiplexer.
        * Returns an interface to transceiver to use multiplexer services.
        * @param aTrx, pointer to transceiver interface
        * @param aTrxId, unique id of the tranceiver
        * @return interface to multiplexer
        */
        static MTrxMuxIf* RegisterTrx( MMuxTrxIf* aTrx, const TUint8 aTrxId );

        /*
        * Registers a link with unique link identifier to multiplexer to send and receive data through
        * transceiver identified with unique transceiver id.
        * Returns an interface to link to use multiplexer services.
        * @param aTrx, pointer to link interface
        * @param aTrxId, unique id of the tranceiver
        * @param aLinkId, unique id of the link
        * @return interface to multiplexer
        */
        static MLinkMuxIf* RegisterLink( MMuxLinkIf* aTrx, const TUint8 aTrxId, const TUint8 aLinkId );

    private:

        static DMux* GetMuxForTrx( const TUint8 aTrxId );

    private:

        // Added convention:
        // Prefix "iSh" means shared member variable
        // Prefix "gSh" means shared global variable

        // 1..N relatio, owned here
        static DMux*    iShMultiplexers[ EAmountOfTrxs ];
        // Mutex guarding iShMultiplexers, not owned here
        static DMutex*  iShMultiplexersMutex;

    };

#endif /* __MULTIPLEXER_H__ */
