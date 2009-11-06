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
* Description:  
*
*/





// INCLUDE FILES
#include <stiftestinterface.h>
#include "sms.h"
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#endif


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSMS::CSMS
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSMS::CSMS(
    CTestModuleIf& aTestModuleIf ):
        CCore( aTestModuleIf ),
        iMsgAttribPckg( iMsgAttrib ),
        iSmsCapsPckg (iSmsCaps)
    {
    }

// -----------------------------------------------------------------------------
// CSMS::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSMS::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif
    iSMSLog = CStifLogger::NewL( KSMSLogPath,
                          KSMSLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    _LIT8(SMS, "SMS: ConstructL");
    iSMSLog->Log((TDesC8)SMS );

    iSMSMsgStorageRequestOngoing = EFalse;
    iSmsParameterCheck = EFalse;
    iTestCaseNumber = ETestCaseNotDefined;

    iSmspListArray = new ( ELeave ) CArrayPtrFlat<TSmsParameters>( 1 );
    InitializeIscControlL(KIscControlTestCaseFileSMS);
    }

// -----------------------------------------------------------------------------
// CSMS::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSMS* CSMS::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CSMS* self = new (ELeave) CSMS( aTestModuleIf );

    CleanupStack::PushL( (CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CSMS::~CSMS
// Destructor.
// -----------------------------------------------------------------------------
//
CSMS::~CSMS()
    {
    // Delete resources allocated from test methods
    Delete();
    // Delete logger
    delete iSMSLog;
    }


// -----------------------------------------------------------------------------
// CCore::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CSMS::RouteCompletion(
    TInt aIpc, //IPC value
    CMmDataPackage* aDataPackage,//pointer to DataPackage
    TInt aResult // Completion result
    )
    {
    _LIT8(KResult, "CSMS::RouteCompletion result: %d");
    iSMSLog->Log((TDesC8)KResult, aResult );

    if ( !iHandleSpecifiedRequests )
        {
        switch( aIpc )
            {
            case EMobileSmsMessagingSendMessage:
                {
                CompleteSendSMSMessage( aResult );
                break;
                }
            case EMobilePhoneStoreRead:
                {
                CompleteReadSMSMessage( aResult,aDataPackage );
                break;
                }
            case EMobilePhoneStoreDelete:
                {
                CompleteDeleteSMSMessage( aResult );
                break;
                }
            case EMobilePhoneStoreDeleteAll:
                {
                CompleteDeleteAllSMSMessages( aResult );
                break;
                }
            case EMobileSmsMessagingSetMoSmsBearer:
                {
                CompleteSetMoSmsBearer( aResult );
                break;
                }
            case EMobilePhoneStoreWrite:
                {
                CompleteWriteSMSMessage( aResult,aDataPackage );
                break;
                }
            case EMobileSmsMessagingReceiveMessage:
                {
                CompleteReceiveSMSMessage( aResult, aDataPackage );
                break;
                }
            case EMmTsyActivateSmsRouting:
                {
                CompleteActivateSMSRouting( aResult );
                break;
                }
            case EMobileSmsMessagingAckSmsStored :
                {
                CompleteAckSMS( aResult );
                break;
                }
            case EMmTsyDeactivateSmsRouting :
                {
                CompleteDeactivateSMSRouting( aResult );
                break;
                }
            case EMobileSmsMessagingResumeSmsReception :
                {
                CompleteResumeSMS ( aResult );
                break;
                }
            case EMobilePhoneStoreReadAllPhase1 :
                {
                CompleteReadAllSMSMessage ( aResult, aDataPackage );
                break;
                }
            case EMobileSmsMessagingNackSmsStored:
                {
                CompleteNackSMS( aResult );
                break;
                }
            case EMobileSmsMessagingGetMessageStoreInfo:
                {
                CompleteSIMMsgStorageInfo( aResult,aDataPackage );
                break;
                }
            case EMobileSmsMessagingGetSmspListPhase1:
                {
                CompleteReadSmsParameters( aResult, aDataPackage );
                break;
                }
            case EMobileSmsMessagingStoreSmspList:
                {
                CompleteStoreSmsParameters( aResult );
                break;
                }
            default:
                break;
            }
        }
    else
        {
        TBool isIPCHandled = EFalse;

        switch(aIpc)
            {
            case EMobileSmsMessagingGetMessageStoreInfo:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingGetMessageStoreInfo" ) ) )
                    {
                    CompleteSIMMsgStorageInfo( aResult,aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyBootNotifySimStatusReadyIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMmTsyBootNotifySimStatusReadyIPC" ) ) )
                    {
                    CompleteSMSCacheInitialization( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneStoreGetInfo:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneStoreGetInfo" ) ) )
                    {
                    CompleteMobilePhoneStoreGetInfo( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneStoreWrite:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneStoreWrite" ) ) )
                    {
                    CompleteWriteSMSMessage( aResult,aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyActivateSmsRouting:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMmTsyActivateSmsRouting" ) ) )
                    {
                    CompleteActivateSMSRouting( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingReceiveMessage:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingReceiveMessage" ) ) )
                    {
                    CompleteReceiveSMSMessage( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingAckSmsStored:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingAckSmsStored" ) ) )
                    {
                    CompleteAckSMS( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMmTsyDeactivateSmsRouting:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMmTsyDeactivateSmsRouting" ) ) )
                    {
                    CompleteDeactivateSMSRouting( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingSendMessage:
            case EMmTsySmsSendSatMessage:
            case EMobileSmsMessagingSendMessageNoFdnCheck:
                {
                if ( ( 0 == iSavedIPCNameForComparison.Compare
                         ( _L( "EMobileSmsMessagingSendMessage" ) ) ) ||
                     ( 0 == iSavedIPCNameForComparison.Compare
                         ( _L( "EMmTsySmsSendSatMessage" ) ) ) ||
                     ( 0 == iSavedIPCNameForComparison.Compare
                         ( _L( "EMobileSmsMessagingSendMessageNoFdnCheck" ) ) ) )
                    {
                    // Check parameters from data package
                    if ( iSmsParameterCheck )
                        {
                        CompleteSendSMSMessage( aResult, aDataPackage );
                        iSmsParameterCheck = EFalse;
                        }
                    else
                        {
                        CompleteSendSMSMessage( aResult );
                        }
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingGetSmspListPhase1:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingGetSmspListPhase1" ) ) )
                    {
                    CompleteReadSmsParameters( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneStoreReadAllPhase1:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneStoreReadAllPhase1" ) ) )
                    {
                    CompleteReadAllSMSMessage ( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneStoreRead:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneStoreRead" ) ) )
                    {
                    CompleteReadSMSMessage( aResult,aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneStoreDelete:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneStoreDelete" ) ) )
                    {
                    CompleteDeleteSMSMessage( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobilePhoneStoreDeleteAll:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobilePhoneStoreDeleteAll" ) ) )
                    {
                    CompleteDeleteAllSMSMessages( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingSetMoSmsBearer:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingSetMoSmsBearer" ) ) )
                    {
                    CompleteSetMoSmsBearer( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingStoreSmspList:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingStoreSmspList" ) ) )
                    {
                    CompleteStoreSmsParameters( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingNackSmsStored:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingNackSmsStored" ) ) )
                    {
                    CompleteNackSMS( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            case EMobileSmsMessagingResumeSmsReception:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                        ( _L( "EMobileSmsMessagingResumeSmsReception" ) ) )
                    {
                    CompleteResumeSMS ( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }
            default:
                {
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

    return ( CScriptBase* ) CSMS::NewL( aTestModuleIf );

    }

//  End of File
