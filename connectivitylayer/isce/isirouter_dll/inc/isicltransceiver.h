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

#ifndef __ISICLTRANSCEIVER_H__
#define __ISICLTRANSCEIVER_H__

#include "misilinkrouterif.h"     // For MISILinkRouterIf
#include "isiinternaldefs.h"      // For KMaxNumOfObjId


class MISIRouterLinkIf;
class DISIRouter;

NONSHARABLE_CLASS( DISICLTransceiver ) : public DBase, public MISILinkRouterIf
    {

    public:

        DISICLTransceiver( DISIRouter* aPtr );

        ~DISICLTransceiver();

        TInt ValidateISIMessage( TDes8& aMessage );

        TInt SendCommIsaEntityNotReachableResp( TDes8& aMessage );

        TUint8 MapDeviceToMedia( const TUint8 aDevice );

        //From MISILinkRouterIf
        TInt RouteISIMessage( TDes8& aMessage );

    private:
    
        //From objectapi
        TInt Send( TDes8& aMessage, const TUint8 aObjId );
    
        enum TISIMedias
            {
            EISIMediaSharedMemory = 0x00,
            EISIMediaTest,
            EISIAmountOfMedias
            };

        MISIRouterLinkIf**   iLinksArray;
        DISIRouter*          iRouter;
    
    public:
     
        static DISICLTransceiver* iThisPtr;   
        
    };

#endif /* __ISICLTRANSCEIVER_H__ */

