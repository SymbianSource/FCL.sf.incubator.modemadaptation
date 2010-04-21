/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

        /**
        * Read application file info from SIM/USIM
        * @param aFileId
        * @param aRecordId
        * @param aTrId Transaction ID:
        * @return Error code
        */
        TInt UiccReadApplFileInfo( const TInt aFileId,
                                   const TInt aRecordId,
                                   const TUint8 aTrId );

        /**
        * Handles Recieved data
        * @param aTransId Transaction ID:
        * @param TInt aStatus : To get the UICC Server response status
        * @param const TDesC8 &aFileData : Data received in ISI Message
        * @retun EFalse/ETrue to notify is operation completed or not
        */
        TBool HandleUiccReadApplFileInfoResp(
            TInt aTransId,
            TInt aStatus,
            const TDesC8 &aFileData );

        /**
        * Checks is MSISDN service available.
        * @return value KErrNone or KErrNotSupported
        */
        TInt CheckMSISDNSupport();

        /**
        * Completes given IPC with data package
        */
        void CompleteThisIPC
            (
            TInt aIPCToBeCompleted,
            CMmDataPackage* aDataPackage,
            TInt ret
            );

        /**
        * Read application file data from SIM/USIM
        * @param aFileId
        * @param aRecordId
        * @param aTrId Transaction ID:
        * @return Error code
        */
        TInt UiccReadApplFileData
        	   (
	           const TInt aFileId,
             const TInt aRecordId,
             const TUint8 aTrId
             );

    public: // Data
        // None

    protected: // Data
        // Store information about operation has been canceled or not
        TBool iCancelOperation;

        // Record ID
        TInt iRecordId;

        // Saved IPC
        TInt iSavedIPC;

        // Attribute to check what kind of read is ongoing
        TTypeOfFileToBeRead iTypeOfReading;
       
        // Attribute to store file id
        TUint16 iFileId;
        
        // Attribute to store Ext FileID
        TUint16 iExtFileId;
        
        // Attribute to store ArrayIndex
        TUint8 iArrayIndex;

        // Service type for EMmTsyONStoreGetInfoIPC
        TServiceType iServiceType;

        // Attribute to Store Entry
        TPBEntry* iStoreEntry;

        // To Store the information about no of entries filled in commontsy araay
        TInt iNumOfEntriesFilled;

        // Length of the EXT file
        TInt iEXTNumberLen;

        // Flag to store EXT needs to be read or not
        TBool iExtensionToRead;
    private: // Data

        // Number of used SDN entries
        TInt iNumOfUsedSDNEntries;

        // Number of used VMBX entries
        TInt iNumOfUsedVMBXEntries;
        
        

    };
#endif // _CMMPHONEBOOK_OPERATION_CACHE_H

// End of File
