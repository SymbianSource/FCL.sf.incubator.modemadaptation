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


#ifndef _USBPNTRACE_H_
#define _USBPNTRACE_H_

#define ASSERT_RESET ASSERT_RESET_ALWAYS


// INCLUDES
// CONSTANTS

// MACROS
// INCLUDES
#include <e32svr.h> // RDebug

#define _T(a) _L(a)

// Trace asserts are made both in UDEB and UREL.
#define __TRACE_PRINTF(a) RDebug::Print a

#ifdef _DEBUG

    _LIT(KUsbPnLinkName, "USB Phonet Link");

    void BuildTraceFunc( const TDesC* aPrefix );
    void DataDumpTraceFunc( const TDesC& aPrefix, const TDesC8& aData );

    #define BUILD_TRACE( a ) BuildTraceFunc( a )
    #define DATA_DUMP_TRACE( a, b ) DataDumpTraceFunc( a, b )

    // define assertions
    #ifdef __WINS__
        #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF((_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__))
        #define TRACE_ASSERT_RETURN(a) __TRACE_PRINTF((_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__))
    #else
        #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF((_L("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__ __TIME__""),__LINE__ ))
        #define TRACE_ASSERT_RETURN(a) __TRACE_PRINTF((_L("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__ __TIME__""),__LINE__ ))
    #endif  //__WINS__

    #if defined COMPONENT_TRACE_FLAG
        #define C_TRACE(a) __TRACE_PRINTF(a)
    #else
        #define C_TRACE(a)
    #endif // COMPONENT_TRACE_FLAG

    #if defined API_TRACE_FLAG
        #define A_TRACE(a) __TRACE_PRINTF(a)
    #else
        #define A_TRACE(a)
    #endif // API_TRACE_FLAG

    #if defined EXTENDED_TRACE_FLAG
        #define E_TRACE(a) __TRACE_PRINTF(a)
    #else
        #define E_TRACE(a)
    #endif // EXTENDED_TRACE_FLAG

#else
    #define TRACE_ASSERT(a)
    #define TRACE_ASSERT_RETURN(a) a
    #define C_TRACE(a)
    #define A_TRACE(a)
    #define E_TRACE(a)
    #define BUILD_TRACE(a)
    #define DATA_DUMP_TRACE(a, b)
#endif // _DEBUG

// Resets and trace asserts are made both in UDEB and UREL (traces only in udeb)
#ifdef __WINS__
  #define ASSERT_RESET_ALWAYS(a,b,c) error // ASSERT_RESET_ALWAYS for (!TRACE_MODE_KERNEL_&& __WINS__) not implemented
  #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) )
#else
  #define ASSERT_RESET_ALWAYS(a,b,c) if(!(a)) { __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) ); User::Panic(b,c); }
  #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file=%s, line=%d, compiled=%s %s "),__FILE__, __LINE__, __DATE__, __TIME__) )
#endif

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif //#ifndef _USBPNTRACE_H_
// End of File
