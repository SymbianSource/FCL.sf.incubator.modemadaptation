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



#ifndef __ISIROUTERTRACE_H__
#define __ISIROUTERTRACE_H__

// INCLUDES
#include <kernel.h>

// CONSTANTS
enum TISIFaultClassCategories
    {
    EDISIUserChannelTraceId,
    EDISIRouterTraceId,
    EDISILinkTraceId,
    EDISIMsgQueueTraceId,
    EDISIIfTraceId,
    EDISIKernelChannelId,
    EDISIThreadContainerTraceId,
    EDISICLTransceiverTraceId,
    };

const TUint8 KClassIdentifierShift( 8 );
const TUint8 KObjIdShift( 16 );
const TUint8 KMediaIdShift( 16 );
const TUint8 KExtraInfoShift( 24 );

// MACROS

#define _T( a ) a
#define __TRACE_PRINTF(a) Kern::Printf a

#ifdef _DEBUG
    #if defined COMPONENT_TRACE_FLAG
        #define C_TRACE(a) __TRACE_PRINTF(a)
    #else
        #define C_TRACE(a) 
    #endif // COMPONENT_TRACE_FLAG
#else
    #define C_TRACE(a)
#endif // _DEBUG

// Resets are made both in UDEB and UREL (traces only in udeb)
#define ASSERT_RESET_ALWAYS(a,b) if(!(a)) { __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); Kern::Fault( "ISI Router:", b ); }
#define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( ("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__) )
#define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ ))
#define TRACE_ASSERT_INFO(a, b) if (!(a)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, extra info 0x%08x, compiled=" __DATE__" "__TIME__, __LINE__, b ));
#define TRACE_WARNING(a, b) if (!(a)) __TRACE_PRINTF(("Warning: file=" __FILE__ ", line=%d, extra info 0x%x, compiled=" __DATE__" "__TIME__, __LINE__, b ));
#define ASSERT_PANIC_USER_THREAD_ALWAYS( a, b, c ) if(!(a)){__TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__));Kern::ThreadKill( b, EExitPanic, c, _L8("ISI Router:") );}
#define ASSERT_THREAD_CONTEXT_ALWAYS( a )ASSERT_RESET_ALWAYS( NKern::CurrentContext() == NKern::EThread, a );

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif // __ISIROUTERTRACE_H__

//  End of File
