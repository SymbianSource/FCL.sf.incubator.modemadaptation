/*
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
#include "miscadaptationref.h"
#include "ssm/startupadaptationcommands.h"

#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "miscadaptationrefTraces.h"
#endif

// EXTERNAL DATA STRUCTURES 
// EXTERNAL FUNCTION PROTOTYPES 
// FORWARD DECLARATIONS
// CONSTANTS

// MACROS
#ifdef _DEBUG
    #define MISC_TRACE(a)         RDebug::Print a

#else // _DEBUG
    #define MISC_TRACE(a)

#endif // _DEBUG

// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES  
// LOCAL FUNCTION PROTOTYPES 

_LIT( KTestCmdSecurityCheckTestFile, "c:\\cmdsecuritychecktest\\pinchecksecuritycaseno.txt" );

// ==================== LOCAL FUNCTIONS ====================

// -----------------------------------------------------------------------------
// Function to create new Misc Adaptation Plugin. Used for a collection of 
// activities.
// -----------------------------------------------------------------------------
//
EXPORT_C MMiscAdaptation* CreateMiscAdaptationRefL()
    {
    CMiscAdaptationRef* miscAdaptationRef = CMiscAdaptationRef::NewL();
    return ( static_cast<MMiscAdaptation*>( miscAdaptationRef ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::NewL 
// Constructor
// -----------------------------------------------------------------------------
//
CMiscAdaptationRef* CMiscAdaptationRef::NewL()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_NEWL,
        "CMiscAdaptationRef::NewL" );
    MISC_TRACE( _L( "CMiscAdaptationRef::NewL" ) );

    CMiscAdaptationRef* self = new( ELeave ) CMiscAdaptationRef;
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::~CMiscAdaptationRef
// Destructor
// -----------------------------------------------------------------------------
//
CMiscAdaptationRef::~CMiscAdaptationRef()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_CMISCADAPTATIONREF,
        "CMiscAdaptationRef::~CMiscAdaptationRef" );
    MISC_TRACE( _L("CMiscAdaptationRef::~CMiscAdaptationRef" ) );

    iFs.Close();

    // Close and release DMC
    iDmc.Close();
    }

CMiscAdaptationRef::CMiscAdaptationRef()
    {
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::ConstructL
// -----------------------------------------------------------------------------
//
void CMiscAdaptationRef::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_CONSTRUCTL,
        "CMiscAdaptationRef::ConstructL" );
    MISC_TRACE( _L( "CMiscAdaptationRef::ConstructL" ) );

    iTestCaseNum = 1;
    User::LeaveIfError( iFs.Connect() );

    // Open DMC
    iDmc.Open();
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::Release
// Deletes and frees memory allocated.
// -----------------------------------------------------------------------------
// 
void CMiscAdaptationRef::Release()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_RELEASE,
        "CMiscAdaptationRef::Release" );
    MISC_TRACE( _L( "CMiscAdaptationRef::Release" ) );

    delete this;
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::GetGlobalStartupMode
// Get the start-up mode from the Cooperating System.
// Reference implementation completes with KErrNone as a cooperating system does 
// not exist on Techview/H4 hrp.
// -----------------------------------------------------------------------------
//
void CMiscAdaptationRef::GetGlobalStartupMode( TDes8& aModePckg, 
    TRequestStatus& aStatus )
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_GETGLOBALSTARTUPMODE,
        "CMiscAdaptationRef::GetGlobalStartupMode" );
    MISC_TRACE( _L( "CMiscAdaptationRef::GetGlobalStartupMode" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;

    StartupAdaptation::TGlobalStartupMode mode = StartupAdaptation::ENormal;

    // Get start up mode from DMC.
    RDmc::TTargetStartupMode dmcTargetMode( RDmc::EStartupModeNormal );

    TInt err = iDmc.GetStartUpMode( dmcTargetMode );

    OstTrace1( TRACE_NORMAL, DUP1_CMISCADAPTATIONREF_GETGLOBALSTARTUPMODE, 
        "CMiscAdaptationRef::GetGlobalStartupMode;dmcTargetMode=%d", dmcTargetMode );
    MISC_TRACE( ( _L( "CMiscAdaptationRef::GetGlobalStartupMode;dmcTargetMode=%d" ),
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
    
    OstTraceExt2( TRACE_NORMAL, DUP2_CMISCADAPTATIONREF_GETGLOBALSTARTUPMODE,
        "CMiscAdaptationRef::GetGlobalStartupMode;mode=%d;err=%d", mode, err );
    MISC_TRACE( ( _L( "CMiscAdaptationRef::GetGlobalStartupMode;mode=%d;err=%d" ),
        mode, err ) );
    
    // Startupmode should be returned to SSM at aModePckg param.
    TPckgBuf<StartupAdaptation::TGlobalStartupMode> startUpMode( mode );
    aModePckg = startUpMode;
    User::RequestComplete( status, err );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::PrepareSimLanguages
// Prepare language codes stored in the SIM in preferred language lists. 
// The next call should be GetSimLanguages()
// Reference implementation completes with KErrNotSupported as SIM support is 
// not available on Techview/H4 hrp.
// -----------------------------------------------------------------------------
//
void CMiscAdaptationRef::PrepareSimLanguages( TSsmLanguageListPriority 
    /*aPriority*/, TDes8& /*aSizePckg*/, TRequestStatus& aStatus )
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_PREPARESIMLANGUAGES,
        "CMiscAdaptationRef::PrepareSimLanguages" );
    MISC_TRACE( _L( "CMiscAdaptationRef::PrepareSimLanguages" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    // No support for SIM on H4hrp/Techview.
    User::RequestComplete( status, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::GetSimLanguagesL
// Get language codes stored in the SIM in preferred language lists. 
// PrepareSimLanguages() call should precede this call.
// Reference implementation completes with KErrNotSupported as SIM support is 
// not available on Techview/H4 hrp.
// -----------------------------------------------------------------------------
//
void CMiscAdaptationRef::GetSimLanguagesL( CBufBase* /*aBuf*/, 
    TInt /*aCount*/ )
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_GETSIMLANGUAGES,
        "CMiscAdaptationRef::GetSimLanguages" );
    MISC_TRACE( _L( "CMiscAdaptationRef::GetSimLanguages" ) );

    // No support for SIM on HRP/Techview.
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::GetHiddenReset
// Retrieve information about Hidden reset. 
// Reference implementation completes with KErrNotSupported as SIM support is 
// not available on Techview/H4 hrp.
// -----------------------------------------------------------------------------
//
void CMiscAdaptationRef::GetHiddenReset( TDes8& /*aHiddenResetPckg*/, 
    TRequestStatus& aStatus )
	{
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_GETHIDDENRESET,
        "CMiscAdaptationRef::GetHiddenReset" );
    MISC_TRACE( _L( "CMiscAdaptationRef::GetHiddenReset" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    
    // No support for SIM on HRP/Techview.
    User::RequestComplete( status, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::Cancel
// Cancel the outstanding request. Reference implementation completes the 
// requests immediately so there is nothing to Cancel.
// On a device, Cancel() needs an implementation as the Request might be 
// outstanding and it needs to be cancelled.
// -----------------------------------------------------------------------------
//
void CMiscAdaptationRef::Cancel()
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_CANCEL,
        "CMiscAdaptationRef::Cancel" );
    MISC_TRACE( _L( "CMiscAdaptationRef::Cancel" ) );
    }

// -----------------------------------------------------------------------------
// CMiscAdaptationRef::SecurityStateChange
// Request the next security state.
// Techview/H4hrp does not support SIM operations so a reference implementation 
// would always return 'KErrNotSupported'.
// The function is modified to suit testing needs for PinCheck Security Command.
// A real implementation is intended to differ a lot based on the runtime 
// changes/events on the device.
 
// Each switch case represents a PIN Check security plug-in functionality in 
// different status are as follows..
 
// EPINCHECK01 : SIM present and PIN1 required.
// EPINCHECK02 : SIM present and rejected
// EPINCHECK03 : SIM blocked PUK required
// EPINCHECK04 : SIM locked security code required.
// EPINCHECK05 : SIM locked security code required when sim lockRestrictionOn
// EPINCHECK06 : SIM blocked and UPUK.
// EPINCHECK07 : SIM blocked security code required when sim lockRestrictionOn
// EPINCHECK08 : SIM present and PIN1 required and SimLock status is wrong
// iTestCaseNum will be reset to 1 at EPINCHECK08 to test all above scenarios 
// for wrong entered pin
// -----------------------------------------------------------------------------
//
void CMiscAdaptationRef::SecurityStateChange( TInt aState, TDes8& aResponsePckg, 
    TRequestStatus& aStatus )
    {
    OstTrace0( TRACE_NORMAL, CMISCADAPTATIONREF_SECURITYSTATECHANGE,
        "CMiscAdaptationRef::SecurityStateChange" );
    MISC_TRACE( _L( "CMiscAdaptationRef::SecurityStateChange" ) );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    StartupAdaptation::TSecurityStateInfo info = StartupAdaptation::EYes;

    //Read Test case number from the file 
    TInt err = iFile.Open( iFs, KTestCmdSecurityCheckTestFile, EFileRead );

    if ( err == KErrNone )
        {
        TRAP( err,iTestCaseNum = iFile.ReadInt32L() );
        iFile.Close();
        if( err )
            {
            MISC_TRACE(_L( "Error while reading tst case number from the file pinchecksecuritycaseno.txt" ) );
            User::RequestComplete(status, err);
            return;
            }
        } 

    switch( iTestCaseNum )
        {
        case EPINCHECK01:
            switch( aState )
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::ESIMBlocked:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::EPINRequired:
                info = StartupAdaptation::EPIN1Required;
                break;
            case StartupAdaptation::ESIMLock:
                info = StartupAdaptation::ESimLockOk;
                break;
            case StartupAdaptation::ESecurityCheckOK:
                info = StartupAdaptation::EYes; 
                break;  
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes;
                break;  
            default:
                break;
            }
            break;

        case EPINCHECK02:
            switch( aState )
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes;
                break;  
            default:
                break;
            }
            break;

        case EPINCHECK03:
            switch( aState )
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::ESIMBlocked:
                info = StartupAdaptation::EPUK1Required;
                break;  
            case StartupAdaptation::ESIMLock:
                info = StartupAdaptation::ESimLockOk;
                break;
            case StartupAdaptation::ESecurityCheckOK:
                info = StartupAdaptation::EYes; 
                break;  
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes;
                break;  
            default:
                break;
            }
            break;

        case EPINCHECK04:
            switch( aState) 
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::ESIMBlocked:
                info = StartupAdaptation::EPUK1Required;
                break;  
            case StartupAdaptation::ESIMLock:
                info = StartupAdaptation::ESimLockOk;
                break;
            case StartupAdaptation::ESecurityCheckOK:
                info = StartupAdaptation::EYes; 
                break;  
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes;
                break;  
            default:
                break;
            } 
            break;

        case EPINCHECK05:
            switch( aState )
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::ESIMBlocked:
                info = StartupAdaptation::EUPUKRequired;
                break;  
            case StartupAdaptation::ESIMLock:
                info = StartupAdaptation::ESimLockRestrictionOn;
                break;
            case StartupAdaptation::ESecurityCheckOK:
                info = StartupAdaptation::EYes; 
                break;  
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes;
                break;  
            default:
                break;  
            } 
            break;
            
        case EPINCHECK06:
            switch( aState )
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::ESIMBlocked:
                info = StartupAdaptation::EUPUKRequired;
                break;  
            case StartupAdaptation::ESIMLock:
                info = StartupAdaptation::ESimLockRestrictionPending;
                break;
            case StartupAdaptation::ESecurityCheckOK:
                info = StartupAdaptation::EYes; 
                break;  
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes; 
                break;  
            default:
                break;  
            }
            break;
            
        case EPINCHECK07:
            switch( aState )
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::ESIMBlocked:
                info = StartupAdaptation::EUPUKRequired;
                break;  
            case StartupAdaptation::ESIMLock:
                info = StartupAdaptation::ESimLockRestricted;
                break;
            case StartupAdaptation::ESecurityCheckOK:
                info = StartupAdaptation::EYes; 
                break;  
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes; 
                break;  
            default:
                break;  
            }
            break;
      
        case EPINCHECK08:
            switch( aState )
            {
            case StartupAdaptation::ESIMPresent:
                info = StartupAdaptation::EYes;
                break;
            case StartupAdaptation::ESIMRejected:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::ESIMBlocked:
                info = StartupAdaptation::ENo;
                break;
            case StartupAdaptation::EPINRequired:
                info = StartupAdaptation::EPIN1Required;
                break;
            case StartupAdaptation::ESIMLock:
                info = StartupAdaptation::ESimLockOk;
                break;
            case StartupAdaptation::ESecurityCheckOK:
                info = StartupAdaptation::EYes; 
                break;  
            case StartupAdaptation::ESecurityCheckFailed:
                info = StartupAdaptation::EYes; 
                break;  
            default:
                break;
            } 
            break;
            
        default:
            break;
        }
        
    TPckgBuf<StartupAdaptation::TSecurityStateInfo> securityStateInfoResult( info );
    aResponsePckg = securityStateInfoResult;  
    User::RequestComplete( status, KErrNone );
    }

// End of File

