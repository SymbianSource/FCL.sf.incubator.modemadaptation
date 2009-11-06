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



#ifndef __MUX_LINK_IF__
#define __MUX_LINK_IF__

/*
* Part of ISCEs internal MUX API.
* Abstract interface for multiplexer to use link services.
*/
class MMuxLinkIf
    {

    public:

        /*
        * Receive de-multiplexed message from multiplexer to specific protocol handler link.
        * Shall be called without FastMutex held. Shall not block the calling thread.
        * TODO : restictions from IST API.
        * @param aMsg, message from multiplexer
        */
        virtual void Receive( TDes8& aMsg ) = 0;

        /*
        * Notifies link of transceiver presecense changes. Implementation shall enque a DFC.
        * Called with fast mutex held. Shall not block the calling thread nor held other FMs.
        * Can be called from 1...N thread contextes.
        * When ok, possible to send. When not ok not possible to send.
        */
        virtual void EnqueTrxPresenceChangedDfc( TBool aPresent ) = 0;

    };

#endif __MUX_LINK_IF__
