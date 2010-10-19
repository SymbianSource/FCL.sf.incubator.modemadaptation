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
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "rmodematcontrollerTraces.h"
#endif


_LIT( KATExtSrvExe, "modematcontroller.exe" );
const TUid KATExtSrvUid3 = { 0x2001FCB1 }; 

static TInt StartServer();

EXPORT_C RModemAtController::RModemAtController() :
    iAtRequest( NULL ), 
    iUnsolicitedDataReq( NULL ),
    iSignalIndReq( NULL ),
    iCommandModeReq( NULL )
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_RMODEMATCONTROLLER, "RModemAtController::RModemAtController" );
    C_TRACE((_L("RModemAtController::RModemAtController()")));
    }

EXPORT_C RModemAtController::~RModemAtController()
    {
    OstTrace0( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_RMODEMATCONTROLLER, "RModemAtController::~RModemAtController" );
    C_TRACE((_L("RModemAtController::~RModemAtController()")));
    if( iUnsolicitedDataReq || iAtRequest || iSignalIndReq || iCommandModeReq ) 
        {
        User::Panic( _L("NotCompleted"), 0 );
        }
#ifdef _DEBUG
    RThread thisRt;
    TInt count = thisRt.RequestCount();
    OstTrace1( TRACE_NORMAL, DUP2_RMODEMATCONTROLLER_RMODEMATCONTROLLER, "RModemAtController::~RModemAtController - >RequestCount;count=%d", count );
    C_TRACE((_L(">RequestCount %d "), count));
#endif

#ifdef _DEBUG
    count = thisRt.RequestCount();	
   	OstTrace1( TRACE_NORMAL, DUP3_RMODEMATCONTROLLER_RMODEMATCONTROLLER, "RModemAtController::~RModemAtController - <RequestCount;count=%d", count );
   	C_TRACE((_L("<RequestCount %d "), count));
#endif 	
    }
    

EXPORT_C TInt RModemAtController::Connect(
    TATPluginInterface aInterface,
    TDesC8& aClientName,
    MAtControllerObserver* aObserver )
    {
    OstTraceExt3( TRACE_NORMAL, RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect;aInterface=%d;aClientName=%s;aObserver=%p", aInterface, aClientName, aObserver );
    C_TRACE((_L("RModemAtController::Connect( aInterface: %d, aObserver: 0x%x)"), aInterface, aObserver));
    DUMP_MESSAGE( aClientName);
#ifdef _DEBUG
    RThread thisRt;
    TInt count = thisRt.RequestCount();	
   	OstTrace1( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - >RequestCount;count=%d", count );
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
   	    OstTrace0( TRACE_NORMAL, DUP2_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - StartServer" );
   	    C_TRACE((_L("RModemAtController::Connect StartServer")));
        retVal = StartServer();
        if ( retVal == KErrNone || retVal==KErrAlreadyExists )
            {
            retVal = CreateSession( KATExtSrvName, version );
            }
        }

    OstTrace1( TRACE_NORMAL, DUP3_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - CreateSession returned;retVal=%d", retVal );
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

        OstTrace1( TRACE_NORMAL, DUP4_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - EATExtSetExtensionInterface returned:;retVal=%d", retVal );
        C_TRACE((_L("RModemAtController::Connect EATExtSetExtensionInterface returned: %d "), retVal));
        if( retVal == KErrAlreadyExists )
            {
            OstTrace0( TRACE_NORMAL, DUP5_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - Modem is already connected, session is ok" );
            C_TRACE( _L("RModemAtController::Connect Modem is already connected, session is ok") );
            retVal = KErrNone;
            }
        if( retVal == KErrServerTerminated)
            {
            OstTrace0( TRACE_NORMAL, DUP6_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - Connection failed, server has terminated" );
            C_TRACE( _L("RModemAtController::Connect Connection failed, server has terminated") );
            }
        }

    if ( retVal != KErrNone )
        {
        OstTrace0( TRACE_NORMAL, DUP7_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - Closing session" );
        C_TRACE( _L("RModemAtController::Connect Closing session") );
        Close();
        }
#ifdef _DEBUG
    count = thisRt.RequestCount();	
   	OstTrace1( TRACE_NORMAL, DUP8_RMODEMATCONTROLLER_CONNECT, "RModemAtController::Connect - <RequestCount;count=%d", count );
   	C_TRACE((_L("<RequestCount %d "), count));
#endif
    return retVal;
    }

EXPORT_C TInt RModemAtController::Disconnect()
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_DISCONNECT, "RModemAtController::Disconnect" );
    C_TRACE((_L("RModemAtController::Disconnect()")));

#ifdef _DEBUG
    RThread thisRt;
    TInt count = thisRt.RequestCount();	
   	OstTrace1( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_DISCONNECT, "RModemAtController::Disconnect - >RequestCount;count=%d", count );
   	C_TRACE((_L(">RequestCount %d "), count));
#endif

	OstTrace1( TRACE_NORMAL, DUP2_RMODEMATCONTROLLER_DISCONNECT, "RModemAtController::Disconnect - session base handle:;Handle()=%d", Handle() );
	C_TRACE((_L("RModemAtController session base handle: %d "), Handle()));

    ReceiveSignalIndCancel( );
    GetCommandModeCancel();
    ReceiveUnsolicitedResultCancel();
    HandleATCommandCancel();

    if( Handle() )
        {
        OstTrace0( TRACE_NORMAL, DUP3_RMODEMATCONTROLLER_DISCONNECT, "RModemAtController::Disconnect - remove pipe" );
        C_TRACE((_L("RModemAtController::Disconnect() remove pipe")));
        SendReceiveRemovePipe();
        Close();
        }

#ifdef _DEBUG
    count = thisRt.RequestCount();	
   	OstTrace1( TRACE_NORMAL, DUP4_RMODEMATCONTROLLER_DISCONNECT, "RModemAtController::Disconnect - <RequestCount;count=%d", count );
   	C_TRACE((_L("<RequestCount %d "), count));
#endif

    return KErrNone;
    } 

EXPORT_C TInt RModemAtController::HandleATCommand(const TDesC8& aCommand, TDes8& aResponse ) 
    {
    OstTraceExt2( TRACE_NORMAL, RMODEMATCONTROLLER_HANDLEATCOMMAND, "RModemAtController::HandleATCommand;aCommand=%s;aResponse=%s", aCommand, aResponse );
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
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_HANDLEATCOMMANDCANCEL, "RModemAtController::HandleATCommandCancel" );
    C_TRACE((_L("RModemAtController::HandleATCommandCancel")));
    if( iAtRequest )
        {
        OstTrace0( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_HANDLEATCOMMANDCANCEL, "RModemAtController::HandleATCommandCancel - Cancelling..." );
        C_TRACE((_L("Cancelling...") ));
        iAtRequest->Cancel();
        delete iAtRequest;
        iAtRequest=NULL;
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::ReceiveSignalInd()
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_RECEIVESIGNALIND, "RModemAtController::ReceiveSignalInd" );
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
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_RECEIVESIGNALINDCANCEL, "RModemAtController::ReceiveSignalIndCancel" );
    C_TRACE((_L("RModemAtController::ReceiveSignalIndCancel")));
    if( iSignalIndReq ) 
        {
        OstTrace0( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_RECEIVESIGNALINDCANCEL, "RModemAtController::ReceiveSignalIndCancel - Cancelling..." );
        C_TRACE((_L("Canceling...") ));
        iSignalIndReq->Cancel();
        delete iSignalIndReq;
        iSignalIndReq = NULL; //iSignalIndReq will delete itself when cancelled
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::ReceiveUnsolicitedResult( TDes8& aResult )
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_RECEIVEUNSOLICITEDRESULT, "RModemAtController::ReceiveUnsolicitedResult" );
    C_TRACE((_L("RModemAtController::ReceiveUnsolicitedResult")));
    if( iUnsolicitedDataReq ) 
        {
        OstTrace0( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_RECEIVEUNSOLICITEDRESULT, "RModemAtController::ReceiveUnsolicitedResult - cancel previous request" );
        C_TRACE((_L("RModemAtController::ReceiveUnsolicitedResult (cancel previous request)")));
        ReceiveUnsolicitedResultCancel();
        }
    TRAPD( err, iUnsolicitedDataReq = new (ELeave) CUnsolicitedDataReq( this, aResult ) ); 
    ASSERT_PANIC_ALWAYS( err == KErrNone );
    return KErrNone;
    }

EXPORT_C  TInt RModemAtController::ReceiveUnsolicitedResultCancel( ) 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_RECEIVEUNSOLICITEDRESULTCANCEL, "RModemAtController::ReceiveUnsolicitedResultCancel" );
    C_TRACE((_L("RModemAtController::ReceiveUnsolicitedResultCancel")));
    if( iUnsolicitedDataReq ) 
        {
        OstTrace0( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_RECEIVEUNSOLICITEDRESULTCANCEL, "RModemAtController::ReceiveUnsolicitedResultCancel - Canceling..." );
        C_TRACE((_L("Canceling...")));
        iUnsolicitedDataReq->Cancel();
        delete iUnsolicitedDataReq;
        iUnsolicitedDataReq = NULL;
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::GetNvramStatus(TDesC8& aCommand, TDes8& aResponse ) 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_GETNVRAMSTATUS, "RModemAtController::GetNvramStatus" );
    C_TRACE((_L("RModemAtController::GetNvramStatus")));
    aResponse = _L8(""); //ensure, that response buffer is empty
    TInt status = SendReceive( EModemATGetNvramStatus, TIpcArgs(&aCommand, &aResponse));
    return status;
    }


EXPORT_C TInt RModemAtController::GetCommandMode() 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_GETCOMMANDMODE, "RModemAtController::GetCommandMode" );
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
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_GETCOMMANDMODECANCEL, "RModemAtController::GetCommandModeCancel" );
    C_TRACE((_L("RModemAtController::GetCommandMode")));
    if( iCommandModeReq )
        {
        OstTrace0( TRACE_NORMAL, DUP1_RMODEMATCONTROLLER_GETCOMMANDMODECANCEL, "RModemAtController::GetCommandModeCancel - Canceling..." );
        C_TRACE((_L("Canceling...")));
        iCommandModeReq->Cancel();
        delete iCommandModeReq;
        iCommandModeReq = NULL;
        }
    return KErrNone;
    }

EXPORT_C TInt RModemAtController::RemovePipe()
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_REMOVEPIPE, "RModemAtController::RemovePipe" );
    C_TRACE((_L("RModemAtController::RemovePipe")));
    SendReceiveRemovePipe();
    return KErrNone;
    }

//called by reqs

void RModemAtController::SendReceiveATCommand( const TDesC8& aCommand,
    TDes8& aResponse, 
    TRequestStatus& aStatus )
    {
    OstTraceExt2( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVEATCOMMAND, "RModemAtController::SendReceiveATCommand;aCommand.Length()=%d;aResponse.Length()=%d", aCommand.Length(), aResponse.Length() );
    C_TRACE((_L("RModemAtController::SendReceiveATCommand acommand len=%d aresponse len=%d"), aCommand.Length(), aResponse.Length() ));
    SendReceive( EModemATHandleCommand, TIpcArgs(&aCommand, &aResponse), aStatus);
    }

void  RModemAtController::SendReceiveUnsolicitedResult( TDes8& aResponse,TRequestStatus& aStatus) 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVEUNSOLICITEDRESULT, "RModemAtController::SendReceiveUnsolicitedResult" );
    C_TRACE((_L("RModemAtController::SendReceiveUnsolicitedResult")));
    SendReceive( EReceiveUnsolicitedResult, TIpcArgs(&aResponse), aStatus);
    }

void RModemAtController::SendReceiveUnsolicitedResulCancel() 
    {        
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVEUNSOLICITEDRESULCANCEL, "RModemAtController::SendReceiveUnsolicitedResulCancel" );
    C_TRACE((_L("RModemAtController::SendReceiveUnsolicitedResulCancel")));
    TInt i = 0;
    SendReceive( EReceiveUnsolicitedResultCancel, TIpcArgs(i) );
    }

void RModemAtController::SendReceiveSignalInd(TRequestStatus& aStatus) 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVESIGNALIND, "RModemAtController::SendReceiveSignalInd" );
    C_TRACE((_L("RModemAtController::SendReceiveSignalInd()")));
    TInt value = 0;
    SendReceive( EModemATReceiveSignalInd, TIpcArgs(&value),aStatus);
    }

void RModemAtController::SendReceiveAtCommandCancel() 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVEATCOMMANDCANCEL, "RModemAtController::SendReceiveAtCommandCancel" );
    C_TRACE((_L("RModemAtController::SendReceiveAtCommandCancel()")));
    TInt i=0;
    SendReceive( EModemATHandleCommandCancel,TIpcArgs(i));
    }

void RModemAtController::SignalIndReceived( TInt aErr ) 
    {
    OstTrace1( TRACE_NORMAL, RMODEMATCONTROLLER_SIGNALINDRECEIVED, "RModemAtController::SignalIndReceived;aErr=%d", aErr );
    C_TRACE((_L("RModemAtController::SignalIndReceived(%d)"), aErr));
    iObserver->HandleSignalIndication( aErr );
    }

void RModemAtController::SendReceiveSignalIndCancel() 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVESIGNALINDCANCEL, "RModemAtController::SendReceiveSignalIndCancel" );
    C_TRACE((_L("RModemAtController::SendReceiveSignalIndCancel()")));
    TInt value = 0;
    SendReceive( EModemATReceiveSignalIndCancel, TIpcArgs(&value));
    }

void RModemAtController::ATCommandResponseReceived( TInt aErr ) 
    {
    OstTrace1( TRACE_NORMAL, RMODEMATCONTROLLER_ATCOMMANDRESPONSERECEIVED, "RModemAtController::ATCommandResponseReceived;aErr=%d", aErr );
    C_TRACE((_L("RModemAtController::ATCommandResponseReceived(%d)"), aErr));
    iObserver->HandleATCommandCompleted(aErr);
    iAtRequest = NULL;
    }

void RModemAtController::UnsolicitedResultReceived( TInt aErr ) 
    {
    OstTrace1( TRACE_NORMAL, RMODEMATCONTROLLER_UNSOLICITEDRESULTRECEIVED, "RModemAtController::UnsolicitedResultReceived;aErr=%d", aErr );
    C_TRACE((_L("RModemAtController::UnsolicitedResultReceived(%d)"), aErr));
    iObserver->HandleUnsolicitedResultReceived( aErr );
    }

void RModemAtController::SendReceiveCommandMode(TDes8& aResponse, TRequestStatus& aStatus) 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVECOMMANDMODE, "RModemAtController::SendReceiveCommandMode" );
    C_TRACE((_L("RModemAtController::SendReceiveCommandMode()")));
    SendReceive( EModemATCommandMode, TIpcArgs(&aResponse), aStatus);
    }

void RModemAtController::SendReceiveCommandModeCancel() 
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVECOMMANDMODECANCEL, "RModemAtController::SendReceiveCommandModeCancel" );
    C_TRACE((_L("RModemAtController::SendReceiveCommandModeCancel()")));
    TInt value = 0;
    SendReceive( EModemATCommandModeCancel, TIpcArgs(&value));
    }

void RModemAtController::CommandModeChanged( TInt aErr, TCommandMode aMode ) 
    {
    OstTraceExt2( TRACE_NORMAL, RMODEMATCONTROLLER_COMMANDMODECHANGED, "RModemAtController::CommandModeChanged;aErr=%d;aMode=%d", aErr, aMode );
    C_TRACE((_L("RModemAtController::CommandModeChanged(%d, %d)"), aErr, aMode));
    iObserver->HandleCommandModeChanged( aErr, aMode );
    }

TInt StartServer()
    {
    OstTrace0( TRACE_NORMAL, _STARTSERVER, "::StartServer" );
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
    OstTrace1( TRACE_NORMAL, DUP1__STARTSERVER, "::StartServer - Server started;status.Int()=%d", status.Int() );
    C_TRACE((_L("Server started, code %d"), status.Int()));
    retTemp = ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
    server.Close();
    return retTemp;
    }

void RModemAtController::SendReceiveRemovePipe()
    {
    OstTrace0( TRACE_NORMAL, RMODEMATCONTROLLER_SENDRECEIVEREMOVEPIPE, "RModemAtController::SendReceiveRemovePipe" );
    C_TRACE((_L(">>RModemAtController::SendReceiveRemovePipe()")));
    TInt value = 0;
    TRequestStatus removePipeStatus = KRequestPending;
    SendReceive( EModemAtRemovePipe, TIpcArgs(&value), removePipeStatus);
    User::WaitForRequest( removePipeStatus );
    C_TRACE((_L("<<RModemAtController::SendReceiveRemovePipe()")));
    }

