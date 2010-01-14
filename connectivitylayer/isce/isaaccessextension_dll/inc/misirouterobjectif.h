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



#ifndef __MISIROUTEROBJECTIF_H__
#define __MISIROUTEROBJECTIF_H__


/*
* Abstract interface for ISI router to use nameservice and communication manager services.
*/
class MISIRouterObjectIf
    {

    public:

        /*
        * Receive a message from router.
        * Responsibility to deallocate the message is transferred to channel.
        * Can be called in 1..N thread contextes.
        * Cat no be called with FM held.
        * Same restríctions than in IST API (see from MTrxMuxIf).
        * @param aMessage, message to receive
        */
        virtual void Receive( const TDesC8& aMessage ) = 0;

    };

#endif /* __MISIROUTEROBJECTIF_H__ */
