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


#ifndef RUSBPNCLIENT_H
#define RUSBPNCLIENT_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  Client API for USB Phonet Link server.
*
*  @lib usbpnclient.lib
*  @since
*/
class RUsbPnClient : public RSessionBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C RUsbPnClient();

    public: // New functions

        /**
        * Connect to USB Phonet Link server.
        * Server starts if first.
        * @since
        * @param
        * @return
        */
        IMPORT_C void ConnectL();

        /**
        * Sends message to server so it can release interface before class controller Stop() can continue.
        * Closes also the handle so rest of the server is destroyed if last reference.
        * @since
        * @param
        * @return
        */
        IMPORT_C void Detach();


    };

#endif      // RUSBPNCLIENT_H

// End of File
