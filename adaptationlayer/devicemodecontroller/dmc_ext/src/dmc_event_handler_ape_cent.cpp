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
*     Implementation of DMC event handler.
*
*/


// INCLUDE FILES
#if _DEBUG
#include <kern_priv.h>
#endif // _DEBUG
#include <pn_const.h>               // PN_MODEM_MCE
#include <phonetisi.h>              // ISI_HEADER_***

// USE_MTC_SERVER is only for debugging purposes until brige hw is available
#ifdef USE_MTC_SERVER
#include <mtcisi.h>                 // MTC related definitions. Temporary for testing
#else // USE_MTC_SERVER
#include <mceisi.h>                 // MCE ISI stuff
#endif // USE_MTC_SERVER

#include <isikernel.h>              // TIsiKernelSend
#include "dmc_extension.h"          // DDmcExtension
#include "dmc_event_handler_ape_cent.h"
#include "dmc_mce_isi.h"
#include "dmc_trace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dmc_event_handler_ape_centTraces.h"
#endif

// EXTERNAL DATA STRUCTURES 
// EXTERNAL FUNCTION PROTOTYPES 
// FORWARD DECLARATIONS
// CONSTANTS

// MACROS
// TO-macro is used only for reading purposes
#define TO(to) ((TUint8)to)

#define SET_BIT(bit_map, bits) bit_map |= bits

// LOCAL CONSTANTS AND MACROS
_LIT(KDmcDrvName, "dmc");

#ifdef USE_MTC_SERVER

#define MCE_STATE_INIT_VALUE      0xFF
#define MCE_ACTION_INIT_VALUE     0xFF

#else // USE_MTC_SERVER

/** 
 * Define MCE init state and action values because an ISI API does not provide them.
 */
#define MCE_STATE_INIT_VALUE      static_cast<TUint8>(static_cast<TInt8>(~(MCE_NORMAL | MCE_LOCAL | MCE_SW_RESET | MCE_POWER_OFF)))
#define MCE_ACTION_INIT_VALUE     static_cast<TUint8>(static_cast<TInt8>(~(MCE_START | MCE_READY)))

#endif // USE_MTC_SERVER

/**
* Modem actions:
*   Constants for iModemActionBitMap
*/
// Modem connection lost
#define MODEM_CONNECTION_NOT_OK   0x01
// Modem state transition ready
#define MODEM_READY               0x02
#define MODEM_NO_ACTIONS          0x00



/**
* STATES of this event hander:
*   Constants for iCurrentstate
*/
#define IDLE                      0x00  // All transactions begins in the IDLE-state.
#define BUSY                      0x01  // Used while checking an incoming transaction request.
#define APE_PWR_OFF               0x02  // Handling APE orginated power off request.
#define MODEM_PWR_OFF             0x03  // Handling Modem orginated power off request.
#define APE_RESET                 0x04  // Handling APE orginated reset.
#define MODEM_CONTROLLED_RESET    0x05  // Handling Modem controlled reset.
#define MODEM_UNCONTROLLED_RESET  0x06  // Handling Modem uncontrolled reset.
/* LAST_STATE is the helper state used for detecting the value
   of the last state */
#define LAST_STATE                (MODEM_UNCONTROLLED_RESET + 1)
#define NO_STATES_DEFINED         0x00

// MODULE DATA STRUCTURES
DDmcExtension* DmcEvHandApeCent::iDmcExtPtr(NULL);
DMutex* DmcEvHandApeCent::iMutexPtr(NULL);
TDfc* DmcEvHandApeCent::iUserEventDfcPtr(NULL);
TBootReason::TStartupMode DmcEvHandApeCent::iTargetStartupMode(TBootReason::EStartupModeNone);
TBool DmcEvHandApeCent::iHiddenStatus(EFalse);
// Init with no valid state value
TUint8 DmcEvHandApeCent::iModemCurrentState(MCE_STATE_INIT_VALUE);
TUint8 DmcEvHandApeCent::iModemActionBitMap(MODEM_NO_ACTIONS);
TUint8 DmcEvHandApeCent::iPowerDownCalled(FALSE);
TUint8 DmcEvHandApeCent::iEventType(USER_CLIENT_NO_EVENT);
TUint8 DmcEvHandApeCent::iCurrentState(IDLE);

// This table defines allowed state transitions.
const TUint8 DmcEvHandApeCent::iStateTransitionMap[LAST_STATE] =
    {
    // FROM IDLE ->
    TO((1 << BUSY |
        1 << APE_PWR_OFF |
        1 << MODEM_PWR_OFF |
        1 << APE_RESET |
        1 << MODEM_CONTROLLED_RESET |
        1 << MODEM_UNCONTROLLED_RESET)),
    // FROM BUSY ->
    TO((1 << IDLE |
        1 << APE_PWR_OFF |
        1 << MODEM_PWR_OFF |
        1 << APE_RESET |
        1 << MODEM_CONTROLLED_RESET |
        1 << MODEM_UNCONTROLLED_RESET)),
    // FROM APE_PWR_OFF ->
    TO((NO_STATES_DEFINED)),
    // FROM MODEM_PWR_OFF ->
    TO((NO_STATES_DEFINED)),
    // FROM APE_RESET ->
    TO((NO_STATES_DEFINED)),
    // FROM MODEM_CONTROLLED_RESET ->
    TO((NO_STATES_DEFINED)),
    // FROM MODEM_UNCONTROLLED_RESET ->
    TO((NO_STATES_DEFINED)),
    };

/* This bitmap defines user events that are handled in an any-state. Kernel event any
   state checking is based on the resource ID and message ID.*/ 
const TUint32 DmcEvHandApeCent::iAnyStateMap((1 << USER_GET_TARGET_STARTUP_MODE |
                                              1 << USER_GET_HIDDEN_RESET_STATUS |
                                              1 << USER_GET_EVENT_TYPE));

// LOCAL FUNCTION PROTOTYPES  


// ==================== LOCAL FUNCTIONS ====================

// -----------------------------------------------------------------------------
// DmcEvHandApeCent::Lock
// -----------------------------------------------------------------------------
//
inline void DmcEvHandApeCent::Lock()
    {
    OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_LOCK,
             "DMC:EH_APE_CEN: Lock()");
    DMC_TRACE(("DMC:EH_APE_CEN: Lock()"));

    NKern::ThreadEnterCS();
    Kern::MutexWait(*iMutexPtr);
    }


// -----------------------------------------------------------------------------
// DmcEvHandApeCent::Unlock
// -----------------------------------------------------------------------------
//
inline void DmcEvHandApeCent::Unlock()
    {
    OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_UNLOCK,
             "DMC:EH_APE_CEN: Unlock()");
    DMC_TRACE(("DMC:EH_APE_CEN: Unlock()"));

    Kern::MutexSignal(*iMutexPtr);
    NKern::ThreadLeaveCS();
    }


// ================= OTHER EXPORTED FUNCTIONS ==============
// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// DDmcExtension::Init
// -----------------------------------------------------------------------------
//
void DmcEvHandApeCent::Init(DDmcExtension* const aDmcExtPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_CENT_INIT_ENTRY,
              "DMC:EH_APE_CEN: Init() # IN - aDmcExtPtr: 0x%x", aDmcExtPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: Init() # IN - aDmcExtPtr: 0x%x "), aDmcExtPtr));

    DMC_TRACE_ASSERT_RESET(aDmcExtPtr, "DMC:EH_APE_CEN: aDmcExtPtr, NULL pointer",
                           KErrNoMemory);

    iDmcExtPtr = aDmcExtPtr;

    /* Add the extension pointer so that ISI-messages can be sent
       directly from the place where the message has been built. */
    MceIsi::Init(aDmcExtPtr);

    DMC_TRACE((("DMC:EH_APE_CEN: Init() - Event Handler -> iCurrentState: 0x%02x "),
                 iCurrentState));

    // Create a mutex
    TInt err = Kern::MutexCreate(iMutexPtr, KDmcDrvName , KMutexOrdGeneral0);

    OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_INIT_1,
              "DMC:EH_APE_CEN: Init() - MutexCreate, err: %d", err);

    DMC_TRACE((("DMC:EH_APE_CEN: Init() - MutexCreate, err: %d "), err));
    DMC_TRACE_ASSERT_RESET(err == KErrNone , "Kern::MutexCreate failed", err);

#if _DEBUG
    /* Print the bitmap(HW_CONF_RECORD) whom boot_reason_api uses in the target startup decision.
       Values of the HW_CONF_RECORD can be found in the "Loader Chipset API" specification. */
    TUint32 startupReason = Kern::SuperPage().iHwStartupReason;

    OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_INIT_2,
              "DMC:EH_APE_CEN: Init() - SuperPage().iHwStartupReason, startupReason: 0x%08x",
              startupReason);

    DMC_TRACE((("DMC:EH_APE_CEN: Init() - SuperPage().iHwStartupReason, startupReason: 0x%08x"),
                 startupReason));
#endif // _DEBUG

    DmcEvHandApeCent::Lock();
    /* Get and set the target startup mode in which a device is started.
       This is needed in case of device crash. */
    TBootReason::GetStartupMode(iTargetStartupMode);
    TBootReason::SetTargetStartupMode(iTargetStartupMode);
    
    // Get a hidden reset status.
    iHiddenStatus = TBootReason::IsHiddenReset();
    DmcEvHandApeCent::Unlock();

    OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_INIT_3,
              "DMC:EH_APE_CEN: Init() - iTargetStartupMode: 0x%02x",
              iTargetStartupMode);
    DMC_TRACE((("DMC:KERN_EXTEN: Init() - iTargetStartupMode: 0x%02x"),
                 iTargetStartupMode));
    OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_INIT_4,
              "DMC:EH_APE_CEN: Init() - iHiddenStatus: %d",
              iHiddenStatus);
    DMC_TRACE((("DMC:KERN_EXTEN: Init() - iHiddenStatus: %d"),
                 iHiddenStatus));
                 
    // Get Modem current state
    MceIsi::MceModemStateQueryReq();

    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_CENT_INIT_RETURN,
              "DMC:EH_APE_CEN: Init() # OUT - iDmcExtPtr: 0x%x", iDmcExtPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: Init() # OUT - iDmcExtPtr: 0x%x"), iDmcExtPtr));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::SubscribeEvents
// Set the user logical device dfc-queue available for modem events.
// -----------------------------------------------------------------------------
//
void DmcEvHandApeCent::SubscribeEvents(TDfc* const aUserEventDfcPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_SUBSCRIBE_EVENTS_ENTRY,
              "DMC:EH_APE_CEN: SubscribeEvents() # IN - aUserEventDfcPtr: 0x%x",
              aUserEventDfcPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: SubscribeEvents() # IN - aUserEventDfcPtr: 0x%x "),
                 aUserEventDfcPtr));

    DMC_TRACE_ASSERT_RESET(aUserEventDfcPtr, "DMC:EH_APE_CEN: aUserEventDfcPtr, NULL pointer",
                           KErrNoMemory);

    iUserEventDfcPtr = aUserEventDfcPtr;
    
    // Check if there are pending events.
    if (iEventType != USER_CLIENT_NO_EVENT)
        {
        OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_SUBSCRIBE_EVENTS_1,
                  "DMC:EH_APE_CEN: SubscribeEvents() - Pending event, iEventType: %d",
                  iEventType);

        if (NKern::CurrentContext() == NKern::EInterrupt)
            {
            iUserEventDfcPtr->Add();
            }
        else
            {
            iUserEventDfcPtr->Enque();
            }        
        }

    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_SUBSCRIBE_EVENTS_RETURN,
              "DMC:EH_APE_CEN: SubscribeEvents() # OUT - iUserEventDfcPtr: 0x%x",
              iUserEventDfcPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: SubscribeEvents() # OUT - iUserEventDfcPtr: 0x%x"),
                 iUserEventDfcPtr));
    }

// -----------------------------------------------------------------------------
// DDmcExtension::UnsubscribeEvents
// -----------------------------------------------------------------------------
//
void DmcEvHandApeCent::UnsubscribeEvents(TDfc* const aUserEventDfcPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_CENT_UNSUBSCRIBE_EVENTS_ENTRY,
              "DMC:EH_APE_CEN: UnsubscribeEvents() # IN - aUserEventDfcPtr: 0x%x", aUserEventDfcPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: UnsubscribeEvents() # IN - aUserEventDfcPtr: 0x%x "), aUserEventDfcPtr));

    /* Must be a NULL pointer since a client must have already delete the DFfc object before calling
       this function. */
    DMC_TRACE_ASSERT_RESET(!aUserEventDfcPtr, "DMC:EH_APE_CEN: aUserEventDfcPtr, Not NULL pointer",
                           KErrGeneral);

    iUserEventDfcPtr = aUserEventDfcPtr;

    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_UNSUBSCRIBE_EVENTS_RETURN,
              "DMC:EH_APE_CEN: UnsubscribeEvents() # OUT - iUserEventDfcPtr: 0x%x",
              iUserEventDfcPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: UnsubscribeEvents() # OUT - iUserEventDfcPtr: 0x%x"),
                 iUserEventDfcPtr));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::HandleEvent
// This function is the only entry point to access to the DMC Ape Centric 
// Event Handler.
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::HandleEvent(const TUint8  aEventType,
                                   const TUint8* const aKernMsgPtr,
                                   TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_HANDLE_EVENT_ENTRY,
                 "DMC:EH_APE_CEN: HandleEvent() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: HandleEvent() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrLocked);

    // First check if the event is part of the any state
    if (IsAnyStateEvent(aEventType, aKernMsgPtr))
        {
        OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_1,
                  "DMC:EH_APE_CEN: HandleEvent() - ANY, iCurrentState: 0x%02x", iCurrentState);
        DMC_TRACE((("DMC:EH_APE_CEN: HandleEvent() - ANY, iCurrentState: 0x%02x"), iCurrentState));

        StateAny(aEventType, aKernMsgPtr, aUsrMsgPtr);

        ret = KErrNone;
        }
    else
        {
        OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_2,
                  "DMC:EH_APE_CEN: HandleEvent() - iCurrentState: 0x%02x", iCurrentState);
        DMC_TRACE((("DMC:KERN_EXTEN: HandleEvent() - iCurrentState: 0x%02x"), iCurrentState));

        // Check the state
        switch (iCurrentState)
            {
            case IDLE:
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_IDLE_1,
                          "DMC:EH_APE_CEN: HandleEvent() - IDLE");
                DMC_TRACE(("DMC:EH_APE_CEN: HandleEvent() - IDLE"));
                
                StateIdle(aEventType, aKernMsgPtr, aUsrMsgPtr);
                
                ret = KErrNone;
                }
                break;

            case BUSY:
                {
                /* In busy state we do not handle anything, only one
                   transaction is allowed at the time. */
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_BUSY_1,
                          "DMC:EH_APE_CEN: HandleEvent() - BUSY");
                DMC_TRACE(("DMC:EH_APE_CEN: HandleEvent() - BUSY"));
                }
                break;
                
            case APE_PWR_OFF:
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_APE_PWR_OFF_1,
                          "DMC:EH_APE_CEN: HandleEvent() - APE_PWR_OFF");
                DMC_TRACE(("DMC:EH_APE_CEN: HandleEvent() - APE_PWR_OFF"));

                StateApePwrOff(aEventType, aKernMsgPtr, aUsrMsgPtr);
                }
                break;

            case MODEM_PWR_OFF:
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_MODEM_PWR_OFF_1,
                          "DMC:EH_APE_CEN: HandleEvent() - MODEM_PWR_OFF");
                DMC_TRACE(("DMC:EH_APE_CEN: HandleEvent() - MODEM_PWR_OFF"));

                StateModemPwrOff(aEventType, aKernMsgPtr, aUsrMsgPtr);
                }
                break;

            case APE_RESET:
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_APE_RESET_1,
                          "DMC:EH_APE_CEN: HandleEvent() - APE_RESET");
                DMC_TRACE(("DMC:EH_APE_CEN: HandleEvent() - APE_RESET"));

                StateApeReset(aEventType, aKernMsgPtr, aUsrMsgPtr);
                }
                break;

            case MODEM_CONTROLLED_RESET:
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_MODEM_CONTROLLED_RESET_1,
                          "DMC:EH_APE_CEN: HandleEvent() - MODEM_CONTROLLED_RESET");
                DMC_TRACE(("DMC:EH_APE_CEN: HandleEvent() - MODEM_CONTROLLED_RESET"));

                StateModemControlledReset(aEventType, aKernMsgPtr, aUsrMsgPtr);
                }
                break;

            case MODEM_UNCONTROLLED_RESET:
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_MODEM_UNCONTROLLED_RESET_1,
                          "DMC:EH_APE_CEN: HandleEvent() - MODEM_UNCONTROLLED_RESET");
                DMC_TRACE(("DMC:EH_APE_CEN: HandleEvent() - MODEM_UNCONTROLLED_RESET"));

                StateModemUncontrolledReset(aEventType, aKernMsgPtr, aUsrMsgPtr);
                }
                break;

            default:
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_STATE_DEFAULT,
                          "DMC:EH_APE_CEN: HandleEvent() - iCurrentState");
                DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: HandleUserEvent() - Illegal state",
                                               KErrArgument);
                }
                break;
            }
        }

    OstTraceExt2(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_HANDLE_EVENT_RETURN,
                 "DMC:EH_APE_CEN: HandleEvent() # OUT - iCurrentState: 0x%02x, ret: 0x%x",
                 iCurrentState, (TUint)ret);
    DMC_TRACE((("DMC:EH_APE_CEN: HandleEvent() # OUT - iCurrentState: 0x%02x, ret: %d"),
                 iCurrentState, ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateAny
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::StateAny(const TUint8  aEventType,
                                const TUint8* const aKernMsgPtr,
                                TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_STATE_ANY_ENTRY,
                 "DMC:EH_APE_CEN: StateAny() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: StateAny() # IN - aType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrNone);
    
    // Supported events in the state ANY. No state transitions are executed from state ANY.
    switch (aEventType)
        {
        case KERNEL_EVENT_ISI:
            {
            TUint8 resourceId = aKernMsgPtr[ISI_HEADER_OFFSET_RESOURCEID];
    
            DMC_TRACE((("DMC:EH_APE_CEN: StateAny() - resource ID: 0x%x: "), resourceId));

#ifdef USE_MTC_SERVER
            if (resourceId == PN_MTC)
                {
                DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - PN_MTC"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];
            
                switch (messageId)
                    {
                    case MTC_STATE_QUERY_RESP:
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - MTC_STATE_QUERY_RESP"));
    
                        HandleMceModemStateQueryResp(aKernMsgPtr);
                        }
                        break;
                    }
                }
#else // USE_MTC_SERVER
            if (resourceId == PN_MODEM_MCE)
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_KERN_1,
                          "DMC:EH_APE_CEN: StateAny() - PN_MODEM_MCE");
                DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - PN_MODEM_MCE"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];
            
                switch (messageId)
                    {
                    case MCE_MODEM_STATE_QUERY_RESP:
                        {
                        OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_KERN_2,
                                  "DMC:EH_APE_CEN: StateAny() - MCE_STATE_QUERY_RESP");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - MCE_STATE_QUERY_RESP"));
    
                        HandleMceModemStateQueryResp(aKernMsgPtr);
                        }
                        break;
                    default:
                        {
                        OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_KERN_3,
                                  "DMC:EH_APE_CEN: StateAny() - default, Unknown MCE MSG");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - default, Unknown MCE MSG"));
                        }
                        break;
                    }
                }
#endif // USE_MTC_SERVER
            }
            break;

        case KERNEL_EVENT_PWR_HANDLER:
            {
            }
            break;

        case USER_GET_TARGET_STARTUP_MODE:
            {
            OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_USER_1,
                      "DMC:EH_APE_CEN: StateAny() - USER_GET_TARGET_STARTUP_MODE");
            DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - USER_GET_TARGET_STARTUP_MODE"));

            // Get the target startup mode if the mode has not been fetched yet
            DmcEvHandApeCent::Lock();

            if (iTargetStartupMode == TBootReason::EStartupModeNone)
                {
                OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_EVENT_2,
                          "DMC:EH_APE_CEN: StateAny() - Get the target startup mode through Boot Reason API");
                DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - Get the target startup mode through Boot Reason API"));

                TBootReason::GetStartupMode(iTargetStartupMode);
                }

            *aUsrMsgPtr = static_cast<TUint>(iTargetStartupMode);
            DmcEvHandApeCent::Unlock();

            OstTrace1(TRACE_FLOW, STATE_ANY_EVENT_3,
                      "DMC:EH_APE_CEN: StateAny() - iTargetStartupMode: 0x%x",
                       iTargetStartupMode);
            DMC_TRACE((("DMC:EH_APE_CEN: StateAny() - iTargetStartupMode: 0x%x"),
                         iTargetStartupMode));
            }
            break;

        case USER_GET_HIDDEN_RESET_STATUS:
            {
            OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_EVENT_4,
                      "DMC:EH_APE_CEN: StateAny() - USER_GET_HIDDEN_RESET_STATUS");
            DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - USER_GET_HIDDEN_RESET_STATUS"));

            // No need to lock because a status is updated once in the init phase.
            *aUsrMsgPtr = iHiddenStatus;

            OstTrace1(TRACE_FLOW, STATE_ANY_HIDDEN_EVENT_5,
                      "DMC:EH_APE_CEN: StateAny() - IsHidden: %d",
                       *aUsrMsgPtr);
            DMC_TRACE((("DMC:EH_APE_CEN: StateAny() - IsHidden: %d"),
                         *aUsrMsgPtr));
            }
            break;

        case USER_GET_EVENT_TYPE:
            {
            OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_EVENT_5,
                      "DMC:EH_APE_CEN: StateAny() - USER_GET_EVENT_TYPE");
            DMC_TRACE(("DMC:EH_APE_CEN: StateAny() - USER_GET_EVENT_TYPE"));

            DmcEvHandApeCent::Lock();
            
            *aUsrMsgPtr = iEventType;
            
            DmcEvHandApeCent::Unlock();

            OstTrace1(TRACE_FLOW, STATE_ANY_EVENT_5,
                      "DMC:EH_APE_CEN: StateAny() - User event type: 0x%x",
                       *aUsrMsgPtr);
            DMC_TRACE((("DMC:EH_APE_CEN: StateAny() - User event type: 0x%x"),
                         *aUsrMsgPtr));
            }
            break;

        default: // aEventType
            {
            // Fatal error if the wrong event type. Make a reset and fix the problem.
            OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_STATE_ANY_EVN_TYPE_DEFAULT,
                      "DMC:EH_APE_CEN: StateAny() - Illegal Event Type");
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: StateAny() - Illegal Event Type",
                                           KErrArgument);
            }
            break;
        }

    OstTrace1(TRACE_ENTRY_EXIT, STATE_ANY_RETURN,
              "DMC:EH_APE_CEN: StateAny() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:EH_APE_CEN: StateAny() # OUT - ret: %d"), ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateIdle
// IDLE-state is the only state in which new transactions are started.
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::StateIdle(const TUint8  aEventType,
                                 const TUint8* const aKernMsgPtr,
                                 TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, STATE_IDLE_ENTRY,
                 "DMC:EH_APE_CEN: StateIdle() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrNone);

    /*
     * When checking whether a request is to be served in the IDLE-state, the 
     * state is changed into BUSY to make sure that no new requqests are taken
     * into a process.
     *
     * If the request is not processed, the state will be changed back to
     * IDLE, otherwise to a state is changed according a request.
     */
    DmcEvHandApeCent::SetNextState(BUSY);

    // Supported events in the IDLE-state
    switch (aEventType)
        {
        case KERNEL_EVENT_ISI:
            {
            OstTrace0(TRACE_FLOW, STATE_STATE_IDLE_EV_TYPE_ISI,
                      "DMC:EH_APE_CEN: StateIdle() - KERNEL_EVENT_ISI");
            DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - KERNEL_EVENT_ISI"));

            // ISI msg pointer has been checked in the associated DfcQueue in the extension.
            TUint8 resourceId = aKernMsgPtr[ISI_HEADER_OFFSET_RESOURCEID];

            OstTrace1(TRACE_FLOW, STATE_STATE_IDLE_KERN_EV_ISI_1,
                      "DMC:EH_APE_CEN: StateIdle() - resourceId: 0x%x", (TUint)resourceId);
            DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - resourceId: 0x%x:"), resourceId));

#ifdef USE_MTC_SERVER
            if (resourceId == PN_MTC)
                {
                DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - PN_MTC"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MTC_STATE_INFO_IND:
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MTC_STATE_INFO_IND"));

                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);

                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);
                        
                        DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - modemState: 0x%x, modemAction: 0x%x"),
                                     modemState, modemAction));

                       // In the IDLE-state we are only interested if Modem starts reset or power off.
                       if (modemAction == MTC_START)
                           {
                           DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MTC_START"));
                           
                           if (modemState == MTC_POWER_OFF)
                               {
                               DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MTC_POWER_OFF"));
                               
                               // First set the target start-up mode to "none" meaning power off.
                               DmcEvHandApeCent::Lock();
                               iTargetStartupMode = TBootReason::EStartupModeNone;
                               TBootReason::SetTargetStartupMode(TBootReason::EStartupModeNone);
                            
                               /* Set the user client event type. The type of the event is read
                                  once the iUserEventDfcPtr dfc-queue gets execution time. */
                               iEventType = USER_CLIENT_POWER_OFF_EVENT;
                               DmcEvHandApeCent::Unlock();
                            
                               DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Boot API, iTargetStartupMode: 0x%x, User iEventType: 0x%x"), 
                                            iTargetStartupMode, iEventType));
                               
                               // Change the state to MODEM_PWR_OFF
                               DmcEvHandApeCent::SetNextState(MODEM_PWR_OFF);
                               }

                           if (modemState == MTC_SW_RESET)
                               {
                               DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MTC_SW_RESET"));
                               
                               // Reset a device back to the same state as started.
                               DmcEvHandApeCent::Lock();
                               TBootReason::SetTargetStartupMode(iTargetStartupMode);
                            
                               /* Set the user client event type. The type of the event is read
                                  once the iUserEventDfcPtr dfc-queue gets execution time. */
                               iEventType = USER_CLIENT_RESET_EVENT;
                               DmcEvHandApeCent::Unlock();
                            
                               DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Boot API, iTargetStartupMode: 0x%x, User iEventType: 0x%x"), 
                                            iTargetStartupMode, iEventType));
                               
                               // Change the state to MODEM_CONTROLLED_RESET
                               DmcEvHandApeCent::SetNextState(MODEM_CONTROLLED_RESET);
                               }

                           // Now check if we have to generate a user event.
                           if ((modemState == MTC_POWER_OFF) || (modemState == MTC_SW_RESET))
                               {
                               DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Generate User event, iUserEventDfcPtr: 0x%x"),
                                             iUserEventDfcPtr));
                               /* Generate the user event.
                                  If no registered clients yet. Remain in the MODEM_CONTROLLED_RESET state
                                  and as soon as the first client subscribes events, the power
                                  off event is generated. */
                               if (iUserEventDfcPtr)
                                   {
                                   if (NKern::CurrentContext() == NKern::EInterrupt)
                                       {
                                       iUserEventDfcPtr->Add();
                                       }
                                   else
                                       {
                                       iUserEventDfcPtr->Enque();
                                       }
                                   }
                               }
                           }
                        }
                        break;

                    default:
                        {
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateIdle - \
                                     MCE ISI message not supported in this state, messageId: 0x%x"),
                                     messageId));
                        }
                        break;
                    }
                }
#else // USE_MTC_SERVER
            if (resourceId == PN_MODEM_MCE)
                {
                OstTrace0(TRACE_FLOW, STATE_STATE_IDLE_KERN_EV_ISI_2,
                          "DMC:EH_APE_CEN: StateIdle() - PN_MODEM_MCE");
                DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - PN_MODEM_MCE"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MCE_MODEM_STATE_IND:
                        {
                        OstTrace0(TRACE_FLOW, STATE_STATE_IDLE_KERN_EV_ISI_CASE_1,
                                      "DMC:EH_APE_CEN: StateIdle() - MCE_MODEM_STATE_IND");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MCE_MODEM_STATE_IND"));

                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);

                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        
                        OstTraceExt2(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_STATE_IDLE_MCE_STATE_IND_1,
                                     "DMC:EH_APE_CEN: DMC:EH_APE_CEN: StateIdle() - modemState: 0x%x, modemAction: 0x%x",
                                     (TUint)modemState, (TUint)modemAction);
                        DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - modemState: 0x%x, modemAction: 0x%x"),
                                     modemState, modemAction));
                        
                        if (modemAction == MCE_START)
                            {
                            OstTrace0(TRACE_FLOW, STATE_STATE_IDLE_KERN_EV_ISI_CASE_2,
                                      "DMC:EH_APE_CEN: StateIdle() - MCE_START");
                            DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MCE_START"));

                            if (modemState == MCE_POWER_OFF)
                                {
                                OstTrace0(TRACE_FLOW, STATE_STATE_IDLE_KERN_EV_ISI_CASE_3,
                                          "DMC:EH_APE_CEN: StateIdle() - MCE_POWER_OFF");
                                DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MCE_POWER_OFF"));
                               
                                // First set the target start-up mode to "none" meaning power off.
                                DmcEvHandApeCent::Lock();
                                iTargetStartupMode = TBootReason::EStartupModeNone;
                                TBootReason::SetTargetStartupMode(TBootReason::EStartupModeNone);
                            
                                /* Set the user client event type. The type of the event is read
                                   once the iUserEventDfcPtr dfc-queue gets execution time. */
                                iEventType = USER_CLIENT_POWER_OFF_EVENT;
                                DmcEvHandApeCent::Unlock();
                            
                                OstTraceExt2(TRACE_FLOW, STATE_STATE_IDLE_KERN_EV_ISI_CASE_4,
                                             "DMC:EH_APE_CEN: StateIdle() - Boot API, iTargetStartupMode: 0x%x, User Event, iEventType: 0x%x",
                                             (TUint)iTargetStartupMode, (TUint)iEventType);
                                DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Boot API, iTargetStartupMode: 0x%x, User Event, iEventType: 0x%x"), 
                                             iTargetStartupMode, iEventType));
                               
                                // Change the state to MODEM_PWR_OFF
                                DmcEvHandApeCent::SetNextState(MODEM_PWR_OFF);
                                }

                           // Modes  makes reset, MODEM_CONTROLLED_RESET. 
                           if (modemState == MCE_SW_RESET)
                               {
                               DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - MCE_SW_RESET"));
                               
                               // Reset a device back to the same state as started.
                               DmcEvHandApeCent::Lock();
                               TBootReason::SetTargetStartupMode(iTargetStartupMode);
                            
                               /* Set the user client event type. The type of the event is read
                                  once the iUserEventDfcPtr dfc-queue gets execution time. */
                               iEventType = USER_CLIENT_RESET_EVENT;
                               DmcEvHandApeCent::Unlock();
                            
                               DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Boot API, iTargetStartupMode: 0x%x, User Event, iEventType: 0x%x"), 
                                            iTargetStartupMode, iEventType));
                               
                               // Change the state to MODEM_CONTROLLED_RESET
                               DmcEvHandApeCent::SetNextState(MODEM_CONTROLLED_RESET);
                               }

                           // Now check if we have to generate a user event.
                           if ((modemState == MCE_POWER_OFF) || (modemState == MCE_START))
                               {
                               OstTrace1(TRACE_FLOW, STATE_STATE_IDLE_KERN_EV_ISI_CASE_5,
                                         "DMC:EH_APE_CEN: StateIdle() - Generate User event, iUserEventDfcPtr: 0x%x",
                                         iUserEventDfcPtr);
                               DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Generate User event, iUserEventDfcPtr: 0x%x"),
                                             iUserEventDfcPtr));
    
                               /* Generate the user event.
                                  If no registered clients yet. Remain in the MODEM_CONTROLLED_RESET state
                                  and as soon as the first client subscribes events, the power
                                  off event is generated. */
                               if (iUserEventDfcPtr)
                                   {
                                   if (NKern::CurrentContext() == NKern::EInterrupt)
                                       {
                                       iUserEventDfcPtr->Add();
                                       }
                                   else
                                       {
                                       iUserEventDfcPtr->Enque();
                                       }
                                   }
                               }
                            }
                        }
                        break;
                   }
                }
#endif // USE_MTC_SERVER
            }
            break;

        case KERNEL_EVENT_PWR_HANDLER:
            {
            // We should never come here.
            OstTrace0(TRACE_FLOW, STATE_STATE_IDLE_EV_TYPE_PWR_HANDLER,
                      "DMC:EH_APE_CEN: StateIdle() - KERNEL_EVENT_PWR_HANDLER");
            DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - KERNEL_EVENT_PWR_HANDLER"));
            }
            break;

        /* Modem connection lost and no other transactions ongoing,
           thus handle as a modem uncontrolled reset. */
        case KERNEL_EVENT_MODEM_CONNECTION_NOT_OK:
            {
            OstTrace0(TRACE_FLOW, STATE_STATE_IDLE_EV_TYPE_CONNECTION_NOK_1,
                      "DMC:EH_APE_CEN: StateIdle() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK");
            DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK"));

            // Reset a device back to the same state as started.
            DmcEvHandApeCent::Lock();
            TBootReason::SetTargetStartupMode(iTargetStartupMode);

            /* Set the user client event type. The type of the event is read
               once the iUserEventDfcPtr dfc-queue gets execution time. */
           iEventType = USER_CLIENT_RESET_EVENT;
           DmcEvHandApeCent::Unlock();

           OstTraceExt2(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_STATE_IDLE_CONNECTION_NOK_2,
                        "DMC:EH_APE_CEN: StateIdle() - Boot API, iTargetStartupMode: 0x%x, User Event, iEventType: 0x%x",
                        (TUint)iTargetStartupMode, (TUint)iEventType);
           DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Boot API, iTargetStartupMode: 0x%x, User Event, iEventType: 0x%x"),
                        iTargetStartupMode, iEventType));

           OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_STATE_IDLE_CONNECTION_NOK_3,
                     "DMC:EH_APE_CEN: StateIdle() - Generate User event, iUserEventDfcPtr: 0x%x", 
                     iUserEventDfcPtr);
           DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - Generate User event, iUserEventDfcPtr: 0x%x"),
                        iUserEventDfcPtr));
           /* Generate the user event.
              If no registered clients yet. Remain in the MODEM_UNCONTROLLED_RESET state
              and as soon as the first client subscribes events, the power
              off event is generated. */
           if (iUserEventDfcPtr)
               {
               if (NKern::CurrentContext() == NKern::EInterrupt)
                   {
                   iUserEventDfcPtr->Add();
                   }
               else
                   {
                   iUserEventDfcPtr->Enque();
                   }
               }

           // Change the state to MODEM_UNCONTROLLED_RESET
           DmcEvHandApeCent::SetNextState(MODEM_UNCONTROLLED_RESET);
            }
            break;

        case USER_GENERATE_RESET:
            {
            OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_STATE_IDLE_USER_EVENT_1,
                      "DMC:EH_APE_CEN: StateIdle() - USER_GENERATE_RESET, targetStartUpMode: 0x%x",
                       (TUint)*aUsrMsgPtr);
            DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() - USER_GENERATE_RESET, targetStartUpMode: 0x%x"),
                         *aUsrMsgPtr));

            // First set the target start-up mode in which a device is booted after a reset
            DmcEvHandApeCent::Lock();
            iTargetStartupMode = static_cast<TBootReason::TStartupMode>(*aUsrMsgPtr);
            TBootReason::SetTargetStartupMode(iTargetStartupMode);
            DmcEvHandApeCent::Unlock();

            // Send indications that we are going to reset.
            // Send DMC_RESET_STARTED_IND

            /* Change the state to APE_RESET in which we wait a power down
               signal from Power Handler.*/
            DmcEvHandApeCent::SetNextState(APE_RESET);
            }
            break;

        case USER_POWER_OFF:
            {
            OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_STATE_IDLE_USER_EVENT_2,
                      "DMC:EH_APE_CEN: StateIdle() - USER_POWER_OFF");
            DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - USER_POWER_OFF"));

            /* Set EStartupModeNone to power off the device.
               A device still might do the reset if e.g. a charger is connected, an alarm
               occurs or a power key is pressed. This detection is done by "Power Controller". */
            DmcEvHandApeCent::Lock();
            iTargetStartupMode = static_cast<TBootReason::TStartupMode>(*aUsrMsgPtr);
            TBootReason::SetTargetStartupMode(iTargetStartupMode);
            DmcEvHandApeCent::Unlock();

            // Send indications that we are going to power off.
            // Send DMC_POWER_OFF_STARTED_IND

            /* Change the state to APE_PWR_OFF in which we wait a power down
               signal from Power Handler.*/
            DmcEvHandApeCent::SetNextState(APE_PWR_OFF);
            }
            break;

        default:
            {
            /* Fatal error. */ 
            OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_STATE_IDLE_EV_TYPE_DEFAULT,
                      "DMC:EH_APE_CEN: StateIdle() - Illegal event type, aUsrMsgPtr->EventId: %d",
                      aEventType);
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: StateIdle() - Illegal event type",
                                           aEventType);
            }
            break;
        }

    // Change a state back to IDLE if the event is not part of the IDLE-state.
    if (iCurrentState == BUSY)
        {
        OstTrace0(TRACE_FLOW, STATE_IDLE_REMAIN_BUSY,
                          "DMC:EH_APE_CEN: StateIdle() - Remain in IDLE");
        DMC_TRACE(("DMC:EH_APE_CEN: StateIdle() - Remain in IDLE"));
        DmcEvHandApeCent::SetNextState(IDLE);
        }

    OstTrace1(TRACE_ENTRY_EXIT, STATE_IDLE_RETURN,
              "DMC:EH_APE_CEN: StateIdle() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:EH_APE_CEN: StateIdle() # OUT - ret: %d"), ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateApePwrOff
// State APE_PWR_OFF: Ape orginated power off request(PC or SSM Adaptation).
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::StateApePwrOff(const TUint8  aEventType,
                                      const TUint8* const aKernMsgPtr,
                                      TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, STATE_PWR_OFF_ENTRY,
                 "DMC:EH_APE_CEN: StateApePwrOff() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: StateApePwrOff() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrNone);

    // Supported events in the APE_POWER_OFF-state
    switch (aEventType)
        {
        case KERNEL_EVENT_ISI:
            {
            TUint8 resourceId = aKernMsgPtr[ISI_HEADER_OFFSET_RESOURCEID];

            OstTrace1(TRACE_FLOW, STATE_APE_PWR_OFF_CASE_KRN_EV_1,
                      "DMC:EH_APE_CEN: StateApePwrOff() - resource ID: 0x%x",
                      (TUint)resourceId);
            DMC_TRACE((("DMC:EH_APE_CEN: StateApePwrOff() - resource ID: 0x%x:"), resourceId));

#ifdef USE_MTC_SERVER
            if (resourceId == PN_MTC)
                {
                DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - PN_MTC"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MTC_STATE_INFO_IND:
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - MTC_STATE_INFO_IND"));

                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);

                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition
                        if ((modemState == MTC_POWER_OFF) && (modemAction == MTC_READY))
                            {
                            DmcEvHandApeCent::Lock();
                            iModemCurrentState = modemState;

                            SET_BIT(iModemActionBitMap, MODEM_READY);

                            DmcEvHandApeCent::Unlock();

                            DMC_TRACE((("DMC:EH_APE_CEN: StateApePwrOff() - MTC_POWER_OFF, MCE_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));

                            /* Modem has shut down properly and ready for power off.
                               Acknowledge Power Handler in case it has already called Power Down,
                               oherwise wait Power Down call. */
                            if (iPowerDownCalled)
                                {
                                DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - Complete Power Down" ));
                                
                                DmcEvHandApeCent::Lock();
                                /* Make sure that Power Handler is not acknowledged twice. */
                                iPowerDownCalled = FALSE;
                                DmcEvHandApeCent::Unlock();

                                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                                }
                            }
                        }
                        break;

                    case MTC_POWER_OFF_RESP: // MCE_POWER_OFF_RESP
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - MTC_POWER_OFF_RESP"));

                        MceIsi::McePowerOffResp(aKernMsgPtr);
                        }
                        break;

                    default:
                        {
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateApePwrOff - \
                                    MCE ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#else // USE_MTC_SERVER
            if (resourceId == PN_MODEM_MCE)
                {
                OstTrace0(TRACE_FLOW, STATE_APE_PWR_OFF_2,
                          "DMC:EH_APE_CEN: StateApePwrOff() - PN_MODEM_MCE");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - PN_MODEM_MCE"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MCE_MODEM_STATE_IND:
                        {
                        OstTrace0(TRACE_FLOW, STATE_APE_PWR_OFF_CASE_1,
                                      "DMC:EH_APE_CEN: StateApePwrOff() - MCE_MODEM_STATE_IND");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - MCE_MODEM_STATE_IND"));

                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);

                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition
                        if ((modemState == MCE_POWER_OFF) && (modemAction == MCE_READY))
                            {
                            DmcEvHandApeCent::Lock();
                            iModemCurrentState = modemState;

                            SET_BIT(iModemActionBitMap, MODEM_READY);

                            DmcEvHandApeCent::Unlock();

                            OstTrace1(TRACE_FLOW, STATE_APE_PWR_OFF_CASE_2,
                                      "DMC:EH_APE_CEN: StateApePwrOff() - MCE_POWER_OFF, MCE_READY, iModemCurrentState: 0x%x",
                                      (TUint)iModemCurrentState);
                            DMC_TRACE((("DMC:EH_APE_CEN: StateApePwrOff() - MCE_POWER_OFF, MCE_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));
                         
                            /* Modem has shut down properly and ready for power off.
                               Acknowledge Power Handler in case it has already called Power Down,
                               oherwise wait Power Down call. */
                            if (iPowerDownCalled)
                                {
                                OstTrace0(TRACE_FLOW, STATE_APE_PWR_OFF_CASE_3, "DMC:EH_APE_CEN: StateApePwrOff() - Complete Power Down");
                                DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - Complete Power Down" ));
                                
                                DmcEvHandApeCent::Lock();
                                /* Make sure that Power Handler is not acknowledged twice. */
                                iPowerDownCalled = FALSE;
                                DmcEvHandApeCent::Unlock();

                                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                                }
                            }
                        }
                        break;

                    case MCE_POWER_OFF_RESP:
                        {
                        OstTrace0(TRACE_FLOW, STATE_APE_PWR_OFF_CASE_5,
                                      "DMC:EH_APE_CEN: StateApePwrOff() - MCE_POWER_OFF_RESP");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - MCE_POWER_OFF_RESP"));

                        MceIsi::McePowerOffResp(aKernMsgPtr);
                        }
                        break;

                    default:
                        {
                        OstTrace1(TRACE_FLOW, STATE_APE_PWR_OFF_DEFAULT,
                                  "DMC:EH_APE_CEN: StateApePwrOff() - MCE ISI message not supported in this state, messageId: 0x%x",
                                  messageId);
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateApePwrOff - \
                                    MCE ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#endif // USE_MTC_SERVER
            }
            break;

        case KERNEL_EVENT_MODEM_CONNECTION_NOT_OK:
            {
            OstTrace1(TRACE_FLOW, STATE_APE_PWR_OFF_CASE_MODEM_NOK_1,
                      "DMC:EH_APE_CEN: StateApePwrOff() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                      iPowerDownCalled);
            DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                        iPowerDownCalled));

            SET_BIT(iModemActionBitMap, MODEM_CONNECTION_NOT_OK);

            /* A modem connection lost and Power handler has started to
               execute the power down, thus we can complete the power down. */
            if (iPowerDownCalled)
                {
                DmcEvHandApeCent::Lock();
                /* Make sure that Power Handler is not acknowledged twice. */
                iPowerDownCalled = FALSE;
                DmcEvHandApeCent::Unlock();

                OstTrace0(TRACE_FLOW, STATE_APE_PWR_OFF_CASE_MODEM_NOK_2,
                          "DMC:EH_APE_CEN: StateApePwrOff() - Modem power off ready, complete power down");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - Modem power off ready, complete power down"));
                
                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            }
            break;

        case KERNEL_EVENT_PWR_HANDLER:
            {
            OstTrace1(TRACE_FLOW, STATE_APE_PWR_KERNEL_EVENT_PWR_HANDLER_1,
                      "DMC:EH_APE_CEN: StateApePwrOff() - KERNEL_EVENT_PWR_HANDLER, iModemActionBitMap: 0x%x",
                      iModemActionBitMap);
            DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - KERNEL_EVENT_PWR_HANDLER, iModemActionBitMap: 0x%x",
                        iModemActionBitMap));

            // Complete the power down if the modem power off is ready.
            if (iModemActionBitMap & (MODEM_READY | MODEM_CONNECTION_NOT_OK))
                {
                OstTrace0(TRACE_FLOW, STATE_APE_PWR___PWR_HANDLER_2,
                          "DMC:EH_APE_CEN: StateApePwrOff() - Modem power off ready, complete power down");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - Modem power off ready, complete power down"));

                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            else
                {
                OstTrace0(TRACE_FLOW, STATE_APE_PWR___PWR_HANDLER_3,
                          "DMC:EH_APE_CEN: StateApePwrOff() - Power off Modem");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApePwrOff() - Power off Modem"));

                // Modem power off not ready yet, wait until it is.
                DmcEvHandApeCent::Lock();
                /* Set TRUE to indicate that Power Handler started shutdown sequence.
                   DMC needs this information to acknowledge to PowerHandler
                   in case the modem connection is lost.*/
                iPowerDownCalled = TRUE;
                DmcEvHandApeCent::Unlock();

                // Power OFF Modem
                MceIsi::McePowerOffReq();
                }
            }
            break;

        default:
            {
            /* Fatal error if the event type. */ 
            OstTrace1(TRACE_FLOW, STATE_APE_PWR_OFF_EVENT_TYPE_DEFAULT,
                      "DMC:EH_APE_CEN: StateApePwrOff() - Illegal event type, aEventType: %d",
                      aEventType);
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: StateApePwrOff() - Illegal event type",
                                           aEventType);
            }
            break;
        }    

    OstTrace1(TRACE_ENTRY_EXIT, STATE_APE_PWR_OFF_RETURN,
              "DMC:EH_APE_CEN: StateApePwrOff() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:EH_APE_CEN: StateApePwrOff() # OUT - ret: %d"), ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateApeReset
// State APE_RESET: Ape orginated reset request(PC or SSM Adaptation).
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::StateApeReset(const TUint8  aEventType,
                                     const TUint8* const aKernMsgPtr,
                                     TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, STATE_APE_RESET_ENTRY,
                 "DMC:EH_APE_CEN: StateApeReset() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: StateApeReset() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrNone);

    // Supported events in the APE_RESET-state
    switch (aEventType)
        {
        case KERNEL_EVENT_ISI:
            {
            TUint8 resourceId = aKernMsgPtr[ISI_HEADER_OFFSET_RESOURCEID];
    
            OstTrace1(TRACE_FLOW, STATE_APE_RESET_CASE_KRN_EV_1,
                      "DMC:EH_APE_CEN: StateApePwrOff() - resourceId: 0x%x",
                      (TUint)resourceId);
            DMC_TRACE((("DMC:EH_APE_CEN: StateApeReset() - resource ID: 0x%x: "), resourceId));

#ifdef USE_MTC_SERVER    
            if (resourceId == PN_MTC)
                {
                DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - PN_MTC"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MTC_STATE_INFO_IND:
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - MTC_STATE_INFO_IND"));
    
                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);
                        
                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition
                        if ((modemState == MTC_SW_RESET) && (modemAction == MTC_READY))
                            {
                            DmcEvHandApeCent::Lock();
                            iModemCurrentState = modemState;

                            SET_BIT(iModemActionBitMap, MODEM_READY);

                            /* Make sure that Power Handler is not acknowledged twice. */
                            iPowerDownCalled = FALSE;
                            DmcEvHandApeCent::Unlock();

                            DMC_TRACE((("DMC:EH_APE_CEN: StateApeReset() - MTC_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));

                            /* Modem has shut down properly and ready for power off.
                               Acknowledge Power Handler in case it has already called Power Down,
                               oherwise wait Power Down call. */
                            if (iPowerDownCalled)
                                {
                                DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - Complete Power Down" ));
                                
                                DmcEvHandApeCent::Lock();
                                /* Make sure that Power Handler is not acknowledged twice. */
                                iPowerDownCalled = FALSE;
                                DmcEvHandApeCent::Unlock();

                                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                                }
                            }
                        }
                        break;
            
                    case MTC_RESET_GENERATE_RESP:
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - MTC_RESET_GENERATE_RESP"));
            
                        MceIsi::MceResetResp(aKernMsgPtr);
                        }
                        break;
            
                    default:
                        {
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateApeReset - MTC ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#else // USE_MTC_SERVER
            if (resourceId == PN_MODEM_MCE)
                {
                OstTrace0(TRACE_FLOW, STATE_APE_RESET_2,
                          "DMC:EH_APE_CEN: StateApeReset() - PN_MODEM_MCE");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - PN_MODEM_MCE"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MCE_MODEM_STATE_IND:
                        {
                        OstTrace0(TRACE_FLOW, STATE_APE_RESET_CASE_1,
                                      "DMC:EH_APE_CEN: StateApeReset() - MCE_MODEM_STATE_IND");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - MCE_MODEM_STATE_IND"));
    
                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);
                        
                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition
                        if ((modemState == MCE_SW_RESET) && (modemAction == MCE_READY))
                            {
                            DmcEvHandApeCent::Lock();
                            iModemCurrentState = modemState;

                            SET_BIT(iModemActionBitMap, MODEM_READY);

                            /* Make sure that Power Handler is not acknowledged twice. */
                            iPowerDownCalled = FALSE;
                            DmcEvHandApeCent::Unlock();
                    
                            OstTrace1(TRACE_FLOW, STATE_APE_RESET_CASE_2,
                                      "DMC:EH_APE_CEN: StateApeReset() - MCE_READY, iModemCurrentState: 0x%x",
                                      (TUint)iModemCurrentState);
                            DMC_TRACE((("DMC:EH_APE_CEN: StateApeReset() - MCE_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));

                            /* Modem has shut down properly and ready for power off.
                               Acknowledge Power Handler in case it has already called Power Down,
                               oherwise wait Power Down call. */
                            if (iPowerDownCalled)
                                {
                                OstTrace0(TRACE_FLOW, STATE_APE_RESET_CASE_3, "DMC:EH_APE_CEN: StateApeReset() - Complete Power Down");
                                DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - Complete Power Down" ));
                                
                                DmcEvHandApeCent::Lock();
                                /* Make sure that Power Handler is not acknowledged twice. */
                                iPowerDownCalled = FALSE;
                                DmcEvHandApeCent::Unlock();

                                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                                }
                            }
                        }
                        break;
            
                    case MCE_RESET_RESP:
                        {
                        OstTrace0(TRACE_FLOW, STATE_APE_RESET_CASE_5,
                                  "DMC:EH_APE_CEN: StateApeReset() - MCE_RESET_RESP");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - MCE_RESET_RESP"));

                        MceIsi::MceResetResp(aKernMsgPtr);
                        }
                        break;

                    default:
                        {
                        OstTrace1(TRACE_FLOW, STATE_APE_RESET_DEFAULT,
                                  "DMC:EH_APE_CEN: StateApeReset() - MCE ISI message not supported in this state, messageId: 0x%x",
                                  messageId);
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateApeReset - MCE ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#endif // USE_MTC_SERVER
            }
            break;

        case KERNEL_EVENT_MODEM_CONNECTION_NOT_OK:
            {
            OstTrace1(TRACE_FLOW, STATE_APE_RESET_CASE_MODEM_NOK_1,
                      "DMC:EH_APE_CEN: StateApeReset() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                      iPowerDownCalled);
            DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                        iPowerDownCalled));

            SET_BIT(iModemActionBitMap, MODEM_CONNECTION_NOT_OK);

            /* A modem connection lost and Power handler has started to
               execute the power down, thus we can complete the power down. */

            if (iPowerDownCalled)
                {
                DmcEvHandApeCent::Lock();
                /* Make sure that Power Handler is not acknowledged twice. */
                iPowerDownCalled = FALSE;
                DmcEvHandApeCent::Unlock();

                OstTrace0(TRACE_FLOW, STATE_APE_RESET_CASE_MODEM_NOK_2,
                          "DMC:EH_APE_CEN: StateApeReset() - Modem power off ready, complete power down");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - Modem power off ready, complete power down"));

                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            }
            break;

        case KERNEL_EVENT_PWR_HANDLER:
            {
            OstTrace1(TRACE_FLOW, STATE_APE_RESET_PWR_HANDLER_1,
                      "DMC:EH_APE_CEN: StateApeReset() - KERNEL_EVENT_PWR_HANDLER, iPowerDownCalled: %d",
                      iPowerDownCalled);
            DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - KERNEL_EVENT_PWR_HANDLER, iPowerDownCalled: %d",
                        iPowerDownCalled));

            // Complete the power down if the modem power off is ready.
            if (iModemActionBitMap & (MODEM_READY | MODEM_CONNECTION_NOT_OK))
                {
                OstTrace0(TRACE_FLOW, STATE_APE_RESET__PWR_HANDLER_2,
                          "DMC:EH_APE_CEN: StateApeReset() - Modem power off ready, complete power down");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - Modem power off ready, complete power down"));

                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            else
                {
                OstTrace0(TRACE_FLOW, STATE_APE_RESET__PWR_HANDLER_3,
                          "DMC:EH_APE_CEN: StateApeReset() - Power off Modem");
                DMC_TRACE(("DMC:EH_APE_CEN: StateApeReset() - Power off Modem"));

                // Modem power off not ready yet, wait until it is.
                DmcEvHandApeCent::Lock();
                /* Set TRUE to indicate that Power Handler started shutdown sequence.
                   DMC needs this information to acknowledge to PowerHandler
                   in case the modem connection is lost.*/
                iPowerDownCalled = TRUE;
                DmcEvHandApeCent::Unlock();

                // Reset Modem
                 MceIsi::MceResetReq();
                }
            }
            break;

        default:
            {
            /* Fatal error if the event type. */ 
            OstTrace1(TRACE_FLOW, STATE_RESET__EVENT_TYPE_CASE_DEFAULT,
                      "DMC:EH_APE_CEN: StateApeReset() - Illegal event type, aEventType: %d",
                      aEventType);
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: StateApeReset() - Illegal event type",
                                           aEventType);
            }
            break;
        }

    OstTrace1(TRACE_ENTRY_EXIT, STATE_RESET_RETURN,
              "DMC:EH_APE_CEN: StateApeReset() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:EH_APE_CEN: StateApeReset() # OUT - ret: %d"), ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateModemPwrOff
// State MODEM_PWR_OFF: Modem orginated power off.
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::StateModemPwrOff(const TUint8  aEventType,
                                        const TUint8* const aKernMsgPtr,
                                        TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, STATE_MODEM_PWR_OFF_ENTRY,
                 "DMC:EH_APE_CEN: StateModemPwrOff() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: StateModemPwrOff() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrNone);

    // Supported events in the MODEM_PWR_OFF-state
    switch (aEventType)
        {
        case KERNEL_EVENT_ISI:
            {
            TUint8 resourceId = aKernMsgPtr[ISI_HEADER_OFFSET_RESOURCEID];
    
            OstTrace1(TRACE_FLOW, STATE_MODEM_PWR_OFF_CASE_KRN_EV_1,
                      "DMC:EH_APE_CEN: StateModemPwrOff() - MCE_POWER_OFF, MCE_READY, resourceId: 0x%x",
                      (TUint)resourceId);
            DMC_TRACE((("DMC:EH_APE_CEN: StateModemPwrOff() - resource ID: 0x%x:"), resourceId));

#ifdef USE_MTC_SERVER    
            if (resourceId == PN_MTC)
                {
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - PN_MTC"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MTC_STATE_INFO_IND:
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - MTC_STATE_INFO_IND"));
    
                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);
                        
                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition
                        if ((modemState == MTC_POWER_OFF) && (modemAction == MTC_READY))
                            {
                            DmcEvHandApeCent::Lock();

                            iModemCurrentState = modemState;
                            SET_BIT(iModemActionBitMap, MODEM_READY);

                            DmcEvHandApeCent::Unlock();

                            DMC_TRACE((("DMC:EH_APE_CEN: StateModemPwrOff() - MTC_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));

                            /* Modem has shut down properly and ready for power off.
                               Acknowledge Power Handler in case it has already called Power Down,
                               oherwise wait Power Down call. */
                            if (iPowerDownCalled)
                                {
                                DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - Complete Power Down" ));
                                
                                DmcEvHandApeCent::Lock();
                                /* Make sure that Power Handler is not acknowledged twice. */
                                iPowerDownCalled = FALSE;
                                DmcEvHandApeCent::Unlock();

                                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                                }
                            }
                        }
                        break;
            
                    default:
                        {
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateModemPwrOff - MTC ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#else // USE_MTC_SERVER
            if (resourceId == PN_MODEM_MCE)
                {
                OstTrace0(TRACE_FLOW, STATE_MODEM_PWR_OFF_2,
                          "DMC:EH_APE_CEN: StateModemPwrOff() - PN_MODEM_MCE");
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - PN_MODEM_MCE"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MCE_MODEM_STATE_IND:
                        {
                        OstTrace0(TRACE_FLOW, STATE_MODEM_PWR_OFF_STATE_CASE_1,
                                      "DMC:EH_APE_CEN: StateModemPwrOff() - MCE_MODEM_STATE_IND");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - MCE_MODEM_STATE_IND"));

                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);
                        
                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition.
                        if ((modemState == MCE_POWER_OFF) && (modemAction == MCE_READY))
                            {
                            DmcEvHandApeCent::Lock();

                            iModemCurrentState = modemState;
                            SET_BIT(iModemActionBitMap, MODEM_READY);
                            /* Make sure that Power Handler is not acknowledged twice. */
                            iPowerDownCalled = FALSE;

                            DmcEvHandApeCent::Unlock();

                            OstTrace1(TRACE_FLOW, STATE_MODEM_PWR_OFF_CASE_CASE_2,
                                      "DMC:EH_APE_CEN: StateModemPwrOff() - MCE_READY, iModemCurrentState: 0x%x",
                                      (TUint)iModemCurrentState);
                            DMC_TRACE((("DMC:EH_APE_CEN: StateModemPwrOff() - MCE_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));

                            /* Modem has shut down properly and ready for reset
                               so inform the power handler that we are ready. */
                            DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                            }
                        }
                        break;

                    default:
                        {
                        OstTrace1(TRACE_FLOW, STATE_MODEM_PWR_OFF_CASE_DEFAULT,
                                  "DMC:EH_APE_CEN: StateModemPwrOff() - MCE ISI message not supported in this state, messageId: 0x%x",
                                  messageId);
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateModemPwrOff - MCE ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#endif // USE_MTC_SERVER
            }
            break;

        case KERNEL_EVENT_MODEM_CONNECTION_NOT_OK:
            {
            OstTrace1(TRACE_FLOW, STATE_MODEM_PWR_OFF_CASE_MODEM_NOK_1,
                      "DMC:EH_APE_CEN: StateModemPwrOff() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                      iPowerDownCalled);
            DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                        iPowerDownCalled));

            SET_BIT(iModemActionBitMap, MODEM_CONNECTION_NOT_OK);

            /* A modem connection lost and Power handler has started to
               execute the power down, thus we can complete the power down. */
            if (iPowerDownCalled)
                {
                OstTrace0(TRACE_FLOW, STATE_MODEM_PWR_OFF_CASE_MODEM_NOK_3,
                          "DMC:EH_APE_CEN: StateModemPwrOff() - Power off Modem");
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - Power off Modem"));

                DmcEvHandApeCent::Lock();
                /* Make sure that Power Handler is not acknowledged twice. */
                iPowerDownCalled = FALSE;
                DmcEvHandApeCent::Unlock();

                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            }
            break;

        case KERNEL_EVENT_PWR_HANDLER:
            {
            OstTrace1(TRACE_FLOW, STATE_MODEM_PWR_OFF__PWR_HANDLER_1,
                      "DMC:EH_APE_CEN: StateModemPwrOff() - KERNEL_EVENT_PWR_HANDLER, iModemActionBitMap: 0x%02x",
                      iModemActionBitMap);
            DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - KERNEL_EVENT_PWR_HANDLER, iModemActionBitMap: 0x%02x",
                        iModemActionBitMap));

            // Complete the power down if the modem power off is ready.
            if (iModemActionBitMap & (MODEM_READY | MODEM_CONNECTION_NOT_OK))
                {
                OstTrace0(TRACE_FLOW, STATE_MODEM_PWR_OFF__PWR_HANDLER_2,
                          "DMC:EH_APE_CEN: StateModemPwrOff() - Modem power off ready, complete power down");
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - Modem power off ready, complete power down"));

                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            else
                {
                OstTrace0(TRACE_FLOW, STATE_MODEM_PWR_OFF__PWR_HANDLER_3,
                          "DMC:EH_APE_CEN: StateModemPwrOff() - Wait modem ready or modem connection NOK");
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemPwrOff() - Wait modem ready or modem connection NOK"));

                // Modem power off not ready yet, wait until it is.
                DmcEvHandApeCent::Lock();
                /* Set TRUE to indicate that Power Handler started shutdown sequence.
                   DMC needs this information to acknowledge to PowerHandler
                   in case the modem connection is lost.*/
                iPowerDownCalled = TRUE;
                DmcEvHandApeCent::Unlock();
                }
            }
            break;

        default:
            {
            /* Fatal error if the event type. */ 
            OstTrace1(TRACE_FLOW, STATE_MODEM_PWR_OFF__EVENT_TYPE_CASE_DEFAULT,
                      "DMC:EH_APE_CEN: StateModemPwrOff() - Illegal event type, aEventType: %d",
                      aEventType);
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: StateModemPwrOff() - Illegal event type",
                                           aEventType);
            }
            break;
        }

    OstTrace1(TRACE_ENTRY_EXIT, STATE_MODEM_PWR_OFF_RETURN,
              "DMC:EH_APE_CEN: StateModemPwrOff() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:EH_APE_CEN: StateModemPwrOff() # OUT - ret: %d"), ret));

    return ret;    
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateModemControlledReset
// State MODEM_CONTROLLED_RESET: Modem orginated reset.
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::StateModemControlledReset(const TUint8  aEventType,
                                                 const TUint8* const aKernMsgPtr,
                                                 TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, STATE_MODEM_CONT_RESET_ENTRY,
                 "DMC:EH_APE_CEN: StateModemControlledReset() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: StateModemControlledReset() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrNone);

    // Supported events in the MODEM_CONTROLLED_RESET-state
    switch (aEventType)
        {
        case KERNEL_EVENT_ISI:
            {
            TUint8 resourceId = aKernMsgPtr[ISI_HEADER_OFFSET_RESOURCEID];
    
            OstTrace1(TRACE_FLOW, STATE_MODEM_CONT_RESET_CASE_KRN_EV_1,
                      "DMC:EH_APE_CEN: StateModemControlledReset() - MCE_POWER_OFF, MCE_READY, resourceId: 0x%x",
                      (TUint)resourceId);
            DMC_TRACE((("DMC:EH_APE_CEN: StateModemControlledReset() - resource ID: 0x%x:"), resourceId));

#ifdef USE_MTC_SERVER    
            if (resourceId == PN_MTC)
                {
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemControlledReset() - PN_MTC"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MTC_STATE_INFO_IND:
                        {
                        DMC_TRACE(("DMC:EH_APE_CEN: StateModemControlledReset() - MTC_STATE_INFO_IND"));

                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);

                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition
                        if ((modemState == MTC_SW_RESET) && (modemAction == MTC_READY))
                            {
                            DmcEvHandApeCent::Lock();
                            iModemCurrentState = modemState;

                            SET_BIT(iModemActionBitMap, MODEM_READY);

                            /* Make sure that Power Handler is not acknowledged twice. */
                            iPowerDownCalled = FALSE;
                            DmcEvHandApeCent::Unlock();

                            DMC_TRACE((("DMC:EH_APE_CEN: StateModemControlledReset() - MTC_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));

                            /* Modem has shut down properly and ready to power off,
                               so inform the power handler that we are ready. */
                            DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                            }
                        }
                        break;
            
                    default:
                        {
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateModemControlledReset - MTC ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#else // USE_MTC_SERVER
            if (resourceId == PN_MODEM_MCE)
                {
                OstTrace0(TRACE_FLOW, STATE_MODEM_CONT_RESET_2,
                          "DMC:EH_APE_CEN: StateModemControlledReset() - PN_MODEM_MCE");
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemControlledReset() - PN_MODEM_MCE"));

                TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];

                switch (messageId)
                    {
                    case MCE_MODEM_STATE_IND:
                        {
                        OstTrace0(TRACE_FLOW, STATE_MODEM_CONT_RESET_CASE_1,
                                      "DMC:EH_APE_CEN: StateModemControlledReset() - MCE_MODEM_STATE_IND");
                        DMC_TRACE(("DMC:EH_APE_CEN: StateModemControlledReset() - MCE_MODEM_STATE_IND"));

                        TUint8 modemState(MCE_STATE_INIT_VALUE);
                        TUint8 modemAction(MCE_ACTION_INIT_VALUE);
                        
                        // Get the modem state and action information
                        MceIsi::MceModemStateInd(aKernMsgPtr, modemState, modemAction);

                        // Update the modem state variable if the modem has finished a state transition.
                        if ((modemState == MCE_SW_RESET) && (modemAction == MCE_READY))
                            {
                            DmcEvHandApeCent::Lock();
                            iModemCurrentState = modemState;

                            /* Set Modem ready so we can complete the power down immediately when Power Handler
                               makes the Power Down Call. */
                            SET_BIT(iModemActionBitMap, MODEM_READY);

                            /* Make sure that Power Handler is not acknowledged twice. */
                            iPowerDownCalled = FALSE;
                            DmcEvHandApeCent::Unlock();

                            OstTrace1(TRACE_FLOW, STATE_MODEM_CONT_RESET_CASE_CASE_2,
                                      "DMC:EH_APE_CEN: StateModemControlledReset() - MCE_READY, iModemCurrentState: 0x%x",
                                      (TUint)iModemCurrentState);
                            DMC_TRACE((("DMC:EH_APE_CEN: StateModemControlledReset() - MCE_READY, iModemCurrentState: 0x%x"), 
                                         iModemCurrentState));

                            /* Modem has shut down properly and ready for reset
                               so inform the power handler that we are ready. */
                            DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                            }
                        }
                        break;

                    default:
                        {
                        OstTrace1(TRACE_FLOW, STATE_MODEM_CONT_RESET_CASE_DEFAULT,
                                  "DMC:EH_APE_CEN: StateModemControlledReset() - MCE ISI message not supported in this state, messageId: 0x%x",
                                  messageId);
                        DMC_TRACE((("MCE_ISIMSG:DMC:EH_APE_CEN: StateModemControlledReset - MCE ISI message not supported in this state, messageId: 0x%x"),
                                    messageId));
                        }
                        break;
                    }
                }
#endif // USE_MTC_SERVER
            }
            break;

        case KERNEL_EVENT_MODEM_CONNECTION_NOT_OK:
            {
            OstTrace1(TRACE_FLOW, STATE_MODEM_CONT_RESET_CASE_MODEM_NOK_1,
                      "DMC:EH_APE_CEN: StateModemControlledReset() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                      iPowerDownCalled);
            DMC_TRACE(("DMC:EH_APE_CEN: StateModemControlledReset() - KERNEL_EVENT_MODEM_CONNECTION_NOT_OK, iPowerDownCalled: %d",
                        iPowerDownCalled));

            SET_BIT(iModemActionBitMap, MODEM_CONNECTION_NOT_OK);

            /* A modem connection lost and Power handler has started to
               execute the power down, thus we can complete the power down. */
            if (iPowerDownCalled)
                {
                DmcEvHandApeCent::Lock();
                /* Make sure that Power Handler is not acknowledged twice. */
                iPowerDownCalled = FALSE;
                DmcEvHandApeCent::Unlock();

                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            }
            break;

        case KERNEL_EVENT_PWR_HANDLER:
            {
            OstTrace1(TRACE_FLOW, STATE_MODEM_CONT_RESET__PWR_HANDLER_1,
                      "DMC:EH_APE_CEN: StateModemControlledReset() - KERNEL_EVENT_PWR_HANDLER, iModemActionBitMap: 0x%02x",
                      iModemActionBitMap);
            DMC_TRACE(("DMC:EH_APE_CEN: StateModemControlledReset() - KERNEL_EVENT_PWR_HANDLER, iModemActionBitMap: 0x%02x",
                        iModemActionBitMap));

            // Complete the power down if the modem power off is ready or the communication is not possible.
            if (iModemActionBitMap & (MODEM_READY | MODEM_CONNECTION_NOT_OK))
                {
                OstTrace0(TRACE_FLOW, STATE_MODEM_CONT_RESET__PWR_HANDLER_2,
                          "DMC:EH_APE_CEN: StateModemControlledReset() - Modem power off ready, complete power down");
                DMC_TRACE(("DMC:EH_APE_CEN: StateModemControlledReset() - Modem power off ready, complete power down"));

                DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
                }
            else
                {
                // Modem power off not ready yet, wait until it is.
                DmcEvHandApeCent::Lock();
                /* Set TRUE to indicate that Power Handler started shutdown sequence.
                   DMC needs this information to acknowledge to PowerHandler
                   in case the modem connection is lost.*/
                iPowerDownCalled = TRUE;
                DmcEvHandApeCent::Unlock();
                }
            }
            break;

        default:
            {
            /* Fatal error. */ 
            OstTrace1(TRACE_FLOW, STATE_MODEM_CONT_RESET__EVENT_TYPE_CASE_DEFAULT,
                      "DMC:EH_APE_CEN: StateModemControlledReset() - Illegal event type, aEventType: %d",
                      aEventType);
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: StateModemControlledReset() - Illegal event type",
                                           aEventType);
            }
            break;
        }

    OstTrace1(TRACE_ENTRY_EXIT, STATE_MODEM_CONT_RESET_PWR_OFF_RETURN,
              "DMC:EH_APE_CEN: StateModemControlledReset() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:EH_APE_CEN: StateModemControlledReset() # OUT - ret: %d"), ret));

    return ret;    
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateModemUncontrolledReset
// State MODEM_UNCONTROLLED_RESET: Modem uncontrolled reset(channel lost)
// -----------------------------------------------------------------------------
//
TInt DmcEvHandApeCent::StateModemUncontrolledReset(const TUint8  aEventType,
                                                   const TUint8* const aKernMsgPtr,
                                                   TUint* const  aUsrMsgPtr)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, STATE_MODEM_UNCONT_RESET_ENTRY,
                 "DMC:EH_APE_CEN: StateModemUncontrolledReset() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x",
                 aEventType, (TUint)aKernMsgPtr, (TUint)aUsrMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: StateModemUncontrolledReset() # IN - aEventType: 0x%x, aKernMsgPtr: 0x%x, aUsrMsgPtr: 0x%x"),
                 aEventType, aKernMsgPtr, aUsrMsgPtr));

    TInt ret(KErrNone);

    // Supported events in the MODEM_CONTROLLED_RESET-state.
    switch (aEventType)
        {
        case KERNEL_EVENT_PWR_HANDLER:
            {
            OstTrace0(TRACE_FLOW, STATE_MODEM_UNCONT_RESET__PWR_HANDLER_1,
                      "DMC:EH_APE_CEN: StateModemUncontrolledReset() - KERNEL_EVENT_PWR_HANDLER");
            DMC_TRACE(("DMC:EH_APE_CEN: StateModemUncontrolledReset() - KERNEL_EVENT_PWR_HANDLER"));

             // Modem power off not ready yet, wait until it is.
            DmcEvHandApeCent::Lock();
            iPowerDownCalled = TRUE;
            DmcEvHandApeCent::Unlock();

            OstTrace0(TRACE_FLOW, STATE_MODEM_UNCONT_RESET__PWR_HANDLER_2,
                      "DMC:EH_APE_CEN: StateModemUncontrolledReset() - Complete power down");
            DMC_TRACE(("DMC:EH_APE_CEN: StateModemUncontrolledReset() - Complete power down"));

             DmcEvHandApeCent::iDmcExtPtr->CompletePowerDown();
            }
            break;

        default:
            {
            /* Do not assert since there might buffered ISI-messages and can be received.
               Just ignore and wait Power Handler Power Down call. */
            OstTrace1(TRACE_FLOW, STATE_MODEM_UNCONT_RESET__EVENT_TYPE_CASE_DEFAULT,
                      "DMC:EH_APE_CEN: StateModemUncontrolledReset() - Event type not handled in MODEM_CONTROLLED_RESET-state, aEventType: %d",
                      aEventType);
            DMC_TRACE((("DMC:EH_APE_CEN: StateModemUncontrolledReset() - Event type not handled in MODEM_CONTROLLED_RESET-state, aEventType: %d"),
                        aEventType));
            }
            break;
        }

    OstTrace1(TRACE_ENTRY_EXIT, STATE_MODEM_UNCONT_RESET_PWR_OFF_RETURN,
              "DMC:EH_APE_CEN: StateModemUncontrolledReset() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:EH_APE_CEN: StateModemUncontrolledReset() # OUT - ret: %d"), ret));


    return ret;    
    }


// -----------------------------------------------------------------------------
// DmcEvHandApeCent::HandleMceModemStateQueryResp
// This function decodes the MCE_MODEM_STATE_QUERY_RESP-message.
// -----------------------------------------------------------------------------
//
void DmcEvHandApeCent::HandleMceModemStateQueryResp(const TUint8* const aMsgPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_CENT_HANDLE_MCE_STATE_QUERY_RESP_ENTRY,
                 "DMC:EH_APE_CEN: HandleMceModemStateQueryResp() # IN - aMsgPtr: 0x%x",
                 (TUint)aMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: HandleMceModemStateQueryResp() # IN - aMsgPtr: 0x%x"), aMsgPtr));

    TUint8 modemCurrentState;

    // Handle the response message
    if (MceIsi::MceModemStateQueryResp(aMsgPtr, modemCurrentState) == KErrNone)
        {
        // Update the modem state
        DmcEvHandApeCent::Lock();
        DmcEvHandApeCent::iModemCurrentState = modemCurrentState;
        DmcEvHandApeCent::Unlock();
        
        OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_HANDLE_MCE_STATE_QUERY_RESP_1,
                  "DMC:EH_APE_CEN: HandleMceModemStateQueryResp() - iModemCurrentState: 0x%02x",
                  DmcEvHandApeCent::iModemCurrentState);
        DMC_TRACE((("DMC:EH_APE_CEN: HandleMceModemStateQueryResp() - iModemCurrentState: 0x%02x"), 
                     DmcEvHandApeCent::iModemCurrentState));
        }
    else
        {
        // What to do now. Until better knowledge we just trace.
        OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_HANDLE_MCE_STATE_QUERY_RESP_2,
                  "DMC:EH_APE_CEN: HandleMceModemStateQueryResp() - modem MCE state transition not ready");
        DMC_TRACE(("DMC:EH_APE_CEN: HandleMceModemStateQueryResp() - modem MCE state transition not ready"));
        }

    OstTrace0(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_CENT_HANDLE_MCE_STATE_QUERY_RESP_RETURN,
              "DMC:EH_APE_CEN: HandleMceModemStateInd() # OUT");
    DMC_TRACE(("DMC:EH_APE_CEN: HandleMceModemStateInd() # OUT"));
    }


// -----------------------------------------------------------------------------
// DmcEvHandApeCent::SetNextState
// Handles a state transition of the DMC Event Handler.
// -----------------------------------------------------------------------------
//
void DmcEvHandApeCent::SetNextState(const TUint8 aNextState)
    {
    OstTraceExt2(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_APE_CENT_SET_NEXT_STATE_ENTRY,
                 "DMC:EH_APE_CEN: SetNextState() # IN - iCurrentState: 0x%02x, aNextState: 0x%02x ",
                  iCurrentState, (TUint)aNextState);
    DMC_TRACE((("DMC:EH_APE_CEN: SetNextState() # IN - iCurrentState: 0x%02x, aNextState: 0x%02x"),
                 iCurrentState, aNextState));

    if ((iStateTransitionMap[iCurrentState] >> aNextState) & 1)
        {
        DmcEvHandApeCent::Lock();
        iCurrentState = aNextState;
        DmcEvHandApeCent::Unlock();

        OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_SET_NEXT_STATE_1,
                  "DMC:EH_APE_CEN: SetNextState()- State transition allowed");
        DMC_TRACE(("DMC:EH_APE_CEN: SetNextState() - State transition allowed"));

        }
    else
        {
        OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_HANDLE_EVENT_IDLE_2,
                  "DMC:EH_APE_CEN: HandleEvent() - Requested state transition not allowed");
                  DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: SetNextState() - Requested state transition not allowed",
                                                 KErrArgument);
        }

    OstTrace1(TRACE_FLOW, DMC_EVENT_HAND_APE_CENT_SET_NEXT_STATE_RETURN,
              "DMC:EH_APE_CEN: SetNextState() # OUT - new iCurrentState: 0x%x",
              iCurrentState);
    DMC_TRACE((("DMC:EH_APE_CEN: SetNextState() # OUT - new iCurrentState: 0x%02x"),
                 iCurrentState));
    }


// -----------------------------------------------------------------------------
// DmcEvHandApeCent::IsAnyStateEvent
// Checks if the requested action is part of the any state.
// -----------------------------------------------------------------------------
//
TUint8 DmcEvHandApeCent::IsAnyStateEvent(const TUint8 aEventType, const TUint8* const aKernMsgPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_IS_ANY_STATE_ENTRY,
              "DMC:EH_APE_CEN: IsAnyStateEvent() # IN - aEventType: 0x%x",
              aEventType);
    DMC_TRACE((("DMC:EH_APE_CEN: IsAnyStateEvent() # IN - aType: 0x%x"), aEventType));

    TUint8 bit(0);
    
    if (aEventType > KERNEL_LAST_KERNEL_EVENT)
        {
        // User any state event checking
        OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_IS_ANY_STATE_1,
                  "DMC:EH_APE_CEN: HandleEvent() - User event");
        DMC_TRACE((("DMC:EH_APE_CEN: User event")));
        bit = ((iAnyStateMap >> aEventType) & 1);
        }
     // Only the listed ISI-messages are part of the any state.
    else if (aEventType == KERNEL_EVENT_ISI)
        {
        // Kernel any state event checking
        OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_IS_ANY_STATE_2,
                  "DMC:EH_APE_CEN: HandleEvent() - Kernel event");
        DMC_TRACE((("DMC:EH_APE_CEN: Kernel event")));

        /* Check msg ID, which are supported by any state.
           A message pointer is already checked in the associated Dfc queue. */        
        TUint8 resourceId = aKernMsgPtr[ISI_HEADER_OFFSET_RESOURCEID];

        DMC_TRACE((("DMC:EH_APE_CEN: IsAnyStateEvent() - resource ID: 0x%x: "), resourceId));

#ifdef USE_MTC_SERVER
        if (resourceId == PN_MTC) // PN_MTC
            {
            DMC_TRACE(("DMC:EH_APE_CEN: IsAnyStateEvent() - PN_MTC"));

            TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];
        
            switch (messageId)
                {
                case MTC_STATE_QUERY_RESP:
                    {
                    DMC_TRACE(("DMC:EH_APE_CEN: IsAnyStateEvent() - MTC_STATE_QUERY_RESP"));

                    bit = 1;
                    }
                    break;
                
                default: // nothing to do
                    break;
                }
            }
#else // USE_MTC_SERVER
        if (resourceId == PN_MODEM_MCE)
            {
            OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_IS_ANY_STATE_3,
                      "DMC:EH_APE_CEN: IsAnyStateEvent() - PN_MODEM_MCE");
            DMC_TRACE(("DMC:EH_APE_CEN: IsAnyStateEvent() - PN_MODEM_MCE"));

            TUint8 messageId = aKernMsgPtr[ISI_HEADER_OFFSET_MESSAGEID];
        
            switch (messageId)
                {
                case MCE_MODEM_STATE_QUERY_RESP:
                    {
                    OstTrace0(TRACE_FLOW, DMC_EVENT_HAND_IS_ANY_STATE_4,
                              "DMC:EH_APE_CEN: IsAnyStateEvent() - MCE_STATE_QUERY_RESP");
                    DMC_TRACE(("DMC:EH_APE_CEN: IsAnyStateEvent() - MCE_STATE_QUERY_RESP"));

                    bit = 1;
                    }
                    break;
                
                default: // nothing to do
                    break;
                }
            }
#endif // USE_MTC_SERVER
        }

    OstTrace1(TRACE_ENTRY_EXIT, DMC_EVENT_HAND_IS_ANY_STATE_RETURN,
              "DMC:EH_APE_CEN: IsAnyStateEvent() # OUT - bit: 0x%02x", bit);
    DMC_TRACE((("DMC:EH_APE_CEN: IsAnyStateEvent() # OUT - bit: 0x%02x"), bit));

    return bit;
    }

// End of File
