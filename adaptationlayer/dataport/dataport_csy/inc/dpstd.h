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



#ifndef DPSTD_H
#define DPSTD_H

// INCLUDES
#include <e32base.h>
#include <e32svr.h>                      

// MACROS

#if defined(USE_FILE_DEBUG)

    void FileDebug( char* aPtr, int id=0, int p1 = 0, int p2 = 0 );
    void FileBuffDebug( TPtr8 );

#if defined(USE_BYTE_COUNT_DEBUG)
    // Only BDEBUG macros used
    // --> smaller log file for HP

    #define RDEBUG( t )
    #define FRDEBUG( e, f )
    #define FRDEBUG2( e, f1, f2 )
    #define TDEBUG( t )

    #define DEBUG( t )
    #define FDEBUG( e, f )
    #define FDEBUG2( e, f1, f2 )

    #define BDEBUG( t ) { FileDebug( t, 0 ); }

    #define BFDEBUG( t, f1 ) { FileDebug( t, 1, f1 ); }

    #define BFDEBUG2( t, f1, f2 ) { FileDebug( t, 2, f1, f2 ); }
#else
    #if defined(TX2PN_LOGGER)
        #define DEBUG( t )
        #define FDEBUG( e, f )
        #define FDEBUG2( e, f1, f2 )
        #define BUFFDEBUG( t ) { FileBuffDebug( t ); }
    #else
        #define DEBUG( t ) { FileDebug( t, 0 ); }
        #define FDEBUG( t, f1 ) { FileDebug( t, 1, f1 ); }
        #define FDEBUG2( t, f1, f2 ) { FileDebug( t, 2, f1, f2 ); }
    #endif  // TX2PN_LOGGER

    #define RDEBUG( t )
    #define FRDEBUG( e, f )
    #define FRDEBUG2( e, f1, f2 )
    #define TDEBUG( t )

#endif  // USE_BYTE_COUNT_DEBUG

#elif defined(USE_SERIAL_DEBUG)

    #define DEBUG( t )
    #define FDEBUG( e, f )
    #define FDEBUG2( e, f1, f2 )
    #define TDEBUG( t )

    _LIT( KLineEnd, "\n" );

    #define RDEBUG( t ) {\
        _LIT( newDes, t );\
        TBuf<128> b( newDes );\
        b.Append( KLineEnd );\
        RDebug::Print( b );\
        }

    #define FRDEBUG( e, f ) {\
        _LIT( newDes, e );\
        TBuf<128> b;\
        b.Format( newDes, f );\
        b.Append( KLineEnd );\
        RDebug::Print( b );\
        }

    #define FRDEBUG2( e, f1, f2 ) {\
        _LIT( newDes, e );\
        TBuf<128> b;\
        b.Format( newDes, f1, f2 );\
        b.Append( KLineEnd );\
        RDebug::Print( b );\
        }

    #define RBUFFDEBUG( t ) {\
        TBuf<320> d;\
        TInt l = t.Length();\
        for ( TInt i( 0 ); i < l; i++ ) {\
            d.Append( t[ i ] );\
            }\
        RDebug::Print( d );\
        }

#elif defined(USE_TRUST_DEBUG)

    void Debug( TDes8& aDes );

    #define DEBUG( t )
    #define FDEBUG( e, f )
    #define FDEBUG2( e, f1, f2 )

    // This one goes to TRUST
    #define TDEBUG( t ) { Debug( t ); }

    _LIT( KLineEnd, "\n" );

    // These goes to serialport
    #define RDEBUG( t ) {\
        _LIT( newDes, t );\
        TBuf<128> b( newDes );\
        b.Append( KLineEnd );\
        RDebug::Print( b );\
        }

    #define FRDEBUG( e, f ) {\
        _LIT( newDes, e );\
        TBuf<128> b;\
        b.Format( newDes, f );\
        b.Append( KLineEnd );\
        RDebug::Print( b );\
        }

    #define FRDEBUG2( e, f1, f2 ) {\
        _LIT( newDes, e );\
        TBuf<128> b;\
        b.Format( newDes, f1, f2 );\
        b.Append( KLineEnd );\
        RDebug::Print( b );\
        }

#else

    #define DEBUG( t )
    #define FDEBUG( e, f )
    #define FDEBUG2( e, f1, f2 )

    #define RDEBUG( t )
    #define FRDEBUG( e, f )
    #define FRDEBUG2( e, f1, f2 )

    #define TDEBUG( t )

#endif

// LOCAL CONSTANTS
// none

// DATA TYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION
// none

#endif  // DPSTD_H

//  End of File
