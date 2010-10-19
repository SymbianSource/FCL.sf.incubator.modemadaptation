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


#ifndef __ISICOMMUNICATIONMANAGER_H__
#define __ISICOMMUNICATIONMANAGER_H__

#include "misirouterobjectif.h"

class MISIObjectRouterIf;
class DISIIndicationHandler;

/*
* ISI router.
*/
NONSHARABLE_CLASS( DISICommunicationManager ) : public DBase, public MISIRouterObjectIf
    {

    public:

        /*
        * Default contructor
        */
        DISICommunicationManager();

        /*
        * Destructor
        */
        ~DISICommunicationManager();

        /*
        * See comments from MISIRouterObjectIf
        */
        void Receive( const TDesC8& aMessage );

    private:

        void HandleBootMessage( const TDesC8& aMessageDuringBoot );

        void HandleCommunicationManagerCommonMessage( const TDesC8& aMessageToCommunicationManager );

        void HandleApeCommunicationManagerMessage( const TDesC8& aMessageToCommunicationManager );

        void HandlePnCommgrMessage( const TDesC8& aMessageToCommunicationManager );

        void HandleMessageMulticast( const TDesC8& aMessageToMulticast );

        void SendNameAddReqs();

        void SendApeCommgrSubscribeResp( const TDesC8& aMessage );

        void SendCommServiceNotIdentifiedResp( const TDesC8& aMessage );

        void SendCommIsiVersionGetResp( const TDesC8& aMessage );

        void SetIsiHeaderForCommonMessage( TDes8& aCommonResponse,
                                           const TDesC8& aCommonRequest );

        static void InitDfc( TAny* aPtr );

    private:
        
        // Owned
        static DMutex*         iCommunicationManagerMutex;
        TDfc*                  iInitDfc;
        DISIIndicationHandler* iISIIndicationHandler;
        TUint8                 iObjectIdentifier;
        TUint8                 iBootMsgCount;
        // Not owned
        MISIObjectRouterIf*    iRouter;
        
    };

#endif /* __ISICOMMUNICATIONMANAGER_H__ */

