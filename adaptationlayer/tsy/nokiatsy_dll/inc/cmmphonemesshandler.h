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



#ifndef CMMPHONEMESSHANDLER_H
#define CMMPHONEMESSHANDLER_H

// INCLUDES
#include <badesca.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <e32base.h>
#include <etelmm.h>
#include <ctsy/serviceapi/mmgsmwcdmautils.h>

#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include "nokiatsy_internal_variation.h"
#include "muiccoperationbase.h"

// CONSTANTS
    //None

// MACROS
    //None

// DATA TYPES
    //None

//  EXTERNAL DATA STRUCTURES
enum TTransIdStates
    {
    EPhoneDeleteEntryTransId = 100,
    EPhoneGetAPNnameTransId = 101,
    EPhoneGetEnumerateAPNEntriesTransId = 102,
    EPhoneReadDataFromSimTransId = 103,
    EPhoneWriteDataTransId = 104,
    EPhoneGetACLStatusTransId = 105,
    EPhoneSetACLStatusTransId = 106
    };

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmMessageRouter;
class TIsiReceiveC;
class CMmSupplServMessHandler;
class CMmUiccMessHandler;

// CLASS DECLARATION

/**
* CMmPhoneMessHandler is used to create and send mode-independent phone
* ISI messages to PhoNet via PhoNetSender. It also receives phone ISI
* messages from PhoNet via PhoNetReceiver. If the message is mode-independent,
* it will be handled here. Otherwise, the message will be sent to message
* handler of active mode.
*/
class CMmPhoneMessHandler:
        public CBase,
        public MMmMessHandlerBase,
        public MMmMessageReceiver,
        public MUiccOperationBase
    {
    public:

        /**
        * Creates a new GSM specific PhoneMessageHandler instance.
        * @param CMmPhoNetSender* aPhoNetSender: Pointer to phonet sender.
        * @param CMmPhoNetReceiver* aPhoNetReceiver: Pointer to phonet receiver.
        * @param CMmMessageRouter* aMessageRouter: Pointer to message router.
        * @param CMmSupplServMessHandler* aSupplServMessHandler: Pointer to
                 supplementary services.
        * @return CMmPhoneMessHandler*: Pointer to message handler.
        */
        static CMmPhoneMessHandler* NewL(
                CMmPhoNetSender* aPhoNetSender,
                CMmPhoNetReceiver* aPhoNetReceiver,
                CMmMessageRouter* aMessageRouter,
                CMmSupplServMessHandler* aSupplServMessHandler,
                CMmUiccMessHandler* aUiccMessHandler);

        /**
        * Destructor.
        */
        ~CMmPhoneMessHandler();

        /**
        * From MMmMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param TInt aIpc: number of the request
        * @param CMmDataPackage* aDataPackage: pointer to data package
        * @return TInt KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * ISI message received by PhonetReceiver, interpret if GSM specific
        * @param const TIsiReceiveC& aIsiMessage: reference to received ISI message
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handle UICC response messages
        * @param aTraId Transaction ID
        * @param aFileData Descriptor for file data
        * @return KErrNone or error code
        */
        TInt ProcessUiccMsg(
            TInt aTraId,
            TInt aStatus,
            const TDesC8& aFileData );

    protected:

        /**
        * Default constructor.
        */
        CMmPhoneMessHandler();

        /**
        * Info serial number read response.
        * @param CIsiMsg* aIsiMsg: Received ISI message
        * @return void
        */
        void InfoSerialNumberReadResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Dynamic flags read request
        * @return KErrNone or error code
        */
        TInt UiccReadDynamicFlagsReq();

        /**
        * Dynamic flags read response
        * @param aStatus Status
        * @param aFileData File data
        * @return None
        */
        void UiccReadDynamicFlagsResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Dynamic flags write request
        * @param aInfo Dynamic flags data
        * @return KErrNone or error code
        */
        TInt UiccWriteDynamicFlagsReq( TUint8 aInfo );

        /**
        * Dynamic flags write response
        * @param aStatus Status
        * @return None
        */
        void UiccWriteDynamicFlagsResp( TInt aStatus );


        /**
        * CSP file request
        * @return KErrNone or error code
        */
        TInt UiccCspFileReq();

        /**
        * CSP file response
        * @param aStatus Status
        * @param aFileData File data
        * @return None
        */
        void UiccCspFileResp( TInt aStatus, const TDesC8& aFileData );

        /**
        * Class attributes are created in ConstructL
        * @param CMmMessageRouter* aMessageRouter: pointer to message
        *  router object
        * @return void : None
        */
        void ConstructL( CMmMessageRouter* aMessageRouter );

    private:

        /**
        * For nokiatsy internal refresh handling
        */
        enum
            {
            KRefreshSms = 0x01,
            KRefreshCallForwardingFlags = 0x02,
            KRefreshOplRules = 0x04,
            KRefreshPnnRecord = 0x08,
            KRefreshOnsName = 0x10
            };

        /**
        * Read IMSI from UICC
        * @return Error code
        */
        TInt UiccImsiReq();

        /**
        * Response to IMSI read request.
        * @param aStatus Status of response message
        * @param aFileData Data from SIM file
        * @return void
        */
        void UiccImsiResp( TInt aStatus, const TDesC8& aFileData );

        /**
        * Request to read Service Provider Name from UICC.
        * @return Symbian error code
        */
        TInt UiccReadServiceProviderName();

        /**
        * Response for request to read Service provider name from UICC.
        * @param aStatus Status of response message
        * @param aFileData Data from SIM file
        * @return void
        */
        void UiccReadServiceProviderNameResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Subscribes responses and indications from PhoNet.
        * @return TInt : success/failure value
        */
        TInt SubscribeEventsFromPhoNet();

        /**
        * Creates InfoSerialNumberReadReq ISI message and sends it to Phonet.
        * @param TUint8 aTransactionId: unique TransactionId number
        * @param TUint8 aTarget: request data
        * @return TInt : success/failure value
        */
        TInt InfoSerialNumberReadReq(
            TUint8 aTransactionId,
            TUint8 aTarget );

        /**
         * Process service type checking for service provider name query
         * @param aFileData File data read from UICC
         * @return Error code
         */
        TInt UiccProcessServiceTypeCheck( const TDesC8& aFileData );

        /**
         * Process service provider name info
         * @param aStatus UICC server status
         * @param aFileData File data read from UICC
         */
        void UiccProcessSpnNameInfo(
            TInt aStatus,
            const TDesC8& aFileData );

    //ATTRIBUTES
    public:
        //none

    protected:

        //Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        //Pointer to the Message Router
        CMmMessageRouter* iMessageRouter;

        //Pointer to the Supplementary Service
        CMmSupplServMessHandler* iSupplServMessHandler;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

        // This flag indicates if re-reading of cached data
        // is going on in commontsy.
        TBool iCommonTSYRefreshPending;

    private:

        // Location index for SimPnReadReq
        TUint16 iLocIndex;

        // SIM Service Table, used to temporarily save data for completion
        RMobilePhone::TMobilePhoneServiceTableV1 iSSTFile;

        // USIM Service Table, used to temporarily save data for completion
        RMobilePhone::TMobilePhoneServiceTableV1 iUSTFile;

        //value of the second display condition byte
        TUint8 iSecondDisplayCondition;

        // Service table
        SSSTFileData* iSSTFileData;

        // Phone Store Info
        SStoreInfo* iStoreInfo;

        //Service provider name
        RMobilePhone::TMobilePhoneServiceProviderNameV2 iServiceProviderName;

        //Keeps track state of ACL enquiry
        TBool iACLIsProgress;

        // Keeps track of APN list
        CDesC8ArrayFlat* iAPNList;

        //Keeps track of APN data.
        RMobilePhone::TAPNEntryV3 iAPNDataBuffer;

        // Keeps track of ACL index to read or delete
        TUint iAPNReadOrDeleteIndex;

        //Current ALS line used
        RMobilePhone::TMobilePhoneALSLine iAlsLine;

        // Keeps track on nokiatsy internal refresh
        TUint8 iInternalRefreshFiles;

        // Keeps track if any errors happened during re-reading of cached files
        TBool iRefreshError;
    };

#endif // CMMPHONEMESSHANDLER_H

//  End of file

