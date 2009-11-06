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




// INCLUDE FILES
#include <stiftestinterface.h>
#include "sim.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIM::CSIM
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIM::CSIM(
    CTestModuleIf& aTestModuleIf ):
        CCore( aTestModuleIf )
    {
    // Initial values
    iSavedPbEntriesCount = -1;
    iPBStoreCacheAdn = NULL;
    iPBStoreCacheFdn = NULL;
    iPBStoreCacheSdn = NULL;
    iIndexAndEntries = NULL;
    iPhoneBookEntry = NULL;
    iPBStoreReadEntries = NULL;
    iENStoreArray = NULL;
    iSavedViagElementsForComparison = NULL;
    }

// -----------------------------------------------------------------------------
// CSIM::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIM::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iSIMLog = CStifLogger::NewL( KSIMLogPath,
                          KSIMLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    _LIT8(KSIM, "CSIM: ConstructL");
    iSIMLog->Log((TDesC8)KSIM );

    if ( NULL == iPBStoreCacheAdn )
       {
        iPBStoreCacheAdn = new ( ELeave )
            CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
       }
    if ( NULL == iPBStoreCacheFdn )
       {
        iPBStoreCacheFdn = new ( ELeave )
            CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
       }
    if ( NULL == iPBStoreCacheSdn )
       {
        iPBStoreCacheSdn = new ( ELeave )
            CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
       }

    if ( NULL == iPBStoreReadEntries )
       {
        iPBStoreReadEntries = new ( ELeave )
            CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
       }
   if ( NULL == iIndexAndEntries )
       {
       iIndexAndEntries = new (ELeave)
                          RMobilePhoneBookStore::TPBIndexAndNumEntries;
       }

    InitializeIscControlL(KIscControlTestCaseFileSIM);

    iTestOngoing = EFalse;
    iPbType[0] =  TPhoneBookTypeUnknown;
    iPbType[1] =  TPhoneBookTypeUnknown;

    }

// -----------------------------------------------------------------------------
// CSIM::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIM* CSIM::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CSIM* self = new (ELeave) CSIM( aTestModuleIf );

    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CSIM::~CSIM()
    {

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iSIMLog;

    if ( iPBStoreCacheAdn )
        {
        delete iPBStoreCacheAdn;
        iPBStoreCacheAdn = NULL;
        }
    if ( iPBStoreCacheFdn )
        {
        delete iPBStoreCacheFdn;
        iPBStoreCacheFdn = NULL;
        }

    // Delete phonebook write entry
    if ( iPhoneBookEntry )
        {
        delete iPhoneBookEntry;
        iPhoneBookEntry = NULL;
        }

   if ( iIndexAndEntries )
        {
        delete iIndexAndEntries;
        iIndexAndEntries = NULL;
        }

    if ( iPBStoreReadEntries )
        {
        delete iPBStoreReadEntries;
        iPBStoreReadEntries = NULL;
        }

    if ( iSavedViagElementsForComparison )
        {
        delete iSavedViagElementsForComparison;
        iSavedViagElementsForComparison = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCore::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIM::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,
        TInt aResult
        )
    {
    // Process the ISI msg according to IPC requested.
    _LIT8(KResult, "CSIM::RouteCompletion result: %d, IPC: %d");
    iSIMLog->Log((TDesC8)KResult, aResult, aIpc );
    TBool isIPCHandled = EFalse;
    switch(aIpc)
        {
        case EMmTsyPhoneBookStoreInitIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreInitIPC" ) ) )
                {
                CompletePbInit( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneBookStoreCacheIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreCacheIPC" ) ) )
                {
                CompletePbStoreCache( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneBookStoreReadIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreReadIPC" ) ) )
                {
                CompleteReadEntries( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneBookStoreWriteIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreWriteIPC" ) ) )
                {
                CompleteWriteEntries( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneBookStoreDeleteIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreDeleteIPC" ) ) )
                {
                CompleteDelete( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneBookStoreDeleteAllIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreDeleteAllIPC" ) ) )
                {
                CompleteDelete( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneBookStoreGetInfoIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreGetInfoIPC" ) ) )
                {
                CompleteGetPbStoreInfo( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyENStoreReadIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyENStoreReadIPC" ) ) )
                {
                CompleteReadENStore( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyENStoreReadAllPhase1IPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyENStoreReadAllPhase1IPC" ) ) )
                {
                CompleteReadENStoreAll( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyENStoreGetInfoIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyENStoreGetInfoIPC" ) ) )
                {
                CompleteGetENStoreInfo( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreReadIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyONStoreReadIPC" ) ) )
                {
                CompleteReadONStore( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreReadEntryIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyONStoreReadEntryIPC" ) ) )
                {
                CompleteReadONStoreEntry( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreGetInfoIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyONStoreGetInfoIPC" ) ) )
                {
                CompleteGetONStoreInfo( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreDeleteIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyONStoreDeleteIPC" ) ) )
                {
                CompleteDeleteONStoreEntry( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreDeleteAllIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EMmTsyONStoreDeleteAllIPC" ) ) )
                {
                CompleteDeleteONStoreEntry( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreWriteIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EMmTsyONStoreWriteIPC" ) ) )
                {
                CompleteWriteONStore( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreReadSizeIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EMmTsyONStoreReadSizeIPC" ) ) )
                {
                CompleteReadONStoreSize( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneGetMailboxNumbers:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetMailboxNumbers" ) ) )
                {
                CompleteGetMailboxNumbers( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreWriteEntryIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EMmTsyONStoreWriteEntryIPC" ) ) )
                {
                CompleteWriteONStoreAll( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneGetServiceTable:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EMobilePhoneGetServiceTable" ) ) )
                {
                CompleteGetServiceTable( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyONStoreWriteSizeIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyONStoreWriteSizeIPC" ) ) )
                {
                CompleteOnStoreWriteSize( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneGetALSLine:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetALSLine" ) ) )
                {
                CompleteGetALSLine( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneSetALSLine:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneSetALSLine" ) ) )
                {
                CompleteSetALSLine( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneGetCustomerServiceProfile:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetCustomerServiceProfile" ) ) )
                {
                CompleteGetCustomerServiceProfile( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneGetPin1DisableSupportedIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneGetPin1DisableSupportedIPC" ) ) )
                {
                CompleteGetPin1DisableSupported( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsySimRefreshRegisterIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsySimRefreshRegisterIPC" ) ) )
                {
                CompleteSimRefreshRegister( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsySimRefreshDoneIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsySimRefreshDoneIPC" ) ) )
                {
                CompleteSimRefreshDone( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneGetSubscriberId:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetSubscriberId" ) ) )
                {
                CompleteGetSubscriberId( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneGetServiceProviderName:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneGetServiceProviderName" ) ) )
                {
                CompleteGetServiceProviderName( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomReadSimFileIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "ECustomReadSimFileIPC" ) ) )
                {
                CompleteReadSimFile( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomGetNetworkProviderNameIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomGetNetworkProviderNameIPC" ) ) )
                {
                CompleteGetNetworkProviderName( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomGetSimAuthenticationDataIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "ECustomGetSimAuthenticationDataIPC" ) ) )
                {
                CompleteGetSimAuthenticationData( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneIMSAuthenticate:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneIMSAuthenticate" ) ) )
                {
                CompleteAuthenticateIMSData( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMobilePhoneAuthorizationInfoPhase1:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobilePhoneAuthorizationInfoPhase1" ) ) )
                {
                CompleteGetIMSAuthorizationInfo( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomGetAlsBlockedIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomGetAlsBlockedIPC" ) ) )
                {
                CompleteGetALSBlockedStatus( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomSetAlsBlockedIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomSetAlsBlockedIPC" ) ) )
                {
                CompleteSetALSBlockedStatus( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsySimRefreshNowIPC :
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsySimRefreshNowIPC" ) ) )
                {
                CompleteSendSIMRefreshNtf( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomCheckAlsPpSupportIPC :
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomCheckAlsPpSupportIPC" ) ) )
                {
                CompleteCheckAlsPpSupport( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EMmTsyPhoneBookStoreFdnInfoIPC :
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyPhoneBookStoreFdnInfoIPC" ) ) )
                {
                CompleteFdnDataSizesReq( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomGetServiceTableSupportbyApplicationIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomGetServiceTableSupportbyApplication" ) ) )
                {
                CompleteCheckServiceSupport( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        case ECustomWriteAlphaStringIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomWriteAlphaStringIPC" ) ) )
                {
                CompleteWriteAlphaString( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomReadAlphaStringIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomReadAlphaStringIPC" ) ) )
                {
                CompleteReadAlphaString( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomDeleteAlphaStringIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomDeleteAlphaStringIPC" ) ) )
                {
                CompleteDeleteAlphaString( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        case EReadViagHomeZoneParamsIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EReadViagHomeZoneParamsIPC" ) ) )
                {
                CompleteViagHomeZoneReadParam( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EReadViagHomeZoneCacheIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EReadViagHomeZoneCacheIPC" ) ) )
                {
                CompleteViagHomeZoneReadCache( aResult, aDataPackage );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EWriteViagHomeZoneCacheIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EWriteViagHomeZoneCacheIPC" ) ) )
                {
                Signal( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case EWriteViagHomeZoneUHZIUESettingsIPC:
            {
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EWriteViagHomeZoneUHZIUESettingsIPC" ) ) )
                {
                Signal( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }
        case ECustomNotifySimCardStatusIPC:
            {
            _LIT8(KDefault, "CSIM::RouteCompletion ECustomNotifySimCardStatusIPC: result: %d");
            iSIMLog->Log((TDesC8)KDefault, aResult );
            //Signal( aResult );
            //CCore::GetNextExpectedCompleteIPC();
            isIPCHandled = ETrue;
            break;
            }
            
        case EMmTsyBootNotifySimStatusReadyIPC:
            {
            _LIT8(KDefault, "CSIM::RouteCompletion EMmTsyBootNotifySimStatusReadyIPC: result: %d");
            iSIMLog->Log((TDesC8)KDefault, aResult );
            if ( 0 == iSavedIPCNameForComparison.Compare
                ( _L( "EMmTsyBootNotifySimStatusReadyIPC" ) ) )
                {
                Signal( aResult );
                CCore::GetNextExpectedCompleteIPC();
                isIPCHandled = ETrue;
                }
            break;
            }  
        default:
            {
            _LIT8(KDefault, "CSIM::RouteCompletion default, IPC: %d");
            iSIMLog->Log((TDesC8)KDefault, aIpc );
            break;
            }
        }

    // Check whether the IPC received should had been exactly the expected one.
    // If not the KErrGeneral is set.
    if( ( iNextIPCMustBeSameAsNextExpected ) &&
        ( !isIPCHandled ) )
        {
        aResult = KErrUnknown;
        Signal( aResult );
        }

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CSIM::NewL( aTestModuleIf );

    }

//  End of File
