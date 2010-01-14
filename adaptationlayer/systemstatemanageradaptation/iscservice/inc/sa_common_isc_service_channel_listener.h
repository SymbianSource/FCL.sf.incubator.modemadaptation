/*
* ==============================================================================
*  Name:           sa_common_isc_service_channel_listener.h
*  Part of:        System adaptation
*  Description:    Declaration of CSACommonIscServiceChannelListener class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:59:19 2009 %
*
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
* Declaration of CSACommonIscServiceChannelListener class
*
*/


#ifndef SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_H
#define SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MSAIscService;

// CLASS DECLARATION

/**
*  CSACommonIscServiceChannelListener
*
*/
class CSACommonIscServiceChannelListener : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aAdaptation           Reference to Adaptation
        * @param aIscApi               Reference to ISC channel
        * @return                      Pointer to created object
        */
        static CSACommonIscServiceChannelListener* NewL(
            MSAIscService& aAdaptation,
            RIscApi& aIscApi );
        
        /**
        * Destructor.
        */
        virtual ~CSACommonIscServiceChannelListener();


    private:

        /**
        * From CActive class
        * This function receives messages via ISC channel
        * @return                      none
        */
        void RunL();

        /**
        * From CActive class
        * This function is called if RunL leaves
        * @param aError                Leave cause
        * @return                      KErrNone
        */
        TInt RunError( TInt aError );

        /**
        * From CActive class
        * This function is called from CActive Cancel() method
        * @return                      none
        */
        void DoCancel();

        /**
        * C++ default constructor.
        * @param aAdaptation           Reference to Adaptation
        * @param aIscApi               Reference to ISC channel
        */
        CSACommonIscServiceChannelListener(
            MSAIscService& aAdaptation,
            RIscApi& aIscApi );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // Data

        // Reference to Adaptation
        MSAIscService& iAdaptation;

        // Reference to monitored ISC channel
        RIscApi& iIscApi;

        // Connection status to monitored ISC channel
        TInt iConnectionStatus;

    };

#endif      // SA_COMMON_ISC_SERVICE_CHANNEL_LISTENER_H

// End of File
