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



#include "csignalindreq.h"
#include "modemattrace.h"
#include "modematclientsrv.h"
#include "rmodematcontroller.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csignalindreqTraces.h"
#endif

CSignalIndReq::CSignalIndReq( 
        RModemAtController* aClient) :
        CActive(EPriorityNormal),
        iClient(aClient)
    { 
    OstTrace1( TRACE_NORMAL, CSIGNALINDREQ_CSIGNALINDREQ, "CSignalIndReq::CSignalIndReq;aClient=%x", aClient );
    C_TRACE((_L("CSignalIndReq::CSignalIndReq(0x%x)"), aClient));
    CActiveScheduler::Add( this );
    aClient->SendReceiveSignalInd( iStatus );
    SetActive();
    }

CSignalIndReq::~CSignalIndReq() 
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSIGNALINDREQ_CSIGNALINDREQ, "CSignalIndReq::~CSignalIndReq" );
    C_TRACE((_L("CSignalIndReq::~CSignalIndReq()")));
    Cancel();
    }

void CSignalIndReq::DoCancel() 
    {
    OstTrace0( TRACE_NORMAL, CSIGNALINDREQ_DOCANCEL, "CSignalIndReq::DoCancel" );
    C_TRACE((_L("CSignalIndReq::DoCancel()")));
    iClient->SendReceiveSignalIndCancel();
    }
 
 void CSignalIndReq::RunL() 
     {
    OstTrace0( TRACE_NORMAL, CSIGNALINDREQ_RUNL, "CSignalIndReq::RunL" );
    C_TRACE((_L("CSignalIndReq::RunL()")));
     if( iStatus.Int() == KErrNone || iStatus.Int() > 0 ) 
        {
        OstTrace1( TRACE_NORMAL, DUP1_CSIGNALINDREQ_RUNL, "CSignalIndReq::RunL - Completing with;iStatus.Int()=%d", iStatus.Int() );
        C_TRACE((_L("Completing with %d"), iStatus.Int()));
        iClient->SignalIndReceived( iStatus.Int() );
        iClient->SendReceiveSignalInd( iStatus );
        SetActive();
        }
     else
        {
        OstTrace1( TRACE_NORMAL, DUP2_CSIGNALINDREQ_RUNL, "CSignalIndReq::RunL -  delete;iStatus.Int()=%d", iStatus.Int() );
        C_TRACE((_L("CSignalIndReq RunL delete %d"), iStatus.Int()));
        delete this;
        }
     }
