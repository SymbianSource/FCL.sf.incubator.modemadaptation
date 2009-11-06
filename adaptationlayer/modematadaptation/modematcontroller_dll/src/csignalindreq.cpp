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

CSignalIndReq::CSignalIndReq( 
        RModemAtController* aClient) :
        CActive(EPriorityNormal),
        iClient(aClient)
    { 
    C_TRACE((_L("CSignalIndReq::CSignalIndReq(0x%x)"), aClient));
    CActiveScheduler::Add( this );
    aClient->SendReceiveSignalInd( iStatus );
    SetActive();
    }

CSignalIndReq::~CSignalIndReq() 
    {
    C_TRACE((_L("CSignalIndReq::~CSignalIndReq()")));
    Cancel();
    }

void CSignalIndReq::DoCancel() 
    {
    C_TRACE((_L("CSignalIndReq::DoCancel()")));
    iClient->SendReceiveSignalIndCancel();
    }
 
 void CSignalIndReq::RunL() 
     {
    C_TRACE((_L("CSignalIndReq::RunL()")));
     if( iStatus.Int() == KErrNone ) 
        {
        C_TRACE((_L("KErrNone %d"), iStatus.Int()));
        iClient->SignalIndReceived( iStatus.Int() );
        iClient->SendReceiveSignalInd( iStatus );
        SetActive();
        }
     else 
        {
        C_TRACE((_L("CSignalIndReq RunL delete %d"), iStatus.Int()));
        delete this;
        }
     }
