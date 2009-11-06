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



/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This file contains the header file of DemoModule of 
    STIF TestFramework.

-------------------------------------------------------------------------------
*/

#ifndef TEXECUTIONANDDebugINFO
#define TEXECUTIONANDDebugINFO

//INCLUDEs
#include <stiftestmodule.h>         // CTestModuleIf
#include <stiflogger.h>             // CLogger

class TTestResult;
class CTestModuleIf;
class CStifLogger;


// An internal structure containing a test result and the testmodule
// interface. If new test tools will come change those members and 
// you don't have to mess the whole interface design.
class TExecutionAndDebugInfo
    {

    public: 

        TExecutionAndDebugInfo( TTestResult& aResult, CTestModuleIf& aIf,
                                CStifLogger& aLogger, const TPtrC aCaseName
                               )
        : iTestResult( aResult ), iTestInterface( aIf ), iLogger( aLogger ),
        iCaseName( aCaseName )
        {};

    public:

        // CTestBase
        const TPtrC     iCaseName;
        // Ref. to logger.
        CStifLogger&    iLogger;
        // Ref. to Test result.
        TTestResult&    iTestResult;
        // Ref. to Testmodule interface.
        CTestModuleIf&  iTestInterface;

    };

#endif