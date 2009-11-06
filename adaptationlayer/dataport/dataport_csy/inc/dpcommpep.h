/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



#ifndef DPCOMMPEP_H
#define DPCOMMPEP_H

//  INCLUDES
#include <e32base.h>

// LOCAL CONSTANTS
// none

// DATA TYPES
// none

// FORWARD DECLARATIONS
class CDpDataPort;
class CDpDataConfig;
class CDpBreak;
class CDpFlowCtrl;

// CLASS DECLARATION

/**
*  CDpCommPEP is COMM PEP of DataPort.
*  CDpCommPep handles COMMON and COMM type status indications and COMM type
*  control requests from DCS. DCS uses the only COMMON type status indication
*  to indicate about DP->DCS flow control change. DCS can use COMM type control
*  requests to configure DTE interface. DCS can use COMM type status indications
*  to indicate about configuration changes, state changes and break signal.
*/
class CDpCommPep : public CBase
    {

    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aDataPort: Dataport main class
        */
        CDpCommPep( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpCommPep();

    public:  // New methods

        /**
        * This method handles PEP control requests, which are received from DCS.
        * @param aCtrlId: PEP Control ID
        * @param aCtrlData: PEP Control data, which follows control ID in the
        * message
        * @return TInt: Error code
        */
        TInt HandlePepCtrlReq( const TUint8 aCtrlId, TDesC8& aCtrlData );

        /**
        * This method handles COMM type PEP status indications, which are
        * received from DCS.
        * @param aIndId: PEP Indication ID
        * @param aIndData: PEP Indication data
        * @return TInt: Error code
        */
        TInt HandlePepStatusInd( const TUint8 aIndId, TDesC8& aIndData );

    private:  // Data

        // DataPort
        CDpDataPort& iDataPort;

        // Port data configuration
        CDpDataConfig& iDataConfig;

        // Break signal handler
        CDpBreak& iBreak;

        // Flow control handler
        CDpFlowCtrl& iFlowCtrl;

    };

#endif      // DPCOMMPEP_H

//  End of File
