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



//  INCLUDES

#ifndef _INC_CMMUSSDMESSHANDLER_
#define _INC_CMMUSSDMESSHANDLER_

#include <e32def.h>

#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include "cmmphonetsender.h"

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

class CMmMessageRouter;
class CMmPhoNetReceiver;
class CMmPhoNetSender;
class CMmUssdTsy;

//  CLASS DEFINITIONS

// DESCRIPTION
//
// CMmUssdMessHandler is used to create and send mode-independent Ussd
// ISI-messages to PhoNet via PhoNetSender. It also receives Ussd ISI-messages
// from PhoNet via PhoNetReceiver. If the message is mode-independent,
// it will be handled here. Otherwise, the message will be sent to message
// handler of active mode.

class CMmUssdMessHandler :
    public CBase, public MMmMessHandlerBase, public MMmMessageReceiver
{
    //METHODS
    public:

        /**
        * C++ destructor
        * @param none
        * @return none
        */
        ~CMmUssdMessHandler();

        /**
        * Two-phased constructor.
        * @param aPhoNetSender Pointer to the phonet sender
        * @param aPhoNetReceiver Pointer to the phonet receiver
        * @param aMessageRouter Pointer to the message router
        */
        static CMmUssdMessHandler* NewL(
            CMmPhoNetSender* aCMmPhoNetSender,
            CMmPhoNetReceiver* aCMmPhoNetReceiver,
            CMmMessageRouter* aMessageRouter );

        /**
        * From CMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @since ?Series60_version
        * @param aIpc IPC number of the request
        * @return TInt KErrNone or error code
        */
        TInt  ExtFuncL( TInt aIpc,
            const CMmDataPackage* aDataPackage );

    public: // New functions

        /**
        * Handles a received message by calling the specific
        * message handling method.
        * @param aIsiMessage, reference to the received message.
        * @return TInt: True if message been handled in this message handler
        */
        void ReceiveMessageL( const TIsiReceiveC &aIsiMessage );

        /**
        * Construct a SS_GSM_USSD_SEND_REQ ISI message
        * @param TUint8 aTransactionId, transaction id number
        * @param TDes8* aData, message data
        * @param TDes8* aAttributes, message attributes
        * @return TInt KErrNone or error code
        */
        TInt SsGsmUssdSendReq(
            const CMmDataPackage* aDataPackage );

        /**
        * Construct a SS_GSM_USSD_SEND_REQ ISI message
        * @return TInt KErrNone or error code
        */
        TInt SsGsmUssdSendReq( void );

        /**
        * Completes successful Ss Gsm Ussd Send Request
        * @param aIsiMessage
        */
        void SsGsmUssdSendResp( const TIsiReceiveC &aIsiMessage );

        /*
        * Completes unsuccessful Ss Gsm Ussd Send Request
        * @param  const TIsiReceiveC &aIsiMessage , the received ISI message
        */
        void SsServiceFailedResp( const TIsiReceiveC &aIsiMessage );

        /**
        * breaks a SS_GSM_USSD_RECEIVE_IND ISI message
        * @param aIsiMessage
        */
        void SsGsmUssdReceiveInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param const TIsiReceiveC &aIsiMessage: The received ISI message
        */
        void HandleError(const TIsiReceiveC &aIsiMessage, TInt aError);

    protected:

        /**
        * C++ constructor
        * @param none
        * @return none
        */
        CMmUssdMessHandler();

    private:

        /**
        * Initialises object attributes.
        */
        void ConstructL();

    //ATTRIBUTES
    public:
        //none

    protected:

        // Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        // pointer to the message router
        CMmMessageRouter* iMessageRouter;


    private:

        TBool iIsSendReleaseCalled;
        TBool iIsUssdSendReqOnGoing;
        TBool iIsSendReleaseReqPending;

        // Etrue if NoFdn request is in active
        TBool iNoFdnUSSDReq;

};

#endif // CMMUSSDMESSHANDLER

//End Of File
