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



#ifndef __ISIROUTERLINKIF_H__
#define __ISIROUTERLINKIF_H__

#include <e32def.h>                 // For TInt, TAny


class MISILinkRouterIf;

/*
* Abstract interface for router to use link services.
*/
class MISIRouterLinkIf
    {

    public:

        /* 
        * Allocated memory for new link interface and returns it to caller.
        * Throws kern fault if not possible to create.
        * Created interface must be released with MISIRouterLinkIf::Release
        */
        static MISIRouterLinkIf* CreateLinkF( MISILinkRouterIf* aRouter,  const TUint8 aMediaId, const TUint8 aTrxId );

        /*
        * Releases resources reserved for interface implementation.
        */
        virtual void Release() = 0;

        /*
        * Sends a ISI message with given priority
        * @param aMsg, message to be send
        */
        virtual TBool Send( TDes8& aMsg ) = 0;

        /*
        * Ask state of the link.
        * Called with FM held, no blocking nor trace etc..
        * @return ETrue if connected, EFalse if disconnected
        */
        virtual TBool StateConnected() = 0;
        
        //  add comments
        virtual TUint8 GetTrxId() = 0;

    };

#endif /* __ISIROUTERLINKIF_H__ */
