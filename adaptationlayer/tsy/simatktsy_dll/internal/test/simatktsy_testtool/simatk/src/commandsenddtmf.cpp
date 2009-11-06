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
* Description:   SendDtmf specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsenddtmf.h"

// CONSTANTS
_LIT( KDtmf,            "Dtmf" );


// -----------------------------------------------------------------------------
// CCommandSendDtmf::CCommandSendDtmf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSendDtmf::CCommandSendDtmf
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESendDtmf ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendDtmf::~CCommandSendDtmf
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSendDtmf::~CCommandSendDtmf()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendDtmf::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendDtmf::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifySendDtmfPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSendDtmf::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendDtmf::CheckData()
    {
    iLog.Log( _L( "CCommandSendDtmf::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
        {
        ret = KErrCorrupt;
        }
        
    // DTMF
    if ( KErrNone != CheckBuffer( KDtmf, iData.iDtmfString ) )
        {
        iLog.Log( _L( "CCommandSendDtmf::CheckData: ERROR \
            - Problem found when validationg DTMF string!" ) );
        ret = KErrCorrupt;
        }

    // IconId
    if ( KErrCorrupt == CheckIcon( iData.iIconId ) )
        {
        ret = KErrCorrupt;
        }

#ifndef LEGACY
    // TextAttribute
    if ( KErrNone != CheckTextAttribute( iData.iTextAttribute ) )
        {
        ret = KErrCorrupt;
        }
# endif // LEGACY
    return ret;
    }
 

// -----------------------------------------------------------------------------
// CCommandSendDtmf::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendDtmf::SendResponse()
    {
    iLog.Log( _L( "CCommandSendDtmf::SendResponse" ) );    
    
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
