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
#include "tsylogger.h"
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationread.h"
#include "cmmphonebookoperationread3g_adn.h"
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
    iTypeOfReading = EBasicEfRead;
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
    const CMmDataPackage* aDataPackage // Data
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::NewL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_NEWL, "CMmPhoneBookOperationRead::NewL" );

    TName phonebookTypeName;

    CMmPhoneBookOperationRead* mmPhoneBookOperationRead =
        new( ELeave ) CMmPhoneBookOperationRead();

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    phoneBookData->GetPhoneBookName( phonebookTypeName );
    
    // Store phoen Book name 
    mmPhoneBookOperationRead->iPhoneBookTypeName = phonebookTypeName;
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
    
    switch( aIpc )
        {
        case EMmTsyPhoneBookStoreReadIPC:
        case EMmTsyONStoreReadIPC:
        case EMmTsyONStoreReadEntryIPC:
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
    // get the index to be read from phonebook
    TInt index( iIndexToRead );

    TUiccReadLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                               ( iMmPhoneBookStoreMessHandler );

    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    cmdParams.record = aRecordNo;
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    // Convert Phone Book name to file id
    TUint16 fileIdExt ( 0x0000 );
    TUint16 pbFileId = ConvertToPBfileId( iPhoneBookTypeName, fileIdExt );
    TUint8 arrayIndex = ConvertToConfArrayIndex( pbFileId );
    
    // get the corect Location to be read from phone book
    if( PB_MBDN_FID == pbFileId)
        {
        // Index to be read contains two types of information.
        // The least significant byte contains the profile number
        // and the most significant byte contains the type.
        index = index || 0x0100; // Only subscriber profile number 1 is supported

        if ( EMailboxIdRead == iTypeOfReading )
            {
            iTypeOfReading = EBasicEfRead;
            }
        else
            {
            iTypeOfReading = EMailboxIdRead;
            }
        }

    // Check for Extension Data is Present or not
    if ( EBasicEfRead == iTypeOfReading )
        {
        cmdParams.fileId = pbFileId;
        // Check for the record Number to be read is valid record number
        if( iIndexToRead <= iMmPhoneBookStoreMessHandler->
                iPBStoreConf[arrayIndex].iNoOfRecords)
            {
            // Check for Number of Slots To be Read
            if( iNumOfEntriesToRead > 0)
                {
                // Check for Valid PhoneBook Entry no.
                if( iIndexToRead > 0)
                    {
                    // Read Request to read that index
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = iIndexToRead;
                    // till End of Record
                    cmdParams.dataAmount = 0;
                    // Start from begining of record
                    cmdParams.dataOffset = 0;
                    }
                else
                    {
                    // Start from first location and Search for First Valid
                    // Entry in the Stored List and if some Entry is invalid
                    // then Read From Sim and Check the Staus its Free
                    // or not till Number of slots to be read equals to 0

                    // Read Request to read that index
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    // read First record
                    iIndexToRead = 1;
                    cmdParams.record = iIndexToRead;
                    // till End of Record
                    cmdParams.dataAmount = 0;
                    // Start from begining of record
                    cmdParams.dataOffset = 0;

                    // Set Flag for first valid Entry location Search
                    iLocationSearch = ETrue;
                    }
                }
            }
        else
            {
            // return error for invalid Entry (Out of max range idf entries)
            ret = KErrArgument;
            }
        }// end of if case for checking extension data
    else if  ( EExtensionRead == iTypeOfReading )
        {
        // Send Request to Read Extension Data
        // Check for UST Table supports for EXT File
        if ( iMmPhoneBookStoreMessHandler->iPBStoreConf[arrayIndex].iExtension )
            {
            // Check for Extension data record in valid
            if ( iMmPhoneBookStoreMessHandler->
                     iPBStoreConf[arrayIndex].iExtNoOfRec >= aRecordNo )
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

    else
        {
        // Read mailbox ID
        if ( iMmPhoneBookStoreMessHandler->
                 iPBStoreConf[arrayIndex].iExtNoOfRec >= aRecordNo )
            {
            // Read Request to read MBI file
            cmdParams.fileId = PB_MBI_FID;
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

    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONREAD_USIMPBREQREAD, "CMmPhoneBookOperationRead::USimPbReqRead;ret=%d", ret );
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
    TInt retExt(KErrNone);
    // Initialize Application file ID to send in next request
    TBuf8<2> applFieldId;
    

    // Handle possible error case
    if ( UICC_STATUS_OK != aStatus )
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleUICCPbRespL-Unsuccessfully completed by UICC");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL.  Unsuccessfully completed by UICC" );
        
        ret = CMmStaticUtility::UICCCSCauseToEpocError(aStatus );
        }

    // Convert Phone Book name to file id
    TUint16 fileIdExt( 0x0000 );
    TUint16 pbFileId = ConvertToPBfileId( iPhoneBookTypeName, fileIdExt );
    TUint8 arrayIndex = ConvertToConfArrayIndex( pbFileId );

    if ( UICC_STATUS_OK == aStatus )
        {
        if ( EBasicEfRead == iTypeOfReading )
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

                // Reset Flag for location Search when first Entry is found
                iLocationSearch = EFalse;
                
                // Seperate Entry data form UICC Server response message
                // Case: <Data available to be filled into array>
                iMmPhoneBookStoreMessHandler->
                    iPBStoreConf[arrayIndex].GetPBEntryFromUICCData( aFileData,
                                                                 iNumberBuf,
                                                                 iNameBuf);
                
                // Check for Is there any extension data
                // And the Extension data record number is valid
                // Index to read Extension file Data is Alpha string Length + 14
                // minus 1 is for Array index Calculation (it starts from 0)
                
                if ( 0xFF == aFileData[ iMmPhoneBookStoreMessHandler->
                        iPBStoreConf[arrayIndex].iAlphaStringlength+ 13]  )
                    {
                    // Append Entry to list
                    iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                                  iStoreEntry,
                                  arrayIndex );

                    // Decrement the number of entries to be read when it is
                    // stored in commonTSY Array
                    iNumOfEntriesToRead--;

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
                        iMailboxIdExist );
                    TF_ASSERT( NULL != iPhoneBookStoreCacheArray );

                    iPhoneBookStoreCacheArray->AppendL( phoneBookStoreMsg );
                    CleanupStack::Pop( phoneBookStoreMsg );
                    iNumOfEntriesFilled++;
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUSimPbRespL - Append entries into array %i",iNumOfEntriesFilled);
OstTrace1( TRACE_NORMAL, DUP4_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL;iNumOfEntriesFilled=%d", iNumOfEntriesFilled );
                    } // End of if Ext Data is not Present
                else
                    {
                    iTypeOfReading = EExtensionRead;
                    // Record no to be read from EXT File
                    TInt recordNo = aFileData[iMmPhoneBookStoreMessHandler->
                        iPBStoreConf[arrayIndex].iAlphaStringlength + 13];
                    
                    // Append EXT record no.
                    iStoreEntry->PBEntryExtRecord.Append( recordNo );

                    retExt = USimPbReqRead( recordNo, aTransId );
                    // if while reading EXT error comes (for invalid Entry)than
                    // read next entry
                    if(( KErrNone != retExt ))
                        {
                        iNumOfEntriesToRead--;
                        iTypeOfReading = EBasicEfRead;
                        }
                    }                        
                }
            else
                {
                //Update Entry Status
                iStoreEntry->iEntryPresent = EFalse;
                // Reset Entry andd Append to the List 
                iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                     iStoreEntry,
                     arrayIndex );

                // Entry is Empty read next Entry
                if(!iLocationSearch)
                    {
                    // Decrement the no of Entries to be read
                    iNumOfEntriesToRead--;
                    }
                }

            // to read next record
            iIndexToRead++;

            }// End of without EXT File Data Case

         // Handling for Extension Numbers
        else if  ( EExtensionRead == iTypeOfReading )
            {
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
                retExt = USimPbReqRead( 
                    aFileData[UICC_EXT_REC_NO_OFFSET], aTransId );
                if( KErrNone != retExt)
                    {
                    iNumOfEntriesToRead--;
                    iTypeOfReading = EBasicEfRead;
                    }
                }
            else
                {
                // Append Entry to list and reset all the EXT data
                iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList(
                                              iStoreEntry,
                                              arrayIndex );

                // Reset Extension File record
                iExtensionRead = EFalse;

                // Check for Extended Data is Addition number 
                if( 0x02 == aFileData[0])
                    {
                    // Check for length upto which no is stored
                    TInt offset = aFileData.Find(&KTagUnusedbyte,1);
                    // store Data
                    iNumberBuf.Append(aFileData.Mid(1,( offset - 1 )));
                    // Decrement no of Entries to be read after Storing it to
                    // CommonTSY Array in EXT data case
                    iNumOfEntriesToRead--;

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
                        iMailboxIdExist );

                    TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
                    iPhoneBookStoreCacheArray->AppendL( phoneBookStoreMsg );
                    CleanupStack::Pop( phoneBookStoreMsg );
                    iNumOfEntriesFilled++;
TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUSimPbRespL - Append entries into array %i",iNumOfEntriesFilled);
OstTrace1( TRACE_NORMAL, DUP5_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL;iNumOfEntriesFilled=%d", iNumOfEntriesFilled );

                    }
                }
            }
        else
            {
            // Read mailbox ID

            // There are 4 bytes describing mailbox id and one of them can
            // be valid at a time
            // 1 Mailbox Dialling Number Identifier – Voicemail
            // 2 Mailbox Dialling Number Identifier – Fax
            // 3 Mailbox Dialling Number Identifier – Electronic Mail
            // 4 Mailbox Dialling Number Identifier – Other

            for ( TUint8 i( 0 ); i < 4; i++ )
                {
                iMailboxIdExist = EFalse;
                if ( 0 != aFileData[i] )
                    {
                    iMailboxIdExist = ETrue;
                    iIndexToRead = aFileData[i];
                    break;
                    }
                }

TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUSimPbRespL - Append entries into array %i",iNumOfEntriesFilled);
            }

        if( ( ( EBasicEfRead == iTypeOfReading ) ||
              ( EMailboxIdRead == iTypeOfReading ) ) &&
              ( 0 != iNumOfEntriesToRead ) )
            {
            ret = USimPbReqRead( iIndexToRead, aTransId );
            }
        }

    // Complete request
    if( ( KErrNone != ret )|| ( 0 == iNumOfEntriesToRead ))
        {
        // Check fo Any Entries Found
        if( ( UICC_STATUS_OK == aStatus )&& ( 0 == iNumOfEntriesFilled ) )
            {
            ret = KErrNotFound;
            }
        
        CPhoneBookDataPackage phoneBookData;
        phoneBookData.SetPhoneBookName( iPhoneBookTypeName );
        iNumOfEntriesFilled = 0;
        iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
            iSavedIPCForComplete,
            &phoneBookData,
            ret );

        iPhoneBookStoreCacheArray = NULL;
        // Set flag to indicate that we can remove this operation from array
        complete = ETrue;
        }
        
    return complete;
    }


// End of File
