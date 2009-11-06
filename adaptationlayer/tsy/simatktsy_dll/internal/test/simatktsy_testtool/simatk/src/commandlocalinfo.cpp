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
#include "commandlocalinfo.h"

// CONSTANTS
_LIT( KType,        "Type" );


// -----------------------------------------------------------------------------
// CCommandLocalInfo::CCommandLocalInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandLocalInfo::CCommandLocalInfo
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ELocalInfo ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandLocalInfo::~CCommandLocalInfo
// Destructor
// -----------------------------------------------------------------------------
//
CCommandLocalInfo::~CCommandLocalInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCommandLocalInfo::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLocalInfo::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifyLocalInfoPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandLocalInfo::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLocalInfo::CheckData()
    {
    iLog.Log( _L( "CCommandLocalInfo::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // TODO
    // DevideId    
    
    // InfoType
    if ( KErrNone != CheckValue( KType, iData.iInfoType ) )
        {
        ret = KErrCorrupt;
        }
    
    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandLocalInfo::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLocalInfo::SendResponse()
    {
    iLog.Log( _L( "CCommandLocalInfo::SendResponse" ) );    
    
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
