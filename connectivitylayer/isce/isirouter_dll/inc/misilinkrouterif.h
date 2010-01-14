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



#ifndef __ISILINKROUTERIF_H__
#define __ISILINKROUTERIF_H__

#include <e32def.h>                 // For TInt, TAny

/*
* Abstract interface for link to use router services.
*/
class MISILinkRouterIf
    {

    public:

        /*
        * Receive message from link.
        * Called without FM held.
        * @param aMsg, message to be received
		* @return Error value
        */
        virtual TInt RouteISIMessage( TDes8& aMsg ) = 0;
    };



#endif /* __ISILINKROUTERIF_H__ */
