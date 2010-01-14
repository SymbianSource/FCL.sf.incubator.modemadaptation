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


#ifndef __ISINAMESERVICE_H__
#define __ISINAMESERVICE_H__

#include "misirouterobjectif.h"
#include "misicommunicationmanagerif.h"

class MISIObjectRouterIf;
class DNameRecords;

/*
* ISI router.
*/
NONSHARABLE_CLASS( DISINameService ) : public DBase, public MISIRouterObjectIf, MISICommunicationManagerIf
    {

    public:

        /*
        * Default contructor
        */
        DISINameService();

        /*
        * Destructor
        */
        ~DISINameService();

        /*
        * See comments from MISIRouterObjectIf
        */
        void Receive( const TDesC8& aMessage );

        void HandlePNSNameMessage( const TDesC8& aMessage );
        void HandleNameQueryResp( const TDesC8& aMessage, const TUint8* msgPtr );
        void HandleNameRemoveResp( const TDesC8& aMessage, const TUint8* msgPtr );

        /*
        * See comments from MISICommunicationManagerIf
        */
        static TBool IsValidResource( const TDesC8& aMessage );

        /*
        * Construct COMM_ISA_ENTITY_NOT_REACHABLE_RESP message and
        * send message to router.
        */
        void BuildAndSendCommIsaEntityNotReachableResp( const TDesC8& aMsg );

    private:

        MISIObjectRouterIf* iRouter;
        TUint8              iObjId;
        DNameRecords*       iNameRecords;
        static DMutex*      iNameServiceMutex;

    };

#endif /* __ISINAMESERVICE_H__ */

