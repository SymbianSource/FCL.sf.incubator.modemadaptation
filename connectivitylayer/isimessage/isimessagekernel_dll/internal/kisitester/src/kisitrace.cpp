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
* Description:  Test extension for kernelisimessage
*
*/



// INCLUDE FILES
#include "kisi_trace.h"

#ifdef _DEBUG

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

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
    __TRACE_PRINTF( ( _T("%s: Possible flags are: { __WINS__ | _DEBUG | __KERNEL_MODE__}"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: trace flags are:    { COMPONENT_TRACE_FLAG | API_TRACE_FLAG | EXTENDED_TRACE_FLAG }"), aPrefix.Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: Following flags are supported" ), aPrefix.Ptr() ) );

#ifdef __WINS__
    __TRACE_PRINTF( ( _T( "%s:     __WINS__" ), aPrefix.Ptr() ) );
#endif

#ifdef _DEBUG
    __TRACE_PRINTF( ( _T( "%s:     _DEBUG" ), aPrefix.Ptr() ) );
#endif

#ifdef __KERNEL_MODE__
    __TRACE_PRINTF( ( _T( "%s:     __KERNEL_MODE__" ), aPrefix.Ptr() ) );
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

#endif // _DEBUG

//  End of File  
