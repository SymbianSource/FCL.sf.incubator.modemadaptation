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
* Description:   Call Control specific test class. Inherits CSatBase.
*
*/



#ifndef SATCALLCONTROL_H
#define SATCALLCONTROL_H

// INCLUDES
#include "satbase.h"

// CLASS DECLARATION

/**
*  Class provides functionality to verify the functionality
*  Call Control.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatCallControl ) : public CSatBase
    {
    
    public: // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CSatCallControl( const TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser, CStifLogger& aLog );
        
        /**
        * Destructor.
        */
        ~CSatCallControl();
        
        
    public: // Implemented pure virtual methods from CSatBase

        /**
        * Method requests call control notifications from Etel and
        * and resets the internal data structure.
        * @param aSimAtkStatus Reference to active object's request status
        * @return KErrNone
        */
        TInt NotifySat( TRequestStatus& aSimAtkStatus );
        
        /**
        * Method verifies the validity of received data by comparing
        * them against scripted parameters.
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        TInt CheckData();

    
    private: // Methods
    
        /**
        * Method verifies the validity of received call data by comparing
        * them against scripted parameters.
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        TInt CheckCallParams();
    
        /**
        * Method verifies the validity of received SS data by comparing
        * them against scripted parameters.
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        TInt CheckSsParams();
    
        /**
        * Method verifies the validity of received USSD data by comparing
        * them against scripted parameters.
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        TInt CheckUssdParams();
    
        /**
        * Method verifies the validity of received PDP data by comparing
        * them against scripted parameters.
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        TInt CheckPdpParams();
    
    
    private: // Data
    
    
        // Call control data and package
#ifndef LEGACY
        RSat::TCallControlV5        iData;
        RSat::TCallControlV5Pckg    iDataPckg;
#else
        RSat::TCallControlV2        iData;
        RSat::TCallControlV2Pckg    iDataPckg;
#endif // LEGACY

        
    private: // Friends

        // Declare Controllable interface to friend to allow access to iData.
        friend class MSatControllable;
       
    };

#endif // SATCALLCONTROL_H
            
// End of File
