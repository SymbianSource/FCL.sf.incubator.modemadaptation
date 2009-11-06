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



#ifndef DPTERMDETECT_H
#define DPTERMDETECT_H

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
*  CDpTermDetect detects terminator from descriptor.
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CDpTermDetect :public CBase
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CDpTermDetect( CDpDataConfig& aDataConfig );

        /**
        * Destructor.
        */
        ~CDpTermDetect();

    public: // New methods

        /**
        * This methods scans terminator characters from descriptor. Terminator
        * arrays's terminator characters are scanned sequentially.
        * @param aDes Descriptor
        * @return Index of terminator in descriptor
        */
        TInt Scan( const TPtr8& aDes );

    public: // Methods from base classes
        
        // none

    protected:  // New methods
        
        // none

    protected: // Methods from base classes
        
        // none

    private: // Methods

        // none

    public:     // Data
        // none

    protected:  // Data
        // none

    private:    // Data
		// Data configuration
        CDpDataConfig& iDataConfig;

    };

#endif      // DPTERMDETECT_H

// End of File
