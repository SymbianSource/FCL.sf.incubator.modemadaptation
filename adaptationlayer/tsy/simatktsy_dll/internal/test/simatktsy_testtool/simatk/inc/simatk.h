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
* Description:   Specific class for SIM ATK TSY tests
*
*/




#ifndef SIMATK_H
#define SIMATK_H

//  INCLUDES
#include "core.h"
#include "satsimsessionend.h"

#include <etelsat.h>
#include <risctestcontrol.h>

// FORWARD DECLARATIONS
class CSatBase;


// CLASS DECLARATION

/**
*  CSimAtk test class for STIF Test Framework TestScripter.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSimAtk ) : public CSimAtkCore
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSimAtk* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CSimAtk();
        

	public: // New functions
        
        /**
        * Creates proper test class based on the received IPC and notifies SAT
        * @param aReqStatus async call status
        * @param aNotifyIpc IPC of notify to be notified
        * @return:Symbian OS error Code
        */
        virtual TInt NotifySimAtkL( TRequestStatus& aReqStatus,
            const TSatIpc& aNotifyIpc );
        
        /**
        * Cancels Notify
        * @param aSatIpc IPC of the notify to be cancelled
        * @return:Symbian OS error Code
        */                 
        virtual TInt NotifySimAtkCancel( const TSatIpc& aNotify );
        

    public: // Functions from base classes
    
        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt RunMethodL( CStifItemParser& aItem );

      
    private:

        /**
        * C++ default constructor.
        */
        CSimAtk( CTestModuleIf& aTestModuleIf );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        */
        void Delete();

        /**
        * Open function for Sim Atk specific services. If parameter is 
        * unidentified, base classes Open-method is called.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt Open( CStifItemParser& aItem );
        
        /**
        * Specific Close for SIM class.
        * If not found Core close called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt Close( CStifItemParser& aItem );

        /**
        * Loads XML file which contains ISC simulating test cases
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt LoadIscTests( CStifItemParser& aItem );
        
        /**
        * Runs a test case from ISC XML file. Usually this is used for
        * simulating ISI messages.
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt RunIscTestCase( CStifItemParser& aItem );
        
        /**
        * Syncs a test case from ISC XML file. 
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt SyncIscTestCase( CStifItemParser& aItem );
        
        /**
        * Cancel on-going test case from test client. 
        * If not found Core open called
        * @param aReason Symbien error code that cased the cancellation
        * @return Symbian OS error code
        */
        TInt CancelTestCase( const TInt aReason );
        
        /**
        * Cancel on-going test case from ISC XML file. 
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt CancelTestCase( CStifItemParser& aItem );
        
        /**
        * Analyzes a content of Etel packet
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt CheckData( CStifItemParser& aItem );
        
        /**
        * Sends a data packet to the ETel. Packet can be for example a
        * terminal response or an envelope.
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt SendResponse( CStifItemParser& aItem );
        
        /**
        * Verifies the functionality of icon loading procedure
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        TInt CheckIcon( CStifItemParser& aItem );

        /**
        * Halts the execution until an async request is completed. Once request
        * has been completed, command array is cleared and Sat notifications 
        * cancelled.
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code or Error code from ISC controller
        */
        TInt WaitForRequest( CStifItemParser& aItem  );
        
        /**
        * Delay
        * If not found Core open called
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code or Error code from ISC controller
        */
        TInt Delay( CStifItemParser& aItem  );
        
        /**
        * Finds the index of requested item in notify array based on
        * the string provided by a test script
        * @param aItem Script line containing requested sat item.
        * @param aIndex Index of requested command in array if found
        * @return Symbian OS error code KErrNone or KErrNotFound
        */
        TInt FindIndex( CStifItemParser& aItem, TInt& aIndex);
        
        /**
        * Finds the index of requested item in notify array based on
        * the ipc of item.
        * @param aRequestedIpc Ipc of requested sat item.
        * @param aIndex Index of requested command in array if found
        * @return Symbian OS error code KErrNone or KErrNotFound
        */
        TInt FindIndex( const TSatIpc& aRequestedIpc, TInt& aIndex);
        
        /**
        * Helper function that maps an error code to corresponding string
        * presentation
        * @return Descriptor to a string presentatation of error
        */
        const TPtrC MapErrorCodeToString( const TInt& aErrorCode );
        
        
    private:    // Data
            
        /**
        * ISC control async call status
        */
        TRequestStatus                    	iIscStatus;
        
        /**
        * Used for simulating ISI messages
        */
        RIscTestControl                     iRIscTestControl;
      
        /**
        * RSat object
        */
        RSat                                iSat;
        
        /**
        * RMobileSmsMessaging object
        */
        RMobileSmsMessaging                 iSmsMessaging;
        
        /**
        * Proactive commands to be tested
        */
        CArrayPtrFlat<CSatBase>*            iArray;
        
        /**        
        * SimSession end class
        */   
        CSatSimSessionEnd                   iSessionEnd;

        /**
        * Flag to keep track wether the SessionEnd is required or not
        */
        TBool                               iNeedSessionEnd;
        
        /**
        * Pointer to stif parser
        */
        CStifItemParser*                    iParser;
        
        /**
        * Pointer of CallControl class
        */
        CSatBase*                           iCallControl;
    };

#endif      // SIMATK_H
            
// End of File
