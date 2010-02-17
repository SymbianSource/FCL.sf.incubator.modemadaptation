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

#ifndef _CMMPHONEBOOK_OPERATION_CACHE_H
#define _CMMPHONEBOOK_OPERATION_CACHE_H


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
* CMmPhoneBookOperationCache is used to create and send GSM-specific PBStore
* ISI messages to PhoNet via PhoNetSender relating to cache.
*/
class CMmPhoneBookOperationCache : public CMmPhoneBookStoreOperationBase
    {
    public:

        /**
        * Two-phased constructor.
        * @param CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler:
        *   Pointer to CMmPhoneBookStoreMessHandler object.
        * @param aIpc IPC number
        * @param const CMmDataPackage* aDataPackage: Packaged data.
        * @return CMmPhoneBookOperationInit*: created object
        */
        static CMmPhoneBookOperationCache* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aUiccMessHandler,
            TInt aIpc,
            const CMmDataPackage* aDataPackage );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationCache();

    protected:
        
        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationCache();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        /**
        * Separates different IPC requests for each other.
        * @param TInt aIpc: Identify number of request.
        * @param const CMmDataPackage* aDataPackage: Packaged data.
        * @return TInt: KErrNone or error value.
        */
        TInt UICCCreateReq( TInt aIpc, const CMmDataPackage* aDataPackage, TUint8 aTransId );

        /**
        * Handles Recieved data
        * @param TBool& aComplete : flag to check operation needs to be removed form the list or not.
        * @param TInt aStatus : To get the UICC Server response status
        * @param const TDeC9 &aFileData : Data received in ISI Message
        * @param TInt aTransId : transaction Id sent in request message
        * @return TInt: KErrNone or error value.
        */
        TBool HandleUICCPbRespL ( TInt aStatus, TUint8 aDetails, const TDesC8 &aFileData, TInt aTraId);
        
        
        /**
        * Creates message in order to read entry from SIM
        * @param aRecordNo entry to be read
        * @return TInt : KErrNone or error value
        */
        TInt USimPbReqRead( TInt aRecordNo, TUint8 aTransId );

        /**
        * Cancels cacheing request
        */
        void CancelReq( TName& aPhoneBook );

    public: // Data
        // None

    protected: // Data
        // Store information about operation has been canceled or not
        TBool iCancelOperation;

    private: // Data

        // Number of used SDN entries
        TInt iNumOfUsedSDNEntries;

        // Number of used VMBX entries
        TInt iNumOfUsedVMBXEntries;
        
        // Flag to store EXT needs to be read or not
        TBool iExtensionToRead;
        
        // To Store the information about no of entries filled in commontsy araay
        TInt iNumOfEntriesFilled;

        // Attribute to Store Entry
        TPBEntry* iStoreEntry;


#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        // Flag keeps track if GAS is cashed or not
        TBool iGASCacheIsReady;

        // Flag keeps track if AAS is cashed or not
        TBool iAASCacheIsReady;

#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

    };
#endif // _CMMPHONEBOOK_OPERATION_CACHE_H

// End of File
