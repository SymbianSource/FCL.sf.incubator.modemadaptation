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
* Description:   GetInput specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandgetinput.h"

// CONSTANTS
_LIT( KText,                "Text" );
_LIT( KRspFormat,           "RspFormat" );
_LIT( KDefaultText,         "DefaultText" );
_LIT( KInputDisplayOption,  "InputDisplayOption" );
_LIT( KRspLength,           "RspLength" );
_LIT( KHelp,                "Help" );


// -----------------------------------------------------------------------------
// CCommandGetInput::CCommandGetInput
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandGetInput::CCommandGetInput
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::EGetInput ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandGetInput::~CCommandGetInput
// Destructor
// -----------------------------------------------------------------------------
//
CCommandGetInput::~CCommandGetInput()
    {
    }

// -----------------------------------------------------------------------------
// CCommandGetInput::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandGetInput::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );
    
    iSat.NotifyGetInputPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandGetInput::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandGetInput::CheckData()
    {
    iLog.Log( _L( "CCommandGetInput::CheckData" ) );
        
    TInt ret( KErrNone );
    
    // InputDisplayOption
    if ( KErrNone != CheckValue( KInputDisplayOption,
        iData.iInputDisplayOption, RSat::EDisplayUserInput ) )
        {
        ret = KErrCorrupt;
        }

    // RspFormat
    if ( KErrNone != CheckValue( KRspFormat, iData.iRspFormat,
        RSat::ESmsDefaultAlphabetUnpacked ) )
        {
        ret = KErrCorrupt;
        }

    // RspLength
    if ( KErrNone != CheckValue( KRspLength, iData.iRspLength.iMinRspLength ) )
        {
        ret = KErrCorrupt;
        }
    else if ( KErrNone != CheckNextValue( iData.iRspLength.iMaxRspLength ) )
        {
        ret = KErrCorrupt;
        }

    // DefaultText
    if ( KErrNone != CheckText( KDefaultText, iData.iDefaultText ) )
        {
        ret = KErrCorrupt;
        }    
    
    // Text
    if ( KErrNone != CheckText( KText, iData.iText ) )
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
// CCommandGetInput::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandGetInput::SendResponse()
    {
    iLog.Log( _L( "CCommandGetInput::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // General result
    SetGeneralResult( iRsp.iGeneralResult );
    
    // Additional info & type
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    
    // ResponseFormat
    if ( KErrNone != GetValue( KRspFormat, iRsp.iRspFormat ) 
        && RSat::KTextString == iRsp.iInfoType )
        {
        // Set to default, same as in Pcmd
        iRsp.iRspFormat = iData.iRspFormat;
        }

    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }       

    return ret;
    }

// End of File
