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

#ifndef CMMPHONEBOOKOPERATIONWRITE_H
#define CMMPHONEBOOKOPERATIONWRITE_H

//  INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>

#include "cmmphonebookstoreoperationbase.h"

//  CONSTANTS
    //none

//  MACROS
    //none

//  DATA TYPES
    //none

//  EXTERNAL DATA STRUCTURES
enum TPBWritePhases
    {
    EPBWritePhase_Read_Entry = 0,
    EPBWritePhase_Read_Ext_Entry,
    EPBWritePhase_Search_Ext_Entry,
    EPBWritePhase_Write_Entry,
    EPBWritePhase_Write_Ext_Entry,
    EPBWritePhase_Read_MBI_profile,
    EPBWritePhase_Write_MBI_profile,
    EPBWritePhase_complete
    };


//  FUNCTION PROTOTYPES
    //none

// CLASS DECLARATION
/**
* CMmPhoneBookOperationWrite is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to write.
*/
class CMmPhoneBookOperationWrite
    : public CMmPhoneBookStoreOperationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmPhoneBookOperationWrite* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            const CMmDataPackage* aDataPackage );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationWrite();

        /**
        * This method created entry point to correct operation
        *
        * @param aPhonebookType PhoneBook type
        * @param aIpc IPC
        * @return Pointer to operation.
        */
        static CMmPhoneBookOperationWrite* Build(
            TName aPhonebookType,
            TInt aIpc );

  private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationWrite();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        /**
        * Separates different IPC requests for each other.
        *
        * @param aIpc Identify number of request.
        * @param aDataPackage Packaged data.
        * @return KErrNone or error value.
        */
        TInt UICCCreateReq(
            TInt aIpc,
            const CMmDataPackage* aDataPackage,
            TUint8 aTransId );

        /**
        * Handles SimPbResp ISI -message
        *
        * @param aIsiMessage
        * @param aComplete Indicates if request can remove from
        *        operationlist or not.
        * @return KErrNone or error value.
        */
        TBool HandleUICCPbRespL(
                TInt aStatus,
                TUint8 aDetails,
                const TDesC8& aFileData,
                TInt aTransId );

        /**
        * Handles HandleWriteReadEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteReadEntryResp(
            TInt aStatus,
            const TDesC8& aFileData,
            TUint8 aTransId,
            TUint16 aFileId,
            TUint16 aFileIdExt,
            TUint8 aArrayIndex );

        /**
        * Handles HandleWriteReadExtEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteReadExtEntryResp(
            TInt aStatus,
            const TDesC8& aFileData,
            TUint8 aTransId,
            TUint16 aFileId,
            TUint16 aFileIdExt);

        /**
        * Handles HandleWriteSearchExtEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteSearchExtEntryResp(
            TInt aStatus,
            const TDesC8& aFileData,
            TUint8 aTransId,
            TUint16 aFileId,
            TUint16 aFileIdExt );
        
        /**
        * Handles HandleWriteEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteEntryResp(
            TInt aStatus,
            TUint8 aTransId,
            TUint16 aFileId,
            TUint16 aFileIdExt );

        /**
        * Handles HandleWriteExtEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteExtEntryResp(
            TInt aStatus,
            TUint8 aTransId,
            TUint16 aFileId,
            TUint16 aFileIdExt);
        
        /**
        * Handles HandleWriteMBIReadResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteMBIReadResp( 
                TInt aStatus,
                TUint8 aDetails,
                TUint8 aTransId,
                const TDesC8 &aFileData ); 
        
        
        /**
        * Handles HandleWriteMBIWriteResp aFileData
        *
        * @param aStatus
        * @param aDetails -
        * @param aTransId - Transaction id received
        * @return complete 
        */
        TInt HandleWriteMBIWriteResp(
                TInt aStatus,
                TUint8 aDetails );

        /**
        * Creates and sends ISI message in order to wite an entry to SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteL(
            TUint8 aTraId,
            TInt16 aIndex,
            CPhoneBookStoreEntry& aDataToWrite );

        /**
        * Creates and sends ISI message in order to wite an entry to SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPBReqWriteEntry(
                TUint16 aFileId,
                TUint8 aIndex,
                TUint8 aTraId,
                CPhoneBookStoreEntry& aDataToWrite );

        /**
        * Creates and sends ISI message in order to wite an ext to SIM
        * @param aFileId File Id
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aFileId 
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteExt(
                TUint8 aTraId,
                TUint8 aIndex,
                TUint16 aFileId,
                TUint16 aFileIdExt,
                CPhoneBookStoreEntry& aDataToWrite );

        /**
        * Creates and sends ISI message in order to read an existing entry Ext record from SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aFileId File id
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteReadExt(
                TUint16 aFileId,
                TUint16 aFileIdExt,
                TUint8 aIndex,
                TUint8 aTraId );

        /**
        * Creates and sends ISI message in order to read an existing entry from SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aFileId File id
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteRead(
                TUint16 aFileId,
                TUint8 aIndex,
                TUint8 aTraId );

        /**
        * Creates and sends ISI message in order to delete EXT file record releted to PB entry
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteExtDelete(
                TUint16 aFileId,
                TUint16 aFileIdExt,
                TUint8 aIndex,
                TUint8 aTransId );
        
        /**
        * Creates and sends ISI message in order to read MBI Profile form first record
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPbReqReadMBI( TUint8 aIndex, TUint8 aTransId );

        
        /**
        * Creates and sends ISI message in order to write MBI Profile form first record
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPBReqWriteMBIProfile( TUint8 aTransId, TUint8 aIndex, TUint8 aOperationType );
    public:     // Data
        // None

    protected:  // Data
        // none

    private:    // Data

        // Attribute to hold the write enty while delete -request is on
        CPhoneBookStoreEntry* iPhoneBookEntry;

        // Keep track on current write -phase
        TPBWritePhases iCurrentWritePhase;
        
        // Array to Store new EXT records
        RArray<TInt> iExtRecordArrayToBeWrite;
        
        // Array to store EXT record nos to be delete
       RArray<TInt> iExtRecordArrayToBeDelete; 
       
       // EXT record number to be read
       TInt iExtRecordNo;
       
       // Store the no of ext records already written
       TUint8 iExtRecordWritten;
       
       // Attribute Store the information if it is location search
       TBool iLocationSearch;
       
       // Attribute to store the information if delete EXT operation going on 
       TBool iExtDeleteOperation;
       
       // For Free Entry Search
       TInt iEntryToWrite;
       
       // To Store Entry to update List
       TPBEntry* iEntryToUpdateInList; 
       
       // to store the MBI operation
       TUint8 iMBIOperation;
};

#endif // CMMPHONEBOOKOPERATIONWRITE_H

// End of file
