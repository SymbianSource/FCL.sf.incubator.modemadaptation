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



#include "ccommandmodereq.h"
#include "modemattrace.h"
#include "modematclientsrv.h"
#include "rmodematcontroller.h"

const TUint KCommandModeBufLength = 1;

CCommandModeReq::CCommandModeReq( RModemAtController* aClient ) :
        CActive( EPriorityNormal ),
        iClient( aClient ),
        iResponseBuf(NULL),
        iResponse(NULL,0)
    { 
    C_TRACE((_L("CCommandModeReq::CCommandModeReq()")));
    CActiveScheduler::Add( this );
    iResponseBuf = HBufC8::NewL( KCommandModeBufLength );
    iResponse.Set( iResponseBuf->Des() );
    iClient->SendReceiveCommandMode( iResponse, iStatus );
    SetActive();
    }

CCommandModeReq::~CCommandModeReq() 
    {
    C_TRACE((_L("CCommandModeReq::~CCommandModeReq()")));
    Cancel();
    delete iResponseBuf;
    iResponseBuf = NULL;
    }

 void CCommandModeReq::RunL() 
    {
    C_TRACE((_L("CCommandModeReq::RunL()")));

    if( iStatus.Int() == KErrNone ) 
        {
        C_TRACE((_L("CCommandModeReq RunL response: 0x%x"), &iResponse));
        TLex8 lex( iResponse );
        TInt mode( 0 );
        if( lex.Val( mode ) == KErrNone )
            {
            C_TRACE((_L("CCommandModeReq command mode %d changed"), mode));
            iClient->CommandModeChanged( iStatus.Int(), (TCommandMode) mode );
            // Start receiving again
            iResponse.Zero();
            iClient->SendReceiveCommandMode( iResponse, iStatus );
            SetActive();
            }
        else
            {
            C_TRACE((_L("CCommandModeReq RunL lex failed %d"), lex.Val( mode )));
            delete this;
            }
        }
    else if( iStatus.Int() == KErrCancel )
        {
        C_TRACE((_L("CCommandModeReq RunL cancelled - delete")));
        delete this;
        }
    }
 
void CCommandModeReq::DoCancel() 
    {
    C_TRACE((_L("CCommandModeReq::DoCancel()")));
    iClient->SendReceiveCommandModeCancel();
    }

