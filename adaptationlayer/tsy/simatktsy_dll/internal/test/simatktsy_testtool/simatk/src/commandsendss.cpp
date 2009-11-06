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
* Description:   SendSS specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsendss.h"
#include "commandsendussd.h"


// -----------------------------------------------------------------------------
// CCommandSendSs::CommandSendSs
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSendSs::CCommandSendSs
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESendSs ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendSs::~CCommandSendSs
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSendSs::~CCommandSendSs()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendSs::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendSs::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifySendSsPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSendSs::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendSs::CheckData()
    {
    iLog.Log( _L( "CCommandSendSs::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // SS string
    // Because TSString- and TAddress-structures are very similar, let's
    // reuse helper method made for TAddress    
    RSat::TAddress ssAddress;
    
    ssAddress.iNumberPlan = iData.iSsString.iNumberPlan;
    ssAddress.iTypeOfNumber = iData.iSsString.iTypeOfNumber;
    ssAddress.iTelNumber.Copy( iData.iSsString.iSsString ); // Can't overflow
    
    if ( KErrNone != CheckAddress( ssAddress ) )
        {
        ret = KErrCorrupt;
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
// CCommandSendSs::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendSs::SendResponse()
    {
    iLog.Log( _L( "CCommandSendSs::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // General results
    SetGeneralResult( iRsp.iGeneralResult );
    SetGeneralResult( iRsp.iGeneralResult2, KSecond );
    
    // Additional infos & types
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    SetAdditionalInfo( iRsp.iInfoType2, iRsp.iAdditionalInfo2, KSecond );
    
    // Get requested action from CallControl via MControllable interface if
    // available
    iRsp.iCcRequestedAction = RequestedAction();
    
    // Check if command was changed into Ussd request
    RSat:: TCallControlType type;
    iRsp.iCcRequestedAction.GetCallControlType( type );
    
    // Apply Ussd if changed into Ussd request
    if ( RSat::ECcUssdString == type 
        && KErrNone != GetUssdString( iRsp.iUssdString ) )
        {
        // Mandatory parameter (UssdString) is missing.
        ret = KErrCorrupt;
        }
    
    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }

    return ret;
    }

// End of File
