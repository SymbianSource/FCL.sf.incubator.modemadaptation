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

#ifndef SA_COMMON_TRACE_H
#define SA_COMMON_TRACE_H

// INCLUDES
#include <e32svr.h>

// PC-LINT OPTIONS
// Removes null statement not in line by itself warnings from 
// COMPONENT_TRACE macros
//lint -esym(960,54)

// Removes "area too small" pointer cast warnings.
//lint -e826

// CONSTANTS

// MACROS

#ifdef _DEBUG

    void BuildTraceFunc( const TDesC& aPrefix );
    void AssertTraceFunc( const TDesC& aPrefix, const TDesC8& aFile, TInt aLine );
    void DataDumpTraceFunc( const TDesC& aPrefix, const TDesC8& aData );
    void IsiMsgApiTrace( const TDesC& aPrefix, const TDesC8& aData );

    #define BUILD_TRACE( a ) BuildTraceFunc( a )
    #define ASSERT_ALWAYS_TRACE( a ) AssertTraceFunc( a, _L8( __FILE__ ), __LINE__ )
    #define ASSERT_TRACE( a, b) if( !b ) { ASSERT_ALWAYS_TRACE( a ); }

    #define ADD_QUOTES(in) #in
    #define BUILD_STRING(in) ADD_QUOTES(in)

    #ifdef API_TRACE_FLAG

        #define API_TRACE( a ) RDebug::Print a
        #define DATA_DUMP_TRACE( a, b ) DataDumpTraceFunc( a, b )
        #define ISIMSG_API_TRACE( a, b ) IsiMsgApiTrace( a, b )

    #else // API_TRACE_FLAG

        #define API_TRACE( a )
        #define DATA_DUMP_TRACE( a, b )
        #define ISIMSG_API_TRACE( a, b )

    #endif // API_TRACE_FLAG

    #ifdef INTERNAL_TRACE_FLAG

        #define INTERNAL_TRACE( a ) RDebug::Print a 

    #else // INTERNAL_TRACE_FLAG

        #define INTERNAL_TRACE( a )

    #endif // INTERNAL_TRACE_FLAG

#else // _DEBUG

    #define BUILD_TRACE( a )
    #define ASSERT_TRACE( a, b)
    #define ASSERT_ALWAYS_TRACE( a )
    #define API_TRACE( a )
    #define INTERNAL_TRACE( a )
    #define DATA_DUMP_TRACE( a, b )
    #define ISIMSG_API_TRACE( a, b )

#endif // _DEBUG

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif //SA_COMMON_TRACE_H

//  End of File

