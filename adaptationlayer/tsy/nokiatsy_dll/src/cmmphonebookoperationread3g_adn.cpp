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
* Description:   ?Description
*
*/

// INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include <pn_const.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tsylogger.h"
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationread3g_adn.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationreadTraces.h"
#include "OstTraceDefinitions.h"
#include "cmmphonebookoperationread3g_adnTraces.h"
#endif


// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
const TUint8 KAnrOffsetNumLength = 1;  // refer from 3GPP TS 31.102 document
const TUint8 KAnrOffsetNumber = 2;
const TUint8 KMinLengthEfAnr = 15;
const TUint8 KMaxEmptyPatternBuf = 243;

// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// ==================== LOCAL FUNCTIONS =====================================
    // None


// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn
// C++ default CMmPhoneBookOperationRead3g_adn can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn()
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn");
    OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CMMPHONEBOOKOPERATIONREAD3G_ADN_TD, "CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn" );
    }

// -----------------------------------------------------------------------------
//CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn C++ destructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationRead3g_adn::~CMmPhoneBookOperationRead3g_adn
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::~CMmPhoneBookOperationRead3g_adn");
    OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_CMMPHONEBOOKOPERATIONREAD3G_ADN_TD, "CMmPhoneBookOperationRead3g_adn::~CMmPhoneBookOperationRead3g_adn" );
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::NewL
// Creates a new CMmPhonebookOperationRead object instance.
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationRead3g_adn* CMmPhoneBookOperationRead3g_adn::NewL
    (
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aUiccMessHandler,
            const CMmDataPackage* aDataPackage,
            TInt aIpc
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::NewL");
    OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_NEWL_TD, "CMmPhoneBookOperationRead3g_adn::NewL" );


    CMmPhoneBookOperationRead3g_adn* mmPhoneBookOperationRead3g_adn =
        new( ELeave ) CMmPhoneBookOperationRead3g_adn();

    if( ( EMmTsyONStoreReadSizeIPC != aIpc )&&
        ( EMmTsyONStoreWriteSizeIPC != aIpc ) )
        {
        TName phonebookTypeName;

        const CPhoneBookDataPackage* phoneBookData =
            static_cast<const CPhoneBookDataPackage*>( aDataPackage );

        phoneBookData->GetPhoneBookName( phonebookTypeName );
        
        // Store phoneBook name 
        mmPhoneBookOperationRead3g_adn->iPhoneBookTypeName = phonebookTypeName;
        }

    mmPhoneBookOperationRead3g_adn->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;
    mmPhoneBookOperationRead3g_adn->iMmUiccMessHandler = aUiccMessHandler;
    
    mmPhoneBookOperationRead3g_adn->ConstructL();
    
    return mmPhoneBookOperationRead3g_adn;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead3g_adn::ConstructL
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::ConstructL");
    OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CONSTRUCTL_TD, "CMmPhoneBookOperationRead3g_adn::ConstructL" );
    
    iLocationSearch = EFalse;
    iCurrentType1Ef = 0;
    iCurrentType2Ef = 0;
    iLocationSearch = EFalse;
    iNumOfEntriesFilled = 0;
    iNumOfEntriesToRead = 0;
    iIndexToRead = 0;

    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UICCCreateReq
// Create request to read Entry
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UICCCreateReq
    (
    TInt aIpc,
    const CMmDataPackage* aDataPackage,
    TUint8 aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UICCCreateReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCCREATEREQ_TD, "CMmPhoneBookOperationRead3g_adn::UICCCreateReq" );

    TInt ret( KErrNotSupported );
    
    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    iSavedIPCForComplete = aIpc;

    // Convert Phone Book name to file id
    iExtFileId = UICC_ILLEGAL_FILE_ID;
    iFileId = ConvertToPBfileId(
                            iPhoneBookTypeName,
                            iExtFileId,
                            iMmUiccMessHandler->GetCardType() );
    iArrayIndex = ConvertToConfArrayIndex( iFileId );

    
    switch( aIpc )
        {
        case EMmTsyONStoreReadIPC:
        case EMmTsyONStoreReadEntryIPC:
            {
            iFileId = PB_MSISDN_FID;
            iExtFileId = PB_EXT5_FID;
            iArrayIndex = EPhonebookTypeMSISDN;
            // Unpack index to be read
            aDataPackage->UnPackData( iIndexToRead );
            ret = USimPbReqRead( iIndexToRead, aTransId );
            break;
            }
        case EMmTsyPhoneBookStoreReadIPC:
            {
            CArrayPtrSeg<CPhoneBookStoreEntry>* prtToReadEntriesArray;
            RMobilePhoneBookStore::TPBIndexAndNumEntries* ptrToIndexAndEntries;
            phoneBookData->UnPackData(
                ptrToIndexAndEntries,
                prtToReadEntriesArray );

            if ( prtToReadEntriesArray && ptrToIndexAndEntries )
                {
                iPhoneBookStoreCacheArray = prtToReadEntriesArray;
                iNumOfEntriesToRead = ptrToIndexAndEntries->iNumSlots;
                iNumOfEntriesFilled = 0;
                iIndexToRead = ptrToIndexAndEntries->iIndex;

                if( PB_ADN_FID == iFileId )
                    {
                    // handle for ADN 3g phoneBook
                    ret = UICCHandleData3gADNReadReq( aDataPackage,
                                                      aTransId );
                    }
                else
                    {
                    ret = USimPbReqRead( iIndexToRead, aTransId );
                    }
                }
            break;
            }
        case EMmTsyONStoreReadSizeIPC:
        case EMmTsyONStoreWriteSizeIPC:
            {
            ret = USimReadWriteSizeReq( aTransId );
            break;
            }
        default:
            {
            // Nothing to do here
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead3g_adn::UICCCreateReq - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL,  DUP2_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCCREATEREQ_TD, "CMmPhoneBookOperationRead3g_adn::UICCCreateReq - UnKnown Ipc : %d", aIpc );
            break;
            }
        } // switch-case

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq
// Handles Read Resp
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq
        (
         const CMmDataPackage* aDataPackage,
         TUint8 aTransId         
        )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCHANDLEDATA3GADNREADREQ_TD, "CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq" );

    TInt ret(KErrNone);

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    CArrayPtrSeg<CPhoneBookStoreEntry>* prtToReadEntriesArray;
    RMobilePhoneBookStore::TPBIndexAndNumEntries* ptrToIndexAndEntries;
    phoneBookData->UnPackData(
        ptrToIndexAndEntries,
        prtToReadEntriesArray );

    if ( prtToReadEntriesArray && ptrToIndexAndEntries )
        {
        iPhoneBookStoreCacheArray = prtToReadEntriesArray;
        iNumOfEntriesToRead = ptrToIndexAndEntries->iNumSlots;
        iNumOfEntriesFilled = 0;
        iIndexToRead = ptrToIndexAndEntries->iIndex;
        
        if( ( iIndexToRead <= iMmPhoneBookStoreMessHandler->
                         iPBStoreConf[EPhonebookTypeAdn].iNoOfRecords ) &&
            ( 0 < iIndexToRead ) )
            {
            ret = iMmPhoneBookStoreMessHandler->GetPBRRecordNum( iIndexToRead, 
                                                                 iCurrentPBRRecordNo);
            }
        else
            {
            // Start Reading from first location , 1st PBR Record
            iCurrentPBRRecordNo = 1;
            }

        if( KErrNone == ret )
            {
            ret = UiccReadEfRecordReq(
                         aTransId,
                         PB_PBR_FID,
                         UICC_SFI_NOT_PRESENT,
                         iCurrentPBRRecordNo );
            }
        }
    return ret;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UiccReadEfRecordReq
// Reads record from EFpbr
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UiccReadEfRecordReq( 
    const TInt aTraId, 
    const TUint16 aFileId, 
    const TUint8 aFileIdSfi,
    const TUint8 aRecordNo )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfRecordReq");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFRECORDREQ_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfRecordReq" );

    TUiccReadLinearFixed params;
    params.messHandlerPtr =
             static_cast<MUiccOperationBase*>( iMmPhoneBookStoreMessHandler );
    params.fileId = aFileId;
    params.fileIdSfi = aFileIdSfi;
    params.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    params.record = aRecordNo;
    params.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    params.filePath.Append( static_cast<TUint8>( MF_FILE ));
    params.filePath.Append( static_cast<TUint8>( APPL_FILE_ID >> 8 ));
    params.filePath.Append( static_cast<TUint8>( APPL_FILE_ID ));
    params.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    params.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    params.trId = static_cast<TUiccTrId>( aTraId );

    return iMmPhoneBookStoreMessHandler->UiccMessHandler()->
                                          CreateUiccApplCmdReq( params );
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::HandleUICCPbRespL
// Separate request
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationRead3g_adn::HandleUICCPbRespL
    (
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8 &aFileData,
    TInt aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::HandleUICCPbRespL");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_HANDLEUICCPBRESPL_TD, "CMmPhoneBookOperationRead3g_adn::HandleUICCPbRespL" );
    
    TBool completed( EFalse );
    TInt ret( KErrNone );
    // Handle possible error case all PhoneBooks extcept Adn
    if ( UICC_STATUS_OK != aStatus )
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::HandleUICCPbRespL-Unsuccessfully completed by UICC");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPHONEBOOKOPERATIONREAD3G_ADN_HANDLEUICCPBRESPL_TD, "CMmPhoneBookOperationRead3g_adn::HandleUICCPbRespL - Unsuccessfully completed by UICC" );
        
        ret = CMmStaticUtility::UICCCSCauseToEpocError(aStatus );
        }
    switch( iSavedIPCForComplete )
        {
        case EMmTsyPhoneBookStoreReadIPC:
        case EMmTsyONStoreReadIPC:
        case EMmTsyONStoreReadEntryIPC:
            {
            if( PB_ADN_FID == iFileId )
                {
                completed = HandleUICC3gADNRespL( aStatus,
                                                  aFileData,
                                                  aTransId );
                }
            else
                {
                completed = USimPbReadRespL( ret, 
                                    aTransId,
                                    aFileData );
                }
            break;
            }
        case EMmTsyONStoreReadSizeIPC:
        case EMmTsyONStoreWriteSizeIPC:
            {
            completed = USimReadWriteSizeResp( aFileData, ret );
            break;
            }
        default:
            {
            // Nothing to do here
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead3g_adn::HandleUICCPbRespL - Unknown IPC: %d", iSavedIPCForComplete);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_HANDLEUICCPBRESPL_TD, "CMmPhoneBookOperationRead3g_adn::HandleUICCPbRespL - UnKnown Ipc : =%d", iSavedIPCForComplete );
            break;
            }
        }
    return completed;

    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL
// HAndles Response for ADN phoneBook
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL
    (
    const TInt aStatus,
    const TDesC8 &aFileData,
    const TInt aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCHANDLEDATA3GADNREADREQ_TD, "CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq" );

    TBool completed ( EFalse );
    TInt ret(KErrNone);
    
    switch( iCurrentReadPhase )
        {
        case EPBReadPhase_PBR_Read_Entry:
            {
            ret = UiccReadEfPbrRecordResp( aStatus, 
                                           aFileData, 
                                           aTransId );

            break;
            }
        case EPBReadPhase_Read_Type1_Entry:
            {
            ret = UiccReadEfType1RespL( aStatus, 
                                       aTransId,
                                       aFileData );

            break;
            }
        case EPBReadPhase_Read_Type2_Entry:
            {
            ret = UiccReadEfType2RespL( aStatus, 
                                       aTransId,
                                       aFileData );

            break;
            }
        case EPBReadPhase_Read_type3_Entry:
            {
            TUint8 fileTag;
            if( iType2OperationOngoing )
                {
                fileTag = iType2FileArray[iCurrentType2Ef].tagValue;
                }
            else
                {
                fileTag = iType1FileArray[iCurrentType1Ef].tagValue;
                }
            ret = UiccReadEfType3RespL( aStatus, 
                                       aTransId,
                                       fileTag,
                                       aFileData );
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL - default branch ");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_HANDLEUICC3GADNRESPL_TD, "CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL - default branch " );
            break;
            }
        }

    if( ( EPB_3G_ADN_Read_Phase_complete == iCurrentReadPhase ) &&
        ( 0 != iNumOfEntriesToRead ) &&
        ( KErrNone == ret ) )
        {
        iIndexToRead++;
        iCurrentEfEntryIndex++;
        
        // Calculate The PBR Record Number needs to be read
        // get the Max no of Entries in one PBR record
        TInt maxNoOfEntry( iMmPhoneBookStoreMessHandler->
                iPBStoreConf[EPhonebookTypeAdn].iNoOfRecords  );

        TInt maxNoOfPbrRec( iMmPhoneBookStoreMessHandler->
                iPBStoreConf[EPhonebookTypeAdn].iPBRNoOfRecords );

        if( iIndexToRead > ( iCurrentPBRRecordNo * ( maxNoOfEntry/maxNoOfPbrRec ) ) )
            {
            // Read Next PBR Record 
            iCurrentPBRRecordNo++;
            if( iCurrentPBRRecordNo <= maxNoOfPbrRec )
                {
                // Reset all Buffers
                iType1FileArray.Reset();
                iType2FileArray.Reset();
                iType3FileArray.Reset();
                
                iCurrentReadPhase = EPBReadPhase_PBR_Read_Entry;
                ret = UiccReadEfRecordReq(
                             aTransId,
                             PB_PBR_FID,
                             UICC_SFI_NOT_PRESENT,
                             iCurrentPBRRecordNo );
                }
            else
                {
                // complete with Error
                ret = KErrArgument;
                iNumOfEntriesToRead = 0;
                }
            }
        else
            {
            // Start Reading next Entry
            ret = StartReadingEntry( aTransId );
            }
        }
    
    if( ( KErrNone != ret ) ||
        ( EPB_3G_ADN_Read_Phase_complete == iCurrentReadPhase ) )
        {
        // Check fo Any Entries Found
        if( ( UICC_STATUS_OK == aStatus )&& ( 0 == iNumOfEntriesFilled ) )
            {
            ret = KErrNotFound;
            }
        
        CPhoneBookDataPackage phoneBookData;
        phoneBookData.SetPhoneBookName( iPhoneBookTypeName );
        iNumOfEntriesFilled = 0;
        iPhoneBookStoreCacheArray = NULL;
        iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
                EMmTsyPhoneBookStoreReadIPC,
                &phoneBookData,
                ret );

        // Set flag to indicate that we can remove this operation from array
        completed = ETrue;
        }

    return completed;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UiccReadEfPbrRecordResp
// Handles response for EFpbr record reading
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UiccReadEfPbrRecordResp( 
    TInt aStatus, 
    const TDesC8 &aFileData, 
    TInt aTraId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfPbrRecordResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFPBRRECORDRESP_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfPbrRecordResp" );

    TInt ret( KErrArgument );
    if( UICC_STATUS_OK == aStatus )
        {
        ret = FetchFileListFromPBR(
                           aFileData,
                           KTagConstructedTagA8,
                           iType1FileArray );
        ret = FetchFileListFromPBR(
                           aFileData,
                           KTagConstructedTagA9,
                           iType2FileArray );
        ret = FetchFileListFromPBR(
                           aFileData,
                           KTagConstructedTagAA,
                           iType3FileArray );
        
        ret = StartReadingEntry( aTraId );
        }
    else
        {
TFLOGSTRING2("TSY: CMmPhoneBookOperationWrite3g_adn::UiccReadEfPbrRecordResp: file reading fails: 0x%x", aStatus );
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFPBRRECORDRESP_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfPbrRecordResp;file reading fails: =%d", aStatus );
        
        iNumOfEntriesToRead = 0;
        }
    return ret;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UiccReadEfType1RespL
// Handles response for Type1 File Array record reading
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UiccReadEfType1RespL(
        TInt aStatus, 
        TInt aTraId,
        const TDesC8 &aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfType1RespL");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE1RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType1RespL" );

     TInt ret ( KErrNone );
     
     CPhoneBookStoreEntry* phoneBookStoreEntry( NULL );
     TBool entryAllocatedNow( EFalse ); // Cache entry allocated now or earlier
     TBool entryStored( EFalse ); // Is entry data stored in cache
     TBool notEmptyEntry( ETrue ); // If Entryis Empty

     if( UICC_STATUS_OK == aStatus )
         {
         CreateInternalPbEntryL( iIndexToRead );
         }
     
     notEmptyEntry = CheckForEmptyEntry( iType1FileArray[iCurrentType1Ef].tagValue,
             aFileData );
     if( notEmptyEntry )
         {
         phoneBookStoreEntry = CreateReadPbEntryL(
             iIndexToRead,
             entryAllocatedNow );
         }
     
     if( UICC_STATUS_OK == aStatus )
         {
         switch( iType1FileArray[iCurrentType1Ef].tagValue )
             {
             case UICC_EF_ADN_TAG: 
                 {
                 // Get name and number
                 iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeAdn].
                 GetPBEntryFromUICCData( aFileData, iNumberBuf, iNameBuf );

                 // Check if there is Any Ext records
                 TInt index( iMmPhoneBookStoreMessHandler->
                         iPBStoreConf[EPhonebookTypeAdn].iAlphaStringlength
                         + KAdnMandatoryBytes );

                 TInt extFileTagIndex( KErrNotFound );
                 iType2OperationOngoing = EFalse;
                 ret = CheckForExtToContinueNextRead(
                         index,
                         aFileData,
                         aTraId,
                         extFileTagIndex );

                 if( KErrNotFound == extFileTagIndex )
                     {
                     entryStored = ETrue;
                     iCurrentType1Ef++;

                     ret = ContinueWithNextReading( aTraId );
                     } // no else
                     
                 if ( entryStored && notEmptyEntry ) // No extension record, store entry
                     {
                     iMmPhoneBookStoreMessHandler->StorePhonebookEntryL(
                             iNameBuf,
                             iNumberBuf,
                             *phoneBookStoreEntry,
                             PB_ADN_FID,
                             iIndexToRead );

                     // Set status in internal list
                     iStoreEntry->iEntryPresent = ETrue;
                     iNumberBuf.Zero(); // Reset buffers for next time
                     iNameBuf.Zero();
                     }
                 break;
                 }
             case UICC_EF_ANR_TAG:
                 {
                 if ( aFileData.Length() && KMinLengthEfAnr <= aFileData.Length() )
                     {
                     // Get and save ANR
                     TInt numLength( aFileData[KAnrOffsetNumLength] );
                     iNumberBuf.Copy( aFileData.Mid( KAnrOffsetNumber, numLength ) );
                     }
                 
                 TInt extFileTagIndex( KErrNotFound );
                 iType2OperationOngoing = EFalse;
                 ret = CheckForExtToContinueNextRead(
                         KAnrExtRecIdOffset,
                         aFileData,
                         aTraId,
                         extFileTagIndex );
                 
                 if( KErrNotFound == extFileTagIndex )
                     {
                     entryStored = ETrue;
                     iCurrentType1Ef++;
                     ret = ContinueWithNextReading( aTraId );
                     }  // no else
                 
                 
                 if( entryStored && notEmptyEntry )
                     {
                     iMmPhoneBookStoreMessHandler->StoreAnrToPhonebookEntryL(
                             iNumberBuf,
                             *phoneBookStoreEntry,
                             PB_ADN_FID ); // Coding is same as in ADN

                     // Set status in internal list
                     iStoreEntry->iEntryPresent = ETrue;
                     
                     // Reset buffer for next time
                     iNumberBuf.Zero();
                     }  // NO ELSE
                 break;
                 }
             case UICC_EF_EMAIL_TAG:
             case UICC_EF_SNE_TAG:
                 {
                 // In type 1 file, EF SNE contains only alpha string
                 iNameBuf.Copy( aFileData );
                 if( notEmptyEntry )
                     {
                     iMmPhoneBookStoreMessHandler->StoreSneEmailToPbEntryL(
                         iNameBuf,
                         *phoneBookStoreEntry,
                         iType1FileArray[iCurrentType1Ef].tagValue );
                     // Set status in internal list
                     iStoreEntry->iEntryPresent = ETrue;
                     }

                 // reset name buffer to reuse next time
                 iNameBuf.Zero();

                 iCurrentType1Ef++;
                 ret = ContinueWithNextReading( aTraId );
                 break;
                 }
             case UICC_EF_IAP_TAG:
                 {
                 if( ( aFileData.Length() == iType2FileArray.Count() ) &&
                     ( notEmptyEntry ) )
                     {
                     for ( TInt i( 0 ); i < aFileData.Length() ; i++ )
                         {
                         TIapInfo iapinfo;
                         iapinfo.recordNo = aFileData[i];
                         iapinfo.fileTag = iType2FileArray[i].tagValue;
                         iapinfo.fileSfi = iType2FileArray[i].tagSFI;
                         iapinfo.fileId = iType2FileArray[i].tagFID;
                         
                         iStoreEntry->iapInfo.Append( iapinfo );
                         }
                     } // no else
                 
                 iCurrentType1Ef++;
                 ret = ContinueWithNextReading( aTraId );
                 break;
                 }
             default:
                 {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfType1RespL - default branch ");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE1RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType1RespL - default branch" );

                 break;
                 }
             }
         }
     else
         {
         // ADN Read failed for some reason so complete the
         // operation with error value
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite3g_adn::UiccReadEfType1RespL - Type1 File Read failed ");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE1RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType1RespL - Type1 file read failed" );

         iCurrentReadPhase = EPB_3G_ADN_Read_Phase_complete;
         ret = KErrArgument;
         }


         if ( entryAllocatedNow )
             {
             TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
             iPhoneBookStoreCacheArray->AppendL( phoneBookStoreEntry );
             CleanupStack::Pop( phoneBookStoreEntry );
             iNumOfEntriesFilled++;
             } // No else

         return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL
// Handles response for Type2 File Array record reading
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL(
        TInt aStatus, 
        TInt aTraId,
        const TDesC8 &aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE2RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL" );

     TInt ret ( KErrNone );
     
     CPhoneBookStoreEntry* phoneBookStoreEntry( NULL );
     TBool entryAllocatedNow( EFalse ); // Cache entry allocated now or earlier
     TBool entryStored( EFalse ); // Is entry data stored in cache
     TBool notEmptyEntry( ETrue ); // Entry is Empty

     if( UICC_STATUS_OK == aStatus )
         {
         CreateInternalPbEntryL( iIndexToRead );
         }

     notEmptyEntry = CheckForEmptyEntry( iType2FileArray[iCurrentType2Ef].tagValue,
             aFileData );
     if( notEmptyEntry )
         {
         phoneBookStoreEntry = CreateReadPbEntryL(
             iIndexToRead,
             entryAllocatedNow );
         }

     if( UICC_STATUS_OK == aStatus )
         {
         // Check for Filetag 
         switch( iType2FileArray[iCurrentType2Ef].tagValue )
             {
             case UICC_EF_ANR_TAG:
                 {
                 if ( aFileData.Length() && KMinLengthEfAnr <= aFileData.Length() )
                     {
                     // Get and save ANR
                     TInt numLength( aFileData[KAnrOffsetNumLength] );
                     iNumberBuf.Copy( aFileData.Mid( KAnrOffsetNumber, numLength ) );
                     }

                 TInt extFileTagIndex( KErrNotFound );
                 // Check if there is Any Ext records
                 iType2OperationOngoing = ETrue;
                 ret = CheckForExtToContinueNextRead(
                           KAnrExtRecIdOffset,
                           aFileData,
                           aTraId,
                           extFileTagIndex );
                 
                 if( KErrNotFound == extFileTagIndex )
                     {
                     entryStored = ETrue;
                     iCurrentType2Ef++;
                     ret = CheckForNextType2Read( aTraId );
                     } // no else
                 
                 if( entryStored && notEmptyEntry )
                     {
                     iMmPhoneBookStoreMessHandler->StoreAnrToPhonebookEntryL(
                         iNumberBuf,
                         *phoneBookStoreEntry,
                         PB_ADN_FID ); // Coding is same as in ADN

                     // Set status in internal list
                     iStoreEntry->iEntryPresent = ETrue;
                     // Reset buffer for next time
                     iNumberBuf.Zero();
                     }
                 break;
                 }
             case UICC_EF_EMAIL_TAG:
             case UICC_EF_SNE_TAG:
                 {
                 // In type 1 file, EF SNE contains only alpha string
                 iNameBuf.Copy( aFileData );
                 if( notEmptyEntry )
                     {
                     iMmPhoneBookStoreMessHandler->StoreSneEmailToPbEntryL(
                         iNameBuf,
                         *phoneBookStoreEntry,
                         iType2FileArray[iCurrentType2Ef].tagValue );
                     // Set status in internal list
                     iStoreEntry->iEntryPresent = ETrue;
                     }

                 // Reset name buffe rto recuse next time
                 iNameBuf.Zero();

                 iCurrentType2Ef++;
                 ret = CheckForNextType2Read( aTraId );
                 break;
                 }
             default:
                 {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL - default branch");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE2RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL - default branch" );
                 break;
                 }
             }
         }
     else
         {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL - Type2 File Read Failed");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE2RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL - Type2 file read failed" );

         iCurrentReadPhase = EPB_3G_ADN_Read_Phase_complete;
         ret = KErrArgument;
         }
     
     if ( entryAllocatedNow )
         {
         TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
         iPhoneBookStoreCacheArray->AppendL( phoneBookStoreEntry );
         CleanupStack::Pop( phoneBookStoreEntry );
         iNumOfEntriesFilled++;
         } // No else

     return ret;
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UiccReadEfType3RespL
// Handles response for Type3 File Array record reading
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UiccReadEfType3RespL(
        TInt aStatus, 
        TInt aTraId,
        TUint8 aFileTag,
        const TDesC8 &aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfType2RespL");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE3RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType3RespL" );
    
     TInt ret ( KErrNone );
     
     CPhoneBookStoreEntry* phoneBookStoreEntry( NULL );
     TBool entryAllocatedNow( EFalse ); // Cache entry allocated now or earlier

     if( UICC_STATUS_OK == aStatus )
         {
         CreateInternalPbEntryL( iIndexToRead );
         phoneBookStoreEntry = CreateReadPbEntryL(
             iIndexToRead,
             entryAllocatedNow );
         }

     
     if( UICC_STATUS_OK == aStatus )
         {
         // Store number , if Data length valid and Data type is Additional Data Type
         if( ( KExtRecordSize == aFileData.Length() ) &&
             ( KAdditionalNoType == aFileData[0] ) )
             {
             // Check if EXT Data Furthe has any ext records
             if( KTagUnusedbyte != aFileData[UICC_EXT_REC_NO_OFFSET] )
                 {
                 // Store Number upto last byte
                 iNumberBuf.Append(aFileData.Mid(1,UICC_EF_EXT_REC_NUM_LEN));

                 // read that Entry
                 TInt index (SearchForFileTagIndex(
                               UICC_EF_EXT1_TAG,
                               iType3FileArray,
                               0 ) );
                               
                 ret = UiccReadEfRecordReq(
                         aTraId,
                         iType3FileArray[index].tagFID,
                         iType3FileArray[index].tagSFI,
                         aFileData[UICC_EXT_REC_NO_OFFSET] );
                 }
             else
                 {
                 // Check for length upto which no is stored
                 TInt offset = aFileData.Find( &KTagUnusedbyte,1 );
                 // store Data
                 iNumberBuf.Append(aFileData.Mid(1,( offset - 1 )));

                 // Save to cache
                 if ( UICC_ADN_PRIM_TAG == aFileTag )
                     {
                     iMmPhoneBookStoreMessHandler->StorePhonebookEntryL(
                         iNameBuf,
                         iNumberBuf,
                         *phoneBookStoreEntry,
                         PB_ADN_FID,
                         iIndexToRead );
                     
                     // Reset Number and name buffer to reuse next time
                     iNumberBuf.Zero(); // Reset buffers for next time
                     iNameBuf.Zero();
                     }
                 else if ( UICC_ANR_PRIM_TAG == aFileTag )
                     {
                     iMmPhoneBookStoreMessHandler->StoreAnrToPhonebookEntryL(
                         iNumberBuf,
                         *phoneBookStoreEntry,
                         PB_ADN_FID ); // Same coding as in ADN
                     
                     // Reset number buffer to reuse next time
                     iNumberBuf.Zero();
                     }

                 if( iType2OperationOngoing )
                     {
                     iCurrentType2Ef++;
                     // Continue with Type2 File operations
                     ret = CheckForNextType2Read( aTraId );
                     }
                 else
                     {
                     iCurrentType1Ef++;
                     // Continue with next read phase
                     ret = ContinueWithNextReading( aTraId );
                     }
                 }
             } // no else
         }
     else
         {
         // EXT Reading fails for some reason complete the operation with
         // error value
TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UiccReadEfType3RespL - EXT Read fail");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCREADEFTYPE3RESPL_TD, "CMmPhoneBookOperationRead3g_adn::UiccReadEfType3RespL - EXT Read fail" );
         
         iCurrentReadPhase = EPB_3G_ADN_Read_Phase_complete;
         ret = KErrArgument;
         }
     
     if ( entryAllocatedNow )
         {
         TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
         iPhoneBookStoreCacheArray->AppendL( phoneBookStoreEntry );
         CleanupStack::Pop( phoneBookStoreEntry );
         iNumOfEntriesFilled++;
         } // No else


     return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationReadg_adn::GetNextType1File
// Get next Type1 valid FileId to be read
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::GetNextType1File()
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::GetNextType1File" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_GETNEXTTYPE1FILE_TD, "CMmPhoneBookOperationRead3g_adn::GetNextType1File" );
    
    TInt ret ( KErrNotFound );

    for(; iCurrentType1Ef < iType1FileArray.Count(); )
        {
        if( ( iType1FileArray[iCurrentType1Ef].tagValue == UICC_EF_PBC_TAG ) ||
                ( iType1FileArray[iCurrentType1Ef].tagValue == UICC_EF_UID_TAG )||
                ( iType1FileArray[iCurrentType1Ef].tagValue == UICC_EF_GRP_TAG ) )
            {
            iCurrentType1Ef++;
            }
        else
            {
            ret = KErrNone;
            break;
            }
        }
    return ret ;
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::CheckForExtToContinueNextRead(
// Start Delete Operation for present Entry
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::CheckForExtToContinueNextRead( 
        TUint8 aIndex, 
        const TDesC8 &aFileData, 
        TInt aTraId,
        TInt &aExtFileTagIndex )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::CheckForExtToContinueNextRead" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CHECKFOREXTTOCONTINUENEXTREAD_TD, "CMmPhoneBookOperationRead3g_adn::CheckForExtToContinueNextRead" );
    
    TInt ret ( KErrNone );

    if( KTagUnusedbyte != aFileData[aIndex] )
        {
        // Start reading Ext1 FileData
        iStoreEntry->PBEntryExtRecord.Append( aFileData[aIndex] );
        
        // Search For Ext1 File tag in iType3FileArray
        aExtFileTagIndex = SearchForFileTagIndex(
                               UICC_EF_EXT1_TAG,
                               iType3FileArray,
                               0 ) ;
                               
        if( KErrNotFound != aExtFileTagIndex )
            {
            iCurrentReadPhase = EPBReadPhase_Read_type3_Entry;
            // Send request to read Ext
            ret = UiccReadEfRecordReq(
                    aTraId,
                    iType3FileArray[aExtFileTagIndex].tagFID,
                    iType3FileArray[aExtFileTagIndex].tagSFI,
                    aFileData[aIndex] );
            } // no else
        } // no else
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::ContinueWithNextReading(
// Start Delete Operation for present Entry
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::ContinueWithNextReading( TUint8 aTraId )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::ContinueWithNextReading" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CONTINUEWITHNEXTREADING_TD, "CMmPhoneBookOperationRead3g_adn::ContinueWithNextReading" );
    
    TInt ret( KErrNone );
    if( KErrNone == GetNextType1File() )
        {
        iType2OperationOngoing = EFalse;
        iCurrentReadPhase = EPBReadPhase_Read_Type1_Entry;
        ret =  UiccReadEfRecordReq( 
                  aTraId, 
                  iType1FileArray[iCurrentType1Ef].tagFID, 
                  iType1FileArray[iCurrentType1Ef].tagSFI,
                  iCurrentEfEntryIndex );
        }
    else
        {
        iCurrentType2Ef  = 0;
        // Check if there is any type 2 File Data present
        if( ( iCurrentType2Ef < iType2FileArray.Count() ) &&
                (  KErrNotFound != GetNextUsedType2FileRecord( iCurrentType2Ef ) ) )
            {
            iType2OperationOngoing = ETrue;
            // Start reading Type2Files
            // Continue with next Type 2 File reading
            iCurrentReadPhase = EPBReadPhase_Read_Type2_Entry;
            ret =  UiccReadEfRecordReq( 
                    aTraId, 
                    iType2FileArray[iCurrentType2Ef].tagFID, 
                    iType2FileArray[iCurrentType2Ef].tagSFI,
                    iStoreEntry->iapInfo[iCurrentType2Ef].recordNo );
            }
        else
            {
            if( !iLocationSearch )
                {
                iNumOfEntriesToRead--;
                }

            iCurrentReadPhase = EPB_3G_ADN_Read_Phase_complete;
            }
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::StartReadingEntry
// STart reading Entry
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::StartReadingEntry( TUint8 aTransId )
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::StartReadingEntry" );
    OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_STARTREADINGENTRY_TD, "CMmPhoneBookOperationRead3g_adn::StartReadingEntry" );

    TInt ret( KErrNone );
    
    // Check if index is valid index or Entry to Written on first free entry
    if( iIndexToRead <= iMmPhoneBookStoreMessHandler->
                              iPBStoreConf[EPhonebookTypeAdn].iNoOfRecords )
        {
        iType2OperationOngoing = EFalse;
        // Start reading The present Entry
        iCurrentReadPhase = EPBReadPhase_Read_Type1_Entry;
        iCurrentType1Ef = 0;

        if( 0 < iIndexToRead )
            {
            ret = iMmPhoneBookStoreMessHandler->GetCurrentEfRecNum( 
                                                     iCurrentPBRRecordNo, 
                                                     iCurrentEfEntryIndex, 
                                                     iIndexToRead );
            
            if( ( KErrNone == GetNextType1File() ) &&
                ( KErrNone == ret ) )
                {
                ret =  UiccReadEfRecordReq( 
                        aTransId, 
                        iType1FileArray[iCurrentType1Ef].tagFID, 
                        iType1FileArray[iCurrentType1Ef].tagSFI,
                        iCurrentEfEntryIndex );
                }  // no else
            }
        else
            {
            iLocationSearch = ETrue;
            // Read First Entry
            iIndexToRead  = 1;
            iCurrentEfEntryIndex = 1;
            
            if( KErrNone == GetNextType1File() )
                {
                ret =  UiccReadEfRecordReq( 
                          aTransId, 
                          iType1FileArray[iCurrentType1Ef].tagFID, 
                          iType1FileArray[iCurrentType1Ef].tagSFI,
                          iCurrentEfEntryIndex );
                }  // no else
            }
        }

    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache3G_adn::CreateInternalPbEntryL
//
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead3g_adn::CreateInternalPbEntryL(
    const TInt aLocation )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3G_adn::CreateInternalPbEntryL" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CREATEINTERNALPBENTRYL_TD, "CMmPhoneBookOperationRead3g_adn::CreateInternalPbEntryL" );

    // Check if this entry was already stored in internal list
    iStoreEntry = iMmPhoneBookStoreMessHandler->FindEntryFromPbList(
        EPhonebookTypeAdn,
        aLocation );
    if( ! iStoreEntry )
        {
        // Add new entry to internal list
        iStoreEntry = new ( ELeave ) TPBEntry();
        iStoreEntry->iEntryIndex = iIndexToRead;
        // Not present by default. if some data for this record is found,
        // it is set as present
        iStoreEntry->iEntryPresent = EFalse;
        iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
            iStoreEntry,
            EPhonebookTypeAdn );
        } // No else
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache3G_adn::CreateReadPbEntryL
//
// -----------------------------------------------------------------------------
//
CPhoneBookStoreEntry* CMmPhoneBookOperationRead3g_adn::CreateReadPbEntryL(
    const TInt aLocation, TBool& aAllocatedNow )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::CreateReadPbEntryL" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CREATEREADPBENTRYL_TD, "CMmPhoneBookOperationRead3g_adn::CreateReadPbEntryL" );

    aAllocatedNow = ETrue;
    CPhoneBookStoreEntry* phoneBookStoreEntry( NULL );
    // Check is there already created CPhoneBookStoreEntry in
    // iPhoneBookStoreCacheArray.
    for ( TInt i ( 0 ); i < iPhoneBookStoreCacheArray->Count(); i++ )
        {
        phoneBookStoreEntry = iPhoneBookStoreCacheArray->At( i );
        if ( phoneBookStoreEntry &&
            ( phoneBookStoreEntry->iLocation == aLocation ) )
            {
            aAllocatedNow = EFalse;
            break;
            }
        // No else
        }

    if ( aAllocatedNow ) // Need to create a new entry
        {
        phoneBookStoreEntry = new( ELeave ) CPhoneBookStoreEntry;
        CleanupStack::PushL( phoneBookStoreEntry );
        phoneBookStoreEntry->ConstructL();
        phoneBookStoreEntry->iLocation = aLocation;
        }
    // No else

    return phoneBookStoreEntry;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::SearchForFileTagIndex
// Get the index for ADN file Tag
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::SearchForFileTagIndex(
            TUint8 aFileTag,
            RArray <TPrimitiveTag>& aFileList,
            TUint8 aOffset )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::SearchForFileTagIndex" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_SEARCHFORFILETAGINDEX_TD, "CMmPhoneBookOperationRead3g_adn::SearchForFileTagIndex" );
    
    TInt index( KErrNotFound );
    for( TInt count(0); count<aFileList.Count(); count++ )
        {
        if( ( aFileTag == aFileList[count].tagValue ) &&
            ( aOffset <= count ) )
            {
            index = count;
            break;
            }  // no else
        }
    return( index );
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::CheckForNextType2Read(
// Check for next Type 1 Read
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::CheckForNextType2Read( TInt aTraId )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::CheckForNextType2Read" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CHECKFORNEXTTYPE2READ_TD, "CMmPhoneBookOperationRead3g_adn::CheckForNextType2Read" );
    
    TInt ret ( KErrNone );
    // Check if there is any type 2 File Data present
    if( ( iCurrentType2Ef < iType2FileArray.Count() ) &&
        ( KErrNotFound != GetNextUsedType2FileRecord( iCurrentType2Ef ) ) )
        {
        iType2OperationOngoing = ETrue;
        // Start reading Type2Files
        // Continue with next Type 2 File reading
        iCurrentReadPhase = EPBReadPhase_Read_Type2_Entry;
        ret =  UiccReadEfRecordReq( 
                aTraId, 
                iType2FileArray[iCurrentType2Ef].tagFID, 
                iType2FileArray[iCurrentType2Ef].tagSFI,
                iStoreEntry->iapInfo[iCurrentType2Ef].recordNo );
        }
    else
        {
        if( !iLocationSearch )
            {
            iNumOfEntriesToRead--;
            }

        iCurrentReadPhase = EPB_3G_ADN_Read_Phase_complete;
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::GetNextUsedType2FileRecord
// Start Delete Operation for present Entry
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::GetNextUsedType2FileRecord( TInt &aOffset )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::GetNextUsedType2FileRecord" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_GETNEXTUSEDTYPE2FILERECORD_TD, "CMmPhoneBookOperationRead3g_adn::GetNextUsedType2FileRecord" );
    
    TInt index( KErrNotFound );
    if( iStoreEntry->iapInfo.Count() == iType2FileArray.Count() )
        {
        for( TInt count( aOffset ); count < iType2FileArray.Count(); count++ )
            {
            if( KTagUnusedbyte != iStoreEntry->iapInfo[count].recordNo )
                {
                aOffset = count;
                index = KErrNone;
                break;
                }  // no else
            }
        }
    return index;
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3G_adn::UiccGetSearchPattern
// resolves P2 and search data for command SEARCH RECORD depending on file type
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationRead3g_adn::CheckForEmptyEntry(
    const TUint8 aFileType,
    const TDesC8 &aFileData )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::CheckForEmptyEntry" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_CHECKFOREMPTYENTRY_TD, "CMmPhoneBookOperationRead3g_adn::CheckForEmptyEntry" );

    TBool notEmpty( ETrue );
    TBuf8<KMaxEmptyPatternBuf> emptyData;
    GetEmptyRecordPattern( aFileType,
                           emptyData );
    for( TInt count(0); count < emptyData.Length(); count++ )
        {
        if( aFileData[count] == emptyData[count] )
            {
            notEmpty = EFalse;
            break;
            }
        }
    return notEmpty;
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3G_adn::UiccGetSearchPattern
// resolves P2 and search data for command SEARCH RECORD depending on file type
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead3g_adn::GetEmptyRecordPattern(
    const TUint8 aFileType,
    TDes8& aEmptyPattern )
    {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::GetEmptyRecordPattern" );
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONREAD3G_ADN_GETEMPTYRECORDPATTERN_TD, "CMmPhoneBookOperationRead3G_adn::GetEmptyRecordPattern" );

    switch( aFileType )
        {
        case UICC_ADN_PRIM_TAG:
            {
            TInt alphaStringLength( iMmPhoneBookStoreMessHandler->
                iPBStoreConf[EPhonebookTypeAdn].iAlphaStringlength );
            // If aplha string field is included in entry, search pattern is
            // all alpa bytes 'FF' and number length 0 and TON&NPI 'FF'
            if ( alphaStringLength )
                {
                aEmptyPattern.AppendFill( KTagUnusedbyte, alphaStringLength );
                aEmptyPattern.Append( 0x00 ); // Search for number length zero
                aEmptyPattern.Append( KTagUnusedbyte ); // and TON&NPI 'FF'
                }
            // Alpha string does not exists, search pattern is for empty number
            else
                {
                aEmptyPattern.Append( 0x00 ); // Search for number length zero
                aEmptyPattern.Append( KTagUnusedbyte ); // and TON&NPI FF
                }
            break;
            }
        case UICC_IAP_PRIM_TAG:
            {
            // Number of bytes in EFiap is same as file amount
            // in EFpbr list with tag "A9". In other words, count
            // of type 2 files
            for( TInt i( 0 ); i < iType2FileArray.Count(); i++ )
                {
                aEmptyPattern.Append( 0x00 );
                }
            break;
            }
        case UICC_ANR_PRIM_TAG:
            {
            aEmptyPattern.Append( KTagUnusedbyte ); // Check for AAS record there is none
            aEmptyPattern.Append( 0x00 ); // For SSC/ BCD Number Length 0
            aEmptyPattern.Append( KTagUnusedbyte ); //TON&NPI FF
            aEmptyPattern.Append( KTagUnusedbyte ); // SSC/BCD forst Byte is FF
            break;
            }
        case UICC_SNE_PRIM_TAG:
        case UICC_EMAIL_PRIM_TAG:
            {
            // If first byte of the EF record is 0xFF, the record
            // is empty
            aEmptyPattern.Append( KTagUnusedbyte );
            break;
            }
        default:
            {
TFLOGSTRING( "TSY: CMmPhoneBookOperationRead3g_adn::GetEmptyRecordPattern: default" );
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_GETEMPTYRECORDPATTERN_TD, "CMmPhoneBookOperationRead3g_adn::GetEmptyRecordPattern : default" );
            break;
            }
        }
    }

