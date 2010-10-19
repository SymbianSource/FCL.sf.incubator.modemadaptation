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



#include "cunsoliciteddatareq.h"
#include "modemattrace.h"
#include "modematclientsrv.h"
#include "rmodematcontroller.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cunsoliciteddatareqTraces.h"
#endif

CUnsolicitedDataReq::CUnsolicitedDataReq( RModemAtController* aClient, TDes8& aResponse ) :
        CActive(EPriorityNormal),
        iClient(aClient),
        iResponse(aResponse)
    { 
    OstTrace1( TRACE_NORMAL, CUNSOLICITEDDATAREQ_CUNSOLICITEDDATAREQ, "CUnsolicitedDataReq::CUnsolicitedDataReq;aClient=%x", aClient );
    C_TRACE((_L("CUnsolicitedDataReq::CUnsolicitedDataReq(aClient: 0x%x this: 0x%x)"), aClient, this));
    CActiveScheduler::Add( this );
    aClient->SendReceiveUnsolicitedResult(aResponse,iStatus);
    SetActive();
    }

CUnsolicitedDataReq::~CUnsolicitedDataReq() 
    {    
    OstTrace0( TRACE_NORMAL, DUP1_CUNSOLICITEDDATAREQ_CUNSOLICITEDDATAREQ, "CUnsolicitedDataReq::~CUnsolicitedDataReq" );
    C_TRACE((_L("CUnsolicitedDataReq::~CUnsolicitedDataReq()")));
    Cancel();
    }

void CUnsolicitedDataReq::RunL() 
    {
    OstTrace0( TRACE_NORMAL, CUNSOLICITEDDATAREQ_RUNL, "CUnsolicitedDataReq::RunL" );
    C_TRACE((_L("CUnsolicitedDataReq::RunL()")));
    if( iStatus.Int() == KErrNone ) 
        {
        OstTrace1( TRACE_NORMAL, DUP1_CUNSOLICITEDDATAREQ_RUNL, "iClient->UnsolicitedResultReceive;this=%x", this );
        C_TRACE((_L("CUnsolicitedDataReq iClient->UnsolicitedResultReceive this: 0x%x"), this));
        iClient->UnsolicitedResultReceived(iStatus.Int());  
        OstTrace1( TRACE_NORMAL, DUP2_CUNSOLICITEDDATAREQ_RUNL, "CUnsolicitedDataReq::RunL - starting again;iStatus.Int()=%d", iStatus.Int() );
        C_TRACE((_L("CUnsolicitedDataReq RunL starting again %d"), iStatus.Int()));
        iClient->SendReceiveUnsolicitedResult( iResponse, iStatus );
        SetActive();
        }
    else
        {
        OstTrace1( TRACE_NORMAL, DUP3_CUNSOLICITEDDATAREQ_RUNL, "CUnsolicitedDataReq::RunL - deleting instance;this=%x", this );
        C_TRACE((_L("CUnsolicitedDataReq::RunL() deleting instance this: 0x%x"), this));
        delete this;
        }
    }

void CUnsolicitedDataReq::DoCancel() 
    {
    OstTrace0( TRACE_NORMAL, CUNSOLICITEDDATAREQ_DOCANCEL, "CUnsolicitedDataReq::DoCancel" );
    C_TRACE((_L("CUnsolicitedDataReq::DoCancel")));
    iClient->SendReceiveUnsolicitedResulCancel();
    }

