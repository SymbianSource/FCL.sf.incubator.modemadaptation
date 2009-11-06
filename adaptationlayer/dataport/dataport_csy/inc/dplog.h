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



#if !defined(__LOG_H__)
#define __LOG_H__

#include <flogger.h>

_LIT( KDataportCSYLogFolder, "DATAPORT" );
_LIT( KDataportCSYLogFile, "DATAPORT.TXT" );

#if defined(_DEBUG)

    #ifdef __WINS__
        #define LOG( AAA ) {\
            _LIT( newDes, AAA );\
            RFileLogger::Write(\
                KDataportCSYLogFolder(),\
                KDataportCSYLogFile(),\
                EFileLoggingModeAppend,\
                newDes() );\
            } // LOG

        #define LOG1( AAA, BBB ) {\
            _LIT( newDes, AAA );\
            RFileLogger::WriteFormat(\
                KDataportCSYLogFolder(),\
                KDataportCSYLogFile(),\
                EFileLoggingModeAppend,\
                TRefByValue<const TDesC>( newDes() ),\
                BBB );\
            } // LOG1

        #define LOG2( AAA, BBB, CCC ) {\
            _LIT( newDes, AAA );\
            RFileLogger::WriteFormat(\
                KDataportCSYLogFolder(),\
                KDataportCSYLogFile(),\
                EFileLoggingModeAppend,\
                TRefByValue<const TDesC>( newDes() ),\
                BBB,\
                CCC );\
            } // LOG2
    #else
        #define LOG( AAA ) {\
            _LIT( newDes, AAA );\
            RDebug::Print( newDes );\
            } // LOG

        #define LOG1( AAA, BBB ) {\
            _LIT( newDes, AAA );\
            RDebug::Print( newDes, BBB );\
            } // LOG1

        #define LOG2( AAA, BBB, CCC ) {\
            _LIT( newDes, AAA );\
            RDebug::Print( newDes, BBB, CCC );\
            } // LOG2
    #endif //__WINS__

    #define LOGM( AAA ) { LOG( AAA ) }
    #define LOGM1( AAA, BBB ) { LOG1( AAA, BBB ) }
    #define LOGM2( AAA, BBB, CCC ) { LOG2( AAA, BBB, CCC ) }
    #define LOGHEX( AAA, BBB ) {\
        TBuf<2140> bufferDump;\
        for ( int z = 0; z < AAA.Length() && z < 2139; z++ ) {\
            TUint8 member = *( AAA.Ptr() + z );\
            if ( member > 31 || member < 127 ) {\
                bufferDump.Append( member );\
                }\
            }\
        bufferDump.Append( '\0' );\
        if ( BBB ) {\
            LOG1( "TX = %S", &bufferDump ); }\
        else {\
            LOG1( "RX = %S", &bufferDump ); }\
        } // LOGHEX

    #define LOGMESSAGE( AAA ) {\
        _LIT( KSpace, " ");\
        TBuf<120> bufferDump;\
        TInt length = AAA.Length();\
        if ( 40 < length ) {\
            length = 40; }\
        for ( TInt i = 0; i < length; i++ ) {\
            bufferDump.AppendNumFixedWidthUC( AAA[i], EHex, 2 );\
            bufferDump.Append( KSpace );\
            }\
        if ( 0 < bufferDump.Length() ) {\
            LOG1( "  ==> [ %S]", &bufferDump );\
            }\
        } // LOGMESSAGE

#else

    #define LOGMESSAGE( AAA )
    #define LOGHEX( AAA, BBB )
    #define LOG( AAA )
    #define LOG1( AAA, BBB ) 
    #define LOG2( AAA, BBB, CCC ) 

    #define LOGM( AAA )
    #define LOGM1( AAA, BBB )
    #define LOGM2( AAA, BBB, CCC ) 
    
#endif  // _DEBUG

#endif //__LOG_H__
