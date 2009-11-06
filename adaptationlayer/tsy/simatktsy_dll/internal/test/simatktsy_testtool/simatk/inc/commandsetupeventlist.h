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
* Description:  Set Up Event List specific test class. Inherits 
*               CCommandBase.
*
*/



#ifndef COMMANDSETUPEVENTLIST_H
#define COMMANDSETUPEVENTLIST_H

// INCLUDES
#include "satcommandbase.h"

// CLASS DECLARATION

/**
*  Class provides functionality to verify the validity of Set Up
*  Event List proactive command and to create an ETel response for it.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CCommandSetUpEventList ) : public CSatCommandBase
    {
    public: // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CCommandSetUpEventList( TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser, CStifLogger& aLog );
        
        /**
        * Destructor.
        */
        ~CCommandSetUpEventList();
        
        
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
        * @param aItem reference to Parser class holding parameter data
        * @return KErrNone or KErrCorrupt if parsing failed or mandatory
        *   parameters were missing.
        */
        TInt SendResponse();
    
    private: // New methods
        
        /**
        * Constructs response packet for Etel from scripted
        * parameters.
        * @param aSingleEvent occured Client originated event
        * @param aEventInfo Packaged event class 
        * @return completion status of a request made to ME side 
        */
          
        TInt SendEvent( RSat::TEventList aSingleEvent, const TDesC8& aEventInfo );

        
    private: // Data
    
        // Proactive command data and package
        RSat::TSetUpEventListV1         iData;
        RSat::TSetUpEventListV1Pckg     iDataPckg;
        
        // Response data and package
        RSat::TSetUpEventListRspV1      iRsp;
        RSat::TSetUpEventListRspV1Pckg  iRspPckg;
    };

#endif // COMMANDSETUPEVENTLIST_H
            
// End of File
