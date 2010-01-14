/*
* ==============================================================================
*  Name:           ssm_emergencycallrf_adaptation.cpp
*  Part of:        SSMA EmergencyCallRfAdaptation
*  Interface:      N/A
*  Description:    Declaration of CEmergencyCallRfAdaptation class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:38:36 2009 %
*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Implementation of ssm_emergencycallrf_adaptation
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ssm_emergencycallrf_adaptationtraces.h"
#endif // OST_TRACE_COMPILER_IN_USE
#include <ssm/startupadaptationcommands.h>
#include <iscnokiadefinitions.h>
#include <mceisi.h>
#include <tisi.h>
#include <pn_const.h>
#include "ssm_emergencycallrf_adaptation.h"
#include "ssm_emergencycallrf_trcommand.h"
#include "sa_common_isc_service.h"
#include "sa_common_trace.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTIONS PROTOTYPES

// CONSTANTS
#define MIN_ISI_MESSAGE_SIZE    36

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

/**
 Function to create new Emergency Call RF Adaptation Plugin.

 @return	new instance of MEmergencyCallRfAdaptation for Emergency Call RF Adaptations.

 @internalComponent
 @released
*/
EXPORT_C MEmergencyCallRfAdaptation* CreateEmergencyCallRfAdaptationL()
    {
    CEmergencyCallRfAdaptation* emergencyCallRfAdaptation = CEmergencyCallRfAdaptation::NewL();
    return ( static_cast<MEmergencyCallRfAdaptation*> ( emergencyCallRfAdaptation ) );
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::NewL
// -----------------------------------------------------------------------------
//
CEmergencyCallRfAdaptation* CEmergencyCallRfAdaptation::NewL()
    {
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_NEWL_1,
               "SSMA # CEmergencyCallRfAdaptation::NewL" );
    INTERNAL_TRACE( _L( "SSMA # CEmergencyCallRfAdaptation::NewL" ) );

    CEmergencyCallRfAdaptation* self = new(ELeave) CEmergencyCallRfAdaptation;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTrace1( TRACE_API, CEMERGENCYCALLRFADAPTATION_NEWL_2,
               "SSMA # CEmergencyCallRfAdaptation::NewL - return 0x%08x", self );
    API_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::NewL - return 0x%08x"), self ));
    return self;
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation
// -----------------------------------------------------------------------------
//
CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation()
    {
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_CEMERGENCYCALLRFADAPTATION_D1,
               "SSMA # CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation" );
    INTERNAL_TRACE( _L( "SSMA # CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation" ) );

    /* Delete the list of ongoing transactions */
    iTransactionPtr.ResetAndDestroy();  

    /* Delete iCSACommonIscServicePtr */
    if ( iCSACommonIscServicePtr )
        {
        OstTrace0( TRACE_API, CEMERGENCYCALLRFADAPTATION_CEMERGENCYCALLRFADAPTATION_D2,
                   "SSMA - CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation - Delete iCSACommonIscServicePtr" );
        API_TRACE((_L( "SSMA - CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation - Delete iCSACommonIscServicePtr" )));

        delete iCSACommonIscServicePtr;
        iCSACommonIscServicePtr = NULL;
        }

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_CEMERGENCYCALLRFADAPTATION_D3,
              "SSMA # CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation - return void" );
   INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation - return void")));

    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation
// -----------------------------------------------------------------------------
//
CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation()
    : iTransId(0)
    {
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_CEMERGENCYCALLRFADAPTATION_1,
        "SSMA # CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation()" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation()")));

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_CEMERGENCYCALLRFADAPTATION_2, 
        "SSMA # CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation - return void")));
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::ConstructL
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_CONSTRUCTL_1,
               "SSMA # CEmergencyCallRfAdaptation::ConstructL()" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ConstructL()")));

    // Minimum isi message size
    TUint8 isiMessageSize( MIN_ISI_MESSAGE_SIZE );

    iCSACommonIscServicePtr = CSACommonIscService::NewL( *this,
                                                         EIscNokiaReserved8, 
                                                         /*(TUint16) KEmergencyCallRfAdaptationDllTypeUidValue,*/
                                                         NULL, // we are not interested in events, so we send NULL
                                                         isiMessageSize );

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_CONSTRUCTL_2,
               "SSMA # CEmergencyCallRfAdaptation::ConstructL - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ConstructL - return void")));
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall
// Request RF activation so that an emergency call can be made.
// @param aStatus to complete when the operation has finished
// @see TRequestStatus
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall( TRequestStatus& aStatus )
    {
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_ACTIVATERFONFOREMERGENCYCALL_1,
               "SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall" );
    INTERNAL_TRACE( _L( "SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall" ) );

    iCancelCommand= EFalse;
    aStatus = KRequestPending;
    iStatusPtr = &aStatus;

    CEmergencyCallRfTrCommand* transactionPtr( NULL );
    TUint8 transId = TransactionId();
    iCommandId = StartupAdaptation::EActivateRfForEmergencyCall;

    transactionPtr = CEmergencyCallRfTrCommand::NewL( *this, transId );

    if ( transactionPtr )
        {
        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_ACTIVATERFONFOREMERGENCYCALL_2,
                   "SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall - Transaction ptr  created" );

        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall - Transaction ptr created"))); 

        // Modifiable pointer to message data buffer
        TPtr8 bufferPtr( NULL, 0 );

        ForwardMessageL( iCommandId, transId, bufferPtr, transactionPtr );
        }
    else
       {
        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_ACTIVATERFONFOREMERGENCYCALL_3,
                   "SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall - transaction ptr not created" );
        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall - transaction ptr not created ")));
        User::RequestComplete( iStatusPtr, KErrNoMemory );
       }

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_ACTIVATERFONFOREMERGENCYCALL_4,
               "SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ActivateRfForEmergencyCall - return void")));

    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall
// Request RF deactivation so that an emergency call can be made.
// @param aStatus to complete when the operation has finished
// @see TRequestStatus
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall( TRequestStatus& aStatus )
    {
    iCancelCommand= EFalse;
    aStatus = KRequestPending;
    iStatusPtr = &aStatus;
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_DEACTIVATERFONFOREMERGENCYCALL_1,
               "SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall" );
    INTERNAL_TRACE( _L( "SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall" ) );

    CEmergencyCallRfTrCommand* transactionPtr( NULL );
    TUint8 transId = TransactionId();
    iCommandId = StartupAdaptation::EDeactivateRfAfterEmergencyCall;

    transactionPtr = CEmergencyCallRfTrCommand::NewL( *this, transId);

    if ( transactionPtr )
        {
        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_DEACTIVATERFONFOREMERGENCYCALL_2,
                   "SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall - Transaction ptr  created" );

        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall - Transaction ptr created"))); 

        // Modifiable pointer to message data buffer
        TPtr8 bufferPtr( NULL, 0 );

        ForwardMessageL( iCommandId, transId, bufferPtr, transactionPtr );
        }
    else
       {
        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_DEACTIVATERFONFOREMERGENCYCALL_3,
                   "SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall - transaction ptr not created" );
        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall - transaction ptr not created ")));
        User::RequestComplete( iStatusPtr, KErrNoMemory );
       } 

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_DEACTIVATERFONFOREMERGENCYCALL_4,
               "SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall - return void")));

    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::Release
// Deletes and frees memory allocated.
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::Release()
    {
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_RELEASE,
               "SSMA # CEmergencyCallRfAdaptation::Release" );
    INTERNAL_TRACE( _L( "SSMA # CEmergencyCallRfAdaptation::Release" ) );
    delete this;
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::Cancel
// Cancel the outstanding request. Reference implementation completes the 
// requests immediately so there is nothing to cancel.
// On a device, Cancel() needs an implementation as the Request might be 
// outstanding and it needs to be cancelled. 
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::Cancel()
    {
    iCancelCommand= ETrue;

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_CANCEL_2,
               "SSMA # CEmergencyCallRfAdaptation::Cancel - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::Cancel - return void")));
	}


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::ForwardMessageL
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::ForwardMessageL( const TInt aCommandId,
                                                  const TUint8 aTransId,
                                                  TDesC8& aData,
                                                  CEmergencyCallRfTrCommand* aNewTransactionPtr )
    {
    OstTraceExt4( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_0,
                  "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL(0x%x,0x%x,0x%x,0x%x)",
                  (TUint)aCommandId, (TUint)aTransId, (TUint)&aData, (TUint)&aNewTransactionPtr );

    INTERNAL_TRACE((_L( "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL(0x%x,0x%x,0x%x,0x%x)"),
                        (TUint)aCommandId, (TUint)aTransId, (TUint)&aData, (TUint)&aNewTransactionPtr ));

    CEmergencyCallRfTrCommand* transactionPtr( NULL );
    TInt32 entryToBeRemoved( KErrNotFound );

    if ( aNewTransactionPtr )
        {
        TInt32 newTrAppendStatus( KErrGeneral );

        transactionPtr = aNewTransactionPtr;

        // If the transaction is already in array, no need to add it
        entryToBeRemoved = iTransactionPtr.Find( transactionPtr );

        // It's a new transaction, thus not assigned to the list yet.
        if ( entryToBeRemoved == KErrNotFound )
            {
            newTrAppendStatus = iTransactionPtr.Append( transactionPtr );

            if ( newTrAppendStatus != KErrNone )
                {
                OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_1,
                           "SSMA - CEmergencyCallRfAdaptation::ForwardMessageL - iTransaction.Append FAILED - status %d.",
                           newTrAppendStatus );
                INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - iTransaction.Append FAILED - status %d."),
                                   newTrAppendStatus));

                delete transactionPtr;
                transactionPtr = NULL;
                User::Leave( newTrAppendStatus );
                }

            // Get the index, needed if the entry needs to be removed later on.
            entryToBeRemoved = iTransactionPtr.Find( transactionPtr );
            }
        OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_2, 
                   "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - entryToBeRemoved %d:",
                   entryToBeRemoved );
        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - entryToBeRemoved %d:"),
                            entryToBeRemoved));

        if ( entryToBeRemoved == KErrNotFound )
            {
            ASSERT_ALWAYS_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - Entry not found")));

            delete transactionPtr;
            transactionPtr = NULL;
            User::Leave( KErrNotFound );
            }

        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_3,
                   "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - New transactionPtr." );
        INTERNAL_TRACE(_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - New transactionPtr."));
        }
    else
        {
        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_4,
                   "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - Search existing transaction from the list." );
        INTERNAL_TRACE(_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - Search existing transaction from the list."));

        // Find an existing transaction if we did not create a new one.
        for ( TUint32 i = 0; i < iTransactionPtr.Count() && transactionPtr == NULL; i++ )
            {
            if ( iTransactionPtr[i]->TransactionId() == aTransId )
                {
                transactionPtr   = iTransactionPtr[i];
                entryToBeRemoved = i;

                OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_5,
                           "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - Transaction found." );
                INTERNAL_TRACE(_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - Transaction found."));
                }
            }
        }

    if ( transactionPtr )
        {
        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_6,
                   "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - forwared to transaction." );
        INTERNAL_TRACE(_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - forwared to transaction."));

        TRAPD( err, transactionPtr->ProcessMessageL( aCommandId, aData ) );

        OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_7,
                  "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - TRAP err: %d: ",
                   err );
        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - TRAP err: %d: "),
                            err));

        TUint32 transactionState = transactionPtr->TransactionState();

        // Transaction completed or failed
        if ( transactionState == EMERGENCYCALLRF_TR_COMPLETED ||
             transactionState == EMERGENCYCALLRF_TR_FAILED ||
             err != KErrNone )
            {

            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_8, 
                       "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - delete transaction." );
            INTERNAL_TRACE(_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - delete transaction."));

            delete transactionPtr;
            transactionPtr = NULL;

            iTransactionPtr.Remove( entryToBeRemoved );

            User::LeaveIfError( err );

            OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_11,
                       "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - Transaction removed." );
            INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - Transaction removed.")));
            }
        }
    else
        {
        ASSERT_ALWAYS_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - unknown message")));
        User::Leave( KErrGeneral );
        }

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_FORWARDMESSAGEL_12,
                   "SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ForwardMessageL - return void")));
    }

// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::SendIsiMessageL
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::SendIsiMessageL( TDesC8& aMessageData )
    {
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_SENDISIMESSAGEL_1,
        "SSMA # CEmergencyCallRfAdaptation::SendIsiMessageL(0x%x)",
        (TUint)&aMessageData );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::SendIsiMessageL(0x%x)"),
        (TUint)&aMessageData));

    // Allocate and initialize space for ISI message
    TUint32 length = ISI_HEADER_SIZE + aMessageData.Length();
    HBufC8* isiMsgData = HBufC8::NewL( length );

    CleanupStack::PushL( isiMsgData );

    TPtr8 isiMsgDataPtr = isiMsgData->Des();

    // Create an ISI message
    TIsiSend isiMsg( isiMsgDataPtr, length );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, PN_DEV_MODEM );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_MCE );
    isiMsg.CopyData( ISI_HEADER_OFFSET_TRANSID, aMessageData );
    isiMsg.Complete();

    // Send ISI message
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_SENDISIMESSAGEL_2,
        "SSMA # CEmergencyCallRfAdaptation::SendIsiMessageL - iCSACommonIscService->SendMessageL(0x%x)",
        (TUint)&aMessageData );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::SendIsiMessageL - iCSACommonIscService->SendIsiMessageL(0x%x)"),
        (TUint)&aMessageData));
    iCSACommonIscServicePtr->SendMessageL( isiMsgDataPtr );

    // Delete temporary buffers
    CleanupStack::PopAndDestroy( isiMsgData );

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_SENDISIMESSAGEL_3,
        "SSMA # CEmergencyCallRfAdaptation::SendIsiMessageL - return void" );
    INTERNAL_TRACE( ( _L( "SSMA # CEmergencyCallRfAdaptation::SendIsiMessageL - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::ReceiveIsiMessageL 
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::ReceiveIsiMessageL( TDesC8& aData )
    {
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_RECEIVEISIMESSAGEL_1,
               "SSMA # CEmergencyCallRfAdaptation::ReceiveIsiMessageL(0x%x)",
               (TUint)&aData );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ReceiveIsiMessageL(0x%x)"),
                       (TUint)&aData ));

    TIsiReceiveC isiMsg( aData );
    TUint8 resourceId( isiMsg.Get8bit( ISI_HEADER_OFFSET_RESOURCEID ) );
    TUint8 transId( isiMsg.Get8bit( ISI_HEADER_OFFSET_TRANSID ) );
    TUint8 messageId( isiMsg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );

    OstTraceExt3( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_RECEIVEISIMESSAGEL_2,
               "SSMA - CEmergencyCallRfAdaptation::ReceiveIsiMessageL - ISI message Id(0x%x, 0x%x, 0x%x)",
               resourceId, transId, messageId ); 
    INTERNAL_TRACE((_L("SSMA - CEmergencyCallRfAdaptation::ReceiveIsiMessageL - ISI message Id(0x%x, 0x%x, 0x%x)"),
                        resourceId, transId, messageId ));

    if ( resourceId == PN_MODEM_MCE )
        {
        switch ( messageId )
            {
            case MCE_RF_STATE_RESP:
                {
                OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_RECEIVEISIMESSAGEL_3,
                           "SSMA # CEmergencyCallRfAdaptation::ReceiveIsiMessageL - MCE_RF_STATE_RESP" );
                INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ReceiveIsiMessageL - MCE_RF_STATE_RESP")));

                CEmergencyCallRfTrCommand* transactionPtr( NULL );

                ForwardMessageL( messageId, transId, aData, transactionPtr );
                break;
                }
            default:
                {
                OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_RECEIVEISIMESSAGEL_4,
                           "SSMA - CEmergencyCallRfAdaptation::ReceiveIsiMessageL - Wrong msg Id from Cellmo - MessageId() 0x%x ",
                           messageId );
                INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ReceiveIsiMessageL - Wrong msg Id from Cellmo - MessageId() 0x%x "),
                                   messageId));
                break;
                }
            }
        }
    else
        {
        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_RECEIVEISIMESSAGEL_5,
                   "SSMA - CEmergencyCallRfAdaptation::ReceiveIsiMessageL -resource id NOT SUPPORTED! " );
        INTERNAL_TRACE( _L( "SSMA # CEmergencyCallRfAdaptation::ReceiveIsiMessageL -resource id NOT SUPPORTED! " ) );
        User::Leave( KErrGeneral );
        }

        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_RECEIVEISIMESSAGEL_6,
                   "SSMA - CEmergencyCallRfAdaptation::ReceiveIsiMessageL -return void " );
        INTERNAL_TRACE( _L( "SSMA # CEmergencyCallRfAdaptation::ReceiveIsiMessageL -return void " ) );

    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::ProcessResponseL
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::ProcessResponseL( TInt aErrCode )
    {
    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSRESPONSEL_1,
                  "SSMA # CEmergencyCallRfAdaptation::ProcessResponseL(0x%d)",
                  (TUint)aErrCode );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessResponseL(0x%x)"),
                       aErrCode ));

    if ( iCancelCommand )
        {
        if ( aErrCode == KErrNone )
            {
            switch ( iCommandId )
                {
                case StartupAdaptation::EActivateRfForEmergencyCall:
                    {
                    CEmergencyCallRfTrCommand* transactionPtr( NULL );
                    TUint8 transId = TransactionId();
                    transactionPtr = CEmergencyCallRfTrCommand::NewL( *this, transId);

                    if ( transactionPtr )
                        {
                        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSRESPONSEL_2,
                                   "SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - Transaction ptr  created" );

                        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - Transaction ptr created"))); 

                        // Modifiable pointer to message data buffer
                        TPtr8 iBufferPtr( NULL, 0 );

                        ForwardMessageL( StartupAdaptation::EDeactivateRfAfterEmergencyCall, transId, iBufferPtr, transactionPtr );
                        }
                    else
                        {
                        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSRESPONSEL_3,
                                   "SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - transaction ptr not created" );
                        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - transaction ptr not created ")));
                        User::RequestComplete( iStatusPtr, KErrCompletion);
                        }
                    iCommandId = KErrCancel;
                    }
                    break;

                case StartupAdaptation::EDeactivateRfAfterEmergencyCall:
                    {
                    CEmergencyCallRfTrCommand* transactionPtr( NULL );
                    TUint8 transId = TransactionId();
                    transactionPtr = CEmergencyCallRfTrCommand::NewL( *this, transId);

                    if ( transactionPtr )
                        {
                        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSRESPONSEL_4,
                                   "SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - Transaction ptr  created" );

                        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - Transaction ptr created"))); 

                        // Modifiable pointer to message data buffer
                        TPtr8 iBufferPtr( NULL, 0 );

                        ForwardMessageL( StartupAdaptation::EActivateRfForEmergencyCall, transId, iBufferPtr, transactionPtr );
                        }
                    else
                        {
                        OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSRESPONSEL_5,
                                   "SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - transaction ptr not created" );
                        INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - transaction ptr not created")));
                        User::RequestComplete( iStatusPtr, KErrCompletion);
                        }
                    iCommandId = KErrCancel;
                    }
                    break;

                case KErrCancel:
                    {
                    User::RequestComplete( iStatusPtr, KErrCancel);
                    }
                    break;

                default:
                    {
                    OstTrace1( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSRESPONSEL_6, 
                               "SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - Unknown CommandId() 0x%x ",
                                iCommandId );
                    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - Unknown CommandId() 0x%x "),
                                        iCommandId ));
                    }
                    break;
                }
            }
        else
            {
            User::RequestComplete( iStatusPtr, KErrCancel );
            }
        }
    else
        {
        User::RequestComplete( iStatusPtr, aErrCode );
        }
    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSRESPONSEL_7, 
               "SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - return void" );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessResponseL - return void")));
    }


// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::ProcessConnectionClosedL
// -----------------------------------------------------------------------------
//
void CEmergencyCallRfAdaptation::ProcessConnectionClosedL( TInt aErrorCode )
    {
    OstTrace1( TRACE_NORMAL,CEMERGENCYCALLRFADAPTATION_PROCESSCONNECTIONCLOSEDL_1,
               "SSMA # CEmergencyCallRfAdaptation::ProcessConnectionClosedL(0x%x)",
               aErrorCode );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::ProcessConnectionClosedL(0x%x)"),
                       aErrorCode ));

    OstTrace0( TRACE_NORMAL, CEMERGENCYCALLRFADAPTATION_PROCESSCONNECTIONCLOSEDL_2,
               "SSMA # CEmergencyCallRfAdaptation::ProcessConnectionClosedL - return void" );
    INTERNAL_TRACE((_L( "SSMA # CEmergencyCallRfAdaptation::ProcessConnectionClosedL - return void" )));
    }


//
// -----------------------------------------------------------------------------
// CEmergencyCallRfAdaptation::TransactionId
// -----------------------------------------------------------------------------
//
TInt CEmergencyCallRfAdaptation::TransactionId()
    { 
    // check for 255
    if ( (++iTransId) >= 255 )
        {
            iTransId = 0; 
        }
		
    OstTrace1( TRACE_NORMAL,CEMERGENCYCALLRFADAPTATION_TRANSACTIONID_1,
               "SSMA # CEmergencyCallRfAdaptation::TransactionId(0x%x)",
               iTransId );
    INTERNAL_TRACE((_L("SSMA # CEmergencyCallRfAdaptation::TransactionId(0x%x)"),
                       iTransId ));
    return iTransId;
    }

// End of file

