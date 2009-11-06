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

#ifndef CMMPHONEBOOKSOPERATIONBASE_H
#define CMMPHONEBOOKSOPERATIONBASE_H

//  INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmstaticutility.h"
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>
#include <badesca.h>    //for arrays
#include <e32base.h>

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
#include <ctsy/rmmcustomapi.h>
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

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

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
const TUint8 SIM_AAS  = 0x10;
const TUint8 SIM_GAS  = 0x11;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

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

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
const TUint8 KMaskAasType = 0x70; //0111 0000 = Additional Alpha String
const TUint8 KMaskGasType = 0x80;     //1000 0000 = Group Alpha String
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

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
const TUint16 KDedicatedFileMBI      = 0x6FC9;  // Dedicated File (MBI)


const TUint8 KTagFCIFileDescriptor = 0x82;     // for file descriptor tag in FCI data received from UICC Server
const TUint8 KTagFCIFileIdentifier = 0x83;       // for File identifier in FCI data
const TUint8 KTagFCIFileSize       = 0x85;      // for File size data
const TUint8 KTagConstructedTagA8  = 0xA8;     // Type 1 files tag in PBR file
const TUint8 KTagConstructedTagA9  = 0xA9;     // Type 2 files tag in PBR file
const TUint8 KTagConstructedTagAA  = 0xAA;     // Type 3 files tag in PBR file
const TUint8 KTagUnusedbyte        = 0xFF;     // constant to find unused bytes

const TUint8 KRecordLength   = 0;
const TUint8 KNoOfRecords    = 1;
const TUint8 KFileSize       = 2;
const TUint8 KFileIdentifier = 3;
const TUint8 KFileStatus     = 4;






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
#define PB_MSISDN_FID                       0x6F40
#define PB_VMBX_FID                         0x6F17
#define PB_EXT1_FID                         0x6F4A
#define PB_EXT2_FID                         0x6F4B
#define PB_EXT3_FID                         0x6F4C
#define PB_EXT4_FID                         0x6F55
#define PB_EXT5_FID                         0x6F4E
#define PB_EXT6_FID                         0x6FC8
#define PB_PBR_FID                          0x4F30
#define DF_PB_FILE_PATH                     0x3F007FFF5F3A
#define UICC_SFI_NOT_PRESENT                0x00
#define DF_PATH_LENGTH                      0x06
#define DF_PATH_TELECOM                     0x7F10
#define DF_PATH_PHONEBOOK                   0x5F3A
#define UICC_SB_LINEAR_FIXED_SB_LENGTH      0x0008
#define UICC_APPL_CMD_RESP_OFFSET           0x07

#define UICC_SB_FCI_FCP_EF_NO_REC_OFFSET    0x12
#define UICC_SB_FCI_FCP_EF_REC_L_OFFSET     0x10

#define UICC_SB_FILE_DATA_OFFSET            0x08
#define UICC_EF_ANR_EXT_REC_OFFSET          0x15
#define UICC_EF_FILE_EXT_FILE_NOT_EXIST     0xFF
#define UICC_EF_FDN_NO_EXT_NUM_LEN          20
#define UICC_EF_MAX_NUM_LEN_WITH_EXT        2814
#define UICC_EF_MAX_NAME_LEN                241
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


// File types

#define UICC_TYPE1_FILE                     1
#define UICC_TYPE2_FILE                     2
#define UICC_TYPE3_FILE                     3


// end UICC constants


//  MACROS
    //none

//  DATA TYPES
    //none

//  EXTERNAL DATA STRUCTURES
enum TPBIniPhases
    {
    EPBInitPhaseADN = 0,
    EPBInitPhaseFDN,
    EPBInitPhaseSDN,
    EPBInitPhaseMBDN,
    EPBInitPhaseMSISDN,
    EPBInitPhaseVMBX,
    EPBInitPhase_File_Info,
    EPBInitPhase_FIle_Data,
    EPBInitPhase_Complete,
    EPBInitPhase_3GADN_PBR,
    EPBInitPhase_3GADN_Type1,
    EPBInitPhase_3GADN_Type2,
    EPBInitPhase_3GADN_Type3,
    EPBIniPhase_3GADNDone,
    EPBIniPhase_PBInitialized,
    EPBIniPhase_Unknown,
    EPBIniPhase_Internal
    };


enum TPhonebookType
    {
    EPhonebookTypeAdn = 0,
    EPhonebookTypeFdn,
    EPhonebookTypeSdn,
    EPhonebookTypeBdn,
    EPhonebookTypeVMBX,
    EPhonebookTypeMBDN
    };

enum TPBWritePhases
    {
    EPBWritePhase_delete = 0,
    EPBWritePhase_rewrite,
    EPBWritePhase_complete
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

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    class CAlphaString;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

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
        static CMmPhoneBookStoreOperationBase* NewL(CMmUiccMessHandler* aUiccMessHandler);
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
        virtual void CancelReq();

        /**
        * Prepares the operation (makes it ready to be launched).
        *
        * @param aIpc IPC request
        * @param aDataPackage parameters to prepare request with
        * @return TInt KErrNotReady on attempt to prepare prepared operation
        */
        virtual TInt PrepareReq( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * Checks if operation is ready to be launched.
        * Base implementation returns EFalse
        *
        * @return TBool ETrue - operation is ready to be launched
        */
        virtual TBool IsPrepared() const;

        /**
        * Launches activated operation.
        *
        * @return TInt KErrNotReady on attempt to launch not prepared operation;
        * or system-wide error code
        */
        virtual TInt LaunchReq();

        /**
        * Completes the request.
        *
        * @param aErrorCode error code to be completed with
        * @return TInt KErrNone or KErrNotSupported if operation is not prepared
        */
        virtual TInt CompleteReq( TInt aErrorCode );

        /**
        * Basic implementation to handling response if operation not found.
        *
        * @param aIsiMessage not used
        * @param aComplete not used
        * @return KErrNotSupported.
        */
        virtual TInt HandleSimPbRespL(
        const TIsiReceiveC& /*aIsiMessage*/, TBool& /*aComplete*/ )
            {TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::HandleSimPbResp - Return KErrNotSupported");
            return KErrNotSupported; };


        /**
        * Basic implementation to handling request if operation not found in UICC.
        *
        * @param aComplete not used
        * @param aStatus not used
        * @param aFileData KErrNotSupported
        * @return KErrNotSupported.
        */
        
        virtual TInt HandleUICCPbRespL( TBool & /*complete*/, TInt /*aStatus*/, const TDesC8& /*aFileData*/, TInt /*aTransId*/)
            {TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::HandleUICCPbRespL - Return KErrNotSupported");
            return KErrNotSupported; 
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
            const CMmDataPackage* /*aDataPackage*/)
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
        static TUint8 ConvertToPBtypeUICC( const TName& aPBType );
        
        
        /**
        * Converts client phonebooktype to client phonebook name.
        *
        * @param aPBType
        * @return TUint8
        */
        void ConvertPBTypeToPbName(const TUint aPhonebookType, TName& aName );

        /**
        * Converts client phonebook to phonebook mask.
        *
        * @param aPBType
        * @return TUint8
        */
        void ConvertToPBname( const TUint8 aTrans, TName& aName );

        /**
        * Separate Phonebook Entry from UICC -message
        *
        * @param aFileData Received UICC message
        * @param aEntry Entry to be separated
        * @return KErrNone or error value.
        */
        TInt SeparatePhoneBookEntryFromUiccMsgL(
            const TDesC8& aFileData,
            TDes8& aNameBuf,
            TUint16 aSimPhonebookType);


        /**
        * Store Phonebook Entry from UICC -message
        *
        * @param aFileData Received UICC message
        * @param aEntry Entry to be separated
        * @return KErrNone or error value.
        */
        void StorePhonebookEntry(
                TDes8& aName,
                TDes8& aNumber,
                CPhoneBookStoreEntry& aEntry);

        /**
        * Gets transactionId.
        *
        * @param None
        * @return TransactionId.
        */
        TUint8 TransId();

        /**
        * Convert number of phonebook type to server's phonebook type
        *
        * @param aPbMask Phonebook mask
        * @return Server phonebook type.
        */
        TUint16 ConvertToSimPhoneBookType(
            const TUint8 aPbMask );


        /**
        * Gets the required parameter from FCI Data
        *
        * @param TDesC8 aFileData FCI Data
        * @param TInt aValue actual value of parameter to be read
        * @param TUint8 aParam parameter to be read
        * @return Error
        */
        TInt HandleFcpData( 
                const TDesC8 &aFileData,
                TInt& aValue,
                TUint8 aParam);


        /**
        * Handle FileData of Type1File Response
        * @param const TDes8& aFileData: UICC Message
        * @return TInt: KErrNone or error value.
        */
        void Get16bit(TInt& aTarget, const TDesC8& aSource, TInt aIndex);



    protected:



    private:

        /**
        * Converts operation mask to IPC
        *
        * @param aDestination Operation mask
        * @param aSource Client IPC
        * @return None
        */
        void ConvertOperationToClientIPCType(
            TInt& aDestination,
            const TUint8 aSource );


        // ConstructL

        void ConstructL();
        /**
        * Collects all needed data together.
        *
        * @param aSbStartOffSet Offset
        * @param aNumOfSubBlocks Number of subblocks
        * @param aIsiMessage ISI message
        * @param aEntry Phonebook entry
        * @param aEmailFound Flag indicate if email found or not
        * @param aAnrFound Flag indicate if anr found or not
        * @param aSneFound Flag indicate if sne found or not
        * @param aGrpFound Flag indicate if grp found or not
        * @return None
        */
        void CollectAvailableDataL(
            TUint& aSbStartOffSet,
            TInt& aNumOfSubBlocks,
            const TIsiReceiveC& aIsiMessage,
            CPhoneBookStoreEntry& aEntry,
            TBool& aEmailFound,
            TBool& aAnrFound,
            TBool& aSneFound
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            ,TBool& aGrpFound
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            );

        /**
        * Construct a SIM_READ_FIELD_REQ message and send it through phonet.
        *
        * @param aTransId Transaction Id
        * @return KErrNone / Error value from phonet
        */
        TInt SimReadFieldReq( TUint8 aTransId );

        /**
        * Breaks received SIM_READ_FIELD_RESP ISI message
        *
        * @param aIsiMessage Received ISI message
        * @param aComplete Indicates if request can remove from
        *        operationlist or not.
        * @return None
        */
        void SimReadFieldRespL(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );



    public:     // Data
        // None

    protected:  // Data

        // Pointer to CMmPhoneBookStoreMessHandler.
        CMmPhoneBookStoreMessHandler* iMmPhoneBookStoreMessHandler;
        
        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

        // TransactionId that consist with phonebook type and transactionId.
        TUint8 iTransactionId;

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

        // Phonebooktype, ADN, FDN, SDN...
        TName iPhonebookType;

        // modifiable Store number if there is extension number also there 
        TBuf8<UICC_EF_MAX_NUM_LEN_WITH_EXT> iNumberBuf;
        
        // Store entry name
        TBuf8<UICC_EF_MAX_NAME_LEN> iNameBuf;


#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        // Holds information for AAS and GAS
        RMmCustomAPI::TAlphaStringParams iAlphaStringParams;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

    private:    // Data
        //none
    };

#endif // CMMPHONEBOOKSOPERATIONBASE_H

// End of file
