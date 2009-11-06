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

CAtCommandReq::CAtCommandReq( RModemAtController* aClient,
    const TDesC8& aCommand,
    TDes8& aResponse ) :
    CActive(EPriorityNormal),
    iClient(aClient)
    {
    C_TRACE((_L("CAtCommandReq::CAtCommandReq(0x%x, 0x%x, 0x%x) "), aClient, &aCommand, &aResponse));
    DUMP_MESSAGE( aCommand );
    CActiveScheduler::Add( this );
    aClient->SendReceiveATCommand( aCommand, aResponse, iStatus);
    SetActive();
    }

CAtCommandReq::~CAtCommandReq() 
    {
    C_TRACE((_L("CAtCommandReq::~CAtCommandReq()")));
    Cancel();
    }
 
 void CAtCommandReq::RunL() 
    {
    C_TRACE((_L("CAtCommandReq::RunL()")));
    iClient->ATCommandResponseReceived( iStatus.Int() );
    delete this;
    }
 
void CAtCommandReq::DoCancel() 
    {
    C_TRACE((_L("CAtCommandReq::DoCancel()")));
    iClient->SendReceiveAtCommandCancel();
    C_TRACE((_L("Status: %d"), iStatus.Int()));
    }

