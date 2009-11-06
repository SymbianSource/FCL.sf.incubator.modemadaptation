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
#include "cmmphonebookstoreoperationlist.h"
#include "cmmphonebookstoreoperationbase.h"
#include "cmmmessagerouter.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookstoremesshandlertraces.h"
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
// CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::\
        CMmPhoneBookStoreMessHandler.");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_CMMPHONEBOOKSTOREMESSHANDLER, "CMmPhoneBookStoreMessHandler::CMmPhoneBookStoreMessHandler" );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler
// C++ destructor.
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::\
        ~CMmPhoneBookStoreMessHandler.");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREMESSHANDLER_CMMPHONEBOOKSTOREMESSHANDLER, "CMmPhoneBookStoreMessHandler::~CMmPhoneBookStoreMessHandler" );
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
    TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - arrived. \
        IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL;aIpc=%d", aIpc );

    TInt ret( KErrNotSupported );
    CMmPhoneBookStoreOperationBase* operation( NULL );

    if ( EMmTsyPhoneBookStoreCacheCancelIPC == aIpc )
        {
        // processing cache cancel IPC
        TUint8 trans( 0 );
        ret = CMmPhoneBookStoreOperationList::CalculateTransactionId(
                  EMmTsyPhoneBookStoreCacheIPC, aDataPackage, trans );
        if ( KErrNone == ret )
            {
            operation = iOperationlist->Find( trans ); // seek from the beginning
            if ( NULL != operation )
                {
                if ( operation->IsPrepared() )
                    { // found running operation - cancel it
                    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - cancel running cache operation");
                    OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - cancel running cache operation" );
                    operation->CancelReq();
                    ret = KErrNone;
                    }
                else
                    { // look for pending operation
                    operation = iOperationlist->Find( trans, operation );
                    if ( NULL != operation && operation->IsPrepared() )
                        { // pending operation is found - cancel it
                        TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - cancel pending cacheing");
                        OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - cancel pending cacheing" );
                        operation->CancelReq();
                        ret = KErrNone;
                        }
                    else
                        { // pending operation is either not found or already canceled
                          // this is error situation: multiple cancel requests
                        TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - error: nothing to cancel");
                        OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - error: nothing to cancel" );
                        ret = KErrServerBusy;
                        }
                    }
                }
            else
                {
                TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - already canceled");
                OstTrace0( TRACE_NORMAL, DUP10_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - already canceled" );
                }
            }
        }
    else if ( EMmTsyPhoneBookStoreCacheIPC == aIpc )
        {
        // processing cache IPC
        TUint8 trans( 0 );
        ret = CMmPhoneBookStoreOperationList::CalculateTransactionId(
                  aIpc, aDataPackage, trans );
        if ( KErrNone == ret )
            {
            operation = iOperationlist->Find( trans );
            if ( NULL != operation )
                { // found running cacheing operation - look for pending
                  // we can not reactivate this one, because even if it is canceled
                  // it is waiting for the response from CellMo
                TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - cacheing already running");
                OstTrace0( TRACE_NORMAL, DUP4_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - cacheing already running" );
                if ( !operation->IsPrepared() )
                    { // pending operation creation/reactivation is allowed
                      // only if running one is canceled
                    operation = iOperationlist->Find( trans, operation );
                    if ( NULL != operation )
                        {
                        TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - pending cacheing is found");
                        OstTrace0( TRACE_NORMAL, DUP5_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - pending cacheing is found" );
                        ret = operation->PrepareReq( aIpc, aDataPackage );

                        if ( KErrArgument == ret )
                            {
                            // request preparation fails - needs to be removed
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL;PrepareReq returns %d", ret);
OstTrace1( TRACE_NORMAL, DUP14_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL;PrepareReq returns %d", ret );
                            iOperationlist->RemoveLastOperationFromList();
                            }
                        else if ( KErrNone != ret )
                            { // this happens when there are 2 consequent
                              // cache requests without cancellation:
                              // cache-cancel-cache-cache
                            TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - error: pending operation is active");
                            OstTrace0( TRACE_NORMAL, DUP6_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - error: pending operation is active" );
                            ret = KErrServerBusy;
                            }
                        else
                            {
                            TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - pending operation reactivated");
                            OstTrace0( TRACE_NORMAL, DUP7_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - pending operation reactivated" );
                            }
                        }
                    else
                        { // no pending operation - create and prepare
                        TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL - preparing pending cacheing");
                        OstTrace0( TRACE_NORMAL, DUP8_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL - preparing pending cacheing" );
                        operation = iOperationlist->BuildL( aDataPackage, aIpc );
                        ret = operation->PrepareReq( aIpc, aDataPackage );

                        if ( KErrArgument == ret )
                            {
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL;PrepareReq returns %d", ret);
OstTrace1( TRACE_NORMAL, DUP13_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL;PrepareReq returns %d", ret );
                            // here we remove the newly created operation
                            // we could turn it back into non-prepared state,
                            // but this breaks the concept
                            iOperationlist->RemoveLastOperationFromList();
                            }
                        }
                    }
                else
                    { // operation was not canceled:
                      // cache-cache
                    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL; running operation was not canceled");
                    OstTrace0( TRACE_NORMAL, DUP9_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL; running operation was not canceled" );
                    ret = KErrServerBusy;
                    }
                }
            else
                { // no cacheing is running - create, prepare & launch
                operation = iOperationlist->BuildL( aDataPackage, aIpc );
                ret = operation->PrepareReq( aIpc, aDataPackage );

                if ( KErrNone == ret )
                    {
                    ret = operation->LaunchReq();
                    }

                if ( KErrNone != ret )
                    {
                    // in case request preparation or phonetsender fail
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL;PrepareReq returns  %d", ret);
OstTrace1( TRACE_NORMAL, DUP12_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL;PrepareReq returns  %d", ret );
                    // it is safe to ignore return value here
                    iOperationlist->RemoveLastOperationFromList();
                    }
                }
            }
        }
    else
        { // all other IPC's
        
        operation = iOperationlist->BuildL( aDataPackage, aIpc );
            ret = operation->UICCCreateReq(aIpc,aDataPackage );

        if ( KErrNone != ret)
            {
TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ExtFuncL;CreateReq returns %d", ret);
OstTrace1( TRACE_NORMAL, DUP11_CMMPHONEBOOKSTOREMESSHANDLER_EXTFUNCL, "CMmPhoneBookStoreMessHandler::ExtFuncL;CreateReq returns %d", ret );
            // it is safe to ignore return value here
            iOperationlist->RemoveLastOperationFromList();
            }
        }

    return ret;
    }



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::ReceiveMessageL
// Called when an ISI message has been received.
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    const TDesC8 &aFileData  ) // received data in UICC Server Message
    {
    TInt ret(KErrNone);
   
    TBool complete( EFalse );
    
    TFLOGSTRING2("TSY: CMmPhoneBookStoreMessHandler::ProcessUiccMsg. transactId:%d", aTraId);
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg" );
    

    switch(aTraId)
        {
        case ETrIdPbInit:
            // Check for operation with transaction id
            CMmPhoneBookStoreOperationBase* operation;
            operation = iOperationlist->Find( aTraId );
            if( operation )
                {
                ret = operation->HandleUICCPbRespL(complete, aStatus, aFileData, aTraId);
                }

            break;
        case ETrIdPbRead:
            break;
        case ETrIdPbUpdate:
            break;
        case ETrIdPbDelete:
            
            break;
        default:
            TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::\
                ProcessUiccMsg. switch case -> default");
            OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKSTOREMESSHANDLER_PROCESSUICCMSG, "CMmPhoneBookStoreMessHandler::ProcessUiccMsg. switch case -> default" );
            break;
        }
    return(ret);
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

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::Gas
// Gets pointer to list of Group Alpha String
// ---------------------------------------------------------------------------
//
CArrayPtrSeg<CMmPhonebookAlphaString>* CMmPhoneBookStoreMessHandler::Gas()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::Gas");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_GAS, "CMmPhoneBookStoreMessHandler::Gas" );

    return iGas;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::SetGas
// Sets pointer to list of Group Alpha String
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreMessHandler::SetGas(
    CArrayPtrSeg<CMmPhonebookAlphaString>* aGas )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::SetGas");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_SETGAS, "CMmPhoneBookStoreMessHandler::SetGas" );
    iGas = aGas;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::Aas
// Gets pointer to list of Additional Alpha String
// ---------------------------------------------------------------------------
//
CArrayPtrSeg<CMmPhonebookAlphaString>* CMmPhoneBookStoreMessHandler::Aas()
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::Aas");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_AAS, "CMmPhoneBookStoreMessHandler::Aas" );

    return iAas;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreMessHandler::SetGas
// Sets pointer to list of Additional Alpha String
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreMessHandler::SetAas(
    CArrayPtrSeg<CMmPhonebookAlphaString>* aAas )
    {
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler::SetAas");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKSTOREMESSHANDLER_SETAAS, "CMmPhoneBookStoreMessHandler::SetAas" );
    iAas = aAas;
    }
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

// End of File
