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


// INCLUDES
#ifndef _CMMENSTOREMESSHANDLER_H_
#define _CMMENSTOREMESSHANDLER_H_

#include <e32def.h>
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>
#include "mmmmesshandlerbase.h"
#include "cmmphonetsender.h"
#include "cmmphonetreceiver.h"

//UICC
#include "muiccoperationbase.h"
#include "cmmuiccmesshandler.h"

// CONSTANTS
    // None

// MACROS
    // None

// DATA TYPES
    // None

// EXTERNAL DATA STRUCTURES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmPhoNetReceiver;
class CMmENStoreTsy;
class CMmENStoreExtInterface;
class TIsiReceiveC;
class CMmMessageRouter;
class CMmUiccMessHandler;

// CLASS DECLARATION

/**
* CMmENStoreMessHandler is used to create and send mode-independent
* ISI-messages to PhoNet via PhoNetSender. It also receives ISI-messages
* from PhoNet via PhoNetReceiver. If the message is mode-independent,
* it will be handled here. Otherwise, the message will be sent to message
* handler of active mode.
*/
class CMmENStoreMessHandler
        : public CBase,
        public MMmMessHandlerBase,
        public MUiccOperationBase
    {
    public:

        /**
        * Destructor
        */
        ~CMmENStoreMessHandler();

        /**
        * Creates a new EN store message handler object
        * @param CMmPhoNetSender*: pointer to phonet sender
        * @param CMmPhoNetReceiver*: pointer to phonet receiver
        * @param CMmMessageRouter*: pointer to message router
        */
        static CMmENStoreMessHandler* NewL(
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler );

        /**
        * Receives ISI messages from phonet receiver
        * @param const TIsiReceiveC: SIM ISI message
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @param TInt aError: Error code
        */
        void HandleError(
            const TIsiReceiveC& aIsiMessage,
            TInt aError );

        /**
        * From CMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param TInt: aIpc IPC number of the request
        * @param CMmDataPackage*: aDataPackage
        * @return TInt: KErrNone or error code
        */
        TInt ExtFuncL(
            TInt aIpc,
            const CMmDataPackage* aDataPackage );

        /**
        * Handles a received message by calling the specific
        * message handling method.
        *
        * @param Transaction id
        * @param Status
        * @param aFileData reference to Data received in message
        * @return KErrNone or error code
        */
        TInt ProcessUiccMsg(
                TInt aTransactionId,
                TInt aStatus,
                TUint8 aDetails,
                const TDesC8 &aFileData );

    protected:
        //None

    private:

        /**
        * Constructor
        */
        CMmENStoreMessHandler();

        /**
        * Initialises object attributes.
        */
        void ConstructL();

        /**
        * Handles EFecc read responses
        * @param TInt aStatus: status returned by UICC server
        * @param const TDesC8& aFileData: data from EFecc file
        * returned by UICC server
        * @return TInt: success or failure
        */
        TInt UiccReadEmergCodesResp(
                TInt aStatus,
                const TDesC8& aFileData );

        /**
        * Provide UICC application file id
        * @param const TDesC8&: reference to data
        * @param TDes8&: reference to buffer where digits are stored
        */
        void GetEmergencyNumberDigits(
                const TDesC8& aReadIn,
                TDes8& aNumberBuf );

        /**
        * Handles EFecc file info
        * @param TInt aStatus: status returned by UICC server
        * @param const TDesC8& aFileData: data from EFecc file
        * returned by UICC server
        * @return TInt: success or failure
        */
        TInt CMmENStoreMessHandler::UiccENStoreGetInfoResp(
                TInt aStatus,
                const TDesC8& aFileData );

        /**
        * Create EFecc read request
        * @param TInt: location to be read
        * @return TInt: succes or failure
        */
        TInt UiccReadEmergCodesReq(
                TInt aIndex,
                TUiccTrId traId );

        /**
        * Create EFecc file info request
        * @param TInt: transaction id
        * @return TInt: succes or failure
        */
        TInt ReadEfEccFileInfo( TUiccTrId aTraId );

    // ATTRIBUTES
    public:

    protected:

        // Pointer to the Phonet sender
        CMmPhoNetSender* iPhoNetSender;

    private:

        //Pointer to the Message router
        CMmMessageRouter* iMessageRouter;

        // Pointer to the UICC Messhandler
        CMmUiccMessHandler* iMmUiccMessHandler;

        // ECC entry location index
        TInt iReadECCLocation;

        // Array for emergency number storing
        CArrayPtrFlat<TENStoreResponse>* iEnNumberArray;

        // IPC value of the last request
        TInt iCurrentIpc;

        // Count of used entries
        TInt iUsedEntries;

        // Count of entries left to be read
        TInt iEntryCount;

        // record number to be read
        TInt iRecordNbr;
    };

#endif // _CMMENSTOREMESSHANDLER_H_

// End of File

