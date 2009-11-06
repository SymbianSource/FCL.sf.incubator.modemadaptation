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


#ifndef CUSBPNSCHEDULER_H
#define CUSBPNSCHEDULER_H

//  INCLUDES
#include <e32base.h>    // For CActiveScheduler

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CUsbPnServer;

// CLASS DECLARATION

/**
*  Server's scheduler.
*/
class CUsbPnScheduler : public CActiveScheduler
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        ~CUsbPnScheduler();

    public: // New functions

        /**
        * Starts USB Phonet Link server thread and the server itself.
        * @since Series ?XX ?SeriesXX_version
        * @param
        * @return Starting status
        */
        IMPORT_C static TInt ThreadStart();


    private:

        /**
        * This function creates the active scheduler for USB Phonet Link server
        * thread and USB Phonet Link server and starts them.
        * @return                      None
        */
        static void ConstructL();


    private:    // Data

        // Reserved pointer for future extension
        CUsbPnServer* iServer;

    };

#endif      // CUSBPNSCHEDULER_H

// End of File
