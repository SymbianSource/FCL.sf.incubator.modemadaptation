/*
* ==============================================================================
*  Name:           ssm_emergencycallrf_adaptation.h
*  Part of:        SSMA EmergencyCallRfAdaptation
*  Interface:      N/A
*  Description:    Declaration of CEmergencyCallRfAdaptation class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:37:57 2009 %
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
* Emergencycallrf_adaptation class declarations.
*
*/

#ifndef SSM_EMERGENCYCALLRF_ADAPTATION_H
#define SSM_EMERGENCYCALLRF_ADAPTATION_H

//INCLUDES
#include <ssm/ssmadaptation.h>
#include <e32debug.h>
#include "msa_isc_service.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEmergencyCallRfTrCommand;
class CSACommonIscService;

// CLASS DECLARATION

/**
 Reference Emergency Call RF adaptation plug-in.
 Implements MEmergencyCallRfAdaptation to Activate and Deactivate RF for emergency calls.

 @internalComponent
 @released

 @see MEmergencyCallRfAdaptation
*/
class CEmergencyCallRfAdaptation : public CBase, public MEmergencyCallRfAdaptation, public MSAIscService
    {
public:
    static CEmergencyCallRfAdaptation* NewL();

    //from MEmergencyCallRfAdaptation
    /*
    * Deletes and frees memory allocated.
    */
    void Release();

    /**
    * Request RF activation so that an emergency call can be made.
    * @param aStatus to complete when the operation has finished
    * @see TRequestStatus 
    */
    void ActivateRfForEmergencyCall( TRequestStatus& aStatus );

    /**
    * Request RF deactivation after an emergency call have been made.
    * @param aStatus to complete when the operation has finished
    * @see TRequestStatus 
    */
    void DeactivateRfForEmergencyCall( TRequestStatus& aStatus );

    /**
    * Cancel the outstanding request.
    */
    void Cancel(); 

private:

    /**
    * C++ constructor.
    */
    CEmergencyCallRfAdaptation();

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

    /**
    * Forwards message to an ongoing transaction.
    * @param aCommandId             Identification of current command
    * @param aTransId               Unique transaction id
    * @param aData                  Data associated with the command.
    * @param aNewTransactionPtr     Pointer to the transaction
    * @return none
    */
    void ForwardMessageL( const TInt aCommandId,
                          const TUint8 aTransId,
                          TDesC8& aData,
                          CEmergencyCallRfTrCommand* aNewTransactionPtr );

    /**
    * Returns new transaction id
    *
    * @since S60 v3.1
    * @return TInt new transaction id
    */
    TInt TransactionId();

    /**
    * Destructor.
    */
    ~CEmergencyCallRfAdaptation();

public: // From MSAIscService

    /**
    * Sends ISI messages
    * @return none
    */
    void SendIsiMessageL( TDesC8& aData );
	
    /**
    * Indicates SSMA Adaptation client about received response message
    * @return none
    */
    void ReceiveIsiMessageL( TDesC8& aData );

	/**
    * Indicates System Services client about received error response message
    * @return none
    */
    void ProcessConnectionClosedL( TInt aErrorCode );

public: /* functions */

    /**
    * Indicates SSMA adaptation client about received response message
    * @param aErrCode               ErrorCode
    * @return none
    */
    void ProcessResponseL( TInt aErrCode );


private: /* Data */

    // Handle for ISC message sending/receiving
    CSACommonIscService* iCSACommonIscServicePtr;

    // Array of Ongoing Transactions
    RPointerArray<CEmergencyCallRfTrCommand> iTransactionPtr;

    // Ongoing command
    TInt iCommandId;

    // Cancel command
    TBool iCancelCommand;

    // Response Status
    TRequestStatus* iStatusPtr;

    // Transaction id
    TInt iTransId;

    };

#endif // SSM_EMERGENCYCALLRF_ADAPTATION_H
