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



//  INCLUDE FILES
#include "cmmmessagerouter.h"
// Temporarily removed for Bridge camp!
//#include "satmessaging.h"
#include "licenseetsy.h"

#include "tsylogger.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "licenseetsytraces.h"
#endif


// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES  
    //None

// CONSTANTS
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// ==================== LOCAL FUNCTIONS =====================================

    //None


// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CNokiaTSYFactory::CNokiaTSYFactory
// Constructor
// -----------------------------------------------------------------------------
//
CNokiaTSYFactory::CNokiaTSYFactory
        (
        // none
        )
    {
    // none
TFLOGSTRING("tsyfactory: CNokiaTSYFactory::CNokiaTSYFactory");
OstTrace0( TRACE_NORMAL, CNOKIATSYFACTORY_CNOKIATSYFACTORY, "CNokiaTSYFactory::CNokiaTSYFactory" );

    iSatMessaging = NULL;
    iMMessageRouter = NULL;
    }

// -----------------------------------------------------------------------------
// CNokiaTSYFactory::~CNokiaTSYFactory
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CNokiaTSYFactory::~CNokiaTSYFactory()
    {
        
TFLOGSTRING("tsyfactory: CNokiaTSYFactory::~CNokiaTSYFactory");
OstTrace0( TRACE_NORMAL, DUP1_CNOKIATSYFACTORY_CNOKIATSYFACTORY, "CNokiaTSYFactory::~CNokiaTSYFactory" );

    delete iMMessageRouter;
    iMMessageRouter = NULL;

    }

// -----------------------------------------------------------------------------
// MLtsyFactoryBase::TCtsyInterfaceVersion CNokiaTSYFactory::Version()
// Returns CTSY interface version.
// -----------------------------------------------------------------------------
//
MLtsyFactoryBase::TCtsyInterfaceVersion CNokiaTSYFactory::Version()
    {
        
TFLOGSTRING("tsyfactory: MLtsyFactoryBase::TCtsyInterfaceVersion");
OstTrace0( TRACE_NORMAL, CNOKIATSYFACTORY_VERSION, "CNokiaTSYFactory::Version" );
    
    return EIfVersion1;
    }

// -----------------------------------------------------------------------------
// Release
// Calls destructor
// -----------------------------------------------------------------------------
//

void CNokiaTSYFactory::Release()
    {
        
TFLOGSTRING("tsyfactory: CNokiaTSYFactory::Release");
OstTrace0( TRACE_NORMAL, CNOKIATSYFACTORY_RELEASE, "CNokiaTSYFactory::Release" );

    // Call destructor
    delete ( this );
    }

// -----------------------------------------------------------------------------
// MMessageRouter* CNokiaTSYFactory::GetMessageRouter
// Calls CMmMessageRouter::NewL
// -----------------------------------------------------------------------------
//

MMessageRouter* CNokiaTSYFactory::GetMessageRouter
    (
    MmMessageManagerCallback& aMessageManager
    )
    {
        
TFLOGSTRING("tsyfactory: CNokiaTSYFactory::GetMessageRouter");
OstTrace0( TRACE_NORMAL, CNOKIATSYFACTORY_GETMESSAGEROUTER, "CNokiaTSYFactory::GetMessageRouter" );

    if ( NULL == iMMessageRouter )
        {
        iMMessageRouter = CMmMessageRouter::NewL( &aMessageManager );
        }
        
    return iMMessageRouter;
    }

// -----------------------------------------------------------------------------
// CTsySatMessagingBase* CNokiaTSYFactory::GetSatMessagingService
// CTsySatMessaging::NewL
// -----------------------------------------------------------------------------
//

CTsySatMessagingBase* CNokiaTSYFactory::GetSatMessagingService( )
    {
        
TFLOGSTRING("tsyfactory: CNokiaTSYFactory::GetSatMessagingService");
OstTrace0( TRACE_NORMAL, CNOKIATSYFACTORY_GETSATMESSAGINGSERVICE, "CNokiaTSYFactory::GetSatMessagingService" );

    if ( NULL == iSatMessaging )
        {
		// Temporarily removed for Bridge camp!
        //iSatMessaging = CTsySatMessaging::NewL( iMMessageRouter );
        //iMMessageRouter->iSatMessaging = iSatMessaging;
        }


    return iSatMessaging;
    }

// -----------------------------------------------------------------------------
// CTsySatMessagingBase* CNokiaTSYFactory::GetSatMessagingService
// CTsySatMessaging::NewL
// -----------------------------------------------------------------------------
//

CTsySatMessagingBase* CNokiaTSYFactory::GetSatMessagingService
    (
    MCtsySatService& /* aSatMessagingService */
    )
    {
        
TFLOGSTRING("tsyfactory: CNokiaTSYFactory::GetSatMessagingService");
OstTrace0( TRACE_NORMAL, DUP1_CNOKIATSYFACTORY_GETSATMESSAGINGSERVICE, "CNokiaTSYFactory::GetSatMessagingService" );


    if ( NULL == iSatMessaging )
        {
		// Temporarily removed for Bridge camp!
        // iSatMessaging = CTsySatMessaging::NewL( iMMessageRouter );
        // iMMessageRouter->iSatMessaging = iSatMessaging;
        }


    return iSatMessaging;
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MLtsyFactoryBase* LTsyFactoryL
// Creates new CNokiaTSYFactory object and returns it.
// -----------------------------------------------------------------------------
//

EXPORT_C MLtsyFactoryBase* LTsyFactoryL
        (
        )
    {
    
TFLOGSTRING("tsyfactory: LtsyFactoryL / Created new CNokiaTSYFactory object");
OstTrace0( TRACE_NORMAL, _LTSYFACTORYL, "::LtsyFactoryL" );

    MLtsyFactoryBase* ltsyFactory = NULL;

    ltsyFactory = new ( ELeave ) CNokiaTSYFactory();

    return ltsyFactory;
    }

//  End of File 

