/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Set and cancel async calls and write logs
*
*/



// INCLUDE FILES
#include <stifitemparser.h>
#include "notify.h"
#include "notifyhandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimAtkNotify::CSimAtkNotify
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSimAtkNotify::CSimAtkNotify
        (
        CTestModuleIf& aTestModuleIf
        ) : CScriptBase( aTestModuleIf )
    {
    iNotifyHandler = NULL;
    iCore = NULL;
    }
    
// -----------------------------------------------------------------------------
// CSimAtkNotify::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimAtkNotify::ConstructL()
    {
    iLog = CStifLogger::NewL( KCoreLogPath, KCoreLogFile,
        CStifLogger::ETxt, CStifLogger::EFile, ETrue );
                                               
    iNotifyHandler = CSimAtkNotifyHandler::NewL( this, iLog );
    }

// -----------------------------------------------------------------------------
// CSimAtkNotify::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimAtkNotify* CSimAtkNotify::NewL
        (
        CTestModuleIf& aTestModuleIf
        )
    {
    CSimAtkNotify* self = new (ELeave) CSimAtkNotify( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;    
    }
 
// -----------------------------------------------------------------------------
// CSimAtkNotify::~CSimAtkNotify
// Destructor
// -----------------------------------------------------------------------------
// 
CSimAtkNotify::~CSimAtkNotify()
    { 
    delete iNotifyHandler;
    iNotifyHandler = NULL;   

    delete iLog;
    iLog = NULL;
    
    iCore = NULL;         
    }
    
// -----------------------------------------------------------------------------
// CSimAtkNotify::CSimAtkCore
// Return pointer to core class
// -----------------------------------------------------------------------------
//   
CSimAtkCore* CSimAtkNotify::SimAtkCore()
	{
	return iCore;
	}
	
	
// -----------------------------------------------------------------------------
// CSimAtkNotify::UseObjectAddress
// Notifies and cancels notifys according to script
// -----------------------------------------------------------------------------
//   
TInt CSimAtkNotify::UseObjectAddress
        (
        CStifItemParser& aItem
        )
    {
    TPtrC objectName;
    TInt ret ( KErrNone );
    
    // Find tag
    ret = aItem.GetString( _L( "UseObjectAddress" ), objectName );
  
    CScriptBase* script = GetTestObject( objectName );
    
    // This must be done every time GetTestObject is just called
    if ( NULL == script )
        {
        iLog->Log( _L( "CSimAtkNotify::UseObjectAddresL \
            - Script == NULL" ) );
        ret = KErrGeneral;
        }
    else
        {
        iLog->Log( _L ( "CSimAtkNotify::UseObjectAddressL \
            - Start to cast objects " ) );    
        iCore = reinterpret_cast <CSimAtkCore*>( script );
    	
    	// Identify the type of notify
    	TPtrC notifyPtr;
    	ret = aItem.GetNextString( notifyPtr );
    	
    	if ( KErrNone == ret )
    	    {
            // Potential notify string found. Identify the notify or
            // cancellation and map it to an ipc.
            
        	TSatIpc notifyIpc;
        	TBool isNotify = EFalse;

        	notifyIpc = iCore->MapStringToIpc( notifyPtr, isNotify );

        	if ( isNotify && ( notifyIpc != KErrNotFound ) )
            	{
            	iNotifyHandler->SetNotify( notifyIpc );
                ret = iCore->Notify( iNotifyHandler->Status(), notifyIpc );
                
                if ( KErrNone == ret )
                    {
                	iLog->Log( _L ( "CSimAtkNotify::UseObjectAddressL \
                	    Notify %S " ), &notifyPtr );

                  	iNotifyHandler->Activate();    			
                    }
                else
                    {
                	iLog->Log( _L ( "CSimAtkNotify::UseObjectAddressL \
                	    Notifying of %S failed! " ), &notifyPtr );

                  	iNotifyHandler->Cancel();    			
                    }                    
            	}
        	else if ( EFalse == isNotify )// This is cancellation of notify
            	{
            	iNotifyHandler->Cancel();    			
            	iLog->Log( _L( "CSimAtkNotify::UseObjectAddressL \
            	    - object Canceled " ) );
            	}
            else
                {
            	iLog->Log( _L( "CSimAtkNotify::UseObjectAddressL \
            	    - Unsupported notify request received! Aborting..." ) );
            	iNotifyHandler->Cancel();
            	
                ret = KErrNotFound;
                }

        	iLog->Log( _L( "CSimAtkNotify::UseObjectAddressL \
        	    Method done " ) );
            }
	    }

    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSimAtkNotify::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//   
TInt CSimAtkNotify::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    TInt count = 0;
    const TStifFunctionInfo* functions = RunMethodCases( count );

    return CScriptBase::RunInternalL( functions, count, aItem );
    }
   
// -----------------------------------------------------------------------------
// CSimAtkNotify::RunInternalL
// Run specified method from derived class or core class.
// -----------------------------------------------------------------------------
// 
TInt CSimAtkNotify::RunInternalL
        ( 
        TStifFunctionInfo const aFunctions[], // Function table
        TInt aCount, // Size of function table
        CStifItemParser& aItem // reference to parser
        )
    {
    iLog->Log( _L( "CSimAtkNotify::RunInternalL " ) );
    TInt count = 0;
    const TStifFunctionInfo* functions = RunMethodCases( count );
    
    TInt total = count + aCount;
    
    TUint8* tmp = new( total * sizeof( TStifFunctionInfo ) ) TUint8;
    User::LeaveIfNull( tmp );
                
    TStifFunctionInfo* allFuncs = ( TStifFunctionInfo* ) tmp;
    
    Mem::Copy( &allFuncs[0], aFunctions,
        aCount * sizeof( TStifFunctionInfo ) );
    Mem::Copy( &allFuncs[aCount], functions,
        count * sizeof( TStifFunctionInfo ) );
        
    TInt ret = CScriptBase::RunInternalL( allFuncs, total, aItem );
    
    delete tmp;

    return ret;
    }
     
// -----------------------------------------------------------------------------
// CSimAtkNotify::RunMethodCases
// Return function table.
// -----------------------------------------------------------------------------
//   
const TStifFunctionInfo* CSimAtkNotify::RunMethodCases( TInt& aCount ) 
    { 
	static TStifFunctionInfo const KFunctions[] =
   	 	{
   	 	// Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
    	ENTRY( "UseObjectAddress", CSimAtkNotify::UseObjectAddress ),
    	};

    aCount = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return KFunctions;
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
    return ( CScriptBase* ) CSimAtkNotify::NewL( aTestModuleIf );
    }    
    
// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll
        (
        TDllReason /*aReason*/ // Reason code
        ) 
    {
    return(KErrNone);
    }
#endif // EKA2
    
//  End of File
