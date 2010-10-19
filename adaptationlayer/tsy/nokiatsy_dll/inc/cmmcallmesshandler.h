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




#ifndef CMMCALLMESSAGEHANDLER_H
#define CMMCALLMESSAGEHANDLER_H

// INCLUDES
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include "nokiatsy_internal_variation.h"
#include <e32base.h>
#include <etelmm.h>
#include <tisi.h>
#include <ctsy/rmmcustomapi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

// CONSTANTS
// Maximum length of UUS sub block
// (defined in call server specification)
const TInt KUserToUserSbMaxLength = 248;

// Maximum length of telephony number
// (defined in call server specification)
const TInt KTelephonyNumberMaxLength = 123;

// Maximum length of ISA address sub block
// (defined in call server specification)
const TInt KAddressSubBlockMaxLength = 252;

// Hard-coded transaction id for ISI messages
const TUint8 KCallTransId = 1;

// Constant for not allocated Symbian call ID
const TInt KSymbianCallIdNone = -1;

// Maximum length of destination post address
const TInt KDestPostAddressMaxLength = 15;

// Bit mask costants
const TUint8 KMaskBits1to3 = 0x07;
const TUint8 KMaskBits1to4 = 0x0F;
const TUint8 KMaskBits5to7 = 0x70;
const TUint8 KMaskBits6to7 = 0x60;
const TUint8 KMaskBits1to7 = 0x7F;
const TUint8 KMaskBit8 = 0x80;

// These OBJ ID is used in pipe direction.
// Pipe-End-Point, Circuit Switched Data server
#define PN_OBJ_CSD_SRV       0x31

// MACROS
//  None

// DATA TYPES
//  None

// FUNCTION PROTOTYPES
//  None

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmPhoNetSender;
class CMmDataPortHandler;
class CMmDtmfMessHandler;
class CMmMessageRouter;

// CLASS DECLARATION

/**
* CEmergencyTimer: Used for delaying emergency call when PDP context deactivate
* is ongoing
*/
class CEmergencyTimer : public CTimer
{
    class CMmCallMessHandler& iCallMsgHandler;

public:

    /**
    * Constructor
    */
    CEmergencyTimer( CMmCallMessHandler& aCallMsgHandler );

    /**
    * Destructor.
    */
    ~CEmergencyTimer();

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

    /**
    * From CActive.
    * Called when timer elapses.
    */
    void RunL();

    /**
    * Start emergency delay timer
    */
    void StartEmergencyTimer();

    /**
    * Stop emergency delay timer
    */
    void StopEmergencyTimer();
};

// CLASS DECLARATION

/**
*  Call related ISI message handler
*  Used for creating and sending call ISI messages to
*  PhoNet via PhoNetSender. It also receives call
*  ISI messages from PhoNetReceiver.
*  @lib (unknown)
*  @since Series S60 2.6
*/
class CMmCallMessHandler : public CBase,
    public MMmMessHandlerBase, public MMmMessageReceiver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPhoNetSender Pointer to the phonet sender
        * @param aPhoNetReceiver Pointer to the phonet receiver
        * @param aDtmfMessHandler Pointer to the DTMF message handler
        * @param aMessageRouter Pointer to the message router
        * @return Pointer to the call message handler
        */
        static CMmCallMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmDtmfMessHandler* aDtmfMessHandler,
            CMmMessageRouter* aMessageRouter );

        /**
        * Destructor.
        */
        ~CMmCallMessHandler();

    public: // New functions

        /**
        * Checks if the received message should be handled by the
        * call message handler
        * @param aIsiMessage Reference to the received ISI message.
        * @return void
        */
        void ReceiveMessageL( const TIsiReceiveC &aIsiMessage );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param TIsiReceiveC& The received ISI message
        * @param aError Error code
        * @return void
        */
        void HandleError( const TIsiReceiveC&, TInt aError );

        /**
        * Initialize Dataport
        * @return void
        */
        void InitializeDataportL();

    public: // Functions from base classes

        /**
        * From MMmMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param aIpc IPC number of the request
        * @param aDataPackage datapackage from CommonTSY
        * @return KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );


    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * C++ default constructor.
        */
        CMmCallMessHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates CALL_CREATE_REQ ISI message and sends it to Phonet.
        * @return KErrNone or error code
        */
        TInt CallCreateReq();

        /**
        * Breaks received CALL_CREATE_RESP ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallCreateResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates CALL_CREATE_REQ ISI message for emergency calls.
        * @param aCallReq Reference to the message to be sent.
        * @return void
        */
        void EmergencyCallCreateReq( TIsiSend* aCallReq );

        /**
        * Breaks received CALL_ANSWER_RESP ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallAnswerResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates CALL_RELEASE_REQ ISI message and sends it to PhonetSender.
        * @param aCallReq Reference to the message to be sent
        * @param aDataPackage Input data from the Symbian OS layer
        * @return void
        */
        void CallReleaseReq(
            TIsiSend* aCallReq,
            const CCallDataPackage* aDataPackage );

        /**
        * Breaks received CALL_RELEASE_RESP ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallReleaseResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks received CALL_RELEASE_IND ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallReleaseInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks CALL_TERMINATED_IND ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallTerminatedInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks received CALL_COMING_IND ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallComingInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates call control request (holding a call etc.) ISI
        * message and sends it to PhonetSender.
        * @param aCallReq Reference to the message to be sent
        * @param aIpc IPC number of the request
        * @return void
        */
        void CallControlReq( TIsiSend* aCallReq, TInt aIpc ) const;

        /**
        * Breaks received CALL_CONTROL_RESP ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallControlResp( const TIsiReceiveC &aIsiMessage ) const;

        /**
        * Breaks received CALL_CONTROL_IND ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallControlInd( const TIsiReceiveC &aIsiMessage ) const;

        /**
        * Creates and sends call property set request to PhonetSender
        * @param aCallReq Reference to the message to be sent
        * @param aDataPackage Input data from the Symbian OS layer
        * @return void
        */
        void CallPropertySetReq(
            TIsiSend* aCallReq,
            const CCallDataPackage* aDataPackage );

        /**
        * Breaks received CALL_PROPERTY_SET_RESP ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallPropertySetResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks received call status indication ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallStatusIndL( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks received call GSM notification indication
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallGsmNotificationInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Resets all fields of incoming mobile call info.
        * @param aIncomingCallInfo Reference to the mobile call info info
        */
        void ResetIncomingCallInfo(
            RMobileCall::TMobileCallInfoV1& aIncomingCallInfo ) const;


        /**
        * Creates sub block for CALL_CREATE_REQ
        * @param aMode Mode of the call to be dialed (voice, csd, etc.)
        * @param aTelNumber Phone number to be called
        * @param aIdRestrict Caller id restriction
        * @param aCallIsiMsg Call IsiMsg
        * @param numOfSbInMsg Number of subblocks
        * @param aCurrentMsgOffset Current msg offset
        * @return void
        */
        void GetCallCreateReqSubBlock(
            TUint8 aMode,
            const TDesC16& aTelNumber,
            RMobileCall::TMobileCallIdRestriction aIdRestrict,
            TIsiSend& aCallIsiMsg,
            TUint8& numOfSbInMsg,
            TInt &aCurrentMsgOffset );

        /**
        * Constructs SAT sub blocks for CALL_CREATE_REQ ISI message.
        * @param aSubAddress Called party sub address
        * @param aBearer Bearer capability sub address
        * @param aCallIsiMsg Call IsiMsg
        * @param aNumOfSbInMsg Number of subblocks
        * @param aCurrentMsgOffset Current msg offset
        * @return void
        */
#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
        void GetSatOriginatedSubBlocks(
            TIsiSend& aCallIsiMsg,
            TUint8& aNumOfSbInMsg,
            TInt& aCurrentMsgOffset ) const;
#else
        void GetSatOriginatedSubBlocks(
            const TBuf<KSubAddressSize>& aSubAddress,
            TBuf8<KBearerCapabilitySize>& aBearer,
            TIsiSend& aCallIsiMsg,
            TUint8& aNumOfSbInMsg,
            TInt& aCurrentMsgOffset ) const;
#endif

        /**
        * Maps ISI call status to Symbian OS call status
        * @param aCallStatus ISI call status
        * @return Symbian OS call status
        */
        RMobileCall::TMobileCallStatus MapCallStatus( TUint8 aCallStatus );

        /**
        * Maps ISI call mode to Symbian OS call mode
        * @param aMobileCallInfo Call info
        * @param aIsiCallMode ISI call mode
        * @return void
        */
        void FillModeData(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TUint8 aIsiCallMode ) const;

        /**
        * Maps ISI values to Symbian OS numbering plan and number type
        * @param aMobileCallInfo Call info
        * @param aOrigAddressType ISI originated address type value
        * @return void
        */
        void FillNumberPlanAndType(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TUint aOrigAddressType ) const;

        /**
        * Maps ISI values to Symbian OS remote party status and number
        * @param aMobileCallInfo Call info
        * @param aOrigPresentInd ISI presentation information value
        * @param aOrigAddress ISI call origination address
        * @param aMobileCallDirection ISI call direction value
        * @return void
        */
        void FillRemoteStatusAndNumber(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TUint8 aOrigPresentInd,
            const TDes16& aOrigAddress,
            RMobileCall::TMobileCallDirection aMobileCallDirection) const;

        /**
        * Maps ISI values to calling name
        * @param aMobileCallInfo Call info
        * @param aOrigInfoPresentInd ISI presentation information value
        * @param aOrigInfoName ISI call origination name
        * @param aOrigInfoNumberSource ISI call origination information source
        * @return void
        */
        void FillCallingName(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TUint8 aOrigInfoPresentInd,
            const TDes16& aOrigInfoName ) const;

        /**
        * Breaks received CallServiceDeniedInd ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CallServiceDeniedInd( const TIsiReceiveC &aIsiMessage );


        /**
        * Checks call ISA status
        * @param aMobileCallInfo.
        * @param aCallStatusISA
        * @return void
        */
        void CheckCallIsaStatus(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TUint8 aCallStatusISA );

        /**
        * Read all address destination
        * @param aMobileCallInfo Call info
        * @param aTargetOrig Target origin
        * @param aIsiMessage Received ISI message
        * @param aSbStartOffSet Subblock start offset
        * @return void
        */
        void ReadAllAddressDestination(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TBuf16<RMobilePhone::KMaxMobileTelNumberSize>& aTargetOrig,
            const TIsiReceiveC &aIsiMessage,
            TUint& aSbStartOffSet,
            TUint8 origPresentInd);

        /**
        * Read all address connected (needed for COLP)
        * @param aMobileCallInfo Call info
        * @param aTargetOrig Target origin
        * @param aIsiMessage Received ISI message
        * @param aSbStartOffSet Subblock start offset
        * @return void
        */
        void ReadAllAddressConnected(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TBuf16<RMobilePhone::KMaxMobileTelNumberSize>& aTargetOrig,
            const TIsiReceiveC &aIsiMessage,
            TUint& aSbStartOffSet);

        /**
        * Read origin call info
        * @param aMobileCallInfo Call info
        * @param aTargetOrigName Target origin name
        * @param aIsiMessage Received ISI message
        * @param aSbStartOffSet Subblock start offset
        * @return void
        */
        void ReadCallInfo(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TBuf16<RMobileCall::KCallingNameSize>& aTargetOrigName,
            const TIsiReceiveC &aIsiMessage,
            TUint& aSbStartOffSet );

        /**
        * Complete incoming call notification
        * @param aMobileCallInfo Call info
        * @param aStatusETel ETel status
        * @return void
        */
        void CompleteIncomingCallNotif(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            RMobileCall::TMobileCallStatus& aStatusETel );

        /**
        * Complete Mobile Call and Info Change
        * @param aMobileCallInfo Call info
        * @param aStatusETel ETel status
        * @param aResultEtel ETel result
        * @return void
        */
        void CompleteMobileCallInfoAndStatusChange(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            RMobileCall::TMobileCallStatus& aStatusETel,
            TInt aResultETel );

        /**
        * Maps ISI values to Symbian OS cause of no cli
        * @param aMobileCallInfo Call info
        * @param aCauseOfNoCli Cause of no cli
        * @return void
        */
        void FillCauseOfNoCli(
            RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            TUint8 aCauseOfNoCli );

        /**
        * Activates user to user signaling service.
        * @param aDataPackage Packed UUS data
        * @return KErrNone or error value.
        */
        TInt ActivateUUS(
            const CMmDataPackage* aDataPackage );

        /**
        * Creates product profile request, in order to read
        * if HSDPA is disabled in PP.
        * @return void
        */
        void InfoPpDataReadReq();

        /**
        * Breaks received INFO_PP_DATA_READ_RESP ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void None
        */
        void InfoPpDataReadResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates GPDS_CONTEXTS_CLEAR request, in order to end
        * all data connections.
        * @return void
        */
        void GpdsContextsClearReq();

        /**
        * Breaks received GPDS_CONTEXTS_CLEAR_RESP ISI message,
        * and proceeds with emergency call dialling.
        * @return void None
        */
        void GpdsContextsClearResp();

        /**
        * Breaks received CSD_VIDEO_CALL_STATUS_IND  ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CsdVideoCallStatusInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks received CSD_MULTIMEDIA_DATA_RATE_IND ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CsdMultimediaDataRateInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates subblocks for CSD_CALL_CONTROL_REQ
        * @param aTelNumber Phone number to be called
        * @param aIdRestrict Caller id restriction
        * @param aCsdIsiMsg CSD IsiMsg
        * @param numOfSbInMsg Number of subblocks
        * @param aCurrentMsgOffset Current msg offset
        * @return void
        */
        void GetCSDCallControlReqSubBlock(
            const TDesC16& aTelNumber,
            RMobileCall::TMobileCallIdRestriction aIdRestrict,
            TIsiSend& aCsdIsiMsg,
            TUint8& aNumOfSbInMsg,
            TInt& aCurrentMsgOffset );

        /**
        * Creates CSD_CALL_CONTROL_REQ ISI message and sends it to PhonetSender.
        * @param aCallOperId Call Operation ID
        * @return TInt : KErrNone or error code
        */
        TInt CsdCallControlReq( TUint8 aCallOperId );

        /**
        * Breaks received CSD_CALL_CONTROL_RESP ISI message.
        * @param aIsiMessage Reference to the received message.
        * @return void
        */
        void CsdCallControlResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Dial data call.
        * @param CDataDataPackage* aCallDataPackage: call data package
        * @param TInt aIPC: EEtelCallDial or EMobileCallDialNoFdnCheck
        * @return TInt : KErrNone or error code
        */
        TInt DialDataCall( const CCallDataPackage* aCallDataPackage );

        /**
        * Answer incoming data call.
        * @param CDataDataPackage* aCallDataPackage: call data package
        * @return TInt KErrNone or error code
        */
        TInt AnswerIncomingDataCall( const CCallDataPackage* aCallDataPackage );

        /**
        * Hangs up the call
        * @param TInt aCallId: call id
        * @return TInt KErrNone or error code
        */
        TInt HangUp( const TInt aCallId );

        /**
        * Updates DialDataCall mobile call info.
        * @param TMobileCallInfoV1& aMobileCallInfo: Mobile call info
        * @param TBool aIsMultimedia: Is call mutimedia/video
        */

        void SetMobileCallInfo(
            const RMobileCall::TMobileCallInfoV1& aMobileCallInfo,
            const TBool aIsMultimedia = EFalse,
            const TBool aIsWaitingCall = EFalse );

        /**
        * Creates PnsPipeCreateReq ISI message and sends it to Phonet.
        * @return error value: Symbian error code
        */
        TInt PnsPipeCreateReq() const;

        /**
        * Creates PnsPipeRemoveReq ISI message and sends it to Phonet.
        * @return error value: Symbian error code
        */
        TInt PnsPipeRemoveReq();

        /**
        * PnsPipeCreate response.
        * @param aIsiMessage, reference to the received message.
        * @return void
        */
        void PnsPipeCreateResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Builds PNS_PIPE_ENABLE_REQ ISI message and sends it to Phonet.
        * @return error value: Symbian error code
        */
        TInt PnsPipeEnableReq() const;

        /**
        * Handles PNS_PIPE_ENABLE_RESP.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPipeEnableResp( const TIsiReceiveC &aIsiMessage );

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Pointer to the phonet sender
        CMmPhoNetSender* iPhoNetSender;

        // Pointer to the message router
        CMmMessageRouter* iMessageRouter;

        // Pointer to the DTMF message handler
        CMmDtmfMessHandler* iDtmfMessHandler;

        // Call releasing value, which is sent in call release request
        TUint8 iReleaseCauseValueSent;

        // Call mode of the latest requested call
        TUint8 iCallMode;

        // Call diagnostic resolution value between CALL_RELEASE_IND and
        // CALL_STATUS_IND handling
        TUint8 iDiagnosticOctet;

        // Call release cause value from CALL_TERMINATED_IND / KCallGsmMmCause
        TInt iGsmMmCauseError;

        // Flag for inband information
        TBool iDisconnectingWithInband;

        // Temporary call info required for incoming calls. The life cycle is
        // - In CALL_COMING_IND, call id and remote address are saved
        // - In CALL_GSM_NOTIFICATION_IND, iForwarded is saved (if applicable)
        // - In CALL_STATUS_IND, the saved information is read
        // - Reset in CALL_STATUS_IND (either if call is ringing, ISA status
        //   MtAlerting, or released by remote party before answering, ISA
        //   status MtRelease) or in CALL_RELEASE_REQ (MO release).
        RMobileCall::TMobileCallInfoV1 iIncomingCallInfo;

        // User to user data
        RMobileCall::TMobileCallUUSRequestV1 iUusData;

        // Buffer for received UUS data
        RMobileCall::TMobileCallUUI iReceivedUusData;

        // Variable to store call ID of waiting call for checking that
        // it is not notified again when it gets ringing.
        TInt iWaitingCall;

        // Subaddress required by dial phase 2
        TBuf<KSubAddressSize> iSubAddress;

        // Bearer required by dial phase 2
        TBuf8<KBearerCapabilitySize> iBearer;

        // Telephone number required by dial phase 2
        TBuf<RMobilePhone::KMaxMobileTelNumberSize> iTelNumber;

        // Emergency number
        TBuf<RMobilePhone::KMaxMobileTelNumberSize> iTelEmergNumber;

        // Call restriction info required by dial phase 2
        RMobileCall::TMobileCallIdRestriction iIdRestrict;

        // Flag for FDN error handling
        TBool iFDNErrorAlredyNotified;

        // Pointer to the dataport handler
        CMmDataPortHandler* iDataPortHandler;

        // Flag which notifies if it is a 3rd party client
        // (ETrue) or not (EFalse)
        TBool iIs3rdPartyDial;

        // Flag which notifies if it is a 3rd party client
        // (ETrue) or not (EFalse)
        TBool iIs3rdPartyAnswerIncomingCall;

        // Destination post address, max lenght is 123
        TBuf16<123> iDestPostAddress;

        // Variable to store and identify call ID for DTMF post address sending
        TUint8 iDtmfPostAddressCallId;

        // True if CALL_CREATE_REQ includes CALL_DESTINATION_POST_ADDRESS
        // subblock
        TBool iDestPostAddressIncluded;

        // ETrue when NoFdnCall request is active
        TBool iNoFdnDial;

        // To block FDN check in SIM originated calls. Doesn't block call
        // control procedure
        TBool iNoFdnCheck;

        // ETrue if HSPDA is forced off in product profile.
        // Modified in startup only.
        TBool iHSDPADisabledInPp;

        // ETrue while an emergency call setup is delayed,
        // i.e. while GPDS contexts are being closed.
        TBool iEmergCallDelayed;

		// Video call connected or disconnected
        TUint8 iVideoCallStatus;

		// bearer rate for multimedia call
        TUint8 iDataRateCode;

        // Call direction
        RMobileCall:: TMobileCallDirection iCallDirection;

        // Mobile call info
        RMobileCall::TMobileCallInfoV1 iMobileCallInfo;

        // Is call multimedia/video call
        TBool iIsMultimedia;

        // Is current call a waiting call
        TBool iIsWaitingCall;

        // Pipe handle for video/multimediacall
        TUint8 iPipeHandle;

        // CALL_ID of the call controlled request
        TUint8 iCallControlCallId;

        // call control result
        TUint8 iCcResult;

        // flag telling whether resource control needs to be disabled
        TUint8 iResourceControlSuppress;

    public:     // Friend classes
        // None
    protected:  // Friend classes
        // Nnone
    private:    // Friend classes

        // Emergency call delay timer
        friend class CEmergencyTimer;
        CEmergencyTimer iEmergencyTimer;

    };

#endif      // CMMCALLMESSHANDLER_H

// End of file
