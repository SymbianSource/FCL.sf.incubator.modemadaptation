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




//  INCLUDES
#ifndef CMMSUPPLSERVMESSHANDLER_H
#define CMMSUPPLSERVMESSHANDLER_H

#include <e32base.h> // Symbian base types
#include <etelmm.h>  // ETel base types
#include <ctsy/rmmcustomapi.h>

#include "cmmphonetreceiver.h"
#include "mmmmesshandlerbase.h"
#include "muiccoperationbase.h"

// CONSTANTS
    //none

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class CMmDataPackage;
class CMmMessageRouter;
class CMobilePhoneCFList;
class CMobilePhoneCBList;
class CMobilePhoneCWList;
class CMmPhoNetSender;
class TIsiReceiveC;
class CMmUiccMessHandler;

// CLASS DECLARATION

// DESCRIPTION

/**
* Simple data container to be passed via function parameters
*/
class TPreprocessedSsServiceCompleteResponse
    {
    public:
        TPreprocessedSsServiceCompleteResponse(
            const TIsiReceiveC& aIsiMessage ) :
            iIsiMessage( aIsiMessage )
            {}
    public:
        const TIsiReceiveC& iIsiMessage;
        TUint8 iTraId;
        TUint8 iNumSubBlocks;
        TUint8 iOperation;
        TUint8 iBasicService;
        TUint16 iSsCode;
    };

// DESCRIPTION

/**
* Used for creating and sending SS ISI messages to
* PhoNet via PhoNetSender. It also receives SS
* ISI messages from PhoNetReceiver.
*
*  @lib (unknown)
*  @since 2.6
*/
class CMmSupplServMessHandler
    : public CBase, public MMmMessHandlerBase, public MMmMessageReceiver,
    public MUiccOperationBase
    {
    public: // types
        // none

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPhoNetSender Pointer to the Phonet sender
        * @param aPhoNetReceiver Pointer to the phonet receiver
        * @param aMessageRouter Pointer to the message router
        * @return created message handler object
        */
        static CMmSupplServMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler );

        /**
        * Destructor.
        */
        ~CMmSupplServMessHandler();

    public: // Methods from base classes

        /**
        * From MMmMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param aIpc IPC number of the request
        * @return TInt KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
         * Handle UICC response messages
         * @param aTraId Transaction ID
         * @param aFileData File data
         * @return KErrNone or error code
         */
         TInt ProcessUiccMsg(
             TInt aTraId,
             TInt aStatus,
             TUint8 aDetails,
             const TDesC8& aFileData );

    public: // New functions

        /**
        * Handles a received message by calling the specific
        * message handling method.
        * @param aIsiMessage, reference to the received message.
        */
        void ReceiveMessageL( const TIsiReceiveC &aIsiMessage );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param CIsiMsg* aIsiMsg: The received ISI message
        * @param TInt aError: Error code
        */
        void HandleError( const TIsiReceiveC&, TInt aError );

        /**
        * Maps Symbian OS number type to ISI values
        * @param aMmTypeOfNumber Symbian OS type of number
        * @param aMmNumberingPlan Symbian OS numbering plan
        * @return aIsiTypeOfNumber ISI type of number
        */
        static void MapTypeOfNumberMmToIsi(
            RMobilePhone::TMobileTON aMmTypeOfNumber,
            RMobilePhone::TMobileNPI aMmNumberingPlan,
            TUint8& aIsiTypeOfNumber );

        /**
        * Updates internal variable that keeps track of which ALS line is used
        * @param RMobilePhone::TMobilePhoneALSLine& aAlsLine : ALS line
        * @param return void: None
        */
        void SetVoiceCallForwardLine(
            RMobilePhone::TMobilePhoneALSLine& aAlsLine );

    private: // New functions

        /**
        * Completes NotifySendNetworkServiceRequest in error case
        * @param const TIsiReceiveC &aIsiMessage  Reference to the received
        *  message.
        */
        void SsStatusInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Completes NotifySendNetworkServiceRequest in succesful case
        * @param const TIsiReceiveC &aIsiMessage  Reference to the received
        *  message.
        */
        void SsCompleteInd( const TIsiReceiveC &aIsiMessage );

    private: // types

        // The SS operations are grouped into
        // the following types. Used as ISA transaction IDs.
        enum TSSOperationType
            {
            ESSOperationTypeUnknown = 0,
            ESSOperationTypeGetCallForwardingStatus,
            ESSOperationTypeSetCallForwardingStatus,
            ESSOperationTypeGetCallBarringStatus,
            ESSOperationTypeSetCallBarringStatus,
            ESSOperationTypeGetCallWaitingStatus,
            ESSOperationTypeSetCallWaitingStatus,
            ESSOperationTypeSetSSPassword,
            ESSOperationTypeGetIdentityServiceStatus,
            ESSOperationTypeSendNetworkServiceRequest,
            ESSOperationTypeSendNetworkServiceRequestNoFdnCheck
            };

    private: // Constructors

        /**
        * C++ default constructor.
        */
        CMmSupplServMessHandler();

        /**
        * 2nd phase constructor
        */
        void ConstructL();

    private: // New methods

        /**
        * Construct a SS_SERVICE_NO_SIMATK_REQ ISI message and adds
        * SS_GSM_SIMATK_INFO subblock to message
        * @param TUint8 aTrId: transaction id
        * @param TUint8 aOperation: operation type
        * @param TUint16 aServiceCode: SS code
        * @param TUint8 aBasicServiceCode: basic service code
        * @param TDesC& ssInfoA: service info A
        * @return TInt: KErrNone or error code
        */
        TInt CreateServiceReqWithCheckInfo(
            const TUint8 aTrId,
            const TUint8 aOperation,
            const TUint16 aServiceCode,
            const TUint8 aBasicServiceCode );

        /**
        * Calls appropiate function for creating SS_SERVICE_NO_SIMATK_REQ,
        * based on SS code
        * @param  TUint8 aTrId: transaction id
        * @param  TInt aOperation: operation type
        * @param  TInt aServiceCode: SS code
        * @param  TDesC& ssInfoA: SS info A
        * @param  TDesC& ssInfoB: SS info B
        * @param  TDesC& ssInfoC: SS info C
        * @return TInt: KErrNone or error code
        */
        TInt SsServiceReqSatOriginated(
            const TUint8 aTrId,
            const TUint8 aOperation,
            const TUint16 aServiceCode,
            const TDesC& ssInfoA,
            const TDesC& ssInfoB,
            const TDesC& ssInfoC );

        /**
        * Construct a SS_REQUEST ISI message for Call Forwarding request.
        * @param TUint8 aTransactionId: transaction identifier
        * @param TUint8 aOperation: operation
        * @param TUint16 aMmiSsCode: SS code
        * @param TUint8 aTypeOfNumber: type of number
        * @param TInt aNoReplyTime: reply timer
        * @param TDesC16 const * aForwToNumber: number for setting call forwarding
        * @param TUint8 aBasicServiceCode: SS basic service code
        * @return TInt : KErrNone or error code
        */
        TInt SsCallForwServiceReq(
            TUint8 aTransactionId,
            TUint8 aOperation,
            TUint16 aMmiSsCode,
            TUint8 aTypeOfNumber,
            TInt aNoReplyTime,
            TDesC16 const* aForwToNumber,
            TUint8 aBasicServiceCode );

        /**
        * Breaks a SS_SERVICE_COMPLETED_RESP ISI-message and gets GSM specific
        * sub blocks.
        * @param CIsiMsg* aIsiMsg: isi message
        * @return void : None
        */
        void SsServiceCompletedRespL( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks a SS_SERVICE_FAILED_RESP ISI-message and gets data from
        * GSM specific sub blocks.
        * @param CIsiMsg* aIsiMsg: isi message
        * @return void : None
        */
        void SsServiceFailedResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks a SS_SERVICE_NOT_SUPPORTED_RESP ISI-message and gets data from
        * GSM specific sub blocks.
        * @param CIsiMsg* aIsiMsg: isi message
        * @return void : None
        */
        void SsServiceNotSupportedResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks a SS_GSM_FORWARDING sub block.
        * @param CSsSubBlock* aSubBlock: Ss sub block
        * @param TUint16 aSsCode: SS condition
        * @return void : None
        */
        void HandleSsGsmForwardingSubBlockL(
            const TIsiReceiveC &aIsiMessage,
            TUint aSbStartOffset,
            TUint16 aSsCode );

        /**
        * Breaks a SS_BSC_INFO sub block which is coming with a
        * Call Barring status response
        * @param const TIsiReceiveC &aIsiMessage
        * @param TUint aSbStartOffset
        * @param TUint16 aSsCode: service code (barring condition)
        * @return void : None
        */
        void HandleBarringSsGsmBscInfoSubBlockL(
            const TIsiReceiveC &aIsiMessage,
            TUint aSbStartOffset,
            TUint16 aSsCode );

        /**
        * Breaks a SS_BSC_INFO sub block which is coming with a
        * Call Waiting status response
        * @param const TIsiReceiveC &aIsiMessage
        * @param TUint aSbStartOffset: sub block offset
        * @return void : None
        */
        void HandleWaitingSsGsmBscInfoSubBlockL(
            const TIsiReceiveC &aIsiMessage,
            TUint aSbStartOffset );

        /**
        * Description: construct a SS_SERVICE_REQ ISI message with
        * SS_GSM_PASSWORD sub block for Call Barring request.
        * @param TUint8 aTransactionId: transaction identifier
        * @param TUint8 aOperation: operation
        * @param TDesC16 const * aOldPassword: Old password
        * @param TDesC16 const & aNewPassword: New password
        * @param TDesC16 const & aVerifyPassword: New password for verification
        * @param TUint8 aBasicServiceCode: Mmi basic service code
        * @return TInt : KErrNone or error code
        */
        TInt SsGsmBarringReq(
            TUint8 aTransactionId,
            TUint8 aOperation,
            TUint16 aMmiSsCode,
            TDesC16 const * aOldPassword,
            TDesC16 const & aNewPassword,
            TDesC16 const & aVerifyPassword,
            TUint8 aBasicServiceCode );

        /**
        * Construct a SS_SERVICE_REQ ISI-message for Call Waiting
        * request.
        * @param TUint8 aTransactionId: transaction identifier
        * @param TUint8 aOperation: operation
        * @param TUint16 aMmiSsCode: Mmi Ss code
        * @param TUint8 aBasicServiceCode: Mmi basic service code
        * @return TInt : KErrNone or error code
        */
        TInt SsGsmWaitingReq(
            TUint8 aTransactionId,
            TUint8 aOperation,
            TUint16 aMmiSsCode,
            TUint8 aBasicServiceCode );

        /**
        * Construct a  SS_SERVICE_REQ ISI-message for Caller Identify
        * (CLIP and CLIR), Connected Line Identify (COLP and COLR)
        * and Caller Name Presentation (CNAP).
        * @param TUint8 aTransactionId: transaction identifier
        * @param TUint16 aMmiSsCode: Mmi Ss code
        * @return TInt : KErrNone or error code
        */
        TInt SsIdentityServiceReq( TUint8 aTransactionId, TUint16 aMmiSsCode );

        /**
        * Completes a failed request to SOS layer.
        * @param TUint8 aTransactionId: the response's transaction ID
        * @param  TInt aError: Symbian error code
        */
        void CompleteFailedSSRequest( TUint8 aTransactionId, TInt aError );

        /**
        * Creates a SS GSM USSD request
        * @param TUint8 aTransactionId: transaction identifier
        * @param TDesC8* aString: Service string
        * @return TInt : KErrNone or error code
        */
        TInt SsGsmUssdSendReq( TUint8 aTransactionId, TDesC8* aString );

        /**
        * Breaks a SS_GSM_USSD_SEND_RESP ISI-message.
        * @param CIsiMsg* aIsiMsg: isi message
        * @return void : None
        */
        void SsGsmUssdSendResp( const TIsiReceiveC &aIsiMessage );

        /**
         * Read call forwarding flags REL4
         * @return KErrNone or error code
         */
        TInt UiccReadCallFwdFlagsRel4Req();

        /**
         * Read call forwarding flags CPHS
         * @return KErrNone or error code
         */
        TInt UiccReadCallFwdFlagsCphsReq();

         /**
         * Read call forwarding flags response
         * @param aStatus Status
         * @param aTraId Transaction ID
         * @param aFileData File data
         * @return None
         */
        void UiccHandleCallFwdFlagsResp(
            TInt aStatus,
            TInt aTrId,
            const TDesC8& aFileData );

        /**
         * Write call forwarding flags REL4
         * @param aVoiceStatus voice flag value
         * @param aFaxStatus fax flag value
         * @param aDataStatus data flag status
         * @return KErrNone or error code
         */
        TInt UiccWriteCallFwdFlagsRel4Req(
            TUint8 aVoiceStatus,
            TUint8 aFaxStatus,
            TUint8 aDataStatus );

        /**
         * Write call forwarding flags CPHS
         * @param aVoiceLine1Status voice line 1 flag value
         * @param aVoiceLine2Status voice line 2 flag value
         * @param aFaxStatus fax flag value
         * @param aDataStatus data flag value
         * @return KErrNone or error code
         */
        TInt UiccWriteCallFwdFlagsCPHSReq(
            TUint8 aVoiceLine1Status,
            TUint8 aVoiceLine2Status,
            TUint8 aFaxStatus,
            TUint8 aDataStatus );

        /**
         * Read voice message flags REL4
         * @return KErrNone or error code
         */
        TInt UiccReadVoiceMsgFlagsRel4Req();

        /**
         * Read voice message flags CPHS
         * @return KErrNone or error code
         */
        TInt UiccReadVoiceMsgFlagsCphsReq();

        /**
         * Read voice message flags response
         * @param aTraId Transaction ID
         * @param aFileData Descriptor for file data
         * @return None
         */
        void UiccReadVoiceMsgFlagsResp(
            TInt aTrId,
            const TDesC8& aFileData );

        /**
         * Write voice message flags REL4
         * @param msgWaiting Voice message flags
         * @return KErrNone or error code
         */
        TInt UiccWriteVoiceMsgFlagsRel4Req(
            const RMobilePhone::TMobilePhoneMessageWaitingV1& msgWaiting );

        /**
         * Write voice message flags CPHS
         * @param msgWaiting Voice message flags
         * @return KErrNone or error code
         */
        TInt UiccWriteVoiceMsgFlagsCPHSReq(
            const RMobilePhone::TMobilePhoneMessageWaitingV1& msgWaiting );

        /**
        * Breaks received SS_SERVICE_COMPLETED_IND ISI message
        * @param CIsiMsg* aIsiMsg: isi message
        * return void : None
        */
        void SsServiceCompletedInd ( const TIsiReceiveC &aIsiMessage );

        // static utility methods

        /**
        * Map mobile service to isi basic service codes
        * @param RMobilePhone::TMobileService aMobileService: multimode api
        * mobile service
        * @param TUint8 *aIsiBasicServiceCode: isi basic service code
        * @return TInt: error value
        */
        TInt MapMobileServiceToBasicServiceCodeIsi(
            RMobilePhone::TMobileService aMobileService,
            TUint8* aIsiBasicServiceCode );

        /**
        * Map multimode api call forwarding conditon to isi call forwarding conditon
        * @param RMobilePhone::TMobilePhoneCFCondition aMmCFCondition: multimode
        * api call forw. condition
        * @param TUint16* aCFConditon: isi call forw. condition
        * @return TInt: error value
        */
        static TInt MapCFConditionMmToIsi(
            RMobilePhone::TMobilePhoneCFCondition aMmCFCondition,
            TUint16* aCFCondition );

        /**
        * Map type of number from isi type to multimode api type
        * @param TUint8 aIsiTypeOfNumber: isi number type
        * @param RMobilePhone::TMobileTON* aMmTypeOfNumber: number type
        * @param RMobilePhone::TMobileNPI* aMmNumberingPlan: numbering plan
        * @return TInt: error value
        */
        static TInt MapTypeOfNumberIsiToMm(
            TUint8 aIsiTypeOfNumber,
            RMobilePhone::TMobileTON* aMmTypeOfNumber,
            RMobilePhone::TMobileNPI* aMmNumberingPlan );

        /**
        * Map call forwarding isi status to multimode api status
        * @param TUint8 aIsiSsStatus: SS Status
        * @param RMobilePhone::TMobilePhoneCFStatus* aMmCFStatus: call forw. status
        * @return None
        */
        static void MapCFStatusIsiToMm(
            TUint8 aIsiSsStatus,
            RMobilePhone::TMobilePhoneCFStatus* aMmCFStatus );

        /**
        * Map call waiting isi status to multimode api status
        * @param TUint8 aIsiSsStatus: SS Status
        * @param RMobilePhone::TMobilePhoneCWStatus* aMmCWStatus: call waiting status
        * @return None
        */
        static void MapCWStatusIsiToMm(
            TUint8 aIsiSsStatus,
            RMobilePhone::TMobilePhoneCWStatus* aMmCWStatus );

        /**
        * Map isi basic service code to multimode api basic service code
        * @param TUint8 aIsiBasicServiceCode: isi basic service code
        * @param RMobilePhone::TMobileService* aMmBasicServiceCode: multimode api
        * basic service code
        * @return TInt: error value
        */
        TInt MapBasicServiceCodeIsiToMm(
            TUint8 aIsiBasicServiceCode,
            RMobilePhone::TMobileService* aMmBasicServiceCode );

        /**
        * Map isi call forwarding conditon to multimode api call forwarding conditon
        * @param TUint16 aCFConditon: isi call forw. condition
        * @param RMobilePhone::TMobilePhoneCFCondition* aMmCFCondition: multimode
        * api call forw. condition
        * @return None
        */
        static void MapCFConditionIsiToMm(
            TUint16 aCFConditon,
            RMobilePhone::TMobilePhoneCFCondition* aMmCFCondition );

        /**
        * Map isi call forwarding timeout to multimode api call forwarding timeout
        * @param TUint8 aISITimeout: isi timeout
        * @param TInt* aMmTimeout: multimode api timeout
        * @return None
        */
        static void MapTimeoutIsiToMm( TUint8 aISITimeout, TInt* aMmTimeout);

        /**
        * Map multimode api operation to isi operation
        * @param RMobilePhone::TMobilePhoneServiceAction aMmOperation: multimode
        * api service action
        * @param TUint8* aIsiOperation: isi operation
        * @return TInt: error value
        */
        static TInt MapOperationMmToIsi(
            RMobilePhone::TMobilePhoneServiceAction aMmOperation,
            TUint8* aIsiOperation );

        /**
        * Map isi call barring conditon to multimode api call barring conditon
        * @param TUint16 aCBConditon: isi call barring condition
        * @param RMobilePhone::TMobilePhoneCBCondition* aMmCBCondition: multimode
        * api call barring condition
        * @return None
        */
        static void MapCBConditionIsiToMm(
            TUint16 aCBConditon,
            RMobilePhone::TMobilePhoneCBCondition* aMmCBCondition );

        /**
        * Map multimode api call barring conditon to isi call barring conditon
        * @param RMobilePhone::TMobilePhoneCBCondition aMmCBCondition: multimode
        * api call barring condition
        * @param TUint16* aCBConditon: isi call barring condition
        * @return TInt: error value
        */
        static TInt MapCBConditionMmToIsi(
            RMobilePhone::TMobilePhoneCBCondition aMmCBCondition,
            TUint16* aCBConditon );

        /**
        * Map isi status to multimode api call barring status
        * @param TUint8 aIsiSsStatus: isi status
        * @param RMobilePhone::TMobilePhoneCBStatus* aMmCFStatus: multimode api
        * status
        * @return None
        */
        static void MapCBStatusIsiToMm(
            TUint8 aIsiSsStatus,
            RMobilePhone::TMobilePhoneCBStatus* aMmCFStatus );

        /**
        * Map multimode api identity service to isi identity service
        * @param RMobilePhone::TMobilePhoneIdService aService: multimode api
        * identity service
        * @param TUint16* aIdentityService: isi identity service
        * @return TInt: error value
        */
        static TInt MapIdentityServiceMmToIsi(
            RMobilePhone::TMobilePhoneIdService aService,
            TUint16* aIdentityService );

        /**
        * Map identity service isi status to multimode status.
        * @param SSsGsmGenericServiceInfo* aIsiIdentityServiceStatus:
        * isi identity service status
        * @param TUint8 aIsiClirOption: isi clir option
        * @param RMobilePhone::TMobilePhoneIdServiceStatus*
        * aMmIdentityServiceStatus: multimode api identity service status
        * @return TInt: error value
        */
        static void MapIdentityServiceStatusIsiToMm(
            TUint8 aIsiIdentityServiceStatus,
            TUint8 aIsiClirOption,
            RMobilePhone::TMobilePhoneIdServiceStatus* aMmIdentityServiceStatus );

        /**
        * Pack text string bytes
        * @param TDes8 &dest: packed message
        * @param TDes8 &src: unpacked message
        * @param TUint byte_count: the number of bytes in source
        * @return TUint16: lenght of the coded message
        */
        static TUint16 GsmLibSmsPackMessage(
            TDes8 &dest,
            TDes8 &src,
            TUint byte_count );

        /**
        * Checks if original SS request is changed or initiated by Call Control
        * @param aResponse: preprocessed data to simplify response processing
        * @return TBool: ETrue if response handled,
        * EFalse if response not handled
        */
        TBool ProcessIfSsRequestChanged(
                const TPreprocessedSsServiceCompleteResponse& aResponse);

        /**
        * Checks if IPC requested allows simple completion
        * @param aResponse: preprocessed data to simplify response processing
        * @return TBool: ETrue if response handled,
        * EFalse if response not handled
        */
        TBool ProcessIfSimpleCompletionPossible(
                const TPreprocessedSsServiceCompleteResponse& aResponse);

        /**
        * Processes SS_GSM_PASSWORD_REGISTRATION response
        * @param aResponse: preprocessed data to simplify response processing
        * @return TBool: ETrue if response handled,
        * EFalse if response not handled
        */
        TBool ProcessSsGsmPasswordRegistration(
                const TPreprocessedSsServiceCompleteResponse& aResponse);

        /**
        * Checks if this is related to call forwarding
        * @param aResponse: preprocessed data to simplify response processing
        * @return TBool: ETrue if response handled,
        * EFalse if response not handled
        */
        TBool ProcessIfCallForwardingL(
                const TPreprocessedSsServiceCompleteResponse& aResponse);

        /**
        * Checks if this is related to call barring
        * @param aResponse: preprocessed data to simplify response processing
        * @return TBool: ETrue if response handled,
        * EFalse if response not handled
        */
        TBool ProcessIfCallBarringL(
               const TPreprocessedSsServiceCompleteResponse& aResponse);

        /**
        * Checks if this is related to call waiting
        * @param aResponse: preprocessed data to simplify response processing
        * @return TBool: ETrue if response handled,
        * EFalse if response not handled
        */
        TBool ProcessIfCallWaitingL(
                const TPreprocessedSsServiceCompleteResponse& aResponse);

        /**
        * Checks if this is related to identity services
        * @param aResponse: preprocessed data to simplify response processing
        * @return TBool: ETrue if response handled,
        * EFalse if response not handled
        */
        TBool ProcessIfIdentityServices(
                const TPreprocessedSsServiceCompleteResponse& aResponse);

    private:    // Data

        //Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        //Pointer to the Message router
        CMmMessageRouter* iMessageRouter;

        // List of RMobilePhone::TMobilePhoneCFInfoEntryV1 entries, containing the
        // results of a GetCallForwardingStatus request
        CMobilePhoneCFList* iCFResults;

        // List of RMobilePhone::TMobilePhoneCBInfoEntryV1 entries, containing the
        // results of a GetCallBarringStatus request
        CMobilePhoneCBList* iCBResults;

        // List of RMobilePhone::TMobilePhoneCWInfoEntryV1 entries, containing the
        // results of a GetCallWaitingStatus request
        CMobilePhoneCWList* iCWResults;

        // The following two flags are needed for service group mappings which
        // are not 1:1 between ETel and ISI. Using these flags is safe:
        //  - All SS operations strictly are a request/response pair
        //  - There cannot be any overlapping SS operations (ensured by
        //    CommonTSY, see CMmSupplServTsy::iSsTransactionOngoing).

        // ETel MM API values RMobilePhone::EVoiceService and
        // RMobilePhone::ETelephony both map to the network-side service
        // "Telephony". If this flag is ETrue, the previous request came with
        // EVoiceService (see MapMobileServiceToBasicServiceCodeIsi).
        // Accordingly, the response must be mapped to EVoiceService again
        // (see MapBasicServiceCodeIsiToMm).
        TBool iVoiceServiceMappedToTelephony;

        // ETel MM API values RMobilePhone::ECircuitDataService and
        // RMobilePhone::EAllBearer both map to the network-side service
        // "All Bearer". If this flag is ETrue, the previous request came with
        // ECircuitDataService (see MapMobileServiceToBasicServiceCodeIsi).
        // Accordingly, the response must be mapped to ECircuitDataService again
        // (see MapBasicServiceCodeIsiToMm).
        TBool iCircuitServiceMappedToAllBearer;

        // Call Forwarding Condition
        RMobilePhone::TMobilePhoneCFCondition iMmCFCondition;

        // Mobile Service
        RMobilePhone::TMobileService iMobileService;

        // Mobile address
        RMobilePhone::TMobileAddress iCFAddress;

        // Stores Call Forwarding Number read from the USIM
        TBuf<RMobilePhone::KMaxMobileTelNumberSize> iCFTelNumberFromSim;

        // Is EMmTsyGetCallForwardingNumberIPC ongoing
        TBool iGetCallForwardingNumber;

        // Is ECustomGetIccCallForwardingStatusIPC ongoing
        TBool iGetIccCallForwardingStatus;

        // Is ECustomGetIccCallForwardingStatusIPC ongoing
        TBool iNotifyIccCallForwardingStatus;

        // Stores Message waiting indicators read from the USIM
        RMobilePhone::TMobilePhoneMessageWaitingV1 iVMIndicators;

        // Stores Call Forwarding indicators read from the USIM
        RMmCustomAPI::TCFIndicators iTCFIndicators;

        // Current ALS line used
        RMobilePhone::TMobilePhoneALSLine iAlsLine;

        //Stores IPC number
        TInt iIpc;

        //Stores SS code
        TUint16 iSsCode;

        // Indicates if SS_GSM_SIMATK_INFO must be inserted
        TBool iCheckInfoSubblock;

        // Indicates if FDN check should be done
        TBool iFdnCheck;

        //Stores voice message flag type
        enum
        {
            EMsgFlagTypeRel4 = 0,
            EMsgFlagTypeCphs
        } iMsgFlagType;

        TBool iResourceControlSuppress;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

    };

#endif // CMMSUPPLSERVMESSHANDLER_H

// End of File
