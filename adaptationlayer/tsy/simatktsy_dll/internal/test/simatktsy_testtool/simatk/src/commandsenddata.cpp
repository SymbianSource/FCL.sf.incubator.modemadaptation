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
* Description:   SendData specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsenddata.h"

// CONSTANTS
_LIT ( KData,               "ChannelData" );
_LIT ( KDestination,        "Destination" );
_LIT ( KMode,               "Mode" );
_LIT ( KChannelDataLength,  "ChannelDataLength" );


// -----------------------------------------------------------------------------
// CCommandSendData::CCommandSendData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSendData::CCommandSendData
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESendData ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendData::~CCommandSendData
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSendData::~CCommandSendData()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendData::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendData::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifySendDataPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSendData::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendData::CheckData()
    {
    iLog.Log( _L( "CCommandSendData::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
        {
        ret = KErrCorrupt;
        }    

    // ChannelData
    switch( CheckBuffer( KData, iData.iChannelData ) )
        {
        case KErrCorrupt:
            {
            iLog.Log( _L( "CCommandSendData::CheckData: ERROR \
                Parameter ChannelData did not match!" ) );
            ret = KErrCorrupt;
            break;
            }
        case KErrArgument:
            {
            iLog.Log( _L( "CCommandSendData::CheckData: ERROR \
                Mandatory parameter ChannelData missing" ) );
            ret = KErrArgument;
            break;
            }
        case KErrNone:
        default:
            {
            // None
            break;
            }
        }
        
    // Destination
    if ( KErrNone != CheckValue( KDestination, iData.iDestination,
        RSat::KChannel1 ) )
        {
        ret = KErrCorrupt;
        }

    // IconId
    if ( KErrCorrupt == CheckIcon( iData.iIconId ) )
        {
        ret = KErrCorrupt;
        }

    // Mode
    if ( KErrNone != CheckValue( KMode, iData.iMode,
        RSat::ESendDataImmediately ) )
        {
        ret = KErrCorrupt;
        }

#ifndef LEGACY
    // TextAttribute
    if ( KErrNone != CheckTextAttribute( iData.iTextAttribute ) )
        {
        ret = KErrCorrupt;
        }
# endif // LEGACY
    return ret;
    }
 

// -----------------------------------------------------------------------------
// CCommandSendData::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendData::SendResponse()
    {
    iLog.Log( _L( "CCommandSendData::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // General result
    SetGeneralResult( iRsp.iGeneralResult );
    
    // Additional info & type
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    
    // ChannelDataLength, default is 0xFF
    iRsp.iChannelDataLength = KMaxTUint8;
    GetValue( KChannelDataLength, iRsp.iChannelDataLength );
    
    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }

    return ret;
    }


// End of File
