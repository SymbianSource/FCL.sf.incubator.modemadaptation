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
* Description:   Specific class for PDI tests
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "pdi.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPDI::CPDI
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPDI::CPDI
        (
        CTestModuleIf& aTestModuleIf
        ) : CCore( aTestModuleIf )
    {
    // Initial values
    }

// -----------------------------------------------------------------------------
// CPDI::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPDI::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iPDILog = CStifLogger::NewL(
        KSSLogPath,
        KSSLogFile,
        CStifLogger::ETxt,
        CStifLogger::EFile,
        EFalse );

    InitializeIscControlL( KIscControlTestCaseFilePDI );

    _LIT8( KPDI, "CPDI: ConstructL" );
    iPDILog->Log( ( TDesC8 ) KPDI );

    }

// -----------------------------------------------------------------------------
// CPDI::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPDI* CPDI::NewL
        (
        CTestModuleIf& aTestModuleIf
        )
    {
    CPDI* self = new( ELeave ) CPDI( aTestModuleIf );

    CleanupStack::PushL( ( CScriptBase* ) self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPDI::~CPDI
// Destructor
// -----------------------------------------------------------------------------
//
CPDI::~CPDI()
    {
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iPDILog;
    }

// -----------------------------------------------------------------------------
// CPDI::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CPDI::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,
        TInt aResult
        )
    {
    // Process the ISI msg according to IPC requested.
    _LIT8( KResult, "CPDI::RouteCompletion, IPC: %d, result: %d" );
    iPDILog->Log( ( TDesC8 ) KResult, aIpc, aResult );

    if ( !iHandleSpecifiedRequests )
        {
        switch(aIpc)
            {
            case EMobilePhoneNotifyCurrentNetworkChange:
                {
                CompleteNotifyCurrentNetworkChange( aResult, aDataPackage );
                break;
                }
            case EPacketGetNtwkRegStatus:
                {
                CompleteGetNetworkRegistrationStatus( aResult, aDataPackage );
                break;
                }
            case EPacketNotifyChangeOfNtwkRegStatus:
                {
                NotifyChangeOfNtwkRegStatus( aResult, aDataPackage );
                break;
                }
            case EPacketDetach:
                {
                CompletePacketServiceDetach( aResult );
                break;
                }
            case EPacketNotifyStatusChange:
                {
                PacketNotifyStatusChange( aResult, aDataPackage );
                break;
                }
            case EPacketRejectActivationRequest:
                {
                CompletePacketServiceRejectActivationRequest( aResult );
                break;
                }
            case EPacketGetAttachMode:
                {
                CompleteGetPacketAttachMode( aResult, aDataPackage );
                break;
                }
            case EPacketSetAttachMode:
                {
                CompleteSetPacketServiceAttachMode( aResult );
                break;
                }
            case EPacketSetDefaultContextParams:
                {
                CompleteSetPacketServiceAttachMode( aResult );
                break;
                }
            case EPacketSetPrefBearer:
                {
                CompleteSetPreferredBearer( aResult );
                break;
                }
            case EPacketContextInitialiseContext:
                {
                CompleteInitializePacketContext( aResult );
                break;
                }
            case EPacketAttach:
                {
                CompletePacketServiceAttach( aResult );
                break;
                }
            case EPacketContextActivate:
                {
                CompleteActivatePacketContext( aResult, aDataPackage );
                break;
                }
            case EPacketContextSetConfig:
                {
                CompleteSetPacketContextConfiguration( aResult/*, aDataPackage */ );
                break;
                }
            case EPacketQoSSetProfileParams:
                {
                CompleteSetProfileQoSParams( aResult/*, aDataPackage */ );
                break;
                }
            case EPacketContextGetDataVolumeTransferred:
                {
                CompleteGetDataVolumeTransferred( aResult, aDataPackage );
                break;
                }
            case EPacketContextNotifyDataTransferred:
                {
                CompleteGetDataVolumeTransferred( aResult, aDataPackage );
                break;
                }
            case EPacketContextDeactivate:
                {
                CompleteDeactivatePacketContext( aResult, aDataPackage );
                break;
                }
            case EPacketContextDelete:
                {
                CompleteDeletePacketContext( aResult/*, aDataPackage*/ );
                break;
                }
            case EPacketContextModifyActiveContext:
                {
                CompleteModifyPacketContext( aResult/*, aDataPackage*/ );
                break;
                }
            case ECustomNotifySsNetworkEventIPC:
                {
                CompleteGetNetworkRegistrationStatus( aResult, aDataPackage );
                break;
                }
            case EPacketNotifyTransferCapsIPC:
                {
                CompleteTransferCapsChange( aResult, aDataPackage );
                break;
                }
            case EPacketContextNotifyStatusChange:
                {
                CompletePacketContextNotifyStatusChange( aResult, aDataPackage );
                break;
                }
            case EPacketContextNotifyConfigChanged:
                {
                CompletePacketContextNotifyConfigChanged( aResult, aDataPackage );
                break;
                }
            case EMobilePhoneGetHomeNetwork:
                {
                CompleteGetHomeNetwork(aResult, aDataPackage );
                break;
                }
            default:
                {
                _LIT8( KDefault, "CPDI::RouteCompletion default" );
                iPDILog->Log( ( TDesC8 ) KDefault );
                break;
                }
            }
        }
    else
        {
        TBool isIPCHandled = EFalse;

        switch(aIpc)
            {
            case EPacketNotifyTransferCapsIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketNotifyTransferCapsIPC" ) ) )
                    {
                    CompleteTransferCapsChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketGetNtwkRegStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketGetNtwkRegStatus" ) ) )
                    {
                    CompleteGetNetworkRegistrationStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketNotifyChangeOfNtwkRegStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketNotifyChangeOfNtwkRegStatus" ) ) )
                    {
                    NotifyChangeOfNtwkRegStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }


            case EPacketNotifyDynamicCapsChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketNotifyDynamicCapsChange" ) ) )
                    {
                    CompleteNotifyDynamicCapsChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketNotifyMSClassChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketNotifyMSClassChange" ) ) )
                    {
                    CompleteNotifyMSClassChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketRejectActivationRequest:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketRejectActivationRequest" ) ) )
                    {
                    CompletePacketServiceRejectActivationRequest( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketGetAttachMode:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketGetAttachMode" ) ) )
                    {
                    CompleteGetPacketAttachMode( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketSetAttachMode:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketSetAttachMode" ) ) )
                    {
                    CompleteSetPacketServiceAttachMode( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketSetPrefBearer:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketSetPrefBearer" ) ) )
                    {
                    CompleteSetPreferredBearer( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketAttach:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketAttach" ) ) )
                    {
                    CompletePacketServiceAttach( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketNotifyContextAdded:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketNotifyContextAdded" ) ) )
                    {
                    CompletePacketNotifyContextAdded( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextInitialiseContext:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextInitialiseContext" ) ) )
                    {
                    CompleteInitializePacketContext( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextSetConfig:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextSetConfig" ) ) )
                    {
                    CompleteSetPacketContextConfiguration( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketSetDefaultContextParams:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketSetDefaultContextParams" ) ) )
                    {
                    CompleteSetPacketServiceAttachMode( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketQoSSetProfileParams:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketQoSSetProfileParams" ) ) )
                    {
                    CompleteSetProfileQoSParams( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextActivate:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextActivate" ) ) )
                    {
                    CompleteActivatePacketContext( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketQoSNotifyProfileChanged:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketQoSNotifyProfileChanged" ) ) )
                    {
                    CompletePacketQoSNotifyProfileChanged( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextNotifyConfigChanged:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextNotifyConfigChanged" ) ) )
                    {
                    CompletePacketContextNotifyConfigChanged( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextNotifyConnectionSpeedChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextNotifyConnectionSpeedChange" ) ) )
                    {
                    CompletePacketContextNotifyConnectionSpeedChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextGetDataVolumeTransferred:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextGetDataVolumeTransferred" ) ) )
                    {
                    CompleteGetDataVolumeTransferred( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextDeactivate:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextDeactivate" ) ) )
                    {
                    CompleteDeactivatePacketContext( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextNotifyStatusChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextNotifyStatusChange" ) ) )
                    {
                    CompletePacketContextNotifyStatusChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketDetach:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketDetach" ) ) )
                    {
                    CompletePacketServiceDetach( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextDelete:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextDelete" ) ) )
                    {
                    CompleteDeletePacketContext( aResult/*, aDataPackage*/ );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneNotifyCurrentNetworkChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyCurrentNetworkChange" ) ) )
                    {
                    CompleteNotifyCurrentNetworkChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketNotifyStatusChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketNotifyStatusChange" ) ) )
                    {
                    CompletePacketNotifyStatusChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EPacketContextModifyActiveContext:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EPacketContextModifyActiveContext" ) ) )
                    {
                    CompleteModifyPacketContext( aResult/*, aDataPackage*/ );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            case EMobilePhoneGetHomeNetwork:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetHomeNetwork" ) ) )
                    {
                    CompleteGetHomeNetwork(aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    isIPCHandled = ETrue;
                    }
                break;
                }

            default:
                {
                _LIT8( KDefault, "CPDI::RouteCompletion default" );
                iPDILog->Log( ( TDesC8 ) KDefault );
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
EXPORT_C CScriptBase* LibEntryL
        (
        CTestModuleIf& aTestModuleIf // Backpointer to STIF Test Framework
        )
    {
    return ( CScriptBase* ) CPDI::NewL( aTestModuleIf );
    }

//  End of File
