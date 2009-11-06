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
* Description:   ReceiveData specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandreceivedata.h"

// CONSTANTS
_LIT( KDestination,         "Destination" );
_LIT( KChannelDataLength,   "ChannelDataLength" );


// -----------------------------------------------------------------------------
// CCommandReceiveData::CCommandReceiveData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandReceiveData::CCommandReceiveData
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::EReceiveData ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandReceiveData::~CCommandReceiveData
// Destructor
// -----------------------------------------------------------------------------
//
CCommandReceiveData::~CCommandReceiveData()
    {
    }

// -----------------------------------------------------------------------------
// CCommandReceiveData::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandReceiveData::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifyReceiveDataPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandReceiveData::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandReceiveData::CheckData()
    {
    iLog.Log( _L( "CCommandReceiveData::CheckData" ) );

    TInt ret( KErrNone );
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
        {
        ret = KErrCorrupt;
        }
        
    // ChannelDataLength
    if ( KErrNone != CheckValue( KChannelDataLength, iData.iChannelDataLength ) )
        {
        ret = KErrCorrupt;
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

#ifndef LEGACY
    // TextAttribute
    if ( KErrNone != CheckTextAttribute( iData.iTextAttribute ) )
        {
        ret = KErrCorrupt;
        }
#endif // LEGACY
    return ret;
    }
 

// -----------------------------------------------------------------------------
// CCommandReceiveData::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandReceiveData::SendResponse()
    {
    iLog.Log( _L( "CCommandReceiveData::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // General result
    SetGeneralResult( iRsp.iGeneralResult );
    
    // Additional info & type
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    
    // ChannelDataLength
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
