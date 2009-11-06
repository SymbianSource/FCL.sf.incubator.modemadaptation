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
* Description:   Base class of all other test classes
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "core.h"
#include <cmmmessagemanagerbase.h>
#include <cmmmessagemanager.h>
#include <mmmmesshandlerbase.h>
#include "cmmmessagerouterbase.h"
#include "cmmmessagerouter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCore::CCore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCore::CCore( 
	CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    
    }

// -----------------------------------------------------------------------------
// CCore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::BaseConstructL()
    {
    iCoreLog = CStifLogger::NewL( KCoreLogPath, 
                          KCoreLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
                          
    _LIT8(KBaseConstruct, "CORE: In BaseConstructL");
    iCoreLog->Log((TDesC8)KBaseConstruct);
    
    CMmMessageRouterBase* messageRouter;
                          
    iMessageManager = NULL;
    // Create Message Manager (the 'core' of the LTSY Plug-in API)
    iMessageManager = CMmMessageManager::NewL();

	_LIT8(KMessageManager, "iMessageManager OK");
	iCoreLog->Log((TDesC8)KMessageManager);
	messageRouter = CMmMessageRouter::NewL( iMessageManager );

	_LIT8(KMessageRouter, "iMessageRouter OK");
	iCoreLog->Log((TDesC8)KMessageRouter);
	// set the pointer to the message router object
	iMessageManager->SetMessageRouter( messageRouter );
		
    messageRouter->SetCore((CCore*)this);

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
    self->BaseConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CCore::~CCore
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

    if ( iMessageManager )
        {
        //delete messageManager object
        delete iMessageManager;
        iMessageManager = NULL;
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
    iIscTestCase->RunTestCase(aTestControlId, aTestCaseId);
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

//  End of File
