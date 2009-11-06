/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for Cell Broadcast tests
*
*/




#ifndef CBS_H
#define CBS_H

//  INCLUDES
#include "core.h"
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <mmmmesshandlerbase.h>
#else
#include <ctsy/rmmcustomapi.h>
#endif




// CONSTANTS
// none

// MACROS
// Logging path
_LIT( KCbsLogPath, "\\logs\\testframework\\Cbs\\" );
// Log file
_LIT( KCbsLogFile, "Cbs.txt" );

_LIT(KIscControlTestCaseFileCbs, "nokiatsy_cbs.xml");

// FORWARD DECLARATIONS
class TDataPackage;
class CMmDataPackage;
class CMessageRouterBase;
class CCore;


// CLASS DECLARATION
class CCbs : public CCore
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCbs* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCbs();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since Series60_ver 2.8
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /**
        * Completes a request or notification via the specific Tsy object,
        * based on IPC number and data package
        * @since Series60_ver 2.8
        * @param aIpc: IPC number
        *        aDataPackage: Data package
        *        aResult: result
        * @return None
        */
        IMPORT_C virtual void RouteCompletion( TInt aIpc,
            CMmDataPackage* aDataPackage, TInt aResult );



    protected:  // Functions from base classes


    private:

        /**
        * C++ default constructor.
        */
        CCbs( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_ver 2.8
        */
        void Delete();
        
        /**
        * Test methods are listed below. 
        */
        
        /**
        * Start CBS routing
        */
        TInt ActivateCbsReceptionL(CStifItemParser& aItem);

        /**
        * Complete Start CBS routing
        */
        void CompleteActivateCbsReception(TInt aErr);

        /**
        * Stop CBS routing
        */
        TInt DeactivateCbsReceptionL(CStifItemParser& aItem);

        /**
        * Complete Stop CBS routing
        */
        void CompleteDeactivateCbsReception(TInt aErr);

        /**
        * Receive a Cell Broadcast message.
        */
        TInt ReceiveCbsL( CStifItemParser& );

        /**
        * Complete Receive a Cell Broadcast message (GSM).
        */
        void CompleteReceiveGsmCbs( TInt aErr, CMmDataPackage& aData );

        /**
        * Complete Receive a Cell Broadcast message (WCDMA).
        */
        void CompleteReceiveWcdmaCbs( TInt aErr, CMmDataPackage& aData );

        /**
        * Read CBS topics (CBMI) from SIM.
        */
        TInt ReadCBSSimTopicsL(CStifItemParser& /*aItem*/);

        /**
        * Complete Read CBS topics (CBMI) from SIM.
        */
        void CompleteReadCBSSimTopics(TInt aErr, CMmDataPackage& aData);

        /**
        * Delete CBS topic (CBMI) from SIM.
        */
        TInt DeleteCBSSimTopicL(CStifItemParser& aItem);

        /**
        * Complete Delete CBS topic (CBMI) from SIM.
        */
        void CompleteDeleteCBSSimTopic(TInt aErr);


    private:    // Data
        CStifLogger* iCbsLog;

        // CBS messages can come in a very fast row
        // ==> need to isolate these requests, otherwise
        // the wrong one might get Signalled.
        TBool iActivateCbsReceptionPending;
        TBool iDeactivateCbsReceptionPending;
        TBool iReceiveCbsPending;

        TBool iReadCBSSimTopicsPending;
        TBool iDeleteCBSSimTopicPending;

        // Reference parameters for parameter checking
        TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize> iCbsMsgGSM;
        TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize> iCbsMsgWCDMA;
        TUint iSbNumber;
        TInt iNumberOfPages;
        TUint8 iMessageType;
        TUint16 iMessageId;
        TUint16 iSerialNum;
        TUint8 iDCS;
        TUint8 iInfoLength;
    };

#endif      // CBS_H

// End of File
