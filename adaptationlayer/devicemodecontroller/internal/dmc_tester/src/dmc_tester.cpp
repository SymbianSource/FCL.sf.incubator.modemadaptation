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
*     Implementation of DMC tester.
*
*/


// INCLUDES
#include <e32base.h>        // CActive
#include <e32cons.h>
#include <e32des8.h>
#include <f32file.h>        // RDebug
#include <flogger.h>        // RFileLogger
#include <e32test.h>        // RTest 

#include "dmc_user_if.h"

// CONSTANTS
_LIT(KDmcTesterName, "dmc_tester");

TInt OpenDmc(RDmc& aDmc);
/**
 *  Class for receiving events.
 */
class CDmcTstEventHandler : public CActive
    {
    public:
        static CDmcTstEventHandler* NewL(RDmc& aDmc);
        /**
        * C++ default constructor.
        */
        CDmcTstEventHandler(RDmc& aDmc);

        /**
        * Destructor.
        */
        virtual ~CDmcTstEventHandler();

    private: // From CActive

        void RunL();
        void DoCancel();
    
    private: // Data
        RDmc iDmc;
        RDmc::TEvent iEvent;
    };

/**
 *  Small helper class that hides the interface specific
 *  operations and makes the test sequence more readable. 
 */
class DmcTester : public CActive
    {
    public:
        static DmcTester* NewL(RDmc& aDmc);
        /**
        * C++ default constructor.
        */
        DmcTester(RDmc& aDmc);

        /**
        * Destructor.
        */
        virtual ~DmcTester();
        RDmc& Initialize();
        void  GetTargetStartUpMode();
        void  PowerOff();
        void  GenerateReset();
        void  GetHiddenResetStatus();
        void  AskNextMode();

    private: // From CActive
        void RunL();
        void DoCancel();

    private: // Data
        RDmc iDmc;      // Dmc logical device driver
        RTest iTest;    // test app
        CConsoleBase* iConsolePtr;
    };


/**
 * Execute tests.
 */
void ExecuteDmcTestsL()
    {
    RDebug::Printf("DMC:TST_TESTER: ExecuteDmcTestsL() # IN");

    // Create the active scheduler
    RDebug::Printf("DMC:TST_TESTER: Initialize() - Create activeSchedulerPtr");
    CActiveScheduler* activeSchedulerPtr = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(activeSchedulerPtr);

    RDebug::Printf("DMC:TST_TESTER: Initialize() - Install activeSchedulerPtr");
    CActiveScheduler::Install(activeSchedulerPtr);

    /* Client 1 */
    RDmc dmc1;
    
    RDebug::Printf("DMC:TST_TESTER: ExecuteDmcTestsL() - DmcTester::CDmcTstEventHandler::NewL()");
    DmcTester* dmcTesterPtr = DmcTester::NewL(dmc1);
    CleanupStack::PushL(dmcTesterPtr);

#if 0
    /* Client 2 for events*/
    RDmc dmc2;

    RDebug::Printf("DMC:TST_TESTER: Initialize() - Client 2, CDmcTstEventHandler::NewL()");
    CDmcTstEventHandler* eventHandlerPtr = CDmcTstEventHandler::NewL(dmc2);
    CleanupStack::PushL(eventHandlerPtr);

    /* Client 3 for events*/
    RDmc dmc3;

    RDebug::Printf("DMC:TST_TESTER: Initialize() - Client 3, CDmcTstEventHandler::NewL()");
    CDmcTstEventHandler* eventHandlerPtr2 = CDmcTstEventHandler::NewL(dmc3);
    CleanupStack::PushL(eventHandlerPtr2);
#endif // 0

    RDebug::Printf("DMC:TST_TESTER: Initialize() - CActiveScheduler::Start()");
    CActiveScheduler::Start();
    
    RDebug::Printf("DMC:TST_TESTER: Initialize() - CleanupStack::PopAndDestroy()");
    CleanupStack::PopAndDestroy(2, activeSchedulerPtr);
    
    RDebug::Printf("DMC:TST_TESTER: ExecuteDmcTestsL() # OUT");
    }


/**
 *  Open RDmc channel
 */
TInt OpenDmc(RDmc& aDmc)
    {
    RDebug::Printf("DMC:TST_TESTER: OpenDmc() # IN");

    TInt err(KErrNone);
    
    err = aDmc.Open();

    if (err != KErrNone)
        {
        RDebug::Printf("DMC:TST_TESTER: OpenDmc() - DMC opening Not OK");
        aDmc.Close();
        }
    else
        {
        RDebug::Printf("DMC:TST_TESTER: OpenDmc() - DMC opening OK");
        }
   
    RDebug::Printf("DMC:TST_TESTER: OpenDmc() # OUT");

    return err;
    }


/******************************************************
 *
 *    Implementation of DmcTester.
 *   
 *
 ******************************************************/


/**
 * Constuctor
 */
DmcTester::DmcTester(RDmc& aDmc)
    : CActive(CActive::EPriorityStandard)
    , iDmc(aDmc)
    , iTest(KDmcTesterName)
    {
    RDebug::Printf("DMC:TST_TESTER: DmcTester::DmcTester() # IN");
    iTest.Printf(_L("* DMC_TESTER *\n"));

    RDebug::Printf("DMC:TST_TESTER: DmcTester() - Add()");
    CActiveScheduler::Add(this);
    
    //RDebug::Printf("DMC:TST_TESTER: DmcTester() - iConsolePtr: 0x%x", iConsolePtr);
    //CConsoleBase* iConsolePtr = iTest.Console();

    TRequestStatus* statusPtr = &iStatus;

    RDebug::Printf("DMC:TST_TESTER: DmcTester() - *statusPtr: 0x%x", statusPtr);
    RDebug::Printf("DMC:TST_TESTER: DmcTester() - User::RequestComplete, KErrNone");
    User::RequestComplete(statusPtr, KErrNone);

    if (!IsActive())
        {
        RDebug::Printf("DMC:TST_TESTER: DmcTester() - SetActive()");
        SetActive();
        }
    else
        {
        RDebug::Printf("DMC:TST_TESTER: DmcTester() - !IsActive() -> else");    
        }

    RDebug::Printf("DMC:TST_TESTER: DmcTester::DmcTester() # OUT");
    }
 
 
/**
 *  Constructor
 */
DmcTester* DmcTester::NewL(RDmc& aDmc)
    {
    RDebug::Printf("DMC:TST_TESTER: NewL() # IN");

    RDebug::Printf("DMC:TST_TESTER: NewL() - OpenDmc");
    (void)OpenDmc(aDmc);

    DmcTester* self = new (ELeave)DmcTester(aDmc);

    RDebug::Printf("DMC:TST_TESTER: NewL() # OUT");
    
    return self;
    }

 
/**
 *  Closes and releases DMC
 */
DmcTester::~DmcTester()
    {
    RDebug::Printf("DMC:TST_TESTER: ~DmcTester() # IN");

    RDebug::Printf("DMC:TST_TESTER: ~DmcTester() - Close()");
    iDmc.Close();

    RDebug::Printf("DMC:TST_TESTER: ~DmcTester # OUT");
    }


/**
 *  Ask the target startup mode
 */
void DmcTester::GetTargetStartUpMode()
    {
    RDebug::Printf("DMC:TST_TESTER: GetTargetStartUpMode() # IN");
    
    RDmc::TTargetStartupMode dmcTargetMode(RDmc::EStartupModeNone);

    RDebug::Printf("DMC:TST_TESTER: GetTargetStartUpMode() - Asking DMC target startup mode");

    TInt err = iDmc.GetStartUpMode(dmcTargetMode);

    if (err == KErrNone)
        {
        switch(dmcTargetMode)
            {
            case RDmc::EStartupModeNone:
                {
                iTest.Printf(_L("RDmc::EStartupModeNone\n"));
                }
                break;
                
            case RDmc::EStartupModeNormal:
                {
                iTest.Printf(_L("RDmc::EStartupModeNormal\n"));
                }
                break;

            case RDmc::EStartupModeCharging:
                {
                iTest.Printf(_L("RDmc::EStartupModeCharging\n"));
                }
                break;

            case RDmc::EStartupModeAlarm:
                {
                iTest.Printf(_L("RDmc::EStartupModeAlarm\n"));
                }
                break;
                
            case RDmc::EStartupModeTest:
                {
                iTest.Printf(_L("RDmc::EStartupModeTest\n"));
                }
                break;

            case RDmc::EStartupModeFota:
                {
                iTest.Printf(_L("RDmc::EStartupModeFota\n"));
                }
                break;

            default:
                iTest.Printf(_L("Unknown starup mode\n"));
                err = KErrGeneral;
                break;
            }
        }
        
    if (err)
        {
        iTest.Printf(_L("Asking target startup mode return error: %d\n"), err);
        User::Panic(KDmcTesterName, err);
        }

    RDebug::Printf("DMC:TST_TESTER: GetTargetStartUpMode() # OUT");
    }


/**
 *  Power OFF the device
 */

void DmcTester::PowerOff()
    {
    RDebug::Printf("DMC:TST_TESTER: PowerOff() # IN");

    TInt err = iDmc.PowerOff();
    RDebug::Printf("DMC:TST_TESTER: PowerOff() - err: %d", err);

    err = UserHal::SwitchOff();
    RDebug::Printf("DMC:TST_TESTER: PowerOff() - UserHal::SwitchOff: %d", err);

    RDebug::Printf("DMC:TST_TESTER: PowerOff() # OUT");
    }


/**
 *  Generate reset
 */

void DmcTester::GenerateReset()
    {
    RDebug::Printf("DMC:TST_TESTER: GenerateReset() # IN");

    // Generate reset
    RDebug::Printf("DMC:TST_TESTER: GenerateReset() - RDmc::EStartupModeTest");

    RDmc::TTargetStartupMode dmcTargetMode(RDmc::EStartupModeTest);

    TInt err = iDmc.GenerateReset(dmcTargetMode);
    RDebug::Printf("DMC:TST_TESTER: GenerateReset() - err: %d", err);

    err = UserHal::SwitchOff();
    RDebug::Printf("DMC:TST_TESTER: GenerateReset() - UserHal::SwitchOff: %d", err);

    RDebug::Printf("DMC:TST_TESTER: GenerateReset() # OUT");
    }
    


/**
 *  Get Hidden Reset Status
 */

void DmcTester::GetHiddenResetStatus()
    {
    RDebug::Printf("DMC:TST_TESTER: GetHiddenResetStatus() # IN");

    TBool hidden(EFalse);

    TInt err = iDmc.IsHiddenReset(hidden);

    RDebug::Printf("DMC:TST_TESTER: GetHiddenResetStatus() - err: %d, hidden: %d",
                    err, hidden);

    RDebug::Printf("DMC:TST_TESTER: GetHiddenResetStatus() # OUT");
    }


/**
 *  Asks next boot mode from user
 */
void DmcTester::AskNextMode()
    {
    RDebug::Printf("DMC:TST_TESTER: AskNextMode() # IN");

    iTest.Printf(_L("\nGive test case number."));
    iTest.Printf(_L("\n%d: Get Start-up Mode"),       1);
    iTest.Printf(_L("\n%d: Power OFF"),               2);
    iTest.Printf(_L("\n%d: Generate Reset"),          3);
    iTest.Printf(_L("\n%d: Get Hidden Reset Status"), 4);
    iTest.Printf(_L("\n%d: Quit"),                    0);

    TUint menu_choice = (TUint)iTest.Getch();
    
    // ascii to decimal translation:
    if (menu_choice >= 0x30)
        {
        menu_choice -= 0x30;
        }

    switch (menu_choice)
        {
        case 1: 
            {
            iTest.Printf(_L("Get Start-up Mode\n\n"));
            GetTargetStartUpMode();
            }
            break;

        case 2:
            {
            iTest.Printf(_L("Power OFF\n\n"));
            PowerOff();
            }
            break;

        case 3:
            {
            iTest.Printf(_L("Generate Reset\n\n"));
            GenerateReset();
            }
            break;

        case 4:
            {
            iTest.Printf(_L("Get Hidden Reset Status\n\n"));
            GetHiddenResetStatus();
            }
            break;

        case 0:
            {
            iTest.Printf(_L("Quit\n\n"));
            }
            break;

        default:
            {
            iTest.Printf(_L("Pressed 0x%02x (not supported)\n\n"), menu_choice);
            }
            break;
        }

    TRequestStatus* statusPtr = &iStatus;

    if (menu_choice == 0)
        {
        RDebug::Printf("DMC:TST_TESTER: AskNextMode(), Test finished");
        RDebug::Printf("DMC:TST_TESTER: AskNextMode() - User::RequestComplete(), KErrGeneral");
        User::RequestComplete(statusPtr, KErrGeneral);
        }
    else
        {
        RDebug::Printf("DMC:TST_TESTER: AskNextMode() - User::RequestComplete(), KErrNone");
        User::RequestComplete(statusPtr, KErrNone);
        }

    RDebug::Printf("DMC:TST_TESTER: AskNextMode() # OUT");
    }


/**
 *  RunL()
 */
void DmcTester::RunL()
    {
    RDebug::Printf("DMC:TST_TESTER: RunL() # IN");
 
    RDebug::Printf("DMC:TST_TESTER: RunL() - iStatus.Int(): %d",
                    iStatus.Int());
    RDebug::Printf("DMC:TST_TESTER: RunL() - &iStatus: 0x%x", &iStatus);

    switch (iStatus.Int())
        {
        case KErrNone:
            {
            RDebug::Printf("DMC:TST_TESTER: RunL() - KErrNone");
            // Show menu and ask a next test step
            AskNextMode();
            //TKeyCode keyCode = iConsolePtr->KeyCode();
            //RDebug::Printf("DMC:TST_TESTER: RunL() - keyCode: 0x%x", keyCode);
            
            //TText8 ch = (TText8)keyCode & 0xFF;
            //TText8 ch = iConsolePtr->Getch();
            //RDebug::Printf("DMC:TST_TESTER: RunL() - ch: 0x%x", ch);

            // Continue receiving events
            RDebug::Printf("DMC:TST_TESTER: RunL() - SetActive()");
            SetActive();
            }
            break;

        case KErrGeneral:
            {
            RDebug::Printf("DMC:TST_TESTER: RunL - CActiveScheduler::Stop()");
            CActiveScheduler::Stop();
            }
            break;

        default:
            {
            RDebug::Printf("DMC:TST_TESTER: RunL - default");
            }
            break;
        }

    RDebug::Printf("DMC:TST_TESTER: RunL # OUT");
    }


/**
 *  DoCancel()
 */
void DmcTester::DoCancel()
    {
    RDebug::Printf("DMC:TST_TESTER: DoCancel() - # IN");

    RDebug::Printf("DMC:TST_TESTER: DoCancel() - # OUT");
    }

/******************************************************
 *
 *    Implementation of CDmcTstEventHandler.
 *    Handles DMC events.
 *
 ******************************************************/

 
/**
 *  Default constructor
 */
CDmcTstEventHandler::CDmcTstEventHandler(RDmc& aDmc)
    : CActive(CActive::EPriorityStandard)
    , iDmc(aDmc)
    {
    RDebug::Printf("DMC:TST_EVHAND: CDmcTstEventHandler() # IN");
 
    RDebug::Printf("DMC:TST_EVHAND: CDmcTstEventHandler() - Add()");
    CActiveScheduler::Add(this);

    // Subscribe events
    RDebug::Printf("DMC:TST_EVHAND: CDmcTstEventHandler - ReceiveEvents(), iStatus: 0x%x", &iStatus);
    iDmc.ReceiveEvents(iStatus, iEvent);

    if (!IsActive())
        {
        RDebug::Printf("DMC:TST_EVHAND: CDmcTstEventHandler() - SetActive()");
        SetActive();
        }
    else
        {
        RDebug::Printf("DMC:TST_EVHAND: CDmcTstEventHandler() - !IsActive() -> else");    
        }

    RDebug::Printf("DMC:TST_EVHAND: CDmcTstEventHandler() # OUT");
    }


/**
 *  Constructor
 */
CDmcTstEventHandler* CDmcTstEventHandler::NewL(RDmc& aDmc)
    {
    RDebug::Printf("DMC:TST_EVHAND: NewL() # IN");

    RDebug::Printf("DMC:TST_EVHAND: NewL() - OpenDmc");
    (void)OpenDmc(aDmc);

    CDmcTstEventHandler* self = new (ELeave)CDmcTstEventHandler(aDmc);

    RDebug::Printf("DMC:TST_EVHAND: NewL() # OUT");
    
    return self;
    }
 
 
/**
 *  Destructor
 */
CDmcTstEventHandler::~CDmcTstEventHandler()
    {
    RDebug::Printf("DMC:TST_EVHAND: ~CDmcTstEventHandler() # IN");
 
    RDebug::Printf("DMC:TST_EVHAND: RunL - CActiveScheduler::Cancel()");
    Cancel();

    RDebug::Printf("DMC:TST_EVHAND: ~CDmcTstEventHandler() # OUT");
    }


/**
 *  RunL()
 */
void CDmcTstEventHandler::RunL()
    {
    RDebug::Printf("DMC:TST_EVHAND: RunL() # IN");

    RDebug::Printf("DMC:TST_EVHAND: RunL() - iStatus.Int(): %d, iEvent: %d",
                    iStatus.Int(), iEvent);
    RDebug::Printf("DMC:TST_EVHAND: CDmcTstEventHandler() - &iStatus: 0x%x", &iStatus);

    switch (iStatus.Int())
        {
        case KErrNone:
            {
            if (iEvent == RDmc::EResetEvent)
                {
                RDebug::Printf("DMC:TST_EVHAND: RunL() - RDmc::EResetEvent");
                }

            if (iEvent == RDmc::EPowerOffEvent)
                {
                RDebug::Printf("DMC:TST_EVHAND: RunL() - RDmc::EPowerOffEvent");
                }

            RDebug::Printf("DMC:TST_EVHAND: RunL() - UserHal::SwitchOff()");

            // Make switch off both in case of reset and power off
            TInt err = UserHal::SwitchOff();
            }
            break;

        default:
            {
            RDebug::Printf("DMC:TST_EVHAND: RunL - default");
            }
            break;
        }
 
#if 0 
    TRequestStatus& status    = iStatus;
    TRequestStatus* statusPtr = &status;
    User::RequestComplete(statusPtr, KErrNone);
#endif // 0

    // Continue receiving events
    RDebug::Printf("DMC:TST_EVHAND: RunL - SetActive()");
    SetActive();

    RDebug::Printf("DMC:TST_EVHAND: RunL # OUT");
    }


/**
 *  DoCancel()
 */
void CDmcTstEventHandler::DoCancel()
    {
    RDebug::Printf("DMC:TST_EVHAND: DoCancel() - # IN");
 
    RDebug::Printf("DMC:TST_EVHAND: DoCancel() - # OUT");
    }



/**
 * The main entrypoint for application
 */
GLDEF_C TInt E32Main()
    {
    RDebug::Printf("DMC:TST_TESTER: E32Main\n");

    __UHEAP_MARK;
    
     // get clean-up stack
    CTrapCleanup* cleanupPtr = CTrapCleanup::New();

    // start scheduler 
    TRAPD(error, ExecuteDmcTestsL());

    // destroy a clean-up stack
    delete cleanupPtr;
    __UHEAP_MARKEND;

    return error;
    }

// End of File
