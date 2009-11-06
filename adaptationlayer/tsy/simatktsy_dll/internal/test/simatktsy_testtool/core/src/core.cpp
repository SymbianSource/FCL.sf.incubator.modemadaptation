/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "core.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimAtkCore::CSimAtkCore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSimAtkCore::CSimAtkCore
        ( 
        CTestModuleIf& aTestModuleIf 
        ): CScriptBase( aTestModuleIf )
    {
    // Initial values
    iObjectType                         = ECore;  // Object type is set here 
    iLastIndex = 0;
    }

// -----------------------------------------------------------------------------
// CSimAtkCore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSimAtkCore::ConstructL()
    {
     iCoreLog = CStifLogger::NewL( KCoreLogPath, KCoreLogFile,
        CStifLogger::ETxt, CStifLogger::EFile, ETrue );

     iCoreLog->Log( _L( "CSimAtkCore::BaseConstructL Return To Core" ) );     
    }

// -----------------------------------------------------------------------------
// CSimAtkCore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSimAtkCore* CSimAtkCore::NewL
        ( 
        CTestModuleIf& aTestModuleIf
        )
    {
    CSimAtkCore* self = new (ELeave) CSimAtkCore( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }
    
// -----------------------------------------------------------------------------
// CSimAtkCore::~CSimAtkCore
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSimAtkCore::~CSimAtkCore()
    {     
    delete iCoreLog;  
    iCoreLog = NULL;     
    }
    
// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll
        (
        TDllReason /*aReason*/ // Reason code
        )
    {
    return ( KErrNone );    
    }
    
#endif // EKA2
    
//  End of File
