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
* Description:   Select Item specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandselectitem.h"

// CONSTANTS
_LIT( KDefaultItemId,       "DefaultItemId");
_LIT( KPresentationType,    "PresentationType");
_LIT( KHelp,                "Help" );


// -----------------------------------------------------------------------------
// CCommandSelectItem::CCommandSelectItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSelectItem::CCommandSelectItem
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatMenuBase ( aIpc, aSat, aParser, aLog, RSat::ESelectItem ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSelectItem::~CCommandSelectItem
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSelectItem::~CCommandSelectItem()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSelectItem::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSelectItem::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifySelectItemPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSelectItem::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSelectItem::CheckData()
    {
    iLog.Log( _L( "CCommandSelectItem::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // First check the validity of common menu structure
    ret = CheckMenuStructure( iData );

    if ( iData.IsRemoveMenu() == EFalse )
        {
        // Display menu received
        
        // DefaultItemId, 0 (none) as default
        if ( KErrNone != CheckValue( KDefaultItemId, 
            iData.iDefaultItemId, 0 ) )
            {
            ret = KErrCorrupt;
            }
        
        // PresentationType
        if ( KErrNone != CheckValue( KPresentationType, 
            iData.iPresentationType, RSat::ENotSpecified ) )
            {
            ret = KErrCorrupt;
            }

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
// CCommandSelectItem::SendResponse
// Creates response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSelectItem::SendResponse()
    {
    iLog.Log( _L( "CCommandSelectItem::SendResponse" ) );    
    
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
