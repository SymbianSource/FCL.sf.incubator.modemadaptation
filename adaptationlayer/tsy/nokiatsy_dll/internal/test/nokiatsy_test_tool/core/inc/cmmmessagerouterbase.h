/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Routes between Tsy objects and Message Handler objects
*
*/



#ifndef CMMMESSAGEROUTERBASE_H
#define CMMMESSAGEROUTERBASE_H


//  INCLUDES
#include <e32base.h>
#include "cmmmessagemanagerbase.h"


// FORWARD DECLARATIONS
class MMmMessHandlerBase;
class CMmDataPackage;
class CMmMessageManager;
class CCore;

// CLASS DECLARATION
class CMmMessageRouterBase
        : public CBase
    {

    // public methods
    public:

        /**
        * Creates a new CMmMessageRouterBase object
        * @return reference to created CMmMessageRouterBase type of object
        */
        static CMmMessageRouterBase* NewL( CMmMessageManager* /*aMessageManager*/ )
			{
			// dummy implementation
			return new ( ELeave ) CMmMessageRouterBase();
			}

        /**
        * Destructor
        */
        IMPORT_C virtual ~CMmMessageRouterBase();

        /**
        * Returns pointer to the message handler for this 
		* request, based on IPC number
        */
        IMPORT_C virtual MMmMessHandlerBase* RouteRequest( TInt /*aIpc*/ );

        /**
        * Completes a request or notification via the specific Tsy object,
		* based on IPC number and data package
        */
        IMPORT_C virtual void RouteCompletion( TInt aIpc, 
            CMmDataPackage* aDataPackage, TInt aResult );

        /**
        * Sets the pointer to PhoneTsy (Common TSY)
        **/
        IMPORT_C void SetCore( CCore* aCore );


        /**
        * Registers a Tsy object for request and notification completions
        */
        void RegisterTsyObject( 
			CMmMessageManagerBase::TTsyObjects aTsyObjectType, CBase* aTsyObject );


        /**
        * Deregisters a Tsy object for request and notification completions
        */
        void DeregisterTsyObject( CBase* aTsyObject );
        
   // protected methods
    protected:
      //None
    // private methods
    private:

		/**
		* Returns the type of Tsy object needed for completion, based 
		* on the IPC number
		*/
		CMmMessageManagerBase::TTsyObjects GetTsyObjectFromIpc( const TInt aIpc ) const;

		/**
		* Returns the pointer to the Tsy object needed for completion, based 
		* on the IPC number and the data package
		*/
        CBase* GetObjectForCompletion(
                const TInt aIpc,
                CMmDataPackage* callPackage
                );


    // public attributes
    public:

    // protected attributes
    protected:

		// pointer to Core
		CCore* iCore;

    // private attributes
    private:

    };


#endif // CMmMessageRouterBase_H

//  End of file
