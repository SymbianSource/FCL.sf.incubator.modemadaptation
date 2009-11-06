/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Refresh specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <satcs.h>
#include <stifparser.h>
#include "commandrefresh.h"

// CONSTANTS
_LIT( KRefreshType,     "RefreshType" );
_LIT( KFileList,        "FileList" );
_LIT( KAID,             "AID" );


// -----------------------------------------------------------------------------
// CCommandRefresh::CCommandRefresh
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandRefresh::CCommandRefresh
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ERefresh ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandGetInput::~CCommandGetInput
// Destructor
// -----------------------------------------------------------------------------
//
CCommandRefresh::~CCommandRefresh()
    {
    }

 // -----------------------------------------------------------------------------
// CCommandRefresh::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
// 
TInt CCommandRefresh::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );
    
    if ( MatchIpc( ESatNotifyRefreshPCmd ) )
        {
        iSat.NotifyRefreshPCmd( aSimAtkStatus, iDataPckg );
        }
    else
        {
        iSat.NotifyRefreshRequired( aSimAtkStatus, iDataPckg );
        }
    
    
    return ret;  
    }

// -----------------------------------------------------------------------------
// CCommandRefresh::CheckData
// Compare details received from ETel and set in script file
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandRefresh::CheckData()
    {
    iLog.Log( _L( "CCommandRefresh::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // RefreshType
    if ( KErrNone != CheckValue( KRefreshType, iData.iType ) )
        {
        ret = KErrCorrupt;
        }
    
    // FileList (optional)
    TInt number( 0 );
    if ( KErrNone != CheckBuffer( KFileList, iData.iFileList ) )
        {
        ret = KErrCorrupt;
        }

    // AID (optional)
    if ( KErrNone != CheckBuffer( KAID, iData.iAid ) )
        {
        ret = KErrCorrupt;
        }
        
    if ( KErrNone == GetValue( KAID, number ) )
        {
        RSat::TAid aidBuf;
        aidBuf.Append( number );
        
        while ( KErrNone == GetNextValue( number ) )
            {
            aidBuf.Append( number );
            }
        if ( aidBuf != iData.iAid )
            {
            iLog.Log( _L(
	            "CCommandRefreshRequired::CheckProactiveCommand - AID did not match") ); 
            ret = KErrCorrupt;
            }
        }
    else if ( iData.iAid.Length() )
        {
        // Parameter was not given from script. 
        // Verify that etel package is empty
        ret = KErrCorrupt;
        }

    return ret;
    }
 
 // -----------------------------------------------------------------------------
// CCommandRefresh::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//   
TInt  CCommandRefresh::SendResponse()
    {
    iLog.Log( _L( "CCommandRefresh::SendResponse" ) );    
    
    TInt ret( KErrNone );
    
    // GeneralResult
    SetGeneralResult( iRsp.iGeneralResult );

    if (  MatchIpc( ESatNotifyRefreshPCmd )  )
        {
        // InfoType & AdditionalInfo
        SetAdditionalInfo ( iRsp.iInfoType, iRsp.iAdditionalInfo );
        
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }
    else
        {
        // This case differs from normal commands.
        // We call RefreshAllowed() method instead of TerminalRsp()
        TRequestStatus req;
        iSat.RefreshAllowed( req, iRspPckg );
        User::WaitForRequest( req );
        ret = req.Int();
        }
   
    return ret;
    }

// End of File
