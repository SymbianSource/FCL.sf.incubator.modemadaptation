/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LICENSEETSY_H
#define LICENSEETSY_H


//  INCLUDES
#include <ctsy/mltsyfactory.h>

// CONSTANTS
    //none

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
   // none

// CLASS DECLARATION

/**
*  This class is used to create a new CNokiaTSYFactory object and interfaces 
*  to get pointers to MMessageRouter and CTsySatMessagingBase.
*  Deleting and NULL setting for CTsySatMessagingBase is done by Common TSY !!!
*
*  @return reference to created MLtsyFactoryBase type of object
*
*  @lib nokiatsyfactory
*  @since Series 60 5.0
*/

class CNokiaTSYFactory : public MLtsyFactoryV1
    {

    // public methods
    public: // Constructors and destructor

        /**
        * Default constructor
        */
        CNokiaTSYFactory();

        /**
        * Destructor
        */
        ~CNokiaTSYFactory();

    public: // Functions from base classes

    /**
       * For getting interface version used in LTSY
       *
       * @since S60 v5.0
         * @return TCtsyInterfaceVersion version
       */

    virtual TCtsyInterfaceVersion Version();

    /**
     * For releasing created LTSY object
     *
     * @since S60 v5.0
     * @return none
     */

    virtual void Release( );

    /**
     * For getting MMessageRouter from Nokia TSY
     *
     * @since S60 v5.0
     * @param aMessageManager refenrence to MmMessageManagerCallback
     * @return pointer to LTSY MMessageRouter object
     */

    virtual MMessageRouter* GetMessageRouter( 
        MmMessageManagerCallback& aMessageManager );

    /**
     * For getting CTsySatMessagingBase from SIM ATK TSY
     *
     * This is deleted and set NULL by Common TSY !!!
     *
     * @since S60 v5.0
     * @return pointer to SIM ATK TSY CTsySatMessagingBase object
     */

    virtual CTsySatMessagingBase* GetSatMessagingService( );
    
   /**
    * For getting CTsySatMessagingBase from SIM ATK TSY
    *
    * This is deleted and set NULL by Common TSY !!!
    *
    * @since S60 v5.0
    * @param aSatMessagingService refenrence to MCtsySatService
    * @return pointer to SIM ATK TSY CTsySatMessagingBase object
    */

    virtual CTsySatMessagingBase* GetSatMessagingService( 
        MCtsySatService& aSatMessagingService );

    // Created message router.
    CMmMessageRouter* iMMessageRouter;

    // Pointer to SAT Messaging
    CTsySatMessagingBase* iSatMessaging;
    };

#endif // LICENSEETSY_H

//  End of file
