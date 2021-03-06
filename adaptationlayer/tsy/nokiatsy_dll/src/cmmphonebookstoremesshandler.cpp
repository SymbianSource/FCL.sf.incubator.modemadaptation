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
#include <etelmmerr.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tsylogger.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <tisi.h>

#include "cmmphonebookstoremesshandler.h"
#include "cmmphonemesshandler.h"
#include "cmmstaticutility.h"
#include "cmmphonebookalphastring.h"
#include "cmmphonebookoperationinit.h"
#include "cmmphonebookoperationinit3g_adn.h"
#include "cmmphonebookoperationread.h"
#include "cmmphonebookoperationread3g_adn.h"
#include "cmmphonebookoperationcache.h"
#include "cmmphonebookoperationcache3g_adn.h"
#include "cmmphonebookoperationwrite.h"
#include "cmmphonebookoperationwrite3g_adn.h"
#include "cmmphonebookoperationdelete.h"
#include "cmmphonebookoperationdelete3g_adn.h"
#include "cmmphonebookstoreoperationlist.h"
#include "cmmphonebookstoreoperationbase.h"
#include "cmmmessagerouter.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookstoremesshandlerTraces.h"
#endif


// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
const TUint8 KMaxAnrLength( 100 );
const TUint8 KMaxSneLength( 241 );
const TUint8 KOffsetVoicemail( 0 );
const TUint8 KOffsetFax( 1 );
const TUint8 KOffsetData( 2 );
const TUint8 KOffsetOther( 3 );

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
// TPrimitiveInitInfo::TPrimitiveInitInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
TPrimitiveInitInfo::TPrimitiveInitInfo()
    {
TFLOGSTRING("TSY: TPrimitiveInitInfo::TPrimitiveInitInfo");
OstTrace0( TRACE_NORMAL,  TPRIMITIVEINITINFO_TPRIMITIVEINITINFO_TD, "TPrimitiveInitInfo::TPrimitiveInitInfo" );


    iNoOfRecords = 0;          // 2 byte long
    iAlphaStringlength = 0;    // 2 byte long
    iNumlength = 0;          // 2 byte long
    iExtNoOfRec = 0;
    iExtension = EFalse;

    }

// ---------------------------------------------------------------------------
// TPrimitiveInitInfo::GetPBEntryFromUICCData
// Separate phonebook entry from ISI message
// ---------------------------------------------------------------------------
//
void TPrimitiveInitInfo::GetPBEntryFromUICCData(
    const TDesC8& aFileData,
    TDes8& aNumber,
    TDes8& aName)
    {
    TUint8 nameLength( iAlphaStringlength );
    TUint8 numLength( 0 );

    if ( iAlphaStringlength )
        {
        // If actual alpha string is shorter than defined in entry structure,
        // copy it until the first 'FF'
        TInt endOfAlphaString( aFileData.Find( &KTagUnusedbyte, 1 ) );
        if ( endOfAlphaString > 0 && endOfAlphaString < iAlphaStringlength )
            {
            nameLength = endOfAlphaString;
            }
TFLOGSTRING("TSY: TPrimitiveInitInfo::GetPBEntryFromUICCData. Saving name.");
OstTrace0( TRACE_NORMAL,  TPRIMITIVEINITINFO_GETPBENTRYFROMUICCDATA_TD, "TPrimitiveInitInfo::GetPBEntryFromUICCData. Saving Name" );
        aName.Copy( aFileData.Mid( 0, nameLength ) );
        }
    // No else. Alpha string length is 0 and it is not copied

    numLength = aFileData[iAlphaStringlength];

    // Check for number Length also
    if( iNumlength < numLength)
        {
        numLength = iNumlength;
        }
    // no else
    // Save number
    if ( KMinLength < numLength )
        {
TFLOGSTRING("TSY: TPrimitiveInitInfo::GetPBEntryFromUICCData. Saving number.");
OstTrace0( TRACE_NORMAL,  DUP1_TPRIMITIVEINITINFO_GETPBENTRYFROMUICCDATA_TD, "TPrimitiveInitInfo::GetPBEntryFromUICCData. Save Number" );

        // Store number in buffer4
        // Start for number
        TInt offset = iAlphaStringlength + 1;
        aNumber.Append(aFileData.Mid(offset,numLength));
        }
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler.");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_CMMPHONEBOOKSTOREMESSHANDLER_TD, "CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler
// C++ destructor.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler.");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKSTOREMESSHANDLER_CMMPHONEBOOKSTOREMESSHANDLER_TD, "CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler" );

// Delete all the entries
    for( TInt pbCount = 0; pbCount < iPBEntryList.Count(); pbCount++ )
        {
        for( TInt count = 0; count < iPBEntryList[pbCount].iEntryList.Count(); count++)
            {
            delete iPBEntryList[pbCount].iEntryList[count];
            }
        iPBEntryList[pbCount].iEntryList.Close();
        }

    delete iOperationlist;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::NewL
// Two-phased constructor.
// Creates a new CMmPhoneBookStoreMessHandler object instance.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreMessHandler* CMmPhoneBookStoreMessHandler::NewL(
    CMmMessageRouter* aMessageRouter,  // Pointer to message router
    CMmUiccMessHandler* aUiccMessHandler) //Pointer to the Uicc Message handler
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::NewL.");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_NEWL_TD, "CMmPhoneBookStoreMessHandler::NewL" );

    // Create PhoneBookStore messagehandler
    CMmPhoneBookStoreMessHandler* mmPhoneBookStoreMessHandler =
        new( ELeave ) CMmPhoneBookStoreMessHandler();

    CleanupStack::PushL( mmPhoneBookStoreMessHandler );

    mmPhoneBookStoreMessHandler->iMessageRouter = aMessageRouter;
    mmPhoneBookStoreMessHandler->iMmUiccMessHandler = aUiccMessHandler;

    mmPhoneBookStoreMessHandler->ConstructL(
        mmPhoneBookStoreMessHandler,
        aUiccMessHandler
        );

    CleanupStack::Pop( mmPhoneBookStoreMessHandler );

    return mmPhoneBookStoreMessHandler;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::ConstructL
// Initialises object attributes.
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreMessHandler::ConstructL(
    CMmPhoneBookStoreMessHandler* /*mmPhoneBookStoreMessHandler*/,
    CMmUiccMessHandler* aUiccMessHandler)
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_CONSTRUCTL_TD, "CMmPhoneBookStoreMessHandler::ConstructL" );

    iOperationlist = CMmPhoneBookStoreOperationList::NewL( this , aUiccMessHandler );
    iNumberOfFdnInfoResps = 0;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::ExtFuncL
// Directs requests from CommonTsy to right method
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::ExtFuncL(
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - arrived. IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL_TD, "CMmPhoneBookStoreMessHandler::ExtFuncL;aIpc=%d", aIpc );

    TInt ret( KErrNone );
    CMmPhoneBookStoreOperationBase* operation( NULL );


    if ( EMmTsyPhoneBookStoreCacheCancelIPC == aIpc )
        {
        // get the phonebook name
        TName phonebookTypeName;
        const CPhoneBookDataPackage* phoneBookData =
            static_cast<const CPhoneBookDataPackage*>( aDataPackage );
        phoneBookData->GetPhoneBookName( phonebookTypeName );

        // processing cache cancel IPC
        iOperationlist->CancelOperation( phonebookTypeName ); // seek from the beginning
        }  // end of EMmTsyPhoneBookStoreCacheCancelIPC
    else if ( EMobilePhoneGetMailboxNumbers == aIpc )
        {
        GetMailboxIdentifiers();
        }
    else
        { // all other IPC's
        // Check for Empty Index
         TInt transId = iOperationlist->FindEmptyIndexTransId();

        if( 0 <= transId )
            {
            // create operation on the basis of IPC
            operation = CreateNewOperationL( aDataPackage, aIpc );

            // Add operation to the operation list
            iOperationlist->AddOperation( transId, operation );

            // Start operation request
            ret = operation->UICCCreateReq(aIpc,aDataPackage, transId );

            if ( KErrNone != ret)
                {
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL;CreateReq returns %d", ret);
OstTrace1( TRACE_NORMAL,  DUP11_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL_TD, "CMmPhoneBookStoreMessHandler::ExtFuncL;CreateReq returns %d", ret );

                iOperationlist->RemoveOperationFromList( transId );
                }
            }
        else
            {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - Server Busy ");
OstTrace0( TRACE_NORMAL,  DUP3_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL_TD, "CMmPhoneBookStoreMessHandler::ExtFuncL - Server Busy " );

            ret = KErrServerBusy;
            }
        }

    return ret;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::ProcessUiccMsg
// Called when an ISI message has been received.
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 aDetails,
    const TDesC8 &aFileData ) // received data in UICC Server Message
    {
    TInt ret(KErrNone);

TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ProcessUiccMsg. transactId:%d", aTraId);
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG_TD, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg" );

    if ( ETrIdReadMailboxIdentifier != aTraId )
        {
        // Check for operation with transaction id
        CMmPhoneBookStoreOperationBase* operation;
        operation = iOperationlist->Find( aTraId );
        if( operation )
            {
            if( operation->HandleUICCPbRespL( aStatus, aDetails, aFileData, aTraId ))
                {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Remove Operation from the list ");
OstTrace0( TRACE_NORMAL,  DUP2_CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG_TD, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Remove operation from the list " );

                // remove operation From the list
                iOperationlist->RemoveOperationFromList( aTraId );
                } // End of operation remove from thelist
            }
        else
            {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Operation not Found ");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG_TD, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Operation not found " );
            }
        }
    else // Mailbox identifiers is special case
        {
        HandleGetMailboxIdentifiers( aStatus, aFileData );
        }

    return(ret);
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::CreateNewOperation
// Separate request and create correct object
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationBase* CMmPhoneBookStoreMessHandler::CreateNewOperationL(
    const CMmDataPackage* aDataPackage,
    TInt aIpc
    )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::CreateNewOperation");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_CREATENEWOPERATION_TD, "CMmPhoneBookStoreOperationList::CreateNewOperation" );

    CMmPhoneBookStoreOperationBase* pointer( NULL );

    switch( aIpc )
        {
        case EMmTsyPhoneBookStoreInitIPC:
            {

            // Get Card type from uiccmesshandler
            // if SIM, phonebook is 2G and located under DFtelecom and ADN pb contains only name/number entries
            // if USIM, phonebook can be 3G local or 3G private. ADN pb entry can contain additional entries
            // 3G local is located under DFtelecom and 3G private under ADFusim, both have same structure however
            // only the path is different? 7F10 vs. 7FFF
            // Here in UICCCreateReq only 3G local has been handled
            // So for both SIM and USIM Phonebook will be under DFtelecom (whose address is 7F10)


            // Chekc for Card type to Create Class for Phonebook Init
            if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperatorInit3G_ADN Phonebook
                pointer = CMmPhoneBookOperationInit3G_adn::NewL(
                        this,
                        iMmUiccMessHandler,
                        aDataPackage
                        );
                }
            else if(UICC_CARD_TYPE_ICC ==  iMmUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperatorInit
                pointer = CMmPhoneBookOperationInit::NewL(
                    this,
                    iMmUiccMessHandler,
                    aDataPackage
                    );
                }
            break;
            }

        case EMmTsyPhoneBookStoreGetInfoIPC:
        case EMmTsyPhoneBookStoreCacheIPC:
        case EMmTsyONStoreGetInfoIPC:
            {

            if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperationCache3G_adn Phonebook
                pointer = CMmPhoneBookOperationCache3G_adn::NewL(
                        this,
                        iMmUiccMessHandler,
                        aIpc,
                        aDataPackage );
                }
            else if(UICC_CARD_TYPE_ICC ==  iMmUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperatorInit
                pointer = CMmPhoneBookOperationCache::NewL(
                    this,
                    iMmUiccMessHandler,
                    aIpc,
                    aDataPackage );
                }
            break;
            }
        case EMmTsyPhoneBookStoreReadIPC:
        case EMmTsyONStoreReadIPC:
        case EMmTsyONStoreReadEntryIPC:
        case EMmTsyONStoreReadSizeIPC:
        case EMmTsyONStoreWriteSizeIPC:
            {
            //call CmmPhonebookOperatorRead
            if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                {
                pointer = CMmPhoneBookOperationRead::NewL(
                    this,
                    iMmUiccMessHandler,
                    aDataPackage,
                    aIpc );
                }
            else if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                {
                pointer = CMmPhoneBookOperationRead3g_adn::NewL(
                        this,
                        iMmUiccMessHandler,
                        aDataPackage,
                        aIpc );
                }
            break;
            }
        case EMmTsyPhoneBookStoreWriteIPC:
        case EMmTsyONStoreWriteEntryIPC:
        case EMmTsyONStoreWriteIPC:
            {
            if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperationWrite
                pointer = CMmPhoneBookOperationWrite::NewL(
                    this,
                    iMmUiccMessHandler,
                    aDataPackage,
                    aIpc);
                }
            else if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                // needs to be changed after 3g ADN implmentation
                // to be implemented for 3G
                pointer = CMmPhoneBookOperationWrite3g_adn::NewL(
                    this,
                    iMmUiccMessHandler,
                    aDataPackage,
                    aIpc );
                }
            break;
            }
        case EMmTsyPhoneBookStoreDeleteIPC:
        case EMmTsyPhoneBookStoreDeleteAllIPC:
            {
            if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType() )
                {
                // Create CmmPhoneBookOperationDelete
                pointer = CMmPhoneBookOperationDelete::NewL(
                    this,
                    iMmUiccMessHandler,
                    aDataPackage );
                }
            else if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                // To be implemented for 3G
                // needs to be changed after 3G ADN implementation
                pointer = CMmPhoneBookOperationDelete3g_adn::NewL(
                    this,
                    iMmUiccMessHandler,
                    aDataPackage );
                }
            break;
            }
        case EMmTsyONStoreDeleteIPC:
        case EMmTsyONStoreDeleteAllIPC:
            {
            TName phonebookName;
            phonebookName.Copy( KETelIccMsisdnPhoneBook );
            CPhoneBookDataPackage package;
            package.SetPhoneBookName( phonebookName );
            pointer = CMmPhoneBookOperationDelete::NewL(
                this,
                iMmUiccMessHandler,
                &package );
            break;
            }
        default:
            {
            // Nothing to do here
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL,  DUP2_CMMPHONEBOOKSTOREOPERATIONLIST_BUILDL_TD, "CMmPhoneBookStoreOperationList::BuildL;Unknown aIpc=%d", aIpc );
            break;
            }
        }

    //return pointer to right operation
    return pointer;
    }




// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::StorePhonebookEntryL
// Store phonebook entry to Array received from commonTSY
// ---------------------------------------------------------------------------
//

void CMmPhoneBookStoreMessHandler::StorePhonebookEntryL(
        TDes8& aName,
        TDes8& aNumber,
        CPhoneBookStoreEntry& aEntry,
        const TUint16 aFileId,
        const TInt aIndexToRead )
    {
    // Save Name

TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::SeparatePhoneBookEntryFromIsiMsgL. Saving name in commonTSY Array.");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONBASE_STOREPHONEBOOKENTRY_TD, "CMmPhoneBookStoreOperationBase::StorePhonebookEntry. Save name to commonTSY Array" );


    aEntry.iText = HBufC::NewL( aName.Length() );
    TPtr ptrToName = aEntry.iText->Des();

    TBuf8<UICC_EF_MAX_NAME_LEN> aNameString;
    TUint16 nameLength = aName.Length();
    if(KMinLength < nameLength)
        {
        // Convert String to 8 bit format
        CMmStaticUtility::ConvertGsmDataToUcs2(aName, nameLength , aNameString );
        TIsiUtility::CopyFromBigEndian(
                aNameString,
                ptrToName );
        }

    // Store Number

TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::SeparatePhoneBookEntryFromIsiMsgL. Saving number in commonTSY Array.");
OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_STOREPHONEBOOKENTRY_TD, "CMmPhoneBookStoreOperationBase::StorePhonebookEntry. Save number to commonTSY Array" );

    TInt numLength = aNumber.Length();
    if(KMinLength < numLength)
        {
        // Check for last lower nibble if " F " then terminate it
            if( 0x0F == ( aNumber[numLength-1]& 0x0F ))
                {
                // Decrement the memory allocated by 1
                aEntry.iNumber = HBufC::NewL( ( 2*numLength ) - 1 );
                }
            else
                {
                // Allocate memory for double the number Length
                aEntry.iNumber = HBufC::NewL( 2*numLength );
                }
            TPtr ptrToNumber = aEntry.iNumber->Des();

            // Convert Number to Ascii Code
            ConvertToUcs2FromBCD(aNumber, ptrToNumber,aFileId);

        }

    // Set record index
    aEntry.iLocation = aIndexToRead;

    // reset both buffers after storing data to commonTSY buffer
    aName.Zero();
    aNumber.Zero();
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::StoreAnrToPbEntry
//
// ---------------------------------------------------------------------------
//

void CMmPhoneBookStoreMessHandler::StoreAnrToPhonebookEntryL(
    TDes8& aAnr,
    CPhoneBookStoreEntry& aEntry,
    const TUint16 aFileId )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::StoreAnrToPhonebookEntryL.");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_STOREANRTOPHONEBOOKENTRYL_TD, "CMmPhoneBookStoreMessHandler::StoreAnrToPhonebookEntryL" );

    TInt anrLength( aAnr.Length() );
    if ( KMaxAnrLength < anrLength )
        {
        aAnr.SetLength( KMaxAnrLength );
        }

    TBufC8<KMaxAnrLength> anrSourceBuf( aAnr );
    TBuf16<KMaxAnrLength> anrTargetBuf;

    // Convert 8-bit number to 16-bit ascii code
    ConvertToUcs2FromBCD( aAnr, anrTargetBuf, aFileId );
    // Add ANR entry to cache
    aEntry.iAnr->AppendL( anrTargetBuf );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::StoreSneToPbEntry
//
// ---------------------------------------------------------------------------
//

void CMmPhoneBookStoreMessHandler::StoreSneEmailToPbEntryL(
    TDes8& aString,
    CPhoneBookStoreEntry& aEntry,
    TUint8 aFileTypeTag )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::StoreSneToPbEntry.");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_STORESNETOPBENTRY_TD, "CMmPhoneBookStoreMessHandler::StoreSneToPbEntry" );

    if ( aString.Length() )
        {
        TBuf16<KMaxSneLength> targetString; // Final SNE/EMAIL for cache
        TBuf8<KMaxSneLength> outputString; // Temporary for converting

        CMmStaticUtility::ConvertGsmDataToUcs2(
            aString,
            aString.Length(),
            outputString );
        // From 8-bit to 16-bit
        TIsiUtility::CopyFromBigEndian( outputString, targetString );

        if ( UICC_SNE_PRIM_TAG == aFileTypeTag )
            {
            aEntry.iSne->AppendL( targetString );
            }
        else if ( UICC_EMAIL_PRIM_TAG == aFileTypeTag )
            {
            aEntry.iEmail->AppendL( targetString );
            }
        // No else
        }
    // No else. If no data, nothing is appended
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::CopyToUcs2FromBCD
// Copies unsigned BCD coded digits to Ascii code
// by index
// -----------------------------------------------------------------------------
//
void CMmPhoneBookStoreMessHandler::ConvertToUcs2FromBCD
       (
        const TDesC8 &aSource,
        TDes16 &aTarget,
        const TUint16 aFileId
        )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToUcs2FromBCD");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_CONVERTTOUCS2FROMBCD_TD, "CMmPhoneBookStoreMessHandler::ConvertToUcs2FromBCD" );

    TInt maxLength(0);
    TUint8 data;
    TBool plus_in_middle = EFalse;
    TBool first_occurance = ETrue;
    TUint8 nibble = 0;

    // Check for the Valid Length for Target
    if( aTarget.MaxSize() < (aSource.Length()* 2))
        {
        // Assign Target length to Max Length
        maxLength = ( aTarget.MaxSize()/2 );

        // Chekc for if target Max length is Odd number
        if( 1 == ( aTarget.MaxLength()% 2 ))
            {
            maxLength++;
            }
        }
    else
        {
        maxLength = aSource.Length();
        }

    // Check for first Byte which is TON/NPI nit equal to 0xFF, which is unused
    if(aSource[0] != TON_NPI_NO_TEL_NBR)
        {
        // Check for International number
        if( MASK_TON_FROM_TON_NPI_BYTE(aSource[0]) == TON_INTERNATIONAL)
            {
            data = MASK_LOWER_NIBBLE_OF_BCD(aSource[1]);

            // Check for first lower nibble if first byte is '*' and '#'
            if(( 0xA != data) && ( 0xB != data))
                {
                // if not then Append + in Start
                aTarget.Append('+');
                }
            else
                {
                // Stiore the informtion that '+' could be in between
                plus_in_middle = ETrue;
                }
            }
        }


    // Read lower nibble
        nibble = 0;
    // Store rest of the digits
    for( TInt count = 1; count < maxLength ; )
        {
        // Check if it's higher nibble
        if(1 == nibble)
            {
            // Shift higher nibble dayta to lower nibble
            data = aSource[count]>>4;
            count++;
            }
        else
            {
            data = aSource[count];
            }
        data = data & 0x0f;
        // Check for higher nibble to End Mark for odd numbers
        if(0x0F != data)
            {
            // Check for the phonebook type to decide which
            //lookup table should be checked
            if( PB_ADN_FID == aFileId )
                {
                // Chek for lookup table LookUptable ADN
                aTarget.Append(LookupArrayAdn[data]);
                }
            else
                {
                // for all other phonebooks
                aTarget.Append(LookupArray[data]);
                }
            }
        // Change the nibble to read next digit
        if(0 == nibble)
            {
            nibble = 1;
            }
        else
            {
            nibble = 0;
            }
        }

    TInt offset = 0;
    // start from higher nibble
    nibble = 1;
    // Check if its a international number and plus in middle is present
    if( plus_in_middle )
        {
        for(TInt count = 1; count < maxLength; )
            {
            // Check if it's higher nibble
            if(1 == nibble)
                {
                // Shift higher nibble dayta to lower nibble
                data = aSource[count]>>4;
                count++;
                }
            else
                {
                data = aSource[count];
                }
            data = data & 0x0f;
            // Check for higher nibble to End Mark for odd numbers
            if((0xA == data) || (0xB == data))
                {
                first_occurance = EFalse;
                }
            if(!first_occurance)
                {
                // Check for if data is some digit or cahracter
                if((data!=0xA) && ( data!=0xB))
                    {
                    offset = 2 * (count - 1);
                    if(1 == nibble)
                        {
                        offset = offset -1;
                        }
                    break;
                    }
                }

            // Change the nibble to read next digit
            if(0 == nibble)
                {
                nibble = 1;
                }
            else
                {
                nibble = 0;
                }
            }// End of checking offset for insert '+'
        }
    // To insert '+' in between the number
    TBuf16<1> insertBuffer;
    insertBuffer.Append('+');
    if(plus_in_middle)
        {
        // Insert '+'
        aTarget.Insert(offset,insertBuffer);
        }
 }



// -----------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::CopyToUcs2FromBCD
// Copies unsigned BCD coded digits to Ascii code
// by index
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::ConvertToBCDFromUCS2
       (
        TDesC16 &aSource,
        TDes8 &aTarget,
        TUint16 aFileId )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToBCDFromUCS2");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_CONVERTTOBCDFROMUCS2_TD, "CMmPhoneBookStoreMessHandler::ConvertToBCDFromUCS2" );

    TInt ret ( KErrNone );
    TInt count( 0 );
    TUint8 plusCount( 0 );
    TBool internationalNumber ( EFalse );

        // Search if more than 1 '+' is present then its a illegal number
    for( TInt i=0; i<aSource.Length(); i++ )
        {
        if( '+' == aSource[i] )
            {
            plusCount++;
            }
        }


    if( plusCount <= 1 )
        {
        if( UICC_INTERNATIONAL_NUM == aSource[count] )
            {
            aTarget.Append( TON_NPI_INTERNATIONAL );
            internationalNumber = ETrue;
            }
        if( ( '*' == aSource[count] ) || ( '#' == aSource[count] ))
            {
            if( plusCount == 1)
                {
                aTarget.Append( TON_NPI_INTERNATIONAL );
                internationalNumber = ETrue;
                }
            }
        if( ! internationalNumber )
            {
            aTarget.Append( TON_NPI_UNKNOWN );
            }
        }
    else
        {
        // its a Illegal number
        ret = KErrGeneral;
        aTarget.Append( TON_NPI_NO_TEL_NBR );
        }

    count = 0;
    while ( count < aSource.Length() )
    {
    if( 0 <= GetBCDCodeforUCS( aSource[count], aFileId ) )
        {
        TInt lo = GetBCDCodeforUCS( aSource[count++], aFileId ); // low nibble
        TInt hi = ( count < aSource.Length() ) ? aSource[count++] : 0xf; // high nibble
        aTarget.Append( (hi<<4)|lo );
        }
    }

    return ret;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::GetBCDCodeforUCS
// Convert UCS String to BCD coding
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::GetBCDCodeforUCS(
         TUint16 aUCSCharacter,
         TUint16 aFileId )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::GetBCDCodeforUCS");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_GETBCDCODEFORUCS_TD, "CMmPhoneBookStoreMessHandler::GetBCDCodeforUCS" );
    TInt bcdNumber( -1 );

    if ( aUCSCharacter >= '0' && aUCSCharacter <='9')
        bcdNumber = aUCSCharacter - '0';
     else if (aUCSCharacter == 'w' && aFileId == PB_ADN_FID)
         bcdNumber = 0xd;
     else if (aUCSCharacter == '?')
         bcdNumber = 0xd;
     else
         bcdNumber = -1;
    return bcdNumber;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::StoreEntryToPhoenBookList
// Stores PhoneBook Entry in PhoneBook list
// ---------------------------------------------------------------------------
//

void CMmPhoneBookStoreMessHandler::StoreEntryToPhoneBookList( TPBEntry* aStoreEntry, TUint8 aPBIndex )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::StoreEntryToPhoenBookList");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_STOREENTRYTOPHOENBOOKLIST_TD, "CMmPhoneBookStoreMessHandler::StoreEntryToPhoenBookList" );

    iPBEntryList[aPBIndex].iEntryList.Append( aStoreEntry );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::ResetEntryInPhoneBookList
// Resets a phonebook entry in phonebook list
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreMessHandler::ResetEntryInPhoneBookList(
    TUint8 aPbIndex,
    TInt aEntryIndex )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ResetEntryInPhoneBookList");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_RESETENTRYINPHONEBOOKLIST_TD, "CMmPhoneBookStoreMessHandler::ResetEntryInPhoneBookList" );
    TInt numOfEntries( iPBEntryList[aPbIndex].iEntryList.Count() );
    for( TInt i( 0 ); i < numOfEntries; i++)
        {
        if ( aEntryIndex == iPBEntryList[aPbIndex].iEntryList[i]->iEntryIndex )
            {
            // Reset values
            iPBEntryList[aPbIndex].iEntryList[i]->iEntryPresent = EFalse;
            iPBEntryList[aPbIndex].iEntryList[i]->PBEntryExtRecord.Reset();
            // Exit loop
            i = numOfEntries;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::IndexCheckInPBList
// Gets pointer to CMmMessageRouter object.
// ---------------------------------------------------------------------------
//

TBool CMmPhoneBookStoreMessHandler::IndexCheckInPBList(
                    TUint8 aIndex,
                    TUint8 aPBIndex,
                    TPBEntry& aEntry )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::IndexCheckInPBList");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_INDEXCHECKINPBLIST_TD, "CMmPhoneBookStoreMessHandler::IndexCheckInPBList" );

    TBool entryFound( EFalse );

    for( TInt count = 0; count < iPBEntryList[aPBIndex].iEntryList.Count(); count++)
        {
        if( aIndex == iPBEntryList[aPBIndex].iEntryList[count]->iEntryIndex )
            {
            entryFound = ETrue;
            aEntry = *( iPBEntryList[aPBIndex].iEntryList[count] );
            }
        }
    return entryFound;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::GetIndexForPresentEntry
// Gets the index for present entry in the Stored list
// ---------------------------------------------------------------------------
//

TInt CMmPhoneBookStoreMessHandler::GetIndexForPresentEntry(
                    TUint8 aIndex,
                    TUint8 aPBIndex )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::GetIndexForPresentEntry");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_GETINDEXFORPRESENTENTRY_TD, "CMmPhoneBookStoreMessHandler::GetIndexForPresentEntry" );

    TInt count( -1 );
    for( ;( count+1 ) < iPBEntryList[aPBIndex].iEntryList.Count(); count++)
        {
        if( aIndex == iPBEntryList[aPBIndex].iEntryList[count+1]->iEntryIndex )
            {
            return (count+1);
            }
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::EmptyEntryCheckInPBList
// Checks for Empty entry in the stored list
// ---------------------------------------------------------------------------
//

TInt CMmPhoneBookStoreMessHandler::EmptyEntryCheckInPBList( TUint8 aPBIndex )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::EmptyEntryCheckInPBList");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_EMPTYENTRYCHECKINPBLIST_TD, "CMmPhoneBookStoreMessHandler::EmptyEntryCheckInPBList" );

    TInt entryNumber( 0 );

    for( TInt count = 0; count < iPBEntryList[aPBIndex].iEntryList.Count(); count++)
        {
        if( !iPBEntryList[aPBIndex].iEntryList[count]->iEntryPresent )
            {
            entryNumber = iPBEntryList[aPBIndex].iEntryList[count]->iEntryIndex;
            break;
            }
        }
    return entryNumber;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::EmptyEntryCheckInPBList
// Gets pointer to CMmMessageRouter object.
// ---------------------------------------------------------------------------
//

void CMmPhoneBookStoreMessHandler::UpdateEntryFromList(
             TPBEntry* aEntry,
             TUint8 aIndex ,
             TUint8 aPBIndex)
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::RemoveEntryFromList");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_REMOVEENTRYFROMLIST_TD, "CMmPhoneBookStoreMessHandler::RemoveEntryFromList" );

    iPBEntryList[aPBIndex].iEntryList[aIndex] = aEntry;

    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList
// REmove the EXt record form the stored list
// ---------------------------------------------------------------------------
//

void CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList(
               TUint8 aIndex ,
               TUint8 aPBIndex)
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_REMOVEEXTENTRYFROMLIST_TD, "CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList" );

    TInt count = iPBEntryList[aPBIndex].iEntryList[aIndex]->PBEntryExtRecord.Count();
    if( 0 < count )
        {
        iPBEntryList[aPBIndex].iEntryList[aIndex]->PBEntryExtRecord.Remove( count-1 );
        iPBEntryList[aPBIndex].iEntryList[aIndex]->PBEntryExtRecord.Compress();
        }
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::MessageRouter
// Gets pointer to CMmMessageRouter object.
// ---------------------------------------------------------------------------
//
CMmMessageRouter* CMmPhoneBookStoreMessHandler::MessageRouter()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::MessageRouter");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_MESSAGEROUTER_TD, "CMmPhoneBookStoreMessHandler::MessageRouter" );

    return iMessageRouter;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::UiccMessHandler
// Gets pointer to CMmMessageRouter object.
// ---------------------------------------------------------------------------
//
CMmUiccMessHandler* CMmPhoneBookStoreMessHandler::UiccMessHandler()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::UiccMessHandler");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_UICCMESSHANDLER_TD, "CMmPhoneBookStoreMessHandler::UiccMessHandler" );

    return iMmUiccMessHandler;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::PhoNetSender
// Gets pointer to PhoNetSender object.
// ---------------------------------------------------------------------------
//
CMmPhoNetSender* CMmPhoneBookStoreMessHandler::PhoNetSender()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::PhoNetSender");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_PHONETSENDER_TD, "CMmPhoneBookStoreMessHandler::PhoNetSender" );

    return iPhoNetSender;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::FindEntryFromPbList
// Finds PB entry from iPBEntryList
// ---------------------------------------------------------------------------
//
TPBEntry* CMmPhoneBookStoreMessHandler::FindEntryFromPbList(
                 TUint8 aIndex,
                 TUint8 aRecordNo )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::FindEntryFromPbList");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_FINDENTRYFROMPBLIST_TD, "CMmPhoneBookStoreMessHandler::FindEntryFromPbList" );

    TPBEntry* ret( NULL );

    for( int i = 0; i < iPBEntryList[aIndex].iEntryList.Count(); i++ )
        {
        if( aRecordNo == iPBEntryList[aIndex].iEntryList[i]->iEntryIndex )
            {
            ret = iPBEntryList[aIndex].iEntryList[i];
            break;
            }
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::GetEntryForType2FileId
// Finds PB entry from iPBEntryList
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreMessHandler::GetEntriesForType2FileId(
    const TInt aCurrentType2EfIndex,
    const TInt aCurrentRecordNum,
    RArray<TInt>& aArray )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::GetEntryForType2FileId");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_GETENTRYFORTYPE2FILEID_TD, "CMmPhoneBookStoreMessHandler::GetEntryForType2FileId" );
    TInt numOfEntries( iPBEntryList[EPhonebookTypeAdn].iEntryList.Count() );

    // Loop all entries that have been created
    for ( TInt i( 0 ); i < numOfEntries; i++ )
        {
        TPBEntry* entry( iPBEntryList[EPhonebookTypeAdn].iEntryList[i] );
        if ( entry )
            {
            RArray<TIapInfo> iapInfo( entry->iapInfo );
            // There are as many IAP infos in entry as type 2 files in PBR
            // Those have been stored in same order as type 2 files in PBR
            // So when elementary file order number is known, corresponding
            // IAP info is known. If recored number there is same as current
            // record number, this type 2 file belongs to this ADN entry
            if ( ( aCurrentType2EfIndex < iapInfo.Count() ) &&
                ( aCurrentRecordNum == iapInfo[aCurrentType2EfIndex].recordNo) )
                {
                aArray.Append( entry->iEntryIndex );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::GetMailboxIdentifiers
//
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::GetMailboxIdentifiers()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::GetMailboxIdentifiers");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_GETMAILBOXNUMBERS_TD, "CMmPhoneBookStoreMessHandler::GetMailboxIdentifiers" );
    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadMailboxIdentifier;
    params.dataOffset = 0;
    params.dataAmount = 0;
    params.record = 1; // Profile 1 is supported only

    params.fileId = KElemFileMailboxIdentifier;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }


// -----------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::HandleGetMailboxIdentifiers
//
// -----------------------------------------------------------------------------
//
void CMmPhoneBookStoreMessHandler::HandleGetMailboxIdentifiers(
    TInt aStatus,
    const TDesC8 &aFileData )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::HandleGetMailboxIdentifiers");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_HANDLEGETMAILBOXIDENTIFIERS_TD, "CMmPhoneBookStoreMessHandler::HandleGetMailboxIdentifiers" );

    RMobilePhone::TMobilePhoneVoicemailIdsV3* voicemailIds(
        new ( ELeave ) RMobilePhone::TMobilePhoneVoicemailIdsV3() );

    TPtrC8 data;
    TInt error( KErrNone );

    if ( ( UICC_STATUS_OK == aStatus ) &&
        ( KOffsetOther < aFileData.Length() ) )
        {
        voicemailIds->iVoice = aFileData[KOffsetVoicemail];
        voicemailIds->iFax = aFileData[KOffsetFax];
        voicemailIds->iData = aFileData[KOffsetData];
        voicemailIds->iOther = aFileData[KOffsetOther];
        }
    else
        {
        error = KErrGeneral;
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &voicemailIds );
    iMessageRouter->Complete(
        EMobilePhoneGetMailboxNumbers,
        &dataPackage,
        error );

    delete voicemailIds;
    voicemailIds = NULL;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::GetPBRRecordNum
//
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::GetPBRRecordNum(
    TInt aIndexToRead,
    TUint8 &aPBRRecNum )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::GetPBRRecordNum");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_GETPBRRECORDNUM_TD, "CMmPhoneBookStoreMessHandler::GetPBRRecordNum" );

    TInt ret( KErrNone );

    // Calculate The PBR Record Number needs to be read
    // get the Max no of Entries in one PBR record
    TInt maxNoOfEntry( iPBStoreConf[EPhonebookTypeAdn].iNoOfRecords  );

    TInt maxNoOfPbrRec( iPBStoreConf[EPhonebookTypeAdn].iPBRNoOfRecords );

    if( maxNoOfPbrRec > 0 )
        {
        if( 0 != ( aIndexToRead%( maxNoOfEntry/maxNoOfPbrRec ) ) )
            {
            aPBRRecNum = 
            ( aIndexToRead / ( maxNoOfEntry/maxNoOfPbrRec ) ) + 1;
            }
        else
            {
            aPBRRecNum = 
            ( aIndexToRead / ( maxNoOfEntry/maxNoOfPbrRec ) );
            }
        }
    else
        {
        ret = KErrNotFound;
        }
    return ret;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::GetCurrentEfRecNum
//
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::GetCurrentEfRecNum(
    TUint8 aPBRRecNum,
    TUint8 &aCurrentRecNum, 
    TInt aIndexToRead )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::GetCurrentEfRecNum");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREMESSHANDLER_GETCURRENTEFRECNUM_TD, "CMmPhoneBookStoreMessHandler::GetCurrentEfRecNum" );

    TInt ret( KErrNone );
    // get ADN anf PBR Max no of records
    TInt maxNoOfEntry( iPBStoreConf[EPhonebookTypeAdn].iNoOfRecords );

    TInt maxPbrNoOfRec(  iPBStoreConf[EPhonebookTypeAdn].iPBRNoOfRecords );


    if( maxPbrNoOfRec > 0 )
        {
        // Get the Entry index in Elementary File
        aCurrentRecNum = aIndexToRead - 
        ( ( maxNoOfEntry/maxPbrNoOfRec )*
                ( aPBRRecNum - 1 ) );
        }
    else
        {
        ret = KErrNotFound;
        }
    return ret;
    }



// End of File
