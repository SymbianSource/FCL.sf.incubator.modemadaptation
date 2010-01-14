/*
* ==============================================================================
*  Name:           ssm_emergencycallrf_trcommand.h
*  Part of:        SSMA EmergencyCallRfAdaptation
*  Interface:      N/A
*  Description:    Declaration of CEmergencyCallRfTrCommand class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:38:00 2009 %
*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* CEmergencyCallRfTrCommand class declarations
*/

#ifndef SSM_EMERGENCYCALLRF_TRCOMMAND_H
#define SSM_EMERGENCYCALLRF_TRCOMMAND_H

//  INCLUDES
#include <e32base.h>
#include "ssm_emergencycallrf_adaptation.h"

// CONSTANTS

// Constants for iTransactionState
#define EMERGENCYCALLRF_TR_INIT      0x00
#define EMERGENCYCALLRF_TR_ONGOING   0x01
#define EMERGENCYCALLRF_TR_COMPLETED 0x02
#define EMERGENCYCALLRF_TR_FAILED    0x03

// MACROS 

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEmergencyCallRfAdaptation;

// CLASS DECLARATION

/**
*  CEmergencyCallRfTrCommand
*
*  This class declarates SSM Adaptation plug-in module, which provides
*  methods for sending Domestic OS messages.
*/
class CEmergencyCallRfTrCommand : public CBase
    {
    public:
        
        /**
        * Two-phased constructor.
        * @param iAdaptation                Reference to CHapticsAdaptation
        * @param aTransactionId             Unique Transaction Id
        */
        static CEmergencyCallRfTrCommand* NewL( CEmergencyCallRfAdaptation &iAdaptation,
                                                TUint8 aTransactionId );

        /**
        * Destructor.
        */
        virtual ~CEmergencyCallRfTrCommand();

    private:

        /**
        * C++ default constructor.
        */
        CEmergencyCallRfTrCommand( CEmergencyCallRfAdaptation &iAdaptation,
                                   TUint8 aTransactionId );

    public: // New functions

        /**
        * Receive messages from Adaptation
        * @param aCommandId             Identification of current command
        * @param aData                  Data associated with the command.
        * @return none 
        */
        void ProcessMessageL( const TInt aCommandId, 
                              TDesC8& aData );

        /**
        * Returns a current transaction ID
        * @return     A transaction ID
        */
        TUint8 TransactionId() const;

        /**
        * Returns transaction state value
        * @return    A current transaction state
        */
        TUint8 TransactionState() const;
       
    private: // New functions

        /**
        * Creates and sends MCE_RF_STATE_REQ message
        * @param aRequestInfo           Requested state info
        * @return none
        */
        void MCE_RF_STATE_REQL( TUint8 aRequestInfo );

        /**
        * Parses received MCE_RF_STATE_RESP
        * @param aData                  Reference to received MCE_RF_STATE_RESP
        * @return none
        */
        void ParseMCE_RF_STATE_RESPL( TDesC8 &aData );

    private: // Data

        // Reference to Adaptation
        CEmergencyCallRfAdaptation &iAdaptation;

        // Transaction ID
        TUint8 iTransactionId;

        /**
        * A current transaction states
        *  - EMERGENCYCALLRF_TR_INIT
        *  - EMERGENCYCALLRF_TR_ONGOING
        *  - EMERGENCYCALLRF_TR_COMPLETED
        *  - EMERGENCYCALLRF_TR_FAILED
        */
        TUint8 iTransactionState;

    };

#endif // SSM_EMERGENCYCALLRF_TRCOMMAND_H

//End of file
