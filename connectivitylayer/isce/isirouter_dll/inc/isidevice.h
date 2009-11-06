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



#ifndef __ISIDEVICE_H__
#define __ISIDEVICE_H__

//  INCLUDES
#include <kernel.h> // For DLogicalDevice


/**
* User side driver factory.
*/
NONSHARABLE_CLASS( DISIDevice ) : public DLogicalDevice
    {

    public:

        /**
        * C++ default constructor.
        */
        DISIDevice();

        /**
        * Destructor.
        */
        ~DISIDevice();

        // Functions from base class DLogicalDevice starts

        /**
        * Called by LDD FW in user thread context. More from DLogicalDevice.
        * @param aChannel
        * @return TInt, KErrNone if ok.
        */
        virtual TInt Create( DLogicalChannelBase*& aChannel );

        /**
        * 2nd stage constructor. More from DLogicalDevice.
        * @return TInt, KErrNone if ok.
        */
        virtual TInt Install();

        /**
        * Gets capabilities. More from DLogicalDevice.
        * @param aDes, descriptor where to set capas.
        * @return None
        */
        virtual void GetCaps( TDes8& aDes ) const;

        // Functions from base class DLogicalDevice ends

        // None own data members.

    };

#endif      // __ISIDEVICE_H__

// End of File
