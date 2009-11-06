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
* Description:   CloseChannel specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandclosechannel.h"

// CONSTANTS
_LIT( KDestination,         "Destination" );


// -----------------------------------------------------------------------------
// CCommandCloseChannel::CCommandCloseChannel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandCloseChannel::CCommandCloseChannel
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ECloseChannel ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandCloseChannel::~CCommandCloseChannel
// Destructor
// -----------------------------------------------------------------------------
//
CCommandCloseChannel::~CCommandCloseChannel()
    {
    }

// -----------------------------------------------------------------------------
// CCommandCloseChannel::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandCloseChannel::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifyCloseChannelPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandCloseChannel::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandCloseChannel::CheckData()
    {
    iLog.Log( _L( "CCommandCloseChannel::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
        {
        ret = KErrCorrupt;
        }    
        
    // Destination
    if ( KErrNone != CheckValue( KDestination, iData.iDestination,
        RSat::KChannel1 ) )
        {
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
#endif // LEGACY
    return ret;
    }
 

// -----------------------------------------------------------------------------
// CCommandCloseChannel::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandCloseChannel::SendResponse()
    {
    iLog.Log( _L( "CCommandCloseChannel::SendResponse" ) );
    
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
