/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _CMMCUSTOMMESSHANDLER_H
#define _CMMCUSTOMMESSHANDLER_H

// INCLUDES
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include "muiccoperationbase.h"
#include "cmmuiccmesshandler.h"

// for internal testing
#include "nokiatsy_internal_variation.h"

#include <ctsy/rmmcustomapi.h>

// CONSTANTS

// Length of Sim info isi msg buffer
const TUint8 KSimInfoBufferSize = 20;

// Time related constants
const TUint16 KSecsInHour = 3600;
const TUint8 KMinsInHour = 60;

// Length if Gss Cs isi msg buffer
const TUint8 KGssCsBufferSize = 6;

// Call property buffer size
const TUint8 KCallPropBuffSize = 4;

// Value for information which value is not needed.
const TUint8 KZero = 0x00;
// Default length for product profile reading request
const TUint8 KDefaultPPlength = 0x01;

// Max length of network proveder name
const TUint8 KNwProviderNameLength = 0x19;
// Constants for remote address length
const TUint8 KRemoteAddressBcdSize = 18;
const TUint8 KRemoteAddressAsciiSize = 34;
const TUint8 KRemoteAddressConversionSize = 36;

// SS Request Complete Notifivation SS Status
const TUint8 KCustomApiSsGsmActive = 0x01;
const TUint8 KCustomApiSsGsmRegistered = 0x02;
const TUint8 KCustomApiSsGsmProvisioned = 0x04;
const TUint8 KCustomApiSsGsmQuiescent = 0x08;

const TUint8 KinfoPpTwoDigitDial = 0x0F;

//value for information which value is two
const TUint8 KTwo = 0x02;

//value for information which value is three
const TUint8 KThree = 0x03;

//value for information which value is four
const TUint8 KFour = 0x04;

//value for information which value is six
const TUint8 KSix = 0x06;

const TUint8 KCustomPadding = 0x00; //Padding byte

// Call Life Timer Constants
const TUint8 KCallLifeTimerDataAmount = 0x04;
const TInt KCallLifeTimerNotSet = -1;
const TUint8 KPmmGroupNokiaTsySize = 100;

// Maximum number of re-trials when requesting data from PMM.
const TUint8 KMaxPMMReadRequests = 0x03;

// These constants are needed for Viag Home Zone
const TUint8 KLengthOfSMSCNumber = 15;
const TUint8 KLengthOfSCPNumber = 15;
const TUint8 KLengthOfHzCzTag = 13;
const TUint8 KHomeZoneActiveBit = 0x01;
const TUint8 KCityZoneActiveBit = 0x02;

const TUint16 KUnusedCbMsgId = 0xffff;

const TUint8 KCustomTransId = 6;

// MACROS
    //None

// DATA TYPES
    //None

// EXTERNAL DATA STRUCTURES
    //None

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
class CMmSecurityMessHandler;
class CMmNetMessHandler;
class CMmPhoNetSender;
class TIsiReceiveC;
class CMmMessageRouter;
class CMmUiccMessHandler;

// CLASS DECLARATION

/**
*  CMmCustomMessHandler is used to create and send GSM specific
*  custom ISI messages to PhoNet via PhoNetSender.
*/
class CMmCustomMessHandler :
    public CBase,
    public MMmMessHandlerBase,
    public MMmMessageReceiver,
    public MUiccOperationBase
    {

    public:

    // Used for saving the ciphering information.
    struct TCiphListEntry
        {
        TBool iCiphStatus;
        TUint8 iTraId;
        };

    // Used for saving the enhanced cell information.
    class TECIDInfo
        {
        public: // data
        RMmCustomAPI::TMmCellInfo iCellInfo; // Cell Information
        TUint iMCC; // Mobile Country Code
        TUint iMNC; // Mobile Network Code
        TUint iCID; // Location area code
        TUint iLAC; // Cell identity
        };

    // This enumeration is needed because different Product Profile
    // requests can be made using the same INFO_PP_READ_REQ/RESP
    // messages. Used as a transaction ID.
    enum TProductProfileRequestType
        {
        EGetAlsPPSupport = 0,
        ECheckTwoDigitDialSupport
        };

    // This enumeration is needed because a SIM_READ_CI_RESP can come
    // as a response to a GetCipheringInfo-request, or triggered by
    // a NET_CIPHERING_IND indication which makes the same request.
    // Used as a transaction ID.
    enum TCipheringInfoRequestType
        {
        EGetCipheringInfo = 0,
        ENotifyCipheringInfo
        };

    // The SIM operations are grouped into the following types.
    // Used as ISA transaction IDs.
    enum TSIMOperationType
        {
        ESIMOperationTypeReadSimTopics = 0,
        ESIMOperationTypeDeleteSimTopic
        };

    enum TUiccSapIfStatus
        {
        EUiccSapIfStatus1 = 1,
        EUiccSapIfStatus2,
        EUiccSapIfStatus3
        };

    public: // Constructors and destructor

        /**
        * Creates a new GSM specific MessageHandler object instance.
        * @param aPhoNetSender: a pointer to a Phonet Sender object
        * @param aPhoNetReceiver:
        * @param aNetMessHandler: a pointer to the Net message handler
        * @param aSecurityMessHandler: a pointer to the security mess handler
        * @param aMessageRouter: a pointer to the message router
        * @return void
        */
        static CMmCustomMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmNetMessHandler* aNetMessHandler,
            CMmSecurityMessHandler* aSecurityMessHandler,
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler);

        /**
        * Destructor.
        * @return void
        */
        ~CMmCustomMessHandler();

    public: // New Functions

        /**
        * Creates CallReleaseReq ISI message and sends it to Phonet.
        * @param TUint8 aTransactionId: unique transaction id
        * @param TUint8 aCallId: Call ID of this call (NOS Call ID)
        * @param TUint8 aCause: cause of releasing
        * @return TInt: success/failure value
        */
        TInt CallReleaseReq(
            TUint8 aTransactionId,
            TUint8 aCallId,
            TUint8 aCause );

        /**
        * Breaks received CallReleaseResp ISI message.
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void CallReleaseResp( const TIsiReceiveC& aIsiMsg );

        /**
        * MO alert indication, used for Remote Alerting Tone in Custom TSY
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @return TInt: void : None
        */
        void CallMoAlertInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Creates a NET_CS_WAKEUP_REQ ISI message and sends it to Phonet
        * @param TUint8 aTransId: transaction Id
        * @return TInt: success/failure value
        */
        TInt NetCsWakeupReq( TUint8 aTransId );

        /**
        * Breaks received NET_CS_WAKEUP_RESP ISI message
        * @param None
        * @return void : None
        */
        void NetCsWakeupResp();

        /**
        * Breaks received NET_CIPHERING_IND ISI message
        * @param TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void NetCipheringInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Breaks received NET_MODEM_REG_STATUS_IND ISI message
        * @param TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void NetModemRegStatusInd( const TIsiReceiveC& aIsiMsg );


        /**
        * Read dynamic flags 2
        * @return void
        */
        TInt UiccReadDynamic2FlagsReq();

        /**
        * Read dynamic flags 2 response
        * @param aStatus Status
        * @param aFileData File data
        * @return void
        */
        void UiccReadDynamic2FlagsResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Write dynamic flags 2
        * @param aInfo Flags 2 data
        * @return void
        */
        TInt UiccWriteDynamic2FlagsReq( TUint8 aInfo );

        /**
        * Write dynamic flags 2 response
        * @param aInfo Flags 2 data
        * @return void
        */
        void UiccWriteDynamic2FlagsResp( TInt aStatus );

        /**
        * Breaks received CALL_TERMINATE_IND ISI messages
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void CallTerminateInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Breaks received CALL_RELEASE_IND ISI messages
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void CallReleaseInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Creates a INFO_PP_READ_REQ ISI messages and sends it to phone
        * @param TProductProfileRequestType aReqType: request type (used
        *        as transaction id)
        * @return void : KErrNone / Error value from phonet
        */
        TInt InfoPpReadReq( TProductProfileRequestType aReqType );

        /**
        * Breaks a INFO_PP_READ_RESP message
        * @param CIsiMsg* aIsiMsg: The received ISI message
        * @return void : None
        */
        void InfoPpReadResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates a CALL_GSM_BLACKLIST_CLEAR_REQ ISI message and sends it to
        * Phonet
        * @param TUint8 aTransId: transaction Id
        * @return TInt : KErrNone or error code
        */
        TInt CallGsmBlackListClearReq( TUint8 aTransId );

        /**
        * Breaks received CALL_GSM_BLACKLIST_CLEAR_RESP ISI message
        * @return void : None
        */
        void CallGsmBlackListClearResp();

        /**
        * Breaks received SS_SERVICE_COMPLETED_IND ISI message
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @return void : None
        */
        void SsServiceCompletedInd ( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates a CALL_EMERGENCY_NBR_CHECK_REQ ISI messages and sends it to
        * phonet
        * @param TUint8 aTransId: Transaction Id
        * @param TMobileTelNumber&: telnumber
        * @param TCheckMode aCheckMode; check mode
        * @return TInt : KErrNone / Error value from phonet
        */
        TInt CallEmergencyNbrCheckReq( TUint8 aTransId,
            RMmCustomAPI::TMobileTelNumber& aNumber,
            RMmCustomAPI::TCheckMode aCheckMode );

        /**
        * Breaks a CALL_EMERGENCY_NBR_CHECK_RESP message
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void CallEmergencyNbrCheckResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Creates a SS_GSM_USSD_SEND_REQ ISI messages and sends it to IsaApi
        * @param TUint8 aTransId: Transaction Id
        * @return TInt : KErrNone / Error value from IsaApi
        */
        TInt SsGsmUssdSendReq( TUint8 aTransId );

        /**
        * Forwards a execution of SS_CANCEL_USSD_SEND_RESP message
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @return void : None
        */
        void SsGsmUssdSendResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Construct a NET_MODEM_REG_STATUS_GET_REQ ISI message and send
        * it through phonet.
        * @param TUint8 aTransId: Transaction Id
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt NetModemRegStatusGetReq( TUint8 aTransId );

        /**
        * Construct a NET_SET_REQ message and send it through phonet.
        * @param TUint8 aTransId: Transaction Id
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt NetSetReq( TUint8 aTransId );
        /**
        * Handles SsRequestComplete response ISI message
        * @return void : None
        */
        void SsRequestComplete();

        /**
        * Read field from SIM/USIM
        * @param aSimFileInfo File info
        * @return Error code
        */
        TInt UiccReadFieldReq( RMmCustomAPI::TSimFileInfo& aSimFileInfo );

        /**
        * Response to read field request
        * @param aStatus Status
        * @param aFileData File data
        * @return void
        */
        void UiccReadFieldResp( TUint8 aStatus, const TDesC8& aFileData );

        /**
        * Read ciphering indicator status from SIM/USIM
        * @param aTrId Transaction ID
        * @return Error code
        */
        TInt UiccReadCiReq( TInt aTrId );

        /**
        * Response to read CI request
        * @param aStatus Status
        * @param aTrId Transaction ID
        * @param aFileData File data
        * @return void
        */
        void UiccReadCiResp(
            TInt aStatus,
            TInt aTrId,
            const TDesC8& aFileData );

        /**
        * Read operator name from SIM/USIM
        * @return Error code
        */
        TInt UiccOperatorReq();

        /**
        * Response to read operator name request
        * @param aStatus Status
        * @param aFileData File data
        * @return none
        */
        void UiccOperatorResp( TUint8 aStatus, const TDesC8& aFileData );

        /**
        * Disconnects UICC server from smartcard and
        * activates UICC SAP APDU interface
        * @return Error code
        */
        TInt UiccSimPowerOffReq();

        /**
        * Handles responses to SIM power off
        * @param aStatus Status
        * @return none
        */
        void UiccSimPowerOffResp( TInt aStatus );

        /**
        * Requests SIM warm reset
        * @return Error code
        */
        TInt UiccSimWarmResetReq();

        /**
        * Response to SIM warm reset
        * @param aStatus Status
        * @return none
        */
        void UiccSimWarmResetResp( TInt aStatus );

        /**
        * Requests Get ATR
        * @return Error code
        */
        TInt UiccSimGetAtrReq();

        /**
        * Handles responses to SIM Get ATR
        * @param aStatus Status
        * @param aFileData File data
        * @return none
        */
        void UiccSimGetAtrResp( TInt aStatus, const TDesC8& aFileData );

        /**
        * Connects UICC server to smartcard and
        * deactivates UICC SAP APDU interface
        * @return Error code
        */
        TInt UiccSimPowerOnReq();

        /**
        * Handles responses to SIM power off
        * @param aStatus Status
        * @return none
        */
        void UiccSimPowerOnResp( TInt aStatus );

        /**
        * Sends APDU to UICC
        * @param aApduParameters APDU parameters
        * @param aTraId transaction id
        * @return Error code
        */
        TInt UiccSendAPDUReq(
            const RMmCustomAPI::TApduParameters& aApduParameters,
            TInt aTraId );

        /**
        * Handles responses to UICC_APDU_RESP
        * @param aStatus Status
        * @param aFileData File data
        * @param aTraId transaction id
        * @return none
        */
        void UiccSendAPDUResp(
            TInt aStatus,
            const TDesC8& aFileData,
            TInt aTraId );

        /**
        * Sends UICC_CARD_REQ
        * @return Error code
        */
        TInt UiccCardReaderStatusReq();

        /**
        * Handles responses for Card Reader Status req
        * @param aStatus Status
        * @param aFileData File data
        * @return none
        */
        void UiccCardReaderStatusResp( TInt aStatus, const TDesC8& aFileData );

        /**
        * Handles UICC_CARD_IND
        * @param aIsiMessage Received ISI message
        * @return void
        */
        void UiccCardInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Read CB message IDs from UICC
        * @param aTraId Transaction identification
        * @return Error code
        */
        TInt UiccReadCbMsgIdsReq( TUiccTrId aTrId );

        /**
        * Response to read CB message IDs from UICC
        * @param aStatus Status
        * @param aTraId Transaction ID
        * @param aFileData File data
        * @return void
        */
        void UiccReadCbMsgIdsResp(
            TUint8 aStatus,
            TInt aTraId,
            const TDesC8& aFileData );

        /**
        * Response to delete CB message ID request
        * @param aStatus Status
        * @return void
        */
        void UiccDeleteCbMsgIdResp( TUint8 aStatus );

        /**
        * Read home zone parameters from UICC
        * @return Error code
        */
        TInt UiccReadViagHomeZoneParametersReq();

        /**
        * Response read home zone parameters request
        * @param aStatus Status
        * @param aFileData File data
        * @return void
        */
        void UiccReadViagHomeZoneParametersResp(
            TUint8 aStatus,
            const TDesC8& aFileData );

        /**
        * Read home zone cache from UICC
        * @params aRecordId Record identifier
        * @return Error code
        */
        TInt UiccReadViagHomeZoneCacheReq(
            const RMmCustomAPI::TViagCacheRecordId& aRecordId );

       /**
        * Response to read home zone cache request
        * @param aStatus Status
        * @param aFileData File data
        * @return void
        */
        void UiccReadViagHomeZoneCacheResp(
            TUint8 aStatus,
            const TDesC8& aFileData );

        /**
         * Request to write home zone settings
         * @param aUhziuiSettings UHZIUI settings
         * @return void
         */
        TInt UiccWriteViagHomeZoneUhziueSettingsReq(
            const RMmCustomAPI::TViagUHZIUESettings& aUhziuiSettings );

        /**
         * Response to write home zone settings
         * @param aStatus Status
         * @return void
         */
        void UiccWriteViagHomeZoneUhziueSettingsResp( TUint8 aStatus );

        /**
        * Write Viag Home Zone cache from UICC
        * @params aRecordId Record identifier
        * @params aViagRecordContent Record content
        * @return error code
        */
        TInt UiccWriteViagHomeZoneCacheReq(
            const RMmCustomAPI::TViagCacheRecordId& aRecordId,
            const RMmCustomAPI::TViagCacheRecordContent& aViagRecordContent );

       /**
        * Response Write Viag Home Zone cache request
        * @param aStatus Status
        * @return void
        */
        void UiccWriteViagHomeZoneCacheResp( TUint8 aStatus );

    public: // Functions from base classes

        /**
        * ISI message for CustomMessageHandler received
        * @param const TIsiReceiveC& aIsiMessage: reference to received ISI message
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param TIsiReceiveC& aIsiMessage: reference to received ISI message
        * @param TInt aError: Error code
        */
        void HandleError( const TIsiReceiveC& aIsiMessage, TInt aError );

        /**
        * From MMmMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @since ?Series60_version
        * @param aIpc IPC number of the request
        * @return TInt KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

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

    protected: // New functions
        //None

    protected: // Functions from base classes
        //None

    private:

        /**
        * C++ default constructor.
        * @return void
        */
        CMmCustomMessHandler();

        /**
        * Initialises object attributes.
        * By default Symbian 2nd phase constructor is private.
        * @return void
        */
        void ConstructL();

        /**
        * Breaks a CALL_GSM_NOTIFICATION_IND ISI-message.
        * @param const TIsiReceiveC& aIsiMsg: Received ISI message
        * @return None
        */
        void CallGsmNotificationInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Handles Forwarding data from a CALL_GSM_NOTIFICATION_IND
        * @param TUint16 aSsCode,
        * @param const TIsiReceiveC& aIsiMsg,
        * @param RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode
        * @return None
        */
        void HandleForwarding(
            TUint16 aSsCode,
            const TIsiReceiveC& aIsiMsg,
            RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode ) const;

        /**
        * Handles Barring data from a CALL_GSM_NOTIFICATION_IND
        * @param TUint16 aSsCode,
        * @param const TIsiReceiveC& aIsiMsg,
        * @param RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode
        * @return None
        */
        void HandleBarring(
            TUint16 aSsCode,
            const TIsiReceiveC& aIsiMsg,
            RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode ) const;

        /**
        * Match function for making finds in the Ciphering list
        * @param TCiphListEntry& aArg1: first argument for match
        * @param TCiphListEntry& aArg2: secnd argument for the match
        * @return TBool: if both arguments match
        */
        static TBool Match(
            const TCiphListEntry& aArg1,
            const TCiphListEntry& aArg2 );

        /**
        * Breaks received NET_MODEM_REG_STATUS_GET_RESP ISI message
        * @param TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void NetModemRegStatusGetResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Breaks received NET_SET_RESP ISI message
        * @param TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void NetSetResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Breaks received SS_SERVICE_FAILED_RESP ISI message
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @return void : None
        */
        void SsServiceFailedResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Construct a NET_RAT_REQ message and send it through phonet.
        * @param TUint8 aTransId: Transaction Id
        * @param TBool aCurrentRats: Use Current RAT setting
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt NetRatReq( TUint8 aTransId, TBool aCurrentRats );

        /**
        * Breaks received NET_RAT_RESP ISI message
        * @param TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void NetRatResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Construct a GSS_CS_SERVICE_REQ message and send it through phonet.
        * @param TUint8 aTransId: Transaction Id
        * @param RMmCustomAPI::TNetworkModeCaps: System network mode to set.
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt GssCsServiceReq(
            TUint8 aTransId,
            RMmCustomAPI::TNetworkModeCaps aNetworkModeCaps );

        /**
        * Request for current RAT
        * @param TUInt8 aTransId: Transaction Id
        * @return TInt : result
        */
        TInt GssCsServiceReq( TUint8 aTransId );

        /**
        * Breaks received GSS_CS_SERVICE_RESP ISI message
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @return void : None
        */
        void GssCsServiceResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Breaks received GSS_CS_SERVICE_FAIL_RESP ISI message
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @return void : None
        */
        void GssCsServiceFailResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the ReadViagHomeZoneParamsResp response message.
        * @param const TIsiReceiveC& aIsiMessage: a pointer to incoming message
        * @return void
        */
        void ReadViagHomeZoneParamsRespL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the ReadViagHomeZoneCacheResp response message.
        * @param const TIsiReceiveC& aIsiMessage: a pointer to incoming message
        * @return void
        */
        void ReadViagHomeZoneCacheResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the WriteViagHomeZoneCacheResp response message.
        * @param const TIsiReceiveC& aIsiMessage: a pointer to incoming message
        * @return void
        */
        void WriteViagHomeZoneCacheResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Construct and send to phonet GSS_HSXPA_USER_SETTING_WRITE_REQ
        * ISI message.
        * @param TUint8 aTraId: a transaction identification
        * @param THSxPAStatus status:
        * @return TInt: error code
        */
        TInt WriteHSxPAStatusReq(
            TUint8 aTraId,
            RMmCustomAPI::THSxPAStatus status );

        /**
        * Breaks GSS_HSXPA_USER_SETTING_WRITE_RESP ISI message
        * @param const TIsiReceiveC& aIsiMessage: the incoming message
        * @return void
        */
        void WriteHSxPAStatusResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Construct and send to phonet GSS_HSXPA_USER_SETTING_READ_REQ
        * ISI message.
        * @param TUint8 aTraId: a transaction identification
        * @return TInt: error code
        */
        TInt ReadHSxPAStatusReq( TUint8 aTraId );

        /**
        * Breaks GSS_HSXPA_USER_SETTING_READ_RESP ISI message
        * @param const TIsiReceiveC& aIsiMessage: the incoming message
        * @return void
        */
        void ReadHSxPAStatusResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Breaks received GSS_HSXPA_SETTING_IND ISI messages
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void : None
        */
        void HSxPASettingInd( const TIsiReceiveC& aIsiMsg );

        /**
        * Construct a NET_NEIGHBOUR_CELLS_REQ message and send it through phonet.
        * @param TUint8 aTraId: a transaction identification
        * @return TInt: error code
        */
        TInt NetNeighbourCellsReq( const TUint8 aTraId );

        /**
        * Breaks received NET_NEIGHBOUR_CELLS_RESP ISI messages.
        * @param const TIsiReceiveC& aIsiMsg: The received ISI message
        * @return void: None
        */
        void NetNeighbourCellsResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Breaks received NET_REG_STATUS_IND or NET_REG_STATUS_IND
        * message and checks if enhanced cell information is chaged
        * ( => needs to be requested/updated).
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @return void: None
        */
        void CheckECIDInfo( const TIsiReceiveC& aIsiMessage );

// Not suppoted for S60 ver 3.2
#if ( NCP_COMMON_S60_VERSION_SUPPORT != S60_VERSION_32 )

        /**
        * Construct a GSS_CS_SERVICE_REQ message including selected
        * band mode and send it through phonet.
        * @param TUint8 aTransId: Transaction Id
        * @param RMmCustomAPI::TNetworkModeCaps: System network mode to set.
        * @param RMmCustomAPI::TBandSelection: Selected band mode.
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt GssCsServiceSetBandReq(
            TUint8 aTransId,
            RMmCustomAPI::TNetworkModeCaps aNetworkModeCaps,
            RMmCustomAPI::TBandSelection aBandSelection );

        /**
        * Construct a GSS_CS_SERVICE_REQ message to get selected
        * band mode.
        * @param TUint8 aTransId: Transaction Id
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt GssCsServiceGetBandReq( TUint8 aTransId );

        /**
        * Breaks received GSS_SELECTED_BANDS_READ operation.
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @return void: None
        */
        void CompleteGetBandSelection( const TIsiReceiveC& aIsiMessage );

#endif // NCP_COMMON_S60_VERSION_SUPPORT

        /**
        * Construcst PERM_PM_RECORD_READ_REQ message sends it through
        * phonet.
        * @param TUint8 aTransId: Transaction id.
        * @return TInt: KErrNone / Error value from phonet.
        */
        TInt ReadLifeTimerFromPermanentMemory( const TUint8 aTransId );

        /**
        * Breaks a PERM_PM_RECORD_READ_RESP ISI-message.
        * @param const TIsiReceiveC& aIsiMessage: Received isi message.
        * @return void: none
        */
        void LifeTimerFromPermanentMemoryReadResponse(
                 const TIsiReceiveC& aIsiMessage );

        /**
        * Construcst PERM_PM_RECORD_WRITE_REQ message sends it through
        * phonet.
        * @param TUint8 aTransId: Transaction id.
        * @return TInt: KErrNone / Error value from phonet / KErrNotReady if PMM
        * has not been able to be read.
        */
        TInt WriteLifeTimerToPermanentMemory(
                 const TUint8 aTransId,
                 TTimeIntervalSeconds aTime );

        /**
        * Breaks a PERM_PM_RECORD_WRITE_RESP ISI-message.
        * @param const TIsiReceiveC& aIsiMessage: Received isi message.
        * @return void: none
        */
        void LifeTimerToPermanentMemoryWriteResponse(
                 const TIsiReceiveC& aIsiMessage );

        /**
        * Reads total call life timer value and returns it in complete.
        * If
        * @param none
        * @return TInt: KErrNone / KErrNotReady if PMM has not been able to be
        * read.
        */
        TInt  GetTotalLifeTimerValue();

        /**
        * Constructs and sends AUTHENTICATE APDU to the UICC
        * @param aDataPackage: data containing authentication parameters
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt CMmCustomMessHandler::UiccSendAuthenticateApdu(
            const CMmDataPackage& aDataPackage );


        /**
        * Constructs RUN GSM ALGORITHM APDU. Used when authentication
        * is done for ICC card
        * @param aParams: parameter where the apdu is constructed
        * @param aRand: RAND value inserted to the APDU
        * @param aTraId: tr id used in UICC request
        * @return void: none
        */
        void UiccCreateRunGsmAlgorithmApdu(
            TUiccSendApdu& aParams,
            const TDesC8& aRand,
            TUiccTrId aTraId );

        /**
        * Handles response APDU for RUN GSM ALGORITHM APDU
        * @param aTraId: transaction id of received message
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void UiccRunGsmAlgorithmApduResp(
            TInt aTraId,
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Constructs GSM context AUTHENTICATE APDU
        * @param params: parameter where the apdu is constructed
        * @param aDataPackage: data containing authentication parameters
        * @return void: none
        */
        void UiccCreateGsmSecurityContextApdu(
            TUiccSendApdu& params,
            const CMmDataPackage& aDataPackage );

        /**
        * Handles response APDU for GSM security context AUTHENTICATE APDU
        * @param aTraId: transaction id of received message
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void UiccGsmSecurityContextApduResp(
            TInt aTraId,
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Constructs 3G security context AUTHENTICATE APDU
        * @param aParams: parameter where the apdu is constructed
        * @param aRand: RAND value inserted to the APDU
        * @param aAuth: AUTH value inserted to the APDU
        * @param aTraId: tr id used in UICC request
        * @return void: none
        */
        void UiccCreate3GSecurityContextApdu(
            TUiccSendApdu& aParams,
            const TDesC8& aRand,
            const TDesC8& aAuth,
            TUiccTrId aTraId );

        /**
        * Handles response APDU for 3G security context AUTHENTICATE APDU
        * @param aTraId: transaction id of received message
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void Uicc3GSecurityContextApduResp(
            TInt aTraId,
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Constructs GBA security context AUTHENTICATE APDU (bootstrapping mode)
        * @param aParams: parameter where the apdu is constructed
        * @param aDataPackage: data containing authentication parameters
        * @return void: none
        */
        void UiccCreateGBABootstrappingApdu(
            TUiccSendApdu& aParams,
            const CMmDataPackage& aDataPackage );

        /**
        * Handles response APDU for GBA security context AUTHENTICATE
        * APDU (bootstrapping mode)
        * @param aTraId: transaction id of received message
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void UiccGBABootstrappingApduResp(
            TInt aTraId,
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Starts GBA bootstrap operation by reading elementary file EFgba. After reading
        * is done, B-Tid and keylifetime is updated to EFgba
        * @param aDataPackage: data containing parameters going to be written
        *                      to the EFgba
        * @return void: none
        */
        void UiccGBABootstrapUpdate(
            const CMmDataPackage& aDataPackage );

        /**
        * Handles response for EFgba read operation
        * @param aStatus: status of the response
        * @param aFileData: file data
        * @return void: none
        */
        void UiccGBABootstrapReadResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Handles response for EFgba write operation
        * @param aStatus: status of the response
        * @return void: none
        */
        void UiccGBABootstrapUpdateResp( TInt aStatus );

        /**
        * Constructs GBA security context AUTHENTICATE APDU
        * (NAF derivation mode)
        * @param aParams: parameter where the apdu is constructed
        * @param aDataPackage: data containing authentication parameters
        * @return void: none
        */
        void UiccCreateGBABootstrapNafDerivationApdu(
            TUiccSendApdu& aParams,
            const CMmDataPackage& aDataPackage );

        /**
        * Handles response APDU for GBA security context AUTHENTICATE
        * APDU (NAF derivation mode)
        * @param aTraId: transaction id of received message
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void UiccGBANafDerivationApduResp(
            TInt aTraId,
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Constructs MBMS security context AUTHENTICATE APDU
        * (MSK Update Mode)
        * @param params: parameter where the apdu is constructed
        * @param aDataPackage: data containing authentication parameters
        * @return void: none
        */
        void UiccCreateMbmsMskUpdateApdu(
            TUiccSendApdu& params,
            const CMmDataPackage& aDataPackage );

        /**
        * Handles response APDU for MBMS security context AUTHENTICATE
        * APDU (MSK Update Mode)
        * @param aTraId: transaction id of received message
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void UiccMbmsMskUpdateApduResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Constructs MBMS security context AUTHENTICATE APDU
        * (MTK Generation Mode)
        * @param params: parameter where the apdu is constructed
        * @param aDataPackage: data containing authentication parameters
        * @return void: none
        */
        void UiccCreateMbmsMtkGenerationApdu(
            TUiccSendApdu& params,
            const CMmDataPackage& aDataPackage );

        /**
        * Handles response APDU for MBMS security context AUTHENTICATE
        * APDU (MTK Generation Mode)
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void UiccMbmsMtkGenerationApduResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Constructs MBMS security context AUTHENTICATE APDU
        * (MSK Deletion Mode)
        * @param params: parameter where the apdu is constructed
        * @param aDataPackage: data containing authentication parameters
        * @return void: none
        */
        void UiccCreateMbmsMskDeletionApdu(
            TUiccSendApdu& params,
            const CMmDataPackage& aDataPackage );

        /**
        * Handles response APDU for MBMS security context AUTHENTICATE
        * APDU (MSK Deletion Mode)
        * @param aStatus: status of the response
        * @param aFileData: response apdu
        * @return void: none
        */
        void UiccMbmsMskDeletionApduResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Constructs APDU to get first block of authenticate response data
        * in case of odd ins code
        * @return void: none
        */
        void CMmCustomMessHandler::UiccCreateFirstBlockOfAuthRespApdu( TUiccTrId aTrId );

        /**
        * Maps sw1 and sw2 from response authenticate apdu to result
        * @param sw1: status of the response
        * @param sw2: response apdu
        * @return TUint8: command result
        */
        TUint8 MapSw1Sw2ToAuthenticateResult( TUint8 sw1, TUint8 sw2 );

        /**
        * Validates received apdu
        * @param aTraId: Identifies the apdu
        * @param aApdu: APDU to be verified
        * @return TUint8: KApduOk if verified successfully,
        *                 otherwise KApduNok
        */
        TUint8 ValidateReceivedAuthenticateApdu(
            TInt aTraId,
            const TDesC8& aApdu );

        /**
        * Validates received gsm security context apdu for authenticate
        * @param aApdu: APDU to be verified
        * @return TUint8: KApduOk if verified successfully,
        *                 otherwise KApduNok
        */
        TUint8 ValidateGsmSecurityContextApduResp( const TDesC8& aApdu );

        /**
        * Validates received 3G security context apdu for authenticate
        * @param aApdu: APDU to be verified
        * @return TUint8: KApduOk if verified successfully,
        *                 otherwise KApduNok
        */
        TUint8 Validate3GSecurityContextApduResp( const TDesC8& aApdu );

        /**
        * Validates received GBA security context apdu (GBA bootstrapping mode)
        * for authenticate
        * @param aApdu: APDU to be verified
        * @return TUint8: KApduOk if verified successfully,
        *                 otherwise KApduNok
        */
        TUint8 ValidateGBABootstrappingApduResp( const TDesC8& aApdu );

        /**
        * Validates received GBA security context apdu (GBA NAF derivation mode)
        * for authenticate
        * @param aApdu: APDU to be verified
        * @return TUint8: KApduOk if verified successfully,
        *                 otherwise KApduNok
        */
        TUint8 ValidateGBANafDerivationApduResp( const TDesC8& aApdu );

        /**
        * Validates received RUN GSM ALGORITHM apdu for authenticate
        * @param aApdu: APDU to be verified
        * @return TUint8: KApduOk if verified successfully,
        *                 otherwise KApduNok
        */
        TUint8 ValidateRunGsmAlgorithmApduResp( const TDesC8& aApdu );

        /**
        * Handles MBMS operation data from received MSK Update
        * authenticate APDU
        * @param aMskUpdate: parameters from MBMS operation data are
        *                    inserted to aMskUpdate
        * @param aMbmsData: MBMS data got from MSK Update authenticate
        *                   APDU
        * @return void: none
        */
        void UiccMskUpdateHandleMbmsOperationData(
            RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aMskUpdate,
            TDesC8& aMbmsData );

        /**
        * Handles OMA BCAST operation data from received authenticate
        * APDU
        * @param aMskUpdate: parameters from MBMS operation data are
        *                    inserted to aMskUpdate
        * @param aMbmsData: APDU data
        * @return void: none
        */
        void UiccMskUpdateHandleOMABcastOperationData(
            RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aMskUpdate,
            TDesC8& aMbmsData );

        /**
        * Handles MBMS operation data from received MTK Generation
        * authenticate APDU
        * @param aMskUpdate: parameters from MBMS operation data are
        *                    inserted to aMskUpdate
        * @param aMbmsData: APDU data
        * @return void: none
        */
        void UiccMtkGenHandleMbmsOperationData(
            RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aMtkGen,
            TDesC8& aMbmsData );

        /**
        * Handles OMA BCAST operation data from received MTK Generation
        * authenticate APDU
        * @param aMtkGen: parameters from MBMS operation data are
        *                    inserted to aMtkGen
        * @param aMbmsData: APDU data
        * @return void: none
        */
        void UiccMtkGenHandleOMABcastOperationData(
            RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aMtkGen,
            TDesC8& aMbmsData );

        /**
        * Finds tlv objects from BER TLV object
        * @param aTlvTag: tag of teh tlv object to be search
        * @param aBerTlv: BER TLV object where to search
        * @param aTlvObject: tlv object data is copied to this
        *                    parameter, if it's found
        * @return TBool: ETrue if tlv object found, otherwise EFalse
        */
        TBool FindTlvObject(
            TUint8 aTlvTag,
            const TDesC8& aBerTlv,
            TDes8& aTlvObject );

        /**
        * calculates the UMTS Ciphering Key (CK) from the GSM
        * Ciphering Key (Kc). This is done by using the c4 algorithm defined in
        * 3GPP TS 33.102 v3.8.0 (Release 1999).
        * @param aCk: Result is inserted to this parameter
        * @param aKc:     Kc which is used in calculation
        * @return void: none
        */
        void DeriveCkFromKc(
            TDes8& aCk,
            const TDesC8& aKc );

        /** derives the UMTS Integrity Key (IK) from the GSM
        * Ciphering Key (Kc). This is done by using the c5 algorithm defined in
        * 3GPP TS 33.102 v3.8.0 (release 1999).
        * @param aEapAka: Result is inserted to this parameter
        * @param aKc:     Kc which is used in calculation
        * @return void: none
        */
        void DeriveIkFromKc(
            TDes8& aIk,
            const TDesC8& aKc );

        /** Handles the IMS authentication
        * @param aDataPackage: data containing authentication parameters
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt UiccHandleImsAuthentication( const CMmDataPackage& aDataPackage );

        /**
        * Handles response for ISim application activation
        * @param aStatus: status of the response
        * @return void: none
        */
        void UiccHandleIsimActivationResp( TInt aStatus );

        /**
         * Collects cellbroadcast topic IDs into array. Leaves if nothig found
         * @param aFileData elementary file data
         * @return array with topic IDs
         */
        CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* CollectCbTopicIdsL(
                const TDesC16& aTopicIds ) const;

    protected: // Data

        // Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

    private: // Data

        // Pointer to the Message router
        CMmMessageRouter* iMessageRouter;

        // Pointer to the Net message handler
        CMmNetMessHandler* iNetMessHandler;

        // Pointer to the Security message handler
        CMmSecurityMessHandler* iSecurityMessHandler;

        // Release cause value sent
        TUint8 iReleaseCauseValueSent;

        // List of Ciphering values, for buffering purpose
        RArray<TCiphListEntry> *iListOfCiphValues;

        // Is indication a refresh indication
        TBool iIsRefresh;

        // Emergency number checking mode
        RMmCustomAPI::TCheckMode iUsedEmergencyNbrCheckMode;

        // Lenght of the emergency number to be checked
        TInt iEmergencyNumberLength;

        // Buffer to hold RAND_PARAMS subblock data
        TBuf8<20> iRandBuf;

        // Buffer to hold AUTN_PARAMS subblock data
        TBuf8<20> iAutnBuf;

        // Buffer to hold B-Tid data
        TBuf8<256> iBTidBuf;

        // Buffer to hold keylifetime data
        TBuf8<256> iKeyLifetimeBuf;

        // Buffer to hold SIM_SB_AM_GBA_NAF_ID_ID subblock data
        TBuf8<254> iNafIdBuf;

        // Buffer to hold SIM_SB_IMS_PRIVATE_USER_ID subblock data
        TBuf8<254> iImpiBuf;

        // Buffer for Mikey data.
        RMmCustomAPI::TMikey iMikey;

        // Buffer for key domain id data
        RMmCustomAPI::TKeyDomainId iKeyDomainId;

        // Buffer for key group id data
        RMmCustomAPI::TKeyGroupId iKeyGroupIdPart;

        // Viag Home Zone Parameters
        RMmCustomAPI::TViagParams iViagParams;

        // Cb msg ids max count
        TInt iCbMsgIdsMaxCount;

        // Saves the SIM CB topic number for deletion
        TUint16 iSimCBTopicToBeDeleted;

        // Is the SIM topic is being deleted (ETrue ) or not (EFalse)
        TBool iTopicInSimMemoryDelete;

        // Enhanced Cell Information
        TECIDInfo iECIDInfo;

        // Is ECIDInfo requested
        TBool iECIDInfoRequested;

        // Call Life Timer;
        TInt iCallLifeTimer;

        // Conter for record retries from PMM during read.
        TUint8 iPMMReadRetryCounter;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

        //keeps track...
        TUint8 iSapApduIfState;

        // is Isim application already activated
        TBool iIsimApplActivated;

    public: // Friend classes
        //None
    protected: // Friend classes
        //None
    private: // Friend classes
        //None
    };

#endif // _CMMCUSTOMMESSHANDLER_H

// End of File
