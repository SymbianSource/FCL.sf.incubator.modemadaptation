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
* Description:   DisplayText specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commanddisplaytext.h"

// CONSTANTS
_LIT( KPriority,            "Priority" );
_LIT( KClearScreenTrigger,  "ClearScreenTrigger" );
_LIT( KImmediateRsp,        "ImmediateRsp" );
_LIT( KText,                "Text" );


// -----------------------------------------------------------------------------
// CCommandDisplayText::CCommandDisplayText
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandDisplayText::CCommandDisplayText
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::EDisplayText ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandDisplayText::~CCommandDisplayText
// Destructor
// -----------------------------------------------------------------------------
//
CCommandDisplayText::~CCommandDisplayText()
    {
    }

// -----------------------------------------------------------------------------
// CCommandDisplayText::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandDisplayText::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );
    
    iSat.NotifyDisplayTextPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandDisplayText::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandDisplayText::CheckData()
    {
    iLog.Log( _L( "CCommandDisplayText::CheckData" ) );
    
    TInt ret( KErrNone );
    
    // Priority
    if ( KErrNone != CheckValue( KPriority, iData.iPriority, 
        RSat::ENormalPriority ) )
        {
        ret = KErrCorrupt;
        }

    // ClearScreenTrigger
    if ( KErrNone != CheckValue( KClearScreenTrigger,
        iData.iClearScreenTrigger, RSat::EUserClear ) )
        {
        ret = KErrCorrupt;
        }

    // ImmediateRsp
    if ( KErrNone != CheckValue( KImmediateRsp, iData.iImmediateRsp, 
        RSat::ENoImmediateRsp ) )
        {
        ret = KErrCorrupt;
        }

    // Text
    if ( KErrNone != CheckText( KText, iData.iText ) )
        {
        ret = KErrCorrupt;
        }

    // Duration
    if ( KErrNone != CheckDuration( iData.iDuration ) )
        {
        ret = KErrCorrupt;
        }
    
    // Icon
    if ( KErrNone != CheckIcon( iData.iIconId ) )
        {
        ret = KErrCorrupt;
        }
    
    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandDisplayText::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandDisplayText::SendResponse()
    {
    iLog.Log( _L( "CCommandDisplayText::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // GeneralResult
    SetGeneralResult( iRsp.iGeneralResult );

    // InfoType & AdditionalInfo
    SetAdditionalInfo ( iRsp.iInfoType, iRsp.iAdditionalInfo );
    
    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }
    
    return ret;
    }

// End of File
