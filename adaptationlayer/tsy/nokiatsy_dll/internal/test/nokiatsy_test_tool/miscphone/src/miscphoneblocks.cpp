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
* Description:   Specific class for MiscPhone tests
*
*/


 

// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "miscphone.h"


// CONSTANTS
_LIT8( KLine, "---------------------------------" );



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCSDI::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMiscPhone::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CCSDI::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CMiscPhone::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    _LIT8 (KLog, "MiscPhone: RunMethodL" );
    iMiscPhoneLog->Log((TDesC8)KLog);
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "GetCustomerServiceProfile",      CMiscPhone::GetCustomerServiceProfile ),
        ENTRY( "GetPhoneId",                     CMiscPhone::GetPhoneId ),
        ENTRY( "RunIscTestCase",                 CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase",                CCore::SyncIscTestCase),
        ENTRY( "SetDriveMode",                   CMiscPhone::SetDriveMode),
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC", CCore::SetExpectedCompleteIPC),
      
        };
        
     
    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

// -----------------------------------------------------------------------------
// CMiscPhone::GetCustomerServiceProfile
// Getting customer service profile
// -----------------------------------------------------------------------------
//
TInt CMiscPhone::GetCustomerServiceProfile( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KPbInit, "CMiscPhone::GetCustomerServiceProfile");
    iMiscPhoneLog->Log((TDesC8)KPbInit);
    
    TInt ret( KErrNotFound );
    
        
    ret = HandleRequestL( EMobilePhoneGetCustomerServiceProfile );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetCustomerServiceProfile request send ok" );      
        iMiscPhoneLog->Log((TDesC8)KSendOk );     
        }
    else
        {
        _LIT8( KSendFailed, "GetCustomerServiceProfile request send failed: %d" );      
        iMiscPhoneLog->Log((TDesC8)KSendFailed, ret );      
        }
    
    
    return ret;

    }


// -----------------------------------------------------------------------------
// CMiscPhone::CompleteGetCustomerServiceProfile
// Complete GetCustomerServiceProfile method function.
// -----------------------------------------------------------------------------
//  
void CMiscPhone::CompleteGetCustomerServiceProfile
    (
    TInt aResult, CMmDataPackage* aDataPackage
    )                            
    {
    _LIT8( KResult, "CompleteGetCustomerServiceProfile result: %d" );     
    iMiscPhoneLog->Log((TDesC8)KResult, aResult );
    
    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetCustomerServiceProfile Failed: %d" );
        iMiscPhoneLog->Log((TDesC8)KFail, aResult );
        }
    else
        {
        RMobilePhone::TMobilePhoneCspFileV1 cspFileEtel;
    
        aDataPackage->UnPackData ( cspFileEtel );
    
        _LIT8( KCCS, "Call completion services: %d" );      
        iMiscPhoneLog->Log((TDesC8)KCCS, cspFileEtel.iCallCompletionServices );
    
        _LIT8( KCOS, "Call offering services: %d" );
        iMiscPhoneLog->Log((TDesC8)KCOS, cspFileEtel.iCallOfferingServices );
    
        _LIT8( KCRS, "Call restriction services: %d" );
        iMiscPhoneLog->Log((TDesC8)KCRS, cspFileEtel.iCallRestrictionServices );
    
        _LIT8( KCF, "Cphs features: %d" );
        iMiscPhoneLog->Log((TDesC8)KCF, cspFileEtel.iCphsFeatures );
    
        _LIT8( KCT, "Cphs teleservices: %d" );
        iMiscPhoneLog->Log((TDesC8)KCT, cspFileEtel.iCphsTeleservices );
    
        _LIT8( KNIS, "Number ident services: %d" );
        iMiscPhoneLog->Log((TDesC8)KNIS, cspFileEtel.iNumberIdentServices );
    
        _LIT8( KOSS, "Other supp services: %d" );
        iMiscPhoneLog->Log((TDesC8)KOSS, cspFileEtel.iOtherSuppServices );

        _LIT8( KP2PS, "Phase 2 plus services: %d" );
        iMiscPhoneLog->Log((TDesC8)KP2PS, cspFileEtel.iPhase2PlusServices );

        _LIT8( KT, "Teleservices: %d" );
        iMiscPhoneLog->Log((TDesC8)KT, cspFileEtel.iTeleservices );
    
        _LIT8( KVAS, "Value added services: %d" );
        iMiscPhoneLog->Log((TDesC8)KVAS, cspFileEtel.iValueAddedServices );

        }
  
    Signal( aResult );

    }




// -----------------------------------------------------------------------------
// CMiscPhone::GetPhoneId
// Getting phone ID
// -----------------------------------------------------------------------------
//
TInt CMiscPhone::GetPhoneId( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KPbInit, "CMiscPhone::GetPhoneId");
    iMiscPhoneLog->Log((TDesC8)KPbInit);
    
    TInt ret( KErrNotFound );
 
    
    ret = HandleRequestL( EMobilePhoneGetPhoneId );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetPhoneId request send ok" );     
        iMiscPhoneLog->Log((TDesC8)KSendOk );     
        }       
    else
        {
        _LIT8( KSendFailed, "GetPhoneId request send failed: %d" );     
        iMiscPhoneLog->Log((TDesC8)KSendFailed, ret );      
        }
    
    
   
    return ret;

    } 


// -----------------------------------------------------------------------------
// CMiscPhone::CompleteGetPhoneId
// Complete GetPhoneId method function.
// -----------------------------------------------------------------------------
//  
void CMiscPhone::CompleteGetPhoneId
    (
    TInt aResult, CMmDataPackage* aDataPackage 
    )                            
    {
    _LIT8( KResult, "CompleteGetPhoneId result: %d" );      
    iMiscPhoneLog->Log((TDesC8)KResult, aResult );
    
    TBuf8<50> id;
    
    aDataPackage->UnPackData(id);
        
   
    TBuf8<255> Buffer;
    _LIT8(KRowFormatter, "Phone id is: %S");

    Buffer.Format(KRowFormatter,&id);
    
    iMiscPhoneLog->Log(Buffer);
    
    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteGetPhoneId Failed: %d" );
        iMiscPhoneLog->Log((TDesC8)KFail, aResult );
        }

  
    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CMiscPhone::SetDriveMode
// Setting drive mode
// -----------------------------------------------------------------------------
//

TInt CMiscPhone::SetDriveMode( CStifItemParser& aItem)
    {
    _LIT8(KPbInit, "CMiscPhone::SetDriveMode");
    iMiscPhoneLog->Log((TDesC8)KPbInit);
    
    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC modestring;
      
    aItem.GetNextString ( modestring );
    
    RMmCustomAPI::TSetDriveMode mode;
     
    if ( _L("DeactivateDriveMode") == modestring )
        {
        mode = RMmCustomAPI::EDeactivateDriveMode;
        }
    else if ( _L("ActivateDriveMode") == modestring )
        {
        mode = RMmCustomAPI::EActivateDriveMode;
        }
    else
        {
        _LIT8(KNotSupp, "CMiscPhone::Selected drive mode setting not supported");
        iMiscPhoneLog->Log((TDesC8)KNotSupp);
        supported = EFalse; 
        }
  
    //Create package
    CMmDataPackage package;
  
    // Set package data
    package.PackData( &mode );
  
  
    if( supported )
        {
    
        ret = HandleRequestL( ECustomSetDriveModeIPC, &package );

        if ( KErrNone == ret )
            {
                _LIT8( KSendOk, "SetDriveMode request send ok" );      
                iMiscPhoneLog->Log((TDesC8)KSendOk );     
            }
        else
            {
            _LIT8( KSendFailed, "SetDriveMode request send failed: %d" );     
            iMiscPhoneLog->Log((TDesC8)KSendFailed, ret );      
            }
        }
    else
        {
        ret = KErrNotSupported;
        }
   
    return ret;

    } 


// -----------------------------------------------------------------------------
// CMiscPhone::CompleteSetDriveMode
// Complete SetDriveMode method function.
// -----------------------------------------------------------------------------
//  

void CMiscPhone::CompleteSetDriveMode
    (
    TInt aResult
    )                            
    {
    _LIT8( KResult, "CompleteSetDriveMode result: %d" );      
    iMiscPhoneLog->Log((TDesC8)KResult, aResult );
    
    
    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSetDriveMode Failed: %d" );
        iMiscPhoneLog->Log((TDesC8)KFail, aResult );
        }

  
    Signal( aResult );

    }



// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


//  End of File

