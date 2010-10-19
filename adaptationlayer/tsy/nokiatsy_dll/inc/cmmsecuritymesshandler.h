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


#ifndef CMMSECURITYMESSHANDLER_H
#define CMMSECURITYMESSHANDLER_H

//  INCLUDES
#include <e32base.h>  // Symbian base types
#include <etelmm.h> //Symbian Etel related types

#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include "nokiatsy_internal_variation.h"
#include "cmmuiccmesshandler.h"

// CONSTANTS
    //none

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class CMmNetMessHandler;
class CMmCallMessHandler;
class CMmPhoNetSender;
class CMmDataPackage;
class TIsiReceiveC;
class CMmMessageRouter;
class CMmUiccMessHandler;
// CLASS DECLARATION

// DESCRIPTION
/**
* Used for creating and sending SECURITY server's ISI messages to
* PhoNet via PhoNetSender. It also receives SECURITY server's
* ISI messages from PhoNetReceiver.
*
*  @lib (unknown)
*  @since 2.6
*/
class CMmSecurityMessHandler
        : public CBase,
          public MMmMessHandlerBase,
          public MMmMessageReceiver,
          public MUiccOperationBase
    {

    public:
        struct CNosBootState
            {
            TBool iSIMReady;
            TBool iSecReady;
            TBool iSecCodeRequired;
            TBool iPinRequired;
            TBool iPinVerified;
            TBool iSmsPReady;
            TBool iCachingActive;
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            TUint8 iMtcCurrentState;
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            TUint8 iMceCurrentState;
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            };
        // This enumeration is needed because different phonebook specific
        // requests can be made using the same SIM_PB_REQ/RESP messages.
        // Used as a transaction ID. Storage classes use traId values 0-10.
        enum TFDNListRequestType
            {
            EGetCurrentFDNListStatus = 11,
            ESetCurrentFDNListStatusToEnabled = 12,
            ESetCurrentFDNListStatusToDisabled = 13
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPhoNetSender Pointer to the Phonet sender
        * @param aPhoNetReceiver Pointer to the Phonet receiver
        * @param aNetMessHandler pointer to the net mess handler
        * @param aCallMessHandler pointer to the call mess handler
        * @param aMessageRouter Pointer to the message router
        * @return created message handler object
        */
        static CMmSecurityMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmNetMessHandler* aNetMessHandler,
            CMmCallMessHandler* aCallMessHandler,
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler );

        /**
        * Destructor.
        */
        ~CMmSecurityMessHandler();

        /**
        * Entry point for request from the message manager
        * @param aIpc
        * @param CMmDataPackage
        */
        TInt ExtFuncL(
            TInt aIpc,
            const CMmDataPackage* aDataPackage );

        /**
        * Handles a received message by calling the specific
        * message handling method.
        * @param TIsiReceiveC, reference to the received message.
        * @return TInt: True if message been handled in this message handler
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Returns GetPukCodeRequired
        * @param None
        * @return TInt
        */
        TInt GetPukCodeReq();


    private:

        /**
        * 2nd phase constructor
        */
        void ConstructL();

        /**
        * Handles requests to verify a security code
        * @param RMobilePhone::TMobilePhoneSecurityCode*: type of sec code
        * @param const RMobilePhone::TCodeAndUnblockCode* aCodes
        * return TInt, success/failure value
        */
        TInt VerifySecurityCode(
            RMobilePhone::TMobilePhoneSecurityCode* aType,
            RMobilePhone::TCodeAndUnblockCode* aCodes );


        /**
        * Handles requsests to change a security code
        * @param TUint8 aTransactionId: Transactionid
        * @param RMobilePhone::TMobilePhoneSecurityCode*: type of sec code
        * @param const RMobilePhone::TMobilePhonePasswordChangeV1* aChange
        * return TInt, success/failure value
        */
        TInt SecCodeChangeReq( TUint8 aTransactionId,
            RMobilePhone::TMobilePhoneSecurityCode* aType,
            RMobilePhone::TMobilePhonePasswordChangeV1* aChange );

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        /**
        * Forms a MTC_RF_STATUS_QUERY_REQ ISI-message.
        * @param TUint8 aTransactionId: Transaction id
        * @return TInt, success/failure value
        */
        TInt MtcRfStatusQueryReq( TUint8 aTransactionId ) const;

        /**
        * Breaks a MTC_RF_STATUS_QUERY_RESP ISI-message.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void MtcRfStatusQueryResp( const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Forms a MTC_STATE_QUERY_REQ ISI-message.
        * @param TUint8 aTransactionId: Transaction id
        * @return TInt, success/failure value
        */
        TInt MtcStateQueryReq( TUint8 aTransactionId ) const;

        /**
        * Breaks a MTC_STATE_QUERY_RESP ISI-message.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void MtcStateQueryRespL( const TIsiReceiveC& aIsiMessage );

        /**
        * MTC state info indication.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return void : None
        */
        void MtcStateInfoIndL( const TIsiReceiveC& aIsiMessage );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        /**
        * Forms a MCE_RF_STATE_QUERY_REQ ISI-message.
        * @return Error code
        */
        TInt MceRfStateQueryReq() const;

        /**
        * Breaks a MCE_RF_STATE_QUERY_RESP ISI-message.
        * @param aIsiMessage: Received ISI message
        * @return None
        */
        void MceRfStateQueryResp( const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Forms a MCE_MODEM_STATE_QUERY_REQ ISI-message.
        * @return Error code
        */
        TInt MceModemStateQueryReq() const;

        /**
        * Breaks a MCE_MODEM_STATE_QUERY_RESP ISI-message.
        * @param aIsiMessage Received ISI message
        * @return None
        */
        void MceModemStateQueryResp( const TIsiReceiveC& aIsiMessage );

        /**
        * MCE state indication.
        * @param aIsiMessage Received ISI message
        * @return None
        */
        void MceModemStateInd( const TIsiReceiveC& aIsiMessage );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

        /**
        * Handles UICC_CARD_IND
        * @param aIsiMessage Received ISI message
        * @return void
        */
        void UiccCardInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles UICC_PIN_IND
        * @param aIsiMessage Received ISI message
        * @return void
        */
        void UiccPinInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates and sends UICC_PIN_REQ ISI message
        * @param aServiceType Service type
        * @param aCode Code
        * @param aUnblockCode Unblocking code
        * @return Error code
        */
        TInt UiccPinReqVerify(
            const TUint8 aServiceType,
            const RMobilePhone::TMobilePassword& aCode,
            const RMobilePhone::TMobilePassword& aUnblockCode);

        /**
        * Handles UICC_PIN_RESP
        * @param aIsiMessage Received ISI message
        * @return void
        */
        void UiccPinResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates and sends UICC_PIN_REQ ISI message
        * @param aLock Lock to be queried
        * @return Error code
        */
        TInt UiccPinReqStateQuery(
            const RMobilePhone::TMobilePhoneLock aLock );

        /**
        * Creates and sends UICC_PIN_REQ ISI message
        * @param aType Code type
        * @param aOldCode Current PIN code
        * @param aNewCode New PIN code
        * @return Error code
        */
        TInt UiccPinReqChange(
            RMobilePhone::TMobilePhoneSecurityCode& aType,
            const RMobilePhone::TMobilePassword& aOldCode,
            const RMobilePhone::TMobilePassword& aNewCode );

        /**
        * Creates and sends UICC_PIN_REQ ISI message
        * @param aLock Lock type
        * @param aSetting New setting to be changed
        * @param aCode PIN code
        * @return Error code
        */
        TInt UiccPinReqChangeState(
            const RMobilePhone::TMobilePhoneLock aLock,
            const RMobilePhone::TMobilePhoneLockSetting& aSetting,
            const RMobilePhone::TMobilePassword& aCode );

        /**
        * Handles PIN code verify response
        * @param aStatus Status
        * @param aIsiMessage ISI message
        * @return void
        */
        void HandleUiccPinVerifyResp(
            const TUint8 aStatus,
            const TIsiReceiveC& aIsiMessage );

        /**
        * Handles PIN code state query response
        * @param aStatus Status
        * @param aIsiMessage ISI message
        * @return void
        */
        void HandleUiccPinInfoResp(
            const TUint8 aStatus,
            const TIsiReceiveC& aIsiMessage );

        /**
        * Handles PIN code changing response
        * @param aStatus Status
        * @param aIsiMessage ISI message
        * @return void
        */
        void HandleUiccPinChangeResp(
            const TUint8 aStatus,
            const TIsiReceiveC& aIsiMessage );

        /**
        * Handles PIN code disabling/enabling response
        * @param aStatus Status
        * @param aIsiMessage ISI message
        * @return void
        */
        void HandleUiccPinStateChangeResp(
            const TUint8 aStatus,
            const TIsiReceiveC& aIsiMessage );

        /**
        * Creates and sends SEC_CODE_VERIFY_REQ ISI message
        * @param aCode Code to be verified
        * @return Error code
        */
        TInt SecCodeVerifyReq( const RMobilePhone::TMobilePassword& aCode );

        /**
        * Reads ICC type from UICC message handler and completes it
        * @return void
        */
        void GetIccType();

        /**
        * Reads active PIN from UICC message handler and completes it
        * @return void
        */
        void GetActivePin();

        /**
        * Reads AID of active USIM application from UICC message handler
        * and completes it
        * @return void
        */
        void GetActiveUsimApplication();

        /**
        * Reads elementary file EFest in case of UICC card
        * @param aTraId transaction id used in read req
        * @return Error code
        */
        TInt ReadEfEst( TUiccTrId aTraId );

        /**
        * Sends INVALIDATE/REHABILITATE command for setting
        * FDN state in case of ICC card
        * @return Error code
        */
        TInt SendFdnStateCommand();

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
        * Handles EFest read response in FDN state setting
        * @param aStatus Status
        * @param aFileData EFest data
        * @return void
        */
        void FdnSetReadEfEstResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Handles EFest read response in FDN state getting
        * @param aStatus Status
        * @param aFileData EFest data
        * @return void
        */
        void FdnGetReadEfEstResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Handles EFest read response
        * @param aStatus Status
        * @return void
        */
        void WriteEfEstResp( const TInt aStatus );

        /**
        * Handles INVALIDATE/REHABILITATE command response
        * in case of ICC card
        * @param aStatus Status
        * @param aFileData response from ICC card
        * @return void
        */
        void FdnStateCommandResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Sends UICC_APPL_CMD_REQ with service type UICC_APPL_FILE_INFO
        * for reading file info for elementary file EFadn in case of ICC card.
        * @param aTraId Transaction ID
        * @return Error code
        */
        TInt ReadEfAdnFileInfo( TUiccTrId aTraId );

        /**
        * Handles UICC_APPL_CMD_RESP with service type UICC_APPL_FILE_INFO
        * for handling file info for elementary file EFadn in case of ICC card
        * and FDN setting.
        * @param aStatus Status
        * @param aFileData response from ICC card
        * @return void
        */
        void FdnSetReadEfAdnFileInfoResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Handles UICC_APPL_CMD_RESP with service type UICC_APPL_FILE_INFO
        * for handling file info for elementary file EFadn in case of ICC card
        * and FDN getting.
        * @param aStatus Status
        * @param aFileData response from ICC card
        * @return void
        */
        void FdnGetReadEfAdnFileInfoResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Sends pin verified event notification to upper level.
        * @param aStatus Status
        * @return void
        */
        void CompleteIfCodeVerified( const TUint8 aStatus );

    protected:

        /**
        * C++ default constructor.
        */
        CMmSecurityMessHandler();

    private:    // Data

        //keep record of security code type when verify it
        RMobilePhone::TMobilePhoneSecurityCode iSecurityCode;

        //Pointer to the Message Router
        CMmMessageRouter* iMessageRouter;

        //Pointer to the Net Message Handler
        CMmNetMessHandler* iNetMessHandler;

        //Pointer to the Call Message Handler
        CMmCallMessHandler* iCallMessHandler;

        //ApplicationNumber of ISIM application
        TUint8 iApplicationNumber;

        // Type of security code in case verify code
        RMobilePhone::TMobilePhoneSecurityCode iSecCodeTypeForVerifyCode;

        // Type of security code in case change code
        RMobilePhone::TMobilePhoneSecurityCode iSecCodeTypeForChangeCode;

    protected:  // Data

        //Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

        //Boot state
        CNosBootState iBootState;

        //Is puk code required
        TBool iPukCodeRequired;

        // Active PIN
        RMobilePhone::TMobilePhoneSecurityCode iActivePin;

        // Application ID
        TUint8 iApplicationId;

        // PIN key reference
        TUint8 iPinId;

        // Flag indicating if PIN attempts left query is in case
        TBool iPinAttemptsLeftQuery;

        // Flag indicating if lock state query is in case
        TBool iLockStateQuery;

        // Security code type
        RMobilePhone::TMobilePhoneSecurityCode iCodeType;

        // variable to store FDN state which is going
        // to be set
        RMobilePhone::TMobilePhoneFdnSetting iFdnSetting;

    };


#endif // CMMSECURITYMESSHANDLER_H

// End of File
