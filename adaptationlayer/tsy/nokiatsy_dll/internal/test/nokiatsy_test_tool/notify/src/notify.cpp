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
* Description:   Sets async calls and write logs
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include <stifparser.h>
#include "notify.h"
#include "core.h"


// CONSTANTS
_LIT ( KNotifyNetworkRegistrationStatusChange, "NotifyNetworkRegistrationStatusChange" );
_LIT ( KNotifyNetworkSelectionSettingChange, "NotifyNetworkSelectionSettingChange" );
_LIT ( KNotifyCurrentNetworkChange, "NotifyCurrentNetworkChange" );
_LIT ( KNotifyModeChange, "NotifyModeChange" );
_LIT ( KNotifySignalStrengthChange, "NotifySignalStrengthChange" );
_LIT ( KNotifyNITZInfoChange, "NotifyNITZInfoChange" );
_LIT ( KNotifyNetworkSecurityLevelChange, "NotifyNetworkSecurityLevelChange" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotify::CNotify
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNotify::CNotify( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    iCore = NULL;
    }

// -----------------------------------------------------------------------------
// CNotify::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNotify::ConstructL()
    {
    iNotifyLog = CStifLogger::NewL( KNotifyLogPath, 
                          KNotifyLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
                          
    _LIT8(KNotify, "CNotify: ConstructL");                      
    iNotifyLog->Log((TDesC8)KNotify); 

    }

// -----------------------------------------------------------------------------
// CNotify::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNotify* CNotify::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CNotify* self = new (ELeave) CNotify( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    
    }
    
// Destructor
CNotify::~CNotify()
    { 
    
    // Delete resources allocated from test methods
    Delete();
    
    // Delete logger
    delete iNotifyLog;  
    
    }
    
// -----------------------------------------------------------------------------
// CNotify::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//   
TInt CNotify::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    TInt count = 0;
    const TStifFunctionInfo* functions = RunMethodCases( count );

    return CScriptBase::RunInternalL( functions, count, aItem );
    
    }

// -----------------------------------------------------------------------------
// CNotify::RunMethodCases
// Return function table.
// -----------------------------------------------------------------------------
//   
const TStifFunctionInfo* CNotify::RunMethodCases( TInt& aCount ) 
    { 
	static TStifFunctionInfo const KFunctions[] =
   	 	{
   	 	// Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        
    	ENTRY( "UseObjectAddress", CNotify::UseObjectAddress ),
    	
    	};

    aCount = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return KFunctions;
    
    } 
// -----------------------------------------------------------------------------
// CNotify::UseObjectAddress
// 
// -----------------------------------------------------------------------------
//      
TInt CNotify::UseObjectAddress( CStifItemParser& aItem )
	{
	_LIT8(KNotify, "Notify::UseObjectAddress");  
	iNotifyLog->Log((TDesC8)KNotify);
	
    TInt ret( KErrNotFound );
	TPtrC objectName;
    aItem.GetNextString ( objectName );
    
    
    CScriptBase* script = GetTestObject( objectName );
    
    if ( NULL == script )  // This must be done every time
        {
        _LIT8(KNULL, "CNotify::Notify ** script == NULL");  
        iNotifyLog->Log((TDesC8)KNULL );
        ret = KErrGeneral;
        }
    else
    	{
        ret = KErrNone;
        
       	TPtrC notifyPtr;
    	aItem.GetNextString( notifyPtr );
	    iNotify=MapScriptToEnum( notifyPtr );
        
		iCore = reinterpret_cast <CCore*>( script );    
		if ( iRequest ) // This checks id boolean is true then call is async call
			{
		    iCore->Notify( iNotify );
			}

		else    // if boolean is false call is cancellation
			{
			_LIT8(KCancelled, "CNotify::UseObjectAddressL - object Canceled");  
			iNotifyLog->Log((TDesC8)KCancelled);
			}
      }
    
	return ret;
	}
	
// -----------------------------------------------------------------------------
// CNotify::MapScriptToEnum
// Return Core TNotifyList enumerale
// -----------------------------------------------------------------------------
//   
CCore::TNotifyList CNotify::MapScriptToEnum( const TDesC& aItem )
    {
    _LIT8(KNotify, "Notify::MapScriptToEnum"); 
    iNotifyLog->Log((TDesC8)KNotify);
	 
	CCore::TNotifyList ret;
	if (aItem == (KNotifyNetworkRegistrationStatusChange))
	    {
		iRequest = ETrue;
		ret = CCore::ENotifyNetworkRegistrationStatusChange;
		}
	else if (aItem == (KNotifyNetworkSelectionSettingChange))
	    {
		iRequest = ETrue;
		ret = CCore::ENotifyNetworkSelectionSettingChange;
		}
	else if (aItem == (KNotifyCurrentNetworkChange))
	    {
		iRequest = ETrue;
		ret = CCore::ENotifyCurrentNetworkChange;
		}
	else if (aItem == (KNotifyModeChange))
	    {
		iRequest = ETrue;
		ret = CCore::ENotifyModeChange;
		}
	else if (aItem == (KNotifySignalStrengthChange))
	    {
		iRequest = ETrue;
		ret = CCore::ENotifySignalStrengthChange;
		}
	else if (aItem == (KNotifyNITZInfoChange))
	    {
		iRequest = ETrue;
		ret = CCore::ENotifyNITZInfoChange;
		}
	else if (aItem == (KNotifyNetworkSecurityLevelChange))
	    {
		iRequest = ETrue;
		ret = CCore::ENotifyNetworkSecurityLevelChange;
		}
	// default returns ENotifyUndefined
	else
 		{
 		ret = CCore::ENotifyUndefined;
 		}
	 		
    return ret;
	 
	}
	
// -----------------------------------------------------------------------------
// CNotify::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CNotify::Delete() 
    {
    
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
    
    return ( CScriptBase* ) CNotify::NewL( aTestModuleIf );
        
    }
    
//  End of File
