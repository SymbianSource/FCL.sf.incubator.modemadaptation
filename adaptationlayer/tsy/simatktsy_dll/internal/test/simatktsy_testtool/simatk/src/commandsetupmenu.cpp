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
* Description:   SetUp Menu specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsetupmenu.h"

// CONSTANTS
_LIT( KItemId,          "ItemId");
_LIT( KHelp,            "Help" );
_LIT( KHelpRequest,     "HelpRequest");


// -----------------------------------------------------------------------------
// CCommandSetUpMenu::CCommandSetUpMenu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSetUpMenu::CCommandSetUpMenu
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatMenuBase ( aIpc, aSat, aParser, aLog, RSat::ESetUpMenu ),
        iDataPckg( iData ),
        iRspPckg( iRsp ),
        iSelectionPckg ( iSelection )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpMenu::~CCommandSetUpMenu
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSetUpMenu::~CCommandSetUpMenu()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpMenu::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpMenu::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifySetUpMenuPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSetUpMenu::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpMenu::CheckData()
    {
    iLog.Log( _L( "CCommandSetUpMenu::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // First check the validity of common menu structure
    ret = CheckMenuStructure( iData );
    
    if ( iData.IsRemoveMenu() == EFalse )
        {
        // Display menu received
        
        // IconListQualifier
        if ( KErrNone != CheckValue( KIconListQualifier,
            iData.iIconListQualifier, RSat::ENoIconId ) )
            {
            ret = KErrCorrupt;
            }
                        
        // Preference
        if ( KErrNone != CheckValue( KPreference, iData.iPreference,
            RSat::ENoSelectionPreference ) )
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

        // Help
        if ( KErrNone != CheckValue( KHelp, iData.iHelp, 
            RSat::ENoHelpAvailable ) )
            {
            ret = KErrCorrupt;
            }    

        }
        
    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandSetUpMenu::SendResponse
// Creates response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpMenu::SendResponse()
    {
    iLog.Log( _L( "CCommandSetUpMenu::SendResponse" ) );    
    
    TInt ret( KErrNone );

    if ( IsEnvelope() )
        {
        if ( KErrNone != GetValue( KItemId, iSelection.iItemId ) )
            {
            // Mandatory parameter not provided by the script
            ret = KErrArgument;
            }
        else
            {
            // Id found, default is HelpNotRequested
            iSelection.iHelp = RSat::EHelpNotRequested;
            GetValue( KHelpRequest, iSelection.iHelp );
            
            // Notify Etel about MenuSelection
            TRequestStatus sendStatus;
            iSat.MenuSelection( sendStatus, iSelectionPckg );
            User::WaitForRequest( sendStatus );
            
            ret = sendStatus.Int();
            }
        }
    else // Terminal response
        {
        // General result
        SetGeneralResult( iRsp.iGeneralResult );
        
        // Additional info & type
        SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
        
        if ( KErrNone == ret )
            {
            ret = CSatCommandBase::SendResponse( iRspPckg );
            }
        }    

    return ret;
    }
    
// End of File
