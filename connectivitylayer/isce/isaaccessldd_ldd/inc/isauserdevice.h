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



#ifndef ISAUSERDEVICE_H
#define ISAUSERDEVICE_H

//  INCLUDES
#include <kernel.h> // For DLogicalChannel
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  User side driver factory up to first 32 channels.
*
*  @lib ISAAccessLDD.ldd
*/
class DISAUserDevice : public DLogicalDevice
    {

    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C DISAUserDevice();

        /**
        * Destructor.
        */
        IMPORT_C ~DISAUserDevice();

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

#endif      // ISAUSERDEVICE_H

// End of File
