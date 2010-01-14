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



#ifndef _MUICCOPERATIONBASE_
#define _MUICCOPERATIONBASE_

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  This class is the base class for all message handlers in NokiaTSY that
*  need to handle UICC messages. This interface (ProcessUiccMsg -function) must
*  be implemented in all message handlers.
*/

class MUiccOperationBase
    {
    public:

        virtual TInt ProcessUiccMsg(
            TInt aTraId,
            TInt aStatus,
            TUint8 aDetails,
            const TDesC8& aFileData ) = 0;

    };

#endif      // _MUICCOPERATIONBASE_

// End of File
