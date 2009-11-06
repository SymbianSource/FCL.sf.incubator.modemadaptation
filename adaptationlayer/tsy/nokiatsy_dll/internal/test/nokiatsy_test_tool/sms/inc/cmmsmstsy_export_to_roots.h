/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   SMSTSY
*
*/



#ifndef CMMSMSTSY_H
#define CMMSMSTSY_H

// INCLUDES
#include <etelmm.h>
#include <et_phone.h>
#include "ctsy/serviceapi/mctsysatservice.h"

//  FORWARD DECLARATIONS
class CMmSmsStorageTsy;
class CMmPhoneTsy;
class CMessStore;
class CMmSmsExtInterface;
class CMmSmsStorageTsy;
class CMmDataPackage;
class CSmsSendRequest;
class CListReadAllAttempt;
class CMmTsyReqHandleStore;
struct TSmsMsg;
struct TSmsParameters;

// CLASS DECLARATION

/**
*  CMmSmsTsy contains extended sms functionality that is mode-independent
*  Extension request handles and parameters are stored as attributes
*
*  @lib CommonTSY.lib
*  @since S60 v2.6
*/
NONSHARABLE_CLASS( CMmSmsTsy ) : public CSubSessionExtBase,
    public MCtsySatService
    {
    public:

        enum TSmsRequestTypes
            {
            EMultimodeSmsReqHandleUnknown,          //0
            //SMS specific requests
            EMultimodeSmsReceiveMessage,
            EMultimodeSmsSetReceiveMode,
            EMultimodeSmsAckStored,
            EMultimodeSmsNackStored,
            EMultimodeSmsResumeReception,
            EMultimodeSmsSendMessage,
            EMultimodeSmsSendSatMessage,
            EMultimodeSmsGetMessageStoreInfo,
            EMultimodeSmsReadSmspListPhase1,
            EMultimodeSmsStoreSmspList,
            EMultimodeSmsNotifyReceiveModeChange,
            EMultimodeSmsNotifyMoSmsBearerChange,
            EMultimodeSmsSetMoSmsBearer,
            EMultimodeSmsNotifySmspListChange,
            //SMS storage specific requests
            EMultimodeSmsGetInfo,
            EMultimodeSimStSmsReadSms,
            EMultimodeSimStSmsWriteSms,
            EMultimodeSimStSmsEraseSms,
            EMultimodeSimStSmsEraseAllSms,
            EMultimodeSimSmsNotifyStoreEvent,
            EMultimodeSimStSmsReadAllSms,
            EMultimodeSimStSmsGetNumOfLoc,

            EMultimodeSmsMaxNumOfRequests
            };

    private:

        /** SMS routing activity */
        enum TSmsRoutingActivity
            {
            ERoutingNotActivated,
            ERoutingActivating,
            ERoutingActivated
            };

    public:

        /**
         * Two-phased constructor.
         * @param aMmPhone Pointer to the MmPhone
         * @return Pointer to the created CMmSmsTsy object
         */
        static CMmSmsTsy* NewL( CMmPhoneTsy* aMmPhone );

        /**
         * Destructor
         */
        ~CMmSmsTsy();

        /**
         * Initialisation method that is called from ETel Server
         *
         * @since S60 v2.6
         */
        virtual void Init();

        /**
         * New instances are created by given name
         *
         * @since S60 v2.6
         * @param aName Name of the object to be created
         * @return Pointer to created object casted to CTelObject*
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * New instances are created by given name
         *
         * @since S60 v2.6
         * @param aNewName Name of the object to be created
         * @return Pointer to created object casted to CTelObject*
         */
        virtual CTelObject* OpenNewObjectL( TDes& aNewName );

        /**
         * Handles extended client requests by calling ExtFunc method
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage Parameters for request
         * @return Error value
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * When the ETel server receives an "extension" client request,
         * it will pass the IPC request number down to the TSY in order to find
         * out what type of request it is
         *
         * @since S60 v2.6
         * @param aIpc IPC number of request
         * @return Request mode for given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC
         *
         * @since S60 v2.6
         * @param aIpc IPC number of request
         * @return Number of slots
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given in
         * parameters
         *
         * @since S60 v2.6
         * @param aIpc IPC number of request
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc,
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification
         *
         * @since S60 v2.6
         * @param aIpc Number of slots
         * @return Error value
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *
         * @since S60 v2.6
         * @param aIpc IPC number of request
         * @return Error value
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Return pointer to the SMS Storage sub-session
         *
         * @since S60 v2.6
         * @return Pointer to SMS Storage sub-session
         */
        virtual CMmSmsStorageTsy* GetSmsStorageTsy();

        /**
         * Activate SMS routing
         *
         * @since S60 v2.6
         */
        virtual void ActivateSmsRoutingL();

        /**
         * This method completes reception of incoming unstored SMS
         *
         * @since S60 v2.6
         * @param aError Error code
         * @param aDataPackage A data package containing TBool (indicates if
         * some errors have occured while handling or acknowledging incoming Sms)
         * and a TSmsMsg* (pointer to Sms message)
         */
        virtual void CompleteReceiveMessage( TInt aError,
            CMmDataPackage* aDataPackage );

        /**
         * Handle internally response for ActivateSmsRouting
         *
         * @since S60 v2.6
         * @param aError Error code
         * @param aDataPackage Package data containing TUint8,
         * status of the routing (active or not)
         */
        virtual void CompleteActivateSmsRouting( TInt aError,
            CMmDataPackage* aDataPackage );

        /**
         * Complete AckSmsStored request
         *
         * @since S60 v2.6
         * @param aError Error code
         */
        virtual void CompleteAckSmsStored( TInt aError );

        /**
         * Complete NackSmsStored request
         *
         * @since S60 v2.6
         * @param aError Error code
         */
        virtual void CompleteNackSmsStored( TInt aError );

        /**
         * Complete CompleteSendMessage request
         *
         * @since S60 v2.6
         * @param aError Error code
         * @param aDataPackage
         */
        virtual void CompleteSendMessage( TInt aError,
            CMmDataPackage* aDataPackage );

        /**
         * From MCtsySatService
         * Send SAT SMS message to the network
         *
         * @since S60 v2.6
         * @param aStkTsySatService Pointer to SAT service interface
         * @param aSmsTpdu Pointer to Sms TPDU
         * @param aScAddress Pointer to service address
         * @param aMobileTON Pointer to type of number
         * @param aMobileNPI Pointer to number plan
         * @param aMoreToSend True if there is more to send
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        virtual TInt SendSatMessage(
            MStkTsySatService& aStkTsySatService,
            TDes8* aSmsTpdu,
            TDes16* aScAddress,
            RMobilePhone::TMobileTON* aMobileTON,
            RMobilePhone::TMobileNPI* aMobileNPI,
            TBool aMoreToSend,
            TTsyReqHandle aTsyReqHandle );

        /**
         * Complete SendSatMessage request
         *
         * @since S60 v2.6
         * @param aError Error code
         */
        virtual void CompleteSendSatMessage( TInt aError );

        /**
         * Complete ResumeSmsReception request
         *
         * @since S60 v2.6
         * @param aError Error code
         */
        virtual void CompleteResumeSmsReception( TInt aError );

        /**
         * Complete get sms message store info
         *
         * @since S60 v2.6
         * @param aError Error code
         * @param aDataPackage
         */
        virtual void CompleteGetMessageStoreInfo( TInt aError,
            CMmDataPackage* aDataPackage );

        /**
         * Complete first phase of read all SMSP sets
         *
         * @since S60 v2.6
         * @param aError Error code
         * @param aDataPackage
         */
        virtual void CompleteReadAllSmspPhase1( TInt aError,
            CMmDataPackage* aDataPackage );

        /**
         * Handle internally SMS servers response to SMS Parameter update
         * request (Store SMSP list)
         *
         * @since S60 v2.6
         * @param aError Error code
         */
        virtual void InternalStoreSmspList( TInt aError );

        /**
         * Set SMS Storage TSY
         *
         * @since S60 v2.6
         * @param aNewPointer Pointer to Storage Tsy
         */
        virtual void SetSmsStorageTsy( CMmSmsStorageTsy* aNewPointer );

        /**
         * Return number of SMS location on SIM card
         *
         * @since S60 v2.6
         * @return Number of SMS location on SIM card
         */
        virtual TInt16 GetSmsNumOfLoc();

        /**
         * Set Sms Number of Location on SIM Card
         *
         * @since S60 v2.6
         * @param aSmsNumOfUsedEntries Number of SMS location
         */
        virtual void SetSmsNumOfLoc( TInt aSmsNumOfUsedEntries );

        /**
         * This method set Sms Send Attributes values for sending MMSAT
         * message
         *
         * @since S60 v2.6
         * @param aMmPhone
         * @param aSmsTpdu Pointer to Sms TPDU
         * @param aScAddress Pointer to service address
         * @param aMobileTON Pointer to type of number
         * @param aMobileNPI Pointer to number plan
         * @param aMoreToSend True if there is more to send
         * @return Error code
         */
        virtual TInt SendSatMessageL(
            CMmPhoneTsy* aMmPhone,
            TDes8* aSmsTpdu,
            TDes16* aScAddress,
            RMobilePhone::TMobileTON* aMobileTON,
            RMobilePhone::TMobileNPI* aMobileNPI,
            TBool aMoreToSend );

        /**
         * Set the information about the CS hardware status: True if off-line,
         * false if on-line
         *
         * @since S60 v2.6
         * @param aIsOffline
         */
        void SetOffline( TBool aIsOffline );

        /**
         * Complete set bearer type for sending SMS messages
         *
         * @since S60 v3.0
         * @param aResult Error value
         */
        void CMmSmsTsy::CompleteSetMoSmsBearer( TInt aResult );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *
         * @since S60 v2.6
         * @param aReqHandleType Req handle type
         * @param aTsyReqHandle Req handle to be stored
         */
        void SetTypeOfResponse( const TInt aReqHandleType,
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete request because of timer expiration
         *
         * @since S60 v2.6
         * @param aReqHandleType Request handle type
         * @param aError Error value
         */
        virtual void Complete( TInt aReqHandleType,
            TInt aError );
#endif // REQHANDLE_TIMER

    private:

        /**
         * C++ default constructor.
         */
        CMmSmsTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Resend an unsent message
         *
         * @since S60 v2.6
         */
        void ResendSms();

        /**
         * Handles extended client requests
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage Data Package
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Get SMS messaging capabilities
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aCaps Pointer to the messaging capabilities
         * @return KErrNone
         */
        TInt GetCaps( const TTsyReqHandle aTsyReqHandle,
            TDes8* aCaps );

        /**
         * Get receive mode
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aReceiveMode Pointer to receive mode
         * @return KErrNone
         */
        TInt GetReceiveMode( const TTsyReqHandle aTsyReqHandle,
            RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode );

        /**
         * Set receive mode
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aReceiveMode Pointer to receive mode
         * @return KErrNone
         */
        TInt SetReceiveMode( const TTsyReqHandle aTsyReqHandle,
            RMobileSmsMessaging::TMobileSmsReceiveMode const* aReceiveMode );

        /**
         * Set receive mode cancel
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SetReceiveModeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notify from receive mode changes
         *
         * @since S60 v2.6
         * @param aReceiveMode Pointer to receive mode
         * @return KErrNone
         */
        TInt NotifyReceiveModeChange(
            RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode );

        /**
         * Cancel notify reveice mode change request
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyReceiveModeChangeCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete notify reveice mode change request
         *
         * @since S60 v2.6
         */
        void CompleteNotifyReceiveModeChange();

        /**
         * This method starts the reception of incoming unstored SMS
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aMsgData Message data
         * @param aMsgAttributes Message attributes
         * @return Error value
         */
        TInt ReceiveMessageL( const TTsyReqHandle aTsyReqHandle,
            TDes8* aMsgData,
            TDes8* aMsgAttributes );

        /**
         * Wait for next incoming unstored message cancel
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt ReceiveMessageCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Store Class 2 message in TSY's internal memory
         *
         * @since S60 v2.6
         * @param aSmsMsg Pointer to TSmsMsg struct
         */
        void StoreClass2MessageL( TSmsMsg const* aSmsMsg );

        /**
         * Deliver Stored Class 2 message to the SMS stack
         * @since S60 v2.6
         */
        void DeliverClass2ToSmsStack();

        /**
         * Ack message
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aMsgData SMS PDU
         * @param aMemoryFull Is SMS stack's memory now full
         * @return Error value
         */
        TInt AckSmsStoredL( const TTsyReqHandle aTsyReqHandle,
            const TDesC8* aMsgData,
            TBool const* aMemoryFull );

        /**
         * Route RP-Error request to messagehandler
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aMsgData Pointer to message data
         * @param aRpCause Pointer to RP cause value
         * @return Error value
         */
        TInt NackSmsStoredL( const TTsyReqHandle aTsyReqHandle,
            const TDesC8* aMsgData,
            TInt* aRpCause );

        /**
         * Resume sms message reception
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt ResumeSmsReceptionL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Retrieve the current setting for the bearer type used
         * for sending SMS messages
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aBearer Pointer to bearer
         * @return KErrNone
         */
        TInt GetMoSmsBearer( const TTsyReqHandle aTsyReqHandle,
            RMobileSmsMessaging::TMobileSmsBearer* aBearer );

        /**
         * Set the bearer type for sending SMS messages
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aBearer Pointer to bearer
         * @return KErrNone
         */
        TInt SetMoSmsBearer( const TTsyReqHandle aTsyReqHandle,
            RMobileSmsMessaging::TMobileSmsBearer* aBearer );

        /**
         * Set the bearer type cancel
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SetMoSmsBearerCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notify from SMS bearer change
         *
         * @since S60 v2.6
         * @param aBearer Pointer to bearer
         * @return KErrNone
         */
        TInt NotifyMoSmsBearerChange(
            RMobileSmsMessaging::TMobileSmsBearer* aBearer );

        /**
         * Notify from SMS bearer change cancel
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyMoSmsBearerChangeCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Send message
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aMsgData Pointer to Message Data
         * @param aMsgAttributes Pointer to Message Attributes
         * @return Error value
         */
        TInt SendMessageL( const TTsyReqHandle aTsyReqHandle,
            TDes8* aMsgData,
            TDes8* aMsgAttributes );

        /**
         * Send sat message
         *
         * @since S60 v2.6
         * @param aSmsTpdu Pointer to SMS PDU
         * @param aScAddress Pointer to SC address
         * @param aMobileTON Type of number
         * @param aMobileNPI Numberin plan identific
         * @param aTsyReqHandle Request handle
         */
        void DoSendSatMessageL( TDes8* aSmsTpdu,
            TDes16* aScAddress,
            RMobilePhone::TMobileTON* aMobileTON,
            RMobilePhone::TMobileNPI* aMobileNPI,
            TBool aMoreToSend,
            TTsyReqHandle aTsyReqHandle );

        /**
         * Enumerate sms message stores
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aCount Pointer to message count
         * @return KErrNone
         */
        TInt EnumerateMessageStores( const TTsyReqHandle aTsyReqHandle,
            TInt* aCount );

        /**
         * Get sms message store info
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aIndex Index
         * @param aInfo Information
         * @return Error value
         */
        TInt GetMessageStoreInfoL( const TTsyReqHandle aTsyReqHandle,
            TInt const* aIndex,
            TDes8* aInfo );

        /**
         * Get sms message store info cancel
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetMessageStoreInfoCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Read SMSP list phase 1
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aId Pointer to client id
         * @param aBufSize Pointer to buffer size
         * @return Error value
         */
        TInt ReadSmspListPhase1L( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TClientId const* aId,
            TInt* aBufSize );

        /**
         * Read SMSP list phase 2
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aId Pointer to client id
         * @param aBuffer Pointer to buffer
         * @return KErrNone
         */
        TInt ReadSmspListPhase2( TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TClientId const* aId,
            TDes8* aBuffer );

        /**
         * Cancel read SMSP list
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt ReadAllSmspCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Create SMSP list from read entries
         *
         * @since S60 v2.6
         * @param aSmspList List of SMSP entries
         */
        void CreateSmspListL( CArrayPtrFlat<TSmsParameters>& aSmspList );

        /**
         * Store SMSP list
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @param aBuffer Pointer to buffer
         * @return KErrNone
         */
        TInt StoreSmspList( const TTsyReqHandle aTsyReqHandle,
            TDes8* aBuffer );

        /**
         * This method trap and calls ProcessStoreSmspListL-method
         *
         * @since S60 v2.6
         */
        void CallProcessStoreSmspList();

        /**
         * Call CompleteStoreSmspList-method or make new SMS parameter update
         * request to DOS
         *
         * @since S60 v2.6
         */
        void ProcessStoreSmspListL();

        /**
         * Complete StoreAllSmsp request to the client
         *
         * @since S60 v2.6
         * @param aError Cause value that is used while completing request
         */
        void CompleteStoreSmspList( TInt aError );

        /**
         * Notify SMSP list change
         *
         * @since S60 v2.6
         * @return KErrNone
         */
        TInt NotifySmspListChange();

        /**
         * Notify SMSP list change cancel
         *
         * @since S60 v2.6
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifySmspListChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete notify SMSP list
         *
         * @since S60 v2.6
         */
        void CompleteNotifySmspListChange();

        /**
         * Reset all class attributes
         *
         * @since S60 v2.6
         */
        void ResetVariables();

        /**
         * Checks wether or not a ETel request can be performed while
         * offline mode is enabled
         *
         * @since S60 v2.6
         * @param aIpc Ipc number of the request
         * @return TBool
         */
        TBool IsRequestPossibleInOffline( TInt aIpc ) const;

    private:

        /**
         * A pointer to the Phone TSY
         * Not own.
         */
        CMmPhoneTsy* iMmPhone;

        /**
         * A pointer to the SMS extension
         * Own.
         */
        CMmSmsExtInterface* iMmSmsExtInterface;

        /**
         * A pointer to the SMS storage sub-session
         * Not own.
         */
        CMmSmsStorageTsy* iMmSmsStorageTsy;

        /**
         * A pointer to the SAT service interface provided by the SAT
         * TSY.
         * Not own.
         */
        MStkTsySatService* iTsySatMessaging;

        /**
         * Routing activity
         */
        TSmsRoutingActivity iServerRoutingActivity;

        /**
         * Receive mode
         */
        RMobileSmsMessaging::TMobileSmsReceiveMode iMobileSmsReceiveMode;

        /**
         * Notify receive mode pointer
         * Not own.
         */
        RMobileSmsMessaging::TMobileSmsReceiveMode* iNotifyReceiveModeChangePtr;

        /**
         * Wait for unstored message pointer
         * Not own.
         */
        TAny* iReceiveMessageParamsPtr;

        /**
         * Wait for unstored message pointer
         * Not own.
         */
        TDes8* iReceiveMessagePduPtr;

        /**
         * Notify SMS bearer pointer
         * Not own.
         */
        RMobileSmsMessaging::TMobileSmsBearer* iNotifySmsBearerPtr;

        /**
         * A pointer to send message attributes
         * Not own.
         */
        TDes8* iSendMessageMsgAttrPckgPtr;

        /**
         * A pointer to get message store info
         * Not own.
         */
        TDes8* iGetMessageStoreInfoPtr;

        /**
         * Store all parameter sets buffer
         * Not own.
         */
        TDes8* iStoreSmspBufferPtr;

        /**
         * Store all parameter sets index
         */
        TInt iStoreSmspIndex;

        /**
         * Request handle type
         */
        TSmsRequestTypes iReqHandleType;

        /**
         * Pointer to TSY request handle store
         * Own.
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /**
         * A table for Sms request handles
         */
        TTsyReqHandle iSmsReqHandles[EMultimodeSmsMaxNumOfRequests];

        /**
         * SMSP list, in streamed format, for two phased list retrieval
         * Not own.
         */
        CBufBase* iSMSPList;

        /**
         * SMSP size buffer, for two phased list retrieval
         */
        TInt* iRetSMSPSize;

        /**
         * SMSP client id for two phased list retrieval
         */
        RMobilePhone::TClientId* iSMSPClientId;

        /**
         * SMS message array for incoming messages that are waiting
         * acknowledging from SMS stack
         * Own.
         */
        CArrayPtrFlat<TSmsMsg>* iSmsMsgArray;

        /**
         * Flag that indicates if client storage is full
         */
        TBool iClientStorageFull;

        /**
         * Number of SMS Locations on Sim Card
         */
        TUint8 iSmsNumOfLoc;

        /**
         * SMS send request
         * stores the last sent message for possible re-send attempts
         * Own.
         */
        CSmsSendRequest* iSmsSendReq;

        /**
         * On-line/off-line status of CS hardware.
         * At this moment, only needed for method ResumeSmsReception
         */
        TBool iIsOffline;

        /**
         * true if a ResumeSmsReception request has come during off-line
         * mode. See method ResumeSmsReception
         */
        TBool iResumeSmsReceptionPending;

};
#endif // CMMSMSTSY_H

//  End of File
