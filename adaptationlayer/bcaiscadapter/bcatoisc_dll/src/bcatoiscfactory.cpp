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
* Description: 
*
*/



#include "bcatoiscfactory.h"
#include "bcatoisc.h"
#include "bcatoisctrace.h"
 
using namespace BasebandChannelAdaptation;

 
/** Deletes the factory */
void CBcaToIscFactory::Release()
    {
    C_TRACE( (_T("CBcaToIscFactory::Release() ->") ) );

    delete this;

    C_TRACE( (_T("CBcaToIscFactory::Release() <-") ) );
    }
 
/**
Factory method: creates a BcaToIsc instance and returns a pointer to it.

@return a pointer to BCA instance.
@leave if BCA to ISC instance could not be constructed.
*/
MBca* CBcaToIscFactory::NewBcaL()
    {
    C_TRACE( (_T("CBcaToIscFactory::NewBcaL() ->") ) );
		
    CBcaToIsc* instance = new(ELeave) CBcaToIsc;
    CleanupStack::PushL(instance);
    instance->ConstructL();
    CleanupStack::Pop(instance);
	
    C_TRACE( (_T("CBcaToIscFactory::NewBcaL() <-") ) );
	
    return instance;
    }



