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
#include "cmmphonetreceiver.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>

// For logging purposes
#include "tsylogger.h"

// ISA CellMo ATK server
#include <pn_const.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonetreceiverTraces.h"
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
    //None

// ======================= LOCAL FUNCTIONS =====================================

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
    _LIT( KTracePhonetReceiver,  "TSY:Phonet Receiver:  [ " );
    _LIT( KTraceBracketClose,    "]" );

    msg.Append( KTracePhonetReceiver );

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
OstTraceExt1( TRACE_NORMAL, DUP2__TFLOG_PRINT_ISIMESSAGE, "Phonet Receiver: [ %s]", trace_msg );
                trace_msg.SetLength( 0 );
              }
            else if ( firstTime ) // 1st line of the trace
              {
              firstTime = EFalse;
OstTraceExt1( TRACE_NORMAL, DUP3__TFLOG_PRINT_ISIMESSAGE, "Phonet Receiver: [ %s", trace_msg );
                trace_msg.SetLength( 0 );
              }
            else if ( ( i + 1 ) == length ) // The last line
              {
OstTraceExt1( TRACE_NORMAL, DUP4_TFLOG_PRINT_ISIMESSAGE, "Phonet Receiver:   %s]", trace_msg );
                trace_msg.SetLength( 0 );
              }
            else // just print bytes
              {
OstTraceExt1( TRACE_NORMAL, DUP5_TFLOG_PRINT_ISIMESSAGE, "Phonet Receiver:   %s", trace_msg );
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

// ==================== MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::CMmPhoNetReceiver
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmPhoNetReceiver::CMmPhoNetReceiver()
        : CActive( 0 ),
        iMessageReceiver( NULL ),
        iMessageBuffer( NULL ),
        iMessageBufferPtr( 0,0 ),
        iNeededBufferLength( 0 ),
        iSatMessHandler( NULL ),
        iSatMessageBuffer( NULL )
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::CMmPhoNetReceiver");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_CMMPHONETRECEIVER, "CMmPhoNetReceiver::CMmPhoNetReceiver" );
    //None
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::~CMmPhoNetReceiver
// Destructor
// -----------------------------------------------------------------------------
//
CMmPhoNetReceiver::~CMmPhoNetReceiver()
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::~CMmPhoNetReceiver");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_CMMPHONETRECEIVER, "CMmPhoNetReceiver::~CMmPhoNetReceiver" );
    // Shutdown the active scheduler correctly.
    Cancel();
    Deque();

    // Closes the array and frees all memory allocated to the array.
    iMsgReceivers.Close();

    delete iMessageBuffer;
    delete iSatMessageBuffer;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::NewL
// Two-phased constructor.
// Creates a new CMmPhoNetReceiver object instance. If either
// mmPhone or phonet received as parameters is NULL, CMmPhoNetReceiver will
// not be created and NULL will be returned.
// -----------------------------------------------------------------------------
//
CMmPhoNetReceiver* CMmPhoNetReceiver::NewL
        (
        RIscApi* aPn // Phonet
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::NewL");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_NEWL, "CMmPhoNetReceiver::NewL" );

    CMmPhoNetReceiver* phoNetReceiver = NULL;

    if ( NULL != aPn )
        {
        phoNetReceiver = new ( ELeave ) CMmPhoNetReceiver();

        CleanupStack::PushL( phoNetReceiver );
        phoNetReceiver->ConstructL();
        CleanupStack::Pop( phoNetReceiver );

        // Set pointer to the Phonet
        phoNetReceiver->iPhoNet = aPn;

        // Adds the specified active object to the current active scheduler
        CActiveScheduler::Add( phoNetReceiver );
        }

    return phoNetReceiver;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CMmPhoNetReceiver::ConstructL()
    {
    // Allocate memory for incoming message.
    TFLOGSTRING("TSY: CMmPhoNetReceiver::ConstructL");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_CONSTRUCTL, "CMmPhoNetReceiver::ConstructL" );

    iMessageBuffer = HBufC8::NewL( KDefaultReceiveBufferSize );
    iMessageBufferPtr.Set( iMessageBuffer->Des() );

    TFLOGSTRING2("TSY: iMessageBuffer len=%d", iMessageBuffer->Length());
OstTrace1( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_CONSTRUCTL, "CMmPhoNetReceiver::ConstructL;iMessageBuffer->Length=%d", iMessageBuffer->Length() );
    TFLOGSTRING3("TSY: iMessageBufferPtr len=%d maxlen=%d",
        iMessageBufferPtr.Length(),
        iMessageBufferPtr.MaxLength());
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONETRECEIVER_CONSTRUCTL, "CMmPhoNetReceiver::ConstructL;iMessageBufferPtr.MaxLength=%d", iMessageBufferPtr.MaxLength() );
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::DoCancel
// Cancels an active receiver request.
// -----------------------------------------------------------------------------
//
void CMmPhoNetReceiver::DoCancel()
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::DoCancel");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_DOCANCEL, "CMmPhoNetReceiver::DoCancel" );
    // Cancel message receiving from Phonet
    iPhoNet->ReceiveCancel();
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::RunError
// Called when RunL leaves.
// Calls message handler's HandleError method,
// iMessageReceiver variable carries pointer to message handler.
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetReceiver::RunError
        (
        TInt aError // Error from RunL or active scheduler
        )
    {
    TFLOGSTRING2("TSY: CMmPhoNetReceiver::RunError - Error code: %d", aError);
OstTrace1( TRACE_NORMAL, CMMPHONETRECEIVER_RUNERROR, "CMmPhoNetReceiver::RunError;aError=%d", aError );
    if ( iMessageReceiver )
        {
        iMessageReceiver->HandleError( TIsiReceiveC( iMessageBufferPtr ),
            aError );
        iMessageReceiver = NULL;
        }

    if ( NULL == iMessageBuffer )
        {
        TFLOGSTRING("TSY: CMmPhoNetReceiver::RunError\
            -- receive buffer resize failed!");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_RUNERROR, "CMmPhoNetReceiver::RunError, receive buffer resize failed!" );
        }

    // Restart message receiving
    TRAPD( result, ReceiveL() );
    return result;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::RunL
// Handles messages received from PhoNet when receiver request
// is completed by PhoNet
// -----------------------------------------------------------------------------
//
void CMmPhoNetReceiver::RunL()
    {
    TFLOGSTRING3("TSY: CMmPhoNetReceiver::RunL\
        -- iMessageBufferPtr len=%d maxlen=%d",
        iMessageBufferPtr.Length(),
        iMessageBufferPtr.MaxLength());
OstTraceExt2( TRACE_NORMAL, CMMPHONETRECEIVER_RUNL, "CMmPhoNetReceiver::RunL;iMessageBufferPtr.Length=%d;iMessageBufferPtr.MaxLength=%d", iMessageBufferPtr.Length(), iMessageBufferPtr.MaxLength() );

    if ( KErrNone != iStatus.Int() && KErrOverflow != iStatus.Int() )
        {
        // If message was too large, allocate more memory and re-request
        // message.
        if ( KErrNoMemory == iStatus.Int() )
            {
            TFLOGSTRING2("TSY: CMmPhoNetReceiver::RunL\
                -- Large message received, resizing buffer to %d",
                iNeededBufferLength);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_RUNL, "CMmPhoNetReceiver::RunL;iNeededBufferLength=%hu", iNeededBufferLength );
            // Request message again
            ReceiveL( iNeededBufferLength );
            }
        else
            {
            // For all other error cases, just start waiting for next message
            TFLOGSTRING2("CMmPhoNetReceiver::Receive - iStatus = %d",
                iStatus.Int());
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONETRECEIVER_RUNL, "CMmPhoNetReceiver::RunL;iStatus.Int=%d", iStatus.Int() );
            ReceiveL();
            }
        }
    else
        {
        // Print message to log
        TFLOG_PRINT_ISIMESSAGE( iMessageBufferPtr );

        // Dispatch message to receivers
        DispatchL( TIsiReceiveC( iMessageBufferPtr ) );

        // Start waiting for next message
        ReceiveL();
        }
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::RegisterL
// Register messagereceiver to phonetreceiver.
// -1 can be used as aMessageId to mean all messages for resource
// -----------------------------------------------------------------------------
//
void CMmPhoNetReceiver::RegisterL
        (
        MMmMessageReceiver* aReceiver,
        TInt aResource,
        TInt aMessageId
        )
    {
    TFLOGSTRING4("TSY: CMmPhoNetReceiver::RegisterL\
        -- receiver=0x%x resource=0x%x msgid=0x%x",
        (TInt)aReceiver,
        aResource,
        aMessageId);
OstTraceExt3( TRACE_NORMAL, CMMPHONETRECEIVER_REGISTERL, "CMmPhoNetReceiver::RegisterL;aReceiver=%x;aResource=%x;aMessageId=%d", (TInt)aReceiver, aResource, aMessageId );

    TInt resourceIndex = -1;

    // Check for double registrations.
    for ( TInt i = 0; i < iMsgReceivers.Count(); i++ )
        {
        const TMessageReceiverInfo& ri = iMsgReceivers[i];

        if ( ri.iResource == aResource )
            {
            // Store index where new registration entry is later to be stored
            resourceIndex = i;

            if ( ( ri.iReceiver == aReceiver ) &&
                ( ri.iMessageId == aMessageId ||
                -1 == ri.iMessageId ||
                -1 == aMessageId ) )
                {
                // We want to avoid situations where the same handler receivers
                // a message twice..
                TFLOGSTRING("TSY: CMmPhoNetReceiver::RegisterL\
                    -- Already registered");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_REGISTERL, "CMmPhoNetReceiver::RegisterL, ASSERT Already registered" );
                TF_ASSERT_NOT_REACHED();
                return;
                }
            }
        }

    // Register
    TMessageReceiverInfo info;
    info.iReceiver = aReceiver;
    info.iResource = aResource;
    info.iMessageId = aMessageId;

    // Because of binary search in DispatchL method we have to keep objects in
    // the order. For example all PN_CALL requests are located in sequential
    // locations.
    if ( -1 == resourceIndex )
        {
        // This is registration for a new server
        TInt i = iMsgReceivers.Count();

        for ( ; i > 0; i-- )
            {
            const TMessageReceiverInfo& ri = iMsgReceivers[i-1];
            if ( ri.iResource < aResource )
                {
                resourceIndex = i;
                break;
                }
            }

        if ( -1 == resourceIndex )
            {
            // Insert new registration at the start of array
            resourceIndex = 0;
            }
        }

    iMsgReceivers.InsertL( info, resourceIndex );
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::DispatchL
// Get received message from phonet and dispatch it
// to all registered receivers. Cache SAT messages if SAT is not yet initialized
// Binary search is used to find first message handler registered to receive
// the message.
// -----------------------------------------------------------------------------
//
void CMmPhoNetReceiver::DispatchL
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::DispatchL");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_DISPATCHL, "CMmPhoNetReceiver::DispatchL" );

    // Get resource and message id
    TInt resource( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TInt messageId( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    TFLOGSTRING3("TSY: CMmPhoNetReceiver::DispatchL - resource: 0x%x, id: 0x%x",
        resource,
        messageId);
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_DISPATCHL, "CMmPhoNetReceiver::DispatchL;resource=%d;messageId=%x", resource, messageId );

    // Get lowest request index using binary search
    TInt highIndex = iMsgReceivers.Count();
    TInt lowIndex = -1;
    TInt probe;
    while ( highIndex - lowIndex > 1 )
        {
        probe = ( highIndex + lowIndex ) / 2;
        const TMessageReceiverInfo& ri = iMsgReceivers[probe];
        if ( ri.iResource < resource )
            {
            lowIndex = probe;
            }
        else
            {
            highIndex = probe;
            }
        }

    // Send ISI message to message handlers
    for ( ; highIndex < iMsgReceivers.Count(); highIndex++ )
        {
        const TMessageReceiverInfo& recInfo = iMsgReceivers[highIndex];
        if ( recInfo.iResource == resource )
            {
            if ( recInfo.iMessageId == messageId || -1 == recInfo.iMessageId )
                {
                iMessageReceiver = recInfo.iReceiver;
                iMessageReceiver->ReceiveMessageL( aIsiMessage );
                }
            }
        else
            {
            // Stop searching
            break;
            }
        }

    // Should be set null here in case SAT receivemessage leaves
    iMessageReceiver = NULL;

    if( iSatMessHandler )
        {
        // Sat message handler takes all messages. So make it fast.
        iSatMessHandler->ReceiveMessageL( aIsiMessage );
        }
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::Receive
// Start waiting for message from Phonet.
// -----------------------------------------------------------------------------
//
void CMmPhoNetReceiver::ReceiveL
        (
        TInt aBufferLength
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::Receive");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_RECEIVEL, "CMmPhoNetReceiver::ReceiveL" );

    // Resize receive buffer size if necessary
    if ( NULL == iMessageBuffer ||
        aBufferLength != iMessageBufferPtr.MaxLength() )
        {
        TFLOGSTRING2("TSY: CMmPhoNetReceiver::Receive - resizing buffer to %d",
            aBufferLength);
OstTrace1( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_RECEIVEL, "CMmPhoNetReceiver::ReceiveL;aBufferLength=%d", aBufferLength );

        // Delete old buffer
        delete iMessageBuffer;
        iMessageBuffer = NULL;

        // Allocate new buffer
        iMessageBuffer = HBufC8::NewL( aBufferLength );
        iMessageBufferPtr.Set( iMessageBuffer->Des() );
        TFLOGSTRING2("TSY: iMessageBuffer len=%d", iMessageBuffer->Length());
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONETRECEIVER_RECEIVEL, "CMmPhoNetReceiver::ReceiveL;iMessageBuffer->Length=%d", iMessageBuffer->Length() );
        }

    // Do asyncronous reqest for message from Phonet
    iMessageBufferPtr.SetLength( 0 );
    iStatus = KRequestPending;
    iPhoNet->Receive( iStatus, iMessageBufferPtr, iNeededBufferLength );
    // Indicate that the active object has issued a
    // request and that it is now outstanding.
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::SetSatMessHandler
// Set SAT messagehandler pointer, called by SIM_ATK_TSY
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetReceiver::SetSatMessHandler
        (
        MMmMessageReceiver* aSatMessHandler // SAT messagehandler
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::SetSatMessHandler");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_SETSATMESSHANDLER, "CMmPhoNetReceiver::SetSatMessHandler" );
    iSatMessHandler = aSatMessHandler;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmPhoNetReceiver::SatReady
// SIM_ATK_TSY requests cached SAT message from NokiaTSY by calling this method
// -----------------------------------------------------------------------------
//
TInt CMmPhoNetReceiver::SatReady
        (
        TUint8 aTypeOfCommandRequested  // Message Id
        )
    {
    TFLOGSTRING("TSY: CMmPhoNetReceiver::SatReady");
OstTrace0( TRACE_NORMAL, CMMPHONETRECEIVER_SATREADY, "CMmPhoNetReceiver::SatReady" );
    TInt ret( KErrNotFound );

    // If there is cached SAT message that
    // matches request type return it to SIM_ATK_TSY
    if ( iSatMessageBuffer )
        {
        const TDesC8& message( *iSatMessageBuffer );

        // Take typeofcommand from TLV
        TUint8 typeOfCommand = ( 0x81 == message[15] ) ? message[20] : message[19];

        TFLOGSTRING3("TSY: CMmPhoNetReceiver::SatReady\
            -- aTypeOfCommandRequested: 0x%x, typeOfCommand: 0x%x",
            aTypeOfCommandRequested,
            typeOfCommand);
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPHONETRECEIVER_SATREADY, "CMmPhoNetReceiver::SatReady;aTypeOfCommandRequested=%hhx;typeOfCommand=%hhx", aTypeOfCommandRequested, typeOfCommand );

        if ( typeOfCommand == aTypeOfCommandRequested )
            {
            TFLOGSTRING("TSY: CMmPhoNetReceiver::SatReady -- sending msg to sat");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONETRECEIVER_SATREADY, "CMmPhoNetReceiver::SatReady, sending msg to sat" );
            // Call receivemessage and catch possible leave.
            TRAP_IGNORE( iSatMessHandler->ReceiveMessageL(
                TIsiReceiveC( message ) ) );
            // Free used buffer
            delete iSatMessageBuffer;
            iSatMessageBuffer = NULL;
            TFLOGSTRING("TSY: CMmPhoNetReceiver::SatReady\
                -- iSatMessageBuffer deleted");
OstTrace0( TRACE_NORMAL, DUP3_CMMPHONETRECEIVER_SATREADY, "CMmPhoNetReceiver::SatReady, iSatMessageBuffer deleted" );
            ret = KErrNone;
            }
        }
    return ret;
    }

// ==================== OTHER EXPORTED FUNCTIONS ===============================
    // None

// End of File
