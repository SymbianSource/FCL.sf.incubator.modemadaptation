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
#include "cmmphonebookoperationwrite.h"
#include "cmmphonebookoperationdelete.h"
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
    // None

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
OstTrace0( TRACE_NORMAL, TPRIMITIVEINITINFO_TPRIMITIVEINITINFO, "TPrimitiveInitInfo::TPrimitiveInitInfo" );


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
    TUint8 nameLength (0);
    TUint8 numLength(0);

    // Maximum record length will be RecordLength -14
    // Actual string length can be find the first FF , which indicates that after
    // that its empty
    nameLength = aFileData.Find(&KTagUnusedbyte,1);
    // Check if MAX length is smaller than the MAX alpha string length
    if( iAlphaStringlength < nameLength)
        {
        // Max lenght wil be the valid alpha string Length
        nameLength = iAlphaStringlength;
        }

    numLength = aFileData[iAlphaStringlength];

    // Chekc for number Length also
    if( iNumlength < numLength)
        {
        numLength = iNumlength;
        }

    // Save name
    if ( KMinLength < nameLength )
        {
TFLOGSTRING("TSY: TPrimitiveInitInfo::GetPBEntryFromUICCData. Saving name.");
OstTrace0( TRACE_NORMAL, TPRIMITIVEINITINFO_GETPBENTRYFROMUICCDATA, "TPrimitiveInitInfo::GetPBEntryFromUICCData. Saving Name" );

        aName.Copy(aFileData.Mid(0,nameLength));
        }
    //no else
    // Save number
    if ( KMinLength < numLength )
        {
TFLOGSTRING("TSY: TPrimitiveInitInfo::GetPBEntryFromUICCData. Saving number.");
OstTrace0( TRACE_NORMAL, DUP1_TPRIMITIVEINITINFO_GETPBENTRYFROMUICCDATA, "TPrimitiveInitInfo::GetPBEntryFromUICCData. Save Number" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_CMMPHONEBOOKSTOREMESSHANDLER, "CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler
// C++ destructor.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler.");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREMESSHANDLER_CMMPHONEBOOKSTOREMESSHANDLER, "CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler" );

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

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    if ( iAas )
      {
      iAas->Reset();
      delete iAas;
      }
    if ( iGas )
      {
      iGas->Reset();
      delete iGas;
      }
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_NEWL, "CMmPhoneBookStoreMessHandler::NewL" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_CONSTRUCTL, "CMmPhoneBookStoreMessHandler::ConstructL" );

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
OstTrace1( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL;aIpc=%d", aIpc );

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
OstTrace1( TRACE_NORMAL, DUP11_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL;CreateReq returns %d", ret );

                iOperationlist->RemoveOperationFromList( transId );
                }
            }
        else
            {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - Server Busy ");
OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - Server Busy " );

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
    const TDesC8 &aFileData  ) // received data in UICC Server Message
    {
    TInt ret(KErrNone);


TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ProcessUiccMsg. transactId:%d", aTraId);
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg" );


    // Check for operation with transaction id
    CMmPhoneBookStoreOperationBase* operation;
    operation = iOperationlist->Find( aTraId );
    if( operation )
        {
        if( operation->HandleUICCPbRespL( aStatus, aDetails, aFileData, aTraId ))
            {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Remove Operation from the list ");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Remove operation from the list " );

            // remove operation From the list
            iOperationlist->RemoveOperationFromList( aTraId );
            } // End of operation remove from thelist
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Operation not Found ");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg. Operation not found " );
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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_CREATENEWOPERATION, "CMmPhoneBookStoreOperationList::CreateNewOperation" );

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
            {

            if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperationCache Phonebook
                pointer = CMmPhoneBookOperationCache::NewL(
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
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        case ECustomReadAlphaStringIPC:
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            {
            //call CmmPhonebookOperatorRead
            if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                {
                pointer = CMmPhoneBookOperationRead::NewL(
                    this,
                    iMmUiccMessHandler,
                    aDataPackage );
                }
            else if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                {
                pointer = CMmPhoneBookOperationRead3g_adn::NewL(
                        this,
                        iMmUiccMessHandler,
                        aDataPackage );
                }
            break;
            }
        case EMmTsyPhoneBookStoreWriteIPC:
            {
            if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperationWrite
                pointer = CMmPhoneBookOperationWrite::NewL(
                    this,
                    aDataPackage );
                }
            else if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                //call CmmPhonebookOperationWrite
                pointer = CMmPhoneBookOperationWrite::NewL(
                    this,
                    aDataPackage );
                }
            break;
            }
        case EMmTsyPhoneBookStoreDeleteIPC:
        case EMmTsyPhoneBookStoreDeleteAllIPC:
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        case ECustomDeleteAlphaStringIPC:
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
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
                }
            break;
            }
        /*
        case EMobilePhoneGetMailboxNumbers:
            {
            pointer =  new ( ELeave )CMmPhonebookOperationCustom(
                iMmPhoneBookStoreMessHandler );
            CleanupStack::PushL( pointer );
            // Add pointer to array
            iPtrOperationArray.AppendL( pointer );
            CleanupStack::Pop( pointer );
            break;
            }
            */
        default:
            {
            // Nothing to do here
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONLIST_BUILDL, "CMmPhoneBookStoreOperationList::BuildL;Unknown aIpc=%d", aIpc );
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
        const TInt aIndexToRead,
        const TBool aMailboxIdExist )
    {
    // Save Name

TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::SeparatePhoneBookEntryFromIsiMsgL. Saving name in commonTSY Array.");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONBASE_STOREPHONEBOOKENTRY, "CMmPhoneBookStoreOperationBase::StorePhonebookEntry. Save name to commonTSY Array" );


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
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONBASE_STOREPHONEBOOKENTRY, "CMmPhoneBookStoreOperationBase::StorePhonebookEntry. Save number to commonTSY Array" );

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


    // If mailbox exists set 1 into higher 8 bits (iLocation is TUint16)
    if ( aMailboxIdExist )
        {
        aEntry.iLocation = 0x0100;
        }

    // Set record index
    aEntry.iLocation = aEntry.iLocation | aIndexToRead;

    // reset both buffers after storing data to commonTSY buffer
    aName.Zero();
    aNumber.Zero();
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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_CONVERTTOUCS2FROMBCD, "CMmPhoneBookStoreMessHandler::ConvertToUcs2FromBCD" );

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
        TDes16 &aSource,
        TDes8 &aTarget,
        TUint16 aFileId )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationBase::ConvertToBCDFromUCS2");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_CONVERTTOBCDFROMUCS2, "CMmPhoneBookStoreMessHandler::ConvertToBCDFromUCS2" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_GETBCDCODEFORUCS, "CMmPhoneBookStoreMessHandler::GetBCDCodeforUCS" );
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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_STOREENTRYTOPHOENBOOKLIST, "CMmPhoneBookStoreMessHandler::StoreEntryToPhoenBookList" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_RESETENTRYINPHONEBOOKLIST, "CMmPhoneBookStoreMessHandler::ResetEntryInPhoneBookList" );
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

TBool CMmPhoneBookStoreMessHandler::IndexCheckInPBList( TUint8 aIndex, TUint8 aPBIndex, TPBEntry& aEntry )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::IndexCheckInPBList");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_INDEXCHECKINPBLIST, "CMmPhoneBookStoreMessHandler::IndexCheckInPBList" );

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

TInt CMmPhoneBookStoreMessHandler::GetIndexForPresentEntry( TUint8 aIndex, TUint8 aPBIndex )
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::GetIndexForPresentEntry");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_GETINDEXFORPRESENTENTRY, "CMmPhoneBookStoreMessHandler::GetIndexForPresentEntry" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_EMPTYENTRYCHECKINPBLIST, "CMmPhoneBookStoreMessHandler::EmptyEntryCheckInPBList" );

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

void CMmPhoneBookStoreMessHandler::UpdateEntryFromList( TPBEntry* aEntry, TUint8 aIndex , TUint8 aPBIndex)
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::RemoveEntryFromList");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_REMOVEENTRYFROMLIST, "CMmPhoneBookStoreMessHandler::RemoveEntryFromList" );

    iPBEntryList[aPBIndex].iEntryList[aIndex] = aEntry;

    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList
// REmove the EXt record form the stored list
// ---------------------------------------------------------------------------
//

void CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList( TUint8 aIndex , TUint8 aPBIndex)
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_REMOVEEXTENTRYFROMLIST, "CMmPhoneBookStoreMessHandler::RemoveExtEntryFromList" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_MESSAGEROUTER, "CMmPhoneBookStoreMessHandler::MessageRouter" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_UICCMESSHANDLER, "CMmPhoneBookStoreMessHandler::UiccMessHandler" );

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
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_PHONETSENDER, "CMmPhoneBookStoreMessHandler::PhoNetSender" );

    return iPhoNetSender;
    }


// End of File
