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
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tsylogger.h"

#include "cmmphonebookstoreoperationlist.h"
#include "cmmphonebookoperationinit.h"
#include "cmmphonebookoperationinit3G_adn.h"
#include "cmmphonebookoperationcache.h"
#include "cmmphonebookoperationread.h"
#include "cmmphonebookoperationwrite.h"
#include "cmmphonebookoperationdelete.h"
#include "cmmphonebookoperationcustom.h"

//For CMmPhoneMessHandler::EServiceTableRequestTypePhonebookMBI
#include "cmmphonemesshandler.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookstoreoperationlisttraces.h"
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
// CMmPhoneBookStoreOperationList::CMmPhoneBookStoreOperationList
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationList::CMmPhoneBookStoreOperationList()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::\
        CMmPhoneBookStoreOperationList");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_CMMPHONEBOOKSTOREOPERATIONLIST, "CMmPhoneBookStoreOperationList::CMmPhoneBookStoreOperationList" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::~CMmPhoneBookStoreOperationList
// C++ destructor.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationList::~CMmPhoneBookStoreOperationList()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::\
        ~CMmPhoneBookStoreOperationList");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONLIST_CMMPHONEBOOKSTOREOPERATIONLIST, "CMmPhoneBookStoreOperationList::~CMmPhoneBookStoreOperationList" );

    iPtrOperationArray.Reset();
    iPtrOperationArray.Close();
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::NewL
// Two-phased constructor.
// Creates a new CMmPhoneBookStoreOperationList object instance.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationList* CMmPhoneBookStoreOperationList::NewL(
    CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
    CMmUiccMessHandler* aMmUiccMessHandler
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::NewL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_NEWL, "CMmPhoneBookStoreOperationList::NewL" );

    CMmPhoneBookStoreOperationList* mmPhoneBookOperationList =
        new( ELeave ) CMmPhoneBookStoreOperationList();

    mmPhoneBookOperationList->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;
    
    mmPhoneBookOperationList->iUiccMessHandler = aMmUiccMessHandler;

    return mmPhoneBookOperationList;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::ConstructL
// Initialises object attributes.
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationList::ConstructL()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::ConstructL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_CONSTRUCTL, "CMmPhoneBookStoreOperationList::ConstructL" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::Build
// Separate request and create correct object
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationBase* CMmPhoneBookStoreOperationList::BuildL(
    const CMmDataPackage* aDataPackage,
    TInt aIpc
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::Build");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_BUILDL, "CMmPhoneBookStoreOperationList::BuildL" );

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
            if(UICC_CARD_TYPE_UICC == iUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperatorInit3G_ADN Phonebook
                pointer = CMmPhoneBookOperationInit3G_adn::NewL(
                        iMmPhoneBookStoreMessHandler,
                        iUiccMessHandler,
                        aDataPackage
                        );
                CleanupStack::PushL( pointer );
                // Add pointer to array
                iPtrOperationArray.AppendL( pointer );
                CleanupStack::Pop( pointer );
                
                }
            else if(UICC_CARD_TYPE_ICC ==  iUiccMessHandler->GetCardType())
                {
                //call CmmPhonebookOperatorInit
                pointer = CMmPhoneBookOperationInit::NewL(
                    iMmPhoneBookStoreMessHandler,
                    iUiccMessHandler,
                    aDataPackage
                    );
                CleanupStack::PushL( pointer );
                // Add pointer to array
                iPtrOperationArray.AppendL( pointer );
                CleanupStack::Pop( pointer );
                }
            break;
            }
            /*
        case EMmTsyPhoneBookStoreGetInfoIPC:
        case EMmTsyPhoneBookStoreCacheIPC:
            {
            //call CmmPhonebookOperatorCache
            pointer = CMmPhoneBookOperationCache::NewL(
                iMmPhoneBookStoreMessHandler,
                aIpc, aDataPackage );
            CleanupStack::PushL( pointer );
            // Add pointer to array
            iPtrOperationArray.AppendL( pointer );
            CleanupStack::Pop( pointer );
            break;
            }
            */
        case EMmTsyPhoneBookStoreReadIPC:
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        case ECustomReadAlphaStringIPC:
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            {
            //call CmmPhonebookOperatorRead
            pointer = CMmPhoneBookOperationRead::NewL(
                iMmPhoneBookStoreMessHandler,
                aDataPackage );
            CleanupStack::PushL( pointer );
            // Add pointer to array
            iPtrOperationArray.AppendL( pointer );
            CleanupStack::Pop( pointer );
            break;
            }
            /*
        case EMmTsyPhoneBookStoreWriteIPC:

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        case ECustomWriteAlphaStringIPC:
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            {
            //call CmmPhonebookOperationWrite
            pointer = CMmPhoneBookOperationWrite::NewL(
                iMmPhoneBookStoreMessHandler,
                aDataPackage );
            CleanupStack::PushL( pointer );
            // Add pointer to array
            iPtrOperationArray.AppendL( pointer );
            CleanupStack::Pop( pointer );
            break;
            }
        case EMmTsyPhoneBookStoreDeleteIPC:
        case EMmTsyPhoneBookStoreDeleteAllIPC:
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        case ECustomDeleteAlphaStringIPC:
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            {
            //call CmmPhonebookOperatorDelete
            pointer = CMmPhoneBookOperationDelete::NewL(
                iMmPhoneBookStoreMessHandler,
                aDataPackage );
            CleanupStack::PushL( pointer );
            // Add pointer to array
            iPtrOperationArray.AppendL( pointer );
            CleanupStack::Pop( pointer );
            break;
            }
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
            TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - \
                Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREOPERATIONLIST_BUILDL, "CMmPhoneBookStoreOperationList::BuildL;Unknown aIpc=%d", aIpc );
            break;
            }
        }

    //return pointer to right operation
    return pointer;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::Find
// Search and returns object to correct operation
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationBase* CMmPhoneBookStoreOperationList::Find(
    TUint8 aTrans,
    const CMmPhoneBookStoreOperationBase* aSearchFrom/*=NULL*/ )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::Find");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_FIND, "CMmPhoneBookStoreOperationList::Find" );

    CMmPhoneBookStoreOperationBase* basePointer( NULL );

    // search correct pointer from array using aTrans
    TUint ind( 0 );
    if ( NULL != aSearchFrom )
        {
        // if object to search from specified, find its index at first
        for ( ; ind < iPtrOperationArray.Count(); ind++ )
            {
            if ( aSearchFrom == iPtrOperationArray[ind] )
                {
                ind++;
                break;
                }
            }
        }
    // looking for object with given transaction id
    // starting index may be not 0 if aSearchFrom is specified
    for ( ; ind < iPtrOperationArray.Count(); ind++)
        {
        if( ( iPtrOperationArray[ind] )->TransId() == aTrans )
            {
            basePointer = iPtrOperationArray[ind];
            break;
            }
        }
    // return pointer if found otherwise NULL
    return basePointer;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::RemoveOperationFromList
// Search and delete correct object from list
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationList::RemoveOperationFromList(
    TUint8 aTrans )  // transactionId
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::\
        RemoveOperationFromList");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_REMOVEOPERATIONFROMLIST, "CMmPhoneBookStoreOperationList::RemoveOperationFromList" );

    TUint ind( 0 );
    TUint operationCount( iPtrOperationArray.Count() );

    // search correct pointer from array using aTrans
    while ( ind < operationCount )
        {
        if( ( iPtrOperationArray[ind] )->TransId() == aTrans )
            {
            delete iPtrOperationArray[ind];
            iPtrOperationArray.Remove( ind );
            iPtrOperationArray.Compress();
            TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationList -Phonebook \
                operation removed succesfully from index: %d" ,ind );
            break;
            }
        ind++;
        }
    TF_ASSERT( ind < operationCount );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::RemoveLastOperationFromList
// Removes last operation from operation list.
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationList::RemoveLastOperationFromList()
    {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::RemoveLastOperationFromList");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_REMOVELASTOPERATIONFROMLIST, "CMmPhoneBookStoreOperationList::RemoveLastOperationFromList" );

    TInt ret( KErrNone );
    TInt count( iPtrOperationArray.Count() );

    if ( 0 < count )
        {
        iPtrOperationArray.Remove( count - 1 );
        }
    else
        {
TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::RemoveLastOperationFromList - no operation to remove");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONLIST_REMOVELASTOPERATIONFROMLIST, "CMmPhoneBookStoreOperationList::RemoveLastOperationFromList - no operation to remove" );

        ret = KErrNotFound;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::CalculateTransactionId
// Calculates transaction Id from IPC number and data package
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationList::CalculateTransactionId(
    TInt aIpc,
    const CMmDataPackage* /*aDataPackage*/,
    TUint8& /*aTransactionId*/
    )
    {
    TInt ret( KErrNone );

    TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationList::CalculateTransactionId;aIpc=%d", aIpc);
OstTrace1( TRACE_NORMAL, CMMPHONEBOOKSTOREOPERATIONLIST_CALCULATETRANSACTIONID, "CMmPhoneBookStoreOperationList::CalculateTransactionId;aIpc=%d", aIpc );

    switch ( aIpc )
        {
        	/*
        case EMmTsyPhoneBookStoreGetInfoIPC:
        case EMmTsyPhoneBookStoreCacheIPC:
            {
            ret = CMmPhoneBookOperationCache::CalculateTransactionId(
                      aIpc, aDataPackage, aTransactionId );
            break;
            }
            */
        default:
            {
            TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::CalculateTransactionId;unknown ipc");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREOPERATIONLIST_CALCULATETRANSACTIONID, "CMmPhoneBookStoreOperationList::CalculateTransactionId; unknown ipc" );
            ret = KErrNotSupported;
            }
        }

    return ret;
    }

// End of File
