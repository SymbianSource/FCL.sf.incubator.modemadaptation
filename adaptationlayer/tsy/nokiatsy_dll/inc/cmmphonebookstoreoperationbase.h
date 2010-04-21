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

#ifndef CMMPHONEBOOKSOPERATIONBASE_H
#define CMMPHONEBOOKSOPERATIONBASE_H

//  INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmstaticutility.h"
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>
#include <badesca.h>    //for arrays
#include <e32base.h>

#include "cmmphonetsender.h"
#include "cmmphonetreceiver.h"
#include "cmmphonebookstoremesshandler.h"


//UICC
#include "muiccoperationbase.h"
#include "cmmuiccmesshandler.h"
#include <uiccisi.h>



//  CONSTANTS
const TInt KLocationFirst = -1;
const TUint8 KMinLength = 1;
const TUint8 KLengthWithSFI = 3;
const TUint8 KLengthWithOutSFI = 2;

_LIT(KInternalPhoneBookType,"Init");

const TUint8 KPadding = 0;

// Mask that is used to separate phonebooktype from transaction Id
const TUint8 KMaskPhonebookType = 0xF0;

// Mask that is used to separate operation from transaction Id
const TUint8 KMaskOperationType = 0x0F;

// Added mask (4 most significant bits) to separate different phonebook's
const TUint8 KMaskAdnType = 0x10;       //0001 0000 = ADN
const TUint8 KMaskFdnType = 0x20;       //0010 0000 = FDN
const TUint8 KMaskSdnType = 0x30;       //0011 0000 = SDN
const TUint8 KMaskBdnType = 0x40;       //0100 0000 = BDN
const TUint8 KMaskMbdnType = 0x50;      //0101 0000 = MBDN
const TUint8 KMaskVoiceMailBox = 0x60;  //0110 0000 = VMBX

//const TUint8 KMaskFreeType = 0x09;  //1001 0000 = Free
//const TUint8 KMaskFreeType = 0xA0;  //1010 0000 = Free
//const TUint8 KMaskFreeType = 0xB0;  //1011 0000 = Free
//const TUint8 KMaskFreeType = 0xC0;  //1100 0000 = Free
//const TUint8 KMaskFreeType = 0xD0;  //1101 0000 = Free
//const TUint8 KMaskFreeType = 0xE0;  //1110 0000 = Free
const TUint8 KMaskUnspecifiedType = 0xF0;//1111 0000 = Unknown

// Added mask (4 least significant bits) to separate different request IPC's
const TUint8 KOperationInit = 0x01;        //0000 0001 = init
const TUint8 KOperationCache = 0x02;       //0000 0010 = cache
const TUint8 KOperationRead = 0x03;        //0000 0011 = Read
const TUint8 KOperationWrite = 0x04;       //0000 0100 = Write
const TUint8 KOperationDelete  = 0x05;     //0000 0101 = Delete
const TUint8 KOperationDeleteAll = 0x6;    //0000 0110 = DeleteAll
const TUint8 KOperationDeleteWrite = 0x07; //0000 0111 = writeDelete = AdnCase
const TUint8 KOperationReWrite = 0x08;      //0000 1000 = rewrite = adn case
const TUint8 KOperationGetMailBoxNumber = 0x09; //0000 1001 = GetMBDN number
const TUint8 KOperationGetInfo = 0x0A;      //0000 1010
//const TUint8 KOperationFree = 0x0C;
//const TUint8 KOperationFree = 0x0D;
//const TUint8 KOperationFree = 0x0E;
const TUint8 KOperationUnknown = 0x0F;      //0000 1111 = Unknown
const TUint8 KOperationMask = 0x0F;

const TUint8 KMBDNPhonebook = 6;
const TUint8 KSimMbdn = 0x00;

//File ID
const TUint16 KMasterFile            = 0x3F00;  // Master File
const TUint16 KDedicatedFileGSM      = 0x7F20;  // Dedicated File (Gsm)


const TUint8 KTagFCIFileDescriptor = 0x82;     // for file descriptor tag in FCI data received from UICC Server
const TUint8 KTagFCIFileIdentifier = 0x83;     // for File identifier in FCI data
const TUint8 KTagFCIFileSize       = 0x80;     // for File size data
const TUint8 KTagConstructedTagA8  = 0xA8;     // Type 1 files tag in PBR file
const TUint8 KTagConstructedTagA9  = 0xA9;     // Type 2 files tag in PBR file
const TUint8 KTagConstructedTagAA  = 0xAA;     // Type 3 files tag in PBR file
const TUint8 KTagUnusedbyte        = 0xFF;     // constant to find unused bytes

const TUint8 KRecordLength   = 0;
const TUint8 KNoOfRecords    = 1;
const TUint8 KFileSize1       = 2;
const TUint8 KFileIdentifier1 = 3;
const TUint8 KFileStatus1     = 4;
const TUint8 KStartRecord = 1; // Record numbering starts from 1

const TUint8 KAdditionalData  = 0x02;
const TUint8 KExtRecordSize  = 13;
const TUint8 KAdnMandatoryBytes = 13;
const TUint8 KAnrExtRecIdOffset = 14;
const TUint8 KExtRecLenWithoutRecId = 12;
const TUint8 KIapRecordsToBeSearched        = 0 ;     // constant to find unused bytes
const TUint8 KMaxNoOfRecInOneEf = 254;

const TUint8 KAdditionalNoType = 0x02;
// UICC constants
#define MF_FILE                             0x3F00 //Master file
#define DF_CURRENT_APP                      0x7FFF //
#define DF_PHONEBOOK                        0x5F3A
#define APPL_FILE_ID                        0x7F10  // Address for DFphonebook
#define UICC_SESSION_ID_NOT_USED            0x00
#define PB_UICC_SB_APPL_PATH_SB_LENGTH      0x0010
#define PB_UICC_SB_APPL_PATH_SB_LENGTH_2    0x0012
#define PB_ADN_FID                          0x6F3A
#define PB_FDN_FID                          0x6F3B
#define PB_SDN_FID                          0x6F49
#define PB_BDN_FID                          0x6F4D
#define PB_MBDN_FID                         0x6FC7
#define PB_MBI_FID                          0x6FC9
#define PB_MSISDN_FID                       0x6F40
#define PB_VMBX_FID                         0x6F17
#define PB_EXT1_FID                         0x6F4A
#define PB_EXT2_FID                         0x6F4B
#define PB_EXT3_FID                         0x6F4C
#define PB_EXT4_FID                         0x6F55
#define PB_EXT5_FID                         0x6F4E
#define PB_EXT6_FID                         0x6FC8
#define PB_EXT7_FID                         0x6FCC
#define PB_PBR_FID                          0x4F30
#define DF_PB_FILE_PATH                     0x3F007FFF5F3A
#define UICC_SFI_NOT_PRESENT                0x00
#define DF_PATH_LENGTH                      0x06
#define DF_PATH_TELECOM                     0x7F10
#define DF_PATH_PHONEBOOK                   0x5F3A
#define UICC_SB_LINEAR_FIXED_SB_LENGTH      0x0008
#define UICC_APPL_CMD_RESP_OFFSET           0x07
#define UICC_ILLEGAL_FILE_ID                0xFFFF

#define UICC_SB_FCI_FCP_EF_NO_REC_OFFSET    0x12
#define UICC_SB_FCI_FCP_EF_REC_L_OFFSET     0x10

#define UICC_SB_FILE_DATA_OFFSET            0x08
#define UICC_EF_ANR_EXT_REC_OFFSET          0x15
#define UICC_EF_FILE_EXT_FILE_NOT_EXIST     0xFF
#define UICC_EF_FDN_NO_EXT_NUM_LEN          20
#define UICC_EF_EXT_REC_NUM_LEN             11
#define UICC_EF_MAX_NUM_LEN_WITH_EXT        2814
#define UICC_EF_ANR_EXT_OFFSET              14
#define UICC_EF_MAX_REC_LEN                 255

#define UICC_EF_FDN_MIN_REC_LEN             14

#define UICC_APPL_CMD_RESP_TRANSID_OFFSET   0x00
#define UICC_APPL_CMD_RESP_NUMSBLOCK_OFFSET 0x07
#define UICC_APPL_CMD_RESP_STATUS_OFFSET    0x04
#define UICC_APPL_CMD_RESP_DETAILS_OFFSET   0x05
#define UICC_SERVER_STATUS_OFFSET           0x03

#define UICC_PBR_CONSTRUCTED_TAG_A8         0xA8
#define UICC_PBR_CONSTRUCTED_TAG_A9         0xA9
#define UICC_PBR_CONSTRUCTED_TAG_AA         0xAA

#define UICC_OFFSET_TO_FIRST_PRIM_TAG       0x25
#define UICC_OFFSET_TO_FIRST_PRIM_TAG_FDI   0x26
#define UICC_OFFSET_TO_FIRST_PRIM_TAG_SFI   0x27
#define UICC_PBR_PRIMITIVE_TAG_SFI_IS_ZERO  0x00

#define UICC_ADN_PRIM_TAG                   0xC0
#define UICC_IAP_PRIM_TAG                   0xC1
#define UICC_SNE_PRIM_TAG                   0xC3
#define UICC_ANR_PRIM_TAG                   0xC4
#define UICC_GRP_PRIM_TAG                   0xC6
#define UICC_EMAIL_PRIM_TAG                 0xCA

#define UICC_FCI_EF_NO_REC_OFFSET           0x12

#define UICC_FCI_FILE_DESC_TAG              0x82
#define UICC_FCI_EF_FDESC_OFFSET_NUM_ENTR   6
#define UICC_FCI_EF_FDESC_OFFSET_ENT_LENG   4
#define UICC_ADN_EF_REC_LEN_FIXED_PART      14

#define UICC_EF_ADN_TAG                     0xC0
#define UICC_EF_IAP_TAG                     0xC1
#define UICC_EF_EXT1_TAG                    0xC2
#define UICC_EF_SNE_TAG                     0xC3
#define UICC_EF_ANR_TAG                     0xC4
#define UICC_EF_PBC_TAG                     0xC5
#define UICC_EF_GRP_TAG                     0xC6
#define UICC_EF_AAS_TAG                     0xC7
#define UICC_EF_GAS_TAG                     0xC8
#define UICC_EF_UID_TAG                     0xC9
#define UICC_EF_EMAIL_TAG                   0xCA
#define UICC_EF_CCP1_TAG                    0xCB

// UICC Server Service type
#define UICC_APPL_READ_TRANSPARENT          0x21
#define UICC_APPL_UPDATE_TRANSPARENT        0x22
#define UICC_APPL_READ_LINEAR_FIXED         0x23
#define UICC_APPL_UPDATE_LINEAR_FIXED       0x24
#define UICC_APPL_FILE_INFO                 0x25
#define UICC_APPL_APDU_SEND                 0x26
#define UICC_APPL_CLEAR_CACHE               0x27
#define UICC_APPL_SESSION_START             0x28
#define UICC_APPL_SESSION_END               0x29

// SST table Service numebers

#define ICC_ADN_SERVICE_NUM                2
#define ICC_FDN_SERVICE_NUM                3
#define ICC_SDN_SERVICE_NUM                18
#define ICC_BDN_SERVICE_NUM                31
#define ICC_MSISDN_SERVICE_NUM             9
#define ICC_EXT1_SERVICE_NUM               10
#define ICC_EXT2_SERVICE_NUM               11
#define ICC_EXT3_SERVICE_NUM               19
#define ICC_EXT4_SERVICE_NUM               32
#define ICC_EXT5_SERVICE_NUM               44
#define ICC_MBDN_SERVICE_NUM               53

// UST Table Service numbers

#define UICC_FDN_SERVICE_NUM                2
#define UICC_SDN_SERVICE_NUM                4
#define UICC_BDN_SERVICE_NUM                6
#define UICC_MSISDN_SERVICE_NUM             21
#define UICC_EXT2_SERVICE_NUM               3
#define UICC_EXT3_SERVICE_NUM               5
#define UICC_EXT4_SERVICE_NUM               7
#define UICC_EXT5_SERVICE_NUM               44
#define UICC_MBDN_SERVICE_NUM               47


// CPHS Service table
#define ICC_CSP_SERVICE_NUM                 1
#define ICC_SST_SERVICE_NUM                 2
#define ICC_MAILBOX_NUM                     3
#define ICC_OP_NAME_SERVICE_NUM             4


#define UICC_NO_EXT_MAX_NUM_LEN             0x14
#define UICC_EXT_REC_NO_OFFSET              12
#define UICC_EMPTY_ENTRY_PATTERN_1          0xFF
#define UICC_EMPTY_ENTRY_PATTERN_2          0x00


// File types

#define UICC_TYPE1_FILE                     1
#define UICC_TYPE2_FILE                     2
#define UICC_TYPE3_FILE                     3


const TIapInfo iapinfo = { UICC_ILLEGAL_FILE_ID, 0x00, 0x00, 0x00};

// end UICC constants


//  MACROS
    //none

//  DATA TYPES
    //none

//  EXTERNAL DATA STRUCTURES


enum TPhonebookType
    {
    EPhonebookTypeAdn = 0,
    EPhonebookTypeFdn,
    EPhonebookTypeSdn,
    EPhonebookTypeBdn,
    EPhonebookTypeVMBX,
    EPhonebookTypeMBDN,
    EPhonebookTypeMSISDN,
    EPhonebookTypeUnknown
    };

enum TTypeOfFileToBeRead
    {
    EStartRead,
    EBasicEfRead,
    EExtensionRead,
    EMailboxIdRead,
    EFileInfoRead,
    EBasicEfReadToGetUsedFileCount
    };

// Struct data

struct TPrimitiveTag
    {
    TUint8 tagValue;     // 1 byte long
    TInt   tagFID;    // 2 byte long
    TUint8  tagSFI;
    };

//  FUNCTION PROTOTYPES
    //none

//  FORWARD DECLARATIONS
class CMmPhoneBookStoreExtInterface;
class CPhoneBookStoreEntry;
class CStorageInfoData;
class CMmUiccMessHandler;

// CLASS DECLARATION
/**
* CMmPhoneBookStoreOperationBase is base class for all operations
*/
class CMmPhoneBookStoreOperationBase
    : public CBase
    {
    public:

        /**
        * C++ constructor.
        */
        CMmPhoneBookStoreOperationBase();

        /**
        * Destructor.
        */
        ~CMmPhoneBookStoreOperationBase();

        // Second phase constructor
        static CMmPhoneBookStoreOperationBase* NewL(
                           CMmUiccMessHandler* aUiccMessHandler);
        /**
        * Basic implementation to handling request if operation not found.
        *
        * @param aIpc not  used
        * @param aDataPackage not used
        * @return KErrNotSupported.
        */
        virtual TInt CreateReq(
            TInt /*aIpc*/,
            const CMmDataPackage* /*aDataPackage*/ )
            {TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::CreateReq - Return KErrNotSupported");
            return KErrNotSupported; };

        /**
        * Cancels the operation.
        */
        virtual void CancelReq( TName& aPhoneBook );

        /**
        * Completes the request.
        *
        * @param aErrorCode error code to be completed with
        * @return TInt KErrNone or KErrNotSupported if operation is not prepared
        */
        virtual TInt CompleteReq( TInt aErrorCode );

        /**
        * Basic implementation to handling request if operation not found in UICC.
        *
        * @param aComplete not used
        * @param aStatus not used
        * @param aFileData KErrNotSupported
        * @return KErrNotSupported.
        */

        virtual TBool HandleUICCPbRespL(
                        TInt /*aStatus*/,
                        TUint8 /*aDetails*/,
                        const TDesC8& /*aFileData*/,
                        TInt /*aTransId*/)
            {
            TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::HandleUICCPbRespL - Return KErrNotSupported");
            return ETrue;
            };

        /**
        * Basic implementation to handling request if operation not found in UICC.
        *
        * @param aIpc not  used
        * @param aDataPackage not used
        * @return KErrNotSupported.
        */
        virtual TInt UICCCreateReq(
            TInt /*aIpc*/,
            const CMmDataPackage* /*aDataPackage*/,
            TUint8 /*aTransId*/ )
            {
            TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::CreateReq - Return KErrNotSupported");
            return KErrNotSupported;
            };


        /**
        * Converts client phonebook to phonebook mask.
        *
        * @param aPBType
        * @return TUint8
        */
        static TUint8 ConvertToPBtype( const TName& aPBType );


        /**
        * Converts client phonebook to phonebook mask.
        *
        * @param aPBType
        * @return TUint8
        */
        static TUint16 ConvertToPBfileId(
                       const TName& aPBType,
                       TUint16& aFileIdExt,
                       TUint8 aCardType );


        /**
        * Converts File ID to Index
        *
        * @param aFileId
        * @return TUint8
        */
        static TUint8 ConvertToConfArrayIndex( const TUint16 aFileId );
        
        /**
        * Converts client phonebook to phonebook mask.
        *
        * @param aPBType
        * @return TUint8
        */
        static TUint8 ConvertToPBtypeUICC( const TName& aPBType );


        /**
        * Converts client phonebooktype to client phonebook name.
        *
        * @param aPBType
        * @return TUint8
        */
        void ConvertPBTypeToPbName(
                      const TUint aPhonebookType,
                      TName& aName );

        /**
        * Converts client phonebook to phonebook mask.
        *
        * @param aPBType
        * @return TUint8
        */
        void ConvertToPBname(
                     const TUint8 aTrans,
                     TName& aName );

        /**
        * Gets PhoneBookName.
        *
        * @param None
        * @return phonebookname.
        */
        const TName& GetPhoneBookName()const;

        /**
        * Convert number of phonebook type to server's phonebook type
        *
        * @param aPbMask Phonebook mask
        * @return Server phonebook type.
        */
        TUint16 ConvertToSimPhoneBookType(
            const TUint8 aPbMask );

        /**
        * Handle to Find the Empty Entry
        * @param const TDesC8& aFileData: Entry data to checked entry is empty or not
        * @return TInt: KErrNone or KErrNotFound
        */
        TInt EmptyEntryCheck( const TDesC8 &aFileData );

        /**
        * Searches wanted file list from EFpbr
        * @param aFileData: data of EFpbr record
        * @param aTag: Tag for file list to be search
        * @param aFileList: parameter where file list is inserted
        * @return TInt: KErrNone or KErrNotFound
        */
        TInt FetchFileListFromPBR(
            const TDesC8 &aFileData,
            const TUint8 aTag,
            RArray <TPrimitiveTag>& aFileList );

    protected:



    private:

        // ConstructL

        void ConstructL();

    public:     // Data
        // None

    protected:  // Data

        // Pointer to CMmPhoneBookStoreMessHandler.
        CMmPhoneBookStoreMessHandler* iMmPhoneBookStoreMessHandler;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

        // To store Phone Book type
        TName iPhoneBookTypeName;

        // Pointer to array for Caching PB Store <ADN/FDN>.
        // CTSY take care of deleting entries.
        CArrayPtrSeg<CPhoneBookStoreEntry>* iPhoneBookStoreCacheArray;

        // Number of FDN info responses.
        TUint8 iNumberOfFdnInfoResps;

        // FDN info responses.
        TPBFdnInfo iPbFdnInfo;

        //slot where SIM contact has been written.
        TUint16 iWriteLocation;

        // Attribute to hold the information what index to be read
        TInt iIndexToRead;

        // Attribute to store record length
        TInt iRecordLength;

        // Keeps track of error value if write completed only partially.
        TInt iRet;

        // modifiable Store number if there is extension number also there
        TBuf8<UICC_EF_MAX_NUM_LEN_WITH_EXT> iNumberBuf;

        // Store entry name
        TBuf8<UICC_EF_MAX_NAME_LEN> iNameBuf;


       // Store for Empty Space
        TUint8 iEmptyIndex;

        // Attribute to check what kind of read is ongoing
        TTypeOfFileToBeRead iTypeOfReading ;

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        // Holds information for AAS and GAS
        RMmCustomAPI::TAlphaStringParams iAlphaStringParams;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

    private:    // Data
        //none
    };

#endif // CMMPHONEBOOKSOPERATIONBASE_H

// End of file
