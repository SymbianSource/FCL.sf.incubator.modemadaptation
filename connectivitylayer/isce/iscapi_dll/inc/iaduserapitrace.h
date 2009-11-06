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



#ifndef IADUSERTRACE_H
#define IADUSERTRACE_H

// CONSTANTS
_LIT( KIADUserApiPanic, "IADUserApiPanic" );

// MACROS
// Panics given due to wrong usage of ISC API.
enum TIsaAccessUserApiPanics
    {
    EIADChannelNumberOutofRange = 0x01,
    EIADChannelNotOpen,
    EIADWrongCustomFunction,
    EIADCloseFailed,
    EIADDriverLoadFailed,
    EIADChannelCreationFailed,
    };

// INCLUDES
#include <e32svr.h>    // For RDebug
#include <e32std.h>
#define _T( a ) _L( a )
#define __TRACE_PRINTF( a ) RDebug::Print a

#ifdef _DEBUG
    #define BUILD_TRACE( a ) BuildTraceFunc( a )
    #ifdef COMPONENT_TRACE_FLAG
        #define C_TRACE( a ) __TRACE_PRINTF( a )
    #else
        #define C_TRACE( a ) 
    #endif // COMPONENT_TRACE_FLAG
#else
    #define C_TRACE( a )
    #define BUILD_TRACE( a )
#endif // _DEBUG
//
// Resets are made both in UDEB and UREL (traces only in udeb)
#ifdef __WINS__
#define ASSERT_PANIC_ALWAYS(a,b) if(!(a)){User::Panic( KIADUserApiPanic, b);};
    #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) )
    #define TRACE_ASSERT_ALWAYS_COND(a)if(!(a)){ __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) );}
#else
    #define ASSERT_PANIC_ALWAYS(a,b) if(!(a)){User::Panic( KIADUserApiPanic, b);}; 
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

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
void BuildTraceFunc
        (
        const TDesC& aPrefix    // Components name.
        )
    {
    __TRACE_PRINTF( ( _T("%s: Compiled=%s - %s"), aPrefix.Ptr(), __DATE__, __TIME__ ) );
    __TRACE_PRINTF( ( _T("%s: Possible flags are: { __WINS__ | _DEBUG }"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: test flags are:     { N/A"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: trace flags are:    { API_TRACE_FLAG | COMPONENT_TRACE_FLAG }"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: Following flags are supported" ), aPrefix.Ptr() ) );

#ifdef __WINS__
    __TRACE_PRINTF( ( _T( "%s:     __WINS__" ), aPrefix.Ptr() ) );
#endif

#ifdef _DEBUG
    __TRACE_PRINTF( ( _T( "%s:     _DEBUG" ), aPrefix.Ptr() ) );
#endif

#ifdef API_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     API_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef COMPONENT_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     COMPONENT_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

__TRACE_PRINTF( ( _T( "%s: End of Flags supported " ), aPrefix.Ptr() ) );

// and so on .. Define all important flags for this component and print them here!!!

    }
#endif // IADUSERTRACE_H

//  End of File
