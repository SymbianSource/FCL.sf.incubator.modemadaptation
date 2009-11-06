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

#ifndef _CMMPHONEBOOK_OPERATION_DELETE_H
#define _CMMPHONEBOOK_OPERATION_DELETE_H

// INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>
#include "cmmphonebookstoreoperationbase.h"

// CONSTANTS
    // None

// MACROS
    // None

// DATA TYPES
    // None

// EXTERNAL DATA STRUCTURES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATION
    // None

// CLASS DECLARATION
/**
* CMmPhoneBookOperationDelete is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to delete.
*/
class CMmPhoneBookOperationDelete : public CMmPhoneBookStoreOperationBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return CMmPhoneBookOperationDelete*: created object
        */
        static CMmPhoneBookOperationDelete* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            const CMmDataPackage* aDataPackage );
        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationDelete();

  private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationDelete();

        /**
        * Separates different IPC requests for each other.
        * @param TInt aIpc: Identify number of request.
        * @param const CMmDataPackage* aDataPackage: Packaged data.
        * @return TInt: KErrNone or error value.
        */
        TInt CreateReq( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * Handles SimPbResp ISI -message
        * @param  TIsiReceiveC& aIsiMessage
        * @param TBool& aComplete: Indicates if request can remove from
        *    operationlist or not.
        * @return TInt: KErrNone or error value.
        */
        TInt HandleSimPbRespL(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Handling delete response
        * @param TIsiReceiveC& aIsiMessage
        * @param TBool& aComplete: Indicates if request can remove from
        *    operationlist or not.
        * @return TInt: KErrNone or error value.
        */
        TInt HandleDeleteResp(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Creates and sends ISI message in order to delete an entry from SIM
        * @param TInt16 aIndex: Location index
        * @return TInt: KErrNone or error value.
        */
        TInt SimPbReqDelete( TInt16 aIndex );

        /**
        * Creates and sends ISI message in order to delete all entries from SIM
        * @return TInt: KErrNone or error value.
        */
        TInt SimPbReqDeleteAll();

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        /**
        * Creates and sends ISI message in order to delete alphastring from SIM
        * @return TInt: KErrNone or error value
        */
        TInt SimDeleteAlphaStringReq();

        /**
        * Receives SIM_PB_RESP(SIM_PB_DELETE) ISI messages from phonet receiver
        * @param TIsiReceiveC: reference to the received message.
        * @return TInt: KErrNone or error code
        */
        TInt SimDeleteAlphaStringRespL( const TIsiReceiveC& aIsiMessage );
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

    public: // Data
        // None

    protected: // Data
        // None

    private: // Data
        // None

};

#endif // _CMMPHONEBOOK_OPERATION_DELETE_H

// End of File
