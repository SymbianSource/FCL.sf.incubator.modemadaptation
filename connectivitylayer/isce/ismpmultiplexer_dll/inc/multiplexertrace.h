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



#ifndef __MUX_TRACE_H__
#define __MUX_TRACE_H__

// INCLUDES
#include <kernel.h>

// CONSTANTS

const TUint8 KFaultIdentifierShift = 8;
const TUint8 KChannelNumberShift = 16;
const TUint8 KExtraInfoShift = 24;

// Identifies in which class assert happened.
enum FaultClassCategories
    {
    EDMultiplexerTraceId = 0,
    EDMuxTraceId,
    };

const TUint8 KClassIdentifierShift( 8 );

// MACROS
#define _T( a ) a
#define __TRACE_PRINTF(a) Kern::Printf a

#ifdef _DEBUG
    #if defined COMPONENT_TRACE_FLAG
        #define C_TRACE(a) __TRACE_PRINTF(a)
    #else
        #define C_TRACE(a) 
    #endif // COMPONENT_TRACE_FLAG
#else // _DEBUG
    #define C_TRACE(a)
#endif // _DEBUG
//

// Resets are made both in UDEB and UREL (traces only in udeb)
// Internal asserts
#define ASSERT_RESET_ALWAYS(a,b) if(!(a)) { __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); Kern::Fault( "Multiplexer Fault:", b ); }
#define ASSERT_THREAD_CONTEXT_ALWAYS( a )ASSERT_RESET_ALWAYS( NKern::CurrentContext() == NKern::EThread, a );

// IST API asserts
#define ISTAPI_ASSERT_KERNEL_THREAD_CONTEXT_ALWAYS NKern::CurrentContext() == NKern::EThread /* TODO Kernel thread context && ( Kern::CurrentThread().iThreadType!=EThreadUser || Kern::CurrentThread().iNThread.iCsCount > 0 )*/

#endif // __MUX_TRACE_H__

//  End of File
