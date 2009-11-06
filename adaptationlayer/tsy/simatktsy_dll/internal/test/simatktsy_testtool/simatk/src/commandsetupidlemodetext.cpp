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
* Description:   SetUpIdleModeText specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsetupidlemodetext.h"

// CONSTANTS
_LIT( KType,                "Type" );
_LIT( KText,                "Text" );

#ifndef LEGACY
_LIT( KCodingScheme,        "CodingScheme" );
#endif // LEGACY

// -----------------------------------------------------------------------------
// CCommandSetUpIdleModeText::CCommandSetUpIdleModeText
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSetUpIdleModeText::CCommandSetUpIdleModeText
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESetUpIdleModeText ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpIdleModeText::~CCommandSetUpIdleModeText
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSetUpIdleModeText::~CCommandSetUpIdleModeText()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpIdleModeText::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpIdleModeText::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifySetUpIdleModeTextPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSetUpIdleModeText::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpIdleModeText::CheckData()
    {
    iLog.Log( _L( "CCommandSetUpIdleModeText::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // Type, default "UpdateIdleModeText"
    if ( KErrNone != CheckValue( KType, iData.iType,
        RSat::EUpdateIdleModeText ) )
        {
        ret = KErrCorrupt;
        }
    
    // Text
    if ( KErrNone != CheckText( KText, iData.iText ) )
        {
        ret = KErrCorrupt;
        }
  
    // IconId
    if ( KErrCorrupt == CheckIcon( iData.iIconId ) )
        {
        ret = KErrCorrupt;
        }
        
#ifndef LEGACY
    // CodingScheme, default "16bit UCS2"
    if ( KErrNone != CheckValue( KCodingScheme, iData.iCodingScheme,
        RSat::E16bitUCS2 ) )
        {
        ret = KErrCorrupt;
        }
# endif // LEGACY
    
    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandSetUpIdleModeText::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpIdleModeText::SendResponse()
    {
    iLog.Log( _L( "CCommandSetUpIdleModeText::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // General result
    SetGeneralResult( iRsp.iGeneralResult );
    
    // Additional info & type
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    
    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }

    return ret;
    }

// End of File
