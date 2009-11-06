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



// Forward declarations
class MMuxLinkIf;

#ifndef __LINK_MUX_IF__
#define __LINK_MUX_IF__

/*
* Part of ISCEs internal MUX API.
* Abstract interface for protocol link to use multiplexer services.
*/
class MLinkMuxIf
    {

    public:

        enum TMessageSendPriority
            {
            EPriorityNormal     = 5,
            EPriorityHigh       = 10,
            };

        /*
        * Informs multiplexer that link is present and returns an interface tomultiplexer for link.
        * Shall not be called with FM held.
        * Creates a new DMux for aTrxId (if not created yet).
        * @param aTrx, pointer to links interface
        * @param aTrxId, unique identifier of the transceiver where link shall register
        * @param aLinkId, unique identifier of the registering link
        * @return pointer to multiplexer interface
        */
        IMPORT_C static MLinkMuxIf* Register( MMuxLinkIf* aTrx, const TUint8 aTrxId, const TUint8 aLinkId );

        /*
        * Sends a message to multiplexer.
        * Multiplexer shall multiplex the message and create the needed multiplexing protocol header on
        * based on aMsg and aMuxingHeaderProtocolId.
        * @param aMsg, message formatted according to links protocol
        * @param aMuxingHeaderProtocolId, unique multiplexing protocol header id of links protocol
        * @param aPriority, send priority, initialized as normal.
        */
        virtual void Send( TDes8& aMsg, const TUint8 aMuxingHeaderProtocolId, const TMessageSendPriority aPriority = EPriorityNormal ) = 0;

    };

#endif __LINK_MUX_IF__
