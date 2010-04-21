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

#ifndef CMMPHONEBOOKSTOREMESSHANDLER_H
#define CMMPHONEBOOKSTOREMESSHANDLER_H

//  INCLUDES
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>
#include "mmmmesshandlerbase.h"

#include "cmmphonetsender.h"
#include "cmmphonetreceiver.h"
#include "nokiatsy_internal_variation.h"

//UICC
#include "muiccoperationbase.h"
#include "cmmuiccmesshandler.h"

//  CONSTANTS
const TInt KUnusedLocation( 0xFF );

//  MACROS

#define UICC_MAX_PB_NUM      8

// LOCAL CONSTANTS AND MACROS
#define TON_NPI_NO_TEL_NBR          0xFF
#define TON_NPI_INTERNATIONAL       0x91
#define TON_NPI_UNKNOWN             0x00
#define TON_INTERNATIONAL           0x10

#define MASK_TON_FROM_TON_NPI_BYTE(a) ((a) & 0x70)
#define MASK_LOWER_NIBBLE_OF_BCD(a) ((a) & 0x0f)      /* B0000_1111 */

#define UICC_DTMF_CONTROL_DIGIT_CHAR_wait   'w'
#define UICC_DTMF_CONTROL_DIGIT_CHAR  'p'
#define UICC_WILD_VALUE_CHAR          '?'
#define UICC_WILD_VALUE_CHAR_ADN      'w'
#define UICC_EXPANSION_CHAR           'e'
#define UICC_INTERNATIONAL_NUM        '+'

#define UICC_EF_MAX_NAME_LEN                241

//  DATA TYPES
    //enum

enum TPBConfData
    {
    ADNConfData = 0,
    FDNConfData,
    SDNConfData,
    MBDNConfData,
    VMBXConfData,
    MSISDNConfData,
    };

struct TIapInfo
    {
    TUint16 fileId;
    TUint8 fileSfi;
    TUint8 fileTag;
    TUint8 recordNo;
    };

struct TPBEntry
    {
    RArray<TInt> PBEntryExtRecord;
    TBool iEntryPresent;
    TInt iEntryIndex;
    TUint8 fileId;
    TUint8 fileSFI;
    RArray<TInt> groupNameIdentifiers;
    RArray<TIapInfo> iapInfo;
    };

struct TPBEntryList
    {
    RArray< TPBEntry* > iEntryList;
    };


class TPrimitiveInitInfo
    {
public:
    TPrimitiveInitInfo();

    void GetPBEntryFromUICCData( const TDesC8 &aFileData, TDes8& aNumber, TDes8& aName);


    TUint16 iNoOfRecords;          // 2 byte long
    TUint16 iAlphaStringlength;    // 2 byte long
    TUint16 iNumlength;          // 2 byte long
    TUint16 iExtNoOfRec;
    TUint8 iMbiRecLen;
    TBool iExtension;

    // Store ADN 3G realted Data also
    TInt iPBRNoOfRecords;
    TInt iIAPRecordLength;
    TInt iANRNoOfRecords;
    TInt iSNENoOfRecords;
    TInt iSNEStringLength;
    TInt iEmailNoOfRecords;
    TInt iEmailStringLength;
    };


// Struct


// look up table for BCD digits

const TUint8 LookupArray[16]=
      {
              '0','1','2','3','4','5','6','7','8','9',    /* 0-9 */
              '*',                                        /* 0xA */
              '#',                                        /* 0xB */
              UICC_DTMF_CONTROL_DIGIT_CHAR,            /* 0xC, DTMF control digit * separator  */
              UICC_WILD_VALUE_CHAR,                    /* 0xD, 'Wild' value */
              UICC_EXPANSION_CHAR,                     /* 0xE, Expansion digit */
      };

const TUint8 LookupArrayAdn[16]=
      {
              '0','1','2','3','4','5','6','7','8','9',    /* 0-9 */
              '*',                                        /* 0xA */
              '#',                                        /* 0xB */
              UICC_DTMF_CONTROL_DIGIT_CHAR,            /* 0xC, DTMF control digit * separator  */
              UICC_WILD_VALUE_CHAR_ADN,                /* 0xD, 'Wild' value */
              UICC_EXPANSION_CHAR,                     /* 0xE, Expansion digit */
      };



//  EXTERNAL DATA STRUCTURES

//  FUNCTION PROTOTYPES
    //none

//  FORWARD DECLARATIONS
class TIsiReceiveC;
class CMmPhoneBookStoreOperationBase;
class CMmPhoneBookStoreOperationList;
class CMmMessageRouter;

// CLASS DECLARATION
/**
* CMmPhoneBookStoreMessHandler is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender. It also receives
* GSM-specific PBStore ISI messages.
*/
class CMmPhoneBookStoreMessHandler
    : public CBase,
      public MMmMessHandlerBase,
      public MMmMessageReceiver,
      public MUiccOperationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmPhoneBookStoreMessHandler* NewL(
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler);

        /**
        * Destructor.
        */
        ~CMmPhoneBookStoreMessHandler();

        /**
        * From CMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        *
        * @param aIpc Identify number of request.
        * @param aDataPackage data
        * @return KErrNone or error code
        */
        TInt ExtFuncL(
            TInt aIpc,
            const CMmDataPackage* aDataPackage );


        /**
        * Handles a received message by calling the specific
        * message handling method.
        *
        * @param Transaction id Reference to the received message transaction ID.
        * @param Status
        * @param aFileData reference to Data received in message
        * @return KErrNone or error code
        */
        TInt ProcessUiccMsg(
            TInt aTransactionId,
            TInt aStatus,
            TUint8 aDetails,
            const TDesC8 &aFileData );

        /**
        * Creates entry point to correct operation.
        *
        * @param aDataPackage Packaged data
        * @param aIpc Identify number of request.
        * @return Pointer to operation.
        */
        CMmPhoneBookStoreOperationBase* CreateNewOperationL(
            const CMmDataPackage* aDataPackage,
            TInt aIpc );

        /**
        * Store Phonebook Entry from UICC -message
        *
        * @param aName Name
        * @param aNumber Number
        * @param aEntry Received UICC data
        * @param aFileId File ID
        * @param const TInt aIndexToRead Index of file
        * @param const TBool aMailboxIdExist Is there a mailbox id
        * @return none
        */
        static void StorePhonebookEntryL( TDes8& aName,
                                          TDes8& aNumber,
                                          CPhoneBookStoreEntry& aEntry,
                                          const TUint16 aFileId,
                                          const TInt aIndexToRead );

        /**
        * Store ANR to phonebook entry
        *
        * @param aAnr Additional number
        * @param aEntry Phonebook entry where ANR is added
        * @param aFileId File ID
        * @return none
        */
        static void StoreAnrToPhonebookEntryL(
            TDes8& aAnr,
            CPhoneBookStoreEntry& aEntry,
            const TUint16 aFileId );


        /**
        * Store SNE or EMAIL to phonebook entry
        * @param aString Second name/email address string
        * @param aEntry Phonebook entry where SNE/EMAIL is added
        * @param aFileTypeTag Tag indicating file type
        * @return none
        */
        static void StoreSneEmailToPbEntryL(
            TDes8& aString,
            CPhoneBookStoreEntry& aEntry,
            TUint8 aFileTypeTag );

        /**
        * Handle number to convert in Ascii Format
        * @param const TDesC8& aSource: Message to be converted in Ascii
        * @param TDes16 aTarget : After conversion data to be staored in
        */
        static void ConvertToUcs2FromBCD(
            const TDesC8 &aSource,
            TDes16 &aTarget,
            const TUint16 aFileData );

        /**
        * Handle number to convert in BCD format from UCS2 Format
        * @param const TDesC16& aSource: Message to be converted in BCD
        * @param TDes8 aTarget : After conversion data to be stored in target buffer
        */
        static TInt ConvertToBCDFromUCS2(
            TDesC16 &aSource,
            TDes8 &aTarget,
            TUint16 aFileId );

        /**
        * Handle number to convert in BCD format from UCS2 Format
        * @param TInt16 aUCSCharacter: Character to be converted
        * @param const TUint16 aFileId :File id
        * @return The BCD number
        */
        static TInt GetBCDCodeforUCS(
            TUint16 aUCSCharacter,
            TUint16 aFileId );

        /**
        * Sets PhoneBook Entry to PhoneBook Entry List.
        *
        * @param aStoreEntry.
        * @return None
        */
        void StoreEntryToPhoneBookList(
            TPBEntry* aStoreEntry,
            TUint8 aPBIndex );

        /**
        * Reset phonebook entry in phoneBook entry list.
        *
        * @param aPbIndex Phonebook index
        * @param aPbIndex Entry index
        * @return None
        */
        void ResetEntryInPhoneBookList( TUint8 aPbIndex, TInt aEntryIndex );

        /**
        * Sets PhoneBook Entry to PhoneBook Entry List.
        *
        * @param aStoreEntry.
        * @return None
        */
        TBool IndexCheckInPBList(
            TUint8 aIndex,
            TUint8 aPBIndex,
            TPBEntry& aEntry );

        /**
        * Get the PBR Record number
        *
        * @param aIndexToRead : actual index to be read or write
        * @param aPBRRecNum : PBR record num calculated
        * @return None
        */
        TInt GetPBRRecordNum(
            TInt aIndexToRead,
            TUint8 &aPBRRecNum );

        /**
        * Get currentElementary File record num
        *
        * @param aIndexToRead : actual index to be read or write
        * @param aCurrentRecNum : Current Ef record num calculated
        * @return None
        */
        TInt GetCurrentEfRecNum(
            TUint8 aPBRRecNum,
            TUint8 &aCurrentRecNum,
            TInt aIndexToRead);

        /**
        * Find Index for Present Entry
        *
        * @param aIndex
        * @param aPBIndex
        * @return Index for Entry
        */
        TInt GetIndexForPresentEntry( TUint8 aIndex, TUint8 aPBIndex );

        /**
        * Finds free entry in the List.
        *
        * @param TUint8 Phonebook index for Phonebook confoguration list
        * @return TInt Index number for free entry
        */
        TInt EmptyEntryCheckInPBList( TUint8 aPBIndex );

        /**
        * Gets MBDN phonebook flag.
        *
        * @param None
        * @return Flag if MBDN phonebook is ready or not.
        */
        TBool MBDNPhonebook();

        /**
        * Sets MBDN phonebook flag.
        *
        * @param aBoolean Flag of MBDN phonebook.
        * @return None
        */
        void SetMBDNPhonebook( TBool aBoolean );

        /**
        * Sets number of FDN info.
        *
        * @param aNumber Number of FDN info.
        * @return None
        */
        void SetNumberOfFdnInfoResps( TUint8 aNumber );

        /**
        * Remove the main Entry Information from Stored list
        *
        * @param aIndex - Index to be removed.
        * @return None
        */
        void UpdateEntryFromList(
            TPBEntry* aEntry,
            TUint8 aIndex,
            TUint8 aPBIndex);

        /**
        * Remove the EXT records Information from Stored list
        *
        * @param aIndex - Index of Ext record to be removed.
        * @return None
        */
        void RemoveExtEntryFromList( TUint8 aIndex, TUint8 aPBIndex);


        /**
        * Finds entry from iPBEntryList
        *
        * @param TUint8 aIndex:
        * @param TUint8 aRecordNo: record to be found
        * @return TPBEntry*: pointer to found entry or NULL
        */
        TPBEntry* FindEntryFromPbList( TUint8 aIndex, TUint8 aRecordNo );


        /**
        * Finds corresponding ADN entry for type 2 file
        *
        * @param aCurrentType2EfIndex Index of current elementary file
        * @param aCurrentRecordNum Current record number
        * @param aArray ADN entry IDs are stored here
        * @return None
        */
        void GetEntriesForType2FileId(
            const TInt aCurrentType2EfIndex,
            const TInt aCurrentRecordNum,
            RArray<TInt>& aArray );

        /**
        * Gets pointer to CMmMessageRouter class.
        *
        * @param None
        * @return Pointer to CMmMessageRouter object.
        */
        CMmMessageRouter* MessageRouter();


        /**
        * Gets pointer to CMmUiccMessHandler class
        *
        * @param None
        * @return Pointer to CMmUiccMessHandler object.
        */
        CMmUiccMessHandler* UiccMessHandler();

        /**
        * Gets pointer to CMmPhoNetSender class.
        *
        * @param None
        * @return Pointer to CMmPhoNetSender object.
        */
        CMmPhoNetSender* PhoNetSender();

    protected:
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookStoreMessHandler();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL(
            CMmPhoneBookStoreMessHandler* mmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aUiccMessHandler);

        /**
        * Gets ISI message from PhoNetReceiver related to SIM_IND
        *
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void SimInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Get mailbox identifiers
        *
        * @return Error code
        */
        TInt GetMailboxIdentifiers();


        /**
        * Handle mailbox identifiers
        *
        * @param aStatus Status
        * @param aFileData File data
        * @return None
        */
        void HandleGetMailboxIdentifiers(
            TInt aStatus,
            const TDesC8 &aFileData );



    public:     // Data
        // table for All phone books Configuration Data
       TPrimitiveInitInfo  iPBStoreConf[UICC_MAX_PB_NUM];

    protected:  //Data
        // None

    private:    // Data

        // Pointer to the Phonet sender
        CMmPhoNetSender*  iPhoNetSender;

        //Pointer to the Message router
        CMmMessageRouter* iMessageRouter;

        // Pointer to the UICC Messhandler
        CMmUiccMessHandler* iMmUiccMessHandler;

        // Array for storing objects of operations.
        CMmPhoneBookStoreOperationList* iOperationlist;

        // Number of FDN info responses
        TUint8 iNumberOfFdnInfoResps;

        // to store CardType
        TUint8 iCardType;

        // Array to Store PhoneBook Entry Status and EXT record no list
        TFixedArray< TPBEntryList,UICC_MAX_PB_NUM > iPBEntryList;
};

#endif // CMMPHONEBOOKSTOREMESSHANDLER_H

// End of File
