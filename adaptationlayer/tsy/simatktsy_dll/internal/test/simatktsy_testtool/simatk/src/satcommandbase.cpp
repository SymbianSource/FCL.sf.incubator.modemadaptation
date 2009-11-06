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
* Description:   Abstract base class for all SIM ATK TSY proactive command
*                tests classes. Inherits CSatBase. Adds functionality to
*                generate responses to SIM by providing general helper methods
*                for all subclasses.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "satcommandbase.h"

// CONSTANTS
_LIT( KDuration,                "Duration" );
_LIT( KGeneralResult,           "GeneralResult" );
_LIT( KGeneralResult2,          "GeneralResult2" );
_LIT( KInfoType,                "InfoType" );
_LIT( KInfoType2,               "InfoType2" );
_LIT( KAdditionalInfo,          "AdditionalInfo" );
_LIT( KAdditionalInfo2,         "AdditionalInfo2" );
_LIT( KSendEnvelope,            "SendEnvelope" );

// Successful general results are 0x0X.
const TInt KSuccessMask     = 0xF0;


// -----------------------------------------------------------------------------
// CSatCommandBase::CSatCommandBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatCommandBase::CSatCommandBase
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog,        
        const RSat::TPCmd   aCommandType
        ) : CSatBase ( aIpc, aSat, aParser, aLog ), iCommandType( aCommandType )
    {
    }

// -----------------------------------------------------------------------------
// CSatCommandBase::~CSatCommandBase
// Destructor
// -----------------------------------------------------------------------------
//
CSatCommandBase::~CSatCommandBase()
    {  
    }
    
    
// -----------------------------------------------------------------------------
// CSatCommandBase::SendResponse
// Sends Terminal Response to Etel
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatCommandBase::SendResponse
        (
        const TDesC8& aRspPckg
        )
    {
    TRequestStatus sendStatus;
    iSat.TerminalRsp( sendStatus, iCommandType, aRspPckg );
    User::WaitForRequest( sendStatus );
        
    return sendStatus.Int();
    }
    

// -----------------------------------------------------------------------------
// CSatCommandBase::SetGeneralResult
// Parses general result from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TBool CSatCommandBase::SetGeneralResult
        (
        RSat::TPCmdResult& aGeneralResult,
        TResultNumber aNumber
        )
    {
    // Set to default
    aGeneralResult = RSat::KSuccess;
    
    // Search either GeneralResult or GeneralResult2 from the script
    TPtrC keyWord( aNumber == KFirst ? KGeneralResult() : KGeneralResult2() );
    
    // Look for integer GR first
    if ( KErrNone != GetValue( keyWord, aGeneralResult, EHex ) )
        {
        TPtrC resultString;
    
        // Integer not found, try string instead
        if ( KErrNone == GetText( keyWord, resultString ) )
            {
            TUint result( MapToValue( resultString, KGeneralResultList,
                KGeneralResultListSize ) );
            aGeneralResult = static_cast<RSat::TPCmdResult>( result );
            }
        }
    
    // Return true if result is success (= 0x0X)
    return aGeneralResult & KSuccessMask ? EFalse : ETrue;
    }

// -----------------------------------------------------------------------------
// CSatCommandBase::SetAdditionalInfo
// Parses additional info from the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatCommandBase::SetAdditionalInfo
        (
        TUint& aInfoType,
        RSat::TAdditionalInfo& aAdditionalInfo,
        TResultNumber aNumber
        )
    {
    TInt ret( KErrNone );
    
    // Use KNoAdditionalInfo as a default value
    aInfoType = RSat::KNoAdditionalInfo;
    aAdditionalInfo.Zero();
    
    // Search either InfoType and AdditionalInfo or
    // InfoType2 and AdditionalInfo2 from the script
    const TPtrC typeKeyWord( aNumber == KFirst ? KInfoType() : KInfoType2() );
    const TPtrC infoKeyWord( aNumber == KFirst 
        ? KAdditionalInfo() : KAdditionalInfo2() );
        
        
    // Parse InfoType from script. First try to find numeric value, 
    // if not found, then try string presentation
    TPtrC infoTypeString;
    TUint scriptedType( 0 );
    
    // Look for integer type first
    if ( KErrNone != GetValue( typeKeyWord, scriptedType, EHex ) )
        {
        // Integer not found, try string instead
        if ( KErrNone == GetText( typeKeyWord, infoTypeString ) )
            {
            TInt value( MapToValue( infoTypeString, KInfoTypeList,
                KInfoTypeListSize ) );
            if ( KErrNotFound != value )
                {
                scriptedType = value;
                }
            }
        }

    if ( 0 != scriptedType )
        {
        // Type was found from the script
        aInfoType = scriptedType;
        }

    // AdditionalInfo
    if ( RSat::KNoAdditionalInfo != aInfoType )
        {
        // Info should be present. Try to parse AdditionalInfo from
        // the script
        RSat::TAdditionalInfo scriptedInfo;
        if ( KErrNone == GetText( infoKeyWord, scriptedInfo ) )
            {
            // AdditionalInfo tag found from the script. first try to
            // map the string presentation to a value. It this fails,
            // copy parsed value directly to the Addtitional info
            TInt value ( MapToValue( scriptedInfo, KAdditionalInfoList,
                KAdditionalInfoListSize ) );
                
            if ( KErrNotFound != value )
                {
                aAdditionalInfo.Append( value );
                }
            else
                {
                aAdditionalInfo.Copy( scriptedInfo );
                }
            }
        }

    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSatCommandBase::SetDuration
// Parses duration struct for the script
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatCommandBase::SetDuration
        (
        RSat::TDuration& aDuration
        )
    {
    TInt ret( KErrNone );

    // Time Units, default NoDurationAvailable
    aDuration.iTimeUnit = RSat::ENoDurationAvailable;

    GetValue( KDuration, aDuration.iTimeUnit );
    
    if ( RSat::ENoDurationAvailable != aDuration.iTimeUnit )
        {
        // Number of units
        TInt units( 0 );
        if ( KErrNone == GetNextValue( units ) )
            {
            aDuration.iNumOfUnits = units;
            }
        else
            {
            ret = KErrNotFound;
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatCommandBase::IsEnvelope
// Returns identifies whether the script line is an envelope or not
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TBool CSatCommandBase::IsEnvelope()
    {
    TBool ret ( EFalse );
    TPtrC script;
    
    if ( KErrNone == GetText( KSendEnvelope, script ) )
        {
        ret = ETrue;
        }
        
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatCommandBase::MapToValue
// Maps a string presentation of value to a numeric value
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CSatCommandBase::MapToValue
        (
        const TDesC& aStringValue,
        TValuePair const aValueArray[],
        const TUint aSize
        )
    {
    TInt aValue( KErrNotFound );
    
    TPtrC current;
    
    for ( TUint i = 0 ; i < aSize && KErrNotFound == aValue ; i++ )
        {
        current.Set( aValueArray[i].iString );

        if ( KErrNotFound != aStringValue.MatchC( current ) )
            {
            aValue = aValueArray[i].iValue ;
            }
        }

    return aValue;
    }
    
// End of File
