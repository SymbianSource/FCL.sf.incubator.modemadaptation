/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for SIM tests
*
*/




#ifndef SIM_H
#define SIM_H

//  INCLUDES
#include "core.h"
#include "ctsy/serviceapi/cmmgsmphonestorageutility.h"

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#include "rmmcustomapiext.h"
#else
#include <ctsy/rmmcustomapi.h>
#endif

// CONSTANTS

// DEFINITIONS
struct SRefPbStorageData
    {
    TBool iIsPhonebookInitialized;
    TBool iIsPhonebookInitializeFailed;
    TInt iXXXNumOfEntries;
    TInt iXXXTextLengthMax;
    TInt iXXXNumberLengthMax;
    };

struct SRefStorageData
    {
    TInt iXXXNumOfEntries ;
    TInt iXXXNumOfEntriesPerEntry ;
    TInt iXXXTextLengthMax ;
    };

enum TPhoneBookType
    {
    TPhoneBookTypeAdn = 0,
    TPhoneBookTypeFdn,
    TPhoneBookTypeSdn,
    TPhoneBookTypeMbdn,
    TPhoneBookTypeUnknown
    };

// MACROS
// Logging path
_LIT( KSIMLogPath, "\\logs\\testframework\\SIM\\" );
// Log file
_LIT( KSIMLogFile, "SIM.txt" );

_LIT(KIscControlTestCaseFileSIM, "nokiatsy_sim.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;
class CSIM;

// CLASS DECLARATION
class CSIM : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIM* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSIM();

        /**
        * Array handling enums
        */
        enum TPBArrayTypeToRead
            {
            EPBStoreCacheArray,
            EPBStoreReadEntriesArray
            };

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
        IMPORT_C virtual void RouteCompletion(
            TInt aIpc,
            CMmDataPackage* aDataPackage,
            TInt aResult );


    protected:  // New functions

        /**
        * Initialize phonebook
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PbInitL( CStifItemParser& aItem );

        /**
        * Complete PB Init
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePbInit( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Cache phone book
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PbStoreCacheL( CStifItemParser& aItem );

        /**
        * Cache phone book cancel
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt PbStoreCacheCancelL( CStifItemParser& aItem );

        /**
        * Complete PbStoreCache
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompletePbStoreCache( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Read entries from sim phone book
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadEntriesL( CStifItemParser& aItem );

        /**
        * Complete CompleteReadEntries
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteReadEntries( TInt aResult );

       /**
        * Write entries to sim phone book
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt WriteEntriesL( CStifItemParser& aItem );

        /**
        * Complete WriteEntries
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteWriteEntries( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Delete entry from sim phone book
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeleteEntryL( CStifItemParser& aItem );

        /**
        * Complete Delete
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteDelete( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Delete all Entries from sim phone book
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeleteAllEntriesL( CStifItemParser& aItem );

        /**
        * Get Pb Store Info
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetPbStoreInfoL( CStifItemParser& aItem );

        /**
        * Complete GetPbStoreInfo
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetPbStoreInfo( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Read EN Store
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadENStore( CStifItemParser& aItem );

        /**
        * Complete ReadENStore
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteReadENStore( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Read EN Store all
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadENStoreAll( CStifItemParser& aItem );

        /**
        * Complete ReadENStoreAll
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteReadENStoreAll( TInt aResult, CMmDataPackage* aDataPackage );


        /**
        * Get EN Store Info
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetENStoreInfo( CStifItemParser& aItem );

        /**
        * Complete GetENStoreInfo
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetENStoreInfo( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Read ON Store
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadONStore( CStifItemParser& aItem );

        /**
        * Complete ReadONStore
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteReadONStore( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Read ON Store Entry
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadONStoreEntry( CStifItemParser& aItem );

        /**
        * Complete ReadONStore Entry
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteReadONStoreEntry( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get own number Store Info
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetONStoreInfo( CStifItemParser& aItem );

        /**
        * Complete GetONStoreInfo
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetONStoreInfo( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Delete own number Store Entry
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeleteONStoreEntry( CStifItemParser& aItem );

        /**
        * Complete DeleteONStoreEntry
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteDeleteONStoreEntry( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Delete all own number Store Entries
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeleteAllONStoreEntries( CStifItemParser& aItem );

        /**
        * Write ON Store
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt WriteONStore( CStifItemParser& aItem );

        /**
        * Complete WriteONStore
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteWriteONStore( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Read ON Store size
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadONStoreSize( CStifItemParser& aItem );

        /**
        * Read ViagHomeZone param
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ViagHomeZoneReadParamL( CStifItemParser& aItem );

        /**
        * Read ViagHomeZone cache
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ViagHomeZoneReadCacheL( CStifItemParser& aItem );

        /**
        * Write ViagHomeZone settings
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ViagHomeZoneWriteSettingsL( CStifItemParser& aItem );

        /**
        * Write ViagHomeZone cache
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ViagHomeZoneWriteCacheL( CStifItemParser& aItem );

        /**
        * Complete WriteONStore
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteReadONStoreSize( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get Mailbox Numbers
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetMailboxNumbers( CStifItemParser& aItem );

        /**
        * Complete GetMailboxNumbers
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetMailboxNumbers( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Write ON Store All
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt WriteONStoreAll( CStifItemParser& aItem );

        /**
        * Complete WriteONStore All
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteWriteONStoreAll( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Read ON Store size
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt OnStoreWriteSize( CStifItemParser& aItem );

        /**
        * Complete WriteONStore
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteOnStoreWriteSize( TInt aResult, CMmDataPackage* aDataPackage );


        /**
        * Get Service Table
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetServiceTable( CStifItemParser& aItem );

        /**
        * Complete Get Service Table
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetServiceTable( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get ALS Line
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetALSLine( CStifItemParser& aItem );

        /**
        * Complete Get ALS Line
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetALSLine( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get ALS Line
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetALSLine( CStifItemParser& aItem );

        /**
        * Complete Get ALS Line
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetALSLine( TInt aResult );

        /**
        * Get ALS Line
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetCustomerServiceProfile( CStifItemParser& aItem );

        /**
        * Complete Get ALS Line
        * @since Series60_ver 3.0
        * @param aResult
        * @param aDataPackage data package
        * @return None
        */
        void CompleteGetCustomerServiceProfile
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Get ALS Line
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetPin1DisableSupported( CStifItemParser& aItem );


        /**
        * Complete Get ALS Line
        * @since Series60_ver 3.0
        * @param aResult
        * @param aDataPackage data package
        * @return None
        */
        void CompleteGetPin1DisableSupported
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Sim Refresh Register
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SimRefreshRegister( CStifItemParser& aItem );

        /**
        * Complete SimRefreshRegister
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSimRefreshRegister( TInt aResult );

        /**
        * Sim Refresh Register
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SimRefreshDone( CStifItemParser& aItem );

        /**
        * Complete SimRefreshRegister
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSimRefreshDone( TInt aResult );

        /**
        * Get Subscriber Id
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetSubscriberId( CStifItemParser& aItem );

        /**
        * Complete GetSubscriberId
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetSubscriberId( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get Subscriber Id
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetServiceProviderName( CStifItemParser& aItem );

        /**
        * Complete GetSubscriberId
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetServiceProviderName
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Read SIM File
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadSimFile( CStifItemParser& aItem );

        /**
        * Complete ReadSimFile
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteReadSimFile( TInt aResult, CMmDataPackage* aDataPackage );

        /**
        * Get Network Provider Name
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetNetworkProviderName( CStifItemParser& aItem );

        /**
        * Complete GetNetworkProviderName
        * @since Series60_ver 3.0
        * @param aResult
        * @param aDataPackage data package
        * @return None
        */
        void CompleteGetNetworkProviderName
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Get SIM Authentication Data
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetSimAuthenticationData( CStifItemParser& aItem );

        /**
        * Complete GetSimAuthenticationData
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetSimAuthenticationData
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Authenticate IMS Data
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt AuthenticateIMSData( CStifItemParser& aItem );

        /**
        * Complete AuthenticateIMSData
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteAuthenticateIMSData
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Get IMS Authorization Info
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetIMSAuthorizationInfo( CStifItemParser& aItem );

        /**
        * Complete GetIMSAuthorizationInfo
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetIMSAuthorizationInfo
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Get ALS Blocked Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt GetALSBlockedStatus( CStifItemParser& aItem );

        /**
        * Complete GetALSBlockedStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteGetALSBlockedStatus
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Set ALS Blocked Status
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetALSBlockedStatus( CStifItemParser& aItem );

        /**
        * Complete SetALSBlockedStatus
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSetALSBlockedStatus
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * SendSIMRefreshNtf
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SendSIMRefreshNtf( CStifItemParser& aItem );

        /**
        * Complete SendSIMRefreshNtf
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteSendSIMRefreshNtf( TInt aResult );

        /**
        * Check alternative line service product profile support
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt CheckAlsPpSupport( CStifItemParser& aItem );

        /**
        * Complete CheckAlsPpSupport
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        void CompleteCheckAlsPpSupport
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Set flag iTestOngoing
        * @since Series60_ver 5.0
        * @param None
        * @return TInt
        */
        TInt SetTestOngoingFlag();

        /**
        * Reads FDN information by using SIM_DATA_SIZES_REQ
        * @since Series60_ver 5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt FdnDataSizesReq( CStifItemParser& aItem );

        /**
        * Complete FdnDataSizesReq
        * @since Series60_ver 5.0
        * @param aResult
        * @param aDataPackage
        * @return None
        */
        void CompleteFdnDataSizesReq
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Check service support availability from UICC.
        * @since Series60_ver 3.2
        * @param aItem        Script line containing method name and parameters
        * @return             Symbian OS error code.
        */
        TInt CheckServiceSupport( CStifItemParser& aItem );

        /**
        * Completes service support availability check
        * @since Series60_ver 3.2
        * @param aResult        Result code returned from NokiaTSY
        * @param aDataPackage   Result data from NokiaTSY
        * @return None
        */
        void CompleteCheckServiceSupport(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Sets expected number of phonebook entries checked on IPC completion
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt SetExpectedNumberOfPbEntries( CStifItemParser& aItem );

        /**
        * Performs verification of the data returned when
        * EReadViagHomeZoneParamsIPC is completed
        * @since Series60_ver 3.2
        * @param aResult        Result code returned from NokiaTSY
        * @param aDataPackage   Result data from NokiaTSY
        * @return None
        */
        void CompleteViagHomeZoneReadParam(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Performs verification of the data returned when
        * EReadViagHomeZoneCacheIPC is completed
        * @since Series60_ver 3.2
        * @param aResult        Result code returned from NokiaTSY
        * @param aDataPackage   Result data from NokiaTSY
        * @return None
        */
        void CompleteViagHomeZoneReadCache(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
        * Requests reading phonebook entry with invalid input parameters
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt InvalidReadEntries( CStifItemParser& aItem );

        /**
        * Requests writing phonebook entry with invalid input parameters
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt InvalidWriteEntries( CStifItemParser& aItem );

        /**
        * Requests cacheing phonebook entries with invalid input parameters
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt InvalidPbStoreCache( CStifItemParser& aItem );

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        /**
        * Write alpha string
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt WriteAlphaString( CStifItemParser& aItem );

        /**
        * Complete Write alpha string
        * @since Series60_ver 3.0
        * @param aResult
        * @param aDataPackage: Data package
        * @return None
        */
        void CompleteWriteAlphaString
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Read alpha string
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt ReadAlphaString( CStifItemParser& aItem );

        /**
        * Complete Read alpha string
        * @since Series60_ver 3.0
        * @param aResult
        * @param aDataPackage: Data package
        * @return None
        */
        void CompleteReadAlphaString
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );

        /**
        * Delete alpha string
        * @since Series60_ver 3.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code.
        */
        TInt DeleteAlphaString( CStifItemParser& aItem );

        /**
        * Complete Delete alpha string
        * @since Series60_ver 3.0
        * @param aResult
        * @param aDataPackage: Data package
        * @return None
        */
        void CompleteDeleteAlphaString
            (
            TInt aResult,
            CMmDataPackage* aDataPackage
            );
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CSIM( CTestModuleIf& aTestModuleIf );

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

        /**
        * Prints Pb Init Info
        * @since Series60_ver 3.0
        * @param aPbInitInfo
        * @return None
        */
        void PrintPbInitInfo(CStorageInfoData* aPbInitInfo);

        /**
        * Prints Entries From Array
        * @since Series60_ver 3.0
        * @param aArrayType
        * @param aPbName phonebook name
        * @return aResult
        */
        TInt ReadEntriesFromArray(
            TPBArrayTypeToRead aArrayType,
            TDesC16& aPbName );

        /**
        * Checks number of entries in the PB cache array
        * @since Series60_ver 3.0
        * @param aResult
        * @return None
        */
        TInt CheckNumberOfPbEntries(
                TPBArrayTypeToRead aArrayType,
                TDesC16& aPbName );

        /**
        * Checks phonebook data
        * @since Series60_ver 5.0
        * @param pbData
        * @param pbType
        * @return aResult
        */
        TInt CheckPbData( CStorageInfoData* aPbData, TPhoneBookType aPbType );

        /**
        * Converts source string containing ascii characters 0-9, a-f, A-F to
        * corresponding hex value located to destination buffer. Aimed to be
        * used when hex data is inputted from .cfg for comparision with
        * value returned by NokiaTSY. Source must be able to divide by two
        * without remainder.
        * @since Series60_ver 3.2
        * @param aSource    Source string
        * @param aDest      Destination buffer for hex value
        * @return TInt      KErrNone if ok, KErrGeneral if no valid character
        *                   has been found, KErrArgument if it is not
        *                   possible to divide source by two.
        */
        TInt GetHexFromString( TPtrC aSource, TDes8& aDest ) const;

        /**
        * Generates and send SIM_AUTHENTICATION_REQ related to MGV
        * @since Series60_ver 3.2
        * @param aString       String containing request type
        * @param aItem         Script line containing method name and parameters
        * @return TInt         KErrNone or error code
        */
        TInt GenerateMgvAuthenticationReq(
            TPtrC aString,
            CStifItemParser& aItem );

        /**
        * Generates and send SIM_AUTHENTICATION_REQ related to MGV
        * @since Series60_ver 3.2
        * @param aString       String containing request type
        * @param aItem         Script line containing method name and parameters
        * @return TInt         KErrNone or error code
        */
        TInt GenerateGbaAuthenticationReq(
            TPtrC aString,
            CStifItemParser& aItem );

        /**
        * Handles completions related to MGV.
        * @since Series60_ver 3.2
        * @param aResult        Result code from NokiaTSY
        * @param aDataPackage   Result data from NokiaTSY
        * @return None
        */
        void HandleMgvCompletion( TInt& aResult, CMmDataPackage* aDataPackage );

        /**
        * Handles completions related to General Bootstrapping.
        * @since Series60_ver 3.2
        * @param aResult        Result code from NokiaTSY
        * @param aDataPackage   Result data from NokiaTSY
        * @return None
        */
        void HandleGbaCompletion( TInt& aResult, CMmDataPackage* aDataPackage );

    public:     // Data

    protected:  // Data

    private:    // Data
       /**
        * Pointer to logger
        */
        CStifLogger*        iSIMLog;

        // Name of the phonebook
        TName iPhoneBookName;
        
        // Card type
        TName iCardType;

        // Pointer to phonebook cache
        CArrayPtrSeg<CPhoneBookStoreEntry>* iPBStoreCacheAdn;
        CArrayPtrSeg<CPhoneBookStoreEntry>* iPBStoreCacheFdn;
        CArrayPtrSeg<CPhoneBookStoreEntry>* iPBStoreCacheSdn;

        // Classes to store auhtentication parameters between request and
        // response.
        RMmCustomAPI::TSimAuthenticationGbaBootstrap
            iAuthenticationGbaBoot;
        RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate
            iAuthenticationGbaBootUpdate;
        RMmCustomAPI::TSimAuthenticationGbaNafDerivation
            iAuthenticationGbaNafDerivation;
        RMmCustomAPI::TSimAuthenticationMgvMskUpdate
            iAuthenticationMgvMskUpdate;
        RMmCustomAPI::TSimAuthenticationMgvMtkGeneration
            iAuthenticationMgvMtkGeneration;

        // Indicates for which scenario test is for.
        TUint iAuthenticationPurpose;

        // Indicates is service support request made with assumption
        // that service is available or not.
        TBool iServiceAvailable;

        //pointer to read entry
        CArrayPtrSeg<CPhoneBookStoreEntry>* iPBStoreReadEntries;

        RMobilePhoneBookStore::TPBIndexAndNumEntries* iIndexAndEntries;

        CPhoneBookStoreEntry* iPhoneBookEntry;

        // EN number array
        CArrayPtrFlat<TENStoreResponse>* iENStoreArray;

        TBool iSendOngoing;

        TBool iTestOngoing;

        // The following variables are for storing given parameters
        // for checking the validity of completed data
        TInt iNumOfUsedPbEntries;
        TInt iIndex;
        TInt iIndexTable[3];
        TInt iDisplayReq;
        TBuf8<RMobilePhone::KIMSISize> iImsi;
        TBuf<20> iNumber;
        TBuf<20> iNumberTable[3];
        TBuf<20> iName;
        TBuf<20> iSavedNameForComparison[2];
        TBuf<20> iSavedNumberForComparison[2];
        TBuf<20> iSavedEmailForComparison[2];
        TBuf<20> iSavedSneForComparison[2];
        TBuf<20> iSavedAnrForComparison[2];
        TInt iSavedPbEntriesCount;
        SRefPbStorageData iPbStorageDataAdn;
        SRefPbStorageData iPbStorageDataFdn;
        SRefPbStorageData iPbStorageDataSdn;
        SRefPbStorageData iPbStorageDataMbdn;
        SRefPbStorageData iPbStorageDataMsisdn;
        SRefPbStorageData iPbStorageDataVmbx;
        SRefStorageData iStorageDataSne;
        SRefStorageData iStorageDataAnr;
        SRefStorageData iStorageDataEmail;
        TPhoneBookType iPbType[2];
        TPBEntryInfo iPbEntryInfo;
        TServiceType iServiceType;
        RMobilePhone::TMobilePhoneALSLine iAlsLine;
        RMobilePhone::TMobilePhoneCspFileV1 iCspFile;
        RMmCustomAPI::TGetAlsBlockStatus iAlsBlockedStatus;
        RMmCustomAPI::TAlsSupport iAlsSupport;

        // Comparison data for ViagHZ testcases
        RMmCustomAPI::TViagParams iSavedViagParamsForComparison;
        RMmCustomAPI::TViagElements* iSavedViagElementsForComparison;
        RMmCustomAPI::TViagCacheRecordContent iSavedRecordForComparison;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // SIM_H

// End of File
