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



#ifndef __TSYLOGGER_H__
#define __TSYLOGGER_H__

// CONSTANTS
#ifndef _DEBUG

// UREL BUILD:
#define TF_LOGGING_METHOD  0   // No logging in UREL builds

#else

// UDEB BUILD:
#define TF_LOGGING_METHOD  2   // 0 = No logging,
                               // 1 = Flogger,
                               // 2 = RDebug
#endif


#if TF_LOGGING_METHOD > 0
#define TF_LOGGING_ENABLED     // This is for backward compatibility
#endif

// FUNCTION PROTOTYPES
#if TF_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KTfLogFolder,"TF");
_LIT(KTfLogFile,"TFLOG.TXT");

#elif TF_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#endif


/*
-----------------------------------------------------------------------------

	LOGGING MACROs

	USE THESE MACROS IN YOUR CODE !

-----------------------------------------------------------------------------
*/

#if TF_LOGGING_METHOD == 1      // Flogger

#define TFLOGTEXT(AAA)                RFileLogger::Write(KTfLogFolder(),KTfLogFile(),EFileLoggingModeAppend, AAA)
#define TFLOGSTRING(AAA)              /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RFileLogger::Write(KTfLogFolder(),KTfLogFile(),EFileLoggingModeAppend,tempLogDes()); } while (0)
#define TFLOGSTRING2(AAA,BBB)         /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KTfLogFolder(),KTfLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB); } while (0)
#define TFLOGSTRING3(AAA,BBB,CCC)     /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KTfLogFolder(),KTfLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); } while (0)
#define TFLOGSTRING4(AAA,BBB,CCC,DDD) /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KTfLogFolder(),KTfLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD); } while (0)


#elif TF_LOGGING_METHOD == 2    // RDebug

#define TFLOGTEXT(AAA)                RDebug::Print(AAA)
#define TFLOGSTRING(AAA)              /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes); } while (0)
#define TFLOGSTRING2(AAA,BBB)         /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB); } while (0)
#define TFLOGSTRING3(AAA,BBB,CCC)     /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC); } while (0)
#define TFLOGSTRING4(AAA,BBB,CCC,DDD) /*lint --e{717}, --e{1534} */ do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC, DDD); } while (0)

#else	// TF_LOGGING_METHOD == 0 or invalid

#define TFLOGTEXT(AAA)                // Example: TFLOGTEXT(own_desc);
#define TFLOGSTRING(AAA)              // Example: TFLOGSTRING("Test");
#define TFLOGSTRING2(AAA,BBB)         // Example: TFLOGSTRING("Test %i", aValue);
#define TFLOGSTRING3(AAA,BBB,CCC)     // Example: TFLOGSTRING("Test %i %i", aValue1, aValue2);
#define TFLOGSTRING4(AAA,BBB,CCC,DDD) // Example: TFLOGSTRING("Test %i %i %i", aValue1, aValue2, aValue3);

#endif  // TF_LOGGING_METHOD

#if TF_LOGGING_METHOD == 1 || TF_LOGGING_METHOD == 2

// Note, #x and __FILE__ must be stored to char string first, making them 16bit
// strings with _LIT will not work in all compilers.

#define TF_ASSERT(x) /*lint --e{717} */ do { if (!(x)) { const TUint8 tempX8[] = #x; \
	TBuf<sizeof(tempX8)> tempX; \
	tempX.Copy(TPtrC8(tempX8)); const TUint8 tempF8[] = __FILE__; TBuf<sizeof(tempF8)> tempF;\
	tempF.Copy(TPtrC8(tempF8)); \
    TFLOGSTRING4("TSY: ASSERT FAILED: %S, file %S, line %d", &tempX, &tempF, __LINE__ ); } } while(0)

#define TF_ASSERT_NOT_REACHED() /*lint --e{717} */ do {  const TUint8 tempF8[] = __FILE__;\
	 TBuf<sizeof(tempF8)> tempF; tempF.Copy(TPtrC8(tempF8));\
	 TFLOGSTRING3("TSY: ASSERT FAILED: unreachable code, file %S, line %d", &tempF, __LINE__ ); } while (0)

#else  // TF_LOGGING_METHOD == 0 or invalid

#define TF_ASSERT(x)
#define TF_ASSERT_NOT_REACHED()

#endif  // TF_LOGGING_METHOD == 1 || TF_LOGGING_METHOD == 2


#endif	// __TSYLOGGER_H__

//  End of File
