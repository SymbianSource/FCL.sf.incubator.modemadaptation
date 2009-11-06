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
* Description:   MO SMS Control specific test class. Inherits CSatBase.
*
*/



#ifndef SATMOSMSCONTROL_H
#define SATMOSMSCONTROL_H

// INCLUDES
#include "satbase.h"

// CLASS DECLARATION

/**
*  Class provides functionality to verify the functionality
*  Call Control.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatMoSmsControl ) : public CSatBase
    {
    
    public: // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CSatMoSmsControl( const TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser, CStifLogger& aLog );
        
        /**
        * Destructor.
        */
        ~CSatMoSmsControl();
        
        
    public: // Implemented pure virtual methods from CSatBase

        /**
        * Method requests call control notifications from Etel and
        * and resets the internal data structure.
        * @param aSimAtkStatus Reference to active object's request status
        * @return KErrNone
        */
        TInt NotifySat( TRequestStatus& aSimAtkStatus );
        
        /**
        * Method verifies the validity of received data by compaing
        * them against scripted parameters.
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        TInt CheckData();

        
    private: // Data
    
        // MO SMS control data and package
        RSat::TMoSmControlV1        iData;
        RSat::TMoSmControlV1Pckg    iDataPckg;
    };

#endif // SATMOSMSCONTROL_H
            
// End of File
