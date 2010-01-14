/*
* ==============================================================================
*  Name:           sa_common_trace.cpp
*  Part of:        System adaptation
*  Description:    Implementation of System Adaptation common traces
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:59:38 2009 %
*
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
* Implementation of System Adaptation common traces
*
*/

// INCLUDE FILES

#include <e32std.h>
#include <pn_const.h>

#include "sa_common_trace.h"

#ifdef _DEBUG

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KSASDataDumpTraceBytesPerLine( 10 );

_LIT8( KSATraceBuildDate, __DATE__ );
_LIT8( KSATraceBuildTime, __TIME__ );

_LIT( KSATraceDataDumpStart, " [ " );
_LIT( KSATraceDataDumpLineAlign, "   " );
_LIT( KSATraceDataDumpStop, " ] " );

_LIT( KSATraceDataDumpValue, "%02x " );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= TRACE FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// AssertTraceFunc
// -----------------------------------------------------------------------------
void AssertTraceFunc( const TDesC& aPrefix, const TDesC8& aFile, TInt aLine )
    {
    HBufC* buffer = HBufC::New( aFile.Length() + 1 );
    HBufC* date = HBufC::New( 32 );
    HBufC* time = HBufC::New( 32 );

    if ( ( buffer ) && ( date ) && ( time ) )
        {
        buffer->Des().Copy( aFile );
        date->Des().Copy( KSATraceBuildDate );
        time->Des().Copy( KSATraceBuildTime );
        RDebug::Print( _L( "%S Assertion failed: file=%S, line=%d, compiled=%S %S" ),
            &aPrefix, buffer, aLine, date, time );
        }
    else
        {
        RDebug::Print( _L( "Assertion and memory allocation failed" ) );
        }

    delete buffer;
    delete date;
    delete time;
    }


// -----------------------------------------------------------------------------
// BuildTraceFunc
// -----------------------------------------------------------------------------
void BuildTraceFunc( const TDesC& aPrefix )
    {
    HBufC* buffer = HBufC::New( aPrefix.Length() + 1 );
    HBufC* date = HBufC::New( 32 );
    HBufC* time = HBufC::New( 32 );

    if ( ( buffer ) && ( date ) && ( time ) )
        {
        buffer->Des().Copy( aPrefix );
        date->Des().Copy( KSATraceBuildDate );
        time->Des().Copy( KSATraceBuildTime );
        RDebug::Print( _L( "%s: compiled=%s %s" ), buffer->Des().PtrZ(),
            date->Des().PtrZ(), time->Des().PtrZ() );

#ifdef __WINS__
        RDebug::Print( _L( "%s: __WINS__                            enabled" ),
            buffer->Des().PtrZ() );
#else
        RDebug::Print( _L( "%s: __WINS__                            disabled" ),
            buffer->Des().PtrZ() );
#endif

#ifdef _DEBUG
        RDebug::Print( _L( "%s: _DEBUG                              enabled" ),
            buffer->Des().PtrZ() );
#else
        RDebug::Print( _L( "%s: _DEBUG                              disabled" ),
            buffer->Des().PtrZ() );
#endif

#ifdef SA_MODULE_TEST_FLAG
        RDebug::Print( _L( "%s: SA_MODULE_TEST_FLAG                 enabled" ),
            buffer->Des().PtrZ() );
#else
        RDebug::Print( _L( "%s: SA_MODULE_TEST_FLAG                 disabled" ),
            buffer->Des().PtrZ() );
#endif
        }
    delete buffer;
    delete date;
    delete time;
    }


// -----------------------------------------------------------------------------
// DataDumpTraceFunc
// -----------------------------------------------------------------------------
void DataDumpTraceFunc( const TDesC& aPrefix, const TDesC8& aData )
    {
    HBufC* buffer = HBufC::New( 255 ); // TODO: max line length

    if ( buffer )
        {
        buffer->Des().Copy( aPrefix );
        buffer->Des().Append( KSATraceDataDumpStart );
        for ( TInt i = 0; i < aData.Length(); i++)
            {
            buffer->Des().AppendFormat( KSATraceDataDumpValue,  aData[i] );

            if( (i % KSASDataDumpTraceBytesPerLine == ( KSASDataDumpTraceBytesPerLine - 1 ) ) 
                && (i + 1 < aData.Length()) )
                {
                RDebug::Print( buffer->Des() );

                buffer->Des().Copy( aPrefix);
                buffer->Des().Append( KSATraceDataDumpLineAlign );
                }
            }
        buffer->Des().Append( KSATraceDataDumpStop );

        RDebug::Print( buffer->Des() );

        }
    delete buffer;
    }

// -----------------------------------------------------------------------------
// IsiMsgApiTrace
// -----------------------------------------------------------------------------

void IsiMsgApiTrace(const TDesC& aPrefix, const TDesC8& aData)
    {
    }

#endif

//  End of File  
