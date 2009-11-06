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
* Description:  Declaration and implementation of KISITESTER
*
*/



#ifndef KISI_TRACE_H
#define KISI_TRACE_H


// INCLUDES
#include <kernel/kernel.h>

// CONSTANTS
#ifndef _DEBUG
//    #define _DEBUG // to get traces from UREL as well
#endif


// MACROS
#define ASSERT_RESET ASSERT_RESET_ALWAYS
#define _T(a) a

#ifdef _DEBUG
    //#define __TRACE_PRINTF(a) PerfPrintf a
  	#define __TRACE_PRINTF(a) Kern::Printf a  // Backup line, do not uncomment
#else
    #define __TRACE_PRINTF(a)
#endif // _DEBUG


#ifdef _DEBUG
    _LIT(KSisaExtensionName, "SISA");

    void BuildTraceFunc( const TDesC& aPrefix );
    void DataDumpTraceFunc( const TDesC& aPrefix, const TDesC8& aData );

    #define BUILD_TRACE( a ) BuildTraceFunc( a )
	#define DATA_DUMP_TRACE( a, b ) DataDumpTraceFunc( a, b )

    #define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ ))
    #define TRACE_ASSERT_RETURN(a) if (!((a)==KErrNone)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ ))

    #if defined COMPONENT_TRACE_FLAG
        #define COMPONENT_TRACE(a) __TRACE_PRINTF(a)
    #else
        #define COMPONENT_TRACE(a) 
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
    #define COMPONENT_TRACE(a)
    #define A_TRACE(a)
    #define E_TRACE(a)
	#define BUILD_TRACE(a)
    #define DATA_DUMP_TRACE(a, b)
#endif // _DEBUG


// Resets are made both in UDEB and UREL (traces only in udeb)
#define ASSERT_RESET_ALWAYS(a,b,c) if(!(a)) { __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); Kern::Fault(b,c); }
#define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( ("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__) )


// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

#endif //#ifndef KISI_TRACE_H
// End of File
