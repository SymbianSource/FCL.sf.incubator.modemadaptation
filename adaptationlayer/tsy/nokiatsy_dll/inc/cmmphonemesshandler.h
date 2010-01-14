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
#include "cmmuiccmesshandler.h"

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
            TUint8 aDetails,
            const TDesC8& aFileData );

        /**
        * Checks if "Refresh Done" will be sent to UICC
        * @param TInt aError : Error value during re-reading
        */
        void SmsCachingCompleted( TInt aError );

        /**
        * Checks if "Refresh Done" will be sent to UICC
        * @param TInt aError : Error value during re-reading
        */
        void ServiceTableCachingCompleted( TInt aError );

        /**
        * Checks if "Refresh Done" will be sent to UICC
        * @param TUint aError : Error value during re-reading
        */
        void CallForwFlagsCachingCompleted( TUint aError );

        /**
        * Checks if "Refresh Done" will be sent to UICC
        * @param aError : Error value during re-reading
        * @return refresh ongoing, True or False.
        */
        TBool OplRulesCachingCompleted( TInt aError );

        /**
        * Checks if "Refresh Done" will be sent to UICC
        * @param aError : Error value during re-reading
        */
        void PnnRecordCachingCompleted( TUint8 aError );

        /**
        * Checks if "Refresh Done" will be sent to UICC
        * @param aError : Error value during re-reading
        */
        void OnsNameCachingCompleted( TUint8 aError );

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

        /**
        * Read request for EFest
        * @return KErrNone or error code
        */
        TInt UiccReadEfEstReq();

        /**
        * Handles response for EFest reading in case of getting
        * ACL status       
        * @param aStatus status of the operation
        * @param aFileData content of the EFest
        * @return none
        */
        void UiccGetAclStatusReadEfEstResp( 
            TInt aStatus, 
            const TDesC8& aFileData );

        /**
        * Handles response for EFest reading in case of setting
        * ACL status       
        * @param aStatus status of the operation
        * @param aFileData content of the EFest
        * @return none
        */
        void UiccSetAclStatusReadEfEstResp( 
            TInt aStatus, 
            const TDesC8& aFileData );

        /**
        * Write request for EFest
        * @param aOldAclState old ACL state
        * @return KErrNone or error code
        */
        TInt UiccSetAclStatusWriteEfEstReq( TUint8 aOldAclState );

        /**
        * Handles response for EFest writing in case of setting
        * ACL status       
        * @param aStatus status of the operation
        * @return none
        */
        void UiccSetAclStatusWriteEfEstResp(
            TInt aStatus );

        /**
        * Read request for EFacl
        * @return KErrNone or error code
        */
        TInt UiccReadAclReq();

        /**
        * Handles response for EFacl reading 
        * @param aStatus status of the operation
        * @param aFileData content of the EFacl
        * @return none
        */
        void UiccReadAclResp(
            TInt aStatus, 
            const TDesC8& aFileData );

        /**
        * Encrypt from ACL to CDesC8ArrayFlat.
        * @param const TDesC8& aTlv
        * @param TInt aTotalEntries
        * @return CDesC8ArrayFlat*
        */
        CDesC8ArrayFlat* DecodeACL(
            const TDesC8& aTlv,
            TInt aTotalEntries) const;

        /**
        * Sends number of ACL index to the client.
        * @return none
        */
        void CompleteEnumerateAPNEntries();

        /**
        * Completes APN entry reading
        * @param TUint8 aIndex
        * @return none
        */
        void CompleteGetAPNName( TUint8 aIndex );

        /**
        * Deletes APN entry from internal cache and writes
        * the change to EFacl
        * @param TUint8 aIndex index to be deleted
        * @return KErrNone or error code
        */
        TInt UiccDeleteApnEntry( TInt aIndex );

        /**
        * Writes APN entries from internal cache to the EFAcl
        * @param TUint8 aIndex index to be deleted
        * @return KErrNone or error code
        */
        TInt UiccWriteEfAclReq();

        /**
        * Handles response for EFAcl writing
        * @param aStatus status of the operation
        * @return none
        */
        void UiccWriteEfAclResp( TInt aStatus );

        /**
        * Calculate total length of ACL.
        * @param CDesC8ArrayFlat* aApnList
        * @return TUint16 Total legtn of ACL.
        */
        TUint16 ACLLength( CDesC8ArrayFlat* aApnList ) const;

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
            KRefreshOnsName = 0x10,
            KRefreshServiceTable = 0x20
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

        /**
         * Handle UICC_REFRESH_IND ISI message
         * @param aIsiMessage ISI message
         * @return void
         */
        void UiccRefreshInd( const TIsiReceiveC &aIsiMessage );

        /**
         * Handle UICC_REFRESH_RESP ISI message
         * @param aIsiMessage ISI message
         * @return void
         */
        void UiccRefreshResp( const TIsiReceiveC &aIsiMessage );

        /**
         * Create UICC_REFRESH_REQ ISI message
         * @param aStatus Status
         * @return Symbian error code
         */
        TInt UiccRefreshReq( TUint8 aStatus );

       /** Complete rerfesh handling
        * @param aIsiMessage ISI message
        * @return void
        */
       void HandleUiccRefresh( const TIsiReceiveC &aIsiMessage );


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

        // Flag for refresh completing
        TBool iCompleteRefresfDone;

        // Stores temporarily ACL status going to be set
        RMobilePhone::TAPNControlListServiceStatus iAclStatus;

        TInt iOngoingAclIpc;
    };

#endif // CMMPHONEMESSHANDLER_H

//  End of file

