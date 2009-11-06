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
* Description:  Header file for kisitester kernel extension handler
*
*/


#ifndef __KISITESTER_HANDLER_H
#define __KISITESTER_HANDLER_H


//- Include Files  ----------------------------------------------------------

#include <kernel/kernel.h>
//- Namespace ---------------------------------------------------------------


//- Using -------------------------------------------------------------------


//- Data Types --------------------------------------------------------------


//- Constants ---------------------------------------------------------------


//- Macros ------------------------------------------------------------------


//- External Data -----------------------------------------------------------


//- Variables ---------------------------------------------------------------


//- Forward Declarations ----------------------------------------------------

class KisiTester;

//- Class Definitions -------------------------------------------------------

class KisiTesterHandler  : public DBase
    {
    public:
        KisiTesterHandler();
        virtual ~KisiTesterHandler();
        TInt DoCreate();
	      void CallBack(TAny* aArg);
    
    private:
        void CharacterIn(const TUint8 aKeyCode) const;
		    KisiTester* iKisiTester;
    };


//- Global Function Prototypes ----------------------------------------------


//- Inline Functions --------------------------------------------------------


//- Namespace ---------------------------------------------------------------

#endif // __KISITESTER_HANDLER_H

// End of File
