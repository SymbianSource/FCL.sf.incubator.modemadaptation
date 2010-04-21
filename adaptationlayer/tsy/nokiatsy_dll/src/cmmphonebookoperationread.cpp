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
#include "tsylogger.h"
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationread.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationreadTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
//const TUint16 KSimPbLocationMBDN = 0x0100;

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
// CMmPhonebookOperationRead::CMmPhonebookOperationRead
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

CMmPhoneBookOperationRead::CMmPhoneBookOperationRead()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::CMmPhoneBookOperationRead");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_CMMPHONEBOOKOPERATIONREAD, "CMmPhoneBookOperationRead::CMmPhoneBookOperationRead" );

    iNumOfEntriesToRead = 0;
    iNumOfEntriesFilled = 0;
    iIndexToRead = 0;
    iTypeOfReading = EStartRead;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::~CMmPhoneBookOperationRead
// C++ destructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationRead::~CMmPhoneBookOperationRead
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::~CMmPhoneBookOperationRead");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD_CMMPHONEBOOKOPERATIONREAD, "CMmPhoneBookOperationRead::~CMmPhoneBookOperationRead" );
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::NewL
// Creates a new CMmPhonebookOperationRead object instance.
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationRead* CMmPhoneBookOperationRead::NewL
    (
    CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
    CMmUiccMessHandler* aUiccMessHandler,
    const CMmDataPackage* aDataPackage, // Data
    TInt aIpc
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::NewL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_NEWL, "CMmPhoneBookOperationRead::NewL" );

    CMmPhoneBookOperationRead* mmPhoneBookOperationRead =
        new( ELeave ) CMmPhoneBookOperationRead();

    if( ( EMmTsyONStoreReadSizeIPC != aIpc )&&
        ( EMmTsyONStoreWriteSizeIPC != aIpc ) )
        {
        TName phonebookTypeName;

        const CPhoneBookDataPackage* phoneBookData =
            static_cast<const CPhoneBookDataPackage*>( aDataPackage );

        phoneBookData->GetPhoneBookName( phonebookTypeName );
        // Store phoen Book name 
        mmPhoneBookOperationRead->iPhoneBookTypeName = phonebookTypeName;

        }
    // get the Transaction id for Phone book and operation combination
    mmPhoneBookOperationRead->iLocationSearch = EFalse;
    mmPhoneBookOperationRead->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;

    mmPhoneBookOperationRead->iMmUiccMessHandler = aUiccMessHandler;

    return mmPhoneBookOperationRead;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead::ConstructL
    (
    // None
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::ConstructL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_CONSTRUCTL, "CMmPhoneBookOperationRead::ConstructL" );
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::UICCCreateReq
// Separate request for USIM
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead::UICCCreateReq
    (
    TInt aIpc,
    const CMmDataPackage* aDataPackage,
    TUint8 aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::UICCCreateReq");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_UICCCREATEREQ, "CMmPhoneBookOperationRead::UICCCreateReq" );

    TInt ret( KErrNotSupported );

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    iSavedIPCForComplete = aIpc;
    
    iFileId = ConvertToPBfileId( iPhoneBookTypeName, iExtFileId, iMmUiccMessHandler->GetCardType() );
    iArrayIndex = ConvertToConfArrayIndex( iFileId );
    
    switch( aIpc )
        {
        case EMmTsyONStoreReadIPC:
        case EMmTsyONStoreReadEntryIPC:
            {
            // For MSISDN PhoneBook
            iFileId = PB_MSISDN_FID;
            iExtFileId = PB_EXT1_FID;
            iArrayIndex = EPhonebookTypeMSISDN;
            // Unpack index to be read
            aDataPackage->UnPackData( iIndexToRead );
            // number of entries to be read for MBDN PhoneBook will always be 1
            iNumOfEntriesToRead = 1;
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

                ret = USimPbReqRead( iIndexToRead, aTransId );
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
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::UICCCreateReq - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD_UICCCREATEREQ, "CMmPhoneBookOperationRead::UICCCreateReq;aIpc=%d", aIpc );
            break;
            }
        } // switch-case
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::SimPbReqRead
// Constructs Data to read entry from SIM
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead::USimPbReqRead
    (
    TInt aRecordNo,
    TUint8 aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimPbReqRead");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_USIMPBREQREAD, "CMmPhoneBookOperationRead::USimPbReqRead" );

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
    
    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );

    // get the corect Location to be read from phone book
    if( ( PB_MBDN_FID == iFileId ) &&
        ( EStartRead == iTypeOfReading ) )
        {
        // Check id MBI file index is valid
        if( ( iIndexToRead <= iMmPhoneBookStoreMessHandler->
                iPBStoreConf[iArrayIndex].iMbiRecLen ) &&
            ( iIndexToRead >= 0 ) )
            {
            // Send request to read MBI File
            cmdParams.fileId = PB_MBI_FID;
            cmdParams.dataAmount = 1;
            
            cmdParams.dataOffset = iIndexToRead;
            cmdParams.record = 1;   // only first profile number is supported
            
            // Set TYpe od reading
            iTypeOfReading = EMailboxIdRead;
            }
        else
            {
            ret = KErrArgument;
            }
        }
    else
        {
        if( EStartRead == iTypeOfReading )
            {
            iTypeOfReading = EBasicEfRead;
            } // no else
        // till End of Record
        cmdParams.dataAmount = 0;
        // Start from begining of record
        cmdParams.dataOffset = 0;
        // Check for Extension Data is Present or not
        if ( EBasicEfRead == iTypeOfReading )
            {
            cmdParams.fileId = iFileId;
            ret = AddParamToReadReq( cmdParams );
            }// end of if case for checking extension data
        else if  ( EExtensionRead == iTypeOfReading )
            {
            // Send Request to Read Extension Data
            // Check for UST Table supports for EXT File
            if ( ( iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[iArrayIndex].iExtension )&&
                 ( iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[iArrayIndex].iExtNoOfRec >= aRecordNo ) &&
                 ( aRecordNo > 0 ) )
                {
                // Read Request to read that index
                cmdParams.fileId = iExtFileId;
                cmdParams.record = aRecordNo;
                }
            else
                {
                ret = KErrGeneral;
                }
            } // no else
        }
    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONREAD_USIMPBREQREAD, "CMmPhoneBookOperationRead::USimPbReqRead;ret=%d", ret );
        }  // no else
    
    return ret;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::USimReadWriteSizeReq
// Send Request to Get Read and Write size
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead::USimReadWriteSizeReq( TUint8 aTransId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimReadWriteSizeReq");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_USIMREADWRITESIZEREQ, "CMmPhoneBookOperationRead::USimReadWriteSizeReq" );

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
    
    cmdParams.fileId = PB_MSISDN_FID;
    cmdParams.serviceType = UICC_APPL_FILE_INFO;
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    
    ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->CreateUiccApplCmdReq( cmdParams );

    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::CreateReqToReadEntry
// Append parametes to request
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead::AddParamToReadReq( TUiccReadLinearFixed& aParams )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::AddParamToReadReq");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_ADDPARAMTOREADREQ, "CMmPhoneBookOperationRead::AddParamToReadReq" );

    TInt ret( KErrNone );
    // Check for the record Number to be read is valid record number
    if( iIndexToRead <= iMmPhoneBookStoreMessHandler->
            iPBStoreConf[iArrayIndex].iNoOfRecords )
        {
        // Check for Valid PhoneBook Entry no.
        if( iIndexToRead > 0)
            {
            aParams.record = iIndexToRead;
            }
        else
            {
            // Start from first location and Search for First Valid
            // Entry in the Stored List and if some Entry is invalid
            // then Read From Sim and Check the Staus its Free
            // or not till Number of slots to be read equals to 0
            
            // read First record
            iIndexToRead = 1;
            aParams.record = iIndexToRead;
            
            // Set Flag for first valid Entry location Search
            iLocationSearch = ETrue;
            }
        }
    else
        {
        // return error for invalid Entry (Out of max range idf entries)
        ret = KErrArgument;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::HandleUICCPbRespL
// Separate request
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationRead::HandleUICCPbRespL
    (
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8 &aFileData,
    TInt aTransId
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleUICCPbRespL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL" );

    TBool complete( EFalse );
    TInt ret(KErrNone);
    // Initialize Application file ID to send in next request
    TBuf8<2> applFieldId;
    

    // Handle possible error case
    if ( UICC_STATUS_OK != aStatus )
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleUICCPbRespL-Unsuccessfully completed by UICC");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL.  Unsuccessfully completed by UICC" );
        
        ret = CMmStaticUtility::UICCCSCauseToEpocError(aStatus );
        }

    switch( iSavedIPCForComplete )
        {
        case EMmTsyPhoneBookStoreReadIPC:
        case EMmTsyONStoreReadIPC:
        case EMmTsyONStoreReadEntryIPC:
            {
            complete = USimPbReadRespL( ret, 
                                aTransId,
                                aFileData );
            break;
            }
        case EMmTsyONStoreReadSizeIPC:
        case EMmTsyONStoreWriteSizeIPC:
            {
            complete = USimReadWriteSizeResp( aFileData, ret );
            break;
            }
        default:
            {
            // Nothing to do here
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUICCPbRespL - Unknown IPC: %d", iSavedIPCForComplete);
OstTrace1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL - Unknown Ipc : %d", iSavedIPCForComplete );
            break;
            }
        }


    return complete;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::USimPbReadRespL
// Handles Response for all phone book read 
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationRead:: USimPbReadRespL( TInt aStatus,
        TUint8 aTransId,
        const TDesC8 &aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimPbReadRespL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_USIMPBREADRESPL, "CMmPhoneBookOperationRead::USimPbReadRespL" );

    TInt ret ( aStatus );
    TBool complete( EFalse );
    TBool entryStored( EFalse );
    TInt emptyEntry( KErrNone );
    
    if ( UICC_STATUS_OK == aStatus )
        {
        if ( EBasicEfRead == iTypeOfReading )
            {
            HandleReadResp( aFileData, aTransId, emptyEntry, entryStored );
            }// End of without EXT File Data Case

         // Handling for Extension Numbers
        else if  ( EExtensionRead == iTypeOfReading )
            {
            HandleExtReadResp( aFileData, aTransId, entryStored );
            }
        else
            {
            TInt mbdnRecNo( aFileData[0] );
            
            iTypeOfReading = EBasicEfRead;
            ret = USimPbReqRead( mbdnRecNo, aTransId );
            }
        }

    if( ( entryStored ) && 
        ( 0 != iNumOfEntriesToRead) )
        {
        iTypeOfReading = EStartRead;
        ret = USimPbReqRead( iIndexToRead, aTransId );
        } // no else
    
    // Complete request
    if( ( KErrNone != ret )|| ( 0 == iNumOfEntriesToRead ))
        {
        // Check fo Any Entries Found
        if( ( UICC_STATUS_OK == aStatus )&& ( 0 == iNumOfEntriesFilled ) )
            {
            ret = KErrNotFound;
            }
        
        if( ( EMmTsyONStoreReadEntryIPC != iSavedIPCForComplete ) &&
                ( EMmTsyONStoreReadIPC != iSavedIPCForComplete ) )
            {
            CPhoneBookDataPackage phoneBookData;
            phoneBookData.SetPhoneBookName( iPhoneBookTypeName );
            iNumOfEntriesFilled = 0;
            iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
                iSavedIPCForComplete,
                &phoneBookData,
                ret );

            iPhoneBookStoreCacheArray = NULL;
            }
        else
            {
            // Store own number Entry and complete the ipc
            StoreAndCompleteOwnNumber( ret, emptyEntry );
            }

        // Set flag to indicate that we can remove this operation from array
        complete = ETrue;
        }
        
    return complete;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::HandleReadResp
// Handles Main Entry file read resp
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead::HandleReadResp( 
        const TDesC8 &aFileData, 
        TUint8 aTransId, 
        TInt &aEmptyEntry, 
        TBool &aEntryStore )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleReadResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_HANDLEREADRESP, "CMmPhoneBookOperationRead::HandleReadResp" );
    
    iStoreEntry = new ( ELeave ) TPBEntry();
    // update Entry Data
    iStoreEntry->iEntryIndex = iIndexToRead;

    // check if Entry is valid Entry or Empty Entry
    aEmptyEntry = EmptyEntryCheck(aFileData);
    
    // if Entry is not empty
    if( KErrNone == aEmptyEntry )
        {
        // Update Entry Status 
        iStoreEntry->iEntryPresent = ETrue;

        // Reset Flag for location Search when first Entry is found
        iLocationSearch = EFalse;
        
        // Seperate Entry data form UICC Server response message
        // Case: <Data available to be filled into array>
        if( iArrayIndex < EPhonebookTypeUnknown )
            {
            iMmPhoneBookStoreMessHandler->
                iPBStoreConf[iArrayIndex].GetPBEntryFromUICCData( aFileData,
                                                             iNumberBuf,
                                                             iNameBuf);
            }
        
        HandleEntryPresentResp( aFileData, aTransId, aEntryStore );
        }
    else
        {
        //Update Entry Status
        iStoreEntry->iEntryPresent = EFalse;
        // Reset Entry andd Append to the List 
        iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
             iStoreEntry,
             iArrayIndex );

        aEntryStore = ETrue;
        // Entry is Empty read next Entry
        if(!iLocationSearch)
            {
            // Decrement the no of Entries to be read
            iNumOfEntriesToRead--;
            }
        }

    // to read next record
    iIndexToRead++;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::HandleEntryPresentResp
// Handles main Entry read resp When Entry is Present
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead::HandleEntryPresentResp( 
        const TDesC8 &aFileData, 
        TUint8 aTransId, 
        TBool &aEntryStore )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleEntryPresentResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_HANDLEENTRYPRESENTRESP, "CMmPhoneBookOperationRead::HandleEntryPresentResp" );

    // Check for Is there any extension data
    // And the Extension data record number is valid
    // Index to read Extension file Data is Alpha string Length + 14
    // minus 1 is for Array index Calculation (it starts from 0)
    
    if ( 0xFF == aFileData[ iMmPhoneBookStoreMessHandler->
            iPBStoreConf[iArrayIndex].iAlphaStringlength+ 13]  )
        {
        if( ( EMmTsyONStoreReadEntryIPC != iSavedIPCForComplete ) &&
                ( EMmTsyONStoreReadIPC != iSavedIPCForComplete ) )
            {
            StoreEntryToListL( aEntryStore );
            }
        else
            {
            // decment for own number read entry
            iNumOfEntriesToRead--;
            iNumOfEntriesFilled++;
            }
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUSimPbRespL - Append entries into array %i",iNumOfEntriesFilled);
OstTrace1( TRACE_NORMAL, DUP4_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL;iNumOfEntriesFilled=%d", iNumOfEntriesFilled );
        } // End of if Ext Data is not Present
    else
        {
        iTypeOfReading = EExtensionRead;
        // Record no to be read from EXT File
        TInt recordNo = aFileData[iMmPhoneBookStoreMessHandler->
            iPBStoreConf[iArrayIndex].iAlphaStringlength + 13];
        
        // Append EXT record no.
        iStoreEntry->PBEntryExtRecord.Append( recordNo );

        // if while reading EXT error comes (for invalid Entry)than
        // read next entry
        if( ( KErrNone != USimPbReqRead( recordNo, aTransId ) ) )
            {
            iNumOfEntriesToRead--;
            iTypeOfReading = EStartRead;
            }  // no else
        }                        
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::HandleExtReadResp
// Handles Ext file read resp
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead::HandleExtReadResp( 
        const TDesC8 &aFileData, 
        TUint8 aTransId,
        TBool &aEntryStored )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleExtReadResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_HANDLEEXTREADRESP, "CMmPhoneBookOperationRead::HandleExtReadResp" );

    // Check for next extension data record
    if ( 0xFF != aFileData[UICC_EXT_REC_NO_OFFSET] )    
        {
        // Again Append the EXT no to Array
        iStoreEntry->PBEntryExtRecord.Append(
              aFileData[UICC_EXT_REC_NO_OFFSET] );

        // Store Number upto last byte
        iNumberBuf.Append(aFileData.Mid(1,11));
        // Again Send request to read next record number and appenf it
        // in number
        if( KErrNone != USimPbReqRead( 
                aFileData[UICC_EXT_REC_NO_OFFSET], aTransId ) )
            {
            iNumOfEntriesToRead--;
            iTypeOfReading = EStartRead;
            } // no else
        }
    else
        {
        // Reset Extension File record
        iExtensionRead = EFalse;

        // Check for Extended Data is Addition number 
        if( 0x02 == aFileData[0])
            {
            // Check for length upto which no is stored
            TInt offset = aFileData.Find(&KTagUnusedbyte,1);
            // store Data
            iNumberBuf.Append(aFileData.Mid(1,( offset - 1 )));
            
            if( ( EMmTsyONStoreReadEntryIPC == iSavedIPCForComplete ) &&
                    ( EMmTsyONStoreReadIPC == iSavedIPCForComplete ) )
                {
                StoreEntryToListL( aEntryStored );
                }
            else
                {
                // decment for own number read entry
                iNumOfEntriesToRead--;
                iNumOfEntriesFilled++;
                }
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUSimPbRespL - Append entries into array %i",iNumOfEntriesFilled);
OstTrace1( TRACE_NORMAL, DUP5_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL;iNumOfEntriesFilled=%d", iNumOfEntriesFilled );
            }
        }
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::USimReadWriteSizeResp
// Send Request to Get Read and Write size
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationRead::USimReadWriteSizeResp( const TDesC8 &aFileData, 
        TInt aStatus )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimReadWriteSizeResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_USIMREADWRITESIZERESP, "CMmPhoneBookOperationRead::USimReadWriteSizeResp" );

    TInt ret( aStatus );
    TInt numOfEntries(0);

    if(UICC_STATUS_OK == aStatus)
        {
        // get the record length and number of entries
        TFci fci( aFileData );
        numOfEntries = fci.GetNumberOfRecords();
        
        // Update it in Internal Buffer
        iMmPhoneBookStoreMessHandler->
              iPBStoreConf[EPhonebookTypeMSISDN].iNoOfRecords = numOfEntries;
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimReadWriteSizeResp - FileInfo read Fail");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD_USIMREADWRITESIZERESP, "CMmPhoneBookOperationRead::USimReadWriteSizeResp - FileInfo Read Fail" );
        
        ret = KErrArgument;
        }
    
    
    // Create data package
    CMmDataPackage numberData;
    // complete request
    numberData.PackData( &numOfEntries );
    iMmPhoneBookStoreMessHandler->MessageRouter()->Complete( 
            iSavedIPCForComplete, 
            &numberData, 
            ret );

    return ETrue;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::StoreEntryToListL
// StoreEntry to internal list and Cache Array
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead::StoreEntryToListL( TBool &aEntryStored )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::StoreEntryToList");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_STOREENTRYTOLIST, "CMmPhoneBookOperationRead::StoreEntryToList" );
 
    iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                  iStoreEntry,
                  iArrayIndex );

    // Decrement the number of entries to be read when it is
    // stored in commonTSY Array
    iNumOfEntriesToRead--;

    // the there is no extension data
    CPhoneBookStoreEntry* phoneBookStoreEntry =
    new( ELeave ) CPhoneBookStoreEntry;
    CleanupStack::PushL( phoneBookStoreEntry );
    phoneBookStoreEntry->ConstructL();
    iMmPhoneBookStoreMessHandler->StorePhonebookEntryL(
        iNameBuf,
        iNumberBuf,
        *phoneBookStoreEntry,
        iFileId,
        iIndexToRead );
    TF_ASSERT( NULL != iPhoneBookStoreCacheArray );

    iPhoneBookStoreCacheArray->AppendL( phoneBookStoreEntry );
    CleanupStack::Pop( phoneBookStoreEntry );
    iNumOfEntriesFilled++;
    aEntryStored = ETrue;
    }





// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::StoreOwnNumber
// StoreEntry to internal list and shared buffer for own number
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead::StoreAndCompleteOwnNumber( TInt aRet , TInt aEmptyEntry)
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::StoreAndCompleteOwnNumber");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_STOREANDCOMPLETEOWNNUMBER, "CMmPhoneBookOperationRead::StoreAndCompleteOwnNumber" );
 
    if( ( EMmTsyONStoreReadEntryIPC == iSavedIPCForComplete ) &&
            ( ( KErrNone != aEmptyEntry ) || ( KErrNone != aRet ) ) )
        {
        // complete ipc with null
        iMmPhoneBookStoreMessHandler->MessageRouter()->Complete( 
                iSavedIPCForComplete, 
                NULL, 
                aRet );
        }
    else
        {
        // Append Entry to list
        iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                      iStoreEntry,
                      iArrayIndex );

        // Decrement the number of entries to be read when it is
        // stored in commonTSY Array
        iNumOfEntriesToRead--;

        // Create ON store structure
        TONStoreMsg* oNStoreMsg = new ( ELeave ) TONStoreMsg();

        // Push oNStoreMsg into CleanupStack
        CleanupStack::PushL( oNStoreMsg );

        // Save Name

        TBuf8<UICC_EF_MAX_NAME_LEN> aNameString;
        // Convert String to 8 bit format
        CMmStaticUtility::ConvertGsmDataToUcs2(iNameBuf, iNameBuf.Length() , aNameString );
        TIsiUtility::CopyFromBigEndian(
                aNameString,
                oNStoreMsg->iName );

        // Convert Number to Ascii Code
        // Save Number
        iMmPhoneBookStoreMessHandler->ConvertToUcs2FromBCD(iNumberBuf, oNStoreMsg->iTelNumber,PB_MBDN_FID );

        // Set location index
        // Map location to client world (increase value by one)
        oNStoreMsg->iLocation = iStoreEntry->iEntryIndex ;

        // Create data package
        CMmDataPackage numberData;
        if( EMmTsyONStoreReadIPC == iSavedIPCForComplete )
            {
            // Complete response with ret
            // Pack the data for sending to the manager
            numberData.PackData( &oNStoreMsg );
            }
        else
            {
            numberData.PackData( oNStoreMsg );
            }
        iMmPhoneBookStoreMessHandler->MessageRouter()->Complete( 
                iSavedIPCForComplete, 
                &numberData, 
                aRet );
        // Delete oNStoreMsg structure
        CleanupStack::PopAndDestroy( oNStoreMsg );

        // Reset current IPC.
        iSavedIPCForComplete = 0;
        iNumOfEntriesFilled++;
        }
    }


// End of File
