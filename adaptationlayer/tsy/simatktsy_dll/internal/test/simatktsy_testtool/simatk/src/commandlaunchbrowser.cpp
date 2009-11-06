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
* Description:   LaunchBrowser specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandlaunchbrowser.h"

// CONSTANTS
_LIT( KBearerList,          "BearerList" );
_LIT( KBrowserId,           "BrowserId" );
_LIT( KBrowserSel,          "BrowserSel" );
_LIT( KText,                "Text" );
_LIT( KUrl,                 "Url" );
_LIT( KFileRef,             "FileRef" );


// -----------------------------------------------------------------------------
// CCommandLaunchBrowser::CCommandLaunchBrowser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandLaunchBrowser::CCommandLaunchBrowser
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ELaunchBrowser ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandLaunchBrowser::~CCommandLaunchBrowser
// Destructor
// -----------------------------------------------------------------------------
//
CCommandLaunchBrowser::~CCommandLaunchBrowser()
    {
    }

// -----------------------------------------------------------------------------
// CCommandLaunchBrowser::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLaunchBrowser::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifyLaunchBrowserPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandLaunchBrowser::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLaunchBrowser::CheckData()
    {
    iLog.Log( _L( "CCommandLaunchBrowser::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
        {
        ret = KErrCorrupt;
        }    

    // BearerList
    if ( KErrNone != CheckBuffer(  KBearerList, iData.iBearerList ) )
        {        
        iLog.Log( _L( "CCommandLaunchBrowser::CheckData: ERROR \
            Bearer list did not match!" ) );
        ret = KErrCorrupt;
        }

    // BrowserId
    if ( KErrNone != CheckValue( KBrowserId, iData.iBrowserId,
        RSat::EBrowserIdNotPresent ) )
        {
        ret = KErrCorrupt;
        }

    // BrowserSel
    if ( KErrNone != CheckValue( KBrowserSel, iData.iBrowserSel,
        RSat::ELaunchBrowserIfNotAlreadyLaunched ) )
        {
        ret = KErrCorrupt;
        }

    // IconId
    if ( KErrCorrupt == CheckIcon( iData.iIconId ) )
        {
        ret = KErrCorrupt;
        }

    // Text
    if ( KErrNone != CheckText( KText, iData.iText ) )
        {
        ret = KErrCorrupt;
        }
        
    // Url
    if ( KErrNone != CheckText( KUrl, iData.iUrl ) )
        {
        ret = KErrCorrupt;
        }
        
    // FileRef
    TUint number( iData.NumberOfFileRef() );
    
    if ( number )
        {
        // Read file refs
        RSat::TProvisioningFileRef files;
        RSat::TProvisioningFileRef aFile;
        for ( TInt i( 0 ) ; i < number ; i++ )
            {
            iData.GetFileRef( i + 1, aFile );
            files.Append( aFile );
            }
        
        if ( KErrNone != CheckBuffer( KFileRef, files ) )
            {
            iLog.Log( _L( "CCommandLaunchBrowser::CheckData \
                - FileRef mismatch!" ) );
            ret = KErrCorrupt;
            }
        }
    else if ( KErrNone == GetValue( KFileRef, number ) )
        {
        iLog.Log( _L( "CCommandLaunchBrowser::CheckData \
            - FileRef not provided by the TSY!" ) );
        ret = KErrCorrupt;
        }
        
    // Loading of Provisioning files is not supported

    return ret;
    }


// -----------------------------------------------------------------------------
// CCommandLaunchBrowser::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLaunchBrowser::SendResponse()
    {
    iLog.Log( _L( "CCommandLaunchBrowser::SendResponse" ) );    
    
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
