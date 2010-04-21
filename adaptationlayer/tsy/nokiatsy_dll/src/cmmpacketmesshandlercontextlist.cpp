/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmpacketmesshandlercontextlist.h"
#include "cmmpacketservicemesshandler.h"
#include <gpdsisi.h>
#include "tsylogger.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpacketmesshandlercontextlistTraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS

// A list for defined proxy ids ( see IscNokiaDefinitions.h )
// Max number of supported contexts is limited to be 11
// Get defined proxy ids
const TUint8 KDefinedProxyIds[KMmMaxNumberOfContexts] = {
    KMmPacketContextProxy1_DOS, KMmPacketContextProxy2_DOS,
    KMmPacketContextProxy3_DOS, KMmPacketContextProxy4_DOS,
    KMmPacketContextProxy5_DOS, KMmPacketContextProxy6_DOS,
    KMmPacketContextProxy7_DOS, KMmPacketContextProxy8_DOS,
    KMmPacketContextProxy9_DOS, KMmPacketContextProxy10_DOS,
    KMmPacketContextProxy11_DOS };

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ============================= LOCAL FUNCTIONS ===============================

    //None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::CMmPacketContextMesshandlerList
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMmPacketContextMesshandlerList::CMmPacketContextMesshandlerList()
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::CMmPacketContextMesshandlerList");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CMMPACKETCONTEXTMESSHANDLERLIST, "CMmPacketContextMesshandlerList::CMmPacketContextMesshandlerList" );

    //None
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmPacketContextMesshandlerList* CMmPacketContextMesshandlerList::NewL()
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::NewL");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_NEWL, "CMmPacketContextMesshandlerList::NewL" );

    CMmPacketContextMesshandlerList* const contextList =
        new ( ELeave ) CMmPacketContextMesshandlerList();

    CleanupStack::PushL( contextList );
    contextList->ConstructL();
    CleanupStack::Pop( contextList );

    return contextList;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMesshandlerList::ConstructL()
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::ConstructL");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CONSTRUCTL, "CMmPacketContextMesshandlerList::ConstructL" );

    InitAttributesAndLists();
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::InitAttributesAndLists
// Initialize used lists and attributes.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMesshandlerList::InitAttributesAndLists()
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::InitAttributesAndLists");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_INITATTRIBUTESANDLISTS, "CMmPacketContextMesshandlerList::InitAttributesAndLists" );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        InitializeContextInfoList( i );
        }

    iNumberOfCreatedContexts = 0;

    iDialUpContextId = GPDS_CID_VOID;
    iDialUpContextName.Zero();

    iSecondaryDialUpContextId = GPDS_CID_VOID;
    iSecondaryDialUpContextName.Zero();
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::~CMmPacketContextMesshandlerList
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CMmPacketContextMesshandlerList::~CMmPacketContextMesshandlerList()
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::~CMmPacketContextMesshandlerList" );
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CMMPACKETCONTEXTMESSHANDLERLIST, "CMmPacketContextMesshandlerList::~CMmPacketContextMesshandlerList" );
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( iContextInfoArray[i].iChannelOpened )
            {
            iContextInfoArray[i].iIscApi.Close();
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::AddObject
// Adds new context info to the info list.
// Sets used proxy id reserved in proxy id list.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::AddObject(
    const TInt aObjectId,
    const TUint8 aChannelId,
    const TInfoName& aContextName,
    const TInfoName& aHostContextName )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::AddObject. Object Id: %d", aObjectId );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_ADDOBJECT, "CMmPacketContextMesshandlerList::AddObject;aObjectId=%d", aObjectId );

    TInt ret( KErrArgument );

    if( 0 != aContextName.Compare( KStringExternal ) ||
        0 != aContextName.Compare( KStringExternal2 ) )
        {
        if ( KMmMaxNumberOfContexts > iNumberOfCreatedContexts )
            {
            // Find given proxy id from proxy id list
            for (  TInt i = 0; i < KMmMaxNumberOfContexts; i++  )
                {
                if ( aChannelId == iContextInfoArray[i].iChannelId )
                    {
                    iContextInfoArray[i].iChannelId =
                        aChannelId;
                    iContextInfoArray[i].iContextName =
                        aContextName;
                    iContextInfoArray[i].iHostContextName =
                        aHostContextName;
                    iContextInfoArray[i].iObjectId =
                        aObjectId;
                    // Increase number of created contexts
                    iNumberOfCreatedContexts++;
                    // proxy id found, break
                    i = KMmMaxNumberOfContexts;
                    ret = KErrNone;
                    }
                }
            }
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::RemoveObject
// Removes context info from the info list.
// Sets used proxy id released in proxy id list.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::RemoveObject(
    TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::RemoveObject. Context Id: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_REMOVEOBJECT, "CMmPacketContextMesshandlerList::RemoveObject;aContextId=%hhu", aContextId );

    // Set ret to KErrArgument to check that context is in list
    TInt ret( KErrArgument );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find aContext from context info list
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::RemoveObject. Context Name: %S", &iContextInfoArray[i].iContextName );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_REMOVEOBJECT, "CMmPacketContextMesshandlerList::RemoveObject;Context Name=%S", iContextInfoArray[i].iContextName );

            // Reset object information
            InitializeContextInfoList( i );
            i = KMmMaxNumberOfContexts;
            iNumberOfCreatedContexts--;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::NumberOfContexts
// This function returns the number of created contexts.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::NumberOfContexts() const
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::NumberOfContexts, %d", iNumberOfCreatedContexts );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_NUMBEROFCONTEXTS, "CMmPacketContextMesshandlerList::NumberOfContexts;iNumberOfCreatedContexts=%d", iNumberOfCreatedContexts );

    return iNumberOfCreatedContexts;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetContextId
// Sets context id to the context info list.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetContextId(
    const TUint8 aContextId,
    const TUint8 aTransactionId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetContextId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETCONTEXTID, "CMmPacketContextMesshandlerList::SetContextId;aContextId=%hhu", aContextId );

    TInt ret( KErrNotFound );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find object from the list
        if ( aTransactionId == iContextInfoArray[i].iCreateContextIdTransId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetContextId. ContextName: %S", &iContextInfoArray[i].iContextName );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETCONTEXTID, "CMmPacketContextMesshandlerList::SetContextId;ContextName=%S", iContextInfoArray[i].iContextName );
            // Set context id to the context info list
            iContextInfoArray[i].iContextId = aContextId;
            // Object found, return KErrNone
            ret = KErrNone;
            }
        // no else
        }
    // Object not found, return KErrNotFound
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetContextNameByContextId
// This function returns context name by context id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::GetContextNameByContextId(
    const TUint8 aContextId,
    TInfoName& aContextName )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetContextNameByContextId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTNAMEBYCONTEXTID, "CMmPacketContextMesshandlerList::GetContextNameByContextId;aContextId=%hhu", aContextId );

    TInt ret( KErrArgument );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find object from the list
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetContextNameByContextId. ContextName: %S", &iContextInfoArray[i].iContextName );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTNAMEBYCONTEXTID, "CMmPacketContextMesshandlerList::GetContextNameByContextId;ContextName=%S", iContextInfoArray[i].iContextName );
            // Set context name to aName
            aContextName = iContextInfoArray[i].iContextName;
            // Context id found, return KErrNone
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }
    // Context id not found, return KErrArgument
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::PipeHandleByContextName
// This function returns pipehandle by context name.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::PipeHandleByContextName(
    const TInfoName* aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::PipeHandleByContextName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_PIPEHANDLEBYCONTEXTNAME, "CMmPacketContextMesshandlerList::PipeHandleByContextName" );

    TUint8 pipeHandle( KInvalidPipeHandle );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::PipeHandleByContextName, PipeHandle: %d", iContextInfoArray[i].iPipeHandle );
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_PIPEHANDLEBYCONTEXTNAME, "CMmPacketContextMesshandlerList::PipeHandleByContextName;PipeHandle=%d", iContextInfoArray[i].iPipeHandle );
            // Return object from the context info list
            pipeHandle = iContextInfoArray[i].iPipeHandle;
            }
        // no else
        }
    // Valid values to return are, 0x00, 0x01, 0xFF = KInvalidPipeHandle
    return pipeHandle;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ProxyIdByContextName
// This function returns proxy id by context name.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::ProxyIdByContextName(
    const TInfoName* aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::ProxyIdByContextName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_PROXYIDBYCONTEXTNAME, "CMmPacketContextMesshandlerList::ProxyIdByContextName" );

    TUint8 proxyId( KTUint8NotDefined );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ProxyIdByContextName, ProxyId: %d", iContextInfoArray[i].iChannelId );
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_PROXYIDBYCONTEXTNAME, "CMmPacketContextMesshandlerList::ProxyIdByContextName;ProxyId=%d", iContextInfoArray[i].iChannelId );

            proxyId = iContextInfoArray[i].iChannelId;
            }
        // no else
        }
    // Valid values to return are, 0x00, 0x01, 0xFF = Not defined
    return proxyId;
    }

TUint8 CMmPacketContextMesshandlerList::ProxyIdByPipeHandle(
    TUint8 aPipeHandle
    )
    {
    TFLOGSTRING2("TSY CMmPacketContextMesshandlerList::ProxyIdByPipeHandle 0x%x>", aPipeHandle );
    OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_PROXYIDBYPIPEHANDLE, "CMmPacketContextMesshandlerList::ProxyIdByPipeHandle;aPipeHandle=%hhx", aPipeHandle );

    TUint8 proxyId( KTUint8NotDefined );
    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aPipeHandle == iContextInfoArray[i].iPipeHandle )
            {
TFLOGSTRING3("TSY CMmPacketContextMesshandlerList::ProxyIdByPipeHandle found 0x%x for pipehandle 0x%x", proxyId, aPipeHandle );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_PROXYIDBYPIPEHANDLE, "CMmPacketContextMesshandlerList::ProxyIdByPipeHandle;proxyId=%hhx;aPipeHandle=%hhx", proxyId, aPipeHandle );
            proxyId = iContextInfoArray[i].iChannelId;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    // Valid values to return are, 0x00, 0x01, 0xFF = Not defined
    return proxyId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::OpenChannel
// Opens channel with the specified id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::OpenChannel( TUint8 aChannelId )
    {
    TFLOGSTRING2("TSY: MmPacketContextMesshandlerList::OpenChannel;aChannelId=%x", aChannelId);
    OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_OPENCHANNEL, "CMmPacketContextMesshandlerList::OpenChannel;aChannelId=%hhu", aChannelId );
    TInt ret( KErrNotReady );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++)
        {
        if ( aChannelId == iContextInfoArray[i].iChannelId )
            {
            if ( !iContextInfoArray[i].iChannelOpened )
                {
                TRequestStatus status( KRequestPending );
                iContextInfoArray[i].iIscApi.Open( aChannelId, status );
                User::WaitForRequest( status );
                ret = status.Int();
                if ( KErrNone == ret )
                    {
                    iContextInfoArray[i].iChannelOpened = ETrue;
                    }
                }
            else
                {
                ret = KErrAlreadyExists;
                }
            i = KMmMaxNumberOfContexts;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::CloseChannel
// Closes channel with the specified id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::CloseChannel( TUint8 aChannelId )
    {
    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::CloseChannel;aChannelId=%x", aChannelId);
    OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CLOSECHANNEL, "CMmPacketContextMesshandlerList::CloseChannel;aChannelId=%hhu", aChannelId );
    TInt ret( KErrNotFound );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aChannelId == iContextInfoArray[i].iChannelId )
            {
            // closing of closed channel is OK
            iContextInfoArray[i].iIscApi.Close();
            ret = KErrNone;
            iContextInfoArray[i].iChannelOpened = EFalse;
            i = KMmMaxNumberOfContexts;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ContextNameByProxyId
// This function returns context name by proxy id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::ContextNameByProxyId(
    const TUint8 aProxyId,
    TInfoName& aContextName )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ContextNameByProxyId. ProxyId: %d", aProxyId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CONTEXTNAMEBYPROXYID, "CMmPacketContextMesshandlerList::ContextNameByProxyId;aProxyId=%hhu", aProxyId );

    TInt ret( KErrArgument );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aProxyId == iContextInfoArray[i].iChannelId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ContextNameByProxyId. ContextName: %S", &iContextInfoArray[i].iContextName );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CONTEXTNAMEBYPROXYID, "CMmPacketContextMesshandlerList::ContextNameByProxyId;ContextName=%S", iContextInfoArray[i].iContextName );
            // Set context name to aName
            aContextName = iContextInfoArray[i].iContextName;
            // Context id found, return KErrNone
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetContextTypeByName
// This function returns context type by context name.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::GetContextTypeByName(
    const TInfoName* const aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetContextTypeByName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTTYPEBYNAME, "CMmPacketContextMesshandlerList::GetContextTypeByName" );

    TUint8 ret( GPDS_CONT_TYPE_NORMAL );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
            // Return object from the context info list
            // return iContextInfoArray[i].iContextType;
            if( 0 != iContextInfoArray[i].iHostContextName.Length() )
                {
                 ret = GPDS_CONT_TYPE_SEC;
                }
            // no else
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetContextTypeByName. ContextName: %S, ContextType: %d", &iContextInfoArray[i].iContextName, ret );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTTYPEBYNAME, "CMmPacketContextMesshandlerList::GetContextTypeByName;ContextName=%S;ContextType=%hhu", iContextInfoArray[i].iContextName, ret );
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetCreateContextIdTransId
// Sets iCreateContextIdTransId of the specified context by context name.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetCreateContextIdTransId(
    const TUint8 aTransactionId,
    const TInfoName* aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::SetCreateContextIdTransId");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETCREATECONTEXTIDTRANSID, "CMmPacketContextMesshandlerList::SetCreateContextIdTransId" );

    TInt ret( KErrNotFound );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetCreateContextIdTransId. ContextName: %S, IdTransId: %d", &iContextInfoArray[i].iContextName, aTransactionId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETCREATECONTEXTIDTRANSID, "CMmPacketContextMesshandlerList::SetCreateContextIdTransId;ContextName=%S;aTransactionId=%d", iContextInfoArray[i].iContextName, aTransactionId );
            // Return object from the context info list
            iContextInfoArray[i].iCreateContextIdTransId = aTransactionId;
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetPipeCreateTransId
// Sets iPipeCreateTransId of the specified context by context name.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetPipeCreateTransId(
    const TUint8 aTransactionId,
    const TInfoName* aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::SetPipeCreateTransId");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPECREATETRANSID, "CMmPacketContextMesshandlerList::SetPipeCreateTransId" );

    TInt ret( KErrNotFound );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetPipeCreateTransId. ContextName: %S, CreateTransId: %d", &iContextInfoArray[i].iContextName, aTransactionId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPECREATETRANSID, "CMmPacketContextMesshandlerList::SetPipeCreateTransId;ContextName=%S;aTransactionId=%hhu", iContextInfoArray[i].iContextName, aTransactionId );
            // Return object from the context info list
            iContextInfoArray[i].iPipeCreateTransId = aTransactionId;
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetGpdsLlConfigureTraId
// Sets iGpdsLlConfigureTraId of the specified context by context Id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetGpdsLlConfigureTraId(
    const TUint8 aTransactionId,
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetGpdsLlConfigureTraId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETGPDSLLCONFIGURETRAID, "CMmPacketContextMesshandlerList::SetGpdsLlConfigureTraId;aContextId=%hhu", aContextId );

    TInt ret( KErrNotFound );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetGpdsLlConfigureTraId. ContextName: %S, L1ConfigureTransId: %d", &iContextInfoArray[i].iContextName, aTransactionId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETGPDSLLCONFIGURETRAID, "CMmPacketContextMesshandlerList::SetGpdsLlConfigureTraId;ContextName=%S;aTransactionId=%hhu", iContextInfoArray[i].iContextName, aTransactionId );

            // Return object from the context info list
            iContextInfoArray[i].iGpdsLlConfigureTraId = aTransactionId;
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetRemovePipeTraId
// Sets the iPipeRemoveTraId specified by pipe handle.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetRemovePipeTraId(
    const TUint8 aTransactionId,
    const TInt aPipeHandle )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetRemovePipeTraId. PipeHandle: %d", aPipeHandle );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETREMOVEPIPETRAID, "CMmPacketContextMesshandlerList::SetRemovePipeTraId;aPipeHandle=%d", aPipeHandle );

    TInt ret( KErrNotFound );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aPipeHandle == iContextInfoArray[i].iPipeHandle )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetRemovePipeTraId. ContextName: %S, RemovePipeTraId: %d", &iContextInfoArray[i].iContextName, aTransactionId );
OstTraceExt2( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLERLIST_SETREMOVEPIPETRAID, "CMmPacketContextMesshandlerList::SetRemovePipeTraId;ContextName=%S;aTransactionId=%hhu", iContextInfoArray[i].iContextName, aTransactionId );

            // Return object from the context info list
            iContextInfoArray[i].iRemovePipeTraId = aTransactionId;
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetActivationTraId
// Sets the iActivationTraId specified by context id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetActivationTraId(
    const TUint8 aContextId,
    const TUint8 aTransactionId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetActivationTraId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETACTIVATIONTRAID, "CMmPacketContextMesshandlerList::SetActivationTraId;aContextId=%hhu", aContextId );

    TInt ret( KErrNotFound );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetActivationTraId. ContextName: %S, ActivationTraId: %d", &iContextInfoArray[i].iContextName, aTransactionId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETACTIVATIONTRAID, "CMmPacketContextMesshandlerList::SetActivationTraId;ContextName=%S;aTransactionId=%hhu", iContextInfoArray[i].iContextName, aTransactionId );

            // Return object from the context info list
            iContextInfoArray[i].iActivationTraId = aTransactionId;
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetAndResetActivationTraId
// Resets the iActivationTraId and returns saved iActivationTraId
// specified by context Id.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::GetAndResetActivationTraId(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetAndResetActivationTraId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETANDRESETACTIVATIONTRAID, "CMmPacketContextMesshandlerList::GetAndResetActivationTraId;aContextId=%hhu", aContextId );

    TUint8 transId( KTUint8NotDefined );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
            // Return object from the context info list
            transId = iContextInfoArray[i].iActivationTraId;
            iContextInfoArray[i].iActivationTraId = KTUint8NotDefined;

TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetAndResetActivationTraId. ContextName: %S, ActivationTraId: %d", &iContextInfoArray[i].iContextName, transId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETANDRESETACTIVATIONTRAID, "CMmPacketContextMesshandlerList::GetAndResetActivationTraId;ContextName=%S;transId=%hhu", iContextInfoArray[i].iContextName, transId );
            }
        // no else
        }

    return transId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetDeletionTraId
// Sets the iDeleteTraId specified by context id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetDeletionTraId(
    const TUint8 aContextId,
    const TUint8 aTransactionId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetDeletionTraId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETDELETIONTRAID, "CMmPacketContextMesshandlerList::SetDeletionTraId;aContextId=%hhu", aContextId );

    TInt ret( KErrNotFound );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetDeletionTraId. ContextName: %S, DeleteTraId: %d", &iContextInfoArray[i].iContextName, aTransactionId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETDELETIONTRAID, "CMmPacketContextMesshandlerList::SetDeletionTraId;ContextName=%S;aTransactionId=%hhu", iContextInfoArray[i].iContextName, aTransactionId );
            // Return object from the context info list
            iContextInfoArray[i].iDeleteTraId= aTransactionId;
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetAndResetDeletionTraId
// Resets the iDeleteTraId and returns saved iDeleteTraId
// specified by context Id.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::GetAndResetDeletionTraId(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetAndResetDeletionTraId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETANDRESETDELETIONTRAID, "CMmPacketContextMesshandlerList::GetAndResetDeletionTraId;aContextId=%hhu", aContextId );

    TUint8 transId( KTUint8NotDefined );

     for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
            // Return object from the context info list
            transId = iContextInfoArray[i].iDeleteTraId;
            iContextInfoArray[i].iDeleteTraId = KTUint8NotDefined;

TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetAndResetActivationTraId. ContextName: %S, DeleteTraId: %d", &iContextInfoArray[i].iContextName, transId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETANDRESETDELETIONTRAID, "CMmPacketContextMesshandlerList::GetAndResetDeletionTraId;ContextName=%s;transId=%hhu", iContextInfoArray[i].iContextName, transId );
            }
        // no else
        }

    return transId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::CreateContextIdTransIdByPipeHandle
// Gets create context id transaction id by pipehandle.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::CreateContextIdTransIdByPipeHandle(
    const TInt aPipeHandle  )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::CreateContextIdTransIdByPipeHandle. PipeHandle: %d", aPipeHandle );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CREATECONTEXTIDTRANSIDBYPIPEHANDLE, "CMmPacketContextMesshandlerList::CreateContextIdTransIdByPipeHandle;aPipeHandle=%d", aPipeHandle );

    TUint8 transId( KTUint8NotDefined );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aPipeHandle == iContextInfoArray[i].iPipeHandle )
            {
            // Return object from the context info list
            transId = iContextInfoArray[i].iCreateContextIdTransId;
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::CreateContextIdTransIdByPipeHandle. ContextName: %S, CreateContextIdTransId: %d", &iContextInfoArray[i].iContextName, transId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CREATECONTEXTIDTRANSIDBYPIPEHANDLE, "CMmPacketContextMesshandlerList::CreateContextIdTransIdByPipeHandle;ContextName=%S;transId=%hhu", iContextInfoArray[i].iContextName, transId );
            }
        // no else
        }

    // Valid values to return are, 0x00, 0x01, 0xFF = Not defined
    return transId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::CreateContextIdTraIdByContextId
// Gets create context id transaction id by context id.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::CreateContextIdTraIdByContextId(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::CreateContextIdTraIdByContextId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CREATECONTEXTIDTRAIDBYCONTEXTID, "CMmPacketContextMesshandlerList::CreateContextIdTraIdByContextId;aContextId=%hhu", aContextId );

    TUint8 transId( KTUint8NotDefined );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
            // Return object from the context info list
            transId = iContextInfoArray[i].iCreateContextIdTransId;

TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::CreateContextIdTraIdByContextId. ContextName: %S, CreateContextIdTransId: %d", &iContextInfoArray[i].iContextName, transId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CREATECONTEXTIDTRAIDBYCONTEXTID, "CMmPacketContextMesshandlerList::CreateContextIdTraIdByContextId;ContextName=%S;transId=%hhu", iContextInfoArray[i].iContextName, transId );
            }
        // no else
        }

    // Valid values to return are, 0x00, 0x01, 0xFF = Not defined
    return transId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::PipeHandleByContextId
// Gets pipehandle specified by context Id.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::PipeHandleByContextId(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::PipeHandleByContextId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_PIPEHANDLEBYCONTEXTID, "CMmPacketContextMesshandlerList::PipeHandleByContextId;aContextId=%hhu", aContextId );

    TUint8 pipeHandle( KInvalidPipeHandle );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
            // Return object from the context info list
            pipeHandle = iContextInfoArray[i].iPipeHandle;
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::PipeHandleByContextId. ContextName: %S, PipeHandle: %d", &iContextInfoArray[i].iContextName, pipeHandle );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_PIPEHANDLEBYCONTEXTID, "CMmPacketContextMesshandlerList::PipeHandleByContextId;ContextName=%S;pipeHandle=%hhu", iContextInfoArray[i].iContextName, pipeHandle );
            }
        // no else
        }

    // Valid values to return are, 0x00, 0x01, 0xFF = KInvalidPipeHandle
    return pipeHandle;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::PipeCreateTransIdOkByTransId
// Checks if transaction id created early match with given.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::PipeCreateTransIdOkByTransId(
    const TUint8 aTransId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::PipeCreateTransIdOkByTransId. TransId: %d", aTransId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_PIPECREATETRANSIDOKBYTRANSID, "CMmPacketContextMesshandlerList::PipeCreateTransIdOkByTransId;aTransId=%hhu", aTransId );

    TInt ret( KErrNotFound );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aTransId == iContextInfoArray[i].iPipeCreateTransId )
            {
TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::PipeCreateTransIdOkByTransId. TransId Matched");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_PIPECREATETRANSIDOKBYTRANSID, "CMmPacketContextMesshandlerList::PipeCreateTransIdOkByTransId, TransId Matched" );
            ret = KErrNone;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::CreateContextIdTraIdOK
// Checks if transaction id created early match with given.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::CreateContextIdTraIdOK(
    const TUint8 aTransId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::CreateContextIdTraIdOK. TransId: %d", aTransId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CREATECONTEXTIDTRAIDOK, "CMmPacketContextMesshandlerList::CreateContextIdTraIdOK;aTransId=%hhu", aTransId );

    TInt ret( KErrNotFound );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aTransId == iContextInfoArray[i].iCreateContextIdTransId )
            {
TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::CreateContextIdTraIdOK. TransId Matched");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CREATECONTEXTIDTRAIDOK, "CMmPacketContextMesshandlerList::CreateContextIdTraIdOK, TransId Matched" );
            ret = KErrNone;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GpdsLlConfigureTraIdOK
// Checks if transaction id created early match with given.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::GpdsLlConfigureTraIdOK(
    const TUint8 aTransId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GpdsLlConfigureTraIdOK. TransId: %d", aTransId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GPDSLLCONFIGURETRAIDOK, "CMmPacketContextMesshandlerList::GpdsLlConfigureTraIdOK;aTransId=%hhu", aTransId );

    TInt ret( KErrNotFound );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aTransId == iContextInfoArray[i].iGpdsLlConfigureTraId )
            {
TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GpdsLlConfigureTraIdOK. TransId Matched");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GPDSLLCONFIGURETRAIDOK, "CMmPacketContextMesshandlerList::GpdsLlConfigureTraIdOK, TransId Matched" );
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetPipeHandle
// Sets pipehandle specified by pipe create transaction Id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetPipeHandle(
    const TUint8 aTransactionId,
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetPipeHandle. TransId: %d", aTransactionId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPEHANDLE, "CMmPacketContextMesshandlerList::SetPipeHandle;aTransactionId=%hhu", aTransactionId );

    TInt ret( KErrNotFound );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aTransactionId == iContextInfoArray[i].iPipeCreateTransId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetPipeHandle. ContextName: %S, PipeHandle: %d", &iContextInfoArray[i].iContextName, aPipeHandle );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPEHANDLE, "CMmPacketContextMesshandlerList::SetPipeHandle;ContextName=%S;aPipeHandle=%hhu", iContextInfoArray[i].iContextName, aPipeHandle );
            iContextInfoArray[i].iPipeHandle = aPipeHandle;
            ret = KErrNone;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetPipeStatus
// Sets pipe status specified by pipe handle.
// -----------------------------------------------------------------------------
//
TBool CMmPacketContextMesshandlerList::SetPipeStatus(
    const TUint8 aPipeHandle,
    const TInt aPipeStatus )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetPipeStatus. PipeHandle: %d", aPipeHandle );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPESTATUS, "CMmPacketContextMesshandlerList::SetPipeStatus;aPipeHandle=%hhu", aPipeHandle );

    TBool ret( EFalse );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aPipeHandle == iContextInfoArray[i].iPipeHandle )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetPipeStatus. ContextName: %S, PipeStatus: %d", &iContextInfoArray[i].iContextName, aPipeStatus );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPESTATUS, "CMmPacketContextMesshandlerList::SetPipeStatus;ContextName=%S;aPipeStatus=%d", iContextInfoArray[i].iContextName, aPipeStatus );
            iContextInfoArray[i].iPipeStatus = aPipeStatus ;
            ret = ETrue;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetPipeStatus
// Gets pipe status specified by context Id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::GetPipeStatus(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetPipeStatus. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETPIPESTATUS, "CMmPacketContextMesshandlerList::GetPipeStatus;aContextId=%hhu", aContextId );

    TInt pipeStatus( PN_PIPE_DISABLE );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
            pipeStatus = iContextInfoArray[i].iPipeStatus;
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetPipeStatus. ContextName: %S, PipeStatus: %d", &iContextInfoArray[i].iContextName, pipeStatus );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETPIPESTATUS, "CMmPacketContextMesshandlerList::GetPipeStatus;ContextName=%S;pipeStatus=%d", iContextInfoArray[i].iContextName, pipeStatus );
            }
        // no else
        }

    return pipeStatus;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetContextIdByContextName
// Gets context Id specified by context name.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::GetContextIdByContextName(
    TInfoName* const aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetContextIdByContextName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTIDBYCONTEXTNAME, "CMmPacketContextMesshandlerList::GetContextIdByContextName" );

    TUint contextId( GPDS_CID_VOID );

    // Find name from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetContextIdByContextName. ContextName: %S, ContextId: %d", &iContextInfoArray[i].iContextName, iContextInfoArray[i].iContextId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTIDBYCONTEXTNAME, "CMmPacketContextMesshandlerList::GetContextIdByContextName;ContextName=%S;contextId=%d", iContextInfoArray[i].iContextName, iContextInfoArray[i].iContextId );

            // Name found, return object
            contextId = iContextInfoArray[i].iContextId;
            }
        // no else
        }

    return contextId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetHostCidName
// Gets host context name by context name.
// -----------------------------------------------------------------------------
//
TInfoName CMmPacketContextMesshandlerList::GetHostCidName(
    TInfoName* aContextName ) const
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetHostCidName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETHOSTCIDNAME, "CMmPacketContextMesshandlerList::GetHostCidName" );

    TInfoName hostName;
    hostName.Zero();

    // Find name from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetHostCidName. ContextName: %S, HostContextName: %S", &iContextInfoArray[i].iContextName, &iContextInfoArray[i].iHostContextName );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETHOSTCIDNAME, "CMmPacketContextMesshandlerList::GetHostCidName;ContextName=%S;HostContextName=%S", iContextInfoArray[i].iContextName, iContextInfoArray[i].iHostContextName );
            // Name found
            hostName.Copy( iContextInfoArray[i].iHostContextName );
            }
        // no else
        }

    return hostName;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::RemoveContextId
// Reset Context id to GPDS_CID_VOID.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::RemoveContextId(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::RemoveContextId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_REMOVECONTEXTID, "CMmPacketContextMesshandlerList::RemoveContextId;aContextId=%hhu", aContextId );

    TInt ret( KErrNotFound );

    // Find name from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::RemoveContextId. ContextName: %S, ContextId: %d", &iContextInfoArray[i].iContextName, aContextId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_REMOVECONTEXTID, "CMmPacketContextMesshandlerList::RemoveContextId;ContextName=%S;aContextId=%hhu", iContextInfoArray[i].iContextName, aContextId );
            // Name found
            iContextInfoArray[i].iContextId = GPDS_CID_VOID;
            i = KMmMaxNumberOfContexts;
            ret = KErrNone;
            }
        // no else
        }

    //if dialup context
    if( KErrNotFound == ret )
        {
        if( aContextId == iDialUpContextId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::RemoveContextId. Dial-Up Context, ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLERLIST_REMOVECONTEXTID, "CMmPacketContextMesshandlerList::RemoveContextId;Dial-Up Context, ContextId=%hhu", aContextId );
            iDialUpContextId = GPDS_CID_VOID;
            iDialUpContextName.Zero();
            ret = KErrNone;
            }
        // no else

        if( aContextId == iSecondaryDialUpContextId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::RemoveContextId. Secondary Dial-Up Context, ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLERLIST_REMOVECONTEXTID, "CMmPacketContextMesshandlerList::RemoveContextId;Secondary Dial-Up Context, ContextId=%hhu", aContextId );
            iSecondaryDialUpContextId = GPDS_CID_VOID;
            iSecondaryDialUpContextName.Zero();
            ret = KErrNone;
            }
        // no else
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ResetPipeHandleAndStatus
// Reset pipe handle and status by pipehandle.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::ResetPipeHandleAndStatus(
    const TUint8 aPipeHandle )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ResetPipeHandleAndStatus. PipeHandle: %d", aPipeHandle );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_RESETPIPEHANDLEANDSTATUS, "CMmPacketContextMesshandlerList::ResetPipeHandleAndStatus;aPipeHandle=%hhu", aPipeHandle );

    TInt ret( KErrNotFound );

    // Find name from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aPipeHandle == iContextInfoArray[i].iPipeHandle )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ResetPipeHandleAndStatus. ContextName: %S", &iContextInfoArray[i].iContextName );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_RESETPIPEHANDLEANDSTATUS, "CMmPacketContextMesshandlerList::ResetPipeHandleAndStatus;ContextName=%S", iContextInfoArray[i].iContextName );

            // Pipe found
            iContextInfoArray[i].iPipeHandle = KInvalidPipeHandle;
            iContextInfoArray[i].iPipeStatus = PN_PIPE_DISABLE;
            i = KMmMaxNumberOfContexts;
            ret = KErrNone;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetInitialiseMember
// Sets initialisation member for specific context identified by context Id.
// -----------------------------------------------------------------------------
//
TBool CMmPacketContextMesshandlerList::SetInitialiseMember(
    const TUint8 aContextId,
    const TBool aFlag )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetInitialiseMember. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETINITIALISEMEMBER, "CMmPacketContextMesshandlerList::SetInitialiseMember;aContextId=%hhu", aContextId );

    TBool ret( EFalse );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetInitialiseMember. ContextName: %S, InitialiseStarted: %d", &iContextInfoArray[i].iContextName, aFlag );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETINITIALISEMEMBER, "CMmPacketContextMesshandlerList::SetInitialiseMember;ContextName=%S;aFlag=%hhu", iContextInfoArray[i].iContextName, aFlag );
            // Return object from the context info list
            iContextInfoArray[i].iInitialiseStarted = aFlag;
            ret = ETrue;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetInitialiseMember
// Gets initialisation member for specific context identified by pipehandle.
// -----------------------------------------------------------------------------
//
TBool CMmPacketContextMesshandlerList::GetInitialiseMember(
    const TInt aPipeHandle )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetInitialiseMember. PipeHandle: %d", aPipeHandle );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETINITIALISEMEMBER, "CMmPacketContextMesshandlerList::GetInitialiseMember;aPipeHandle=%d", aPipeHandle );

    TBool ret( EFalse );

    // Find name from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aPipeHandle == iContextInfoArray[i].iPipeHandle )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetInitialiseMember. ContextName: %S, InitialiseStarted: %d", &iContextInfoArray[i].iContextName, iContextInfoArray[i].iInitialiseStarted );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETINITIALISEMEMBER, "CMmPacketContextMesshandlerList::GetInitialiseMember;ContextName=%S;InitialiseStarted=%hhu", iContextInfoArray[i].iContextName, iContextInfoArray[i].iInitialiseStarted );
            ret = iContextInfoArray[i].iInitialiseStarted;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ContextIdByPipeHandle
// Gets contextId of the specified context by Pipehandle.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::ContextIdByPipeHandle(
    const TInt aPipeHandle )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ContextIdByPipeHandle. PipeHandle: %d", aPipeHandle );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CONTEXTIDBYPIPEHANDLE, "CMmPacketContextMesshandlerList::ContextIdByPipeHandle;aPipeHandle=%d", aPipeHandle );

    TUint8 contextId( GPDS_CID_VOID );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aPipeHandle == iContextInfoArray[i].iPipeHandle )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ContextIdByPipeHandle. ContextId: %d", iContextInfoArray[i].iContextId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CONTEXTIDBYPIPEHANDLE, "CMmPacketContextMesshandlerList::ContextIdByPipeHandle;ContextId=%hhu", iContextInfoArray[i].iContextId );

            // Return object from the context info list
            contextId = iContextInfoArray[i].iContextId;
            }
        // no else
        }

    // Valid values to return are, 0x00, 0x01, 0xFF = GPDS_CID_VOID
    return contextId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetContextConfigurationType
// Sets configuration type by contextId.
// -----------------------------------------------------------------------------
//
TBool CMmPacketContextMesshandlerList::SetContextConfigurationType(
    const TUint8 aContextId,
    const TInt aConfigType )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetContextConfigurationType. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETCONTEXTCONFIGURATIONTYPE, "CMmPacketContextMesshandlerList::SetContextConfigurationType;aContextId=%hhu", aContextId );

    TBool ret( EFalse );

     // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetContextConfigurationType. ContextName: %S, ConfigType: %d", &iContextInfoArray[i].iContextName, aConfigType );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETCONTEXTCONFIGURATIONTYPE, "CMmPacketContextMesshandlerList::SetContextConfigurationType;ContextName=%S;aConfigType=%d", iContextInfoArray[i].iContextName, aConfigType );

            // Return object from the context info list
            iContextInfoArray[i].iContextConfigurationType = aConfigType;
            ret = ETrue;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetContextConfigurationType
// Gets configuration type by contextId.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::GetContextConfigurationType(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetContextConfigurationType. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTCONFIGURATIONTYPE, "CMmPacketContextMesshandlerList::GetContextConfigurationType;aContextId=%hhu", aContextId );

    TInt ret( KTIntNotDefined );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
            // Return object from the context info list
            ret = iContextInfoArray[i].iContextConfigurationType;
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetContextConfigurationType. ContextName: %S, ConfigType: %d", &iContextInfoArray[i].iContextName, ret );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTCONFIGURATIONTYPE, "CMmPacketContextMesshandlerList::GetContextConfigurationType;ContextName=%S;ret=%d", iContextInfoArray[i].iContextName, ret );
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SaveConfig
// Saves configuration for specific context.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SaveConfig(
    const TUint8 aContextId,
    const TInt aConfigurationType,
    RPacketContext::TContextConfigGPRS* aConfig,
    RPacketContext::TContextConfigR99_R4* aConfig99 )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::SaveConfig");
OstTrace0( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLERLIST_SAVECONFIG, "CMmPacketContextMesshandlerList::SaveConfig" );

    TInt ret( KErrArgument );

     // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SaveConfig. ContextName: %S, ContextId: %d", &iContextInfoArray[i].iContextName, aContextId );
OstTraceExt2( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SAVECONFIG, "CMmPacketContextMesshandlerList::SaveConfig;ContextName=%S;aContextId=%hhu", iContextInfoArray[i].iContextName, aContextId );
            // Return object from the context info list
            if( TPacketDataConfigBase::KConfigGPRS == aConfigurationType )
                {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SaveConfig. ConfigType: %d", aConfigurationType );
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SAVECONFIG, "CMmPacketContextMesshandlerList::SaveConfig;aConfigurationType=%d", aConfigurationType );
                iContextInfoArray[i].iConfig = *aConfig;
                iContextInfoArray[i].iContextConfigurationType =
                    aConfigurationType;
                ret = KErrNone;
                }

            else if( TPacketDataConfigBase::KConfigRel99Rel4 ==
                aConfigurationType )
                {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SaveConfig. ConfigType: %d", aConfigurationType );
OstTrace1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLERLIST_SAVECONFIG, "CMmPacketContextMesshandlerList::SaveConfig;aConfigurationType=%d", aConfigurationType );
                iContextInfoArray[i].iConfig99 =
                    *aConfig99;
                iContextInfoArray[i].iContextConfigurationType =
                    aConfigurationType;
                ret = KErrNone;
                }
            // no else

            i = KMmMaxNumberOfContexts;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SaveConfig
// Get configuration for specific context by context Id and configuration type.
// -----------------------------------------------------------------------------
//
TPacketDataConfigBase* CMmPacketContextMesshandlerList::GetConfig(
    const TUint8 aContextId,
    const TInt aConfigurationType )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetConfig");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETCONFIG, "CMmPacketContextMesshandlerList::GetConfig" );

    TPacketDataConfigBase* config;
    config = NULL;

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetConfig. ContextName: %S, ContextId: %d", &iContextInfoArray[i].iContextName, aContextId );
OstTraceExt2( TRACE_NORMAL, DUP3_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONFIG, "CMmPacketContextMesshandlerList::GetConfig;ContextName=%S;aContextId=%hhu", iContextInfoArray[i].iContextName, aContextId );
            // Return object from the context info list
            if( TPacketDataConfigBase::KConfigGPRS == aConfigurationType )
                {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetConfig. ConfigType: %d", aConfigurationType );
OstTrace1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONFIG, "CMmPacketContextMesshandlerList::GetConfig;aConfigurationType=%d", aConfigurationType );
                // Return object from the context info list
                config = &( iContextInfoArray[i].iConfig );
                }
            else if( TPacketDataConfigBase::KConfigRel99Rel4 ==
                aConfigurationType )
                {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetConfig. ConfigType: %d", aConfigurationType );
OstTrace1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONFIG, "CMmPacketContextMesshandlerList::GetConfig;aConfigurationType=%d", aConfigurationType );
                // Return object from the context info list
                config = &( iContextInfoArray[i].iConfig99 );
                }
            // no else
            }
        // no else
        }

    return config;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetPipeResetTraId
// Sets pipe reset transaction id for specific context by context Id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetPipeResetTraId(
    const TUint8 aContextId,
    const TUint8 aTransId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetPipeResetTraId. ContextId: %d",  aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPERESETTRAID, "CMmPacketContextMesshandlerList::SetPipeResetTraId;aContextId=%hhu", aContextId );

    TInt ret( KErrNotFound );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::SetPipeResetTraId. ContextName: %S, PipeResetTraId: %d", &iContextInfoArray[i].iContextName, aTransId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETPIPERESETTRAID, "CMmPacketContextMesshandlerList::SetPipeResetTraId;ContextName=%S;aTransId=%hhu", iContextInfoArray[i].iContextName, aTransId );
            // Return object from the context info list
            iContextInfoArray[i].iPipeResetTraId = aTransId;
            i = KMmMaxNumberOfContexts;
            ret = KErrNone;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::CheckAndResetPipeResetTraId
// Checks and resets pipe reset transaction id early saved.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::CheckAndResetPipeResetTraId(
    const TUint8 aTransId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::CheckAndResetPipeResetTraId. TransId: %d", aTransId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CHECKANDRESETPIPERESETTRAID, "CMmPacketContextMesshandlerList::CheckAndResetPipeResetTraId;aTransId=%hhu", aTransId );

    TInt ret( KErrNotFound );

    // Find context pipehandle from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aTransId == iContextInfoArray[i].iPipeResetTraId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::CheckAndResetPipeResetTraId. ContextName: %S", &iContextInfoArray[i].iContextName );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CHECKANDRESETPIPERESETTRAID, "CMmPacketContextMesshandlerList::CheckAndResetPipeResetTraId;ContextName=%S", iContextInfoArray[i].iContextName );
            ret = KErrNone;
            i = KMmMaxNumberOfContexts;
            iContextInfoArray[i].iPipeResetTraId = KTUint8NotDefined;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ProxyIdByContextId
// Gets proxy Id specified by context Id.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::ProxyIdByContextId(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ProxyIdByContextId. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_PROXYIDBYCONTEXTID, "CMmPacketContextMesshandlerList::ProxyIdByContextId;aContextId=%hhu", aContextId );

    TUint channelId( KTUint8NotDefined );

    // Find context proxyId from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::ProxyIdByContextId. ContextName: %S, ChannelId: %d", &iContextInfoArray[i].iContextName, iContextInfoArray[i].iChannelId );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_PROXYIDBYCONTEXTID, "CMmPacketContextMesshandlerList::ProxyIdByContextId;ContextName=%S;ChannelId=%hhu", iContextInfoArray[i].iContextName, iContextInfoArray[i].iChannelId );
            channelId = iContextInfoArray[i].iChannelId;
            }
        // no else
        }

    // Valid values to return are, 0x00, 0x01, 0xFF = Not defined
    return channelId;

    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ProxyIdByContextId
// Sets context Id and name for dial-up context.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMesshandlerList::SetDialUpContextId(
    const TUint8 aContextId )
    {

    if( 0 == iDialUpContextName.Compare( KStringExternal ) )
        {

        TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::SetDialUpContextId iSecondaryDialUpContextName.");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETDIALUPCONTEXTID, "CMmPacketContextMesshandlerList::SetDialUpContextId, iSecondaryDialUpContextName" );

        iSecondaryDialUpContextName = KStringExternal2;
        iSecondaryDialUpContextId = aContextId;
        }
    else
        {

        TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::SetDialUpContextId iDialUpContextId.");
OstTrace0( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETDIALUPCONTEXTID, "CMmPacketContextMesshandlerList::SetDialUpContextId, iDialUpContextId" );

        iDialUpContextId = aContextId;
        iDialUpContextName = KStringExternal;
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetDialUpContextId
// Gets context Id for dial-up context.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::GetDialUpContextId()
    {

TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetDialUpContextId");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETDIALUPCONTEXTID, "CMmPacketContextMesshandlerList::GetDialUpContextId" );

    return iDialUpContextId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::DialUpContextName
// Gets dial-up context name specified by context Id.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::DialUpContextName(
    const TUint8 aContextId,
    TInfoName& aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::DialUpContextName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_DIALUPCONTEXTNAME, "CMmPacketContextMesshandlerList::DialUpContextName" );

    TInt ret( KErrNotFound );

    if( aContextId == iDialUpContextId )
        {

        TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::DialUpContextName. DialUpContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_DIALUPCONTEXTNAME, "CMmPacketContextMesshandlerList::DialUpContextName;DialUpContextId=%hhu", aContextId );
        aContextName = iDialUpContextName;
        ret = KErrNone;
        }
    // no else

    if( aContextId == iSecondaryDialUpContextId )
        {

        TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::DialUpContextName. SecondaryDialUpContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLERLIST_DIALUPCONTEXTNAME, "CMmPacketContextMesshandlerList::DialUpContextName;DialUpContextName=%hhu", aContextId );
        aContextName = iSecondaryDialUpContextName;
        ret = KErrNone;
        }
    // no else

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetDialUpContextIdByName
// Gets dial-up context id specified by context name.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::GetDialUpContextIdByName(
    TInfoName* const aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetDialUpContextIdByName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETDIALUPCONTEXTIDBYNAME, "CMmPacketContextMesshandlerList::GetDialUpContextIdByName" );

    TUint8 contextId( GPDS_CID_VOID );

    if ( *aContextName == iDialUpContextName )
        {

        TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetDialUpContextIdByName. DialUpContextId: %d", iDialUpContextId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETDIALUPCONTEXTIDBYNAME, "CMmPacketContextMesshandlerList::GetDialUpContextIdByName;DialUpContextId=%hhu", contextId );
        // Name found, return object
        contextId = iDialUpContextId;
        }
    // no else

    if ( *aContextName == iSecondaryDialUpContextName )
        {
        // Name found, return object
        contextId = iSecondaryDialUpContextId;

        TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetDialUpContextIdByName. SecondaryDialUpContextId: %d", contextId );

OstTraceExt1( TRACE_NORMAL, DUP2_CMMPACKETCONTEXTMESSHANDLERLIST_GETDIALUPCONTEXTIDBYNAME, "CMmPacketContextMesshandlerList::GetDialUpContextIdByName;SecondaryDialUpContextId=%hhu", contextId );
        }
    // no else

    return contextId;

    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::RemoveObjectByName
// Removes buffered data belonging to a context specified by name.
// -----------------------------------------------------------------------------
//
void CMmPacketContextMesshandlerList::RemoveObjectByName(
    TInfoName* aContextName )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::RemoveObjectByName");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_REMOVEOBJECTBYNAME, "CMmPacketContextMesshandlerList::RemoveObjectByName" );

    // Set ret to KErrArgument to check that context is in list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find aContext from context info list
        if ( *aContextName == iContextInfoArray[i].iContextName )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::RemoveObjectByName. Context Name: %S", &iContextInfoArray[i].iContextName );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_REMOVEOBJECTBYNAME, "CMmPacketContextMesshandlerList::RemoveObjectByName;ContextName=%S", iContextInfoArray[i].iContextName );

            // Reset object information
            InitializeContextInfoList( i );
            // Decrease number of created contexts
            iNumberOfCreatedContexts--;
            i = KMmMaxNumberOfContexts;
            }
        // no else
        }
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetDNSAddressInfoToContextInfo
// Adds DNS related information to context info array.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetDNSAddressInfoToContextInfo(
    TUint8 aContextId )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::SetDNSAddressInfoToContextInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETDNSADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::SetDNSAddressInfoToContextInfo" );

    TInt ret( KErrNotFound );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find aContext from context info list
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetDNSAddressInfoToContextInfo. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETDNSADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::SetDNSAddressInfoToContextInfo;aContextId=%hhu", aContextId );
            iContextInfoArray[i].iAddDNSAddress = ETrue;
            ret = KErrNone;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetDNSAddressInfoToContextInfo
// Gets DNS related information from context info array.
// -----------------------------------------------------------------------------
//
TBool CMmPacketContextMesshandlerList::GetDNSAddressInfoToContextInfo(
    TUint8 aContextId )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetDNSAddressInfoToContextInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETDNSADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::GetDNSAddressInfoToContextInfo" );

    TBool addDNSAddress( EFalse );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find aContext from context info list
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetDNSAddressInfoToContextInfo. ContextId: %d, AddDNSAddress: %d", aContextId, iContextInfoArray[i].iAddDNSAddress );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETDNSADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::GetDNSAddressInfoToContextInfo;aContextId=%hhu;AddDNSAddress=%hhu", aContextId, iContextInfoArray[i].iAddDNSAddress );
            addDNSAddress = iContextInfoArray[i].iAddDNSAddress;
            }
        // no else
        }

    return addDNSAddress;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::SetPCSCFAddressInfoToContextInfo
// Adds PCSCF related information to context info array.
// -----------------------------------------------------------------------------
//
TInt CMmPacketContextMesshandlerList::SetPCSCFAddressInfoToContextInfo(
    TUint8 aContextId )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::SetPCSCFAddressInfoToContextInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_SETPCSCFADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::SetPCSCFAddressInfoToContextInfo" );

    TInt ret( KErrNotFound );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find aContext from context info list
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::SetPCSCFAddressInfoToContextInfo. ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_SETPCSCFADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::SetPCSCFAddressInfoToContextInfo;aContextId=%hhu", aContextId );
            iContextInfoArray[i].iAddPCSCFAddress = ETrue;
            ret = KErrNone;
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetPCSCFAddressInfoToContextInfo
// Gets PCSCF related information from context info array.
// -----------------------------------------------------------------------------
//
TBool CMmPacketContextMesshandlerList::GetPCSCFAddressInfoToContextInfo(
    TUint8 aContextId )
    {

    TFLOGSTRING("TSY: CMmPacketContextMesshandlerList::GetPCSCFAddressInfoToContextInfo");
OstTrace0( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETPCSCFADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::GetPCSCFAddressInfoToContextInfo" );

    TBool addPCSCFAddress( EFalse );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        // Find aContext from context info list
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
TFLOGSTRING3("TSY: CMmPacketContextMesshandlerList::GetPCSCFAddressInfoToContextInfo. ContextId: %d, AddPCSCFAddress: %d", aContextId, iContextInfoArray[i].iAddPCSCFAddress );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETPCSCFADDRESSINFOTOCONTEXTINFO, "CMmPacketContextMesshandlerList::GetPCSCFAddressInfoToContextInfo;aContextId=%hhu;AddPCSCFAddress=%hhu", aContextId, iContextInfoArray[i].iAddPCSCFAddress );
            addPCSCFAddress = iContextInfoArray[i].iAddPCSCFAddress;
            }
        // no else
        }

    return addPCSCFAddress;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::ContextIdByListIndex
// Gets context id of the specified context by list index.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::ContextIdByListIndex(
    const TInt aListIndex )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ContextIdByListIndex, Index: %d", aListIndex );
OstTrace1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_CONTEXTIDBYLISTINDEX, "CMmPacketContextMesshandlerList::ContextIdByListIndex;aListIndex=%d", aListIndex );

    TUint8 contextId( GPDS_CID_VOID );

    if ( aListIndex < KMmMaxNumberOfContexts )
        {
        // Return context id from the context info list
        contextId = iContextInfoArray[aListIndex].iContextId;
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::ContextIdByListIndex. ContextId: %d", contextId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_CONTEXTIDBYLISTINDEX, "CMmPacketContextMesshandlerList::ContextIdByListIndex;contextId=%hhu", contextId );
        }
    // no else

    return contextId;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::GetContextTypeById
// This function returns context type by context id.
// -----------------------------------------------------------------------------
//
TUint8 CMmPacketContextMesshandlerList::GetContextTypeById(
    const TUint8 aContextId )
    {

    TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetContextTypeByName, ContextId: %d", aContextId );
OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTTYPEBYID, "CMmPacketContextMesshandlerList::GetContextTypeById;aContextId=%hhu", aContextId );

    TUint8 ret( GPDS_CONT_TYPE_NORMAL );

    // Find context id from context info list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( aContextId == iContextInfoArray[i].iContextId )
            {
            // if host context is defined => context is secondary context
            if( 0 != iContextInfoArray[i].iHostContextName.Length() )
                {
                 ret = GPDS_CONT_TYPE_SEC;
                }
            // no else
TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::GetContextTypeById, ContextType: %d", ret );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTTYPEBYID, "CMmPacketContextMesshandlerList::GetContextTypeById;ContextType=%hhu", ret );
            }
        // no else
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmPacketContextMesshandlerList::InitializeContextInfoList
// This function initializes context info
// -----------------------------------------------------------------------------
//
void CMmPacketContextMesshandlerList::InitializeContextInfoList(
    const TUint8 aContextId )
    {

TFLOGSTRING2("TSY: CMmPacketContextMesshandlerList::InitializeContextInfoList, ContextId: %d", aContextId );
//OstTraceExt1( TRACE_NORMAL, CMMPACKETCONTEXTMESSHANDLERLIST_GETCONTEXTTYPEBYID, "CMmPacketContextMesshandlerList::GetContextTypeById;aContextId=%hhu", aContextId );


    iContextInfoArray[aContextId].iContextId = GPDS_CID_VOID;

    iContextInfoArray[aContextId].iChannelId = KDefinedProxyIds[aContextId];

    iContextInfoArray[aContextId].iContextName.Zero();
    iContextInfoArray[aContextId].iHostContextName.Zero();
    iContextInfoArray[aContextId].iPipeStatus = PN_PIPE_DISABLE;
    iContextInfoArray[aContextId].iPipeHandle = KInvalidPipeHandle;
    iContextInfoArray[aContextId].iCreateContextIdTransId = KTUint8NotDefined;
    iContextInfoArray[aContextId].iPipeCreateTransId = KTUint8NotDefined;
    iContextInfoArray[aContextId].iContextType = GPDS_CONT_TYPE_NORMAL ;
    iContextInfoArray[aContextId].iInitialiseStarted = EFalse;
    iContextInfoArray[aContextId].iContextConfigurationType = KTIntNotDefined;
    iContextInfoArray[aContextId].iActivationTraId = KTUint8NotDefined;
    iContextInfoArray[aContextId].iPipeResetTraId = KTUint8NotDefined;
    iContextInfoArray[aContextId].iDeleteTraId = KTUint8NotDefined;
    iContextInfoArray[aContextId].iChannelOpened = EFalse;

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

    //None


//  End of File
