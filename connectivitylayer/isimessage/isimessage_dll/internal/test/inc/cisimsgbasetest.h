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


#ifndef CIsiMsgBaseTest_H
#define CIsiMsgBaseTest_H

// INCLUDES
#include "tisi.h"
#include "mtest.h"                  // For MTest
#include "ctestbase.h"              // For CTestBase

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
class CTestLogger;

/**
*  Implements the IsiMessageBase class that has the needed functions
*  to send and receive IsiMessages.
*
*  @lib ISIMessageCommonTest
*  @since N/A
*/
class CIsiMsgBaseTest : public CBase
    {

    // Functions
    public:
        
        /**
        * Two-phased constructor.
        * @param None
        * @return CIsiMsgBaseTest*, new instance of CIsiMsgBaseTest.
        */
        static CIsiMsgBaseTest* NewL( TExecutionAndDebugInfo& aInfo,
                                      TInt& aResult );        

        /**
        * Two-phased constructor. Leaves pointer to CleanupStack.
        * @param None
        * @return CIsiMsgBaseTest*, new instance of CIsiMsgBaseTest.
        */
        static CIsiMsgBaseTest* NewLC( TExecutionAndDebugInfo& aInfo,
                                       TInt& aResult );

        /**
        * Destructor.
        * @param None
        * @return None
        */
        ~CIsiMsgBaseTest();

        /**
        * For TestCase execution. Closes the connection. 
        * @param None
        * @return None
        */
        void Close();

        /**
        * For TestCase execution. Creates new IsiMsg.
        * @param None
        */
        void GetIsiMsgL();

        /**
        * Set the status of the case as failed / passed.
        * @param TExecutionAndDebugInfo& aInfo, for debugging and results.
        * @param TInt aResult, result of this function.
        * @param const TDesC& aComment, the comment to print.
        * @return None
        */
        void PrintCaseProgress( TInt aResult,
                                const TDesC& aComment
                              );

        /**
        * Helper function return 16bit value from index.
        * @param TInt aIndex
        * @return TUint16
        */
        TUint16 Get16BitFromTestMessage( const TUint8 *aMessage, TInt aIndex );

        /**
        * Helper function return 16bit value from index.
        * @param TInt aIndex
        * @return TUint16
        */
        TUint32 Get32BitFromTestMessage( const TUint8 *aMessage, TInt aIndex );

    protected:

        // None

    private:

        /**
        * Constructor
        * @param None
        * @return None
        */
        CIsiMsgBaseTest( TExecutionAndDebugInfo& aInfo,
                         TInt& aResult );

        /**
        * Second phase constructor.
        * @param None
        * @return None
        */
        void ConstructL();

    // Variables
    public:

        // None

    protected:

        // None

    private:

        // The execution and debug info.
        TExecutionAndDebugInfo& iInfo;
        // Result
        TInt&                   iResult;
        // Writes notices to STIF UI, ATS, and logfile.
        CTestLogger*            iTestLog;


    };

#endif 

// End of File
