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



#include "queue.h"                  // For DQueue
#include "isauserchannel.h"         // For DISAUserChannel
#include "iadtrace.h"               // For C_TRACE, ASSERT_RESET.. and fault codes.
#include "isaaccessextension.h"     // For DIsaAccessExtension
#include "iadnokiadefinitions.h"    // For checking allowed user channels.

// Extracting and adding the pipeheader.
#include <phonetisi.h>              // For ISI_HEADER_SIZE
#include <pipeisi.h>                // For PNS_PIPE_DATA_OFFSET_DATA
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "isauserchannelTraces.h"
#endif
const TInt KPipeDataHeader( ISI_HEADER_SIZE + PNS_PIPE_DATA_OFFSET_DATA );

const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );
const TInt KNoParams( KErrNone );
const TInt KOneParam( 1 );
const TInt KTwoParams( 2 );
const TInt KThreeParams( 3 );

const TInt KDestStartOffset( 0 );
//
// user <-> kernel interaction() done in LDD DFC thread
// 
// kernel<->kernel interaction() done in Extension DFC thread
//
////////////////// NOTE! /////////////////////////////////////////
// 
// SYNCHRONIZATION:
// Check sync of mutex guarded shread members. 
// Rule1: No blocking operation like trace prints inside mutex guareded code sections.
//
// DEMAND_PAGING
// Receive (write k->u) is done only in LDD thread context to allow Extension thread to continue when dp swaps.
// Send ((write u->k) is not done at the moment in LDD thread context only. Check is it possible to happend (not to be in usable memory after send (unlikely?)).

// Change this to use UniqueID of DThread instead and to extension 
void DISAUserChannel::CheckDfc()
    {
    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_CHECKDFC_ENTRY, ">DISAUserChannel::CheckDfc" );

    DObject* tempObj = reinterpret_cast<DObject*>( &Kern::CurrentThread() ); 
    TUint8* buffer = ( TUint8* )Kern::Alloc( 100 );
    TPtr8* bufferPtr = new ( TPtr8 )( buffer, 100 );
    tempObj->Name( *bufferPtr );
    C_TRACE( ( _T( "DISAUserChannel::CheckDfc" ) ) );
    if ( bufferPtr->Compare( KIADLddDfc ) != KErrNone )
        {
        for( TInt i( 0 );i < bufferPtr->Length(); ++i )
           {
           C_TRACE( ( _T( "%c" ), i, bufferPtr->Ptr()[ i ] ) );
           }
        ASSERT_RESET_ALWAYS( 0, EIADWrongDFCQueueUsed| EIADFaultIdentifier3 << KFaultIdentifierShift  );
        }
    Kern::Free( buffer );
    delete bufferPtr;
    bufferPtr = NULL;

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_CHECKDFC_EXIT, "<DISAUserChannel::CheckDfc" );
    }

#ifdef _DEBUG
#define    ASSERT_DFCTHREAD_INLDD() CheckDfc()
#else
#define    ASSERT_DFCTHREAD_INLDD()
#endif

EXPORT_C DISAUserChannel::DISAUserChannel(
        // None
        )
    :
    iChannelNumber( KNotInitializedChannel ),
    iIADConnectionStatusPtr( NULL ),
    iIADFlowControlStatusPtr( NULL ),
    iFlowCtrlStatus( EIADTransmissionEnd ),
    iLastNotifiedFlowCtrlStatus( EIADTransmissionEnd ),
    iReceiveBufPtr( NULL ),
    iDataReceiveBufPtr( NULL ),
    iNeededBufLen( NULL ),
    iNeededDataBufLen( NULL ),
    iRx( NULL ),
    iDataRx( NULL ),
    iThread( &Kern::CurrentThread() ),
    iPep( NULL ),
    iReqQueue( NULL ),
    iMsgQue( MsgQueFunc, this, NULL, 1 ),
    iRxDeAllocate( NULL ),
    iDataRxDeAllocate( NULL )
    {
    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_DISAUSERCHANNEL_ENTRY, ">DISAUserChannel::DISAUserChannel" );

    C_TRACE( ( _T( "DISAUserChannel::DISAUserChannel ->" ) ) );

    iExtensionApi = DIsaAccessExtension::GetChannel2IADApi();
    iEmptyRxQueueDfc = new TDfc( EmptyRxQueueDfc, this, DIsaAccessExtension::GetDFCThread( EIADLddDfcQueue ), KIADLddEmptyRxQueuePriori );
    iEmptyDataRxQueueDfc = new TDfc( EmptyDataRxQueueDfc, this, DIsaAccessExtension::GetDFCThread( EIADLddDfcQueue ), KIADLddEmptyDataRxQueuePriori );
    ASSERT_RESET_ALWAYS( iExtensionApi && iEmptyRxQueueDfc && iEmptyDataRxQueueDfc,
                                             EIADMemoryAllocationFailure | EIADFaultIdentifier20 << KFaultIdentifierShift );
    iConnectionStatusDfc = new TDfc( ConnectionStatusDfc, this, DIsaAccessExtension::GetDFCThread( EIADLddDfcQueue ), KIADLddConnStatPriori );
    iFlowCtrlStatusDfc = new TDfc( FlowCtrlStatusDfc, this, DIsaAccessExtension::GetDFCThread( EIADLddDfcQueue ), KIADLddFlowStatPriori );
    iCompleteChannelRequestDfc = new TDfc( CompleteChReqDfc, this, DIsaAccessExtension::GetDFCThread( EIADLddDfcQueue ), KIADLddCompleteChannelPriori );
    // For sync between ldd <-> extension dfc threads. NOTE! must not be held when accessing to user memory!
    iChFastMutex = new NFastMutex();
    ASSERT_RESET_ALWAYS( iConnectionStatusDfc && iFlowCtrlStatusDfc && iCompleteChannelRequestDfc && iChFastMutex,
                                           EIADMemoryAllocationFailure | EIADFaultIdentifier21 << KFaultIdentifierShift );

    iCleanDfc = new TDfc( CleanBlocksDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADLddEmptyRxQueuePriori );
    iCleanDataDfc = new TDfc( CleanDataBlocksDfc, this, DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ), KIADLddEmptyDataRxQueuePriori );
    ASSERT_RESET_ALWAYS( iCleanDfc && iCleanDataDfc, EIADMemoryAllocationFailure | EIADFaultIdentifier21 << KFaultIdentifierShift );

    C_TRACE( ( _T( "DISAUserChannel::DISAUserChannel <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_DISAUSERCHANNEL_EXIT, "<DISAUserChannel::DISAUserChannel" );
    }

void DISAUserChannel::MsgQueFunc(
        TAny* aPtr
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_MSGQUEFUNC_ENTRY, ">DISAUserChannel::MsgQueFunc;aPtr=%x", ( TUint )( aPtr ) );

    DISAUserChannel* ch = reinterpret_cast<DISAUserChannel*>( aPtr );
    ch->HandleMsg( ch->iMsgQue.iMessage );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_MSGQUEFUNC_EXIT, "<DISAUserChannel::MsgQueFunc" );
    }

DISAUserChannel::~DISAUserChannel(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_DISAUSERCHANNEL_ENTRY, ">DISAUserChannel::~DISAUserChannel" );
    
    C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel 0x%x 0x%x ->" ), iChannelNumber, this ) );
    OstTraceExt5( TRACE_NORMAL, DISAUSERCHANNEL_DISAUSERCHANNEL, "DISAUserChannel::~DISAUserChannel;iEmptyRxQueueDfc=%x;iEmptyDataRxQueueDfc=%x;iConnectionStatusDfc=%x;iFlowCtrlStatusDfc=%x;iCompleteChannelRequestDfc=%x", (TUint)iEmptyRxQueueDfc, (TUint)iEmptyDataRxQueueDfc, (TUint)iConnectionStatusDfc, (TUint)iFlowCtrlStatusDfc, (TUint)iCompleteChannelRequestDfc );
    if( iEmptyRxQueueDfc )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iEmptyRxQueueDfc 0x%x" ), iEmptyRxQueueDfc ) );
        iEmptyRxQueueDfc->Cancel();
        delete iEmptyRxQueueDfc;
        iEmptyRxQueueDfc = NULL;
        }
    if( iEmptyDataRxQueueDfc )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iEmptyDataRxQueueDfc 0x%x" ), iEmptyDataRxQueueDfc ) );
        iEmptyDataRxQueueDfc->Cancel();
        delete iEmptyDataRxQueueDfc;
        iEmptyDataRxQueueDfc = NULL;
        }
    if( iConnectionStatusDfc )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iConnectionStatusDfc 0x%x" ), iConnectionStatusDfc ) );
        iConnectionStatusDfc->Cancel();
        delete iConnectionStatusDfc;
        iConnectionStatusDfc = NULL;
        }
    if( iFlowCtrlStatusDfc )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iFlowCtrlStatusDfc 0x%x" ), iFlowCtrlStatusDfc ) );
        iFlowCtrlStatusDfc->Cancel();
        delete iFlowCtrlStatusDfc;
        iFlowCtrlStatusDfc = NULL;
        }
    if( iCompleteChannelRequestDfc )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iCompleteChannelRequestDfc 0x%x" ), iCompleteChannelRequestDfc ) );
        iCompleteChannelRequestDfc->Cancel();
        delete iCompleteChannelRequestDfc;
        iCompleteChannelRequestDfc = NULL;
        }
    OstTraceExt4( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_DISAUSERCHANNEL, "DISAUserChannel::~DISAUserChannel;iRx=%x;iDataRx=%x;iRxDeAllocate=%x;iDataRxDeAllocate=%x", (TUint)iRx, (TUint)iDataRx, (TUint)iRxDeAllocate, (TUint)iDataRxDeAllocate );
    
    if( iRx )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iRx 0x%x" ), iRx ) );
        delete iRx;
        iRx = NULL;
        }
    if( iDataRx )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iDataRx 0x%x" ), iDataRx ) );
        delete iDataRx;
        iDataRx = NULL;
        }
    if( iRxDeAllocate )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iRxDeAllocate 0x%x" ), iRxDeAllocate ) );
        delete iRxDeAllocate;
        iRxDeAllocate = NULL;
        }
    if( iDataRxDeAllocate )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iDataRxDeAllocate 0x%x" ), iDataRxDeAllocate ) );
        delete iDataRxDeAllocate;
        iDataRxDeAllocate = NULL;
        }
    // Not owned
    OstTraceExt4( TRACE_NORMAL, DUP9_DISAUSERCHANNEL_DISAUSERCHANNEL, "DISAUserChannel::~DISAUserChannel;iIADConnectionStatusPtr=%x;iIADFlowControlStatusPtr=%x;iReceiveBufPtr=%x;iDataReceiveBufPtr=%x", (TUint)iIADConnectionStatusPtr, (TUint)iIADFlowControlStatusPtr, (TUint)iReceiveBufPtr, (TUint)iDataReceiveBufPtr );
    
    if( iIADConnectionStatusPtr )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iIADConnectionStatusPtr 0x%x" ), iIADConnectionStatusPtr ) );
        iIADConnectionStatusPtr = NULL;
        }
    if( iIADFlowControlStatusPtr )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iIADFlowControlStatusPtr 0x%x" ), iIADFlowControlStatusPtr ) );
        iIADFlowControlStatusPtr = NULL;
        }
    if( iReceiveBufPtr )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iReceiveBufPtr 0x%x" ), iReceiveBufPtr ) );
        iReceiveBufPtr = NULL;
        }
    if( iDataReceiveBufPtr )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iDataReceiveBufPtr 0x%x" ), iDataReceiveBufPtr ) );
        iDataReceiveBufPtr = NULL;
        }

    OstTraceExt4( TRACE_NORMAL, DUP10_DISAUSERCHANNEL_DISAUSERCHANNEL, "DISAUserChannel::~DISAUserChannel;iNeededBufLen=%x;iNeededDataBufLen=%x;iIADUserChannelNumberPtr=%x;iReqQueue=%x", (TUint)iNeededBufLen, (TUint)iNeededDataBufLen, (TUint)iIADUserChannelNumberPtr, (TUint)iReqQueue );

    
    if( iNeededBufLen )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iNeededBufLen 0x%x" ), iNeededBufLen ) );
        iNeededBufLen = NULL;
        }
    if( iNeededDataBufLen )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iNeededDataBufLen 0x%x" ), iNeededDataBufLen ) );
        iNeededDataBufLen = NULL;
        }
    if( iIADUserChannelNumberPtr )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iIADUserChannelNumberPtr 0x%x" ), iIADUserChannelNumberPtr ) );
        iIADUserChannelNumberPtr = NULL;
        }
    
    if( iReqQueue )
       {
       delete iReqQueue;
       iReqQueue = NULL;
       }
    iPep = NULL;
    iExtensionApi = NULL;
    // owned
    if( iChFastMutex )
        {
        C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel iChFastMutex" ) ) );
        delete iChFastMutex;
        iChFastMutex = NULL;
        }
    Kern::SafeClose( reinterpret_cast<DObject*&>(iThread), NULL );
    C_TRACE( ( _T( "DISAUserChannel::~DISAUserChannel 0x%x 0x%x <-" ), iChannelNumber, this ) );

    }

TInt DISAUserChannel::DoCreate(
        TInt, //aUnit,          // Used to hold channelid now in anInfo instead no need to extension device
        const TDesC8* anInfo,   // Channelid now in anInfo instead no need to extension device 
        const TVersion& // aVer    // Not used at the moment.
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_DOCREATE_ENTRY, "DISAUserChannel::DoCreate;anInfo=%x;this=%x", ( TUint )( anInfo ), (TUint)(this) );

    C_TRACE( ( _T( "DISAUserChannel::DoCreate 0x%x 0x%x ->" ), iChannelNumber, anInfo ) );
    if( !Kern::CurrentThreadHasCapability( ECapabilityCommDD, __PLATSEC_DIAGNOSTIC_STRING( "Check by: ISAAccessDriver" ) ) ) return KErrPermissionDenied;  
    ASSERT_RESET_ALWAYS( anInfo, EIADChannelNumberNotSpecifiedInInfo | EIADFaultIdentifier1 << KFaultIdentifierShift );
    // Check for channel number inside anInfo.
    ASSERT_RESET_ALWAYS( anInfo->Length() > 0 , EIADOverTheLimits | EIADFaultIdentifier39 << KFaultIdentifierShift );                                                
    TUint16 channel = static_cast<TUint16>( ( *anInfo )[ 0 ] );
    ASSERT_RESET_ALWAYS( ( channel < EIADNokiaLastUserChannel ),EIADWrongParameter | EIADFaultIdentifier19 << KFaultIdentifierShift );
    iChannelNumber = ~channel; // In user thread context thread in CS, cannot be pre-empted.
    C_TRACE( ( _T( "DISAUserChannel::DoCreate channelnumber 0x%x 0x%x" ), iChannelNumber, this ) );
    iRx = new DQueue( KIADLddRxQueuSize );
    iDataRx =new DQueue( KIADLddDataRxQueuSize );
    iReqQueue = new DReqQueue();
    ASSERT_RESET_ALWAYS( iRx && iDataRx && iReqQueue, EIADMemoryAllocationFailure | EIADFaultIdentifier22 << KFaultIdentifierShift );
    iRxDeAllocate = new DQueue( KIADLddRxQueuSize );
    iDataRxDeAllocate = new DQueue( KIADLddDataRxQueuSize );
    // Changed to extension DFC to guarantee that extension is handled only by one thread extensionDFC.
    // Other DFC functions handling user<->kernel copying done in ldd DFC.
    SetDfcQ( DIsaAccessExtension::GetDFCThread( EIADExtensionDfcQueue ) );// this DFCFunction prio is now 1 by the LDD framework if needed increase?
    iMsgQ.Receive();
    iMsgQue.SetDfcQ( DIsaAccessExtension::GetDFCThread( EIADLddDfcQueue ) );
    iMsgQue.Receive();
    DObject* thread = reinterpret_cast<DObject*>( iThread );
    // Open is needed to increase ref count to calling thread that is decreased in Kern::SafeClose
    // Possible returns KErrNone ? KErrGeneral
    TInt threadOpen( thread->Open() );
    TRACE_ASSERT_INFO( threadOpen == KErrNone, (TUint8)iChannelNumber << KChannelNumberShift );
    C_TRACE( ( _T( "DISAUserChannel::DoCreate 0x%x %d <-" ), iChannelNumber, threadOpen ) );

    OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_DOCREATE_EXIT, "<DISAUserChannel::DoCreate;iChannelNumber=%hx;this=%x;retVal=%d", iChannelNumber, (TUint)this, threadOpen );    
    return threadOpen;
    }

void DISAUserChannel::HandleMsg
        (
        TMessageBase* aMsg
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_HANDLEMSG_ENTRY, ">DISAUserChannel::HandleMsg;aMsg=%x", ( TUint )( aMsg ) );

    C_TRACE( ( _T( "DISAUserChannel::HandleMsg 0x%x->" ), aMsg ) );
    TThreadMessage& m= *( static_cast< TThreadMessage* >( aMsg ) );
    TInt id( m.iValue );
    if( static_cast<TInt>( ECloseMsg ) == id )
        {
        C_TRACE( ( _T( "DISAUserChannel::HandleMsg ECloseMsg 0x%x" ), iChannelNumber ) );
        OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_HANDLEMSG, "DISAUserChannel::HandleMsg ECloseMsg;iChannelNumber=%hx", iChannelNumber );
        
        m.Complete( HandleSyncRequest( EIADClose, NULL ), EFalse );
        }
    else if( KMaxTInt == id )
        {
        C_TRACE( ( _T( "DISAUserChannel::HandleMsg cancel" ) ) );
        OstTrace0( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_HANDLEMSG, "DISAUserChannel::HandleMsg cancel" );
        
        DoCancel( id, m.Int0() );
        m.Complete( KErrNone, ETrue );
        }
    else
        {
        ASSERT_RESET_ALWAYS( ( KErrNotFound < id ), EIADWrongRequest | EIADFaultIdentifier5 << KFaultIdentifierShift );
        C_TRACE( ( _T( "DISAUserChannel::HandleMsg completeValue == KErrNone" ) ) );
        OstTrace1( TRACE_NORMAL, DUP2_DISAUSERCHANNEL_HANDLEMSG, "DISAUserChannel::HandleMsg;id=%d", id );
        
        TInt completeValue( KErrNone );
        TInt ulen( KErrNotFound );
        switch ( id )
            {
            case EIADClose:
            case EIADGetConnectionStatus:
            case EIADGetMaxDataSize:
            case EIADResetQueues:
            case EIADGetFlowControlStatus:
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
            case EIADReturnLoan:
#endif
                {
                ulen = KNoParams;
                break;
                }
            case EIADAsyncClose:
            case EIADSend:
            case EIADDataSend:
            case EIADSubscribeIndications:
            case EIADSendIndication:
            case EIADSubscribeIndications32Bit:
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
            case EIADLoan:
#endif
                {
                ulen = KOneParam;
                break;
                }
            case EIADAsyncNotifyFlowControlStatus:
            case EIADAsyncNotifyConnectionStatus:
            case EIADAsyncSend:
            case EIADAsyncDataSend:
            case EIADAsyncSubscribeIndications:
            case EIADAsyncSendIndication:
            case EIADAsyncSubscribeIndications32Bit:
            //case EIADAsyncFTDInformation:
                {
                ulen = KTwoParams;
                break;
                }
            case EIADAsyncOpen:
            case EIADAsyncReceive:
            case EIADAsyncDataReceive:
                {
                ulen = KThreeParams;
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier6 << KFaultIdentifierShift );
                break;
                }
            }
        TUint32* table[ KThreeParams ];
        completeValue = Kern::ThreadRawRead( iThread, m.Ptr0(), table, ulen * sizeof( TAny* ) );
        if( completeValue == KErrNone )
            {
            switch( id )
                {
                // All asynchronous requests.
                case EIADAsyncOpen:
                case EIADAsyncNotifyConnectionStatus:
                case EIADAsyncClose:
                case EIADAsyncReceive:
                case EIADAsyncDataReceive:
                case EIADAsyncNotifyFlowControlStatus:
                case EIADAsyncSend:
                case EIADAsyncDataSend:
                case EIADAsyncSubscribeIndications:
                case EIADAsyncSendIndication:
                case EIADAsyncSubscribeIndications32Bit:
                //case EIADAsyncFTDInformation:
                    {
                    // No need to check return value in async functions, completed to user
                    HandleAsyncRequest( id, table );
                    break;
                    }
                case EIADClose:
                case EIADSend:
                case EIADDataSend:
                case EIADSubscribeIndications:
                case EIADSendIndication:
                case EIADGetMaxDataSize:
                case EIADSubscribeIndications32Bit:
                case EIADGetConnectionStatus:
                case EIADResetQueues:
                case EIADGetFlowControlStatus:
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
                case EIADLoan:
                case EIADReturnLoan:
#endif
                    {
                    completeValue = HandleSyncRequest( id, table );
                    break;
                    }
                default:
                    {
                    ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier7 << KFaultIdentifierShift );
                    break;
                    }
                }
            }
        m.Complete( completeValue, ETrue );
        }
    C_TRACE( ( _T( "DISAUserChannel::HandleMsg <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_HANDLEMSG_EXIT, "<DISAUserChannel::HandleMsg");
    }

TInt DISAUserChannel::Request(
        TInt aReqNo,
        TAny* a1,
        TAny* //a2
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_REQUEST_ENTRY, "ZDISAUserChannel::Request;aReqNo=%d;a1=%x", aReqNo, ( TUint )( a1 ) );

    C_TRACE( ( _T( "DISAUserChannel::Request %d, 0x%x, 0x%x 0x%x->" ), aReqNo, a1, iChannelNumber ) );
    ASSERT_RESET_ALWAYS( aReqNo >= ( TInt ) EMinRequestId, EIADWrongRequest | EIADFaultIdentifier8 << KFaultIdentifierShift );
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
    ASSERT_RESET_ALWAYS( aReqNo <= EIADReturnLoan || aReqNo == KMaxTInt, 
                                            EIADWrongRequest | EIADFaultIdentifier1 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( ( iChannelNumber < EIADNokiaLastUserChannel || EIADAsyncOpen == aReqNo || EIADLoan == aReqNo ),
                                               EIADWrongParameter | EIADFaultIdentifier18 << KFaultIdentifierShift );
#else
    ASSERT_RESET_ALWAYS( aReqNo <= EIADDeAllocateBlock || aReqNo == KMaxTInt, 
                                            EIADWrongRequest | EIADFaultIdentifier1 << KFaultIdentifierShift );
    ASSERT_RESET_ALWAYS( ( iChannelNumber < EIADNokiaLastUserChannel || EIADAsyncOpen == aReqNo ),
                                               EIADWrongParameter | EIADFaultIdentifier18 << KFaultIdentifierShift );
#endif
    TInt result( KErrNotFound );
    // All request go now in kernel context: easies synchronization between isa access threads.
    // We have the option still to use user context here. (CAREFULL IF DOING SO WITH DATA CORRUPTION)
    // Give to kernel address space in EXT DFC context.
    // Demand paging: if needed change this DFC to LDD DFC and
    // give the read data from LDD DFC to EXT DFC via another SendReceive (MessageQue).
    TThreadMessage& m=Kern::Message();
    m.iValue = aReqNo;
    m.iArg[ KFirstParam ] = a1;
    m.iArg[ KSecondParam ] = NULL;
    // Own kernelmessage queu for receive and datareceive which are not handling shareddata below channel (in extension).
    if( aReqNo == EIADAsyncReceive || aReqNo == EIADAsyncDataReceive )
        {
        result = m.SendReceive( &iMsgQue );
        }
    else
        {
        result = m.SendReceive( &iMsgQ );
        }
    C_TRACE( ( _T( "DISAUserChannel::Request %d, 0x%x, 0x%x %d <-" ), aReqNo, a1, iChannelNumber, result ) );
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_REQUEST, "<DISAUserChannel::Request;ret=%d", result );
    return result;     
    }

///// Functions from MChannelCallback start (from extension binary)
void DISAUserChannel::CompleteChannelRequest(
        TInt aRequest,
        TInt aStatusToComplete
        )
    {
    OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_COMPLETECHANNELREQUEST_ENTRY, ">DISAUserChannel::CompleteChannelRequest;aRequest=%d;aStatusToComplete=%d;iChannelNumber=%hx", aRequest, aStatusToComplete, iChannelNumber );

    C_TRACE( ( _T( "DISAUserChannel::CompleteChannelRequest %d %d 0x%x ->" ), aRequest, aStatusToComplete, iChannelNumber ) );
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    TIADReq req( static_cast<TIADAsyncRequest>( aRequest ), aStatusToComplete );
    if( iReqQueue->Add( req ) )
        {
        TRACE_ASSERT_INFO( !iCompleteChannelRequestDfc->Queued(), (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)aRequest );
        iCompleteChannelRequestDfc->Enque();
        }
    C_TRACE( ( _T( "DISAUserChannel::CompleteChannelRequest %d %d 0x%x <-" ), aRequest, aStatusToComplete, iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_COMPLETECHANNELREQUEST_EXIT, "<DISAUserChannel::CompleteChannelRequest" );
    }

void DISAUserChannel::CompleteChReqDfc(
        TAny* aPtr
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_COMPLETECHREQDFC_ENTRY, ">DISAUserChannel::CompleteChReqDfc;aPtr=%x", ( TUint )( aPtr ) );

    C_TRACE( ( _T( "DISAUserChannel::CompleteChReqDfc ->" ) ) );
    // Make sure that user side is accessed only by ldd DFCThread.
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    ASSERT_DFCTHREAD_INLDD();
    DISAUserChannel* chPtr = reinterpret_cast<DISAUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISAUserChannel::CompleteChReqDfc 0x%x" ), chPtr->iChannelNumber ) );
    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_COMPLETECHREQDFC, "DISAUserChannel::CompleteChReqDfc;iChannelNumber=%hx", chPtr->iChannelNumber );
    
    TIADReq requ = chPtr->iReqQueue->Get();
    if( EIADAsyncOpen == requ.iRequest )
        {
        C_TRACE( ( _T( "DISAUserChannel::CompleteChReqDfc req complete open" ) ) );
        OstTraceExt1( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_COMPLETECHREQDFC, "DISAUserChannel::CompleteChReqDfc;iChannelNumber=%hx", chPtr->iChannelNumber );
        
        TAny* tempUserChNmbrPtr( chPtr->iIADUserChannelNumberPtr );
        // Check of KErrNone and KErrInUse (same object same id open) and in only those cases ~iCh = ~~iCh kernel already has.
        TUint16 chNumber( ( KErrNone == requ.iCompleteStatus || KErrInUse == requ.iCompleteStatus ? ~chPtr->iChannelNumber : KNotInitializedChannel ) );
        chPtr->iChannelNumber = chNumber;
        ASSERT_RESET_ALWAYS( tempUserChNmbrPtr, 0 );
        ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( chPtr->iThread, tempUserChNmbrPtr, &chNumber, sizeof(TUint16), chPtr->iThread ), 
                                        EIADDesWriteFailed | ( TUint8 )( chPtr->iChannelNumber ) << KChannelNumberShift );
        }
    // Request was already pending..
    TRequestStatus* status( chPtr->iReqQueue->GetReq( requ.iRequest ) );
    Kern::RequestComplete( chPtr->iThread, status, requ.iCompleteStatus );
    C_TRACE( ( _T( "DISAUserChannel::CompleteChReqDfc req complete %d stat %d" ), requ.iRequest, requ.iCompleteStatus ) );
    OstTraceExt2( TRACE_NORMAL, DUP2_DISAUSERCHANNEL_COMPLETECHREQDFC, "DISAUserChannel::CompleteChReqDfc complete %x stat %x", requ.iRequest, requ.iCompleteStatus );
    
    chPtr->iReqQueue->SetReq( requ.iRequest, NULL );
    if( !chPtr->iReqQueue->Empty() )
        {
        CompleteChReqDfc( chPtr );
        }
    C_TRACE( ( _T( "DISAUserChannel::CompleteChReqDfc <-" ), chPtr->iChannelNumber  ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_COMPLETECHREQDFC_EXIT, "<DISAUserChannel::CompleteChReqDfc" );
    }


void DISAUserChannel::NotifyConnectionStatus(
        TIADConnectionStatus aStatus    // Connection status to be written to client.
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_NOTIFYCONNECTIONSTATUS_ENTRY, ">DISAUserChannel::NotifyConnectionStatus;aStatus=%x", ( TUint )&( aStatus ) );

    C_TRACE( ( _T( "DISAUserChannel::NotifyConnectionStatus 0x%x %d ->" ), iChannelNumber, iConnectionStatus ) );
    // Atomic STR no need to synch.
    iConnectionStatus = aStatus;    
    ASSERT_RESET_ALWAYS( !iConnectionStatusDfc->Queued(), EIADDFCAlreadyQueued | EIADFaultIdentifier2 << KFaultIdentifierShift );
    iConnectionStatusDfc->Enque();
    C_TRACE( ( _T( "DISAUserChannel::NotifyConnectionStatus 0x%x %d <-" ), iChannelNumber, iConnectionStatus ) );

    OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_NOTIFYCONNECTIONSTATUS_EXIT, "<DISAUserChannel::NotifyConnectionStatus;iChannelNumber=%hx;iConnectionStatus=%d", iChannelNumber, iConnectionStatus );
    }

void DISAUserChannel::ConnectionStatusDfc(
        TAny* aPtr
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_CONNECTIONSTATUSDFC_ENTRY, "DISAUserChannel::ConnectionStatusDfc;aPtr=%x", ( TUint )( aPtr ) );

    C_TRACE( ( _T( "DISAUserChannel::NotifyConnectionStatusDfc ->" ) ) );
    ASSERT_DFCTHREAD_INLDD();
    DISAUserChannel* chPtr = reinterpret_cast<DISAUserChannel*>( aPtr );
    if( chPtr->iIADConnectionStatusPtr )
        {
        
        TIADReq req( EIADAsyncNotifyConnectionStatus, KErrNone );
        if( chPtr->iReqQueue->Add( req ) )
           {
           TInt temp( chPtr->iConnectionStatus );
           C_TRACE( ( _T( "DISAUserChannel::NotifyConnectionStatusDfc 0x%x request active, writing %d" ), chPtr->iChannelNumber, temp ) );
           OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_CONNECTIONSTATUSDFC, "DISAUserChannel::ConnectionStatusDfc;chPtr->iChannelNumber=%hx;temp=%d", chPtr->iChannelNumber, temp );
           
           ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( chPtr->iThread, chPtr->iIADConnectionStatusPtr, &temp, sizeof(TUint16), chPtr->iThread ), 
                                        EIADDesWriteFailed | ( TUint8 )( chPtr->iChannelNumber ) << KChannelNumberShift | EIADFaultIdentifier1 << KFaultIdentifierShift );
           TRACE_ASSERT_INFO( !chPtr->iCompleteChannelRequestDfc->Queued(), ( TUint8 )( chPtr->iChannelNumber ) << KChannelNumberShift );
           CompleteChReqDfc( chPtr );
           }
        }
    C_TRACE( ( _T( "DISAUserChannel::NotifyConnectionStatusDfc 0x%x <-" ), chPtr->iChannelNumber ) );

    OstTraceExt1( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_CONNECTIONSTATUSDFC_EXIT, "<DISAUserChannel::ConnectionStatusDfc;chPtr->iChannelNumber=%hx", chPtr->iChannelNumber );
    }

void DISAUserChannel::NotifyFlowCtrl(
        TIADFlowControlStatus aStatus
        )
    {

    OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_NOTIFYFLOWCTRL_ENTRY, "DISAUserChannel::NotifyFlowCtrl;aStatus=%x;iChannelNumber=%hx;iFlowCtrlStatus=%d", ( TUint )( aStatus ), iChannelNumber, iFlowCtrlStatus );
    C_TRACE( ( _T( "DISAUserChannel::NotifyFlowCtrl 0x%x %d > %d ->" ), iChannelNumber, iFlowCtrlStatus, aStatus ) );
    // Atomic STR no need to synch.
    iFlowCtrlStatus = aStatus;
    TRACE_WARNING( !iFlowCtrlStatusDfc->Queued(), iChannelNumber );
    iFlowCtrlStatusDfc->Enque();
    C_TRACE( ( _T( "DISAUserChannel::NotifyFlowCtrl 0x%x %d %d <-" ), iChannelNumber, iFlowCtrlStatus, aStatus ) );
    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_NOTIFYFLOWCTRL_EXIT, "<DISAUserChannel::NotifyFlowCtrl" );

    }

void DISAUserChannel::FlowCtrlStatusDfc(
        TAny* aPtr
        )
    {

    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_FLOWCTRLSTATUSDFC_ENTRY, ">DISAUserChannel::FlowCtrlStatusDfc;aPtr=%x", ( TUint )( aPtr ) );
    DISAUserChannel* chPtr = reinterpret_cast<DISAUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISAUserChannel::FlowCtrlStatusDfc 0x%x ->" ), chPtr->iChannelNumber ) );
    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_FLOWCTRLSTATUSDFC, "DISAUserChannel::FlowCtrlStatusDfc;chPtr->iChannelNumber=%hx", chPtr->iChannelNumber );
    ASSERT_DFCTHREAD_INLDD();
    if( chPtr->iIADFlowControlStatusPtr &&
      ( chPtr->iLastNotifiedFlowCtrlStatus != chPtr->iFlowCtrlStatus ) )
        {
        TIADReq req( EIADAsyncNotifyFlowControlStatus, KErrNone );
        if( chPtr->iReqQueue->Add( req ) )
           {
            TInt temp( chPtr->iFlowCtrlStatus );
            C_TRACE( ( _T( "DISAUserChannel::FlowCtrlStatusDfc 0x%x request active, writing %d" ), chPtr->iChannelNumber, temp ) );
            OstTraceExt2( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_FLOWCTRLSTATUSDFC, "DISAUserChannel::FlowCtrlStatusDfc;chPtr->iChannelNumber=%hx;writing iFlowCtrlStatus=%d", chPtr->iChannelNumber, temp );
            
            ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( chPtr->iThread, chPtr->iIADFlowControlStatusPtr, &temp, sizeof(TUint16), chPtr->iThread ), 
                                        EIADDesWriteFailed | ( TUint8 )( chPtr->iChannelNumber ) << KChannelNumberShift | EIADFaultIdentifier2 << KFaultIdentifierShift );
            TRACE_ASSERT( !chPtr->iCompleteChannelRequestDfc->Queued() );
            CompleteChReqDfc( chPtr );
            chPtr->iLastNotifiedFlowCtrlStatus = temp;
            }
        }
    C_TRACE( ( _T( "DISAUserChannel::FlowCtrlStatusDfc 0x%x <-" ), chPtr->iChannelNumber ) );
    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_FLOWCTRLSTATUSDFC_EXIT, "<DISAUserChannel::FlowCtrlStatusDfc" );

    }

void DISAUserChannel::ReceiveDataMsg(
        const TDesC8& aDataMessage
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_RECEIVEDATAMSG_ENTRY, "DISAUserChannel::ReceiveDataMsg;aDataMessage=%x", ( TUint )&( aDataMessage ) );

    C_TRACE( ( _T( "DISAUserChannel::ReceiveDataMsg ->" ) ) );
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    iDataRx->Add( aDataMessage );
    iEmptyDataRxQueueDfc->Enque();
    C_TRACE( ( _T( "DISAUserChannel::ReceiveDataMsg <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_RECEIVEDATAMSG_EXIT, "<DISAUserChannel::ReceiveDataMsg" );
    }

void DISAUserChannel::ReceiveMsg(
        const TDesC8& aMessage
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_RECEIVEMSG_ENTRY, ">DISAUserChannel::ReceiveMsg;aMessage=%x;iChannelNumber=%hx", ( TUint )&( aMessage ), iChannelNumber );

    C_TRACE( ( _T( "DISAUserChannel::ReceiveMsg 0x%x 0x%x ->" ), &aMessage, iChannelNumber ) );
    // Can only be called from thread context.
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    iRx->Add( aMessage );
    iEmptyRxQueueDfc->Enque();
    C_TRACE( ( _T( "DISAUserChannel::ReceiveMsg 0x%x 0x%x <-" ), &aMessage, iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_RECEIVEMSG_EXIT, "<DISAUserChannel::ReceiveMsg" );
    }

void DISAUserChannel::RegisterPep(
        MDataMessageStatus& aPep
        )
    {
OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_REGISTERPEP_ENTRY, ">DISAUserChannel::RegisterPep;aPep=%x;iChannelNumber=%hx", ( TUint )&( aPep ), iChannelNumber );

    C_TRACE( ( _T( "DISAUserChannel::RegisterPep 0x%x 0x%x ->" ), iPep, iChannelNumber ) );
    iPep = &aPep;
    C_TRACE( ( _T( "DISAUserChannel::RegisterPep 0x%x 0x%x <-" ), iPep, iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_REGISTERPEP, "<DISAUserChannel::RegisterPep" );
    }

///// Functions from MChannelCallback end (from extension binary)


void DISAUserChannel::EmptyRxQueueDfc(
        TAny* aPtr // Pointer to self
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYRXQUEUEDFC_ENTRY, ">DISAUserChannel::EmptyRxQueueDfc;aPtr=%x", ( TUint )( aPtr ) );

    ASSERT_DFCTHREAD_INLDD();
    DISAUserChannel& chTmp = *reinterpret_cast<DISAUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc 0x%x ->" ), chTmp.iChannelNumber ) );
    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYRXQUEUEDFC, "DISAUserChannel::EmptyRxQueueDfc;iChannelNumber=%hx", chTmp.iChannelNumber );
    
    if( ( chTmp.iReqQueue->GetReq( EIADAsyncReceive ) ) && ( chTmp.iRx->Count() > 0 ) )
        {
        TDes8& tmpDes = chTmp.iRx->Get();
        TInt desMaxLen( Kern::ThreadGetDesMaxLength( chTmp.iThread, chTmp.iReceiveBufPtr ) );
        // If user descriptor legth is enough and length get not failed.
        if( desMaxLen >= tmpDes.Length() )
            {
            // Write to user address space (iReceiveBufPtr) the content of tmpDes starting from zero offset as 8bit descriptor data.
            TInt writeError( Kern::ThreadDesWrite( chTmp.iThread, chTmp.iReceiveBufPtr, tmpDes, KDestStartOffset, KChunkShiftBy0, chTmp.iThread ) );
            if( writeError == KErrNone )
                {
                C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc write 0x%x k->u 0x%x ok 0x%x" ), &tmpDes, chTmp.iReceiveBufPtr, chTmp.iChannelNumber ) );
                OstTraceExt3( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_EMPTYRXQUEUEDFC, "DISAUserChannel::EmptyRxQueueDfc write ok;src=%x;dest=%x;iChannelNumber=%hx", (TUint)&(tmpDes), (TUint)(chTmp.iReceiveBufPtr), chTmp.iChannelNumber );
                
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncReceive ), writeError );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                chTmp.iRxDeAllocate->Add( tmpDes );
                chTmp.iCleanDfc->Enque();// If same prio in ldd & extension dfc threads ok, scheduled after this dfc run and now context switch happens from ldd->ext->ldd
                }
            // Write unsuccesfull don't deallocate the block.
            else
                {
                C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc write 0x%x k->u 0x%x NOK, reason %d 0x%x" ), &tmpDes, chTmp.iReceiveBufPtr, writeError, chTmp.iChannelNumber ) );
                OstTraceExt4( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYRXQUEUEDFC_WRITE_FAIL, "DISAUserChannel::EmptyRxQueueDfc write fail;src=%x;dest=%x;writeError=%d;iChannelNumber=%hx", (TUint)&(tmpDes), (TUint)(chTmp.iReceiveBufPtr), writeError, chTmp.iChannelNumber );
                
                TRACE_ASSERT_INFO( 0, ( TUint8 )( chTmp.iChannelNumber ) << KChannelNumberShift | (TUint16)writeError );
                // Roll the message back to que.
                chTmp.iRx->RollBack( tmpDes );
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncReceive ), writeError );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                }
            }
        // If descriptor invalid.
        else
            {
            TRACE_WARNING( 0, (TUint8)chTmp.iChannelNumber << KChannelNumberShift | (TUint16)tmpDes.Length() );
            // Roll the message back to que.
            chTmp.iRx->RollBack( tmpDes );
            // If invalid content.
            if( KErrBadDescriptor == desMaxLen )
                {
                TRACE_ASSERT_INFO( 0, (TUint8)chTmp.iChannelNumber << KChannelNumberShift | (TUint16)desMaxLen );
                C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc invalid descriptor 0x%x" ), chTmp.iChannelNumber ) );
                OstTraceExt1( TRACE_NORMAL, DUP2_DISAUSERCHANNEL_EMPTYRXQUEUEDFC, "DISAUserChannel::EmptyRxQueueDfc KErrBadDescriptor;iChannelNumber=%hx", chTmp.iChannelNumber );
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncReceive ), KErrBadDescriptor );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                }
            // If length too small (if other assert).
            else
                {
                TUint16 neededLength( tmpDes.Length() );
                TRACE_WARNING( 0, (TUint8)chTmp.iChannelNumber << KChannelNumberShift | (TUint16)desMaxLen );
                C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc descriptor length too small length %d needed %d 0x%x" ), desMaxLen, neededLength, chTmp.iChannelNumber ) );
                OstTraceExt3( TRACE_NORMAL, DUP3_DISAUSERCHANNEL_EMPTYRXQUEUEDFC, "DISAUserChannel::EmptyRxQueueDfc too small length;desMaxLen=%d;neededLength=%hu;iChannelNumber=%hx", desMaxLen, neededLength, chTmp.iChannelNumber );
                
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( chTmp.iThread, chTmp.iNeededBufLen, &neededLength, sizeof(TUint16), chTmp.iThread ), 
                                        EIADDesWriteFailed | ( TUint8 )( chTmp.iChannelNumber ) << KChannelNumberShift | EIADFaultIdentifier3 << KFaultIdentifierShift );
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncReceive ), KErrNoMemory );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                }
            }
        }
    else
        {
        C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc 0x%x no receive active or no message" ), chTmp.iChannelNumber ) );
        OstTraceExt1( TRACE_NORMAL, DUP4_DISAUSERCHANNEL_EMPTYRXQUEUEDFC, "DISAUserChannel::EmptyRxQueueDfc no receive active or no message;iChannelNumber=%hx", chTmp.iChannelNumber );
        
        }
    C_TRACE( ( _T( "DISAUserChannel::EmptyRxQueueDfc 0x%x <-" ), chTmp.iChannelNumber ) );

    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYRXQUEUEDFC_EXIT, "<DISAUserChannel::EmptyRxQueueDfc;iChannelNumber=%hx", (TUint16)chTmp.iChannelNumber );
    }

void DISAUserChannel::CleanBlocksDfc(
        TAny* aPtr // Pointer to self
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_CLEANBLOCKSDFC_ENTRY, "DISAUserChannel::CleanBlocksDfc;aPtr=%x", ( TUint )( aPtr ) );

    DISAUserChannel* chTmp = reinterpret_cast<DISAUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISAUserChannel::CleanBlocksDfc 0x%x ->" ), chTmp->iChannelNumber ) );
    while( chTmp->iRxDeAllocate->Count() > 0 )
        {
        TDes8& blockToDeAllocate = chTmp->iRxDeAllocate->Get();
        C_TRACE( ( _T( "DISAUserChannel::CleanBlocksDfc dealloc 0x%x" ), &blockToDeAllocate ) );
        OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_CLEANBLOCKSDFC, "DISAUserChannel::CleanBlocksDfc;blockToDeAllocate=%x;iChannelNumber=%hx", (TUint)&(blockToDeAllocate), chTmp->iChannelNumber );
               
        
        chTmp->iExtensionApi->DeAllocateBlock( blockToDeAllocate );
        }
    C_TRACE( ( _T( "DISAUserChannel::CleanBlocksDfc 0x%x <-" ), chTmp->iChannelNumber ) );

    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_CLEANBLOCKSDFC_EXIT, "<DISAUserChannel::CleanBlocksDfc;iChannelNumber=%hx", chTmp->iChannelNumber );
    }

void DISAUserChannel::CleanDataBlocksDfc(
        TAny* aPtr // Pointer to self
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_CLEANDATABLOCKSDFC_ENTRY, ">DISAUserChannel::CleanDataBlocksDfc;aPtr=%x", ( TUint )( aPtr ) );

    DISAUserChannel* chTmp = reinterpret_cast<DISAUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISAUserChannel::CleanDataBlocksDfc 0x%x ->" ), chTmp->iChannelNumber ) );
    while( chTmp->iDataRxDeAllocate->Count() > 0 )
        {
        TDes8& blockToDeAllocate = chTmp->iDataRxDeAllocate->Get();
        C_TRACE( ( _T( "DISAUserChannel::CleanDataBlocksDfc dealloc 0x%x" ), &blockToDeAllocate ) );
        OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_CLEANDATABLOCKSDFC, "DISAUserChannel::CleanDataBlocksDfc;blockToDeAllocate=%x;iChannelNumber=%hx", (TUint)&(blockToDeAllocate), chTmp->iChannelNumber );
        
        chTmp->iExtensionApi->DeAllocateBlock( blockToDeAllocate );
        ASSERT_RESET_ALWAYS( chTmp->iPep, EIADNullParameter | EIADFaultIdentifier31 << KFaultIdentifierShift  );
        chTmp->iPep->DataMessageDelivered();
        }
    C_TRACE( ( _T( "DISAUserChannel::CleanDataBlocksDfc 0x%x <-" ), chTmp->iChannelNumber ) );

    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_CLEANDATABLOCKSDFC_EXIT, "<DISAUserChannel::CleanDataBlocksDfc;iChannelNumber=%hx", chTmp->iChannelNumber);
    }

void DISAUserChannel::EmptyDataRxQueueDfc(
        TAny* aPtr // Pointer to self
        )
    {
    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC_ENTRY, "DISAUserChannel::EmptyDataRxQueueDfc;aPtr=%x", ( TUint )( aPtr ) );

    ASSERT_DFCTHREAD_INLDD();
    DISAUserChannel& chTmp = *reinterpret_cast<DISAUserChannel*>( aPtr );
    C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc 0x%x ->" ), chTmp.iChannelNumber ) );
    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC, "DISAUserChannel::EmptyDataRxQueueDfc;iChannelNumber=%hx", chTmp.iChannelNumber );
    
    if( ( chTmp.iReqQueue->GetReq( EIADAsyncDataReceive ) ) && ( chTmp.iDataRx->Count() > 0 ) )
        {
        TDes8& tmpDes = chTmp.iDataRx->Get();
        TInt tmpLength( tmpDes.Length() - KPipeDataHeader );
        C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc tmpDes %d %d %d" ), tmpDes.Length(), tmpLength, tmpDes.MaxLength() ) );
        OstTraceExt3( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC, "DISAUserChannel::EmptyDataRxQueueDfc;srcLength()=%d;srcCopyLength=%d;srcMaxLength()=%d", tmpDes.Length(), tmpLength, tmpDes.MaxLength() );
        TInt desMaxLen( Kern::ThreadGetDesMaxLength( chTmp.iThread, chTmp.iDataReceiveBufPtr ) );
        // If user descriptor legth is enough and length get not failed.
        if( desMaxLen >= tmpLength )
            {
            if ( ( chTmp.iChannelNumber  == EIscNokiaNifPep0        ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep1      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep2      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep3      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep4      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep5      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep6      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep7      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep8      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep9      ||
                     chTmp.iChannelNumber  == EIscNokiaNifPep10 )   &&
                     ( tmpLength > 1500 ) )
                {
                TInt fault_code = 0;
                fault_code = fault_code | (TUint16)(chTmp.iChannelNumber);
                fault_code = fault_code << 16;
                fault_code = fault_code | (TUint16)(tmpLength);
                Kern::Fault("FATAL ERROR: Nif length over 1500 bytes [chn][len]", fault_code);
                }

            TUint8* tmpPtr( const_cast<TUint8*>( tmpDes.Ptr() + KPipeDataHeader ) );
            TPtr8 tmp( tmpPtr, tmpLength, tmpLength );
            TInt writeError( Kern::ThreadDesWrite( chTmp.iThread, chTmp.iDataReceiveBufPtr, tmp, KDestStartOffset, KChunkShiftBy0, chTmp.iThread ) );
            if( writeError == KErrNone )
                {
                C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc write 0x%x k->u 0x%x ok 0x%x" ), &tmpDes, chTmp.iDataReceiveBufPtr, chTmp.iChannelNumber ) );
                OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC__WRITE_OK, "DISAUserChannel::EmptyDataRxQueueDfc write ok;src=%x;dest=%x;iChannelNumber=%hx", (TUint)&tmpDes, (TUint)chTmp.iDataReceiveBufPtr, chTmp.iChannelNumber );
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncDataReceive ), writeError );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                chTmp.iDataRxDeAllocate->Add( tmpDes );
                if( chTmp.iDataRx->Count() == 0 )
                    {
                    chTmp.iCleanDataDfc->Enque();// If same prio in ldd & extension dfc threads ok, scheduled after this dfc run and now context switch happens from ldd->ext->ldd
                    }
                }
            // Write unsuccesfull don't deallocate the block.
            else
                {
                C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc write 0x%x k->u 0x%x NOK, reason %d 0x%x" ), &tmpDes, chTmp.iDataReceiveBufPtr, writeError, chTmp.iChannelNumber ) );
                OstTraceExt4( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC_WRITE_FAIL, "DISAUserChannel::EmptyDataRxQueueDfc;src=%x;dest=%x;writeError=%d;iChannelNumber=%hx", (TUint)&tmpDes, (TUint)chTmp.iDataReceiveBufPtr, writeError, chTmp.iChannelNumber );
                TRACE_ASSERT_INFO( 0, ( TUint8 )( chTmp.iChannelNumber ) << KChannelNumberShift | (TUint16)tmpLength );
                // Roll the message back to que.
                chTmp.iDataRx->RollBack( tmpDes );
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncDataReceive ), writeError );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                }
            }
        // If descriptor invalid.
        else
            {
            TRACE_WARNING( 0, (TUint8)chTmp.iChannelNumber << KChannelNumberShift | (TUint16)tmpLength );
            // Roll the message back to que.
            chTmp.iDataRx->RollBack( tmpDes );
            // If invalid content.
            if( KErrBadDescriptor == desMaxLen )
                {
                TRACE_ASSERT_INFO( 0, (TUint8)chTmp.iChannelNumber << KChannelNumberShift );
                C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc invalid descriptor 0x%x" ), chTmp.iChannelNumber ) );
                OstTraceExt1( TRACE_NORMAL, DUP2_DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC, "DISAUserChannel::EmptyDataRxQueueDfc invalid descriptor;iChannelNumber=%hx", chTmp.iChannelNumber );
                
                // CHANGE: call now straight a way in LDD thread context the complete without enqueing the dfc function inbetween.
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncDataReceive ), KErrBadDescriptor );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                }
            // If length too small (if other assert).
            else
                {
                TUint16 neededLength( tmpLength );
                TRACE_WARNING( 0, (TUint8)chTmp.iChannelNumber << KChannelNumberShift | (TUint16)desMaxLen );
                C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc descriptor length too small length %d needed %d 0x%x" ), desMaxLen, neededLength, chTmp.iChannelNumber ) );
                OstTraceExt3( TRACE_NORMAL, DUP3_DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC, "DISAUserChannel::EmptyDataRxQueueDfc descriptor length too small;desMaxLen=%d;neededLength=%hu;iChannelNumber=%hx", desMaxLen, neededLength, chTmp.iChannelNumber );
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( chTmp.iThread, chTmp.iNeededDataBufLen, &neededLength, sizeof(TUint16), chTmp.iThread ), 
                                        EIADDesWriteFailed | ( TUint8 )( chTmp.iChannelNumber ) << KChannelNumberShift | EIADFaultIdentifier4 << KFaultIdentifierShift );
                TIADReq req( static_cast<TIADAsyncRequest>( EIADAsyncDataReceive ), KErrNoMemory );
                if( chTmp.iReqQueue->Add( req ) )
                    {
                    TRACE_ASSERT( !chTmp.iCompleteChannelRequestDfc->Queued() );
                    CompleteChReqDfc( &chTmp );
                    }
                }
            }
        }
    else
        {
        C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc 0x%x no datareceive active or no message" ), chTmp.iChannelNumber ) );
        OstTraceExt1( TRACE_NORMAL, DUP4_DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC, "DISAUserChannel::EmptyDataRxQueueDfc no datareceive active or no message;iChannelNumber=%hx", chTmp.iChannelNumber );
        }
    C_TRACE( ( _T( "DISAUserChannel::EmptyDataRxQueueDfc 0x%x <-" ), chTmp.iChannelNumber ) );
    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_EMPTYDATARXQUEUEDFC_EXIT, "<DISAUserChannel::EmptyDataRxQueueDfc;chTmp.iChannelNumber=%hx", chTmp.iChannelNumber );

    }

void DISAUserChannel::ResetQueues(
        // None
        )
    {
    OstTraceExt1( TRACE_NORMAL, DISAUSERCHANNEL_RESETQUEUES_ENTRY, ">DISAUserChannel::ResetQueues;iChannelNumber=%hx", iChannelNumber );
    C_TRACE( ( _T( "DISAUserChannel::ResetQueues 0x%x ->" ), iChannelNumber ) );
    OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_RESETQUEUES, "DISAUserChannel::ResetQueues Rx;iChannelNumber=%hx;iRx=%x;iDataRx=%x", iChannelNumber, (TUint)iRx, (TUint)iDataRx );
    if( iRx )
        {
        C_TRACE( ( _T( "DISAUserChannel::ResetQueues 0x%x iRx 0x%x" ), iChannelNumber, iRx ) );
        while( iRx->Count() )
            {
            iExtensionApi->DeAllocateBlock( iRx->Get() );
            }
        }
    if( iDataRx )
        {
        C_TRACE( ( _T( "DISAUserChannel::ResetQueues 0x%x iDataRx 0x%x" ), iChannelNumber, iDataRx ) );
        while( iDataRx->Count() )
            {
            iExtensionApi->DeAllocateBlock( iDataRx->Get() );
            }
        }
    //PERF
    if( iRxDeAllocate )
        {
        C_TRACE( ( _T( "DISAUserChannel::ResetQueues 0x%x iRxDeAllocate 0x%x" ), iChannelNumber, iDataRx ) );
        while( iRxDeAllocate->Count() )
            {
            iExtensionApi->DeAllocateBlock( iRxDeAllocate->Get() );
            }
        }
    //PERF
    //PERF
    if( iDataRxDeAllocate )
        {
        C_TRACE( ( _T( "DISAUserChannel::ResetQueues 0x%x iDataRxDeAllocate 0x%x" ), iChannelNumber, iDataRx ) );
        while( iDataRxDeAllocate->Count() )
            {
            iExtensionApi->DeAllocateBlock( iDataRxDeAllocate->Get() );
            }
        }
    //PERF

    C_TRACE( ( _T( "DISAUserChannel::ResetQueues 0x%x <-" ), iChannelNumber ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_RESETQUEUES_EXIT, "<DISAUserChannel::ResetQueues" );
    }

void DISAUserChannel::HandleAsyncRequest(
        TInt aRequest,
        TAny* a1
        )
    {
    OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_HANDLEASYNCREQUEST_ENTRY, "DISAUserChannel::HandleAsyncRequest;aRequest=%d;a1=%x;iChannelNumber=%hx", aRequest, ( TUint )( a1 ), iChannelNumber );

    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest ->" ) ) );
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
    TRACE_ASSERT_INFO( tablePtr[ KFirstParam ], (TUint16)aRequest );
    TRequestStatus* requestStatus = reinterpret_cast<TRequestStatus*>( tablePtr[ KFirstParam ] );
    // If request already active.
    if( ( iReqQueue->GetReq( static_cast< TIADAsyncRequest >( aRequest ) ) ) )
        {
        // Fault if request is already pending and the request status pointer is different.
        // Fault prints 0-7bits: request, 8-15bits: ch number, 16-31bits: fault identifier
        C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest Channel 0x%x setting request %d active twice, illegal" ), iChannelNumber, aRequest ) );
        OstTraceExt2( TRACE_NORMAL, DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest double request;iChannelNumber=%hx;aRequest=%d", iChannelNumber, aRequest );
        
        // Assert cause request already active.
        TRACE_ASSERT_INFO( 0, ( EIADSameRequestTwice | static_cast<TUint8>( iChannelNumber ) << KChannelNumberShift | static_cast<TUint8>( aRequest ) << KExtraInfoShift ) );
        // Active object should not give same request object twice before completing the first one.
        ASSERT_PANIC_USER_THREAD_ALWAYS( iReqQueue->GetReq( static_cast< TIADAsyncRequest >( aRequest ) ) == requestStatus, iThread,
                                         ( EIADSameRequestTwice | static_cast<TUint8>( iChannelNumber ) << KChannelNumberShift | static_cast<TUint8>( aRequest ) << KExtraInfoShift ) );
        ////////////////// NOTE!!! CHANGE TO API SPECIFICATION !!!!!!!!!!!!!!!!! //////////////////////////
        ////////////////// PREVIOUSLY KErrAlreadyExists was returned !!!!!!!!!!!!!!!!! //////////////////////////
        }
    else
        {
        iReqQueue->SetReq( static_cast< TIADAsyncRequest >( aRequest ), requestStatus );
        switch ( aRequest )
            {
            case EIADAsyncOpen:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncOpen" ) ) );
                OstTrace0( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncOpen" );
                
                iIADUserChannelNumberPtr = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                TAny* thirdParam = reinterpret_cast<TAny*>( tablePtr[ KThirdParam ] );
                // If resource used, if not length is zero.
                TInt openInfoLength = Kern::ThreadGetDesLength( iThread, thirdParam );
                // Resource used in channel opening.
                if( openInfoLength > 0 )
                    {
                    C_TRACE( ( _T( "DIscChannel::HandleAsyncRequest EIADAsyncOpen resource" ) ) );
                    // In channel opening we can use dynamic allocation no speed requirements for open.
                    OstTrace0( TRACE_NORMAL, DUP2_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncOpen resource" );                    
                    TUint8* buffer = reinterpret_cast<TUint8*>( Kern::Alloc( openInfoLength ) );
                    TPtr8* bufferPtr = new TPtr8( buffer, openInfoLength );
                    ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, thirdParam, *bufferPtr, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier1 << KFaultIdentifierShift );
                    // Must be called in extension context.
                    iExtensionApi->Open( ~iChannelNumber, aRequest, *bufferPtr, this );
                    Kern::Free( buffer );
                    delete bufferPtr;
                    bufferPtr = NULL;
                    }
                // Resource not used in channel opening.
                else
                    {
                     C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncOpen normal" ) ) );
                    // Must be called in extension context.
                    iExtensionApi->Open( ~iChannelNumber, aRequest, this );
                    }
                break;
                }
            case EIADAsyncClose:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncClose" ) ) );
                OstTraceExt1( TRACE_NORMAL, DUP3_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncClose;iChannelNumber=%hx", iChannelNumber );
                
                ResetQueues();
                if( KNotInitializedChannel != iChannelNumber )
                    {                
                    // Cancel any outstanding request.
                    // Remember in asynch close not to cancel it when closing.
                    for( TInt i( 0 ); i < EIADAsyncLast; ++i )
                        {
                        if( iReqQueue->GetReq( static_cast< TIADAsyncRequest >( i ) ) && EIADAsyncClose != i )
                            {
                            C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncClose req to cancel %d" ), i ) );
                            OstTraceExt1( TRACE_NORMAL, DUP17_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncClose;ReqToCancel=%hx", i );
                            
                            DoCancel( KMaxTInt, i );
                            }
                        }
                    // Must be called in extension context.
                    iExtensionApi->Close( iChannelNumber );
                    }
                CompleteChannelRequest( EIADAsyncClose, KErrNone );
                break;
                }
            case EIADAsyncReceive:
                {
                iReceiveBufPtr = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                iNeededBufLen = reinterpret_cast<TUint16*>( tablePtr[ KThirdParam ] );
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncReceive 0x%x 0x%x" ), iNeededBufLen, iReceiveBufPtr ) );
                OstTraceExt2( TRACE_NORMAL, DUP4_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncReceive;iNeededBufLen=%x;iReceiveBufPtr=%x", (TUint)iNeededBufLen, (TUint)iReceiveBufPtr );                
                iEmptyRxQueueDfc->Enque();
                break;
                }
            case EIADAsyncNotifyConnectionStatus:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncNotifyConnectionStatus" ) ) );
                iIADConnectionStatusPtr = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncNotifyConnectionStatus TBR 0x%x" ), iIADConnectionStatusPtr ) );
                OstTrace1( TRACE_NORMAL, DUP5_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncNotifyConnectionStatus;iIADConnectionStatusPtr=%x", iIADConnectionStatusPtr );                
                break;
                }
            case EIADAsyncDataReceive:
                {
                iDataReceiveBufPtr = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                iNeededDataBufLen = reinterpret_cast<TUint16*>( tablePtr[ KThirdParam ] );
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncDataReceive 0x%x 0x%x" ), iNeededDataBufLen, iDataReceiveBufPtr ) );
                OstTraceExt2( TRACE_NORMAL, DUP7_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncDataReceive;iNeededDataBufLen=%x;iDataReceiveBufPtr=%x", (TUint)iNeededDataBufLen, (TUint)iDataReceiveBufPtr );                
                iEmptyDataRxQueueDfc->Enque();
                break;
                }
            case EIADAsyncNotifyFlowControlStatus:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncNotifyFlowControlStatus" ) ) );
                iIADFlowControlStatusPtr = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncNotifyFlowControlStatus TBR 0x%x" ), iIADFlowControlStatusPtr ) );
                OstTrace1( TRACE_NORMAL, DUP8_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncNotifyFlowControlStatus;iIADFlowControlStatusPtr=%x", iIADFlowControlStatusPtr );
                
                if ( iLastNotifiedFlowCtrlStatus != iFlowCtrlStatus )
                    {
                    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest iFlowCtrlStatus changed!" ) ) );
                    OstTraceExt2( TRACE_NORMAL, DUP6_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest flow control status changed;iLastNotifiedFlowCtrlStatus=%d;iFlowCtrlStatus=%d", iLastNotifiedFlowCtrlStatus, iFlowCtrlStatus );
                    
                    TRACE_ASSERT( !iFlowCtrlStatusDfc->Queued() );
                    iFlowCtrlStatusDfc->Enque();
                    }
                break;
                }
            case EIADAsyncSend:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSend" ) ) );
                TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
                TAny* secondParam = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                ASSERT_RESET_ALWAYS( secondParam, EIADNullParameter | EIADFaultIdentifier23 << KFaultIdentifierShift  );
                TInt msgLength( Kern::ThreadGetDesLength( iThread, secondParam ) );
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSend 0x%x %d" ), secondParam, msgLength ) );
                OstTraceExt2( TRACE_NORMAL, DUP10_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncSend;secondParam=%x;msgLength=%d", (TUint)secondParam, msgLength );
                
                TInt error( KErrBadDescriptor );
                if( msgLength > 0 )
                    {
                    // Previously there were Kern::Alloc (allocating from kernel memory a block where copy from user was made no only
                    // one copy from user to allocated block that is to be send.
                    TDes8& sendBlock = iExtensionApi->AllocateBlock( msgLength );
                    ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, secondParam, sendBlock, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier2 << KFaultIdentifierShift );
                    TRACE_ASSERT_INFO( sendBlock.Length() == msgLength, (TUint8)iChannelNumber << KChannelNumberShift );
                    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSend 0x%x 0x%x" ), &sendBlock, iChannelNumber ) );
                    OstTraceExt2( TRACE_NORMAL, DUP16_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncSend;sendBlock=%x;iChannelNumber=%hx", (TUint)&sendBlock, iChannelNumber );
                    
                    error = iExtensionApi->SendMessage( sendBlock, iChannelNumber );
                    }
                else
                    {
                    error = ( msgLength == KErrNone ) ? KErrBadDescriptor : msgLength;
                    TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)msgLength );
                    }
                CompleteChannelRequest( aRequest, error );
                break;
                }
            case EIADAsyncDataSend:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncDataSend" ) ) );
                TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
                TAny* secondParam = reinterpret_cast<TAny*>( tablePtr[ KSecondParam ] );
                ASSERT_RESET_ALWAYS( secondParam, EIADNullParameter | EIADFaultIdentifier24 << KFaultIdentifierShift  );
                TInt msgLength( Kern::ThreadGetDesLength( iThread, secondParam ) );
                C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADAsyncDataSend 0x%x %d" ), secondParam, msgLength ) );
                OstTraceExt2( TRACE_NORMAL, DUP9_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncDataSend;secondParam=%x;msgLength=%d", (TUint)secondParam, msgLength );                
                TInt error( KErrBadDescriptor );
                if( msgLength > KErrNone )
                    {
                    // Allocate a block for data. Allocation adds +11 to the msglength for pipe headers
                    TDes8& sendBlock = iExtensionApi->AllocateDataBlock( msgLength );
                    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncDataSend max %d length %d" ), sendBlock.MaxLength(), sendBlock.Length() ) );
                    // Use pre-allocated TPtr (no memory allocation from heap nor stack).
                    // Set it to point to +11 from sendblock and to be as long as data to be send
                    TInt tmpMaxLength( sendBlock.MaxLength() - KPipeDataHeader );
                    TUint8* tmpPtr( const_cast<TUint8*>( ( sendBlock.Ptr() + KPipeDataHeader ) ) );
                    TPtr* test = static_cast<TPtr8*>( &sendBlock );
                    test->Set( tmpPtr, 0, tmpMaxLength );
                    ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, secondParam, *test, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier3 << KFaultIdentifierShift );
                    TInt tmpMaxLength2( sendBlock.MaxLength() + KPipeDataHeader );
                    TInt tmpLength2( sendBlock.Length() + KPipeDataHeader );
                    TUint8* tmpPtr2( const_cast<TUint8*>( ( sendBlock.Ptr() - KPipeDataHeader ) ) );
                    test->Set( tmpPtr2, tmpLength2, tmpMaxLength2 );
                    TRACE_ASSERT_INFO( sendBlock.Length() > msgLength, (TUint8)iChannelNumber << KChannelNumberShift );
                    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncDataSend 0x%x 0x%x" ), &sendBlock, iChannelNumber ) );
                    OstTraceExt4( TRACE_NORMAL, DUP15_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncDataSend;sendBlock=%x;sendBlock.MaxLength()=%d;sendBlock.Length()=%d;iChannelNumber=%hx", (TUint)&sendBlock, sendBlock.MaxLength(), sendBlock.Length(), iChannelNumber );
                    
                    error = iExtensionApi->SendMessage( sendBlock, iChannelNumber );
                    }
                else
                    {
                    error = ( msgLength == KErrNone ) ? KErrBadDescriptor : msgLength;
                    TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)msgLength );
                    }
                CompleteChannelRequest( aRequest, error );
                break;
                }
            case EIADAsyncSubscribeIndications:
            case EIADAsyncSubscribeIndications32Bit:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSubscribeIndications | EIADAsyncSubscribeIndications32Bit" ) ) );
                TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
                TAny* secondParam = reinterpret_cast<TAny*>(tablePtr[ KSecondParam ]);
                ASSERT_RESET_ALWAYS( secondParam, EIADNullParameter | EIADFaultIdentifier25 << KFaultIdentifierShift  );
                TInt orderLength( Kern::ThreadGetDesLength( iThread, secondParam ) );
                C_TRACE( ( _T( "DISAUserChannel::DISAUserChannel::HandleAsyncRequest EIADAsyncSubscribeIndications | EIADAsyncSubscribeIndications32Bit 0x%x %d" ), secondParam, orderLength ) );
                OstTraceExt2( TRACE_NORMAL, DUP12_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncSubscribeIndications(8/32bit);secondParam=%x;orderLength=%d", (TUint)secondParam, orderLength );
                
                TInt error( KErrBadDescriptor );
                if( orderLength > 0 )
                    {
                    // Previously there were Kern::Alloc (allocating from kernel memory a block where copy from user was made no only
                    // one copy from user to allocated block that is to be send.
                    TDes8& sendBlock = iExtensionApi->AllocateBlock( ( orderLength ) );
                    ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, secondParam, sendBlock, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier4 << KFaultIdentifierShift );
                    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSubscribeIndications | EIADAsyncSubscribeIndications32Bit 0x%x 0x%x" ), &sendBlock, iChannelNumber ) );
                    OstTraceExt2( TRACE_NORMAL, DUP14_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncSubscribeIndications(8/32bit);sendBlock=%x;iChannelNumber=%hx", (TUint)&sendBlock, iChannelNumber );
                    
                    TRACE_ASSERT_INFO( sendBlock.Length() == orderLength, (TUint8)iChannelNumber << KChannelNumberShift );
                    // No return values check needed. Request completed with error value by multiplexer
                    TBool thirtyTwoBit( ( EIADSubscribeIndications32Bit == aRequest ) ? ETrue : EFalse );
                    error = iExtensionApi->OrderIndication( sendBlock, iChannelNumber, thirtyTwoBit );
                    iExtensionApi->DeAllocateBlock( sendBlock );
                    }
                else
                    {
                    error = orderLength;
                    TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)error );                    
                    }
                CompleteChannelRequest( aRequest, error );
                break;
                }
            case EIADAsyncSendIndication:
                {
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSendIndication" ) ) );
                TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
                TAny* secondParam = reinterpret_cast<TAny*>(tablePtr[ KSecondParam ]);
                ASSERT_RESET_ALWAYS( secondParam, EIADNullParameter | EIADFaultIdentifier26 << KFaultIdentifierShift  );
                TInt msgLength( Kern::ThreadGetDesLength( iThread, secondParam ) );
                C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSendIndication TBR 0x%x %d" ), secondParam, msgLength ) );
                OstTraceExt2( TRACE_NORMAL, DUP11_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncSendIndication;secondParam=%x;msgLength=%d", (TUint)secondParam, msgLength );                
                TInt error( KErrBadDescriptor );
                if( msgLength > 0 )
                    {
                    // Previously there were Kern::Alloc (allocating from kernel memory a block where copy from user was made now only
                    // one copy from user to allocated block that is to be send.
                    TDes8& sendBlock = iExtensionApi->AllocateBlock( msgLength );
                    ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, secondParam, sendBlock, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier5 << KFaultIdentifierShift );
                    TRACE_ASSERT_INFO( sendBlock.Length() == msgLength, (TUint8)iChannelNumber << KChannelNumberShift );
                    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest EIADAsyncSendIndication 0x%x %d %d 0x%x" ), &sendBlock, sendBlock.Length(), msgLength, iChannelNumber ) );
                    OstTraceExt5( TRACE_NORMAL, DUP13_DISAUSERCHANNEL_HANDLEASYNCREQUEST, "DISAUserChannel::HandleAsyncRequest EIADAsyncSendIndication;sendBlock=%x;sendBlock.Length()=%d;sendBlock.MaxLength()=%d;msgLength=%d;iChannelNumber=%hx", (TUint)&sendBlock, sendBlock.Length(), sendBlock.MaxLength(), msgLength, iChannelNumber );
                    
                    error = iExtensionApi->SendIndication( sendBlock, iChannelNumber );
                    }
                else
                    {
                    error = msgLength;
                    TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)error );
                    }
                CompleteChannelRequest( aRequest, error );
                break;
                }
            default:
                {
                ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier10 << KFaultIdentifierShift );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DISAUserChannel::HandleAsyncRequest <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_HANDLEASYNCREQUEST_EXIT, "<DISAUserChannel::HandleAsyncRequest" );
    }

TInt DISAUserChannel::HandleSyncRequest(
        TInt aRequest,
        TAny* a1
        )
    {
    OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_HANDLESYNCREQUEST_ENTRY, "DISAUserChannel::HandleSyncRequest;aRequest=%d;a1=%x;iChannelNumber=%hx", aRequest, ( TUint )( a1 ), iChannelNumber );

    C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest ->" ) ) );
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0 );
    TInt error( KErrNotSupported );
    switch( aRequest )
        {
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
        case EIADLoan:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADLoan" ) ) );
            OstTrace0( TRACE_NORMAL, DUY1_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADLoan" );
            TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );;
            iIADUserChannelNumberPtr = reinterpret_cast<TAny*>( tablePtr[ KFirstParam ] );
            ASSERT_RESET_ALWAYS( tablePtr[ KFirstParam ], EIADNullParameter | EIADFaultIdentifier27 << KFaultIdentifierShift );
            error = iExtensionApi->Loan( ~iChannelNumber, aRequest, this );
            TAny* tempUserChNmbrPtr( iIADUserChannelNumberPtr );
            // Check of KErrNone and KErrInUse (same object same id open) and in only those cases ~iCh = ~~iCh kernel already has.
            TUint16 chNumber( ( KErrNone == error ? ~iChannelNumber : KNotInitializedChannel ) );
            iChannelNumber = chNumber;
            ASSERT_RESET_ALWAYS( tempUserChNmbrPtr, 0 );
            // NOTE!!!!!!!! THIS MUST BE SET TO OWN DFC DUE TO DP, NOT URGENT CAUSE CALLER MOST LIKELY NOT SWAPPED. CHANGE REQUIRES LOAN TO BECOME ASYNC.
            ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadRawWrite( iThread, tempUserChNmbrPtr, &chNumber, sizeof(TUint16), iThread ), 0 );
            break;
            }
        case EIADReturnLoan:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADReturnLoan" ) ) );
            OstTrace0( TRACE_NORMAL, DUY2_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADReturnLoan" );
// OLD WORKS, no cancellation of requests error = iExtensionApi->ReturnLoan( iChannelNumber, aRequest, this );
            // NEW
            ResetQueues();
            if( KNotInitializedChannel != iChannelNumber )
                {
                // Cancel any outstanding request. // remember in asynch close not to close it asynhronously
                for( TInt i( 0 ); i < EIADAsyncLast; ++i )
                    {
                    if( iReqQueue->GetReq( static_cast< TIADAsyncRequest >( i ) ) )
                        {
                        C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADReturnLoan req to cancel %d" ), i ) );
                        OstTrace1( TRACE_NORMAL, DUY3_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADReturnLoan;reqToCancel=%d", i );
                        DoCancel( KMaxTInt, i );
                        }
                    }
                // Must be called in extension context.
                error = iExtensionApi->ReturnLoan( iChannelNumber, aRequest, this );
                }
            //NEW
            break;
            }
#endif
        case EIADClose:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADClose" ) ) );
            OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADClose" );
            ResetQueues();
            if( KNotInitializedChannel != iChannelNumber )
                {
                // Cancel any outstanding request. // remember in asynch close not to close it asynhronously
                for( TInt i( 0 ); i < EIADAsyncLast; ++i )
                    {
                    if( iReqQueue->GetReq( static_cast< TIADAsyncRequest >( i ) ) )
                        {
                        C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADClose req to cancel %d" ), i ) );
                        OstTrace1( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADClose;reqToCancel=%d", i );
                        DoCancel( KMaxTInt, i );
                        }
                    }
                // Must be called in extension context.
                iExtensionApi->Close( iChannelNumber );
                error = KErrNone;
                }
           break;
            }
        case EIADSend:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSend" ) ) );
            TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
            TAny* firstParam = reinterpret_cast<TAny*>(tablePtr[ KFirstParam ]);
            ASSERT_RESET_ALWAYS( firstParam, EIADNullParameter | EIADFaultIdentifier27 << KFaultIdentifierShift  );
            TInt msgLength( Kern::ThreadGetDesLength( iThread, firstParam ) );
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSend 0x%x %d" ), firstParam, msgLength ) );
            OstTraceExt2( TRACE_NORMAL, DUP2_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADSend;firstParam=%x;msgLength=%d", (TUint)firstParam, msgLength );
            
            if( msgLength > 0 )
                {
                // Previously there were Kern::Alloc (allocating from kernel memory a block where copy from user was made no only
                // one copy from user to allocated block that is to be send.
                TDes8& sendBlock = iExtensionApi->AllocateBlock( msgLength );
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, firstParam, sendBlock, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier6 << KFaultIdentifierShift );
                TRACE_ASSERT_INFO( sendBlock.Length() == msgLength, (TUint8)iChannelNumber << KChannelNumberShift );
                C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSend 0x%x 0x%x" ), &sendBlock, iChannelNumber ) );
                OstTraceExt2( TRACE_NORMAL, DUP13_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADSend;sendBlock=%x;iChannelNumber=%hx", (TUint)&sendBlock, iChannelNumber );
                
                error = iExtensionApi->SendMessage( sendBlock, iChannelNumber );
                }
            else
                {
                error = ( msgLength == KErrNone ) ? KErrBadDescriptor : msgLength;
                TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)msgLength );
                }
            break;
            }
        case EIADDataSend:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADDataSend" ) ) );
            TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
            TAny* firstParam = reinterpret_cast<TAny*>(tablePtr[ KFirstParam ]);
            ASSERT_RESET_ALWAYS( firstParam, EIADNullParameter | EIADFaultIdentifier32 << KFaultIdentifierShift  );
            TInt msgLength( Kern::ThreadGetDesLength( iThread, firstParam ) );
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADDataSend 0x%x %d" ), firstParam, msgLength ) );
            OstTraceExt2( TRACE_NORMAL, DUP3_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADDataSend;firstParam=%x;msgLength=%d", (TUint)firstParam, msgLength );            
            if( msgLength > 0 )
                {
                // Previously there were Kern::Alloc (allocating from kernel memory a block where copy from user was made no only
                // one copy from user to allocated block that is to be send.
                
                // Allocate a block for data. Allocation adds +11 to the msglength for pipe headers
                TDes8& sendBlock = iExtensionApi->AllocateDataBlock( msgLength );
                C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest max %d length %d" ), sendBlock.MaxLength(), sendBlock.Length() ) );
                // Use pre-allocated TPtr (no memory allocation from heap nor stack).
                // Set it to point to +11 from sendblock and to be as long as data to be send
                TInt tmpMaxLength( sendBlock.MaxLength() - KPipeDataHeader );
                TUint8* tmpPtr( const_cast<TUint8*>( ( sendBlock.Ptr() + KPipeDataHeader ) ) );
                TPtr* test = static_cast<TPtr8*>( &sendBlock );
                test->Set( tmpPtr, 0, tmpMaxLength );
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, firstParam, *test, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier7 << KFaultIdentifierShift );
                TInt tmpMaxLength2( sendBlock.MaxLength() + KPipeDataHeader );
                TInt tmpLength2( sendBlock.Length() + KPipeDataHeader );
                TUint8* tmpPtr2( const_cast<TUint8*>( ( sendBlock.Ptr() - KPipeDataHeader ) ) );
                test->Set( tmpPtr2, tmpLength2, tmpMaxLength2 );
                TRACE_ASSERT_INFO( sendBlock.Length() > msgLength, (TUint8)iChannelNumber << KChannelNumberShift );
                C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADDataSend 0x%x 0x%x" ), &sendBlock, iChannelNumber ) );
                OstTraceExt4( TRACE_NORMAL, DUP4_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADDataSend;sendBlock=%x;Length=%x;MaxLength=%x;iChannelNumber=%hx", (TUint)&sendBlock, sendBlock.Length(), sendBlock.MaxLength(), iChannelNumber );
                
                error = iExtensionApi->SendMessage( sendBlock, iChannelNumber );
                }
            else
                {
                error = ( msgLength == KErrNone ) ? KErrBadDescriptor : msgLength;
                TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)msgLength );
                }
            break;
            }
        case EIADSubscribeIndications:
        case EIADSubscribeIndications32Bit:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSubscribeIndications | EIADSubscribeIndications32Bit" ) ) );
            TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
            TAny* firstParam = reinterpret_cast<TAny*>(tablePtr[ KFirstParam ]);
            ASSERT_RESET_ALWAYS( firstParam, EIADNullParameter | EIADFaultIdentifier29 << KFaultIdentifierShift  );
            TInt orderLength( Kern::ThreadGetDesLength( iThread, firstParam ) );
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSubscribeIndications | EIADSubscribeIndications32Bit 0x%x %d" ), firstParam, orderLength ) );
            OstTraceExt2( TRACE_NORMAL, DUP5_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADSubscribeIndications(8/32bit);firstParam=%x;orderLength=%d", (TUint)firstParam, orderLength );
            
            if( orderLength > KErrNone )
                {
                // Previously there were Kern::Alloc (allocating from kernel memory a block where copy from user was made no only
                // one copy from user to allocated block that is to be send.
                TDes8& sendBlock = iExtensionApi->AllocateBlock( ( orderLength ) );
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, firstParam, sendBlock, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier8 << KFaultIdentifierShift );
                C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSubscribeIndications 0x%x 0x%x" ), &sendBlock, iChannelNumber ) );
                OstTraceExt2( TRACE_NORMAL, DUP7_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADSubscribeIndications(8/32bit);sendBlock=%x;iChannelNumber=%hx", (TUint)&sendBlock, iChannelNumber );                
                TRACE_ASSERT_INFO( sendBlock.Length() == orderLength, (TUint8)iChannelNumber << KChannelNumberShift );
                // No return values check needed. Request completed with error value by multiplexer
                TBool thirtyTwoBit( ( EIADSubscribeIndications32Bit == aRequest ) ? ETrue : EFalse );
                error = iExtensionApi->OrderIndication( sendBlock, iChannelNumber, thirtyTwoBit );
                iExtensionApi->DeAllocateBlock( sendBlock );
                }
            else
                {
                error = orderLength;
                TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)error);
                }
            break;
            }
        case EIADSendIndication:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSendIndication" ) ) );
            TUint32* tablePtr = reinterpret_cast<TUint32*>( a1 );
            TAny* firstParam = reinterpret_cast<TAny*>(tablePtr[ KFirstParam ]);
            ASSERT_RESET_ALWAYS( firstParam, EIADNullParameter | EIADFaultIdentifier30 << KFaultIdentifierShift  );
            TInt msgLength( Kern::ThreadGetDesLength( iThread, firstParam ) );
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSendIndication TBR 0x%x %d" ), firstParam, msgLength ) );
            OstTraceExt2( TRACE_NORMAL, DUP6_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADSendIndication;firstParam=%x;msgLength=%d", (TUint)firstParam, msgLength );
            if( msgLength > KErrNone )
                {
                // Previously there were Kern::Alloc (allocating from kernel memory a block where copy from user was made now only
                // one copy from user to allocated block that is to be send.
                TDes8& sendBlock = iExtensionApi->AllocateBlock( msgLength );
                ASSERT_RESET_ALWAYS( KErrNone == Kern::ThreadDesRead( iThread, firstParam, sendBlock, 0, KChunkShiftBy0 ), EIADDesReadFailed | EIADFaultIdentifier9 << KFaultIdentifierShift );
                TRACE_ASSERT_INFO( sendBlock.Length() == msgLength, (TUint8)iChannelNumber << KChannelNumberShift );
                C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADSendIndication 0x%x %d %d 0x%x" ), &sendBlock, sendBlock.Length(), msgLength, iChannelNumber ) );
                OstTraceExt4( TRACE_NORMAL, DUP8_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADSendIndication;sendBlock=%x;sendBlock.Length()=%d;msgLength=%d;iChannelNumber=%hx", (TUint)&sendBlock, sendBlock.Length(), msgLength, iChannelNumber );
                // No return values check needed. Request completed with error value by multiplexer
                error = iExtensionApi->SendIndication( sendBlock, iChannelNumber );
                }
            else
                {
                error = msgLength;
                TRACE_ASSERT_INFO( 0, (TUint8)iChannelNumber << KChannelNumberShift | (TUint16)error );
                }
            break;
            }
        case EIADGetMaxDataSize:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADGetMaxDataSize" ) ) );
            OstTrace0( TRACE_NORMAL, DUP9_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADGetMaxDataSize" );            
            error = iExtensionApi->GetMaxDataSize();
            break;
            }
        case EIADGetConnectionStatus:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADGetConnectionStatus" ) ) );
            OstTrace0( TRACE_NORMAL, DUP10_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADGetConnectionStatus" );
            
            error = iExtensionApi->GetConnectionStatus();
            break;
            }
        case EIADResetQueues:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADResetQueues" ) ) );
            OstTrace0( TRACE_NORMAL, DUP11_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADResetQueues" );
            
            ResetQueues();
            error = KErrNone;
            break;
            }
        case EIADGetFlowControlStatus:
            {
            C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest EIADGetFlowControlStatus" ) ) );
            OstTrace0( TRACE_NORMAL, DUP12_DISAUSERCHANNEL_HANDLESYNCREQUEST, "DISAUserChannel::HandleSyncRequest EIADGetFlowControlStatus" );
            
            error = iFlowCtrlStatus;
            iLastNotifiedFlowCtrlStatus = error;            
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier11 << KFaultIdentifierShift );
            break;
            }
        }
    C_TRACE( ( _T( "DISAUserChannel::HandleSyncRequest %d <-" ), error ) );

    OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_HANDLESYNCREQUEST_EXIT, "<DISAUserChannel::HandleSyncRequest;error=%d", error );
    return error;
    }

void DISAUserChannel::DoCancel(
        TInt aRequest,
        TInt aMask )
    {
OstTraceExt3( TRACE_NORMAL, DISAUSERCHANNEL_DOCANCEL_ENTRY, ">DISAUserChannel::DoCancel;aRequest=%d;aMask=%d;iChannelNumber=%hx", aRequest, aMask, iChannelNumber );

    C_TRACE( ( _T( "DISAUserChannel::DoCancel ->" ) ) );
    switch( aMask&aRequest )
        {
        case EIADAsyncNotifyConnectionStatus:
            {
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncNotifyConnectionStatus 0x%x" ), iIADConnectionStatusPtr ) );
            OstTrace1( TRACE_NORMAL, DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncNotifyConnectionStatus;iIADConnectionStatusPtr=%x", iIADConnectionStatusPtr );            
            iIADConnectionStatusPtr = NULL;
            break;
            }
        case EIADAsyncNotifyFlowControlStatus:
            {
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncNotifyFlowControlStatus 0x%x" ), iIADFlowControlStatusPtr ) );
            OstTrace1( TRACE_NORMAL, DUP1_DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncNotifyFlowControlStatus;iIADFlowControlStatusPtr=%x", iIADFlowControlStatusPtr );
            
            iIADFlowControlStatusPtr = NULL;
            break;
            }
        case EIADAsyncReceive:
            {
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncReceive 0x%x 0x%x" ), iReceiveBufPtr, iNeededBufLen ) );
            OstTraceExt2( TRACE_NORMAL, DUP2_DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncReceive;iReceiveBufPtr=%x;iNeededBufLen=%x", (TUint)iReceiveBufPtr, (TUint)iNeededBufLen );
            
            iReceiveBufPtr = NULL;
            iNeededBufLen = NULL;
            break;
            }
        case EIADAsyncDataReceive:
            {
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncDataReceive 0x%x 0x%x" ), iDataReceiveBufPtr, iNeededDataBufLen ) );
            OstTraceExt2( TRACE_NORMAL, DUP3_DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncDataReceive;iDataReceiveBufPtr=%x;iNeededDataBufLen=%x", (TUint)iDataReceiveBufPtr, (TUint)iNeededDataBufLen );
            
            iDataReceiveBufPtr = NULL;
            iNeededDataBufLen = NULL;
            break;
            }
        case EIADAsyncOpen:
            {
            // Just complete with cancel
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncOpen" ) ) );
            OstTrace0( TRACE_NORMAL, DUP4_DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncOpen" );            
            break;
            }
        case EIADAsyncSubscribeIndications:
        case EIADAsyncSendIndication:
        case EIADAsyncSubscribeIndications32Bit:
            {
            // Cause activation and cancelation request are run in the same thread. not actually possible to cancel request.
            // Just complete with cancel.
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncSubscribeIndications | EIADAsyncSendIndication | EIADAsyncSubscribeIndications32bit" ) ) );
            OstTrace0( TRACE_NORMAL, DUP5_DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncSubscribeIndications | EIADAsyncSendIndication | EIADAsyncSubscribeIndications32Bit" );
            break;
            }
        case EIADAsyncSend:
            {
            // Just complete with cancel.
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncSend" ) ) );
            OstTrace0( TRACE_NORMAL, DUP6_DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncSend" );
            
            break;
            }
        case EIADAsyncDataSend:
            {
            // Just complete with cancel.
            C_TRACE( ( _T( "DISAUserChannel::DoCancel EIADAsyncSend" ) ) );
            OstTrace0( TRACE_NORMAL, DUP7_DISAUSERCHANNEL_DOCANCEL, "DISAUserChannel::DoCancel EIADAsyncDataSend" );
            
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, EIADWrongRequest | EIADFaultIdentifier12 << KFaultIdentifierShift );
            break;
            }
        }
    CompleteChannelRequest( aMask&aRequest, KErrCancel );
    C_TRACE( ( _T( "DISAUserChannel::DoCancel <-" ) ) );

    OstTrace0( TRACE_NORMAL, DISAUSERCHANNEL_DOCANCEL_EXIT, "<DISAUserChannel::DoCancel" );
    }



