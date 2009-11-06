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
* Description:   
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "core.h"
#include "simatk.h"


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// CSimAtkCore::MapStringToIpc
// Maps string presentation of notify to notify ipc and
// identifies wether the string is notify or cancellation
// -----------------------------------------------------------------------------
//
EXPORT_C TSatIpc CSimAtkCore::MapStringToIpc
        (
        const TDesC& aNotifyString,
        TBool& aIsNotify
        )
    {
	TSatIpc notifyType( KErrNotFound );
    
	if ( KErrNotFound != aNotifyString.Find( _L( "Cancel" ) ) )
	    {
	    // This is cancellation.
	    aIsNotify = EFalse;
	    notifyType = KErrNone;
	    }
	else
	    {
	    aIsNotify = ETrue;
	    
	    // Most of the time successive searchs on same notify are made.
	    // Searching is started from last array known position and it continues
	    // until the match is made or the whole array has been searched.
        TUint8 i( iLastIndex );
	    do
	        {
	        TPtrC current( KNotifyList[i].iNotifyString );
	        
            if ( KErrNotFound != current.Find( aNotifyString ) )
                {
                // We have a match.
                notifyType = KNotifyList[i].iIpc;
                iLastIndex = i;
                }
            else if ( ++i == KNotifyListSize )
                {
                // No match. Increase index and reset to zero if array limit
                // is reached.
                i = 0;
                }
            // Array has been throughly searched without match when index
            // and stored last index have same values again.                
	        } while ( KErrNotFound == notifyType && i != iLastIndex );
        }

    return notifyType;
	}
	
// -----------------------------------------------------------------------------
// CSimAtkCore::MapIpcToString
// Maps ipc presentation of notify to string and
// identifies wether the string is notify or cancellation
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CSimAtkCore::MapIpcToString
        (
        const TSatIpc aNotifyIpc
        )
    {
    TPtrC notifyString( KNullDesC );
    
    // Search logic is described in detail in function above
    TUint8 i( iLastIndex );
    do
        {
        if ( aNotifyIpc == KNotifyList[i].iIpc )
            {
            notifyString.Set( KNotifyList[i].iNotifyString );
            iLastIndex = i;
            }
        else if ( ++i == KNotifyListSize )
            {
            i = 0;
            }                

        } while ( notifyString == KNullDesC && i != iLastIndex );    
	        
    return notifyString;
	}
	
// -----------------------------------------------------------------------------
// CSimAtkCore::Notify
// Notify method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
EXPORT_C TInt CSimAtkCore::Notify
        (
        TRequestStatus& aStatus, // Request status
        const TSatIpc& aNotifyIpc // Notify IPC
        )
	{
	TInt ret ( KErrGeneral ); 
	iCoreLog->Log( _L( "CSimAtkCore::Notify" ) );
	
	switch( ObjectType() )
    	{
    	case ESimAtk:
            {
            // This calls notification from CC
        	iCoreLog->Log( _L( "CSimAtkCore::Notify \
        	    Will be handled in SimAtk subclass" ) );
        	
        	TRAPD
        	    ( 
        	    error,
        	    ret = iSimAtk->NotifySimAtkL( aStatus, aNotifyIpc )
        	    );
        	    
        	if ( KErrNone != error )
        	    {
        	    iCoreLog->Log( _L( "CSimAtkCore::Notify \
        	        Notifying SimAtk failed" ) );
        	    ret = error;
        	    }
        	    
            break;
            }
        default:
            {          
            // Should never enter this block
            iCoreLog->Log( _L( "CSimAtkCore::Notify Default block" ) );
            ret = KErrDied;                          
            break;
            }
            
    	}

    return ret;	
    }

// -----------------------------------------------------------------------------
// CSimAtkCore::NotifyCancel
// NotifyCancel method function.
// returns derived object type
// -----------------------------------------------------------------------------
// 	
EXPORT_C void CSimAtkCore::NotifyCancel
        (
        const TSatIpc& aNotifyIpc // A notify to be cancelled
        )
    {
    iCoreLog->Log( _L( "CSimAtkCore::NotifyCancel" ) );
	
	switch( ObjectType() )
    	{
    	case CSimAtkCore::ECore:
    	    {
    	    break;
    	    }
	   
        case CSimAtkCore::ESimAtk:
            {
            // This calls notification from SIM ATK
        	iCoreLog->Log( _L( "CSimAtkCore::NotifyCancel \
        	    Will be handled in SimAtk subclass" ) );
        	    
            iSimAtk->NotifySimAtkCancel( aNotifyIpc );
            break;    
    		}	   
        default:
            {
            // Should never enter this block
            iCoreLog->Log( _L( "CSimAtkCore::NotifyCancel Default block" ) );
            break;
            }            
    	}	
    }
		
// -----------------------------------------------------------------------------
// CSimAtkCore::SetString
// SetString method function.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSimAtkCore::SetString
        (
        const TDesC& aString
        )
	{
	iString.Set( aString );	
	}
	
// -----------------------------------------------------------------------------
// CSimAtkCore::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSimAtkCore::RunMethodL
        ( 
        CStifItemParser& aItem
        )
    {
    TInt count = 0;
    const TStifFunctionInfo* functions = RunMethodCases( count );

    return CScriptBase::RunInternalL( functions, count, aItem );    
    }
    
// -----------------------------------------------------------------------------
// CSimAtkCore::Close
// Close method function.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSimAtkCore::Close
        (
        CStifItemParser& aItem
        )
	{
	TInt ret (KErrNotFound); 
	TPtrC string;
    aItem.GetNextString ( string );
		
	// Close Phone	
	if ( _L( "DefaultPhone" ) == iString )
		{
		iRphone.Close();
		iCoreLog->Log( _L( "CSimAtkCore::Close Closed Phone." ) );
		
		ret = KErrNone;		
		}
		
	// Close RTelServer	
	else if ( _L( "RTelServer" ) == iString )
		{
    	iRtel.Close();
	    iCoreLog->Log( _L( "CSimAtkCore::Close Closed RTelServer." ) );
	    
	    ret = KErrNone;	    
		}
		
	// Close RCommServer	
	else if ( _L( "Commserver" ) == iString )
		{
    	iRcomm.Close();
	    iCoreLog->Log( _L( "CSimAtkCore::Close Closed Commserver." ) );
	    
	    ret = KErrNone;	    		
	    }
	    
	else
		{
		iCoreLog->Log( _L( "CSimAtkCore::Close Can not close anything" ) );
		}
    	
    return ret;        
    }	

// -----------------------------------------------------------------------------
// CSimAtkCore::Open
// Open method function.
// -----------------------------------------------------------------------------
// 
EXPORT_C TInt CSimAtkCore::Open
        (
            CStifItemParser& aItem
        )
	{    
    TInt ret( KErrNotFound );
    TPtrC string;
    aItem.GetNextString ( string );
    
    // Open default phone
    if ( _L( "DefaultPhone" ) == iString )
    	{		   
	   	ret = iRphone.Open( iRtel, iString /*iPhoneInfo.iName*/ );
	   	
	   	if ( KErrNone == ret )
	   		{
	   		iCoreLog->Log( _L( "CSimAtkCore::Open \
	   		    Default phone open successful." ) );
	   		}
	   		
	   	else if ( KErrAlreadyExists == ret  )
			{
	    	iCoreLog->Log( _L( "CSimAtkCore::Open \
	    	    Default phone is already open: %d." ), ret );
	    	}
	   		
	   	else
	   		{
	   		iCoreLog->Log( _L( "CSimAtkCore::Open \
	   		    Can not open default phone: %d." ), ret );
	   		} 	    
	   		 	    	
		}
		
	// Open false phone
    else if ( _L( "FalsePhone" ) == iString )
    	{		   
	   	ret = iRphone.Open( iRtel, iString );

	   	if ( KErrNotFound == ret  )
			{
	    	iCoreLog->Log( _L( "CSimAtkCore::Open \
	    	    False phone:(expected = Not found): %d." ), ret );
	    	}
	   		
	   	else
	   		{
	   		iCoreLog->Log( _L( "CSimAtkCore::Open \
	   		    False phone fails: %d." ), ret );
	   		} 	    
	   		 	    	
		}		
				
	// Enumerate phones    
	else if ( _L( "EnumeratePhones" ) == iString )
	   	{		   
	   	TInt index;
	   	
   	   	iCoreLog->Log( _L( "CSimAtkCore::Open \
   	   	    Starting to open enumerate phones." ) );
		
		ret = iRtel.EnumeratePhones( index );
		
		if ( KErrNone == ret )
			{
			iCoreLog->Log( _L( "CSimAtkCore::Open \
			    EnumeratePhones returns: %d" ), index );
			}
		
	   	else
	   		{
	   		iCoreLog->Log( _L( "CSimAtkCore::Open \
	   		    Can not open enumeratePhones: %d." ), ret );
	   		}		
        }
     
    else
    	{
    	iCoreLog->Log( _L( "CSimAtkCore::Open \
    	    Can not open any call: %d." ), ret );
    	}
    	
    return ret;        
    }

// -----------------------------------------------------------------------------
// CSimAtkCore::SetObjectType
// SetObjectType method function.
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSimAtkCore::SetObjectType
        (
        TObjectType aType,
        TAny* aModule
        )
    {  
    // Stores object type
    iObjectType = aType;
    
    switch( iObjectType ) 
        {
        case ESimAtk:
            {
            iSimAtk = reinterpret_cast <CSimAtk*>( aModule );
            break;
            } 
        default: 
            {
            break;
            }
        }
    }
        
// -----------------------------------------------------------------------------
// CSimAtkCore::RunInternalL
// Run specified method from derived class or core class.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSimAtkCore::RunInternalL
        ( 
        TStifFunctionInfo const aFunctions[], // Function table
        TInt aCount, // Number of function in table
        CStifItemParser& aItem // Reference to parser
        )
    {    
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
// CSimAtkCore::Connect
// Connect method function.
// -----------------------------------------------------------------------------
//
TInt CSimAtkCore::Connect
        (
        CStifItemParser& aItem
        )
    {   
    TInt ret( KErrCouldNotConnect );
    TPtrC string;
    aItem.GetNextString ( string );

 	// Connect to RTelServer.
    if ( _L( "RTelServer" ) == string )
    	{
    	ret = iRtel.Connect();
    	if ( KErrNone == ret )
    		{   	    
    		iCoreLog->Log( _L( "CSimAtkCore::Connect \
    		    Connected to RTelServer." ) );
    		}
    		
    	else
    		{
    		iCoreLog->Log( _L( "CSimAtkCore::Connect \
    		    Connecting to RTelServer fails: %d." ), ret ); 
    		} 
    	}
   	
	// Connect to RCommServer.    	
    else if ( _L( "RCommServer" ) == string )
    	{
    	ret = iRcomm.Connect();
    	if ( KErrNone == ret )
    		{
    		iCoreLog->Log( _L( "CSimAtkCore::Connect \
    		    Connected to RCommServer." ) );
    		}
    		
    	else 
    		{
    		iCoreLog->Log( _L( "CSimAtkCore::Connect \
    		    RCommServer connecting fails: %d. " ), ret );
    		}
      	}
    
	else
      	{
      	iCoreLog->Log( _L( "CSimAtkCore::Connect \
      	    Could not connect: %d. " ), ret );
      	}
    	
    return ret;    
    }   

// -----------------------------------------------------------------------------
// CSimAtkCore::Load
// Load method function.
// -----------------------------------------------------------------------------
//
TInt CSimAtkCore::Load
        (
        CStifItemParser& aItem
        )
	{	   
    // Print to log file

	TInt ret( KErrNotFound );
    TPtrC string;
    aItem.GetNextString ( string );
    
	// Load PhoneTSY.tsy
    if ( _L( "PhoneTsy.tsy" ) == string )
    	{
	    iCoreLog->Log( _L( "CSimAtkCore::Load, Loading PhoneTsy" ) );
	     
	    ret = iRtel.LoadPhoneModule( _L( "PhoneTsy.tsy" ) );
	    
	    if ( KErrNone == ret )
		    {
		    iCoreLog->Log( _L( "CSimAtkCore::Load \
		        PhoneTsy loaded successfully: %d." ), ret );	
		    }
		    
		else if ( KErrAlreadyExists == ret  )
			{
	    	iCoreLog->Log( _L( "CSimAtkCore::Load \
	    	    PhoneTsy is already loaded: %d." ), ret );
	    	}
	    	
	    else 
	    	{
	    	iCoreLog->Log( _L( "CSimAtkCore::Load \
	    	    PhoneTsy loading fails: %d." ), ret );
	    	}
    	}
    	
    // Load Ecuart.csy    
    else if ( _L( "Ecuart.csy" ) == string )
    	{    	
    	iCoreLog->Log( _L( "CSimAtkCore::Load, Loading Ecuart" ) );
    	
		ret = iRcomm.LoadCommModule( _L( "Ecuart.csy" ) );
	    
	    if ( KErrNone == ret )
	    	{	
	    	iCoreLog->Log( _L( "CSimAtkCore::Load \
	    	    Ecuart loaded successfully: %d." ), ret );
	    	}
	    	
		else if ( KErrAlreadyExists == ret )
			{
	    	iCoreLog->Log( _L( "CSimAtkCore::Load \
	    	    Ecuart already loaded: %d." ), ret );
	    	}
	    	
	    else 
	    	{
	    	iCoreLog->Log( _L( "CSimAtkCore::Load \
	    	    Ecuart loading fails: %d." ), ret );
	    	}
	    }
	    	
    else 
    	{
    	iCoreLog->Log( _L( "CSimAtkCore::Load No TSY or CSY found!" ), ret );
    	}
    
    return ret;        
    }

// -----------------------------------------------------------------------------
// CSimAtkCore::ObjectType
// ObjectType method function.
// returns derived object type
// -----------------------------------------------------------------------------
// 	        
TInt CSimAtkCore::ObjectType()
	{
	iCoreLog->Log( _L( "CSimAtkCore::ObjectType %d" ), iObjectType ); 
	return iObjectType;		
	}
     
// -----------------------------------------------------------------------------
// CSimAtkCore::RunMethodCasesL
// Return function table.
// -----------------------------------------------------------------------------
//
EXPORT_C const TStifFunctionInfo* CSimAtkCore::RunMethodCases
        (
        TInt& aCount // Output, number of functions.
        )
    {  
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        
        ENTRY( "Connect", CSimAtkCore::Connect ),
        ENTRY( "Load", CSimAtkCore::Load ),
        ENTRY( "Open", CSimAtkCore::Open ),
        ENTRY( "Unload", CSimAtkCore::Unload ),
        ENTRY( "Close", CSimAtkCore::Close )
	    };

    aCount = sizeof( KFunctions ) / 
                sizeof( TStifFunctionInfo );

    return KFunctions;    
    } 
	
// -----------------------------------------------------------------------------
// CSimAtkCore::Unload
// Unload method function.
// -----------------------------------------------------------------------------
//
TInt CSimAtkCore::Unload
        (
        CStifItemParser& aItem
        )
    {    
	TInt ret( KErrNotFound );
    TPtrC string;
    aItem.GetNextString ( string );
    
    // PhoneTsy.tsy
    if ( _L( "PhoneTsy.tsy" ) == string )
     	{     	    
	    iCoreLog->Log( _L( "CSimAtkCore::Unload \
	        Starting to Unload PhoneTsy.tsy. " ) );
	    
	    ret = iRtel.UnloadPhoneModule( _L( "PhoneTsy.tsy" ) );
	   	
	   	if ( KErrNone == ret )
		    {
		    iCoreLog->Log( _L( "CSimAtkCore::Unload \
		        UnLoading PhoneTsy.tsy successfully: %d." ), ret );	
		    }
		    
	    else 
	    	{
	    	iCoreLog->Log( _L( "CSimAtkCore::Unload \
	    	    UnLoading PhoneTsy.tsy fails: %d." ), ret );
	    	}

		}
	
	// Ecuart.csy NOTE: this is not needed because closing RTelServer
	// closes the ecuart.csy. However it is left to code for now.
	// Remove from final release if no implementation required!
    else if ( _L( "Ecuart.csy" ) == string )
    	{	    
	    iCoreLog->Log( _L( "CSimAtkCore::Unload \
	        Starting to Unload Ecuart.csy." ) );
	        
	    ret = iRcomm.UnloadCommModule( _L( "Ecuart.csy" ) );
	    
	    if ( KErrNone == ret )
		    {
		    iCoreLog->Log( _L( "CSimAtkCore::Unload \
		        UnLoading Ecuart.csy successfully: %d." ), ret );	
		    }
		    
	    else 
	    	{
	    	iCoreLog->Log( _L( "CSimAtkCore::Unload \
	    	    UnLoading Ecuart.csy fails: %d." ), ret );
	    	}
		}		
	else
		{
		iCoreLog->Log( _L( "CSimAtkCore::Unload \
		    Can not unload anything: %d." ), ret );
		}
	    
    return ret;        
    }
    
//  End of File
