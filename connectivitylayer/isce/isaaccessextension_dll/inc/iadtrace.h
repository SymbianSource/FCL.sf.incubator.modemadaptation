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
#include <kernel.h>

// CONSTANTS

const TUint8 KFaultIdentifierShift = 8;
const TUint8 KChannelNumberShift = 16;
const TUint8 KExtraInfoShift = 24;

enum TIADFaultCategories
    {
    EIADMemoryAllocationFailure = 1,            // 0x01  Ident 23
    EIADNullParameter,                          // 0x02  Ident 30
    EIADWrongParameter,                         // 0x03  Ident 19
    EIADQueueOutOfSync,                         // 0x04  ident 15
    EIADWrongRequest,                           // 0x05  ident 14
    EIADSameRequestTwice,                       // 0x06  ident 0
    EIADChannelNumberNotSpecifiedInInfo,        // 0x07  ident 1
    EIADWrongDFCQueueUsed,                      // 0x08  ident 4
    EIADDoNotCallInISRContext,                  // 0x09  ident 0
    EIADDFCAlreadyQueued,                       // 0x0a  ident 2
    EIADWrongKernelResponse,                    // 0x0b  ident 0
    EIADDesReadFailed,                          // 0x0d  ident 9
    EIADConfigurationInvalid,                   // 0x0e  ident 0
    EIADInvalidCtrlMessage,                     // 0x0f  ident 4
    EIADCmtConnectionNotInit,                   // 0x10  ident 0
    EIADCmtConnectionLost,                      // 0x11  ident 4
    EIADTooManyBytesToPresent,                  // 0x12  ident 0
    EIADChannelOpenedBeforePhysicalLayerInit,   // 0x13  ident 0
    EIADWrongTypeOfOpenPending,                 // 0x14  ident 2
    EIADBufferNotReleased,                      // 0x15  ident 0
    EIADUnknownPipeFCType,                      // 0x16  ident 2
    EIADWrongPipeHandle,                        // 0x17  ident 10
    EIADUnknownPipeOrPepMsg,                    // 0x18  ident 2
    EIADUnkownMedia,                            // 0x19  ident 0
    EIADIndicationOrderFailed,                  // 0x2a  ident 0
    EIADNotSupported,                           // 0x2b  ident 5
    EIADCommon,                                 // 0x2c  ident 0
    EIADOverTheLimits,                          // 0x2d  ident 43
    EIADDesWriteFailed,                         // 0x2e  ident 4
    };
// Used to identify code line where reset happens
enum TIADFaultIdentifier
    {
    EIADFaultIdentifier1 = 1,                   // 0x01
    EIADFaultIdentifier2,                       // 0x02
    EIADFaultIdentifier3,                       // 0x03
    EIADFaultIdentifier4,                       // 0x04
    EIADFaultIdentifier5,                       // 0x05
    EIADFaultIdentifier6,                       // 0x06
    EIADFaultIdentifier7,                       // 0x07
    EIADFaultIdentifier8,                       // 0x08
    EIADFaultIdentifier9,                       // 0x09
    EIADFaultIdentifier10,                      // 0x0a
    EIADFaultIdentifier11,                      // 0x0b
    EIADFaultIdentifier12,                      // 0x0c
    EIADFaultIdentifier13,                      // 0x0d
    EIADFaultIdentifier14,                      // 0x0e
    EIADFaultIdentifier15,                      // 0x0f
    EIADFaultIdentifier16,                      // 0x10
    EIADFaultIdentifier17,                      // 0x11
    EIADFaultIdentifier18,                      // 0x12
    EIADFaultIdentifier19,                      // 0x13
    EIADFaultIdentifier20,                      // 0x14
    EIADFaultIdentifier21,                       // 0x15
    EIADFaultIdentifier22,                       // 0x16
    EIADFaultIdentifier23,                       // 0x17
    EIADFaultIdentifier24,                       // 0x18
    EIADFaultIdentifier25,                       // 0x19
    EIADFaultIdentifier26,                       // 0x20
    EIADFaultIdentifier27,                       // 0x21
    EIADFaultIdentifier28,                       // 0x22    
    EIADFaultIdentifier29,                       // 0x23    
    EIADFaultIdentifier30,                       // 0x24    
    EIADFaultIdentifier31,                       // 0x25    
    EIADFaultIdentifier32,                       // 0x26    
    EIADFaultIdentifier33,                       // 0x27    
    EIADFaultIdentifier34,                       // 0x28    
    EIADFaultIdentifier35,                       // 0x29    
    EIADFaultIdentifier36,                       // 0x2a    
    EIADFaultIdentifier37,                       // 0x2b    
    EIADFaultIdentifier38,                       // 0x2c    
    EIADFaultIdentifier39,                       // 0x2d    
    EIADFaultIdentifier40,                       // 0x2e    
    EIADFaultIdentifier41,                       // 0x2f    
    EIADFaultIdentifier42,                       // 0x30    
    EIADFaultIdentifier43,                       // 0x31    
    };
    
//ISCE
enum TClassIdentifier
    {
    EDISILinkTraceId = 0x00,
    };
const TUint8 KClassIdentifierShift( 8 );

//ISCE

// MACROS
#define _T( a ) a
#define __TRACE_PRINTF(a) Kern::Printf a

#ifdef _DEBUG //UNCOMMENT THESE SOON TO ABLE TRACES ONLY IN UDEB

    void BuildTraceFunc( const TDesC& aPrefix );
    void DataDumpTraceFunc( const TDesC8& aPtr, TBool aSend );

    #define BUILD_TRACE( a ) BuildTraceFunc( a )
#if defined FULL_DATA_DUMP
    #define FULL_DATA_TRACE_DUMP
#endif
#if defined DATA_DUMP
    #define DATA_DUMP_TRACE( a, b ) DataDumpTraceFunc( a, b )
#else
    #define DATA_DUMP_TRACE( a, b )
#endif
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
    #define C_TRACE(a)
    #define A_TRACE(a)
    #define E_TRACE(a)
    #define BUILD_TRACE(a)
    #define DATA_DUMP_TRACE(a, b)
#endif // _DEBUG
//
// Resets are made both in UDEB and UREL (traces only in udeb)
#define ASSERT_RESET_ALWAYS(a,b) if(!(a)) { __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); Kern::Fault( "IAD Fault:", b ); }
#define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( ("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__) )
#define TRACE_ASSERT(a) if (!(a)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ ))
#define TRACE_ASSERT_WITH_MSG(a, b) if (!(a)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__" "__TIME__, __LINE__ ));__TRACE_PRINTF((b));
#define TRACE_ASSERT_INFO(a, b) if (!(a)) __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, extra info 0x%08x, compiled=" __DATE__" "__TIME__, __LINE__, b ));
#define TRACE_WARNING(a, b) if (!(a)) __TRACE_PRINTF(("Warning: file=" __FILE__ ", line=%d, extra info 0x%x, compiled=" __DATE__" "__TIME__, __LINE__, b ));

// Give appropriate context as param, fault if current context different.
#define ASSERT_CONTEXT_ALWAYS( a, b )NKern::TContext current_context=(NKern::TContext)NKern::CurrentContext();ASSERT_RESET_ALWAYS( current_context == a, EIADDoNotCallInISRContext | b<<KChannelNumberShift);
#define ASSERT_PANIC_USER_THREAD_ALWAYS( a, b, c ) if(!(a)){__TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__));Kern::ThreadKill( b, EExitPanic, c, _L8("IAD Fault") );}

//MIST2IADAPI asserts
#define IAD_ASSERT_RESET_ALWAYS( a, b, c ) if(!(a)) { __TRACE_PRINTF(("Assertion failed: file=" __FILE__ ", line=%d, compiled="__DATE__" "__TIME__, __LINE__)); Kern::Fault( c, b ); }
#define IAD_ASSERT_THREAD_CONTEXT_ALWAYS( a, b )IAD_ASSERT_RESET_ALWAYS( NKern::CurrentContext() == NKern::EThread, a, b );


// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif // IADTRACE_H

//  End of File
