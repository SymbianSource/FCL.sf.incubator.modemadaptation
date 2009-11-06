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
* Description:   Sim session end specific test class.
*
*/



#ifndef SATSIMSESSIONEND_H
#define SATSIMSESSIONEND_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class RSat;

/**
*  Class provides functionality to verify the functionality
*  Sim session end.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatSimSessionEnd ) : public CBase
    {
    
    public: // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CSatSimSessionEnd( const RSat& aSat );
        
        /**
        * Destructor.
        */
        ~CSatSimSessionEnd();
        
        
    public: // New methods

        /**
        * Method requests Notifies Etel about SimSessionEnd
        * @return none
        */
        void NotifySat();
        
        /**
        * Method cancels pending Etel about SimSessionEnd request
        * @return none
        */
        void Cancel();
        
        /**
        * Waits for SessionEnd completion from SimAtkTsy for a short while.
        * @return KErrNone if SessionEnd was received OK, or KErrTimedOut
        */
        TInt WaitSessionEnd();
        
        /**
        * Returns the current status of SessionEnd's timeout-status.
        * @return Returns ETrue if SessionEnd was not received in time for 
        * last message sent to SIM. Otherwise EFalse is returned
        */
        TBool TimedOut();
        
        /**
        * Returns the notification status of SessionEnd
        * @return Returns KRequestPending if SessionEnd has been notified,
        * otherwise zero.
        */
        TInt Status();
                

    private: // Member data
    
        /**
        * Request status of SessionEnd-notification
        */
        TRequestStatus  iSessionEndStatus;

        /**
        * Flag keeps track whether the SessionEnd was received or not
        */
        TBool           iSessionEndCompleted;  
        
        /**
        * Reference to SAT
        */        
        const RSat&     iSat;       
    };

#endif // SATSIMSESSIONEND_H
            
// End of File
