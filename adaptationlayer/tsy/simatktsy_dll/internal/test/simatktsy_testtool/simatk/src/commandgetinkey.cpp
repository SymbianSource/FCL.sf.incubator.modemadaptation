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
* Description:   GetInkey specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandgetinkey.h"

// CONSTANTS
_LIT( KMode,            "Mode" );
_LIT( KText,            "Text" );
_LIT( KRspFormat,       "RspFormat" );
_LIT( KHelp,            "Help" );



// -----------------------------------------------------------------------------
// CCommandGetInkey::CCommandGetInkey
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandGetInkey::CCommandGetInkey
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::EGetInkey ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandGetInkey::~CCommandGetInkey
// Destructor
// -----------------------------------------------------------------------------
//
CCommandGetInkey::~CCommandGetInkey()
    {
    }

// -----------------------------------------------------------------------------
// CCommandGetInkey::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandGetInkey::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifyGetInkeyPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandGetInkey::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandGetInkey::CheckData()
    {
    iLog.Log( _L( "CCommandGetInkey::CheckData" ) );
    
    TInt ret( KErrNone );
    
    // Mode
    if ( KErrNone != CheckValue( KMode, iData.iMode ) )
        {
        ret = KErrCorrupt;
        }

    // Format
    if ( KErrNone != CheckValue( KRspFormat, iData.iRspFormat ) )
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

    // Help
    if ( KErrNone != CheckValue( KHelp, iData.iHelp, RSat::ENoHelpAvailable ) )
        {
        ret = KErrCorrupt;
        }    

    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandGetInkey::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandGetInkey::SendResponse()
    {
    iLog.Log( _L( "CCommandGetInkey::SendResponse" ) );    
    
    TInt ret( KErrNone );
    
    // General result
    SetGeneralResult( iRsp.iGeneralResult );
    
    // Additional info & type
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    
    // Duration. Ignore return value; duration field is optional
    SetDuration( iRsp.iDuration );

    // Response format. Defaults to original.
    iRsp.iRspFormat = iData.iRspFormat;
    GetValue( KRspFormat, iRsp.iRspFormat );

    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }

    return ret;
    }

// End of File
