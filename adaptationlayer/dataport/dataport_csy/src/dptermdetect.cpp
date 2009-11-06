/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "dpdef.h"                   // dataport definitions
#include    "dpdataport.h"              // dataport main and c32 interface
#include    "dptermdetect.h"            // terminator bytes detection
#include    "dpstd.h"                   // fault codes etc.
#include    "dpdataconfig.h"            // configuration store
#include    "dplog.h"                   // dataport logging
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dptermdetecttraces.h"
#endif

// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES
// none

// CONSTANTS
// none

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================

// none


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDpTermDetect::CDpTermDetect
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDpTermDetect::CDpTermDetect(
    CDpDataConfig& aDataConfig ) :
    iDataConfig( aDataConfig )
    {
    OstTrace0( TRACE_NORMAL, CDPTERMDETECT_CDPTERMDETECT, "CDpTermDetect::CDpTermDetect" );
    LOGM("CDpTermDetect::CDpTermDetect");
    }

// -----------------------------------------------------------------------------
// CDpTermDetect::~CDpTermDetect
// Destructor
// -----------------------------------------------------------------------------
//
CDpTermDetect::~CDpTermDetect()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPTERMDETECT_CDPTERMDETECT, "CDpTermDetect::~CDpTermDetect" );
    LOGM("CDpTermDetect::~CDpTermDetect");
    DEBUG( "CDpTermDetect::~CDpTermDetect" )
    }

// -----------------------------------------------------------------------------
// CDpTermDetect::Scan
// This methods scans terminator characters from descriptor.
// -----------------------------------------------------------------------------
//
TInt CDpTermDetect::Scan(
    const TPtr8& aDes )
    {
    OstTrace0( TRACE_NORMAL, CDPTERMDETECT_SCAN, "CDpTermDetect::Scan" );
    LOGM("CDpTermDetect::Scan");
    FDEBUG("CDpTermDetect::Scan %d", 0)

    TInt termCount( iDataConfig.TerminatorCount() );
    TInt ret( KErrNotFound );
    TBool isTerminator (EFalse );

    // The C32 client has set terminator array's characters.
    // Client may change terminators during data receive.
    // --> we must ask it from DataConfig.

    // Terminator array's terminator characters are scanned sequentially.
    for ( TInt j( 0 ); j < termCount; j++ ) //termCount is max 4
        {
        TBuf8<1> terminator;
        terminator.Append(iDataConfig.Terminator(j));

        ret = aDes.Find( terminator );

        if ( 0 <= ret )
            {
            isTerminator = ETrue;
            break;
            }
        //no else
        }

    if ( !isTerminator )
        {
        ret = KErrNotFound;
        }
    //no else

    return ret;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// none


//  End of File
