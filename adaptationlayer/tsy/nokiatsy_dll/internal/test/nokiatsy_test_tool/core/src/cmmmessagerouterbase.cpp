/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Routes between Tsy objects and Message Handler objects
*
*/



//  INCLUDE FILES
#include "cmmmessagerouterbase.h"
#include "ctsy/pluginapi/cmmdatapackage.h"

#include "core.h"

#include "tflogger.h"
#include "ctsy/serviceapi/mmtsy_ipcdefs.h"



// ================= MEMBER FUNCTIONS =======================================



// -----------------------------------------------------------------------------
// CMmMessageRouterBase::~CMmMessageRouterBase
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMmMessageRouterBase::~CMmMessageRouterBase
        (
        // none
        )
    {
    }


// -----------------------------------------------------------------------------
// CMmMessageRouterBase::RouteRequest
// Returns pointer to the message handler for this request, based on IPC number
// -----------------------------------------------------------------------------
//
EXPORT_C MMmMessHandlerBase* CMmMessageRouterBase::RouteRequest
    (
    TInt /*aIpc*/
    )
    {
    // dummy implementation
    return NULL;
    }


// -----------------------------------------------------------------------------
// CMmMessageRouterBase::SetCore
// Sets the pointer to CCore
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMessageRouterBase::SetCore
	(
	CCore* aCore
	)
	{
	iCore = aCore;
	}

// -----------------------------------------------------------------------------
// CMmMessageRouterBase::RegisterTsyObject
// Registers a Tsy object for request and notification completions
// -----------------------------------------------------------------------------
//
void CMmMessageRouterBase::RegisterTsyObject
        (
		CMmMessageManagerBase::TTsyObjects /*aTsyObjectType*/,
        CBase* /*aTsyObject*/
        )
    {


    }


// -----------------------------------------------------------------------------
// CMmMessageRouterBase::DeregisterTsyObject
// Registers a Tsy object for request and notification completions
// -----------------------------------------------------------------------------
//
void CMmMessageRouterBase::DeregisterTsyObject
        (
        CBase* /*aTsyObject*/
        )
    {
    }

// -----------------------------------------------------------------------------
// CMmMessageRouterBase::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMessageRouterBase::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,
        TInt aResult
        )
    {
    iCore->RouteCompletion(aIpc, aDataPackage, aResult);      
    return;
    }

// -----------------------------------------------------------------------------
// CMmMessageRouterBase::GetObjectForCompletion
// Returns the pointer to the Tsy object needed for completion, based
// on the IPC number and the data package
// -----------------------------------------------------------------------------
//
CBase* CMmMessageRouterBase::GetObjectForCompletion
        (
        const TInt /*aIpc*/,
        CMmDataPackage* /*aDataPackage*/
        )
    {
    // dummy implementation
    CBase* mmObject = NULL;
    return mmObject;

    }


// -----------------------------------------------------------------------------
// CMmMessageRouterBase::GetTsyObjectFromIpc
// Returns the type of Tsy object needed for completion, based
// on the IPC number
// -----------------------------------------------------------------------------
//
CMmMessageManagerBase::TTsyObjects CMmMessageRouterBase::GetTsyObjectFromIpc
        (
        const TInt /*aIpc*/
        ) const

    {
    // dummy implementation
    CMmMessageManagerBase::TTsyObjects retObject ( CMmMessageManagerBase::EPhoneTsy );

    return retObject;
    }
    

// ================= OTHER EXPORTED FUNCTIONS ===============================

    //None


//  End of File

