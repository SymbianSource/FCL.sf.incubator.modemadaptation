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


#ifndef CTestLogger_H
#define CTestLogger_H

// INCLUDES
#include "texecutionanddebuginfo.h"     // For TExecutionAndDebugInfo
#include "ttestenums.h"                 // For TMsgType, TStatusType

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

/**
*  Implements the IsiMessageBase class that has the needed functions
*  to send and receive IsiMessages.
*
*  @lib ISIMessageCommonTest
*  @since N/A
*/
class CTestLogger : public CBase
    {

    // Functions
    public:

        /**
        * Two-phased constructor.
        * @param None
        * @return CTestLogger*, new instance of CTestLogger.
        */
        static CTestLogger* NewL();        

        /**
        * Two-phased constructor. Leaves pointer to CleanupStack.
        * @param None
        * @return CTestLogger*, new instance of CTestLogger.
        */
        static CTestLogger* NewLC();  

        /**
        * Destructor.
        * @param None
        * @return None
        */
        ~CTestLogger();

        /**
        * Writes IsiMessage to logfile.
        * @param TExecutionAndDebugInfo& aInfo,
        * @param const TDesC& aBuf,
        * @param const TMsgType,
        * @return None
        */
        void PrintHexL( TExecutionAndDebugInfo& aInfo,
                        const TDesC8& aBuf,
                        const TMsgType aType
                      );

        /**
        * Set the status of the case as failed / passed.
        * @param TExecutionAndDebugInfo& aInfo, for debugging and results.
        * @param TInt aResult, result of this function.
        * @param const TDesC& aComment, the comment to print.
        * @return None
        */
        void PrintCaseProgress( TExecutionAndDebugInfo& aInfo,
                                TInt aResult,
                                const TDesC& aComment
                              );

        /**
        * Set the status of the case as failed / passed.
        * @param TExecutionAndDebugInfo& aInfo, for debugging and results.
        * @param TInt aResult, result of this function.
        * @param const TDesC& aFunctionName, name of the current function.
        * @return None
        */
        void PrintCaseStatus( TExecutionAndDebugInfo& aInfo,
                              TInt aResult,
                              const TDesC& aFunctionName,
                              const TStatusType aType
                            );

    private:

        /**
        * Constructor
        * @param None
        * @return None
        */
        CTestLogger();

        void ConstructL();

    // Variables
    public:

        // None

    protected:

        // None

    private:

        // None

    };

#endif 

// End of File
