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
* Description:   Specific class for SIM ATK TSY tests. Inherits CCommandBase.
*
*/



#ifndef COMMANDDISPLAYTEXT_H
#define COMMANDDISPLAYTEXT_H

// INCLUDES
#include "satcommandbase.h"

// CLASS DECLARATION

/**
*  Class provides functionality to verify the validity DisplayText
*  proactive command and to create an ETel response for it.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CCommandDisplayText ) : public CSatCommandBase
    {
    
    public: // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CCommandDisplayText( const TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser, CStifLogger& aLog );
        
        /**
        * Destructor.
        */
        ~CCommandDisplayText();
        

    public: // Implemented pure virtual methods from CSatBase

        /**
        * Method requests notifications from Etel and
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

        
    public: // Inherit pure virtual methods from CCommandBase
    
        /**
        * Constructs response packet for Etel from scripted
        * parameters.
        * @return KErrNone or KErrCorrupt if parsing failed or mandatory
        *   parameters were missing.
        */
        TInt SendResponse();


    private: // Data
      
        // Proactive command data and package
        RSat::TDisplayTextV2            iData;
        RSat::TDisplayTextV2Pckg        iDataPckg;

        // Response data and package
        RSat::TDisplayTextRspV1         iRsp;
        RSat::TDisplayTextRspV1Pckg     iRspPckg;
    };

#endif // COMMANDDISPLAYTEXT_H
            
// End of File
