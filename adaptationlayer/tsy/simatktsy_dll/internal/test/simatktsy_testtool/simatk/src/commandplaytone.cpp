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
* Description:   PlayTone specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandplaytone.h"

// CONSTANTS
_LIT( KTone,        "Tone" );


// -----------------------------------------------------------------------------
// CCommandPlayTone::CCommandPlayTone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandPlayTone::CCommandPlayTone
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::EPlayTone ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandGetInput::~CCommandGetInput
// Destructor
// -----------------------------------------------------------------------------
//
CCommandPlayTone::~CCommandPlayTone()
    {
    }

 // -----------------------------------------------------------------------------
// CCommandPlayTone::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
// 
TInt CCommandPlayTone::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );
    
    iSat.NotifyPlayTonePCmd( aSimAtkStatus, iDataPckg );
    
    return ret;  
    }

// -----------------------------------------------------------------------------
// CCommandPlayTone::CheckData
// Compare details received from ETel and set in script file
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandPlayTone::CheckData()
    {
    iLog.Log( _L( "CCommandPlayTone::CheckData" ) );    
    
    TInt ret( KErrNone );

    // Tone
    if ( KErrNone != CheckValue( KTone, iData.iTone ) )
        {
        ret = KErrCorrupt;
        }    
    
    // AlphaId
    if ( KErrNone != CheckAlphaId( iData.iAlphaId ) )
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
// CCommandPlayTone::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//   
TInt  CCommandPlayTone::SendResponse()
    {
    iLog.Log( _L( "CCommandPlayTone::SendResponse" ) );    
    
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
