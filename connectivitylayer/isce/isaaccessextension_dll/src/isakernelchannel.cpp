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



#include <pn_const.h>               // For PN_NO_ROUTING

#include <nk_priv.h>                // For __ASSERT_SYSTEM_LOCK etc..

#include "isakernelchannel.h"
#include "queue.h"                  // For DQueue
#include "iadtrace.h"               // For ASSERT_...
#include "isaaccessextension.h"     // For DIsaAccessExtension
#include "iadnokiadefinitions.h"     // For checking allowed kernel channels.

// Extracting and adding the pipeheader.
#include <phonetisi.h>  // For ISI_HEADER_SIZE
#include <pipeisi.h>    // For PNS_PIPE_DATA_OFFSET_DATA
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "isakernelchannelTraces.h"
#endif
const TInt KPipeDataHeader( ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_DATA );

// CONST
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );
const TInt KFourthParam( 3 );

const TInt KDestroyChannelMsg( 0xff );

// TODO: change this to use UniqueID instead and to extension..
void DISAKernelChannel::CheckDfc()
{
	OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_CHECKDFC_ENTRY, ">DISAKernelChannel::CheckDfc" );

    C_TRACE( ( _T( "DISAKernelChannel::CheckDfc>" ) ) );
    DObject* tempObj = reinterpret_cast<DObject*>( &Kern::CurrentThread() );
    TUint8* buffer = ( TUint8* )Kern::Alloc( 100 );
    TPtr8* bufferPtr = new ( TPtr8 )( buffer, 100 );
    tempObj->Name( *bufferPtr );
    if ( bufferPtr->Compare( KIADExtensionDfc ) != KErrNone )
        {
        for( TInt i( 0 );i < bufferPtr->Length(); ++i )
           {
           C_TRACE( ( _T( "bufferPtr[ %d ] %c" ), i, bufferPtr->Ptr()[ i ] ) );
           }
        ASSERT_RESET_ALWAYS( 0, EIADWrongDFCQueueUsed | EIADFaultIdentifier1 << KFaultIdentifierShift );
        }
    Kern::Free( buffer );
    delete bufferPtr;
    bufferPtr = NULL;
    C_TRACE( ( _T( "DISAKernelChannel::CheckDfc<" ) ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_CHECKDFC_EXIT, "<DISAKernelChannel::CheckDfc" );
    }

#ifdef _DEBUG
#define    ASSERT_DFCTHREAD_INEXT() CheckDfc()
#else
#define    ASSERT_DFCTHREAD_INEXT()
#endif

DISAKernelChannel::DISAKernelChannel(
        // None
        ) :
        iKernelChMsgQue( MsgQDfc,this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADKernelChMsgQueDfcPriori )
    {
    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_ENTRY, ">DISAKernelChannel::DISAKernelChannel" );

    C_TRACE( ( _T( "DISAKernelChannel::DISAKernelChannel ->" ) ) );

    iChannelNumber = KNotInitializedChannel;

    //owned
    iPtrPtrToRxBuf = NULL;
    iPtrPtrToDataRxBuf = NULL;

    iRx = new DQueue( KIADKernelRxQueuSize );
    iDataRx = new DQueue( KIADKernelDataRxQueuSize );
    // NOTE! This now in extension DFC, kernel client must ensure not to write to user add space in this threads context.
    iEmptyRxDfc = new TDfc( EmptyRxDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADKernelEmptyRxQueuePriori );
    iEmptyDataRxDfc = new TDfc( EmptyDataRxDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADKernelEmptyRxQueuePriori );

    ASSERT_RESET_ALWAYS( iRx && iDataRx && iEmptyRxDfc && iEmptyDataRxDfc, EIADMemoryAllocationFailure | EIADFaultIdentifier7 << KFaultIdentifierShift );

    for( TInt i( KErrNone ); i < EIADAsyncLast; ++i )
        {
        iRequests[ i ].iDfcFunction = NULL;
        iRequests[ i ].iRequestStatus = NULL;//*KRequestPending;
        }
    iExtensionApi = DIsaAccessExtension::GetChannel2IADApi();
    ASSERT_RESET_ALWAYS( iExtensionApi, EIADMemoryAllocationFailure | EIADFaultIdentifier8 << KFaultIdentifierShift  );
    iPep = NULL;
    C_TRACE( ( _T( "DISAKernelChannel::DISAKernelChannel calling Receive..." ) ) );
    iKernelChMsgQue.Receive();
    C_TRACE( ( _T( "DISAKernelChannel::DISAKernelChannel <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_EXIT, "<DISAKernelChannel::DISAKernelChannel" );
    }

DISAKernelChannel::~DISAKernelChannel(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DUP1_DISAKERNELCHANNEL_DISAKERNELCHANNEL_ENTRY, ">DISAKernelChannel::~DISAKernelChannel" );

    // Send channel destroyed message to complete with EFalse.
    TThreadMessage& m=Kern::Message();
    m.iValue = KDestroyChannelMsg;
    m.SendReceive( &iKernelChMsgQue );
    C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel 0x%x ->" ), iChannelNumber ) );
    if( iRx )
        {
        C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel iRx" ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_RX, "DISAKernelChannel::~DISAKernelChannel iRx" );
        
        delete iRx;
        iRx =NULL;
        }
    if( iDataRx )
        {
        C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel iDataRx" ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_DATARX, "DISAKernelChannel::~DISAKernelChannel iDataRx" );
        delete iDataRx;
        iDataRx =NULL;
        }
    if( iPtrPtrToRxBuf )
        {
        C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel iPtrPtrToRxBuf" ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_PTRPTRTORXBUF, "DISAKernelChannel::~DISAKernelChannel iPtrPtrToRxBuf" );
        iPtrPtrToRxBuf =NULL;
        }
    if( iPtrPtrToDataRxBuf )
        {
        C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel iPtrPtrToDataRxBuf" ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_PTRPTRTODATARXBUF, "DISAKernelChannel::~DISAKernelChannel iPtrPtrToDataRxBuf" );
        iPtrPtrToDataRxBuf =NULL;
        }
    if( iEmptyRxDfc )
        {
        C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel iEmptyRxDfc" ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_EMTPYRXDFC, "DISAKernelChannel::~DISAKernelChannel iEmptyRxDfc" );
        iEmptyRxDfc->Cancel();
        delete iEmptyRxDfc;
        iEmptyRxDfc = NULL;
        }
    if( iEmptyDataRxDfc )
        {
        C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel iEmptyDataRxDfc" ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DISAKERNELCHANNEL_EMTPYDATARXDFC, "DISAKernelChannel::~DISAKernelChannel iEmptyDataRxDfc" );
        iEmptyDataRxDfc->Cancel();
        delete iEmptyDataRxDfc;
        iEmptyDataRxDfc = NULL;
        }

    // not owned, just using
    iPep = NULL;
    iExtensionApi = NULL;
    C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel table TBR" ) ) );
    for( TInt i( KErrNone ); i < EIADAsyncLast; ++i )
        {
        iRequests[ i ].iDfcFunction = NULL;
        iRequests[ i ].iRequestStatus = NULL;
        }
    iChannelNumber = KNotInitializedChannel;
    C_TRACE( ( _T( "DISAKernelChannel::~DISAKernelChannel 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DUP1_DISAKERNELCHANNEL_DISAKERNELCHANNEL_EXIT, "<DISAKernelChannel::~DISAKernelChannel" );
    }

// Handling of the request from kernel api
TInt DISAKernelChannel::HandleRequest(
        TThreadMessage& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_HANDLEREQUEST_ENTRY, ">DISAKernelChannel::HandleRequest;aMsg=%x", ( TUint )&( aMsg ) );
    C_TRACE( ( _T( "DISAKernelChannel::HandleRequest 0x%x 0x%x 0x%x ->" ), this, iChannelNumber, aMsg.iValue ) );
    // NOTE! Major change previously went as client thread's context for allocating etc... now going as
    // context of the extension dfc thread.

    // __NK_ASSERT_UNLOCKED;
    __ASSERT_NO_FAST_MUTEX; // nk_priv.h published partner.
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, iChannelNumber );
    // TODO
    //@pre    Kernel must be unlocked
    //@pre    interrupts enabled

    C_TRACE( ( _T( "DISAKernelChannel::HandleRequest 0x%x == 0x%x " ), iChannelNumber, KNotInitializedChannel ) );
    TInt valueToReturn( KErrNotSupported );
    if( KNotInitializedChannel == iChannelNumber )
        {
        // In case of a open request.
        if( EIADAsyncOpen == aMsg.iValue )
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleRequest open.. 0x%x" ), this ) );
            OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_HANDLEREQUEST, "DISAKernelChannel::HandleRequest;this=%x", this );
            
            valueToReturn = aMsg.SendReceive( &iKernelChMsgQue );
            }
        // In case of a already closed channel close request.
        else
            {
            // Do nothing
            C_TRACE( ( _T( "DISAKernelChannel::HandleRequest already closed channel.. 0x%x" ), this ) );
            OstTrace1( TRACE_NORMAL, DUP1_DISAKERNELCHANNEL_HANDLEREQUEST, "DISAKernelChannel::HandleRequest already closed channel;this=%x", this );
            
            valueToReturn = KErrNone;
            ASSERT_RESET_ALWAYS( ( EIADClose == aMsg.iValue ), EIADWrongParameter | EIADFaultIdentifier6 << KFaultIdentifierShift  );
            }
        }
    else
        {
        // Do nothing
        valueToReturn = KErrInUse;
        if( EIADAsyncOpen != aMsg.iValue )
            {
            valueToReturn = aMsg.SendReceive( &iKernelChMsgQue );
            }
        }
    C_TRACE( ( _T( "DISAKernelChannel::HandleRequest 0x%x %d <-" ), this, valueToReturn ) );

    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_HANDLEREQUEST_EXIT, "<DISAKernelChannel::HandleRequest;valueToReturn=%d", valueToReturn );
    return valueToReturn;
    }

void DISAKernelChannel::NotifyConnectionStatus(
        TIADConnectionStatus aStatus
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_NOTIFYCONNECTIONSTATUS_ENTRY, ">DISAKernelChannel::NotifyConnectionStatus;aStatus=%x", ( TUint )&( aStatus ) );

    C_TRACE( ( _T( "DISAKernelChannel::NotifyConnectionStatus -> %d 0x%x" ), aStatus, this ) );
    // If we have DFC active.
    CompleteChannelRequest( EIADAsyncNotifyConnectionStatus, aStatus );
    C_TRACE( ( _T( "DISAKernelChannel::NotifyConnectionStatus <- %d 0x%x" ), aStatus, this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_NOTIFYCONNECTIONSTATUS_EXIT, "<DISAKernelChannel::NotifyConnectionStatus" );
    }

void DISAKernelChannel::ReceiveMsg(
        const TDesC8& aMessage
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_RECEIVEMSG_ENTRY, ">DISAKernelChannel::ReceiveMsg;aMessage=%x", ( TUint )&( aMessage ) );

    C_TRACE( ( _T( "DISAKernelChannel::ReceiveMsg 0x%x 0x%x ->" ), iChannelNumber, &aMessage ) );
    // Can only be called from thread context.
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );// 0 is iad?
    iRx->Add( aMessage );
//    TRACE_ASSERT( !iEmptyRxDfc->Queued() );
    iEmptyRxDfc->Enque();
    C_TRACE( ( _T( "DISAKernelChannel::ReceiveMsg 0x%x 0x%x <-" ), iChannelNumber, &aMessage ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_RECEIVEMSG_EXIT, "<DISAKernelChannel::ReceiveMsg" );
    }

// Register pep associated to this channel to listen data message
// deliveries.
void DISAKernelChannel::RegisterPep(
        MDataMessageStatus& aPep
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_REGISTERPEP_ENTRY, ">DISAKernelChannel::RegisterPep;aPep=%x", ( TUint )&( aPep ) );

    C_TRACE( ( _T( "DISAKernelChannel::RegisterPep 0x%x 0x%x ->" ), iPep, iChannelNumber ) );
    iPep = &aPep;
    C_TRACE( ( _T( "DISAKernelChannel::RegisterPep 0x%x 0x%x <-" ), iPep, iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_REGISTERPEP_EXIT, "<DISAKernelChannel::RegisterPep" );
    }

// When data message is written to user thread call
// MDataMesssageStatus::DataMessageDelivered. It's responsibility is to
// tell PEP that one data message e.g.a credit is used. PEP's should also
// keep count of data messages added to channel to be abel to control the
// legacy flowcontrol
void DISAKernelChannel::ReceiveDataMsg(
        const TDesC8& aDataMessage
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_RECEIVEDATAMSG_ENTRY, ">DISAKernelChannel::ReceiveDataMsg;aDataMessage=%x", ( TUint )&( aDataMessage ) );

    C_TRACE( ( _T( "DISAKernelChannel::ReceiveDataMsg 0x%x 0x%x ->" ), iChannelNumber, &aDataMessage ) );
    // Can only be called from thread context.
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );// 0 is iad?
    iDataRx->Add( aDataMessage );
    iEmptyDataRxDfc->Enque();
    C_TRACE( ( _T( "DISAKernelChannel::ReceiveDataMsg 0x%x 0x%x <-" ), iChannelNumber, &aDataMessage ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_RECEIVEDATAMSG_EXIT, "<DISAKernelChannel::ReceiveDataMsg" );
    }

void DISAKernelChannel::NotifyFlowCtrl(
        TIADFlowControlStatus aStatus
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_NOTIFYFLOWCTRL_ENTRY, ">DISAKernelChannel::NotifyFlowCtrl;aStatus=%x", ( TUint )&( aStatus ) );

    C_TRACE( ( _T( "DISAKernelChannel::NotifyFlowCtrl -> %d 0x%x" ), aStatus, this ) );
    // If we have DFC active.
    CompleteChannelRequest( EIADAsyncNotifyFlowControlStatus, aStatus );
    C_TRACE( ( _T( "DISAKernelChannel::NotifyFlowCtrl <- %d 0x%x" ), aStatus, this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_NOTIFYFLOWCTRL_EXIT, "<DISAKernelChannel::NotifyFlowCtrl" );
    }


void DISAKernelChannel::CompleteChannelRequest(
        TInt aRequest,
        TInt aStatusToComplete
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELCHANNEL_COMPLETECHANNELREQUEST_ENTRY, ">DISAKernelChannel::CompleteChannelRequest;aRequest=%d;aStatusToComplete=%d", aRequest, aStatusToComplete );

    C_TRACE( ( _T( "DISAKernelChannel::CompleteChannelRequest %d %d 0x%x ->" ), aRequest, aStatusToComplete, iChannelNumber ) );
    ASSERT_DFCTHREAD_INEXT();
    // Acceptable asynchronous requests in kernel channel.
    ASSERT_RESET_ALWAYS( (  aRequest == EIADAsyncOpen || 
                            aRequest == EIADAsyncReceive ||
                            aRequest == EIADAsyncDataReceive ||
                            aRequest == EIADAsyncNotifyFlowControlStatus ||
                            aRequest == EIADAsyncNotifyConnectionStatus ), EIADWrongParameter | EIADFaultIdentifier7 << KFaultIdentifierShift  );
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    ASSERT_RESET_ALWAYS( EIADAsyncLast > ( aRequest ), EIADOverTheLimits | EIADFaultIdentifier35 << KFaultIdentifierShift );                                                
    if( iRequests[ aRequest ].iDfcFunction )
        {
        // Clien't DFC function. The writing to user addr. space and DP issues are in resp of the client.
        TDfc* completeDfc = iRequests[ aRequest ].iDfcFunction;
        C_TRACE( ( _T( "DISAKernelChannel::CompleteChannelRequest %d %d 0x%x 0x%x" ), aRequest, aStatusToComplete, completeDfc, iChannelNumber ) );
        OstTraceExt4( TRACE_NORMAL, DISAKERNELCHANNEL_COMPLETECHANNELREQUEST, "DISAKernelChannel::CompleteChannelRequest;aRequest=%d;aStatusToComplete=%d;iChannelNumber=%hx;completeDfc=%x", aRequest, aStatusToComplete, iChannelNumber, (TUint)(completeDfc) );
        
        *iRequests[ aRequest ].iRequestStatus = aStatusToComplete;
        C_TRACE( ( _T( "DISAKernelChannel::CompleteChannelRequest %d %d 0x%x 0x%x TBR" ), aRequest, aStatusToComplete, completeDfc, iChannelNumber ) );
        iChannelNumber = ( aRequest == EIADAsyncOpen ? ( KErrNone == aStatusToComplete || KErrInUse == aStatusToComplete ? ~iChannelNumber : KNotInitializedChannel ) : iChannelNumber );
        ASSERT_RESET_ALWAYS( !completeDfc->Queued(), EIADDFCAlreadyQueued | EIADFaultIdentifier1 << KFaultIdentifierShift );
        completeDfc->Enque();
        C_TRACE( ( _T( "DISAKernelChannel::CompleteChannelRequest %d %d 0x%x 0x%x TBR" ), aRequest, aStatusToComplete, completeDfc, iChannelNumber ) );
        iRequests[ aRequest ].iDfcFunction = NULL;
        }
    C_TRACE( ( _T( "DISAKernelChannel::CompleteChannelRequest %d %d 0x%x <-" ), aRequest, aStatusToComplete, iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_COMPLETECHANNELREQUEST_EXIT, "<DISAKernelChannel::CompleteChannelRequest" );
    }

void DISAKernelChannel::MsgQDfc(
        TAny* aPtr
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_MSGQDFC_ENTRY, ">DISAKernelChannel::MsgQDfc;aPtr=%x", ( TUint )( aPtr ) );

    C_TRACE( ( _T( "DISAKernelChannel::MsgQDfc ->" ) ) );
    DISAKernelChannel* tmp = reinterpret_cast<DISAKernelChannel*>( aPtr );
    tmp->HandleThreadMsg( static_cast<TThreadMessage&>(*tmp->iKernelChMsgQue.iMessage ) );
    C_TRACE( ( _T( "DISAKernelChannel::MsgQDfc <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_MSGQDFC_EXIT, "<DISAKernelChannel::MsgQDfc" );
    }

void DISAKernelChannel::HandleThreadMsg(
         TThreadMessage& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_HANDLETHREADMSG_ENTRY, ">DISAKernelChannel::HandleThreadMsg;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DISAKernelChannel::HandleThreadMsg 0x%x ->" ), &aMsg ) );
    TThreadMessage& m = ( aMsg );
    /* TODO CLOSING See LDD Framework DLogicalChannel */
    TInt completeValue( KErrNone );
    TBool complete( ETrue );
    switch( m.iValue )
        {
        // All asynchronous requests. Return result after DFC function is run.
        case EIADAsyncOpen:
        case EIADAsyncNotifyConnectionStatus:
        case EIADAsyncNotifyFlowControlStatus:
        case EIADAsyncReceive:
        case EIADAsyncDataReceive:
            {
            // No need to check return value in async functions, completed to client from DFC.
            HandleDfcRequest( m );
            break;
            }
        case EIADAllocateBlock:
        case EIADAllocateDataBlock:
        case EIADDeAllocateBlock:
        case EIADDeAllocateDataBlock:
        case EIADSend:
        case EIADDataSend:
        case EIADSendIndication:
        case EIADSubscribeIndications:
        case EIADSubscribeIndications32Bit:
            {
            completeValue = HandleSyncRequest( m );
            break;
            }
        // From synchronized request return the result immediately
        case EIADClose:
            {
            completeValue = HandleSyncRequest( m );
            break;
            }
        case KDestroyChannelMsg:
            {
            completeValue = KErrNone;
            complete = EFalse;
            break;
            }
        case KMaxTInt:
            {
            completeValue = KErrNone;
            DoCancel( KMaxTInt, m.Int0() );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier1 << KFaultIdentifierShift );
            break;
            }
        }
    m.Complete( completeValue, complete );
    C_TRACE( ( _T( "DISAKernelChannel::HandleThreadMsg <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_HANDLETHREADMSG_EXIT, "<DISAKernelChannel::HandleThreadMsg" );
    }

void DISAKernelChannel::HandleDfcRequest(
        TThreadMessage& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_HANDLEDFCREQUEST_ENTRY, ">DISAKernelChannel::HandleDfcRequest;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DISAKernelChannel::HandleDfcRequest 0x%x ->" ), &aMsg ) );
    TThreadMessage& m = ( aMsg );
    TInt request( m.iValue );
    ASSERT_RESET_ALWAYS( m.iArg, EIADNullParameter | EIADFaultIdentifier1 << KFaultIdentifierShift );
    // If request already active.
    ASSERT_RESET_ALWAYS( EIADAsyncLast > ( request ), EIADOverTheLimits | EIADFaultIdentifier36 << KFaultIdentifierShift );                                                
    if( iRequests[ request ].iDfcFunction )
        {
        C_TRACE( ( _T( "DISAKernelChannel::HandleDfcRequest existing 0x%x request 0x%x" ), this, request ) );
        TRACE_ASSERT_INFO(0, (TUint8)iChannelNumber<<KChannelNumberShift | (TUint8)request<<KExtraInfoShift );
        // Active object should not give same request object twice before completing the first one.
        ASSERT_RESET_ALWAYS( !iRequests[ request ].iDfcFunction, ( EIADSameRequestTwice | static_cast<TUint8>( iChannelNumber ) << KChannelNumberShift | static_cast<TUint8>( request ) << KExtraInfoShift ) );
        //CompleteChannelRequest( request, KErrAlreadyExists );
        }
    else
        {
        C_TRACE( ( _T( "HandleDfcRequest handling %d 0x%x" ), request, this ) );
        //NOTE! This are tight to isakernelapi parameter passing!!
        C_TRACE( ( _T( "TBR *** TBR m.iArg[ 0 ]=0x%x m.iArg[ 1 ]=0x%x m.iArg[ 2 ]=0x%x m.iArg[ 3 ]=0x%x "), m.iArg[ 0 ], m.iArg[ 1 ], m.iArg[ 2 ], m.iArg[ 3 ] ) );
        TUint32* tablePtr = reinterpret_cast<TUint32*>( m.Ptr0() );
        TInt* dfcStatus = reinterpret_cast<TInt*>( tablePtr[ 0 ] );
        TDfc* dfc = reinterpret_cast<TDfc*>( tablePtr[ 1 ] );
        ASSERT_RESET_ALWAYS( dfcStatus, EIADNullParameter | EIADFaultIdentifier2 << KFaultIdentifierShift  );
        ASSERT_RESET_ALWAYS( dfc, EIADNullParameter | EIADFaultIdentifier3 << KFaultIdentifierShift  );
        iRequests[ request ].iDfcFunction = dfc;
        iRequests[ request ].iRequestStatus = dfcStatus;
        // Set the request to pending.
        *iRequests[ request ].iRequestStatus = KRequestPending;
        C_TRACE( ( _T( "HandleDfcRequest dfc 0x%x dfcStatus 0x%x dfcStatus %d " ), iRequests[ request ].iDfcFunction, iRequests[ request ].iRequestStatus, *iRequests[ request ].iRequestStatus ) );
        switch( request )
            {
            case EIADAsyncOpen:
                {
                C_TRACE( ( _T( "HandleDfcRequest EIADNokiaKernelOpen 0x%x" ), this ) );
                iChannelNumber = tablePtr[ KThirdParam ];
                iChannelNumber = ~iChannelNumber;
                C_TRACE( ( _T( "HandleDfcRequest EIADNokiaKernelOpen 0x%x 0x%x TBR" ), iChannelNumber, ~iChannelNumber ) );
                const TDesC8& resource = *reinterpret_cast<const TDesC8*>( tablePtr[ KFourthParam ] );
                ASSERT_RESET_ALWAYS( resource.Length() > ( 3 ), EIADOverTheLimits | EIADFaultIdentifier37 << KFaultIdentifierShift );                                                
                C_TRACE( ( _T( "0x%x 0x%x 0x%x 0x%x TBR" ), resource[ 0 ], resource[ 1 ], resource[ 2 ], resource[ 3 ] ) );
                if( PN_NO_ROUTING == resource[ KFourthParam ] )
                    {
                    C_TRACE( ( _T( "HandleDfcRequest EIADNokiaKernelOpen normal 0x%x" ), this ) );
                    iExtensionApi->Open( ~iChannelNumber, request, this );
                    }
                else
                    {
                    // TODO : trace works only for 8-bit values.
                    C_TRACE( ( _T( "HandleDfcRequest EIADNokiaKernelOpen resource 0x%x 0x%x" ), resource[ KFourthParam ], this ) );
                    iExtensionApi->Open( ~iChannelNumber, request, resource, this );
                    }
                break;
                }
            case EIADAsyncNotifyConnectionStatus:
                {
                C_TRACE( ( _T( "HandleDfcRequest EIADAsyncNotifyConnectionStatus 0x%x" ), this ) );
                // Nothing to do, the dfc and status are marked as active.
                break;
                }
            case EIADAsyncNotifyFlowControlStatus:
                {
                C_TRACE( ( _T( "HandleDfcRequest EIADAsyncNotifyFlowControlStatus 0x%x" ), this ) );
                // Nothing to do, the dfc and status are marked as active.
                break;
                }                
            case EIADAsyncReceive:
                {
                C_TRACE( ( _T( "HandleDfcRequest EIADAsyncReceive 0x%x" ), this ) );
                ASSERT_RESET_ALWAYS( !iPtrPtrToRxBuf, EIADBufferNotReleased | ( iChannelNumber << KChannelNumberShift ) );
                iPtrPtrToRxBuf = reinterpret_cast<TDes8**>( tablePtr[ KThirdParam ] );
                C_TRACE( ( _T( "PTR iPtrPtrToRxBuf PTR 0x%x REF 0x%x" ), iPtrPtrToRxBuf, &iPtrPtrToRxBuf ) );
                C_TRACE( ( _T( "HandleDfcRequest EIADAsyncReceive 0x%x 0x%x" ), this, iPtrPtrToRxBuf ) );
                iEmptyRxDfc->Enque();
                break;
                }
            case EIADAsyncDataReceive:
                {
                C_TRACE( ( _T( "HandleDfcRequest EIADAsyncDataReceive 0x%x" ), this ) );
                ASSERT_RESET_ALWAYS( !iPtrPtrToDataRxBuf, EIADBufferNotReleased | ( iChannelNumber << KChannelNumberShift ) );
                iPtrPtrToDataRxBuf = reinterpret_cast<TDes8**>( tablePtr[ KThirdParam ] );
                C_TRACE( ( _T( "PTR iPtrPtrToDataRxBuf PTR 0x%x REF 0x%x" ), iPtrPtrToDataRxBuf, &iPtrPtrToDataRxBuf ) );
                C_TRACE( ( _T( "HandleDfcRequest EIADAsyncDataReceive 0x%x 0x%x" ), this, iPtrPtrToDataRxBuf ) );
                iEmptyDataRxDfc->Enque();
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier2 << KFaultIdentifierShift );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DISAKernelChannel::HandleDfcRequest 0x%x <-" ), &aMsg ) );

    }


TInt DISAKernelChannel::HandleSyncRequest(
        TThreadMessage& aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_HANDLESYNCREQUEST_ENTRY, ">DISAKernelChannel::HandleSyncRequest;aMsg=%x", ( TUint )&( aMsg ) );

    C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest 0x%x ->" ), &aMsg ) );
    TThreadMessage& m = ( aMsg );
    TInt request( m.iValue );
    ASSERT_RESET_ALWAYS( m.iArg, EIADNullParameter | EIADFaultIdentifier4 << KFaultIdentifierShift  );
    TInt returnValue( KErrNone );
    C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest handling %d 0x%x" ), request, this ) );
    //NOTE! These are tight to isakernelapi parameter passing!!
    TUint32* tablePtr = reinterpret_cast<TUint32*>( m.Ptr0() );
    switch( request )
        {
        case EIADClose:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADClose 0x%x" ), this ) );
            ResetQueues();
            for( TInt i( KErrNone ); i < EIADAsyncLast; ++i )
                {
                C_TRACE( ( _T( "DISAKernelChannel::CancelRequests req to cancel %d" ), i ) );
                DoCancel( KMaxTInt, i );
                }
            Close( iChannelNumber );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADClose 0x%x 0x%x == 0x%x " ), this, iChannelNumber, KNotInitializedChannel ) );
            break;
            }
        case EIADAllocateBlock:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADAllocateBlock 0x%x" ), this ) );
            const TInt size = *reinterpret_cast<TInt*>( tablePtr[ KFirstParam ] );
            TDes8*& block = *(reinterpret_cast<TDes8**>( tablePtr[ KSecondParam ] ));
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADAllocateBlock 0x%x 0x%x TBR" ), this, block ) );
            block = ( &iExtensionApi->AllocateBlock( size ) );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADAllocateBlock 0x%x 0x%x TBR2" ), this, block ) );
            break;
            }
        case EIADAllocateDataBlock:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADAllocateDataBlock 0x%x" ), this ) );
            const TInt size = *reinterpret_cast<TInt*>( tablePtr[ KFirstParam ] );
            TDes8*& block = *(reinterpret_cast<TDes8**>( tablePtr[ KSecondParam ] ));
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADAllocateDataBlock 0x%x 0x%x TBR" ), this, block ) );
            block = ( &iExtensionApi->AllocateDataBlock( size ) );
            C_TRACE( ( _T( "GET block 0x%x length %d maxLength %d" ), &block, block->Length(), block->MaxLength() ) );
            TInt tmpLength( block->MaxLength() - KPipeDataHeader );
            TUint8* tmpPtr( const_cast<TUint8*>( ( block->Ptr() + KPipeDataHeader ) ) );
            TPtr* test = static_cast<TPtr8*>( block );
            test->Set( tmpPtr, 0, tmpLength );
            C_TRACE( ( _T( "MOD block 0x%x length %d maxLength %d tmpPtr = 0x%x tmpPtr-KPipeDataHeader=0x%x" ), &block, block->Length(), block->MaxLength(), tmpPtr, ( tmpPtr - KPipeDataHeader ) ) );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADAllocateDataBlock 0x%x 0x%x TBR2" ), this, block ) );
            break;
            }            
        case EIADDeAllocateBlock:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDeAllocateBlock 0x%x" ), this ) );
            TDes8& block = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDeAllocateBlock 0x%x 0x%x TBR2" ), this, &block ) );
            // Needed to ensure that right channel is deleting the right block. (Could it be done otherways too?)
            if( iPtrPtrToRxBuf )
                {
                if ( &block == *iPtrPtrToRxBuf )
                    {
                    C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest release iPtrPtrToRxBuf") ) );
                    iPtrPtrToRxBuf = NULL;
                    }
                }
            iExtensionApi->DeAllocateBlock( block );
            break;
            }
        case EIADDeAllocateDataBlock:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDeAllocateDataBlock 0x%x" ), this ) );
            // Needed to ensure that right channel is deleting the right block. (Could it be done otherways too?)
            TPtr8& block = *reinterpret_cast<TPtr8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDeAllocateDataBlock 0x%x 0x%x TBR2" ), this, &block ) );
            if( iPtrPtrToDataRxBuf )
                {
                if ( &block == *iPtrPtrToDataRxBuf ) // TODO: do less branches
                    {
                    C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest release iPtrPtrToDataRxBuf") ) );
                    iPtrPtrToDataRxBuf = NULL;
                    }
                }
            TDes8* block2 = &block;
            TDes8*& tmpPtrPtr = *( &block2 );
            TInt tmpLength( ( block2->Length() + KPipeDataHeader ) );
            TInt tmpMaxLength( ( block2->MaxLength() + KPipeDataHeader ) );
            TUint8* tmpPtr( const_cast<TUint8*>( ( tmpPtrPtr->Ptr() - KPipeDataHeader ) ) );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDeAllocateDataBlock TBR tmpPtr=0x%x tmpPtr+KPipeDataHeader=0x%x TBR2" ), tmpPtr, (tmpPtr+KPipeDataHeader) ) );
            block.Set( tmpPtr, tmpLength, tmpMaxLength );
            iExtensionApi->DeAllocateBlock( block );
            ASSERT_RESET_ALWAYS( iPep, EIADNullParameter | EIADFaultIdentifier28 << KFaultIdentifierShift  );
            iPep->DataMessageDelivered();


            break;
            }
        case EIADSend:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADSend 0x%x" ), this ) );
            TDes8& block = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADSend 0x%x 0x%x TBR2" ), this, &block ) );
            returnValue = iExtensionApi->SendMessage( block, iChannelNumber );
            break;
            }
        case EIADDataSend:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDataSend 0x%x" ), this ) );
            TPtr8& block = *reinterpret_cast<TPtr8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDataSend 0x%x 0x%x TBR2" ), this, &block ) );
            TDes8* block2 = &block;
            TDes8*& tmpPtrPtr = *( &block2 );
            TInt tmpLength( ( block2->Length() + KPipeDataHeader ) );
            TInt tmpMaxLength( ( block2->MaxLength() + KPipeDataHeader ) );
            TUint8* tmpPtr( const_cast<TUint8*>( ( tmpPtrPtr->Ptr() - KPipeDataHeader ) ) );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADDeAllocateDataBlock TBR tmpPtr=0x%x tmpPtr+KPipeDataHeader=0x%x TBR2" ), tmpPtr, (tmpPtr+KPipeDataHeader) ) );
            block.Set( tmpPtr, tmpLength, tmpMaxLength );
            returnValue = iExtensionApi->SendMessage( block, iChannelNumber );
            break;
            }            
        case EIADSendIndication:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADSendIndication 0x%x" ), this ) );
            TDes8& block = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADSendIndication 0x%x 0x%x TBR2" ), this, &block ) );
            returnValue = iExtensionApi->SendIndication( block, iChannelNumber );
            break;
            }
        case EIADSubscribeIndications:
        case EIADSubscribeIndications32Bit:
            {
            C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest EIADSubscribeIndications | EIADSubscribeIndications32Bit:0x%x" ), this ) );
            TDes8& subscription = *reinterpret_cast<TDes8*>( tablePtr[ KFirstParam ] );
            TBool thirtyTwoBit( EIADSubscribeIndications32Bit == request ? ETrue : EFalse );
            returnValue = iExtensionApi->OrderIndication( subscription, iChannelNumber, thirtyTwoBit );
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier3 << KFaultIdentifierShift );
            break;
            }
        }
    C_TRACE( ( _T( "DISAKernelChannel::HandleSyncRequest 0x%x %d <-" ), &aMsg, returnValue ) );
    return returnValue;

    }

void DISAKernelChannel::EmptyRxDfc(
        TAny* aPtr // self
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_EMPTYRXDFC_ENTRY, ">DISAKernelChannel::EmptyRxDfc;aPtr=%x", ( TUint )( aPtr ) );

    C_TRACE( ( _T( "DISAKernelChannel::EmptyRxDfc ->" ) ) );
    DISAKernelChannel& chTmp = *reinterpret_cast<DISAKernelChannel*>( aPtr );
    if( ( chTmp.iRequests[ EIADAsyncReceive ].iDfcFunction ) && ( chTmp.iRx->Count() > 0 ) )
        {
        C_TRACE( ( _T( "DISAKernelChannel::EmptyRxDfc 0x%x writing to kernel client" ), chTmp.iChannelNumber ) );
        TDes8*& tmpWrite = *chTmp.iPtrPtrToRxBuf;
        tmpWrite = &chTmp.iRx->Get();
        C_TRACE( ( _T( "PTR2 iPtrPtrToRxBuf PTR 0x%x REF 0x%x" ), chTmp.iPtrPtrToRxBuf, &chTmp.iPtrPtrToRxBuf ) );
        OstTraceExt2( TRACE_NORMAL, DISAKERNELCHANNEL_EMPTYRXDFC, "DISAKernelChannel::EmptyRxDfc;iPtrPtrToRxBuf=%x;iPtrPtrToRxBuf_ref=%x", (TUint)(chTmp.iPtrPtrToRxBuf), (TUint)&(chTmp.iPtrPtrToRxBuf) );
        
        C_TRACE( ( _T( "DISAKernelChannel::EmptyRxDfc client 0x%x " ), *chTmp.iPtrPtrToRxBuf ) );
        chTmp.CompleteChannelRequest( EIADAsyncReceive, KErrNone );
        }
    else
        {
        C_TRACE( ( _T( "DISAKernelChannel::EmptyRxDfc 0x%x no receive active or no message" ), chTmp.iChannelNumber ) );
        OstTraceExt1( TRACE_NORMAL, DUP1_DISAKERNELCHANNEL_EMPTYRXDFC, "DISAKernelChannel::EmptyRxDfc;iChannelNumber=%hx", chTmp.iChannelNumber );
        
        }
    C_TRACE( ( _T( "DISAKernelChannel::EmptyRxDfc <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_EMPTYRXDFC_EXIT, "<DISAKernelChannel::EmptyRxDfc" );
    }

void DISAKernelChannel::EmptyDataRxDfc(
        TAny* aPtr // self
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_EMPTYDATARXDFC_ENTRY, ">DISAKernelChannel::EmptyDataRxDfc;aPtr=%x", ( TUint )( aPtr ) );

    C_TRACE( ( _T( "DISAKernelChannel::EmptyDataRxDfc ->" ) ) );
    DISAKernelChannel& chTmp = *reinterpret_cast<DISAKernelChannel*>( aPtr );
    if( ( chTmp.iRequests[ EIADAsyncDataReceive ].iDfcFunction ) && ( chTmp.iDataRx->Count() > 0 ) )
        {
        C_TRACE( ( _T( "DISAKernelChannel::EmptyDataRxDfc 0x%x writing to kernel client" ), chTmp.iChannelNumber ) );
        TDes8*& tmpWrite = ( *chTmp.iPtrPtrToDataRxBuf );
        tmpWrite = ( &chTmp.iDataRx->Get() );
        C_TRACE( ( _T( "GET tmpWrite 0x%x length %d maxLength %d" ), &tmpWrite, tmpWrite->Length(), tmpWrite->MaxLength() ) );
        // Adjust the start not to contain pipe header!
        TInt tmpLength( tmpWrite->Length() - KPipeDataHeader );
        TInt tmpMaxLength( tmpWrite->MaxLength() - KPipeDataHeader );
        TUint8* tmpPtr( const_cast<TUint8*>( ( tmpWrite->Ptr() + KPipeDataHeader ) ) );
        TPtr* test = static_cast<TPtr8*>(tmpWrite);
        test->Set( tmpPtr, tmpLength, tmpMaxLength );
        C_TRACE( ( _T( "MOD tmpWrite 0x%x length %d maxLength %d" ), &tmpWrite, tmpWrite->Length(), tmpWrite->MaxLength() ) );
        C_TRACE( ( _T( "PTR2 iPtrPtrToDataRxBuf PTR 0x%x REF 0x%x" ), chTmp.iPtrPtrToDataRxBuf, &chTmp.iPtrPtrToDataRxBuf ) );
        C_TRACE( ( _T( "DISAKernelChannel::EmptyDataRxDfc client 0x%x " ), *chTmp.iPtrPtrToDataRxBuf ) );
        OstTraceExt3( TRACE_NORMAL, DISAKERNELCHANNEL_EMPTYDATARXDFC, "DISAKernelChannel::EmptyDataRxDfc;tmpWrite=%x;iPtrPtrToDataRxBuf=%x;iPtrPtrToDataRxBuf_ref=%x", (TUint)&(tmpWrite), (TUint)(chTmp.iPtrPtrToDataRxBuf), (TUint)&(chTmp.iPtrPtrToDataRxBuf) );
        
        chTmp.CompleteChannelRequest( EIADAsyncDataReceive, KErrNone );
        }
    else
        {
        C_TRACE( ( _T( "DISAKernelChannel::EmptyDataRxDfc 0x%x no datareceive active or no datamessage" ), chTmp.iChannelNumber ) );
        OstTraceExt1( TRACE_NORMAL, DUP1_DISAKERNELCHANNEL_EMPTYDATARXDFC, "DISAKernelChannel::EmptyDataRxDfc;iChannelNumber=%hx", chTmp.iChannelNumber );
        
        }
    C_TRACE( ( _T( "DISAKernelChannel::EmptyDataRxDfc <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_EMPTYDATARXDFC_EXIT, "<DISAKernelChannel::EmptyDataRxDfc" );
    }

void DISAKernelChannel::DoCancel(
        TInt aRequest,
        TInt aMask )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_ENTRY, ">DISAKernelChannel::DoCancel;aRequest=%d;aMask=%d", aRequest, aMask );

    C_TRACE( ( _T( "DISAKernelChannel::DoCancel ->" ) ) );
    ASSERT_RESET_ALWAYS( EIADAsyncLast > ( aMask&aRequest ), EIADOverTheLimits | EIADFaultIdentifier38 << KFaultIdentifierShift );                                                
    if( iRequests[ aMask&aRequest ].iDfcFunction )
        {
        switch( aMask&aRequest )
            {
            case EIADAsyncNotifyFlowControlStatus:
                {
                // Nothing to do. Just cancel with KErrCancel.
                C_TRACE( ( _T( "DISAKernelChannel::DoCancel EIADAsyncNotifyFlowControlStatus nothing" ) ) );
                OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_FLOW, "DISAKernelChannel::DoCancel EIADAsyncNotifyFlowControlStatus nothing" );
                
                break;
                }
            case EIADAsyncNotifyConnectionStatus:
                {
                // Nothing to do. Just cancel with KErrCancel.
                C_TRACE( ( _T( "DISAKernelChannel::DoCancel EIADAsyncNotifyConnectionStatus nothing" ) ) );
                OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_CONNSTATUS, "DISAKernelChannel::DoCancel EIADAsyncNotifyConnectionStatus nothing" );
                
                break;
                }
            case EIADAsyncReceive:
                {
                C_TRACE( ( _T( "DISAKernelChannel::DoCancel EIADAsyncReceive iPtrPtrToRxBuf 0x%x -> NULL" ), iPtrPtrToRxBuf ) );
                OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_RECEIVE, "DISAKernelChannel::DoCancel EIADAsyncDataReceive;iPtrPtrToRxBuf=%x", iPtrPtrToRxBuf );
                
                iPtrPtrToRxBuf = NULL;
                break;
                }
            case EIADAsyncDataReceive:
                {
                C_TRACE( ( _T( "DISAKernelChannel::DoCancel EIADAsyncDataReceive iPtrPtrToDataRxBuf 0x%x -> NULL" ), iPtrPtrToDataRxBuf ) );
                OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_DATARECEIVE, "DISAKernelChannel::DoCancel EIADAsyncDataReceive;iPtrPtrToDataRxBuf=%x", iPtrPtrToDataRxBuf );
                iPtrPtrToDataRxBuf = NULL;
                break;
                }
            case EIADAsyncOpen:
                {
                C_TRACE( ( _T( "DISAKernelChannel::DoCancel EIADAsyncOpen nothing" ) ) );
                OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_OPEN, "DISAKernelChannel::DoCancel EIADAsyncOpen" );
                
                ResetQueues();
                Close( ~iChannelNumber );
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier4 << KFaultIdentifierShift );
                break;
                }
            }
        CompleteChannelRequest( aMask&aRequest, KErrCancel );
        }
    else
        {
        C_TRACE( ( _T( "DISAKernelChannel::DoCancel nothing to cancel" ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_IGNORE, "DISAKernelChannel::DoCancel nothing to cancel" );
        
        }
    C_TRACE( ( _T( "DISAKernelChannel::DoCancel <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_DOCANCEL_EXIT, "<DISAKernelChannel::DoCancel" );
    }

void DISAKernelChannel::ResetQueues(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, DISAKERNELCHANNEL_RESETQUEUES_ENTRY, ">DISAKernelChannel::ResetQueues;iChannelNumber=%hx", iChannelNumber );

    // TODO: SSI cancel pending sending
    C_TRACE( ( _T( "DISAKernelChannel::ResetQueues 0x%x ->" ), iChannelNumber ) );
    if( iRx )
        {
        C_TRACE( ( _T( "DISAKernelChannel::ResetQueues 0x%x iRx 0x%x" ), iChannelNumber, iRx ) );
        OstTraceExt2( TRACE_NORMAL, DISAKERNELCHANNEL_RESETQUEUES, "DISAKernelChannel::ResetQueues Rx;iChannelNumber=%hx;iRx=%x", iChannelNumber, (TUint)(iRx) );
        
        while( iRx->Count() )
            {
            iExtensionApi->DeAllocateBlock( iRx->Get() );
            }
        }
    if( iDataRx )
        {
        C_TRACE( ( _T( "DISAKernelChannel::ResetQueues 0x%x iDataRx 0x%x" ), iChannelNumber, iDataRx ) );
        OstTrace1( TRACE_NORMAL, DISAKERNELCHANNEL_RESETQUEUES_DATA, "DISAKernelChannel::ResetQueues;iDataRx=%x", iDataRx );
        
        while( iDataRx->Count() )
            {
            iExtensionApi->DeAllocateBlock( iDataRx->Get() );
            }
        }
    C_TRACE( ( _T( "DISAKernelChannel::ResetQueues 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_RESETQUEUES_EXIT, "<DISAKernelChannel::ResetQueues" );
    }

void DISAKernelChannel::Close(
        const TUint16 aChannelId
        )
    {
    OstTraceExt1( TRACE_NORMAL, DISAKERNELCHANNEL_CLOSE_ENTRY, ">DISAKernelChannel::Close;aChannelId=%hx", aChannelId );

    C_TRACE( ( _T( "DISAKernelChannel::Close 0x%x 0x%x ->" ), iChannelNumber, aChannelId ) );
    iExtensionApi->Close( aChannelId );
    iChannelNumber = KNotInitializedChannel;
    C_TRACE( ( _T( "DISAKernelChannel::Close 0x%x 0x%x <-" ), iChannelNumber, aChannelId ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELCHANNEL_CLOSE_EXIT, "<DISAKernelChannel::Close" );
    }

//From objectapi
void DISAKernelChannel::Receive(
        const TDesC8& aMessage
        )
    {
    C_TRACE( ( _T( "DISAKernelChannel::Receive 0x%x ->" ), &aMessage ) );
    ReceiveMsg( aMessage );
    C_TRACE( ( _T( "DISAKernelChannel::Receive 0x%x <-" ), &aMessage ) );
    }
// End of file.

