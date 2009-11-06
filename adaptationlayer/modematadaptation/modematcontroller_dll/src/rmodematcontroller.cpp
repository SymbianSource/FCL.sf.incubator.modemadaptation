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
*
*/



#include "mmodematobserver.h"
#include "rmodematcontroller.h"
#include "modematclientsrv.h"
#include "modemattrace.h"
#include "catcommandreq.h"
#include "cunsoliciteddatareq.h"
#include "csignalindreq.h"
#include "ccommandmodereq.h"

_LIT( KATExtSrvExe, "modematcontroller.exe" );
const TUid KATExtSrvUid3 = { 0x2001FCB1 }; 

static TInt StartServer();

EXPORT_C RModemAtController::RModemAtController() :
    iAtRequest( NULL ), 
    iUnsolicitedDataReq( NULL ),
    iSignalIndReq( NULL ),
    iCommandModeReq( NULL )
    {
    C_TRACE((_L("RModemAtController::RModemAtController()")));
    }

EXPORT_C RModemAtController::~RModemAtController()
    {
    C_TRACE((_L("RModemAtController::~RModemAtController()")));
    if( iUnsolicitedDataReq || iAtRequest || iSignalIndReq || iCommandModeReq ) 
        {
        User::Panic( _L("NotCompleted"), 0 );
        }
#ifdef _DEBUG
    RThread thisRt;
    TInt count = thisRt.RequestCount();
    C_TRACE((_L(">RequestCount %d "), count));
#endif

#ifdef _DEBUG
    count = thisRt.RequestCount();	
   	C_TRACE((_L("<RequestCount %d "), count));
#endif 	
    }
    

EXPORT_C TInt RModemAtController::Connect(
    TATPluginInterface aInterface,
    TDesC8& aClientName,
    MAtControllerObserver* aObserver )
    {
    C_TRACE((_L("RModemAtController::Connect( aInterface: %d, aObserver: 0x%x)"), aInterface, aObserver));
    DUMP_MESSAGE( aClientName);
#ifdef _DEBUG
    RThread thisRt;
    TInt count = thisRt.RequestCount();	
   	C_TRACE((_L(">RequestCount %d "), count));
#endif	
    TVersion version( KServerMajorVersionNumber,
                      KServerMinorVersionNumber,
                      KServerBuildVersionNumber );

    iObserver = aObserver;
    iConnectedIf = aInterface;
    TInt retVal = CreateSession( KATExtSrvName, version);
    if ( retVal != KErrNone && retVal != KErrAlreadyExists )
        {
   	    C_TRACE((_L("RModemAtController::Connect StartServer")));
        retVal = StartServer();
        if ( retVal == KErrNone || retVal==KErrAlreadyExists )
            {
            retVal = CreateSession( KATExtSrvName, version );
            }
        }

    C_TRACE((_L("RModemAtController::Connect CreateSession returned: %d "), retVal));

    if ( retVal == KErrNone || retVal == KErrAlreadyExists )
        {
        TInt ifUid = aInterface;
        retVal = SendReceive( EATExtSetExtensionInterface, TIpcArgs(ifUid, &aClientName) );

        if( aInterface == ECommonPlugin )
            {
            ReceiveSignalInd();
            GetCommandMode();
            }

        C_TRACE((_L("RModemAtController::Connect EATExtSetExtensionInterface returned: %d "), retVal));
        if( retVal == KErrAlreadyExists )
            {
            C_TRACE( _L("RModemAtController::Connect Modem is already connected, session is ok") );
            retVal = KErrNone;
            }
        if( retVal == KErrServerTerminated)
            {
            C_TRACE( _L("RModemAtController::Connect Connection failed, server has terminated") );
            }
        }

    if ( retVal != KErrNone )
        {
        C_TRACE( _L("RModemAtController::Connect Closing session") );
        Close();
        }
#ifdef _DEBUG
    count = thisRt.RequestCount();	
   	C_TRACE((_L("<RequestCount %d "), count));
#endif
    return retVal;
    }

EXPORT_C TInt RModemAtController::Disconnect()
    {
    C_TRACE((_L("RModemAtController::Disconnect()")));

#ifdef _DEBUG
    RThread thisRt;
    TInt count = thisRt.RequestCount();	
   	C_TRACE((_L(">RequestCount %d "), count));
#endif

	C_TRACE((_L("RModemAtController session base handle: %d "), Handle()));
    if( Handle() )
	    {
        C_TRACE((_L("RModemAtController::Disconnect() remove pipe")));
        SendReceiveRemovePipe();
        }

    ReceiveSignalIndCancel( );
    GetCommandModeCancel();
    ReceiveUnsolicitedResultCancel();
    HandleATCommandCancel();

    Close();

#ifdef _DEBUG
    count = thisRt.RequestCount();	
   	C_TRACE((_L("<RequestCount %d "), count));
#endif

    return KErrNone;
    } 

EXPORT_C TInt RModemAtController::HandleATCommand(const TDesC8& aCommand, TDes8& aResponse ) 
    {
    C_TRACE((_L("RModemAtController::HandleATCommand") ));
    DUMP_MESSAGE( aCommand);
    if( iAtRequest )
        {
        TRACE_ASSERT_ALWAYS;
        return KErrAlreadyExists;
        }
    
    TRAPD( err, iAtRequest = new (ELeave)CAtCommandReq( this, aCommand, aResponse) );
    ASSERT_PANIC_ALWAYS( err == KErrNone );
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::HandleATCommandCancel()
    {
    C_TRACE((_L("RModemAtController::HandleATCommandCancel")));
    if( iAtRequest )
        {
        C_TRACE((_L("Cancelling...") ));
        iAtRequest->Cancel();
        delete iAtRequest;
        iAtRequest=NULL;
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::ReceiveSignalInd()
    {
    C_TRACE((_L("RModemAtController::ReceiveSignalInd")));
    if( iSignalIndReq ) 
        {
        TRACE_ASSERT_ALWAYS;
        return KErrAlreadyExists;
        }
    TRAPD( err, iSignalIndReq = new (ELeave) CSignalIndReq( this ) );
    ASSERT_PANIC_ALWAYS( err == KErrNone );
    return KErrNone;
    }


EXPORT_C TInt RModemAtController::ReceiveSignalIndCancel( ) 
    {
    C_TRACE((_L("RModemAtController::ReceiveSignalIndCancel")));
    if( iSignalIndReq ) 
        {
        C_TRACE((_L("Canceling...") ));
        iSignalIndReq->Cancel();
        delete iSignalIndReq;
        iSignalIndReq = NULL; //iSignalIndReq will delete itself when cancelled
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::ReceiveUnsolicitedResult( TDes8& aResult )
    {
    C_TRACE((_L("RModemAtController::ReceiveUnsolicitedResult")));
    if( iUnsolicitedDataReq ) 
        {
        TRACE_ASSERT_ALWAYS;
        return KErrAlreadyExists;
        }
    TRAPD( err, iUnsolicitedDataReq = new (ELeave) CUnsolicitedDataReq( this, aResult ) ); 
    ASSERT_PANIC_ALWAYS( err == KErrNone );
    return KErrNone;
    }

EXPORT_C  TInt RModemAtController::ReceiveUnsolicitedResultCancel( ) 
    {
    C_TRACE((_L("RModemAtController::ReceiveUnsolicitedResultCancel")));
    if( iUnsolicitedDataReq ) 
        {
        C_TRACE((_L("Canceling...")));
        iUnsolicitedDataReq->Cancel();
        delete iUnsolicitedDataReq;
        iUnsolicitedDataReq = NULL;
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::GetNvramStatus(TDesC8& aCommand, TDes8& aResponse ) 
    {
    C_TRACE((_L("RModemAtController::GetNvramStatus")));
    aResponse = _L8(""); //ensure, that response buffer is empty
    TInt status = SendReceive( EModemATGetNvramStatus, TIpcArgs(&aCommand, &aResponse));
    return status;
    }


EXPORT_C TInt RModemAtController::GetCommandMode() 
    {
    C_TRACE((_L("RModemAtController::GetCommandMode")));
    
    if( iCommandModeReq )
        {
        TRACE_ASSERT_ALWAYS;
        return KErrAlreadyExists;
        }
    TRAPD( err, iCommandModeReq = new (ELeave) CCommandModeReq( this ) ); 
    ASSERT_PANIC_ALWAYS( err == KErrNone );
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::GetCommandModeCancel()
    {
    C_TRACE((_L("RModemAtController::GetCommandMode")));
    if( iCommandModeReq )
        {
        C_TRACE((_L("Canceling...")));
        iCommandModeReq->Cancel();
        delete iCommandModeReq;
        iCommandModeReq = NULL;
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::RemovePipe()
    {
    C_TRACE((_L("RModemAtController::RemovePipe")));
    SendReceiveRemovePipe();
    return KErrNone;
    }

//called by reqs

void RModemAtController::SendReceiveATCommand( const TDesC8& aCommand,
    TDes8& aResponse, 
    TRequestStatus& aStatus )
    {
    C_TRACE((_L("RModemAtController::SendReceiveATCommand acommand len=%d aresponse len=%d"), aCommand.Length(), aResponse.Length() ));
    SendReceive( EModemATHandleCommand, TIpcArgs(&aCommand, &aResponse), aStatus);
    }

void  RModemAtController::SendReceiveUnsolicitedResult( TDes8& aResponse,TRequestStatus& aStatus) 
    {
    C_TRACE((_L("RModemAtController::SendReceiveUnsolicitedResult")));
    SendReceive( EReceiveUnsolicitedResult, TIpcArgs(&aResponse), aStatus);
    }

void RModemAtController::SendReceiveUnsolicitedResulCancel() 
    {        
    C_TRACE((_L("RModemAtController::SendReceiveUnsolicitedResulCancel")));
    TInt i = 0;
    SendReceive( EReceiveUnsolicitedResultCancel, TIpcArgs(i) );
    }

void RModemAtController::SendReceiveSignalInd(TRequestStatus& aStatus) 
    {
    C_TRACE((_L("RModemAtController::SendReceiveSignalInd()")));
    TInt value = 0;
    SendReceive( EModemATReceiveSignalInd, TIpcArgs(&value),aStatus);
    }

void RModemAtController::SendReceiveAtCommandCancel() 
    {
    C_TRACE((_L("RModemAtController::SendReceiveAtCommandCancel()")));
    TInt i=0;
    SendReceive( EModemATHandleCommandCancel,TIpcArgs(i));
    }

void RModemAtController::SignalIndReceived( TInt aErr ) 
    {
    C_TRACE((_L("RModemAtController::SignalIndReceived(%d)"), aErr));
    iObserver->HandleSignalIndication( aErr );
    }

void RModemAtController::SendReceiveSignalIndCancel() 
    {
    C_TRACE((_L("RModemAtController::SendReceiveSignalIndCancel()")));
    TInt value = 0;
    SendReceive( EModemATReceiveSignalIndCancel, TIpcArgs(&value));
    }

void RModemAtController::ATCommandResponseReceived( TInt aErr ) 
    {
    C_TRACE((_L("RModemAtController::ATCommandResponseReceived(%d)"), aErr));
    iObserver->HandleATCommandCompleted(aErr);
    iAtRequest = NULL;
    }

void RModemAtController::UnsolicitedResultReceived( TInt aErr ) 
    {
    C_TRACE((_L("RModemAtController::UnsolicitedResultReceived(%d)"), aErr));
    iObserver->HandleUnsolicitedResultReceived( aErr );
    }

void RModemAtController::SendReceiveCommandMode(TDes8& aResponse, TRequestStatus& aStatus) 
    {
    C_TRACE((_L("RModemAtController::SendReceiveCommandMode()")));
    SendReceive( EModemATCommandMode, TIpcArgs(&aResponse), aStatus);
    }

void RModemAtController::SendReceiveCommandModeCancel() 
    {
    C_TRACE((_L("RModemAtController::SendReceiveCommandModeCancel()")));
    TInt value = 0;
    SendReceive( EModemATCommandModeCancel, TIpcArgs(&value));
    }

void RModemAtController::CommandModeChanged( TInt aErr, TCommandMode aMode ) 
    {
    C_TRACE((_L("RModemAtController::CommandModeChanged(%d, %d)"), aErr, aMode));
    iObserver->HandleCommandModeChanged( aErr, aMode );
    }

TInt StartServer()
    {
    C_TRACE((_L("RModemAtController::StartServer()")));
    const TUidType serverUid( KNullUid, KNullUid, KATExtSrvUid3 );
    RProcess server;
    TInt retTemp = server.Create( KATExtSrvExe, KNullDesC, serverUid );
    server.SetPriority( EPriorityHigh );
    if ( retTemp != KErrNone )
        {
        return retTemp;
        }
    TRequestStatus status;
    server.Rendezvous( status );
    if ( status != KRequestPending )
        {
        server.Kill( 0 );
        }
    else
        {
        server.Resume();
        }
    User::WaitForRequest( status );
    C_TRACE((_L("Server started, code %d"), status.Int()));
    retTemp = ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
    server.Close();
    return retTemp;
    }

void RModemAtController::SendReceiveRemovePipe()
    {
    C_TRACE((_L(">>RModemAtController::SendReceiveRemovePipe()")));
    TInt value = 0;
    TRequestStatus removePipeStatus = KRequestPending;
    SendReceive( EModemAtRemovePipe, TIpcArgs(&value), removePipeStatus);
    User::WaitForRequest( removePipeStatus );
    C_TRACE((_L("<<RModemAtController::SendReceiveRemovePipe()")));
    }

