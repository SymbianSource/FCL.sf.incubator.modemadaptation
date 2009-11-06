/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef TSSPARSER_H
#define TSSPARSER_H

// INCLUDES
#include <tisi.h>  // isi message related functions
#include <ss_wmisi.h> // SS ISI message definitions

// CONSTANTS
// SS operation types
_LIT(KSsOperationErasure, "##" );
_LIT(KSsOperationInterrogation, "*#" );
_LIT(KSsOperationRegistration, "**" );
_LIT(KSsOperationActivation, "*" );
_LIT(KSsOperationDeactivation, "#" );

static const TInt SS_OPERATION_UNDEFINED( 0xFF );
static const TInt SS_CODE_UNDEFINED( 0xFF );
static const TInt MAX_NUM_OF_SUPPLEMENTARY_INFOS( 3 );
// max length of service string (from 3GPP TS 22.030)
static const TInt MAX_LENGTH_OF_SERVICE_STRING( 40 );

// DATA TYPES
//
enum TCharacter
    {
    ECharacterSeparator = 0, // '*'
    ECharacterEndMark, // '#'
    ECharacterIllegal
    };

// CLASS DECLARATION

/**
* Parser class for SS service string received from client (SAT server):
* Parsers SS service string: operation, service code and service information
*/
class TSsParser
    {

    public: // New functions

        /**
        * Constructor
        */
        TSsParser();

        /**
        * Parses a SS string
        * @param const TDesC& aServiceString: SS string to be parsed
        * @return TInt: error
        */
        TInt Parse( const TDesC& aServiceString );

        /**
        * Gets SS operation type
        * @param TUint8& aSsOperation: SS operation type
        * @return void
        */
        void GetSsOperation( TUint8& aSsOperation ) const;

        /**
        * Gets SS code
        * @param TUint16& aServiceCode: SS code
        * @return void
        */
        void GetServiceCode( TUint16& aServiceCode ) const;

        /**
        * Gets service infos a, b and c. If SS info does not exist, empty
        * descriptor is returned
        * @param TPtrC& aSupplementaryInfoA: SS info A
        * @param TPtrC& aSupplementaryInfoB: SS info B
        * @param TPtrC& aSupplementaryInfoC: SS info C
        * @return void
        */
        void GetServiceInfo(
            TPtrC& aSupplementaryInfoA,
            TPtrC& aSupplementaryInfoB,
            TPtrC& aSupplementaryInfoC ) const;

        /**
        * Converts descriptor to integer
        * @param const TDesC& aString: descriptor to be converted
        * @return TInt: converted value
        */
        TInt GetInt( const TDesC& aString );

    private:

        /**
        * Extracts operation type from service string
        * @return TUint8: operation type
        */
        TUint8 ExtractSsOperation( );

        /**
        * Extracts parts from service string
        * @return void
        */
        void ExtractPart( TPtrC& aPart );

        /**
        * Extracts separator marks ( *, # ) from service string
        * @return TCharacter: separator mark
        */
        TCharacter ExtractSeparator( );


    private:    // Data

        TUint8 iSsOperation; // operation type, e.g. activation
        TUint16 iServiceCode; // supplementary service code
        TPtrC iServiceString; // ss string to be parsed
        TPtrC iSupplementaryInfoA; // service information A, defined in 3GPP TS 22-030
        TPtrC iSupplementaryInfoB; // service information B  defined in 3GPP TS 22-030
        TPtrC iSupplementaryInfoC; // service information C, defined in 3GPP TS 22-030
    };

#endif      // TSSPARSER_H

// End of File
