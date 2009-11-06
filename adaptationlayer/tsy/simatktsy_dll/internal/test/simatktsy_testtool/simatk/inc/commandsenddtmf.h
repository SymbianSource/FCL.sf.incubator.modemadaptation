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
* Description:   Send Dtmf specific test class. Inherits CCommandBase.
*
*/



#ifndef COMMANDSENDDTMF_H
#define COMMANDSENDDTMF_H

// INCLUDES
#include "satcommandbase.h"

// CLASS DECLARATION

/**
*  Class provides functionality to verify the validity SendDtmf
*  proactive command and to create an ETel response for it.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CCommandSendDtmf ) : public CSatCommandBase
    {
    public: // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CCommandSendDtmf( TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser, CStifLogger& aLog );
        
        /**
        * Destructor.
        */
        ~CCommandSendDtmf();
        
        
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
        * @param aItem Reference to Parser class holding parameter data
        * @param aLog Reference to logger
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

        
    private: // Data
    
        // Proactive command data and package
#ifndef LEGACY
        RSat::TSendDtmfV5               iData;
        RSat::TSendDtmfV5Pckg           iDataPckg;
#else
        RSat::TSendDtmfV1               iData;
        RSat::TSendDtmfV1Pckg           iDataPckg;
#endif // LEGACY

        
        // Response data and package
        RSat::TSendDataRspV2            iRsp;
        RSat::TSendDataRspV2Pckg        iRspPckg;
    };

#endif // COMMANDSENDDTMF_H
            
// End of File
