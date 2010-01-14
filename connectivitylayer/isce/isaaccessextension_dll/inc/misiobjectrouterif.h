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



#ifndef __MISIOBJECTROUTERIF_H__
#define __MISIOBJECTROUTERIF_H__

class MISIRouterObjectIf;

/*
* Abstract interface for nameservice and communication manager to use router services.
*/
class MISIObjectRouterIf
    {

    public:
        
        /*
        * Sends a message to router.
        * Responsibility to deallocate the message is transferred router.
        * @param aMessage, messages reference
        * @return, KErrNone if succesfull, KErrNotReady if transceiver is not present.
        */
        virtual TInt Send( TDes8& aMessage, const TUint8 aObjId ) = 0;

        /*
        * Informs router that channel is ready to be open.
        * If transceiver is not found sets the open pending.
        * Router shall complete the pending opens when transceiver is ready.
        * @param aUID, clients UID.
        * @param aObjId, reference to write clients object id.
        * @param aCallback, pointer to channel interface.
        */
        IMPORT_C static MISIObjectRouterIf* Connect( const TInt32 aUID, TUint8& aObjId, MISIRouterObjectIf* aCallback );        

    };

#endif /* __MISIOBJECTROUTERIF_H__ */
