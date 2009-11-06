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


 
#ifndef BCATOISCFACTORY_H
#define BCATOISCFACTORY_H

#include <networking/bcafactory.h>
#include <e32base.h>

/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation
{
	
    /**
	  * The interface to the factory responsible for CBcaToIsc creation.
	  **/
    NONSHARABLE_CLASS(CBcaToIscFactory): public CBase, public MBcaFactory
    {
    
    public:
    
    virtual void Release();
    virtual MBca* NewBcaL();
    
    };

}
#endif //BCATOISCFACTORY_H
