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


// INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include <pn_const.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmessagerouter.h"
#include "cmmphonebookstoreoperationbase.h"
#include "cmmphonebookoperationcache.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationcachetraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
const TUint8 KEFBasicLength   = 14;

// Extension length is 22 (11 bytes having 2 digits/byte)
const TUint8 KEFExtensionLength   = 22;

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
// CMmPhoneBookOperationCache::CMmPhoneBookOperationCache
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationCache::CMmPhoneBookOperationCache
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::CMmPhoneBookOperationCache");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_CMMPHONEBOOKOPERATIONCACHE, "CMmPhoneBookOperationCache::CMmPhoneBookOperationCache" );
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::~CMmPhoneBookOperationCache
// C++ destructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationCache::~CMmPhoneBookOperationCache
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::~CMmPhoneBookOperationCache");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONCACHE_CMMPHONEBOOKOPERATIONCACHE, "CMmPhoneBookOperationCache::~CMmPhoneBookOperationCache" );
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::NewL
// Two-phased constructor.
// Creates a new CmmPhonebookOperatorInit object instance.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationCache* CMmPhoneBookOperationCache::NewL
    (
    CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
    CMmUiccMessHandler* aUiccMessHandler,
    TInt /*aIpc*/,
    const CMmDataPackage* aDataPackage // Data
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::NewL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_NEWL, "CMmPhoneBookOperationCache::NewL" );

    TName phonebookTypeName ;

    CMmPhoneBookOperationCache* mmPhoneBookOperationCache =
            new( ELeave ) CMmPhoneBookOperationCache();

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    phoneBookData->GetPhoneBookName( phonebookTypeName );

    mmPhoneBookOperationCache->iPhoneBookTypeName = phonebookTypeName;

    mmPhoneBookOperationCache->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;

    mmPhoneBookOperationCache->iMmUiccMessHandler = aUiccMessHandler;

    return mmPhoneBookOperationCache;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::UiccCreateReq
// Separate request
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationCache::UICCCreateReq
    (
    TInt aIpc,
    const CMmDataPackage* aDataPackage,
    TUint8 aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::CreateReq");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_CREATEREQ, "CMmPhoneBookOperationCache::CreateReq" );

    TInt ret( KErrNotSupported );

    // Transaction ID is saved to separate EMmTsyONStoreGetInfoIPC handling
    // from other PB handling. Also it can be used for any separation purpose.
    iSavedIPC = aIpc;

    switch( aIpc )
        {
        case EMmTsyPhoneBookStoreGetInfoIPC:
            {
            iNumOfUsedSDNEntries = 0;
            iNumOfUsedVMBXEntries = 0;
            break;
            }
        case EMmTsyONStoreGetInfoIPC:
            {
            iFileId = PB_MSISDN_FID;
            iExtFileId = PB_EXT1_FID;
            iArrayIndex = EPhonebookTypeMSISDN;

            TInt recordId( 0 );
            iEXTNumberLen = 0;
            iTypeOfReading = EFileInfoRead;
            aDataPackage->UnPackData( recordId );
            ret = UiccReadApplFileInfo( PB_MSISDN_FID,
                                        recordId,
                                        aTransId );
            break;
            }
        case EMmTsyPhoneBookStoreCacheIPC:
            {
            // Convert Phone Book name to file id
            iExtFileId = UICC_ILLEGAL_FILE_ID;
            iFileId = ConvertToPBfileId( iPhoneBookTypeName, 
                                         iExtFileId,  
                                         iMmUiccMessHandler->GetCardType());
            iArrayIndex = ConvertToConfArrayIndex( iFileId );

            const CPhoneBookDataPackage* phoneBookData(
                static_cast<const CPhoneBookDataPackage*>( aDataPackage ) );

            CArrayPtrSeg<CPhoneBookStoreEntry>* prtToCacheArray;
            phoneBookData->UnPackData( prtToCacheArray );

            if ( prtToCacheArray )
                {
                iPhoneBookStoreCacheArray = prtToCacheArray;
                ret = KErrNone;
                iIndexToRead = 1 ; // Strat from 1st Record to read
                // Start Reading records from 1st record
                ret = USimPbReqRead( iIndexToRead, aTransId );
                }
            else
                {
                ret = KErrArgument;
                }

            break;
            }
        default:
            {
            // Nothing to do here
TFLOGSTRING2("TSY: CMmPhoneBookOperationCache::CreateReq - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONCACHE_UICCCREATEREQ, "CMmPhoneBookOperationCache::UICCCreateReq;Unknown aIpc=%d", aIpc );
            break;
            }
        }//switch-case
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::HandleUICCPbRespL
// Handle response
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationCache::HandleUICCPbRespL (
      TInt aStatus,
      TUint8 /*aDetails*/,
      const TDesC8 &aFileData,
      TInt aTransId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::HandleUICCPbRespL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL,"CMmPhoneBookOperationCache::HandleUICCPbRespL" );

    TInt ret( KErrNone );
    TInt retExt( KErrNone );
    TBool complete ( EFalse );

// IF UICC Serever response is not OK
    if ( ( UICC_STATUS_OK != aStatus ) && ( !iExtensionToRead) )
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::HandleSimPbResp:Unsuccessfully completed by UICC");
OstTrace0( TRACE_NORMAL, DUP4_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL - Unsuceesfully completed by UICC" );

        ret = CMmStaticUtility::UICCCSCauseToEpocError( aStatus );
        }


    if ( ( !iCancelOperation ) &&
         ( EMmTsyONStoreGetInfoIPC != iSavedIPC ) )
        {
        switch(iFileId)
            {
            case PB_ADN_FID:
            case PB_FDN_FID:
            case PB_SDN_FID:
                {
                // Start Caching for ADN Phone Book
                if(!iExtensionToRead)
                    {
                    // Check for UICC_SERVER_OK
                    if( UICC_STATUS_OK == aStatus)
                        {
                        iStoreEntry = new ( ELeave ) TPBEntry();
                        // update Entry Data
                        iStoreEntry->iEntryIndex = iIndexToRead;

                        // check if Entry is valid Entry or Empty Entry
                        TInt retval = EmptyEntryCheck(aFileData);

                        // if Entry is not empty
                        if( KErrNone == retval)
                            {
                            // Update Entry Status
                            iStoreEntry->iEntryPresent = ETrue;
                            // Seperate Entry data form UICC Server response message
                            // Case: <Data available to be filled into array>
                            iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[iArrayIndex].
                            GetPBEntryFromUICCData(
                                aFileData,
                                iNumberBuf,
                                iNameBuf);

                            // Check for Is there any extension data
                            // And the Extension data record number is valid
                            // Index to read Extension file Data is Alpha string
                            // Length + 14 , minus 1 is for Array index Calculation
                            //(it starts from 0)
                            if ( 0xFF == aFileData[iMmPhoneBookStoreMessHandler->
                                         iPBStoreConf[iArrayIndex].
                                         iAlphaStringlength + 13]  )
                                {
                                // Append Entry to list
                                iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                                              iStoreEntry,
                                              iArrayIndex );

                                // the there is no extension data
                                CPhoneBookStoreEntry* phoneBookStoreMsg =
                                new( ELeave ) CPhoneBookStoreEntry;
                                CleanupStack::PushL( phoneBookStoreMsg );
                                phoneBookStoreMsg->ConstructL();

                                iMmPhoneBookStoreMessHandler->StorePhonebookEntryL(
                                    iNameBuf,
                                    iNumberBuf,
                                    *phoneBookStoreMsg,
                                    iFileId,
                                    iIndexToRead );
                                TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
                                iPhoneBookStoreCacheArray->AppendL( phoneBookStoreMsg );
                                CleanupStack::Pop( phoneBookStoreMsg );
                                iNumOfEntriesFilled++;
TFLOGSTRING2("TSY: CMmPhoneBookOperationCache::HandleUICCPbRespL - Append entries into array %d",iNumOfEntriesFilled);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL; - Append entries into array iNumOfEntriesFilled=%hhu", iNumOfEntriesFilled );

                                } // End of if Ext Data is not Present
                            else
                                {
                                iExtensionToRead = ETrue;
                                // Record no to be read from EXT File
                                TInt recordNo = aFileData[iMmPhoneBookStoreMessHandler->
                                               iPBStoreConf[iArrayIndex].
                                               iAlphaStringlength + 13];

                                // Append EXT record no.
                                iStoreEntry->PBEntryExtRecord.Append( recordNo );

                                retExt = USimPbReqRead( recordNo, aTransId );
                                // if while reading EXT error comes
                                //(for invalid Entry)than read next entry
                                if(( KErrNone != retExt ))
                                    {
                                    iExtensionToRead = EFalse;
                                    }
                                } // End for EXT File data present
                            } // end for case when entry is not empty
                        else
                            {
                            //Update Entry Status
                            iStoreEntry->iEntryPresent = EFalse;
                            // Reset Entry andd Append to the List
                            iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                                 iStoreEntry,
                                 iArrayIndex );
                            }

                        // to read next record
                        iIndexToRead++;

                        } // end of check for UICC status
                    } // End of without EXT File Data Case
                else  // Handling for Extension Numbers
                    {
                    if( UICC_STATUS_OK == aStatus )
                        {
                        // Check for next extension data record
                        if(0xFF != aFileData[UICC_EXT_REC_NO_OFFSET])
                            {
                            // Again Append the EXT no to Array
                            iStoreEntry->PBEntryExtRecord.Append(
                                  aFileData[UICC_EXT_REC_NO_OFFSET] );

                            // Store Number upto last byte
                            iNumberBuf.Append(aFileData.Mid(1,UICC_EF_EXT_REC_NUM_LEN ));
                            // Again Send request to read next record number
                            //and append it in number
                            retExt = USimPbReqRead(
                                aFileData[UICC_EXT_REC_NO_OFFSET], aTransId );
                            if( KErrNone != retExt)
                                {
                                iExtensionToRead = EFalse;
                                }
                            }  // end for checking next record number in EXT
                        else
                            {
                            // Append Entry to list and reset all the EXT data
                            iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                                                          iStoreEntry,
                                                          iArrayIndex );
                            // Reset Extension File record
                            iExtensionToRead = EFalse;

                            // Check for Extended Data is Addition number
                            if( 0x02 == aFileData[0])
                                {
                                // Check for length upto which no is stored
                                TInt offset = aFileData.Find(&KTagUnusedbyte,1);
                                // store Data
                                iNumberBuf.Append(aFileData.Mid(1,( offset - 1 )));

                                // the there is extension data
                                CPhoneBookStoreEntry* phoneBookStoreMsg =
                                new( ELeave ) CPhoneBookStoreEntry;
                                CleanupStack::PushL( phoneBookStoreMsg );
                                phoneBookStoreMsg->ConstructL();

                                iMmPhoneBookStoreMessHandler->StorePhonebookEntryL(
                                                              iNameBuf,
                                                              iNumberBuf,
                                                              *phoneBookStoreMsg,
                                                              iFileId,
                                                              iIndexToRead );
                                TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
                                iPhoneBookStoreCacheArray->AppendL(
                                                           phoneBookStoreMsg );
                                CleanupStack::Pop( phoneBookStoreMsg );
                                iNumOfEntriesFilled++;
TFLOGSTRING2("TSY: CMmPhoneBookOperationCache::HandleUSimPbRespL - Append entries into array %d",iNumOfEntriesFilled);
OstTraceExt1( TRACE_NORMAL, DUP5_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL; - Append entries into array iNumOfEntriesFilled=%hhu", iNumOfEntriesFilled );
                                } // end for checking Data type in EXT
                            } // end for Entry store
                        }
                    else
                        {
                        // To read next Entry
                        iExtensionToRead = EFalse;
                        }
                    } // end for EXT file handling
                break;
                } // end for ADN phone book cache
            default:
                {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::HandleUICCPbRespL. PhoneBook operation not supported ");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL. PhoneBook Operation not supported" );
                break;
                }
            } // End of switch case

        // Check for next location be read when its not for EXT Data

        if( ( !iExtensionToRead ) &&( iIndexToRead <=
              iMmPhoneBookStoreMessHandler->iPBStoreConf[iArrayIndex].iNoOfRecords ) )
            {
            // Send request to read next Entry
            USimPbReqRead( iIndexToRead, aTransId );
            }

        // Complete IPC if error is there OR all Entries are read
        if( ((KErrNone != ret) ||
             ( iIndexToRead > iMmPhoneBookStoreMessHandler->
                              iPBStoreConf[iArrayIndex].iNoOfRecords ))
             && ( !iExtensionToRead ))
            {
            // Check if any entries are filled in commonTSY Array
            if( 0 == iNumOfEntriesFilled )
                {
                ret = KErrNotFound;
                }
            // Complete
            CPhoneBookDataPackage phoneBookData;
            phoneBookData.SetPhoneBookName( iPhoneBookTypeName );

            iPhoneBookStoreCacheArray = NULL;

            iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
                EMmTsyPhoneBookStoreCacheIPC,
                &phoneBookData,
                ret );
            // Set flag to indicate that we can remove this operation from array
            complete = ETrue;
            }
        }
    else if ( EMmTsyONStoreGetInfoIPC == iSavedIPC )
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::HandleUICCPbRespL: Handle EMmTsyONStoreGetInfoIPC");
OstTrace0( TRACE_NORMAL, DUP6_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL: Handle EMmTsyONStoreGetInfoIPC" );
        complete = HandleUiccReadApplFileInfoResp( aTransId,
                                                   aStatus,
                                                   aFileData );
        }
    else
        { // Operation has been canceled
        complete = ETrue;
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::HandleUICCPbRespL; operation was canceled");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL; operation was canceled" );
        }
    return complete;

    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::USimPbReqRead
// Constructs Data to read entry from SIM
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationCache::USimPbReqRead
    (
    TInt aRecordNo,
    TUint8 aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimPbReqRead");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_USIMPBREQREAD, "CMmPhoneBookOperationCache::USimPbReqRead" );


    TInt ret( KErrNone );
    TInt appFileID ( APPL_FILE_ID );   // Application File id for DFphonebook

    TUiccReadLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*>
                               ( iMmPhoneBookStoreMessHandler );

    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( appFileID>>8);
    cmdParams.filePath.Append( appFileID);

    if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
        {
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
        }

    switch( iFileId )
        {
        case PB_ADN_FID:
        case PB_FDN_FID:
        case PB_SDN_FID:
            {
            // For 2G ADN Phonebook EXT1 will be the extension number store
            cmdParams.trId = static_cast<TUiccTrId> ( aTransId );
            cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
            cmdParams.record = aRecordNo;

            // Check for Extension Data is Present or not
            if(!iExtensionToRead)
                {
                // Check for the record Number to be
                //read is valid record number
                if( iIndexToRead <= iMmPhoneBookStoreMessHandler->
                        iPBStoreConf[iArrayIndex].iNoOfRecords)
                    {
                    // Start from first location and Search for First Valid
                    //Entry in the Stored List And if some Entry is invalid
                    // then Read From Sim and Check the Staus its Free
                    // or not till end of the records

                    cmdParams.fileId = iFileId;
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    // till End of Record
                    cmdParams.dataAmount = 0;
                    // Start from begining of record
                    cmdParams.dataOffset = 0;
                    }
                else
                    {
                    ret = KErrNotFound;
                    }
                } // end of if case for checking extension data
            else
                {
                // Send Request to Read Extension Data
                // Check for UST Table supports for EXT File
                if( iMmPhoneBookStoreMessHandler->
                        iPBStoreConf[iArrayIndex].iExtension )
                    {
                    // Check for Extension data record in valid
                    if(iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[iArrayIndex].iExtNoOfRec >= aRecordNo)
                        {
                        // Read Request to read that index
                        cmdParams.fileId = iExtFileId;
                        cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                        cmdParams.record = aRecordNo;
                        // till End of Record
                        cmdParams.dataAmount = 0;
                        // Start from begining of record
                        cmdParams.dataOffset = 0;
                        }
                    else
                        {
                        ret = KErrGeneral;
                        }
                    }
                else
                    {
                    ret = KErrGeneral;
                    }
                }
                break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimPbReqRead - PhoneBook Not supported");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONCACHE_USIMPBREQREAD, "CMmPhoneBookOperationCache::USimPbReqRead - PhoneBook not supported" );
                break;
            }
        }


    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONCACHE_USIMPBREQREAD, "CMmPhoneBookOperationCache::USimPbReqRead;returns ret=%d", ret );
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::CancelReq
// Cancels cacheing request
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationCache::CancelReq( TName& aPhoneBook )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::CancelReq");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_CANCELREQ, "CMmPhoneBookOperationCache::CancelReq" );
    if( iPhoneBookTypeName == aPhoneBook )
        {
        iPhoneBookStoreCacheArray = NULL;
        iCancelOperation = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::UiccReadApplFileInfo
// Read application file info
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationCache::UiccReadApplFileInfo(
    const TInt aFileId,
    const TInt aRecordId,
    const TUint8 aTrId )
    {
TFLOGSTRING3("TSY: CMmPhoneBookOperationCache::UiccReadApplFileInfo, aTraId: %d, aRecordId: %d", aTrId, aRecordId );
OstTraceExt2( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_UICCREADAPPLFILEINFO, "CMmPhoneBookOperationCache::UiccReadApplFileInfo;aFileId=%d;aRecordId=%d", aFileId, aRecordId );

    TInt ret( KErrNone );

    iRecordId = aRecordId;

    // Check wheter MSIDN is supported at all
    ret = CheckMSISDNSupport();

    if ( KErrNone == ret )
        {
        // Set parameters for UICC_APPL_CMD_REQ message
        TUiccReadLinearFixed params;
        params.messHandlerPtr  = static_cast<MUiccOperationBase*>
                                   ( iMmPhoneBookStoreMessHandler );
        params.trId = static_cast<TUiccTrId> ( aTrId );
        params.dataOffset = 0;
        params.dataAmount = 0;
        params.record = iRecordId;

        params.fileId = aFileId;
        params.fileIdSfi = UICC_SFI_NOT_PRESENT;
        params.serviceType = UICC_APPL_FILE_INFO;

        // File id path
        params.filePath.Append( KMasterFileId >> 8 );
        params.filePath.Append( KMasterFileId );
        params.filePath.Append( APPL_FILE_ID>>8);
        params.filePath.Append( APPL_FILE_ID);

        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( params );
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::UiccReadApplFileInfo MSISDN is not activated on SIM" );
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONCACHE_UICCREADAPPLFILEINFO, "CMmPhoneBookOperationCache::UiccReadApplFileInfo MSISDN is not activated on SIM" );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::HandleUiccReadApplFileInfoResp
// Read application file info
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationCache::HandleUiccReadApplFileInfoResp(
      TInt aTransId,
      TInt aStatus,
      const TDesC8 &aFileData )
    {
TFLOGSTRING2("TSY: CMmPhoneBookOperationCache::HandleUiccReadApplFileInfoResp, aStatus: %d", aStatus );
OstTrace1( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCREADAPPLFILEINFORESP, "CMmPhoneBookOperationCache::HandleUiccReadApplFileInfoResp;aStatus=%d", aStatus );

    TInt ret( KErrNone );
    TBool completed( EFalse );

    CMmDataPackage numberData;

    if ( ( UICC_STATUS_OK != aStatus ) || ( 0 == aFileData.Length() ) )
        {
        // Complete response with the array of ON entries with ret
        // Pack the data for sending to the manager
        ret = CMmStaticUtility::UICCCSCauseToEpocError( aStatus );
        TInt numofEntry( -1 );
        numberData.PackData( &numofEntry );
        CompleteThisIPC( EMmTsyONStoreGetInfoIPC, &numberData, ret );
        completed = ETrue;
        }
    else
        {
        switch ( iTypeOfReading )
            {
            case EFileInfoRead:
                {
                TFci fci( aFileData );
                // Number of entries and name length can be fetched here.
                // Number length and number of used entries will be updated
                // during next sequences.
                iServiceType.iNumOfEntries = fci.GetNumberOfRecords();
                iServiceType.iNameLen = fci.GetRecordLength() - KEFBasicLength;

                iTypeOfReading = EBasicEfRead;

                // Start from the 1st record
                iRecordId = KStartRecord;

                UiccReadApplFileData(
                    PB_MSISDN_FID,
                    iRecordId,
                    aTransId );
                break;
                }

            case EBasicEfRead:
                {
                // Check for Is there any extension data
                // And the Extension data record number is valid
                // Index to read Extension file Data is Alpha string Length +
                // basic EF length minus 1 is for Array index Calculation
                // (it starts from 0)

                iServiceType.iNumLen = UICC_NO_EXT_MAX_NUM_LEN;
                TInt fileId( iExtFileId );
                iRecordId = 
                    aFileData[iServiceType.iNameLen + KEFBasicLength - 1];
                if ( 0xFF == iRecordId )
                    {
                    iServiceType.iNumLen =
                        UICC_NO_EXT_MAX_NUM_LEN;

                    // No extension. Continue with file count reading.
                    iTypeOfReading = EBasicEfReadToGetUsedFileCount;

                    // EFSMS file id is used for file count reading.
                    fileId = iFileId;

                    // Start from the 1st record
                    iRecordId = KStartRecord;
                    }
                else
                    {
                    iTypeOfReading = EExtensionRead;
                    }

                UiccReadApplFileData(
                    fileId,
                    iRecordId,
                    aTransId );
                break;
                }

            case EExtensionRead:
                {
                // If current record number is the same as the next record
                // number in the extension file complete the whole session
                // with cause KErrArgument.
                if ( iRecordId ==
                         aFileData[UICC_EXT_REC_NO_OFFSET] )
                    {
                    TInt numofEntry( -1 );
                    numberData.PackData( &numofEntry );
                    CompleteThisIPC( EMmTsyONStoreGetInfoIPC,
                                     &numberData,
                                     KErrCorrupt );
                    completed = ETrue;
                    }
                else
                    {
                    iServiceType.iNumLen += KEFExtensionLength;
                    iRecordId = aFileData[UICC_EXT_REC_NO_OFFSET];

                    // In case of ICC EXT1 is used, but in UICC case it is EXT5
                    TInt fileId( iExtFileId );
                    if ( 0xFF == iRecordId )
                        {
                        // The last record found. Check count of used entires
                        iTypeOfReading = EBasicEfReadToGetUsedFileCount;
                        // EFMSISDN file id is used for file count reading.
                        fileId = PB_MSISDN_FID;
                        // Start from the 1st record
                        iRecordId = 1;
                        }
                    else
                        {
                        iTypeOfReading = EExtensionRead;
                        }

                    UiccReadApplFileData(
                        fileId,
                        iRecordId,
                        aTransId );
                    }
                break;
                }

            case EBasicEfReadToGetUsedFileCount:
                {
                TInt retval( EmptyEntryCheck( aFileData ) );
                if ( KErrNone == retval )
                    {
                    iServiceType.iUsedEntries++;
                    }
                // No else, because used entries counter is increased only when
                // there is data in the record.
                iRecordId++;
                if ( iRecordId > iServiceType.iNumOfEntries )
                    {
                    numberData.PackData( &iServiceType );
                    CompleteThisIPC( EMmTsyONStoreGetInfoIPC,
                                     &numberData,
                                     ret );
                    completed = ETrue;
                    }
                else
                    {
                    UiccReadApplFileData(
                        PB_MSISDN_FID,
                        iRecordId,
                        aTransId );
                    }
                break;
                }

            default:
                {
TFLOGSTRING2("TSY: CMmPhoneBookOperationCache::HandleUiccReadApplFileInfoResp - Unknown iTypeOfReading: %d", iTypeOfReading);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCREADAPPLFILEINFORESP, "CMmPhoneBookOperationCache::HandleUiccReadApplFileInfoResp;Unknown iTypeOfReading=%hhu", iTypeOfReading );
                completed = ETrue;
                break;
                }
            }
        }
    return completed;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::HandleUiccReadApplFileInfoResp
// Read application file info
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationCache::CheckMSISDNSupport
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::CheckMSISDNSupport" );
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_CHECKMSISDNSUPPORT, "CMmPhoneBookOperationCache::CheckMSISDNSupport" );

    TInt ret( KErrNotSupported );

    if ( iMmUiccMessHandler->GetServiceStatus( UICC_MSISDN_SERVICE_NUM ) )
       {
       ret = KErrNone;
       }
    // No else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::CompleteThisIPC
// Read application file info
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationCache::CompleteThisIPC
    (
    TInt aIPCToBeCompleted,
    CMmDataPackage* aDataPackage,
    TInt ret
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationCache::CompleteThisIPC" );
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONCACHE_CHECKMSISDNSUPPORT, "CMmPhoneBookOperationCache::CheckMSISDNSupport" );

    iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
        aIPCToBeCompleted,
        aDataPackage,
        ret );
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationCache::UiccReadApplFileData
// Read application file info
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationCache::UiccReadApplFileData(
    const TInt aFileId,
    const TInt aRecordId,
    const TUint8 aTrId )
    {
TFLOGSTRING3("TSY: CMmPhoneBookOperationCache::UiccReadApplFileData, aTraId: %d, aRecordId: %d", aTrId, aRecordId );
OstTraceExt2( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_UICCREADAPPLFILEDATA, "CMmPhoneBookOperationCache::UiccReadApplFileData;aTrId=%d;aRecordId=%d", aTrId, aRecordId );

    TInt ret( KErrNone );

    TUiccReadLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*>
                               ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);

    if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
        {
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
        }

    cmdParams.trId = static_cast<TUiccTrId>( aTrId );
    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    cmdParams.record = aRecordId;
    cmdParams.fileId = aFileId;
    cmdParams.dataAmount = 0;
    cmdParams.dataOffset = 0;

    ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );

    return ret;
    }

// End of file
