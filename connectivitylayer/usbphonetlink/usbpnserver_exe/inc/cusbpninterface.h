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


#ifndef USBPNINTERFACE_H
#define USBPNINTERFACE_H

//  INCLUDES
#include <e32base.h>    // For CBase
#include <iscapi.h>     // For RIscApi
#include <d32usbc.h>    // For RDevUsbcClient

#include "musbpnaltobserver.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MUsbPnControlObserver;
class CUsbPnAlt;
class CUsbPnIsaReceiver;
class CUsbPnUsbReceiver;

// CLASS DECLARATION

class CUsbPnInterface : public CBase, public MUsbPnAltObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUsbPnInterface* NewL();

        /**
        * Destructor.
        */
        ~CUsbPnInterface();


    public: // Functions from base classes

        /**
        * Handles device state notifications. Device state or alternate setting change.
        * @param  TUint aDeviceState USB device's state
        */
        void HandleDeviceStateChange( TUint aDeviceState );


    private:

        /**
        * C++ default constructor.
        */
        CUsbPnInterface();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Initialises Phonet interfaces
        */
        void InitialiseInterfaceL();

        /**
        * Releases Phonet interfaces in opposite order.
        */
        void ReleaseInterface();

        /**
        * Activates data transfer mode when USB host selects alternate setting.
        * @since Series ?XX ?SeriesXX_version
        * @param TUint aAltSetNumber. 0 for altset zero. 1 for data transfer mode.
        */
        void SetAltSetting( TUint aAltSetNumber );


    private:    // Data

        // Ownership. Alternate setting listening active object
        CUsbPnAlt*              iAlt;

        // Ownership. Isa receiver active object
        CUsbPnIsaReceiver*      iIsaReceiver;

        // Ownership. USB receiver active object
        CUsbPnUsbReceiver*      iUsbReceiver;

        // Ownership.
        RDevUsbcClient          iCommLdd;

        // Ownership.
        RDevUsbcClient          iPnDataLdd;

        // Ownership.
        RIscApi                 iIscApi;

        // Boolean flag for succesful initialisation
        TBool                   iInitialised;

    };

#endif      // USBPNINTERFACE_H

// End of File
