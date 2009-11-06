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



#ifndef __ISICHANNELROUTERIF_H__
#define __ISICHANNELROUTERIF_H__

#include <e32def.h>     // For TInt, TAny

class MISIRouterChannelIf;

/*
* Abstract interface for channel to use router services.
*/
class MISIChannelRouterIf
    {

    public:

        enum TISIDfcQThreadType
            {
    	      EISIKernelMainThread = 0,
            EISIKernelRequestCompletionThread,
            EISIUserMainThread,
            EISIUserRequestCompletionThread
            };

        /*
        * Informs router that channel is closed.
        */
        virtual void Disconnect( const TUint8 aObjId ) = 0;

        /*
        * Returns thread according to TISIDfcThread enums.
        * @param aThreadIndex, threads index in table
        * @return pointer to thread
        */
        virtual TDfcQue* GetDfcThread( const TISIDfcQThreadType aType ) = 0;

        virtual void FreeDfcThread( TDfcQue* aThread ) = 0;

        /*
        * Returns the interface.
        * @return pointer to the interface
        */
        static MISIChannelRouterIf* GetIf();

        /*
        * Informs router that channel is ready to be open.
        * If transceiver is not found sets the open pending.
        * Router shall complete the pending opens when transceiver is ready.
        * @param aUID, clients UID.
        * @param aObjId, reference to write clients object id.
        * @param aCallback, pointer to channel interface.
        */
        virtual void Connect( const TInt32 aUID, TUint8& aObjId, MISIRouterChannelIf* aCallback ) = 0;

        /*
        * Sends a message to router.
        * Responsibility to deallocate the message is transferred router.
        * @param aMessage, messages reference
        * @return, KErrNone if succesfull, KErrNotReady if transceiver is not present.
        */
        virtual TInt Send( TDes8& aMessage, const TUint8 aObjId ) = 0;

    };

#endif /* __ISICHANNELROUTERIF_H__ */
