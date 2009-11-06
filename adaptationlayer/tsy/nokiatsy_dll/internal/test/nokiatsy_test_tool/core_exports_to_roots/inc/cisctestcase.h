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
// Isc testcase observer interface
class MIscTestCaseObserver
    {
public:
    virtual void IscTestCaseCompleteL() =  0;
    };

// Helper class to handle IscTestControl
class CIscTestCase : public CActive
    {
public:

    /**
    * C++ default constructor.
    */
    CIscTestCase()
    :CActive(KMinTInt),
    iObserver(NULL)
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
    * @param aObserver      object to be notified when testcase is done
    */
    void RunTestCase(TInt aTestControlId, TInt aTestCaseId, MIscTestCaseObserver& aObserver)
        {
        iIscTestControl.RunTestCase((aTestControlId<<20)|aTestCaseId, iStatus);
        SetActive();
        iObserver = &aObserver;
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
        TInt err = iStatus.Int();
        if (KErrCancel != err)
            {
            User::LeaveIfError(err);
            iObserver->IscTestCaseCompleteL();
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
    MIscTestCaseObserver* iObserver;
    RIscTestControl iIscTestControl;
    };


#endif      // CISCTESTCASE_H

// End of File
