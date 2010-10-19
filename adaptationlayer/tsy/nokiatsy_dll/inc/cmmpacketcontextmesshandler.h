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

#ifndef CMMPACKETCONTEXTMESSHANDLER_H
#define CMMPACKETCONTEXTMESSHANDLER_H

// INCLUDES
#include <etelpckt.h>
#include <etelqos.h>
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h" // for MMmMessageReceiver
#include "cmmpipecontrol.h"
#include "nokiatsy_internal_variation.h"

// CONSTANTS
// Client connection speed rates between 0 and 2097152 bps
const TInt KClientSpeeed0 = 0;
const TInt KClientSpeeed8192 = 8192;
const TInt KClientSpeeed16384 = 16384;
const TInt KClientSpeeed32768 = 32768;
const TInt KClientSpeeed65536 = 65536;
const TInt KClientSpeeed131072 = 131072;
const TInt KClientSpeeed262144 = 262144;
const TInt KClientSpeeed524288 = 524288;
const TInt KClientSpeeed1048576 = 1048576;
const TInt KClientSpeeed2097152 = 2097152;

// The maximum lengths for subblocks
// See: ISI General Packet Data Server Message Descriptions
const TUint16 KMaximumLengthOfGpdsTftInfo = 256;
const TUint8 KMaximumLengthOfGpdsTftAuthTokenInfo = 248;
const TUint8 KMaximumLengthOfGpdsTftFilterInfo = 148;

// Access point buffer 256
const TInt KAccessPointBuffer252 = 252;

// Nuber of bytes to be removed from total length for auth token info
const TInt KAuthTokenInfoChecking = 4;

// Index value 2
const TInt KIndexValue2 = 2;

// GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO value 2
const TInt KGdpsSharedTftParamIPFlowInfo2 = 2;

// GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO value 3
const TInt KGdpsSharedTftParamIPFlowInfo3 = 3;

// Maximum length of GDPS_ACTIVATE_REQ sub block
const TInt KMaxLengthOfGdpsActivateReqSB = 20;

// Remove extraordinary bytes from RPacketContext::KMiscProtocolBufferLength
// to get size for DNS buffer
const TInt KRemoveOtherThanDNSBytes = 3;

// DNS Buffer length
const TInt KDNSBufferLength = 16;

// CSCF Buffer length
const TInt KCSCFBufferLength = 16;

// CSCF Buffer length
const TInt KSignallingFlagBuffer = 1;

// Data Length for TLV
const TInt KTLVDataLen = 2;

// PCO Id len
const TInt KPCOIdLen = 1;

// Message data buffer size 1 byte
const TInt KMessageDataBufSize1 = 1;

// Filter array minus value
const TInt KFilterArray = 1;

// Initializing mask to 0x80 = 1000 0000 b
const TInt KInitMask0x80 = 0x80;

// Sub block count 1
const TInt KSubBlockCount1 = 1;

// Buff size for proxy id for channel
const TInt KBuffProxyIdForChannel = 2;

// Byte count for ptr to TLV data
const TInt KPtrToTLVdata = 4;

// Flow id's array index
const TInt KFlowIdsArrayIndex1 = 1;

// Call mode init value
const TInt KCallModeInitValue = 0xFF;

// Filler byte for ISI messages
const TUint8 KCallPadding = 0x00;

// Subbclock length
const TInt KSubBlockLengthOffset1 = 1;

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
class CMmPacketQoSMessHandler;
class CMmMessageRouter;
class RPacketContext;
class RPacketContext::CTFTMediaAuthorizationV3;
struct RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier;

// CLASS DECLARATION

/**
*  CMmPacketContextMessHandler is used to create and send
*  GSM-specific Packet Context ISI messages to PhoNet via PhoNetSender.
*  It also receives GSM-specific Packet Context ISI messages from Phonet
*  via PhoNetReceiver.
*/
class CMmPacketContextMessHandler : public CBase,
    public MMmMessHandlerBase,
    public MMmMessageReceiver
    {

    public: // Constructors and destructor

        /**
        * Creates a new packet context message handler object
        * @param CMmPhoNetSender*: pointer to phonet sender
        * @param CMmPhoNetReceiver*: pointer to phonet receiver
        * @param CMmPacketContextMesshandlerList*: pointer to context list
        * @param CMmPacketQoSMessHandler*: pointer to QoS message handler
        * @param CMmMessageRouter*: pointer to message router
        * @return CMmPacketContextMessHandler*: pointer to the message handler
        */
        static CMmPacketContextMessHandler* NewL(
            CMmPhoNetSender* const aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmPacketContextMesshandlerList* aMmPacketContextMesshandlerList,
            CMmPacketQoSMessHandler* aMmPacketQoSMessHandler,
            CMmMessageRouter* aMessageRouter );

        /**
        * Destructor.
        */
        ~CMmPacketContextMessHandler();

    public: // New functions

        /**
        * Isimsg received by PhonetReceiver, interpret if GSM Packet Context
        * specific.
        * @param TIsiReceiveC, reference to the received message.
        * @return void
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

        /**
        * This function handles pipe related response messages and
        * completes operations.
        * @param aMessageId: message id
        * @param aTransactionId: transaction id
        * @param aPipeHandle: pipe handle
        * @param aErrorCause: error cause
        */
        void CompletePipeOperation(
            const TInt aMessageId,
            const TUint8 aTransactionId,
            const TUint8 aPipeHandle,
            const TUint8 aErrorCause );

    public: // Functions from base classes

        /**
        * From MMmMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler.
        * ExtFuncL is called by the MessageManager.
        * @param aIpc: IPC number of the request
        * @param aDataPackage: datapackage from CommonTSY
        * @return TInt: KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CMmPacketContextMessHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Create an Isi-message for creating a context id.
        * @param aCreateContextIdTransId: create context id transaction id
        * @return TInt: error code (return value from PhoNetSender)
        */
        TInt GpdsContextIdCreateReq(
            const TUint8 aCreateContextIdTransId ) const;

        /**
        * Configure a local link between Context and Pipe.
        * @param aTransactionId: transaction id
        * @param aContextId: context id
        * @param aPipeHandle: pipe handle
        * @return TInt: Error code (return value from PhoNetSender)
        */
        TInt GpdsLlConfigureReq(
            const TUint8 aTransactionId,
            const TUint8 aContextId,
            const TUint8 aPipeHandle ) const;

        /**
        * Create an Isi-message for context configuration.
        * @param aTransactionId: ransaction id
        * @param aContextId: context id
        * @param aPdpType: Gpds server's Pdp address type
        * @param aContextType: Context type
        * @param aPrimaryContextId: primary context id
        * @param aDataCompression: data compression flag
        * @param aHeaderCompression: header compression flag
        * @param aAccessPointName: Access point name
        * @param aPacketFilterPtr: packet filter
        * @param aMediaAuthorization: media authorization
        * @param aSignallingIndication: signalling indication
        * @param aPCFCSAddressIndication: PCFCS address request indication
        * @param aDNSAddressRequestIndication: DNS address request indication
        * @return TInt: Error code (return value from PhoNetSender)
        */
        TInt GpdsContextConfigureReq(
            const TUint8 aTransactionId,
            const TUint8 aContextId,
            const TUint8 aPdpType,
            const TUint8 aContextType,
            const TUint8 aPrimaryContextId,
            const TUint8 aDataCompression,
            const TUint8 aHeaderCompression,
            const TBuf8<KAccessPointBuffer252>& aAccessPointName,
            CArrayFixFlat<RPacketContext::TPacketFilterV2>* aPacketFilterPtr,
            RPacketContext::CTFTMediaAuthorizationV3* aMediaAuthorization,
            TBool aSignallingIndication = EFalse,
            TBool aPCFCSAddressIndication = EFalse,
            TBool aDNSAddressRequestIndication = EFalse );

        /**
        * Create an Isi-message for context activation.
        * @param aTransactionId: transaction id
        * @param aContextId: context id
        * @param aPdpAddress: PDP address
        * @param aPdpType: type of PDP address
        * @return TInt: Error code (return value from PhoNetSender)
        */
        TInt GpdsContextActivateReq(
            const TUint8 aTransactionId,
            const TUint8 aContextId,
            const TDesC8& aPdpAddress,
            const TUint8 aPdpType ) const;

        /**
        * Deactivate a Context.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: Error code (return value from PhoNetSender)
        */
        TInt GpdsContextDeactivateReq(
            const CMmDataPackage& aDataPackage );

        /**
        * Request the Context Status info.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: Error code (return value from PhoNetSender)
        */
        TInt GpdsContextStatusReq(
            const CMmDataPackage& aDataPackage );

        /**
        * Modify a Context.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: Error code (return value from PhoNetSender)
        */
        TInt GpdsContextModifyReq(
            const CMmDataPackage* aPackage );

        /**
        * Add packet filter info to GPDS Context Configure or Modify request.
        * @param aPdpType: PDP address type
        * @param aFilter: packet TFT filter
        * @param aBuffer: reference to message buffer
        * @return TUint16: length of PacketFilter
        */
        TUint16 AddPacketFilterInfo(
            const TUint8 aPdpType,
            RPacketContext::TPacketFilterV2* aFilter,
            TDes8& aBuffer ) const;

        /**
        * This request is for compound of configuration and authentication.
        * @param aTransactionId: transaction id
        * @param aContextId: context id
        * @param aAuthInfo: authentication info
        * @param aResponse: response information
        * @param aChallenge: challenge information
        * @param aId: TProtocolConfig Id
        * @return TInt: Error code (return value from PhoNetSender)
        */
        TInt SetAuthenticationConfigReq(
            const TUint8 aTransactionId,
            const TUint8 aContextId,
            RPacketContext::TAuthInfo aAuthInfo,
            RPacketContext::TAuthData aResponse,
            RPacketContext::TAuthData aChallenge,
            TUint8  aId );

        /**
        * Response for Context Id creation.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextIdCreateResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Gpds context id create indication.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextIdCreateInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Response for Context Local Link configuration.
        * @param aIsiMsg: received Isi message
        */
        void GpdsLlConfigureResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Response for Context Configuration.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextConfigureResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Response for setting the Context Authentication info.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextAuthResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Indicate that Context Activation has been started.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextActivatingInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Indicate that Context Activation has succeeded.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextActivateIndL(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Indication that Context Activation has failed.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextActivateFailInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Indicate the Context Modification.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextModifyInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Response for Context Deactivation.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextDeactivateRespL(
            const TIsiReceiveC& aIsiMessageg );

        /**
        * Indicate that Context Deactivation has been started.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextDeactivatingInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Indicate that Context Deactivation has succeeded.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextDeactivateInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Response for Context Status inquiry.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextStatusResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Indicate the final data transferred over the airlink.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextStatusInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Indicate that Context Id has been deleted.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextIdDeleteInd(
            const TIsiReceiveC& aIsiMessage );

        /**
        * Response for GPDS modify request.
        * @param aIsiMsg: received Isi message
        */
        void GpdsContextModifyResp(
            const TIsiReceiveC& aIsiMessage );

        /**
        * This function starts the three phase context initialisation.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: KErrNone or error code
        */
        TInt InitialiseContext(
            const CMmDataPackage& aDataPackage );

        /**
        * This function starts the second phase of the context initialisation.
        * @param aPipeHandle: pipe handle
        * @return TInt: KErrNone or error code
        */
        TInt InitialiseContextPhase2(
            TUint8 aPipeHandle );

        /**
        * This function starts the third phase of the context initialisation.
        * @param aStatus: GPDS Status
        * @param aContextId: Context Id
        * @param aTrId: transaction id used in context initialisation sequence
        */
        void InitialiseContextPhase3(
            TUint8 aStatus,
            TUint8 aContextId,
            TUint8 aTrId );

        /**
        * Set configuration data for context.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: KErrNone or error code
        */
        TInt SetConfigL(
            const CMmDataPackage& aDataPackage );

        /**
        * Add TFT packet filter for context.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: KErrNone or error code
        */
        TInt AddPacketFilter(
            const CMmDataPackage* aDataPackage );

        /**
        * Delete context related to the network e.g. pipe deletion.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: KErrNone or error code
        */
        TInt Delete(
            const CMmDataPackage& aDataPackage );

        /**
        * This function starts the second phase of the context activation.
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: KErrNone or error code
        */
        TInt ActivatePhase2(
            const CMmDataPackage& aDataPackage );

        /**
        * Map the Packet API pdp type to the Gpds server pdp type.
        * @param aPdpTypeServer: server pdp type
        * @param aPdpTypeClient: client pdp type
        * @return TInt: KErrNone or error code
        */
        TInt MapPdpType(
            TUint8& aPdpTypeServer,
            const RPacketContext::TProtocolType aPdpTypeClient );

        /**
        * Map the Gpds server speed to the Packet API connection speed.
        * @param aConnectionSpeedServer: server connection speed rate
        * @param aConnectionSpeedClient: client connection speed rate
        */
         void MapConnectionSpeed(
            const TUint8 aConnectionSpeedServer,
            TInt& aConnectionSpeedClient );

        /**
        * This function generates transaction id for use of current context.
        * @param contextId
        * @return TUint8: transaction id
        */
         TUint8 GenerateTraId(
            const TUint8 aContextId );

        /**
        * Map the Client TFT operation to the Gpds server TFT operation.
        * @param aClientTypeOperation: client operation
        * @param aTftOperation: server operation
        */
        void MapTftOperation(
            const TInt aClientTypeOperation,
            TUint8& aTftOperation );

        /*
        * Makes request for media authorization params for specific context
        * @param aDataPackage: data package from CommonTSY
        * @return TInt: KErrNone or error code
        */
        TInt AddMediaAuthorizationL(
            const CMmDataPackage* aDataPackage );

        /*
        * Creates subblocks to GpdsContextConfigureOrModifyReq.
        * @param aMediaAuthorizationArray: media authorization array
        * @param aBuffer: reference to message buffer
        * @param aNumOfSbs: number of subblocks
        * @return TInt: KErrNone or error code
        */
        TInt AddMediaAuthorizationInfo(
            CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>*
                aMediaAuthorizationArray,
            TDes8& aBuffer,
            TUint8& aNumOfSbs ) const;

        /*
        * Appends one media authorization unit.
        * @param aMediaAuthorization: media authorization data
        * @param aBuffer: reference to message buffer
        * @param aNumOfSbs: number of subblocks appended
        * @return TInt: KErrNone or KErrTooBig if buffer is not big enough to
        * fit all the information
        * @note method does not reset aNumOfSbs on entry
        */
        TInt AppendMediaAuthorizationUnit(
                const RPacketContext::CTFTMediaAuthorizationV3&
                    aMediaAuthorization,
                TDes8& aBuffer,
                TUint8& aNumOfSbs ) const;

        /**
        * This methods checks if secondary contexts needs to be deactivated
        * and allows incoming call.
        * (for allow incoming call feature).
        */
        void AllowIncomingCallActivationL();

        /**
        * Appends GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO to the buffer
        * @param aAuthorizationToken: authorization token to be appended
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks appended
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if passed buffer is
        * not big enough to fit the subblock
        */
        TInt AppendGpdsSharedTFTParameterAuthTokenInfo(
             const TDesC8& aAuthorizationToken,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO to the buffer
        * @param aFlowIdentifier: data to be appended
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks appended
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if passed buffer is
        * not big enough to fit the subblock
        */
        TInt AppendGpdsSharedTFTParameterIPFlowInfo(
             const RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier&
             aFlowIdentifier,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Fills the buffer as GPDS_TFT_INFO complex subblock.
        * Caller passes in buffer big enough to hold GPDS_TFT_INFO subblock
        * @param aOperation: desired operation (constants starting with GPDS_TFT_)
        * @param aMediaAuthorization: media authorization
        * @note aMediaAuthorization can be NULL
        * @param aPacketFilter: packet filters array
        * @note aPacketFilter can be NULL
        * @note both aMediaAuthorization and aPacketFilter can not be NULL at
        * the same time
        * @param aPdpType: type
        * @param aBuffer: buffer to be filled
        * @return TInt: KErrNone or KErrTooBig if any of the subblocks doesnt
        * fit into the buffer
        */
        TInt FillGpdsTFTInfo( TUint8 aOperation,
             RPacketContext::CTFTMediaAuthorizationV3* aMediaAuthorization,
             CArrayFixFlat<RPacketContext::TPacketFilterV2>* aPacketFilterPtr,
             TUint8 aPdpType,
             TDes8& aBuffer) const;

        /**
        * Fills the buffer as GPDS_QOS99_REQ_INFO subblock.
        * Caller passes in buffer big enough to hold GPDS_QOS99_REQ_INFO subblock
        * @param aQos99Params: Qos99 parameters
        * @param aBuffer: buffer to be filled
        * @return KErrNone or system-wide error
        */
        TInt FillGpdsQos99ReqInfo(
             const RPacketQoS::TQoSR99_R4Requested& aQos99Params,
             TDes8& aBuffer );

        /**
        * Fills the buffer as GPDS_QOS_REQ_INFO subblock.
        * Caller passes in buffer big enough to hold GPDS_QOS99_REQ_INFO subblock
        * @param aQos97Params: Qos97 parameters
        * @param aBuffer: buffer to be filled
        * @return KErrNone or system-wide error
        */
        TInt FillGpdsQosReqInfo(
             const RPacketQoS::TQoSGPRSRequested& aQos97Params,
             TDes8& aBuffer );

        /**
        * Appends GPDS_FILT_FLOW_LABEL_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsFilterFlowLabelInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_FILT_TOS_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsFilterTosInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_FILT_SPI_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsFilterSpiInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_FILT_SRC_PORT_RANGE_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsFilterSrcPortRangeInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_FILT_DST_PORT_RANGE_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsFilterDstPortRangeInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_FILT_PROTOCOL_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsFilterProtocolInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_FILT_SRC_IPV4_ADDR_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsFilterSrcIPv4AddressInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS_FILT_SRC_IPV6_ADDR_INFO subblock to the passed buffer.
        * @param aFilter: filter instance
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: number of subblocks added
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit
        * the subblock
        */
        TInt AppendGpdsFilterSrcIPv6AddressInfo(
             const RPacketContext::TPacketFilterV2& aFilter,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Appends GPDS IP source address information to the buffer.
        * Buffer should be prepared by the caller before sending it here.
        * Only amount of masked bits and IP address themselves are appended
        * @param aInputIpAddress: input IP address
        * @param aInputIpAddressMask: input IP address mask
        * @param aBuffer: destination buffer
        * @return TInt: KErrNone or KErrArgument in case invalid IP (black hole)
        * is specified
        */
        TInt AppendIpAddressInfo(
             const TDesC8& aInputIpAddress,
             const TDesC8& aInputIpAddressMask,
             TDes8& aBuffer ) const;

        /**
        * Appends GPDS_TFT_FILTER_INFO subblock.
        * @param aFilterId: filter id
        * @param aBuffer: destination buffer
        * @param aNumOfSbs: destination buffer
        * @note aNumOfSbs is not reset on entry
        * @return TInt: KErrNone or KErrTooBig if destination cant fit the subblock
        */
        TInt AppendGpdsTftFilterInfo(
             TUint8 aFilterId,
             TDes8& aBuffer,
             TUint8& aNumOfSbs ) const;

        /**
        * Breaks received CallModemResourceInd ISI message.
        * @param aIsiMessage: received Isi message.
        */
        void CallModemResourceIndL( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates CallModemResourceReq ISI message to modem call server.
        * @param isiMessage
        * @return None
        */
        void CallModemResourceReqAllowed();

        /**
        * Breaks received CallModemResourceResp ISI message.
        * @param aIsiMessage: received Isi message.
        */
        void CallModemResourceResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates SendCallmodemResourceDenied ISI message to modem call server.
        * @param aDatapackage
        * @return None
        */
        void CallModemResourceReqDenied(
            const TUint8 aCauseType,
            const TUint8 aCause,
            const TDesC8& aDetailedCause );

        /*
        * This method completes drive mode functions, if drive mode flag
        * is enabled
        * @param aDataPackage: data package
        * @return TInt: KErrNone
        */
        void CompleteDCMdrivemodeFunctionalityL();

        /*
        * This method interpretes channel ID from context name.
        * @param TInfoName: Context name
        * @return TInt: Channel ID
        */
        TInt getProxyId(const TInfoName& contextName);

        /**
        * PnsNameAdd indication.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsNameAddInd( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsNameRemove indication.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsNameRemoveInd( const TIsiReceiveC& aIsiMessage );

    public: // Data
        // None

    protected: // Data
        // None

    private: // Data

        // Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        // A pointer to the PhonetReceiver.
        CMmPhoNetReceiver* iPhoNetReceiver;

        // Pointer to list of opened contexts
        CMmPacketContextMesshandlerList* iContextList;

        // Pointer to the CMmPacketQoSMessHandler
        CMmPacketQoSMessHandler* iMmPacketQoSMessHandler;

        // Pointer to the message router
        CMmMessageRouter* iMessageRouter;

        // Pointer to the pipe controller, created and owned.
        CMmPipeControl* iMmPipeController;

        // Primary context id
        TUint8 iPrimaryContextId;

        // Context id
        TUint8 iDialUpContextId;

        // Dial-up context name
        TInfoName iDialUpContextName;

        // Error code used for context id creating
        TInt iInitialiseError;

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
        // Helps creating transaction id for pipe creation for eacf contexts
        // before context id available
        TInt iTransIdForProxies;
#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

        // Call id for incoming call (for allow incoming call feature)
        TUint8 iCallModemResourceCallId;

        // List of contexts to be deactivated (for allow incoming call feature)
        CArrayFixFlat<TUint8>* iDeactivateContextList;

        // Buffer for isimessage. Used in resource control
        HBufC8* iResourceControlMsg;

        // drive mode flag
        TBool iDriveModeFlag;

    };

#endif // CMMPACKETCONTEXTMESSHANDLER_H

// End of File
