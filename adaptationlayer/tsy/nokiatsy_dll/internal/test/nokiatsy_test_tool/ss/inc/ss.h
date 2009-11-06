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
* Description:   Specifick class for SS tests
*
*/




#ifndef SS_H
#define SS_H

//  INCLUDES
#include "core.h"
#include "ctsy/serviceapi/cmmgsmphonestorageutility.h"

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#else
#include <ctsy/rmmcustomapi.h>
#endif

// CONSTANTS

// MACROS
// Logging path
_LIT( KSSLogPath, "\\logs\\testframework\\SS\\" );
// Log file
_LIT( KSSLogFile, "SS.txt" );

_LIT(KIscControlTestCaseFileCSS, "nokiatsy_ss.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;
class CSS;


// CLASS DECLARATION
class CSS : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSS* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSS();


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
        * Get Call Barring Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetCallBarringStatus( CStifItemParser& aItem );

        /**
        * Complete GetCallBarringStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetCallBarringStatus( TInt aResult, CMmDataPackage* aDataPackage );


        /**
        * Get Call Barring Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetCallForwardingStatus( CStifItemParser& aItem );

        /**
        * Complete GetCallBarringStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetCallForwardingStatus( TInt aResult, CMmDataPackage* aDataPackage );

         /**
        * Get Call Waiting Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetCallWaitingStatus( CStifItemParser& aItem );

        /**
        * Complete GetCallWaitingStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetCallWaitingStatus( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get Identity Service Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetIdentityServiceStatus( CStifItemParser& aItem );

        /**
        * Complete GetIdentityServiceStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetIdentityServiceStatus( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Set Call Barring Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetCallBarringStatus( CStifItemParser& aItem );

        /**
        * Complete SetCallBarringStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetCallBarringStatus( TInt aResult );

        /**
        * Set Call Forwarding Service Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetCallForwardingStatus( CStifItemParser& aItem );

        /**
        * Complete SetCallForwardingStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetCallForwardingStatus( TInt aResult );

        /**
        * Set Call Waiting Service Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetCallWaitingStatus( CStifItemParser& aItem );

        /**
        * Complete SetCallWaitingStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetCallWaitingStatus( TInt aResult );

        /**
        * Set Supplementary Services Password
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetSupplementaryServicesPassword( CStifItemParser& aItem );

        /**
        * Complete SetSupplementaryServicesPassword
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetSupplementaryServicesPassword( TInt aResult );

        /**
        * PrintStatusInfo
        * @param aIpc: IPC number
        * @param aDataPackage: Data package
        * @return None
        */
        void PrintStatusInfo( CMmDataPackage* aDataPackage, TInt aIpc );

        /**
        * Get Call Forwarding Number
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetCallForwardingNumber( CStifItemParser& aItem );

        /**
        * Complete GetCallForwardingNumber
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetCallForwardingNumber( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get Icc Call Forwarding Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetIccCallForwardingStatus( CStifItemParser& aItem );

        /**
        * Complete GetCallForwardingNumber
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetIccCallForwardingStatus( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get Icc Message Waiting Indicators
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetIccMessageWaitingIndicators( CStifItemParser& aItem );

        /**
        * Complete Icc Message Waiting Indicators
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetIccMessageWaitingIndicators( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Set Icc Message Waiting Indicators
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetIccMessageWaitingIndicators( CStifItemParser& aItem );

        /**
        * Complete Icc Message Waiting Indicators
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetIccMessageWaitingIndicators( TInt aResult );

        /**
        * Get Call Forwarding Number
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendUSSDMessage( CStifItemParser& aItem );

        /**
        * Complete SendUSSDMessage
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSendUSSDMessage( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * SSServiceFailed
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SSServiceFailed( CStifItemParser& aItem );

        /**
        * Complete SSServiceFailed
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSSServiceFailed( TInt aResult );

        /**
        * USSDSendRelease
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt USSDSendRelease( CStifItemParser& aItem );

        /**
        * Complete USSDSendRelease
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteUSSDSendRelease( TInt aResult );

        /**
        * SendNetworkServiceRequest
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendNetworkServiceRequest( CStifItemParser& aItem );

        /**
        * Complete SendNetworkServiceRequest
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSendNetworkServiceRequest( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * SendNetworkServiceRequestNoFdnCheck
        * @since Series60_ver 5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendNetworkServiceRequestNoFdnCheck( CStifItemParser& aItem );

        /**
        * CompleteSendNetworkServiceRequestNoFdnCheck
        * @since Series60_ver 5.0
        * @param aResult
        * @param aDataPackage
        * @return None
        */
        void CompleteSendNetworkServiceRequestNoFdnCheck(
            TInt aResult,
            CMmDataPackage* aDataPackage );


        /**
        * SSServiceCompleted
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SSServiceCompleted( CStifItemParser& aItem );

        /**
        * Complete SSServiceCompleted
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSSServiceCompleted( TInt aResult );

        /**
        * Complete CompleteNotifyCallBarringStatusChange
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteNotifyCallBarringStatusChange( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Complete NotifyCallForwardingStatusChange
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteNotifyCallForwardingStatusChange( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Complete NotifyCallForwardingStatusChange
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteNotifyCallWaitingStatusChange( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Complete NotifySendNetworkServiceRequest
        * @since Series60_ver 3.0
        * @param aResult
        * @param aDataPackage
        * @return None
        */
        void CompleteNotifySendNetworkServiceRequest( TInt aResult, CMmDataPackage* aDataPackage );

        /**
         * Set flag iSSSendNetworkServiceRequestTestOngoing
         * @since Series60_ver 5.0
         * @param aItem
         * @return TInt
         */
         TInt SetSendNetworkServiceRequestTestOngoingFlag( CStifItemParser& aItem );

        /**
         * Send ESatNotifyCallControlRequest
         * @since Series60_ver 5.0
         * @param aItem
         * @return TInt
         */
         TInt SendSatNotifyCallControlRequest( CStifItemParser& aItem );

    protected:  // Functions from base classes


    private:

        /**
        * C++ default constructor.
        */
        CSS( CTestModuleIf& aTestModuleIf );

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
        * Test methods are listed below.
        */




    public:     // Data

    protected:  // Data

    private:    // Data

        TBool iSSFailTestOngoing;

        TBool iUSSDSendReleaseTestOngoing;

        TBool iCompleteFailInUSSDHandler;

        TBool iSSCompleteTestOngoing;

        TBool iSSSendNetworkServiceRequestTestOngoing;

        RMobilePhone::TMobilePhoneCBInfoEntryV1 iCBInfoEntry;

        RMobilePhone::TMobilePhoneCFInfoEntryV1 iCFInfoEntry;

        RMobilePhone::TMobilePhoneCWInfoEntryV1 iCWInfoEntry;

        RMobilePhone::TMobilePhoneIdServiceStatus iStatusETel;

        RMobilePhone::TMobilePhoneCBCondition iCBCondition;

        // This is not used yet.
        RMobilePhone::TMobilePhoneCFCondition iCFCondition;

        RMobilePhone::TMobilePhoneMessageWaitingV1 iMSGWaiting;

        // Saves GetIccMessageWaitingIndicators type (Rel4 or CPHS)
        TUint8 iIccMessageWaitingIndicatorsType;

        /**
        * Pointer to logger
        */
        CStifLogger* iSSLog;

        // When this is set all Icc call forwardings are expected to be set.
        TBool iAllForwardingsOn;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // SIM_H

// End of File