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
#include "cmmstaticutility.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookstoreoperationbaseTraces.h"
#endif
// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
const TUint8 KPbrTlvLengthWithSfi( 5 );
const TUint8 KPbrTlvLengthWithoutSfi( 4 );


// MACROS
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
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::CMmPhoneBookStoreOperationBase");
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
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::~CMmPhoneBookStoreOperationBase");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_CMMPHONEBOOKSTOREOPERATIONBASE, "CMmPhoneBookStoreOperationBase::~CMmPhoneBookStoreOperationBase" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::CancelReq
// Cancels the operation. Canceling of canceled operation is allowed.
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationBase::CancelReq( TName& /*aPhoneBook*/ )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::CancelReq - does nothing");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CANCELREQ, "CMmPhoneBookStoreOperationBase::CancelReq - does nothing" );
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
// CMmPhoneBookStoreOperationBase::GetPhoneBookName
// Cancels the operation. Canceling of canceled operation is allowed.
// ---------------------------------------------------------------------------
//
const TName& CMmPhoneBookStoreOperationBase::GetPhoneBookName()const
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::GetPhoneBookName - PhoenbookName");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_GETPHONEBOOKNAME, "CMmPhoneBookStoreOperationBase::GetPhoneBookName - PhoneBookName" );

    return iPhoneBookTypeName;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType
// Converts internal phonebook number to server phonebook number
// ---------------------------------------------------------------------------
//
TUint16 CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType(
    const TUint8 aPbMask )
    {
TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType SourcePBType: 0x%x", aPbMask);
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
TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::ConvertToSimPhoneBookType phoneBookType: 0x%x" ,phoneBookType );
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

TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBtype : phonebooktype: %d", pbMask);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBTYPE, "CMmPhoneBookStoreOperationBase::ConvertToPBtype;pbMask=%hhx", pbMask );

    return pbMask;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::ConvertToPBfileId
// Converts client phonebook type to internal phonebook mask
// ---------------------------------------------------------------------------
//
TUint16 CMmPhoneBookStoreOperationBase::ConvertToPBfileId(
    const TName& aPBType, TUint16 &aFileIdExt, TUint8 aCardType )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBfileId");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBFILEID, "CMmPhoneBookStoreOperationBase::ConvertToPBfileId" );

    TUint16 fileId( 0 );

    if ( 0 == aPBType.CompareF( KETelIccAdnPhoneBook ) )
        {
        fileId = PB_ADN_FID;
        aFileIdExt = PB_EXT1_FID;
        }
    else if ( 0 == aPBType.CompareF( KETelIccBdnPhoneBook ) )
        {
        fileId = PB_BDN_FID;
        aFileIdExt = 0x0000;
        }
    else if ( 0 == aPBType.CompareF( KETelIccSdnPhoneBook ) )
        {
        fileId = PB_SDN_FID;
        aFileIdExt = PB_EXT3_FID;
        }
    else if ( 0 == aPBType.CompareF( KETelIccFdnPhoneBook ) )
        {
        fileId = PB_FDN_FID;
        aFileIdExt = PB_EXT2_FID;
        }
    else if ( 0 == aPBType.CompareF( KETelIccVoiceMailBox ) )
        {
        fileId = PB_VMBX_FID;
        aFileIdExt = PB_EXT1_FID;
        }
    else if ( 0 == aPBType.CompareF( KETelIccMbdnPhoneBook ) )
        {
        fileId = PB_MBDN_FID;
        aFileIdExt = PB_EXT6_FID;
        }
    else if ( 0 == aPBType.CompareF( KETelIccMsisdnPhoneBook ) )
        {
        if( UICC_CARD_TYPE_UICC == aCardType )
            {
            aFileIdExt = PB_EXT5_FID;
            }
        else
            {
            aFileIdExt = PB_EXT1_FID;
            }
        fileId = PB_MSISDN_FID;
        }
    else
        {
        // none
        fileId = UICC_ILLEGAL_FILE_ID;
        }
TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBfileId phonebookfileid: %d", fileId);
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBFILEID, "CMmPhoneBookStoreOperationBase::ConvertToPBfileId;fileId=%hu", fileId );


    return fileId;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::ConvertToConfArrayIndex
// Converts File id to Array index, in which Initialization configuration data stored
// ---------------------------------------------------------------------------
//
TUint8 CMmPhoneBookStoreOperationBase::ConvertToConfArrayIndex(
    const TUint16 aFileId )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToConfArrayIndex");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOCONFARRAYINDEX, "CMmPhoneBookStoreOperationBase::ConvertToConfArrayIndex" );
    TUint8 index( 0 );
    switch(aFileId)
        {
        case PB_ADN_FID:
            index = EPhonebookTypeAdn;
            break;
        case PB_FDN_FID:
            index = EPhonebookTypeFdn;
            break;
        case PB_SDN_FID:
            index = EPhonebookTypeSdn;
            break;
        case PB_MBDN_FID:
            index = EPhonebookTypeMBDN;
            break;
        case PB_MSISDN_FID:
            index = EPhonebookTypeMSISDN;
            break;
        case PB_VMBX_FID:
            index = EPhonebookTypeVMBX;
            break;
        }
    return index;
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
TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname : source: %d" ,aTrans );
OstTraceExt1( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname;source=%hhu", aTrans );

    // get number of phonebook type from transaction Id
    TUint8 numOfPbType( aTrans & KMaskPhonebookType );

    if ( KMaskAdnType == numOfPbType )
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname - ADN");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - ADN" );
        aName = KETelIccAdnPhoneBook;
        }
    else if ( KMaskFdnType == numOfPbType )
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname - FDN");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - FDN" );
        aName = KETelIccFdnPhoneBook;
        }
    else if ( KMaskSdnType == numOfPbType )
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname - SDN");
OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - SDN" );
        aName = KETelIccSdnPhoneBook;
        }
    else if ( KMaskBdnType == numOfPbType )
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname - BDN");
OstTrace0( TRACE_NORMAL, DUP4_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - BDN" );
        aName = KETelIccBdnPhoneBook;
        }
    else if ( KMaskVoiceMailBox == numOfPbType )
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname - VMBX");
OstTrace0( TRACE_NORMAL, DUP5_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - VMBX" );
        aName = KETelIccVoiceMailBox;
        }
    else if ( KMaskMbdnType == numOfPbType )
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname - MBDN");
OstTrace0( TRACE_NORMAL, DUP6_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - MBDN" );
        aName = KETelIccMbdnPhoneBook;
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToPBname - unknown -> Error situation!" );
OstTrace0( TRACE_NORMAL, DUP7_CMMPHONEBOOKSTOREOPERATIONBASE_CONVERTTOPBNAME, "CMmPhoneBookStoreOperationBase::ConvertToPBname - unknown -> Error situation!" );
        //Nothing can do.
        }
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead::EmptyEntryCheck
// Check for Entry is Empty or not
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationBase::EmptyEntryCheck( const TDesC8 &aFileData)
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::EmptyEntryCheck");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD_EMPTYENTRYCHECK, "CMmPhoneBookStoreOperationBase::EmptyEntryCheck" );


    TInt ret( KErrNone);

    if(( 0xFF == aFileData[0])
       || (0xFF == aFileData[1]))
        {
        ret = KErrNotFound;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookStoreOperationBase::FetchFileListFromPBR
// Search wanted file list from EFpbr
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationBase::FetchFileListFromPBR(
    const TDesC8 &aFileData,
    const TUint8 aTag,
    RArray <TPrimitiveTag>& aFileList )
    {
TFLOGSTRING( "TSY: CMmPhoneBookStoreOperationBase::FetchFileListFromPBR" );
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_FETCHFILELISTFROMPBR, "CMmPhoneBookStoreOperationBase::FetchFileListFromPBR" );

    TInt ret( KErrNone );
    TPrimitiveTag primTag;

    TInt offset( aFileData.Find( &aTag, 1 ) );
    TInt nextOffset( 0 );

    if( offset != KErrNotFound )
        {
        // Get the File Tag Length
        offset++;
        TUint8 tagLength = aFileData[offset++];

        for( TInt i = 0; i < tagLength; i += nextOffset )
            {
            // Get the Tag name
            primTag.tagValue = aFileData[offset++];
            TUint8 len( aFileData[offset++] );

            // Get the File ID which is 2byte long
            primTag.tagFID = CMmStaticUtility::Get16Bit( aFileData, offset );
            offset += 2;

            // If file Tag length is 3 the SFI is available and if file tag
            // length is 2 the only File ID is present
            if( KLengthWithSFI == len )
                {
                primTag.tagSFI = aFileData[offset++];
                // get the offset for next file tag
                nextOffset = KPbrTlvLengthWithSfi;
                }
            else if( KLengthWithOutSFI == len )
                {
                primTag.tagSFI = 0;
                // get the offset for next file tag
                nextOffset = KPbrTlvLengthWithoutSfi;
                }
            else
                {
                ret = KErrGeneral;
                break;
                }
            aFileList.Append( primTag );
            }
        }
    else
        {
        ret = KErrNotFound;
        }

    return ret;
    }

// End of File
