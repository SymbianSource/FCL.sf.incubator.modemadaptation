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

CUnsolicitedDataReq::CUnsolicitedDataReq( RModemAtController* aClient, TDes8& aResponse ) :
        CActive(EPriorityNormal),
        iClient(aClient),
        iResponse(aResponse)
    { 
    C_TRACE((_L("CUnsolicitedDataReq::CUnsolicitedDataReq(aClient: 0x%x this: 0x%x)"), aClient, this));
    CActiveScheduler::Add( this );
    aClient->SendReceiveUnsolicitedResult(aResponse,iStatus);
    SetActive();
    }

CUnsolicitedDataReq::~CUnsolicitedDataReq() 
    {    
    C_TRACE((_L("CUnsolicitedDataReq::~CUnsolicitedDataReq()")));
    Cancel();
    }

void CUnsolicitedDataReq::RunL() 
    {
    C_TRACE((_L("CUnsolicitedDataReq::RunL()")));
    if( iStatus.Int() == KErrNone ) 
        {
        C_TRACE((_L("CUnsolicitedDataReq iClient->UnsolicitedResultReceive this: 0x%x"), this));
        iClient->UnsolicitedResultReceived(iStatus.Int());  
        C_TRACE((_L("CUnsolicitedDataReq RunL starting again %d"), iStatus.Int()));
        iClient->SendReceiveUnsolicitedResult( iResponse, iStatus );
        SetActive();
        }
    else
        {
        C_TRACE((_L("CUnsolicitedDataReq::RunL() deleting instance this: 0x%x"), this));
        delete this;
        }
    }

void CUnsolicitedDataReq::DoCancel() 
    {
    C_TRACE((_L("CUnsolicitedDataReq::DoCancel")));
    iClient->SendReceiveUnsolicitedResulCancel();
    }

