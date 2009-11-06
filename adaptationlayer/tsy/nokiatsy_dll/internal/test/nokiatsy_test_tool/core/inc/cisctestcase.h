/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Activeobject for handling ISC Test Control
*
*/




#ifndef CISCTESTCASE_H
#define CISCTESTCASE_H


//  INCLUDES
#include "risctestcontrol.h"

// CLASS DECLARATION
// Helper class to handle IscTestControl
class CIscTestCase : public CActive
    {
public:

    /**
    * C++ default constructor.
    */
    CIscTestCase()
    :CActive(EPriorityStandard)
        {
        }


    /**
    * Destructor.
    */
    ~CIscTestCase()
        {
        Cancel();
        // and close isc test control
        iIscTestControl.Close();
        }


    /**
    * Initialize ISC Test Control
    * @param aTestCaseFile XML file containing ISC Test Control test cases
    */
    void InitializeL(const TDesC& aTestCaseFile)
        {
        TInt err;

        err = iIscTestControl.Connect();
        User::LeaveIfError(err);

        err = iIscTestControl.LoadTestCases(aTestCaseFile);
        User::LeaveIfError(err);
        
        CActiveScheduler::Add(this);
        
        }


    /**
    * Runs ISC Test Control Testcase
    * @param aTestControlId isc-test-control-id from testcase XML file
    * @param aTestCaseId    test case id
    */
    void RunTestCase(TInt aTestControlId, TInt aTestCaseId)
        {
        iIscTestControl.RunTestCase((aTestControlId<<20)|aTestCaseId, iStatus);
        SetActive();
        }

    /**
    * Syncronize nokiatsy -side with isc test control side
    */
    void Sync()
        {
        iIscTestControl.SyncTestCase();
        }


    /**
    * ActiveObject RunL, called win ISC Test Control testcase is done
    */
    void RunL()
        {
        if (iStatus.Int() != KErrNone)        
            {
            User::LeaveIfError(iStatus.Int());
            }
        }

    /**
    * ActiveObject cancel
    */
    void DoCancel()
        {
        //
        iIscTestControl.CancelTestCase();
        }


private:
    RIscTestControl iIscTestControl;
    };


#endif      // CISCTESTCASE_H
            
// End of File
