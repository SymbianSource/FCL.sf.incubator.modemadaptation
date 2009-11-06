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


#ifndef MMMMESSHANDLERBASE_H
#define MMMMESSHANDLERBASE_H

//  INCLUDES
#include <e32base.h>
#include <et_tsy.h>

// FORWARD DECLARATIONS

/**
*  This class is the base class for all message handlers in NokiaTSY.
*  It defines an interface for passing client requests to NTSY's message
*  handlers. This interface (ExtFuncL -function) must be implemented in
*  all message handlers.
*/

class CMmDataPackage;

// CLASS DECLARATION

class MMmMessHandlerBase
    {
    public:

        /**
        * ExtFuncL
        *
        * @param aDataPackage
        * @return Error value
        */
        virtual TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage ) = 0;
};

#endif // MMMMESSHANDLERBASE_H

//  End of file
