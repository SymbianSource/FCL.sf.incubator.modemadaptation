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



#ifndef IADTRACE_H
#define IADTRACE_H

// INCLUDES
#include <e32svr.h>	// RDebug

// CONSTANTS

    
// MACROS
#define _T(a) _L(a)

#ifdef _DEBUG
	#define __TRACE_PRINTF(a) RDebug::Print a
#else
    #define __TRACE_PRINTF(a)
#endif // _DEBUG

#ifdef _DEBUG



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



// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif // IADTRACE_H

//  End of File
