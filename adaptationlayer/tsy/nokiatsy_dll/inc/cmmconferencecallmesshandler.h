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



#ifndef CMMCONFERENCECALLMESSHANDLER_H
#define CMMCONFERENCECALLMESSHANDLER_H

// INCLUDES
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"

#include <e32base.h>

// CONSTANTS
// hard-coded transaction id for ISI messages
const TUint8 KConferenceCallTransId = 2;

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmMessageRouter;

// CLASS DECLARATION

/**
* Used for creating and sending conference call ISI messages to
* PhoNet via PhoNetSender. It also receives conference call
* ISI messages from PhoNetReceiver.
*/
class CMmConferenceCallMessHandler :
    public CBase,
    public MMmMessHandlerBase,
    public MMmMessageReceiver
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPhoNetSender pointer to the Phonet sender
        * @param aPhoNetReceiver pointer to the Phonet receiver
        * @param aMessageRouter pointer to the message router object
        * @return created message handler object
        */
        static CMmConferenceCallMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter );

        /**
        * Destructor.
        */
        ~CMmConferenceCallMessHandler();

    public: // Methods from base classes

        /**
        * From MMmMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param aIpc IPC number of the request
        * @param CMmDataPackage* aDataPackage: pointer to datapackage
        * @return KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

    public: // New functions

        /**
        * Checks if the received message should be handled in the common
        * Conference Call Message handler.
        * @param aIsiMsg, reference to the received message.
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMsg );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param aIsiMsg The received ISI message
        * @param aError: Error code
        */
        void HandleError( const TIsiReceiveC& aIsiMsg, TInt aError );

    protected: // New functions

        /**
        * C++ default constructor.
        */
        CMmConferenceCallMessHandler();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Creates Call control request ( create conference call etc. ) ISI
        * message and sends it to Phonet.
        * @param aTransactionId: unique TransactionId number
        * @param aCallId: call ID (sometimes common CC Call ID)
        * @param aOperation: control operation
        * @param aIsCreateConference: is this CreateConference request
        * @return success/failure value
        */
        TInt CallControlReq(
            TUint8 aTransactionId,
            TUint8 aCallId,
            TUint8 aOperation,
            TBool aIsCreateConference );

        /**
        * Breaks received CallControlResp ISI message.
        * @param aIsiMsg, reference to the received message.
        */
        void CallControlResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Breaks received CallControlInd ISI message.
        * @param aIsiMsg, reference to the received message.
        */
        void CallControlInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Creates CallReleaseReq (HangUp conference call) ISI
        * message and sends it to Phonet.
        * @param aTransactionId, unique transaction id
        * @param aCallId, Call ID of this call (Conference Call ID)
        * @param aCause, cause of releasing
        * @return success/failure value
        */
        TInt CallReleaseReq(
            TUint8 aTransactionId,
            TUint8 aCallId,
            TUint8 aCause );

        /**
        * Breaks received CallReleaseResp ISI message.
        * @param aIsiMsg, reference to the received message.
        */
        void CallReleaseResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Utility function that maps an ETel call id to a call id
        * in ISA Call Server's format.
        * @param aETelCallId call Id in ETel format
        * @return call Id in ISA format, or KCallIdNone if illegal argument.
        */
        static TUint8 MapETelCallIdToISACallId( const TInt aETelCallId );

    public: // Member data
        // None

    protected: // Member data
        // None

    private: // Member data

        // Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        // Last release cause value
        TUint8 iReleaseCauseValueSent;

        // Last transaction id for conference call creation
        TBool iIsCreateConference;

        // Last conference call control operation
        TUint8 iLastOperation;

        // Pointer to the CMmMessageRouter
        CMmMessageRouter* iMessageRouter;

    };

#endif // CMMCONFERENCECALLMESSHANDLER_H

// End of File
