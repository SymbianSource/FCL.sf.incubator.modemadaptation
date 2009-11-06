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
#include    "dpparityhandler.h"         // parity bit handling (7-bit data mode)
#include    "dplog.h"                   // dataport logging
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpparityhandlertraces.h"
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
// CDpParityHandler::CDpParityHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDpParityHandler::CDpParityHandler()
    {
    OstTrace0( TRACE_NORMAL, CDPPARITYHANDLER_CDPPARITYHANDLER, "CDpParityHandler::CDpParityHandler" );
    LOGM("CDpParityHandler::CDpParityHandler");

    // Generate lookuptable which is used to find out right parity
    // (Even, Odd, Space, Mark)in 7-bit data mode
    GenerateLookupTable();
    }

// -----------------------------------------------------------------------------
// CDpParityHandler::~CDpParityHandler
// Destructor
// -----------------------------------------------------------------------------
//
CDpParityHandler::~CDpParityHandler()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPPARITYHANDLER_CDPPARITYHANDLER, "CDpParityHandler::~CDpParityHandler" );
    LOGM("CDpParityHandler::~CDpParityHandler");
    }

// -----------------------------------------------------------------------------
// CDpParityHandler::GenerateLookupTable
// Generates lookup table to get different parity bits.
// -----------------------------------------------------------------------------
//
void CDpParityHandler::GenerateLookupTable()
    {
    OstTrace0( TRACE_NORMAL, CDPPARITYHANDLER_GENERATELOOKUPTABLE, "CDpParityHandler::GenerateLookupTable" );
    LOGM("CDpParityHandler::GenerateLookupTable");

    TUint8 mask( 0x1 );
    TInt count( 0 );

    for ( TUint8 s( 0 ); s < KDpParityTableSize; s++ )
        {
        mask = 0x1;
        count = 0;
        for ( TInt i( 0 ); i < 8; i++ )
            {
            if ( 0 != ( s & mask ) )
                {
                count++;
                }
            //no else
            mask = static_cast<TUint8>( mask << 1 );
            }

        if ( 0 == count%2 )
            {
            //even
            iTable[s][0] = s;                                 //parity 0
            //odd
            iTable[s][1] = static_cast<TUint8>( s | 0x80 );   //parity 1
            //space
            iTable[s][2] = s;                                 //parity 0
            //mark
            iTable[s][3] = static_cast<TUint8>( s | 0x80 );   //parity 1
            }
        else
            {
            //even
            iTable[s][0] = static_cast<TUint8>( s | 0x80 );   //parity 1
            //odd
            iTable[s][1] = s;                                 //parity 0
            //space
            iTable[s][2] = s;                                 //parity 0
            //mark
            iTable[s][3] = static_cast<TUint8>( s | 0x80 );   //parity 1
            }
        }
    }

// -----------------------------------------------------------------------------
// CDpParityHandler::AddParityBits
// Add paritybits to descriptor given as parameter.
// -----------------------------------------------------------------------------
//
TInt CDpParityHandler::AddParityBits(
    TPtr8& aDes,
    const TParity aParity )
    {
    OstTrace0( TRACE_NORMAL, CDPPARITYHANDLER_ADDPARITYBITS, "CDpParityHandler::AddParityBits" );
    LOGM("CDpParityHandler::AddParityBits");

    TInt len( aDes.Length() );
    TInt i( 0 );
    TUint8 mask( 0x7F );
    TInt ret( KErrNone );

    // empty descriptor
    if ( len == 0 )
        {
        ret = KErrNotFound;
        }
    else
        {
        switch ( aParity )
            {
            case EParityEven:
                {
                for( i = 0; i < len; i++ )
                    {
                    aDes[i] = Even( static_cast<TUint8>( aDes[i] & mask ) );
                    }
                break;
                }
            case EParityOdd:
                {
                for( i = 0; i < len; i++ )
                    {
                    aDes[i] = Odd( static_cast<TUint8>( aDes[i] & mask ) );
                    }
                break;
                }
            case EParityMark:
                {
                for( i = 0; i < len; i++ )
                    {
                    aDes[i] = Mark( static_cast<TUint8>( aDes[i] & mask ) );
                    }
                break;
                }
            case EParitySpace:
                {
                for( i = 0; i < len; i++ )
                    {
                    aDes[i] = Space( static_cast<TUint8>( aDes[i] & mask ) );
                    }
                break;
                }
            case EParityNone:
                {
                //none
                break;
                }
            default:
                {
                //none
                break;
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpParityHandler::ExtractParityBits
// Extract parity bits from descriptor given as parameter.
// We do not check, whether parity is right or wrong.
// GSM error correction is enough.
// -----------------------------------------------------------------------------
//
TInt CDpParityHandler::ExtractParityBits(
    TPtr8& aDes )
    {
    OstTrace0( TRACE_NORMAL, CDPPARITYHANDLER_EXTRACTPARITYBITS, "CDpParityHandler::ExtractParityBits" );
    LOGM("CDpParityHandler::ExtractParityBits");

    TInt len( aDes.Length() );
    TUint8 mask( 0x7F );
    TInt ret( KErrNone );

    // empty descriptor
    if ( 0 == len )
        {
        ret = KErrNotFound;
        }
    else
        {
        for ( TInt i( 0 ); i < len; i++ )
            {
            // extract parity bit
            aDes[i] = static_cast<TUint8>( aDes[i] & mask );
            }
        }

    return ret;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// none

//  End of File
