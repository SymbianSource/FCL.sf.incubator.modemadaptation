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
* Description:   Handles notification of the active object
*
*/



#ifndef SIMATKNOTIFYHANDLER_H
#define SIMATKNOTIFYHANDLER_H

//  INCLUDES
#include "notify.h"

/**
*  Notify class for Test Framework TestScripter.
*  Handles notification of the active object. Signaling when
*  call back return to RunL or cancellation reaches DoCancel
*  function. Notify writes it own log file. 
*  @lib SimAtkNotify.dll
*/
NONSHARABLE_CLASS( CSimAtkNotifyHandler ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSimAtkNotifyHandler* NewL( CSimAtkNotify* aNotify,
            CStifLogger* aNotifyLog );
            
        /**
        * Destructor.
        */
        virtual ~CSimAtkNotifyHandler();
            
            
    public: // New functions
            
        /**
        * Activates this active object with current iStatus
        * @param none
        * @return none
        */      
        void Activate(); 

        /**
        * Stores notify IPC to class member
        * @param aNotifyIpc IPC of the notify
        * @return none
        */    
        void SetNotify( TSatIpc aNotifyIpc );
        
        /**
        * returns this active object status for component 
        * that uses this object
        * @param none
        * @return active object status
        */  
        TRequestStatus& Status();
               

    public: // Functions from base classes
          
        /**
        * Implements cancellation of an outstanding request.
        * @param none
        * @return none
        */
        void DoCancel();        

        
        /**
        * The active scheduler calls this function if this active object's
        * RunL() function leaves. This gives this active object the 
        * opportunity to perform any necessary cleanup.
        * @param aError RunL error code
        * @return Symbian Os Error
        */
        TInt RunError(TInt aError);

        /**
        * CompleteNotification is handled in here
        * @param none
        * @return none
        */
        void RunL();
        
        
    private:

        /**
        * C++ default constructor.
        */
        CSimAtkNotifyHandler();
        
        /**
        * Constructor.
        */ 
        CSimAtkNotifyHandler( CSimAtkNotify* aNotify,
            CStifLogger* aNotifyLog );

        /**
        * Standart ConstructL.
        */
        void ConstructL();


    private:    // Data

        /**
        * Class member holding last async call
        */           
        TSatIpc    		                iNotifyIpc;
       
        /**
        * Pointer to CNotify class
        */   
        CSimAtkNotify*             		iNotify;
        
        /**
        * Pointer to logger
        */   
        CStifLogger*                	iLog;
        
    };
    
#endif // SIMATKNOTIFYHANDLER_H

// End of file
