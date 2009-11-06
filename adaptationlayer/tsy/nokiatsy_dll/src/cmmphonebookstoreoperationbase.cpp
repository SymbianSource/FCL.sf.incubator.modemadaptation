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
* Description:   ?Description
*
*/

//  INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include "tsylogger.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmessagerouter.h"
#include "cmmphonebookstoreoperationbase.h"
#include "cmmphonemesshandler.h"
#include "cmmuiccmesshandler.h"

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
#include "cmmphonebookalphastring.h"
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookstoreoperationbasetraces.h"
#endif
// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
//const TUint8 KMinLength = 1;


// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// ==================== LOCAL FUNCTIONS ======================================
    // None


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::CMmPhoneBookStoreOperationBase
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationBase::CMmPhoneBookStoreOperationBase()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::\
        CMmPhoneBookStoreOperationBase");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CMMPHONEBOOKSTOREOPERATIONBASE, "CMmPhoneBookStoreOperationBase::CMmPhoneBookStoreOperationBase" );

    iRet = KErrNone;
    iWriteLocation = 0;
    iNumberOfFdnInfoResps = 0;
    iWriteLocation = 0;
    iRet = 0;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::~CMmPhoneBookStoreOperationBase
// C++ destructor.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationBase::~CMmPhoneBookStoreOperationBase()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::\
        ~CMmPhoneBookStoreOperationBase");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_CMMPHONEBOOKSTOREOPERATIONBASE, "CMmPhoneBookStoreOperationBase::~CMmPhoneBookStoreOperationBase" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::CancelReq
// Cancels the operation. Canceling of canceled operation is allowed.
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationBase::CancelReq()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::CancelReq - does nothing");
    OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CANCELREQ, "CMmPhoneBookStoreOperationBase::CancelReq - does nothing" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::PrepareReq
// Prepares the operation (makes it ready to be launched).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationBase::PrepareReq(
        TInt /*aIpc*/,
        const CMmDataPackage* /*aDataPackage*/
        )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::PrepareReq - not supported");
    OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKSTOREOPERATIONBASE_PREPAREREQ, "CMmPhoneBookStoreOperationBase::PrepareReq - not supported" );
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::IsPrepared
// Checks if operation is ready to be launched.
// ---------------------------------------------------------------------------
//
TBool CMmPhoneBookStoreOperationBase::IsPrepared() const
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::IsPrepared");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_ISPREPARED, "CMmPhoneBookStoreOperationBase::IsPrepared" );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::LaunchReq
// Launches activated operation.
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationBase::LaunchReq()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::LaunchReq - not supported");
    OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKSTOREOPERATIONBASE_LAUNCHREQ, "CMmPhoneBookStoreOperationBase::LaunchReq - not supported" );
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::CompleteReq
// Completes the request.
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationBase::CompleteReq( TInt /*aErrorCode*/ )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::CompleteReq - not supported");
    OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_COMPLETEREQ, "CMmPhoneBookStoreOperationBase::CompleteReq - not supported" );
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// TUint8 CMmPhoneBookStoreOperationBase::TransId
// Gets transactionId
// ---------------------------------------------------------------------------
//
TUint8 CMmPhoneBookStoreOperationBase::TransId()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::TransId");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_TRANSID, "CMmPhoneBookStoreOperationBase::TransId" );

    return iTransactionId;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType
// Converts internal phonebook number to server phonebook number
// ---------------------------------------------------------------------------
//
TUint16 CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType(
    const TUint8 aPbMask )
    {
    TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::\
        ConvertToSimPhoneBookType SourcePBType: 0x%x", aPbMask);
OstTraceExt1( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOSIMPHONEBOOKTYPE, "CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType;aPbMask=%hhx", aPbMask );

    TUint16 phoneBookType( KMaskUnspecifiedType );

    switch( aPbMask )
        {
        case KMaskMbdnType:
            {
            phoneBookType = PB_MBDN_FID;
            break;
            }
        case KMaskAdnType:
            {
            phoneBookType = PB_ADN_FID;
            break;
            }
        case KMaskFdnType:
            {
            phoneBookType = PB_FDN_FID;
            break;
            }
        case KMaskSdnType:
            {
            phoneBookType = PB_SDN_FID;
            break;
            }
        case KMaskVoiceMailBox:
            {// When Mask is SDN we set service type to SIM_PB_VMBX
            phoneBookType = PB_VMBX_FID;
            break;
            }
       case KMaskBdnType:
            {
            phoneBookType = PB_BDN_FID;
            break;
            }
        default:
            {
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOSIMPHONEBOOKTYPE, "CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType;ASSERT phoneBookType (%d) != KMaskUnspecifiedType", phoneBookType );
            TF_ASSERT( phoneBookType != KMaskUnspecifiedType );
            break;
            }
        }

    TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::\
        ConvertToSimPhoneBookType phoneBookType: 0x%x" ,phoneBookType );
OstTrace1( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOSIMPHONEBOOKTYPE, "CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType;phoneBookType=%x", phoneBookType );

    return phoneBookType;
    }



void CMmPhoneBookStoreOperationBase::ConvertPBTypeToPbName(const TUint aPhoneBook , TName& apbName)
    {
    switch(aPhoneBook)
        {
        case EPhonebookTypeAdn:
            apbName.Copy(KETelIccAdnPhoneBook);
            break;
        case EPhonebookTypeFdn:
            apbName.Copy(KETelIccFdnPhoneBook);
            break;
        case EPhonebookTypeBdn:
            apbName.Copy(KETelIccBdnPhoneBook);
            break;
        case EPhonebookTypeSdn:
            apbName.Copy(KETelIccSdnPhoneBook);
            break;
        case EPhonebookTypeMBDN:
            apbName.Copy(KETelIccMbdnPhoneBook);
            break;
        case EPhonebookTypeVMBX:
            apbName.Copy(KETelIccVoiceMailBox);
            break;
            
        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::ConvertToPBtype
// Converts client phonebook type to internal phonebook mask
// ---------------------------------------------------------------------------
//
TUint8 CMmPhoneBookStoreOperationBase::ConvertToPBtype(
    const TName& aPBType )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBtype");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBTYPE, "CMmPhoneBookStoreOperationBase::ConvertToPBtype" );

    TUint8 pbMask( 0 );

    if ( 0 == aPBType.CompareF( KETelIccAdnPhoneBook ) )
        {
        pbMask = KMaskAdnType;
        }
    else if ( 0 == aPBType.CompareF( KETelIccBdnPhoneBook ) )
        {
        pbMask = KMaskBdnType;
        }
    else if ( 0 == aPBType.CompareF( KETelIccSdnPhoneBook ) )
        {
        pbMask = KMaskSdnType;
        }
    else if ( 0 == aPBType.CompareF( KETelIccFdnPhoneBook ) )
        {
        pbMask = KMaskFdnType;
        }
    else if ( 0 == aPBType.CompareF( KETelIccVoiceMailBox ) )
        {
        pbMask = KMaskVoiceMailBox;
        }
    else if ( 0 == aPBType.CompareF( KETelIccMbdnPhoneBook ) )
        {
        pbMask = KMaskMbdnType;
        }
    else
        {
        pbMask = KMaskUnspecifiedType;
        }

    TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBtype \
        phonebooktype: %d", pbMask);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBTYPE, "CMmPhoneBookStoreOperationBase::ConvertToPBtype;pbMask=%hhx", pbMask );

    return pbMask;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::ConvertToPBname
// Converts internal phonebook mask to client phonebook type
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationBase::ConvertToPBname(
    const TUint8 aTrans,
    TName& aName )
    {
    TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
        source: %d" ,aTrans );
OstTraceExt1( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname;source=%hhu", aTrans );

    // get number of phonebook type from transaction Id
    TUint8 numOfPbType( aTrans & KMaskPhonebookType );

    if ( KMaskAdnType == numOfPbType )
        {
        TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
            - ADN");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - ADN" );
        aName = KETelIccAdnPhoneBook;
        }
    else if ( KMaskFdnType == numOfPbType )
        {
        TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
            - FDN");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - FDN" );
        aName = KETelIccFdnPhoneBook;
        }
    else if ( KMaskSdnType == numOfPbType )
        {
        TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
            - SDN");
OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - SDN" );
        aName = KETelIccSdnPhoneBook;
        }
    else if ( KMaskBdnType == numOfPbType )
        {
        TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
            - BDN");
OstTrace0( TRACE_NORMAL, DUP4_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - BDN" );
        aName = KETelIccBdnPhoneBook;
        }
    else if ( KMaskVoiceMailBox == numOfPbType )
        {
        TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
            - VMBX");
OstTrace0( TRACE_NORMAL, DUP5_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - VMBX" );
        aName = KETelIccVoiceMailBox;
        }
    else if ( KMaskMbdnType == numOfPbType )
        {
        TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
            - MBDN");
OstTrace0( TRACE_NORMAL, DUP6_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - MBDN" );
        aName = KETelIccMbdnPhoneBook;
        }
    else
        {
        TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname \
            - unknown -> Error situation!" );
OstTrace0( TRACE_NORMAL, DUP7_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - unknown -> Error situation!" );
        //Nothing can do.
        }
    }




// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::SeparatePhoneBookEntryFromIsiMsgL
// Separate phonebook entry from ISI message
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationBase::SeparatePhoneBookEntryFromUiccMsgL(
    const TDesC8& aFileData,      // ISI message
    TDes8& aNameBuf,
    TUint16 aSimPhonebookType)        // Entry to be filled
    {
    TInt ret( KErrNone );
    TBool nameNumberFound (ETrue);
    TUint8 nameLength (0);
    TUint8 numLength(0);
    
    // Maximum record length will be RecordLength -14 
    // Actual string length can be find the first FF , which indicates that after
    // that its empty
    nameLength = aFileData.Find(&KTagUnusedbyte,1);
    // minus 1 is for TON/NPI digit , which is 1 byte long
    numLength = aFileData[iRecordLength -14 + 1] - 1;
    
    if(nameLength == 1 && numLength == 1)
        nameNumberFound = EFalse;
    
    switch(aSimPhonebookType)
        {
        case PB_FDN_FID:
        case PB_BDN_FID:
        case PB_MBDN_FID:
        case PB_MSISDN_FID:
        case PB_VMBX_FID:
            {
            // Save name
            if ( KMinLength < nameLength )
                {
                TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::\
                    SeparatePhoneBookEntryFromIsiMsgL. Saving name.");
        OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_SEPARATEPHONEBOOKENTRYFROMISIMSGL, "CMmPhoneBookStoreOperationBase::SeparatePhoneBookEntryFromIsiMsgL, Saving name" );

                aNameBuf.Copy(aFileData.Mid(0,nameLength));
                }
            //no else

            // Save number
            if ( KMinLength < numLength )
                {
                // Store number in buffer
                iNumberBuf.Append(aFileData.Mid(10,numLength));
                }
            //no else

            }
            break;
        default:
            // None
            break;
        }
    
    if(!nameNumberFound)
        ret = KErrNotFound;

    return ret;
            
        }


void CMmPhoneBookStoreOperationBase::StorePhonebookEntry(
        TDes8& /*aName*/,
        TDes8& /*aNumber*/,
        CPhoneBookStoreEntry& /*aEntry*/)
    {
    
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::ConvertOperationToClientIPCType
// Converts internal request mask to client ipc request.
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationBase::ConvertOperationToClientIPCType(
    TInt& aDestination,
    const TUint8 aSource )
    {
    TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::\
        ConvertOperationToClientIPCType - source: %d", aSource );
OstTraceExt1( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTOPERATIONTOCLIENTIPCTYPE, "CMmPhoneBookStoreOperationBase::ConvertOperationToClientIPCType;aSource=%hhu", aSource );

    switch( aSource )
        {
        case KOperationInit:
            {
            aDestination = EMmTsyPhoneBookStoreInitIPC;
            break;
            }
        case KOperationCache:
            {
            aDestination = EMmTsyPhoneBookStoreCacheIPC;
            break;
            }
        case KOperationRead:
            {
            aDestination = EMmTsyPhoneBookStoreReadIPC;
            break;
            }
        case KOperationWrite:
        case KOperationReWrite:
            {
            aDestination = EMmTsyPhoneBookStoreWriteIPC;
            break;
            }
        case KOperationDelete:
        case KOperationDeleteWrite:
            {
            aDestination = EMmTsyPhoneBookStoreDeleteIPC;
            break;
            }
        case KOperationDeleteAll:
            {
            aDestination = EMmTsyPhoneBookStoreDeleteAllIPC;
            break;
            }
        case KOperationGetMailBoxNumber:
            {
            aDestination = EMobilePhoneGetMailboxNumbers;
            break;
            }
        default: //KOperationUnknown
            {
            TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::\
                ConvertOperationToClientIPCType - Default case" );
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTOPERATIONTOCLIENTIPCTYPE, "CMmPhoneBookStoreOperationBase::ConvertOperationToClientIPCType, Default case" );
            aDestination = KErrNotFound;
            break;
            }
        }
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::HandleFcpData
// Handles FCP data for both SIm and USIM card
// to get the different parameters
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationBase::HandleFcpData( const TDesC8 &aFileData, TInt& aValue , TUint8 aParam )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::HandleFcpData");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_HANDLEFCPDATA, "CMmPhoneBookStoreOperationBase::HandleFcpData" );
    
    
    TInt ret(KErrNone);
    if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
        {
        switch(aParam)
            {
            case KRecordLength:
                {
                // get the offset for File Descriptor for all EF's
                TUint8 offset = aFileData.Find(&KTagFCIFileDescriptor,1);
                Get16bit(aValue, aFileData, (offset+4) );
                }
                break;
            case KNoOfRecords:
                {
                // get the no of records
                TUint8 offset = aFileData.Find(&KTagFCIFileDescriptor,1);
                aValue = aFileData[offset + 6];
                }
                break;
            case KFileSize:
                {
                // get the file size tith tag value "85"
                TUint8 offset = aFileData.Find(&KTagFCIFileSize,1);
                aValue = aFileData[offset + 2];
                }
                break;
            case KFileIdentifier:
                {
                TUint8 offset = aFileData.Find(&KTagFCIFileIdentifier,1);
                Get16bit(aValue, aFileData, (offset+2) );
                }
                break;
            default:
                {
                TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::HandleFcpData - No such parameter exist in UICC");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_HANDLEFCPDATA, "CMmPhoneBookStoreOperationBase::HandleFcpData - No such parameter exist in UICC" );
                }
                break;
            }
        }
    else if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
        {
        switch(aParam)
            {
            case KRecordLength:
                {
                // 14 is the index for record length Data
                aValue = aFileData[14];
                }
                break;
            case KNoOfRecords:
                {
                // 2 is the index value for File Size Data
                TInt fileSize(0);
                Get16bit(fileSize, aFileData, 2 );
                
                // 14 is the index for record Length Data
                TInt recordLength(0);
                recordLength = aFileData[14];
                
                // get the No of records
                
                aValue = (fileSize/recordLength);
                }
                break;
            case KFileSize:
                {
                // 2 is the index location for File Size which is 2 bytes long
                Get16bit(aValue, aFileData, 2 );
                }
                break;
            case KFileIdentifier:
                {
                // 4 is the index location for File ID which is 2 bytes long
                Get16bit(aValue, aFileData, 4 );
                }
                break;
            case KFileStatus:
                {
                // 11 is index value for File Status
                aValue = aFileData[11];
                }
            default:
                TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::HandleFcpData - No such parameter exist in ICC");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_HANDLEFCPDATA, "CMmPhoneBookStoreOperationBase::HandleFcpData - No such parameter exist in ICC Card" );
                break;
            }
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::Get16bit
// Copies unsigned 16-bit word from source descriptor from location pointed
// by index
// -----------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationBase::Get16bit
        (
        TInt& aTarget,   //Target
        const TDesC8& aSource,    //Source
        TInt aIndex         //Index
        )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::Get16bit");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_GET16BIT, "CMmPhoneBookStoreOperationBase::Get16bit" );
    
    
    if ( aIndex + 1 < aSource.Length() )
        {
        aTarget = TUint16( aSource[aIndex] ) << 8;
        aTarget = TUint16( aTarget | aSource[aIndex + 1] );
        }
    }



// End of File
