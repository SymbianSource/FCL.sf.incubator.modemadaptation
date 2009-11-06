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
* Description:   SatMoSmsControl Control specific test class. Inherits CSatBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "satmosmscontrol.h"

// CONSTANTS
_LIT( KControlResult, "ControlResult" );


// -----------------------------------------------------------------------------
// CSatMoSmsControl::CSatMoSmsControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatMoSmsControl::CSatMoSmsControl
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatBase ( aIpc, aSat, aParser, aLog ), iDataPckg( iData )
    {
    }

// -----------------------------------------------------------------------------
// CSatMoSmsControl::~CSatMoSmsControl
// Destructor
// -----------------------------------------------------------------------------
//
CSatMoSmsControl::~CSatMoSmsControl()
    {
    }

// -----------------------------------------------------------------------------
// CSatMoSmsControl::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatMoSmsControl::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );

    iSat.NotifyMoSmControlRequest( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatMoSmsControl::CheckData
// Verifies the validity of received SatMoSmsControl control event
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatMoSmsControl::CheckData()
    {
    iLog.Log( _L( "CSatMoSmsControl::CheckData" ) );

    TInt ret( KErrNone );

    // GeneralResult
    if ( KErrNone != CheckValue( KControlResult, iData.iResult ) )
        {
        ret = KErrCorrupt;
        }

    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
        {
        ret = KErrCorrupt;
        }

    // not supported fields:
    // iRpAddress
    // iTpAddress

    return ret;
    }

// End of file
