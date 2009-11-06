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



#ifndef CMMPACKETSERVICEMESSHANDLER_H
#define CMMPACKETSERVICEMESSHANDLER_H

// INCLUDES
#include <net_modemisi.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/rmmcustomapi.h>

#include "cmmpacketmesshandlercontextlist.h"
#include "cmmpacketcontextmesshandler.h"
#include "cmmpacketqosmesshandler.h"
#include "cmmphonetreceiver.h"

// CONSTANTS
const TUint8 KGpdsPadding = 0x00; // Filler byte for ISI messages

// Buffer size 1 byte
const TInt KBuffSizeOneByte = 1;

// Buffer size 4 byte
const TInt KBuffSizeFourBytes = 4;

// Dummy transaction id DC
const TInt KDummyTrIdDC = 0xDC;

// Dummy transaction id DD
const TInt KDummyTrIdDD = 0xDD;

// Dummy transaction id DF
const TInt KDummyTrIdDF = 0xDF;

// Access point buffer 256
const TInt KAccessPointBuffer256 = 256;

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class TIsiReceiveC;
class TDataPackage;
class CMmPhoNetSender;
class CMmMessageRouter;

// CLASS DECLARATION

/**
* CMmPacketServiceMessHandler is used to create and send
* GSM/WCDMA-specific packet service ISI messages to PhoNet via PhoNetSender
* It also receives GSM/WCDMA -specific packet service ISI messages from
* Phonet via PhoNetReceiver.
*/
class CMmPacketServiceMessHandler :
    public CBase,
    public MMmMessHandlerBase,
    public MMmMessageReceiver
    {
    public: // Constructors and destructor

        /**
        * Default destructor.
        */
        ~CMmPacketServiceMessHandler();

        /**
        * Two-phased constructor.
		* @param aPhoNetSender: pointer to phonetsender
		* @param aPhoNetReceiver: pointer to phonetreceiver
		* @param aMessageRouter: pointer to messagerouter
        * @return CMmPacketServiceMessHandler*: pointer to the message handler
        */
        static CMmPacketServiceMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter );

    public: // Functions from base classes

		/**
        * Handles a received message by directs it to right messhandler.
        * @param aIsiMessage: reference to the received message.
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles errors coming from PhoNetReceiver RunError
        * @param aIsiMessage, reference to the received message.
        * @param TInt aError: Error code
        */
        void HandleError( const TIsiReceiveC& aIsiMessage, TInt aError );

        /**
        * ExtFunc is called by the MessageManager when it has request for the TSY.
		* Directs requests to right method
		* @param aIpc ipc number of request
		* @param aDataPackage datapackage contains request related data
		* @return error value
        */
        TInt ExtFuncL(
			TInt aIpc,
			const CMmDataPackage* aDataPackage );

    public: // New functions

		/**
		* Returns pointer to Messagerouter
		* @return CMmMessageRouter*
		*/
		CMmMessageRouter* GetMessageRouter();

		/**
		* Returns pointer to PacketContextList
		* @return CMmPacketContextMesshandlerList*
		*/
		CMmPacketContextMesshandlerList* PacketContextList();

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CMmPacketServiceMessHandler();

        /**
        * Second phase constructor of CMmPacketServiceMessHandler.
        */
        void ConstructL();

    private: // New functions

        /**
        * Configure request to the network.
		* @param aTransctionId: transaction Id
		* @param aAttachMode: attach mode
		* @param aMtContextActivationMode: mt context activation mode
		* @param aClassCMode: class c mode
		* @param alwaysOnMode: for always on mode
		* @return error value
        */
        TInt GpdsConfigureReq(
			const TUint8 aTransctionId,
			const TUint8 aAttachMode,
			const TUint8 aMtContextActivationMode,
			const TUint8 aClassCMode,
			RMmCustomAPI::TSetAlwaysOnMode alwaysOnMode	);

        /**
        * Status request to the network.
		* @return error value
        */
        TInt GpdsStatusReq() const;

        /**
        * Attach to the network.
		* @param None
		* @return error value
        */
        TInt GpdsAttachReq() const;

        /**
        * Detach to the network.
		* @param None
		* @return error value
        */
        TInt GpdsDetachReq() const;

        /**
        * Get Registration Status request.
		* @param None
		* @return error value
        */
        TInt NetModemRegStatusGetReq() const;

        /**
        * Reject network initiated context activation request.
		* @return error value
        */
        TInt GpdsContextNwiActRejectReq() const;

        /**
        * Configuration info request to the network.
		* @param TBool aCalledOnInitPhase true if called on initialisation phase
		* @return error value
        */
        TInt GpdsConfigurationInfoReq(
			TBool aCalledOnInitPhase ) const;

        /**
        * SetContextDefaultParameters request to the network.
		* @param aDataCompression data Compression
		* @param aHeaderCompression header Compression
		* @param aAccessPointName access point name
		* @param aPdpType pdp type
		* @return error value
        */
        TInt GpdsContextConfigureReq(
            TUint8 aDataCompression,
            TUint8 aHeaderCompression,
            TBuf8<256> aAccessPointName,
            TUint8 aPdpType );

		/**
        * Sets Attach Mode to the GPDS server.
		* @param aMode attach mode
		* @return error value
        */
		TInt SetAttachMode(
			RPacketService::TAttachMode* const aMode );

		/**
		* Sets preferrer bearer to the GPDS server
		* @param aPackage contains request related data
		* @return error value
		*/
		TInt SetPreferredBearer(
            const CMmDataPackage& aPackage );

		/**
		* Set default context parameters
		* @param aPackage contains request related data
		* @return error value
		*/
        TInt SetDefaultContextParams(
		    const CMmDataPackage& aPackage );

        /**
        * Current Network Radio Access Technology Request.
        * @return error value
        */
        TInt NetRatReq();

        /**
        * Gpds configure response.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsConfigureResp(
            const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Gpds status response.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsStatusResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Gpds attach response.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsAttachResp(
            const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Gpds detach response.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsDetachResp(
            const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Network registration status get response or indication.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void NetModemRegStatusGetRespOrInd(
            const TIsiReceiveC &aIsiMessage );

        /**
        * Network initiated context activation reject response.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsContextNwiActRejectResp(
            const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Gpds configuration info response.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsConfigurationInfoResp(
            const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Gpds context id create indication.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsContextIdCreateInd(
            const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Gpds configuration info indication.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsConfigurationInfoInd(
            const TIsiReceiveC& aIsiMessage ) const;

        /**
        * Gpds transfer status indication.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsTransferStatusInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Gpds attach indication.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsAttachInd() const;

        /**
        * Gpds detach indication.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsDetachInd() const;

        /**
        * Gpds network requested context activation indication.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsContextNwiActRequestInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Response for SetDefaultContextParams request.
		* @param TIsiReceiveC& aIsiMessage: Received ISI message
        */
        void GpdsContextConfigureResp(
            const TIsiReceiveC& aIsiMessage ) const;

		/**
        * Map network registration status to packet service status.
		* @param aRegistrationStatus network registration status
		* @param aSuccessCode succescode default set KNetCauseOk
		* @return TRegistrationStatus
        */
        RPacketService::TRegistrationStatus MapNtwkRegStatus(
            TUint8 aRegistrationStatus,
            const TUint8 aSuccessCode = NET_CAUSE_OK ) const;

		/**
        * Completes gsm/wcdma registration info.
		* @param aCsServices defines if cs services are supported
		* @param aGprsSupportInCell is gprs supported in current cell
		* @param aEdgeGprsSupport is edge gprs supported
        */
		void CompleteGsmRegistrationInfo(
			const TBool aCsServices,
			const TBool aGprsSupportInCell,
			const TBool aEdgeGprsSupport );

	    /**
	    * Completes transfer caps change.
	    * @param aEdgeGprsSupport is edge gprs supported
	    */
		void CompletePacketNotifyTransferCaps(
		    const TBool aEdgeGprsSupport );

		/**
        * Completes dynamic capabilities change to the service tsy.
        */
        void CompleteDynamicCapsChange();

		/**
		* Completes Transfer Status Indication
		* @param aTransferCause transfer cause
		*/
		void CompleteTransferStatusIndication(
			const TUint8 aTransferCause );

		/**
		* Completes MS Class change notification to the Service Tsy
		* @param aGprsNetworkMode gprs network mode
		*/
		void CompleteNotifyMSClassChange(
			const TUint8 aGprsNetworkMode );

		/**
		* Completes Gpds Status Request.
		* @param aAttachStatus attach status
		* @param aTransferStatus transfer status
		*/
		void CompleteGpdsStatusRequest(
			const TUint8 aAttachStatus,
			const TUint8 aTransferStatus );

        /**
        * Network Rat Indication or Response.
        */
        void NetRatIndOrResp(
            const TIsiReceiveC &aIsiMessage  );

        /**
        * Network radio information indication.
        * @param const TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void NetRadioInfoInd( const TIsiReceiveC& aIsiMessage );

    public: // Data
        // None

    protected: // Data
        // None

    private: // Data

        // Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        // pointer to the message router
        CMmMessageRouter* iMessageRouter;

        // Last info of circuit switch services availability from network server
        TBool iLastCsServices;

		// Pointer to CMmPacketContextMesshandlerList, created and owned
        CMmPacketContextMesshandlerList* iPacketMesshandlerContextList;

        // Last info of gprs support in current cell from network server
        TBool iLastGprsSupportInCell;

        // Last info of dual transfer mode in current cell from network server
        TBool iLastDtmSupportInCell;

		// Is edge gprs supported
        TBool iLastEdgeGprsSupport;

        // Is HSDPA allocated
        TBool iLastHsdpaAllocationStatus;

		// Dynamic capabilities of gsm/wcdma extension
        RPacketService::TDynamicCapsFlags iDynamicCaps;

        // Current network mode
        RMobilePhone::TMobilePhoneNetworkMode iCurrentMode;

        // set always on mode
        RMmCustomAPI::TSetAlwaysOnMode iAlwaysOnMode;

    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };

#endif // CMMPACKETSERVICEMESSHANDLER_H

// End of File
