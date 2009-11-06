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
* Description:   SetUpIdleModeText specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandopenchannel.h"

// CONSTANTS
_LIT( KAccessName,                  "AccessName" );
_LIT( KBearer,                      "Bearer" );
_LIT( KBearerParams,                "BearerParams" );
_LIT( KBufferSize,                  "BufferSize" );
_LIT( KDestinationAddress,          "DestinationAddress" );
_LIT( KLinkEstablishment,           "LinkEstablishment" );
_LIT( KLocalAddress,                "LocalAddress" );
_LIT( KReconnectionMode,            "ReconnectionMode" );
_LIT( KSimMeInterface,              "SimMeInterface" );
_LIT( KUserLogin,                   "UserLogin" );
_LIT( KUserPassword,                "UserPassword" );

const TUint KIpV4Separator = '.';
const TUint KIpV6Separator = '-';


// -----------------------------------------------------------------------------
// CCommandOpenChannel::CCommandOpenChannel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandOpenChannel::CCommandOpenChannel
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::EOpenChannelGprs ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandOpenChannel::~CCommandOpenChannel
// Destructor
// -----------------------------------------------------------------------------
//
CCommandOpenChannel::~CCommandOpenChannel()
    {
    }

// -----------------------------------------------------------------------------
// CCommandOpenChannel::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandOpenChannel::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret( KErrNone );
    
    // Clear the packages
    Init( iData );
    Init( iRsp );

    iSat.NotifyOpenChannelPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandOpenChannel::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandOpenChannel::CheckData()
    {
    iLog.Log( _L( "CCommandOpenChannel::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    TBuf<RSat::KTextStringMaxSize>buffer; // Buffer for string data (250)
    
    // AccessName
    if ( KErrNone != CheckText( KAccessName, iData.iAccessName ) )
        {
        ret = KErrCorrupt;
        }
    
    // AlphaId
    if ( KErrCorrupt == CheckAlphaId( iData.iAlphaId ) )
        {
        ret = KErrCorrupt;
        }    

    // Bearer
    if ( KErrNone != CheckValue( KBearer, iData.iBearer.iType,
        RSat::EDefaultBearer ) )
        {
        ret = KErrCorrupt;
        }
    else if ( iData.iBearer.iType != RSat::EDefaultBearer )
        {
        if ( KErrNone != CheckBuffer( KBearerParams, iData.iBearer.iParams ) )
            {
            ret = KErrArgument;
            }
        }
    else if ( iData.iBearer.iParams.Length() )
        {
        iLog.Log( _L( "CCommandOpenChannel::CheckData: ERROR Bearer: \
            Length of params should be 0 when using default bearer!" ) );
        ret = KErrCorrupt;
        }
    
    // BufferSize
    if ( KErrNone != CheckValue( KBufferSize, iData.iBufferSize, 1400 ) )
        {
        ret = KErrCorrupt;
        }    
    
    // DestinationAddress
    if ( KErrNone != CheckOtherAddress( KDestinationAddress ) )
        {
        iLog.Log( _L( "CCommandOpenChannel::CheckData: ERROR \
            DestinationAddress mismatch!" ) );
        ret = KErrCorrupt;
        }

    // IconId
    if ( KErrCorrupt == CheckIcon( iData.iIconId ) )
        {
        ret = KErrCorrupt;
        }

    // LinkEstablishment
    if ( KErrNone != CheckValue( KLinkEstablishment, iData.iLinkEst,
        RSat::EImmediate ) )
        {
        ret = KErrCorrupt;
        }    

    // LocalAddress
    if ( KErrNone != CheckOtherAddress( KLocalAddress ) )
        {
        iLog.Log( _L( "CCommandOpenChannel::CheckData: ERROR \
            LocalAddress mismatch!" ) );
        ret = KErrCorrupt;
        }

    // ReconnectionMode
    if ( KErrNone != CheckValue( KReconnectionMode, iData.iReconnectionMode,
        RSat::ENoAutomaticReconnection ) )
        {
        ret = KErrCorrupt;
        }    

    // SimMeInterface
    if ( KErrNone != CheckValue( KSimMeInterface,
        iData.iSimMeInterface.iTransportProto, RSat::EProtocolNotPresent ) )
        {
        ret = KErrCorrupt;
        }    
    if ( RSat::EProtocolNotPresent != iData.iSimMeInterface.iTransportProto )
        {
        if ( KErrNone != CheckNextValue(
            iData.iSimMeInterface.iPrtNumber, 0 ) )
            {
            ret = KErrCorrupt;
            }
        }
    
    // UserLogin
    if ( KErrNone != CheckText( KUserLogin, iData.iUserLogin ) )
        {
        ret = KErrCorrupt;
        }

    // UserPassword
    if ( KErrNone != CheckText( KUserPassword, iData.iUserPassword ) )
        {
        ret = KErrCorrupt;
        }

    return ret;
    }
 

// -----------------------------------------------------------------------------
// CCommandOpenChannel::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandOpenChannel::SendResponse()
    {
    iLog.Log( _L( "CCommandOpenChannel::SendResponse" ) );    
    
    TInt ret( KErrNone );

    // General result
    SetGeneralResult( iRsp.iGeneralResult );
    
    // Additional info & type
    SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
    
    // Bearer,use PCmd's as default
    iRsp.iBearer.iType = iData.iBearer.iType;
    GetValue( KBearer, iRsp.iBearer.iType );
    
    // Bearer params
    TInt number( 0 );
    if ( KErrNone == GetValue( KBearerParams, number, EHex ) )
        {
        do
            {
            iRsp.iBearer.iParams.Append( number );
            } while ( KErrNone == GetNextValue( number, EHex ) );
        }
    else
        {
        // Not provided by the script. Use params received in PCmd.
        iRsp.iBearer.iParams.Copy( iData.iBearer.iParams );
        }        
    
    // BufferSize, use 1400 as default
    iRsp.iBufferSize = 1400;
    GetValue( KBufferSize, iRsp.iBufferSize );

    if ( KErrNone == ret )
        {
        ret = CSatCommandBase::SendResponse( iRspPckg );
        }

    return ret;
    }
    

// -----------------------------------------------------------------------------
// CCommandOpenChannel::CheckOtherAddress
// Verifies the content of TOtherAddress structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandOpenChannel::CheckOtherAddress
        (
        const TDesC& aAddressType
        )
    {
    TInt ret( KErrNone );
    
    iLog.Log( _L( "CCommandOpenChannel::CheckOtherAddress %S" ),
        &aAddressType );
    
    // Get reference to requested member
    RSat::TOtherAddress& address = ( aAddressType 
        == KLocalAddress ? iData.iLocalAddress : iData.iDestinationAddress );
        
    switch ( address.iType )
        {
        case RSat::EAddressNotPresent:
            {
            if ( address.iAddress.Length() )
                {
                iLog.Log( _L( "CCommandOpenChannel::CheckOtherAddress: ERROR \
                    Address is found but type is AddressNotPresent!" ) );
                ret = KErrCorrupt;
                }
            break;
            }
        case RSat::EIPv4Address:
        case RSat::EIPv6Address:
            {
            // Check address.
            TBuf<RSat::KPcktAddressMaxSize> scriptedAddress;     
            if ( KErrNone != GetText( aAddressType, scriptedAddress ) )
                {
                iLog.Log( _L( "CCommandOpenChannel::CheckOtherAddress: ERROR \
                    Mandatory parameter %S missing!" ), &aAddressType );
                ret = KErrArgument;
                }
            else
                {
                // Get address type. Use default if not provided by the script
                TInt number( RSat::EIPv4Address ); 
                GetNextValue( number );
                    
                // Format address string
                // Separator character depends on address type (IPv4 vs. v6)
                TChar separator( address.iType == RSat::EIPv4Address 
                    ? KIpV4Separator : KIpV6Separator );
                
                // Reformat address' integer presentation to string format
                TUint length( address.iAddress.Length() );
                TBuf<2 * RSat::KPcktAddressMaxSize> addressAsString;
                for ( TUint i( 0 ) ; i < length ; i++ )
                    {
                    addressAsString.AppendNumFixedWidth(
                        address.iAddress[i], EHex, 2 );

                    if ( i < length - 1 )
                        {
                        addressAsString.Append( separator );
                        }
                    }
                    
                // Check address    
                if ( KErrNone != addressAsString.CompareF( scriptedAddress ) )
                    {
                    iLog.Log( _L( "CCommandOpenChannel::CheckOtherAddress: \
                        ERROR Address does not match!" ) );
                    ret = KErrCorrupt;        
                    }
                
                // Check type
                if ( number != address.iType )
                    {
                    iLog.Log( _L( "CCommandOpenChannel::CheckOtherAddress: \
                        ERROR Address type does not match!" ) );
                    ret = KErrCorrupt;        
                    }
                }
            break;
            }
        case RSat::EAddressNotSet:
        default:
            {
            iLog.Log( _L( "CCommandOpenChannel::CheckOtherAddress: ERROR \
                Address type is not properly initialized!" ) );
            
            ret = KErrCorrupt;        
            break;
            }
        }

    return ret;
    }

// End of File
