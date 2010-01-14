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



#ifndef __MUX_H__
#define __MUX_H__

// Class declarations
class MMuxTrxIf;
class MMuxLinkIf;

#include <mtrxmuxif.h>      // For exported MTrxMuxIf

#include "iscedefs.h"       // For EMuxAmountOfProtocols
#include "mlinkmuxif.h"     // For internal MLinkMuxIf

/*
* Class represents a multiplexer.
* De-multiplexes data from one transceiver to multiple protocol links.
* Multiplexes data from multiple protocol links to one transceiver.
*/
NONSHARABLE_CLASS( DMux ) : public DBase, public MTrxMuxIf, public MLinkMuxIf
    {

    public:

        /*
        * Default constructor
        * @param aTrxId, transceivers id
        */
        DMux( const TUint8 aTrxId );

        /*
        * Destructor
        */
        ~DMux();

        /*
        * Returns unique id of the transceiver where this multiplexer is connected.
        *@return unique transceiver id
        */
        TUint8 GetTrxId() const;

        /*
        * Set link with unique link
        * @return unique transceiver id
        */
        void SetLink( MMuxLinkIf* aLink, const TUint8 aLinkId );

        /*
        * Set transceiver 
        *@param pointer to transceivers interface
        */
        void SetTrx( MMuxTrxIf* aTrx );

        // From MTrxMuxIf start
        /*
        * See comments from MTrxMuxIf.
        */
        TDes8& AllocateBlock( const TUint16 aSize );

        /*
        * See comments from MTrxMuxIf.
        */
        void DeallocateBlock( TDes8& aBlock );

        /*
        * See comments from MTrxMuxIf.
        * @param aData, data to be de-multiplexed to approriate protocol link accorind to aMuxingHeader
        * @param aMuxingHeader, 32-bit multiplexing protocol header, implementation obeys MeXe Modem-Host communication protocol L2
        */
        void Receive( TDes8& aData, const TUint32 aMuxingHeader );

        /*
        * See comments from MTrxMuxIf.
        */
        void Unregister();
        // From MTrxMuxIf end

        // From MLinkMuxIf

        /*
        * See comments from MLinkMuxIf.
        * Multiplexer shall obey MeXe Modem-Host communication protocol L2
        * @param aMsg, message to be multiplexed
        * @param aMuxingHeaderProtocolId, unique multiplexing header protocol id, implementation obeys MeXe Modem-Host communication protocol L2
        * @param aPriority, priority of the message multiplexer shall map priority from link to transceiver
        */
        void Send( TDes8& aMsg, const TUint8 aMuxingHeaderProtocolId, const TMessageSendPriority aPriority = EPriorityNormal );
        // From MLinkMuxIf

    private:

        /*
        * Called when transceiver is either registered or unregistered.
        * @param aPresent, true if registered
        */
        void NotifyTrxStatusChangeToAllLinks( TBool aPresent );
        
    private:

        // Added convention:
        // Prefix "iSh" means shared member variable
        // Prefix "gSh" means shared global variable

        // Guards class shared states of this class, owned
        NFastMutex*     iFastMutex;
        // Each multiplexer can have 1..N protocols, not owned
        MMuxLinkIf*     iShLinks[ EMuxAmountOfProtocols ];
        // Each multiplexer can have 1 transceiver, not ownet
        MMuxTrxIf*      iShTrx;
        // Transceivers unique identifier
        TUint8          iShTrxId;

    };


#endif /* __MUX_H__ */
