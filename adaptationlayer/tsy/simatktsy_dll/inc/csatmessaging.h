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



#ifndef SATMESSAGING_H
#define SATMESSAGING_H

//  INCLUDES

// SatMessagingBase class
#include <ctsy/serviceapi/ctsysatmessagingbase.h>
// CMessageRouter class of NokiaTSY component.
#include "cmmmessagerouter.h"


// FORWARD DECLARATIONS
class CMmPhoneTsy;


// CLASS DECLARATION
/**
*  Sat notifications base class.
*  Sat Messaging class.
*  Constructor for SAT messaging
*
*  @lib SIMATKTSY.LIB
*  @since Series 60 Series60_3.0
*/
class CTsySatMessaging : public CTsySatMessagingBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aMmPhone pointer to mmphone class
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aName Sat application fixed name
        */
        IMPORT_C static CTsySatMessaging* NewL( CMmPhoneTsy* aMmPhone,
            CMmMessageRouterBase* aRouter, TName aName );

    };

#endif  // SATMESSAGING_H

// End of File
