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



#ifndef TOPERATORNAMES_H
#define TOPERATORNAMES_H

// INCLUDES
#include <e32std.h>

// CONSTANTS
    //None

// MACROS
    //None

// DATA TYPES
    //None

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// CLASS DECLARATION

/**
* Used for reading operator detailed information from operators.h file.
*/
class TOperatorNames
    {
    public:

        /**
        * Contructor.
        */
        TOperatorNames();

        /**
        * Destructor.
        */
        ~TOperatorNames();

    public:

        /**
        * Gets Country name from list
        * @param aMCC is Country code
        * @param aCountryId country name from list
        * @return KErrNone or error code
        */
        static TInt GetCountryID(
            const TUint aMCC,
            TDes& aCountryId );

        /**
        * Generates Latin name from list
        * @param aMCC is Country code
        * @param aMNC is Network code
        * @param aOperatorName is Operator name from list
        * @return KErrNone or error code
        */
        static TInt GetLatinName(
            const TUint aMCC,
            const TUint aMNC,
            TDes& aOperatorName );

        /**
        * Generates APAC name from list
        * @param aMCC is Country code
        * @param aMNC is Network code
        * @param aOperatorName is Operator name from list
        * @return KErrNone or error code
        */
        static TInt GetUnicodeName(
            const TUint aMCC,
            const TUint aMNC,
            TDes& aOperatorName );

        /**
        * Generates Country + MNC name from list
        * @param aMCC is Country code
        * @param aMNC is Network code
        * @param aOperatorName is Operator name from list
        * @return KErrNone or error code
        */
        static TInt GetCountryMNCName(
            const TUint aMCC,
            const TUint aMNC,
            TDes& aOperatorName );

        /**
        * Generates MCC + MNC name from list
        * @param aMCC is Country code
        * @param aMNC is Network code
        * @param aOperatorName is Operator name from list
        */
        static void GetMCCMNCName(
            const TUint aMCC,
            const TUint aMNC,
            TDes& aOperatorName );

        /**
        * Gets Operator name from list
        * @param aMCC is Country code
        * @param aMNC is Network code
        * @param aOperatorName is Operator name from list
        * @return hard coded nametype value
        */
        static TInt GetName(
            const TUint aMCC,
            const TUint aMNC,
            TDes& aOperatorName );
    };

#endif // TOPERATORNAMES_H

// End of File
