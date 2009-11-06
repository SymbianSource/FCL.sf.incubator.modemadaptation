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


#define ASSERT_RESET ASSERT_RESET_ALWAYS

// INCLUDES
// CONSTANTS

// MACROS
// INCLUDES
#include "usbpntrace.h"

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
void BuildTraceFunc( const TDesC* aPrefix ) // aPrefix is the component name (from developer)
    {

    __TRACE_PRINTF( ( _T("%s: Compiled= __DATE__ - __TIME__"), aPrefix->Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: Possible flags are: { _DEBUG | COMPONENT_TRACE_FLAG | API_TRACE_FLAG | EXTENDED_TRACE_FLAG | OST_TRACE_COMPILER_IN_USE }"), aPrefix->Ptr() ) );
    __TRACE_PRINTF( ( _T("%s: Following flags are supported" ), aPrefix->Ptr() ) );

#ifdef _DEBUG
    __TRACE_PRINTF( ( _T( "%s:     _DEBUG" ), aPrefix->Ptr() ) );
#endif

#ifdef COMPONENT_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     CCOMPONENT_TRACE_FLAG" ), aPrefix->Ptr() ) );
#endif

#ifdef API_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     API_TRACE_FLAG" ), aPrefix->Ptr() ) );
#endif

#ifdef EXTENDED_TRACE_FLAG
    __TRACE_PRINTF( ( _T( "%s:     EXTENDED_TRACE_FLAG" ), aPrefix->Ptr() ) );
#endif

#ifdef OST_TRACE_COMPILER_IN_USE
    __TRACE_PRINTF( ( _T( "%s:     OST_TRACE_COMPILER_IN_USE" ), aPrefix->Ptr() ) );
#endif

    __TRACE_PRINTF( ( _T( "%s: End of Flags supported " ), aPrefix->Ptr() ) );

// and so on .. Define all important flags for this component and print them here!!!

    }

#endif

//  End of File
