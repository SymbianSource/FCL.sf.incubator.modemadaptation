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
#include "bcatoisctrace.h"

using namespace BasebandChannelAdaptation;
	

extern "C"
    {
    IMPORT_C MBcaFactory* NewBcaToIscFactoryL();	// Force export
    }

/**
 * Creates a new BCA factory. This must be the first exported function
 * in the BCA.DLL.
 *
 * @return a newly created CBcaToIscFactory.
 */
EXPORT_C MBcaFactory* NewBcaToIscFactoryL()
    {
    C_TRACE( (_T("NewBcaToIscFactoryL() ->") ) );
    
    C_TRACE( (_T("NewBcaToIscFactoryL() <-") ) );
    
    return new (ELeave) CBcaToIscFactory;
    }
	
// end of BcaToIscDllMain.cpp
