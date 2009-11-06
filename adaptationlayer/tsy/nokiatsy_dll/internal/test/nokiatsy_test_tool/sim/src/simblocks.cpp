/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "sim.h"


// CONSTANTS
_LIT8( KLine, "---------------------------------" );

_LIT8( KAdnPhoneBook, "AdnPhoneBook" );
_LIT8( KBdnPhoneBook, "BdnPhoneBook" );
_LIT8( KSdnPhoneBook, "SdnPhoneBook" );
_LIT8( KFdnPhoneBook, "FdnPhoneBook" );
_LIT8( KMBDN, "MBDN" );
_LIT8( KVoiceMailBox, "VoiceMailBox" );

_LIT( KViagInactive, "Inactive" );
_LIT( KViagCZActive, "CZActive" );
_LIT( KViagHZActive, "HZActive" );

const TPtrC PhoneALSLine[] =
    {
    _L("EAlternateLinePrimary"),
    _L("EAlternateLineAuxiliary"),
    _L("EAlternateLineUnknown"),
    _L("EAlternateLineNotAvailable")
    };

// Indicator what kind of test is on going.
enum TAuthenticationTestPurpose
    {
    EGbaBootOk = 1,
    EGbaBootAuthFail,
    EGbaBootVerificationFail,
    EGbaNafDerOk,
    EGbaNafDerAuthFail,
    EGbaBootUpdateOk,
    EGbaBootUpdateError,
    EGbaBootError,
    EGbaBootNafError,
    EMgvMskUpdateOk = 11,
    EMgvMskUpdateNull,
    EMgvMskUpdateError,
    EMgvMtkGenerationOk,
    EMgvMtkGenerationError,
    EMgvMskDeletionOk,
    EMgvMskDeletionError,
    EMgvMskDeletionAuthFail,
    EMgvMskUpdateWithOmaBCastExtension,
    EMgvMtkGenerationWithBcastManagementData,
    EMgvMtkGenerationWithBCastParentalControlData,
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIM::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CSIM::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CSIM::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSIM::RunMethodL(
    CStifItemParser& aItem )
    {
   _LIT8 (KLog, "SIM: RunMethodL" );
   iSIMLog->Log((TDesC8)KLog);

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "PbInit",                  CSIM::PbInitL ),
        ENTRY( "PbStoreCache",            CSIM::PbStoreCacheL ),
        ENTRY( "PbStoreCacheCancel",      CSIM::PbStoreCacheCancelL ),
        ENTRY( "ReadEntries",             CSIM::ReadEntriesL ),
        ENTRY( "WriteEntries",            CSIM::WriteEntriesL ),
        ENTRY( "DeleteEntry",             CSIM::DeleteEntryL ),
        ENTRY( "DeleteAllEntries",        CSIM::DeleteAllEntriesL ),
        ENTRY( "GetPbStoreInfo",          CSIM::GetPbStoreInfoL ),
        ENTRY( "ReadENStore",             CSIM::ReadENStore ),
        ENTRY( "ReadENStoreAll",          CSIM::ReadENStoreAll ),
        ENTRY( "GetENStoreInfo",          CSIM::GetENStoreInfo ),
        ENTRY( "ReadONStore",             CSIM::ReadONStore ),
        ENTRY( "ReadONStoreEntry",        CSIM::ReadONStoreEntry ),
        ENTRY( "GetONStoreInfo",          CSIM::GetONStoreInfo ),
        ENTRY( "DeleteONStoreEntry",      CSIM::DeleteONStoreEntry ),
        ENTRY( "DeleteAllONStoreEntries", CSIM::DeleteAllONStoreEntries ),
        ENTRY( "WriteONStore",            CSIM::WriteONStore ),
        ENTRY( "ReadONStoreSize",         CSIM::ReadONStoreSize ),
        ENTRY( "GetMailboxNumbers",       CSIM::GetMailboxNumbers ),
        ENTRY( "WriteONStoreAll",         CSIM::WriteONStoreAll ),
        ENTRY( "OnStoreWriteSize",        CSIM::OnStoreWriteSize ),
        ENTRY( "GetServiceTable",         CSIM::GetServiceTable ),
        ENTRY( "GetALSLine",              CSIM::GetALSLine ),
        ENTRY( "SetALSLine",              CSIM::SetALSLine ),
        ENTRY( "GetCustomerServiceProfile", CSIM::GetCustomerServiceProfile ),
        ENTRY( "GetPin1DisableSupported", CSIM::GetPin1DisableSupported ),
        ENTRY( "SimRefreshRegister",      CSIM::SimRefreshRegister ),
        ENTRY( "SimRefreshDone",          CSIM::SimRefreshDone ),
        ENTRY( "GetSubscriberId",         CSIM::GetSubscriberId ),
        ENTRY( "GetServiceProviderName",  CSIM::GetServiceProviderName ),
        ENTRY( "ReadSimFile",             CSIM::ReadSimFile ),
        ENTRY( "GetNetworkProviderName",  CSIM::GetNetworkProviderName ),
        ENTRY( "GetSimAuthenticationData",CSIM::GetSimAuthenticationData ),
        ENTRY( "AuthenticateIMSData",     CSIM::AuthenticateIMSData ),
        ENTRY( "GetIMSAuthorizationInfo", CSIM::GetIMSAuthorizationInfo ),
        ENTRY( "GetALSBlockedStatus",     CSIM::GetALSBlockedStatus ),
        ENTRY( "SetALSBlockedStatus",     CSIM::SetALSBlockedStatus ),
        ENTRY( "SendSIMRefreshNtf",       CSIM::SendSIMRefreshNtf),
        ENTRY( "CheckAlsPpSupport",       CSIM::CheckAlsPpSupport),
        ENTRY( "FdnDataSizesReq",         CSIM::FdnDataSizesReq),
        ENTRY( "GetServiceSupport",       CSIM::CheckServiceSupport),
        ENTRY( "SetExpectedNumberOfPbEntries", CSIM::SetExpectedNumberOfPbEntries),
        ENTRY( "ViagHomeZoneReadParam",   CSIM::ViagHomeZoneReadParamL),
        ENTRY( "ViagHomeZoneReadCache",   CSIM::ViagHomeZoneReadCacheL),
        ENTRY( "ViagHomeZoneWriteSettings", CSIM::ViagHomeZoneWriteSettingsL),
        ENTRY( "ViagHomeZoneWriteCache",  CSIM::ViagHomeZoneWriteCacheL),
        ENTRY( "InvalidReadEntries",      CSIM::InvalidReadEntries),
        ENTRY( "InvalidWriteEntries",     CSIM::InvalidWriteEntries),
        ENTRY( "InvalidPbStoreCache",     CSIM::InvalidPbStoreCache),


#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        ENTRY( "WriteAlphaString",        CSIM::WriteAlphaString),
        ENTRY( "ReadAlphaString",         CSIM::ReadAlphaString),
        ENTRY( "DeleteAlphaString",       CSIM::DeleteAlphaString),
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC",  CCore::SetExpectedCompleteIPC),
        ENTRY( "SetTestOngoingFlag",      CSIM::SetTestOngoingFlag),
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "SetOperInfoAPAC",         CCore::SetOperInfoAPAC ),
        ENTRY( "RunIscTestCase",          CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase",         CCore::SyncIscTestCase),
        ENTRY( "RunIscTestCaseStartNTSYAfterTestToolStart",
                CCore::RunIscTestCaseStartNTSYAfterTestToolStart),
        ENTRY( "SyncIscTestCaseStartNTSYAfterTestToolStart",
                CCore::SyncIscTestCaseStartNTSYAfterTestToolStart),
        };


    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CSIM::PbInitL
// Phonebook initialization
// -----------------------------------------------------------------------------
//
TInt CSIM::PbInitL( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SIM::PbInit");
    iSIMLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString( string );

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        if ( iHandleSpecifiedRequests )
            {
            // Check for card Type 
            TInt value;
            aItem.GetNextString( string );

            if ( _L("UICC") == string )
                {
                iCardType = _L("UICC");
                aItem.GetNextInt( value );
                iStorageDataAnr.iXXXTextLengthMax = value;
                aItem.GetNextInt( value );
                iStorageDataAnr.iXXXNumOfEntriesPerEntry = value;
                aItem.GetNextInt( value );
                iStorageDataAnr.iXXXNumOfEntries = value;

                aItem.GetNextInt( value );
                iStorageDataSne.iXXXNumOfEntries = value;
                aItem.GetNextInt( value );
                iStorageDataSne.iXXXNumOfEntriesPerEntry = value;
                aItem.GetNextInt( value );
                iStorageDataSne.iXXXTextLengthMax = value;
                
                aItem.GetNextInt( value );
                iStorageDataEmail.iXXXNumOfEntries = value;
                aItem.GetNextInt( value );
                iStorageDataEmail.iXXXNumOfEntriesPerEntry = value;
                aItem.GetNextInt( value );
                iStorageDataEmail.iXXXTextLengthMax = value;
                }
            // store phonebook info
            aItem.GetNextInt( value );
            iPbStorageDataAdn.iIsPhonebookInitialized = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataAdn.iIsPhonebookInitializeFailed = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataAdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataAdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataAdn.iXXXNumberLengthMax = value;
            
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXNumberLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXNumberLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataMbdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataMbdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataMbdn.iXXXNumberLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataMsisdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataMsisdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataMsisdn.iXXXNumberLengthMax = value;
            
            if( _L("ICC") == string )
                {
                iCardType = _L("ICC");

                aItem.GetNextInt( value );
                iPbStorageDataVmbx.iXXXNumOfEntries = value;
                aItem.GetNextInt( value );
                iPbStorageDataVmbx.iXXXTextLengthMax = value;
                aItem.GetNextInt( value );
                iPbStorageDataVmbx.iXXXNumberLengthMax = value;
                }
            aItem.GetNextInt( value );
            iPbType[value] = TPhoneBookTypeAdn;
            }
        }
    else if ( _L("Bdn") == string )
        {
        iSIMLog->Log((TDesC8)KBdnPhoneBook);
        iPhoneBookName = KETelIccBdnPhoneBook;
        }
    else if ( _L("Sdn") == string )
        {
        iSIMLog->Log((TDesC8)KSdnPhoneBook);
        iPhoneBookName = KETelIccSdnPhoneBook;
        if ( iHandleSpecifiedRequests )
            {
            // store phonebook info
            TInt value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iIsPhonebookInitialized = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iIsPhonebookInitializeFailed = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataSdn.iXXXNumberLengthMax = value;
            aItem.GetNextInt( value );
            iPbType[value] = TPhoneBookTypeSdn;
            }
        }
    else if ( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        if ( iHandleSpecifiedRequests )
            {
            // store phonebook info
            TInt value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iIsPhonebookInitialized = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iIsPhonebookInitializeFailed = ( TBool )value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXNumOfEntries = value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXTextLengthMax = value;
            aItem.GetNextInt( value );
            iPbStorageDataFdn.iXXXNumberLengthMax = value;
            aItem.GetNextInt( value );
            iPbType[value] = TPhoneBookTypeFdn;
            }
        }
    else if ( _L("VoiceMailBox") == string )
        {
        iSIMLog->Log((TDesC8)KVoiceMailBox);
        iPhoneBookName = KETelIccVoiceMailBox;
        }
    else
        {
        _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );

        ret = HandleRequestL( EMmTsyPhoneBookStoreInitIPC,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "PbInit request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "PbInit request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompletePbInit
// Complete PbInit method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompletePbInit
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompletePbInit result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );
    // Response information
    CStorageInfoData* pbInitResp;
    TInt i;

    if ( KErrNone == aResult )
        {
        aDataPackage->UnPackData ( &pbInitResp );
        PrintPbInitInfo( pbInitResp );

        if ( iHandleSpecifiedRequests )
            {
            // select used phonebook type for data validation
            TPhoneBookType pbType( TPhoneBookTypeUnknown );
            for ( i = 0; i < 2; i++)
                {
                if ( iPbType[i] != TPhoneBookTypeUnknown )
                    {
                    pbType = iPbType[i];
                    break;
                    }
                }
            aResult = CheckPbData( pbInitResp, pbType );
            // reset phonebook type
            iPbType[i] = TPhoneBookTypeUnknown;
            }
        }
    else
        {
        _LIT8( KFail, "CompletePbInit Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    // in case of failure, send result immediately
    if( KErrNone != aResult )
        {
        Signal( aResult );
        }

    // in successful case, send result if test is not ongoing
    else if( ! iTestOngoing )
        {
        Signal( aResult );
        }

    iTestOngoing = EFalse;
    }

// -----------------------------------------------------------------------------
// CSIM::PrintPbInitInfo
// Prints Pb info
// -----------------------------------------------------------------------------
//
void CSIM::PrintPbInitInfo
        (
        CStorageInfoData* aPbInitInfo
        )
    {
    iSIMLog->Log((TDesC8)KLine );
    _LIT8( KText, "Pb Init Info" );
    iSIMLog->Log((TDesC8)KText );

    _LIT8( KADNNumOfEntries, "ADNNumOfEntries: %d" );
    iSIMLog->Log((TDesC8)KADNNumOfEntries, aPbInitInfo->iADNNumOfEntries );

    _LIT8( KADNTextLengthMax, "ADNTextLengthMax: %d" );
    iSIMLog->Log((TDesC8)KADNTextLengthMax, aPbInitInfo->iADNTextLengthMax );

    _LIT8( KADNNumberLengthMax, "ADNNumberLengthMax: %d" );
    iSIMLog->Log((TDesC8)KADNNumberLengthMax, aPbInitInfo->iADNNumberLengthMax );

    _LIT8( KFDNNumOfEntries, "FDNNumOfEntries: %d" );
    iSIMLog->Log((TDesC8)KFDNNumOfEntries, aPbInitInfo->iFDNNumOfEntries );

    _LIT8( KFDNTextLengthMax, "FDNTextLengthMax: %d" );
    iSIMLog->Log((TDesC8)KFDNTextLengthMax, aPbInitInfo->iFDNTextLengthMax );

    _LIT8( KFDNNumberLengthMax, "FDNNumberLengthMax: %d" );
    iSIMLog->Log((TDesC8)KFDNNumberLengthMax, aPbInitInfo->iFDNNumberLengthMax );

    _LIT8( KSDNNumOfEntries, "SDNNumOfEntries: %d" );
    iSIMLog->Log((TDesC8)KSDNNumOfEntries, aPbInitInfo->iSDNNumOfEntries );

    _LIT8( KSDNTextLengthMax, "SDNTextLengthMax: %d" );
    iSIMLog->Log((TDesC8)KSDNTextLengthMax, aPbInitInfo->iSDNTextLengthMax );

    _LIT8( KSDNNumberLengthMax, "SDNNumberLengthMax: %d" );
    iSIMLog->Log((TDesC8)KSDNNumberLengthMax, aPbInitInfo->iSDNNumberLengthMax );

    _LIT8( KVMBXNumOfEntries, "VMBXNumOfEntries: %d" );
    iSIMLog->Log((TDesC8)KVMBXNumOfEntries, aPbInitInfo->iVMBXNumOfEntries );

    _LIT8( KVMBXTextLengthMax, "VMBXTextLengthMax: %d" );
    iSIMLog->Log((TDesC8)KVMBXTextLengthMax, aPbInitInfo->iVMBXTextLengthMax );

    _LIT8( KVMBXNumberLengthMax, "VMBXNumberLengthMax: %d" );
    iSIMLog->Log((TDesC8)KVMBXNumberLengthMax, aPbInitInfo->iVMBXNumberLengthMax );

    _LIT8( KGRPNumOfEntriesPerEntry, "GRPNumOfEntriesPerEntry: %d" );
    iSIMLog->Log((TDesC8)KGRPNumOfEntriesPerEntry, aPbInitInfo->iGRPNumOfEntriesPerEntry );

    _LIT8( KANRNumOfEntries, "ANRNumOfEntries: %d" );
    iSIMLog->Log((TDesC8)KANRNumOfEntries, aPbInitInfo->iANRNumOfEntries );

    _LIT8( KANRNumOfEntriesPerEntry, "ANRNumOfEntriesPerEntry: %d" );
    iSIMLog->Log((TDesC8)KANRNumOfEntriesPerEntry, aPbInitInfo->iANRNumOfEntriesPerEntry );

    _LIT8( KANRNumLengthMax, "ANRNumLengthMax: %d" );
    iSIMLog->Log((TDesC8)KANRNumLengthMax, aPbInitInfo->iANRNumLengthMax );

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    _LIT8( KANRTextLengthMax, "ANRTextLengthMax (AAS): %d" );
    iSIMLog->Log((TDesC8)KANRTextLengthMax, aPbInitInfo->iANRTextLengthMax );

    _LIT8( KNumOfAasMax, "NumOfAasMax: %d" );
    iSIMLog->Log((TDesC8)KNumOfAasMax, aPbInitInfo->iNumOfAasMax );

    _LIT8( KGRPTextLengthMax, "GRPTextLengthMax: %d" );
    iSIMLog->Log((TDesC8)KGRPTextLengthMax, aPbInitInfo->iGRPTextLengthMax );

    _LIT8( KNumOfGasMax, "NumOfGasMax: %d" );
    iSIMLog->Log((TDesC8)KNumOfGasMax, aPbInitInfo->iNumOfGasMax );
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    }


// -----------------------------------------------------------------------------
// CSIM::PbStoreCacheL
// Phonebook-cache (ADN & FDN)
// -----------------------------------------------------------------------------
//
TInt CSIM::PbStoreCacheL( CStifItemParser& aItem )
    {
    _LIT8(KPbStoreCache, "SIM::PbStoreCache");
    iSIMLog->Log((TDesC8)KPbStoreCache);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString( string );
    CPhoneBookDataPackage package;

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &iPBStoreCacheAdn );
        }
    else if ( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;

        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &iPBStoreCacheFdn );
        }
    else if ( _L("Sdn") == string )
        {
        iSIMLog->Log((TDesC8)KSdnPhoneBook);
        iPhoneBookName = KETelIccSdnPhoneBook;

        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &iPBStoreCacheSdn );
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        ret = HandleRequestL( EMmTsyPhoneBookStoreCacheIPC,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "PbStoreCache request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "PbStoreCache request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::PbStoreCacheCancelL
// Phonebook-cacheing cancel (ADN, FDN, SDN)
// -----------------------------------------------------------------------------
//
TInt CSIM::PbStoreCacheCancelL( CStifItemParser& aItem )
    {
    _LIT8(KPbStoreCacheCancel, "SIM::PbStoreCacheCancel");
    iSIMLog->Log((TDesC8)KPbStoreCacheCancel);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString( string );
    CPhoneBookDataPackage package;

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &iPBStoreCacheAdn );
        // Reallocating ADN phonebook store array to simulate error case
        delete iPBStoreCacheAdn;
        iPBStoreCacheAdn = NULL;
        iPBStoreCacheAdn = new ( ELeave )
                           CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
        _LIT8(KAdnPBReallocation, "ADN PB entries array reallocated");
        iSIMLog->Log((TDesC8)KAdnPBReallocation);
        }
    else if ( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &iPBStoreCacheFdn );
        // Reallocating FDN phonebook store array to simulate error case
        delete iPBStoreCacheFdn;
        iPBStoreCacheFdn = NULL;
        iPBStoreCacheFdn = new ( ELeave )
                           CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
        _LIT8(KFdnPBReallocation, "FDN PB entries array reallocated");
        iSIMLog->Log((TDesC8)KFdnPBReallocation);
        }
    else if ( _L("Sdn") == string )
        {
        iSIMLog->Log((TDesC8)KSdnPhoneBook);
        iPhoneBookName = KETelIccSdnPhoneBook;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &iPBStoreCacheSdn );
        // Reallocating SDN phonebook store array to simulate error case
        delete iPBStoreCacheSdn;
        iPBStoreCacheSdn = NULL;
        iPBStoreCacheSdn = new ( ELeave )
                           CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
        _LIT8(KSdnPBReallocation, "SDN PB entries array reallocated");
        iSIMLog->Log((TDesC8)KSdnPBReallocation);
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        ret = HandleRequestL( EMmTsyPhoneBookStoreCacheCancelIPC,
                             &package );
        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "PbStoreCacheCancel request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "PbStoreCacheCancel request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompletePbStoreCache
// Complete PbStoreCache method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompletePbStoreCache
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompletePbStoreCache result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        const CPhoneBookDataPackage* phoneBookData =
            static_cast<const CPhoneBookDataPackage*>(aDataPackage);

        TName pbName;
        phoneBookData->GetPhoneBookName( pbName );

        // Later return value will be set as a value for aResult if
        // response data is checked for this case in function called below
        ( void ) ReadEntriesFromArray( EPBStoreCacheArray, pbName );

        // check phonebook name
        if( iHandleSpecifiedRequests )
            {
            if( iPhoneBookName != pbName )
                {
                aResult = KErrGeneral;
                }
            }

        // Check number of cached phonebook entries
        if ( -1 != iSavedPbEntriesCount )
            {
            aResult = CheckNumberOfPbEntries( EPBStoreCacheArray, pbName );
            }
        }
    else
        {
        _LIT8( KFail, "CompletePbStoreCache Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    // complete if failure
    if( KErrNone != aResult )
        {
        Signal( aResult );
        }
    // complete if test is not ongoing
    else if( ! iTestOngoing )
        {
        Signal( aResult );
        }
    iTestOngoing = EFalse;
    }

// -----------------------------------------------------------------------------
// CSIM::ReadEntriesL
// Read phone book entries
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadEntriesL( CStifItemParser& aItem )
    {
    _LIT8(KReadEntries, "SIM::ReadEntries");
    iSIMLog->Log((TDesC8)KReadEntries);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    TPtrC string2;
    TInt indexToRead;
    TInt numEntriesToRead;

    // 1. parameter = phonebook name
    aItem.GetNextString( string );
    // 2. parameter = index to be read
    aItem.GetNextInt( indexToRead );
    // 3. parameter = number of entries to be read
    aItem.GetNextInt( numEntriesToRead );

    // for checking the validity of response data

    // 4. parameter = name
    aItem.GetNextString( string2 );
    iSavedNameForComparison[0].Zero();
    iSavedNameForComparison[0].Append( string2 );

    // 5. parameter = number
    aItem.GetNextString( string2 );
    iSavedNumberForComparison[0].Zero();
    iSavedNumberForComparison[0].Append( string2 );

    // 6. parameter = email
    aItem.GetNextString( string2 );
    iSavedEmailForComparison[0].Zero();
    iSavedEmailForComparison[0].Append( string2 );

    // 7. parameter = SNE
    aItem.GetNextString( string2 );
    iSavedSneForComparison[0].Zero();
    iSavedSneForComparison[0].Append( string2 );

    // 8.parameter = ANR
    aItem.GetNextString( string2 );
    iSavedAnrForComparison[0].Zero();
    iSavedAnrForComparison[0].Append( string2 );

    // if there is second entry to be read, use index 1
    // 9. parameter = name
    aItem.GetNextString( string2 );
    iSavedNameForComparison[1].Zero();
    iSavedNameForComparison[1].Append( string2 );

    // 10. parameter = number
    aItem.GetNextString( string2 );
    iSavedNumberForComparison[1].Zero();
    iSavedNumberForComparison[1].Append( string2 );

    // 11. parameter = email
    aItem.GetNextString( string2 );
    iSavedEmailForComparison[1].Zero();
    iSavedEmailForComparison[1].Append( string2 );

    // 12. parameter = SNE
    aItem.GetNextString( string2 );
    iSavedSneForComparison[1].Zero();
    iSavedSneForComparison[1].Append( string2 );

    // 13.parameter = ANR
    aItem.GetNextString( string2 );
    iSavedAnrForComparison[1].Zero();
    iSavedAnrForComparison[1].Append( string2 );


    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if ( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else if ( _L("MBDN") == string )
        {
        iSIMLog->Log((TDesC8)KMBDN);
        iPhoneBookName = KETelIccMbdnPhoneBook;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        iIndexAndEntries->iIndex = indexToRead;
        iIndexAndEntries->iNumSlots = numEntriesToRead;

        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &iIndexAndEntries, &iPBStoreReadEntries );

        ret = HandleRequestL( EMmTsyPhoneBookStoreReadIPC,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "ReadEntries request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "ReadEntries request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteReadEntries
// Complete ReadEntries method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadEntries
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CompleteReadEntries result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        aResult = ReadEntriesFromArray( EPBStoreReadEntriesArray, iPhoneBookName );
        }
    else
        {
        _LIT8( KFail, "CompleteReadEntries Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::ReadEntriesFromArray
// Prints Pb entries
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadEntriesFromArray(
    TPBArrayTypeToRead aArrayType,
    TDesC16& aPbName )

    {
    TInt result = KErrNone;
    TInt num;

    _LIT8(KReadEntriesFromArray, "SIM::ReadEntriesFromArray");
    iSIMLog->Log((TDesC8)KReadEntriesFromArray);

    CArrayPtrSeg<CPhoneBookStoreEntry>* ptrToArray = NULL;

    if ( EPBStoreCacheArray == aArrayType )
        {
        if ( 0 == aPbName.CompareF( KETelIccAdnPhoneBook ) )
            {
            ptrToArray = iPBStoreCacheAdn;
            _LIT8(KADN, "ADN entries: ");
            iSIMLog->Log((TDesC8)KADN);
            }
        else if ( 0 == aPbName.CompareF( KETelIccFdnPhoneBook ) )
            {
            ptrToArray = iPBStoreCacheFdn;
            _LIT8(KFDN, "FDN entries: ");
            iSIMLog->Log((TDesC8)KFDN);
            }
        else if ( 0 == aPbName.CompareF( KETelIccSdnPhoneBook ) )
            {
            ptrToArray = iPBStoreCacheSdn;
            _LIT8(KSDN, "SDN entries: ");
            iSIMLog->Log((TDesC8)KSDN);
            }
        }
    else if ( EPBStoreReadEntriesArray == aArrayType )
        {
        ptrToArray = iPBStoreReadEntries;
        }

    for ( TInt locationIndex = 0; locationIndex < ptrToArray->Count();
          locationIndex++ )
        {
        CPhoneBookStoreEntry* ptr;
        ptr = ptrToArray->At( locationIndex );

        iSIMLog->Log((TDesC8)KLine);
        _LIT8( KLocation, "Location: %d" );
        iSIMLog->Log((TDesC8)KLocation, ptr->iLocation );

        // Name
        if ( ptr->iText && ( 0 < ptr->iText->Size() ) )
            {
            _LIT8( KName, "Name:" );
            iSIMLog->Log((TDesC8)KName);
            iSIMLog->Log( *(ptr->iText ));

            if ( ( 0 != iSavedNameForComparison[locationIndex].Compare ( _L ( "DONTCARE" ) ) ) &&
                 ( KErrNone == result ) )
                {
                if ( iSavedNameForComparison[locationIndex] != *(ptr->iText ) )
                    {
                    result = KErrGeneral;
                    }
                }
            }

        // Number
        if ( ptr->iNumber && ( 0 < ptr->iNumber->Size() ) )
            {
            _LIT8( KNumber, "Number:" );
            iSIMLog->Log((TDesC8)KNumber);
            iSIMLog->Log( *(ptr->iNumber ));

            if ( ( 0 != iSavedNumberForComparison[locationIndex].Compare ( _L ( "DONTCARE" ) ) ) &&
                 ( KErrNone == result ) )
                {
                if ( iSavedNumberForComparison[locationIndex] != *(ptr->iNumber ) )
                    {
                    result = KErrGeneral;
                    }
                }
            }

        // Email
        if (ptr->iEmail && (0 < ptr->iEmail->MdcaCount()))
            {
            for( num = 0; num < ptr->iEmail->MdcaCount(); num++)
                {
                _LIT8( KEmail, "Email:" );
                iSIMLog->Log((TDesC8)KEmail);
                iSIMLog->Log( (ptr->iEmail->MdcaPoint( num ) ));

                if ( ( 0 != iSavedEmailForComparison[locationIndex].Compare ( _L ( "DONTCARE" ) ) ) &&
                        ( KErrNone == result ) )
                    {
                    TBuf<100> tempBuffer( ptr->iEmail->MdcaPoint( num ) );
                    if ( iSavedEmailForComparison[locationIndex] != tempBuffer )
                        {
                        result = KErrGeneral;
                        }
                    }
                }
            }

        // Sne
        if (ptr->iSne && (0 < ptr->iSne->MdcaCount()))
            {
            for( num = 0; num < ptr->iSne->MdcaCount(); num++)
                {
                _LIT8( KSne, "Sne:" );
                iSIMLog->Log((TDesC8)KSne);
                iSIMLog->Log( (ptr->iSne->MdcaPoint( num ) ));

                if ( ( 0 != iSavedSneForComparison[locationIndex].Compare ( _L ( "DONTCARE" ) ) ) &&
                        ( KErrNone == result ) )
                    {
                    TBuf<100> tempBuffer( ptr->iSne->MdcaPoint( num ) );
                    if ( iSavedSneForComparison[locationIndex] != tempBuffer )
                        {
                        result = KErrGeneral;
                        }
                    }
                }
            }

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        if (ptr->iGrp && (0 < ptr->iGrp->MdcaCount()))
            {
            for(TInt num = 0; num < ptr->iGrp->MdcaCount(); num++)
                {
                _LIT8( KGrp, "Grp:" );
                iSIMLog->Log((TDesC8)KGrp);
                iSIMLog->Log( (ptr->iGrp->MdcaPoint( num ) ));
                }
            }
        if (ptr->iAnr && (0 < ptr->iAnr->Count()))
            {
            for(TInt num = 0; num < ptr->iAnr->Count(); num++)
                {
                TPhBkAdditionalEntry entry = ptr->iAnr->At(num);
                _LIT8( KAnrNum, "AnrNum:" );
                iSIMLog->Log((TDesC8)KAnrNum);
                iSIMLog->Log( entry.iTelNum );

                if ( entry.iAlphaTag.Length())
                    {
                    _LIT8( KAas, "Aas:" );
                    iSIMLog->Log((TDesC8)KAas);
                    iSIMLog->Log( entry.iAlphaTag );
                    }
                }
            }
#else

        // Anr
        if ( ptr->iAnr && (0 < ptr->iAnr->MdcaCount()))
            {
            for( num = 0; num < ptr->iAnr->MdcaCount(); num++)
                {
                _LIT8( KAnr, "Anr:" );
                iSIMLog->Log((TDesC8)KAnr);
                iSIMLog->Log( (ptr->iAnr->MdcaPoint( num ) ));
                if ( ( 0 != iSavedAnrForComparison[locationIndex].Compare ( _L ( "DONTCARE" ) ) ) &&
                        ( KErrNone == result ) )
                    {
                    TBuf<100> tempBuffer( ptr->iAnr->MdcaPoint( num )  );
                    if ( iSavedAnrForComparison[locationIndex] != tempBuffer )
                        {
                        result = KErrGeneral;
                        }
                    }
                }
             }
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        iSIMLog->Log((TDesC8)KLine);
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CSIM::WriteEntriesL
// Checks number of PB entries in the cache
// -----------------------------------------------------------------------------
//
TInt CSIM::CheckNumberOfPbEntries( TPBArrayTypeToRead aArrayType,
        TDesC16& aPbName )
    {
    TInt result( KErrNone );

    _LIT8(KCheckNumberOfPbEntries, "SIM::CheckNumberOfPbEntries");
    iSIMLog->Log((TDesC8)KCheckNumberOfPbEntries);

    CArrayPtrSeg<CPhoneBookStoreEntry>* ptrToArray = NULL;

    if ( EPBStoreCacheArray == aArrayType )
        {
        if ( 0 == aPbName.CompareF( KETelIccAdnPhoneBook ) )
            {
            ptrToArray = iPBStoreCacheAdn;
            _LIT8(KADN, "Checking number of ADN entries: ");
            iSIMLog->Log((TDesC8)KADN);
            }
        else if ( 0 == aPbName.CompareF( KETelIccFdnPhoneBook ) )
            {
            ptrToArray = iPBStoreCacheFdn;
            _LIT8(KFDN, "Checking number of FDN entries: ");
            iSIMLog->Log((TDesC8)KFDN);
            }
        else if ( 0 == aPbName.CompareF( KETelIccSdnPhoneBook ) )
            {
            ptrToArray = iPBStoreCacheSdn;
            _LIT8(KSDN, "Checking number of SDN entries: ");
            iSIMLog->Log((TDesC8)KSDN);
            }
        }
    else if ( EPBStoreReadEntriesArray == aArrayType )
        {
        ptrToArray = iPBStoreReadEntries;
        }

    if ( ptrToArray->Count() != iSavedPbEntriesCount )
        {
        _LIT8(KEntriesNumberDoesNotMatch, "PB entries number does not match: \
                exp=%d, act=%d");
        iSIMLog->Log((TDesC8)KEntriesNumberDoesNotMatch,
                iSavedPbEntriesCount, ptrToArray->Count());
        result = KErrGeneral;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CSIM::WriteEntriesL
// Writes a PB entry
// -----------------------------------------------------------------------------
//
TInt CSIM::WriteEntriesL( CStifItemParser& aItem )
    {
    _LIT8(KReadEntries, "SIM::WriteEntriesL");
    iSIMLog->Log((TDesC8)KReadEntries);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    TInt index;
    TPtrC16 name;
    TPtrC16 number;
    TPtrC16 email;
    TPtrC16 sne;
    TPtrC16 anr;
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    TPtrC16 aas;
    TPtrC16 grp;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

    // store given parameters for checking validity of completed data
    if ( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( index );
        iPbEntryInfo.iLocation = ( TUint16 )index;
        aItem.GetNextInt( index );
        iPbEntryInfo.iMaxNumLength = index;
        }

    aItem.GetNextString( string );
    aItem.GetNextInt( index );

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else if( _L("MBDN") == string )
        {
        iSIMLog->Log((TDesC8)KMBDN);
        iPhoneBookName = KETelIccMbdnPhoneBook;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        // NOTE: when this entry is created, it is always
        // put on the cache in succesful case.
        iPhoneBookEntry = new ( ELeave ) CPhoneBookStoreEntry();
        iPhoneBookEntry->ConstructL();

        if ( KErrNone == aItem.GetNextString( name ) )
            {
            iPhoneBookEntry->iText =
                HBufC::NewLC( name.Length() );
            iPhoneBookEntry->iText->Des() = name;
            CleanupStack::Pop( iPhoneBookEntry->iText );
            }
        if ( KErrNone == aItem.GetNextString( number ) )
            {
            iPhoneBookEntry->iNumber = HBufC::NewLC( number.Length() );
            iPhoneBookEntry->iNumber->Des() = number;
            CleanupStack::Pop( iPhoneBookEntry->iNumber );
            }
        if ( KErrNone == aItem.GetNextString( email ) )
            {
            iPhoneBookEntry->iEmail->AppendL( email );
            }
        if ( KErrNone == aItem.GetNextString( sne ) )
            {
            iPhoneBookEntry->iSne->AppendL( sne );
            }
        if ( KErrNone == aItem.GetNextString( anr ) )
            {
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            TPhBkAdditionalEntry entry;
            entry.iTelNum.Copy(anr);
            if( KErrNone == aItem.GetNextString( aas ) )
                {
                entry.iAlphaTag.Copy(aas);
                }
            iPhoneBookEntry->iAnr->AppendL( entry );
#else // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            iPhoneBookEntry->iAnr->AppendL( anr );
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            }
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        if ( KErrNone == aItem.GetNextString( grp ) )
            {
            iPhoneBookEntry->iGrp->AppendL( grp );
            }
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &index, &iPhoneBookEntry );

        ret = HandleRequestL( EMmTsyPhoneBookStoreWriteIPC,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "WriteEntries request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "WriteEntries request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );

            // Delete phonebook write entry
            if ( iPhoneBookEntry )
                {
                delete iPhoneBookEntry;
                iPhoneBookEntry = NULL;
                }
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteWriteEntries
// Complete PbInit method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteWriteEntries
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteWriteEntries result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TPBEntryInfo entryInfo;
        //unpack data
        aDataPackage->UnPackData( entryInfo );

        // check data validity
        if( iHandleSpecifiedRequests )
            {
            if( entryInfo.iLocation != iPbEntryInfo.iLocation ||
                 entryInfo.iMaxNumLength != iPbEntryInfo.iMaxNumLength )
                {
                aResult = KErrArgument;
                }
            }

        _LIT8( KEntryInfo, "EntryInfo: MaxNumLength = %d, Location = %d" );
        iSIMLog->Log((TDesC8)KEntryInfo, entryInfo.iMaxNumLength, entryInfo.iLocation );
        }
    else
        {
        _LIT8( KFail, "CompleteWriteEntries Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::DeleteEntryL
// Deletes antry from sim phone book
// -----------------------------------------------------------------------------
//
TInt CSIM::DeleteEntryL( CStifItemParser& aItem )
    {
    _LIT8(KDeleteEntry, "SIM::DeleteEntry");
    iSIMLog->Log((TDesC8)KDeleteEntry);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    TInt index;

    // store given parameters for checking validity of completed data
    if( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( index );
        iPbEntryInfo.iLocation = ( TUint16 )index;
        aItem.GetNextInt( index );
        iPbEntryInfo.iMaxNumLength = index;
        }

    aItem.GetNextString ( string );
    aItem.GetNextInt( index );

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else if( _L("MBDN") == string )
        {
        iSIMLog->Log((TDesC8)KMBDN);
        iPhoneBookName = KETelIccMbdnPhoneBook;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &index );

        ret = HandleRequestL( EMmTsyPhoneBookStoreDeleteIPC,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "DeleteEntry request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "DeleteEntry request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );

            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteDelete
// Complete DeleteEntry method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteDelete
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteDelete result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TPBEntryInfo entryInfo;
        //unpack data
        aDataPackage->UnPackData( entryInfo );

        // check data validity
        if( iHandleSpecifiedRequests )
            {
            if( entryInfo.iLocation != iPbEntryInfo.iLocation ||
                 entryInfo.iMaxNumLength != iPbEntryInfo.iMaxNumLength )
                {
                aResult = KErrGeneral;
                }
            }

        _LIT8( KEntryInfo, "EntryInfo: MaxNumLength = %d" );
        iSIMLog->Log((TDesC8)KEntryInfo, entryInfo.iMaxNumLength  );
        }
    else
        {
        _LIT8( KFail, "CompleteDelete Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::DeleteAllEntriesL
// Deletes all entries from sim phone book
// -----------------------------------------------------------------------------
//
TInt CSIM::DeleteAllEntriesL( CStifItemParser& aItem )
    {
    _LIT8(KDeleteEntry, "SIM::DeleteAllEntries");
    iSIMLog->Log((TDesC8)KDeleteEntry);

    TInt ret( KErrNotFound );
    TInt index;
    TBool supported( ETrue );
    TPtrC string;

    // store given parameters for checking validity of completed data
    if ( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( index );
        iPbEntryInfo.iLocation = ( TUint16 )index;
        aItem.GetNextInt( index );
        iPbEntryInfo.iMaxNumLength = index;
        }

    aItem.GetNextString ( string );

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );

        ret = HandleRequestL( EMmTsyPhoneBookStoreDeleteAllIPC,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "DeleteAllEntries request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "DeleteAllEntries request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );

            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::GetPbStoreInfoL
// Get phone book store info
// -----------------------------------------------------------------------------
//
TInt CSIM::GetPbStoreInfoL( CStifItemParser& aItem )
    {
    _LIT8(KDeleteEntry, "SIM::GetPbStoreInfo");
    iSIMLog->Log((TDesC8)KDeleteEntry);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString ( string );

    // for checking the validity of response data
    if( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( iNumOfUsedPbEntries );
        }


    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else if ( _L("VoiceMailBox") == string )
        {
        iSIMLog->Log((TDesC8)KVoiceMailBox);
        iPhoneBookName = KETelIccVoiceMailBox;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );

        ret = HandleRequestL( EMmTsyPhoneBookStoreGetInfoIPC,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "GetPbStoreInfo request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "GetPbStoreInfo request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );

            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }
// -----------------------------------------------------------------------------
// CSIM::CompleteGetPbStoreInfo
// Complete GetPbStoreInfo method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetPbStoreInfo
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetPbStoreInfo result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TInt numberOfusedPbEntries;
        //unpack data
        aDataPackage->UnPackData( numberOfusedPbEntries );

        if( iHandleSpecifiedRequests )
            {
            if( iNumOfUsedPbEntries != numberOfusedPbEntries )
                {
                aResult = KErrGeneral;
                }
            }

        _LIT8( KPbStoreInfo, "PbStoreInfo: numberOfusedPbEntries = %d" );
        iSIMLog->Log((TDesC8)KPbStoreInfo, numberOfusedPbEntries  );
        }
    else
        {
        _LIT8( KFail, "CompleteGetPbStoreInfo Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::ReadENStore
// Read emergency number Store Info
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadENStore( CStifItemParser& aItem )
    {
    _LIT8(KReadENStore, "SIM::ReadENStore");
    iSIMLog->Log((TDesC8)KReadENStore);

    TInt ret( KErrNotFound );
    aItem.GetNextInt( iIndex );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        TPtrC string;
        aItem.GetNextString( string );
        iNumber.Append( string );
        }
    //Create package
    CPhoneBookDataPackage package;
    package.PackData( &iIndex );

    ret = HandleRequestL( EMmTsyENStoreReadIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ReadENStore request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "ReadENStore request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }
// -----------------------------------------------------------------------------
// CSIM::CompleteReadENStore
// Complete ReadENStore method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadENStore
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteReadENStore result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TENStoreResponse enStoreResp;
        //unpack data
        aDataPackage->UnPackData( enStoreResp );

        iSIMLog->Log((TDesC8)KLine );
        _LIT8( KPbStoreInfo, "ENStore Location: %d " );
        iSIMLog->Log((TDesC8)KPbStoreInfo, enStoreResp.iLocation );

        _LIT8( KNumber, "ENStore Number:" );
        iSIMLog->Log((TDesC8)KNumber);
        iSIMLog->Log( enStoreResp.iECCNumber);
        iSIMLog->Log((TDesC8)KLine );

        // check data
        if( iHandleSpecifiedRequests )
            {
            if( iIndex != enStoreResp.iLocation ||
                iNumber != enStoreResp.iECCNumber )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteReadENStore Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::ReadENStoreAll
// Read emergency number Stotre Info
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadENStoreAll( CStifItemParser& aItem )
    {
    _LIT8(KReadENStore, "SIM::ReadENStore");
    iSIMLog->Log((TDesC8)KReadENStore);

    TInt ret( KErrNotFound );
    TInt index;
    aItem.GetNextInt( index );

    //Create package
    CPhoneBookDataPackage package;
    package.PackData( &index );

    // store given parameters for validity checking
    if( iHandleSpecifiedRequests )
        {
        for ( TInt i = 0; i < 3; i++)
            {
            TPtrC string;
            aItem.GetNextInt( iIndexTable[i] );
            aItem.GetNextString( string );
            iNumberTable[i].Append( string );
            }
        }

    ret = HandleRequestL( EMmTsyENStoreReadAllPhase1IPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ReadENStoreAll request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "ReadENStoreAll request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }
// -----------------------------------------------------------------------------
// CSIM::CompleteReadENStoreAll
// Complete ReadENStore method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadENStoreAll
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteReadENStoreAll result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        //unpack data
        aDataPackage->UnPackData( iENStoreArray );

        iSIMLog->Log((TDesC8)KLine );

        // check data validity
        if( iHandleSpecifiedRequests )
            {
            for ( TInt i = 0; i < 3; i++)
                {
                TENStoreResponse* enStoreResp = iENStoreArray->At( i );
                if ( enStoreResp->iLocation != iIndexTable[i] ||
                     enStoreResp->iECCNumber != iNumberTable[i] )
                    {
                    aResult = KErrGeneral;
                    break;
                    }
                }
            }

        // Loop until TSY's internal ENStorage is empty
        while ( !( 0 == iENStoreArray->Count() ) )
            {
            TENStoreResponse* enStoreResp = iENStoreArray->At( 0 );

            _LIT8( KPbStoreInfo, "ENStore Location: %d " );
            iSIMLog->Log((TDesC8)KPbStoreInfo, enStoreResp->iLocation );

            _LIT8( KNumber, "ENStore Number:" );
            iSIMLog->Log((TDesC8)KNumber);
            iSIMLog->Log( enStoreResp->iECCNumber);

            // Delete internal list member
            delete enStoreResp;
            iENStoreArray->Delete( 0 );
            iENStoreArray->Compress();
            }


        iSIMLog->Log((TDesC8)KLine );
        }
    else
        {
        _LIT8( KFail, "CompleteReadENStoreAll Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetENStoreInfo
// Get emergency number Store Info
// -----------------------------------------------------------------------------
//
TInt CSIM::GetENStoreInfo( CStifItemParser& aItem )
    {
    _LIT8(KGetENStoreInfo, "SIM::GetENStoreInfo");
    iSIMLog->Log((TDesC8)KGetENStoreInfo);

    TInt ret( KErrNotFound );
    TInt index;
    aItem.GetNextInt( index );

    //Create package
    CPhoneBookDataPackage package;
    package.PackData( &index );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( iNumOfUsedPbEntries );
        }

    ret = HandleRequestL( EMmTsyENStoreGetInfoIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetENStoreInfo request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetENStoreInfo request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetENStoreInfo
// Complete GetENStoreInfo method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetENStoreInfo
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetENStoreInfo result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TInt numberOfusedENEntries;
        //unpack data
        aDataPackage->UnPackData( numberOfusedENEntries );

        // validity checking
        if( iHandleSpecifiedRequests )
            {
            if ( iNumOfUsedPbEntries != numberOfusedENEntries )
                {
                aResult = KErrGeneral;
                }
            }

        _LIT8( KENStoreInfo, "ENStoreInfo: numberOfusedENEntries = %d" );
        iSIMLog->Log((TDesC8)KENStoreInfo, numberOfusedENEntries  );
        }
    else
        {
        _LIT8( KFail, "CompleteGetENStoreInfo Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::ReadONStore
// Read own number Store Info
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadONStore( CStifItemParser& aItem )
    {
    _LIT8(KReadONStore, "SIM::ReadONStore");
    iSIMLog->Log((TDesC8)KReadONStore);

    TInt ret( KErrNotFound );
    aItem.GetNextInt( iIndex );

    //Create package
    CPhoneBookDataPackage package;
    package.PackData( &iIndex );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        TPtrC string;
        aItem.GetNextString( string );
        iName.Append( string );
        aItem.GetNextString( string );
        iNumber.Append( string );
        }

    ret = HandleRequestL( EMmTsyONStoreReadIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ReadONStore request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "ReadONStore request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteReadONStore
// Complete ReadONStore method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadONStore
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteReadONStore result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TONStoreMsg* onStoreMsg;
        //unpack data
        aDataPackage->UnPackData( onStoreMsg );

        iSIMLog->Log((TDesC8)KLine );
        _LIT8( KONStoreInfo, "ONStore Location: %d " );
        iSIMLog->Log((TDesC8)KONStoreInfo, onStoreMsg->iLocation );

        _LIT8( KName, "ONStore Name:" );
        iSIMLog->Log((TDesC8)KName);
        iSIMLog->Log( onStoreMsg->iName);

        _LIT8( KNumber, "ONStore Number:" );
        iSIMLog->Log((TDesC8)KNumber);
        iSIMLog->Log( onStoreMsg->iTelNumber );
        iSIMLog->Log((TDesC8)KLine );

        // validity checking
        if( iHandleSpecifiedRequests )
            {
            if ( iIndex != onStoreMsg->iLocation ||
                 iName != onStoreMsg->iName ||
                 iNumber != onStoreMsg->iTelNumber )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteReadONStore Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::ReadONStoreEntry
// Read own number Store Info
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadONStoreEntry( CStifItemParser& aItem )
    {
    _LIT8(KReadONStore, "SIM::ReadONStoreEntry");
    iSIMLog->Log((TDesC8)KReadONStore);

    TInt ret( KErrNotFound );
    aItem.GetNextInt( iIndex );

    //Create package
    CPhoneBookDataPackage package;
    package.PackData( &iIndex );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        TPtrC string;
        aItem.GetNextString( string );
        iName.Append( string );
        aItem.GetNextString( string );
        iNumber.Append( string );
        }

    ret = HandleRequestL( EMmTsyONStoreReadEntryIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ReadONStoreEntry request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "ReadONStoreEntry request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteReadONStoreEntry
// Complete ReadONStoreEntry method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadONStoreEntry
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteReadONStoreEntry result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TONStoreMsg onStoreMsg;
        //unpack data
        aDataPackage->UnPackData( onStoreMsg );

        iSIMLog->Log((TDesC8)KLine );
        _LIT8( KONStoreInfo, "ONStoreEntry Location: %d " );
        iSIMLog->Log((TDesC8)KONStoreInfo, onStoreMsg.iLocation );

        _LIT8( KName, "ONStoreEntry Name:" );
        iSIMLog->Log((TDesC8)KName);
        iSIMLog->Log( onStoreMsg.iName);

        _LIT8( KNumber, "ONStoreEntry Number:" );
        iSIMLog->Log((TDesC8)KNumber);
        iSIMLog->Log( onStoreMsg.iTelNumber );
        iSIMLog->Log((TDesC8)KLine );

        // validity checking
        if( iHandleSpecifiedRequests )
            {
            if ( iIndex != onStoreMsg.iLocation ||
                 iName != onStoreMsg.iName ||
                 iNumber != onStoreMsg.iTelNumber )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteReadONStoreEntry Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetONStoreInfo
// Get own number Store Info
// -----------------------------------------------------------------------------
//
TInt CSIM::GetONStoreInfo( CStifItemParser& aItem )
    {
    _LIT8(KGetENStoreInfo, "SIM::GetONStoreInfo");
    iSIMLog->Log((TDesC8)KGetENStoreInfo);

    TInt ret( KErrNotFound );
    TInt index;
    aItem.GetNextInt( index );

    //Create package
    CPhoneBookDataPackage package;
    package.PackData( &index );

    // store given parameters for validity checking
    if( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( iServiceType.iNumOfEntries );
        aItem.GetNextInt( iServiceType.iUsedEntries );
        aItem.GetNextInt( iServiceType.iNameLen );
        aItem.GetNextInt( iServiceType.iNumLen );
        }

    ret = HandleRequestL( EMmTsyONStoreGetInfoIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetONStoreInfo request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetONStoreInfo request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetONStoreInfo
// Complete GetONStoreInfo method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetONStoreInfo
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetONStoreInfo result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        // Response information
        TServiceType serviceType;
        //unpack data
        aDataPackage->UnPackData( serviceType );

        iSIMLog->Log((TDesC8)KLine );
        _LIT8( KONStoreInfo, "ONStoreInfo:" );
        iSIMLog->Log((TDesC8)KONStoreInfo );
        _LIT8( KNumOfEntries, "Number of entries = %d" );
        iSIMLog->Log((TDesC8)KNumOfEntries, serviceType.iNumOfEntries  );
        _LIT8( KUsedEntries, "Used entries = %d" );
        iSIMLog->Log((TDesC8)KUsedEntries, serviceType.iUsedEntries  );
        _LIT8( KNameLen, "Name Length = %d" );
        iSIMLog->Log((TDesC8)KNameLen, serviceType.iNameLen  );
        _LIT8( KNumLen, "Num Length = %d" );
        iSIMLog->Log((TDesC8)KNumLen, serviceType.iNumLen  );
        iSIMLog->Log((TDesC8)KLine );

        // validity checking
        if( iHandleSpecifiedRequests )
            {
            if( iServiceType.iNumOfEntries != serviceType.iNumOfEntries ||
                iServiceType.iUsedEntries != serviceType.iUsedEntries ||
                iServiceType.iNameLen != serviceType.iNameLen ||
                iServiceType.iNumLen != serviceType.iNumLen )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetONStoreInfo Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::DeleteONStoreEntry
// Delete own number Store Entry
// -----------------------------------------------------------------------------
//
TInt CSIM::DeleteONStoreEntry( CStifItemParser& aItem )
    {
    _LIT8(KDeleteONStoreEntry, "SIM::DeleteONStoreEntry");
    iSIMLog->Log((TDesC8)KDeleteONStoreEntry);

    TInt ret( KErrNotFound );
    TInt index;
    aItem.GetNextInt( index );

    //Create package
    CPhoneBookDataPackage package;
    package.PackData( &index );

    ret = HandleRequestL( EMmTsyONStoreDeleteIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "DeleteONStoreEntry request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "DeleteONStoreEntry request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetONStoreInfo
// Complete GetONStoreInfo method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteDeleteONStoreEntry
        (
        TInt aResult,
        CMmDataPackage* /*aDataPackage*/
        )
    {
    _LIT8( KResult, "CompleteDeleteONStoreEntry result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetONStoreInfo Failed" );
        iSIMLog->Log((TDesC8)KFail  );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::DeleteAllONStoreEntries
// Delete own number Store Entry
// -----------------------------------------------------------------------------
//
TInt CSIM::DeleteAllONStoreEntries( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KDeleteONStoreEntry, "SIM::DeleteAllONStoreEntries");
    iSIMLog->Log((TDesC8)KDeleteONStoreEntry);

    TInt ret( KErrNotFound );

    //Create package
    CPhoneBookDataPackage package;

    ret = HandleRequestL( EMmTsyONStoreDeleteAllIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "DeleteAllONStoreEntries request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "DeleteAllONStoreEntries request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::WriteONStore
// Write entry to own number store
// -----------------------------------------------------------------------------
//
TInt CSIM::WriteONStore( CStifItemParser& aItem )
    {
    _LIT8(KWriteONStore, "SIM::WriteONStore");
    iSIMLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );
    TInt mode;
    TInt service;
    TInt type;
    TInt numberPlan;
    TPtrC16 number;
    TPtrC16 text;

    RMobileONStore::TMobileONEntryV1 entry;
    RMobileONStore::TMobileONEntryV1Pckg entryPckg( entry );

    if( KErrNone == aItem.GetNextInt( iIndex ) )
        {
        entry.iIndex = iIndex;
        }
    if( KErrNone == aItem.GetNextInt( mode ) )
        {
        entry.iMode = static_cast<RMobilePhone::TMobilePhoneNetworkMode>(mode);
        }
    if( KErrNone == aItem.GetNextInt( service ))
        {
        entry.iService = static_cast<RMobilePhone::TMobileService>(service);
        }
    if( KErrNone == aItem.GetNextInt( type ) )
        {
        entry.iNumber.iTypeOfNumber = static_cast<RMobilePhone::TMobileTON>(type);
        }
    if( KErrNone == aItem.GetNextInt( numberPlan ) )
        {
        entry.iNumber.iNumberPlan =
                        static_cast<RMobilePhone::TMobileNPI>(numberPlan);
        }
    if( KErrNone == aItem.GetNextString( number ) )
        {
        entry.iNumber.iTelNumber.Copy(number);
        }
     if( KErrNone == aItem.GetNextString( text ) )
        {
        entry.iText.Copy(text);
        }

    //Create package
    CPhoneBookDataPackage package;
    RMobileONStore::TMobileONEntryV1Pckg* entryPckgPtr = &entryPckg;
    package.PackData( &entryPckgPtr );

    ret = HandleRequestL( EMmTsyONStoreWriteIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "WriteONStore request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "WriteONStore request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteWriteONStore
// Complete WriteONStore method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteWriteONStore
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteWriteONStore result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
         TInt location = 0;

         if ( aDataPackage )
            {
            aDataPackage->UnPackData( location );
            _LIT8( KLocation, "Location: %d" );
            iSIMLog->Log((TDesC8)KLocation, location );
            // validity checking
            if( iHandleSpecifiedRequests )
                {
                if ( iIndex != location )
                    {
                    aResult = KErrArgument;
                    }
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteWriteONStore Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::ReadONStoreSize
// Read size of on store
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadONStoreSize( CStifItemParser& aItem )
    {
    _LIT8(KReadONStoreSize, "SIM::ReadONStoreSize");
    iSIMLog->Log((TDesC8)KReadONStoreSize);

    TInt ret( KErrNotFound );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( iNumOfUsedPbEntries );
        }

    ret = HandleRequestL( EMmTsyONStoreReadSizeIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ReadONStoreSize request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "ReadONStoreSize request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::ViagHomeZoneReadParam
// Read ViagHomeZone param
// {smsc, scp, hzver, {x, y, r2, id, status, tag}[4]}
// -----------------------------------------------------------------------------
//
TInt CSIM::ViagHomeZoneReadParamL( CStifItemParser& aItem )
    {
    _LIT8( KViagHomeZoneReadParam, "CSIM::ViagHomeZoneReadParam" );
    iSIMLog->Log((TDesC8)KViagHomeZoneReadParam);

    // filling parameters
    // smsc
    TPtrC ptrSmsC;
    User::LeaveIfError(
        aItem.GetNextString( ptrSmsC ) );
    iSavedViagParamsForComparison.iSmsC = ptrSmsC;

    // scp
    TPtrC ptrScp;
    User::LeaveIfError(
        aItem.GetNextString( ptrScp ) );
    iSavedViagParamsForComparison.iScp = ptrScp;

    // subscribed zone and version
    TUint zoneAndVersion( 0 );
    User::LeaveIfError(
        aItem.GetNextInt( zoneAndVersion, EHex ) );
    iSavedViagParamsForComparison.iSubscribedZoneAndVersion = zoneAndVersion;

    // creating elements
    iSavedViagElementsForComparison =
        new(ELeave) RMmCustomAPI::TViagElements(
            RMmCustomAPI::KViagElementCount );

    // filling elements
    TPtrC homezoneFlag;
    for ( TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++ )
        {
        RMmCustomAPI::TViagElement element;

        // fetching X coordinate
        TUint xCoord( 0 );
        User::LeaveIfError(
            aItem.GetNextInt( xCoord, EHex ) );
        element.iCoordinates.iX = xCoord;
        // fetching Y coordinate
        TUint yCoord( 0 );
        User::LeaveIfError(
            aItem.GetNextInt( yCoord, EHex ) );
        element.iCoordinates.iY = yCoord;
        // fetching R2
        TUint r2;
        User::LeaveIfError(
            aItem.GetNextInt( r2, EHex ) );
        element.iCoordinates.iR2 = r2;

        // fetching zone id
        TUint zoneId( 0 );
        User::LeaveIfError(
            aItem.GetNextInt( zoneId, EHex ) );
        element.iCoordinates.iZoneId = zoneId;

        // fetching homezone status flag
        User::LeaveIfError( aItem.GetNextString( homezoneFlag ) );
        if ( 0 == homezoneFlag.Compare( KViagInactive ) )
            {
            element.iActiveFlag = RMmCustomAPI::ENotActive;
            }
        else if ( 0 == homezoneFlag.Compare( KViagHZActive ) )
            {
            element.iActiveFlag = RMmCustomAPI::EHomeZone;
            }
        else if ( 0 == homezoneFlag.Compare( KViagCZActive ) )
            {
            element.iActiveFlag = RMmCustomAPI::ECityZone;
            }
        else
            {
            User::Leave( KErrArgument );
            }

        // fetching tag
        TPtrC name;
        User::LeaveIfError(
            aItem.GetNextString( name ) );
        element.iName = name;

        iSavedViagElementsForComparison->AppendL( element );
        }

    TInt ret( HandleRequestL( EReadViagHomeZoneParamsIPC ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EReadViagHomeZoneParamsIPC request send ok" );
        iSIMLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "EReadViagHomeZoneParamsIPC request send failed: %d" );
        iSIMLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::ViagHomeZoneReadCache
// Read ViagHomeZone cache
// {cacheid, recnum, lac, cellid}
// -----------------------------------------------------------------------------
//
TInt CSIM::ViagHomeZoneReadCacheL( CStifItemParser& aItem )
    {
    _LIT8(KViagHomeZoneReadCache, "CSIM::ViagHomeZoneReadCache");
    iSIMLog->Log((TDesC8)KViagHomeZoneReadCache);

    RMmCustomAPI::TViagCacheRecordId record;

    // fetching cacheid for request
    TUint cacheid;
    User::LeaveIfError(
        aItem.GetNextInt( cacheid, EHex ) );
    record.iCacheId = cacheid;
    // fetching recnum for request
    TUint recnum;
    User::LeaveIfError(
        aItem.GetNextInt( recnum, EHex ) );
    // XML data and CFG data should match
    record.iRecordId = recnum - 1;


    // fetching LAC for response
    TUint lac( 0 );
    User::LeaveIfError(
        aItem.GetNextInt( lac, EHex ) );
    iSavedRecordForComparison.iLac = lac;
    // fetching CellId for response
    TUint cellId( 0 );
    User::LeaveIfError(
        aItem.GetNextInt( cellId, EHex ) );
    iSavedRecordForComparison.iCellId = cellId;

    CMmDataPackage package;
    package.PackData( &record );
    TInt ret( HandleRequestL( EReadViagHomeZoneCacheIPC, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EReadViagHomeZoneCacheIPC request send ok" );
        iSIMLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "EReadViagHomeZoneCacheIPC request send failed: %d" );
        iSIMLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::ViagHomeZoneWriteSettings
// Write ViagHomeZone settings
// {settings, version}
// -----------------------------------------------------------------------------
//
TInt CSIM::ViagHomeZoneWriteSettingsL( CStifItemParser& aItem )
    {
    _LIT8( KViagHomeZoneWriteSettings, "CSIM::ViagHomeZoneWriteSettings" );
    iSIMLog->Log((TDesC8)KViagHomeZoneWriteSettings);

    CMmDataPackage package;
    RMmCustomAPI::TViagUHZIUESettings settings;

    // fetching settings
    TUint set( 0 );
    User::LeaveIfError( aItem.GetNextInt( set, EHex ) );
    settings.iSettings = set;

    // fetching version
    TUint ver( 0 );
    User::LeaveIfError( aItem.GetNextInt( ver, EHex ) );
    settings.iVersion = ver;

    package.PackData( &settings );
    TInt ret( HandleRequestL( EWriteViagHomeZoneUHZIUESettingsIPC, &package ) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EWriteViagHomeZoneUHZIUESettingsIPC request send ok" );
        iSIMLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "EWriteViagHomeZoneUHZIUESettingsIPC request send failed: %d" );
        iSIMLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::ViagHomeZoneWriteCache
// Write ViagHomeZone cache
// {cacheid, recnum, lac, cellid}
// -----------------------------------------------------------------------------
//
TInt CSIM::ViagHomeZoneWriteCacheL( CStifItemParser& aItem )
    {
    _LIT8( KViagHomeZoneWriteCache, "CSIM::ViagHomeZoneWriteCache" );
    iSIMLog->Log( (TDesC8)KViagHomeZoneWriteCache );

    CMmDataPackage package;
    RMmCustomAPI::TViagCacheRecordId recordId;
    RMmCustomAPI::TViagCacheRecordContent recordContent;

    // fetching cacheId
    TUint cacheid( 0 );
    User::LeaveIfError( aItem.GetNextInt( cacheid, EHex ) );
    recordId.iCacheId = cacheid;

    // fetching record number
    TUint recnum( 0 );
    User::LeaveIfError( aItem.GetNextInt( recnum, EHex ) );
    // XML and CFG should match
    recordId.iRecordId = recnum - 1;

    // fetching lac
    TUint lac( 0 );
    User::LeaveIfError( aItem.GetNextInt( lac, EHex ) );
    recordContent.iLac = lac;

    // fetching CellId
    TUint cellid( 0 );
    User::LeaveIfError( aItem.GetNextInt( cellid, EHex ) );
    recordContent.iCellId = cellid;

    package.PackData( &recordId, &recordContent );
    TInt ret( HandleRequestL( EWriteViagHomeZoneCacheIPC, &package) );

    if ( KErrNone == ret )
        {
        _LIT8( KRequestOk, "EWriteViagHomeZoneCacheIPC request send ok" );
        iSIMLog->Log( (TDesC8)KRequestOk );
        }
    else
        {
         _LIT8( KRequestFailed, "EWriteViagHomeZoneCacheIPC request send failed: %d" );
        iSIMLog->Log( (TDesC8)KRequestFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteReadONStoreSize
// Complete ReadONStoreSize method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadONStoreSize
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteReadONStoreSize result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        TInt size = 0;
        if ( aDataPackage )
            {
            aDataPackage->UnPackData( size );
            _LIT8( KSize, "Size: %d" );
            iSIMLog->Log((TDesC8)KSize, size );
            // validity checking
            if( iHandleSpecifiedRequests )
                {
                if ( iNumOfUsedPbEntries != size )
                    {
                    aResult = KErrGeneral;
                    }
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteReadONStoreSize Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::OnStoreWriteSize
// Write size of on store
// -----------------------------------------------------------------------------
//
TInt CSIM::OnStoreWriteSize( CStifItemParser& aItem )
    {
    _LIT8(KReadONStoreWriteSize, "SIM::OnStoreWriteSize");
    iSIMLog->Log((TDesC8)KReadONStoreWriteSize);

    TInt ret( KErrNotFound );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( iNumOfUsedPbEntries );
        }

    ret = HandleRequestL( EMmTsyONStoreWriteSizeIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "OnStoreWriteSize request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "OnStoreWriteSize request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteOnStoreWriteSize
// Complete OnStoreWriteSize method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteOnStoreWriteSize
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteOnStoreWriteSize result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        TInt size = 0;
        if ( aDataPackage )
            {
            aDataPackage->UnPackData( size );
            _LIT8( KSize, "Size: %d" );
            iSIMLog->Log((TDesC8)KSize, size );
            // validity checking
            if( iHandleSpecifiedRequests )
                {
                if ( iNumOfUsedPbEntries != size )
                    {
                    aResult = KErrGeneral;
                    }
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteOnStoreWriteSize Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetMailboxNumbers
// Get mailbox numbers
// -----------------------------------------------------------------------------
//
TInt CSIM::GetMailboxNumbers( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KGetMailboxNumbers, "SIM::GetMailboxNumbers");
    iSIMLog->Log((TDesC8)KGetMailboxNumbers);

    TInt ret( KErrNotFound );
    ret = HandleRequestL( EMobilePhoneGetMailboxNumbers );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetMailboxNumbers request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetMailboxNumbers request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetMailboxNumbers
// Complete GetMailboxNumbers method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetMailboxNumbers
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetMailboxNumbers result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetMailboxNumbers Failed" );
        iSIMLog->Log((TDesC8)KFail  );
        }
    else
        {
        RMobilePhone::TMobilePhoneVoicemailIdsV3* voicemailIds;
        aDataPackage->UnPackData( &voicemailIds );

        _LIT8( KVoice, "VoicemailIds: Voice = %d" );
        iSIMLog->Log((TDesC8)KVoice, voicemailIds->iVoice  );
        _LIT8( KFax, "VoicemailIds: Fax = %d" );
        iSIMLog->Log((TDesC8)KFax, voicemailIds->iFax  );
            _LIT8( KData, "VoicemailIds: Data = %d" );
        iSIMLog->Log((TDesC8)KData, voicemailIds->iData  );
        _LIT8( KOther, "VoicemailIds: Other = %d" );
        iSIMLog->Log((TDesC8)KOther, voicemailIds->iOther  );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::WriteONStoreAll
// Write entry to own number store
// -----------------------------------------------------------------------------
//
TInt CSIM::WriteONStoreAll( CStifItemParser& aItem )
    {
    _LIT8(KWriteONStore, "SIM::WriteONStoreAll");
    iSIMLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );
    TInt mode;
    TInt service;
    TInt type;
    TInt numberPlan;
    TPtrC16 number;
    TPtrC16 text;

    RMobileONStore::TMobileONEntryV1 entry;

    if( KErrNone == aItem.GetNextInt( iIndex ) )
        {
        entry.iIndex = iIndex;
        }
    if( KErrNone == aItem.GetNextInt( mode ) )
        {
        entry.iMode = static_cast<RMobilePhone::TMobilePhoneNetworkMode>(mode);
        }
    if( KErrNone == aItem.GetNextInt( service ))
        {
        entry.iService = static_cast<RMobilePhone::TMobileService>(service);
        }
    if( KErrNone == aItem.GetNextInt( type ) )
        {
        entry.iNumber.iTypeOfNumber = static_cast<RMobilePhone::TMobileTON>(type);
        }
    if( KErrNone == aItem.GetNextInt( numberPlan ) )
        {
        entry.iNumber.iNumberPlan =
                static_cast<RMobilePhone::TMobileNPI>(numberPlan);
        }
    if( KErrNone == aItem.GetNextString( number ) )
        {
        entry.iNumber.iTelNumber.Copy(number);
        }
     if( KErrNone == aItem.GetNextString( text ) )
        {
        entry.iText.Copy(text);
        }

    //Create package
    CMmDataPackage package;
    package.PackData( &entry );

    ret = HandleRequestL( EMmTsyONStoreWriteEntryIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "WriteONStoreAll request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "WriteONStoreAll request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteWriteONStoreAll
// Complete WriteONStore method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteWriteONStoreAll
        (
        TInt aResult,
        CMmDataPackage* /*aDataPackage*/
        )
    {
    _LIT8( KResult, "CompleteWriteONStoreAll result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteWriteONStoreAll Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetServiceTable
// Get service table
// -----------------------------------------------------------------------------
//
TInt CSIM::GetServiceTable( CStifItemParser& aItem )
    {
    _LIT8(KWriteONStore, "SIM::GetServiceTable");
    iSIMLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;

    RMobilePhone::TMobilePhoneServiceTable table;

    aItem.GetNextString ( string );

    if ( _L("SIM") == string )
        {
        table = RMobilePhone::ESIMServiceTable;
        }
    else if( _L("USIM") == string )
        {
        table = RMobilePhone::EUSIMServiceTable;
        }
    else
        {
         _LIT8(KNotSupp, "SIM:: not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CMmDataPackage package;
        package.PackData( &table );

        ret = HandleRequestL( EMobilePhoneGetServiceTable,
                             &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "GetServiceTable request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "GetServiceTable request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }


    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetServiceTable
// Complete GetServiceTable function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetServiceTable
        (
        TInt aResult,
        CMmDataPackage* /*aDataPackage*/
        )
    {
    _LIT8( KResult, "CompleteGetServiceTable result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {

        }
    else
        {
        _LIT8( KFail, "CompleteGetServiceTable Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetALSLine
// Get ALS Line
// -----------------------------------------------------------------------------
//
TInt CSIM::GetALSLine( CStifItemParser& aItem )
    {
    _LIT8(KGetALSLine, "SIM::GetALSLine");
    iSIMLog->Log((TDesC8)KGetALSLine);

    TInt ret( KErrNotFound );

    // store given parameter for validity checking
    if( iHandleSpecifiedRequests )
        {
        TInt temp;
        aItem.GetNextInt( temp );
        iAlsLine = (RMobilePhone::TMobilePhoneALSLine) temp;
        }

    ret = HandleRequestL( EMobilePhoneGetALSLine );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetALSLine request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetALSLine request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetALSLine
// Complete GetALSLine method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetALSLine
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetALSLine result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneALSLine alsLine;

         if( aDataPackage )
            {
            aDataPackage->UnPackData( alsLine );
            _LIT8( KLine, "ALS Line:" );
            iSIMLog->Log((TDesC8)KLine );
            iSIMLog->Log( PhoneALSLine[alsLine] );
            // validity checking
            if( iHandleSpecifiedRequests )
                {
                if ( iAlsLine != alsLine)
                    {
                    aResult = KErrGeneral;
                    }
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetALSLine Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::SetALSLine
// Set ALS Line
// -----------------------------------------------------------------------------
//
TInt CSIM::SetALSLine( CStifItemParser& aItem )
    {
    _LIT8(KSetALSLine, "SIM::SetALSLine");
    iSIMLog->Log((TDesC8)KSetALSLine);

    TInt ret( KErrNotFound );

    TInt alsLine;
    aItem.GetNextInt( alsLine );

    //Create package
    CMmDataPackage package;
    package.PackData( &alsLine );

    ret = HandleRequestL( EMobilePhoneSetALSLine, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SetALSLine request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "SetALSLine request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteSetALSLine
// Complete SetALSLine method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteSetALSLine
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CompleteSetALSLine result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetALSLine Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetCustomerServiceProfile
// Get Customer Service Profile
// -----------------------------------------------------------------------------
//
TInt CSIM::GetCustomerServiceProfile( CStifItemParser& aItem )
    {
    _LIT8(KGetCSP, "SIM::GetCustomerServiceProfile");
    iSIMLog->Log((TDesC8)KGetCSP);

    TInt ret( KErrNotFound );

    // store given parameters for validity checking
    if( iHandleSpecifiedRequests )
        {
        TInt temp;
        aItem.GetNextInt( temp );
        iCspFile.iCallOfferingServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCallRestrictionServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iOtherSuppServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCallCompletionServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iTeleservices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCphsTeleservices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iCphsFeatures = temp;
        aItem.GetNextInt( temp );
        iCspFile.iNumberIdentServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iPhase2PlusServices = temp;
        aItem.GetNextInt( temp );
        iCspFile.iValueAddedServices = temp;
        }

    ret = HandleRequestL( EMobilePhoneGetCustomerServiceProfile );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetCustomerServiceProfile request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetCustomerServiceProfile request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetALSLine
// Complete GetALSLine method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetCustomerServiceProfile
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetCustomerServiceProfile result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneCspFileV1 cspFile;

         if( aDataPackage )
            {
            aDataPackage->UnPackData( cspFile );

            _LIT8( KCOS, "CallOfferingServices: %d" );
            iSIMLog->Log((TDesC8)KCOS, cspFile.iCallOfferingServices );
            _LIT8( KCRS, "CallRestrictionServices: %d" );
            iSIMLog->Log((TDesC8)KCRS, cspFile.iCallRestrictionServices );
            _LIT8( KOSS, "OtherSuppServices: %d" );
            iSIMLog->Log((TDesC8)KOSS, cspFile.iOtherSuppServices );
            _LIT8( KCCS, "CallCompletionServices: %d" );
            iSIMLog->Log((TDesC8)KCCS, cspFile.iCallCompletionServices );
            _LIT8( KTS, "Teleservices: %d" );
            iSIMLog->Log((TDesC8)KTS, cspFile.iTeleservices );
            _LIT8( KCTS, "CphsTeleservices: %d" );
            iSIMLog->Log((TDesC8)KCTS, cspFile.iCphsTeleservices );
            _LIT8( KCF, "CphsFeatures: %d" );
            iSIMLog->Log((TDesC8)KCF, cspFile.iCphsFeatures );
            _LIT8( KNIS, "NumberIdentServices: %d" );
            iSIMLog->Log((TDesC8)KNIS, cspFile.iNumberIdentServices );
            _LIT8( KPPS, "Phase2PlusServices: %d" );
            iSIMLog->Log((TDesC8)KPPS, cspFile.iPhase2PlusServices );
            _LIT8( KVAS, "ValueAddedServices: %d" );
            iSIMLog->Log((TDesC8)KVAS, cspFile.iValueAddedServices );

            // validity checking
            if( iHandleSpecifiedRequests )
                {
                if( iCspFile.iCallOfferingServices != cspFile.iCallOfferingServices ||
                    iCspFile.iCallRestrictionServices != cspFile.iCallRestrictionServices ||
                    iCspFile.iOtherSuppServices != cspFile.iOtherSuppServices ||
                    iCspFile.iCallCompletionServices != cspFile.iCallCompletionServices ||
                    iCspFile.iTeleservices != cspFile.iTeleservices ||
                    iCspFile.iCphsTeleservices != cspFile.iCphsTeleservices ||
                    iCspFile.iCphsFeatures != cspFile.iCphsFeatures ||
                    iCspFile.iNumberIdentServices != cspFile.iNumberIdentServices ||
                    iCspFile.iPhase2PlusServices != cspFile.iPhase2PlusServices ||
                    iCspFile.iValueAddedServices != cspFile.iValueAddedServices )
                    {
                    aResult = KErrGeneral;
                    }
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetCustomerServiceProfile Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSIM::GetPin1DisableSupported
// Get Customer Service Profile
// -----------------------------------------------------------------------------
//
TInt CSIM::GetPin1DisableSupported( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KGetCSP, "SIM::GetCustomerServiceProfile");
    iSIMLog->Log((TDesC8)KGetCSP);

    TInt ret( KErrNotFound );
    ret = HandleRequestL( EMmTsyPhoneGetPin1DisableSupportedIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetPin1DisableSupported request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetPin1DisableSupported request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetPin1DisableSupported
// Complete GetPin1DisableSupported method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetPin1DisableSupported
        (
        TInt aResult,
        CMmDataPackage* /*aDataPackage*/
        )
    {
    _LIT8( KResult, "CompleteGetPin1DisableSupported result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {

        }
    else
        {
        _LIT8( KFail, "CompleteGetPin1DisableSupported Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }
// -----------------------------------------------------------------------------
// CSIM::SimRefreshRegister
// Sim Refresh Register
// -----------------------------------------------------------------------------
//
TInt CSIM::SimRefreshRegister( CStifItemParser& aItem )
    {
    _LIT8(KSimRefresh, "SIM::SimRefreshRegister");
    iSIMLog->Log((TDesC8)KSimRefresh);

    TUint filesToRegister;
    aItem.GetNextInt( filesToRegister );

    //Create package
    CMmDataPackage package;
    package.PackData( &filesToRegister );

    TInt ret( KErrNotFound );
    ret = HandleRequestL( EMmTsySimRefreshRegisterIPC, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SimRefreshRegister request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "SimRefreshRegister request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CSIM::CompleteSimRefreshRegister
// Complete SimRefreshRegister method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteSimRefreshRegister
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CompleteSimRefreshRegister result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {

        }
    else
        {
        _LIT8( KFail, "CompleteSimRefreshRegister Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::SimRefreshDone
// Sim Refresh Done
// -----------------------------------------------------------------------------
//
TInt CSIM::SimRefreshDone( CStifItemParser& aItem )
    {
    _LIT8(KSimRefresh, "SIM::SimRefreshDone");
    iSIMLog->Log((TDesC8)KSimRefresh);

    TUint error;
    aItem.GetNextInt( error );

    //Create package
    CMmDataPackage package;
    package.PackData( &error );

    TInt ret( KErrNotFound );
    ret = HandleRequestL( EMmTsySimRefreshDoneIPC, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SimRefreshDone request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "SimRefreshDone request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );

        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CSIM::CompleteSimRefreshDone
// Complete SimRefreshRegister method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteSimRefreshDone
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CompleteSimRefreshDone result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {

        }
    else
        {
        _LIT8( KFail, "CompleteSimRefreshDone Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetSubscriberId
// Get Subscriber Id (IMSI)
// -----------------------------------------------------------------------------
//
TInt CSIM::GetSubscriberId( CStifItemParser& aItem )
    {
    _LIT8(KIMSI, "SIM::GetSubscriberId");
    iSIMLog->Log((TDesC8)KIMSI);

    TInt ret( KErrNotFound );

    // store given parameters for validity checking
    if ( iHandleSpecifiedRequests )
        {
        TPtrC temp;
        aItem.GetNextString( temp );
        iImsi.Append(temp);
        }

    ret = HandleRequestL( EMobilePhoneGetSubscriberId );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetSubscriberId request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetSubscriberId request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CSIM::CompleteGetSubscriberId
// Complete GetSubscriberId method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetSubscriberId
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetSubscriberId result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        TBuf8<RMobilePhone::KIMSISize> imsi;
        aDataPackage->UnPackData( imsi );
        iSIMLog->Log((TDesC8)KLine );
        _LIT8( KIMSI, "IMSI:" );
        for( TInt i( 0 ); i < RMobilePhone::KIMSISize; i++)
            {
            iSIMLog->Log((TDesC8)KIMSI );
            _LIT8( KIMSI2, " %d" );
            iSIMLog->Log((TDesC8)KIMSI2, imsi[i] );
            }
        iSIMLog->Log((TDesC8)KLine );

        // validity checking
        if ( iHandleSpecifiedRequests )
            {
            if( imsi != iImsi )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetSubscriberId Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }
// -----------------------------------------------------------------------------
// CSIM::GetServiceProviderName
// GetServiceProviderName
// -----------------------------------------------------------------------------
//
TInt CSIM::GetServiceProviderName( CStifItemParser& aItem )
    {
    _LIT8(KIMSI, "SIM::GetSubscriberId");
    iSIMLog->Log((TDesC8)KIMSI);

    TInt ret( KErrNotFound );

    if( iHandleSpecifiedRequests )
        {
        aItem.GetNextInt( iDisplayReq );
        }

    ret = HandleRequestL( EMobilePhoneGetServiceProviderName );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetServiceProviderName request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetServiceProviderName request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CSIM::CompleteGetServiceProviderName
// Complete GetServiceProviderName method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetServiceProviderName
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetServiceProviderName result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneServiceProviderNameV2 serviceProviderName;
        aDataPackage->UnPackData( serviceProviderName );

        TBuf8<255> drBuffer;
        _LIT8(KRowFormatter1, "Display req: %d");
        drBuffer.Format(KRowFormatter1, &serviceProviderName.iDisplayReq);
        iSIMLog->Log(drBuffer);

        TBuf8<255> nameBuffer;
        _LIT8(KRowFormatter2, "Service provider name: %S");
        nameBuffer.Format(KRowFormatter2, &serviceProviderName.iSPName);
        iSIMLog->Log(nameBuffer);

        TBuf8<255> plmnBuffer;
        _LIT8(KRowFormatter3, "PLMN field: %S");
        plmnBuffer.Format(KRowFormatter3, &serviceProviderName.iPLMNField);
        iSIMLog->Log(plmnBuffer);

        if( iHandleSpecifiedRequests )
            {
            if(serviceProviderName.iDisplayReq != iDisplayReq )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetServiceProviderName Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSIM::ReadSimFile
// ReadSimFile
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadSimFile( CStifItemParser& aItem )
    {
    _LIT8(KReadSimFile, "SIM::ReadSimFile");
    iSIMLog->Log((TDesC8)KReadSimFile);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;

    RMmCustomAPI::TSimFileInfo simFileInfo;

    // File ID
    TBuf8<8> path; // File path in SIM where file is read
    TInt offSet; // Offset from beginning of the file
    TInt size; // Amount of data to read

    TInt masterFile;  // Master File
    TInt dedicatedFile1;  // Dedicated File
    TInt dedicatedFile2;  // Dedicated File
    TInt elementaryFile;  // Elementary File

    aItem.GetNextInt( masterFile );
    aItem.GetNextInt( dedicatedFile1 );
    aItem.GetNextInt( dedicatedFile2 );
    aItem.GetNextInt( elementaryFile );
    aItem.GetNextInt( offSet );
    aItem.GetNextInt( size );

    path.Append(masterFile>>8);
    path.Append(masterFile);
    path.Append(dedicatedFile1>>8);
    path.Append(dedicatedFile1);
    path.Append(dedicatedFile2>>8);
    path.Append(dedicatedFile2);
    path.Append(elementaryFile>>8);
    path.Append(elementaryFile);

    simFileInfo.iPath = path;
    simFileInfo.iOffSet = (TUint16)offSet;
    simFileInfo.iSize = (TUint16)size;

    //Create package
    CMmDataPackage dataPackage;
    dataPackage.PackData( &simFileInfo );

    ret = HandleRequestL( ECustomReadSimFileIPC,
                         &dataPackage );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ReadSimFile request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "ReadSimFile request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteReadSimFile
// Complete ReadSimFile method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadSimFile
        (
        TInt aResult,
        CMmDataPackage* /*aDataPackage*/
        )
    {


    _LIT8( KResult, "CompleteReadSimFile result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        _LIT8( KPass, "CompleteReadSimFile Passed: %d" );
        iSIMLog->Log((TDesC8)KPass, aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteReadSimFile Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }



// -----------------------------------------------------------------------------
// CSIM::GetNetworkProviderName
// GetNetworkProviderName
// -----------------------------------------------------------------------------
//
TInt CSIM::GetNetworkProviderName( CStifItemParser& aItem )
    {
    _LIT8(KWriteONStore, "SIM::GetNetworkProviderName");
    iSIMLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );

    // for validity checking
    if( iHandleSpecifiedRequests )
        {
        TPtrC temp;
        aItem.GetNextString( temp );
        if ( temp == _L("Orange") )
            {
            iName.Append( _L(" Orange") );
            }
        }

    ret = HandleRequestL( ECustomGetNetworkProviderNameIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetNetworkProviderName request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetNetworkProviderName request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetNetworkProviderName
// Complete GetNetworkProviderName method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetNetworkProviderName
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetNetworkProviderName result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        //network provider name
        TDesC* name;

        aDataPackage->UnPackData( name );

        _LIT8( KName, "Name:" );
        iSIMLog->Log((TDesC8)KName);
        iSIMLog->Log( *name );

        // validity checking
        if( iHandleSpecifiedRequests )
            {
            TBuf<20> tempName;
            tempName.Append( *name );
            if ( iName != tempName )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetNetworkProviderName Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetHexFromString
// Converts source string containing ascii characters 0-9, a-f, A-F to
// corresponding hex value.
// (other items was commented in header)
// -----------------------------------------------------------------------------
//
TInt CSIM::GetHexFromString( TPtrC aSource, TDes8& aDest ) const
    {
    TUint8 number( 0 );
    TInt ret( 0 );

    TBuf<2> charBuf;
    TPtrC ptr;
    TLex lexChar;

    //It must be possible to divide source by two
    if ( 0 == ( aSource.Length() % 2 ) )
        {
        for ( TUint i = 0;
              (i < ( aDest.MaxLength() * 2 ) && i < aSource.Length() );
              i = i + 2)
            {
            charBuf.Zero();
            // Take two characters to be converted.
            charBuf.Append( aSource[i] );
            charBuf.Append( aSource[i+1] );
            ptr.Set( charBuf );
            lexChar = ptr;
            ret = lexChar.Val( number, EHex );
            aDest.Append( number );

            if ( KErrNone != ret)
                {
                break;
                }
            } //for
        } // if ( 0 == ( aSource.Length() % 2 ) )
    else
        {
        ret = KErrArgument;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::GetSimAuthenticationData
//
// -----------------------------------------------------------------------------
//
TInt CSIM::GetSimAuthenticationData( CStifItemParser& aItem )
    {
    _LIT8(KWriteONStore, "SIM::GetSimAuthenticationData");
    iSIMLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;

    aItem.GetNextString ( string );

    if ( _L("SIM") == string )
        {

        //Create package
        CMmDataPackage package;
        RMmCustomAPI::TSimAuthenticationEapSim::TSimAuthenticationEapSim();
        RMmCustomAPI::TSimAuthenticationEapSim eapSim;

        TInt rfsState = 1; // ERfsActivated;

        //Fill iRandomParameters, used as data in SIM_SB_RAND_PARAMS sub block.
        eapSim.iRandomParameters.Fill( 0x00, 16 );

        eapSim.iSRES.Append( 0 );
        eapSim.iKC.Append( 0 );

        package.PackData( &eapSim, &rfsState );
        ret = HandleRequestL( ECustomGetSimAuthenticationDataIPC, &package );
        }
    else if( _L("AKA") == string )
        {
        //Create package
        CMmDataPackage package;
        RMmCustomAPI::TSimAuthenticationEapAka::TSimAuthenticationEapAka();
        RMmCustomAPI::TSimAuthenticationEapAka eapAka;

        TInt rfsState = 1; // ERfsActivated;

        //Fill iRandomParameters and iAUTN. They are used as data in
        //SIM_SB_RAND_PARAMS  and SIM_SB_UTMS_AUTN_PARAMS sub blocks.
        eapAka.iRandomParameters.Fill( 0x00, 16);
        eapAka.iAUTN.Fill( 0x00, 16);

        eapAka.iRES.Append( 0 );
        eapAka.iCK.Append( 0 );
        eapAka.iIK.Append( 0 );
        eapAka.iAUTS.Append( 0 );

        package.PackData( &eapAka, &rfsState );
        ret = HandleRequestL( ECustomGetSimAuthenticationDataIPC, &package );
        }
    else if( _L("GBABOOT") == string
        || _L("GBABOOTUPDATE") == string
        || _L("GBABOOTNAFDER") == string )
        {
        if ( iHandleSpecifiedRequests )
            {
            ret = GenerateGbaAuthenticationReq( string, aItem );
            }
        else
            {
            _LIT8(KNotSupported, "SIM::GetSimAuthenticationData - GBA only supported with iHandleSpecifiedRequest");
            iSIMLog->Log( ( TDesC8 )KNotSupported );
            }
        }

    else if( _L("MGVMTKGENERATION") == string
        || _L("MGVMSKUPDATE") == string
        || _L("MGVMSKDELETION") == string )
        {
        if ( iHandleSpecifiedRequests )
            {
            ret = GenerateMgvAuthenticationReq( string, aItem );
            }
        else
            {
            _LIT8(KNotSupported, "SIM::GetSimAuthenticationData - MGV only supported with iHandleSpecifiedRequest");
            iSIMLog->Log( ( TDesC8 )KNotSupported);
            }
        }

    else
    {
     _LIT8(KNotSupp, "SIM::Not supported");
    iSIMLog->Log( ( TDesC8 )KNotSupp);
    supported = EFalse;
    }

    if( supported )
        {
        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "GetSimAuthenticationData request send ok" );
            iSIMLog->Log( ( TDesC8 )KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "GetSimAuthenticationData request send failed: %d" );
            iSIMLog->Log( ( TDesC8 )KSendFailed, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetSimAuthenticationData
// Complete WriteONStore method function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetSimAuthenticationData
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetSimAuthenticationData result: %d" );
    iSIMLog->Log( ( TDesC8 )KResult, aResult );

    if( KErrNone == aResult && !iHandleSpecifiedRequests )
        {
        RMmCustomAPI::TSimAuthenticationBase* basePtr;
        aDataPackage->UnPackData( &basePtr );

        switch ( basePtr->ExtensionId() )
            {
            case RMmCustomAPI::TSimAuthenticationBase::EEapSim:
                {
                //2G authentication
                RMmCustomAPI::TSimAuthenticationEapSim* eapSim;
                aDataPackage->UnPackData( &eapSim );

                _LIT8( KSRES, "2G: SRES:" );
                iSIMLog->Log( ( TDesC8 )KSRES );
                TInt i = 0;
                for ( i = 0; i < ( eapSim->iSRES ).Length(); i++ )
                    {
                    _LIT8( KSRES2, "%d" );
                    iSIMLog->Log( ( TDesC8 )KSRES2, eapSim->iSRES[i] );
                    }

                _LIT8( KKC, "2G: KC:" );
                iSIMLog->Log( ( TDesC8)KKC );
                for ( i = 0; i < (eapSim->iKC).Length(); i++ )
                    {
                    _LIT8( KKC2, "%d" );
                    iSIMLog->Log( ( TDesC8 )KKC2, eapSim->iKC[i] );
                    }

                break;
                }
            case RMmCustomAPI::TSimAuthenticationBase::EEapAka:
                {
                //3G authentication
                RMmCustomAPI::TSimAuthenticationEapAka* eapAka;

                aDataPackage->UnPackData( &eapAka );

                _LIT8( KRES, "3G: RES:" );
                iSIMLog->Log( ( TDesC8 )KRES );
                TInt i = 0;
                for ( i = 0; i < ( eapAka->iRES ).Length(); i++ )
                    {
                    _LIT8( KRES2, "%d" );
                    iSIMLog->Log( ( TDesC8 )KRES2, eapAka->iRES[i] );
                    }

                _LIT8( KCK, "3G: CK:" );
                iSIMLog->Log( ( TDesC8 )KCK );
                for ( i = 0; i < ( eapAka->iCK ).Length(); i++)
                    {
                    _LIT8( KCK2, "%d" );
                    iSIMLog->Log( ( TDesC8 )KCK2, eapAka->iCK[i] );
                    }
                _LIT8( KIK, "3G: IK:" );
                iSIMLog->Log((TDesC8)KIK );
                for ( i = 0; i < ( eapAka->iIK ).Length(); i++ )
                    {
                    _LIT8( KIK2, "%d" );
                    iSIMLog->Log( ( TDesC8 )KIK2, eapAka->iIK[i] );
                    }
                break;
                }
            default:
                {
                _LIT8( KFail, "CompleteGetSimAuthenticationData - unknown extensionId: %d" );
                iSIMLog->Log( ( TDesC8 )KFail, basePtr->ExtensionId() );
                break;
                }
            } // switch ( basePtr->ExtensionId() )
        } // if( KErrNone == aResult && !iHandleSpecifiedRequests )
    else if ( KErrNone != aResult && !iHandleSpecifiedRequests )
        {
        _LIT8( KFail, "CompleteGetSimAuthenticationData Failed: %d" );
        iSIMLog->Log( ( TDesC8 )KFail, aResult );
        
        if( -3214 == aResult || // KErrMMEtelSqnVerificationFailed
            -3213 == aResult )  // KErrMMEtelMacVerificationFailed
            {
                RMmCustomAPI::TSimAuthenticationEapAka* eapAka;
                aDataPackage->UnPackData( &eapAka );
                
                _LIT8( KRES, "3G: AUTS:" );
                iSIMLog->Log( ( TDesC8 )KRES );
                
                TInt i = 0;
                for ( i = 0; i < ( eapAka->iAUTS ).Length(); i++ )
                    {
                    _LIT8( KRES2, "%d" );
                    iSIMLog->Log( ( TDesC8 )KRES2, eapAka->iAUTS[i] );
                    }
            }
        
        }

    else if ( iHandleSpecifiedRequests )
        {
        // GBA related tests
        if ( EGbaBootOk == iAuthenticationPurpose
            || EGbaBootAuthFail == iAuthenticationPurpose
            || EGbaBootVerificationFail == iAuthenticationPurpose
            || EGbaBootError == iAuthenticationPurpose
            || EGbaNafDerOk == iAuthenticationPurpose
            || EGbaNafDerAuthFail == iAuthenticationPurpose
            || EGbaBootNafError == iAuthenticationPurpose
            || EGbaBootUpdateOk == iAuthenticationPurpose
            || EGbaBootUpdateError == iAuthenticationPurpose )
            {
            HandleGbaCompletion( aResult, aDataPackage );
            }

        // MGV relates tests
        if ( EMgvMskUpdateOk == iAuthenticationPurpose
            || EMgvMskUpdateNull == iAuthenticationPurpose
            || EMgvMskUpdateError == iAuthenticationPurpose
            || EMgvMskUpdateWithOmaBCastExtension == iAuthenticationPurpose
            || EMgvMtkGenerationOk == iAuthenticationPurpose
            || EMgvMtkGenerationError == iAuthenticationPurpose
            || EMgvMskDeletionOk == iAuthenticationPurpose
            || EMgvMskDeletionError == iAuthenticationPurpose
            || EMgvMskDeletionAuthFail == iAuthenticationPurpose
            || EMgvMtkGenerationWithBcastManagementData == iAuthenticationPurpose 
            || EMgvMtkGenerationWithBCastParentalControlData == iAuthenticationPurpose)
            {
            HandleMgvCompletion( aResult, aDataPackage );
            }
        } // else if ( iHandleSpecifiedRequests )

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GenerateMgvAuthenticationReq
// Generates and sends SIM_AUTHENTICATION_REQ message related to MGV.
// (other items was commented in header)
// -----------------------------------------------------------------------------
//
TInt CSIM::GenerateMgvAuthenticationReq( TPtrC aString, CStifItemParser& aItem )
    {
    // String to describe that conversion from input parameter to hex has failed
    _LIT8(KFail, "SIM::GenerateMgvAuthenticationReq - Hex getting from string failed: %d");
    TInt ret( KErrNone );
    CMmDataPackage package;

    // Read test case purpose
    aItem.GetNextInt( iAuthenticationPurpose );

    if ( _L("MGVMSKUPDATE") == aString )
        {
        RMmCustomAPI::TSimAuthenticationMgvMskUpdate mgvMskUpdate;

        // Pointers to strings passed in from .cfg script
        TPtrC mikey;
        TPtrC retParam;

        // Read and convert parameters going to DOS
        aItem.GetNextString( mikey );
        ret = GetHexFromString( mikey, mgvMskUpdate.iMikey );

        // Read and convert string to hexadecimals and store them
        // (data coming to SOS). Expected incoming data depends on test
        // purpose (ok, null Mikey, authentication fail).
        if ( KErrNone == ret )
            {
            if ( EMgvMskUpdateOk == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMskUpdate.iMikey );
                }
             else if( EMgvMskUpdateWithOmaBCastExtension == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMskUpdate.iMikey );
                    
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMskUpdate.iBCASTManagement );
                    
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMskUpdate.iParentalRating );
                    
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMskUpdate.iSecurityPolicyExt );
                }
            }
            else if ( EMgvMskUpdateNull == iAuthenticationPurpose
                || EMgvMskUpdateError == iAuthenticationPurpose )
                {
                // nothing must be saved for comparision.
                }

        // If conversion ok, pack data
        if ( KErrNone == ret )
            {
            TInt rfsState = 1; // ERfsActivated;
            package.PackData( &mgvMskUpdate, &rfsState );
            }
        } // if ( _L("MGVMSKUPDATE") == aString )

    else if ( _L("MGVMTKGENERATION") == aString )
        {
        RMmCustomAPI::TSimAuthenticationMgvMtkGeneration mtkGen;

        // Pointers to strings passed in from .cfg script
        TPtrC mikey;
        TPtrC retParam;

        // Read and convert parameters going to DOS
        aItem.GetNextString( mikey );
        ret = GetHexFromString( mikey, mtkGen.iMikey );

        // Read and convert string to hexadecimals and store them
        // (data coming to SOS). Expected incoming data depends on test
        // purpose (ok, error).
        if ( KErrNone == ret )
            {
            if ( EMgvMtkGenerationOk == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMtkGeneration.iMtkSalt );
                }
            else if ( EMgvMtkGenerationWithBcastManagementData == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMtkGeneration.iBCASTManagement );
                    
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMtkGeneration.iTrafficEncryptionKey );
                    
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMtkGeneration.iMtkSalt );
                }    
            else if( EMgvMtkGenerationWithBCastParentalControlData == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMtkGeneration.iBCASTManagement );
                    
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationMgvMtkGeneration.iParentalControl );
                } 
            }
            else if ( EMgvMtkGenerationError == iAuthenticationPurpose )
                {
                // nothing must be saved for comparision.
                }

        // If conversion ok, pack data
        if ( KErrNone == ret )
            {
            TInt rfsState = 1; // ERfsActivated;
            package.PackData( &mtkGen, &rfsState );
            }
        } // else if ( _L("MGVMTKGENERATION") == aString )

    else if ( _L("MGVMSKDELETION") == aString )
        {
        RMmCustomAPI::TSimAuthenticationMgvMskDeletion mskDel;

        // Pointers to strings passed in from .cfg script
        TPtrC domainId;
        TPtrC groupId;

        // Read and convert parameters going to DOS
        aItem.GetNextString( domainId );
        ret = GetHexFromString( domainId, mskDel.iKeyDomainId );
        if ( KErrNone == ret )
            {
            aItem.GetNextString( groupId );
            ret = GetHexFromString( groupId, mskDel.iKeyGroupIdPart );
            }

        // If conversion ok, pack data
        if ( KErrNone == ret )
            {
            TInt rfsState = 1; // ERfsActivated;
            package.PackData( &mskDel, &rfsState );
            }
        } //  else if ( _L("MGVMSKDELETION") == aString ) )

    // Send data
    if ( KErrNone == ret )
        {
        ret = HandleRequestL( ECustomGetSimAuthenticationDataIPC, &package );
        }
    else
        {
        iSIMLog->Log( ( TDesC8 )KFail, ret);
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::GenerateGbaAuthenticationReq
// Generates and sends SIM_AUTHENTICATION_REQ message related to GBA.
// (other items was commented in header)
// -----------------------------------------------------------------------------
//
TInt CSIM::GenerateGbaAuthenticationReq( TPtrC aString, CStifItemParser& aItem )
    {
    // String to describe that conversion from input parameter to hex has failed
    _LIT8(KFail, "SIM::GenerateGbaAuthenticationReq - Hex getting from string failed: %d");
    TInt ret( KErrNone );
    CMmDataPackage package;

    // Read test case purpose
    aItem.GetNextInt( iAuthenticationPurpose );

    if ( _L("GBABOOT") == aString )
        {
        RMmCustomAPI::TSimAuthenticationGbaBootstrap gbaBoot;

        // Pointers to strings passed in from .cfg script
        TPtrC randParams;
        TPtrC autn;
        TPtrC retParam;

        // Read and convert parameters going to DOS
        aItem.GetNextString( autn );
        aItem.GetNextString( randParams );
        ret = GetHexFromString( randParams, gbaBoot.iRandomParameters );
        if ( KErrNone == ret )
            {
            ret = GetHexFromString( autn, gbaBoot.iAUTN );
            }

        // Read and convert string to hexadecimals and store them
        // (data coming to SOS). Expected incoming data depends on test
        // purpose (ok, authentication fail, verification fail).
        if ( KErrNone == ret )
            {
            if ( EGbaBootOk == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationGbaBoot.iRES );
                }
            else if ( ( EGbaBootAuthFail == iAuthenticationPurpose )
                || ( EGbaBootError == iAuthenticationPurpose ) )
                {
                // nothing is returned from DOS to SOS in these cases.
                }
            else if ( EGbaBootVerificationFail == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationGbaBoot.iAUTS );
                }
            } // if ( KErrNone == ret )

        //Pack data
        if ( KErrNone == ret )
            {
            TInt rfsState = 1; // ERfsActivated;
            package.PackData( &gbaBoot, &rfsState );
            }

        } //if ( _L("GBABOOT") == aString )

    else if ( _L("GBABOOTUPDATE") == aString )
        {
        RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate gbaUpdate;

        // Pointers to strings passed in from .cfg script
        TPtrC bTid;
        TPtrC keyLifetime;
        TPtrC retParam;

        // Read and convert parameters going to DOS
        aItem.GetNextString( bTid );
        aItem.GetNextString( keyLifetime );
        ret = GetHexFromString( bTid, gbaUpdate.iBTid );
        if ( KErrNone == ret )
            {
            ret = GetHexFromString( keyLifetime, gbaUpdate.iKeyLifeTime );
            }

        // Read and convert string to hexadecimals and store it
        // (data coming to SOS).
        if ( KErrNone == ret )
            {
            if ( EGbaBootUpdateOk == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationGbaBootUpdate.iRandomParameters );
                }
            else
                {
                // nothing to be compared
                }

            }

        //Pack data
        if ( KErrNone == ret )
            {
            TInt rfsState = 1; // ERfsActivated;
            package.PackData( &gbaUpdate, &rfsState );
            }

        } // else if ( _L("GBABOOT") == aString )

    else if ( _L("GBABOOTNAFDER") == aString )
        {
        RMmCustomAPI::TSimAuthenticationGbaNafDerivation gbaNaf;

        // Pointers to strings passed in from .cfg script
        TPtrC nafId;
        TPtrC impi;
        TPtrC retParam;

        // Read and convert parameters going to DOS
        aItem.GetNextString( nafId );
        aItem.GetNextString( impi );
        ret = GetHexFromString( nafId, gbaNaf.iNafId );
        if ( KErrNone == ret )
            {
            ret = GetHexFromString( impi, gbaNaf.iImpi );
            }

        // Read and convert string to hexadecimals and store them
        // (data coming to SOS). Expected incoming data depends on test
        // purpose (ok, authentication fail).
        if ( KErrNone == ret )
            {
            if ( EGbaNafDerOk == iAuthenticationPurpose )
                {
                aItem.GetNextString( retParam );
                ret = GetHexFromString(
                    retParam,
                    iAuthenticationGbaNafDerivation.iKsExtNaf );
                }
            else if ( ( EGbaNafDerAuthFail == iAuthenticationPurpose )
                || ( EGbaBootNafError == iAuthenticationPurpose ) )
                {
                // nothing is returned from DOS to SOS in this case.
                }
            }

        //Pack data
        if ( KErrNone == ret )
            {
            TInt rfsState = 1; // ERfsActivated;
            package.PackData( &gbaNaf, &rfsState );
            }
        } // else if ( _L("GBABOOTNADFER") == aString )

    // Send data
    if ( KErrNone == ret )
        {
        ret = HandleRequestL( ECustomGetSimAuthenticationDataIPC, &package );
        }
    else
        {
        iSIMLog->Log( ( TDesC8 ) KFail, ret);
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSIM::HandleMgvCompletion
// Handles MGV related SIM_AUTHENTICATION completions. Checks if completion data
// is as waited.
// (other items was commented in header)
// -----------------------------------------------------------------------------
//
void CSIM::HandleMgvCompletion( TInt& aResult, CMmDataPackage* aDataPackage )
    {
    _LIT8( KCompareOk, "CSIM::HandleMgvCompletion: Comparison OK between waited and completed data" );
    _LIT8( KCompareFail, "CSIM::HandleMgvCompletion: Comparison failed between waited and completed data" );
    _LIT8( KExtFail, "CSIM::HandleMgvCompletion: extension id of packet wrong!" );

    if ( EMgvMskUpdateOk == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationMgvMskUpdate* mgvMskUpdate;
        aDataPackage->UnPackData( &mgvMskUpdate );

         // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate
            != mgvMskUpdate->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMskUpdate.iMikey.Compare(
            mgvMskUpdate->iMikey ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }

        // Log completed data
        _LIT8( KMikey, "MGV MSK update, received Mikey:" );
        iSIMLog->Log( ( TDesC8 )KMikey );
        for ( TInt i( 0 ); i < ( mgvMskUpdate->iMikey ).Length(); i++ )
            {
            _LIT8( KMikey2, "%d" );
            iSIMLog->Log( ( TDesC8 )KMikey2, mgvMskUpdate->iMikey[i] );
            }
        } // if ( EMgvMskUpdateOk == iAuthenticationPurpose )

    else if ( EMgvMskUpdateNull == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationMgvMskUpdate* mgvMskUpdate;
        aDataPackage->UnPackData( &mgvMskUpdate );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate
            != mgvMskUpdate->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Mikey sub block must be included to ISI message, but Mikey
        // length must be zero.
        if ( 0 == iAuthenticationMgvMskUpdate.iMikey.Length() )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
        } // else if ( EMgvMskUpdateNull == iAuthenticationPurpose )
        
    else if ( EMgvMskUpdateWithOmaBCastExtension == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationMgvMskUpdate* mgvMskUpdate;
        aDataPackage->UnPackData( &mgvMskUpdate );
        
                 // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate
            != mgvMskUpdate->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }
            
        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMskUpdate.iMikey.Compare(
            mgvMskUpdate->iMikey ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }

        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMskUpdate.iBCASTManagement.Compare(
            mgvMskUpdate->iBCASTManagement ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMskUpdate.iParentalRating.Compare(
            mgvMskUpdate->iParentalRating ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMskUpdate.iSecurityPolicyExt.Compare(
            mgvMskUpdate->iSecurityPolicyExt ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
        
        
         // Log completed Mikey data
        _LIT8( KMikey, "MGV MSK update with OMa Bcast extension, received Mikey:" );
        iSIMLog->Log( ( TDesC8 )KMikey );
        for ( TInt i( 0 ); i < ( mgvMskUpdate->iMikey ).Length(); i++ )
            {
            _LIT8( KMikey2, "%d" );
            iSIMLog->Log( ( TDesC8 )KMikey2, mgvMskUpdate->iMikey[i] );
            }
            
         // Log completed BCast Management data
        _LIT8( KBCastManagement, "MGV MSK update with OMa Bcast extension, received BCast Management:" );
        iSIMLog->Log( ( TDesC8 )KBCastManagement );
        for ( TInt i( 0 ); i < ( mgvMskUpdate->iBCASTManagement ).Length(); i++ )
            {
            _LIT8( KBCastManagement2, "%d" );
            iSIMLog->Log( ( TDesC8 )KBCastManagement2, mgvMskUpdate->iBCASTManagement[i] );
            }
            
         // Log completed Parental Rating data
        _LIT8( KParentalRating, "MGV MSK update with OMa Bcast extension, received Parental Rating:" );
        iSIMLog->Log( ( TDesC8 )KParentalRating );
        for ( TInt i( 0 ); i < ( mgvMskUpdate->iParentalRating ).Length(); i++ )
            {
            _LIT8( KParentalRating2, "%d" );
            iSIMLog->Log( ( TDesC8 )KParentalRating2, mgvMskUpdate->iParentalRating[i] );
            }
            
         // Log completed Security policy ext data
        _LIT8( KSpe, "MGV MSK update with OMa Bcast extension, received SEcurity policy extension data:" );
        iSIMLog->Log( ( TDesC8 )KSpe );
        for ( TInt i( 0 ); i < ( mgvMskUpdate->iSecurityPolicyExt ).Length(); i++ )
            {
            _LIT8( KSpe2, "%d" );
            iSIMLog->Log( ( TDesC8 )KSpe2, mgvMskUpdate->iSecurityPolicyExt[i] );
            }
        } // else if ( EMgvMskUpdateWithOmaBCastExtension == iAuthenticationPurpose )
    
    else if ( EMgvMskUpdateError == iAuthenticationPurpose )
        {
        // nothing to compare or to do in this case.
        }

    else if ( EMgvMtkGenerationOk == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* mtkGen;
        aDataPackage->UnPackData( &mtkGen );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMtkGeneration
            != mtkGen->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMtkGeneration.iMtkSalt.Compare(
            mtkGen->iMtkSalt ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }

        // Log completed data
        _LIT8( KSalt, "MGV MTK generation, received MTK salt:" );
        iSIMLog->Log( ( TDesC8 )KSalt );
        for ( TInt i( 0 ); i < ( mtkGen->iMtkSalt ).Length(); i++ )
            {
            _LIT8( KSalt2, "%d" );
            iSIMLog->Log( ( TDesC8 )KSalt2, mtkGen->iMtkSalt[i] );
            }
        } // if ( EMgvMtkGenerationOk == iAuthenticationPurpose )
        
    else if( EMgvMtkGenerationWithBcastManagementData == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* mtkGen;
        aDataPackage->UnPackData( &mtkGen );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMtkGeneration
            != mtkGen->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare BCast management completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMtkGeneration.iBCASTManagement.Compare(
            mtkGen->iBCASTManagement ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        // Compare TEK completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMtkGeneration.iTrafficEncryptionKey.Compare(
            mtkGen->iTrafficEncryptionKey ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        // Compare SALT completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMtkGeneration.iMtkSalt.Compare(
            mtkGen->iMtkSalt ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        if ( 0 == mtkGen->iMikey.Length() &&
             0 == mtkGen->iParentalControl.Length() )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        // Log completed BCast management data
        _LIT8( KBCastManagementData, "MGV MTK generation with BCast management data operation, received BCast management data:" );
        iSIMLog->Log( ( TDesC8 )KBCastManagementData );
        for ( TInt i( 0 ); i < ( mtkGen->iBCASTManagement ).Length(); i++ )
            {
            _LIT8( KBCastManagementData2, "%d" );
            iSIMLog->Log( ( TDesC8 )KBCastManagementData2, mtkGen->iBCASTManagement[i] );
            }
            
        // Log completed TEK data
        _LIT8( KTek, "MGV MTK generation with BCast management data operation, received TEK data:" );
        iSIMLog->Log( ( TDesC8 )KTek );
        for ( TInt i( 0 ); i < ( mtkGen->iTrafficEncryptionKey ).Length(); i++ )
            {
            _LIT8( KTek2, "%d" );
            iSIMLog->Log( ( TDesC8 )KTek2, mtkGen->iTrafficEncryptionKey[i] );
            }
            
        // Log completed TEK data
        _LIT8( KSalt, "MGV MTK generation with BCast management data operation, received SALT data:" );
        iSIMLog->Log( ( TDesC8 )KSalt );
        for ( TInt i( 0 ); i < ( mtkGen->iMtkSalt ).Length(); i++ )
            {
            _LIT8( KSalt2, "%d" );
            iSIMLog->Log( ( TDesC8 )KSalt2, mtkGen->iMtkSalt[i] );
            }
        } // else if( EMgvMtkGenerationWithBcastManagementData == iAuthentcationPurpose )
        
    else if(EMgvMtkGenerationWithBCastParentalControlData == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* mtkGen;
        aDataPackage->UnPackData( &mtkGen );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMtkGeneration
            != mtkGen->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare BCast management completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMtkGeneration.iBCASTManagement.Compare(
            mtkGen->iBCASTManagement ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        // Compare TEK completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationMgvMtkGeneration.iParentalControl.Compare(
            mtkGen->iParentalControl ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        if ( 0 == mtkGen->iMikey.Length() &&
             0 == mtkGen->iMtkSalt.Length() &&
             0 == mtkGen->iTrafficEncryptionKey.Length() )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }
            
        // Log completed BCast management data
        _LIT8( KBCastManagementData, "MGV MTK generation with BCast parental control operation, received BCast management data:" );
        iSIMLog->Log( ( TDesC8 )KBCastManagementData );
        for ( TInt i( 0 ); i < ( mtkGen->iBCASTManagement ).Length(); i++ )
            {
            _LIT8( KBCastManagementData2, "%d" );
            iSIMLog->Log( ( TDesC8 )KBCastManagementData2, mtkGen->iBCASTManagement[i] );
            }
            
        // Log completed TEK data
        _LIT8( KParentalControl, "MGV MTK generation with BCast parental control operation, received parental control data:" );
        iSIMLog->Log( ( TDesC8 )KParentalControl );
        for ( TInt i( 0 ); i < ( mtkGen->iParentalControl ).Length(); i++ )
            {
            _LIT8( KParentalControl2, "%d" );
            iSIMLog->Log( ( TDesC8 )KParentalControl2, mtkGen->iParentalControl[i] );
            }
        } // else if(EMgvMtkGenerationWithBCastParentalControlData == iAuthenticationPurpose )
        

    else if ( EMgvMtkGenerationError == iAuthenticationPurpose )
        {
        // nothing to compare or to do in this case.
        }

    else if ( EMgvMskDeletionOk == iAuthenticationPurpose
        || EMgvMskDeletionError == iAuthenticationPurpose
        || EMgvMskDeletionAuthFail == iAuthenticationPurpose )
        {
        _LIT8( KOk, "MGV MSK deletion, completion received with error code: %d" );
        iSIMLog->Log( ( TDesC8 )KOk, aResult );
        }

    else
        {
         _LIT8( KDefault, "CSIM::HandleMgvCompletion: no matching auth purpose id" );
        iSIMLog->Log( ( TDesC8 )KDefault );
        aResult = KErrGeneral;
        }
    }


// -----------------------------------------------------------------------------
// CSIM::HandleGbaCompletion
// Handles GBA related SIM_AUTHENTICATION completions. Checks if completion data
// is as waited.
// (other items was commented in header)
// -----------------------------------------------------------------------------
//
void CSIM::HandleGbaCompletion( TInt& aResult, CMmDataPackage* aDataPackage )
    {
    _LIT8( KCompareOk, "CSIM::HandleGbaCompletion: Comparison OK between waited and completed data" );
    _LIT8( KCompareFail, "CSIM::HandleGbaCompletion: Comparison failed between waited and completed data" );
    _LIT8( KExtFail, "CSIM::HandleGbaCompletion: extension id of packet wrong!" );

    if ( EGbaBootOk == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationGbaBootstrap* gbaBoot;
        aDataPackage->UnPackData( &gbaBoot );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap
            != gbaBoot->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationGbaBoot.iRES.Compare( gbaBoot->iRES ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }

        // Log completed data
        _LIT8( KRES, "GBA bootstrap, received RES:" );
        iSIMLog->Log( ( TDesC8 )KRES );
        for ( TInt i( 0 ); i < ( gbaBoot->iRES ).Length(); i++ )
            {
            _LIT8( KRES2, "%d" );
            iSIMLog->Log( ( TDesC8 )KRES2, gbaBoot->iRES[i] );
            }
        }

    else if ( ( EGbaBootAuthFail == iAuthenticationPurpose )
        || ( EGbaBootError == iAuthenticationPurpose ) )
        {
            // Nothing to do or compare in these cases.
        }

    else if ( EGbaBootVerificationFail == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationGbaBootstrap* gbaBoot;
        aDataPackage->UnPackData( &gbaBoot );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap
            != gbaBoot->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationGbaBoot.iAUTS.Compare(
            gbaBoot->iAUTS ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }

        // Log completed data
        _LIT8( KAUTS, "GBA bootstrap, received AUTS:" );
        iSIMLog->Log( ( TDesC8 )KAUTS );
        for ( TInt i( 0 ); i < ( gbaBoot->iAUTS ).Length(); i++ )
            {
            _LIT8( KAUTS2, "%d" );
            iSIMLog->Log( ( TDesC8 )KAUTS2, gbaBoot->iAUTS[i] );
            }
        }


    else if ( EGbaBootUpdateOk == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate* gbaUpdate;
        aDataPackage->UnPackData( &gbaUpdate );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapUpdate
            != gbaUpdate->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationGbaBootUpdate.iRandomParameters.Compare(
            gbaUpdate->iRandomParameters ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }

        // Log completed data
        _LIT8( KRand, "GBA bootstrap update, received random params:" );
        iSIMLog->Log( ( TDesC8 )KRand );
        for ( TInt i( 0 ); i < ( gbaUpdate->iRandomParameters ).Length(); i++ )
            {
            _LIT8( KRand2, "%d" );
            iSIMLog->Log(
                ( TDesC8 )KRand2,
                gbaUpdate->iRandomParameters[i] );
            }
        }

    else if ( EGbaBootUpdateError == iAuthenticationPurpose )
        {
        // nothing to compare or to do in this case.
        }

    else if ( EGbaNafDerOk == iAuthenticationPurpose )
        {
        RMmCustomAPI::TSimAuthenticationGbaNafDerivation* gbaNaf;
        aDataPackage->UnPackData( &gbaNaf );

        // Check correctness of extension id
        if ( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapNafDerivation
            != gbaNaf->ExtensionId() )
            {
            iSIMLog->Log( ( TDesC8 )KExtFail );
            aResult = KErrGeneral;
            }

        // Compare completion data to waited data (inputted from .cfg)
        if ( 0 == iAuthenticationGbaNafDerivation.iKsExtNaf.Compare(
            gbaNaf->iKsExtNaf ) )
            {
            iSIMLog->Log( ( TDesC8 )KCompareOk );
            }
        else
            {
            iSIMLog->Log( ( TDesC8 )KCompareFail );
            aResult = KErrGeneral;
            }

        // Log completed data
        _LIT8( KNAF, "GBA NAF derivation, received Ks Ext NAF:" );
        iSIMLog->Log( ( TDesC8 )KNAF );
        for ( TInt i( 0 ); i < ( gbaNaf->iKsExtNaf ).Length(); i++ )
            {
            _LIT8( KNAF2, "%d" );
            iSIMLog->Log( ( TDesC8 )KNAF2, gbaNaf->iKsExtNaf[i] );
            }
        }

    else if ( ( EGbaNafDerAuthFail == iAuthenticationPurpose )
        || ( EGbaBootNafError == iAuthenticationPurpose ) )
        {
            // No need for comparison or actions it these cases.
        }

    else
        {
        _LIT8( KDefault, "CSIM::HandleGbaCompletion: no matching auth purpose" );
        iSIMLog->Log( ( TDesC8 )KDefault );
        aResult = KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
// CSIM::AuthenticateIMSData
// Authenticating IMS Data
// -----------------------------------------------------------------------------
//
TInt CSIM::AuthenticateIMSData( CStifItemParser& aItem )
    {
    _LIT8(KWriteONStore, "SIM::AuthenticateIMSData");
    iSIMLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC16 rand;
    TPtrC16 aunt;

    RMobilePhone::TImsAuthenticateDataV5 authenticationData;

    if ( KErrNone == aItem.GetNextString ( rand ))
        {
        authenticationData.iRAND.Copy( rand );
        }
    if ( KErrNone == aItem.GetNextString ( aunt ))
        {
        authenticationData.iAUTN.Copy( aunt );
        }

    //Create package
    CMmDataPackage package;
    package.PackData( &authenticationData );

    ret = HandleRequestL( EMobilePhoneIMSAuthenticate, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "AuthenticateIMSData request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "AuthenticateIMSData request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }


    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteAuthenticateIMSData
// Complete AuthenticateIMSData function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteAuthenticateIMSData
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteAuthenticateIMSData result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    RMobilePhone::TImsAuthenticateDataV5 authenticationData;
        aDataPackage->UnPackData( authenticationData );

    if( KErrNone == aResult )
        {
        _LIT8( KRES, "IMS: RES:" );
         iSIMLog->Log((TDesC8)KRES );
         TInt i = 0;
         for( i = 0; i < (authenticationData.iRES).Length(); i++)
            {
            _LIT8( KRES2, "%d" );
            iSIMLog->Log((TDesC8)KRES2, authenticationData.iRES[i] );
            }

         _LIT8( KCK, "IMS: CK:" );
         iSIMLog->Log((TDesC8)KCK );
         for( i = 0; i < (authenticationData.iCK).Length(); i++)
            {
            _LIT8( KCK2, "%d" );
            iSIMLog->Log((TDesC8)KCK2, authenticationData.iCK[i] );
            }
         _LIT8( KIK, "IMS: IK:" );
         iSIMLog->Log((TDesC8)KIK );
         for( i = 0; i < (authenticationData.iIK).Length(); i++)
            {
            _LIT8( KIK2, "%d" );
            iSIMLog->Log((TDesC8)KIK2, authenticationData.iIK[i] );
            }
        }
    else
        {
        _LIT8( KFail, "CompleteAuthenticateIMSData Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );

        _LIT8( KAUTS, "IMS: AUTS:" );
         iSIMLog->Log((TDesC8)KAUTS );
         TInt i = 0;
         for( i = 0; i < (authenticationData.iAUTS).Length(); i++)
            {
            _LIT8( KiAUTS2, "%d" );
            iSIMLog->Log((TDesC8)KiAUTS2, authenticationData.iAUTS[i] );
            }
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::GetIMSAuthorizationInfo
// Getting IMS Authorization Info
// -----------------------------------------------------------------------------
//
TInt CSIM::GetIMSAuthorizationInfo( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KWriteONStore, "SIM::GetIMSAuthorizationInfo");
    iSIMLog->Log((TDesC8)KWriteONStore);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    ret = HandleRequestL( EMobilePhoneAuthorizationInfoPhase1 );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetIMSAuthorizationInfo request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetIMSAuthorizationInfo request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }


    return ret;

    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetIMSAuthorizationInfo
// Complete GetIMSAuthorizationInfo function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetIMSAuthorizationInfo
        (
        TInt aResult,
        CMmDataPackage* /*aDataPackage*/
        )
    {
    _LIT8( KResult, "CompleteGetIMSAuthorizationInfo result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        _LIT8( KPass, "CompleteGetIMSAuthorizationInfo Passed: %d" );
        iSIMLog->Log((TDesC8)KPass, aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteGetIMSAuthorizationInfo Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSIM::GetALSBlockedStatus
// Getting ALS Blocked Status
// -----------------------------------------------------------------------------
//
TInt CSIM::GetALSBlockedStatus( CStifItemParser& aItem )
    {
    _LIT8(KAlsBlock, "SIM::GetALSBlockedStatus");
    iSIMLog->Log((TDesC8)KAlsBlock);

    TInt ret( KErrNotFound );

    // for validity checking
    if ( iHandleSpecifiedRequests )
        {
        TInt temp;
        aItem.GetNextInt( temp );
        iAlsBlockedStatus = ( RMmCustomAPI::TGetAlsBlockStatus )temp;
        }


    ret = HandleRequestL( ECustomGetAlsBlockedIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetALSBlockedStatus request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "GetALSBlockedStatus request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteGetALSBlockedStatus
// Complete GetALSBlockedStatus function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteGetALSBlockedStatus
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteGetALSBlockedStatus result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    RMmCustomAPI::TGetAlsBlockStatus aBlockStatus;
    aDataPackage->UnPackData ( aBlockStatus );

    if ( KErrNone == aResult )
        {
        if ( aBlockStatus == RMmCustomAPI::EBlockStatusActive )
            {
            _LIT8( KStatus, "Als Blocking status is - - - - EBlockStatusActive" );
            iSIMLog->Log((TDesC8)KStatus );
            }
        else if ( aBlockStatus == RMmCustomAPI::EBlockStatusInactive )
            {
            _LIT8( KStatus, "Als Blocking status is - - - - EBlockStatusInactive" );
            iSIMLog->Log((TDesC8)KStatus );
            }

        _LIT8( KPass, "CompleteGetALSBlockedStatus Passed: %d" );
        iSIMLog->Log((TDesC8)KPass, aResult );

        // for validity checking
        if ( iHandleSpecifiedRequests )
            {
            if ( aBlockStatus != iAlsBlockedStatus )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else if ( KErrNotFound == aResult )
        {
        _LIT8( KStatus, "Als Blocking status is - - - - EBlockStatusNotSupported" );
        iSIMLog->Log((TDesC8)KStatus);

        _LIT8( KPass, "CompleteGetALSBlockedStatus Passed: %d" );
        iSIMLog->Log((TDesC8)KPass, aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteGetALSBlockedStatus Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::SetALSBlockedStatus
// Setting ALS Blocked Status
// -----------------------------------------------------------------------------
//
TInt CSIM::SetALSBlockedStatus( CStifItemParser& aItem )
    {
    _LIT8(KSetAlsBlock, "SIM::SetALSBlockedStatus");
    iSIMLog->Log((TDesC8)KSetAlsBlock );

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TPtrC status;

    aItem.GetNextString ( status );

    RMmCustomAPI::TSetAlsBlock aBlockStatus;

    if ( _L("Activate") == status )
        {
        aBlockStatus = RMmCustomAPI::EActivateBlock;
        _LIT8( KActivate, "SetALSBlockedStatus = = EActivateBlock" );
        iSIMLog->Log((TDesC8)KActivate );
        }
    else if( _L("Deactivate") == status )
        {
        aBlockStatus = RMmCustomAPI::EDeactivateBlock;
        _LIT8( KDeactivate, "SetALSBlockedStatus = = EDeactivateBlock" );
        iSIMLog->Log((TDesC8)KDeactivate );
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected status not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    //Create package
    CMmDataPackage package;
    package.PackData( &aBlockStatus );

    ret = HandleRequestL( ECustomSetAlsBlockedIPC, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SetALSBlockedStatus request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "SetALSBlockedStatus request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteSetALSBlockedStatus
// Complete SetALSBlockedStatus function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteSetALSBlockedStatus
        (
        TInt aResult,
        CMmDataPackage* /*aDataPackage*/
        )
    {
    _LIT8( KResult, "CompleteSetALSBlockedStatus result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        _LIT8( KPass, "CompleteSetALSBlockedStatus Passed: %d" );
        iSIMLog->Log((TDesC8)KPass, aResult );
        }
    else
        {
        _LIT8( KFail, "CompleteSetALSBlockedStatus Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );

        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSIM::SendSIMRefreshNtf
// -----------------------------------------------------------------------------
//
TInt CSIM::SendSIMRefreshNtf( CStifItemParser& /*aItem*/ )
    {

    iSIMLog->Log(_L8("CSIM::SendSIMRefreshNtf"));
    iSendOngoing = ETrue;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteSendSIMRefreshNtf
// -----------------------------------------------------------------------------
//
void CSIM::CompleteSendSIMRefreshNtf( TInt aResult )
    {
    iSIMLog->Log(_L8("CSIM::CompleteSendSIMRefreshNtf, err=%d"),aResult);

    if ( iSendOngoing )
        {
        iSendOngoing = EFalse;
        iSIMLog->Log(_L8("CSIM::CompleteSendSIMRefreshNtf"));

        Signal( aResult );

        }
    }

// -----------------------------------------------------------------------------
// CSIM::CheckAlsPpSupport
// -----------------------------------------------------------------------------
//
TInt CSIM::CheckAlsPpSupport( CStifItemParser& aItem )
    {

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ECustomCheckAlsPpSupportIPC );

    // for validity checking
    if ( iHandleSpecifiedRequests )
        {
        TInt temp;
        aItem.GetNextInt( temp );
        iAlsSupport = ( RMmCustomAPI::TAlsSupport )temp;
        }

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "CheckAlsPpSupport request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "CheckAlsPpSupport request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;

    }


// -----------------------------------------------------------------------------
// CSIM::CompleteCheckAlsPpSupport
// -----------------------------------------------------------------------------
//
void CSIM::CompleteCheckAlsPpSupport( TInt aResult, CMmDataPackage* aDataPackage )
    {


    RMmCustomAPI::TAlsSupport alsSupport;


    _LIT8( KResult, "CompleteCheckAlsPpSupport result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if( KErrNone == aResult )
        {
        _LIT8( KPass, "CompleteCheckAlsPpSupport Passed: %d" );
        iSIMLog->Log((TDesC8)KPass, aResult );

        aDataPackage->UnPackData ( alsSupport );

        if( alsSupport == RMmCustomAPI::EAlsSupportOn )
            {
            iSIMLog->Log( _L8("ALS support on") );
            }
        else if( alsSupport == RMmCustomAPI::EAlsSupportOff )
            {
            iSIMLog->Log( _L8("ALS support off") );
            }
        // for validity checking
        if ( iHandleSpecifiedRequests )
            {
            if ( alsSupport != iAlsSupport )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CompleteCheckAlsPpSupport Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSIM::SetTestOngoingFlag
// Sets flag iTestOngoing on
// -----------------------------------------------------------------------------
//
TInt CSIM::SetTestOngoingFlag()
    {
    iTestOngoing = ETrue;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CSIM::CheckPbData
// Check that phonebook data is as expected
// -----------------------------------------------------------------------------
//
TInt CSIM::CheckPbData( CStorageInfoData* aPbData, TPhoneBookType aPbType )
    {
    TInt error( KErrNone );

    if ( TPhoneBookTypeAdn == aPbType )
        {
        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataAdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataAdn.iIsPhonebookInitializeFailed ||
             aPbData->iADNNumOfEntries != iPbStorageDataAdn.iXXXNumOfEntries ||
             aPbData->iADNTextLengthMax != iPbStorageDataAdn.iXXXTextLengthMax ||
             aPbData->iADNNumberLengthMax != iPbStorageDataAdn.iXXXNumberLengthMax  ||
             aPbData->iFDNNumOfEntries != iPbStorageDataFdn.iXXXNumOfEntries ||
             aPbData->iFDNNumberLengthMax != iPbStorageDataFdn.iXXXNumberLengthMax ||
             aPbData->iFDNTextLengthMax != iPbStorageDataFdn.iXXXTextLengthMax ||
             aPbData->iSDNNumOfEntries != iPbStorageDataSdn.iXXXNumOfEntries ||
             aPbData->iSDNNumberLengthMax != iPbStorageDataSdn.iXXXNumberLengthMax ||
             aPbData->iSDNTextLengthMax != iPbStorageDataSdn.iXXXTextLengthMax ||
             aPbData->iMBDNNumOfEntries != iPbStorageDataMbdn.iXXXNumOfEntries ||
             aPbData->iMBDNNumberLengthMax != iPbStorageDataMbdn.iXXXNumberLengthMax ||
             aPbData->iMBDNTextLengthMax != iPbStorageDataMbdn.iXXXTextLengthMax ||
             aPbData->iMSISDNNumOfEntries != iPbStorageDataMsisdn.iXXXNumOfEntries ||
             aPbData->iMSISDNNumberLengthMax != iPbStorageDataMsisdn.iXXXNumberLengthMax ||
             aPbData->iMSISDNTextLengthMax != iPbStorageDataMsisdn.iXXXTextLengthMax
             )
            {
            error = KErrGeneral;
            }
        if( _L("UICC") == iCardType )
            {
            if(
                    aPbData->iANRNumLengthMax != iStorageDataAnr.iXXXTextLengthMax ||
                    aPbData->iANRNumOfEntries != iStorageDataAnr.iXXXNumOfEntries ||
                    aPbData->iANRNumOfEntriesPerEntry != iStorageDataAnr.iXXXNumOfEntriesPerEntry  ||
                    aPbData->iEmailNumOfEntries != iStorageDataEmail.iXXXNumOfEntries ||
                    aPbData->iEmailNumOfEntriesPerEntry != iStorageDataEmail.iXXXNumOfEntriesPerEntry ||
                    aPbData->iEmailTextLengthMax != iStorageDataEmail.iXXXTextLengthMax ||
                    aPbData->iSNENumOfEntries != iStorageDataSne.iXXXNumOfEntries ||
                    aPbData->iSNENumOfEntriesPerEntry != iStorageDataSne.iXXXNumOfEntriesPerEntry ||
                    aPbData->iSNETextLengthMax != iStorageDataSne.iXXXTextLengthMax
            )
                {
                error = KErrGeneral;
                }
            }
        
        if( _L("ICC") == iCardType )
            {
            if(
                    aPbData->iVMBXNumOfEntries != iPbStorageDataVmbx.iXXXNumOfEntries ||
                    aPbData->iVMBXNumberLengthMax != iPbStorageDataVmbx.iXXXNumberLengthMax ||
                    aPbData->iVMBXTextLengthMax != iPbStorageDataVmbx.iXXXTextLengthMax 
              )
                {
                error = KErrGeneral;
                }
            }

        }
    else if ( TPhoneBookTypeFdn == aPbType )
        {
        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataFdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataFdn.iIsPhonebookInitializeFailed ||
             aPbData->iFDNNumOfEntries != iPbStorageDataFdn.iXXXNumOfEntries ||
             aPbData->iFDNTextLengthMax != iPbStorageDataFdn.iXXXTextLengthMax ||
             aPbData->iFDNNumberLengthMax != iPbStorageDataFdn.iXXXNumberLengthMax )
            {
            error = KErrGeneral;
            }
        }
    else if ( TPhoneBookTypeSdn == aPbType )
        {
        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataSdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataSdn.iIsPhonebookInitializeFailed ||
             aPbData->iFDNNumOfEntries != iPbStorageDataSdn.iXXXNumOfEntries ||
             aPbData->iFDNTextLengthMax != iPbStorageDataSdn.iXXXTextLengthMax ||
             aPbData->iFDNNumberLengthMax != iPbStorageDataSdn.iXXXNumberLengthMax )
            {
            error = KErrGeneral;
            }
        }
    else if ( TPhoneBookTypeMbdn == aPbType )
        {
        if ( aPbData->iIsPhonebookInitialized != iPbStorageDataMbdn.iIsPhonebookInitialized  ||
             aPbData->iIsPhonebookInitializeFailed != iPbStorageDataMbdn.iIsPhonebookInitializeFailed ||
             aPbData->iFDNNumOfEntries != iPbStorageDataMbdn.iXXXNumOfEntries ||
             aPbData->iFDNTextLengthMax != iPbStorageDataMbdn.iXXXTextLengthMax ||
             aPbData->iFDNNumberLengthMax != iPbStorageDataMbdn.iXXXNumberLengthMax )
            {
            error = KErrGeneral;
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CSIM::FdnDataSizesReq
// Reads FDN information
// -----------------------------------------------------------------------------
//
TInt CSIM::FdnDataSizesReq( CStifItemParser& aItem )
    {
    _LIT8(KPbFdnDataSizeReq, "SIM::FdnDataSizesReq");
    iSIMLog->Log((TDesC8)KPbFdnDataSizeReq);

    TInt ret( KErrNone );
    // store phonebook info
    TInt value;
    aItem.GetNextInt( value );
    iPbStorageDataFdn.iXXXNumOfEntries = value;
    aItem.GetNextInt( value );
    iPbStorageDataFdn.iXXXNumberLengthMax = value;
    aItem.GetNextInt( value );
    iPbStorageDataFdn.iXXXTextLengthMax = value;

    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    iPhoneBookName = KETelIccAdnPhoneBook;
    package.SetPhoneBookName( iPhoneBookName );

    ret = HandleRequestL( EMmTsyPhoneBookStoreInitIPC,
                         &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "PB init request send ok" );
        iSIMLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "PB init request send failed: %d" );
        iSIMLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteFdnDataSizesReq
// Completes FdnDataSizesReq function.
// -----------------------------------------------------------------------------
//
void CSIM::CompleteFdnDataSizesReq
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CompleteFdnDataSizesReq result: %d" );
    iSIMLog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        // Response information
        TPBFdnInfo pbFdnInfo;
        aDataPackage->UnPackData( pbFdnInfo );

        if ( iHandleSpecifiedRequests )
            {
            if ( iPbStorageDataFdn.iXXXNumOfEntries != pbFdnInfo.iFDNNumOfEntries ||
                 iPbStorageDataFdn.iXXXTextLengthMax != pbFdnInfo.iFDNTextLengthMax ||
                 iPbStorageDataFdn.iXXXNumberLengthMax != pbFdnInfo.iFDNNumberLengthMax )
                {
                aResult = KErrGeneral;
                }

            _LIT8( KFDNNumOfEntries, "FDNNumOfEntries: %d" );
            iSIMLog->Log((TDesC8)KFDNNumOfEntries, pbFdnInfo.iFDNNumOfEntries );
            _LIT8( KFDNTextLengthMax, "FDNTextLengthMax: %d" );
            iSIMLog->Log((TDesC8)KFDNTextLengthMax, pbFdnInfo.iFDNTextLengthMax );
            _LIT8( KFDNNumberLengthMax, "FDNNumberLengthMax: %d" );
            iSIMLog->Log((TDesC8)KFDNNumberLengthMax, pbFdnInfo.iFDNNumberLengthMax );
            }
        }
    else
        {
        _LIT8( KFail, "CompleteFdnDataSizesReq Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSIM::CheckServiceSupport
// Checks if service is supported in UICC.
// (other items was commented in header)
// -----------------------------------------------------------------------------
//
TInt CSIM::CheckServiceSupport( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSIM::CheckServiceSupport" );
    iSIMLog->Log( ( TDesC8 )KLog );

    TInt ret( KErrNone );
    TInt application( 0 );
    TInt appAvailable( 0 );

    aItem.GetNextInt( application );
    aItem.GetNextInt( appAvailable );

    RMmCustomAPI::TAppSupport appSupport;
    appSupport.iAppNum = application;

    if ( 0 == appAvailable )
        {
        iServiceAvailable = EFalse;
        }
    else if ( 1 == appAvailable )
        {
        iServiceAvailable = ETrue;
        }
    else
        {
        _LIT8( KLog, "CSIM::CheckServiceSupport: not supported availability %d" );
        iSIMLog->Log( ( TDesC8 )KLog, appAvailable );
        ret = KErrNotSupported;
        }

    if ( KErrNone == ret )
        {
        // Create package
        CMmDataPackage package;
        package.PackData( &appSupport );

        ret = HandleRequestL(
            ECustomGetServiceTableSupportbyApplicationIPC,
            &package );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteCheckServiceSupport
// Completes service support check request.
// (other items was commented in header)
// -----------------------------------------------------------------------------
//
void CSIM::CompleteCheckServiceSupport
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KLog, "CSIM::CompleteGetServiceSupport - result: %d" );
    iSIMLog->Log( ( TDesC8 )KLog, aResult );

    RMmCustomAPI::TAppSupport appSupport;

    if( KErrNone == aResult )
        {
        aDataPackage->UnPackData( appSupport );

        if ( iServiceAvailable == appSupport.iSupported )
            {
            iSIMLog->Log( _L8("Service Support received correctly") );
            }
        else
            {
            iSIMLog->Log( _L8("Service Support received NOT correclty") );
            }
        }
    else
        {
        _LIT8( KFail, "CompleteGetServiceSupport failed: %d" );
        iSIMLog->Log( ( TDesC8 )KFail, aResult );
        }

    Signal( aResult );
    }
// -----------------------------------------------------------------------------
// CSIM::SetExpectedNumberOfPbEntries
// Sets expected number of phonebook entries checked on IPC completion
// -----------------------------------------------------------------------------
//
TInt CSIM::SetExpectedNumberOfPbEntries( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSIM::SetExpectedNumberOfPbEntries" );
    iSIMLog->Log( ( TDesC8 )KLog );
    aItem.GetNextInt( iSavedPbEntriesCount );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteViagHomeZoneReadParam
// Performs verification of the data returned when
// EReadViagHomeZoneParamsIPC is completed
// -----------------------------------------------------------------------------
//
void CSIM::CompleteViagHomeZoneReadParam(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CSIM::CompleteViagHomeZoneReadParam - result: %d" );
    iSIMLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        RMmCustomAPI::TViagParams viagParams;
        RMmCustomAPI::TViagElements* viagElements( NULL );

        aDataPackage->UnPackData( viagParams, viagElements );

        // match parameters
        if ( viagParams.iScp != iSavedViagParamsForComparison.iScp
            || viagParams.iSmsC != iSavedViagParamsForComparison.iSmsC
            || viagParams.iSubscribedZoneAndVersion !=
                iSavedViagParamsForComparison.iSubscribedZoneAndVersion )
            {
            aResult = KErrGeneral;
            }

        // match elements
        for ( TInt i = 0;
            i < RMmCustomAPI::KViagElementCount && KErrNone == aResult;
            i++)
            {
            RMmCustomAPI::TViagElement& element =
                viagElements->At( i );
            RMmCustomAPI::TViagElement& expected =
                iSavedViagElementsForComparison->At( i );

            if ( element.iCoordinates.iX != expected.iCoordinates.iX
                || element.iCoordinates.iY != expected.iCoordinates.iY
                || element.iCoordinates.iR2 != expected.iCoordinates.iR2
                || element.iCoordinates.iZoneId != expected.iCoordinates.iZoneId
                || element.iName != expected.iName
                || element.iActiveFlag != expected.iActiveFlag )
                {
                aResult = KErrGeneral;
                }
            }
        }
    else
        {
        _LIT8( KFail, "CSIM::CompleteViagHomeZoneReadParam failed: %d" );
        iSIMLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteViagHomeZoneReadCache
// Performs verification of the data returned when
// EReadViagHomeZoneCacheIPC is completed
// -----------------------------------------------------------------------------
//
void CSIM::CompleteViagHomeZoneReadCache(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8( KLog, "CSIM::CompleteViagHomeZoneReadCache - result: %d" );
    iSIMLog->Log( ( TDesC8 )KLog, aResult );

    if ( KErrNone == aResult )
        {
        RMmCustomAPI::TViagCacheRecordContent record;

        aDataPackage->UnPackData( record );

        if ( iSavedRecordForComparison.iLac != record.iLac
            || iSavedRecordForComparison.iCellId != record.iCellId )
            {
            aResult = KErrGeneral;
            }
        }
    else
        {
        _LIT8( KFail, "CSIM::CompleteViagHomeZoneReadCache failed: %d" );
        iSIMLog->Log( ( TDesC8 )KFail, aResult );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }

    }

// -----------------------------------------------------------------------------
// CSIM::InvalidReadEntries
// Requests reading phonebook entry with invalid input parameters
// -----------------------------------------------------------------------------
//
TInt CSIM::InvalidReadEntries( CStifItemParser& aItem )
    {
    _LIT8( KLog, "CSIM::InvalidReadEntries" );
    iSIMLog->Log( ( TDesC8 )KLog );

    TInt ret( KErrNone );
    TPtrC string;
    TBool supported ( ETrue );

    aItem.GetNextString( string );

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if ( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else if ( _L("MBDN") == string )
        {
        iSIMLog->Log((TDesC8)KMBDN);
        iPhoneBookName = KETelIccMbdnPhoneBook;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        TAny* dummyPointer( NULL );
        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &dummyPointer, &dummyPointer );

        ret = HandleRequestL( EMmTsyPhoneBookStoreReadIPC,
                             &package );

        // we expect error
        Signal( ret );
        // to prevent testcase from termination
        ret = KErrNone;
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::InvalidWriteEntries
// Requests writing phonebook entry with invalid input parameters
// -----------------------------------------------------------------------------
//
TInt CSIM::InvalidWriteEntries( CStifItemParser& aItem )
    {
    _LIT8(KInvalidWriteEntries, "SIM::InvalidWriteEntries");
    iSIMLog->Log((TDesC8)KInvalidWriteEntries);

    TInt ret( KErrNone );
    TBool supported( ETrue );
    TPtrC string;

    aItem.GetNextString( string );

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else if( _L("MBDN") == string )
        {
        iSIMLog->Log((TDesC8)KMBDN);
        iPhoneBookName = KETelIccMbdnPhoneBook;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        TInt dummyIndex( 1 );
        TAny* dummyPointer( NULL );

        //Create package
        CPhoneBookDataPackage package;
        // Set package data
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &dummyIndex, &dummyPointer );

        ret = HandleRequestL( EMmTsyPhoneBookStoreWriteIPC,
                             &package );

        // we expect error
        Signal( ret );
        // to prevent testcase from termination
        ret = KErrNone;
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::InvalidPbStoreCache
// Requests cacheing phonebook entries with invalid input parameters
// -----------------------------------------------------------------------------
//
TInt CSIM::InvalidPbStoreCache( CStifItemParser& aItem )
    {
    _LIT8(KInvalidPbStoreCache, "SIM::InvalidPbStoreCache");
    iSIMLog->Log((TDesC8)KInvalidPbStoreCache);

    TInt ret( KErrNone );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString( string );

    if ( _L("Adn") == string )
        {
        iSIMLog->Log((TDesC8)KAdnPhoneBook);
        iPhoneBookName = KETelIccAdnPhoneBook;
        }
    else if ( _L("Fdn") == string )
        {
        iSIMLog->Log((TDesC8)KFdnPhoneBook);
        iPhoneBookName = KETelIccFdnPhoneBook;
        }
    else if ( _L("Sdn") == string )
        {
        iSIMLog->Log((TDesC8)KSdnPhoneBook);
        iPhoneBookName = KETelIccSdnPhoneBook;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected PB not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported )
        {
        TAny* dummyPointer( NULL );

        CPhoneBookDataPackage package;
        package.SetPhoneBookName( iPhoneBookName );
        package.PackData( &dummyPointer );

        ret = HandleRequestL( EMmTsyPhoneBookStoreCacheIPC,
                             &package );

        // we expect error
        Signal( ret );
        // to prevent testcase from termination
        ret = KErrNone;
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
// -----------------------------------------------------------------------------
// CSIM::DeleteAlphaString
// Delete AlphaString
// -----------------------------------------------------------------------------
//
TInt CSIM::DeleteAlphaString( CStifItemParser& aItem )
    {
    _LIT8(KSetAlsBlock, "SIM::DeleteAlphaString");
    iSIMLog->Log((TDesC8)KSetAlsBlock );

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TPtrC fileType;
    TInt index;

    aItem.GetNextString ( fileType );

    RMmCustomAPI::TAlphaStringParams alphaStringParams;

    if ( _L("AAS") == fileType )
        {
        alphaStringParams.iFileType = RMmCustomAPI::EAas;
        }
    else if ( _L("GAS") == fileType )
        {
        alphaStringParams.iFileType = RMmCustomAPI::EGas;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected status not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        if( KErrNone == aItem.GetNextInt ( index ))
            {
            alphaStringParams.iIndex = index;
            }

        //Create package
        CMmDataPackage package;
        package.PackData( &alphaStringParams );

        ret = HandleRequestL( ECustomDeleteAlphaStringIPC, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "DeleteAlphaString request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "DeleteAlphaString request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteDeleteAlphaString
// Complete WriteAlphaString
// -----------------------------------------------------------------------------
//
void CSIM::CompleteDeleteAlphaString
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8(KSetAlsBlock, "SIM::CompleteDeleteAlphaString");

    if( KErrNone == aResult )
        {
        RMmCustomAPI::TAlphaStringParams alphaStringParams;
        //Unpack package
        aDataPackage->UnPackData( alphaStringParams );

        _LIT8( KLocation, "Deleted Location = %d" );
        iSIMLog->Log((TDesC8)KLocation, alphaStringParams.iIndex  );

        if(RMmCustomAPI::EAas == alphaStringParams.iFileType)
            {
            _LIT8( KTypeAAS, "Type: AAS" );
            iSIMLog->Log((TDesC8)KTypeAAS );
            }
        else
            {
            _LIT8( KTypeGAS, "Type: GAS" );
            iSIMLog->Log((TDesC8)KTypeGAS );
            }
        }
    else
        {
        _LIT8( KFail, "CompleteDeleteAlphaString Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSIM::WriteAlphaString
//
// -----------------------------------------------------------------------------
//
TInt CSIM::WriteAlphaString( CStifItemParser& aItem )
    {
    _LIT8(KSetAlsBlock, "SIM::WriteAlphaString");
    iSIMLog->Log((TDesC8)KSetAlsBlock );

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TPtrC fileType;
    TInt index;
    TPtrC16 name;

    aItem.GetNextString ( fileType );

    RMmCustomAPI::TAlphaStringParams alphaStringParams;

    if ( _L("AAS") == fileType )
        {
        alphaStringParams.iFileType = RMmCustomAPI::EAas;
        }
    else if ( _L("GAS") == fileType )
        {
        alphaStringParams.iFileType = RMmCustomAPI::EGas;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected status not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        if( KErrNone == aItem.GetNextInt ( index ))
            {
            alphaStringParams.iIndex = index;
            }

        if( KErrNone == aItem.GetNextString ( name ))
            {
            alphaStringParams.iAlphaString.Copy( name );
            }

        //Create package
        CMmDataPackage package;
        package.PackData( &alphaStringParams );

        ret = HandleRequestL( ECustomWriteAlphaStringIPC, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "WriteAlphaString request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "WriteAlphaString request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteWriteAlphaString
// Complete WriteAlphaString
// -----------------------------------------------------------------------------
//
void CSIM::CompleteWriteAlphaString
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {

    _LIT8(KSetAlsBlock, "SIM::CompleteWriteAlphaString");

    if( KErrNone == aResult )
        {
        RMmCustomAPI::TAlphaStringParams alphaStringParams;
        //Unpack package
        aDataPackage->UnPackData( alphaStringParams );

        _LIT8( KLocation, "Location = %d" );
        iSIMLog->Log((TDesC8)KLocation, alphaStringParams.iIndex  );

        if(RMmCustomAPI::EAas == alphaStringParams.iFileType)
            {
            _LIT8( KTypeAAS, "Type: AAS" );
            iSIMLog->Log((TDesC8)KTypeAAS );
            }
        else
            {
            _LIT8( KTypeGAS, "Type: GAS" );
            iSIMLog->Log((TDesC8)KTypeGAS );
            }

        _LIT8( KAasString, "AasString:" );
        iSIMLog->Log((TDesC8)KAasString  );
        iSIMLog->Log( alphaStringParams.iAlphaString  );
        }
    else
        {
        _LIT8( KFail, "CompleteWriteAlphaString Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSIM::ReadAlphaString
//
// -----------------------------------------------------------------------------
//
TInt CSIM::ReadAlphaString( CStifItemParser& aItem )
    {
    _LIT8(KSetAlsBlock, "SIM::ReadAlphaString");
    iSIMLog->Log((TDesC8)KSetAlsBlock );

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TPtrC fileType;
    TInt index;

    aItem.GetNextString ( fileType );

    RMmCustomAPI::TAlphaStringParams alphaStringParams;

    if ( _L("AAS") == fileType )
        {
        alphaStringParams.iFileType = RMmCustomAPI::EAas;
        }
    else if ( _L("GAS") == fileType )
        {
        alphaStringParams.iFileType = RMmCustomAPI::EGas;
        }
    else
        {
         _LIT8(KNotSupp, "SIM::Selected status not supported");
        iSIMLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        if( KErrNone == aItem.GetNextInt ( index ))
            {
            alphaStringParams.iIndex = index;
            }

        //Create package
        CMmDataPackage package;
        package.PackData( &alphaStringParams );

        ret = HandleRequestL( ECustomReadAlphaStringIPC, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "ReadAlphaString request send ok" );
            iSIMLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "ReadAlphaString request send failed: %d" );
            iSIMLog->Log((TDesC8)KSendFailed, ret );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSIM::CompleteReadAlphaString
// Complete ReadAlphaString
// -----------------------------------------------------------------------------
//
void CSIM::CompleteReadAlphaString
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8(KSetAlsBlock, "SIM::CompleteReadAlphaString");

    if( KErrNone == aResult )
        {
        RMmCustomAPI::TAlphaStringParams alphaStringParams;
        //Unpack package
        aDataPackage->UnPackData( alphaStringParams );

        _LIT8( KLocation, "Location = %d" );
        iSIMLog->Log((TDesC8)KLocation, alphaStringParams.iIndex  );

        if(RMmCustomAPI::EAas == alphaStringParams.iFileType)
            {
            _LIT8( KTypeAAS, "Type: AAS" );
            iSIMLog->Log((TDesC8)KTypeAAS );
            }
        else
            {
            _LIT8( KTypeGAS, "Type: GAS" );
            iSIMLog->Log((TDesC8)KTypeGAS );
            }

        _LIT8( KAasString, "AasString: " );
        iSIMLog->Log((TDesC8)KAasString  );
        iSIMLog->Log( alphaStringParams.iAlphaString  );
        }
    else
        {
        _LIT8( KFail, "CompleteReadAlphaString Failed: %d" );
        iSIMLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
