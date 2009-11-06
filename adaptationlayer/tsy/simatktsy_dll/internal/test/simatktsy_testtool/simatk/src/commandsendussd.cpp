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
* Description:   SendUssd specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsendussd.h"


// -----------------------------------------------------------------------------
// CCommandSendUssd::CommandSendUssd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSendUssd::CCommandSendUssd
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESendUssd ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendUssd::~CCommandSendUssd
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSendUssd::~CCommandSendUssd()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendUssd::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendUssd::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifySendUssdPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSendUssd::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendUssd::CheckData()
    {
    iLog.Log( _L( "CCommandSendUssd::CheckData" ) );    
    
    TInt ret( KErrNone );
        
    // UssdString
    RSat::TUssdString scriptedUssd;
    if ( KErrNone == GetUssdString( scriptedUssd ) )
        {
        // String
        if ( scriptedUssd.iUssdString != iData.iUssdString.iUssdString )
            {
            iLog.Log( _L( "CCommandSendUssd::CheckData: ERROR \
                UssdString mismatch!" ) );
            ret = KErrCorrupt;
            }
        
        // DCS
        if ( scriptedUssd.iDcs != iData.iUssdString.iDcs )
            {
            iLog.Log( _L( "CCommandSendUssd::CheckData: ERROR \
                UssdString DCS mismatch!" ) );
            ret = KErrCorrupt;
            }
        }
    else
        {
        iLog.Log( _L( "CCommandSendUssd::CheckData: ERROR \
            Mandatory parameter UssdString missing" ) );
        ret = KErrArgument;
        }
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
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
// CCommandSendUssd::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendUssd::SendResponse()
    {
    iLog.Log( _L( "CCommandSendUssd::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // General results
    TBool isSuccess( SetGeneralResult( iRsp.iGeneralResult ) );
    SetGeneralResult( iRsp.iGeneralResult2, KSecond );
    
    // Additional infos & types
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    SetAdditionalInfo( iRsp.iInfoType2, iRsp.iAdditionalInfo2, KSecond );
    
    // Get requested action from CallControl via MControllable interface if
    // available
    iRsp.iCcRequestedAction = RequestedAction();
    
    // USSD String required if succees and not modified by call control
    if ( isSuccess && iRsp.iGeneralResult != RSat::KModifiedByCallControl )
        {
        ret = GetUssdString( iRsp.iUssdString );
        }
    
    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }

    return ret;
    }

// End of File
