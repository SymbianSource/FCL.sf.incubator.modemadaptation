/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class of all other test classes
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "core.h"
#include "cmmessagemanagercallback.h"
#include "tsylogger.h"


static const TUint KIscTestCaseCompletionSignalCode = 1;

// ============================ MEMBER FUNCTIONS ===============================

TTestCaseObserverHelper::TTestCaseObserverHelper(CCore& aCore) :
    iCore(aCore)
    {}

void TTestCaseObserverHelper::IscTestCaseCompleteL()
    {
    iCore.IscTestCaseCompleteL();
    }

// -----------------------------------------------------------------------------
// CCore::CCore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCore::CCore(
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
        iHelper(*this)
    {

    }

// -----------------------------------------------------------------------------
// CCore::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::BaseConstructL( CCore* aTestModule )
    {
    iCoreLog = CStifLogger::NewL( KCoreLogPath,
                          KCoreLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    _LIT8(KBaseConstruct, "CCore:BaseConstructL");
    iCoreLog->Log((TDesC8)KBaseConstruct);

    iTestModule = aTestModule;

    iIscTestCase = new(ELeave) CIscTestCase();

    }

// -----------------------------------------------------------------------------
// CCore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCore* CCore::NewL(
    CTestModuleIf& aTestModuleIf )
    {

    CCore* self = new (ELeave) CCore( aTestModuleIf );
    CleanupStack::PushL( (CScriptBase*)self );
    self->BaseConstructL( self );
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CCore::~CCore()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CCore::~CCore()
    {
    iCoreLog->Log(_L("CCore::~CCore") );

    // Delete resources allocated from test methods
    Delete();

    // delete isc controller
    delete iIscTestCase;

    CloseNTSY();

    // Delete logger
    delete iCoreLog;
    }

// -----------------------------------------------------------------------------
// CCore::CloseNTSY()
// Close/delete NTSY
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::CloseNTSY()
    {
    iCoreLog->Log(_L("CCore::CloseNTSY") );

    if ( iMessageManagerCallBack )
        {
        //delete messageManager object
        delete iMessageManagerCallBack;
        iMessageManagerCallBack = NULL;
        }

    if ( iLtsyFactory )
        {
        iLtsyFactory->Release();
        iLtsyFactory = NULL;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::InitializeIscControlL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::InitializeIscControlL(const TDesC& aIscTestCaseFile)
    {
    iCoreLog->Log(_L("CCore::InitializeIscControl(%S)"), &aIscTestCaseFile );
    iIscTestCase->InitializeL(aIscTestCaseFile);
    }

// -----------------------------------------------------------------------------
// CCCore::SyncIscControl
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::SyncIscControl()
    {
    iCoreLog->Log(_L("CCore::SyncIscControl"));
    iIscTestCase->Sync();
    }

// -----------------------------------------------------------------------------
// CCore::RunIscControlTestCase
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::RunIscControlTestCase(TInt aTestControlId, TInt aTestCaseId)
    {
    iCoreLog->Log(_L("CCore::RunIscControlTestCase (%d,%d)"), aTestControlId, aTestCaseId);
    iIscTestCase->RunTestCase(aTestControlId, aTestCaseId, iHelper);
    }

// -----------------------------------------------------------------------------
// CCore::CreateMMAndLTSYfactoryAndGetMessageRouter
// Create Message Manager, LTSY factory and get message router
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::CreateMMAndLTSYfactoryAndGetMessageRouter( void )
    {
    // Create Message Manager (the 'core' of the LTSY Plug-in API)

    if( !iMessageManagerCallBack )
        {
        iMessageManagerCallBack = CMmMessageManagerCallback::NewL( iTestModule );

        // create LTSY factory
        iLtsyFactory = ( MLtsyFactoryV1* ) LTsyFactoryL();

        // Get message router
        iMessageRouter = iLtsyFactory->GetMessageRouter( *iMessageManagerCallBack );
        }
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CCore::NewL( aTestModuleIf );

    }

// -----------------------------------------------------------------------------
// CCore::IscTestCaseCompleteL
// Handles notification when ISC testcase is done
// -----------------------------------------------------------------------------
//
void CCore::IscTestCaseCompleteL()
    {
    _LIT8(KLog, "CCore::IscTestCaseCompleteL");
    iCoreLog->Log((TDesC8)KLog);
    // to keep the functionality
    Signal(KIscTestCaseCompletionSignalCode);
    }
//  End of File
