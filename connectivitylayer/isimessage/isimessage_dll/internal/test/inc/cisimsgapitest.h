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


#ifndef CIsiMsgApiTest_H
#define CIsiMsgApiTest_H

// INCLUDES
#include <tisi.h>               // For TIsiMsg
//#include <symbianisicommon.h>       // For TIsiMsg
//#include "mtcisi.h"                 // For IsiMtc Defines
//#include "audisi.h"                 // For IsiMAud Defines

#include "tisimsgtestconstants.h"
#include "mtest.h"
#include "ctestbase.h"

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#define MTC_STATE_QUERY_REQ 0x02

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
*  Implements the test IsiMsg 2G Api functions.
*  
*  NOTE!! Change these test's if the API functions of
*  IsiMessage will be changed.
* 
*  @lib isimsg2gtest
*  @since N/A
*/
class CIsiMsgApiTest : public CTestBase
    {

    // Functions
    public:

        /**
        * Static second phase constructor. 
        * @param None
        * @return CIsiMsgApiTest*, new instance of CIsiMsgApiTest.
        */
        static CIsiMsgApiTest* NewL();
        
        /**
        * Destructor.
        * @param None
        * @return None
        */
        ~CIsiMsgApiTest();

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase00L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase01L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase02L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase03L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase04L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase05L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase06L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase07L( TExecutionAndDebugInfo& aInfo );
        
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase08L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase09L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase10L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase11L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase12L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase13L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase14L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase15L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase16L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase17L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase18L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase19L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase20L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase21L( TExecutionAndDebugInfo& aInfo ); 

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase22L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase23L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase24L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase25L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase26L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase27L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase28L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase29L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase30L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase31L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase32L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase33L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase34L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase35L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase36L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase37L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase38L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase39L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase40L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase41L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase42L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase43L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase44L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase45L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase46L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase47L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase48L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase49L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase50L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase51L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase52L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase53L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase54L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase55L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase56L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase57L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase58L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase59L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase60L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase61L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase62L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase63L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase64L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase65L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase66L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase67L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase68L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase69L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase70L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase71L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase72L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase73L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase74L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase75L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase76L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase77L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase78L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase79L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase80L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase81L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase82L( TExecutionAndDebugInfo& aInfo );

        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase83L( TExecutionAndDebugInfo& aInfo );
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase84L( TExecutionAndDebugInfo& aInfo );
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase85L( TExecutionAndDebugInfo& aInfo );
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase86L( TExecutionAndDebugInfo& aInfo );
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase87L( TExecutionAndDebugInfo& aInfo );      
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase88L( TExecutionAndDebugInfo& aInfo ); 
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase89L( TExecutionAndDebugInfo& aInfo );      
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase90L( TExecutionAndDebugInfo& aInfo ); 
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase91L( TExecutionAndDebugInfo& aInfo );                  
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase92L( TExecutionAndDebugInfo& aInfo ); 
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase93L( TExecutionAndDebugInfo& aInfo ); 
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase94L( TExecutionAndDebugInfo& aInfo ); 
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase95L( TExecutionAndDebugInfo& aInfo ); 
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase96L( TExecutionAndDebugInfo& aInfo ); 
        /**
        * From MTest.
        * @param TExecutionAndDebugInfo& aInfo
        * @return TInt KErrNone
        */
        TInt RunClassCase97L( TExecutionAndDebugInfo& aInfo ); 
    protected:

        // None

    private:

        /**
        * Default constuctor.
        * @param None
        * @return None
        */
        CIsiMsgApiTest();

        /**
        * Second phase constructor.
        * @param None
        * @return None 
        */
        void ConstructL();

        /**
        * Initialize the test cases. From CTestBase.
        * @param None
        * @return None
        */
        void InitializeCasesL();

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
