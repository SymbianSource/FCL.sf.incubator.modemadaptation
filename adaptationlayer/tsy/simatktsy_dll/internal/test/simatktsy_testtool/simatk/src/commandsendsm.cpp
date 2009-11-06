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
* Description:   SendSm specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsendsm.h"

// CONSTANTS
_LIT( KSmsTpdu,         "SmsTpdu" );
_LIT( KSmsResult,       "SmsResult" );


// -----------------------------------------------------------------------------
// CCommandSendSm::CommandSendSm
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSendSm::CCommandSendSm
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESendSm ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendSm::~CCommandSendSm
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSendSm::~CCommandSendSm()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSendSm::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendSm::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifySendSmPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSendSm::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendSm::CheckData()
    {
    iLog.Log( _L( "CCommandSendSm::CheckData" ) );
        
    TInt ret( KErrNone );
        
    switch( CheckBuffer( KSmsTpdu, iData.iSmsTpdu ) )
        {
        case KErrCorrupt:
            {
            iLog.Log( _L( "CCommandSendSm::CheckData: ERROR \
                Parameter SmsTpdu did not match!" ) );
            ret = KErrCorrupt;
            break;
            }
        case KErrArgument:
            {
            iLog.Log( _L( "CCommandSendSm::CheckData: ERROR \
                Mandatory parameter SmsTpdu missing" ) );
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
        
    // Smsc address
    if ( 0 < iData.iAddress.iTelNumber.Length() )
        {
        // Smsc provided by proactive command. Compare against scripted
        // values
        switch ( CheckAddress( iData.iAddress ) )
            {
            case KErrNotFound:
                {
                ret = KErrNotFound;
                break;
                }
            case KErrCorrupt:
                {
                ret = KErrCorrupt;
                break;
                }
            case KErrNone:
            default:
                {
                // None
                break;
                }
            }
        }
    else
        {
        // Proactive command did not provide SMSC address. Scripted SMSC
        // is used as a default SMSC.
        if ( KErrNone != GetAddress( iData.iAddress ) )
            {
            iLog.Log( _L( "CCommandSendSm::CheckData: ERROR \
                Mandatory parameter default SMSC missing!" ) );
            ret = KErrArgument;
            }
        }

    // SendSmNoLogging
    if ( KErrNone == ret )
        {
        iLog.Log( _L("CCommandSendSm::CheckData, starting to send an SMS" ) );
        RSat::TSatSmsV1 satSm;
        RSat::TSatSmsV1Pckg satSmPckg( satSm );
        
        satSm.iBuf = iData.iSmsTpdu;
        satSm.iServiceCenter = iData.iAddress;
        TRequestStatus satSmStatus;
        TUint16 satSmRef( 0x00 ); // Not used anywhere

        iSat.SendMessageNoLogging( satSmStatus, satSmPckg, satSmRef );
        
        User::WaitForRequest( satSmStatus );
        
        ret = satSmStatus.Int();
        
        TInt expectedResult( KErrNone ); //Default if SmsResult is not given
        
        if ( KErrNone == GetValue( KSmsResult, expectedResult ) )
            {
            // Compare scripted and actual result
            ret == expectedResult ? ret = KErrNone : ret = ret ;
            }
        if ( KErrNone != ret )
            {
            iLog.Log( _L("CCommandSendSm::CheckData: SMS sending failed \
            with error code: %d"), ret ) ;
            }
        else
            {
            iLog.Log(_L("CCommandSendSm::CheckData: SMS sending succseed \
                Expected vs. received result: OK ") );     
            }
        }

    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandSendSm::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSendSm::SendResponse()
    {
    iLog.Log( _L( "CCommandSendSm::SendResponse" ) );    
    
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
