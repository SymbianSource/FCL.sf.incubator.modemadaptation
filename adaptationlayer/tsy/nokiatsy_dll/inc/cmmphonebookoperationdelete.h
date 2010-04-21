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

#ifndef CMMPHONEBOOKOPERATIONDELETE_H
#define CMMPHONEBOOKOPERATIONDELETE_H

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
enum TPBDeletePhases
    {
    EPBDeletePhaseReadExtensions,
    EPBDeletePhaseReadEntry,
    EPBDeletePhaseDeleteExtension,
    EPBDeletePhase_Read_MBI_profile,
    EPBDeletePhase_delete_MBI_profile,
    EPBDeletePhaseDeleteEntry
    };


//  FUNCTION PROTOTYPES
    //none

// CLASS DECLARATION
/**
* CMmPhoneBookOperationWrite is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to write.
*/
class CMmPhoneBookOperationDelete
    : public CMmPhoneBookStoreOperationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmPhoneBookOperationDelete* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aUiccMessHandler,
            const CMmDataPackage* aDataPackage );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationDelete();

        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationDelete();

        /**
        * This method creates entry point to correct operation
        * @param aPhonebookType PhoneBook type
        * @param aIpc IPC
        * @return Pointer to operation.
        */
        static CMmPhoneBookOperationDelete* Build(
            TName aPhonebookType,
            TInt aIpc );

    protected:
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
        * Handles UICC response ISI -message
        * @param aStatus Status
        * @param aFileData File data
        * @param aTransId Transaction ID
        * @return Is operation going to be deleted
        */
        TBool HandleUICCPbRespL(
            TInt aStatus,
            TUint8 aDetails,
            const TDesC8& aFileData,
            TInt aTransId );

    private:

        /**
        * Handles request to delete a phonebook entry
        * @return KErrNone or error value
        */
        TInt UiccPbReqDelete();

        /**
        * Creates and sends ISI message in order to delete an entry
        * @return KErrNone or error value
        */
        TInt UiccPbReqDeleteEntry();

        /**
        * Creates and sends ISI message in order to delete an extension
        * @param aExtRecordNum Extension record number
        * @return KErrNone or error value
        */
        TInt UiccPbReqDeleteExt( TInt aExtRecordNum );

        /**
        * Creates and sends ISI message in order to read an entry
        * @return KErrNone or error value
        */
        TInt UiccPbReqReadEntry();

        /**
        * Handles phonebook entry data
        * @param aFileData File data
        * @return KErrNone or error value
        */
        TInt HandleReadEntryResp( const TDesC8& aFileData );

        /**
        * Handles Delete for phoenBook Entry
        * @param aFileData File data
        * @return KErrNone or error value
        */
        TInt HandleDeleteEntryResp(
            TBool &aComplete,
            TInt &aLocation );

        /**
        * Handles Deelte for phoenBook Entry Ext Data
        * @param aFileData File data
        * @return KErrNone or error value
        */
        TInt HandleDeleteExtResp();

        /**
        * Creates and sends ISI message in order to read an extension
        * @return KErrNone or error value
        */
        TInt UiccPbReqReadExt( TUint8 aExtRecordNum );

        /**
        * Handles phonebook extension data
        * @param aFileData File data
        * @return KErrNone or error value
        */
        TInt HandleReadExtResp( const TDesC8& aFileData );
        
        /**
        * Creates request to read MBI record
        * @return KErrNone or error value
        */
        TInt UiccPbReqReadMBI();
        
        /**
        * Handles phonebook extension data
        * @param aStatus
        * @param aDetails
        * @param aFileData File data
        * @return KErrNone or error value
        */
        TInt HandleMBIReadResp(TInt aStatus, TUint8 aDetails, TBool &aComplete, const TDesC8& aFileData );
        
        /**
        * Creates request to Delete MBI profile
        * @return KErrNone or error value
        */
        TInt UiccPBReqDeleteMBIProfile();

        
    public:     // Data
        // None

    protected:  // Data
        // Phone book entry to be deleted
        CPhoneBookStoreEntry* iPhoneBookEntry;

        // IPC number of operation
        TInt iIpc;

    private:    // Data

       // Keep track on current delete phase
       TPBDeletePhases iCurrentDeletePhase;


       // Number of entries
       TInt iNumOfEntries;

       // Number of extension records in entry
       TInt iNumOfExtensions;

       // Elementary file ID
       TUint16 iFileId;

       // Extension file ID
       TUint16 iFileIdExt;

       // Record number in phonebook to be deleted
       TInt iIndex;

       // Phonebook entry
       TPBEntry iEntry;

       // Transaction ID used in delete operations
       TUint8 iTransId;

       // Location index of phonebook type
       TUint8 iArrayIndex;

       // Information of location already read
       TBool iLocationFoundInPbList;
       
       // Array to store ext rexords to be deletet
       RArray<TInt> iExtRecordArrayToBeDelete;
       
       // Attribute to store MBI Profiletype
       TUint8 iMBIProfileType;
};

#endif // CMMPHONEBOOKOPERATIONDELETE_H

// End of file
