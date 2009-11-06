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



#ifndef CMMPACKETQOSMESSHANDLER_H
#define CMMPACKETQOSMESSHANDLER_H

// INCLUDES
#include <etelqos.h>
#include <in_sock.h>
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"
#include <ctsy/serviceapi/cmmutility.h>

// CONSTANTS

// Shift 1 bit
const TInt KShift1Bit = 1;

// Shift 8 bits
const TInt KShift8Bits = 8;

// Shift 16 bits
const TInt KShift16Bits = 16;

// Shift 24 bits
const TInt KShift24Bits = 24;

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmPacketContextMesshandlerList;
class TIsiReceiveC;
class CMmMessageRouter;

// CLASS DECLARATION

// DESCRIPTION

/**
* CMmPacketQoSMessHandler is used to create and send GSM/WCDMA-
* specific packet context ISI messages to PhoNet via PhoNetSender.
* It also receives GSM/WCDMA-specific packet context ISI messages from
* Phonet via PhoNetReceiver.
*/
class CMmPacketQoSMessHandler: public CBase,
    public MMmMessHandlerBase,
    public MMmMessageReceiver
    {
    public: // Constructor and destructor

        /**
        * Two-phased constructor.NewL method is used to create a new instance
        * of CMmPacketQoSMessHandler class.
        * @param aPhoNetSender: pointer to phonetsender
        * @param aPhoNetReceiver: pointer to phonet receiver
		* @param aMmPacketContextMesshandlerList: pointer to context list
		* @param aMessageRouter: pointer to message router
        * @return CMmPacketQoSMessHandler*: pointer to the message handler
        */
        static CMmPacketQoSMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
			CMmPacketContextMesshandlerList* aMmPacketContextMesshandlerList,
			CMmMessageRouter* aMessageRouter );

        /**
        * Destructor.
        */
        ~CMmPacketQoSMessHandler();

    public: // new classes
 	/**
    * Server side QoS R97/98 parameters
	* @param iPrecedence
	* @param iDelay
	* @param iPeakThroughput
	* @param iReliability
	* @param iMeanThroughput
    */
    class TQoSInfo
        {
        public:
            TUint8 iPrecedence;
            TUint8 iDelay;
            TUint8 iPeakThroughput;
            TUint8 iReliability;
            TUint8 iMeanThroughput;
        };

    /**
    * Server side QoS R99 parameters
	* @param iTrafficClass
	* @param iDeliveryOrderReqd
	* @param iDeliverErroneousSDU
	* @param iBER
	* @param iSDUErrorRatio
	* @param iTrafficHandlingPriority
	* @param iTransferDelay
	* @param iMaxSDUSize
	* @param iMaxUplinkRate
	* @param iMaxDownlinkRate
	* @param iGuaranteedUplinkRate
	* @param iGuaranteedDownlinkRate
    */
    class TQoS99Info
        {
        public:
            TUint8 iTrafficClass;
            TUint8 iDeliveryOrderReqd;
            TUint8 iDeliverErroneousSDU;
            TUint8 iBER;
            TUint8 iSDUErrorRatio;
            TUint8 iTrafficHandlingPriority;
            TUint16 iTransferDelay;
            TUint16 iMaxSDUSize;
            TUint16 iMaxUplinkRate;
            TUint16 iMaxDownlinkRate;
            TUint16 iGuaranteedUplinkRate;
            TUint16 iGuaranteedDownlinkRate;
            TUint8	iSignallingIndication;
            TUint8	iSourceStatisticsDescriptor;
        };

    /**
    * Structure for server side QoS parameters
	* @param iExtensionId
	* @param TQoSInfo* iQoSMinInfo
	* @param TQoSInfo* iQoSReqInfo
	* @param TQoS99Info* iQoS99MinInfo
	* @param TQoS99Info* iQoS99ReqInfo
    */
    class TQoS
        {
        public:
            TInt iExtensionId;
            TQoSInfo* iQoSMinInfo;
            TQoSInfo* iQoSReqInfo;
            TQoS99Info* iQoS99MinInfo;
            TQoS99Info* iQoS99ReqInfo;
        };

    public: // New functions

        /**
        * IsiMsg received by PhonetReceiver, interpret if GSM/WCDMA packet
        * context specific
        * @param TIsiReceiveC, reference to the received message.
        * @return void
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

		/**
		* Maps parameters to server
		* @param aQoSInfo refence to TQoSInfo
		* @param aPrecedence refence to RPacketQoS::TQoSPrecedence
		* @param aDelay refence to RPacketQoS::TQoSDelay
		* @param aReliability refence to RPacketQoS::TQoSReliability
		* @param aPeakThroughput refence to RPacketQoS::TQoSPeakThroughput
		* @param aMeanThroughput refence to RPacketQoS::TQoSMeanThroughput
		* @return error value
		*/
		TInt MapQoSInfoToServer(
			TQoSInfo& aQoSInfo,
			const RPacketQoS::TQoSPrecedence& aPrecedence,
			const RPacketQoS::TQoSDelay& aDelay,
			const RPacketQoS::TQoSReliability& aReliability,
			const RPacketQoS::TQoSPeakThroughput& aPeakThroughput,
			const RPacketQoS::TQoSMeanThroughput& aMeanThroughput );

        /**
        * Map QoS99Info To Server
		* @param aQos99Info Server side QoS R99 info packet
		* @param aTrafficClass Traffic class
		* @param aDeliveryOrderReqd Delivery order required
		* @param aDeliverErroneousSDU Delivery of erroneous SDUs
		* @param aBER Bit error ratio
		* @param aSDUErrorRatio SDU error ratio
		* @param aTrafficHandlingPriority Traffic handling priority
		* @return error value
        */
		TInt MapQoS99InfoToServer(
			TQoS99Info& aQoS99Info,
			const RPacketQoS::TTrafficClass& aTrafficClass,
			const RPacketQoS::TDeliveryOrder& aDeliveryOrderReqd,
			const RPacketQoS::TErroneousSDUDelivery& aDeliverErroneousSDU,
			const RPacketQoS::TBitErrorRatio& aBER,
			const RPacketQoS::TSDUErrorRatio& aSDUErrorRatio,
			const RPacketQoS::TTrafficHandlingPriority& aTrafficHandlingPriority );

		/**
        * Copy QoS99 Values To Server
		* @param aQos99Info QoS R99 info packet
		* @param aMaxTransferDelay Maximum transfer delay
		* @param aMaxSDUSize Maximum SDU size
		* @param aMaxRate Maximum data rate
		* @param aGuaranteedRate Guaranteed rate for uplink and downlink
		* @return error value
		*/
		TInt CopyQoS99ValuesToServer(
			TQoS99Info& aQoS99Info,
			const TInt aMaxTransferDelay,
			const TInt aMaxSDUSize,
			const RPacketQoS::TBitRate& aMaxRate,
			const RPacketQoS::TBitRate& aGuaranteedRate );

    public: // Functions from base classes

		/**
		* ExtFunc is called by the MessageManager when it has request for the TSY.
		* Directs requests to right method
		* @param aIpc IPC number of request
		* @param aDataPackage Datapackage includes request related data
		* @return error value
		*/
		TInt ExtFuncL(
            TInt aIpc,
            const CMmDataPackage* aDataPackage );

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // Constructor and destructor

        /**
        * C++ default constructor
        */
        CMmPacketQoSMessHandler();

        /**
        * Second phase constructor of CMmPacketContext.
        */
        void ConstructL();

    private: // New functions
 
        /**
        * Creates GpdsContextConfigureReq ISI message and sends it to Phonet.
		* @param aContextId ,Context identifier
		* @param aPdpType ,Pdp type
		* @param aContextType ,Context type
		* @param aPrimaryContextId ,Primary context identifier
		* @param TQoS& aQoS ,Reference to TQoS
		* @param aSignallingIndication ,Signalling indication
		* @param aSourceStatisticsDescriptor ,Source Statistics Descriptor
		* @return error value
        */
        TInt GpdsContextConfigureReq(
            const TUint8 aContextId,
            const TUint8 aPdpType,
            const TUint8 aContextType,
            const TUint8 aPrimaryContextId,
            const TQoS& aQoS,
			const TBool aSignallingIndication,
			const TBool aSourceStatisticsDescriptor ) const;

        /**
        * Breaks a GPDS_CONTEXT_CONFIGURE_RESP ISI-message
		* @param aIsiMsg pointer to isimessage reived from NOS
        */
        void GpdsContextConfigureResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Breaks a GPDS_CONTEXT_ACTIVATE_IND or
        * GPDS_CONTEXT_MODIFY_IND ISI-message
		* @param aIsiMsg pointer to isimessage received from NOS
        */
        void GpdsContextActivateOrModifyInd(
            const TIsiReceiveC& aIsiMessage );

		/**
		* Sets QoS profile parameters
		* @param aDataPackage contains request related data
		* @return error value
		*/
		TInt SetProfileParametersL(
			const CMmDataPackage* aDataPackage );

		/**
        * Reset Requested QoS parameters
		* @param aQos referance to struct TQos
        */
		void ResetRequestedQoS(
			TQoS& aQoS );

		/**
        * Completes Notify Profile Changed
		* @param aQoSNegotiated Pointer to TQoSInfo structure
		* @param aQoS99Negotiated Pointer to TQoS99Info structure
		* @param aQoSNegValid Negotiated QoS 97 parameter indicator
		* @param aQoS99NegValid Negotiated QoS 99 parameter indicator
		* @param aContextId Context object ID number
        */
		void CompleteNotifyProfileChanged(
			TQoSInfo*  aQoSNegotiated,
			TQoS99Info*  aQoS99Negotiated,
			const TBool aQoSNegValid,
			const TBool aQoS99NegValid,
			TUint8 aContextId );

        /**
        * Maps QoSInfo To Client
		* @param aQoSInfo Reference to QoS Info
		* @param aPrecedence Reference to Precedence
		* @param aDelay Reference to Delay
		* @param aReliability Reference to Reliability
		* @param aPeakThroughput Reference to PeakThroughput
		* @param aMeanThroughputReference to MeanThroughput
        */
		void MapQoSInfoToClient(
			const TQoSInfo& aQoSInfo,
			RPacketQoS::TQoSPrecedence& aPrecedence,
			RPacketQoS::TQoSDelay& aDelay,
			RPacketQoS::TQoSReliability& aReliability,
			RPacketQoS::TQoSPeakThroughput& aPeakThroughput,
			RPacketQoS::TQoSMeanThroughput& aMeanThroughput );

		/**
        * Maps QoSInfo To Client
		* @param aQoS99Info Reference to QoS Info
		* @param aQoS99Negotiated Reference to negotiated QoS 99 parameters
		* @param aQoSNegValid Reference to negotiated QoS R5 parameters
		*/
		void MapQoS99InfoToClient(
			const TQoS99Info& aQoS99Info,
			RPacketQoS::TQoSR99_R4Negotiated& aQoS99Negotiated,
			RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated );

    protected: // Data
        // None

    private: // Data

        // Pointer to the phonet sender
        CMmPhoNetSender* iPhoNetSender;

		// Pointer to the message router
		CMmMessageRouter* iMessageRouter;

        // Pointer to the messhandlerlist
	    CMmPacketContextMesshandlerList*
	        iMmPacketContextMesshandlerList;

		// Pointer to the TQoSGPRSNegotiated
        RPacketQoS::TQoSGPRSNegotiated  iQoSGPRSNegotiated;

		// Pointer to the negotiated QoS 99 parameters
        RPacketQoS::TQoSR99_R4Negotiated iQoSR99_R4Negotiated;

        RPacketQoS::TQoSR5Negotiated iQoSR5_Negotiated;

		// Structure containing server side QoS parameters
		TQoS iQos;

    };

#endif // CMMPACKETQOSMESSHANDLER_H

// End of file
