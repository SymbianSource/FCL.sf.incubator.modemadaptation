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
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationreadtraces.h"
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

    mmPhoneBookOperationRead->iTransactionId = ETrIdPbRead;

    mmPhoneBookOperationRead->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;

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
    const CMmDataPackage* aDataPackage
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead::UICCCreateReq");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_UICCCREATEREQ, "CMmPhoneBookOperationRead::UICCCreateReq" );

    TInt ret( KErrNotSupported );

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    switch( aIpc )
        {
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

                // get the Service type to be read
                iServiceType = UICC_APPL_FILE_INFO;


                // Handle ADN Phonebook


                    switch(iMmUiccMessHandler->GetCardType())
                        {
                        case UICC_CARD_TYPE_ICC:
                            {
                            ret = USimPbReqRead( 0 );
                            }
                            break;
                        case UICC_CARD_TYPE_UICC:
                            {
                            }
                            break;
                        case UICC_CARD_TYPE_UNKNOWN:
                            break;
                        default:
                            break;
                        }
                }

            break;
            }
        default:
            {
            // Nothing to do here
            TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::UICCCreateReq - \
            Unknown IPC: %d", aIpc);
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
    TInt aRecordNo
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationRead::USimPbReqRead");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_USIMPBREQREAD, "CMmPhoneBookOperationRead::USimPbReqRead" );

    TInt ret( KErrNone );
    // Get phonebook type from transactionId and convert it to USim pbtype
    TUint16 simPhonebookType( ConvertToSimPhoneBookType(
        iTransactionId & KMaskPhonebookType ) ); // Service type

    // get the index to be read from phonebook
    TInt index( iIndexToRead );

    TUiccReadLinearFixed cmdParams;
    //cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> iMmPhoneBookStoreMessHandler;

    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
    //cmdParams.filePath.Copy(aApplFieldId);
    cmdParams.filePath.Append(static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( DF_PHONEBOOK ));

    // get the corect Location to be read from phone book
    if( PB_MBDN_FID == simPhonebookType)
        {
        // Index to be read contains two types of information.
        // The least significant byte contains the profile number
        // and the most significant byte contains the type.
        index = index || 0x0100; // Only subscriber profile number 1 is supported
        }

    // Read phonebook elementary file to get the Entry information
    if( !iExtensionRead )
        {
        cmdParams.trId = ETrIdPbRead;
        cmdParams.fileId = simPhonebookType;

        if( iServiceType == UICC_APPL_READ_LINEAR_FIXED )
            {
            cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
            // Least significant byte gives the actual record no. to be read
            cmdParams.record = aRecordNo;
            }
        else if(iServiceType == UICC_APPL_FILE_INFO)
            {
            cmdParams.serviceType = UICC_APPL_FILE_INFO;
            }
        }
    else
        {
        switch( simPhonebookType )
            {
            case PB_ADN_FID:
                {
                // For 2G ADN Phonebook EXT1 will be the extension number store
                cmdParams.trId = ETrIdPbRead;
                cmdParams.fileId = PB_EXT1_FID;

                if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    }
                else if( UICC_APPL_FILE_INFO == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_FILE_INFO;
                    }
                break;
                }
            case PB_FDN_FID:
                {
                // For FDN Phonebook EXT2 will be the extension number store
                cmdParams.trId = ETrIdPbRead;
                cmdParams.fileId = PB_EXT2_FID;

                if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    }
                else if( UICC_APPL_FILE_INFO == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_FILE_INFO;
                    }
                break;
                }
            case PB_SDN_FID:
                {
                // For SDN Phonebook EXT3 will be the extension number store
                cmdParams.trId = ETrIdPbRead;
                cmdParams.fileId = PB_EXT3_FID;

                if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    }
                else if( UICC_APPL_FILE_INFO == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_FILE_INFO;
                    }
                break;
                }
            case PB_BDN_FID:
                {
                // For BDN Phonebook EXT4 will be the extension number store
                cmdParams.trId = ETrIdPbRead;
                cmdParams.fileId = PB_EXT4_FID;

                if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    }
                else if( UICC_APPL_FILE_INFO == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_FILE_INFO;
                    }
                break;
                }
            case PB_MBDN_FID:
                {
                // For MBDN Phonebook EXT6 will be the extension number store
                cmdParams.trId = ETrIdPbRead;
                cmdParams.fileId = PB_EXT6_FID;

                if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    }
                else if( UICC_APPL_FILE_INFO == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_FILE_INFO;
                    }
                break;
                }
            case PB_MSISDN_FID:
                {
                // For MSISDN Phonebook EXT5 will be the extension number store
                cmdParams.trId = ETrIdPbRead;
                cmdParams.fileId = PB_EXT5_FID;

                if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    }
                else if( UICC_APPL_FILE_INFO == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_FILE_INFO;
                    }
                break;
                }
            case PB_VMBX_FID:
                {
                // For VMBX Phonebook EXT2 will be the extension number store
                cmdParams.trId = ETrIdPbRead;
                cmdParams.fileId = PB_EXT2_FID;

                if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
                    cmdParams.record = aRecordNo;
                    }
                else if( UICC_APPL_FILE_INFO == iServiceType )
                    {
                    cmdParams.serviceType = UICC_APPL_FILE_INFO;
                    }
                break;
                }
            }
        }

    ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
        CreateUiccApplCmdReq( cmdParams );
    TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
    return ret;
    }






// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::CreateReq
// Separate request
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead::HandleUICCPbRespL
    (
    TBool & /*aComplete*/,
    TInt aStatus,
    const TDesC8 &aFileData,
    TInt aTransId
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleUICCPbRespL");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL" );

    TInt ret(KErrNone);
    // Initialize Application file ID to send in next request
    TBuf8<2> applFieldId;
    applFieldId.Append(0x7F);
    applFieldId.Append(0x10);

    // Check for Error if returned from UICC Server

    // Handle possible error case
    if ( UICC_STATUS_OK != aStatus )
        {
        TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleUICCPbRespRead-\
            Unsuccessfully completed by SIMSON");
//OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL, Unsuccessfully completed by UICC Server" );
        ret = CMmStaticUtility::UICCCSCauseToEpocError(aStatus );
        }


    // Get phonebook type from transactionId and convert it to USim pbtype
    TUint16 simPhonebookType( ConvertToSimPhoneBookType(
        iTransactionId & KMaskPhonebookType ) ); // Service type

    if(UICC_STATUS_OK == aStatus)
        {
        switch(aTransId)
            {
            case ETrIdPbReadADN:
                {
                // handle USIM ADN phonebook read
                //HandleUICC3gADNRespL(aStatus, aFileData, aTransId);
                }
                break;
            case ETrIdPbRead:   // Same Read for all phonebook Type extension EF's
                {
                if(!iExtensionRead)
                    {
                    if(iServiceType == UICC_APPL_FILE_INFO)
                        {
                        // Check for the record no. to read is valid or not
                        TInt offSet ( aFileData.Find(&KTagFCIFileDescriptor,1) );
                        if(KErrNotFound != offSet )
                            {
                            // Number of entries is 1 byte long
                            iNumOfPhoneBookRecords = aFileData[offSet + UICC_FCI_EF_FDESC_OFFSET_NUM_ENTR];
                            // Get the record length
                            // -14 is for data other than alpha string
                            //Get16bit(iRecordLength, aFileData, (offSet+4) );

                            if((iNumOfPhoneBookRecords*255) >= (iIndexToRead & 0x00FF))
                                {
                                // read that entry no
                                iServiceType = UICC_APPL_UPDATE_LINEAR_FIXED;
                                if(iNumOfEntriesToRead > 0)
                                  {
                                    // Mask iIndexToRead lower byte to get the entry no need to be read
                                    USimPbReqRead( iIndexToRead && 0x00FF );
                                  }
                                else
                                  {
                                    ret = KErrNotSupported;
                                  }
                                }
                            else
                                {
                                // invalid location to read
                                ret = KErrGeneral;
                                }
                            }
                        }
                    else if(iServiceType == UICC_APPL_UPDATE_LINEAR_FIXED)
                        {
                        // to read next record
                        iIndexToRead++;
                        iNumOfEntriesToRead--;
                        // Seperate Entry data form UICC Server response message
                        // Case: <Data available to be filled into array>
                        if ( UICC_STATUS_OK == aStatus )
                            {
                           TInt retval ( SeparatePhoneBookEntryFromUiccMsgL(
                                   aFileData,
                                   iNameBuf,
                                   simPhonebookType) );

                            if ( 0xFF == aFileData[iRecordLength] )  // Check for Is there any extension data
                                {
                                // the there is no extension data
                                CPhoneBookStoreEntry* phoneBookStoreMsg =
                                    new( ELeave ) CPhoneBookStoreEntry;
                                CleanupStack::PushL( phoneBookStoreMsg );
                                phoneBookStoreMsg->ConstructL();

                                if(KErrNone == retval)
                                    {
                                    StorePhonebookEntry(iNameBuf,iNumberBuf,*phoneBookStoreMsg);
                                    TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
                                    iPhoneBookStoreCacheArray->AppendL( phoneBookStoreMsg );
                                    CleanupStack::Pop( phoneBookStoreMsg );
                                    iNumOfEntriesFilled++;
                                    TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUSimPbRespL - Append entries into array %i",iNumOfEntriesFilled);
                                    }
                                else
                                {
                                    CleanupStack::PopAndDestroy( phoneBookStoreMsg );
                                }

                                // Check for Extension number present or not
                                }
                            else
                                {
                                USimPbReqRead( aFileData[iRecordLength] );
                                iExtensionRead = ETrue;
                                iServiceType = UICC_APPL_READ_LINEAR_FIXED;
                                }
                            }
                        }
                    }
                else  // Handling for Extension Numbers
                    {
                    if(UICC_APPL_UPDATE_LINEAR_FIXED == iServiceType)
                        {
                        if(0xFF != aFileData[13])    // Check for next extension data record
                            {
                            // Store Number upto last byte
                             iNumberBuf.Append(aFileData.Mid(1,11));
                            // Again Send request to read next record number and appenf it in number
                            iServiceType = UICC_APPL_READ_LINEAR_FIXED;
                            USimPbReqRead( aFileData[13] );
                           }
                        else
                            {
                            // Check for length upto which no is stored
                            TInt offset = aFileData.Find(&KTagUnusedbyte,1);
                            // store Data
                            iNumberBuf.Append(aFileData.Mid(1,offset));

                            // the there is no extension data
                            CPhoneBookStoreEntry* phoneBookStoreMsg =
                                new( ELeave ) CPhoneBookStoreEntry;
                            CleanupStack::PushL( phoneBookStoreMsg );
                            phoneBookStoreMsg->ConstructL();

                            StorePhonebookEntry(iNameBuf,iNumberBuf,*phoneBookStoreMsg);

                            TF_ASSERT( NULL != iPhoneBookStoreCacheArray );
                            iPhoneBookStoreCacheArray->AppendL( phoneBookStoreMsg );
                            CleanupStack::Pop( phoneBookStoreMsg );
                            iNumOfEntriesFilled++;
                            TFLOGSTRING2("TSY: CMmPhoneBookOperationRead::HandleUSimPbRespL - \
                            Append entries into array %i",iNumOfEntriesFilled);
                //OstTrace1( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONREAD_HANDLEUSIMPBRESPL, "CMmPhoneBookOperationRead::HandleUSimPbRespL;Append entries into array=%d", iNumOfEntriesFilled );


                            }
                        }
                    }
                }
                break;
            default:
                // There is no such phonebook exist
                ret = KErrGeneral;
                break;
            }
        }

        // Case: <complete request>
        if ( KErrNone != ret || 0 == iNumOfEntriesToRead )
            {
            // If no entries found
            if ( 0 == iNumOfEntriesFilled )
                {
                ret = KErrNotFound;
                }

            TName phoneBookName;
            CPhoneBookDataPackage phoneBookData;

            // Convert number of phonebook type to Sim pbtype
            ConvertToPBname( aTransId, phoneBookName );
            phoneBookData.SetPhoneBookName( phoneBookName );
            iNumOfEntriesFilled = 0;
            iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
                EMmTsyPhoneBookStoreReadIPC,
                &phoneBookData,
                ret );

            iPhoneBookStoreCacheArray = NULL;
            // Set flag to indicate that we can remove this operation from array
            }
    return ret;
    }


// End of File
