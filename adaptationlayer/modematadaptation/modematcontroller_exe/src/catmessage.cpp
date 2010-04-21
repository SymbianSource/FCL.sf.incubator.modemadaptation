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

CAtMessage::CAtMessage( const TUint8 aMessageType,
    CModemAtSession* aSession,
    const RMessage2& aRequest ): 
    iMessageType( aMessageType ),
    iSession( aSession ), 
    iRequest( aRequest ),
    iMessagePtr( NULL, 0)
    {
    C_TRACE(( _L("CAtMessage::CAtMessage() this: 0x%x, aRequest 0x%x handle: 0x%x"), this, &aRequest, aRequest.Handle() ));
    TInt length = aRequest.GetDesLength( KATModemCommandArgumentIndex );
    C_TRACE((_L("CAtMessage aMessageType: %d at command length: %d"), aMessageType, length ));
    iMessage = HBufC8::New( length );
    iMessagePtr.Set( iMessage->Des() );
    TInt err = aRequest.Read( KATModemCommandArgumentIndex, iMessagePtr );
    C_TRACE((_L("aRequest.Read err: %d"), err ));
    if( err != KErrNone )
        {
        TRACE_ASSERT_ALWAYS;
        }
    DUMP_MESSAGE( iMessagePtr );
    iRequest.Write( KATModemResponseArgumentIndex, KEmptyString );
    C_TRACE(( _L("CAtMessage done") ));
    }

CAtMessage::~CAtMessage()
    {
    C_TRACE (( _L("CAtMessage::~CAtMessage()") ));
    if( !iRequest.IsNull() )
       {
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
    C_TRACE (( _L("CAtMessage::CancelRequest()") ));
    if( !iRequest.IsNull() )
       {
       C_TRACE (( _L("CAtMessage::CancelRequest complete iRequest KErrCancel iRequest: 0x%x handle 0x%x"), &iRequest, iRequest.Handle() ));
       iRequest.Complete( KErrCancel );
       }
    }

CModemAtSession* CAtMessage::GetSession() 
    {
    C_TRACE (( _L("CAtMessage::GetSession() 0x%x"), &iSession ));
    return iSession;
    }

TPtr8 CAtMessage::GetBuffer() 
    {
    C_TRACE (( _L("CAtMessage::GetBuffer()")));
    return iMessagePtr;
    }

TUint8 CAtMessage::GetMessageType() 
    {
    C_TRACE (( _L("CAtMessage::GetMessageType()"), iMessageType ));
    return iMessageType;
    }

void CAtMessage::ReceiveATResponse( const TDesC8& aResponse, TBool aComplete ) 
    {
    C_TRACE (( _L("CAtMessage::ReceiveATResponse( aResponse 0x%x, aComplete %d )"), &aResponse, (TInt)aComplete ));
    C_TRACE (( _L("CAtMessage::ReceiveATResponse iMessageType: %d"), iMessageType ));
    if( !iRequest.IsNull() )
        {
        C_TRACE (( _L("CAtMessage::ReceiveATResponse Writing to iRequest: 0x%x handle 0x%x"), &iRequest, iRequest.Handle() ));
        TInt err = iRequest.Write( KATModemResponseArgumentIndex, aResponse, iRequest.GetDesLength( KATModemResponseArgumentIndex ));
        TRACE_ASSERT( err == KErrNone );
        if( aComplete )
            {
            C_TRACE (( _L("CAtMessage::ReceiveATResponse completing (%d) this: 0x%x iRequest: 0x%x handle: 0x%x"), err, this, &iRequest, iRequest.Handle() ));
            iRequest.Complete( err );
            }
        else
            {
            C_TRACE (( _L("CAtMessage::ReceiveATResponse not completing intermediate data yet(%d)"), err ));
            }
        }
    else
        {
        TRACE_ASSERT( 0 );
        }
    }

