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
* Description:   Specific class for SIM ATK TSY tests
*
*/




// INCLUDE FILES
#include <stifparser.h>
#include "simatk.h"
#include "saticon.h"
#include "saterrors.h"

#include "commandgetinkey.h"
#include "commanddisplaytext.h"
#include "commandgetinput.h"
#include "commandsendss.h"
#include "commandsendsm.h"
#include "commandsetupcall.h"
#include "commandplaytone.h"
#include "commandsetupmenu.h"
#include "commandselectitem.h"
#include "satcallcontrol.h"
#include "satmosmscontrol.h"
#include "commandrefresh.h"
#include "commandsendussd.h"
#include "commandlocalinfo.h"
#include "commandsetupeventlist.h"
#include "commandsetupidlemodetext.h"
#include "commandopenchannel.h"
#include "commandsenddata.h"
#include "commandlaunchbrowser.h"
#include "commandsenddtmf.h"
#include "commandlanguagenotification.h"
#include "commandclosechannel.h"
#include "commandreceivedata.h"
#include "commandgetchannelstatus.h"


// CONSTANTS
_LIT( KCommandType, "CommandType" );

// Commands can omit SessionEnd procedure from script by setting this flag to 0
_LIT( KSessionEnd, "SessionEnd" );


// -----------------------------------------------------------------------------
// CSimAtk::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CSimAtk::Delete() 
    {
    iCoreLog->Log( _L( "CSimAtk::Delete" ) );    
    // No implementation required
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CSimAtk::RunMethodL
        ( 
        CStifItemParser& aItem
        )
    {
    iCoreLog->Log( _L( "CSimAtk::RunMethodL" ) ); 
    
    // Store parser
    if ( !iParser )   
        {
        iParser = &aItem;
        }
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
    
        // Generic    
        ENTRY( "Open", CSimAtk::Open ),
        ENTRY( "Close", CSimAtk::Close ),
        ENTRY( "WaitForRequest", CSimAtk::WaitForRequest ),
        ENTRY( "Delay", CSimAtk::Delay ),
        // ISC Control
        ENTRY( "LoadIscTests", CSimAtk::LoadIscTests ),
        ENTRY( "RunIscTestCase", CSimAtk::RunIscTestCase ),
        ENTRY( "SyncIscTestCase", CSimAtk::SyncIscTestCase ),
        ENTRY( "CancelTestCase", CSimAtk::CancelTestCase ),

        ENTRY( "CheckData", CSimAtk::CheckData ),
        ENTRY( "CheckProactiveCommand", CSimAtk::CheckData ),
        ENTRY( "CheckIcon", CSimAtk::CheckIcon ),
        
        ENTRY( "SendResponse", CSimAtk::SendResponse ),
        ENTRY( "SendEnvelope", CSimAtk::SendResponse ),
        ENTRY( "SendTerminalResponse", CSimAtk::SendResponse )
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::Open
// Open method function.
// -----------------------------------------------------------------------------
// 
TInt CSimAtk::Open
        (
        CStifItemParser& aItem
        )
	{
    iCoreLog->Log( _L( "CSimAtk::Open" ) );    
	   
    TInt ret( KErrNotFound );
	TPtrC string;
    aItem.GetString ( _L( "Open" ), string );    
    
    if ( _L( "SAT" ) == string )
        {
        ret = iSat.Open( iRphone );
        iCoreLog->Log( _L( "CSimAtk::Open: SAT, return %d" ), ret );
        }
    else if ( _L( "IscControl" ) == string )
        {
        ret = iRIscTestControl.Connect();
        iCoreLog->Log( _L( "CSimAtk::Open: IscControl, return %d" ), ret );
        }
    else if ( _L( "SmsMessaging" ) == string )
        {
        ret = iSmsMessaging.Open( iRphone );
        iCoreLog->Log( _L( "CSimAtk::Open: SmsMessaging, return %d" ), ret );
        }
    else
        {           
        // If any Open item not found in SimAtk,then checks Core.
    	CSimAtkCore::SetString( string );
    	ret = CSimAtkCore::Open( aItem );
        }
        
	return ret;	
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::Close
// Close method function.
// -----------------------------------------------------------------------------
// 
TInt CSimAtk::Close
        (
        CStifItemParser& aItem
        )
	{    
    iCoreLog->Log( _L( "CSimAtk::Close" ) );    
    TInt ret( KErrNotFound );
	TPtrC string;
    aItem.GetString ( _L( "Close" ), string );
    
    if ( _L( "SAT" ) == string)
        {
        iSat.Close(); 
        ret = KErrNone;
        }
    else if ( _L( "IscControl" ) == string)
        {
        iRIscTestControl.Close();
        ret = KErrNone;
        }
    else if ( _L( "SmsMessaging" ) == string)
        {
        iSmsMessaging.Close();
        ret = KErrNone;
        }
    else
        {          
        // If any Close item not found in SimAtk, then checks Core.
    	CSimAtkCore::SetString( string );
    	ret = CSimAtkCore::Close( aItem );
        }
        
	return ret;	
    }

// -----------------------------------------------------------------------------
// CSimAtk::NotifySimAtkL
// Activates the notify
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::NotifySimAtkL
        (
        TRequestStatus& aReqStatus,
        const TSatIpc& aNotifyIpc
        )
    {
    iCoreLog->Log( _L( "CSimAtk::NotifySimAtk" ) );    
    
    TInt ret( KErrNone );
    
    CSatBase* command = NULL;
    
    TPtrC notifyName( MapIpcToString( aNotifyIpc ) );    
    
    TInt index ( 0 );
    
    // Create test class only if it does not already exist
    if ( KErrNotFound == FindIndex ( aNotifyIpc, index ) )
        {
        switch ( aNotifyIpc )
            {
            case ESatNotifyDisplayTextPCmd:
        	    {
        	    command = new (ELeave) CCommandDisplayText( aNotifyIpc, iSat,
        	        *iParser, *iCoreLog );
        		break;     
                }
            case ESatNotifyGetInkeyPCmd:
                {
        	    command = new (ELeave) CCommandGetInkey( aNotifyIpc, iSat,
        	        *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyGetInputPCmd:
                {
        	    command = new (ELeave) CCommandGetInput( aNotifyIpc, iSat,
        	        *iParser, *iCoreLog );           
                break;
                }
            case ESatNotifyPlayTonePCmd:
                {
                command = new (ELeave) CCommandPlayTone( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyRefreshPCmd:
                {
                command = new (ELeave) CCommandRefresh( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyRefreshRequiredParam:
                {
                command = new (ELeave) CCommandRefresh( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySendSsPCmd:
                {
                command = new (ELeave) CCommandSendSs( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                
                // Set Cc pointer    
                CCommandSendUssd* ss 
                    = static_cast<CCommandSendUssd*> ( command );
                ss->SetCallControl( &iCallControl );
                break;
                }
            case ESatNotifySendSmPCmd:
                {
                command = new (ELeave) CCommandSendSm( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySetUpCallPCmd:
                {
                command = new (ELeave) CCommandSetUpCall( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                
                // Set Cc pointer    
                CCommandSendUssd* call 
                    = static_cast<CCommandSendUssd*> ( command );
                call->SetCallControl( &iCallControl );
                break;
                }
            case ESatNotifySetUpMenuPCmd:
                {
                command = new (ELeave) CCommandSetUpMenu( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySelectItemPCmd:
                {
                command = new (ELeave) CCommandSelectItem( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySendUssdPCmd:
                {
                command = new (ELeave) CCommandSendUssd( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                
                // Set Cc pointer    
                CCommandSendUssd* ussd 
                    = static_cast<CCommandSendUssd*> ( command );
                ussd->SetCallControl( &iCallControl );

                break;
                }
            case ESatNotifyLocalInfoPCmd:
                {
                command = new (ELeave) CCommandLocalInfo( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySetUpEventListPCmd:
                {
                command = new (ELeave) CCommandSetUpEventList( aNotifyIpc,
                    iSat, *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySetUpIdleModeTextPCmd:
                {
                command = new (ELeave) CCommandSetUpIdleModeText( aNotifyIpc, 
                    iSat, *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyOpenChannelPCmd:
                {
                command = new (ELeave) CCommandOpenChannel( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySendDataPCmd:
                {
                command = new (ELeave) CCommandSendData( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyLaunchBrowserPCmd:
                {
                command = new (ELeave) CCommandLaunchBrowser( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifySendDtmfPCmd:
                {
                command = new (ELeave) CCommandSendDtmf( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyLanguageNotificationPCmd:
                {
                command = new (ELeave) CCommandLanguageNotification(
                    aNotifyIpc, iSat, *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyCloseChannelPCmd:
                {
                command = new (ELeave) CCommandCloseChannel( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyReceiveDataPCmd:
                {
                command = new (ELeave) CCommandReceiveData( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyGetChannelStatusPCmd:
                {
                command = new (ELeave) CCommandGetChannelStatus( aNotifyIpc,
                iSat, *iParser, *iCoreLog );
                break;
                }
            case ESatNotifyCallControlRequest:
                {
                command = new (ELeave) CSatCallControl( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );

                // Store pointer for SendSs/Ussd & Setup Call commands
                iCallControl = command;
                break;
                }
            case ESatNotifyMoSmControlRequest:
                {
                command = new (ELeave) CSatMoSmsControl( aNotifyIpc, iSat,
                    *iParser, *iCoreLog );
                break;
                }                
            case ESatNotifyProactiveSimSessionEnd:
                {
                // None. SimSessionEnd is notified automatically.
                break;
                }                
            default:
        		{
        		iCoreLog->Log( _L( "CSimAtk::NotifySimAtk  %S not supported! \
        		    Aborting..." ), &notifyName );
        		ret = KErrNotSupported;
        		CancelTestCase( ret );
        		break;
        		}
            }
            
        if ( KErrNone == ret )
            {
            // Put command to array
    	    iArray->AppendL( command );
            }
	    
        }
    else
        {
        // Reusing existing test class
        command = iArray->At( index );
        }
            
    
    // If command was created, notify SAT
    if ( NULL != command )
        {
        // Notify the SAT about command and SimSessionEnd
		iCoreLog->Log( _L( "CSimAtk::NotifySimAtk \
		    %S async method activated" ), &notifyName );

        command->NotifySat( aReqStatus );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSimAtk::NotifySimAtkCancel
// Cancels notifys
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::NotifySimAtkCancel
        (
        const TSatIpc& aNotify
        )
    {
    iCoreLog->Log( _L( "CSimAtk::NotifySimAtkCancel" ) );
    
    TInt ret( KErrNone );
    TInt index ( 0 );
    
    ret = FindIndex( aNotify, index );
    
    if ( KErrNone == ret )
        {
        iArray->At( index )->CancelRequest();
        
        iArray->Delete( index );
        
        if ( ESatNotifyCallControlRequest == aNotify )
            {
            iCallControl = NULL;
            }
    	
    	iCoreLog->Log( _L( "CSimAtk::NotifySimAtkCancel \
    	    Async method cancelled" ) ); 
        }
    else
        {
    	iCoreLog->Log( _L( "CSimAtk::NotifySimAtkCancel \
    	    Cancellation requested but command not found! Aborting..." ) );

        CancelTestCase( ret );
        }
    
    return ret;
    }
   
// -----------------------------------------------------------------------------
// CSimAtk::LoadIscTests
// Connects to ISC test control and loads XML file
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::LoadIscTests
        (
        CStifItemParser& aItem
        )
    {
    iCoreLog->Log( _L( "CSimAtk::LoadIscTests" ) );    

    TPtrC string;
    aItem.GetString ( _L( "LoadIscTests" ), string );    
    TInt ret( iRIscTestControl.LoadTestCases( string ) );   
 
    if ( KErrNone == ret )
        {
        iCoreLog->Log( _L( "CSimAtk::LoadIscTests:\
            %S loaded successfully." ), &string );
        }
    else
        {
        iCoreLog->Log( _L( "CSimAtk::LoadIscTests \
            Loading failed: %S " ), &string );
            
        TPtrC errorString = MapErrorCodeToString( ret );
        
        iCoreLog->Log( _L( "CSimAtk::LoadIscTests \
            %S" ), &errorString );
        }
    
    return ret;
    } 

// -----------------------------------------------------------------------------
// CSimAtk::RunIscTestCase
// Starts the test procedure
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::RunIscTestCase
        (
        CStifItemParser& aItem
        )
    {
    iCoreLog->Log( _L( "CSimAtk::RunIscTestCase" ) );
    
    TInt ret( KErrNone );
    
    // XML file ID
    TInt xmlId(0);    
    if ( KErrNone == ( ret = aItem.GetInt( _L( "RunIscTestCase" ), xmlId ) ) )
        {
        // Test case ID
        TInt testId(0);
        if ( KErrNone == ( ret = aItem.GetNextInt( testId ) ) )
            {
            iCoreLog->Log( _L( "CSimAtk::RunIscTestCase: Id: %d" ), testId );
            TInt test =  ( xmlId << 20 ) | testId;
            iRIscTestControl.RunTestCase( test, iIscStatus );
            }
        }
    if ( KErrNone != ret )
        {
        iCoreLog->Log( _L( "CSimAtk::RunIscTestCase Running failed!" ) );
        CancelTestCase( ret );
        }
   
    return ret; 
    }

// -----------------------------------------------------------------------------
// CSimAtk::WaitForStatus
// Halts the execution until an async req is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::WaitForRequest
        (
        CStifItemParser& /*aItem*/
        )
    {
    iCoreLog->Log( _L( "CSimAtk::WaitForRequest" ) );    

    User::WaitForRequest( iIscStatus );
    
    TInt errorStatus( iIscStatus.Int() );
    
    if ( KErrNone == errorStatus && iNeedSessionEnd && iSessionEnd.TimedOut() )
        {
        errorStatus = KErrTimedOut;            
        }
        
    if ( KErrNone != errorStatus )
        {
        CancelTestCase( errorStatus );        
        }
       
    return errorStatus; 
    }

// -----------------------------------------------------------------------------
// CSimAtk::SyncIscTestCase
// Connects to ISC test control and loads XML file
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::SyncIscTestCase
        (
        CStifItemParser& /*aItem*/
        )
    {
    TInt ret ( KErrNone );
    
    iCoreLog->Log( _L( "CSimAtk::SyncIscTestCase" ) );    
    
    ret = iRIscTestControl.SyncTestCase();
    
    iCoreLog->Log( _L( "CSimAtk::SyncIscTestCase \
        Synch complete. Status: %d" ), ret );
        
    if ( KErrNone != ret )
        {
        CancelTestCase( ret );
        }
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::CancelTestCase
// Test client originated test cancellation
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::CancelTestCase
        (
        const TInt aReason
        )
    {
    TPtrC error( MapErrorCodeToString( aReason ) );
    
    iCoreLog->Log( _L( "CSimAtk::CancelTestCase: %S" ), &error );

    if ( iArray )
        {
        // Cancel all notifys and empty the array
        for ( TInt i = 0 ; i < iArray->Count() ; i ++ )
            {
            iArray->At( i )->CancelRequest();
            }
        iArray->Reset();

        iCallControl = NULL;
        }
        
    iSessionEnd.Cancel();

    iRIscTestControl.CancelTestCase();
    
    
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::CancelTestCase
// Script originated test cancellation
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::CancelTestCase
        (
        CStifItemParser& /*aItem*/
        )
    {
    iCoreLog->Log( _L( "CSimAtk::CancelTestCase" ) );    

    if ( iArray )
        {
        // Cancel all notifys and empty the array
        for ( TInt i = 0 ; i < iArray->Count() ; i ++ )
            {
            iArray->At( i )->CancelRequest();
            }
        iArray->Reset();

        iCallControl = NULL;
        }

    iRIscTestControl.CancelTestCase();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSimAtk::Delay
// Halt script execution for given time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSimAtk::Delay
        (
        CStifItemParser& aItem
        )
    {
    iCoreLog->Log( _L( "CSimAtk::Delay" ) );
    
    TInt delay(0);
    aItem.GetNextInt( delay );
    User::After( delay );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::CheckData
// Verifies the validity of received data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSimAtk::CheckData
        (
        CStifItemParser& aItem
        )
    {
    iCoreLog->Log( _L( "CSimAtk::CheckData" ) );    

    TInt ret( KErrNone );
    TInt commandIndex;
    
    if ( KErrNone == FindIndex( aItem, commandIndex ) )
        {
        ret = iArray->At( commandIndex )->CheckData();
        }
    else
        {
		iCoreLog->Log( _L( "CSimAtk::CheckData Unidentified command" ) );
		ret = KErrGeneral;
        }
    
    // Proactive command data was invalid or unexpected command received
    if (KErrNone != ret)
        {
        iCoreLog->Log( _L( "CSimAtk::CheckData \
            Checking of data failed, aborting." ) );
        CancelTestCase( ret );
        }
    
    return ret;    
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::SendResponse
// Creates data packet from a script and sends it to Sim
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSimAtk::SendResponse
        (
        CStifItemParser& aItem
        )
    {
    iCoreLog->Log( _L( "CSimAtk::SendResponse" ) );    

    TInt ret( KErrNone );
    TInt commandIndex;
    
    if ( KErrNone == FindIndex( aItem, commandIndex ) )
        {
        // Find command from array
        CSatCommandBase* proactiveCommand = 
            static_cast<CSatCommandBase*>(iArray->At( commandIndex ) );
            
        // Check if notifying of SessionEnd is omitted from script. Needed
        // by default
        iNeedSessionEnd = ETrue;
        aItem.GetInt( KSessionEnd, iNeedSessionEnd );
        
        // In case of sending envelopes, SessionEnd has to be notified here
        if ( iNeedSessionEnd && KRequestPending != iSessionEnd.Status() )
            {
            iSessionEnd.NotifySat(); 
            }        

        ret = proactiveCommand->SendResponse();
        
        if ( iNeedSessionEnd && KErrNone == ret )
            {
            // Sending OK, wait for a while for session end
            ret = iSessionEnd.WaitSessionEnd();
            }
        
        switch ( ret )
            {
            case KErrNone:
                {
        		iCoreLog->Log( _L( "CSimAtk::SendResponse \
        		    Response sent successfully" ), ret );
                break;
                }
            case KErrTimedOut:
                {
                // When there's data coming from SIM, SessionEnd is not
                // completed and time out occurs. This is not error however,
                // final check for SessionEnd status is made in WaitForRequest
                ret = KErrNone;
                
        		iCoreLog->Log( _L( "CSimAtk::SendResponse \
        		    SessionEnd Timed out" ) );
                break;
                }
            default:
                {
        		iCoreLog->Log( _L( "CSimAtk::SendResponse \
        		    Command returned an error code %d!" ), ret );
                break;
                }
            }
        }
    else
        {
		iCoreLog->Log( _L( "CSimAtk::SendResponse \
		    Unidentified command" ) );
		ret = KErrGeneral;
        }

    if ( KErrNone != ret )
        {
        CancelTestCase( ret );
        }

    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::CheckIcon
// Verifies the validity of provided icon info
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSimAtk::CheckIcon
        (
        CStifItemParser& aItem
        )
    {
    TInt ret( KErrNone );

    // Try to load mandatory SimType parameter from script
    TPtrC simType;
    
    if ( KErrNone == aItem.GetString( KLoadIcon, simType ) )
        {
        TInt instanceNumber( 0 );
        TInt recordId( 0 );
        
        // Get instance number identifier
        if ( KErrNone != aItem.GetNextInt( instanceNumber ) )
            {
            ret = KErrArgument;
            }
        // Get record id.
        else if ( KErrNone != aItem.GetNextInt( recordId ) )
            {
            ret = KErrArgument;
            }
        else
            {
            CSatIcon satIcon( iSat, simType, recordId );
            ret = satIcon.CheckIcon( iCoreLog, instanceNumber );
            }
        }
    else
        {
        ret = KErrArgument;
        }
        
    if ( KErrNone != ret )
        {
        CancelTestCase( ret );
        }        
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::FindIndex
// Finds the index of command in array based on the script parameter
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::FindIndex
        (
        CStifItemParser& aItem, // Reference to parser
        TInt& aIndex // Index output
        )
    {
    iCoreLog->Log( _L( "CSimAtk::FindIndex (by name)") );
                
    TInt ret ( KErrNotFound );
    TPtrC cmdName;
    
    if ( iArray )
        {
        if ( KErrNone != aItem.GetString( KCommandType, cmdName ) )
            {
            // Script did not provide command type.
            // Return index 0 (first element) as a default.
            if ( 0 < iArray->Count() )
                {
                aIndex = 0;
                ret = KErrNone;
                }
            }
        else
            {
            // Script provided a command type. For example:
            // "CommandType ESatNotifyDisplayTextPCmd"
            // Loop throught the array and look for matching command type.
            // If array contains several commands of same type, latest is.
            // returned            
            TBool requestPending( EFalse ); // Not used
            TSatIpc requestedCommand = 
                MapStringToIpc( cmdName, requestPending );
            
            // Find command based on type
            ret = FindIndex( requestedCommand, aIndex );
            }
        } // if (iArray)
    else
        {
        // iArray is NULL
        ret = KErrCorrupt;
        }
        
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::FindIndex
// Finds the index of command in array based on the ipc of the command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSimAtk::FindIndex
        (
        const TSatIpc& aRequestedIpc, // Requested type
        TInt& aIndex // Index output
        )
    {
    iCoreLog->Log( _L( "CSimAtk::FindIndex (by IPC)") );
    TInt ret ( KErrNotFound );
    
    if ( iArray )
        {
        TInt i = iArray->Count() - 1;
        
        for ( ; KErrNotFound == ret && i >= 0 ; i-- )
            {
            if ( iArray->At( i )->MatchIpc( aRequestedIpc ) )
                {
                aIndex = i;
                ret = KErrNone;
                }
            }
        }
        
    if ( KErrNone != ret )        
        {
        TPtrC cmdName( MapIpcToString( aRequestedIpc ) );
        iCoreLog->Log( _L( "CSimAtk::FindIndex \
            %S not found!"), &cmdName );
        }        
        
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSimAtk::MapErrorCodeToString
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
const TPtrC CSimAtk::MapErrorCodeToString( const TInt& aErrorCode )
    {
    TPtrC errorString( KNullDesC );
    
    for ( TUint8 i( 0 ) ; errorString == KNullDesC 
          && i < KSatErrorListSize ; i++ )
        {
        if ( KSatErrorList[i].iErrorCode == aErrorCode )
            {
            errorString.Set( KSatErrorList[i].iErrorString );
            }
        }
        
    if ( errorString == KNullDesC )
        {
        errorString.Set( _L( "Unidentified error code" ) );

        iCoreLog->Log( _L( "CSimAtk::MapErrorCodeToString \
            Error code %d" ), aErrorCode );
        }

    return errorString;
    }
    
//  End of File
