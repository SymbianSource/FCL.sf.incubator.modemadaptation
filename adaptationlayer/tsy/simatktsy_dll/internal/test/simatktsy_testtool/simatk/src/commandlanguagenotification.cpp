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
* Description:   Language Notification specific test class. Inherits 
*                CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandlanguagenotification.h"


// CONSTANTS
_LIT( KType,                    "Type" );
_LIT( KLanguage,                "Language" );


// -----------------------------------------------------------------------------
// CCommandLanguageNotification::CCommandLanguageNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandLanguageNotification::CCommandLanguageNotification
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ELanguageNotification ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandLanguageNotification::~CCommandLanguageNotification
// Destructor
// -----------------------------------------------------------------------------
//
CCommandLanguageNotification::~CCommandLanguageNotification()
    {
    }

// -----------------------------------------------------------------------------
// CCommandLanguageNotification::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLanguageNotification::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifyLanguageNotificationPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandLanguageNotification::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLanguageNotification::CheckData()
    {
    iLog.Log( _L( "CCommandLanguageNotification::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // NotificationType    
    if ( KErrNone != CheckValue( KType, iData.iNotificationType,
        RSat::ENonSpecificLangNotification ) )
        {
        ret = KErrCorrupt;
        }
        
    // Language    
    if ( RSat::ESpecificLangNotification == iData.iNotificationType )
        {
        if ( KErrNone != CheckValue( KLanguage, iData.iLanguage ) )
            {
            ret = KErrCorrupt;
            }
        }
    else if ( iData.iLanguage )
        {
        iLog.Log( _L( "CCommandLanguageNotification::CheckData: ERROR \
            Language provided but Type-parameter is not set!" ) );
        ret = KErrCorrupt;
        }

    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandLanguageNotification::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandLanguageNotification::SendResponse()
    {
    iLog.Log( _L( "CCommandLanguageNotification::SendResponse" ) );    
    
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
