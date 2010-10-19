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



#ifndef CMMNETMESSHANDLER_H
#define CMMNETMESSHANDLER_H

// INCLUDES
#include <e32base.h> // Symbian base types
#include <etelmm.h> // Symbian Etel related types

#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include "muiccoperationbase.h"
#include "nokiatsy_internal_variation.h"

// CONSTANTS

// 2.8 AVKON LAF UI specification states that signal strength is
// indicated by 0 to 7 bars
const TInt8 KMaxSignalStrengthBars = 7;
// GpdsRadioActivityInd / GPDS_RADIO_OFF completion delay (2000000us = 2 seconds)
const TInt KGPDSRAICompleteDelay = 2000000;

// MACROS
    //None

// DATA TYPES
typedef TBuf<4> TOperatorCode; // Code data size is 4
typedef TBuf<3> TCountryCode; // Code data size is 3

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmDataPackage;
class CMmNetOperatorNameHandler;
class CMmMessageRouter;
class TIsiReceiveC;
class TIsiSubBlock;
class CDelayTimer;
class CMmUiccMessHandler;

// CLASS DECLARATION

/**
* Used for creating and sending NET server's ISI messages to
* PhoNet via PhoNetSender. It also receives NET server's
* ISI messages from PhoNetReceiver.
*/
class CMmNetMessHandler
        : public CBase, public MMmMessHandlerBase, public MMmMessageReceiver,
        public MUiccOperationBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPhoNetSender: pointer to the Phonet sender.
        * @param aPhoNetReceiver: pointer to the Phonet receiver.
        * @param aMessageRouter: Pointer to the message router.
        * @return created message handler object.
        */
        static CMmNetMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler );

        /**
        * Destructor.
        */
        ~CMmNetMessHandler();

    public: // Functions

        /**
        * Handle UICC response messages
        * @param aTraId Transaction ID
        * @param aFileData Descriptor for file data
        * @return KErrNone or error code
        */
        TInt ProcessUiccMsg( TInt aTraId, TInt aStatus, TUint8 aDetails, const TDesC8& aFileData );

        /**
        * Entry point for request from the message manager
        * @param aIpc
        * @param CMmDataPackage
        * @return TInt ret: KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * Handles a received message by calling the specific
        * message handling method.
        * @param TIsiReceiveC, reference to the received message.
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles errors coming from PhoNetReceiver RunError
        * @param const TIsiReceiveC& aIsiMessage: The received ISI message
        * @param TInt aError: Error code
        */
        void HandleError( const TIsiReceiveC& aIsiMessage, TInt aError );

        /**
        * Returns boolean of IMS PIN verify status
        * @return TBool: EFalse or ETrue
        */
        TBool GetImsPinVerifyStatus();

        /**
        * Maps the isa operator code to Symbian values
        * @param TDesC8 aIsaOperatorAndCountryCode: Operator and country code
        * @param TMobilePhoneNetworkIdentity aOperatorCode: Network identity
        * @param TMobilePhoneNetworkCountryCode aCountryCode: Country code
        * @param TInt aMNCLength: Mnc default lenght is three
        * @return TBool networkCodeMapped: EFalse or ETrue
        */
        static TBool MapOperatorAndCountryCode(
            const TDesC8& aIsaOperatorAndCountryCode,
            RMobilePhone::TMobilePhoneNetworkIdentity& aOperatorCode,
            RMobilePhone::TMobilePhoneNetworkCountryCode& aCountryCode,
            TInt aMNCLength = 3 );

        /**
        * Maps the isa network status to symbian values.
        * @param TUint8 aISANetworkStatus: Network status value.
        * @param TMobilePhoneNetworkStatus aNWStatus: Network status value.
        * @return None
        */
        static void MapNwStatus(
            TUint8 aISANetworkStatus,
            RMobilePhone::TMobilePhoneNetworkStatus& aNWStatus );

        /**
        * Saves GPDS radio activity information and completes
        * NotifyCurrentNetworkChange if state changed.
        * @param aNewPacketTrancferStatus: New radio activity state
        */
        void CompleteGpdsRadioActivityInd( TBool aNewPacketTrancferStatus );

        /**
        * Completes IPC EMobilePhoneNotifyCurrentNetworkChange and
        * EMobilePhoneGetNetworkRegistrationStatus.
        * @param aLocationAreaData includes Location Area data
        * @param aNetworkInfoData includes Network data
        */
        void CompleteMobilePhoneNotifyCurrentNetworkChange(
            const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationAreaData,
            const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfoData );

        /**
        * Continue handling of queued messages if available.
        * @return None
        */
        void ContinueHandlingNetModemRegStatusMessages();

        /**
        * Starts reading Operator Name String and Operator PLMN List rules
        * when SIM is ready.
        * @return None
        */
        void InitializeOnsAndOplReading();

        /**
        * Gets pointer to CMmMessageRouter class.
        * @return Pointer to CMmMessageRouter object.
        */
        CMmMessageRouter* GetMessageRouter();

        /**
        * Gets pointer to CMmPhoNetSender class.
        * @return Pointer to CMmPhoNetSender object.
        */
        CMmPhoNetSender* GetPhoNetSender();

        /**
        * Gets pointer to CMmNetOperatorNameHandler class.
        * @return Pointer to CMmNetOperatorNameHandler object.
        */
        CMmNetOperatorNameHandler* GetNetOperatorNameHandler();

        /**
        * Extracts network registration status from NET_MODEM_REG_STATUS_IND,
        * NET_SET_RESP or NET_MODEM_REG_STATUS_GET_RESP
        * @param aIsiMessage ISI-message to extract data from
        * @param aRegistrationStatus resulting registration status
        * @return TInt KErrNone, KErrNotFound if subblock not found
        * or system-wide error
        */
        TInt ExtractNetworkRegistrationStatus(
            const TIsiReceiveC& aIsiMessage,
            TUint8& aRegistrationStatus
            ) const;

        /**
        * Handles last received NET_MODEM_REG_STATUS_IND again
        * so that network information is completed to the upper 
        * layers
        * @return void
        */
        void HandleLastNetModemRegStatusInd();

    private: // Constructors and destructor

        /**
        * 2nd phase constructor
        */
        void ConstructL();

    private: // Functions

        /**
        * Breaks a NET_MODEM_REG_STATUS_IND ISI-message.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetModemRegStatusInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Sends a NET_MODEM_REG_STATUS_GET_REQ ISI-message.
        * @param TUint8 aTransactionId: Transaction id
        * @return TInt, success/failure value
        */
        TInt NetModemRegStatusGetReq( TUint8 aTransactionId ) const;

        /**
        * Net modem available get request.
        * @param TUint8 aTransactionId: Transaction id
        * @return TInt, success/failure value
        */
        TInt NetModemAvailableGetReq( TUint8 aTransactionId ) const;

        /**
        * GSM specific part of NET_MODEM_AVAILABLE_GET_RESP ISI-message.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetModemAvailableGetRespL( const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Net available cancel request.
        * @param TUint8 aTransactionId: Transaction id
        * @return TInt, success/failure value
        */
        TInt NetAvailableCancelReq( TUint8 aTransactionId );

        /**
        * Net available cancel response.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetAvailableCancelResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Net set request.
        * @param TUint8 aTransactionId: Transaction id
        * @param TBool aIsManual: Mode of the Selection
        * @param TOperatorCode aMnc: Pointer to MNC code
        * @param TCountryCode aMcc: Pointer to MCC code
        * @return TInt, success/failure value
        */
        TInt NetSetReq(
            TUint8 aTransactionId,
            TBool aIsManual,
            const TOperatorCode& aMnc,
            const TCountryCode& aMcc ) const;

        /**
        * Net set cancel request.
        * @param TUint8 aTransactionId: Transaction id
        * @return TInt, success/failure value
        */
        TInt NetSetCancelReq( TUint8 aTransactionId );

        /**
        * Net set cancel response
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @retrun None
        */
        void NetSetCancelResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Request the current Radio access technology
        * @param TUint8 aTransactionId: Transaction id
        * @return TInt: Error code
        */
        TInt NetRatReq( TUint8 aTransactionId ) const;

        /**
        * Breaks a NET_RAT_RESP ISI message
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetRatResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Net network select mode set req.
        * @param TUint8 aTransactionId: Transaction id
        * @param RMobilePhone::TMobilePhoneNetworkSelectionV1& aNetworkSelection:
        * Network selection
        * @return TInt, success/failure value
        */
        TInt NetNetworkSelectModeSetReq(
            TUint8 aTransactionId,
            const RMobilePhone::TMobilePhoneNetworkSelectionV1& aNetworkSelection ) const;

        /**
        * Net network select mode set response
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetNetworkSelectModeSetResp( const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Network Get RSSI request.
        * @param None
        * @return TInt, success/failure value
        */
        TInt NetRssiGetReq() const;

        /**
        * Network Get RSSI response.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetRssiGetResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Network RSSI indication.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetRssiInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Handle RSSI and complete to CommonTSY.
        * @param TUint8 aBarsIsi: bar percentage (ISI value)
        * @return TUint8 aRSSIIsi: RSSI in dBm with inverse sign (ISI value)
        */
        void HandleRssi(
            TUint8 aBarsIsi,
            TUint8 aRSSIIsi ) const;

        /**
        * Network time indication.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetTimeInd( const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Network ciphering indication.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetCipheringInd( const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Ciphering indicator read request
        * @return Error code
        */
        TInt UiccReadCiReq();

        /**
        * Ciphering indicator read response
        * @param aStatus Status
        * @param aFileData File data
        * @return None
        */
        void UiccReadCiResp( TInt aStatus, const TDesC8& aFileData );

        /**
        * Network radio information indication.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetRadioInfoInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Read home network
        * @return Error code
        */
        TInt UiccCsReadReq();

        /**
        * Response to read EHPLMN
        * @param aStatus Status
        * @param aFileData File data containig EHPLMN
        * @return None
        */
        void UiccCsReadEhpmlnResp( TInt aStatus, const TDesC8& aFileData );

        /**
        * Response to read HPLMN
        * @param aStatus Status
        * @param aFileData File data containig HPLMN
        */
        void UiccCsReadHpmlnResp( TInt aStatus, const TDesC8& aFileData );

        /**
        * Set network info
        * @param None
        * @return None
        */
        void SetSimNetworkInfo();

        /**
        * GPDS radio activity indication.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void GpdsRadioActivityInd( const TIsiReceiveC& aIsiMessage );

        /**
        * GPDS Context Deactivate indication.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void GpdsContextDeactivateInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Completes IPC EMobilePhoneGetHomeNetwork.
        */
        void CompleteMobilePhoneGetHomeNetwork();

        /**
        * Queue functionality for NET_MODEM_REG_STATUS_IND,
        * NET_MODEM_REG_STATUS_GET_RESP and NET_SET_RESP ISI-messages.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void QueueNetModemRegStatusMessagesL( const TIsiReceiveC& aIsiMessage );

        /**
        * Start handling queued messages.
        * @return None
        */
        void StartHandlingNetModemRegStatusMessages();

        /**
        * Finds offset to NET_GSM_REG_INFO or NET_CURRENT_CELL_INFO
        * in NET_REG_STATUS_IND, NET_SET_RESP or NET_REG_STATUS_GET_RESP
        * @param aIsiMessage ISI-message to process
        * @param aSbOffset resulting offset to the subblock
        * @return TInt KErrNone or system-wide error
        */
        TInt FindOperatorInfoSubBlock(
            const TIsiReceiveC& aIsiMessage,
            TUint& aSbOffset
            ) const;

        /**
        * Extracts network registration status from NET_REG_INFO_COMMON
        * subblock of NET_REG_STATUS_IND, NET_SET_RESP or
        * NET_REG_STATUS_GET_RESP.
        * @param aIsiMessage ISI-message to process
        * @param aOperatorCode BCD-string containing operator code
        * @param aRegistrationStatus resulting registration status
        * @param aSbStartOffset NET_REG_INFO_COMMON subblock offset
        * @return TInt KErrNone or system-wide error
        */
        TInt ExtractNetworkRegistrationStatus(
            const TIsiReceiveC& aIsiMessage,
            const TDesC8& aOperatorCode,
            TUint8& aRegistrationStatus,
            TUint& aSbStartOffset
            ) const;

    protected: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CMmNetMessHandler();

    protected: // Functions

        /**
        * Breaks a NET_RAT_IND ISI-message.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetRatInd( const TIsiReceiveC& aIsiMessage );

        // NETWORK UTILITY FUNCTIONS
        /**
        * Maps the isa network registration status to Symbian values.
        * @param TUint8 aISARegistrationStatus: Network registration status.
        * @param TMobilePhoneRegistrationStatus aRegStatus: Phone registration
        * status.
        * @param TMobilePhoneNetworkStatus aNWStatus: Phone network status.
        * @return None
        */
        static void MapNWAndRegStatus(
            TUint8 aISARegistrationStatus,
            RMobilePhone::TMobilePhoneRegistrationStatus& aRegStatus,
            RMobilePhone::TMobilePhoneNetworkStatus& aNWStatus );

        /**
        * Maps the isa network selection mode to Symbian values.
        * @param TUInt8 aISASelectionMode: Network selection mode.
        * @param TMobilePhoneNetworkSelectionV1 aSelectionMode:
        * Network selection mode.
        * @return None
        */
        static void MapNWSelectionMode(
            TUint8 aISASelectionMode,
            RMobilePhone::TMobilePhoneNetworkSelectionV1& aSelectionMode );

    private: // Data

        // Pointer to the NetOperatorNameHandler.
        CMmNetOperatorNameHandler* iNetOperatorNameHandler;

        // Pointer to the Message Router
        CMmMessageRouter* iMessageRouter;

        // Cached network selection method
        TUint8 iSelectionMethod;

        // Wether the network selection cancel has been issued or not
        TBool iSelectNetworkCancelIssued;

        // Wether the network search cancel has been issued or not
        TBool iSearchNetworkCancelIssued;

        // Network mode: ENetworkModeGsm, ENetworkModeWcdma or
        // ENetworkModeUnknown
        RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;

        // If PIN code needs to be verified before IMSI can be used.
        TBool iImsPinVerifyRequired;

        // If ISIM application is used
        TBool iIsimUsed;

        // MNC lenght
        TUint8 iMNClength;

        // Home PLMN
        TBuf8<4> iHplmn;

        // Network access technology
        RMobilePhone::TMobilePhoneNetworkAccess iNetworkAccess;

        // Radio Resource Control (RRC) is in Cell_DCH state (there is
        // no cell/location info available while in this state)
        TBool iCell_DCH;

        // Packet transfer state (there is
        // no cell/location info available while in this state)
        TBool iPacketTransfer;

        // Delay timer for completion of Packet transfer state
        CDelayTimer* iDelayTimer;

        // Network and location information have to be saved while RRC is
        // (temporarily) in Cell_DCH state
        RMobilePhone::TMobilePhoneNetworkInfoV5 iLastNWInfo;
        RMobilePhone::TMobilePhoneLocationAreaV1 iLastLocInfo;

        // Home network information have to be saved while operator name
        // is requested from the NET Server.
        RMobilePhone::TMobilePhoneNetworkInfoV5 iHomeInfo;

        // Is NET_MODEM_REG_STATUS_IND, NET_MODEM_REG_STATUS_GET_RESP or
        // NET_SET_RESP message handling ongoing.
        TBool iNetMessageHandlingOngoing;

        // Array for queueing NET_MODEM_REG_STATUS_IND,
        // NET_MODEM_REG_STATUS_GET_RESP and NET_SET_RESP messages.
        RPointerArray<HBufC8> iNetMessageQueue;

        // For storing latest received NET_MODEM_REG_STATUS_IND
        HBufC8* iLastNetModemRegStatusInd;

        // Heap-based instance
        RMobilePhone::TMobilePhoneNetworkInfoV5 iNetworkInfoHelper;
    protected: // Data

        // Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;
    };

/**
* CDelayTimer: Used for delaying for completion of Packet transfer state
* from GPDS_RADIO_AVTIVITY_IND.
*/
class CDelayTimer : public CTimer
    {

    public: // Constructors and destructors

        /**
        * Two-phased constructor.
        * @param aNetMessHandler: Pointer to the net message handler
        * @return CDelayTimer object
        */
        static CDelayTimer* NewL( CMmNetMessHandler* aNetMessHandler );

        /**
        * ~CDelayTimer.
        * Destructor.
        */
        ~CDelayTimer();

    private: // Constructors and destructors

        /**
        * C++ default constructor.
        */
        CDelayTimer();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * From CActive.
        * Called when operation completes.
        */
        void RunL();

        /**
        * From CActive.
        * Called when operation is cancelled.
        */
        void DoCancel();

    public: // New functions

        /**
        * Start delay timer.
        * @param aNewPacketTransferState: New packet transfer state
        * @param aTimeOut: Time out in ms
        */
        void StartDelayTimer(
            TBool aNewPacketTransferState,
            TInt aTimeOut );

    private: // Data

        /**
        * pointer to the net message handler
        */
        CMmNetMessHandler* iNetMessHandler;

        /**
        * new packet transfer state
        */
        TBool iNewPacketTransferState;

    };

#endif // CMMNETMESSHANDLER_H

// End of File
