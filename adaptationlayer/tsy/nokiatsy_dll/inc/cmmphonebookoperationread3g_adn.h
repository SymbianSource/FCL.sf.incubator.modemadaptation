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

#ifndef _CMMPHONEBOOK_OPERATION_READ_3G_ADN_H
#define _CMMPHONEBOOK_OPERATION_READ_3G_ADN_H

// INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>
#include "cmmphonebookoperationread.h"
#include "muiccoperationbase.h"

// CONSTANTS
    // None

// MACROS
    // None

// DATA TYPES
    // None

// EXTERNAL DATA STRUCTURES

enum TPBAdn3GReadPhases
    {
    EPBReadPhase_PBR_Read_Entry = 0,
    EPBReadPhase_Read_Type1_Entry,
    EPBReadPhase_Read_Type2_Entry,
    EPBReadPhase_Read_type3_Entry,
    EPB_3G_ADN_Read_Phase_complete
    };

// FUNCTION PROTOTYPES
    // None

// CLASS DECLARATION
/**
* CMmPhoneBookOperationInit3G_ADN is used to create and send USIM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to read.
*/
class CMmPhoneBookOperationRead3g_adn : public CMmPhoneBookOperationRead
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return CMmPhoneBookOperationRead3g_adn*: created object
        */
        static CMmPhoneBookOperationRead3g_adn* NewL
               (
                CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
                CMmUiccMessHandler* aUiccMessHandler,
                const CMmDataPackage* aDataPackage, // Data
                TInt aIpc
               );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationRead3g_adn();

    protected:
        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationRead3g_adn();


  private:

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();
        
        // Transmit 

        /**
        * Check for valid ipc and PhoneBook
        * @param aIpc : Ipc
        * @param aDataPackage: Data received from Commontsy
        * @param aTransId : Transaction id
        * @return TInt: KErrNone or error value.
        */
        TInt UICCCreateReq
            (
            TInt aIpc,
            const CMmDataPackage* aDataPackage,
            TUint8 aTransId
            );

        /**
        * Constructs Data to read entry from USIM ADN Phonebook
        * @param
        * @return TInt: KErrNone or error value.
        */
        TInt UICCHandleData3gADNReadReq
             (            
             const CMmDataPackage* aDataPackage,
             TUint8 aTransId
             );
        
        
        /**
        * Creates request to read record
        * @param aTraId : Transaction Id
        * @param aFileId : FielId
        * @param aFileidSfi : aFileIdSfi
        * @param aRecordNo : record number to be read
        * @return TInt: KErrNone or error value.
        */
        TInt UiccReadEfRecordReq( 
            const TInt aTraId, 
            const TUint16 aFileId, 
            const TUint8 aFileIdSfi,
            const TUint8 aRecordNo );

        
        // Receive

        
        TBool HandleUICCPbRespL
            (
            TInt aStatus,
            TUint8 aDetails,
            const TDesC8 &aFileData,
            TInt aTransId
            );

        /**
        * Handles SimPbResp ISI -message
        * @param  TInt aTagValue
        * @param  TDes8& aFileData
        * @return TInt: KErrNone or error value.
        */
        TBool HandleUICC3gADNRespL(const TInt aStatus, 
                                  const TDesC8& aFileData,
                                  const TInt aTransId);
        
        /**
        * Handles response for PBR record read
        * @param  TInt aStatus : UICC Server status
        * @param  TDesC8& aFileData : Response Data
        * @param TInt aTraId : transaction id
        * @return TInt: KErrNone or error value.
        */
        TInt UiccReadEfPbrRecordResp( 
            TInt aStatus, 
            const TDesC8 &aFileData, 
            TInt aTraId );

        /**
        * Handles response for Type1 files record read response
        * @param  TInt aStatus : UICC Server status
        * @param TInt aTraId : transaction id
        * @param  TDesC8& aFileData : Response Data
        * @return TInt: KErrNone or error value.
        */
        TInt UiccReadEfType1RespL(
                TInt aStatus, 
                TInt aTraId,
                const TDesC8 &aFileData );

        /**
        * Handles response for Type2 files record read response
        * @param  TInt aStatus : UICC Server status
        * @param TInt aTraId : transaction id
        * @param  TDesC8& aFileData : Response Data
        * @return TInt: KErrNone or error value.
        */
        TInt UiccReadEfType2RespL(
                TInt aStatus, 
                TInt aTraId,
                const TDesC8 &aFileData );

        /**
        * Handles response for Type3 files record read response
        * @param  TInt aStatus : UICC Server status
        * @param TInt aTraId : transaction id
        * @param  TDesC8& aFileData : Response Data
        * @return TInt: KErrNone or error value.
        */
        TInt UiccReadEfType3RespL(
                TInt aStatus, 
                TInt aTraId,
                TUint8 aFileTag,
                const TDesC8 &aFileData );

        /**
        * Send request to continue reading Entry after reading Ext record
        * @param TUint8 aIndex : record index to be read
        * @const TDesC8 &aFileData : response data for Ext record read
        * @param TInt aTraId : Transaction id
        * @TInt &aExtFileTagIndex : Index offset for Ext record identifier 
        * in filedata
        * @return TInt : KErrNone or Error value
        */
        TInt CheckForExtToContinueNextRead( 
                TUint8 aIndex, 
                const TDesC8 &aFileData, 
                TInt aTraId,
                TInt &aExtFileTagIndex );

        /**
        * Send request to continue reading Entry
        * @param TUint8 aTraId : transaction id
        * @return TInt : error value
        */
        TInt ContinueWithNextReading( TUint8 aTraId );

        /**
        * Send Request to start reading Entry
        * @param TUitn8 aTransId : TransactionId
        * @return TInt : errorValue
        */
        TInt StartReadingEntry( TUint8 aTransId );

        /**
        * Creates Entry to append in the internal List
        * @param TInt aLocation : Entry Index
        */
        void CreateInternalPbEntryL(
            const TInt aLocation );

        /**
        * Creates Entry to append in the Sharing Buffer
        * @param TInt aLocation : Entry Index
        * @TBool &aAllocatedNow . If the Entry is Allocated now 
        * or returned earlier allocated entry
        * @return CPhoneBookStoreEntry* : Entry pointer
        */
        CPhoneBookStoreEntry* CreateReadPbEntryL(
            const TInt aLocation, TBool& aAllocatedNow );

        /**
        * Search for Index for respective FileId in respective FileList
        * @param TUint8 aFileTag : FileTag needs to be searched
        * @param RArray <TPrimitiveTag>& aFileList : FileList in 
        * which search needs to be done
        * @param TUinr8 aOffset : offset for start index
        * @return TInt : KErrNone or KErrNotFound
        */
        TInt SearchForFileTagIndex(
                    TUint8 aFileTag,
                    RArray <TPrimitiveTag>& aFileList,
                    TUint8 aOffset );

        /**
        * Send request to read next Type 2 file
        * @param TInt aTraId : Transaction id
        * @return TInt : KErrNone or KErrNotFound
        */
        TInt CheckForNextType2Read( TInt aTraId );
        
        /**
        * Get next used Type 2 File index
        * @param TInt aOffset : offset to start location
        * @return TInt : KErrNone or KErrNotFound
        */
        TInt GetNextUsedType2FileRecord( TInt &aOffset );

        /**
        * GetNext valid Type1 FileId 
        * @return TInt : KErrNotFound ot KErrNone
        */
        TInt GetNextType1File();

        /**
        * Check Entry is Empty or not
        * @param TUint8 aFileType : Type1 Type1 or Type3
        * @param TDesC8 &aFileData 
        * @return TBool : ETrue or EFalse
        */
        TBool CheckForEmptyEntry(
            const TUint8 aFileType,
            const TDesC8 &aFileData );

        /**
        * Get the empty record pattern for 
        * all ADN 3G Files
        * @param  TUint8 aFileType : Type1 Type2 or Type3
        * @param  TDes8& aEmptyPattern : Empty pattern for file
        */
        void GetEmptyRecordPattern(
            const TUint8 aFileType,
            TDes8& aEmptyPattern );

    public: // Data
        // None

    protected: // Data
        // None

    private: // Data
        // None

        // Attribute to store the Current PBR rrecord number
        TUint8 iCurrentPBRRecordNo;
        
        // Attribute to store the current EF record number
        TUint8 iCurrentEfEntryIndex;
        
        // Attribute to store the Current Phase ongoing
        TPBAdn3GReadPhases iCurrentReadPhase;

        // stores type 1 file list from EFpbr
        RArray <TPrimitiveTag> iType1FileArray;
        
        // stores type 2 file list from EFpbr
        RArray <TPrimitiveTag> iType2FileArray;
        
        // stores type 3 file list from EFpbr
        RArray <TPrimitiveTag> iType3FileArray;
        
        // Attribute Store the current type 1 file reading
        TInt iCurrentType1Ef;
        
        // Attribute to Current Type2 File
        TInt iCurrentType2Ef;
        
        // Attribute to store information about typeFileoperation ongoing
        TBool iType2OperationOngoing;


};

#endif // _CMMPHONEBOOK_OPERATION_READ_H

// End of file
