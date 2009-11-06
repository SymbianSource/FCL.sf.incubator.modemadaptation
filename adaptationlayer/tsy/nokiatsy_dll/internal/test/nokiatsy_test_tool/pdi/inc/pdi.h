/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specifick class for PDI tests
*
*/




#ifndef PDI_H
#define PDI_H

//  INCLUDES
#include "core.h"
#include "ctsy/serviceapi/cmmgsmphonestorageutility.h"
#include "pcktcs.h" //ETel Packet
#include "etelpckt.h" //ETel Packet

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#include <iscapi.h>
#include <pcktcs.h> // for packet data ipcs
#include <pn_const.h>
#include <iscnokiadefinitions.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#else
#include <ctsy/rmmcustomapi.h>
#endif

#if defined(NCP_COMMON_S60_VERSION_SUPPORT ) && ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
#include <ctsy/serviceapi/cmmutility.h>
#else
#include <cmmcommonstaticutility.h>
#endif

// CONSTANTS

// MACROS
// Logging path
_LIT( KSSLogPath, "\\logs\\testframework\\PDI\\" );
// Log file
_LIT( KSSLogFile, "PDI.txt" );

_LIT( KIscControlTestCaseFilePDI, "nokiatsy_pdi.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;


// CLASS DECLARATION
class CPDI : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPDI* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CPDI();


    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /**
        * Completes a request or notification via the specific Tsy object,
        * based on IPC number and data package
        * @since Series60_ver 2.8
        * @param aIpc: IPC number
        *        aDataPackage: Data package
        *        aResult: result
        * @return None
        */
        IMPORT_C virtual void RouteCompletion( TInt aIpc,
            CMmDataPackage* aDataPackage, TInt aResult );


    protected:  // New functions

        /**
        * Complete Notify current network status
        * @since Series60_ver 3.0
        * @param aResult Symbian OS error code
        * @param aDataPackage Data package containgin location area and network information
        * @return Symbian OS error code.
        */
        void CompleteNotifyCurrentNetworkChange( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Notify current network change
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt NotifyCurrentNetworkChange( CStifItemParser& aItem );

      /**
        * Get Network Registration Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetNetworkRegistrationStatusL( CStifItemParser& aItem );

        /**
        * Complete GetNetworkRegistrationStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetNetworkRegistrationStatus( TInt aResult, CMmDataPackage* aDataPackage );

    /**
        * Sets expected registration status on completion of
        * EPacketNotifyChangeOfNtwkRegStatus
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt NotifyChangeOfNtwkRegStatusSetParameterL( CStifItemParser& aItem );

    /**
        * NotifyChangeofNtwkRegStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void NotifyChangeOfNtwkRegStatus( TInt aResult, CMmDataPackage* aDataPackage );

    /**
        * Get HomeNetwork
        * @since Series60_ver 2.8
        * @return Symbian OS error code.
        */
        TInt GetHomeNetworkL( CStifItemParser& aItem );

    /**
        * Complete Get HomeNetwork
        * @since Series60_ver 2.8
        * @param aResult
        *        aDataPackage
        * @return none.
        */
        void CompleteGetHomeNetwork( TInt aResult,
            CMmDataPackage* aDataPackage );

    /**
        * Detach from Packet Service Network
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PacketServiceDetachL( CStifItemParser& aItem );

        /**
        * Complete PacketServiceDetach
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePacketServiceDetach( TInt aResult );

        /**
        * PacketNotifyStatusChange
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void PacketNotifyStatusChange( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Reject Packet Serive Activation request
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PacketServiceRejectActivationRequestL( CStifItemParser& aItem );

        /**
        * Complete PacketServiceRejectActivationRequest
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePacketServiceRejectActivationRequest( TInt aResult );

        /**
        * Get Packet Attach Mode
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetPacketAttachModeL( CStifItemParser& aItem );

        /**
        * Complete GetPacketAttachMode
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetPacketAttachMode( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Attach Packet Service
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PacketServiceAttachL( CStifItemParser& aItem );

        /**
        * Complete PacketServiceAttach
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePacketServiceAttach( TInt aResult );

        /**
        * Attach Packet Service
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetPacketServiceAttachModeL( CStifItemParser& aItem );

        /**
        * Complete SetPacketServiceAttachMode
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetPacketServiceAttachMode( TInt aResult );

        /**
        * Set Defaul Context Parameters for Packet Service
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PacketServiceSetDefaultContextParametersL( CStifItemParser& aItem );

        /**
        * Complete PacketServiceSetDefaultContextParameters
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePacketServiceSetDefaultContextParameters( TInt aResult );

        /**
        * Set Preferred Bearer
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetPreferredBearerL( CStifItemParser& aItem );

        /**
        * Complete SetPreferredBearer
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetPreferredBearer( TInt aResult );

        /**
        * Get data volume transferred
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetDataVolumeTransferredL( CStifItemParser& aItem );

        /**
        * Complete GetDataVolumeTransferred
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetDataVolumeTransferred( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Initialize packet context
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt InitializePacketContextL( CStifItemParser& aItem );

        /**
        * Complete InitializePacketContext
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteInitializePacketContext( TInt aResult );

    /**
        * Activate packet context
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ActivatePacketContextL( CStifItemParser& aItem );

        /**
        * Complete ActivatePacketContext
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteActivatePacketContext( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Setting Packet Context Configuration
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetPacketContextConfigurationL( CStifItemParser& aItem );

        /**
        * Complete SetPacketContextConfiguration
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetPacketContextConfiguration( TInt aResult );

        /**
        * Setting Profile Quality of Service Params
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetProfileQoSParamsL( CStifItemParser& aItem );

        /**
        * Complete Setting Profile Quality of Service Params
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetProfileQoSParams( TInt aResult );

        /**
        * Deactivating Packet Context
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeactivatePacketContextL( CStifItemParser& aItem );

        /**
        * Complete Deactivating Packet Context
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteDeactivatePacketContext( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Deleting Packet Context
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeletePacketContextL( CStifItemParser& aItem );

        /**
        * Complete Deleting Packet Context
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteDeletePacketContext( TInt aResult );

        /**
        * Mapping QoS Params for R99_4
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        void MapQoSR99_4Params(
            RPacketQoS::TQoSR99_R4Requested& aProfile,
            TDesC* aMinTrafficClass,
            TDesC* aMinDeliveryOrderReq,
            TDesC* aMinDeliveryErroneusSDU,
            TDesC* aMaxBER,
            TDesC* aMaxSDUErrorRatio,
            TDesC* aMinTrafficHandlingPriority );

        /**
        * Mapping QoS Params for GPRS
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        void MapQoSGPRS(
            RPacketQoS::TQoSGPRSRequested& aProfileGPRS,
            TDesC* aMinDelay,
            TDesC* aMinMeanThroughput,
            TDesC* aMinPeakThroughput,
            TDesC* aMinPrecedence,
            TDesC* aMinReliability,
            TDesC* aReqDelay,
            TDesC* aReqMeanThroughput,
            TDesC* aReqPeakThroughput,
            TDesC* aReqPrecedence,
            TDesC* aReqReliability );

        /**
        * Adding Packet Filters
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt AddPacketFiltersL( CStifItemParser& aItem );

        /**
        * Adding Media Authorization Params
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt AddMediaAuthorizationParamsL( CStifItemParser& aItem );

        /**
        * Modifying Packet Context
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ModifyPacketContextL( CStifItemParser& aItem );

        /**
        * Complete Modifying Packet Context
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteModifyPacketContext( TInt aResult );

        /**
        * Complete transfer caps change
        * EPacketNotifyTransferCapsIPC
        */
        void CompleteTransferCapsChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Context Status Change Notification
        * EPacketContextNotifyStatusChange
        */
        TInt PacketContextNotifyStatusChange( CStifItemParser& aItem );

        /**
        * Complete Context Status Change Notification
        * EPacketContextNotifyStatusChange
        */
        void CompletePacketContextNotifyStatusChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Context Config Change Notification
        * EPacketContextNotifyConfigChange
        */
        TInt PacketContextNotifyConfigChanged( CStifItemParser& aItem );

        /**
        * Complete Context Config Change Notification
        * EPacketContextNotifyConfigChange
        */
        void CompletePacketContextNotifyConfigChanged(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Setting Notify Dynamic Caps Change Reference Parameters
        */
        TInt SetNotifyDynamicCapsChangeRefParamsL( CStifItemParser& aItem );

        /**
        * Setting Notify Transfer Caps  Reference Parameters
        */
        TInt SetNotifyTransferCapsRefParamsL( CStifItemParser& aItem );

        /**
        * Complete Notify Dynamic Caps Change Notification
        * EPacketNotifyDynamicCapsChange
        */
        void CompleteNotifyDynamicCapsChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Setting Notify MSClass Change Reference Parameters
        */
        TInt SetNotifyMSClassChangeRefParamsL( CStifItemParser& aItem );

        /**
        * Complete Notify MS Class Change Notification
        * EPacketNotifyMSClassChange
        */
        void CompleteNotifyMSClassChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Complete Packet Notify Context Added
        * EPacketNotifyContextAdded
        */
        void CompletePacketNotifyContextAdded(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Complete Packet QoS Notify Profile Changed
        * EPacketQoSNotifyProfileChanged
        */
        void CompletePacketQoSNotifyProfileChanged(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Complete Packet Context Notify Connection Speed Change
        * EPacketContextNotifyConnectionSpeedChange
        */
        void CompletePacketContextNotifyConnectionSpeedChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Set expected connection speed value for comparison
        */
        TInt SetExpectedPacketContextNotifyConnectionSpeedL( CStifItemParser& aItem );

        /**
        * Set expected context misc status
        */
        TInt SetExpectedContextMicsStatusL( CStifItemParser& aItem );

        /**
        * Complete Packet Status Change Notification
        * EPacketNotifyStatusChange
        */
        void CompletePacketNotifyStatusChange(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Set PDI reference parameters for parameter checking
        */
        TInt SetPdiReferenceParameters( CStifItemParser& aItem );

        /**
        * Set reference context name received upon NTSY
        * completing EPacketContextActivate
        */
        TInt SetContextNameForComparison( CStifItemParser& aItem );

    protected:  // Functions from base classes


    private:

    struct TContextParams
      {
      TInfoName iContextName;
        RPacketContext::TProtocolType iPdpType;
        TBool iContextType;
        TInfoName iPrimaryContextName;
        RPacketContext::TContextStatus iContextStatus;
      };

    enum TPacketFilterFilterOperation
          {
          ETftCreateNew,
          EDeleteExisting,
          EAddPacketFilters,
          EReplacePacketFilters,
          EDeletePacketFilters,
          ETftNoOperation
          };

    class TContextConfiguration
          {
          public:
          TInfoName iContextName;
          RPacketContext::TProtocolType iConfiguration;
              RPacketQoS::TQoSGPRSRequested* iQosParams97;
              RPacketQoS::TQoSR99_R4Requested* iQosParams99;
              RPacketQoS::TQoSR5Requested* iQosParamsR5;
              CArrayFixFlat<RPacketContext::TPacketFilterV2>* iTftFilterArray;
              TInt iFilterIdArray[ 8 ];
              TPacketFilterFilterOperation iTftOperation;
          };

    RPacketContext::TContextConfigGPRS* iConfig;

        /**
        * C++ default constructor.
        */
        CPDI( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_ver 2.8
        */
        void Delete();

        /**
        * Add P-CSCF address discovery request into
        * configuration / iProtocolConfigOption.iMiscBuffer.
        * @param aContextConfig: Context configuration parameters
        */
        void AddPCSCFAddressDiscoveryRequest(
            TPacketDataConfigBase& aContextConfig );

        /**
        * Get P-CSCF address from
        * configuration / iProtocolConfigOption.iMiscBuffer.
        * @param aAddress: received P-CSCF address
        * @param aContextConfig: Context configuration parameters
        */
        void GetPCSCFAddress(
            TDes& aAddress,
            const RPacketContext::TContextConfigGPRS& aConfig );

        /**
        * Test methods are listed below.
        */
      CArrayFixFlat<RPacketContext::TPacketFilterV2>* iFilterArray;
      CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>* iMediaAuthorizationArray;


    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * Pointer to logger
        */
        CStifLogger* iPDILog;

          //For message control
        TBool iAttachOngoing;
        TBool iSetQoSOngoing;
        TBool iInitOngoing;
        TBool iActivateOngoing;
        TBool iDeactivateOngoing;
        TBool iDeleteOngoing;
        TBool iDetachOngoing;
        TBool iDefaultConOngoing;
        TBool iSetAttachOngoing;
        TBool iModifyOngoing;
        TBool iGetDataVolOngoing;
        TBool iContextStatusChangeOngoing;
        TBool iPCSCFAddressRequested;
        TBool iContextConfigChangeOngoing;
        TUint iNotifyCurrentNetworkChangeOngoing;
        TBool iGetHomeNetworkOngoing;

        RPacketService::TRegistrationStatus iRegStatusClientForComparison;
        TBool iLastGprsSupportInCellForComparison;
        TBool iLastCsServicesForComparison;
        RPacketService::TMSClass iMsClassForComparison;
        RPacketService::TAttachMode iAttachModeForComparison;
        TInfoName iContextNameForComparison;
        TInt iConnectionSpeedClientForComparison;
        TInt iTransferCaps;

        TBuf<100> iProfile;
        RPacketQoS::TQoSGPRSRequested iProfileGPRS;
        RPacketQoS::TQoSR99_R4Requested iProfileR99_R4;
        RPacketQoS::TQoSR5Requested iProfileR5;
        RPacketContext::TDataVolume iDataVolumeForComparison;
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
        CMmCommonStaticUtility::TContextMisc iMiscForComparison;
#else
        TContextMisc iMiscForComparison;
#endif
        RPacketService::TStatus iPacketStatus;
        TBool iResumed;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };


#endif      // PDI_H

// End of File