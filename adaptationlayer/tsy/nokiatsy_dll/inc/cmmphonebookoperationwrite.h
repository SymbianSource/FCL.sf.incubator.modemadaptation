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
    EPBWritePhase_Read_Entry = 1,
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
            CMmUiccMessHandler* aUiccMessHandler,
            const CMmDataPackage* aDataPackage,
            TInt aIpc);

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
        
        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationWrite();
        
    protected:
        
        /**
        * Creates and sends ISI message in order to read MBI Profile form first 
        * record
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPbReqReadMBI( TUint8 aOffset, TUint8 aTransId );

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
        * Copy data from one buffer to othe
        * @param &aSource : source buffer
        * @param &aTraget : target buffer
        */
        static void CopyData( const TDesC16& aSource, TDes16& aTarget );



  private:


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
        * Handles HandleWriteReadEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration
        * data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteReadEntryResp(
            TInt aStatus,
            const TDesC8& aFileData,
            TUint8 aTransId );

        /**
        * Handles HandleWriteReadExtEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration
        *  data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteReadExtEntryResp(
            TInt aStatus,
            const TDesC8& aFileData,
            TUint8 aTransId );
        
        /**
        * Handles HandleWriteSearchExtEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration
        *  data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteSearchExtEntryResp(
            TInt aStatus,
            const TDesC8& aFileData,
            TUint8 aTransId );
        
        /**
        * Handles HandleWriteEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration
        *  data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteEntryResp(
            TInt aStatus,
            TUint8 aTransId );

        /**
        * Handles HandleWriteExtEntryResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration
        *  data Stored
        * @param aStatus - Response message status
        * @return complete 
        */
        TInt HandleWriteExtEntryResp(
            TInt aStatus,
            TUint8 aTransId );
        
        /**
        * Handles HandleWriteMBIReadResp aFileData
        *
        * @param aFileData
        * @param aTransId - Transaction id received
        * @param aFileId - file id for main PB file
        * @param aFileIdExt - file Id for EXT fiel related to main PB file
        * @ param aArrayIndex - Array index for the Array in which configuration
        *  data Stored
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
        * Creates request to Writy Msisdn Main Entry
        *
        * @param aTransId - Transaction id received
        * @param RMobileONStore::TMobileONEntryV1 aEntry - Entry to be written
        * @return complete 
        */
        TInt UiccPbReqWriteMsisdn( 
                     TUint8 aTransId, 
                     RMobileONStore::TMobileONEntryV1 aEntry );

        /**
        * Creates and sends ISI message in order to wite an entry to SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPBReqWriteEntry(
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
                CPhoneBookStoreEntry& aDataToWrite );

        /**
        * Creates and sends ISI message in order to read an existing entry Ext 
        * record from SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aFileId File id
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteReadExt(
                TUint8 aIndex,
                TUint8 aTraId );

        /**
        * Creates and sends ISI message in order to read an existing entry from
        * SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aFileId File id
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteRead(
                TUint8 aIndex,
                TUint8 aTraId );

        /**
        * Starts Write Entry When Entry is present in internal List
        * SIM
        *
        * @param TPBEntry &aEntry : Internal List Entry
        * @param TUint8 aExtRecNo : Total number of Ext records 
        * for new Entry to write
        * @param TUint8 aTransId : Transaction id
        * @param CPhoneBookStoreEntry& aDataToWrite : New Entry data
        * @param TUint16 aPbFileId : PhoneBook FileId
        * @param TUint16 aPbExtFileId : Phonebook Ext File id
        * @return KErrNone or error value
        */
        TInt StartWriteEntryIndex( 
                TUint16 aIndex,
                TUint8 aExtRecNo, 
                TUint8 aTransId, 
                CPhoneBookStoreEntry& aDataToWrite );
        
        /**
        * Creates and sends ISI message in order to delete EXT file record
        * releted to PB entry
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPbReqWriteExtDelete(
                TUint8 aIndex,
                TUint8 aTransId );
        
        
        /**
        * Creates and sends ISI message in order to write MBI Profile form
        * first record
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt UiccPBReqWriteMBIProfile( 
                        TUint8 aTransId,
                        TUint8 aIndex,
                        TUint8 aOperationType );
        
        /**
        * Get the Ext record number to write the new Entry
        * first record
        * @param &aExtRecordNo : Number of EXT records
        */
        void GetExtRecNum( TUint8 &aExtRecordNo );

        /**
        * Check for Ext record numbers for new Entry are sufficient or not
        * @param &aExtRecordNo : Number of EXT records
        * @param entry : Entry present in internal list
        * @param aTransId : transaction id
        */
        TInt CheckForExtRecordNum( TPBEntry entry, 
                                   TUint8 aTransId );

        /**
        * Continue with writing new Entry
        * @param &aExtRecordNo : Number of EXT records
        * @param entry : Entry present in internal list
        * @param aTransId : transaction id
        */
        TInt ContinueWriteEntry( 
                TUint8 aExtRecNo, 
                TPBEntry entry, 
                TUint8 aTransId,
                CPhoneBookStoreEntry& aDataToWrite );

    public:     // Data
        // None

    protected:  // Data
        // Attribute to hold the write enty while delete -request is on
        CPhoneBookStoreEntry* iPhoneBookEntry;

        // Keep track on current write -phase
        TPBWritePhases iCurrentWritePhase;

        // Attribute Store the information if it is location search
        TBool iLocationSearch;
       
       // Store the no of ext records already written
       TUint8 iExtRecordWritten;
       
       // attribute to store the recieved ipc
       TInt iSavedIpc;
        
       // attribute to store file id
       TUint16 iFileId;
       
       // Attribute to store Ext File id
       TUint16 iExtFileId;
       
       // attribute to store arry index
       TUint8 iArrayIndex;
       
    private:    // Data


        // Array to Store new EXT records
        RArray<TInt> iExtRecordArrayToBeWrite;

        // Array to store EXT record nos to be delete
       RArray<TInt> iExtRecordArrayToBeDelete; 

       // EXT record number to be read
       TInt iExtRecordNo;
       
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
