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

//  INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include <pn_const.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tsylogger.h"
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationdelete.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationdeletetraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS


// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
const TUint8 KExtensionDataBytes( 13 );
const TUint8 KEfAdnDataBytes( 14 );
const TUint8 KMaxAlphaStringBytes( 241 );

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// ==================== LOCAL FUNCTIONS =====================================
    // None


// ================= MEMBER FUNCTIONS =======================================

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::CMmPhoneBookOperationDelete
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMmPhoneBookOperationDelete::CMmPhoneBookOperationDelete()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::CMmPhoneBookOperationDelete");
OstTrace0( TRACE_NORMAL,  CMmPhoneBookOperationDelete_CMmPhoneBookOperationDelete_TD, "CMmPhoneBookOperationDelete::CMmPhoneBookOperationDelete" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::~CMmPhoneBookOperationDelete
// C++ destructor.
// ---------------------------------------------------------------------------
//
CMmPhoneBookOperationDelete::~CMmPhoneBookOperationDelete()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::~CMmPhoneBookOperationDelete");
OstTrace0( TRACE_NORMAL,  DUP1_CMmPhoneBookOperationDelete_CMmPhoneBookOperationDelete_TD, "CMmPhoneBookOperationDelete::~CMmPhoneBookOperationDelete" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::NewL
// Creates a new CMmPhoneBookOperationDelete object instance.
// Two-phased constructor.
// ---------------------------------------------------------------------------

CMmPhoneBookOperationDelete* CMmPhoneBookOperationDelete::NewL(
    CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
    CMmUiccMessHandler* aUiccMessHandler,
    const CMmDataPackage* aDataPackage ) // Data
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::NewL");
OstTrace0( TRACE_NORMAL,  CMmPhoneBookOperationDelete_NEWL_TD, "CMmPhoneBookOperationDelete::NewL" );

    CMmPhoneBookOperationDelete* mmPhoneBookOperationDelete =
        new( ELeave ) CMmPhoneBookOperationDelete();

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    // Store phonebook name
    TName phonebookTypeName;
    phoneBookData->GetPhoneBookName( phonebookTypeName );
    mmPhoneBookOperationDelete->iPhoneBookTypeName = phonebookTypeName;

    mmPhoneBookOperationDelete->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;

    mmPhoneBookOperationDelete->iMmUiccMessHandler = aUiccMessHandler;

    mmPhoneBookOperationDelete->iNumOfExtensions = 0;
    mmPhoneBookOperationDelete->iFileIdExt = 0;
    mmPhoneBookOperationDelete->iIndex = 0;
    mmPhoneBookOperationDelete->iEntry.iEntryIndex = 0;
    mmPhoneBookOperationDelete->iEntry.iEntryPresent = EFalse;
    mmPhoneBookOperationDelete->iTransId = 0xFF;
    mmPhoneBookOperationDelete->iArrayIndex = 0;
    mmPhoneBookOperationDelete->iLocationFoundInPbList = EFalse;
    mmPhoneBookOperationDelete->iExtRecordArrayToBeDelete.Reset();
    mmPhoneBookOperationDelete->iNumOfEntries = 0;

    return mmPhoneBookOperationDelete;
    }

// ---------------------------------------------------------------------------
// TInt CMmPhoneBookOperationDelete::UICCCreateReq
// Separate requests
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UICCCreateReq(
    TInt aIpc,
    const CMmDataPackage* aDataPackage,
    TUint8 aTransId )
    {
TFLOGSTRING2("TSY: CMmPhoneBookOperationDelete::UICCCreateReq Ipc: %d", aIpc);
OstTraceExt1( TRACE_NORMAL,  DUP1_CMmPhoneBookOperationDelete_UICCCREATEREQ_TD, "CMmPhoneBookOperationDelete::UICCCreateReq;aIpc=%hd", aIpc );

    TInt ret( KErrNotSupported );
    iIpc = aIpc;
    iTransId = aTransId;
    // Convert phone book name to file id
    iFileId = ConvertToPBfileId(
        iPhoneBookTypeName,
        iFileIdExt,
        iMmUiccMessHandler->GetCardType() );
    // Find location from internal array
    iArrayIndex = ConvertToConfArrayIndex( iFileId );

    const CPhoneBookDataPackage* phoneBookData(
        static_cast<const CPhoneBookDataPackage*>( aDataPackage ) );

    switch( aIpc )
        {
        case EMmTsyPhoneBookStoreDeleteIPC:
        case EMmTsyONStoreDeleteIPC:
            {
            phoneBookData->UnPackData( iIndex );

            if( PB_MBDN_FID == iFileId )
                {
                // Store MBI Profile
                iMBIProfileType = iIndex;
                // For MBDN PhoneBook first read MBI file
                // Check if the mailbox inidcation type is correct
                if( iIndex < iMmPhoneBookStoreMessHandler->
                        iPBStoreConf[iArrayIndex].iMbiRecLen )
                    {
                    iCurrentDeletePhase = EPBDeletePhase_Read_MBI_profile;
                    // read MBDN record number from MBI first record Profile number
                    ret = UiccPbReqReadMBI();
                    }
                else
                    {
                    ret = KErrArgument;
                    }
                }
            else
                {
                ret = UiccPbReqDelete();
                }
            break;
            }
        case EMmTsyPhoneBookStoreDeleteAllIPC:
        case EMmTsyONStoreDeleteAllIPC:
            {
            if( PB_MBDN_FID != iFileId)
                {
                iNumOfEntries = iMmPhoneBookStoreMessHandler->
                    iPBStoreConf[iArrayIndex].iNoOfRecords;
                if ( iNumOfEntries )
                    {
                    // Start to delete entries from the last one
                    iIndex = iNumOfEntries;
                    ret = UiccPbReqDelete();
                    iNumOfEntries--;
                    }
                }
            else
                {
                // For first Profile Type Read
                iMBIProfileType = 0;
                iIndex = iMBIProfileType;
                // For MBDN PhoneBook first read MBI file
                // Check if the mailbox inidcation type is correct
                if( iMBIProfileType < iMmPhoneBookStoreMessHandler->
                        iPBStoreConf[iArrayIndex].iMbiRecLen )
                    {
                    iCurrentDeletePhase = EPBDeletePhase_Read_MBI_profile;
                    // read MBDN record number from MBI first record Profile number
                    ret = UiccPbReqReadMBI();
                    }
                else
                    {
                    ret = KErrArgument;
                    }
                }
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmPhoneBookOperationDelete::CreateReq - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONDELETE_CREATEREQ_TD, "CMmPhoneBookOperationDelete::CreateReq;aIpc=%d", aIpc );
            break;
            }
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::UiccPbReqDelete
// Constructs an ISI-message to read entry from SIM
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UiccPbReqDelete()
    {
TFLOGSTRING3("TSY: CMmPhoneBookOperationDelete::UiccPbReqDelete, iTransId:%d,index:%d", iTransId, iIndex);
OstTraceExt2( TRACE_NORMAL,  CMmPhoneBookOperationDelete_UICCPBREQWRITEL_TD, "CMmPhoneBookOperationDelete::UiccPbReqDelete;iTransId=%hhu;iIndex=%hd", iTransId, iIndex );

    TInt ret( KErrArgument );

    switch ( iFileId )
        {
        case PB_ADN_FID:
        case PB_FDN_FID:
        case PB_MBDN_FID:
        case PB_VMBX_FID:
        case PB_MSISDN_FID:
            {
            // Check if the location to be deleted is valid
            if( ( iIndex <= iMmPhoneBookStoreMessHandler->
                iPBStoreConf[iArrayIndex].iNoOfRecords ) &&
                ( 0 < iIndex ) )
                {
                // Check if entry can be found in list
                if( iMmPhoneBookStoreMessHandler->IndexCheckInPBList(
                    iIndex,
                    iArrayIndex,
                    iEntry ) )
                    {
                    iLocationFoundInPbList = ETrue;
                    // Check if there are extension records for this entry
                    iNumOfExtensions = iEntry.PBEntryExtRecord.Count();
                    // Entry is present and there is at least one extension
                    if( iEntry.iEntryPresent && iNumOfExtensions )
                        {
                        // Start to delete the last extension (write 'FF')
                        iCurrentDeletePhase = EPBDeletePhaseDeleteExtension;
                        TInt record(
                            iEntry.PBEntryExtRecord[iNumOfExtensions - 1] );
                        ret = UiccPbReqDeleteExt( record );
                        iNumOfExtensions--;
                        }
                    // Entry is not present -> fill by 'FF' just in case
                    // or entry is present but no extensions -> delete entry
                    else
                        {
                        iCurrentDeletePhase = EPBDeletePhaseDeleteEntry;
                        ret = UiccPbReqDeleteEntry();
                        }
                    }
                // Index has not been read yet. At first it must be read to
                // define number of possible extensions
                else
                    {
                    iCurrentDeletePhase = EPBDeletePhaseReadEntry;
                    ret = UiccPbReqReadEntry();
                    }
                }
            }
        default:
            {
            break;
            }
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::UiccPBReqDeleteEntry
// Constructs an ISI-message to Write main Entry data
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UiccPbReqDeleteEntry()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::UiccPBReqDeleteEntry");
OstTrace0( TRACE_NORMAL,  CMmPhoneBookOperationDelete_UICCPBREQWRITEENTRY_TD, "CMmPhoneBookOperationDelete::UiccPBReqDeleteEntry" );

    TUiccWriteLinearFixed cmdParams;
    cmdParams.messHandlerPtr =
        static_cast<MUiccOperationBase*>( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
    if( PB_MBDN_FID == iFileId )
        {
        cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
        }
    else
        {
        cmdParams.filePath.Append( APPL_FILE_ID >> 8 );
        cmdParams.filePath.Append( APPL_FILE_ID );
        }

    cmdParams.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;
    cmdParams.fileId = iFileId;
    cmdParams.trId = static_cast<TUiccTrId>( iTransId );

    cmdParams.record = iIndex;
    // Buffer for data to be written, max size is 241 bytes for alpha
    // string and 14 mandatory data bytes. See 3GPP TS 51.011 V4.15.0
    // Chapter 10.5.1
    TBuf8<KEfAdnDataBytes+KMaxAlphaStringBytes> dataBuffer;
    // Check if alphastring exists
    TUint16 alphaStringLength( iMmPhoneBookStoreMessHandler->
        iPBStoreConf[ iArrayIndex ].iAlphaStringlength );
    // Filel buffer (alpha string bytes + mandatory bytes) by 'FF'
    dataBuffer.AppendFill( 0xFF, alphaStringLength + KEfAdnDataBytes );
    cmdParams.fileData.Append( dataBuffer );
    return iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::UiccPbReqDeleteExt
// Constructs an ISI-message to delete extension record
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UiccPbReqDeleteExt( TInt aExtRecordNum )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::UiccPbReqDeleteExt");
OstTrace0( TRACE_NORMAL,  CMmPhoneBookOperationDelete_UICCPBWRITEEXT_TD, "CMmPhoneBookOperationDelete::UiccPbReqDeleteExt" );

    TUiccWriteLinearFixed cmdParams;
    cmdParams.messHandlerPtr =
        static_cast<MUiccOperationBase*>( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
    if( ( PB_EXT6_FID == iFileIdExt ) ||
        ( PB_EXT5_FID == iFileIdExt ) )
        {
        cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
        }
    else
        {
        cmdParams.filePath.Append( APPL_FILE_ID >> 8 );
        cmdParams.filePath.Append( APPL_FILE_ID );
        }

    cmdParams.fileId = iFileIdExt;
    cmdParams.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;
    cmdParams.trId = static_cast<TUiccTrId>( iTransId );
    cmdParams.record = aExtRecordNum;

    // File Data
    TBuf8<KExtensionDataBytes>extFileData;
    extFileData.Append( 0x00 ); // Type of record ( 0x00 == unknown )
    // Rest of data is set by 'FF'
    extFileData.AppendFill( 0xFF, ( KExtensionDataBytes - 1 ) );
    cmdParams.fileData.Append( extFileData );

    return iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::UiccPbReqReadEntry
// Constructs an ISI-message to read main entry data
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UiccPbReqReadEntry()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::UiccPBReqDeleteEntry");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_UICCPBREQREADENTRY_TD, "CMmPhoneBookOperationDelete::UiccPbReqReadEntry" );

    TUiccReadLinearFixed cmdParams;
    cmdParams.messHandlerPtr =
        static_cast<MUiccOperationBase*>( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
    if( PB_MBDN_FID == iFileId )
        {
        cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
        }
    else
        {
        cmdParams.filePath.Append( APPL_FILE_ID >> 8 );
        cmdParams.filePath.Append( APPL_FILE_ID );
        }

    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    cmdParams.fileId = iFileId;
    cmdParams.trId = static_cast<TUiccTrId>( iTransId );
    cmdParams.record = iIndex;

    return iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::UiccPbReqReadMBI
// Constructs an ISI-message to Read MBI profile from first record
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UiccPbReqReadMBI( )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::UiccPbReqReadMBI");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_UICCPBREQREADMBI_TD, "CMmPhoneBookOperationDelete::UiccPbReqReadMBI" );

        TInt ret ( KErrNone );
        TUiccReadLinearFixed cmdParams;
        cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*>
                                   ( iMmPhoneBookStoreMessHandler );
        cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
        cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
        cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

        cmdParams.trId = static_cast<TUiccTrId>( iTransId );
        cmdParams.fileId = PB_MBI_FID;
        cmdParams.serviceType =  UICC_APPL_READ_LINEAR_FIXED ;
        cmdParams.dataAmount = 1;
        cmdParams.dataOffset = iMBIProfileType;
        cmdParams.record = 1;   // only first profile number is supported


        if( KErrNone == ret )
            {
            ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
                CreateUiccApplCmdReq( cmdParams );
            }
        return ret;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::UiccPBReqDeleteMBIProfile
// Send Request for MBBI Profile Write
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UiccPBReqDeleteMBIProfile()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::UiccPBReqWriteMBIProfile");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_UICCPBREQDELETEMBIPROFILE_TD, "CMmPhoneBookOperationDelete::UiccPBReqDeleteMBIProfile" );

    TInt ret ( KErrNone );

    TUiccWriteLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*>
                               ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    cmdParams.trId = static_cast<TUiccTrId>( iTransId );
    cmdParams.fileId = PB_MBI_FID;
    cmdParams.serviceType =  UICC_APPL_READ_LINEAR_FIXED ;
    cmdParams.dataAmount = 1;
    cmdParams.dataOffset = iMBIProfileType;
    cmdParams.record = 1;   // only first profile number is supported

    // Append FileData needs to be write
    cmdParams.fileData.Append( 0 );


    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::HandleReadEntryResp
// Handles phonebook entry data
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::HandleReadEntryResp(
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::HandleReadEntryResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_HANDLEREADENTRYRESP_TD, "CMmPhoneBookOperationDelete::HandleReadEntryResp" );

    TInt ret( KErrNotFound );

    // Check if alphastring exists
    TUint16 alphaStringLength( iMmPhoneBookStoreMessHandler->
        iPBStoreConf[ iArrayIndex ].iAlphaStringlength );

    // File contains possible alpha string and 14 mandatory bytes
    TUint8 index( alphaStringLength + KEfAdnDataBytes - 1 );

    if ( index < aFileData.Length() )
        {
        TUint8 extRecord( aFileData[index] );
        // Check if this entry has extension records
        if ( 0xFF == extRecord ) // No extension records, just delete entry
            {
            iCurrentDeletePhase = EPBDeletePhaseDeleteEntry;
            ret = UiccPbReqDeleteEntry();
            }
        else // Start to read extensions
            {
            iExtRecordArrayToBeDelete.Append( extRecord ); // Store records
            iCurrentDeletePhase = EPBDeletePhaseReadExtensions;
            ret = UiccPbReqReadExt( extRecord );
            }
        }
    return ret;
    }





// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::HandleDeleteEntryResp
// Handles phonebook Entry Delete
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::HandleDeleteEntryResp(
    TBool &aComplete,
    TInt &aLocation )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::HandleDeleteEntryResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_HANDLEDELETEENTRYRESP_TD, "CMmPhoneBookOperationDelete::HandleDeleteEntryResp" );

    TInt ret ( KErrNone );

    if ( iLocationFoundInPbList ) // Entry already in list, reset
        {
        iMmPhoneBookStoreMessHandler->ResetEntryInPhoneBookList(
            iArrayIndex,
            iIndex );
        }
    else
        {
        // This location has not been read earlier, add it to list
        TPBEntry* storeEntry = new ( ELeave ) TPBEntry();
        storeEntry->iEntryPresent = EFalse;
        storeEntry->iEntryIndex = iIndex;
        storeEntry->PBEntryExtRecord.Reset();
        iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
            storeEntry,
            iArrayIndex );
        }

    // If it is MBDN Phone Book then update MBI File also
    if( PB_MBDN_FID == iFileId)
        {
        // Start Writing MBI file
        iCurrentDeletePhase = EPBDeletePhase_delete_MBI_profile;
        ret = UiccPBReqDeleteMBIProfile();
        }
    else
        {
            // Ready for complete
            aComplete = ETrue;
        if ( EMmTsyPhoneBookStoreDeleteAllIPC == iIpc &&
            iNumOfEntries )
            {
            // Continue deleting entries
            iIndex = iNumOfEntries;
            ret = UiccPbReqDelete();
            iNumOfEntries--;
            }
        else
            {
            // Ready for complete
            aComplete = ETrue;
            // In case of delete all location is 0
            if ( EMmTsyPhoneBookStoreDeleteIPC == iIpc )
                {
                aLocation = iIndex;
                }
            }
        }

    return ret;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::HandleReadExtResp
// Handles phonebook Ext data Read
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::HandleDeleteExtResp()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::HandleDeleteExtResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_HANDLEDELETEEXTRESP_TD, "CMmPhoneBookOperationDelete::HandleDeleteExtResp" );

    TInt ret ( KErrNone );

    // Entry was read earlier and extension records saved
    if ( iLocationFoundInPbList )
        {
        // Check if there are extensions left, in that case
        // continue deleting extensions
        if ( iNumOfExtensions )
            {
            ret = UiccPbReqDeleteExt(
                iEntry.PBEntryExtRecord[iNumOfExtensions-1] );
            iNumOfExtensions--;
            }
        // All extensions were deleted, next delete the main entry
        else
            {
            iCurrentDeletePhase = EPBDeletePhaseDeleteEntry;
            ret = UiccPbReqDeleteEntry();
            }
        }
    // Extensions were read from SIM and saved to internal buffer
    else
        {
        iNumOfExtensions = iExtRecordArrayToBeDelete.Count();
        if ( iNumOfExtensions )
            {
            ret = UiccPbReqDeleteExt(
                iExtRecordArrayToBeDelete[iNumOfExtensions - 1] );
            iExtRecordArrayToBeDelete.Remove(
                iNumOfExtensions - 1 );
            iExtRecordArrayToBeDelete.Compress();
            }
        else // It is time to delete the main entry
            {
            iCurrentDeletePhase = EPBDeletePhaseDeleteEntry;
            ret = UiccPbReqDeleteEntry();
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::UiccPbReqDeleteExt
// Constructs an ISI-message to delete extension record
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::UiccPbReqReadExt( TUint8 aExtRecordNum )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::UiccPbReqDeleteExt");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_UICCPBREQREADEXT_TD, "CMmPhoneBookOperationDelete::UiccPbReqReadExt" );

    TUiccReadLinearFixed cmdParams;
    cmdParams.messHandlerPtr =
        static_cast<MUiccOperationBase*>( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
    if( ( PB_EXT6_FID == iFileIdExt ) ||
        ( PB_EXT5_FID == iFileIdExt ) )
        {
        cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
        }
    else
        {
        cmdParams.filePath.Append( APPL_FILE_ID >> 8 );
        cmdParams.filePath.Append( APPL_FILE_ID );
        }

    cmdParams.fileId = iFileIdExt;
    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    cmdParams.trId = static_cast<TUiccTrId>( iTransId );
    cmdParams.record = aExtRecordNum;

    return iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::HandleReadExtResp
// Handles phonebook entry data
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::HandleReadExtResp(
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::HandleReadExtResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_HANDLEREADEXTRESP_TD, "CMmPhoneBookOperationDelete::HandleReadExtResp" );

    TInt ret( KErrNotFound );

    // Read next record
    if ( KExtensionDataBytes == aFileData.Length() )
        {
        // Next record is located in the last byte
        TUint8 nextRecord( aFileData[KExtensionDataBytes - 1] );
        if ( 0xFF == nextRecord ) // No more extension records left
            {
            iCurrentDeletePhase = EPBDeletePhaseDeleteExtension;
            iNumOfExtensions = iExtRecordArrayToBeDelete.Count();
            if ( iNumOfExtensions )
                {
                // Start to delete extensions in reverse order
                ret = UiccPbReqDeleteExt(
                    iExtRecordArrayToBeDelete[iNumOfExtensions - 1] );
                iExtRecordArrayToBeDelete.Remove( iNumOfExtensions - 1 );
                iExtRecordArrayToBeDelete.Compress();
                }
            }
        else // Read the next extension
            {
            iExtRecordArrayToBeDelete.Append( nextRecord );
            ret = UiccPbReqReadExt( nextRecord );
            }
        }

    return ret;
    }


// ---------------------------------------------------------------------------
// TBool CMmPhoneBookOperationDelete::HandleWriteMBIReadResp
// Handle response for MBI profile read
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationDelete::HandleMBIReadResp(
        TInt aStatus,
        TUint8 aDetails,
        TBool &aComplete,
        const TDesC8 &aFileData )
    {
    TInt ret ( KErrNone );
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::HandleMBIReadResp");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_HANDLEMBIREADRESP_TD, "CMmPhoneBookOperationDelete::HandleMBIReadResp" );

    if( UICC_STATUS_OK  == aStatus )
        {
        iIndex = aFileData[0];
        if( ( 0 != iIndex ) && ( iIndex <= iMmPhoneBookStoreMessHandler->
                iPBStoreConf[iArrayIndex].iNoOfRecords ) )
            {
            ret = UiccPbReqDelete();
            }
        else
            {
            // Again read next MBI Profile type
            iMBIProfileType++;
            if ( EMmTsyPhoneBookStoreDeleteAllIPC == iIpc &&
                    ( iMBIProfileType < iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[iArrayIndex].iMbiRecLen ) )
                {
                iCurrentDeletePhase = EPBDeletePhase_Read_MBI_profile;
                // read MBDN record number from MBI first record Profile number
                ret = UiccPbReqReadMBI();
                }
            else
                {
                if( EMmTsyPhoneBookStoreDeleteIPC == iIpc )
                    {
                    ret = KErrArgument;
                    }
                aComplete = ETrue;
                }
            }
        }
    else
        {
        if( UICC_SECURITY_CONDITIONS_NOT_SATISFIED == aDetails )
            {
            ret = KErrAccessDenied;
            }
        else
            {
            ret = KErrArgument;
            }
        }
    return ret;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookOperationDelete::HandleUICCPbRespL
// Separate response
// ---------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationDelete::HandleUICCPbRespL(
    TInt aStatus,
    TUint8 aDetails,
    const TDesC8& aFileData,
    TInt /*aTransId*/ )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationDelete::HandleUICCPbRespL");
OstTrace0( TRACE_NORMAL,  CMmPhoneBookOperationDelete_HANDLEUICCPBRESPL_TD, "CMmPhoneBookOperationDelete::HandleUICCPbRespL" );

    TInt ret( KErrNone );
    TBool complete( EFalse );

    TInt location( 0 );

    if ( UICC_STATUS_OK == aStatus )
        {
        switch ( iCurrentDeletePhase )
            {
            case EPBDeletePhaseReadExtensions:
                {
                ret = HandleReadExtResp( aFileData );
                break;
                }
            case EPBDeletePhaseReadEntry:
                {
                ret = HandleReadEntryResp( aFileData );
                break;
                }
            case EPBDeletePhaseDeleteExtension:
                {
                ret = HandleDeleteExtResp();
                break;
                }
            case EPBDeletePhaseDeleteEntry:
                {
                ret = HandleDeleteEntryResp( complete, location );
                break;
                }
            case EPBDeletePhase_Read_MBI_profile:
                {
                ret = HandleMBIReadResp( aStatus, aDetails, complete, aFileData );
                break;
                }
            case EPBDeletePhase_delete_MBI_profile:
                {
                if( UICC_STATUS_OK  != aStatus )
                    {
                    if( UICC_SECURITY_CONDITIONS_NOT_SATISFIED == aDetails )
                        {
                        ret = KErrAccessDenied;
                        }
                    else
                        {
                        ret = KErrArgument;
                        }
                    }
                else
                    {
                    // Continue deleting entries
                    // increment iMBIProfileType to read next profile
                    iMBIProfileType++;

                    if ( EMmTsyPhoneBookStoreDeleteAllIPC == iIpc &&
                        ( iMBIProfileType < iMmPhoneBookStoreMessHandler->
                                iPBStoreConf[iArrayIndex].iMbiRecLen ) )
                        {
                        iCurrentDeletePhase = EPBDeletePhase_Read_MBI_profile;
                        // read MBDN record number from MBI first record Profile number
                        ret = UiccPbReqReadMBI();
                        }
                    else
                        {
                        // Ready for complete
                        complete = ETrue;
                        // In case of delete all location is 0
                        if ( EMmTsyPhoneBookStoreDeleteIPC == iIpc )
                            {
                            location = iMBIProfileType - 1 ;
                            }
                        }
                    }
                break;
                }
            default:
                {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleSimPbRespL - No such delete operation phase supported ");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKOPERATIONDELETE_HANDLEUICCPBRESPL_TD, "CMmPhoneBookOperationDelete::HandleUICCPbRespL- No such delete operation phase supported" );
                break;
                }
            } // End of switch( iCurrentDeletePhase )
        } // End of if ( UICC_STATUS_OK == aStatus )
    else // Request failed, complete and remove operation
        {
TFLOGSTRING2("TSY: CMmPhoneBookOperationDelete::HandleUICCPbRespL, Request failed, ret = %d", aStatus );
OstTrace1( TRACE_NORMAL,  CMMPHONEBOOKOPERATIONDELETE_HANDLEUICCPBRESPL_TD, "CMmPhoneBookOperationDelete::HandleUICCPbRespL;aStatus=%d", aStatus );
        complete = ETrue;
        ret = KErrNotFound;
        }

    // Error occured in some phase, stop operation and complete
    if ( ret != KErrNone )
        {
        complete = ETrue;
        }

    if ( complete )
        {
        TPBEntryInfo pbEntryInfo;
        pbEntryInfo.iLocation = 0;
        pbEntryInfo.iMaxNumLength = 0;

        if( KErrNone == ret )
            {
            pbEntryInfo.iMaxNumLength = iMmPhoneBookStoreMessHandler->
            iPBStoreConf[iArrayIndex].iNumlength;
            if( iIpc == EMmTsyPhoneBookStoreDeleteIPC )
                {
                pbEntryInfo.iLocation = location;
                }
            }

        CPhoneBookDataPackage phoneBookData;
        phoneBookData.SetPhoneBookName( iPhoneBookTypeName );
        phoneBookData.PackData( &pbEntryInfo );

        iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
            iIpc,
            &phoneBookData,
            ret );
        }

    return complete;
    }


// End of file
