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


#ifndef _MEMMANAGERTRACE_H_
#define _MEMMANAGERTRACE_H_

// INCLUDES
#include <kernel.h>

// CONSTANTS

const TUint8 KFaultIdentifierShift = 8;
const TUint8 KChannelNumberShift = 16;
const TUint8 KExtraInfoShift = 24;

// Identifies in which class assert happened.
enum FaultClassCategories
    {
    EDMemmanagerTraceId = 0,
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
#define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( ("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__) )
#define ASSERT_RESET_ALWAYS(a,b) if(!(a)) { __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); Kern::Fault( "ISCE MemMgr:", b ); }
#define ASSERT_THREAD_CONTEXT_ALWAYS( a )ASSERT_RESET_ALWAYS( NKern::CurrentContext() == NKern::EThread, a );

#endif // _MEMMANAGERTRACE_H_

//  End of File
