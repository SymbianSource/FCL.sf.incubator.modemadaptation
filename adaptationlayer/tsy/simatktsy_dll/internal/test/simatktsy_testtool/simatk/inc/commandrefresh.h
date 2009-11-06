/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CommandRefresh specific test class. Inherits CCommandBase.
*
*/



#ifndef COMMANDRefresh_H
#define COMMANDRefresh_H

// INCLUDES
#include "satcommandbase.h"



// CLASS DECLARATION

/**
*  Class provides functionality to verify the validity Refresh
*  proactive command and to create an ETel response for it.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CCommandRefresh ) : public CSatCommandBase
    {
    
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CCommandRefresh( const TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser, CStifLogger& aLog );
        
       /**
        * Destructor.
        */
        ~CCommandRefresh();

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
        
    public: // Implemented pure virtual methods from CCommandBase
    
        /**
        * Constructs response packet for Etel from scripted
        * parameters.
        * @return KErrNone or KErrCorrupt if parsing failed or mandatory
        *   parameters were missing.
        */
        TInt SendResponse();
        

    private: // New methods
        
        /**
        * Method verifies the validity of received data by comparing
        * them against scripted parameters.
        * @param aItem reference to Parser class holding parameter data
        * @param aLog Reference to logger
        * @param aData Refresh data to be verified
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        static TInt VerifyRefreshPackage( CStifItemParser& aItem, 
            CStifLogger& aLog, const RSat::TRefreshV2& aData );

        
    private:  // Data
      
        // Proactive command data and package
        RSat::TRefreshV2                    iData;
        RSat::TRefreshV2Pckg                iDataPckg;

        // Response data and package
        RSat::TRefreshRspV1                 iRsp;
        RSat::TRefreshRspV1Pckg             iRspPckg;
    };

#endif      // COMMANDRefresh_H
            
// End of File
