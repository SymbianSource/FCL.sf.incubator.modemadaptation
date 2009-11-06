/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _CMMPHONEBOOK_OPERATION_CUSTOM_H
#define _CMMPHONEBOOK_OPERATION_CUSTOM_H

// INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmphonebookstoreoperationbase.h"
#include <e32base.h>

// CONSTANTS
    //None

// MACROS
    //None

// DATA TYPES
    //None

// EXTERNAL DATA STRUCTURES
    //None

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
class CAlphaString;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

// CLASS DECLARATION

/**
* CMmPhonebookOperationCustom is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to custom requests.
*/
class CMmPhonebookOperationCustom
    : public CMmPhoneBookStoreOperationBase
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        CMmPhonebookOperationCustom(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler );

        /**
        * Destructor.
        */
        ~CMmPhonebookOperationCustom();

    private:

        /**
        * Separates different IPC requests for each other.
        * @param TInt aIpc: Identify number of request.
        * @param const CMmDataPackage* aDataPackage: Packaged data.
        * @return TInt: KErrNone or error value.
        */
        TInt CreateReq( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * Handles SimPbResp ISI -message
        * @param TIsiReceiveC& aIsiMessage
        * @param TBool& aComplete: Indicates if request can remove from
        * operationlist or not.
        * @return TInt: KErrNone or error value.
        */
        TInt HandleSimPbRespL( const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Construct a SIM_READ_FIELD_REQ message and send it through phonet.
        * @param TUint8 aTransId: Transaction Id
        * @return TInt: KErrNone / Error value from phonet
        */
        TInt SimReadFieldReq( TUint8 aTransId );

        /**
        * Breaks received SIM_READ_FIELD_RESP ISI message
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @param TBool& aComplete Indicates if request can remove from
        * operationlist or not.
        * @return TInt: KErrNone or error value.
        */
        TInt SimReadFieldRespL( const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

    public: // Data
        //None
    protected: // Data
        //None
    private: // Data
        //None
    };

#endif // _CMMPHONEBOOK_OPERATION_CUSTOM_H

// End of File
