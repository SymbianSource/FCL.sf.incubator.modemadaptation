/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _CMMBROADMESSHANDLER_H_
#define _CMMBROADMESSHANDLER_H_

//  INCLUDES
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include "cmmsmsgsmutility.h"

//  CONSTANTS
    //none

//  MACROS
    //none

//  DATA TYPES
    //none


//  EXTERNAL DATA STRUCTURES
    //none


//  FUNCTION PROTOTYPES
    //none


//  FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmPhoNetReceiver;
class CMmMessageRouter;
class TIsiReceiveC;

// CLASS DECLARATION

/**
* CMmBroadMessHandler is used to create and send mode-independent SMS
* ISI-messages to PhoNet via PhoNetSender. It also receives SMS ISI-messages
* from PhoNet via PhoNetReceiver. If the message is mode-independent,
* it will be handled here. Otherwise, the message will be sent to message
* handler of active mode.
*  @lib nokiatsy.lib
*  @since Series60_2.6
*/
class CMmBroadMessHandler : public CBase, public MMmMessHandlerBase, public MMmMessageReceiver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * Creates a new Broadcast MessageHandler object instance.
        * @param aPhoNetSender Pointer to the PhonetSender
        * @param aPhoNetReceiver Pointer to the PhonetReceiver
        * @param aMessageRouter Pointer to the message router object
        * @return Pointer to CMmBroadMessHandler
        */
        static CMmBroadMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter );

        /**
        * Destructor
        * @return: None
        */
        ~CMmBroadMessHandler();

    public: // New functions
        //none

    public: // Functions from base classes

        /**
        * Isimsg received by PhonetReceiver
        * @param aIsiMsg The received Isi message
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );


        /**
        * From CMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param aIpc IPC number of the request
        * @param aDataPackage Pointer to data package
        * @return KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

    private:

        /**
        * Constructor
        * @return: None
        */
        CMmBroadMessHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @return None
        */
        void ConstructL();

        /**
        * Create CB routing request
        * @param aTransId Transaction ID for ISI message
        * @param aDataPackage Packed data
        * @return KErrNone or error code
        */
        TInt SmsCbRoutingRequest( TUint8 aTransId,
            const CMmDataPackage* aDataPackage );

        /**
        * Handle response to CB routing request
        * @param aSmsCbRoutingResp SMS_CB_ROUTING_RESP ISI message
        * @return void
        */
        void SmsCbRoutingResp( const TIsiReceiveC& aSmsCbRoutingResp );

        /**
        * Handle incoming CB message
        * @param aSmsCbRoutingInd SMS_CB_ROUTING_IND ISI message
        * @return void
        */
        void SmsCbRoutingIndL( const TIsiReceiveC& aSmsCbRoutingInd );

    public:     // Data
        // none

    protected:  // Data

        //Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        //Array to store the different WCDMA CBS messages
        CArrayPtrFlat< TWcdmaCbsMsg >* iCbsMsg;

    private:    // Data

        //Pointer to the message router object
        CMmMessageRouter* iMessageRouter;

        //CB Subscription number.
        TUint8 iCbSubscriptionNumber;

    private: // types

        // The broadcast request types are grouped into
        // the following types. Used as ISA transaction IDs.
        enum TBroadcastRequestType
            {
            EBroadcastMessagingUnknown = 0,
            EBroadcastMessagingReceiveMessage,
            EBroadcastMessagingReceiveMessageCancel,
            EBroadcastMessagingSetFilterSetting,
            };

    };

#endif // _CMMBROADMESSHANDLER_H_

//End of file
