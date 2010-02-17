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


// INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include <pn_const.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationcache.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationcachetraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS

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

    switch( aIpc )
        {
        case EMmTsyPhoneBookStoreGetInfoIPC:
            {
            iNumOfUsedSDNEntries = 0;
            iNumOfUsedVMBXEntries = 0;
            break;
            }
        case EMmTsyPhoneBookStoreCacheIPC:
            {
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
OstTraceExt1( TRACE_NORMAL, CMMPHONEBOOKOPERATIONCACHE_UICCCREATEREQ, "CMmPhoneBookOperationCache::UiccCreateReq; Unknown IPC =%hd", aIpc );
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


    if( !iCancelOperation )
        {
        // Convert Phone Book name to file id
        TUint16 fileIdExt ( UICC_ILLEGAL_FILE_ID );
        TUint16 pbFileId = ConvertToPBfileId( iPhoneBookTypeName, fileIdExt,  iMmUiccMessHandler->GetCardType());
        TUint8 pbArrayIndex = ConvertToConfArrayIndex( pbFileId );
        
        switch(pbFileId)
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
                            iPBStoreConf[pbArrayIndex].
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
                                         iPBStoreConf[pbArrayIndex].
                                         iAlphaStringlength + 13]  )
                                {
                                // Append Entry to list
                                iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                                              iStoreEntry,
                                              pbArrayIndex );

                                // the there is no extension data
                                CPhoneBookStoreEntry* phoneBookStoreMsg =
                                new( ELeave ) CPhoneBookStoreEntry;
                                CleanupStack::PushL( phoneBookStoreMsg );
                                phoneBookStoreMsg->ConstructL();
                                
                                iMmPhoneBookStoreMessHandler->StorePhonebookEntryL(
                                    iNameBuf,
                                    iNumberBuf,
                                    *phoneBookStoreMsg,
                                    pbFileId,
                                    iIndexToRead,
                                    EFalse );
                                TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
                                iPhoneBookStoreCacheArray->AppendL( phoneBookStoreMsg );
                                CleanupStack::Pop( phoneBookStoreMsg );
                                iNumOfEntriesFilled++;
TFLOGSTRING2("TSY: CMmPhoneBookOperationCache::HandleUICCPbRespL - Append entries into array %d",iNumOfEntriesFilled);
OstTraceExt1( TRACE_NORMAL, DUP6_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL;iNumOfEntriesFilled=%hd", iNumOfEntriesFilled );
                                
                                } // End of if Ext Data is not Present
                            else
                                {
                                iExtensionToRead = ETrue;
                                // Record no to be read from EXT File
                                TInt recordNo = aFileData[iMmPhoneBookStoreMessHandler->
                                               iPBStoreConf[pbArrayIndex].
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
                                 pbArrayIndex );
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
                                                          pbArrayIndex );
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
                                                              pbFileId,
                                                              iIndexToRead,
                                                              EFalse );
                                TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
                                iPhoneBookStoreCacheArray->AppendL( 
                                                           phoneBookStoreMsg );
                                CleanupStack::Pop( phoneBookStoreMsg );
                                iNumOfEntriesFilled++;
TFLOGSTRING2("TSY: CMmPhoneBookOperationCache::HandleUSimPbRespL - Append entries into array %d",iNumOfEntriesFilled);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMPHONEBOOKOPERATIONCACHE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationCache::HandleUICCPbRespL;iNumOfEntriesFilled=%hd", iNumOfEntriesFilled );
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
              iMmPhoneBookStoreMessHandler->iPBStoreConf[pbArrayIndex].iNoOfRecords ) )
            {
            // Send request to read next Entry
            USimPbReqRead( iIndexToRead, aTransId );
            }

        // Complete IPC if error is there OR all Entries are read
        if( ((KErrNone != ret) || 
             ( iIndexToRead > iMmPhoneBookStoreMessHandler->
                              iPBStoreConf[pbArrayIndex].iNoOfRecords )) 
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
    
    // Convert Phone Book name to file id
    TUint16 fileIdExt ( 0x0000 );
    TUint16 pbFileId = ConvertToPBfileId( iPhoneBookTypeName, fileIdExt, iMmUiccMessHandler->GetCardType() );
    TUint8 pbArrayIndex = ConvertToConfArrayIndex( pbFileId );

    switch( pbFileId )
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
                        iPBStoreConf[pbArrayIndex].iNoOfRecords)
                    {
                    // Start from first location and Search for First Valid 
                    //Entry in the Stored List And if some Entry is invalid
                    // then Read From Sim and Check the Staus its Free 
                    // or not till end of the records
                    
                    cmdParams.fileId = pbFileId;
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
                        iPBStoreConf[pbArrayIndex].iExtension )
                    {
                    // Check for Extension data record in valid
                    if(iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[pbArrayIndex].iExtNoOfRec >= aRecordNo)
                        {
                        // Read Request to read that index
                        cmdParams.fileId = fileIdExt;
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
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimPbReqRead - PhoenBook Not supported");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONCACHE_USIMPBREQREAD, "CMmPhoneBookOperationCache::USimPbReqRead - PhoneBook not supported" );
                break;
            }
        }
        

    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONCACHE_USIMPBREQREAD, "CMmPhoneBookOperationCache::USimPbReqRead;ret=%hd", ret );
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

// End of file
