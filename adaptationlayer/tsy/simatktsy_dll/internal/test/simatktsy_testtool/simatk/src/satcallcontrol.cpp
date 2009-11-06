/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Call Control specific test class. Inherits CSatBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "satcallcontrol.h"

// CONSTANTS
_LIT( KControlResult,       "ControlResult" );
_LIT( KAlphaIdValidity,     "AlphaIdValidity" );
_LIT( KType,                "Type" );
_LIT( KActionOriginator,    "ActionOriginator" );
_LIT( KBCRepeatIndicator,   "BCRepeatIndicator" );
_LIT( KCcp1,                "Ccp1" );
_LIT( KCcp2,                "Ccp2" );
_LIT( KSubAddress,          "SubAddress" );

#ifndef LEGACY
_LIT( KPdpContextParams,    "PdpContextParameters" );
#endif // LEGACY

// -----------------------------------------------------------------------------
// CSatCallControl::CSatCallControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatCallControl::CSatCallControl
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatBase ( aIpc, aSat, aParser, aLog ), iDataPckg( iData )
    {
    }

// -----------------------------------------------------------------------------
// CSatCallControl::~CSatCallControl
// Destructor
// -----------------------------------------------------------------------------
//
CSatCallControl::~CSatCallControl()
    {
    }

// -----------------------------------------------------------------------------
// CSatCallControl::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatCallControl::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );

    iSat.NotifyCallControlRequest( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatCallControl::CheckData
// Verifies the validity of received call control event
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatCallControl::CheckData()
    {
    iLog.Log( _L( "CSatCallControl::CheckData" ) );
    
    TInt ret( KErrNone );
    
    // AlphaId & AlphaIdValidity
    RSat::TAlphaIdValidity validity;
    RSat::TAlphaId alphaId;
    iData.GetAlphaId( validity, alphaId );
    
    // AlphaIdValidity
    if ( KErrNone != CheckValue( KAlphaIdValidity, validity, RSat::ENoAlphaId ) )
        {
        ret = KErrCorrupt;
        }
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( alphaId ) )
        {
        ret = KErrCorrupt;
        }

    // CallControlType
    RSat::TCallControlType cct;
    iData.GetCallControlType( cct );

    if ( KErrNone != CheckValue( KType, cct, RSat::ECcTypeNotSet ) )
        {
        ret = KErrCorrupt;
        }
    if ( RSat::ECcTypeNotSet == cct )
        {
        iLog.Log( _L( "CSatCallControl::CheckData \
            - CallControlType not set!" ) );
        // ret = KErrCorrupt;
        }
        
    // GeneralResult
    RSat::TControlResult controlResult;
    iData.GetCcGeneralResult( controlResult );
    
    if ( KErrNone != CheckValue( KControlResult, controlResult ) )
        {
        ret = KErrCorrupt;
        }

    // ActionOriginator
    RSat::TActionOriginator ao;
    iData.GetActionOriginator( ao );

    if ( KErrNone != CheckValue( KActionOriginator, ao, RSat::EActionOriginatorNotSet ) )
        {
        ret = KErrCorrupt;
        }
    
    if ( RSat::EActionOriginatorNotSet == ao 
        && RSat::EAllowedWithModifications == controlResult )
        {
        iLog.Log( _L( "CSatCallControl::CheckData \
            - ActionOriginator not set!" ) );
        // ret = KErrCorrupt;
        }

    // Following methods apply only when CC modified the call    
    if ( RSat::EAllowedWithModifications == controlResult )
        {
        switch( cct )
            {
            case RSat::ECcAddress:
                {
                if ( KErrNone != CheckCallParams() )
                    {
                    ret = KErrCorrupt;
                    }
                break;
                }
            case RSat::ECcSsString:
                {
                if ( KErrNone != CheckSsParams() )
                    {
                    ret = KErrCorrupt;
                    }
                break;
                }
            case RSat::ECcUssdString:
                {
                if ( KErrNone != CheckUssdParams() )
                    {
                    ret = KErrCorrupt;
                    }
                break;
                }
#ifndef LEGACY
            case RSat::ECcPDPParameters:
                {
                // Parameters are no longer provided to client
                break;
                }
#endif // LEGACY
            case RSat::ECcTypeNotSet:
            default:
                {
                iLog.Log( _L( "CSatCallControl::CheckData \
                    - Unknown CallControlType!" ) );
                ret = KErrCorrupt;
                break;
                }
            }

        }

    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatCallControl::CheckCallParams
// Verifies the received parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatCallControl::CheckCallParams()
    {
    iLog.Log( _L( "CSatCallControl::CheckCallParams" ) );

    TInt ret( KErrNone );
    
    // BCRepeatIndicator
    RSat::TBCRepeatIndicator ri;
    iData.GetBCRepeatIndicator( ri );

    if ( KErrNone != CheckValue( KBCRepeatIndicator, ri,
        RSat::EBCAlternateMode ) )
        {
        ret = KErrCorrupt;
        }
    
    // CallName, not supported but let's make sure it is not set.
    TName name;
    iData.GetCallName( name );
    if ( name.Length() )
        {
        iLog.Log( _L( "CSatCallControl::CheckCallParams \
            - CallName received!" ) );
        ret = KErrCorrupt;
        }

    // CallSetUpParams
    RSat::TCallSetUpParams params;
    iData.GetCallSetUpDetails( params );
    
    if ( KErrNone != CheckAddress( params.iAddress ) )
        {
        ret = KErrCorrupt;
        } 
        
    if ( KErrCorrupt == CheckBuffer( KCcp1, params.iCcp1 ) )
        {
        ret = KErrCorrupt;
        }    

    if ( KErrCorrupt == CheckBuffer( KCcp2, params.iCcp2 ) )
        {
        ret = KErrCorrupt;
        }    
    
    // Text
    if ( KErrNone != CheckText( KSubAddress, params.iSubAddress ) )
        {
        ret = KErrCorrupt;
        }
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatCallControl::CheckSsParams
// Verifies the received parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatCallControl::CheckSsParams()
    {
    iLog.Log( _L( "CSatCallControl::CheckSsParams" ) );

    TInt ret( KErrNone );
    
    // Get CC modified data
    RSat::TSsString ss;
    iData.GetSendSsDetails( ss );
    
    // Because TSString- and TAddress-structures are very similar, let's
    // reuse helper method made for TAddress    
    RSat::TAddress ssAddress;
    
    ssAddress.iNumberPlan = ss.iNumberPlan;
    ssAddress.iTypeOfNumber = ss.iTypeOfNumber;
    ssAddress.iTelNumber.Copy( ss.iSsString ); // Can't overflow
    
    if ( KErrNone != CheckAddress( ssAddress ) )
        {
        ret = KErrCorrupt;
        }
            
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatCallControl::CheckUssdParams
// Verifies the received parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatCallControl::CheckUssdParams()
    {
    iLog.Log( _L( "CSatCallControl::CheckUssdParams" ) );

    TInt ret( KErrNone );
    
    // Get CC modified data
    RSat::TUssdString ussd;
    iData.GetSendUssdDetails( ussd );
    
    // Reference from script
    RSat::TUssdString scriptedUssd;
    if ( KErrNone != GetUssdString( scriptedUssd ) )
        {
        iLog.Log( _L( "CSatCallControl::CheckData \
            - Mandatory parameter UssdString missing from script!" ) );
        ret = KErrArgument;
        }
    else
        {
        // String
        if ( scriptedUssd.iUssdString != ussd.iUssdString )
            {
            iLog.Log( _L( "CSatCallControl::CheckUssdParams: ERROR \
                UssdString mismatch!" ) );
            ret = KErrCorrupt;
            }
        
        // DCS
        if ( scriptedUssd.iDcs != ussd.iDcs )
            {
            iLog.Log( _L( "CSatCallControl::CheckUssdParams: ERROR \
                UssdString DCS mismatch!" ) );
            ret = KErrCorrupt;
            }
        }
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatCallControl::CheckPdpParams
// Verifies the received parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatCallControl::CheckPdpParams()
    {
    iLog.Log( _L( "CSatCallControl::CheckPdpParams" ) );

    TInt ret( KErrNone );

#ifndef LEGACY

	TPdpParameters pdpParameters;
	iData.GetPdpParameters( pdpParameters );
        
    if ( KErrCorrupt == CheckBuffer( KPdpContextParams, pdpParameters ) )
        {
        ret = KErrCorrupt;
        }    

#endif // LEGACY
    
    return ret;
    }

// End of file
