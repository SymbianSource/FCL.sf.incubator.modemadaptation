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



// INCLUDE FILES

#include "isimessagetrace.h"

#ifdef _DEBUG

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KIsiMessageDataDumpTraceBytesPerLine = 10;
const TInt KIsiMessageDataDumpTraceBytesPerData = 3;

_LIT( KIsiMessageTraceDataDumpStart, " [ " );
_LIT( KIsiMessageTraceDataDumpLineAlign, "   " );
_LIT( KIsiMessageTraceDataDumpStop, " ] " );
_LIT( KIsiMessageTraceDataDumpValue, "  " );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= TRACE FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// BuildTraceFunc
// -----------------------------------------------------------------------------
void BuildTraceFunc( const TDesC& aPrefix ) // aPrefix is the component name (from developer)
    {

    __TRACE_PRINTF( ( _T("%s: Compiled=%s - %s"), aPrefix.Ptr(), __DATE__, __TIME__ ) );
    __TRACE_PRINTF( ( _T("%s: Possible flags are: { __WINS__ | _DEBUG | COMPONENT_TRACE_FLAG | API_TRACE_FLAG | EXTENDED_TRACE_FLAG }"), aPrefix.Ptr() ) );
	__TRACE_PRINTF( ( _T("%s: Following flags are supported" ), aPrefix.Ptr() ) );

#ifdef __WINS__
	__TRACE_PRINTF( ( _T( "%s:     __WINS__" ), aPrefix.Ptr() ) );
#endif

#ifdef _DEBUG
	__TRACE_PRINTF( ( _T( "%s:     _DEBUG" ), aPrefix.Ptr() ) );
#endif

#ifdef COMPONENT_TRACE_FLAG
	__TRACE_PRINTF( ( _T( "%s:     COMPONENT_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef API_TRACE_FLAG
	__TRACE_PRINTF( ( _T( "%s:     API_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

#ifdef EXTENDED_TRACE_FLAG    
	__TRACE_PRINTF( ( _T( "%s:     EXTENDED_TRACE_FLAG" ), aPrefix.Ptr() ) );
#endif

    __TRACE_PRINTF( ( _T( "%s: End of Flags supported " ), aPrefix.Ptr() ) );

// and so on .. Define all important flags for this component and print them here!!!

    }


// -----------------------------------------------------------------------------
// DataDumpTraceFunc
// -----------------------------------------------------------------------------
void DataDumpTraceFunc( const TDesC& aPrefix, const TDesC8& aData )
    {
#if defined TRACE_MODE_KERNEL // from stack

    TBuf<255> buffer; // modify this to your purposes
        // buffer max length is needed for prefix + max line length output (each data byte is represended using 3 bytes)
        // so for example default KIsiMessageDataDumpTraceBytesPerLine == 10 means 30 bytes + KIsiMessageTraceDataDumpStart = 3 bytes => 33 bytes + Prefix
    if ( ( aPrefix.Length() + ( ( const TDesC& ) KIsiMessageTraceDataDumpStart ).Length() + ( KIsiMessageDataDumpTraceBytesPerLine * KIsiMessageDataDumpTraceBytesPerData ) ) < buffer.MaxLength() )
        {
        buffer.Append( aPrefix );
        buffer.Append( KIsiMessageTraceDataDumpStart );
        TInt i = 0;
        for ( i = 0; i < aData.Length(); i++)
            {
            if( ( i % KIsiMessageDataDumpTraceBytesPerLine == ( KIsiMessageDataDumpTraceBytesPerLine - 1 ) ) && (i + 1 < aData.Length() ) )
                {
                buffer.Copy( aPrefix );
                buffer.Append( KIsiMessageTraceDataDumpLineAlign );
                }
            }
        buffer.Append( KIsiMessageTraceDataDumpStop );
                           
        }
    else
        {
        C_TRACE( ( _T( "ERROR: DataDumpTraceFunc - aPrefix too long!" ) ) );
        }

#else

	#define _HBUFC_MAX 255	// MaxLength (HBufC) was not available, don't know if it has been deprecated or something...
    HBufC* buffer = HBufC::New( _HBUFC_MAX ); // from heap to save stack // modify this to your purposes
    // buffer max length is needed for prefix + max line length output (each data byte is represended using 3 bytes)
    if ( buffer != NULL )
    if ( ( buffer != NULL ) && 
        ( ( aPrefix.Length() + ( ( const TDesC& ) KIsiMessageTraceDataDumpStart ).Length() + ( KIsiMessageDataDumpTraceBytesPerLine * KIsiMessageDataDumpTraceBytesPerData ) ) < _HBUFC_MAX ) )
        {
        buffer->Des().Copy( aPrefix );
        buffer->Des().Append( KIsiMessageTraceDataDumpStart );
        for ( TInt i = 0; i < aData.Length(); i++)
            {
            buffer->Des().AppendFormat( KIsiMessageTraceDataDumpValue,  aData[i] );
            
            if( (i % KIsiMessageDataDumpTraceBytesPerLine == ( KIsiMessageDataDumpTraceBytesPerLine - 1 ) ) && (i + 1 < aData.Length()) )
                {
                buffer->Des().Copy( aPrefix );
                buffer->Des().Append( KIsiMessageTraceDataDumpLineAlign );
                }
            }
        buffer->Des().Append( KIsiMessageTraceDataDumpStop );
                          
        }
    delete buffer;
#endif // TRACE_MODE_KERNEL
    }
    
#endif

//  End of File  
