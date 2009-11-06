/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "pdi.h"

#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <etelutils.h>
#include <in_sock.h>

#if defined(NCP_COMMON_S60_VERSION_SUPPORT ) && ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
#include <ctsy/serviceapi/cmmutility.h>
#else
#include <cmmcommonstaticutility.h>
#endif


// CONSTANTS
_LIT8( KLine, "---------------------------------" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIM::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CPDI::Delete()
    {
    _LIT8 (KDelete, "CPDI::Delete" );
    iPDILog->Log((TDesC8)KDelete );
    }

// -----------------------------------------------------------------------------
// CSIM::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CPDI::RunMethodL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8 (KLog, "PDI: RunMethodL" );
    iPDILog->Log((TDesC8)KLog);

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "NotifyChangeOfNtwkRegStatusSetParameter",  CPDI::NotifyChangeOfNtwkRegStatusSetParameterL ),
        ENTRY( "NotifyCurrentNetworkChange",               CPDI::NotifyCurrentNetworkChange ),
        ENTRY( "GetNetworkRegistrationStatus",             CPDI::GetNetworkRegistrationStatusL ),
        ENTRY( "GetHomeNetwork",                           CPDI::GetHomeNetworkL ),
        ENTRY( "PacketServiceDetach",                      CPDI::PacketServiceDetachL ),
        ENTRY( "PacketServiceRejectActivationRequest",     CPDI::PacketServiceRejectActivationRequestL ),
        ENTRY( "GetPacketAttachMode",                      CPDI::GetPacketAttachModeL ),
        ENTRY( "PacketServiceAttach",                      CPDI::PacketServiceAttachL ),
        ENTRY( "SetPacketServiceAttachMode",               CPDI::SetPacketServiceAttachModeL ),
        ENTRY( "PacketServiceSetDefaultContextParameters", CPDI::PacketServiceSetDefaultContextParametersL ),
        ENTRY( "SetPreferredBearer",                       CPDI::SetPreferredBearerL ),
        ENTRY( "GetDataVolumeTransferred",                 CPDI::GetDataVolumeTransferredL ),
        ENTRY( "InitializePacketContext",                  CPDI::InitializePacketContextL ),
        ENTRY( "ActivatePacketContext",                    CPDI::ActivatePacketContextL ),
        ENTRY( "SetPacketContextConfiguration",            CPDI::SetPacketContextConfigurationL ),
        ENTRY( "SetProfileQoSParams",                      CPDI::SetProfileQoSParamsL ),
        ENTRY( "DeactivatePacketContext",                  CPDI::DeactivatePacketContextL ),
        ENTRY( "DeletePacketContext",                      CPDI::DeletePacketContextL ),
        ENTRY( "AddPacketFilters",                         CPDI::AddPacketFiltersL ),
        ENTRY( "AddMediaAuthorizationParams",              CPDI::AddMediaAuthorizationParamsL ),
        ENTRY( "ModifyPacketContext",                      CPDI::ModifyPacketContextL ),
        ENTRY( "PacketContextNotifyStatusChange",          CPDI::PacketContextNotifyStatusChange ),
        ENTRY( "PacketContextNotifyConfigChanged",         CPDI::PacketContextNotifyConfigChanged ),
        ENTRY( "SetNotifyDynamicCapsChangeRefParams",      CPDI::SetNotifyDynamicCapsChangeRefParamsL),
        ENTRY( "SetNotifyTransferCapsRefParams",           CPDI::SetNotifyTransferCapsRefParamsL),
        ENTRY( "SetNotifyMSClassChangeRefParams",          CPDI::SetNotifyMSClassChangeRefParamsL),
        ENTRY( "SetExpectedPacketContextNotifyConnectionSpeed",
            CPDI::SetExpectedPacketContextNotifyConnectionSpeedL),
        ENTRY( "SetExpectedContextMicsStatus",             CPDI::SetExpectedContextMicsStatusL),

        ENTRY( "RunIscTestCase",                           CCore::RunIscTestCase ),
        ENTRY( "SyncIscTestCase",                          CCore::SyncIscTestCase ),
        ENTRY( "RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined",
                    CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined),
        ENTRY( "SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined",
                    CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined),
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC", CCore::SetExpectedCompleteIPC),
        ENTRY( "SetPdiReferenceParameters", CPDI::SetPdiReferenceParameters),
        ENTRY( "SetContextNameForComparison", CPDI::SetContextNameForComparison ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CPDI::NotifyCurrentNetworkChange
// Getting Change Notify of Current Network
// -----------------------------------------------------------------------------
//
TInt CPDI::NotifyCurrentNetworkChange
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KLog, "CPDI::NotifyCurrentNetworkChange" );
    iPDILog->Log( ( TDesC8 ) KLog );

    TInt ret( KErrNone );

	TPtrC statusParam;
	aItem.GetNextString( statusParam );

	if( _L("AreaKnown") == statusParam  )
 		{
        iNotifyCurrentNetworkChangeOngoing = 1;
 		}
 	else if( _L("LocationAreaCode") == statusParam )
 	    {
        iNotifyCurrentNetworkChangeOngoing = 2;
 	    }
 	else if( _L("CellId") == statusParam )
 	    {
        iNotifyCurrentNetworkChangeOngoing = 3;
 	    }
 	else
 	    {
		_LIT8( KFail, "CPDI::NotifyCurrentNetworkChange, Invalid parameter" );
    	iPDILog->Log( ( TDesC8 ) KFail );

        iNotifyCurrentNetworkChangeOngoing = 0;
 	    ret = KErrNotFound;
 	    }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteNotifyCurrentNetworkChange
// Complete NotifyCurrentNetworkChange
// -----------------------------------------------------------------------------
//
void CPDI::CompleteNotifyCurrentNetworkChange
		(
		TInt aResult, CMmDataPackage* aDataPackage
		)
	{
    _LIT8( KLog, "CPDI::CompleteNotifyCurrentNetworkChange, result: %d" );
	iPDILog->Log( ( TDesC8 ) KLog, aResult );

	if ( iHandleSpecifiedRequests ||
	    ( 0 < iNotifyCurrentNetworkChangeOngoing ) )
	    {
    	if( KErrNone == aResult )
    		{
    	    RMobilePhone::TMobilePhoneNetworkInfoV5 currNetworkInfo;
            RMobilePhone::TMobilePhoneLocationAreaV1 currLocationArea;

            aDataPackage->UnPackData( currNetworkInfo, currLocationArea );

            switch ( iNotifyCurrentNetworkChangeOngoing )
                {
                case 1:
                    {
                    _LIT8( KResult,
                        "CPDI::CompleteNotifyCurrentNetworkChange, iAreaKnown: %d" );
                    iPDILog->Log(
                        ( TDesC8 ) KResult, currLocationArea.iAreaKnown );

                    Signal( currLocationArea.iAreaKnown );
                    break;
                    }
                case 2:
                    {
                    _LIT8( KResult,
                        "CPDI::CompleteNotifyCurrentNetworkChange, iLocationAreaCode: %d" );
                    iPDILog->Log(
                        ( TDesC8 ) KResult, currLocationArea.iLocationAreaCode );

                    Signal( currLocationArea.iLocationAreaCode );
                    break;
                    }
                case 3:
                    {
                    _LIT8( KResult,
                        "CPDI::CompleteNotifyCurrentNetworkChange, iCellId: %d" );
                    iPDILog->Log(
                        ( TDesC8 ) KResult, currLocationArea.iCellId );

                    Signal( currLocationArea.iCellId );
                    break;
                    }
                default:
                    {
            		_LIT8( KFail, "CPDI::CompleteNotifyCurrentNetworkChange, Invalid parameter" );
                	iPDILog->Log( ( TDesC8 ) KFail );

                    Signal( KErrGeneral );
                    break;
                    }
                }
            }
    	else
    	    {
    		_LIT8( KFail, "CompleteNotifyCurrentNetworkChange Failed: %d" );
        	iPDILog->Log( ( TDesC8 ) KFail, aResult );

        	Signal( aResult );
    	    }

    	iNotifyCurrentNetworkChangeOngoing = 0;
	    }
	//no else
	}

// -----------------------------------------------------------------------------
// CPDI::GetNetworkRegistrationStatusL
// Getting call barring status
// -----------------------------------------------------------------------------
//
TInt CPDI::GetNetworkRegistrationStatusL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::GetNetworkRegistrationStatus" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );
    TPtrC string;
    aItem.GetNextString ( string );

    iRegStatusClientForComparison = RPacketService::EUnknown;

    if ( iHandleSpecifiedRequests )
        {
        if ( _L("ERegisteredOnHomeNetwork") == string )
            {
            iPDILog->Log( _L("ERegisteredOnHomeNetwork.") );
            iRegStatusClientForComparison = RPacketService::ERegisteredOnHomeNetwork;
            }
        else if ( _L("ERegisteredRoaming") == string )
            {
            iPDILog->Log( _L("ERegisteredRoaming.") );
            iRegStatusClientForComparison = RPacketService::ERegisteredRoaming;
            }
        }
    ret = HandleRequestL( EPacketGetNtwkRegStatus );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetNetworkRegistrationStatus request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "GetNetworkRegistrationStatus request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
	}

// -----------------------------------------------------------------------------
// CPDI::CompleteGetNetworkRegistrationStatus
// Complete GetNetworkRegistrationStatus method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteGetNetworkRegistrationStatus
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompleteGetNetworkRegistrationStatus result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if ( iHandleSpecifiedRequests )
        {
        RPacketService::TRegistrationStatus regStatusClient;
        aDataPackage->UnPackData( regStatusClient );

        if ( regStatusClient != iRegStatusClientForComparison )
            {
            aResult = KErrGeneral;
            }
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetNetworkRegistrationStatus Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

TInt CPDI::NotifyChangeOfNtwkRegStatusSetParameterL( CStifItemParser& aItem )
    {
    _LIT8( KNotifyChangeOfNtwkRegStatusSetParameter,
           "CPDI::NotifyChangeOfNtwkRegStatusSetParameter" );
    iPDILog->Log((TDesC8)KNotifyChangeOfNtwkRegStatusSetParameter);

    TPtrC string;
    aItem.GetNextString ( string );
    iRegStatusClientForComparison = RPacketService::EUnknown;

    if ( iHandleSpecifiedRequests )
        {
        if ( _L("ERegisteredOnHomeNetwork") == string )
            {
            iPDILog->Log( _L("ERegisteredOnHomeNetwork.") );
            iRegStatusClientForComparison = RPacketService::ERegisteredOnHomeNetwork;
            }
        else if ( _L("ERegisteredRoaming") == string )
            {
            iPDILog->Log( _L("ERegisteredRoaming.") );
            iRegStatusClientForComparison = RPacketService::ERegisteredRoaming;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPDI::GetNotifyChangeofNtwkRegStatus
// Getting Notify Change of Network Registration Status
// -----------------------------------------------------------------------------
//
void CPDI::NotifyChangeOfNtwkRegStatus
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::NotifyChangeOfNtwkRegStatus result: %d" );
    iPDILog->Log( (TDesC8)KResult, aResult );

    if ( KErrNone != aResult )
        {
        _LIT8( KFail, "NotifyChangeOfNtwkRegStatus Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }
    else
        {
        if ( iHandleSpecifiedRequests )
            {
            RPacketService::TRegistrationStatus regstatus;
            aDataPackage->UnPackData( regstatus );
            if ( iRegStatusClientForComparison != regstatus )
                {
                _LIT8( KComparisonFailed, "NotifyChangeOfNtwkRegStatus comparison failed: exp=%d; act=%d" );
                iPDILog->Log( (TDesC8)KComparisonFailed,
                    iRegStatusClientForComparison,
                    aResult );
                aResult = KErrGeneral;
                }
            }
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::GetHomeNetwork
// GetHomeNetwork method function.
// We dont care about home network details since this is checked in
// CMmNetMessHandler module tests
// -----------------------------------------------------------------------------
//
TInt CPDI::GetHomeNetworkL( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KGetHomeNetwork, "CPDI::GetHomeNetwork");
    iPDILog->Log((TDesC8)KGetHomeNetwork);

    TInt ret( HandleRequestL( EMobilePhoneGetHomeNetwork ) );
    iGetHomeNetworkOngoing = ETrue;

    if ( KErrNone == ret )
        {
        _LIT8( KOk, "GetHomeNetwork request send ok: %d" );
        iPDILog->Log((TDesC8)KOk, ret );
        }
    else
        {
        _LIT8( KFail, "GetHomeNetwork request send fails: %d" );
        iPDILog->Log((TDesC8)KFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteGetHomeNetwork
// Complete GetHomeNetwork method function.
// We dont need to check HPLMN details here since this is the functionality
// of CMmNetMessHandler
// -----------------------------------------------------------------------------
//
void CPDI::CompleteGetHomeNetwork
    (
    TInt aResult,
    CMmDataPackage* /*aDataPackage*/
    )
    {
    _LIT8( KResult, "CPDI::CompleteGetHomeNetwork result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );
    if ( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetHomeNetwork Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }
    if ( ( iGetHomeNetworkOngoing ) ||
       ( iHandleSpecifiedRequests ) )
        {
        iGetHomeNetworkOngoing = EFalse;
        _LIT8( KResultComplete, "GetHomeNetwork Completed" );
        iPDILog->Log((TDesC8)KResultComplete );
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::PacketServiceDetachL
// Detach Packet Service
// -----------------------------------------------------------------------------
//
TInt CPDI::PacketServiceDetachL
        (
        CStifItemParser& /*aItem*/
        )
    {
    _LIT8( KPDI, "CPDI::PacketServiceDetach" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    ret = HandleRequestL( EPacketDetach );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "PacketServiceDetach request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        iDetachOngoing = ETrue;
        }
    else
        {
        _LIT8( KSendFailed, "PacketServiceDetach request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketServiceDetach
// Complete PacketServiceDetach method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketServiceDetach
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompletePacketServiceDetach result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePacketServiceDetach Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if( iDetachOngoing || iHandleSpecifiedRequests )
        {
        iDetachOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompletePacketServiceDetach") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::PacketNotifyStatusChange
// Getting Notify Change of Packet Network Connection Status
// -----------------------------------------------------------------------------
//
void CPDI::PacketNotifyStatusChange
        (
        TInt aResult, CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KPDI, "CPDI::PacketNotifyStatusChange" );
    iPDILog->Log((TDesC8)KPDI );

    RPacketService::TStatus status;
    TBool value = EFalse;
    aDataPackage->UnPackData ( status, value );

    _LIT8( KStatus, "Status : %d" );
    iPDILog->Log((TDesC8)KStatus, status );

    _LIT8( KValue, "Value : %d" );
    iPDILog->Log((TDesC8)KValue, value );

    _LIT8( KResult, "PacketNotifyStatusChange result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "PacketNotifyStatusChange Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::PacketServiceRejectActivationRequestL
// Reject Packet Service Activation Request
// -----------------------------------------------------------------------------
//
TInt CPDI::PacketServiceRejectActivationRequestL
        (
        CStifItemParser& /*aItem*/
        )
    {
    _LIT8( KPDI, "CPDI::PacketServiceRejectActivationRequest" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    ret = HandleRequestL( EPacketRejectActivationRequest );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "PacketServiceRejectActivationRequest request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "PacketServiceRejectActivationRequest request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketServiceRejectActivationRequest
// Complete PacketServiceDetach method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketServiceRejectActivationRequest
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompletePacketServiceRejectActivationRequest result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePacketServiceRejectActivationRequest Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    iPDILog->Log( _L("signal CompletePacketServiceRejectActivationRequest") );
    }

// -----------------------------------------------------------------------------
// CPDI::GetPacketAttachModeL
// Getting Packet Attach Mode
// -----------------------------------------------------------------------------
//
TInt CPDI::GetPacketAttachModeL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::GetPacketAttachMode" );
    iPDILog->Log((TDesC8)KPDI );

    TPtrC string;
    aItem.GetNextString ( string );

    iAttachModeForComparison = RPacketService::EAttachWhenNeeded;

    if ( iHandleSpecifiedRequests )
        {
        if ( _L("EAttachWhenPossible") == string )
            {
            iPDILog->Log( _L("EAttachWhenPossible.") );
            iAttachModeForComparison = RPacketService::EAttachWhenPossible;
            }
        }

    TInt ret( KErrNotFound );

    TBool bootUpPhase = EFalse;

    CMmDataPackage package;

    // Set package data
    package.PackData( &bootUpPhase );

    ret = HandleRequestL( EPacketGetAttachMode, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetPacketAttachMode request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "GetPacketAttachMode request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteGetPacketAttachMode
// Complete GetPacketAttachMode method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteGetPacketAttachMode
        (
        TInt aResult, CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompleteGetPacketAttachMode result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if ( iHandleSpecifiedRequests )
        {
        RPacketService::TAttachMode attachMode(
            RPacketService::EAttachWhenNeeded );
        aDataPackage->UnPackData( attachMode );

        if( attachMode != iAttachModeForComparison )
            {
            aResult = KErrGeneral;
            }
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetPacketAttachMode Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    iPDILog->Log( _L("signal CompleteGetPacketAttachMode") );
    }

// -----------------------------------------------------------------------------
// CPDI::PacketServiceAttachL
// Attach Packet Service
// -----------------------------------------------------------------------------
//
TInt CPDI::PacketServiceAttachL
        (
        CStifItemParser& /*aItem*/
        )
    {
    _LIT8( KPDI, "CPDI::PacketServiceAttach" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    ret = HandleRequestL( EPacketAttach );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "PacketServiceAttach request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        iAttachOngoing = ETrue;
        }
    else
        {
        _LIT8( KSendFailed, "PacketServiceAttach request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketServiceAttach
// Complete PacketServiceAttach method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketServiceAttach
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompletePacketServiceAttach result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePacketServiceAttach Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    iPDILog->Log( _L("signal CompletePacketServiceAttach") );
    }

// -----------------------------------------------------------------------------
// CPDI::SetPacketServiceAttachModeL
// Set Attach Mode for Packet Service
// -----------------------------------------------------------------------------
//
TInt CPDI::SetPacketServiceAttachModeL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetPacketServiceAttachMode" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TBool supported( ETrue );

    TPtrC modestring;

    aItem.GetNextString ( modestring );

    RPacketService::TAttachMode mode;

    if ( _L("AttachWhenPossible") == modestring )
        {
        mode = RPacketService::EAttachWhenPossible;
        }
    else if ( _L("AttachWhenNeeded") == modestring )
        {
        mode = RPacketService::EAttachWhenNeeded;
        }
    else
        {
        _LIT8(KNotSupp, "CPDI::Selected packet attach mode not supported");
        iPDILog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CMmDataPackage package;

        package.PackData(&mode);

        // Set package data
        ret = HandleRequestL( EPacketSetAttachMode, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetPacketServiceAttachMode request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            iSetAttachOngoing = ETrue;
            }
        else
            {
            _LIT8( KSendFailed, "SetPacketServiceAttachMode request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteSetPacketServiceAttachMode
// Complete SetPacketServiceAttachMode method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteSetPacketServiceAttachMode
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompleteSetPacketServiceAttachMode result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSetPacketServiceAttachMode Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if ( iHandleSpecifiedRequests && !iDefaultConOngoing && !iSetAttachOngoing )
        {
        Signal(aResult);
        }

    if( iDefaultConOngoing || iSetAttachOngoing )
        {
        iDefaultConOngoing = EFalse;
        iSetAttachOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteSetPacketServiceAttachMode") );
        }

    }

// -----------------------------------------------------------------------------
// CPDI::PacketServiceSetDefaultContextParametersL
// Set Default Context Params for Packet Service
// -----------------------------------------------------------------------------
//
TInt CPDI::PacketServiceSetDefaultContextParametersL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::PacketServiceSetDefaultContextParameters" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TPtrC configstring;
    TPtrC protocolstring;
    TPtrC compression;

    aItem.GetNextString ( configstring );
    aItem.GetNextString ( protocolstring );
    aItem.GetNextString ( compression );

    RPacketContext::TContextConfigGPRS baseGPRS;
    RPacketContext::TContextConfigR99_R4 baseR99_R4;

    if( _L("GPRS") == configstring )
        {
        baseGPRS = RPacketContext::TContextConfigGPRS();

        if( _L("PdpTypeIPv4") == protocolstring )
            {
            baseGPRS.iPdpType = RPacketContext::EPdpTypeIPv4;
            }
        else if( _L("PdpTypeIPv6") == protocolstring )
            {
            baseGPRS.iPdpType = RPacketContext::EPdpTypeIPv6;
            }
        else if( _L("PdpTypePPP") == protocolstring )
            {
            baseGPRS.iPdpType = RPacketContext::EPdpTypePPP;
            }
        else
            {
            _LIT8( KNotSupported, "Selected protocol type not supported" );
            iPDILog->Log((TDesC8)KNotSupported, ret );
            supported = EFalse;
            }

        if( _L("Header") == compression )
            {
            baseGPRS.iPdpCompression = RPacketContext::KPdpHeaderCompression;
            }
        else if( _L("Data") == compression )
            {
            baseGPRS.iPdpCompression = RPacketContext::KPdpDataCompression;
            }
        else
            {
            _LIT8( KNotSupported, "Selected compression not supported" );
            iPDILog->Log((TDesC8)KNotSupported, ret );
            supported = EFalse;
            }
        }
    else if( _L("R99_R4") == configstring )
        {
        baseR99_R4 = RPacketContext::TContextConfigR99_R4();

        if( _L("PdpTypeIPv4") == protocolstring )
            {
            baseR99_R4.iPdpType = RPacketContext::EPdpTypeIPv4;
            }
        else if( _L("PdpTypeIPv6") == protocolstring )
            {
            baseR99_R4.iPdpType = RPacketContext::EPdpTypeIPv6;
            }
        else if( _L("PdpTypePPP") == protocolstring )
            {
            baseR99_R4.iPdpType = RPacketContext::EPdpTypePPP;
            }
        else
            {
            _LIT8( KNotSupported, "Selected protocol type not supported" );
            iPDILog->Log((TDesC8)KNotSupported, ret );
            supported = EFalse;
            }
        }
    else
        {
        _LIT8( KNotSupported, "Selected context configuration not supported" );
        iPDILog->Log((TDesC8)KNotSupported, ret );
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CMmDataPackage package;

        if( _L("GPRS") == configstring )
            {
            package.PackData(&baseGPRS);
            }
        else
            {
            package.PackData(&baseR99_R4);
            }

        // Set package data
        ret = HandleRequestL( EPacketSetDefaultContextParams, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "PacketServiceSetDefaultContextParameters request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            iDefaultConOngoing = ETrue;
            }
        else
            {
            _LIT8( KSendFailed, "PacketServiceSetDefaultContextParameters request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::SetPreferredBearerL
// Set Preferred Bearer
// -----------------------------------------------------------------------------
//
TInt CPDI::SetPreferredBearerL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetPreferredBearer" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TBool supported( ETrue );

    TPtrC bearerstring;

    aItem.GetNextString( bearerstring );

    RPacketService::TPreferredBearer bearer;

    if ( _L("BearerCircuitSwitched") == bearerstring )
        {
        bearer = RPacketService::EBearerCircuitSwitched;
        }
    else if ( _L("BearerPacketSwitched") == bearerstring )
        {
        bearer = RPacketService::EBearerPacketSwitched;
        }
    else
        {
        _LIT8(KNotSupp, "CPDI::Selected bearer type not supported");
        iPDILog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CMmDataPackage package;

        package.PackData(&bearer);

        // Set package data
        ret = HandleRequestL( EPacketSetPrefBearer, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetPreferredBearer request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "SetPreferredBearer request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteSetPreferredBearer
// Complete SetPreferredBearer method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteSetPreferredBearer
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompleteSetPreferredBearer result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSetPreferredBearer Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    iPDILog->Log( _L("signal CompleteSetPreferredBearer") );
    }

// -----------------------------------------------------------------------------
// CPDI::GetDataVolumeTransferredL
// Get data volume transferred
// -----------------------------------------------------------------------------
//
TInt CPDI::GetDataVolumeTransferredL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::GetDataVolumeTransferred" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TPtrC cName;
    TInt bytesSent = 0;
    TInt bytesReceived = 0;

    aItem.GetNextString ( cName );
    aItem.GetNextInt( bytesSent );
    aItem.GetNextInt( bytesReceived );

    iDataVolumeForComparison.iBytesSent = bytesSent;
    iDataVolumeForComparison.iBytesReceived = bytesReceived;

    TInfoName contextName = cName;
    iContextNameForComparison = cName;

    //Create package
    CMmDataPackage package;

    package.PackData( &contextName );

    // Set package data
    ret = HandleRequestL( EPacketContextGetDataVolumeTransferred, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetDataVolumeTransferred request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        iGetDataVolOngoing = ETrue;
        }
    else
        {
        _LIT8( KSendFailed, "GetDataVolumeTransferred request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteGetDataVolumeTransferred
// Complete GetDataVolumeTransferred method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteGetDataVolumeTransferred
        (
        TInt aResult, CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompleteGetDataVolumeTransferred result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    TInfoName aContextName;
    RPacketContext::TDataVolume aDataVolume;

    aDataPackage->UnPackData( aContextName, aDataVolume );

    if ( ( ( aContextName != iContextNameForComparison ) ||
           ( aDataVolume.iBytesReceived != iDataVolumeForComparison.iBytesReceived ) ||
           ( aDataVolume.iBytesSent != iDataVolumeForComparison.iBytesSent ) ) &&
         ( iHandleSpecifiedRequests ) )
        {
        aResult = KErrGeneral;
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetDataVolumeTransferred Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if( iGetDataVolOngoing || iHandleSpecifiedRequests )
        {
        iGetDataVolOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteGetDataVolumeTransferred") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::InitializePacketContextL
// Initializing packet context
// -----------------------------------------------------------------------------
//
TInt CPDI::InitializePacketContextL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::InitializePacketContext" );
    iPDILog->Log( ( TDesC8 ) KPDI );

    TInt ret( KErrNotFound );

    TPtrC cName;
    TPtrC hName;

    aItem.GetNextString ( cName );
    aItem.GetNextString ( hName );

    TInfoName contextName = cName;
    iContextNameForComparison = cName;
    TInfoName hostContextName = hName;

    if ( 0 == contextName.Compare( hostContextName ) )
        {
        hostContextName.Zero();
        }

    //Create package
    CMmDataPackage package;
    package.PackData( &contextName, &hostContextName );

    // Set package data
    ret = HandleRequestL( EPacketContextInitialiseContext, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "InitializePacketContext request send ok" );
        iPDILog->Log( ( TDesC8 ) KSendOk );
        iInitOngoing = ETrue;
        }
    else
        {
        _LIT8( KSendFail, "InitializePacketContext request send failed: %d" );
        iPDILog->Log( ( TDesC8 ) KSendFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteInitializePacketContext
// Complete InitializePacketContext method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteInitializePacketContext
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompleteInitializePacketContext result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteInitializePacketContext Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if( iInitOngoing )
        {
        iInitOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteInitializePacketContext") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::ActivatePacketContextL
// Activating packet context
// -----------------------------------------------------------------------------
//
TInt CPDI::ActivatePacketContextL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::ActivatePacketContext" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );
    TPtrC cName;

    aItem.GetNextString ( cName );

    TInfoName contextName = cName;

    //Create package
    CMmDataPackage package;

    package.PackData( &contextName );

    // Set package data
    ret = HandleRequestL( EPacketContextActivate, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "ActivatePacketContext request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        iActivateOngoing = ETrue;
        }
    else
        {
        _LIT8( KSendFailed, "ActivatePacketContext request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteActivatePacketContext
// Complete ActivatePacketContext method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteActivatePacketContext
        (
        TInt aResult, CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompleteActivatePacketContext result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    TInfoName aContextName;

    aDataPackage->UnPackData( aContextName );

    if ( aContextName != iContextNameForComparison )
        {
        aResult = KErrGeneral;
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteActivatePacketContext Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if( iActivateOngoing || iHandleSpecifiedRequests )
        {
        iActivateOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteActivatePacketContext") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::SetPacketContextConfigurationL
// Setting Packet Context Configuration
// -----------------------------------------------------------------------------
//
TInt CPDI::SetPacketContextConfigurationL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetPacketContextConfiguration" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TBool supported( ETrue );

    TPtrC namestring;
    TPtrC configstring;
    TPtrC protocolstring;
    TPtrC moOrMtContextConfString;
    TPtrC requesPCSCFAddr;

    aItem.GetNextString( namestring );
    aItem.GetNextString( configstring );
    aItem.GetNextString( protocolstring );
    aItem.GetNextString( moOrMtContextConfString );
    aItem.GetNextString( requesPCSCFAddr );

    RPacketContext::TContextConfigGPRS configGPRS;
    RPacketContext::TContextConfigR99_R4 configR99_R4;
    RPacketContext::TContextConfig_R5 configR5;

    if ( _L("GPRS") == configstring )
        {
        if( _L("PdpTypeIPv4") == protocolstring )
            {
            configGPRS.iPdpType = RPacketContext::EPdpTypeIPv4;
            }
        else if( _L("PdpTypeIPv6") == protocolstring )
            {
            configGPRS.iPdpType = RPacketContext::EPdpTypeIPv6;
            }
        else
            {
            _LIT8( KNotSupported, "Selected protocol type not supported" );
            iPDILog->Log((TDesC8)KNotSupported, ret );
            supported = EFalse;
            }
        }
    else if ( _L("R99_R4") == configstring )
        {
        if( _L("PdpTypeIPv4") == protocolstring )
            {
            configR99_R4.iPdpType = RPacketContext::EPdpTypeIPv4;
            }
        else if( _L("PdpTypeIPv6") == protocolstring )
            {
            configR99_R4.iPdpType = RPacketContext::EPdpTypeIPv6;
            }
        else
            {
            _LIT8( KNotSupported, "Selected protocol type not supported" );
            iPDILog->Log((TDesC8)KNotSupported, ret );
            supported = EFalse;
            }
        }
    else if ( _L("R5") == configstring )
        {
        if( _L("PdpTypeIPv4") == protocolstring )
            {
            configR5.iPdpType = RPacketContext::EPdpTypeIPv4;
            }
        else if( _L("PdpTypeIPv6") == protocolstring )
            {
            configR5.iPdpType = RPacketContext::EPdpTypeIPv6;
            }
        else
            {
            _LIT8( KNotSupported, "Selected protocol type not supported" );
            iPDILog->Log((TDesC8)KNotSupported, ret );
            supported = EFalse;
            }
        }
    else
        {
        _LIT8( KNotSupported, "Selected context configuration not supported" );
        iPDILog->Log((TDesC8)KNotSupported, ret );
        supported = EFalse;
        }

    if( supported )
        {
         //Create package
        CMmDataPackage package;

        TInfoName contextName = namestring;
        iContextNameForComparison = namestring;

        if ( _L("GPRS") == configstring )
            {
            if ( _L("NWICONTEXTCONF") == moOrMtContextConfString )
                {
                configGPRS.iNWIContext = ETrue;
                }
            else
                {
                configGPRS.iNWIContext = EFalse;
                }

            if ( _L("RequesPCSCFAddr") == requesPCSCFAddr )
                {
                //Add P-CSCF Address request into iMiscBuffer
                AddPCSCFAddressDiscoveryRequest( configGPRS );
                iPCSCFAddressRequested = ETrue;
                }
            else
                {
                iPCSCFAddressRequested = EFalse;
                }

            package.PackData( &configGPRS, &contextName);
            }
        else if ( _L("R99_R4") == configstring )
            {
            if ( _L("NWICONTEXTCONF") == moOrMtContextConfString )
                {
                configR99_R4.iNWIContext = ETrue;
                }
            else
                {
                configR99_R4.iNWIContext = EFalse;
                }

            if ( _L("RequesPCSCFAddr") == requesPCSCFAddr )
                {
                //Add P-CSCF Address request into iMiscBuffer
                AddPCSCFAddressDiscoveryRequest( configR99_R4 );
                iPCSCFAddressRequested = ETrue;
                }
            else
                {
                iPCSCFAddressRequested = EFalse;
                }

            package.PackData( &configR99_R4, &contextName);
            }
        else
            {
            if ( _L("NWICONTEXTCONF") == moOrMtContextConfString )
                {
                configR5.iNWIContext = ETrue;
                }
            else
                {
                configR5.iNWIContext = EFalse;
                }

            if ( _L("RequesPCSCFAddr") == requesPCSCFAddr )
                {
                //Add P-CSCF Address request into iMiscBuffer
                AddPCSCFAddressDiscoveryRequest( configR5 );
                iPCSCFAddressRequested = ETrue;
                }
            else
                {
                iPCSCFAddressRequested = EFalse;
                }

            package.PackData( &configR5, &contextName);
            }

        // Set package data
        ret = HandleRequestL( EPacketContextSetConfig, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetPacketContextConfiguration request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "SetPacketContextConfiguration request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteSetPacketContextConfiguration
// Complete Setting Packet Context Configuration
// -----------------------------------------------------------------------------
//
void CPDI::CompleteSetPacketContextConfiguration
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompleteSetPacketContextConfiguration result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSetPacketContextConfiguration Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CPDI::SetProfileQoSParamsL
// Setting Profile Quality of Service Params
// -----------------------------------------------------------------------------
//
TInt CPDI::SetProfileQoSParamsL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetProfileQoSParams" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TPtrC profilestring;
    TPtrC signallingIndication;

    TPtrC minDelay;
    TPtrC minMeanThroughput;
    TPtrC minPeakThroughput;
    TPtrC minPrecedence;
    TPtrC minReliability;
    TPtrC reqDelay;
    TPtrC reqMeanThroughput;
    TPtrC reqPeakThroughput;
    TPtrC reqPrecedence;
    TPtrC reqReliability;

    TPtrC pdpType;
    TPtrC contextName;
    TPtrC primaryContextName;
    TPtrC contextStatus;

    TPtrC minTrafficClass;
    TPtrC minDeliveryOrderReq;
    TPtrC minDeliveryErroneusSDU;
    TPtrC maxBER;
    TPtrC maxSDUErrorRatio;
    TPtrC minTrafficHandlingPriority;

    aItem.GetNextString( profilestring );
    iProfile.Zero();
    iProfile.Append( profilestring );

    aItem.GetNextString( signallingIndication );

    aItem.GetNextString( contextName );
    aItem.GetNextString( primaryContextName );
    aItem.GetNextString( pdpType );
    aItem.GetNextString( contextStatus );

    aItem.GetNextString( minDelay );
    aItem.GetNextString( minMeanThroughput );
    aItem.GetNextString( minPeakThroughput );
    aItem.GetNextString( minPrecedence );
    aItem.GetNextString( minReliability );
    aItem.GetNextString( reqDelay );
    aItem.GetNextString( reqMeanThroughput );
    aItem.GetNextString( reqPeakThroughput );
    aItem.GetNextString( reqPrecedence );
    aItem.GetNextString( reqReliability );

    aItem.GetNextString( minTrafficClass );
    aItem.GetNextString( minDeliveryOrderReq );
    aItem.GetNextString( minDeliveryErroneusSDU );
    aItem.GetNextString( maxBER );
    aItem.GetNextString( maxSDUErrorRatio );
    aItem.GetNextString( minTrafficHandlingPriority );

    TContextParams params;

    params.iContextName = contextName;
    if ( 0 != primaryContextName.Compare( contextName ) )
        {
        params.iPrimaryContextName = primaryContextName;
        }

    params.iContextType = EFalse;

    if( _L("PdpTypeIPv4") == pdpType )
        {
        params.iPdpType = RPacketContext::EPdpTypeIPv4;
        }
    else if ( _L("PdpTypeIPv6") == pdpType )
        {
        params.iPdpType = RPacketContext::EPdpTypeIPv6;
        }
    else if ( _L("PdpTypeX25") == pdpType )
        {
        params.iPdpType = RPacketContext::EPdpTypeX25;
        }
    else if ( _L("PdpTypePPP") == pdpType )
        {
        params.iPdpType = RPacketContext::EPdpTypePPP;
        }
    else if ( _L("PdpTypeCDPD") == pdpType )
        {
        params.iPdpType = RPacketContext::EPdpTypeCDPD;
        }
    else
        {
        iPDILog->Log( _L8("Selected Pdp type not supported") );
        supported = EFalse;
        }

    if( _L("StatusUnknown") == contextStatus )
        {
        params.iContextStatus = RPacketContext::EStatusUnknown;
        }
    else if ( _L("StatusInactive") == contextStatus )
        {
        params.iContextStatus = RPacketContext::EStatusInactive;
        }
    else if ( _L("StatusActivating") == contextStatus )
        {
        params.iContextStatus = RPacketContext::EStatusActivating;
        }
    else if ( _L("StatusActive") == contextStatus )
        {
        params.iContextStatus = RPacketContext::EStatusActive;
        }
    else if ( _L("StatusDeactivating") == contextStatus )
        {
        params.iContextStatus = RPacketContext::EStatusDeactivating;
        }
    else if ( _L("StatusSuspended") == contextStatus )
        {
        params.iContextStatus = RPacketContext::EStatusSuspended;
        }
    else if ( _L("StatusDeleted") == contextStatus )
        {
        params.iContextStatus = RPacketContext::EStatusDeleted;
        }
    else
        {
        iPDILog->Log( _L8("Selected Status not supported") );
        supported = EFalse;
        }

    if ( _L("GPRS") == profilestring )
        {
        iProfileGPRS = RPacketQoS::TQoSGPRSRequested();

        MapQoSGPRS( iProfileGPRS, &minDelay, &minMeanThroughput,
                        &minPeakThroughput, &minPrecedence, &minReliability,
                        &reqDelay, &reqMeanThroughput, &reqPeakThroughput,
                        &reqPrecedence, &reqReliability );
        }
    else if ( _L("R99_R4") == profilestring )
        {
        iProfileR99_R4 = RPacketQoS::TQoSR99_R4Requested();

        MapQoSR99_4Params( iProfileR99_R4, &minTrafficClass, &minDeliveryOrderReq,
                        &minDeliveryErroneusSDU, &maxBER, &maxSDUErrorRatio,
                        &minTrafficHandlingPriority );
        }
    else if ( _L("R5") == profilestring )
        {
        iProfileR5 = RPacketQoS::TQoSR5Requested();

        if( _L("True") == signallingIndication )
            {
            iProfileR5.iSignallingIndication = ETrue;
            }
        else if ( _L("False") == signallingIndication )
            {
            iProfileR5.iSignallingIndication = EFalse;
            }
        else
            {
            iPDILog->Log( _L8("Selected signalling indication option not supported") );
            supported = EFalse;
            }

        MapQoSR99_4Params( iProfileR5, &minTrafficClass, &minDeliveryOrderReq,
                        &minDeliveryErroneusSDU, &maxBER, &maxSDUErrorRatio,
                        &minTrafficHandlingPriority );
        }
    else
        {
        iPDILog->Log( _L8("Selected qos type not supported") );
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
        CMmDataPackage package;

        TPckg<TPacketDataConfigBase>* profilePckgPtr;

        if ( _L("GPRS") == profilestring )
            {
            TPckg<TPacketDataConfigBase> profilePckg = TPckg<TPacketDataConfigBase>(iProfileGPRS);
            profilePckgPtr = &profilePckg;

            package.PackData(&params, &profilePckgPtr);
            }
        else if ( _L("R99_R4") == profilestring )
            {
            TPckg<TPacketDataConfigBase> profilePckg = TPckg<TPacketDataConfigBase>(iProfileR99_R4);
            profilePckgPtr = &profilePckg;

            package.PackData(&params, &profilePckgPtr);
            }
        else
            {
            TPckg<TPacketDataConfigBase> profilePckg = TPckg<TPacketDataConfigBase>(iProfileR5);
            profilePckgPtr = &profilePckg;

            package.PackData(&params, &profilePckgPtr);
            }

        // Set package data
        ret = HandleRequestL( EPacketQoSSetProfileParams, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetProfileQoSParams request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            iSetQoSOngoing = ETrue;
            }
        else
            {
            _LIT8( KSendFailed, "SetProfileQoSParams request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::MapQoSGPRSParams
//
// -----------------------------------------------------------------------------
//
void CPDI::MapQoSGPRS
        (
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
        TDesC* aReqReliability
        )
    {
    _LIT8( KPDI, "CPDI::MapQoSGPRS" );
    iPDILog->Log((TDesC8)KPDI );

    if( _L("UnspecifiedDelayClass") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
        }
    else if ( _L("DelayClass1") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EDelayClass1;
        }
    else if ( _L("DelayClass2") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EDelayClass2;
        }
    else if ( _L("DelayClass3") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EDelayClass3;
        }
    else if ( _L("DelayClass4") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EDelayClass4;
        }
    else if ( _L("Delay40ms") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EDelay40ms;
        }
    else if ( _L("Delay120ms") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EDelay120ms;
        }
    else if ( _L("Delay360ms") == *aMinDelay )
        {
        aProfileGPRS.iMinDelay = RPacketQoS::EDelay360ms;
        }

    if( _L("UnspecifiedMeanThroughput") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
        }
    else if ( _L("MeanThroughput100") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput100;
        }
    else if ( _L("MeanThroughput200") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput200;
        }
    else if ( _L("MeanThroughput500") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
        }
    else if ( _L("MeanThroughput1000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput1000;
        }
    else if ( _L("MeanThroughput2000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput2000;
        }
    else if ( _L("MeanThroughput5000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput5000;
        }
    else if ( _L("MeanThroughput10000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput10000;
        }
    else if ( _L("MeanThroughput20000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput20000;
        }
    else if ( _L("MeanThroughput50000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput50000;
        }
    else if ( _L("MeanThroughput100000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput100000;
        }
    else if ( _L("MeanThroughput200000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput200000;
        }
    else if ( _L("MeanThroughput500000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput500000;
        }
    else if ( _L("MeanThroughput1000000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput1000000;
        }
    else if ( _L("MeanThroughput2000000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput2000000;
        }
    else if ( _L("MeanThroughput5000000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput5000000;
        }
    else if ( _L("MeanThroughput10000000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput10000000;
        }
    else if ( _L("MeanThroughput20000000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput20000000;
        }
    else if ( _L("MeanThroughput50000000") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput50000000;
        }
    else if ( _L("MeanThroughputBestEffort") == *aMinMeanThroughput )
        {
        aProfileGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;
        }

    if( _L("UnspecifiedPeakThroughput") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
        }
    else if ( _L("PeakThroughput1000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
        }
    else if ( _L("PeakThroughput2000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput2000;
        }
    else if ( _L("PeakThroughput4000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput4000;
        }
    else if ( _L("PeakThroughput8000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput8000;
        }
    else if ( _L("PeakThroughput16000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput16000;
        }
    else if ( _L("PeakThroughput32000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput32000;
        }
    else if ( _L("PeakThroughput64000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput64000;
        }
    else if ( _L("PeakThroughput128000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput128000;
        }
    else if ( _L("PeakThroughput256000") == *aMinPeakThroughput )
        {
        aProfileGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput256000;
        }

    if( _L("UnspecifiedPrecedence") == *aMinPrecedence )
        {
        aProfileGPRS.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
        }
    else if ( _L("PriorityHighPrecedence") == *aMinPrecedence )
        {
        aProfileGPRS.iMinPrecedence = RPacketQoS::EPriorityHighPrecedence;
        }
    else if ( _L("PriorityMediumPrecedence") == *aMinPrecedence )
        {
        aProfileGPRS.iMinPrecedence = RPacketQoS::EPriorityMediumPrecedence;
        }
    else if ( _L("PriorityLowPrecedence") == *aMinPrecedence )
        {
        aProfileGPRS.iMinPrecedence = RPacketQoS::EPriorityLowPrecedence;
        }

    if( _L("UnspecifiedReliabilityClass") == *aMinReliability )
        {
        aProfileGPRS.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
        }
    else if ( _L("ReliabilityClass1") == *aMinReliability )
        {
        aProfileGPRS.iMinReliability = RPacketQoS::EReliabilityClass1;
        }
    else if ( _L("ReliabilityClass2") == *aMinReliability )
        {
        aProfileGPRS.iMinReliability = RPacketQoS::EReliabilityClass2;
        }
    else if ( _L("ReliabilityClass3") == *aMinReliability )
        {
        aProfileGPRS.iMinReliability = RPacketQoS::EReliabilityClass3;
        }
    else if ( _L("ReliabilityClass4") == *aMinReliability )
        {
        aProfileGPRS.iMinReliability = RPacketQoS::EReliabilityClass4;
        }
    else if ( _L("ReliabilityClass5") == *aMinReliability )
        {
        aProfileGPRS.iMinReliability = RPacketQoS::EReliabilityClass5;
        }

    if( _L("EUnspecifiedDelayClass") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
        }
    else if ( _L("DelayClass1") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EDelayClass1;
        }
    else if ( _L("DelayClass2") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EDelayClass2;
        }
    else if ( _L("DelayClass3") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EDelayClass3;
        }
    else if ( _L("DelayClass4") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EDelayClass4;
        }
    else if ( _L("Delay40ms") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EDelay40ms;
        }
    else if ( _L("Delay120ms") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EDelay120ms;
        }
    else if ( _L("Delay360ms") == *aReqDelay )
        {
        aProfileGPRS.iReqDelay = RPacketQoS::EDelay360ms;
        }

    if( _L("UnspecifiedMeanThroughput") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
        }
    else if ( _L("MeanThroughput100") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput100;
        }
    else if ( _L("MeanThroughput200") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput200;
        }
    else if ( _L("MeanThroughput500") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput500;
        }
    else if ( _L("MeanThroughput1000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput1000;
        }
    else if ( _L("MeanThroughput2000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput2000;
        }
    else if ( _L("MeanThroughput5000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput5000;
        }
    else if ( _L("MeanThroughput10000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput10000;
        }
    else if ( _L("MeanThroughput20000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput20000;
        }
    else if ( _L("MeanThroughput50000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput50000;
        }
    else if ( _L("MeanThroughput100000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput100000;
        }
    else if ( _L("MeanThroughput200000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput200000;
        }
    else if ( _L("MeanThroughput500000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput500000;
        }
    else if ( _L("MeanThroughput1000000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput1000000;
        }
    else if ( _L("MeanThroughput2000000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput2000000;
        }
    else if ( _L("MeanThroughput5000000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput5000000;
        }
    else if ( _L("MeanThroughput10000000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput10000000;
        }
    else if ( _L("MeanThroughput20000000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput20000000;
        }
    else if ( _L("MeanThroughput50000000") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput50000000;
        }
    else if ( _L("MeanThroughputBestEffort") == *aReqMeanThroughput )
        {
        aProfileGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;
        }

    if( _L("UnspecifiedPeakThroughput") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
        }
    else if ( _L("PeakThroughput1000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
        }
    else if ( _L("PeakThroughput2000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput2000;
        }
    else if ( _L("PeakThroughput4000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput4000;
        }
    else if ( _L("PeakThroughput8000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput8000;
        }
    else if ( _L("PeakThroughput16000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput16000;
        }
    else if ( _L("PeakThroughput32000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput32000;
        }
    else if ( _L("PeakThroughput64000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput64000;
        }
    else if ( _L("PeakThroughput128000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput128000;
        }
    else if ( _L("PeakThroughput256000") == *aReqPeakThroughput )
        {
        aProfileGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput256000;
        }

    if( _L("UnspecifiedPrecedence") == *aReqPrecedence )
        {
        aProfileGPRS.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
        }
    else if ( _L("PriorityHighPrecedence") == *aReqPrecedence )
        {
        aProfileGPRS.iReqPrecedence = RPacketQoS::EPriorityHighPrecedence;
        }
    else if ( _L("PriorityMediumPrecedence") == *aReqPrecedence )
        {
        aProfileGPRS.iReqPrecedence = RPacketQoS::EPriorityMediumPrecedence;
        }
    else if ( _L("PriorityLowPrecedence") == *aReqPrecedence )
        {
        aProfileGPRS.iReqPrecedence = RPacketQoS::EPriorityLowPrecedence;
        }

    if( _L("UnspecifiedReliabilityClass") == *aReqReliability )
        {
        aProfileGPRS.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
        }
    else if ( _L("ReliabilityClass1") == *aReqReliability )
        {
        aProfileGPRS.iReqReliability = RPacketQoS::EReliabilityClass1;
        }
    else if ( _L("ReliabilityClass2") == *aReqReliability )
        {
        aProfileGPRS.iReqReliability = RPacketQoS::EReliabilityClass2;
        }
    else if ( _L("ReliabilityClass3") == *aReqReliability )
        {
        aProfileGPRS.iReqReliability = RPacketQoS::EReliabilityClass3;
        }
    else if ( _L("ReliabilityClass4") == *aReqReliability )
        {
        aProfileGPRS.iReqReliability = RPacketQoS::EReliabilityClass4;
        }
    else if ( _L("ReliabilityClass5") == *aReqReliability )
        {
        aProfileGPRS.iReqReliability = RPacketQoS::EReliabilityClass5;
        }
    }

// -----------------------------------------------------------------------------
// CPDI::MapQoSR99_4Params
//
// -----------------------------------------------------------------------------
//
void CPDI::MapQoSR99_4Params
        (
        RPacketQoS::TQoSR99_R4Requested& aProfile,
        TDesC* aMinTrafficClass,
        TDesC* aMinDeliveryOrderReq,
        TDesC* aMinDeliveryErroneusSDU,
        TDesC* aMaxBER,
        TDesC* aMaxSDUErrorRatio,
        TDesC* aMinTrafficHandlingPriority
        )
    {
    _LIT8( KPDI, "CPDI::MapQoSR99_4Params" );
    iPDILog->Log((TDesC8)KPDI );

    if ( _L("TrafficClassUnspecified") == *aMinTrafficClass )
        {
        aProfile.iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
        }
    else if ( _L("TrafficClassConversational") == *aMinTrafficClass )
        {
        aProfile.iMinTrafficClass = RPacketQoS::ETrafficClassConversational;
        }
    else if ( _L("TrafficClassStreaming") == *aMinTrafficClass )
        {
        aProfile.iMinTrafficClass = RPacketQoS::ETrafficClassStreaming;
        }
    else if ( _L("TrafficClassInteractive") == *aMinTrafficClass )
        {
        aProfile.iMinTrafficClass = RPacketQoS::ETrafficClassInteractive;
        }
     else if ( _L("TrafficClassBackground") == *aMinTrafficClass )
        {
        aProfile.iMinTrafficClass = RPacketQoS::ETrafficClassBackground;
        }

    if ( _L("DeliveryOrderUnspecified") == *aMinDeliveryOrderReq )
        {
        aProfile.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
        }
    else if ( _L("DeliveryOrderRequired") == *aMinDeliveryOrderReq )
        {
        aProfile.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;
        }
    else if ( _L("DeliveryOrderNotRequired") == *aMinDeliveryOrderReq )
        {
        aProfile.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderNotRequired;
        }

    if ( _L("ErroneousSDUDeliveryUnspecified") == *aMinDeliveryErroneusSDU )
        {
        aProfile.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
        }
    else if ( _L("ErroneousSDUNoDetection") == *aMinDeliveryErroneusSDU )
        {
        aProfile.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUNoDetection;
        }
    else if ( _L("ErroneousSDUDeliveryRequired") == *aMinDeliveryErroneusSDU )
        {
        aProfile.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired;
        }
    else if ( _L("ErroneousSDUDeliveryNotRequired") == *aMinDeliveryErroneusSDU )
        {
        aProfile.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
        }

    if( _L("BERUnspecified") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBERUnspecified;
        }
    else if ( _L("BERFivePerHundred") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBERFivePerHundred;
        }
    else if ( _L("BEROnePerHundred") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBEROnePerHundred;
        }
    else if ( _L("BERFivePerThousand") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBERFivePerThousand;
        }
    else if ( _L("BERFourPerThousand") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBERFourPerThousand;
        }
    else if ( _L("BEROnePerThousand") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBEROnePerThousand;
        }
    else if ( _L("BEROnePerTenThousand") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBEROnePerTenThousand;
        }
    else if ( _L("BEROnePerHundredThousand") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBEROnePerHundredThousand;
        }
    else if ( _L("BEROnePerMillion") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBEROnePerMillion;
        }
    else if ( _L("BERSixPerHundredMillion") == *aMaxBER )
        {
        aProfile.iMaxBER = RPacketQoS::EBERSixPerHundredMillion;
        }

    if ( _L("SDUErrorRatioUnspecified") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
        }
    else if ( _L("SDUErrorRatioOnePerTen") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTen;
        }
    else if ( _L("SDUErrorRatioOnePerHundred") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundred;
        }
    else if ( _L("SDUErrorRatioSevenPerThousand") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioSevenPerThousand;
        }
    else if ( _L("SDUErrorRatioOnePerThousand") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
        }
    else if ( _L("SDUErrorRatioOnePerTenThousand") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
        }
    else if ( _L("SDUErrorRatioOnePerHundredThousand") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundredThousand;
        }
    else if ( _L("SDUErrorRatioOnePerMillion") == *aMaxSDUErrorRatio )
        {
        aProfile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
        }

    if ( _L("TrafficPriorityUnspecified") == *aMinTrafficHandlingPriority )
        {
        aProfile.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
        }
    else if ( _L("TrafficPriority1") == *aMinTrafficHandlingPriority )
        {
        aProfile.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
        }
    else if ( _L("TrafficPriority2") == *aMinTrafficHandlingPriority )
        {
        aProfile.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority2;
        }
    else if ( _L("TrafficPriority3") == *aMinTrafficHandlingPriority )
        {
        aProfile.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;
        }
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteSetProfileQoSParams
// Complete Setting Profile Quality of Service Params
// -----------------------------------------------------------------------------
//
void CPDI::CompleteSetProfileQoSParams
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompleteSetProfileQoSParams result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSetProfileQoSParams Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if( iSetQoSOngoing || iHandleSpecifiedRequests )
        {
        iSetQoSOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteSetProfileQoSParams") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::DeactivatePacketContextL
// Deactivating packet context
// -----------------------------------------------------------------------------
//
TInt CPDI::DeactivatePacketContextL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::DeactivatePacketContext" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TPtrC contextnamestring;
    aItem.GetNextString( contextnamestring );
    TInfoName contextName = contextnamestring;
    iContextNameForComparison = contextnamestring;

    //Create package
    CMmDataPackage package;

    package.PackData(&contextName);

    // Set package data
    ret = HandleRequestL( EPacketContextDeactivate, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "DeactivatePacketContext request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        iDeactivateOngoing = ETrue;
        }
    else
        {
        _LIT8( KSendFailed, "DeactivatePacketContext request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteDeactivatePacketContext
// Complete deactivating packet context
// -----------------------------------------------------------------------------
//
void CPDI::CompleteDeactivatePacketContext
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompleteDeactivatePacketContext result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    TInfoName* contextName;
    aDataPackage->UnPackData( &contextName );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteDeactivatePacketContext Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if ( ( *contextName != iContextNameForComparison ) &&
         ( iHandleSpecifiedRequests ) )
        {
        aResult = KErrGeneral;
        }

    if( iDeactivateOngoing || iHandleSpecifiedRequests )
        {
        iDeactivateOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteDeactivatePacketContext") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::DeletePacketContextL
// Deleting packet context
// -----------------------------------------------------------------------------
//
TInt CPDI::DeletePacketContextL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8(KPDI , "CPDI::DeletePacketContext" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TPtrC contextnamestring;
    aItem.GetNextString( contextnamestring );
    TInfoName contextName = contextnamestring;
    iContextNameForComparison = contextnamestring;

    //Create package
    CMmDataPackage package;

    package.PackData(&contextName);

    // Set package data
    ret = HandleRequestL( EPacketContextDelete, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "DeletePacketContext request send ok" );
        iPDILog->Log((TDesC8)KSendOk );
        iDeleteOngoing = ETrue;
        }
    else
        {
        _LIT8( KSendFailed, "DeletePacketContext request send failed: %d" );
        iPDILog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteDeletePacketContext
// Complete deleting packet context
// -----------------------------------------------------------------------------
//
void CPDI::CompleteDeletePacketContext
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompleteDeletePacketContext result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteDeletePacketContext Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if( iDeleteOngoing || iHandleSpecifiedRequests )
        {
        iDeleteOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteDeletePacketContext") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::AddPacketFiltersL
// Adding Packet Filters
// -----------------------------------------------------------------------------
//
TInt CPDI::AddPacketFiltersL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::AddPacketFilters" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TBool supported( ETrue );

    TPtrC contextnamestring;
    TPtrC compression;
    TPtrC protocolstring;
    TInt destportmax;
    TInt destportmin;
    TInt flowlabel;
    TInt ipsecspi;
    TInt protocolnumberornextheader;
    TInt srcportmax;
    TInt srcportmin;
    TInt trafficclass;
    TInt id;

    aItem.GetNextString( contextnamestring );
    aItem.GetNextString( compression );
    aItem.GetNextString( protocolstring );
    aItem.GetNextInt( destportmax );
    aItem.GetNextInt( destportmin );
    aItem.GetNextInt( flowlabel );
    aItem.GetNextInt( ipsecspi );
    aItem.GetNextInt( protocolnumberornextheader );
    aItem.GetNextInt( srcportmax );
    aItem.GetNextInt( srcportmin );
    aItem.GetNextInt( trafficclass );
    aItem.GetNextInt( id );

    TInt iContextConfigurationType;
    iContextConfigurationType = TPacketDataConfigBase::KConfigGPRS;

    TInfoName contextName = contextnamestring;

    RPacketContext::TPacketFilterV2 filter = RPacketContext::TPacketFilterV2();

    filter.iDestPortMax = destportmax;
    filter.iDestPortMin = destportmin;
    filter.iFlowLabel= flowlabel;
    filter.iIPSecSPI = ipsecspi;
    filter.iProtocolNumberOrNextHeader = protocolnumberornextheader;
    filter.iSrcPortMax = srcportmax;
    filter.iSrcPortMin = srcportmin;
    filter.iTOSorTrafficClass = trafficclass;
    filter.iId = id;
    RPacketContext::TContextConfigGPRS config;

    if( _L("Data") == compression )
        {
        config.iPdpCompression = RPacketContext::KPdpDataCompression;
        }
    else if( _L("Header") == compression )
        {
        config.iPdpCompression = RPacketContext::KPdpHeaderCompression;
        }
    else
        {
        iPDILog->Log(_L8("Selected compression not supported"));
        supported = EFalse;
        }

    if( _L("PdpTypeIPv4") == protocolstring )
        {
        config.iPdpType = RPacketContext::EPdpTypeIPv4;
        }
    else if( _L("PdpTypeIPv6") == protocolstring )
        {
        config.iPdpType = RPacketContext::EPdpTypeIPv6;
        }
    else
        {
        _LIT8( KNotSupported, "Selected protocol type not supported" );
        iPDILog->Log((TDesC8)KNotSupported, ret );
        supported = EFalse;
        }

    iConfig = &config;

    CArrayFixFlat<RPacketContext::TPacketFilterV2>* iFilterTemporaryArray;

    iFilterTemporaryArray = new ( ELeave ) CArrayFixFlat<RPacketContext::TPacketFilterV2>( 1 );
    iFilterTemporaryArray->AppendL( filter );

    if( supported )
        {
        CMmPacketDataPackage package;

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
        package.SetConfigAndFilter( iConfig, iContextConfigurationType );

#else
        package.SetConfig( iConfig, iContextConfigurationType );
#endif
        package.PackData( &contextName, iFilterTemporaryArray );

        // Set package data
        ret = HandleRequestL( EPacketContextAddPacketFilter, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "AddPacketFilters request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "AddPacketFilters request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::AddMediaAuthorizationParamsL
// Adding Packet Filters
// -----------------------------------------------------------------------------
//
TInt CPDI::AddMediaAuthorizationParamsL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::AddMediaAuthorizationParams" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );

    TBool supported( ETrue );

    TPtrC contextnamestring;

    TPtrC compression;
    TPtrC protocolstring;

    TPtrC tokenname;
    TInt flownumber;
    TInt mediacomponentnumber;

    TInt flowcount;

    aItem.GetNextString( contextnamestring );
    aItem.GetNextString( compression );
    aItem.GetNextString( protocolstring );
    aItem.GetNextInt( flownumber );
    aItem.GetNextInt( mediacomponentnumber );
    aItem.GetNextString( tokenname );
    aItem.GetNextInt( flowcount );

    RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier fI;

    fI.iIPFlowNumber = flownumber;
    fI.iMediaComponentNumber = mediacomponentnumber;

    TInfoName contextName = contextnamestring;

    RPacketContext::CTFTMediaAuthorizationV3* maParams = RPacketContext::CTFTMediaAuthorizationV3::NewL();

    for(int i = 0; i < flowcount; i++)
        {
        maParams->iFlowIds.Append(fI);
        }

    TBuf8<255> token8bit;
    token8bit.Copy(tokenname);

    maParams->iAuthorizationToken = token8bit;

    RPacketContext::TContextConfigGPRS config;

    if( _L("Data") == compression )
        {
        config.iPdpCompression = RPacketContext::KPdpDataCompression;
        }
    else if( _L("Header") == compression )
        {
        config.iPdpCompression = RPacketContext::KPdpHeaderCompression;
        }
    else
        {
        iPDILog->Log(_L8("Selected compression not supported"));
        supported = EFalse;
        }

    if( _L("PdpTypeIPv4") == protocolstring )
        {
        config.iPdpType = RPacketContext::EPdpTypeIPv4;
        }
    else if( _L("PdpTypeIPv6") == protocolstring )
        {
        config.iPdpType = RPacketContext::EPdpTypeIPv6;
        }
    else
        {
        _LIT8( KNotSupported, "Selected protocol type not supported" );
        iPDILog->Log((TDesC8)KNotSupported, ret );
        supported = EFalse;
        }

    iConfig = &config;

    TInt iContextConfigurationType;
    iContextConfigurationType = TPacketDataConfigBase::KConfigGPRS;

    if( supported )
        {
        //Create package
        CMmPacketDataPackage package;

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
        package.SetConfigAndFilter( iConfig, iContextConfigurationType );
#else
        package.SetConfig( iConfig, iContextConfigurationType );
#endif


        package.PackData( &contextName, maParams);

        // Set package data
        ret = HandleRequestL( EPacketAddMediaAuthorization, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "AddMediaAuthorizationParams request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "AddMediaAuthorizationParams request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::ModifyPacketContextL
// Modifying Packet Context
// -----------------------------------------------------------------------------
//
TInt CPDI::ModifyPacketContextL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::ModifyPacketContext" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNotFound );
    TBool supported( ETrue );

    TPtrC contextnamestring;

    TPtrC minDelay;
    TPtrC minMeanThroughput;
    TPtrC minPeakThroughput;
    TPtrC minPrecedence;
    TPtrC minReliability;
    TPtrC reqDelay;
    TPtrC reqMeanThroughput;
    TPtrC reqPeakThroughput;
    TPtrC reqPrecedence;
    TPtrC reqReliability;

    TPtrC minTrafficClass;
    TPtrC minDeliveryOrderReq;
    TPtrC minDeliveryErroneusSDU;
    TPtrC maxBER;
    TPtrC maxSDUErrorRatio;
    TPtrC minTrafficHandlingPriority;

    TPtrC tftoperation;
    TPtrC pdpconfiguration;

    TPtrC filterauthorizationpresent;

    aItem.GetNextString( contextnamestring );

    aItem.GetNextString( minDelay );
    aItem.GetNextString( minMeanThroughput );
    aItem.GetNextString( minPeakThroughput );
    aItem.GetNextString( minPrecedence );
    aItem.GetNextString( minReliability );
    aItem.GetNextString( reqDelay );
    aItem.GetNextString( reqMeanThroughput );
    aItem.GetNextString( reqPeakThroughput );
    aItem.GetNextString( reqPrecedence );
    aItem.GetNextString( reqReliability );

    aItem.GetNextString( minTrafficClass );
    aItem.GetNextString( minDeliveryOrderReq );
    aItem.GetNextString( minDeliveryErroneusSDU );
    aItem.GetNextString( maxBER );
    aItem.GetNextString( maxSDUErrorRatio );
    aItem.GetNextString( minTrafficHandlingPriority );

    aItem.GetNextString( tftoperation );
    aItem.GetNextString( pdpconfiguration );

    aItem.GetNextString( filterauthorizationpresent );

    TContextConfiguration config;

    config.iContextName = contextnamestring;

    RPacketQoS::TQoSGPRSRequested profileGPRS;

    MapQoSGPRS( profileGPRS, &minDelay, &minMeanThroughput,
                    &minPeakThroughput, &minPrecedence, &minReliability,
                    &reqDelay, &reqMeanThroughput, &reqPeakThroughput,
                    &reqPrecedence, &reqReliability );

    if( minDelay == _L("x") )
        {
        config.iQosParams97 = NULL;
        }
    else
        {
        config.iQosParams97 = &profileGPRS;
        }

    RPacketQoS::TQoSR99_R4Requested profileR99_R4;

    MapQoSR99_4Params( profileR99_R4, &minTrafficClass, &minDeliveryOrderReq,
                        &minDeliveryErroneusSDU, &maxBER, &maxSDUErrorRatio,
                        &minTrafficHandlingPriority );

    if( minTrafficClass == _L("x") )
        {
        config.iQosParams99 = NULL;
        }
    else
        {
        config.iQosParams99 = &profileR99_R4;
        }

    if( _L("TftCreateNew") == tftoperation )
        {
        config.iTftOperation = ETftCreateNew;
        }
    else if( _L("DeleteExisting") == tftoperation )
        {
        config.iTftOperation = EDeleteExisting;
        }
    else if( _L("AddPacketFilters") == tftoperation )
        {
        config.iTftOperation = EAddPacketFilters;
        }
    else if( _L("ReplacePacketFilters") == tftoperation )
        {
        config.iTftOperation = EReplacePacketFilters;
        }
    else if( _L("DeletePacketFilters") == tftoperation )
        {
        config.iTftOperation = EDeletePacketFilters;

        for( TInt i=0; i < 8; i++ )
            {
            config.iFilterIdArray[i] = 0xFF;
            }
        }
    else if( _L("TftNoOperation") == tftoperation )
        {
        config.iTftOperation = ETftNoOperation;
        }
    else
        {
        iPDILog->Log(_L8("Selected tft operation not supported"));
        supported = EFalse;
        }

    if( _L("PdpTypeIPv4") == pdpconfiguration )
        {
        config.iConfiguration = RPacketContext::EPdpTypeIPv4;
        }
    else if( _L("PdpTypeIPv6") == pdpconfiguration )
        {
        config.iConfiguration = RPacketContext::EPdpTypeIPv6;
        }
    else
        {
        iPDILog->Log(_L8("Selected pdp configuration not supported"));
        supported = EFalse;
        }

    RPacketContext::TPacketFilterV2 filter = RPacketContext::TPacketFilterV2();

    RPacketContext::CTFTMediaAuthorizationV3* maParams = RPacketContext::CTFTMediaAuthorizationV3::NewL();
    RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier fI;

    if( ( _L("Packetfilter") == filterauthorizationpresent )
            || ( _L("Both") == filterauthorizationpresent ) )
        {
        filter.iDestPortMax = 65535;
        filter.iDestPortMin = 1;
        filter.iFlowLabel= 261072;
        filter.iIPSecSPI = 2222;
        filter.iProtocolNumberOrNextHeader = 20;
        filter.iSrcPortMax = 65535;
        filter.iSrcPortMin = 1;
        filter.iTOSorTrafficClass = 11;
        filter.iId = 1;
        filter.iEvaluationPrecedenceIndex = 100;

        iFilterArray = new ( ELeave ) CArrayFixFlat<RPacketContext::TPacketFilterV2>( 1 );
        iFilterArray->AppendL( filter );

        config.iTftFilterArray = iFilterArray;

        if( _L("Packetfilter") == filterauthorizationpresent ) iMediaAuthorizationArray = NULL;
        }
    else if( ( _L("Mediaauthorization") == filterauthorizationpresent )
            || ( _L("Both") == filterauthorizationpresent ) )
        {
        fI.iIPFlowNumber = 1;
        fI.iMediaComponentNumber = 1;

        maParams->iFlowIds.Append(fI);

        maParams->iAuthorizationToken = _L8("token");

        iMediaAuthorizationArray = new ( ELeave ) CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>( 1 );
        iMediaAuthorizationArray->AppendL( *maParams );

        iMediaAuthorizationArray->AppendL( *maParams );
        iMediaAuthorizationArray->AppendL( *maParams );

        if( _L("Mediaauthorization") == filterauthorizationpresent ) config.iTftFilterArray = NULL;
        }
    else if( _L("None") == filterauthorizationpresent )
        {
        iMediaAuthorizationArray = NULL;
        config.iTftFilterArray = NULL;
        }
    else
        {
        iPDILog->Log(_L8("Selected pdp configuration not supported"));
        supported = EFalse;
        }

    config.iQosParamsR5 = NULL;

    if( supported )
        {
        //Create package
        CMmDataPackage package;

        package.PackData(&config, &iMediaAuthorizationArray );

        // Set package data
        ret = HandleRequestL( EPacketContextModifyActiveContext, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "ModifyPacketContext request send ok" );
            iPDILog->Log((TDesC8)KSendOk );
            iModifyOngoing = ETrue;
            }
        else
            {
            _LIT8( KSendFailed, "ModifyPacketContext request send failed: %d" );
            iPDILog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteModifyPacketContext
// Complete Modifying Packet Context
// -----------------------------------------------------------------------------
//
void CPDI::CompleteModifyPacketContext
        (
        TInt aResult
        )
    {
    _LIT8( KResult, "CPDI::CompleteModifyPacketContext result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteModifyPacketContext Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    if( ( iModifyOngoing ) || ( iHandleSpecifiedRequests ))
        {
        iModifyOngoing = EFalse;
        Signal( aResult );
        iPDILog->Log( _L("signal CompleteModifyPacketContext") );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteTransferCapsChange
// CompleteTransferCapsChange method function.
// Completes transfer capabilities change.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteTransferCapsChange
        (
        TInt aResult, CMmDataPackage* aDataPackage
        )
    {
    TInt KHSDPA = 1;
    TInt result ( KErrNotFound );
    result = aResult;

    _LIT8( KResult, "CPDI::CompleteTransferCapsChange result: %d" );
    iPDILog->Log((TDesC8)KResult, result );

    if ( KErrNone == result )
        {
        // create temporary variable to hold the transfer capabilities
        TInt transferCaps ( 0 );

        // unpack the data, contains the transfer caps
        aDataPackage->UnPackData ( transferCaps );

        // Dynamic transfer caps:
        // Transfer caps for HSDPA is 0x00000001.
        // Transfer caps for HSUPA is 0x00000002.
        // Transfer caps for EGPRS is 0x00000004.

        // Check HSDPA
        if ( iHandleSpecifiedRequests )
            {
            _LIT8( KTransferCaps, "CPDI::CompleteTransferCapsChange transferCaps: %d" );
            iPDILog->Log((TDesC8)KTransferCaps, transferCaps );

            _LIT8( KITransferCaps, "CPDI::CompleteTransferCapsChange iTransferCaps: %d" );
            iPDILog->Log((TDesC8)KITransferCaps, iTransferCaps );

            if ( KErrNone == result )
                {
                _LIT8( KResultOfHsdpa, "HSDPA resource is allocated == TEST PASSED" );
                iPDILog->Log((TDesC8)KResultOfHsdpa );
                }
            else
                {
                _LIT8( KResultIsFail, "Result of test is not correct == TEST FAILED" );
                iPDILog->Log((TDesC8)KResultIsFail );

                result = KErrGeneral;
                }
            }
        //no else

        }
    else
        {
        _LIT8( KResultFail, "CompleteTransferCapsChange Failed: %d" );
        iPDILog->Log((TDesC8)KResultFail, result );
        }

    if ( iHandleSpecifiedRequests )
        {
        Signal(result);
        }
    //no else
    }

// -----------------------------------------------------------------------------
// CPDI::PacketContextNotifyStatusChange
// Notify context status change.
// -----------------------------------------------------------------------------
//
TInt CPDI::PacketContextNotifyStatusChange
        (
        CStifItemParser& /*aItem*/
        )
    {
    _LIT8( KContextNotifyStatus, "CPDI::PacketContextNotifyStatusChange" );
    iPDILog->Log( ( TDesC8 ) KContextNotifyStatus );

    iContextStatusChangeOngoing = ETrue;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketContextNotifyStatusChange
// Completes context status change notification.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketContextNotifyStatusChange
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    TInfoName contextName;

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    CMmCommonStaticUtility::TContextMisc misc;
#else
    TContextMisc misc;
#endif

    aDataPackage->UnPackData ( contextName, misc );

    TInt result = aResult;
    _LIT8( KPDI, "CPDI::CompletePacketContextNotifyStatusChange, result: %d" );
    iPDILog->Log( ( TDesC8 ) KPDI, result );
    iPDILog->Log( ( TDesC8 ) KLine );

    //Reserve enough buffer space for loging string mapping
    TBuf<40> status;

    switch ( misc.iStatus )
        {
        case RPacketContext::EStatusUnknown:
            {
            status.Append( _L( "Status: EStatusUnknown" ) );
            break;
            }
        case RPacketContext::EStatusInactive:
            {
            status.Append( _L( "Status: EStatusInactive" ) );
            break;
            }
        case RPacketContext::EStatusActivating:
            {
            status.Append( _L( "Status: EStatusActivating" ) );
            break;
            }
        case RPacketContext::EStatusActive:
            {
            status.Append( _L( "Status: EStatusActive" ) );
            break;
            }
        case RPacketContext::EStatusDeactivating:
            {
            status.Append( _L( "Status: EStatusDeactivating" ) );
            break;
            }
        case RPacketContext::EStatusSuspended:
            {
            status.Append( _L( "Status: EStatusSuspended" ) );
            break;
            }
        case RPacketContext::EStatusDeleted:
            {
            status.Append( _L( "Status: EStatusDeleted" ) );
            break;
            }
        default:
            {
            status.Append( _L( "Status: DEFAULT / NOT SUPPORTED!" ) );
            result = KErrGeneral;
            break;
            }
        }

    _LIT( KStatus, "Context name: %S, Status: %S" );
    iPDILog->Log( ( TDesC ) KStatus, &contextName, &status );
    iPDILog->Log( ( TDesC8 ) KLine );

    if ( ( iMiscForComparison.iStatus != misc.iStatus ) &&
         ( iHandleSpecifiedRequests ) )
        {
        result = KErrGeneral;
        }

    if ( KErrNone != result )
        {
        _LIT8( KResultFail, "CompletePacketContextNotifyStatusChange Failed: %d" );
        iPDILog->Log((TDesC8)KResultFail, result );
        }

    if ( iContextStatusChangeOngoing || iHandleSpecifiedRequests )
        {
        iContextStatusChangeOngoing = EFalse;
        _LIT8( KResultComplete, "PacketContextStatusChange Completed" );
        iPDILog->Log( ( TDesC8 ) KResultComplete );

        Signal( result );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::PacketContextNotifyStatusChange
// Notify context config change.
// -----------------------------------------------------------------------------
//
TInt CPDI::PacketContextNotifyConfigChanged
        (
        CStifItemParser& /*aItem*/
        )
    {
    _LIT8( KContextNotifyConfig, "CPDI::PacketContextNotifyConfigChanged" );
    iPDILog->Log( ( TDesC8 ) KContextNotifyConfig );

    iContextConfigChangeOngoing = ETrue;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketContextNotifyStatusChange
// Completes context config change notification.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketContextNotifyConfigChanged
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    TInt result( aResult );
    _LIT8( KPDI, "CPDI::CompletePacketContextNotifyConfigChanged, result: %d" );
    iPDILog->Log( ( TDesC8 ) KPDI, result );
    iPDILog->Log( ( TDesC8 ) KLine );

    RPacketContext::TContextConfigGPRS* config = NULL;
    TInfoName* contextName;
    aDataPackage->UnPackData( &contextName, &config );

    _LIT( KCName, "Context name: %S" );
    iPDILog->Log( ( TDesC ) KCName, contextName );

    if ( KErrNone == result )
        {
        if ( 0 == config->iPdpAddress.Compare( _L8( "174437928" ) ) )
            {
            _LIT( KAddress, "Received IPv4 PDP address: 10.101.182.40" );
            iPDILog->Log( KAddress );            
            }
        else if ( 0 == 
            config->iPdpAddress.Compare( _L8("2001:618:400:6a:0:0:0:abc") ) )
            {
            _LIT( KAddress, "Received IPv6 PDP address: 2001:618:400:6a:0:0:0:abc" );
            iPDILog->Log( KAddress );            
            }
        else
            {
            _LIT( KAddress, "PDP address does not match" );
            result = KErrGeneral;
            }
        }

    if ( KErrNone == result )
        {
        if ( 0 == 
            config->iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Compare( 
            _L8( "3227089894" ) ) )
            {
            _LIT( KAddress, "Received IPv4 PDNS address: 192.89.123.230" );
            iPDILog->Log( KAddress );            
            }
        else if ( 0 == 
            config->iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Compare(
            _L8("2001:618:400:6a:0:0:0:abd") ) )
            {
            _LIT( KAddress, "Received IPv6 PDNS address: 2001:618:400:6a:0:0:0:abd" );
            iPDILog->Log( KAddress );            
            }
        else
            {
            _LIT( KAddress, "PDNS address does not match" );
            result = KErrGeneral;
            }        
        }

    if ( KErrNone == result )
        {
        if ( 0 == 
            config->iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Compare( 
            _L8( "3227089895" ) ) )
            {
            _LIT( KAddress, "Received IPv4 SDNS address: 192.89.123.231" );
            iPDILog->Log( KAddress );            
            }
        else if ( 0 == 
            config->iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Compare(
            _L8("2001:618:400:6a:0:0:0:abe") ) )
            {
            _LIT( KAddress, "Received IPv6 SDNS address: 2001:618:400:6a:0:0:0:abe" );
            iPDILog->Log( KAddress );            
            }
        else
            {
            _LIT( KAddress, "SDNS address does not match" );
            result = KErrGeneral;
            }        
        }

    if ( KErrNone == result )
        {
        if( NULL != config && iPCSCFAddressRequested )
            {
            TBuf<RPacketContext::KMaxPDPAddressLength> addrBuff;

            GetPCSCFAddress(
                addrBuff, *config );

            if ( 0 == addrBuff.Compare( _L( "1.2.3.4" ) ) )
                {
                _LIT( KAddress, "Received IPv4 P-CSCF address: %s" );
                iPDILog->Log( ( TDesC )KAddress, addrBuff.PtrZ() );
                }
            else if ( 0 == addrBuff.Compare( _L( "2001:490:ff0:c195::10" ) ) )
                {
                _LIT( KAddress, "Received IPv6 P-CSCF address: %s" );
                iPDILog->Log( ( TDesC )KAddress, addrBuff.PtrZ() );
                }
            else
                {
                _LIT( KAddressError, "P-CSCF address doesn't match!" );
                iPDILog->Log( ( TDesC )KAddressError );
                result = KErrGeneral;
                }
            }
        //no else
        }
    //no else

    iPDILog->Log( ( TDesC8 ) KLine );

    if ( ( *contextName != iContextNameForComparison ) &&
         ( iHandleSpecifiedRequests ) )
        {
        result = KErrGeneral;
        }

    if ( KErrNone != result )
        {
        _LIT8( KResultFail, "CompletePacketContextNotifyConfigChanged Failed: %d" );
        iPDILog->Log( ( TDesC8 ) KResultFail, result );
        }
    //no else

    if ( iContextConfigChangeOngoing || iHandleSpecifiedRequests )
        {
        iContextConfigChangeOngoing = EFalse;
        _LIT8( KResultComplete, "PacketContextConfigChange Completed" );
        iPDILog->Log( ( TDesC8 ) KResultComplete );

        Signal( result );
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CPDI::AddPCSCFAddressDiscoveryRequest
// Add P-CSCF address discovery request into
// configuration / iProtocolConfigOption.iMiscBuffer.
// ----------------------------------------------------------------------------
//
void CPDI::AddPCSCFAddressDiscoveryRequest
        (
        TPacketDataConfigBase& aContextConfig
        )
    {
    _LIT8( KPCSCFAddressReq, "CPDI::AddPCSCFAddressDiscoveryRequest" );
    iPDILog->Log( ( TDesC8 ) KPCSCFAddressReq );

    // Request P-CSCF address discovery
    RPacketContext::TPcoId pcoItem(
        RPacketContext::EEtelPcktPCSCFAddressRequest );

    if ( TPacketDataConfigBase::KConfigGPRS == aContextConfig.ExtensionId() )
        {
        RPacketContext::TContextConfigGPRS& config =
            ( RPacketContext::TContextConfigGPRS& )aContextConfig;

        TPtr8 ptrToTLVdata(
            const_cast<TUint8*>(
                config.iProtocolConfigOption.iMiscBuffer.Ptr() ),
            config.iProtocolConfigOption.iMiscBuffer.Length(),
            config.iProtocolConfigOption.iMiscBuffer.MaxLength() );

        TTlvStruct<RPacketContext::TPcoId, RPacketContext::TPcoItemDataLength>
            tlv( ptrToTLVdata, 0 );

        tlv.AppendItemL( pcoItem, TPtr8( static_cast<TUint8*>( NULL ), 0, 0  ) );

        config.iProtocolConfigOption.iMiscBuffer.SetLength(
            ptrToTLVdata.Length() );
        }
    else
        {
        RPacketContext::TContextConfigR99_R4& config =
            ( RPacketContext::TContextConfigR99_R4& )aContextConfig;

        TPtr8 ptrToTLVdata(
            const_cast<TUint8*>(
                config.iProtocolConfigOption.iMiscBuffer.Ptr() ),
            config.iProtocolConfigOption.iMiscBuffer.Length(),
            config.iProtocolConfigOption.iMiscBuffer.MaxLength() );

        TTlvStruct<RPacketContext::TPcoId, RPacketContext::TPcoItemDataLength>
            tlv( ptrToTLVdata, 0 );

        tlv.AppendItemL( pcoItem, TPtr8( static_cast<TUint8*>( NULL ), 0, 0) );

        config.iProtocolConfigOption.iMiscBuffer.SetLength(
            ptrToTLVdata.Length() );
        }
    }

// -----------------------------------------------------------------------------
// CPDI::GetPCSCFAddress
// Get P-CSCF address from configuration / iProtocolConfigOption.iMiscBuffer.
// -----------------------------------------------------------------------------
//
void CPDI::GetPCSCFAddress
        (
        TDes& aAddress,
        const RPacketContext::TContextConfigGPRS& aConfig
        )
    {
    RPacketContext::TPcoId pcoItem(
        RPacketContext::EEtelPcktPCSCFAddress );

    TPtr8 ptrToTLVdata(
        const_cast<TUint8*>( aConfig.iProtocolConfigOption.
            iMiscBuffer.Ptr() ),
        aConfig.iProtocolConfigOption.iMiscBuffer.Length(),
        aConfig.iProtocolConfigOption.iMiscBuffer.MaxLength() );

    TTlvStruct<RPacketContext::TPcoId,
        RPacketContext::TPcoItemDataLength> tlv( ptrToTLVdata, 0 );

    TPtr8 addrPtr( NULL, 0 );

    TInt leaveCode( KErrNone );
    TInt pcoItemValue( KErrNone );
    TInt addrCount( 0 );

    while ( KErrNone == leaveCode && KErrNone == pcoItemValue )
        {
        TRAP( leaveCode, pcoItemValue = tlv.NextItemL( pcoItem, addrPtr ) );
        if ( KErrNone == leaveCode && KErrNone == pcoItemValue )
            {
            if ( 4 == addrPtr.Length() )
                {
                TInetAddr inetAddr;
                inetAddr.SetAddress(
                    INET_ADDR(
                        addrPtr[0], addrPtr[1], addrPtr[2], addrPtr[3] ) );
                inetAddr.Output( aAddress );
                }
            else if ( 16 == addrPtr.Length() )
                {
                TIp6Addr ip6addr;
                TPckg<TIp6Addr> addrPckg( ip6addr );
                TInetAddr inetAddr;
                addrPckg.Copy( addrPtr );
                inetAddr.SetAddress( ip6addr );
                inetAddr.Output( aAddress );
                }
            addrCount++;
            }
        }

    _LIT8( KLogging, "Received P-CSCF address count: %d" );
    iPDILog->Log( ( TDesC8 ) KLogging, addrCount );
    }

// -----------------------------------------------------------------------------
// CPDI::SetNotifyDynamicCapsChangeRefParamsL
// Setting Notify Dynamic Caps Change Reference Parameters
// -----------------------------------------------------------------------------
//
TInt CPDI::SetNotifyDynamicCapsChangeRefParamsL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetNotifyDynamicCapsChangeRefParamsL" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNone );

    aItem.GetNextInt( iLastGprsSupportInCellForComparison );
    aItem.GetNextInt( iLastCsServicesForComparison );

    return ret;
	}

// -----------------------------------------------------------------------------
// CPDI::SetNotifyTransferCapsRefParamsL
// Setting Notify Transfer Caps Reference Parameters
// -----------------------------------------------------------------------------
//
TInt CPDI::SetNotifyTransferCapsRefParamsL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetNotifyTransferCapsRefParamsL" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNone );
    TInt lastHSDPAAllocatedStatusForComparison( 0 );
    TInt lastEGPRSStatusForComparison( 0 );

    aItem.GetNextInt( lastHSDPAAllocatedStatusForComparison );
    aItem.GetNextInt( lastEGPRSStatusForComparison );

    _LIT8( KlastHSDPAAllocatedStatusForComparison, "lastHSDPAAllocatedStatusForComparison: %d" );
    iPDILog->Log( ( TDesC8 ) KlastHSDPAAllocatedStatusForComparison, lastHSDPAAllocatedStatusForComparison );

    _LIT8( KlastEGPRSStatusForComparison, "lastEGPRSStatusForComparison: %d" );
    iPDILog->Log( ( TDesC8 ) KlastEGPRSStatusForComparison, lastEGPRSStatusForComparison );

    // transfer capabilities
    // Dynamic transfer caps:
    // Transfer caps for HSDPA is 0x00000001.
    // Transfer caps for HSUPA is 0x00000002.
    // Transfer caps for EGPRS is 0x00000004.

    if ( lastHSDPAAllocatedStatusForComparison && !lastEGPRSStatusForComparison )
        {
        iTransferCaps = 1;
        }
    else if( !lastHSDPAAllocatedStatusForComparison && lastEGPRSStatusForComparison )
        {
        iTransferCaps = 4;
        }
    else if( lastHSDPAAllocatedStatusForComparison && lastEGPRSStatusForComparison )
        {
        iTransferCaps = 5;
        }
    else
        {
        iTransferCaps = 0;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CPDI::CompleteNotifyDynamicCapsChange
// Complete CompleteNotifyDynamicCapsChange method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteNotifyDynamicCapsChange
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    TBool lastGprsSupportInCell;
    TBool lastCsServices;

    aDataPackage->UnPackData( lastGprsSupportInCell, lastCsServices );

    if ( ( lastGprsSupportInCell != iLastGprsSupportInCellForComparison ) ||
         ( lastCsServices != iLastCsServicesForComparison ) )
        {
        aResult = KErrGeneral;
        }

    _LIT8( KResult, "CPDI::CompleteNotifyDynamicCapsChange result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteNotifyDynamicCapsChange Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CPDI::SetNotifyMSClassChangeRefParamsL
// Setting Notify MSClass Change Reference Parameters
// -----------------------------------------------------------------------------
//
TInt CPDI::SetNotifyMSClassChangeRefParamsL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetNotifyMSClassChangeRefParamsL" );
    iPDILog->Log((TDesC8)KPDI );

    TInt ret( KErrNone );

    TPtrC string;
    aItem.GetNextString ( string );

    iMsClassForComparison = RPacketService::EMSClassUnknown;

    if ( _L("EMSClassSuspensionRequired") == string )
        {
        iPDILog->Log( _L("EMSClassSuspensionRequired.") );
        iMsClassForComparison = RPacketService::EMSClassSuspensionRequired;
        }

    return ret;
	}

// -----------------------------------------------------------------------------
// CPDI::CompleteNotifyMSClassChange
// Complete CompleteNotifyMSClassChange method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompleteNotifyMSClassChange
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompleteNotifyMSClassChange result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );
    RPacketService::TMSClass msClass( RPacketService::EMSClassUnknown );

    aDataPackage->UnPackData( msClass );

    if ( msClass != iMsClassForComparison )
        {
        aResult = KErrGeneral;
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteNotifyMSClassChange Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketNotifyContextAdded
// Complete CompletePacketNotifyContextAdded method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketNotifyContextAdded
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompletePacketNotifyContextAdded result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );
    TInfoName contextName;

    aDataPackage->UnPackData( contextName );

    if ( contextName != iContextNameForComparison )
        {
        aResult = KErrGeneral;
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePacketNotifyContextAdded Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketQoSNotifyProfileChanged
// Complete CompletePacketQoSNotifyProfileChanged method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketQoSNotifyProfileChanged
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompletePacketQoSNotifyProfileChanged result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    // Data package
    const CMmPacketDataPackage* data = reinterpret_cast<const CMmPacketDataPackage*>( aDataPackage );

    // Negotieated QoSGPRS profile
    RPacketQoS::TQoSGPRSNegotiated iQoSGPRSNegotiated;

    // Negotieated QoS 99 parameters
    RPacketQoS::TQoSR99_R4Negotiated iQoSR99_R4Negotiated;

    // Negotiated QoS rel5 parameters
    RPacketQoS::TQoSR5Negotiated iQoSR5Negotiated;

    // Context name
    TInfoName contextName;


    //Unpack/get QoS parameters
    data->GetProfileChanged( iQoSGPRSNegotiated, iQoSR99_R4Negotiated, iQoSR5Negotiated );

    if ( 0 == iProfile.Compare ( _L ( "GPRS" ) ) )
        {
        if ( ( iQoSGPRSNegotiated.iMeanThroughput != iProfileGPRS.iMinMeanThroughput ) ||
             ( iQoSGPRSNegotiated.iPeakThroughput != iProfileGPRS.iMinPeakThroughput ) ||
             ( iQoSGPRSNegotiated.iPrecedence != iProfileGPRS.iMinPrecedence ) ||
             ( iQoSGPRSNegotiated.iReliability != iProfileGPRS.iMinReliability ) ||
             ( iQoSGPRSNegotiated.iDelay != iProfileGPRS.iMinDelay ) )
            {
            aResult = KErrGeneral;
            }
        }
    else if ( iProfile.Compare ( _L ( "R99_R4" ) ) )
        {
        }
    else if ( iProfile.Compare ( _L ( "R5" ) ) )
        {
        if ( ( iQoSR5Negotiated.iTrafficClass != iProfileR5.iMinTrafficClass ) ||
             ( iQoSR5Negotiated.iDeliveryOrderReqd != iProfileR5.iMinDeliveryOrderReqd ) ||
             ( iQoSR5Negotiated.iDeliverErroneousSDU != iProfileR5.iMinDeliverErroneousSDU ) ||
             ( iQoSR5Negotiated.iBER != iProfileR5.iMaxBER ) ||
             ( iQoSR5Negotiated.iSDUErrorRatio != iProfileR5.iMaxSDUErrorRatio ) ||
             ( iQoSR5Negotiated.iTrafficHandlingPriority != iProfileR5.iMinTrafficHandlingPriority ) )
            {
            aResult = KErrGeneral;
            }
        }

    //Unpack context name
    data->UnPackData( contextName );

    if ( contextName != iContextNameForComparison )
        {
        aResult = KErrGeneral;
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePacketQoSNotifyProfileChanged Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CPDI::CompletePacketContextNotifyConnectionSpeedChange
// Complete CompletePacketContextNotifyConnectionSpeedChange method function.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketContextNotifyConnectionSpeedChange
        (
        TInt aResult,
        CMmDataPackage* aDataPackage
        )
    {
    _LIT8( KResult, "CPDI::CompletePacketContextNotifyConnectionSpeedChange result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );
    TInfoName contextName;
    TInt connectionSpeedClient( 0 );

    aDataPackage->UnPackData( contextName, connectionSpeedClient );

    if ( ( contextName != iContextNameForComparison ) ||
         ( connectionSpeedClient != iConnectionSpeedClientForComparison ) )
        {
        aResult = KErrGeneral;
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompletePacketContextNotifyConnectionSpeedChange Failed: %d" );
        iPDILog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CPDI::SetExpectedPacketContextNotifyConnectionSpeedL
// Set expected connection speed value for comparison
// -----------------------------------------------------------------------------
//
TInt CPDI::SetExpectedPacketContextNotifyConnectionSpeedL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetExpectedPacketContextNotifyConnectionSpeed" );
    iPDILog->Log((TDesC8)KPDI );

    aItem.GetNextInt( iConnectionSpeedClientForComparison );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPDI::SetExpectedContextMicsStatusL
// Set expected connection speed value for comparison
// -----------------------------------------------------------------------------
//
TInt CPDI::SetExpectedContextMicsStatusL
        (
        CStifItemParser& aItem
        )
    {
    _LIT8( KPDI, "CPDI::SetExpectedContextMicsStatus" );
    iPDILog->Log((TDesC8)KPDI );

    if ( iHandleSpecifiedRequests )
        {
        TPtrC contextmisc;
        aItem.GetNextString( contextmisc );

        if( _L("EStatusDeactivating") == contextmisc )
            {
            iMiscForComparison.iStatus = RPacketContext::EStatusDeactivating;
            }
        else if( _L("EStatusInactive") == contextmisc )
            {
            iMiscForComparison.iStatus = RPacketContext::EStatusInactive;
            }
        else if( _L("EStatusDeleted") == contextmisc )
            {
            iMiscForComparison.iStatus = RPacketContext::EStatusDeleted;
            }
        }

    return KErrNone;
    }

// CPDI::CompletePacketNotifyStatusChange
// Completes packet status change.
// -----------------------------------------------------------------------------
//
void CPDI::CompletePacketNotifyStatusChange
    (
    TInt aResult,
    CMmDataPackage* aDataPackage
    )
    {
    _LIT8( KResult, "CPDI::CompletePacketNotifyStatusChange result: %d" );
    iPDILog->Log((TDesC8)KResult, aResult );

    // unpack data
    TBool resumed;
    RPacketService::TStatus packetStatus;
    aDataPackage->UnPackData( packetStatus, resumed );

    // Parameter checking
    if ( packetStatus != iPacketStatus || resumed != iResumed )
        {
        aResult = KErrGeneral;
        }

    Signal(aResult);

    }

// -----------------------------------------------------------------------------
// CPDI::SetPdiReferenceParameters
// Sets PDI reference parameters for parameter checking
// -----------------------------------------------------------------------------
//
TInt CPDI::SetPdiReferenceParameters( CStifItemParser& aItem )
    {
    _LIT8( KSetPdiReferenceParameters, "PDI::SetPdiReferenceParameters" );
    iPDILog->Log( (TDesC8) KSetPdiReferenceParameters );

    // First parameter is test case number
    TInt testCaseNumber;
    aItem.GetNextInt( testCaseNumber );

    // Set reference parameters depending on test case number
    if ( 708 == testCaseNumber )
        {
        // Packet status
        TInt temp;
        aItem.GetNextInt( temp );
        iPacketStatus = (RPacketService::TStatus)temp;
        // Resume info
        aItem.GetNextInt( iResumed );
        }
    else if ( 803 == testCaseNumber )
        {
        // PacketStatus
        TInt temp;
        aItem.GetNextInt( temp );
        iPacketStatus = (RPacketService::TStatus)temp;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPDI::SetContextNameForComparison
// Set reference context name received upon NTSY completing
// EPacketContextActivate
// -----------------------------------------------------------------------------
//
TInt CPDI::SetContextNameForComparison( CStifItemParser& aItem )
    {
    _LIT8( KSetContextNameForComparison, "PDI::SetContextNameForComparison" );
    iPDILog->Log( (TDesC8) KSetContextNameForComparison );

    TPtrC refContextName;
    aItem.GetNextString( refContextName );

    iContextNameForComparison = refContextName;

    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

// End of File

