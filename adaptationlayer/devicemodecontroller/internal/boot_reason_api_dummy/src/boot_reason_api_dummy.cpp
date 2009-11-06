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
*     Implementation of Boot Reason API dummy.
*
*/


// INCLUDES
#include <e32def.h>
#include <kernel/kernel.h>     // Kern::Printf
#include "boot_reason_api.h"

// MACROS
#ifdef _DEBUG
    #define DUMMY_TRACE(a)    Kern::Printf a

#else // _DEBUG 

    #define DUMMY_TRACE(a)
#endif // _DEBUG


class TBootReasonDummy
    {
    public:
        TBootReasonDummy();
    };

DECLARE_STANDARD_EXTENSION()
    {
    Kern::Printf("DMC:DUMMY_BOOT: DECLARE_STANDARD_EXTENSION()");

    
    TBootReasonDummy* bootReasonAptPtr = new TBootReasonDummy;
    
    Kern::Printf("DMC:KERN_EXTEN: DECLARE_STANDARD_EXTENSION() - bootReasonAptPtr: 0x%x ",
                  bootReasonAptPtr);

    return KErrNone;
    }


TBootReasonDummy::TBootReasonDummy()
    {
    DUMMY_TRACE(("DMC:DUMMY_BOOT: TBootReasonDummy() # IN"));

    DUMMY_TRACE(("DMC:DUMMY_BOOT: TBootReasonDummy() # OUT"));
    }

/**
* Boot Reason API dummy imlementations
*/
EXPORT_C void TBootReason::GetStartupMode(TStartupMode& aStartupMode)
    {
    DUMMY_TRACE(("DMC:DUMMY_BOOT: GetStartupMode() # IN"));
    
    aStartupMode = TBootReason::EStartupModeCharging;

    DUMMY_TRACE((("DMC:DUMMY_BOOT: GetStartupMode() - aStartupMode: %d"),
                   aStartupMode));

    DUMMY_TRACE(("DMC:DUMMY_BOOT: GetStartupMode() # OUT"));
    }


EXPORT_C void TBootReason::SetTargetStartupMode(TStartupMode aTargetStartupMode)
    {
    DUMMY_TRACE((("DMC:DUMMY_BOOT: SetTargetStartupMode() # IN - aTargetStartupMode: %d"),
                   aTargetStartupMode));

    DUMMY_TRACE(("DMC:DUMMY_BOOT: SetTargetStartupMode() # OUT"));
    }


EXPORT_C void TBootReason::GetTargetStartupMode(TStartupMode& aStartupMode)
    {
    DUMMY_TRACE(("DMC:DUMMY_BOOT: GetTargetStartupMode() # IN"));
    
    aStartupMode = TBootReason::EStartupModeAlarm;

    DUMMY_TRACE((("DMC:DUMMY_BOOT: SetTargetStartupMode() # OUT - aStartupMode: %d"),
                   aStartupMode));
    }


EXPORT_C TBool TBootReason::IsHiddenReset()
    {
    DUMMY_TRACE(("DMC:DUMMY_BOOT: IsHiddenReset() # IN"));

    DUMMY_TRACE((("DMC:DUMMY_BOOT: IsHiddenReset() # OUT - IsHidden: %d"),
                   ETrue));

    return ETrue;
    }


EXPORT_C void TBootReason::SetSleepTest(TUint16 aTimeout)
    {
    DUMMY_TRACE((("DMC:DUMMY_BOOT: SetSleepTest() # IN - aTimeout: %d"),
                 aTimeout));

    DUMMY_TRACE(("DMC:DUMMY_BOOT: # OUT"));
    }

// End of File
