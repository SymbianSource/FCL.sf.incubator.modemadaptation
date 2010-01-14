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


//  INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include <pn_const.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <etelmmerr.h>      // For error code
#include <ctsy/serviceapi/gsmerror.h>       // For error code
#include "tsylogger.h"
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationwrite.h"
#include "cmmphonebookalphastring.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationwritetraces.h"
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

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::CMmPhoneBookOperationWrite
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMmPhoneBookOperationWrite::CMmPhoneBookOperationWrite()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::\
        CMmPhoneBookOperationWrite");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_CMMPHONEBOOKOPERATIONWRITE, "CMmPhoneBookOperationWrite::CMmPhoneBookOperationWrite" );
    iPhoneBookEntry = NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::~CMmPhoneBookOperationWrite
// C++ destructor.
// ---------------------------------------------------------------------------
//
CMmPhoneBookOperationWrite::~CMmPhoneBookOperationWrite()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::\
        ~CMmPhoneBookOperationWrite");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_CMMPHONEBOOKOPERATIONWRITE, "CMmPhoneBookOperationWrite::~CMmPhoneBookOperationWrite" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::NewL
// Creates a new CMmPhonebookOperationWrite object instance.
// Two-phased constructor.
// ---------------------------------------------------------------------------

CMmPhoneBookOperationWrite* CMmPhoneBookOperationWrite::NewL(
    CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
    const CMmDataPackage* aDataPackage ) // Data
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::NewL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_NEWL, "CMmPhoneBookOperationWrite::NewL" );

    TName phonebookTypeName;

    CMmPhoneBookOperationWrite* mmPhoneBookOperationWrite =
        new( ELeave ) CMmPhoneBookOperationWrite();

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    phoneBookData->GetPhoneBookName( phonebookTypeName );
    // Store phonebook name 
    mmPhoneBookOperationWrite->iPhoneBookTypeName = phonebookTypeName;

    mmPhoneBookOperationWrite->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;

    return mmPhoneBookOperationWrite;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::ConstructL
// Initialises object attributes.
// ---------------------------------------------------------------------------
//
void CMmPhoneBookOperationWrite::ConstructL()
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::ConstructL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_CONSTRUCTL, "CMmPhoneBookOperationWrite::ConstructL" );
    }


// ---------------------------------------------------------------------------
// TInt CMmPhoneBookOperationWrite::CreateReq
// Separate request
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UICCCreateReq
        (
        TInt aIpc,
        const CMmDataPackage* aDataPackage,
        TUint8 aTransId
        )
    {
TFLOGSTRING2("TSY: CMmPhoneBookOperationWrite::UICCCreateReq Ipc: %d", aIpc);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_UICCCREATEREQ, "CMmPhoneBookOperationWrite::UICCCreateReq;aIpc=%hd", aIpc );
    
    TInt ret (KErrNone);
    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

        if( aIpc == EMmTsyPhoneBookStoreWriteIPC )
            {
                TInt16 index( 0 );
                // Store Entry to the operation which needs to be write in SIM
                phoneBookData->UnPackData( index, iPhoneBookEntry );

                if ( iPhoneBookEntry )
                    {
                    // get the index Number needs to be write
                    iPhoneBookEntry->iLocation = static_cast<TUint16>( index );
                    
                    // Convert Phone Book name to file id
                    TUint16 fileIdExt ( UICC_ILLEGAL_FILE_ID );
                    TUint16 pbFileId = ConvertToPBfileId( iPhoneBookTypeName, fileIdExt );
                    TUint8 pbArrayIndex ( ConvertToConfArrayIndex( pbFileId ) );

                    if( PB_MBDN_FID == pbFileId )
                        {
                        // For MBDN PhoneBook first read MBI file 
                        // Check if the mailbox inidcation type is correct
                        if( index <= iMmPhoneBookStoreMessHandler->
                                iPBStoreConf[pbArrayIndex].iMbiRecLen )
                            {
                            iCurrentWritePhase = EPBWritePhase_Read_MBI_profile;
                            // read MBDN record number from MBI first record Profile number
                            ret = UiccPbReqReadMBI( index, aTransId );
                            }
                        else
                            {
                            ret = KErrArgument;
                            }
                        }
                    else
                        {
                        ret = UiccPbReqWriteL( aTransId, index, *iPhoneBookEntry );
                        }
                    }
            }
        else
            {
TFLOGSTRING2("TSY: CMmPhoneBookOperationWrite::UICCCreateReq Unknown Ipc: %d", aIpc);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONWRITE_UICCCREATEREQ, "CMmPhoneBookOperationWrite::UICCCreateReq;Unknown aIpc =%hd", aIpc );
            }
    return ret;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPbReqWriteL
// Constructs an ISI-message to read entry from SIM
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UiccPbReqWriteL(
        TUint8 aTransId,
        TInt16 aIndex,
        CPhoneBookStoreEntry& aDataToWrite )
    {
TFLOGSTRING3("TSY: CMmPhoneBookOperationWrite::SimPbReqWriteL, aTransId:%d,index:%d", aTransId,  aIndex);
OstTraceExt2( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEL, "CMmPhoneBookOperationWrite::UiccPbReqWriteL;aTransId=%hhu;aIndex=%hd", aTransId, aIndex );
    
    TInt ret( KErrNone );
    // Check for no of EXT records required for Writing this Entry
    TUint8 extRecordNo ( 0 ); 

    if( UICC_EF_FDN_NO_EXT_NUM_LEN < iPhoneBookEntry->iNumber->Length() )
        {
        extRecordNo++;
        TInt extDataLength = iPhoneBookEntry->iNumber->Length() - 
                             UICC_EF_FDN_NO_EXT_NUM_LEN;
        while ( extDataLength > UICC_EF_EXT_REC_NUM_LEN)
            {
            extRecordNo++;
            extDataLength = extDataLength - UICC_EF_EXT_REC_NUM_LEN;
            }
        }
        
    // reserve Space for ext record no
    iExtRecordArrayToBeWrite.ReserveL( extRecordNo );
    // File reserved memory with 0
    for( TInt count = 0; count < extRecordNo; count++ )
        {
        iExtRecordArrayToBeWrite.Insert( 0 , count );
        }

    // Convert Phone Book name to file id
    TUint16 fileIdExt ( UICC_ILLEGAL_FILE_ID );
    TUint16 pbFileId = ConvertToPBfileId( iPhoneBookTypeName, fileIdExt );

    if( UICC_ILLEGAL_FILE_ID != pbFileId )
        {
        TUint8 pbArrayIndex ( ConvertToConfArrayIndex( pbFileId ) );

        // Check if the index To read is valid
        if(  aIndex <= iMmPhoneBookStoreMessHandler->
                iPBStoreConf[pbArrayIndex].iNoOfRecords )
            {
            // if Entry present in list Store it locally
            TPBEntry entry;
            if( 0 < aIndex )
                {
                // Check for index present the store list
                if( iMmPhoneBookStoreMessHandler->IndexCheckInPBList(
                        aIndex,
                        pbArrayIndex,
                        entry ) )
                    {
                    // Check if Entry is present or not 
                    if( entry.iEntryPresent )
                        {
                        // Check if no if ext records are sufficient from previous Entry
                        if( extRecordNo  > entry.PBEntryExtRecord.Count() )
                            {
                            // Update old record number which needs to be updated by new entry
                            if( entry.PBEntryExtRecord.Count() > 0 )
                                {
                                for( TInt i=0; i <  entry.PBEntryExtRecord.Count(); i++ )
                                    {
                                    // Append record number to be write from old Entry
                                    iExtRecordArrayToBeWrite.Append( 
                                            entry.PBEntryExtRecord[i] );
                                    iExtRecordArrayToBeDelete.Append( 
                                            entry.PBEntryExtRecord[i] );
                                    }
                                }
                            // Search for more free records
                            iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
                            
                            // Send request to read first entry in Ext
                            iExtRecordNo = 1 ;
                            ret = UiccPbReqWriteReadExt(
                                    pbFileId,
                                    fileIdExt,
                                    iExtRecordNo,
                                    aTransId);
                            }
                        else  // when reocrds to be delete and write are 0 or to be write record less
                            {
                            // Check of both records are 0
                            if( ( extRecordNo == 0 ) && 
                                    ( entry.PBEntryExtRecord.Count() == 0 ))
                                {
                                // Start Writing Entry
                                iCurrentWritePhase = EPBWritePhase_Write_Entry;
                                ret = UiccPBReqWriteEntry( 
                                        pbFileId,
                                        aIndex,
                                        aTransId,
                                        aDataToWrite );
                                }
                            else
                                {
                                // Start copy Ext record number to be write array
                                for( TInt i=0; i <  extRecordNo; i++ )
                                    {
                                    // Append record number to be write from old Entry
                                    iExtRecordArrayToBeWrite.Append(
                                            entry.PBEntryExtRecord[i]);
                                    iExtRecordArrayToBeDelete.Append(
                                            entry.PBEntryExtRecord[i]);
                                    }
                                // Update records needs to be deleted
                                // From old Entry
                                for( TInt i=extRecordNo; i <  entry.PBEntryExtRecord.Count(); i++ )
                                    {
                                    // Append record number to be deleted from old Entry
                                    iExtRecordArrayToBeDelete.Append( 
                                            entry.PBEntryExtRecord[i]);
                                    }
                                // Start Deleting the EXT Records
                                iCurrentWritePhase = EPBWritePhase_Write_Ext_Entry;
                                iExtDeleteOperation = ETrue;
                                TUint8 index ( iExtRecordArrayToBeDelete[0] );
                                // Send request to Write main Entry
                                ret = UiccPbReqWriteExtDelete(
                                        pbFileId,
                                        fileIdExt,
                                        index,
                                        aTransId );
                                }
                            }
                        }
                    else
                        {
                        // If entry not present
                        // Check if EXT Records are needed for Entry to be written
                        if( 0 < extRecordNo )
                            {
                            // Search for free ext records 
                            iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
                            
                            // Send request To read First EXT record
                            iExtRecordNo = 1;
                            ret = UiccPbReqWriteReadExt(
                                    pbFileId,
                                    fileIdExt,
                                    iExtRecordNo,
                                    aTransId );
                            }
                        else
                            {
                            // Start Writting Entry
                            iCurrentWritePhase = EPBWritePhase_Write_Entry;
                            TUint8 indexToWrite = iPhoneBookEntry->iLocation;
                            ret = UiccPBReqWriteEntry(
                                    pbFileId,
                                    indexToWrite,
                                    aTransId,
                                    aDataToWrite );
                            }
                        }
                    }   // end of if Entry is present in the list
                else
                    {
                    // First Read that Entry from SIM and then delete that entry with EXT records if there are Any
                    iCurrentWritePhase = EPBWritePhase_Read_Entry;
                    ret = UiccPbReqWriteRead( pbFileId, aIndex, aTransId );
                    
                    // read Main entry with given index number
                    }
                }
            else
                {
                // Start Serching For first free Entry in the Stored list
                TInt index = iMmPhoneBookStoreMessHandler->
                EmptyEntryCheckInPBList( pbArrayIndex );
                if( 0 < index )
                    {
                    //Check if EXT records Are needed or not
                    if( 0 < extRecordNo )
                        {
                        // Start Serching for EXT records
                        // Search for free ext records 
                        iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
                            
                        // Send request To read First EXT record
                        iExtRecordNo = 1;
                        ret = UiccPbReqWriteReadExt(
                                pbFileId,
                                fileIdExt,
                                iExtRecordNo,
                                aTransId );
                        }
                    else
                        {
                        // Start Writing Entry on given Index
                        // Start Writting Entry
                        iCurrentWritePhase = EPBWritePhase_Write_Entry;
                        ret = UiccPBReqWriteEntry(
                                pbFileId,
                                index,
                                aTransId,
                                aDataToWrite );
                        }
                    }
                else
                    {
                    // Start searching for first free Entry from SIM
                    iCurrentWritePhase = EPBWritePhase_Read_Entry;
                    iLocationSearch = ETrue;
                    iEntryToWrite = 1;
                    ret = UiccPbReqWriteRead(
                            pbFileId,
                            iEntryToWrite,
                            aTransId );
                    }
                }
            }
        else
            {
            ret = KErrArgument;
            }            
        }
    else
        {
        ret = KErrArgument;
        }
    return ret;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPBReqWriteEntry
// Constructs an ISI-message to Write main Entry data
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UiccPBReqWriteEntry( TUint16 aFileId, TUint8 aIndex, TUint8 aTransId, CPhoneBookStoreEntry& aDataToWrite )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::UiccPBReqWriteEntry");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEENTRY, "CMmPhoneBookOperationWrite::UiccPBReqWriteEntry" );

    TInt ret( KErrNone );
    
    TUiccWriteLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                               ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    cmdParams.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;
    cmdParams.fileId = aFileId;
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    // Check for valid record number
    TUint8 arrayIndex ( ConvertToConfArrayIndex( aFileId ) );
    if( aIndex <= iMmPhoneBookStoreMessHandler->iPBStoreConf[arrayIndex].iNoOfRecords )
        {
        cmdParams.record = aIndex;
        }
    else
        {
        ret = KErrArgument;
        }
    
    
    TBuf8<241>nameBuf;
    
    TPtr16 ptrToName = aDataToWrite.iText->Des();
    // Convert the Entry Name from ASCII format to UCS2
    CMmStaticUtility::ConvertUcs2ToGsmUcs2Data( ptrToName, nameBuf );
    if( nameBuf.Length() < iMmPhoneBookStoreMessHandler->
                           iPBStoreConf[ arrayIndex ].iAlphaStringlength )
        {
        // Fill rest of the Bytes 
        for( TInt count = nameBuf.Length(); count < ( iMmPhoneBookStoreMessHandler->
                      iPBStoreConf[ arrayIndex ].iAlphaStringlength ); count++ )
            {
            nameBuf.Append( KTagUnusedbyte );
            }
        }
    TBuf8<300>numberBuf;
    TPtr16 ptrToNumber = aDataToWrite.iNumber->Des();
    iMmPhoneBookStoreMessHandler->ConvertToBCDFromUCS2(
                                  ptrToNumber,
                                  numberBuf,
                                  aFileId );
    
    // Store number length
    TUint8 numLength ( numberBuf.Length() );
    
    if( numberBuf.Length() < UICC_EF_EXT_REC_NUM_LEN )
        {
        // fill  rest of the bytes
        for( TInt count = numberBuf.Length(); count < UICC_EF_EXT_REC_NUM_LEN; count++)
            {
            numberBuf.Append( KTagUnusedbyte );
            }
        }
    
    TBuf8<200>fileDataWrite;
    fileDataWrite.Append( nameBuf.Mid( 0, iMmPhoneBookStoreMessHandler->
                          iPBStoreConf[ arrayIndex ].iAlphaStringlength ) );
    fileDataWrite.Append( numLength );
    fileDataWrite.Append( numberBuf.Mid( 0, UICC_EF_EXT_REC_NUM_LEN ) );
    // Append Capability configuration byte as not used
    fileDataWrite.Append( KTagUnusedbyte );

    // Chekc for EXT Data is present or not
    if( 0 < iExtRecordArrayToBeWrite.Count() )
        {
        // Append Extension recorNo from iExtRecordArray to be write
        fileDataWrite.Append( iExtRecordArrayToBeWrite[0] );
        }
    else
        {
        // there is no EXT record
        fileDataWrite.Append( KTagUnusedbyte );
        }
    
    // Append 
    cmdParams.fileData.Append( fileDataWrite );
    
    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEENTRY, "CMmPhoneBookOperationWrite::UiccPBReqWriteEntry;ret=%hd", ret );
        
        }

    return ret;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPbReqWriteExt
// Constructs an ISI-message to Write EXT Data 
// ---------------------------------------------------------------------------
//

TInt CMmPhoneBookOperationWrite::UiccPbReqWriteExt(
        TUint8 aTransId,
        TUint8 aIndex,
        TUint16 aFileId,
        TUint16 aFileIdExt,
        CPhoneBookStoreEntry& aDataToWrite )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::UiccPbReqWriteExt");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBWRITEEXT, "CMmPhoneBookOperationWrite::UiccPbReqWriteExt" );
    
    TInt ret( KErrNone );
    
    TUiccWriteLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                                ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    cmdParams.fileId = aFileIdExt;
    cmdParams.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    
    // File Data
    TBuf8<255>numBuf;
    TBuf8<13>extFileData;
    TPtr16 ptrToNumber = aDataToWrite.iNumber->Des();

    iMmPhoneBookStoreMessHandler->ConvertToBCDFromUCS2(
                                  ptrToNumber,
                                  numBuf,
                                  aFileId );
    
        if( numBuf.Length() > ( 10 + ( iExtRecordWritten* 11 ) ))
            {
            // form EXt file Data
            // Append Record type
            extFileData.Append( 0x02 );
            // Append Data
            extFileData.Append( numBuf.Mid( 11 + ( iExtRecordWritten* 11 ), 
                     ( numBuf.Length() - ( 11 + ( iExtRecordWritten* 11 ) ) ) ) );
            // Check if Data ids less than 11
            if( extFileData.Length() < 12 )
                {
                extFileData.AppendFill( 0xFF, ( 12 - extFileData.Length() ) );
                }
            // Check if there is more Ext Record to be write
            if( iExtRecordArrayToBeWrite.Count() > 0 )
                {
                // Append record Number
                extFileData.Append( iExtRecordArrayToBeWrite[0] );
                }
            else
                {
                extFileData.Append( 0xFF );
                }
            }
        else
            {
            // Append Record type
            extFileData.Append( 0x02 );
            // Append Data
            extFileData.Append( numBuf.Mid( 11 + ( iExtRecordWritten* 11 ), 11 ) );
            
            // Check if there is more Ext Record to be write
            if( iExtRecordArrayToBeWrite.Count() > 0 )
                {
                // Append record Number
                extFileData.Append( iExtRecordArrayToBeWrite[0] );
                }
            else
                {
                extFileData.Append( 0xFF );
                }
            }
        
        cmdParams.fileData.Append( extFileData );

    TUint8 arrayIndex ( ConvertToConfArrayIndex( aFileId ));
    if( aIndex <= iMmPhoneBookStoreMessHandler->iPBStoreConf[arrayIndex].iExtNoOfRec )
        {
        cmdParams.record = aIndex;
        }
    else
        {
        ret = KErrGeneral;
        }
    
    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTraceExt1( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEEXT, "CMmPhoneBookOperationWrite::UiccPbReqWriteExt;ret=%hd", ret );
        }
    
    return ret;
    }





// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPbReqWriteReadExt
// Constructs an ISI-message to Read The existing Entry EXT record
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UiccPbReqWriteReadExt(
             TUint16 aFileId,
             TUint16 aFileIdExt,
             TUint8 aIndex,
             TUint8 aTransId)
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::UiccPbReqWriteEXTDelete");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEREADEXT, "CMmPhoneBookOperationWrite::UiccPbReqWriteReadExt" );
    
    TInt ret ( KErrNone );
    
    TUiccReadLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                               ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    
    // Append transaction id 
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    
    // Append File id
    cmdParams.fileId = aFileIdExt;
    
    // Append serviceType
    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    
    
    TUint8 arrayIndex( ConvertToConfArrayIndex( aFileId ) );
    // Check for the valid index neds to be read
    if( aIndex <= iMmPhoneBookStoreMessHandler->iPBStoreConf[arrayIndex].iExtNoOfRec )
        {
        // Append recor number
        cmdParams.record = aIndex;
        }
    else
        {
        ret = KErrGeneral;
        }
    
    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEREADEXT, "CMmPhoneBookOperationWrite::UiccPbReqWriteReadExt;ret=%hd", ret );
        }

    return ret;
    
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPbReqWriteRead
// Constructs an ISI-message to Read The existing Entry
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UiccPbReqWriteRead(
                     TUint16 aFileId,
                     TUint8 aIndex,
                     TUint8 aTransId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::UiccPbReqWriteRead");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEREAD, "CMmPhoneBookOperationWrite::UiccPbReqWriteRead" );
    
    TInt ret ( KErrNone );
    
    TUiccReadLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                               ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    cmdParams.fileId = aFileId;
    cmdParams.serviceType =  UICC_APPL_READ_LINEAR_FIXED ;
    TUint8 arrayIndex ( ConvertToConfArrayIndex( aFileId ) );
    if( aIndex <= iMmPhoneBookStoreMessHandler->iPBStoreConf[arrayIndex].iNoOfRecords )
        {
        cmdParams.record = aIndex;
        }
    else
        {
        ret = KErrOverflow;
        }
    
    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEREAD, "CMmPhoneBookOperationWrite::UiccPbReqWriteRead;ret=%hd", ret );
        }

    return ret;

    }
// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPbReqWriteEXTDelete
// Constructs an ISI-message to delete EXT File Data
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UiccPbReqWriteExtDelete(
                  TUint16 aFileId,
                  TUint16 aFileIdExt,
                  TUint8 aIndex,
                  TUint8 aTransId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::UiccPbReqWriteEXTDelete");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEEXTDELETE, "CMmPhoneBookOperationWrite::UiccPbReqWriteEXTDelete" );
    
    TInt ret ( KErrNone );
    TBuf8<13>deleteEntryData;
    
    TUiccWriteLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                               ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    cmdParams.fileId = aFileIdExt;
    cmdParams.serviceType =  UICC_APPL_UPDATE_LINEAR_FIXED ;
    
    TUint8 arrayIndex ( ConvertToConfArrayIndex( aFileId ) );

    
    if( aIndex <= iMmPhoneBookStoreMessHandler->iPBStoreConf[arrayIndex].iExtNoOfRec)
        {
        cmdParams.record = aIndex;
        }
    else
        {
        ret = KErrGeneral;
        }
    
    
    // Append record Type Byte
    deleteEntryData.Append( 0x00 );
    // Fill Empty record Data
    deleteEntryData.AppendFill( 0xFF, 11 );
    // Fill the next record number
    deleteEntryData.Append( 0xFF );
    
    cmdParams.fileData.Append( deleteEntryData );

    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEEXTDELETE, "CMmPhoneBookOperationWrite::UiccPbReqWriteExtDelete;ret=%hd", ret );
        }

    return ret ;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPbReqReadMBI
// Constructs an ISI-message to Read MBI profile from first record 
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UiccPbReqReadMBI( TUint8 aIndex, TUint8 aTransId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::UiccPbReqReadMBI");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_UICCPBREQREADMBI, "CMmPhoneBookOperationWrite::UiccPbReqReadMBI" );
        
        TInt ret ( KErrNone );
        TUiccReadLinearFixed cmdParams;
        cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                                   ( iMmPhoneBookStoreMessHandler );
        cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
        cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
        cmdParams.filePath.Append( APPL_FILE_ID>>8);
        cmdParams.filePath.Append( APPL_FILE_ID);
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
        cmdParams.trId = static_cast<TUiccTrId>( aTransId );
        cmdParams.fileId = PB_MBI_FID;
        cmdParams.serviceType =  UICC_APPL_READ_LINEAR_FIXED ;
        cmdParams.dataAmount = 1;
        cmdParams.dataOffset = aIndex;
        cmdParams.record = 1;   // only first profile number is supported
        
        
        if( KErrNone == ret )
            {
            ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
                CreateUiccApplCmdReq( cmdParams );
            }
        return ret;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::UiccPBReqWriteMBIProfile
// Send Request for MBBI Profile Write
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::UiccPBReqWriteMBIProfile( 
        TUint8 aTransId,
        TUint8 aIndex,
        TUint8 aOperationType)
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::UiccPBReqWriteMBIProfile");
OstTrace0( TRACE_FATAL, CMMPHONEBOOKOPERATIONWRITE_UICCPBREQWRITEMBIPROFILE, "CMmPhoneBookOperationWrite::UiccPBReqWriteMBIProfile" );

    TInt ret ( KErrNone );
    
    TUiccWriteLinearFixed cmdParams;
    cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> 
                               ( iMmPhoneBookStoreMessHandler );
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append( APPL_FILE_ID>>8);
    cmdParams.filePath.Append( APPL_FILE_ID);
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));
    cmdParams.trId = static_cast<TUiccTrId>( aTransId );
    cmdParams.fileId = PB_MBI_FID;
    cmdParams.serviceType =  UICC_APPL_READ_LINEAR_FIXED ;
    cmdParams.dataAmount = 1;
    cmdParams.dataOffset = aIndex;
    cmdParams.record = 1;   // only first profile number is supported
    
    if( KOperationDelete == aOperationType )
        {
        cmdParams.fileData.Append( 0 );
        }
    else
        {
        cmdParams.fileData.Append( iEntryToWrite );
        }
    
    
    if( KErrNone == ret )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
        }

    return ret;
    
    }
// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::HandleUICCPbRespL
// Separate response
// ---------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationWrite::HandleUICCPbRespL(
    TInt aStatus,
    TUint8 aDetails,
    const TDesC8& aFileData,
    TInt aTransId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleUICCPbRespL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationWrite::HandleUICCPbRespL" );
    
    TBool complete ( EFalse );
    TInt ret ( KErrNone );
    
    TUint16 fileIdExt ( 0x0000 );
    TUint16 fileId = ConvertToPBfileId( iPhoneBookTypeName, fileIdExt );
    //// Check for PhoenBook Index for which needs to checked
    TUint8 pbArrayIndex = ConvertToConfArrayIndex( fileId );

    switch( iCurrentWritePhase )
        {
        case EPBWritePhase_Read_Entry:
            {
            ret = HandleWriteReadEntryResp(
                  aStatus,
                  aFileData,
                  aTransId,
                  fileId,
                  fileIdExt,
                  pbArrayIndex );
            break;
            }
        case EPBWritePhase_Read_Ext_Entry:
            {
            ret = HandleWriteReadExtEntryResp(
                  aStatus,
                  aFileData,
                  aTransId,
                  fileId,
                  fileIdExt );
            break;
            }
        case EPBWritePhase_Search_Ext_Entry:
            {
            ret = HandleWriteSearchExtEntryResp(
                  aStatus,
                  aFileData,
                  aTransId,
                  fileId,
                  fileIdExt );
            break;
            }
        case EPBWritePhase_Write_Entry:
            {
            ret = HandleWriteEntryResp(
                  aStatus,
                  aTransId,
                  fileId,
                  fileIdExt );
            break;
            }
        case EPBWritePhase_Write_Ext_Entry:
            {
            ret = HandleWriteExtEntryResp(
                  aStatus,
                  aTransId,
                  fileId,
                  fileIdExt );
            break;
            }
        case EPBWritePhase_Read_MBI_profile:
            {
            ret = HandleWriteMBIReadResp(
                    aStatus,
                    aDetails,
                    aTransId,
                    aFileData );
            break;
            }
        case EPBWritePhase_Write_MBI_profile:
            {
            ret = HandleWriteMBIWriteResp(
                    aStatus,
                    aDetails);
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleSimPbRespL - NO such Write operation Phase supported ");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_HANDLEUICCPBRESPL, "CMmPhoneBookOperationWrite::HandleUICCPbRespL - No such write operation phase supported" );
            break;
            }
        }
    
    if( ( KErrNone != ret ) || ( iCurrentWritePhase == EPBWritePhase_complete ))
        {
        // Complete the to common TSY
        CPhoneBookDataPackage phoneBookData;

        //create a struct
        TPBEntryInfo pbEntryInfo;
        // Set the structure as it has failed, we don't care of following
        // information so set to 0
        pbEntryInfo.iMaxNumLength = 0;
        pbEntryInfo.iLocation = 0;
        if( KErrNone == ret )
            {
            pbEntryInfo.iLocation = iPhoneBookEntry->iLocation;
            }
        // For EXT file Errors need not to pass to commontsy
        if( KErrGeneral == ret )
            {
            ret = KErrNone;
            }
        phoneBookData.SetPhoneBookName( iPhoneBookTypeName );

        //Pack Data and complete
        phoneBookData.PackData( &pbEntryInfo );
        iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
            EMmTsyPhoneBookStoreWriteIPC,
            &phoneBookData,
            ret );

        // set flag to indicate that we can remove this operation from array
        complete = ETrue;
        }
    return complete;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::HandleWriteReadEntryResp
// Handle response for Read Entry Phase
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::HandleWriteReadEntryResp(
    TInt aStatus,
    const TDesC8& aFileData,
    TUint8 aTransId,
    TUint16 aFileId,
    TUint16 aFileIdExt,
    TUint8 aArrayIndex )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleWriteReadEntryResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_HANDLEWRITEREADENTRYRESP, "CMmPhoneBookOperationWrite::HandleWriteReadEntryResp" );
    TInt ret ( KErrNone );
    
    if( UICC_STATUS_OK == aStatus )
        {
        // Check for Emtry Entry
        if( KErrNone == EmptyEntryCheck( aFileData ) )
            {
            // if entry is present
            // Check if its a location search
            if( !iLocationSearch )
                {
                
                // Check for Ext data present or not
                if( 0xFF == aFileData[iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[aArrayIndex].iAlphaStringlength+ 13] )
                    {
                    // No EXT Records 
                    // Check if EXT reords needed for Entry to be write
                    if( ( KErrNotFound != iExtRecordArrayToBeWrite.Find( 0 ) ))
                        {
                        // Start Search for Ext record
                        // Search for more free records
                        iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
                        
                        // Send request to read first entry in Ext
                        iExtRecordNo = 1;
                        ret = UiccPbReqWriteReadExt(
                              aFileId,
                              aFileIdExt,
                              iExtRecordNo,
                              aTransId );
                        }
                    else
                        {
                        // Start Write main Entry
                        iCurrentWritePhase = EPBWritePhase_Write_Entry;
                        TUint8 indexToWrite ( 0 );
                        if( PB_MBDN_FID == aFileId )
                            {
                            indexToWrite = iEntryToWrite;
                            }
                        else
                            {
                            indexToWrite = iPhoneBookEntry->iLocation;
                            }
                        ret = UiccPBReqWriteEntry(
                              aFileId,
                              indexToWrite,
                              aTransId,
                              *iPhoneBookEntry );
                        }
                    }
                else
                    {
                    // Append this Record number to be Delete record Array
                    iExtRecordArrayToBeDelete.Append(
                                    iMmPhoneBookStoreMessHandler->
                                    iPBStoreConf[aArrayIndex].iAlphaStringlength+ 13 );
                    
                    // Start Reading EXT Record
                    iCurrentWritePhase = EPBWritePhase_Read_Ext_Entry;
                    TUint8 indexToRead = iMmPhoneBookStoreMessHandler->
                               iPBStoreConf[aArrayIndex].iAlphaStringlength+ 13;
                    ret = UiccPbReqWriteReadExt(
                          aFileId,
                          aFileIdExt,
                          indexToRead,
                          aTransId );
                    }
                }
            else
                {
                // Serach for next location
                // Start searching for first free Entry
                iCurrentWritePhase = EPBWritePhase_Read_Entry;
                iEntryToWrite++;
                ret = UiccPbReqWriteRead(
                      aFileId,
                      iEntryToWrite,
                      aTransId );
                }
            }
        else
            {
            // entry not found and
            // Check if EXT reords needed for Entry to be write
            iLocationSearch = EFalse;
            if( 0 < iExtRecordArrayToBeWrite.Count() )
                {
                // Start Search for Ext record
                // Search for more free records
                iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
                
                // Send request to read first entry in Ext
                iExtRecordNo = 1;
                ret = UiccPbReqWriteReadExt(
                      aFileId,
                      aFileIdExt,
                      iExtRecordNo,
                      aTransId);
                }
            else
                {
                // For MBDN PhoneBook First update MBI File
                
                // Start Write Entry
                iCurrentWritePhase = EPBWritePhase_Write_Entry;
                TUint8 index ( 0 ); 
                if( ( PB_MBDN_FID == aFileId )|| ( 0 == iPhoneBookEntry->iLocation ))
                    {
                    index = iEntryToWrite; 
                    }
                else
                    {
                    index = iPhoneBookEntry->iLocation;
                    }
                ret = UiccPBReqWriteEntry(
                        aFileId,
                        index,
                        aTransId,
                        *iPhoneBookEntry );
                }
            }
        }
    else
        {
        ret = KErrGeneral;
        }

    return ret;
    }





// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::HandleWriteReadExtEntryResp
// Handle Read EXt fiel response
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::HandleWriteReadExtEntryResp(
    TInt aStatus,
    const TDesC8& aFileData,
    TUint8 aTransId,
    TUint16 aFileId,
    TUint16 aFileIdExt)
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleWriteReadExtEntryResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_HANDLEWRITEREADEXTENTRYRESP, "CMmPhoneBookOperationWrite::HandleWriteReadExtEntryResp" );
    TInt ret ( KErrNone );
    
    if( UICC_STATUS_OK == aStatus )
        {
        // Check if There is further records for this Entry
        if( 0xFF == aFileData[UICC_EXT_REC_NO_OFFSET] )
            {
            // There is no Further records
            if( 0 < iExtRecordArrayToBeWrite.Count() )
                {
                // Check if to be delete records are sufficient to write new entry
                if( iExtRecordArrayToBeWrite.Count() <= 
                    iExtRecordArrayToBeDelete.Count())
                    {
                    // NO need to search for new records
                    // Copy record nos
                    for( TInt i = 0; i<iExtRecordArrayToBeWrite.Count(); i++)
                        {
                        iExtRecordArrayToBeWrite[i] = iExtRecordArrayToBeDelete[i];
                        }
                    // Start Deleting Ext data
                    iCurrentWritePhase = EPBWritePhase_Write_Ext_Entry;
                    iExtDeleteOperation= ETrue;
                    //last index
                    TUint8 index = iExtRecordArrayToBeDelete[ iExtRecordArrayToBeDelete.Count() -1 ];
                    ret = UiccPbReqWriteExtDelete(
                          aFileId,
                          aFileIdExt,
                          index,
                          aTransId );
                    }
                else
                    {
                    // Copy the records
                    for( TInt i = 0; i<iExtRecordArrayToBeDelete.Count(); i++)
                        {
                        iExtRecordArrayToBeWrite[i] = iExtRecordArrayToBeDelete[i];
                        }
                    // Start Searching for remaining records
                    iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
                    // Start Searching for first location
                    iExtRecordNo = 1;
                    ret = UiccPbReqWriteReadExt(
                          aFileId,
                          aFileIdExt,
                          iExtRecordNo,
                          aTransId );
                    }
                }
            else
                {
                // Start Writing EXT Data delete
                if( 0 < iExtRecordArrayToBeDelete.Count() )
                    {
                    // Start Writing Ext data
                    iCurrentWritePhase = EPBWritePhase_Write_Ext_Entry;
                    iExtDeleteOperation = ETrue;
                    //last index
                    TUint8 index = 
                      iExtRecordArrayToBeDelete[ iExtRecordArrayToBeDelete.Count() -1 ];
                    ret = UiccPbReqWriteExtDelete(
                          aFileId,
                          aFileIdExt,
                          index,
                          aTransId );
                    }
                else
                    {
                    // Start Writing main Entry
                    iCurrentWritePhase = EPBWritePhase_Write_Entry;
                    //last index
                    TUint8 index (0 );
                    if( PB_MBDN_FID == aFileId )
                        {
                        index = iEntryToWrite;
                        }
                    else
                        {
                        index = iPhoneBookEntry->iLocation;
                        }
                    ret = UiccPBReqWriteEntry(
                          aFileId,
                          index,
                          aTransId,
                          *iPhoneBookEntry );
                    }
                }
            }
        else
            {
            // Append Current record no to the Array
            iExtRecordArrayToBeDelete.Append( aFileData[UICC_EXT_REC_NO_OFFSET] );
            // Raed Next 
            iCurrentWritePhase = EPBWritePhase_Read_Ext_Entry;
            ret = UiccPbReqWriteReadExt(
                  aFileId,
                  aFileIdExt,
                  aFileData[UICC_EXT_REC_NO_OFFSET],
                  aTransId );
            }
        }
    else
        {
            ret = KErrGeneral;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookOperationWrite::HandleWriteSearchExtEntryResp
// Handle Search for Ext record response
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::HandleWriteSearchExtEntryResp(
    TInt aStatus,
    const TDesC8& aFileData,
    TUint8 aTransId,
    TUint16 aFileId,
    TUint16 aFileIdExt)
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleWriteSearchExtEntryResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_HANDLEWRITESEARCHEXTENTRYRESP, "CMmPhoneBookOperationWrite::HandleWriteSearchExtEntryResp" );
    TInt ret ( KErrNone );
    TUint8 arrayIndex ( ConvertToConfArrayIndex( aFileId ) );
    
    if( UICC_STATUS_OK == aStatus)
        {
        // Check if Entry is Free
        if( KErrNotFound == EmptyEntryCheck( aFileData ) )
            {
            TInt location ( iExtRecordArrayToBeWrite.Find ( 0 ) );
            // Append this Record no in Array
            iExtRecordArrayToBeWrite[location] = iExtRecordNo;
            // Check if there is any record no 0 in to be write record array
            if( KErrNotFound == iExtRecordArrayToBeWrite.Find ( 0 ) )
                {
                // Start Writing to EXT records
                // first do the delete records
                if( iExtRecordArrayToBeDelete.Count() > 0 )
                    {
                    // Start Writing or deleting the Entry
                    iCurrentWritePhase = EPBWritePhase_Write_Ext_Entry;
                    iExtDeleteOperation = ETrue;
                    TUint8 index = 
                    iExtRecordArrayToBeDelete[ iExtRecordArrayToBeDelete.Count() -1 ];
                    ret = UiccPbReqWriteExtDelete( 
                            aFileId,
                            aFileIdExt,
                            index,
                            aTransId );
                    iExtRecordArrayToBeDelete.Remove( 
                            iExtRecordArrayToBeDelete.Count() -1 );
                    iExtRecordArrayToBeDelete.Compress();
                    }
                else 
                    {
                    // Start Writing the Entry
                    iCurrentWritePhase = EPBWritePhase_Write_Entry;
                    TUint8 index ( 0 );
                        // Start writing main Entry
                    if( ( iPhoneBookEntry->iLocation == 0 )||( PB_MBDN_FID == aFileId ) )
                        {
                        index = iEntryToWrite;
                        }
                    else
                        {
                        index = iPhoneBookEntry->iLocation;
                        }
                    if( 0 < index )
                        {
                        ret = UiccPBReqWriteEntry(
                                aFileId,
                                index,
                                aTransId,
                                *iPhoneBookEntry );
                        }
                    else
                        {
                        ret = KErrNotFound;
                        }
                    }
                }
            else
                {
                // further search for free records
                iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
                TUint8 index = iExtRecordNo++;
                if( index < iMmPhoneBookStoreMessHandler->
                            iPBStoreConf[ arrayIndex ].iExtNoOfRec )
                    {
                    ret = UiccPbReqWriteReadExt(
                          aFileId,
                          aFileIdExt,
                          index,
                          aTransId );
                    }
                else
                    {
                    ret = KErrGeneral;
                    }
                }
            }
        else
            {
            // further search for free records
            iCurrentWritePhase = EPBWritePhase_Search_Ext_Entry;
            TUint8 index = iExtRecordNo++;
            if( index < iMmPhoneBookStoreMessHandler->
                        iPBStoreConf[ arrayIndex ].iExtNoOfRec )
                {
                ret = UiccPbReqWriteReadExt(
                      aFileId,
                      aFileIdExt,
                      index,
                      aTransId );
                }
            else
                {
                ret = KErrGeneral;
                }
            }
        }
    else
        {
        ret = KErrGeneral;
        }

    return ret;
    }



// ---------------------------------------------------------------------------
// TBool CMmPhoneBookOperationWrite::HandleWriteEntryResp
// Handle write response for EXT part of Entry
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::HandleWriteEntryResp(
    TInt aStatus,
    TUint8 aTransId,
    TUint16 aFileId,
    TUint16 aFileIdExt) 
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleWriteEntryResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_HANDLEWRITEENTRYRESP, "CMmPhoneBookOperationWrite::HandleWriteEntryResp" );
    TInt ret ( KErrNone );
    TUint8 arrayIndex ( ConvertToConfArrayIndex( aFileId ));
    
    if( UICC_STATUS_OK == aStatus )
        {
        iEntryToUpdateInList = new ( ELeave ) TPBEntry();
        
        // Update the Entry needs to be Stored
        if( ( 0 == iPhoneBookEntry->iLocation )|| ( PB_MBDN_FID == aFileId ) )
            {
            iEntryToUpdateInList->iEntryIndex = iEntryToWrite;
            iEntryToUpdateInList->iEntryPresent = ETrue;
            }
        else
            {
            iEntryToUpdateInList->iEntryIndex = iPhoneBookEntry->iLocation;
            iEntryToUpdateInList->iEntryPresent = ETrue;
            }

        // Check there is ant EXT Data to be Write or Delete
        if( iExtRecordArrayToBeWrite.Count() > 0 )
            {
            // Update Entry
            iEntryToUpdateInList->PBEntryExtRecord.Append( iExtRecordArrayToBeWrite[0] );
            iCurrentWritePhase = EPBWritePhase_Write_Ext_Entry;
            // Start Writing to EXT records
            TUint8 index = iExtRecordArrayToBeWrite[0];
            ret = UiccPbReqWriteExt(
                    aTransId,
                    index,
                    aFileId,
                    aFileIdExt,
                    *iPhoneBookEntry );
            }
        else
            {
            // Remove old Entry from the list 
            TInt index = iMmPhoneBookStoreMessHandler->
                         GetIndexForPresentEntry( iPhoneBookEntry->iLocation, arrayIndex );
            if( 0 <= index )
                {
                iMmPhoneBookStoreMessHandler->UpdateEntryFromList( iEntryToUpdateInList, index, arrayIndex );
                }
            else
                {
                // Append the Entry to the List
                iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList( iEntryToUpdateInList, arrayIndex );
                }
            // If it is MBDN Phone Book then update MBI File also
            if( PB_MBDN_FID == aFileId)
                {
                // Start Writing MBI file
                iCurrentWritePhase = EPBWritePhase_Write_MBI_profile;
                ret = UiccPBReqWriteMBIProfile( 
                        aTransId,
                        iPhoneBookEntry->iLocation,
                        KOperationWrite);
                iMBIOperation = KOperationWrite;
                }
            else
                {
                iCurrentWritePhase = EPBWritePhase_complete;
                }
            }
        }
    else
        {
        // If it is a MBDN PhoneBook Then delete MBI profile by writin 0
        if( PB_MBDN_FID == aFileId)
            {
            // Start Writing MBI file
            iCurrentWritePhase = EPBWritePhase_Write_MBI_profile;
            ret = UiccPBReqWriteMBIProfile( 
                    aTransId,
                    iPhoneBookEntry->iLocation,
                    KOperationDelete );
            iMBIOperation = KOperationDelete;
            }
        else
            {
            ret = KErrArgument;
            }
        }
    return ret;
    
    }



// ---------------------------------------------------------------------------
// TBool CMmPhoneBookOperationWrite::HandleWriteExtEntryResp
// Handle write response for EXT part of Entry
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::HandleWriteExtEntryResp(
    TInt aStatus,
    TUint8 aTransId,
    TUint16 aFileId,
    TUint16 aFileIdExt) 
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleWriteExtEntryResp");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONWRITE_HANDLEWRITEEXTENTRYRESP, "CMmPhoneBookOperationWrite::HandleWriteExtEntryResp" );
    TInt ret ( KErrNone );
    TUint8 arrayIndex ( ConvertToConfArrayIndex( aFileId ));
    
    if( UICC_STATUS_OK  == aStatus )
        {
        if( iExtDeleteOperation )
            {
            if( iExtRecordArrayToBeDelete.Count() > 0 )
                {
                // remove the Entry Which has been deleted
                iExtRecordArrayToBeDelete.Remove(
                                          iExtRecordArrayToBeDelete.Count() - 1 );
                iExtRecordArrayToBeDelete.Compress();
                }
            // Remove old Entry from the list 
            TInt index = iMmPhoneBookStoreMessHandler->GetIndexForPresentEntry( iPhoneBookEntry->iLocation, arrayIndex );
            if( 0 <= index )
                {
                iMmPhoneBookStoreMessHandler->RemoveExtEntryFromList( index, arrayIndex);
                }
            
            if( iExtRecordArrayToBeDelete.Count() > 0 )
                {
                iCurrentWritePhase = EPBWritePhase_Write_Ext_Entry;
                iExtDeleteOperation = ETrue;
                TUint8 index = iExtRecordArrayToBeDelete[
                               iExtRecordArrayToBeDelete.Count() -1 ];
                ret = UiccPbReqWriteExtDelete( aFileId, aFileIdExt, index, aTransId );
                }
            else
                {
                // Start Writing the Entry
                iCurrentWritePhase = EPBWritePhase_Write_Entry;
                TUint8 index ( 0 );
                // Start writing main Entry
                if( ( iPhoneBookEntry->iLocation == 0 )||( PB_MBDN_FID == aFileId ) )
                    {
                    index = iEntryToWrite;
                    }
                else
                    {
                    index = iPhoneBookEntry->iLocation;
                    }
                if( 0 < index )
                    {
                    ret = UiccPBReqWriteEntry(
                          aFileId,
                          index,
                          aTransId,
                          *iPhoneBookEntry );
                    }
                else
                    {
                    ret = KErrNotFound;
                    }
                }
            }
        else
            {
            // Append the record number to the Entry
            iEntryToUpdateInList->PBEntryExtRecord.Append( iExtRecordArrayToBeWrite[0] );
            // remove the Entry Which has been Written
            iExtRecordArrayToBeWrite.Remove( 0 );
            iExtRecordArrayToBeWrite.Compress();
            
            // Start Writing to EXT records
            // first do the delete records
            if( iExtRecordArrayToBeWrite.Count() > 0 )
                {
                iEntryToUpdateInList->PBEntryExtRecord.Append( iExtRecordArrayToBeWrite[0] );
                TUint8 index = iExtRecordArrayToBeWrite[0];
                ret = UiccPbReqWriteExt(
                      aTransId,
                      index,
                      aFileId,
                      aFileIdExt,
                      *iPhoneBookEntry );
                }
            else
                {
                // Overwrite old Entry from the list 
                TInt index = iMmPhoneBookStoreMessHandler->
                             GetIndexForPresentEntry( iPhoneBookEntry->iLocation, arrayIndex );
                if( 0 <= index )
                    {
                    iMmPhoneBookStoreMessHandler->UpdateEntryFromList( iEntryToUpdateInList, index, arrayIndex );
                    }
                else
                    {
                    // Append the Entry to the List
                    iMmPhoneBookStoreMessHandler->StoreEntryToPhoneBookList( iEntryToUpdateInList, arrayIndex );
                    }
                // If it is MBDN PhoenBook then update MBI Profile also
                if( PB_MBDN_FID == aFileId)
                    {
                    // Start Writing MBI file
                    iCurrentWritePhase = EPBWritePhase_Write_MBI_profile;
                    ret = UiccPBReqWriteMBIProfile( 
                            aTransId,
                            iPhoneBookEntry->iLocation,
                            KOperationWrite);
                    iMBIOperation = KOperationWrite;
                    }
                else
                    {
                    // Complete the Task
                    iCurrentWritePhase = EPBWritePhase_complete;
                    }
                }
            }
        }
    else
        {
        if( PB_MBDN_FID == aFileId )
            {
            // Start Writing MBI File
            iCurrentWritePhase = EPBWritePhase_Write_MBI_profile;
            ret = UiccPBReqWriteMBIProfile( 
                    aTransId,
                    iPhoneBookEntry->iLocation,
                    KOperationWrite);
            iMBIOperation = KOperationWrite;
            }
        else
            {
            ret = KErrGeneral;
            }
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// TBool CMmPhoneBookOperationWrite::HandleWriteMBIReadResp
// Handle write response for MBI profile read
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::HandleWriteMBIReadResp(
        TInt aStatus,
        TUint8 aDetails,
        TUint8 aTransId,
        const TDesC8 &aFileData ) 
    {
    TInt ret ( KErrNone );
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleWriteMBIReadResp");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONWRITE_HANDLEWRITEMBIREADRESP, "CMmPhoneBookOperationWrite::HandleWriteMBIReadResp" );


    if( UICC_STATUS_OK  == aStatus )
        {
        iEntryToWrite = aFileData[0];
        ret = UiccPbReqWriteL( aTransId, iEntryToWrite, *iPhoneBookEntry );
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
// TBool CMmPhoneBookOperationWrite::HandleWriteMBIWriteResp
// Handle write response for MBI profile write
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationWrite::HandleWriteMBIWriteResp(
        TInt aStatus,
        TUint8 aDetails )
    {
    TInt ret ( KErrNone );
TFLOGSTRING("TSY: CMmPhoneBookOperationWrite::HandleWriteMBIWriteResp");
OstTrace0( TRACE_FATAL, CMMPHONEBOOKOPERATIONWRITE_HANDLEWRITEMBIWRITERESP, "CMmPhoneBookOperationWrite::HandleWriteMBIWriteResp" );

    if( UICC_STATUS_OK  == aStatus )
        {
        // Check for Which operation this response was 
        if ( KOperationDelete == iMBIOperation )
            {
            ret = KErrAccessDenied;
            }
        // Send Request to write entry on MBDN file        
        iCurrentWritePhase = EPBWritePhase_complete;
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

// End of file
