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


#include "modemattrace.h"
#include "cmodematsrv.h"
#include "catmessage.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "catmessageTraces.h"
#endif

const TInt KReplyTypeOther( 3 );

CAtMessage::CAtMessage( const TUint8 aMessageType,
    CModemAtSession* aSession,
    const RMessage2& aRequest ): 
    iMessageType( aMessageType ),
    iSession( aSession ), 
    iRequest( aRequest ),
    iMessagePtr( NULL, 0)
    {
    OstTraceExt3( TRACE_NORMAL, CATMESSAGE_CATMESSAGE, "CAtMessage::CAtMessage;this=%p;aRequest=%p;aRequest.Handle()=%x", this, &aRequest, (TUint)aRequest.Handle() );
    C_TRACE(( _L("CAtMessage::CAtMessage() this: 0x%x, aRequest 0x%x handle: 0x%x"), this, &aRequest, aRequest.Handle() ));
    TInt length = aRequest.GetDesLength( KATModemCommandArgumentIndex );
    OstTraceExt2( TRACE_NORMAL, DUP2_CATMESSAGE_CATMESSAGE, "CAtMessage::CAtMessage;aMessageType=%d;length=%d", aMessageType, length );
    C_TRACE((_L("CAtMessage aMessageType: %d at command length: %d"), aMessageType, length ));
    iMessage = HBufC8::New( length );
    iMessagePtr.Set( iMessage->Des() );
    TInt err = aRequest.Read( KATModemCommandArgumentIndex, iMessagePtr );
    OstTrace1( TRACE_NORMAL, DUP3_CATMESSAGE_CATMESSAGE, "CAtMessage::CAtMessage - aRequest.Read err:;err=%d", err );
    C_TRACE((_L("aRequest.Read err: %d"), err ));
    if( err != KErrNone )
        {
        TRACE_ASSERT_ALWAYS;
        }
    DUMP_MESSAGE( iMessagePtr );
    iRequest.Write( KATModemResponseArgumentIndex, KEmptyString );
    OstTrace0( TRACE_NORMAL, DUP4_CATMESSAGE_CATMESSAGE, "CAtMessage::CAtMessage - done" );
    C_TRACE(( _L("CAtMessage done") ));
    }

CAtMessage::~CAtMessage()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CATMESSAGE_CATMESSAGE, "CAtMessage::~CAtMessage" );
    C_TRACE (( _L("CAtMessage::~CAtMessage()") ));
    if( !iRequest.IsNull() )
        {
        OstTraceExt3( TRACE_NORMAL, DUP5_CATMESSAGE_CATMESSAGE, "CAtMessage::~CAtMessage;this=%p;iRequest=%p;iRequest.Handle()=%x", this, &iRequest, (TUint)iRequest.Handle() );
        C_TRACE (( _L("CAtMessage::~CAtMessage() !iRequest.IsNull this: 0x%x iRequest: 0x%x handle: 0x%x"), this, &iRequest, iRequest.Handle() ));
        }
    if( iMessage )
    	{
        delete iMessage;
    	iMessage = NULL;
    	}
    }

void CAtMessage::CancelRequest()
    {
    OstTrace0( TRACE_NORMAL, CATMESSAGE_CANCELREQUEST, "CAtMessage::CancelRequest" );
    C_TRACE (( _L("CAtMessage::CancelRequest()") ));
    if( !iRequest.IsNull() )
       {
       OstTraceExt2( TRACE_NORMAL, DUP1_CATMESSAGE_CANCELREQUEST, "CAtMessage::CancelRequest - complete iRequest KErrCancel;iRequest=%p;iRequest.Handle()=%x", &iRequest, (TUint)iRequest.Handle() );
       C_TRACE (( _L("CAtMessage::CancelRequest complete iRequest KErrCancel iRequest: 0x%x handle 0x%x"), &iRequest, iRequest.Handle() ));
       iRequest.Complete( KErrCancel );
       }
    }

CModemAtSession* CAtMessage::GetSession() 
    {
    OstTrace1( TRACE_NORMAL, CATMESSAGE_GETSESSION, "CAtMessage::GetSession;iSession=%x", iSession );
    C_TRACE (( _L("CAtMessage::GetSession() 0x%x"), &iSession ));
    return iSession;
    }

TPtr8& CAtMessage::GetBuffer() 
    {
    OstTrace0( TRACE_NORMAL, CATMESSAGE_GETBUFFER, "CAtMessage::GetBuffer" );
    C_TRACE (( _L("CAtMessage::GetBuffer()")));
    return iMessagePtr;
    }

TUint8 CAtMessage::GetMessageType() 
    {
    OstTrace1( TRACE_NORMAL, CATMESSAGE_GETMESSAGETYPE, "CAtMessage::GetMessageType;iMessageType=%u", iMessageType );
    C_TRACE (( _L("CAtMessage::GetMessageType()"), iMessageType ));
    return iMessageType;
    }

void CAtMessage::ReceiveATResponse( const TDesC8& aResponse, TBool aComplete, TInt aCompleteCode ) 
    {
    //OstTraceExt3( TRACE_NORMAL, CATMESSAGE_RECEIVEATRESPONSE, "CAtMessage::ReceiveATResponse;aResponse=%p;aComplete=%d;aCompleteCode=%d", &aResponse, (TUint)aComplete, aCompleteCode );
    OstTrace1( TRACE_NORMAL, DUP1_CATMESSAGE_RECEIVEATRESPONSE, "CAtMessage::ReceiveATResponse - ;iMessageType=%d", iMessageType );
    C_TRACE (( _L("CAtMessage::ReceiveATResponse( aResponse 0x%x, aComplete %d )"), &aResponse, (TInt)aComplete ));
    C_TRACE (( _L("CAtMessage::ReceiveATResponse iMessageType: %d, aCompleteCode: %d"), iMessageType, aCompleteCode ));
    if( !iRequest.IsNull() )
        {
        OstTraceExt2( TRACE_NORMAL, DUP2_CATMESSAGE_RECEIVEATRESPONSE, "CAtMessage::ReceiveATResponse - Writing to iRequest:;iRequest=%p;iRequest.Handle()=%d", &iRequest, (TUint)iRequest.Handle() );
        C_TRACE (( _L("CAtMessage::ReceiveATResponse Writing to iRequest: 0x%x handle 0x%x"), &iRequest, iRequest.Handle() ));
        TInt length = iRequest.GetDesMaxLength( KATModemResponseArgumentIndex );
        HBufC8* intermediateBuffer = HBufC8::New( length );
        TPtr8 ptr = intermediateBuffer->Des();
        TInt readErr = iRequest.Read( KATModemResponseArgumentIndex, ptr );
        TRACE_ASSERT( readErr == KErrNone );
        if( ptr.Length() > 0 )
            {
            OstTrace1( TRACE_NORMAL, DUP3_CATMESSAGE_RECEIVEATRESPONSE, "CAtMessage::ReceiveATResponse - Complete with EReplyTypeOther :;ptr.Length()=%d", ptr.Length() );
            C_TRACE(( _T("Complete with EReplyTypeOther :%d"), ptr.Length() ));
            aCompleteCode = KReplyTypeOther;
            }
        ptr.Append( aResponse );
        DUMP_MESSAGE( ptr );
        TInt err = iRequest.Write( KATModemResponseArgumentIndex, ptr );
        TRACE_ASSERT( err == KErrNone );
        delete intermediateBuffer;
        intermediateBuffer = NULL;
        if( aComplete )
            {
            OstTraceExt4( TRACE_NORMAL, DUP4_CATMESSAGE_RECEIVEATRESPONSE, "CAtMessage::ReceiveATResponse -;completion code=%d;this=%p;iRequest=%p;iRequest.Handle()=%d", err, this, &iRequest, (TUint)iRequest.Handle() );
            C_TRACE (( _L("CAtMessage::ReceiveATResponse completing (%d) this: 0x%x iRequest: 0x%x handle: 0x%x"), err, this, &iRequest, iRequest.Handle() ));
            iRequest.Complete( aCompleteCode );
            }
        else
            {
            OstTrace1( TRACE_NORMAL, DUP5_CATMESSAGE_RECEIVEATRESPONSE, "CAtMessage::ReceiveATResponse - not completing intermediate data yet;err=%d", err );
            C_TRACE (( _L("CAtMessage::ReceiveATResponse not completing intermediate data yet(%d)"), err ));
            }
        }
    else
        {
        TRACE_ASSERT( 0 );
        }
    }

