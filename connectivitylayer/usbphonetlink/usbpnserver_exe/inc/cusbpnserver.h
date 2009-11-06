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


#ifndef CUSBPNSERVER_H
#define CUSBPNSERVER_H

//  INCLUDES
#include <e32base.h>            // For CServer2

#include "usbpndefinitions.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CUsbPnInterface;

// CLASS DECLARATION

/**
*  Server class
*/
class CUsbPnServer : public CServer2
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUsbPnServer* NewL();

        /**
        * Destructor.
        */
        ~CUsbPnServer();

    public: // New functions

        /**
        * Session will inform the server that the session has been opened
        * @return                     None
        */
        void SessionOpened();

        /**
        * Session will inform the server that the session has been closed
        * @return                     None
        */
        void SessionClosed();

        /**
        * Deletes interface so it's safe for class controller's Stop() to return
        * @return                     None
        */
        void Detach();

    public: // Functions from base classes

        /**
        * From CServer2. This function creates new server side session
        * @param aVersion              Requested server version
        * @param aMessage
        * @return                      Pointer to created server side session
        */
        CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;


    private:

        /**
        * C++ default constructor.
        */
        CUsbPnServer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data

        // Ownership. Instance of main class which handles events.
        CUsbPnInterface*    iInterface;

        // The count of sessions
        TInt                iSessionCount;

    };

#endif      // CUSBPNSERVER_H

// End of File
