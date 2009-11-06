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



// INCLUDE FILES
#include <kern_priv.h>               // For __ASSERT_CRITICAL published partner

#include <isakernelapi.h>
#include "iadtrace.h"                // FOR ASSERT..
#include "isakernelchannel.h"        // FOR DISAKernelChannel
#include "iadnokiadefinitions.h"     // For checking allowed kernel channels.
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "isakernelapiTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
const TInt KFirstParam( 0 );
const TInt KSecondParam( 1 );
const TInt KThirdParam( 2 );
const TInt KFourthParam( 3 );
const TInt KOneParam( 1 );
const TInt KTwoParams( 2 );
const TInt KThreeParams( 3 );
const TInt KFourParams( 4 );

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
// None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DIsaKernelAPI::NewF
// -----------------------------------------------------------------------------
//
EXPORT_C DIsaKernelAPI* DIsaKernelAPI::NewF(
        // None
        )
    {
    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_NEWF_ENTRY, ">DIsaKernelAPI::NewF" );

    C_TRACE((_T("DIsaKernelAPI::CreateNew ->")));
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    // ASSERT IN CRITICAL SECTION // TODO: All where allocation from kernel heap (new)
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, 0xff );
    __ASSERT_CRITICAL;// From kern_priv.h published partner
    DIsaKernelAPI* tmp = new DIsaKernelAPI();
    ASSERT_RESET_ALWAYS( tmp, EIADMemoryAllocationFailure | EIADFaultIdentifier5 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::CreateNew 0x%x <-" ), tmp ) );
    
    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_NEWF_EXIT, "<DIsaKernelAPI::NewF;return=%x", tmp );
    return tmp;
    }

// -----------------------------------------------------------------------------
// DIsaKernelAPI::DIsaKernelAPI
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C DIsaKernelAPI::DIsaKernelAPI(
        // None
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_DISAKERNELAPI_ENTRY, ">DIsaKernelAPI::DIsaKernelAPI;this=%x", (TUint)this );

    C_TRACE((_T("DIsaKernelAPI::DIsaKernelAPI ->")));
    // TODO: change this do more fancy, factory or derivation?
    iKernelChannel = new DISAKernelChannel();
    ASSERT_RESET_ALWAYS( iKernelChannel, EIADMemoryAllocationFailure | EIADFaultIdentifier6 << KFaultIdentifierShift );
    C_TRACE((_T("DIsaKernelAPI::DIsaKernelAPI 0x%x <-"), this ));

    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_DISAKERNELAPI_EXIT, "<DIsaKernelAPI::DIsaKernelAPI;this=%x", this );
    }

// -----------------------------------------------------------------------------
// DIsaKernelAPI::~DIsaKernelAPI
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C DIsaKernelAPI::~DIsaKernelAPI(
        // None
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_DISAKERNELAPI_DES_ENTRY, ">DIsaKernelAPI::~DIsaKernelAPI;this=%x", this );

    C_TRACE((_T("DIsaKernelAPI::~DIsaKernelAPI 0x%x ->"), this ));
    Close();
    if( iKernelChannel )
        {
        C_TRACE( ( _T( "DIsaKernelAPI::~DIsaKernelAPI delete iKernelChannel.." ) ) );
        OstTrace0( TRACE_NORMAL, DISAKERNELAPI_DISAKERNELAPI_DELETE, "DIsaKernelAPI::~DIsaKernelAPI delete iKernelChannel" );
        
        delete iKernelChannel;
        C_TRACE( ( _T( "DIsaKernelAPI::~DIsaKernelAPI deleted ..iKernelChannel" ) ) );
        iKernelChannel = NULL;
        }
    C_TRACE((_T("DIsaKernelAPI::~DIsaKernelAPI <-")));

    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_DISAKERNELAPI_DES_EXIT, "<DIsaKernelAPI::~DIsaKernelAPI;this=%x", this );
    }

EXPORT_C TDes8& DIsaKernelAPI::AllocateMsgBlock(
        const TInt aSize
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELAPI_ALLOCATEMSGBLOCK_ENTRY, ">DIsaKernelAPI::AllocateMsgBlock;aSize=%d;this=%x", aSize, (TUint)this );
    
    C_TRACE( ( _T( "DIsaKernelAPI::AllocateMsgBlock %d ->"), aSize ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    TDes8* temp = NULL;
    C_TRACE( ( _T( "DIsaKernelAPI::AllocateMsgBlock %d 0x%x TBR"), aSize, temp ) );
    TInt size( aSize );
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &size );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &temp );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADAllocateBlock;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier1 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::AllocateMsgBlock %d 0x%x <-"), aSize, temp ) );

    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_ALLOCATEMSGBLOCK_EXIT, "<DIsaKernelAPI::AllocateMsgBlock;temp=%x", temp);
    return *temp;
    }


EXPORT_C TDes8& DIsaKernelAPI::AllocateDataMsgBlock(
        const TInt aSize
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELAPI_ALLOCATEDATAMSGBLOCK_ENTRY, ">DIsaKernelAPI::AllocateDataMsgBlock;aSize=%d;this=%x", aSize, (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::AllocateDataMsgBlock %d ->"), aSize ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    TDes8* temp = NULL;
    C_TRACE( ( _T( "DIsaKernelAPI::AllocateDataMsgBlock %d 0x%x TBR"), aSize, temp ) );
    TInt size( aSize );
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &size );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &temp );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADAllocateDataBlock;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier2 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::AllocateDataMsgBlock %d 0x%x <-"), aSize, temp ) );
    
    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_ALLOCATEDATAMSGBLOCK_EXIT, "<DIsaKernelAPI::AllocateDataMsgBlock");
    return *temp;
    }


EXPORT_C void DIsaKernelAPI::Close(
        // None
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_CLOSE_ENTRY, ">DIsaKernelAPI::Close;this=%x", (TUint)this );
// todo ichannelnumber??

    C_TRACE( ( _T( "DIsaKernelAPI::Close 0x%x ->" ), this ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    __ASSERT_CRITICAL;// From kern_priv.h published partner
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADClose;
    m.iArg[ KFirstParam ] = NULL;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier3 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::Close 0x%x <-" ), this ) );

    OstTrace0( TRACE_NORMAL, DUP1_DISAKERNELAPI_CLOSE_EXIT, "<DIsaKernelAPI::Close" );
    }

EXPORT_C void DIsaKernelAPI::DataReceive(
        TDes8*& aDataMsgBlock,
        TInt&   aDfcStatus,
        const   TDfc& aDataReceiveDfc
        )
    {
    OstTraceExt4( TRACE_NORMAL, DISAKERNELAPI_DATARECEIVE_ENTRY, ">DIsaKernelAPI::DataReceive;aDataMsgBlock=%x;aDfcStatus=%d;aDataReceiveDfc=%x;this=%x", ( TUint )( aDataMsgBlock ), aDfcStatus, ( TUint )&( aDataReceiveDfc ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::DataReceive 0x%x 0x%x ->"), aDataMsgBlock, &aDataReceiveDfc ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    ASSERT_RESET_ALWAYS( !aDataMsgBlock, EIADWrongParameter | ( TUint8 )( iKernelChannel->GetChannelNumber() ) << KChannelNumberShift | EIADFaultIdentifier5 << KFaultIdentifierShift );
    aDfcStatus = KRequestPending;
    TAny* params[ KThreeParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aDfcStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &const_cast<TDfc&>( aDataReceiveDfc ) );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( &aDataMsgBlock );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADAsyncDataReceive;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier4 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::DataReceive 0x%x 0x%x <-"), aDataMsgBlock, &aDataReceiveDfc ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_DATARECEIVE, "<DIsaKernelAPI::DataReceive" );
    }


EXPORT_C void DIsaKernelAPI::DataReceiveCancel(
        // None
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_DATARECEIVECANCEL_ENTRY, ">DIsaKernelAPI::DataReceiveCancel;this=%x", (TUint)this );

     C_TRACE( ( _T( "DIsaKernelAPI::DataReceiveCancel 0x%x ->"), this ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EIADAsyncDataReceive );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier5 << KFaultIdentifierShift  );
    C_TRACE( ( _T( "DIsaKernelAPI::DataReceiveCancel 0x%x <-"), this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_DATARECEIVECANCEL_EXIT, "<DIsaKernelAPI::DataReceiveCancel");
    }


EXPORT_C TInt DIsaKernelAPI::DataSend(
        TDes8& aMsg
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELAPI_DATASEND_ENTRY, ">DIsaKernelAPI::DataSend;aMsg=%x;this=%x", ( TUint )&( aMsg ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::DataSend 0x%x ->"), &aMsg ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aMsg );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADDataSend;
    m.iArg[ KFirstParam ] = params;
    TInt retValue( iKernelChannel->HandleRequest( m ) );
    C_TRACE( ( _T( "DIsaKernelAPI::DataSend 0x%x %d <-"), &aMsg, retValue ) );

    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_DATASEND, "<DIsaKernelAPI::DataSend;retValue=%d", retValue);
    return retValue;	
    }

EXPORT_C void DIsaKernelAPI::DeAllocateMsgBlock(
        TDes8& aMsgBlock
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELAPI_DEALLOCATEMSGBLOCK_ENTRY, ">DIsaKernelAPI::DeAllocateMsgBlock;aMsgBlock=%x;this=%x", ( TUint )&( aMsgBlock ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::DeAllocateMsgBlock 0x%x ->"), &aMsgBlock ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aMsgBlock );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADDeAllocateBlock;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier6 << KFaultIdentifierShift  );
    C_TRACE( ( _T( "DIsaKernelAPI::DeAllocateMsgBlock 0x%x <-"), &aMsgBlock ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_DEALLOCATEMSGBLOCK_EXIT, "<DIsaKernelAPI::DeAllocateMsgBlock" );
    }


EXPORT_C void DIsaKernelAPI::DeAllocateDataMsgBlock(
        TDes8& aMsgBlock
        )
    {
OstTraceExt2( TRACE_NORMAL, DISAKERNELAPI_DEALLOCATEDATAMSGBLOCK_ENTRY, ">DIsaKernelAPI::DeAllocateDataMsgBlock;aMsgBlock=%x;this=%x", ( TUint )&( aMsgBlock ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::DeAllocateDataMsgBlock 0x%x ->"), &aMsgBlock ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aMsgBlock );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADDeAllocateDataBlock;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier7 << KFaultIdentifierShift  );
    C_TRACE( ( _T( "DIsaKernelAPI::DeAllocateDataMsgBlock 0x%x <-"), &aMsgBlock ) );

    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_DEALLOCATEDATAMSGBLOCK_EXIT, "<DIsaKernelAPI::DeAllocateDataMsgBlock;aMsgBlock=%x", ( TUint )&( aMsgBlock ) );
    }

EXPORT_C void DIsaKernelAPI::NotifyFlowChange(
        TDfc& aNotifyFlowDfc,
        TInt& aDfcStatus
        )
    {
    OstTraceExt3( TRACE_NORMAL, DISAKERNELAPI_NOTIFYFLOWCHANGE_ENTRY, ">DIsaKernelAPI::NotifyFlowChange;aNotifyFlowDfc>=%x;aDfcStatus=%d;this=%x", ( TUint )&( aNotifyFlowDfc ), aDfcStatus, (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::NotifyFlowChange 0x%x %d 0x%x 0x%x ->" ), &aDfcStatus, aDfcStatus, &aNotifyFlowDfc, this ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    aDfcStatus = KRequestPending;
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aDfcStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aNotifyFlowDfc );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADAsyncNotifyFlowControlStatus;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier8 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::NotifyFlowChange 0x%x %d 0x%x 0x%x <-" ), &aDfcStatus, aDfcStatus, &aNotifyFlowDfc, this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_NOTIFYFLOWCHANGE_EXIT, "<DIsaKernelAPI::NotifyFlowChange");
    }


EXPORT_C void DIsaKernelAPI::NotifyFlowChangeCancel(
        // None
        )
    {
OstTrace1( TRACE_NORMAL, DISAKERNELAPI_NOTIFYFLOWCHANGECANCEL_ENTRY, ">DIsaKernelAPI::NotifyFlowChangeCancel;this=%x", (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::NotifyFlowChangeCancel 0x%x ->"), this ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EIADAsyncNotifyFlowControlStatus );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier9 << KFaultIdentifierShift  );
    C_TRACE( ( _T( "DIsaKernelAPI::NotifyFlowChangeCancel 0x%x <-"), this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_NOTIFYFLOWCHANGECANCEL_EXIT, "<DIsaKernelAPI::NotifyFlowChangeCancel" );
    }

EXPORT_C void DIsaKernelAPI::NotifyStateChange(
        TDfc& aNotifyStateDfc,
        TInt& aDfcStatus
        )
    {
    OstTraceExt3( TRACE_NORMAL, DISAKERNELAPI_NOTIFYSTATECHANGE_ENTRY, ">DIsaKernelAPI::NotifyStateChange;aNotifyStateDfc=%x;aDfcStatus=%d;this=%x", ( TUint )&( aNotifyStateDfc ), aDfcStatus, (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::NotifyStateChange 0x%x %d 0x%x 0x%x ->" ), &aDfcStatus, aDfcStatus, &aNotifyStateDfc, this ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    // TODO using of m.iArg 
    aDfcStatus = KRequestPending;
    TAny* params[ KTwoParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aDfcStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aNotifyStateDfc );

    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADAsyncNotifyConnectionStatus;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier10 << KFaultIdentifierShift  );
    C_TRACE( ( _T( "DIsaKernelAPI::NotifyStateChange 0x%x %d 0x%x 0x%x <-" ), &aDfcStatus, aDfcStatus, &aNotifyStateDfc, this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_NOTIFYSTATECHANGE_EXIT, "<DIsaKernelAPI::NotifyStateChange" );
    }

EXPORT_C void DIsaKernelAPI::NotifyStateChangeCancel(
        // None
        ) const
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_NOTIFYSTATECHANGECANCEL_ENTRY, ">DIsaKernelAPI::NotifyStateChangeCancel;this=%x", (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::NotifyStateChangeCancel 0x%x ->"), this ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EIADAsyncNotifyConnectionStatus );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier11 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::NotifyStateChangeCancel 0x%x <-"), this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_NOTIFYSTATECHANGECANCEL_EXIT, "<DIsaKernelAPI::NotifyStateChangeCancel" );
    }

// KErrNone : succesfully opened
// KErrInUse : same object tries to open channel again
// KErrAlreadyExists: other object tries to open channelid that is opened by another object
EXPORT_C void DIsaKernelAPI::Open(
        const TUint16 aChannelNumber,
        TInt& aDfcStatus,
        TDfc& aOpenDfc,
        const TUint32 aResource // 32-bit resource id support
        )
    {
    OstTraceExt5( TRACE_NORMAL, DISAKERNELAPI_OPEN_ENTRY, ">DIsaKernelAPI::Open;aChannelNumber=%hu;aDfcStatus=%d;aOpenDfc=%x;aResource=%u;this=%x", aChannelNumber, aDfcStatus, ( TUint )&( aOpenDfc ), ( TUint )( aResource ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::Open 0x%x 0x%x 0x%x %d 0x%x 0x%x ->" ), this, aChannelNumber, &aDfcStatus, aDfcStatus, &aOpenDfc, aResource ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    ASSERT_CONTEXT_ALWAYS( NKern::EThread, aChannelNumber );
    ASSERT_RESET_ALWAYS( ( EIADNokiaLastUserChannel <= aChannelNumber && aChannelNumber < EIADSizeOfChannels ), EIADWrongParameter | aChannelNumber << KChannelNumberShift | EIADFaultIdentifier4 << KFaultIdentifierShift );
    // TODO: reading of params with kumemget? and the before giving to channel
    aDfcStatus = KRequestPending;
    // TODO using of m.iArg 
    TAny* params[ KFourParams ];
    aDfcStatus = KRequestPending;
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aDfcStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &aOpenDfc );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( aChannelNumber );
    TBuf8<4> channelInfo;
    channelInfo.Append( aResource >> 24 );
    channelInfo.Append( aResource >> 16 );
    channelInfo.Append( aResource >> 8 );
    channelInfo.Append( aResource );
    params[ KFourthParam ] = reinterpret_cast<TAny*>( &channelInfo );

    C_TRACE( ( _T( "0x%x 0x%x 0x%x 0x%x TBR" ), channelInfo[ 0 ], channelInfo[ 1 ], channelInfo[ 2 ], channelInfo[ 3 ] ) );

    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADAsyncOpen;
    m.iArg[ KFirstParam ] = params;

    C_TRACE( ( _T( "TBR****TBR m.iArg[ 0 ]=0x%x m.iArg[ 1 ]=0x%x m.iArg[ 2 ]=0x%x m.iArg[ 3 ]=0x%x" ), m.iArg[ 0 ], m.iArg[ 1 ], m.iArg[ 2 ], m.iArg[ 3 ] ) );
    TUint32* tablePtr = reinterpret_cast<TUint32*>( m.Ptr0() );
    C_TRACE( ( _T( "TBR****TBR tablePtr[ 0 ]=0x%x tablePtr[ 1 ]=0x%x tablePtr[ 2 ]=0x%x tablePtr[ 3 ]=0x%x" ), tablePtr[ 0 ], tablePtr[ 1 ], tablePtr[ 2 ], tablePtr[ 3 ] ) );

    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | (TUint8)aChannelNumber << KChannelNumberShift | EIADFaultIdentifier12 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::Open 0x%x <-" ), this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_OPEN_EXIT, "<DIsaKernelAPI::Open");
    }

EXPORT_C void DIsaKernelAPI::OpenCancel(
        // None
        )
    {
OstTrace1( TRACE_NORMAL, DISAKERNELAPI_OPENCANCEL_ENTRY, ">DIsaKernelAPI::OpenCancel;this=%x", (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::OpenCancel ->") ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EIADAsyncOpen );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | iKernelChannel->GetChannelNumber() << KChannelNumberShift | EIADFaultIdentifier13 << KFaultIdentifierShift );
    C_TRACE( ( _T( "DIsaKernelAPI::OpenCancel <-") ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_OPENCANCEL_EXIT, "<DIsaKernelAPI::OpenCancel" );
    }

EXPORT_C void DIsaKernelAPI::Receive(
        TDes8*& aMsgBlock,
        TInt&   aDfcStatus,
        const   TDfc& aReceiveDfc
        )
    {
    OstTraceExt4( TRACE_NORMAL, DISAKERNELAPI_RECEIVE_ENTRY, ">DIsaKernelAPI::Receive;aMsgBlock=%x;aDfcStatus=%d;aReceiveDfc=%x;this=%x", ( TUint )( aMsgBlock ), aDfcStatus, ( TUint )&( aReceiveDfc ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::Receive 0x%x 0x%x ->"), aMsgBlock, &aReceiveDfc ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    ASSERT_RESET_ALWAYS( !aMsgBlock, EIADWrongParameter | ( TUint8 )( iKernelChannel->GetChannelNumber() ) << KChannelNumberShift | EIADFaultIdentifier7 << KFaultIdentifierShift );
    aDfcStatus = KRequestPending;
    TAny* params[ KThreeParams ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aDfcStatus );
    params[ KSecondParam ] = reinterpret_cast<TAny*>( &const_cast<TDfc&>( aReceiveDfc ) );
    params[ KThirdParam ] = reinterpret_cast<TAny*>( &aMsgBlock );
    
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADAsyncReceive;
    m.iArg[ KFirstParam ] = params;
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier14 << KFaultIdentifierShift  );
    C_TRACE( ( _T( "DIsaKernelAPI::Receive 0x%x 0x%x <-"), aMsgBlock, &aReceiveDfc ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_RECEIVE_EXIT, "<DIsaKernelAPI::Receive" );
    }

EXPORT_C void DIsaKernelAPI::ReceiveCancel(
        // None
        )
    {
    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_RECEIVECANCEL_ENTRY, ">DIsaKernelAPI::ReceiveCancel;this=%x", (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::ReceiveCancel 0x%x ->"), this ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = KMaxTInt;
    m.iArg[ KFirstParam ] = reinterpret_cast<TAny*>( EIADAsyncReceive );
    ASSERT_RESET_ALWAYS( ( KErrNone == iKernelChannel->HandleRequest( m ) ), EIADWrongKernelResponse | EIADFaultIdentifier15 << KFaultIdentifierShift  );
    C_TRACE( ( _T( "DIsaKernelAPI::ReceiveCancel 0x%x <-"), this ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_RECEIVECANCEL_EXIT, "<DIsaKernelAPI::ReceiveCancel" );
    }

EXPORT_C TInt DIsaKernelAPI::Send(
        TDes8& aMsg
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELAPI_SEND_ENTRY, ">DIsaKernelAPI::Send;aMsg=%x;this=%x", ( TUint )&( aMsg ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::Send 0x%x ->"), &aMsg ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aMsg );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADSend;
    m.iArg[ KFirstParam ] = params;
    TInt retValue( iKernelChannel->HandleRequest( m ) );
    C_TRACE( ( _T( "DIsaKernelAPI::Send 0x%x %d <-"), &aMsg, retValue ) );

    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_SEND_EXIT, "<DIsaKernelAPI::Send;retValue=%d", retValue );
    return retValue;
    }

EXPORT_C TInt DIsaKernelAPI::SendIndication(
        TDes8& aMsg
        )
    {
    OstTraceExt2( TRACE_NORMAL, DISAKERNELAPI_SENDINDICATION_ENTRY, ">DIsaKernelAPI::SendIndication;aMsg=%x;this=%x", ( TUint )&( aMsg ), (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::SendIndication 0x%x ->"), &aMsg ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( &aMsg );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = EIADSendIndication;
    m.iArg[ KFirstParam ] = params;
    TInt retValue( iKernelChannel->HandleRequest( m ) );
    C_TRACE( ( _T( "DIsaKernelAPI::SendIndication 0x%x %d <-"), &aMsg, retValue ) );

    OstTrace0( TRACE_NORMAL, DISAKERNELAPI_SENDINDICATION_EXIT, "<DIsaKernelAPI::SendIndication" );
    return retValue;
    }

// KErrBadDescriptor, subscription length zero (0) or order length wrong: !32bit% 5 and !8bit%2 
// KErrUnderFlow, too long subscription
EXPORT_C TInt DIsaKernelAPI::SubscribeIndications(
        const TDesC8&   aIndications,
        const TBool     aThirtyTwoBit
        )
    {
    OstTraceExt3( TRACE_NORMAL, DISAKERNELAPI_SUBSCRIBEINDICATIONS_ENTRY, ">DIsaKernelAPI::SubscribeIndications;aIndications=%x;aThirtyTwoBit=%d;this=%x", ( TUint )&( aIndications ), aThirtyTwoBit, (TUint)this );

    C_TRACE( ( _T( "DIsaKernelAPI::SubscribeIndications 0x%x %d ->"), &aIndications, aThirtyTwoBit ) );
    // TODO: ASSERT KERNEL CONTEXT
    // TODO: OTHER ASSERTS
    TAny* params[ KOneParam ];
    params[ KFirstParam ] = reinterpret_cast<TAny*>( const_cast<TDesC8*>( &aIndications ) );
    // Get's the current thread's thread message.
    TThreadMessage& m = Kern::Message();
    m.iValue = ( aThirtyTwoBit ? EIADSubscribeIndications32Bit : EIADSubscribeIndications );
    m.iArg[ KFirstParam ] = params;
    TInt retValue( iKernelChannel->HandleRequest( m ) );
    C_TRACE( ( _T( "DIsaKernelAPI::SubscribeIndications 0x%x %d %d <-"), &aIndications, aThirtyTwoBit, retValue ) );

    OstTrace1( TRACE_NORMAL, DISAKERNELAPI_SUBSCRIBEINDICATIONS_EXIT, "<DIsaKernelAPI::SubscribeIndications;retValue=%d", retValue );
    return retValue;
    }

// End of file
