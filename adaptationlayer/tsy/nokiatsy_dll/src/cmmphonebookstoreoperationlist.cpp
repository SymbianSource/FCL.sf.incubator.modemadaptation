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
#include "cmmphonebookoperationread3g_adn.h"
#include "cmmphonebookoperationwrite.h"
#include "cmmphonebookoperationdelete.h"
#include "cmmphonebookoperationcustom.h"

//For CMmPhoneMessHandler::EServiceTableRequestTypePhonebookMBI
#include "cmmphonemesshandler.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookstoreoperationlistTraces.h"
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
    OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_CMMPHONEBOOKSTOREOPERATIONLIST_TD, "CMmPhoneBookStoreOperationList::CMmPhoneBookStoreOperationList" );
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
    OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKSTOREOPERATIONLIST_CMMPHONEBOOKSTOREOPERATIONLIST_TD, "CMmPhoneBookStoreOperationList::~CMmPhoneBookStoreOperationList" );

    iPtrOperationArrayNew.DeleteAll();
    // resets all elements to 
    iPtrOperationArrayNew.Reset();

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
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_NEWL_TD, "CMmPhoneBookStoreOperationList::NewL" );

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
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_CONSTRUCTL_TD, "CMmPhoneBookStoreOperationList::ConstructL" );
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::AddOperation
// Add Creted operation to List
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationList::AddOperation(
    TUint8 aTrans,
    CMmPhoneBookStoreOperationBase* aOperation )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::AddOperation");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_ADDOPERATION_TD, "CMmPhoneBookStoreOperationList::AddOperation" );

    if( (( aTrans - ETrIdPbOperationStart ) >= 0)&& ((aTrans - ETrIdPbOperationStart) < KMaxPbTrIdCount ))
        {
        iPtrOperationArrayNew[aTrans - ETrIdPbOperationStart] = aOperation;
        }
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::Find
// Search and returns object to correct operation
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreOperationBase* CMmPhoneBookStoreOperationList::Find(
    TUint8 aTrans )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::Find");
OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_FIND_TD, "CMmPhoneBookStoreOperationList::Find" );

    CMmPhoneBookStoreOperationBase* basePointer( NULL );
    //transaction id is basically just an index to operation array + start offset
    //of phonebook operations
    TInt ind ( aTrans - ETrIdPbOperationStart );
    
    if( (ind >= 0)&& (ind < KMaxPbTrIdCount ))
        {
        basePointer = iPtrOperationArrayNew[aTrans - ETrIdPbOperationStart];
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
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::RemoveOperationFromList");
    OstTrace0( TRACE_NORMAL,  DUP1_CMMPHONEBOOKSTOREOPERATIONLIST_REMOVEOPERATIONFROMLIST_TD, "CMmPhoneBookStoreOperationList::RemoveOperationFromList" );
    

    delete iPtrOperationArrayNew[aTrans - ETrIdPbOperationStart];
    iPtrOperationArrayNew[aTrans - ETrIdPbOperationStart] = NULL;
            
    TFLOGSTRING2("TSY: CMmPhoneBookStoreOperationList -Phonebook operation removed succesfully from index: %d" , ( aTrans - ETrIdPbOperationStart ) );

    TF_ASSERT( ( aTrans - ETrIdPbOperationStart ) < KMaxPbTrIdCount );
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::FindEmptyIndexTransId
// Check for the empty Index available in operation array
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreOperationList::FindEmptyIndexTransId()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::FindEmptyIndexTransId");
    OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_FINDEMPTYINDEXTRANSID_TD, "CMmPhoneBookStoreOperationList::FindEmptyIndexTransId" );
    
    TInt transId = -1;
    for( TInt count = 0 ; count < KMaxPbTrIdCount ; count++)
        {
        if( iPtrOperationArrayNew[count] == NULL )
            {
            transId = count + ETrIdPbOperationStart ;
            break;
            }
        }
    return transId;
    }


// ---------------------------------------------------------------------------
// CMmPhoneBookStoreOperationList::CancelOperation
// Cancel all operation with same phonebook name
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreOperationList::CancelOperation( TName &aPhoneBook)
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreOperationList::CancelOperation");
    OstTrace0( TRACE_NORMAL,  CMMPHONEBOOKSTOREOPERATIONLIST_CANCELOPERATION_TD, "CMmPhoneBookStoreOperationList::CancelOperation" );
  
    // Chekc for all operation which have the same phonebook name
    for( TInt count = 0 ; count < KMaxPbTrIdCount ; count++)
        {
        if( iPtrOperationArrayNew[count]!= NULL )
            {
            // Call Cancel function for that operation
            iPtrOperationArrayNew[count]->CancelReq( aPhoneBook );
            }
        }
    }



// End of File
