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
*     This file provides the information required for building
*     Boot Reason API library.
*
*/


#ifndef __BOOT_REASON_API_H__
#define __BOOT_REASON_API_H__

// Include files
#include <e32def.h>

// Constants

// Macros

// Data types

// Function prototypes

// Forward declarations

// Class declaration

/**
@brief TBootReason class

Execution:         Synchronous
Re-entrant:        No
Blocking:          No
Panic mode:        Kern::Fault
Memory allocation: No
Execution time:    Quick
SMP safe:          Yes

@lib boot_reason_api.lib
*/
class TBootReason
    {
    public:
        enum TStartupMode
            {
            EStartupModeNone = 0,
            EStartupModeNormal,
            EStartupModeCharging,
            EStartupModeAlarm,
            EStartupModeTest,
            EStartupModeFota
            };

        /**
        This function is used to read startup mode.

        @param      aStartupMode On return contains one of the enumerated values.

        @return     None.
        */
        IMPORT_C static void GetStartupMode( TStartupMode& aStartupMode );

        /**
        This function is used to write target startup mode.
        
        @param      aTargetStartupMode Target startup mode.

        @return     None.
        */
        IMPORT_C static void SetTargetStartupMode( TStartupMode aTargetStartupMode );

        /**
        This function is used to read target startup mode.

        @param      aTargetStartupMode On return contains one of the enumerated values.

        @return     None.
        */
        IMPORT_C static void GetTargetStartupMode( TStartupMode& aTargetStartupMode );

        /**
        This function is used to read a hidden reset status.

        This query should return ETrue if the system startup is caused by a SW reset
        (e.g. a critical SW failure).
        This query should return EFalse if the system startup is initiated by any
        other means (e.g. power key press or a wakeup alarm).

        @param      None.

        @return     ETrue / EFalse.

        */
        IMPORT_C static TBool IsHiddenReset();

        /**
        This function is used to write sleep test timeout value.

        @param      aResetReasons Sleep test timeout value in milliseconds. The value
                    must not be equal to zero.

        @return     None.
        */
        IMPORT_C static void SetSleepTest( TUint16 aTimeout );
    };

#endif // __BOOT_REASON_API_H__
