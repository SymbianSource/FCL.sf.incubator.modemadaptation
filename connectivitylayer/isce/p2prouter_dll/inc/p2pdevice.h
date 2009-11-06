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



#ifndef __P2PDEVICE_H__
#define __P2PDEVICE_H__

//  INCLUDES
#include <kernel.h> // For DLogicalDevice
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* User side driver factory.
*
*/
NONSHARABLE_CLASS( DP2PDevice ) : public DLogicalDevice
    {

    public:

        /**
        * C++ default constructor.
        */
        DP2PDevice();

        /**
        * Destructor.
        */
        ~DP2PDevice();

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

#endif      // __P2PDEVICE_H__

// End of File
