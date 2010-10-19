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



#include "catcommandreq.h"
#include "modemattrace.h"
#include "modematclientsrv.h"
#include "rmodematcontroller.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "catcommandreqTraces.h"
#endif

CAtCommandReq::CAtCommandReq( RModemAtController* aClient,
    const TDesC8& aCommand,
    TDes8& aResponse ) :
    CActive(EPriorityNormal),
    iClient(aClient)
    {
    OstTraceExt3( TRACE_NORMAL, CATCOMMANDREQ_CATCOMMANDREQ, "CAtCommandReq::CAtCommandReq;aClient=%p;aCommand=%p;aResponse=%p", aClient, &aCommand, &aResponse );
    OstTraceExt1( TRACE_NORMAL, DUP2_CATCOMMANDREQ_CATCOMMANDREQ, "CAtCommandReq::CAtCommandReq - ;aCommand=%s", aCommand );
    C_TRACE((_L("CAtCommandReq::CAtCommandReq(0x%x, 0x%x, 0x%x) "), aClient, &aCommand, &aResponse));
    DUMP_MESSAGE( aCommand );
    CActiveScheduler::Add( this );
    aClient->SendReceiveATCommand( aCommand, aResponse, iStatus);
    SetActive();
    }

CAtCommandReq::~CAtCommandReq() 
    {
    OstTrace0( TRACE_NORMAL, DUP1_CATCOMMANDREQ_CATCOMMANDREQ, "CAtCommandReq::~CAtCommandReq" );
    C_TRACE((_L("CAtCommandReq::~CAtCommandReq()")));
    Cancel();
    }
 
 void CAtCommandReq::RunL() 
    {
    OstTrace1( TRACE_NORMAL, CATCOMMANDREQ_RUNL, "CAtCommandReq::RunL() complete;iStatus.Int()=%d", iStatus.Int() );
    C_TRACE((_L("CAtCommandReq::RunL() complete: %d"), iStatus.Int()));
    iClient->ATCommandResponseReceived( iStatus.Int() );
    delete this;
    }
 
void CAtCommandReq::DoCancel() 
    {
    OstTrace0( TRACE_NORMAL, CATCOMMANDREQ_DOCANCEL, "CAtCommandReq::DoCancel" );
    C_TRACE((_L("CAtCommandReq::DoCancel()")));
    iClient->SendReceiveAtCommandCancel();
    C_TRACE((_L("Status: %d"), iStatus.Int()));
    }

