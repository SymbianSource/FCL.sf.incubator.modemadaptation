/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "tssparser.h" // class definition file
#include "tsylogger.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tssparserTraces.h"
#endif
 // for TSY logging

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSsParser::TSsParser()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSsParser::TSsParser()
    {
TFLOGSTRING( "TSY: TSsParser::TSsParser" );
OstTrace0( TRACE_NORMAL, TSSPARSER_TSSPARSER, "TSsParser::TSsParser" );
    iSsOperation = SS_OPERATION_UNDEFINED;
    iServiceCode = SS_UNKNOWN_SERVICE;
    }


// -----------------------------------------------------------------------------
// TSsParser::Parse
// Parses a SS string
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt TSsParser::Parse( const TDesC& aServiceString )
    {
TFLOGSTRING2( "TSY: TSsParser::Parse, servicestring: %S", &aServiceString );
OstTraceExt1( TRACE_NORMAL, TSSPARSER_PARSE, "TSsParser::Parse;aServiceString=%S", aServiceString );

    TInt error( KErrNone );

    // at first check that service string is not too long
    if( MAX_LENGTH_OF_SERVICE_STRING < aServiceString.Length( ) ||
         0 >= aServiceString.Length( ) )
        {
TFLOGSTRING( "TSY: TSsParser::Parse, error: service string too long or does not exist!" );
OstTrace0( TRACE_NORMAL, DUP1_TSSPARSER_PARSE, "TSsParser::Parse, error: service string too long or does not exist!" );
        error = KErrArgument;
        }

    else
        {
        // copy service string to member variable for parsing
        iServiceString.Set( aServiceString );
        // operation type: activation, deactivation, interrogation,
        // registration or erasure
        error = ExtractSsOperation( );
        }

    // If operation was parsed successfully, extract service code
    if( KErrNone == error )
        {
        TPtrC tempBuffer;
        ExtractPart( tempBuffer );
        // convert service code to integer
        iServiceCode = GetInt( tempBuffer );
TFLOGSTRING2( "TSY: TSsParser::Parse, service code: %d", iServiceCode );
OstTraceExt1( TRACE_NORMAL, DUP2_TSSPARSER_PARSE, "TSsParser::Parse;iServiceCode=%hu", iServiceCode );

        if( SS_UNKNOWN_SERVICE == iServiceCode )
            {
            error = KErrArgument;
TFLOGSTRING( "TSY: TSsParser::Parse, error: illegal sevice code!" );
OstTrace0( TRACE_NORMAL, DUP3_TSSPARSER_PARSE, "TSsParser::Parse, error: illegal sevice code!" );
            }
        }

    // service code was parsed successfully, extract supplementary infos a,b,c
    if( KErrNone == error )
        {
        TPtrC tempSiBuffer[MAX_NUM_OF_SUPPLEMENTARY_INFOS];
        TInt i( 0 );
        TCharacter character( ECharacterIllegal );
        // 3 supplementary info fields may exists, separated by '*'
        // and ended by '#'.
        // possible combinations for supplementary info:
        // *SIA*SIB*SIC#, *SIA*SIB#, *SIA**SIC#, *SIA#, **SIB*SIC#,
        // **SIB#, ***SIC#, # (from 3GPP TS 22.030)
        for( i = 0; i <= MAX_NUM_OF_SUPPLEMENTARY_INFOS; i++ )
            {
            character = ExtractSeparator( );
            if( ECharacterSeparator == character &&
                 i < MAX_NUM_OF_SUPPLEMENTARY_INFOS ) // last mark can't be '*'
                {
                ExtractPart( tempSiBuffer[i] );
                }
            else if( ECharacterEndMark == character
                && !iServiceString.Length() )
                {
                // this was the end of string, and there's no more to be parsed
                break;
                }
            else
                {
                error = KErrArgument;
TFLOGSTRING( "TSY: TSsParser::Parse, error: illegal end mark!" );
OstTrace0( TRACE_NORMAL, DUP4_TSSPARSER_PARSE, "TSsParser::Parse, error: illegal end mark!" );
                break;
                }
            }
        iSupplementaryInfoA.Set( tempSiBuffer[0] );
        iSupplementaryInfoB.Set( tempSiBuffer[1] );
        iSupplementaryInfoC.Set( tempSiBuffer[2] );
        }

TFLOGSTRING2( "TSY: TSsParser::Parse, SS code: %d", iSsOperation );
OstTraceExt1( TRACE_NORMAL, DUP5_TSSPARSER_PARSE, "TSsParser::Parse;iSsOperation=%hhu", iSsOperation );
TFLOGSTRING2( "TSY: TSsParser::Parse, SS code: %d", iServiceCode );
OstTraceExt1( TRACE_NORMAL, DUP6_TSSPARSER_PARSE, "TSsParser::Parse;iServiceCode=%hu", iServiceCode );
TFLOGSTRING2( "TSY: TSsParser::Parse, SS info A: %S", &iSupplementaryInfoA );
OstTraceExt1( TRACE_NORMAL, DUP7_TSSPARSER_PARSE, "TSsParser::Parse;iSupplementaryInfoA=%S", iSupplementaryInfoA );
TFLOGSTRING2( "TSY: TSsParser::Parse, SS info B: %S", &iSupplementaryInfoB );
OstTraceExt1( TRACE_NORMAL, DUP8_TSSPARSER_PARSE, "TSsParser::Parse;iSupplementaryInfoB=%S", iSupplementaryInfoB );
TFLOGSTRING2( "TSY: TSsParser::Parse, SS info C: %S", &iSupplementaryInfoC );
OstTraceExt1( TRACE_NORMAL, DUP9_TSSPARSER_PARSE, "TSsParser::Parse;iSupplementaryInfoC=%S", iSupplementaryInfoC );
TFLOGSTRING2( "TSY: TSsParser::Parse, SS string left: %S", &iServiceString );
OstTraceExt1( TRACE_NORMAL, DUP10_TSSPARSER_PARSE, "TSsParser::Parse;iServiceString=%S", iServiceString );

    return error;
    }


// -----------------------------------------------------------------------------
// TSsParser::GetSsOperation
// Gets ss operation type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TSsParser::GetSsOperation( TUint8& aSsOperation ) const
    {
TFLOGSTRING( "TSY: TSsParser::GetSsOperation" );
OstTrace0( TRACE_NORMAL, TSSPARSER_GETSSOPERATION, "TSsParser::GetSsOperation" );
    aSsOperation = iSsOperation;
    }


// -----------------------------------------------------------------------------
// TSsParser::GetServiceCode
// Gets ss code
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TSsParser::GetServiceCode( TUint16& aServiceCode ) const
    {
TFLOGSTRING("TSY: TSsParser::GetServiceCode" );
OstTrace0( TRACE_NORMAL, TSSPARSER_GETSERVICECODE, "TSsParser::GetServiceCode" );
    aServiceCode = iServiceCode;
    }


// -----------------------------------------------------------------------------
// TSsParser::GetServiceInfo
// Gets SS infos a,b and c. If does not exist, empty descriptor is returned
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TSsParser::GetServiceInfo(
    TPtrC& aSupplementaryInfoA,
    TPtrC& aSupplementaryInfoB,
    TPtrC& aSupplementaryInfoC ) const
    {
TFLOGSTRING( "TSY: TSsParser::GetServiceInfo" );
OstTrace0( TRACE_NORMAL, TSSPARSER_GETSERVICEINFO, "TSsParser::GetServiceInfo" );
    aSupplementaryInfoA.Set( iSupplementaryInfoA );
    aSupplementaryInfoB.Set( iSupplementaryInfoB );
    aSupplementaryInfoC.Set( iSupplementaryInfoC );
    }


// -----------------------------------------------------------------------------
// TSsParser::ExtractSsOperation
// Parses operation type ( 1 or 2 first elements from service string )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TSsParser::ExtractSsOperation( )
    {
TFLOGSTRING( "TSY: TSsParser::ExtractSsOperation" );
OstTrace0( TRACE_NORMAL, TSSPARSER_EXTRACTSSOPERATION, "TSsParser::ExtractSsOperation" );
    TInt error( KErrNone );

    // SS operation is one or two first characters in the service string
    if( iServiceString.Left( 2 ) == KSsOperationErasure )
        {
        iSsOperation = SS_ERASURE; // unregister and deactivate
        iServiceString.Set( iServiceString.Mid( 2 ) ); // extract remaining part
TFLOGSTRING2( "TSY: TSsParser::ExtractSsOperation, SS operation: erasure (%d)", iSsOperation );
OstTraceExt1( TRACE_NORMAL, DUP1_TSSPARSER_EXTRACTSSOPERATION, "TSsParser::ExtractSsOperation;SS operation: erasure (%hhu)", iSsOperation );
        }
    else if( iServiceString.Left( 2 ) == KSsOperationInterrogation )
        {
        iSsOperation = SS_INTERROGATION; // check status
        iServiceString.Set( iServiceString.Mid ( 2 ) );
TFLOGSTRING2( "TSY: TSsParser::ExtractSsOperation, SS operation: interrogation (%d)", iSsOperation );
OstTraceExt1( TRACE_NORMAL, DUP2_TSSPARSER_EXTRACTSSOPERATION, "TSsParser::ExtractSsOperation;SS operation: interrogation (%hhu)", iSsOperation );
        }
    else if( iServiceString.Left( 2 ) == KSsOperationRegistration )
        {
        iSsOperation = SS_REGISTRATION; // register and activate
        iServiceString.Set( iServiceString.Mid( 2 ) );
TFLOGSTRING2( "TSY: TSsParser::ExtractSsOperation, SS operation: registration (%d)", iSsOperation );
OstTraceExt1( TRACE_NORMAL, DUP3_TSSPARSER_EXTRACTSSOPERATION, "TSsParser::ExtractSsOperation;SS operation: registration (%hhu)", iSsOperation );
        }
    else if( iServiceString.Left( 1 ) == KSsOperationActivation )
        {
        iSsOperation = SS_ACTIVATION; // activate
        iServiceString.Set( iServiceString.Mid( 1 ) );
TFLOGSTRING2( "TSY: TSsParser::ExtractSsOperation, SS operation: activation (%d)", iSsOperation );
OstTraceExt1( TRACE_NORMAL, DUP4_TSSPARSER_EXTRACTSSOPERATION, "TSsParser::ExtractSsOperation;SS operation: activation (%hhu)", iSsOperation );
        }
    else if( iServiceString.Left( 1 ) == KSsOperationDeactivation )
        {
        iSsOperation = SS_DEACTIVATION; // unregister
        iServiceString.Set( iServiceString.Mid( 1 ) );
TFLOGSTRING2( "TSY: TSsParser::ExtractSsOperation, SS operation: deactivation (%d)", iSsOperation );
OstTraceExt1( TRACE_NORMAL, DUP5_TSSPARSER_EXTRACTSSOPERATION, "TSsParser::ExtractSsOperation;SS operation: deactivation (%hhu)", iSsOperation );
        }
    else
        {
        iSsOperation = SS_OPERATION_UNDEFINED;
TFLOGSTRING2( "TSY: TSsParser::ExtractSsOperation, SS operation: undefined (%d)", iSsOperation );
OstTraceExt1( TRACE_NORMAL, DUP6_TSSPARSER_EXTRACTSSOPERATION, "TSsParser::ExtractSsOperation;SS operation: undefined (%hhu)", iSsOperation );
        error = KErrArgument;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// TSsParser::ExtractPart
// Extras individual parts from service string
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TSsParser::ExtractPart( TPtrC& aPart )
    {
TFLOGSTRING( "TSY: TSsParser::ExtractPart" );
OstTrace0( TRACE_NORMAL, TSSPARSER_EXTRACTPART, "TSsParser::ExtractPart" );
    TUint i( 0 );
    // service code and supplementary info ends to '*' or '#'
    // if there is no data to be extracted, empty buffer is returned.
    while( i < iServiceString.Length( ) &&
           iServiceString[i] != '*' &&
           iServiceString[i] != '#' )
        {
        i++;
        }
    // extracted part is returned as reference variable
    aPart.Set( iServiceString.Left( i ) );
TFLOGSTRING2( "TSY: TSsParser::ExtractPart, extracted string: %S", &aPart );
OstTraceExt1( TRACE_NORMAL, DUP1_TSSPARSER_EXTRACTPART, "TSsParser::ExtractPart;aPart=%S", aPart );
    // set remaining part of service string
    iServiceString.Set( iServiceString.Mid( i ) );
    }


// -----------------------------------------------------------------------------
// TSsParser::ExtractSeparator
// Extracts separator marks from service string
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCharacter TSsParser::ExtractSeparator( )
    {
TFLOGSTRING( "TSY: TSsParser::ExtractSeparator" );
OstTrace0( TRACE_NORMAL, TSSPARSER_EXTRACTSEPARATOR, "TSsParser::ExtractSeparator" );
    TCharacter mark( ECharacterIllegal );
    // There should be at least the end mark '#' left in string
    if( iServiceString.Length() > 0 )
        {
        if( '#' == iServiceString[0] )
            {
TFLOGSTRING( "TSY: TSsParser::ExtractSeparator: end mark '#'" );
OstTrace0( TRACE_NORMAL, DUP1_TSSPARSER_EXTRACTSEPARATOR, "TSsParser::ExtractSeparator, end mark '#'" );
            mark = ECharacterEndMark;
            }
        else if( '*' == iServiceString[0] )
            {
TFLOGSTRING( "TSY: TSsParser::ExtractSeparator: separator mark '*'" );
OstTrace0( TRACE_NORMAL, DUP2_TSSPARSER_EXTRACTSEPARATOR, "TSsParser::ExtractSeparator, separator mark '*'" );
            mark = ECharacterSeparator;
            }
        // no else
        iServiceString.Set( iServiceString.Mid( 1 ) );
        }
    else // end mark was missing
        {
TFLOGSTRING( "TSY: TSsParser::ExtractSeparator: illegal ss string, no end mark!" );
OstTrace0( TRACE_NORMAL, DUP3_TSSPARSER_EXTRACTSEPARATOR, "TSsParser::ExtractSeparator, illegal ss string, no end mark!" );
        }
    return mark;
    }


// -----------------------------------------------------------------------------
// TSsParser::GetInt
// Convert descriptor to integer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt TSsParser::GetInt( const TDesC& aString )
    {
TFLOGSTRING( "TSY: TSsParser::GetInt" );
OstTrace0( TRACE_NORMAL, TSSPARSER_GETINT, "TSsParser::GetInt" );
    TInt code;

    if( TLex( aString ).Val( code ) != KErrNone)
        {
        code = SS_ALL_TELE_AND_BEARER;
        }

    return code;
    }


//  End of File
