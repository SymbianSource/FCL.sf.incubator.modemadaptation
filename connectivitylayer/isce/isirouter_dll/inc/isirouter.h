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



#ifndef __ISIROUTER_H__
#define __ISIROUTER_H__


#include "misilinkrouterif.h"     // For MISILinkRouterIf
#include "misiobjectrouterif.h"  // For MISIObjectRouterIf
#include "isiinternaldefs.h"      // For KMaxAmountOfObjId
//#include "misiobjectrouterif.h"


class MISIRouterObjectIf;
class MISIRouterObjectIf;
class MISIRouterLinkIf;
class DISICLTransceiver;
class DISIThreadContainer;

/*
* ISI router.
*/
NONSHARABLE_CLASS( DISIRouter ) : public DBase, public MISIObjectRouterIf
    {

    public:

        /*
        * Default contructor
        */
        DISIRouter();

        /*
        * Destructor
        */
        ~DISIRouter();
        
        //From CLTransceiver
        TBool Receive( TDes8& aMessage, const TUint8 aObjId );
        
        /*
        * See comments from MISIObjectRouterIf
        */
        TInt Send( TDes8& aMessage, const TUint8 aObjId );

        /*
        * See comments from MISIObjectRouterIf
        */
        void Disconnect( const TUint8 aObjId );

        /*
        * See comments from MISIObjectRouterIf
        */
        TDfcQue* GetDfcThread( const TISIDfcQThreadType aType );

        void FreeDfcThread( TDfcQue* aThread );

        /*
        * See comments from MISIObjectRouterIf
        */
        void Connect( const TInt32 aUID, TUint8& aObjId, MISIRouterObjectIf* aCallback );

        /*
        * Returns pointer to router it self, called only from MISIObjectRouterIf::GetIf
        * @return pointer to router.
        */
        static DISIRouter* GetRouter(){ return iSelfPtr; };

    private:

        NONSHARABLE_CLASS( TISIClient )
            {
            public:
                TInt32                 iUID;
                TUint8                 iObjId;
                MISIRouterObjectIf*   iChannel;
            };

        NONSHARABLE_CLASS( TStaticId )
            {
            public:
                TStaticId( const TInt32 aUID, TUint8 aObjId ){iUID = aUID; iObjId = aObjId;};
                TInt32 iUID;
                TUint8 iObjId;
            };

    private:

        /*
        * Handles initialization of this class.
        * Called in own thread context not supervisors
        */
        void Initialize();

        TBool CheckUIDUniqueness( const TInt32 aUID );

        TUint8 ReserveStaticObjId( const TInt32 aUID );

        TUint8 ReserveNewDynamicObjId();

        static void InitializeDfc( TAny* aPtr );

    public:

        // Written only once no need to synch.
        static DISIRouter*   iSelfPtr;


        DISICLTransceiver* GetDISICLTransceiver(){ return iShCLTransceiver;};

    private:
	
        // Owned, synchronized internally with fastmutexes.
        RArray< TStaticId* > iStaticObjIdTable;
        // Owned, synchronized internally with fastmutexes.
        TISIClient*          iClientTable[ KMaxAmountOfObjId ];//  switch this to objects
        // Guards class shared array, owned
        NFastMutex*          iClientTableFastMutex;
        // Owned, executed only from one thread.
        TDfc*                iInitializeDfc;
        // Owned, executed only from one thread.
        TDfcQue*             iInitThread;
        // Owned, synch done inside the class.
        DISICLTransceiver*   iShCLTransceiver;
        // Owned, synch done inside the class.
        DISIThreadContainer* iShClientThreadContainer;

    };

#endif /* __ISIROUTER_H__ */

