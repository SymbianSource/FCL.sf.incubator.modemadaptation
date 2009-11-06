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



#ifndef DPPARITYHANDLER_H
#define DPPARITYHANDLER_H

// INCLUDES
// none

// CONSTANTS
// none

// MACROS
// none

// DATA TYPES
// none

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  CDpParityHandler is parity handler.
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CDpParityHandler :public CBase
    {
    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CDpParityHandler();

        /**
        * Destructor.
        */
        ~CDpParityHandler();

    public: // New methods

        /**
        * This method returns even parity for byte.
        * @param aByte Byte
        * @return Even parity
        */
        inline const TUint8 Even( const TUint8 aByte );

        /**
        * This method returns odd parity for byte.
        * @param aByte Byte
        * @return Odd parity
        */
        inline const TUint8 Odd( const TUint8 aByte );

        /**
        * This method return space parity for byte.
        * @param aByte Byte
        * @return Space parity
        */
        inline const TUint8 Space( const TUint8 aByte );

        /**
        * This method return mark parity for byte.
        * @param aByte Byte
        * @return Mark parity
        */
        inline const TUint8 Mark( const TUint8 aByte );

        /**
        * This method adds parity bits to descriptor.
        * @param aDes Descriptor
        * @param aParity Parity setting
        * @return Error code
        */
        TInt AddParityBits( TPtr8& aDes, const TParity aParity );

        /**
        * This method extracts parity bits from descriptor.
        * @param aDes Descriptor
        * @return Error code
        */
        TInt ExtractParityBits( TPtr8& aDes );

    public:  // Methods from base classes

        // none

    protected:  // New methods
        
        // none

    protected:  // Methods from base classes

        // none

    private: // Methods

        /**
        * This method generates lookup table to get different parity bits.
        */
        void GenerateLookupTable();

    public:     // Data
        // none

    protected:  // Data
        // none

    private:    // Data
        // Parity table
        TUint8 iTable[KDpParityTableSize][KDpParityTableWidth];

    };

#include "dpparityhandler.inl" // inline methods

#endif // DPPARITYHANDLER_H

// End of File
