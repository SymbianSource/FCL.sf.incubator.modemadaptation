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



#ifndef __ISIUSERAPITRACE_H_
#define __ISIUSERAPITRACE_H_

// CONSTANTS
_LIT( KISIUserApiPanic, "ISI User Panic:" );

// MACROS
// Panics given due to wrong usage of ISI API.
enum TISIUserApiPanics
    {
    EISIChannelNumberOutOfRange = 0x01,
    EISIChannelNotOpen,
    EISIMemAllocFailure,
    EISICloseFailed,
    EISIDriverLoadFailed,
    EISIChannelCreationFailed,
    };


// INCLUDES
#include <e32svr.h>    // For RDebug
#include <e32std.h>
#define _T( a ) _L( a )
#define __TRACE_PRINTF( a ) RDebug::Print a

#ifdef _DEBUG
    #ifdef COMPONENT_TRACE_FLAG
        #define C_TRACE( a ) __TRACE_PRINTF( a )
    #else
        #define C_TRACE( a ) 
    #endif // COMPONENT_TRACE_FLAG
#else
    #define C_TRACE( a )
#endif // _DEBUG
//
// Resets are made both in UDEB and UREL (traces only in udeb)
#ifdef __WINS__
#define ASSERT_PANIC_ALWAYS(a,b) if(!(a)){User::Panic( KISIUserApiPanic, b);};
    #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) )
    #define TRACE_ASSERT_ALWAYS_COND(a)if(!(a)){ __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) );}
#else
    #define ASSERT_PANIC_ALWAYS(a,b) if(!(a)){User::Panic( KISIUserApiPanic, b);}; 
    #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file=%s, line=%d, compiled=%s %s "),__FILE__, __LINE__, __DATE__, __TIME__) )
    #define TRACE_ASSERT_ALWAYS_COND(a)if(!(a)) {__TRACE_PRINTF( (_L("Assertion failed: file=%s, line=%d, compiled=%s %s "),__FILE__, __LINE__, __DATE__, __TIME__) );}
#endif

// Trace asserts activated also in UREL
// define assertions
#ifdef __WINS__
#define TRACE_ASSERT(a) if (!(a)) {__TRACE_PRINTF((_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__));}
#else
#define TRACE_ASSERT(a) if (!(a)) {__TRACE_PRINTF((_L("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__ __TIME__""),__LINE__ ));}
#endif  //__WINS__

#endif // __ISIUSERAPITRACE_H_

//  End of File
