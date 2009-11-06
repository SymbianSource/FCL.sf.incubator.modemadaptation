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
* Description:   SetUpCall specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsetupcall.h"
#include "commandsendussd.h"


// CONSTANTS
_LIT( KAlphaIdCallSetUpPhase,       "AlphaIdCallSetUpPhase" );
_LIT( KCapabilityConfigParams,      "CapabilityConfigParams" );
_LIT( KIconIdCallSetUpPhase,        "IconIdCallSetUpPhase" );
_LIT( KSubAddress,                  "SubAddress" );
_LIT( KType,                        "Type" );


// -----------------------------------------------------------------------------
// CCommandSetUpCall::CCommandSetUpCall
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSetUpCall::CCommandSetUpCall
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESetUpCall ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpCall::~CCommandSetUpCall
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSetUpCall::~CCommandSetUpCall()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpCall::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpCall::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifySetUpCallPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSetUpCall::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpCall::CheckData()
    {
    iLog.Log( _L( "CCommandSetUpCall::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // Address
    if ( KErrNone != CheckAddress( iData.iAddress ) )
        {
        ret = KErrCorrupt;
        }
    
    // AlphaIdConfirmationPhase
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaIdConfirmationPhase ) )
        {
        ret = KErrCorrupt;
        }

    // AlphaIdCallSetUpPhase
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaIdCallSetUpPhase,
        KAlphaIdCallSetUpPhase ) )
        {
        ret = KErrCorrupt;
        }
        
    // CapabilityConfigParams
    if ( KErrCorrupt == CheckBuffer( KCapabilityConfigParams, 
        iData.iCapabilityConfigParams ) )
        {
        iLog.Log( _L( "CCommandSetUpCall::CheckData: ERROR \
            CapabilityConfigParams mismatch!" ) );        
        ret = KErrCorrupt;
        }
    
    // Duration
    if ( KErrCorrupt == CheckDuration( iData.iDuration ) )
        {
        ret = KErrCorrupt;
        }
        
    // IconId
    if ( KErrCorrupt == CheckIcon( iData.iIconIdConfirmationPhase ) )
        {
        ret = KErrCorrupt;
        }
        
    // IconIdCallSetUpPhase
    if ( KErrCorrupt == CheckIcon( iData.iIconIdCallSetUpPhase,
        KIconIdCallSetUpPhase ) )
        {
        ret = KErrCorrupt;
        }
        
    // SubAddress
    RSat::TSubAddress subAddress;
    GetText( KSubAddress, subAddress );
    if ( KErrNotFound == iData.iSubAddress.Match( subAddress ) )
        {
        iLog.Log( _L( "CCommandSetUpCall::CheckData: ERROR \
            SubAddress mismatch!" ) );
        ret = KErrCorrupt;
        }
        
    // Type, Default "OnlyIfNotBusy"
    if ( KErrNone != CheckValue( KType, iData.iType, RSat::EOnlyIfNotBusy ) )
        {
        ret = KErrCorrupt;
        }
    
    
    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandSetUpCall::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpCall::SendResponse()
    {
    iLog.Log( _L( "CCommandSetUpCall::SendResponse" ) );    
    
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
