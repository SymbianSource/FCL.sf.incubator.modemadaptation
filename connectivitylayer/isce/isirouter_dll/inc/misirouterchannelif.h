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



#ifndef __ISIROUTERCHANNELIF_H__
#define __ISIROUTERCHANNELIF_H__

#include <e32def.h>     // For TInt, TAny

/*
* Abstract interface for router to use channel services.
*/
class MISIRouterChannelIf
    {

    public:

        /*
        * Enques channel request completing DFC.
        * Called in P2P router ext thread context.
        * @param aRequest, request to be completed
        * @param aStatusToComplete, status to complete to client.
        */
        virtual void EnqueChannelRequestCompleteDfc( TInt aRequest, TInt aStatusToComplete ) = 0;

        /*
        * Receive a message from router.
        * Responsibility to deallocate the message is transferred to channel.
        * Can be called in 1..N thread contextes.
        * Cat no be called with FM held.
        * Same restríctions than in IST API (see from MTrxMuxIf).
        * @param aMessage, message to receive
        */
        virtual void ReceiveMsg( const TDesC8& aMessage ) = 0;

    };



#endif /* __ISIROUTERCHANNELIF_H__ */
