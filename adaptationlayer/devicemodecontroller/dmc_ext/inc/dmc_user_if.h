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
*     Declaration of RDmc class.
*
*/


#ifndef DMC_USER_IF_H
#define DMC_USER_IF_H

//  INCLUDES
#include <e32def.h>    // For data types
#include <e32cmn.h>    // RBusLogicalChannel
#include <e32ver.h>    // For version

#ifndef TRACE_MODE_KERNEL
#include <e32svr.h>    // RDebug
#endif

//  CONSTANTS
_LIT(KDmcDrvName, "dmc");

// MACROS
#ifdef _DEBUG
    #define DMC_USER_TRACE(a)    RDebug::Print a
#else // _DEBUG 
    #define DMC_USER_TRACE(a)
#endif // _DEBUG

// Macro that prints out the assertion and makes a User::Panic if given statement is false
#define DMC_USER_TRACE_ASSERT_RESET(a, b, c)  \
    if (!(a))                                 \
        {                                     \
        User::Panic(b, c);                    \
        }

// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class RDmc : public RBusLogicalChannel
    {
    public:

        enum TVer
            {
            EMajorVersionNumber = 1,
            EMinorVersionNumber = 0,
            EBuildVersionNumber = KE32BuildVersionNumber
            };

        /**
        * Defines a target start-up mode in which a device is started.
        */
        enum TTargetStartupMode
            {
            EStartupModeNone = 0,
            EStartupModeNormal,
            EStartupModeCharging,
            EStartupModeAlarm,
            EStartupModeTest,
            EStartupModeFota
            };

        /**
        * Function names in channels DoControl.
        */
        enum TControl
            {
            EGetTargetStartUpMode = 1,
            EGenerateReset,
            EPowerOff,
            EIsHiddenReset,
            EReceiveEvents
            };

        /**
        * Event ID's.
        */
        enum TEvent
            {
            // Reset is used for both a mode change and reset.
            EResetEvent = 1,
            EPowerOffEvent
            };

        /**
        * Creates the logical channel for the current thread.
        *
        * @param     None
        * @return    KErrNone, if successful, otherwise one of the other
        *            system wide error codes.
        */
        inline TInt Open();

        /**
        * Closes the logical channel for the current thread.
        * @param     None
        * @return    NoneKErrNone, if successful, otherwise one of the other
        *            system wide error codes.
        */
        inline void Close();

        /**
        * Gets a start-up mode to which a device is started up.
        *
        * @param   aTargetStartUpMode, defines a mode a device is booted up.
        * @return  KErrNone if succesfull, otherwise one of the other
        *          system wide error code.
        */
        inline TInt GetStartUpMode(TTargetStartupMode& aTargetStartUpMode);

        /**
        * Power off a device.
        *
        * @param   None
        * @return  KErrNone if succesfull,
        *          KErrLocked indicates that a reset cannot performed because
        *                     the reset, power off or mode change is ongoing or
        *                     one of the system wide error code
        */
        inline TInt PowerOff();

        /**
        * Reset a device.
        *
        * @param   aTargetStartUpMode.
        * @return  KErrNone if succesfull,
        *          KErrLocked indicates that a reset cannot performed because
        *                     the reset, power off or mode change is ongoing or
        *                     one of the system wide error code
        */
        inline TInt GenerateReset(TTargetStartupMode& aTargetStartUpMode);

        /**
        * Returns a hidden reset status.
        *
        * @param   aHiddenStatus    ETrue if the reset is hidden, otherwise EFalse.
        * @return  KErrNone if succesfull, otherwise one of the other
        *          system wide error code.
        */
        inline TInt IsHiddenReset(TBool& aHiddenStatus);
    
        /**
        * Receive asynchronous events.
        *
        * @param   aStatus    Request status, completed with KErrNone when data has arrived.
        * @param   aEvent     A type of the event.
        * @return  None
        */
        inline void ReceiveEvents(TRequestStatus& aStatus, TEvent& aEvent);
    };


#ifndef __KERNEL_MODE__
#include "dmc_user_if.inl"
#endif  // __KERNEL_MODE__

#endif // DMC_USER_IF_H

//  End of File
