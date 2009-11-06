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
* Description:   Specific class for SIM ATK tests
*
*/




// INCLUDE FILES
#include "simatk.h"
#include "satbase.h"

// -----------------------------------------------------------------------------
// CSimAtk::CSimAtk
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSimAtk::CSimAtk
        (
        CTestModuleIf& aTestModuleIf
        ): CSimAtkCore( aTestModuleIf ),
        iSessionEnd( iSat ),
        iParser( NULL ),
        iCallControl( NULL )
    { 
    }

// -----------------------------------------------------------------------------
// CSimAtk::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimAtk::ConstructL()
    {
    CSimAtkCore::ConstructL();
    
    // This sets object type to baseclass to make possible to callback while 
    // notification sequence
    
    CSimAtkCore::SetObjectType( CSimAtkCore::ESimAtk ,this);
 
    iArray = new (ELeave) CArrayPtrFlat<CSatBase>( 2 );
    
    // shall be studied later why this module leaks handles..
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );
    
    }

// -----------------------------------------------------------------------------
// CSimAtk::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimAtk* CSimAtk::NewL
        ( 
        CTestModuleIf& aTestModuleIf
        )
    {
    CSimAtk* self = new (ELeave) CSimAtk( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL(); // Calls SimAtk ConstructL
    CleanupStack::Pop();

    return self;    
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSimAtk::~CSimAtk()
    {
    if ( iArray )
        {
        iArray->ResetAndDestroy();
        delete iArray;
        iArray = NULL;
        }
    
    // If test case is canceled, script does not close resources.
    iRIscTestControl.Close();
    iSat.Close();
    iSmsMessaging.Close();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL
        (
        CTestModuleIf& aTestModuleIf // Backpointer to STIF Test Framework
        )
    {    
    return ( CScriptBase* ) CSimAtk::NewL( aTestModuleIf );        
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {    
    return ( KErrNone );    
    }
    
#endif // EKA2
    
//  End of File
