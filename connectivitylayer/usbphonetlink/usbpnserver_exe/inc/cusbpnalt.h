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


#ifndef CUSBPNALT_H
#define CUSBPNALT_H

//  INCLUDES
#include <e32base.h>  // For CActive

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MUsbPnAltObserver;
class RDevUsbcClient;
class RIscApi;

// CLASS DECLARATION

/**
*  Listens device state / alternate setting changes.
*/
class CUsbPnAlt : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUsbPnAlt* NewL( MUsbPnAltObserver& aAltObserver, RDevUsbcClient& aLdd, RIscApi& aIscApi );

        /**
        * Destructor.
        */
        ~CUsbPnAlt();

    public: // New functions

        //
        void ListenStateChange();

    protected:  // Functions from base classes
        /**
        * From CActive.
        */
        void DoCancel( );

        /**
        * From CActive.
        */
        void RunL( );

        /**
        * From CActive.
        */
        TInt RunError( TInt );

    private:

        /**
        * C++ default constructor.
        */
        CUsbPnAlt( MUsbPnAltObserver& aObserver, RDevUsbcClient& aLdd, RIscApi& aIscApi );
#ifndef NCP_COMMON_BRIDGE_FAMILY
        /**
        * Sends control message to ISA side media module SOS USB MM.
        * @param const TUint8 aId SubMessageId for control message
        */
        void SendControlMessage( const TUint8 aId ) const;
#endif /* NCP_COMMON_BRIDGE_FAMILY */

    private:    // Data

        // Observer instance
        MUsbPnAltObserver&      iAltObserver;

        // Reference to USB driver channel
        RDevUsbcClient&         iLdd;

        // Reference to ISCAPI handle
        RIscApi&                iIscApi;

        // Member for holding device state
        TUint                   iState;

    };

#endif      // CUSBPNALT_H

// End of File
