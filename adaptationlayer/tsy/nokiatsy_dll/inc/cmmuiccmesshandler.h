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
* Description:   ?Description
*
*/



#ifndef _CMMUICCMESSHANDLER_
#define _CMMUICCMESSHANDLER_

//  INCLUDES
#include "muiccoperationbase.h"
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"

#include <etelmm.h> // Symbian Etel related types
#include <e32def.h>
#include <e32base.h>
#include <tisi.h>
#include <uiccisi.h>

// CONSTANTS
const TUint8 KFilePathLength( 16 );
const TUint8 KFileDataLength( 200 );
const TUint16 KApduDataLength( 270 );

// UICC master file
const TUint16 KMasterFileId( 0x3F00 );

// ADFusim
const TUint16 KAdfUsimFileId( 0x7FFF );

// Operator specific dedicated files
// Orange operator specific dedicated file
const TUint16 KOrangeDedicatedFile( 0x7F40 );
// Operator specific dedicated file
const TUint16 KO2DedicatedFile( 0x7F43 );

// UICC elementary files
const TUint16 KElemFileAdministrativeData         ( 0x6FAD );
const TUint16 KElemFileOperatorName               ( 0x6F14 );
const TUint16 KElemFileShortMessages              ( 0x6F3C );
const TUint16 KElemFileCellBroadcastMessId        ( 0x6F45 );
const TUint16 KElemFileDyn2FlagsOrange            ( 0x6F92 );
const TUint16 KElemFileHomeZone                   ( 0x6F60 );
const TUint16 KElemFileHomeZoneCache1             ( 0x6F61 );
const TUint16 KElemFileHomeZoneCache2             ( 0x6F62 );
const TUint16 KElemFileHomeZoneCache3             ( 0x6F63 );
const TUint16 KElemFileHomeZoneCache4             ( 0x6F64 );
const TUint16 KElemFileUhziueSettings             ( 0x6F87 );
const TUint16 KElemFileEhplmn                     ( 0x6FD9 );
const TUint16 KElemFileImsi                       ( 0x6F07 );
const TUint16 KElemFileCustomerServiceProfile     ( 0x6F15 );
const TUint16 KElemFileServiceProviderName        ( 0x6F46 );
const TUint16 KElemFileServiceProviderDisplayInfo ( 0x6FCD );
const TUint16 KElemFileDynFlagsOrange             ( 0x6F9F );
const TUint16 KElemFileCallFwdFlagsCphs           ( 0x6F13 );
const TUint16 KElemFileCallFwdIndicationStatus    ( 0x6FCB );
const TUint16 KElemFileVoiceMsgWaitingFlagsCphs   ( 0x6F11 );
const TUint16 KElemFileMessageWaitingIndStatus    ( 0x6FCA );
const TUint16 KElemSimServiceTable                ( 0x6F38 );
const TUint16 KElemGba                            ( 0x6FD6 );
const TUint16 KElemEst                            ( 0x6F56 );
const TUint16 KElemCphsInformation                ( 0x6F16 );

const TUint8 KEfSstSize (0xFF);
const TUint8 KEfUstSize (0xFF);
const TUint8 KEfCphsInfoSize (0xFF);

// Unique transaction IDs for all UICC operations
enum TUiccTrId
    {
    ETrIdNotDefined = 0,
    ETrIdWriteCallFwdFlagsCphs,
    ETrIdWriteCallFwdFlagsRel4,
    ETrIdReadCallFwdFlagsCphs,
    ETrIdWriteVoiceMsgFlags,
    ETrIdReadVoiceMsgFlags, // 5
    ETrIdReadCipheringIndicatorStatus,
    ETrIdGetUiccServerStatus,
    ETrIdGetUiccApplication,
    ETrIdActivateUiccApplication,
    ETrIdStartupCompleted, // 10
    ETrIdReadImsi,
    ETrIdPbOpInit,
    ETrIdReadCsDataEhplmn,
    ETrIdReadCsDataHplmn,
    ETrIdReadServiceProviderName, // 15
    ETrIdReadServiceProviderDisplayInfo,
    ETrIdReadCallFwdFlagsRel4,
    ETrIdReadCsp,
    ETrIdReadField,
    ETrIdReadMailboxIdentifier, // 20
    ETrIdReadCbMsgIds,
    ETrIdUpdateCbMsgIdsPhase1,
    ETrIdUpdateCbMsgIdsPhase2,
    ETrIdReadCipheringIndicatorStatusCustom,
    ETrIdNotifyCipheringIndicatorStatusCustom, // 25
    ETrIdReadOperatorNameCustom,
    ETrIdReadViagHomeZoneParamsCustom,
    ETrIdReadViagHomeZoneCacheCustom,
    ETrIdWriteViagHomeZoneCacheCustom,
    ETrIdWriteViagHomeZoneUhziueSettingsCustom, // 30
    ETrIdReadVoiceMsgFlagsRel4,
    ETrIdReadVoiceMsgFlagsCphs,
    ETrIdWriteVoiceMsgFlagsRel4,
    ETrIdWriteVoiceMsgFlagsCphs,
    ETrIdReadDynamicFlags, // 35
    ETrIdWriteDynamicFlags,
    ETrIdReadDynamic2Flags,
    ETrIdWriteDynamic2Flags,
    ETrIdSimPowerOff,
    ETrIdSimWarmReset, //40
    ETrIdSimGetATR,
    ETrIdSimPowerOn,
    ETrIdSendAPDUv2,
    ETrIdCardReaderStatus,
    ETrIdEEapAkaAuthenticate, // 45
    ETrIdEEapAuthenticate,
    ETrIdSendAPDU,
    ETrIdEEapSimAuthenticate,
    ETrIdEGbaBootstrap,
    ETrIdSstCache, // 50
    ETrIdEGbaBootstrapRead,
    ETrIdEGbaBootstrapUpdate,
    ETrIdEGbaNafDerivation,
    ETrIdEMbmsMskUpdate,
    ETrIdEMbmsMtkGeneration, // 55
    ETrIdEMbmsMskDeletion,
    ETrIdRunGsmAlgorithmSim,
    ETrIdRunGsmAlgorithmAka,
    ETrIdRunGsmAlgorithmIms,
    ETrIdActivateIsimApplication, // 60
    ETrIdEEapAkaAuthenticateIms,
    ETrIdReadSMS,
    ETrIdWriteSMS,
    ETrIdReadSMSRecordCount,
    ETrIdReadSMSForComplete,     // 65
    ETrIdPbInit,
    ETrIdPbInitFDN,
    ETrIdPbRead,
    ETrIdPbReadADN,
    ETrIdPbUpdate,              // 70
    ETrIdPbDelete,
    ETrIdSetFdnStateReadEst,
    ETrIdSetFdnStateWriteEst,
    ETrIdSetFdnIcc,
    ETrIdSetFdnStateReadFileInfo, // 75
    ETrIdGetFdnStateReadEst,
    ETrIdGetFdnStateReadFileInfo,
    ETrIdCphsCache,
    ENumOfUiccTrIds
    };

// Common parameters needed in UICC_APPL_READ_TRANSPARENT,
// UICC_APPL_UPDATE_TRANSPARENT, UICC_APPL_READ_LINEAR_FIXED,
// UICC_APPL_UPDATE_LINEAR_FIXED, UICC_APPL_FILE_INFO and
// UICC_APPL_APDU_SEND
class TUiccParamsBase
    {
    public:
        // Constructor
        TUiccParamsBase();

        // Data
        MUiccOperationBase* messHandlerPtr; // Pointer to message handler
        TUiccTrId trId; // Unique transaction ID for every UICC operation
        TUint16 fileId; // ID of elementary file
        TUint8 fileIdSfi; // Filled only in case SFI is present
        TUint8 serviceType; // UICC service type
        TBuf8<KFilePathLength> filePath; // Elementary file path, (DF ) + EF
                                        // If same as fileId, no need to fill
    } ;

// Parameters needed in UICC_APPL_FILE_INFO
class TUiccApplFileInfo: public TUiccParamsBase
    {
    public:
        // Constructor
        TUiccApplFileInfo();
    };

// Parameters needed in UICC_APPL_READ_TRANSPARENT
class TUiccReadTransparent: public TUiccParamsBase
    {
    public:
        // Constructor
        TUiccReadTransparent();

        // Data
        TUint16 dataOffset; // Offset of elementary file data
        TUint16 dataAmount; // Used only in reading, bytes to be read
    };

// Parameters needed in UICC_APPL_UPDATE_TRANSPARENT
class TUiccWriteTransparent: public TUiccReadTransparent
    {
    public:
        // Constructor
        TUiccWriteTransparent();

        // Data
        TBuf8<KFileDataLength> fileData; // Data to be updated
    };

// Parameters needed in UICC_APPL_READ_LINEAR_FIXED
class TUiccReadLinearFixed: public TUiccReadTransparent
    {
    public:
        // Constructor
        TUiccReadLinearFixed();

        // Data
        TUint8 record; // Record number
    } ;

// Parameters needed in UICC_APPL_UPDATE_LINEAR_FIXED
class TUiccWriteLinearFixed: public TUiccReadLinearFixed
    {
    public:
        // Constructor
        TUiccWriteLinearFixed();
        // Data
        TBuf8<KFileDataLength> fileData; // Data to be updated
    } ;

// Parameters needed in UICC_APPL_APDU_SEND
class TUiccSendApdu: public TUiccParamsBase
    {
    public:
        // Constructor
        TUiccSendApdu();
        // Data
        TBuf8<KApduDataLength> apdu; // ADPU going to be send
    } ;

// Parameters needed in UICC_APDU_REQ
class TUiccParamsApduReq: public TUiccParamsBase
    {
    public:
        // Constructor
        TUiccParamsApduReq();

        // Data
        TUint8 action; //   UICC_SB_APDU_ACTIONS / Action
        // Data
        TBuf8<KApduDataLength> apduData; // APDU data
    } ;

// FORWARD DECLARATIONS
    class CMmPhoNetSender;
    class CMmPhoNetReceiver;
    class CMmMessageRouter;

// CLASS DECLARATION

/**
*  Class used for sending and receiving UICC messages
*/
class CMmUiccMessHandler :
    public CBase,
    public MMmMessageReceiver,
    public MUiccOperationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmUiccMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter );

        /**
        * Destructor.
        */
        virtual ~CMmUiccMessHandler();

    public: // New functions

        /**
        * Receives ISI messages from phonet
        * @param aIsiMsg Received ISI message
        * @return void
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMsg );

        /**
        * Create and send UICC_APPL_CMD_REQ message
        * @param aParams Parameters needed in message
        * @param aApplType Application type
        * @return KErrNone or error code
        */
        TInt CreateUiccApplCmdReq(
            const TUiccParamsBase& aParams,
            const TUint8 aApplType = UICC_APPL_TYPE_UICC_USIM );

        /**
        * Create and send UICC_APPLICATION_REQ message
        * @param aParams Parameters needed in message
        * @param aServiceType Service type
        * @param aApplType Application type
        * @return KErrNone or error code
        */
        TInt CreateUiccApplicationReq(
            const TUiccParamsBase& aParams,
            TUint8 aServiceType,
            TUint8 aApplType = UICC_APPL_TYPE_UICC_USIM );

        /**
        * Provide UICC application ID
        * @return TUint8
        */
        TUint8 GetApplicationId() const;

        /**
        * Provide UICC application type
        * @return TUint8
        */
        TUint8 GetApplicationType() const;

        /**
        * Provide UICC card type
        * @return TUint8
        */
        TUint8 GetCardType() const;

        /**
        * Provide UICC application file id
        * @return const TDesC8&
        */
        const TDesC8& GetApplicationFileId() const;

        /**
        * Provide UICC application AID
        * @return const TDesC8&
        */
        const RMobilePhone::TAID& GetAid() const;

        /**
        * Create and send UICC_CONNECTOR_REQ message
        * @param aParams Parameters needed in message
        * @return KErrNone or error code
        */
        TInt CreateUiccConnectorReq( const TUiccParamsBase& aParams );

        /**
        * Create and send UICC_APDU_REQ message
        * @param aParams Parameters needed in message
        * @return KErrNone or error code
        */
        TInt CreateUiccApduReq( const TUiccParamsApduReq& aParams );

        /**
        * Create and send UICC_CARD_REQ
        * @param aParams Parameters needed in message
        * @return KErrNone or error code
        */
        TInt CreateUiccCardReq( const TUiccParamsBase& aParams );

        /**
        * Caches SIM service table (EFust or EFsst depending
        * on card type in use)
        * @return none
        */
        void InitializeSimServiceTableCache();

        /**
        * Gets service status from service table
        * (from EFust or EFsst depending on card type)
        * on card type in use)
        * @return ETrue if service supported, otherwise EFalse
        */
        TBool GetServiceStatus( TUint8 aServiceNo );

        /**
        * Gets ISIM application status
        * @return TInt status of ISIM application
        */
        TUint8 GetIsimApplicationStatus() const;

        /**
        * Handle UICC response messages
        * @param aTraId Transaction ID
        * @param aStatus status of the UICC response
        * @param aFileData Descriptor for file data
        * @return KErrNone or error code
        */
        TInt ProcessUiccMsg(
            TInt aTraId,
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Checks is ISim application found during boot
        * @return TBool ETrue if ISIM application found, otherwise EFalse
        */
        TBool IsIsimApplicationFound() const;

        /**
        * Get PIN1 key reference
        * @return PIN1 key reference
        */
        TUint8 GetPin1KeyReference();

        /**
        * Get PIN2 key reference
        * @return PIN2 key reference
        */
        TUint8 GetPin2KeyReference();

        /**
        * Get active PIN
        * @return Active PIN
        */
        RMobilePhone::TMobilePhoneSecurityCode GetActivePin();

        /**
        * Change active PIN
        * @return void
        */
        void ChangeActivePin();

        /**
        * Caches CPHS Information from elementary file in
        * case if ICC card
        * @return none
        */
        void InitializeCphsInformationCache();

        /**
        * Gets status of the CPHS information in case of ICC card
        * (CPHS information is read form elementary file CPHS Information.
        * file identifies is 0x6F16 )
        * @return ETrue if service supported, otherwise EFalse
        */
        TBool GetCphsInformationStatus( TUint8 aServiceNo );

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        void SendTerminalProfile();

        /**
        * C++ default constructor.
        */
        CMmUiccMessHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver );

        /**
        * Create and append UICC_SB_APPL_PATH
        * @param aParams Parameters needed for subblock
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset where subblock is appended
        * @param aSizeOfSubblock Subblock size to be returned
        * @return void
        */
        void CreateUiccSbApplPath(
            const TUiccParamsBase& aParams,
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset,
            TUint8& aSizeOfSubblock );

        /**
        * Create and append UICC_SB_CLIENT
        * @param aIsiMsg    ISI message
        * @param aMsgOffset Offset where subblock is appended
        * @param aApplType  type of the application
        * @return void
        */
        void CreateUiccSbClient(
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset,
            const TUint8 aApplType = UICC_APPL_TYPE_UICC_USIM );

        /**
        * Create and append UICC_SB_TRANSPARENT
        * @param aParams Parameters needed for subblock
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset where subblock is appended
        * @return void
        */
        void CreateUiccSbTransparent(
            const TUiccReadTransparent& aParams,
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset );

        /**
        * Create and append UICC_SB_LINEAR_FIXED
        * @param aParams Parameters needed for subblock
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset where subblock is appended
        * @return void
        */
        void CreateUiccSbLinearFixed(
            const TUiccReadLinearFixed& aParams,
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset );

        /**
        * Create and append UICC_SB_FILE_DATA
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset where subblock is appended
        * @param aUpdateData Data to be updated
        * @return void
        */
        void CreateUiccSbFileData(
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset,
            const TDesC8& aUpdateData );

        /**
        * Create and append UICC_SB_APPLICATION
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset where subblock is appended
        * @return void
        */
        void CreateUiccSbApplication(
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset,
            TUint8 aApplType = UICC_APPL_TYPE_UICC_USIM );

        /**
        * Get file data
        * @param aIsiMsg Received ISI message
        * @return File data
        */
        const TPtrC8 GetFileData( const TIsiReceiveC& aIsiMsg );

        /**
        * Get file FCI
        * @param aIsiMsg Received ISI message
        * @return FCI data
        */
        const TPtrC8 GetFileFCI( const TIsiReceiveC& aIsiMsg );

        /**
        * Create and append UICC_SB_APDU_ACTIONS
        * @param aIsiMsg ISI message
        * @return void
        */
        void CreateUiccSbApduActions(
            TIsiSend& aIsiMsg,
            TUint8 aAction);

        /**
        * Handle UICC_RESP ISI message
        * @param aIsiMsg ISI message
        * @return Server status
        */
        TUint8 HandleUiccResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Handle UICC_APPLICATION_RESP ISI message
        * @param aIsiMsg ISI message
        * @return KErrNone or error code
        */
        TInt HandleUiccApplicationResp( const TIsiReceiveC& aIsiMsg );

        /**
        * Get apdu data
        * @param aIsiMsg Received ISI message
        * @return APDU data
        */
        const TPtrC8 GetApduData( const TIsiReceiveC& aIsiMsg );

        /**
        * Create and append UICC_SB_APDU
        * @param aIsiMsg ISI message
        * @param aMsgOffset Offset where subblock is appended
        * @param aAPDUData Data to be updated
        * @return void
        */
        void CreateUiccSbApdu(
            TIsiSend& aIsiMsg,
            TUint8 aMsgOffset,
            const TDesC8& aAPDUData );

        /**
        * Handles response to the service table reading
        * @param aStatus   status of the UICC response
        * @param aFileData service table data
        * @return void
        */
        void SimServiceTableCacheResp(
            TInt aStatus,
            const TDesC8& aFileData );

        /**
        * Stores PIN key references
        * @param aFileData File data
        * @return none
        */
        void StorePinKeyReferences( const TDesC8& aFileData );

        /**
        * Handles response to the CPHS information reading
        * @param aStatus   status of the UICC response
        * @param aFileData CPHS information data
        * @return void
        */
        void CphsInformationCacheResp(
            TInt aStatus,
            const TDesC8& aFileData );


    private:    // Data

        CMmPhoNetSender* iPhoNetSender;

        // Pointer to the Message router
        CMmMessageRouter* iMessageRouter;

        // Array for message handler pointers
        TFixedArray<MUiccOperationBase*, ENumOfUiccTrIds> iMessHandlerPrtList;

        TUint8 iCardType;
        TUint8 iApplicationType;
        TUint8 iApplicationId;
        TUint8 iApplicationStatus;
        TUint8 iUiccServerStatus;
        TUint8 iUiccClientId;
        TUint8 iIsimApplicationId;
        TUint8 iIsimApplicationStatus;
        TUint8 iUiccIsimClientId;
        TUint8 iPin1Id;
        TUint8 iPin2Id;
        TBool iIsimApplicationFound;
        // Application file ID, two bytes
        TBuf8<2> iApplFileId;
        // Flag for keeping tranactionid in list, so that future indications
        // can be relayed to interested party.
        TBool iDoNotRemoveTransactionID;
        // SIM Service Table, used to cache EFsst
        TBuf8<KEfSstSize> iSSTFile;
        // USIM Service Table, used to cache EFust
        TBuf8<KEfUstSize> iUSTFile;
        // CPHS Information Table, used to cache CPHS information
        TBuf8<KEfCphsInfoSize> iCPHSInformation;
        // Store AID of active application
        RMobilePhone::TAID iAid;
        // Store active PIN
        RMobilePhone::TMobilePhoneSecurityCode iActivePin;

    };

#endif      // _CMMUICCMESSHANDLER_

// End of File
