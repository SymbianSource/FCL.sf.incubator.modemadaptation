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
*     DMC trace.
*
*/


#ifndef DMC_TRACE_H
#define DMC_TRACE_H

#ifndef DMC_INTERNAL_INCLUDE
#error Illegal usage of dmc_trace.h, It is only for DMC internal use !
#endif // DMC_INTERNAL_INCLUDE

// INCLUDES
#ifdef TRACE_MODE_KERNEL
    #include <kernel.h>
#else // TRACE_MODE_KERNEL
    #include <e32svr.h>     // RDebug
#endif // TRACE_MODE_KERNEL

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

#ifdef _DEBUG

#ifdef TRACE_MODE_KERNEL
    #define DMC_TRACE(a)    Kern::Printf a
#else // TRACE_MODE_KERNEL
    #define DMC_TRACE(a)    RDebug::Print a
#endif // TRACE_MODE_KERNEL

#else // _DEBUG 

    #define DMC_TRACE(a)

#endif // _DEBUG

#ifdef TRACE_MODE_KERNEL

    // Macro that prints out the assertion and makes a Kern::Fault if given statement is false
    #define DMC_TRACE_ASSERT_RESET(a, b, c)       \
        if (!(a))                                 \
            {                                     \
            DMC_TRACE(("DMC: Assertion failed, file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); \
            Kern::Fault(b,c);                     \
            }
    
    // Macro that prints out the assertion and makes a Kern::Fault without a conditional statement
    #define DMC_TRACE_ASSERT_RESET_BRUTAL(b, c)       \
            DMC_TRACE(("DMC: Assertion failed, file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); \
            Kern::Fault(b,c);

#else // TRACE_MODE_KERNEL

    // Macro that prints out the assertion and makes a User::Panic if given statement is false
    #define DMC_TRACE_ASSERT_RESET(a, b, c)       \
        if (!(a))                                 \
            {                                     \
            User::Panic(b, c);                    \
            }

#endif // TRACE_MODE_KERNEL

#define DMC_TRACE_ASSERT(a)  \
    if (!(a))                \
        {                    \
        DMC_TRACE(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ )); \
        }

#define DMC_TRACE_ALWAYS_ASSERT  \
        DMC_TRACE(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ ));

#endif // DMC_TRACE_H

//  End of File
