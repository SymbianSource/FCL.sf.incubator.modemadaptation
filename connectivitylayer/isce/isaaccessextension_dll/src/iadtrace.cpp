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
#include <phonetisi.h>          // For ISI_HEADER_OFFSET_MEDIA etc...
#include "iadtrace.h"           // For C_TRACE..
#include "iadhelpers.h"         // For GET_RECEIVER

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS

// ============================= TRACE FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// BuildTraceFunc
// -----------------------------------------------------------------------------
void BuildTraceFunc
        (
        const TDesC& aPrefix    // Components name.
        )
    {
    __TRACE_PRINTF( ( _T("%s: Compiled=%s - %s"), aPrefix.Ptr(), __DATE__, __TIME__ ) );
    __TRACE_PRINTF( ( _T("%s: Possible flags are: { __WINS__ | _DEBUG | __KERNEL_MODE__ }"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: flags continue:     { NCP_COMMON_SIZE_ARCHITECTURE_IN_USE | NCP_COMMON_CONVERGENCE_ARCHITECTURE_IN_USE"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: test flags are:     { NCP_COMMON_MINIOS | CASW_ISCISIMULTIPLEXER_MAX_AMOUNT_OF_PARALLEL_DATACHANNELS | ISCTESTSTUB_SUPPORT_FLAG"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: trace flags are:    { COMPONENT_TRACE_FLAG | API_TRACE_FLAG | EXTENDED_TRACE_FLAG }"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: trace flags:        { MESSAGE_TRACE_FLAG | DATA_DUMP_TRACE_FLAG }"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: Following flags are supported" ), aPrefix.Ptr() ) );

#ifdef __WINS__
    __TRACE_PRINTF( ( _T( "%s:     __WINS__" ), aPrefix.Ptr() ) );
#endif

#ifdef _DEBUG
    __TRACE_PRINTF( ( _T( "%s:     _DEBUG" ), aPrefix.Ptr() ) );
#endif

#ifdef __KERNEL_MODE__
    __TRACE_PRINTF( ( _T( "%s:     __KERNEL_MODE__" ), aPrefix.Ptr() ) );
#endif

#ifdef NCP_COMMON_SIZE_ARCHITECTURE_IN_USE
    __TRACE_PRINTF( ( _T( "%s:     NCP_COMMON_SIZE_ARCHITECTURE_IN_USE" ), aPrefix.Ptr() ) );
#endif

#ifdef NCP_COMMON_CONVERGENCE_ARCHITECTURE_IN_USE
    __TRACE_PRINTF( ( _T( "%s:     NCP_COMMON_CONVERGENCE_ARCHITECTURE_IN_USE" ), aPrefix.Ptr() ) );
#endif

#ifdef CASW_ISCISIMULTIPLEXER_MAX_AMOUNT_OF_PARALLEL_DATACHANNELS
    __TRACE_PRINTF( ( _T( "%s:     CASW_ISCISIMULTIPLEXER_MAX_AMOUNT_OF_PARALLEL_DATACHANNELS %d" ), aPrefix.Ptr(), CASW_ISCISIMULTIPLEXER_MAX_AMOUNT_OF_PARALLEL_DATACHANNELS ) );
#endif

#ifdef ISCTESTSTUB_SUPPORT_FLAG
    __TRACE_PRINTF( ( _T( "%s:     ISCTESTSTUB_SUPPORT_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef NCP_COMMON_MINIOS
    __TRACE_PRINTF( ( _T( "%s:     NCP_COMMON_MINIOS" ), aPrefix.Ptr() ) );
#endif

#ifdef COMPONENT_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     COMPONENT_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef API_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     API_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef EXTENDED_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     EXTENDED_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef MESSAGE_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     MESSAGE_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef DATA_DUMP_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     DATA_DUMP_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

    __TRACE_PRINTF( ( _T( "%s: End of Flags supported " ), aPrefix.Ptr() ) );

// and so on .. Define all important flags for this component and print them here!!!

    }

// -----------------------------------------------------------------------------
// DataDumpTraceFunc
// -----------------------------------------------------------------------------
void DataDumpTraceFunc
        (
        const TDesC8& aPtr,
        TBool aSend
        )
    {

    const TInt KHBufCMax( 255 );
    TBuf8<KHBufCMax> buffer;
    const TUint8* ptr( aPtr.Ptr() );
    if( aSend )
        {
        C_TRACE( ( _T( "##-> S" ) ) );
        }
    else
        {
        C_TRACE( ( _T( "##-> R" ) ) );
        }
    C_TRACE( ( _T( "Media   0x%x" ), ptr[ ISI_HEADER_OFFSET_MEDIA ] ) );
    C_TRACE( ( _T( "RecDev  0x%x" ), GET_RECEIVER_DEV( ptr ) ) );
    C_TRACE( ( _T( "SendDev 0x%x" ), GET_SENDER_DEV( ptr ) ) );
    C_TRACE( ( _T( "Function0x%x" ), ptr[ ISI_HEADER_OFFSET_RESOURCEID ] ) );
    C_TRACE( ( _T( "Length  0x%x" ), GET_LENGTH( ptr ) ) );
    C_TRACE( ( _T( "RecObj  0x%x" ), GET_RECEIVER_OBJ( ptr ) ) );
    C_TRACE( ( _T( "SendObj 0x%x" ), GET_SENDER_OBJ( ptr ) ) );
    C_TRACE( ( _T( "TransId 0x%x" ), ptr[ ISI_HEADER_OFFSET_TRANSID ] ) );
    C_TRACE( ( _T( "MsgId   0x%x" ), ptr[ ISI_HEADER_OFFSET_MESSAGEID ] ) );
#ifdef FULL_DATA_TRACE_DUMP
    const TInt length( aPtr.Length() );
    C_TRACE( ( _T( "FULL_DATA_TRACE_DUMP TBR %d" ), length ) );
    for( TInt i( ISI_HEADER_OFFSET_SUBMESSAGEID ); i < length; ++i )
       {
       C_TRACE( ( _T( "[%d] = 0x%x" ), i, ptr[ i ] ) );
       }
#endif
    if( aSend )
        {
        C_TRACE( ( _T( "<-## S" ) ) );
        }
    else
        {
        C_TRACE( ( _T( "<-## R" ) ) );
        }

    }

//  End of File

