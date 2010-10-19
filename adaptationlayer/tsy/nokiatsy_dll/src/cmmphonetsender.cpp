/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <tisi.h>
#include "cmmphonetsender.h"
#include "cmmphonetreceiver.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "tsylogger.h" // for logging purposes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonetsenderTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
#ifdef OST_TRACE_COMPILER_IN_USE
const TUint8 KBufferForOSTTracing = 100;
#endif
#ifdef TF_LOGGING_ENABLED
const TUint8 KBufferForTFLogging = 130;
#endif

// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
    // None

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TFLOG_PRINT_ISIMESSAGE
// local function for priting ISI message contents
// -----------------------------------------------------------------------------
//
#if defined (TF_LOGGING_ENABLED) || defined (OST_TRACE_COMPILER_IN_USE)
static void TFLOG_PRINT_ISIMESSAGE( const TDesC8& aBuf )
    {
    _LIT( KTraceSpace,           " " );

    // Get the length of the ISI message
    TInt length = aBuf.Length();

#ifdef TF_LOGGING_ENABLED
    TBuf<KBufferForTFLogging> msg;
    _LIT( KTracePhonetSender,    "TSY:Phonet Sender: [ " );
    _LIT( KTraceBracketClose,    "]" );

    msg.Append( KTracePhonetSender );

    if ( 250 < length )
        {
        // Print only 250 first bytes
        length = 250;
        }

    for ( TInt i = 0; i < length; i++ )
        {
        msg.AppendNumFixedWidthUC( aBuf[i], EHex, 2 );
        msg.Append( KTraceSpace );
        if ( KLogSizeOfBuffer-10 < msg.Length() )
            {
            if ( i == ( length - 1 ) )
                {
                // Last number, append "]" character
                msg.Append( KTraceBracketClose );
                }

            TFLOGTEXT( msg );
            msg.SetLength( 0 );
            }
        }

    if ( 0 < msg.Length() )
        {
        // Buffer contains data. Print also last line
        msg.Append( KTraceBracketClose );
        TFLOGTEXT( msg );
        }
#endif // TF_LOGGING_ENABLED

    // Ost tracing
#ifdef OST_TRACE_COMPILER_IN_USE
    TBuf8<KBufferForOSTTracing> trace_msg;
    TUint8 counter = 1;
    TBool firstTime = ETrue;
    for ( TInt i = 0; i < length; i++ )
        {
        trace_msg.AppendNumFixedWidthUC( aBuf[i], EHex, 2 );
        trace_msg.Append( KTraceSpace );
        
        if ( ( counter > 24 ) ||     // 25 bytes / line
           ( ( i + 1 ) == length ) ) // All bytes collected
          {
          if ( ( firstTime ) &&
               ( ( i + 1 ) == length ) ) // All bytes collected and traced
              {
              firstTime = EFalse;
OstTraceExt1( TRACE_NORMAL,  _TFLOG_PRINT_ISIMESSAGE_TD, "Phonet Sender: [ %s]", trace_msg );
                trace_msg.SetLength( 0 );
              }
            else if ( firstTime ) // 1st line of the trace
              {
              firstTime = EFalse;
OstTraceExt1( TRACE_NORMAL,  DUP1__TFLOG_PRINT_ISIMESSAGE_TD, "Phonet Sender: [ %s", trace_msg );
                trace_msg.SetLength( 0 );
              }
            else if ( ( i + 1 ) == length ) // The last line
              {
OstTraceExt1( TRACE_NORMAL,  DUP4__TFLOG_PRINT_ISIMESSAGE_TD, "Phonet Sender:   %s]", trace_msg );
                trace_msg.SetLength( 0 );
              }
            else // just print bytes
              {
OstTraceExt1( TRACE_NORMAL,  DUP5__TFLOG_PRINT_ISIMESSAGE_TD, "Phonet Sender:   %s", trace_msg );
                trace_msg.SetLength( 0 );
              }
            counter = 0;
          }
        counter++;
        }
#endif // OST_TRACE_COMPILER_IN_USE
     }

#else // TF_LOGGING_ENABLED || OST_TRACE_COMPILER_IN_USE

#define TFLOG_PRINT_ISIMESSAGE(x)

#endif // TF_LOGGING_ENABLED || OST_TRACE_COMPILER_IN_USE

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmPhoNetSender::CMmPhoNetSender
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMmPhoNetSender::CMmPhoNetSender()
        :iSendBuffer( NULL ),
        iSendBufferPtr( 0,0 )
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::CMmPhoNetSender");
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_CMMPHONETSENDER_TD, "CMmPhoNetSender::CMmPhoNetSender" );
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::~CMmPhoNetSender
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmPhoNetSender::~CMmPhoNetSender()
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::~CMmPhoNetSender");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONETSENDER_CMMPHONETSENDER_TD, "CMmPhoNetSender::~CMmPhoNetSender" );
    delete iSendBuffer;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmPhoNetSender* CMmPhoNetSender::NewL
        (
        RIscApi* aPn // Pointer to the IscApi
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::NewL");
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_NEWL_TD, "CMmPhoNetSender::NewL" );

    CMmPhoNetSender* phoNetSender = NULL;

    if ( NULL != aPn )
        {
        phoNetSender = new( ELeave ) CMmPhoNetSender();
        CleanupStack::PushL( phoNetSender );
        phoNetSender->iPhoNet = aPn;
        phoNetSender->ConstructL();
        CleanupStack::Pop( phoNetSender );
        }

    return phoNetSender;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmPhoNetSender::ConstructL()
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::ConstructL");
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_CONSTRUCTL_TD, "CMmPhoNetSender::ConstructL" );
    iSendBuffer = HBufC8::NewL( KMaxSendBufferSize );
    iSendBufferPtr.Set( iSendBuffer->Des() );

    TFLOGSTRING2("TSY: iSendBuffer len=%d", iSendBuffer->Length());
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONETSENDER_CONSTRUCTL_TD, "CMmPhoNetSender::ConstructL;iSendBuffer->Length=%d", iSendBuffer->Length() );
    TFLOGSTRING3("TSY: iSendBufferPtr len=%d maxlen=%d",
        iSendBufferPtr.Length(),
        iSendBufferPtr.MaxLength());
OstTrace1( TRACE_NORMAL,  DUP2_CMMPHONETSENDER_CONSTRUCTL_TD, "CMmPhoNetSender::ConstructL;iSendBufferPtr.MaxLength=%d", iSendBufferPtr.MaxLength() );
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::SendBufferDes
// Description: Returns pointer to buffer for ISI message construction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDes8& CMmPhoNetSender::SendBufferDes()
    {
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_SENDBUFFERDES_TD, "CMmPhoNetSender::SendBufferDes" );
    return iSendBufferPtr;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::Send
// Sends a message to the PhoNet if PhonetSender is not blocked
// for example by the object of type CMmPhoneTsy (out of memory situation).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetSender::Send
        (
        const TDesC8& aMsg // ISI message to be sent to Phonet
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::Send");
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_SEND_TD, "CMmPhoNetSender::Send" );

    TInt ret( KErrServerBusy );

    ret = iPhoNet->Send( aMsg );

    // If sending failed
    if ( KErrNone != ret )
        {
        // Return general error value to client hiding the real Phonet
        // problem. There is no need to identify the problem more clearly
        // above MM API.
        ret = KErrGeneral;
        }

    TFLOG_PRINT_ISIMESSAGE( aMsg );

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::Send
// Send ISI message, given resource, transaction message id and contents
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetSender::Send
        (
        TInt aResource, // ISI resource (server)
        TInt aTransactionId, // ISI transaction ID
        TInt aMessageId, // ISI message ID
        const TDesC8& aData // Data part of ISI message
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::Send (resource+transaction+data)");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONETSENDER_SEND_TD, "CMmPhoNetSender::Send (resource+transaction+data)" );

    // We shall use send buffer
    TDes8& buffer( SendBufferDes() );
    buffer.Zero();
    const TInt header_size( 10 ); // Header + transaction id + message id
    TIsiSend msg( buffer, header_size + aData.Length() );
    msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, aResource );
    msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, aMessageId );
    msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, aTransactionId );
    msg.CopyData( header_size, aData );

    // Add padding
    while ( buffer.Length() % 4 )
        {
        buffer.Append( 0x00 );
        }

    return Send( msg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::EventSend
//
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetSender::EventSend
        (
        const TDesC8& aEvent
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::EventSend");
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_EVENTSEND_TD, "CMmPhoNetSender::EventSend" );

    return iPhoNet->CustomFunction( EIscNokiaEventSend, (TDes8*)&aEvent );
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::SubscribeEvents
//
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetSender::SubscribeEvents
        (
        const TDesC8& aEvents // Events to be subscribed
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::SubscribeEvents");
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_SUBSCRIBEEVENTS_TD, "CMmPhoNetSender::SubscribeEvents" );

    return iPhoNet->CustomFunction( EIscNokiaEventSubscribe, (TDes8*)&aEvents );
    }

// -----------------------------------------------------------------------------
// CMmPhoNetSender::GetChannelInfo
//
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetSender::GetChannelInfo
        (
        const TUint16 aChannel,
        TDes8& aInfo
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetSender::GetChannelInfo");
OstTrace0( TRACE_NORMAL,  CMMPHONETSENDER_GETCHANNELINFO_TD, "CMmPhoNetSender::GetChannelInfo" );

    return iPhoNet->GetChannelInfo( aChannel, aInfo );
    }

// ==================== OTHER EXPORTED FUNCTIONS ===============================
    // None

// End of File



