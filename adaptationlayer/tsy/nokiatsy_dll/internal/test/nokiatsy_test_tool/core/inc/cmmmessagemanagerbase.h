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
* Description:   Forwards Tsy requests from Symbian OS to Domestic OS 
*
*/



#ifndef CMMMESSAGEMANAGERBASE_H
#define CMMMESSAGEMANAGERBASE_H


//  INCLUDES
#include <e32base.h>

// CONSTANTS
	//none

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;

// CLASS DECLARATION
/**
*  Forwards Tsy requests from Symbian OS to Domestic OS
*  
*  @lib (unknown)
*  @since 2.6
*/
class CMmMessageManagerBase
        : public CBase
    {

    // public types
    public:

        //TTsyObjects
        enum TTsyObjects
            {
            EPhoneTsy,
            ELineTsy,
            ECallTsy,
            EFaxTsy,
            ESmsMessagingTsy,
            EBroadcastMessagingTsy,
            EUssdMessagingTsy,
            EPhoneStoreTsy,
            ESmsStoreTsy,
            ENamStoreTsy,
            EONStoreTsy,
            EENStoreTsy,
            EPhoneBookStoreTsy,
            EConferenceCallTsy,
            EPacketDataTsy,
            ESatTsy,
            ECustomTsy,
			EDtmfTsy,
            ESecurityTsy,
            //ECustomExtTsy
            EMaxNumOfTsyObjects
            };

    // public methods
    public:

        /**
        * Destructor
        */
        virtual ~CMmMessageManagerBase( void );

        /**
        * Forwards a request to DOS layer, request doesn't contain data
        */
        virtual TInt HandleRequestL( 
                const TInt aIpc ) = 0;

        /**
        * Forwards a request to DOS layer, request contains 
        * packaged data by Common TSY
        */
        virtual TInt HandleRequestL( 
                const TInt aIpc,
                const CMmDataPackage* aDataPackage ) = 0;

        /**
        * Forwards a request to DOS layer, request contains 
        * packaged data by Etel Server.
        */
        virtual TInt HandleRequestL(    
                const TInt aIpc,
                const TDataPackage* aDataPackage ) = 0;

		/**
        * Completes a request or a notification with no data
        */
        virtual void Complete( TInt aIpc, TInt aResult ) = 0;

        /**
        * Completes a request or a notification with packaged data
        */
        virtual void Complete( TInt aIpc, CMmDataPackage* data, TInt aResult ) = 0;

        /**
        * Registers a Tsy object for request and notification completions
        */
        virtual void RegisterTsyObject( TTsyObjects aTsyObjectType, 
                                                 CBase* aTsyObject ) = 0;
                                                 
        /**
        * Deregisters a Tsy object from request and notification completions
        */
        virtual void DeregisterTsyObject(CBase* aTsyObject) = 0;

    // protected methods
    protected:

	// private methods
    private:

    // public attributes
    public:

    // protected attributes
    protected:

    // private attributes
    private:


    };
#endif // CMMMESSAGEMANAGERBASE_H

//  End of file
