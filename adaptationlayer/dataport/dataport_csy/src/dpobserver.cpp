/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "dpdef.h"                   // dataport definitions
#include    "dpobserver.h"              // abstract observer class
#include    "dplog.h"                   // dataport logging
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpobservertraces.h"
#endif

// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES
// none

// CONSTANTS
const TInt CDpObserver::iOffset = _FOFF(CDpObserver,iSlink);

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================

// none


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CDpObserver::CDpObserver
// C++ default constructor.
// ---------------------------------------------------------
CDpObserver::CDpObserver()
    {
    OstTrace0( TRACE_NORMAL, CDPOBSERVER_CDPOBSERVER, "CDpObserver::CDpObserver" );
    }

// ---------------------------------------------------------
// CDpObserver::~CDpObserver
// Destructor
// ---------------------------------------------------------
CDpObserver::~CDpObserver()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPOBSERVER_CDPOBSERVER, "CDpObserver::~CDpObserver" );
    iSlink.iNext = NULL;
    }

//=============================================================================


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CDpSubject::CDpSubject
// C++ default constructor.
// ---------------------------------------------------------
CDpSubject::CDpSubject() :
    iObserverList( CDpObserver::iOffset ),
    iObserverIter( iObserverList )
    {
    OstTrace0( TRACE_NORMAL, CDPSUBJECT_CDPSUBJECT, "CDpSubject::CDpSubject" );
    LOGM(" CDpSubject::CDpSubject");
    }

// ---------------------------------------------------------
// CDpSubject::~CDpSubject
// Destructor
// ---------------------------------------------------------
CDpSubject::~CDpSubject()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPSUBJECT_CDPSUBJECT, "CDpSubject::~CDpSubject" );
    LOGM(" CDpSubject::~CDpSubject");

    CDpObserver* item= NULL;

    // set observer to first item in the list
    iObserverIter.SetToFirst();

    while( NULL != (item = iObserverIter++) )
        {
        iObserverList.Remove( *item );
        }
    }

// ---------------------------------------------------------
// CDpSubject::Attach
// Attach observer to this subject
// ---------------------------------------------------------
//
void CDpSubject::Attach(
    CDpObserver& aObserver)
    {
    OstTrace0( TRACE_NORMAL, CDPSUBJECT_ATTACH, "CDpSubject::Attach" );
    LOGM(" CDpSubject::Attach");
    iObserverList.AddLast( aObserver );
    }

// ---------------------------------------------------------
// CDpSubject::Detach
// Detach observer from this subject
// ---------------------------------------------------------
//
void CDpSubject::Detach(
    CDpObserver& aObserver)
    {
    OstTrace0( TRACE_NORMAL, CDPSUBJECT_DETACH, "CDpSubject::Detach" );
    LOGM(" CDpSubject::Detach");

    iObserverList.Remove( aObserver );
    }

// ---------------------------------------------------------
// CDpSubject::Notify
// Notify all observers of this subject object
// i.e. call UpDate() in every observer object.
// ---------------------------------------------------------
//
void CDpSubject::Notify()
    {
    OstTrace0( TRACE_NORMAL, CDPSUBJECT_NOTIFY, "CDpSubject::Notify" );
    LOGM(" CDpSubject::Notify");

    CDpObserver* item = NULL;
    iObserverIter.SetToFirst();
    while( NULL != (item = iObserverIter++) )
        {
        item->UpDate( this );
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
// none

//  End of File
