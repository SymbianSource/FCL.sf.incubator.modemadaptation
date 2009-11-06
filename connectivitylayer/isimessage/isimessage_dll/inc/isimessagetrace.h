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



#ifndef ISIMESSAGETRACE_H
#define ISIMESSAGETRACE_H

// INCLUDES
// CONSTANTS

// MACROS

    #ifdef TRACE_MODE_KERNEL


		#define _LIT_FOR_ASSERT(a) reinterpret_cast< const char* > ( ( ( const TDesC& )( a ) ).Ptr() )
        #define _T(a) a
    #else 

		// INCLUDES
		#include <e32svr.h>	// RDebug
        #include <e32std.h>

		#define _LIT_FOR_ASSERT(a) error
        #define _T(a) _L(a)

    #endif // TRACE_MODE_KERNEL



// macro __TRACE_PRINTF id defined because it is needed in build trace. It has also
// been included in other trace macros

#if defined TRACE_MODE_KERNEL
	#define __TRACE_PRINTF(a) Kern::Printf a
#else
	#define __TRACE_PRINTF(a) RDebug::Print a
#endif


#ifdef _DEBUG

    void BuildTraceFunc( const TDesC& aPrefix );
    void DataDumpTraceFunc( const TDesC& aPrefix, const TDesC8& aData );

    #define BUILD_TRACE( a ) BuildTraceFunc( a )
	#define DATA_DUMP_TRACE( a, b ) DataDumpTraceFunc( a, b )

    #if defined TRACE_MODE_KERNEL

		// define assertions
            #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ ))
            #ifdef __WINS__
                #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF((_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__ ))
            #else
                #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF((_L("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__""),__LINE__ ))
            #endif  //__WINS__

    #else 

		// define assertions
        #ifdef __WINS__
            #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF((_L("IsiMessage error, assertion failed: file= TIsi.cpp, line=%d  "),__LINE__) )
        #else
            #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF((_L("IsiMessage error, assertion failed: file= TIsi.cpp, line=%d  "),__LINE__) )
        #endif  //__WINS__

    #endif // TRACE_MODE_KERNEL


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
    #define C_TRACE(a)
    #define A_TRACE(a)
    #define E_TRACE(a)
	#define BUILD_TRACE(a)
    #define DATA_DUMP_TRACE(a, b)
#endif // _DEBUG


// Both in UDEB and UREL (Always traces)
#if defined TRACE_MODE_KERNEL

        #define ASSERT_RESET_ALWAYS(a,b,c) if(!(a)) { __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); Kern::Fault(b,c); }
		#define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( ("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__) )

        #ifdef __WINS__
            #define ASSERT_RESET_ALWAYS(a,b,c) if(!(a)) { __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__" ), __LINE__) ); Plat::Fault(_L(b),c); }
            #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( _L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__" ), __LINE__)
        #else
            #define ASSERT_RESET_ALWAYS(a,b,c) if(!(a)) { __TRACE_PRINTF( (_L("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__"" ), __LINE__) ); Plat::Fault(b,c); }
            #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__"" ), __LINE__) )
        #endif //WINS

#else
    #ifdef __WINS__
		#define ASSERT_RESET_ALWAYS(a,b,c) if(!(a)) { __TRACE_PRINTF( (_L("Assertion failed: file=TIsi.cpp, line=%d," ), __LINE__) ); User::Panic(b,c); }
        #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) )
    #else
		#define ASSERT_RESET_ALWAYS(a,b,c) if(!(a)) { __TRACE_PRINTF( (_L("Assertion failed: file=TIsi.cpp, line=%d," ), __LINE__) ); User::Panic(b,c); }
        #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__ __TIME__""),__LINE__) )
    #endif
#endif // kernel


// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif // ISIMESSAGETRACE_H

//  End of File
