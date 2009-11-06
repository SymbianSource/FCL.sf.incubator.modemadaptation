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


#ifndef USBPNCLASSCONTROLLER_H
#define USBPNCLASSCONTROLLER_H

//  INCLUDES
#include <cusbclasscontrollerplugin.h>  // For CUsbClassControllerPlugin

#include "rusbpnclient.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



/**
*  CUsbPNClassController
*  This class implements the Usb Isa class controller used to control the usb media.
*  See Lotus Notes:
*  TSW Docman / Core Components Factory / USB PhoNet Link / Design
*  ID:
*/
class CUsbPNClassController : public CUsbClassControllerPlugIn
    {
    public: // New functions.
        /**
        * Constructs a CUsbIsaClassController object.
        * @param MUsbClassControllerNotify& aOwner  USB Device that owns and manages the class
        * @return static CUsbIsaClassController*    A new CUsbIsaClassController object
        */
        static CUsbPNClassController* NewL(MUsbClassControllerNotify& aOwner);

        /**
        * Standard active object RunL.
        * @return void
        */
        void RunL();

        /**
        * Standard active object cancellation function.
        * @return void
        */
        void DoCancel();

        /**
        * Standard active object error-handling function.
        * @param TInt aError    Not used.
        * @return TInt
        */
        TInt RunError(TInt aError);

        /**
        * Destructor.
        */
        ~CUsbPNClassController();

        /**
        * Called by UsbMan to start this class.
        * @param TRequestStatus& aStatus    Will be completed with success or failure.
        * @return void
        */
        void Start(TRequestStatus& aStatus);

        /**
        * Called by UsbMan to stop this class.
        * @param TRequestStatus& aStatus    Will be completed with success or failure.
        * @return void
        */
        void Stop(TRequestStatus& aStatus);

        /**
        * Returns information about the interfaces supported by this class.
        * @param TUsbDescriptor& aDescriptorInfo    Will be filled in with interface information.
        * @return void
        */
        void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

    private:
        /**
        * Constructor.
        * @param MUsbClassControllerNotify& aOwner  USB Device that owns and manages the class
        */
        CUsbPNClassController(MUsbClassControllerNotify& aOwner);

        // Handle to server
        RUsbPnClient    iClient;

    };


#endif // #ifndef USBPNCLASSCONTROLLER_H
