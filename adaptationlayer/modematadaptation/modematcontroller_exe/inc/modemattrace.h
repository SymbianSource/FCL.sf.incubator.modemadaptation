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



#ifndef MODEMATTRACE_H
#define MODEMATTRACE_H

// INCLUDES
// CONSTANTS

// MACROS
// INCLUDES
#include <e32svr.h>    // For RDebug
#include <e32std.h>

_LIT( KATModemPanic, "ATModemSrvPanic" );

#define KMaxLogLineLength 512
#define _T(a) _L(a)


#ifdef _DEBUG

NONSHARABLE_CLASS(TOverflowTruncate16) : public TDes16Overflow
    {
public:
    void Overflow(TDes16& /*aDes*/) {}
    };

NONSHARABLE_CLASS(TOverflowTruncate8) : public TDes8Overflow
    {
public:
    void Overflow(TDes8& /*aDes*/) {}
    };


inline void Trace(TRefByValue<const TDesC16> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    TBuf16<KMaxLogLineLength> theFinalString;
    TOverflowTruncate16 overflow;
    theFinalString.AppendFormatList(aFmt,list,&overflow);
    RDebug::Print(theFinalString);
    }

inline void Trace(TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list, aFmt);
    TOverflowTruncate8 overflow;
    TBuf8<KMaxLogLineLength> buf8;
    buf8.AppendFormatList(aFmt, list, &overflow);
    TBuf16<KMaxLogLineLength> buf16(buf8.Length());
    buf16.Copy(buf8);
    TRefByValue<const TDesC> tmpFmt(_L("%S"));
    RDebug::Print(tmpFmt, &buf16);
    }

inline void DumpMessage( const TDesC8& aPtr )
    {
    RDebug::Print(_L("Length %d "), aPtr.Length());
    const TUint8* data = aPtr.Ptr();
    for( TInt i = 0 ; i < aPtr.Length() ; i++ )
        {
        RDebug::Print(_L("[%d] 0x%x '%c'"),i,data[i],data[i]);
        }
    }

#endif

#define PANIC_TRACE( aMsg, aP1, aP2 )\
    {\
    RDebug::Printf( aMsg, aP1, aP2 );\
    } 

static void Panic( const TDesC8& aFileName, const TInt aLineNum, TBool aPanic )
    {
    PANIC_TRACE( "Assertion failed in file=%S, line=%d", &aFileName, aLineNum );
    if( aPanic )
        {
        TBuf16<KMaxLogLineLength> buf16(aFileName.Length());
        buf16.Copy(aFileName);
        User::Panic( buf16, aLineNum );
        }
    }

#ifdef _DEBUG
    #define BUILD_TRACE(a) BuildTraceFunc(a)
    #define C_TRACE(a) __TRACE_PRINTF(a) 
    #define DUMP_MESSAGE(a) DumpMessage(a)
#ifdef __WINS__
    #define __TRACE_PRINTF(a) RDebug::Print a
#else
    #define __TRACE_PRINTF(a) Trace a
#endif
#else
    #define C_TRACE( a )
    #define BUILD_TRACE( a ) 
    #define DUMP_MESSAGE(a) 
    #define __TRACE_PRINTF(a) 
    #define __ASSERT_FILE__(s) _LIT(KFileName,s)
#endif // _DEBUG

// Resets are made both in UDEB and UREL (traces only in udeb)
#ifdef __WINS__
    #define ASSERT_PANIC_ALWAYS(a) 
    #define ASSERT_ALWAYS 
    #define TRACE_ASSERT_ALWAYS __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) )
    #define TRACE_ASSERT_ALWAYS_COND(a)if(!(a)){ __TRACE_PRINTF( (_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__) );}
#else
    #define ASSERT_PANIC_ALWAYS(a) if(!(a)){Panic( _L8(__FILE__), __LINE__, ETrue );}
    #define ASSERT_ALWAYS Panic( _L8(__FILE__), __LINE__, ETrue )
    #define TRACE_ASSERT_ALWAYS Panic( _L8(__FILE__), __LINE__, EFalse );
    #define TRACE_ASSERT_ALWAYS_COND(a)if(!(a)) {Panic( _L8(__FILE__), __LINE__, EFalse );}
#endif


// Trace asserts activated also in UREL
// define assertions
#ifdef __WINS__
    #define TRACE_ASSERT(a) if (!(a)) {__TRACE_PRINTF((_L("Assertion failed: file= __FILE__ , line=%d, compiled=__DATE__ __TIME__ "),__LINE__));}
#else
    #define TRACE_ASSERT(a) if (!(a)) {Panic( _L8(__FILE__), __LINE__, EFalse );}
#endif  //__WINS__

#endif


// End of File






