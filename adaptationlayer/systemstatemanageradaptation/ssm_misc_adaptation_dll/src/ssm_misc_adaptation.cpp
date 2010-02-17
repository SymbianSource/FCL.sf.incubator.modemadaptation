/*
* ==============================================================================
*  Name:           ssm_misc_adaptation.cpp
*  Part of:        SSMA MiscAdaptation
*  Interface:      N/A
*  Description:    Implementation of CMiscAdaptation class
*  %version:       2 %
*  %date_modified: Fri Feb 12 10:56:14 2010 %
*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of miscadaptation.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include "ssm_misc_adaptation.h"
#include "ssm/startupadaptationcommands.h"
#include "sa_common_trace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ssm_misc_adaptationtraces.h"
#endif

// EXTERNAL DATA STRUCTURES 
// EXTERNAL FUNCTION PROTOTYPES 
// FORWARD DECLARATIONS
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES  
// LOCAL FUNCTION PROTOTYPES 


// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Function to create new Misc Adaptation Plugin. Used for a collection of 
// activities.
// -----------------------------------------------------------------------------
//
EXPORT_C MMiscAdaptation* CreateMiscAdaptationL()
    {
    CMiscAdaptation* miscAdaptation = CMiscAdaptation::NewL();
    return ( static_cast<MMiscAdaptation*>( miscAdaptation ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::NewL 
// Constructor
// -----------------------------------------------------------------------------
//
CMiscAdaptation* CMiscAdaptation::NewL()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_NEWL_1,
               "SSMA # CMiscAdaptation::NewL" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::NewL" ) );

    CMiscAdaptation* self = new( ELeave ) CMiscAdaptation;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTrace1( TRACE_API, CMISCADAPTATION_NEWL_2,
               "SSMA # CEmergencyCallRfAdaptation::NewL - return 0x%08x", self );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::NewL - return 0x%08x"), self ));
    return self;
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::~CMiscAdaptation
// Destructor
// -----------------------------------------------------------------------------
//
CMiscAdaptation::~CMiscAdaptation()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_CMISCADAPTATION_D1,
               "SSMA # CMiscAdaptation::~CMiscAdaptation" );
    INTERNAL_TRACE( _L("SSMA # CMiscAdaptation::~CMiscAdaptation" ) );

    // Close and release DMC
    iDmc.Close();

    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_CMISCADAPTATION_D2,
               "SSMA # CMiscAdaptation::~CMiscAdaptation - return void" );
    INTERNAL_TRACE( _L("SSMA # CMiscAdaptation::~CMiscAdaptation - return void" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::CMiscAdaptation 
// Constructor
// -----------------------------------------------------------------------------
//
CMiscAdaptation::CMiscAdaptation()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_CMISCADAPTATION,
               "SSMA # CMiscAdaptation::CMiscAdaptation" );
    INTERNAL_TRACE( _L("SSMA # CMiscAdaptation::CMiscAdaptation" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::ConstructL
// -----------------------------------------------------------------------------
//
void CMiscAdaptation::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_CONSTRUCTL_1,
               "SSMA # CMiscAdaptation::ConstructL" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::ConstructL" ) );

    // Open DMC
    iDmc.Open();

    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_CONSTRUCTL_2,
               "SSMA # CMiscAdaptation::ConstructL - return void" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::ConstructL return void" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::Release
// Deletes and frees memory allocated.
// -----------------------------------------------------------------------------
// 
void CMiscAdaptation::Release()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_RELEASE,
               "SSMA # CMiscAdaptation::Release" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::Release" ) );

    delete this;
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::GetGlobalStartupMode
// Get the start-up mode from the Cooperating System.
// -----------------------------------------------------------------------------
//
void CMiscAdaptation::GetGlobalStartupMode( TDes8& aModePckg, 
                                            TRequestStatus& aStatus )
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_GETGLOBALSTARTUPMODE_1,
               "SSMA # CMiscAdaptation::GetGlobalStartupMode" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::GetGlobalStartupMode" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;

    StartupAdaptation::TGlobalStartupMode mode = StartupAdaptation::ENormal;

    // Get start up mode from DMC.
    RDmc::TTargetStartupMode dmcTargetMode( RDmc::EStartupModeNormal );

    TInt err = iDmc.GetStartUpMode( dmcTargetMode );

    OstTrace1( TRACE_NORMAL, CMISCADAPTATION_GETGLOBALSTARTUPMODE_2, 
               "SSMA # CMiscAdaptation::GetGlobalStartupMode; dmcTargetMode=%d", 
               dmcTargetMode );
    INTERNAL_TRACE( ( _L( "SSMA # CMiscAdaptation::GetGlobalStartupMode; dmcTargetMode=%d" ),
                      dmcTargetMode ) );

    if ( err == KErrNone )
        {
        switch( dmcTargetMode )
            {
            // Map startup from DMC mode to SSM mode.
            case RDmc::EStartupModeNormal:
                {
                mode = StartupAdaptation::ENormal;
                break;
                }
            case RDmc::EStartupModeAlarm:
                {
                mode = StartupAdaptation::EAlarm;
                break;
                }
            case RDmc::EStartupModeCharging:
                {
                mode = StartupAdaptation::ECharging;
                break;
                }
            case RDmc::EStartupModeTest:
                {
                mode = StartupAdaptation::ETest;
                break;
                }
            case RDmc::EStartupModeNone:
            case RDmc::EStartupModeFota:
            default:
                {
                err = KErrNotSupported;
                break;
                }
            }
        }
    
    OstTraceExt2( TRACE_NORMAL, CMISCADAPTATION_GETGLOBALSTARTUPMODE_3,
                  "SSMA # CMiscAdaptation::GetGlobalStartupMode; mode=%d; err=%d", 
                  mode, err );
    INTERNAL_TRACE( ( _L( "SSMA # CMiscAdaptation::GetGlobalStartupMode; mode=%d; err=%d" ),
                      mode, err ) );
    
    // Startupmode should be returned to SSM at aModePckg param.
    TPckgBuf<StartupAdaptation::TGlobalStartupMode> startUpMode( mode );
    aModePckg = startUpMode;
    User::RequestComplete( status, err );

    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_GETGLOBALSTARTUPMODE_4,
               "SSMA # CMiscAdaptation::GetGlobalStartupMode - return void" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::GetGlobalStartupMode - return void" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::PrepareSimLanguages
// Prepare language codes stored in the SIM in preferred language lists. 
// The next call should be GetSimLanguages()
// -----------------------------------------------------------------------------
//
void CMiscAdaptation::PrepareSimLanguages( TSsmLanguageListPriority aPriority, 
                                           TDes8& aSizePckg, 
                                           TRequestStatus& aStatus )
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_PREPARESIMLANGUAGES_1,
               "SSMA # CMiscAdaptation::PrepareSimLanguages" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::PrepareSimLanguages" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    
    TInt languageCount(1);
    
    switch( aPriority )
        {
        case EPrimaryLanguages:
            break;
        case ESecondaryLanguages:
            break;
        case ETertiaryLanguages:
            break;
        default:
            break;
        }

    OstTraceExt2( TRACE_NORMAL, CMISCADAPTATION_PREPARESIMLANGUAGES_2,
               "SSMA # CMiscAdaptation::PrepareSimLanguages - aPriority:%d languageCount:%d",
               aPriority, languageCount );
    INTERNAL_TRACE( ( _L( "SSMA # CMiscAdaptation::PrepareSimLanguages - aPriority:%d languageCount:%d" ),
                          aPriority, languageCount ) );

    TPckgBuf<TInt> returnValue(languageCount);
    aSizePckg = returnValue;

    User::RequestComplete( status, KErrNone );

    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_PREPARESIMLANGUAGES_3,
               "SSMA # CMiscAdaptation::PrepareSimLanguages - return void" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::PrepareSimLanguages - return void" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::GetSimLanguagesL
// Get language codes stored in the SIM in preferred language lists. 
// PrepareSimLanguages() call should precede this call.
// -----------------------------------------------------------------------------
//
void CMiscAdaptation::GetSimLanguagesL( CBufBase* aBuf, 
                                        TInt aCount )
    {
    OstTrace1( TRACE_NORMAL, CMISCADAPTATION_GETSIMLANGUAGES_1,
               "SSMA # CMiscAdaptation::GetSimLanguages - aCount:%d", aCount );
    INTERNAL_TRACE( ( _L( "SSMA # CMiscAdaptation::GetSimLanguages - aCount:%d" ), aCount ) );


    StartupAdaptation::RLanguageList languageList;
    // only as an example...
    for( TInt i=0; i<aCount; i++)
        {
        languageList.Append(StartupAdaptation::EPrefLangEnglish);
        }

    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_GETSIMLANGUAGES_2,
               "SSMA # CMiscAdaptation::GetSimLanguages - write languages to stream." );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::GetSimLanguages - write languages to stream." ) );

    RBufWriteStream writeStream(*aBuf);
    CleanupClosePushL(writeStream); 
    
    for (TInt i=0; i<aCount; i++)
        {
        writeStream.WriteInt32L(languageList[i]);
        }
    writeStream.CommitL();
    CleanupStack::PopAndDestroy();
    languageList.Close();

    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_GETSIMLANGUAGES_3,
               "SSMA # CMiscAdaptation::GetSimLanguages - return void" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::GetSimLanguages - return void" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::GetHiddenReset
// Retrieve information about Hidden reset.
// -----------------------------------------------------------------------------
//
void CMiscAdaptation::GetHiddenReset( TDes8& aHiddenResetPckg,
                                      TRequestStatus& aStatus )
    {
    TBool iHiddenResetStatus = EFalse;
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_GETHIDDENRESET_1,
               "SSMA # CMiscAdaptation::GetHiddenReset" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::GetHiddenReset" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;

    TInt err = iDmc.IsHiddenReset( iHiddenResetStatus );
    
    OstTraceExt2( TRACE_NORMAL, CMISCADAPTATION_GETHIDDENRESET_2, 
                   "SSMA # CMiscAdaptation::GetHiddenReset IsHiddenReset; Status = %d; Error = %d",
                   iHiddenResetStatus, err );
    INTERNAL_TRACE( ( _L( "SSMA # CMiscAdaptation::GetHiddenReset IsHiddenReset Status = %d; Error = %d" ),
                      iHiddenResetStatus, err ) );

    // Startupmode should be returned to SSM at aHiddenResetPckg param.
    TPckgBuf<TBool> hiddenReset( iHiddenResetStatus );
    aHiddenResetPckg = hiddenReset;
    User::RequestComplete( status, err );

    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_GETHIDDENRESET_3,
               "SSMA # CMiscAdaptation::GetHiddenReset - return void" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::GetHiddenReset - return void" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::Cancel
// Cancel the outstanding request. Reference implementation completes the 
// requests immediately so there is nothing to Cancel.
// On a device, Cancel() needs an implementation as the Request might be 
// outstanding and it needs to be cancelled.
// -----------------------------------------------------------------------------
//
void CMiscAdaptation::Cancel()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_CANCEL,
               "SSMA # CMiscAdaptation::Cancel" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::Cancel" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptation::SecurityStateChange
// Request the next security state.
// -----------------------------------------------------------------------------
//
void CMiscAdaptation::SecurityStateChange( TInt aState,
                                           TDes8& aResponsePckg,
                                           TRequestStatus& aStatus )
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATION_SECURITYSTATECHANGE,
               "SSMA # CMiscAdaptation::SecurityStateChange" );
    INTERNAL_TRACE( _L( "SSMA # CMiscAdaptation::SecurityStateChange" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    StartupAdaptation::TSecurityStateInfo info = StartupAdaptation::EYes;

    switch( aState )
        {
        case StartupAdaptation::ESIMPresent:
            info = StartupAdaptation::ENo;
            break;
        case StartupAdaptation::ESIMReadable:
            info = StartupAdaptation::EYes;
            break;
        case StartupAdaptation::ESIMInvalid:
            info = StartupAdaptation::ENo;
            break;
        case StartupAdaptation::ESIMRejected:
            info = StartupAdaptation::ENo;
            break;
        case StartupAdaptation::ESIMBlocked:
            info = StartupAdaptation::ENo;
            break;
        case StartupAdaptation::EPINRequired:
            info = StartupAdaptation::ENo;
            break;
        case StartupAdaptation::ESIMCodesOK:
            info = StartupAdaptation::EYes;
            break;
        case StartupAdaptation::ESIMLock:
            info = StartupAdaptation::ESimLockOk;
            break;
        case StartupAdaptation::ESecurityCheckOK:
            info = StartupAdaptation::EYes; 
            break;  
        case StartupAdaptation::ESecurityCheckFailed:
            info = StartupAdaptation::ENo;
            break;  
        case StartupAdaptation::ESIMLessOfflineSupported:
            info = StartupAdaptation::EYes; 
            break; 
        case StartupAdaptation::ESECCodeRequired:
            info = StartupAdaptation::ENo; 
            break; 
        case StartupAdaptation::ESECCodeOK:
            info = StartupAdaptation::EYes; 
            break; 
        default:
            break;
        }
        
    OstTraceExt2( TRACE_NORMAL, CMISCADAPTATION_SECURITYSTATECHANGE_2,
               "SSMA # CMiscAdaptation::SecurityStateChange: aState:%d info:%d", aState, info );
    INTERNAL_TRACE( ( _L( "SSMA # CMiscAdaptation::SecurityStateChange: aState:%d info:%d" ), 
                          aState, info ) );
    
    TPckgBuf<StartupAdaptation::TSecurityStateInfo> result( info );
    aResponsePckg = result;  
    User::RequestComplete( status, KErrNone );
    }

// End of File

