/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _CMMPHONEBOOKALPHASTRING_H
#define _CMMPHONEBOOKALPHASTRING_H

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

// INCLUDES
#include <e32base.h>

// CONSTANTS
    // None

// MACROS
    // None

// DATA TYPES
    // None

// EXTERNAL DATA STRUCTURES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATION
    // None

// CLASS DECLARATION

/**
* cmmmPhonebookAlphaString class is used to handle SIMphonebook-specific data
* for AAS and GAS.
*/
class CMmPhonebookAlphaString : public CBase
    {
    public:

        /**
        * Destructor.
        */
        virtual ~CMmPhonebookAlphaString()
            {
            };

        /**
        * C++ default constructor.
        */
        inline CMmPhonebookAlphaString()
            {
            iLocation = 0;
            iAasString.Zero();
            iGasString.Zero();
            iType = RMmCustomAPI::EAas;
            };

    public: // PB SIM Store related data

        // Keeps track of alphastring location
        TUint16 iLocation;

        // Buffer of Additional Alpha String
        TBuf<40> iAasString;

        // Buffer of Group Alpha String
        TBuf<40> iGasString;

        // Separate AAS and GAS
        RMmCustomAPI::TFileType iType;

    };
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

#endif // _CMMPHONEBOOKALPHASTRING_H

// End of File