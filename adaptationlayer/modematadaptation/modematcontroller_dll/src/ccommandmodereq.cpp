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
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ccommandmodereqTraces.h"
#endif

const TUint KCommandModeBufLength = 1;

CCommandModeReq::CCommandModeReq( RModemAtController* aClient ) :
        CActive( EPriorityNormal ),
        iClient( aClient ),
        iResponseBuf(NULL),
        iResponse(NULL,0)
    { 
    OstTrace0( TRACE_NORMAL, CCOMMANDMODEREQ_CCOMMANDMODEREQ, "CCommandModeReq::CCommandModeReq" );
    C_TRACE((_L("CCommandModeReq::CCommandModeReq()")));
    CActiveScheduler::Add( this );
    iResponseBuf = HBufC8::NewL( KCommandModeBufLength );
    iResponse.Set( iResponseBuf->Des() );
    iClient->SendReceiveCommandMode( iResponse, iStatus );
    SetActive();
    }

CCommandModeReq::~CCommandModeReq() 
    {
    OstTrace0( TRACE_NORMAL, DUP1_CCOMMANDMODEREQ_CCOMMANDMODEREQ, "CCommandModeReq::~CCommandModeReq" );
    C_TRACE((_L("CCommandModeReq::~CCommandModeReq()")));
    Cancel();
    delete iResponseBuf;
    iResponseBuf = NULL;
    }

 void CCommandModeReq::RunL() 
    {
    OstTrace0( TRACE_NORMAL, CCOMMANDMODEREQ_RUNL, "CCommandModeReq::RunL" );
    C_TRACE((_L("CCommandModeReq::RunL()")));

    if( iStatus.Int() == KErrNone ) 
        {
        C_TRACE((_L("CCommandModeReq RunL response: 0x%x"), &iResponse));
        TLex8 lex( iResponse );
        TInt mode( 0 );
        if( lex.Val( mode ) == KErrNone )
            {
            OstTrace1( TRACE_NORMAL, DUP1_CCOMMANDMODEREQ_RUNL, "CCommandModeReq command mode;mode=%d;changed", mode );
            C_TRACE((_L("CCommandModeReq command mode %d changed"), mode));
            iClient->CommandModeChanged( iStatus.Int(), (TCommandMode) mode );
            // Start receiving again
            iResponse.Zero();
            iClient->SendReceiveCommandMode( iResponse, iStatus );
            SetActive();
            }
        else
            {
            OstTrace1( TRACE_NORMAL, DUP2_CCOMMANDMODEREQ_RUNL, "CCommandModeReq RunL lex failed;lex.Val( mode )=%d", lex.Val( mode ) );
            C_TRACE((_L("CCommandModeReq RunL lex failed %d"), lex.Val( mode )));
            delete this;
            }
        }
    else if( iStatus.Int() == KErrCancel )
        {
        OstTrace0( TRACE_NORMAL, DUP3_CCOMMANDMODEREQ_RUNL, "CCommandModeReq RunL cancelled - delete" );
        C_TRACE((_L("CCommandModeReq RunL cancelled - delete")));
        delete this;
        }
    }
 
void CCommandModeReq::DoCancel() 
    {
    OstTrace0( TRACE_NORMAL, CCOMMANDMODEREQ_DOCANCEL, "CCommandModeReq::DoCancel" );
    C_TRACE((_L("CCommandModeReq::DoCancel()")));
    iClient->SendReceiveCommandModeCancel();
    }

