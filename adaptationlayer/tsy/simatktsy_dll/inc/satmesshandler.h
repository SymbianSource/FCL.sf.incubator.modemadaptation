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



#ifndef SATMESSHANDLER_H
#define SATMESSHANDLER_H


// INCLUDES
#include "cmmphonetreceiver.h"              // mmmmessagereceiver interface
#include <etelsat.h>                        // etel sat api
#include <product_profile_definitions.h>    // product profile definitions


// CONSTANTS
// UICC file path max length
const TUint8 KUiccFilePathLength( 8 );
// UICC file path length when reading service table
const TUint8 KUiccFilePathLengthServiceTable( 6 );

// Call Server's maximum address length. Use only with Call Server messages.
const TUint8 KCallServerMaxAddressLenght( 123 );
const TInt KAtkPdpCcRespMaxSize( 690 );

// Bitmask for status bit of call control in USIM
const TUint8 KCallControlBitMaskUsim( 0x20 );
// Bitmask for status bit of call control in SIM
const TUint8 KCallControlBitMaskSim( 0x08 );
// Bitmask for status bit of SMS PP DD in USIM
const TUint8 KSmsPpDdBitMaskUsim( 0x08 );
// Bitmask for status bit of SMS PP DD in SIM
const TUint8 KSmsPpDdBitMaskSim( 0x02 );
// Bitmask for status bit of MO SMS control in USIM
const TUint8 KMoSmsControlBitMaskUsim( 0x40 );
// Bitmask for status bit of MO SMS control in SIM
const TUint8 KMoSmsControlBitMaskSim( 0x10 );
// Bitmask for status bit of USSD CC  in SIM
const TUint8 KMoUssdCallControlBitMaskSim( 0x01 );
// Unique transaction IDs for UICC messages
const TUint8 KUiccTrIdCommon( 0xFA );
const TUint8 KUiccTrIdReadSatIcon( 0xFB );
const TUint8 KUiccTrIdReadIconClut( 0xF8 );
const TUint8 KUiccTrIdReadIconInstancePhase1( 0xF7 );
const TUint8 KUiccTrIdReadIconInstancePhase2( 0xF6 );
const TUint8 KUiccTrIdServiceTableByte4( 0xFC );
const TUint8 KUiccTrIdServiceTableByte5( 0xF9 );
const TUint8 KUiccTrIdServiceTableByte7( 0xFD );
const TUint8 KUiccTrIdServiceTableByte6( 0xFF );

// PollInterval, PollingOff
const TUint8 KDefaultPollInterval( 0x00 );
const TUint8 KMinPollInterval( 0x05 );
const TUint8 KMaxPollInterval( 0xFF );

const TUint8 KBlackAndWhite( 0x11 ); // Image Coding Scheme



// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CTsySatMessaging;
class CBerTlv;

// CLASS DECLARATION

/**
*  SAT message handler class.
*  Handles message sending and receiving to and from phonet.
*
*  @lib simatktsy.lib
*  @since Series60_ver 2.6
*/
class CSatMessHandler : public CBase, public MMmMessageReceiver
    {
    public: // Data types

        //Type of SIM card
        enum TIccType
            {
            EIccTypeSim2GGsm,
            EIccTypeSim3G,
            EIccTypeSimUnknown
            };

        //Ss and Ussd status
        enum TSsStatus
            {
            ENotBusy,
            ESsBusy,
            EUssdBusy
            };

        // Location information
        struct TLocationInfo
            {
            TInt            iRegStatus; // Registration status
            TBuf8<2>        iLac;       // Location Area Code
            TBuf8<4>        iCellId;
            TBuf8<3>        iOperatorCode;
            };


    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aTsySatMessaging pointer to TsySatMessaging class
        * @param aPnSend Phonetsender for passing messages to phonet
        */
        static CSatMessHandler* NewL( CTsySatMessaging* aTsySatMessaging,
            CMmPhoNetSender* aPnSend );

        /**
        * Destructor.
        */
        virtual ~CSatMessHandler();


    private:    // Constructors

        /**
        * C++ default constructor.
        * @param aTsySatMessaging pointer to TsySatMessaging class
        * @param aPnSend Phonetsender for passing messages to phonet
        */
        CSatMessHandler( CTsySatMessaging* aTsySatMessaging,
            CMmPhoNetSender* aPnSend );


    public:     // New methods

        /**
        * Display text response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aGeneralResult result of the proactive command display text
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt DisplayTextTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Get Inkey response method.
        * @since NCP 3.1
        * @param aTransId Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aRsp Response packet from Etel
        * @return phonet sender status
        */
        TInt GetInkeyTerminalResp( const TUint8 aTransId,
            const TDes8& aCommandDetails, const RSat::TGetInkeyRspV2& aRsp );

        /**
        * Get Input response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details tlv
        * @param aGeneralResult result of the proactive command Get Input
        * @param aAdditionalInfo Additional info response might give
        * @param aTextString text string provided by the user
        * @param aDataCodingScheme data coding scheme for text string
        * @return phonet sender status
        */
        TInt GetInputTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo,
            TDesC16& aTextString, TUint8 aDataCodingScheme );

        /**
        * Play Tone response method.
        * @since Series60_ver 2.6
        * @param Unique transaction ID
        * @param aCommandDetails
        * @param aGeneralResult result of the proactive command
        * @param aAdditionalInfo Additional info given in some cases
        * @return phonet sender status
        */
        TInt PlayToneTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Setup Menu response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aGeneralResult result of the proactice command Setup Menu
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt SetUpMenuTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Select Item response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aGeneralResult result of the proactice command Select Item
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt SelectItemTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Send SM response method.
        * @since Series60_ver 2.6
        * @param aPCmdNumber Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aGeneralResult result of the proactice command Send SM
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt SendSmTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Send SS response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactive command Send SS
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt SendSsTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TDesC8& aAdditionalInfo );

        /**
        * Send DTMF response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aPCmdNumber Command number
        * @param aGeneralResult result of the proactice command Send SS
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt SendDtmfTerminalResp( TUint8 aTransId,  TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Send USSD response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails command details of the PCmd
        * @param aGeneralResult result of the proactive command Send USSD
        * @param aAdditionalInfo Additional info the response might give
        * @param aUssdData Ussd text string, without the DCS byte
        * @param aUssdCbsDataCodingScheme, DCS byte of the USSD data
        * @return phonet sender status
        */
        TInt SendUssdTerminalResp( TUint8 aTransId,  TDes8& aCommandDetails,
            TUint8  aGeneralResult, TUint8 aAdditionalInfo,
            TDes& aUssdData, TUint8 aUssdCbsDataCodingScheme );

        /**
        * Polling Interval response method
        * @since Series60_ver 2.8
        * @param aPCmdNumber Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aGeneralResult result of the proactive command
        * @param aAdditionalInfo Additional info response might give
        * @param aNumOfUnits time interval
        * @return phonet sender status
        */
        TInt PollIntervalTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo,
            TUint8 aNumOfUnits );

        /**
        * Setup Call response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactive command Setup Call
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt SetUpCallTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Refresh response method.
        * @since Series60_ver 2.6
        * @param aPCmdNumber Unique transaction ID
        * @param aGeneralResult result of the proactice command Refresh
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt RefreshTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * Menu Selection method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aItemIdentifier identifier of the selected menu item
        * @param aHelp help requested by the user indicator
        * @return phonet sender status
        */
        void MenuSelectionInd( TUint8 aTransId, TUint8 aItemIdentifier,
            TUint8 aHelp );

        /**
        * Launch Browser terminal response method
        * @since Series60_ver 2.6
        * @param aTransId transaction id
        * @param aPCmdNumber command number
        * @param aGeneralResult
        * @param aAdditionalInfo
        * @param aCmdQualifier
        * @return phonet sender status
        */
        TInt LaunchBrowserTerminalResp( TUint8 aTransId,
            TDes8& aCommandDetails, TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
        * Test if a command was performed successfully
        * @since Series60_ver 2.6
        * @param aGeneralResult
        * @return ETrue if command performed successfully, else EFalse
        */
        TBool CommandPerformedSuccessfully( TUint8 aGeneralResult );

        /**
        * SetupIdleModeText Terminal Response
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aGeneralResult result of the proactive command display text
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt SetUpIdleModeTextTerminalResp( TUint8 aTransId,
            TDes8& aCommandDetails, TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
        * SetUpEventList response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aGeneralResult result of the proactive command display text
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */

        TInt SetUpEventListTerminalResp( TUint8 aTransId,
            TDes8& aCommandDetails, TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
        * PollingOff response method.
        * @since Series60_ver 2.6
        * @param aPCmdNumber Unique transaction ID
        * @param aCommandDetails command details tlv
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt PollingOffTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aAdditionalInfo );

        /**
        * LocalInfo response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactice command
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt LocalInfoTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TDes& aAdditionalInfo );

       /**
        * Timer management response method.
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactive command
        * @param aTimerValue[3]: timer value in hours, minutes and seconds
        * @param aTimerId: timer identifier
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt TimerMgmtTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TUint8 aTimerValue[3], TUint8 aTimerId,
            TUint8 aAdditionalInfo );

        /**
        * Timer expiration indication
        * @since Series60_ver 2.6
        * @param TUint8 aTransId: Unique transaction ID
        * @param TUint8 aTimerId: timer identification
        * @param TUint8 aTimerValue: timer value
        * @return TInt:
        */
        TInt TimerExpirationInd ( TUint8 aTransId, TUint8 aTimerId,
            TUint8 aTimerValue[3] );

        /**
        * More time response
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactive command
        * @return phonet sender status
        */
        TInt MoreTimeTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult );

        /**
        * Open Channel response method
        * @since Series60_ver 3.0
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactive command
        * @param aAdditionalInfo Additional info response might give
        * @param aBearerType
        * @param aBearer
        * @param aBufferSize
        * @return phonet sender status
        */
        TInt OpenChannelTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, TDes8& aAdditionalInfo, TUint8 aBearerType,
            TDesC8& aBearer, TUint16 aBufferSize );

        /**
        * Get Channel Status response method.
        * @since Series60_ver 3.0
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactice command
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt GetChannelStatusTerminalResp( TUint8 aTransId,
            TDes8& aCommandDetails, TUint8 aGeneralResult,
            const TDes8& aAdditionalInfo );

        /**
        * Close Channel response method.
        * @since Series60_ver 3.0
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactice command
        * @param aAdditionalInfo Additional info response might give
        * @return phonet sender status
        */
        TInt CloseChannelTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, const TDes8& aAdditionalInfo );

        /**
        * Receive Data response method.
        * @since Series60_ver 3.0
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactice command
        * @param aAdditionalInfo Additional info response might give
        * @param aChannelDataLength
        * @return phonet sender status
        */
        TInt ReceiveDataTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, const TDes8& aAdditionalInfo,
            TUint8 aChannelDataLength );

        /**
        * Send Data response method.
        * @since Series60_ver 3.0
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactice command
        * @param aAdditionalInfo Additional info response might give
        * @param aChannelDataLength
        * @return phonet sender status
        */
        TInt SendDataTerminalResp( TUint8 aTransId, TDes8& aCommandDetails,
            TUint8 aGeneralResult, const TDes8& aAdditionalInfo,
            TUint8 aChannelDataLength );

        /**
        * SendTerminalResponse
        * @since Series60_ver 2.6
        * @param aCommandDetails Command details TLV
        * @param aBerTlvSpecificData data to be sent
        * @return phonet sender status
        */
        TInt SendTerminalResponse( const TDes8& aCommandDetails,
            const TDesC8& aBerTlvSpecificData,
            const TUint8 aTransId = 0 );

        /**
        * Sends an empty terminal response to the SIMSON
        * @param aTransId transaction id / should be same as in REQ
        * @return phonet sender status
        */
        TInt SendEmptyTerminalRsp( const TInt aTransId  );

        /**
        * send UICC_CAT_REQ
        * @since Series60_ver 3.0
        * @param aTransId Transaction id of envelope
        * @param aBerTlv TLV data of the envelope command
        * @param aStore Decides wether the provided envelope
        *  is stored for resending or not.
        * @return phonet sender status
        */
        TInt UiccCatReqEnvelope( TUint8 aTransId, const TDesC8& aBerTlv,
            TBool aStore = ETrue );

        /**
        * Send a Sms-Pp report to SMS server, with subblocks
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction Id
        * @param aData data to be appended to the request
        * @return None
        */
        void SendSmsReportReq( TUint8 aTransId, TDesC8& aData );

        /**
        * Send a request to SMS server
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction Id
        * @param aMsgId Message identifier of the request
        * @param aData data to be appended to the request
        * @return None
        */
        void SmsResoureConfReq( TUint8 aTransId, TUint8 aMsgId, TDesC8& aData );

        /**
        * Breaks a INFO_SERIAL_NUMBER_READ_RESP ISI-message
        * @since Series60_ver 3.0
        * @param TIsiReceiveC& aIsiMessage response to envelope
        * @return none
        */
        void InfoSerialNumberReadResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Send NET_NEIGHBOUR_CELLS_REQ message to Phonet.
        * @since Series60_ver 2.6
        * @param aTransId a unique transaction id
        * @param aCellInfoType NET_NEIGHBOUR_CELL_INFO_TYPE
        * @return TInt: Phonet sender status
        */
        TInt NetNeighbourCellsReq( TUint8 aTransId, TUint8 aCellInfoType );

        /**
        * Breaks a NET_NEIGHBOUR_CELLS_RESP ISI-message
        * @since Series60_ver 3.0
        * @param aIsiMessage response to envelope
        * @return none
        */
        void NetNeighbourCellResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Sends a CALL_MODEM_RESOURCE_REQ ISI-message to modem Call server.
        * @param sTransId Unique transaction id
        * @param aMsg ISI message data to be sent
        * @return TInt: Phonet sender status
        */
        TInt CallModemResourceReq( TUint8 aTransId, const TDesC8& aMsg );

        /**
        * Sends a CALL_MODEM_RESOURCE_CONF_REQ ISI-message to modem Call server
        * @param aResourceId resource to be controlled
        * @param aResourceIdMask mask for resource to be controlled
        * @return TInt: Phonet sender status
        */
        TInt CallModemResourceConfReq( TUint16 aResourceId, TUint16 aResourceIdMask );

        /**
        * Sends a SS_RESOURCE_CONF_REQ ISI-message to modem SS server.
        * @return TInt: Phonet sender status
        */
        TInt SsResourceConfReq();

        /**
        * Sends a SS_RESOURCE_CONTROL_REQ ISI-message to modem SS server.
        * @param aTransId Unique transaction id
        * @param aMsg ISI message data to be sent
        * @return TInt: Phonet sender status
        */
        TInt SsResourceControlReq(
            TUint8 aTransId,
            const TDesC8& aMsg
            );

        /**
        * Sends a GPDS_RESOURCE_CONF_REQ ISI-message to modem SS server.
        * @return TInt: Phonet sender status
        */
        TInt GpdsResourceConfReq();

        /**
        * Sends a GPDS_RESOURCE_CONTROL_REQ ISI-message to modem SS server.
        * @param aTransId Unique transaction id
        * @param aMsg ISI message data to be sent
        * @return TInt: Phonet sender status
        */
        TInt GpdsResourceControlReq(
            TUint8 aTransId,
            const TDesC8& aMsg
            );

        /**
        * Set Polling Interval, sending a request to sim server
        * @since Series60_ver 2.6
        * @param aTraId Transaction Id
        * @param aValue in seconds
        * @return none
        */
        void SetPollingInterval( TUint8 aTraId, TUint8 aValue );

        /**
        * Method to check SIM server response to a refresh request
        * @since Series60_ver 2.6
        * @param aStatus SIM server status
        * @return none
        */
        void RefreshResult( TUint8 aStatus );

        /**
        * Method to check SIM server response to a set polling request
        * Called by SimAtkRespL
        * @since Series60_ver 2.6
        * @param aStatus SIM server status
        * @param aTransId Transaction Id
        * @param aInterval obtained from the ISI message of the response
        * @param None
        */
        void SetPollingResult( TUint8 aStatus, TUint8 aTransId,
            TUint8 aInterval );

        /**
        * Method to check that proactive command is in correct format.
        * @since NCP 5.0
        * @param aIsiMessage
        * @return KErrNone or KErrCorrupt
        */
        TInt CheckProactiveCommand( const TIsiReceiveC& aIsiMessage );

        /**
        * Send a SMS_CB_ROUTING_REQ ISI-message
        * @since Series60_ver 2.6
        * @param aTransId
        * @param aRoutingCommand
        * Return value: phonet sender return value
        */
        TInt SmsCbRoutingReq( TUint8 aTransId, TUint8 aRoutingCommand );

        /**
        * Breaks a SMS_CB_ROUTING_RESP ISI-message
        * @since NCP 3.1
        * @param aIsiMessage Message from SMS server
        * Return value: none
        */
        void SmsCbRoutingResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Breaks a SMS_RESOURCE_CONF_IND ISI-message
        * @since NCP 3.1
        * @param aIsiMessage Message from SMS server
        * Return value: none
        */
        void SmsResourceConfInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates InfoSerialNumberReadReq ISI message and sends it to Phonet
        * @since Series60_ver 3.0
        * @param aTransId: unique transaction id
        * @param aTarget: requested data
        * return value TInt : success/failure value
        */
        TInt InfoSerialNumberReadReq( TUint8 aTransId, TUint8 aTarget );

        /**
        * Creates NET_CELL_INFO_GET_REQ ISI message and sends it to Phonet
        * @param aTransId: unique transaction id
        * return value TInt: success/failure value
        */
        TInt NetCellInfoGetReq( TUint8 aTransId );

        /**
        * Breaks a NET_CELL_INFO_GET_RESP ISI-message
        * @param aIsiMessage Net cell info get resp ISI message
        * Return value: none
        */
        void NetCellInfoGetResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Sends a GSS_CS_SERVICE_REQ isi-message to phonet
        * @since Series60_ver 2.6
        * @param aTransId Transaction ID
        * @param TUint8 aOperation: Gss operation
        * @retrun TInt: Phonet sender status
        */
        TInt GssCsServiceReq( TUint8 aTransId, TUint8 aOperation );

        /**
        * Breaks a GSS_CS_SERVICE_RESP ISI-message
        * @since Series60_ver 3.0
        * @param aIsiMessage ISI message from GSS server
        * Return value: none
        */
        void GssCsServiceResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Breaks a GSS_CS_SERVICE_FAIL_RESP ISI-message
        * @since Series60_ver 3.0
        * @param aIsiMessage ISI message from GSS server
        * Return value: none
        */
        void GssCsServiceFailResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Language Notification response
        * @since Series60_ver 2.6
        * @param aTransId Unique transaction ID
        * @param aCommandDetails Command details TLV
        * @param aGeneralResult result of the proactive command
        * @param aLanguage
        * @return phonet sender status
        */
        TInt LanguageNotificationTerminalResp( TUint8 aTransId,
            TDes8& aCommandDetails, TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
        * Breaks a NET_TIME_IND isi-message
        * @since Series60_ver 3.0
        * @param aIsiMessage received indication
        * @return none
        */
        void NetTimeInd( const TIsiReceiveC& aIsiMessage );


        /**
        * Sends a NET_RAT_REQ isi-message to phonet
        * @since Series60_ver 2.6
        * @param aTransId Transaction ID
        * @retrun TInt: Phonet sender status
        */
        TInt NetRatReq( TUint8 aTransId );

        /*
        * Breaks a NET_RAT_RESP ISI-message
        * @since Series60_ver 3.0
        * @param aIsiMessage Net rat resp ISI message
        * Return value: none
        */
        void NetRatResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Send a response to SMS server after a SMS resource Indication received from
        * SMS sever.
        * This response tells whether the SMS can be sent or not.
        * @since Series60_ver 2.6
        * @param aTransId Transaction id, same as in the request
        * @param aData Data to be sent
        * @return none
        */
        TInt SendSmsResourceReq( TUint8 aTransId, TDesC8& aData,
            TUint8 aReceiverObject );

        /**
        * Breaks a SS_STATUS_IND isi-message
        * @since Series60_ver 3.0
        * @param TIsiReceiveC& aIsiMessage received indication
        * @return none
        */
        void SsStatusInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Helper method to find out does the given result require an additional
        * info byte(s) in terminal response
        * @param aGeneralResult
        * @return ETrue if result needs an additional info byte(s), else EFalse
        */
        TBool AdditionalInfoNeeded( const TUint8 aGeneralResult );

        /**
        * Breaks a NET_RAT_IND isi-message
        * @since Series60_ver 3.0
        * @param aIsiMessage received indication
        * @return none
        */
        void NetRatInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Stores Network service status to an internal structure
        * @param aIsiMessage NetCellInfoInd or NetCellInfoGetResp
        * @return none
        */
        void StoreNetServiceStatus( const TIsiReceiveC& aIsiMessage );

        /**
        * Getter for location information
        * @since NCP 5.0
        * @return Location Information structure
        */
        const TLocationInfo& LocationInfo();

        /**
        * Sends a INFO_PP_READ_REQ isi-message to phonet
        * @since NCP 5.0
        * @param aReqType Product profile feature being requested
        * @retrun TInt: Phonet sender status
        */
        TInt InfoPpReadReq( const TUint8 aReqType );

        /**
        * Breaks a INFO_PP_READ_RESP isi-message
        * @since Series60_ver 5.0
        * @param aIsiMessage received indication
        * @return none
        */
        void InfoPpReadResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Validates lengths of simple tlv objects inside BER TLV
        * object
        * @param berTlv BER TLV object
        * @retrun TInt: Validation status
        */
        TInt CheckTlvObjects( CBerTlv& berTlv );

        /**
        * Activate or DeActivate MO SMS Control on SMS Server according to SIM
        * Server Response
        * @param MO SM control status in SIM card
        * @return none
        */
        void SimMoSmsControlAvail( TUint8 aStatus );

        /**
        * Send UICC_CAT_REQ(UICC_SB_TERMINAL_PROFILE) ISI message
        * @return none
        */
        void UiccTerminalProfileReq();

        /**
        * Send UICC_APPL_CMD_REQ ISI message for reading data
        * from service table of UICC
        * @param aTrId Transaction ID
        * @param aFileOffset Offset of service table to be read
        * @return Error code
        */
        TInt UiccReadServiceTableReq( TUint8 aTrId, TUint16 aFileOffset );

        /**
        * Send UICC_APPL_CMD_REQ ISI message for reading data
        * from EF img
        * @param aRecordNumber Record number
        * @param aFileOffset File Offset
        * @param aDataAmount Data amount
        * @param aTrId Transaction ID
        * @return Error code
        */
        TInt UiccReadSatIconReq(
            TUint8 aRecordNumber,
            TUint8 aFileOffset,
            TUint8 aDataAmount,
            TUint8 aTrId );

        /**
        * Send UICC_APPL_CMD_REQ ISI message for reading image instance
        * @param aFileId File ID
        * @return Error code
        */
        TInt UiccReadSatIconInstanceReq( TUint16 aFileId );

        /**
        * Send UICC_CAT_REQ ISI message
        * @param aCommand Command
        * @return Error code
        */
        TInt UiccCatReq( TUint8 aCommand );

        /**
        * Send UICC_CAT_REQ ISI message for refresh command
        * @param aTransId Transaction ID
        * @param aServiceType Service type
        * @param aFileList File list
        * @param aAid Application ID
        * @return Error code
        */
        TInt UiccCatReqRefresh(
            const TUint8 aTransId,
            const TUint8 aServiceType,
            TDes8& aFileList,
            const TDesC8& aAid );

        /**
        * Send UICC_CAT_REQ ISI message for terminal response
        * @param aCommandDetails Command details
        * @param aBerTlvSpecificData BER-TLV data
        * @param aTransId Transaction ID
        * @return Error code
        */
        TInt UiccCatReqTerminalResponse(
            const TDes8& aCommandDetails,
            const TDesC8& aBerTlvSpecificData,
            const TUint8 aTransId = 0 );

        /**
        * Handle UICC_CAT_RESP ISI message for envelope response
        * @param aIsiMessage Received ISI message
        * @return Information about handling of the message
        */
        TBool UiccCatRespEnvelope( const TIsiReceiveC& aIsiMessage );

        /**
        * Handle UICC_CAT_RESP ISI message for terminal response
        * @param aIsiMessage Received ISI message
        * @return Information about handling of the message
        */
        TBool UiccCatRespTerminalResponse( const TIsiReceiveC& aIsiMessage );

        /**
        * Get application file ID
        * @return Application file ID
        */
        const TDesC8& GetApplicationFileId();

        /**
        * Set status of iSatReady flag
        * @param aSatReadyStatus SAT status
        * @return None
        */
        void SetSatReadyStatus( TBool aSatReadyStatus );

        /**
        * Get status of iSatReady flag
        * @return SAT status
        */
        TBool GetSatReadyStatus();

        /**
        * Get status of iTerminalProfileSent flag
        * @return Terminal profile status
        */
        TBool GetTerminalProfileStatus();

        /**
        * Get clut data
        * @return Clut data
        */
        const TDesC8& GetClut();

    public: // Inline functions

        /**
        * Returns time zone information received from network.
        * @since NCP 5.0
        * @param none
        * @return TUint8 Time zone
        */
        inline TUint8 TimeZone() const;

        /**
        * Method to check if IMEI is successfully received
        * @since NCP 5.0
        * @return ETrue if IMEI is known, otherwise EFalse
        * @param None
        */
        inline TBool ImeiAvailable() const;

        /**
        * Method to check if IMEISV is successfully received
        * @return ETrue if IMEISV is known, otherwise EFalse
        * @param None
        */
        inline TBool ImeiSvAvailable() const;

        /**
        * Method to check Location information status
        * @since NCP 5.0
        * @return ETrue or EFalse
        * @param None
        */
        inline TBool ServiceAvailable() const;

        /**
        * Get Polling off status
        * @since NCP 5.0
        * @return Return ETrue if Polling is set off,
        * and false after a polling interval pcmd
        */
        inline TBool PollingOff() const;

        /**
        * Set Polling off status
        * @since NCP 5.0
        * @param aPollingOff true after a Polling Off,
        * and false after a polling interval pcmd
        * @return none
        */
        inline void SetPollingOff( const TBool aPollingOff );

        /**
        * Get Ss status
        * @since NCP 5.0
        * @return status indicating if Ss or Ussd transaction ongoing
        */
        inline TSsStatus SsStatus() const;

        /**
        * Get current access technology
        * @since NCP 5.1
        * @return current access technology
        */
        inline TUint8 CurrentAccessTechnology() const;

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
        /**
        * Get current status of INFO_PP_ENHANCED_NETWORK_SELECTION PP flag
        * @return current status of INFO_PP_ENHANCED_NETWORK_SELECTION PP flag
        */
        inline TBool EnsSupported() const;
#endif

        /**
        * Get Old Polling interval status
        * @return Return ETrue if Old Polling is set on,
        * and EFalse otherwise
        */
        inline TBool OldPollInterval() const;

    public: // Functions from MMmMessageReceiver-interface

        /**
        * Called by phonet receiver when an ISI message has
        * been received
        * @since Series60_ver 3.0
        * @param aIsiMessage The received ISI message
        * @return none
        */
        virtual void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles errors which comes come from PhoNetReceiver RunError
        * @param aIsiMessage The received ISI message
        * @param aError Error code
        * @return none
        */
        virtual void HandleError( const TIsiReceiveC& /*aIsiMessage*/,
            TInt /*aErrorCode*/ );


    private:    // Constructors

        /**
        * Copy constructor, usage not allowed
        * @param Satmesshandler reference
        */
        CSatMessHandler( const CSatMessHandler& aRhs );

        /**
        * Assignment operator, usage not allowed
        * @param Satmesshandler reference
        * @return aRhs SatMessHandler reference
        */
        CSatMessHandler& operator = ( const CSatMessHandler& aRhs );

        /**
        * Constructor method for sat message handler.
        */
        void ConstructL();


    private:    // New methods

        /**
        * Handles the Net server related messages
        * @since Series60_ver 3.0
        * @param aIsiMessage Isi message from net server
        * @return none
        */
        void NetServerMessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the Phone Info server related messages
        * @since Series60_ver 3.0
        * @param aIsiMsg proactive command
        * @return none
        */
        void PhoneInfoMessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the UICC server related messages
        * @param aIsiMessage received ISI message
        * @return Information about handling of response
        */
        TBool UiccServerMessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the Sms server related messages
        * @since NCP 3.1
        * @param aIsiMessage Message from SMS server
        * @return none
        */
        void SmsServerMessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the Ss server related messages
        * @since Series60_ver 3.0
        * @param aIsiMsg proactive command
        * @return none
        */
        void SsServerMessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles the GSM Stack server related messages
        * @since Series60_ver 3.0
        * @param aIsiMessage ISI message from GSS server
        * @return none
        */
        void GsmStackServerMessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles proactive procedures, i.e Call Control, Data Download,
        * Event Download and MO-SMS Control related messages
        * @since NCP 3.1
        * @param aIsiMessage received ISI message
        * @return none
        */
        void ProactiveProcedureMessageReceivedL(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Sends a PN_ATK message with the data given as parameter
        * @since NCP 5.1
        * @param aReceiverObject: Receiver object ID
        * @param aTransactionId: Message Transaction ID
        * @param aMessageId: Message identifier
        * @param aData: Message data to be sent
        * @return Error/Success value from sending the message
        */
        TInt CSatMessHandler::SendPnAtkMessage( const TUint8 aReceiverObject,
            const TUint8 aTransactionId, const TInt aMessageId,
            const TDesC8& aData );

        /**
        * Breaks UICC_APPL_CMD_RESP ISI message
        * @param aReceiverObject: Receiver object ID
        * @return None
        */
        void UiccApplCmdResp( const TIsiReceiveC& aIsiMessage );

        /**
        * Breaks a UICC_CAT_IND ISI-message
        * @param aIsiMessage Received ISI message
        * @return None
        */
        void UiccCatInd( const TIsiReceiveC& aIsiMessage );

    private: // Data

        // Pointer to SAT messaging object
        CTsySatMessaging*   iTsySatMessaging;

        // Pointer to Phonet Sender
        CMmPhoNetSender*    iPnSend;

        // Storage for terminal resp transaction id.
        TInt                iTerminalRespTraId;

        // Save the SimReadField transaction id used by GetIcon.
        TInt                iGetIconSimReadFieldTraId;

        // Save the SimReadField transaction id used by DataDownload.
        TInt                iDataDownloadSimReadFieldTraId;

        // Variable for storing card id
        TUint8              iCardId;

        // For storing IMEI code.
        TBuf8<16>           iIMEI;

        // For storing IMEI-SV BCD string
        TBuf8<9>           iIMEIsv;

        // For storing NMR
        TBuf8<16>           iNMR;

        // For storing BCCH channel list
        TBuf<32>            iBCCHChannelList;

        // For checking if IMEI code has been received
        TBool               iImeiAvailable;

        // For checking if IMEISV code has been received
        TBool               iImeiSvAvailable;

        // Stores the state of SMS CB routing request
        TBool               iSmsCbRoutingComplete;

        // Refresh proactive command is currently active flag.
        TBool               iRefreshRequestIsOn;

        // Variable for storing Timing Advance
        TUint8              iTimingAdvance;

        // Variable for storing Timing advance Status
        TUint8              iTaStatus;

        // Time zone information from Network. 0xFF if not received.
        TUint8              iTimeZone;

        // SIM Server's object Id
        TUint8              iSimServerObjectId;

        // Variable for storing Current Access Technology
        TUint8              iCurrentAccTech;

        // Flag is true after a Polling Off, and false after a polling
        // interval pcmd
        TBool               iPollingOff;

        // status indicating if Ss or Ussd transaction ongoing
        TSsStatus           iSsStatus;

        // Flag is set true if requested Poll Interval
        // is longer than KMaxLegacyPollInterval
        TBool               iLongPollIntervalReq;

        // Location Information
        TLocationInfo       iLocInfo;

        // UTRAN Measurement results
        TPtrC8              iUtranNMR;

#ifdef INFO_PP_ATK_MO_SM_CONTROL
        // Boolean to check if MO SMS control is supported in product profile
        TBool               iMoSmsSupportedInPp;
#endif // INFO_PP_ATK_MO_SM_CONTROL

        // For storing transaction id in product profile queries. Used only
        // with INFO_PP_ATK_MO_SM_CONTROL, but there can be several
        // simultaneous PP queries ongoing
        TUint8              iInfoPpMoSmsTraId;

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
        // Boolean to hold status of INFO_PP_ENHANCED_NETWORK_SELECTION in
        // product profile
        TBool               iEnsSupportedInPp;

        // Boolean to check if TR for SET UP EVENT LIST has been sent
        TBool               iSetUpEventListTrSent;
#endif // INFO_PP_ENHANCED_NETWORK_SELECTION

        // Previous registration status
        TUint8              iPreviousRegistrationStatus;

        // Boolean to check if  OLD_POLL_INTERVAL control is supported in product profile
        TBool               iOldPollIntervalSupportedInPp;

        // To store Subscription no
        TUint8              iSmsCbSubscriptionNo;

        // To store SAT server status
        TBool               iSatReady;

        // To store status of terminal profile
        TBool               iTerminalProfileSent;

        // To store card type
        TUint8              iCardType;

        // To store application ID
        TUint8              iApplicationId;

        // Application file ID
        TBuf8<2>            iApplFileId;

        // To store UICC client ID
        TUint8              iUiccClientId;

        // Polling interval
        TUint8              iPollingInterval;

        // Image coding scheme
        TUint8              iImageCodingScheme;

        // Number of clut in image instance data
        TInt                iNumberOfClut;

        // Clut data
        TPtrC8              iClutData;

    };
#include "satmesshandler.inl"

#endif    // SATMESSHANDLER_H

// End of File
