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



//  INCLUDES

#ifndef _CMMSMSMESSHANDLER_H_
#define _CMMSMSMESSHANDLER_H_

#include <etelmm.h>
#include <tisi.h>

#include "mmmmesshandlerbase.h"
#include "cmmsmsgsmutility.h" // ksimsbsmsgsmisimsgmaxlength, koperatornamemaxlength
#include "cmmphonetreceiver.h"
#include "cmmsmscache.h" // cmmsmscache
#include "muiccoperationbase.h"
#include "cmmuiccmesshandler.h"

// CONSTANTS

// SMS record length, as specified in ETSI TS 31 102
// "4.2.25   EFSMS (Short messages)"
const TUint8 KSmsElemetaryFileRecordLength = 176;

// for file descriptor tag in FCI data received from UICC Server
const TUint8 KTagFCIFileDescriptor = 0x82;

#define UICC_FCI_EF_FDESC_OFFSET_NUM_ENTR   6


// MACROS
//none

// DATA TYPES
//none

// EXTERNAL DATA STRUCTURES
//none

// FUNCTION PROTOTYPES
//none


//  FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmPhoNetReceiver;
class CMmMessageRouter;
class TIsiReceiveC;
class CMmUiccMessHandler;

//DESCRIPTION
//
//CMmSmsMessHandler is used to create and send SMS
//ISI-messages to PhoNet via PhoNetSender. It also receives SMS ISI-messages
//from PhoNet via PhoNetReceiver.
class CMmSmsMessHandler :
    public CBase,
    public MMmMessHandlerBase,
    public MMmMessageReceiver,
    public MUiccOperationBase
    {

    private: // types

        // The SMS request types are grouped into
        // the following types. Used as ISA transaction IDs.
        enum TSmsRequestType
            {
            ESmsMessagingUnknown = 0,
            ESmsMessagingGetMessageStoreInfo,
            ESmsMessagingReceiveMessage,
            ESmsMessagingSendMessage,
            ESmsMessagingSendSatMessage,
            ESmsMessagingSendNoFdnMessage,
            ESmsMessagingResumeSmsReception,
            ESmsMessagingGetSmspListPhase1,
            ESmsMessagingStoreSmspList,
            ESmsMessagingAckSmsStored,
            ESmsMessagingNackSmsStored,
            EPhoneStoreDelete,
            EPhoneStoreDeleteAll,
            EPhoneStoreWrite,
            EInternalNack,
            ESms2Cache,
            ESms2WriteReceived

            };

    //METHODS
    public:

        /**
        * Destructor
        * @return: None
        */
        ~CMmSmsMessHandler();

        /**
        * Creates a SMS MessageHandler object instance.
        * @param CMmPhoNetSender* aPhoNetSender: A pointer to PhoNet sender
        * @param CMmPhoNetReceiver* aPhoNetReceiver: pointer to PhoNet receiver
        * @param CMmMessageRouter* aMessageRouter: pointer to the message
        * router object
        * @return CMmSmsMessHandler*: Pointer to the created object
        */
        static CMmSmsMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler );

        /**
        * From CMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param TInt aIpc: IPC number of the request
        * @param const CMmDataPackage* aDataPackage: Data Package
        * @return TInt KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * Isimsg received by PhonetReceiver
        * @param const TIsiReceiveC& aIsiMsg: The received Isi message
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @param TInt aError: Error code
        */
        void HandleError( const TIsiReceiveC& aIsiMsg, TInt aError );

        /**
        * Start reading SMS's from SIM to cache
        * @return error code
        */
        void InitializeSmsCache();

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

    private:

        /**
        * Handle EMobileSmsMessagingGetMessageStoreInfo/EMobilePhoneStoreGetInfo
        * @return error code
        */
        TInt GetSmsStoreInfo( TInt aIpc );

        /**
        * Handle EMobilePhoneStoreDelete
        * @return error code
        */
        TInt DeleteSms( const CMmDataPackage* aDataPackage );

        /**
        * Handle EMobilePhoneStoreDeleteAll
        * @return error code
        */
        TInt DeleteAllSms();

        /**
        * Handle EMobilePhoneStoreRead
        * @return error code
        */
        TInt ReadSms( const CMmDataPackage* aDataPackage );

        /**
        * Handle EMobilePhoneStoreReadAllPhase1
        * @return error code
        */
        TInt ReadAllSmsL();

        /**
        * Handle EMobilePhoneStoreWrite
        * @return error code
        */
        TInt WriteSms( const RMobileSmsStore::TMobileGsmSmsEntryV1& aEntry,
                       TInt aIndex);

        /**
        * Constructor
        * @return: None
        */
        CMmSmsMessHandler();

        /**
        * Initialises object attributes.
        * @return None
        */
        void ConstructL();

        /**
        * Response for SmsMessageSendReq
        * @param const TIsiReceiveC& aIsiMsg: The Isi message
        * @param TInt aIpc: IPC number (ETel Messaging- or SAT-
        * originated SMS sending)
        */
        void SmsMessageSendResp(
            const TIsiReceiveC& aIsiMsg,
            TInt  aIpc );

        /**
        * Handle internally SIM server response to SimStSmsRead
        * (ReadAllSms) request
        * @param TBool aSupportedPduType: Is read PDU's type supported
        * @param TSmsMsg* aSmsMsg: A pointer to the sms message
        * @param Int aSmsLocIndex: index of the SIM SMS slot
        * @param TUint8 aSmsNumOfLoc: number of SIM SMS slots
        * @return void: None
        */
        void InternalRetrieveSmsListReadSmsL(
            TBool aSupportedPduType,
            TSmsMsg* aSmsMsg,
            TInt aSmsLocIndex,
            TUint8 aSmsNumOfLoc );

        /**
        * Sms gsm received pp report request (Ack or Nack)
        * @param TUint8 aTransactionId: Transaction identifier
        * @param const TDesC8* aMsgData: A pointer to the Message data
        * @param TInt aRpCause: RP cause
        * @return TInt: KErrNone or error code
        */
        TInt SmsReceivedMsgReportReq(
            TUint8 aTransactionId,
            const TDesC8* aMsgData,
            TInt aRpCause );

        /**
        * Coverts RP cause value to SMS cause cause value.
        * @param aRpCause - RP Cause value
        * @return TUint8 - SMS Cause value
        */
        TUint8 SmsMapCause( TInt aRpCause );

        /* Complete reading of one SMSP set
        * @param TSmsParameters* aParameters: A pointer to sms parameter
        * @return: void: None
        */
        void InternalRetrieveSmspListL( TSmsParameters* aParameters );

        /**
        * Activate, deactivate or query status of SMS receiving
        * @param aAction Reception command
        * @return KErrNone or error code
        */
        TInt SmsReceiveMessageReq( TUint8 aAction );

        /**
        * Sms message send request to sms server
        * @param TUint8 aTransactionId: Transaction identifier
        * @param const CMmDataPackage* aDataPackage: Data package
        * with SMS message data and attributes
        * @param TBool aSmsCheckDisableFdn: If ETrue the sub block
        * SMS_SB_CHECK_INFO with SMS_CHECK_DISABLE_FDN is inculed
        * when SMS_MESSAGE_SEND_REQ is sent.
        * @return TInt: KErrNone or error code
        */
        TInt SmsMessageSendReq(
            TUint8 aTransactionId,
            const  CMmDataPackage* aDataPackage,
            TBool  aSmsCheckDisableFdn );

        /**
        * Creates SMS_MESSAGE_SEND_REQ ISI message with all needed
        * subblocks and sends it to SMS server.
        * @param aTransactionId Transaction ID
        * @param aMsgAttr Message attributes
        * @param aMsgData TPDU
        * @param aSubblockId Subblock ID
        * @param TBool aSmsCheckDisableFdn: If ETrue the sub block
        * SMS_SB_CHECK_INFO with SMS_CHECK_DISABLE_FDN is inculed
        * when SMS_MESSAGE_SEND_REQ is sent.
        * @return Symbian error code
        */
        TInt CreateSmsMessageSendReq(
            TUint8 aTransactionId,
            RMobileSmsMessaging::TMobileSmsSendAttributesV1* aMsgAttr,
            const TDesC8& aMsgData,
            TUint8 aSubblockId,
            TBool aSmsCheckDisableFdn );

        /**
        * Response to SMS_RECEIVED_MSG_REQ.
        * @param aIsiMsg ISI message
        */
        void SmsReceiveMessageResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Incoming SMS message
        * @param const TIsiReceiveC& aIsiMsg: The ISI message
        */
        void SmsReceivedMsgInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Read received class2 SMS that has just been stored
        * to SIM card
        * @param TIsiReceiveC* aIsiMsg: A pointer to Isi message
        * @return TInt: KErrNone
        */
        TInt ReadReceivedClass2Sms( TIsiReceiveC* aIsiMsg );

        /**
        * Response for SmsReceivedMsgReportReq
        * @param const TIsiReceiveC& aIsiMsg: Isi message
        */
        void SmsReceivedMsgReportResp( const TIsiReceiveC& aIsiMsg );

        // SIM Server responses
        /**
        * SIM Server responses
        * @param const TIsiReceiveC& aIsiMsg: The received Isi message
        * @return TInt: KErrNone
        */
        TInt SimStSmsWriteResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Build Service Centre Address
        * @param RMobilePhone::TMobileAddress const& scPtr: Pointer to
        * Service center
        * @param TDes8& aScAddress: Service centre address
        * @return void: None
        */
        void BuildScAddress(
            RMobilePhone::TMobileAddress const & scPtr,
            TDes8& aScAddress ) const;

        /**
        * Build Destination Address
        * @param RMobilePhone::TMobileAddress const& scPtr: Service centre ptr
        * @param TDes8& aScAddress: Destination address
        * @return void: None
        */
        void BuildDeAddress(
            RMobilePhone::TMobileAddress const & scPtr,
            TDes8& aDeAddress ) const;

        /**
        * Check message datacoding scheme
        * @param TUint8 dcs: Data coding scheme
        * @return TBool: ETrue if 7bit, else EFalse
        */
        TBool IsDataCodingScheme7Bit( TUint8 aDcs ) const;

        /**
        * Calculate number of characters in user data
        * @param TUint8 aCharacterAmount: Character amount (from ISI message)
        * @param TUint8 aDataLength: Data length in bytes (from ISI message)
        * @param TPtrC8 const& aUserData: User data
        * @return TUint8: number of characters in user data
        */
        TUint8 CalculateNumberOfCharsInUserData(
            TUint8 aCharacterAmount,
            TUint8 aDataLength,
            TPtrC8 const& aUserData ) const;

        /**
        * Check if the SMS is from class2 or not
        * @param const TIsiReceiveC& aIsiMsg: The Isi message
        * @param TUint8& aSmsClass2ReplaceTpPid: set to tppid to replace
        * @return TBool: ETrue is SMS Class is Class2
        */
        TBool IsSmsClass2(
            const TIsiReceiveC& aIsiMsg,
            TUint8& aSmsClass2ReplaceTpPid );

        /**
        * Checks if aReadSmsIsiMsg contains same TP-PID as
        * aReceivedTpPid, and same sender number and Service Center number
        * as aReceivedSmsIsiMsg.
        * @param TUint8 aReceivedTpPid: received TP-PID
        * @param const TIsiReceiveC& aReceivedSmsIsiMsg: received isi msg
        * @param TDesC8& aSMSOnSIM: SMS stored on SIM
        */
        TBool CheckTpPidAndSenderAndServiceCenter(
            TUint8 aReceivedTpPid,
            const TIsiReceiveC& aReceivedSmsIsiMsg,
            RMobileSmsStore::TMobileGsmSmsEntryV1* aSMSOnSIM );

        /**
        * Sends a SMS_SETTINGS_UPDATE_REQ (updates only the route
        * setting) and saves the bearer setting in iMobileSmsBearer.
        * @param RMobileSmsMessaging::TMobileSmsBearer& aBearer : Preferred
        * SMS Bearer (CS or PS)
        * @return TInt: KErrNone or error code
        */
        TInt SmsSettingsUpdateReq(
            RMobileSmsMessaging::TMobileSmsBearer& aBearer );

        /**
        * Response for SmsSettingsUpdateReq
        * @param const TIsiReceiveC& aIsiMsg: The receiced ISI message
        * @return void
        */
        void SmsSettingsUpdateResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Processes class2 SMS. Called by
        * CMmSmsMessHandler::SmsReceivedMsgInd.
        * @param const TIsiReceiveC& aIsiMsg received ISI-message
        * @param TUint8 aIsReplace: is this replacement
        * @return TInt system-wide error code
        * @note does not complete request on error
        */
        TInt SmsClass2ReceivedMsgInd( const TIsiReceiveC& aIsiMsg,
                TUint8 aIsReplace );

        /**
        * Processes class1 SMS. Called by
        * CMmSmsMessHandler::SmsReceivedMsgInd.
        * @param const TIsiReceiveC& aIsiMsg received ISI-message
        * @return TInt system-wide error code
        * @note does not complete request on error
        */
        TInt SmsClass1ReceivedMsgInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Returns the length of destination address field from TPDU
        * @param aMsgData SMS TPDU
        * @param subBlockId Subblock ID
        * @param aDestAddressLength Reference to destination address length
        * @return Symbian error code
        */
        TInt GetDestAddressLength(
            const TDesC8& aMsgData,
            TUint8 subBlockId,
            TUint8& aDestAddressLength ) const;

        /**
        * Creates SMS_SB_SUBMIT subblock and appends it to ISI message
        * @param aMsgData SMS TPDU
        * @param aIsiMsg ISI message
        * @param aDestAddressLength Destination address length
        * @param aMsgOffset Offset of ISI message where to append subblock
        * @return void
        */
        void BuildSmsSbSubmit(
            const TDesC8& aMsgData,
            TIsiSend& aIsiMsg,
            TUint8 aDestAddressLength,
            TUint8 aMsgOffset ) const;

        /**
        * Creates SMS_SB_COMMAND subblock and appends it to ISI message
        * @param aMsgData SMS TPDU
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset of ISI message where to append subblock
        * @return void
        */
        void BuildSmsSbCommand(
            const TDesC8& aMsgData,
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset ) const;

        /**
        * Creates SMS_SB_ADDRESS subblock and appends it to ISI message
        * @param aAddress Address data
        * @param aIsiMsg ISI message
        * @param aAddressType Address type
        * @param aMsgOffset Offset of ISI message where to append subblock
        * @return Length of created subblock
        */
        TUint8 BuildSmsSbAddress(
            const TDesC8& aAddress,
            TIsiSend& aIsiMsg,
            TUint8 aAddressType,
            TUint8 aMsgOffset ) const;

        /**
        * Creates SMS_SB_USER_DATA subblock and appends it to ISI message
        * @param aMsgData SMS TPDU
        * @param aIsiMsg ISI message
        * @param aTpUdl User data length from TPDU
        * @param aTpUserDataIndex Index of user data in TPDU
        * @param aDefaultAlphabet Is 7-bit coding used
        * @param aMsgOffset Offset of ISI message where to append subblock
        * @return Length of created subblock
        */
        TUint8 BuildSmsSbUserData(
            const TDesC8& aMsgData,
            TIsiSend& aIsiMsg,
            TUint8 aTpUdl,
            TUint8 aTpUserDataIndex,
            TBool aDefaultAlphabet,
            TUint8 aMsgOffset ) const;

        /**
        * Creates SMS_SB_VALIDITY_PERIOD subblock and appends it to ISI message
        * @param aMsgData SMS TPDU
        * @param aIsiMsg ISI message
        * @param aTpVpIndex Index of validity period in TPDU
        * @param aTpVpLength Length of validity period
        * @param aMsgOffset Offset of ISI message where to append subblock
        * @return void
        */
        void BuildSmsSbValidityPeriod(
            const TDesC8& aMsgData,
            TIsiSend& aIsiMsg,
            TUint8 aTpVpIndex,
            TUint8 aTpVpLength,
            TUint8 aMsgOffset )const;

        /**
        * Creates SMS_SB_CHECK_INFO sub block with SMS_CHECK_DISABLE_FDN and appends it
        * to ISI message.
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset of ISI message where to append subblock
        * @return void
        */
        void BuildSmsCheckInfo(
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset )const;

        /**
        * Read SMS or EF SMS record count from SIM/USIM
        * @param aRecordId (If record count is requested this value has no
        * effect => may be set e.g. 0 or FF)
        * @param aTrId Transaction ID:
        * - ETrIdReadSMS when SMS is read from certain record.
        * - ETrIdReadSMSRecordCount when EF SMS record count is requested.
        * @return Error code
        */
        TInt UiccReadSMSOrSMSRecordCountReq( const TUint8 aRecordId,
                                             const TUiccTrId aTrId );

        /**
        * Read Class 2 SMS from SIM/USIM response
        * @param aStatus Status
        * @param aFileData File data
        * @return void
        */
        void UiccReadSMSResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Read Class 2 SMS from SIM/USIM response when SMS is at first
        * written to SIM
        * @param aStatus Status
        * @param aFileData File data
        * @return void
        */
        void UiccReadSMSRespForComplete(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Write Class 2 SMS from SIM/USIM
        * @param aInfo Flags 2 data
        * @return void
        */
        TInt UiccWriteSMSReq(
            const RMobileSmsStore::TMobileGsmSmsEntryV1& aEntry,
            const TUint8 aRecordId );

        /**
        * Write Class 2 SMS from SIM/USIM response
        * @param aInfo Flags 2 data
        * @return void
        */
        void UiccWriteSMSResp( TInt aStatus );

        /**
        * Read number of EF SMS records
        * @return void
        */
        void GetNumOfEFSMSRecords( void );

        /**
        * Response for Read EF SMS record count
        * @param aStatus Status
        * @param aFileData File data
        * @return void
        */
        void GetNumOfEFSMSRecordsResp(
            TInt aStatus,
            const TDesC8& aFileData );

    //ATTRIBUTES
    public:
        //none

    private:

        // Pointer to the message router
        CMmMessageRouter* iMessageRouter;

        // Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        // Operator name from SIM
        TBuf16<KOperatorNameMaxLength> iOperatorName;

        // SIM SMS cache
        CMmSmsCache iSmsCache;

        // Slot (location ) in which the SMS is going to be written on SIM card
        // This is needed because write resp doesn't contain the slot written to
        // information..
        TUint8 iSmsSlotLocation;

        // Read all parameter sets location index
        TUint8 iLocationOfSmspSet;

        // Amounth of SMS parameter sets to be read
        TUint8 iAmountOfSmspSets;

        // Temporary SMS parameter storage array, that is used while
        // retrieving of SMSP list
        CArrayPtrFlat<TSmsParameters>* iSmspListArray;

        // Write SMS message pointer
        const RMobileSmsStore::TMobileGsmSmsEntryV1* iWriteSmsEntryPtr;

        // SMS message list storage array
        CArrayPtrFlat<TSmsMsg>* iSmsListArray;

        // flag to check if a Class2 SMS has been received while
        // SMS lots were being read
        TBool iReceivedClass2ToBeReSent;

        // MO SMS Bearer
        RMobileSmsMessaging::TMobileSmsBearer iMobileSmsBearer;

        // Is SMS Sending ongoing
        TBool iSMSSendingOngoing;

        // Record ID saved during query
        TUint8 iRecordId;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

        // Flag to check is the case class 2 SMS write or EMobileStoreWrite
        TBool iSMSClass2Write;
    };

#endif // _CMMSMSMESSHANDLER_H_

//End of file
