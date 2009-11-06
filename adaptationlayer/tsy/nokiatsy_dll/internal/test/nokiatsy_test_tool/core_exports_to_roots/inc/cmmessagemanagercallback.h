/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Forwards Tsy requests from Symbian OS to Domestic OS
*
*/



#ifndef CMMMESSAGEMANAGERCALLBACK_H
#define CMMMESSAGEMANAGERCALLBACK_H

// INCLUDES

#include <ctsy/pluginapi/cmmdatapackage.h>
#include "ctsy/pluginapi/mmmessagemanagercallback.h"


// CONSTANTS
    //none

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class CCore;



// CLASS DECLARATION
/**
*  Forwards Tsy requests from Symbian OS to Domestic OS
*/
class CMmMessageManagerCallback : public MmMessageManagerCallback
    {

    // public methods
    public:

        /**
        * Creates a new CMmMessageManager object
        * @return reference to created CMmMessageManager type of object
        */
        static CMmMessageManagerCallback* NewL( CCore* aCore );

        /**
        * Destructor
        */
        virtual ~CMmMessageManagerCallback();

        /**
        * From MmMessageManagerCallback
        * Completes a request or a notification with no data
        */
        void Complete( TInt aIpc, TInt aResult );

        /**
        * From MmMessageManagerCallback
        * Completes a request or a notification with packaged data
        */
        void Complete( TInt aIpc, CMmDataPackage* data, TInt aResult );
        
    // protected methods
    protected:


    // private methods
    private:
    
        void ConstructL ( CCore* aCore );

        /**
        * Default constructor
        */
        CMmMessageManagerCallback();

    // public attributes
    public:

    // protected attributes
    protected:

        //writing package for Handlers (only one at the moment)
        CMmDataPackage iDataPackage;

    // private attributes
    private:
    
        CCore* iCore;

    };


#endif // CMMMESSAGEMANAGERCALLBACK_H

//  End of file
