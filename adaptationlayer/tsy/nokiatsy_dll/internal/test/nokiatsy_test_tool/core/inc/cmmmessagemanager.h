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



#ifndef CMmMessageManager_H
#define CMmMessageManager_H

// INCLUDES
#include "cmmmessagemanagerbase.h"
#include "ctsy/pluginapi/cmmdatapackage.h"
#include "ctsy/serviceapi/mmtsy_ipcdefs.h"
#include "pcktcs.h"
#include "satcs.h"
#include "etelcdmacs.h"
#include "etelmmcs.h"

// CONSTANTS
	//none

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmMessageRouterBase;

// CLASS DECLARATION
/**
*  Forwards Tsy requests from Symbian OS to Domestic OS
*  
*  @lib (unknown)
*  @since 2.6
*/
class CMmMessageManager : public CMmMessageManagerBase
    {

    // public methods
    public:

        /**
        * Creates a new CMmMessageManager object
        * @return reference to created CMmMessageManager type of object
        */
        static CMmMessageManager* NewL();

        /**
        * Destructor
        */
        virtual ~CMmMessageManager();

        /**
        * Creates and initialises class attributes
        */
        void ConstructL();

		/**
        * Forwards a request to DOS layer, request doesn't contain data
        */
        virtual TInt HandleRequestL( 
			const TInt aIpc );

		/**
        * Forwards a request to DOS layer, request contains 
        * packaged data by Common TSY
        */
        virtual TInt HandleRequestL( 
                const TInt aIpc,
                const CMmDataPackage* aDataPackage );

		/**
        * Forwards a request to DOS layer, request contains 
        * packaged data by Etel Server.
        */
        virtual TInt HandleRequestL(    
                const TInt aIpc,
                const TDataPackage* aDataPackage );

		/**
        * Completes a request or a notification with no data
        */
        virtual void Complete( TInt aIpc, TInt aResult );

        /**
        * Completes a request or a notification with packaged data
        */
        virtual void Complete( TInt aIpc, CMmDataPackage* data,
			TInt aResult );

        /**
        * Registers a Tsy object for request and notification completions
        */
        virtual void RegisterTsyObject( TTsyObjects aTsyObjectType, 
			CBase* aTsyObject );

        /**
        * Deregisters a Tsy object for request and notification completions
        */
        virtual void DeregisterTsyObject( CBase* aTsyObject );

		/** 
		* Sets the message router object
		*/
		void SetMessageRouter( CMmMessageRouterBase* aMessageRouter );

	// protected methods
    protected:


	// private methods
    private:

        /**
        * Default constructor
        */
        CMmMessageManager();

    // public attributes
    public:

    // protected attributes
    protected:

        //writing package for Handlers (only one at the moment)
        CMmDataPackage iDataPackage;

		CMmMessageRouterBase* iMessageRouter;
	// private attributes
    private:

    };


#endif // CMmMessageManager_H

//  End of file
