/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



// INCLUDE FILES
#include "toperatornames.h"
#include "operators.h"
#include "cmmnetoperatornamehandler.h"
#include "cmmstaticutility.h"
#include "tsylogger.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "toperatornamesTraces.h"
#endif


// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CopyToDes
// Copies C-string to descriptor.
// -----------------------------------------------------------------------------
//
template <class T> void CopyToDes( TDes& aDest, const T* str )
    {
TFLOGSTRING("TSY: CopyToDes");
OstTrace0( TRACE_NORMAL, _COPYTODES, "::CopyToDes" );

    aDest.Zero();
    while ( *str && aDest.Length() < aDest.MaxLength() )
        {
        aDest.Append( ( TText ) *str++ );
        }
    }

// -----------------------------------------------------------------------------
// FindCountry
// Search Country name from country list.
// -----------------------------------------------------------------------------
//
const TCountry* FindCountry( const TUint aMCC )
    {
TFLOGSTRING("TSY: FindCountry");
OstTrace0( TRACE_NORMAL, _FINDCOUNTRY, "::FindCountry" );

    const TCountry* country( NULL );

    for ( TInt i = 0; i < sizeof( countrylist ) / sizeof( TCountry ); i++ )
        {
        if ( countrylist[i].iMCC == aMCC )
            {
            country = &countrylist[i];
            break;
            }
        }

    return country;
    }

// -----------------------------------------------------------------------------
// FindOperator
// Search Operator name from operators list.
// -----------------------------------------------------------------------------
//
const TOperator* FindOperator( const TUint aMCC, const TUint aMNC )
    {
TFLOGSTRING("TSY: FindOperator");
OstTrace0( TRACE_NORMAL, _FINDOPERATOR, "::FindOperator" );

    const TOperator* oper( NULL );
    const TCountry* country( FindCountry( aMCC ) );

    if ( country )
        {
        for ( TInt i = 0; i < country->iOperatorCount; i++ )
            {
            if ( country->iOperators[i].iMNC == aMNC )
                {
                oper = &country->iOperators[i];
                break;
                }
            }
        }

    return oper;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TOperatorNames::GetCountryID
// Gets Country name from list.
// -----------------------------------------------------------------------------
//
TInt TOperatorNames::GetCountryID(
    const TUint aMCC,
    TDes& aCountryId )
    {
TFLOGSTRING("TSY: TOperatorNames::GetCountryID");
OstTrace0( TRACE_NORMAL, TOPERATORNAMES_GETCOUNTRYID, "TOperatorNames::GetCountryID" );

    TInt ret( KErrNotFound );
    const TCountry* country( FindCountry( aMCC ) );

    if ( country )
        {
        CopyToDes( aCountryId, country->iCountryId );
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// TOperatorNames::GetLatinName
// Generates Latin name from list.
// -----------------------------------------------------------------------------
//
TInt TOperatorNames::GetLatinName(
    const TUint aMCC,
    const TUint aMNC,
    TDes& aOperatorName )
    {
TFLOGSTRING("TSY: TOperatorNames::GetLatinName");
OstTrace0( TRACE_NORMAL, TOPERATORNAMES_GETLATINNAME, "TOperatorNames::GetLatinName" );

    TInt ret( KErrNotFound );
    const TOperator* oper( FindOperator( aMCC, aMNC ) );

    if ( oper )
        {
        CopyToDes( aOperatorName, oper->iLatinName );
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// TOperatorNames::GetUnicodeName
// Generates APAC name from list.
// -----------------------------------------------------------------------------
//
TInt TOperatorNames::GetUnicodeName(
    const TUint aMCC,
    const TUint aMNC,
    TDes& aOperatorName )
    {
TFLOGSTRING("TSY: TOperatorNames::GetUnicodeName");
OstTrace0( TRACE_NORMAL, TOPERATORNAMES_GETUNICODENAME, "TOperatorNames::GetUnicodeName" );

    TInt ret( KErrNotFound );
    const TOperator* oper( FindOperator( aMCC, aMNC ) );

    if ( oper && oper->iUnicodeName )
        {
        CopyToDes( aOperatorName, oper->iUnicodeName );
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// TOperatorNames::GetCountryMNCName
// Generates Country + MNC name from list.
// -----------------------------------------------------------------------------
//
TInt TOperatorNames::GetCountryMNCName(
    const TUint aMCC,
    const TUint aMNC,
    TDes& aOperatorName )
    {
TFLOGSTRING("TSY: TOperatorNames::GetCountryMNCName");
OstTrace0( TRACE_NORMAL, TOPERATORNAMES_GETCOUNTRYMNCNAME, "TOperatorNames::GetCountryMNCName" );

    TInt ret( KErrNotFound );
    const TCountry* country( FindCountry( aMCC ) );

    if ( country )
        {
        CopyToDes( aOperatorName, country->iCountryId );
        aOperatorName.Append(' '); // space
        aOperatorName.AppendNum( aMNC );
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// TOperatorNames::GetMCCMNCName
// Generates MCC + MNC name from list.
// -----------------------------------------------------------------------------
//
void TOperatorNames::GetMCCMNCName(
    const TUint aMCC,
    const TUint aMNC,
    TDes& aOperatorName )
    {
TFLOGSTRING("TSY: TOperatorNames::GetMCCMNCName");
OstTrace0( TRACE_NORMAL, TOPERATORNAMES_GETMCCMNCNAME, "TOperatorNames::GetMCCMNCName" );

    aOperatorName.Zero();

    if ( aMCC != 0 || aMNC != 0 )
        {
        aOperatorName.AppendNum( aMCC );
        aOperatorName.Append(' '); // space
        aOperatorName.AppendNum( aMNC );
        }
    }

// -----------------------------------------------------------------------------
// TOperatorNames::GetName
// Gets Operator name from list.
// -----------------------------------------------------------------------------
//
TInt TOperatorNames::GetName(
    const TUint aMCC,
    const TUint aMNC,
    TDes& aOperatorName )
    {
TFLOGSTRING("TSY: TOperatorNames::GetName");
OstTrace0( TRACE_NORMAL, TOPERATORNAMES_GETNAME, "TOperatorNames::GetName" );

    TInt ret( KErrNotFound );

    TInt nameType( ENetNameNotSet );

    aOperatorName.Zero();

    if ( CMmStaticUtility::IsOperinfoApac() )
        {
        ret = GetUnicodeName( aMCC, aMNC, aOperatorName );

        if ( KErrNone == ret )
            {
            // Name type is APAC name.
            nameType = ENetApacName;
            }
        }

    if ( KErrNone != ret )
        {
        ret = GetLatinName( aMCC, aMNC, aOperatorName );

        if ( KErrNone == ret )
            {
            // Name type is Latin name.
            nameType = ENetLatinName;
            }
        }

    if ( KErrNone != ret )
        {
        ret = GetCountryMNCName( aMCC, aMNC, aOperatorName );

        if ( KErrNone == ret )
            {
            // Name type is Country name + MNC number.
            nameType = ENetCountryMncName;
            }
        }

    if ( KErrNone != ret )
        {
        GetMCCMNCName( aMCC, aMNC, aOperatorName );

        // Name type is MCC number + MNC number.
        nameType = ENetMccMncName;
        }

    return nameType;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
    //None

//  End of File
