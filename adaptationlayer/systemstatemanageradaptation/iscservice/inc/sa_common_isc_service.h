/*
* ==============================================================================
*  Name:           sa_common_isc_service.h
*  Part of:        System adaptation
*  Description:    Declaration of CSACommonIscService class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:59:16 2009 %
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
* Declaration of CSACommonIscService class
*
*/


#ifndef SA_COMMON_ISC_SERVICE_H
#define SA_COMMON_ISC_SERVICE_H

//  INCLUDES
#include <e32base.h>
#include <iscapi.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSACommonIscServiceChannelListener;
class MSAIscService;

// CLASS DECLARATION

/**
*  CSACommonIscService
*  This class declarates System Adaptation plug-in module, which provides
*  ISC interface for other System Adaptation plug-in modules.
*
*/
class CSACommonIscService : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSACommonIscService* NewL( MSAIscService& aAdaptation,
            const TUint16 aChannelNumber,
            TDes8* aEvents,
            const TUint16 aClientISIMessageSize );

        /**
        * Destructor.
        */
        virtual ~CSACommonIscService();

    public: // Functions from base classes

        /**
        * This function is used to send messages via ISC channel
        * @param aMessage              Reference to message
        * @return                      none
        */
        void SendMessageL( TDesC8& aMessage );

    private:

        /**
        * C++ default constructor.
        */
        CSACommonIscService( MSAIscService& aAdaptation,
            const TUint16 aChannelNumber,
            const TUint16 aClientISIMessageSize );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TDes8* aEvents );

        /**
        * From CActive
        * This function receives messages from ISC API
        * @return                      none
        */
        void RunL();

        /**
        * From CActive
        * This function is called by Kernel when an outstading request is
        * canceled with Cancel() function
        * @return                      none
        */
        void DoCancel();

        /**
        * This function is used to subscribe reception of events via ISC API
        * @param aEvents               Pointer to list of subscribed events
        * @return                      none
        */
        void SubscribeEventsL( TDes8 *aEvents );

        /**
        * This function starts receiving messages via ISC channel
        * @param aMessageLength        Length of message buffer required for
        *                              received message
        * @return                      none
        */
        void ReceiveMessageL( TUint16 aMessageLength );

    private:    // Data

        // Reference to Adaptation
        MSAIscService& iAdaptation;

        // ISC API interface object
        RIscApi iIscApi;

        // Pointer to ISC Channel lister object
        CSACommonIscServiceChannelListener* iSACommonIscServiceChannelListener;

        // Used ISC channel
        TUint16 iChannelNumber;

        // Min size of ISI message
        TUint16 iClientISIMessageSize;

        // Message data length of pending message 
        TUint16 iMessageLength;

        // Pointer to buffer descritor where a message data is received from ISC API
        HBufC8* iBuffer;

        // Modifiable pointer to message data buffer
        TPtr8 iBufferPtr;
    };

#endif      // SA_COMMON_ISC_SERVICE_H

// End of File
